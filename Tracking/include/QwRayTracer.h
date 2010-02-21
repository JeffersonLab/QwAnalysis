/*! \file   QwRayTracer.h
 *  \brief  Definition of the ray-tracing bridging method for R2/R3 partial tracks
 *
 *  \author Jie Pan
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \date   Thu Nov 26 11:44:51 CST 2009
 */

#ifndef QWRAYTRACER_H
#define QWRAYTRACER_H

// System headers
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

// Qweak headers
#include "VQwBridgingMethod.h"

#if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)
# include <Math/Interpolator.h>
#else
# warning "The QwRayTracer look-up table momentum determination will not be built!"
#endif

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

/** \class QwRayTracer
 *  \ingroup QwTracking
 *
 *   Integrating the equations of motion for electrons in the QTOR.
 *   The 4'th order Runge-Kutta method is used.
 *
 *   The Newton-Raphson method is used to solve for the momentum of the track.
 *
 */
class QwRayTracer: public VQwBridgingMethod {

  public:

    QwRayTracer();
    virtual ~QwRayTracer();

    void LoadMagneticFieldMap();
    int LoadMomentumMatrix();

    void GenerateLookUpTable();

    void SetStartAndEndPoints(TVector3 startposition, TVector3 startdirection,
                              TVector3 endposition, TVector3 enddirection);

    int BridgeFrontBackPartialTrack();

    int DoForcedBridging() {
        return -1;
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

    int ReadSimPartialTrack(const TString filename, int evtnum,
                            std::vector<TVector3> *startposition,
                            std::vector<TVector3> *startdirection,
                            std::vector<TVector3> *endposition,
                            std::vector<TVector3> *enddirection);
    void GetBridgingResult(Double_t *buffer);

  private:

    int Filter();

    int SearchTable();

    int Shooting();

    double EstimateInitialMomentum(TVector3 direction);

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

    // Region2 WirePlane1
    Int_t fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit;
    Int_t fRegion2_ChamberFront_WirePlane1_NbOfHits;
    std::vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX;
    std::vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY;
    std::vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ;
    std::vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX;
    std::vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY;
    std::vector <Float_t> fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ;

    // Region3 WirePlaneU
    Int_t fRegion3_ChamberFront_WirePlaneU_HasBeenHit;
    Int_t fRegion3_ChamberFront_WirePlaneU_NbOfHits;
    std::vector <Int_t> fRegion3_ChamberFront_WirePlaneU_ParticleType;
    std::vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalPositionX;
    std::vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalPositionY;
    std::vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ;
    std::vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX;
    std::vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY;
    std::vector <Float_t> fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ;

    Float_t fPrimary_OriginVertexKineticEnergy;
    Float_t fPrimary_PrimaryQ2;
    Float_t fPrimary_CrossSectionWeight;

    Int_t fMatchFlag; // MatchFlag = -2 : cannot match
                      // MatchFlag = -1 : potential track cannot pass through the filter
                      // MatchFlag = 0; : matched with look-up table
                      // MatchFlag = 1; : matched by using shooting method
                      // MatchFlag = 2; : potential track is forced to match

}; // class QwRayTracer

#endif // QWRAYTRACER_H
