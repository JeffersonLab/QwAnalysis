#include "QwGUILumiDetector.h"
//#include <iostream>
#include "TLine.h"
#include "TGaxis.h"

ClassImp(QwGUILumiDetector);

const Int_t QwGUILumiDetector::fSleepTimeMS = 2000;

enum QwGUILumiDetectorIndentificator {
  BA_DS_LUMI,
  BA_US_LUMI,
  BA_LUMI,
  BA_SCALER,
 
};
enum QwGUILumiDetectorComboBox {
  CMB_LUMI,
  CMB_SCALER,
};

//LUMI_DET_TYPES is the size of the enum
enum QwGUIHallCBeamlineDeviceTypes {
  UNKNOWN_TYPE,
  VQWK_LUMI,
  SCALER_LUMI,
  DS_LUMI,
  US_LUMI,
  SCALER_DS_LUMI,
  SCALER_US_LUMI,
};
const char *QwGUILumiDetector::LumiDetectorHists[LUMI_DET_HST_NUM] = 
  { "C20AXM_hw_raw","C20AXM_block2_raw",
    "AmpCh1_block1_raw","H00_YP_block1_raw"};

QwGUILumiDetector::QwGUILumiDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab, const char *objName, const char *mainname, UInt_t w, UInt_t h)

  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dBtnLayout = NULL;

  dButtonUser = NULL;
  dButtonDetail = NULL;

  dButtonUSLumi = NULL;
  dButtonDSLumi = NULL;
  dButtonLumiAccess = NULL;
  dButtonScalerAccess = NULL;
  dLumiFrame  = NULL;
  dSCALERFrame  = NULL;
  dComboBoxLUMI  = NULL;
  dComboBoxSCALER  = NULL;

  PosVariation[0] = NULL;
  PosVariation[1] = NULL;

  AddThisTab(this);

  SetHistoAccumulate(1);
  SetHistoReset(0);


}

QwGUILumiDetector::~QwGUILumiDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;
  if(dBtnLayout) delete dBtnLayout;
  if(dButtonUSLumi) delete dButtonUSLumi;
  if(dButtonDSLumi) delete dButtonDSLumi;
  if(dButtonLumiAccess) delete dButtonLumiAccess;
  if(dButtonScalerAccess) delete dButtonScalerAccess;
  if(dLumiFrame) delete dLumiFrame;
  if(dSCALERFrame) delete dSCALERFrame;
  if(dComboBoxLUMI) delete dComboBoxLUMI;
  if(dComboBoxSCALER) delete dComboBoxSCALER;

  // RemoveThisTab(this);
  // IsClosing(GetName());
}

void QwGUILumiDetector::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);
  dBtnLayout = new TGLayoutHints(kLHintsExpandX | kLHintsTop,
				    5,5,5,5);



  dTabFrame = new TGHorizontalFrame(this);
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  // TGVertical3DLine *separator = new TGVertical3DLine(this);
  // dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));


  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

  //hall c lumi access frame
  dLumiFrame= new TGVerticalFrame(dControlsFrame,50,100);
  //hall c lumi scaler access frame
  dSCALERFrame= new TGVerticalFrame(dControlsFrame,50,100);

  dButtonLumiAccess=new TGTextButton(dLumiFrame, "&Lumis Asym/Yield", BA_LUMI);
  dButtonLumiAccess->SetEnabled(kFALSE);
  dButtonScalerAccess=new TGTextButton(dSCALERFrame, "&Scaler Asym/Yield", BA_SCALER);
  dButtonScalerAccess->SetEnabled(kFALSE);
  

  dButtonUSLumi=new TGTextButton(dControlsFrame, "&US Lumis Means", BA_US_LUMI);
  dButtonDSLumi=new TGTextButton(dControlsFrame, "&DS Lumis Means", BA_DS_LUMI);

  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  

  dControlsFrame->AddFrame(dButtonUSLumi,dBtnLayout );
  dControlsFrame->AddFrame(dButtonDSLumi,dBtnLayout );
  dControlsFrame->AddFrame(dLumiFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dSCALERFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  

  //Add LUMI  combo box
  dComboBoxLUMI=new TGComboBox(dLumiFrame,CMB_LUMI);
  dComboBoxLUMI->Resize(100,20);//To make it better looking

  //Add SCALER  combo box
  dComboBoxSCALER=new TGComboBox(dSCALERFrame,CMB_SCALER);
  dComboBoxSCALER->Resize(100,20);//To make it better looking
  

  dLumiFrame->AddFrame(dComboBoxLUMI, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dLumiFrame->AddFrame(dButtonLumiAccess, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dSCALERFrame->AddFrame(dComboBoxSCALER, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 10, 10, 5, 5));
  dSCALERFrame->AddFrame(dButtonScalerAccess, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

 
  dButtonUSLumi -> Associate(this);
  dButtonDSLumi -> Associate(this);
  dButtonLumiAccess -> Associate(this);
  dButtonScalerAccess -> Associate(this);
  dComboBoxLUMI -> Associate(this);
  dComboBoxSCALER -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  // dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  // 				"QwGUIHallCBeamline",
  // 				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

}

void QwGUILumiDetector::LoadHistoMapFile(TString mapfile)
{ 
  TString varname, varvalue;
  TString modtype, namech,dettype;
  //  Int_t count_names;
  fLUMIDevices.resize(LUMI_DET_TYPES);
  QwParameterFile mapstr(mapfile.Data());  //Open the file
  //fLUMIDevices.clear();
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    /*
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname=="name"){
	//printf("%s - %s \n",varname.Data(),varvalue.Data());
	fLUMIDevices.at(COMBINED).push_back(varvalue);
      }
    }
    else
    */{

      modtype   = mapstr.GetTypedNextToken<TString>();	// module type
      if (modtype=="VQWK" || modtype=="SCALER"){
	mapstr.GetTypedNextToken<TString>();	//slot number
	mapstr.GetTypedNextToken<TString>();	//channel number
	dettype=mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	namech.ToLower();

	if (dettype=="integrationpmt"){
	  //printf("%s - %s \n",modtype.Data(),namech.Data() );
	  fLUMIDevices.at(VQWK_LUMI).push_back(namech);
	  if (namech.Contains("uslumi")){
	    //printf("uslumi %s - %s \n",modtype.Data(),namech.Data() );
	    fLUMIDevices.at(US_LUMI).push_back(namech);
	  }
	  if (namech.Contains("dslumi")){
	    //printf("dslumi %s - %s \n",modtype.Data(),namech.Data() );
	    fLUMIDevices.at(DS_LUMI).push_back(namech);
	  }
	}
	else if (dettype=="scalerpmt"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fLUMIDevices.at(SCALER_LUMI).push_back(namech);
	  if (namech.Contains("uslumi")){
	    //printf("uslumi scaler %s - %s \n",modtype.Data(),namech.Data() );
	    fLUMIDevices.at(SCALER_US_LUMI).push_back(namech);
	  }
	  if (namech.Contains("dslumi")){
	    //printf("dslumi scaler %s - %s \n",modtype.Data(),namech.Data() );
	    fLUMIDevices.at(SCALER_DS_LUMI).push_back(namech);
	  }
	}
      }

    }

  }
  /*
  //printf("no. of hallC devices %d \n",fLUMIDevices.size());
   printf(" Hall C LUMI  Device List\n" );
  for (Size_t i=0;i<fLUMIDevices.size()-4;i++)
    for (Size_t j=0;j<fLUMIDevices.at(i).size();j++)
      printf("%s \n",fLUMIDevices.at(i).at(j).Data() );	 
  */
  LoadLUMICombo();//Load LUMI list into the combo box
  LoadSCALERCombo();//Load LUMI_SCALER list into the combo box


};

void QwGUILumiDetector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

// void QwGUILumiDetector::OnObjClose(char *obj)
// {
// }


// void QwGUILumiDetector::OnRemoveThisTab()
// {

// }



void QwGUILumiDetector::PlotUSLumi()
{
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  TString dummyname;

  Int_t LumiCount = 0;
  LumiCount = (Int_t) fLUMIDevices.at(US_LUMI).size();

  Double_t max_range = 0.0;

  max_range = (Double_t)LumiCount - offset ; 

  
  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     PosVariation[0] = new TH1F("Asym", "Asymmetry Variation",LumiCount, min_range, max_range);
     PosVariation[1] = new TH1F("Yield", "Yield variation",LumiCount , min_range, max_range); 
    for(Int_t p = 0; p <LumiCount ; p++) 
    {

      sprintf (histo, "asym_%s_hw",fLUMIDevices.at(US_LUMI).at(p).Data() );
      histo1= (TH1F *)dMapFile->Get(histo); 
      if (histo1!=NULL) {
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	histo1->SetName(histo);
	    
	dummyname = histo1->GetName();
	    
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1->GetMean());
	PosVariation[0] -> SetBinError  (xcount, histo1->GetRMS());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	if(ldebug)SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      sprintf (histo, "yield_%s_hw", fLUMIDevices.at(US_LUMI).at(p).Data());
      histo2= (TH1F *)dMapFile->Get(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,10," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
	PosVariation[1] -> SetBinError  (ycount, histo2->GetRMS());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	if(ldebug) SummaryHist(histo2);
	delete histo2; histo2= NULL; 
      }

	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("US LUMI Asymmetry Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("Asymmetry");
    PosVariation[0] -> GetXaxis() -> SetTitle("US LUMI");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("US LUMI Yield Variation");
    PosVariation[1] -> GetYaxis()-> SetTitle ("Yield");
    PosVariation[1] -> GetXaxis() -> SetTitle("US LUMI");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;



  //  printf("QwGUILumiDetector::PlotUSLumi() \n");
};

void QwGUILumiDetector::PlotDSLumi()
{
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  TString dummyname;

  Int_t LumiCount = fLUMIDevices.at(DS_LUMI).size();
  Double_t max_range = (Double_t)LumiCount - offset ; 

  


  

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     PosVariation[0] = new TH1F("Asym", "Asymmetry Variation",LumiCount, min_range, max_range);
     PosVariation[1] = new TH1F("Yield", "Yield variation",LumiCount , min_range, max_range); 
    for(Int_t p = 0; p <LumiCount ; p++) 
    {

      sprintf (histo, "asym_%s_hw",fLUMIDevices.at(DS_LUMI).at(p).Data() );
      histo1= (TH1F *)dMapFile->Get(histo); 
      if (histo1!=NULL) {
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	histo1->SetName(histo);
	    
	dummyname = histo1->GetName();
	    
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1->GetMean());
	PosVariation[0] -> SetBinError  (xcount, histo1->GetRMS());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	if(ldebug) SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      sprintf (histo, "yield_%s_hw", fLUMIDevices.at(DS_LUMI).at(p).Data());
      histo2= (TH1F *)dMapFile->Get(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,10," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
	PosVariation[1] -> SetBinError  (ycount, histo2->GetRMS());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	if(ldebug) SummaryHist(histo2);
	delete histo2; histo2= NULL; 
      }

	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("DS LUMI Asymmetry Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("Asymmetry");
    PosVariation[0] -> GetXaxis() -> SetTitle("DS LUMI");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("DS LUMI Yield Variation");
    PosVariation[1] -> GetYaxis()-> SetTitle ("Yield");
    PosVariation[1] -> GetXaxis() -> SetTitle("DS LUMI");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;

};

void QwGUILumiDetector::SetComboIndex(Int_t cmb_id, Int_t id)
{
    if (cmb_id==CMB_LUMI)
      fCurrentLUMIIndex=id;

    if (cmb_id==CMB_SCALER)
      fCurrentSCALERIndex=id;
};

void QwGUILumiDetector::LoadLUMICombo()
{

  dComboBoxLUMI->RemoveAll();
  //  printf("QwGUILumiDetector::LoadHCBCMCombo \n");
  std::size_t i = 0;
  for(i=0;i<fLUMIDevices.at(VQWK_LUMI).size();i++){
    dComboBoxLUMI->AddEntry(fLUMIDevices.at(VQWK_LUMI).at(i),i);
    //printf("%s \n",fLUMIDevices.at(VQWK_LUMI).at(i).Data());
  }
  if (fLUMIDevices.at(VQWK_LUMI).size()>0)
    dButtonLumiAccess->SetEnabled(kTRUE);
  fCurrentLUMIIndex=-1;  
};

void QwGUILumiDetector::LoadSCALERCombo()
{
  dComboBoxSCALER->RemoveAll();
  //  printf("QwGUILumiDetector::LoadHCBCMCombo \n");
  std::size_t i = 0;
  for(i=0;i<fLUMIDevices.at(SCALER_LUMI).size();i++){
    dComboBoxSCALER->AddEntry(fLUMIDevices.at(SCALER_LUMI).at(i),i);
    //printf("%s \n",fLUMIDevices.at(VQWK_LUMI).at(i).Data());
  }
  if (fLUMIDevices.at(SCALER_LUMI).size()>0)
    dButtonScalerAccess->SetEnabled(kTRUE);
  fCurrentSCALERIndex=-1;    
};


void QwGUILumiDetector::PlotLumi()
{
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode


  Bool_t ldebug = false;

   while (1){
     if (fCurrentLUMIIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s_hw",fLUMIDevices.at(VQWK_LUMI).at(fCurrentLUMIIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s_hw",fLUMIDevices.at(VQWK_LUMI).at(fCurrentLUMIIndex).Data() );
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
      if(ldebug) SummaryHist(histo1);
      mc->cd(2);
      histo2->Draw();
      if(ldebug) SummaryHist(histo2);
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   printf("---------------PlotLUMI()--------------------\n");
  //mc->Modified();
  //mc->Update(); 
};

void QwGUILumiDetector::PlotLumiScaler()
{
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  Bool_t ldebug = false;
  SetHistoDefaultMode();//bring the histo mode to accumulate mode

   while (1){
     if (fCurrentSCALERIndex<0)
       break;
      if (GetHistoPause()==0){
	sprintf (histo, "asym_%s",fLUMIDevices.at(SCALER_LUMI).at(fCurrentSCALERIndex).Data() );
	histo1= (TH1F *)dMapFile->Get(histo);
	sprintf (histo, "yield_%s",fLUMIDevices.at(SCALER_LUMI).at(fCurrentSCALERIndex).Data() );
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
      if(ldebug) SummaryHist(histo1);
      mc->cd(2);
      histo2->Draw();
      if(ldebug) SummaryHist(histo2);
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   //   printf("---------------PlotLumiScaler()--------------------\n");  
};

// void QwGUILumiDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
// {
//   // if(event == kButton1Double){
//   //   Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
//   //   if(pad > 0 && pad <= LUMI_DET_HST_NUM)
//   //     {
//   // 	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
//   // 						     GetNewWindowName(),"QwGUILumiDetector",
//   // 						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
//   // 	if(!dMiscWindow){
//   // 	  return;
//   // 	}
//   // 	DataWindowArray.Add(dMiscWindow);
//   // 	dMiscWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
//   // 	dMiscWindow->DrawData(*((TH1D*)HistArray[pad-1]));
//   // 	SetLogMessage(Form("Looking at %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);

//   // 	return;
//   //     }
//   // }
// }


Bool_t QwGUILumiDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  if(dMapFileFlag){
  switch (GET_MSG(msg)){

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_ENTER:
      switch (parm1) {

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
	    case BA_DS_LUMI:
	      PlotDSLumi();
	      break;
	    case BA_US_LUMI:
	      PlotUSLumi();
	      break;
	    case BA_LUMI:
	      PlotLumi();
	      break;
	    case BA_SCALER:
	      PlotLumiScaler();
	      break;
	    }
	  
	  break;
	}
	
      case kCM_COMBOBOX:
	{
	  switch (parm1) {
	    case CMB_LUMI:
	      SetComboIndex(CMB_LUMI,parm2);
	      break;
	    case CMB_SCALER:
	      SetComboIndex(CMB_SCALER,parm2);
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


// void QwGUILumiDetector::SummaryHist(TH1 *in)
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

