/**********************************************************\
* File: QwDriftChamber.C                                   *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwDriftChamber.h"



#include "QwParameterFile.h"

Int_t OK = 0;

const UInt_t QwDriftChamber::kMaxNumberOfTDCsPerROC = 21;
const UInt_t QwDriftChamber::kMaxNumberOfChannelsPerTDC = 64;

const UInt_t QwDriftChamber::kReferenceChannelPlaneNumber = 99;

QwDriftChamber::QwDriftChamber(TString region_tmp):fDEBUG(kFALSE),fNumberOfTDCs(0)
{
region = region_tmp;
};


Int_t QwDriftChamber::LoadChannelMap(TString mapfile){
  TString varname, varvalue;
  UInt_t  chan, plane, wire;
  size_t numchannels = 0;

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
      } else if (varname=="slot"){
	RegisterSlotNumber(value);
      }
    } else {
      //  Break this line into tokens to process it.
      chan  = (atol(mapstr.GetNextToken(", ").c_str()));
      plane = (atol(mapstr.GetNextToken(", ").c_str()));
      wire  = (atol(mapstr.GetNextToken(", ").c_str()));

      LinkChannelToWire(chan, plane, wire);
    }


    //  Get channel #, plane #, and wire #
  }
  //  Construct the wire data structures.
  fWireData.resize(fWiresPerPlane.size());
  fReferenceChannelsPerWire.resize(fWiresPerPlane.size());
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    fWireData.at(i).resize(fWiresPerPlane.at(i));
    fReferenceChannelsPerWire.at(i).resize(fWiresPerPlane.at(i),0);
  }
  for (size_t i=0; i<fTDC_Index.size(); i++){
    Int_t refchan = i;
    for (size_t j=0; j<fTDC_Index.at(i).size(); j++){
      Int_t mytdc = fTDC_Index.at(i).at(j);
      if (mytdc!=-1){
	for (size_t k=0; k<fTDCPtrs.at(mytdc).size(); k++){
	  Int_t plane = fTDCPtrs.at(mytdc).at(k).first;
	  if (plane>0 && plane !=kReferenceChannelPlaneNumber){
	    Int_t wire  = fTDCPtrs.at(mytdc).at(k).second;
	    fReferenceChannelsPerWire.at(plane).at(wire) = refchan;
	  }
	}
      }
    }
  }
  //
  ReportConfiguration();
  return OK;
};

void  QwDriftChamber::ClearEventData()
{
  for (size_t i=0; i<fWireData.size(); i++){
    for (size_t j=0; j<fWireData.at(i).size(); j++){
      fWireData.at(i).at(j).clear();
    }
  }
  for (size_t i=0; i<fReferenceData.size(); i++){
    fReferenceData.at(i).clear();
  }
};


void  QwDriftChamber::ReportConfiguration(){
  for (size_t i = 0; i<fROC_IDs.size(); i++){
    for (size_t j=0; j<fBank_IDs.at(i).size(); j++){
      Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
      std::cout << "ROC " << fROC_IDs.at(i) 
		<< ", subbank " << fBank_IDs.at(i).at(j) 
		<< ":  subbank index==" << ind
		<< std::endl;
      for (size_t k=0; k<kMaxNumberOfTDCsPerROC; k++){
	Int_t tdcindex = GetTDCIndex(ind,k);
	std::cout << "    Slot " << k;
	if (tdcindex == -1) 
	  std::cout << "  Empty" << std::endl;
	else
	  std::cout << "  TDC#" << tdcindex << std::endl;
      }
    }
  }
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    if (fWiresPerPlane.at(i) == 0) continue;
    std::cout << "Plane " << i << " has " << fWireData.at(i).size()
	      << " wires" 
	      <<std::endl;
  }
};

Int_t QwDriftChamber::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
  Int_t index = GetSubbankIndex(roc_id,bank_id);
  
  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
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
		    << "; fTDCPtrs.at(tdcindex).at(chan).first=="
		    << fTDCPtrs.at(tdcindex).at(chan).first
		    << "; fTDCPtrs.at(tdcindex).at(chan).second=="
		    << fTDCPtrs.at(tdcindex).at(chan).second
		    << std::endl;
	}
      }
    }
  }
  return OK;
};


void  QwDriftChamber::ConstructHistograms(TDirectory *folder)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
  //  Now create the histograms...

///////////////First set of histos////////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
 
      TotHits[i] = new TH1F(Form("%sHitsOnEachWirePlane%d",region.Data(),i),Form("Total hits on all wires in plane %d",i),fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5);
      TotHits[i]->GetXaxis()->SetTitle("Wire #");
      TotHits[i]->GetYaxis()->SetTitle("Events");
}
//////////////////////////////////////////////////////////////////

///////////////Second set of histos///////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
   WiresHit[i] = new TH1F(Form("%sWiresHitPlane%d",region.Data(),i),Form("Number of Wires Hit in plane %d",i),20,-0.5,20.5);
   WiresHit[i]->GetXaxis()->SetTitle("Wires Hit per Event");
   WiresHit[i]->GetYaxis()->SetTitle("Events");
}
//////////////////////////////////////////////////////////////////

//////////////Third set of histos/////////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{ 
      HitsWire[i] = new TH2F(Form("%sHitsOnEachWirePerEventPlane%d",region.Data(),i),Form("hits on all wires per event in plane %d",i),fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5,7,0.5,7.5);
    
 HitsWire[i]->GetXaxis()->SetTitle("Wire Number");
 HitsWire[i]->GetYaxis()->SetTitle("Hits");
}
///////////////////////////////////////////////////////////////////////

/////////////Forth set of histos//////////////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{ 
   TOFP[i] = new TH1F(Form("%sTimeofFlightPlane%d",region.Data(),i),Form("Time of flight for events in plane %d",i),400,-65000,65000);
   TOFP[i]->GetXaxis()->SetTitle("Time of Flight");
   TOFP[i]->GetYaxis()->SetTitle("Hits");
}
///////////////////////////////////////////////////////////////////////

//////////////Fifth set of histos/////////////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{ 
   TOFW[i] = new TH2F(Form("%sTimeofFlightperWirePlane%d",region.Data(),i),Form("Time of flight for each wire in plane %d",i),fWiresPerPlane[i]+1,-0.5,fWiresPerPlane[i]+0.5,2000,-40000,65000);
   TOFW[i]->GetXaxis()->SetTitle("Wire Number");
   TOFW[i]->GetYaxis()->SetTitle("Time of Flight");
}
///////////////////////////////////////////////////////////////////////      
};


void  QwDriftChamber::FillHistograms()
{
  //   //  This is a dumb check to see if we enter this routine.
  //   if (fWireData.at(1).at(45).size()>0){
  //     std::cout << "QwDriftChamber::FillHistograms()  "
  // 	      << "fWireData.at(1).at(45).size()=="
  // 	      << fWireData.at(1).at(45).size()
  // 	      << "; fWireData.at(1).at(45).at(0)=="
  // 	      << fWireData.at(1).at(45).at(0)
  // 	      << std::endl;
  //   }

  //  Fill all of the histograms.


/////first set of histos////////////////////////////////
for (size_t p=1; p<fWiresPerPlane.size(); p++)
{
    for (size_t i=0; i<fWireData.at(p).size(); i++)
      {
      if (fWireData.at(p).at(i).size()>0)
      	{
      	TotHits[p]->Fill(i,1);
      	}
      }	
}
////////////////////////////////////////////////////////

/////Second Set of Histos///////////////////////////////
for (size_t p=1; p<fWiresPerPlane.size(); p++)
{ 
Double_t nhit=0;	
    for (size_t i=0; i<fWireData.at(p).size(); i++)
      {
	      if (fWireData.at(p).at(i).size()>0)
	      	{
	      	nhit+=1;
	      	}
      	}      	
WiresHit[p]->Fill(nhit);
      
}
////////////////////////////////////////////////////////

////////////////Third set of histos/////////////////////
for (size_t p=1; p<fWiresPerPlane.size(); p++)
{
    for (size_t i=0; i<fWireData.at(p).size(); i++)
      {
      	HitsWire[p]->Fill(i,fWireData.at(p).at(i).size());
      }
}
////////////////////////////////////////////////////////

//////////////Fourth set of histos//////////////////////
for (size_t p=1; p<fWiresPerPlane.size(); p++)
{
   for (size_t i=0; i<fWireData.at(p).size(); i++)
   {
   if (fWireData.at(p).at(i).size()>0)
      {
      for (size_t j=0; j<fWireData.at(p).at(i).size(); j++)
         {
         TOFP[p]->Fill(fWireData.at(p).at(i).at(j));
         }
      }  
   }   
}
////////////////////////////////////////////////////////

//////////////Fifth set of histos//////////////////////
for (size_t p=1; p<fWiresPerPlane.size(); p++)
{
   for (size_t i=0; i<fWireData.at(p).size(); i++)
   {
      for (size_t j=0; j<fWireData.at(p).at(i).size(); j++)
      {
         TOFW[p]->Fill(i,fWireData.at(p).at(i).at(j));
      }
   }
}      
////////////////////////////////////////////////////////        
};

void  QwDriftChamber::DeleteHistograms()
{
  //  Run the destructors for all of the histogram object pointers.

///////////First set of histos//////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
  if (TotHits[i]!=NULL){
    TotHits[i]->Delete(); TotHits[i] = NULL;
  }
}
////////////////////////////////////////////////////////

/////////Second set of histos///////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
 if (WiresHit[i]!=NULL){
 WiresHit[i]->Delete(); WiresHit[i] = NULL;
 }
}
////////////////////////////////////////////////////////

////////Third set of histos/////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
 if (HitsWire[i]!=NULL){
 HitsWire[i]->Delete(); HitsWire[i] = NULL;
 }
}
////////////////////////////////////////////////////////

////////Fourth set of histos////////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
 if (TOFP[i]!=NULL){
 TOFP[i]->Delete(); TOFP[i] = NULL;
 }
}
////////////////////////////////////////////////////////

//////////Fifth set of histos///////////////////////////
for (size_t i=1;i<fWiresPerPlane.size();i++)
{
 if (TOFW[i]!=NULL){
 TOFW[i]->Delete(); TOFW[i] = NULL;
 }
}
////////////////////////////////////////////////////////
};

void  QwDriftChamber::SubtractReferenceTimes()
{
  Bool_t refs_okay = kTRUE;
  std::vector<Double_t> reftimes;
  reftimes.resize(fReferenceData.size());
  for (size_t i=0; i<fReferenceData.size(); i++){
    if (fReferenceData.at(i).size()==0){
      //  There isn't a reference time!
      refs_okay = kFALSE;
    } else {
      reftimes.at(i) = fReferenceData.at(i).at(0);
    }
  }
  if (refs_okay) {
    for (size_t i=0; i<fReferenceData.size(); i++){
      for (size_t j=0; j<fReferenceData.at(i).size(); j++){
	fReferenceData.at(i).at(j) -= reftimes.at(i);
      }
    }
    for (size_t i=0; i<fWireData.size(); i++){
      for (size_t j=0; j<fWireData.at(i).size(); j++){
	Int_t refindex = fReferenceChannelsPerWire.at(i).at(j);
	for (size_t k=0; k<fWireData.at(i).at(j).size(); k++){
	fWireData.at(i).at(j).at(k) -= reftimes.at(refindex);
	}
      }
    }
  }
};



void  QwDriftChamber::FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data)
{
  Int_t tdcindex = GetTDCIndex(bank_index,slot_num);
  if (tdcindex != -1){
    Int_t plane = fTDCPtrs.at(tdcindex).at(chan).first;
    Int_t wire  = fTDCPtrs.at(tdcindex).at(chan).second;
    if (plane == -1 || wire == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
    } else if (plane == kReferenceChannelPlaneNumber){
      fReferenceData.at(wire).push_back(data);
    } else {
      fWireData.at(plane).at(wire).push_back(data);
    }
  };
};


void QwDriftChamber::ClearAllBankRegistrations(){
  VQwSubsystem::ClearAllBankRegistrations();
  fTDC_Index.clear();
  fTDCPtrs.clear();
  fWireData.clear();
  fNumberOfTDCs = 0;
}

Int_t QwDriftChamber::RegisterROCNumber(const UInt_t roc_id){
  VQwSubsystem::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  if (fReferenceChannels.size()<=fCurrentBankIndex){
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  return fCurrentBankIndex;
};

Int_t QwDriftChamber::RegisterSlotNumber(UInt_t slot_id){
  if (slot_id<kMaxNumberOfTDCsPerROC){
    if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fTDC_Index.size()){
      std::pair<Int_t, Int_t> tmppair;
      tmppair.first = -1;
      tmppair.second = -1;
      fTDCPtrs.resize(fNumberOfTDCs+1);
      fTDCPtrs.at(fNumberOfTDCs).resize(kMaxNumberOfChannelsPerTDC,tmppair);
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

Int_t QwDriftChamber::LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire){
  fReferenceChannels.at(fCurrentBankIndex).first  = fCurrentTDCIndex;
  fReferenceChannels.at(fCurrentBankIndex).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).first  = plane;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).second = fCurrentBankIndex;
  return OK;
};

Int_t QwDriftChamber::LinkChannelToWire(const UInt_t chan, const UInt_t plane, const Int_t wire){
  if (plane == kReferenceChannelPlaneNumber){
    LinkReferenceChannel(chan, plane, wire);
  } else {
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).first  = plane;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).second = wire;
    if (plane>=fWiresPerPlane.size()){
      fWiresPerPlane.resize(plane+1);
    }
    if (wire>=fWiresPerPlane.at(plane)){
      fWiresPerPlane.at(plane) =  wire+1;
    }
  }
  return OK;
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
