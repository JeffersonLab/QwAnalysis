/*! \file   QwTrajMatrix.h
 *
 *  \author Jie Pan <jpan@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \date   2010-01-23
 *
 *  \brief  Generate the momentum matrix and define the searching method
 */

#ifndef QWTRAJMATRIX_H
#define QWTRAJMATRIX_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// System headers
#include <cstdlib>
#include <iostream>
#include <vector>

// ROOT headers
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TStopwatch.h>

// Qweak headers
#include "QwMagneticField.h"

// ROOT Math Interpolator headers
#if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)
# include <Math/Interpolator.h>
#else
# warning "The QwRayTracer look-up table momentum determination will not be built!"
#endif

#include "QwRayTracer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Qweak headers
#include "VQwBridgingMethod.h"
#include "QwUnits.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// degree = 180.0/3.1415926535897932
#define DEGREE 57.295779513

// scale factor of the magnetic field
#define BSCALE 1.04

// maximum iteration of shooting method
#define MAX_ITERATION 10

// lool-up table parameters, p in [MeV], r and z in [cm], theta and phi in [degree]
#define P_MAX          1180
#define P_MIN          980
#define DP             10
#define P_GRIDSIZE     ((P_MAX-P_MIN)/DP+1)

#define R_MAX          100
#define R_MIN          30
#define DR             1
#define R_GRIDSIZE     ((R_MAX-R_MIN)/DR+1)

#define PHI_MAX        360
#define PHI_MIN        0
#define DPHI           1
#define PHI_GRIDSIZE   ((PHI_MAX-PHI_MIN)/DPHI+1)

#define VERTEXZ_MAX    -630
#define VERTEXZ_MIN    -670
#define DZ             2
#define Z_GRIDSIZE     ((VERTEXZ_MAX-VERTEXZ_MIN)/DZ+1)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// temporary class for store partial track parameter
class QwPartialTrackParameter {

  public:
    float fPositionR;
    float fPositionPhi;
    float fDirectionTheta;
    float fDirectionPhi;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class QwTrajMatrix {

  public:

    /// \brief Default constructor
    QwTrajMatrix();
    /// \brief Destructor
    virtual ~QwTrajMatrix();

    const bool WriteTrajMatrix(const std::string filename);
    const bool ReadTrajMatrixFile(const std::string filename);

    const QwPartialTrackParameter& at(const unsigned int index) const {
      return fBackTrackParameterTable.at(index);
    };

  private:

    /// \brief Index based on cylindrical coordinates (integer arguments)
    const unsigned int Index(const unsigned int index_p, const unsigned int ind_r, const unsigned int ind_phi, const unsigned int ind_z, const bool range_check = false) const;
    /// \brief Index based on cylindrical coordinates (double arguments)
    const unsigned int Index(const double p, const double r, const double phi, const double z) const;

    /// Table with the track parameters
    std::vector<QwPartialTrackParameter> fBackTrackParameterTable;

}; // class QwTrajMatrix


/**
 * Calculate the index based on the cylindrical world coordinates
 * @param index_p Index in momentum
 * @param index_r Index in radial position
 * @param index_phi Index in azimuthal angle
 * @param index_z Index in longitudinal position
 * @param range_check Flag to check the ranges
 * @return Index in the vector
 */
inline const unsigned int QwTrajMatrix::Index(
	const unsigned int index_p,
	const unsigned int index_r,
	const unsigned int index_phi,
	const unsigned int index_z,
	const bool range_check) const
{
  return index_z
         + Z_GRIDSIZE * index_phi
         + Z_GRIDSIZE * PHI_GRIDSIZE * index_r
         + Z_GRIDSIZE * PHI_GRIDSIZE * R_GRIDSIZE * index_p;
}

inline const unsigned int QwTrajMatrix::Index(
	const double p,
	const double r,
	const double phi,
	const double z) const
{
  int index_p = ((int) (p + 0.5) - P_MIN) / DP;
  int index_r = ((int) (r + 0.5) - R_MIN) / DR;
  int index_phi = ((int) (phi + 0.5) - PHI_MIN) / DPHI;
  int index_z = ((int) (z + 0.5) - VERTEXZ_MIN) / DZ;
  return Index(index_p, index_r, index_phi, index_z, true);
}



#endif // QWTRAJMATRIX_H
