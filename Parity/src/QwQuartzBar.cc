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


void  QwQuartzBar::ProcessEvent(){
};


void  QwQuartzBar::ConstructHistograms(TDirectory *folder, TString prefix){
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

void  QwQuartzBar::ConstructBranchAndVector(TTree *tree, TString prefix, std::vector<Float_t> &values)
{
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ConstructBranchAndVector(tree, prefix, values);
    }
  }
};

void  QwQuartzBar::FillTreeVector(std::vector<Float_t> &values)
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


void  QwQuartzBar::DeleteHistograms(){
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->DeleteHistograms();
    }
  }  
};


QwQuartzBar& QwQuartzBar::operator=  (const QwQuartzBar &value){
  if (fADC_Data.size() == value.fADC_Data.size()){
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL && value.fADC_Data.at(i) != NULL){
	*(QwVQWK_Module*)fADC_Data.at(i) = *(QwVQWK_Module*)value.fADC_Data.at(i);
      }
    }
  } else {
    std::cerr << "QwQuartzBar::operator=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};

QwQuartzBar& QwQuartzBar::operator+= (const QwQuartzBar &value){
  if (fADC_Data.size() == value.fADC_Data.size()){
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL && value.fADC_Data.at(i) != NULL){
	*(QwVQWK_Module*)fADC_Data.at(i) += *(QwVQWK_Module*)value.fADC_Data.at(i);
      }
    }
  } else {
    std::cerr << "QwQuartzBar::operator+=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};

QwQuartzBar& QwQuartzBar::operator-= (const QwQuartzBar &value){
  if (fADC_Data.size() == value.fADC_Data.size()){
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL && value.fADC_Data.at(i) != NULL){
	*(QwVQWK_Module*)fADC_Data.at(i) -= *(QwVQWK_Module*)value.fADC_Data.at(i);
      }
    }
  } else {
    std::cerr << "QwQuartzBar::operator-=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};

void QwQuartzBar::Sum(QwQuartzBar &value1, QwQuartzBar &value2){
  *this =  value1;
  *this += value2;
};

void QwQuartzBar::Difference(QwQuartzBar &value1, QwQuartzBar &value2){
  *this =  value1;
  *this -= value2;
};

void QwQuartzBar::Ratio(QwQuartzBar &numer, QwQuartzBar &denom){
  if (fADC_Data.size() == numer.fADC_Data.size() 
      && fADC_Data.size() == denom.fADC_Data.size() ){
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL && numer.fADC_Data.at(i) != NULL
	  && denom.fADC_Data.at(i) != NULL){
	((QwVQWK_Module*)fADC_Data.at(i))->Ratio(*(QwVQWK_Module*)numer.fADC_Data.at(i), *(QwVQWK_Module*)denom.fADC_Data.at(i));
      }
    }
  } else {
    std::cerr << "QwQuartzBar::operator-=:  Problems!!!"
	      << std::endl;
  }  
};

