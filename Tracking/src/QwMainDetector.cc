/**********************************************************\
* File: QwMainDetector.cc                                   *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwMainDetector.h"

const UInt_t QwMainDetector::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwMainDetector::kMaxNumberOfChannelsPerModule = 32;



QwMainDetector::QwMainDetector(TString region_tmp):VQwSubsystemTracking(region_tmp){
};

QwMainDetector::~QwMainDetector(){
  DeleteHistograms();
  
  fPMTs.clear();
};



Int_t QwMainDetector::LoadChannelMap(TString mapfile){
  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum, channum;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = atol(varvalue.Data());
      if (varname=="roc"){
	RegisterROCNumber(value);
      } else if (varname=="bank"){
	RegisterSubbank(value);
      } else if (varname=="slot"){
        RegisterSlotNumber(value);
      } else if (varname=="module"){
	RegisterModuleType(varvalue);
      }
    } else {
      //  Break this line into tokens to process it.
      channum   = (atol(mapstr.GetNextToken(", \t").c_str()));
      name      = mapstr.GetNextToken(", \t").c_str();

      //  Check to see if we've encountered this channel or name yet
      if (fModulePtrs.at(fCurrentIndex).at(channum).first>=0){
	//  We've seen this channel
      } else if (FindSignalIndex(fCurrentType, name)>=0){
	//  We've seen this signal
      } else {
	//  If not, push a new record into the element array
	LinkChannelToSignal(channum, name);
      }
    }
  }
  //
  return 0;
};


void  QwMainDetector::ClearEventData(){
  SetDataLoaded(kFALSE);
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).SetValue(0);
    }
  }
};

Int_t QwMainDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
};



Int_t QwMainDetector::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    SetDataLoaded(kTRUE);
    for(size_t i=0; i<num_words ; i++){
      //  Decode this word as a V775TDC word.
      DecodeV775Word(buffer[i]);

      if (! IsSlotRegistered(index, GetV775SlotNumber())) continue;

      if (IsV775ValidData()){
	// This is a V775 TDC data word
	try {
	  FillRawWord(index,GetV775SlotNumber(),GetV775ChannelNumber(),
		      GetV775Data());
	}
	catch (std::exception& e) {
	  std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: " 
		    << e.what() << std::endl;
	  Int_t chan = GetV775ChannelNumber();
	  std::cerr << "   Parameters:  index=="<<index
		    << "; GetV775SlotNumber()=="<<GetV775SlotNumber()
		    << "; GetV775ChannelNumber()=="<<chan
		    << "; GetV775Data()=="<<GetV775Data()
		    << std::endl;
	  Int_t modindex = GetModuleIndex(index, GetV775SlotNumber());
	  std::cerr << "   GetModuleIndex()=="<<modindex
		    << "; fModulePtrs.at(modindex).size()=="
		    << fModulePtrs.at(modindex).size()
		    << "; fModulePtrs.at(modindex).at(chan).first {module type}=="
		    << fModulePtrs.at(modindex).at(chan).first
		    << "; fModulePtrs.at(modindex).at(chan).second {signal index}=="
		    << fModulePtrs.at(modindex).at(chan).second
		    << std::endl;
	}
      }
    }
  }

  return 0;
};


void  QwMainDetector::ProcessEvent(){
  if (! HasDataLoaded()) return;
  
};


void  QwMainDetector::ConstructHistograms(TDirectory *folder, TString &prefix){
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructHistograms(folder, prefix);
    }
  }
};

void  QwMainDetector::FillHistograms(){
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).FillHistograms();
    }
  }
};

void  QwMainDetector::ConstructBranchAndVector(TTree *tree, TString prefix, std::vector<Float_t> &values)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructBranchAndVector(tree, prefix, values);
    }
  }
};

void  QwMainDetector::FillTreeVector(std::vector<Float_t> &values)
{
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){  
      fPMTs.at(i).at(j).FillTreeVector(values);
    }
  }
};


void  QwMainDetector::DeleteHistograms(){
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).DeleteHistograms();
    } 
  }
};


QwMainDetector& QwMainDetector::operator=  (const QwMainDetector &value){
  if (fPMTs.size() == value.fPMTs.size()){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){	
	fPMTs.at(i).at(j) = value.fPMTs.at(i).at(j);
      }
    }
  } else {
    std::cerr << "QwMainDetector::operator=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};




void QwMainDetector::ClearAllBankRegistrations(){
  VQwSubsystemTracking::ClearAllBankRegistrations();
  fModuleIndex.clear();
  fModulePtrs.clear();
  fModuleTypes.clear();
  fNumberOfModules = 0;
}

Int_t QwMainDetector::RegisterROCNumber(const UInt_t roc_id){
  VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return fCurrentBankIndex;
};

Int_t QwMainDetector::RegisterSlotNumber(UInt_t slot_id){
  std::pair<Int_t, Int_t> tmppair;
  tmppair.first  = -1;
  tmppair.second = -1;
  if (slot_id<kMaxNumberOfModulesPerROC){
    if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fModuleIndex.size()){
      fModuleTypes.resize(fNumberOfModules+1);
      fModulePtrs.resize(fNumberOfModules+1);
      fModulePtrs.at(fNumberOfModules).resize(kMaxNumberOfChannelsPerModule,
					      tmppair);
      fNumberOfModules = fModulePtrs.size();
      fModuleIndex.at(fCurrentBankIndex).at(slot_id) = fNumberOfModules-1;
      fCurrentSlot     = slot_id;
      fCurrentIndex = fNumberOfModules-1;
    }
  } else {
    std::cerr << "QwMainDetector::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfModulesPerROC << std::endl;
  }
  return fCurrentIndex;
};

const QwMainDetector::EModuleType QwMainDetector::RegisterModuleType(TString moduletype){
  moduletype.ToUpper();

  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792"){
    fCurrentType = V792_ADC;
  } else if (moduletype=="V775"){
    fCurrentType = V775_TDC;
  }
  fModuleTypes.at(fCurrentIndex) = fCurrentType;
  if (fPMTs.size()<=fCurrentType){
    fPMTs.resize(fCurrentType+1);
  }
  return fCurrentType;
};


Int_t QwMainDetector::LinkChannelToSignal(const UInt_t chan, const TString &name){
  size_t index = fCurrentType;
  fPMTs.at(index).push_back(QwPMT_Channel(name));
  fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
  fModulePtrs.at(fCurrentIndex).at(chan).second = 
    fPMTs.at(index).size() -1;
};

void QwMainDetector::FillRawWord(Int_t bank_index, 
				 Int_t slot_num, 
				 Int_t chan, UInt_t data){
  Int_t modindex = GetModuleIndex(bank_index,slot_num);
  if (modindex != -1){
    EModuleType modtype = EModuleType(fModulePtrs.at(modindex).at(chan).first);
    Int_t chanindex     = fModulePtrs.at(modindex).at(chan).second;
    if (modtype == EMPTY || chanindex == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
    } else {
      fPMTs.at(modtype).at(chanindex).SetValue(data);
    }
  };
};


Int_t QwMainDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const {
  Int_t modindex = -1;
  if (bank_index>=0 && bank_index<fModuleIndex.size()){
    if (slot_num>=0 && slot_num<fModuleIndex.at(bank_index).size()){
      modindex = fModuleIndex.at(bank_index).at(slot_num);
    }
  }
  return modindex;
};


Int_t QwMainDetector::FindSignalIndex(const QwMainDetector::EModuleType modtype, const TString &name) const{
  size_t index = modtype;
  Int_t chanindex = -1;
  for (size_t chan=0; chan<fPMTs.at(index).size(); chan++) {
    if (name == fPMTs.at(index).at(chan).GetElementName()) {
      chanindex = chan;
      break;
    }
  }
  return chanindex;
};
