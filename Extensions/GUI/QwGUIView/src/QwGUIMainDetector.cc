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

  SetDFTCalculated(kFalse);

  ClearData();

}

QwGUIMainDetector::~QwGUIMainDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
  ClearData();
}

void QwGUIMainDetector::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);

  dTabFrame = new TGVerticalFrame(this,10,10);


  dMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
				     0, 0, 1, 1);
  dMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  
  dMenuData = new TGPopupMenu(fClient->GetRoot());
  dMenuData->AddEntry("Yield &Histogram", M_DATA_HISTO);
  dMenuData->AddEntry("Yield &Graph", M_DATA_GRAPH);
  dMenuData->AddEntry("Yield &DFT", M_DFT_HISTO);
//   dMenuData->AddSeparator();

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Data", dMenuData, dMenuBarItemLayout);

  dTabFrame->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuData->Associate(this);

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
//     printf("Called QwGUIMainDetector::OnObjClose\n");
    dROOTCont = NULL;
  }
}

void QwGUIMainDetector::OnNewDataContainer()
{

  TObject *obj;
  TTree *tree;
  ClearData();
  TH1D *hst;
  TGraph *grp;

  if(dROOTCont){
    
    obj = dROOTCont->ReadData("MD_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	tree = (TTree*)obj->Clone();
	
	for(int i = 0; i < MAIN_DET_HST_NUM; i++){

	  tree->Draw(Form("%s:hw_sum_raw",MainDetectorHists[i]),"","goff",tree->GetEntries(),0);
	  hst = new TH1D(Form("hst%02d",i),Form("%s:hw_sum_raw",MainDetectorHists[i]),1000,9,11);
	  grp = new TGraph();
	  grp->SetTitle(Form("%s:hw_sum_raw",MainDetectorHists[i]));
	  grp->SetName(Form("grp%02d",i));
	  hst->SetDirectory(0);

	  for(int j = 0; j < tree->GetEntries(); j++){
	    dCurrentData[i].push_back(tree->GetV1()[j]);
	    hst->Fill(tree->GetV1()[j]*1.0e-6);
	    grp->SetPoint(j,j*1.06+1,tree->GetV1()[j]*1.0e-6);
	  }
	  HistArray.Add(hst);
	  GraphArray.Add(grp);
	}	
      }
    }  
  }

  PlotHistograms();

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
  TIter nexth(HistArray.MakeIterator());
  obj = nexth();
  while(obj){    
    delete obj;
    obj = nexth();
  }
  TIter nextg(GraphArray.MakeIterator());
  obj = nextg();
  while(obj){    
    delete obj;
    obj = nextg();
  }

  TIter nextd(DFTArray.MakeIterator());
  obj = nextd();
  while(obj){    
    delete obj;
    obj = nextd();
  }
  HistArray.Clear();
  GraphArray.Clear();
  DFTArray.Clear();

  for(int i = 0; i < MAIN_DET_HST_NUM; i++){
    dCurrentData[i].clear();
    dCurrentData[i].resize(0);
  }
  
}

void QwGUIMainDetector::PlotHistograms()
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

  SetPlotDataType(PLOT_TYPE_HISTO);
}

void QwGUIMainDetector::PlotGraphs()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    ((TGraph*)obj)->Draw("ap");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(PLOT_TYPE_GRAPH);
}

void QwGUIMainDetector::PlotDFT()
{
  if(!IsDFTCalculated())
    CalculateDFT();

  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(DFTArray.MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    ((TH1*)obj)->Draw();
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(PLOT_TYPE_DFT);
  
}

void QwGUIMainDetector::CalculateDFT()
{
  Int_t N = 0;                   //Current number of events/helicity
                                 //states in the data.

  Double_t dt = 1.06e-3;         //Sampling interval in seconds,
                                 //corresponding to the current
                                 //helicity reversal rate.

  Double_t T = 0;                //Sampling time (run length) in seconds;

  Double_t fmax = 1.0/(2*dt);    //Maximum frequency bandwidth set accodingt
                                 //to the Nyquist sampling criterion.  

  Double_t df;                   //Frequency resolution= 1/T

  Int_t M = (Int_t)(N/2);        //Number of frequency samples = fmax/df = N/2;

  Double_t fValx  = 0;            //Temporary variable to hold the frequency coordinate
  Double_t fValyR = 0;            //Temporary variable to hold the real component of the frequency amplitude coordinate
  Double_t fValyI = 0;            //Temporary variable to hold the imaginary component of the frequency amplitude coordinate

  Double_t tValx = 0;//new Double_t;            //Temporary variable to hold the time coordinate
  Double_t tValy = 0;//new Double_t;            //Temporary variable to hold the time sample value coordinate

  TH1D *hst;
  TGraph *grp;

  TObject *obj;
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){
    grp = ((TGraph*)obj);
    N = grp->GetN();
    T = N*dt;
    df = 1.0/T;
    M = (Int_t)(N/2);

    hst = new TH1D(Form("%s_DFT",grp->GetName()),Form("%s DFT",grp->GetTitle()),M,0,fmax);

    for(int j = 0; j < M; j++){
      fValx = j*df;
      fValyR = 0;
      fValyI = 0;
      for(int k = 0; k < N; k++){
	if(grp->GetPoint(k,tValx,tValy) > 0){
	  fValyR += tValy*TMath::Cos(2*TMath::Pi()*j*k/N);
	  fValyI += tValy*TMath::Sin(2*TMath::Pi()*j*k/N);
	}
	else{
	  fValyI += 0;
	  fValyR += 0;
	}
      } 
      hst->SetBinContent(j+1,sqrt(fValyR*fValyR+fValyI*fValyI));
    }
    DFTArray.Add(hst);
    obj = next();
  }

  SetDFTCalculated(kTrue);
}

void QwGUIMainDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= MAIN_DET_HST_NUM){
      if(GetPlotDataType() == PLOT_TYPE_HISTO){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TH1D*)HistArray[pad-1]));
	SetLogMessage(Form("Looking at histogram %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);
	
	return;
      }
      else if(GetPlotDataType() == PLOT_TYPE_GRAPH){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     HistArray[pad-1]->GetTitle(), PT_GRAPH,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)GraphArray[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TGraph*)GraphArray[pad-1]));
	SetLogMessage(Form("Looking at graph %s\n",(char*)GraphArray[pad-1]->GetTitle()),kTrue);
      }
      else if(GetPlotDataType() == PLOT_TYPE_DFT){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     DFTArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)DFTArray[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TH1D*)DFTArray[pad-1]));
	SetLogMessage(Form("Looking at graph %s\n",(char*)DFTArray[pad-1]->GetTitle()),kTrue);
      }
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
	
      case M_DATA_HISTO:
	PlotHistograms();
	break;

      case M_DATA_GRAPH:
	PlotGraphs();
	break;

      case M_DFT_HISTO:
	PlotDFT();
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
