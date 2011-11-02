/**********************************************************\
* File: QwEventRing.h                                      *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2009-11-08 15:40>                           *
\**********************************************************/

#ifndef __QwEventRing__
#define __QwEventRing__

#include <vector>

#include <fstream>
#include "QwSubsystemArrayParity.h"

#include "QwVQWK_Channel.h"

class QwEventRing {

/******************************************************************
 *  Class: QwEventRing
 *  Description : The event buffer to reduce  beam trips effects on running 
 *  averages.
 *
 ******************************************************************/
 public:
  QwEventRing() { };
  QwEventRing(QwOptions &options) { ProcessOptions(options); };
  QwEventRing(QwSubsystemArrayParity &event, Int_t ring_size);//this will create a fixed size vent ring
  virtual ~QwEventRing() { };

  /// \brief Define options
  static void DefineOptions(QwOptions &options);
  /// \brief Process options
  void ProcessOptions(QwOptions &options);

  /// \brief Add the subsystem to the ring
  void push(QwSubsystemArrayParity &event);
  /// \brief Return the last subsystem in the ring
  QwSubsystemArrayParity& pop();

  /// \brief Return the read status of the ring
  Bool_t IsReady();
  /// \brief Update parameters when an event fails
  void FailedEvent(UInt_t);
  /// \brief check the error flag and flag with kBeamTripError for failed events in ev mode = 3
  Bool_t CheckEvent(UInt_t);

  Int_t GetFailedEventCount() const { return fFailedEventCount; };

  void SetupRing(QwSubsystemArrayParity &event);


 private:

  Int_t fRING_SIZE;//this is the length of the ring
  Int_t fEVENT_HOLDOFF;//no,of good events to discard after a beam trip
  Int_t fMIN_BT_COUNT;//minimum no.of events fails to determine a beam trip has occured


  Int_t fNextToBeFilled;//counts events in the ring
  Int_t fNextToBeRead;//keep track off when to read next from the ring.

  
  Int_t fEventsSinceLastTrip;//no.of good events after a beam trip
  Int_t fFailedEventCount;//Counts on.of failed events. if the fFailedEventCount> Min_BT_COUNT then we have a beam trip
    
  Bool_t bGoodEvent; //beam trip status.
  //kTRUE -> At present no beam trip occurred
  //kFALSE -> A beam trip occurred
  Bool_t bEVENT_READY; //If kTRUE, the good events are added to the event ring. After a beam trip this is set to kFALSE
  //after discarding LEAVE_COUNT no.of good event this is set to kTRUE

  Bool_t bRING_READY; //set to true after ring is filled with good events and time to process them. Set to kFALSE after processing 
  //all the events in the ring
  std::vector<QwSubsystemArrayParity> fEvent_Ring;
  //to track all the rolling averages for stability checks
  QwSubsystemArrayParity fRollingAvg;

  //parameters used in event cut mode 3
  Bool_t  bGoodEvent_ev3; //beam trip status in event cut mode 3
  Bool_t bEVENT_READY_ev3; //After a beam trip this is set to kFALSE, after flagging  LEAVE_COUNT no.of good event this is set to kTRUE
  
  //for debugging purposes
  
  FILE *out_file;   
  static const Bool_t bDEBUG=kFALSE;//kTRUE;
  static const Bool_t bDEBUG_Write=kFALSE;

  //to accumulate the running avg
  QwBeamCharge   fTargetCharge;
  QwBeamCharge   fChargeRunningSum;
  Double_t fStability;//Stability level in units of sigmafChargeRunningSum.
  Bool_t bStability;
  UInt_t fErrorCode;

};




#endif
