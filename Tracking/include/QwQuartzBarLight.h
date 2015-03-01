#ifndef QWQUARTZBARLIGHT_H
#define QWQUARTZBARLIGHT_H

// System headers
#include <vector>

// ROOT headers
#include "TObject.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TObjArray.h"
#include "TVector3.h"

// Qweak headers
#include "QwTypes.h"
#include "QwTreeLine.h"

// Forward declarations
class QwHit;
class QwHitContainer;
class QwGEMCluster;
class QwTreeLine;
class QwPartialTrack;
class QwTrack;
class QwVertex;


/**
 * \class QwQuartzBarLight
 * \ingroup QwTracking
 *
 * \brief Contains header information of a tracked event
 *
 * Objects of this class contain the header information of a tracked event,
 * such as the run number, event number, the trigger type, etc.
 */
class QwQuartzBarLight: public TObject {

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

    /// Default constructor
    QwQuartzBarLight();
    /// Destructor
    virtual ~QwQuartzBarLight();


#if 0
    /// Set the run number
    void SetRunNumber(const UInt_t runnumber) { fRunNumber = runnumber; };
    /// Get the run number
    const UInt_t GetRunNumber() const { return fRunNumber; };

    /// Set the event number
    void SetEventNumber(const ULong_t eventnumber) { fEventNumber = eventnumber; };
    /// Get the event number
    const ULong_t GetEventNumber() const { return fEventNumber; };

    /// Set the event time
    void SetEventTime(const ULong_t eventtime) { fEventTime = eventtime; };
    /// Get the event time
    const ULong_t GetEventTime() const { return fEventTime; };

    /// Set the event type
    void SetEventType(const UInt_t eventtype) { fEventType = eventtype; };
    /// Get the event type
    const UInt_t GetEventType() const { return fEventType; };

    /// Set the event trigger
    void SetEventTrigger(const UInt_t eventtrigger) { fEventTrigger = eventtrigger; };
    /// Get the event trigger
    const UInt_t GetEventTrigger() const { return fEventTrigger; };

    /// Set the beam helicity
    void SetBeamHelicity(const EQwHelicity helicity) { fBeamHelicity = helicity; };
    /// Get the beam helicity
    const EQwHelicity GetBeamHelicity() const { return fBeamHelicity; };
#endif

    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwQuartzBarLight& h);

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwQuartzBarLight,1);
  #endif
};

/// Output stream operator
inline std::ostream& operator<< (std::ostream& stream, const QwQuartzBarLight& h) {
  stream << "Run " << h.fRunNumber << ", ";
  stream << "event " << h.fEventNumber << ":";
  return stream;
}



#endif // QWQUARTZBARLIGHT_H
