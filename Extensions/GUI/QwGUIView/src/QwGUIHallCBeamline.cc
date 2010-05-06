#include "QwGUIHallCBeamline.h"

#include <TG3DLine.h>
#include "TGaxis.h"

ClassImp(QwGUIHallCBeamline);

enum QwGUIHallCBeamlineIndentificator {
  BA_POS_DIFF,
  BA_TGT_PARAM

};

const char *QwGUIHallCBeamline::RootTrees[TRE_NUM] = 
  {
    "HEL_Tree",
    "MPS_Tree"
  };

const char *QwGUIHallCBeamline::HallCBeamlineDevices[HCLINE_DEV_NUM]=
  {
    "qwk_3c07","qwk_3c08","qwk_3c11","qwk_3c12","qwk_3c14","qwk_3c16"
  };


QwGUIHallCBeamline::QwGUIHallCBeamline(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dBtnLayout          = NULL;
  dBtnPosDiff         = NULL;
  dBtnTgtParam        = NULL;

  PosDiffVar[0] = NULL;
  PosDiffVar[1] = NULL;
 
  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

}

QwGUIHallCBeamline::~QwGUIHallCBeamline()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dBtnPosDiff)         delete dBtnPosDiff;
  if(dBtnTgtParam)        delete dBtnTgtParam;

  delete [] PosDiffVar;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIHallCBeamline::MakeLayout()
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
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.25);
  gStyle->SetPadLeftMargin(0.1);  
  gStyle->SetPadRightMargin(0.03);  

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.08);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleBorderSize(0);
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
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dBtnPosDiff ,dBtnLayout );
  dControlsFrame->AddFrame(dBtnTgtParam,dBtnLayout );

  dBtnPosDiff -> Associate(this);
  dBtnTgtParam-> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIHallCBeamline",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  //  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);
  //  super_canvas -> Divide(2,4);

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


void QwGUIHallCBeamline::OnNewDataContainer()
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
    
    if(pad > 0 && pad <= HCLINE_DEV_NUM)
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


void QwGUIHallCBeamline::PositionDifferences()
{

  Bool_t ldebug = kFALSE;
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
  PosDiffVar[0] = new TH1D("dxvar", "#Delta X Variation", HCLINE_DEV_NUM, 0.0, HCLINE_DEV_NUM);
  PosDiffVar[1] = new TH1D("dyvar", "#Delta Y variation", HCLINE_DEV_NUM, 0.0, HCLINE_DEV_NUM);
 
  for(Int_t p = 0; p <HCLINE_DEV_NUM ; p++) 
    {
      sprintf (histo, "diff_%sRelX.hw_sum", HallCBeamlineDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	  obj -> Draw(histo);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetName(histo);
	  PosDiffVar[0] -> SetBinContent(p+1, dummyhist->GetMean()*1000);
	  PosDiffVar[0] -> SetBinError  (p+1, dummyhist->GetMeanError()*1000);
	  PosDiffVar[0] -> GetXaxis()->SetBinLabel(p+1,HallCBeamlineDevices[p]);
	  PosDiffVar[0]->SetStats(0);
	  SummaryHist(dummyhist);
	  delete dummyhist; dummyhist= NULL;
	}
      else 
	{
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  status = kFALSE;
	}

      
      sprintf (histo, "diff_%sRelY.hw_sum", HallCBeamlineDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	  obj -> Draw(histo);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetName(histo);
	  PosDiffVar[1] -> SetBinContent(p+1, dummyhist->GetMean()*1000); //convert mum to nm
	  PosDiffVar[1] -> SetBinError  (p+1, dummyhist->GetMeanError()*1000);
	  PosDiffVar[1] -> GetXaxis()->SetBinLabel(p+1, HallCBeamlineDevices[p]);
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
QwGUIHallCBeamline::SummaryHist(TH1 *in)
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





// Display the beam postion and angle in X & Y, beam energy and beam charge on the target.
void QwGUIHallCBeamline::DisplayTargetParameters()
{
  
  TH1D * t[5];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kFALSE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);


  // Beam energy is not calculated by the analyser yet. 
  const char * TgtBeamParameters[5]=
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
      sprintf (histo, "yield_qwk_%s.hw_sum", TgtBeamParameters[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

	  if(strcmp( TgtBeamParameters[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	      sprintf (histo, "yield_qwk_%s.hw_sum*0.000076*100/(4*yield_qwk_%s.num_samples)", 
		       TgtBeamParameters[p],TgtBeamParameters[p]);
	 
	  mc->cd(p+1);
	  obj -> Draw(histo);
	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(TgtBeamParameters[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.5);

	  if(strcmp( TgtBeamParameters[p],"average_charge") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("Current (#muA)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetX") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("X Position (#mum)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(34);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetY") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("Y Position (#mum)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(46);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetXSlope") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("X Slope");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(34);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetYSlope") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("Y Slope");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(46);
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

  if(ldebug) printf("----------------------------------------------------\n");
  mc->Modified();
  mc->Update();

}




// Process events generated by the object in the frame.
Bool_t QwGUIHallCBeamline::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
	      case M_TBIN_SELECT:
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



