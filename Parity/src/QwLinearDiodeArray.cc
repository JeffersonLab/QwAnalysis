/**********************************************************\
* File: QwLinearDiodeArray.cc                                          *
*                                                         *
* Author: B. Waidyawansa                                  *
* Time-stamp:  09-14-2010                                 *
\**********************************************************/

#include "QwLinearDiodeArray.h"
#include "QwHistogramHelper.h"
#include "QwParameterFile.h"
#include <stdexcept>


const size_t QwLinearDiodeArray::kMaxElements = 8;

/* Position calibration factor, transform ADC counts in mm*/
const Double_t QwLinearDiodeArray::kQwLinearDiodeArrayCalibration = 18.77;

void  QwLinearDiodeArray::InitializeChannel(TString name)
{
  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");

  //  We don't initialize the photodiode channels yet.
  
  fRelPos[0].InitializeChannel(name+"RelMean","derived");
  fRelPos[1].InitializeChannel(name+"RelVariance","derived");
  
  bFullSave=kTRUE;

  return;
};

void  QwLinearDiodeArray::InitializeChannel(TString subsystem, TString name)
{
  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(subsystem, "QwLinearDiodeArray", name+"_EffectiveCharge","derived");

  //  We don't initialize the photodiode channels yet.

  fRelPos[0].InitializeChannel(subsystem, "QwLinearDiodeArray", name+"RelMean","derived");
  fRelPos[1].InitializeChannel(subsystem, "QwLinearDiodeArray", name+"RelVariance","derived");

  bFullSave=kTRUE;

  return;
};

void QwLinearDiodeArray::ClearEventData()
{
  size_t i=0;

  for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].ClearEventData();

  for(i=0;i<2;i++){
    fRelPos[i].ClearEventData();
  }
  fEffectiveCharge.ClearEventData();

 return;
};


Bool_t QwLinearDiodeArray::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=0;
  for(size_t i=0;i<fPhotodiode.size();i++)
    {
      fDeviceErrorCode|= fPhotodiode[i].ApplyHWChecks();  //OR the error code from each wire
      fEventIsGood &= (fDeviceErrorCode & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within LinearArray terminals photodiode[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" photodiode[ "<<i<<" ] sequence num "<<fPhotodiode[i].GetSequenceNumber()<<" sample size "<<fPhotodiode[i].GetNumberOfSamples()<<std::endl;
    }

  return fEventIsGood;
};


Int_t QwLinearDiodeArray::GetEventcutErrorCounters()
{
  size_t i=0;

  for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].GetEventcutErrorCounters();
  for(i=0;i<2;i++) {
    fRelPos[i].GetEventcutErrorCounters();
  }
  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
};


Bool_t QwLinearDiodeArray::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  size_t i=0;

  //Event cuts for four wires
  for(i=0;i<fPhotodiode.size();i++){
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


void QwLinearDiodeArray::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{
  QwWarning << "QwLinearDiodeArray::SetSingleEventCuts:  "
	    << "Does not do anything yet." << QwLog::endl;
};


void  QwLinearDiodeArray::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel mean, meansqr;
  static QwVQWK_Channel tmp("tmp");
  static QwVQWK_Channel tmp2("tmp2");

  mean.InitializeChannel("mean","raw");
  meansqr.InitializeChannel("meansqr","raw");


  size_t i = 0;


  ApplyHWChecks();
  //first apply HW checks and update HW  error flags. 
  // Calling this routine here and not in ApplySingleEventCuts  
  //makes a difference for a LinearArrays because they have derrived devices.

  fEffectiveCharge.ClearEventData();

  for(i=0;i<fPhotodiode.size();i++){
    fPhotodiode[i].ProcessEvent();
    fEffectiveCharge+=fPhotodiode[i];
  }
  
  if (localdebug) fEffectiveCharge.PrintInfo();
  
  //  First calculate the mean pad position and mean of squared pad position
  //  with respect to the center of the array, in units of pad spacing.
  mean.ClearEventData();
  meansqr.ClearEventData();
  for (i=0;i<fPhotodiode.size();i++){
    Double_t pos = 0.5 + 1.0*i - 0.5*fPhotodiode.size();
    tmp = fPhotodiode[i];
    tmp.Scale(pos);  // Scale for S(i)*pos
    mean+=tmp;
    tmp.Scale(pos);  // Scale again for S(i)*(pos**2)
    meansqr+=tmp;
  }
  fRelPos[0].Ratio(mean,fEffectiveCharge);
  tmp = meansqr;
  meansqr.Ratio(tmp,fEffectiveCharge);
  tmp2.Product(fRelPos[0], fRelPos[0]);

  //  Now calculate the variance
  fRelPos[1].Difference(meansqr,tmp2);

  for(i=0;i<2;i++){
    fRelPos[i].Scale(kQwLinearDiodeArrayCalibration);
    if(localdebug){
      std::cout<<" LinearArray name="<<fElementName<<axis[i];
      std::cout<<" event number= "<<fPhotodiode[i*2].GetSequenceNumber()<<" \n";
      std::cout<<" hw  photodiode["<<i*2<<"]="<<fPhotodiode[i*2].GetHardwareSum()<<"  ";
      std::cout<<" hw  photodiode["<<i*2+1<<"]="<<fPhotodiode[i*2+1].GetHardwareSum()<<"\n";
      std::cout<<" hw numerator= "<<mean.GetHardwareSum()<<"  ";
      std::cout<<" hw denominator= "<<fEffectiveCharge.GetHardwareSum()<<"\n";
      std::cout<<" hw  fRelPos["<<axis[i]<<"]="<<fRelPos[i].GetHardwareSum()<<"\n \n";
    }
  }
  
  return;
};


Int_t QwLinearDiodeArray::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<fPhotodiode.size())
    {
      fPhotodiode[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
    std::cerr <<
      "QwLinearDiodeArray::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";
    }
  return word_position_in_buffer;
};



void QwLinearDiodeArray::PrintValue() const
{
  for (size_t i = 0; i < 2; i++) {
    fRelPos[i].PrintValue();
  }
  return;
};

void QwLinearDiodeArray::PrintInfo() const
{
  size_t i = 0;
  for (i = 0; i < fPhotodiode.size(); i++) fPhotodiode[i].PrintInfo();
  for (i = 0; i < 2; i++) {
    fAbsPos[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();
};


TString QwLinearDiodeArray::GetSubElementName(Int_t subindex)
{
  TString thisname;
  size_t localindex=999999;
  if (subindex>-1) localindex = subindex;

  if(localindex<fPhotodiode.size())
    thisname=fPhotodiode[subindex].GetElementName();
  else
    std::cerr<< "QwLinearDiodeArray::GetSubElementName for "
	     << GetElementName()<<" this subindex, "
	     << subindex << ", doesn't exist \n";

  return thisname;
}

UInt_t QwLinearDiodeArray::GetSubElementIndex(TString subname)
{
  size_t localindex=999999;
  //  Interpret the subname as the pad index.
  if (subname.IsDigit()){
    Int_t tmpval = subname.Atoi();
    if (tmpval>-1) localindex = tmpval;
  }

  if (localindex >=0 && localindex <= kMaxElements){
    //  Resize the array to include this subelement if it doesn't already.
    if (localindex >= fPhotodiode.size()){
      TString name = GetSubsystemName();
      size_t oldsize = fPhotodiode.size();
      fPhotodiode.resize(localindex+1);
      for (size_t i=oldsize; i<fPhotodiode.size(); i++){
	fPhotodiode[i].InitializeChannel(name+subname,"raw");
      }
    }
  } else {
    std::cerr << "QwLinearDiodeArray::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;
    localindex=999999;
  }
  return localindex;
};

void  QwLinearDiodeArray::GetAbsolutePosition()
{
  for(size_t i=0;i<2;i++){
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }

};

QwLinearDiodeArray& QwLinearDiodeArray::operator= (const QwLinearDiodeArray &value)
{
  VQwBPM::operator= (value);

  if (GetElementName()!=""){
    size_t i = 0;
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(i=0;i<fPhotodiode.size();i++) this->fPhotodiode[i]=value.fPhotodiode[i];
    for(i=0;i<2;i++) {
      this->fRelPos[i]=value.fRelPos[i];
    }
  }
  return *this;
};


QwLinearDiodeArray& QwLinearDiodeArray::operator+= (const QwLinearDiodeArray &value)
{

  if (GetElementName()!=""){
    size_t i = 0;
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(i=0;i<fPhotodiode.size();i++) this->fPhotodiode[i]+=value.fPhotodiode[i];
    for(i=0;i<2;i++) {
      this->fRelPos[i]+=value.fRelPos[i];
    }
  }
  return *this;
};

QwLinearDiodeArray& QwLinearDiodeArray::operator-= (const QwLinearDiodeArray &value)
{

  if (GetElementName()!=""){
    size_t i = 0;
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(i=0;i<fPhotodiode.size();i++) this->fPhotodiode[i]-=value.fPhotodiode[i];
    for(i=0;i<2;i++) {
      this->fRelPos[i]-=value.fRelPos[i];
    }
  }
  return *this;
};


void QwLinearDiodeArray::Ratio(QwLinearDiodeArray &numer, QwLinearDiodeArray &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // LinearArray is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
};



void QwLinearDiodeArray::Scale(Double_t factor)
{
  size_t i = 0;
  fEffectiveCharge.Scale(factor);

  for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].Scale(factor);
  for(i=0;i<2;i++){
    fRelPos[i].Scale(factor);
  }
  return;
};


void QwLinearDiodeArray::CalculateRunningAverage()
{
  size_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].CalculateRunningAverage();
  return;
};

void QwLinearDiodeArray::AccumulateRunningSum(const QwLinearDiodeArray& value)
{
  // TODO This is unsafe, see QwBeamline::AccumulateRunningSum
  size_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
  return;
};


void  QwLinearDiodeArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(prefix);
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].ConstructHistograms(folder, thisprefix);
    }
    for(i=0;i<2;i++) {
      fRelPos[i].ConstructHistograms(folder, thisprefix);
    }
  }
  return;
};

void  QwLinearDiodeArray::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fEffectiveCharge.FillHistograms();
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].FillHistograms();
    }
    for(i=0;i<2;i++){
      fRelPos[i].FillHistograms();
    }
  }
  return;
};

void  QwLinearDiodeArray::DeleteHistograms()
{
  if (GetElementName()=="") {
  }
  else {
    fEffectiveCharge.DeleteHistograms();
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].DeleteHistograms();
    }
    for(i=0;i<2;i++) {
      fRelPos[i].DeleteHistograms();
    }
  }
  return;
};


void  QwLinearDiodeArray::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
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
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
    for(i=0;i<2;i++) {
      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }

  }
  return;
};

void  QwLinearDiodeArray::ConstructBranch(TTree *tree, TString &prefix)
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
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
    }
    for(i=0;i<2;i++) {
      fRelPos[i].ConstructBranch(tree,thisprefix);
    }

  }
  return;
};

void  QwLinearDiodeArray::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
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
	size_t i = 0;
	if(bFullSave) {
	  for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
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

void  QwLinearDiodeArray::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fEffectiveCharge.FillTreeVector(values);
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<fPhotodiode.size();i++) fPhotodiode[i].FillTreeVector(values);
    }
    for(i=0;i<2;i++){
      fRelPos[i].FillTreeVector(values);
    }
  }
  return;
};

void QwLinearDiodeArray::Copy(VQwDataElement *source)
{
  try
    {
      if( typeid(*source)==typeid(*this) ) {
       QwLinearDiodeArray* input = ((QwLinearDiodeArray*)source);
       this->fElementName = input->fElementName;
       this->fEffectiveCharge.Copy(&(input->fEffectiveCharge));
       this->bFullSave = input->bFullSave;
       size_t i = 0;
       for(i = 0; i<fPhotodiode.size(); i++) this->fPhotodiode[i].Copy(&(input->fPhotodiode[i]));
       for(i = 0; i<2; i++){
	 this->fRelPos[i].Copy(&(input->fRelPos[i]));
       }
     }
      else {
       TString loc="Standard exception from QwLinearDiodeArray::Copy = "
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

void QwLinearDiodeArray::SetEventCutMode(Int_t bcuts)
{
  size_t i = 0;
  bEVENTCUTMODE=bcuts;
  for (i=0;i<fPhotodiode.size();i++) fPhotodiode[i].SetEventCutMode(bcuts);
  for (i=0;i<2;i++) {
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


void QwLinearDiodeArray::MakeLinearArrayList()
{
  size_t i = 0;

  QwVQWK_Channel qpd_sub_element;

  for(i=0;i<2;i++) {
    qpd_sub_element.ClearEventData();
    qpd_sub_element.Copy(&fRelPos[i]);
    qpd_sub_element = fRelPos[i];
    fLinearArrayElementList.push_back( qpd_sub_element );
  }
  return;
}


std::vector<QwDBInterface> QwLinearDiodeArray::GetDBEntry()
{

  size_t i = 0;
  UShort_t n_qpd_element = 0;

  std::vector <QwDBInterface> row_list;
  row_list.clear();

  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  for(n_qpd_element=0; n_qpd_element<fLinearArrayElementList.size(); n_qpd_element++) {

    row.Reset();
    // the element name and the n (number of measurements in average)
    // is the same in each block and hardwaresum.

    name          = fLinearArrayElementList.at(n_qpd_element).GetElementName();
    beam_n        = fLinearArrayElementList.at(n_qpd_element).GetGoodEventCount();

    // Get HardwareSum average and its error
    avg           = fLinearArrayElementList.at(n_qpd_element).GetHardwareSum();
    err           = fLinearArrayElementList.at(n_qpd_element).GetHardwareSumError();
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
      avg           = fLinearArrayElementList.at(n_qpd_element).GetBlockValue(i);
      err           = fLinearArrayElementList.at(n_qpd_element).GetBlockErrorValue(i);
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

void  QwLinearDiodeArray::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  
  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX / kQwLinearDiodeArrayCalibration) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX / kQwLinearDiodeArrayCalibration) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY / kQwLinearDiodeArrayCalibration) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY / kQwLinearDiodeArrayCalibration) * sumY / 2.0; // = sumY - meanYP;

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


void QwLinearDiodeArray::RandomizeEventData(int helicity, double time)
{
  for (size_t i=0; i<fPhotodiode.size(); i++) fPhotodiode[i].RandomizeEventData(helicity, time);

  return;
};


void QwLinearDiodeArray::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (size_t i=0; i<2; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
};


void QwLinearDiodeArray::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (size_t i=0; i<fPhotodiode.size(); i++) fPhotodiode[i].EncodeEventData(buffer);
};


void QwLinearDiodeArray::SetDefaultSampleSize(Int_t sample_size)
{
  for(size_t i=0;i<fPhotodiode.size();i++) fPhotodiode[i].SetDefaultSampleSize((size_t)sample_size);
  return;
};


void QwLinearDiodeArray::SetSubElementPedestal(Int_t j, Double_t value)
{
  fPhotodiode.at(j).SetPedestal(value);
  return;
}

void QwLinearDiodeArray::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fPhotodiode.at(j).SetCalibrationFactor(value);
  return;
}
