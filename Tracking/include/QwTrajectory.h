/*! \file   QwTrajectory.h
 *  \author J. Pan
 *  \date   Thu Nov 26 11:44:51 CST 2009
 *  \brief  Raytrace in magnetic field to bridging R2/R3 partial tracks.
 *
 *  \ingroup QwTracking
 *
 *   Integrating the equations of motion for electrons in the QTOR.
 *   The 4'th order Runge-Kutta method is used.
 *
 *   The Newton-Raphson method is used to solve for the momentum of the track.
 *
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef __QWTRAJECTORY_H__
#define __QWTRAJECTORY_H__

#include <iostream>
#include <vector>
//#include <boost/multi_array.hpp>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRotation.h>

#include "QwMagneticField.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// scale factor of the magnetic field
#define BSCALE 1.04

// maximum iteration of shooting method
#define MAX_ITERATION 10

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

  // temporary class for store partial track parameter
  class PartialTrackParameter{

    public:
      float position_r;
      float position_phi;
      float direction_theta;
      float direction_phi;

  };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class QwTrajectory {

 public:
    QwTrajectory();
    ~QwTrajectory();

    void LoadMagneticFieldMap();
    void SetMagneticField(QwMagneticField *bfield){ B_Field = bfield; };

    int LoadMomentumMatrix();

    int BridgeFrontBackPartialTrack(TVector3 startpoint,
                                    TVector3 startpointdirection,
                                    TVector3 endpoint,
                                    TVector3 endpointdirection);

    int Filter(TVector3 fStartPoint, TVector3 fStartDirection,
               TVector3 fEndPoint, TVector3 fEndDirection);

    int Shooting(TVector3 fStartPoint, TVector3 fStartDirection,
                 TVector3 fEndPoint, TVector3 fEndDirection);

    double EstimateInitialMomentum(TVector3 direction);

    TVector3 Integrate(double E0,
                       TVector3 startpoint,
                       TVector3 direction,
                       double step, 
                       double z_endplane);

    double GetMomentum(){ return momentum; };
    double GetDirectionX(){ return uvx[0]; };
    double GetDirectionY(){ return uvy[0]; };
    double GetDirectionZ(){ return uvz[0]; };
    TVector3 GetFieldIntegral() { return TVector3(bdlx,bdly,bdlz); };

 private:

    QwMagneticField *B_Field;

    double bdlx; /// x component of the field integral
    double bdly; /// y component of the field integral
    double bdlz; /// z component of the field integral

    double xx[2], yy[2], zz[2];     /// coordinates of position
    double uvx[2], uvy[2], uvz[2];  /// unit vector of momentum

    double momentum;  /// electron momentum

    int *table_index;   /// index of the look-up table
    std::vector <PartialTrackParameter> backtrackparametertable;  /// look-up table

}; // class QwTrajectory

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif  // __QWTRAJECTORY_H__
