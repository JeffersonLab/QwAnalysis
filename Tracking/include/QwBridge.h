#ifndef QWBRIDGE_H
#define QWBRIDGE_H

#include "QwHit.h"
#include "QwHitContainer.h"

/*! \brief This class has not yet been implemented */
///
/// \ingroup QwTrackingAnl

class QwBridge {

  public:

    double xOff, yOff, ySOff;	/// Offset expected from lookup
    double ySlopeMatch;		/// Bending: slope match in Y
    double yMatch;		/// match in y
    double ySMatch;		/// matching in Yslope
    double xMatch;		/// match in x
    double Momentum;		/// momentum
    QwHit *hits;  		/// matching hits
    QwHitContainer qwhits;	/// matching hits

  private:

};

#endif // QWBRIDGE_H
