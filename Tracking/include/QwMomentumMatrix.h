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

#include <iostream>
#include <vector>

// ROOT headers
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TStopwatch.h>

//  KLUDGE:  Add this header file to force the compiler to build
//  the correct dependency structure.  The proper thing would
//  be to have the Makefile recurse through all classes that
//  any included class depends on; I have a partial solution
//  but it isn't ready to commit tonight (2009dec15; pking).
// Qweak headers
#include "QwMagneticField.h"

#if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)
# include <Math/Interpolator.h>
#else
# warning "The QwRayTracer look-up table momentum determination will not be built!"
#endif

#include "QwRayTracer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Qweak headers
#include "VQwBridgingMethod.h"

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

class QwTrajMatrix: public VQwBridgingMethod {

  public:

    /// \brief Default constructor
    QwTrajMatrix();
    /// \brief Destructor
    virtual ~QwTrajMatrix();

    void GenerateTrajMatrix();

  private:

}; // class QwTrajMatrix

#endif // QWTRAJMATRIX_H
