/*File: QwDriftChamberHDC.C                                *
*                                                          *
* Author: P. M. King , Rakitha Beminiwattha                *
* Time-stamp: <2008-07-08 15:40>                           *
**********************************************************/

#include "QwDriftChamberHDC.h"

#include "QwParameterFile.h"
#include "QwLog.h"
#include<boost/bind.hpp>

// Register this subsystem with the factory
QwSubsystemFactory<QwDriftChamberHDC> theDriftChamberHDCFactory("QwDriftChamberHDC");


QwDriftChamberHDC::QwDriftChamberHDC(TString region_tmp):VQwSubsystem(region_tmp),
                                                         QwDriftChamber(region_tmp,fTDCHits)
{
  OK=0;
};



//LoadGeometryDefinition will load QwDetectorInfo vector from a map file
//Currently this method is specific to each region

Int_t QwDriftChamberHDC::LoadGeometryDefinition(TString mapfile)
{

  QwMessage << "Region 2 Qweak Geometry Loading..... " << QwLog::endl;

  TString varname, varvalue,package, direction,dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires,detectorId,region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo local_region2_detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize(kNumPackages);

  //  Int_t pkg,pln;


  DIRMODE=0;

  

  QwParameterFile mapstr(mapfile.Data());  //Open the file

  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      //UInt_t value = atol(varvalue.Data());
      if (varname=="name"){//Beginning of detector information
	DIRMODE=1;
      }
    } else if (DIRMODE==1){
      //  Break this line into tokens to process it.
      varvalue     = (mapstr.GetNextToken(", ").c_str());//this is the sType
      Zpos         = (atof(mapstr.GetNextToken(", ").c_str()));
      rot          = (atof(mapstr.GetNextToken(", ").c_str()) * Qw::deg);
      sp_res       = (atof(mapstr.GetNextToken(", ").c_str()));
      track_res    = (atof(mapstr.GetNextToken(", ").c_str()));
      slope_match  = (atof(mapstr.GetNextToken(", ").c_str()));
      package      = mapstr.GetNextToken(", ").c_str();
      region       = (atol(mapstr.GetNextToken(", ").c_str()));
      dType        = mapstr.GetNextToken(", ").c_str();
      direction    = mapstr.GetNextToken(", ").c_str();
      Det_originX  = (atof(mapstr.GetNextToken(", ").c_str()));
      Det_originY  = (atof(mapstr.GetNextToken(", ").c_str()));
      ActiveWidthX = (atof(mapstr.GetNextToken(", ").c_str()));
      ActiveWidthY = (atof(mapstr.GetNextToken(", ").c_str()));
      ActiveWidthZ = (atof(mapstr.GetNextToken(", ").c_str()));
      WireSpace    = (atof(mapstr.GetNextToken(", ").c_str()));
      FirstWire    = (atof(mapstr.GetNextToken(", ").c_str()));
      W_rcos       = (atof(mapstr.GetNextToken(", ").c_str()));
      W_rsin       = (atof(mapstr.GetNextToken(", ").c_str()));
      TotalWires   = (atol(mapstr.GetNextToken(", ").c_str()));
      detectorId   = (atol(mapstr.GetNextToken(", ").c_str()));

      QwDebug << " HDC : Detector ID " << detectorId << " " << varvalue
              << " Package "     << package << " Plane " << Zpos
              << " Region "      << region << QwLog::endl;

      if (region==2){
	    local_region2_detector.SetDetectorInfo(
						   dType, 
						   Zpos, 
						   rot, 
						   sp_res, 
						   track_res, 
						   slope_match, 
						   package, 
						   region, 
						   direction, 
						   Det_originX, Det_originY, 
						   ActiveWidthX, ActiveWidthY, ActiveWidthZ, 
						   WireSpace, 
						   FirstWire, 
						   W_rcos, W_rsin, 
						   TotalWires, 
						   detectorId
						   );

	    if      (package == "u") fDetectorInfo.at(kPackageUp).push_back(local_region2_detector);
	    else if (package == "d") fDetectorInfo.at(kPackageDown).push_back(local_region2_detector);
      }
    }

  }

  QwMessage << "Loaded Qweak Geometry"<<" Total Detectors in kPackageUP "
	    << fDetectorInfo.at ( kPackageUp ).size()
	    << ", "
	    << "kPackagDown "
	    << fDetectorInfo.at ( kPackageDown ).size()
	    << QwLog::endl;

  QwMessage << "Sorting detector info..." << QwLog::endl;

  std::size_t i = 0;

  std::sort(fDetectorInfo.at(kPackageUp).begin(), fDetectorInfo.at(kPackageUp).end());
  plane = 1;
  for ( i=0; i < fDetectorInfo.at(kPackageUp).size(); i++) {
    fDetectorInfo.at(kPackageUp).at(i).fPlane = plane++;
    QwMessage << " kPackageUp Region " << fDetectorInfo.at(kPackageUp).at(i).fRegion 
	      << " Detector ID " << fDetectorInfo.at(kPackageUp).at(i).fDetectorID 
	      << QwLog::endl;
  }

  plane = 1;
  std::sort(fDetectorInfo.at(kPackageDown).begin(), fDetectorInfo.at(kPackageDown).end());
  for ( i=0; i < fDetectorInfo.at(kPackageDown).size(); i++) {
    fDetectorInfo.at(kPackageDown).at(i).fPlane = plane++;
    QwMessage << " kPackageDown Region " << fDetectorInfo.at(kPackageDown).at(i).fRegion 
	      << " Detector ID " << fDetectorInfo.at(kPackageDown).at(i).fDetectorID 
	      << QwLog::endl;
  }

  QwMessage << "Qweak Geometry Loaded " << QwLog::endl;







  return OK;

};



void  QwDriftChamberHDC::SubtractReferenceTimes()
{
  std::vector<Double_t> reftimes;
  std::vector<Bool_t>   refchecked;
  std::vector<Bool_t>   refokay;
  Bool_t allrefsokay;
  //  Int_t counter = 1;

  std::size_t ref_size = 0;
  std::size_t i = 0;
  std::size_t j = 0;

  ref_size = fReferenceData.size();


  reftimes.resize  ( ref_size );
  refchecked.resize( ref_size );
  refokay.resize   ( ref_size );

  for ( i=0; i<ref_size; i++ ) {
    reftimes.at(i)   = 0.0;
    refchecked.at(i) = kFALSE;
    refokay.at(i)    = kFALSE;
  }

  allrefsokay = kTRUE;

  UInt_t bankid      = 0;
  Double_t raw_time  = 0.0;
  Double_t ref_time  = 0.0;
  Double_t time      = 0.0;
  Double_t educated_guess_t0_correction = 11255.0;

  Bool_t local_debug = false;
  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
    {
      //  Only try to check the reference time for a bank if there is at least one
      //  non-reference hit in the bank.
      bankid = hit->GetSubbankID();
      if ( not refchecked.at(bankid) ) {
	if ( fReferenceData.at(bankid).empty() ) {
	  QwWarning << "QwDriftChamberHDC::SubtractReferenceTimes:  Subbank ID "
		    << bankid << " is missing a reference time." << QwLog::endl;
	  refokay.at(bankid) = kFALSE;
	  allrefsokay        = kFALSE;
	}
	else {
	  reftimes.at(bankid) = fReferenceData.at(bankid).at(0);
	  refokay.at(bankid)  = kTRUE;
	}
	if ( refokay.at(bankid) ){
	  for ( j=0; i<fReferenceData.at(bankid).size(); j++ ) 
	    {
	      fReferenceData.at(bankid).at(j) -= reftimes.at(bankid);
	    }
	}
	refchecked.at(bankid) = kTRUE;
      }
      
    if ( refokay.at(bankid) ) {
      raw_time = (Double_t) hit -> GetRawTime();
      ref_time = (Double_t) reftimes.at(bankid);
      Int_t bank_index = hit->GetSubbankID();
      Int_t slot_num   = hit->GetModule();
      time = fF1TDContainer->ReferenceSignalCorrection(raw_time, ref_time, bank_index, slot_num);

      hit -> SetTime(time+educated_guess_t0_correction); // an educated guess 
      if(local_debug) {
	QwMessage << this->GetSubsystemName()
		  << " BankIndex " << std::setw(2) << bank_index
		  << " Slot "      << std::setw(2) << slot_num
		  << " RawTime : " << std::setw(6) << raw_time
		  << " RefTime : " << std::setw(6) << ref_time
		  << " time : "    << std::setw(6) << time
		  << std::endl;
	
      }
      // if ( counter>0 ) {
      // 	if (hit->GetDetectorID().fPlane==7){//this will read the first hit time of trig_h1
      // 	  trig_h1=hit->GetTime();
      // 	  //std::cout<<"********Found trig_h1 "<< trig_h1<<std::endl;
      // 	  counter=0;
      // 	}
      // }
      //      counter++;
    }
  }
  
  bankid = 0;
  
  if ( not allrefsokay ) {
    std::vector<QwHit> tmp_hits;
    tmp_hits.clear();
    for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) 
      {
	bankid = hit->GetSubbankID();
	if ( refokay.at(bankid) ) tmp_hits.push_back(*hit);
      }
    // std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
    fTDCHits.clear();
    fTDCHits = tmp_hits;
    // std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
  }
  
  
  // Bool_t refs_okay = kTRUE;
  // std::vector<Double_t> reftimes;
  // std::bitset< fReferenceData.size() > refchecked;
  // Boot_t allrefsokay = kTRUE;

  // Int_t counter=1;
  // std::size_t i = 0;
  
  // for ( i=0; i<fReferenceData.size(); i++){
    
  //   if (fReferenceData.at(i).size()==0){
  //     //  There isn't a reference time!
  //     //QwWarning << "QwDriftChamber:HDC:SubtractReferenceTimes:  Subbank ID "
  //     //<< i << " is missing a reference time." << QwLog::endl;
  //     refs_okay = kFALSE;
  //   } else {
  //     reftimes.at(i) = fReferenceData.at(i).at(0);
  //   }
  // }
  // if (refs_okay) {
  //   for (size_t i=0; i<fReferenceData.size(); i++){
  //     for (size_t j=0; j<fReferenceData.at(i).size(); j++){
  // 	fReferenceData.at(i).at(j) -= reftimes.at(i);
  //     }
  //   }
  //   for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {

  //     hit1->SetTime(QwDriftChamber::fF1TDC.ActualTimeDifference(hit1->GetRawTime(),reftimes.at(hit1->GetSubbankID())) );
  //     if (counter>0){
  // 	if (hit1->GetDetectorID().fPlane==7){//this will read the first hit time of trig_h1
  // 	  trig_h1=hit1->GetTime();
  // 	  //std::cout<<"********Found trig_h1 "<< trig_h1<<std::endl;
  // 	  counter=0;
  // 	}
  //     }
  //     counter++;
  //   }
  // }
  return;
};


Double_t  QwDriftChamberHDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector)
{
  //0.00545449393  0.0668865488  0.000352462179 -2.00383196E-05  3.57577417E-07  -2.82802562E-09  7.89009965E-12
  // Double_t dt_ = 0.12 * (drifttime-trig_h1 + 933.0);
  //Double_t dt_ = 0.12 * (drifttime);
 
  Double_t dt_= drifttime;
  Double_t dd_ = 0.0;
  if(dt_<0 || dt_ >350) return dd_ ;
  dd_ = -0.0916472
      + 0.0167346 * dt_
      + 0.000523155 * dt_ * dt_
      + (-5.08001E-06 * dt_ * dt_ * dt_)
      + ( 2.0181E-08 * dt_ * dt_ * dt_ * dt_)
      + (-3.85257E-11 * dt_ * dt_ * dt_ * dt_ * dt_)
      + ( 2.89459E-14 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_);

   //   dd_ = 0.00545449393
//       + 0.0668865488 * dt_
//       + 0.000352462179 * dt_ * dt_
//       + (-2.00383196E-05 * dt_ * dt_ * dt_)
//       + ( 3.57577417E-07 * dt_ * dt_ * dt_ * dt_)
//       + (-2.82802562E-09 * dt_ * dt_ * dt_ * dt_ * dt_)
//       + ( 7.89009965E-12 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_);

  //std::cout<<" Drift distance "<<dd_<<" Drift time "<<dt_<<" Original value  "<<drifttime<<std::endl;

  // This formula returns a drift distance in [mm], but the tracking code
  // expects a number in [cm].  Since the geometry definitions are in [cm],
  // we should stick to [cm] as unit of distance. (wdconinc)
  return dd_ / 10.0;
};


void  QwDriftChamberHDC::FillRawTDCWord (Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data)
{
  Bool_t local_debug = false;
  Int_t tdcindex =  0;

  tdcindex = GetTDCIndex(bank_index,slot_num);

  if (tdcindex not_eq -1) {

    Int_t hitcnt  = 0;
    Int_t plane   = 0;
    Int_t wire    = 0;
    EQwDetectorPackage package = kPackageNull;
    EQwDirectionID direction   = kDirectionNull;
    
    plane   = fTDCPtrs.at(tdcindex).at(chan).fPlane;
    wire    = fTDCPtrs.at(tdcindex).at(chan).fElement;
    package = fTDCPtrs.at(tdcindex).at(chan).fPackage;

  

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
};




Int_t QwDriftChamberHDC::BuildWireDataStructure(const UInt_t chan, 
						const EQwDetectorPackage package, 
						const Int_t plane, 
						const Int_t wire)
{
  if (plane == kReferenceChannelPlaneNumber){
    LinkReferenceChannel(chan, plane, wire);
  } 
  else {
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
};

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
  std::cout << " QwDriftChamberHDC::AddChannelDefinition END"<<std::endl;
  return OK;
}

void  QwDriftChamberHDC::ProcessEvent()
{
  if (not HasDataLoaded()) return;

  SubtractReferenceTimes();

  Int_t package = 0;
  Int_t plane   = 0;

  QwDetectorID local_id;
  QwDetectorInfo * local_info;

  for(std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++) {
    
    //if (hit->GetDetectorID().fPlane<7){
    //std::cout<<"Plane "<<hit->GetDetectorID().fPlane<<std::endl;
    // Set the detector info pointer for this hit
    // (TODO Should probably go in VQwSubsystemTracking or even VQwSubsystem)
    
    local_id   = hit->GetDetectorID();
    package    = local_id.fPackage;
    plane      = local_id.fPlane - 1;
    // ahha, here is a hidden magic number 1.
    local_info = & fDetectorInfo.at(package).at(plane);
    
    hit->SetDetectorInfo(local_info);
    //     hit->SetDriftDistance(CalculateDriftDistance(hit1->GetTime(),hit1->GetDetectorID()));
    //}
  }
  
  ApplyTimeCalibration();
  FillDriftDistanceToHits();

  return;
};


Int_t QwDriftChamberHDC::LoadChannelMap(TString mapfile)
{
    TString varname, varvalue;
    UInt_t value   = 0;
    UInt_t  chan   = 0;
    UInt_t DIRMODE = 0;
    Int_t  plane   = 0;
    Int_t  wire    = 0;

    EQwDetectorPackage package = kPackageNull;
    EQwDirectionID   direction = kDirectionNull;

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
	  chan    = (atol(mapstr.GetNextToken(", ").c_str()));
	  //package = 1;
	  plane   = (atol(mapstr.GetNextToken(", ").c_str()));
	  wire    = (atol(mapstr.GetNextToken(", ").c_str()));
	  
	  // VDC and HDC
	  BuildWireDataStructure(chan, package, plane, wire);
	  
        } 
	else if (DIRMODE==1) {
	  //this will decode the wire plane directions - Rakitha
	  plane     = (atol(mapstr.GetNextToken(", ").c_str()));
	  direction = (EQwDirectionID) (atol(mapstr.GetNextToken(", ").c_str()));
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


    // /   ReportConfiguration();

    return OK;
};




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
// };



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
			       Form("%s%sHitsOnEachWirePlane%d", prefix.Data(), region.Data(), iplane),
			       Form("Total hits on all wires in plane %d", iplane),
			       fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset
			       );
    
    TotHits[iplane]->GetXaxis()->SetTitle("Wire #");
    TotHits[iplane]->GetYaxis()->SetTitle("Events");
    
    WiresHit[iplane] = new TH1F(
				Form("%s%sWiresHitPlane%d", prefix.Data(), region.Data(), iplane),
				Form("Number of Wires Hit in plane %d",iplane),
				20, bin_offset, 20+bin_offset
				);
    WiresHit[iplane]->GetXaxis()->SetTitle("Wires Hit per Event");
    WiresHit[iplane]->GetYaxis()->SetTitle("Events");
    
    HitsWire[iplane] = new TH2F(
				Form("%s%sHitsOnEachWirePerEventPlane%d", prefix.Data(), region.Data(), iplane),
				Form("hits on all wires per event in plane %d", iplane),
				fWiresPerPlane[iplane],bin_offset,fWiresPerPlane[iplane]+bin_offset,
				7, -bin_offset, 7-bin_offset
				);
    HitsWire[iplane]->GetXaxis()->SetTitle("Wire Number");
    HitsWire[iplane]->GetYaxis()->SetTitle("Hits");
    
    TOFP[iplane] = new TH1F(
			    Form("%s%sTimeofFlightPlane%d", prefix.Data(), region.Data(), iplane),
			    Form("Subtracted time of flight for events in plane %d", iplane),
			    400,0,0
			    );
    TOFP[iplane] -> SetDefaultBufferSize(buffer_size);
    TOFP[iplane] -> GetXaxis()->SetTitle("Time of Flight");
    TOFP[iplane] -> GetYaxis()->SetTitle("Hits");
    
    
    TOFP_raw[iplane] = new TH1F(
				Form("%s%sRawTimeofFlightPlane%d", prefix.Data(), region.Data(), iplane),
				Form("Raw time of flight for events in plane %d", iplane),
				//			     400,-65000,65000);
				400, 0,0
				);
    TOFP_raw[iplane] -> SetDefaultBufferSize(buffer_size);
    TOFP_raw[iplane]->GetXaxis()->SetTitle("Time of Flight");
    TOFP_raw[iplane]->GetYaxis()->SetTitle("Hits");
    
    TOFW[iplane] = new TH2F(
			    Form("%s%sTimeofFlightperWirePlane%d", prefix.Data(), region.Data(), iplane),
			    Form("Subtracted time of flight for each wire in plane %d", iplane),
			    fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset,
			    100,-40000,65000
			    );
    // why this range is not -65000 ??
    TOFW[iplane]->GetXaxis()->SetTitle("Wire Number");
    TOFW[iplane]->GetYaxis()->SetTitle("Time of Flight");
    
    TOFW_raw[iplane] = new TH2F(
				Form("%s%sRawTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),iplane),
				Form("Raw time of flight for each wire in plane %d",iplane),
				fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset,
				100,-40000,65000
				);
    // why this range is not -65000 ??
    TOFW_raw[iplane]->GetXaxis()->SetTitle("Wire Number");
    TOFW_raw[iplane]->GetYaxis()->SetTitle("Time of Flight");
  }
  return;
};



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


  for(std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++) {
    
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
    time     = hit->GetTime();

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
};



// void  QwDriftChamberHDC::DeleteHistograms()
// {
//   TSeqCollection *file_list = gROOT->GetListOfFiles();
//   for(Int_t i=0; i<file_list->GetSize(); i++) {
//     TFile * file = (TFile*) file_list->At(i);
//     file -> WriteObject(QwDriftChamber::fF1TDContainer->GetErrorSummary(),
// 			Form("%s : F1TDC Error Summary", GetSubsystemName().Data())
// 			);
//   }

//   return;
// };


void  QwDriftChamberHDC::ClearEventData()
{
  SetDataLoaded(kFALSE);
  QwDetectorID this_det;
  //  Loop through fTDCHits, to decide which detector elements need to be cleared.
  for (std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
    this_det = hit1->GetDetectorID();
    fWireData.at(this_det.fPlane).at(this_det.fElement).ClearHits();
  }
  fTDCHits.clear();
  std::size_t i = 0;
  for (i=0; i<fReferenceData.size(); i++) {
    fReferenceData.at(i).clear();
  }
  return;
};




void QwDriftChamberHDC::ApplyTimeCalibration()
{
  Double_t f1tdc_resolution_ns = 0.0;
  f1tdc_resolution_ns = fF1TDContainer -> GetF1TDCResolution();
  if (f1tdc_resolution_ns==0.0) {
    f1tdc_resolution_ns = 0.116312881651642913;
    //  printf("WARNING : QwDriftChamberHDC::ApplyTimeCalibration() the predefined resolution %8.6lf (ns) is used to do further, but it must be checked.\n", f1tdc_resolution_ns);
  }

  for(std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++) 
    {
      hit -> SetTime(f1tdc_resolution_ns*hit->GetTime() );
    }
  
  return;
};
