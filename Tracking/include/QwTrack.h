/*!
 * \file   QwTrack.h
 * \brief  Definition of the track class
 *
 * \author Wouter Deconinck
 * \date   2009-12-11
 */

#ifndef QWTRACK_H
#define QWTRACK_H

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwPartialTrack.h"
#include "QwBridge.h"
#include "QwVertex.h"

/**
 * \class QwTrack
 * \ingroup QwTracking
 * \brief Contains the complete track as a concatenation of partial tracks
 *
 * A QwTrack contains the complete description of the track as a concatenation
 * of multiple partial tracks.  Associated with the track is the kinematical
 * information, for use in the final Q^2 determination.
 *
 * Several vectors of QwPartialTracks are stored in the QwTrack object.  This
 * allows for combining different QwPartialTracks with each other, and selecting
 * the optimal fit.
 */
class QwTrack: public VQwTrackingElement, public QwObjectCounter<QwTrack> {

  public:

    /// Default constructor
    QwTrack();
    /// Constructor with front and back partial track
    QwTrack(const QwPartialTrack* front, const QwPartialTrack* back);
    /// Copy constructor by reference
    QwTrack(const QwTrack& that);
    /// Copy constructor from pointer
    QwTrack(const QwTrack* that);
    /// Virtual destructor
    virtual ~QwTrack();

    /// Assignment operator
    QwTrack& operator=(const QwTrack& that);

    /// Initialization
    void Initialize();

    bool IsUsed() { return isused; };

    void Print(const Option_t* option = 0) const {
      if (!this) return;
      std::cout << "Track: " << ZVx << ", " << TVx;
      std::cout << std::endl;
      next->Print();
    };

    void SortBridgedTracks() { };

    friend ostream& operator<< (ostream& stream, const QwTrack& t);

  public:

    double ZVx, TVx;		/// Vertex position in Z and transverse
    double The, Phi;		/// theta and phi of track
    int    AngleCorr;		/// are theta and phi are corrected for the holding field
    double rDXSl;		/// bending in the magnet (x direction)
    double fChi;		/// combined chi square

    double fMomentum;		/// spectrometer and calorimeter Energy

    double fXBj, fY, fQ2, fW2, fNu;	/// kinematics

    QwBridge *fBridge;	//!	/// magnet matching information
    int    iMagnetMatched;	/// number of magnet hits along track
    int    yTracks;		/// number of y tracks

    QwTrack *next;		//! next track (do not store in ROOT files)
    QwTrack *ynext;		//! link for y tracks (do not store)
    QwTrack *usednext;		//! link of used tracks (do not store)

    QwPartialTrack *fFront;	//! front partial track (do not store)
    QwPartialTrack *fBack;	//! back partial track (do not store)

    QwVertex *beamvertex;	//! beam vertex (do not store)

    bool   isused;		/// used (part of usedTrack list)
    bool   inBounds;		/// lookup table was usable

    int    iCharge;		/// charge of particle

  ClassDef(QwTrack,1);

}; // class QwTrack

#endif // QWTRACK_H
