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

  Short_t i = 0;
  for(i=0;i<4;i++) {
    fWire[i].InitializeChannel(name+subelement[i],"raw");
    if(kDEBUG)
      std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
  }

  fWSum.InitializeChannel(name+"WSum","derived");

  for(i=0;i<2;i++) fRelPos[i].InitializeChannel(name+"Rel"+axis[i],"derived");
  for(i=0;i<3;i++) fAbsPos[i].InitializeChannel(name+axis[i],"derived");

  SetElementName(name);
  bFullSave=kTRUE;

  return;
};
/********************************************************/
void QwBPMStripline::ClearEventData()
{
  Short_t i = 0;
  for(i=0;i<4;i++) fWire[i].ClearEventData();
  for(i=0;i<2;i++) fRelPos[i].ClearEventData();
  for(i=0;i<3;i++) fAbsPos[i].ClearEventData();

  fWSum.ClearEventData();

  return;
};
/********************************************************/

Int_t QwBPMStripline::GetEventcutErrorCounters()
{
  Short_t i = 0;
  for(i=0;i<4;i++) fWire[i].GetEventcutErrorCounters();
  for(i=0;i<2;i++) fRelPos[i].GetEventcutErrorCounters();
  for(i=0;i<3;i++) fAbsPos[i].GetEventcutErrorCounters();
  fWSum.GetEventcutErrorCounters();

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
void QwBPMStripline::RandomizeEventData(int helicity, double time)
{
  for (Short_t i=0; i<4; i++) fWire[i].RandomizeEventData(helicity, time);

  return;
};
/********************************************************/
void QwBPMStripline::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  // This needs to be modified to allow setting the position
  for (Short_t i=0; i<2; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
};
/********************************************************/
void QwBPMStripline::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (Short_t i=0; i<4; i++) fWire[i].EncodeEventData(buffer);
};
/********************************************************/

Bool_t QwBPMStripline::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;
  //Event cuts for Relative X & Y
  for(i=0;i<2;i++){

    if (fRelPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      fRelPos[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }

    //update the event cut counters
    fRelPos[i].UpdateHWErrorCounters();

    fDeviceErrorCode|=fRelPos[i].GetEventcutErrorFlag();//Get the Event cut error flag for RelX/Y
  }
  //Event cuts for Absolute X & Y
  for(i=0;i<3;i++){
    if (fAbsPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      fAbsPos[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //update the event cut counters
    fAbsPos[i].UpdateHWErrorCounters();

    fDeviceErrorCode|=fAbsPos[i].GetEventcutErrorFlag();//Get the Event cut error flag for AbsX/Y
  }

  //Event cuts for four wire sum (WSum)

  if (fWSum.ApplySingleEventCuts()){ //for WSum
      status&=kTRUE;
  }
  else{
    fWSum.UpdateEventCutErrorCount();
    status&=kFALSE;
    if (bDEBUG) std::cout<<" WSum event cut failed ";
  }
  //update the event cut counters
  fWSum.UpdateHWErrorCounters();

  //Event cuts for four wires
  for(i=0;i<4;i++){
    if (fWire[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      fWire[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //update the event cut counters
    fWire[i].UpdateHWErrorCounters();

    fDeviceErrorCode|=fWire[i].GetEventcutErrorFlag();//Get the Event cut error flag for wires
  }

  return status;
};


/********************************************************/


void QwBPMStripline::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX){
  //QwMessage<<" QwBPMStripline Event Cuts "<<<<QwLog::endl;
  QwMessage<<" Event Cut Device "<<fElementName;
  if (ch_name=="relx"){//cuts for the relative x and y
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="rely"){//cuts for the relative x and y
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="absx"){//cuts for the relative x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="absy"){//cuts for the relative x and y
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[1].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="absz"){//cuts for the relative x and y
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[2].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="wsum"){//cuts for the relative x and y
    QwMessage<<"WSum LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWSum.SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="xp"){//cuts for the relative x and y
    QwMessage<<"XP LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[0].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="xm"){//cuts for the relative x and y
    QwMessage<<"XM LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[1].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="yp"){//cuts for the relative x and y
    QwMessage<<"YP LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[2].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="ym"){//cuts for the relative x and y
    QwMessage<<"YM LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[3].SetSingleEventCuts(minX,maxX);
  }
};

/********************************************************/

void QwBPMStripline::SetDefaultSampleSize(Int_t sample_size)
{
  for(Short_t i=0;i<4;i++) fWire[i].SetDefaultSampleSize((size_t)sample_size);
  return;
};

/********************************************************/

void  QwBPMStripline::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel numer("numerator"), denom("denominator");


  ApplyHWChecks();//first apply HW checks and update HW  error flags. Calling this routine here and not in ApplySingleEventCuts  makes a difference for a BPMs because they have derrived devices.


  fWSum.ClearEventData();
  Short_t i = 0;
  for(i=0;i<4;i++)
    {
      fWire[i].ProcessEvent();
      fWSum+=fWire[i];
    }

  if (localdebug) fWSum.PrintInfo();



  for(i=0;i<2;i++)
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

  for(i=0;i<2;i++) {
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].Offset(fOffset[i]);
  }
  fAbsPos[2].Offset(fOffset[2]);

  return;
};

/********************************************************/
void QwBPMStripline::PrintValue() const
{
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].PrintValue();
  for (i = 0; i < 2; i++) fAbsPos[i].PrintValue();
}

/********************************************************/
void QwBPMStripline::PrintInfo() const
{
  Short_t i = 0;
  for (i = 0; i < 4; i++) fWire[i].PrintInfo();
  for (i = 0; i < 2; i++) fRelPos[i].PrintInfo();
  for (i = 0; i < 3; i++) fAbsPos[i].PrintInfo();
  fWSum.PrintInfo();
}

/********************************************************/
Bool_t QwBPMStripline::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=0;
  for(Short_t i=0;i<4;i++)
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
    {
      fWire[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
    std::cerr <<
      "QwBPMStripline::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";
    }
  return word_position_in_buffer;
};
/********************************************************/
void QwBPMStripline::SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  fOffset[0]=Xoffset;
  fOffset[1]=Yoffset;
  fOffset[2]=Zoffset;
  //  std::cout<<" offsets X "<<fOffset[0]<<" Y="<<fOffset[1]<<" Z ="<<fOffset[2]<<std::endl;
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
  for(Short_t i=0;i<4;i++) if(subname==subelement[i])localindex=i;

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
    Short_t i = 0;
    for(i=0;i<4;i++) this->fWire[i]=value.fWire[i];
    for(i=0;i<2;i++) this->fRelPos[i]=value.fRelPos[i];
    for(i=0;i<3;i++)
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
      Short_t i = 0;
      this->fWSum+=value.fWSum;
      for(i=0;i<4;i++) this->fWire[i]+=value.fWire[i];
      for(i=0;i<2;i++) this->fRelPos[i]+=value.fRelPos[i];
      for(i=0;i<3;i++) this->fAbsPos[i]+=value.fAbsPos[i];
    }
  return *this;
};

QwBPMStripline& QwBPMStripline::operator-= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
    {
      Short_t i = 0;
      this->fWSum-=value.fWSum;
      for(i=0;i<4;i++) this->fWire[i]-=value.fWire[i];
      for(i=0;i<2;i++) this->fRelPos[i]-=value.fRelPos[i];
      for(i=0;i<3;i++) this->fAbsPos[i]-=value.fAbsPos[i];
    }
  return *this;
};


void QwBPMStripline::Sum(QwBPMStripline &value1, QwBPMStripline &value2)
{
  *this =  value1;
  *this += value2;
};

void QwBPMStripline::Difference(QwBPMStripline &value1, QwBPMStripline &value2)
{
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
//       for(Short_t i=0;i<4;i++)
// 	this->fWire[i].Ratio(numer.fWire[i], denom.fWire[i]);
//       for(Short_t i=0;i<2;i++)
// 	this->fRelPos[i].Ratio(numer.fRelPos[i], denom.fRelPos[i]);
//       for(Short_t i=0;i<3;i++)
// 	this->fAbsPos[i].Ratio(numer.fAbsPos[i], denom.fAbsPos[i]);
//     }

  return;
};


void QwBPMStripline::Scale(Double_t factor)
{
  Short_t i = 0;
  for(i=0;i<2;i++) fRelPos[i].Scale(factor);
  for(i=0;i<3;i++) fAbsPos[i].Scale(factor);
  return;
};

void QwBPMStripline::CalculateRunningAverage()
{
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].CalculateRunningAverage();
  for (i = 0; i < 3; i++) fAbsPos[i].CalculateRunningAverage();
  // No data for z position
  return;
};

void QwBPMStripline::AccumulateRunningSum(const QwBPMStripline& value)
{
  // TODO This is unsafe, see QwBeamline::AccumulateRunningSum
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
  for (i = 0; i < 3; i++) fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  // No data for z position
  return;
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

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    //we calculate the asym_ for the fWSum becasue its an asymmetry and not a difference.
    fWSum.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(prefix);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].ConstructHistograms(folder, thisprefix);
    }
    for(i=0;i<2;i++) fRelPos[i].ConstructHistograms(folder, thisprefix);
    for(i=0;i<3;i++) fAbsPos[i].ConstructHistograms(folder, prefix);
    //No data for z position
  }
  return;
};

void  QwBPMStripline::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fWSum.FillHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].FillHistograms();
    }
    for(i=0;i<2;i++) fRelPos[i].FillHistograms();
    for(i=0;i<3;i++) fAbsPos[i].FillHistograms();
    //No data for z position
  }
  return;
};

void  QwBPMStripline::DeleteHistograms()
{
  if (GetElementName()=="") {
  }
  else {
    fWSum.DeleteHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].DeleteHistograms();
    }
    for(i=0;i<2;i++) fRelPos[i].DeleteHistograms();
    for(i=0;i<3;i++) fAbsPos[i].DeleteHistograms();
  }
  return;
};


void  QwBPMStripline::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(prefix);

    fWSum.ConstructBranchAndVector(tree,prefix,values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
    for(i=0;i<2;i++) fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    for(i=0;i<3;i++) fAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);

  }
  return;
};

void  QwBPMStripline::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fWSum.FillTreeVector(values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].FillTreeVector(values);
    }
    for(i=0;i<2;i++) fRelPos[i].FillTreeVector(values);
    for(i=0;i<3;i++) fAbsPos[i].FillTreeVector(values);
  }
  return;
};

//******************************************************************
void QwBPMStripline::Copy(VQwDataElement *source)
{
  try
    {
     if( typeid(*source)==typeid(*this) ) {
       QwBPMStripline* input = ((QwBPMStripline*)source);
       this->fElementName = input->fElementName;
       this->fWSum.Copy(&(input->fWSum));
       this->bRotated = input->bRotated;
       this->bFullSave = input->bFullSave;
       Short_t i = 0;
       for(i = 0; i<3; i++) this->fOffset[i] = input->fOffset[i];
       for(i = 0; i<4; i++) this->fWire[i].Copy(&(input->fWire[i]));
       for(i = 0; i<2; i++) this->fRelPos[i].Copy(&(input->fRelPos[i]));
       for(i = 0; i<3; i++) this->fAbsPos[i].Copy(&(input->fAbsPos[i]));
     }
     else {
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

void QwBPMStripline::SetEventCutMode(Int_t bcuts)
{
  Short_t i = 0;
  bEVENTCUTMODE=bcuts;
  for (i=0;i<4;i++) fWire[i].SetEventCutMode(bcuts);
  for (i=0;i<2;i++) fRelPos[i].SetEventCutMode(bcuts);
  for (i=0;i<3;i++) fAbsPos[i].SetEventCutMode(bcuts);
  fWSum.SetEventCutMode(bcuts);
}


void QwBPMStripline::MakeBPMList()
{
  UShort_t i = 0;

  QwVQWK_Channel bpm_sub_element;

  for(i=0;i<2;i++) {
    bpm_sub_element.ClearEventData();
    bpm_sub_element.Copy(&fRelPos[i]);
    bpm_sub_element = fRelPos[i];
    fBPMElementList.push_back( bpm_sub_element );
  }

  //   for(i=0;i<3;i++) {
  //     bpm_sub_element.ClearEventData();
  //     bpm_sub_element.Copy(&fAbsPos[i]);
  //     bpm_sub_element = fAbsPos[i];
  //     fBPMElementList.push_back( bpm_sub_element );
  //   }


  return;
}


std::vector<QwDBInterface> QwBPMStripline::GetDBEntry()
{

  UShort_t i = 0;
  UShort_t n_bpm_element = 0;

  std::vector <QwDBInterface> row_list;
  row_list.clear();

  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  for(n_bpm_element=0; n_bpm_element<fBPMElementList.size(); n_bpm_element++) {

    row.Reset();
    // the element name and the n (number of measurements in average)
    // is the same in each block and hardwaresum.

    name          = fBPMElementList.at(n_bpm_element).GetElementName();
    beam_n        = fBPMElementList.at(n_bpm_element).GetGoodEventCount();

    // Get HardwareSum average and its error
    avg           = fBPMElementList.at(n_bpm_element).GetHardwareSum();
    err           = fBPMElementList.at(n_bpm_element).GetHardwareSumError();
    // ADC subblock sum : 0 in MySQL database
    beam_subblock = 0;

    row.SetDetectorName(name);
    row.SetSubblock(beam_subblock);
    row.SetN(beam_n);
    row.SetValue(avg);
    row.SetError(err);

    row_list.push_back(row);

    // Get four Block averages and thier errors

    for(i=0; i<4; i++) {
      row.Reset();
      avg           = fBPMElementList.at(n_bpm_element).GetBlockValue(i);
      err           = fBPMElementList.at(n_bpm_element).GetBlockErrorValue(i);
      beam_subblock = (UInt_t) (i+1);
      // QwVQWK_Channel  | MySQL
      // fBlock[0]       | subblock 1
      // fBlock[1]       | subblock 2
      // fBlock[2]       | subblock 3
      // fBlock[3]       | subblock 4
      row.SetDetectorName(name);
      row.SetSubblock(beam_subblock);
      row.SetN(beam_n);
      row.SetValue(avg);
      row.SetError(err);

      row_list.push_back(row);
    }
  }

  return row_list;

};

