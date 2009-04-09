/**********************************************************\
* File: QwQuartzBar.cc                                   *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwQuartzBar.h"

#include <sstream>

QwQuartzBar::QwQuartzBar(TString region_tmp):VQwSubsystemParity(region_tmp){
};

QwQuartzBar::~QwQuartzBar(){
  DeleteHistograms();
  
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      delete fADC_Data.at(i);
      fADC_Data.at(i) = NULL;
    }
  }
  fADC_Data.clear();
};


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
	if (modnum >= fADC_Data.size())  fADC_Data.resize(modnum+1, new QwVQWK_Module());
	fADC_Data.at(modnum)->SetChannel(channum, name);
      } else {
	std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
		  << std::endl;
      }
    }
  }
  //
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
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ClearEventData();
    }
  }
};

Int_t QwQuartzBar::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t index = GetSubbankIndex(roc_id,bank_id);
  if (index>=0 && num_words>0){
    //  We want to process the configuration data for this ROC.
    UInt_t words_read = 0;
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
 	words_read += fADC_Data.at(i)->ProcessConfigBuffer(&(buffer[words_read]), 
							   num_words-words_read);
      }
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
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
 	words_read += fADC_Data.at(i)->ProcessEvBuffer(&(buffer[words_read]), 
						       num_words-words_read);
      }
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


void  QwQuartzBar::ProcessEvent()
{
  // stub function= QwQuartzBar::ProcessEvent()
  // Buddhini & Julie, Jan 16, 2009
  
  return;
};


void  QwQuartzBar::ConstructHistograms(TDirectory *folder, TString &prefix){
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ConstructHistograms(folder, prefix);
    }
  }
};

void  QwQuartzBar::FillHistograms(){
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->FillHistograms();
    }
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
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ConstructBranchAndVector(tree, prefix, values);
    }
  }
};

void  QwQuartzBar::FillTreeVector(std::vector<Double_t> &values)
{
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->FillTreeVector(values);
    } else {
      std::cerr << "QwQuartzBar::FillTreeVector:  "
		<< "fADC_Data.at(" << i << ") is NULL"
		<< std::endl;
    }
  }
};

//****************************************************************Added *By Buddhini
void  QwQuartzBar::Copy(VQwSubsystem *source)
{
  // stub function= QwQuartzBar::Copy(VQwSubsystem *source)
  // Buddhini & Julie, Jan 16, 2009
  
  return;
}

VQwSubsystem*  QwQuartzBar::Copy()
{
  // stub function= QwQuartzBar::Copy()
  QwQuartzBar* TheCopy=new QwQuartzBar("Quartz bar Copy");
  TheCopy->Copy(this);
  return TheCopy;
}






Bool_t  QwQuartzBar::Compare(VQwSubsystem *source)
{
  // stub function= QwQuartzBar::Compare
  // Buddhini & Julie, Jan 16, 2009
  
  return kTRUE;
}

VQwSubsystem&  QwQuartzBar::operator=  ( VQwSubsystem *value)
{
  
  if(Compare(value))
    {
      QwQuartzBar* input= (QwQuartzBar*)value;
      for(size_t i=0;i<input->fADC_Data.size();i++){
	*(QwVQWK_Module*)fADC_Data.at(i) = *(QwVQWK_Module*)(input->fADC_Data.at(i));
      }
    }
  return *this;
};

VQwSubsystem&  QwQuartzBar::operator+=  ( VQwSubsystem *value)
{
  // Buddhini, January 14,2009. 
  if(Compare(value))
    {
      QwQuartzBar* input= (QwQuartzBar*)value ;
      for(size_t i=0;i<input->fADC_Data.size();i++){
	*(QwVQWK_Module*)fADC_Data.at(i) += *(QwVQWK_Module*)(input->fADC_Data.at(i));
      }
    }
  return *this;
};

VQwSubsystem&  QwQuartzBar::operator-=  ( VQwSubsystem *value)
{
  // Buddhini, January 14,2009. 
  if(Compare(value))
    {      
      QwQuartzBar* input= (QwQuartzBar*)value;
      for(size_t i=0;i<input->fADC_Data.size();i++){
	*(QwVQWK_Module*)fADC_Data.at(i) -= *(QwVQWK_Module*)(input->fADC_Data.at(i));
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
	&& fADC_Data.size() == denom1.fADC_Data.size() ){
      for (size_t i=0; i<fADC_Data.size(); i++){
	if (fADC_Data.at(i) != NULL && numer1.fADC_Data.at(i) != NULL
	    && denom1.fADC_Data.at(i) != NULL){
	  ((QwVQWK_Module*)fADC_Data.at(i))->Ratio(*(QwVQWK_Module*)numer1.fADC_Data.at(i), *(QwVQWK_Module*)denom1.fADC_Data.at(i));
	}
      }
    } else {
      std::cerr << "QwQuartzBar::operator-=:  Problems!!!"
		<< std::endl;
    }  
  }
};

