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
#include <Math/Interpolator.h>
#include <TStopwatch.h>

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
#define DP             20
#define P_GRIDSIZE     ((P_MAX-P_MIN)/DP+1)

#define R_MAX          100
#define R_MIN          30
#define DR             2
#define R_GRIDSIZE     ((R_MAX-R_MIN)/DR+1)

#define PHI_MAX        360
#define PHI_MIN        0
#define DPHI           2
#define PHI_GRIDSIZE   ((PHI_MAX-PHI_MIN)/DPHI+1)

#define VERTEXZ_MAX    -630
#define VERTEXZ_MIN    -670
#define DZ             4
#define Z_GRIDSIZE     ((VERTEXZ_MAX-VERTEXZ_MIN)/DZ+1)

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Forward declarations
class QwMagneticField;
class QwBridge;

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

class QwTrajectory {

public:
    QwTrajectory();
    ~QwTrajectory();

    void LoadMagneticFieldMap();
    int LoadMomentumMatrix();

    void GenerateLookUpTable();

    void SetStartAndEndPoints(TVector3 startpoint, TVector3 startpointdirection,
                              TVector3 endpoint, TVector3 endpointdirection);

    int BridgeFrontBackPartialTrack();

    int DoForcedBridging(TVector3 startpoint, TVector3 startpointdirection,
                         TVector3 endpoint, TVector3 endpointdirection) {
        return 0;
    };

    int Integrate(double e0, double step);

    QwBridge* GetBridgingInfo();

    double GetMomentum() {
        return fMomentum;
    };

    TVector3 GetHitLocation() {
        return fHitDirection;
    };
    double GetHitLocationX() {
        return fHitLocation.X();
    };
    double GetHitLocationY() {
        return fHitLocation.Y();
    };
    double GetHitLocationZ() {
        return fHitLocation.Z();
    };

    TVector3 GetHitDirection() {
        return fHitDirection;
    };
    double GetHitDirectionX() {
        return fHitDirection.X();
    };
    double GetHitDirectionY() {
        return fHitDirection.Y();
    };
    double GetHitDirectionZ() {
        return fHitDirection.Z();
    };

    TVector3 GetFieldIntegral() {
        return TVector3(fBdlx,fBdly,fBdlz);
    };
    double GetFieldIntegralX() {
        return fBdlx;
    };
    double GetFieldIntegralY() {
        return fBdly;
    };
    double GetFieldIntegralZ() {
        return fBdlz;
    };

    void PrintInfo();

private:

    int Filter();

    int SearchTable();

    int Shooting();

    double EstimateInitialMomentum(TVector3 direction);

    void ReadSimPartialTrack();

private:

    QwMagneticField *fBfield;

    double fBdlx; /// x component of the field integral
    double fBdly; /// y component of the field integral
    double fBdlz; /// z component of the field integral

    double fMomentum;  /// electron momentum

    std::vector <QwPartialTrackParameter> fBackTrackParameterTable;  /// look-up table

    TVector3 fStartPosition, fStartDirection;
    double fStartPositionR, fStartPositionPhi;
    double fStartDirectionTheta, fStartDirectionPhi;

    TVector3 fEndPosition, fEndDirection;
    double fEndPositionR, fEndPositionPhi;
    double fEndDirectionTheta, fEndDirectionPhi;

    TVector3 fHitLocation, fHitDirection;
    double fHitLocationR, fHitLocationPhi;
    double fHitDirectionTheta, fHitDirectionPhi;

    double fPositionROff;
    double fPositionPhiOff;
    double fDirectionThetaOff;
    double fDirectionPhiOff;

    double fPositionXOff;
    double fPositionYOff;

    double fDirectionXOff;
    double fDirectionYOff;
    double fDirectionZOff;

    int fSimFlag;

}; // class QwTrajectory

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif  // __QWTRAJECTORY_H__
