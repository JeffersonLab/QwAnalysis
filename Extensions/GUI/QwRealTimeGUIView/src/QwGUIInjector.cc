#include "QwGUIInjector.h"


#include "TG3DLine.h"
#include "TGaxis.h"
#include "TStyle.h"



ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POS_DIFF,
  BA_POS_DIFF_RMS,
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
  dButtonPosDiffMean      = NULL;
  dButtonMeanPos          = NULL;
  dButtonCharge           = NULL;
  dButtonPosVariation     = NULL;
  dButtonPosDiffRms       = NULL;
  dButtonInjectorSCALER   = NULL;
  dComboBoxInjectorBCM    = NULL;
  dComboBoxInjectorSCALER = NULL;
  
  PosVariation[0] = NULL;
  PosVariation[1] = NULL;


  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

  SetHistoAccumulate(1);
  SetHistoReset(0);

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
  if(dButtonPosDiffMean)    delete dButtonPosDiffMean;
  if(dButtonPosDiffRms)     delete dButtonPosDiffRms;
  if(dButtonCharge)         delete dButtonCharge;
  if(dButtonPosVariation)   delete dButtonPosVariation;
  if(dButtonInjectorSCALER) delete dButtonInjectorSCALER;
  if(dButtonMeanPos)        delete dButtonMeanPos; 

  if(dComboBoxInjectorBCM)    delete dComboBoxInjectorBCM;
  if(dComboBoxInjectorSCALER) delete dComboBoxInjectorSCALER;
  

  delete [] PosVariation;

  //RemoveThisTab(this);
  //  IsClosing(GetName());
}

void QwGUIInjector::LoadHistoMapFile(TString mapfile){  //Now this is called at the QwGUIMain
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
  /*
   printf(" Injector  Device List\n" );
  for (Size_t i=0;i<fInjectorDevices.size();i++)
    for (Size_t j=0;j<fInjectorDevices.at(i).size();j++)
      printf("%s \n",fInjectorDevices.at(i).at(j).Data() );
  */

  LoadInjectorBCMCombo();//Load BCM list into the combo box
  LoadInjectorSCALERCombo();//Load SCALER list into the combo box
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
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

  //Injector bcm access frame
  dInjectorBCMFrame= new TGVerticalFrame(dControlsFrame,50,100);
  //Injector scaler access frame
  dInjectorSCALERFrame= new TGVerticalFrame(dControlsFrame,50,100);

 
  dButtonPosDiffMean = new TGTextButton(dControlsFrame, "Position Differences", BA_POS_DIFF);
  // dButtonPosDiffRms = new TGTextButton(dControlsFrame, "Position Differences RMS", BA_POS_DIFF_RMS);
  dButtonMeanPos = new TGTextButton(dControlsFrame, "Mean Beam Positions", BA_MEAN_POS);
  dButtonPosVariation = new TGTextButton(dControlsFrame, "BPM Effective Charge", BA_POS_VAR);
  dButtonCharge = new TGTextButton(dInjectorBCMFrame, "&BCM Yield/Asymmetry", BA_CHARGE);
  dButtonCharge->SetEnabled(kFALSE);
  
  dButtonInjectorSCALER = new TGTextButton(dInjectorSCALERFrame, "&SCALER Yield/Asymmetry", BA_HCSCALER);
  dButtonInjectorSCALER->SetEnabled(kFALSE);


  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dButtonPosDiffMean,dBtnLayout );
  //  dControlsFrame->AddFrame(dButtonPosDiffRms,dBtnLayout );
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

  

  dButtonPosDiffMean -> Associate(this);
  //  dButtonPosDiffRms -> Associate(this);
  dButtonMeanPos -> Associate(this);
  dButtonCharge -> Associate(this);
  dButtonPosVariation -> Associate(this);
  dButtonInjectorSCALER -> Associate(this);
  dComboBoxInjectorBCM->Associate(this);
  dComboBoxInjectorSCALER->Associate(this);

 
  dCanvas->GetCanvas()->SetBorderMode(0);
  // dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  // 				"QwGUIInjector",
  // 				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");


}

void QwGUIInjector::OnReceiveMessage(char *obj)
{

}

// void QwGUIInjector::OnObjClose(char *obj)
// {
//  //  if(!strcmp(obj,"dROOTFile")){
// // //     printf("Called QwGUIInjector::OnObjClose\n");

// //     dROOTCont = NULL;
// //   }
// }


// void QwGUIInjector::OnNewDataContainer()
// {


// };

// void QwGUIInjector::OnRemoveThisTab()
// {

// };



/*This function plots the mean and rms of the bpm effective charge/four wire sum*/

void QwGUIInjector::BPM_EffectiveCharge()
{
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;

  char histo[128];
  
  Int_t xcount = 0;
  //Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;
  Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPMSTRIPLINE).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  TString dummyname;

  Bool_t ldebug = kFALSE;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
 
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetTitleSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"x");
  gStyle->SetTitleOffset(1.0,"x");
  gStyle->SetTitleOffset(0.5,"y");

   while (1){ 
     PosVariation[0] = new TH1F("Eff_Asym_yield", "Eff_Charge Asym Mean Variation",BPMSTriplinesCount , min_range, max_range);
     PosVariation[1] = new TH1F("Eff_Asym_rms", "Eff_Charge Asym RMS Variation",BPMSTriplinesCount , min_range, max_range); 
     for(Int_t p = 0; p <BPMSTriplinesCount ; p++) {
       sprintf (histo, "asym_%s_EffectiveCharge_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
       histo1= (TH1F *)dMapFile->Get(histo); 
       if (histo1!=NULL){
	 xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	 if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	 histo1->SetName(histo);
	 
	 dummyname = histo1->GetName();
	 
	 dummyname.Replace(0,9," ");
	 dummyname.ReplaceAll("_EffectiveCharge_hw", "");
	 PosVariation[0] -> SetBinContent(xcount, histo1->GetMean()*1e6);
	 PosVariation[0] -> SetBinError  (xcount, histo1->GetMeanError()*1e6);
	 PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	 PosVariation[1] -> SetBinContent(xcount, histo1->GetRMS()*1e6);
	 PosVariation[1] -> SetBinError  (xcount, 0);
	 PosVariation[1] -> GetXaxis()->SetBinLabel(xcount, dummyname);

	 if(ldebug) SummaryHist(histo1);
	 delete histo1; histo1= NULL;
       }
      
//        sprintf (histo, "yield_%s_EffectiveCharge_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
//        histo2= (TH1F *)dMapFile->Get(histo); 
//        if(histo2!=NULL){		
// 	 ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
// 	 if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
// 	 histo2->SetName(histo);
// 	 dummyname = histo2->GetName();
// 	 dummyname.Replace(0,9," ");
// 	 dummyname.ReplaceAll("_EffectiveCharge_hw", "");
// 	 PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
// 	 PosVariation[1] -> SetBinError  (ycount, histo2->GetRMS());
// 	 PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
// 	 if(ldebug) SummaryHist(histo2);
// 	 delete histo2; histo2= NULL; 
//        }
      
	  
    }
    xcount = 0;
    //ycount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE); 
    PosVariation[0] -> SetTitle("Eff_Charge Asymmetry Mean");
    PosVariation[0] -> GetYaxis() -> SetTitle("ppm");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("Eff_Charge Asymmetry RMS");
    PosVariation[1] -> GetYaxis()-> SetTitle ("ppm");
    PosVariation[1] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[1] -> Draw("E1");

    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <2 ; p++){
      delete PosVariation[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;
}


/* This function plots the bcm charge asymmetry and yield */

void QwGUIInjector::PlotChargeAsym()
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
     if (fCurrentBCMIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s_hw",fInjectorDevices.at(VQWK_BCM).at(fCurrentBCMIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s_hw",fInjectorDevices.at(VQWK_BCM).at(fCurrentBCMIndex).Data() );
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
  

   printf("---------------PlotChargeAsym()--------------------\n");

   return;
}


/* This function plots the position differences mean and rms  in X and Y.*/

void QwGUIInjector::PositionDifferences(){
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
 
  gStyle->SetLabelSize(0.1,"x");
  gStyle->SetLabelSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"x");
  gStyle->SetTitleOffset(1.0,"x");
  gStyle->SetTitleOffset(0.5,"y");
    

   while (1){ 
     PosVariation[0] = new TH1F("INDiffX", "X Difference Mean Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariation[1] = new TH1F("INDiffY", "Y Difference Mean variation", BPMSTriplinesCount, min_range, max_range); 
     PosVariationRms[0] = new TH1F("INDiffRmsX", "X Difference Rms Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariationRms[1] = new TH1F("INDiffRmsY", "Y Difference Rms variation", BPMSTriplinesCount, min_range, max_range); 
  
   for(Int_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      //sprintf (histo, "asym_%sX_hw",fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() );
      sprintf (histo, "diff_%sX_hw",fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() ); 
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
	PosVariationRms[0] -> SetBinContent(xcount, histo1->GetRMS());
	PosVariationRms[0] -> SetBinError  (xcount, 0.0);
	PosVariationRms[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	if(ldebug) SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      //sprintf (histo, "asym_%sY_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
      sprintf (histo, "diff_%sY_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
      histo2= (TH1F *)dMapFile->Get(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2->GetMean());
	PosVariation[1] -> SetBinError  (ycount, histo2->GetRMS());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	PosVariationRms[1] -> SetBinContent(ycount, histo2->GetRMS());
	PosVariationRms[1] -> SetBinError  (ycount, 0.0);
	PosVariationRms[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	if(ldebug) SummaryHist(histo2);
	delete histo2; histo2= NULL; 
      }
	  
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,4);


    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("");
    PosVariation[0] -> GetYaxis() -> SetTitle("BPM #DeltaX (mm)");
    //PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
    PosVariation[0] -> Draw("E1");
    //gPad->Update();
    //mc->Modified();
    //mc->Update();
    

    mc->cd(2);
    PosVariationRms[0] -> SetMarkerStyle(20);
    PosVariationRms[0] -> SetStats(kFALSE);
    PosVariationRms[0] -> SetTitle("");
    PosVariationRms[0] -> GetYaxis() -> SetTitle("BPM #DeltaX RMS (mm)");
    //PosVariationRms[0] -> GetXaxis() -> SetTitle("BPM X RMS");
    PosVariationRms[0] -> Draw("E1");


    mc->cd(3);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetTitle("");
    PosVariation[1] -> GetYaxis()-> SetTitle ("BPM #DeltaY (mm)");
    // PosVariation[1] -> GetXaxis() -> SetTitle("BPM Y");
    PosVariation[1] -> Draw("E1");
    

    mc->cd(4);
    PosVariationRms[1] -> SetMarkerStyle(20);
    PosVariationRms[1] -> SetStats(kFALSE);
    PosVariationRms[1] -> SetTitle("");
    PosVariationRms[1] -> GetYaxis() -> SetTitle("BPM #DeltaY RMS (mm)");
    // PosVariationRms[1] -> GetXaxis() -> SetTitle("BPM Y RMS");
    PosVariationRms[1] -> Draw("E1");


    gPad->Update();
    mc->Modified();
    mc->Update();
    for (Int_t p = 0; p <NUM_POS ; p++){
      delete PosVariation[p];
      delete PosVariationRms[p];
    }
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }


  return;



}
  
/* This function plots the mean bpm positions in X and Y */

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
    for(Int_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      sprintf (histo, "%sX_hw",fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data() );
      histo1= (TH1F *)dMapFile->Get(histo); 
      if (histo1!=NULL) {
	xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	histo1->SetName(histo);
	    
	dummyname = histo1->GetName();
	    
	dummyname.Replace(0,4," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1->GetMean());
	PosVariation[0] -> SetBinError  (xcount, histo1->GetRMS());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	if(ldebug) SummaryHist(histo1);
	delete histo1; histo1= NULL;
      }
	  
      sprintf (histo, "%sY_hw", fInjectorDevices.at(VQWK_BPMSTRIPLINE).at(p).Data());
      histo2= (TH1F *)dMapFile->Get(histo); 
      if(histo2!=NULL){		
	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	histo2->SetName(histo);
	dummyname = histo2->GetName();
	dummyname.Replace(0,4," ");
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
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
   }

  return;

};  
  

// void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
// {

// }

void QwGUIInjector::PlotSCALER(){
  
};

void QwGUIInjector::SetComboIndex(Int_t cmb_id, Int_t id){

  if (cmb_id==CMB_INJECTORBCM)
    fCurrentBCMIndex=id;

  if (cmb_id==CMB_INJECTORSCALER)
    fCurrentSCALERIndex=id;
};

void QwGUIInjector::LoadInjectorBCMCombo(){
  dComboBoxInjectorBCM->RemoveAll();
  //printf("QwGUIInjector::LoadInjectorBCMCombo \n");
  std::size_t i=0;
  for(i=0;i<fInjectorDevices.at(VQWK_BCM).size();i++){
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
  std::size_t i=0;
  for(i=0;i<fInjectorDevices.at(SCALER_HALO).size();i++){
    dComboBoxInjectorSCALER->AddEntry(fInjectorDevices.at(SCALER_HALO).at(i),i);
    //printf("%s \n",fInjectorDevices.at(SCALER_HALO).at(i).Data());
  }
  if (fInjectorDevices.at(SCALER_HALO).size()>0)
    dButtonInjectorSCALER->SetEnabled(kTRUE);

  fCurrentSCALERIndex=-1;
};


Bool_t QwGUIInjector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  if(dMapFileFlag) {
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
		case BA_POS_DIFF:
		  //printf("text button id %ld pressed\n", parm1);
		  PositionDifferences();
		  break;
		  
		case BA_CHARGE:
		  //printf("text button id %ld pressed\n", parm1);
		  PlotChargeAsym();//PlotPositionDiff(); 
		  break;

		case BA_POS_VAR:
		  //printf("PlotPosData() button id %ld pressed\n", parm1);
		  BPM_EffectiveCharge();
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
	      case CMB_INJECTORBCM:
		SetComboIndex(CMB_INJECTORBCM,parm2);
		break;
	      case CMB_INJECTORSCALER:
		SetComboIndex(CMB_INJECTORSCALER,parm2);
		break;	      
	      }
	    }
	    break;
	    
	default:
	  break;
	}
       default:
	break;
    }
  }
  return kTRUE;
}

