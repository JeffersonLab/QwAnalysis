#include "QwGUIInjector.h"


#include "TG3DLine.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TGraphErrors.h"


ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BM_POS_DIFF,
  BM_MEAN_POS,
  BM_POS,
  BM_EFF_Q,
  BM_CHARGE,
  BM_HCSCALER,
};

//INJECTOR_DET_TYPES is the size of the enum
enum QwGUIInjectorDeviceTypes {
  UNKNOWN_TYPE,
  VQWK_BPM,
  VQWK_BCM,
  VQWK_QPD,
  VQWK_LINA,
};

//Combo box
enum QwGUIHallCBeamlinePlotsComboBox {
  CMB_INJECTORCHARGE,
  CMB_INJECTORSCALER,
  CMB_INJECTORPOS,
};


QwGUIInjector::QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame               = NULL;
  dControlsFrame          = NULL;
  dCanvas                 = NULL;  
  dBtnLayout              = NULL;
  dButtonPosDiffMean      = NULL;
  dButtonMeanPos          = NULL;
  dButtonEffCharge        = NULL;
  dComboBoxChargeMonitors = NULL;
  dComboBoxScalers        = NULL;
  dComboBoxPosMonitors    = NULL;
  not_found               = NULL;
  dPositionMon            = NULL;
  dChargeMon              = NULL;
  dScaler                 = NULL;

  PosVariation[0] = NULL;
  PosVariation[1] = NULL;

  AddThisTab(this);

  HistArray.Clear();
  DataWindowArray.Clear();

  SetHistoAccumulate(1);
  SetHistoReset(0);

}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)             delete dTabFrame;
  if(dControlsFrame)        delete dControlsFrame;
  if(dCanvas)               delete dCanvas;  
  if(dBtnLayout)            delete dBtnLayout;
  if(dButtonPosDiffMean)    delete dButtonPosDiffMean;
  if(dButtonEffCharge)      delete dButtonEffCharge;
  if(dButtonMeanPos)        delete dButtonMeanPos; 
  if(dComboBoxChargeMonitors)delete dComboBoxChargeMonitors;
  if(dComboBoxPosMonitors)   delete dComboBoxPosMonitors;
  if(dComboBoxScalers)       delete dComboBoxScalers;
  if(not_found)              delete not_found;
  if(dPositionMon)           delete dPositionMon;
  if(dChargeMon)             delete dChargeMon;
  if(dScaler)                delete dScaler;

  delete [] PosVariation;
}


/* This function reads in the devices from the detector map file and loads them in to the individual combo boxes.
It is called at the QwGUIMain. 
*/
void QwGUIInjector::LoadHistoMapFile(TString mapfile){

  TString varname, varvalue;
  TString modtype, namech,dettype;
  Int_t count_bpms=0;
  Int_t count_qpds=0;
  Int_t count_lina=0;

  fInjectorDevices.resize(INJECTOR_DET_TYPES);
  fInjectorPositionType.resize(0);

 //Open the detector map file
  QwParameterFile mapstr(mapfile.Data());
 
  // loop over the rows
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    else{
      modtype   = mapstr.GetTypedNextToken<TString>();	// module type
      if (modtype=="VQWK" || modtype=="SCALER"){
	mapstr.GetTypedNextToken<TString>();	//slot number
	mapstr.GetTypedNextToken<TString>();	//channel number
	dettype=mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	namech.ToLower();

	// Now sort out detectors to the two categories of charge monitors and position monitors.
	if (modtype=="VQWK"){
	  if(dettype=="bpmstripline" || dettype=="qpd" || dettype =="lineararray"){
	    namech.Remove(namech.Length()-2,2);
	    if(dettype=="bpmstripline"){
	      count_bpms=count(fInjectorDevices.at(VQWK_BPM).begin(),fInjectorDevices.at(VQWK_BPM).end(),namech);
	      if(!count_bpms) fInjectorDevices.at(VQWK_BPM).push_back(namech);
	    }
	    if(dettype=="qpd"){
	      count_qpds=count(fInjectorDevices.at(VQWK_QPD).begin(),fInjectorDevices.at(VQWK_QPD).end(),namech);
	      if(!count_qpds) fInjectorDevices.at(VQWK_QPD).push_back(namech);
	      dButtonMeanPos->SetEnabled(kFALSE);
	      dButtonEffCharge->SetEnabled(kFALSE);
	      dButtonPosDiffMean->SetEnabled(kFALSE);    
	    }
	    if (dettype=="lineararray"){
	      count_lina=count(fInjectorDevices.at(VQWK_LINA).begin(),fInjectorDevices.at(VQWK_LINA).end(),namech);	
	      if(!count_lina) fInjectorDevices.at(VQWK_LINA).push_back(namech);	  
	      dButtonMeanPos->SetEnabled(kFALSE);
	      dButtonEffCharge->SetEnabled(kFALSE);    
	      dButtonPosDiffMean->SetEnabled(kFALSE);    
	    }
	  }
	  else if (dettype=="bcm"){
	    fInjectorDevices.at(VQWK_BCM).push_back(namech);
	    dComboBoxChargeMonitors->SetEnabled(kTRUE); 
	  }
	}
      }      
    }
  }

  // Fill the comboboxes using the information from the map file
  FillComboBoxes();

};

void QwGUIInjector::MakeLayout()
{

  // Start by cleaning up any exsisting layouts.
  SetCleanup(kDeepCleanup);

  //Draw the main tab frame.
  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  //The main frame will be divided in to two parts.
  // 1. The controls frame
  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  //Draw a seperator
  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  // 2. The canvas
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

   // Create buttons
  // Buttons to display overview of beam properties along beamline
  dButtonPosDiffMean = new TGTextButton(dControlsFrame, "Position Differences", BM_POS_DIFF);
  dButtonMeanPos = new TGTextButton(dControlsFrame, "Beam Position", BM_MEAN_POS);
  dButtonEffCharge = new TGTextButton(dControlsFrame, "BPM Effective Charge", BM_EFF_Q);


  // Add the buttons to the frame.
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dButtonPosDiffMean,dBtnLayout );
  dControlsFrame->AddFrame(dButtonEffCharge, dBtnLayout);
  dControlsFrame->AddFrame(dButtonMeanPos, dBtnLayout);

  // Create combo boxes
  // combo box for charge monitors
  dComboBoxChargeMonitors=new TGComboBox(dControlsFrame,CMB_INJECTORCHARGE);
  dComboBoxChargeMonitors->Resize(50,20);//To make it better looking
  dComboBoxChargeMonitors->SetEnabled(kFALSE); 

  // combo box for position monitors
  dComboBoxPosMonitors=new TGComboBox(dControlsFrame,CMB_INJECTORPOS);
  dComboBoxPosMonitors->Resize(50,20);//To make it better looking
  
  // SCALER  combo box
  dComboBoxScalers=new TGComboBox(dControlsFrame,CMB_INJECTORSCALER);
  dComboBoxScalers->Resize(50,20);//To make it better looking
  dComboBoxScalers->SetEnabled(kFALSE);

  // Create labels
  dPositionMon = new TGLabel(dControlsFrame,"Position Monitors");
  dChargeMon = new TGLabel(dControlsFrame,"Charge Monitors");
  dScaler = new TGLabel(dControlsFrame,"Scalers");

  //add components to the Injector  detector frame
  dControlsFrame->AddFrame(dChargeMon,dBtnLayout);
  dControlsFrame->AddFrame(dComboBoxChargeMonitors, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dControlsFrame->AddFrame(dPositionMon,dBtnLayout);
  dControlsFrame->AddFrame(dComboBoxPosMonitors, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dControlsFrame->AddFrame(dScaler,dBtnLayout);  
  dControlsFrame->AddFrame(dComboBoxScalers, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));

  dButtonPosDiffMean -> Associate(this);
  dButtonMeanPos -> Associate(this);
  dButtonEffCharge -> Associate(this);
  dComboBoxChargeMonitors->Associate(this);
  dComboBoxPosMonitors->Associate(this);
  dComboBoxScalers->Associate(this);
  dCanvas->GetCanvas()->SetBorderMode(0);

  not_found  = new TText(); 
  not_found->SetTextColor(2);
  not_found->SetTextSize(0.1);


}

void QwGUIInjector::OnReceiveMessage(char *obj)
{
  //do nothing
}

/* This function grabs the index of the device from the selected combo box*/
void QwGUIInjector::SetComboIndex(Int_t cmb_id, Int_t id){

  if (cmb_id==CMB_INJECTORCHARGE)
    fCurrentChargeMonitorIndex=id;

  if (cmb_id==CMB_INJECTORPOS)
    fCurrentPositionMonitorIndex=id;

  if (cmb_id==CMB_INJECTORSCALER)
    fCurrentSCALERIndex=id;
};


/* This function fills the combo boxes*/
void QwGUIInjector::FillComboBoxes(){
  
  size_t i=0;
  size_t k=0;
  
  // clear the combo boxes
  dComboBoxChargeMonitors->RemoveAll();
  dComboBoxPosMonitors->RemoveAll();
  dComboBoxScalers->RemoveAll();

  // Don't change the filling oreder. If you change this it will effect other functions that
  // read the combo boxes to get the different detectors/

  // Fill the charge monitors
  for(i=0;i<fInjectorDevices.at(VQWK_BCM).size();i++){
    dComboBoxChargeMonitors->AddEntry(fInjectorDevices.at(VQWK_BCM).at(i),i);
  }

  // Fill the position monitors
  for(i=0;i<fInjectorDevices.at(VQWK_BPM).size();i++){
    dComboBoxPosMonitors->AddEntry(fInjectorDevices.at(VQWK_BPM).at(i),i);
    fInjectorPositionType.push_back("VQWK_BPM");
  }
  k= dComboBoxPosMonitors->GetNumberOfEntries();
  
  for(i=0;i<fInjectorDevices.at(VQWK_QPD).size();i++){
    dComboBoxPosMonitors->AddEntry(fInjectorDevices.at(VQWK_QPD).at(i),k+i);
    fInjectorPositionType.push_back("VQWK_QPD");
  }
  k= dComboBoxPosMonitors->GetNumberOfEntries();

  for(i=0;i<fInjectorDevices.at(VQWK_LINA).size();i++){
    dComboBoxPosMonitors->AddEntry(fInjectorDevices.at(VQWK_LINA).at(i),k+i);
    fInjectorPositionType.push_back("VQWK_LINA");
  }
  
  fCurrentPositionMonitorIndex=-1;

  // done!

}


/*This function plots the mean and rms of the bpm effective charge/four wire sum*/

void QwGUIInjector::BPM_EffectiveCharge()
{
  char histo[128];
  Int_t xcount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;
  const Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPM).size();
  Double_t max_range = (Double_t)BPMSTriplinesCount - offset ; 

  TString dummyname; 
  Bool_t ldebug = kFALSE;

  std::vector<TH1F *> histo1;//_array;
  histo1.resize(BPMSTriplinesCount);
  std::vector<TH1F *> histo1_buff;//_array;
  histo1_buff.resize(BPMSTriplinesCount);
  
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
       
       if (GetHistoPause()==0){
	 sprintf (histo, "asym_%s_EffectiveCharge_hw", fInjectorDevices.at(VQWK_BPM).at(p).Data());
	 histo1[p]= (TH1F *)dMapFile->Get(histo); 
       }
       if (histo1[p]!=NULL){
	 if (GetHistoReset()){
	   histo1_buff[p]=(TH1F*)histo1[p]->Clone(Form("%s_buff",histo1[p]->GetName()));
	   *histo1[p]=*histo1[p]-*histo1_buff[p];
	   if (p==BPMSTriplinesCount-1)//once all histo are buffered set the reser state
	     SetHistoReset(0);
	 }else if (GetHistoAccumulate()==0){
	   *histo1[p]=*histo1[p]-*histo1_buff[p];
	 }	    
	 xcount++; 
	 if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	 histo1[p]->SetName(histo);
	 
	 dummyname = histo1[p]->GetName();
	 
	 dummyname.Replace(0,9," ");
	 dummyname.ReplaceAll("_EffectiveCharge_hw", "");
	 PosVariation[0] -> SetBinContent(xcount, histo1[p]->GetMean()*1e6);
	 PosVariation[0] -> SetBinError  (xcount, histo1[p]->GetMeanError()*1e6);
	 PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	 PosVariation[1] -> SetBinContent(xcount, histo1[p]->GetRMS()*1e6);
	 PosVariation[1] -> SetBinError  (xcount, 0);
	 PosVariation[1] -> GetXaxis()->SetBinLabel(xcount, dummyname);

	 if(ldebug) SummaryHist(histo1[p]);
       }
	  
    }
    xcount = 0;
    mc->Clear();
    mc->Divide(1,2);



    
    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetMarkerColor(4);
    PosVariation[0] -> SetLineColor(2);
    PosVariation[0] -> SetStats(kFALSE); 
    PosVariation[0] -> SetTitle("Eff_Charge Asymmetry Mean");
    PosVariation[0] -> GetYaxis() -> SetTitle("ppm");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM ");
    PosVariation[0] -> Draw("E1");
    
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetLineColor(28); 
    PosVariation[1] -> SetFillColor(28); 
    PosVariation[1] -> SetFillStyle(3002); 
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


/* This function plots the asymmetry and yield of charge monitors */

void QwGUIInjector::PlotChargeMonitors()
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
     if (fCurrentChargeMonitorIndex<0)
       break;
     if (GetHistoPause()==0){
       sprintf (histo, "asym_%s_hw",fInjectorDevices.at(VQWK_BCM).at(fCurrentChargeMonitorIndex).Data() );
       histo1= (TH1F *)dMapFile->Get(histo);
       sprintf (histo, "yield_%s_hw",fInjectorDevices.at(VQWK_BCM).at(fCurrentChargeMonitorIndex).Data() );
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
      if((histo1!=NULL) && ((histo1->GetEntries())!= 0)){
	histo1->Draw();
	histo1->SetFillColor(28);
	histo1->SetFillStyle(3002);
	histo1->SetLineColor(28);
	if(ldebug) SummaryHist(histo1);
      } 
      else{
	gPad->Clear();
	if (histo1->GetEntries()== 0)
	  not_found ->DrawText(0.3,0.5,"Empty!");
	else
	  not_found ->DrawText(0.3,0.5,"Not Found!");
	not_found ->Draw();
      }

      mc->cd(2);
      if((histo2!=NULL) && ((histo2->GetEntries())!= 0)){
	histo2->Draw();
	histo2->SetFillColor(39);
	histo2->SetFillStyle(3002);
	histo2->SetLineColor(39);
	if(ldebug) SummaryHist(histo2);
      } 
      else{
	gPad->Clear();
	if (histo2->GetEntries()== 0)
	  not_found ->DrawText(0.3,0.5,"Empty!");
	else
	  not_found ->DrawText(0.3,0.5,"Not Found!");
	not_found ->Draw();
      }

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   printf("---------------PlotChargeMonitors()--------------------\n");

   return;
}


/* This function plots the individual paramters of beam position monitors*/

void QwGUIInjector::PlotPositionMonitors(){

  Bool_t ldebug = false;

  TH1F *histo[11];
  TH1F *histo_buff[11]; 

  Double_t mean[8];
  Double_t error[8];
  Double_t points[8];
  Double_t fakeerror[8];

  TString hist[11];


  TCanvas *mc = NULL;
  TPad*pad1;
  TPad*pad2;
  TGraphErrors * value;

  not_found ->SetTextSize(0.06);

  TString name;
  Int_t loc;
  size_t size = 10;

  gStyle->SetStatH(0.3);
  gStyle->SetStatW(0.5);     
  gStyle->SetOptStat(1110);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");

  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->cd();

  // Linear array has different number of pads and properties than postion monitors.
  if(fInjectorPositionType.at(fCurrentPositionMonitorIndex)=="VQWK_LINA"){
    pad1 = new TPad("pad1","pad1",0.005,0.465,0.995,0.995);
    pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.475);
    
    pad1->Draw();
 
    pad2->Draw();
    pad2->Divide(3,1);
    size = 11;
  }
  else {
    pad1 = new TPad("pad1","pad1",0.005,0.665,0.995,0.995);
    pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.675);
    
    pad1->Draw();
    pad1->Divide(4,1);
    pad2->Draw();
    pad2->Divide(3,2);
  }

  SetHistoDefaultMode();//bring the histo mode to accumulate mode

  while(1){
    if (fCurrentPositionMonitorIndex<0)
      break;

    // Continue reading the histograms from the map file until PAUSE button is clicked.
    if (GetHistoPause()==0){
      if (fInjectorPositionType.at(fCurrentPositionMonitorIndex)=="VQWK_BPM"){
	name = fInjectorDevices.at(VQWK_BPM).at(fCurrentPositionMonitorIndex).Data();
	hist[0]=Form("%sXP_hw",name.Data());
	histo[0]= (TH1F *)dMapFile->Get(hist[0]);
	hist[1]=Form("%sXM_hw",name.Data());
	histo[1]= (TH1F *)dMapFile->Get(hist[1]);
	hist[2]=Form("%sYP_hw",name.Data());
	histo[2]= (TH1F *)dMapFile->Get(hist[2]);
	hist[3]=Form("%sYM_hw",name.Data());
	histo[3]= (TH1F *)dMapFile->Get(hist[3]);
	hist[4]=Form("%sX_hw",name.Data());
	histo[4]= (TH1F *)dMapFile->Get(hist[4]);
	hist[5]=Form("%sY_hw",name.Data());
	histo[5]= (TH1F *)dMapFile->Get(hist[5]);
	hist[6]=Form("%s_EffectiveCharge_hw",name.Data());
	histo[6]= (TH1F *)dMapFile->Get(hist[6]);
	hist[7]=Form("diff_%sX_hw",name.Data());
	histo[7]= (TH1F *)dMapFile->Get(hist[7]);
	hist[8]=Form("diff_%sY_hw",name.Data());
	histo[8]= (TH1F *)dMapFile->Get(hist[8]);
	hist[9]=Form("asym_%s_EffectiveCharge_hw",name.Data());
	histo[9]= (TH1F *)dMapFile->Get(hist[9]);
      }
      else if(fInjectorPositionType.at(fCurrentPositionMonitorIndex)=="VQWK_QPD"){
	loc = fCurrentPositionMonitorIndex-fInjectorDevices.at(VQWK_BPM).size();
	name = fInjectorDevices.at(VQWK_QPD).at(loc).Data();
	hist[0]=Form("%sTL_hw",name.Data());
	histo[0]= (TH1F *)dMapFile->Get(hist[0]);
	hist[1]=Form("%sTR_hw",name.Data());
	histo[1]= (TH1F *)dMapFile->Get(hist[1]);
	hist[2]=Form("%sBL_hw",name.Data());
	histo[2]= (TH1F *)dMapFile->Get(hist[2]);
	hist[3]=Form("%sBR_hw",name.Data());
	histo[3]= (TH1F *)dMapFile->Get(hist[3]);
	hist[4]=Form("%sX_hw",name.Data());
	histo[4]= (TH1F *)dMapFile->Get(hist[4]);
	hist[5]=Form("%sY_hw",name.Data());
	histo[5]= (TH1F *)dMapFile->Get(hist[5]);
	hist[6]=Form("%s_EffectiveCharge_hw",name.Data());
	histo[6]= (TH1F *)dMapFile->Get(hist[6]);
	hist[7]=Form("diff_%sX_hw",name.Data());
	histo[7]= (TH1F *)dMapFile->Get(hist[7]);
	hist[8]=Form("diff_%sY_hw",name.Data());
	histo[8]= (TH1F *)dMapFile->Get(hist[8]);
	hist[9]=Form("asym_%s_EffectiveCharge_hw",name.Data());
	histo[9]= (TH1F *)dMapFile->Get(hist[9]);
      } 
      else if(fInjectorPositionType.at(fCurrentPositionMonitorIndex)=="VQWK_LINA"){
	loc = fCurrentPositionMonitorIndex-fInjectorDevices.at(VQWK_BPM).size()-fInjectorDevices.at(VQWK_QPD).size();
	name = fInjectorDevices.at(VQWK_LINA).at(loc).Data();
	hist[0]=Form("%sp1_hw",name.Data());
	histo[0]= (TH1F *)dMapFile->Get(hist[0]);
	hist[1]=Form("%sp2_hw",name.Data());
	histo[1]= (TH1F *)dMapFile->Get(hist[1]);
	hist[2]=Form("%sp3_hw",name.Data());
	histo[2]= (TH1F *)dMapFile->Get(hist[2]);
	hist[3]=Form("%sp4_hw",name.Data());
	histo[3]= (TH1F *)dMapFile->Get(hist[3]);
	hist[4]=Form("%sp5_hw",name.Data());
	histo[4]= (TH1F *)dMapFile->Get(hist[4]);
	hist[5]=Form("%sp6_hw",name.Data());
	histo[5]= (TH1F *)dMapFile->Get(hist[5]);
	hist[6]=Form("%sp7_hw",name.Data());
	histo[6]= (TH1F *)dMapFile->Get(hist[6]);
	hist[7]=Form("%sp8_hw",name.Data());
	histo[7]= (TH1F *)dMapFile->Get(hist[7]);
	hist[8]=Form("%sRelMean_hw",name.Data());
	histo[8]= (TH1F *)dMapFile->Get(hist[8]);
	hist[9]=Form("%sRelVariance_hw",name.Data());
	histo[9]= (TH1F *)dMapFile->Get(hist[9]);    
 	hist[10]=Form("diff_%sRelMean_hw",name.Data());
 	histo[10]= (TH1F *)dMapFile->Get(hist[10]); 
	
      }
      else 
	return;
      
    } // The deafult is to accumulate always with no reset.


     // if RESET is clicked : reset the histogram and accumulate
    if (GetHistoReset()){
      for(size_t i=0;i<size;i++){
	if (histo[i]!=NULL ){
	  histo_buff[i]=(TH1F*)histo[i]->Clone(Form("%s_buff",histo[i]->GetName()));
	  *histo[i]=*histo[i]-*histo_buff[i];
	}
      }
      SetHistoReset(0);
    }
    else if (GetHistoAccumulate()==0){ // Accumulate after reset
      for(size_t i=0;i<size;i++){
	if (histo[i]!=NULL ){
	  *histo[i]=*histo[i]-*histo_buff[i];
	}
      }
    }

    // Now draw the histograms
    //For different detectors the number of histograms are different
    if(fInjectorPositionType.at(fCurrentPositionMonitorIndex)=="VQWK_LINA"){  
      
      pad1->cd();
      for(size_t i=0;i<8;i++){

	if((histo[i]!=NULL) && ((histo[i]->GetEntries())!= 0)){
	  mean[i]= histo[i]->GetMean();
	  error[i]= histo[i]->GetMeanError();
	  points[i]= i+1;
	  fakeerror[i]= 0.0;
	}
	//PosVariation[0] -> GetXaxis()->SetBinLabel(i+1, dummyname);

//  	pad1->cd(i+1);
// 	if((histo[i]!=NULL) && ((histo[i]->GetEntries())!= 0)){
// 	  histo[i]->Draw();
// 	  histo[i]->SetFillColor(8);
// 	  histo[i]->SetFillStyle(3002);
// 	  histo[i]->SetLineColor(8);

// 	  if(ldebug) SummaryHist(histo[i]);
// 	} 
//  	else{
// 	  gPad->Clear();
// 	  not_found ->DrawText(0.3,0.5,"Empty!");
// 	  not_found ->Draw();
//  	}
      }

      value = new TGraphErrors(8, points,mean,fakeerror,error);
      value->SetMarkerColor(4);
      value->SetMarkerStyle(22);
      value->SetMarkerSize(1.5);
      value->SetLineColor(2);
      value->SetTitle("Mean value of pads");

      pad1->cd(1);
      value->Draw("APE"); 
	
      pad2->cd();
      // Draw Mean and Variance 
      for(size_t i=8;i<11;i++){
	pad2->cd(i-7);
	if(histo[i]!=NULL && ((histo[i]->GetEntries())!= 0)){
	  histo[i]->Draw();
	  histo[i]->SetFillColor(46);
	  histo[i]->SetFillStyle(3002);
	  histo[i]->SetLineColor(46);
	  if(ldebug) SummaryHist(histo[i]);
	} 
	else{
	  gPad->Clear();
	  not_found ->DrawText(0.005,0.3,Form("%s Empty!",hist[i].Data()));
	  not_found ->Draw();
	}
      }     
    }   
    else{
      
      // Draw wires/pads
      pad1->cd();
      for(size_t i=0;i<4;i++){
	pad1->cd(i+1);
	if((histo[i]!=NULL) && ((histo[i]->GetEntries())!= 0)){
	  histo[i]->Draw();
	  histo[i]->SetFillColor(8);
	  histo[i]->SetFillStyle(3002);
	  histo[i]->SetLineColor(8);
	  if(ldebug) SummaryHist(histo[i]);
	} 
	else{
	  gPad->Clear();
	  not_found ->DrawText(0.005,0.3,Form("%s Empty",hist[i].Data()));
	  not_found ->Draw();
	}
      }
      
      pad2->cd();
      // Draw X,Y and effective charge 
      for(size_t i=4;i<7;i++){
	pad2->cd(i-3);
	if(histo[i]!=NULL && ((histo[i]->GetEntries())!= 0)){
	  histo[i]->Draw();
	  histo[i]->SetFillColor(38);
	  histo[i]->SetFillStyle(3002);
	  histo[i]->SetLineColor(38);
	  if(ldebug) SummaryHist(histo[i]);
	} 
	else{
	  gPad->Clear();
	  not_found ->DrawText(0.005,0.3,Form("%s Empty",hist[i].Data()));
	  not_found ->Draw();
	}
      }
      // Draw dX, dY and effective charge asymmetry
      for(size_t i=7;i<10;i++){
	pad2->cd(i-3);
	if(histo[i]!=NULL && ((histo[i]->GetEntries())!= 0) ){
	  histo[i]->Draw();
	  histo[i]->SetFillColor(46);
	  histo[i]->SetFillStyle(3002);
	  histo[i]->SetLineColor(46);
	  if(ldebug) SummaryHist(histo[i]);
	} 
	else{
	  gPad->Clear();
	  not_found ->DrawText(0.005,0.3,Form("%s Empty",hist[i].Data()));
	  not_found ->Draw();
	}
      }
    }
    mc->Modified();
    mc->Update();
    
    
    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  
  printf("---------------PlotPositionMonitors()--------------------\n");
  
  return;
  
};  


/* This function plots the position differences mean and rms  in X and Y.*/

void QwGUIInjector::PositionDifferences(){

  char chisto1[150];
  char chisto2[150]; 

  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPM).size();
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
 
  gStyle->SetLabelSize(0.1,"x");
  gStyle->SetLabelSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"x");
  gStyle->SetTitleOffset(1.0,"x");
  gStyle->SetTitleOffset(0.5,"y");
    
  SetHistoDefaultMode();//bring the histo mode to accumulate mode

   while (1){ 
     PosVariation[0] = new TH1F("INDiffX", "X Difference Mean Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariation[1] = new TH1F("INDiffY", "Y Difference Mean variation", BPMSTriplinesCount, min_range, max_range); 
     PosVariationRms[0] = new TH1F("INDiffRmsX", "X Difference Rms Variation", BPMSTriplinesCount, min_range, max_range);
     PosVariationRms[1] = new TH1F("INDiffRmsY", "Y Difference Rms variation", BPMSTriplinesCount, min_range, max_range); 
  
   for(Int_t p = 0; p <BPMSTriplinesCount ; p++) 
    {
      if (GetHistoPause()==0){
	sprintf (chisto1, "diff_%sX_hw",fInjectorDevices.at(VQWK_BPM).at(p).Data() ); 
	histo1[p]= (TH1F *)dMapFile->Get(chisto1);
	sprintf (chisto2, "diff_%sY_hw", fInjectorDevices.at(VQWK_BPM).at(p).Data());
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
	    
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[0] -> SetBinContent(xcount, histo1[p]->GetMean());
	PosVariation[0] -> SetBinError  (xcount, histo1[p]->GetMeanError());
	PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	PosVariationRms[0] -> SetBinContent(xcount, histo1[p]->GetRMS());
	PosVariationRms[0] -> SetBinError  (xcount, 0.0);
	PosVariationRms[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	if(ldebug) SummaryHist(histo1[p]);

	ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, chisto2);
	histo2[p]->SetName(chisto2);
	dummyname = histo2[p]->GetName();
	dummyname.Replace(0,9," ");
	dummyname.ReplaceAll("_hw", "");
	PosVariation[1] -> SetBinContent(ycount, histo2[p]->GetMean());
	PosVariation[1] -> SetBinError  (ycount, histo2[p]->GetMeanError());
	PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	PosVariationRms[1] -> SetBinContent(ycount, histo2[p]->GetRMS());
	PosVariationRms[1] -> SetBinError  (ycount, 0.0);
	PosVariationRms[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	if(ldebug) SummaryHist(histo2[p]);
      }  
	  
    }
    xcount = 0;
    ycount = 0;
    mc->Clear();
    mc->Divide(1,4);


    mc->cd(1);
    PosVariation[0] -> SetMarkerStyle(20);
    PosVariation[0] -> SetMarkerColor(4);
    PosVariation[0] -> SetLineColor(2);
    PosVariation[0] -> SetStats(kFALSE);
    PosVariation[0] -> SetTitle("");
    PosVariation[0] -> GetYaxis() -> SetTitle("BPM #DeltaX (mm)");
    PosVariation[0] -> Draw("E1");
    

    mc->cd(2);
    PosVariationRms[0] -> SetMarkerStyle(20);
    PosVariationRms[0] -> SetStats(kFALSE);
    PosVariationRms[0] -> SetLineColor(28); 
    PosVariationRms[0] -> SetFillColor(28);
    PosVariationRms[0] -> SetFillStyle(3002); 
    PosVariationRms[0] -> SetTitle("");
    PosVariationRms[0] -> GetYaxis() -> SetTitle("BPM #DeltaX RMS (mm)");
    PosVariationRms[0] -> Draw("E1");


    mc->cd(3);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetMarkerColor(4);
    PosVariation[1] -> SetLineColor(2);
    PosVariation[1] -> SetTitle("");
    PosVariation[1] -> GetYaxis()-> SetTitle ("BPM #DeltaY (mm)");
    PosVariation[1] -> Draw("E1");
    

    mc->cd(4);
    PosVariationRms[1] -> SetMarkerStyle(20);
    PosVariationRms[1] -> SetStats(kFALSE);
    PosVariationRms[1] -> SetLineColor(28); 
    PosVariationRms[1] -> SetFillColor(28);
    PosVariationRms[1] -> SetFillStyle(3002); 
    PosVariationRms[1] -> SetTitle("");
    PosVariationRms[1] -> GetYaxis() -> SetTitle("BPM #DeltaY RMS (mm)");
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
  
  char chisto1[150];
  char chisto2[150]; 

  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;

  Int_t BPMSTriplinesCount = fInjectorDevices.at(VQWK_BPM).size();
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
    for(Int_t p = 0; p <BPMSTriplinesCount ; p++) 
      {
	if (GetHistoPause()==0){
	  sprintf (chisto1, "%sX_hw",fInjectorDevices.at(VQWK_BPM).at(p).Data() );
	  histo1[p]= (TH1F *)dMapFile->Get(chisto1); 
	  sprintf (chisto2, "%sY_hw", fInjectorDevices.at(VQWK_BPM).at(p).Data());
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
    PosVariation[0] -> SetMarkerColor(4);
    PosVariation[0] -> SetLineColor(2);
    PosVariation[0] -> SetTitle("Mean BPM X Variation");
    PosVariation[0] -> GetYaxis() -> SetTitle("Pos (mm)");
    PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
    PosVariation[0] -> Draw("E1");
   
    mc->cd(2);
    PosVariation[1] -> SetMarkerStyle(20);
    PosVariation[1] -> SetStats(kFALSE);
    PosVariation[1] -> SetMarkerColor(4);
    PosVariation[1] -> SetLineColor(2);
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
  

/* This function will plot the scaler information*/
void QwGUIInjector::PlotSCALER(){
  // need to code here to display scalers..
};



/* This is the main function of the GUI which handles the communication between different GUI objects*/
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
		case BM_POS_DIFF:
		  //printf("text button id %ld pressed\n", parm1);
		  PositionDifferences();
		  break;
		  
		case BM_EFF_Q:
		  //printf("PlotPosData() button id %ld pressed\n", parm1);
		  BPM_EffectiveCharge();
		  break;
		  
		case BM_MEAN_POS:
		  //printf("PlotPosData() button id %ld pressed\n", parm1);
		  PlotBPMPositions();
		  break;
		  
		default:
		  break;
		}
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
	      case CMB_INJECTORCHARGE:
		{
		  SetComboIndex(CMB_INJECTORCHARGE,parm2);
		  PlotChargeMonitors();
		  break;
		}
	      case CMB_INJECTORPOS:
		{
		  SetComboIndex(CMB_INJECTORPOS,parm2);
		  PlotPositionMonitors(); 
		  break;
		}
	      case CMB_INJECTORSCALER:
		{
		  SetComboIndex(CMB_INJECTORSCALER,parm2);
		  break;
		}	      
	      default:
		break;
	      }
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

