/**********************************************************\
* File: QwBPMStripline.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBPMStripline.h"
#include "QwHistogramHelper.h"
#include <stdexcept>



const Bool_t QwBPMStripline::kDEBUG = kFALSE;

/*! Position calibration factor, transform ADC counts in mm
value read in G0BeamMonitor on May 2008*/
const Double_t QwBPMStripline::kQwStriplineCalibration = 18.77;
const Double_t QwBPMStripline::kRotationCorrection = 1./1.414;
const TString QwBPMStripline::subelement[4]={"XP","XM","YP","YM"};
const TString QwBPMStripline::axis[3]={"X","Y","Z"};
/* With X being vertical up and Z being the beam direction toward the beamdump */

/********************************************************/
void  QwBPMStripline::InitializeChannel(TString name, Bool_t ROTATED)
{
  bRotated=ROTATED;

  SetOffset(0,0,0);
  counter=0;  //used to validate sequence number in the IsGoodEvent()
  Event_Counter=0; //count event cut passes events for average calculation
  
  for(int i=0;i<4;i++)
    {
      fWire[i].InitializeChannel(name+subelement[i],"raw");
      if(kDEBUG)
	std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
    }

  for(int i=0;i<2;i++){
	fRelPos[i].InitializeChannel(name+"Rel"+axis[i],"derived");
	fRelPos_Running_AVG[i]=0;
  }

  for(int i=0;i<3;i++)
	fAbsPos[i].InitializeChannel(name+axis[i],"derived");
 
  SetElementName(name);
  bFullSave=kTRUE;

  return;
};
/********************************************************/
void QwBPMStripline::ClearEventData()
{
  for(int i=0;i<4;i++)
	fWire[i].ClearEventData();

  for(int i=0;i<2;i++)
	fRelPos[i].ClearEventData();

  for(int i=0;i<3;i++)
	fAbsPos[i].ClearEventData();

	return;
};
/********************************************************/

void QwBPMStripline::ReportErrorCounters(){
  for(int i=0;i<4;i++)
    fWire[i].ReportErrorCounters();

     
  //std::cout<<"RelX ";
  fRelPos[0].ReportErrorCounters();
  //std::cout<<"RelY ";
  fRelPos[1].ReportErrorCounters();
};

/********************************************************/
void QwBPMStripline::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  // Rotate the requested position if necessary (this is not tested yet)
  if (bRotated) {
    Double_t rotated_meanX = (meanX + meanY) / kRotationCorrection;
    Double_t rotated_meanY = (meanX - meanY) / kRotationCorrection;
    meanX = rotated_meanX;
    meanY = rotated_meanY;
  }

  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX / kQwStriplineCalibration) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX / kQwStriplineCalibration) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY / kQwStriplineCalibration) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY / kQwStriplineCalibration) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is not tested yet)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  Double_t sigmaXM = sigmaXP;
  Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  Double_t sigmaYM = sigmaYP;

  // Propagate these parameters to the ADCs
  fWire[0].SetRandomEventParameters(meanXP, sigmaXP);
  fWire[1].SetRandomEventParameters(meanXM, sigmaXM);
  fWire[2].SetRandomEventParameters(meanYP, sigmaYP);
  fWire[3].SetRandomEventParameters(meanYM, sigmaYM);
};
/********************************************************/
void QwBPMStripline::RandomizeEventData(int helicity)
{
  for (int i = 0; i < 4; i++)
    fWire[i].RandomizeEventData(helicity);

  return;
};
/********************************************************/
void QwBPMStripline::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  // This needs to be modified to allow setting the position
  for (int i = 0; i < 2; i++) {
    fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
  }

  return;
};
/********************************************************/
void QwBPMStripline::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (int i = 0; i < 4; i++)
    fWire[i].EncodeEventData(buffer);
};
/********************************************************/
 
Bool_t QwBPMStripline::ApplySingleEventCuts(){
  Bool_t status=kTRUE;
  
  

  if (fGoodEvent){


    if (!(fULimitX==0 && fLLimitX==0 && fULimitY==0 && fLLimitY==0)){//if fULimitX, fLLimitX, fULimitY & fLLimitY  are non  zero then perform event cuts on this BCM.

      //we only need to check two final values 
      if (fRelPos[0].GetHardwareSum()<=fULimitX && fRelPos[0].GetHardwareSum()>=fLLimitX){ //for RelX
	//fRelPos_Running_AVG[0]=(((Event_Counter-1)*fRelPos_Running_AVG[0])/Event_Counter +  fRelPos[0].GetHardwareSum()/Event_Counter); //this is the running avg of the BCM
	status&=kTRUE;
      }
      else{
	fRelPos[0].UpdateEventCutErrorCount();
	status&=kFALSE;
	if (bDEBUG) std::cout<<" Rel X event cut failed ";
      }
      if (fRelPos[1].GetHardwareSum()<=fULimitY && fRelPos[1].GetHardwareSum()>=fLLimitY){//for RelY
	//fRelPos_Running_AVG[1]=(((Event_Counter-1)*fRelPos_Running_AVG[1])/Event_Counter +  fRelPos[1].GetHardwareSum()/Event_Counter); //this is the running avg of the BCM
	status&=kTRUE;
      //std::cout<<" ";
	}
      else{
	fRelPos[1].UpdateEventCutErrorCount();
	status&=kFALSE;
	if (bDEBUG) std::cout<<" Rel Y event cut failed ";
      }
      if (status){
	Event_Counter++;//Increment the counter, it is a good event.
	CalculateRunningAverages();//calculate the averages for RelX and RelY	
      }
	
    }
    else             
       status=kTRUE;
    

  }else{
    for(int i=0;i<4;i++)
	 fWire[i].UpdateHWErrorCount();
    if (bDEBUG) std::cout<<" Hardware failed ";
    status=kFALSE;
  }


  //std::cout<<"QwBPMStripline::"<<GetElementName()<<" sample size"<<fSampleSize<<std::endl;    
  //if (!status)
  //std::cout<<"QwBPMStripline::"<<GetElementName()<<" event cuts falied "<<std::endl;

  
  return status;
};

/********************************************************/

Int_t QwBPMStripline::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

/********************************************************/
void QwBPMStripline::ResetRunningAverages(){
   std::cout<<"QwBPMStripline::"<<GetElementName();
  for(Short_t i=0;i<2;i++){
    std::cout<<"Running AVG "<<fRelPos_Running_AVG[i]<<" AVG^2 "<<fRelPos_Running_AVG_square[i];
    fRelPos_Running_AVG[i]=0;
    fRelPos_Running_AVG_square[i]=0;
  }
  std::cout<<std::endl;
  Event_Counter=0;  
}

/********************************************************/

void QwBPMStripline::CalculateRunningAverages(){
  //calculate the running AVG
  for(Short_t i=0;i<2;i++){
    fRelPos_Running_AVG[i]=(((Event_Counter-1)*fRelPos_Running_AVG[i])/Event_Counter +  fRelPos[i].GetHardwareSum()/Event_Counter); //this is the running avg of the BCM
    fRelPos_Running_AVG_square[i]=(((Event_Counter-1)*fRelPos_Running_AVG_square[i])/Event_Counter +  fRelPos[i].GetHardwareSum()*fRelPos[i].GetHardwareSum()/Event_Counter); //this is the running avg of the BCM 
  }
}

/********************************************************/

Bool_t QwBPMStripline::CheckRunningAverages(Bool_t bDisplayAVG){
  Bool_t status;
  Double_t fRunning_sigma[2];

  if (!(fULimitX==0 && fLLimitX==0 && fULimitY==0 && fLLimitY==0)){// if samplesize is zero or less  BPM is not in the eventcut list ignore it.
    
    if (fRelPos_Running_AVG[0]<=fULimitX && fRelPos_Running_AVG[0]>=fLLimitX){
      status &= kTRUE;
      fRunning_sigma[0]=(fRelPos_Running_AVG_square[0]-(fRelPos_Running_AVG[0]*fRelPos_Running_AVG[0]))/Event_Counter;
      if (bDisplayAVG)
	std::cout<<" Running AVG "<<GetElementName()<<" RelX current running AVG "<<fRelPos_Running_AVG[0]<<" current running AVG^2: "<<fRelPos_Running_AVG_square[0]<<" \n Uncertainty in mean "<<fRunning_sigma[0]<<std::endl;
    }
    else{
      if (bDisplayAVG)
	std::cout<<" QwBPMStripline::"<<GetElementName()<<" RelX current running AVG "<<fRelPos_Running_AVG[0]<<" is out of range ! "<<" current running AVG^2: "<<fRelPos_Running_AVG_square[0]<<"\n Uncertainty in mean "<<fRunning_sigma[0]<<std::endl;
      status &= kFALSE;
    }
    if (fRelPos_Running_AVG[1]<=fULimitY && fRelPos_Running_AVG[1]>=fLLimitY){
      status &= kTRUE;
      fRunning_sigma[1]=(fRelPos_Running_AVG_square[1]-(fRelPos_Running_AVG[1]*fRelPos_Running_AVG[1]))/Event_Counter;
      if (bDisplayAVG)
	std::cout<<" Running AVG "<<GetElementName()<<" RelY current running AVG "<<fRelPos_Running_AVG[1]<<" current running AVG^2: "<<fRelPos_Running_AVG_square[1]<<"\n Uncertainty in mean "<<fRunning_sigma[1]<<std::endl;
    }
    else{
      if (bDisplayAVG)
	std::cout<<" QwBPMStripline:: "<<GetElementName()<<" RelY current running AVG "<<fRelPos_Running_AVG[1]<<" is out of range !"<<" current running AVG^2: "<<fRelPos_Running_AVG_square[1]<<"\n Uncertainty in mean "<<fRunning_sigma[1]<<std::endl;
    }
  }
  else
    status = kTRUE;
      
  return status;
}

/********************************************************/

Int_t QwBPMStripline::SetSingleEventCuts(std::vector<Double_t> & dEventCuts){

  fLLimitX=dEventCuts.at(0);
  fULimitX=dEventCuts.at(1);
  fLLimitY=dEventCuts.at(2);
  fULimitY=dEventCuts.at(3);
  fSampleSize=(Int_t)dEventCuts.at(4);
  
  
  
  return 0; 
};


/********************************************************/

void  QwBPMStripline::ProcessEvent()
{
  if(ApplyHWChecks())// IsGoodEvent() is checking for hadware consistency.
    {

      for(int i=0;i<4;i++)
	fWire[i].ProcessEvent();


      static QwVQWK_Channel numer("numerator"), denom("denominator");

      for(int i=0;i<2;i++)
	{
	  numer.Difference(fWire[i*2],fWire[i*2+1]);
	  denom.Sum(fWire[i*2],fWire[i*2+1]);
	  fRelPos[i].Ratio(numer,denom);
	  fRelPos[i].Scale(kQwStriplineCalibration);
	  if(kDEBUG)
	    {
	      std::cout<<" stripline name="<<fElementName<<axis[i];
	      std::cout<<" event number="<<fWire[i*2].GetSequenceNumber()<<"\n";
	      std::cout<<" hw  Wire["<<i*2<<"]="<<fWire[i*2].GetHardwareSum()<<"  ";
	      std::cout<<" hw  Wire["<<i*2+1<<"]="<<fWire[i*2+1].GetHardwareSum()<<"\n";
	      std::cout<<" hw numerator="<<numer.GetHardwareSum()<<"  ";
	      std::cout<<" hw denominator="<<denom.GetHardwareSum()<<"\n";
	      std::cout<<" hw  fRelPos["<<i<<"]="<<fRelPos[i].GetHardwareSum()<<"\n \n";
	    }
	}
      if(bRotated)
	{
	  /* for this one I suppose that the direction [0] is vertical and up,
	     direction[3] is the beam line direction toward the beamdump
	     if rotated than the frame is rotated by 45 deg counter clockwise*/
	  numer=fRelPos[0];
	  denom=fRelPos[1];
	  fRelPos[0].Sum(numer,denom);
	  fRelPos[1].Difference(numer,denom);
	  fRelPos[0].Scale(kRotationCorrection);
	  fRelPos[1].Scale(kRotationCorrection);
	}
      for(int i=0;i<3;i++)
	fAbsPos[i].Offset(fOffset[i]);
      
      fGoodEvent=kTRUE; //this will notify hardware consistency check has succeded
    } 
  else
    fGoodEvent=kFALSE; //this will notify hardware consistency check has failed

  return;
};
/********************************************************/
void QwBPMStripline::Print()
{
  for(int i=0;i<4;i++)
    fWire[i].Print();
  for(int i=0;i<2;i++)
    fRelPos[i].Print();
  return;
}

/********************************************************/
Bool_t QwBPMStripline::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

   Bool_t status=kTRUE;;
  //std::cout<<" wire[0] sequence num "<<fWire[0].GetSequenceNumber()<<" sample size "<<fWire[0].GetNumberOfSamples()<<std::endl;

  if (fSampleSize>0){// if samplesize is 0 then this do not check for hardware check on this BPM.
    
    //std::cout<<"BPM  wire [0 ] sample size "<<fWire[0].GetNumberOfSamples()<<std::endl; 

    for(int i=0;i<4;i++) 
      {
	
	
	//check for wire hw sums returing same values

      	if (fPrevious_HW_Sum[i] != fWire[i].GetRawHardwareSum()){
	  fPrevious_HW_Sum[i] = fWire[i].GetRawHardwareSum();
	  fHW_Sum_Stuck_Counter[i]=0;
	}else
	  fHW_Sum_Stuck_Counter[i]++;//hw_sum is same increment the counter

	//check for the hw_sum is giving the same value
	if (fHW_Sum_Stuck_Counter[i] > 0){
	  if (bDEBUG) std::cout<<" BPM hardware sum is same for more than "<<fHW_Sum_Stuck_Counter[i]<<" time consecutively  "<<std::endl;
	  status&=kFALSE;
	}

	

	status= fWire[i].ApplyHWChecks(); 
	fEventIsGood &= status;      
	status= fWire[i].MatchSequenceNumber(fWire[0].GetSequenceNumber());
      
	fEventIsGood &= status;
	
	status= fWire[i].MatchNumberOfSamples(fSampleSize); //check for sample size

	if (!status){	
	  if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;  
	  if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
	  //std::cout<<" wire[0] sequence num "<<fWire[0].GetSequenceNumber()<<" sample size "<<fWire[0].GetNumberOfSamples()<<std::endl;
	}
	fEventIsGood &= status;
	
      
      }
    if (fEventIsGood){//if values are consistant within each BPMStripline
      fSequenceNo_Prev++;
      if (counter==0 || fWire[0].GetSequenceNumber()==0){//we have a repeating sequence number
	fSequenceNo_Prev=fWire[0].GetSequenceNumber();     
      }
  
      if (!fWire[0].MatchSequenceNumber(fSequenceNo_Prev)){
	//fEventIsGood&=kFALSE; 
	if (bDEBUG) std::cout<<" QwBPMStripline "<<fWire[0].GetElementName()<<" Sequence number incorrect order predicted value= "<<fSequenceNo_Prev<<" Current value= "<< fWire[0].GetSequenceNumber()<<std::endl;     
      }      
      counter++;
    }else
      counter=0;//resetting the counter after IsGoodEvent() a faliure

  /*
  if(!fEventIsGood||kDEBUG)
    {
      std::cerr<<"QwBPMStripline::IsGoodEvent()\n";
      std::cerr<<" test for BPM "<<GetElementName()<<"\n";
      std::cerr<<"Individual wire goodness ";
      for(int i=0;i<4;i++)
	std::cerr<<subelement[i]<<":"<<fWire[i].IsGoodEvent()<<" ;";
      std::cerr<<"\n";
      std::cerr<<" sequence numbers(event number) =";
      for(int i=0;i<4;i++)
	std::cerr<<fWire[i].GetSequenceNumber()<<":";
      std::cerr<<"\n";
      std::cerr<<" number of samples =";
      for(int i=0;i<4;i++)
	std::cerr<<fWire[i].GetNumberOfSamples()<<":";
      std::cerr<<"\n";
      if(!fEventIsGood) std::cerr<<"Unable to construct relative positions \n\n";
    }

  */
  }
  else{
    if (bDEBUG) std::cout<<GetElementName()<<" Ignored BPMStripline "<<std::endl; 
    fEventIsGood=kTRUE;
  }
  return fEventIsGood;
};
/********************************************************/
Int_t QwBPMStripline::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<4)
    fWire[index].ProcessEvBuffer(buffer,word_position_in_buffer);
  else
    std::cerr <<
      "QwBPMStripline::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";

  return word_position_in_buffer;
};
/********************************************************/
void QwBPMStripline::SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  fOffset[0]=Xoffset;
  fOffset[1]=Yoffset;
  fOffset[2]=Zoffset;

  return;
};

void QwBPMStripline::SetSubElementPedestal(Int_t j, Double_t value)
{
  fWire[j].SetPedestal(value);
  return;
}

void QwBPMStripline::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fWire[j].SetCalibrationFactor(value);
  return;
}
/********************************************************/
TString QwBPMStripline::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<4&&subindex>-1)
    thisname=fWire[subindex].GetElementName();
  else
    std::cerr<<"QwBPMStripline::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

UInt_t QwBPMStripline::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=999999;
  for(int i=0;i<4;i++)
    if(subname==subelement[i])localindex=i;

  if(localindex>3)
    std::cerr << "QwBPMStripline::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
};
/********************************************************/
QwBPMStripline& QwBPMStripline::operator= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
  {
    this->bRotated=value.bRotated;
	for(int i=0;i<4;i++)
		this->fWire[i]=value.fWire[i];
	for(int i=0;i<2;i++)
		this->fRelPos[i]=value.fRelPos[i];
	for(int i=0;i<3;i++)
		{
		this->fAbsPos[i]=value.fAbsPos[i];
		this->fOffset[i]=value.fOffset[i];
		}
	}
  return *this;
};

QwBPMStripline& QwBPMStripline::operator+= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
    {
      for(int i=0;i<4;i++)
	this->fWire[i]+=value.fWire[i];
      for(int i=0;i<2;i++)
	this->fRelPos[i]+=value.fRelPos[i];
      for(int i=0;i<3;i++)
	this->fAbsPos[i]+=value.fAbsPos[i];
    }
  return *this;
};

QwBPMStripline& QwBPMStripline::operator-= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
    {
      for(int i=0;i<4;i++)
	this->fWire[i]-=value.fWire[i];
      for(int i=0;i<2;i++)
	this->fRelPos[i]-=value.fRelPos[i];
      for(int i=0;i<3;i++)
	this->fAbsPos[i]-=value.fAbsPos[i];
    }
  return *this;
};


void QwBPMStripline::Sum(QwBPMStripline &value1, QwBPMStripline &value2){
  *this =  value1;
  *this += value2;
};

void QwBPMStripline::Difference(QwBPMStripline &value1, QwBPMStripline &value2){
  *this =  value1;
  *this -= value2;
};

void QwBPMStripline::Ratio(QwBPMStripline &numer, QwBPMStripline &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // stripline is the difference only not the asymmetries
  *this=numer;

//   if (GetElementName()!="")
//     {
//       for(int i=0;i<4;i++)
// 	this->fWire[i].Ratio(numer.fWire[i], denom.fWire[i]);
//       for(int i=0;i<2;i++)
// 	this->fRelPos[i].Ratio(numer.fRelPos[i], denom.fRelPos[i]);
//       for(int i=0;i<3;i++)
// 	this->fAbsPos[i].Ratio(numer.fAbsPos[i], denom.fAbsPos[i]);
//     }

  return;
};


void QwBPMStripline::Scale(Double_t factor)
{
  for(int i=0;i<2;i++)
    {
      fRelPos[i].Scale(factor);
      fAbsPos[i].Scale(factor);
    }
}



/********************************************************/
void QwBPMStripline::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;

  return;
}


void  QwBPMStripline::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";
      SetRootSaveStatus(prefix);

      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].ConstructHistograms(folder, thisprefix);
      for(int i=0;i<2;i++)
 	fRelPos[i].ConstructHistograms(folder, thisprefix);
      // for(int i=0;i<3;i++)
      //  fAbsPos[i].ConstructHistograms(folder, prefix);
    }
  return;
};

void  QwBPMStripline::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].FillHistograms();
      for(int i=0;i<2;i++)
	fRelPos[i].FillHistograms();
      // for(int i=0;i<3;i++)
      //  fAbsPos[i].FillHistograms();
    }
  return;
};

void  QwBPMStripline::DeleteHistograms()
{
  if (GetElementName()=="")
    {
    }
  else
    {
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].DeleteHistograms();
      for(int i=0;i<2;i++)
	fRelPos[i].DeleteHistograms();
    }
  return;
};


void  QwBPMStripline::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";

      SetRootSaveStatus(prefix);
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].ConstructBranchAndVector(tree,thisprefix,values);
      for(int i=0;i<2;i++)
	fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
  return;
};

void  QwBPMStripline::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree.
  } else
    {
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].FillTreeVector(values);
      for(int i=0;i<2;i++)
	fRelPos[i].FillTreeVector(values);

    }
  return;
};

//******************************************************************
void QwBPMStripline::Copy(VQwDataElement *source)
{
  try
    {
     if(typeid(*source)==typeid(*this))
       {
	 QwBPMStripline* input = ((QwBPMStripline*)source);
	 this->fElementName = input->fElementName;
	 this->bRotated = input->bRotated;
	 this->bFullSave = input->bFullSave;
	 for(int i = 0; i < 3; i++)
	   this->fOffset[i] = input->fOffset[i];
	 for(int i = 0; i < 4; i++)
	   this->fWire[i].Copy(&(input->fWire[i]));
	 for(int i = 0; i < 2; i++)
	   this->fRelPos[i].Copy(&(input->fRelPos[i]));
	 for(int i = 0; i < 3; i++)
	   this->fAbsPos[i].Copy(&(input->fAbsPos[i]));
       }
     else
	{
	  TString loc="Standard exception from QwBPMStripline::Copy = "
	    +source->GetElementName()+" "
	    +this->GetElementName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }

  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}


