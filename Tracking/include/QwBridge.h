#ifndef QWBRIDGE_H
#define QWBRIDGE_H

#include <math.h>
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwMagneticField.h"
#include "QwTrajectory.h"

/*! \brief This class has not yet been implemented */
///
/// \ingroup QwTrackingAnl

class QwBridge {

  public:

    QwBridge();
    ~QwBridge();

    double xOff, yOff, ySOff;	/// Offset expected from lookup
    double ySlopeMatch;		/// Bending: slope match in Y
    double yMatch;		/// match in y
    double ySMatch;		/// matching in Yslope
    double xMatch;		/// match in x
    double Momentum;		/// momentum
    QwHit *hits;  		/// matching hits
    QwHitContainer qwhits;	/// matching hits


    QwMagneticField *B_Field;

//NOTE jpan: where is the best location to load the field map?
    int LoadMagneticFieldMap();
    int BridgeFrontBackPartialTrack(TVector3 startpoint, TVector3 direction, TVector3 endpoint);
    int Shooting(TVector3 fStartPoint, TVector3 fDirection, TVector3 fEndPoint);

    double GetMomentum(){ return momentum; };

    QwTrajectory *trajectory;

  private:

    double momentum;

};

#endif // QWBRIDGE_H
