#include "QwVQWK_Channel.h"
 
// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwBlinder.h"
#include "QwHistogramHelper.h"


const Bool_t QwVQWK_Channel::kDEBUG = kFALSE;

const Int_t  QwVQWK_Channel::kWordsPerChannel = 6;
const Int_t  QwVQWK_Channel::kMaxChannels     = 8;


/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is roughly 76 uV per count, and zero counts corresponds
 *   to zero voltage.
 *   Store as the exact value for 20 V range, 18 bit ADC.
 */
const Double_t QwVQWK_Channel::kVQWK_VoltsPerBit = (20./(1<<18));

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
  //  fDeviceErrorCode=0;
  if (bEVENTCUTMODE>0){//Global switch to ON/OFF event cuts set at the event cut file

    if (bDEBUG)
      QwWarning<<" QwQWVK_Channel "<<GetElementName()<<"  "<<GetNumberOfSamples()<<QwLog::endl;

    // Sample size check
    bStatus = MatchNumberOfSamples(fNumberOfSamples_map);//compare the default sample size with no.of samples read by the module
    if (!bStatus) {
      fDeviceErrorCode |= kErrorFlag_sample;
    }

    // Check SW and HW return the same sum
    bStatus = (GetRawHardwareSum() == GetRawSoftwareSum());
    //fEventIsGood = bStatus;
    if (!bStatus) {
      fDeviceErrorCode |= kErrorFlag_SW_HW;
    }



    //check sequence number
    fSequenceNo_Prev++;
    if (fSequenceNo_Counter==0 || GetSequenceNumber()==0){//starting the data run
      fSequenceNo_Prev=GetSequenceNumber();
    }

    if (!MatchSequenceNumber(fSequenceNo_Prev)){//we have a sequence number error
      fEventIsGood=kFALSE;
      fDeviceErrorCode|=kErrorFlag_Sequence;
      if (bDEBUG)       QwWarning<<" QwQWVK_Channel "<<GetElementName()<<" Sequence number  previous value = "<<fSequenceNo_Prev<<" Current value= "<< GetSequenceNumber()<<QwLog::endl;
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
      if (bDEBUG) QwWarning<<" BCM hardware sum is same for more than  "<<fADC_Same_NumEvt<<" time consecutively  "<<QwLog::endl;
      fDeviceErrorCode|=kErrorFlag_SameHW;
    }

    //check for the hw_sum is zero
    if (GetRawHardwareSum()==0){
      fDeviceErrorCode|=kErrorFlag_ZeroHW;
    }
    if (!fEventIsGood)    
      fSequenceNo_Counter=0;//resetting the counter after ApplyHWChecks() a failure

    if ((TMath::Abs(GetRawHardwareSum())*kVQWK_VoltsPerBit/fNumberOfSamples) > GetVQWKSaturationLimt()){
      if (bDEBUG) 
	QwWarning << this->GetElementName()<<" "<<GetRawHardwareSum() << "Saturating VQWK invoked! " <<TMath::Abs(GetRawHardwareSum())*kVQWK_VoltsPerBit/fNumberOfSamples<<" Limit "<<GetVQWKSaturationLimt() << QwLog::endl;
      fDeviceErrorCode|=kErrorFlag_VQWK_Sat; 
    }

  }
  else {
    fGoodEventCount = 1;
    fDeviceErrorCode = 0;
  }

  //UpdateHWErrorCounters(fDeviceErrorCode);//update the error counters based on the fDeviceErrorCode


 return fDeviceErrorCode;
}


/********************************************************/
void QwVQWK_Channel::UpdateErrorCounters(UInt_t error_flag){
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
  if ( (kErrorFlag_VQWK_Sat &  error_flag)==kErrorFlag_VQWK_Sat)
    fErrorCount_HWSat++; //increment the hw saturation error counter
  if ( ((kErrorFlag_EventCut_L &  error_flag)==kErrorFlag_EventCut_L) || ((kErrorFlag_EventCut_U &  error_flag)==kErrorFlag_EventCut_U))
    fNumEvtsWithEventCutsRejected++; //increment the event cut error counter
  
}
/********************************************************/

void QwVQWK_Channel::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  SetDataToSave(datatosave);
  SetNumberOfDataWords(6);
  SetNumberOfSubElements(5);

  fDeviceErrorCode=0;//Initialize the device error code
  fErrorFlag=0;//Initialize the error flag

  kFoundPedestal = 0;
  kFoundGain = 0;

  fPedestal            = 0.0;
  fCalibrationFactor   = 1.0;

  fBlocksPerEvent      = 4;

  fTreeArrayIndex      = 0;
  fTreeArrayNumEntries = 0;

  ClearEventData();

  fPreviousSequenceNumber = 0;
  fNumberOfSamples_map    = 0;
  fNumberOfSamples        = 0;

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
  fErrorCount_HWSat      = 0;


  fDeviceErrorCode       = 0;
  fDefErrorFlag          = 0;

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

void QwVQWK_Channel::InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave){
  InitializeChannel(name,datatosave);
  SetSubsystemName(subsystem);
  SetModuleType(instrumenttype);
  //PrintInfo();
}

/********************************************************/
Int_t QwVQWK_Channel::GetEventcutErrorCounters()
{// report number of events failed due to HW and event cut failure
  ReportErrorCounters();//print the summary
  return 1;
}

void QwVQWK_Channel::ClearEventData()
{
  for (Int_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock_raw[i] = 0;
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
  fErrorFlag=fDefErrorFlag;
  return;
}

void QwVQWK_Channel::RandomizeEventData(int helicity, double time)
{
  // The blocks are assumed to be independent measurements
  Double_t* block = new Double_t[fBlocksPerEvent];
  Double_t sqrt_fBlocksPerEvent = 0.0;
  sqrt_fBlocksPerEvent = sqrt(fBlocksPerEvent);

  // Calculate drift (if time is not specified, it stays constant at zero)
  Double_t drift = 0.0;
  for (UInt_t i = 0; i < fMockDriftFrequency.size(); i++) {
    drift += fMockDriftAmplitude[i] * sin(2.0 * Qw::pi * fMockDriftFrequency[i] * time + fMockDriftPhase[i]);
  }

  // Calculate signal
  for (Int_t i = 0; i < fBlocksPerEvent; i++) {
    block[i] = 
      fMockGaussianMean * (1 + helicity * fMockAsymmetry) / fBlocksPerEvent
      + fMockGaussianSigma / sqrt_fBlocksPerEvent * GetRandomValue()
      + drift / fBlocksPerEvent;
  }

  SetEventData(block);

  delete block;
  return;
}

void QwVQWK_Channel::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  Double_t* block = new Double_t[fBlocksPerEvent];
  for (Int_t i = 0; i < fBlocksPerEvent; i++)
    block[i] = hwsum / fBlocksPerEvent;
  SetEventData(block);

  delete block;
  return;
}


// SetEventData() is used by the mock data generator to turn "model"
// data values into their equivalent raw data.  It should be used
// nowhere else.  -- pking, 2010-09-16
void QwVQWK_Channel::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fHardwareBlockSum = 0.0;
  fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
  for (Int_t i = 0; i < fBlocksPerEvent; i++) {
    fBlock[i] = block[i];
    fBlockM2[i] = 0.0; // second moment is zero for single events
    fHardwareBlockSum += block[i];
  }

  fSequenceNumber = sequencenumber;
  fNumberOfSamples = 16680;

  Double_t thispedestal = 0.0;
  thispedestal = fPedestal * fNumberOfSamples;

  fHardwareBlockSum_raw = 0;
  for (Int_t i = 0; i < fBlocksPerEvent; i++)
    {
      fBlock_raw[i] = Int_t(fBlock[i] / fCalibrationFactor +	thispedestal / (fBlocksPerEvent * 1.));
      fHardwareBlockSum_raw += fBlock_raw[i];
    }
  //  fHardwareBlockSum_raw = fHardwareBlockSum / fCalibrationFactor + thispedestal;
  fSoftwareBlockSum_raw = fHardwareBlockSum_raw;

  return;
}

void QwVQWK_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  Long_t localbuf[6] = {0};

  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    //    localbuf[4] = 0;
    for (Int_t i = 0; i < 4; i++) {
      localbuf[i] = fBlock_raw[i];
      //        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
    }
    // The following causes many rounding errors and skips due to the check
    // that fHardwareBlockSum_raw == fSoftwareBlockSum_raw in IsGoodEvent().
    localbuf[4] = fHardwareBlockSum_raw;
    localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (Int_t i = 0; i < 6; i++){
        buffer.push_back(localbuf[i]);
    }
  }
}



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
      for (Int_t i=0; i<kWordsPerChannel; i++){
	localbuf[i] = buffer[i];
        localbuf_signed[i] = static_cast<Int_t>(localbuf[i]);
      }

      fSoftwareBlockSum_raw = 0;
      for (Int_t i=0; i<fBlocksPerEvent; i++){
	fBlock_raw[i] = localbuf_signed[i];
	fSoftwareBlockSum_raw += fBlock_raw[i];
      }
      fHardwareBlockSum_raw = localbuf_signed[4];

      /*  Permanent change in the structure of the 6th word of the ADC readout.
       *  The upper 16 bits are the number of samples, and the upper 8 of the
       *  lower 16 are the sequence number.  This matches the structure of
       *  the ADC readout in block read mode, and now also in register read mode.
       *  P.King, 2007sep04.
       */
      fSequenceNumber   = (localbuf[5]>>8)  & 0xFF;
      fNumberOfSamples  = (localbuf[5]>>16) & 0xFFFF;

      words_read = fNumberOfDataWords;

    } else
      {
	std::cerr << "QwVQWK_Channel::ProcessEvBuffer: Not enough words!"
		  << std::endl;
      }
  return words_read;
}



void QwVQWK_Channel::ProcessEvent()
{
  if (fNumberOfSamples == 0 && fHardwareBlockSum_raw == 0) {
    //  There isn't valid data for this channel.  Just flag it and
    //  move on.
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      fBlock[i] = 0.0;
      fBlockM2[i] = 0.0;
    }
    fHardwareBlockSum = 0.0;
    fHardwareBlockSumM2 = 0.0;
    fDeviceErrorCode |= kErrorFlag_sample;
  } else if (fNumberOfSamples == 0) {
    //  This is probably a more serious problem.
    QwWarning << "QwVQWK_Channel::ProcessEvent:  Channel "
	      << this->GetElementName().Data()
	      << " has fNumberOfSamples==0 but has valid data in the hardware sum.  "
	      << "Flag this as an error."
	      << QwLog::endl;
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      fBlock[i] = 0.0;
      fBlockM2[i] = 0.0;
    }
    fHardwareBlockSum = 0.0;
    fHardwareBlockSumM2 = 0.0;
    fDeviceErrorCode|=kErrorFlag_sample;
  } else {
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      fBlock[i] = fCalibrationFactor * ( (1.0 * fBlock_raw[i] * fBlocksPerEvent / fNumberOfSamples) - fPedestal );
      fBlockM2[i] = 0.0; // second moment is zero for single events
    }
    fHardwareBlockSum = fCalibrationFactor * ( (1.0 * fHardwareBlockSum_raw / fNumberOfSamples) - fPedestal );
    fHardwareBlockSumM2 = 0.0; // second moment is zero for single events
    //   if(GetElementName().Contains("md"))
    //     printf("Detector %s signal =  %1.4e\n",this->GetElementName().Data(),fHardwareBlockSum);
  }
  return;
}

Double_t QwVQWK_Channel::GetAverageVolts() const
{
  //Double_t avgVolts = (fBlock[0]+fBlock[1]+fBlock[2]+fBlock[3])*kVQWK_VoltsPerBit/fNumberOfSamples;
  Double_t avgVolts = fHardwareBlockSum * kVQWK_VoltsPerBit / fNumberOfSamples;
  //std::cout<<"QwVQWK_Channel::GetAverageVolts() = "<<avgVolts<<std::endl;
  return avgVolts;

}

void QwVQWK_Channel::PrintInfo() const
{
  std::cout<<"***************************************"<<"\n";
  std::cout<<"Subsystem "<<GetSubsystemName()<<"\n"<<"\n";
  std::cout<<"Beam Instrument Type: "<<GetModuleType()<<"\n"<<"\n";
  std::cout<<"QwVQWK channel: "<<GetElementName()<<"\n"<<"\n";
  std::cout<<"fPedestal= "<< fPedestal<<"\n";
  std::cout<<"fCalibrationFactor= "<<fCalibrationFactor<<"\n";
  std::cout<<"fBlocksPerEvent= "<<fBlocksPerEvent<<"\n"<<"\n";
  std::cout<<"fSequenceNumber= "<<fSequenceNumber<<"\n";
  std::cout<<"fNumberOfSamples= "<<fNumberOfSamples<<"\n";
  std::cout<<"fBlock_raw ";

  for (Int_t i = 0; i < fBlocksPerEvent; i++)
    std::cout << " : " << fBlock_raw[i];
  std::cout<<"\n";
  std::cout<<"fHardwareBlockSum_raw= "<<fHardwareBlockSum_raw<<"\n";
  std::cout<<"fSoftwareBlockSum_raw= "<<fSoftwareBlockSum_raw<<"\n";
  std::cout<<"fBlock ";
  for (Int_t i = 0; i < fBlocksPerEvent; i++)
    std::cout << " : " << fBlock[i];
  std::cout << std::endl;

  std::cout << "fHardwareBlockSum = "<<fHardwareBlockSum << std::endl;
  std::cout << "fHardwareBlockSumM2 = "<<fHardwareBlockSumM2 << std::endl;
  std::cout << "fHardwareBlockSumError = "<<fHardwareBlockSumError << std::endl;

  return;
}

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
	for (Int_t i=0; i<fBlocksPerEvent; i++){
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
	Int_t index=0;
	for (Int_t i=0; i<fBlocksPerEvent; i++){
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
}

void  QwVQWK_Channel::FillHistograms()
{
  Int_t index=0;
  if (IsNameEmpty())
    {
      //  This channel is not used, so skip creating the histograms.
    } else if (fDeviceErrorCode==0)//Fill only if no HW error in the channel
      {
	if(fDataToSave==kRaw)
	  {
	    for (Int_t i=0; i<fBlocksPerEvent; i++)
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
	    for (Int_t i=0; i<fBlocksPerEvent; i++)
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
}

void  QwVQWK_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{

  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    TString list="";

    bHw_sum=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "hw_sum");
    bHw_sum_raw=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "hw_sum_raw");
    bBlock=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "block");
    bBlock_raw=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "block_raw");
    bNum_samples=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "num_samples");
    bDevice_Error_Code=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "Device_Error_Code");
    bSequence_number=gQwHists.MatchVQWKElementFromList(GetSubsystemName().Data(), GetModuleType().Data(), "sequence_number");

    if (bHw_sum){
      values.push_back(0.0);
      list += "hw_sum/D";
    }
    if (bBlock){
      values.push_back(0.0);
      list += ":block0/D";

      values.push_back(0.0);
      list += ":block1/D";

      values.push_back(0.0);
      list += ":block2/D";

      values.push_back(0.0);
      list += ":block3/D";
    }

    if (bNum_samples){
      values.push_back(0.0);
      list += ":num_samples/D";
    }

    if (bDevice_Error_Code){
      values.push_back(0.0);
      list += ":Device_Error_Code/D";
    }

    if(fDataToSave==kRaw)
      {
	if (bHw_sum_raw){
	  values.push_back(0.0);
	  list += ":hw_sum_raw/D";
	}
	if (bBlock_raw){
	  values.push_back(0.0);
	  list += ":block0_raw/D";

	  values.push_back(0.0);
	  list += ":block1_raw/D";

	  values.push_back(0.0);
	  list += ":block2_raw/D";

	  values.push_back(0.0);
	  list += ":block3_raw/D";
	}
	if (bSequence_number){
	  values.push_back(0.0);
	  list += ":sequence_number/D";
	}
      }

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
 
    if (gQwHists.MatchDeviceParamsFromList(basename.Data()) && (bHw_sum || bBlock || bNum_samples || bDevice_Error_Code || bHw_sum_raw || bBlock_raw || bSequence_number)){

      if (list=="hw_sum/D")//this is for the RT mode
	list=basename+"/D";	
      
      if (kDEBUG)
	QwMessage <<"base name "<<basename<<" List "<<list<<  QwLog::endl;
      tree->Branch(basename, &(values[fTreeArrayIndex]), list);
    }

    if (kDEBUG){
      std::cerr << "QwVQWK_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
		<< "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
		<< "; values.size()==" << values.size()
		<< "; list==" << list
		<< std::endl;
    }
  }
  //exit(1);
  return;
}

void  QwVQWK_Channel::ConstructBranch(TTree *tree, TString &prefix)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    tree->Branch(basename,&fHardwareBlockSum,basename+"/D");
    if (kDEBUG){
      std::cerr << "QwVQWK_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
		<< "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
		<< std::endl;
    }
  }
  return;
}


void  QwVQWK_Channel::FillTreeVector(std::vector<Double_t> &values) const
{
  if (IsNameEmpty()) {
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries <= 0) {
    if (bDEBUG) std::cerr << "QwVQWK_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    if (bDEBUG) std::cerr << "QwVQWK_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    UInt_t index = fTreeArrayIndex;
    //hw_sum
    if (bHw_sum)
      values[index++] = this->GetHardwareSum();

    if (bBlock){
      for (Int_t i=0; i<fBlocksPerEvent; i++){
	//blocki
	values[index++] = this->GetBlockValue(i);
      }
    }

    //num_samples
    if (bNum_samples)
      values[index++] = this->fNumberOfSamples;

    //Device_Error_Code
    if (bDevice_Error_Code)
      values[index++] = this->fDeviceErrorCode;

    if(fDataToSave==kRaw)
      {
	//hw_sum_raw
	if (bHw_sum_raw)
	  values[index++] = this->GetRawHardwareSum();

	if (bBlock_raw){
	  for (Int_t i=0; i<fBlocksPerEvent; i++){
	    //blocki_raw
	    values[index++] = this->GetRawBlockValue(i);
	  }
	}

	//sequence_number
	if (bSequence_number)
	  values[index++]=this->fSequenceNumber;
      }
  }
}

// VQwDataElement& QwVQWK_Channel::operator= (const  VQwDataElement& data_value)
// {
//   QwVQWK_Channel * value;
//   value=(QwVQWK_Channel *)&data_value;
//   if(this ==value) return *this;

//   if (!IsNameEmpty())
//     {
//       for (Int_t i=0; i<fBlocksPerEvent; i++){
//         this->fBlock_raw[i] = value->fBlock_raw[i];
//         this->fBlock[i] = value->fBlock[i];
//         this->fBlockM2[i] = value->fBlockM2[i];
//       }
//       this->fHardwareBlockSum_raw = value->fHardwareBlockSum_raw;
//       this->fSoftwareBlockSum_raw = value->fSoftwareBlockSum_raw;
//       this->fHardwareBlockSum = value->fHardwareBlockSum;
//       this->fHardwareBlockSumM2 = value->fHardwareBlockSumM2;
//       this->fHardwareBlockSumError = value->fHardwareBlockSumError;
//       this->fGoodEventCount=value->fGoodEventCount;
//       this->fNumberOfSamples = value->fNumberOfSamples;
//       this->fSequenceNumber  = value->fSequenceNumber;
//       this->fErrorFlag       = (value->fErrorFlag);
//       this->fDeviceErrorCode = (value->fDeviceErrorCode);
//     }

//   return *this;
// }



QwVQWK_Channel& QwVQWK_Channel::operator= (const QwVQWK_Channel &value)
{
  if(this ==&value) return *this;

  if (!IsNameEmpty()) {
    VQwHardwareChannel::operator=(value);
    for (Int_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock_raw[i] = value.fBlock_raw[i];
      this->fBlock[i]     = value.fBlock[i];
      this->fBlockM2[i]   = value.fBlockM2[i];
    }
    this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum = value.fHardwareBlockSum;
    this->fHardwareBlockSumM2 = value.fHardwareBlockSumM2;
    this->fHardwareBlockSumError = value.fHardwareBlockSumError;
    this->fNumberOfSamples = value.fNumberOfSamples;
    this->fSequenceNumber  = value.fSequenceNumber;
  }
  return *this;
}

void QwVQWK_Channel::AssignValueFrom(const  VQwDataElement* valueptr)
{
  const QwVQWK_Channel* tmpptr;
  tmpptr = dynamic_cast<const QwVQWK_Channel*>(valueptr);
  if (tmpptr!=NULL){
    *this = *tmpptr;
  } else {
    TString loc="Standard exception from QwVQWK_Channel::AssignValueFrom = "
      +valueptr->GetElementName()+" is an incompatable type.";
    throw std::invalid_argument(loc.Data());
  }
}

const QwVQWK_Channel QwVQWK_Channel::operator+ (const QwVQWK_Channel &value) const
{
  QwVQWK_Channel result = *this;
  result += value;
  return result;
}

QwVQWK_Channel& QwVQWK_Channel::operator+= (const QwVQWK_Channel &value)
{
  if (!IsNameEmpty()) {
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
      this->fBlock[i] += value.fBlock[i];
      this->fBlock_raw[i] += value.fBlock_raw[i];
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw = value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw = value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum += value.fHardwareBlockSum;
    this->fHardwareBlockSumM2 = 0.0;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
    this->fErrorFlag       |= (value.fErrorFlag);
  }

  return *this;
}

const QwVQWK_Channel QwVQWK_Channel::operator- (const QwVQWK_Channel &value) const
{
  QwVQWK_Channel result = *this;
  result -= value;
  return result;
}

QwVQWK_Channel& QwVQWK_Channel::operator-= (const QwVQWK_Channel &value)
{
  if (!IsNameEmpty()){
    for (Int_t i=0; i<fBlocksPerEvent; i++){
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
    this->fErrorFlag       |= (value.fErrorFlag);
  }

  return *this;
}

const QwVQWK_Channel QwVQWK_Channel::operator* (const QwVQWK_Channel &value) const
{
  QwVQWK_Channel result = *this;
  result *= value;
  return result;
}

QwVQWK_Channel& QwVQWK_Channel::operator*= (const QwVQWK_Channel &value)
{
  if (!IsNameEmpty()){
    for (Int_t i=0; i<fBlocksPerEvent; i++){
      this->fBlock[i] *= value.fBlock[i];
      this->fBlock_raw[i] *= value.fBlock_raw[i];
      this->fBlockM2[i] = 0.0;
    }
    this->fHardwareBlockSum_raw *= value.fHardwareBlockSum_raw;
    this->fSoftwareBlockSum_raw *= value.fSoftwareBlockSum_raw;
    this->fHardwareBlockSum *= value.fHardwareBlockSum;
    this->fHardwareBlockSumM2 = 0.0;
    this->fNumberOfSamples *= value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
    this->fErrorFlag       |= (value.fErrorFlag);
  }

  return *this;
}

void QwVQWK_Channel::Sum(const QwVQWK_Channel &value1, const QwVQWK_Channel &value2)
{
  *this  = value1;
  *this += value2;
  return;
}

void QwVQWK_Channel::Difference(const QwVQWK_Channel &value1, const QwVQWK_Channel &value2)
{
  *this  = value1;
  *this -= value2;
  return;
}

void QwVQWK_Channel::Ratio(const QwVQWK_Channel &numer, const QwVQWK_Channel &denom)
{
  if (!IsNameEmpty()) {
    *this  = numer;
    *this /= denom;

    //  Set the raw values to zero.
    for (Int_t i = 0; i < fBlocksPerEvent; i++) fBlock_raw[i] = 0;
    fHardwareBlockSum_raw = 0;
    fSoftwareBlockSum_raw = 0;
    // Remaining variables
    fNumberOfSamples = denom.fNumberOfSamples;
    fSequenceNumber  = 0;
    fGoodEventCount  = denom.fGoodEventCount;
    fDeviceErrorCode = (numer.fDeviceErrorCode|denom.fDeviceErrorCode);//error code is ORed.
    fErrorFlag       = (numer.fErrorFlag|denom.fErrorFlag);
  }
  return;
}

QwVQWK_Channel& QwVQWK_Channel::operator/= (const QwVQWK_Channel &denom)
{
  //  In this function, leave the "raw" variables untouched.
  //  
  Double_t ratio;
  Double_t variance;
  if (!IsNameEmpty()) {
    // The variances are calculated using the following formula:
    //   Var[ratio] = ratio^2 (Var[numer] / numer^2 + Var[denom] / denom^2)
    //
    // This requires that both the numerator and denominator are non-zero!
    //
    for (Int_t i = 0; i < 4; i++) {
      if (this->fBlock[i] != 0.0 && denom.fBlock[i] != 0.0){
	ratio    = (this->fBlock[i]) / (denom.fBlock[i]);
        variance =  ratio * ratio *
           (this->fBlockM2[i] / this->fBlock[i] / this->fBlock[i]
          + denom.fBlockM2[i] / denom.fBlock[i] / denom.fBlock[i]);
	fBlock[i]   = ratio;
	fBlockM2[i] = variance;
      } else if (this->fBlock[i] == 0.0) {
	this->fBlock[i]   = 0.0;
        this->fBlockM2[i] = 0.0;
      } else {
        QwVerbose << "Attempting to divide by zero block in " 
		  << GetElementName() << QwLog::endl;
	fBlock[i]   = 0.0;
        fBlockM2[i] = 0.0;
      }
    }
    if (this->fHardwareBlockSum != 0.0 && denom.fHardwareBlockSum != 0.0){
      ratio    =  (this->fHardwareBlockSum) / (denom.fHardwareBlockSum);
      variance =  ratio * ratio *
	(this->fHardwareBlockSumM2 / this->fHardwareBlockSum / this->fHardwareBlockSum
	 + denom.fHardwareBlockSumM2 / denom.fHardwareBlockSum / denom.fHardwareBlockSum);
      fHardwareBlockSum   = ratio;
      fHardwareBlockSumM2 = variance;
    } else if (this->fHardwareBlockSum == 0.0) {
      fHardwareBlockSum   = 0.0;
      fHardwareBlockSumM2 = 0.0;
    } else {
      QwVerbose << "Attempting to divide by zero sum in " 
		<< GetElementName() << QwLog::endl;
      fHardwareBlockSumM2 = 0.0;
    }
    // Remaining variables
    //  Don't change fNumberOfSamples, fSequenceNumber, fGoodEventCount,
    //  or fErrorFlag.
    //  'OR' the device error codes together.
    fDeviceErrorCode |= denom.fDeviceErrorCode;
  }

  // Nanny
  if (fHardwareBlockSum != fHardwareBlockSum)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;

  return *this;
}

void QwVQWK_Channel::Product(const QwVQWK_Channel &value1, const QwVQWK_Channel &value2)
{
  if (!IsNameEmpty()){
    for (Int_t i = 0; i < fBlocksPerEvent; i++) {
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
    this->fErrorFlag       = (value1.fErrorFlag|value2.fErrorFlag);
  }
  return;
}

/**
This function will add a offset to the hw_sum and add offset/fBlocksPerEvent for blocks.
 */
void QwVQWK_Channel::AddChannelOffset(Double_t offset)
{
  Double_t blockoffset = offset / fBlocksPerEvent;

  if (!IsNameEmpty()){
      fHardwareBlockSum += offset;
      for (Int_t i=0; i<fBlocksPerEvent; i++) fBlock[i] += blockoffset;
  }
  return;
}

void QwVQWK_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      for (Int_t i = 0; i < fBlocksPerEvent; i++) {
        fBlock[i] *= scale;
        fBlockM2[i] *= scale * scale;
      }
      fHardwareBlockSum *= scale;
      fHardwareBlockSumM2 *= scale * scale;
    }
}


void QwVQWK_Channel::DivideBy(const QwVQWK_Channel &denom)
{
  *this /= denom;
}






/** Moments and uncertainty calculation on the running sums and averages
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
  }else if (n2 == -1 && value.fDeviceErrorCode == 0) {
    n2 = -1;
  }else
    n2 = -100;//ignore it
  Int_t n = n1 + n2;

  // Set up variables
  Double_t M11 = fHardwareBlockSum;
  Double_t M12 = value.fHardwareBlockSum;
  Double_t M22 = value.fHardwareBlockSumM2;
  if (n2 == 0) {
    // no good events for addition
    return;
  } else if (n2 == -1) {
    // simple version for removal of single event from the sum
    fGoodEventCount--;
    //QwMessage<<"Deaccumulate before "<<QwLog::endl;
    if (n>0){
      fHardwareBlockSum -= (M12 - M11) / n;
      fHardwareBlockSumM2 -= (M12 - M11)
	* (M12 - fHardwareBlockSum); // note: using updated mean
      // and for individual blocks
      for (Int_t i = 0; i < 4; i++) {
	M11 = fBlock[i];
	M12 = value.fBlock[i];
	M22 = value.fBlockM2[i];
	fBlock[i] -= (M12 - M11) / n;
	fBlockM2[i] -= (M12 - M11) * (M12 - fBlock[i]); // note: using updated mean
      }
      //QwMessage<<"Deaccumulate "<<QwLog::endl;
    }else if (n==0){
      //QwMessage<<"Deaccumulate at zero "<<QwLog::endl;
      /*
      fHardwareBlockSum -= (M12 - M11) / n;
      fHardwareBlockSumM2 -= (M12 - M11) * (M12 - fHardwareBlockSum); // note: using updated mean
      // and for individual blocks
      for (Int_t i = 0; i < 4; i++) {
	M11 = fBlock[i];
	M12 = value.fBlock[i];
	M22 = value.fBlockM2[i];
	fBlock[i] -= (M12 - M11) / n;
	fBlockM2[i] -= (M12 - M11) * (M12 - fBlock[i]); // note: using updated mean
      }
      */
    }

  } else if (n2 == 1) {
    // simple version for addition of single event
    fGoodEventCount++;
    fHardwareBlockSum += (M12 - M11) / n;
    fHardwareBlockSumM2 += (M12 - M11)
         * (M12 - fHardwareBlockSum); // note: using updated mean
    // and for individual blocks
    for (Int_t i = 0; i < 4; i++) {
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
    for (Int_t i = 0; i < 4; i++) {
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
}


void QwVQWK_Channel::CalculateRunningAverage()
{
  if (fGoodEventCount <= 0)
    {
      for (Int_t i = 0; i < fBlocksPerEvent; i++) {
        fBlockError[i] = 0.0;
      }
      fHardwareBlockSumError = 0.0;
    }
  else
    {
      // We use a biased estimator by dividing by n.  Use (n - 1) to get the
      // unbiased estimator for the standard deviation.
      //
      // Note we want to calculate the error here, not sigma:
      //    sigma = sqrt(M2 / n);
      //    error = sigma / sqrt (n) = sqrt(M2) / n;
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
        fBlockError[i] = sqrt(fBlockM2[i]) / fGoodEventCount;
      fHardwareBlockSumError = sqrt(fHardwareBlockSumM2) / fGoodEventCount;

      //Stability check
      if ((fErrorFlag & kStabilityCut)==kStabilityCut)//check to see the channel has stability cut activated in the event cut file
	if (GetValueWidth()>fStability){//if the width is greater than the stability required flag the event
	  fErrorFlag|=kBeamStabilityError;
	  fDeviceErrorCode|=kBeamStabilityError;
	}
	  
    }
}


void QwVQWK_Channel::PrintValue() const
{
  QwMessage << std::setprecision(4)
            << std::setw(18) << std::left << GetSubsystemName()      << " "
            << std::setw(18) << std::left << GetModuleType()      << " "
            << std::setw(18) << std::left << GetElementName()      << " "
            << std::setw(12) << std::left << GetHardwareSum()      << "+/- "
            << std::setw(12) << std::left << GetHardwareSumError() << " sig "
	    << std::setw(12) << std::left << GetHardwareSumWidth() << " "
            << std::setw(10) << std::left << GetGoodEventCount()   << " "
            << std::setw(12) << std::left << GetBlockValue(0)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(0) << " "
            << std::setw(12) << std::left << GetBlockValue(1)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(1) << " "
            << std::setw(12) << std::left << GetBlockValue(2)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(2) << " "
            << std::setw(12) << std::left << GetBlockValue(3)      << "+/- "
            << std::setw(12) << std::left << GetBlockErrorValue(3) << " "
            << std::setw(12) << std::left << GetGetEventcutErrorFlag() << " "
            << QwLog::endl;
}

std::ostream& operator<< (std::ostream& stream, const QwVQWK_Channel& channel)
{
  stream << channel.GetHardwareSum();
  return stream;
}

/**
 * Blind this channel as an asymmetry
 * @param blinder Blinder
 */
void QwVQWK_Channel::Blind(const QwBlinder *blinder)
{
  if (!IsNameEmpty()) {
    if (blinder->IsBlinderOkay() && fDeviceErrorCode==0){
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
	blinder->BlindValue(fBlock[i]);
      blinder->BlindValue(fHardwareBlockSum);
    } else {
      blinder->ModifyThisErrorCode(fDeviceErrorCode);
      for (Int_t i = 0; i < fBlocksPerEvent; i++)  fBlock[i] = 0.0;
      fHardwareBlockSum = 0.0;
    }
  }
  return;
}

/**
 * Blind this channel as a difference with specified yield
 * @param blinder Blinder
 * @param yield Corresponding yield
 */
void QwVQWK_Channel::Blind(const QwBlinder *blinder, const QwVQWK_Channel& yield)
{
  if (!IsNameEmpty()) {
    if (blinder->IsBlinderOkay() && fDeviceErrorCode==0){
      for (Int_t i = 0; i < fBlocksPerEvent; i++)
	blinder->BlindValue(fBlock[i], yield.fBlock[i]);
      blinder->BlindValue(fHardwareBlockSum, yield.fHardwareBlockSum);
    } else {
      blinder->ModifyThisErrorCode(fDeviceErrorCode);
      for (Int_t i = 0; i < fBlocksPerEvent; i++)  fBlock[i] = 0.0;
      fHardwareBlockSum = 0.0;
    }
  }
  return;
}

Bool_t QwVQWK_Channel::MatchSequenceNumber(size_t seqnum)
{

  Bool_t status = kTRUE;
  if (!IsNameEmpty()){
    status = (fSequenceNumber==seqnum);
    }
  return status;
}

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
}

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
}

Bool_t QwVQWK_Channel::ApplySingleEventCuts()//This will check the limits and update event_flags and error counters
{
  Bool_t status;

  if (bEVENTCUTMODE>=2){//Global switch to ON/OFF event cuts set at the event cut file

    if (fULimit==0 && fLLimit==0){
      status=kTRUE;
    } else  if (GetHardwareSum()<=fULimit && GetHardwareSum()>=fLLimit){
      if (fDeviceErrorCode==0)
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

    UpdateErrorCounters(fDeviceErrorCode);//update the event cut/HW  error count
    if (bEVENTCUTMODE==3){
      status=kTRUE; //Update the event cut fail flag but pass the event.
    }

    fErrorFlag |=fDeviceErrorCode; //update the device error code to the fErrorFlag.
    //if (GetElementName()=="qwk_bcm1" )//&& fErrorFlag==67109136)
    //  QwMessage<<"QwVQWK_Ch after setting device error code "<<fErrorFlag<<" ev state "<<status <<QwLog::endl;

  }
  else{
    status=kTRUE;
    fErrorFlag=0;
  }


  return status;
}

void QwVQWK_Channel::Copy(const VQwDataElement *source){
  const QwVQWK_Channel* input =
    dynamic_cast<const QwVQWK_Channel*>(source);
  if (input == NULL){
    TString loc="Standard exception from QwVQWK_Channel::Copy = "
      +source->GetElementName()+" "
      +this->GetElementName()+" are not of the same type";
    throw(std::invalid_argument(loc.Data()));
  } else {
    Copy(*input);
  }
}

void QwVQWK_Channel::Copy(const QwVQWK_Channel& input)
{
  VQwHardwareChannel::Copy(input);
  fBlocksPerEvent = input.fBlocksPerEvent;
  fNumberOfSamples_map = input.fNumberOfSamples_map;
  fSaturationABSLimit  = input.fSaturationABSLimit;
  //  Copy the flags used for tree trimming
  bHw_sum            = input.bHw_sum;
  bHw_sum_raw        = input.bHw_sum_raw;
  bBlock             = input.bBlock;
  bBlock_raw         = input.bBlock_raw;
  bNum_samples       = input.bNum_samples;
  bDevice_Error_Code = input.bDevice_Error_Code;
  bSequence_number   = input.bSequence_number;

  this->fNumberOfSamples       = input.fNumberOfSamples;
  this->fHardwareBlockSum      = input.fHardwareBlockSum;
  this->fHardwareBlockSumError = input.fHardwareBlockSumError;
  this->fGoodEventCount=input.fGoodEventCount;
  for(Int_t i=0; i<4; i++ ) {
    this->fBlock[i] = input.fBlock[i];
    this->fBlockError[i] = input.fBlockError[i];
  }
}

void  QwVQWK_Channel::PrintErrorCounterHead(){
  TString message;
  message  = Form("%-20s\t","Device name");
  message += "   Sample";
  message += "    SW_HW";
  message += " Sequence";
  message += "   SameHW";
  message += "   ZeroHW";
  message += "   HW Sat";
  message += " EventCut";
  QwMessage << message << QwLog::endl; 
}

void  QwVQWK_Channel::PrintErrorCounterTail(){
  QwMessage << "---------------------------------------------------"
	    << QwLog::endl;
}

void  QwVQWK_Channel::ReportErrorCounters()
{
  TString message;
  if (fErrorCount_sample || fErrorCount_SW_HW 
      || fErrorCount_Sequence || fErrorCount_SameHW 
      || fErrorCount_ZeroHW || fErrorCount_HWSat || fNumEvtsWithEventCutsRejected){
    message  = Form("%-20s\t",GetElementName().Data());
    message += Form(" %8d", fErrorCount_sample);
    message += Form(" %8d", fErrorCount_SW_HW);
    message += Form(" %8d", fErrorCount_Sequence);
    message += Form(" %8d", fErrorCount_SameHW);
    message += Form(" %8d", fErrorCount_ZeroHW);
    message += Form(" %8d", fErrorCount_HWSat);
    message += Form(" %8d", fNumEvtsWithEventCutsRejected);
    
    if((fDataToSave == kRaw) && (!kFoundPedestal||!kFoundGain)){
      message += " ~Warning~ No Pedestal or Gain entered in map file for this channel";
    }

    QwMessage << message << QwLog::endl;
  }
  return;
}
