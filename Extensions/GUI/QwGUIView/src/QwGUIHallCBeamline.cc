#include "QwGUIHallCBeamline.h"

#include <TG3DLine.h>
#include "TGaxis.h"

ClassImp(QwGUIHallCBeamline);

enum QwGUIHallCBeamlineIndentificator {
  BA_POS_DIFF,
  BA_AQ_DIFF,
  BA_TGT_PARAM,
  BA_PLOT_HISTOS,
  BA_FAST_RASTER,
  BA_CORRELATIONS,
  CMB_HISTOS,
  PLOT_XX,
  PLOT_YY,
  PLOT_XY,
};

const char *QwGUIHallCBeamline::RootTrees[NUM_TREE] = 
  {
    "Hel_Tree",
    "Mps_Tree",
    "event_tree"
  };

enum EQwGUIDatabaseHistogramIDs {
  ID_XP,
  ID_XM,
  ID_YP,
  ID_YM,
  ID_EF_Q,
  ID_ABSX,
  ID_ABSY,
  ID_BCM_Q,
  ID_SC,
  ID_DX,
  ID_DY,
  ID_YIELDX,
  ID_YIELDY,
  ID_ASYM_EF_Q,
  ID_BCM_YIELD,
  ID_BCM_ASYM,
  ID_SC_YIELD,
  ID_SC_ASYM,

};

// The list of BPMs in the  Hall C beamline including the virtual target bpm. Please don't change this. 
const char *QwGUIHallCBeamline::HallC_BPMS[HCLINE_BPMS]=
  {

    "bpm3c07","bpm3c07a","bpm3c08","bpm3c11","bpm3c12","bpm3c14","bpm3c16","bpm3c17",
    "bpm3c18","bpm3c19","bpm3p02a","bpm3p02b","bpm3p03a","bpm3c20","bpm3c21","bpm3h02",
    "bpm3h04","bpm3h07a","bpm3h07b","bpm3h07c","bpm3h08","bpm3h09","bpm3h09b","target"
  };

// The list of BCMs in the Hall C line. 
const char *QwGUIHallCBeamline::HallC_BCMS[HCLINE_BCMS]=
  {
    "bcm1","bcm2","bcm5","bcm6"
  };


QwGUIHallCBeamline::QwGUIHallCBeamline(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  correlations        = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dBtnLayout          = NULL;
  dBtnPosDiff         = NULL;
  dBtnAqDiff          = NULL;
  dBtnTgtParam        = NULL;
  dCmbHistos          = NULL;
  dBtnPlotHistos      = NULL;
  dCmbLayout          = NULL;
  dBtnRaster          = NULL;
  PosDiffVar[0]       = NULL;
  PosDiffVar[1]       = NULL;
 
  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

}

QwGUIHallCBeamline::~QwGUIHallCBeamline()
{
  if(dTabFrame)           delete dTabFrame;
  // if(dControlsFrame)      delete dControlsFrame;
  if(dCanvas)             delete dCanvas;  
  if(correlations)        delete correlations;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dBtnPosDiff)         delete dBtnPosDiff;
  if(dBtnAqDiff)          delete dBtnAqDiff;
  if(dBtnTgtParam)        delete dBtnTgtParam;
  // if(dBtnCorrelations)    delete dBtnCorrelations;
  if(dCmbHistos)          delete dCmbHistos;
  if(dBtnPlotHistos)      delete dBtnPlotHistos; 
  // if(dCmbLayout)          delete dCmbLayout;
  if(dBtnRaster)          delete dBtnRaster;

  delete [] PosDiffVar;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIHallCBeamline::MakeLayout()
{

  // SetCleanup(kDeepCleanup);
  
  //Set Styles
  //stats
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(0);     

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
//   gStyle->SetPadTopMargin(0.01);
//   gStyle->SetPadBottomMargin(0.25);
//   gStyle->SetPadLeftMargin(0.1);  

  // histo parameters
  gStyle->SetPalette(1,0);
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.08);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gROOT->ForceStyle();

  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  // correlation options
  correlations = new TGButtonGroup(dControlsFrame, "Position Correlations");
  correlations ->SetTitlePos(TGGroupFrame::kCenter);
   
  dBtnPosDiff       = new TGTextButton(dControlsFrame, "&Position Difference Variation", BA_POS_DIFF);
  dBtnAqDiff        = new TGTextButton(dControlsFrame, "&BCM Double Differences", BA_AQ_DIFF);
  dBtnTgtParam      = new TGTextButton(dControlsFrame, "&Beam Parameters on Target", BA_TGT_PARAM);
  dBtnPlotHistos    = new TGTextButton(dControlsFrame, "&Plot Histograms", BA_PLOT_HISTOS);
  dBtnRaster        = new TGTextButton(dControlsFrame, "&Fast Raster", BA_FAST_RASTER);
  //dBtnCorrelations  = new TGTextButton(dControlsFrame, "&Correlations", BA_CORRELATIONS);
  dCmbHistos        = new TGComboBox(dControlsFrame, CMB_HISTOS);

  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  // Add entries to the drop donw menu.
  //Mps based
  dCmbHistos->AddEntry("XP Wire", ID_XP );
  dCmbHistos->AddEntry("XM Wire", ID_XM );
  dCmbHistos->AddEntry("YP Wire", ID_YP );
  dCmbHistos->AddEntry("YM Wire", ID_YM );
  dCmbHistos->AddEntry("Effective Charge", ID_EF_Q);
  dCmbHistos->AddEntry("AbsX", ID_ABSX);
  dCmbHistos->AddEntry("AbsY", ID_ABSY);
  dCmbHistos->AddEntry("bcm_charge", ID_BCM_Q );
  dCmbHistos->AddEntry("scalers", ID_SC );
  // Helicity based
  dCmbHistos->AddEntry("diff_X", ID_DX);
  dCmbHistos->AddEntry("diff_Y", ID_DY);
  dCmbHistos->AddEntry("yield_X", ID_YIELDX);
  dCmbHistos->AddEntry("yield_Y", ID_YIELDY);
  dCmbHistos->AddEntry("asym_Effective Charge", ID_ASYM_EF_Q);
  dCmbHistos->AddEntry("bcm_yield", ID_BCM_YIELD );
  dCmbHistos->AddEntry("sc_yield", ID_SC_YIELD );
  dCmbHistos->AddEntry("bcm_asyms", ID_BCM_ASYM );
  dCmbHistos->AddEntry("sc_asym", ID_SC_ASYM );
  dCmbHistos->Resize(150,20);


  // Add radio buttons to the correlation group
  new TGRadioButton(correlations, "XX", PLOT_XX);
  new TGRadioButton(correlations, "YY", PLOT_YY);
  new TGRadioButton(correlations, "XY", PLOT_XY);
  dBtnCorrelations  = new TGTextButton(correlations, "&Correlations", BA_CORRELATIONS);
  correlations->SetButton(PLOT_XX);
  //correlations->Connect("Pressed(Int_t)", "ButtonWindow", this, "DoHPosition(Int_t)");
  dControlsFrame->AddFrame(correlations, new TGLayoutHints(kLHintsExpandX));

  // Add buttons to the controls frame
  dControlsFrame -> AddFrame(dBtnPosDiff ,dBtnLayout );
  dControlsFrame -> AddFrame(dBtnAqDiff ,dBtnLayout );
  dControlsFrame -> AddFrame(dBtnTgtParam,dBtnLayout );
  dControlsFrame -> AddFrame(dBtnRaster,dBtnLayout );
  dControlsFrame -> AddFrame(dCmbHistos,dCmbLayout );
  dControlsFrame -> AddFrame(dBtnPlotHistos,dBtnLayout );

  //Buttons
  dBtnPosDiff      -> Associate(this);
  dBtnAqDiff       -> Associate(this);
  dBtnTgtParam     -> Associate(this);
  dBtnPlotHistos   -> Associate(this);  
  dBtnCorrelations -> Associate(this);
  dBtnRaster       -> Associate(this);
  dCmbHistos       -> Associate(this);

  //Canvas settings
  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIHallCBeamline",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);

}

void QwGUIHallCBeamline::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIHallCBeamline::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIHallCBeamline::OnObjClose\n");

    dROOTCont = NULL;
  }
}


void QwGUIHallCBeamline::OnNewDataContainer(RDataContainer *cont)
{

  TObject *obj = NULL;
  TObject *copy = NULL;
  parity_off = kFALSE;

  ClearData();
  
  if(dROOTCont){
    for(Short_t p = 0; p < NUM_TREE; p++) {
      obj = dROOTCont->ReadTree(RootTrees[p]);
      if(obj)
	{
	  copy = obj->Clone();
	  HistArray.Add(copy);
	  if(RootTrees[p] == "event_tree")
	    parity_off = kTRUE;
	}
    }
  }
};

void QwGUIHallCBeamline::OnRemoveThisTab()
{

};

void QwGUIHallCBeamline::ClearData()
{

  //  TObject *obj;
  //   TIter next(HistArray.MakeIterator());
  //   obj = next();
  //   while(obj){    
  //     delete obj;
  //     obj = next();
  //   }
  HistArray.Clear();//Clear();
}

//Stuff to do after the tab is selected
void QwGUIHallCBeamline::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= HCLINE_BPMS)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIHallCBeamline",
						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TH1D*)HistArray[pad-1]));
	SetLogMessage(Form("Looking at %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);

	return;
      }
  }
}

/**************************************************************************
Plot correlations
*/
void QwGUIHallCBeamline::Correlations(TString axis1, TString axis2)
{
  Bool_t ldebug = kFALSE;
  TObject *obj       = NULL;
  TCanvas *mc        = NULL;
  TH2D    *dummyhist = NULL;
  TString histo;

  std::vector<TString> devices;
  
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(6,4);

  // Get Mps tree
  obj = HistArray.At(1);  
  if( !obj ) return; 

  if(ldebug) std::cout<<" In correlations function\n";

  // Check to see which devices are available
  for(Int_t p = 0; p <HCLINE_BPMS-1 ; p++) {
    histo = Form("qwk_%s%s.hw_sum", HallC_BPMS[p],axis1.Data());
    if( ((TTree*) obj)->FindLeaf(histo) ){
      if(ldebug) std::cout<<"Found "<<HallC_BPMS[p]<<std::endl;
      devices.push_back(HallC_BPMS[p]);
    }
  }

  // Plot the correlations
  for(size_t p = 0; p <devices.size()-1 ; p++) {
    mc->cd(p+1);
    obj->Draw(Form("qwk_%s%s.hw_sum:qwk_%s%s.hw_sum>>htemp",
		   devices[p].Data(),axis1.Data(),
		   devices[p+1].Data(), axis2.Data()));
    dummyhist = (TH2D*)gDirectory->Get("htemp");   
    dummyhist->GetXaxis()->SetTitle(Form("%s%s (mm)",devices[p+1].Data(),axis1.Data()));
    dummyhist->GetYaxis()->SetTitle(Form("%s%s (mm)",devices[p].Data(),axis2.Data()));
    dummyhist->SetStats(kTRUE);
    dummyhist->DrawCopy();
    dummyhist->Clear();  
    mc->Update();
  
  }

  return;

}


/**************************************************************************
 Plot the Faster raster
*/
void QwGUIHallCBeamline::FastRaster()
{

  TObject *obj          = NULL;
  TCanvas *mc           = NULL;
  TH2D* fRateMap        = NULL;

  if(fRateMap) delete fRateMap;
  mc = dCanvas->GetCanvas();
  mc->Clear();

  obj = dROOTCont->ReadData("tracking_histo/raster_rate_map");
  if(!obj) {
    std::cout<<"raster rate map not found!\n";
    return;
  }
  else{
    fRateMap = (TH2D*)obj;    
     fRateMap->Draw();
    mc->Update();
    return;
  }
}



/**************************************************************************
 Plot the pattern (e.g. quartet) based aboslute position differences for bpms X an Y.
*/

void QwGUIHallCBeamline::PositionDifferences()
{

  Bool_t ldebug = kTRUE;

  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.06,"y");

  TObject *obj       = NULL;
  TCanvas *mc        = NULL;
  TH1D    *dummyhist = NULL;

  std::vector<Int_t> x_devices;
  std::vector<Int_t> y_devices; 

  std::vector<Double_t> x_mean;  
  std::vector<Double_t> x_err;
  std::vector<Double_t> y_mean;  
  std::vector<Double_t> y_err;


  // check to see if the TH1s used for the calculation are empty.
//   for(Short_t i=0;i<2;i++) {
//     if(PosDiffVar[i]) delete PosDiffVar[i]; PosDiffVar[i] = NULL;
//   }

 
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  // Get HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return; 

  if(ldebug) {
    printf("PositionDiffercences -------------------------------\n");
    printf("Found in tree named %s \n", obj->GetName());
  }

  
  for(Int_t p = 0; p <HCLINE_BPMS ; p++) {
    sprintf (histo, "diff_qwk_%sX_hw", HallC_BPMS[p]);
    obj = dROOTCont->ReadData(Form("hel_histo/diff_qwk_%sX_hw",HallC_BPMS[p]));

    if(obj){
      x_devices.push_back(p);
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
      dummyhist = (TH1D*)obj;
      x_mean.push_back(dummyhist->GetMean()*1e6);
      x_err.push_back(dummyhist->GetMeanError()*1e6);

      SummaryHist(dummyhist);
      delete dummyhist; dummyhist= NULL;
    }
    obj = dROOTCont->ReadData(Form("hel_histo/diff_qwk_%sY_hw",HallC_BPMS[p]));

    sprintf (histo, "diff_qwk_%sY_hw", HallC_BPMS[p]);
    if(obj){   
      y_devices.push_back(p);
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
      dummyhist = (TH1D*)obj;
      y_mean.push_back(dummyhist->GetMean()*1e6);
      y_err.push_back(dummyhist->GetMeanError()*1e6);

      SummaryHist(dummyhist);
      delete dummyhist; dummyhist= NULL;
    }
  }

  PosDiffVar[0] = new TH1D("dxvar", "#Delta X Variation", x_devices.size(), 0.5, x_devices.size()+0.5);
  PosDiffVar[1] = new TH1D("dyvar", "#Delta Y variation", y_devices.size(), 0.5, y_devices.size()+0.5);
  
  for(size_t p = 0; p <x_devices.size() ; p++) {
    PosDiffVar[0] -> SetBinContent(p+1, x_mean[p]); // mm = 1e6nm
    PosDiffVar[0] -> SetBinError  (p+1, x_err[p]);
    PosDiffVar[0] -> GetXaxis()->SetBinLabel(p+1,HallC_BPMS[x_devices[p]]);
    PosDiffVar[0] -> SetStats(0);
  }
  
  for(size_t p = 0; p <y_devices.size() ; p++) {
    PosDiffVar[1] -> SetBinContent(p+1, y_mean[p]); //mm -> nm
    PosDiffVar[1] -> SetBinError  (p+1, y_err[p]);
    PosDiffVar[1] -> GetXaxis()->SetBinLabel(p+1, HallC_BPMS[y_devices[p]]);
    PosDiffVar[1] -> SetStats(0);
  }

    mc->Clear();
    mc->Divide(1,2);

    mc->cd(1);
    SummaryHist(PosDiffVar[0]);
    PosDiffVar[0] -> SetMarkerStyle(20);
    PosDiffVar[0] -> SetMarkerColor(2);
    PosDiffVar[0] -> SetTitle("#Delta X Variation");
    PosDiffVar[0] -> GetYaxis()->CenterTitle();
    PosDiffVar[0] -> GetYaxis() -> SetTitle("#Delta X (nm)");
    PosDiffVar[0] -> Draw("E1");
    gPad->Update();
  
  
    mc->cd(2);
    SummaryHist(PosDiffVar[1]);
    PosDiffVar[1] -> SetMarkerStyle(20);
    PosDiffVar[1] -> SetMarkerColor(4);
    PosDiffVar[1] -> GetYaxis()->CenterTitle();
    PosDiffVar[1] -> SetTitle("#Delta Y Variation");
    PosDiffVar[1] -> GetYaxis()-> SetTitle ("#Delta Y (nm)");
    PosDiffVar[1] -> Draw("E1");
    gPad->Update();
    
    
    if(ldebug) printf("----------------------------------------------------\n");
    mc->Modified();
    mc->Update();
    
    if(dummyhist) delete dummyhist;
    x_devices.clear();
    y_devices.clear();
    x_mean.clear();
    x_err.clear();
    y_mean.clear();
    y_err.clear();

    return;
}



void 
QwGUIHallCBeamline::SummaryHist(TH1 *in)
{

  if((in->GetEntries())!=0){
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
  } 
  else
    {
  return;
    }
};

/***************************************************************************
   Display bcm sigals
*/
void QwGUIHallCBeamline::BCMDoubleDifference()
{
  Bool_t ldebug = kFALSE;

  gStyle->SetStatY(0.999);
  gStyle->SetStatX(0.999);
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.3); 
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetPadTopMargin(0.02);
  gStyle->SetPadBottomMargin(0.2);

  TString plotcommand;
  TString cut;

  TString xlabel;
  TTree *tree = NULL;
  TCanvas *mc = NULL;

  // display error messages
  TText* not_found  = new TText(); 
  not_found->SetTextColor(2);


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,3);

  // Get the HEL tree
  tree = (TTree*)HistArray.At(0);  
  if( !tree ) return;  
  if(ldebug) {
    printf("BCM double differences -----------------------\n");
    printf("Found the tree named %s \n", tree->GetName());
  }
  
  //start with bcm1
  for(Int_t p = 1; p <4 ; p++) {
    mc->cd(p);
    plotcommand = Form("(asym_qwk_%s.hw_sum - asym_qwk_%s.hw_sum)*1e6 >>htemp",
		       HallC_BCMS[0],HallC_BCMS[p]);
    cut = Form("asym_qwk_%s.Device_Error_Code == 0 && asym_qwk_%s.Device_Error_Code == 0 && ErrorFlag==0",
	       HallC_BCMS[0],HallC_BCMS[p]);
    
    tree->Draw(plotcommand,cut);
    
    TH1* h = (TH1*)gPad->GetPrimitive("htemp"); 
    
    if(h){	
      if(ldebug) printf("Drew %s\n", plotcommand.Data());

      h -> SetName(Form("asym %s - asym %s",HallC_BCMS[0],HallC_BCMS[p]));
      h->SetTitle("");
      h -> GetYaxis()->SetTitle("Quartets");
      h -> GetXaxis()->SetTitle("ppm");
      h -> GetXaxis()->SetTitleColor(1);
      h -> GetYaxis()->SetTitleSize(0.08);
      h -> GetXaxis()->SetTitleSize(0.08);
      h -> GetYaxis()->CenterTitle();
      h -> GetXaxis()->CenterTitle();
      h->SetFillColor(kGreen-5);
      h->GetXaxis()->SetTitleOffset(0.9);
      h->GetYaxis()->SetTitleOffset(0.5);
      gPad->SetLogy();
      h->DrawCopy();
      delete h;
    }
    else {
      not_found ->DrawText(0.2,0.5,Form("%s not found!",Form("asym %s - asym %s",HallC_BCMS[0],HallC_BCMS[p]) ));
      not_found->DrawClone();
      not_found->Clear();
    }    
  }



  //start with bcm2
  for(Int_t p = 2; p <4 ; p++) {
    mc->cd(p+2);
    plotcommand = Form("(asym_qwk_%s.hw_sum - asym_qwk_%s.hw_sum)*1e6 >>htemp",
		       HallC_BCMS[1],HallC_BCMS[p]);
    
    cut = Form("asym_qwk_%s.Device_Error_Code == 0 && asym_qwk_%s.Device_Error_Code == 0 && ErrorFlag==0",
	       HallC_BCMS[1],HallC_BCMS[p]);
    
    tree->Draw(plotcommand,cut);
    TH1 *h = (TH1*)gPad->GetPrimitive("htemp"); 
    
    if(h){	
      if(ldebug) printf("Drew %s\n", plotcommand.Data());

      h -> SetName(Form("asym %s - asym %s",HallC_BCMS[1],HallC_BCMS[p]));
      h->SetTitle("");
      h -> GetYaxis()->SetTitle("Quartets");
      h -> GetXaxis()->SetTitle("ppm");
      h -> GetXaxis()->SetTitleColor(1);
      h -> GetYaxis()->SetTitleSize(0.08);
      h -> GetXaxis()->SetTitleSize(0.08);
      h -> GetYaxis()->CenterTitle();
      h -> GetXaxis()->CenterTitle();
      h->SetFillColor(kGreen-5);
      h->GetXaxis()->SetTitleOffset(0.9);
      h->GetYaxis()->SetTitleOffset(0.5);
      gPad->SetLogy();
      h->DrawCopy();
      delete h;
    }
    else {
      not_found ->DrawText(0.2,0.5,Form("%s not found!",Form("asym %s - asym %s",HallC_BCMS[1],HallC_BCMS[p]) ));
      not_found->DrawClone();
      not_found->Clear();
    }    
  }

  //bcm 5 - bcm 6
  mc->cd(6);
  plotcommand = Form("(asym_qwk_%s.hw_sum - asym_qwk_%s.hw_sum)*1e6 >>htemp",
			       HallC_BCMS[2],HallC_BCMS[3]);
  
  cut = Form("asym_qwk_%s.Device_Error_Code == 0 && asym_qwk_%s.Device_Error_Code == 0 && ErrorFlag==0",
		     HallC_BCMS[2],HallC_BCMS[3]);
  
  tree->Draw(plotcommand,cut);
  TH1* h = (TH1*)gPad->GetPrimitive("htemp"); 
  
  if(h){	
      if(ldebug) printf("Drew %s\n", plotcommand.Data());

      h -> SetName(Form("asym %s - asym %s",HallC_BCMS[2],HallC_BCMS[3]));
      h->SetTitle("");
      h -> GetYaxis()->SetTitle("Quartets");
      h -> GetXaxis()->SetTitle("ppm");
      h -> GetXaxis()->SetTitleColor(1);
      h -> GetYaxis()->SetTitleSize(0.08);
      h -> GetXaxis()->SetTitleSize(0.08);
      h -> GetYaxis()->CenterTitle();
      h -> GetXaxis()->CenterTitle();
      h->SetFillColor(kGreen-5);
      h->GetXaxis()->SetTitleOffset(0.9);
      h->GetYaxis()->SetTitleOffset(0.5);
      gPad->SetLogy();
      h->DrawCopy();
      delete h;
    }
    else {
      not_found ->DrawText(0.2,0.5,Form("%s not found!",Form("asym %s - asym %s",HallC_BCMS[2],HallC_BCMS[3]) ));
      not_found->DrawClone();
      not_found->Clear();
    }    
  
  if(ldebug) printf("----------------------------------------------------\n");
  mc->Modified();
  mc->Update();
  
}





/***************************************************************************
   Display the beam postion and angle in X & Y, beam energy and beam charge on the target.
*/
void QwGUIHallCBeamline::DisplayTargetParameters()
{
  Bool_t ldebug = kTRUE;

  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.3); 
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetPadTopMargin(0.02);
  gStyle->SetPadBottomMargin(0.2);

  TH1D * t[6];
  TString xlabel;
  TObject *obj = NULL;
  TCanvas *mc = NULL;

  // display error messages
  TText* not_found  = new TText(); 
  not_found->SetTextColor(2);

  // Beam energy is not calculated by the analyser yet. 
  const char * TgtBeamParameters[6]=
    {
      "targetX","targetY","targetXSlope","targetYSlope","charge","energy"
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,3);

  // Get the HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return;  
  if(ldebug) {
    printf("Target beam parameters-----------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  for(Short_t i=0;i<6;i++) 
    {
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <6 ; p++) {
    mc->cd(p+1);
    gPad->SetLogy();
    sprintf (histo, "yield_qwk_%s.hw_sum", TgtBeamParameters[p]);

    if( ((TTree*) obj)->FindLeaf(histo) ){
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	
      if(strcmp( TgtBeamParameters[p],"average_charge") == 0) 
	sprintf (histo, "yield_qwk_%s.hw_sum", 
		 TgtBeamParameters[p]);
	
      obj -> Draw(histo);
      t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
      t[p] -> SetName(TgtBeamParameters[p]);
      t[p] -> SetTitle("");
      t[p] -> GetYaxis()->SetTitle("Quartets");
      t[p] -> GetYaxis()->SetTitleSize(0.08);
      t[p] -> GetXaxis()->SetTitleSize(0.08);
      t[p] -> GetYaxis()->CenterTitle();
      t[p] -> GetXaxis()->CenterTitle();
      t[p] -> GetXaxis()->SetTitleOffset(0.9);
      t[p] -> GetYaxis()->SetTitleOffset(0.5);
      t[p] -> GetXaxis()->SetTitleColor(1);


      if(strcmp( TgtBeamParameters[p],"charge") == 0) { 
	t[p] -> GetXaxis()->SetTitle("Current (#muA)");
	t[p] -> GetXaxis()->SetDecimals();
	t[p] -> SetFillColor(42);
      }
      else if(strcmp( TgtBeamParameters[p],"targetX") == 0){ 
	t[p] -> GetXaxis()->SetTitle("X Position (mm)");
	t[p] -> GetXaxis()->SetDecimals();
	t[p] -> SetFillColor(34);
      }
      else if(strcmp( TgtBeamParameters[p],"targetY") == 0) {
	t[p] -> GetXaxis()->SetTitle("Y Position (mm)");
	t[p] -> GetXaxis()->SetDecimals();
	t[p] -> SetFillColor(46);
      }
      else if(strcmp( TgtBeamParameters[p],"targetXSlope") == 0){
	t[p] -> GetXaxis()->SetTitle("X Slope");
	t[p] -> GetXaxis()->SetDecimals();
	t[p] -> SetFillColor(34);
      }
      else if(strcmp( TgtBeamParameters[p],"targetYSlope") == 0) {
	t[p] -> GetXaxis()->SetTitle("Y Slope");
	t[p] -> GetXaxis()->SetDecimals();
	t[p] -> SetFillColor(46);
      }
      else if(strcmp( TgtBeamParameters[p],"energy") == 0) {
	t[p] -> GetXaxis()->SetTitle("dP/P");
	t[p] -> GetXaxis()->SetDecimals();
	t[p] -> SetFillColor(kGreen-5);
      }
      SummaryHist(t[p]);	      
      
    }
    else {
      not_found ->DrawText(0.2,0.5,Form("%s not found!", histo));
      not_found->DrawClone();
      not_found->Clear();
    }
    
  }
   
  if(ldebug) printf("----------------------------------------------------\n");
  mc->Modified();
  mc->Update();
  
}




/**
 Plot the selcted option.
*/
void QwGUIHallCBeamline::PlotHistograms()
{ 
  gStyle->SetOptStat(kTRUE);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.4); 
  gStyle->SetLabelSize(0.08,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetTextSize(0.13);
  gStyle->SetPadTopMargin(0.02);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetLabelSize(0.09,"x");

  Bool_t ldebug = kFALSE;

  TCanvas *mc        = NULL;
  TObject *obj       = NULL;	
  TH1 * dummyhist = NULL;

  TString element;
  TString name;

  TText* not_found  = new TText(); 
  not_found->SetTextColor(2);

  mc = dCanvas->GetCanvas();
  mc->Clear();
  gDirectory->Clear();

  //Plot MPS based variables
  /********************************************************************/
  if(select.Contains("MPS")){
   
    obj = HistArray.At(1);
    char histogram[128];  
    element = "";

    // For single wires
    if (select.Contains("XP")||select.Contains("XM")||select.Contains("YP")||select.Contains("YM")){
      mc->Divide(4,6);
      element = select.Remove(0,4); 
      for(Int_t p = 0; p <HCLINE_BPMS ; p++) {
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "qwk_%s%s.hw_sum", HallC_BPMS[p],element.Data());
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp");
	  name = Form("%s %s",HallC_BPMS[p],element.Data()); 
	  dummyhist -> SetName(name);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  dummyhist -> GetXaxis() -> SetTitle("ADC counts");
	  dummyhist -> GetXaxis() ->SetNdivisions(010);
	  dummyhist->SetFillColor(40);
	  dummyhist->Draw();
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!",HallC_BPMS[p] ));
	  not_found ->DrawClone();
	  not_found ->Clear();
	} 
      }
    } // absolute positions
    else if (select.Contains("ABSX")||select.Contains("ABSY")){
      mc->Divide(4,6);
      element = select.Remove(0,7); 
      for(Int_t p = 0; p <HCLINE_BPMS ; p++) {      
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "qwk_%s%s.hw_sum", HallC_BPMS[p],element.Data()); 
	if (ldebug) std::cout << "histogram = " << histogram << std::endl;
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp");
	  name = Form("%s Abs%s",HallC_BPMS[p],element.Data()); 
	  dummyhist -> SetName(name);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  dummyhist -> GetXaxis() -> SetTitle("Position (mm)");
	  dummyhist -> GetXaxis() ->SetNdivisions(010);
	  dummyhist->SetFillColor(41);
	  dummyhist->Draw();
	}  
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BPMS[p] ));
	  not_found ->DrawClone();
	  not_found ->Clear();
	}
      }
    } // effective charge
    else if (select.Contains("EF_Q")){
      mc->Divide(4,6);
      for(Int_t p = 0; p <HCLINE_BPMS ; p++) {
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "qwk_%s_EffectiveCharge.hw_sum", HallC_BPMS[p]);
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp"); 
	  name = Form("%s eff Q",HallC_BPMS[p]); 
	  dummyhist -> SetName(name);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  dummyhist -> GetXaxis() -> SetTitle("ADC counts");
	  dummyhist -> GetXaxis() -> SetNdivisions(010);
	  dummyhist->SetFillColor(42);
	  dummyhist->Draw();
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BPMS[p] ));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    } // bcms
    else if (select.Contains("BCM")){
      mc->Divide(2,2);
      
      for(Int_t p = 0; p <HCLINE_BCMS ; p++) {
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "qwk_%s.hw_sum", HallC_BCMS[p]);
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp"); 
	  name = Form("%s",HallC_BCMS[p]); 
	  dummyhist -> SetName(name);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  dummyhist -> GetXaxis() -> SetTitle("current(#muA)");
	  dummyhist -> GetXaxis() ->SetNdivisions(010);
	  dummyhist->SetFillColor(43);
	  dummyhist->Draw();
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BCMS[p]));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    }
  } // Plot Hel tree based variables
  else if(select.Contains("HEL")){
  /********************************************************************/

    obj = HistArray.At(0);
    char histogram[128];  
    element = "";

    // For position differences
    if (select.Contains("DIFFX")||select.Contains("DIFFY")){
      mc->Divide(4,6);
      element = select.Remove(0,8); 

      for(Int_t p = 0; p <HCLINE_BPMS ; p++) {
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "diff_qwk_%s%s.hw_sum", HallC_BPMS[p],element.Data());
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp"); 
	  name = Form("%s diff%s",HallC_BPMS[p],element.Data()); 
	  dummyhist -> SetName(name);
	  dummyhist->SetFillColor(44);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  dummyhist -> GetXaxis() -> SetTitle("Position Diff (mm)");
	  dummyhist -> GetXaxis() ->SetNdivisions(010);
	  dummyhist->Draw();
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BPMS[p]));
	  not_found ->DrawClone();
	  not_found ->Clear();
	}
      }
    } // position yields
    else if (select.Contains("YIELDX")||select.Contains("YIELDY")){
      mc->Divide(4,6);
      element = select.Remove(0,9); 
      for(Int_t p = 0; p <HCLINE_BPMS ; p++) {
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "yield_qwk_%s%s.hw_sum", HallC_BPMS[p],element.Data());
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp");
	  name = Form("%s Yield%s",HallC_BPMS[p],element.Data());  
	  dummyhist -> SetName(name);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  // dummyhist -> GetYaxis() -> SetTitle("Quartets");
	  dummyhist -> GetXaxis() -> SetTitle("Position (mm)");
	  dummyhist -> GetXaxis() ->SetNdivisions(010);
	  dummyhist->SetFillColor(45);
	  dummyhist->Draw();
	}
	else  {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BPMS[p]));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    } // effective charge asymmetry
    else if (select.Contains("AEF_Q")){
      mc->Divide(4,6);
      for(Int_t p = 0; p <HCLINE_BPMS ; p++) {
	mc->cd(p+1);
	gPad->SetLogy();
	sprintf (histogram, "asym_qwk_%s_EffectiveCharge.hw_sum", HallC_BPMS[p]);
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1*)gPad->GetPrimitive("htemp"); 
	  name = Form("%s eff Q",HallC_BPMS[p]); 
	  dummyhist -> SetName(name);
	  dummyhist -> SetTitle("");
	  dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	  dummyhist -> GetXaxis() -> SetTitleColor(1);
	  dummyhist -> GetXaxis() -> CenterTitle();
	  //dummyhist -> GetYaxis() -> SetTitle("Quartets");
	  dummyhist -> GetXaxis() -> SetTitle("Charge");
	  dummyhist -> GetXaxis() ->SetNdivisions(010);
	  dummyhist->SetFillColor(46);
	  dummyhist->Draw();
	}
	else  {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BPMS[p]));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    }
    if (select.Contains("BCM")){
      mc->Divide(2,2);
      gPad->SetLogy();
	for(Int_t p = 0; p <HCLINE_BCMS ; p++) {
	  mc->cd(p+1);
	  gPad->SetLogy();
	  if(select.Contains("YBCM")){
	    sprintf (histogram, "yield_qwk_%s.hw_sum", HallC_BCMS[p]);
	    name = Form("yield %s",HallC_BCMS[p]); 
	  }
	  else if(select.Contains("ABCM")){
	    sprintf (histogram, "asym_qwk_%s.hw_sum", HallC_BCMS[p]); 
	    name = Form("asym %s",HallC_BCMS[p]); 
	  }
	  if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	  if( ((TTree*) obj)->FindLeaf(histogram) ) {
	    obj -> Draw(histogram);	 
	    dummyhist = (TH1*)gPad->GetPrimitive("htemp");
	    dummyhist -> SetName(name);
	    dummyhist -> SetTitle("");
	    dummyhist -> GetXaxis() -> SetTitleSize(0.1);
	    dummyhist -> GetXaxis() -> SetTitleColor(1);
	    dummyhist -> GetXaxis() -> CenterTitle();
	    dummyhist -> GetXaxis() ->SetNdivisions(010);
	    if(select.Contains("YBCM"))
	      dummyhist -> GetXaxis() -> SetTitle("Current (#muA)");
	    if(select.Contains("ABCM"))
	      dummyhist -> GetXaxis() -> SetTitle("asymmetry (ppm)");
	    dummyhist->SetFillColor(47);
	    dummyhist->Draw();
	  }
	  else {
	    not_found ->DrawText(0.2,0.5,Form("%s not found!", HallC_BCMS[p]));
	    not_found->DrawClone();
	    not_found->Clear();
	  }
	}
    }
  }
  
  select="";

  mc->Modified();
  mc->Update();
  return;
}




// Process events generated by the object in the frame.
Bool_t QwGUIHallCBeamline::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

  switch (GET_MSG(msg))
    {
    // case kC_TEXTENTRY:
    //   switch (GET_SUBMSG(msg)) 
    // 	{
    // 	case kTE_ENTER:
    // 	  switch (parm1) 
    // 	    {
    // 	    default:
    // 	      break;
    // 	    }
    // 	default:
    // 	  break;
    // 	}
      
    case kC_COMMAND:
      if(dROOTCont){
	switch (GET_SUBMSG(msg))
	  {
	  case kCM_BUTTON:
	    {
	      switch(parm1)
		{
		case BA_POS_DIFF:
		  PositionDifferences();
		  break;

		case BA_AQ_DIFF:
		  BCMDoubleDifference();
		  break;
		  
		case BA_TGT_PARAM:
		  DisplayTargetParameters();
		  break;
		  
		case BA_PLOT_HISTOS:
		  PlotHistograms();
		  break
		    ;
		case BA_FAST_RASTER:
		  FastRaster();
		  break;
		case BA_CORRELATIONS:
		  Correlations("X","X");
		  break;	     
		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (dCmbHistos ->GetSelected()) {
	      case ID_XP:
		select="MPS_XP";
		break;
	      case ID_XM:
		select="MPS_XM";
		break;
	      case ID_YP:
		select="MPS_YP";
		break;
	      case ID_YM:
		select="MPS_YM";
		break;
	      case ID_EF_Q:
		select="MPS_EF_Q";
		break;
	      case ID_SC:
		select="MPS_SC";
		break;
	      case ID_ABSX:
		select="MPS_ABSX";
		break;	      
	      case ID_ABSY:
		select="MPS_ABSY";
		break;
	      case ID_BCM_Q:
		select="MPS_BCM_Q";
		break;
	      case ID_DX:
		select="HEL_DIFFX";
		break;
	      case ID_DY:
		select="HEL_DIFFY";
		break;
	      case ID_YIELDX:
		select="HEL_YIELDX";
		break;
	      case ID_YIELDY:
		select="HEL_YIELDY";
		break;
	      case ID_ASYM_EF_Q:
		select="HEL_AEF_Q";
		break;
	      case ID_BCM_YIELD:
		select="HEL_YBCM";
		break;
	      case ID_BCM_ASYM:
		select="HEL_ABCM";
		break;
	      case ID_SC_YIELD:
		select="HEL_SC_YIELD";
		break;
	      case ID_SC_ASYM:
		select="HEL_SC_ASYM";
		break;
	      default:
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
	std::cout<<"Please open a root file to view data. \n";
      }
      default:
	break;
    }
  
  return kTRUE;
}



