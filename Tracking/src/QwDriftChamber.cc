/**********************************************************\
* File: QwDriftChamber.C                                   *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/

#include "QwDriftChamber.h"





const UInt_t QwDriftChamber::kMaxNumberOfTDCsPerROC = 21;
const UInt_t QwDriftChamber::kMaxNumberOfChannelsPerTDC = 64;

const UInt_t QwDriftChamber::kReferenceChannelPlaneNumber = 99;



QwDriftChamber::QwDriftChamber(TString region_tmp,std::vector< QwHit > &fWireHits_TEMP):VQwSubsystemTracking(region_tmp),fDEBUG(kFALSE),fNumberOfTDCs(0),OK(0),fWireHits(fWireHits_TEMP)
{
  
};

QwDriftChamber::QwDriftChamber(TString region_tmp):VQwSubsystemTracking(region_tmp),fDEBUG(kFALSE),fNumberOfTDCs(0),OK(0),fWireHits(fTDCHits)
{
  
};



Int_t QwDriftChamber::LoadChannelMap(TString mapfile){
  TString varname, varvalue;
  UInt_t  chan, package, plane, wire, direction, DIRMODE;
  DIRMODE=0;
  

  fDirectionData.resize(2);//currently we have 1  package - Rakitha (10/23/2008)
  fDirectionData.at(1).resize(13); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)

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
	DIRMODE=0;
      } else if (varname=="slot"){
	RegisterSlotNumber(value);
	DIRMODE=0;
      }else if (varname=="pkg"){
	//this will identify the coming sequence is wire plane to direction mapping - Rakitha
	DIRMODE=1;
	package=value;
      }      
    } else if (DIRMODE==0){
      //  Break this line into tokens to process it.
      chan    = (atol(mapstr.GetNextToken(", ").c_str()));
      package = 1;
      plane   = (atol(mapstr.GetNextToken(", ").c_str()));
      wire    = (atol(mapstr.GetNextToken(", ").c_str()));

      BuildWireDataStructure(chan, package, plane, wire);
    }else if (DIRMODE==1){
      //this will decode the wire plane directions - Rakitha
      plane   = (atol(mapstr.GetNextToken(", ").c_str()));
      direction    = (atol(mapstr.GetNextToken(", ").c_str()));
      fDirectionData.at(package).at(plane)=direction;
    }

  }


  //  Construct the wire data structures.
  
  AddChannelDefinition(plane, wire);
 
  for (size_t i=0; i<fDirectionData.at(1).size(); i++){
    std::cout<<"Direction data Plane "<<i<<" "<<fDirectionData.at(1).at(i)<<std::endl;
    } 
  //
  ReportConfiguration();
  return OK;
};



void  QwDriftChamber::CalculateDriftDistance()
{
  for(std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) {

      hit1->SetDriftDistance(CalculateDriftDistance(hit1->GetTime(),hit1->GetDetectorID()));

  }
  
};

Double_t  QwDriftChamber::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector){
  return 0;
}




void  QwDriftChamber::ClearEventData()
{
  SetDataLoaded(kFALSE);
  QwDetectorID this_det;
  //  Loop through fTDCHits, to decide which detector elements need to be cleared.
  for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
    this_det = hit1->GetDetectorID();
    fWireData.at(this_det.fPlane).at(this_det.fElement).ClearHits();
  }
  fTDCHits.clear();
  for (size_t i=0; i<fReferenceData.size(); i++){
    fReferenceData.at(i).clear();
  }
};










Int_t QwDriftChamber::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
  Int_t index = GetSubbankIndex(roc_id,bank_id);
  
  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.

    SetDataLoaded(kTRUE);
    if (fDEBUG) std::cout << "QwDriftChamber::ProcessEvBuffer:  "
			  << "Begin processing ROC" << roc_id << std::endl;
    for(size_t i=0; i<num_words ; i++){
      //  Decode this word as a F1TDC word.
      DecodeF1Word(buffer[i]);

      if (GetF1SlotNumber() == 31){
	//  This is a custom word which is not defined in 
	//  the F1TDC, so we can use it as a marker for
	//  other data; it may be useful for something.

      }
      if (! IsSlotRegistered(index, GetF1SlotNumber())) continue;

      if (IsAF1Headerword()){
	// This is a F1 TDC header/trailer word
	//  This might be a problem, but often is not...
	//  Do we need to do something?

      } else {
	// This is a F1 TDC data word
	try {
	  FillRawTDCWord(index,GetF1SlotNumber(),GetF1ChannelNumber(),
			 GetF1Data());
	}
	catch (std::exception& e) {
	  std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: " 
		    << e.what() << std::endl;
	  Int_t chan = GetF1ChannelNumber();
	  std::cerr << "   Parameters:  index=="<<index
		    << "; GetF1SlotNumber()=="<<GetF1SlotNumber()
		    << "; GetF1ChannelNumber()=="<<chan
		    << "; GetF1Data()=="<<GetF1Data()
		    << std::endl;
	  Int_t tdcindex = GetTDCIndex(index, GetF1SlotNumber());
	  std::cerr << "   GetTDCIndex()=="<<tdcindex
		    << "; fTDCPtrs.at(tdcindex).size()=="
		    << fTDCPtrs.at(tdcindex).size()
		    << "; fTDCPtrs.at(tdcindex).at(chan).fPlane=="
		    << fTDCPtrs.at(tdcindex).at(chan).fPlane
		    << "; fTDCPtrs.at(tdcindex).at(chan).fElement=="
		    << fTDCPtrs.at(tdcindex).at(chan).fElement
		    << std::endl;
	}
      }
    }
  }
  return OK;
};


void  QwDriftChamber::ConstructHistograms(TDirectory *folder, TString& prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  

  //  Now create the histograms...

  TString region = GetSubsystemName();

  //  Loop over the number of planes.  

  for (size_t i=1;i<fWiresPerPlane.size();i++) {
    ///////////////First set of histos////////////////////////////////
    TotHits[i] = new TH1F(Form("%s%sHitsOnEachWirePlane%d", prefix.Data() ,region.Data(),i),
			  Form("Total hits on all wires in plane %d",i),
			  fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5);
    TotHits[i]->GetXaxis()->SetTitle("Wire #");
    TotHits[i]->GetYaxis()->SetTitle("Events");
    
    
    ///////////////Second set of histos////////////////////////////////
    WiresHit[i] = new TH1F(Form("%s%sWiresHitPlane%d", prefix.Data() ,region.Data(),i),
			   Form("Number of Wires Hit in plane %d",i),
			   20,-0.5,20.5);
    WiresHit[i]->GetXaxis()->SetTitle("Wires Hit per Event");
    WiresHit[i]->GetYaxis()->SetTitle("Events");

    //////////////Third set of histos/////////////////////////////////
    HitsWire[i] = new TH2F(Form("%s%sHitsOnEachWirePerEventPlane%d", prefix.Data() ,region.Data(),i),
			   Form("hits on all wires per event in plane %d",i),
			   fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5,
			   7,0.5,7.5);
    
    HitsWire[i]->GetXaxis()->SetTitle("Wire Number");
    HitsWire[i]->GetYaxis()->SetTitle("Hits");

    /////////////Fourth set of histos//////////////////////////////////////
    TOFP[i] = new TH1F(Form("%s%sTimeofFlightPlane%d", prefix.Data() ,region.Data(),i),
		       Form("Subtracted time of flight for events in plane %d",i),
		       400,-65000,65000);
    TOFP[i]->GetXaxis()->SetTitle("Time of Flight");
    TOFP[i]->GetYaxis()->SetTitle("Hits");
    TOFP_raw[i] = new TH1F(Form("%s%sRawTimeofFlightPlane%d", prefix.Data() ,region.Data(),i),
		       Form("Raw time of flight for events in plane %d",i),
		       400,-65000,65000);
    TOFP_raw[i]->GetXaxis()->SetTitle("Time of Flight");
    TOFP_raw[i]->GetYaxis()->SetTitle("Hits");
    
    //////////////Fifth set of histos/////////////////////////////////////
    TOFW[i] = new TH2F(Form("%s%sTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),i),
		       Form("Subtracted time of flight for each wire in plane %d",i),
		       fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5,
		       100,-40000,65000);
    TOFW[i]->GetXaxis()->SetTitle("Wire Number");
    TOFW[i]->GetYaxis()->SetTitle("Time of Flight");
    TOFW_raw[i] = new TH2F(Form("%s%sRawTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),i),
		       Form("Raw time of flight for each wire in plane %d",i),
		       fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5,
		       100,-40000,65000);
    TOFW_raw[i]->GetXaxis()->SetTitle("Wire Number");
    TOFW_raw[i]->GetYaxis()->SetTitle("Time of Flight");
  }
};


void  QwDriftChamber::FillHistograms()
{
  

  if (! HasDataLoaded()) return;

  
  
  QwDetectorID   this_detid;
  QwDetectorInfo *this_det;
  
  //  Fill all of the histograms.

  std::vector<Int_t> wireshitperplane(fWiresPerPlane.size(),0);

  for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
    this_detid = hit1->GetDetectorID();
    if (this_detid.fPlane<=0 || this_detid.fElement<=0){
      std::cout << "QwDriftChamber::FillHistograms:  Bad plane or element index:  fPlane=="
		<< this_detid.fPlane << ", fElement==" << this_detid.fElement << std::endl;
      continue;
    }
    this_det   = &(fWireData.at(this_detid.fPlane).at(this_detid.fElement));

    if (hit1->IsFirstDetectorHit()){
      //  If this is the first hit for this detector, then let's plot the
      //  total number of hits this wire had.
      HitsWire[this_detid.fPlane]->Fill(this_detid.fElement,this_det->GetNumHits());
      //  Also increment the total number of events in whichthis wire was hit.
      TotHits[this_detid.fPlane]->Fill(this_detid.fElement,1);
      //  Increment the number of wires hit in this plane
      wireshitperplane.at(this_detid.fPlane) += 1;
    }

    //  Fill ToF histograms
    TOFP_raw[this_detid.fPlane]->Fill(hit1->GetRawTime());
    TOFW_raw[this_detid.fPlane]->Fill(this_detid.fElement,hit1->GetRawTime());
    TOFP[this_detid.fPlane]->Fill(hit1->GetTime());
    TOFW[this_detid.fPlane]->Fill(this_detid.fElement,hit1->GetTime());
    
    

  }

  for (size_t iplane=1; iplane<fWiresPerPlane.size(); iplane++) {
    WiresHit[iplane]->Fill(wireshitperplane[iplane]);
    
  }

};

void  QwDriftChamber::DeleteHistograms()
{
  //  Run the destructors for all of the histogram object pointers.
  for (size_t i=1;i<fWiresPerPlane.size();i++) {
    ///////////First set of histos//////////////////////////
    if (TotHits[i]!=NULL){
      TotHits[i]->Delete(); TotHits[i] = NULL;
    }
    /////////Second set of histos///////////////////////////
    if (WiresHit[i]!=NULL){
      WiresHit[i]->Delete(); WiresHit[i] = NULL;
    }
    ////////Third set of histos/////////////////////////////
    if (HitsWire[i]!=NULL){
      HitsWire[i]->Delete(); HitsWire[i] = NULL;
    }
    ////////Fourth set of histos////////////////////////////
    if (TOFP[i]!=NULL){
      TOFP[i]->Delete(); TOFP[i] = NULL;
    }
    if (TOFP_raw[i]!=NULL){
      TOFP_raw[i]->Delete(); TOFP_raw[i] = NULL;
    }
    //////////Fifth set of histos///////////////////////////
    if (TOFW[i]!=NULL){
      TOFW[i]->Delete(); TOFW[i] = NULL;
    }
    if (TOFW_raw[i]!=NULL){
      TOFW_raw[i]->Delete(); TOFW_raw[i] = NULL;
    }
  }
};


void QwDriftChamber::ClearAllBankRegistrations(){
  VQwSubsystemTracking::ClearAllBankRegistrations();
  fTDC_Index.clear();
  fTDCPtrs.clear();
  fWireData.clear();
  fNumberOfTDCs = 0;
}

Int_t QwDriftChamber::RegisterROCNumber(const UInt_t roc_id){
  VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  if (fReferenceChannels.size()<=fCurrentBankIndex){
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  //std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return fCurrentBankIndex;
};

Int_t QwDriftChamber::RegisterSlotNumber(UInt_t slot_id){
  if (slot_id<kMaxNumberOfTDCsPerROC){
    if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fTDC_Index.size()){
      fTDCPtrs.resize(fNumberOfTDCs+1);
      fTDCPtrs.at(fNumberOfTDCs).resize(kMaxNumberOfChannelsPerTDC);
      fNumberOfTDCs = fTDCPtrs.size();
      fTDC_Index.at(fCurrentBankIndex).at(slot_id) = fNumberOfTDCs-1;
      fCurrentSlot     = slot_id;
      fCurrentTDCIndex = fNumberOfTDCs-1;
    }
  } else {
    std::cerr << "QwDriftChamber::RegisterSlotNumber:  Slot number "
	      << slot_id << " is larger than the number of slots per ROC, "
	      << kMaxNumberOfTDCsPerROC << std::endl;
  }
  return fCurrentTDCIndex;
};

Int_t QwDriftChamber::GetTDCIndex(size_t bank_index, size_t slot_num) const {
  Int_t tdcindex = -1;
  if (bank_index>=0 && bank_index<fTDC_Index.size()){
    if (slot_num>=0 && slot_num<fTDC_Index.at(bank_index).size()){
      tdcindex = fTDC_Index.at(bank_index).at(slot_num);
    }
  }
  return tdcindex;
};
