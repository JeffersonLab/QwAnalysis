#include "QwGUIInjector.h"


#include <TG3DLine.h>
#include "TGaxis.h"




ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POS_DIFF,
  BA_CHARGE,
  BA_POS_VAR,
  BA_HCSCALER,
  BA_MEAN_POS,
};

//INJECTOR_DET_TYPES is the size of the enum
enum QwGUIInjectorDeviceTypes {
  UNKNOWN_TYPE,
  VQWK_BPMSTRIPLINE,
  VQWK_BPMCAVITY,
  VQWK_BCM,  
  SCALER_HALO,
  COMBINED,
};

//Combo box
enum QwGUIHallCBeamlinePlotsComboBox {
  CMB_INJECTORBCM,
  CMB_INJECTORSCALER,
};


QwGUIInjector::QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  //QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  //QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  //QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  dTabFrame               = NULL;
  dControlsFrame          = NULL;
  dInjectorBCMFrame       = NULL;
  dInjectorSCALERFrame    = NULL;
  dCanvas                 = NULL;  
  dTabLayout              = NULL;
  dCnvLayout              = NULL;
  dSubLayout              = NULL;
  dBtnLayout              = NULL;
  dButtonPos              = NULL;
  dButtonMeanPos          = NULL;
  dButtonCharge           = NULL;
  dButtonPosVariation     = NULL;
  dButtonInjectorSCALER   = NULL;
  dComboBoxInjectorBCM    = NULL;
  dComboBoxInjectorSCALER = NULL;
  
  PosVariation[0] = NULL;
  PosVariation[1] = NULL;

  LoadHistoMapFile(Form("%s/Parity/prminput/qweak_beamline.map",gSystem->Getenv("QWANALYSIS")));

  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);
  LoadInjectorBCMCombo();//Load BCM list into the combo box
  LoadInjectorSCALERCombo();//Load SCALER list into the combo box

}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)             delete dTabFrame;
  if(dControlsFrame)        delete dControlsFrame;
  if(dCanvas)               delete dCanvas;  
  if(dInjectorBCMFrame)     delete dInjectorBCMFrame;
  if(dInjectorSCALERFrame)  delete dInjectorSCALERFrame;
  if(dTabLayout)            delete dTabLayout;
  if(dCnvLayout)            delete dCnvLayout;
  if(dSubLayout)            delete dSubLayout;
  if(dBtnLayout)            delete dBtnLayout;
  if(dButtonPos)            delete dButtonPos;
  if(dButtonCharge)         delete dButtonCharge;
  if(dButtonPosVariation)   delete dButtonPosVariation;
  if(dButtonInjectorSCALER) delete dButtonInjectorSCALER;
  if(dButtonMeanPos)        delete dButtonMeanPos; 

  if(dComboBoxInjectorBCM)    delete dComboBoxInjectorBCM;
  if(dComboBoxInjectorSCALER) delete dComboBoxInjectorSCALER;
  

  delete [] PosVariation;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIInjector::LoadHistoMapFile(TString mapfile){
  TString varname, varvalue;
  TString modtype, namech,dettype;
  Int_t count_names;
  fInjectorDevices.resize(INJECTOR_DET_TYPES);
  QwParameterFile mapstr(mapfile.Data());  //Open the file
  //fInjectorDevices.clear();
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname=="name"){
	//printf("%s - %s \n",varname.Data(),varvalue.Data());
	fInjectorDevices.at(COMBINED).push_back(varvalue);
      }
    }
    else{
      modtype   = mapstr.GetNextToken(", ").c_str();	// module type
      if (modtype=="VQWK" || modtype=="SCALER"){
	mapstr.GetNextToken(", ");	//slot number
	mapstr.GetNextToken(", ");	//channel number
	dettype=mapstr.GetNextToken(", ");	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
	namech.ToLower();
	if (modtype=="VQWK" && dettype=="bpmstripline")
	  namech.Remove(namech.Length()-2,2);

	count_names=count(fInjectorDevices.at(VQWK_BPMSTRIPLINE).begin(),fInjectorDevices.at(VQWK_BPMSTRIPLINE).end(),namech);

	if (!count_names && dettype=="bpmstripline"){
	  //printf("%s - %s \n",modtype.Data(),namech.Data() );	  
	  fInjectorDevices.at(VQWK_BPMSTRIPLINE).push_back(namech);
	}
	else if (dettype=="bpmcavity"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fInjectorDevices.at(VQWK_BPMCAVITY).push_back(namech);
	}
	else if (dettype=="bcm"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fInjectorDevices.at(VQWK_BCM).push_back(namech);
	}
	else if (dettype=="halomonitor"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fInjectorDevices.at(SCALER_HALO).push_back(namech);
	}

      }

    }

  }

  //printf("no. of hallC devices %d \n",fInjectorDevices.size());
   printf(" Injector  Device List\n" );
  for (Size_t i=0;i<fInjectorDevices.size();i++)
    for (Size_t j=0;j<fInjectorDevices.at(i).size();j++)
      printf("%s \n",fInjectorDevices.at(i).at(j).Data() );	  
};

void QwGUIInjector::MakeLayout()
{

  SetCleanup(kDeepCleanup);

  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));


  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  //Injector bcm access frame
  dInjectorBCMFrame= new TGVerticalFrame(dControlsFrame,50,100);
  //Injector scaler access frame
  dInjectorSCALERFrame= new TGVerticalFrame(dControlsFrame,50,100);

 
  dButtonPos = new TGTextButton(dControlsFrame, "&Beam Position Asymmetries", BA_POS_DIFF);
  dButtonMeanPos = new TGTextButton(dControlsFrame, "&Mean Beam Positions", BA_MEAN_POS);
  dButtonPosVariation = new TGTextButton(dControlsFrame, "BPM Eff_Charge Variation", BA_POS_VAR);
  dButtonCharge = new TGTextButton(dInjectorBCMFrame, "&BCM Yield/Asymmetry", BA_CHARGE);
  dButtonCharge->SetEnabled(kFALSE);
  
  dButtonInjectorSCALER = new TGTextButton(dInjectorSCALERFrame, "&SCALER Yield/Asymmetry", BA_HCSCALER);
  dButtonInjectorSCALER->SetEnabled(kFALSE);


  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dButtonPos,dBtnLayout );
  dControlsFrame->AddFrame(dButtonPosVariation, dBtnLayout);
  dControlsFrame->AddFrame(dButtonMeanPos, dBtnLayout);
  dControlsFrame->AddFrame(dInjectorBCMFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dInjectorSCALERFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  //Add Injector BCM  combo box
  dComboBoxInjectorBCM=new TGComboBox(dInjectorBCMFrame,CMB_INJECTORBCM);
  dComboBoxInjectorBCM->Resize(50,20);//To make it better looking
  
  //Add Injector SCALER  combo box
  dComboBoxInjectorSCALER=new TGComboBox(dInjectorSCALERFrame,CMB_INJECTORSCALER);
  dComboBoxInjectorSCALER->Resize(50,20);//To make it better looking

  //add components to the Injector  bcm access frame
  dInjectorBCMFrame->AddFrame(dComboBoxInjectorBCM, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dInjectorBCMFrame->AddFrame(dButtonCharge,dBtnLayout);

  //add components to the Injector  SCALER access frame
  dInjectorSCALERFrame->AddFrame(dComboBoxInjectorSCALER, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dInjectorSCALERFrame->AddFrame(dButtonInjectorSCALER, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  

  dButtonPos -> Associate(this);
  dButtonMeanPos -> Associate(this);
  dButtonCharge -> Associate(this);
  dButtonPosVariation -> Associate(this);
  dButtonInjectorSCALER -> Associate(this);
  dComboBoxInjectorBCM->Associate(this);
  dComboBoxInjectorSCALER->Associate(this);

 
  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIInjector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");


}

void QwGUIInjector::OnReceiveMessage(char *obj)
{

}

void QwGUIInjector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIInjector::OnObjClose\n");

    dROOTCont = NULL;
  }
}


void QwGUIInjector::OnNewDataContainer()
{


};

void QwGUIInjector::OnRemoveThisTab()
{

};

void QwGUIInjector::ClearData()
{


}






void QwGUIInjector::PositionDifferences()
{
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;
  Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     PosVariation[0] = new TH1F("Eff_Asym", "Eff_Charge Asym Variation",BPMSTriplinesCount , min_range, max_range);
     PosVariation[1] = new TH1F("Eff_Yield", "Eff_Charge Yield Variation",BPMSTriplinesCount , min_range, max_range); 
     for(Short_t p = 0; p <BPMSTriplinesCount ; p++) {
       sprintf (histo, "asym_%s_EffectiveCharge_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
       histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
       if (histo1!=NULL){
	 xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	 if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	 histo1->SetName(histo);
	 
	 dummyname = histo1->GetName();
	 
	 dummyname.Replace(0,9," ");
	 dummyname.ReplaceAll("_EffectiveCharge_hw", "");
	 PosVariation[0] -> SetBinContent(xcount, histo1->GetMean());
	 PosVariation[0] -> SetBinError  (xcount, histo1->GetMeanError());
	 PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	 SummaryHist(histo1);
	 delete histo1; histo1= NULL;
       }
      
       sprintf (histo, "yield_%s_EffectiveCharge_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
       histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
       if(histo2!=NULL){		
	 ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	 if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	 histo2->SetName(histo);
	 dummyname = histo2->GetName();
	 dummyname.Replace(0,9," ");
	 dummyname.ReplaceAll("_EffectiveCharge_hw", "");
	 PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
	 PosVariation[1] -> SetBinError  (ycount, histo2->GetMeanError());
	 PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	 SummaryHist(histo2);
	 delete histo2; histo2= NULL; 
       }
      
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    //SummaryHist(PosVariation[0]);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetTitle("Eff_Charge Asymmetry");
    PosVariation[0] -> GetYaxis() -> SetTitle("#No Units");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    //SummaryHist(PosVariation[1]);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetTitle("Eff_Charge Yield");
    PosVariation[1] -> GetYaxis()-> SetTitle ("Coulomb");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[1] -> Draw("E1");

    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Short_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;
}

void QwGUIInjector::PlotChargeAsym()
{
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  

   while (1){
     if (fCurrentBCMIndex<0)
       break;
     sprintf (histo, "asym_%s_hw",fInjectorDevices.at(VQWK_BCM).at(fCurrentBCMIndex).Data() );
     histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
     sprintf (histo, "yield_%s_hw",fInjectorDevices.at(VQWK_BCM).at(fCurrentBCMIndex).Data() );
     histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
    
    if (histo1!=NULL || histo2!=NULL ) {
    
      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      SummaryHist(histo1);
      mc->cd(2);
      histo2->Draw();
      SummaryHist(histo2);
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
  

   printf("---------------PlotChargeAsym()--------------------\n");

   return;
}


void QwGUIInjector::PlotBPMAsym(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  
  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     PosVariation[0] = new TH1F("AsymX", "Asymmetry X Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariation[1] = new TH1F("AsymY", "Asymmetry Y variation", BPMSTriplinesCount, min_range, max_range); 
    for(Short_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      sprintf (histo, "asym_%sX_hw",fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() );
      histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
      if (histo1!=NULL) {
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	histo1->SetName(histo);
	    
	dummyname = histo1->GetName();
	    
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1->GetMean());
	PosVariation[0] -> SetBinError  (xcount, histo1->GetMeanError());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      sprintf (histo, "asym_%sY_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
      histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
	PosVariation[1] -> SetBinError  (ycount, histo2->GetMeanError());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	SummaryHist(histo2);
	delete histo2; histo2= NULL; 
      }
	  
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    //SummaryHist(PosVariation[0]);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetTitle("Asymmetry X Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("Asymmetry");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    //SummaryHist(PosVariation[1]);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetTitle("Asymmetry Y Variation");
    PosVariation[1] -> GetYaxis()-> SetTitle ("Asymmetry");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM Y");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Short_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }


  return;



}
  
  

void QwGUIInjector::PlotBPMPositions(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  


  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 

   while (1){ 
     PosVariation[0] = new TH1F("PosX", "Mean X Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariation[1] = new TH1F("PosY", "Mean Y variation", BPMSTriplinesCount, min_range, max_range); 
    for(Short_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      sprintf (histo, "%sX_hw",fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() );
      histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
      if (histo1!=NULL) {
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	histo1->SetName(histo);
	    
	dummyname = histo1->GetName();
	    
	dummyname.Replace(0,4," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1->GetMean());
	PosVariation[0] -> SetBinError  (xcount, histo1->GetMeanError());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      sprintf (histo, "%sY_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
      histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,4," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
	PosVariation[1] -> SetBinError  (ycount, histo2->GetMeanError());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	SummaryHist(histo2);
	delete histo2; histo2= NULL; 
      }
	  
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    //SummaryHist(PosVariation[0]);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetTitle("Mean BPM X Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("Pos (mm)");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    //SummaryHist(PosVariation[1]);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetTitle("Mean BPM Y Variation");
    PosVariation[1] -> GetYaxis()-> SetTitle ("Pos (mm)");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM Y");
    PosVariation[1] -> Draw("E1");
    
    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Short_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;

};  
  

void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{

}

void QwGUIInjector::PlotSCALER(){
  
};

void QwGUIInjector::SetComboIndex(Short_t cmb_id, Short_t id){

  if (cmb_id==CMB_INJECTORBCM)
    fCurrentBCMIndex=id;

  if (cmb_id==CMB_INJECTORSCALER)
    fCurrentSCALERIndex=id;
};

void QwGUIInjector::LoadInjectorBCMCombo(){
  dComboBoxInjectorBCM->RemoveAll();
  //printf("QwGUIInjector::LoadInjectorBCMCombo \n");
  for(Size_t i=0;i<fInjectorDevices.at(VQWK_BCM).size();i++){
    dComboBoxInjectorBCM->AddEntry(fInjectorDevices.at(VQWK_BCM).at(i),i);
    //printf("%s \n",fInjectorDevices.at(VQWK_BCM).at(i).Data());
  }
  if (fInjectorDevices.at(VQWK_BCM).size()>0)
    dButtonCharge->SetEnabled(kTRUE);
  fCurrentBCMIndex=-1;
};

void QwGUIInjector::LoadInjectorSCALERCombo(){
  dComboBoxInjectorSCALER->RemoveAll();
  //printf("QwGUIInjector::LoadInjectorSCALERCombo \n");
  for(Size_t i=0;i<fInjectorDevices.at(SCALER_HALO).size();i++){
    dComboBoxInjectorSCALER->AddEntry(fInjectorDevices.at(SCALER_HALO).at(i),i);
    //printf("%s \n",fInjectorDevices.at(SCALER_HALO).at(i).Data());
  }
  if (fInjectorDevices.at(SCALER_HALO).size()>0)
    dButtonInjectorSCALER->SetEnabled(kTRUE);

  fCurrentSCALERIndex=-1;
};


Bool_t QwGUIInjector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
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
      if(dROOTCont){
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
		  printf("PlotPosData() button id %ld pressed\n", parm1);
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
		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
	      case M_TBIN_SELECT:
		break;
	      case CMB_INJECTORBCM:
		SetComboIndex(CMB_INJECTORBCM,parm2);
		break;
	      case CMB_INJECTORSCALER:
		SetComboIndex(CMB_INJECTORSCALER,parm2);
		break;	      
	      }
	    }
	    break;
	    
	  case kCM_MENUSELECT:
	    break;
	    
	  case kCM_MENU:
	    
	    switch (parm1) {
	      
	    case M_FILE_OPEN:
	      break;
	      
	      
	      
	  default:
	    break;
	  }
	  
	default:
	  break;
	}
      }
      else{
	std::cout<<"Please load the map file to view data. \n";
      }
      default:
	break;
    }
  
  return kTRUE;
}



void 
QwGUIInjector::SummaryHist(TH1 *in)
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
