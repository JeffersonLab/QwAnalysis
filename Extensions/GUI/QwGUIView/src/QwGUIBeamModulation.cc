#include "QwGUIBeamModulation.h"

#include <TG3DLine.h>
#include "TGaxis.h"

ClassImp(QwGUIBeamModulation);

enum QwGUIBeamModulationIndentificator {
  BA_POS_DIFF,
  BA_TGT_PARAM,
  BA_TGT_BPM,
  BA_DET_SENSITIVITY,
  BA_LUMI_SENSITIVITY,
  BA_FG_CHANNEL,
  BA_LEM_CHANNEL,
  BA_TRIM_CHANNEL
};

const char *QwGUIBeamModulation::RootTrees[TRE_NUM] = 
  {
    "HEL_Tree",
    "MPS_Tree"
  };

// const char *QwGUIBeamModulation::BeamModulationTrees[BEAMMODULATION_TRE_NUM] = 
//   {
//     "HEL_Tree",
//     "MPS_Tree"
//   };

const char *QwGUIBeamModulation::BeamModulationDevices[BEAMMODULATION_DEV_NUM]=
  {
    "qwk_fgx1","qwk_fgx2","qwk_fgy1","qwk_fgy2","qwk_fge","qwk_fgphase",
    "qwk_lemx1","qwk_lemx2","qwk_lemy1","qwk_lemy2",
    "qwk_trimx1","qwk_trimx2","qwk_trimy1","qwk_trimy2",
    "qwk_3c07","qwk_3c08","qwk_3c11","qwk_3c12","qwk_3c14","qwk_3c16"
  };


// const char *QwGUIBeamModulation::BeamModulationDevices2[BEAMMODULATION_DEV_NUM]=
//   {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a"
  
//     };


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
  dBtnPosDiff             = NULL;
  dBtnTgtParam            = NULL;
  dBtnTgtBPM              = NULL;
  dBtnDetectorSensitivity = NULL;
  dBtnLumiSensitivity     = NULL;
  dBtnFgChannel           = NULL;
  dBtnLEMChannel          = NULL;
  dBtnTRIMChannel         = NULL;

  PosDiffVar[0] = NULL;
  PosDiffVar[1] = NULL;
 
  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

}

QwGUIBeamModulation::~QwGUIBeamModulation()
{
  if(dTabFrame)               delete dTabFrame;
  if(dControlsFrame)          delete dControlsFrame;
  if(dCanvas)                 delete dCanvas;  
  if(dTabLayout)              delete dTabLayout;
  if(dCnvLayout)              delete dCnvLayout;
  if(dSubLayout)              delete dSubLayout;
  if(dBtnLayout)              delete dBtnLayout;
  if(dBtnPosDiff)             delete dBtnPosDiff;
  if(dBtnTgtParam)            delete dBtnTgtParam;
  if(dBtnTgtBPM)              delete dBtnTgtBPM;
  if(dBtnDetectorSensitivity) delete dBtnDetectorSensitivity;
  if(dBtnLumiSensitivity)     delete dBtnLumiSensitivity;
  if(dBtnFgChannel)           delete dBtnFgChannel;
  if(dBtnLEMChannel)          delete dBtnLEMChannel;
  if(dBtnTRIMChannel)         delete dBtnTRIMChannel;

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
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.2);     
 
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

  dBtnPosDiff  = new TGTextButton(dControlsFrame, "&Position Difference Variation", BA_POS_DIFF);
  dBtnTgtParam = new TGTextButton(dControlsFrame, "&Beam Parameters at Target", BA_TGT_PARAM);
  dBtnTgtBPM = new TGTextButton(dControlsFrame, "&Target BPMs", BA_TGT_BPM);
  dBtnDetectorSensitivity = new TGTextButton(dControlsFrame, "&Detector Sensitivity", BA_DET_SENSITIVITY);
  dBtnLumiSensitivity = new TGTextButton(dControlsFrame, "&Lumi Sensitivity", BA_LUMI_SENSITIVITY);
  dBtnFgChannel = new TGTextButton(dControlsFrame, "&Function Generator", BA_FG_CHANNEL);
  dBtnLEMChannel = new TGTextButton(dControlsFrame, "&LEM Channels", BA_LEM_CHANNEL);
  dBtnTRIMChannel = new TGTextButton(dControlsFrame, "&TRIM Channels", BA_TRIM_CHANNEL);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 6, 6);

  dControlsFrame->AddFrame(dBtnPosDiff ,dBtnLayout );
  dControlsFrame->AddFrame(dBtnTgtParam,dBtnLayout );
  dControlsFrame->AddFrame(dBtnTgtBPM,dBtnLayout );
  dControlsFrame->AddFrame(dBtnDetectorSensitivity,dBtnLayout );
  dControlsFrame->AddFrame(dBtnLumiSensitivity,dBtnLayout );
  dControlsFrame->AddFrame(dBtnFgChannel,dBtnLayout );
  dControlsFrame->AddFrame(dBtnLEMChannel,dBtnLayout );
  dControlsFrame->AddFrame(dBtnTRIMChannel,dBtnLayout );

  dBtnPosDiff -> Associate(this);
  dBtnTgtParam-> Associate(this);
  dBtnTgtBPM-> Associate(this);
  dBtnDetectorSensitivity-> Associate(this);
  dBtnLumiSensitivity-> Associate(this);
  dBtnFgChannel-> Associate(this);
  dBtnLEMChannel-> Associate(this);
  dBtnTRIMChannel-> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIBeamModulation",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  //  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);
  //  super_canvas -> Divide(2,4);

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

//Stuff to do after the tab is selected
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


void QwGUIBeamModulation::PositionDifferences()
{

  Bool_t ldebug = kTRUE;
  TObject *obj = NULL;
  TCanvas *mc = NULL;
  TH1D *dummyhist = NULL;
  TPaveText * errlabel;
  Bool_t status = kTRUE;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);

  // check to see if the TH1s used for the calculation are empty.
  for(Short_t i=0;i<2;i++) 
    {
      if(PosDiffVar[i]) delete PosDiffVar[i];  PosDiffVar[i] = NULL;
    }


  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  // Get HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return; 

  if(ldebug) {
    printf("PositionDiffercences -------------------------------\n");
    printf("Found th tree named %s \n", obj->GetName());
  }

  char histo[128];
  PosDiffVar[0] = new TH1D("dxvar", "#Delta X Variation", BEAMMODULATION_DEV_NUM, 0.0, BEAMMODULATION_DEV_NUM);
  PosDiffVar[1] = new TH1D("dyvar", "#Delta Y variation", BEAMMODULATION_DEV_NUM, 0.0, BEAMMODULATION_DEV_NUM);
 
  for(Int_t p = 0; p <BEAMMODULATION_DEV_NUM ; p++) 
    {
      sprintf (histo, "diff_%sRelX.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	  obj -> Draw(histo);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetName(histo);
	  PosDiffVar[0] -> SetBinContent(p+1, dummyhist->GetMean()*1000);
	  PosDiffVar[0] -> SetBinError  (p+1, dummyhist->GetMeanError()*1000);
	  PosDiffVar[0] -> GetXaxis()->SetBinLabel(p+1,BeamModulationDevices[p]);
	  PosDiffVar[0]->SetStats(0);
	  SummaryHist(dummyhist);
	  delete dummyhist; dummyhist= NULL;
	}
      else 
	{
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  status = kFALSE;
	}

      
      sprintf (histo, "diff_%sRelY.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	  obj -> Draw(histo);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetName(histo);
	  PosDiffVar[1] -> SetBinContent(p+1, dummyhist->GetMean()*1000); //convert mum to nm
	  PosDiffVar[1] -> SetBinError  (p+1, dummyhist->GetMeanError()*1000);
	  PosDiffVar[1] -> GetXaxis()->SetBinLabel(p+1, BeamModulationDevices[p]);
	  PosDiffVar[1]->SetStats(0);
	  SummaryHist(dummyhist);
	  delete dummyhist; dummyhist= NULL;
	}
      else 
	{
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  status = kFALSE;
	}
    }

  
  if (!status) 
    {
      mc->Clear();
      errlabel ->Draw();
      mc->Update();
    }
  else
    {
      mc->Clear();
      mc->Divide(1,2);
      
      mc->cd(1);
      SummaryHist(PosDiffVar[0]);
      PosDiffVar[0] -> SetMarkerStyle(20);
      PosDiffVar[0] -> SetMarkerColor(2);
      PosDiffVar[0] -> SetTitle("#Delta X Variation");
      PosDiffVar[0] -> GetYaxis() -> SetTitle("#Delta X (nm)");
      PosDiffVar[0] -> GetXaxis() -> SetTitle("BPM X");
      PosDiffVar[0] -> Draw("E1");
      gPad->Update();
      
      
      mc->cd(2);
      SummaryHist(PosDiffVar[1]);
      PosDiffVar[1] -> SetMarkerStyle(20);
      PosDiffVar[1] -> SetMarkerColor(4);
      PosDiffVar[1] -> SetTitle("#Delta Y Variation");
      PosDiffVar[1] -> GetYaxis()-> SetTitle ("#Delta Y (nm)");
      PosDiffVar[1] -> GetXaxis() -> SetTitle("BPM Y");
      PosDiffVar[1] -> Draw("E1");
      gPad->Update();
 
  
      if(ldebug) printf("----------------------------------------------------\n");
      mc->Modified();
      mc->Update();
    }
  
  if(dummyhist) delete dummyhist;
  return;
}



void 
QwGUIBeamModulation::SummaryHist(TH1 *in)
{

  Double_t out[4] = {0.0};
  Double_t test   = 0.0;

  out[0] = in -> GetMean();
  out[1] = in -> GetMeanError();
  out[2] = in -> GetRMS();
  out[3] = in -> GetRMSError();
  test   = in -> GetRMS()/sqrt(in->GetEntries());

  //  Print the calculated values:

  //  printf("%sName%s", BOLD, NORMAL);
  //  printf("%22s", in->GetName());
  //  printf("  %sMean%s%s", BOLD, NORMAL, " : ");
  //  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[0], NORMAL, BLUE, out[1], NORMAL);
  //  printf("  %sSD%s%s", BOLD, NORMAL, " : ");
  //  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[2], NORMAL, GREEN, out[3], NORMAL);
  //  printf(" %sRMS/Sqrt(N)%s %s%+4.2e%s \n", BOLD, NORMAL, BLUE, test, NORMAL);
  return;
};

//------------------------------------------------------------------------------------------
// Display the Target BPMs response.
void QwGUIBeamModulation::DisplayTargetBPM()
{
  
  TH1D * t[5];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);


  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[5]=
    {
      "0i07WSum", "0i02WSum", "0i02aWSum", "0i05WSum", "0l04WSum", 
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,3);

  // Get the HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return;  
  if(ldebug) {
    printf("Target BPM -----------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<5;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <5 ; p++) 
    {
      sprintf (histo, "asym_qwk_%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	      sprintf (histo, "asym_qwk_%s.hw_sum*0.000076*100/(4*asym_qwk_%s.num_samples)", 
		       BeamModulationDevices[p],BeamModulationDevices[p]);
	 
	  mc->cd(p+1);
	  obj -> Draw(histo);
	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);

	  if(strcmp( BeamModulationDevices[p],"0i07WSum") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("3H09X");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0i02WSum") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("3H09Y");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0i02aWSum") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("3H09BX");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0i05WSum") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("3H09BY" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0l04WSum") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("3C12");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }

	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End Target BPM ---------------------------------\n");
  mc->Modified();
  mc->Update();

}


//------------------------------------------------------------------------------------------
// Display the Mian Detector sensitivity.
void QwGUIBeamModulation::DisplayDetectorSensitivity()
{
  
  TH1D * t[8];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);


  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[8]=
    {
      "batext1", "batext2", "batery6", "batery7", "0r05X", "0r05Y", "0r02X", "0r02Y",
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(3,3);

  // Get the HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return;  
  if(ldebug) {
    printf("Detector Sensitivity -----------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<8;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <8 ; p++) 
    {
      sprintf (histo, "yield_qwk_%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	      sprintf (histo, "yield_qwk_%s.hw_sum*0.000076*100/(4*yield_qwk_%s.num_samples)", 
		       BeamModulationDevices[p],BeamModulationDevices[p]);
	 
	  mc->cd(p+1);
	  obj -> Draw(histo);
	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);

	  if(strcmp( BeamModulationDevices[p],"batext1") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("DET 1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"batext2") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("DET 2");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"batery6") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("DET 3");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"batery7") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("DET 4" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0r05X") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("DET 5");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0r05Y") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("DET 6");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0r02X") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("DET 7");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"0r02Y") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("DET 8");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }

	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End Detector Sensitivity ---------------------------------\n");
  mc->Modified();
  mc->Update();

}


//------------------------------------------------------------------------------------------
// Display the sensitivity of the Luminosity Monitors.
void QwGUIBeamModulation::DisplayLumiSensitivity()
{
  
  TH1D * t[8];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);


  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[8]=
    {
      "dslumi1", "dslumi2", "dslumi3", "dslumi4", "dslumi5", "dslumi6", "dslumi7", "dslumi8",
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(3,3);

  // Get the HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return;  
  if(ldebug) {
    printf("Lumi Sensitivity -----------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<8;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <8 ; p++) 
    {
      sprintf (histo, "yield_qwk_%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

      	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	    sprintf (histo, "yield_qwk_%s.hw_sum*0.000076*100/(4*yield_qwk_%s.num_samples)", 
 	       BeamModulationDevices[p],BeamModulationDevices[p]);
	 
	  // To plot in symetric manner
	  //	  mc->cd(1);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(2);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(3);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(4);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(6);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(7);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(8);
	  //	  obj -> Draw(histo);
	  //	  mc->cd(9);
	  //	  obj -> Draw(histo);

	  mc->cd(p+1);
	  obj -> Draw(histo);

	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);


	  if(strcmp( BeamModulationDevices[p],"dslumi1") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("LUMI 1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
       	    }
	  else  if(strcmp( BeamModulationDevices[p],"dslumi2") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("LUMI 2");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"dslumi3") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("LUMI 3");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"dslumi4") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("LUMI 4" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"dslumi5") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("LUMI 5");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"dslumi6") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("LUMI 6");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"dslumi7") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("LUMI 7");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"dslumi8") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("LUMI 8");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }

	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End Lumi Sensitivity ---------------------------------\n");
  mc->Modified();
  mc->Update();

}

//------------------------------------------------------------------------------------------
// Display the Function Generator Channels
void QwGUIBeamModulation::DisplayFgChannel()
{ 

  TH1D * t[6];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);

  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[6]=
    {
      "qwk_fgx1", "qwk_fgx2", "qwk_fgy1", "qwk_fgy2", "qwk_fge", "qwk_fgphase",
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(3,2);

  // Get the HEL tree
  obj = HistArray.At(1);  
  if( !obj ) return;  
  if(ldebug) {
    printf("Function Generator Channels ------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<6;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <6 ; p++) 
    {
      sprintf (histo, "%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

      	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	    sprintf (histo, "%s.hw_sum*0.000076*100/(4*%s.num_samples)", 
 	       BeamModulationDevices[p],BeamModulationDevices[p]);

	  mc->cd(p+1);
	  obj -> Draw(histo);

	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);


	  if(strcmp( BeamModulationDevices[p],"qwk_fgx1") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("FG X1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
       	    }
	  else  if(strcmp( BeamModulationDevices[p],"qwk_fgx2") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("FG X2");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_fgy1") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("FG Y1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_fgy2") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("FG Y2" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_fge") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("FG E");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(52);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_fgphase") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("FG Phase");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }

	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End Function Generator Channels------------------\n");
  mc->Modified();
  mc->Update();

}

//------------------------------------------------------------------------------------------
// Display the LEM Current Transducer Channels
void QwGUIBeamModulation::DisplayLEMChannel()
{ 

  TH1D * t[4];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);

  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[4]=
    {
      "qwk_lemx1", "qwk_lemx2", "qwk_lemy1", "qwk_lemy2",
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);

  // Get the HEL tree
  obj = HistArray.At(1);  
  if( !obj ) return;  
  if(ldebug) {
    printf("LEM Channels --------------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<4;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <4 ; p++) 
    {
      sprintf (histo, "%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

      	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	    sprintf (histo, "%s.hw_sum*0.000076*100/(4*%s.num_samples)", 
 	       BeamModulationDevices[p],BeamModulationDevices[p]);

	  mc->cd(p+1);
	  obj -> Draw(histo);

	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);


	  if(strcmp( BeamModulationDevices[p],"qwk_lemx1") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("LEM X1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
       	    }
	  else  if(strcmp( BeamModulationDevices[p],"qwk_lemx2") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("LEM X2");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_lemy1") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("LEM Y1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_lemy2") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("LEM Y2" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }

	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End LEM Channels------------------------\n");
  mc->Modified();
  mc->Update();

}

//------------------------------------------------------------------------------------------
// Display the TRIM Power Amplifier Channels
void QwGUIBeamModulation::DisplayTRIMChannel()
{ 

  TH1D * t[4];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);

  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[4]=
    {
      "qwk_trimx1", "qwk_trimx2", "qwk_trimy1", "qwk_trimy2",
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);

  // Get the HEL tree
  obj = HistArray.At(1);  
  if( !obj ) return;  
  if(ldebug) {
    printf("TRIM Channels --------------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<4;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <4 ; p++) 
    {
      sprintf (histo, "%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

      	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	    sprintf (histo, "%s.hw_sum*0.000076*100/(4*%s.num_samples)", 
 	       BeamModulationDevices[p],BeamModulationDevices[p]);

	  mc->cd(p+1);
	  obj -> Draw(histo);

	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);


	  if(strcmp( BeamModulationDevices[p],"qwk_trimx1") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("TRIM X1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
       	    }
	  else  if(strcmp( BeamModulationDevices[p],"qwk_trimx2") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("TRIM X2");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_trimy1") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("TRIM Y1");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"qwk_trimy2") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("TRIM Y2" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }

	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End TRIM Channels------------------------\n");
  mc->Modified();
  mc->Update();

}


//------------------------------------------------------------------------------------------
// Display the beam postion and angle in X & Y, beam energy and beam charge on the target.
void QwGUIBeamModulation::DisplayTargetParameters()
{
  
  TH1D * t[5];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kTRUE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);


  // Beam energy is not calculated by the analyser yet. 
  const char * BeamModulationDevices[5]=
    {
      "targetX","targetY","targetXSlope","targetYSlope","average_charge"
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

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<5;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <5 ; p++) 
    {
      sprintf (histo, "yield_qwk_%s.hw_sum", BeamModulationDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	      sprintf (histo, "yield_qwk_%s.hw_sum*0.000076*100/(4*yield_qwk_%s.num_samples)", 
		       BeamModulationDevices[p],BeamModulationDevices[p]);
	 
	  mc->cd(p+1);
	  obj -> Draw(histo);
	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(BeamModulationDevices[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.7);

	  if(strcmp( BeamModulationDevices[p],"average_charge") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("Current (#muA)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"targetX") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("X Position (#mum)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"targetY") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("Y Position (#mum)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"targetXSlope") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("X Angle (#murad)" );
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(50);
	    }
	  else 	if(strcmp( BeamModulationDevices[p],"targetYSlope") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("Y Angle (#murad)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(8);
	    }
	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("End of Target beam parameters---------------------------------\n");
  mc->Modified();
  mc->Update();

}


//-------------------------------------------------------------------------------------------
// Process events generated by the object in the frame.
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
		case   BA_POS_DIFF:
		  PositionDifferences();
		  break;
		  
		case BA_TGT_PARAM:
		  DisplayTargetParameters();
		  break;
		
		case BA_TGT_BPM:
		  DisplayTargetBPM();
		  break;

		case BA_DET_SENSITIVITY:
		  DisplayDetectorSensitivity();
		  break;
		
		case BA_LUMI_SENSITIVITY:
		  DisplayLumiSensitivity();
		  break;

		case BA_FG_CHANNEL:
		  DisplayFgChannel();
		  break;

		case BA_LEM_CHANNEL:
		  DisplayLEMChannel();
		  break;

		case BA_TRIM_CHANNEL:
		  DisplayTRIMChannel();
		  break;

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



