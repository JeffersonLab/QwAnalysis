#include "QwGUIHallCBeamline.h"

#include "TGaxis.h"
#include "TColor.h"
#include "TStyle.h"


ClassImp(QwGUIHallCBeamline);

const Int_t QwGUIHallCBeamline::fSleepTimeMS = 2000;

enum QwGUIHallCBeamlineIndentificator {
  BA_POS_DIFF,
  BA_CHARGE,
  BA_POS_VAR,
  BA_HCSCALER,
  BA_MEAN_POS,
  BA_TGT_X,
  BA_TGT_Y,
  BA_TGT_ANGLE_X,
  BA_TGT_ANGLE_Y,
  BA_TGT_CHG,
  BA_TGT_RASTER
};

//HALLC_DET_TYPES is the size of the enum
enum QwGUIHallCBeamlineDeviceTypes {
  UNKNOWN_TYPE,
  VQWK_BPMSTRIPLINE,
  VQWK_BPMCAVITY,
  VQWK_BCM,  
  SCALER_HALO,
  COMBINED
};

//Combo box
enum QwGUIHallCBeamlinePlotsComboBox {
  CMB_HCBCM,
  CMB_HCSCALER
};


QwGUIHallCBeamline::QwGUIHallCBeamline(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 

  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dHCBCMFrame         = NULL;
  dHCSCALERFrame      = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dBtnLayout          = NULL;
  dButtonPos          = NULL;
  dButtonMeanPos      = NULL;
  dButtonCharge       = NULL;
  dButtonPosVariation = NULL;
  dButtonHCSCALER     = NULL;
  dButtonTgtX         = NULL;
  dButtonTgtY         = NULL;
  dButtonTgtAngleX    = NULL;
  dButtonTgtAngleY    = NULL;
  dButtonTgtCharge    = NULL;
  dButtonTgtRaster    = NULL;
  dComboBoxHCBCM      = NULL;
  
  
  PosVariation[0] = NULL;
  PosVariation[1] = NULL;

  //DataWindowArray.Clear();

  AddThisTab(this);
  
  SetHistoAccumulate(1);
  SetHistoReset(0);
}

QwGUIHallCBeamline::~QwGUIHallCBeamline()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dHCBCMFrame)         delete dHCBCMFrame;
  if(dHCSCALERFrame)      delete dHCSCALERFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dButtonPos)          delete dButtonPos;
  if(dButtonCharge)       delete dButtonCharge;
  if(dButtonPosVariation) delete dButtonPosVariation;
  if(dComboBoxHCBCM)      delete dComboBoxHCBCM;
  if(dButtonHCSCALER)     delete dButtonHCSCALER;
  if(dButtonMeanPos)      delete dButtonMeanPos; 
  if(dButtonTgtX)         delete dButtonTgtX;
  if(dButtonTgtY)         delete dButtonTgtY;
  if(dButtonTgtAngleX)    delete dButtonTgtAngleX;
  if(dButtonTgtAngleY)    delete dButtonTgtAngleY;
  if(dButtonTgtCharge)    delete dButtonTgtCharge;
  if(dButtonTgtRaster)    delete dButtonTgtRaster;
  delete [] PosVariation;

  // RemoveThisTab(this);
  //  IsClosing(GetName());
}

void QwGUIHallCBeamline::LoadHistoMapFile(TString mapfile){//this is called in the QwGUIMain c'tor

  TString varname, varvalue;
  TString modtype, namech,dettype;
  Int_t count_names;
  fHallCDevices.resize(HALLC_DET_TYPES);
  QwParameterFile mapstr(mapfile.Data());  //Open the file
  //fHallCDevices.clear();
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname=="name"){
	//printf("%s - %s \n",varname.Data(),varvalue.Data());
	fHallCDevices.at(COMBINED).push_back(varvalue);
      }
    }
    else{
      modtype   = mapstr.GetTypedNextToken<TString>();	// module type
      if (modtype=="VQWK" || modtype=="SCALER"){
	mapstr.GetTypedNextToken<TString>();	//slot number
	mapstr.GetTypedNextToken<TString>();	//channel number
	dettype=mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	namech.ToLower();
	if (modtype=="VQWK" && dettype=="bpmstripline")
	  namech.Remove(namech.Length()-2,2);

	count_names=count(fHallCDevices.at(VQWK_BPMSTRIPLINE).begin(),fHallCDevices.at(VQWK_BPMSTRIPLINE).end(),namech);

	if (!count_names && dettype=="bpmstripline"){
	  //printf("%s - %s \n",modtype.Data(),namech.Data() );	  
	  fHallCDevices.at(VQWK_BPMSTRIPLINE).push_back(namech);
	}
	else if (dettype=="bpmcavity"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fHallCDevices.at(VQWK_BPMCAVITY).push_back(namech);
	}
	else if (dettype=="bcm"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fHallCDevices.at(VQWK_BCM).push_back(namech);
	}
	else if (dettype=="halomonitor"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fHallCDevices.at(SCALER_HALO).push_back(namech);
	}

      }

    }

  }
  /*
  //printf("no. of hallC devices %d \n",fHallCDevices.size());
   printf(" Hall C  Device List\n" );
  for (Size_t i=0;i<fHallCDevices.size();i++)
    for (Size_t j=0;j<fHallCDevices.at(i).size();j++)
      printf("%s \n",fHallCDevices.at(i).at(j).Data() );
  */
  LoadHCBCMCombo();//Load BCM list into the combo box
  LoadHCSCALERCombo();//Load SCALER list into the combo box

};

void QwGUIHallCBeamline::MakeLayout()
{

  SetCleanup(kDeepCleanup);

  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));


  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  // TGVertical3DLine *separator = new TGVertical3DLine(this);
  // dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

 
  //hall c bcm access frame
  dHCBCMFrame= new TGVerticalFrame(dControlsFrame,50,100);
  //hall c scaler access frame
  dHCSCALERFrame= new TGVerticalFrame(dControlsFrame,50,100);


  dButtonPos          = new TGTextButton(dControlsFrame, "&Beam Position Diffs", BA_POS_DIFF);
  dButtonMeanPos      = new TGTextButton(dControlsFrame, "&Mean Beam Positions", BA_MEAN_POS);
  dButtonCharge       = new TGTextButton(dHCBCMFrame, "B&CM Yield/Asymmetry", BA_CHARGE);
  dButtonHCSCALER     = new TGTextButton(dHCSCALERFrame, "&SCALER Yield/Asymmetry", BA_HCSCALER);
  dButtonPosVariation = new TGTextButton(dControlsFrame, "BPM Eff_Charge Variation", BA_POS_VAR);
  dButtonTgtX         = new TGTextButton(dControlsFrame, "Target X", BA_TGT_X);
  dButtonTgtY         = new TGTextButton(dControlsFrame, "Target Y", BA_TGT_Y);
  dButtonTgtAngleX    = new TGTextButton(dControlsFrame, "Target ANGLE X", BA_TGT_ANGLE_X);
  dButtonTgtAngleY    = new TGTextButton(dControlsFrame, "Target ANGLE Y", BA_TGT_ANGLE_Y);
  dButtonTgtCharge    = new TGTextButton(dControlsFrame, "Target Charge", BA_TGT_CHG);
  dButtonTgtRaster    = new TGTextButton(dControlsFrame, "Fast Raster", BA_TGT_RASTER);
  
  dButtonCharge   -> SetEnabled(kFALSE);
  dButtonHCSCALER -> SetEnabled(kFALSE);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dButtonPos,dBtnLayout );
  

  //dControlsFrame->AddFrame(dButtonCharge,dBtnLayout);
  dControlsFrame->AddFrame(dButtonPosVariation, dBtnLayout);
  dControlsFrame->AddFrame(dButtonMeanPos, dBtnLayout);
  dControlsFrame->AddFrame(dHCBCMFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dHCSCALERFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dButtonTgtX, dBtnLayout);
  dControlsFrame->AddFrame(dButtonTgtY, dBtnLayout);
  dControlsFrame->AddFrame(dButtonTgtAngleX, dBtnLayout);
  dControlsFrame->AddFrame(dButtonTgtAngleY, dBtnLayout);
  dControlsFrame->AddFrame(dButtonTgtCharge, dBtnLayout);
  dControlsFrame->AddFrame(dButtonTgtRaster, dBtnLayout);



  //Add HC BCM  combo box
  dComboBoxHCBCM=new TGComboBox(dHCBCMFrame,CMB_HCBCM);
  dComboBoxHCBCM->Resize(50,20);//To make it better looking
  
  //Add HC SCALER  combo box
  dComboBoxHCSCALER=new TGComboBox(dHCSCALERFrame,CMB_HCSCALER);
  dComboBoxHCSCALER->Resize(50,20);//To make it better looking

  //add components to the hall c bcm access frame
  dHCBCMFrame->AddFrame(dComboBoxHCBCM, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dHCBCMFrame->AddFrame(dButtonCharge, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  //add components to the hall c SCALER access frame
  dHCSCALERFrame->AddFrame(dComboBoxHCSCALER, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dHCSCALERFrame->AddFrame(dButtonHCSCALER, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));


  dButtonPos          -> Associate(this);
  dButtonMeanPos      -> Associate(this);
  dButtonCharge       -> Associate(this);
  dButtonPosVariation -> Associate(this);
  dButtonHCSCALER     -> Associate(this);
  dButtonTgtX         -> Associate(this);
  dButtonTgtY         -> Associate(this);
  dButtonTgtAngleX    -> Associate(this);
  dButtonTgtAngleY    -> Associate(this);
  dComboBoxHCBCM      -> Associate(this);
  dComboBoxHCSCALER   -> Associate(this);
  dButtonTgtCharge    -> Associate(this);
  dButtonTgtRaster    -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  // dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  // 				"QwGUIHallCBeamline",
  // 				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");



}

void QwGUIHallCBeamline::OnReceiveMessage(char *obj)
{

}

// void QwGUIHallCBeamline::OnObjClose(char *obj)
// {
// }


// void QwGUIHallCBeamline::OnRemoveThisTab()
// {

// };




void QwGUIHallCBeamline::PositionDifferences()
{
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  //Int_t BPMSTriplinesCount = 0;
  Int_t BPMSTriplinesCount = (Int_t) fHallCDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  TString dummyname;

  Bool_t ldebug = false;

  std::vector<TH1F *> histo1;//_array;
  histo1.resize(BPMSTriplinesCount);


  std::vector<TH1F *> histo1_buff;//_array;
  histo1_buff.resize(BPMSTriplinesCount);

  char histo[128];
  
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 
  SetHistoDefaultMode();//bring the histo mode to accumulate mode

   while (1){ 
     PosVariation[0] = new TH1F("Eff_Asym", "Eff_Charge Asym Variation",BPMSTriplinesCount , min_range, max_range);
     PosVariation[1] = new TH1F("Eff_Asym_RMS", "Eff_Charge Asym RMS Variation",BPMSTriplinesCount , min_range, max_range); 
    for(Int_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      if (GetHistoPause()==0){
	sprintf (histo, "asym_%s_EffectiveCharge_hw", fHallCDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
	histo1[p]= (TH1F *)dMapFile->Get(histo); 
      }
      if (histo1[p]!=NULL)
	{
	  if (GetHistoReset()){
	    histo1_buff[p]=(TH1F*)histo1[p]->Clone(Form("%s_buff",histo1[p]->GetName()));
	    *histo1[p]=*histo1[p]-*histo1_buff[p];
	    if (p==BPMSTriplinesCount-1)//once all histo are buffered set the reser state
	      SetHistoReset(0);
	  }else if (GetHistoAccumulate()==0){
	    *histo1[p]=*histo1[p]-*histo1_buff[p];
	  }	    
	
	  xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	  histo1[p]->SetName(histo);
	      
	  dummyname = histo1[p]->GetName();
	       
	  dummyname.Replace(0,9," ");
	  dummyname.ReplaceAll("_EffectiveCharge_hw", "");
	  PosVariation[0] -> SetBinContent(xcount, histo1[p]->GetMean()*1e6);
	  PosVariation[0] -> SetBinError  (xcount, histo1[p]->GetMeanError()*1e6);//this gives std deviation not RMS
	  PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	  PosVariation[1] -> SetBinContent(xcount, histo1[p]->GetRMS()*1e6);
	  PosVariation[1] -> SetBinError  (xcount, 0);
	  PosVariation[1] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	  
	  if(ldebug) SummaryHist(histo1[p]);

	}	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("Eff_Charge Asymmetry");
    PosVariation[0] -> GetYaxis() -> SetTitle("ppm");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("Eff_Charge Asymmetry Width");
    PosVariation[1] -> GetYaxis()-> SetTitle ("ppm");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;
}

void QwGUIHallCBeamline::PlotChargeAsym()
{
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  
  SetHistoDefaultMode();//bring the histo mode to accumulate mode

   while (1){
     if (fCurrentBCMIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s_hw",fHallCDevices.at(VQWK_BCM).at(fCurrentBCMIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s_hw",fHallCDevices.at(VQWK_BCM).at(fCurrentBCMIndex).Data() );
       histo2= (TH1F *)dMapFile->Get(histo);
     }
    
    if (histo1!=NULL && histo2!=NULL ) {
      if (GetHistoReset()){
	histo1_buff=(TH1F*)histo1->Clone(Form("%s_buff",histo1->GetName()));
	*histo1=*histo1-*histo1_buff;
	histo2_buff=(TH1F*)histo2->Clone(Form("%s_buff",histo2->GetName()));
	*histo2=*histo2-*histo2_buff;
	SetHistoReset(0);
      }else if (GetHistoAccumulate()==0){
	*histo1=*histo1-*histo1_buff;
	*histo2=*histo2-*histo2_buff;
      }
      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      mc->cd(2);
      histo2->Draw();
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   //   printf("---------------PlotChargeAsym()--------------------\n");
  //mc->Modified();
  //mc->Update();
  

  return;
}


void QwGUIHallCBeamline::PlotBPMAsym(){

  char chisto1[150];
  char chisto2[150]; 
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t BPMSTriplinesCount = fHallCDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  
  std::vector<TH1F *> histo1;//_array;
  histo1.resize(BPMSTriplinesCount);
  std::vector<TH1F *> histo2;//_array;
  histo2.resize(BPMSTriplinesCount);

  std::vector<TH1F *> histo1_buff;//_array;
  histo1_buff.resize(BPMSTriplinesCount);
  std::vector<TH1F *> histo2_buff;//_array;
  histo2_buff.resize(BPMSTriplinesCount);

  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 
  SetHistoDefaultMode();//bring the histo mode to accumulate mode

   while (1){ 
     PosVariation[0] = new TH1F("HCDiffX", "X Difference Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariation[1] = new TH1F("HCDiffY", "Y Difference variation", BPMSTriplinesCount, min_range, max_range); 
    for(Int_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      if (GetHistoPause()==0){
	sprintf (chisto1, "diff_%sX_hw",fHallCDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() );
	histo1[p] = (TH1F *)dMapFile->Get(chisto1); 
	sprintf (chisto2, "diff_%sY_hw", fHallCDevices.at(VQWK_BPMSTRIPLINE).at(p).Data()); 
	histo2[p] = (TH1F *)dMapFile->Get(chisto2); 
      }

      if (histo1[p]!=NULL && histo2[p]!=NULL) {
	if (GetHistoReset()){
	  histo1_buff[p]=(TH1F*)histo1[p]->Clone(Form("%s_buff",histo1[p]->GetName()));
	  *histo1[p]=*histo1[p]-*histo1_buff[p];
	  histo2_buff[p]=(TH1F*)histo2[p]->Clone(Form("%s_buff",histo2[p]->GetName()));
	  *histo2[p]=*histo2[p]-*histo2_buff[p];
	  if (p==BPMSTriplinesCount-1)//once all histo are buffered set the reser state
	    SetHistoReset(0);
	}else if (GetHistoAccumulate()==0){
	  *histo1[p]=*histo1[p]-*histo1_buff[p];
	  *histo2[p]=*histo2[p]-*histo2_buff[p];
	}	    
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, chisto1);
	histo1[p]->SetName(chisto1);
	    
	dummyname = histo1[p]->GetName();
	    
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1[p]->GetMean());
	PosVariation[0] -> SetBinError(xcount, histo1[p]->GetMeanError());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);

	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, chisto2);
	histo2[p]->SetName(chisto2);
	dummyname = histo2[p]->GetName();
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2[p]->GetMean());
	PosVariation[1] -> SetBinError(ycount, histo2[p]->GetMeanError());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);

      }
	  
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("X Difference Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("mm");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("Y Difference Variation");
    PosVariation[1] -> GetYaxis()-> SetTitle ("mm");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM Y");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;



}

void QwGUIHallCBeamline::PlotBPMPositions(){

  char chisto1[128];
  char chisto2[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t BPMSTriplinesCount = fHallCDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  std::vector<TH1F *> histo1;//_array;
  histo1.resize(BPMSTriplinesCount);
  std::vector<TH1F *> histo2;//_array;
  histo2.resize(BPMSTriplinesCount);

  std::vector<TH1F *> histo1_buff;//_array;
  histo1_buff.resize(BPMSTriplinesCount);
  std::vector<TH1F *> histo2_buff;//_array;
  histo2_buff.resize(BPMSTriplinesCount);
  


  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 
  SetHistoDefaultMode();//bring the histo mode to accumulate mode

  while (1){ 
     PosVariation[0] = new TH1F("PosX", "Mean X Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariation[1] = new TH1F("PosY", "Mean Y variation", BPMSTriplinesCount, min_range, max_range); 
     for(Int_t p = 0; p <BPMSTriplinesCount ; p++) {

       if (GetHistoPause()==0){
	 sprintf (chisto1, "%sX_hw",fHallCDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() );
	 histo1[p]= (TH1F *)dMapFile->Get(chisto1); 
	 sprintf (chisto2, "%sY_hw", fHallCDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
	 histo2[p]= (TH1F *)dMapFile->Get(chisto2);
       }

      if (histo1[p]!=NULL && histo2[p]!=NULL) {
	if (GetHistoReset()){
	  histo1_buff[p]=(TH1F*)histo1[p]->Clone(Form("%s_buff",histo1[p]->GetName()));
	  *histo1[p]=*histo1[p]-*histo1_buff[p];
	  histo2_buff[p]=(TH1F*)histo2[p]->Clone(Form("%s_buff",histo2[p]->GetName()));
	  *histo2[p]=*histo2[p]-*histo2_buff[p];
	  if (p==BPMSTriplinesCount-1)//once all histo are buffered set the reser state
	    SetHistoReset(0);
	}else if (GetHistoAccumulate()==0){
	  *histo1[p]=*histo1[p]-*histo1_buff[p];
	  *histo2[p]=*histo2[p]-*histo2_buff[p];
	}	    
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, chisto1);
	histo1[p]->SetName(chisto1);
	    
	dummyname = histo1[p]->GetName();
	    
	dummyname.Replace(0,4," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1[p]->GetMean());
	PosVariation[0] -> SetBinError(xcount, histo1[p]->GetMeanError());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);

	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, chisto2);
	histo2[p]->SetName(chisto2);
	dummyname = histo2[p]->GetName();
	dummyname.Replace(0,4," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2[p]->GetMean());
	PosVariation[1] -> SetBinError(ycount, histo2[p]->GetMeanError());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
      }

	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("Mean BPM X Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("Pos (mm)");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("Mean BPM Y Variation");
    PosVariation[1] -> GetYaxis()-> SetTitle ("Pos (mm)");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM Y");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;

};
  
  
  
void QwGUIHallCBeamline::PlotSCALER(){
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram

  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode

  while (1){
     if (fCurrentSCALERIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s",fHallCDevices.at(SCALER_HALO).at(fCurrentSCALERIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s",fHallCDevices.at(SCALER_HALO).at(fCurrentSCALERIndex).Data() );
       histo2= (TH1F *)dMapFile->Get(histo);
     }
    
    if (histo1!=NULL && histo2!=NULL ) {
      if (GetHistoReset()){
	histo1_buff=(TH1F*)histo1->Clone(Form("%s_buff",histo1->GetName()));
	*histo1=*histo1-*histo1_buff;
	histo2_buff=(TH1F*)histo2->Clone(Form("%s_buff",histo2->GetName()));
	*histo2=*histo2-*histo2_buff;
	SetHistoReset(0);
      }else if (GetHistoAccumulate()==0){
	*histo1=*histo1-*histo1_buff;
	*histo2=*histo2-*histo2_buff;
      }
    
      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      mc->cd(2);
      histo2->Draw();
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

  //   printf("---------------PlotSCALER()--------------------\n");
  //mc->Modified();
  //mc->Update();
  

  return;
};  

void QwGUIHallCBeamline::PlotTargetPos(Int_t tgtcoord){
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  
  char histon1[128]; //name of the histogram
  char histon2[128]; //name of the histogram

  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode

   while (1){
     switch (tgtcoord)
       {
       case BA_TGT_X:
	 sprintf (histon1, "diff_qwk_targetX_hw" );
	 sprintf (histon2, "yield_qwk_targetX_hw" );
	 break;
       case BA_TGT_Y:
	 sprintf (histon1, "diff_qwk_targetY_hw" );
	 sprintf (histon2, "yield_qwk_targetY_hw" );
	 break;
       case BA_TGT_ANGLE_X:
	 sprintf (histon1, "diff_qwk_targetXSlope_hw" );
	 sprintf (histon2, "yield_qwk_targetXSlope_hw" );
	 break;
       case BA_TGT_ANGLE_Y:
	 sprintf (histon1, "diff_qwk_targetYSlope_hw" );
	 sprintf (histon2, "yield_qwk_targetYSlope_hw" );
	 break;
       }

     if (GetHistoPause()==0){
       histo1= (TH1F *)dMapFile->Get(histon1);
       histo2= (TH1F *)dMapFile->Get(histon2);
     }
     
    
    if (histo1!=NULL && histo2!=NULL ) {
      if (GetHistoReset()){
	histo1_buff=(TH1F*)histo1->Clone(Form("%s_buff",histo1->GetName()));
	*histo1=*histo1-*histo1_buff;
	histo2_buff=(TH1F*)histo2->Clone(Form("%s_buff",histo2->GetName()));
	*histo2=*histo2-*histo2_buff;
	SetHistoReset(0);
      }else if (GetHistoAccumulate()==0){
	*histo1=*histo1-*histo1_buff;
	*histo2=*histo2-*histo2_buff;
      }

      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      mc->cd(2);
      histo2->Draw();
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   //   printf("---------------PlotTargetPos(%i)--------------------\n",tgtcoord);
   //mc->Modified();
   //mc->Update();
  

  return;
};

void QwGUIHallCBeamline::PlotTargetCharge(){
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram

  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode
  

   while (1){
     if (GetHistoPause()==0){
       sprintf (histo, "asym_qwk_charge_hw" );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_qwk_charge_hw" );
       histo2= (TH1F *)dMapFile->Get(histo); 
     }
    
    if (histo1!=NULL && histo2!=NULL ) {
      
      if (GetHistoReset()){
	histo1_buff=(TH1F*)histo1->Clone(Form("%s_buff",histo1->GetName()));
	*histo1=*histo1-*histo1_buff;
	histo2_buff=(TH1F*)histo2->Clone(Form("%s_buff",histo2->GetName()));
	*histo2=*histo2-*histo2_buff;
	SetHistoReset(0);
      }else if (GetHistoAccumulate()==0){
	*histo1=*histo1-*histo1_buff;
	*histo2=*histo2-*histo2_buff;
      }
      
      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      mc->cd(2);
      histo2->Draw();
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(fSleepTimeMS);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
   

   //   printf("---------------PlotTargetCharge() %d--------------------\n",GetHistoAccumulate());
   return;
};



void QwGUIHallCBeamline::PlotFastRaster()
{
  TH2D *histo1=NULL;
  TH2D *histo1_buff=NULL; 
  char histo[128]; //name of the histogram
  TCanvas *mc        = NULL;

  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode
   

  gStyle->SetPalette(1,0);
  while (1)
    {
      if (GetHistoPause()==0){
       sprintf (histo, "raster_rate_map" );
       histo1= (TH2D*) dMapFile->Get("raster_rate_map");
      }
 
      
      if(histo1) {
	if (GetHistoReset()){
	  histo1_buff=(TH2D*)histo1->Clone(Form("%s_buff",histo1->GetName()));
	  *histo1=*histo1-*histo1_buff;
	  SetHistoReset(0);
	}else if (GetHistoAccumulate()==0){
	  *histo1=*histo1-*histo1_buff;
	}
	mc->Clear();
	gPad->SetGridx();
	gPad->SetGridy();
	histo1->Draw();
      }
      else {
	mc->Clear();
	std::cout << "there is no raster" << std::endl;
	break;
      }
      mc->Modified();
      mc->Update();
      gSystem->Sleep(fSleepTimeMS);
      if (gSystem->ProcessEvents()){
	break;
      }
    }
  return;
}



// void QwGUIHallCBeamline::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
// {

// }


Bool_t QwGUIHallCBeamline::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  if(dMapFileFlag){
  // Process events generated by the object in the frame.
  
  switch (GET_MSG(msg))
    {
    case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) 
	{
	case kTE_ENTER:
	  switch (parm1) 
	    {
	    default:
	      break;
	    }
	default:
	  break;
	}
      
    case kC_COMMAND:
      switch (GET_SUBMSG(msg))
	  {
	  case kCM_BUTTON:
	    {
	      switch(parm1)
		{
		case   BA_POS_DIFF:
		  //printf("text button id %ld pressed\n", parm1);		  
		  PlotBPMAsym();
		  break;
		  
		case BA_CHARGE:
		  
		  //printf("text button id %ld pressed\n", parm1);
		  PlotChargeAsym();//PlotPositionDiff(); 
		  break;

		case BA_POS_VAR:
		  //printf("PlotPosData() button id %ld pressed\n", parm1);
		  PositionDifferences();
		  break;

		case BA_HCSCALER:
		  //printf("PlotPosData() button id %ld pressed\n", parm1);
		  PlotSCALER();
		  break;

		case BA_MEAN_POS:
		  //printf("PlotPosData() button id %ld pressed\n", parm1);
		  PlotBPMPositions();
		  break;
		case BA_TGT_X:
		  PlotTargetPos(BA_TGT_X);
		  break;
		case BA_TGT_Y:
		  PlotTargetPos(BA_TGT_Y);
		  break;
		case BA_TGT_ANGLE_X:
		  PlotTargetPos(BA_TGT_ANGLE_X);
		  break;
		case BA_TGT_ANGLE_Y:
		  PlotTargetPos(BA_TGT_ANGLE_Y);
		  break;
		case BA_TGT_CHG:
		  PlotTargetCharge();
		  break;
		case BA_TGT_RASTER:
		  PlotFastRaster();
		  break;
		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
	      case CMB_HCBCM:
		SetComboIndex(CMB_HCBCM,parm2);
		break;
	      case CMB_HCSCALER:
		SetComboIndex(CMB_HCSCALER,parm2);
		break;
	      }
	    }
	    break;
	    
	}
    
      default:
	break;
    }
  }
  return kTRUE;
}


void QwGUIHallCBeamline::LoadHCBCMCombo(){
  dComboBoxHCBCM->RemoveAll();
  //printf("QwGUIHallCBeamline::LoadHCBCMCombo \n");
  for(Size_t i=0;i<fHallCDevices.at(VQWK_BCM).size();i++){
    dComboBoxHCBCM->AddEntry(fHallCDevices.at(VQWK_BCM).at((Int_t)i), (Int_t) i);
    //printf("%s \n",fHallCDevices.at(VQWK_BCM).at(i).Data());
  }
  if (fHallCDevices.at(VQWK_BCM).size()>0)
    dButtonCharge->SetEnabled(kTRUE);
  fCurrentBCMIndex=-1;
};

void QwGUIHallCBeamline::LoadHCSCALERCombo(){
  dComboBoxHCSCALER->RemoveAll();
  //printf("QwGUIHallCBeamline::LoadHCSCALERCombo \n");
  for(Size_t i=0;i<fHallCDevices.at(SCALER_HALO).size();i++){
    
    dComboBoxHCSCALER->AddEntry(fHallCDevices.at(SCALER_HALO).at((Int_t)i), (Int_t) i);
    //printf("%s \n",fHallCDevices.at(SCALER_HALO).at(i).Data());
  }
  if (fHallCDevices.at(SCALER_HALO).size()>0)
    dButtonHCSCALER->SetEnabled(kTRUE);

  fCurrentSCALERIndex=-1;
};


void QwGUIHallCBeamline::SetComboIndex(Int_t cmb_id, Int_t id){
    if (cmb_id==CMB_HCBCM)
      fCurrentBCMIndex=id;
    //else
    //fCurrentBCMIndex=-1;

    if (cmb_id==CMB_HCSCALER)
      fCurrentSCALERIndex=id;
    //else
    //fCurrentSCALERIndex=-1;
}


// void 
// QwGUIHallCBeamline::SummaryHist(TH1 *in)
// {

//   Double_t out[4] = {0.0};
//   Double_t test   = 0.0;

//   out[0] = in -> GetMean();
//   out[1] = in -> GetMeanError();
//   out[2] = in -> GetRMS();
//   out[3] = in -> GetRMSError();
//   test   = in -> GetRMS()/sqrt(in->GetEntries());

//   printf("%sName%s", BOLD, NORMAL);
//   printf("%22s", in->GetName());
//   printf("  %sMean%s%s", BOLD, NORMAL, " : ");
//   printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[0], NORMAL, BLUE, out[1], NORMAL);
//   printf("  %sSD%s%s", BOLD, NORMAL, " : ");
//   printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[2], NORMAL, GREEN, out[3], NORMAL);
//   printf(" %sRMS/Sqrt(N)%s %s%+4.2e%s \n", BOLD, NORMAL, BLUE, test, NORMAL);
//   return;
// };


