#ifndef QWTRACK_H
#define QWTRACK_H

#include "tracking.h"
#include "QwPartialTrack.h"
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
class QwTrack {

  public:

    QwTrack();
    ~QwTrack();

    void SortBridgedTracks();

  public:

    double ZVx, TVx;		/*!< Vertex posistion in Z and transverse */
    double The, Phi;		/*!< theta and phi of track */
    int    AngleCorr;		/*!< true if theta and phi are corrected for
				     the holding field */
    double rDXSl;		/*!< bending in the magnet (x direction) */
    double chi;			/*!< combined chi square */

    double P;			/*!< spectrometer and calorimeter Energy */

    double XBj, Y, Q2, W2, Nu;	/*!< kinematics */

    int    RecoEvent;		/*!< track from mcHits instead of digits */

    Bridge *bridge;		/*!< magnet matching information */
    int    iMagnetMatched;	/*!< number of magnet hits along track */
    int    yTracks;		/*!< number of y tracks */

    QwTrack *next;		/*!< next track */
    QwTrack *ynext;		/*!< link for y tracks */
    QwTrack *usednext;		/*!< link of used tracks */

    QwPartialTrack *front;	/*!< front partial track */
    QwPartialTrack *back;	/*!< back partial track */

    QwVertex *beamvertex;	/*!< beam vertex */

    bool   Used;		/*!< used (part of usedTrack list) */
    bool   inBounds;		/*!< lookup table was usable */

    int    iCharge;		/*!< charge of particle */

};

#endif // QWTRACK_H
