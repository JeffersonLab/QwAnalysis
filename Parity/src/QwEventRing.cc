
#include "QwEventRing.h"



QwEventRing::QwEventRing(QwSubsystemArrayParity &event, Int_t ring_size){
  
  fRING_SIZE=ring_size;
  //fEVENT_HOLDOFF=event_holdoff;
  //fMIN_BT_COUNT=min_BT_count;
  fEvent_Ring.resize(fRING_SIZE);

  bRING_READY=kFALSE;
  //bGoodEvent=kTRUE;
  bEVENT_READY=kTRUE;
  fNextToBeFilled=0;
  fNextToBeRead=0;
  //fEventsSinceLastTrip=1;
  //fFailedEventCount=0;
  for(int i=0;i<fRING_SIZE;i++){
    fEvent_Ring[i].Copy(&event); //populate the event ring
  }
  fRollingAvg.Copy(&event); //populate the rolling sum sub system
  
  //open the log file
  if (bDEBUG_Write)
    out_file = fopen("Ring_log.txt", "wt");
  
}




void QwEventRing::SetupRing(QwSubsystemArrayParity &event){
  QwMessage<<" Ring size is "<<fRING_SIZE<<" events"<<QwLog::endl;
  if (fRING_SIZE>10000){
    QwError<<"Ring size is too large. Set a value below 10000 events."<<QwLog::endl;
    exit(1);
  }
  fEvent_Ring.resize(fRING_SIZE);

  bRING_READY=kFALSE;
  //bGoodEvent=kTRUE;
  //bGoodEvent_ev3=kTRUE;
  bEVENT_READY=kTRUE;
  //bEVENT_READY_ev3=kTRUE;
  fNextToBeFilled=0;
  fNextToBeRead=0;
  //fEventsSinceLastTrip=1;
  //fFailedEventCount=0;
  for(int i=0;i<fRING_SIZE;i++){
    fEvent_Ring[i].Copy(&event); //populate the event ring
  }
  fRollingAvg.Copy(&event); //populate the rolling sum sub system
  //open the log file
  if (bDEBUG_Write)
    out_file = fopen("Ring_log.txt", "wt");


  //fTargetCharge.InitializeChannel("q_targ","derived");
  //fChargeRunningSum.InitializeChannel("q_targ","derived");

  fErrorCode=0;
}

void QwEventRing::DefineOptions(QwOptions &options){
  // Define the execution options
  options.AddDefaultOptions();
  options.AddOptions()("ring.size", po::value<int>()->default_value(4800),"QwEventRing: ring/buffer size");
  //options.AddOptions()("ring.bt", po::value<int>()->default_value(2),"QwEventRing: minimum beam trip count");
  //options.AddOptions()("ring.hld", po::value<int>()->default_value(25000),"QwEventRing: ring hold off");
  options.AddOptions()("ring.stability_cut", po::value<double>()->default_value(1),"QwEventRing: Stability ON/OFF");

}

void QwEventRing::ProcessOptions(QwOptions &options){
  //Reads Event Ring parameters from cmd  
  if (gQwOptions.HasValue("ring.size"))
    fRING_SIZE=gQwOptions.GetValue<int>("ring.size");
  /*
  if (gQwOptions.HasValue("ring.bt"))
    fMIN_BT_COUNT=gQwOptions.GetValue<int>("ring.bt"); 
  if (gQwOptions.HasValue("ring.hld"))
    fEVENT_HOLDOFF=gQwOptions.GetValue<int>("ring.hld");
  */
  if (gQwOptions.HasValue("ring.stability_cut"))
    fStability=gQwOptions.GetValue<double>("ring.stability_cut");

  if (fStability>0.0)
    bStability=kTRUE;
  else
    bStability=kFALSE;
 
}
void QwEventRing::push(QwSubsystemArrayParity &event){
  /*
  if (CheckEvent(event.GetEventcutErrorFlag())){
    fFailedEventCount=0;//reset the failed event counter if event is good
    if (!bGoodEvent_ev3){//this means we are coming from a beam trip in ev mode 3
      QwMessage<<" Beam trip recovered [ev mode 3] "<<QwLog::endl;
      bGoodEvent_ev3=kTRUE;//set it to true again
      fEventsSinceLastTrip=0;
      bEVENT_READY_ev3=kFALSE;
      fErrorCode|=kBeamTripError;//set hold_off events with beam trip error
    }
  }
  */
  if (bDEBUG) QwMessage << "QwEventRing::push:  BEGIN" <<QwLog::endl;
  /*
  if (!bGoodEvent){//this means we are coming from a beam trip
    QwMessage<<" Beam trip recovered  "<<QwLog::endl;
    bGoodEvent=kTRUE;//set it to true again
    bEVENT_READY=kFALSE;//set this flag to flase till we pass no.of good events
    //we want to let go some good events-> LEAVE_COUNT
    fEventsSinceLastTrip=0;
  }
  
  */
  

  if (bEVENT_READY){
    fEvent_Ring[fNextToBeFilled]=event;//copy the current good event to the ring 
    if (bStability){
      //event.RequestExternalValue("q_targ", &fTargetCharge);
      //fChargeRunningSum.AccumulateRunningSum(fTargetCharge);
      fRollingAvg.AccumulateAllRunningSum(event);
    }

    //if eve mode = 3 flag fEVENT_HOLDOFF events with kBeamTripError flag
    /*
    if (!bEVENT_READY_ev3){
      fEvent_Ring[fNextToBeFilled].UpdateEventcutErrorFlag(fErrorCode);
      if (bDEBUG) QwMessage<<" Setting flag to holding events "<<fEventsSinceLastTrip<<QwLog::endl;
      fEventsSinceLastTrip++;//increment event counter
      if (fEventsSinceLastTrip >= fEVENT_HOLDOFF){//after we have left LEAVE_COUNT no.of events
	bEVENT_READY_ev3=kTRUE;//now from next event onward add them to the ring  
	fErrorCode=0;
      }  
    }
    */

    if (bDEBUG) QwMessage<<" Filled at "<<fNextToBeFilled;//<<"Ring count "<<fRing_Count<<QwLog::endl; 
    if (bDEBUG_Write) fprintf(out_file," Filled at %d ",fNextToBeFilled);


    fNextToBeFilled=(fNextToBeFilled+1)%fRING_SIZE;
    
    if(fNextToBeFilled == 0){
      //then we have RING_SIZE events to process
      if (bDEBUG) QwMessage<<" RING FILLED "<<fNextToBeFilled+1; //<<QwLog::endl; 
      if (bDEBUG_Write) fprintf(out_file," RING FILLED ");
      bRING_READY=kTRUE;//ring is filled with good multiplets
      fNextToBeFilled=0;//next event to be filled
      fNextToBeRead=0;//first element in the ring  
      //check for current ramps
      if (bStability){
	//fChargeRunningSum.CalculateRunningAverage();
	fRollingAvg.CalculateRunningAverage();
	/*
	//The fRollingAvg dose not contain any regular errorcodes since it only accumulate rolling sum for errorflag==0 event.
	//The only errorflag it generates is the stability cut faliure error when the rolling avg is computed. 
	//Therefore when fRollingAvg.GetEventcutErrorFlag() is called it will return non-zero error code only if a global stability cut has failed
	//When fRollingAvg.GetEventcutErrorFlag() is called the fErrorFlag of the subsystemarrayparity object will be updated with any global
	//stability cut faliures
	*/
	fRollingAvg.GetEventcutErrorFlag(); //to update the global error code in the fRollingAvg
	for(Int_t i=0;i<fRING_SIZE;i++){
	  fEvent_Ring[i].UpdateEventcutErrorFlag(fRollingAvg);
	  fEvent_Ring[i].GetEventcutErrorFlag();
	}
	
      }
      /*
      if (bStability && fChargeRunningSum.GetValueWidth()>fStability){//if the SD is large than the fStability
	
	QwMessage<<"-----------Stability Check Failed-----------"<<QwLog::endl;
	//fChargeRunningSum.PrintValue();
	QwMessage << " Running Average +/- width "<<fChargeRunningSum.GetValue()<<" +/- "<<fChargeRunningSum.GetValueWidth()<<" Stable width < "<<fStability<<QwLog::endl;
	QwMessage<<"-----------Stability Check Failed-----------"<<QwLog::endl;
	//fErrorCode|=kBeamStabilityError;//set hold_off events with beam stability error
	
	//bEVENT_READY_ev3=kFALSE;
	//for(Int_t i=0;i<fRING_SIZE;i++){
	//fEvent_Ring[i].UpdateEventcutErrorFlag(kBeamStabilityError);	  
	//fEvent_Ring[i].UpdateEventcutErrorFlag(fRollingAvg);
	//}
      }
      */
    }
    //ring processing is done at a separate location
  }else{
    //still we are counting good events after a beam trip that leave alone
    //if (bDEBUG) QwMessage<<" Event since last trip \n"<<fEventsSinceLastTrip;//<<QwLog::endl; 
    //if (bDEBUG_Write)  fprintf(out_file," After Trip  %d \n",fEventsSinceLastTrip);
    //fEventsSinceLastTrip++;//increment event counter
    //if (fEventsSinceLastTrip >= fEVENT_HOLDOFF)//after we have left LEAVE_COUNT no.of events
    //bEVENT_READY=kTRUE;//now from next event onward add them to the ring     
  }
  
  
}

void QwEventRing::FailedEvent(UInt_t error_flag){
  if (((error_flag & kBCMErrorFlag)==kBCMErrorFlag) && ((error_flag & kEventCutMode3)== 0)){//check to see the single event cut is related to a beam current error and not in event cut mode 3
    if (bDEBUG) 
      QwMessage<<"Beam Trip kind single event cut failed!"<<QwLog::endl;
  } else
    return; //do nothing
  fFailedEventCount++;
  
  if (bGoodEvent){//a first faliure after set of good event bGoodEvent is TRUE. This is TRUE untill there is a beam trip
    if (fFailedEventCount >= fMIN_BT_COUNT){//if events failed equal to minimum beam trip count
      if (bGoodEvent) QwMessage<<" Beam Trip "<<QwLog::endl;
      bGoodEvent=kFALSE;// a beam trip occured, set this to false
      
      if (bDEBUG)
	QwMessage<<" Beam Trip "<<fFailedEventCount;
	
      if (bDEBUG_Write) fprintf(out_file," Beam Trip %d \n ",fFailedEventCount);
      fNextToBeFilled=0;//fill at the top ring is useless after the beam trip
      fNextToBeRead=0;//first element in the ring	
      bRING_READY=kFALSE;      
    }
  }
  
  if (bDEBUG) QwMessage<<" Failed count \n"<<fFailedEventCount;
  if (bDEBUG_Write) fprintf(out_file," Failed count %d error_flag %x\n",fFailedEventCount,error_flag);
}

Bool_t QwEventRing::CheckEvent(UInt_t error_flag){

  if (((error_flag & kBCMErrorFlag)==kBCMErrorFlag)  && ((error_flag & kEventCutMode3)== kEventCutMode3)){
    //this is a global event failed in ev mode = 3
    fFailedEventCount++;
    if (bDEBUG)
      QwMessage<<" Beam Trip [ev mode 3] related error"<<QwLog::endl;
    if (bGoodEvent_ev3){//Im ev mode 3, A first faliure after set of good event bGoodEvent is TRUE. This is TRUE untill there is a beam trip
      if (fFailedEventCount >= fMIN_BT_COUNT){//if events failed equal to minimum beam trip count
	if (bGoodEvent_ev3) QwMessage<<" Beam Trip [ev mode 3]"<<QwLog::endl;
	bGoodEvent_ev3=kFALSE;// a beam trip occured, set this to false
	//Now the all the event in the ring must be flagged with kBeamTripError flag
	fErrorCode=kBeamTripError;
	for(Int_t i=0;i<fRING_SIZE;i++)
	  fEvent_Ring[i].UpdateEventcutErrorFlag(fErrorCode);
      }
    }     
  }
  else
    return kTRUE;

  if (bDEBUG) QwMessage<<" Failed count \n"<<fFailedEventCount;
  if (bDEBUG_Write) fprintf(out_file," [ev3]Failed count %d error_flag %x\n",fFailedEventCount,error_flag); 
  
  return kFALSE;
}

QwSubsystemArrayParity& QwEventRing::pop(){
  Int_t tempIndex;
  tempIndex=fNextToBeRead;  
  if (bDEBUG) QwMessage<<" Read at "<<fNextToBeRead<<QwLog::endl; 
  if (bDEBUG_Write) fprintf(out_file," Read at %d \n",fNextToBeRead);
  
  if (fNextToBeRead==(fRING_SIZE-1)){
    bRING_READY=kFALSE;//setting to false is an extra measure of security to prevent reading a NULL value. 
  }
  if (bStability){
    //fEvent_Ring[tempIndex].RequestExternalValue("q_targ", &fTargetCharge); 
    //fChargeRunningSum.DeaccumulateRunningSum(fTargetCharge);
    fRollingAvg.DeaccumulateRunningSum(fEvent_Ring[tempIndex]);
  }
  //fChargeRunningSum.CalculateRunningAverage();
  //fChargeRunningSum.PrintValue();
  fNextToBeRead=(fNextToBeRead+1)%fRING_SIZE;  
  return fEvent_Ring[tempIndex];  
}


Bool_t QwEventRing::IsReady(){ //Check for readyness to read data from the ring using the pop() routine   
  return bRING_READY;
}

