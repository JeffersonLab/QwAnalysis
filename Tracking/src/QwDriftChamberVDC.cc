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

#include "boost/bind.hpp"


const UInt_t QwDriftChamberVDC::BackPlanenum=4;
const UInt_t QwDriftChamberVDC::Linenum=8;

QwDriftChamberVDC::QwDriftChamberVDC(TString region_tmp):VQwSubsystem(region_tmp),
                                                         QwDriftChamber(region_tmp,fWireHitsVDC)
{
  std::vector<QwDelayLine> temp;
  temp.resize(Linenum);
  DelayLineArray.resize(BackPlanenum,temp);
  fDelayLinePtrs.resize(21);
  OK=0;
};

Int_t QwDriftChamberVDC::LoadQweakGeometry(TString mapfile)
{
  std::cout<<"Region 3 Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires, detectorId, region, DIRMODE;
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

  std::cout << "Sorting detector info..." << std::endl;
  plane = 1;
  std::sort(fDetectorInfo.at(kPackageUp).begin(),
            fDetectorInfo.at(kPackageUp).end());
  for(size_t i = 0; i < fDetectorInfo.at(kPackageUp).size(); i++) {
    fDetectorInfo.at(kPackageUp).at(i).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at(kPackageUp).at(i).fRegion<<" Detector ID "<<fDetectorInfo.at(kPackageUp).at(i).fDetectorID << std::endl;
  }

  plane = 1;
  std::sort(fDetectorInfo.at(kPackageDown).begin(),
            fDetectorInfo.at(kPackageDown).end());
  for(size_t i = 0; i < fDetectorInfo.at(kPackageDown).size(); i++) {
    fDetectorInfo.at(kPackageDown).at(i).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at(kPackageDown).at(i).fRegion<<" Detector ID " << fDetectorInfo.at(kPackageDown).at(i).fDetectorID << std::endl;
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
      hit1->SetTime( SubtractReference(hit1->GetRawTime(),reftimes.at(hit1->GetSubbankID())) );
    }
  }
}

Double_t  QwDriftChamberVDC::CalculateDriftDistance(Double_t drifttime, QwDetectorID detector, Double_t angle){
  Double_t a0=0,a1=0,b0=0,b1=0,b2=0,c0=0,c1=0;
  Double_t tsp1=0,tsp2=0,x=0;

  a0=-3.48422+0.0286254*angle;
  a1=28.6858-0.293514*angle;
  b0=-19.467+0.477792*angle;
  b1=38.7557-0.595964*angle;
  b2=-1.37907+0.0521888*angle;
  c0=27.7324-1.19175*angle;
  c1=22.3955-0.00131218*angle;

  //the cutpoint is 2.1 and 5.9;
  tsp1=b0+2.1*b1+2.1*2.1*b2;
  tsp2=b0+5.9*b1+5.9*5.9*b2;

  if (drifttime < tsp1) {                    // Select Region 1
  x = (drifttime - a0)/a1;}
  else if (drifttime <= tsp2){ // Select Region 2
  x = (-b1 + sqrt(b1*b1 - 4*b2*(b0-drifttime)))/(2*b2);}
  else{                                  // Select Region 3
  x = (drifttime - c0)/c1;
}
  return x;
};


void  QwDriftChamberVDC::FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data){
  //Int_t tdcindex = GetTDCIndex(bank_index,slot_num);
  Int_t tdcindex=1;
  if (tdcindex != -1){
        Int_t hitCount = 1;
        Int_t package  = 1;
	//Int_t plane    = fTDCPtrs.at(tdcindex).at(chan).fPlane;
	//Int_t wire     = fTDCPtrs.at(tdcindex).at(chan).fElement;

	Int_t plane=-2;
	if(slot_num==3)
	  plane=fDelayLinePtrs.at(slot_num).at(chan).BackPlane;
	Int_t wire=0;
	Int_t direction=0;


    if (plane == -1 || wire == -1){
      //  This channel is not connected to anything.
      //  Do nothing.
      //  } else if (plane == (Int_t) kReferenceChannelPlaneNumber){
      //fReferenceData.at(wire).push_back(data);
    } else if (slot_num==4){
      fReferenceData.at(wire).push_back(data);
    } else {
      //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_1"<<endl;
      //direction =fDirectionData.at(package-1).at(plane-1); //wire direction is accessed from the vector and updates the QwHit with it. Rakitha(10/23/2008)

      // hitCount=std::count_if(fTDCHits.begin(),fTDCHits.end(),boost::bind(&QwHit::WireMatches,_1,2,boost::ref(package),boost::ref(plane),boost::ref(wire)) );
      //std::cout<<"At QwDriftChamberVDC::FillRawTDCWord_2"<<endl;
      fTDCHits.push_back(QwHit(bank_index, slot_num, chan, hitCount, kRegionID3, package, plane,direction, wire, data));//in order-> bank index, slot num, chan, hitcount, region=3, package, plane,,direction, wire,wire hit time

    }

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
      //std::cout << "fCurrent: " << fTDCPtrs.size() << std::endl;
      //std::cout << "chan: " << chan << " " << plane << std::endl;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPackage = package;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fPlane   = plane;
    fTDCPtrs.at(fCurrentTDCIndex).at(chan).fElement = wire;
    if (plane>=fWiresPerPlane.size()){
	fWiresPerPlane.resize(plane+1);
	//fWiresPerPlane.resize(plane);
    }
    //if (wire>=fWiresPerPlane.at(plane)){
    //    fWiresPerPlane.at(plane) =  wire+1;
    // }
    fWiresPerPlane.at(plane)=280;
  }
  return OK;
};

Int_t QwDriftChamberVDC::AddChannelDefinition(const UInt_t plane, const UInt_t wire){
  fWireData.resize(fWiresPerPlane.size());
  for (size_t i=1; i<fWiresPerPlane.size(); i++){
    fWireData.at(i).resize(fWiresPerPlane.at(i));
  }

  for (size_t i=0; i<fTDC_Index.size(); i++){
    //    Int_t refchan = i;
    for (size_t j=0; j<fTDC_Index.at(i).size(); j++){
      Int_t mytdc = fTDC_Index.at(i).at(j);
      //std::cout << "mytdc: " << mytdc << std::endl;
      if (mytdc!=-1){
	for (size_t k=0; k<fTDCPtrs.at(mytdc).size(); k++){
	  //	  Int_t package = fTDCPtrs.at(mytdc).at(k).fPackage;
	  Int_t plane   = fTDCPtrs.at(mytdc).at(k).fPlane;
	  if (plane>0 && plane != (Int_t) kReferenceChannelPlaneNumber){
	      //Int_t wire  = fTDCPtrs.at(mytdc).at(k).fElement;
	    fWireData.at(plane).at(wire).SetElectronics(i,j,k);
	  }
	}
      }
    }
  }
  return OK;
}


//Int_t  QwDriftChamberVDC::LoadChannelMap(TString mapfile)
//{
//  return LoadMap(mapfile);
//}





Int_t QwDriftChamberVDC::LoadChannelMap(TString mapfile)
{
  //some type(like string,int)need to be changed to root type

  TString varname,varvalue;
  UInt_t value = 0;
  UInt_t channum;            //store temporary channel number
  UInt_t bpnum,lnnum;        //store temp backplane and line number
  UInt_t pknum,plnum,dir,firstwire,LR;         //store temp package,plane,firstwire and left or right information
  Bool_t IsFirstChannel = true;

  std::vector<Double_t> tmpWindows;
  QwParameterFile mapstr ( mapfile.Data() );

  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );
    mapstr.TrimWhitespace();
    if(mapstr.LineIsEmpty()) continue;

    if(mapstr.HasVariablePair("=",varname,varvalue))   //to judge whether we find a new slot
      {
	varname.ToLower();
	value = atol ( varvalue.Data() );   //as long as the slot is in order, we do not need this line
	if (varname == "roc")
	  {
	    RegisterROCNumber(value);
	    std::cout << "ROC number: " << value << std::endl;
	  }
	if(varname == "slot"){
	  RegisterSlotNumber(value);
	}
	continue;        //go to the next line
      }

    channum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    bpnum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    lnnum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));

    if(channum ==0 && bpnum ==0){
      if( IsFirstChannel == true ) IsFirstChannel = false;
      else                         continue;
    }

    if(bpnum == kReferenceChannelPlaneNumber)
      {
	LinkReferenceChannel ( channum, bpnum, lnnum );
	continue;
      }

    LR=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    fDelayLinePtrs.at(value).push_back(QwDelayLineID(bpnum,lnnum,LR));    //the slot and channel number must be in order
    pknum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    plnum=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    dir=(atol(mapstr.GetNextToken ( ", \t()" ).c_str()));
    firstwire=(atol ( mapstr.GetNextToken ( ", \t()" ).c_str()));

    //BuildWireDataStructure(channum,pknum,plnum,firstwire);
    if(DelayLineArray.at(bpnum).at(lnnum).fill == false)   //if this delay line has not been filled in the data
      {
	std::string a = mapstr.GetNextToken(", \t()") ;
	while (a.size() !=0){
	  tmpWindows.push_back(atof(a.c_str()));
	  a = mapstr.GetNextToken(", \t()");
	}
	DelayLineArray.at(bpnum).at(lnnum).Package=pknum;
	DelayLineArray.at(bpnum).at(lnnum).Plane=plnum;
	DelayLineArray.at(bpnum).at(lnnum).Direction=dir;
	DelayLineArray.at(bpnum).at(lnnum).FirstWire=firstwire;
	for (size_t i=0;i<tmpWindows.size() /2;i++)
	  {
	    std::pair<double,double> a(tmpWindows.at ( 2*i ),tmpWindows.at ( 2*i+1 ));
	    DelayLineArray.at(bpnum).at(lnnum).Windows.push_back (a);
	  }
	std::cout << "DelayLine: back plane: " << bpnum << "line number " << lnnum << " Windows.size: "  << DelayLineArray.at(bpnum).at(lnnum).Windows.size() << std::endl;
	DelayLineArray.at(bpnum).at(lnnum).fill=true;
        tmpWindows.clear();
      }
  }
  return OK;
}



void QwDriftChamberVDC::ReadEvent(TString& eventfile){
  TString varname,varvalue;
  UInt_t slotnum,channum;            //store temporary channel number
  //  UInt_t pknum,plnum;         //store temp package,plane,firstwire and left or right information
  UInt_t value = 0;
  Double_t signal = 0.0; // double? unsigned int ? by jhlee
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

    slotnum = (atol(mapstr.GetNextToken( ", \t()" ).c_str()));
    channum = (atol(mapstr.GetNextToken( ", \t()" ).c_str()));
    signal  = (atof(mapstr.GetNextToken( ", \t()" ).c_str()));
    //std::cout << "signal is: " << signal << endl;
    fTDCHits.push_back(QwHit(value,slotnum,channum,0, kRegionID3,0,0,0,0, (UInt_t) signal));
  }        //only know TDC information and time value
}




void QwDriftChamberVDC::ProcessEvent(){
  if (! HasDataLoaded()) return;
  //  Do the reference time subtration and subtration of time offsets.
  SubtractReferenceTimes();

  double real_time=0,drift_distance=0;
  double tmpTime=0,left_time=0,right_time=0;

  int tmpCrate=0,tmpModule=0,tmpChan=0,tmpbp=0,tmpln=0;



  for(std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++){
    //this for loop will fill in the tdc hits data into the corresponding delay line
    QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
    tmpCrate=iter->GetSubbankID();
    tmpModule=tmpElectronicsID.fModule;
    tmpChan=tmpElectronicsID.fChannel;

    tmpTime=iter->GetTime();

    //std::cout << "tmpModule " << tmpModule << " tmpChan: " << tmpChan << " time: " <<  tmpTime << std::endl ;

    if (fDelayLinePtrs.at(tmpModule).at(tmpChan).Side == 0)
      DelayLineArray.at(fDelayLinePtrs.at(tmpModule).at(tmpChan).BackPlane).at(fDelayLinePtrs.at(tmpModule).at(tmpChan).Linenumber).LeftHits.push_back(tmpTime);
    else
      DelayLineArray.at(fDelayLinePtrs.at(tmpModule).at(tmpChan).BackPlane).at(fDelayLinePtrs.at(tmpModule).at(tmpChan).Linenumber).RightHits.push_back(tmpTime);
  }



  for(std::vector<QwHit>::iterator iter=fTDCHits.begin();iter!=fTDCHits.end();iter++)
    {
      QwElectronicsID tmpElectronicsID=iter->GetElectronicsID();
      tmpCrate=iter->GetSubbankID();
      tmpTime= iter->GetTime();
      tmpModule = tmpElectronicsID.fModule;
      tmpChan   = tmpElectronicsID.fChannel;

      tmpbp    = fDelayLinePtrs.at(tmpModule).at(tmpChan).BackPlane;
      tmpln    = fDelayLinePtrs.at(tmpModule).at(tmpChan).Linenumber;

      bool kDir=false;

      if (DelayLineArray.at(tmpbp).at(tmpln).processed == false)         //if this delay line has been processed
	{
	  int tmpdir= DelayLineArray.at(tmpbp).at(tmpln).Direction;

	  if(tmpbp==0 || tmpbp ==3)
	    kDir=false;
	  else kDir=true;
	  DelayLineArray.at(tmpbp).at(tmpln).ProcessHits(kDir);

	  Int_t Wirecount=DelayLineArray.at(tmpbp).at(tmpln).Wire.size();
	  for(Int_t i=0;i<Wirecount;i++)
	    {
	      Int_t Ambiguitycount=DelayLineArray.at(tmpbp).at(tmpln).Wire.at(i).size();   //if there's a ambiguity, it's 2; if not, this is 1
	      Bool_t tmpAM=false;
	      if(Ambiguitycount==1) tmpAM=false;
	      else tmpAM=true;
	      Int_t order_L=DelayLineArray.at(tmpbp).at(tmpln).Hitscount.at(i).first;
	      Int_t order_R=DelayLineArray.at(tmpbp).at(tmpln).Hitscount.at(i).second;
	      left_time=DelayLineArray.at(tmpbp).at(tmpln).LeftHits.at(order_L);
	      right_time=DelayLineArray.at(tmpbp).at(tmpln).RightHits.at(order_R);

	      real_time=(left_time+right_time)/2;
	      real_time=0.1132*real_time;

	      for(Int_t j=0;j<Ambiguitycount;j++)
		{
		  QwHit NewQwHit(tmpCrate,tmpModule ,tmpChan,order_L, kRegionID3, DelayLineArray.at(tmpbp).at(tmpln).Package,DelayLineArray.at (tmpbp).at(tmpln).Plane,tmpdir,DelayLineArray.at(tmpbp).at(tmpln).Wire.at(i).at(j),left_time);

		  //AddChannelDefinition(DelayLineArray.at (tmpbp).at(tmpln).Plane,DelayLineArray.at(tmpbp).at(tmpln).Wire.at(i).at(j) );
		  NewQwHit.SetHitNumberR(order_R);
		  NewQwHit.SetTime(real_time);

		  real_time=1600-real_time;
		  if(real_time<0) real_time=0;
		  drift_distance=CalculateDriftDistance(real_time,iter->GetDetectorID());
		  NewQwHit.SetDriftDistance(0.1*drift_distance);

		  //Bool_t tmpAM=DelayLineArray.at(tmpbp).at(tmpln).Ambiguous;
		  if(j==0 && tmpAM==true)
		    NewQwHit.AmbiguityID ( tmpAM,true );
		  else if(j==1 && tmpAM==true)
		    NewQwHit.AmbiguityID(tmpAM,false);
		  fWireHits.push_back(NewQwHit);
		}
	    }
	}
    }
}


 void QwDriftChamberVDC::ClearEventData(){
   SetDataLoaded(kFALSE);
   QwDetectorID this_det;
   for(std::vector<QwHit>::iterator hit1=fTDCHits.begin();hit1!=fTDCHits.end();hit1++){
     this_det = hit1->GetDetectorID();
   }
   fTDCHits.clear();
   fWireHits.clear();
   Int_t index = 0;
   for(size_t i=0;i<DelayLineArray.size();i++)
     {
       index = DelayLineArray.at(i).size();

       for(Int_t j=0;j<index;j++){
	 DelayLineArray.at(i).at(j).processed=false;
	 DelayLineArray.at(i).at(j).LeftHits.clear();
	 DelayLineArray.at(i).at(j).RightHits.clear();
	 DelayLineArray.at(i).at(j).Hitscount.clear();
	 DelayLineArray.at(i).at(j).Wire.clear();
       }
     }

   for (size_t i=0;i<fReferenceData.size(); i++){
     fReferenceData.at(i).clear();
   }
 }
