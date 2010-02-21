/*!
 * \file   QwTrackingWorker.h
 * \brief  Controls all the routines involved in finding tracks in an event
 *
 * \author Wolfgang Wander <wwc@hermes.desy.de>
 * \author Burnham Stokes <bestokes@jlab.org>
 * \author Wouter Deconinck <wdconinc@mit.edu>
 * \date   2009-09-04 18:06:23
 */

#ifndef QWTRACKINGWORKER_H
#define QWTRACKINGWORKER_H

// Qweak headers
#include "QwTypes.h"
#include "QwRayTracer.h"

// Forward declarations
class QwSubsystemArrayTracking;
class QwTrackingTreeRegion;
class QwTrackingTree;
class QwHitContainer;
class QwPartialTrack;
class QwTrack;
class QwEvent;
class QwBridge;

/**
 *  \class QwTrackingWorker
 *  \ingroup QwTracking
 *  \brief Controls all the routines involved in finding tracks in an event
 *
 * The tracking worker coordinates the track finding based on the hit list
 * provided to the ProcessHits() method.
 */
class QwTrackingWorker {

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

    /// \brief Region 2 levels
    int  levelsr2;

    /// \brief Region 3 levels
    int  levelsr3;

    /// \brief Initialize the pattern search tree
    void InitTree();

    // Local flags
    bool fShowEventPattern;
    bool fShowMatchingPattern;

    QwRayTracer *raytracer;

}; // class QwTrackingWorker

#endif // QWTRACKINGWORKER_H
