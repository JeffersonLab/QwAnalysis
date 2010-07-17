#include <QwGUIScanner.h>

ClassImp(QwGUIScanner);

using namespace QwParityDB;

const char *QwGUIScanner::ScannerDataNames[SCANNER_INDEX] = 
  {"BackADC","BackSCA","BackTDC","FrontADC","FrontSCA","FrontTDC",
   "PositionX_ADC","PositionY_ADC","PositionX_VQWK","PositionY_VQWK",
   "PowerSupply_VQWK","CoincidenceSCA",
   "back__adc","back__sca","back__tdc","front_adc","front_sca","front_tdc",
   "pos_x_adc","pos_y_adc","pos_x_vqwk","pos_y_vqwk","power_vqwk"
//    "scanner_front_adc","scanner_back__adc","scanner_pos_x_adc","scanner_pos_y_adc",
//    "scanner_position_x","scanner_position_y","scanner_ref_posi_x","scanner_ref_posi_y",
//    "scanner_rate_map"
    };

QwGUIScanner::QwGUIScanner(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dNumberEntryDlg = NULL;

  AddThisTab(this);

  ClearRootData();
  ClearDBData();
}

QwGUIScanner::~QwGUIScanner()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
  ClearRootData();
  ClearDBData();
}

void QwGUIScanner::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);

  dTabFrame = new TGVerticalFrame(this,10,10);


  dMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
				     0, 0, 1, 1);
  dMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  
  dMenuPlot = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot->AddEntry("&Rate Map", SCANNER_PLOT_RATEMAP);
  dMenuPlot->AddEntry("&Histograms (processed data)", SCANNER_PLOT_HISTOGRAM);
  dMenuPlot->AddEntry("&Histograms (raw data)", SCANNER_PLOT_RAW);
  //dMenuPlot->AddSeparator();

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Plots", dMenuPlot, dMenuBarItemLayout);

  dTabFrame->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuPlot->Associate(this);

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);     
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);

  dMenuPlot->CheckEntry(SCANNER_PLOT_RATEMAP);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  				"QwGUIScanner",
  				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  TCanvas *mc = dCanvas->GetCanvas();
  mc->Divide(6,4);

}

void QwGUIScanner::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIScanner::OnObjClose(char *obj)
{
  if(!obj) return;
  TString name = obj;

  if(name.Contains("dDataWindow")){
    QwGUIDataWindow* window = (QwGUIDataWindow*)DataWindowArray.Remove(DataWindowArray.FindObject(obj));
    if(window) delete window;
  }

  if(!strcmp(obj,"dNumberEntryDlg")){
    delete dNumberEntryDlg;
    dNumberEntryDlg = NULL;
  }

  if(!strcmp(obj,"dProgrDlg")){
    dProcessHalt = kTrue;
    dProgrDlg = NULL;
  }   

  QwGUISubSystem::OnObjClose(obj);
}

void QwGUIScanner::OnNewDataContainer(RDataContainer *cont)
{

  if(!cont) return;

  TObject *obj;
  TTree *tree;
  TH1D *hst;
  TGraph *grp;
  TProfile *prf;

  Double_t min = 5e9;
  Double_t max = 0;
  Double_t sum = 0;
  Double_t sumsq = 0;
  Double_t mean = 0;
  Double_t rms = 0;
  Int_t events = 0;
  

  if(!strcmp(cont->GetDataName(),"ROOT") && dROOTCont){

    obj = dROOTCont->ReadData("tree");
    if(obj) {
       printf("Reading data from 'tree'\n");
    }
    else {
       obj = dROOTCont->ReadData("MPS_Tree");
       if(obj) printf("Reading data from 'MPS_Tree'\n");
    }

    if(obj){
       if(obj->InheritsFrom("TTree")){
	 tree = (TTree*)obj->Clone();

	 ClearRootData();  
	
	 for(int i = 0; i < SCANNER_INDEX; i++){

	  if ( tree -> FindLeaf(Form("scanner.%s",ScannerDataNames[i])) ) 
 	  {
	    min = 5e9;
	    max = 0;
	    sum = 0;  
	    sumsq = 0;
	    events = 0;

	    tree->Draw(Form("scanner.%s",ScannerDataNames[i]),"","",tree->GetEntries(),0);

	    for(int j = 0; j < tree->GetEntries(); j++){
	      dCurrentData[i].push_back(tree->GetV1()[j]);

	      if(dCurrentData[i].at(j) > 0){
		if(dCurrentData[i].at(j) < min) min = dCurrentData[i].at(j);
		if(dCurrentData[i].at(j) > max) max = dCurrentData[i].at(j);
		sum += dCurrentData[i].at(j);
		sumsq += dCurrentData[i].at(j)*dCurrentData[i].at(j);
		events++;
	      }
	    }

	    mean = sum/events;
	    rms = sqrt(sumsq/events - mean*mean);
	    printf("Mean = %f, RMS = %f\n",mean,rms);

	    dCurrentDataMin[i] = min;
	    dCurrentDataMax[i] = max;
	    dCurrentDataMean[i] = mean;
	    dCurrentDataRMS[i] = rms;

	    hst = new TH1D(Form("hst%02d",i),Form("scanner.%s",ScannerDataNames[i]),1000,-4*rms,+4*rms);
	    grp = new TGraph();
	    grp->SetTitle(Form("scanner.%s",ScannerDataNames[i]));
	    grp->SetName(Form("grp%02d",i));
	    hst->SetDirectory(0);

	    hst->GetXaxis()->SetTitle("RMS (mean subtracted) [V]");
	    hst->GetXaxis()->CenterTitle();
	    hst->GetXaxis()->SetTitleSize(0.04);
	    hst->GetXaxis()->SetLabelSize(0.04);
	    hst->GetXaxis()->SetTitleOffset(1.25);
	    hst->GetYaxis()->SetLabelSize(0.04);
	    
	    for(int j = 0; j < tree->GetEntries(); j++){
	      if(dCurrentData[i].at(j) > 0){
		hst->Fill(dCurrentData[i].at(j)-mean);
		grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData[i].at(j));
	      }
	    }
	    grp->GetYaxis()->SetRangeUser(min,max);
	    grp->GetXaxis()->SetTitle("Time [sec]");
	    grp->GetXaxis()->CenterTitle();
	    grp->GetXaxis()->SetTitleSize(0.04);
	    grp->GetXaxis()->SetLabelSize(0.04);
	    grp->GetXaxis()->SetTitleOffset(1.25);
	    grp->GetYaxis()->SetTitle("Amplitude [V]");
	    grp->GetYaxis()->CenterTitle();
	    grp->GetYaxis()->SetTitleSize(0.04);
	    grp->GetYaxis()->SetLabelSize(0.04);
	    grp->GetYaxis()->SetTitleOffset(1.5);

	    HistArray.Add(hst);
	    GraphArray.Add(grp);
	  }

	  else if ( tree -> FindLeaf(Form("scanner.%s_raw",ScannerDataNames[i])) ) 
 	  {
	    min = 5e9;
	    max = 0;
	    sum = 0;  
	    sumsq = 0;
	    events = 0;

	    tree->Draw(Form("scanner.%s_raw",ScannerDataNames[i]),"","goff",tree->GetEntries(),0);
	    
	    for(int j = 0; j < tree->GetEntries(); j++){
	      dCurrentData[i].push_back(tree->GetV1()[j]);

	      if(dCurrentData[i].at(j) > 0){
		if(dCurrentData[i].at(j) < min) min = dCurrentData[i].at(j);
		if(dCurrentData[i].at(j) > max) max = dCurrentData[i].at(j);
		sum += dCurrentData[i].at(j);
		sumsq += dCurrentData[i].at(j)*dCurrentData[i].at(j);
		events++;
	      }
	    }

	    mean = sum/events;
	    rms = sqrt(sumsq/events - mean*mean);
	    printf("Mean = %f, RMS = %f\n",mean,rms);

	    dCurrentDataMin[i] = min;
	    dCurrentDataMax[i] = max;
	    dCurrentDataMean[i] = mean;
	    dCurrentDataRMS[i] = rms;

	    hst = new TH1D(Form("hst%02d",i),Form("scanner.%s_raw",ScannerDataNames[i]),1000,-4*rms,+4*rms);
	    grp = new TGraph();
	    grp->SetTitle(Form("scanner.%s_raw",ScannerDataNames[i]));
	    grp->SetName(Form("grp%02d",i));
	    hst->SetDirectory(0);

	    hst->GetXaxis()->SetTitle("RMS (mean subtracted) [V]");
	    hst->GetXaxis()->CenterTitle();
	    hst->GetXaxis()->SetTitleSize(0.04);
	    hst->GetXaxis()->SetLabelSize(0.04);
	    hst->GetXaxis()->SetTitleOffset(1.25);
	    hst->GetYaxis()->SetLabelSize(0.04);
	    
	    for(int j = 0; j < tree->GetEntries(); j++){
	      if(dCurrentData[i].at(j) > 0){
		hst->Fill(dCurrentData[i].at(j)-mean);
		grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData[i].at(j));
	      }
	    }
	    grp->GetYaxis()->SetRangeUser(min,max);
	    grp->GetXaxis()->SetTitle("Time [sec]");
	    grp->GetXaxis()->CenterTitle();
	    grp->GetXaxis()->SetTitleSize(0.04);
	    grp->GetXaxis()->SetLabelSize(0.04);
	    grp->GetXaxis()->SetTitleOffset(1.25);
	    grp->GetYaxis()->SetTitle("Amplitude [V]");
	    grp->GetYaxis()->CenterTitle();
	    grp->GetYaxis()->SetTitleSize(0.04);
	    grp->GetYaxis()->SetLabelSize(0.04);
	    grp->GetYaxis()->SetTitleOffset(1.5);

	    HistArray.Add(hst);
	    GraphArray.Add(grp);
	  }

	}	
      }

      PlotHistograms();
    }
  }

  if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){

    ClearDBData();
  }

}

void QwGUIScanner::OnRemoveThisTab()
{

}

void QwGUIScanner::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWind")) return;
  
  printf("Received Messager From: %s\n",obj);
}

void QwGUIScanner::ClearDBData()
{

}

void QwGUIScanner::ClearRootData()
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

  HistArray.Clear();
  GraphArray.Clear();

  for(int i = 0; i < SCANNER_INDEX; i++){
    dCurrentData[i].clear();
    dCurrentData[i].resize(0);
  }  
}

void QwGUIScanner::PlotHistograms()
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
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
}

void QwGUIScanner::PlotGraphs()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
    ((TGraph*)obj)->Draw("ap");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(SCANNER_PLOT_TYPE_GRAPH);
}

void QwGUIScanner::PlotRateMap()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();

  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
    ((TH2*)obj)->Draw("");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
}


void QwGUIScanner::PlotRawDataHist()
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
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
}



void QwGUIScanner::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= SCANNER_INDEX){
      if(GetPlotDataType() == SCANNER_PLOT_TYPE_HISTO){

	QwGUIDataWindow *dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
							   "QwGUIScanner",HistArray[pad-1]->GetTitle(), PT_HISTO_1D,
							   DDT_MD,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
	dDataWindow->SetStaticData(HistArray[pad-1],DataWindowArray.GetLast());

	Connect(dDataWindow,"IsClosing(char*)","QwGUIScanner",(void*)this,"OnObjClose(char*)");
	Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIScanner",(void*)this,"OnReceiveMessage(char*)");
	Connect(dDataWindow,"UpdatePlot(char*)","QwGUIScanner",(void*)this,"OnUpdatePlot(char *)");      

	dDataWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
	dDataWindow->DrawData(*((TH1D*)HistArray[pad-1]));
	SetLogMessage(Form("Looking at histogram %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);
	
	return;
      }
      else if(GetPlotDataType() == SCANNER_PLOT_TYPE_GRAPH){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIScanner",
						     GraphArray[pad-1]->GetTitle(), PT_GRAPH,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)GraphArray[pad-1]->GetTitle());
	dMiscWindow->SetPlotTitleX("Time [sec]");
	dMiscWindow->SetPlotTitleY("Amplitude [V]");
	dMiscWindow->SetPlotTitleOffset(1.25,1.8);
	dMiscWindow->SetAxisMin(((TGraph*)GraphArray[pad-1])->GetXaxis()->GetXmin(),dCurrentDataMin[pad-1]);
	dMiscWindow->SetAxisMax(((TGraph*)GraphArray[pad-1])->GetXaxis()->GetXmax(),dCurrentDataMax[pad-1]);
	dMiscWindow->SetLimitsFlag(kTrue);
	
	dMiscWindow->DrawData(*((TGraph*)GraphArray[pad-1]));
	SetLogMessage(Form("Looking at graph %s\n",(char*)GraphArray[pad-1]->GetTitle()),kTrue);
      }
    }
  }
}

Bool_t QwGUIScanner::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
	
      case SCANNER_PLOT_RATEMAP:
        PlotRateMap();	
	break;

      case SCANNER_PLOT_HISTOGRAM:
	PlotHistograms();
	break;

      case SCANNER_PLOT_RAW:
	//PlotGraphs();
        PlotRawDataHist();
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
