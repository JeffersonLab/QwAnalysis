#include "QwGUITrackFinding.h"

#include <TG3DLine.h>
#include <TGaxis.h>

#include "QwDetectorInfo.h"
#include "QwEvent.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"

////////////////Injector stuff/////////////////
#include "QwGUIInjector.h"

#include <TG3DLine.h>
#include "TGaxis.h"


////////////end injector stuff////////////

ClassImp(QwGUITrackFinding);

enum QwGUITrackFindingIndentificator {
  TF_CHI2,
  TF_RESIDUAL,
  TF_OFFSET,
  TF_SLOPE,
  TF_PCHI2,
  TF_PRESIDUAL,
  TF_POFFSET,
  TF_PSLOPE,
//drop down menu
  TF_PLOT_LINEAR,
  TF_PLOT_LOG,
  TL_PLOT_SLOPE_X,
  TL_PLOT_SLOPE_U,
  TL_PLOT_SLOPE_V,
  TL_PLOT_CHI2_X,
  TL_PLOT_CHI2_U,
  TL_PLOT_CHI2_V,
  TL_PLOT_OFFSET_X,
  TL_PLOT_OFFSET_U,
  TL_PLOT_OFFSET_V,
  TL_PLOT_RESIDUAL_X,
  TL_PLOT_RESIDUAL_U,
  TL_PLOT_RESIDUAL_V,
};

QwGUITrackFinding::QwGUITrackFinding(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dControlsFrame1     = NULL;
  dCanvas             = NULL;
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dBtnLayout          = NULL;
  dBtnChi2            = NULL;
  dBtnResidual        = NULL;
  dBtnOffset	      = NULL;
  dBtnSlope	      = NULL;
  dBtnpChi2           = NULL;
  dBtnpResidual       = NULL;
  dBtnpOffset	      = NULL;
  dBtnpSlope	      = NULL;

  for (Short_t i = 0; i < CHAMBERS_NUM; i++)
    {
      histoChi2[i] = NULL;
    }

  // Name of the relevant ROOT tree
  fRootTrees.push_back("hit_tree");
  fRootTrees.push_back("event_tree");

  // Selection criteria for all drift chambers
  fDriftChambers.push_back("fRegion==2");
  fDriftChambers.push_back("fRegion==3");

  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

}

QwGUITrackFinding::~QwGUITrackFinding()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dControlsFrame1)     delete dControlsFrame1;
  if(dCanvas)             delete dCanvas;
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dBtnChi2)            delete dBtnChi2;
  if(dBtnResidual)        delete dBtnResidual;
  if(dBtnOffset)	  delete dBtnOffset;
  if(dBtnSlope) 	  delete dBtnSlope;
  if(dBtnpChi2)           delete dBtnpChi2;
  if(dBtnpResidual)       delete dBtnpResidual;
  if(dBtnpOffset)	  delete dBtnpOffset;
  if(dBtnpSlope) 	  delete dBtnpSlope;

  delete[] histoChi2;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUITrackFinding::MakeLayout()
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

  dBigFrame = new TGVerticalFrame(this);
  dTabFrame = new TGHorizontalFrame(this);
//  dBigFrame->AddFrame(dTabFrame,new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));
  AddFrame(dBigFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);  //For TreeLine buttons
  dControlsFrame1 = new TGVerticalFrame(this); //For PartialTrack buttons
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 25, 5));
  dTabFrame->AddFrame(dControlsFrame1, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 25, 5));

  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 0,0,30,0));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200);
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 1, 10));

  dBtnChi2  = new TGTextButton(dControlsFrame, "&TL_Chi2", TF_CHI2);
  dBtnResidual = new TGTextButton(dControlsFrame, "&TL_Residuals", TF_RESIDUAL);
  dBtnOffset  = new TGTextButton(dControlsFrame, "&TL_Offset", TF_OFFSET);
  dBtnSlope  = new TGTextButton(dControlsFrame, "&TL_Slope", TF_SLOPE);
  dBtnpChi2  = new TGTextButton(dControlsFrame1, "&PT_Chi2", TF_PCHI2);
  dBtnpResidual = new TGTextButton(dControlsFrame1, "&PT_Residuals", TF_PRESIDUAL);
  dBtnpOffset  = new TGTextButton(dControlsFrame1, "&PT_Offset", TF_POFFSET);
  dBtnpSlope  = new TGTextButton(dControlsFrame1, "&PT_Slope", TF_PSLOPE);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dBtnChi2 ,dBtnLayout );
  dControlsFrame->AddFrame(dBtnResidual,dBtnLayout );
  dControlsFrame->AddFrame(dBtnOffset,dBtnLayout );
  dControlsFrame->AddFrame(dBtnSlope,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpChi2 ,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpResidual,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpOffset,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpSlope,dBtnLayout );

  dBtnChi2 -> Associate(this);
  dBtnResidual-> Associate(this);
  dBtnOffset -> Associate(this);
  dBtnSlope -> Associate(this);
  dBtnpChi2 -> Associate(this);
  dBtnpResidual-> Associate(this);
  dBtnpOffset -> Associate(this);
  dBtnpSlope -> Associate(this);

///////////// Menu drop down /////////////
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);

  dTabFrameVert = new TGVerticalFrame(this,10,10);


  dMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
				     0, 0, 1, 1);
  dMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  
  dMenuPlot1 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot2 = new TGPopupMenu(fClient->GetRoot());

  dMenuPlot1->AddEntry("&Linear", TF_PLOT_LINEAR);
  dMenuPlot1->AddEntry("&Logarithmic", TF_PLOT_LOG);

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Scale", dMenuPlot1, dMenuBarItemLayout);
  dMenuBar->AddPopup("&Plane", dMenuPlot2, dMenuBarItemLayout);

  dTabFrameVert->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuPlot1->Associate(this);
  dMenuPlot2->Associate(this);
  dBigFrame->AddFrame(dTabFrameVert, dMenuBarLayout);
  dBigFrame->AddFrame(dTabFrame,new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));


  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUITrackFinding",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  //  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);
  //  super_canvas -> Divide(2,4);

}

void QwGUITrackFinding::OnReceiveMessage(char *obj)
{

}

void QwGUITrackFinding::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
    dROOTCont = NULL;
  }
}


void QwGUITrackFinding::OnNewDataContainer(RDataContainer *cont)
{
std::cout << "Opened" << std::endl;
  if (!cont) return;

  if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont) {

    TObject* obj = dROOTCont->ReadData(fRootTrees.at(1));
    
    // Note: fRootTrees[0] = hit_tree, fRootTrees[1] = event_tree

    if (obj) {
	
       if (obj->InheritsFrom("TTree")) {
         TTree* tree = (TTree*) obj->Clone();

         if (tree -> FindBranch("events")) //if (tree -> FindBranch("events"))
           {

            Int_t entries = tree->GetEntries();
	    std::cout<<"Entries in tree: "<< entries << std::endl;

            QwHitRootContainer* roothitlist = 0;
            QwEvent* event = 0;

            tree->SetBranchAddress("hits", &roothitlist);
            tree->SetBranchAddress("events", &event); //there is no events is hit_tree 6/30/10

// Create histogram using Draw
// Region 2 in blue and Region 3 in green

// Histogram from fQwTreeLines
// TreeLines Chi2
//TL Chi2 R2
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2", "events.fQwTreeLines.fRegion==2"); //(100,0.0,60.0)
	    TH1F *fChiR2 = (TH1F*)gDirectory->Get("fChiR2");
	    fChiR2->SetFillColor(kBlue);
            fChiR2->SetTitle("TreeLines Chi2 (Region 2)");
//TL Chi2 R2 X,U,V Planes
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2X", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==1"); //(100,0.0,60.0)
	    TH1F *fChiR2X = (TH1F*)gDirectory->Get("fChiR2X");
	    fChiR2X->SetFillColor(kBlue);
            fChiR2X->SetTitle("TreeLines Chi2 (Region 2, X Plane)");
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2U", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==3"); //(100,0.0,60.0)
	    TH1F *fChiR2U = (TH1F*)gDirectory->Get("fChiR2U");
	    fChiR2U->SetFillColor(kBlue);
            fChiR2U->SetTitle("TreeLines Chi2 (Region 2, U Plane)");
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2V", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==4"); //(100,0.0,60.0)
	    TH1F *fChiR2V = (TH1F*)gDirectory->Get("fChiR2V");
	    fChiR2V->SetFillColor(kBlue);
            fChiR2V->SetTitle("TreeLines Chi2 (Region 2, V Plane)");
//TL Chi2 R3
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR3", "events.fQwTreeLines.fRegion==3");
	    TH1F *fChiR3 = (TH1F*)gDirectory->Get("fChiR3");
	    fChiR3->SetFillColor(kGreen);
            fChiR3->SetTitle("TreeLines Chi2 (Region 3)");
//TL Chi2 R3 U,V
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR3U", "events.fQwTreeLines.fRegion==3&& (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
	    TH1F *fChiR3U = (TH1F*)gDirectory->Get("fChiR3U");
	    fChiR3U->SetFillColor(kGreen);
            fChiR3U->SetTitle("TreeLines Chi2 (Region 3, U Plane)");
	    tree->Draw("events.fQwTreeLines.fChi>>fChiR3V", "events.fQwTreeLines.fRegion==3&& (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
	    TH1F *fChiR3V = (TH1F*)gDirectory->Get("fChiR3V");
	    fChiR3V->SetFillColor(kGreen);
            fChiR3V->SetTitle("TreeLines Chi2 (Region 3, V Plane)");

//TreeLines Residuals
	    tree->Draw("events.fQwTreeLines.fAverageResidual>>Residuals");
	    TH1F *Residuals = (TH1F*)gDirectory->Get("Residuals");

//TreeLines Offsets
	    tree->Draw("events.fQwTreeLines.fOffset>>offset"); //(100.,-170.,110.)
	    TH1F *offset = (TH1F*)gDirectory->Get("offset");
//TL Offsets R2
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2","events.fQwTreeLines.fRegion==2");
	    TH1F *offsetR2 = (TH1F*)gDirectory->Get("offsetR2");
	    offsetR2->SetFillColor(kBlue);
            offsetR2->SetTitle("TreeLines Offset (Region 2)");
//TL Offsets R2 X,U,V Planes
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2X","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==1");
	    TH1F *offsetR2X = (TH1F*)gDirectory->Get("offsetR2X");
	    offsetR2X->SetFillColor(kBlue);
            offsetR2X->SetTitle("TreeLines Offset (Region 2, X Plane)");
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2U","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==3");
	    TH1F *offsetR2U = (TH1F*)gDirectory->Get("offsetR2U");
	    offsetR2U->SetFillColor(kBlue);
            offsetR2U->SetTitle("TreeLines Offset (Region 2, U Plane)");
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2V","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==4");
	    TH1F *offsetR2V = (TH1F*)gDirectory->Get("offsetR2V");
	    offsetR2V->SetFillColor(kBlue);
            offsetR2V->SetTitle("TreeLines Offset (Region 2, V Plane)");
//TL Offsets R3
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR3","events.fQwTreeLines.fRegion==3");
	    TH1F *offsetR3 = (TH1F*)gDirectory->Get("offsetR3");
	    offsetR3->SetFillColor(kGreen);
            offsetR3->SetTitle("TreeLines Offset (Region 3)");
//TL Offsets R3 U,V Planes
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR3U","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
	    TH1F *offsetR3U = (TH1F*)gDirectory->Get("offsetR3U");
	    offsetR3U->SetFillColor(kGreen);
            offsetR3U->SetTitle("TreeLines Offset (Region 3, U Plane)");
	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR3V","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
	    TH1F *offsetR3V = (TH1F*)gDirectory->Get("offsetR3V");
	    offsetR3V->SetFillColor(kGreen);
            offsetR3V->SetTitle("TreeLines Offset (Region 3, V Plane)");

//TreeLines Slope
	    tree->Draw("events.fQwTreeLines.fSlope>>slope"); //(100,-0.2,1.6)
	    TH1F *slope = (TH1F*)gDirectory->Get("slope");
	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2","events.fQwTreeLines.fRegion==2");
	    TH1F *slopeR2 = (TH1F*)gDirectory->Get("slopeR2");
	    slopeR2->SetFillColor(kBlue);
            slopeR2->SetTitle("TreeLines Slope (Region 2)");
	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2X","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==1");
	    TH1F *slopeR2X = (TH1F*)gDirectory->Get("slopeR2X");
	    slopeR2X->SetFillColor(kBlue);
            slopeR2X->SetTitle("TreeLines Slope (Region 2, X Planes)");
	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2U","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==3");
	    TH1F *slopeR2U = (TH1F*)gDirectory->Get("slopeR2U");
	    slopeR2U->SetFillColor(kBlue);
            slopeR2U->SetTitle("TreeLines Slope (Region 2, U Planes)");
	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2V","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==4");
	    TH1F *slopeR2V = (TH1F*)gDirectory->Get("slopeR2V");
	    slopeR2V->SetFillColor(kBlue);
            slopeR2V->SetTitle("TreeLines Slope (Region 2, V Planes)");

	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR3","events.fQwTreeLines.fRegion==3");
	    TH1F *slopeR3 = (TH1F*)gDirectory->Get("slopeR3");
	    slopeR3->SetFillColor(kGreen);
            slopeR3->SetTitle("TreeLines Slope (Region 3)");
	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR3U","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
	    TH1F *slopeR3U= (TH1F*)gDirectory->Get("slopeR3U");
	    slopeR3U->SetFillColor(kGreen);
            slopeR3U->SetTitle("TreeLines Slope (Region 3, U Planes)");
	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR3V","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
	    TH1F *slopeR3V= (TH1F*)gDirectory->Get("slopeR3V");
	    slopeR3V->SetFillColor(kGreen);
            slopeR3V->SetTitle("TreeLines Slope (Region 3, V Planes)");

//Historams from tQwPartialTracks
//PartialTracks Chi2
	    tree->Draw("events.fQwPartialTracks.fChi>>fpChiR2", "events.fQwPartialTracks.fRegion==2 "); //  (100,0.0,2000.0)
	    TH1F *fpChiR2 = (TH1F*)gDirectory->Get("fpChiR2");
	    fpChiR2->SetFillColor(kBlue);
            fpChiR2->SetTitle("PartialTracks Chi2 (Region 2)");
	    tree->Draw("events.fQwPartialTracks.fChi>>fpChiR3", "events.fQwPartialTracks.fRegion==3");
	    TH1F *fpChiR3 = (TH1F*)gDirectory->Get("fpChiR3");
	    fpChiR3->SetFillColor(kGreen);
            fpChiR3->SetTitle("PartialTracks Chi2 (Region 3)");
//PartialTracks Residuals
	    tree->Draw("events.fQwPartialTracks.fAverageResidual>>pResiduals");
	    TH1F *pResiduals = (TH1F*)gDirectory->Get("pResiduals");
//PartialTracks Offset X
// 	    tree->Draw("events.fQwPartialTracks.fOffsetX>>poffsetX"); //(100.,-100000.,160000.)
// 	    TH1F *poffsetX = (TH1F*)gDirectory->Get("poffsetX");
	    tree->Draw("events.fQwPartialTracks.fOffsetX>>poffsetXR2","events.fQwPartialTracks.fRegion==2");
	    TH1F *poffsetXR2 = (TH1F*)gDirectory->Get("poffsetXR2");
	    poffsetXR2->SetFillColor(kBlue);
            poffsetXR2->SetTitle("PartialTracks X Offset (Region 2)");
	    tree->Draw("events.fQwPartialTracks.fOffsetX>>poffsetXR3","events.fQwPartialTracks.fRegion==3");
	    TH1F *poffsetXR3 = (TH1F*)gDirectory->Get("poffsetXR3");
	    poffsetXR3->SetFillColor(kGreen);
            poffsetXR3->SetTitle("PartialTracks X Offset (Region 3)");
//PartialTracks Offset Y
// 	    tree->Draw("events.fQwPartialTracks.fOffsetY>>poffsetY"); //(100.,-40000.,90000.)
// 	    TH1F *poffsetY = (TH1F*)gDirectory->Get("poffsetY");
	    tree->Draw("events.fQwPartialTracks.fOffsetY>>poffsetYR2","events.fQwPartialTracks.fRegion==2");
	    TH1F *poffsetYR2 = (TH1F*)gDirectory->Get("poffsetYR2");
	    poffsetYR2->SetFillColor(kBlue-4);
            poffsetYR2->SetTitle("PartialTracks Y Offset (Region 2)");
	    tree->Draw("events.fQwPartialTracks.fOffsetY>>poffsetYR3","events.fQwPartialTracks.fRegion==3");
	    TH1F *poffsetYR3 = (TH1F*)gDirectory->Get("poffsetYR3");
	    poffsetYR3->SetFillColor(kGreen-4);
            poffsetYR3->SetTitle("PartialTracks Y Offset (Region 3)");
//PartialTracks Slope X
// 	    tree->Draw("events.fQwPartialTracks.fSlopeX>>pSlopeX"); //(100.,-350.,250.)
// 	    TH1F *pSlopeX = (TH1F*)gDirectory->Get("pSlopeX");
	    tree->Draw("events.fQwPartialTracks.fSlopeX>>pSlopeXR2","events.fQwPartialTracks.fRegion==2");
	    TH1F *pSlopeXR2 = (TH1F*)gDirectory->Get("pSlopeXR2");
	    pSlopeXR2->SetFillColor(kBlue);
            pSlopeXR2->SetTitle("PartialTracks X Slope (Region 2)");
	    tree->Draw("events.fQwPartialTracks.fSlopeX>>pSlopeXR3","events.fQwPartialTracks.fRegion==3");
	    TH1F *pSlopeXR3 = (TH1F*)gDirectory->Get("pSlopeXR3");
	    pSlopeXR3->SetFillColor(kGreen); 
            pSlopeXR3->SetTitle("PartialTracks X Slope (Region 3)");
//PartialTracks Slope Y
// 	    tree->Draw("events.fQwPartialTracks.fSlopeY>>pSlopeY"); //(100.,-200.,100.)
// 	    TH1F *pSlopeY = (TH1F*)gDirectory->Get("pSlopeY");
	    tree->Draw("events.fQwPartialTracks.fSlopeY>>pSlopeYR2","events.fQwPartialTracks.fRegion==2");
	    TH1F *pSlopeYR2 = (TH1F*)gDirectory->Get("pSlopeYR2");
	    pSlopeYR2->SetFillColor(kBlue-4);
            pSlopeYR2->SetTitle("PartialTracks Y Slope (Region 2)");
	    tree->Draw("events.fQwPartialTracks.fSlopeY>>pSlopeYR3","events.fQwPartialTracks.fRegion==3");
	    TH1F *pSlopeYR3 = (TH1F*)gDirectory->Get("pSlopeYR3");
	    pSlopeYR3->SetFillColor(kGreen-4);
            pSlopeYR3->SetTitle("PartialTracks Y Slope (Region 3)");

//fQwHits
	    tree->Draw("events.fQwTreeLines.fQwHits.fDistance>>hdist", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fPlane!=0");
            TH1F *hdist = (TH1F*)gDirectory->Get("hdist");
	    tree->Draw("events.fQwTreeLines.fNQwHits>>fNQwHits");
	    TH1F *fNQwHits = (TH1F*)gDirectory->Get("fNQwHits");

//  Add Histogram to HistArray

	   HistArray.Add(fChiR2);	//0
	   HistArray.Add(fChiR3);
 	   HistArray.Add(Residuals);
           HistArray.Add(offset);
	   HistArray.Add(offsetR2);
	   HistArray.Add(offsetR3);	//5
	   HistArray.Add(slope);
	   HistArray.Add(slopeR2);
	   HistArray.Add(slopeR3);

	   HistArray.Add(fpChiR2);	
	   HistArray.Add(fpChiR3);	//10
	   HistArray.Add(poffsetXR2);
	   HistArray.Add(poffsetXR3);
	   HistArray.Add(poffsetYR2);
	   HistArray.Add(poffsetYR3);
	   HistArray.Add(pSlopeXR2);	//15
	   HistArray.Add(pSlopeXR3);
	   HistArray.Add(pSlopeYR2);
	   HistArray.Add(pSlopeYR3);
 	   HistArray.Add(pResiduals);

	   HistArray.Add(slopeR2X);	//20
	   HistArray.Add(slopeR2U);
	   HistArray.Add(slopeR2V);
	   HistArray.Add(slopeR3U);
	   HistArray.Add(slopeR3V);

	   HistArray.Add(fChiR2X);	//25
	   HistArray.Add(fChiR2U);
	   HistArray.Add(fChiR2V);
	   HistArray.Add(fChiR3U);
	   HistArray.Add(fChiR3V);

	   HistArray.Add(offsetR2X);	//30
	   HistArray.Add(offsetR2U);
	   HistArray.Add(offsetR2V);
	   HistArray.Add(offsetR3U);
	   HistArray.Add(offsetR3V);

           HistArray.Add(hdist);	
	   HistArray.Add(fNQwHits);

        }
	else{
	std::cout<<"no events branch"<<std::endl;
        }
	
      }
      else{
	std::cout<<"no Tree inherited"<<std::endl;
      }
        
  
//     TCanvas *mc = NULL;
//     mc = dCanvas->GetCanvas();
//     mc->Clear();
//        PlotHistograms();
    }
    else{
	std::cout<<"no directory fRootTrees"<<std::endl;
    }
  }

};

void QwGUITrackFinding::OnRemoveThisTab()
{

};

void QwGUITrackFinding::ClearData()
{
  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();
  while (obj) {
    delete obj;
    obj = next();
  }
  HistArray.Clear();
}

//Stuff to do after the tab is selected
void QwGUITrackFinding::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();

    if(pad > 0 && pad <= CHAMBERS_NUM)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUITrackFinding",
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

void QwGUITrackFinding::PlotHistograms()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();

  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
    ((TH1*)obj)->Draw("");
    std::cout << "Plotting histo " << ind << std::endl;
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();
}

void QwGUITrackFinding::PlotLinear()
{
  std::cout<<"Linear..."<<std::endl;
  TCanvas *mc = dCanvas->GetCanvas();
  mc->cd(1)->SetLogy(0);
  mc->cd(2)->SetLogy(0);
  mc->cd(3)->SetLogy(0);
  mc->cd(4)->SetLogy(0);
  gPad->Update();
}

void QwGUITrackFinding::PlotLog()
{
  std::cout<<"Logarithmic..."<<std::endl;
  TCanvas *mc = dCanvas->GetCanvas();
  mc->cd(1)->SetLogy(1);
  mc->cd(2)->SetLogy(1);
  mc->cd(3)->SetLogy(1);
  mc->cd(4)->SetLogy(1);
  gPad->Update();
}

// TreeLines
void QwGUITrackFinding::PlotChi2()
{
  std::cout<<"Plotting Chi2..."<<std::endl;

  dMenuPlot1->DisableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->DisableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  dMenuPlot2->AddEntry("&X", TL_PLOT_CHI2_X);
  dMenuPlot2->AddEntry("&U",  TL_PLOT_CHI2_U);
  dMenuPlot2->AddEntry("&V",  TL_PLOT_CHI2_V);
  dMenuPlot2->AddEntry("&All", TF_CHI2);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);


  obj = HistArray.At(0);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();
  mc->cd(2);
  mc->cd(2)->SetLogy();
  obj->Draw();

  obj = HistArray.At(1);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();
  mc->cd(4);
  mc->cd(4)->SetLogy();
  obj->Draw();
  gPad->Update();

  std::cout<<"Done"<<std::endl;

}

// Display the beam postion and angle in X & Y, beam energy and beam charge on the target.
void QwGUITrackFinding::PlotResidual()
{  std::cout<<"Plotting Residuals..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  dMenuPlot2->AddEntry("&X", TL_PLOT_RESIDUAL_X);
  dMenuPlot2->AddEntry("&U",  TL_PLOT_RESIDUAL_U);
  dMenuPlot2->AddEntry("&V",  TL_PLOT_RESIDUAL_V);
  dMenuPlot2->AddEntry("&All", TF_RESIDUAL);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();


  obj = HistArray.At(2);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();
  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotOffset() 
{  std::cout<<"Plotting Offset..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  dMenuPlot2->AddEntry("&X", TL_PLOT_OFFSET_X);
  dMenuPlot2->AddEntry("&U",  TL_PLOT_OFFSET_U);
  dMenuPlot2->AddEntry("&V",  TL_PLOT_OFFSET_V);
  dMenuPlot2->AddEntry("&All", TF_OFFSET);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);
//   obj = HistArray.At(3);  // Get histogram from tree
//   if(! obj) 
// 	{
// 	std::cout<<"Error: no obj in HistArray"<<std::endl;
// 	return;
// 	};
//   obj->Draw();

  obj = HistArray.At(4);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(5);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotSlope()
{  std::cout<<"Plotting Slope..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  dMenuPlot2->AddEntry("&X", TL_PLOT_SLOPE_X);
  dMenuPlot2->AddEntry("&U",  TL_PLOT_SLOPE_U);
  dMenuPlot2->AddEntry("&V",  TL_PLOT_SLOPE_V);
  dMenuPlot2->AddEntry("&All", TF_SLOPE);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);
  obj = HistArray.At(7);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(8);
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}


//PartialTracks
void QwGUITrackFinding::PlotpChi2()
{
  std::cout<<"Plotting Chi2..."<<std::endl;

  dMenuPlot1->DisableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->DisableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  TObject *obj = NULL;  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);

  obj = HistArray.At(9);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();
  mc->cd(2);
  mc->cd(2)->SetLogy();
  obj->Draw();

  obj = HistArray.At(10);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();
  mc->cd(4);
  mc->cd(4)->SetLogy();
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotpResidual()
{  std::cout<<"Plotting Residuals..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();

  obj = HistArray.At(19);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();
  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotpOffset()
{  std::cout<<"Plotting Offset..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);

  mc->cd(1)->SetLogy();
  obj = HistArray.At(11);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2)->SetLogy();
  obj = HistArray.At(13);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3)->SetLogy();
  obj = HistArray.At(12);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(4)->SetLogy();
  obj = HistArray.At(14);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();
  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotpSlope()
{  std::cout<<"Plotting Slope..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("X"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("U"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("V"));
  dMenuPlot2->DeleteEntry(dMenuPlot2->GetEntry("All"));

   TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);

  mc->cd(1)->SetLogy();
  obj = HistArray.At(15);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2)->SetLogy();
  obj = HistArray.At(17);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3)->SetLogy();
  obj = HistArray.At(16);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(4)->SetLogy();
  obj = HistArray.At(18);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();
  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

// Plotting X,U, and V Planes

void QwGUITrackFinding::PlotTLChi2X()
{
  std::cout<<"Plotting Chi2 X..."<<std::endl;

  dMenuPlot1->DisableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->DisableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);


  obj = HistArray.At(25);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();
  mc->cd(2);
  mc->cd(2)->SetLogy();
  obj->Draw();

mc->cd(3)->Clear();
mc->cd(4)->Clear();

  gPad->Update();

  std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotTLChi2U()
{
  std::cout<<"Plotting Chi2 U..."<<std::endl;

  dMenuPlot1->DisableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->DisableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);


  obj = HistArray.At(26);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();
  mc->cd(2);
  mc->cd(2)->SetLogy();
  obj->Draw();

  obj = HistArray.At(28);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();
  mc->cd(4);
  mc->cd(4)->SetLogy();
  obj->Draw();
  gPad->Update();

  std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotTLChi2V()
{
  std::cout<<"Plotting Chi2 V..."<<std::endl;

  dMenuPlot1->DisableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->DisableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,2);


  obj = HistArray.At(27);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();
  mc->cd(2);
  mc->cd(2)->SetLogy();
  obj->Draw();

  obj = HistArray.At(29);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();
  mc->cd(4);
  mc->cd(4)->SetLogy();
  obj->Draw();
  gPad->Update();

  std::cout<<"Done"<<std::endl;
}


void QwGUITrackFinding::PlotTLOffsetX()
{  std::cout<<"Plotting Offset X..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);

  obj = HistArray.At(30);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2)->Clear();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLOffsetU()
{  std::cout<<"Plotting Offset U..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);

  obj = HistArray.At(31);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(33);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLOffsetV()
{  std::cout<<"Plotting Offset V..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);

  obj = HistArray.At(32);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(34);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLSlopeX()
{  std::cout<<"Plotting Slope X..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);


  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);
  obj = HistArray.At(20);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2)->Clear(); //There is no X Plane for Region 3

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLSlopeU()
{  std::cout<<"Plotting Slope U..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);
  obj = HistArray.At(21);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(23);
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLSlopeV()
{  std::cout<<"Plotting Slope V..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  mc->cd(1);
  obj = HistArray.At(22);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(24);
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
  std::cout<<"Done"<<std::endl;
  return;
}


// Process events generated by the object in the frame.
Bool_t QwGUITrackFinding::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
		case TF_CHI2:
		  PlotChi2();
		  break;

		case TF_RESIDUAL:
		  PlotResidual();
		  break;

		case TF_OFFSET:
		  PlotOffset();
		  break;

		case TF_SLOPE:
		  PlotSlope();
		  break;

		case TF_PCHI2:
		  PlotpChi2();
		  break;

		case TF_PRESIDUAL:
		  PlotpResidual();
		  break;

		case TF_POFFSET:
		  PlotpOffset();
		  break;

		case TF_PSLOPE:
		  PlotpSlope();
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

	    case TF_PLOT_LINEAR:
	      PlotLinear();
	      break;

	    case TF_PLOT_LOG:
	      PlotLog();
	      break;

	    case  TL_PLOT_SLOPE_X:
	      PlotTLSlopeX();
	      break;

	    case  TL_PLOT_SLOPE_U:
	      PlotTLSlopeU();
	      break;

	    case  TL_PLOT_SLOPE_V:
	      PlotTLSlopeV();
	      break;

	    case TF_SLOPE:
	      PlotSlope();
	      break;

	    case TL_PLOT_CHI2_X:
	      PlotTLChi2X();
	      break;

	    case TL_PLOT_CHI2_U:
	      PlotTLChi2U();
	      break;

	    case TL_PLOT_CHI2_V:
	      PlotTLChi2V();
	      break;

	    case TF_CHI2:
	      PlotChi2();
	      break;

	    case TL_PLOT_OFFSET_X:
	      PlotTLOffsetX();
	      break;

	    case TL_PLOT_OFFSET_U:
	      PlotTLOffsetU();
	      break;

	    case TL_PLOT_OFFSET_V:
	      PlotTLOffsetV();
	      break;

	    case TF_OFFSET:
	      PlotOffset();
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



