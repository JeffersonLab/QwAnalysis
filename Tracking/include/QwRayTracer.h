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
 *   The Newton-Raphson method is used to solve for `the momentum of the track.
 *
 */


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
#include "QwPartialTrack.h"


// Forward declarations
class QwMagneticField;
class QwPartialTrackParameter;
class QwBridge;


class QwRayTracer: public VQwBridgingMethod {

  public:

    /// Default constructor
    QwRayTracer();
    /// Destructor
    virtual ~QwRayTracer();

    static bool LoadMagneticFieldMap();

    void GenerateLookUpTable();

    Int_t Bridge(const QwPartialTrack* front, const QwPartialTrack* back);

    Int_t DoForcedBridging() {
        return -1;
    };

    /// \brief Integrate using the Runge-Kutta 4th order algorithm
    bool IntegrateRK4(TVector3& r0, TVector3& v0, const Double_t p0, Double_t z_end, Double_t step);

    QwBridge* GetBridgingInfo();

    Double_t GetMomentum() {
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
    Double_t GetFieldIntegralX() {
        return fBdlx;
    };
    Double_t GetFieldIntegralY() {
        return fBdly;
    };
    Double_t GetFieldIntegralZ() {
        return fBdlz;
    };

    void PrintInfo();

    void GetBridgingResult(Double_t *buffer);

  private:

    /// Magnetic field (static)
    static QwMagneticField *fBfield;

    Double_t fBdlx; /// x component of the field integral
    Double_t fBdly; /// y component of the field integral
    Double_t fBdlz; /// z component of the field integral

    Double_t fMomentum;  /// electron momentum
    Double_t fScatteringAngle;

    TVector3 fStartPosition;
    TVector3 fHitPosition;
    TVector3 fHitDirection;

    Double_t fPositionROff;
    Double_t fPositionPhiOff;
    Double_t fDirectionThetaOff;
    Double_t fDirectionPhiOff;

    Double_t fPositionXOff;
    Double_t fPositionYOff;

    Double_t fDirectionXOff;
    Double_t fDirectionYOff;
    Double_t fDirectionZOff;

    Int_t fSimFlag;

    Int_t fMatchFlag; // MatchFlag = -2 : cannot match
                      // MatchFlag = -1 : potential track cannot pass through the filter
                      // MatchFlag = 0; : matched with look-up table
                      // MatchFlag = 1; : matched by using shooting method
                      // MatchFlag = 2; : potential track is forced to match

}; // class QwRayTracer


#endif // __QWRAYTRACER_H__
