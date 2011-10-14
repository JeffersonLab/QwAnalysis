/**********************************************************\
* File: VQwSubsystemParity.h                               *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __VQWSUBSYSTEMPARITY__
#define __VQWSUBSYSTEMPARITY__

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwSubsystem.h"

// Forward declarations
class QwBlinder;
class QwParityDB;


/**
 * \class VQwSubsystemParity
 * \ingroup QwAnalysis
 *
 * \brief Virtual base class for the parity subsystems
 *
 *   Virtual base class for the classes containing the
 *   event-based information from each parity subsystem.
 *   This will define the interfaces used in communicating
 *   with the CODA routines.
 *
 */
class VQwSubsystemParity: virtual public VQwSubsystem {

  public:

    /// Constructor with name
    VQwSubsystemParity(TString name): VQwSubsystem(name) {
      SetEventTypeMask(0x1); // only accept 0x1
    };
    /// Default destructor
    virtual ~VQwSubsystemParity() { };


    /// \brief Fill the database
    virtual void FillDB(QwParityDB *db, TString type) { };


    // VQwSubsystem routine is overridden. Call it at the beginning by VQwSubsystem::operator=(value)
    virtual VQwSubsystem& operator=  (VQwSubsystem *value) = 0;
    virtual VQwSubsystem& operator+= (VQwSubsystem *value) = 0;
    virtual VQwSubsystem& operator-= (VQwSubsystem *value) = 0;
    virtual void Sum(VQwSubsystem *value1, VQwSubsystem *value2) = 0;
    virtual void Difference(VQwSubsystem *value1, VQwSubsystem *value2) = 0;
    virtual void Ratio(VQwSubsystem *numer, VQwSubsystem *denom) = 0;
    virtual void Scale(Double_t factor) = 0;

    virtual VQwSubsystem* Copy() = 0;

    /// \brief Update the running sums for devices
    virtual void AccumulateRunningSum(VQwSubsystem* value) = 0;
    /// \brief Calculate the average for all good events
    virtual void CalculateRunningAverage() = 0;

    /// \brief Load the event cuts file
    virtual Int_t LoadEventCuts(TString filename) = 0;
    /// \brief Apply the single event cuts
    virtual Bool_t ApplySingleEventCuts() = 0;
    /// \brief Report the number of events failed due to HW and event cut failures
    virtual Int_t GetEventcutErrorCounters() = 0;
    /// \brief Return the error flag to the main routine
    virtual UInt_t GetEventcutErrorFlag() = 0;

    /// \brief Blind the asymmetry of this subsystem
    virtual void Blind(const QwBlinder *blinder) { return; };
    /// \brief Blind the difference of this subsystem
    virtual void Blind(const QwBlinder *blinder, const VQwSubsystemParity* subsys) { return; };

    /// \brief Print values of all channels
    virtual void PrintValue() const { };


    virtual Bool_t CheckForEndOfBurst() const {return kFALSE;};
	


  private:

    /// Private default constructor (not implemented, will throw linker error on use)
    VQwSubsystemParity();

}; // class VQwSubsystemParity

#endif // __VQWSUBSYSTEMPARITY__
