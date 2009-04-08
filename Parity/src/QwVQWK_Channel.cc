#include "QwVQWK_Channel.h"
#include "QwHistogramHelper.h"
#include <stdexcept>




const Bool_t QwVQWK_Channel::kDEBUG = kFALSE;


/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is 0.07929 mV per count, and zero counts corresponds 
 *   to zero voltage.
 */
const Double_t QwVQWK_Channel::kVQWK_VoltsPerBit = 76.29e-6;

/********************************************************/
Bool_t QwVQWK_Channel::IsGoodEvent()
{ 
  Bool_t fEventIsGood=kTRUE;			
  fEventIsGood &= (GetRawHardwareSum()==GetRawSoftwareSum());
  // at some point we want to have a check on the number of the samples in one block
  // std::cout<<GetElementName()<<"  hws="<<GetHardwareSum()<<"  soft="<<GetSoftwareSum()<<std::endl;

  return fEventIsGood;
};
/********************************************************/
void QwVQWK_Channel::ClearEventData(){
  for (size_t i=0; i<4; i++){
    fBlock[i] = 0;
    fBlock_raw[i] = 0;
  }
  fHardwareBlockSum_raw = 0;
  fSoftwareBlockSum_raw = 0;
  fHardwareBlockSum = 0;
  fSequenceNumber   = 0;
  fNumberOfSamples  = 0;
};

Int_t QwVQWK_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;
  Long_t localbuf[6];
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
      //  std::cout<<"first word read is="<<Double_t(localbuf[0])<<std::endl;
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

void QwVQWK_Channel::ProcessEvent()
{

  Double_t thispedestal=fPedestal*fNumberOfSamples;

  for(int i=0;i<4;i++)
      fBlock[i]= fCalibrationFactor*
	(fBlock_raw[i]-thispedestal/(fBlocksPerEvent*1.));

  fHardwareBlockSum=fCalibrationFactor*
    (fHardwareBlockSum_raw-thispedestal);
  
  return;
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
	    if (fHistograms[index] != NULL)
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

void  QwVQWK_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {  
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "hw_sum/F";
    values.push_back(0.0);
    list += ":block0/F";
    values.push_back(0.0);
    list += ":block1/F";
    values.push_back(0.0);
    list += ":block2/F";
    values.push_back(0.0);
    list += ":block3/F";
    if(fDataToSave==kRaw)
      {
	values.push_back(0.0);
	list += ":hw_sum_raw/F";
	values.push_back(0.0);
	list += ":block0_raw/F";
	values.push_back(0.0);
	list += ":block1_raw/F";
	values.push_back(0.0);
	list += ":block2_raw/F";
	values.push_back(0.0);
	list += ":block3_raw/F";
	values.push_back(0.0);
	list += ":sequence_number/F";
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

void  QwVQWK_Channel::FillTreeVector(std::vector<Float_t> &values)
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
    if(fDataToSave==kRaw)
      {
	values[index++] = this->GetRawHardwareSum();
	for (size_t i=0; i<4; i++){
	  values[index++] = this->GetRawBlockValue(i);
	}
	values[index++]=this->fSequenceNumber;
      }
  }
  if (kDEBUG && GetElementName()=="Bar1Right"){
    std::cerr << "&(values["<<fTreeArrayIndex<<"])=="<< &(values[fTreeArrayIndex])
	      << std::endl;
    size_t index=fTreeArrayIndex;
    for (size_t i=0; i<fTreeArrayNumEntries; i++){
      std::cerr << "values[" << index << "]==" << values[index++] << " ";
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
      this->fHardwareBlockSum= value.fHardwareBlockSum;
      this->fNumberOfSamples  = value.fNumberOfSamples;
      this->fSequenceNumber   = value.fSequenceNumber;
    }
  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator+= (const QwVQWK_Channel &value){
  if (!IsNameEmpty()){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] += value.fBlock[i];
      this->fBlock_raw[i]=0;
    }
    this->fHardwareBlockSum_raw =0;
    this->fSoftwareBlockSum_raw =0;
    this->fHardwareBlockSum += value.fHardwareBlockSum;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
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
      this->fBlock[i] = (numer.fBlock[i]) / (denom.fBlock[i]);
      this->fBlock_raw[i]=0;
    }
    this->fHardwareBlockSum = (numer.fHardwareBlockSum) / (denom.fHardwareBlockSum);
    this->fSoftwareBlockSum_raw = 0;
    this->fHardwareBlockSum_raw = 0;
    this->fNumberOfSamples = denom.fNumberOfSamples;
    this->fSequenceNumber  = 0;
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
    if (! status)  
      std::cerr << "QwVQWK_Channel::MatchNumberOfSamples:  Channel "
		<< GetElementName()
		<< " had fNumberOfSamples==" << fNumberOfSamples
		<< " and was supposed to have " << numsamp
		<< std::endl;
  }
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


