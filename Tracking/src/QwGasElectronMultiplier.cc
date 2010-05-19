/**********************************************************\
 * File: QwGasElectronMultiplier.cc                         *
 *                                                          *
 * Author: J. Pan                                           *
 * Date:  Fri Oct 30 00:12:25 CDT 2009                      *
\**********************************************************/

//This could be a place to hold the interface functions for region 1 GEM.

#include "QwGasElectronMultiplier.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwGasElectronMultiplier>
  theGasElectronMultiplierFactory("QwGasElectronMultiplier");


QwGasElectronMultiplier::QwGasElectronMultiplier(TString region_tmp):VQwSubsystem(region_tmp),
								     VQwSubsystemTracking(region_tmp){};

QwGasElectronMultiplier::~QwGasElectronMultiplier()
{
  DeleteHistograms();
}

/*  Member functions derived from VQwSubsystemTracking. */
Int_t QwGasElectronMultiplier::LoadChannelMap(TString mapfile ){
  std::cout<<"Region 1 GEM  Map Info Loading..... "<<std::endl;
  TString varname, varvalue;

  Int_t roc=0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      Int_t value = atoi(varvalue.Data());
      if (varname=="roc"){
	//RegisterROCNumber(value);
	std::cout<<"ROC "<<roc<<std::endl;
	roc=value;
      } else if (varname=="bank"){
	VQwSubsystemTracking::RegisterROCNumber(roc,value);
	std::cout<<"Registering ROC "<<roc<<" bank "<<value<<std::endl;
      }
    }
  }
  std::cout<<"Region 1 GEM Map loaded "<<std::endl;
  return 0;

};

Int_t QwGasElectronMultiplier::LoadInputParameters(TString mapfile)
{
  return 0;
};

Int_t QwGasElectronMultiplier::LoadQweakGeometry(TString mapfile)
{

  std::cout<<"Region 1 Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction,dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires,detectorId,region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match, Det_originX,Det_originY;
  Double_t ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo temp_Detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize(kNumPackages);

  DIRMODE = 0;

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
      rot          = (atof(mapstr.GetNextToken(", ").c_str()));
      sp_res       = (atof(mapstr.GetNextToken(", ").c_str()));
      track_res    = (atof(mapstr.GetNextToken(", ").c_str()));
      slope_match  = (atof(mapstr.GetNextToken(", ").c_str()));
      package      = mapstr.GetNextToken(", ").c_str();
      region       = (atoi(mapstr.GetNextToken(", ").c_str()));
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
      TotalWires   = (atoi(mapstr.GetNextToken(", ").c_str()));
      detectorId   = (atoi(mapstr.GetNextToken(", ").c_str()));
      //std::cout<<"Detector ID "<<detectorId<<" "<<varvalue<<" Package "<<package<<" Plane "<<Zpos<<" Region "<<region<<std::endl;

      if (region==1){
	temp_Detector.SetDetectorInfo(dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId);
	if (package == "u")       fDetectorInfo.at(kPackageUp).push_back(temp_Detector);
	else if (package == "d")  fDetectorInfo.at(kPackageDown).push_back(temp_Detector);
      }
    }
  }

  std::cout<<"Loaded Qweak Geometry"<<" Total Detectors in pkg_u 1 "<<fDetectorInfo.at(kPackageUp).size()<< " pkg_d 2 "<<fDetectorInfo.at(kPackageDown).size()<<std::endl;

  std::cout << "Sorting detector info..." << std::endl;
  plane = 1;
  std::sort(fDetectorInfo.at(kPackageUp).begin(),
            fDetectorInfo.at(kPackageUp).end());//sort by Z position
  for (UInt_t i = 0; i < fDetectorInfo.at(kPackageUp).size(); i++) {
    fDetectorInfo.at(kPackageUp).at(i).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at(kPackageUp).at(i).fRegion<<" Detector ID "<<fDetectorInfo.at(kPackageUp).at(i).fDetectorID << std::endl;
  }

  plane = 1;
  std::sort(fDetectorInfo.at(kPackageDown).begin(),
            fDetectorInfo.at(kPackageDown).end());
  for (UInt_t i = 0; i < fDetectorInfo.at(kPackageDown).size(); i++) {
    fDetectorInfo.at(kPackageDown).at(i).fPlane = plane++;
    std::cout<<" Region " << fDetectorInfo.at(kPackageDown).at(i).fRegion
	     <<" Detector ID " << fDetectorInfo.at(kPackageDown).at(i).fDetectorID
	     << std::endl;
  }

  std::cout<<"Qweak Region 1 GEM Geometry Loaded "<<std::endl;

  return 0;
};

void  QwGasElectronMultiplier::ClearEventData()
{
  /*
    for(std::vector<QwHit>::iterator hit1=fGEMHits.begin(); hit1!=fGEMHits.end(); hit1++) {

    }
  */
  fGEMHits.clear();
  return;
};

Int_t QwGasElectronMultiplier::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

Int_t QwGasElectronMultiplier::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t VFAT_index = 0;

  fCurrentROC_ID  = roc_id;
  fCurrentBank_ID = bank_id;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (bDEBUG) std::cout<<"subbank index "<<index<<std::endl;
  if (index>=0 && num_words>0){//Reading VFAT cards data
    for(UInt_t i=0; i<num_words ; i++){
      if (buffer[i] == 0x18){//0x18(24) is the VFAT header word. In complete mode there will be 12 VFATs. So in each event buffer there will be 12 VFAT headers
	if (bDEBUG) std::cout<<"ROC "<<roc_id<<" bank "<<bank_id<<" VFAT "<<(i+1)<<" data-> ";
	//There are 14 words in each VFAT fragment in the buffer.
	//In each fragment VFAT data words starts after 9 words into the fragment.
	//There will be 6 data words in each VFAT
	for (Short_t j=0; j<6 ; j++){
	  fVFATChannel[VFAT_index][2*j]   = buffer[(i+8+j)]>>16;
	  fVFATChannel[VFAT_index][2*j+1] = buffer[(i+8+j)] & 0x0000ffff;
	}
	i=i+14;
	FillVFATWord(VFAT_index);//we have only have one VFAT in the current CODA file
	if (bDEBUG){
	  for(Short_t i=0;i<N_VFAT;i++){
	    std::cout<<" "<<fVFATChannel[0][i];
	  }
	  std::cout<<std::endl;
	}

	VFAT_index++;
      }
    }
    //std::cout<<"VFAT 1 data ";
  }

  return 0;
};

void QwGasElectronMultiplier::FillVFATWord(Int_t VFAT_index)
{

  Int_t  VFAT_data    = 0;
  UInt_t bitwise_mask = 0;

  //fill all VFAT channels into fBuffer_VFAT[VFAT_index][channel] structure.

  for (Int_t VFAT_channel=0; VFAT_channel<N_VFAT; VFAT_channel++){
    VFAT_data    = fVFATChannel[VFAT_index][VFAT_channel];
    bitwise_mask = 0;

    for(Short_t j=0; j<16; j++)
      {
	bitwise_mask = (UInt_t) pow(2,15-j);

	if   ( VFAT_data & bitwise_mask ) fBuffer_VFAT[VFAT_index][16*VFAT_channel+j] = 1;
	else                              fBuffer_VFAT[VFAT_index][16*VFAT_channel+j] = 0;
      }

//     if ( VFAT_data & 32768 ) fBuffer_VFAT[VFAT_index][16*VFAT_channel   ] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel   ] = 0;

//     if ( VFAT_data & 16384 ) fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 1] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 1] = 0;

//     if ( VFAT_data & 8192 )  fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 2] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 2] = 0;

//     if ( VFAT_data & 4096 )  fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 3] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 3] = 0;

//     if ( VFAT_data & 2048 )  fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 4] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 4] = 0;

//     if ( VFAT_data & 1024 )  fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 5] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 5] = 0;

//     if ( VFAT_data & 512 )   fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 6] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 6] = 0;

//     if ( VFAT_data & 256 )   fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 7] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 7] = 0;

//     if ( VFAT_data & 128 )   fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 8] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 8] = 0;

//     if ( VFAT_data & 64 )    fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 9] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+ 9] = 0;

//     if ( VFAT_data & 32 )    fBuffer_VFAT[VFAT_index][16*VFAT_channel+10] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+10] = 0;

//     if ( VFAT_data & 16 )    fBuffer_VFAT[VFAT_index][16*VFAT_channel+11] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+11] = 0;

//     if ( VFAT_data & 8 )     fBuffer_VFAT[VFAT_index][16*VFAT_channel+12] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+12] = 0;

//     if ( VFAT_data & 4 )     fBuffer_VFAT[VFAT_index][16*VFAT_channel+13] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+13] = 0;

//     if ( VFAT_data & 2 )     fBuffer_VFAT[VFAT_index][16*VFAT_channel+14] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+14] = 0;

//     if ( VFAT_data & 1 )     fBuffer_VFAT[VFAT_index][16*VFAT_channel+15] = 1;
//     else                     fBuffer_VFAT[VFAT_index][16*VFAT_channel+15] = 0;
  }

  for (Int_t i=0;i<N_VFAT;i++){

    GetBC(fVAFTBC[i],i);//Bean Count
    GetEC(fVAFTEC[i],i);// Event Count
    GetFlags(fVAFTFLG[i],i);// Flags
    GetChipId(fVAFTChipID[i],i);//chip Id
    //Adding hits for i th VFAT
    GetChannelData(fVAFTChannelData[i],i);//Has all the data for each VFAT and each hit is added inside this routine
  }

  return;
};

void QwGasElectronMultiplier::GetBC(Int_t bean_count[], Int_t VFAT_index)
{//return the VFAT Bean Count

  for(Short_t i=4;i<16;i++)  bean_count[i-4]=fBuffer_VFAT[VFAT_index][i];
  return;

};
void QwGasElectronMultiplier::GetEC(Int_t error_check[], Int_t VFAT_index)
{//return the VFAT Event Count

  for(Short_t i=20;i<28;i++) error_check[i-20]=fBuffer_VFAT[VFAT_index][i];
  return;

};

void QwGasElectronMultiplier::GetFlags(Int_t flags[], Int_t VFAT_index)
{//return the VFAT Flags

  for(Short_t i=28;i<32;i++) flags[i-28]=fBuffer_VFAT[VFAT_index][i];
  return;

};

void QwGasElectronMultiplier::GetChipId(Int_t chip_id[], Int_t VFAT_index)
{//return the VFAT ID

  for(Short_t i=36;i<48;i++) chip_id[i-36]=fBuffer_VFAT[VFAT_index][i];
  return;

};
void QwGasElectronMultiplier::GetChannelData(Int_t channel_data[], Int_t VFAT_index)
{//return the VFAT Channel Data

  Short_t ioffset = 0;
  for(Short_t i=48;i<176;i++){
    ioffset = i-48;
    channel_data[ioffset] = fBuffer_VFAT[VFAT_index][i];
    if (channel_data[ioffset]) {
      //std::cout<<"A Hit "<<VFAT_index<<" "<<i-48+1<<" ROC Bank "<<fCurrentROC_ID<<" "<<fCurrentBank_ID<<std::endl;
      AddHit(VFAT_index,ioffset);
    }
  }
  return;
};

void QwGasElectronMultiplier::AddHit(Int_t VFAT_index, Int_t channel)
{//Add a new hit to the fGEMHits

  Int_t Strip=0;
  EQwDirectionID Plane;
  EQwDetectorPackage Package;

  if (VFAT_index<6){//package up
    Package=kPackageUp;
    channel++;//start channel indexing from 1
  }
  else{
    VFAT_index=VFAT_index-6;//package down
    Package=kPackageDown;
    channel++;//start channel indexing from 1
  }

  //Radial Strips
  if (VFAT_index != 2 && VFAT_index != 3){
    Plane=kDirectionR;//for radial strips
    if (VFAT_index==0){
      if (channel<=64) Strip=4+4*(channel-1);
      else             Strip=2+4*(128-channel);
    }
    else if (VFAT_index==1){
      if (channel<=64) Strip=258+4*(channel-1);
      else             Strip=260+4*(128-channel);
    }
    else if (VFAT_index==4){
      if (channel<=64) Strip=257+4*(64-channel);
      else             Strip=259+4*(channel-65);
    }
    else if (VFAT_index==5){
      if (channel<=64) Strip=3+4*(64-channel);
      else             Strip=1+4*(channel-65);
    }

    //Add the radial hit
    //hitCount gives the total number of hits on a given wire
    fGEMHits.push_back(QwHit(fCurrentBank_ID,VFAT_index,channel,0,kRegionID1,Package,Plane,Plane,Strip,0));//in order-> bank index, VFAT no (1-6), chan, hitcount(=0), region=1, package, plane,direction(equiv plane), Strip no., data (=0)
  }

  else{
    //Transverse strips
    Plane=kDirectionY;//for transverse strips
    if (VFAT_index==2){
      if (channel<=64) Strip = 127-2*(  channel-1)+128;//128 offset added
      else             Strip = 128-2*(128-channel)+128;//128 offset added
    }
    else if (VFAT_index==3){
      if (channel<=64) Strip = -1*(2+2*(  channel-1))+128;
      else             Strip = -1*(1+2*(128-channel))+128;
    }
    //Add the transverse hit
    //hitCount gives the total number of hits on a given wire
    fGEMHits.push_back(QwHit(fCurrentBank_ID,VFAT_index,channel,0,kRegionID1,Package,Plane,Plane,Strip,0));//in order-> bank index, VFAT no (1-6), chan, hitcount(=0), region=1, package, plane,direction(equiv plane), Strip no., data (=0)

  }

  return;

};


void  QwGasElectronMultiplier::ProcessEvent()
{

  //std::cout<<"Event"<<std::endl;
  for(std::vector<QwHit>::iterator hit1=fGEMHits.begin(); hit1!=fGEMHits.end(); hit1++) {

    QwDetectorID local_id = hit1->GetDetectorID();
    int package = local_id.fPackage;
    int plane = local_id.fPlane;

    //Detectors are ordered in Up, radial(R), up  transverse(Y), down radial(R) , down transverse(Y)
    QwDetectorInfo* local_info0 = & fDetectorInfo.at(package).at(0);//each package has two planes (R and Y)
    QwDetectorInfo* local_info1 = & fDetectorInfo.at(package).at(1);
    if (local_info0->GetElementDirection()==local_id.fPlane){
      hit1->SetDetectorInfo(local_info0);
      if (bDEBUG_Hitlist) std::cout<<"Package "<<package<<" Det Plane "<<plane<<" Module  "<<hit1->fModule<<" Channel  "<<hit1->fChannel<<" Direction "<<hit1->GetDirection()<<" Strip "<<hit1->GetElement() <<std::endl;
    }
    else if (local_info1->GetElementDirection()==local_id.fPlane){
      hit1->SetDetectorInfo(local_info1);
      if (bDEBUG_Hitlist) std::cout<<"Package "<<package<<" Det Plane "<<plane<<" Module  "<<hit1->fModule<<" Channel  "<<hit1->fChannel<<" Direction "<<hit1->GetDirection()<<" Strip "<<hit1->GetElement() <<std::endl;
    }
  }
  return;
};

void  QwGasElectronMultiplier::FillListOfHits(QwHitContainer& hitlist)
{
};

void  QwGasElectronMultiplier::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
  //  Now create the histograms...

  TString region = GetSubsystemName();

  // create VFAT histograms
  for (Int_t i=0;i<N_VFAT;i++){
    VFAT[i]             = new TH1D(Form("%s_VFAT%d",region.Data(),i),
				   Form("%s_VFAT%d",region.Data(),i),
				   VFAT_DATA_SIZE, 0, VFAT_DATA_SIZE); //create a 1D histogram (double precision)  datastream
    VFAT_BC[i]          = new TH1D(Form("%s_VFAT%d_Bean_Count",region.Data(),i),
				   Form("%s_VFAT%d_Bean_Count",region.Data(),i),
				   VFAT_BC_SIZE, 0, VFAT_BC_SIZE); //create a 1D histogram (double precision)  bean count
    VFAT_EC[i]          = new TH1D(Form("%s_VFAT%d_Event_Count",region.Data(),i),
				   Form("%s_VFAT%d_Event_Count",region.Data(),i),
				   VFAT_EC_SIZE, 0, VFAT_EC_SIZE); //create a 1D histogram (double precision)  error check
    VFAT_Flags[i]       = new TH1D(Form("%s_VFAT%d_Flags",region.Data(),i),
				   Form("%s_VFAT%d_Flags",region.Data(),i),
				   VFAT_FLG_SIZE, 0, VFAT_FLG_SIZE); //create a 1D histogram (double precision)  flags
    VFAT_ChipId[i]      = new TH1D(Form("%s_VFAT%d_Chip_ID",region.Data(),i),
				   Form("%s_VFAT%d_Chip_ID",region.Data(),i),
				   VFAT_CID_SIZE, 0, VFAT_CID_SIZE); //create a 1D histogram (double precision)  chip id
    VFAT_ChannelData[i] = new TH1D(Form("%s_VFAT%d_Channel_Data",region.Data(),i),
				   Form("%s_VFAT%d_Channel_Data",region.Data(),i),
				   VFAT_CHANNEL_DATA_SIZE, 0, VFAT_CHANNEL_DATA_SIZE); //create a 1D histogram (double precision)  channel data
  }

  //create GEM Histograms
  for (Int_t i=0;i<N_GEM;i++){
    GEM[i] = new TH2D(Form("%s_GEM%d",region.Data(),i),
		      Form("%s_GEM%d",region.Data(),i),
		      GEM_TRANSVERSESTRIPS, 0, GEM_TRANSVERSESTRIPS,
		      GEM_RADIALSTRIPS,     0, GEM_RADIALSTRIPS);//2D plot with x is transverse; y is radial
  }
};




void  QwGasElectronMultiplier::FillHistograms()
{
  //if (! HasDataLoaded()) return;


  QwDetectorID   this_detid;

  Short_t i = 0;
  Short_t j = 0;

  //Fill VAFT Histograms
  for (i=0;i<N_VFAT;i++){

    for(j=0;j<VFAT_DATA_SIZE;j++) {
      if( fBuffer_VFAT[i][j]==1 ) VFAT[i]->Fill(j);
    }

    for(j=0;j<VFAT_CHANNEL_DATA_SIZE;j++) {
      if( fVAFTChannelData[i][j]==1 ) VFAT_ChannelData[i]->Fill(j);
    }

    for(j=0;j<VFAT_CID_SIZE;j++) {
      if( fVAFTChipID[i][j]==1 ) VFAT_ChipId[i]->Fill(j);
    }

    for(j=0;j<VFAT_BC_SIZE;j++) {
      if( fVAFTBC[i][j]==1 ) VFAT_BC[i]->Fill(j);
    }
    for(j=0;j<VFAT_EC_SIZE;j++) {
      if( fVAFTEC[i][j]==1 ) VFAT_EC[i]->Fill(j);
    }
    for(j=0;j<VFAT_FLG_SIZE;j++) {
      if( fVAFTFLG[i][j]==1 ) VFAT_Flags[i]->Fill(j);
    }
  }


  //Fill GEM Histograms
  /*
    for(std::vector<QwHit>::iterator hit1=fGEMHits.begin(); hit1!=fGEMHits.end(); hit1++) {
    this_detid = hit1->GetDetectorID();
    if (bDEBUG) std::cout << "GEM::FillHistograms:  fRegion==" << this_detid.fRegion << ", fPackage==" << this_detid.fPackage << std::endl;
    if (this_detid.fPackage==kPackageUp)//GEM1
    GEM[0]->Fill(hit1->GetPhiPosition(),hit1->GetRPosition());
    else if (this_detid.fPackage==kPackageDown)//GEM2
    GEM[1]->Fill(hit1->GetPhiPosition(),hit1->GetRPosition());
    }
  */

};


void  QwGasElectronMultiplier::DeleteHistograms()
{

  // delete VFAT histograms
  for (Short_t i=0;i<N_VFAT;i++){
    if(VFAT[i]!= NULL){
      delete VFAT[i];
      VFAT[i] = NULL;
    }
    if(VFAT_BC[i]!= NULL){
      delete VFAT_BC[i];
      VFAT_BC[i] = NULL;
    }
    if(VFAT_EC[i]!= NULL){
      delete VFAT_EC[i];
      VFAT_EC[i] = NULL;
    }
    if(VFAT_Flags[i]!= NULL){
      delete VFAT_Flags[i];
      VFAT_Flags[i] = NULL;
    }
    if(VFAT_ChipId[i]!= NULL){
      delete VFAT_ChipId[i];
      VFAT_ChipId[i] = NULL;
    }
    if(VFAT_ChannelData[i]!= NULL){
      delete VFAT_ChannelData[i];
      VFAT_ChannelData[i] = NULL;
    }
  }
  for (Short_t i=0;i<N_GEM;i++){
    if(GEM[i]!= NULL){
      delete GEM[i];
      GEM[i] = NULL;
    }
  }
  /*
  // display buffer
  for ( int i = 0; i < 12; ++i )
  {
  printf("\n\n\nVFAT %i",i);

  for ( int j = 0; j < 192; ++j )
  {
  if ( j % 16 == 0 )
  printf("\n");
  else if ( j % 4 == 0 )
  printf("\t");

  printf("%i ",fBuffer_VFAT[i][j]);
  }
  }
  */

};


/*
//Old routine to calculate strip hits
//if (! HasDataLoaded()) return;
//std::cout<<"QwGasElectronMultiplier::ProcessEvent "<<std::endl;


//Debug only (since we do not have data for VFAT3 and VFAT4 fill them both with ones)
for ( Int_t PhiStrip = 0; PhiStrip < 128; ++PhiStrip ){
//if(PhiStrip%8==0){
fVAFTChannelData[2][PhiStrip]=1;//VFAT3
fVAFTChannelData[3][PhiStrip]=1;//VFAT4
//}
}

//Debug only


//GEM1 only for GEM2 replace 0-5 with 6-11 in the fVAFTChannelData[0-5][] array
// left half ( VFAT4 =fVAFTChannelData[3]  )
for ( Int_t PhiStrip = 0; PhiStrip < 128; ++PhiStrip )
{
if ( fVAFTChannelData[3][PhiStrip] > 0 )   //Transverse (Left)
{
// VFAT1 =fVAFTChannelData[0]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[0][bRadialStrip] > 0 )  // Radial (Bottom)
{
if ( bRadialStrip > 64 ){
fRadialPosition=4*(bRadialStrip-64)+2;
fTransversePosition=PhiStrip;
}
else{
fRadialPosition=4*bRadialStrip;
fTransversePosition=PhiStrip;
}
if (bDEBUG) std::cout<<"1 R "<<fRadialPosition<<" T "<<fTransversePosition<<std::endl;//this is were Hit object is added for each hit.
AddHit(fRadialPosition,fTransversePosition);
}
}


// VFAT2 =fVAFTChannelData[1]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[1][bRadialStrip] > 0 ) // Radial (Top)
{
if ( bRadialStrip > 64 ){
fRadialPosition=256+4*(128-bRadialStrip)-2;
fTransversePosition=PhiStrip;
}
else{
fRadialPosition=256+4*bRadialStrip;
fTransversePosition=PhiStrip;
}
if (bDEBUG) std::cout<<"2 R "<<fRadialPosition<<" T "<<fTransversePosition<<std::endl;//this is were Hit object is added for each hit.
AddHit(fRadialPosition,fTransversePosition);
}
}


// VFAT5 =fVAFTChannelData[4]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[4][bRadialStrip] > 0 ) // Radial (Top)
{
if ( bRadialStrip > 64 ){
fRadialPosition=256+4*(bRadialStrip-64)+3;
fTransversePosition=PhiStrip;
}
else{
fRadialPosition=256+4*(64-bRadialStrip)-3;
fTransversePosition=PhiStrip;
}
if (bDEBUG) std::cout<<"5 R "<<fRadialPosition<<" T "<<fTransversePosition<<std::endl;//this is were Hit object is added for each hit.
AddHit(fRadialPosition,fTransversePosition);
}
}


// VFAT6 =fVAFTChannelData[5]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[5][bRadialStrip] > 0 ) // Radial (Bottom)
{
if ( bRadialStrip > 64 ){
fRadialPosition=4*(bRadialStrip-64)+1;
fTransversePosition=PhiStrip;
}
else{
fRadialPosition=4*(64-bRadialStrip)-1;
fTransversePosition=PhiStrip;
}
if (bDEBUG) std::cout<<"6 R "<<fRadialPosition<<" T "<<fTransversePosition<<std::endl;//this is were Hit object is added for each hit.
AddHit(fRadialPosition,fTransversePosition);//Hit object is added for each hit.
}
}
}
}



// right half ( VFAT3 =fVAFTChannelData[2]  )
for ( Int_t PhiStrip = 0; PhiStrip < 128; ++PhiStrip )
{
if ( fVAFTChannelData[2][PhiStrip] > 0 )   //Transverse (Left)
{
// VFAT1 =fVAFTChannelData[0]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[0][bRadialStrip] > 0 )  // Radial (Bottom)
{
if ( bRadialStrip > 64 ){
fRadialPosition=4*(bRadialStrip-64)+2;
fTransversePosition=128+PhiStrip;
}
else{
fRadialPosition=4*bRadialStrip;
fTransversePosition=128+PhiStrip;
}
AddHit(fRadialPosition,fTransversePosition);//Hit object is added for each hit.
}
}

// VFAT2 =fVAFTChannelData[1]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[1][bRadialStrip] > 0 ) // Radial (Top)
{
if ( bRadialStrip > 64 ){
fRadialPosition=256+4*(128-bRadialStrip)-2;
fTransversePosition=128+PhiStrip;
}
else{
fRadialPosition=256+4*bRadialStrip;
fTransversePosition=128+PhiStrip;
}
AddHit(fRadialPosition,fTransversePosition);//Hit object is added for each hit.
}
}


// VFAT5 =fVAFTChannelData[4]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[4][bRadialStrip] > 0 ) // Radial (Top)
{
if ( bRadialStrip > 64 ){
fRadialPosition=256+4*(bRadialStrip-64)+3;
fTransversePosition=128+PhiStrip;
}
else{
fRadialPosition=256+4*(64-bRadialStrip)-3;
fTransversePosition=128+PhiStrip;
}
AddHit(fRadialPosition,fTransversePosition);//Hit object is added for each hit.
}
}


// VFAT6 =fVAFTChannelData[5]
for ( Int_t bRadialStrip = 0; bRadialStrip < 128; ++bRadialStrip )
{
if ( fVAFTChannelData[5][bRadialStrip] > 0 ) // Radial (Bottom)
{
if ( bRadialStrip > 64 ){
fRadialPosition=4*(bRadialStrip-64)+1;
fTransversePosition=128+PhiStrip;
}
else{
fRadialPosition=4*(64-bRadialStrip)-1;
fTransversePosition=128+PhiStrip;
}
AddHit(fRadialPosition,fTransversePosition);//Hit object is added for each hit.
}
}
}
}
*/


