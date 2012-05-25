#include "QwGUIBeamModulation.h"

#include <TG3DLine.h>
#include "TGaxis.h"
#include "TStyle.h"

ClassImp(QwGUIBeamModulation);

enum QwGUIBeamModulationIndentificator {
  BA_BPM_RESP,

};

const char *QwGUIBeamModulation::RootTrees[TRE_NUM] = 
  {
    "Hel_Tree",
    "Mps_Tree"
  };

const char *QwGUIBeamModulation::BeamModulationDevices[BEAMMODULATION_DEV_NUM]=
  {
    "qwk_fgx1","qwk_fgx2","qwk_fgy1","qwk_fgy2","qwk_fge","qwk_fgphase",
    "qwk_lemx1","qwk_lemx2","qwk_lemy1","qwk_lemy2",
    "qwk_trimx1","qwk_trimx2","qwk_trimy1","qwk_trimy2",
    "qwk_3c07","qwk_3c08","qwk_3c11","qwk_3c12","qwk_3c14","qwk_3c16"
  };

QwGUIBeamModulation::QwGUIBeamModulation(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame               = NULL;
  dControlsFrame          = NULL;
  dCanvas                 = NULL;  
  dTabLayout              = NULL;
  dCnvLayout              = NULL;
  dSubLayout              = NULL;
  dBtnLayout              = NULL;
  dBtnBPMResp             = NULL;

  PosDiffVar[0] = NULL;
  PosDiffVar[1] = NULL;
 
  TreeArray.Clear();
  PlotArray.Clear();
  DataWindowArray.Clear();

  RemoveSelectedDataWindow();

  AddThisTab(this);

}

void QwGUIBeamModulation::CleanUp()
{
  // delete [] PosDiffVar;

  TObject* obj;
  vector <TObject*> obja;
  TIter *next = new TIter(DataWindowArray.MakeIterator());
  obj = next->Next();
  while(obj){
    obja.push_back(obj);
    //     delete obj;
    obj = next->Next();
  }
  delete next;
  
  for(uint l = 0; l < obja.size(); l++)
    delete obja[l];
  
  DataWindowArray.Clear();

  //don't delete the trees (done by the main class when necessary) 
  TreeArray.Clear();

  //We do, however, need to clean up the histograms ...
  //Need to check below that we set SetDirectory(0) for all of them ...
  next = new TIter(PlotArray.MakeIterator());
  obj = next->Next();
  while(obj){
    delete obj;
    obj = next->Next();
  }
  delete next;
  PlotArray.Clear();
}

QwGUIBeamModulation::~QwGUIBeamModulation()
{
  // if(dTabFrame)               delete dTabFrame;
  // if(dControlsFrame)          delete dControlsFrame;
  if(dCanvas)                 delete dCanvas;  
  if(dTabLayout)              delete dTabLayout;
  if(dCnvLayout)              delete dCnvLayout;
  if(dSubLayout)              delete dSubLayout;
  if(dBtnLayout)              delete dBtnLayout;
  if(dBtnBPMResp)             delete dBtnBPMResp;

  CleanUp();

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIBeamModulation::MakeLayout()
{

  SetCleanup(kDeepCleanup);
  
  //Set Styles
  //stats
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(0);  
  gStyle->SetStatH(0.1);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadTopMargin(0.15);
  gStyle->SetPadBottomMargin(0.20);
  gStyle->SetPadLeftMargin(0.12);  
  gStyle->SetPadRightMargin(0.03);  

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.08);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleBorderSize(2);
  gStyle->SetTitleFillColor(10);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");


  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  dBtnBPMResp  = new TGTextButton(dControlsFrame, "&BPM Response Diagnostic", BA_BPM_RESP);

  dControlsFrame->AddFrame(dBtnBPMResp ,dBtnLayout );

  dBtnBPMResp -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);

  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIBeamModulation",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");
}

void QwGUIBeamModulation::OnReceiveMessage(char *msg)
{
  TObject *obj = NULL;
  Int_t ind = 0;
  TString message = msg;

  if(message.Contains("dDataWindow")){

    if(message.Contains("Add to")){

      message.ReplaceAll("Add to dDataWindow_",7,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
	ind = DataWindowArray.IndexOf(obj);
	SetSelectedDataWindow(ind);
      }
    }
    else if(message.Contains("Don't add to")){

      message.ReplaceAll("Don't add to dDataWindow_",13,"",0);
      obj = DataWindowArray.FindObject(message);
      if(obj){
	RemoveSelectedDataWindow();
      }
    }
  }

}

void QwGUIBeamModulation::OnObjClose(char *obj)
{
  if(!obj) return;
  TString name = obj;

  if(name.Contains("dDataWindow")){
    QwGUIDataWindow* window = (QwGUIDataWindow*)DataWindowArray.Remove(DataWindowArray.FindObject(obj));
    if(window){
      if(window == GetSelectedDataWindow()) { RemoveSelectedDataWindow();}
    }
  }

  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIBeamModulation::OnObjClose\n");
    ClearData();
    dROOTCont = NULL;
  }

  QwGUISubSystem::OnObjClose(obj);
}

QwGUIDataWindow *QwGUIBeamModulation::GetSelectedDataWindow()
{
  if(dSelectedDataWindow < 0 || dSelectedDataWindow > DataWindowArray.GetLast()) return NULL;

  return (QwGUIDataWindow*)DataWindowArray[dSelectedDataWindow];
}


void QwGUIBeamModulation::OnNewDataContainer(RDataContainer *cont)
{

  TObject *obj = NULL;
  TObject *copy = NULL;
 
  ClearData();

  if(dROOTCont){
    for(Short_t p = 0; p < TRE_NUM; p++) {
      obj = dROOTCont->ReadTree(RootTrees[p]);
      if(obj)
	{
	  copy = obj->Clone();
	  TreeArray.Add(copy);
	}
    }
  }
};

void QwGUIBeamModulation::OnRemoveThisTab()
{

};

void QwGUIBeamModulation::ClearData()
{
  TObject *obj = NULL;

  //don't delete the trees (done by the main class when necessary) 
  TreeArray.Clear();

  //We do, however, need to clean up the histograms ...
  //Need to check below that we set SetDirectory(0) for all of them ...
  TIter *next = new TIter(PlotArray.MakeIterator());
  obj = next->Next();
  while(obj){
    delete obj;
    obj = next->Next();
  }
  delete next;
  PlotArray.Clear();

  if(dCanvas) {
    TCanvas *c = dCanvas->GetCanvas();
    if(c){
      c->Clear();
      c->Modified();
      c->Update();
    }
  }
}

// Stuff to do after the tab is selected

void QwGUIBeamModulation::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  QwGUIDataWindow *dDataWindow = GetSelectedDataWindow();
  Bool_t add = kFalse;
  TObject *plot = NULL;
  
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();

    printf("%s,%d pad = %d\n",__FILE__,__LINE__,pad);

    // if(pad > 0 && pad <= BEAMMODULATION_DEV_NUM)
    if(pad > 0 && pad <= PlotArray.GetLast()+1) {
      plot = PlotArray[pad-1];
      
      if(plot){

	if(plot->InheritsFrom("TProfile")){
	
	  if(!dDataWindow){
	    dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					      "QwGUIBeamModulation",((TProfile*)plot)->GetTitle(), PT_PROFILE,
					      DDT_BPM,600,400);
	    if(!dDataWindow){
	      return;
	    }
	    DataWindowArray.Add(dDataWindow);
	  }
	  else
	    add = kTrue;
	  
	  // DataWindowArray.Add(dDataWindow);
	  dDataWindow->SetPlotTitle((char*)((TProfile*)plot)->GetTitle());
	  dDataWindow->DrawData(*((TProfile*)plot));
	  SetLogMessage(Form("Looking at beam modulation profile %s\n",(char*)((TProfile*)plot)->GetTitle()),add);
	  
	  Connect(dDataWindow,"IsClosing(char*)","QwGUIBeamModulation",(void*)this,"OnObjClose(char*)");
	  Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIBeamModulation",(void*)this,"OnReceiveMessage(char*)");
	  //Connect(dDataWindow,"UpdatePlot(char*)","QwGUIBeamModulation",(void*)this,"OnUpdatePlot(char *)");
	  dDataWindow->SetRunNumber(GetRunNumber());
	  return;
	}
	
	if(plot->InheritsFrom("TH1")){
	  
	  if(!dDataWindow){
	    dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					      "QwGUIBeamModulation",((TH1D*)plot)->GetTitle(), PT_HISTO_1D,
					      DDT_BPM,600,400);
	    if(!dDataWindow){
	      return;
	    }
	    DataWindowArray.Add(dDataWindow);
	  }
	  else
	    add = kTrue;
	  
	  dDataWindow->SetStaticData(plot,DataWindowArray.GetLast());
	  dDataWindow->SetPlotTitle((char*)((TH1D*)plot)->GetTitle());
	  dDataWindow->DrawData(*((TH1D*)plot),add);
	  SetLogMessage(Form("Looking at beam modulation histogram %s\n",(char*)((TH1D*)plot)->GetTitle()),add);
	  
	  Connect(dDataWindow,"IsClosing(char*)","QwGUIBeamModulation",(void*)this,"OnObjClose(char*)");
	  Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIBeamModulation",(void*)this,"OnReceiveMessage(char*)");
	  //Connect(dDataWindow,"UpdatePlot(char*)","QwGUIBeamModulation",(void*)this,"OnUpdatePlot(char *)");
	  dDataWindow->SetRunNumber(GetRunNumber());
	  return;
	}
	
	
	if(plot->InheritsFrom("TGraphErrors")){
	  
	  if(!dDataWindow){
	    dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					      "QwGUIBeamModulation",((TGraphErrors*)plot)->GetTitle(), PT_GRAPH_ER,
					      DDT_BPM,600,400);
	    if(!dDataWindow){
	      return;
	    }
	    DataWindowArray.Add(dDataWindow);
	  }
	  else
	    add = kTrue;
	  
	  dDataWindow->SetPlotTitle((char*)((TGraphErrors*)plot)->GetTitle());
	  dDataWindow->DrawData(*((TGraphErrors*)plot),add);
	  SetLogMessage(Form("Looking at beam modulation graph %s\n",(char*)((TGraphErrors*)plot)->GetTitle()),add);
	  
	  Connect(dDataWindow,"IsClosing(char*)","QwGUIBeamModulation",(void*)this,"OnObjClose(char*)");
	  Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIBeamModulation",(void*)this,"OnReceiveMessage(char*)");
	  //Connect(dDataWindow,"UpdatePlot(char*)","QwGUIBeamModulation",(void*)this,"OnUpdatePlot(char *)");
	  dDataWindow->SetRunNumber(GetRunNumber());
	  return;
	}
	
	if(plot->InheritsFrom("TGraph")){
	  
	  if(!dDataWindow){
	    dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					      "QwGUIBeamModulation",((TGraph*)plot)->GetTitle(), PT_GRAPH,
					      DDT_BPM,600,400);
	    if(!dDataWindow){
	      return;
	    }
	    DataWindowArray.Add(dDataWindow);
	  }
	  else
	    add = kTrue;
	  
	  dDataWindow->SetPlotTitle((char*)((TGraph*)plot)->GetTitle());
	  dDataWindow->DrawData(*((TGraph*)plot),add);
	  SetLogMessage(Form("Looking at beam modulation graph %s\n",(char*)((TGraph*)plot)->GetTitle()),add);
	  
	  Connect(dDataWindow,"IsClosing(char*)","QwGUIBeamModulation",(void*)this,"OnObjClose(char*)");
	  Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIBeamModulation",(void*)this,"OnReceiveMessage(char*)");
	  //Connect(dDataWindow,"UpdatePlot(char*)","QwGUIBeamModulation",(void*)this,"OnUpdatePlot(char *)");
	  dDataWindow->SetRunNumber(GetRunNumber());
	  return;
	}
      }
    }
  }
}

void QwGUIBeamModulation::PlotBPMResponse(void)
{

  TTree *tree = NULL;

  TCanvas *canvas = NULL;

  TString  cut;
  TString  error_cut = "ErrorFlag == 0x4018080";
  TString  ramp_cut = "(( ramp.block3 + ramp.block1) - (ramp.block2 - ramp.block0)) > 5e2";

  TProfile *profx = new TProfile("profx", "profx", 100, 0., 360., -4., 4.);
  TProfile *profy = new TProfile("profy", "profy", 100, 0., 360., -4., 4.);
  TProfile *profxp = new TProfile("profxp", "profxp", 100, 0., 360., -4., 4.);
  TProfile *profyp = new TProfile("profyp", "profyp", 100, 0., 360., -4., 4.);
  TProfile *profe = new TProfile("profe", "profe", 100, 0., 360., -4., 4.);

  TH2F *histr = new TH2F("histr", "histr", 10e3, 0, 1000, 100, -0.01, 0.1);

  Double_t offset;
  Double_t amplitude;
  Double_t phase;

  TPaveText *no_datax = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");
  TPaveText *no_datay = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");
  TPaveText *no_dataxp = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");
  TPaveText *no_datayp = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");
  TPaveText *no_datae = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");
  TPaveText *no_datar = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");

  TF1 *sine = new TF1("sine", "[0] + [1]*sin(x*0.017951 + [2])", 0, 360.);

  canvas = dCanvas->GetCanvas();
  canvas->Clear();
  canvas->Divide(2,3);

  // Grab the Mps_Tree from the data container: Hel_Tree(0), Hel_Tree(1)
  tree = (TTree *)TreeArray.At(1);
  if(!tree) return;

  cut = error_cut + " && " + ramp_cut;

  std::cout << "Executing Beam Modulation Diagnostic Plots." << std::endl;

  gStyle->SetOptFit(01011);
  gStyle->SetOptStat("ne");
  gStyle->SetStatW(0.10);     
  gStyle->SetStatH(0.0025);     
  gStyle->SetStatX(0.9);     
  gStyle->SetStatY(0.9);     
  gStyle->SetStatFontSize(0.03);
 
  canvas->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();

  tree->Draw("qwk_targetX:0.09*ramp>>profx", cut + " && bm_pattern_number == 11 || bm_pattern_number == 0");
  profx = (TProfile *)gDirectory->Get("profx");
  if(profx->GetEntries() == 0){
    profx->Delete();
    no_datax->Clear();
    no_datax->AddText("No modulation X data found.");
    no_datax->Draw();
    PlotArray.Add(NULL);
  }
  else{
    profx->GetXaxis()->SetTitle("phase (deg)");
    profx->GetYaxis()->SetTitle("BPM Amplitude (mm)");
    sine->SetLineColor(2);
    sine->SetRange(10., 350.);
    profx->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(2);
    profx->SetTitle(Form("Target BPM Response to X Modulation: |Amplitude| = %f", TMath::Abs(amplitude) ));
    profx->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profx->Draw();
    profx->SetDirectory(0);
    PlotArray.Add(profx);
    canvas->Modified();
    canvas->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250);
  }

  canvas->cd(2);
  gPad->SetGridx();
  gPad->SetGridy();

  tree->Draw("qwk_targetXSlope:0.09*ramp>>profxp", cut + " && bm_pattern_number == 14  || bm_pattern_number == 3");
  profxp = (TProfile *)gDirectory->Get("profxp");
  if(profxp->GetEntries() == 0){
    profxp->Delete();
    no_dataxp->Clear();
    no_dataxp->AddText("No modulation XP data found.");
    no_dataxp->Draw();
    PlotArray.Add(NULL);
  }
  else{
    profxp->GetXaxis()->SetTitle("phase (deg)");
    profxp->GetYaxis()->SetTitle("BPM Amplitude (rad)");
    sine->SetLineColor(2);
    sine->SetRange(10., 350.);
    sine->SetParameter(1, 4.e-6);
    profxp->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(2);
    profxp->SetTitle(Form("Target BPM Response to XP Modulation: |Amplitude| = %e", TMath::Abs(amplitude) ));
    profxp->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profxp->Draw();
    profxp->SetDirectory(0);
    PlotArray.Add(profxp);
    canvas->Modified();
    canvas->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250); 
  }

  canvas->cd(3);
  tree->Draw("qwk_targetY:0.09*ramp>>profy", cut + " && bm_pattern_number == 12 || bm_pattern_number == 1");
  profy = (TProfile *)gDirectory->Get("profy");
  if(profy->GetEntries() == 0){
    profy->Delete();
    no_datay->Clear();
    no_datay->AddText("No modulation Y data found.");
    no_datay->Draw();
    PlotArray.Add(NULL);
  }
  else{
    gPad->SetGridx();
    gPad->SetGridy();
  
    profy->GetXaxis()->SetTitle("phase (deg)");
    profy->GetYaxis()->SetTitle("BPM Amplitude (mm)");
    sine->SetLineColor(3);
    sine->SetRange(10., 350.);
    profy->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(3);
    profy->SetTitle(Form("Target BPM Response to Y Modulation: |Amplitude| = %f", TMath::Abs(amplitude) ));
    profy->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profy->Draw();
    profy->SetDirectory(0);
    PlotArray.Add(profy);
    canvas->Modified();
    canvas->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250); 
  }

  canvas->cd(4);
  gPad->SetGridx();
  gPad->SetGridy();

  tree->Draw("qwk_targetYSlope:0.09*ramp>>profyp", cut + " && bm_pattern_number == 15 || bm_pattern_number == 4");
  profyp = (TProfile *)gDirectory->Get("profyp");
  if(profyp->GetEntries() == 0){
    profyp->Delete();
    no_datayp->Clear();
    no_datayp->AddText("No modulation YP data found.");
    no_datayp->Draw();
    PlotArray.Add(NULL);
  }
  else{
    profyp->GetXaxis()->SetTitle("phase (deg)");
    profyp->GetYaxis()->SetTitle("BPM Amplitude (rad)");
    sine->SetLineColor(4);
    sine->SetRange(10., 350.);
    sine->SetParameter(1, 4.e-6);
    profyp->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(2);
    profyp->SetTitle(Form("Target BPM Response to YP Modulation: |Amplitude| = %e", TMath::Abs(amplitude) ));
    profyp->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profyp->Draw();
    profyp->SetDirectory(0);
    PlotArray.Add(profyp);
    canvas->Modified();
    canvas->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250); 
  }

  canvas->cd(5);
  gPad->SetGridx();
  gPad->SetGridy();

  tree->Draw("qwk_bpm3c12X:0.09*ramp>>profe", cut + " && bm_pattern_number == 13 || bm_pattern_number == 2");
  profyp = (TProfile *)gDirectory->Get("profe");
  if(profe->GetEntries() == 0){
    profe->Delete();
    no_datae->Clear();
    no_datae->AddText("No modulation E data found.");
    no_datae->Draw();
    PlotArray.Add(NULL);
  }
  else{
    profe->GetXaxis()->SetTitle("phase (deg)");
    profe->GetYaxis()->SetTitle("BPM Amplitude (rad)");
    sine->SetLineColor(5);
    sine->SetRange(10., 350.);
    sine->SetParameter(1, 4.e-6);
    profe->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(2);
    profe->SetTitle(Form("Target BPM Response to E Modulation: |Amplitude| = %e", TMath::Abs(amplitude) ));
    profe->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profe->Draw();
    profe->SetDirectory(0);
    PlotArray.Add(profe);
    canvas->Modified();
    canvas->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250); 
  }

  canvas->cd(6);
  gPad->SetGridx();
  gPad->SetGridy();

  tree->Draw("0.3*76.29e-6*ramp:event_number*0.001>>histr", "");
  histr = (TH2F *)gDirectory->Get("histr");
  if(histr->GetEntries() == 0){
    histr->Delete();
    no_datar->Clear();
    no_datar->SetTextColor(2);
    no_datar->AddText("No readback of the ramp signal in the ADC.");
    no_datar->Draw();
    PlotArray.Add(NULL);
  }
  else{
    histr->GetXaxis()->SetTitle("event number (msec)");
    histr->GetYaxis()->SetTitle("Ramp Amplitude (I)");
    histr->SetTitle("ADC readback of ramp signal");
    histr->Draw();
    histr->SetDirectory(0);
    PlotArray.Add(histr);
    canvas->Modified();
    canvas->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250); 
  }

  canvas->Modified();
  canvas->Update();

}

void QwGUIBeamModulation::MakeHCLogEntry()
{

  if(dCanvas)
    SubmitToHCLog(dCanvas->GetCanvas());

}


Bool_t QwGUIBeamModulation::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

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
		case   BA_BPM_RESP:
		  PlotBPMResponse();
		  break;
		  
		// case BA_TGT_PARAM:

		}

	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
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



