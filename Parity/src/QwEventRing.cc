
#include "QwEventRing.h"



QwEventRing::QwEventRing(QwSubsystemArrayParity &event, Int_t ring_size, Int_t event_holdoff, Int_t min_BT_count){
  
  fRING_SIZE=ring_size;
  fEVENT_HOLDOFF=event_holdoff;
  fMIN_BT_COUNT=min_BT_count;
  fEvent_Ring.resize(fRING_SIZE);

  bRING_READY=kFALSE;
  bGoodEvent=kTRUE;
  bEVENT_READY=kTRUE;
  fNextToBeFilled=0;
  fNextToBeRead=0;
  fEventsSinceLastTrip=1;
  fFailedEventCount=0;
  for(int i=0;i<fRING_SIZE;i++){
    fEvent_Ring[i].Copy(&event); //populate the event ring
  }

  //open the log file
  if (bDEBUG_Write)
    out_file = fopen("Ring_log.txt", "wt");
  
};




void QwEventRing::SetupRing(QwSubsystemArrayParity &event){
  /*
  fRING_SIZE=ring_size;
  fEVENT_HOLDOFF=event_holdoff;
  fMIN_BT_COUNT=min_BT_count;
  */
  std::cout<<" Ring "<<fRING_SIZE<<" , "<<fMIN_BT_COUNT<<" , "<<fEVENT_HOLDOFF<<std::endl;
  fEvent_Ring.resize(fRING_SIZE);

  bRING_READY=kFALSE;
  bGoodEvent=kTRUE;
  bEVENT_READY=kTRUE;
  fNextToBeFilled=0;
  fNextToBeRead=0;
  fEventsSinceLastTrip=1;
  fFailedEventCount=0;
  for(int i=0;i<fRING_SIZE;i++){
    fEvent_Ring[i].Copy(&event); //populate the event ring
  }

  //open the log file
  if (bDEBUG_Write)
    out_file = fopen("Ring_log.txt", "wt");
  
}

void QwEventRing::DefineOptions(QwOptions &options){
  // Define the execution options
  options.AddDefaultOptions();
  options.AddOptions()("ring.size", po::value<int>()->default_value(32),"QwEventRing: ring/buffer size");
  options.AddOptions()("ring.bt", po::value<int>()->default_value(4),"QwEventRing: minimum beam trip count");
  options.AddOptions()("ring.hld", po::value<int>()->default_value(16),"QwEventRing: ring hold off");

}

void QwEventRing::ProcessOptions(QwOptions &options){
  //Reads Event Ring parameters from cmd  
  if (gQwOptions.HasValue("ring.size"))
    fRING_SIZE=gQwOptions.GetValue<int>("ring.size");
  if (gQwOptions.HasValue("ring.bt"))
    fMIN_BT_COUNT=gQwOptions.GetValue<int>("ring.bt"); 
  if (gQwOptions.HasValue("ring.hld"))
    fEVENT_HOLDOFF=gQwOptions.GetValue<int>("ring.hld");
};
void QwEventRing::push(QwSubsystemArrayParity &event){
  if (bDEBUG) std::cerr << "QwEventRing::push:  BEGIN" <<std::endl;

  if (!bGoodEvent){//this means we are coming from a beam trip
    bGoodEvent=kTRUE;//set it to true again
    bEVENT_READY=kFALSE;//set this flag to flase till we pass no.of good events
    //we want to let go some good events-> LEAVE_COUNT
    fEventsSinceLastTrip=1;
  }
  
  fFailedEventCount=0;//reset the failed event counter 

  if (bEVENT_READY){
    fEvent_Ring[fNextToBeFilled]=event;//copy the current good event to the ring   


    if (bDEBUG) std::cout<<" Filled at "<<fNextToBeFilled;//<<"Ring count "<<fRing_Count<<std::endl; 
    if (bDEBUG_Write) fprintf(out_file," Filled at %d ",fNextToBeFilled);


    fNextToBeFilled=(fNextToBeFilled+1)%fRING_SIZE;
    
    if(fNextToBeFilled == 0){
      //then we have RING_SIZE events to process
      if (bDEBUG) std::cout<<" RING FILLED "<<fNextToBeFilled+1; //<<std::endl; 
      if (bDEBUG_Write) fprintf(out_file," RING FILLED ");
      bRING_READY=kTRUE;//ring is filled with good multiplets
      fNextToBeFilled=0;//next event to be filled
      fNextToBeRead=0;//first element in the ring      
    }
    //ring processing is done at a separate location
  }else{
    //still we are counting good events after a beam trip that leave alone
    if (bDEBUG) std::cout<<" Event since last trip \n"<<fEventsSinceLastTrip;//<<std::endl; 
    if (bDEBUG_Write)  fprintf(out_file," After Trip  %d \n",fEventsSinceLastTrip);
    fEventsSinceLastTrip++;//increment event counter
    if (fEventsSinceLastTrip >= fEVENT_HOLDOFF)//after we have left LEAVE_COUNT no.of events
      bEVENT_READY=kTRUE;//now from next event onward add them to the ring     
  }
  
  
};

void QwEventRing::FailedEvent(Int_t error_flag){
  if ((error_flag & kErrorFlag_BeamTrip)==kErrorFlag_BeamTrip){//check to see the single event cut is related to a beam current error
    if (bDEBUG) 
      QwMessage<<"Beam Trip kind single event cut failed!"<<QwLog::endl;
  } else
    return; //do nothing
  fFailedEventCount++;
  
  if (bGoodEvent){//a first faliure after set of good event bGoodEvent is TRUE. This is TRUE untill there is a beam trip
    if (fFailedEventCount >= fMIN_BT_COUNT){//if events failed equal to minimum beam trip count
      if (bGoodEvent) std::cout<<" Beam Trip "<<std::endl;
      bGoodEvent=kFALSE;// a beam trip occured, set this to false
      
      if (bDEBUG)
	std::cout<<" Beam Trip "<<fFailedEventCount;
	
      if (bDEBUG_Write) fprintf(out_file," Beam Trip %d \n ",fFailedEventCount);
      fNextToBeFilled=0;//fill at the top ring is useless after the beam trip
      fNextToBeRead=0;//first element in the ring	
      bRING_READY=kFALSE;      
    }
  }
    
  if (bDEBUG) std::cout<<" Failed count \n"<<fFailedEventCount;
  if (bDEBUG_Write) fprintf(out_file," Failed count %d error_flag %x\n",fFailedEventCount,error_flag);
};

QwSubsystemArrayParity& QwEventRing::pop(){
  Int_t tempIndex;
  tempIndex=fNextToBeRead;  
  if (bDEBUG) std::cout<<" Read at "<<fNextToBeRead<<std::endl; 
  if (bDEBUG_Write) fprintf(out_file," Read at %d \n",fNextToBeRead);
  
  if (fNextToBeRead==(fRING_SIZE-1)){
    bRING_READY=kFALSE;//setting to false is an extra measure of security to prevent reading a NULL value. 
  }
  fNextToBeRead=(fNextToBeRead+1)%fRING_SIZE;  
  return fEvent_Ring[tempIndex];  
};


Bool_t QwEventRing::IsReady(){ //Check for readyness to read data from the ring using the pop() routine   
  return bRING_READY;
};

