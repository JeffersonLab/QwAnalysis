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
  
  
  for(int i=0;i<4;i++)
    {
      fWire[i].InitializeChannel(name+subelement[i],"raw");
      if(kDEBUG)
	std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
    }

  fWSum.InitializeChannel(name+"WSum","derived");


  for(int i=0;i<2;i++)
    fRelPos[i].InitializeChannel(name+"Rel"+axis[i],"derived");	
  


  for(int i=0;i<3;i++)
    fAbsPos[i].InitializeChannel(name+axis[i],"derived");
 
  SetElementName(name);
  bFullSave=kTRUE;

  //set default limits to event cuts 
  fULimitX=0;
  fLLimitX=0;
  fULimitY=0;
  fLLimitY=0;

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

  fWSum.ClearEventData();

  return;
};
/********************************************************/

Int_t QwBPMStripline::GetEventcutErrorCounters(){
  for(int i=0;i<4;i++)
    fWire[i].GetEventcutErrorCounters();

     
  //std::cout<<"RelX ";
  fRelPos[0].GetEventcutErrorCounters();
  //std::cout<<"RelY ";
  fRelPos[1].GetEventcutErrorCounters();

  return 1;
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
  
  ApplyHWChecks();//first apply HW checks and update HW  error flags.

    if (fRelPos[0].ApplySingleEventCuts(fLLimitX,fULimitX)){ //for RelX  
      status=kTRUE;
    }
    else{
      fRelPos[0].UpdateEventCutErrorCount();
      status=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
    fDeviceErrorCode|=fRelPos[0].GetEventcutErrorFlag();//Get the Event cut error flag for RelX
    //if (fRelPos[1].GetHardwareSum()<=fULimitY && fRelPos[1].GetHardwareSum()>=fLLimitY){//for RelY
    if (fRelPos[1].ApplySingleEventCuts(fLLimitY,fULimitY)){
      status&=kTRUE;
      //std::cout<<" ";
    }
    else{
      fRelPos[1].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel Y event cut failed ";
    }
    fDeviceErrorCode|=fRelPos[1].GetEventcutErrorFlag();//Get the Event cut error flag for RelY
	
    

  
  
  
  return status;
};


/********************************************************/




Int_t QwBPMStripline::SetSingleEventCuts(Double_t minX, Double_t maxX, Double_t minY, Double_t maxY ){

  fLLimitX=minX;
  fULimitX=maxX;
  fLLimitY=minY;
  fULimitY=maxY;
  
  return 0; 
};




/********************************************************/

void QwBPMStripline::SetDefaultSampleSize(Int_t sample_size){

  for(int i=0;i<4;i++)
    fWire[i].SetDefaultSampleSize((size_t)sample_size);
  
  
};

/********************************************************/

void  QwBPMStripline::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel numer("numerator"), denom("denominator");
  
  


  fWSum.ClearEventData();
      
  for(int i=0;i<4;i++){
    fWire[i].ProcessEvent();
    fWSum+=fWire[i];
  }

  if (localdebug) fWSum.Print();

     
  
  for(int i=0;i<2;i++)
    {
      numer.Difference(fWire[i*2],fWire[i*2+1]);
      denom.Sum(fWire[i*2],fWire[i*2+1]);
      fRelPos[i].Ratio(numer,denom);
      fRelPos[i].Scale(kQwStriplineCalibration);
      if(kDEBUG)
	{
	  std::cout<<" stripline name="<<fElementName<<axis[i];
	  std::cout<<" event number= "<<fWire[i*2].GetSequenceNumber()<<" \n";
	  std::cout<<" hw  Wire["<<i*2<<"]="<<fWire[i*2].GetHardwareSum()<<"  ";
	  std::cout<<" hw  Wire["<<i*2+1<<"]="<<fWire[i*2+1].GetHardwareSum()<<"\n";
	  std::cout<<" hw numerator= "<<numer.GetHardwareSum()<<"  ";
	  std::cout<<" hw denominator= "<<denom.GetHardwareSum()<<"\n";
	  std::cout<<" hw  fRelPos["<<axis[i]<<"]="<<fRelPos[i].GetHardwareSum()<<"\n \n";
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
  

  return;
};
/********************************************************/
void QwBPMStripline::Print()
{
  for(int i=0;i<4;i++)
    fWire[i].Print();
  for(int i=0;i<2;i++)
    fRelPos[i].Print();
  fWSum.Print();

  return;
}

/********************************************************/
Bool_t QwBPMStripline::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;
   
  fDeviceErrorCode=0;
  for(int i=0;i<4;i++) 
    {
      fDeviceErrorCode|= fWire[i].ApplyHWChecks();  //OR the error code from each wire
      fEventIsGood &= (fDeviceErrorCode & 0x0);//AND with 0 since zero means HW is good.	
      
      if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;  
      if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
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
    this->fWSum=value.fWSum;

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
      this->fWSum+=value.fWSum;
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
      this->fWSum-=value.fWSum;
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
  this->fWSum.Ratio(numer.fWSum,denom.fWSum);
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

void QwBPMStripline::Calculate_Running_Average(){
  for(int i=0;i<2;i++)
    fRelPos[i].Calculate_Running_Average();      

  for(int i=0;i<3;i++)
    fAbsPos[i].Calculate_Running_Average();
};

void QwBPMStripline::Do_RunningSum(){
  for(int i=0;i<2;i++)
    fRelPos[i].Do_RunningSum();
      
  for(int i=0;i<3;i++)
    fAbsPos[i].Do_RunningSum();
};


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
      //we calculate the asym_ for the fWSum becasue its an asymmetry and not a difference.
      fWSum.ConstructHistograms(folder, prefix);    
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
      fWSum.FillHistograms();
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
      fWSum.DeleteHistograms();
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

      fWSum.ConstructBranchAndVector(tree,prefix,values);

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
      fWSum.FillTreeVector(values);
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
	 this->fWSum.Copy(&(input->fWSum));
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

void QwBPMStripline::SetEventCutMode(Int_t bcuts){
  bEVENTCUTMODE=bcuts;
  //std::cout<<GetElementName()<<" Event Cut Mode "<<bcuts<<std::endl;
  for (Int_t i=0;i<4;i++)
    fWire[i].SetEventCutMode(bcuts);
  for (Int_t i=0;i<2;i++)
    fRelPos[i].SetEventCutMode(bcuts);
  for (Int_t i=0;i<2;i++)
    fAbsPos[i].SetEventCutMode(bcuts);
  fWSum.SetEventCutMode(bcuts);
}




TString QwBPMStripline::GetSubElementName(TString type)
{
  TString tmp;

  tmp = "dummy";

  if( type.Contains("Rel", TString::kIgnoreCase) )
    {
      if      ( type.Contains("X", TString::kIgnoreCase) ) tmp = fRelPos[0].GetElementName();
      else if ( type.Contains("Y", TString::kIgnoreCase) ) tmp = fRelPos[1].GetElementName();
      else   ; // tmp = "dummy";
    }
  else if( type.Contains("Abs", TString::kIgnoreCase) )
    {
      if      ( type.Contains("X", TString::kIgnoreCase) ) tmp = fAbsPos[0].GetElementName();
      else if ( type.Contains("Y", TString::kIgnoreCase) ) tmp = fAbsPos[1].GetElementName();
      else if ( type.Contains("Z", TString::kIgnoreCase) ) tmp = fAbsPos[2].GetElementName();
      else   ; // tmp = "dummy";
    }

  if(tmp == "dummy") printf("Please, check what you ask for, it is out of range.\n");

  return tmp;
}




Double_t QwBPMStripline::GetAverage(TString type)
{
  Double_t tmp = -9999;

  if( type.Contains("Rel", TString::kIgnoreCase) )
    {
      if      ( type.Contains("X", TString::kIgnoreCase) ) tmp = fRelPos[0].GetAverage();
      else if ( type.Contains("Y", TString::kIgnoreCase) ) tmp = fRelPos[1].GetAverage();
      else                                  ;
    }
  else if( type.Contains("Abs", TString::kIgnoreCase) )
    {
      if      ( type.Contains("X", TString::kIgnoreCase) ) tmp = fAbsPos[0].GetAverage();
      else if ( type.Contains("Y", TString::kIgnoreCase) ) tmp = fAbsPos[1].GetAverage();
      else if ( type.Contains("Z", TString::kIgnoreCase) ) tmp = fAbsPos[2].GetAverage();
      else                                  ;
    }



  if(tmp == -9999) printf("GerAverage Error QwBOMStripline.cc : Please, check what you ask for, it is out of range.\n");

  return tmp;
}




Double_t QwBPMStripline::GetAverageError(TString type)
{
  Double_t tmp = -9999;

  if( type.Contains("Rel", TString::kIgnoreCase) )
    {
      if      ( type.Contains("X", TString::kIgnoreCase) ) tmp = fRelPos[0].GetAverageError();
      else if ( type.Contains("Y", TString::kIgnoreCase) ) tmp = fRelPos[1].GetAverageError();
      else                                  ;
    }
  else if( type.Contains("Abs", TString::kIgnoreCase) )
    {
      if      ( type.Contains("X", TString::kIgnoreCase) ) tmp = fAbsPos[0].GetAverageError();
      else if ( type.Contains("Y", TString::kIgnoreCase) ) tmp = fAbsPos[1].GetAverageError();
      else if ( type.Contains("Z", TString::kIgnoreCase) ) tmp = fAbsPos[2].GetAverageError();
      else                                  ;
    }

  if(tmp == -9999) printf("GerAverageError Error QwBOMStripline.cc : Please, check what you ask for, it is out of range.\n");

  return tmp;
}



QwParityDB::beam QwBPMStripline::GetDBEntry(QwDatabase *db, TString mtype, TString subname)
{
  QwParityDB::beam row(0);
  
  
  // Get run and analysis id from db, which is sent by "main"
  
  UInt_t run_id      = db ->GetRunID();
  UInt_t analysis_id = db ->GetAnalysisID(); 
  UInt_t monitor_id  = 0;     // monitor_id | int(10) unsigned | NO   | PRI | NULL    |
  Char_t measurement_type[4];

  TString name;
  Double_t avg = 0.0;
  Double_t err = 0.0;

  name = this->GetSubElementName(subname);
  
 
  if(mtype.Contains("yield"))
    {
      sprintf(measurement_type, "yq");
    }
  else if(mtype.Contains("asymmetry"))
    {
      sprintf(measurement_type, "aq");
    }
  else if(mtype.Contains("average") )
    {
      sprintf(measurement_type, "yq");
    }
  else if(mtype.Contains("runningsum"))
    {
      sprintf(measurement_type, "yq");
    }
  else
    {
      sprintf(measurement_type, "null");
    }
  
  name = this -> GetSubElementName(subname);
  avg  = this -> GetAverage(subname);
  err  = this -> GetAverageError(subname);
     
	  
  if      ( name.Contains("qpdRelX")   ) monitor_id = QWK_XQPD;
  else if ( name.Contains("1i02RelX")  ) monitor_id = QWK_1I02X;
  else if ( name.Contains("1i04RelX")  ) monitor_id = QWK_1I04X;
  else if ( name.Contains("1i06RelX")  ) monitor_id = QWK_1I06X;
  else if ( name.Contains("0i02RelX")  ) monitor_id = QWK_0I02X;
  else if ( name.Contains("0i02aRelX") ) monitor_id = QWK_0I02AX;
  else if ( name.Contains("0i05RelX")  ) monitor_id = QWK_0i05X;
  else if ( name.Contains("0i07RelX")  ) monitor_id = QWK_0i07X;
  else if ( name.Contains("0l01RelX")  ) monitor_id = QWK_0L01X;
  else if ( name.Contains("0l02RelX")  ) monitor_id = QWK_0L02X;
  else if ( name.Contains("0l03RelX")  ) monitor_id = QWK_0L03X;
  else if ( name.Contains("0l04RelX")  ) monitor_id = QWK_0L04X;
  else if ( name.Contains("0l05RelX")  ) monitor_id = QWK_0L05X;
  else if ( name.Contains("0l06RelX")  ) monitor_id = QWK_0L06X;
  else if ( name.Contains("0l07RelX")  ) monitor_id = QWK_0L07X;
  else if ( name.Contains("0l08RelX")  ) monitor_id = QWK_0L08X;
  else if ( name.Contains("0l09RelX")  ) monitor_id = QWK_0L09X;
  else if ( name.Contains("0l10RelX")  ) monitor_id = QWK_0L10X;
  else if ( name.Contains("0r01RelX")  ) monitor_id = QWK_0R01X;
  else if ( name.Contains("0r02RelX")  ) monitor_id = QWK_0R02X;
  else if ( name.Contains("0r05RelX")  ) monitor_id = QWK_0R05X;
  else if ( name.Contains("0r06RelX")  ) monitor_id = QWK_0R06X;

  else if ( name.Contains("qpdRelY")   ) monitor_id = QWK_YQPD;
  else if ( name.Contains("1i02RelY")  ) monitor_id = QWK_1I02Y;
  else if ( name.Contains("1i04RelY")  ) monitor_id = QWK_1I04Y;
  else if ( name.Contains("1i06RelY")  ) monitor_id = QWK_1I06Y;
  else if ( name.Contains("0i02RelY")  ) monitor_id = QWK_0I02Y;
  else if ( name.Contains("0i02aRelY") ) monitor_id = QWK_0I02AY;
  else if ( name.Contains("0i05RelY")  ) monitor_id = QWK_0i05Y;
  else if ( name.Contains("0i07RelY")  ) monitor_id = QWK_0i07Y;
  else if ( name.Contains("0l01RelY")  ) monitor_id = QWK_0L01Y;
  else if ( name.Contains("0l02RelY")  ) monitor_id = QWK_0L02Y;
  else if ( name.Contains("0l03RelY")  ) monitor_id = QWK_0L03Y;
  else if ( name.Contains("0l04RelY")  ) monitor_id = QWK_0L04Y;
  else if ( name.Contains("0l05RelY")  ) monitor_id = QWK_0L05Y;
  else if ( name.Contains("0l06RelY")  ) monitor_id = QWK_0L06Y;
  else if ( name.Contains("0l07RelY")  ) monitor_id = QWK_0L07Y;
  else if ( name.Contains("0l08RelY")  ) monitor_id = QWK_0L08Y;
  else if ( name.Contains("0l09RelY")  ) monitor_id = QWK_0L09Y;
  else if ( name.Contains("0l10RelY")  ) monitor_id = QWK_0L10Y;
  else if ( name.Contains("0r01RelY")  ) monitor_id = QWK_0R01Y;
  else if ( name.Contains("0r02RelY")  ) monitor_id = QWK_0R02Y;
  else if ( name.Contains("0r05RelY")  ) monitor_id = QWK_0R05Y;
  else if ( name.Contains("0r06RelY")  ) monitor_id = QWK_0R06Y;
  else                                   monitor_id = QWK_UNDEFINED;
      
  row.monitor_id          = monitor_id;
  row.analysis_id         = analysis_id;
  row.measurement_type_id = measurement_type;
  row.value               = avg;
  row.error               = err;
  printf("%s::RunID %d AnalysisID %d %4s MonitorID %4d %18s , [%18.2e, %12.2e] \n", 
	 mtype.Data(), run_id, analysis_id, measurement_type, monitor_id, name.Data(),  avg, err);

 return row;
  
};
