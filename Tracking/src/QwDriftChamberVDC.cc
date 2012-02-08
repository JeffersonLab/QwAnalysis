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
#include <algorithm>


const UInt_t QwDriftChamberVDC::kBackPlaneNum=16;
const UInt_t QwDriftChamberVDC::kLineNum=8;

// Register this subsystem with the factory
RegisterSubsystemFactory(QwDriftChamberVDC);
bool invalid(QwHit& hit){
  return hit.GetDriftDistance() < 0;
}

QwDriftChamberVDC::QwDriftChamberVDC ( TString region_tmp ) :
  VQwSubsystem ( region_tmp ), QwDriftChamber ( region_tmp,fWireHitsVDC )
{
  std::vector<QwDelayLine> temp;
  temp.clear();
  temp.resize ( kLineNum );
  fDelayLineArray.resize ( kBackPlaneNum, temp );
  fDelayLinePtrs.resize ( 21 );
  OK=0;
}

Int_t QwDriftChamberVDC::LoadGeometryDefinition ( TString mapfile )
{
  std::cout<<"Region 3 Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  Int_t    plane, TotalWires, detectorId, region, DIRMODE;
  Double_t Zpos=0.0, rot, sp_res, track_res, slope_match;
  Double_t Det_originX, Det_originY;
  Double_t ActiveWidthX, ActiveWidthY, ActiveWidthZ;
  Double_t WireSpace, FirstWire, W_rcos, W_rsin, tilt;
  Double_t ZPOS [5],XPOS [5],YPOS [5], TILT [5], ROT [5];

  fDetectorInfo.clear();

  DIRMODE=0;

  QwParameterFile mapstr ( mapfile.Data() );  //Open the file
  fDetectorMapsNames.push_back(mapstr.GetParamFilename());
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
	  varvalue     = mapstr.GetTypedNextToken<TString>();//this is the sType
	  if (varvalue != "drift") continue;
	  for (Int_t i=0;i<5;i++)
	    ZPOS[i]      = mapstr.GetTypedNextToken<Double_t>();			
	  for (Int_t i=0;i<5;i++)
	    ROT[i]      = mapstr.GetTypedNextToken<Double_t>() * Qw::deg;
	  sp_res       = mapstr.GetTypedNextToken<Double_t>();
	  track_res    = mapstr.GetTypedNextToken<Double_t>();
	  slope_match  = mapstr.GetTypedNextToken<Double_t>();
	  package      = mapstr.GetTypedNextToken<TString>();
	  region       = mapstr.GetTypedNextToken<Int_t>();
	  if (region != 3) continue;

	  dType        = mapstr.GetTypedNextToken<TString>();
	  direction    = mapstr.GetTypedNextToken<TString>();
	  for (Int_t i=0;i<5;i++)
	    XPOS[i]      = mapstr.GetTypedNextToken<Double_t>();
	  for (Int_t i=0;i<5;i++)
	    YPOS[i]      = mapstr.GetTypedNextToken<Double_t>();
	  ActiveWidthX = mapstr.GetTypedNextToken<Double_t>();
	  ActiveWidthY = mapstr.GetTypedNextToken<Double_t>();
	  ActiveWidthZ = mapstr.GetTypedNextToken<Double_t>();
	  WireSpace    = mapstr.GetTypedNextToken<Double_t>();
	  FirstWire    = mapstr.GetTypedNextToken<Double_t>();
	  W_rcos       = mapstr.GetTypedNextToken<Double_t>();
	  W_rsin       = mapstr.GetTypedNextToken<Double_t>();
	  for (Int_t i=0;i<5;i++)
	    TILT[i]      = mapstr.GetTypedNextToken<Double_t>() * Qw::deg;
	  TotalWires   = mapstr.GetTypedNextToken<Int_t>();
	  detectorId   = mapstr.GetTypedNextToken<Int_t>();


			
	  // Octant R3 is in front of, is the only value used
	  fR3Octant=gQwOptions.GetValue<Int_t> ("R3-octant");
          Int_t oct=fR3Octant;
	  if (oct == 8) oct=4;
	  if (oct == 7) oct=5;
	  Zpos = ZPOS[oct-1];
	  Det_originX = XPOS[oct-1];
	  Det_originY = YPOS[oct-1];
	  rot = ROT[oct-1];
	  tilt = TILT[oct-1];
			
	  QwDebug << " VDC : Detector ID " << detectorId << " " << varvalue
		  << " Package "     << package << " Plane " << Zpos
		  << " Region "      << region << QwLog::endl;
	  std::cout << " Package " << package 
	    //			<< " Plane " << plane 
			<< " Region " << region
			<< " R3-octant " << fR3Octant
			<< " oct       " << oct
			<< " Zpos      " << Zpos
			<< " Det_origin [x,y] " << Det_originX << " , " << Det_originY
			<< " Rot " << rot 
			<< " Tilt " << tilt
			<< " Detector ID " << detectorId
			<< std::endl;

	  if ( region==3 )
	    {
	      
	 
	      
	      QwDetectorInfo* detector = new QwDetectorInfo();
	      detector->SetDetectorInfo(dType, Zpos,
					rot, sp_res, track_res, slope_match,
					package,region, direction,
					Det_originX, Det_originY,
					ActiveWidthX, ActiveWidthY, ActiveWidthZ,
					WireSpace, FirstWire,
					W_rcos, W_rsin, tilt,
					TotalWires,
					detectorId);
	      if(package=="u")
		detector->SetOctantNumber((fR3Octant+4)%8);
	      else
		detector->SetOctantNumber(fR3Octant);
	      fDetectorInfo.push_back(detector);
	    }
	}
    }

  QwMessage << "Loaded Qweak Geometry" << " Total Detectors in kPackageUP "
	    << fDetectorInfo.in(kPackageUp).size()
	    << ", "
	    << "kPackagDown "
	    << fDetectorInfo.in(kPackageDown).size()
	    << QwLog::endl;

  QwMessage << "Sorting detector info..." << QwLog::endl;

  plane = 1;
  QwGeometry detector_info_up = fDetectorInfo.in(kPackageUp);
  for (size_t i = 0; i < detector_info_up.size(); i++)
    {
      detector_info_up.at(i)->fPlane = plane++;
      QwMessage << " kPackageUp Region " << detector_info_up.at(i)->fRegion
		<< " Detector ID " << detector_info_up.at(i)->fDetectorID
		<< QwLog::endl;
    }

  plane = 1;
  QwGeometry detector_info_down = fDetectorInfo.in(kPackageDown);
  for (size_t i = 0; i < detector_info_down.size(); i++)
    {
      detector_info_down.at(i)->fPlane = plane++;
      QwMessage << " kPackageDown Region " << detector_info_down.at(i)->fRegion
		<< " Detector ID " << detector_info_down.at(i)->fDetectorID
		<< QwLog::endl;
    }

  mapstr.Close(); // Close the file (ifstream)

  QwMessage << "Qweak Geometry Loaded " << QwLog::endl;

  //  ReportConfiguration();

  return OK;
}



void  QwDriftChamberVDC::SubtractReferenceTimes()
{
  std::vector<Double_t> reftimes;
  std::vector<Bool_t>   refchecked;
  std::vector<Bool_t>   refokay;
  Bool_t allrefsokay;

  std::size_t ref_size = 0;
  std::size_t i = 0;
  std::size_t j = 0;

  ref_size = fReferenceData.size();

  reftimes.resize  ( ref_size );
  refchecked.resize( ref_size );
  refokay.resize   ( ref_size );

  for ( i=0; i<ref_size; i++ )
    {
      reftimes.at  (i) = 0.0;
      refchecked.at(i) = kFALSE;
      refokay.at   (i) = kFALSE;
    }

  allrefsokay = kTRUE;

  UInt_t bankid      = 0;
  Double_t raw_time_arb_unit = 0.0;
  Double_t ref_time_arb_unit = 0.0;
  Double_t time_arb_unit     = 0.0;

  // Double_t raw_time  = 0.0;
  // Double_t ref_time  = 0.0;
  // Double_t time      = 0.0;
  Bool_t local_debug = false;

  std::size_t ref_data_size = 0;
  std::size_t bank = 0;
  Double_t diff_between_refs = 0.0;

  //test if the reference time is ok
  
  for ( bank=0; bank< ref_size; bank++ )
    {
      if(fReferenceMaster.size()==0) continue;

      ref_data_size = fReferenceData.at(bank).size();
      for ( i=0; i<ref_data_size; ++i )
	{
	  if(fReferenceMaster.at(bank).size()==0) continue;
	  
	  diff_between_refs = fReferenceData.at ( bank ).at ( i )  -  fReferenceMaster.at ( bank ).at(0);
	  // 			std::cout << fReferenceData.at ( bank ).at ( i ) << " " << fReferenceMaster.at ( bank ).at(0) << std::endl;
	  // 			std::cout << "diff: " << diff << std::endl;
	  if ( diff_between_refs> -400.0 || diff_between_refs < -940.0 ) {       
	    fReferenceData.at ( bank ).erase ( fReferenceData.at(bank).begin() +i );
	    i--;
	  }
	  else{
	  }
	  ref_data_size = fReferenceData.at ( bank ).size();
	}
    }
  

  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ )
    {
      //  Only try to check the reference time for a bank if there is at least one
      //  non-reference hit in the bank.
      bankid = hit->GetSubbankID();
   
      if ( not refchecked.at ( bankid ) )
	{

	  if ( fReferenceData.at ( bankid ).empty() )
	    {
	      if(local_debug) {
		QwWarning << "QwDriftChamberVDC::SubtractReferenceTimes:  Subbank ID "
			  << bankid << " is missing a reference time." << QwLog::endl;
	      }
	      refokay.at ( bankid ) = kFALSE;
	      allrefsokay           = kFALSE;
	    }
	  else
	    {
	      reftimes.at ( bankid ) = fReferenceData.at ( bankid ).at ( fReferenceData.at ( bankid ).size()-1 );
	      refokay.at ( bankid ) = kTRUE;
	    }

	  if ( refokay.at ( bankid ) )
	    {
	      for ( j=0; j<fReferenceData.at ( bankid ).size(); j++ )
		{
		  fReferenceData.at ( bankid ).at ( j ) -= reftimes.at ( bankid );
		}
	    }
	  refchecked.at ( bankid ) = kTRUE;
	}

      if ( refokay.at ( bankid ) )
	{
	  Int_t slot_num    = hit -> GetModule();
	  raw_time_arb_unit = (Double_t) hit -> GetRawTime();
	  ref_time_arb_unit = (Double_t) reftimes.at(bankid);
	  // raw_time = ( Double_t ) hit -> GetRawTime();
	  // ref_time = ( Double_t ) reftimes.at ( bankid );
	  // Int_t bank_index = hit->GetSubbankID();
	  // Int_t slot_num   = hit->GetModule();
	  
	  time_arb_unit = fF1TDContainer->ReferenceSignalCorrection(raw_time_arb_unit, ref_time_arb_unit, bankid, slot_num);
	  
	  hit -> SetTime(time_arb_unit); 
	  hit -> SetRawRefTime((UInt_t) ref_time_arb_unit);
	  // time = fF1TDContainer->ReferenceSignalCorrection ( raw_time, ref_time, bankid, slot_num );
	  // hit -> SetTime ( time );
	  
	  if(local_debug) {
	    QwMessage << this->GetSubsystemName()
		      << " BankIndex " << std::setw(2) << bankid
		      << " Slot "      << std::setw(2) << slot_num
		      << " RawTime : " << std::setw(6) << raw_time_arb_unit
		      << " RefTime : " << std::setw(6) << ref_time_arb_unit
		      << " time : "    << std::setw(6) << time_arb_unit
		      << std::endl;
	    
	  }


	}
    }

  bankid = 0;

  if ( not allrefsokay )
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
  //    Double_t angle_degree = 45.0;
  Double_t distance_mm = 0.0;
  Double_t distance_cm = 0.0;
  Double_t dt=drifttime;
  Double_t resolution=0.5;     //resolution is 0.5ns
  //     Double_t cut0=0,cut1=0,cut2=0;

  // 	if ( fTtoDNumbers.size() <15 )
  // 	{
  // 		std::cerr << "the size of parameters is not correct, please check! " << std::endl;
  // 		return -1;
  // 	}

  //     cut0=fTtoDNumbers.at(0);
  //     cut1=fTtoDNumbers.at(1);
  //     cut2=fTtoDNumbers.at(2);


  Int_t index = (Int_t) (dt/resolution);

 
  if(dt>=0 && dt<310){
    distance_mm= ( dt-resolution*index ) /resolution * ( fTtoDNumbers.at ( index+1 )-fTtoDNumbers.at ( index ) ) +fTtoDNumbers.at ( index );}
  else{
    distance_mm=-50;
  }
  // 	}
  //     if ( dt < cut0 )
  //       {
  //         distance_mm = fTtoDNumbers.at(3)+fTtoDNumbers.at(4)*dt+fTtoDNumbers.at(5)*dt*dt+fTtoDNumbers.at(6)*dt*dt*dt/1000000;
  //       }
  //     else if ( dt>=cut0 && dt<cut1 )
  //       {
  //         distance_mm = fTtoDNumbers.at(7)+fTtoDNumbers.at(8)*dt;
  //       }
  //     else if ( dt>=cut1 && dt < cut2 )
  //       {
  //         distance_mm = fTtoDNumbers.at(9)+fTtoDNumbers.at(10)*dt+fTtoDNumbers.at(11)*dt*dt+fTtoDNumbers.at(12)*dt*dt*dt/1000000;
  //       }
  //     else if ( dt>=cut2 && dt <=400 )
  //       {
  //         distance_mm = fTtoDNumbers.at(13)+fTtoDNumbers.at(14)*dt;
  //       }
  //     else { distance_mm = -50;}

  //     Double_t p[12]={0.0};
  //     Double_t cut_time[3]={60,236,300};
  //
  //     p[0]=-0.0217483;
  //     p[1]=0.0671134;
  //     p[2]=0.000371857;
  //     p[3]=-6.10097;
  //
  //     p[4]=0.797553;
  //     p[5]=0.0538063;
  //
  //     p[6]=-39.0646;
  //     p[7]=0.449894;
  //     p[8]=-0.00119686;
  //     p[9]=1.00028;
  //
  //     p[10]=14.7995;
  //     p[11]=0.00100113;
  //
  //
  //     if ( dt < cut_time[0] )
  //       {
  //         distance_mm = p[0]+p[1]*dt+p[2]*dt*dt+p[3]*dt*dt*dt/1000000;
  //       }
  //     else if ( dt>=cut_time[0] && dt<cut_time[1] )
  //       {
  //         distance_mm = p[4]+p[5]*dt;
  //       }
  //     else if ( dt>=cut_time[1] && dt < cut_time[2] )
  //       {
  //         distance_mm = p[6]+p[7]*dt+p[8]*dt*dt+p[9]*dt*dt*dt/1000000;
  //       }
  //     else if ( dt>=cut_time[2] )
  //       {
  //         distance_mm = p[10]+p[11]*dt;
  //       }
  //     else {}


  //Double_t p[11]={0.0};

  //for arg-50-ethane-50
  //p[ 0] = -1.749 + 0.021670*angle_degree; // [T], then what unit of 0.021670 is?
  // [T]/[DEGREE]
  // jhlee, confirmed by siyuan
  // Thursday, July  8 10:50:23 EDT 2010
  //p[ 1] = 24.100 - 0.305100*angle_degree; // [T]/[L]   0.305100 [T]/([L].[DEGREE])
  //p[ 2] = -3.133 + 0.009078*angle_degree; // [T]
  //p[ 3] = 28.510 - 0.295100*angle_degree; // [T]/[L]
  //p[ 4] =  7.869 - 0.163500*angle_degree; // [T]
  //p[ 5] = 17.690 - 0.120800*angle_degree; // [T]/[L]
  //p[ 6] =  2.495 - 0.041040*angle_degree; // [T]/[L2]
  //p[ 7] =  3.359 - 0.478000*angle_degree; // [T]
  //p[ 8] = 29.350 - 0.219500*angle_degree; // [T]/[L]
  //p[ 9] = 55.680 - 1.730000*angle_degree; // [T]
  //p[10] = 20.380 - 0.004761*angle_degree; // [T]/[L]

  //Double_t cut[4]  = {0.5, 1.5, 4.0, 6.0};// [L] // mm
  //Double_t time[4] = {0.0};// [T]

  //time[0] = 0.5* ( p[0] + p[1]*cut[0] + p[2] + p[3]*cut[0] );
  //time[1] = 0.5* ( p[2] + p[3]*cut[1] + p[4] + p[5]*cut[1] + p[6]*cut[1]*cut[1] );
  //time[2] = 0.5* ( p[4] + p[5]*cut[2] + p[6]*cut[2]*cut[2] + p[7] + p[8]*cut[2] );
  //time[3] = 0.5* ( p[7] + p[8]*cut[3] + p[9] +p[10]*cut[3] );

  /*if ( drifttime < time[0] )
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
    }*/
  //if(drifttime>=4 && drifttime < 4.1)
  //std::cout << "drifttime: " << drifttime << " " << "x: " << x << std::endl;

  distance_cm = 0.1*distance_mm;
  return distance_cm;
}


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
	  //fReferenceData.at ( wire ).push_back ( data );
	  fReferenceData.at ( wire ).push_back ( data );
	}
      else if ( plane == kCodaMasterPlaneNumber )
	{
			
	  fReferenceMaster.at ( wire ).push_back ( data );
	}
      else
	{
	  plane=fDelayLinePtrs.at ( slot_num ).at ( chan ).fBackPlane;
	  //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_1"<<endl;
	  hitCount = std::count_if ( fTDCHits.begin(), fTDCHits.end(),
				     boost::bind (
						  &QwHit::WireMatches,_1, kRegionID3, boost::ref ( package ), boost::ref ( plane ), boost::ref ( wire )
						  )
				     );

	  fTDCHits.push_back (
			      QwHit (
				     bank_index,
				     slot_num,
				     chan,
				     hitCount,
				     kRegionID3,
				     package,
				     plane,
				     direction,
				     wire,
				     data
				     )
			      );
	  //in order-> bank index, slot num, chan, hitcount, region=3, package, plane,,direction, wire,wire hit time

	}

    }
}



Int_t QwDriftChamberVDC::BuildWireDataStructure ( const UInt_t chan,
						  const EQwDetectorPackage package,
						  const Int_t plane,
						  const Int_t wire )
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

      Int_t real_size=4* ( ( Int_t ) package-1 ) +plane;
      if ( real_size>= ( Int_t ) fWiresPerPlane.size() ) // plane is Int_t
	{
	  fWiresPerPlane.resize ( real_size + 1 );
	  // size() is one more larger than last plane number
	  // For VDC, plane    1,2,....,8
	  //          vector 0,1,2,....,8
	  //          thus, vector.size() returns 9
	  // So the magic number "1" is.
	  // Wednesday, July 28 21:58:13 EDT 2010, jhlee
	}
      fWiresPerPlane.at ( real_size ) = r3_wire_number_per_plane;
    }
  return OK;
}


Int_t QwDriftChamberVDC::AddChannelDefinition()
{
  bool temp_local_debug = false;
  //   if (temp_local_debug){
  //     std::cout << " QwDriftChamberVDC::AddChannelDefinition"<<std::endl;
  //     std::cout << "plane " << plane << " wire " << wire << std::endl;
  //   }

  std::size_t i =0;

  fWireData.resize ( fWiresPerPlane.size() );
  for ( i=1; i<fWiresPerPlane.size(); i++ )
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
		  Int_t package = fTDCPtrs.at ( mytdc ).at ( k ).fPackage;
		  Int_t plane   = fTDCPtrs.at ( mytdc ).at ( k ).fPlane;
		  if ( ( plane>0 ) and ( plane not_eq ( Int_t ) kReferenceChannelPlaneNumber ) and ( plane not_eq ( Int_t ) kCodaMasterPlaneNumber) )
		    {
		      Int_t wire  = fTDCPtrs.at ( mytdc ).at ( k ).fElement;
		      Int_t window_number=0;
		      if ( wire<9 ) window_number=18;
		      else if ( wire==152 ) window_number=16;
		      else window_number=17;
		      for ( Int_t index=0;index<window_number;index++ )
			{

			  fWireData.at ( 4* ( package-1 ) +plane ).at ( wire-1 ).SetElectronics ( i,j,k );
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



Int_t QwDriftChamberVDC::LoadChannelMap ( TString mapfile )
{
  //some type(like string,Int_t)need to be changed to root type
  LoadTimeWireOffset ( "R3_timeoffset.map" );
  LoadTtoDParameters ( "R3_TtoDTable.map" );
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
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
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

      channum = mapstr.GetTypedNextToken<Int_t>();
      bpnum   = mapstr.GetTypedNextToken<Int_t>();
      lnnum   = mapstr.GetTypedNextToken<Int_t>();

      if ( channum ==0 and bpnum ==0 )
	{
	  if ( IsFirstChannel == kTRUE ) IsFirstChannel = kFALSE;
	  else                         continue;
	}

      if ( bpnum == kReferenceChannelPlaneNumber || bpnum == kCodaMasterPlaneNumber )
	{
	  LinkReferenceChannel ( channum, bpnum, lnnum );
	  continue;
	}

      LR= mapstr.GetTypedNextToken<Int_t>();
      fDelayLinePtrs.at ( value ).push_back ( QwDelayLineID ( bpnum,lnnum,LR ) );    //the slot and channel number must be in order
      //pknum=(atol(mapstr.GetTypedNextToken<TString>()));
      pknum = mapstr.GetTypedNextToken<TString>();
      plnum = mapstr.GetTypedNextToken<Int_t>();
      //dir=(atol(mapstr.GetTypedNextToken<TString>()));
      dir= mapstr.GetTypedNextToken<TString>();
      firstwire= mapstr.GetTypedNextToken<Int_t>();


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
	  string_a = mapstr.GetTypedNextToken<std::string>();
	  while ( string_a.size() !=0 )
	    {
	      tmpWindows.push_back ( atof ( string_a.c_str() ) );
	      string_a = mapstr.GetTypedNextToken<std::string>();
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

  mapstr.Close(); // Close the file (ifstream)
  return OK;
}



void QwDriftChamberVDC::ReadEvent ( TString& eventfile )
{
  TString varname,varvalue;
  UInt_t slotnum = 0;
  UInt_t channum = 0;            //store temporary channel number
  //  UInt_t pknum,plnum;         //store temp package,plane,firstwire and left or right information
  UInt_t value = 0;
  Double_t signal = 0.0; // Double_t? unsigned Int_t ? by jhlee
  EQwDetectorPackage package = kPackageNull;
  EQwDirectionID direction = kDirectionNull;
  
  QwParameterFile mapstr ( eventfile.Data() );
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

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
      
      slotnum = mapstr.GetTypedNextToken<Int_t>();
      channum = mapstr.GetTypedNextToken<Int_t>();
      signal  = mapstr.GetTypedNextToken<Double_t>();
      //std::cout << "signal is: " << signal << endl;
      fTDCHits.push_back ( QwHit ( value,slotnum,channum,0, kRegionID3,package,0,direction,0, ( UInt_t ) signal ) );
    }        //only know TDC information and time value

  mapstr.Close(); // Close the file (ifstream)
  return;
}




void QwDriftChamberVDC::ProcessEvent()
{
  if ( not HasDataLoaded() ) return;

  SubtractReferenceTimes();  // A.U. unit
  UpdateHits();
  
  if ( fDisableWireTimeOffset == false ){
    
    SubtractWireTimeOffset();
    FillDriftDistanceToHits();
    
    std::vector<QwHit>::iterator it;
    it=std::remove_if(fWireHits.begin(),fWireHits.end(),invalid);
    if(it!=fWireHits.end())
      fWireHits.erase(it,fWireHits.end());    
  }
        
  return;
}





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

  std::size_t index = 0;
  std::size_t j = 0;
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
  for( i=0;i<fReferenceMaster.size();i++)
    fReferenceMaster.at(i).clear();
  return;
}




void QwDriftChamberVDC::DefineOptions ( QwOptions& options )
{
  options.AddOptions() ( "use-tdchit",
			 po::value<bool>()->default_bool_value(false),
			 "creat tdc based tree" );
  options.AddOptions() ( "disable-wireoffset",
			 po::value<bool>()->default_bool_value(false),
			 "disable the ablitity of subtracting t0 for every wire" );
  options.AddOptions() ("R3-octant",
			po::value<int>()->default_value(1),
			"MD Package 2 of R3 is in front of" );
}

void QwDriftChamberVDC::ProcessOptions ( QwOptions& options )
{
  fUseTDCHits=options.GetValue<bool> ( "use-tdchit" );
  fDisableWireTimeOffset=options.GetValue<bool> ( "disable-wireoffset" );
  fR3Octant=options.GetValue<int> ( "R3-octant" );
}


void  QwDriftChamberVDC::ConstructHistograms ( TDirectory *folder, TString& prefix )
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

  TotHits.resize ( total_plane_number );
  TOFP.resize ( total_plane_number );
  TOFP_raw.resize ( total_plane_number );
  WiresHit.resize ( total_plane_number );
  TOFW.resize ( total_plane_number );
  TOFW_raw.resize ( total_plane_number );
  HitsWire.resize ( total_plane_number );

  std::size_t iplane = 0;
  // std::cout <<  "QwDriftChamberVDC::ConstructHistograms, "
  // <<  "we are contructing histograms with index from 0 to " <<total_plane_number
  // << "\n"
  // <<  "Thus, fWiresPerPlane.size() returns "
  // << total_plane_number
  // << " and its array definition is ["
  // << total_plane_number
  // << "]."
  // <<  " And hist[i] <-> hist.at(i) <-> fWiresPerplane[i] <-> fWiresPerPlane.at(i)"
  // << std::endl;

  // wire_per_plane is the number of wire per plane?
  //
  // we skip the first zero-th plane or wire histogram. thus
  // i starts with '1'. hist[0] is NULL

  for ( iplane=1; iplane< total_plane_number; iplane++ )
    {
      //   std::cout << "QwDriftChamberVDC iplane" << iplane << std::endl;
      // push_back can "push" iplane = 1 into TotHits.at(0) ??
      TotHits[iplane] = new TH1F (
				  Form ( "%s%sWiresPlane%zu", prefix.Data(), region.Data(), iplane ),
				  Form ( "Total hits on all wires in plane %zu", iplane ),
				  fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset
				  );

      TotHits[iplane]->GetXaxis()->SetTitle ( "Wire #" );
      TotHits[iplane]->GetYaxis()->SetTitle ( "Events" );

      WiresHit[iplane] = new TH1F (
				   Form ( "%s%sNumberHitsPlane%d", prefix.Data(), region.Data(), iplane ),
				   Form ( "Number of Wires Hit in plane %d",iplane ),
				   20, bin_offset, 20+bin_offset
				   );
      WiresHit[iplane]->GetXaxis()->SetTitle ( "Wires Hit per Event" );
      WiresHit[iplane]->GetYaxis()->SetTitle ( "Events" );

      HitsWire[iplane] = new TH2F (
				   Form ( "%s%sNumberHitsVsWirePlane%d", prefix.Data(), region.Data(), iplane ),
				   Form ( "hits on all wires per event in plane %d", iplane ),
				   fWiresPerPlane[iplane],bin_offset,fWiresPerPlane[iplane]+bin_offset,
				   7, -bin_offset, 7-bin_offset
				   );
      HitsWire[iplane]->GetXaxis()->SetTitle ( "Wire Number" );
      HitsWire[iplane]->GetYaxis()->SetTitle ( "Hits" );

      TOFP[iplane] = new TH1F (
			       Form ( "%s%sTimePlane%d", prefix.Data(), region.Data(), iplane ),
			       Form ( "Subtracted time of flight for events in plane %d", iplane ),
			       310,0,0
			       );
      TOFP[iplane] -> SetDefaultBufferSize ( buffer_size );
      TOFP[iplane] -> GetXaxis()->SetTitle ( "Time of Flight" );
      TOFP[iplane] -> GetYaxis()->SetTitle ( "Hits" );


      TOFP_raw[iplane] = new TH1F (
				   Form ( "%s%sRawTimePlane%d", prefix.Data(), region.Data(), iplane ),
				   Form ( "Raw time of flight for events in plane %d", iplane ),
				   //			     400,-65000,65000);
				   310, 0,0
				   );
      TOFP_raw[iplane] -> SetDefaultBufferSize ( buffer_size );
      TOFP_raw[iplane]->GetXaxis()->SetTitle ( "Time of Flight" );
      TOFP_raw[iplane]->GetYaxis()->SetTitle ( "Hits" );

      TOFW[iplane] = new TH2F (
			       Form ( "%s%sTimeVsWirePlane%d", prefix.Data(), region.Data(), iplane ),
			       Form ( "Subtracted time of flight for each wire in plane %d", iplane ),
			       fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset,
			       100,-2000,2000
			       );
      // why this range is not -65000 ??
      TOFW[iplane]->GetXaxis()->SetTitle ( "Wire Number" );
      TOFW[iplane]->GetYaxis()->SetTitle ( "Time of Flight" );

      TOFW_raw[iplane] = new TH2F (
				   Form ( "%s%sRawTimeofFlightperWirePlane%d", prefix.Data() ,region.Data(),iplane ),
				   Form ( "Raw time of flight for each wire in plane %d",iplane ),
				   fWiresPerPlane[iplane], bin_offset, fWiresPerPlane[iplane]+bin_offset,
				   100,-40000,65000
				   );
      // why this range is not -65000 ??
      TOFW_raw[iplane]->GetXaxis()->SetTitle ( "Wire Number" );
      TOFW_raw[iplane]->GetYaxis()->SetTitle ( "Time of Flight" );
    }
  return;
}



void  QwDriftChamberVDC::FillHistograms()
{

  Bool_t local_debug = false;
  if ( not HasDataLoaded() ) return;

  QwDetectorID   this_detid;
  QwDetectorInfo *this_det;

  std::vector<Int_t> wireshitperplane ( fWiresPerPlane.size(),0 );

  UInt_t raw_time = 0;
  Double_t time   = 0.0;

  Int_t plane = 0;
  Int_t element = 0;
  EQwDetectorPackage package = kPackageNull;

  Int_t plane_index = 0;

  for ( std::vector<QwHit>::iterator hit=fTDCHits.begin(); hit!=fTDCHits.end(); hit++ )
    {

      this_detid = hit->GetDetectorID();
      plane      = this_detid.fPlane;
      element    = this_detid.fElement;
      package    = this_detid.fPackage;

      plane_index = 4* ( ( Int_t ) package -1 ) + plane;

      if ( plane<=0 or element<=0 )
	{
	  if ( local_debug )
	    {
	      QwMessage << "QwDriftChamberVDC::FillHistograms:  Bad plane or element index:"
			<< "  fPlane = "  << plane
			<< "  fElement= " << element
			<< QwLog::endl;
	    }
	  continue;
	}

      raw_time = hit->GetRawTime();
      time     = hit->GetTimeNs();

      //  Fill ToF histograms
      TOFP_raw[plane_index]->Fill ( raw_time );
      TOFW_raw[plane_index]->Fill ( element, raw_time );


      if ( local_debug )
	{
	  QwMessage << " QwDriftChamberVDC::FillHistograms() plane " << plane
		    << " element " << element
		    << " package " << package
		    << " plane_index " << plane_index
		    << QwLog::endl;
	}
    }
  for ( std::vector<QwHit>::iterator hit1=fWireHits.begin(); hit1!=fWireHits.end(); hit1++ )
    {

      this_detid = hit1->GetDetectorID();
      plane      = this_detid.fPlane;
      element    = this_detid.fElement;
      package    = this_detid.fPackage;

      if ( plane<=0 or element<=0 )
	{
	  if ( local_debug )
	    {
	      QwMessage << "QwDriftChamberVDC::FillHistograms:  Bad plane or element index:"
			<< "  fPlane = "  << plane
			<< "  fElement= " << element
			<< QwLog::endl;
	    }
	  continue;
	}

      plane_index = 4* ( ( Int_t ) package -1 ) + plane;
      this_det   = & ( fWireData.at ( plane_index ).at ( element-1 ) );


      if ( hit1->IsFirstDetectorHit() )
	{
	  //  If this is the first hit for this detector, then let's plot the
	  //  total number of hits this wire had.
	  //		 std::cout << "this_det->GetNumHits: " << this_det->GetNumHits() << std::endl;
	  HitsWire[plane_index]->Fill ( element,this_det->GetNumHits() );

	  // //     //  Also increment the total number of events in whichthis wire was hit.
	  TotHits[plane_index]->Fill ( element,1 );
	  //  Increment the number of wires hit in this plane
	  wireshitperplane.at ( plane_index ) += 1;
	}
      //  Fill ToF histograms

      time     = hit1->GetTimeNs();

      TOFP[plane_index]->Fill ( time );
      TOFW[plane_index]->Fill ( element,time );
      this_det->ClearHits();


    }

  std::size_t iplane = 0;

  for ( iplane=1; iplane<fWiresPerPlane.size(); iplane++ )
    {
      WiresHit[iplane]->Fill ( wireshitperplane[iplane] );
    }
  return;
}



Int_t QwDriftChamberVDC::LoadTimeWireOffset ( TString t0_map )
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


void QwDriftChamberVDC::SubtractWireTimeOffset()
{
  Int_t plane = 0;
  Int_t wire  = 0;
  EQwDetectorPackage package = kPackageNull;

  Double_t t0_NS = 0.0;

  Double_t educated_guess_t0_correction_NS = 91; 
  // Double_t educated_guess_t0_correction_AU = educated_guess_t0_correction_NS/fF1TDCResolutionNS;

  std::size_t nhits = fWireHits.size();

  for(std::size_t i=0;i<nhits;i++)
    {
      package = fWireHits.at(i).GetPackage();
      plane   = fWireHits.at(i).GetPlane();
      wire    = fWireHits.at(i).GetElement();
      t0_NS   = fTimeWireOffsets.at ( package-1 ).at ( plane-1 ).at ( wire-1 );
      t0_NS  += educated_guess_t0_correction_NS;

      // real_time=fWireHits.at(i).GetTime()-t0-91;
      fWireHits.at(i).SubtractTimeNsOffset(t0_NS);                     // time unit is ns Replace fTimeNs
      fWireHits.at(i).SubtractTimeAuOffset(t0_NS/fF1TDCResolutionNS);  // time unit is a.u. Replace fTime
    }
  return;
}



void QwDriftChamberVDC::UpdateHits()
{

  Double_t real_time_au  = 0.0;
  Double_t tmpTime_au    = 0.0;
  Double_t left_time_au  = 0.0;
  Double_t right_time_au = 0.0;

  Int_t    tmpCrate   = 0;
  Int_t    tmpModule  = 0;
  Int_t    tmpChan    = 0;
  Int_t    tmpbp      = 0;
  Int_t    tmpln      = 0;
  Int_t    plane      = 0;
  Int_t    wire_hit   = 0;
  Int_t    mycount    = 0;
  
  Bool_t  kDir        = kTRUE;
  Bool_t  tmpAM       = kFALSE;

  std::vector<Int_t> wire_array;
  wire_array.clear();

  Int_t channel_offset = 64;

  QwElectronicsID tmpElectronicsID;

  // processing the delay line starts....
  for ( std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++ )
    {
      //this for loop will Fill in the tdc hits data Int_to the corresponding delay line
      tmpElectronicsID = iter->GetElectronicsID();
      tmpCrate         = iter->GetSubbankID();
      tmpTime_au       = iter->GetTime();  // a.u.
      tmpModule        = tmpElectronicsID.fModule;
      tmpChan          = tmpElectronicsID.fChannel;
      
      if ( tmpCrate==3 ) tmpChan+=channel_offset; // ROC10

      if ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fSide == 0 ) {
	fDelayLineArray
	  .at( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane )
	  .at( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber)
	  .LeftHits.push_back ( tmpTime_au );
      }
      else {
	fDelayLineArray
	  .at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane )
	  .at ( fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber )
	  .RightHits.push_back ( tmpTime_au );
      }
    }

  EQwDetectorPackage package;
  EQwDirectionID direction;

  for ( std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++ )
    {
      tmpElectronicsID = iter->GetElectronicsID();
      tmpCrate         = iter->GetSubbankID();
      // if(tmpCrate == 0) break;
      //    tmpTime_au        = iter->GetTime();
      tmpModule        = tmpElectronicsID.fModule;
      tmpChan          = tmpElectronicsID.fChannel;
      // Int_t tmpROC     = tmpElectronicsID.fROC;
      if ( tmpCrate==3 ) tmpChan+=channel_offset; // ROC10

      tmpbp  = fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fBackPlane;
      tmpln  = fDelayLinePtrs.at ( tmpModule ).at ( tmpChan ).fLineNumber;

      plane     = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fPlane ;
      package   = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fPackage;
      direction = fDelayLineArray.at ( tmpbp ).at ( tmpln ).fDirection;

      // QwMessage << "QwDriftChamberVDC::ProcessEvent() :"
      //  	  << " plane = "  << plane
      //    	  << " direction = " << direction
      //    	  << " package = " << package
      // 	  << " roc = " << tmpROC
      // 	  << " slot = " << tmpModule
      // 	  << " chan = " << tmpChan

      //    	  << QwLog::endl;

      if ( fDelayLineArray.at ( tmpbp ).at ( tmpln ).Processed == kFALSE )   //if this delay line has been Processed
	{
	  wire_array.clear();
	  
	  if ( tmpbp==0 || tmpbp ==3 || tmpbp==4 || tmpbp==7 || tmpbp==8 || tmpbp==11 || tmpbp==12 || tmpbp==15 ) {
	    kDir = kTRUE;         //true means left-right
	  }
	  else { 
	    kDir = kFALSE;
	  }
	  
	  fDelayLineArray.at ( tmpbp ).at ( tmpln ).ProcessHits ( kDir );

	  std::size_t Wirecount = fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.size();
	  //	    std::cout << "Wirecount " << Wirecount << std::endl;
	  for ( std::size_t i=0;i<Wirecount;i++ )
	    {
	      std::size_t  Ambiguitycount = fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.at ( i ).size();   //if there's a ambiguity, it's 2; if not, this is 1
	      if ( Ambiguitycount==1 ) tmpAM = kFALSE;
	      else                     tmpAM = kTRUE;
	      
	      Int_t order_L = fDelayLineArray.at ( tmpbp ).at ( tmpln ).Hitscount.at ( i ).first;
	      Int_t order_R = fDelayLineArray.at ( tmpbp ).at ( tmpln ).Hitscount.at ( i ).second;

	      left_time_au  = fDelayLineArray.at ( tmpbp ).at ( tmpln ).LeftHits.at  ( order_L );
	      right_time_au = fDelayLineArray.at ( tmpbp ).at ( tmpln ).RightHits.at ( order_R );

	      for ( std::size_t j=0;j<Ambiguitycount;++j )
		{
		  real_time_au = ( left_time_au+right_time_au ) /2.0; // a.u.

		  wire_hit     = fDelayLineArray.at ( tmpbp ).at ( tmpln ).Wire.at ( i ).at ( j );

		  wire_array.push_back ( wire_hit );

		  mycount = count ( wire_array.begin(), wire_array.end(), wire_hit ) - 1;

		  Int_t temp_Chan=0;
		  if ( tmpCrate==3 ) temp_Chan=tmpChan-channel_offset;
		  else               temp_Chan=tmpChan;

		  const QwDetectorInfo* local_info = fDetectorInfo.in(package).at(plane-1);

		  QwHit NewQwHit ( tmpCrate, tmpModule, temp_Chan, mycount, kRegionID3, package, plane, direction, wire_hit );

		  NewQwHit.SetHitNumberR       ( order_R );
		  NewQwHit.SetDetectorInfo     ( local_info );
		  NewQwHit.SetAmbiguityID      ( tmpAM, j );

		  NewQwHit.SetTime             ( real_time_au );
		  NewQwHit.ApplyTimeCalibration( fF1TDCResolutionNS );  // Fill fTimeRes and fTimeNs in QwHit

		  fWireData.at ( 4* ( package-1 ) +plane ).at ( wire_hit-1 ).PushHit ( ( Int_t ) real_time_au );

		  fWireHits.push_back ( NewQwHit );
		}
	    }
	}
    }


  return;
}



// void QwDriftChamberVDC::ApplyTimeCalibration()
// {

//   for(std::vector<QwHit>::iterator iter=fWireHits.begin(); iter!=fWireHits.end(); ++iter)
//     {
//       iter->SetTime(fF1TDCResolutionNS*iter->GetTime());
//       iter->SetTimeRes(fF1TDCResolutionNS);
//     }

//   return;
// }



void QwDriftChamberVDC::LoadTtoDParameters ( TString ttod_map )
{

  QwParameterFile mapstr ( ttod_map.Data() );
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  
  Double_t t = 0.0;
  Double_t d = 0.0;
  
  while ( mapstr.ReadNextLine() )
    {
      mapstr.TrimComment ( '!' );
      mapstr.TrimWhitespace();
      if ( mapstr.LineIsEmpty() ) continue;
      
      t= mapstr.GetTypedNextToken<Double_t>();
      d = mapstr.GetTypedNextToken<Double_t>();
      fTtoDNumbers.push_back ( d );
    }
  
  mapstr.Close(); // Close the file (ifstream)
  return;
}
