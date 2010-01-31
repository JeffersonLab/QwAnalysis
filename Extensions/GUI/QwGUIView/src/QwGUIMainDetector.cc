#include <QwGUIMainDetector.h>

ClassImp(QwGUIMainDetector);

const char *QwGUIMainDetector::MainDetectorHists[MAIN_DET_HST_NUM] = 
  {"bar1left","bar2left","bar3left","bar4left","bar5left","bar6left","bar7left","bar8left",
   "bar1right","bar2right","bar3right","bar4right","bar5right","bar6right","bar7right","bar8right"};

QwGUIMainDetector::QwGUIMainDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;

//   QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
//   QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Parity/prminput");

//   TString mapfile = "qweak_adc.map";
//   QwParameterFile mapstr(mapfile.Data());

  AddThisTab(this);
}

QwGUIMainDetector::~QwGUIMainDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIMainDetector::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);

  dTabFrame = new TGHorizontalFrame(this,10,10);
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);     
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIMainDetector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");
  TCanvas *mc = dCanvas->GetCanvas();
  mc->Divide(4,4);

}

void QwGUIMainDetector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIMainDetector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
    printf("Called QwGUIMainDetector::OnObjClose\n");

    dROOTCont = NULL;
  }
}

void QwGUIMainDetector::OnNewDataContainer()
{

  TObject *obj;
  TTree *tree;
  TBranch *branch;
  Double_t val;
  ClearData();
  TH1D *hst[MAIN_DET_HST_NUM];

  if(dROOTCont){
    
    obj = dROOTCont->ReadData("MD_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	tree = (TTree*)obj->Clone();
	
	for(int i = 0; i < MAIN_DET_HST_NUM; i++){
	  sprintf(dMiscbuffer,"%s:hw_sum_raw",MainDetectorHists[i]);
	  tree->Draw(dMiscbuffer,"","goff",tree->GetEntries(),0);
	  sprintf(dMiscbuffer2,"hst%02d",i);
	  hst[i] = new TH1D(dMiscbuffer2,dMiscbuffer,1000,9,11);
	  for(int j = 0; j < tree->GetEntries(); j++){
	    //printf("Value %1.2e\n",tree->GetV1()[i]);
	    hst[i]->Fill(tree->GetV1()[j]*1.0e-6);
	  }
	  HistArray.Add(hst[i]);
	}
      }
    }  
  }
  PlotData();

//     for(int p = 0; p < MAIN_DET_HST_NUM; p++){
	
//       obj = dROOTCont->ReadData(MainDetectorHists[p]);
//       if(obj){

// 	if(obj->InheritsFrom("TH1")){
// 	  copy = obj->Clone();
// 	  ((TH1*)copy)->SetName(Form("%s_cp",((TH1*)obj)->GetName()));
// 	  ((TH1*)copy)->SetDirectory(0);
// 	  HistArray.Add(copy);
// 	}
//       }
//     }

}

void QwGUIMainDetector::OnRemoveThisTab()
{

}

void QwGUIMainDetector::ClearData()
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

void QwGUIMainDetector::PlotData()
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

void QwGUIMainDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= MAIN_DET_HST_NUM)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
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


Bool_t QwGUIMainDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
