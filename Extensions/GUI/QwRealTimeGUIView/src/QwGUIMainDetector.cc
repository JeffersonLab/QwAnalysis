#include <QwGUIMainDetector.h>

#include <TG3DLine.h>
#include "TGaxis.h"

ClassImp(QwGUIMainDetector);

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
  dNumberEntryDlg  = NULL;
  dButtonMD16      = NULL;
  dButtonMDBkg     = NULL;
  dButtonMDCmb     = NULL;
  dButtonMDPMT     = NULL;
  dButtonMDVPMT    = NULL;

  dComboBoxMDPMT   = NULL;
  dComboBoxMDVPMT  = NULL;


  AddThisTab(this);

  
  ClearData();



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
  if(dButtonMDVPMT)   delete dButtonMDVPMT;
  if(dComboBoxMDPMT)  delete dComboBoxMDPMT;

  RemoveThisTab(this);
  IsClosing(GetName());
  ClearData();
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

  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));
  
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200);     
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

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

  dControlsFrame->AddFrame(dButtonMD16, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));
  dControlsFrame->AddFrame(dButtonMDBkg, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));
  dControlsFrame->AddFrame(dButtonMDCmb, new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5));
  dControlsFrame->AddFrame(dMDPMTFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dMDVPMTFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  dMDPMTFrame->AddFrame(dComboBoxMDPMT, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dMDPMTFrame->AddFrame(dButtonMDPMT, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  dMDVPMTFrame->AddFrame(dComboBoxMDVPMT, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dMDVPMTFrame->AddFrame(dButtonMDVPMT, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));  

  dButtonMD16->Associate(this);
  dButtonMDBkg->Associate(this);
  dButtonMDCmb->Associate(this);
  dButtonMDPMT->Associate(this);
  dButtonMDVPMT->Associate(this);
  dComboBoxMDPMT->Associate(this);
  dComboBoxMDVPMT->Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  				"QwGUIMainDetector",
  				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  

}

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
      modtype   = mapstr.GetNextToken(", ").c_str();	// module type
      if (modtype=="VQWK" || modtype=="VPMT"){
	mapstr.GetNextToken(", ");	//slot number
	mapstr.GetNextToken(", ");	//channel number
	dettype=mapstr.GetNextToken(", ");	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
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
  TH1F *histo2=NULL;
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  

   while (1){
     if (fCurrentPMTIndex<0)
       break;
     sprintf (histo, "asym_%s_hw",fMDDevices.at(VQWK).at(fCurrentPMTIndex).Data() );
     histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
     sprintf (histo, "yield_%s_hw",fMDDevices.at(VQWK).at(fCurrentPMTIndex).Data() );
     histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
    
    if (histo1!=NULL && histo2!=NULL ) {
    
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
  

   printf("---------------DrawMDPMTPlots()--------------------\n");
  //mc->Modified();
  //mc->Update();
  

  return;
};

void QwGUIMainDetector::DrawMDVPMTPlots(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  

   while (1){
     if (fCurrentVPMTIndex<0)
       break;
     sprintf (histo, "asym_%s_hw",fMDDevices.at(VPMT).at(fCurrentVPMTIndex).Data() );
     histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
     sprintf (histo, "yield_%s_hw",fMDDevices.at(VPMT).at(fCurrentVPMTIndex).Data() );
     histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
    
    if (histo1!=NULL && histo2!=NULL ) {
    
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
  

   printf("---------------DrawMDVPMTPlots()--------------------\n");
  //mc->Modified();
  //mc->Update();
  

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
      histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
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
      histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
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

  
  TH1D *dummyhist = NULL;

  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     MDPlots[0] = new TH1F("Asym", "Asymmetry Variation",MDBkgCount , min_range, max_range);
     MDPlots[1] = new TH1F("Yield", "Yield variation",MDBkgCount , min_range, max_range); 

    for(Short_t p = MAIN_DET_INDEX; p <fMDDevices.at(VQWK).size(); p++) 
    {
      sprintf (histo, "asym_%s_hw",fMDDevices.at(VQWK).at(p).Data() );
      histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
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
      histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
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

void QwGUIMainDetector::DrawMDCmbPlots(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t MDCmbCount = fMDDevices.at(VPMT).size();
  Double_t max_range = (Double_t)MDCmbCount- offset ; 

  
  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     MDPlots[0] = new TH1F("Asym", "Asymmetry Variation",MDCmbCount , min_range, max_range);
     MDPlots[1] = new TH1F("Yield", "Yield variation",MDCmbCount , min_range, max_range); 
    for(Short_t p = 0; p <MDCmbCount ; p++) 
    {
      sprintf (histo, "asym_%s_hw",fMDDevices.at(VPMT).at(p).Data() );
      histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
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
	  
      sprintf (histo, "yield_%s_hw", fMDDevices.at(VPMT).at(p).Data());
      histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
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
    for (Short_t p = 0; p < 2 ; p++){
      delete MDPlots[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }
   

  return;

};

void QwGUIMainDetector::LoadMDPMTCombo(){
  dComboBoxMDPMT->RemoveAll();
  //printf("QwGUIHallCBeamline::LoadHCBCMCombo \n");
  for(Size_t i=0;i<fMDDevices.at(VQWK).size();i++){
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
  for(Size_t i=0;i<fMDDevices.at(VPMT).size();i++){
    dComboBoxMDVPMT->AddEntry(fMDDevices.at(VPMT).at(i),i);
    //printf("%s \n",fHallCDevices.at(SCALER_HALO).at(i).Data());
  }
  if (fMDDevices.at(VPMT).size()>0)
    dButtonMDVPMT->SetEnabled(kTRUE);

  fCurrentVPMTIndex=-1;
};


void QwGUIMainDetector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIMainDetector::OnObjClose(char *obj)
{
  if(!obj) return;
  TString name = obj;


  if(!strcmp(obj,"dROOTFile")){
    dROOTCont = NULL;
  }

  if(!strcmp(obj,"dNumberEntryDlg")){
    delete dNumberEntryDlg;
    dNumberEntryDlg = NULL;
  }

  if(!strcmp(obj,"dProgrDlg")){
    dProcessHalt = kTrue;
    dProgrDlg = NULL;
  }   

  QwGUISubSystem::OnObjClose(obj);
}

void QwGUIMainDetector::OnNewDataContainer()
{

 
 
}

void QwGUIMainDetector::OnRemoveThisTab()
{

}

void QwGUIMainDetector::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWind")) return;
  
  printf("Received Messager From: %s\n",obj);
}


void QwGUIMainDetector::ClearData()
{


  
}





void QwGUIMainDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{

}

Bool_t QwGUIMainDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
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
	//printf("text button id %ld pressed \n", parm1);		  
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
      }	      
      break;
    }
    case kCM_COMBOBOX:
      {
	switch (parm1) {
	case M_TBIN_SELECT:
	  break;
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
  
  return kTRUE;
}

void QwGUIMainDetector::SummaryHist(TH1 *in)
{

  Double_t out[4] = {0.0};
  Double_t test   = 0.0;

  out[0] = in -> GetMean();
  out[1] = in -> GetMeanError();
  out[2] = in -> GetRMS();
  out[3] = in -> GetRMSError();
  test   = in -> GetRMS()/sqrt(in->GetEntries());

  printf("%sName%s", BOLD, NORMAL);
  printf("%22s", in->GetName());
  printf("  %sMean%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[0], NORMAL, BLUE, out[1], NORMAL);
  printf("  %sSD%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[2], NORMAL, GREEN, out[3], NORMAL);
  printf(" %sRMS/Sqrt(N)%s %s%+4.2e%s \n", BOLD, NORMAL, BLUE, test, NORMAL);
  return;
};

void QwGUIMainDetector::SetComboIndex(Short_t cmb_id, Short_t id){
    if (cmb_id==CMB_MDPMT)
      fCurrentPMTIndex=id;
    //else
    //fCurrentBCMIndex=-1;

    if (cmb_id==CMB_MDVPMT)
      fCurrentVPMTIndex=id;
    //else
    //fCurrentSCALERIndex=-1;
}

