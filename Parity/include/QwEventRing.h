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



class QwEventRing{

/******************************************************************
 *  Class: QwHelicityPattern
 *  Description : The event buffer to reduce  beam trips effects on running 
 *  averages.
 *
 ******************************************************************/
 public:
  QwEventRing(){}
  QwEventRing(QwSubsystemArrayParity &event, Int_t ring_size, Int_t event_holdoff, Int_t min_BT_count);//this will create a fixed size vent ring
  ~QwEventRing(){

  };

  /// \brief Define options function
  static void DefineOptions(QwOptions &options);

  void push(QwSubsystemArrayParity &event);//add the subsystem to the ring
  QwSubsystemArrayParity& pop();//return the last subsystem in the ring

  Bool_t IsReady();//return the read status of the ring
  void FailedEvent(Int_t);//update parameters when an event fails
 
  void ProcessOptions(QwOptions &options);
  void SetupRing(QwSubsystemArrayParity &event);





 private:

  Int_t fRING_SIZE;//this is the length of the ring
  Int_t fEVENT_HOLDOFF;//no,of good events to discard after a beam trip
  Int_t fMIN_BT_COUNT;//minimum no.of events fails to determine a beam trip has occured


  Int_t fNextToBeFilled;//counts events in the ring
  Int_t fNextToBeRead;//keep track off when to read next from the ring.

  
  Int_t fEventsSinceLastTrip;//no.of good events after a beam trip
  Int_t fFailedEventCount;//Counts on.of failed events. if the fFailedEventCount> Min_BT_COUNT then we have a beam trip

  
  Bool_t bGoodEvent; //Bbeam trip status. 
  //kTRUE -> At present no beam trip occured
  //kFALSE -> A beam trip occured
  Bool_t bEVENT_READY; //If kTRUE, the good events are added to the event ring. After a beam trip this is set to kFALSE
  //after discarding LEAVE_COUNT no.of good event this is set to kTRUE

  Bool_t bRING_READY; //set to true after ring is filled with good events and time to process them. Set to kFALSE after processing 
  //all the events in the ring
  std::vector<QwSubsystemArrayParity> fEvent_Ring;


  //for debugging purposes
  
  FILE *out_file;   
  static const Bool_t bDEBUG=kFALSE;//kTRUE;
  static const Bool_t bDEBUG_Write=kFALSE;

  static const Int_t kErrorFlag_BeamTrip = 0x100; // in Decimal 256 to event ring to identify the single event cut is failed for a BCM

};




#endif
