/**********************************************************\
 * File: QwDriftChamberVDC.C                                *
 *                                                          *
 * Author: P. M. King , Rakitha Beminiwattha                *
 * Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/



#include "QwDriftChamberVDC.h"
#include "QwParameterFile.h"
#include "boost/bind.hpp"


const UInt_t QwDriftChamberVDC::kBackPlaneNum=16;
const UInt_t QwDriftChamberVDC::kLineNum=8;

// Register this subsystem with the factory
QwSubsystemFactory<QwDriftChamberVDC> theDriftChamberVDCFactory("QwDriftChamberVDC");

QwDriftChamberVDC::QwDriftChamberVDC ( TString region_tmp ):
  VQwSubsystem ( region_tmp ), QwDriftChamber ( region_tmp,fWireHitsVDC )
{
  Double_t f1_time_offset = 64495.0;
  Double_t difference     = 30000.0;
  Double_t time_shift     = 0.0;//-8929.0; // is it valid with the new configuration?

  //  difference = 0.5*f1_time_offset;

  //  SetReferenceParameters(-30000., 30000., 64495.,-8929.);
  SetReferenceParameters(-difference, difference, f1_time_offset, time_shift);
  std::vector<QwDelayLine> temp;
  temp.clear();
  temp.resize ( kLineNum );
  fDelayLineArray.resize ( kBackPlaneNum, temp );
  fDelayLinePtrs.resize ( 21 );
  OK=0;
};

Int_t QwDriftChamberVDC::LoadGeometryDefinition ( TString mapfile )
{
  std::cout<<"Region 3 Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  Int_t    plane, TotalWires, detectorId, region, DIRMODE;
  Double_t Zpos, rot, sp_res, track_res, slope_match;
  Double_t Det_originX, Det_originY;
  Double_t ActiveWidthX, ActiveWidthY, ActiveWidthZ;
  Double_t WireSpace, FirstWire, W_rcos, W_rsin;

  QwDetectorInfo local_region3_detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize ( kNumPackages );

  DIRMODE=0;

  QwParameterFile mapstr ( mapfile.Data() );  //Open the file

  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if ( mapstr.LineIsEmpty() )  continue;

    if ( mapstr.HasVariablePair ( "=",varname,varvalue ) ) {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      //UInt_t value = atol(varvalue.Data());
      if ( varname=="name" ) { //Beginning of detector information
	DIRMODE=1;
      }
    }
    else if ( DIRMODE==1 ) {
      //  Break this line Int_to tokens to process it.
      varvalue     = ( mapstr.GetNextToken ( ", " ).c_str() );//this is the sType
      Zpos         = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      rot          = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) * Qw::deg);
      sp_res       = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      track_res    = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      slope_match  = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      package      = mapstr.GetNextToken ( ", " ).c_str();
      region       = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
      dType        = mapstr.GetNextToken ( ", " ).c_str();
      direction    = mapstr.GetNextToken ( ", " ).c_str();
      Det_originX  = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      Det_originY  = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      ActiveWidthX = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      ActiveWidthY = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      ActiveWidthZ = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      WireSpace    = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      FirstWire    = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      W_rcos       = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      W_rsin       = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      TotalWires   = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
      detectorId   = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
      
      if ( region==3 ) 
	{
	  local_region3_detector.SetDetectorInfo(
						 dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, 
						 Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, 
						 WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId 
						 );
	  if      ( package == "u" ) fDetectorInfo.at( kPackageUp   ).push_back(local_region3_detector);
 	  else if ( package == "d" ) fDetectorInfo.at( kPackageDown ).push_back(local_region3_detector);
	}
    }
  }


  std::cout << "Loaded Qweak Geometry"<<" Total Detectors in kPackageUP "
	    << fDetectorInfo.at( kPackageUp   ).size() 
	    << ", "
	    << "kPackagDown "
	    << fDetectorInfo.at( kPackageDown ).size() 
	    << std::endl;
  
  std::size_t i = 0;

  std::cout << "Sorting detector info..." << std::endl;
  std::sort ( fDetectorInfo.at(kPackageUp).begin(), fDetectorInfo.at(kPackageUp).end() );

  plane = 1;
  for ( i=0; i<fDetectorInfo.at(kPackageUp).size(); i++ ) {
    fDetectorInfo.at(kPackageUp).at(i).fPlane = plane++;
    std::cout << "kPackageUp   " 
	      << "Region "      << fDetectorInfo.at(kPackageUp).at(i).fRegion
	      << ", "
	      << "Detector ID " << fDetectorInfo.at(kPackageUp).at(i).fDetectorID 
	      << std::endl;
  }

  std::sort (fDetectorInfo.at(kPackageDown).begin(), fDetectorInfo.at(kPackageDown).end());
  plane = 1;
  for ( i=0; i<fDetectorInfo.at(kPackageDown).size(); i++) {
    fDetectorInfo.at(kPackageDown).at(i).fPlane = plane++;
    std::cout << "kPackageDown "
	      << "Region "       << fDetectorInfo.at(kPackageDown).at(i).fRegion
	      << ", "
	      << "Detector ID " << fDetectorInfo.at(kPackageDown).at(i).fDetectorID 
	      << std::endl;
  }

  std::cout<<"Qweak Geometry Loaded "<<std::endl;

  return OK;
}






void  QwDriftChamberVDC::ReportConfiguration()
{
  UInt_t i,j,k;
  i=j=k=0;
  Int_t ind = 0;
  Int_t tdcindex = 0;
  for ( i=0; i<fROC_IDs.size(); i++ ) {
    for ( j=0; j<fBank_IDs.at ( i ).size(); j++ ) {
      ind = GetSubbankIndex ( fROC_IDs.at ( i ),fBank_IDs.at ( i ).at ( j ) );
      std::cout << "ROC " << fROC_IDs.at ( i )
		<< ", subbank " << fBank_IDs.at ( i ).at ( j )
		<< ":  subbank index==" << ind
		<< std::endl;
      for ( k=0; k<kMaxNumberOfTDCsPerROC; k++ ) {
	tdcindex = GetTDCIndex ( ind, k );
	std::cout << "    Slot " << k;
	if ( tdcindex == -1 )
	  std::cout << "  Empty" << std::endl;
	else
	  std::cout << "  TDC#" << tdcindex << std::endl;
      }
    }
  }
  for ( i=0; i<fWiresPerPlane.size(); i++ ) {
    if ( fWiresPerPlane.at ( i ) == 0 ) continue;
    std::cout << "Plane " << i << " has " << fWireData.at ( i ).size()
	      << " wires"
	      <<std::endl;
  }

  return;
};



void  QwDriftChamberVDC::SubtractReferenceTimes()
{
  UInt_t i = 0;
  std::vector<Double_t> reftimes;
  std::vector<Bool_t>   refchecked;
  std::vector<Bool_t>   refokay;
  Bool_t allrefsokay;

  reftimes.resize  ( fReferenceData.size() );
  refchecked.resize( fReferenceData.size() );
  refokay.resize   ( fReferenceData.size() );

  for ( i=0; i<fReferenceData.size(); i++ ) {
    refchecked.at(i) = kFALSE;
    refokay.at(i)    = kFALSE;
  }

  allrefsokay = kTRUE;

  UInt_t bankid      = 0;
  Double_t raw_time  = 0.0;
  Double_t ref_time  = 0;
  Double_t time      = 0.0;
  Double_t time2     = 0.0;
  Double_t delta     = 0.0;
  Bool_t local_debug = false;

  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) {
    //  Only try to check the reference time for a bank if there is at least one
    //  non-reference hit in the bank.
    bankid = hit->GetSubbankID();

    if (bankid == 0)   std::cerr<< "BANK id" << bankid << std::endl;
    //
    // if bankid == 0, print out bank id, and then what?
    //
    if ( !refchecked.at(bankid) ){

      if ( fReferenceData.at( bankid ).empty() ) {
	std::cerr << "QwDriftChamberVDC::SubtractReferenceTimes:  Subbank ID "
		  << bankid << " is missing a reference time." << std::endl;
	refokay.at(bankid) = kFALSE;
	allrefsokay        = kFALSE;
      } 
      else {
	reftimes.at(bankid) = fReferenceData.at(bankid).at(0);
	refokay.at(bankid)  = kTRUE;
      }
      
      if ( refokay.at(bankid) ){
	for ( i=0; i<fReferenceData.at(bankid).size(); i++ ) {
	  fReferenceData.at(bankid).at(i) -= reftimes.at(bankid);
	}
      }
      refchecked.at(bankid) = kTRUE;
    }
    
    if ( refokay.at(bankid) ){
      raw_time = (Double_t) hit -> GetRawTime();
      ref_time = (Double_t) reftimes.at(bankid);
      time     = ActualTimeDifference(raw_time, ref_time);
      time2    = SubtractReference(raw_time, ref_time);
      delta    = fabs(time - time2);
      // hit -> SetRawTime((UInt_t)raw_time); // why I cannot see any rawtime in qwhit? by jhlee Tuesday, July 13 16:12:26 EDT 2010
      hit -> SetTime(time);
      if(local_debug) {
	  QwMessage << " RawTime : " << raw_time
		    << " RefTime : " << ref_time
		    << " time    : " << time
		    << " time2   : " << time2
		    << " delta   : " << delta
		    << std::endl;
	
      }
    }
  }

  bankid = 0;

  if (! allrefsokay){
    std::vector<QwHit> tmp_hits;
    tmp_hits.clear();
    for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ ) {
      bankid = hit->GetSubbankID();
      if ( refokay.at(bankid) ){
	tmp_hits.push_back(*hit);
      }
    }
    // std::cerr << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
    fTDCHits.clear();
    fTDCHits = tmp_hits;
    // std::cerr << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
  }

  return;
}


Double_t  QwDriftChamberVDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector, Double_t angle_degree)
{
  Double_t distance_mm = 0.0;
  Double_t distance_cm = 0.0;

  Double_t p[11]={0.0};
  ////   for arg-65-ethane-35
  //     p[0]=-1.683+0.02112*angle_degree;
  //     p[1]=23.32-0.298*angle_degree;
  //     p[2]=-4.313+0.01863*angle_degree;
  //     p[3]=31.32-0.3284*angle_degree;
  //     p[4]=7.728-0.1812*angle_degree;
  //     p[5]=19.29-0.1266*angle_degree;
  //     p[6]=2.876-0.04789*angle_degree;
  //     p[7]=0.8265-0.4889*angle_degree;
  //     p[8]=33.25-0.2585*angle_degree;
  //     p[9]=62.38-1.955*angle_degree;
  //     p[10]=22.61-0.005106*angle_degree;

  //for arg-50-ethane-50
  p[ 0] = -1.749 + 0.021670*angle_degree; // [T], then what unit of 0.021670 is?
                                          // [T]/[DEGREE]
                                          // jhlee, confirmed by siyuan
                                          // Thursday, July  8 10:50:23 EDT 2010
  p[ 1] = 24.100 - 0.305100*angle_degree; // [T]/[L]   0.305100 [T]/([L].[DEGREE])
  p[ 2] = -3.133 + 0.009078*angle_degree; // [T]
  p[ 3] = 28.510 - 0.295100*angle_degree; // [T]/[L]
  p[ 4] =  7.869 - 0.163500*angle_degree; // [T]
  p[ 5] = 17.690 - 0.120800*angle_degree; // [T]/[L]
  p[ 6] =  2.495 - 0.041040*angle_degree; // [T]/[L2]
  p[ 7] =  3.359 - 0.478000*angle_degree; // [T]
  p[ 8] = 29.350 - 0.219500*angle_degree; // [T]/[L]
  p[ 9] = 55.680 - 1.730000*angle_degree; // [T]
  p[10] = 20.380 - 0.004761*angle_degree; // [T]/[L]

  Double_t cut[4]  = {0.5, 1.5, 4.0, 6.0};// [L] // mm
  Double_t time[4] = {0.0};// [T]

  time[0] = 0.5*(p[0] + p[1]*cut[0] + p[2] + p[3]*cut[0]);
  time[1] = 0.5*(p[2] + p[3]*cut[1] + p[4] + p[5]*cut[1] + p[6]*cut[1]*cut[1]);
  time[2] = 0.5*(p[4] + p[5]*cut[2] + p[6]*cut[2]*cut[2] + p[7] + p[8]*cut[2]);
  time[3] = 0.5*(p[7] + p[8]*cut[3] + p[9] +p[10]*cut[3]);

  if (drifttime < time[0]) {
    distance_mm = (drifttime - p[0])/p[1];
  }
  else if ( drifttime>=time[0] && drifttime<time[1] ) {
    distance_mm = (drifttime-p[2])/p[3];
  }
  else if ( drifttime>=time[1] && drifttime < time[2] ) {
    distance_mm = (-p[5] + sqrt( p[5]*p[5] - 4.0*p[6]*(p[4]-drifttime)) ) / (2.0*p[6]);
  }
  else if ( drifttime>=time[2] && drifttime<time[3] ) {
    distance_mm = (drifttime - p[7])/p[8];
  }
  else if ( drifttime>=time[3] ){
    distance_mm = (drifttime - p[9])/p[10];
  }
  //if(drifttime>=4 && drifttime < 4.1)
  //std::cout << "drifttime: " << drifttime << " " << "x: " << x << std::endl;

  distance_cm = 0.1*distance_mm;
  return distance_cm;
};


void  QwDriftChamberVDC::FillRawTDCWord ( Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data ) {
  Int_t tdcindex = GetTDCIndex(bank_index,slot_num);
  //Int_t tdcindex=1;
  if ( tdcindex != -1 ) {
    Int_t hitCount = 1;
    EQwDetectorPackage package  = kPackageUp;
    Int_t plane    = fTDCPtrs.at(tdcindex).at(chan).fPlane;
    Int_t wire     = fTDCPtrs.at(tdcindex).at(chan).fElement;

    //  Int_t plane=-2;
    //  if ( slot_num==3 )
    //      plane=fDelayLinePtrs.at ( slot_num ).at ( chan ).fBackPlane;
    //  Int_t wire=0;
    EQwDirectionID direction=kDirectionNull;


    if ( plane == -1 || wire == -1 ) {
      //  This channel is not connected to anything.
      //  Do nothing.
      //  } else if (plane == (Int_t) kReferenceChannelPlaneNumber){
      //fReferenceData.at(wire).push_back(data);
      //        } else if ( slot_num==4 ) {
    } else if (plane == (Int_t) kReferenceChannelPlaneNumber){
      fReferenceData.at ( wire ).push_back ( data );
    } else {
      plane=fDelayLinePtrs.at ( slot_num ).at ( chan ).fBackPlane;
      //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_1"<<endl;
      //direction =fDirectionData.at(package-1).at(plane-1); //wire direction is accessed from the vector and updates the QwHit with it. Rakitha(10/23/2008)

      hitCount=std::count_if(fTDCHits.begin(),fTDCHits.end(),boost::bind(&QwHit::WireMatches,_1,3,boost::ref(package),boost::ref(plane),boost::ref(wire)) );
      fTDCHits.push_back ( QwHit ( bank_index, slot_num, chan, hitCount, kRegionID3, package, plane,direction, wire, data ) );//in order-> bank index, slot num, chan, hitcount, region=3, package, plane,,direction, wire,wire hit time

    }

  };
};

// void  QwDriftChamberVDC::FillRawTDCWord ( Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data ) 
// {
//   Int_t tdcindex = 0;

//   tdcindex = GetTDCIndex(bank_index,slot_num);

//   if ( tdcindex not_eq -1 ) 
//     { 
//       Int_t hitcnt = 0;
//       Int_t plane  = 0;
//       Int_t wire   = 0;
//       Int_t package = 0;

//       hitcnt  = 1;
//       plane   = fTDCPtrs.at(tdcindex).at(chan).fPlane;
//       wire    = fTDCPtrs.at(tdcindex).at(chan).fElement;
//       package = fTDCPtrs.at(tdcindex).at(chan).fPackage;
//       //      EQwDetectorPackage package  = kPackageUp;
//       EQwDirectionID direction = kDirectionNull;
      
//       if ( plane not_eq -1 || wire not_eq -1 ) {
// 	//  This channel is not connected to anything.
// 	//  Do nothing.
// 	//  } else if (plane == (Int_t) kReferenceChannelPlaneNumber){
// 	//fReferenceData.at(wire).push_back(data);
//       } 
//       else if (plane == (Int_t) kReferenceChannelPlaneNumber) {
// 	fReferenceData.at ( wire ).push_back ( data );
//       } 
//       else {
// 	plane = fDelayLinePtrs.at(slot_num).at(chan).fBackPlane;
// 	//std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_1"<<endl;
// 	//direction =fDirectionData.at(package-1).at(plane-1); //wire direction is accessed from the vector and updates the QwHit with it. Rakitha(10/23/2008)
// 	hitcnt = std::count_if(fTDCHits.begin(),fTDCHits.end(),boost::bind(&QwHit::WireMatches,_1,3,boost::ref(package),boost::ref(plane),boost::ref(wire)) );

// 	fTDCHits.push_back ( QwHit (bank_index,slot_num,chan,hitcnt,kRegionID3, (EQwDetectorPackage)package,plane,direction,wire,data));
// 	//in order-> bank index, slot num, chan, hitcount, region=3, package, plane,,direction, wire,wire hit time
	
//       }
      
//     }
//   return;
// };








Int_t QwDriftChamberVDC::BuildWireDataStructure ( const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire ) 
{

  Int_t r3_wire_number_per_plane = 279;

  //  std::cout << "R3 chan " << chan
  //	    << " package "  << package
  //	    << " plane " << plane
  //	    << " wire " << wire
  //	    << std::endl;
  if ( plane == kReferenceChannelPlaneNumber ) {
    LinkReferenceChannel ( chan, plane, wire );
  } 
  else {
    fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPackage = package;
    fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPlane   = plane;
    fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fElement = wire;

    if ( plane>=fWiresPerPlane.size() ) {
      fWiresPerPlane.resize ( plane+1 );
    }
    //if (wire>=fWiresPerPlane.at(plane)){
    //fWiresPerPlane.at(plane) =  wire+1;
    //}
    fWiresPerPlane.at(plane) = r3_wire_number_per_plane;
  }
  return OK;
};

// Int_t QwDriftChamberVDC::AddChannelDefinition ( const UInt_t plane, const UInt_t wire ) 
// {

//   fWireData.resize ( fWiresPerPlane.size() );
//   for ( size_t i=1; i<fWiresPerPlane.size(); i++ ) {
//     fWireData.at ( i ).resize ( fWiresPerPlane.at ( i ) );
//   }

//   for ( size_t i=0; i<fTDC_Index.size(); i++ ) {
//     //    Int_t refchan = i;
//     for ( size_t j=0; j<fTDC_Index.at ( i ).size(); j++ ) {
//       Int_t mytdc = fTDC_Index.at ( i ).at ( j );
//       //std::cout << "mytdc: " << mytdc << std::endl;
//       if ( mytdc!=-1 ) {
//   	for ( size_t k=0; k<fTDCPtrs.at ( mytdc ).size(); k++ ) {
//   	  //	  Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
//   	  Int_t plane   = fTDCPtrs.at ( mytdc ).at ( k ).fPlane;
//   	  if ( plane>0 && plane != ( Int_t ) kReferenceChannelPlaneNumber ) {
//   	    //Int_t wire  = fTDCPtrs.at(mytdc).at(k).fElement;
//   	    fWireData.at ( plane ).at ( wire ).SetElectronics ( i,j,k );
//   	  }
//   	}
//       }
//     }
//   }
//   return OK;
// }


void  QwDriftChamberVDC::FillHistograms()
{

  //     if (! HasDataLoaded()) return;


  //     QwDetectorID   this_detid;
  //     QwDetectorInfo *this_det;

  //     //  Fill all of the histograms.


  //     std::vector<Int_t> wireshitperplane(fWiresPerPlane.size(),0);


  //     //for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {
  //     for (std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) {

  //         this_detid = hit1->GetDetectorID();
  //         //std::cout << "fElement during FillHistogram: " << this_detid.fElement << std::endl;
  //         if (this_detid.fPlane<=0 || this_detid.fElement<=0) {
  //             if (fDEBUG) {
  //                 std::cout << "QwDriftChamber::FillHistograms:  Bad plane or element index:  fPlane=="
  //                 << this_detid.fPlane << ", fElement==" << this_detid.fElement << std::endl;
  //             }
  //             continue;
  //         }


  //         this_det   = &(fWireData.at(this_detid.fPlane).at(this_detid.fElement));
  // //std::cout << "getnumberhits: " << this_det->GetNumHits() << std::endl;
  //         if (hit1->IsFirstDetectorHit()) {
  //             //  If this is the first hit for this detector, then let's plot the
  //             //  total number of hits this wire had.
  //             HitsWire[this_detid.fPlane]->Fill(this_detid.fElement,this_det->GetNumHits());

  //             //  Also increment the total number of events in whichthis wire was hit.
  //             TotHits[this_detid.fPlane]->Fill(this_detid.fElement,1);
  //             //  Increment the number of wires hit in this plane
  //             wireshitperplane.at(this_detid.fPlane) += 1;
  //         }

  //         //  Fill ToF histograms
  //         TOFP_raw[this_detid.fPlane]->Fill(hit1->GetRawTime());
  //         TOFW_raw[this_detid.fPlane]->Fill(this_detid.fElement,hit1->GetRawTime());
  //         TOFP[this_detid.fPlane]->Fill(hit1->GetTime());
  //         TOFW[this_detid.fPlane]->Fill(this_detid.fElement,hit1->GetTime());



  //     }

  //     for (size_t iplane=1; iplane<fWiresPerPlane.size(); iplane++) {
  //         WiresHit[iplane]->Fill(wireshitperplane[iplane]);

  //     }
};


//Int_t  QwDriftChamberVDC::LoadChannelMap(TString mapfile)
//{
//  return LoadMap(mapfile);
//}





Int_t QwDriftChamberVDC::LoadChannelMap ( TString mapfile ) 
{
  //some type(like string,Int_t)need to be changed to root type
  LoadTimeWireOffset("R3_timeoffset.txt");
  TString varname,varvalue;
  UInt_t value = 0;
  UInt_t channum;            //store temporary channel number
  UInt_t bpnum,lnnum;        //store temp backplane and line number
  UInt_t plnum,firstwire,LR;         //store temp package,plane,firstwire and left or right information
  TString pknum,dir;
  Bool_t IsFirstChannel = kTRUE;

  std::vector<Double_t> tmpWindows;
  QwParameterFile mapstr ( mapfile.Data() );

  UInt_t package = 0;
  UInt_t direction = 0;
  std::string string_a;
  std::pair<Double_t,Double_t> pair_a;

  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );
    mapstr.TrimWhitespace();
    if ( mapstr.LineIsEmpty() ) continue;

    if ( mapstr.HasVariablePair ( "=",varname,varvalue ) ) { //to judge whether we find a new slot
      varname.ToLower();
      value = QwParameterFile::GetUInt(varvalue);
      if (value ==0){
	value = atol(varvalue.Data());
      }
      if ( varname == "roc" ) {
	RegisterROCNumber ( value , 0);
      } else if (varname=="bank") {
	RegisterSubbank(value);
      } else if ( varname == "slot" ) {
	RegisterSlotNumber ( value );
      }
      continue;        //go to the next line
    }

    channum = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    bpnum   = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    lnnum   = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );

    if ( channum ==0 && bpnum ==0 ) {
      if ( IsFirstChannel == kTRUE ) IsFirstChannel = kFALSE;
      else                         continue;
    }

    if ( bpnum == kReferenceChannelPlaneNumber ) {
      LinkReferenceChannel ( channum, bpnum, lnnum );
      continue;
    }

    LR= ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    fDelayLinePtrs.at ( value ).push_back ( QwDelayLineID ( bpnum,lnnum,LR ) );    //the slot and channel number must be in order
    //pknum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    pknum=mapstr.GetNextToken ( ", \t()" ).c_str();
    plnum= ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    //dir=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    dir= mapstr.GetNextToken ( ", \t()" ).c_str();
    firstwire= ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );


    if     (pknum=="u") {package=kPackageUp;}
    else if(pknum=="v") {package=kPackageDown;}

    BuildWireDataStructure(channum,package,plnum,firstwire);

    if ( fDelayLineArray.at ( bpnum ).at ( lnnum ).Fill == kFALSE ) { //if this delay line has not been Filled in the data
      string_a = mapstr.GetNextToken ( ", \t()" ) ;
      while ( string_a.size() !=0 ) {
	tmpWindows.push_back ( atof ( string_a.c_str() ) );
	string_a = mapstr.GetNextToken ( ", \t()" );
      }

      fDelayLineArray.at(bpnum).at(lnnum).fPackage=(EQwDetectorPackage)package;
      fDelayLineArray.at ( bpnum ).at ( lnnum ).fPlane=plnum;

      if( dir == "u") direction=kDirectionU;
      else if(dir == "v") direction=kDirectionV;
      fDelayLineArray.at(bpnum).at(lnnum).fDirection=(EQwDirectionID)direction;
      //    if ( dir == "u" )
      //        fDelayLineArray.at ( bpnum ).at ( lnnum ).fDirection= kDirectionU;
      //    else if ( dir == "v" )
      //        fDelayLineArray.at ( bpnum ).at ( lnnum ).fDirection= kDirectionV;
      fDelayLineArray.at ( bpnum ).at ( lnnum ).fFirstWire=firstwire;
      for ( size_t i=0;i<tmpWindows.size() /2;i++ ) {
	pair_a.first  = tmpWindows.at ( 2*i );
	pair_a.second = tmpWindows.at ( 2*i+1 );
	fDelayLineArray.at ( bpnum ).at ( lnnum ).Windows.push_back ( pair_a );
      }

      //      std::cout << "DelayLine: back plane: " << bpnum
      //		<< "line number " << lnnum
      //		<< " Windows.size: "  << fDelayLineArray.at ( bpnum ).at ( lnnum ).Windows.size()
      //		<< std::endl;
      fDelayLineArray.at ( bpnum ).at ( lnnum ).Fill=kTRUE;
      tmpWindows.clear();
    }
  }
  return OK;
}



void QwDriftChamberVDC::ReadEvent ( TString& eventfile ) {
  TString varname,varvalue;
  UInt_t slotnum,channum;            //store temporary channel number
  //  UInt_t pknum,plnum;         //store temp package,plane,firstwire and left or right information
  UInt_t value = 0;
  Double_t signal = 0.0; // Double_t? unsigned Int_t ? by jhlee
  EQwDetectorPackage package = kPackageNull;
  EQwDirectionID direction = kDirectionNull;

  QwParameterFile mapstr ( eventfile.Data() );
  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );
    mapstr.TrimWhitespace();
    if ( mapstr.LineIsEmpty() ) continue;
    if ( mapstr.HasVariablePair ( "=",varname,varvalue ) ) { //to judge whether we find a new crate
      varname.ToLower();
      value = atol ( varvalue.Data() );
      continue;
    }

    slotnum = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    channum = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    signal  = ( atof ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
    //std::cout << "signal is: " << signal << endl;
    fTDCHits.push_back ( QwHit ( value,slotnum,channum,0, kRegionID3,package,0,direction,0, ( UInt_t ) signal ) );
  }        //only know TDC information and time value
}




void QwDriftChamberVDC::ProcessEvent() 
{
  if ( ! HasDataLoaded() ) return;
  //  Do the reference time subtration and subtration of time offsets.
  SubtractReferenceTimes();
  //LoadTimeWireOffset("R3_timeoffset.txt");

  Double_t real_time=0;
  Double_t tmpTime=0,left_time=0,right_time=0;
  Int_t tmpCrate=0,tmpModule=0,tmpChan=0,tmpbp=0,tmpln=0,plane=0,wire_hit=0,mycount=0;
  Bool_t kDir=kTRUE,tmpAM=kFALSE;
  std::vector<Int_t> wire_array;
  wire_array.clear();


  // processing the delay line starts....
  for ( std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++ ) {
    //this for loop will Fill in the tdc hits data Int_to the corresponding delay line
    QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
    tmpCrate=iter->GetSubbankID();
    tmpModule=tmpElectronicsID.fModule;
    tmpChan=tmpElectronicsID.fChannel;

    if(tmpCrate==3) tmpChan+=64;
    tmpTime=iter->GetTime();

    if ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fSide == 0 )
      fDelayLineArray.at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane ).at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber ).LeftHits.push_back ( tmpTime );
    else
      fDelayLineArray.at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane ).at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber ).RightHits.push_back ( tmpTime );
  }



  for ( std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++ ) {
    QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
    tmpCrate=iter->GetSubbankID();
    tmpTime= iter->GetTime();
    tmpModule = tmpElectronicsID.fModule;
    tmpChan   = tmpElectronicsID.fChannel;

    if(tmpCrate==3) tmpChan+=64;
    tmpbp    = fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane;
    tmpln    = fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber;
    plane =    fDelayLineArray.at ( tmpbp ).at ( tmpln ).fPlane;
    EQwDetectorPackage package = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fPackage;
    EQwDirectionID direction = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fDirection;


    if ( fDelayLineArray.at ( tmpbp ).at ( tmpln ).Processed == kFALSE ) { //if this delay line has been Processed

      if ( tmpbp==0 || tmpbp ==3 || tmpbp==4 || tmpbp==7 || tmpbp==8 || tmpbp==11 || tmpbp==12 || tmpbp==15)
	kDir=kTRUE;         //true means left-right
      else kDir=kFALSE;
      fDelayLineArray.at ( tmpbp ).at ( tmpln ).ProcessHits ( kDir );

      Int_t Wirecount=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.size();
      for ( Int_t i=0;i<Wirecount;i++ ) {
	Int_t Ambiguitycount=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.at ( i ).size();   //if there's a ambiguity, it's 2; if not, this is 1
	if ( Ambiguitycount==1 ) tmpAM=kFALSE;
	else tmpAM=kTRUE;
	Int_t order_L=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Hitscount.at ( i ).first;
	Int_t order_R=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Hitscount.at ( i ).second;
	left_time=fDelayLineArray.at ( tmpbp ).at ( tmpln ).LeftHits.at ( order_L );
	right_time=fDelayLineArray.at ( tmpbp ).at ( tmpln ).RightHits.at ( order_R );

	for ( Int_t j=0;j<Ambiguitycount;j++ ) {
	  real_time= ( left_time+right_time ) /2;
	  wire_hit=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.at ( i ).at ( j );
	  wire_array.push_back ( wire_hit );
	  mycount=count ( wire_array.begin(),wire_array.end(),wire_hit )-1;

	  Int_t temp_Chan=0;
	  if(tmpCrate==3) temp_Chan=tmpChan-64;
	  else {temp_Chan=tmpChan;}

	  QwHit NewQwHit ( tmpCrate, tmpModule, temp_Chan, mycount, kRegionID3,package, plane,direction,wire_hit );

	  NewQwHit.SetHitNumberR ( order_R );

	  NewQwHit.SetTime ( real_time );

	  //Int_t temp_plane=plane-1;
	  //if(tmpCrate==3) temp_plane=plane-5;

	  QwDetectorInfo* local_info = & fDetectorInfo.at ( package ).at ( plane-1 );
	  NewQwHit.SetDetectorInfo ( local_info );

	  NewQwHit.SetAmbiguityID ( tmpAM,j );
	  fWireHits.push_back ( NewQwHit );
	}
      }
    }
  }
  ApplyTimeCalibration();
  SubtractWireTimeOffset();
  CalculateDriftDistance();
};





void QwDriftChamberVDC::ClearEventData() 
{
  SetDataLoaded ( kFALSE );
  QwDetectorID this_det;
  for ( std::vector<QwHit>::iterator hit1=fTDCHits.begin();hit1!=fTDCHits.end();hit1++ ) {
    this_det = hit1->GetDetectorID();
  }
  
  fTDCHits.clear();
  fWireHits.clear();
  
  Int_t index = 0;
  Int_t j = 0;
  std::size_t i = 0;
  for ( i=0;i<fDelayLineArray.size();i++ ) {
    index = fDelayLineArray.at ( i ).size();

    for ( j=0;j<index;j++ ) {
      fDelayLineArray.at ( i ).at ( j ).Processed=kFALSE;
      fDelayLineArray.at ( i ).at ( j ).LeftHits.clear();
      fDelayLineArray.at ( i ).at ( j ).RightHits.clear();
      fDelayLineArray.at ( i ).at ( j ).Hitscount.clear();
      fDelayLineArray.at ( i ).at ( j ).Wire.clear();
    }
  }
  
  for ( i=0;i<fReferenceData.size(); i++ ) {
    fReferenceData.at ( i ).clear();
  }
  return;
}





Int_t QwDriftChamberVDC:: ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t index = 0;
  
  if (fDEBUG)  std::cout << "ProcessConfigurationBuffer" << std::endl;

  index = GetSubbankIndex(roc_id, bank_id);

  if (index>=0 and num_words>0) {
    //    SetDataLoaded(kTRUE);
    if (fDEBUG)  {
      std::cout << "QwDriftChamberVDC::ProcessConfigurationBuffer:  "
		<< "Begin processing ROC" << roc_id << std::endl;
      PrintConfigrationBuffer(buffer, num_words);
    }
  }
  
  return OK;
};



// Test function
void  QwDriftChamberVDC::PrintConfigrationBuffer(UInt_t *buffer, UInt_t num_words)
{
  UInt_t ipt = 0;
  UInt_t j = 0;
  UInt_t k = 0;

  for (j = 0; j < (num_words/5); j++) {
    printf("buffer[%5d] = 0x:", ipt);
    for (k=j; k<j+5; k++) {
      printf("%12x", buffer[ipt++]);
    }
    printf("\n");
  }

  if (ipt < num_words) {
    printf("buffer[%5d] = 0x:", ipt);
    for (k=ipt; k<num_words; k++) {
      printf("%12x", buffer[ipt++]);
    }
    printf("\n");
  }
  printf("\n");
  
  return;
}

void QwDriftChamberVDC::DefineOptions(QwOptions& options)
{
 options.AddOptions()("use-tdchit",
                          po::value<bool>()->zero_tokens()->default_value(false),
                          "creat tdc based tree");
}

void QwDriftChamberVDC::ProcessOptions(QwOptions& options)
{
 fUseTDCHits=options.GetValue<bool>("use-tdchit");
}
