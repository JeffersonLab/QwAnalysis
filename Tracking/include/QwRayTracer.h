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
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TRandom3.h>
#include <TStopwatch.h>

// Qweak headers
#include "QwOptions.h"
#include "VQwBridgingMethod.h"
#include "QwPartialTrack.h"
#include "QwMagneticField.h"


class QwRayTracer: public VQwBridgingMethod {

  public:

    /// Default constructor
    QwRayTracer(QwOptions& options);
    /// Destructor
    virtual ~QwRayTracer();

    /// \brief Define command line and config file options
    static void DefineOptions(QwOptions& options);
    /// \brief Process command line and config file options
    void ProcessOptions(QwOptions& options);

    /// \brief Load the magnetic field based on config file options
    bool LoadMagneticFieldMap(QwOptions& options);

    /// Get magnetic field current
    double GetMagneticFieldCurrent() const {
      if (fBfield) return fBfield->GetActualCurrent();
      else return 0.0;
    }
    /// Set magnetic field current
    void SetMagneticFieldCurrent(const double current) {
      if (fBfield) fBfield->SetActualCurrent(current);
    }

    /// \brief Bridge from the front to back partial track
    const QwTrack* Bridge(const QwPartialTrack* front, const QwPartialTrack* back);

    /// \brief Runge-Kutta numerical integration by Butcher tableau
    int IntegrateRK(
        const TMatrixD& A,
        const TMatrixD& b,
        TVector3& r,
        TVector3& v,
        const double p,
        const double z,
        const double h,
        const double epsilon);

    /// \brief Runge-Kutta numerical integration
    int IntegrateRK(
        TVector3& r,
        TVector3& v,
        const double p,
        const double z,
        const int order,
        const double h);

  private:

    /// Magnetic field (static)
    static QwMagneticField *fBfield;

    /// Runge-Kutta method order
    double fIntegrationOrder;
    /// Runge-Kutta step size
    double fIntegrationStep;

    /// Newton's method step size in position
    double fPositionResolution;
    /// Newton's method step size in momentum
    double fMomentumStep;

    Double_t fBdl;  ///< scalar field integrals
    Double_t fBdlx; ///< x component of the field integral
    Double_t fBdly; ///< y component of the field integral
    Double_t fBdlz; ///< z component of the field integral

}; // class QwRayTracer

#endif // __QWRAYTRACER_H__
