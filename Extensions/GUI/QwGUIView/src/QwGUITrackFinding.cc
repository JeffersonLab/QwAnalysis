#include "QwGUITrackFinding.h"

#include <TG3DLine.h>
#include <TGaxis.h>

#include "QwDetectorInfo.h"
#include "QwEvent.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"
#include "QwTreeLine.h"
#include "QwPartialTrack.h"

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
  TF_PPACKAGE,
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
  dBtnPackage	      = NULL;

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
  if(dBtnPackage) 	  delete dBtnPackage;

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
  dBtnPackage = new TGTextButton(dControlsFrame1, "&PT_Package", TF_PPACKAGE);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dBtnChi2 ,dBtnLayout );
  dControlsFrame->AddFrame(dBtnResidual,dBtnLayout );
  dControlsFrame->AddFrame(dBtnOffset,dBtnLayout );
  dControlsFrame->AddFrame(dBtnSlope,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpChi2 ,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpResidual,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpOffset,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnpSlope,dBtnLayout );
  dControlsFrame1->AddFrame(dBtnPackage,dBtnLayout );

  dBtnChi2 -> Associate(this);
  dBtnResidual-> Associate(this);
  dBtnOffset -> Associate(this);
  dBtnSlope -> Associate(this);
  dBtnpChi2 -> Associate(this);
  dBtnpResidual-> Associate(this);
  dBtnpOffset -> Associate(this);
  dBtnpSlope -> Associate(this);
  dBtnPackage -> Associate(this);
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
  dMenuPlot1->AddEntry("&Semi-Log", TF_PLOT_LOG);

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Scale", dMenuPlot1, dMenuBarItemLayout);
  dMenuBar->AddPopup("&Direction", dMenuPlot2, dMenuBarItemLayout);

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
//std::cout << "Opened" << std::endl;
  if (!cont) return;

  if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont) {
    TObject* obj = dROOTCont->ReadData("event_tree"); //for run 710, use "tree"  8/5/10

    // Note: fRootTrees[0] = hit_tree, fRootTrees[1] = event_tree

    if (obj) {
       if (obj->InheritsFrom("TTree")) {
         TTree* tree = (TTree*) obj->Clone();

         if (tree -> FindBranch("events")) //else: couldn't find events branch
           {

            Int_t entries = tree->GetEntries();
//	    std::cout<<"Entries in tree: "<< entries << std::endl;

            //          QwHitRootContainer* roothitlist = 0;
            QwEvent* event = 0;

//          tree->SetBranchAddress("hits", &roothitlist);
           tree->SetBranchAddress("events", &event); 

/////// Create histos for TreeLines ////////////
            TH1D* chi_TLr2 = 	new TH1D("Chi2 (TLr2)","TreeLines Chi2 (Region 2)",100,0.0,40.0);
            TH1D* chi_TLr2x = 	new TH1D("Chi2 (TLr2x)","TreeLines Chi2 (Region 2, X Planes)",100,0.0,40.0);
            TH1D* chi_TLr2u = 	new TH1D("Chi2 (TLr2u)", "TreeLines Chi2 (Region 2, U Planes)",100,0.0,40.0);
            TH1D* chi_TLr2v = 	new TH1D("Chi2 (TLr2v)","TreeLines Chi2 (Region 2, V Planes)",100,0.0,40.0);
            TH1D* chi_TLr3_0 = 		new TH1D("Chi2 (TLr3_0)","TreeLines Chi2 (Region 3, fPlane=0)",100,0.0,40.0);
            TH1D* chi_TLr3u_0 = 	new TH1D("Chi2 (TLr3u_0)","TreeLines Chi2 (Region 3U, fPlane=0)",100,0.0,40.0);
            TH1D* chi_TLr3v_0 = 	new TH1D("Chi2 (TLr3v_0)","TreeLines Chi2 (Region 3V, fPlane=0)",100,0.0,40.0);
            TH1D* chi_TLr3less0 = 	new TH1D("Chi2 (TLr3less0)","TreeLines Chi2 (Region 3, fPlane>0)",100,0.0,40.0);
            TH1D* chi_TLr3uless0 = 	new TH1D("Chi2 (TLr3uless0)","TreeLines Chi2 (Region 3U, fPlane>0)",100,0.0,40.0);
            TH1D* chi_TLr3vless0 = 	new TH1D("Chi2 (TLr3vless0)","TreeLines Chi2 (Region 3V, fPlane>0)",100,0.0,40.0);

            TH1D* resid_TLr2 = 	new TH1D("Residual (TLr2)","TreeLines Residual (Region 2)",100,0.0,2.0);
            TH1D* resid_TLr2x = new TH1D("Residual (TLr2x)","TreeLines Residual (Region 2, X Planes)",100,0.0,2.0);
            TH1D* resid_TLr2u = new TH1D("Residual (TLr2u)","TreeLines Residual (Region 2, U Planes)",100,0.0,2.0);
            TH1D* resid_TLr2v = new TH1D("Residual (TLr2v)","TreeLines Residual (Region 2, V Planes)",100,0.0,2.0);
            TH1D* resid_TLr3_0 = 	new TH1D("Residual (TLr3_0)","TreeLines Residual (Region 3, fPlane=0)",100,0.0,2.0);
            TH1D* resid_TLr3u_0 = 	new TH1D("Residual (TLr3u_0)","TreeLines Residual (Region 3U, fPlane=0)",100,0.0,2.0);
            TH1D* resid_TLr3v_0 = 	new TH1D("Residual (TLr3v_0)","TreeLines Residual (Region 3V, fPlane=0)",100,0.0,2.0);
            TH1D* resid_TLr3less0 = 	new TH1D("Residual (TLr3less0)","TreeLines Residual (Region 3, fPlane>0)",100,0.0,2.0);
            TH1D* resid_TLr3uless0 = 	new TH1D("Residual (TLr3uless0)","TreeLines Residual (Region 3U, fPlane=0)",100,0.0,2.0);
            TH1D* resid_TLr3vless0 = 	new TH1D("Residual (TLr3vless0)","TreeLines Residual (Region 3V, fPlane>0)",100,0.0,2.0);

            TH1D* offset_TLr2 = new TH1D("Offset (TLr2)","TreeLines Offset (Region 2)",100,-10.0,90.0);
            TH1D* offset_TLr2x= new TH1D("Offset (TLr2x)","TreeLines Offset (Region 2, X Planes)",100,-10.0,90.0);
            TH1D* offset_TLr2u= new TH1D("Offset (TLr2u)","TreeLines Offset (Region 2, U Planes)",100,-10.0,90.0);
            TH1D* offset_TLr2v= new TH1D("Offset (TLr2v)","TreeLines Offset (Region 2, V Planes)",100,-10.0,90.0);
            TH1D* offset_TLr3_0 =	new TH1D("Offset (TLr3_0)","TreeLines Offset (Region 3, fPlane=0)",100,-150.0,60.0);
            TH1D* offset_TLr3u_0= 	new TH1D("Offset (TLr3u_0)","TreeLines Offset (Region 3U, fPlane=0)",100,-150.0,60.0);
            TH1D* offset_TLr3v_0= 	new TH1D("Offset (TLr3v_0)","TreeLines Offset (Region 3V, fPlane=0)",100,-150.0,60.0);
            TH1D* offset_TLr3less0 =	new TH1D("Offset (TLr3less0)","TreeLines Offset (Region 3, fPlane>0)",100,-150.0,60.0);
            TH1D* offset_TLr3uless0= 	new TH1D("Offset (TLr3uless0)","TreeLines Offset (Region 3U, fPlane>0)",100,-150.0,60.0);
            TH1D* offset_TLr3vless0= 	new TH1D("Offset (TLr3vless0)","TreeLines Offset (Region 3V fPlane>0)",100,-150.0,60.0);

            TH1D* slope_TLr2 = 	new TH1D("Slope (TLr2)","TreeLines Slope (Region 2)",100,-0.04,0.19);
            TH1D* slope_TLr2x = new TH1D("Slope (TLr2x)","TreeLines Slope (Region 2, X Planes)",100,-0.04,0.19);
            TH1D* slope_TLr2u = new TH1D("Slope (TLr2u)","TreeLines Slope (Region 2, U Planes)",100,-0.04,0.19);
            TH1D* slope_TLr2v = new TH1D("Slope (TLr2v)","TreeLines Slope (Region 2, V Planes)",100,-0.04,0.19);
            TH1D* slope_TLr3_0 = 	new TH1D("Slope (TLr3_0)","TreeLines Slope (Region 3, fPlane=0)",100,0.4,1.5);
            TH1D* slope_TLr3u_0 = 	new TH1D("Slope (TLr3u_0)","TreeLines Slope (Region 3U, fPlane=0)",100,0.4,1.5);
            TH1D* slope_TLr3v_0 = 	new TH1D("Slope (TLr3v_0)","TreeLines Slope (Region 3V, fPlane=0)",100,0.4,1.5);
            TH1D* slope_TLr3less0 = 	new TH1D("Slope (TLr3less0)","TreeLines Slope (Region 3, fPlane>0)",100,0.4,1.5);
            TH1D* slope_TLr3uless0 = 	new TH1D("Slope (TLr3uless0)","TreeLines Slope (Region 3U, fPlane>0)",100,0.4,1.5);
            TH1D* slope_TLr3vless0 = 	new TH1D("Slope (TLr3vless0)","TreeLines Slope (Region 3V, fPlane>0)",100,0.4,1.5);

/////// Create histos for PartialTracks //////////// ranges not set yet
            TH1D* chi_PTr2 =	new TH1D("Chi2 (PTr2)","PartialTracks Chi2 (Region 2)",100,0.0,2100.0);
            TH1D* chi_PTr3 = 	new TH1D("Chi2 (PTr3)","PartialTracks Chi2 (Region 3)",100,0.0,2100.0);
            TH1D* resid_PTr2 = 	new TH1D("Residual (PTr2)","PartialTracks Residual (Region 2)",100,0.0,0.08);
            TH1D* resid_PTr3 = 	new TH1D("Residual (PTr3)","PartialTracks Residual (Region 3)",100,0.0,0.08);
            TH1D* offsetX_PTr2= new TH1D("OffsetX (PTr2)","PartialTracks OffsetX (Region 2)",100,-100.0,100.0);
            TH1D* offsetX_PTr3= new TH1D("OffsetX (PTr3)","PartialTracks OffsetX (Region 3)",100,-250000.0,250000.0);
            TH1D* offsetY_PTr2= new TH1D("OffsetY (PTr2)","PartialTracks OffsetY (Region 2)",100,-100.0,100.0);
            TH1D* offsetY_PTr3= new TH1D("OffsetY (PTr3)","PartialTracks OffsetY (Region 3)",100,-10000000.0,2000000.0);
            TH1D* slopeX_PTr2 = new TH1D("SlopeX (PTr2)","PartialTracks SlopeX (Region 2)",100,-0.3,0.3);
            TH1D* slopeX_PTr3 = new TH1D("SlopeX (PTr3)","PartialTracks SlopeX (Region 3)",100,-600.0,500.0);
            TH1D* slopeY_PTr2 = new TH1D("SlopeY (PTr2)","PartialTracks SlopeY (Region 2)",100,-0.3,0.3);
            TH1D* slopeY_PTr3 = new TH1D("SlopeY (PTr3)","PartialTracks SlopeY (Region 3)",100,-5000.0,25000.0);
	    TH1D* package_PT =	new TH1D("Packages (R2R3)", "Region2/Region3 Package Value Comparisons",15, 10.0, 25.0);

////////// Color and format histos ////////////////////
	    chi_TLr2	->	SetFillColor(kBlue);
	    chi_TLr2x	->	SetFillColor(kBlue);
	    chi_TLr2u	->	SetFillColor(kBlue);
	    chi_TLr2v	->	SetFillColor(kBlue);
	    chi_TLr3_0	->	SetFillColor(kGreen);
	    chi_TLr3u_0	->	SetFillColor(kGreen);
	    chi_TLr3v_0	->	SetFillColor(kGreen);
	    chi_TLr3less0->	SetFillColor(kGreen);
	    chi_TLr3uless0->	SetFillColor(kGreen);
	    chi_TLr3vless0->	SetFillColor(kGreen);
	    resid_TLr2	->	SetFillColor(kBlue);
	    resid_TLr2x	->	SetFillColor(kBlue);
	    resid_TLr2u	->	SetFillColor(kBlue);
	    resid_TLr2v	->	SetFillColor(kBlue);
	    resid_TLr3_0->	SetFillColor(kGreen);
	    resid_TLr3u_0->	SetFillColor(kGreen);
	    resid_TLr3v_0->	SetFillColor(kGreen);
	    resid_TLr3less0->	SetFillColor(kGreen);
	    resid_TLr3uless0->	SetFillColor(kGreen);
	    resid_TLr3vless0->	SetFillColor(kGreen);
	    offset_TLr2	->	SetFillColor(kBlue);
	    offset_TLr2x->	SetFillColor(kBlue);
	    offset_TLr2u->	SetFillColor(kBlue);
	    offset_TLr2v->	SetFillColor(kBlue);
	    offset_TLr3_0->	SetFillColor(kGreen);
	    offset_TLr3u_0->	SetFillColor(kGreen);
	    offset_TLr3v_0->	SetFillColor(kGreen);
	    offset_TLr3less0->	SetFillColor(kGreen);
	    offset_TLr3uless0->	SetFillColor(kGreen);
	    offset_TLr3vless0->	SetFillColor(kGreen);
	    slope_TLr2	->	SetFillColor(kBlue);
	    slope_TLr2x	->	SetFillColor(kBlue);
	    slope_TLr2u	->	SetFillColor(kBlue);
	    slope_TLr2v	->	SetFillColor(kBlue);
	    slope_TLr3_0->	SetFillColor(kGreen);
	    slope_TLr3u_0->	SetFillColor(kGreen);
	    slope_TLr3v_0->	SetFillColor(kGreen);
	    slope_TLr3less0->	SetFillColor(kGreen);
	    slope_TLr3uless0->	SetFillColor(kGreen);
	    slope_TLr3vless0->	SetFillColor(kGreen);

	    chi_PTr2	->	SetFillColor(kBlue);
	    chi_PTr3	->	SetFillColor(kGreen);
	    resid_PTr2	->	SetFillColor(kBlue);
	    resid_PTr3	->	SetFillColor(kGreen);
            offsetX_PTr2->	SetFillColor(kBlue);
            offsetX_PTr3->	SetFillColor(kGreen);
            offsetY_PTr2->	SetFillColor(kBlue);
            offsetY_PTr3->	SetFillColor(kGreen);
            slopeX_PTr2->	SetFillColor(kBlue);
            slopeX_PTr3->	SetFillColor(kGreen);
            slopeY_PTr2->	SetFillColor(kBlue);
            slopeY_PTr3->	SetFillColor(kGreen);
	    package_PT	->	SetFillColor(kBlue);

// Fill Histograms
	    std::cout<<"Filling Track Finding Histos..."<< std::endl;

            for(int j = 0; j < tree->GetEntries(); j++){
// 	    	std::cout<<"Getting entry "<< j<< std::endl;
//		if (tree->GetEntry(j)==NULL ){
			tree->GetEntry(j);
//		}	
// 	    	std::cout<<"Got Entry "<< j<< std::endl;
//               event->PrintPartialTracks();

		  Int_t nTreeLines = event->GetNumberOfTreeLines();
		  for (int iTreeLine = 0; iTreeLine < nTreeLines; iTreeLine++) {
			const QwTreeLine* treeline = event->GetTreeLine(iTreeLine);
                	if (treeline->GetRegion() == kRegionID2) {
//				std::cout << "Region 2 availiable" << std::endl;
                  		chi_TLr2->	Fill(treeline->GetChi());
                  		resid_TLr2->	Fill(treeline->GetAverageResidual());
                  		offset_TLr2->	Fill(treeline->GetOffset());
                  		slope_TLr2->	Fill(treeline->GetSlope());
//				std::cout << "Filled w/ chi" << std::endl;
				if (treeline->GetDirection()==1){
					chi_TLr2x->	Fill(treeline->GetChi());
					resid_TLr2x->	Fill(treeline->GetAverageResidual());
					offset_TLr2x->	Fill(treeline->GetOffset());
					slope_TLr2x->	Fill(treeline->GetSlope());
//					std::cout << "Direction 2x availiable" << std::endl;
				}
				if (treeline->GetDirection()==3){
					chi_TLr2u->	Fill(treeline->GetChi());
					resid_TLr2u->	Fill(treeline->GetAverageResidual());
					offset_TLr2u->	Fill(treeline->GetOffset());
					slope_TLr2u->	Fill(treeline->GetSlope());
//					std::cout << "Direction 2u availiable" << std::endl;
				}
				if (treeline->GetDirection()==4){
					chi_TLr2v->	Fill(treeline->GetChi());
					resid_TLr2v->	Fill(treeline->GetAverageResidual());
					offset_TLr2v->	Fill(treeline->GetOffset());
					slope_TLr2v->	Fill(treeline->GetSlope());
//					std::cout << "Direction 2v availiable" << std::endl;
				}
                	}
                	if (treeline->GetRegion() == kRegionID3) {
//				std::cout << "Region 3 availiable" << std::endl;
				if (treeline->GetPlane()==0){
                  			chi_TLr3_0->	Fill(treeline->GetChi());
                  			resid_TLr3_0->	Fill(treeline->GetAverageResidual());
                  			offset_TLr3_0->	Fill(treeline->GetOffset());
                  			slope_TLr3_0->	Fill(treeline->GetSlope());
				}
				if (treeline->GetPlane()>0){
					chi_TLr3less0->		Fill(treeline->GetChi());
					resid_TLr3less0->	Fill(treeline->GetAverageResidual());
					offset_TLr3less0->	Fill(treeline->GetOffset());
					slope_TLr3less0->	Fill(treeline->GetSlope());
				}
				if (treeline->GetDirection()==3){
					if (treeline->GetPlane()==0){
						chi_TLr3u_0->	Fill(treeline->GetChi());
						resid_TLr3u_0->	Fill(treeline->GetAverageResidual());
						offset_TLr3u_0->	Fill(treeline->GetOffset());
						slope_TLr3u_0->	Fill(treeline->GetSlope());
					}
					if (treeline->GetPlane()>0){
						chi_TLr3uless0->	Fill(treeline->GetChi());
						resid_TLr3uless0->	Fill(treeline->GetAverageResidual());
						offset_TLr3uless0->	Fill(treeline->GetOffset());
						slope_TLr3uless0->	Fill(treeline->GetSlope());
					}
//					std::cout << "Direction 3u availiable" << std::endl;
				}
				if (treeline->GetDirection()==4){
					if (treeline->GetPlane()==0){
						chi_TLr3v_0->		Fill(treeline->GetChi());
						resid_TLr3v_0->		Fill(treeline->GetAverageResidual());
						offset_TLr3v_0->	Fill(treeline->GetOffset());
						slope_TLr3v_0->		Fill(treeline->GetSlope());
					}
					if (treeline->GetPlane()>0){
						chi_TLr3vless0->	Fill(treeline->GetChi());
						resid_TLr3vless0->	Fill(treeline->GetAverageResidual());
						offset_TLr3vless0->	Fill(treeline->GetOffset());
						slope_TLr3vless0->	Fill(treeline->GetSlope());
						}
//					std::cout << "Direction 3v availiable" << std::endl;
				}
                	}
		  }

// //Fill partial tracks 
		  Int_t nPartialTracks = event->GetNumberOfPartialTracks();
		  int hasPackage=0;		// initize counter for packages 
						// package number (1 or 2) for Region 2 will be denoted in the tens position
						// package number (1 or 2) for Region 3 will be denoted in the ones position
						// 0 denotes that the corresponding region is not avaible for entry j
						// eg: hasPackage=21 signifies the package value 2 for Region 2
						//					     and 1 for Region 3
		  for (int iPartialTrack = 0; iPartialTrack < nPartialTracks; iPartialTrack++) {
			const QwPartialTrack* partialtrack = event->GetPartialTrack(iPartialTrack);
			switch (partialtrack->GetRegion()){
		    	    case kRegionIDNull:
				break;
		    	    case kRegionID1:
				break;
		    	    case kRegionID2:
//				std::cout << "Partial Region 2 availiable" << std::endl;
                  		chi_PTr2->Fill(partialtrack->GetChiWeight());
                  		resid_PTr2->Fill(partialtrack->GetAverageResidual());
                  		offsetX_PTr2->Fill(partialtrack->fOffsetX);
                  		offsetY_PTr2->Fill(partialtrack->fOffsetY);
                  		slopeX_PTr2->Fill(partialtrack->fSlopeX);
                  		slopeY_PTr2->Fill(partialtrack->fSlopeY);
				//Check the package of Region 2 and modify hasPackage accordingly
			   	switch (partialtrack->GetPackage()){
				  case kPackageNull:
				    break;
				  case 1:
//				    std::cout << "old hasPackage= "<<hasPackage << std::endl;
				    hasPackage= hasPackage%10 +10; //changes tens value to 1 while keeping ones place value
//				    std::cout << "new hasPackage= "<<hasPackage << std::endl;
				    break;
				  case 2:
//				    std::cout << "old hasPackage= "<<hasPackage << std::endl;
				    hasPackage= hasPackage%10 +20; //changes tens value to 2 while keeping ones place value
//				    std::cout << "new hasPackage= "<<hasPackage << std::endl;
				    break;

			   	}
		  		//std::cout << "hasRegion: "<< hasRegion << std::endl;
                		break;
		    	    case kRegionID3:
//				std::cout << "Partial Region 3 availiable" << std::endl;
                  		chi_PTr3->Fill(partialtrack->GetChiWeight());
                  		resid_PTr3->Fill(partialtrack->GetAverageResidual());
                  		offsetX_PTr3->Fill(partialtrack->fOffsetX);
                  		offsetY_PTr3->Fill(partialtrack->fOffsetY);
                  		slopeX_PTr3->Fill(partialtrack->fSlopeX);
                  		slopeY_PTr3->Fill(partialtrack->fSlopeY);
				//Check package for Region 3 and modify hasPackage accordingly
			   	switch (partialtrack->GetPackage()){
				  case kPackageNull:
				    break;
				  case 1:
//				    std::cout << "old hasPackage= "<<hasPackage << std::endl;
				    hasPackage= hasPackage-hasPackage%10 +1; //changes ones value to 1 while keeping tens place value
//				    std::cout << "new hasPackage= "<<hasPackage << std::endl;
				    break;
				  case 2:
//				    std::cout << "old hasPackage= "<<hasPackage << std::endl;
				    hasPackage= hasPackage-hasPackage%10 +2; //changes ones value to 2 while keeping tens place value
				    break;
//				    std::cout << "new hasPackage= "<<hasPackage << std::endl;
			   	}
		  		//std::cout << "hasRegion: "<< hasRegion << std::endl;
				break;
		    	    case kRegionIDTrig:
				break;
		    	    case kRegionIDCer:
				break;
		    	    case kRegionIDScanner:
				break;
			}
// 			if (hasPackage== 11){
//                   		package_PT->Fill(hasPackage);
// 			}
			switch (hasPackage){ //if an entry has both Region 2 and Region 3 data. Plot the package combination.
				case 1:
				  break;
				case 2:
				  break;
				case 10:
				  break;
				case 20:
                  		  break;
				case 11:
                  		  package_PT->Fill(hasPackage);
                  		  break;
				case 12:
                  		  package_PT->Fill(hasPackage);
                  		  break;
				case 21:
                  		  package_PT->Fill(hasPackage);
                  		  break;
				case 22:
                  		  package_PT->Fill(hasPackage);
                  		  break;
			}
		  }



//               QwHitContainer* hitlist = roothitlist->Convert();
//               for (QwHitContainer::iterator hit = hitlist->begin();
//                    hit != hitlist->end(); hit++) {
// 
//                 if (hit->GetRegion() == kRegionID3) {
//                   hst->Fill(hit->GetDriftDistance());
// 
//                 }
//               }
	    }

// // Defining histogram
//             TH1D* hst = new TH1D("distance","distance",100,0.0,3.0);
//             hst->SetDirectory(0);
// 
//             hst->GetXaxis()->SetTitle("distance");
//             hst->GetXaxis()->CenterTitle();
//             hst->GetXaxis()->SetTitleSize(0.04);
//             hst->GetXaxis()->SetLabelSize(0.04);
//             hst->GetXaxis()->SetTitleOffset(1.25);
//             hst->GetYaxis()->SetLabelSize(0.04);
// 		
//             for(int j = 0; j < tree->GetEntries(); j++){
//               tree->GetEntry(j);
//               QwHitContainer* hitlist = roothitlist->Convert();
//// 	    std::cout<<"hitlist made "<< j << std::endl;
//               for (QwHitContainer::iterator hit = hitlist->begin();
//                    hit != hitlist->end(); hit++) {
// 
//                 if (hit->GetRegion() == kRegionID3) {
//                   hst->Fill(hit->GetDriftDistance());
// 
//                 }
//               }
// //               for (QwEvent::iterator eventStep=event->begin(); eventStep!=event->end(); eventStep++) {
// // 			
// //                 if (eventStep->GetRegion() == kRegionID3) {
// //                   hst->Fill(eventStep->GetChi());
// //                 }
// //               }
// 
//             }
///////////defining histo end//////////////////////

///////////// Creating histos from Draw()  ////////////////////
// // Region 2 in blue and Region 3 in green
// // 
// // Histogram from fQwTreeLines
// // TreeLines Chi2
// // TL Chi2 R2
// // 	Note: Draw command creates a canvas when a ROOT file is opened. 7/20/10 -started happening after updating gui revision
//  	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2", "events.fQwTreeLines.fRegion==2"); //(100,0.0,60.0)
//  	    TH1F *fChiR2 = (TH1F*)gDirectory->Get("fChiR2");
// 	    fChiR2->SetFillColor(kBlue);
//             fChiR2->SetTitle("TreeLines Chi2 (Region 2)");
// //TL Chi2 R2 X,U,V Planes
// 	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2X", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==1"); //(100,0.0,60.0)
// 	    TH1F *fChiR2X = (TH1F*)gDirectory->Get("fChiR2X");
// 	    fChiR2X->SetFillColor(kBlue);
//             fChiR2X->SetTitle("TreeLines Chi2 (Region 2, X Plane)");
// 	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2U", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==3"); //(100,0.0,60.0)
// 	    TH1F *fChiR2U = (TH1F*)gDirectory->Get("fChiR2U");
// 	    fChiR2U->SetFillColor(kBlue);
//             fChiR2U->SetTitle("TreeLines Chi2 (Region 2, U Plane)");
// 	    tree->Draw("events.fQwTreeLines.fChi>>fChiR2V", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==4"); //(100,0.0,60.0)
// 	    TH1F *fChiR2V = (TH1F*)gDirectory->Get("fChiR2V");
// 	    fChiR2V->SetFillColor(kBlue);
//             fChiR2V->SetTitle("TreeLines Chi2 (Region 2, V Plane)");
// //TL Chi2 R3
// 	    tree->Draw("events.fQwTreeLines.fChi>>fChiR3", "events.fQwTreeLines.fRegion==3");
// 	    TH1F *fChiR3 = (TH1F*)gDirectory->Get("fChiR3");
// 	    fChiR3->SetFillColor(kGreen);
//             fChiR3->SetTitle("TreeLines Chi2 (Region 3)");
// //TL Chi2 R3 U,V
// 	    tree->Draw("events.fQwTreeLines.fChi>>fChiR3U", "events.fQwTreeLines.fRegion==3&& (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
// 	    TH1F *fChiR3U = (TH1F*)gDirectory->Get("fChiR3U");
// 	    fChiR3U->SetFillColor(kGreen);
//             fChiR3U->SetTitle("TreeLines Chi2 (Region 3, U Plane)");
// 	    tree->Draw("events.fQwTreeLines.fChi>>fChiR3V", "events.fQwTreeLines.fRegion==3&& (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
// 	    TH1F *fChiR3V = (TH1F*)gDirectory->Get("fChiR3V");
// 	    fChiR3V->SetFillColor(kGreen);
//             fChiR3V->SetTitle("TreeLines Chi2 (Region 3, V Plane)");
// 
// //TreeLines Residuals
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>Residuals");
// 	    TH1F *Residuals = (TH1F*)gDirectory->Get("Residuals");
// //TL Residuals R2
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR2", "events.fQwTreeLines.fRegion==2");
// 	    TH1F *ResidualsR2 = (TH1F*)gDirectory->Get("ResidualsR2");
// 	    ResidualsR2->SetFillColor(kBlue);
//             ResidualsR2->SetTitle("TreeLines Residuals (Region 2)");
// //TL Residuals R2 X,U,V Planes
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR2X", "events.fQwTreeLines.fRegion==2&& events.fQwTreeLines.fDirection==1");
// 	    TH1F *ResidualsR2X = (TH1F*)gDirectory->Get("ResidualsR2X");
// 	    ResidualsR2X->SetFillColor(kBlue);
//             ResidualsR2X->SetTitle("TreeLines Residuals (Region 2, X Plane)");
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR2U", "events.fQwTreeLines.fRegion==2&& events.fQwTreeLines.fDirection==3");
// 	    TH1F *ResidualsR2U = (TH1F*)gDirectory->Get("ResidualsR2U");
// 	    ResidualsR2U->SetFillColor(kBlue);
//             ResidualsR2U->SetTitle("TreeLines Residuals (Region 2, U Plane)");
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR2V", "events.fQwTreeLines.fRegion==2&& events.fQwTreeLines.fDirection==4");
// 	    TH1F *ResidualsR2V = (TH1F*)gDirectory->Get("ResidualsR2V");
// 	    ResidualsR2V->SetFillColor(kBlue);
//             ResidualsR2V->SetTitle("TreeLines Residuals (Region 2, V Plane)");
// 
// //TL Residuals R3
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR3", "events.fQwTreeLines.fRegion==3");
// 	    TH1F *ResidualsR3 = (TH1F*)gDirectory->Get("ResidualsR3");
// 	    ResidualsR3->SetFillColor(kGreen);
//             ResidualsR3->SetTitle("TreeLines Residuals (Region 3)");
// //TL Residuals R3 U, V Planes
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR3U", "events.fQwTreeLines.fRegion==3&& (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
// 	    TH1F *ResidualsR3U = (TH1F*)gDirectory->Get("ResidualsR3U");
// 	    ResidualsR3U->SetFillColor(kGreen);
//             ResidualsR3U->SetTitle("TreeLines Residuals (Region 3, U Plane)");
// 	    tree->Draw("events.fQwTreeLines.fAverageResidual>>ResidualsR3V", "events.fQwTreeLines.fRegion==3&& (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
// 	    TH1F *ResidualsR3V = (TH1F*)gDirectory->Get("ResidualsR3V");
// 	    ResidualsR3V->SetFillColor(kGreen);
//             ResidualsR3V->SetTitle("TreeLines Residuals (Region 3, V Plane)");
// 
// //TreeLines Offsets
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offset"); //(100.,-170.,110.)
// 	    TH1F *offset = (TH1F*)gDirectory->Get("offset");
// //TL Offsets R2
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2","events.fQwTreeLines.fRegion==2");
// 	    TH1F *offsetR2 = (TH1F*)gDirectory->Get("offsetR2");
// 	    offsetR2->SetFillColor(kBlue);
//             offsetR2->SetTitle("TreeLines Offset (Region 2)");
// //TL Offsets R2 X,U,V Planes
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2X","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==1");
// 	    TH1F *offsetR2X = (TH1F*)gDirectory->Get("offsetR2X");
// 	    offsetR2X->SetFillColor(kBlue);
//             offsetR2X->SetTitle("TreeLines Offset (Region 2, X Plane)");
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2U","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==3");
// 	    TH1F *offsetR2U = (TH1F*)gDirectory->Get("offsetR2U");
// 	    offsetR2U->SetFillColor(kBlue);
//             offsetR2U->SetTitle("TreeLines Offset (Region 2, U Plane)");
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR2V","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==4");
// 	    TH1F *offsetR2V = (TH1F*)gDirectory->Get("offsetR2V");
// 	    offsetR2V->SetFillColor(kBlue);
//             offsetR2V->SetTitle("TreeLines Offset (Region 2, V Plane)");
// //TL Offsets R3
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR3","events.fQwTreeLines.fRegion==3");
// 	    TH1F *offsetR3 = (TH1F*)gDirectory->Get("offsetR3");
// 	    offsetR3->SetFillColor(kGreen);
//             offsetR3->SetTitle("TreeLines Offset (Region 3)");
// //TL Offsets R3 U,V Planes
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR3U","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
// 	    TH1F *offsetR3U = (TH1F*)gDirectory->Get("offsetR3U");
// 	    offsetR3U->SetFillColor(kGreen);
//             offsetR3U->SetTitle("TreeLines Offset (Region 3, U Plane)");
// 	    tree->Draw("events.fQwTreeLines.fOffset>>offsetR3V","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
// 	    TH1F *offsetR3V = (TH1F*)gDirectory->Get("offsetR3V");
// 	    offsetR3V->SetFillColor(kGreen);
//             offsetR3V->SetTitle("TreeLines Offset (Region 3, V Plane)");
// 
// //TreeLines Slope
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slope"); //(100,-0.2,1.6)
// 	    TH1F *slope = (TH1F*)gDirectory->Get("slope");
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2","events.fQwTreeLines.fRegion==2");
// 	    TH1F *slopeR2 = (TH1F*)gDirectory->Get("slopeR2");
// 	    slopeR2->SetFillColor(kBlue);
//             slopeR2->SetTitle("TreeLines Slope (Region 2)");
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2X","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==1");
// 	    TH1F *slopeR2X = (TH1F*)gDirectory->Get("slopeR2X");
// 	    slopeR2X->SetFillColor(kBlue);
//             slopeR2X->SetTitle("TreeLines Slope (Region 2, X Planes)");
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2U","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==3");
// 	    TH1F *slopeR2U = (TH1F*)gDirectory->Get("slopeR2U");
// 	    slopeR2U->SetFillColor(kBlue);
//             slopeR2U->SetTitle("TreeLines Slope (Region 2, U Planes)");
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR2V","events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fDirection==4");
// 	    TH1F *slopeR2V = (TH1F*)gDirectory->Get("slopeR2V");
// 	    slopeR2V->SetFillColor(kBlue);
//             slopeR2V->SetTitle("TreeLines Slope (Region 2, V Planes)");
// 
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR3","events.fQwTreeLines.fRegion==3");
// 	    TH1F *slopeR3 = (TH1F*)gDirectory->Get("slopeR3");
// 	    slopeR3->SetFillColor(kGreen);
//             slopeR3->SetTitle("TreeLines Slope (Region 3)");
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR3U","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==1 || events.fQwTreeLines.fPlane==3)");
// 	    TH1F *slopeR3U= (TH1F*)gDirectory->Get("slopeR3U");
// 	    slopeR3U->SetFillColor(kGreen);
//             slopeR3U->SetTitle("TreeLines Slope (Region 3, U Planes)");
// 	    tree->Draw("events.fQwTreeLines.fSlope>>slopeR3V","events.fQwTreeLines.fRegion==3 && (events.fQwTreeLines.fPlane==2 || events.fQwTreeLines.fPlane==4)");
// 	    TH1F *slopeR3V= (TH1F*)gDirectory->Get("slopeR3V");
// 	    slopeR3V->SetFillColor(kGreen);
//             slopeR3V->SetTitle("TreeLines Slope (Region 3, V Planes)");
// 
// //Historams from tQwPartialTracks
// //PartialTracks Chi2
// 	    tree->Draw("events.fQwPartialTracks.fChi>>fpChiR2", "events.fQwPartialTracks.fRegion==2 "); //  (100,0.0,2000.0)
// 	    TH1F *fpChiR2 = (TH1F*)gDirectory->Get("fpChiR2");
// 	    fpChiR2->SetFillColor(kBlue);
//             fpChiR2->SetTitle("PartialTracks Chi2 (Region 2)");
// 	    tree->Draw("events.fQwPartialTracks.fChi>>fpChiR3", "events.fQwPartialTracks.fRegion==3");
// 	    TH1F *fpChiR3 = (TH1F*)gDirectory->Get("fpChiR3");
// 	    fpChiR3->SetFillColor(kGreen);
//             fpChiR3->SetTitle("PartialTracks Chi2 (Region 3)");
// //PartialTracks Residuals
// 	    tree->Draw("events.fQwPartialTracks.fAverageResidual>>pResiduals");
// 	    TH1F *pResiduals = (TH1F*)gDirectory->Get("pResiduals");
// 	    tree->Draw("events.fQwPartialTracks.fAverageResidual>>pResidualsR2", "events.fQwPartialTracks.fRegion==2");
// 	    TH1F *pResidualsR2 = (TH1F*)gDirectory->Get("pResidualsR2");
// 	    pResidualsR2->SetFillColor(kBlue);
//             pResidualsR2->SetTitle("PartialTracks Residuals (Region 2)");
// 	    tree->Draw("events.fQwPartialTracks.fAverageResidual>>pResidualsR3", "events.fQwPartialTracks.fRegion==3");
// 	    TH1F *pResidualsR3 = (TH1F*)gDirectory->Get("pResidualsR3");
// 	    pResidualsR3->SetFillColor(kGreen);
//             pResidualsR3->SetTitle("PartialTracks Residuals (Region 3)");
// 
// //PartialTracks Offset X
// // 	    tree->Draw("events.fQwPartialTracks.fOffsetX>>poffsetX"); //(100.,-100000.,160000.)
// // 	    TH1F *poffsetX = (TH1F*)gDirectory->Get("poffsetX");
// 	    tree->Draw("events.fQwPartialTracks.fOffsetX>>poffsetXR2","events.fQwPartialTracks.fRegion==2");
// 	    TH1F *poffsetXR2 = (TH1F*)gDirectory->Get("poffsetXR2");
// 	    poffsetXR2->SetFillColor(kBlue);
//             poffsetXR2->SetTitle("PartialTracks X Offset (Region 2)");
// 	    tree->Draw("events.fQwPartialTracks.fOffsetX>>poffsetXR3","events.fQwPartialTracks.fRegion==3");
// 	    TH1F *poffsetXR3 = (TH1F*)gDirectory->Get("poffsetXR3");
// 	    poffsetXR3->SetFillColor(kGreen);
//             poffsetXR3->SetTitle("PartialTracks X Offset (Region 3)");
// //PartialTracks Offset Y
// // 	    tree->Draw("events.fQwPartialTracks.fOffsetY>>poffsetY"); //(100.,-40000.,90000.)
// // 	    TH1F *poffsetY = (TH1F*)gDirectory->Get("poffsetY");
// 	    tree->Draw("events.fQwPartialTracks.fOffsetY>>poffsetYR2","events.fQwPartialTracks.fRegion==2");
// 	    TH1F *poffsetYR2 = (TH1F*)gDirectory->Get("poffsetYR2");
// 	    poffsetYR2->SetFillColor(kBlue-4);
//             poffsetYR2->SetTitle("PartialTracks Y Offset (Region 2)");
// 	    tree->Draw("events.fQwPartialTracks.fOffsetY>>poffsetYR3","events.fQwPartialTracks.fRegion==3");
// 	    TH1F *poffsetYR3 = (TH1F*)gDirectory->Get("poffsetYR3");
// 	    poffsetYR3->SetFillColor(kGreen-4);
//             poffsetYR3->SetTitle("PartialTracks Y Offset (Region 3)");
// //PartialTracks Slope X
// // 	    tree->Draw("events.fQwPartialTracks.fSlopeX>>pSlopeX"); //(100.,-350.,250.)
// // 	    TH1F *pSlopeX = (TH1F*)gDirectory->Get("pSlopeX");
// 	    tree->Draw("events.fQwPartialTracks.fSlopeX>>pSlopeXR2","events.fQwPartialTracks.fRegion==2");
// 	    TH1F *pSlopeXR2 = (TH1F*)gDirectory->Get("pSlopeXR2");
// 	    pSlopeXR2->SetFillColor(kBlue);
//             pSlopeXR2->SetTitle("PartialTracks X Slope (Region 2)");
// 	    tree->Draw("events.fQwPartialTracks.fSlopeX>>pSlopeXR3","events.fQwPartialTracks.fRegion==3");
// 	    TH1F *pSlopeXR3 = (TH1F*)gDirectory->Get("pSlopeXR3");
// 	    pSlopeXR3->SetFillColor(kGreen);
//             pSlopeXR3->SetTitle("PartialTracks X Slope (Region 3)");
// //PartialTracks Slope Y
// // 	    tree->Draw("events.fQwPartialTracks.fSlopeY>>pSlopeY"); //(100.,-200.,100.)
// // 	    TH1F *pSlopeY = (TH1F*)gDirectory->Get("pSlopeY");
// 	    tree->Draw("events.fQwPartialTracks.fSlopeY>>pSlopeYR2","events.fQwPartialTracks.fRegion==2");
// 	    TH1F *pSlopeYR2 = (TH1F*)gDirectory->Get("pSlopeYR2");
// 	    pSlopeYR2->SetFillColor(kBlue-4);
//             pSlopeYR2->SetTitle("PartialTracks Y Slope (Region 2)");
// 	    tree->Draw("events.fQwPartialTracks.fSlopeY>>pSlopeYR3","events.fQwPartialTracks.fRegion==3");
// 	    TH1F *pSlopeYR3 = (TH1F*)gDirectory->Get("pSlopeYR3");
// 	    pSlopeYR3->SetFillColor(kGreen-4);
//             pSlopeYR3->SetTitle("PartialTracks Y Slope (Region 3)");
// 
// //fQwHits
// 	    tree->Draw("events.fQwTreeLines.fQwHits.fDistance>>hdist", "events.fQwTreeLines.fRegion==2 && events.fQwTreeLines.fPlane!=0");
//             TH1F *hdist = (TH1F*)gDirectory->Get("hdist");
// 	    tree->Draw("events.fQwTreeLines.fNQwHits>>fNQwHits");
// 	    TH1F *fNQwHits = (TH1F*)gDirectory->Get("fNQwHits");
///////////// Creating histos from Draw end ////////////////////

//  Add Histogram to HistArray

//	   HistArray.Add(fChiR2);	//0
// 	   HistArray.Add(fChiR2X);	
// 	   HistArray.Add(fChiR2U);
// 	   HistArray.Add(fChiR2V);
// 	   HistArray.Add(fChiR3);	//4
// 	   HistArray.Add(fChiR3U);
// 	   HistArray.Add(fChiR3V);
//
//  	   HistArray.Add(ResidualsR2);	//7
//  	   HistArray.Add(ResidualsR2X);
//  	   HistArray.Add(ResidualsR2U);
//  	   HistArray.Add(ResidualsR2V);
//  	   HistArray.Add(ResidualsR3);	//11
//  	   HistArray.Add(ResidualsR3U);	
//  	   HistArray.Add(ResidualsR3V);
//
// 	   HistArray.Add(offsetR2);	//14
// 	   HistArray.Add(offsetR2X);	
// 	   HistArray.Add(offsetR2U);
// 	   HistArray.Add(offsetR2V);
// 	   HistArray.Add(offsetR3);	//18
// 	   HistArray.Add(offsetR3U);
// 	   HistArray.Add(offsetR3V);
//
// 	   HistArray.Add(slopeR2);	//21
// 	   HistArray.Add(slopeR2X);	
// 	   HistArray.Add(slopeR2U);
// 	   HistArray.Add(slopeR2V); 
// 	   HistArray.Add(slopeR3);	//25
// 	   HistArray.Add(slopeR3U);
// 	   HistArray.Add(slopeR3V);
//
// 	   HistArray.Add(fpChiR2);	//28
// 	   HistArray.Add(fpChiR3);
//  	   HistArray.Add(pResidualsR2);	//30
//  	   HistArray.Add(pResidualsR3);
// 	   HistArray.Add(poffsetXR2);	//32
// 	   HistArray.Add(poffsetXR3);
// 	   HistArray.Add(poffsetYR2);	//34
// 	   HistArray.Add(poffsetYR3);
// 	   HistArray.Add(pSlopeXR2);	//36
// 	   HistArray.Add(pSlopeXR3);
// 	   HistArray.Add(pSlopeYR2);	//38
// 	   HistArray.Add(pSlopeYR3);
// 
// 
//            HistArray.Add(hdist);	
// 

//////end Fill HillArray with Draw method////
// 	   HistArray.Add(hst); 		//45


//////// Add to HistArray (using histo from loop method) //////////////
//	//TreeLines
 	   HistArray.Add(chi_TLr2);		//0
 	   HistArray.Add(chi_TLr2x);
 	   HistArray.Add(chi_TLr2u);	
 	   HistArray.Add(chi_TLr2v);
	   HistArray.Add(chi_TLr3_0);		//4
 	   HistArray.Add(chi_TLr3u_0);
 	   HistArray.Add(chi_TLr3v_0);	
	   HistArray.Add(chi_TLr3less0);	//7
 	   HistArray.Add(chi_TLr3uless0);
 	   HistArray.Add(chi_TLr3vless0);
 	   HistArray.Add(resid_TLr2);		//10
 	   HistArray.Add(resid_TLr2x);
 	   HistArray.Add(resid_TLr2u);	
 	   HistArray.Add(resid_TLr2v);
 	   HistArray.Add(resid_TLr3_0);		//14
 	   HistArray.Add(resid_TLr3u_0);
 	   HistArray.Add(resid_TLr3v_0);
 	   HistArray.Add(resid_TLr3less0);	//17
 	   HistArray.Add(resid_TLr3uless0);
 	   HistArray.Add(resid_TLr3vless0);
 	   HistArray.Add(offset_TLr2);		//20
 	   HistArray.Add(offset_TLr2x);
 	   HistArray.Add(offset_TLr2u);	
 	   HistArray.Add(offset_TLr2v);
 	   HistArray.Add(offset_TLr3_0);	//24
 	   HistArray.Add(offset_TLr3u_0);
 	   HistArray.Add(offset_TLr3v_0);
 	   HistArray.Add(offset_TLr3less0);	//27
 	   HistArray.Add(offset_TLr3uless0);
 	   HistArray.Add(offset_TLr3vless0);
 	   HistArray.Add(slope_TLr2);		//30
 	   HistArray.Add(slope_TLr2x);
 	   HistArray.Add(slope_TLr2u);	
 	   HistArray.Add(slope_TLr2v);
 	   HistArray.Add(slope_TLr3_0);		//34
 	   HistArray.Add(slope_TLr3u_0);
 	   HistArray.Add(slope_TLr3v_0);
 	   HistArray.Add(slope_TLr3less0);	//37
 	   HistArray.Add(slope_TLr3uless0);
 	   HistArray.Add(slope_TLr3vless0);
	// Partial Tracks
	   HistArray.Add(chi_PTr2);	//40
	   HistArray.Add(chi_PTr3);
	   HistArray.Add(resid_PTr2);	//42
	   HistArray.Add(resid_PTr3);
	   HistArray.Add(offsetX_PTr2);	//44
	   HistArray.Add(offsetX_PTr3);
	   HistArray.Add(offsetY_PTr2);	//46
	   HistArray.Add(offsetY_PTr3);
	   HistArray.Add(slopeX_PTr2);	//48
	   HistArray.Add(slopeX_PTr3);
	   HistArray.Add(slopeY_PTr2);	//50
	   HistArray.Add(slopeY_PTr3);
 	   HistArray.Add(package_PT);	//52	
        }
	else{
	std::cout<<"could not find 'events' branch"<<std::endl;
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
	std::cout<<"no data read from dROOTCont, obj==false"<<std::endl;
    }
   }
   else{std::cout<<"no ROOT"<<std::endl;}

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
//  std::cout<<"Linear..."<<std::endl;
  TCanvas *mc = dCanvas->GetCanvas();
  mc->cd(1)->SetLogy(0);
  mc->cd(2)->SetLogy(0);
  mc->cd(3)->SetLogy(0);
  mc->cd(4)->SetLogy(0);
  gPad->Update();
}

void QwGUITrackFinding::PlotLog()
{
//  std::cout<<"Semi-Log..."<<std::endl;
  TCanvas *mc = dCanvas->GetCanvas();
  mc->cd(1)->SetLogy(1);
  mc->cd(2)->SetLogy(1);
  mc->cd(3)->SetLogy(1);
  mc->cd(4)->SetLogy(1);
  gPad->Update();
}

//Treelines
void QwGUITrackFinding::PlotChi2()
{
  std::cout<<"Plotting Chi2..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

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
  mc->Divide(1,3);


  obj = HistArray.At(0);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();

  obj = HistArray.At(4);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(2);
  obj->Draw();

  obj = HistArray.At(7);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();

  gPad->Update();

//  std::cout<<"Done"<<std::endl;

}

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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(10);  // Region 2
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(14);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(17);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(20);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(24);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(27);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(30);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(34); // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(37);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
  return;
}


//PartialTracks
void QwGUITrackFinding::PlotpChi2()
{
  std::cout<<"Plotting Chi2..."<<std::endl;

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
  mc->Divide(1,2);

  obj = HistArray.At(40);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();

  obj = HistArray.At(41);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(2);
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotpResidual()
{  std::cout<<"Plotting PT Residuals..."<<std::endl;

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
  mc->Divide(1,2);

  mc->cd(1);
  obj = HistArray.At(42);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(43);  // Get histogram from tree
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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

  mc->cd(1);
  obj = HistArray.At(44);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(46);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(45);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(4);
  obj = HistArray.At(47);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();
  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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

  mc->cd(1);
  obj = HistArray.At(48);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(50);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(49);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(4);
  obj = HistArray.At(51);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();
  gPad->Update();
//  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotPTPackage()
{  std::cout<<"Plotting Package..."<<std::endl;

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
  mc->Divide(1,1);

  mc->cd(1);
  obj = HistArray.At(52);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();
  gPad->Update();
//  std::cout<<"Done"<<std::endl;
  return;
}


// Plotting X,U, and V Planes

void QwGUITrackFinding::PlotTLChi2X()
{
  std::cout<<"Plotting Chi2 X..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);


  obj = HistArray.At(1);  // Region 2 (no x dir for Region 3)
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();

mc->cd(2)->Clear();
mc->cd(3)->Clear();

  gPad->Update();

//  std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotTLChi2U()
{
  std::cout<<"Plotting Chi2 U..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);


  obj = HistArray.At(2);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();

  obj = HistArray.At(5);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(2);
  obj->Draw();

  obj = HistArray.At(8);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();
  gPad->Update();

 // std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotTLChi2V()
{
  std::cout<<"Plotting Chi2 V..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR); //Linear and Log Plots are both already displayed below
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);


  obj = HistArray.At(3);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(1);
  obj->Draw();

  obj = HistArray.At(6);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(2);
  obj->Draw();

  obj = HistArray.At(9);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  mc->cd(3);
  obj->Draw();
  gPad->Update();

//  std::cout<<"Done"<<std::endl;
}

void QwGUITrackFinding::PlotTLResidualX()
{  std::cout<<"Plotting Residuals X..."<<std::endl;

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(11);  // Region 2 (no x dir for Region 3)
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2)->Clear();
  mc->cd(3)->Clear();
  gPad->Update();
//  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLResidualU()
{  std::cout<<"Plotting Residuals U..."<<std::endl;

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(12);  // Region 2
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(15);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(18);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
  return;
}

void QwGUITrackFinding::PlotTLResidualV()
{  std::cout<<"Plotting Residuals V..."<<std::endl;

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(13);  // Region 2
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(16);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(19);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
  return;
}



void QwGUITrackFinding::PlotTLOffsetX()
{  std::cout<<"Plotting Offset X..."<<std::endl;

  dMenuPlot1->EnableEntry(TF_PLOT_LINEAR);
  dMenuPlot1->EnableEntry(TF_PLOT_LOG);

  TObject *obj = NULL;

  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,3);

  mc->cd(1);

  obj = HistArray.At(21);  // Region 2 (no x dir for Region 3)
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2)->Clear();
  mc->cd(3)->Clear();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);

  obj = HistArray.At(22);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(25);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(28);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(23);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(26);  // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(29);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(31);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2)->Clear(); //There is no X Plane for Region 3
  mc->cd(3)->Clear();
  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(32);  // Region 2
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(35); // Region 3
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(38);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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
  mc->Divide(1,3);

  mc->cd(1);
  obj = HistArray.At(33);  // Get histogram from tree
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};

  obj->Draw();

  mc->cd(2);
  obj = HistArray.At(36);
  if(! obj)
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  mc->cd(3);
  obj = HistArray.At(39);  // Region 3
  if(! obj) 
	{
	std::cout<<"Error: no obj in HistArray"<<std::endl;
	return;
	};
  obj->Draw();

  gPad->Update();
//  std::cout<<"Done"<<std::endl;
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

		case TF_PPACKAGE:
		  PlotPTPackage();
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

	    case TL_PLOT_RESIDUAL_X:
	      PlotTLResidualX();
	      break;

	    case TL_PLOT_RESIDUAL_U:
	      PlotTLResidualU();
	      break;

	    case TL_PLOT_RESIDUAL_V:
	      PlotTLResidualV();
	      break;

	    case TF_RESIDUAL:
	      PlotResidual();
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



