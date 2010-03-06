#include "QwVQWK_Channel.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwUnits.h"
#include "QwHistogramHelper.h"


const Bool_t QwVQWK_Channel::kDEBUG = kFALSE;


// Randomness generator: Mersenne twister with period 2^19937 - 1
//
// This is defined as static to avoid getting stuck with 100% correlated
// ADC channels when each channel goes through the same list of pseudo-
// random numbers...
boost::mt19937 QwVQWK_Channel::fRandomnessGenerator;
boost::normal_distribution<double> QwVQWK_Channel::fNormalDistribution;
// The boost::variate_generator has operator() overloaded to get a new random
// value according to the distribution in the second template argument, based
// on the uniform random value generated by the first template argument.
// For example: fNormalRandomVariable() will return a random normal variable.
boost::variate_generator < boost::mt19937, boost::normal_distribution<double> >
  QwVQWK_Channel::fNormalRandomVariable(fRandomnessGenerator, fNormalDistribution);


/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is 0.07629 mV per count, and zero counts corresponds
 *   to zero voltage.
 */
const Double_t QwVQWK_Channel::kVQWK_VoltsPerBit = 76.29e-6;

/********************************************************/
Int_t QwVQWK_Channel::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;
  Bool_t bStatus;
  fDeviceErrorCode=0;//Initialize the error flag
  /*
  //debug- Ring analysis
  fEventCounter++;
  fTripCounter++;
  */

  if (bEVENTCUTMODE>0){//Global switch to ON/OFF event cuts set at the event cut file

    if (bDEBUG)
      std::cout<<" QwQWVK_Channel "<<GetElementName()<<"  "<<GetNumberOfSamples()<<std::endl;
    //Sample size check

    bStatus= MatchNumberOfSamples(fNumberOfSamples_map);//compare the default sample size with no.of samples read by the module


    if (!bStatus){
      fDeviceErrorCode|=kErrorFlag_sample;
      fErrorCount_sample++; //increment the error counter
    }
    //check SW and HW return the same sum
    bStatus= (GetRawHardwareSum()==GetRawSoftwareSum());
    //fEventIsGood =bStatus;
    if (!bStatus){
      fDeviceErrorCode|=kErrorFlag_SW_HW;
      fErrorCount_SW_HW++;
    }



    //check sequence number
    fSequenceNo_Prev++;
    if (fSequenceNo_Counter==0 || GetSequenceNumber()==0){//starting the data run
      fSequenceNo_Prev=GetSequenceNumber();
    }

    if (!MatchSequenceNumber(fSequenceNo_Prev)){//we have a sequence number error
      fEventIsGood=kFALSE;
      fDeviceErrorCode|=kErrorFlag_Sequence;
      fErrorCount_Sequence++;
      if (bDEBUG)       std::cout<<" QwQWVK_Channel "<<GetElementName()<<" Sequence number  previous value = "<<fSequenceNo_Prev<<" Current value= "<< GetSequenceNumber()<<std::endl;
    }

    fSequenceNo_Counter++;

    //Checking for HW_sum is returning same value.
    if (fPrev_HardwareBlockSum != GetRawHardwareSum()){
      //std::cout<<" BCM hardware sum is different  "<<std::endl;
      fPrev_HardwareBlockSum = GetRawHardwareSum();
      fADC_Same_NumEvt=0;
    }else
      fADC_Same_NumEvt++;//hw_sum is same increment the counter

    //check for the hw_sum is giving the same value
    if (fADC_Same_NumEvt>0){//we have ADC stuck with same value
      if (bDEBUG) std::cout<<" BCM hardware sum is same for more than  "<<fADC_Same_NumEvt<<" time consecutively  "<<std::endl;
      fDeviceErrorCode|=kErrorFlag_SameHW;
      fErrorCount_SameHW++;
    }

    //check for the hw_sum is zero
    if (GetRawHardwareSum()==0){
      fDeviceErrorCode|=kErrorFlag_ZeroHW;
      fErrorCount_ZeroHW++;
    }
    if (!fEventIsGood)
      fSequenceNo_Counter=0;//resetting the counter after ApplyHWChecks() a faliure

  }
  else
    fDeviceErrorCode=0;



  /*
  //debug- Ring analysis
  if (fEventCounter%100000==0){
    bTrip=kTRUE;
    fTripCounter=0;
  }

  if (bTrip && fTripCounter==801)
    bTrip=kFALSE;

  if (bTrip){
    if (fTripCounter<401)
      fHardwareBlockSum=fHardwareBlockSum*exp(-1*fTripCounter/75);//special lines to induce decaying beam trip
    else
      fHardwareBlockSum=fHardwareBlockSum*exp(-1*(800-fTripCounter)/75);
  }
  */


  return fDeviceErrorCode;
};

/********************************************************/


void QwVQWK_Channel::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  SetNumberOfDataWords(6);
  fSamplesPerBlock = 16680; //jpan: total samples = fSamplesPerBlock x fBlocksPerEvent
  fBlocksPerEvent = 4;
  fPedestal=0.0;
  fCalibrationFactor=1.;

  // Mock drifts
  fMockDriftAmplitude.clear();
  fMockDriftFrequency.clear();
  fMockDriftPhase.clear();

  // Mock asymmetries
  fMockAsymmetry = 0.0;
  fMockGaussianMean = 0.0;
  fMockGaussianSigma = 0.0;

  fEventNumber = 0;

  // Event cuts
  fNumEvtsWithEventCutsRejected=0;//init error counters
  fADC_Same_NumEvt=0;//init HW_Check counters
  fPrev_HardwareBlockSum=0;//init HW_Check counters
  fSequenceNo_Counter=0;//init HW_Check counters

  if (datatosave=="raw") fDataToSave=kRaw;
  else
    if (datatosave=="derived") fDataToSave=kDerived;

  fGoodEventCount=0;//initialize the event counter
  fRunning_sum=0;
  fRunning_sum_square=0;
  //init error counters//
  fErrorCount_sample=0;//for sample size check
  fErrorCount_SW_HW=0;//HW_sum==SW_sum check
  fErrorCount_Sequence=0;//sequence number check
  fErrorCount_SameHW=0;//check to see ADC returning same HW value
  fErrorCount_ZeroHW=0;
  /*
  //debug- Ring analysis
  fEventCounter=1;
  fTripCounter=0;
  bTrip=kFALSE;
  */
  fDeviceErrorCode=0;
  return;
}

/********************************************************/
Int_t QwVQWK_Channel::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  ReportErrorCounters();//print the summary
  return 1;
}

void QwVQWK_Channel::ClearEventData()
{
  for (size_t i = 0; i < 4; i++) {
    fBlock[i] = 0;
    fBlock_raw[i] = 0;
  }
  fHardwareBlockSum_raw = 0;
  fSoftwareBlockSum_raw = 0;
  fHardwareBlockSum = 0;
  fSequenceNumber   = 0;
  fNumberOfSamples  = 0;
};

void QwVQWK_Channel::RandomizeEventData(int helicity)
{
  // The blocks are assumed to be independent measurements
  Double_t block[fBlocksPerEvent];
  Double_t sqrt_fBlocksPerEvent = 0.0;
  sqrt_fBlocksPerEvent = sqrt(fBlocksPerEvent);

  // Calculate drift
  Double_t time = fEventNumber * Qw::ms; // WARNING: Hard-coded helicity window!!!
  Double_t drift = 0.0;
  for (size_t i = 0; i < fMockDriftFrequency.size(); i++) {
    drift += fMockDriftAmplitude[i] * sin(2.0 * Qw::pi * fMockDriftFrequency[i] * time + fMockDriftPhase[i]);
  }

  // Calculate signal
  for (size_t i = 0; i < fBlocksPerEvent; i++)
    block[i] = fMockGaussianMean * (1 + helicity * fMockAsymmetry) / fBlocksPerEvent
      + fMockGaussianSigma / sqrt_fBlocksPerEvent * fNormalRandomVariable()
      + drift / fBlocksPerEvent;

  SetEventData(block);
};

void QwVQWK_Channel::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  Double_t block[fBlocksPerEvent];
  for (size_t i = 0; i < fBlocksPerEvent; i++)
    block[i] = hwsum / fBlocksPerEvent;
  SetEventData(block);
};

void QwVQWK_Channel::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  // Clear existing values
  fMockDriftAmplitude.clear();
  fMockDriftFrequency.clear();
  fMockDriftPhase.clear();
  // Add new values
  fMockDriftAmplitude.push_back(amplitude);
  fMockDriftFrequency.push_back(frequency);
  fMockDriftPhase.push_back(phase);
};

void QwVQWK_Channel::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  // Add new values
  fMockDriftAmplitude.push_back(amplitude);
  fMockDriftFrequency.push_back(frequency);
  fMockDriftPhase.push_back(phase);
};

void QwVQWK_Channel::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fMockGaussianMean = mean;
  fMockGaussianSigma = sigma;
};

void QwVQWK_Channel::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fMockAsymmetry = asymmetry;
};

void QwVQWK_Channel::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fHardwareBlockSum = 0.0;
  for (size_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock[i] = block[i];
    fHardwareBlockSum += block[i];
  }
  fSequenceNumber = sequencenumber;
  fNumberOfSamples = 16680;

  Double_t thispedestal = 0.0;
  thispedestal = fPedestal * fNumberOfSamples;

  for (size_t i = 0; i < fBlocksPerEvent; i++)
      fBlock_raw[i] = fBlock[i] / fCalibrationFactor +
	thispedestal / (fBlocksPerEvent * 1.);

  fHardwareBlockSum_raw = fHardwareBlockSum / fCalibrationFactor + thispedestal;
  fSoftwareBlockSum_raw = fHardwareBlockSum_raw;
};


Int_t QwVQWK_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;
  Long_t localbuf[6] = {0};

  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
    words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords)
    {
      for (size_t i=0; i<6; i++){
	localbuf[i] = buffer[i];
      }
      fSoftwareBlockSum_raw = 0.0;
      for (size_t i=0; i<4; i++){
	fBlock_raw[i] = Double_t(localbuf[i]);
	fSoftwareBlockSum_raw += fBlock_raw[i];
      }
      fHardwareBlockSum_raw = Double_t(localbuf[4]);

      /*  Permanent change in the structure of the 6th word of the ADC readout.
       *  The upper 16 bits are the number of samples, and the upper 8 of the
       *  lower 16 are the sequence number.  This matches the structure of
       *  the ADC readout in block read mode, and now also in register read mode.
       *  P.King, 2007sep04.
       */
      fSequenceNumber   = (localbuf[5]>>8)  & 0xFF;
      fNumberOfSamples  = (localbuf[5]>>16) & 0xFFFF;

      words_read = fNumberOfDataWords;

      if (kDEBUG && GetElementName()=="SCAN_POW") {

        for (size_t i=0; i<5; i++){
	  std::cout<<"  hex("<<std::hex<<localbuf[i]<<") dec("<<std::dec<<Double_t(localbuf[i])<<") ";
        }
        Double_t average = 0.0;
	average = Double_t(localbuf[4])/fNumberOfSamples;
//         std::cout<<std::endl<<" SoftwareBlockSum_raw="<<fSoftwareBlockSum_raw
//                  <<"  NumberOfSamples="<<fNumberOfSamples
//                  <<"  average="<<average
//                  <<"  avg_voltage="<< kVQWK_VoltsPerBit*average<<std::endl;
      }

      if (kDEBUG && GetElementName()=="Bar1Right")
	{
	  //    if (num_words_left == 6){
	  std::cout << std::hex
		    << buffer[0] << " " << buffer[1] << " "
		    << buffer[2] << " " << buffer[3] << " "
		    << buffer[4] << " " << buffer[5] << " --- "
		    << std::dec
		    << fBlock_raw[0] << " " << fBlock_raw[1] << " "
		    << fBlock_raw[2] << " " << fBlock_raw[3] << " "
		    << fSoftwareBlockSum_raw << " " << fHardwareBlockSum_raw
		    << " " << fSequenceNumber << " " << fNumberOfSamples
		    << std::endl;
	}
    } else
      {
	std::cerr << "QwVQWK_Channel::ProcessEvBuffer: Not enough words!"
		  << std::endl;
      }
  return words_read;
};

void QwVQWK_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  Long_t localbuf[6] = {0};

  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    localbuf[4] = 0;
    for (size_t i = 0; i < 4; i++) {
        localbuf[i] = Long_t(fBlock_raw[i]);
        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
    }
    // The following causes many rounding errors and skips due to the check
    // that fHardwareBlockSum_raw == fSoftwareBlockSum_raw in IsGoodEvent().
    //localbuf[4] = Long_t(fHardwareBlockSum_raw);
    localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (size_t i = 0; i < 6; i++){
        buffer.push_back(localbuf[i]);
    }
  }
};


void QwVQWK_Channel::ProcessEvent()
{
  Double_t thispedestal = 0.0;
  thispedestal = fPedestal * fNumberOfSamples;

  for (Int_t i = 0; i < 4; i++)
      fBlock[i]= fCalibrationFactor *
	(fBlock_raw[i] - thispedestal / (fBlocksPerEvent*1.));

  fHardwareBlockSum = fCalibrationFactor *
    (fHardwareBlockSum_raw - thispedestal);

  return;
};

Double_t QwVQWK_Channel::GetAverageVolts(){
  Double_t avgVolts = 0.0;
  avgVolts = (fBlock[0]+fBlock[1]+fBlock[2]+fBlock[3])*kVQWK_VoltsPerBit/fNumberOfSamples;
  //std::cout<<"QwVQWK_Channel::GetAverageVolts() = "<<avgVolts<<std::endl;
  return avgVolts;

};

void QwVQWK_Channel::Print() const
{
  std::cout<<"***************************************"<<"\n";
  std::cout<<"QwVQWK channel: "<<GetElementName()<<"\n"<<"\n";
  std::cout<<"fPedestal= "<< fPedestal<<"\n";
  std::cout<<"fCalibrationFactor= "<<fCalibrationFactor<<"\n";
  std::cout<<"fSamplesPerBlock= "<< fSamplesPerBlock<<"\n";
  std::cout<<"fBlocksPerEvent= "<<fBlocksPerEvent<<"\n"<<"\n";
  std::cout<<"fSequenceNumber= "<<fSequenceNumber<<"\n";
  std::cout<<"fNumberOfSamples= "<<fNumberOfSamples<<"\n";
  std::cout<<"fBlock_raw ";
  for(int i=0;i<4;i++)
    std::cout<<" : "<<fBlock_raw[i];
  std::cout<<"\n";
  std::cout<<"fHardwareBlockSum_raw= "<<fHardwareBlockSum_raw<<"\n";
  std::cout<<"fSoftwareBlockSum_raw= "<<fSoftwareBlockSum_raw<<"\n";
 std::cout<<"fBlock ";
  for(int i=0;i<4;i++)
    std::cout<<" : "<<fBlock[i];
  std::cout<<"\n";
  std::cout<<"fHardwareBlockSum= "<<fHardwareBlockSum<<"\n";

  return;
}

void  QwVQWK_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (IsNameEmpty()){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    if (prefix == TString("asym_")
       || prefix == TString("diff_")
       || prefix == TString("yield_"))
      fDataToSave=kDerived;

    TString basename, fullname;
    basename = prefix + GetElementName();

    if(fDataToSave==kRaw)
      {
	fHistograms.resize(8+2+1, NULL);
	size_t index=0;
	for (size_t i=0; i<4; i++){
	  fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_block%d_raw",i));
	  fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_block%d",i));
	  index += 2;
	}
	fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_hw_raw"));
	fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_hw"));
	index += 2;
	fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_sw-hw_raw"));
      }
    else if(fDataToSave==kDerived)
      {
	fHistograms.resize(4+1, NULL);
	size_t index=0;
	for (size_t i=0; i<4; i++){
	  fHistograms[index] = gQwHists.Construct1DHist(basename+Form("_block%d",i));
	  index += 1;
	}
	fHistograms[index] = gQwHists.Construct1DHist(basename+Form("_hw"));
      }
    else
      {
	// this is not recognized
      }

  }
};

void  QwVQWK_Channel::FillHistograms(){
  size_t index=0;
  if (IsNameEmpty())
    {
      //  This channel is not used, so skip creating the histograms.
    } else
      {
	if(fDataToSave==kRaw)
	  {
	    for (size_t i=0; i<4; i++)
	      {
		if (fHistograms[index] != NULL)
		  fHistograms[index]->Fill(this->GetRawBlockValue(i));
		if (fHistograms[index+1] != NULL)
		  fHistograms[index+1]->Fill(this->GetBlockValue(i));
		index+=2;
	      }
	    if (fHistograms[index] != NULL)
	      fHistograms[index]->Fill(this->GetRawHardwareSum());
	    if (fHistograms[index+1] != NULL)
	      fHistograms[index+1]->Fill(this->GetHardwareSum());
	    index+=2;
	    if (fHistograms[index] != NULL)
	      fHistograms[index]->Fill(this->GetRawSoftwareSum()-this->GetHardwareSum());
	  }
	else if(fDataToSave==kDerived)
	  {
	    for (size_t i=0; i<4; i++)
	      {
		if (fHistograms[index] != NULL)
		  fHistograms[index]->Fill(this->GetBlockValue(i));
		index+=1;
	      }
	    if (fHistograms[index] != NULL)
	      fHistograms[index]->Fill(this->GetHardwareSum());
	  }
	else
	  {
	    // this is not defined
	  }
      }
};

void  QwVQWK_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "hw_sum/D";
    values.push_back(0.0);
    list += ":block0/D";
    values.push_back(0.0);
    list += ":block1/D";
    values.push_back(0.0);
    list += ":block2/D";
    values.push_back(0.0);
    list += ":block3/D";
    values.push_back(0.0);
    list += ":num_samples/D";
    values.push_back(0.0);
    list += ":Device_Error_Code/D";
    if(fDataToSave==kRaw)
      {
	values.push_back(0.0);
	list += ":hw_sum_raw/D";
	values.push_back(0.0);
	list += ":block0_raw/D";
	values.push_back(0.0);
	list += ":block1_raw/D";
	values.push_back(0.0);
	list += ":block2_raw/D";
	values.push_back(0.0);
	list += ":block3_raw/D";
	values.push_back(0.0);
	list += ":sequence_number/D";
      }

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
    if (kDEBUG && GetElementName()=="Bar1Right"){
      std::cerr << "QwVQWK_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
		<< "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
		<< "; values.size()==" << values.size()
		<< "; list==" << list
		<< std::endl;
    }
  }
};

void  QwVQWK_Channel::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries<=0){
    std::cerr << "QwVQWK_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    std::cerr << "QwVQWK_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    size_t index=fTreeArrayIndex;
    values[index++] = this->GetHardwareSum();
    for (size_t i=0; i<4; i++){
      values[index++] = this->GetBlockValue(i);
    }
    values[index++] = this->fNumberOfSamples;

    values[index++] = this->fDeviceErrorCode;
    if(fDataToSave==kRaw)
      {
	values[index++] = this->GetRawHardwareSum();
	for (size_t i=0; i<4; i++){
	  values[index++] = this->GetRawBlockValue(i);
	}
	values[index++]=this->fSequenceNumber;
      }
  }
  //if (kDEBUG && GetElementName()=="bar1right"){
  if (kDEBUG && (GetElementName()=="bar1left_bar5left_sum" || GetElementName()=="bar1right"))
    {
    std::cerr<<"Fill vector for "<<GetElementName()<<std::endl;
    std::cerr << "&(values["<<fTreeArrayIndex<<"])=="<< &(values[fTreeArrayIndex])
	      << std::endl;
    size_t index=fTreeArrayIndex;
    for (size_t i=0; i<fTreeArrayNumEntries; i++){
      std::cerr << "values[" << index << "]==" << values[index] << " ";
      index++; // after previous statement to avoid ambiguity in ostream expression
    }
    std::cerr << std::endl;
  }
};



QwVQWK_Channel& QwVQWK_Channel::operator= (const QwVQWK_Channel &value){
  if (!IsNameEmpty())
    {
      for (size_t i=0; i<4; i++){
	this->fBlock_raw[i] = value.fBlock_raw[i];
	this->fBlock[i] = value.fBlock[i];
      }
      this->fHardwareBlockSum_raw= value.fHardwareBlockSum_raw;
      this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
      this->fHardwareBlockSum = value.fHardwareBlockSum;
      this->fNumberOfSamples  = value.fNumberOfSamples;
      this->fSequenceNumber   = value.fSequenceNumber;
      this->fDeviceErrorCode = (value.fDeviceErrorCode);//error code is updated.
    }
  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator+= (const QwVQWK_Channel &value){
  if (!IsNameEmpty()){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] += value.fBlock[i];
      this->fBlock_raw[i]+=value.fBlock[i];
    }
    this->fHardwareBlockSum_raw =value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw =value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum += value.fHardwareBlockSum;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
  }
  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator-= (const QwVQWK_Channel &value){
  if (!IsNameEmpty()){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] -= value.fBlock[i];
      this->fBlock_raw[i]=0;
    }
    this->fHardwareBlockSum_raw =0;
    this->fSoftwareBlockSum_raw =0;
    this->fHardwareBlockSum -= value.fHardwareBlockSum;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
  }
  return *this;
};

void QwVQWK_Channel::Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2){
  *this =  value1;
  *this += value2;
};

void QwVQWK_Channel::Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2){
  *this =  value1;
  *this -= value2;
};

void QwVQWK_Channel::Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom){
  if (!IsNameEmpty()){
    for (size_t i=0; i<4; i++){
      if (denom.fBlock[i]!=0){
	this->fBlock[i] = (numer.fBlock[i]) / (denom.fBlock[i]);
	this->fBlock_raw[i]=0;
      }else
	this->fBlock[i] = 0;
    }
    if (denom.fHardwareBlockSum!=0)//this fail safe check is needed when EVENTCUTS (HW) are turned off.
      this->fHardwareBlockSum = (numer.fHardwareBlockSum) / (denom.fHardwareBlockSum);
    else
      this->fHardwareBlockSum = 0;
    this->fSoftwareBlockSum_raw = 0;
    this->fHardwareBlockSum_raw = 0;
    this->fNumberOfSamples = denom.fNumberOfSamples;
    this->fSequenceNumber  = 0;
    this->fDeviceErrorCode = (numer.fDeviceErrorCode|denom.fDeviceErrorCode);//error code is ORed.
  }
};

void QwVQWK_Channel::Offset(Double_t offset)
{
  if (!IsNameEmpty())
    {
      for (size_t i=0; i<4; i++)
	fBlock[i] +=offset;
      fHardwareBlockSum += 4*offset;
    }
};


void QwVQWK_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      for (size_t i=0; i<4; i++)
	fBlock[i]=fBlock[i]*scale;
      fHardwareBlockSum=fHardwareBlockSum*scale;
    }
};

void QwVQWK_Channel::Calculate_Running_Average(){

//   if (!fGoodEventCount){
//     std::cout<<GetElementName()<<"\t0\t 0\t 0"<<std::endl;
//   }else{
//     fAverage_n=fRunning_sum/fGoodEventCount;
//     fAverage_n_square=fRunning_sum_square/fGoodEventCount;
//     fAverage_error= sqrt(((fAverage_n_square-fAverage_n*fAverage_n)/fGoodEventCount));
//     //    std::cout<<GetElementName()<<" \t "<<this->fAverage_n <<" \t "<< fAverage_error <<" \t "<<fGoodEventCount <<std::endl;

//   }


  if (fGoodEventCount)
    {
      fAverage_n=fRunning_sum/fGoodEventCount;
      fAverage_n_square=fRunning_sum_square/fGoodEventCount;
      fAverage_error= sqrt(((fAverage_n_square-fAverage_n*fAverage_n)/fGoodEventCount));
    }
  else
    {
      fAverage_n        = 0.0;
      fAverage_n_square = 0.0;
      fAverage_error    = 0.0;
    }
  
  this -> Print_Running_Average();
    //    std::cout<<GetElementName()<<" \t "<<this->fAverage_n <<" \t "<< fAverage_error <<" \t "<<fGoodEventCount <<std::endl;

};

void QwVQWK_Channel::Print_Running_Average()
{
  std::cout
    << this->GetElementName()<<" \t "
    << this->fAverage_n      <<" \t "
    << this->fAverage_error  <<" \t "
    << fGoodEventCount 
    <<std::endl;
}

void QwVQWK_Channel::Do_RunningSum(){
  if (fDeviceErrorCode == 0){//if the device HW is good

    fRunning_sum+=fHardwareBlockSum;//increase the sum. sum square and event counter
    fRunning_sum_square+=fHardwareBlockSum*fHardwareBlockSum;
    fGoodEventCount++;
  }

};


Bool_t QwVQWK_Channel::MatchSequenceNumber(size_t seqnum)
{

  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fSequenceNumber==seqnum);
    }
  return status;
};

Bool_t QwVQWK_Channel::MatchNumberOfSamples(size_t numsamp)
{
  //std::cout<<" QwQWVK_Channel "<<GetElementName()<<"  "<<fNumberOfSamples_map<<" "<<numsamp<<" "<<fNumberOfSamples<<std::endl;
  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fNumberOfSamples==numsamp);
    if (! status){
      if (bDEBUG) std::cerr << "QwVQWK_Channel::MatchNumberOfSamples:  Channel "
		<< GetElementName()
		<< " had fNumberOfSamples==" << fNumberOfSamples
		<< " and was supposed to have " << numsamp
		<< std::endl;
      //      Print();
    }
  }
  return status;
};

Bool_t QwVQWK_Channel::ApplySingleEventCuts(Double_t LL=0,Double_t UL=0){
  Bool_t status;
  if (bEVENTCUTMODE>=2){//Global switch to ON/OFF event cuts set at the event cut file

    if (LL==0 && UL==0){
      status=kTRUE;
    } else  if (GetHardwareSum()<=UL && GetHardwareSum()>=LL){
      if (!fDeviceErrorCode)
	status=kTRUE;
      else
	status=kFALSE;//If the device HW is falied
    }
    else{
      if (GetHardwareSum()> UL)
	fDeviceErrorCode|=kErrorFlag_EventCut_U;
      else
	fDeviceErrorCode|=kErrorFlag_EventCut_L;
      status=kFALSE;
    }
    if (bEVENTCUTMODE==3){
      if (!status) 
	UpdateEventCutErrorCount();//update the event cut  error count
      status=kTRUE; //Update the event cut fail flag but pass the event.
    }


  }
  else
    status=kTRUE;


    return status;
};






void QwVQWK_Channel::Copy(VQwDataElement *source)
{
    try
    {
     if(typeid(*source)==typeid(*this))
       {
	 QwVQWK_Channel* input=((QwVQWK_Channel*)source);
	 this->fElementName=input->fElementName;
	 this->fPedestal=input->GetPedestal();
	 this->fCalibrationFactor=input->GetCalibrationFactor();
	 this->fDataToSave=kDerived;
	 this->fDeviceErrorCode=input->fDeviceErrorCode;

	 this->fRunning_sum=input->fRunning_sum;
	 this->fRunning_sum_square=input->fRunning_sum_square;
	 this->fAverage_n= input->fAverage_n;
	 this->fAverage_n_square=input->fAverage_n_square;
	 this->fGoodEventCount=input->fGoodEventCount;
       }
     else
       {
	 TString loc="Standard exception from QwVQWK_Channel::Copy = "
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

void  QwVQWK_Channel::ReportErrorCounters(){



  if (fErrorCount_sample || fErrorCount_SW_HW || fErrorCount_Sequence || fErrorCount_SameHW || fErrorCount_ZeroHW || fNumEvtsWithEventCutsRejected){
     std::cout<<GetElementName();
     //if (fErrorCount_sample)
       std::cout <<"\t"<<fErrorCount_sample;
       //if (fErrorCount_SW_HW)
      std::cout <<"\t"<<fErrorCount_SW_HW;
      //if (fErrorCount_Sequence )
      std::cout <<" \t "<<fErrorCount_Sequence;
      //if (fErrorCount_SameHW)
      std::cout <<" \t "<<fErrorCount_SameHW ;
      std::cout <<" \t "<<fErrorCount_ZeroHW ;
      //if (fNumEvtsWithEventCutsRejected)
      std::cout<< " \t " << fNumEvtsWithEventCutsRejected<<"\n";
      //if (fErrorCount_sample || fErrorCount_SW_HW || fErrorCount_Sequence || fErrorCount_SameHW)
      //std::cout <<"*************End of Error summary*****************"<<std::endl;
  }
};
