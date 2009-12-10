#ifndef QWBRIDGE_H
#define QWBRIDGE_H

#include <math.h>
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwMagneticField.h"
#include "QwTrajectory.h"

// ROOT headers
#include <TFile.h>
#include <TTree.h>

#define MAX_ITERATION 10

/*! \brief This class bridge the front and back partial track to find out the momentum */
///
/// \ingroup QwTracking


  // temporary class 
  class PartialTrackParameter{
    public:
      float position_r;
      float position_phi;
      float direction_theta;
      float direction_phi;
  };


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


    QwTrajectory *trajectory;
    QwMagneticField *B_Field;

//NOTE jpan: where is the best location to load the field map?
    void LoadMagneticFieldMap();
    int BridgeFrontBackPartialTrack(TVector3 startpoint,
                                    TVector3 startpointdirection,
                                    TVector3 endpoint,
                                    TVector3 endpointdirection);
    int Filter(TVector3 fStartPoint, TVector3 fStartDirection, TVector3 fEndPoint, TVector3 fEndDirection);
    int Shooting(TVector3 fStartPoint, TVector3 fStartDirection, TVector3 fEndPoint, TVector3 fEndDirection);

    double GetMomentum(){ return momentum; };

    static int LoadMomentumMatrix();

  private:

    double momentum;

};


#endif // QWBRIDGE_H
