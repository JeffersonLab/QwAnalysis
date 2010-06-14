#include "QwGUIInjector.h"

#include <TG3DLine.h>
#include "TGaxis.h"


ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POS_DIFF,
  BA_CHARGE,
  BA_POS_VAR,
};

const char *QwGUIInjector::InjectorTrees[INJECTOR_DET_TRE_NUM] = 
  {
    "HEL_Tree",
    "MPS_Tree"
  };

const char *QwGUIInjector::InjectorDevices[INJECTOR_DEV_NUM]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05","qwk_0i07","qwk_0l01","qwk_0l02","qwk_bcm0l02",
   "qwk_0l03","qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07",
   "qwk_0l08","qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02",
   "qwk_0r05","qwk_0r06"
    };


QwGUIInjector::QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
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
  dButtonPos          = NULL;
  dButtonCharge       = NULL;
  dButtonPosVariation = NULL;
  

  PosVariation[0] = NULL;
  PosVariation[1] = NULL;


  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);
  

}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dButtonPos)          delete dButtonPos;
  if(dButtonCharge)       delete dButtonCharge;
  if(dButtonPosVariation) delete dButtonPosVariation;
  

  delete [] PosVariation;

  RemoveThisTab(this);
  IsClosing(GetName());
}

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
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

 
  //dControlsFrame->AddFrame( dButtonPos, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 0, 5));
  
 //  TGGroupFrame *group = new TGGroupFrame(dControlsFrame, "Controls");
//   group->SetTitlePos(TGGroupFrame::kCenter);

  dButtonPos = new TGTextButton(dControlsFrame, "&Beam Position Differences", BA_POS_DIFF);
  dButtonCharge = new TGTextButton(dControlsFrame, "&Beam Charge Asymmetries", BA_CHARGE);
  dButtonPosVariation = new TGTextButton(dControlsFrame, "&Beam Position Variation", BA_POS_VAR);

  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dButtonPos,dBtnLayout );
  dControlsFrame->AddFrame(dButtonCharge,dBtnLayout);
  dControlsFrame->AddFrame(dButtonPosVariation, dBtnLayout);
  
  // dControlsFrame->AddFrame(group,new TGLayoutHints(kLHintsExpandX));


  dButtonPos -> Associate(this);
  dButtonCharge -> Associate(this);
  dButtonPosVariation -> Associate(this);
 
  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIInjector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

 //  TCanvas *mc = dCanvas->GetCanvas();
//   mc->Divide( 2, 4);

  Int_t wid = dCanvas->GetCanvasWindowId();
  //  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);
  //  super_canvas -> Divide(2,4);

}

void QwGUIInjector::OnReceiveMessage(char *obj)
{

}

void QwGUIInjector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIInjector::OnObjClose\n");

    dROOTCont = NULL;
  }
}


void QwGUIInjector::OnNewDataContainer()
{

  TObject *obj = NULL;
  TObject *copy = NULL;
  
  /*  
  ClearData();

  if(dROOTCont){
    for(Short_t p = 0; p < INJECTOR_DET_TRE_NUM; p++) {
      obj = dROOTCont->ReadTree(InjectorTrees[p]);
      if(obj)
	{
	  copy = obj->Clone();
	  HistArray.Add(copy);
	}
    }
  }
  */
};

void QwGUIInjector::OnRemoveThisTab()
{

};

void QwGUIInjector::ClearData()
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






void QwGUIInjector::PositionDifferences()
{
  TH1F *histo1=NULL;

  Bool_t ldebug = false;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();


   while (1){ 
    histo1= (TH1F *)dROOTCont->GetObjFromMapFile("diff_qwk_qpdRelY_hw");
   
    
    mc->Clear();
    mc->Divide(1,1);

    mc->cd(1);
    histo1->Draw();
    gPad->Update();
    gPad->Update();

    mc->Modified();
    mc->Update();

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

  printf("---------------PositionDifferences()--------------------\n");
  //mc->Modified();
  //mc->Update();
  

  return;
}

void QwGUIInjector::PlotPositionDiff()
{
  TH1F *histo1=NULL;

  Bool_t ldebug = false;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();


   while (1){
    histo1= (TH1F *)dROOTCont->GetObjFromMapFile("diff_qwk_qpdRelX_hw");
   
    
    mc->Clear();
    mc->Divide(1,1);

    mc->cd(1);
    histo1->Draw();
    gPad->Update();
    gPad->Update();

    mc->Modified();
    mc->Update();

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

  printf("---------------PlotPositionDiff()--------------------\n");
  //mc->Modified();
  //mc->Update();
  

  return;
}


void QwGUIInjector::PlotChargeAsym(){
  TCanvas *mc = NULL;
  TH1F *histo2=NULL;
  mc = dCanvas->GetCanvas();

  
  while (1){
    histo2= (TH1F *)dROOTCont->GetObjFromMapFile("asym_qwk_bcm0l02_hw");

    
    mc->Clear();
    mc->Divide(1,1);

    mc->cd(1);
    histo2->Draw();
    gPad->Update();

    mc->Modified();
    mc->Update();

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      //printf("UpdateMemoryMap() Exit %d ",gSystem->ProcessEvents());
      break;
    }
  }
  return;
}
  
  
  
  

void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= INJECTOR_DEV_NUM)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIInjector",
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


Bool_t QwGUIInjector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
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
      if(dROOTCont){
	switch (GET_SUBMSG(msg))
	  {
	  case kCM_BUTTON:
	    {
	      switch(parm1)
		{
		case   BA_POS_DIFF:
		  //printf("text button id %ld pressed\n", parm1);	
		  PlotPositionDiff();
		  break;
		  
		case BA_CHARGE:
		  
		  //printf("text button id %ld pressed\n", parm1);
		  PlotChargeAsym();
		  break;

		case BA_POS_VAR:
		  printf("PlotPosData() button id %ld pressed\n", parm1);
		  PositionDifferences();
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
	std::cout<<"Please load the map file to view data. \n";
      }
      default:
	break;
    }
  
  return kTRUE;
}



void 
QwGUIInjector::SummaryHist(TH1 *in)
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
