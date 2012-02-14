#include "QwGUIMainDetector.h"

ClassImp(QwGUIMainDetector);

const Int_t QwGUIMainDetector::fSleepTimeMS = 2000;

//MD_DET_TYPES is the size of the enum
enum QwGUIMainDetectorDeviceTypes {
  UNKNOWN_TYPE,
  VQWK,
  VPMT,  
};


enum QwGUIMainDetectorBTNIndentificator {
  BA_MD_16,
  BA_MD_BKG,
  BA_MD_CMB,
  BA_MD_PMT,
  BA_MD_VPMT,
  BA_MD_CMNS,
};

//Combo box
enum QwGUIMainDetectorComboBox {
  CMB_MDPMT,
  CMB_MDVPMT,
};

QwGUIMainDetector::QwGUIMainDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame        = NULL;
  dCanvas          = NULL;  
  dControlsFrame   = NULL;  
  dMDPMTFrame      = NULL;
  dMDVPMTFrame     = NULL;
  dTabLayout       = NULL;
  dCnvLayout       = NULL;
  //  dNumberEntryDlg  = NULL;
  dButtonMD16      = NULL;
  dButtonMDBkg     = NULL;
  dButtonMDCmb     = NULL;
  dButtonMDPMT     = NULL;
  dButtonMDVPMT    = NULL;
  dButtonMDCommonNoise = NULL;

  dComboBoxMDPMT   = NULL;
  dComboBoxMDVPMT  = NULL;

  MDPlots[0] = NULL;
  MDPlots[1] = NULL;
  fMDDevices.clear();

  AddThisTab(this);

  fCurrentPMTIndex = 0;
  fCurrentVPMTIndex = 0;
  
  //  ClearData();

  SetHistoAccumulate(1);
  SetHistoReset(0);

}

QwGUIMainDetector::~QwGUIMainDetector()
{
  if(dTabFrame)       delete dTabFrame;
  if(dCanvas)         delete dCanvas;  
  if(dTabLayout)      delete dTabLayout;
  if(dCnvLayout)      delete dCnvLayout;
  if(dControlsFrame)  delete dControlsFrame;
  if(dMDPMTFrame)     delete dMDPMTFrame;
  if(dMDVPMTFrame)    delete dMDVPMTFrame;
  if(dButtonMD16)     delete dButtonMD16;
  if(dButtonMDBkg)    delete dButtonMDBkg;
  if(dButtonMDCmb)    delete dButtonMDCmb;
  if(dButtonMDPMT)    delete dButtonMDPMT;
  if(dComboBoxMDVPMT) delete dComboBoxMDVPMT;
  if(dButtonMDCommonNoise) delete dButtonMDCommonNoise;
  if(dButtonMDVPMT)   delete dButtonMDVPMT;
  if(dComboBoxMDPMT)  delete dComboBoxMDPMT;

  delete [] MDPlots;
  //  RemoveThisTab(this);
  //  IsClosing(GetName());
  //  ClearData();
}

void QwGUIMainDetector::MakeLayout()
{
  SetCleanup(kDeepCleanup);
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);

  dTabFrame = new TGHorizontalFrame(this);
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  //TGVertical3DLine *separator = new TGVertical3DLine(this);
  //dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));
  
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200);     
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

  //Add PMT and Comb PMT frames
  dMDPMTFrame = new TGVerticalFrame(dControlsFrame,50,100);
  dMDVPMTFrame = new TGVerticalFrame(dControlsFrame,50,100);

  dButtonMD16 = new TGTextButton(dControlsFrame, "&MD Plots", BA_MD_16);
  dButtonMDBkg  = new TGTextButton(dControlsFrame, "&Aux MD Plots", BA_MD_BKG);
  dButtonMDCmb  = new TGTextButton(dControlsFrame, "&Comb. MD Plots", BA_MD_CMB);

  //add components to PMT frame
  dButtonMDPMT = new TGTextButton(dMDPMTFrame, "&PMT Plot", BA_MD_PMT);
  dButtonMDPMT->SetEnabled(kFALSE);
  dComboBoxMDPMT = new TGComboBox(dMDPMTFrame,CMB_MDPMT);
  dComboBoxMDPMT->Resize(120,20);//To make it better looking

  //add components toV PMT frame
  dButtonMDVPMT = new TGTextButton(dMDVPMTFrame, "&VPMT Plots", BA_MD_VPMT);
  dButtonMDVPMT->SetEnabled(kFALSE);
  dComboBoxMDVPMT = new TGComboBox(dMDVPMTFrame,CMB_MDVPMT);
  dComboBoxMDVPMT->Resize(120,20);//To make it better looking

  dButtonMDCommonNoise = new TGTextButton(dControlsFrame, "Common Mode &Noise", BA_MD_CMNS);

  dControlsFrame->AddFrame(dButtonMD16, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));
  dControlsFrame->AddFrame(dButtonMDBkg, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));
  dControlsFrame->AddFrame(dButtonMDCmb, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));
  dControlsFrame->AddFrame(dMDPMTFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dMDVPMTFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  dMDPMTFrame->AddFrame(dComboBoxMDPMT, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dMDPMTFrame->AddFrame(dButtonMDPMT, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  dMDVPMTFrame->AddFrame(dComboBoxMDVPMT, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dMDVPMTFrame->AddFrame(dButtonMDVPMT, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));  

  dControlsFrame->AddFrame(dButtonMDCommonNoise, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));

  dButtonMD16->Associate(this);
  dButtonMDBkg->Associate(this);
  dButtonMDCmb->Associate(this);
  dButtonMDPMT->Associate(this);
  dButtonMDVPMT->Associate(this);
  dButtonMDCommonNoise->Associate(this);
  dComboBoxMDPMT->Associate(this);
  dComboBoxMDVPMT->Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);

};

void QwGUIMainDetector::LoadHistoMapFile(TString mapfile){  //Now this is called at the QwGUIMain
  
  TString varname, varvalue;
  TString modtype, namech,dettype;
  fMDDevices.resize(MD_DET_TYPES);
  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //ignore
    }
    else{
      modtype   = mapstr.GetTypedNextToken<TString>();	// module type
      if (modtype=="VQWK" || modtype=="VPMT"){
	mapstr.GetTypedNextToken<TString>();	//slot number
	mapstr.GetTypedNextToken<TString>();	//channel number
	dettype=mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	namech.ToLower();

	if (dettype=="integrationpmt"){
	  //printf("%s - %s \n",modtype.Data(),namech.Data() );	  
	  fMDDevices.at(VQWK).push_back(namech);
	}
	else if (dettype=="combinationpmt"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fMDDevices.at(VPMT).push_back(namech);
	}
	

      }

    }

  }
  /*
  printf(" MainDetector Device List\n" );	 
  for (Size_t i=0;i<fMDDevices.size();i++)
    for (Size_t j=0;j<fMDDevices.at(i).size();j++)
      printf("%s \n",fMDDevices.at(i).at(j).Data() );
  */	
  LoadMDPMTCombo();
  LoadMDVPMTCombo();
};

void QwGUIMainDetector::DrawMDPMTPlots(){  
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode 

   while (1){
     if (fCurrentPMTIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s_hw",fMDDevices.at(VQWK).at(fCurrentPMTIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s_hw",fMDDevices.at(VQWK).at(fCurrentPMTIndex).Data() );
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

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  
  return;
};

void QwGUIMainDetector::DrawMDVPMTPlots(){
  TH1F *histo1=NULL;
  TH1F *histo1_buff=NULL; 
  TH1F *histo2=NULL;
  TH1F *histo2_buff=NULL; 
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode
 

   while (1){
     if (fCurrentVPMTIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s_hw",fMDDevices.at(VPMT).at(fCurrentVPMTIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s_hw",fMDDevices.at(VPMT).at(fCurrentVPMTIndex).Data() );
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

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  
  return;  
};


void QwGUIMainDetector::DrawMD16Plots(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Double_t max_range = (Double_t)MAIN_DET_INDEX - offset ; 

  
  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     MDPlots[0] = new TH1F("Asym", "Asymmetry Variation", MAIN_DET_INDEX, min_range, max_range);
     MDPlots[1] = new TH1F("Yield", "Yield variation", MAIN_DET_INDEX, min_range, max_range); 
    for(Short_t p = 0; p <MAIN_DET_INDEX ; p++) 
    {
      sprintf (histo, "asym_%s_hw",fMDDevices.at(VQWK).at(p).Data() );
      histo1= (TH1F *)dMapFile->Get(histo); 
      if (histo1!=NULL) {
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	histo1->SetName(histo);
	    
	dummyname = histo1->GetName();
	    
	dummyname.Replace(0,5," ");
	dummyname.ReplaceAll("_hw", "");
	MDPlots[0] -> SetBinContent(xcount, histo1->GetMean());
	MDPlots[0] -> SetBinError  (xcount, histo1->GetRMS());
	MDPlots[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	if(ldebug) SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      sprintf (histo, "yield_%s_hw", fMDDevices.at(VQWK).at(p).Data());
      histo2= (TH1F *)dMapFile->Get(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,6," ");
	dummyname.ReplaceAll("_hw", "");
	MDPlots[1] -> SetBinContent(ycount, histo2->GetMean());
	MDPlots[1] -> SetBinError  (ycount, histo2->GetRMS());
	MDPlots[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	if(ldebug) SummaryHist(histo2);
	delete histo2; histo2= NULL; 
      }
	  
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    MDPlots[0] -> SetMarkerStyle(20);
    MDPlots[0] -> SetStats(kFALSE);
    MDPlots[0] -> SetTitle("Asymmetry Variation");
    MDPlots[0] -> GetYaxis() -> SetTitle("Asymmetry");
    MDPlots[0] -> GetXaxis() -> SetTitle("MD");
    MDPlots[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    MDPlots[1] -> SetMarkerStyle(20);
    MDPlots[1] -> SetStats(kFALSE);
    MDPlots[1] -> SetTitle("Yield Variation");
    MDPlots[1] -> GetYaxis()-> SetTitle ("Yield");
    MDPlots[1] -> GetXaxis() -> SetTitle("MD");
    MDPlots[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Short_t p = 0; p <2 ; p++){
      delete MDPlots[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }
   

  return;

};

void QwGUIMainDetector::DrawMDBkgPlots(){
  /*
    It is assumed that in the map file 16 MD channels are in the beginning of the MD channel map.
    The Bkg channels are followd then in the map file
  */
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;
  
  Int_t MDBkgCount = (fMDDevices.at(VQWK).size() - MAIN_DET_INDEX);
  Double_t max_range = (Double_t)MDBkgCount - offset ;
  //printf("Bkg count %2d  mak range %f\n",MDBkgCount , max_range);
  if (max_range<min_range){
    printf("Histogram max/min range error \n");
    return;
  }

  
  // TH1D *dummyhist = NULL;

  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 
  std::size_t p = 0;

  while (1) { 
    MDPlots[0] = new TH1F("Asym", "Asymmetry Variation",MDBkgCount , min_range, max_range);
    MDPlots[1] = new TH1F("Yield", "Yield variation",MDBkgCount , min_range, max_range); 
    p = 0;
    for( p = MAIN_DET_INDEX; p <fMDDevices.at(VQWK).size(); p++) 
      {
	sprintf (histo, "asym_%s_hw",fMDDevices.at(VQWK).at(p).Data() );
	histo1= (TH1F *)dMapFile->Get(histo); 
	if (histo1!=NULL) {
	  xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	  if(ldebug) 
	    printf("Found %2d : a histogram name %22s\n", xcount, histo);
	  histo1->SetName(histo);
	    
	  dummyname = histo1->GetName();
	    
	  dummyname.Replace(0,5," ");
	  dummyname.ReplaceAll("_hw", "");
	  MDPlots[0] -> SetBinContent(xcount, histo1->GetMean());
	  MDPlots[0] -> SetBinError  (xcount, histo1->GetRMS());
	  MDPlots[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	  if(ldebug) SummaryHist(histo1);
	  delete histo1; histo1= NULL;
	}
	  
	sprintf (histo, "yield_%s_hw", fMDDevices.at(VQWK).at(p).Data());
	histo2= (TH1F *)dMapFile->Get(histo); 
	if(histo2!=NULL){		
	  ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	  if(ldebug) 
	    printf("Found %2d : a histogram name %22s\n", ycount, histo);
	  histo2->SetName(histo);
	  dummyname = histo2->GetName();
	  dummyname.Replace(0,6," ");
	  dummyname.ReplaceAll("_hw", "");
	  MDPlots[1] -> SetBinContent(ycount, histo2->GetMean());
	  MDPlots[1] -> SetBinError  (ycount, histo2->GetRMS());
	  MDPlots[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	  if(ldebug) SummaryHist(histo2);
	  delete histo2; histo2= NULL; 
	}
	  
	  
      }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    MDPlots[0] -> SetMarkerStyle(20);
    MDPlots[0] -> SetStats(kFALSE);
    MDPlots[0] -> SetTitle("Asymmetry Variation");
    MDPlots[0] -> GetYaxis() -> SetTitle("Asymmetry");
    MDPlots[0] -> GetXaxis() -> SetTitle("MD");
    MDPlots[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    MDPlots[1] -> SetMarkerStyle(20);
    MDPlots[1] -> SetStats(kFALSE);
    MDPlots[1] -> SetTitle("Yield Variation");
    MDPlots[1] -> GetYaxis()-> SetTitle ("Yield");
    MDPlots[1] -> GetXaxis() -> SetTitle("MD Bkg");
    MDPlots[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <2 ; p++){
      delete MDPlots[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
   

  return;

};

void QwGUIMainDetector::DrawMDCmbPlots()
{
  //gStyle->SetOptFit(1); 
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.2);

  Int_t xcount = 0;
  Int_t ycount = 0; 

  Double_t offset = 0.5;
  Double_t min_range = 4;
  
  Int_t MDCmbCount = fMDDevices.at(VPMT).size()-4;
  Double_t max_range = (Double_t)MDCmbCount- offset ; 

  TString dummyname;
  Bool_t ldebug = kFALSE;

  std::vector<TH1F *> histo1;//_array;
  histo1.resize(MDCmbCount);

  std::vector<TH1F *> histo2;//_array;
  histo2.resize(MDCmbCount);

  std::vector<TH1F *> histo1_buff;//_array;
  histo1_buff.resize(MDCmbCount);
  std::vector<TH1F *> histo2_buff;//_array;
  histo2_buff.resize(MDCmbCount);

  char histo[128];

  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  SetHistoDefaultMode();//bring the histo mode to accumulate mode

  // Fit
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
  TF1* fit=NULL;

  /// grp to draw md asymmetries
  TGraphErrors* grp = NULL;
  Double_t x[8]={1,2,3,4,5,6,7,8};
  Double_t xe[8]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  Double_t mean[8];
  Double_t error[8];


  while (1){ 
    // MDPlots[0] = new TH1F("Asymmetry", "Asymmetry Variation",MDCmbCount , min_range, max_range);
    //    MDPlots[1] = new TH1F("Yield", "Yield variation",MDCmbCount , min_range, max_range); 
    MDPlots[1] = new TH1F("RMS", "RMS Variation",8, -0.5, 7.5);
    
    for(Int_t p = 0; p <MDCmbCount ; p++) 
      {
	if (GetHistoPause()==0){
	  sprintf (histo, "asym_%s_hw", fMDDevices.at(VPMT).at(p).Data());
	  histo1[p]= (TH1F *)dMapFile->Get(histo); 
	}
	if (histo1[p]!=NULL)
	  {
	    if (GetHistoReset()){
	      histo1_buff[p]=(TH1F*)histo1[p]->Clone(Form("%s_buff",histo1[p]->GetName()));
	      *histo1[p]=*histo1[p]-*histo1_buff[p];
	      if (p==MDCmbCount-1)//once all histo are buffered set the reser state
		SetHistoReset(0);
	    }else if (GetHistoAccumulate()==0){
	      *histo1[p]=*histo1[p]-*histo1_buff[p];
	    }	    
	
	    xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	    if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	    histo1[p]->SetName(histo);
	      
	    dummyname = histo1[p]->GetName();
	       
	    dummyname.Replace(0,5," ");
	    dummyname.ReplaceAll("_hw", "");

	    mean[p]=histo1[p]->GetMean()*1e6; // ppm
	    error[p] = histo1[p]->GetMeanError()*1e6; //ppm

// 	    MDPlots[0] -> SetBinContent(xcount, histo1[p]->GetMean()*1e6);
// 	    MDPlots[0] -> SetBinError  (xcount, histo1[p]->GetMeanError()*1e6);//this gives std deviation not RMS
// 	    MDPlots[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	    MDPlots[1] -> SetBinContent(xcount, histo1[p]->GetRMS()*1e6);
	    MDPlots[1] -> SetBinError  (xcount, 0);
	    MDPlots[1] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	  
	    if(ldebug) SummaryHist(histo1[p]);

	  }	  
      }


    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);
    
    mc->cd(1);
    grp  = new TGraphErrors(8,x,mean,xe,error);
    grp->SetMarkerSize(1);
    grp->SetMarkerStyle(21);
    grp->Draw("AEP");
    grp-> Fit("cosfit","Q");
    fit =grp->GetFunction("cosfit");
    fit->SetLineColor(kBlue);
    grp->GetXaxis()->SetTitle("Octant");
    grp->GetXaxis()->CenterTitle();
    grp->GetYaxis()->SetTitle("MD Asymmetry (ppm)");
    grp->GetYaxis()->CenterTitle();
    grp->GetYaxis()->SetTitleOffset(0.7);
    grp->GetXaxis()->SetTitleOffset(0.8);
    grp->SetTitle("Md asymmetry variation");


    //    gPad->Update();
//     MDPlots[0] -> SetMarkerStyle(20);
//     MDPlots[0] -> SetStats(kFALSE);
//     MDPlots[0] -> SetTitle("Asymmetry Variation");
//     MDPlots[0] -> GetYaxis() -> SetTitle("(ppm)");
//     MDPlots[0] -> GetXaxis() -> SetTitle("MD");
//     MDPlots[0] -> Draw("E1");
//     MDPlots[0] -> Fit("cosfit","B");
//     fit =MDPlots[0]->GetFunction("cosfit");
//     fit->SetLineColor(kBlue);
  
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    MDPlots[1] -> SetMarkerStyle(20);
    MDPlots[1] -> SetStats(kFALSE);
    MDPlots[1] -> SetTitle("RMS Variation");
    MDPlots[1] -> GetYaxis()-> SetTitle ("ppm");
    MDPlots[1] -> GetXaxis() -> SetTitle("MD");
    MDPlots[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p < 2 ; p++){
      delete MDPlots[p];
    }

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
   
  return;

};

void QwGUIMainDetector::CalculateCommNoise(){
  printf("QwGUIHallCBeamline::CalculateCommNoise \n");

  TTree *tree=NULL;

  TH1F* md_pair_hist26      = NULL;
  TH1F* md_pair_hist48      = NULL;
  TH1F* md_all_even_hist    = NULL;
  TF1 * md_pair_hist26_fit  = NULL;
  TF1 * md_pair_hist48_fit  = NULL;
  TF1 * MDEvenAll_fit       = NULL;
  Double_t md_pair_hist26_sigma = 0.0;
  Double_t md_pair_hist48_sigma = 0.0;
  Double_t md_evenall_sigma     = 0.0;
  
  // Odd 
  TH1F* md_pair_hist15      = NULL;
  TH1F* md_pair_hist37      = NULL;
  TH1F* md_all_odd_hist     = NULL;
  TF1 * md_pair_hist15_fit  = NULL;
  TF1 * md_pair_hist37_fit  = NULL;
  TF1 * MDOddAll_fit        = NULL;
  Double_t md_pair_hist15_sigma = 0.0;
  Double_t md_pair_hist37_sigma = 0.0;
  Double_t md_oddall_sigma      = 0.0;

  //All
  TH1F* md_all_hist      = NULL;
  TF1 * MDAll_fit        = NULL;
  Double_t md_all_sigma = 0.0;
  
  //common mode noise parameters
  Double_t uncorrelated_width[3]      = {0.0};//0-even,1-odd,2-all pairs
  Double_t common_mode_noise[3]       = {0.0};//0-even,1-odd,2-all pairs
  Double_t fractional_broadening[3]   = {0.0};//0-even,1-odd,2-all pairs
  TString MD_name[3] = {"MD even", "MD odd", "MD all"};
 

  TCanvas *mc = NULL;

  TPaveText pt(.0,.0,1,1);

  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(3,2);
  mc->cd(1);
  tree= (TTree *)dMapFile->Get("Hel_Tree");

  if (tree==NULL){
    printf("Hel_Tree tree object does not exist!\n");
    return;
  }

  
  md_pair_hist26=(TH1F*) GetHisto(tree,"(asym_qwk_md2barsum+asym_qwk_md6barsum)/2.0*1.0e6","","");
  if (!md_pair_hist26) {
    printf("md_pair_hist26 unable to process!\n");
    return;
  }
  printf("MD 2&6 No PreRad\n");
  md_pair_hist26 -> SetTitle("MD 2&6 No PreRad");
  md_pair_hist26 -> GetXaxis()->SetTitle("MD 2 & 6 Average (ppm)");
  md_pair_hist26 -> Fit("gaus", "M");
  md_pair_hist26_fit = md_pair_hist26 -> GetFunction("gaus");
  md_pair_hist26_fit->SetLineColor(kRed);
  
  if (md_pair_hist26_fit) {
    md_pair_hist26_sigma = md_pair_hist26_fit -> GetParameter(2);
  }

  
  
  mc->cd(2);  
  md_pair_hist48=(TH1F*) GetHisto(tree,"(asym_qwk_md4barsum+asym_qwk_md8barsum)/2.0*1.0e6","","");
  if (!md_pair_hist48) {
    printf("md_pair_hist48 unable to process!\n");
    return;
  }

  printf("MD 4&8 No PreRad\n");
  md_pair_hist48 -> SetTitle("MD 4&8 No PreRad");
  md_pair_hist48 -> GetXaxis()->SetTitle("MD 4 & 8 Average (ppm)");
  md_pair_hist48 -> Fit("gaus", "M");
  md_pair_hist48_fit = md_pair_hist48 -> GetFunction("gaus");
  md_pair_hist48_fit->SetLineColor(kRed);

  if (md_pair_hist48_fit) {
    md_pair_hist48_sigma = md_pair_hist48_fit -> GetParameter(2);
  }
 
  mc->cd(3);
  md_pair_hist15=(TH1F*) GetHisto(tree,"(asym_qwk_md1barsum+asym_qwk_md5barsum)/2.0*1.0e6","","");
  if (!md_pair_hist15) {
    printf("md_pair_hist15 unable to process!\n");
    return;
  }

  printf("MD 1&5 with PreRad\n");
  md_pair_hist15 -> SetTitle("MD 1&5 PreRad");
  md_pair_hist15 -> GetXaxis()->SetTitle("MD 1 & 5 Average (ppm)");
  md_pair_hist15 -> Fit("gaus", "M");
  md_pair_hist15_fit = md_pair_hist15 -> GetFunction("gaus");
  md_pair_hist15_fit->SetLineColor(kRed);

  if (md_pair_hist15_fit) {
    md_pair_hist15_sigma = md_pair_hist15_fit -> GetParameter(2);
  }


  mc->cd(4);
  md_pair_hist37=(TH1F*) GetHisto(tree,"(asym_qwk_md3barsum+asym_qwk_md7barsum)/2.0*1.0e6","","");
  if (!md_pair_hist37) {
    printf("md_pair_hist37 unable to process!\n");
    return;
  }

  printf("MD 3&7 with PreRad\n");
  md_pair_hist37 -> SetTitle("MD 3&7 PreRad");
  md_pair_hist37 -> GetXaxis()->SetTitle("MD 3 & 7 Average (ppm)");
  md_pair_hist37 -> Fit("gaus", "M");
  md_pair_hist37_fit = md_pair_hist37 -> GetFunction("gaus");
  md_pair_hist37_fit->SetLineColor(kRed);
  if (md_pair_hist37_fit) {
    md_pair_hist37_sigma = md_pair_hist37_fit -> GetParameter(2);
  }

  mc->cd(5);
  md_all_even_hist=(TH1F*) GetHisto(tree,"(asym_qwk_md2barsum+asym_qwk_md4barsum+asym_qwk_md6barsum+asym_qwk_md8barsum)/4.0*1.0e6","","");
  if (!md_all_even_hist) {
    printf("md_all_even_hist unable to process!\n");
    return;
  }
  printf("MD all even no PreRad\n");
  md_all_even_hist -> SetTitle("MD All Even No PreRad");
  md_all_even_hist -> GetXaxis()->SetTitle("MD All Even Average (ppm)");

  md_all_even_hist->Fit("gaus", "M");
  
  MDEvenAll_fit = md_all_even_hist-> GetFunction("gaus");
  MDEvenAll_fit->SetLineColor(kRed);
 
  if(MDEvenAll_fit) {
    md_evenall_sigma = MDEvenAll_fit -> GetParameter(2);
  }

  md_all_odd_hist=(TH1F*) GetHisto(tree,"(asym_qwk_md1barsum+asym_qwk_md3barsum+asym_qwk_md5barsum+asym_qwk_md7barsum)/4.0*1.0e6","","");
  if (!md_all_odd_hist) {
    printf("md_all_odd_hist unable to process!\n");
    return;
  }
  printf("MD all odd with PreRad\n");
  md_all_odd_hist -> SetTitle("MD All Odd With PreRad");
  md_all_odd_hist -> GetXaxis()->SetTitle("MD All odd Average (ppm)");

  md_all_odd_hist->Fit("gaus", "M");
  
  MDOddAll_fit = md_all_odd_hist-> GetFunction("gaus");
  MDOddAll_fit->SetLineColor(kRed);
 
  if(MDOddAll_fit) {
    md_oddall_sigma = MDOddAll_fit -> GetParameter(2);
  }

  md_all_hist=(TH1F*) GetHisto(tree,"(asym_qwk_md2barsum+asym_qwk_md4barsum+asym_qwk_md6barsum+asym_qwk_md8barsum+asym_qwk_md1barsum+asym_qwk_md3barsum+asym_qwk_md5barsum+asym_qwk_md7barsum)/8.0*1.0e6","","");
  if (!md_all_hist) {
    printf("md_all_hist unable to process!\n");
    return;
  }
  printf("MD all \n");
  md_all_hist -> SetTitle("MD All Bars");
  md_all_hist -> GetXaxis()->SetTitle("MD All Average (ppm)");

  md_all_hist->Fit("gaus", "M");
  
  MDAll_fit = md_all_hist-> GetFunction("gaus");
  MDAll_fit->SetLineColor(kRed);
 
  if(MDAll_fit) {
    md_all_sigma = MDAll_fit -> GetParameter(2);
  }

  //Calculate uncorrelated widths
  uncorrelated_width[0] = TMath::Sqrt(md_pair_hist26_sigma*md_pair_hist26_sigma + md_pair_hist48_sigma*md_pair_hist48_sigma) / 2.0;
  uncorrelated_width[1] = TMath::Sqrt(md_pair_hist15_sigma*md_pair_hist15_sigma + md_pair_hist37_sigma*md_pair_hist37_sigma) / 2.0;
  uncorrelated_width[2] = TMath::Sqrt(md_pair_hist26_sigma*md_pair_hist26_sigma + md_pair_hist48_sigma*md_pair_hist48_sigma +
				      md_pair_hist15_sigma*md_pair_hist15_sigma + md_pair_hist37_sigma*md_pair_hist37_sigma) / 4.0;

  
  common_mode_noise[0] = TMath::Sqrt(TMath::Abs(md_evenall_sigma*md_evenall_sigma - uncorrelated_width[0]*uncorrelated_width[0]));
  if(uncorrelated_width[0] != 0.0) {
    fractional_broadening[0] = md_evenall_sigma / uncorrelated_width[0];
  }
  else {
    fractional_broadening[0] = 0.0;
    printf("Even Bar uncorrelated_width = %8.2lf. don't calculate fractional broadening.\n", uncorrelated_width[0]);
  }
  printf("%7s Bars uncorelated witdth %8.2lf [ppm], common mode noise %8.2lf [ppm], fractional broadening %4.2lf\n",
	 MD_name[0].Data(),    uncorrelated_width[0], common_mode_noise[0], fractional_broadening[0]);


  common_mode_noise[1] = TMath::Sqrt(TMath::Abs(md_oddall_sigma*md_oddall_sigma - uncorrelated_width[1]*uncorrelated_width[1]));
  if(uncorrelated_width[1] != 0.0) {
    fractional_broadening[1] = md_oddall_sigma / uncorrelated_width[1];
  }
  else {
    fractional_broadening[1] = 0.0;
    printf("Odd Bar uncorrelated_width = %8.2lf. don't calculate fractional broadening.\n", uncorrelated_width[1]);
  }
  printf("%7s Bars uncorelated witdth %8.2lf [ppm], common mode noise %8.2lf [ppm], fractional broadening %4.2lf\n",
	 MD_name[1].Data(),    uncorrelated_width[1], common_mode_noise[1], fractional_broadening[1]);


  common_mode_noise[2] = TMath::Sqrt(TMath::Abs(md_all_sigma*md_all_sigma - uncorrelated_width[2]*uncorrelated_width[2]));
  if(uncorrelated_width[2] != 0.0) {
    fractional_broadening[2] = md_all_sigma / uncorrelated_width[2];
  }
  else {
    fractional_broadening[2] = 0.0;
    printf("All Bars uncorrelated_width = %8.2lf. don't calculate fractional broadening.\n", uncorrelated_width[2]);
  }
  printf("%7s Bars uncorelated witdth %8.2lf [ppm], common mode noise %8.2lf [ppm], fractional broadening %4.2lf\n",
	 MD_name[2].Data(),    uncorrelated_width[2], common_mode_noise[2], fractional_broadening[2]);

  mc->cd(6);
  //  pt.AddText(Form("%7s Bars uncorr. #sigma %8.2lf [ppm]",MD_name[0].Data(), uncorrelated_width[0]));
  pt.AddText(Form("%7s Bars C.M.N %8.2lf [ppm]",MD_name[0].Data(), common_mode_noise[0]));
  pt.AddText(Form("%7s Bars frac. broad. %4.2lf",MD_name[0].Data(), fractional_broadening[0]));
  
  //  pt.AddText(Form("%7s Bars uncorr. #sigma %8.2lf [ppm]",MD_name[1].Data(), uncorrelated_width[1]));
  pt.AddText(Form("%7s Bars C.M.N %8.2lf [ppm]",MD_name[1].Data(), common_mode_noise[1]));
  pt.AddText(Form("%7s Bars frac. broad. %4.2lf",MD_name[1].Data(), fractional_broadening[1]));

  //  pt.AddText(Form("%7s Bars uncorr. #sigma %8.2lf [ppm]",MD_name[2].Data(), uncorrelated_width[2]));
  pt.AddText(Form("%7s Bars C.M.N %8.2lf [ppm]",MD_name[2].Data(), common_mode_noise[2]));
  pt.AddText(Form("%7s Bars frac. broad. %4.2lf",MD_name[2].Data(), fractional_broadening[2]));

  pt.SetTextAlign(12);
  pt.SetTextSize(0.05);

  pt.Draw();
  gPad->Update();
  mc->Modified();
  mc->Update();

  while (1){
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }    
  }
};

void QwGUIMainDetector::LoadMDPMTCombo(){
  dComboBoxMDPMT->RemoveAll();
  //printf("QwGUIHallCBeamline::LoadHCBCMCombo \n");
  std::size_t i =0;
  for(i=0;i<fMDDevices.at(VQWK).size();i++){
    dComboBoxMDPMT->AddEntry(fMDDevices.at(VQWK).at(i),i);
    //printf("%s \n",fHallCDevices.at(VQWK_BCM).at(i).Data());
  }
  if (fMDDevices.at(VQWK).size()>0)
    dButtonMDPMT->SetEnabled(kTRUE);
  fCurrentPMTIndex=-1;
};

void QwGUIMainDetector::LoadMDVPMTCombo(){
  dComboBoxMDVPMT->RemoveAll();
  //printf("QwGUIHallCBeamline::LoadHCSCALERCombo \n");
  std::size_t i = 0;
  for(i=0;i<fMDDevices.at(VPMT).size();i++){
    dComboBoxMDVPMT->AddEntry(fMDDevices.at(VPMT).at(i),i);
    //printf("%s \n",fHallCDevices.at(SCALER_HALO).at(i).Data());
  }
  if (fMDDevices.at(VPMT).size()>0)
    dButtonMDVPMT->SetEnabled(kTRUE);

  fCurrentVPMTIndex=-1;
};


void QwGUIMainDetector::OnReceiveMessage(char *obj)
{
}

void QwGUIMainDetector::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWind")) return;
  
  printf("Received Messager From: %s\n",obj);
}


Bool_t QwGUIMainDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  if(dMapFileFlag) {
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
    
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:{
      switch(parm1){
      case   BA_MD_16:
	//printf("text button id %ld pressed \n", parm1);		  
	DrawMD16Plots();
	break; 
      case   BA_MD_BKG:
	//printf("text button id %ld pressed \n", parm1);		  
	DrawMDBkgPlots();	
	break;  
      case   BA_MD_CMB:
	//	printf("text button id %ld pressed \n", parm1);		  
	DrawMDCmbPlots();	
	break;  
      case   BA_MD_PMT:
	//printf("text button id %ld pressed \n", parm1);		  
	DrawMDPMTPlots();	
	break;  
      case   BA_MD_VPMT:
	//printf("text button id %ld pressed \n", parm1);		  
	DrawMDVPMTPlots();	
	break;  
      case BA_MD_CMNS:
	//printf("text button BA_MD_CMNS %ld pressed \n", parm1);		  
	CalculateCommNoise();	
	break;
      }	      
      break;
    }
    case kCM_COMBOBOX:
      {
	switch (parm1) {
	case CMB_MDPMT:
	  SetComboIndex(CMB_MDPMT,parm2);
	  break;
	case CMB_MDVPMT:
	  SetComboIndex(CMB_MDVPMT,parm2);
	  break;
	}
      }
      break;

    case kCM_MENUSELECT:
      break;
      
    case kCM_MENU:

      switch (parm1) {
	

      default:
	break;
      }
      
    default:
      break;
    }
    
  default:
    break;
  }
  }
  return kTRUE;
}

void QwGUIMainDetector::SetComboIndex(Int_t cmb_id, Int_t id){
    if (cmb_id==CMB_MDPMT)
      fCurrentPMTIndex=id;
    //else
    //fCurrentBCMIndex=-1;

    if (cmb_id==CMB_MDVPMT)
      fCurrentVPMTIndex=id;
    //else
    //fCurrentSCALERIndex=-1;
}

TH1F* QwGUIMainDetector::GetHisto(TTree *tree, const TString name, const TCut cut, Option_t* option)
{
  tree ->Draw(name, cut, option);
  TH1F* tmp = NULL;
  tmp = (TH1F*)  gPad -> GetPrimitive("htemp");
  if(! tmp) {
    return 0;
  }
  return tmp;
}

