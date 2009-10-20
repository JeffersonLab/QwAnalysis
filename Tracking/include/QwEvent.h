/**
 * \class	QwEvent	QwEvent.h
 * \brief Contains a tracked event, i.e. all information from hits to tracks
 *
 * A QwEvent contains all event information, from hits over partial track to
 * complete tracks.  It serves as the final product of the tracking code.
 *
 * \ingroup QwTrackingAnl
 *
 */

#ifndef QWEVENT_H
#define QWEVENT_H

// C and C++ headers
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TObjArray.h"

// Qweak headers
#include "QwTypes.h"
#include "QwTrackingTreeLine.h"

// Forward declarations
class QwHit;
class QwHitContainer;
class QwTrackingTreeLine;
class QwPartialTrack;
class QwTrack;
class QwVertex;


class QwEventHeader: public TObject {

  private:

    UInt_t fRunNumber;		///< Run number
    //
    ULong_t fEventNumber;	///< Event number
    ULong_t fEventTime;		///< Event time (unix time? some time in boost?)
    //
    UInt_t fEventType;		///< Event type (probably bit pattern)
    UInt_t fEventTrigger;	///< Event trigger (probably bit pattern)
    //
    EQwHelicity fBeamHelicity;	///< Beam helicity (from MPS pattern phase)

  public:

    QwEventHeader(){};
    virtual ~QwEventHeader(){};

    void SetRunNumber(const UInt_t runnumber) { fRunNumber = runnumber; };
    UInt_t GetRunNumber() const { return fRunNumber; };

    void SetEventNumber(const ULong_t eventnumber) { fEventNumber = eventnumber; };
    ULong_t GetEventNumber() const { return fEventNumber; };

    void SetEventTime(const ULong_t eventtime) { fEventTime = eventtime; };
    ULong_t GetEventTime() const { return fEventTime; };

    void SetEventType(const UInt_t eventtype) { fEventType = eventtype; };
    UInt_t GetEventType() const { return fEventType; };

    void SetEventTrigger(const UInt_t eventtrigger) { fEventTrigger = eventtrigger; };
    UInt_t GetEventTrigger() const { return fEventTrigger; };

    void SetBeamHelicity(const EQwHelicity helicity) { fBeamHelicity = helicity; };
    EQwHelicity GetBeamHelicity() const { return fBeamHelicity; };

  ClassDef(QwEventHeader,1);
};


class QwEvent: public TObject {

  public:

    // Event header
    QwEventHeader* fEventHeader;

    // Hits
    #define QWEVENT_MAX_NUM_HITS 1000
    Int_t fNQwHits; ///< Number of QwHits in the array
    TClonesArray        *fQwHits; ///< Array of QwHits
    static TClonesArray *gQwHits; ///< Static array of QwHits

    // Tree lines
    #define QWEVENT_MAX_NUM_TREELINES 1000
    Int_t fNQwTreeLines; ///< Number of QwTreeLines in the array
    TClonesArray        *fQwTreeLines; ///< Array of QwTreeLines
    static TClonesArray *gQwTreeLines; ///< Static array of QwTreeLines

    // Tree lines (2nd attempt at getting this into the TBrowser)
    Int_t fNQwTreeLines2; ///< Number of QwTreeLines in the array
    TObjArray *fQwTreeLines2; ///< Array of QwTreeLines

    // Tree lines (3rd attempt)
    std::vector <QwTrackingTreeLine> fQwTreeLines3;

    // Partial tracks
    #define QWEVENT_MAX_NUM_PARTIALTRACKS 1000
    Int_t fNQwPartialTracks; ///< Number of QwPartialTracks in the array
    TClonesArray        *fQwPartialTracks; ///< Array of QwPartialTracks
    static TClonesArray *gQwPartialTracks; ///< Static array of QwPartialTracks


  public:

    QwEvent();
    virtual ~QwEvent();

    // Event header
    QwEventHeader* GetEventHeader() { return fEventHeader; };

    // Housekeeping methods for lists
    void Clear(Option_t *option = ""); // Clear the current event
    void Reset(Option_t *option = ""); // Reset the static arrays (periodically)

    // Creating, adding, and getting hits
    QwHit* CreateNewHit();
    void AddHit(QwHit* hit);
    void ClearHits(Option_t *option = "");
    void ResetHits(Option_t *option = "");
    // Adding, and getting hit containers
    void AddHitContainer(QwHitContainer* hitlist);
    QwHitContainer* GetHitContainer();
    // Get the number of hits
    Int_t GetNumberOfHits() const { return fNQwHits; };
    // Print the list of hits
    void PrintHits();

    // Creating and adding tree lines
    QwTrackingTreeLine* CreateNewTreeLine();
    void AddTreeLine(QwTrackingTreeLine* treeline);
    void ClearTreeLines(Option_t *option = "");
    void ResetTreeLines(Option_t *option = "");
    // Get the number of tree lines
    Int_t GetNumberOfTreeLines() const { return fNQwTreeLines; };
    // Print the list of tree lines
    void PrintTreeLines();

    // Creating and adding tree lines
    QwTrackingTreeLine* CreateNewTreeLine2();
    void AddTreeLine2(QwTrackingTreeLine* treeline);
    void ClearTreeLines2(Option_t *option = "");
    void ResetTreeLines2(Option_t *option = "");
    // Get the number of tree lines
    Int_t GetNumberOfTreeLines2() const { return fNQwTreeLines2; };
    // Print the list of tree lines
    void PrintTreeLines2();

    // Creating and adding partial tracks
    QwPartialTrack* CreateNewPartialTrack();
    void AddPartialTrack(QwPartialTrack* partialtrack);
    void ClearPartialTracks(Option_t *option = "");
    void ResetPartialTracks(Option_t *option = "");
    // Get the number of partial tracks
    Int_t GetNumberOfPartialTracks() const { return fNQwPartialTracks; };
    // Print the list of partial tracks
    void PrintPartialTracks();

    // Output function
    void Print();

  public:

    /*! list of tree lines [upper/lower][region][type][u/v/x/y] */
    QwTrackingTreeLine* treeline[kNumPackages][kNumRegions][kNumTypes][kNumDirections]; //!

    /*! list of partial tracks [package][region][type] */
    QwPartialTrack* parttrack[kNumPackages][kNumRegions][kNumTypes]; //!

    /*! list of complete tracks [package] */
    QwTrack* track[kNumPackages]; //!

    /*! list of vertices in this event */
    //std::vector< QwVertex* > vertex; //!

  ClassDef(QwEvent,1);

}; // class QwEvent

#endif // QWEVENT_H
