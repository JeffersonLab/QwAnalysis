/**********************************************************\
* File: QwScaler_Channel.cc                              *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#include "QwScaler_Channel.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

#include <QwLog.h>


const Bool_t VQwScaler_Channel::kDEBUG = kFALSE;


/********************************************************/
Int_t VQwScaler_Channel::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

void VQwScaler_Channel::ClearEventData()
{
  fValue_Raw = 0;
  fValue     = 0.0;
  fValueM2   = 0.0;

  fPedestal  = 0.0;
  fCalibrationFactor = 1.0;

  fGoodEventCount = 0;
  fDeviceErrorCode = 0;
};

void VQwScaler_Channel::RandomizeEventData(int helicity)
{

  Double_t mean = 300.0;
  Double_t sigma = 50.0;
  UInt_t Dataword = abs((Int_t)gRandom->Gaus(mean,sigma));

  //At this point, I am not clear about the sigle word structure yet.
  //How many bits will be configured for data, which bits will be configured for user info?
  //std::cout<<"word = "<<std::hex<<word<<std::dec<<std::endl;
  fValue_Raw = Dataword;
};


/*!  Static member function to return the word offset within a data buffer
 *   given the module number index and the channel number index.
 *   @param moduleindex   Scaler index within this buffer; counts from 1
 *   @param channelindex  word index within this scaler; counts from 1
 *   @return   The number of words offset to the beginning of this
 *             scaler word from the beginning of the buffer.
 */
Int_t VQwScaler_Channel::GetBufferOffset(Int_t scalerindex, Int_t wordindex)
{
  Int_t offset = -1;
  Int_t kMaxWords = 32; // usually the scalers have 32 data words starting from 0
  Int_t header = 1;

  if (scalerindex<0 ){
    QwError << "QwScaler_Channel::GetBufferOffset:  Invalid scaler index,"
	    << scalerindex
	    << ".  Must be zero or greater."
	    << QwLog::endl;
  } else if (scalerindex<0 || wordindex>kMaxWords){
    QwError << "QwScaler_Channel::GetBufferOffset:  Invalid word index,"
	    << wordindex
	    << ".  Must be in range [0," << kMaxWords << "]."
	    << QwLog::endl;
  } else {
    offset = (header + kMaxWords)*scalerindex + header +wordindex ;
  }
  return offset;
}


void VQwScaler_Channel::SetEventData(Double_t value){

  this->fValue     = value;
  this->fValue_Raw = (UInt_t)value;
  //std::cout<<"fValue is set to: value = "<<value<<std::endl;

}


template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    buffer.push_back( ((this->fValue_Raw<<data_shift)&data_mask) );
    //std::cout<<"this->fValue="<<this->fValue<<std::endl;
  }
};


template<unsigned int data_mask, unsigned int data_shift>
Int_t QwScaler_Channel<data_mask,data_shift>::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,
							      UInt_t index)
{
  UInt_t words_read = 0;
  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
      words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords) {
    fValue_Raw = ((buffer[0] & data_mask) >> data_shift);
    fValue     = fCalibrationFactor * (Double_t(fValue_Raw) - fPedestal);
    words_read = fNumberOfDataWords;
  } else {
    //QwError << "QwScaler_Channel::ProcessEvBuffer: Not enough words!"<< QwLog::endl;
  }
  return words_read;
};


void VQwScaler_Channel::ProcessEvent()
{
  fValue = fCalibrationFactor * (Double_t(fValue_Raw) - fPedestal);
};


void VQwScaler_Channel::PrintValue() const
{
  QwMessage << std::setprecision(4)
            << std::setw(23) << std::left << GetElementName() << ","
            << std::setw(15) << std::right << GetValue() << " +/- " << fValueError
            << QwLog::endl;
}

void VQwScaler_Channel::PrintInfo() const
{
  QwMessage << "***************************************" << QwLog::endl;
  QwMessage << "QwScaler channel: " << GetElementName()
            << QwLog::endl;
}

void VQwScaler_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    TString basename, fullname;
    basename = prefix + GetElementName();

    fHistograms.resize(1, NULL);
    size_t index=0;
    fHistograms[index]   = gQwHists.Construct1DHist(basename);
    index += 1;
  }
};

void  VQwScaler_Channel::FillHistograms()
{
  size_t index = 0;
  if (IsNameEmpty()) {
    //  This channel is not used, so skip creating the histograms.
  } else {
    if (fHistograms[index] != NULL)
      //std::cout<<GetElementName()<<" is filled with value "<<this->fValue<<std::endl;
      fHistograms[index]->Fill(this->fValue);
    index += 1;
  }
};


void  VQwScaler_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = basename + "/D";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    if (gQwHists.MatchDeviceParamsFromList(basename.Data()))
      tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

void  VQwScaler_Channel::ConstructBranch(TTree *tree, TString &prefix)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();

    tree->Branch(basename, &fValue, basename+"/D");
  }
};

void  VQwScaler_Channel::FillTreeVector(std::vector<Double_t> &values) const
{
  if (IsNameEmpty()) {
    //  This channel is not used, so skip setting up the tree.
  } else if (fTreeArrayNumEntries <= 0) {
    QwError << "VQwScaler_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	    << fTreeArrayNumEntries << QwLog::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries) {
    QwError << "VQwScaler_Channel::FillTreeVector:  values.size()=="
	    << values.size() << " name: " << fElementName
	    << "; fTreeArrayIndex+fTreeArrayNumEntries=="
            << fTreeArrayIndex << '+' << fTreeArrayNumEntries << '='
	    << fTreeArrayIndex+fTreeArrayNumEntries
	    << QwLog::endl;
  } else {
    size_t index = fTreeArrayIndex;
    values[index++] = this->fValue;
  }
};


VQwScaler_Channel& VQwScaler_Channel::operator= (const VQwScaler_Channel &value)
{
  if (!IsNameEmpty()) {
    this->fValue  = value.fValue;
    this->fValueM2 = value.fValueM2;
    this->fDeviceErrorCode = value.fDeviceErrorCode;//error code is updated.
    this->fGoodEventCount = value.fGoodEventCount;
  }
  return *this;
};


VQwScaler_Channel& VQwScaler_Channel::operator+= (const VQwScaler_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue  += value.fValue;
    this->fValueM2 = 0.0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.

  }
  return *this;
};

VQwScaler_Channel& VQwScaler_Channel::operator-= (const VQwScaler_Channel &value)
{
  if (!IsNameEmpty()){
    this->fValue  -= value.fValue;
    this->fValueM2 = 0.0;
    this->fDeviceErrorCode |= (value.fDeviceErrorCode);//error code is ORed.
  }
  return *this;
};

void VQwScaler_Channel::Sum(VQwScaler_Channel &value1, VQwScaler_Channel &value2)
{
  *this =  value1;
  *this += value2;
};

void VQwScaler_Channel::Difference(VQwScaler_Channel &value1, VQwScaler_Channel &value2){
  *this =  value1;
  *this -= value2;
};

void VQwScaler_Channel::Ratio(VQwScaler_Channel &numer, VQwScaler_Channel &denom){
  if (!IsNameEmpty()){

    // Take the ratio of the hardware sum
    if (denom.fValue != 0.0)
      fValue = (numer.fValue) / (denom.fValue);
    else
      fValue = 0.0;

    fValue_Raw = 0;
    

    // The variances are calculated using the following formula:
    //   Var[ratio] = ratio^2 (Var[numer] / numer^2 + Var[denom] / denom^2)
    //
    // This requires that both the numerator and denominator are non-zero!
    //

    if (numer.fValue != 0.0 && denom.fValue != 0.0)
      fValueM2 = fValue * fValue *
         (numer.fValueM2 / numer.fValue / numer.fValue
        + denom.fValueM2 / denom.fValue / denom.fValue);
    else
      fValueM2 = 0.0;

    // Remaining variables
    fGoodEventCount  = denom.fGoodEventCount;
    fDeviceErrorCode = (numer.fDeviceErrorCode|denom.fDeviceErrorCode);//error code is ORed.
  }
};

void VQwScaler_Channel::Offset(Double_t offset)
{
  if (!IsNameEmpty())
    {

    }
};


void VQwScaler_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      this->fValue *= scale;
    }
};



Bool_t VQwScaler_Channel::ApplySingleEventCuts()
{
  return kTRUE;
};

void VQwScaler_Channel::AccumulateRunningSum(const VQwScaler_Channel& value)
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
  Double_t M11 = fValue;
  Double_t M12 = value.fValue;
  Double_t M22 = value.fValueM2;
  if (n2 == 0) {
    // no good events for addition
    return;
  } else if (n2 == 1) {
    // simple version for addition of single event
    fGoodEventCount++;
    fValue += (M12 - M11) / n;
    fValueM2 += (M12 - M11) * (M12 - fValue); // note: using updated mean
  } else if (n2 > 1) {
    // general version for addition of multi-event sets
    fGoodEventCount += n2;
    fValue += n2 * (M12 - M11) / n;
    fValueM2 += M22 + n1 * n2 * (M12 - M11) * (M12 - M11) / n;
  }

  // Nanny
  if (fValue != fValue)
    QwWarning << "Angry Nanny: NaN detected in " << GetElementName() << QwLog::endl;

  return;
};

void VQwScaler_Channel::CalculateRunningAverage(){
  //  See notes in QwVQWK_Channel;  we are using:
  //         error = sqrt(M2)/n,
  //  or alternately we could use the unbiased estimator for both
  //  the sigma and error on the mean:
  //          error = sqrt(M2)/(n-1)
  if(fGoodEventCount > 0) {
    fValueError = sqrt(fValueM2) / fGoodEventCount;
  } else {
    fValueError = 0.0;
  }

}

void VQwScaler_Channel::Copy(VQwDataElement *source)
{
    try
    {
     if(typeid(*source)==typeid(*this))
       {
	 VQwScaler_Channel* input = dynamic_cast<VQwScaler_Channel*>(source);
         this->fElementName       = input->fElementName;
         this->fValue_Raw         = input->fValue_Raw;
         this->fValue             = input->fValue;
         this->fValueM2           = input->fValueM2;
         this->fPedestal          = input->fPedestal;
         this->fCalibrationFactor = input->fCalibrationFactor;
         this->fGoodEventCount    = input->fGoodEventCount;
         this->fDeviceErrorCode   = input->fDeviceErrorCode;
       }
     else
       {
	 TString loc="Standard exception from VQwScaler_Channel::Copy = "
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

void  VQwScaler_Channel::ReportErrorCounters(){
    if (fNumEvtsWithHWErrors>0)
      QwMessage << "QwScaler_Channel " << GetElementName()
		<< " had " << fNumEvtsWithHWErrors
		<< " events with a hardware faliure."
		<< QwLog::endl;

    if (fNumEvtsWithEventCutsRejected>0)
      QwMessage << "QwScaler_Channel " << GetElementName()
		<< " had " << fNumEvtsWithEventCutsRejected
		<< " events rejected by Event Cuts."
		<< QwLog::endl;
  };


//  These explicit class template instantiations should be the
//  last thing in this file.  This list should cover all of the
//  types that are typedef'ed in the header file.
template class QwScaler_Channel<0x00ffffff,0>;  // QwSIS3801D24_Channel
template class QwScaler_Channel<0xffffffff,0>;  // QwSIS3801_Channel, etc.
