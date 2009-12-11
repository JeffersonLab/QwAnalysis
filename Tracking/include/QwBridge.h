#ifndef QWBRIDGE_H
#define QWBRIDGE_H

#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwMagneticField.h"
#include "QwTrajectory.h"

/*! \brief This class bridge the front and back partial track to find out the momentum */
///
/// \ingroup QwTracking


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

  private:

};


#endif // QWBRIDGE_H
