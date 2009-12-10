/**
 * \class	QwTrackingWorker	QwTrackingWorker.h
 *
 * \brief	Controls all the routines involved in finding tracks in an event
 *
 * \author	Wolfgang Wander <wwc@hermes.desy.de>
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@mit.edu>
 *
 * \date	2009-09-04 18:06:23
 * \ingroup	QwTracking
 *
 */

#ifndef QWTRACKINGWORKER_H
#define QWTRACKINGWORKER_H

// Qweak headers
#include "VQwSystem.h"
#include "QwTypes.h"
#include "globals.h"

// Forward declarations
class QwSubsystemArrayTracking;
class QwTrackingTreeRegion;
class QwTrackingTree;
class QwHitContainer;
class QwPartialTrack;
class QwTrack;
class QwEvent;
class QwBridge;

/*------------------------------------------------------------------------*//*!

 \class QwTrackingWorker

 \brief Controls all the routines involved in finding tracks in an event.

 \ingroup QwTracking

*//*-------------------------------------------------------------------------*/

class QwTrackingWorker: public VQwSystem {

  public:

    int tlayers;	///< number of tracking layers
    int tlaym1;		///< ...

    int ngood;		///< number of good events
    int nbad;		///< number of bad events

    int R2Good;
    int R2Bad;
    int R3Good;
    int R3Bad;

    QwTrackingWorker(const char* name);
    ~QwTrackingWorker();

    /// \brief Define command line and config file options
    static void DefineOptions();

    /// \brief Get the debug level
    int GetDebugLevel () { return fDebug; };
    /// \brief Set the debug level
    void SetDebugLevel (int debug) { fDebug = debug; };

    void BCheck (double E, QwPartialTrack *f, QwPartialTrack *b, double TVertex, double ZVertex);
    QwTrack* rcLinkUsedTracks (QwTrack *track, int package);

    /// \brief Process the hit list and construct the event
    QwEvent* ProcessHits (QwSubsystemArrayTracking *detectors, QwHitContainer *hitlist);

  private:

    /// \brief Pattern search tree for all configurations
    QwTrackingTreeRegion* fSearchTree[kNumPackages * kNumRegions * kNumTypes * kNumDirections];

    /// \brief Debug level
    int fDebug;

    /// \brief Region 2 bit patterns
    char *channelr2[TLAYERS];
    /// \brief Region 2 bit pattern hashes
    int  *hashchannelr2[TLAYERS];
    /// \brief Region 2 levels
    int  levelsr2;

    /// \brief Region 3 bit patterns
    char *channelr3[NUMWIRESR3 + 1];
    /// \brief Region 3 bit pattern hashes
    int  *hashchannelr3[NUMWIRESR3 + 1];
    /// \brief Region 3 levels
    int  levelsr3;

    /// \brief Initialize the pattern search tree
    void InitTree();

    // Local flags
    bool fShowEventPattern;
    bool fShowMatchingPattern;

    QwBridge *bridge;

}; // class QwTrackingWorker

#endif // QWTRACKINGWORKER_H
