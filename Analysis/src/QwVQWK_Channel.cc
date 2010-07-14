#include "QwVQWK_Channel.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwHistogramHelper.h"
#include "QwBlinder.h"


const Bool_t QwVQWK_Channel::kDEBUG = kFALSE;

const Int_t  QwVQWK_Channel::kWordsPerChannel = 6;
const Int_t  QwVQWK_Channel::kMaxChannels     = 8;


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

/*!  Static member function to return the word offset within a data buffer
 *   given the module number index and the channel number index.
 *   @param moduleindex   Module index within this buffer; counts from zero
 *   @param channelindex  Channel index within this module; counts from zero
 *   @return   The number of words offset to the beginning of this
 *             channel's data from the beginning of the VQWK buffer.
 */
Int_t QwVQWK_Channel::GetBufferOffset(Int_t moduleindex, Int_t channelindex){
    Int_t offset = -1;
    if (moduleindex<0 ){
      QwError << "QwVQWK_Channel::GetBufferOffset:  Invalid module index,"
	      << moduleindex
	      << ".  Must be zero or greater."
	      << QwLog::endl;
    } else if (channelindex<0 || channelindex>kMaxChannels){
      QwError << "QwVQWK_Channel::GetBufferOffset:  Invalid channel index,"
	      << channelindex
	      << ".  Must be in range [0," << kMaxChannels << "]."
	      << QwLog::endl;
    } else {
      offset = ( (moduleindex * kMaxChannels) + channelindex )
	* kWordsPerChannel;
    }
    return offset;
  }


/********************************************************/
Int_t QwVQWK_Channel::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;
  Bool_t bStatus;
  fDeviceErrorCode=0;//Initialize the error flag

  if (bEVENTCUTMODE>0){//Global switch to ON/OFF event cuts set at the event cut file

    if (bDEBUG)
      std::cout<<" QwQWVK_Channel "<<GetElementName()<<"  "<<GetNumberOfSamples()<<std::endl;
    //Sample size check

    bStatus= MatchNumberOfSamples(fNumberOfSamples_map);//compare the default sample size with no.of samples read by the module


    if (!bStatus){
      fDeviceErrorCode|=kErrorFlag_sample;
    }
    //check SW and HW return the same sum
    bStatus= (GetRawHardwareSum()==GetRawSoftwareSum());
    //fEventIsGood =bStatus;
    if (!bStatus){
      fDeviceErrorCode|=kErrorFlag_SW_HW;
    }



    //check sequence number
    fSequenceNo_Prev++;
    if (fSequenceNo_Counter==0 || GetSequenceNumber()==0){//starting the data run
      fSequenceNo_Prev=GetSequenceNumber();
    }

    if (!MatchSequenceNumber(fSequenceNo_Prev)){//we have a sequence number error
      fEventIsGood=kFALSE;
      fDeviceErrorCode|=kErrorFlag_Sequence;
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
    }

    //check for the hw_sum is zero
    if (GetRawHardwareSum()==0){
      fDeviceErrorCode|=kErrorFlag_ZeroHW;
    }
    if (!fEventIsGood)
      fSequenceNo_Counter=0;//resetting the counter after ApplyHWChecks() a failure

  }
  else {
    fGoodEventCount = 1;
    fDeviceErrorCode = 0;
  }

  //UpdateHWErrorCounters(fDeviceErrorCode);//update the error counters based on the fDeviceErrorCode


 return fDeviceErrorCode;
};

/********************************************************/
void QwVQWK_Channel::UpdateHWErrorCounters(Int_t error_flag){
  if ( (kErrorFlag_sample &  error_flag)==kErrorFlag_sample)
    fErrorCount_sample++; //increment the hw error counter
  if ( (kErrorFlag_SW_HW &  error_flag)==kErrorFlag_SW_HW)
    fErrorCount_SW_HW++; //increment the hw error counter
  if ( (kErrorFlag_Sequence &  error_flag)==kErrorFlag_Sequence)
    fErrorCount_Sequence++; //increment the hw error counter
  if ( (kErrorFlag_SameHW &  error_flag)==kErrorFlag_SameHW)
    fErrorCount_SameHW++; //increment the hw error counter
  if ( (kErrorFlag_ZeroHW &  error_flag)==kErrorFlag_ZeroHW)
    fErrorCount_ZeroHW++; //increment the hw error counter
};
/********************************************************/

void QwVQWK_Channel::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  SetNumberOfDataWords(6);

  if      (datatosave == "raw")
    fDataToSave = kRaw;
  else if (datatosave == "derived")
    fDataToSave = kDerived;
  else
    fDataToSave = kRaw; // wdc, added default fall-through

  fPedestal            = 0.0;
  fCalibrationFactor   = 1.0;

  fSamplesPerBlock     = 16680; //jpan: total samples = fSamplesPerBlock x fBlocksPerEvent
  fBlocksPerEvent      = 4;

  fTreeArrayIndex      = 0;
  fTreeArrayNumEntries = 0;

  ClearEventData();

  fPreviousSequenceNumber = 0;
  fNumberOfSamples_map    = 0;

  // Use internal random variable by default
  fUseExternalRandomVariable = false;

  // Mock drifts
  fMockDriftAmplitude.clear();
  fMockDriftFrequency.clear();
  fMockDriftPhase.clear();

  // Mock asymmetries
  fMockAsymmetry     = 0.0;
  fMockGaussianMean  = 0.0;
  fMockGaussianSigma = 0.0;

  // Event cuts
  fULimit=0;
  fLLimit=0;
  fNumEvtsWithEventCutsRejected = 0;

  //init error counters//
  fErrorCount_sample     = 0;
  fErrorCount_SW_HW      = 0;
  fErrorCount_Sequence   = 0;
  fErrorCount_SameHW     = 0;
  fErrorCount_ZeroHW     = 0;

  fDeviceErrorCode       = 0;

  fADC_Same_NumEvt       = 0;
  fSequenceNo_Prev       = 0;
  fSequenceNo_Counter    = 0;
  fPrev_HardwareBlockSum = 0.0;

  fGoodEventCount        = 0;

  bEVENTCUTMODE          = 0;

  //std::cout<< "name = "<<name<<" error count same _HW = "<<fErrorCount_SameHW <<std::endl;
  return;
}

/********************************************************/
Int_t QwVQWK_Channel::GetEventcutErrorCounters()
{// report number of events failed due to HW and event cut failure
  ReportErrorCounters();//print the summary
  return 1;
};

void QwVQWK_Channel::ClearEventData()
{
  for (Short_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock_raw[i] = 0.0;
    fBlock[i] = 0.0;
    fBlockM2[i] = 0.0;
    fBlockError[i] = 0.0;
  }
  fHardwareBlockSum_raw = 0;
  fSoftwareBlockSum_raw = 0;
  fHardwareBlockSum   = 0.0;
  fHardwareBlockSumM2 = 0.0;
  fHardwareBlockSumError = 0.0;
  fSequenceNumber   = 0;
  fNumberOfSamples  = 0;
  fGoodEventCount   = 0;
  fDeviceErrorCode  = 0; // set to zero. Important for derrived devices.
  return;
};

void QwVQWK_Channel::RandomizeEventData(int helicity, double time)
{
  // The blocks are assumed to be independent measurements
  Double_t block[fBlocksPerEvent];
  Double_t sqrt_fBlocksPerEvent = 0.0;
  sqrt_fBlocksPerEvent = sqrt(fBlocksPerEvent);

  // Calculate drift (if time is not specified, it stays constant at zero)
  Double_t drift = 0.0;
  for (UInt_t i = 0; i < fMockDriftFrequency.size(); i++) {
    drift += fMockDriftAmplitude[i] * sin(2.0 * Qw::pi * fMockDriftFrequency[i] * time + fMockDriftPhase[i]);
  }

  // Calculate signal
  for (Short_t i = 0; i < fBlocksPerEvent; i++) {

    // External or internal randomness?
    double random_variable;
    if (fUseExternalRandomVariable)
      random_variable = fExternalRandomVariable; // external normal random variable
    else
      random_variable = fNormalRandomVariable(); // internal normal random variable

    block[i] =
        fMockGaussianMean * (1 + helicity * fMockAsymmetry) / fBlocksPerEvent
      + fMockGaussianSigma / sqrt_fBlocksPerEvent * random_variable
      + drift / fBlocksPerEvent;
  }

  SetEventData(block);
  return;
};

void QwVQWK_Channel::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  Double_t block[fBlocksPerEvent];
  for (Short_t i = 0; i < fBlocksPerEvent; i++)
    block[i] = hwsum / fBlocksPerEvent;
  SetEventData(block);
  return;
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
  return;
};

void QwVQWK_Channel::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  // Add new values
  fMockDriftAmplitude.push_back(amplitude);
  fMockDriftFrequency.push_back(frequency);
  fMockDriftPhase.push_back(phase);
  return;
};

void QwVQWK_Channel::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fMockGaussianMean = mean;
  fMockGaussianSigma = sigma;
  return;
};

void QwVQWK_Channel::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fMockAsymmetry = asymmetry;
  return;
};

void QwVQWK_Channel::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fHardwareBlockSum = 0.0;
  fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
  for (Short_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock[i] = block[i];
    fBlockM2[i] = 0.0; // second moment is zero for single events
    fHardwareBlockSum += block[i];
  }

  fSequenceNumber = sequencenumber;
  fNumberOfSamples = 16680;

  Double_t thispedestal = 0.0;
  thispedestal = fPedestal * fNumberOfSamples;

  for (Short_t i = 0; i < fBlocksPerEvent; i++)
    {
      fBlock_raw[i] = fBlock[i] / fCalibrationFactor +	thispedestal / (fBlocksPerEvent * 1.);
    }

  fHardwareBlockSum_raw = fHardwareBlockSum / fCalibrationFactor + thispedestal;
  fSoftwareBlockSum_raw = fHardwareBlockSum_raw;

  return;
};


Int_t QwVQWK_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;
  UInt_t localbuf[kWordsPerChannel] = {0};
  // The conversion from UInt_t to Double_t discards the sign, so we need an intermediate
  // static_cast from UInt_t to Int_t.
  Int_t localbuf_signed[kWordsPerChannel] = {0};

  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
    words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords)
    {
      for (Short_t i=0; i<kWordsPerChannel; i++){
	localbuf[i] = buffer[i];
        localbuf_signed[i] = static_cast<Int_t>(localbuf[i]);
      }

      fSoftwareBlockSum_raw = 0.0;
      for (Short_t i=0; i<fBlocksPerEvent; i++){
	fBlock_raw[i] = Double_t(localbuf_signed[i]);
	fSoftwareBlockSum_raw += fBlock_raw[i];
      }
      fHardwareBlockSum_raw = Double_t(localbuf_signed[4]);

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

        for (Short_t i=0; i<(kWordsPerChannel-1); i++){
	  std::cout<<"  hex("<<std::hex<<localbuf[i]<<") dec("<<std::dec<<Double_t(localbuf_signed[i])<<") ";
        }

        Double_t average = 0.0;
	average = Double_t(localbuf[4])/fNumberOfSamples;
//         std::cout<<std::endl<<" SoftwareBlockSum_raw="<<fSoftwareBlockSum_raw
//                  <<"  NumberOfSamples="<<fNumberOfSamples
//                  <<"  average="<<average
//                  <<"  avg_voltage="<< kVQWK_VoltsPerBit*average<<std::endl;
      }


      if (kDEBUG && GetElementName()=="MD1Pos")
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
    for (Short_t i = 0; i < 4; i++) {
        localbuf[i] = Long_t(fBlock_raw[i]);
        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
    }
    // The following causes many rounding errors and skips due to the check
    // that fHardwareBlockSum_raw == fSoftwareBlockSum_raw in IsGoodEvent().
    //localbuf[4] = Long_t(fHardwareBlockSum_raw);
    localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (Short_t i = 0; i < 6; i++){
        buffer.push_back(localbuf[i]);
    }
  }
};


void QwVQWK_Channel::ProcessEvent()
{
  Double_t thispedestal = 0.0;
  thispedestal = fPedestal * fNumberOfSamples;

  for (Short_t i = 0; i < fBlocksPerEvent; i++)
    {
      fBlock[i] = fCalibrationFactor * (fBlock_raw[i] - thispedestal / (fBlocksPerEvent*1.0));
      fBlockM2[i] = 0.0; // second moment is zero for single events
    }

  fHardwareBlockSum = fCalibrationFactor *  ( fHardwareBlockSum_raw - thispedestal );
  fHardwareBlockSumM2 = 0.0; // second moment is zero for single events

//   if(GetElementName().Contains("md"))
//     printf("Detector %s signal =  %1.4e\n",this->GetElementName().Data(),fHardwareBlockSum);

  return;
};

Double_t QwVQWK_Channel::GetAverageVolts() const
{
  //Double_t avgVolts = (fBlock[0]+fBlock[1]+fBlock[2]+fBlock[3])*kVQWK_VoltsPerBit/fNumberOfSamples;
  Double_t avgVolts = fHardwareBlockSum * kVQWK_VoltsPerBit / fNumberOfSamples;
  //std::cout<<"QwVQWK_Channel::GetAverageVolts() = "<<avgVolts<<std::endl;
  return avgVolts;

};

void QwVQWK_Channel::PrintInfo() const
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

  for (Short_t i = 0; i < fBlocksPerEvent; i++)
    std::cout << " : " << fBlock_raw[i];
  std::cout<<"\n";
  std::cout<<"fHardwareBlockSum_raw= "<<fHardwareBlockSum_raw<<"\n";
  std::cout<<"fSoftwareBlockSum_raw= "<<fSoftwareBlockSum_raw<<"\n";
  std::cout<<"fBlock ";
  for (Short_t i = 0; i < fBlocksPerEvent; i++)
    std::cout << " : " << fBlock[i];
  std::cout << std::endl;

  std::cout << "fHardwareBlockSum = "<<fHardwareBlockSum << std::endl;
  std::cout << "fHardwareBlockSumM2 = "<<fHardwareBlockSumM2 << std::endl;

  return;
};

void  QwVQWK_Channel::ConstructHistograms(TDirectory *folder, TString &prefix)
{
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
	for (Short_t i=0; i<fBlocksPerEvent; i++){
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
	Short_t index=0;
	for (Short_t i=0; i<fBlocksPerEvent; i++){
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

void  QwVQWK_Channel::FillHistograms()
{
  Short_t index=0;
  if (IsNameEmpty())
    {
      //  This channel is not used, so skip creating the histograms.
    } else
      {
	if(fDataToSave==kRaw)
	  {
	    for (Short_t i=0; i<fBlocksPerEvent; i++)
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
	      fHistograms[index]->Fill(this->GetRawSoftwareSum()-this->GetRawHardwareSum());
	  }
	else if(fDataToSave==kDerived)
	  {
	    for (Short_t i=0; i<fBlocksPerEvent; i++)
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

void  QwVQWK_Channel::DeleteHistograms()
{
  //std::cout<<"Device Name "<<GetElementName()<<" fHistograms.size() "<<fHistograms.size()<<std::endl;
  if ((fDataToSave==kRaw) || (fDataToSave==kDerived)){
  for (UInt_t i=0; i<fHistograms.size(); i++){
    if (fHistograms[i] != NULL)
      fHistograms[i]->Delete();
    fHistograms[i] = NULL;
  }
  }
  fHistograms.clear();

}

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
    //tree->Branch(basename,&fHardwareBlockSum);
    if (kDEBUG && GetElementName()=="MD1Pos"){
      std::cerr << "QwVQWK_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
		<< "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
		<< "; values.size()==" << values.size()
		<< "; list==" << list
		<< std::endl;
    }
  }
  return;
};

void  QwVQWK_Channel::ConstructBranch(TTree *tree, TString &prefix)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    tree->Branch(basename,&fHardwareBlockSum);
    if (kDEBUG){
      std::cerr << "QwVQWK_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
		<< "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
		<< std::endl;
    }
  }
  return;
};


void  QwVQWK_Channel::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries<=0){
    if (bDEBUG) std::cerr << "QwVQWK_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    if (bDEBUG) std::cerr << "QwVQWK_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    UInt_t index=fTreeArrayIndex;
    values[index++] = this->GetHardwareSum();
    for (Short_t i=0; i<fBlocksPerEvent; i++){
      values[index++] = this->GetBlockValue(i);
    }
    values[index++] = this->fNumberOfSamples;

    values[index++] = this->fDeviceErrorCode;
    if(fDataToSave==kRaw)
      {
	values[index++] = this->GetRawHardwareSum();
	for (Short_t i=0; i<fBlocksPerEvent; i++){
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
    UInt_t index=fTreeArrayIndex;
    for (UInt_t i=0; i<fTreeArrayNumEntries; i++){
      std::cerr << "values[" << index << "]==" << values[index] << " ";
      index++; // after previous statement to avoid ambiguity in ostream expression
    }
    std::cerr << std::endl;
  }
};



QwVQWK_Channel& QwVQWK_Channel::operator= (const QwVQWK_Channel &value)
{
  if(this ==&value) return *this;

  if (!IsNameEmpty())
    {
      for (Short_t i=0; i<fBlocksPerEvent; i++){
        this->fBlock_raw[i] = value.fBlock_raw[i];
        this->fBlock[i] = value.fBlock[i];
        this->fBlockM2[i] = value.fBlockM2[i];
      }
      this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;
      this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
      this->fHardwareBlockSum = value.fHardwareBlockSum;
      this->fHardwareBlockSumM2 = value.fHardwareBlockSumM2;
      this->fNumberOfSamples = value.fNumberOfSamples;
      this->fSequenceNumber  = value.fSequenceNumber;
      this->fDeviceErrorCode = (value.fDeviceErrorCode);//error code is updated.
    }

  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator+= (const QwVQWK_Channel &value)
{
  if (!IsNameEmpty()) {
    for (Short_t i = 0; i < fBlocksPerEvent; i++) {
      this->fBlock[i] += value.fBlock[i];
      this->fBlock_raw[i] += value.fBlock[i];
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum += value.fHardwareBlockSum;
    this->fHardwareBlockSumM2 = 0.0;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
  }

  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator-= (const QwVQWK_Channel &value)
{
  if (!IsNameEmpty()){
    for (Short_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock[i] -= value.fBlock[i];
      this->fBlock_raw[i] = 0;
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw = 0;
    this->fSoftwareBlockSum_raw = 0;
    this->fHardwareBlockSum -= value.fHardwareBlockSum;
    this->fHardwareBlockSumM2 = 0.0;
    this->fNumberOfSamples += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
  }

  return *this;
};

void QwVQWK_Channel::Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2)
{
  *this  = value1;
  *this += value2;
  return;
};

void QwVQWK_Channel::Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2)
{
  *this  = value1;
  *this -= value2;
  return;
};

void QwVQWK_Channel::Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom)
{
  if (!IsNameEmpty()) {

    // Take the ratio of the individual blocks
    for (Short_t i = 0; i < fBlocksPerEvent; i++) {
      if (denom.fBlock[i] != 0.0)
        fBlock[i] = (numer.fBlock[i]) / (denom.fBlock[i]);
      else
        fBlock[i] = 0.0;
      // raw is always zero on derived quantities
      fBlock_raw[i] = 0.0;
    }
    // Take the ratio of the hardware sum
    if (denom.fHardwareBlockSum != 0.0)
      fHardwareBlockSum = (numer.fHardwareBlockSum) / (denom.fHardwareBlockSum);
    else
      fHardwareBlockSum = 0.0;
    fHardwareBlockSum_raw = 0.0;
    fSoftwareBlockSum_raw = 0.0;

    // The variances are calculated using the following formula:
    //   Var[ratio] = ratio^2 (Var[numer] / numer^2 + Var[denom] / denom^2)
    //
    // This requires that both the numerator and denominator are non-zero!
    //
    for (Short_t i = 0; i < 4; i++) {
      if (numer.fBlock[i] != 0.0 && denom.fBlock[i] != 0.0)
        fBlockM2[i] = fBlock[i] * fBlock[i] *
           (numer.fBlockM2[i] / numer.fBlock[i] / numer.fBlock[i]
          + denom.fBlockM2[i] / denom.fBlock[i] / denom.fBlock[i]);
      else
        fBlockM2[i] = 0.0;
    }
    if (numer.fHardwareBlockSum != 0.0 && denom.fHardwareBlockSum != 0.0)
      fHardwareBlockSumM2 = fHardwareBlockSum * fHardwareBlockSum *
         (numer.fHardwareBlockSumM2 / numer.fHardwareBlockSum / numer.fHardwareBlockSum
        + denom.fHardwareBlockSumM2 / denom.fHardwareBlockSum / denom.fHardwareBlockSum);
    else
      fHardwareBlockSumM2 = 0.0;

    // Remaining variables
    fNumberOfSamples = denom.fNumberOfSamples;
    fSequenceNumber  = 0;
    fGoodEventCount  = denom.fGoodEventCount;
    fDeviceErrorCode = (numer.fDeviceErrorCode|denom.fDeviceErrorCode);//error code is ORed.
  }

  // Nanny
  if (fHardwareBlockSum != fHardwareBlockSum)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;

  return;
};

void QwVQWK_Channel::Product(QwVQWK_Channel &value1, QwVQWK_Channel &value2)
{
  if (!IsNameEmpty()){
    for (Short_t i = 0; i < fBlocksPerEvent; i++) {
      this->fBlock[i] = (value1.fBlock[i]) * (value2.fBlock[i]);
      this->fBlock_raw[i] = 0;
      // For a single event the second moment is still zero
      this->fBlockM2[i] = 0.0;
    }

    // For a single event the second moment is still zero
    this->fHardwareBlockSumM2 = 0.0;

    this->fSoftwareBlockSum_raw = 0;
    this->fHardwareBlockSum_raw = value1.fHardwareBlockSum_raw * value2.fHardwareBlockSum_raw;
    this->fHardwareBlockSum = value1.fHardwareBlockSum * value2.fHardwareBlockSum;
    this->fNumberOfSamples = value1.fNumberOfSamples;
    this->fSequenceNumber  = 0;
    this->fDeviceErrorCode = (value1.fDeviceErrorCode|value2.fDeviceErrorCode);//error code is ORed.
  }
  return;
};

void QwVQWK_Channel::AddChannelOffset(Double_t offset)
{
  Double_t blockoffset = offset / fBlocksPerEvent;

  if (!IsNameEmpty()){
      fHardwareBlockSum += fBlocksPerEvent*offset;
      for (Short_t i=0; i<fBlocksPerEvent; i++) fBlock[i] += blockoffset;
  }
  return;
};

void QwVQWK_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      for (Short_t i = 0; i < fBlocksPerEvent; i++) {
        fBlock[i] *= scale;
        fBlockM2[i] *= scale * scale;
      }
      fHardwareBlockSum *= scale;
      fHardwareBlockSumM2 *= scale * scale;
    }

  return;
};


/** Moments and uncertainty calculation on the running sums and averages
 *
 * The calculation of the first and second moments of the running sum is not
 * completely straightforward due to numerical instabilities associated with
 * small variances and large average values.  The naive computation taking
 * the difference of the square of the average and the average of the squares
 * leads to the subtraction of two very large numbers to get a small number.
 *
 * Alternative algorithms (including for higher order moments) are supplied in
 *  Pebay, Philippe (2008), "Formulas for Robust, One-Pass Parallel Computation
 *  of Covariances and Arbitrary-Order Statistical Moments", Technical Report
 *  SAND2008-6212, Sandia National Laboratories.
 *  http://infoserve.sandia.gov/sand_doc/2008/086212.pdf
 *
 * In the following formulas the moments \f$ M^1 \f$ and \f$ M^2 \f$ are defined
 * \f{eqnarray}
 *     M^1 & = & \frac{1}{n} \Sum^n y \\
 *     M^2 & = & \Sum^n (y - \mu)
 * \f}
 *
 * Recurrence relations for the addition of a single event:
 * \f{eqnarray}
 *     M^1_n & = & M^1_{n-1} + \frac{y - M^1_{n-1}}{n} \\
 *     M^2_n & = & M^2_{n-1} + (y - M^1_{n-1})(y - M^1_n)
 * \f}
 *
 * For the addition of an already accumulated sum:
 * \f{eqnarray}
 *     M^1 & = & M^1_1 + n_2 \frac{M^1_2 - M^1_1}{n} \\
 *     M^2 & = & M^2_1 + M^2_2 + n_1 n_2 \frac{(M^1_2 - M^1_1)^2}{n}
 * \f}
 *
 * In these recursive formulas we start from \f$ M^1 = y \f$ and \f$ M^2 = 0 \f$.
 *
 * To calculate the mean and standard deviation we use
 * \f{eqnarray}
 *          \mu & = & M^1 \\
 *     \sigma^2 & = & \frac{1}{n} M^2
 * \f}
 * The standard deviation is a biased estimator, but this is what ROOT uses.
 * Better would be to divide by \f$ (n-1) \f$.
 *
 * We use the formulas provided there for the calculation of the first and
 * second moments (i.e. average and variance).
 */
/**
 * Accumulate the running moments M1 and M2
 * @param value Object (single event or accumulated) to add to running moments
 */
void QwVQWK_Channel::AccumulateRunningSum(const QwVQWK_Channel& value)
{
  // Moment calculations
  Int_t n1 = fGoodEventCount;
  Int_t n2 = value.fGoodEventCount;
  // If there are no good events, check whether device HW is good
  if (n2 == 0 && value.fDeviceErrorCode == 0) {
    n2 = 1;
  }
  Int_t n = n1 + n2;

  // Set up variables
  Double_t M11 = fHardwareBlockSum;
  Double_t M12 = value.fHardwareBlockSum;
  Double_t M22 = value.fHardwareBlockSumM2;
  if (n2 == 0) {
    // no good events for addition
    return;
  } else if (n2 == 1) {
    // simple version for addition of single event
    fGoodEventCount++;
    fHardwareBlockSum += (M12 - M11) / n;
    fHardwareBlockSumM2 += (M12 - M11)
         * (M12 - fHardwareBlockSum); // note: using updated mean
    // and for individual blocks
    for (Short_t i = 0; i < 4; i++) {
      M11 = fBlock[i];
      M12 = value.fBlock[i];
      M22 = value.fBlockM2[i];
      fBlock[i] += (M12 - M11) / n;
      fBlockM2[i] += (M12 - M11) * (M12 - fBlock[i]); // note: using updated mean
    }
  } else if (n2 > 1) {
    // general version for addition of multi-event sets
    fGoodEventCount += n2;
    fHardwareBlockSum += n2 * (M12 - M11) / n;
    fHardwareBlockSumM2 += M22 + n1 * n2 * (M12 - M11) * (M12 - M11) / n;
    // and for individual blocks
    for (Short_t i = 0; i < 4; i++) {
      M11 = fBlock[i];
      M12 = value.fBlock[i];
      M22 = value.fBlockM2[i];
      fBlock[i] += n2 * (M12 - M11) / n;
      fBlockM2[i] += M22 + n1 * n2 * (M12 - M11) * (M12 - M11) / n;
    }
  }

  // Nanny
  if (fHardwareBlockSum != fHardwareBlockSum)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;

  return;
};



void QwVQWK_Channel::CalculateRunningAverage()
{
  if (fGoodEventCount <= 0)
    {
      for (Short_t i = 0; i < fBlocksPerEvent; i++) {
        fBlockError[i] = 0.0;
      }
      fHardwareBlockSumError = 0.0;
    }
  else
    {
      // We use a biased estimator by dividing by n.  Use (n - 1) to get the
      // unbiased estimator for the standard deviation.
      for (Short_t i = 0; i < fBlocksPerEvent; i++)
        fBlockError[i] = sqrt(fBlockM2[i] / fGoodEventCount);
      fHardwareBlockSumError = sqrt(fHardwareBlockSumM2 / fGoodEventCount);
    }
};


void QwVQWK_Channel::PrintValue() const
{
  QwMessage << std::setprecision(4)
            << std::setw(18) << std::left << GetElementName()      << " "
            << std::setw(12) << std::left << GetHardwareSum()      << "+/- "
            << std::setw(12) << std::left << GetHardwareSumError() << " "
            << std::setw(10) << std::left << GetGoodEventCount()   << " "
            << std::setw(12) << std::left << GetBlockValue(0)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(0) << " "
            << std::setw(12) << std::left << GetBlockValue(1)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(1) << " "
            << std::setw(12) << std::left << GetBlockValue(2)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(2) << " "
            << std::setw(12) << std::left << GetBlockValue(3)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(3) << " "
            << QwLog::endl;
}


/**
 * Blind this channel as an asymmetry
 * @param blinder Blinder
 */
void QwVQWK_Channel::Blind(const QwBlinder *blinder)
{
  if (!IsNameEmpty()) {
    for (Short_t i = 0; i < fBlocksPerEvent; i++)
      blinder->BlindValue(fBlock[i]);
    blinder->BlindValue(fHardwareBlockSum);
  }
  return;
};

/**
 * Blind this channel as a difference with specified yield
 * @param blinder Blinder
 * @param yield Corresponding yield
 */
void QwVQWK_Channel::Blind(const QwBlinder *blinder, const QwVQWK_Channel& yield)
{
  if (!IsNameEmpty()) {
    for (Short_t i = 0; i < fBlocksPerEvent; i++)
      blinder->BlindValue(fBlock[i], yield.fBlock[i]);
    blinder->BlindValue(fHardwareBlockSum, yield.fHardwareBlockSum);
  }
  return;
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
  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fNumberOfSamples==numsamp);
    if (! status){
      if (bDEBUG)
	std::cerr << "QwVQWK_Channel::MatchNumberOfSamples:  Channel "
		<< GetElementName()
		<< " had fNumberOfSamples==" << fNumberOfSamples
		<< " and was supposed to have " << numsamp
		<< std::endl;
      //      PrintChannel();
    }
  }
  return status;
};

Bool_t QwVQWK_Channel::ApplySingleEventCuts(Double_t LL=0,Double_t UL=0)//only check to see HW_Sum is within these given limits
{
  Bool_t status = kFALSE;

  if (LL==0 && UL==0){
    status=kTRUE;
  } else  if (GetHardwareSum()<=UL && GetHardwareSum()>=LL){
    if (!fDeviceErrorCode)
      status=kTRUE;
    else
      status=kFALSE;//If the device HW is failed
  }
  std::cout<<this->fDeviceErrorCode<<std::endl;
  return status;
};

void QwVQWK_Channel::SetSingleEventCuts(Double_t min, Double_t max){
  fULimit=max;
  fLLimit=min;
};

Bool_t QwVQWK_Channel::ApplySingleEventCuts()//This will check the limits and update event_flags and error counters
{
  Bool_t status;
  if (bEVENTCUTMODE>=2){//Global switch to ON/OFF event cuts set at the event cut file

    if (fULimit==0 && fLLimit==0){
      status=kTRUE;
    } else  if (GetHardwareSum()<=fULimit && GetHardwareSum()>=fLLimit){
      if (!fDeviceErrorCode)
	status=kTRUE;
      else
	status=kFALSE;//If the device HW is failed
    }
    else{
      if (GetHardwareSum()> fULimit)
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
	 this->fElementName           = input->fElementName;
	 this->fPedestal              = input->GetPedestal();
	 this->fCalibrationFactor     = input->GetCalibrationFactor();
	 this->fDataToSave            = kDerived;
	 this->fDeviceErrorCode       = input->fDeviceErrorCode;
	 this->fGoodEventCount        = input->fGoodEventCount;

	 this->fHardwareBlockSum      = input->fHardwareBlockSum;
	 this->fHardwareBlockSumError = input->fHardwareBlockSumError;

	 for(UShort_t i=0; i<4; i++ ) {
	   this->fBlock[i] = input->fBlock[i];
	   this->fBlockError[i] = input->fBlockError[i];
	 }

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

void  QwVQWK_Channel::ReportErrorCounters()
{

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
  return;
};
