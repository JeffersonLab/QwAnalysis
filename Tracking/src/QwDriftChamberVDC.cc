/**********************************************************\
* File: QwDriftChamberVDC.C                                *
*                                                          *
* Author: P. M. King , Rakitha Beminiwattha                *
* Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/



/*
Note: This is a dummy code from region 2 to generate hits for region 3 set of hits.
This generates identical set of hits to region 2 for region 3.
 */



#include "QwDriftChamberVDC.h"

#include "QwParameterFile.h"

#include<boost/bind.hpp>


const UInt_t QwDriftChamberVDC::BackPlanenum=16;
const UInt_t QwDriftChamberVDC::Linenum=8;

QwDriftChamberVDC::QwDriftChamberVDC(TString region_tmp): QwDriftChamber(region_tmp,fWireHitsVDC){
  std::vector<QwDelayLine> temp;
  temp.resize(Linenum);
  DelayLineArray.resize(BackPlanenum,temp);
  OK=0;
};

Int_t QwDriftChamberVDC::LoadQweakGeometry(TString mapfile){
  std::cout<<"Region 3 Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction,dType;
  Int_t  chan,  plane, TotalWires,detectorId,region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo temp_Detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize(kNumPackages);
  Int_t pkg,pln;

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
      //std::cout<<"Detector ID "<<detectorId<<" "<<varvalue<<" Package "<<package<<" Plane "<<Zpos<<" Region "<<region<<std::endl;

      if (region==3){
	    temp_Detector.SetDetectorInfo(dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId);


	    if (package == "u")
	      fDetectorInfo.at(kPackageUp).push_back(temp_Detector);
	    else if (package == "d")
	      fDetectorInfo.at(kPackageDown).push_back(temp_Detector);
      }
    }

  }
  std::cout<<"Loaded Qweak Geometry"<<" Total Detectors in pkg_d 1 "<<fDetectorInfo.at(kPackageUp).size()<< " pkg_d 2 "<<fDetectorInfo.at(kPackageDown).size()<<std::endl;

  for(int i=0;i<fDetectorInfo.at(kPackageUp).size();i++){
    std::cout<<" Region "<<fDetectorInfo.at(kPackageUp).at(i).fRegion<<" Detector ID "<<fDetectorInfo.at(kPackageUp).at(i).fDetectorID << std::endl;
  }
  for(int i=0;i<fDetectorInfo.at(kPackageDown).size();i++){
    std::cout<<" Region "<<fDetectorInfo.at(kPackageDown).at(i).fRegion<<" Detector ID " << fDetectorInfo.at(kPackageUp).at(i).fDetectorID << std::endl;
  }

  std::cout<<"Qweak Geometry Loaded "<<std::endl;







  return OK;
}






void  QwDriftChamberVDC::ReportConfiguration(){
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



void  QwDriftChamberVDC::SubtractReferenceTimes(){

  Bool_t refs_okay = kTRUE;
  std::vector<Double_t> reftimes;

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
      hit1->SubtractReference(reftimes.at(hit1->GetSubbankID()));
    }
  }
};

Double_t  QwDriftChamberVDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector){
  return 0;
};


void  QwDriftChamberVDC::FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data){
  Int_t tdcindex = GetTDCIndex(bank_index,slot_num);
  if (tdcindex != -1){
    Int_t hitCount=0;
    Int_t package = 2;
    Int_t plane   = fTDCPtrs.at(tdcindex).at(chan).fPlane;
    Int_t wire    = fTDCPtrs.at(tdcindex).at(chan).fElement;
    Int_t direction;

    if (plane == -1 || wire == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
    } else if (plane == kReferenceChannelPlaneNumber){
      fReferenceData.at(wire).push_back(data);
    }/* else {
      //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_1"<<endl;
      direction =fDirectionData.at(package-1).at(plane-1); //wire direction is accessed from the vector and updates the QwHit with it. Rakitha(10/23/2008)

      hitCount=std::count_if(fTDCHits.begin(),fTDCHits.end(),boost::bind(&QwHit::WireMatches,_1,2,boost::ref(package),boost::ref(plane),boost::ref(wire)) );
      //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_2"<<endl;
      fTDCHits.push_back(QwHit(bank_index, slot_num, chan, hitCount,3, package, plane,direction, wire, data));//in order-> bank index, slot num, chan, hitcount, region=3, package, plane,,direction, wire,wire hit time

    }
    */
  };
};






Int_t QwDriftChamberVDC::LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire){
  fReferenceChannels.at(fCurrentBankIndex).first  = fCurrentTDCIndex;
  fReferenceChannels.at(fCurrentBankIndex).second = chan;
  //  Register a reference channel with the wire equal to the bank index.
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = 0;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
  fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = fCurrentBankIndex;
  return OK;
};

Int_t QwDriftChamberVDC::BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire){
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

Int_t QwDriftChamberVDC::AddChannelDefinition(const UInt_t plane, const UInt_t wire){
  fWireData.resize(fWiresPerPlane.size());
  for (size_t i=0; i<fWiresPerPlane.size(); i++){
    fWireData.at(i).resize(fWiresPerPlane.at(i));
  }
  for (size_t i=0; i<fTDC_Index.size(); i++){
    Int_t refchan = i;
    for (size_t j=0; j<fTDC_Index.at(i).size(); j++){
      Int_t mytdc = fTDC_Index.at(i).at(j);
      if (mytdc!=-1){
	for (size_t k=0; k<fTDCPtrs.at(mytdc).size(); k++){
	  //	  Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
	  Int_t plane   = fTDCPtrs.at(mytdc).at(k).fPlane;
	  if (plane>0 && plane !=kReferenceChannelPlaneNumber){
	    Int_t wire  = fTDCPtrs.at(mytdc).at(k).fElement;
	    fWireData.at(plane).at(wire).SetElectronics(i,j,k);
	  }
	}
      }
    }
  }
  return OK;
}


void QwDriftChamberVDC::LoadMap(TString& mapfile){
  //some type(like string,int)need to be changed to root type

  TString varname,varvalue;
  UInt_t channum;            //store temporary channel number
  UInt_t bpnum,lnnum;        //store temp backplane and line number
  UInt_t pknum,plnum,firstwire,LR;         //store temp package,plane,firstwire and left or right information
  std::vector<QwDelayLineID> tmpDelayLineID;
  std::vector<Double_t> tmpWindows;
  QwParameterFile mapstr ( mapfile.Data() );
  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );
    mapstr.TrimWhitespace();
    if(mapstr.LineIsEmpty()) continue;

    if(mapstr.HasVariablePair("=",varname,varvalue))   //to judge whether we find a new slot
      {
	varname.ToLower();
	UInt_t value = atol ( varvalue.Data() );   //as long as the slot is in order, we do not need this line
	if (tmpDelayLineID.size() !=0)
	  {
	    fDelayLinePtrs.push_back (tmpDelayLineID);
	    tmpDelayLineID.clear();
	  }
	continue;        //go to the next line
      }
    channum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    bpnum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    lnnum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));

    if(bpnum == kReferenceChannelPlaneNumber)
      {
	LinkReferenceChannel ( channum, bpnum, lnnum );
	continue;
      }

    LR=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    tmpDelayLineID.push_back(QwDelayLineID(bpnum,lnnum,LR));    //the slot and channel number must be in order
    if(DelayLineArray.at(bpnum).at(lnnum).fill == false)   //if this delay line has not been filled in the data
      {
	pknum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
	plnum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
	firstwire=(atol ( mapstr.GetNextToken ( ", \t()" ).c_str()));
	string a=mapstr.GetNextToken(", \t()") ;
	while (a.size() !=0){
	  tmpWindows.push_back(atof(a.c_str()));
	  a=mapstr.GetNextToken(", \t()");
	}
	DelayLineArray.at(bpnum).at(lnnum).Package=pknum;
	DelayLineArray.at(bpnum).at(lnnum).Plane=plnum;
	DelayLineArray.at(bpnum).at(lnnum).FirstWire=firstwire;
	for (int i=0;i<tmpWindows.size() /2;i++)
	  {
	    std::pair<double,double> a(tmpWindows.at ( 2*i ),tmpWindows.at ( 2*i+1 ));
	    DelayLineArray.at(bpnum).at(lnnum).Windows.push_back (a);
	  }
	DelayLineArray.at(bpnum).at(lnnum).fill=true;
      }
  }

  if(tmpDelayLineID.size() !=0)     //to push the last tmpDelayLineID vector into fDelayLinePtrs
    {
      fDelayLinePtrs.push_back(tmpDelayLineID);
      tmpDelayLineID.clear();
    }
}



void QwDriftChamberVDC::ReadEvent(TString& eventfile){
  TString varname,varvalue;
  UInt_t slotnum,channum;            //store temporary channel number
  UInt_t pknum,plnum;         //store temp package,plane,firstwire and left or right information
  UInt_t value;
  Double_t signal;
  QwParameterFile mapstr(eventfile.Data());
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment( '!' );
    mapstr.TrimWhitespace();
    if (mapstr.LineIsEmpty()) continue;
    if (mapstr.HasVariablePair("=",varname,varvalue))   //to judge whether we find a new crate
      {
	varname.ToLower();
	value = atol(varvalue.Data());
	continue;
      }

    slotnum=(atol(mapstr.GetNextToken( ", \t()" ).c_str()));
    channum=(atol(mapstr.GetNextToken( ", \t()" ).c_str()));

    signal=(atof(mapstr.GetNextToken( ", \t()" ).c_str()));
    //std::cout << "signal is: " << signal << endl;
    fTDCHits.push_back(QwHit(value,slotnum,channum,0,3,0,0,0,0,signal));
  }        //only know TDC information and time value
}




void QwDriftChamberVDC::ProcessEvent(){
  if (! HasDataLoaded()) return;
  //  Do the reference time subtration and subtration of time offsets.
  SubtractReferenceTimes();

  for(std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++){
    //this for loop will fill in the tdc hits data into the corresponding delay line
    QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
    int tmpCrate=iter->GetSubbankID();
    int tmpModule=tmpElectronicsID.fModule;
    int tmpChan=tmpElectronicsID.fChannel;
    const double tmpTime=iter->GetTime();

    if (fDelayLinePtrs.at(tmpModule).at(tmpChan).Side == 0)
      DelayLineArray.at(fDelayLinePtrs.at(tmpModule).at(tmpChan).BackPlane).at(fDelayLinePtrs.at(tmpModule).at(tmpChan).Linenumber).LeftHits.push_back(tmpTime);
    else
      DelayLineArray.at(fDelayLinePtrs.at(tmpModule).at(tmpChan).BackPlane).at(fDelayLinePtrs.at(tmpModule).at(tmpChan).Linenumber).RightHits.push_back(tmpTime);
  }

  for(std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++)
    {
      QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
      QwDetectorID    tmpDetectorID=iter->GetDetectorID();
      int tmpCrate=iter->GetSubbankID();
      const int tmpTime=iter->GetTime();
      int tmpModule=tmpElectronicsID.fModule;
      int tmpChan=tmpElectronicsID.fChannel;
      int tempbp=fDelayLinePtrs.at(tmpModule).at(tmpChan).BackPlane;
      int templn=fDelayLinePtrs.at(tmpModule).at(tmpChan).Linenumber;
      if (DelayLineArray.at(tempbp).at(templn).processed == false)         //if this delay line has been processed
	{
	  DelayLineArray.at(tempbp).at(templn).ProcessHits();
	  int Wirecount=DelayLineArray.at(tempbp).at(templn).Wire.size();
	  for(int i=0;i<Wirecount;i++)
	    {
	      int Ambiguitycount=DelayLineArray.at(tempbp).at(templn).Wire.at(i).size();   //if there's a ambiguity, it's 2; if not, this is 1
	      int order_L=DelayLineArray.at(tempbp).at(templn).Hitscount.at(i).first;
	      int order_R=DelayLineArray.at(tempbp).at(templn).Hitscount.at(i).second;
	      for(int j=0;j<Ambiguitycount;j++)
		{
		  QwHit NewQwHit(tmpCrate,tmpModule ,tmpChan,order_L,3,DelayLineArray.at(tempbp).at(templn).Package,DelayLineArray.at (tempbp).at(templn).Plane,tmpDetectorID.fDirection,DelayLineArray.at(tempbp).at(templn).Wire.at(i).at(j),tmpTime);
		  NewQwHit.SetHitNumberR(order_R);

		  bool tmpAM=DelayLineArray.at(tempbp).at(templn).Ambiguous;
		  if(j==0 && tmpAM==true)
		    NewQwHit.AmbiguityID ( tmpAM,true );
		  else if(j==1 && tmpAM==true)
					NewQwHit.AmbiguityID(tmpAM,false);
		  fWireHitsVDC.push_back(NewQwHit);
		}
	    }
	}
    }
}

