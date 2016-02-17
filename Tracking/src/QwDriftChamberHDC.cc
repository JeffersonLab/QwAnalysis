/*File: QwDriftChamberHDC.C                                *
*                                                          *
* Author: P. M. King , Rakitha Beminiwattha                *
* Time-stamp: <2008-07-08 15:40>                           *
**********************************************************/

#include "QwDriftChamberHDC.h"

#include "QwParameterFile.h"
#include "QwLog.h"
#include <boost/bind.hpp>

// Register this subsystem with the factory
RegisterSubsystemFactory(QwDriftChamberHDC);


void  QwDriftChamberHDC::SubtractReferenceTimes()
{
  UInt_t   bank_index        = 0;
  Double_t raw_time_arb_unit = 0.0;
  Double_t ref_time_arb_unit = 0.0;
  Double_t time_arb_unit     = 0.0;
  // EQwDetectorPackage package = kPackageNull;

  Bool_t local_debug = false;
  Int_t slot_num = 0;

  TString reference_name1 = "MasterTrigger";
  TString reference_name2 = "CopyMasterTrigger";

  std::vector<QwHit>::iterator end=fTDCHits.end();
  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=end; hit++ ) 
    {

      bank_index        =            hit  -> GetSubbankID();
      slot_num          =            hit  -> GetModule();
      raw_time_arb_unit = (Double_t) hit  -> GetRawTime();


      // John L. introduced this package match reference time subtraction for only Region 3,
      // I was told it would be better to extend this concept to Region2, so I did, and I 
      // got the worst result.....  Q2 = +2.735199e-02  its RMS +1.192504e-02 
      // if I don't use this method in Region2, Q2 = +2.292274e-02  and RMS = +7.133603e-03
      // if I don't use this method in Region3, I couldn't get "peak" structure 
      // in time distributions of four VDC chambers. 
      // 
      // Why does this method give us a good result for only Region 3? MUX? 
      // I don't know ..
      // Friday, February 17 14:01:12 EST 2012, jhlee

      //    package           =            hit  -> GetPackage();
      // if (package == kPackageUp) { // Up is 1
      // 	reference_name1 = "TrigScintPkg1";
      // }
      // else if (package == kPackageDown) {// Down is 2
      // 	reference_name1 = "TrigScintPkg2";
      // }
      // else {
      // 	reference_name1 = "MasterTrigger";
      // }

      ref_time_arb_unit = fF1RefContainer -> GetReferenceTimeAU(bank_index, reference_name1);
      //
      // if there is no reference time due to a channel error, try to use a copy of mater trigger
      // 
      if(ref_time_arb_unit==0.0) {
	ref_time_arb_unit =  fF1RefContainer->GetReferenceTimeAU(bank_index, reference_name2);
      }
      // second time, it returns 0.0, we simply ignore this event .... 
      // set time zero. ReferenceSignalCorrection() will return zero, and increase RFM counter...
      //
      time_arb_unit = fF1TDContainer->ReferenceSignalCorrection(raw_time_arb_unit, ref_time_arb_unit, bank_index, slot_num);

      hit -> SetTime(time_arb_unit); 
      hit -> SetRawRefTime((UInt_t) ref_time_arb_unit);

      if(local_debug) {
	QwMessage << this->GetSubsystemName()
		  << " BankIndex " << std::setw(2) << bank_index
		  << " Slot "      << std::setw(2) << slot_num
		  << " RawTime : " << std::setw(6) << raw_time_arb_unit
		  << " RefTime : " << std::setw(6) << ref_time_arb_unit
		  << " time : "    << std::setw(6) << time_arb_unit
		  << std::endl;
	
      }
    }



  return;
}




// void  QwDriftChamberHDC::SubtractReferenceTimes()
// {
//   std::vector<Double_t> reftimes;
//   std::vector<Bool_t>   refchecked;
//   std::vector<Bool_t>   refokay;
//   Bool_t allrefsokay;
//   //  Int_t counter = 1;

//   std::size_t ref_size = 0;
//   std::size_t i = 0;
//   std::size_t j = 0;

//   ref_size = fReferenceData.size();


//   reftimes.resize  ( ref_size );
//   refchecked.resize( ref_size );
//   refokay.resize   ( ref_size );

//   for ( i=0; i<ref_size; i++ ) {
//     reftimes.at(i)   = 0.0;
//     refchecked.at(i) = kFALSE;
//     refokay.at(i)    = kFALSE;
//   }

//   allrefsokay = kTRUE;

//   UInt_t bankid      = 0;
//   Double_t raw_time_arb_unit = 0.0;
//   Double_t ref_time_arb_unit = 0.0;
//   Double_t time_arb_unit     = 0.0;

//   Bool_t local_debug = true;

//   for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
//     {
//       //  Only try to check the reference time for a bank if there is at least one
//       //  non-reference hit in the bank.
//       bankid = hit->GetSubbankID();
//       if ( not refchecked.at(bankid) ) {
// 	if ( fReferenceData.at(bankid).empty() ) {
// 	  QwWarning << "QwDriftChamberHDC::SubtractReferenceTimes:  Subbank ID "
// 		    << bankid << " is missing a reference time." << QwLog::endl;
// 	  refokay.at(bankid) = kFALSE;
// 	  allrefsokay        = kFALSE;
// 	}
// 	else {
// 	  if(fReferenceData.at(bankid).size() not_eq 1) {
// 	    std::cout << "Multiple hits are recorded in the reference channel, we use the first hit signal as the refererence signal." << std::endl;
// 	  }
// 	  reftimes.at(bankid) = fReferenceData.at(bankid).at(0);
// 	  refokay.at(bankid)  = kTRUE;
// 	}
// 	if ( refokay.at(bankid) ){
// 	  for ( j=0; j<fReferenceData.at(bankid).size(); j++ ) 
// 	    {
// 	      fReferenceData.at(bankid).at(j) -= reftimes.at(bankid);
// 	    }
// 	}
// 	refchecked.at(bankid) = kTRUE;
//       }
      
//     if ( refokay.at(bankid) ) {
//       Int_t slot_num    = hit -> GetModule();
//       raw_time_arb_unit = (Double_t) hit -> GetRawTime();
//       ref_time_arb_unit = (Double_t) reftimes.at(bankid);
//       //      raw_time = (Double_t) hit -> GetRawTime();
//       //      ref_time = (Double_t) reftimes.at(bankid);
//       //      Int_t bank_index = hit->GetSubbankID();
//       //      Int_t slot_num   = hit->GetModule();

//       time_arb_unit = fF1TDContainer->ReferenceSignalCorrection(raw_time_arb_unit, ref_time_arb_unit, bankid, slot_num);

//       hit -> SetTime(time_arb_unit); 
//       hit -> SetRawRefTime((UInt_t) ref_time_arb_unit);

//       //  hit -> SetTime(time+educated_guess_t0_correction); // an educated guess 


//       if(local_debug) {
// 	QwMessage << this->GetSubsystemName()
// 		  << " BankIndex " << std::setw(2) << bankid
// 		  << " Slot "      << std::setw(2) << slot_num
// 		  << " RawTime : " << std::setw(6) << raw_time_arb_unit
// 		  << " RefTime : " << std::setw(6) << ref_time_arb_unit
// 		  << " time : "    << std::setw(6) << time_arb_unit
// 		  << std::endl;
	
//       }
//     }
//   }
  
//   bankid = 0;
  
//   if ( not allrefsokay ) {
//     std::vector<QwHit> tmp_hits;
//     tmp_hits.clear();
//     for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
//       {
// 	bankid = hit->GetSubbankID();
// 	if ( refokay.at(bankid) ) tmp_hits.push_back(*hit);
//       }
//     // std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
//     fTDCHits.clear();
//     fTDCHits = tmp_hits;
//     // std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
//   }
  
//   return;
// }


Double_t  QwDriftChamberHDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector)
{
  //0.00545449393  0.0668865488  0.000352462179 -2.00383196E-05  3.57577417E-07  -2.82802562E-09  7.89009965E-12
  // Double_t dt_ = 0.12 * (drifttime-trig_h1 + 933.0);
  //Double_t dt_ = 0.12 * (drifttime);
 
  Double_t dt_= drifttime;
  //Double_t dd_ = 0.0,t0=0.0;
  Double_t dd_ = 0.0;
  Double_t resolution=1.0;
  //dt_-=t0;
  if(dt_>=0 && dt_<130){
    Int_t index = (Int_t) (dt_/resolution);
  dd_=( dt_-resolution*index ) /resolution * ( fTtoDNumbers.at ( index+1 )-fTtoDNumbers.at ( index ) ) +fTtoDNumbers.at ( index );}
  else{
    dd_=-50;
  }
  //dd_ = -0.0138896
  //  + 0.00987685 * dt_
  //  + 0.00100368 * dt_ * dt_
  //  + (-1.79785E-06 * dt_ * dt_ * dt_)
  //  + ( -8.96859E-08 * dt_ * dt_ * dt_ * dt_)
  //  + (6.11736E-10 * dt_ * dt_ * dt_ * dt_ * dt_)
  //  + ( -1.15889E-12 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_);
  /*
  dd_ = 0.078067
        + 0.0437269 * dt_
        + 0.00117295 * dt_ * dt_
        + (-2.25244E-05 * dt_ * dt_ * dt_ )
        + (1.56369E-07 * dt_ * dt_ * dt_ * dt_  )
        + (-4.93323E-10 * dt_ * dt_ * dt_ * dt_ * dt_ )
    + (5.91555E-13 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_ );
  */
  /*  modified on 418
    dd_ = 0.134955
        + 0.0173593 * dt_
        + 0.00254912 * dt_ * dt_
        + (-4.26414E-05 * dt_ * dt_ * dt_ )
        + (2.86094E-07 * dt_ * dt_ * dt_ * dt_  )
        + (-8.85512E-10 * dt_ * dt_ * dt_ * dt_ * dt_ )
     + (1.04669E-12 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_ );
  */
  /*dd_ = -0.0382463
        + 0.0704644 * dt_
        - 0.00089724454912 * dt_ * dt_
        + (4.8703E-05 * dt_ * dt_ * dt_ )
        + (-8.40592E-07 * dt_ * dt_ * dt_ * dt_  )
        + (5.61285E-09 * dt_ * dt_ * dt_ * dt_ * dt_ )
     + (-1.31489E-11 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_ );
  */
  //dd_ = 0.015993
  //    + 0.0466621 * dt_
  //    + 0.00180132 * dt_ * dt_
  //    + (-5.96573E-05 * dt_ * dt_ * dt_ )
  //    + (1.18497E-06 * dt_ * dt_ * dt_ * dt_  )
  //    + (-1.2071E-08 * dt_ * dt_ * dt_ * dt_ * dt_ )
  // + (4.50584E-11 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_ );
  /*  dd_ = -0.0181483
      + 0.0867558 * dt_
       -0.00115513 * dt_ * dt_
       + (4.41846E-05 * dt_ * dt_ * dt_)
       + (-6.9308E-07 * dt_ * dt_ * dt_ * dt_)
       + (4.30973E-09 * dt_ * dt_ * dt_ * dt_ * dt_)
       + (-9.32219E-12 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_);
  */
  //dd_ = -0.119434
  //   + 0.0321642 * dt_
  //   + 0.0011334 * dt_ * dt_
  //   + (-1.08359E-05 * dt_ * dt_ * dt_)
  //   + (-2.78272E-08 * dt_ * dt_ * dt_ * dt_)
  //   + (5.97029E-10 * dt_ * dt_ * dt_ * dt_ * dt_)
  //   + (-1.67945E-12 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_);

  //std::cout<<" Drift distance "<<dd_<<" Drift time "<<dt_<<" Original value  "<<drifttime<<std::endl;

  // This formula returns a drift distance in [mm], but the tracking code
  // expects a number in [cm].  Since the geometry definitions are in [cm],
  // we should stick to [cm] as unit of distance. (wdconinc)
  return dd_ / 10.0;
}


void  QwDriftChamberHDC::FillRawTDCWord (Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data)
{
  Bool_t local_debug = false;
  Int_t tdcindex =  0;

  tdcindex = GetTDCIndex(bank_index,slot_num);

  if (tdcindex not_eq -1) {

    Int_t hitcnt  = 0;
    EQwDirectionID direction   = kDirectionNull;
    
    fF1RefContainer->SetReferenceSignal(bank_index, slot_num, chan, data, local_debug);


    Int_t plane   = fTDCPtrs.at(tdcindex).at(chan).fPlane;
    Int_t wire    = fTDCPtrs.at(tdcindex).at(chan).fElement;
    EQwDetectorPackage package = fTDCPtrs.at(tdcindex).at(chan).fPackage;
    Int_t octant  = fTDCPtrs.at(tdcindex).at(chan).fOctant;

  

    if (plane == -1 or wire == -1){
      //  This channel is not connected to anything. Do nothing.
    }
    else if (plane == kReferenceChannelPlaneNumber){
      fReferenceData.at(wire).push_back(data);
      //now wire contains the value fCurrentBankIndex so we can assign the ref timing data to it.
    }
    else {
   
      direction = (EQwDirectionID)fDirectionData.at(package-1).at(plane-1); 
      //Wire Direction is accessed from the vector -Rakitha (10/23/2008)
      //hitCount gives the total number of hits on a given wire -Rakitha (10/23/2008)
      hitcnt = std::count_if(fTDCHits.begin(), fTDCHits.end(), 
			     boost::bind(
					 &QwHit::WireMatches, _1, kRegionID2, boost::ref(package), boost::ref(plane), boost::ref(wire)
					 ) 
			     );
      
      fTDCHits.push_back(
			 QwHit(
			       bank_index, 
			       slot_num, 
			       chan, 
                               hitcnt, 
                               kRegionID2, 
                               package, 
                               octant,
                               plane,
                               direction, 
                               wire, 
			       data
			       )
			 );
      //in order-> bank index, slot num, chan, hitcount, region=2, package, plane,,direction, wire,wire hit time
      if(local_debug) {
	std::cout << "At QwDriftChamberHDC::FillRawTDCWord " << "\n";
	std::cout << " hitcnt " << hitcnt
		  << " plane  " << plane
		  << " wire   " << wire
		  << " package " << package 
		  << " fTDCHits.size() " <<  fTDCHits.size() 
		  << std::endl;
      }

    }

  }
  
  return;
}




Int_t QwDriftChamberHDC::BuildWireDataStructure(const UInt_t chan, 
                                                const EQwDetectorPackage package, 
                                                const Int_t octant,
                                                const Int_t plane, 
                                                const Int_t wire)
{
  if (plane == kReferenceChannelPlaneNumber ){
    LinkReferenceChannel(chan, plane, wire);
  } 
  else {
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fOctant  = octant;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = package;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = wire;
    
    //    std::cout << "fWiresPerPlane.size() " << fWiresPerPlane.size()
    //	      << " plane " << plane
    //	      << std::endl;
    
    if (plane >= (Int_t) fWiresPerPlane.size()){ // plane is Int_t
      fWiresPerPlane.resize(plane+1);
      // size() is one more larger than last plane number
      // For HDC, plane    1,2,....,12 
      //          vector 0,1,2,....,12
      //          thus, vector.size() returns 13
      // So the magic number "1" is. 
      // Wednesday, July 28 21:56:34 EDT 2010, jhlee
    }
    if (wire>=fWiresPerPlane.at(plane)){
      fWiresPerPlane.at(plane) =  wire+1;
    }
  }
  return OK;
}


Int_t QwDriftChamberHDC::AddChannelDefinition()
{
  bool temp_local_debug = false;

  if (temp_local_debug){
    std::cout << " QwDriftChamberHDC::AddChannelDefinition"<<std::endl;
   }

  std::size_t i =0;

  fWireData.resize(fWiresPerPlane.size());

  for (i=0; i<fWiresPerPlane.size(); i++) {

    if(temp_local_debug){
      std::cout << "wire #" << i
		<< " " << fWiresPerPlane.at(i)
		<< std::endl;
    }
    fWireData.at(i).resize(fWiresPerPlane.at(i));
  }
  
  Int_t mytdc = 0;
  for ( i=0; i<fTDC_Index.size(); i++){
    for (size_t j=0; j<fTDC_Index.at(i).size(); j++){
      mytdc = fTDC_Index.at(i).at(j);
      if (mytdc not_eq -1){
	for (size_t k=0; k<fTDCPtrs.at(mytdc).size(); k++){
	  //	  Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
	  Int_t plane   = fTDCPtrs.at(mytdc).at(k).fPlane;
	  if (( plane>0) and (plane not_eq (Int_t) kReferenceChannelPlaneNumber) ){
	    Int_t wire  = fTDCPtrs.at(mytdc).at(k).fElement;
	    fWireData.at(plane).at(wire).SetElectronics(i,j,k);
	    if(temp_local_debug) {
	      std::cout << "mytdc " << mytdc
			<< "wire #" << wire
			<< " plane  " << plane
			<< " (i j k) (" << i  <<" " << j << " "<< k << ")"
			<< std::endl;
	    }
	  }
	}
      }
    }
  }
  
  return OK;
}

void  QwDriftChamberHDC::ProcessEvent()
{
  if (not HasDataLoaded()) return;

  SubtractReferenceTimes();  // A.U. unit
  UpdateHits();              // Fill QwDetectorInfo, fTimeRes (ns), and fTimeNs (ns) in QwHits

  SubtractWireTimeOffset();  // Subtract t0 offset (ns) and educated guesss t0 (a.u.) from fTime (a.u.) and fTimeNs (ns)
  FillDriftDistanceToHits(); // must call GetTimeNs() instead of GetTime()

  return;
}


Int_t QwDriftChamberHDC::LoadChannelMap(TString mapfile)
{
  Bool_t local_debug = false;
  LoadTtoDParameters ( "R2_TtoDTable.map"  );
  LoadTimeWireOffset ( "R2_timeoffset.map" );

  TString varname, varvalue;
  UInt_t value   = 0;
  UInt_t  chan   = 0;
  UInt_t DIRMODE = 0;
  Int_t  plane   = 0;
  Int_t  wire    = 0;
  TString name   = "";

  EQwDetectorPackage package = kPackageNull;
  EQwDirectionID   direction = kDirectionNull;

  fDirectionData.resize(2);//currently we have 2  package - Rakitha (10/23/2008)
  fDirectionData.at(0).resize(12); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)
  fDirectionData.at(1).resize(12); //currently we have 12 wire planes in each package - Rakitha (10/23/2008)

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)) {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      value = QwParameterFile::GetUInt(varvalue);
      if (value ==0){
	value = atol(varvalue.Data());
      }
      if (varname=="roc") {
	RegisterROCNumber(value,0);
	DIRMODE=0;
      } 
      else if (varname=="bank") {
	RegisterSubbank(value);
	DIRMODE=0;
      } 
      else if (varname=="pkg") {
	//this will identify the coming sequence is wire plane to direction mapping - Rakitha
	DIRMODE=1;
	package=(EQwDetectorPackage)value;
      }
      else if (varname=="slot") {
	RegisterSlotNumber(value);
	DIRMODE=0;
      } 
	    
    } 
    else if (DIRMODE==0) {
      //  Break this line into tokens to process it.
      chan    = mapstr.GetTypedNextToken<Int_t>();
      plane   = mapstr.GetTypedNextToken<Int_t>();
      //          wire    = mapstr.GetTypedNextToken<Int_t>();
      name    = mapstr.GetTypedNextToken<TString>();

      Int_t octant = 0;
      fR2Octant = gQwOptions.GetValue<int>("R2-octant");
      if (package == kPackage1)   octant = (fR2Octant + 4) % 8;
      if (package == kPackage2) octant =  fR2Octant;

      if(local_debug) {
        printf("chan  %8d plan %4d  wire %12s\n", chan, plane, name.Data());
      }
      if (plane==kReferenceChannelPlaneNumber) {
	fF1RefContainer -> AddF1TDCReferenceSignal(new F1TDCReferenceSignal(fCurrentBankIndex, fCurrentSlot, chan, name));

        if (name=="MasterTrigger" ) {
          wire = 0;
          BuildWireDataStructure(chan, package, octant, plane, wire);
        }
      }
      else {
        wire = name.Atoi();
        // VDC and HDC
        BuildWireDataStructure(chan, package, octant, plane, wire);
      }
          
    } 
    else if (DIRMODE==1) {
      //this will decode the wire plane directions - Rakitha
      plane     = mapstr.GetTypedNextToken<Int_t>();
      direction = (EQwDirectionID) mapstr.GetTypedNextToken<Int_t>();
      fDirectionData.at(package-1).at(plane-1) = direction;
    }
	
  }
    
   


  //  Construct the wire data structures.
  AddChannelDefinition();

  /*
    for (size_t i=0; i<fDirectionData.at(0).size(); i++){
    std::cout<<"Direction data Plane "<<i+1<<" "<<fDirectionData.at(0).at(i)<<std::endl;
    }
  */
  //

  mapstr.Close(); // Close the file (ifstream)
  // /   ReportConfiguration();

  return OK;
}




// Int_t QwDriftChamberHDC::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
// {
//   Int_t subbank_index = 0;
//   Bool_t local_debug = false;
  
//   subbank_index = GetSubbankIndex(roc_id, bank_id);
//   if ( local_debug ) {
//     std::cout << "QwDriftChamberVDC::ProcessConfigurationBuffer" << std::endl;
//     std::cout << " roc id " << roc_id
// 	      << " bank_id " << bank_id
// 	      << " subbank_index " << subbank_index
// 	      << " num_words " << num_words
// 	      << std::endl;
//   }
  
//   if (subbank_index>=0 and num_words>0) {
//     //    SetDataLoaded(kTRUE);
//     if (local_debug) {
//       std::cout << "QwDriftChamberHDC::ProcessConfigurationBuffer:  "
// 		<< "Begin processing ROC" << roc_id << std::endl;
//       PrintConfigrationBuffer(buffer,num_words);
//     }
//   }
  
//   return OK;
// }


void QwDriftChamberHDC::DefineOptions ( QwOptions& options )
{
  options.AddOptions("Tracking options") ("R2-octant",
			po::value<int>()->default_value(1),
			"MD Package 2 of R2 is in front of" );
}

void QwDriftChamberHDC::ProcessOptions (QwOptions& options)
{
  fR2Octant = options.GetValue<int>("R2-octant");
}

void  QwDriftChamberHDC::ConstructHistograms(TDirectory *folder, TString& prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if ( folder ) folder->cd();
  //  Now create the histograms...
  TString region = GetSubsystemName();
  //  Loop over the number of planes.

  const Short_t buffer_size  = 2000;
  Float_t bin_offset = -0.5;

  std::size_t total_plane_number = 0;
  total_plane_number = fWiresPerPlane.size();

  TotHits.resize(total_plane_number);
  TOFP.resize(total_plane_number);
  TOFP_raw.resize(total_plane_number);
  WiresHit.resize(total_plane_number);
  TOFW.resize(total_plane_number);
  TOFW_raw.resize(total_plane_number);
  HitsWire.resize(total_plane_number);

  std::size_t iplane = 0;
  
  //std::cout <<  "QwDriftChamberHDC::ConstructHistograms, " 
  //	    <<  "we are contructing histograms with index from 0 to " <<total_plane_number 
  //	    << "\n"
  //	    <<  "Thus, fWiresPerPlane.size() returns "
  //	    << total_plane_number
  //	    << " and its array definition is ["
  //	    << total_plane_number
  //	    << "]."
  //	    <<  " And hist[i] <-> hist.at(i) <-> fWiresPerplane[i] <-> fWiresPerPlane.at(i)"
  //	    << std::endl;

  // wire_per_plane is the number of wire per plane?
  // 
  // we skip the first zero-th plane or wire histogram. thus
  // i starts with '1'. hist[0] is NULL
  
  for ( iplane=1; iplane<total_plane_number; iplane++) {

    // push_back can "push" iplane = 1 into TotHits.at(0) ??
    TotHits[iplane] = new TH1F(
			       Form("%s%sHitsOnEachWirePlane%d", prefix.Data(), region.Data(), (Int_t) iplane),
			       Form("Total hits on all wires in plane %d", (Int_t) iplane),
			       fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset
			       );
    
    TotHits[iplane]->GetXaxis()->SetTitle("Wire #");
    TotHits[iplane]->GetYaxis()->SetTitle("Events");
    
    WiresHit[iplane] = new TH1F(
				Form("%s%sWiresHitPlane%d", prefix.Data(), region.Data(), (Int_t) iplane),
				Form("Number of Wires Hit in plane %d",(Int_t) iplane),
				20, bin_offset, 20+bin_offset
				);
    WiresHit[iplane]->GetXaxis()->SetTitle("Wires Hit per Event");
    WiresHit[iplane]->GetYaxis()->SetTitle("Events");
    
    HitsWire[iplane] = new TH2F(
				Form("%s%sHitsOnEachWirePerEventPlane%d", prefix.Data(), region.Data(), (Int_t) iplane),
				Form("hits on all wires per event in plane %d", (Int_t) iplane),
				fWiresPerPlane[iplane],bin_offset,fWiresPerPlane[iplane]+bin_offset,
				7, -bin_offset, 7-bin_offset
				);
    HitsWire[iplane]->GetXaxis()->SetTitle("Wire Number");
    HitsWire[iplane]->GetYaxis()->SetTitle("Hits");
    
    TOFP[iplane] = new TH1F(
			    Form("%s%sTimeofFlightPlane%d", prefix.Data(), region.Data(), (Int_t) iplane),
			    Form("Subtracted time of flight for events in plane %d", (Int_t) iplane),
			    400,0,0
			    );
    TOFP[iplane] -> SetDefaultBufferSize(buffer_size);
    TOFP[iplane] -> GetXaxis()->SetTitle("Time of Flight");
    TOFP[iplane] -> GetYaxis()->SetTitle("Hits");
    
    
    TOFP_raw[iplane] = new TH1F(
				Form("%s%sRawTimeofFlightPlane%d", prefix.Data(), region.Data(), (Int_t) iplane),
				Form("Raw time of flight for events in plane %d", (Int_t) iplane),
				//			     400,-65000,65000);
				400, 0,0
				);
    TOFP_raw[iplane] -> SetDefaultBufferSize(buffer_size);
    TOFP_raw[iplane]->GetXaxis()->SetTitle("Time of Flight");
    TOFP_raw[iplane]->GetYaxis()->SetTitle("Hits");
    
    TOFW[iplane] = new TH2F(
			    Form("%s%sTimeofFlightperWirePlane%d", prefix.Data(), region.Data(), (Int_t) iplane),
			    Form("Subtracted time of flight for each wire in plane %d",(Int_t) iplane),
			    fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset,
			    100,-40000,65000
			    );
    // why this range is not -65000 ??
    TOFW[iplane]->GetXaxis()->SetTitle("Wire Number");
    TOFW[iplane]->GetYaxis()->SetTitle("Time of Flight");
    
    TOFW_raw[iplane] = new TH2F(
				Form("%s%sRawTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),(Int_t)iplane),
				Form("Raw time of flight for each wire in plane %d",(Int_t)iplane),
				fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset,
				100,-40000,65000
				);
    // why this range is not -65000 ??
    TOFW_raw[iplane]->GetXaxis()->SetTitle("Wire Number");
    TOFW_raw[iplane]->GetYaxis()->SetTitle("Time of Flight");
  }
  return;
}



void  QwDriftChamberHDC::FillHistograms() 
{
  Bool_t local_debug = false;
  if (not HasDataLoaded()) return;
  
  QwDetectorID   this_detid;
  QwDetectorInfo *this_det;
  
  //  Fill all of the histograms.
  
  std::vector<Int_t> wireshitperplane(fWiresPerPlane.size(),0);
  
  UInt_t raw_time = 0;
  Double_t time   = 0.0;

  Int_t plane   = 0;
  Int_t element = 0;

  std::vector<QwHit>::iterator end=fTDCHits.end();
  for(std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=end; hit++) {
    
    this_detid = hit->GetDetectorID();
    plane      = this_detid.fPlane;
    element    = this_detid.fElement;

    if (plane<=0 or element<=0) {
      if (local_debug) {
	QwMessage << "QwDriftChamberHDC::FillHistograms:  Bad plane or element index:"
		  << "  fPlane = "  << plane
		  << "  fElement= " << element
		  << QwLog::endl;
      }
      continue;
    }

    
    this_det= &(fWireData.at(plane).at(element));
    
    if (hit->IsFirstDetectorHit()) {
      //  If this is the first hit for this detector, then let's plot the
      //  total number of hits this wire had.
      HitsWire[plane]->Fill(element,this_det->GetNumHits());
      
      //  Also increment the total number of events in whichthis wire was hit.
      TotHits[plane]->Fill(element,1);
      //  Increment the number of wires hit in this plane
      wireshitperplane.at(plane) += 1;
    }
    
    raw_time = hit->GetRawTime();
    time     = hit->GetTimeNs();

    //  Fill ToF histograms
    TOFP_raw[plane]->Fill(raw_time);
    TOFW_raw[plane]->Fill(element, raw_time);
    TOFP    [plane]->Fill(time);
    TOFW    [plane]->Fill(element, time);
  } 

  std::size_t iplane = 0;
    
  for (iplane=1; iplane<fWiresPerPlane.size(); iplane++) {
    WiresHit[iplane]->Fill(wireshitperplane[iplane]);
  }
  return;
}


void  QwDriftChamberHDC::ClearEventData()
{
  SetDataLoaded(kFALSE);
  QwDetectorID this_det;
  //  Loop through fTDCHits, to decide which detector elements need to be cleared.
  std::vector<QwHit>::iterator end=fTDCHits.end();
  for (std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=end; hit1++) {
    this_det = hit1->GetDetectorID();
    fWireData.at(this_det.fPlane).at(this_det.fElement).ClearHits();
  }
  fTDCHits.clear();

  fF1RefContainer->ClearEventData();
  std::size_t i = 0;
  for (i=0; i<fReferenceData.size(); i++) {
    fReferenceData.at(i).clear();
  }
  return;
}



void QwDriftChamberHDC::UpdateHits()
{


  EQwDetectorPackage package = kPackageNull;
  Int_t plane   = 0;

  QwDetectorID local_id;
  QwDetectorInfo * local_info;

  std::vector<QwHit>::iterator end=fTDCHits.end();
  for(std::vector<QwHit>::iterator iter=fTDCHits.begin(); iter!=end; ++iter)
    {

      local_id   = iter->GetDetectorID();
      package    = local_id.fPackage;
      plane      = local_id.fPlane - 1;
      // ahha, here is a hidden magic number 1.
      local_info = fDetectorInfo.in(package).at(plane);
      iter->SetDetectorInfo(local_info);
      iter->ApplyTimeCalibration(fF1TDCResolutionNS); // Fill fTimeRes and fTimeNs in QwHit
    }


  return;
}


void QwDriftChamberHDC::SubtractWireTimeOffset()
{
  Int_t plane = 0;
  Int_t wire  = 0;
  EQwDetectorPackage package = kPackageNull;
  Double_t t0_NS = 0.0;

  Double_t educated_guess_t0_correction_AU = 11255.0;
  Double_t educated_guess_t0_correction_NS = educated_guess_t0_correction_AU*fF1TDCResolutionNS;

  //hit -> SetTime(time+educated_guess_t0_correction); // an educated guess 

  std::size_t nhits=fTDCHits.size();

  for(std::size_t i=0;i<nhits;i++)
    {
      package = fTDCHits.at(i).GetPackage();
      plane   = fTDCHits.at(i).GetPlane();
      wire    = fTDCHits.at(i).GetElement();
      t0_NS   = fTimeWireOffsets.at ( package-1 ).at ( plane-1 ).at ( wire-1 );
      t0_NS   = t0_NS - educated_guess_t0_correction_NS;
      
      fTDCHits.at(i).SubtractTimeNsOffset(t0_NS);                    // time unit is ns, Replace fTimeNs
      fTDCHits.at(i).SubtractTimeAuOffset(t0_NS/fF1TDCResolutionNS); // time unit is a.u. Replace fTime
     }
  return ;
}

void QwDriftChamberHDC::LoadTtoDParameters ( TString ttod_map )
{
  QwParameterFile mapstr(ttod_map.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  
  while (mapstr.ReadNextLine())
    {
      mapstr.TrimComment('!');
      mapstr.TrimWhitespace();
      if (mapstr.LineIsEmpty()) continue;
      
      /* Double_t t = */ mapstr.GetTypedNextToken<Double_t>();
      Double_t d = mapstr.GetTypedNextToken<Double_t>();
      fTtoDNumbers.push_back(d);
    }

  mapstr.Close(); // Close the file (ifstream)
}


Int_t QwDriftChamberHDC::LoadTimeWireOffset ( TString t0_map )
{
  //std::cout << "beginning to load t0 file... " << std::endl;
  //
  QwParameterFile mapstr ( t0_map.Data() );
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  TString varname,varvalue;
  Int_t plane=0,wire=0;
  Double_t t0 = 0.0;
  EQwDetectorPackage package = kPackageNull;

  while ( mapstr.ReadNextLine() )
    {
      mapstr.TrimComment ( '!' );
      mapstr.TrimWhitespace();
      if ( mapstr.LineIsEmpty() ) continue;
      if ( mapstr.HasVariablePair ( "=",varname,varvalue ) )
	{
	  varname.ToLower();
	  if ( varname=="package" )
	    {
	      package = ( EQwDetectorPackage ) atoi ( varvalue.Data() );
	      if ( package> ( Int_t ) fTimeWireOffsets.size() ) fTimeWireOffsets.resize ( package );
	    }
	  else if ( varname=="plane" )
	    {
	      //std::cout << "package: "  <<  fTimeWireOffsets.at(package-1).size()<< std::endl;
	      plane = atoi ( varvalue.Data() );
	      if ( plane> ( Int_t ) fTimeWireOffsets.at ( package-1 ).size() ) fTimeWireOffsets.at ( package-1 ).resize ( plane );
	      //std::cout << "plane: "  <<  fTimeWireOffsets.at(package-1).size()<< std::endl;

	      // To Siyuan, * : can package be obtained before plane in while loop? if plane is before package
	      //                we have at(-1), thus, if condition is always "false", I guess.
	      //            * : if, else if then can we expect something more?
	      // from Han
	    }
	  continue;
	}

      wire = mapstr.GetTypedNextToken<Int_t>();
      t0   = mapstr.GetTypedNextToken<Int_t>();

      if ( wire > ( Int_t ) fTimeWireOffsets.at ( package-1 ).at ( plane-1 ).size() )
	{
	  fTimeWireOffsets.at ( package-1 ).at ( plane-1 ).resize ( wire );

	  fTimeWireOffsets.at ( package-1 ).at ( plane-1 ).at ( wire-1 ) = t0;
	}

    }
  //

  mapstr.Close(); // Close the file (ifstream)
  return OK;
}
