/*File: QwDriftChamberHDC.C                                *
*                                                          *
* Author: P. M. King , Rakitha Beminiwattha                *
* Time-stamp: <2008-07-08 15:40>                           *
**********************************************************/

#include "QwDriftChamberHDC.h"

#include "QwParameterFile.h"
#include "QwLog.h"

#include<boost/bind.hpp>





QwDriftChamberHDC::QwDriftChamberHDC(TString region_tmp):VQwSubsystem(region_tmp),
                                                         QwDriftChamber(region_tmp,fTDCHits)
{
  OK=0;
};



//LoadQweakGeometry will load QwDetectorInfo vector from a map file
//Currently this method is specific to each region

Int_t QwDriftChamberHDC::LoadQweakGeometry(TString mapfile)
{

  QwMessage << "Region 2 Qweak Geometry Loading..... " << QwLog::endl;

  TString varname, varvalue,package, direction,dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires,detectorId,region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo temp_Detector;

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
      varvalue = (mapstr.GetNextToken(", ").c_str());//this is the sType
      Zpos = (atof(mapstr.GetNextToken(", ").c_str()));
      rot = (atof(mapstr.GetNextToken(", ").c_str()));
      sp_res = (atof(mapstr.GetNextToken(", ").c_str()));
      track_res = (atof(mapstr.GetNextToken(", ").c_str()));
      slope_match = (atof(mapstr.GetNextToken(", ").c_str()));
      package = mapstr.GetNextToken(", ").c_str();
      region  = (atol(mapstr.GetNextToken(", ").c_str()));
      dType = mapstr.GetNextToken(", ").c_str();
      direction  = mapstr.GetNextToken(", ").c_str();
      Det_originX = (atof(mapstr.GetNextToken(", ").c_str()));
      Det_originY = (atof(mapstr.GetNextToken(", ").c_str()));
      ActiveWidthX = (atof(mapstr.GetNextToken(", ").c_str()));
      ActiveWidthY = (atof(mapstr.GetNextToken(", ").c_str()));
      ActiveWidthZ = (atof(mapstr.GetNextToken(", ").c_str()));
      WireSpace = (atof(mapstr.GetNextToken(", ").c_str()));
      FirstWire = (atof(mapstr.GetNextToken(", ").c_str()));
      W_rcos = (atof(mapstr.GetNextToken(", ").c_str()));
      W_rsin = (atof(mapstr.GetNextToken(", ").c_str()));
      TotalWires = (atol(mapstr.GetNextToken(", ").c_str()));
      detectorId = (atol(mapstr.GetNextToken(", ").c_str()));

      QwDebug << " Detector ID " << detectorId << " " << varvalue
              << " Package " << package << " Plane " << Zpos
              << " Region " << region << QwLog::endl;

      if (region==2){
	    temp_Detector.SetDetectorInfo(dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId);


	    if (package == "u")
	      fDetectorInfo.at(kPackageUp).push_back(temp_Detector);
	    else if (package == "d")
	      fDetectorInfo.at(kPackageDown).push_back(temp_Detector);
      }
    }

  }
  QwMessage << "Loaded Qweak Geometry"<<" Total Detectors in pkg_d 1 "<<fDetectorInfo.at(kPackageUp).size()<< " pkg_d 2 "<<fDetectorInfo.at(kPackageDown).size() << QwLog::endl;

  QwMessage << "Sorting detector info..." << QwLog::endl;
  plane = 1;
  std::sort(fDetectorInfo.at(kPackageUp).begin(),
            fDetectorInfo.at(kPackageUp).end());
  for (UInt_t i = 0; i < fDetectorInfo.at(kPackageUp).size(); i++) {
    fDetectorInfo.at(kPackageUp).at(i).fPlane = plane++;
    QwMessage << " Region " << fDetectorInfo.at(kPackageUp).at(i).fRegion << " Detector ID " << fDetectorInfo.at(kPackageUp).at(i).fDetectorID << QwLog::endl;
  }

  plane = 1;
  std::sort(fDetectorInfo.at(kPackageDown).begin(),
            fDetectorInfo.at(kPackageDown).end());
  for (UInt_t i = 0; i < fDetectorInfo.at(kPackageDown).size(); i++) {
    fDetectorInfo.at(kPackageDown).at(i).fPlane = plane++;
    QwMessage << " Region " << fDetectorInfo.at(kPackageDown).at(i).fRegion << " Detector ID " << fDetectorInfo.at(kPackageDown).at(i).fDetectorID << QwLog::endl;
  }

  QwMessage << "Qweak Geometry Loaded " << QwLog::endl;







  return OK;

};



void  QwDriftChamberHDC::ReportConfiguration(){
  for (size_t i = 0; i<fROC_IDs.size(); i++){
    for (size_t j=0; j<fBank_IDs.at(i).size(); j++){
      Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
      QwMessage << "ROC " << fROC_IDs.at(i)
		<< ", subbank " << fBank_IDs.at(i).at(j)
		<< ":  subbank index==" << ind
		<< QwLog::endl;
      for (size_t k=0; k<kMaxNumberOfTDCsPerROC; k++){
	Int_t tdcindex = GetTDCIndex(ind,k);
	QwMessage << "    Slot " << k;
	if (tdcindex == -1)
	  QwMessage << "  Empty" << QwLog::endl;
	else
	  QwMessage << "  TDC#" << tdcindex << QwLog::endl;
      }
    }
  }
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    if (fWiresPerPlane.at(i) == 0) continue;
    QwMessage << "Plane " << i << " has " << fWireData.at(i).size()
	      << " wires"
	      << QwLog::endl;
  }
};



void  QwDriftChamberHDC::SubtractReferenceTimes()
{
  Bool_t refs_okay = kTRUE;
  std::vector<Double_t> reftimes;
  Int_t counter=1;


  reftimes.resize(fReferenceData.size());
  for (size_t i=0; i<fReferenceData.size(); i++){
    if (fReferenceData.at(i).size()==0){
      //  There isn't a reference time!
      std::cerr << "QwDriftChamber::SubtractReferenceTimes:  Subbank ID "
		<< i << " is missing a reference time." << std::endl;
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
    for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {

      hit1->SetTime(SubtractReference(hit1->GetTime(),reftimes.at(hit1->GetSubbankID())) );
      if (counter>0){
	if (hit1->GetDetectorID().fPlane==7){//this will read the first hit time of trig_h1
	  trig_h1=hit1->GetTime();
	  //std::cout<<"********Found trig_h1 "<< trig_h1<<std::endl;
	  counter=0;
	}
      }
      counter++;
    }
  }
};

Double_t  QwDriftChamberHDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector){
  //0.00545449393  0.0668865488  0.000352462179 -2.00383196E-05  3.57577417E-07  -2.82802562E-09  7.89009965E-12
  Double_t dt_ = 0.12 * (drifttime-trig_h1 + 933.0);
  Double_t dd_;

  dd_ = 0.00545449393
      + 0.0668865488 * dt_
      + 0.000352462179 * dt_ * dt_
      + (-2.00383196E-05 * dt_ * dt_ * dt_)
      + ( 3.57577417E-07 * dt_ * dt_ * dt_ * dt_)
      + (-2.82802562E-09 * dt_ * dt_ * dt_ * dt_ * dt_)
      + ( 7.89009965E-12 * dt_ * dt_ * dt_ * dt_ * dt_ * dt_);

  //std::cout<<" Drift distance "<<dd_<<" Drift time "<<dt_<<" Original value  "<<drifttime<<std::endl;

  // This formula returns a drift distance in [mm], but the tracking code
  // expects a number in [cm].  Since the geometry definitions are in [cm],
  // we should stick to [cm] as unit of distance. (wdconinc)
  return dd_ / 10.0;
};


void  QwDriftChamberHDC::FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data)
{
  Int_t tdcindex = GetTDCIndex(bank_index,slot_num);
  if (tdcindex != -1){
    EQwDirectionID direction = kDirectionNull;
    Int_t hitCount = 0;
    EQwDetectorPackage package  =   kPackageUp;//one package available
    Int_t plane    = fTDCPtrs.at(tdcindex).at(chan).fPlane;
    Int_t wire     = fTDCPtrs.at(tdcindex).at(chan).fElement;

    //Int_t hitCount;
    if (plane == -1 || wire == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
    } else if (plane == (Int_t) kReferenceChannelPlaneNumber){
      fReferenceData.at(wire).push_back(data);//now wire contains the value fCurrentBankIndex so we can assign the ref timing data to it.
    } else {

      direction = (EQwDirectionID)fDirectionData.at(package-1).at(plane-1); //Wire Direction is accessed from the vector -Rakitha (10/23/2008)

      //hitCount gives the total number of hits on a given wire -Rakitha (10/23/2008)
      hitCount=std::count_if(fTDCHits.begin(),fTDCHits.end(),boost::bind(&QwHit::WireMatches,_1,2,boost::ref(package),boost::ref(plane),boost::ref(wire)) );
      fTDCHits.push_back(QwHit(bank_index, slot_num, chan, hitCount, kRegionID2, package, plane,direction, wire, data));//in order-> bank index, slot num, chan, hitcount, region=2, package, plane,,direction, wire,wire hit time

    }
  };
};




Int_t QwDriftChamberHDC::LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire){
  fReferenceChannels.at(fCurrentBankIndex).first  = fCurrentTDCIndex;//fCurrentBankIndex is updated at RegisterROCNumber() and fCurrentTDCIndex is updated at RegisterSlotNumber()
  fReferenceChannels.at(fCurrentBankIndex).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = 0;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = fCurrentBankIndex;
  return OK;
};

Int_t QwDriftChamberHDC::BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire){
  if (plane == kReferenceChannelPlaneNumber){
    LinkReferenceChannel(chan, plane, wire);
  } else {
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = package;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = wire;
    if (plane>=fWiresPerPlane.size()){
      fWiresPerPlane.resize(plane+1);
    }
    if (wire>=fWiresPerPlane.at(plane)){
      fWiresPerPlane.at(plane) =  wire+1;
    }
  }
  return OK;
};

Int_t QwDriftChamberHDC::AddChannelDefinition(const UInt_t plane, const UInt_t wire)
{
  fWireData.resize(fWiresPerPlane.size());
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    fWireData.at(i).resize(fWiresPerPlane.at(i));
  }
  for (size_t i=0; i<fTDC_Index.size(); i++){
    //  Int_t refchan = i;
    for (size_t j=0; j<fTDC_Index.at(i).size(); j++){
      Int_t mytdc = fTDC_Index.at(i).at(j);
      if (mytdc!=-1){
	for (size_t k=0; k<fTDCPtrs.at(mytdc).size(); k++){
	  //	  Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
	  Int_t plane   = fTDCPtrs.at(mytdc).at(k).fPlane;
	  if (plane>0 && plane != (Int_t) kReferenceChannelPlaneNumber){
	    Int_t wire  = fTDCPtrs.at(mytdc).at(k).fElement;
	    fWireData.at(plane).at(wire).SetElectronics(i,j,k);
	  }
	}
      }
    }
  }
  return OK;
}

void  QwDriftChamberHDC::ProcessEvent(){
  if (! HasDataLoaded()) return;

  SubtractReferenceTimes();


  for(std::vector<QwHit>::iterator hit1=fTDCHits.begin(); hit1!=fTDCHits.end(); hit1++) {

    //if (hit1->GetDetectorID().fPlane<7){
      //std::cout<<"Plane "<<hit1->GetDetectorID().fPlane<<std::endl;

      // Set the detector info pointer for this hit
      // (TODO Should probably go in VQwSubsystemTracking or even VQwSubsystem)
      QwDetectorID local_id = hit1->GetDetectorID();
      int package = local_id.fPackage;
      int plane = local_id.fPlane - 1;
      QwDetectorInfo* local_info = & fDetectorInfo.at(package).at(plane);
      hit1->SetDetectorInfo(local_info);

      hit1->SetDriftDistance(CalculateDriftDistance(hit1->GetTime(),hit1->GetDetectorID()));

    //}

  }



};
