/**********************************************************\
* File: QwGasElectronMultiplier.cc                         *
*                                                          *
* Author: J. Pan                                           *
* Date:  Fri Oct 30 00:12:25 CDT 2009                      *
\**********************************************************/

//This could be a place to hold the interface functions for region 1 GEM.

#include "QwGasElectronMultiplier.h"

  QwGasElectronMultiplier::QwGasElectronMultiplier(TString region_tmp):VQwSubsystemTracking(region_tmp){};

  QwGasElectronMultiplier::~QwGasElectronMultiplier()
    {
      DeleteHistograms();
    }

  /*  Member functions derived from VQwSubsystemTracking. */
  Int_t QwGasElectronMultiplier::LoadChannelMap(TString mapfile ){return 0;};

  Int_t QwGasElectronMultiplier::LoadInputParameters(TString mapfile) {  return 0;};

  Int_t QwGasElectronMultiplier::LoadQweakGeometry(TString mapfile)
  {

  std::cout<<"Region 1 Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction,dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires,detectorId,region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo temp_Detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize(kNumPackages);

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

      if (region==1){
	    temp_Detector.SetDetectorInfo(dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId);


	    if (package == "u")
	      fDetectorInfo.at(kPackageUp).push_back(temp_Detector);
	    else if (package == "d")
	      fDetectorInfo.at(kPackageDown).push_back(temp_Detector);
      }
    }

  }
  std::cout<<"Loaded Qweak Geometry"<<" Total Detectors in pkg_u 1 "<<fDetectorInfo.at(kPackageUp).size()<< " pkg_d 2 "<<fDetectorInfo.at(kPackageDown).size()<<std::endl;

  std::cout << "Sorting detector info..." << std::endl;
  plane = 1;
  std::sort(fDetectorInfo.at(kPackageUp).begin(),
            fDetectorInfo.at(kPackageUp).end());
  for (UInt_t i = 0; i < fDetectorInfo.at(kPackageUp).size(); i++) {
    fDetectorInfo.at(kPackageUp).at(i).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at(kPackageUp).at(i).fRegion<<" Detector ID "<<fDetectorInfo.at(kPackageUp).at(i).fDetectorID << std::endl;
  }

  plane = 1;
  std::sort(fDetectorInfo.at(kPackageDown).begin(),
            fDetectorInfo.at(kPackageDown).end());
  for (UInt_t i = 0; i < fDetectorInfo.at(kPackageDown).size(); i++) {
    fDetectorInfo.at(kPackageDown).at(i).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at(kPackageDown).at(i).fRegion<<" Detector ID " << fDetectorInfo.at(kPackageDown).at(i).fDetectorID << std::endl;
  }

  std::cout<<"Qweak Region 1 GEM Geometry Loaded "<<std::endl;

  return 0;
};

  void  QwGasElectronMultiplier::ClearEventData(){};

  Int_t QwGasElectronMultiplier::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return 0;};

  Int_t QwGasElectronMultiplier::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return 0;};

  void  QwGasElectronMultiplier::ProcessEvent(){
    if (! HasDataLoaded()) return;
  };

  void  QwGasElectronMultiplier::FillListOfHits(QwHitContainer& hitlist){};
  
  void  QwGasElectronMultiplier::ConstructHistograms(TDirectory *folder, TString &prefix){};
  void  QwGasElectronMultiplier::FillHistograms(){};
  void  QwGasElectronMultiplier::DeleteHistograms(){};



