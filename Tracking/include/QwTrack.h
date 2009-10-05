#ifndef QWTRACK_H
#define QWTRACK_H

#include "TObject.h"

#include "QwPartialTrack.h"
#include "QwBridge.h"
#include "QwVertex.h"

/*------------------------------------------------------------------------*//*!

 \brief Contains the complete track as a concatenation of partial tracks

 A QwTrack contains the complete description of the track as a concatenation
 of multiple partial tracks.  Associated with the track is the kinematical
 information, for use in the final Q^2 determination.

 Several vectors of QwPartialTracks are stored in the QwTrack object.  This
 allows for combining different QwPartialTracks with each other, and selecting
 the optimal fit.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
class QwTrack: public TObject {

  public:

    QwTrack();
    ~QwTrack();

    bool IsUsed() { return isused; };

    void Print() {
      if (!this) return;
      std::cout << "Track: " << ZVx << ", " << TVx;
      std::cout << std::endl;
      next->Print();
    };

    void SortBridgedTracks() { };

  public:

    double ZVx, TVx;		/// Vertex posistion in Z and transverse
    double The, Phi;		/// theta and phi of track
    int    AngleCorr;		/// are theta and phi are corrected for the holding field
    double rDXSl;		/// bending in the magnet (x direction)
    double fChi;		/// combined chi square

    double fMomentum;		/// spectrometer and calorimeter Energy

    double fXBj, fY, fQ2, fW2, fNu;	/// kinematics

    int    RecoEvent;		/// track from mcHits instead of digits

    QwBridge *bridge;		/// magnet matching information
    int    iMagnetMatched;	/// number of magnet hits along track
    int    yTracks;		/// number of y tracks

    QwTrack *next;		//! next track (do not store in ROOT files)
    QwTrack *ynext;		//! link for y tracks (do not store)
    QwTrack *usednext;		//! link of used tracks (do not store)

    QwPartialTrack *front;	//! front partial track (do not store)
    QwPartialTrack *back;	//! back partial track (do not store)

    QwVertex *beamvertex;	//! beam vertex (do not store)

    bool   isused;		/// used (part of usedTrack list)
    bool   inBounds;		/// lookup table was usable

    int    iCharge;		/// charge of particle

  ClassDef(QwTrack,1);

};

#endif // QWTRACK_H
