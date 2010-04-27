/**********************************************************\
* File: QwIntegrationPMT.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegrationPMT.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

/********************************************************/
void QwIntegrationPMT::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
};

void QwIntegrationPMT::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration);
	return;
};
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  return;
};
/********************************************************/
void QwIntegrationPMT::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
};
/********************************************************/
void QwIntegrationPMT::ReportErrorCounters(){
  fTriumf_ADC.ReportErrorCounters();
};
/********************************************************/
void QwIntegrationPMT::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
};
/********************************************************/
void QwIntegrationPMT::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
};
/********************************************************/
void QwIntegrationPMT::RandomizeEventData(int helicity)
{
  fTriumf_ADC.RandomizeEventData(helicity);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
};

Double_t QwIntegrationPMT::GetHardwareSum()
{
  return fTriumf_ADC.GetHardwareSum();
};

Double_t QwIntegrationPMT::GetBlockValue(Int_t blocknum)
{
  return fTriumf_ADC.GetBlockValue(blocknum);
};

/********************************************************/
void QwIntegrationPMT::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetEventNumber(int event)
{
  fTriumf_ADC.SetEventNumber(event);
  return;
};
/********************************************************/
void QwIntegrationPMT::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
};
/********************************************************/
void  QwIntegrationPMT::ProcessEvent()
{

  fTriumf_ADC.ProcessEvent();


  return;
};
/********************************************************/
Bool_t QwIntegrationPMT::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

    fDeviceErrorCode=fTriumf_ADC.ApplyHWChecks();//will check for consistancy between HWSUM and SWSUM also check for sample size
    fEventIsGood=(fDeviceErrorCode & 0x0);//if no HW error return true


  return fEventIsGood;
};
/********************************************************/

Int_t QwIntegrationPMT::SetSingleEventCuts(std::vector<Double_t> & dEventCuts){//two limts and sample size
  fLLimit=dEventCuts.at(0);
  fULimit=dEventCuts.at(1);
  fDevice_flag=(Int_t)dEventCuts.at(2);
  //std::cout<<GetElementName()<<" IntegrationPMT fDevice_flag "<<fDevice_flag<<std::endl;

  return 1;
};


///* will not compile with Buddhini's code 12nov09
void QwIntegrationPMT::SetDefaultSampleSize(Int_t sample_size){
 fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}
//*/

/********************************************************/
Bool_t QwIntegrationPMT::ApplySingleEventCuts(){


//std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;
  ApplyHWChecks();//first apply HW checks and update HW  error flags.

  if (fDevice_flag==1){// if fDevice_flag==1 then perform the event cut limit test

    //if (fTriumf_ADC.GetHardwareSum()<=fULimit && fTriumf_ADC.GetHardwareSum()>=fLLimit){ // Check event cuts + HW check status
    if (fTriumf_ADC.ApplySingleEventCuts(fLLimit,fULimit)){
      status=kTRUE;
      //std::cout<<" BCM Sample size "<<fTriumf_ADC.GetNumberOfSamples()<<std::endl;
    }
    else{
      fTriumf_ADC.UpdateEventCutErrorCount();//update event cut falied counts
      if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fTriumf_ADC.GetHardwareSum();
      status&=kFALSE;//kTRUE;//kFALSE;
    }
  }else
    status =kTRUE;


  return status;

};

/********************************************************/

Int_t QwIntegrationPMT::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}


/********************************************************/


Int_t QwIntegrationPMT::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
};
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
};

QwIntegrationPMT& QwIntegrationPMT::operator+= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};

QwIntegrationPMT& QwIntegrationPMT::operator-= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};


void QwIntegrationPMT::Sum(QwIntegrationPMT &value1, QwIntegrationPMT &value2){
  *this =  value1;
  *this += value2;
};

void QwIntegrationPMT::Difference(QwIntegrationPMT &value1, QwIntegrationPMT &value2){
  *this =  value1;
  *this -= value2;
};

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
};

void QwIntegrationPMT::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}


void QwIntegrationPMT::Print() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<IntegrationPMT_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fTriumf_ADC.Print();
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
};

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
};

void  QwIntegrationPMT::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranchAndVector(tree, prefix,values);
    }
  return;
};

void  QwIntegrationPMT::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.FillTreeVector(values);
    }
  return;
};

void  QwIntegrationPMT::DeleteHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.DeleteHistograms();
    }
  return;
};
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


void QwIntegrationPMT::Calculate_Running_Average(){
  fTriumf_ADC.Calculate_Running_Average();
};

void QwIntegrationPMT::Do_RunningSum(){
  fTriumf_ADC.Do_RunningSum();
};

void QwIntegrationPMT::BlindMe(QwBlinder *blinder){
  fTriumf_ADC.BlindMe(blinder);
};

QwParityDB::md_data QwIntegrationPMT::GetMainDetectorDBEntry(QwDatabase *db, TString mtype, TString subname)
{
  QwParityDB::md_data row(0);
  
  UInt_t md_run_id        = 0;
  UInt_t md_analysis_id   = 0;
  UInt_t main_detector_id = 0;
  Char_t md_measurement_type[4];
  UInt_t md_subblock      = 0;
  UInt_t md_n             = 0;

  TString name;
  Double_t avg = 0.0;
  Double_t err = 0.0;

  if(mtype.Contains("yield"))
    {
      sprintf(md_measurement_type, "yq");
    }
  else if(mtype.Contains("asymmetry"))
    {
      sprintf(md_measurement_type, "aq");
    }
  else if(mtype.Contains("average") )
    {
      sprintf(md_measurement_type, "yq");
    }
  else if(mtype.Contains("runningsum"))
    {
      sprintf(md_measurement_type, "yq");
    }
  else
    {
      sprintf(md_measurement_type, " ");
    }
  
  name = this->GetElementName();
  avg  = this->GetAverage("");
  err  = this->GetAverageError("");
  md_subblock   = 0;
  md_n          = 0;

  md_run_id        = db->GetRunID();
  md_analysis_id   = db->GetAnalysisID();
  main_detector_id = db->GetMainDetectorID(name.Data());

  row.analysis_id         = md_analysis_id;
  row.measurement_type_id = md_measurement_type;
  row.main_detector_id    = main_detector_id;
  row.subblock            = md_subblock;  // this will be used later. At the moment, 0
  row.n                   = md_n;         // this will be used later. At the moment, 0
  row.value               = avg;
  row.error               = err;

  printf("%12s::RunID %d AnalysisID %d %4s MainDetectorID %4d %18s , Subblock %d, n %d [%18.2e, %12.2e] \n", 
	 mtype.Data(), md_run_id, md_analysis_id, md_measurement_type, main_detector_id, name.Data(),  
	 md_subblock, md_n, avg, err);
  
  return row;
  
};



QwParityDB::lumi_data QwIntegrationPMT::GetLumiDetectorDBEntry(QwDatabase *db, TString mtype, TString subname)
{
  
  QwParityDB::lumi_data row(0);
  
  UInt_t lumi_analysis_id = 0;
  UInt_t lumi_detector_id = 0;
  Char_t lumi_measurement_type[4];
  UInt_t lumi_subblock    = 0;
  UInt_t lumi_n           = 0;

  TString name;
  Double_t avg = 0.0;
  Double_t err = 0.0;

  if(mtype.Contains("yield"))
    {
      sprintf(lumi_measurement_type, "yq");
    }
  else if(mtype.Contains("asymmetry"))
    {
      sprintf(lumi_measurement_type, "aq");
    }
  else if(mtype.Contains("average") )
    {
      sprintf(lumi_measurement_type, "yq");
    }
  else if(mtype.Contains("runningsum"))
    {
      sprintf(lumi_measurement_type, "yq");
    }
  else
    {
      sprintf(lumi_measurement_type, " ");
    }
  
  name = this->GetElementName();
  avg  = this->GetAverage("");
  err  = this->GetAverageError("");

  lumi_analysis_id = db->GetAnalysisID();
  lumi_detector_id = db->GetLumiDetectorID(name.Data());

  lumi_subblock    = 0;
  lumi_n           = 0;

  row.analysis_id         = lumi_analysis_id;
  row.measurement_type_id = lumi_measurement_type;
  row.lumi_detector_id    = lumi_detector_id;
  row.subblock            = lumi_subblock;  // this will be used later. At the moment, 0
  row.n                   = lumi_n;         // this will be used later. At the moment, 0
  row.value               = avg;
  row.error               = err;

  
  return row;
  
};
