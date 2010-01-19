#include "QwGUIInjector.h"

ClassImp(QwGUIInjector);

const char *QwGUIInjector::InjectorHists[INJECTOR_DET_HST_NUM] = 
  { "yield_qwk_qpdym_hw","C20AXM_block2_raw",
    "AmpCh1_block1_raw","H00_YP_block1_raw"};

QwGUIInjector::QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;

  AddThisTab(this);
}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIInjector::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 10, 10, 11, 12);

  dTabFrame = new TGHorizontalFrame(this,10,10);
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);     
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIInjector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");
  TCanvas *mc = dCanvas->GetCanvas();
  mc->Divide( INJECTOR_DET_HST_NUM/2, INJECTOR_DET_HST_NUM/2);
  PlotData();
}

void QwGUIInjector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIInjector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
    printf("Called QwGUIInjector::OnObjClose\n");

    dROOTCont = NULL;
  }
}

void QwGUIInjector::OnNewDataContainer()
{

  TObject *obj;
  TObject *copy;
  ClearData();

  if(dROOTCont){
    for(int p = 0; p < INJECTOR_DET_HST_NUM; p++){
	
      obj = dROOTCont->ReadData(InjectorHists[p]);
      if(obj){

	if(obj->InheritsFrom("TH1")){
	  copy = obj->Clone();
	  ((TH1*)copy)->SetName(Form("%s_cp",((TH1*)obj)->GetName()));
	  ((TH1*)copy)->SetDirectory(0);
	  HistArray.Add(copy);
	}
      }
    }
  }  

}

void QwGUIInjector::OnRemoveThisTab()
{

}

void QwGUIInjector::ClearData()
{

  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();
  while(obj){    
    delete obj;
    obj = next();
  }
  HistArray.Clear();
}

void QwGUIInjector::PlotData()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    ((TH1*)obj)->Draw("");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

}

void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= INJECTOR_DET_HST_NUM)
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
  switch (GET_MSG(msg)){

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_ENTER:
      switch (parm1) {

      default:
	break;
      }

    default:
      break;
    }

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
      
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
    
  default:
    break;
  }
  
  return kTRUE;
}
