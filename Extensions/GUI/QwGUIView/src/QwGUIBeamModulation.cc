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
 
  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

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

  delete [] PosDiffVar;

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
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIBeamModulation",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();

  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);

}

void QwGUIBeamModulation::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIBeamModulation::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIBeamModulation::OnObjClose\n");

    dROOTCont = NULL;
  }
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
	  HistArray.Add(copy);
	}
    }
  }
};

void QwGUIBeamModulation::OnRemoveThisTab()
{

};

void QwGUIBeamModulation::ClearData()
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

// Stuff to do after the tab is selected

void QwGUIBeamModulation::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= BEAMMODULATION_DEV_NUM)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIBeamModulation",
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

void QwGUIBeamModulation::PlotBPMResponse(void)
{

  TTree *tree = NULL;

  TCanvas *canvas = NULL;

  TString  cut;
  TString  error_cut = "ErrorFlag == 0x4018080";
  TString  ramp_cut = "(( ramp.block3 + ramp.block1) - (ramp.block2 - ramp.block0)) > 5e2";

  TProfile *profx = new TProfile("profx", "profx", 100, 0., 360., -4., 4.);
  TProfile *profy = new TProfile("profy", "profy", 100, 0., 360., -4., 4.);

  Double_t offset;
  Double_t amplitude;
  Double_t phase;

  TPaveText *no_data = new TPaveText(.15,.15,.8,.8, "TL NDC ARC");

  TF1 *sine = new TF1("sine", "[0] + [1]*sin(x*0.017951 + [2])", 0, 360.);

  canvas = dCanvas->GetCanvas();
  canvas->Clear();
  canvas->Divide(1,3);

  // Grab the Mps_Tree from the data container: Hel_Tree(0), Hel_Tree(1)
  tree = (TTree *)HistArray.At(1);
  if(!tree) return;

  cut = error_cut + " && " + ramp_cut;

  std::cout << "Executing Beam Modulation Diagnostic Plots." << std::endl;

  gStyle->SetOptFit(01011);
  gStyle->SetOptStat("ne");
  gStyle->SetStatW(0.04);     
  gStyle->SetStatH(0.005);     
  gStyle->SetStatX(0.9);     
  gStyle->SetStatY(0.9);     
  gStyle->SetStatFontSize(0.03);
 
  canvas->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();

  tree->Draw("qwk_targetX:0.09*ramp>>profx", cut + " && bm_pattern_number == 11");
  profx = (TProfile *)gDirectory->Get("profx");
  if(profx->GetEntries() == 0){
    profx->Delete();
    no_data->Clear();
    no_data->AddText("No modulation X data found.");
    no_data->Draw();
  }
  else{
    profx->GetXaxis()->SetTitle("phase (deg)");
    profx->GetYaxis()->SetTitle("BPM Amplitude (mm)");
    sine->SetLineColor(2);
    sine->SetRange(10., 350.);
    sine->SetParLimits(2, 0, 100.);
    profx->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(2);
    profx->SetTitle(Form("Target BPM Response to X Modulation: %f + %f*Sin( a*x + %f)", offset, amplitude, phase));
    profx->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profx->Draw();
  }

  canvas->cd(2);
  tree->Draw("qwk_targetY:0.09*ramp>>profy", cut + " && bm_pattern_number == 12");
  profy = (TProfile *)gDirectory->Get("profy");
  if(profy->GetEntries() == 0){
    profy->Delete();
    no_data->Clear();
    no_data->AddText("No modulation Y data found.");
    no_data->Draw();
  }
  else{
    gPad->SetGridx();
    gPad->SetGridy();
  
    profy->GetXaxis()->SetTitle("phase (deg)");
    profy->GetYaxis()->SetTitle("BPM Amplitude (mm)");
    sine->SetLineColor(3);
    sine->SetRange(10., 350.);
    sine->SetParLimits(2, 0, 100.);
    profy->Fit("sine","BR");
    offset = sine->GetParameter(0);
    amplitude = sine->GetParameter(1);
    phase = sine->GetParameter(2);
    profy->SetTitle(Form("Target BPM Response to Y Modulation: %f + %f*Sin( a*x + %f)", offset, amplitude, phase));
    profy->SetAxisRange(offset - TMath::Abs(amplitude*2.), offset + TMath::Abs(amplitude*2.), "Y");
    profy->Draw();
  }

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



