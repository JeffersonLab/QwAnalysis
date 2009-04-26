#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

#include<vector>

#include "QwTypes.h"





class QwDetectorInfo{
  ///
  ///  Tracking detector information class.  This will be used in an array
  ///  indexed by the package, plane, and wire,
  ///
public:

  QwDetectorInfo(){}
  void SetDetectorInfo(TString sdType, Double_t Zpos1, Double_t rot,Double_t  sp_res,Double_t  track_res,Double_t slope_match, TString spackage, Int_t region,TString planeDir, Double_t Det_originX,Double_t Det_originY,Double_t ActivewidthX,Double_t ActivewidthY,Double_t ActivewidthZ,Double_t WireSpace,Double_t FirstWire,Double_t W_rcos,Double_t W_rsin,Int_t totalwires, Int_t detId ){

    Zpos=Zpos1;
    Detector_Rot=rot;
    Spacial_Res=sp_res;
    Track_Res=track_res;
    Slope_Match=slope_match;
    DetectorOriginX=Det_originX;
    DetectorOriginY=Det_originY;
    ActiveWidthX=ActivewidthX;
    ActiveWidthY=ActivewidthY;
    ActiveWidthZ=ActivewidthZ;
    WireSpacing=WireSpace;
    FirstWirePos=FirstWire;
    Wire_rcosX=W_rcos;
    Wire_rsinX=W_rsin;
    TotalWires=totalwires;
    DetectorId=detId;
    if (sdType == "d" && region == 2)
      dType = kTypeDriftHDC;
    else if (sdType == "d" && region == 3)
      dType = kTypeDriftVDC;
    else if (sdType == "t")
       dType = kTypeTrigscint;
    else if (sdType == "c")
       dType = kTypeCerenkov;
    else if (sdType == "g")
       dType = kTypeGem;

    if (spackage == "u")
      package=kPackageUp;//i.e. =1
    else if (spackage == "d")
      package=kPackageDown;//i.e. =2

    if (region == 1)
      Region = kRegionID1;
    else if (region == 2)
      Region = kRegionID2;
    else if (region == 3)
      Region = kRegionID3;

    if (planeDir == "x")
      PlaneDir=kDirectionX;
    else if (planeDir == "u")
      PlaneDir=kDirectionU;
    else if (planeDir == "v")
      PlaneDir=kDirectionV;
    else if (planeDir == "y")
      PlaneDir=kDirectionY;

  };

	//identification info for readout channels. Filled at load time.
	int fCrate; //ROC number
	int fModule; //F1TDC slot number or module index
	int fChannel; //channel number


	//Detector geometry information
	enum EQwDetectorType dType;
	Double_t Zpos;
	Double_t Detector_Rot;
	Double_t Spacial_Res;
	Double_t Track_Res;
	Double_t Slope_Match;
	enum EQwDetectorPackage package;
	enum EQwRegionID Region;
	enum EQwDirectionID PlaneDir;
	Double_t DetectorOriginX;
	Double_t DetectorOriginY;
	Double_t ActiveWidthX;
	Double_t ActiveWidthY;
	Double_t ActiveWidthZ;
	Double_t WireSpacing;
	Double_t FirstWirePos;
	Double_t Wire_rcosX;
	Double_t Wire_rsinX;
	Int_t TotalWires;
	Int_t DetectorId;

	//reference channel index in list of reference channels (most prob. filled at load time)
	int fReferenceChannelIndex;


	//list of active hits by absolute hit number from QwHit array. filled for each event; cleared after each event.
	std::vector<int> fHitID;


	void SetElectronics(int crt,int mdl,int chn)
	  {
	    fCrate=crt;
	    fModule=mdl;
	    fChannel=chn;
	    fReferenceChannelIndex=1;
	  }

	Bool_t IsHit()
	  {
	    return (!fHitID.empty());
	  }

	Int_t GetNumHits(){return fHitID.size();};

	void ClearHits()
	  {
	    if (!fHitID.empty())
	      fHitID.clear();
	  }

	void PushHit(int time)
	  {
	    fHitID.push_back(time);
	  }
};

#endif
