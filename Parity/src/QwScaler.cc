//
// C++ Implementation: QwScaler
//
// Description: used to collect and process the information from the scaler channel
//
//
// Author: siyuan yang <sxyang@email.wm.edu>, (C) Oct 13th 2010
//


#include "QwScaler.h"

QwSubsystemFactory<QwScaler> theScalerFactory("QwScaler");

Int_t QwScaler::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
	return 0;
};


Int_t QwScaler::LoadChannelMap(TString mapfile)
{
  bool local_debug = false;

  TString varname, varvalue;
  TString modtype, dettype, name, keyword;
  Int_t modnum, channum;
  Int_t currentrocread = 0;
  Int_t currentbankread = 0;
  Int_t wordsofar = 0;
  Int_t currentsubbankindex = -1;
  
  QwParameterFile mapstr(mapfile.Data());  // Open the file
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing whitespace (spaces or tabs).
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      // This is a declaration line.  Decode it.
      
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
        currentrocread=value;
        RegisterROCNumber(value,0);
      } else if (varname == "bank") {
        currentbankread=value;
        RegisterSubbank(value);
        if(currentsubbankindex != GetSubbankIndex(currentrocread,currentbankread)){
          currentsubbankindex = GetSubbankIndex(currentrocread,currentbankread);
        }
      }
    } else {
      Bool_t lineok = kTRUE;

      //  Break this line into tokens to process it.
      modtype   = mapstr.GetNextToken(", \t").c_str();
      modnum    = atol(mapstr.GetNextToken(", \t").c_str());
      channum   = atol(mapstr.GetNextToken(", \t").c_str());
      dettype   = mapstr.GetNextToken(", \t").c_str();
      name      = mapstr.GetNextToken(", \t").c_str();
      dettype.ToLower();
      name.ToLower();

//       QwMollerChannelID newChannelID;
//       newChannelID.fModuleType    = modtype;
//       newChannelID.fModuleNumber  = modnum;
//       newChannelID.fChannelName   = name;
//       newChannelID.fDetectorType  = dettype;
//       newChannelID.fChannelNumber = channum;
//       newChannelID.fWordInSubbank = wordsofar;
       
      if (modtype == "SIS3801"){
        wordsofar += 1;
      }else if(modtype == "SIS7200"){
        wordsofar += 1;
      }else if(modtype == "VQWK"){
        wordsofar += 6;
      }else {
        std::cerr << "Unknown module type: " << modtype << ". Skipping channel "  << name << '.' << std::endl;
        lineok = kFALSE;
      }

      if (name.Length() == 0){
        lineok = kFALSE;
      }

//    add new modules until current number (modnum) is reached 
      std::size_t mod_size;
      mod_size = fSCAs.size();
      if(local_debug) std::cout << "modnum: " << modnum << std::endl;
      while ((Int_t) mod_size <= modnum) {
         std::vector<QwSIS3801D24_Channel> new_module;
         fSCAs.push_back(new_module);
	mod_size = fSCAs.size();
	if(local_debug) std::cout << "mod_size: " << mod_size << std::endl;
      }
      std::size_t channel_size;
      channel_size= fSCAs.at(modnum).size();
      if(local_debug) std::cout << "arrive here!" << std::endl;
      while ((Int_t) channel_size <= channum) {
         QwSIS3801D24_Channel new_module(name);
         fSCAs.at(modnum).push_back(new_module);
	channel_size = fSCAs.at(modnum).size();
      }
    }
}
      //change this line if names are supposed to be exclusive, as of now, 
//       newChannelID.fIndex = -1; // GetChannelIndex(name, modnum);

//       if (newChannelID.fIndex == -1 && lineok){
//         QwSTR7200_Channel localSTR7200_Channel(newChannelID.fChannelName);
//         fSTR7200_Channel[modnum].push_back(localSTR7200_Channel);
// //        fSTR7200_Channel[modnum][fMollerChannelID.size() - 1].SetDefaultSampleSize(fSample_size);
//         newChannelID.fIndex = fSTR7200_Channel[modnum].size() - 1;
//         //std::cout << name << ':' << newChannelID.fIndex << ':' << wordsofar << '\n';
//       }

      //  Push a new record into the element array
//       if(lineok){
//         fMollerChannelID.push_back(newChannelID);
//       }

 // end looping over parameter file

//   fPrevious_STR7200_Channel.resize(fSTR7200_Channel.size());
//   for(size_t i = 0; i < fSTR7200_Channel.size(); i++){
//     fPrevious_STR7200_Channel[i].resize(fSTR7200_Channel[i].size());
//     for(size_t j = 0; j < fSTR7200_Channel[i].size(); j++){
//       fPrevious_STR7200_Channel[i][j].Copy(&(fSTR7200_Channel[i][j]));
//     }
//   }

//  for (size_t i = 0; i < fMollerChannelID.size(); i++){
//     std::cout << i << ": " << fMollerChannelID[i].fChannelName << ' ' << fMollerChannelID[i].fChannelNumber << ' ' << fMollerChannelID[i].fIndex << std::endl;
//   }

  return 0;
};

void QwScaler::ProcessOptions(QwOptions &){};
Int_t QwScaler::LoadInputParameters(TString){ return 0;};
void QwScaler::ClearEventData(){};

void  QwScaler::Copy(VQwSubsystem *source)
{
 return;
}


VQwSubsystem*  QwScaler::Copy()
{
 return 0;
}

Int_t QwScaler::ProcessConfigurationBuffer(UInt_t, UInt_t, UInt_t*, UInt_t){
  return 0;
}

Int_t QwScaler::ProcessConfigurationBuffer(UInt_t ev_type, UInt_t, UInt_t, UInt_t*, UInt_t)
{
 return 0;
}

Int_t QwScaler::ProcessEvBuffer(UInt_t ev_type, UInt_t roc_id, UInt_t bank_id, UInt_t *buffer, UInt_t num_words)
{
  return ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
}


void QwScaler::ProcessEvent(){
  return;
};


void QwScaler::ConstructHistograms(TDirectory* folder, TString& prefix){
//   for(size_t i = 0; i < fSCAs.size(); i++){
//     for(size_t j = 0; j < fSCAs.at(i).size(); j++){
//       fSCAs.at(i).at(j).ConstructHistograms(folder, prefix);
//     }
//   }
};

void QwScaler::FillHistograms(){
//   for(size_t i = 0; i < fSCAs.size(); i++){
//     for(size_t j = 0; j < fSCAs.at(i).size(); j++){
//       fSCAs.at(i).at(j).FillHistograms();
//     }
//   }
};

void QwScaler::DeleteHistograms(){
  for(size_t i = 0; i < fSCAs.size(); i++){
    for(size_t j = 0; j < fSCAs.at(i).size(); j++){
      fSCAs.at(i).at(j).DeleteHistograms();
    }
  }
};

void QwScaler::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values){
  fTreeArrayIndex = values.size();

  TString basename;
  if (prefix=="") basename = "scaler";
  else basename = prefix;

  TString list = "";
  for(size_t i = 0; i < fSCAs.size(); i++){
    for(size_t j = 0; j < fSCAs.at(i).size(); j++){
//       fSCAs.at(i).at(j).ConstructBranchAndVector(tree, prefix, values);
	if(fSCAs.at(i).at(j).GetElementName()==""){
		}
	else{
		values.push_back(0.0);
		list+=":"+fSCAs.at(i).at(j).GetElementName()+"/D";
	}
    }
  }
	if(list[0]==':'){
	    list = list(1,list.Length()-1);
  }
	tree->Branch(basename,&values[fTreeArrayIndex],list);
};

void QwScaler::FillTreeVector(std::vector<Double_t> &values) const {
  Int_t index = fTreeArrayIndex;
  for(size_t i = 0; i < fSCAs.size(); i++){
    for(size_t j = 0; j < fSCAs.at(i).size(); j++){
//       fSCAs.at(i).at(j).FillTreeVector(values);
	if(fSCAs.at(i).at(j).GetElementName()==""){
	}
	else{
	values[index]=fSCAs.at(i).at(j).GetValue();
	index++;
	}
     }
  }
};

void QwScaler::ConstructBranch(TTree *tree, TString& prefix){

};
  		
void QwScaler::ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file){


};



VQwSubsystem&  QwScaler::operator=(VQwSubsystem *value){
  // std::cout << "QwScaler assignment (operator=)" << std::endl;
  if(Compare(value)){
    //VQwSubsystem::operator=(value);
    QwScaler* input = dynamic_cast<QwScaler *> (value);
    for(size_t i = 0; i < input->fSCAs.size(); i++){
      for(size_t j = 0; j < fSCAs.at(i).size(); j++){
        this->fSCAs.at(i).at(j) = input->fSCAs.at(i).at(j);
      }
    }
  }
  return *this; 
};

VQwSubsystem&  QwScaler::operator+=(VQwSubsystem *value){
  //std::cout << "QwScaler addition assignment (operator+=)" << std::endl;
  if(Compare(value)){
    QwScaler* input = dynamic_cast<QwScaler *> (value);
    for(size_t i = 0; i < input->fSCAs.size(); i++){
      for(size_t j = 0; j < input->fSCAs.at(i).size(); j++){
        this->fSCAs.at(i).at(j) += input->fSCAs.at(i).at(j);
        //std::cout << "+= " << this->fSTR7200_Channel[i][j].GetValue() << std::endl;
      }
    }
  }
  return *this;
};

VQwSubsystem&  QwScaler::operator-=(VQwSubsystem *value){
  //std::cout << "QwScaler subtraction assignment (operator-=)" << std::endl;
  if(Compare(value)){
    QwScaler* input = dynamic_cast<QwScaler *> (value);
    for(size_t i = 0; i < input->fSCAs.size(); i++){
      for(size_t j = 0; j < input->fSCAs.at(i).size(); j++){
        this->fSCAs.at(i).at(j) -= input->fSCAs.at(i).at(j);
      }
    }
  }
  return *this;
};

void QwScaler::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2){
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
};

void QwScaler::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2){
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
};

void QwScaler::Ratio(VQwSubsystem  *value1, VQwSubsystem  *value2){
  return;
};

void QwScaler::Scale(Double_t factor){
 return;
};

void QwScaler::AccumulateRunningSum(VQwSubsystem* value){
  return;
};

void QwScaler::CalculateRunningAverage(){
 return;
};

Int_t QwScaler::LoadEventCuts(TString filename){return 0;};

Bool_t QwScaler::ApplySingleEventCuts(){
  return true;
};

Int_t QwScaler::GetEventcutErrorCounters(){
  return 0;
};

Int_t QwScaler::GetEventcutErrorFlag(){
  return 0;
};

float* QwScaler::GetRawChannelArray(){
  size_t len = 0;
  float *result = new float[len];
 
  return result;
};

Int_t QwScaler::GetChannelIndex(TString channelName, UInt_t module_number)
{
	return 0; 
}


Bool_t QwScaler::Compare(VQwSubsystem *source){
  	return true;
};

void QwScaler::print(){
  std::cout << " " << fSCAs.size() << std::endl;
  UInt_t max = 0;
  for(size_t i = 0; i < fSCAs.size(); i++){
    UInt_t next = fSCAs.at(i).size();
    if (next > max){
      max = next;
    }
  }

//   for(size_t i = 0; i < max; i++){
//     std::cout << fMollerChannelID[i].fChannelName << ":\t" << (fMollerChannelID[i].fChannelName.Length() < 14 ? "\t" : "");
//     for(size_t j = 0; j < fSTR7200_Channel.size(); j++){
// 
//       if ( i < fSTR7200_Channel[j].size()){
//         std::cout << "0x" << std::hex << (int)fSTR7200_Channel[j][i].GetValue() << std::dec;
//       } else {
//         std::cout << "            ";
//       }
//       std::cout << "\t\t";
//     }
//     std::cout << std::endl;
//   }

}

void QwScaler::PrintValue() const {
  for(size_t i = 0; i < fSCAs.size(); i++){
    for(size_t j = 0; j < fSCAs.at(i).size(); j++){
         fSCAs.at(i).at(j).PrintValue();
    }
  }
}
