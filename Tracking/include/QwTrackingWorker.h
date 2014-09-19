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
#include "QwOptions.h"
#include "QwGeometry.h"
#include "QwRayTracer.h"

// Forward declarations
class QwSubsystemArrayTracking;
class QwTrackingTreeRegion;
class QwTrackingTree;
class QwHitContainer;
class QwPartialTrack;
class QwTrack;
class QwEvent;
class QwBridgingTrackFilter;
class QwMatrixLookup;

// Forward declarations (modules)
class QwTrackingTreeSearch;
class QwTrackingTreeCombine;
class QwTrackingTreeSort;
class QwTrackingTreeMatch;


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

    int ngood;		///< number of good events
    int nbad;		///< number of bad events

    int R2Good;
    int R2Bad;
    int R3Good;
    int R3Bad;
    
    int nbridged;      ///< number of beidged tracks

    /// \brief Default constructor with name
    QwTrackingWorker(QwOptions& options, const QwGeometry& geometry);
    /// \brief Destructor
    virtual ~QwTrackingWorker();

    /// \brief Define command line and config file options
    static void DefineOptions(QwOptions& options);
    /// \brief Process command line and config file options
    void ProcessOptions(QwOptions& options);

    /// \brief Get the debug level
    int GetDebugLevel() const { return fDebug; };
    /// \brief Set the debug level
    void SetDebugLevel(const int debug) { fDebug = debug; };

    /// \brief Get the geometry
    const QwGeometry GetGeometry() const { return fGeometry; };
    /// \brief Set the geometry
    void SetGeometry(const QwGeometry& geometry) { fGeometry = geometry; };

    /// Get the magnetic field current
    double GetMagneticFieldCurrent() const {
      if (fRayTracer) return fRayTracer->GetMagneticFieldCurrent();
      else return 0.0;
    }
    /// Set the magnetic field current
    void SetMagneticFieldCurrent(const double current) {
      if (fRayTracer) fRayTracer->SetMagneticFieldCurrent(current);
    }

    /// \brief Process the hit list and construct the event
    void ProcessEvent (const QwSubsystemArrayTracking *detectors, QwEvent *event);

  private:

    /// \brief Detector geometry
    QwGeometry fGeometry;

    /// \brief Debug level
    int fDebug;

    /// \brief Regenerate the search tree
    bool fRegenerate;

    /// \brief Region 2 levels
    int fLevelsR2;
    /// \brief Region 3 levels
    int fLevelsR3;

    /// \brief Initialize the pattern search tree
    void InitTree(const QwGeometry& geometry);

    /// \name Momentum determination bridging methods
    //@{
    /// Track filter
    QwBridgingTrackFilter* fBridgingTrackFilter;
    /// Ray tracing bridging method
    QwRayTracer* fRayTracer;
    //@}


    /// \name Parsed configuration options
    //@{
    bool fDisableTracking;	///< Disable all tracking
    bool fPrintPatternDatabase; ///< Print the pattern database
    bool fShowEventPattern;	///< Show all event patterns
    bool fShowMatchingPattern;	///< Show matching event patterns
    bool fDisableMomentum;	///< Disable momentum reconstruction
    bool fDisableMatrixLookup;	///< Disable matrix lookup momentum reconstruction
    bool fDisableRayTracer;	///< Disable ray tracer momentum reconstruction
    bool fMismatchPkg;          ///< Indicates if the pkg for R2 and R3 is differnt at the same octant
    std::string fFilenameFieldmap;	///< Filename of the fieldmap in QW_FIELDMAP
    std::string fFilenameLookupTable;	///< Filename of the lookup table in QW_LOOKUP
    //@}

  private:

    /// \name Helper modules
    //@{
    QwTrackingTreeSearch*  fTreeSearch;  ///< Module that handles the tree search
    QwTrackingTreeCombine* fTreeCombine; ///< Module that combines treelines and partial tracks
    QwTrackingTreeSort*    fTreeSort;    ///< Module that sorts lists of treelines and partial tracks
    QwTrackingTreeMatch*   fTreeMatch;   ///< Module that matches up VDC front and back treelines
    //@}

  private:

    // Disabled default constructor
    QwTrackingWorker();

}; // class QwTrackingWorker

#endif // QWTRACKINGWORKER_H
