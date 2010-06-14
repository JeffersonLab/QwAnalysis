/**********************************************************\
* File: QwDriftChamber.C                                   *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
*         J. H. Lee                                        *
* Time-stamp: <2010-04-12 12:12>                           *
\**********************************************************/

#include "QwDriftChamber.h"

#include "QwLog.h"
#include "QwColor.h"
#include "QwParameterFile.h"

const UInt_t QwDriftChamber::kMaxNumberOfTDCsPerROC = 21;
const UInt_t QwDriftChamber::kMaxNumberOfChannelsPerTDC = 64;
const UInt_t QwDriftChamber::kReferenceChannelPlaneNumber = 99;


// OK, fDEBUG, fNumberOfTDCs
QwDriftChamber::QwDriftChamber(TString region_tmp,std::vector< QwHit > &fWireHits_TEMP)
        :VQwSubsystem(region_tmp),
        VQwSubsystemTracking(region_tmp),
        fWireHits(fWireHits_TEMP)
{
    OK            = 0;
    fDEBUG        = kFALSE;
    fNumberOfTDCs = 0;

    /*for (int i1 = 0; i1 < kNumPackages; i1++)
      for (int i2 = 0; i2 < 2; i2++)
        for (int i3 = 0; i3 < 279; i3++)
          fTimeWireOffsets[i1][i2][i3] = 0;*/

};

QwDriftChamber::QwDriftChamber(TString region_tmp)
  :VQwSubsystemTracking(region_tmp),fWireHits(fTDCHits)
{
    OK            = 0;
    fDEBUG        = kFALSE;
    fNumberOfTDCs = 0;
    /*for (int i1 = 0; i1 < kNumPackages; i1++)
      for (int i2 = 0; i2 < 2; i2++)
        for (int i3 = 0; i3 < 279; i3++)
          fTimeWireOffsets[i1][i2][i3] = 0;*/

};



Int_t QwDriftChamber::LoadChannelMap(TString mapfile)
{
    TString varname, varvalue;
    UInt_t  chan, package, plane, wire, direction, DIRMODE;
    wire = plane = package = 0;
    DIRMODE=0;


    fDirectionData.resize(2);//currently we have 2  package - Rakitha (10/23/2008)
    fDirectionData.at(0).resize(12); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)
    fDirectionData.at(1).resize(12); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)

    QwParameterFile mapstr(mapfile.Data());  //Open the file

    while (mapstr.ReadNextLine()) {
        mapstr.TrimComment('!');   // Remove everything after a '!' character.
        mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
        if (mapstr.LineIsEmpty())  continue;

        if (mapstr.HasVariablePair("=",varname,varvalue)) {
            //  This is a declaration line.  Decode it.
            varname.ToLower();
            UInt_t value = atol(varvalue.Data());
            if (varname=="roc") {
                RegisterROCNumber(value);
                DIRMODE=0;
            } else if (varname=="slot") {
                RegisterSlotNumber(value);
                DIRMODE=0;
            } else if (varname=="pkg") {
                //this will identify the coming sequence is wire plane to direction mapping - Rakitha
                DIRMODE=1;
                package=value;
            }
        } else if (DIRMODE==0) {
            //  Break this line into tokens to process it.
            chan    = (atol(mapstr.GetNextToken(", ").c_str()));
            package = 1;
            plane   = (atol(mapstr.GetNextToken(", ").c_str()));
            wire    = (atol(mapstr.GetNextToken(", ").c_str()));

            BuildWireDataStructure(chan, package, plane, wire);
        } else if (DIRMODE==1) {
            //this will decode the wire plane directions - Rakitha
            plane     = (atol(mapstr.GetNextToken(", ").c_str()));
            direction = (atol(mapstr.GetNextToken(", ").c_str()));
            fDirectionData.at(package-1).at(plane-1)=direction;
        }

    }


    //  Construct the wire data structures.

    AddChannelDefinition(plane, wire);

    /*
    for (size_t i=0; i<fDirectionData.at(0).size(); i++){
    std::cout<<"Direction data Plane "<<i+1<<" "<<fDirectionData.at(0).at(i)<<std::endl;
    }
    */
    //
    ReportConfiguration();
    return OK;
};


void  QwDriftChamber::CalculateDriftDistance()
{ //Currently This routine is not in use the drift distance calculation is done at ProcessEvent() on each sub-class
  for (std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) {

    if (hit1->GetTime()<0) continue;
    hit1->SetDriftDistance(CalculateDriftDistance(hit1->GetTime(),hit1->GetDetectorID()));

  }
  return;
};




void  QwDriftChamber::ClearEventData()
{
  SetDataLoaded(kFALSE);
  QwDetectorID this_det;
  //  Loop through fTDCHits, to decide which detector elements need to be cleared.
  for (std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
    this_det = hit1->GetDetectorID();
    fWireData.at(this_det.fPlane).at(this_det.fElement).ClearHits();
  }
  fTDCHits.clear();
  for (UInt_t i=0; i<fReferenceData.size(); i++) {
    fReferenceData.at(i).clear();
  }
  return;
};










Int_t QwDriftChamber::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  Int_t  index = GetSubbankIndex(roc_id,bank_id);


  Int_t  old_event_number     = -1;
  Int_t  new_event_number     = -1;
  UInt_t  old_trigger_time    = 0;
  UInt_t  new_trigger_time    = 0;
  UInt_t  trigger_time_offset = 0;

  const UInt_t valid_trigger_time_offset = 1;
  const UInt_t max_f1_trigger_time = 511;
  const UInt_t min_f1_trigger_time = 0;

  Int_t tdc_slot_number    = 0;
  Int_t tdc_channel_number = 0;

  Bool_t temp_print_flag = false;

  if (index>=0 && num_words>0) {
    //  We want to process this ROC.  Begin looping through the data.
    SetDataLoaded(kTRUE);

    if (fDEBUG) std::cout << "QwDriftChamber::ProcessEvBuffer:  "
			  << "Begin processing ROC" << roc_id << std::endl;

    if(temp_print_flag) printf("\n");

    for (UInt_t i=0; i<num_words ; i++) {

      //  Decode this word as a F1TDC word.
      DecodeTDCWord(buffer[i], roc_id); // MQwF1TDC or MQwV775TDC
      // For MQwF1TDC,   roc_id is needed to print out some warning messages.
      // For MQwV775TDC, roc_id isn't necessary, thus I set roc_id=0 in
      //                 MQwV775TDC.h  (Mon May  3 12:32:06 EDT 2010 jhlee)


      tdc_slot_number = GetTDCSlotNumber();

      if ( tdc_slot_number == 31) {
	//  This is a custom word which is not defined in
	//  the F1TDC, so we can use it as a marker for
	//  other data; it may be useful for something.
      }

      if(! IsSlotRegistered(index, tdc_slot_number) ) continue;

      tdc_channel_number = GetTDCChannelNumber();

      if ( IsValidDataword() ) {//;;
	// This is a TDC data word
	try {
	  //std::cout<<"At QwDriftChamber::ProcessEvBuffer"<<std::endl;
	  FillRawTDCWord(index, tdc_slot_number, tdc_channel_number,
			 GetTDCData());
	  PrintTDCData(temp_print_flag);
	}
	catch (std::exception& e) {
	  std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: "
		    << e.what() << std::endl;
	  std::cerr << "   Parameters:  index=="<<index
		    << "; GetF1SlotNumber()=="<< tdc_slot_number
		    << "; GetF1ChannelNumber()=="<<tdc_channel_number
		    << "; GetF1Data()=="<<GetTDCData()
		    << std::endl;
	  Int_t tdcindex = GetTDCIndex(index, tdc_slot_number);
	  std::cerr << "   GetTDCIndex()=="<<tdcindex
		    << "; fTDCPtrs.at(tdcindex).size()=="
		    << fTDCPtrs.at(tdcindex).size()
		    << "; fTDCPtrs.at(tdcindex).at(chan).fPlane=="
		    << fTDCPtrs.at(tdcindex).at(tdc_channel_number).fPlane
		    << "; fTDCPtrs.at(tdcindex).at(chan).fElement=="
		    << fTDCPtrs.at(tdcindex).at(tdc_channel_number).fElement
		    << std::endl;
	  }
      }//;;
      else {//;;
	PrintTDCHeader(temp_print_flag);
	new_trigger_time = GetTDCTriggerTime();

	// Check it is whether F1TDC or V775TDC
	if(  new_trigger_time > min_f1_trigger_time || new_trigger_time < max_f1_trigger_time ) {
	  // the following routine is valid  for only F1TDC
	  new_event_number = GetTDCEventNumber();

	  // skip the first event.
	  if(old_event_number > 0) {

	    if(temp_print_flag) printf("i : %d, old event %d new event %d\n", i, old_event_number, new_event_number);
	    if( new_event_number != old_event_number ) {

	      // Any difference in the Event Number among the chips indicates a serious error
	      // that requires a reset of the board.
	      QwError << QwColor(Qw::kBold)
		      << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << QwLog::endl;
	      QwError << QwColor(Qw::kBold)
		      << "       REQUIRE a reset of the F1TDC board at ROC"  << roc_id << " Slot " << tdc_slot_number << QwLog::endl;
	      QwError << QwColor(Qw::kBold)
		      << "       Please contact (a) Qweak DAQ expert(s) immediately."<< QwLog::endl;
	      QwError << QwColor(Qw::kBold)
		      << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << QwLog::endl;
	      //	      printf(" QwAnalysis is terminated and there is no ROOT output at the moment.\n");
	      //	      exit(1);
	    }

	    trigger_time_offset = abs( new_trigger_time - old_trigger_time );

	    if( trigger_time_offset > valid_trigger_time_offset ) {

	      // Trigger Time difference of up to 1 count among the chips is acceptable
	      // For the Trigger Time, this assumes that an external SYNC_RESET signal has
	      // been successfully applied at the start of the run
	      // Should we stop QwAnalysis or mark this buffer as bad?

	      if( temp_print_flag ) {
	      QwMessage << QwColor(Qw::kBlue)
			<< "There are SYNC_RESET issue on the F1TDC board at Ch "<<  tdc_channel_number
			<< " ROC " << roc_id << " Slot " << tdc_slot_number << QwLog::endl;
              QwWarning << QwColor(Qw::kBlue)
			<<"        Please contact (a) Qweak DAQ expert(s) immediately."<< QwLog::endl;
	      }

	    }

	  }
	  // save a Event Number and a Trigger Time so as to compare with next ones.
	  old_event_number = new_event_number;

	}
	old_trigger_time = new_trigger_time;
      }//;;
    }//;

  } // for (UInt_t i=0; i<num_words ; i++) {


  return OK;
};


void  QwDriftChamber::ConstructHistograms(TDirectory *folder, TString& prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
  //  Now create the histograms...
  TString region = GetSubsystemName();
  //  Loop over the number of planes.

  const Short_t buffer_size  = 2000;
  Float_t bin_offset = -0.5;

  for (UInt_t i=1;i<fWiresPerPlane.size();i++)
    {
      ///////////////First set of histos////////////////////////////////
      TotHits[i] = new TH1F(Form("%s%sHitsOnEachWirePlane%d", prefix.Data() ,region.Data(),i),
			    Form("Total hits on all wires in plane %d",i),
			    fWiresPerPlane[i], bin_offset, fWiresPerPlane[i]+bin_offset);

      TotHits[i]->GetXaxis()->SetTitle("Wire #");
      TotHits[i]->GetYaxis()->SetTitle("Events");


      ///////////////Second set of histos////////////////////////////////
      WiresHit[i] = new TH1F(Form("%s%sWiresHitPlane%d", prefix.Data() ,region.Data(),i),
			     Form("Number of Wires Hit in plane %d",i),
			     20, bin_offset, 20+bin_offset);
      WiresHit[i]->GetXaxis()->SetTitle("Wires Hit per Event");
      WiresHit[i]->GetYaxis()->SetTitle("Events");

      //////////////Third set of histos/////////////////////////////////
      HitsWire[i] = new TH2F(Form("%s%sHitsOnEachWirePerEventPlane%d", prefix.Data() ,region.Data(),i),
			     Form("hits on all wires per event in plane %d",i),
			     fWiresPerPlane[i],bin_offset,fWiresPerPlane[i]+bin_offset,
			     7, -bin_offset, 7-bin_offset);

      HitsWire[i]->GetXaxis()->SetTitle("Wire Number");
      HitsWire[i]->GetYaxis()->SetTitle("Hits");

      /////////////Fourth set of histos//////////////////////////////////////
      TOFP[i] = new TH1F(Form("%s%sTimeofFlightPlane%d", prefix.Data() ,region.Data(),i),
			 Form("Subtracted time of flight for events in plane %d",i),
			 400,0,0);
      TOFP[i] -> SetDefaultBufferSize(buffer_size);
      TOFP[i] -> GetXaxis()->SetTitle("Time of Flight");
      TOFP[i] -> GetYaxis()->SetTitle("Hits");


      TOFP_raw[i] = new TH1F(Form("%s%sRawTimeofFlightPlane%d", prefix.Data() ,region.Data(),i),
			     Form("Raw time of flight for events in plane %d",i),
			     //			     400,-65000,65000);
			     400, 0,0);
      TOFP_raw[i] -> SetDefaultBufferSize(buffer_size);
      TOFP_raw[i]->GetXaxis()->SetTitle("Time of Flight");
      TOFP_raw[i]->GetYaxis()->SetTitle("Hits");

      //////////////Fifth set of histos/////////////////////////////////////

      TOFW[i] = new TH2F(Form("%s%sTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),i),
			 Form("Subtracted time of flight for each wire in plane %d",i),
			 fWiresPerPlane[i], bin_offset, fWiresPerPlane[i]+bin_offset,
			 100,-40000,65000);
      // why this range is not -65000 ??
      TOFW[i]->GetXaxis()->SetTitle("Wire Number");
      TOFW[i]->GetYaxis()->SetTitle("Time of Flight");

      TOFW_raw[i] = new TH2F(Form("%s%sRawTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),i),
			     Form("Raw time of flight for each wire in plane %d",i),
			     fWiresPerPlane[i], bin_offset, fWiresPerPlane[i]+bin_offset,
			     100,-40000,65000);
      // why this range is not -65000 ??
      TOFW_raw[i]->GetXaxis()->SetTitle("Wire Number");
      TOFW_raw[i]->GetYaxis()->SetTitle("Time of Flight");
    }
  return;
};


void  QwDriftChamber::FillHistograms() {

    if (! HasDataLoaded()) return;


    QwDetectorID   this_detid;
    QwDetectorInfo *this_det;

    //  Fill all of the histograms.


    std::vector<Int_t> wireshitperplane(fWiresPerPlane.size(),0);

    for (std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
        this_detid = hit1->GetDetectorID();
        //std::cout << "fElement during FillHistogram: " << this_detid.fElement << std::endl;
        if (this_detid.fPlane<=0 || this_detid.fElement<=0) {
            if (fDEBUG) {
                std::cout << "QwDriftChamber::FillHistograms:  Bad plane or element index:  fPlane=="
                << this_detid.fPlane << ", fElement==" << this_detid.fElement << std::endl;
            }
            continue;
        }


        this_det   = &(fWireData.at(this_detid.fPlane).at(this_detid.fElement));

        if (hit1->IsFirstDetectorHit()) {
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
        TOFP    [this_detid.fPlane]->Fill(hit1->GetTime());
        TOFW    [this_detid.fPlane]->Fill(this_detid.fElement,hit1->GetTime());



    }

    for (UInt_t iplane=1; iplane<fWiresPerPlane.size(); iplane++)
      {
	WiresHit[iplane]->Fill(wireshitperplane[iplane]);
      }
    return;
};


void  QwDriftChamber::DeleteHistograms()
{
  //  Run the destructors for all of the histogram object pointers.
  //for (size_t i=1;i<fWiresPerPlane.size();i++) {
  for (UInt_t i=1;i<fWiresPerPlane.size();i++) {
    ///////////First set of histos//////////////////////////
    if (TotHits[i]!= NULL) {
      delete TotHits[i];
      TotHits[i] = NULL;
    }
    /////////Second set of histos///////////////////////////
    if (WiresHit[i]!= NULL) {
      delete WiresHit[i];
      WiresHit[i] = NULL;
    }
    ////////Third set of histos/////////////////////////////
    if (HitsWire[i]!= NULL) {
      delete HitsWire[i];
      HitsWire[i] = NULL;
    }
    ////////Fourth set of histos////////////////////////////
    if (TOFP[i]!= NULL) {
      delete TOFP[i];
      TOFP[i] = NULL;
    }
    if (TOFP_raw[i]!= NULL) {
      delete TOFP_raw[i];
      TOFP_raw[i] = NULL;
    }
    //////////Fifth set of histos///////////////////////////
    if (TOFW[i]!= NULL) {
      delete TOFW[i];
      TOFW[i] = NULL;
    }
    if (TOFW_raw[i]!= NULL) {
      delete TOFW_raw[i];
      TOFW_raw[i] = NULL;
    }
  }
  return;

};



void QwDriftChamber::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();
  fTDC_Index.clear();
  fTDCPtrs.clear();
  fWireData.clear();
  fNumberOfTDCs = 0;
  return;
}

Int_t QwDriftChamber::RegisterROCNumber(const UInt_t roc_id)
{
  VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);//subbank id is directly related to the ROC
  if (fReferenceChannels.size()<=fCurrentBankIndex) {
    fReferenceChannels.resize(fCurrentBankIndex+1);
    fReferenceData.resize(fCurrentBankIndex+1);
  }
  std::vector<Int_t> tmpvec(kMaxNumberOfTDCsPerROC,-1);
  fTDC_Index.push_back(tmpvec);
  //std::cout<<"Registering ROC "<<roc_id<<std::endl;

  return fCurrentBankIndex;
};



Int_t QwDriftChamber::RegisterSlotNumber(UInt_t slot_id)
{
    if (slot_id<kMaxNumberOfTDCsPerROC) {
        if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fTDC_Index.size()) {
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
    if (bank_index>=0 && bank_index<fTDC_Index.size()) {
        if (slot_num>=0 && slot_num<fTDC_Index.at(bank_index).size()) {
            tdcindex = fTDC_Index.at(bank_index).at(slot_num);
        }
    }
    return tdcindex;
};


Int_t QwDriftChamber::LinkReferenceChannel ( const UInt_t chan, const UInt_t plane, const UInt_t wire )
{
    fReferenceChannels.at ( fCurrentBankIndex ).first  = fCurrentTDCIndex;
    fReferenceChannels.at ( fCurrentBankIndex ).second = chan;
    //  Register a reference channel with the wire equal to the bank index.
    fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPackage = 0;
    fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPlane   = plane;
    fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fElement = fCurrentBankIndex;
    return OK;
};

Int_t QwDriftChamber::LoadTimeWireOffset(TString t0_map)
{
  //std::cout << "beginning to load t0 file... " << std::endl;
  //
  QwParameterFile mapstr ( t0_map.Data() );

  TString varname,varvalue;
  Int_t package=0,plane=0,wire=0;
  Double_t t0 = 0.0;

  while ( mapstr.ReadNextLine() )
    {
      mapstr.TrimComment ( '!' );
      mapstr.TrimWhitespace();
      if ( mapstr.LineIsEmpty() ) continue;
      if ( mapstr.HasVariablePair ( "=",varname,varvalue ) ) {
	varname.ToLower();
	if (varname=="package") {
	  package = atoi ( varvalue.Data() );
	  if (package> (Int_t) fTimeWireOffsets.size()) fTimeWireOffsets.resize(package);
	} else if (varname=="plane") {
	  //std::cout << "package: "  <<  fTimeWireOffsets.at(package-1).size()<< std::endl;
	  plane = atoi(varvalue.Data());
	  if (plane> (Int_t) fTimeWireOffsets.at(package-1).size()) fTimeWireOffsets.at(package-1).resize(plane);
	  //std::cout << "plane: "  <<  fTimeWireOffsets.at(package-1).size()<< std::endl;

	  // To Siyuan, * : can package be obtained before plane in while loop? if plane is before package
	  //                we have at(-1), thus, if condition is always "false", I guess.
	  //            * : if, else if then can we expect something more?
	  // from Han
	}
	continue;
      }

      wire = ( atoi ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
      t0   = ( atoi ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );

      if (wire > (Int_t)fTimeWireOffsets.at(package-1).at(plane-1).size()) fTimeWireOffsets.at(package-1).at(plane-1).resize(wire);

      fTimeWireOffsets.at(package-1).at(plane-1).at(wire-1) = t0;

    }
  //
  return OK;
}


void QwDriftChamber::SubtractWireTimeOffset()
{
  Int_t package=0,plane=0,wire=0;
  Double_t t0 = 0.0;

  for ( std::vector<QwHit>::iterator iter=fWireHits.begin();iter!=fWireHits.end();iter++ ) {

    package = iter->GetPackage();
    plane   = iter->GetPlane();
    wire    = iter->GetElement();
    t0      = fTimeWireOffsets.at(package-1).at(plane-1).at(wire-1);

    // They are too many magic numbers.

    if (t0>-1300 && t0<-1500) {
      if      (plane == 1) t0 = -1423.75;
      else if (plane == 2) t0 = -1438.28;
      // else ??
    }

    iter->SetTime(iter->GetTime()-t0);
  }
  return;
};


void QwDriftChamber::ApplyTimeCalibration()
{

  Double_t region3_f1tdc_resolution = 0.113186191284663271;

  // 0.1132 ns is for the first CODA setup,  it was replaced as 0.1163ns after March 13 2010
  // need to check them with Siyuan (jhlee)
  //
  // 0.1163 ns is the magic number we want to setup during the Qweak experiment
  // because of DAQ team at JLab suggestion. (That guarantees the best performance of F1TDC)

  for ( std::vector<QwHit>::iterator iter=fWireHits.begin();iter!=fWireHits.end();iter++ )
    {
      iter->SetTime(region3_f1tdc_resolution*iter->GetTime());
    }

  return;
};
