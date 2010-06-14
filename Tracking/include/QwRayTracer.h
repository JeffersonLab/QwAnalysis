/*! \file   QwRayTracer.h
 *
 *  \author Jie Pan <jpan@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *
 *  \date   Thu Nov 26 11:44:51 CST 2009
 *  \brief  Definition of the ray-tracing bridging method for R2/R3 partial tracks
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

#ifndef __QWRAYTRACER_H__
#define __QWRAYTRACER_H__

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

// Qweak headers
#include "VQwBridgingMethod.h"
#include "QwMagneticField.h"
#include "QwPartialTrack.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// scale factor of the magnetic field
#define BSCALE 1.04

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Forward declarations
class QwMagneticField;
class QwPartialTrackParameter;
class QwBridge;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class QwRayTracer: public VQwBridgingMethod {

  public:

    /// Default constructor
    QwRayTracer();
    /// Destructor
    virtual ~QwRayTracer();

    static const bool LoadMagneticFieldMap(const std::string filename);

    void GenerateLookUpTable();

    const int Bridge(const QwPartialTrack* front, const QwPartialTrack* back);

    int DoForcedBridging() {
        return -1;
    };

    /// \brief Integrate using the Runge-Kutta 4th order algorithm
    const bool IntegrateRK4(TVector3& r0, TVector3& v0, const double p0, double z_end, const double step);

    QwBridge* GetBridgingInfo();

    double GetMomentum() {
        return fMomentum;
    };

    TVector3 GetHitPosition() {
        return fHitPosition;
    };
    TVector3 GetHitDirection() {
        return fHitDirection;
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

    void GetBridgingResult(Double_t *buffer);

  private:

    /// Magnetic field (static)
    static QwMagneticField *fBfield;

    double fBdlx; /// x component of the field integral
    double fBdly; /// y component of the field integral
    double fBdlz; /// z component of the field integral

    double fMomentum;  /// electron momentum

    TVector3 fHitPosition, fHitDirection;

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

    Int_t fMatchFlag; // MatchFlag = -2 : cannot match
                      // MatchFlag = -1 : potential track cannot pass through the filter
                      // MatchFlag = 0; : matched with look-up table
                      // MatchFlag = 1; : matched by using shooting method
                      // MatchFlag = 2; : potential track is forced to match

}; // class QwRayTracer

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // __QWRAYTRACER_H__
