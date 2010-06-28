#include "QwGUITrackFinding.h"

#include <TG3DLine.h>
#include <TGaxis.h>

#include "QwDetectorInfo.h"
#include "QwEvent.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"

ClassImp(QwGUITrackFinding);

enum QwGUITrackFindingIndentificator {
  TF_CHI2,
  TF_RESIDUAL
};

QwGUITrackFinding::QwGUITrackFinding(const TGWindow *p, const TGWindow *main, const TGTab *tab,
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
  dBtnChi2            = NULL;
  dBtnResidual        = NULL;

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
  if(dCanvas)             delete dCanvas;
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dBtnChi2)            delete dBtnChi2;
  if(dBtnResidual)        delete dBtnResidual;

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


  dTabFrame = new TGHorizontalFrame(this);
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200);
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  dBtnChi2  = new TGTextButton(dControlsFrame, "&Chi2", TF_CHI2);
  dBtnResidual = new TGTextButton(dControlsFrame, "&Residuals", TF_RESIDUAL);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dBtnChi2 ,dBtnLayout );
  dControlsFrame->AddFrame(dBtnResidual,dBtnLayout );

  dBtnChi2 -> Associate(this);
  dBtnResidual-> Associate(this);

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
  if (!cont) return;

  if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont) {

    TObject* obj = dROOTCont->ReadData(fRootTrees.at(0));
    // Note: fRootTrees[0] = event_tree

    if (obj) {
       if (obj->InheritsFrom("TTree")) {
         TTree* tree = (TTree*) obj->Clone();

         if (tree -> FindBranch("events"))
           {

            Int_t entries = tree->GetEntries();
            std::cout << "Entries in tree: " << entries << std::endl;

            QwHitRootContainer* roothitlist = 0;
            QwEvent* event = 0;

            tree->SetBranchAddress("hits", &roothitlist);
            tree->SetBranchAddress("events", &event);

            // Defining histogram
            TH1D* hst = new TH1D("distance","distance",100,0.0,3.0);
            hst->SetDirectory(0);

            hst->GetXaxis()->SetTitle("distance");
            hst->GetXaxis()->CenterTitle();
            hst->GetXaxis()->SetTitleSize(0.04);
            hst->GetXaxis()->SetLabelSize(0.04);
            hst->GetXaxis()->SetTitleOffset(1.25);
            hst->GetYaxis()->SetLabelSize(0.04);

            for(int j = 0; j < tree->GetEntries(); j++){
              tree->GetEntry(j);

              QwHitContainer* hitlist = roothitlist->Convert();
              for (QwHitContainer::iterator hit = hitlist->begin();
                   hit != hitlist->end(); hit++) {

                if (hit->GetRegion() == kRegionID3) {
                  hst->Fill(hit->GetDriftDistance());
                }
              }
            }

            HistArray.Add(hst);
        }
      }

      PlotHistograms();
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


void QwGUITrackFinding::PlotChi2()
{
  return;
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




// Display the beam postion and angle in X & Y, beam energy and beam charge on the target.
void QwGUITrackFinding::PlotResidual()
{
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



