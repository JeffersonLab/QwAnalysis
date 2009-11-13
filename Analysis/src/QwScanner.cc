
/**********************************************************\
* File: QwScanner.cc                              *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Thu May 21 21:48:20 CDT 2009                             *
\**********************************************************/

#include "QwScanner.h"

extern QwHistogramHelper gQwHists;

const UInt_t QwScanner::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwScanner::kMaxNumberOfChannelsPerModule = 32;

//QwScanner::QwScanner(TString region_tmp):VQwSubsystemTracking(region_tmp){
QwScanner::QwScanner(TString region_tmp)
                    :VQwSubsystem(region_tmp){

    TString name = region_tmp;
    InitializeChannel(name,"raw");

   MainDetCenterX = 330.0; //units: cm
   MainDetCenterY = 0.0;

   HomePositionOffsetX = -20.0;
   HomePositionOffsetY = -100.0;

   Cal_FactorX = 5.333; //units: cm/V, assume linear, 40cm/8V
   Cal_FactorY = 26.667; //units: cm/V, assume linear, 200cm/8V

   fCurrentPotentialX = 2.0; //units: Volts
   fCurrentPotentialY = 2.0;
   fVoltageOffsetX = 2.0; //units: Volts
   fVoltageOffsetY = 2.0;

   fDirectionX = PreDirectionX = 1.0;
   fDirectionY = PreDirectionY = 1.0;

   myTimer = abs(gRandom->Gaus(15,3));
   FrontScaData = 0;
   BackScaData = 0;
   CoincidenceScaData = 0;

   prefix_trig = TString("trig_");
   prefix_sum = TString("sum_");

   eventnumber = 0;
   trigevtnum = 0;
   sumevtnum = 0;
   fFrontSCA = 0.0;
   fBackSCA = 0.0;
   fCoincidenceSCA = 0.0;
   fFrontADC = 0.0;
   fFrontTDC = 0.0;
   fBackADC = 0.0;
   fBackTDC = 0.0;
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

      else if (modtype=="SIS3801"){
        //std::cout<<"modnum="<<modnum<<"    "<<"fSCAs.size="<<fSCAs.size()<<std::endl;
	if (modnum >= (Int_t) fSCAs.size())  fSCAs.resize(modnum+1, new MQwSIS3801_Module());
	fSCAs.at(modnum)->SetChannel(channum, name);

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

  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->ClearEventData();
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
  //fDEBUG = 1;
  if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
                          << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
                          << ", num_words "<<num_words<<", index "<<index<<std::endl;

  SumFlag = 0;
  TrigFlag = 0;

  //  This is a VQWK bank if bank_id=2103
  if (bank_id==2103){

    sumevtnum++;
    SumFlag = 5;

    if (index>=0 && num_words>0){

      //  This is a VQWK bank We want to process this ROC.  Begin looping through the data.
      if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer: Processing Bank "<<bank_id
                            << " fADC_Data.size() = "<<fADC_Data.size()<<std::endl;
      UInt_t words_read = 0;
      for (size_t i=0; i<fADC_Data.size(); i++){
        if (fADC_Data.at(i) != NULL){
   	  words_read += fADC_Data.at(i)->ProcessEvBuffer(&(buffer[words_read]),
						       num_words-words_read);
          if (fDEBUG) {
          std::cout<<"QwScanner::ProcessEvBuffer(VQWK): "<<words_read<<" words_read, "<<num_words<<" num_words"<<" Data: ";
          for(UInt_t j=0; j<words_read; j++)
            std::cout<<"     "<<std::hex<<buffer[j]<<std::dec;
          std::cout<<std::endl;
          }
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


  //  This is a SCA bank if bank_id=2102
  if (bank_id==2102){

    if (index>=0 && num_words>0){

      //  This is a SCA bank We want to process this ROC.  Begin looping through the data.
      if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer: Processing Bank "<<bank_id
                            << " fSCAs.size() = "<<fSCAs.size()<<std::endl;
      UInt_t words_read = 0;
      for (size_t i=0; i<fSCAs.size(); i++){
        if (fSCAs.at(i) != NULL){
   	  words_read += fSCAs.at(i)->ProcessEvBuffer(&(buffer[words_read]),
						       num_words-words_read);
          if (fDEBUG) {
          std::cout<<"QwScanner::ProcessEvBuffer(SCA): "<<words_read<<" words_read, "<<num_words<<" num_words"<<" Data: ";
          for(UInt_t j=0; j<words_read; j++)
            std::cout<<"     "<<std::hex<<buffer[j]<<std::dec;
          std::cout<<std::endl;
          }
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


  //  This is a QADC/TDC bank (bank_id=2101)
  if (bank_id==2101){

    trigevtnum++;
    TrigFlag = 5;

    if (index>=0 && num_words>0){
      //  We want to process this ROC.  Begin looping through the data.
      if (fDEBUG) std::cout << "FocalPlaneScanner::ProcessEvBuffer:  "
                          << "Begin processing ROC" << roc_id <<", Bank "<<bank_id<< std::endl;


          if (fDEBUG) 
          std::cout<<"QwScanner::ProcessEvBuffer (trig) Data: \n";

      for(size_t i=0; i<num_words ; i++){
        //  Decode this word as a V775TDC word.
        DecodeTDCWord(buffer[i]);

 //       if (! IsSlotRegistered(index, GetTDCSlotNumber())) continue;

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
  //std::cout<<"Scanner Events will be processed here."<<std::endl;

  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ProcessEvent();
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ProcessEvent();
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->ProcessEvent();
    }
  }

};


void  QwScanner::ConstructHistograms(TDirectory *folder, TString &prefix){

  prefix = TString("scanner_");

  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructHistograms(folder, prefix);
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->ConstructHistograms(folder, prefix);
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->ConstructHistograms(folder, prefix);
    }
  }

//construct specified scanner histograms 
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_power_supply")));
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_position_x")));
    fHistograms1D.push_back( gQwHists.Construct1DHist(TString("scanner_position_y")));
    fHistograms2D.push_back( gQwHists.Construct2DHist(TString("scanner_rate_map")));

};

void  QwScanner::FillHistograms(){
  //std::cout<<"QwScanner::FillHistograms():"<<std::endl;
  if (! HasDataLoaded()) return;

  //Fill trigger data
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).FillHistograms();
      if(fPMTs.at(i).at(j).GetElementName()==TString("quartz_front_adc"))
        fFrontADC = fPMTs.at(i).at(j).GetValue();
      if(fPMTs.at(i).at(j).GetElementName()==TString("quartz_back_adc"))
        fBackADC = fPMTs.at(i).at(j).GetValue();
      if(fPMTs.at(i).at(j).GetElementName()==TString("quartz_front_tdc"))
        fFrontTDC = fPMTs.at(i).at(j).GetValue();
      if(fPMTs.at(i).at(j).GetElementName()==TString("quartz_back_tdc"))
        fBackTDC = fPMTs.at(i).at(j).GetValue();
    }
  }

  //Fill position data
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->FillHistograms();

        for(size_t j=0; j<fHistograms1D.size();j++){
          if(fHistograms1D.at(j)->GetTitle()==TString("scanner_power_supply")){
            fPowSupply = fADC_Data.at(i)->GetChannel(TString("SCAN_POW"))->GetAverageVolts();
            fHistograms1D.at(j)->Fill(fPowSupply);
          }

          if(fHistograms1D.at(j)->GetTitle()==TString("scanner_position_x")){
            fPositionX = fADC_Data.at(i)->GetChannel(TString("SCAN_POSX"))->GetAverageVolts();
            fPositionX = (fPositionX-fVoltageOffsetX)*Cal_FactorX + MainDetCenterX + HomePositionOffsetX;
            fHistograms1D.at(j)->Fill(fPositionX);
          }

          if(fHistograms1D.at(j)->GetTitle()==TString("scanner_position_y")){
            fPositionY = fADC_Data.at(i)->GetChannel(TString("SCAN_POSY"))->GetAverageVolts();
            fPositionY = (fPositionY-fVoltageOffsetY)*Cal_FactorY + MainDetCenterY + HomePositionOffsetY;
            fHistograms1D.at(j)->Fill(fPositionY);
          }
        //std::cout<<"PositionX: "<<fPositionX<<"  PositionY: "<<fPositionY<<std::endl;
        }
    }
  }

  //Fill scaler data
  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->FillHistograms();
      fRate = fSCAs.at(i)->GetChannel(TString("coincidence_sca"))->GetValue();
      fCoincidenceSCA = fRate;
      fFrontSCA = fSCAs.at(i)->GetChannel(TString("front_sca"))->GetValue();
      fBackSCA = fSCAs.at(i)->GetChannel(TString("back_sca"))->GetValue();
    }
  }

  //Fill rate map
  for(size_t j=0; j<fHistograms2D.size();j++){
    if(fHistograms2D.at(j)->GetTitle()==TString("scanner_rate_map")){
      Int_t checkvalidity = 1;
      Double_t PreValue = get_value( fHistograms2D.at(j), fPositionY, fPositionX, checkvalidity);
      if(checkvalidity!=0){
        if(PreValue>0){
          fRate = (PreValue + fRate)*0.5;  //average value for this bin
        }
        fHistograms2D.at(j)->SetBinContent(fPositionY,fPositionX,fRate);
        Int_t xbin = fHistograms2D.at(j)->GetXaxis()->FindBin( fPositionY ); 
        Int_t ybin = fHistograms2D.at(j)->GetYaxis()->FindBin( fPositionX ); 
        fHistograms2D.at(j)->SetBinContent( fHistograms2D.at(j)->GetBin( xbin, ybin ), fRate);
      }
    }
  }

};

void  QwScanner::ConstructTrees(TFile *rootfile)
{
      rootfile->cd();

      //raw data tree for trigger event
      ScannerTrigTree = new TTree("Scanner_TrigTree","scanner trigevent data tree");
      ScannerTrigVector.reserve(6000);
      ConstructBranchAndVector(ScannerTrigTree, prefix_trig, ScannerTrigVector);

      //raw data tree for sum/scaler event
      ScannerSumTree = new TTree("Scanner_SumTree","scanner sumevent data tree");
      ScannerSumVector.reserve(6000);
      ConstructBranchAndVector(ScannerSumTree, prefix_sum, ScannerSumVector);

      //correlated data tree for both sum and trigger events (with deducated position data)
      ScannerEvtTree = new TTree("ScannerEvt","scanner event data tree");
      ScannerEvtTree->Branch("eventnumber",&eventnumber,"eventnumber/I");
      ScannerEvtTree->Branch("trigevtnum",&trigevtnum,"trigevtnum/I");
      ScannerEvtTree->Branch("sumevtnum",&sumevtnum,"sumevtnum/I");
      ScannerEvtTree->Branch("sumflag",&SumFlag,"sumflag/I");
      ScannerEvtTree->Branch("trigflag",&TrigFlag,"trigflag/I");
      ScannerEvtTree->Branch("pow_supply",&fPowSupply,"pow_supply/D");
      ScannerEvtTree->Branch("position_x",&fPositionX,"position_x/D");
      ScannerEvtTree->Branch("position_y",&fPositionY,"position_y/D");
      ScannerEvtTree->Branch("front_sca",&fFrontSCA,"front_sca/D");
      ScannerEvtTree->Branch("back_sca",&fBackSCA,"back_sca/D");
      ScannerEvtTree->Branch("coincidence_sca",&fCoincidenceSCA,"coincidence_sca/D");
      ScannerEvtTree->Branch("front_adc",&fFrontADC,"front_adc/D");
      ScannerEvtTree->Branch("back_adc",&fBackADC,"back_adc/D");
      ScannerEvtTree->Branch("front_tdc",&fFrontTDC,"front_tdc/D");
      ScannerEvtTree->Branch("back_tdc",&fBackTDC,"back_tdc/D");

};

void  QwScanner::FillTrees()
{
     eventnumber++;

     if(TrigFlag == 5){
        FillTreeVector(ScannerTrigVector, prefix_trig);
        ScannerTrigTree->Fill();
     }

     if(SumFlag == 5){
        FillTreeVector(ScannerSumVector, prefix_sum);
        ScannerSumTree->Fill();
     }

     ScannerEvtTree->Fill();
};

void  QwScanner::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if(prefix == TString("trig_")){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
        fPMTs.at(i).at(j).ConstructBranchAndVector(tree, prefix, values);
      }
    }
  return;
  }

  if(prefix == TString("sum_")){
  // Convert vector<float> to vector<double>
    //std::vector<double> dvalues(values.begin(), values.end());
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
        fADC_Data.at(i)->ConstructBranchAndVector(tree, prefix, values);
      }
    }

    for (size_t i=0; i<fSCAs.size(); i++){
      if (fSCAs.at(i) != NULL){
        fSCAs.at(i)->ConstructBranchAndVector(tree, prefix, values);
      }
    }

  return;
  }

};

void  QwScanner::FillTreeVector(std::vector<Double_t> &values, TString &prefix)
{
  if (! HasDataLoaded()) return;

  //fill trigvalues
  if(prefix == TString("trig_")){
    for (size_t i=0; i<fPMTs.size(); i++){
      for (size_t j=0; j<fPMTs.at(i).size(); j++){
          fPMTs.at(i).at(j).FillTreeVector(values);
      }
    }
  return;
  }

  //fill sumvalues
  if(prefix == TString("sum_")){
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
        fADC_Data.at(i)->FillTreeVector(values);
      } else {
        std::cerr << "QwScanner::FillTreeVector:  "
		<< "fADC_Data.at(" << i << ") is NULL"
		<< std::endl;
      }
    }

    for (size_t i=0; i<fSCAs.size(); i++){
      if (fSCAs.at(i) != NULL){
        fSCAs.at(i)->FillTreeVector(values);
      } else {
        std::cerr << "QwScanner::FillTreeVector:  "
		<< "fADC_Data.at(" << i << ") is NULL"
		<< std::endl;
      }
    }
  }
  return;
};


void  QwScanner::DeleteHistograms(){
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).DeleteHistograms();
    }
  }

  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->DeleteHistograms();
    }
  }

  for (size_t i=0; i<fHistograms1D.size(); i++){
    if (fHistograms1D.at(i) != NULL){
      fHistograms1D.at(i)->Delete();
      fHistograms1D.at(i) =  NULL;
    }
  }

  for (size_t i=0; i<fHistograms2D.size(); i++){
    if (fHistograms2D.at(i) != NULL){
      fHistograms2D.at(i)->Delete();
      fHistograms2D.at(i) =  NULL;
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

      if (fBank_IDs.at(i).at(j)==2101){
        for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++){
	  Int_t QadcTdcindex = GetModuleIndex(ind,k);
	  if (QadcTdcindex != -1){
	    std::cout << "    Slot " << k;
	    std::cout << "  Module#" << QadcTdcindex << std::endl;
            }
          }
        }
        else if (fBank_IDs.at(i).at(j)==2102) {
	  std::cout << "    Number of SIS3801 Scaler:  " << fSCAs.size() << std::endl;
        }
        else if (fBank_IDs.at(i).at(j)==2103) {
	  std::cout << "    Number of TRIUMF ADC:  " << fADC_Data.size() << std::endl;
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

Int_t QwScanner::RegisterSubbank(const UInt_t bank_id)
{
        Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
        fCurrentBankIndex++;
        std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
        fModuleIndex.push_back(tmpvec);
        //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
        return stat;
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
    fModuleTypes.at(fCurrentIndex) = fCurrentType;
    if ((Int_t) fPMTs.size()<=fCurrentType){
      fPMTs.resize(fCurrentType+1);
    }
  } 

  else if (moduletype=="V775"){
    fCurrentType = V775_TDC;
    fModuleTypes.at(fCurrentIndex) = fCurrentType;
    if ((Int_t) fPMTs.size()<=fCurrentType){
      fPMTs.resize(fCurrentType+1);
  }

  }

  else if (moduletype=="SIS3801"){



  }

  return fCurrentType;
};


Int_t QwScanner::LinkChannelToSignal(const UInt_t chan, const TString &name){
  size_t index = fCurrentType;
  if (index == 0 || index == 1) {
    fPMTs.at(index).push_back(QwPMT_Channel(name));
    fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
    fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(index).size() -1;
  }
  else if (index ==2) {
    std::cout<<"scaler module has not been implemented yet."<<std::endl;
  }
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
void QwScanner::SetTriggerEventData(Double_t TrigEventValue)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).SetValue(TrigEventValue);
    }
  }
}

void QwScanner::SetPositionEventData(Double_t* PositionEvBuf, UInt_t sequencenumber)
{
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->SetEventData(PositionEvBuf,sequencenumber);
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
//jpan: triggering scheme - still unclear!
//VQWK module will be gated by beam helicity states 
//QADC module will be in self-triggering mode, i.e., triggered
//by the left-right PMT's coincidence with a low threshold setting.
//TDC's will be started by the left and right PMT's coincidence,
//stopped by what???
//
// The VQWK and scaler will be on the same gate (gate A), the QADC/TDC
// on another gate (gate B, provided by the scanner coincidence). 
// "A OR B" will give the gate(or readout interruption signal) for 
// the master board.
//
void QwScanner::RandomizeEventData(int helicity)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){

   //jpan: It is not strict to use i,j as slot number and channel number,
  // but it is only for generating mock data anyway.
      fPMTs.at(i).at(j).RandomizeEventData(helicity,i,j);
    }
  }

  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->RandomizeEventData(helicity);
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
void QwScanner::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> SumBuffer;
  std::vector<UInt_t> ScaBuffer;
  std::vector<UInt_t> TrigBuffer;

  std::vector<UInt_t> LocalSumBuffer;
  std::vector<UInt_t> LocalScaBuffer;
  std::vector<UInt_t> LocalTrigBuffer;
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;

  myTimer-=1;  //timer counting down

if(myTimer>0){

//mock data for trigger events
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).EncodeEventData(LocalTrigBuffer);

      //assume a trimming level for the adc
      Double_t front_adc_trimmer = 1200.0;
      Double_t back_adc_trimmer = 1100.0;

      Double_t front_adc_value = 0.0;
      Double_t back_adc_value = 0.0;

      if(fPMTs.at(i).at(j).GetElementName()==TString("quartz_front_adc")){
         front_adc_value = UInt_t(fPMTs.at(i).at(j).GetValue())&0xFFF;
         if (front_adc_value>front_adc_trimmer){
             FrontScaData ++;
             //std::cout<<"     front_adc_value, FrontScaData  "<<front_adc_value<<"   "<<FrontScaData<<std::endl;
         }
      }

      if(fPMTs.at(i).at(j).GetElementName()==TString("quartz_back_adc")){
         back_adc_value = UInt_t(fPMTs.at(i).at(j).GetValue())&0xFFF;
         if (back_adc_value>back_adc_trimmer){
             BackScaData ++;
             //std::cout<<"     back_adc_value, BackScaData  "<<back_adc_value<<"   "<<BackScaData<<std::endl;
         }
      }

      if(  (back_adc_value>back_adc_trimmer) && (back_adc_value>back_adc_trimmer) ) {
        CoincidenceScaData ++;
      }
    }
  }
}

else { //(myTimer ==0)

//mock data for scaler events
//replace mock data for ch0, ch1, ch2
  for (size_t i=0; i<fSCAs.size(); i++){
    if (fSCAs.at(i) != NULL){
      fSCAs.at(i)->GetChannel(TString("front_sca"))->SetEventData(FrontScaData);
      fSCAs.at(i)->GetChannel(TString("back_sca"))->SetEventData(BackScaData);
      fSCAs.at(i)->GetChannel(TString("coincidence_sca"))->SetEventData(CoincidenceScaData);
      fSCAs.at(i)->EncodeEventData(LocalScaBuffer);
    }
  }

FrontScaData = 0;
BackScaData = 0;
CoincidenceScaData = 0;

//mock data for position events
  for (size_t i=0; i<fADC_Data.size(); i++){
    if (fADC_Data.at(i) != NULL){
      fADC_Data.at(i)->EncodeEventData(LocalSumBuffer);
    }
  }

//replace mock data for ch1, ch2, ch3
    Double_t kVQWK_VoltsPerBit = 76.29e-6;
    UInt_t fNumberOfSamples = 16680;
    UInt_t fSequenceNumber = 255;
    UInt_t localbuf[6];

//ch1: DC +9.6 +/-0.2 V power supply
    Double_t mean = 9.6;  //units: V
    Double_t sigma = 0.0001;
    Double_t pValue;
    localbuf[4] = 0;
    for (size_t i = 0; i < 4; i++) {
        pValue = gRandom->Gaus(mean,sigma);
        localbuf[i] = (UInt_t) (pValue/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
    }
    localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (size_t i = 0; i < 6; i++){
        LocalSumBuffer[i] =localbuf[i];
    }

//ch2: X motion, DC 2-9.5V, linear motion, back and forth
// speed = 5cm/s, 0.02cm per helicity state (assume 4 ms duration)
// Cal_FactorX = 5.333 cm/V, ==>> step size = 0.02cm / 5.333cm/V = 0.00375V,
// so the voltage in each block will increase ~ 0.00375/4 = 0.0009375V
    mean = 0.00375;  //units: V
    sigma = 0.0000001;
    if (fCurrentPotentialX>9.5) fDirectionX = -1.0;
    if (fCurrentPotentialX<2.0) fDirectionX = 1.0;

    localbuf[4] = 0;
    for (size_t i = 0; i < 4; i++) {
        pValue = 0.25*gRandom->Gaus(mean,sigma);
        fCurrentPotentialX+=pValue*fDirectionX;
        localbuf[i] = (UInt_t) (fCurrentPotentialX/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
    }
    localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (size_t i = 0; i < 6; i++){
        LocalSumBuffer[6+i] =localbuf[i];
    }

//ch3: Y motion, stepping motion with 200 steps, increase from 2 to 9.5V, 
// moving when X-motion changing direction, Cal_FactorY = 26.667 cm/V, 
// each step = (9.5-2)V/200=0.0375V
    mean = 0.0375;  //units: V
    sigma = 0.0000001;
    if (fCurrentPotentialY>9.5) fDirectionY = -1.0;
    if (fCurrentPotentialY<2.0) fDirectionY = 1.0;

    localbuf[4] = 0;
    if (PreDirectionX != fDirectionX){
      PreDirectionX = fDirectionX;
      for (size_t i = 0; i < 4; i++) {
        pValue = 0.25*gRandom->Gaus(mean,sigma);
        fCurrentPotentialY+=pValue*fDirectionY;
        localbuf[i] = (UInt_t) (fCurrentPotentialY/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
      }
    }
    else {
      for (size_t i = 0; i < 4; i++) {
        fCurrentPotentialY=gRandom->Gaus(fCurrentPotentialY,sigma);
        localbuf[i] = (UInt_t) (fCurrentPotentialY/kVQWK_VoltsPerBit * fNumberOfSamples/4.0);
        localbuf[4] += localbuf[i]; // fHardwareBlockSum_raw
      }
    }
    localbuf[5] = (fNumberOfSamples << 16 & 0xFFFF0000)
                | (fSequenceNumber  << 8  & 0x0000FF00);

    for (size_t i = 0; i < 6; i++){
        LocalSumBuffer[12+i] =localbuf[i];
    }

  myTimer = abs(gRandom->Gaus(15,3));
} //end of if(myTimer ==0)

  // If there is element data, generate the subbank header
  if (LocalSumBuffer.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(LocalSumBuffer.size() + 1);    // subbank size
    subbankheader.push_back((2103 << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + LocalSumBuffer.size() + 1);    // bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    SumBuffer.insert(SumBuffer.end(), rocheader.begin(), rocheader.end());
    SumBuffer.insert(SumBuffer.end(), subbankheader.begin(), subbankheader.end());
    SumBuffer.insert(SumBuffer.end(), LocalSumBuffer.begin(), LocalSumBuffer.end());
  }


  if (LocalScaBuffer.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(LocalScaBuffer.size() + 1);    // subbank size
    subbankheader.push_back((2102 << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + LocalScaBuffer.size() + 1);    // bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    ScaBuffer.insert(ScaBuffer.end(), rocheader.begin(), rocheader.end());
    ScaBuffer.insert(ScaBuffer.end(), subbankheader.begin(), subbankheader.end());
    ScaBuffer.insert(ScaBuffer.end(), LocalScaBuffer.begin(), LocalScaBuffer.end());
  }


  if (LocalTrigBuffer.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(LocalTrigBuffer.size() + 1);    //subbank size
//    subbankheader.push_back((2101 << 16) | (0x01 << 8) | (1 & 0xff));
    subbankheader.push_back((2101 << 16) | (0x01 << 8) | (1 & 0xff));
        // subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + LocalTrigBuffer.size() + 1);    // bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
        // bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    TrigBuffer.insert(TrigBuffer.end(), rocheader.begin(), rocheader.end());
    TrigBuffer.insert(TrigBuffer.end(), subbankheader.begin(), subbankheader.end());
    TrigBuffer.insert(TrigBuffer.end(), LocalTrigBuffer.begin(), LocalTrigBuffer.end());
  }

    //Copy elements from TrigBuffer, ScaBuffer and SumBuffer into buffer
    for (UInt_t i=0; i<TrigBuffer.size(); i++)
       buffer.push_back(TrigBuffer.at(i));

    for (UInt_t i=0; i<ScaBuffer.size(); i++)
       buffer.push_back(ScaBuffer.at(i));

    for (UInt_t i=0; i<SumBuffer.size(); i++)
       buffer.push_back(SumBuffer.at(i));

    // Print buffer
    int kDebug = 0;

    if (kDebug) {
      std::cout << std::endl << "SumBuffer: ";
      for (size_t i = 0; i < SumBuffer.size(); i++)
        std::cout << std::hex << SumBuffer.at(i) << " ";
      std::cout << std::dec << std::endl;
    }
    if (kDebug) {
      std::cout << std::endl << "ScaBuffer: ";
      for (size_t i = 0; i < ScaBuffer.size(); i++)
        std::cout << std::hex << ScaBuffer.at(i) << " ";
      std::cout << std::dec << std::endl;
    }
    if (kDebug) {
      std::cout << std::endl << "TrigBuffer: ";
      for (size_t i = 0; i < TrigBuffer.size(); i++)
        std::cout << std::hex << TrigBuffer.at(i) << " ";
      std::cout << std::dec << std::endl;
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

//scanner analysis utilities
Double_t QwScanner::get_value( TH2* h, Double_t x, Double_t y, Int_t& checkvalidity)
{
  if (checkvalidity)
    {
      bool x_ok = ( h->GetXaxis()->GetXmin() < x && x < h->GetXaxis()->GetXmax() );
      bool y_ok = ( h->GetYaxis()->GetXmin() < y && y < h->GetYaxis()->GetXmax() );
     
      if (! ( x_ok && y_ok)) 
	{
	  //if (!x_ok) std::cerr << "x value " << x << " out of range ["<< h->GetXaxis()->GetXmin() <<","<< h->GetXaxis()->GetXmax() << "]" << std::endl;
	  //if (!y_ok) std::cerr << "y value " << y << " out of range ["<< h->GetYaxis()->GetXmin() <<","<< h->GetYaxis()->GetXmax() << "]" << std::endl;
	  checkvalidity=0;
	  return -1e20;
	}
    }

  const int xbin = h->GetXaxis()->FindBin( x ); 
  const int ybin = h->GetYaxis()->FindBin( y ); 

  return h->GetBinContent( h->GetBin( xbin, ybin ));
};

