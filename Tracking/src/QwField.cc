#include <iostream>

#include "QwField.h"


//! Create a field map object without rotation, offset, or correction
QwField::QwField(std::string fieldmap, double scalingfactor)
{
  // Debug level
  fDebug = true;

  // Set scaling factor
  fBFieldScalingFactor = scalingfactor;

  // No rotation or offset
  fFieldOffset.SetXYZ(0.0, 0.0, 0.0);
  fFieldRotation.SetToIdentity();

  // Initialize the grid
  InitializeGrid();

  // Read the field map into memory
  ReadFieldMap(fieldmap);
};

//! Initialize the grid to the (hardcoded) expected size
// TODO The grid extent and size should be included in the field map file
// or in a meta-data reference text file.
void QwField::InitializeGrid()
{
  // Direction r
  fGridMin[kR] =   2.0;
  fGridMax[kR] = 300.0;
  fGridStep[kR] =  2.0;
  fGridNSteps[kR] = (fGridMax[kR] - fGridMin[kR]) / fGridStep[kR];

  // Direction phi
  fGridMin[kPhi] =   0.0;
  fGridMax[kPhi] = 359.0;
  fGridStep[kPhi] =  1.0;
  fGridNSteps[kPhi] = (fGridMax[kPhi] - fGridMin[kPhi]) / fGridStep[kPhi];

  // Direction z
  fGridMin[kZ] = -250.0;
  fGridMax[kZ] =  250.0;
  fGridStep[kZ] =   2.0;
  fGridNSteps[kZ] = (fGridMax[kZ] - fGridMin[kZ]) / fGridStep[kZ];

  // Resize the grid
  unsigned int newsize = (fGridNSteps[kR] + 1)
                       * (fGridNSteps[kPhi] + 1)
                       * (fGridNSteps[kZ] + 1);
  if (fDebug) std::cout << "Starting with field map size " << newsize << std::endl;
  fGridSize = newsize;
  fGridValueX.resize(newsize);
  fGridValueY.resize(newsize);
  fGridValueZ.resize(newsize);
};

//! Read the field map from file in the object
void QwField::ReadFieldMap(std::string fieldmap)
{
  // Open the field map
  std::ifstream input(fieldmap.c_str(), std::ios_base::in);
  // Check for success
  if (input.good())
    fFieldMap = fieldmap;
  else {
    std::cerr << "[QwField] Error: Could not open field map file!" << std::endl;
    std::cerr << "File name: " << fieldmap << std::endl;
  }

  // Loop over all entries
  unsigned int entries = 0;
  while (input.good()) {

    // Progress bar
    if (entries % (fGridSize / 10) == 0)
      std::cout << 100 * entries / fGridSize << "%" << std::flush;
    if (entries % (fGridSize / 10) != 0 && entries % (fGridSize / 40) == 0)
      std::cout << "." << std::flush;

    // Read position in polar coordinates
    int pos[3];
    input >> pos[kR] >> pos[kZ] >> pos[kPhi];
    // Read field in cartesian coordinates
    double field[3];
    input >> field[kX] >> field[kY] >> field[kZ];

    // Determine index position: phi, r, z
    unsigned int index = GetIndex(pos);
    if (index >= fGridSize) {
      unsigned int oldsize = fGridSize;
      unsigned int newsize = index + fGridNSteps[kPhi] * fGridNSteps[kR] + 1;
      std::cout << "[QwField] Warning: Need to resize the field map"
                            << " from " << oldsize
                            << " to "   << newsize
                            << " at index " << index << std::endl;
      fGridSize = newsize;
      fGridValueX.resize(newsize);
      fGridValueY.resize(newsize);
      fGridValueZ.resize(newsize);
    }

    // Store values in field map
    fGridValueX.at(index) = field[kX];
    fGridValueY.at(index) = field[kY];
    fGridValueZ.at(index) = field[kZ];
    entries++;

  } // end of while over input file

  // End of progress bar
  std::cout << std::endl;

  // Check whether we got all the entries we expected
  if (fGridSize != entries) {
    // TODO Peiqing's field map has 66 entries missing!!!  I would prefer to just
    // throw out the entire map if there are bugs in it.
    std::cout << "[QwField] Warning: There are gaps in the field map file!" << std::endl;
  }

  // Debug output
  if (fDebug) std::cout << "Read " << entries << " field map values in array "
                        << "of size " << fGridValueX.size() << "." << std::endl;

  // Close the field map
  input.close();
};


//! Return the index in one direction of a grid point
const unsigned int QwField::GetIndex(const double pos, const int coord) const
{
  return (pos - fGridMin[coord]) / fGridStep[coord];
};

//! Return the combined index in all directions of a grid point
const unsigned int QwField::GetIndex(const TVector3 pos) const
{
  if (! IsValidPosition(pos)) return 0;

  int index[3];
  index[kR]   =  (pos.Pt() - fGridMin[kR])   / fGridStep[kR];
  index[kPhi] = (pos.Phi() - fGridMin[kPhi]) / fGridStep[kPhi];
  index[kZ]   =   (pos.Z() - fGridMin[kZ])   / fGridStep[kZ];
  return GetIndex(index);
};

//! Return the combined index in all directions from the single direction indices
const unsigned int QwField::GetIndex(const int iR, const int iPhi, const int iZ) const
{
  // Safety check on grid initialization
  unsigned int index = 0;
  if (fGridStep[kPhi] > 0 && fGridStep[kR] > 0 && fGridStep[kZ] > 0) {
    index = (iPhi - fGridMin[kPhi]) / fGridStep[kPhi]
          + (iR   - fGridMin[kR])   / fGridStep[kR] * fGridNSteps[kPhi]
          + (iZ   - fGridMin[kZ])   / fGridStep[kZ] * fGridNSteps[kPhi] * fGridNSteps[kR];
  }
  return index;
};

//! Return the combined index in all directions from the single direction indices
const unsigned int QwField::GetIndex(const int* i) const
{
  return GetIndex(i[kR], i[kPhi], i[kZ]);
};

//! Returns true if the position has a valid field interpolation
const bool QwField::IsValidPosition(const TVector3 pos) const
{
  // r is the transverse distance, not the magnitude of the position vector
  return ((fGridMin[kPhi] <= pos.Phi() && pos.Phi() <= fGridMax[kPhi])
       && (fGridMin[kR]   <= pos.Pt()  && pos.Pt()  <= fGridMax[kR])
       && (fGridMin[kZ]   <= pos.Z()   && pos.Z()   <= fGridMax[kZ]));
}

//! Determines the combined indices of the eight corners of the local unit cube
const std::vector<unsigned int> QwField::GetLocalCorners(TVector3 pos) const
{
  // Determine fractional indices
  int index[3];
  index[kR]   = static_cast<int> (floor((pos.Pt()  - fGridMin[kR])   / fGridStep[kR]));
  index[kPhi] = static_cast<int> (floor((pos.Phi() - fGridMin[kPhi]) / fGridStep[kPhi]));
  index[kZ]   = static_cast<int> (floor((pos.Z()   - fGridMin[kZ])   / fGridStep[kZ]));

  // Allocate memory for the corners
  std::vector<unsigned int> corners;

  // Determine the eight courners
  corners.push_back(GetIndex(index[kPhi], index[kR], index[kZ]));
  corners.push_back(GetIndex(index[kPhi]+1, index[kR], index[kZ]));
  corners.push_back(GetIndex(index[kPhi], index[kR]+1, index[kZ]));
  corners.push_back(GetIndex(index[kPhi], index[kR], index[kZ]+1));
  corners.push_back(GetIndex(index[kPhi]+1, index[kR], index[kZ]+1));
  corners.push_back(GetIndex(index[kPhi], index[kR]+1, index[kZ]+1));
  corners.push_back(GetIndex(index[kPhi]+1, index[kR]+1, index[kZ]));
  corners.push_back(GetIndex(index[kPhi]+1, index[kR]+1, index[kZ]+1));

  return corners;
}

//! Determine the coordinates in the local unit cube
const double* QwField::GetLocalCoordinates(TVector3 pos) const
{
  // Allocate memory for the coordinates
  double* coords = new double[3];

  // Determine fractional indices
  double integer_part;
  coords[kR]   = modf((pos.Perp() - fGridMin[kR]) / fGridStep[kR], &integer_part);
  coords[kPhi] = modf((pos.Phi() - fGridMin[kPhi]) / fGridStep[kPhi], &integer_part);
  coords[kZ]   = modf((pos.Z() - fGridMin[kZ]) / fGridStep[kZ], &integer_part);

  return coords;
}

//! Determine the interpolated field at the requested position
const TVector3 QwField::GetField(TVector3 position) const
{
  // Start with zero field value
  TVector3 field(0.0, 0.0, 0.0);

  // Mis-alignment transformations
  position += fFieldOffset;
  position *= fFieldRotation;

  // Check whether position is within the field map boundaries
  if (! IsValidPosition(position)) return field;

  // Trilinear interpolation in a unit cube:
  //
  //   Vxyz = V000 (1 - x) (1 - y) (1 - z) +
  //          V100 x (1 - y) (1 - z) +
  //          V010 (1 - x) y (1 - z) +
  //          V001 (1 - x) (1 - y) z +
  //          V101 x (1 - y) z +
  //          V011 (1 - x) y z +
  //          V110 x y (1 - z) +
  //          V111 x y z
  //

  // Determine the corner indices
  std::vector<unsigned int> corners = GetLocalCorners(position);
  // and the local scaled coordinates
  const double* r = GetLocalCoordinates(position);

  field.SetX(
      fGridValueX[corners.at(0)] * (1-r[kX]) * (1-r[kY]) * (1-r[kZ])
    + fGridValueX[corners.at(1)] * (r[kX]) * (1-r[kY]) * (1-r[kZ])
    + fGridValueX[corners.at(2)] * (1-r[kX]) * (r[kY]) * (1-r[kZ])
    + fGridValueX[corners.at(3)] * (1-r[kX]) * (1-r[kY]) * (r[kZ])
    + fGridValueX[corners.at(4)] * (r[kX]) * (1-r[kY]) * (r[kZ])
    + fGridValueX[corners.at(5)] * (1-r[kX]) * (r[kY]) * (r[kZ])
    + fGridValueX[corners.at(6)] * (r[kX]) * (r[kY]) * (1-r[kZ])
    + fGridValueX[corners.at(7)] * (r[kX]) * (r[kY]) * (r[kZ]) );

  field.SetY(
      fGridValueY[corners.at(0)] * (1-r[kX]) * (1-r[kY]) * (1-r[kZ])
    + fGridValueY[corners.at(1)] * (r[kX]) * (1-r[kY]) * (1-r[kZ])
    + fGridValueY[corners.at(2)] * (1-r[kX]) * (r[kY]) * (1-r[kZ])
    + fGridValueY[corners.at(3)] * (1-r[kX]) * (1-r[kY]) * (r[kZ])
    + fGridValueY[corners.at(4)] * (r[kX]) * (1-r[kY]) * (r[kZ])
    + fGridValueY[corners.at(5)] * (1-r[kX]) * (r[kY]) * (r[kZ])
    + fGridValueY[corners.at(6)] * (r[kX]) * (r[kY]) * (1-r[kZ])
    + fGridValueY[corners.at(7)] * (r[kX]) * (r[kY]) * (r[kZ]) );

  field.SetZ(
      fGridValueZ[corners.at(0)] * (1-r[kX]) * (1-r[kY]) * (1-r[kZ])
    + fGridValueZ[corners.at(1)] * (r[kX]) * (1-r[kY]) * (1-r[kZ])
    + fGridValueZ[corners.at(2)] * (1-r[kX]) * (r[kY]) * (1-r[kZ])
    + fGridValueZ[corners.at(3)] * (1-r[kX]) * (1-r[kY]) * (r[kZ])
    + fGridValueZ[corners.at(4)] * (r[kX]) * (1-r[kY]) * (r[kZ])
    + fGridValueZ[corners.at(5)] * (1-r[kX]) * (r[kY]) * (r[kZ])
    + fGridValueZ[corners.at(6)] * (r[kX]) * (r[kY]) * (1-r[kZ])
    + fGridValueZ[corners.at(7)] * (r[kX]) * (r[kY]) * (r[kZ]) );

  // Remove temporary local coordinates
  delete[] r;

  // Scale by magnetic field factor
  field *= fBFieldScalingFactor;

  return field;
};
