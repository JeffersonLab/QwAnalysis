
#include "QwEventRing.h"



QwEventRing::QwEventRing(QwSubsystemArrayParity &event, Int_t ring_size){
  
  fRING_SIZE=ring_size;
   fEvent_Ring.resize(fRING_SIZE);

  bRING_READY=kFALSE;
  bEVENT_READY=kTRUE;
  fNextToBeFilled=0;
  fNextToBeRead=0;

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
  bEVENT_READY=kTRUE;
  fNextToBeFilled=0;
  fNextToBeRead=0;

  for(int i=0;i<fRING_SIZE;i++){
    fEvent_Ring[i].Copy(&event); //populate the event ring
  }
  fRollingAvg.Copy(&event); //populate the rolling sum sub system
  //open the log file
  if (bDEBUG_Write)
    out_file = fopen("Ring_log.txt", "wt");


}

void QwEventRing::DefineOptions(QwOptions &options){
  // Define the execution options
  options.AddDefaultOptions();
  options.AddOptions()("ring.size", po::value<int>()->default_value(4800),"QwEventRing: ring/buffer size");
  options.AddOptions()("ring.stability_cut", po::value<double>()->default_value(1),"QwEventRing: Stability ON/OFF");

}

void QwEventRing::ProcessOptions(QwOptions &options){
  //Reads Event Ring parameters from cmd  
  Double_t fStability;
  if (gQwOptions.HasValue("ring.size"))
    fRING_SIZE=gQwOptions.GetValue<int>("ring.size");

  if (gQwOptions.HasValue("ring.stability_cut"))
    fStability=gQwOptions.GetValue<double>("ring.stability_cut");

  if (fStability>0.0)
    bStability=kTRUE;
  else
    bStability=kFALSE;
 
}
void QwEventRing::push(QwSubsystemArrayParity &event){

  if (bDEBUG) QwMessage << "QwEventRing::push:  BEGIN" <<QwLog::endl;

  

  if (bEVENT_READY){
    fEvent_Ring[fNextToBeFilled]=event;//copy the current good event to the ring 
    if (bStability){
      fRollingAvg.AccumulateAllRunningSum(event);
    }


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
    }
    //ring processing is done at a separate location
  }else{
  }
  
  
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
     fRollingAvg.DeaccumulateRunningSum(fEvent_Ring[tempIndex]);
  }
  fNextToBeRead=(fNextToBeRead+1)%fRING_SIZE;  
  return fEvent_Ring[tempIndex];  
}


Bool_t QwEventRing::IsReady(){ //Check for readyness to read data from the ring using the pop() routine   
  return bRING_READY;
}

