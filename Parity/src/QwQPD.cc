/**********************************************************\
* File: QwQPD.cc                                          *
*                                                         *
* Author: B. Waidyawansa                                  *
* Time-stamp:  09-14-2010                                 *
\**********************************************************/

#include "QwQPD.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

/* Position calibration factor, transform ADC counts in mm*/
const Double_t QwQPD::kQwQPDCalibration = 18.77;
const TString  QwQPD::subelement[4]={"TL","TR","BR","BL"};

void  QwQPD::InitializeChannel(TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");

  for(i=0;i<4;i++) {
    fPhotodiode[i].InitializeChannel(name+subelement[i],"raw");
    
    if(localdebug)
      std::cout<<" photodiode ["<<i<<"]="<<fPhotodiode[i].GetElementName()<<"\n";
  }
  
  for(i=0;i<2;i++) 
    fRelPos[i].InitializeChannel(name+"Rel"+axis[i],"derived");
  
  bFullSave=kTRUE;

  return;
};

void  QwQPD::InitializeChannel(TString subsystem, TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(subsystem, "QwQPD", name+"_EffectiveCharge","derived");

  for(i=0;i<4;i++) {
    fPhotodiode[i].InitializeChannel(subsystem, "QwQPD", name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" photodiode ["<<i<<"]="<<fPhotodiode[i].GetElementName()<<"\n";
  }

  for(i=0;i<2;i++) fRelPos[i].InitializeChannel(subsystem, "QwQPD", name+"Rel"+axis[i],"derived");

  bFullSave=kTRUE;

  return;
};

void QwQPD::ClearEventData()
{
  Short_t i=0;

  for(i=0;i<4;i++) fPhotodiode[i].ClearEventData();

  for(i=0;i<2;i++){
    fRelPos[i].ClearEventData();
  }
  fEffectiveCharge.ClearEventData();

 return;
};


Bool_t QwQPD::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=0;
  for(Short_t i=0;i<4;i++)
    {
      fDeviceErrorCode|= fPhotodiode[i].ApplyHWChecks();  //OR the error code from each wire
      fEventIsGood &= (fDeviceErrorCode & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within QPD terminals photodiode[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" photodiode[ "<<i<<" ] sequence num "<<fPhotodiode[i].GetSequenceNumber()<<" sample size "<<fPhotodiode[i].GetNumberOfSamples()<<std::endl;
    }

  return fEventIsGood;
};


Int_t QwQPD::GetEventcutErrorCounters()
{
  Short_t i=0;

  for(i=0;i<4;i++) fPhotodiode[i].GetEventcutErrorCounters();
  for(i=0;i<2;i++) {
    fRelPos[i].GetEventcutErrorCounters();
  }
  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
};


Bool_t QwQPD::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;

  //Event cuts for four wires
  for(i=0;i<4;i++){
    if (fPhotodiode[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      fPhotodiode[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //update the event cut counters
    fPhotodiode[i].UpdateHWErrorCounters();
    //Get the Event cut error flag for wires
    fDeviceErrorCode|=fPhotodiode[i].GetEventcutErrorFlag();
  }

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
    //Get the Event cut error flag for RelX/Y
    fDeviceErrorCode|=fRelPos[i].GetEventcutErrorFlag();
  }

 //Event cuts for four wire sum (EffectiveCharge)
  if (fEffectiveCharge.ApplySingleEventCuts()){
      status&=kTRUE;
  }
  else{
    fEffectiveCharge.UpdateEventCutErrorCount();
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }
  //update the event cut counters
  fEffectiveCharge.UpdateHWErrorCounters();
  //Get the Event cut error flag for EffectiveCharge
  fDeviceErrorCode|=fEffectiveCharge.GetEventcutErrorFlag();


  return status;

};


void QwQPD::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="tl"){
    QwMessage<<"TL LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="tr"){
    QwMessage<<"TR LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="br"){
    QwMessage<<"BR LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[2].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="bl"){
    QwMessage<<"BL LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[3].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(minX,maxX);
  }else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(minX,maxX);

  }

};


void  QwQPD::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel numer[2];
  static QwVQWK_Channel tmp("tmp");

  numer[0].InitializeChannel("Xnumerator","raw");
  numer[1].InitializeChannel("Ynumerator","raw");

  Short_t i = 0;


  ApplyHWChecks();
  //first apply HW checks and update HW  error flags. 
  // Calling this routine here and not in ApplySingleEventCuts  
  //makes a difference for a QPDs because they have derrived devices.

  fEffectiveCharge.ClearEventData();

  for(i=0;i<4;i++){
    fPhotodiode[i].ProcessEvent();
    fEffectiveCharge+=fPhotodiode[i];
  }
  
  if (localdebug) fEffectiveCharge.PrintInfo();
   
  for(i=0;i<2;i++){
    numer[i].ClearEventData();
    tmp.ClearEventData();
    numer[i].Difference(fPhotodiode[0],fPhotodiode[2]);
    tmp.Difference(fPhotodiode[1],fPhotodiode[3]);
    tmp.Scale((2*i-1));
    numer[i]+=tmp;
    
    fRelPos[i].Ratio(numer[i],fEffectiveCharge);
    fRelPos[i].Scale(kQwQPDCalibration);
    if(localdebug){
      std::cout<<" QPD name="<<fElementName<<axis[i];
      std::cout<<" event number= "<<fPhotodiode[i*2].GetSequenceNumber()<<" \n";
      std::cout<<" hw  photodiode["<<i*2<<"]="<<fPhotodiode[i*2].GetHardwareSum()<<"  ";
      std::cout<<" hw  photodiode["<<i*2+1<<"]="<<fPhotodiode[i*2+1].GetHardwareSum()<<"\n";
      std::cout<<" hw numerator= "<<numer[i].GetHardwareSum()<<"  ";
      std::cout<<" hw denominator= "<<fEffectiveCharge.GetHardwareSum()<<"\n";
      std::cout<<" hw  fRelPos["<<axis[i]<<"]="<<fRelPos[i].GetHardwareSum()<<"\n \n";
    }
  }
  
  return;
};


Int_t QwQPD::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<4)
    {
      fPhotodiode[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
    std::cerr <<
      "QwQPD::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";
    }
  return word_position_in_buffer;
};



void QwQPD::PrintValue() const
{
  for (Short_t i = 0; i < 2; i++) {
    fRelPos[i].PrintValue();
  }
  return;
};

void QwQPD::PrintInfo() const
{
  Short_t i = 0;
  for (i = 0; i < 4; i++) fPhotodiode[i].PrintInfo();
  for (i = 0; i < 2; i++) {
    fAbsPos[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();
};


TString QwQPD::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<4&&subindex>-1)
    thisname=fPhotodiode[subindex].GetElementName();
  else
    std::cerr<<"QwQPD::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

UInt_t QwQPD::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=999999;
  for(Short_t i=0;i<4;i++) if(subname==subelement[i])localindex=i;

  if(localindex>3)
    std::cerr << "QwQPD::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
};

void  QwQPD::GetAbsolutePosition()
{
  for(Short_t i=0;i<2;i++){
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }

};

QwQPD& QwQPD::operator= (const QwQPD &value)
{
  VQwBPM::operator= (value);

  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fPhotodiode[i]=value.fPhotodiode[i];
    for(i=0;i<2;i++) {
      this->fRelPos[i]=value.fRelPos[i];
    }
  }
  return *this;
};


QwQPD& QwQPD::operator+= (const QwQPD &value)
{

  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fPhotodiode[i]+=value.fPhotodiode[i];
    for(i=0;i<2;i++) {
      this->fRelPos[i]+=value.fRelPos[i];
    }
  }
  return *this;
};

QwQPD& QwQPD::operator-= (const QwQPD &value)
{

  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fPhotodiode[i]-=value.fPhotodiode[i];
    for(i=0;i<2;i++) {
      this->fRelPos[i]-=value.fRelPos[i];
    }
  }
  return *this;
};


void QwQPD::Ratio(QwQPD &numer, QwQPD &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // QPD is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
};



void QwQPD::Scale(Double_t factor)
{
  Short_t i = 0;
  fEffectiveCharge.Scale(factor);

  for(i=0;i<4;i++) fPhotodiode[i].Scale(factor);
  for(Short_t i=0;i<2;i++){
    fRelPos[i].Scale(factor);
  }
  return;
};


void QwQPD::CalculateRunningAverage()
{
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].CalculateRunningAverage();
  return;
};

void QwQPD::AccumulateRunningSum(const QwQPD& value)
{
  // TODO This is unsafe, see QwBeamline::AccumulateRunningSum
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
  return;
};


void  QwQPD::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(prefix);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].ConstructHistograms(folder, thisprefix);
    }
    for(i=0;i<2;i++) {
      fRelPos[i].ConstructHistograms(folder, thisprefix);
    }
  }
  return;
};

void  QwQPD::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fEffectiveCharge.FillHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].FillHistograms();
    }
    for(i=0;i<2;i++){
      fRelPos[i].FillHistograms();
    }
  }
  return;
};

void  QwQPD::DeleteHistograms()
{
  if (GetElementName()=="") {
  }
  else {
    fEffectiveCharge.DeleteHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].DeleteHistograms();
    }
    for(i=0;i<2;i++) {
      fRelPos[i].DeleteHistograms();
    }
  }
  return;
};


void  QwQPD::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
    for(i=0;i<2;i++) {
      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }

  }
  return;
};

void  QwQPD::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranch(tree,prefix);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
    }
    for(i=0;i<2;i++) {
      fRelPos[i].ConstructBranch(tree,thisprefix);
    }

  }
  return;
};

void  QwQPD::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      if (modulelist.HasValue(devicename)){
	TString thisprefix=prefix;
	if(prefix=="asym_")
	  thisprefix="diff_";

	SetRootSaveStatus(prefix);

	fEffectiveCharge.ConstructBranch(tree,prefix);
	Short_t i = 0;
	if(bFullSave) {
	  for(i=0;i<4;i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
	}
	for(i=0;i<2;i++) {
	  fRelPos[i].ConstructBranch(tree,thisprefix);
	}

	QwMessage <<" Tree leaves added to "<<devicename<<" Corresponding channels"<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }





  return;
};

void  QwQPD::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fEffectiveCharge.FillTreeVector(values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].FillTreeVector(values);
    }
    for(i=0;i<2;i++){
      fRelPos[i].FillTreeVector(values);
    }
  }
  return;
};

void QwQPD::Copy(VQwDataElement *source)
{
  try
    {
      if( typeid(*source)==typeid(*this) ) {
       QwQPD* input = ((QwQPD*)source);
       this->fElementName = input->fElementName;
       this->fEffectiveCharge.Copy(&(input->fEffectiveCharge));
       this->bFullSave = input->bFullSave;
       Short_t i = 0;
       for(i = 0; i<4; i++) this->fPhotodiode[i].Copy(&(input->fPhotodiode[i]));
       for(i = 0; i<2; i++){
	 this->fRelPos[i].Copy(&(input->fRelPos[i]));
       }
     }
      else {
       TString loc="Standard exception from QwQPD::Copy = "
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

void QwQPD::SetEventCutMode(Int_t bcuts)
{
  Short_t i = 0;
  bEVENTCUTMODE=bcuts;
  for (i=0;i<4;i++) fPhotodiode[i].SetEventCutMode(bcuts);
  for (i=0;i<2;i++) {
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


void QwQPD::MakeQPDList()
{
  UShort_t i = 0;

  QwVQWK_Channel qpd_sub_element;

  for(i=0;i<2;i++) {
    qpd_sub_element.ClearEventData();
    qpd_sub_element.Copy(&fRelPos[i]);
    qpd_sub_element = fRelPos[i];
    fQPDElementList.push_back( qpd_sub_element );
  }
  return;
}


std::vector<QwDBInterface> QwQPD::GetDBEntry()
{

  UShort_t i = 0;
  UShort_t n_qpd_element = 0;

  std::vector <QwDBInterface> row_list;
  row_list.clear();

  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  for(n_qpd_element=0; n_qpd_element<fQPDElementList.size(); n_qpd_element++) {

    row.Reset();
    // the element name and the n (number of measurements in average)
    // is the same in each block and hardwaresum.

    name          = fQPDElementList.at(n_qpd_element).GetElementName();
    beam_n        = fQPDElementList.at(n_qpd_element).GetGoodEventCount();

    // Get HardwareSum average and its error
    avg           = fQPDElementList.at(n_qpd_element).GetHardwareSum();
    err           = fQPDElementList.at(n_qpd_element).GetHardwareSumError();
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
      avg           = fQPDElementList.at(n_qpd_element).GetBlockValue(i);
      err           = fQPDElementList.at(n_qpd_element).GetBlockErrorValue(i);
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

/**********************************
 * Mock data generation routines
 **********************************/

void  QwQPD::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  
  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX / kQwQPDCalibration) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX / kQwQPDCalibration) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY / kQwQPDCalibration) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY / kQwQPDCalibration) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is not tested yet)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  Double_t sigmaXM = sigmaXP;
  Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  Double_t sigmaYM = sigmaYP;

  // Propagate these parameters to the ADCs
  fPhotodiode[0].SetRandomEventParameters(meanXP, sigmaXP);
  fPhotodiode[1].SetRandomEventParameters(meanXM, sigmaXM);
  fPhotodiode[2].SetRandomEventParameters(meanYP, sigmaYP);
  fPhotodiode[3].SetRandomEventParameters(meanYM, sigmaYM);
};


void QwQPD::RandomizeEventData(int helicity, double time)
{
  for (Short_t i=0; i<4; i++) fPhotodiode[i].RandomizeEventData(helicity, time);

  return;
};


void QwQPD::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (Short_t i=0; i<2; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
};


void QwQPD::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (Short_t i=0; i<4; i++) fPhotodiode[i].EncodeEventData(buffer);
};


void QwQPD::SetDefaultSampleSize(Int_t sample_size)
{
  for(Short_t i=0;i<4;i++) fPhotodiode[i].SetDefaultSampleSize((size_t)sample_size);
  return;
};


void QwQPD::SetSubElementPedestal(Int_t j, Double_t value)
{
  fPhotodiode[j].SetPedestal(value);
  return;
}

void QwQPD::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fPhotodiode[j].SetCalibrationFactor(value);
  return;
}
