/**********************************************************\
 * File: QwDriftChamberVDC.C                                *
 *                                                          *
 * Author: P. M. King , Rakitha Beminiwattha                *
 *         J. H. Lee                                        *
 * Time-stamp: Wednesday, July 28 15:52:19 EDT 2010         *
\**********************************************************/



#include "QwDriftChamberVDC.h"
#include "QwParameterFile.h"
#include "boost/bind.hpp"


const UInt_t QwDriftChamberVDC::kBackPlaneNum=16;
const UInt_t QwDriftChamberVDC::kLineNum=8;

// Register this subsystem with the factory
QwSubsystemFactory<QwDriftChamberVDC> theDriftChamberVDCFactory ( "QwDriftChamberVDC" );

QwDriftChamberVDC::QwDriftChamberVDC ( TString region_tmp ) :
        VQwSubsystem ( region_tmp ), QwDriftChamber ( region_tmp,fWireHitsVDC )
{
  std::vector<QwDelayLine> temp;
  temp.clear();
  temp.resize ( kLineNum );
  fDelayLineArray.resize ( kBackPlaneNum, temp );
  fDelayLinePtrs.resize ( 21 );
  OK=0;
};

Int_t QwDriftChamberVDC::LoadGeometryDefinition( TString mapfile )
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

  while ( mapstr.ReadNextLine() )
    {
      mapstr.TrimComment ( '!' );   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if ( mapstr.LineIsEmpty() )  continue;

      if ( mapstr.HasVariablePair ( "=",varname,varvalue ) )
        {
	  //  This is a declaration line.  Decode it.
	  varname.ToLower();
	  //UInt_t value = atol(varvalue.Data());
	  if ( varname=="name" )   //Beginning of detector information
            {
	      DIRMODE=1;
            }
        }
      else if ( DIRMODE==1 )
        {
	  //  Break this line Int_to tokens to process it.
	  varvalue     = ( mapstr.GetNextToken ( ", " ).c_str() );//this is the sType
	  Zpos         = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
	  rot          = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) * Qw::deg );
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

	  QwDebug << " VDC : Detector ID " << detectorId << " " << varvalue
		  << " Package "     << package << " Plane " << Zpos
		  << " Region "      << region << QwLog::endl;
	  


	  if (region==3) {
	    local_region3_detector.SetDetectorInfo (
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
	    if      ( package == "u" ) fDetectorInfo.at(kPackageUp).push_back(local_region3_detector);
	    else if ( package == "d" ) fDetectorInfo.at(kPackageDown).push_back(local_region3_detector);
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

  std::vector< QwDetectorInfo >::size_type i = 0;
  
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
 
  ReportConfiguration();

  return OK;
}






void  QwDriftChamberVDC::ReportConfiguration()
{
    UInt_t i,j,k;
    i=j=k=0;
    Int_t ind = 0;
    Int_t tdcindex = 0;
    for ( i=0; i<fROC_IDs.size(); i++ )
    {
        for ( j=0; j<fBank_IDs.at ( i ).size(); j++ )
        {
            ind = GetSubbankIndex ( fROC_IDs.at ( i ),fBank_IDs.at ( i ).at ( j ) );
            std::cout << "ROC " << fROC_IDs.at ( i )
            << ", subbank " << fBank_IDs.at ( i ).at ( j )
            << ":  subbank index==" << ind
            << std::endl;
            for ( k=0; k<kMaxNumberOfTDCsPerROC; k++ )
            {
                tdcindex = GetTDCIndex ( ind, k );
                std::cout << "    Slot " << k;
                if ( tdcindex == -1 )
                    std::cout << "  Empty" << std::endl;
                else
                    std::cout << "  TDC#" << tdcindex << std::endl;
            }
        }
    }
    // for ( i=0; i<fWiresPerPlane.size(); i++ ) {
    //   if ( fWiresPerPlane.at ( i ) == 0 ) continue;
    //   std::cout << "Plane " << i << " has " << fWireData.at ( i ).size()
    // 	      << " wires"
    // 	      <<std::endl;
    // }

    return;
};



void  QwDriftChamberVDC::SubtractReferenceTimes()
{
    std::size_t i = 0;
    std::vector<Double_t> reftimes;
    std::vector<Bool_t>   refchecked;
    std::vector<Bool_t>   refokay;
    Bool_t allrefsokay;

    std::size_t ref_size = 0;
    ref_size = fReferenceData.size();

    reftimes.resize ( ref_size );
    refchecked.resize ( ref_size );
    refokay.resize ( ref_size );

    for ( i=0; i< ref_size; i++ )
    {
        reftimes.at ( i )   = 0.0;
        refchecked.at ( i ) = kFALSE;
        refokay.at ( i )    = kFALSE;
    }

    allrefsokay = kTRUE;

    UInt_t bankid      = 0;
    Double_t raw_time  = 0.0;
    Double_t ref_time  = 0.0;
    Double_t time      = 0.0;
    // Double_t time2     = 0.0;
    // Double_t delta     = 0.0;
    Bool_t local_debug = false;

    for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ )
    {
        //  Only try to check the reference time for a bank if there is at least one
        //  non-reference hit in the bank.
        bankid = hit->GetSubbankID();

        //   if (bankid == 0) QwMessage << "BANK id" << bankid << QwLog::endl;
        //
        // if bankid == 0, print out bank id, and then what?
        //
        if ( !refchecked.at ( bankid ) )
        {

            if ( fReferenceData.at ( bankid ).empty() )
            {
                QwWarning << "QwDriftChamberVDC::SubtractReferenceTimes:  Subbank ID "
                << bankid << " is missing a reference time." << QwLog::endl;
                refokay.at ( bankid ) = kFALSE;
                allrefsokay        = kFALSE;
            }
            else
            {
                reftimes.at ( bankid ) = fReferenceData.at ( bankid ).at ( 0 );
                refokay.at ( bankid )  = kTRUE;
            }

            if ( refokay.at ( bankid ) )
            {
                for ( i=0; i<fReferenceData.at ( bankid ).size(); i++ )
                {
                    fReferenceData.at ( bankid ).at ( i ) -= reftimes.at ( bankid );
                }
            }
            refchecked.at ( bankid ) = kTRUE;
        }

        if ( refokay.at ( bankid ) )
        {
            raw_time = ( Double_t ) hit -> GetRawTime();
            ref_time = ( Double_t ) reftimes.at ( bankid );
            time     = QwDriftChamber::fF1TDC.ActualTimeDifference ( raw_time, ref_time );
            //      time2    = SubtractReference(raw_time, ref_time);
            //   delta    = fabs(time - time2);
            hit -> SetTime ( time );
            if ( local_debug )
            {
                QwMessage << " RawTime : " << raw_time
                << " RefTime : " << ref_time
                << " time    : " << time
                //		    << " time2   : " << time2
                //		    << " delta   : " << delta
                << std::endl;

            }
        }
    }

    bankid = 0;

    if ( ! allrefsokay )
    {
        std::vector<QwHit> tmp_hits;
        tmp_hits.clear();
        for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ )
        {
            bankid = hit->GetSubbankID();
            if ( refokay.at ( bankid ) )
            {
                tmp_hits.push_back ( *hit );
            }
        }
        // std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
        fTDCHits.clear();
        fTDCHits = tmp_hits;
        // std::cout << "FTDC size " << fTDCHits.size() << "tmp hit size " << tmp_hits.size() << std::endl;
    }

    return;
}


Double_t  QwDriftChamberVDC::CalculateDriftDistance ( Double_t drifttime, QwDetectorID detector )
{
    Double_t angle_degree = 45.0;
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

    time[0] = 0.5* ( p[0] + p[1]*cut[0] + p[2] + p[3]*cut[0] );
    time[1] = 0.5* ( p[2] + p[3]*cut[1] + p[4] + p[5]*cut[1] + p[6]*cut[1]*cut[1] );
    time[2] = 0.5* ( p[4] + p[5]*cut[2] + p[6]*cut[2]*cut[2] + p[7] + p[8]*cut[2] );
    time[3] = 0.5* ( p[7] + p[8]*cut[3] + p[9] +p[10]*cut[3] );

    if ( drifttime < time[0] )
    {
        distance_mm = ( drifttime - p[0] ) /p[1];
    }
    else if ( drifttime>=time[0] && drifttime<time[1] )
    {
        distance_mm = ( drifttime-p[2] ) /p[3];
    }
    else if ( drifttime>=time[1] && drifttime < time[2] )
    {
        distance_mm = ( -p[5] + sqrt ( p[5]*p[5] - 4.0*p[6]* ( p[4]-drifttime ) ) ) / ( 2.0*p[6] );
    }
    else if ( drifttime>=time[2] && drifttime<time[3] )
    {
        distance_mm = ( drifttime - p[7] ) /p[8];
    }
    else if ( drifttime>=time[3] )
    {
        distance_mm = ( drifttime - p[9] ) /p[10];
    }
    //if(drifttime>=4 && drifttime < 4.1)
    //std::cout << "drifttime: " << drifttime << " " << "x: " << x << std::endl;

    distance_cm = 0.1*distance_mm;
    return distance_cm;
};


void  QwDriftChamberVDC::FillRawTDCWord ( Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data )
{
  Int_t tdcindex = 0;
  tdcindex = GetTDCIndex ( bank_index,slot_num );
  
  if ( tdcindex not_eq -1 )
    {
      Int_t hitCount = 1;
      Int_t plane    = 0;
      Int_t wire     = 0;
      EQwDetectorPackage package = kPackageUp;
      EQwDirectionID direction   = kDirectionNull;
      
      plane = fTDCPtrs.at ( tdcindex ).at ( chan ).fPlane;
      wire  = fTDCPtrs.at ( tdcindex ).at ( chan ).fElement;
 
      if ( plane == -1 or wire == -1 )
        {
	  //  This channel is not connected to anything.  Do nothing.
        }
      else if ( plane == kReferenceChannelPlaneNumber )
        {
	  //std::cout << bank_index << " " << slot_num << " " << tdcindex << std::endl;
	  fReferenceData.at ( wire ).push_back ( data );
        }
      else
        {
	  plane=fDelayLinePtrs.at ( slot_num ).at ( chan ).fBackPlane;
	  //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_1"<<endl;
	  hitCount = std::count_if ( fTDCHits.begin(),fTDCHits.end(),boost::bind ( &QwHit::WireMatches,_1,3,boost::ref ( package ),boost::ref ( plane ),boost::ref ( wire ) ) );
	  
	  fTDCHits.push_back(
			     QwHit(
				   bank_index, 
				   slot_num, 
				   chan, 
				   hitCount, 
				   kRegionID2, 
				   package, 
				   plane,
				   direction, 
				   wire, 
				   data
				   )
			     );
	  //in order-> bank index, slot num, chan, hitcount, region=3, package, plane,,direction, wire,wire hit time

        }

    };
};



Int_t QwDriftChamberVDC::BuildWireDataStructure(const UInt_t chan, 
						const EQwDetectorPackage package, 
						const Int_t plane, 
						const Int_t wire)
{
  
  Int_t r3_wire_number_per_plane = 279;
  
  //  std::cout << "R3 chan " << chan
  //	    << " package "  << package
  //	    << " plane " << plane
  //	    << " wire " << wire
  //	    << std::endl;
  if ( plane == kReferenceChannelPlaneNumber )
    {
      LinkReferenceChannel ( chan, plane, wire );
    }
  else
    {
      fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPackage = package;
      fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fPlane   = plane;
      fTDCPtrs.at ( fCurrentTDCIndex ).at ( chan ).fElement = wire;

      Int_t real_size=4*(package-1)+plane;
      if ( real_size>= (Int_t) fWiresPerPlane.size() ) // plane is Int_t
        {
	  fWiresPerPlane.resize ( real_size );
        }
      //if (wire>=fWiresPerPlane.at(plane)){
      //fWiresPerPlane.at(plane) =  wire+1;
      //}
      fWiresPerPlane.at ( real_size-1 ) = r3_wire_number_per_plane;
    }
  return OK;
};


Int_t QwDriftChamberVDC::AddChannelDefinition()
{
    bool temp_local_debug = false;
//   if (temp_local_debug){
//     std::cout << " QwDriftChamberVDC::AddChannelDefinition"<<std::endl;
//     std::cout << "plane " << plane << " wire " << wire << std::endl;
//   }

    std::size_t i =0;

    fWireData.resize ( fWiresPerPlane.size() );
    for ( i=0; i<fWiresPerPlane.size(); i++ )
    {
        if ( temp_local_debug )
        {
            std::cout << "wire #" << i
            << " " << fWiresPerPlane.at ( i )
            << std::endl;
        }
        fWireData.at ( i ).resize ( fWiresPerPlane.at ( i ) );
    }

    Int_t mytdc = 0;
    for ( i=0; i<fTDC_Index.size(); i++ )
    {
        for ( size_t j=0; j<fTDC_Index.at ( i ).size(); j++ )
        {
            mytdc = fTDC_Index.at ( i ).at ( j );
            if ( mytdc not_eq -1 )
            {
                for ( size_t k=0; k<fTDCPtrs.at ( mytdc ).size(); k++ )
                {
                    Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
                    Int_t plane   = fTDCPtrs.at ( mytdc ).at ( k ).fPlane;
                    if ( ( plane>0 ) and ( plane not_eq ( Int_t ) kReferenceChannelPlaneNumber ) )
                    {
                        Int_t wire  = fTDCPtrs.at ( mytdc ).at ( k ).fElement;
                        Int_t window_number=0;
                        if ( wire<9 ) window_number=18;
                        else if ( wire==152 ) window_number=16;
                        else window_number=17;
                        for ( Int_t index=0;index<window_number;index++ )
                        {

                            fWireData.at ( 4*(package-1)+plane-1 ).at ( wire-1 ).SetElectronics ( i,j,k );
                            wire+=8;
                            if ( temp_local_debug )
                            {
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
    }
    std::cout << " QwDriftChamberVDC::AddChannelDefinition END"<<std::endl;
    return OK;
}


void  QwDriftChamberVDC::FillHistograms()
{

  Bool_t local_debug = true;
  if (not HasDataLoaded()) return;
  
  QwDetectorID   this_detid;
  QwDetectorInfo *this_det;

  std::vector<Int_t> wireshitperplane(fWiresPerPlane.size(),0);

  UInt_t raw_time = 0;
  Double_t time   = 0.0;

  Int_t plane = 0;
  Int_t element = 0;
  EQwDetectorPackage package = kPackageNull;

  Int_t wire_index = 0;
  
  for(std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++) {

     this_detid = hit->GetDetectorID();
     plane      = this_detid.fPlane;
     element    = this_detid.fElement;
     package    = this_detid.fPackage;
     
     if (plane<=0 or element<=0) {
       if (local_debug) {
	 QwMessage << "QwDriftChamberVDC::FillHistograms:  Bad plane or element index:"
		   << "  fPlane = "  << plane
		   << "  fElement= " << element
		   << QwLog::endl;
       }
       continue;
     }
     
     raw_time = hit->GetRawTime();
     time     = hit->GetTime();
     
     //  Fill ToF histograms
     TOFP_raw[plane]->Fill(raw_time);
     TOFW_raw[plane]->Fill(element, raw_time);
  }
    //  wire_index = 4*( (Int_t) package -1 ) + plane;
     
    //  this_det= &(fWireData.at(wire_index).at(element));
     
    // if (hit->IsFirstDetectorHit()) {
    //   HitsWire[wire_index]->Fill(element,this_det->GetNumHits());
    //   //  Also increment the total number of events in whichthis wire was hit.
    //   TotHits[wire_index]->Fill(element,1);
    //   //  Increment the number of wires hit in this plane
    //   wireshitperplane.at(wire_index) += 1;
    //   this_det->ClearHits();
    // }

  // // for (std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++) {
             
  // //   this_detid = hit1->GetDetectorID();

  // //   //std::cout << "fElement during FillHistogram: " << this_detid.fElement << std::endl;
  // //   if (this_detid.fPlane<=0 || this_detid.fElement<=0) {
  // //     if (fDEBUG) {
  // // 	std::cout << "QwDriftChamber::FillHistograms:  Bad plane or element index:  fPlane=="
  // // 		  << this_detid.fPlane << ", fElement==" << this_detid.fElement << std::endl;
  // //     }
  // //     continue;
  // //   }

  // //   Int_t index=4*(this_detid.fPackage-1)+this_detid.fPlane-1;
  // //   this_det   = &(fWireData.at(index).at(this_detid.fElement-1));
	     
	  
  // //   if (hit1->IsFirstDetectorHit()) {
  // //     //  If this is the first hit for this detector, then let's plot the
  // //     //  total number of hits this wire had.
  // //     //		 std::cout << "this_det->GetNumHits: " << this_det->GetNumHits() << std::endl;
  // //     HitsWire[index+1]->Fill(this_detid.fElement,this_det->GetNumHits());
                 
  // //     //  Also increment the total number of events in whichthis wire was hit.
  // //     TotHits[index+1]->Fill(this_detid.fElement,1);
  // //     //  Increment the number of wires hit in this plane
  // //     wireshitperplane.at(index) += 1;
  // //   }
  // //   //  Fill ToF histograms
  // //   //             TOFP_raw[index+1]->Fill(hit1->GetRawTime());
  // //   //             TOFW_raw[index+1]->Fill(this_detid.fElement,hit1->GetRawTime());
  // //   TOFP[index+1]->Fill(hit1->GetTime());
  // //   TOFW[index+1]->Fill(this_detid.fElement,hit1->GetTime());
  // //   this_det->ClearHits();
	      

  // }
 
  // for (size_t iplane=1; iplane<=fWiresPerPlane.size(); iplane++) {
  //   WiresHit[iplane]->Fill(wireshitperplane[iplane-1]);

  // }
};



Int_t QwDriftChamberVDC::LoadChannelMap ( TString mapfile )
{
  //some type(like string,Int_t)need to be changed to root type
  LoadTimeWireOffset ( "R3_timeoffset.txt" );
  TString varname,varvalue;
  UInt_t value   = 0;
  UInt_t channum = 0;            //store temporary channel number
  UInt_t lnnum   = 0;        //store line number
  Int_t  bpnum   = 0;    // temp backplane
  UInt_t plnum,firstwire,LR;         //store temp package,plane,firstwire and left or right information
  plnum = firstwire = LR = 0;

  TString pknum,dir;
  Bool_t IsFirstChannel = kTRUE;

  std::vector<Double_t> tmpWindows;
  QwParameterFile mapstr ( mapfile.Data() );

  EQwDetectorPackage package = kPackageNull;
  EQwDirectionID   direction = kDirectionNull;

  std::string string_a;
  std::pair<Double_t,Double_t> pair_a;

  while ( mapstr.ReadNextLine() )
    {
      mapstr.TrimComment ( '!' );
      mapstr.TrimWhitespace();
      if ( mapstr.LineIsEmpty() ) continue;

      if ( mapstr.HasVariablePair ( "=",varname,varvalue ) )   //to judge whether we find a new slot
        {
	  varname.ToLower();
	  value = QwParameterFile::GetUInt ( varvalue );
	  if ( value ==0 )
            {
	      value = atol ( varvalue.Data() );
            }
	  if ( varname == "roc" )
            {
	      RegisterROCNumber ( value , 0 );
            }
	  else if ( varname=="bank" )
            {
	      RegisterSubbank ( value );
            }
	  else if ( varname == "slot" )
            {
	      RegisterSlotNumber ( value );
            }
	  continue;        //go to the next line
        }

      channum = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
      bpnum   = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );
      lnnum   = ( atol ( mapstr.GetNextToken ( ", \t()" ).c_str() ) );

      if ( channum ==0 and bpnum ==0 )
        {
	  if ( IsFirstChannel == kTRUE ) IsFirstChannel = kFALSE;
	  else                         continue;
        }

      if ( bpnum == kReferenceChannelPlaneNumber )
        {
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


      if ( pknum=="u" )
        {
	  package=kPackageUp;
        }
      else if ( pknum=="v" )
        {
	  package=kPackageDown;
        }

      BuildWireDataStructure ( channum,package,plnum,firstwire );

      if ( fDelayLineArray.at ( bpnum ).at ( lnnum ).Fill == kFALSE )   //if this delay line has not been Filled in the data
        {
	  string_a = mapstr.GetNextToken ( ", \t()" ) ;
	  while ( string_a.size() !=0 )
            {
	      tmpWindows.push_back ( atof ( string_a.c_str() ) );
	      string_a = mapstr.GetNextToken ( ", \t()" );
            }

	  fDelayLineArray.at ( bpnum ).at ( lnnum ).fPackage=  package;
	  fDelayLineArray.at ( bpnum ).at ( lnnum ).fPlane=plnum;
	  if ( dir == "u" ) direction=kDirectionU;
	  else if ( dir == "v" ) direction=kDirectionV;
	  fDelayLineArray.at ( bpnum ).at ( lnnum ).fDirection= direction;
	  //    if ( dir == "u" )
	  //        fDelayLineArray.at ( bpnum ).at ( lnnum ).fDirection= kDirectionU;
	  //    else if ( dir == "v" )
	  //        fDelayLineArray.at ( bpnum ).at ( lnnum ).fDirection= kDirectionV;
	  fDelayLineArray.at ( bpnum ).at ( lnnum ).fFirstWire=firstwire;
	  for ( size_t i=0;i<tmpWindows.size() /2;i++ )
            {
	      pair_a.first  = tmpWindows.at ( 2*i );
	      pair_a.second = tmpWindows.at ( 2*i+1 );
	      fDelayLineArray.at ( bpnum ).at ( lnnum ).Windows.push_back ( pair_a );
            }

	  //            std::cout << "DelayLine: back plane: " << bpnum
	  //      		<< "line number " << lnnum
	  //      		<< " Windows.size: "  << fDelayLineArray.at ( bpnum ).at ( lnnum ).Windows.size()
	  //      		<< std::endl;
	  fDelayLineArray.at ( bpnum ).at ( lnnum ).Fill=kTRUE;
	  tmpWindows.clear();
        }
    }
  AddChannelDefinition();
  return OK;
}



void QwDriftChamberVDC::ReadEvent ( TString& eventfile )
{
    TString varname,varvalue;
    UInt_t slotnum,channum;            //store temporary channel number
    //  UInt_t pknum,plnum;         //store temp package,plane,firstwire and left or right information
    UInt_t value = 0;
    Double_t signal = 0.0; // Double_t? unsigned Int_t ? by jhlee
    EQwDetectorPackage package = kPackageNull;
    EQwDirectionID direction = kDirectionNull;

    QwParameterFile mapstr ( eventfile.Data() );
    while ( mapstr.ReadNextLine() )
    {
        mapstr.TrimComment ( '!' );
        mapstr.TrimWhitespace();
        if ( mapstr.LineIsEmpty() ) continue;
        if ( mapstr.HasVariablePair ( "=",varname,varvalue ) )   //to judge whether we find a new crate
        {
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


    Double_t real_time=0.0;
    Double_t tmpTime=0.0,left_time=0.0,right_time=0.0;
    Int_t tmpCrate=0,tmpModule=0,tmpChan=0,tmpbp=0,tmpln=0,plane=0,wire_hit=0,mycount=0;
    Bool_t kDir=kTRUE,tmpAM=kFALSE;
    std::vector<Int_t> wire_array;
    wire_array.clear();


    // processing the delay line starts....
    for ( std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++ )
    {
        //this for loop will Fill in the tdc hits data Int_to the corresponding delay line
        QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
        tmpCrate=iter->GetSubbankID();
        tmpModule=tmpElectronicsID.fModule;
        tmpChan=tmpElectronicsID.fChannel;

        if ( tmpCrate==3 ) tmpChan+=64;
        tmpTime=iter->GetTime();

        if ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fSide == 0 )
            fDelayLineArray.at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane ).at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber ).LeftHits.push_back ( tmpTime );
        else
            fDelayLineArray.at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane ).at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber ).RightHits.push_back ( tmpTime );
    }



    for ( std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++ )
    {
        QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
        tmpCrate=iter->GetSubbankID();
        tmpTime= iter->GetTime();
        tmpModule = tmpElectronicsID.fModule;
        tmpChan   = tmpElectronicsID.fChannel;

        if ( tmpCrate==3 ) tmpChan+=64;
        tmpbp    = fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane;
        tmpln    = fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber;
        plane =    fDelayLineArray.at ( tmpbp ).at ( tmpln ).fPlane;
        EQwDetectorPackage package = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fPackage;
        EQwDirectionID direction = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fDirection;

        if ( fDelayLineArray.at ( tmpbp ).at ( tmpln ).Processed == kFALSE )   //if this delay line has been Processed
        {

            if ( tmpbp==0 || tmpbp ==3 || tmpbp==4 || tmpbp==7 || tmpbp==8 || tmpbp==11 || tmpbp==12 || tmpbp==15 )
                kDir=kTRUE;         //true means left-right
            else kDir=kFALSE;
            fDelayLineArray.at ( tmpbp ).at ( tmpln ).ProcessHits ( kDir );

            Int_t Wirecount=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.size();
            for ( Int_t i=0;i<Wirecount;i++ )
            {
                Int_t Ambiguitycount=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.at ( i ).size();   //if there's a ambiguity, it's 2; if not, this is 1
                if ( Ambiguitycount==1 ) tmpAM=kFALSE;
                else tmpAM=kTRUE;
                Int_t order_L=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Hitscount.at ( i ).first;
                Int_t order_R=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Hitscount.at ( i ).second;
                left_time=fDelayLineArray.at ( tmpbp ).at ( tmpln ).LeftHits.at ( order_L );
                right_time=fDelayLineArray.at ( tmpbp ).at ( tmpln ).RightHits.at ( order_R );

                for ( Int_t j=0;j<Ambiguitycount;j++ )
                {
                    real_time= ( left_time+right_time ) /2;
                    wire_hit=fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.at ( i ).at ( j );
                    wire_array.push_back ( wire_hit );
                    mycount=count ( wire_array.begin(),wire_array.end(),wire_hit )-1;

                    Int_t temp_Chan=0;
                    if ( tmpCrate==3 ) temp_Chan=tmpChan-64;
                    else
                    {
                        temp_Chan=tmpChan;
                    }

                    QwHit NewQwHit ( tmpCrate, tmpModule, temp_Chan, mycount, kRegionID3, package, plane, direction, wire_hit );

                    NewQwHit.SetHitNumberR ( order_R );

                    NewQwHit.SetTime ( real_time );


                    QwDetectorInfo* local_info = & fDetectorInfo.at ( package ).at ( plane-1 );
                    NewQwHit.SetDetectorInfo ( local_info );
		    fWireData.at(4*(package-1)+plane-1).at(wire_hit-1).PushHit((Int_t) real_time); 

                    NewQwHit.SetAmbiguityID ( tmpAM,j );
                    fWireHits.push_back ( NewQwHit );
                }
            }
        }
    }


    ApplyTimeCalibration();


    if ( fDisableWireTimeOffset==false )
        SubtractWireTimeOffset();
    else {};

    FillDriftDistanceToHits();
};





void QwDriftChamberVDC::ClearEventData()
{
    SetDataLoaded ( kFALSE );
    QwDetectorID this_det;
    for ( std::vector<QwHit>::iterator hit1=fTDCHits.begin();hit1!=fTDCHits.end();hit1++ )
    {
        this_det = hit1->GetDetectorID();
    }

    fTDCHits.clear();
    fWireHits.clear();

    Int_t index = 0;
    Int_t j = 0;
    std::size_t i = 0;
    for ( i=0;i<fDelayLineArray.size();i++ )
    {
        index = fDelayLineArray.at ( i ).size();

        for ( j=0;j<index;j++ )
        {
            fDelayLineArray.at ( i ).at ( j ).Processed=kFALSE;
            fDelayLineArray.at ( i ).at ( j ).LeftHits.clear();
            fDelayLineArray.at ( i ).at ( j ).RightHits.clear();
            fDelayLineArray.at ( i ).at ( j ).Hitscount.clear();
            fDelayLineArray.at ( i ).at ( j ).Wire.clear();
        }
    }

    for ( i=0;i<fReferenceData.size(); i++ )
    {
        fReferenceData.at ( i ).clear();
    }
    return;
}





Int_t QwDriftChamberVDC::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t subbank_index = 0;
  Bool_t local_debug = false;
  
  subbank_index = GetSubbankIndex(roc_id, bank_id);
  if ( local_debug ) {
    std::cout << "QwDriftChamberVDC::ProcessConfigurationBuffer" << std::endl;
    std::cout << " roc id " << roc_id
	      << " bank_id " << bank_id
	      << " subbank_index " << subbank_index
	      << " num_words " << num_words
	      << std::endl;
  }
  
  if (subbank_index>=0 and num_words>0) {
    //    SetDataLoaded(kTRUE);
    if (local_debug) {
      std::cout << "QwDriftChamberVDC::ProcessConfigurationBuffer:  "
		<< "Begin processing ROC" << roc_id << std::endl;
      PrintConfigrationBuffer(buffer,num_words);
    }
  }
  
  return OK;
};



void QwDriftChamberVDC::PrintConfigrationBuffer(UInt_t *buffer,UInt_t num_words)
{
  UInt_t ipt = 0;
  UInt_t j = 0;
  UInt_t k = 0;
  
  for ( j=0; j<(num_words/5); j++ ) {
    printf ( "buffer[%5d] = 0x:", ipt );
    for ( k=j; k<j+5; k++ ) {
      printf ( "%12x", buffer[ipt++] );
    }
    printf ( "\n" );
  }
  
  if ( ipt<num_words ) {
    printf ( "buffer[%5d] = 0x:", ipt );
    for ( k=ipt; k<num_words; k++ ) {
      printf ( "%12x", buffer[ipt++] );
    }
    printf ( "\n" );
  }
  printf ( "\n" );
  
  return;
}

void QwDriftChamberVDC::DefineOptions ( QwOptions& options )
{
    options.AddOptions() ( "use-tdchit",
                           po::value<bool>()->zero_tokens()->default_value ( false ),
                           "creat tdc based tree" );
    options.AddOptions() ( "disable-wireoffset",
                           po::value<bool>()->zero_tokens()->default_value ( false ),
                           "disable the ablitity of subtracting t0 for every wire" );
}

void QwDriftChamberVDC::ProcessOptions ( QwOptions& options )
{
    fUseTDCHits=options.GetValue<bool> ( "use-tdchit" );
    fDisableWireTimeOffset=options.GetValue<bool> ( "disable-wireoffset" );
}
