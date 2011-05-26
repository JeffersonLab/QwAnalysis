/**********************************************************\
* File: QwIntegratedRasterChannel.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegratedRasterChannel.h"

// System headers
#include <stdexcept>

// Qweak database headers
#include "QwDBInterface.h"

/********************************************************/
void QwIntegratedRasterChannel::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
}

void QwIntegratedRasterChannel::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration);
	return;
}
/********************************************************/
void  QwIntegratedRasterChannel::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
  return;
}
/********************************************************/
void  QwIntegratedRasterChannel::InitializeChannel(TString subsystem, TString name, TString datatosave){
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(subsystem, "QwIntegratedRasterChannel", name, datatosave);
  SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
  return;  
}
/********************************************************/
void QwIntegratedRasterChannel::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
}


/********************************************************/
void QwIntegratedRasterChannel::UseExternalRandomVariable()
{
  fTriumf_ADC.UseExternalRandomVariable();
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::SetExternalRandomVariable(double random_variable)
{
  fTriumf_ADC.SetExternalRandomVariable(random_variable);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::RandomizeEventData(int helicity, double time)
{
  fTriumf_ADC.RandomizeEventData(helicity, time);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
}
/********************************************************/
void QwIntegratedRasterChannel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
}
/********************************************************/
void  QwIntegratedRasterChannel::ProcessEvent()
{
  ApplyHWChecks();//first apply HW checks and update HW  error flags. Calling this routine either in ApplySingleEventCuts or here do not make any difference for a BCM but do for a BPMs because they have derrived devices.
  fTriumf_ADC.ProcessEvent();
  return;
}
/********************************************************/
Bool_t QwIntegratedRasterChannel::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=fTriumf_ADC.ApplyHWChecks();//will check for HW consistancy and return the error code (=0 is HW good)
  fEventIsGood=(fDeviceErrorCode & 0x0);//if no HW error return true


  return fEventIsGood;
}
/********************************************************/

Int_t QwIntegratedRasterChannel::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){
  fTriumf_ADC.SetSingleEventCuts(LL,UL);
  return 1;
}

/********************************************************/

void QwIntegratedRasterChannel::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kBCMErrorFlag;
  QwMessage<<"QwIntegratedRasterChannel Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fTriumf_ADC.SetSingleEventCuts(errorflag,LL,UL,stability);

}

void QwIntegratedRasterChannel::SetDefaultSampleSize(Int_t sample_size){
  fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}


/********************************************************/
Bool_t QwIntegratedRasterChannel::ApplySingleEventCuts(){
  //std::cout<<" QwIntegratedRasterChannel::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;


  if (fTriumf_ADC.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{

    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fTriumf_ADC.GetHardwareSum();
    status&=kFALSE;
  }
  fDeviceErrorCode|=fTriumf_ADC.GetEventcutErrorFlag();//retrun the error flag for event cuts


  return status;

}

/********************************************************/

Int_t QwIntegratedRasterChannel::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  fTriumf_ADC.GetEventcutErrorCounters();
  return 1;
}



Int_t QwIntegratedRasterChannel::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
}
/********************************************************/
QwIntegratedRasterChannel& QwIntegratedRasterChannel::operator= (const QwIntegratedRasterChannel &value)
{
//   std::cout<<" Here in QwIntegratedRasterChannel::operator= \n";
  if (GetElementName()!="")
    {
      this->fTriumf_ADC=value.fTriumf_ADC;
      this->fPedestal=value.fPedestal;
      this->fCalibration=value.fCalibration;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
}

QwIntegratedRasterChannel& QwIntegratedRasterChannel::operator+= (const QwIntegratedRasterChannel &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

QwIntegratedRasterChannel& QwIntegratedRasterChannel::operator-= (const QwIntegratedRasterChannel &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}


void QwIntegratedRasterChannel::Sum(QwIntegratedRasterChannel &value1, QwIntegratedRasterChannel &value2){
  *this =  value1;
  *this += value2;
}

void QwIntegratedRasterChannel::Difference(QwIntegratedRasterChannel &value1, QwIntegratedRasterChannel &value2){
  *this =  value1;
  *this -= value2;
}

void QwIntegratedRasterChannel::Ratio(QwIntegratedRasterChannel &numer, QwIntegratedRasterChannel &denom)
{
  *this =  numer;
  return;
}

void QwIntegratedRasterChannel::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}

void QwIntegratedRasterChannel::CalculateRunningAverage() {
  fTriumf_ADC.CalculateRunningAverage();
}

void QwIntegratedRasterChannel::AccumulateRunningSum(const QwIntegratedRasterChannel& value) {
  fTriumf_ADC.AccumulateRunningSum(value.fTriumf_ADC);
}


void QwIntegratedRasterChannel::PrintValue() const
{
  fTriumf_ADC.PrintValue();
}

void QwIntegratedRasterChannel::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fTriumf_ADC.PrintInfo();
}

/********************************************************/
void  QwIntegratedRasterChannel::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.ConstructHistograms(folder, prefix);
    }
  return;
}

void  QwIntegratedRasterChannel::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.FillHistograms();
    }


  return;
}

void  QwIntegratedRasterChannel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip
  } else
    {    
    TString thisprefix=prefix;
    if(prefix=="asym_"){
      if (GetElementName()=="qwk_raster_x"||GetElementName()=="qwk_raster_y"||GetElementName()=="qwk_raster_sumxy") thisprefix="diff_";
    }
      fTriumf_ADC.ConstructBranchAndVector(tree, thisprefix,values);
    }
  return;
}

void  QwIntegratedRasterChannel::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranch(tree, prefix);
      // this functions doesn't do anything yet
    }
  return;
}

void  QwIntegratedRasterChannel::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {

      //QwMessage <<" QwIntegratedRasterChannel "<<devicename<<QwLog::endl;
      if (modulelist.HasValue(devicename)){
	fTriumf_ADC.ConstructBranch(tree, prefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }
  return;
}

void  QwIntegratedRasterChannel::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.FillTreeVector(values);
      // this functions doesn't do anything yet
    }
  return;
}

void  QwIntegratedRasterChannel::DeleteHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.DeleteHistograms();
    }
  return;
}
/********************************************************/
void  QwIntegratedRasterChannel::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwIntegratedRasterChannel* input=((QwIntegratedRasterChannel*)source);
	  this->fElementName=input->fElementName;
	  this->fPedestal=input->fPedestal;
	  this->fCalibration=input->fCalibration;
	  this->fTriumf_ADC.Copy(&(input->fTriumf_ADC));
	}
      else
	{
	  TString loc="Standard exception from QwIntegratedRasterChannel::Copy = "
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



std::vector<QwDBInterface> QwIntegratedRasterChannel::GetDBEntry()
{
  UShort_t i = 0;

  std::vector <QwDBInterface> row_list;
  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  row.Reset();

  // the element name and the n (number of measurements in average)
  // is the same in each block and hardwaresum.

  name          = fTriumf_ADC.GetElementName();
  beam_n        = fTriumf_ADC.GetGoodEventCount();

  // Get HardwareSum average and its error
  avg           = fTriumf_ADC.GetHardwareSum();
  err           = fTriumf_ADC.GetHardwareSumError();
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
    avg           = fTriumf_ADC.GetBlockValue(i);
    err           = fTriumf_ADC.GetBlockErrorValue(i);
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

  return row_list;

}
