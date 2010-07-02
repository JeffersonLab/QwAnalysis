#include <QwGUIMainDetector.h>

ClassImp(QwGUIMainDetector);

using namespace QwParityDB;

const char *QwGUIMainDetector::MainDetectorPMTNames[MAIN_PMT_INDEX] = 
  {"md1neg","md2neg","md3neg","md4neg","md5neg","md6neg","md7neg","md8neg",
   "md1pos","md2pos","md3pos","md4pos","md5pos","md6pos","md7pos","md8pos"};

const char *QwGUIMainDetector::MainDetectorNames[MAIN_DET_INDEX] = 
  {"md1","md2","md3","md4","md5","md6","md7","md8"};

const char *QwGUIMainDetector::MainDetectorBlockTypesRaw[MAIN_DET_BLOCKIND] = 
  {"hw_sum_raw","block0_raw","block1_raw","block2_raw","block3_raw"};

const char *QwGUIMainDetector::MainDetectorBlockTypes[MAIN_DET_BLOCKIND] = 
  {"hw_sum","block0","block1","block2","block3"};


void QwGUIMainDetectorDataStructure::PushData(Double_t item)
{
  
  Data.push_back(item);
	      
//   if(item != 0){
		
    if(item < DataMin) DataMin = item;
    if(item > DataMax) DataMax = item;
		
    DataSum += item;
    EnsambleSum += item;
    
    DataSumSq += item*item;
    EnsambleSumSq += item*item;
    EnsambleCounter++;
    
    if(EnsambleCounter == EnsambleSize){
      EnsambleMean.push_back(EnsambleSum/EnsambleSize);
      EnsambleError.push_back(sqrt(EnsambleSumSq/EnsambleSize -pow(EnsambleSum/EnsambleSize,2))/sqrt(EnsambleSize));
      EnsambleReset();
    }
//   }
}

void QwGUIMainDetectorDataStructure::CalculateStats()
{
  DataMean = DataSum/Data.size();	  
  DataRMS  = sqrt(DataSumSq/Data.size() - DataMean*DataMean);	  
}

void QwGUIMainDetectorDataStructure::Clean() 
{
  if(Data.size()){ 
    Data.clear();  
    Data.resize(0); 
  } 
  FFT.clear(); FFT.resize(0); 
  EnsambleMean.clear(); EnsambleMean.resize(0);
  EnsambleError.clear(); EnsambleError.resize(0); 
  EnsambleSum = 0;
  EnsambleSumSq = 0;
  EnsambleCounter = 0;
  DataMean  = 0;
  DataMin   = 0;
  DataMax   = 0;
  DataRMS   = 0;
  DataSum   = 0;  
  DataSumSq = 0;
} 


QwGUIMainDetectorData::QwGUIMainDetectorData(Int_t size) 
{   
  if(size < 0) {
    dSize = 0; 
    dData = NULL;
  }
  else {
    dSize = size;
    dData = new QwGUIMainDetectorDataStructure[dSize];
  }
}

void QwGUIMainDetectorData::Clean() {
  
  for(int i = 0; i < dSize; i++){ 
    dData[i].Clean(); 
  } 
};


QwGUIMainDetector::QwGUIMainDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dNumberEntryDlg = NULL;

  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){
    dCurrentYields[n] = new QwGUIMainDetectorData(MAIN_PMT_INDEX);
    dCurrentYields[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentPMTAsyms[n] = new QwGUIMainDetectorData(MAIN_PMT_INDEX);
    dCurrentPMTAsyms[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentDETAsyms[n] = new QwGUIMainDetectorData(MAIN_DET_INDEX);
    dCurrentDETAsyms[n]->SetName(MainDetectorBlockTypes[n]);
  }

  NewDataInit();
  ClearRootData();
  ClearDFTData();
  ClearDBData();

  AddThisTab(this);
}

void QwGUIMainDetector::NewDataInit()
{
  FFTOptions opts;

  opts.calcFlag    = kFalse;
  opts.cancelFlag  = kFalse;
  opts.changeFlag  = kFalse;
  opts.Start       = 0;
  opts.Length      = 0; 
  opts.TotalLength = 0;

  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){
    dCurrentYields[n]->Clean();
    dCurrentYields[n]->SetFFTOptions(opts);
    dCurrentPMTAsyms[n]->Clean();
    dCurrentPMTAsyms[n]->SetFFTOptions(opts);
    dCurrentDETAsyms[n]->Clean();
    dCurrentDETAsyms[n]->SetFFTOptions(opts);
  }

  SetDataIndex(0);
}

QwGUIMainDetector::~QwGUIMainDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  ClearRootData();
  ClearDFTData();
  ClearDBData();

  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){
    delete dCurrentYields[n];
    delete dCurrentPMTAsyms[n];
    delete dCurrentDETAsyms[n];
  }

  RemoveThisTab(this);
  IsClosing(GetName());

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
  
  dMenuPlot = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot->AddEntry("&Histogram", M_PLOT_HISTO);
  dMenuPlot->AddEntry("&Graph", M_PLOT_GRAPH);
  dMenuPlot->AddEntry("&DFT", M_PLOT_DFT);
  dMenuPlot->AddEntry("DFT &Seq", M_PLOT_DFTPROF);
//   dMenuData->AddSeparator();

  dMenuData  = new TGPopupMenu(fClient->GetRoot());

  dMenuBlock1 = new TGPopupMenu(fClient->GetRoot());
  dMenuData->AddPopup("&Yields", dMenuBlock1);
  dMenuBlock1->AddEntry("Block &1", M_DATA_PMT_YIELD_B1);
  dMenuBlock1->AddEntry("Block &2", M_DATA_PMT_YIELD_B2);
  dMenuBlock1->AddEntry("Block &3", M_DATA_PMT_YIELD_B3);
  dMenuBlock1->AddEntry("Block &4", M_DATA_PMT_YIELD_B4);
  dMenuBlock1->AddEntry("Block &Sum", M_DATA_PMT_YIELD_SUM);
//   dMenuData->AddEntry("&Detector Yields", M_DATA_PMT_YIELD);
//   dMenuData->AddEntry("&Total Yield", M_DATA_ALL_YIELD);
//   dMenuData->Addopup("&Yield Combinations", dMenuYieldComb, dMenuBarItemLayout);
//   dMenuYieldComb->AddEntry("")
  dMenuData->AddSeparator();

  dMenuBlock2 = new TGPopupMenu(fClient->GetRoot());
  dMenuData->AddPopup("&PMT Asymmetries", dMenuBlock2);
  dMenuBlock2->AddEntry("Block &1", M_DATA_PMT_ASYM_B1);
  dMenuBlock2->AddEntry("Block &2", M_DATA_PMT_ASYM_B2);
  dMenuBlock2->AddEntry("Block &3", M_DATA_PMT_ASYM_B3);
  dMenuBlock2->AddEntry("Block &4", M_DATA_PMT_ASYM_B4);
  dMenuBlock2->AddEntry("Block &Sum", M_DATA_PMT_ASYM_SUM);

  dMenuData->AddSeparator();

  dMenuBlock3 = new TGPopupMenu(fClient->GetRoot());
  dMenuData->AddPopup("&Detector Asymmetries", dMenuBlock3);
  dMenuBlock3->AddEntry("Block &1", M_DATA_DET_ASYM_B1);
  dMenuBlock3->AddEntry("Block &2", M_DATA_DET_ASYM_B2);
  dMenuBlock3->AddEntry("Block &3", M_DATA_DET_ASYM_B3);
  dMenuBlock3->AddEntry("Block &4", M_DATA_DET_ASYM_B4);
  dMenuBlock3->AddEntry("Block &Sum", M_DATA_DET_ASYM_SUM);

  dMenuData->AddSeparator();

  dMenuBlock4 = new TGPopupMenu(fClient->GetRoot());
  dMenuData->AddPopup("&Combination Asymmetry", dMenuBlock4);
  dMenuBlock4->AddEntry("Block &1", M_DATA_CMB_ASYM_B1);
  dMenuBlock4->AddEntry("Block &2", M_DATA_CMB_ASYM_B2);
  dMenuBlock4->AddEntry("Block &3", M_DATA_CMB_ASYM_B3);
  dMenuBlock4->AddEntry("Block &4", M_DATA_CMB_ASYM_B4);
  dMenuBlock4->AddEntry("Block &Sum", M_DATA_CMB_ASYM_SUM);

//   dMenuData->AddSeparator();

//   dMenuData->AddEntry("&Total Asymmetry", M_DATA_ALL_ASYM);

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Plot Type", dMenuPlot, dMenuBarItemLayout);
  dMenuBar->AddPopup("&Data Type", dMenuData, dMenuBarItemLayout);

  dTabFrame->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuData->Associate(this);
  dMenuPlot->Associate(this);

  dMenuPlot->DisableEntry(M_PLOT_HISTO);
  dMenuPlot->DisableEntry(M_PLOT_GRAPH);
  dMenuPlot->DisableEntry(M_PLOT_DFT);
  dMenuPlot->DisableEntry(M_PLOT_DFTPROF);

  dMenuBlock1->DisableEntry(M_DATA_PMT_YIELD_B1);   
  dMenuBlock1->DisableEntry(M_DATA_PMT_YIELD_B2);   
  dMenuBlock1->DisableEntry(M_DATA_PMT_YIELD_B3);   
  dMenuBlock1->DisableEntry(M_DATA_PMT_YIELD_B4);   
  dMenuBlock1->DisableEntry(M_DATA_PMT_YIELD_SUM);

  dMenuBlock2->DisableEntry(M_DATA_PMT_ASYM_B1); 
  dMenuBlock2->DisableEntry(M_DATA_PMT_ASYM_B2); 
  dMenuBlock2->DisableEntry(M_DATA_PMT_ASYM_B3); 
  dMenuBlock2->DisableEntry(M_DATA_PMT_ASYM_B4); 
  dMenuBlock2->DisableEntry(M_DATA_PMT_ASYM_SUM);

  dMenuBlock3->DisableEntry(M_DATA_DET_ASYM_B1); 
  dMenuBlock3->DisableEntry(M_DATA_DET_ASYM_B2); 
  dMenuBlock3->DisableEntry(M_DATA_DET_ASYM_B3); 
  dMenuBlock3->DisableEntry(M_DATA_DET_ASYM_B4); 
  dMenuBlock3->DisableEntry(M_DATA_DET_ASYM_SUM);

  dMenuBlock4->DisableEntry(M_DATA_CMB_ASYM_B1); 
  dMenuBlock4->DisableEntry(M_DATA_CMB_ASYM_B2); 
  dMenuBlock4->DisableEntry(M_DATA_CMB_ASYM_B3); 
  dMenuBlock4->DisableEntry(M_DATA_CMB_ASYM_B4); 
  dMenuBlock4->DisableEntry(M_DATA_CMB_ASYM_SUM);

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);     
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);

//   Int_t wid = dCanvas->GetCanvasWindowId();
//   QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
//   dCanvas->AdoptCanvas(mc);

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



void QwGUIMainDetector::OnNewDataContainer(RDataContainer *cont)
{

  if(!cont) return;

  TObject *obj;
  TTree *MPSTree = NULL;
  TTree *HELTree = NULL;
  //  TProfile *prf;
  
//   Double_t min = 5e9;
//   Double_t max = 0;
//   Double_t sum = 0;
//   Double_t sumsq = 0;
//   Double_t mean = 0;
//   Double_t rms = 0;
  Int_t events = 0;
  Int_t bl = 0;
  Int_t det = 0;
  Int_t ev  = 0;
//   Double_t smplsum = 0;
//   Double_t smplsumsq = 0;
//   Double_t smplmean = 0;
//   Double_t smplrms = 0;  

  //Start filling root data  
  if(!strcmp(cont->GetDataName(),"ROOT") && dROOTCont){

    obj = dROOTCont->ReadData("MPS_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	MPSTree = (TTree*)obj->Clone();
      }
    }
    obj = dROOTCont->ReadData("HEL_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	HELTree = (TTree*)obj->Clone();
      }
    }

    if(!HELTree && !MPSTree){
      //put some warning dialog here, indicating that this
      //is not an open or valid root file ...
      return;
    }
    //otherwise continue

    //Get rid of old data
    NewDataInit();
    ClearRootData();    
    ClearDFTData();

    QwGUIMainDetectorDataStructure *CurrentPMTYield = NULL;
    QwGUIMainDetectorDataStructure *CurrentPMTAsym = NULL;

    //cycle through the main detectors and possible combinations thereof

    InitProgressDlg("Detector Event Fill","Block","Detector",0,5,dCurrentYields[0]->GetNumElements(),0,2);
 
    bl = 0;
    for(int n = 0; n < MAIN_DET_BLOCKIND; n++){

      CurrentPMTYield = dCurrentYields[n]->GetElements();
      CurrentPMTAsym = dCurrentPMTAsyms[n]->GetElements();
		
      dProgrDlg->ResetRange(0,dCurrentYields[n]->GetNumElements(),0);
      det = 0;
      for(int i = 0; i < dCurrentYields[n]->GetNumElements(); i++){

	if (MPSTree && MPSTree -> FindBranch(Form("%s",MainDetectorPMTNames[i]))) {
	    MPSTree->Draw(Form("%s.%s",MainDetectorPMTNames[i],dCurrentYields[n]->GetName()),"",
			  "goff",MPSTree->GetEntries(),0);
  	    
	    CurrentPMTYield[i].EnsambleReset();
	    events = 0;

// 	    dProgrDlg->ResetRange(0,0,MPSTree->GetEntries());
// 	    ev = 0;
	    for(int j = 0; j < MPSTree->GetEntries(); j++){

	      CurrentPMTYield[i].PushData(MPSTree->GetV1()[j]*76.3e-6/480);
	      
// 	      if(MPSTree->GetV1()[j] != 0){
		events++;
// 	      }

// 	      ev++;
// 	      if(dProcessHalt) break;
// 	      IncreaseProgress(0,0,&ev,0,0,20000);
	
	    }
	    CurrentPMTYield[i].CalculateStats();
// 	    printf("Det %d block %d: Min = %1.6e, Max = %1.6e, Mean = %1.6e, RMS = %1.6e\n",i,n,
// 		   CurrentPMTYield[i].GetDataMin(),CurrentPMTYield[i].GetDataMax(),CurrentPMTYield[i].GetDataMean(),
// 		   CurrentPMTYield[i].GetDataRMS());
	    FillYieldPlots(i,n);
	}
	
	if ( HELTree && HELTree -> FindBranch(Form("asym_%s",MainDetectorPMTNames[i]))) {
	  HELTree->Draw(Form("asym_%s.%s",MainDetectorPMTNames[i],dCurrentPMTAsyms[n]->GetName()),"",
			"goff",HELTree->GetEntries(),0);

	  CurrentPMTAsym[i].EnsambleReset();
	  events = 0;
	  
	  for(int j = 0; j < HELTree->GetEntries(); j++){
	    
	    CurrentPMTAsym[i].PushData(HELTree->GetV1()[j]);
	    
// 	    if(HELTree->GetV1()[j] != 0){
	      events++;
// 	    }	
	  }
	  CurrentPMTAsym[i].CalculateStats();
	  FillPMTAsymPlots(i,n);
	}

	det++;
	if(dProcessHalt) break;
	IncreaseProgress(0,&det,0,0,1,0);

	printf("Det %d finsihed\n",i);
      }
      
      bl++;
      if(dProcessHalt) break;
      IncreaseProgress(&bl,0,0,1,0,0);
    }

    if(dProgrDlg)
      dProgrDlg->CloseWindow();

    dMenuPlot->EnableEntry(M_PLOT_HISTO);
    dMenuPlot->EnableEntry(M_PLOT_GRAPH);
    dMenuPlot->EnableEntry(M_PLOT_DFT);
    dMenuPlot->EnableEntry(M_PLOT_DFTPROF);


    if(MPSTree){
      dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B1);   
      dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B2);   
      dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B3);   
      dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B4);   
      dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_SUM);
    }

    if(HELTree){

      dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B1); 
      dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B2); 
      dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B3); 
      dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B4); 
      dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_SUM);

//     dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B1); 
//     dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B2); 
//     dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B3); 
//     dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B4); 
//     dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_SUM);
  
//     dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B1); 
//     dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B2); 
//     dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B3); 
//     dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B4); 
//     dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_SUM);

    }

    if(MPSTree){
      dMenuPlot->CheckEntry(M_PLOT_HISTO);
      dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
      SetDataIndex(0);
      SetDataType(PMT_YIELD);
    }
    else if(HELTree){
      dMenuPlot->CheckEntry(M_PLOT_HISTO);
      dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
      SetDataIndex(0);
      SetDataType(PMT_ASYM);
    }

    PlotHistograms(); 
  }
  //End filling root data  


  
  //Start filling DB data

  if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){

    ClearDBData();      
   
  }

}

void QwGUIMainDetector::FillYieldPlots(Int_t det, Int_t dTInd)
{
//   Int_t events = 0;
  Int_t smplev = 0;

  TH1D *hst;
  TGraph *grp;

  QwGUIMainDetectorDataStructure dCurrentData = dCurrentYields[dTInd]->GetElements()[det];

  hst = new TH1D(Form("hst%02d_%d",det,dTInd),Form("%s.%s",MainDetectorPMTNames[det],dCurrentYields[dTInd]->GetName()),
		 1000,-5*dCurrentData.GetDataRMS(),+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(Form("%s.%s",MainDetectorPMTNames[det],dCurrentYields[dTInd]->GetName()));
  grp->SetName(Form("grp%02d_%d",det,dTInd));
  hst->SetDirectory(0);
  
  hst->GetXaxis()->SetTitle("RMS (mean subtracted) [V]");
  hst->GetXaxis()->CenterTitle();
  hst->GetXaxis()->SetTitleSize(0.04);
  hst->GetXaxis()->SetLabelSize(0.04);
  hst->GetXaxis()->SetTitleOffset(1.25);
  hst->GetYaxis()->SetLabelSize(0.04);
  
  Int_t smplcnt = 0;
  for(int j = 0; j < dCurrentData.Length(); j++){
    if(dCurrentData.GetData(j) != 0){
      if(smplev == dCurrentData.GetEnsambleSize() && 
	 dCurrentData.GetNumEnsambles() > smplcnt+1){
	smplcnt++; 
	smplev = 0;
	// 		  printf("Det %d: Size = %d, smplcnt = %d\n",i,dCurrentDataSampleMean[det].size(), smplcnt);
      }
      hst->Fill(dCurrentData.GetData(j) - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMin(),dCurrentData.GetDataMax());
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
    
  
  YieldHistArray[dTInd].Add(hst);
  YieldGraphArray[dTInd].Add(grp);
  dCurrentYields[dTInd]->SetFFTTotalLength(dCurrentData.Length());

  
}

void QwGUIMainDetector::FillPMTAsymPlots(Int_t det, Int_t dTInd)
{
//   Int_t events = 0;
  Int_t smplev = 0;

  TH1D *hst;
  TGraph *grp;

  QwGUIMainDetectorDataStructure dCurrentData = dCurrentPMTAsyms[dTInd]->GetElements()[det];

  hst = new TH1D(Form("hst%02d_%d",det,dTInd),Form("%s.%s",MainDetectorPMTNames[det],dCurrentPMTAsyms[dTInd]->GetName()),
		 1000,-5*dCurrentData.GetDataRMS(),+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(Form("%s.%s",MainDetectorPMTNames[det],dCurrentPMTAsyms[dTInd]->GetName()));
  grp->SetName(Form("grp%02d_%d",det,dTInd));
  hst->SetDirectory(0);
  
  hst->GetXaxis()->SetTitle("RMS (mean subtracted) [V]");
  hst->GetXaxis()->CenterTitle();
  hst->GetXaxis()->SetTitleSize(0.04);
  hst->GetXaxis()->SetLabelSize(0.04);
  hst->GetXaxis()->SetTitleOffset(1.25);
  hst->GetYaxis()->SetLabelSize(0.04);
  
  Int_t smplcnt = 0;
  for(int j = 0; j < dCurrentData.Length(); j++){
    if(dCurrentData.GetData(j) != 0){
      if(smplev == dCurrentData.GetEnsambleSize() && 
	 dCurrentData.GetNumEnsambles() > smplcnt+1){
	smplcnt++; 
	smplev = 0;
	// 		  printf("Det %d: Size = %d, smplcnt = %d\n",i,dCurrentDataSampleMean[det].size(), smplcnt);
      }
      hst->Fill(dCurrentData.GetData(j) - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMin(),dCurrentData.GetDataMax());
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
    
  
  PMTAsymHistArray[dTInd].Add(hst);
  PMTAsymGraphArray[dTInd].Add(grp);
  dCurrentPMTAsyms[dTInd]->SetFFTTotalLength(dCurrentData.Length());  
}


void QwGUIMainDetector::OnRemoveThisTab()
{

}

void QwGUIMainDetector::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWind")) return;
  
  printf("Received Messager From: %s\n",obj);
}

void QwGUIMainDetector::ClearDBData()
{

}

void QwGUIMainDetector::ClearRootData()
{
  TIter *next;

  TObject *obj;
  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){

    next = new TIter(YieldHistArray[n].MakeIterator());

    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(YieldGraphArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(PMTAsymHistArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(PMTAsymGraphArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(DetAsymHistArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(DetAsymGraphArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    YieldHistArray[n].Clear();
    YieldGraphArray[n].Clear();

    PMTAsymHistArray[n].Clear();
    PMTAsymGraphArray[n].Clear();

    DetAsymHistArray[n].Clear();
    DetAsymGraphArray[n].Clear();

//     delete dCurrentYields[n];
//     delete dCurrentPMTAsyms[n];
//     delete dCurrentDETAsyms[n];
  }  
}

void QwGUIMainDetector::ClearDFTData(Bool_t ALL)
{
  TObject *obj;
  TObjArray *objarray1;
  TObjArray *objarray2;  
  TIter *next;
  
  if(ALL){
    
    for(int n = 0; n < MAIN_DET_BLOCKIND; n++){

      next = new TIter(YieldProfileArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;


      next = new TIter(YieldDFTArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;

      YieldProfileArray[n].Clear();
      YieldDFTArray[n].Clear();
      
      next = new TIter(PMTAsymProfileArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;
      
      next = new TIter(PMTAsymDFTArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;

      PMTAsymProfileArray[n].Clear();
      PMTAsymDFTArray[n].Clear();
      
      next = new TIter(DetAsymProfileArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;
      
      next = new TIter(DetAsymDFTArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;

      DetAsymProfileArray[n].Clear();
      DetAsymDFTArray[n].Clear();
      
    }
  }
  else{

    if(GetDataType() == PMT_YIELD){
      objarray1 = &YieldProfileArray[GetDataIndex()];
      objarray2 = &YieldDFTArray[GetDataIndex()];
    }    
    if(GetDataType() == PMT_ASYM){
      objarray1 = &PMTAsymProfileArray[GetDataIndex()];
      objarray2 = &PMTAsymDFTArray[GetDataIndex()];
    }    
    if(GetDataType() == DET_ASYM){
      objarray1 = &DetAsymProfileArray[GetDataIndex()];
      objarray2 = &DetAsymDFTArray[GetDataIndex()];
    }    
      
    next = new TIter(objarray1->MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;
    
    next = new TIter(objarray2->MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    objarray1->Clear();
    objarray2->Clear();
  }

}

void QwGUIMainDetector::PlotHistograms()
{
  TObjArray *objarray;

  if(GetDataType() == PMT_YIELD){
    objarray = &YieldHistArray[GetDataIndex()];
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymHistArray[GetDataIndex()];
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymHistArray[GetDataIndex()];
  }    

  Int_t ind = 1;
//   Double_t rad = 1.0/sqrt(2.0)/2.0;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();

  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
//     gPad->SetPad(0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/16)-0.05,
// 		 0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/16)-0.05,
// 		 0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/16)+0.05,
// 		 0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/16)+0.05);
    ((TH1*)obj)->Draw("");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_HISTO);
}

void QwGUIMainDetector::PlotGraphs()
{
  TObjArray *objarray;

  if(GetDataType() == PMT_YIELD){
    objarray = &YieldGraphArray[GetDataIndex()];
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymGraphArray[GetDataIndex()];
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymGraphArray[GetDataIndex()];
  }    


  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(objarray->MakeIterator());
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

  SetPlotType(PLOT_TYPE_GRAPH);
}

void QwGUIMainDetector::PlotDFTProfile()
{
  TObjArray *objarray;

  if(GetDataType() == PMT_YIELD){
    objarray = &YieldProfileArray[GetDataIndex()];
    if(!dCurrentYields[GetDataIndex()]->IsFFTCalculated()) return;
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymProfileArray[GetDataIndex()];
    if(!dCurrentPMTAsyms[GetDataIndex()]->IsFFTCalculated()) return;
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymProfileArray[GetDataIndex()];
    if(!dCurrentDETAsyms[GetDataIndex()]->IsFFTCalculated()) return;
  }    

  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
    ((TProfile*)obj)->Draw();
//      ((TProfile*)obj)->ProjectionX("")->Draw();
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_PROFILE);
}

void QwGUIMainDetector::PlotDFT()
{
  TObjArray *objarray;


  if(GetDataType() == PMT_YIELD){
    objarray = &YieldDFTArray[GetDataIndex()];
//     if(!dCurrentYields[GetDataIndex()]->IsFFTCalculated()) 
      CalculateDFT();
    if(!dCurrentYields[GetDataIndex()]->IsFFTCalculated()) return;
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymDFTArray[GetDataIndex()];
//     if(!dCurrentPMTAsyms[GetDataIndex()]->IsFFTCalculated()) 
      CalculateDFT();
    if(!dCurrentPMTAsyms[GetDataIndex()]->IsFFTCalculated()) return;
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymDFTArray[GetDataIndex()];
//     if(!dCurrentDETAsyms[GetDataIndex()]->IsFFTCalculated()) 
      CalculateDFT();
    if(!dCurrentDETAsyms[GetDataIndex()]->IsFFTCalculated()) return;
  }    

  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    gPad->SetLogy();
    ((TH1*)obj)->Draw();    

    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_DFT);
  
}

void QwGUIMainDetector::CalculateDFT()
{
  QwGUIMainDetectorData *dCurrentData;
  QwGUIMainDetectorDataStructure *dCurrentDataStr;
  Int_t INDEX = 0;
  const char **Names = NULL;
  TObjArray *DFTArray;
  TObjArray *ProfArray;


  if(GetDataType() == PMT_YIELD){    
    dCurrentData    = dCurrentYields[GetDataIndex()];
    dCurrentDataStr = dCurrentYields[GetDataIndex()]->GetElements();
    INDEX = MAIN_PMT_INDEX;
    Names = MainDetectorPMTNames;
    DFTArray = &YieldDFTArray[GetDataIndex()];
    ProfArray = &YieldProfileArray[GetDataIndex()];

  }
  else if(GetDataType() == PMT_ASYM){
    dCurrentData    = dCurrentPMTAsyms[GetDataIndex()];
    dCurrentDataStr = dCurrentPMTAsyms[GetDataIndex()]->GetElements();
    INDEX = MAIN_PMT_INDEX;
    Names = MainDetectorPMTNames;
    DFTArray = &PMTAsymDFTArray[GetDataIndex()];
    ProfArray = &PMTAsymProfileArray[GetDataIndex()];
  }
  else if(GetDataType() == DET_ASYM){
    dCurrentData    = dCurrentDETAsyms[GetDataIndex()];
    dCurrentDataStr = dCurrentDETAsyms[GetDataIndex()]->GetElements();
    INDEX = MAIN_DET_INDEX;
    Names = MainDetectorNames;
    DFTArray = &DetAsymDFTArray[GetDataIndex()];
    ProfArray = &DetAsymProfileArray[GetDataIndex()];
  }
  else
    return;

  FFTOptions *fftopts = dCurrentData->GetFFTOptions();

  TProfile *prf = NULL;
  TH1D *hst = NULL;
  TH1D *fft = NULL;
  Int_t gc = 0;
  Int_t mc = 0;

  new QwGUIFFTWindowSelectionDialog(fClient->GetRoot(), GetMain(), "sdlg","QwGUIMainDetector",fftopts);

  if(fftopts->cancelFlag) return;
  if(fftopts->changeFlag || !dCurrentData->IsFFTCalculated()){

    if(fftopts->Length <= 0) return;
    if(fftopts->Start < 0 || fftopts->Start >= dCurrentDataStr[0].Length()) return;
    if(fftopts->Start + fftopts->Length > dCurrentDataStr[0].Length()) return;

    
    ClearDFTData(kFalse);
    for(int l = 0; l < INDEX; l++){      
      
      if(dCurrentDataStr[l].Length() > 0){

	prf = new TProfile(Form("%s_%s_prf",Names[l],dCurrentData->GetName()),
			   Form("%s_%s",Names[l],dCurrentData->GetName()),
			   fftopts->Length,1.0*fftopts->Start/SAMPLING_RATE,
			   1.0*(fftopts->Start+fftopts->Length)/SAMPLING_RATE);
	
	for(int n = fftopts->Start; n < fftopts->Length+fftopts->Start; n++){ 
	  if(dCurrentDataStr[l].GetData(n) != 0){
	    prf->Fill(1.0*n/SAMPLING_RATE,dCurrentDataStr[l].GetData(n)-dCurrentDataStr[l].GetDataMean());
// 	    if(l == 8)
// 	      printf("det %s data = %1.5e\n",Names[l],dCurrentDataStr[l].GetData(n));
	  }
	}      
	
	hst = prf->ProjectionX(Form("%s_%s-prj",Names[l],dCurrentData->GetName()));
	
	TH1 *fftmag = NULL;
	TVirtualFFT::SetTransform(0);
	fftmag = ((TH1*)hst)->FFT(fftmag,"MAG");
	fftmag->SetName(Form("%s_%s_fft-mag",Names[l],dCurrentData->GetName()));
	fftmag->SetTitle("Magnitude of the transform");
	
	fft = new TH1D(Form("%s_%s_fft",Names[l],dCurrentData->GetName()),
		       Form("%s %s FFT",Names[l],dCurrentData->GetName()),
		       (Int_t)(fftopts->Length/2.0)-1,0.0,(Int_t)(SAMPLING_RATE/2.0));
	
	for(int i = 0; i < (Int_t)(fftopts->Length/2); i++) 
	  fft->SetBinContent(i,1.25*fftmag->GetBinContent(i)/fftopts->Length);
	
	fft->GetXaxis()->SetTitle("Frequency [Hz]");
	fft->GetXaxis()->CenterTitle();
	fft->GetXaxis()->SetTitleSize(0.04);
	fft->GetXaxis()->SetLabelSize(0.04);
	fft->GetXaxis()->SetTitleOffset(1.25);
	fft->GetYaxis()->SetTitle("Amplitude [V]");
	fft->GetYaxis()->CenterTitle();
	fft->GetYaxis()->SetTitleSize(0.04);
	fft->GetYaxis()->SetLabelSize(0.04);
	fft->GetYaxis()->SetTitleOffset(1.25);
	
	DFTArray->Add(fft);
	ProfArray->Add(prf);
	delete hst;
	delete fftmag;
	prf->SetDirectory(0);
      }
    }    
  }
  dCurrentData->SetFFTCalculated(kTrue);
}

// void QwGUIMainDetector::CalculateDFT()
// {
//   TH1D *hst = NULL;
//   TH1D *fft = NULL;
//   TProfile *prof = NULL;
//   TObject *obj;

//   TIter next(ProfileArray.MakeIterator());
//   Int_t nbins = 0;

//   obj = next();

//   while(obj){
//     prof = ((TProfile*)obj);
//     hst = prof->ProjectionX(Form("%s-prfh",prof->GetName()));
//     TH1 *fftmag = NULL;
//     TVirtualFFT::SetTransform(0);
//     fftmag = ((TH1*)hst)->FFT(fftmag,"MAG");
//     fftmag->SetName(Form("%s-fftm",prof->GetName()));
//     fftmag->SetTitle("Magnitude of the transform");
    
//     nbins = fftmag->GetXaxis()->GetNbins();
//     fft = new TH1D(Form("%s_fft",prof->GetName()),
// 		   Form("%s FFT",prof->GetTitle()),
// 		   nbins/2.0,0,SAMPLING_RATE/2.0);

//     for(int i = 0; i < nbins/2; i++)
//       fft->SetBinContent(i,fftmag->GetBinContent(i)/nbins);
		   

//     DFTArray.Add(fft);
//     obj = next();
//   }
//   SetDFTCalculated(kTrue);
// }

// void QwGUIMainDetector::CalculateDFT()
// {
//   Int_t N = 0;                     //Current number of events/helicity
//                                    //states in the data.

//   //Double_t dt = 1.06e-3;         //Sampling interval in seconds,
//   Double_t dt = 1.0/SAMPLING_RATE; //corresponding to the current
//                                    //helicity reversal rate (see header).

//   Double_t T = 0;                  //Sampling time (run length) in seconds;

//   Double_t fmax = 1.0/(2*dt);      //Maximum frequency bandwidth set according
//                                    //to the Nyquist sampling criterion.  

//   Double_t df;                     //Frequency resolution= 1/T

//   Int_t M = (Int_t)(N/2);          //Number of frequency samples = fmax/df = N/2;

//   Double_t fValx  = 0;           //Temporary variable to hold the frequency coordinate
//   Double_t fValyR = 0;           //Temporary variable to hold the real component of the frequency amplitude coordinate
//   Double_t fValyI = 0;           //Temporary variable to hold the imaginary component of the frequency amplitude coordinate

//   Double_t tValx  = 0;           //Temporary variable to hold the time coordinate
//   Double_t tValy  = 0;           //Temporary variable to hold the time sample value coordinate

//   TH1D *hst;
//   TGraph *grp;

//   InitProgressDlg("DFT Calculation","Detectors","Frequency Bin",0, 
// 		  GraphArray.GetEntries(),M+1,0,2);

//   TObject *obj;
//   TIter next(GraphArray.MakeIterator());
//   obj = next();

//   Int_t gc = 0;
//   Int_t nc = 0;
//   Int_t mc = 0;


//   Int_t NEvents = 0;
//   Double_t val = 0.0;
//   Int_t retval= 0;

//   char Question[200];
//   memset(Question,'\0',sizeof(Question));
//   sprintf(Question,"There are %d events in the current file!\nProcessing all of them may take a long time!\nWould you like to enter a smaller number of samples to process?",((TGraph*)obj)->GetN());

//   if(((TGraph*)obj)->GetN() > 5000){
//     dNumberEntryDlg = new RNumberEntryDialog(fClient->GetRoot(), this, "dNumberEntryDlg","QwGUIMainDetector",
// 					     "DFT Processing",Question,&val,&retval,600,300);
//     if(retval && val >= 0) {NEvents = (Int_t)val;} else return;
//   }

//   while(obj){

//     grp = ((TGraph*)obj);
//     if(!NEvents) N = grp->GetN(); else N = NEvents;
//     T = N*dt;
//     df = 1.0/T;
//     M = (Int_t)(N/2);

//     hst = new TH1D(Form("%s_DFT",grp->GetName()),Form("%s DFT",grp->GetTitle()),M,0,fmax);

//     dProgrDlg->ResetRange(0,M,0);
//     mc = 0;
//     for(int j = 0; j < M; j++){  
//       fValx = j*df;
//       fValyR = 0;
//       fValyI = 0;
//       nc = 0;

//       for(int k = 0; k < N; k++){
//       #if ROOT_VERSION_CODE >= ROOT_VERSION(5,22,0)
//         // Only starting with ROOT version 5.22 does GetPoint return an Int_t
// 	// Ref: http://root.cern.ch/root/html522/TGraph.html#TGraph:GetPoint
// 	if(grp->GetPoint(k,tValx,tValy) > 0){
// 	  fValyR += tValy*TMath::Cos(2*TMath::Pi()*j*k/N);
// 	  fValyI += tValy*TMath::Sin(2*TMath::Pi()*j*k/N);
// 	}
// 	else{
// 	  fValyI += 0;
// 	  fValyR += 0;
// 	}
//       #else
// 	// Before ROOT version 5.22 GetPoint returns void,
// 	// but tValx and tValy are unchanged in case of problems
// 	// Ref: http://root.cern.ch/root/html520/TGraph.html#TGraph:GetPoint
// 	tValx = 0; tValy = 0;
//         grp->GetPoint(k,tValx,tValy);
// 	fValyR += tValy*TMath::Cos(2*TMath::Pi()*j*k/N);
// 	fValyI += tValy*TMath::Sin(2*TMath::Pi()*j*k/N);
//       #endif
//       } 
//       hst->SetBinContent(j+1,sqrt(fValyR*fValyR+fValyI*fValyI));

//       mc++;
//       if(dProcessHalt) {SetDFTCalculated(kFalse); return;};
//       IncreaseProgress(0,&mc,0,0,10,0);	
      
//     }
//     DFTArray.Add(hst);
//     obj = next();

//     gc++;
//     if(dProcessHalt) {SetDFTCalculated(kFalse); return;};
//     IncreaseProgress(&gc,0,0,1,0,0);	
//   }
//   if(dProgrDlg)
//     dProgrDlg->CloseWindow();

//   SetDFTCalculated(kTrue);
// }


void QwGUIMainDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  QwGUIMainDetectorDataStructure *dCurrentDataStr;
  TObjArray *HistArray;
  TObjArray *GraphArray;
  TObjArray *DFTArray;
  TObjArray *ProfileArray;

  if(event == kButton1Double){

    if(GetDataType() == PMT_YIELD){    
//       dCurrentData    = dCurrentYields[GetDataIndex()];
      dCurrentDataStr = dCurrentYields[GetDataIndex()]->GetElements();
//       INDEX = MAIN_PMT_INDEX;
//       Names = MainDetectorPMTNames;
      DFTArray = &YieldDFTArray[GetDataIndex()];
      ProfileArray = &YieldProfileArray[GetDataIndex()];
      HistArray = &YieldHistArray[GetDataIndex()];
      GraphArray = &YieldGraphArray[GetDataIndex()];
  }
  else if(GetDataType() == PMT_ASYM){
//     dCurrentData    = dCurrentPMTAsyms[GetDataIndex()];
    dCurrentDataStr = dCurrentPMTAsyms[GetDataIndex()]->GetElements();
//     INDEX = MAIN_PMT_INDEX;
//     Names = MainDetectorPMTNames;
    DFTArray = &PMTAsymDFTArray[GetDataIndex()];
    ProfileArray = &PMTAsymProfileArray[GetDataIndex()];
    HistArray = &PMTAsymHistArray[GetDataIndex()];
    GraphArray = &PMTAsymGraphArray[GetDataIndex()];

  }
  else if(GetDataType() == DET_ASYM){
//     dCurrentData    = dCurrentDETAsyms[GetDataIndex()];
    dCurrentDataStr = dCurrentDETAsyms[GetDataIndex()]->GetElements();
//     INDEX = MAIN_DET_INDEX;
//     Names = MainDetectorNames;
    DFTArray = &DetAsymDFTArray[GetDataIndex()];
    ProfileArray = &DetAsymProfileArray[GetDataIndex()];
    HistArray = &DetAsymHistArray[GetDataIndex()];
    GraphArray = &DetAsymGraphArray[GetDataIndex()];
  }
  else
    return;


    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= MAIN_PMT_INDEX){
      if(GetPlotType() == PLOT_TYPE_HISTO){

	QwGUIDataWindow *dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
							   "QwGUIMainDetector",(*HistArray)[pad-1]->GetTitle(), PT_HISTO_1D,
							   DDT_MD,600,400);
// 	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
// 						     GetNewWindowName(),"QwGUIMainDetector",
// 						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
	dDataWindow->SetStaticData((*HistArray)[pad-1],DataWindowArray.GetLast());

	Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
	Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
	Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");      

	dDataWindow->SetPlotTitle((char*)(*HistArray)[pad-1]->GetTitle());
	dDataWindow->DrawData(*((TH1D*)(*HistArray)[pad-1]));
	SetLogMessage(Form("Looking at histogram %s\n",(char*)(*HistArray)[pad-1]->GetTitle()),kTrue);
	
	return;
      }
      else if(GetPlotType() == PLOT_TYPE_GRAPH){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     (*GraphArray)[pad-1]->GetTitle(), PT_GRAPH,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)(*GraphArray)[pad-1]->GetTitle());
	dMiscWindow->SetPlotTitleX("Time [sec]");
	dMiscWindow->SetPlotTitleY("Amplitude [V]");
	dMiscWindow->SetPlotTitleOffset(1.25,1.8);
	dMiscWindow->SetAxisMin(((TGraph*)(*GraphArray)[pad-1])->GetXaxis()->GetXmin(),
				dCurrentDataStr[pad-1].GetDataMin());
	dMiscWindow->SetAxisMax(((TGraph*)(*GraphArray)[pad-1])->GetXaxis()->GetXmax(),
				dCurrentDataStr[pad-1].GetDataMax());
	dMiscWindow->SetLimitsFlag(kTrue);
	
	dMiscWindow->DrawData(*((TGraph*)(*GraphArray)[pad-1]));
	SetLogMessage(Form("Looking at graph %s\n",(char*)(*GraphArray)[pad-1]->GetTitle()),kTrue);
      }
      else if(GetPlotType() == PLOT_TYPE_DFT){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     (*DFTArray)[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)(*DFTArray)[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TH1D*)(*DFTArray)[pad-1]));
	SetLogMessage(Form("Looking at graph %s\n",(char*)(*DFTArray)[pad-1]->GetTitle()),kTrue);
      }
      else if(GetPlotType() == PLOT_TYPE_PROFILE){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     (*ProfileArray)[pad-1]->GetTitle(), PT_PROFILE,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)(*ProfileArray)[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TProfile*)(*ProfileArray)[pad-1]));
	SetLogMessage(Form("Looking at DFT profile %s\n",(char*)(*ProfileArray)[pad-1]->GetTitle()),kTrue);
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
	
      case M_PLOT_HISTO:
	dMenuPlot->UnCheckEntry(M_PLOT_GRAPH);
	dMenuPlot->CheckEntry(M_PLOT_HISTO);
	dMenuPlot->UnCheckEntry(M_PLOT_DFT);
	dMenuPlot->UnCheckEntry(M_PLOT_DFTPROF);
	PlotHistograms();
	break;

      case M_PLOT_GRAPH:
	dMenuPlot->CheckEntry(M_PLOT_GRAPH);
	dMenuPlot->UnCheckEntry(M_PLOT_HISTO);
	dMenuPlot->UnCheckEntry(M_PLOT_DFT);
	dMenuPlot->UnCheckEntry(M_PLOT_DFTPROF);
	PlotGraphs();
	break;

      case M_PLOT_DFT:
	dMenuPlot->UnCheckEntry(M_PLOT_GRAPH);
	dMenuPlot->UnCheckEntry(M_PLOT_HISTO);
	dMenuPlot->CheckEntry(M_PLOT_DFT);
	dMenuPlot->UnCheckEntry(M_PLOT_DFTPROF);
	PlotDFT();
	break;	

      case M_PLOT_DFTPROF:
	dMenuPlot->UnCheckEntry(M_PLOT_GRAPH);
	dMenuPlot->UnCheckEntry(M_PLOT_HISTO);
	dMenuPlot->UnCheckEntry(M_PLOT_DFT);
	dMenuPlot->CheckEntry(M_PLOT_DFTPROF);
	PlotDFTProfile();
	break;

	/*PMT_YIELD*************************************/

      case M_DATA_PMT_YIELD_SUM:
	dMenuData->CheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B1);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B2);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B3);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B4);
	dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
	SetDataIndex(0);
	SetDataType(PMT_YIELD);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_PMT_YIELD_B1:
	dMenuData->CheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B1);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B2);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B3);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_SUM);
	SetDataIndex(1);
	SetDataType(PMT_YIELD);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_PMT_YIELD_B2:
	dMenuData->CheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B1);
	dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B2);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B3);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_SUM);
	SetDataIndex(2);
	SetDataType(PMT_YIELD);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_PMT_YIELD_B3:
	dMenuData->CheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B1);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B2);
	dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B3);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_SUM);
	SetDataIndex(3);
	SetDataType(PMT_YIELD);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;
	
      case M_DATA_PMT_YIELD_B4:
	dMenuData->CheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B1);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B2);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_B3);
	dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B4);
	dMenuBlock1->UnCheckEntry(M_DATA_PMT_YIELD_SUM);
	SetDataIndex(4);
	SetDataType(PMT_YIELD);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;
	


	/*PMT_ASYM*************************************/

      case M_DATA_PMT_ASYM_SUM:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->CheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B1);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B2);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B3);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B4);
	dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
	SetDataIndex(0);
	SetDataType(PMT_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_PMT_ASYM_B1:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->CheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B1);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B2);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B3);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_SUM);
	SetDataIndex(1);
	SetDataType(PMT_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_PMT_ASYM_B2:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->CheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B1);
	dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B2);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B3);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_SUM);
	SetDataIndex(2);
	SetDataType(PMT_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_PMT_ASYM_B3:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->CheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B1);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B2);
	dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B3);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_SUM);
	SetDataIndex(3);
	SetDataType(PMT_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;
	
      case M_DATA_PMT_ASYM_B4:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->CheckEntryByData(dMenuBlock2);
	dMenuData->UnCheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B1);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B2);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_B3);
	dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B4);
	dMenuBlock2->UnCheckEntry(M_DATA_PMT_ASYM_SUM);
	SetDataIndex(4);
	SetDataType(PMT_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;



	/*DET_ASYM*************************************/

      case M_DATA_DET_ASYM_SUM:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->CheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B1);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B2);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B3);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B4);
	dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_SUM);
	SetDataIndex(0);
	SetDataType(DET_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_DET_ASYM_B1:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->CheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_B1);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B2);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B3);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_SUM);
	SetDataIndex(1);
	SetDataType(DET_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_DET_ASYM_B2:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->CheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B1);
	dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_B2);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B3);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_SUM);
	SetDataIndex(2);
	SetDataType(DET_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;

      case M_DATA_DET_ASYM_B3:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->CheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B1);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B2);
	dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_B3);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_SUM);
	SetDataIndex(3);
	SetDataType(DET_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
	break;
	
      case M_DATA_DET_ASYM_B4:
	dMenuData->UnCheckEntryByData(dMenuBlock1);
	dMenuData->UnCheckEntryByData(dMenuBlock2);
	dMenuData->CheckEntryByData(dMenuBlock3);
	dMenuData->UnCheckEntryByData(dMenuBlock4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B1);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B2);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_B3);
	dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_B4);
	dMenuBlock3->UnCheckEntry(M_DATA_DET_ASYM_SUM);
	SetDataIndex(4);
	SetDataType(DET_ASYM);
	if(dMenuPlot->IsEntryChecked(M_PLOT_GRAPH))
	  PlotGraphs();
	if(dMenuPlot->IsEntryChecked(M_PLOT_HISTO))
	  PlotHistograms();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFT))
	  PlotDFT();
	if(dMenuPlot->IsEntryChecked(M_PLOT_DFTPROF))
	  PlotDFTProfile();
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







// void QwGUIMainDetector::OnNewDataContainer(RDataContainer *cont)
// {

//   if(!cont) return;

//   TObject *obj;
//   TTree *tree;
//   TH1D *hst;
//   TGraph *grp;
//   //  TProfile *prf;
  
//   Double_t min = 5e9;
//   Double_t max = 0;
//   Double_t sum = 0;
//   Double_t sumsq = 0;
//   Double_t mean = 0;
//   Double_t rms = 0;
//   Int_t events = 0;
//   Int_t smplev = 0;
//   Double_t smplsum = 0;
//   Double_t smplsumsq = 0;
//   Double_t smplmean = 0;
//   Double_t smplrms = 0;
  

//   if(!strcmp(cont->GetDataName(),"ROOT") && dROOTCont){

//     ClearRootData();  
//     ClearDFTData();
//     obj = dROOTCont->ReadData("MPS_Tree");
//     SetDFTCalculated(kFalse);
//     if(obj){
//        if(obj->InheritsFrom("TTree")){
// 	 tree = (TTree*)obj->Clone();

// 	 fftopts.changeFlag  = kFalse;
// 	 fftopts.cancelFlag  = kFalse;
// 	 fftopts.Start       = 0;
// 	 fftopts.Length      = 0;
// 	 fftopts.TotalLength = 0;
	
// 	 for(int i = 0; i < MAIN_DET_INDEX; i++){

// 	  if ( tree -> FindLeaf(Form("%s.hw_sum_raw",MainDetectorPMTNames[i]))) 
//  	  {
// 	    min = 5e9;
// 	    max = 0;
// 	    sum = 0;  
// 	    sumsq = 0;
// 	    smplsum = 0;  
// 	    smplsumsq = 0;
// 	    events = 0;
// 	    smplev = 0;

// 	    tree->Draw(Form("%s.hw_sum_raw",MainDetectorPMTNames[i]),"","goff",tree->GetEntries(),0);

// 	    for(int j = 0; j < tree->GetEntries(); j++){
// 	      dCurrentData[i].push_back(tree->GetV1()[j]*76.3e-6/480);// + 4.0*TMath::Sin(2*TMath::Pi()*(j+1)*1.1e-3*20*(i+1)));

// 	      if(dCurrentData[i].at(j) != 0){
// 		if(dCurrentData[i].at(j) < min) min = dCurrentData[i].at(j);
// 		if(dCurrentData[i].at(j) > max) max = dCurrentData[i].at(j);
// 		sum += dCurrentData[i].at(j);
// 		smplsum += dCurrentData[i].at(j);
// 		sumsq += dCurrentData[i].at(j)*dCurrentData[i].at(j);
// 		smplsumsq += dCurrentData[i].at(j)*dCurrentData[i].at(j);
// 		events++;
// 		if(smplev == 1000){
// 		  smplmean = smplsum/smplev;
// 		  smplrms = sqrt(smplsumsq/smplev - smplmean*smplmean);
// 		  dCurrentDataSampleMean[i].push_back(smplmean);
// 		  dCurrentDataSampleError[i].push_back(smplrms/sqrt(smplev));
// 		  smplsum = 0;
// 		  smplsumsq = 0;
// 		  smplev = 0;
// 		}
// 		smplev++;
// 	      }
// // 	      else{
// // 		printf("Det %d: time = %f, ampl = %f\n",i,1.0*(j+1)/SAMPLING_RATE,dCurrentData[i].at(j));
// // 	      }
// 	    }

// 	    mean = sum/events;
// 	    rms = sqrt(sumsq/events - mean*mean);

// 	    dCurrentDataMin[i] = min;
// 	    dCurrentDataMax[i] = max;
// 	    dCurrentDataMean[i] = mean;
// 	    dCurrentDataRMS[i] = rms;

// 	    hst = new TH1D(Form("hst%02d",i),Form("%s.hw_sum_raw",MainDetectorPMTNames[i]),1000,-4*rms,+4*rms);
// 	    grp = new TGraph();
// 	    grp->SetTitle(Form("%s.hw_sum_raw",MainDetectorPMTNames[i]));
// 	    grp->SetName(Form("grp%02d",i));
// 	    hst->SetDirectory(0);

// 	    hst->GetXaxis()->SetTitle("RMS (mean subtracted) [V]");
// 	    hst->GetXaxis()->CenterTitle();
// 	    hst->GetXaxis()->SetTitleSize(0.04);
// 	    hst->GetXaxis()->SetLabelSize(0.04);
// 	    hst->GetXaxis()->SetTitleOffset(1.25);
// 	    hst->GetYaxis()->SetLabelSize(0.04);
	    
// 	    smplev = 0;
// 	    Int_t smplcnt = 0;
// 	    for(int j = 0; j < tree->GetEntries(); j++){
// 	      if(dCurrentData[i].at(j) != 0){
// 		if(smplev == 1000 && dCurrentDataSampleMean[i].size() > smplcnt+1){
// 		  smplcnt++; 
// 		  smplev = 0;
// // 		  printf("Det %d: Size = %d, smplcnt = %d\n",i,dCurrentDataSampleMean[i].size(), smplcnt);
// 		}
// 		hst->Fill(dCurrentData[i].at(j) - dCurrentDataSampleMean[i].at(smplcnt));// mean);
// 		grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData[i].at(j));
// 		smplev++;
// 	      }
// 	    }
// 	    printf("Det %d finsihed\n",i);
// 	    grp->GetYaxis()->SetRangeUser(min,max);
// 	    grp->GetXaxis()->SetTitle("Time [sec]");
// 	    grp->GetXaxis()->CenterTitle();
// 	    grp->GetXaxis()->SetTitleSize(0.04);
// 	    grp->GetXaxis()->SetLabelSize(0.04);
// 	    grp->GetXaxis()->SetTitleOffset(1.25);
// 	    grp->GetYaxis()->SetTitle("Amplitude [V]");
// 	    grp->GetYaxis()->CenterTitle();
// 	    grp->GetYaxis()->SetTitleSize(0.04);
// 	    grp->GetYaxis()->SetLabelSize(0.04);
// 	    grp->GetYaxis()->SetTitleOffset(1.5);

// 	    HistArray.Add(hst);
// 	    GraphArray.Add(grp);
// 	  }
// 	}	
//       }
//     }

//     fftopts.TotalLength = tree->GetEntries();
//     PlotHistograms();
//   }

//   if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){

//     ClearDBData();      
   
//   }

// }



// void QwGUIMainDetector::CalculateDFT()
// {
//   if(dCurrentData[0].size() == 0) return;

//   if(!IsDFTCalculated()){

//     fftopts.changeFlag  = kFalse;
//     fftopts.cancelFlag  = kFalse;
//     fftopts.Start       = 0;
//     fftopts.Length      = 0;
//     fftopts.TotalLength = 0;
//   }

//   if(dMenuBlock1->IsEntryCheckEntry(M_DATA_PMT_YIELD_SUM);
//      fftopts.TotalLength = ;



//   TProfile *prf = NULL;
//   TH1D *hst = NULL;
//   TH1D *fft = NULL;
//   Int_t gc = 0;
//   Int_t mc = 0;

//   new QwGUIFFTWindowSelectionDialog(fClient->GetRoot(), GetMain(), "sdlg","QwGUIMainDetector",&fftopts);

//   if(fftopts.cancelFlag) return;
//   if(fftopts.changeFlag || !IsDFTCalculated()){

//     if(fftopts.Length <= 0) return;
//     if(fftopts.Start < 0 || fftopts.Start >= dCurrentData[0].size()) return;
//     if(fftopts.Start + fftopts.Length > dCurrentData[0].size()) return;

// //     InitProgressDlg("FFT Calculation","Detectors","Data",0,MAIN_DET_INDEX,fftopts.Length,0,2);
    
//     ClearDFTData();
//     for(int det = 0; det < MAIN_DET_INDEX; det++){      
      
//       prf = new TProfile(Form("prf%02d",det),Form("%s.hw_sum_raw",MainDetectorPMTNames[det]),
// 			 fftopts.Length,1.0*fftopts.Start/SAMPLING_RATE,1.0*(fftopts.Start+fftopts.Length)/SAMPLING_RATE);
      
// //       dProgrDlg->ResetRange(0,fftopts.Length,0);
//       mc = 0;
//       for(int n = fftopts.Start; n < fftopts.Length+fftopts.Start; n++){ 
// 	if(dCurrentData[det].at(n) != 0)
// 	  prf->Fill(1.0*n/SAMPLING_RATE,dCurrentData[det].at(n)-dCurrentDataMean[det]);
// // 	mc++;
// // 	if(dProcessHalt) {SetDFTCalculated(kFalse); return;};
// // 	IncreaseProgress(0,&mc,0,0,1000,0);	
//       }      

//       hst = prf->ProjectionX(Form("%s-prj",MainDetectorPMTNames[det]));

//       TH1 *fftmag = NULL;
//       TVirtualFFT::SetTransform(0);
//       fftmag = ((TH1*)hst)->FFT(fftmag,"MAG");
//       fftmag->SetName(Form("%s-fft-mag",MainDetectorPMTNames[det]));
//       fftmag->SetTitle("Magnitude of the transform");
      
//       fft = new TH1D(Form("%s_fft",MainDetectorPMTNames[det]),
// 		     Form("%s FFT",MainDetectorPMTNames[det]),
// 		     (Int_t)(fftopts.Length/2.0)-1,0.0,(Int_t)(SAMPLING_RATE/2.0));

//       for(int i = 0; i < (Int_t)(fftopts.Length/2); i++) fft->SetBinContent(i,1.25*fftmag->GetBinContent(i)/fftopts.Length);

//       fft->GetXaxis()->SetTitle("Frequency [Hz]");
//       fft->GetXaxis()->CenterTitle();
//       fft->GetXaxis()->SetTitleSize(0.04);
//       fft->GetXaxis()->SetLabelSize(0.04);
//       fft->GetXaxis()->SetTitleOffset(1.25);
//       fft->GetYaxis()->SetTitle("Amplitude [V]");
//       fft->GetYaxis()->CenterTitle();
//       fft->GetYaxis()->SetTitleSize(0.04);
//       fft->GetYaxis()->SetLabelSize(0.04);
//       fft->GetYaxis()->SetTitleOffset(1.25);

//       DFTArray.Add(fft);
//       ProfileArray.Add(prf);
//       delete hst;
//       delete fftmag;
//       prf->SetDirectory(0);

// //       gc++;
// //       if(dProcessHalt) {SetDFTCalculated(kFalse); return;};
// //       IncreaseProgress(&gc,0,0,1,0,0);	
//     }
// //     if(dProgrDlg)
// //       dProgrDlg->CloseWindow();
    
//   }
//   SetDFTCalculated(kTrue);
// }
