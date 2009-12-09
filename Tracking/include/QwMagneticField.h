//
//  date: Sun Nov 29 22:28:23 CST 2009
//
//  jpan: QTOR field map reading class.
//        Modified from the Qweak geant4 simulation code
//

#ifndef _QwMagneticField_h_
#define _QwMagneticField_h_

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>

#include <TVector3.h>
#include <TRotation.h>


/// Allowed interpolation methods
enum EQwInterpolationMethod {
  kTrilinearInterpolation,
  kDoubleBilinearInterpolation,
  kNearestNeighborInterpolation
};


/// Field map storage data type
typedef float field_t;

class QwMagneticField
{
  public:

  QwMagneticField();
  QwMagneticField(std::string filename) { ReadFieldMap(filename); };
  virtual ~QwMagneticField();

  /// \brief Set the interpolation method
  void SetInterpolationMethod(const EQwInterpolationMethod method)
    { fInterpolationMethod = method; };
  /// \brief Get the interpolation method
  const EQwInterpolationMethod GetInterpolationMethod() const
    { return fInterpolationMethod; };

  /// \brief Set the data reduction factor
  void SetDataReductionFactor(const Int_t datareductionfactor)
    { fDataReductionFactor = datareductionfactor; };
  /// \brief Get the data reduction factor
  const Int_t GetDataReductionFactor() const
    { return fDataReductionFactor; };

  /// \brief Set the field scaling factor
  void SetFieldScalingFactor(const Double_t fieldscalingfactor)
    { fFieldScalingFactor = fieldscalingfactor; };
  /// \brief Get the field scaling factor
  const Double_t GetFieldScalingFactor() const
    { return fFieldScalingFactor; };

  void GetFieldValue(const double point[3], double *field, double scalefactor) const {
    GetCartesianFieldValue(point, field, scalefactor);
  };
  void GetCartesianFieldValue(const double point[3], double *field, double scalefactor) const;
  void GetCylindricalFieldValue(const double point[3], double *field, double scalefactor) const;

  void GetFieldValueFromGridCell( const  int GridPoint_R,
				  const  int GridPoint_Phi,
				  const  int GridPoint_Z,
				  double *BFieldGridValue ) const;

  void InitializeGrid();
  void ReadFieldMap(std::string filename);


  void SetFieldMap_RMin      ( double Rmin      ) { rMinFromMap    = Rmin;      }
  void SetFieldMap_RMax      ( double Rmax      ) { rMaxFromMap    = Rmax;      }
  void SetFieldMap_RStepsize ( double Rstepsize ) { gridstepsize_r = Rstepsize; }

  void SetFieldMap_ZMin      ( double Zmin      ) { zMinFromMap    = Zmin;      }
  void SetFieldMap_ZMax      ( double Zmax      ) { zMaxFromMap    = Zmax;      }
  void SetFieldMap_ZStepsize ( double Zstepsize ) { gridstepsize_z = Zstepsize; }

  void SetFieldMap_PhiMin      ( double Phimin      ) { phiMinFromMap    = Phimin;      }
  void SetFieldMap_PhiMax      ( double Phimax      ) { phiMaxFromMap    = Phimax;      }
  void SetFieldMap_PhiStepsize ( double Phistepsize ) { gridstepsize_phi = Phistepsize; }

private:

  /// \name Calculate the interpolated field values using various methods
  // @{
  /// \brief Flag to select the interpolation method
  EQwInterpolationMethod fInterpolationMethod;
  /// \brief Trilinear interpolation
  void CalculateTrilinear(const double point[3], double *field) const;
  /// \brief Double bilinear interpolation
  void CalculateDoubleBilinear(const double point[3], double *field) const;
  /// \brief Nearest-neighbor interpolation
  void CalculateNearestNeighbor(const double point[3], double *field) const;
  // @}

  /// \brief Data reduction factor: value of 2 will take only every second grid entry
  Int_t fDataReductionFactor;

  int nGridPointsInR;
  int nGridPointsInPhi;
  int nGridPointsInZ;

  double rMinFromMap;
  double rMaxFromMap;

  double phiMinFromMap;
  double phiMaxFromMap;

  double zMinFromMap;
  double zMaxFromMap;

  double gridstepsize_r;
  double gridstepsize_phi;
  double gridstepsize_z;

  double fUnitBfield; // units of field map

  int fGridSize;

  /// \name Storage space for the table of field values
  // @{
  //std::vector< std::vector< std::vector< double > > > BFieldGridData_X;
  //std::vector< std::vector< std::vector< double > > > BFieldGridData_Y;
  //std::vector< std::vector< std::vector< double > > > BFieldGridData_Z;
  std::vector< field_t > BFieldGridData_X; ///< field values in X
  std::vector< field_t > BFieldGridData_Y; ///< field values in X
  std::vector< field_t > BFieldGridData_Z; ///< field values in Z
  std::vector< field_t > BFieldGridData_R; ///< field values in R
  std::vector< field_t > BFieldGridData_Phi; ///< field values in Phi
  // @}

  bool   invertX, invertY, invertZ;

  TVector3* BField_ANSYS;

  double fFieldScalingFactor;
  double fZoffset;

};

#endif
