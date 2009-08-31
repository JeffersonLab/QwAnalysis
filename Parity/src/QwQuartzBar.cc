/**********************************************************\
* File: QwQuartzBar.cc                                   *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwQuartzBar.h"

#include <sstream>

Int_t QwQuartzBar::LoadChannelMap(TString mapfile){
  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum, channum;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing whitespace (spaces or tabs).
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Decode it.
      varname.ToLower();
     UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname=="roc"){
	RegisterROCNumber(value,0);
      } else if (varname=="bank"){
	RegisterSubbank(value);
      }
    } else {
      //  Break this line into tokens to process it.
      modtype   = mapstr.GetNextToken(", ").c_str();
      modnum    = (atol(mapstr.GetNextToken(", ").c_str()));
      channum   = (atol(mapstr.GetNextToken(", ").c_str()));
      dettype   = mapstr.GetNextToken(", ").c_str();
      name      = mapstr.GetNextToken(", ").c_str();

      //  Push a new record into the element array
      if (modtype=="VQWK"){
	if ( modnum >= (Int_t) fADC_Data.size() )
	  fADC_Data.push_back(QwVQWK_Module());
	fADC_Data.at(modnum).SetChannel(channum, name);
      } else {
	std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
		  << std::endl;
      }
    }
  }
  //
  return 0;
};

Int_t QwQuartzBar::LoadEventCuts(TString & filename){
  return 0;
};



Int_t QwQuartzBar::LoadInputParameters(TString pedestalfile)
{
  //   stub function = QwQuartzBar::LoadInputParameters
  //added by Buddhini++++January 16, 2009
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  TString localname;

  Int_t lineread=0;

  QwParameterFile mapstr(pedestalfile.Data());  //Open the file
  while (mapstr.ReadNextLine())
    {
      lineread+=1;
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      else
	{
	  varname = mapstr.GetNextToken(", \t").c_str();	//name of the channel
	  varname.ToLower();
	  varname.Remove(TString::kBoth,' ');
	  varped= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
	  varcal= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
	  if(ldebug) std::cout<<"inputs for channel "<<varname
			      <<": ped="<<varped<<": cal="<<varcal<<"\n";
	  // Bool_t notfound=kTRUE;
 	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
    };
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void  QwQuartzBar::ClearEventData(){
  for (size_t i=0; i<fADC_Data.size(); i++){
    fADC_Data.at(i).ClearEventData();
  }
};

/********************************************************/
void QwQuartzBar::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  // Set parameters on all QwVQWK_Modules
  for (size_t i = 0; i < fADC_Data.size(); i++)
    fADC_Data.at(i).SetRandomEventParameters(mean, sigma);
  return;
};

void QwQuartzBar::SetRandomEventAsymmetry(Double_t asymmetry)
{
  // Set asymmetry on all QwVQWK_Modules
  for (size_t i = 0; i < fADC_Data.size(); i++)
    fADC_Data.at(i).SetRandomEventAsymmetry(asymmetry);
  return;
};

void QwQuartzBar::RandomizeEventData(int helicity)
{
  // Randomize all QwVQWK_Modules
  for (size_t i = 0; i < fADC_Data.size(); i++)
    fADC_Data.at(i).RandomizeEventData(helicity);
}

//*****************************************************************
void QwQuartzBar::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fADC_Data.size(); i++) {
    fADC_Data.at(i).EncodeEventData(elements);
  }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (elements.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(elements.size() + 1);	// subbank size
    subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
		// subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
		// bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
    buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
    buffer.insert(buffer.end(), elements.begin(), elements.end());
  }
}

//*****************************************************************
Int_t QwQuartzBar::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t index = GetSubbankIndex(roc_id,bank_id);
  if (index>=0 && num_words>0){
    //  We want to process the configuration data for this ROC.
    UInt_t words_read = 0;
    for (size_t i = 0; i < fADC_Data.size(); i++) {
      words_read += fADC_Data.at(i).ProcessConfigBuffer(&(buffer[words_read]),
							   num_words-words_read);
    }
  }
  return 0;
};



Int_t QwQuartzBar::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (fDEBUG) std::cout << "QwDriftChamber::ProcessEvBuffer:  "
                          << "Begin processing ROC" << roc_id << std::endl;
    UInt_t words_read = 0;
    for (size_t i = 0; i < fADC_Data.size(); i++) {
      words_read += fADC_Data.at(i).ProcessEvBuffer(&(buffer[words_read]),
						       num_words-words_read);
    }
    if (num_words != words_read){
      std::cerr << "QwQuartzBar::ProcessEvBuffer:  There were "
		<< num_words-words_read
		<< " leftover words after decoding everything we recognize."
		<< std::endl;
    }
  }
  return 0;
};

Bool_t QwQuartzBar::SingleEventCuts(){
  //For the QuartzBar no general routine to check validity
  return true;
};


void  QwQuartzBar::ProcessEvent()
{
  // stub function= QwQuartzBar::ProcessEvent()
  // Buddhini & Julie, Jan 16, 2009

  return;
};


void  QwQuartzBar::ConstructHistograms(TDirectory *folder, TString &prefix){
  for (size_t i = 0; i < fADC_Data.size(); i++) {
    fADC_Data.at(i).ConstructHistograms(folder, prefix);
  }
};

void  QwQuartzBar::FillHistograms(){
  for (size_t i = 0; i < fADC_Data.size(); i++) {
    fADC_Data.at(i).FillHistograms();
  }
};

void  QwQuartzBar::DeleteHistograms()
{
  // stub function= QwQuartzBar::CDeleteHistograms()
  // Buddhini & Julie, Jan 16, 2009

  return;
}


void  QwQuartzBar::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (size_t i = 0; i < fADC_Data.size(); i++) {
    fADC_Data.at(i).ConstructBranchAndVector(tree, prefix, values);
  }
};

void  QwQuartzBar::FillTreeVector(std::vector<Double_t> &values)
{
  for (size_t i = 0; i < fADC_Data.size(); i++) {
    fADC_Data.at(i).FillTreeVector(values);
  }
};

QwVQWK_Channel* QwQuartzBar::GetChannel(const TString name)
{
  if (! fADC_Data.empty()) {
    for (std::vector<QwVQWK_Module>::iterator module = fADC_Data.begin(); module != fADC_Data.end(); ++module) {
      QwVQWK_Channel* channel = module->GetChannel(name);
      if (channel) return channel;
    }
  }
  return 0;
};
//****************************************************************Added *By Buddhini
void  QwQuartzBar::Copy(VQwSubsystem *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      QwQuartzBar* input = ((QwQuartzBar*) source);
      this->fADC_Data.resize(input->fADC_Data.size());
      for (size_t i = 0; i < this->fADC_Data.size(); i++)
        this->fADC_Data[i].Copy(&(input->fADC_Data[i]));
    } else {
      TString loc = "Standard exception from QwQuartzBar::Copy = "
        + source->GetSubsystemName() + " "
        + this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return;
}

VQwSubsystem*  QwQuartzBar::Copy()
{
  // stub function= QwQuartzBar::Copy()
  QwQuartzBar* TheCopy = new QwQuartzBar("Quartz bar Copy");
  TheCopy->Copy(this);
  return TheCopy;
}




Bool_t  QwQuartzBar::Compare(VQwSubsystem *value)
{
  Bool_t result = kTRUE;
  // Check whether the types are identical
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;
  } else {
    // Check whether the internal ADC module sizes are identical
    QwQuartzBar* input = (QwQuartzBar*) value;
    if (input->fADC_Data.size() != fADC_Data.size())
      result = kFALSE;
  }
  return result;
}

VQwSubsystem&  QwQuartzBar::operator= (VQwSubsystem *value)
{

  if(Compare(value))
    {
      QwQuartzBar* input = (QwQuartzBar*) value;
      for (size_t i = 0; i < input->fADC_Data.size(); i++) {
	this->fADC_Data.at(i) = input->fADC_Data.at(i);
      }
    }
  return *this;
};

VQwSubsystem&  QwQuartzBar::operator+=  ( VQwSubsystem *value)
{
  // Buddhini, January 14,2009.
  if(Compare(value))
    {
      QwQuartzBar* input = (QwQuartzBar*) value ;
      for (size_t i = 0; i < input->fADC_Data.size(); i++) {
	fADC_Data.at(i) += input->fADC_Data.at(i);
      }
    }
  return *this;
};

VQwSubsystem&  QwQuartzBar::operator-=  ( VQwSubsystem *value)
{
  // Buddhini, January 14,2009.
  if(Compare(value))
    {
      QwQuartzBar* input = (QwQuartzBar*) value;
      for (size_t i = 0; i < input->fADC_Data.size(); i++) {
	fADC_Data.at(i) -= input->fADC_Data.at(i);
      }
    }
  return *this;
};


void QwQuartzBar::Sum(VQwSubsystem *value1,VQwSubsystem *value2){
  *this =  value1;
  *this += value2;
};

void QwQuartzBar::Difference(VQwSubsystem *value1,VQwSubsystem *value2){
  *this =  value1;
  *this -= value2;
};

void QwQuartzBar::Ratio(VQwSubsystem *numer,VQwSubsystem  *denom){

  if(Compare(numer) && Compare(denom)){
    QwQuartzBar& numer1 = *(QwQuartzBar*)numer;
    QwQuartzBar& denom1 = *(QwQuartzBar*)denom;


    if (fADC_Data.size() == numer1.fADC_Data.size()
     && fADC_Data.size() == denom1.fADC_Data.size() ) {
      for (size_t i = 0; i < fADC_Data.size(); i++) {
        fADC_Data.at(i).Ratio(numer1.fADC_Data.at(i), denom1.fADC_Data.at(i));
      }
    } else {
      std::cerr << "QwQuartzBar::operator-=:  Problems!!!"
		<< std::endl;
    }
  }
};

