/**********************************************************\
* File: QwIntegrationPMT.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegrationPMT.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"

/********************************************************/
void QwIntegrationPMT::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
}

void QwIntegrationPMT::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration);
	return;
}
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
  return;
}
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString subsystem, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(subsystem,"QwIntegrationPMT", name, datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
  return;
}
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString subsystem, TString module, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(subsystem,module, name, datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
  return;
}
/********************************************************/
void QwIntegrationPMT::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
}
/********************************************************/
void QwIntegrationPMT::ReportErrorCounters(){
  fTriumf_ADC.ReportErrorCounters();
}
/********************************************************/
void QwIntegrationPMT::UseExternalRandomVariable()
{
  fTriumf_ADC.UseExternalRandomVariable();
  return;
}
/********************************************************/
void QwIntegrationPMT::SetExternalRandomVariable(double random_variable)
{
  fTriumf_ADC.SetExternalRandomVariable(random_variable);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
void QwIntegrationPMT::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
}
/********************************************************/
void QwIntegrationPMT::RandomizeEventData(int helicity, double time)
{
  fTriumf_ADC.RandomizeEventData(helicity, time);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
}

Double_t QwIntegrationPMT::GetValue()
{
  return fTriumf_ADC.GetValue();
}

Double_t QwIntegrationPMT::GetValue(Int_t blocknum)
{
  return fTriumf_ADC.GetValue(blocknum);
}

/********************************************************/
void QwIntegrationPMT::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
}
/********************************************************/
void QwIntegrationPMT::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
}
/********************************************************/
void  QwIntegrationPMT::ProcessEvent()
{
  ApplyHWChecks();//first apply HW checks and update HW  error flags.
  fTriumf_ADC.ProcessEvent();

  return;
}
/********************************************************/
Bool_t QwIntegrationPMT::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=fTriumf_ADC.ApplyHWChecks();//will check for consistancy between HWSUM and SWSUM also check for sample size
  eventokay=(deviceerror & 0x0);//if no HW error return true


  return eventokay;
}
/********************************************************/

Int_t QwIntegrationPMT::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){//std::vector<Double_t> & dEventCuts){//two limts and sample size
  fTriumf_ADC.SetSingleEventCuts(LL,UL);
  return 1;
}

/********************************************************/
void QwIntegrationPMT::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kPMTErrorFlag;
  QwMessage<<"QwIntegrationPMT Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fTriumf_ADC.SetSingleEventCuts(errorflag,LL,UL,stability);

}

/********************************************************/

void QwIntegrationPMT::SetDefaultSampleSize(Int_t sample_size){
 fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}
//*/

/********************************************************/
Bool_t QwIntegrationPMT::ApplySingleEventCuts(){


//std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;

  if (fTriumf_ADC.ApplySingleEventCuts()){
    status=kTRUE;
    //std::cout<<" BCM Sample size "<<fTriumf_ADC.GetNumberOfSamples()<<std::endl;
  }
  else{
    status&=kFALSE;//kTRUE;//kFALSE;
  }

  return status;

}

/********************************************************/

Int_t QwIntegrationPMT::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  fTriumf_ADC.GetEventcutErrorCounters();
  return 1;
}


/********************************************************/


Int_t QwIntegrationPMT::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
}  Double_t fULimit, fLLimit;

/********************************************************/
QwIntegrationPMT& QwIntegrationPMT::operator= (const QwIntegrationPMT &value)
{
//   std::cout<<" Here in QwIntegrationPMT::operator= \n";
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

QwIntegrationPMT& QwIntegrationPMT::operator+= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

QwIntegrationPMT& QwIntegrationPMT::operator-= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}


void QwIntegrationPMT::Sum(QwIntegrationPMT &value1, QwIntegrationPMT &value2){
  *this =  value1;
  *this += value2;
}

void QwIntegrationPMT::Difference(QwIntegrationPMT &value1, QwIntegrationPMT &value2){
  *this =  value1;
  *this -= value2;
}

void QwIntegrationPMT::Ratio(QwIntegrationPMT &numer, QwIntegrationPMT &denom)
{
  //  std::cout<<"QwIntegrationPMT::Ratio element name ="<<GetElementName()<<" \n";
  if (GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fTriumf_ADC.Ratio(numer.fTriumf_ADC,denom.fTriumf_ADC);
      this->fPedestal=0;
      this->fCalibration=0;
    }
  return;
}

void QwIntegrationPMT::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}

void QwIntegrationPMT::Normalize(VQwDataElement* denom)
{
	if (fIsNormalizable) {
  	QwVQWK_Channel vqwk_denom;
  	vqwk_denom.Copy(denom);
  	fTriumf_ADC.DivideBy(vqwk_denom);
  }
  return;
}

void QwIntegrationPMT::PrintValue() const
{
  fTriumf_ADC.PrintValue();
}

void QwIntegrationPMT::PrintInfo() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<IntegrationPMT_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fTriumf_ADC.PrintInfo();
  std::cout<< "Blindability is "    << (fIsBlindable?"TRUE":"FALSE") 
	   <<std::endl;
  std::cout<< "Normalizability is " << (fIsNormalizable?"TRUE":"FALSE")
	   <<std::endl;
  return;
}

/********************************************************/
void  QwIntegrationPMT::ConstructHistograms(TDirectory *folder, TString &prefix)
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

void  QwIntegrationPMT::FillHistograms()
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

void  QwIntegrationPMT::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranchAndVector(tree, prefix,values);
    }
  return;
}

void  QwIntegrationPMT::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranch(tree, prefix);
    }
  return;
}

void  QwIntegrationPMT::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
   TString devicename;
   devicename=GetElementName();
   devicename.ToLower();
   if (GetElementName()==""){
     //  This channel is not used, so skip filling the histograms.
   } else {
     if (modulelist.HasValue(devicename)){
       fTriumf_ADC.ConstructBranch(tree, prefix);
       QwMessage <<"QwIntegrationPMT Tree leave added to "<<devicename<<QwLog::endl;
       }

   }
  return;
}


void  QwIntegrationPMT::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    fTriumf_ADC.FillTreeVector(values);
  }
}

/********************************************************/
void  QwIntegrationPMT::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwIntegrationPMT* input=((QwIntegrationPMT*)source);
	  this->fElementName=input->fElementName;
	  this->fPedestal=input->fPedestal;
	  this->fCalibration=input->fCalibration;
	  this->fIsBlindable=input->fIsBlindable;
	  this->fIsNormalizable=input->fIsNormalizable;
	  this->fTriumf_ADC.Copy(&(input->fTriumf_ADC));
	}
      else
	{
	  TString loc="Standard exception from QwIntegrationPMT::Copy = "
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


void QwIntegrationPMT::CalculateRunningAverage()
{
  fTriumf_ADC.CalculateRunningAverage();
}

void QwIntegrationPMT::AccumulateRunningSum(const QwIntegrationPMT& value)
{
  fTriumf_ADC.AccumulateRunningSum(value.fTriumf_ADC);
}

void QwIntegrationPMT::Blind(const QwBlinder *blinder)
{
  if (fIsBlindable)  fTriumf_ADC.Blind(blinder);
}

void QwIntegrationPMT::Blind(const QwBlinder *blinder, const QwIntegrationPMT& yield)
{
  if (fIsBlindable)  fTriumf_ADC.Blind(blinder, yield.fTriumf_ADC);
}


std::vector<QwDBInterface> QwIntegrationPMT::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  fTriumf_ADC.AddEntriesToList(row_list);
  return row_list;

  //   UShort_t i = 0;
  //   std::vector<QwDBInterface> row_list;
  //   QwDBInterface row;
  
  //   TString name;
  //   Double_t avg         = 0.0;
  //   Double_t err         = 0.0;
  //   UInt_t beam_subblock = 0;
  //   UInt_t beam_n        = 0;

  //   row_list.clear();
  //   row.Reset();

  //   // the element name and the n (number of measurements in average)
  //   // is the same in each block and hardwaresum.
  
  //   name          = fTriumf_ADC.GetElementName();
  //   beam_n        = fTriumf_ADC.GetGoodEventCount();

  //   // Get HardwareSum average and its error
  //   avg           = fTriumf_ADC.GetHardwareSum();
  //   err           = fTriumf_ADC.GetHardwareSumError();
  //   // ADC subblock sum : 0 in MySQL database
  //   beam_subblock = 0;

  //   row.SetDetectorName(name);
  //   row.SetSubblock(beam_subblock);
  //   row.SetN(beam_n);
  //   row.SetValue(avg);
  //   row.SetError(err);

  //   row_list.push_back(row);

  //   // Get four Block averages and thier errors

  //   for(i=0; i<4; i++) {
  //     row.Reset();
  //     avg           = fTriumf_ADC.GetBlockValue(i);
  //     err           = fTriumf_ADC.GetBlockErrorValue(i);
  //     beam_subblock = (UInt_t) (i+1);
  //     // QwVQWK_Channel  | MySQL
  //     // fBlock[0]       | subblock 1
  //     // fBlock[1]       | subblock 2
  //     // fBlock[2]       | subblock 3
  //     // fBlock[3]       | subblock 4
  //     row.SetDetectorName(name);
  //     row.SetSubblock(beam_subblock);
  //     row.SetN(beam_n);
  //     row.SetValue(avg);
  //     row.SetError(err);

  //     row_list.push_back(row);
  //   }

  //   return row_list;

}

