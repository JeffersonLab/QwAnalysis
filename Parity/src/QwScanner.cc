
/**********************************************************\
* File: QwScanner.h                              *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Thu May 21 21:48:20 CDT 2009                             *
\**********************************************************/

#include "QwScanner.h"

const UInt_t QwScanner::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwScanner::kMaxNumberOfChannelsPerModule = 32;

//QwScanner::QwScanner(TString region_tmp):VQwSubsystemTracking(region_tmp){
QwScanner::QwScanner(TString region_tmp)
                    :VQwSubsystem(region_tmp){

    TString name = region_tmp;
    InitializeChannel(name,"raw");
};


QwScanner::~QwScanner(){
  DeleteHistograms();
  fPMTs.clear();

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      delete fADC_Data.at(i);
      fADC_Data.at(i) = NULL;
    }
  }
  fADC_Data.clear();

};



Int_t QwScanner::LoadChannelMap(TString mapfile){
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
        //std::cout<<"modnum="<<modnum<<"    "<<"fADC_Data.size="<<fADC_Data.size()<<std::endl;
	if (modnum >= (Int_t) fADC_Data.size())  fADC_Data.resize(modnum+1, new QwVQWK_Module());
	fADC_Data.at(modnum)->SetChannel(channum, name);
      }

      else if (modtype=="V792" || modtype=="V775"){
	RegisterModuleType(modtype);
      //  Check to see if we've encountered this channel or name yet
      if (fModulePtrs.at(fCurrentIndex).at(channum).first>=0){
	//  We've seen this channel
      } else if (FindSignalIndex(fCurrentType, name)>=0){
	//  We've seen this signal
      } else {
	//  If not, push a new record into the element array
        if (modtype=="V792") std::cout<<"V792: ";
        if (modtype=="V775") std::cout<<"V775: ";
	LinkChannelToSignal(channum, name);
      }
      }

      else {
	std::cerr << "LoadChannelMap:  Unknown line: " << mapstr.GetLine().c_str()
		  << std::endl;
      }
    }
  }
  //
  ReportConfiguration();
  return 0;
};



Int_t QwScanner::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kTRUE;
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
	  if (ldebug) std::cout<<"inputs for channel "<<varname
			      <<": ped="<<varped<<": cal="<<varcal<<"\n";
	  // Bool_t notfound=kTRUE;
	}
    }
  if (ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
  };


void  QwScanner::ClearEventData(){
  SetDataLoaded(kFALSE);
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).SetValue(0);
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ClearEventData();
    }
  }

};


Int_t QwScanner::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
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


//         jpan@jlab.org
//         We need to process QADC, TDC and TRIUMF ADC data

//         The decoding of the CAEN V792 ADC is nearly identical;
//         the 792 does not use the "DataValidBit", so it is
//         disabled in this version to allow the V775 class to work
//         for both the ADC and TDC.

Int_t QwScanner::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
  Int_t index = GetSubbankIndex(roc_id,bank_id);

  SetDataLoaded(kTRUE);

  //  This is a VQWK bank if bank_id=2222
  if (bank_id==2222){

    if (index>=0 && num_words>0){

      //  This is a VQWK bank We want to process this ROC.  Begin looping through the data.
      if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
                          << "Begin processing ROC" << roc_id << std::endl;
      UInt_t words_read = 0;
      for (size_t i=0; i<fADC_Data.size(); i++){
        if (fADC_Data.at(i) != NULL){
   	  words_read += fADC_Data.at(i)->ProcessEvBuffer(&(buffer[words_read]),
						       num_words-words_read);
        }
      }
      if (num_words != words_read){
        std::cerr << "QwScanner::ProcessEvBuffer:  There were "
		  << num_words-words_read
		  << " leftover words after decoding everything we recognize."
		  << std::endl;
      }
    }
  }

  //  This is a QADC/TDC bank if bank_id=0
  if (bank_id==0){

    if (index>=0 && num_words>0){
      //  We want to process this ROC.  Begin looping through the data.

      for(size_t i=0; i<num_words ; i++){
        //  Decode this word as a V775TDC word.
        DecodeTDCWord(buffer[i]);

        if (! IsSlotRegistered(index, GetTDCSlotNumber())) continue;

        if (IsValidDataword()){
	  // This is a V775 TDC data word
	  try {
	    FillRawWord(index,GetTDCSlotNumber(),GetTDCChannelNumber(),
		        GetTDCData());
	  }
	  catch (std::exception& e) {
	    std::cerr << "Standard exception from FocalPlaneScanner::FillRawTDCWord: "
		      << e.what() << std::endl;
	    Int_t chan = GetTDCChannelNumber();
	    std::cerr << "   Parameters:  index=="<<index
		      << "; GetV775SlotNumber()=="<<GetTDCSlotNumber()
		      << "; GetV775ChannelNumber()=="<<chan
		      << "; GetV775Data()=="<<GetTDCData()
		      << std::endl;
	    Int_t modindex = GetModuleIndex(index, GetTDCSlotNumber());
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
  }

  return 0;
};


void  QwScanner::ProcessEvent(){
  if (! HasDataLoaded()) return;

};


//To-do: need to implement rate map, X-Y position, scaler

void  QwScanner::ConstructHistograms(TDirectory *folder, TString &prefix){

  std::cout<<"Construct histograms for scanner:"<<std::endl;

  std::cout<<"V792/V775 modules:"<<std::endl;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructHistograms(folder, prefix);
    }
  }

  std::cout<<"VQWK module:"<<std::endl;
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ConstructHistograms(folder, prefix);
    }
  }

};

void  QwScanner::FillHistograms(){
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).FillHistograms();
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->FillHistograms();
    }
  }

};


void  QwScanner::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructBranchAndVector(tree, prefix, values);
    }
  }

  // Convert vector<float> to vector<double>
  std::vector<double> dvalues(values.begin(), values.end());
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ConstructBranchAndVector(tree, prefix, dvalues);
    }
  }

};

void  QwScanner::FillTreeVector(std::vector<Double_t> &values)
{
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).FillTreeVector(values);
    }
  }

  // Convert vector<float> to vector<double>
  std::vector<double> dvalues(values.begin(), values.end());
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->FillTreeVector(dvalues);
    } else {
      std::cerr << "QwScanner::FillTreeVector:  "
		<< "fADC_Data.at(" << i << ") is NULL"
		<< std::endl;
    }
  }

};


void  QwScanner::DeleteHistograms(){
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).DeleteHistograms();
    }
  }
};

void  QwScanner::ReportConfiguration(){
  std::cout << "Configuration of the focal plane scanner:"<< std::endl;
  for (size_t i = 0; i<fROC_IDs.size(); i++){
    for (size_t j=0; j<fBank_IDs.at(i).size(); j++){

      Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
      std::cout << "ROC " << fROC_IDs.at(i)
		<< ", subbank " << fBank_IDs.at(i).at(j)
		<< ":  subbank index==" << ind
		<< std::endl;

      if (fBank_IDs.at(i).at(j)!=2222){
        for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++){
	  Int_t QadcTdcindex = GetModuleIndex(ind,k);
	  if (QadcTdcindex != -1){
	    std::cout << "    Slot " << k;
	    std::cout << "  Module#" << QadcTdcindex << std::endl;
            }
          }
        }else {
	  std::cout << "    Number of QWAK:  " << fADC_Data.size() << std::endl;
       }
    }
  }

}; //ReportConfiguration()


Bool_t  QwScanner::Compare(QwScanner &value)
{

  return kTRUE;
}


QwScanner& QwScanner::operator=  (QwScanner &value){
  if (fPMTs.size() == value.fPMTs.size()){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
	fPMTs.at(i).at(j) = value.fPMTs.at(i).at(j);
      }
    }
  }
//  else if (Compare(value)) {
//    QwScanner* input= (QwScanner &)value;
//    for(size_t i=0;i<input->fADC_Data.size();i++){
//      *(QwVQWK_Module*)fADC_Data.at(i) = *(QwVQWK_Module*)(input->fADC_Data.at(i));
//    }
//  }
  else {
    std::cerr << "QwScanner::operator=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};


QwScanner& QwScanner::operator+=  ( QwScanner &value)
{
  if (fPMTs.size() == value.fPMTs.size()){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
	fPMTs.at(i).at(j) += value.fPMTs.at(i).at(j);
      }
    }
  }
//  else if(Compare(value))
//  {
//    QwScanner* input= (QwScanner &)value ;
//    for(size_t i=0;i<input->fADC_Data.size();i++){
//      *(QwVQWK_Module*)fADC_Data.at(i) += *(QwVQWK_Module*)(input->fADC_Data.at(i));
//      }
//   }
  else {
    std::cerr << "QwScanner::operator=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};


QwScanner& QwScanner::operator-=  ( QwScanner &value)
{
  if (fPMTs.size() == value.fPMTs.size()){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
	fPMTs.at(i).at(j) -= value.fPMTs.at(i).at(j);
      }
    }
  }
//  else if(Compare(value))
//  {
//    QwScanner* input= (QwScanner &)value ;
//    for(size_t i=0;i<input->fADC_Data.size();i++){
//      *(QwVQWK_Module*)fADC_Data.at(i) -= *(QwVQWK_Module*)(input->fADC_Data.at(i));
//      }
//   }
  else {
    std::cerr << "QwScanner::operator=:  Problems!!!"
	      << std::endl;
  }
  return *this;
};


void QwScanner::ClearAllBankRegistrations(){
  VQwSubsystem::ClearAllBankRegistrations();
  fModuleIndex.clear();
  fModulePtrs.clear();
  fModuleTypes.clear();
  fNumberOfModules = 0;
}

Int_t QwScanner::RegisterROCNumber(const UInt_t roc_id){
  VQwSubsystem::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return fCurrentBankIndex;
};

Int_t QwScanner::RegisterSlotNumber(UInt_t slot_id){
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
    std::cerr << "QwScanner::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfModulesPerROC << std::endl;
  }
  return fCurrentIndex;
};

const QwScanner::EModuleType QwScanner::RegisterModuleType(TString moduletype){
  moduletype.ToUpper();

  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792"){
    fCurrentType = V792_ADC;
  } else if (moduletype=="V775"){
    fCurrentType = V775_TDC;
  }
  fModuleTypes.at(fCurrentIndex) = fCurrentType;
  if ((Int_t) fPMTs.size()<=fCurrentType){
    fPMTs.resize(fCurrentType+1);
  }
  return fCurrentType;
};


Int_t QwScanner::LinkChannelToSignal(const UInt_t chan, const TString &name){
  size_t index = fCurrentType;
  fPMTs.at(index).push_back(QwPMT_Channel(name));
  fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
  fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(index).size() -1;
  std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
  return 0;
};

void QwScanner::FillRawWord(Int_t bank_index,
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


Int_t QwScanner::GetModuleIndex(size_t bank_index, size_t slot_num) const {
  Int_t modindex = -1;
  if (bank_index>=0 && bank_index<fModuleIndex.size()){
    if (slot_num>=0 && slot_num<fModuleIndex.at(bank_index).size()){
      modindex = fModuleIndex.at(bank_index).at(slot_num);
    }
  }
  return modindex;
};


Int_t QwScanner::FindSignalIndex(const QwScanner::EModuleType modtype, const TString &name) const{
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


/********************************************************/
void QwScanner::SetEventData(Double_t* scannerevent, UInt_t sequencenumber)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).SetValue(scannerevent[4+i]);
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->SetEventData(scannerevent,sequencenumber);
    }
  }

  return;
};

/********************************************************/
void QwScanner::SetPedestal(Double_t pedestal)
{
  fPedestal=pedestal;

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->SetPedestal(fPedestal);
    }
  }

  return;
};

void QwScanner::SetCalibrationFactor(Double_t calib)
{
  fCalibration=calib;
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->SetCalibrationFactor(fCalibration);
    }
  }

  return;
};

/********************************************************/
void  QwScanner::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->InitializeChannel(name,datatosave);
    }
  }

  return;
};
/********************************************************/

void QwScanner::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->SetRandomEventParameters(mean, sigma);
    }
  }

  return;
};
/********************************************************/
void QwScanner::RandomizeEventData(int helicity)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).RandomizeEventData(helicity);
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->RandomizeEventData(helicity);
    }
  }
  return;
};

/********************************************************/
void QwScanner::EncodeEventData(std::vector<UInt_t> &SumBuffer, std::vector<UInt_t> &TrigBuffer)
{
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->EncodeEventData(SumBuffer);
    }
  }

  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).EncodeEventData(TrigBuffer);
    }
  }

};
/********************************************************/

void QwScanner::Print()
{
  //fTriumf_ADC.Print();
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->Print();
    }
  }

  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).Print();
    }
  }

  return;
}
