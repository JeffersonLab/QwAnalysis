#ifndef QWBRIDGE_H
#define QWBRIDGE_H

// System headers
#include <math.h>

// ROOT headers
#include <TVector3.h>
#include <TFile.h>
#include <TTree.h>

// Qweak headers
#include "QwHit.h"
#include "QwHitContainer.h"

// Forward declarations
class QwMagneticField;

#define MAX_ITERATION 10

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
    double fMomentum;		/// momentum
    QwHit *hits;  		/// matching hits
    QwHitContainer qwhits;	/// matching hits

  private:

};


#endif // QWBRIDGE_H
