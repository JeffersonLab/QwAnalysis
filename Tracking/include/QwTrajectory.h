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

// ROOT headers
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRotation.h>

// Forward declarations
class QwMagneticField;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// degree = 180.0/3.1415926535897932
#define DEGREE 57.295779513

// scale factor of the magnetic field
#define BSCALE 1.04

// maximum iteration of shooting method
#define MAX_ITERATION 10

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

  // temporary class for store partial track parameter
  class QwPartialTrackParameter{

    public:
      float fPositionR;
      float fPositionPhi;
      float fDirectionTheta;
      float fDirectionPhi;

  };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class QwTrajectory {

 public:
    QwTrajectory();
    ~QwTrajectory();

    void LoadMagneticFieldMap();
    void SetMagneticField(QwMagneticField *bfield){ fBfield = bfield; };

    int LoadMomentumMatrix();

    int BridgeFrontBackPartialTrack(TVector3 startpoint, TVector3 startpointdirection,
                                    TVector3 endpoint, TVector3 endpointdirection);

    int Filter(TVector3 startpoint, TVector3 startpointdirection,
               TVector3 endpoint, TVector3 endpointdirection);

    int Shooting(TVector3 startpoint, TVector3 startpointdirection,
                 TVector3 endpoint, TVector3 endpointdirection);

    double EstimateInitialMomentum(TVector3 direction);

    TVector3 Integrate(double E0,
                       TVector3 startpoint,
                       TVector3 direction,
                       double step,
                       double z_endplane);

    double GetMomentum(){ return fMomentum; };
    double GetDirectionX(){ return fDirectionX[0]; };
    double GetDirectionY(){ return fDirectionY[0]; };
    double GetDirectionZ(){ return fDirectionZ[0]; };
    TVector3 GetFieldIntegral() { return TVector3(fBdlx,fBdly,fBdlz); };

 private:

    QwMagneticField *fBfield;

    double fPositionX[2], fPositionY[2], fPositionZ[2];     /// coordinates of position
    double fDirectionX[2], fDirectionY[2], fDirectionZ[2];  /// unit vector of momentum

    double fBdlx; /// x component of the field integral
    double fBdly; /// y component of the field integral
    double fBdlz; /// z component of the field integral

    double fMomentum;  /// electron momentum

    int *fTableIndex;   /// index of the look-up table
    std::vector <QwPartialTrackParameter> fBackTrackParameterTable;  /// look-up table

}; // class QwTrajectory

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif  // __QWTRAJECTORY_H__
