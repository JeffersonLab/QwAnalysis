#include <QwGUIMainDetector.h>

ClassImp(QwGUIMainDetector);

using namespace QwParityDB;

const char *QwGUIMainDetector::MainDetectorBlockTypesRaw[MAIN_DET_BLOCKIND] = 
  {"hw_sum_raw","block0_raw","block1_raw","block2_raw","block3_raw"};

const char *QwGUIMainDetector::MainDetectorBlockTypes[MAIN_DET_BLOCKIND] = 
  {"hw_sum","block0","block1","block2","block3"};


void QwGUIMainDetectorDataStructure::PushData(Double_t item)
{
  
  Data.push_back(item);
	      
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
  dYieldCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dNumberEntryDlg = NULL;

  MAIN_PMT_INDEX = 0;
  MAIN_DET_INDEX = 0;
  MAIN_DET_COMBIND = 0;
  MAIN_MSC_INDEX = 0;
  RemoveSelectedDataWindow();

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
    dCurrentCMBAsyms[n]->Clean();
    dCurrentCMBAsyms[n]->SetFFTOptions(opts);
    dCurrentMSCAsyms[n]->Clean();
    dCurrentMSCAsyms[n]->SetFFTOptions(opts);
    dCurrentMSCYields[n]->Clean();
    dCurrentMSCYields[n]->SetFFTOptions(opts);
  }

  SetYieldDataIndex(0);
  SetPMTAsymDataIndex(0);
  SetDETAsymDataIndex(0);
  SetCMBAsymDataIndex(0);
  SetMSCAsymDataIndex(0);
  SetMSCYieldDataIndex(0);
  SetSummaryDataIndex(0);
}

QwGUIMainDetector::~QwGUIMainDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dYieldCanvas)    delete dYieldCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  ClearRootData();
  ClearDFTData();
  ClearDBData();

  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){
    delete dCurrentYields[n];
    delete dCurrentPMTAsyms[n];
    delete dCurrentDETAsyms[n];
    delete dCurrentCMBAsyms[n];
    delete dCurrentMSCAsyms[n];
    delete dCurrentMSCYields[n];
  }

  TObject *obj;
  TIter *next = new TIter(DataWindowArray.MakeIterator());
  
  obj = next->Next();
  while(obj){    
    delete obj;
    obj = next->Next();
  }
  delete next;

  DataWindowArray.Clear();
  
  RemoveThisTab(this);
  IsClosing(GetName());

}

void QwGUIMainDetector::MakeCurrentModeTabs()
{

  //*** Yield Tab *****************************************************************

  TGCompositeFrame *dYieldTab = dMDTab->AddTab("Yields");
  dYieldFrame = new TGVerticalFrame(dYieldTab,10,10);
  
  dMenuPlot1 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot1->AddEntry("&Histogram", M_PLOT_HISTO_YIELD);
  dMenuPlot1->AddEntry("&Graph", M_PLOT_GRAPH_YIELD);
  dMenuPlot1->AddEntry("&DFT", M_PLOT_DFT_YIELD);
  dMenuPlot1->AddEntry("DFT &Seq", M_PLOT_DFTPROF_YIELD);

  dMenuBlock1 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock1->AddEntry("Block &1", M_DATA_PMT_YIELD_B1);
  dMenuBlock1->AddEntry("Block &2", M_DATA_PMT_YIELD_B2);
  dMenuBlock1->AddEntry("Block &3", M_DATA_PMT_YIELD_B3);
  dMenuBlock1->AddEntry("Block &4", M_DATA_PMT_YIELD_B4);
  dMenuBlock1->AddEntry("Block &Sum", M_DATA_PMT_YIELD_SUM);

  dMenuBar1 = new TGMenuBar(dYieldFrame, 1, 1, kHorizontalFrame);
  dMenuBar1->AddPopup("&Plot Type", dMenuPlot1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar1->AddPopup("&Data Block", dMenuBlock1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dYieldFrame->AddFrame(dMenuBar1, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock1->Associate(this);
  dMenuPlot1->Associate(this);

  dYieldCanvas   = new TRootEmbeddedCanvas("pYieldC", dYieldFrame,10, 10);     
  dYieldFrame->AddFrame(dYieldCanvas,dCnvLayout);
  dYieldFrame->Resize(GetWidth(),GetHeight());


  //*** End Yield Tab *****************************************************************



  //*** PMT Asym Tab *****************************************************************

  TGCompositeFrame *dPMTAsymTab = dMDTab->AddTab("PMT Asym");
  dPMTAsymFrame = new TGVerticalFrame(dPMTAsymTab,10,10);
  
  dMenuPlot2 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot2->AddEntry("&Histogram", M_PLOT_HISTO_PMT_ASYM);
  dMenuPlot2->AddEntry("&Graph", M_PLOT_GRAPH_PMT_ASYM);
  dMenuPlot2->AddEntry("&DFT", M_PLOT_DFT_PMT_ASYM);
  dMenuPlot2->AddEntry("DFT &Seq", M_PLOT_DFTPROF_PMT_ASYM);

  dMenuBlock2 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock2->AddEntry("Block &1", M_DATA_PMT_ASYM_B1);
  dMenuBlock2->AddEntry("Block &2", M_DATA_PMT_ASYM_B2);
  dMenuBlock2->AddEntry("Block &3", M_DATA_PMT_ASYM_B3);
  dMenuBlock2->AddEntry("Block &4", M_DATA_PMT_ASYM_B4);
  dMenuBlock2->AddEntry("Block &Sum", M_DATA_PMT_ASYM_SUM);

  dMenuBar2 = new TGMenuBar(dPMTAsymFrame, 1, 1, kHorizontalFrame);
  dMenuBar2->AddPopup("&Plot Type", dMenuPlot2, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar2->AddPopup("&Data Block", dMenuBlock2, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dPMTAsymFrame->AddFrame(dMenuBar2, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock2->Associate(this);
  dMenuPlot2->Associate(this);

  dPMTAsymCanvas   = new TRootEmbeddedCanvas("pPMTAC", dPMTAsymFrame,10, 10);     
  dPMTAsymFrame->AddFrame(dPMTAsymCanvas,dCnvLayout);
  dPMTAsymFrame->Resize(GetWidth(),GetHeight());


  //*** End PMT Asym Tab *****************************************************************


  //*** DET Asym Tab *****************************************************************

  TGCompositeFrame *dDETAsymTab = dMDTab->AddTab("DET Asym");
  dDETAsymFrame = new TGVerticalFrame(dDETAsymTab,10,10);
  
  dMenuPlot3 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot3->AddEntry("&Histogram", M_PLOT_HISTO_DET_ASYM);
  dMenuPlot3->AddEntry("&Graph", M_PLOT_GRAPH_DET_ASYM);
  dMenuPlot3->AddEntry("&DFT", M_PLOT_DFT_DET_ASYM);
  dMenuPlot3->AddEntry("DFT &Seq", M_PLOT_DFTPROF_DET_ASYM);

  dMenuBlock3 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock3->AddEntry("Block &1", M_DATA_DET_ASYM_B1);
  dMenuBlock3->AddEntry("Block &2", M_DATA_DET_ASYM_B2);
  dMenuBlock3->AddEntry("Block &3", M_DATA_DET_ASYM_B3);
  dMenuBlock3->AddEntry("Block &4", M_DATA_DET_ASYM_B4);
  dMenuBlock3->AddEntry("Block &Sum", M_DATA_DET_ASYM_SUM);

  dMenuBar3 = new TGMenuBar(dDETAsymFrame, 1, 1, kHorizontalFrame);
  dMenuBar3->AddPopup("&Plot Type", dMenuPlot3, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar3->AddPopup("&Data Block", dMenuBlock3, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dDETAsymFrame->AddFrame(dMenuBar3, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock3->Associate(this);
  dMenuPlot3->Associate(this);

  dDETAsymCanvas   = new TRootEmbeddedCanvas("pDETAC", dDETAsymFrame,10, 10);     
  dDETAsymFrame->AddFrame(dDETAsymCanvas,dCnvLayout);
  dDETAsymFrame->Resize(GetWidth(),GetHeight());


  //*** End DET Asym Tab *****************************************************************

  //*** CMB Asym Tab *****************************************************************

  TGCompositeFrame *dCMBAsymTab = dMDTab->AddTab("CMB Asym");
  dCMBAsymFrame = new TGVerticalFrame(dCMBAsymTab,10,10);
  
  dMenuPlot4 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot4->AddEntry("&Histogram", M_PLOT_HISTO_CMB_ASYM);
  dMenuPlot4->AddEntry("&Graph", M_PLOT_GRAPH_CMB_ASYM);
  dMenuPlot4->AddEntry("&DFT", M_PLOT_DFT_CMB_ASYM);
  dMenuPlot4->AddEntry("DFT &Seq", M_PLOT_DFTPROF_CMB_ASYM);

  dMenuBlock4 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock4->AddEntry("Block &1", M_DATA_CMB_ASYM_B1);
  dMenuBlock4->AddEntry("Block &2", M_DATA_CMB_ASYM_B2);
  dMenuBlock4->AddEntry("Block &3", M_DATA_CMB_ASYM_B3);
  dMenuBlock4->AddEntry("Block &4", M_DATA_CMB_ASYM_B4);
  dMenuBlock4->AddEntry("Block &Sum", M_DATA_CMB_ASYM_SUM);

  dMenuBar4 = new TGMenuBar(dCMBAsymFrame, 1, 1, kHorizontalFrame);
  dMenuBar4->AddPopup("&Plot Type", dMenuPlot4, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar4->AddPopup("&Data Block", dMenuBlock4, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dCMBAsymFrame->AddFrame(dMenuBar4, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock4->Associate(this);
  dMenuPlot4->Associate(this);

  dCMBAsymCanvas   = new TRootEmbeddedCanvas("pCMBAC", dCMBAsymFrame,10, 10);     
  dCMBAsymFrame->AddFrame(dCMBAsymCanvas,dCnvLayout);
  dCMBAsymFrame->Resize(GetWidth(),GetHeight());


  //*** End CMB Asym Tab *****************************************************************

  //*** MSC Asym Tab *****************************************************************

  TGCompositeFrame *dMSCAsymTab = dMDTab->AddTab("MSC Asym");
  dMSCAsymFrame = new TGVerticalFrame(dMSCAsymTab,10,10);
  
  dMenuPlot5 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot5->AddEntry("&Histogram", M_PLOT_HISTO_MSC_ASYM);
  dMenuPlot5->AddEntry("&Graph", M_PLOT_GRAPH_MSC_ASYM);
  dMenuPlot5->AddEntry("&DFT", M_PLOT_DFT_MSC_ASYM);
  dMenuPlot5->AddEntry("DFT &Seq", M_PLOT_DFTPROF_MSC_ASYM);

  dMenuBlock5 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock5->AddEntry("Block &1", M_DATA_MSC_ASYM_B1);
  dMenuBlock5->AddEntry("Block &2", M_DATA_MSC_ASYM_B2);
  dMenuBlock5->AddEntry("Block &3", M_DATA_MSC_ASYM_B3);
  dMenuBlock5->AddEntry("Block &4", M_DATA_MSC_ASYM_B4);
  dMenuBlock5->AddEntry("Block &Sum", M_DATA_MSC_ASYM_SUM);

  dMenuBar5 = new TGMenuBar(dMSCAsymFrame, 1, 1, kHorizontalFrame);
  dMenuBar5->AddPopup("&Plot Type", dMenuPlot5, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar5->AddPopup("&Data Block", dMenuBlock5, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dMSCAsymFrame->AddFrame(dMenuBar5, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock5->Associate(this);
  dMenuPlot5->Associate(this);

  dMSCAsymCanvas   = new TRootEmbeddedCanvas("pMSCAC", dMSCAsymFrame,10, 10);     
  dMSCAsymFrame->AddFrame(dMSCAsymCanvas,dCnvLayout);
  dMSCAsymFrame->Resize(GetWidth(),GetHeight());


  //*** End MSC Asym Tab *****************************************************************


  //*** MSC Yield Tab *****************************************************************

  TGCompositeFrame *dMSCYieldTab = dMDTab->AddTab("MSC Yield");
  dMSCYieldFrame = new TGVerticalFrame(dMSCYieldTab,10,10);
  
  dMenuPlot6 = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot6->AddEntry("&Histogram", M_PLOT_HISTO_MSC_YIELD);
  dMenuPlot6->AddEntry("&Graph", M_PLOT_GRAPH_MSC_YIELD);
  dMenuPlot6->AddEntry("&DFT", M_PLOT_DFT_MSC_YIELD);
  dMenuPlot6->AddEntry("DFT &Seq", M_PLOT_DFTPROF_MSC_YIELD);

  dMenuBlock6 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock6->AddEntry("Block &1", M_DATA_MSC_YIELD_B1);
  dMenuBlock6->AddEntry("Block &2", M_DATA_MSC_YIELD_B2);
  dMenuBlock6->AddEntry("Block &3", M_DATA_MSC_YIELD_B3);
  dMenuBlock6->AddEntry("Block &4", M_DATA_MSC_YIELD_B4);
  dMenuBlock6->AddEntry("Block &Sum", M_DATA_MSC_YIELD_SUM);

  dMenuBar6 = new TGMenuBar(dMSCYieldFrame, 1, 1, kHorizontalFrame);
  dMenuBar6->AddPopup("&Plot Type", dMenuPlot6, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar6->AddPopup("&Data Block", dMenuBlock6, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dMSCYieldFrame->AddFrame(dMenuBar6, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock6->Associate(this);
  dMenuPlot6->Associate(this);

  dMSCYieldCanvas   = new TRootEmbeddedCanvas("pMSCYL", dMSCYieldFrame,10, 10);     
  dMSCYieldFrame->AddFrame(dMSCYieldCanvas,dCnvLayout);
  dMSCYieldFrame->Resize(GetWidth(),GetHeight());


  //*** End MSC Yield Tab *****************************************************************

  //*** SUM Tab *****************************************************************

  TGCompositeFrame *dSUMTab = dMDTab->AddTab("Summary");
  dSUMFrame = new TGVerticalFrame(dSUMTab,10,10);
  
//   dMenuPlot7 = new TGPopupMenu(fClient->GetRoot());
//   dMenuPlot7->AddEntry("&Histogram", M_PLOT_HISTO_SUM);
//   dMenuPlot7->AddEntry("&Graph", M_PLOT_GRAPH_SUM);
//   dMenuPlot7->AddEntry("&DFT", M_PLOT_DFT_SUM);
//   dMenuPlot7->AddEntry("DFT &Seq", M_PLOT_DFTPROF_SUM);

  dMenuBlock7 = new TGPopupMenu(fClient->GetRoot());
  dMenuBlock7->AddEntry("Block &1", M_DATA_SUM_B1);
  dMenuBlock7->AddEntry("Block &2", M_DATA_SUM_B2);
  dMenuBlock7->AddEntry("Block &3", M_DATA_SUM_B3);
  dMenuBlock7->AddEntry("Block &4", M_DATA_SUM_B4);
  dMenuBlock7->AddEntry("Block &Sum", M_DATA_SUM_SUM);

  dMenuBar7 = new TGMenuBar(dSUMFrame, 1, 1, kHorizontalFrame);
//   dMenuBar7->AddPopup("&Plot Type", dMenuPlot7, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  dMenuBar7->AddPopup("&Data Block", dMenuBlock7, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

  dSUMFrame->AddFrame(dMenuBar7, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
  dMenuBlock7->Associate(this);
//   dMenuPlot7->Associate(this);

  dSUMCanvas   = new TRootEmbeddedCanvas("pSUMYL", dSUMFrame,10, 10);     
  dSUMFrame->AddFrame(dSUMCanvas,dCnvLayout);
  dSUMFrame->Resize(GetWidth(),GetHeight());


  //*** End SUM  Tab *****************************************************************



  dYieldTab->AddFrame(dYieldFrame,dTabLayout);
  dPMTAsymTab->AddFrame(dPMTAsymFrame,dTabLayout);
  dDETAsymTab->AddFrame(dDETAsymFrame,dTabLayout);
  dCMBAsymTab->AddFrame(dCMBAsymFrame,dTabLayout);
  dMSCAsymTab->AddFrame(dMSCAsymFrame,dTabLayout);
  dMSCYieldTab->AddFrame(dMSCYieldFrame,dTabLayout);
  dSUMTab->AddFrame(dSUMFrame,dTabLayout);

  dYieldCanvas->GetCanvas()->SetBorderMode(0);
  dYieldCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  				"QwGUIMainDetector",
  				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  dPMTAsymCanvas->GetCanvas()->SetBorderMode(0);
  dPMTAsymCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				       "QwGUIMainDetector",
				       this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  dDETAsymCanvas->GetCanvas()->SetBorderMode(0);
  dDETAsymCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				       "QwGUIMainDetector",
				       this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  dCMBAsymCanvas->GetCanvas()->SetBorderMode(0);
  dCMBAsymCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				       "QwGUIMainDetector",
				       this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  dMSCAsymCanvas->GetCanvas()->SetBorderMode(0);
  dMSCAsymCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				       "QwGUIMainDetector",
				       this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  dMSCYieldCanvas->GetCanvas()->SetBorderMode(0);
  dMSCYieldCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				       "QwGUIMainDetector",
				       this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  dSUMCanvas->GetCanvas()->SetBorderMode(0);
  dSUMCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				   "QwGUIMainDetector",
				   this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");


  TCanvas *mc = dYieldCanvas->GetCanvas();
  mc->Divide(4,4);

  mc = dPMTAsymCanvas->GetCanvas();
  mc->Divide(4,4);

  mc = dDETAsymCanvas->GetCanvas();
  mc->Divide(2,4);

  Double_t rem = 100.0;
  Int_t div = MAIN_DET_COMBIND;
  Int_t row = 1;
  Int_t col = 1;
  while(rem){
    div--;
    rem = MAIN_DET_COMBIND%div;
  }
  if(div == 1){
    rem = 100.0;
    div = MAIN_DET_COMBIND+1;
    while(rem){
      div--;
      rem = MAIN_DET_COMBIND%div;
    }
  }    
  row = (Int_t)(MAIN_DET_COMBIND/div);
  col = div;

  mc = dCMBAsymCanvas->GetCanvas();
  mc->Divide(row,div);

  mc = dMSCAsymCanvas->GetCanvas();
  mc->Divide(2,4);

  mc = dMSCYieldCanvas->GetCanvas();
  mc->Divide(2,4);

  mc = dSUMCanvas->GetCanvas();
  mc->Divide(2,3);

  dMenuPlot1->DisableEntry(M_PLOT_HISTO_YIELD);
  dMenuPlot1->DisableEntry(M_PLOT_GRAPH_YIELD);
  dMenuPlot1->DisableEntry(M_PLOT_DFT_YIELD);
  dMenuPlot1->DisableEntry(M_PLOT_DFTPROF_YIELD);

  dMenuPlot2->DisableEntry(M_PLOT_HISTO_PMT_ASYM);
  dMenuPlot2->DisableEntry(M_PLOT_GRAPH_PMT_ASYM);
  dMenuPlot2->DisableEntry(M_PLOT_DFT_PMT_ASYM);
  dMenuPlot2->DisableEntry(M_PLOT_DFTPROF_PMT_ASYM);

  dMenuPlot3->DisableEntry(M_PLOT_HISTO_DET_ASYM);
  dMenuPlot3->DisableEntry(M_PLOT_GRAPH_DET_ASYM);
  dMenuPlot3->DisableEntry(M_PLOT_DFT_DET_ASYM);
  dMenuPlot3->DisableEntry(M_PLOT_DFTPROF_DET_ASYM);

  dMenuPlot4->DisableEntry(M_PLOT_HISTO_CMB_ASYM);
  dMenuPlot4->DisableEntry(M_PLOT_GRAPH_CMB_ASYM);
  dMenuPlot4->DisableEntry(M_PLOT_DFT_CMB_ASYM);
  dMenuPlot4->DisableEntry(M_PLOT_DFTPROF_CMB_ASYM);

  dMenuPlot5->DisableEntry(M_PLOT_HISTO_MSC_ASYM);
  dMenuPlot5->DisableEntry(M_PLOT_GRAPH_MSC_ASYM);
  dMenuPlot5->DisableEntry(M_PLOT_DFT_MSC_ASYM);
  dMenuPlot5->DisableEntry(M_PLOT_DFTPROF_MSC_ASYM);

  dMenuPlot6->DisableEntry(M_PLOT_HISTO_MSC_YIELD);
  dMenuPlot6->DisableEntry(M_PLOT_GRAPH_MSC_YIELD);
  dMenuPlot6->DisableEntry(M_PLOT_DFT_MSC_YIELD);
  dMenuPlot6->DisableEntry(M_PLOT_DFTPROF_MSC_YIELD);

//   dMenuPlot7->DisableEntry(M_PLOT_HISTO_SUM);
//   dMenuPlot7->DisableEntry(M_PLOT_GRAPH_SUM);
//   dMenuPlot7->DisableEntry(M_PLOT_DFT_SUM);
//   dMenuPlot7->DisableEntry(M_PLOT_DFTPROF_SUM);

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

  dMenuBlock5->DisableEntry(M_DATA_MSC_ASYM_B1); 
  dMenuBlock5->DisableEntry(M_DATA_MSC_ASYM_B2); 
  dMenuBlock5->DisableEntry(M_DATA_MSC_ASYM_B3); 
  dMenuBlock5->DisableEntry(M_DATA_MSC_ASYM_B4); 
  dMenuBlock5->DisableEntry(M_DATA_MSC_ASYM_SUM);

  dMenuBlock6->DisableEntry(M_DATA_MSC_YIELD_B1); 
  dMenuBlock6->DisableEntry(M_DATA_MSC_YIELD_B2); 
  dMenuBlock6->DisableEntry(M_DATA_MSC_YIELD_B3); 
  dMenuBlock6->DisableEntry(M_DATA_MSC_YIELD_B4); 
  dMenuBlock6->DisableEntry(M_DATA_MSC_YIELD_SUM);

  dMenuBlock7->DisableEntry(M_DATA_SUM_B1); 
  dMenuBlock7->DisableEntry(M_DATA_SUM_B2); 
  dMenuBlock7->DisableEntry(M_DATA_SUM_B3); 
  dMenuBlock7->DisableEntry(M_DATA_SUM_B4); 
  dMenuBlock7->DisableEntry(M_DATA_SUM_SUM);

  dMDTab->MapSubwindows(); 
  dMDTab->Layout();
}

void QwGUIMainDetector::MakeTrackingModeTabs()
{

}

void QwGUIMainDetector::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);


  dTabLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY,
				 2, 2, 5, 1);

  dTabFrame = new TGHorizontalFrame(this,10,10);

  dMDTab = new TGTab(dTabFrame,10,10);
  dMDTab->Associate(this);


  dTabFrame->AddFrame(dMDTab,new TGLayoutHints(kLHintsLeft | kLHintsTop |
					       kLHintsExpandX | kLHintsExpandY));

  AddFrame(dTabFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop |
				     kLHintsExpandX | kLHintsExpandY));
}

#ifndef ROOTCINTMODE

Int_t QwGUIMainDetector::LoadCurrentModeChannelMap()
{
  TString mapfile = Form("%s/setupfiles/qweak_maindet.map",gSystem->Getenv("QWSCRATCH"));
  TString varname, varvalue;
  TString modtype, dettype, namech, nameofcombinedchan;
  Int_t modnum, channum, combinedchans;
  std::vector<TString> combinedchannelnames;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //  This is a declaration line.  Do nothing ...
    }
    else {
      //  Break this line into tokens to process it.
      modtype   = mapstr.GetNextToken(", ").c_str();	// module type
      if (modtype == "VQWK"){
	modnum    = (atol(mapstr.GetNextToken(", ").c_str()));	//slot number
	channum   = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
	dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
	namech.ToLower();
	if(modnum == 4){
	  MainDetectorMscNames.push_back(namech);
	  MAIN_MSC_INDEX++;
	}
	if(modnum < 4){
	  MainDetectorPMTNames.push_back(namech);
	  MAIN_PMT_INDEX++;
	}
	if(modnum == 2){
	  namech.ReplaceAll("neg",3,"",0);
	  //namech.ReplaceAll("pos",3,"",0);
	  MainDetectorNames.push_back(namech);
	  MAIN_DET_INDEX++;
	}
        
      }
      else if (modtype == "VPMT"){
	channum       = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
	combinedchans = (atol(mapstr.GetNextToken(", ").c_str()));	//number of combined channels
	dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
	namech.ToLower();
	combinedchannelnames.clear();

	MainDetectorCombinationNames.push_back(namech);    
	MAIN_DET_COMBIND++;	
	
      }
      else
	return 0;
    }
  }

  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){
    dCurrentYields[n] = new QwGUIMainDetectorData(MAIN_PMT_INDEX);
    dCurrentYields[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentPMTAsyms[n] = new QwGUIMainDetectorData(MAIN_PMT_INDEX);
    dCurrentPMTAsyms[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentDETAsyms[n] = new QwGUIMainDetectorData(MAIN_DET_INDEX);
    dCurrentDETAsyms[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentCMBAsyms[n] = new QwGUIMainDetectorData(MAIN_DET_COMBIND);
    dCurrentCMBAsyms[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentMSCAsyms[n] = new QwGUIMainDetectorData(MAIN_MSC_INDEX);
    dCurrentMSCAsyms[n]->SetName(MainDetectorBlockTypes[n]);
    dCurrentMSCYields[n] = new QwGUIMainDetectorData(MAIN_MSC_INDEX);
    dCurrentMSCYields[n]->SetName(MainDetectorBlockTypes[n]);
  }

  NewDataInit();
  ClearRootData();
  ClearDFTData();
  ClearDBData();

  return 1;
}

Int_t QwGUIMainDetector::LoadTrackingModeChannelMap()
{

  return 1;
}

#endif

void QwGUIMainDetector::OnReceiveMessage(char *msg)
{
  TObject *obj = NULL;
  Int_t ind = 0;
  TString message = msg;
//   char *ptr = NULL;
  if(message.Contains("dDataWindow")){

    if(message.Contains("Add to")){

      message.ReplaceAll("Add to dDataWindow_",7,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
	ind = DataWindowArray.IndexOf(obj);
// 	printf("Object = %s, %d\n",obj->GetName(),ind);
	SetSelectedDataWindow(ind);
      }
      
//       message.ReplaceAll("Add to dDataWindow_",19,"",0);
//       printf("Selected graph = %s or %d = %s\n",message.Data(),message.Atoi(),
// 	     ((RSDataWindow*)DataWindowArray[message.Atoi()-1])->GetObjectName());
      
    }
    else if(message.Contains("Don't add to")){


      message.ReplaceAll("Don't add to dDataWindow_",13,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
// 	ind = DataWindowArray.IndexOf(obj);
// 	printf("Object = %s, %d\n",obj->GetName(),ind);
	RemoveSelectedDataWindow();
      }

//       message.ReplaceAll("Don't add to dDataWindow_",25,"",0);
//       printf("De-selected graph = %s or %d = %s\n",message.Data(),message.Atoi(),
// 	     ((RSDataWindow*)DataWindowArray[message.Atoi()-1])->GetObjectName());
      
    }
  }
}

QwGUIDataWindow *QwGUIMainDetector::GetSelectedDataWindow()
{
  if(dSelectedDataWindow < 0 || dSelectedDataWindow > DataWindowArray.GetLast()) return NULL;
  
  return (QwGUIDataWindow*)DataWindowArray[dSelectedDataWindow];
}  


void QwGUIMainDetector::OnObjClose(char *obj)
{
  if(!obj) return;
  TString name = obj;

  if(name.Contains("dDataWindow")){
    QwGUIDataWindow* window = (QwGUIDataWindow*)DataWindowArray.Remove(DataWindowArray.FindObject(obj));    
    if(window == GetSelectedDataWindow()) { RemoveSelectedDataWindow();}
    printf("Removing %s\n",name.Data());
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


void QwGUIMainDetector::GetCurrentModeData(TTree *MPSTree, TTree *HELTree)
{
  if(!MPSTree && !HELTree) return;
  Int_t events = 0;
  Int_t bl = 0;
  Int_t det = 0;

  LoadCurrentModeChannelMap();
  MakeCurrentModeTabs();
  
  //Get rid of old data
  NewDataInit();
  ClearRootData();    
  ClearDFTData();
  
  QwGUIMainDetectorDataStructure *CurrentPMTYield  = NULL;
  QwGUIMainDetectorDataStructure *CurrentPMTAsym   = NULL;
  QwGUIMainDetectorDataStructure *CurrentDETAsym   = NULL;
  QwGUIMainDetectorDataStructure *CurrentCMBAsym   = NULL;
  QwGUIMainDetectorDataStructure *CurrentMSCAsym   = NULL;
  QwGUIMainDetectorDataStructure *CurrentMSCYield  = NULL;
  
  //cycle through the main detectors and possible combinations thereof
  
  InitProgressDlg("Detector Event Fill","Block","Detector",0,kTrue,4,dCurrentYields[0]->GetNumElements(),0,2);
  
  bl = 0;
  IncreaseProgress(&bl,0,0,1,0,0);
  for(int n = 0; n < MAIN_DET_BLOCKIND; n++){
    
    CurrentPMTYield  = dCurrentYields[n]->GetElements();
    CurrentPMTAsym   = dCurrentPMTAsyms[n]->GetElements();
    CurrentDETAsym   = dCurrentDETAsyms[n]->GetElements();
    CurrentCMBAsym   = dCurrentCMBAsyms[n]->GetElements();
    CurrentMSCAsym   = dCurrentMSCAsyms[n]->GetElements();
    CurrentMSCYield  = dCurrentMSCYields[n]->GetElements();
    
    dProgrDlg->ResetRange(0,dCurrentYields[n]->GetNumElements(),0);
    det = 1;
    for(int i = 0; i < dCurrentYields[n]->GetNumElements(); i++){
      
      if (MPSTree && MPSTree -> FindBranch(Form("%s",MainDetectorPMTNames[i].Data()))) {
	MPSTree->Draw(Form("%s.%s",MainDetectorPMTNames[i].Data(),dCurrentYields[n]->GetName()),"",
		      "goff",MPSTree->GetEntries(),0);
	
	CurrentPMTYield[i].EnsambleReset();
	events = 0;
	
	for(int j = 0; j < MPSTree->GetEntries(); j++){
	  
	  CurrentPMTYield[i].PushData(MPSTree->GetV1()[j]*76.3e-6/480);	      
	  events++;
	}
	CurrentPMTYield[i].CalculateStats();
	CurrentPMTYield[i].SetName(Form("%s.%s Yield",MainDetectorPMTNames[i].Data(),
					dCurrentYields[n]->GetName()));
	FillYieldPlots(i,n);
      }
      
      if ( HELTree && HELTree -> FindBranch(Form("asym_%s",MainDetectorPMTNames[i].Data()))) {
	HELTree->Draw(Form("asym_%s.%s",MainDetectorPMTNames[i].Data(),dCurrentPMTAsyms[n]->GetName()),"",
		      "goff",HELTree->GetEntries(),0);
	
	CurrentPMTAsym[i].EnsambleReset();
	events = 0;
	
	for(int j = 0; j < HELTree->GetEntries(); j++){
	  
	  CurrentPMTAsym[i].PushData(HELTree->GetV1()[j]);
	  events++;
	}
	CurrentPMTAsym[i].CalculateStats();
	CurrentPMTAsym[i].SetName(Form("%s.%s PMT Asy.",MainDetectorPMTNames[i].Data(),
				       dCurrentPMTAsyms[n]->GetName()));
	FillPMTAsymPlots(i,n);
      }
      
      det++;
      if(dProcessHalt) break;
      IncreaseProgress(0,&det,0,0,1,0);
    }
    
    for(int i = 0; i < dCurrentDETAsyms[n]->GetNumElements(); i++){
      
      CurrentDETAsym[i].EnsambleReset();
      events = 0;
      
      for(int j = 0; j < CurrentPMTAsym[i].Length(); j++){
	
	CurrentDETAsym[i].PushData(0.5*(CurrentPMTAsym[i].GetData(j)+CurrentPMTAsym[i+8].GetData(j)));
	events++;
      }
      CurrentDETAsym[i].CalculateStats();
      CurrentDETAsym[i].SetName(Form("%s.%s Detector Asy.",MainDetectorNames[i].Data(),
				     dCurrentDETAsyms[n]->GetName()));
      FillDETAsymPlots(i,n);
    }
    
    for(int i = 0; i < dCurrentCMBAsyms[n]->GetNumElements(); i++){
      
      if ( HELTree && HELTree -> FindBranch(Form("asym_%s",MainDetectorCombinationNames[i].Data()))) {
	
	HELTree->Draw(Form("asym_%s.%s",MainDetectorCombinationNames[i].Data(),dCurrentCMBAsyms[n]->GetName()),"",
		      "goff",HELTree->GetEntries(),0);
	
	
	CurrentCMBAsym[i].EnsambleReset();
	events = 0;
	
	for(int j = 0; j < HELTree->GetEntries(); j++){
	  
	  CurrentCMBAsym[i].PushData(HELTree->GetV1()[j]);
	  events++;
	}
	CurrentCMBAsym[i].CalculateStats();
	CurrentCMBAsym[i].SetName(Form("%s.%s Combination Asy.",MainDetectorCombinationNames[i].Data(),
				       dCurrentCMBAsyms[n]->GetName()));
	FillCMBAsymPlots(i,n);
	
      }
    }
    
    for(int i = 0; i < dCurrentMSCAsyms[n]->GetNumElements(); i++){
      
      if ( HELTree && HELTree -> FindBranch(Form("asym_%s",MainDetectorMscNames[i].Data()))) {
	
	HELTree->Draw(Form("asym_%s.%s",MainDetectorMscNames[i].Data(),dCurrentMSCAsyms[n]->GetName()),"",
		      "goff",HELTree->GetEntries(),0);
	
	
	CurrentMSCAsym[i].EnsambleReset();
	events = 0;
	
	for(int j = 0; j < HELTree->GetEntries(); j++){
	  
	  CurrentMSCAsym[i].PushData(HELTree->GetV1()[j]);
	  events++;
	}
	CurrentMSCAsym[i].CalculateStats();
	CurrentMSCAsym[i].SetName(Form("%s.%s Asymmetry",MainDetectorMscNames[i].Data(),
				       dCurrentMSCAsyms[n]->GetName()));
	FillMSCAsymPlots(i,n);
	
      }
    }
    
    for(int i = 0; i < dCurrentMSCYields[n]->GetNumElements(); i++){
      
      if ( MPSTree && MPSTree -> FindBranch(Form("%s",MainDetectorMscNames[i].Data()))) {
	
	MPSTree->Draw(Form("%s.%s",MainDetectorMscNames[i].Data(),dCurrentMSCYields[n]->GetName()),"",
		      "goff",MPSTree->GetEntries(),0);
	
	
	CurrentMSCYield[i].EnsambleReset();
	events = 0;
	
	for(int j = 0; j < MPSTree->GetEntries(); j++){
	  CurrentMSCYield[i].PushData(MPSTree->GetV1()[j]*76.3e-6/480);
	  events++;
	}
	CurrentMSCYield[i].CalculateStats();
	CurrentMSCYield[i].SetName(Form("%s.%s Yield",MainDetectorMscNames[i].Data(),
					dCurrentMSCYields[n]->GetName()));
	FillMSCYieldPlots(i,n);
	
      }
    }
    FillSummaryPlots(n);
    
    bl++;
    if(dProcessHalt) break;
    IncreaseProgress(&bl,0,0,1,0,0);
  }
  
  if(dProgrDlg)
    dProgrDlg->CloseWindow();
  
  if(MPSTree && MAIN_PMT_INDEX > 0){
    
    dMenuPlot1->EnableEntry(M_PLOT_HISTO_YIELD);
    dMenuPlot1->EnableEntry(M_PLOT_GRAPH_YIELD);
    dMenuPlot1->EnableEntry(M_PLOT_DFT_YIELD);
    
    dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B1);   
    dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B2);   
    dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B3);   
    dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_B4);   
    dMenuBlock1->EnableEntry(M_DATA_PMT_YIELD_SUM);
    
    dMenuPlot1->CheckEntry(M_PLOT_HISTO_YIELD);
    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
    SetYieldDataIndex(0);
    SetDataType(PMT_YIELD);
    PlotHistograms();
  }
  
  if(HELTree && MAIN_PMT_INDEX > 0){
    
    dMenuPlot2->EnableEntry(M_PLOT_HISTO_PMT_ASYM);
    dMenuPlot2->EnableEntry(M_PLOT_GRAPH_PMT_ASYM);
    dMenuPlot2->EnableEntry(M_PLOT_DFT_PMT_ASYM);
    
    dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B1); 
    dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B2); 
    dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B3); 
    dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_B4); 
    dMenuBlock2->EnableEntry(M_DATA_PMT_ASYM_SUM);
    
    dMenuPlot2->CheckEntry(M_PLOT_HISTO_PMT_ASYM);
    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
    SetPMTAsymDataIndex(0);
    SetDataType(PMT_ASYM);
    PlotHistograms(); 
  }
  
  if(HELTree && MAIN_DET_INDEX > 0){
    
    dMenuPlot3->EnableEntry(M_PLOT_HISTO_DET_ASYM);
    dMenuPlot3->EnableEntry(M_PLOT_GRAPH_DET_ASYM);
    dMenuPlot3->EnableEntry(M_PLOT_DFT_DET_ASYM);
    
    dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B1); 
    dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B2); 
    dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B3); 
    dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_B4); 
    dMenuBlock3->EnableEntry(M_DATA_DET_ASYM_SUM);
    
    dMenuPlot3->CheckEntry(M_PLOT_HISTO_DET_ASYM);
    dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_SUM);
    SetDETAsymDataIndex(0);
    SetDataType(DET_ASYM);
    PlotHistograms(); 
  }
  
  if(HELTree && MAIN_DET_COMBIND > 0){
    
    dMenuPlot4->EnableEntry(M_PLOT_HISTO_CMB_ASYM);
    dMenuPlot4->EnableEntry(M_PLOT_GRAPH_CMB_ASYM);
    dMenuPlot4->EnableEntry(M_PLOT_DFT_CMB_ASYM);
    
    dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B1); 
    dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B2); 
    dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B3); 
    dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_B4); 
    dMenuBlock4->EnableEntry(M_DATA_CMB_ASYM_SUM);
    
    dMenuPlot4->CheckEntry(M_PLOT_HISTO_CMB_ASYM);
    dMenuBlock4->CheckEntry(M_DATA_CMB_ASYM_SUM);
    SetCMBAsymDataIndex(0);
    SetDataType(CMB_ASYM);
    PlotHistograms(); 
  }
  
  if(HELTree && MAIN_MSC_INDEX > 0){
    
    dMenuPlot5->EnableEntry(M_PLOT_HISTO_MSC_ASYM);
    dMenuPlot5->EnableEntry(M_PLOT_GRAPH_MSC_ASYM);
    dMenuPlot5->EnableEntry(M_PLOT_DFT_MSC_ASYM);
    
    dMenuBlock5->EnableEntry(M_DATA_MSC_ASYM_B1); 
    dMenuBlock5->EnableEntry(M_DATA_MSC_ASYM_B2); 
    dMenuBlock5->EnableEntry(M_DATA_MSC_ASYM_B3); 
    dMenuBlock5->EnableEntry(M_DATA_MSC_ASYM_B4); 
    dMenuBlock5->EnableEntry(M_DATA_MSC_ASYM_SUM);
    
    dMenuPlot5->CheckEntry(M_PLOT_HISTO_MSC_ASYM);
    dMenuBlock5->CheckEntry(M_DATA_MSC_ASYM_SUM);
    SetMSCAsymDataIndex(0);
    SetDataType(MSC_ASYM);
    PlotHistograms(); 
    
    dMenuPlot6->EnableEntry(M_PLOT_HISTO_MSC_YIELD);
    dMenuPlot6->EnableEntry(M_PLOT_GRAPH_MSC_YIELD);
    dMenuPlot6->EnableEntry(M_PLOT_DFT_MSC_YIELD);
    
    dMenuBlock6->EnableEntry(M_DATA_MSC_YIELD_B1); 
    dMenuBlock6->EnableEntry(M_DATA_MSC_YIELD_B2); 
    dMenuBlock6->EnableEntry(M_DATA_MSC_YIELD_B3); 
    dMenuBlock6->EnableEntry(M_DATA_MSC_YIELD_B4); 
    dMenuBlock6->EnableEntry(M_DATA_MSC_YIELD_SUM);
    
    dMenuPlot6->CheckEntry(M_PLOT_HISTO_MSC_YIELD);
    dMenuBlock6->CheckEntry(M_DATA_MSC_YIELD_SUM);
    SetMSCAsymDataIndex(0);
    SetDataType(MSC_YIELD);
    PlotHistograms(); 
  }
  
  if(HELTree || MPSTree){
    
    dMenuBlock7->EnableEntry(M_DATA_SUM_B1);   
    dMenuBlock7->EnableEntry(M_DATA_SUM_B2);   
    dMenuBlock7->EnableEntry(M_DATA_SUM_B3);   
    dMenuBlock7->EnableEntry(M_DATA_SUM_B4);   
    dMenuBlock7->EnableEntry(M_DATA_SUM_SUM);
    
    dMenuBlock7->CheckEntry(M_DATA_SUM_SUM);
    SetSummaryDataIndex(0);
    SetDataType(SUMMARY);
    PlotGraphs(); 
    
    SetPlotType(PLOT_TYPE_HISTO,0);
    SetPlotType(PLOT_TYPE_HISTO,1);
    SetPlotType(PLOT_TYPE_HISTO,2);
    SetPlotType(PLOT_TYPE_HISTO,3);
    SetPlotType(PLOT_TYPE_HISTO,4);
    SetPlotType(PLOT_TYPE_HISTO,5);
    SetPlotType(PLOT_TYPE_GRAPH,6);
    
  }
  
  SetDataType(PMT_YIELD);
  
  //End filling root data    

}

void QwGUIMainDetector::GetTrackingModeData(TTree *tree)
{
  if(!tree) return;

  LoadTrackingModeChannelMap();
  MakeTrackingModeTabs();

  //etc ...
}

void QwGUIMainDetector::OnNewDataContainer(RDataContainer *cont)
{

  if(!cont) return;

  TObject *obj;
  TTree *MPSTree = NULL;
  TTree *HELTree = NULL;
  TTree *tree = NULL;

  if(!strcmp(cont->GetDataName(),"ROOT") && dROOTCont){
  //Start filling root data  

    //Current Mode
    obj = dROOTCont->ReadData("Mps_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	MPSTree = (TTree*)obj->Clone();
      }
    }
    obj = dROOTCont->ReadData("Hel_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	HELTree = (TTree*)obj->Clone();
      }
    }

    //Tracking Mode
    obj = dROOTCont->ReadData("tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	HELTree = (TTree*)obj->Clone();
      }
    }

    if(HELTree || MPSTree){
      GetCurrentModeData(MPSTree,HELTree);
    }
    else if(tree){
      GetTrackingModeData(tree);
    }


  }
  else if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){
    //Start filling DB data
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

  hst = new TH1D(Form("yield%02d_%d",det,dTInd),
		 dCurrentData.GetName(),
		 // 		 Form("%s.%s Yield",MainDetectorPMTNames[det].Data(),dCurrentYields[dTInd]->GetName()),
		 1000,dCurrentData.GetDataMean()-5*dCurrentData.GetDataRMS(),dCurrentData.GetDataMean()+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(dCurrentData.GetName());//Form("%s.%s Yield",MainDetectorPMTNames[det].Data(),dCurrentYields[dTInd]->GetName()));
  grp->SetName(Form("yield%02d_%d",det,dTInd));
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
      hst->Fill(dCurrentData.GetData(j));// - dCurrentData.GetEnsambleMean(smplcnt));// mean);
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

  hst = new TH1D(Form("pmtasym%02d_%d",det,dTInd),dCurrentData.GetName(),
		 //Form("%s.%s PMT Asymmetry",MainDetectorPMTNames[det].Data(),dCurrentPMTAsyms[dTInd]->GetName()),
		 1000,dCurrentData.GetDataMean()-5*dCurrentData.GetDataRMS(),
		 dCurrentData.GetDataMean()+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(dCurrentData.GetName());//Form("%s.%s PMT Asymmetry",MainDetectorPMTNames[det].Data(),dCurrentPMTAsyms[dTInd]->GetName()));
  grp->SetName(Form("pmtasym%02d_%d",det,dTInd));
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
      hst->Fill(dCurrentData.GetData(j));// - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMean() - 4.0*dCurrentData.GetDataRMS(),
				dCurrentData.GetDataMean() + 4.0*dCurrentData.GetDataRMS());
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

void QwGUIMainDetector::FillDETAsymPlots(Int_t det, Int_t dTInd)
{
//   Int_t events = 0;
  Int_t smplev = 0;

  TH1D *hst;
  TGraph *grp;

  QwGUIMainDetectorDataStructure dCurrentData = dCurrentDETAsyms[dTInd]->GetElements()[det];

  hst = new TH1D(Form("detasym%02d_%d",det,dTInd),dCurrentData.GetName(),
// 		 Form("%s.%s Detector Asymmetry",MainDetectorNames[det].Data(),
// 						   dCurrentDETAsyms[dTInd]->GetName()),
		 1000,dCurrentData.GetDataRMS()-5*dCurrentData.GetDataRMS(),dCurrentData.GetDataRMS()+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(dCurrentData.GetName());//Form("%s.%s Detector Asymmetry",MainDetectorNames[det].Data(),dCurrentDETAsyms[dTInd]->GetName()));
  grp->SetName(Form("detasym%02d_%d",det,dTInd));
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
      hst->Fill(dCurrentData.GetData(j));// - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMean() - 4.0*dCurrentData.GetDataRMS(),
				dCurrentData.GetDataMean() + 4.0*dCurrentData.GetDataRMS());
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
    
  
  DetAsymHistArray[dTInd].Add(hst);
  DetAsymGraphArray[dTInd].Add(grp);
  dCurrentDETAsyms[dTInd]->SetFFTTotalLength(dCurrentData.Length());  
}

void QwGUIMainDetector::FillCMBAsymPlots(Int_t det, Int_t dTInd)
{
//   Int_t events = 0;
  Int_t smplev = 0;

  TH1D *hst;
  TGraph *grp;

  QwGUIMainDetectorDataStructure dCurrentData = dCurrentCMBAsyms[dTInd]->GetElements()[det];

  hst = new TH1D(Form("cmbasym%02d_%d",det,dTInd),dCurrentData.GetName(),
// 		 Form("%s.%s Combination Asymmetry",MainDetectorCombinationNames[det].Data(),
// 						   dCurrentCMBAsyms[dTInd]->GetName()),
		 1000,dCurrentData.GetDataRMS()-5*dCurrentData.GetDataRMS(),dCurrentData.GetDataRMS()+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(dCurrentData.GetName());
  //Form("%s.%s Combination Asymmetry",MainDetectorCombinationNames[det].Data(),
// 		     dCurrentCMBAsyms[dTInd]->GetName()));
  grp->SetName(Form("cmbasym%02d_%d",det,dTInd));
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
      hst->Fill(dCurrentData.GetData(j));// - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMean() - 4.0*dCurrentData.GetDataRMS(),
				dCurrentData.GetDataMean() + 4.0*dCurrentData.GetDataRMS());
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
    
  CMBAsymHistArray[dTInd].Add(hst);
  CMBAsymGraphArray[dTInd].Add(grp);
  dCurrentCMBAsyms[dTInd]->SetFFTTotalLength(dCurrentData.Length());  
}


void QwGUIMainDetector::FillMSCAsymPlots(Int_t det, Int_t dTInd)
{
  Int_t smplev = 0;

  TH1D *hst;
  TGraph *grp;

  QwGUIMainDetectorDataStructure dCurrentData = dCurrentMSCAsyms[dTInd]->GetElements()[det];

  hst = new TH1D(Form("mscasym%02d_%d",det,dTInd),dCurrentData.GetName(),
		 1000,dCurrentData.GetDataRMS()-5*dCurrentData.GetDataRMS(),dCurrentData.GetDataRMS()+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(dCurrentData.GetName());
  grp->SetName(Form("mscasym%02d_%d",det,dTInd));
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
      hst->Fill(dCurrentData.GetData(j));// - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMean() - 4.0*dCurrentData.GetDataRMS(),
				dCurrentData.GetDataMean() + 4.0*dCurrentData.GetDataRMS());
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
    
  MSCAsymHistArray[dTInd].Add(hst);
  MSCAsymGraphArray[dTInd].Add(grp);
  dCurrentMSCAsyms[dTInd]->SetFFTTotalLength(dCurrentData.Length());  
}

void QwGUIMainDetector::FillMSCYieldPlots(Int_t det, Int_t dTInd)
{
  Int_t smplev = 0;

  TH1D *hst;
  TGraph *grp;

  QwGUIMainDetectorDataStructure dCurrentData = dCurrentMSCYields[dTInd]->GetElements()[det];

  hst = new TH1D(Form("mscyield%02d_%d",det,dTInd),dCurrentData.GetName(),
		 1000,dCurrentData.GetDataMean()-5*dCurrentData.GetDataRMS(),dCurrentData.GetDataMean()+5*dCurrentData.GetDataRMS());
  grp = new TGraph();
  grp->SetTitle(dCurrentData.GetName());
  grp->SetName(Form("mscyield%02d_%d",det,dTInd));
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
      hst->Fill(dCurrentData.GetData(j));// - dCurrentData.GetEnsambleMean(smplcnt));// mean);
      grp->SetPoint(j,1.0*(j+1)/SAMPLING_RATE,dCurrentData.GetData(j));
      smplev++;
    }
  }
  grp->GetYaxis()->SetRangeUser(dCurrentData.GetDataMean() - 4.0*dCurrentData.GetDataRMS(),
				dCurrentData.GetDataMean() + 4.0*dCurrentData.GetDataRMS());
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
    
  MSCYieldHistArray[dTInd].Add(hst);
  MSCYieldGraphArray[dTInd].Add(grp);
  dCurrentMSCYields[dTInd]->SetFFTTotalLength(dCurrentData.Length());  
}

void QwGUIMainDetector::FillSummaryPlots(Int_t dTInd)
{
  QwGUIMainDetectorDataStructure *dCurrentDataStr = NULL;
  vector <TString> Names;

  //PMT Yields
  
  TGraphErrors *grph = new TGraphErrors(MAIN_PMT_INDEX);
  grph->SetName("PMT_Yield_Stats");
  grph->SetTitle("PMT Yield Mean and RMS (md1- -> md8+)");
  dCurrentDataStr = dCurrentYields[dTInd]->GetElements();
  Names = MainDetectorPMTNames;

  if(dCurrentDataStr){
    for(int i = 0; i < (Int_t)MAIN_PMT_INDEX; i++){
      grph->SetPoint(i,i+1,dCurrentDataStr[i].GetDataMean());
      grph->SetPointError(i,0,dCurrentDataStr[i].GetDataRMS());
//       grph->GetXaxis()->SetBinLabel(i+1,Names[i].Data());
    }
  }
  SummaryGraphArray[dTInd].Add(grph);

  //PMT Asymmmetries

  grph = new TGraphErrors(MAIN_PMT_INDEX);
  grph->SetName("PMT_Asym_Stats");
  grph->SetTitle("PMT Asymmetry Mean and RMS (md1- -> md8+)");
  dCurrentDataStr = dCurrentPMTAsyms[dTInd]->GetElements();
  Names = MainDetectorPMTNames;
  if(dCurrentDataStr){
    for(int i = 0; i < (Int_t)MAIN_PMT_INDEX; i++){
      grph->SetPoint(i,i+1,dCurrentDataStr[i].GetDataMean());
      grph->SetPointError(i,0,dCurrentDataStr[i].GetDataRMS());
//       grph->GetXaxis()->SetBinLabel(i+1,Names[i].Data());
    }
  }
  SummaryGraphArray[dTInd].Add(grph);

  //DET Asymmmetries

  grph = new TGraphErrors(MAIN_DET_INDEX);
  grph->SetName("DET_Asym_Stats");
  grph->SetTitle("Detector Asymmetry Mean and RMS (md1 -> md8)");
  dCurrentDataStr = dCurrentDETAsyms[dTInd]->GetElements();
  Names = MainDetectorNames;
  if(dCurrentDataStr){
    for(int i = 0; i < (Int_t)MAIN_DET_INDEX; i++){
      grph->SetPoint(i,i+1,dCurrentDataStr[i].GetDataMean());
      grph->SetPointError(i,0,dCurrentDataStr[i].GetDataRMS());
//       grph->GetXaxis()->SetBinLabel(i+1,Names[i].Data());
    }
  }
  SummaryGraphArray[dTInd].Add(grph);

  //CMB Asymmmetries

  grph = new TGraphErrors(MAIN_DET_COMBIND);
  grph->SetName("CMB_Asym_Stats");
  grph->SetTitle("Detector Combination Asymmetry Mean and RMS");
  dCurrentDataStr = dCurrentCMBAsyms[dTInd]->GetElements();
  Names = MainDetectorCombinationNames;
  if(dCurrentDataStr){
    for(int i = 0; i < (Int_t)MAIN_DET_COMBIND; i++){
      grph->SetPoint(i,i+1,dCurrentDataStr[i].GetDataMean());
      grph->SetPointError(i,0,dCurrentDataStr[i].GetDataRMS());
//       grph->GetXaxis()->SetBinLabel(i+1,Names[i].Data());
    }
  }
  SummaryGraphArray[dTInd].Add(grph);

  //MSC Asymmmetries

  grph = new TGraphErrors(MAIN_MSC_INDEX);
  grph->SetName("MSC_Asym_Stats");
  grph->SetTitle("Auxiliary Detector Asymmetry Mean and RMS (PMT+LED,PMT,PMT+LG,md9-,md9+,I,V,Cage)");
  dCurrentDataStr = dCurrentMSCAsyms[dTInd]->GetElements();
  Names = MainDetectorMscNames;
  if(dCurrentDataStr){
    for(int i = 0; i < (Int_t)MAIN_MSC_INDEX; i++){
      grph->SetPoint(i,i+1,dCurrentDataStr[i].GetDataMean());
      grph->SetPointError(i,0,dCurrentDataStr[i].GetDataRMS());
//       grph->GetXaxis()->SetBinLabel(i+1,Names[i].Data());
    }
  }
  SummaryGraphArray[dTInd].Add(grph);

  //MSC Yields

  grph = new TGraphErrors(MAIN_MSC_INDEX);
  grph->SetName("MSC_Yield_Stats");
  grph->SetTitle("Auxiliary Detector Yield Mean and RMS (PMT+LED,PMT,PMT+LG,md9-,md9+,I,V,Cage)");
  dCurrentDataStr = dCurrentMSCYields[dTInd]->GetElements();
  Names = MainDetectorMscNames;
  if(dCurrentDataStr){
    for(int i = 0; i < (Int_t)MAIN_MSC_INDEX; i++){
      grph->SetPoint(i,i+1,dCurrentDataStr[i].GetDataMean());
      grph->SetPointError(i,0,dCurrentDataStr[i].GetDataRMS());
//       grph->GetXaxis()->SetBinLabel(i+1,Names[i].Data());
    }
  }
  SummaryGraphArray[dTInd].Add(grph);

}


void QwGUIMainDetector::OnRemoveThisTab()
{

}

void QwGUIMainDetector::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWind")) return;
  
  printf("Received Message From: %s\n",obj);
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

    next = new TIter(CMBAsymGraphArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(CMBAsymHistArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(MSCAsymHistArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(MSCAsymGraphArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(MSCYieldHistArray[n].MakeIterator());
    obj = next->Next();
    while(obj){    
      delete obj;
      obj = next->Next();
    }
    delete next;

    next = new TIter(MSCYieldGraphArray[n].MakeIterator());
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

    CMBAsymHistArray[n].Clear();
    CMBAsymGraphArray[n].Clear();

    MSCAsymHistArray[n].Clear();
    MSCAsymGraphArray[n].Clear();

    MSCYieldHistArray[n].Clear();
    MSCYieldGraphArray[n].Clear();
  }  

//   TObject *obj;
  next = new TIter(DataWindowArray.MakeIterator());
  
  obj = next->Next();
  while(obj){    
    delete obj;
    obj = next->Next();
  }
  delete next;

  DataWindowArray.Clear();

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

      next = new TIter(CMBAsymProfileArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;
      
      next = new TIter(CMBAsymDFTArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;

      CMBAsymProfileArray[n].Clear();
      CMBAsymDFTArray[n].Clear();

      next = new TIter(MSCAsymProfileArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;
      
      next = new TIter(MSCAsymDFTArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;

      MSCAsymProfileArray[n].Clear();
      MSCAsymDFTArray[n].Clear();

      next = new TIter(MSCYieldProfileArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;
      
      next = new TIter(MSCYieldDFTArray[n].MakeIterator());
      obj = next->Next();
      while(obj){    
	delete obj;
	obj = next->Next();
      }
      delete next;

      MSCYieldProfileArray[n].Clear();
      MSCYieldDFTArray[n].Clear();
      
    }
  }
  else{

    if(GetDataType() == PMT_YIELD){
      objarray1 = &YieldProfileArray[GetYieldDataIndex()];
      objarray2 = &YieldDFTArray[GetYieldDataIndex()];
    }    
    if(GetDataType() == PMT_ASYM){
      objarray1 = &PMTAsymProfileArray[GetPMTAsymDataIndex()];
      objarray2 = &PMTAsymDFTArray[GetPMTAsymDataIndex()];
    }    
    if(GetDataType() == DET_ASYM){
      objarray1 = &DetAsymProfileArray[GetDETAsymDataIndex()];
      objarray2 = &DetAsymDFTArray[GetDETAsymDataIndex()];
    }    
    if(GetDataType() == CMB_ASYM){
      objarray1 = &CMBAsymProfileArray[GetCMBAsymDataIndex()];
      objarray2 = &CMBAsymDFTArray[GetCMBAsymDataIndex()];
    }    
    if(GetDataType() == MSC_ASYM){
      objarray1 = &MSCAsymProfileArray[GetMSCAsymDataIndex()];
      objarray2 = &MSCAsymDFTArray[GetMSCAsymDataIndex()];
    }    
    if(GetDataType() == MSC_YIELD){
      objarray1 = &MSCYieldProfileArray[GetMSCYieldDataIndex()];
      objarray2 = &MSCYieldDFTArray[GetMSCYieldDataIndex()];
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

Bool_t QwGUIMainDetector::PlotHistograms()
{
  TObjArray *objarray = NULL;
  TCanvas *mc = NULL;

  if(GetDataType() == PMT_YIELD){
    objarray = &YieldHistArray[GetYieldDataIndex()];
    mc = dYieldCanvas->GetCanvas();
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymHistArray[GetPMTAsymDataIndex()];
    mc = dPMTAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymHistArray[GetDETAsymDataIndex()];
    mc = dDETAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == CMB_ASYM){
    objarray = &CMBAsymHistArray[GetCMBAsymDataIndex()];
    mc = dCMBAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_ASYM){
    objarray = &MSCAsymHistArray[GetMSCAsymDataIndex()];
    mc = dMSCAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_YIELD){
    objarray = &MSCYieldHistArray[GetMSCYieldDataIndex()];
    mc = dMSCYieldCanvas->GetCanvas();
  }    

  if(!mc) return kFalse;
  Int_t ind = 1;

  Double_t rad = 1.0/sqrt(2.0)/2.0;

  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();

  while(obj){
    mc->cd(ind);
    gPad->SetLogy(1);

    if(GetDataType() == DET_ASYM){
      gPad->SetPad(0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1);
    }
    ((TH1*)obj)->Draw("");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_HISTO,GetActiveTab());
  return kTrue;
}

Bool_t QwGUIMainDetector::PlotGraphs()
{
  TObjArray *objarray;
  Double_t rad = 1.0/sqrt(2.0)/2.0;

  TCanvas *mc = NULL;

  if(GetDataType() == PMT_YIELD){
    objarray = &YieldGraphArray[GetYieldDataIndex()];
    mc = dYieldCanvas->GetCanvas();
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymGraphArray[GetPMTAsymDataIndex()];
    mc = dPMTAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymGraphArray[GetDETAsymDataIndex()];
    mc = dDETAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == CMB_ASYM){
    objarray = &CMBAsymGraphArray[GetCMBAsymDataIndex()];
    mc = dCMBAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_ASYM){
    objarray = &MSCAsymGraphArray[GetMSCAsymDataIndex()];
    mc = dMSCAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_YIELD){
    objarray = &MSCYieldGraphArray[GetMSCYieldDataIndex()];
    mc = dMSCYieldCanvas->GetCanvas();
  }    
  if(GetDataType() == SUMMARY){
    objarray = &SummaryGraphArray[GetSummaryDataIndex()];
    mc = dSUMCanvas->GetCanvas();
  }    

  if(!mc) return kFalse;

  Int_t ind = 1;


  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
    if(GetDataType() == DET_ASYM){
      gPad->SetPad(0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1);
    }
    ((TGraph*)obj)->Draw("ap");
    ((TGraph*)obj)->GetYaxis()->UnZoom();
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_GRAPH,GetActiveTab());
  return kTrue;
}

Bool_t QwGUIMainDetector::PlotDFTProfile()
{
  TObjArray *objarray;
  Double_t rad = 1.0/sqrt(2.0)/2.0;

  TCanvas *mc = NULL;

  if(GetDataType() == PMT_YIELD){
    objarray = &YieldProfileArray[GetYieldDataIndex()];
    if(!dCurrentYields[GetYieldDataIndex()]->IsFFTCalculated()) return kFalse;
    mc = dYieldCanvas->GetCanvas();
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymProfileArray[GetPMTAsymDataIndex()];
    if(!dCurrentPMTAsyms[GetPMTAsymDataIndex()]->IsFFTCalculated()) return kFalse;
    mc = dPMTAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymProfileArray[GetDETAsymDataIndex()];
    if(!dCurrentDETAsyms[GetDETAsymDataIndex()]->IsFFTCalculated()) return kFalse;
    mc = dDETAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == CMB_ASYM){
    objarray = &CMBAsymProfileArray[GetCMBAsymDataIndex()];
    if(!dCurrentCMBAsyms[GetCMBAsymDataIndex()]->IsFFTCalculated()) return kFalse;
    mc = dCMBAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_ASYM){
    objarray = &MSCAsymProfileArray[GetMSCAsymDataIndex()];
    if(!dCurrentMSCAsyms[GetMSCAsymDataIndex()]->IsFFTCalculated()) return kFalse;
    mc = dMSCAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_YIELD){
    objarray = &MSCYieldProfileArray[GetMSCYieldDataIndex()];
    if(!dCurrentMSCYields[GetMSCYieldDataIndex()]->IsFFTCalculated()) return kFalse;
    mc = dMSCYieldCanvas->GetCanvas();
  }    

  if(!mc) return kFalse;

  Int_t ind = 1;

  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    gPad->SetLogy(0);
    if(GetDataType() == DET_ASYM){
      gPad->SetPad(0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1);
    }
    ((TProfile*)obj)->Draw();
//      ((TProfile*)obj)->ProjectionX("")->Draw();
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_PROFILE,GetActiveTab());
  return kTrue;
}

Bool_t QwGUIMainDetector::PlotDFT()
{
  TObjArray *objarray = NULL;
  Double_t rad = 1.0/sqrt(2.0)/2.0;
  TCanvas *mc = NULL;


  if(GetDataType() == PMT_YIELD){
    objarray = &YieldDFTArray[GetYieldDataIndex()];
    if(!CalculateDFT() &&
       !dCurrentYields[GetYieldDataIndex()]->IsFFTCalculated()){
      return kFalse;
    }
    mc = dYieldCanvas->GetCanvas();
  }    
  if(GetDataType() == PMT_ASYM){
    objarray = &PMTAsymDFTArray[GetPMTAsymDataIndex()];
    if(!CalculateDFT() &&
       !dCurrentPMTAsyms[GetPMTAsymDataIndex()]->IsFFTCalculated()){
      return kFalse;
    }
    mc = dPMTAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == DET_ASYM){
    objarray = &DetAsymDFTArray[GetDETAsymDataIndex()];
    if(!CalculateDFT() &&
       !dCurrentDETAsyms[GetDETAsymDataIndex()]->IsFFTCalculated()){
      return kFalse;
    }
    mc = dDETAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == CMB_ASYM){
    objarray = &CMBAsymDFTArray[GetDETAsymDataIndex()];
    if(!CalculateDFT() &&
       !dCurrentCMBAsyms[GetCMBAsymDataIndex()]->IsFFTCalculated()){
      return kFalse;
    }
    mc = dCMBAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_ASYM){
    objarray = &MSCAsymProfileArray[GetMSCAsymDataIndex()];
    if(!CalculateDFT() &&
       !dCurrentMSCAsyms[GetMSCAsymDataIndex()]->IsFFTCalculated()){
      return kFalse;
    }
    mc = dMSCAsymCanvas->GetCanvas();
  }    
  if(GetDataType() == MSC_YIELD){
    objarray = &MSCYieldProfileArray[GetMSCYieldDataIndex()];
    if(!CalculateDFT() &&
       !dCurrentMSCYields[GetMSCYieldDataIndex()]->IsFFTCalculated()){
      return kFalse;
    }
    mc = dMSCYieldCanvas->GetCanvas();
  }    



  if(!mc) return kFalse;
  Int_t ind = 1;


  TObject *obj;
  TIter next(objarray->MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    gPad->SetLogy();
    if(GetDataType() == DET_ASYM){
      gPad->SetPad(0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)-0.1,
		   0.5+rad*TMath::Sin(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1,
		   0.5+rad*TMath::Cos(2*(ind-1)*TMath::Pi()/8 - TMath::Pi()/2)+0.1);
    }
    ((TH1*)obj)->Draw();    

    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotType(PLOT_TYPE_DFT,GetActiveTab());
  return kTrue;
}

Bool_t QwGUIMainDetector::CalculateDFT()
{
  RMsgBox *msgbx = NULL;
  QwGUIMainDetectorData *dCurrentData = NULL;
  QwGUIMainDetectorDataStructure *dCurrentDataStr = NULL;
  Int_t INDEX = 0;
  vector <TString> Names;
  TObjArray *DFTArray  = NULL;
  TObjArray *ProfArray = NULL;


  if(GetDataType() == PMT_YIELD){    
    dCurrentData    = dCurrentYields[GetYieldDataIndex()];
    dCurrentDataStr = dCurrentYields[GetYieldDataIndex()]->GetElements();
    INDEX = MAIN_PMT_INDEX;
    Names = MainDetectorPMTNames;
    DFTArray = &YieldDFTArray[GetYieldDataIndex()];
    ProfArray = &YieldProfileArray[GetYieldDataIndex()];

  }
  else if(GetDataType() == PMT_ASYM){
    dCurrentData    = dCurrentPMTAsyms[GetPMTAsymDataIndex()];
    dCurrentDataStr = dCurrentPMTAsyms[GetPMTAsymDataIndex()]->GetElements();
    INDEX = MAIN_PMT_INDEX;
    Names = MainDetectorPMTNames;
    DFTArray = &PMTAsymDFTArray[GetPMTAsymDataIndex()];
    ProfArray = &PMTAsymProfileArray[GetPMTAsymDataIndex()];
  }
  else if(GetDataType() == DET_ASYM){
    dCurrentData    = dCurrentDETAsyms[GetDETAsymDataIndex()];
    dCurrentDataStr = dCurrentDETAsyms[GetDETAsymDataIndex()]->GetElements();
    INDEX = MAIN_DET_INDEX;
    Names = MainDetectorNames;
    DFTArray = &DetAsymDFTArray[GetDETAsymDataIndex()];
    ProfArray = &DetAsymProfileArray[GetDETAsymDataIndex()];
  }
  else if(GetDataType() == CMB_ASYM){
    dCurrentData    = dCurrentCMBAsyms[GetCMBAsymDataIndex()];
    dCurrentDataStr = dCurrentCMBAsyms[GetCMBAsymDataIndex()]->GetElements();
    INDEX = MAIN_DET_COMBIND;
    Names = MainDetectorCombinationNames;
    DFTArray = &CMBAsymDFTArray[GetCMBAsymDataIndex()];
    ProfArray = &CMBAsymProfileArray[GetCMBAsymDataIndex()];
  }
  else if(GetDataType() == MSC_ASYM){
    dCurrentData    = dCurrentMSCAsyms[GetMSCAsymDataIndex()];
    dCurrentDataStr = dCurrentMSCAsyms[GetMSCAsymDataIndex()]->GetElements();
    INDEX = MAIN_MSC_INDEX;
    Names = MainDetectorMscNames;
    DFTArray = &MSCAsymDFTArray[GetMSCAsymDataIndex()];
    ProfArray = &MSCAsymProfileArray[GetMSCAsymDataIndex()];
  }
  else if(GetDataType() == MSC_YIELD){
    dCurrentData    = dCurrentMSCYields[GetMSCYieldDataIndex()];
    dCurrentDataStr = dCurrentMSCYields[GetMSCYieldDataIndex()]->GetElements();
    INDEX = MAIN_MSC_INDEX;
    Names = MainDetectorMscNames;
    DFTArray = &MSCYieldDFTArray[GetMSCYieldDataIndex()];
    ProfArray = &MSCYieldProfileArray[GetMSCYieldDataIndex()];
  }
  else
    return kFalse;

  if(!dCurrentData || !dCurrentDataStr || !DFTArray || !ProfArray) return kFalse;

  if(!INDEX) return kFalse;

  FFTOptions *fftopts = dCurrentData->GetFFTOptions();
  
  TProfile *prf = NULL;
  TH1D *hst = NULL;
  TH1D *fft = NULL;
  
  new QwGUIFFTWindowSelectionDialog(fClient->GetRoot(), GetMain(), "sdlg","QwGUIMainDetector",fftopts);

  if(fftopts->cancelFlag) return kFalse;
  if(fftopts->changeFlag || !dCurrentData->IsFFTCalculated()){

    if(fftopts->Length <= 0) return kFalse;
    if(fftopts->Start < 0 || fftopts->Start >= dCurrentDataStr[0].Length()) return kFalse;
    if(fftopts->Start + fftopts->Length > dCurrentDataStr[0].Length()) return kFalse;

    
    InitProgressDlg("FFT Calculation","Detector","Event",0,kTrue,INDEX,fftopts->Length,0,2);
 
    Int_t det = 1;
    Int_t evn = 1;
    IncreaseProgress(&det,0,0,1,0,0);
    IncreaseProgress(0,&evn,0,0,1,0);

    gSystem->ProcessEvents();

//     Int_t retval;
//     //strcpy(buffer,"FFT Calculation In Progress!");
//     msgbx = new RMsgBox(fClient->GetRoot(), GetMain(),"msgbx", "QwGUIMainDetector", "Data Processing",
// 				 "FFT Calculation In Progress!",kMBIconExclamation, kMBCancel, &retval);
//     if(retval == kMBCancel){ 
//       return kFalse;
//     }    
        
    
    ClearDFTData(kFalse);
    for(int l = 0; l < INDEX; l++){      
      
      if(dCurrentDataStr[l].Length() > 0){

	gSystem->ProcessEvents();

	prf = new TProfile(Form("%s_%s_prf",Names[l].Data(),dCurrentData->GetName()),
			   Form("%s FFT Data",dCurrentDataStr[l].GetName()),
			   fftopts->Length,1.0*fftopts->Start/SAMPLING_RATE,
			   1.0*(fftopts->Start+fftopts->Length)/SAMPLING_RATE);
	
	for(int n = fftopts->Start; n < fftopts->Length+fftopts->Start; n++){ 
	  if(dCurrentDataStr[l].GetData(n) != 0){
	    prf->Fill(1.0*n/SAMPLING_RATE,dCurrentDataStr[l].GetData(n)-dCurrentDataStr[l].GetDataMean());
// 	    if(l == 8)
// 	      printf("det %s data = %1.5e\n",Names[l].Data(),dCurrentDataStr[l].GetData(n));
	  }
	}      
	
	hst = prf->ProjectionX(Form("%s_%s-prj",Names[l].Data(),dCurrentData->GetName()));
	
	TH1 *fftmag = NULL;
	TVirtualFFT::SetTransform(0);
	fftmag = ((TH1*)hst)->FFT(fftmag,"MAG");
	fftmag->SetName(Form("%s_%s_fft-mag",Names[l].Data(),dCurrentData->GetName()));
	fftmag->SetTitle("Magnitude of the transform");
	
	fft = new TH1D(Form("%s_%s_fft",Names[l].Data(),dCurrentData->GetName()),
		       Form("%s FFT",dCurrentDataStr[l].GetName()),
		       (Int_t)(fftopts->Length/2.0)-1,0.0,(Int_t)(SAMPLING_RATE/2.0));
	
	evn = 1;
	for(int i = 0; i < (Int_t)(fftopts->Length/2); i++) {
	  fft->SetBinContent(i,1.25*fftmag->GetBinContent(i)/fftopts->Length);
  
	  evn++;
	  if(dProcessHalt) break;
	  IncreaseProgress(0,&evn,0,0,5000,0);
	}
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
      det++;
      if(dProcessHalt) break;
      IncreaseProgress(&det,0,0,1,0,0);
    }
  }

  if(dProgrDlg)
    dProgrDlg->CloseWindow();


  dCurrentData->SetFFTCalculated(kTrue);

  if(GetDataType() == PMT_YIELD) dMenuPlot1->EnableEntry(M_PLOT_DFTPROF_YIELD);   
  if(GetDataType() == PMT_ASYM ) dMenuPlot2->EnableEntry(M_PLOT_DFTPROF_PMT_ASYM);
  if(GetDataType() == DET_ASYM ) dMenuPlot3->EnableEntry(M_PLOT_DFTPROF_DET_ASYM);
  if(GetDataType() == CMB_ASYM ) dMenuPlot4->EnableEntry(M_PLOT_DFTPROF_CMB_ASYM);
  if(GetDataType() == MSC_ASYM ) dMenuPlot5->EnableEntry(M_PLOT_DFTPROF_MSC_ASYM);
  if(GetDataType() == MSC_YIELD ) dMenuPlot6->EnableEntry(M_PLOT_DFTPROF_MSC_YIELD);

//   if(msgbx)
//     delete msgbx;
  
  return kTrue;
}


void QwGUIMainDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  QwGUIDataWindow *dDataWindow = NULL;
  QwGUIMainDetectorDataStructure *dCurrentDataStr;
  TObjArray *HistArray;
  TObjArray *GraphArray;
  TObjArray *DFTArray;
  TObjArray *ProfileArray;
  UInt_t pad = 0;
  Bool_t add = kFalse;

  if(event == kButton1Double){
    
    if(GetDataType() == PMT_YIELD){    
      dCurrentDataStr = dCurrentYields[GetYieldDataIndex()]->GetElements();
      DFTArray = &YieldDFTArray[GetYieldDataIndex()];
      ProfileArray = &YieldProfileArray[GetYieldDataIndex()];
      HistArray = &YieldHistArray[GetYieldDataIndex()];
      GraphArray = &YieldGraphArray[GetYieldDataIndex()];
      pad = dYieldCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    }
    else if(GetDataType() == PMT_ASYM){
      dCurrentDataStr = dCurrentPMTAsyms[GetPMTAsymDataIndex()]->GetElements();
      DFTArray = &PMTAsymDFTArray[GetPMTAsymDataIndex()];
      ProfileArray = &PMTAsymProfileArray[GetPMTAsymDataIndex()];
      HistArray = &PMTAsymHistArray[GetPMTAsymDataIndex()];
      GraphArray = &PMTAsymGraphArray[GetPMTAsymDataIndex()];
      pad = dPMTAsymCanvas->GetCanvas()->GetSelectedPad()->GetNumber();      
    }
    else if(GetDataType() == DET_ASYM){
      dCurrentDataStr = dCurrentDETAsyms[GetDETAsymDataIndex()]->GetElements();
      DFTArray = &DetAsymDFTArray[GetDETAsymDataIndex()];
      ProfileArray = &DetAsymProfileArray[GetDETAsymDataIndex()];
      HistArray = &DetAsymHistArray[GetDETAsymDataIndex()];
      GraphArray = &DetAsymGraphArray[GetDETAsymDataIndex()];
      pad = dDETAsymCanvas->GetCanvas()->GetSelectedPad()->GetNumber();      
    }
    else if(GetDataType() == CMB_ASYM){
      dCurrentDataStr = dCurrentCMBAsyms[GetCMBAsymDataIndex()]->GetElements();
      DFTArray = &CMBAsymDFTArray[GetCMBAsymDataIndex()];
      ProfileArray = &CMBAsymProfileArray[GetCMBAsymDataIndex()];
      HistArray = &CMBAsymHistArray[GetCMBAsymDataIndex()];
      GraphArray = &CMBAsymGraphArray[GetCMBAsymDataIndex()];
      pad = dCMBAsymCanvas->GetCanvas()->GetSelectedPad()->GetNumber();      
    }
    else if(GetDataType() == MSC_ASYM){
      dCurrentDataStr = dCurrentMSCAsyms[GetMSCAsymDataIndex()]->GetElements();
      DFTArray = &MSCAsymDFTArray[GetMSCAsymDataIndex()];
      ProfileArray = &MSCAsymProfileArray[GetMSCAsymDataIndex()];
      HistArray = &MSCAsymHistArray[GetMSCAsymDataIndex()];
      GraphArray = &MSCAsymGraphArray[GetMSCAsymDataIndex()];
      pad = dMSCAsymCanvas->GetCanvas()->GetSelectedPad()->GetNumber();      
    }
    else if(GetDataType() == MSC_YIELD){
      dCurrentDataStr = dCurrentMSCYields[GetMSCYieldDataIndex()]->GetElements();
      DFTArray = &MSCYieldDFTArray[GetMSCYieldDataIndex()];
      ProfileArray = &MSCYieldProfileArray[GetMSCYieldDataIndex()];
      HistArray = &MSCYieldHistArray[GetMSCYieldDataIndex()];
      GraphArray = &MSCYieldGraphArray[GetMSCYieldDataIndex()];
      pad = dMSCYieldCanvas->GetCanvas()->GetSelectedPad()->GetNumber();      
    }
    else if(GetDataType() == SUMMARY){
//       dCurrentDataStr = dCurrentMSCYields[GetMSCYieldDataIndex()]->GetElements();
//       DFTArray = &MSCYieldDFTArray[GetMSCYieldDataIndex()];
//       ProfileArray = &MSCYieldProfileArray[GetMSCYieldDataIndex()];
//       HistArray = &MSCYieldHistArray[GetMSCYieldDataIndex()];
      GraphArray = &SummaryGraphArray[GetSummaryDataIndex()];
      pad = dSUMCanvas->GetCanvas()->GetSelectedPad()->GetNumber();      
    }
    else
      return;

    if(pad > 0 && pad <= MAIN_PMT_INDEX){
      if(GetPlotType(GetActiveTab()) == PLOT_TYPE_HISTO){
	
	dDataWindow = GetSelectedDataWindow();
	if(!dDataWindow)
	  dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					    "QwGUIMainDetector",(*HistArray)[pad-1]->GetTitle(), PT_HISTO_1D,
					    DDT_MD,600,400);
	else
	  add = kTrue;

	if(!dDataWindow){
	  return;
	}
       
	DataWindowArray.Add(dDataWindow);
	dDataWindow->SetStaticData((*HistArray)[pad-1],DataWindowArray.GetLast());

	Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
	Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
	Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");      

	dDataWindow->SetPlotTitle((char*)(*HistArray)[pad-1]->GetTitle());
	dDataWindow->DrawData(*((TH1D*)(*HistArray)[pad-1]),add);
	SetLogMessage(Form("Looking at histogram %s\n",(char*)(*HistArray)[pad-1]->GetTitle()),kTrue);
	
	return;
      }
      else if(GetPlotType(GetActiveTab()) == PLOT_TYPE_GRAPH){


	dDataWindow = GetSelectedDataWindow();
	if(!dDataWindow)
	  dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					    "QwGUIMainDetector",(*GraphArray)[pad-1]->GetTitle(), PT_GRAPH,
					    DDT_MD,600,400);
	else
	  add = kTrue;


// 	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
// 						     GetNewWindowName(),"QwGUIMainDetector",
// 						     (*GraphArray)[pad-1]->GetTitle(), PT_GRAPH,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
	dDataWindow->SetPlotTitle((char*)(*GraphArray)[pad-1]->GetTitle());
	if(GetDataType() != SUMMARY){
	  dDataWindow->SetPlotTitleX("Time [sec]");
	  dDataWindow->SetPlotTitleY("Amplitude [V]");
	  dDataWindow->SetPlotTitleOffset(1.25,1.8);
	  dDataWindow->SetAxisMin(((TGraph*)(*GraphArray)[pad-1])->GetXaxis()->GetXmin(),
				  dCurrentDataStr[pad-1].GetDataMin());
	  dDataWindow->SetAxisMax(((TGraph*)(*GraphArray)[pad-1])->GetXaxis()->GetXmax(),
				  dCurrentDataStr[pad-1].GetDataMax());
	  dDataWindow->SetLimitsFlag(kTrue);
	  dDataWindow->DrawData(*((TGraph*)(*GraphArray)[pad-1]),add);
	}
	else{
	  dDataWindow->DrawData(*((TGraphErrors*)(*GraphArray)[pad-1]),add);
	  
	}
	SetLogMessage(Form("Looking at graph %s\n",(char*)(*GraphArray)[pad-1]->GetTitle()),kTrue);
      }
      else if(GetPlotType(GetActiveTab()) == PLOT_TYPE_DFT){

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
      else if(GetPlotType(GetActiveTab()) == PLOT_TYPE_PROFILE){

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
  MDMenuIdentifiers dtype;

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

    case kCM_TAB:

      switch(dMDTab->GetCurrent()){

      case 0:
	SetDataType(PMT_YIELD);
	break;

      case 1:
	SetDataType(PMT_ASYM);
	break;

      case 2:
	SetDataType(DET_ASYM);
	break;

      case 3:
	SetDataType(CMB_ASYM);
	break;

      case 4:
	SetDataType(MSC_ASYM);
	break;

      case 5:
	SetDataType(MSC_YIELD);
	break;

      case 6:
	SetDataType(SUMMARY);
	break;

      default:
	break;
      }

      break;
      
    case kCM_MENU:

      switch (parm1) {
	
      case M_PLOT_HISTO_YIELD:
	dtype = GetDataType();
	SetDataType(PMT_YIELD);
	if(PlotHistograms()){
	  dMenuPlot1->UnCheckEntries();
	  dMenuPlot1->CheckEntry(M_PLOT_HISTO_YIELD);
	}
	else
	  SetDataType(dtype);
	break;

      case M_PLOT_HISTO_PMT_ASYM:
	dtype = GetDataType();
	SetDataType(PMT_ASYM);
	if(PlotHistograms()){
	  dMenuPlot2->UnCheckEntries();
	  dMenuPlot2->CheckEntry(M_PLOT_HISTO_PMT_ASYM);
	}
	else
	  SetDataType(dtype);
	break;

      case M_PLOT_HISTO_DET_ASYM:
	dtype = GetDataType();
	SetDataType(DET_ASYM);
	if(PlotHistograms()){
	  dMenuPlot3->UnCheckEntries();
	  dMenuPlot3->CheckEntry(M_PLOT_HISTO_DET_ASYM);
	}
	else
	  SetDataType(dtype);
	break;

      case M_PLOT_HISTO_CMB_ASYM:
	dtype = GetDataType();
	SetDataType(CMB_ASYM);
	if(PlotHistograms()){
	  dMenuPlot4->UnCheckEntries();
	  dMenuPlot4->CheckEntry(M_PLOT_HISTO_CMB_ASYM);
	}
	else
	  SetDataType(dtype);
	break;

      case M_PLOT_HISTO_MSC_ASYM:
	dtype = GetDataType();
	SetDataType(MSC_ASYM);
	if(PlotHistograms()){
	  dMenuPlot5->UnCheckEntries();
	  dMenuPlot5->CheckEntry(M_PLOT_HISTO_MSC_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_HISTO_MSC_YIELD:
	dtype = GetDataType();
	SetDataType(MSC_YIELD);
	if(PlotHistograms()){
	  dMenuPlot6->UnCheckEntries();
	  dMenuPlot6->CheckEntry(M_PLOT_HISTO_MSC_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;


      case M_PLOT_GRAPH_YIELD:
	dtype = GetDataType();
	SetDataType(PMT_YIELD);
	if(PlotGraphs()){
	  dMenuPlot1->UnCheckEntries();
	  dMenuPlot1->CheckEntry(M_PLOT_GRAPH_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_GRAPH_PMT_ASYM:
	dtype = GetDataType();
	SetDataType(PMT_ASYM);
	if(PlotGraphs()){
	  dMenuPlot2->UnCheckEntries();
	  dMenuPlot2->CheckEntry(M_PLOT_GRAPH_PMT_ASYM);
	}
	else
	  SetDataType(dtype);
	break;

      case M_PLOT_GRAPH_DET_ASYM:
	dtype = GetDataType();
	SetDataType(DET_ASYM);
	if(PlotGraphs()){
	  dMenuPlot3->UnCheckEntries();
	  dMenuPlot3->CheckEntry(M_PLOT_GRAPH_DET_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_GRAPH_CMB_ASYM:
	dtype = GetDataType();
	SetDataType(CMB_ASYM);
	if(PlotGraphs()){
	  dMenuPlot4->UnCheckEntries();
	  dMenuPlot4->CheckEntry(M_PLOT_GRAPH_CMB_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_GRAPH_MSC_ASYM:
	dtype = GetDataType();
	SetDataType(MSC_ASYM);
	if(PlotGraphs()){
	  dMenuPlot5->UnCheckEntries();
	  dMenuPlot5->CheckEntry(M_PLOT_GRAPH_MSC_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_GRAPH_MSC_YIELD:
	dtype = GetDataType();
	SetDataType(MSC_YIELD);
	if(PlotGraphs()){
	  dMenuPlot6->UnCheckEntries();
	  dMenuPlot6->CheckEntry(M_PLOT_GRAPH_MSC_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;


      case M_PLOT_DFT_YIELD:
	dtype = GetDataType();
	SetDataType(PMT_YIELD);
	if(PlotDFT()){
	  dMenuPlot1->UnCheckEntries();
	  dMenuPlot1->CheckEntry(M_PLOT_DFT_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;	

      case M_PLOT_DFT_PMT_ASYM:
	dtype = GetDataType();
	SetDataType(PMT_ASYM);
	if(PlotDFT()){
	  dMenuPlot2->UnCheckEntries();
	  dMenuPlot2->CheckEntry(M_PLOT_DFT_PMT_ASYM);
	}
	break;	

      case M_PLOT_DFT_DET_ASYM:
	dtype = GetDataType();
	SetDataType(DET_ASYM);
	if(PlotDFT()){
	  dMenuPlot3->UnCheckEntries();
	  dMenuPlot3->CheckEntry(M_PLOT_DFT_DET_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;	

      case M_PLOT_DFT_CMB_ASYM:
	dtype = GetDataType();
	SetDataType(CMB_ASYM);
	if(PlotDFT()){
	  dMenuPlot4->UnCheckEntries();
	  dMenuPlot4->CheckEntry(M_PLOT_DFT_CMB_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;	

      case M_PLOT_DFT_MSC_ASYM:
	dtype = GetDataType();
	SetDataType(MSC_ASYM);
	if(PlotDFT()){
	  dMenuPlot5->UnCheckEntries();
	  dMenuPlot5->CheckEntry(M_PLOT_DFT_MSC_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_DFT_MSC_YIELD:
	dtype = GetDataType();
	SetDataType(MSC_YIELD);
	if(PlotDFT()){
	  dMenuPlot6->UnCheckEntries();
	  dMenuPlot6->CheckEntry(M_PLOT_DFT_MSC_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;


      case M_PLOT_DFTPROF_YIELD:
	dtype = GetDataType();
	SetDataType(PMT_YIELD);
	if(PlotDFTProfile()){
	  dMenuPlot1->UnCheckEntries();
	  dMenuPlot1->CheckEntry(M_PLOT_DFTPROF_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_DFTPROF_PMT_ASYM:
	dtype = GetDataType();
	SetDataType(PMT_ASYM);
	if(PlotDFTProfile()){
	  dMenuPlot2->UnCheckEntries();
	  dMenuPlot2->CheckEntry(M_PLOT_DFTPROF_PMT_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_DFTPROF_DET_ASYM:
	dtype = GetDataType();
	SetDataType(DET_ASYM);
	if(PlotDFTProfile()){
	  dMenuPlot3->UnCheckEntries();
	  dMenuPlot3->CheckEntry(M_PLOT_DFTPROF_DET_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_DFTPROF_CMB_ASYM:
	dtype = GetDataType();
	SetDataType(CMB_ASYM);
	if(PlotDFTProfile()){
	  dMenuPlot4->UnCheckEntries();
	  dMenuPlot4->CheckEntry(M_PLOT_DFTPROF_CMB_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_DFTPROF_MSC_ASYM:
	dtype = GetDataType();
	SetDataType(MSC_ASYM);
	if(PlotDFTProfile()){
	  dMenuPlot5->UnCheckEntries();
	  dMenuPlot5->CheckEntry(M_PLOT_DFTPROF_MSC_ASYM);
	}
        else                 
          SetDataType(dtype);
	break;

      case M_PLOT_DFTPROF_MSC_YIELD:
	dtype = GetDataType();
	SetDataType(MSC_YIELD);
	if(PlotDFTProfile()){
	  dMenuPlot6->UnCheckEntries();
	  dMenuPlot6->CheckEntry(M_PLOT_DFTPROF_MSC_YIELD);
	}
        else                 
          SetDataType(dtype);
	break;

	/*PMT_YIELD*************************************/

      case M_DATA_PMT_YIELD_SUM:
	SetYieldDataIndex(0);
	SetDataType(PMT_YIELD);
	if(dMenuPlot1->IsEntryChecked(M_PLOT_GRAPH_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_HISTO_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFT_YIELD))
	  if(PlotDFT()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFTPROF_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_SUM);
	  }
	break;

      case M_DATA_PMT_YIELD_B1:
	SetYieldDataIndex(1);
	SetDataType(PMT_YIELD);
	if(dMenuPlot1->IsEntryChecked(M_PLOT_GRAPH_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_HISTO_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFT_YIELD))
	  if(PlotDFT()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFTPROF_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	break;

      case M_DATA_PMT_YIELD_B2:
	SetYieldDataIndex(2);
	SetDataType(PMT_YIELD);
	if(dMenuPlot1->IsEntryChecked(M_PLOT_GRAPH_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_HISTO_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFT_YIELD))
	  if(PlotDFT()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFTPROF_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	break;

      case M_DATA_PMT_YIELD_B3:
	SetYieldDataIndex(3);
	SetDataType(PMT_YIELD);
	if(dMenuPlot1->IsEntryChecked(M_PLOT_GRAPH_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_HISTO_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFT_YIELD))
	  if(PlotDFT()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFTPROF_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	break;
	
      case M_DATA_PMT_YIELD_B4:
	SetYieldDataIndex(4);
	SetDataType(PMT_YIELD);
	if(dMenuPlot1->IsEntryChecked(M_PLOT_GRAPH_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_HISTO_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFT_YIELD))
	  if(PlotDFT()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	if(dMenuPlot1->IsEntryChecked(M_PLOT_DFTPROF_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock1->UnCheckEntries();
	    dMenuBlock1->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	break;
	


	/*PMT_ASYM*************************************/

      case M_DATA_PMT_ASYM_SUM:
	SetPMTAsymDataIndex(0);
	SetDataType(PMT_ASYM);
	if(dMenuPlot2->IsEntryChecked(M_PLOT_GRAPH_PMT_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_HISTO_PMT_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFT_PMT_ASYM))
	  if(PlotDFT()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFTPROF_PMT_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_SUM);
	  }
	break;

      case M_DATA_PMT_ASYM_B1:
	SetPMTAsymDataIndex(1);
	SetDataType(PMT_ASYM);
	if(dMenuPlot2->IsEntryChecked(M_PLOT_GRAPH_PMT_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_HISTO_PMT_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFT_PMT_ASYM))
	  if(PlotDFT()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFTPROF_PMT_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	break;

      case M_DATA_PMT_ASYM_B2:
	SetPMTAsymDataIndex(2);
	SetDataType(PMT_ASYM);
	if(dMenuPlot2->IsEntryChecked(M_PLOT_GRAPH_PMT_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_HISTO_PMT_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFT_PMT_ASYM))
	  if(PlotDFT()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFTPROF_PMT_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	break;

      case M_DATA_PMT_ASYM_B3:
	SetPMTAsymDataIndex(3);
	SetDataType(PMT_ASYM);
	if(dMenuPlot2->IsEntryChecked(M_PLOT_GRAPH_PMT_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_HISTO_PMT_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFT_PMT_ASYM))
	  if(PlotDFT()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFTPROF_PMT_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	break;
	
      case M_DATA_PMT_ASYM_B4:
	SetPMTAsymDataIndex(4);
	SetDataType(PMT_ASYM);
	if(dMenuPlot2->IsEntryChecked(M_PLOT_GRAPH_PMT_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_HISTO_PMT_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFT_PMT_ASYM))
	  if(PlotDFT()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot2->IsEntryChecked(M_PLOT_DFTPROF_PMT_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock2->UnCheckEntries();
	    dMenuBlock2->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	break;



	/*DET_ASYM*************************************/

      case M_DATA_DET_ASYM_SUM:
	SetDETAsymDataIndex(0);
	SetDataType(DET_ASYM);
	if(dMenuPlot3->IsEntryChecked(M_PLOT_GRAPH_DET_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_SUM);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_HISTO_DET_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_SUM);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFT_DET_ASYM))
	  if(PlotDFT()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_SUM);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFTPROF_DET_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_DET_ASYM_SUM);
	  }
	break;

      case M_DATA_DET_ASYM_B1:
	SetDETAsymDataIndex(1);
	SetDataType(DET_ASYM);
	if(dMenuPlot3->IsEntryChecked(M_PLOT_GRAPH_DET_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_HISTO_DET_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFT_DET_ASYM))
	  if(PlotDFT()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFTPROF_DET_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	break;

      case M_DATA_DET_ASYM_B2:
	SetDETAsymDataIndex(2);
	SetDataType(DET_ASYM);
	if(dMenuPlot3->IsEntryChecked(M_PLOT_GRAPH_DET_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_HISTO_DET_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFT_DET_ASYM))
	  if(PlotDFT()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFTPROF_DET_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	break;

      case M_DATA_DET_ASYM_B3:
	SetDETAsymDataIndex(3);
	SetDataType(DET_ASYM);
	if(dMenuPlot3->IsEntryChecked(M_PLOT_GRAPH_DET_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_HISTO_DET_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFT_DET_ASYM))
	  if(PlotDFT()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFTPROF_DET_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	break;
	
      case M_DATA_DET_ASYM_B4:
	SetDETAsymDataIndex(4);
	SetDataType(DET_ASYM);
	if(dMenuPlot3->IsEntryChecked(M_PLOT_GRAPH_DET_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_HISTO_DET_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFT_DET_ASYM))
	  if(PlotDFT()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot3->IsEntryChecked(M_PLOT_DFTPROF_DET_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock3->UnCheckEntries();
	    dMenuBlock3->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	break;

	/*CMB_ASYM*************************************/

      case M_DATA_CMB_ASYM_SUM:
	SetCMBAsymDataIndex(0);
	SetDataType(CMB_ASYM);
	if(dMenuPlot4->IsEntryChecked(M_PLOT_GRAPH_CMB_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_CMB_ASYM_SUM);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_HISTO_CMB_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_CMB_ASYM_SUM);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFT_CMB_ASYM))
	  if(PlotDFT()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_CMB_ASYM_SUM);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFTPROF_CMB_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_CMB_ASYM_SUM);
	  }
	break;

      case M_DATA_CMB_ASYM_B1:
	SetCMBAsymDataIndex(1);
	SetDataType(CMB_ASYM);
	if(dMenuPlot4->IsEntryChecked(M_PLOT_GRAPH_CMB_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_HISTO_CMB_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFT_CMB_ASYM))
	  if(PlotDFT()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFTPROF_CMB_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	break;

      case M_DATA_CMB_ASYM_B2:
	SetCMBAsymDataIndex(2);
	SetDataType(CMB_ASYM);
	if(dMenuPlot4->IsEntryChecked(M_PLOT_GRAPH_CMB_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_HISTO_CMB_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFT_CMB_ASYM))
	  if(PlotDFT()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFTPROF_CMB_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	break;

      case M_DATA_CMB_ASYM_B3:
	SetCMBAsymDataIndex(3);
	SetDataType(CMB_ASYM);
	if(dMenuPlot4->IsEntryChecked(M_PLOT_GRAPH_CMB_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_HISTO_CMB_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFT_CMB_ASYM))
	  if(PlotDFT()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFTPROF_CMB_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	break;
	
      case M_DATA_CMB_ASYM_B4:
	SetCMBAsymDataIndex(4);
	SetDataType(CMB_ASYM);
	if(dMenuPlot4->IsEntryChecked(M_PLOT_GRAPH_CMB_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_HISTO_CMB_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFT_CMB_ASYM))
	  if(PlotDFT()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot4->IsEntryChecked(M_PLOT_DFTPROF_CMB_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock4->UnCheckEntries();
	    dMenuBlock4->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	break;



	/*MSC_ASYM*************************************/

      case M_DATA_MSC_ASYM_SUM:
	SetMSCAsymDataIndex(0);
	SetDataType(MSC_ASYM);
	if(dMenuPlot5->IsEntryChecked(M_PLOT_GRAPH_MSC_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_MSC_ASYM_SUM);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_HISTO_MSC_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_MSC_ASYM_SUM);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFT_MSC_ASYM))
	  if(PlotDFT()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_MSC_ASYM_SUM);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFTPROF_MSC_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_MSC_ASYM_SUM);
	  }
	break;

      case M_DATA_MSC_ASYM_B1:
	SetMSCAsymDataIndex(1);
	SetDataType(MSC_ASYM);
	if(dMenuPlot5->IsEntryChecked(M_PLOT_GRAPH_MSC_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_HISTO_MSC_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFT_MSC_ASYM))
	  if(PlotDFT()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFTPROF_MSC_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B1);
	  }
	break;

      case M_DATA_MSC_ASYM_B2:
	SetMSCAsymDataIndex(2);
	SetDataType(MSC_ASYM);
	if(dMenuPlot5->IsEntryChecked(M_PLOT_GRAPH_MSC_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_HISTO_MSC_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFT_MSC_ASYM))
	  if(PlotDFT()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFTPROF_MSC_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B2);
	  }
	break;

      case M_DATA_MSC_ASYM_B3:
	SetMSCAsymDataIndex(3);
	SetDataType(MSC_ASYM);
	if(dMenuPlot5->IsEntryChecked(M_PLOT_GRAPH_MSC_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_HISTO_MSC_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFT_MSC_ASYM))
	  if(PlotDFT()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFTPROF_MSC_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B3);
	  }
	break;
	
      case M_DATA_MSC_ASYM_B4:
	SetMSCAsymDataIndex(4);
	SetDataType(MSC_ASYM);
	if(dMenuPlot5->IsEntryChecked(M_PLOT_GRAPH_MSC_ASYM))
	  if(PlotGraphs()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_HISTO_MSC_ASYM))
	  if(PlotHistograms()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFT_MSC_ASYM))
	  if(PlotDFT()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	if(dMenuPlot5->IsEntryChecked(M_PLOT_DFTPROF_MSC_ASYM))
	  if(PlotDFTProfile()){
	    dMenuBlock5->UnCheckEntries();
	    dMenuBlock5->CheckEntry(M_DATA_PMT_ASYM_B4);
	  }
	break;



	/*MSC_YIELD*************************************/

      case M_DATA_MSC_YIELD_SUM:
	SetMSCYieldDataIndex(0);
	SetDataType(MSC_YIELD);
	if(dMenuPlot6->IsEntryChecked(M_PLOT_GRAPH_MSC_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_MSC_YIELD_SUM);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_HISTO_MSC_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_MSC_YIELD_SUM);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFT_MSC_YIELD))
	  if(PlotDFT()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_MSC_YIELD_SUM);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFTPROF_MSC_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_MSC_YIELD_SUM);
	  }
	break;

      case M_DATA_MSC_YIELD_B1:
	SetMSCYieldDataIndex(1);
	SetDataType(MSC_YIELD);
	if(dMenuPlot6->IsEntryChecked(M_PLOT_GRAPH_MSC_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_HISTO_MSC_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFT_MSC_YIELD))
	  if(PlotDFT()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFTPROF_MSC_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B1);
	  }
	break;

      case M_DATA_MSC_YIELD_B2:
	SetMSCYieldDataIndex(2);
	SetDataType(MSC_YIELD);
	if(dMenuPlot6->IsEntryChecked(M_PLOT_GRAPH_MSC_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_HISTO_MSC_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFT_MSC_YIELD))
	  if(PlotDFT()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFTPROF_MSC_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B2);
	  }
	break;

      case M_DATA_MSC_YIELD_B3:
	SetMSCYieldDataIndex(3);
	SetDataType(MSC_YIELD);
	if(dMenuPlot6->IsEntryChecked(M_PLOT_GRAPH_MSC_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_HISTO_MSC_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFT_MSC_YIELD))
	  if(PlotDFT()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFTPROF_MSC_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B3);
	  }
	break;
	
      case M_DATA_MSC_YIELD_B4:
	SetMSCYieldDataIndex(4);
	SetDataType(MSC_YIELD);
	if(dMenuPlot6->IsEntryChecked(M_PLOT_GRAPH_MSC_YIELD))
	  if(PlotGraphs()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_HISTO_MSC_YIELD))
	  if(PlotHistograms()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFT_MSC_YIELD))
	  if(PlotDFT()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	if(dMenuPlot6->IsEntryChecked(M_PLOT_DFTPROF_MSC_YIELD))
	  if(PlotDFTProfile()){
	    dMenuBlock6->UnCheckEntries();
	    dMenuBlock6->CheckEntry(M_DATA_PMT_YIELD_B4);
	  }
	break;


	/*SUMMARY*************************************/

      case M_DATA_SUM_SUM:
	SetSummaryDataIndex(0);
	SetDataType(SUMMARY);
	if(PlotGraphs()){
	  dMenuBlock7->UnCheckEntries();
	  dMenuBlock7->CheckEntry(M_DATA_SUM_SUM);
	}
	break;

      case M_DATA_SUM_B1:
	SetSummaryDataIndex(1);
	SetDataType(SUMMARY);
	if(PlotGraphs()){
	  dMenuBlock7->UnCheckEntries();
	  dMenuBlock7->CheckEntry(M_DATA_SUM_B1);
	}
	break;

      case M_DATA_SUM_B2:
	SetSummaryDataIndex(2);
	SetDataType(SUMMARY);
	if(PlotGraphs()){
	  dMenuBlock7->UnCheckEntries();
	  dMenuBlock7->CheckEntry(M_DATA_SUM_B2);
	}
	break;

      case M_DATA_SUM_B3:
	SetSummaryDataIndex(3);
	SetDataType(SUMMARY);
	if(PlotGraphs()){
	  dMenuBlock7->UnCheckEntries();
	  dMenuBlock7->CheckEntry(M_DATA_SUM_B3);
	}
	break;
	
      case M_DATA_SUM_B4:
	SetSummaryDataIndex(4);
	SetDataType(SUMMARY);
	if(PlotGraphs()){
	  dMenuBlock7->UnCheckEntries();
	  dMenuBlock7->CheckEntry(M_DATA_SUM_B4);
	}
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


