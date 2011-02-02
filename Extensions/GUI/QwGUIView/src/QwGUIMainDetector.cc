//To Do: NewDataInit() probably needs more attention with regard to opening
//       new files, when one is currently loaded!
//       
//       Edit starting with the newly created function ::CleanUp() ...

#include <QwGUIMainDetector.h>

void PrintMemInfo(const char* text) {
  
  MemInfo_t info;

  gSystem->GetMemInfo(&info);
  
  printf("Memory = %d %s\n",info.fMemUsed,text);
  printf("Swap   = %d %s\n",info.fSwapUsed,text); 
}


ClassImp(QwGUIMainDetector);

using namespace QwParityDB;

QwGUIMainDetectorDataStructure::QwGUIMainDetectorDataStructure(Int_t ID, Int_t Run, TString Type) 
//   : TObject()
{ 
  CurrentTree = NULL;
  CurrentFile = NULL;
  ThisDetectorBranch = NULL;
  MPSCntLeaf = NULL;
  DetectorName = " ";        
  DataType = Type;
  DetectorID   = ID;        
  RunNumber    = Run;
  HistoMode = kFalse;

  NumTreeLeafs = 0;     
  ActiveTreeLeaf = -1;    
  EventCounter = 0;      
  NumTreeEvents = 0;

};

Int_t QwGUIMainDetectorDataStructure::SetTree(TTree *tree)
{
  NumTreeLeafs = 0;
  if(tree){
    HistoMode = kFalse;
    CurrentTree = tree;
    NumTreeEvents = CurrentTree->GetEntries();
    ThisDetectorBranch = CurrentTree->GetBranch(DetectorName);
    MPSCntLeaf = CurrentTree->GetLeaf("mps_counter");
    if(ThisDetectorBranch){
      
      EventOptions *fft = NULL;
      TGraph *grp = NULL;
      TObjArray *leafes = ThisDetectorBranch->GetListOfLeaves();
      TObject *obj;
      TString objname;
      TIter *next;
      Int_t c = 0;
      next = new TIter(leafes->MakeIterator());
      obj = next->Next();
      while(obj){

	objname = obj->GetName();
	
// 	if(onjname.Contains("sequence_number") ||
// 	   onjname.Contains("num_samples") ||
// 	   onjname.Contains("Device_Error_Code")) continue;

	TreeLeafName.push_back(objname.Data());
	if(TreeLeafName[c].Contains("block") || TreeLeafName[c].Contains("sum")){

	  if(DetectorName.Contains("asym")){	    
	      TreeLeafYUnits.push_back("Asymmetry");
	  }
	  else{
	    if(TreeLeafName[c].Contains("raw"))
	      TreeLeafYUnits.push_back("Samples");
	    else
	      TreeLeafYUnits.push_back("Amplitude [V/#muA]");
	  }
	  TreeLeafXUnits.push_back("Time [sec]");
	}
	else{
	  TreeLeafXUnits.push_back("");
	  TreeLeafYUnits.push_back("");
	}
	
	DataName.push_back(Form("%s_%s",DetectorName.Data(),TreeLeafName[c].Data()));
	grp = new TGraph();
	grp->SetName(Form("%s_grp",DataName[c].Data()));//Form("%s.%s",DetectorName,TreeLeafName[c].data())));
	grp->SetTitle(Form("Run %d %s",GetRunNumber(),DataName[c].Data()));//Form("%s.%s",DetectorName,TreeLeafName[c])));
	DataGraph.push_back(grp);
// 	TreeLeafMenuID.push_back(IDOFFSET+DetectorID*100+c); 
	fft = new EventOptions;
	fft->calcFlag    = kFalse;
	fft->cancelFlag  = kFalse;
	fft->changeFlag  = kFalse;
	fft->Start       = 0;
	fft->Length      = -1; 
	fft->TotalLength = 0;
	TreeLeafFFTOpts.push_back(fft);
	TreeLeafFFTCalc.push_back(kFalse);

	TreeLeafMean.push_back(0); 
	TreeLeafSum.push_back(0);  
	TreeLeafSumSq.push_back(0);
	TreeLeafMin.push_back(0);  
	TreeLeafMax.push_back(0);  
	TreeLeafRMS.push_back(0);  
	TreeLeafError.push_back(0);
	StatEventCounter.push_back(0);
	obj = next->Next();
	NumTreeLeafs++;
	c++;
      }
      delete next;

      TreeLeafData = new Double_t[NumTreeLeafs]; 
      ThisDetectorBranch->SetAddress(TreeLeafData);
      EventCounter = 0;
    }
    if(MPSCntLeaf) {
      MPSCntLeaf->ResetAddress(&MPSCounter);
    }

  }
  return NumTreeLeafs;
};

Int_t QwGUIMainDetectorDataStructure::SetHistograms(RDataContainer *cont, TTree *tree)
{
  TH1F *tmp = NULL;

  NumTreeLeafs = 0;
  if(tree && cont){

    HistoMode = kTrue;
    CurrentTree = tree;
    CurrentFile = cont;
//     NumTreeEvents = CurrentTree->GetEntries();
    ThisDetectorBranch = CurrentTree->GetBranch(DetectorName);
//     MPSCntLeaf = CurrentTree->GetLeaf("mps_counter");
    if(ThisDetectorBranch){
      
//       EventOptions *fft = NULL;
//       TGraph *grp = NULL;
      TObjArray *leafes = ThisDetectorBranch->GetListOfLeaves();
      TString objname;
      TObject *obj;
      TIter *next;
      Int_t c = 0;
      next = new TIter(leafes->MakeIterator());
      obj = next->Next();
      while(obj){	
	objname = obj->GetName();
	
	if(objname.Contains("sequence_number") ||
	   objname.Contains("num_samples") ||
	   objname.Contains("Device_Error_Code")) { obj = next->Next(); continue; }
	
	TreeLeafName.push_back(objname.Data());
	if(TreeLeafName[c].Contains("_sum")) TreeLeafName[c].ReplaceAll("_sum",4,"",0);	
	DataName.push_back(Form("%s_%s",DetectorName.Data(),TreeLeafName[c].Data()));
	printf("DataName = %s\n",DataName[c].Data());
	
	if(DetectorName.Contains("asym")){	    
	  TreeLeafXUnits.push_back("Asymmetry");
	  tmp = (TH1F*)CurrentFile->ReadData(Form("hel_histo/%s",DataName[c].Data()));
	}
	else{
	  if(TreeLeafName[c].Contains("raw"))
	    TreeLeafXUnits.push_back("Samples");
	  else
	    TreeLeafXUnits.push_back("Amplitude [V/#muA]");
	  
	  tmp = (TH1F*)CurrentFile->ReadData(Form("mps_histo/%s",DataName[c].Data()));
	}



	if(tmp){

	  TH1D *hst = new TH1D(*(TH1D*)(tmp->Clone()));	  
	  hst->SetTitle(Form("Run %d %s",GetRunNumber(),DataName[c].Data()));
	  hst->GetXaxis()->SetTitle(TreeLeafXUnits[c].Data());
	  hst->GetXaxis()->SetRangeUser(tmp->GetMean()-10*tmp->GetRMS(), tmp->GetMean()+10*tmp->GetRMS());
	  hst->GetXaxis()->CenterTitle();
	  hst->GetXaxis()->SetTitleSize(0.06);
	  hst->GetXaxis()->SetLabelSize(0.06);
	  hst->GetXaxis()->SetTitleOffset(1.25);
	  hst->GetXaxis()->SetTitleColor(1);
	  hst->SetNdivisions(506,"X");
	  hst->GetYaxis()->SetLabelSize(0.06);

	  hst->SetDirectory(0);

	  DataHisto.push_back(hst);
	  
	  TreeLeafMean.push_back(0); 
	  TreeLeafSum.push_back(0);  
	  TreeLeafSumSq.push_back(0);
	  TreeLeafMin.push_back(0);  
	  TreeLeafMax.push_back(0);  
	  TreeLeafRMS.push_back(0);  
	  TreeLeafError.push_back(0);
	  StatEventCounter.push_back(0);
	  NumTreeLeafs++;
	  c++;
	}
	obj = next->Next();
      }
      delete next;
    }
  }
  return NumTreeLeafs;
};


void QwGUIMainDetectorDataStructure::FillData(Double_t sample)
{
  Double_t data;
  Double_t scale = 1.0;
  if(DetectorName.Contains("asym")) scale = 4.0;

  for(uint i = 0; i < NumTreeLeafs; i++){
    Int_t flag = 0;
    data = TreeLeafData[i];    

    DataGraph[i]->SetPoint(EventCounter,sample/SAMPLING_RATE,data);
    if(data != 0){
      
      if(!StatEventCounter[i]){TreeLeafMin[i] = data; TreeLeafMax[i] = data; flag = 1;} 
      else{
	if(data < TreeLeafMin[i]) TreeLeafMin[i] = data;
	if(data > TreeLeafMax[i]) TreeLeafMax[i] = data;
      }
      TreeLeafSum[i] += data;
      TreeLeafSumSq[i] += data*data;
      StatEventCounter[i]++;
    }
  }
  EventCounter++;
}

void QwGUIMainDetectorDataStructure::CalculateStats()
{
  if(HistoMode){

    Double_t edge = 0;
    Double_t width = 0;
    
    for(uint i = 0; i < NumTreeLeafs; i++){
      TreeLeafMean[i] = DataHisto[i]->GetMean();
      TreeLeafRMS[i]  = DataHisto[i]->GetRMS();
      TreeLeafError[i] = DataHisto[i]->GetRMS()/sqrt(DataHisto[i]->GetEntries());

      TreeLeafMin[i] = 999999999;
      TreeLeafMax[i] = -999999999;

      for(int n = 0; n < DataHisto[i]->GetEntries(); n++){

	if(DataHisto[i]->GetBinContent(n) != 0){

	  edge = DataHisto[i]->GetBinLowEdge(n);
	  width = DataHisto[i]->GetBinWidth(n);
	  
	  if(edge < TreeLeafMin[i]) {TreeLeafMin[i] = edge;}
	  if(edge+width >= TreeLeafMax[i]) {TreeLeafMax[i] = edge+width;}
	}	
      }
    }
  }
  else{
    for(uint i = 0; i < NumTreeLeafs; i++){
      
      TreeLeafMean[i] = TreeLeafSum[i]/StatEventCounter[i];
      TreeLeafRMS[i]  = sqrt(TreeLeafSumSq[i]/StatEventCounter[i] - TreeLeafMean[i]*TreeLeafMean[i]);
      TreeLeafError[i] = TreeLeafRMS[i]/sqrt(StatEventCounter[i]);
      
      TreeLeafFFTOpts[i]->cancelFlag  = kFalse;
      TreeLeafFFTOpts[i]->changeFlag  = kTrue;
      TreeLeafFFTOpts[i]->Start       = 0;
      TreeLeafFFTOpts[i]->Length      = EventCounter; 
      TreeLeafFFTOpts[i]->TotalLength = EventCounter;
    }
  }
}


void QwGUIMainDetectorDataStructure::FillHistograms()
{
  if(!DataGraph.size() || HistoMode) return;

  TH1D *hst  = NULL;
  TGraph * grp = NULL;
  Double_t y;

  for(uint i = 0; i < NumTreeLeafs; i++){

    grp = DataGraph[i];
    if(grp){    
      hst = new TH1D(Form("%s_hst",DataName[i].Data()),Form("Run %d %s",GetRunNumber(),DataName[i].Data()),
		     1000,TreeLeafMean[i]-5*TreeLeafRMS[i],TreeLeafMean[i]+5*TreeLeafRMS[i]);
      hst->SetDirectory(0);
      hst->SetBit(TH1::kCanRebin);

      for(int j = 0; j < grp->GetN(); j++){	
	y = grp->GetY()[j];
	if(y != 0){
	  hst->Fill(y);
	}
      }
      TreeLeafRMSError.push_back(hst->GetRMSError());
      hst->SetDirectory(0);
      hst->GetXaxis()->SetTitle(TreeLeafYUnits[i].Data());
      hst->GetXaxis()->CenterTitle();
      hst->GetXaxis()->SetTitleSize(0.06);
      hst->GetXaxis()->SetLabelSize(0.06);
      hst->GetXaxis()->SetTitleOffset(1.25);
      hst->GetXaxis()->SetTitleColor(1);
      hst->SetNdivisions(506,"X");
      hst->GetYaxis()->SetLabelSize(0.06);
  
      grp->GetYaxis()->SetRangeUser(TreeLeafMin[i],TreeLeafMax[i]);
      grp->GetXaxis()->SetTitle(TreeLeafXUnits[i].Data());
      grp->GetXaxis()->CenterTitle();
      grp->GetXaxis()->SetTitleSize(0.06);
      grp->GetXaxis()->SetTitleColor(1);
      grp->GetXaxis()->SetLabelSize(0.06);
      grp->GetXaxis()->SetTitleOffset(1.25);
      grp->GetYaxis()->SetTitle(TreeLeafYUnits[i].Data());
      grp->GetYaxis()->CenterTitle();
      grp->GetYaxis()->SetTitleSize(0.06);
      grp->GetYaxis()->SetLabelSize(0.06);
      grp->GetYaxis()->SetTitleOffset(1.5);

      DataHisto.push_back(hst);
    }
  }

}

Int_t QwGUIMainDetectorDataStructure::CalculateFFTs(EventOptions *opts, UInt_t leafind)
{
  if(!DataGraph.size() || HistoMode) return 0;

  if(leafind < 0 || leafind >= NumTreeLeafs) return 0;

  UInt_t ind = leafind;

  if(DataGraph.size() <= ind) return 0;
  if(!DataGraph[ind]) return 0;

  if(TreeLeafName[ind].Contains("sequence_number") ||
     TreeLeafName[ind].Contains("num_samples") ||
     TreeLeafName[ind].Contains("Device_Error_Code")) { 
    FFTHisto.push_back(NULL);
    FFTProfile.push_back(NULL);
    return 0;
  }

  if(opts){
    if(opts->cancelFlag) return 0;
    if(!opts->changeFlag) return 0;
    TreeLeafFFTOpts[ind]->cancelFlag  = opts->cancelFlag;
    TreeLeafFFTOpts[ind]->changeFlag  = opts->changeFlag;
    TreeLeafFFTOpts[ind]->Start       = opts->Start;
    TreeLeafFFTOpts[ind]->Length      = opts->Length; 
    TreeLeafFFTOpts[ind]->TotalLength = opts->TotalLength;
  }
  
  Int_t RATE = SAMPLING_RATE;
  if(DetectorName.Contains("asym")) RATE = (Int_t)(SAMPLING_RATE/4);
  Double_t y;
  
  TProfile *prf = new TProfile(Form("%s_prf",DataName[ind].Data()),
			       Form("Run %d %s FFT Data",GetRunNumber(),DataName[ind].Data()),
			       TreeLeafFFTOpts[ind]->Length,1.0*TreeLeafFFTOpts[ind]->Start/RATE,
			       1.0*(TreeLeafFFTOpts[ind]->Start+TreeLeafFFTOpts[ind]->Length)/RATE);
  if(!prf) return 0;
  
  for(uint n = TreeLeafFFTOpts[ind]->Start; n < TreeLeafFFTOpts[ind]->Length+TreeLeafFFTOpts[ind]->Start; n++){
    if(n >= 0 && n < (uint)DataGraph[ind]->GetN()){
      y = DataGraph[ind]->GetY()[n];
      if(y != 0){
	prf->Fill(1.0*n/RATE,y-TreeLeafMean[ind]);      
      }
    }
  }
  TH1D *fft = NULL;

  TH1D *hst = prf->ProjectionX(Form("%s_prj",DataName[ind].Data()));
  //	TH1D *hist2 = new TH1D(*(TH1D*)hst->Clone());
  if(!hst) {delete prf; return 0;}

  TH1 *fftmag = NULL;
  TVirtualFFT::SetTransform(0);
  fftmag = ((TH1D*)hst)->FFT(fftmag,"MAG");
  fftmag->SetName(Form("%s_fft-mag",DataName[ind].Data()));
  fftmag->SetTitle(Form("Run %d %s FFT Magnitude",GetRunNumber(),DataName[ind].Data()));
  
  fft = new TH1D(Form("%s_fft",DataName[ind].Data()),
		 Form("Run %d %s FFT",GetRunNumber(),DataName[ind].Data()),
		 (Int_t)(TreeLeafFFTOpts[ind]->Length/2.0)-1,0.0,(Int_t)(RATE/2.0));
  if(!fft){delete prf; delete hst; return 0;}

  for(int i = 0; i < (Int_t)(TreeLeafFFTOpts[ind]->Length/2); i++) {
    fft->SetBinContent(i,fftmag->GetBinContent(i)/TreeLeafFFTOpts[ind]->Length);
  }
  TVirtualFFT *transf = TVirtualFFT::GetCurrentTransform();
  delete transf;

  fft->GetXaxis()->SetTitle("Frequency [Hz]");
  fft->GetXaxis()->CenterTitle();
  fft->GetXaxis()->SetTitleSize(0.06);
  fft->GetXaxis()->SetLabelSize(0.06);
  fft->GetXaxis()->SetTitleColor(1);
  fft->GetXaxis()->SetTitleOffset(1.25);
  fft->GetYaxis()->SetTitle("Amplitude [V/#muA]");
  fft->GetYaxis()->CenterTitle();
  fft->GetYaxis()->SetTitleSize(0.06);
  fft->GetYaxis()->SetLabelSize(0.06);
  fft->GetYaxis()->SetTitleOffset(1.25);
  
  fft->SetDirectory(0);
  FFTHisto.push_back(fft);
//   FFTProfile.push_back(prf);
  delete hst;
  delete fftmag;
  delete prf;
//   prf->SetDirectory(0);
  
  return 1;
  
}

void QwGUIMainDetectorDataStructure::Clean()
{
  //PrintMemInfo("Before QwGUIMainDetectorDataStructure::Clean()");
  
  for(uint i = 0; i < DataGraph.size(); i++){    
    if(DataGraph[i]) { delete DataGraph[i]; DataGraph[i] = NULL;}
  }
  DataGraph.clear();
  for(uint i = 0; i < DataHisto.size(); i++){    
    if(DataHisto[i]) { delete DataHisto[i]; DataHisto[i] = NULL;}
  }
  DataHisto.clear();
  for(uint i = 0; i < FFTHisto.size(); i++){    
    if(FFTHisto[i]) { delete FFTHisto[i]; FFTHisto[i] = NULL;}
  }
  FFTHisto.clear();
  for(uint i = 0; i < FFTProfile.size(); i++){    
    if(FFTProfile[i]) { delete FFTProfile[i]; FFTProfile[i] = NULL;}
  }
  FFTProfile.clear();

  for(uint i = 0; i < TreeLeafFFTOpts.size(); i++){    
    if(TreeLeafFFTOpts[i]) { delete TreeLeafFFTOpts[i]; TreeLeafFFTOpts[i] = NULL;}
  }
  TreeLeafFFTOpts.clear();

  CurrentTree = NULL;
  ThisDetectorBranch = NULL;
  DetectorName = " ";        
  DetectorID   = -1;        

  NumTreeLeafs = 0;     
  ActiveTreeLeaf = -1;    
  EventCounter = 0;      
  NumTreeEvents = 0;
  RunNumber    = -1;

  if(TreeLeafData) delete[] TreeLeafData; TreeLeafData = NULL;

  TreeLeafName.clear();      
  DataName.clear();       
  TreeLeafMean.clear();      
  TreeLeafSum.clear();        
  TreeLeafSumSq.clear();     
  TreeLeafMin.clear();       
  TreeLeafMax.clear();       
  TreeLeafRMS.clear();       
  TreeLeafError.clear();     
  TreeLeafFFTCalc.clear();

  //PrintMemInfo("After QwGUIMainDetectorDataStructure::Clean()");
    
}

const char* QwGUIMainDetectorDataStructure::GetTreeLeafName(UInt_t n)
{
  if(n < 0 || n >= TreeLeafName.size()) return NULL;

  return TreeLeafName[n];
}

Double_t QwGUIMainDetectorDataStructure::GetTreeLeafMin(UInt_t n)
{
  if(n < 0 || n >= TreeLeafMin.size()) return 0;

  return TreeLeafMin[n];
}

Double_t QwGUIMainDetectorDataStructure::GetTreeLeafMax(UInt_t n)
{
  if(n < 0 || n >= TreeLeafMax.size()) return 0;

  return TreeLeafMax[n];
}

Double_t QwGUIMainDetectorDataStructure::GetTreeLeafMean(UInt_t n)
{
  if(n < 0 || n >= TreeLeafMean.size()) return 0;

  return TreeLeafMean[n];
}

Double_t QwGUIMainDetectorDataStructure::GetTreeLeafRMS(UInt_t n)
{
  if(n < 0 || n >= TreeLeafRMS.size()) return 0;

  return TreeLeafRMS[n];
}

Double_t QwGUIMainDetectorDataStructure::GetTreeLeafError(UInt_t n)
{
  if(n < 0 || n >= TreeLeafError.size()) return 0;

  return TreeLeafError[n];
}

Double_t QwGUIMainDetectorDataStructure::GetTreeLeafRMSError(UInt_t n)
{
  if(n < 0 || n >= TreeLeafRMSError.size()) return 0;

  return TreeLeafRMSError[n];
}


void QwGUIMainDetectorDataStructure::DrawHistogram(UInt_t i)
{
  if(i < 0 || i >= DataHisto.size()) return;  
  if(DataHisto[i])
    DataHisto[i]->Draw("");
}

TObject *QwGUIMainDetectorDataStructure::GetHistogram(UInt_t i)
{
  if(i < 0 || i >= DataHisto.size()) return NULL;  
  return DataHisto[i];
}

void QwGUIMainDetectorDataStructure::DrawGraph(UInt_t i)
{
  if(i < 0 || i >= DataGraph.size()) return;
  if(DataGraph[i]){
    DataGraph[i]->Draw("ap");
    DataGraph[i]->GetYaxis()->UnZoom();
  }
}

TObject *QwGUIMainDetectorDataStructure::GetGraph(UInt_t i)
{
  if(i < 0 || i >= DataGraph.size()) return NULL;  
  return DataGraph[i];
}

void QwGUIMainDetectorDataStructure::DrawFFTHistogram(UInt_t i)
{
  if(TreeLeafName[i].Contains("sequence_number") ||
     TreeLeafName[i].Contains("num_samples") ||
     TreeLeafName[i].Contains("Device_Error_Code")) { return;}

  if(i < 0 || i >= FFTHisto.size()) return;
  if(FFTHisto[i])
    FFTHisto[i]->Draw("");
}

TObject *QwGUIMainDetectorDataStructure::GetFFTHistogram(UInt_t i)
{
  if(i < 0 || i >= FFTHisto.size()) return NULL;  
  return FFTHisto[i];
}


void QwGUIMainDetectorDataStructure::DrawFFTProfile(UInt_t i)
{
  if(TreeLeafName[i].Contains("sequence_number") ||
     TreeLeafName[i].Contains("num_samples") ||
     TreeLeafName[i].Contains("Device_Error_Code")) { return;}

  if(i < 0 || i >= FFTProfile.size()) return;
  if(FFTProfile[i])
    FFTProfile[i]->Draw("");
}

TObject *QwGUIMainDetectorDataStructure::GetFFTProfile(UInt_t i)
{
  if(i < 0 || i >= FFTProfile.size()) return NULL;  
  return FFTProfile[i];
}

QwGUIMainDetectorDataType::QwGUIMainDetectorDataType(UInt_t thisID, Bool_t summary)
//   : TObject()
{
  ID = thisID;
  NumberOfLeafs = 0;
  dSummary = summary;
  dLinkedInd = -1;
  dNumberOfPads = 0;
  CurrentTree = NULL;
  CurrentFile = NULL;
  HistoMode = kFalse;
//   dTabLayout = NULL;
  dMenuBar = NULL;          
  dMenuPlot = NULL;         
  dMenuBlock = NULL;        
  dCanvas = NULL;      
  dFrame = NULL;       
}

void QwGUIMainDetectorDataType::Clean() 
{
  //PrintMemInfo("Before QwGUIMainDetectorDataType::Clean()");

  for(uint i = 0; i < dDetDataStr.size(); i++){
    if(dDetDataStr[i]){
      dDetDataStr[i]->Clean();
      delete dDetDataStr[i];
    }
  }
  for(uint i = 0; i < dMeanGraph.size(); i++){
    if(dMeanGraph[i]){
      delete dMeanGraph[i];
    }
  }
  for(uint i = 0; i < dRMSGraph.size(); i++){
    if(dRMSGraph[i]){
      delete dRMSGraph[i];
    }
  }

  dDetDataStr.clear();
  dMeanGraph.clear();
  dRMSGraph.clear();

  dLinkedTypes.clear();
  dLinkedInd = -1;
  
  delete dMenuPlotLayout;
  delete dMenuBlockLayout;
  delete dMenuBarLayout;
  delete dCanvasLayout;
  delete dMenuBar;          
  delete dMenuPlot;         
  delete dMenuBlock;        
  delete dCanvas;      
  delete dFrame;       
  delete dTab;

  //PrintMemInfo("After QwGUIMainDetectorDataType::Clean()");
}

UInt_t QwGUIMainDetectorDataType::SetHistograms(RDataContainer *cont, TTree *tree, vector <TString> DetNames, UInt_t thisRun)
{
  if(dSummary) return 0;

  UInt_t temp = 0;
  if(cont && tree){
    HistoMode = kTrue;

    QwGUIMainDetectorDataStructure * dataStr;
    CurrentTree = tree;
    CurrentFile = cont;
    for(uint i = 0; i < DetNames.size(); i++){
      TBranch *branch = CurrentTree->GetBranch(DetNames[i].Data());
      if(branch){
	dataStr = new QwGUIMainDetectorDataStructure(ID+200+i,thisRun,Type);
	dataStr->SetDetectorName(DetNames[i].Data());
	dDetDataStr.push_back(dataStr);
	temp = dDetDataStr[i]->SetHistograms(cont,tree);
	if(!i)  NumberOfLeafs = temp;
      }
    }
  }
  return  NumberOfLeafs;
}

UInt_t QwGUIMainDetectorDataType::SetTree(RDataContainer *cont, TTree *tree, vector <TString> DetNames, UInt_t thisRun)
{
  if(dSummary) return 0;
  UInt_t temp = 0;
  if(tree && tree){
    HistoMode = kFalse;
    QwGUIMainDetectorDataStructure * dataStr;
    CurrentTree = tree;
    CurrentFile = cont;
    for(uint i = 0; i < DetNames.size(); i++){
      TBranch *branch = CurrentTree->GetBranch(DetNames[i].Data());
      if(branch){
	dataStr = new QwGUIMainDetectorDataStructure(ID+200+i,thisRun,Type);
	dataStr->SetDetectorName(DetNames[i].Data());
	dDetDataStr.push_back(dataStr);
	temp = dDetDataStr[i]->SetTree(tree);
	if(!i)  NumberOfLeafs = temp;
      }
    }
  }
  return  NumberOfLeafs;
}

void QwGUIMainDetectorDataType::FillData(Double_t sample)
{
  if(!dSummary){
    for(uint i = 0; i < dDetDataStr.size(); i++){
      if(dDetDataStr[i])
	dDetDataStr[i]->FillData(sample);
    }
  }
}

void QwGUIMainDetectorDataType::ProcessData(const char* SummaryTitle)
{

  if(!dSummary){

    TGraphErrors *grph = NULL;

    for(uint i = 0; i < dDetDataStr.size(); i++){
      dDetDataStr[i]->CalculateStats();
      dDetDataStr[i]->FillHistograms();
      if(!HistoMode){
	for(uint j = 0; j < NumberOfLeafs; j++){
	  dDetDataStr[i]->CalculateFFTs(0,j);      
	}
      }
    }
    
    for(uint j = 0; j < NumberOfLeafs; j++){
      grph = new TGraphErrors(dDetDataStr.size());
      grph->SetName(Form("%s_SignalMean",GetType()));
      grph->SetTitle(Form("%s Mean: %s %s",GetType(),dDetDataStr[0]->GetTreeLeafName(j),SummaryTitle));
      
      for(uint i = 0; i < dDetDataStr.size(); i++){
	grph->SetPoint(i,i+1,dDetDataStr[i]->GetTreeLeafMean(j));
	grph->SetPointError(i,0,dDetDataStr[i]->GetTreeLeafError(j));      
      }    

      grph->GetXaxis()->CenterTitle();
      grph->GetXaxis()->SetTitleSize(0.06);
      grph->GetXaxis()->SetTitleColor(1);
      grph->GetXaxis()->SetLabelSize(0.06);
      grph->GetXaxis()->SetTitleOffset(1.25);
//       grph->GetYaxis()->SetTitle(TreeLeafYUnits[i].Data());
      grph->GetYaxis()->CenterTitle();
      grph->GetYaxis()->SetTitleSize(0.06);
      grph->GetYaxis()->SetLabelSize(0.06);
      grph->GetYaxis()->SetTitleOffset(1.5);

      
      dMeanGraph.push_back(grph);
      
      grph = new TGraphErrors(dDetDataStr.size());
      grph->SetName(Form("%s_SignalRMS",GetType()));
      grph->SetTitle(Form("%s RMS: %s %s",GetType(),dDetDataStr[0]->GetTreeLeafName(j),SummaryTitle));
      
      for(uint i = 0; i < dDetDataStr.size(); i++){
	grph->SetPoint(i,i+1,dDetDataStr[i]->GetTreeLeafRMS(j));
	if(dDetDataStr[i]->GetHistogram(j))
	  grph->SetPointError(i,0,((TH1D*)dDetDataStr[i]->GetHistogram(j))->GetRMSError());
	else
	  grph->SetPointError(i,0,0);
      }    

      grph->GetXaxis()->CenterTitle();
      grph->GetXaxis()->SetTitleSize(0.06);
      grph->GetXaxis()->SetTitleColor(1);
      grph->GetXaxis()->SetLabelSize(0.06);
      grph->GetXaxis()->SetTitleOffset(1.25);
//       grph->GetYaxis()->SetTitle(TreeLeafYUnits[i].Data());
      grph->GetYaxis()->CenterTitle();
      grph->GetYaxis()->SetTitleSize(0.06);
      grph->GetYaxis()->SetLabelSize(0.06);
      grph->GetYaxis()->SetTitleOffset(1.5);
      
      dRMSGraph.push_back(grph);
    }
  }
  else{

    //Set the number of leafs for the summary structure to
    //the largest number found for all branches in the data.
    //Must be careful to check for existence in each branch,
    //when plotting the summary data.

    NumberOfLeafs = 0;
    for(uint l = 0; l < dLinkedTypes.size(); l++){
      if(dLinkedTypes[l] && 
	 dLinkedTypes[l]->GetNumberOfLeafs() > NumberOfLeafs) {
	NumberOfLeafs = dLinkedTypes[l]->GetNumberOfLeafs();
	dLinkedInd = l;
      }
    }    
  }
}

void QwGUIMainDetectorDataType::PlotData()
{
  TCanvas *mc = dCanvas->GetCanvas();
  if(!mc) return;

  if(!dSummary){    

    if(GetNumDetectors() < 1) return;
    for(uint i = 0; i < GetNumDetectors(); i++){
     
      if(GetDetector(i)){
	for(uint j = 0; j < NumberOfLeafs; j++){
	
	  if(dLeafMenuID[j] == CurrentLeafMenuItemID){

	    if(CurrentPlotMenuItemID == HistoMenuID){
	    
	      mc->cd(i+1);
	      GetDetector(i)->DrawHistogram(j);
	      PlotType = PLOT_TYPE_HISTO;
	      gPad->SetLogy(1);

	    }
	    if(CurrentPlotMenuItemID == GraphMenuID){
	      
	      mc->cd(i+1);
	      GetDetector(i)->DrawGraph(j);
	      PlotType = PLOT_TYPE_GRAPH;
	      gPad->SetLogy(0);
	      
	    }
	    if(CurrentPlotMenuItemID == FFTMenuID){
	      
	      mc->cd(i+1);
	      GetDetector(i)->DrawFFTHistogram(j);
	      PlotType = PLOT_TYPE_HISTO;
	      gPad->SetLogy(1);
	      
	    }
	    if(CurrentPlotMenuItemID == ProfMenuID){
	      
	      mc->cd(i+1);
	      GetDetector(i)->DrawFFTProfile(j);
	      PlotType = PLOT_TYPE_HISTO;
	      
	    }
	  }
	}
      }
      mc->Modified();
      mc->Update();
    }
  }
  else{    
    for(uint i = 0; i < dLinkedTypes.size(); i++){
      if(dLinkedTypes[i])
	for(uint j = 0; j < NumberOfLeafs; j++){
	  if(dLeafMenuID[j] == CurrentLeafMenuItemID){
	    mc->cd(2*(i)+1);
	    dLinkedTypes[i]->DrawMeanGraph(j);
	    gPad->SetLogy(0);      
	    mc->cd(2*(i+1));
	    dLinkedTypes[i]->DrawRMSGraph(j);
	    PlotType = PLOT_TYPE_GRAPH;
	    gPad->SetLogy(0);      
	  }
	}
    }
    mc->Modified();
    mc->Update();
    
  }
}

QwGUIMainDetectorDataStructure* QwGUIMainDetectorDataType::GetSelectedDetector()
{
  TCanvas *mc = dCanvas->GetCanvas();
  if(!mc) return NULL;
  
  UInt_t pad = mc->GetSelectedPad()->GetNumber();
  UInt_t ind = pad-1;
  if(ind < 0 || ind >= GetNumDetectors()) return NULL;
  
  return GetDetector(ind);
}

TObject* QwGUIMainDetectorDataType::GetSelectedPlot()
{
  QwGUIMainDetectorDataStructure* det = NULL;
  TCanvas *mc = dCanvas->GetCanvas();
  if(!mc) return NULL;

  UInt_t pad = mc->GetSelectedPad()->GetNumber();
  UInt_t ind = pad-1;

  if(!dSummary){    

    if(ind < 0 || ind >= GetNumDetectors()) return NULL;
    
    det = GetDetector(ind);
    if(!det) return NULL;

    for(uint j = 0; j < NumberOfLeafs; j++){
	
      if(dLeafMenuID[j] == CurrentLeafMenuItemID){

	if(CurrentPlotMenuItemID == HistoMenuID){	    
	  return det->GetHistogram(j);
	}
	if(CurrentPlotMenuItemID == GraphMenuID){
	  return det->GetGraph(j);
	}
	if(CurrentPlotMenuItemID == FFTMenuID){
	  return det->GetFFTHistogram(j);
	}
	if(CurrentPlotMenuItemID == ProfMenuID){
	  return det->GetFFTProfile(j);
	}
      }
    }
    
  }
  else{    

    if(!((pad-1)%2)){ 
      ind = (pad-1)/2;
    }
    else{
      ind = pad/2-1;
    }

    if(ind < 0 || ind >= dLinkedTypes.size()) return NULL;

    if(!dLinkedTypes[ind]) return NULL;

    for(uint j = 0; j < NumberOfLeafs; j++){
      if(dLeafMenuID[j] == CurrentLeafMenuItemID){

	if(!((pad-1)%2)){ 
	  return dLinkedTypes[ind]->GetMeanGraph(j);
	}
	else{
	  return dLinkedTypes[ind]->GetRMSGraph(j);
	}
      }
    }
  }
  return NULL;
}

UInt_t QwGUIMainDetectorDataType::GetCurrentLeafIndex()
{
  for(uint j = 0; j < NumberOfLeafs; j++){
    if(dLeafMenuID[j] == CurrentLeafMenuItemID){
      return j;
    }
  }
  return -1;
}

void QwGUIMainDetectorDataType::CalculateFFTs(EventOptions* fftopts)
{
//   for(int j = 0; j < NumberOfLeafs; j++){
	
//     if(dLeafMenuID[j] == CurrentLeafMenuItemID){
//       for(int i = 0; i < dSize; i++){
// 	if(dDetDataStr[i].CalculateFFT(fftopts,j)){
	  
// 	  dMenuPlot->EnableEntry(ProfMenuID);

	  
// 	}
//       }
//     }

//   }
}

TObject *QwGUIMainDetectorDataType::GetMeanGraph(UInt_t i)
{
  if(i < 0 || i >= dMeanGraph.size()) return NULL;
  
  return dMeanGraph[i];

}

TObject *QwGUIMainDetectorDataType::GetRMSGraph(UInt_t i)
{
  if(i < 0 || i >= dRMSGraph.size()) return NULL;
  
  return dRMSGraph[i];

}

void QwGUIMainDetectorDataType::DrawMeanGraph(UInt_t i)
{
  if(i < 0 || i >= dMeanGraph.size()) return;
  if(dMeanGraph[i]){
    
    dMeanGraph[i]->GetXaxis()->CenterTitle();
    dMeanGraph[i]->GetXaxis()->SetTitleSize(0.06);
    dMeanGraph[i]->GetXaxis()->SetTitleColor(1);
    dMeanGraph[i]->GetXaxis()->SetLabelSize(0.06);
    dMeanGraph[i]->GetXaxis()->SetTitleOffset(1.25);
    //       grph->GetYaxis()->SetTitle(TreeLeafYUnits[i].Data());
    dMeanGraph[i]->GetYaxis()->CenterTitle();
    dMeanGraph[i]->GetYaxis()->SetTitleSize(0.06);
    dMeanGraph[i]->GetYaxis()->SetLabelSize(0.06);
    dMeanGraph[i]->GetYaxis()->SetTitleOffset(1.5);
    dMeanGraph[i]->SetMarkerSize(1.0);
    dMeanGraph[i]->SetLineColor(2);

    dMeanGraph[i]->Draw("ap");
  }

}

void QwGUIMainDetectorDataType::DrawRMSGraph(UInt_t i)
{
  if(i < 0 || i >= dRMSGraph.size()) return;
  if(dRMSGraph[i]){

    dRMSGraph[i]->GetXaxis()->CenterTitle();
    dRMSGraph[i]->GetXaxis()->SetTitleSize(0.06);
    dRMSGraph[i]->GetXaxis()->SetTitleColor(1);
    dRMSGraph[i]->GetXaxis()->SetLabelSize(0.06);
    dRMSGraph[i]->GetXaxis()->SetTitleOffset(1.25);
    //    grph->GetYaxis()->SetTitle(TreeLeafYUnits[i].Data());
    dRMSGraph[i]->GetYaxis()->CenterTitle();
    dRMSGraph[i]->GetYaxis()->SetTitleSize(0.06);
    dRMSGraph[i]->GetYaxis()->SetLabelSize(0.06);
    dRMSGraph[i]->GetYaxis()->SetTitleOffset(1.5);
    dRMSGraph[i]->SetMarkerSize(1.0);
    dRMSGraph[i]->SetLineColor(2);

    dRMSGraph[i]->Draw("ap");
  }
}

TRootEmbeddedCanvas *QwGUIMainDetectorDataType::MakeDataTab(TGTab *dMDTab, 
							    const TGWindow *parent, 
							    const TGWindow *client,
							    Int_t w, Int_t h)
{
  if(!dMDTab) return NULL;
  dNumberOfPads = 0;
  MenuBaseID = ID;

  dTab = dMDTab->AddTab(Type.Data());
  dFrame = new TGVerticalFrame(dTab,10,10);
  if(!dFrame) return NULL;

  dMenuBar = new TGMenuBar(dFrame, 1, 1, kHorizontalFrame);
  if(!dMenuBar) return NULL;
  dMenuBlock = new TGPopupMenu(client);
  if(!dMenuBlock) return NULL;

  if(!dSummary){

    dMenuPlot = new TGPopupMenu(client);
    if(!dMenuPlot) return NULL;
  
    HistoMenuID = MenuBaseID+1;
    dMenuPlot->AddEntry("&Histogram", HistoMenuID);
    CurrentPlotMenuItemID = HistoMenuID;
    GraphMenuID = MenuBaseID+2;
    dMenuPlot->AddEntry("&Graph", GraphMenuID);
    FFTMenuID = MenuBaseID+3;
    dMenuPlot->AddEntry("&FFT Histogram", FFTMenuID);
//     ProfMenuID = MenuBaseID+4;
//     dMenuPlot->AddEntry("FFT &Profile",ProfMenuID);

    dMenuPlotLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
    dMenuBar->AddPopup("&Plot Type", dMenuPlot, dMenuPlotLayout);

    dMenuPlot->EnableEntry(HistoMenuID);
    dMenuPlot->EnableEntry(GraphMenuID);
    if(HistoMode){ 
      dMenuPlot->DisableEntry(GraphMenuID);
      dMenuPlot->DisableEntry(FFTMenuID);
//       dMenuPlot->DisableEntry(ProfMenuID);
    }

    dMenuPlot->EnableEntry(FFTMenuID);
    dMenuPlot->EnableEntry(ProfMenuID);

    dMenuPlot->CheckEntry(HistoMenuID);
    CurrentPlotMenuItemID = HistoMenuID;
    PlotType = PLOT_TYPE_HISTO;
  }

  for(uint i = 0; i < NumberOfLeafs; i++){
    dLeafMenuID.push_back(MenuBaseID+10+i);
    CurrentLeafMenuItemID = dLeafMenuID[0];
    if(!dSummary)
      dMenuBlock->AddEntry(dDetDataStr[0]->GetTreeLeafName(i),dLeafMenuID[i]);
    else
      if( dLinkedInd >= 0 && dLinkedTypes[dLinkedInd]->GetDetector(0) )
	dMenuBlock->AddEntry(dLinkedTypes[dLinkedInd]->GetDetector(0)->GetTreeLeafName(i),dLeafMenuID[i]);

    dMenuBlock->EnableEntry(dLeafMenuID[i]);
    if(!i){
      dMenuBlock->CheckEntry(dLeafMenuID[i]);
      CurrentLeafMenuItemID = dLeafMenuID[i];
    }
  }


  dMenuBlockLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  dMenuBar->AddPopup("&Data Block", dMenuBlock, dMenuBlockLayout);

  dMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
  dFrame->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuBlock->Associate(parent);
  if(!dSummary){dMenuPlot->Associate(parent);}

  dCanvas   = new TRootEmbeddedCanvas(Form("p%sC",GetType()), dFrame,10, 10);
  if(!dCanvas) return NULL;

  dCanvasLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,0, 0, 1, 2);
  dFrame->AddFrame(dCanvas, dCanvasLayout);
  dFrame->Resize(w,h);

  dTab->AddFrame(dFrame,new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY,2, 2, 5, 1));

  TCanvas *mc = dCanvas->GetCanvas();

  Int_t n = 0;
  Int_t xn = 0;
  Int_t yn = 0;  
  
  if(!dSummary) n = GetNumDetectors();
  else n = dLinkedTypes.size()*2;
  
  if(n > 1){
    if(n == 2) {xn = 2; yn = 1;}
    if(n == 3) {xn = 2; yn = 2;}
    if(n == 4) {xn = 2; yn = 2;}
    if(n == 5) {xn = 3; yn = 2;}
    if(n == 6) {xn = 3; yn = 2;}
    if(n == 7) {xn = 4; yn = 2;} 
    if(n == 8) {xn = 4; yn = 2;}
    if(n == 9) {xn = 3; yn = 3;}
    if(n == 10){xn = 4; yn = 3;}
    if(n == 11){xn = 4; yn = 3;}
    if(n == 12){xn = 4; yn = 3;}
    if(n == 13){xn = 4; yn = 4;}
    if(n == 14){xn = 4; yn = 4;}
    if(n == 15){xn = 4; yn = 4;}
    if(n == 16){xn = 4; yn = 4;}

    mc->Divide(xn,yn);
    dNumberOfPads = xn*yn;
  }
  else
    dNumberOfPads = 1;

  return dCanvas;
}


void QwGUIMainDetectorDataType::SetCurrentMenuItemID(UInt_t id)
{

  if(id == HistoMenuID) {
    dMenuPlot->UnCheckEntries();
    dMenuPlot->CheckEntry(HistoMenuID);
    CurrentPlotMenuItemID = HistoMenuID;
  }
  if(id == GraphMenuID)	{
    dMenuPlot->UnCheckEntries();
    dMenuPlot->CheckEntry(GraphMenuID);
    CurrentPlotMenuItemID = GraphMenuID;
  }
  if(id == FFTMenuID  )	{
    dMenuPlot->UnCheckEntries();
    dMenuPlot->CheckEntry(FFTMenuID);
    CurrentPlotMenuItemID = FFTMenuID;    
  }  
  if(id == ProfMenuID )	{
    dMenuPlot->UnCheckEntries();
    dMenuPlot->CheckEntry(ProfMenuID);
    CurrentPlotMenuItemID = ProfMenuID ;
  }

  for(uint i = 0; i < dLeafMenuID.size(); i++){
    if(id == dLeafMenuID[i]){
      dMenuBlock->UnCheckEntries();
      dMenuBlock->CheckEntry(id);
      CurrentLeafMenuItemID = id;
    }
  }
}


QwGUIMainDetector::QwGUIMainDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{
  dTabFrame = NULL;
  dSUMCanvas = NULL;
  dMDTab = NULL;
  dNumberEntryDlg = NULL;
  dMenuBar = NULL; 
  dMenuBlock = NULL;

  dCurrentYields = NULL;
  dCurrentPMTAsyms = NULL;
  dCurrentDETAsyms = NULL;
  dCurrentCMBAsyms = NULL;
  dCurrentMSCAsyms = NULL;
  dCurrentMSCYields = NULL;

  NewDataInit();

  RemoveSelectedDataWindow();

  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWSCRATCH"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  AddThisTab(this);
  dActiveTab = -1;
  SetCurrentMenuItemID(0);

  //PrintMemInfo("At construction");

}



void QwGUIMainDetector::NewDataInit()
{
  if(IsTrackingModeOpen())   UnMapTrackingModeTabs();
  if(IsCurrentModeOpen()) UnMapCurrentModeTabs();

  for(uint i = 0; i < dCurrentModeData.size(); i++){
    if(dCurrentModeData[i]) delete dCurrentModeData[i];
  }
  dCurrentModeData.clear();

  dYieldCanvas    = NULL;  
  dPMTAsymCanvas  = NULL;  
  dDETAsymCanvas  = NULL;  
  dCMBAsymCanvas  = NULL;  
  dMSCAsymCanvas  = NULL;  
  dMSCYieldCanvas = NULL;  

  SetCurrentModeOpen(kFalse);
  SetTrackingModeOpen(kFalse);
}

QwGUIMainDetector::~QwGUIMainDetector()
{
  CleanUp();

  if(dMDTab) delete dMDTab;
  if(dTabFrame)  delete dTabFrame;

  RemoveThisTab(this);
  IsClosing(GetName());

}

void QwGUIMainDetector::CleanUp()
{
  if(IsTrackingModeOpen()) UnMapTrackingModeTabs();
  if(IsCurrentModeOpen()) UnMapCurrentModeTabs();

  TObject *obj;
  TIter *next = new TIter(DataWindowArray.MakeIterator());

  obj = next->Next();
  while(obj){
    delete obj;
    obj = next->Next();
  }
  delete next;

  DataWindowArray.Clear();

  for(uint i = 0; i < dCurrentModeData.size(); i++){
    if(dCurrentModeData[i]){
      dCurrentModeData[i]->Clean();
      delete dCurrentModeData[i];
    }
  }
  dCurrentModeData.clear();

  dYieldCanvas    = NULL;  
  dPMTAsymCanvas  = NULL;  
  dDETAsymCanvas  = NULL;  
  dCMBAsymCanvas  = NULL;  
  dMSCAsymCanvas  = NULL;  
  dMSCYieldCanvas = NULL;  
  dSUMCanvas = NULL;  

  MainDetectorPMTNames.clear();
  MainDetectorMscNames.clear();
  MainDetectorCombinationNames.clear();
  MainDetectorNames.clear();
  MAIN_PMT_INDEX = 0;
  MAIN_DET_INDEX = 0;
  MAIN_DET_COMBIND = 0;
  MAIN_MSC_INDEX = 0;

  SetCurrentModeOpen(kFalse);
  SetTrackingModeOpen(kFalse);

  //PrintMemInfo("After all clean up");

//   dROOTCont->Close(kFalse);
  dROOTCont = NULL;
}

void QwGUIMainDetector::MapTrackingModeTabs()
{

}

void QwGUIMainDetector::MapCurrentModeTabs()
{
  if(!dMDTab) return;
  dMDTab->AddTab("PMT Yield");
  dMDTab->AddTab("PMT Asymmetry");
  dMDTab->AddTab("Det. Asymmetry");
  dMDTab->AddTab("Comb. Asymmetry");
  dMDTab->AddTab("Msc. Asymmetry");
  dMDTab->AddTab("Msc. Asymmetry");
  dMDTab->AddTab("Summary");
  dMDTab->MapSubwindows();
  dMDTab->Layout();
}

void QwGUIMainDetector::UnMapTrackingModeTabs()
{
  if(!dMDTab) return;

  int nt = dMDTab->GetNumberOfTabs();
  for (int i = 0 ; i < nt; i++) {
    dMDTab->RemoveTab(0); 
  }
  dMDTab->Layout();
}

void QwGUIMainDetector::UnMapCurrentModeTabs()
{
  if(!dMDTab) return;

  int nt = dMDTab->GetNumberOfTabs();
  for (int i = 0 ; i < nt; i++) {    
    dMDTab->RemoveTab(0); 
  }
  dMDTab->Layout();
}

Int_t QwGUIMainDetector::MakeCurrentModeTabs()
{

  dYieldCanvas = dCurrentYields->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());
  if(!dYieldCanvas) return 0;

  dPMTAsymCanvas = dCurrentPMTAsyms->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());
  if(!dPMTAsymCanvas) return 0;
  
  dDETAsymCanvas = dCurrentDETAsyms->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());
  if(!dDETAsymCanvas) return 0;

  dCMBAsymCanvas = dCurrentCMBAsyms->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());
  if(!dCMBAsymCanvas) return 0;

  dMSCYieldCanvas = dCurrentMSCYields->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());    
  if(!dMSCYieldCanvas) return 0;

  dMSCAsymCanvas = dCurrentMSCAsyms->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());
  if(!dMSCAsymCanvas) return 0;

  dSUMCanvas = dCurrentSummary->MakeDataTab(dMDTab,this,fClient->GetRoot(),GetWidth(), GetHeight());  
  if(!dSUMCanvas) return 0;

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
  
  dMDTab->MapSubwindows();
  dMDTab->Layout();
  dActiveTab = 0;
  dMDTab->SetTab(dActiveTab);


  for(uint j = 0; j < dCurrentModeData.size(); j++)
    PlotCurrentModeData(j);

  return 1;
}

Int_t QwGUIMainDetector::MakeTrackingModeTabs()
{
  if(IsCurrentModeOpen()) UnMapCurrentModeTabs();
  if(!IsTrackingModeOpen()){

    

  }
  else
    MapTrackingModeTabs();

  return 1;
}

void QwGUIMainDetector::MakeLayout()
{
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
  //This function should only be called if a new current mode root 
  //file is opened

  MainDetectorPMTNames.clear();
  MainDetectorMscNames.clear();
  MainDetectorCombinationNames.clear();
  MainDetectorNames.clear();
  MAIN_PMT_INDEX = 0;
  MAIN_DET_INDEX = 0;
  MAIN_DET_COMBIND = 0;
  MAIN_MSC_INDEX = 0;

  //  TString mapfile = Form("%s/setupfiles/qweak_maindet.map",gSystem->Getenv("QWSCRATCH"));
  TString mapfile = Form("qweak_maindet.map");
  TString varname, varvalue;
  TString modtype, dettype, namech, nameofcombinedchan;
  Int_t modnum, channum, combinedchans;

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
      }
      else if (modtype == "VPMT"){
	channum       = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
	combinedchans = (atol(mapstr.GetNextToken(", ").c_str()));	//number of combined channels
	dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
	namech.ToLower();
	if(combinedchans == 2){
	  MainDetectorNames.push_back(namech);
	  MAIN_DET_INDEX++;
	}
	if(combinedchans >= 4){
	  MainDetectorCombinationNames.push_back(namech);
	  MAIN_DET_COMBIND++;
	}

      }
      else
	return 0;
    }
  }

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

//   printf("Message = %s\n",message.Data());

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

  if(!strcmp(obj,"dROOTFile")){

    CleanUp();

  }

  QwGUISubSystem::OnObjClose(obj);
}

Int_t QwGUIMainDetector::SetupCurrentModeDataStructures(TTree *MPSTree,TTree *HELTree)
{
  vector <TString> temp1;
  for(uint i = 0; i < MainDetectorPMTNames.size(); i++)
    temp1.push_back(Form("asym_%s",MainDetectorPMTNames[i].Data()));
  vector <TString> temp2;
  for(uint j = 0; j < MainDetectorMscNames.size(); j++)
    temp2.push_back(Form("asym_%s",MainDetectorMscNames[j].Data()));
  vector <TString> temp3;
  for(uint k = 0; k < MainDetectorNames.size(); k++)
    temp3.push_back(Form("asym_%s",MainDetectorNames[k].Data()));
  vector <TString> temp4;
  for(uint l = 0; l < MainDetectorCombinationNames.size(); l++)
    temp4.push_back(Form("asym_%s",MainDetectorCombinationNames[l].Data()));

  if(ReadTreeEvents()){
    
    
    dCurrentYields = new QwGUIMainDetectorDataType(1000); if(!dCurrentYields) return 0;
    dCurrentYields->SetType("PMT Yield");
    dCurrentYields->SetTree(dROOTCont,MPSTree,MainDetectorPMTNames,GetRunNumber());
        
    dCurrentPMTAsyms = new QwGUIMainDetectorDataType(1100); if(!dCurrentPMTAsyms) return 0;
    dCurrentPMTAsyms->SetType("PMT Asym");
    dCurrentPMTAsyms->SetTree(dROOTCont,HELTree,temp1,GetRunNumber());
    
    dCurrentDETAsyms = new QwGUIMainDetectorDataType(1200); if(!dCurrentDETAsyms) return 0;
    dCurrentDETAsyms->SetType("Det. Asym");
    dCurrentDETAsyms->SetTree(dROOTCont,HELTree,temp3,GetRunNumber());
    
    dCurrentCMBAsyms = new QwGUIMainDetectorDataType(1300); if(!dCurrentCMBAsyms) return 0;
    dCurrentCMBAsyms->SetType("Comb. Asym");
    dCurrentCMBAsyms->SetTree(dROOTCont,HELTree,temp4,GetRunNumber());
    
    dCurrentMSCAsyms = new QwGUIMainDetectorDataType(1400); if(!dCurrentMSCAsyms) return 0;
    dCurrentMSCAsyms->SetType("Misc. Asym");
    dCurrentMSCAsyms->SetTree(dROOTCont,HELTree,temp2,GetRunNumber());

    dCurrentMSCYields = new QwGUIMainDetectorDataType(1500); if(!dCurrentMSCYields) return 0;
    dCurrentMSCYields->SetType("Misc. Yield");
    dCurrentMSCYields->SetTree(dROOTCont,MPSTree,MainDetectorMscNames,GetRunNumber());
    
    dCurrentSummary = new QwGUIMainDetectorDataType(1600,kTrue); if(!dCurrentSummary) return 0;
    dCurrentSummary->SetType("Summary");
    //Do not set a tree for this one!
    
  }
  else{

    //setup for histogram showing only!    
    
    dCurrentYields = new QwGUIMainDetectorDataType(1000); if(!dCurrentYields) return 0;
    dCurrentYields->SetType("PMT Yield");
    dCurrentYields->SetHistograms(dROOTCont,MPSTree,MainDetectorPMTNames,GetRunNumber());
        
    dCurrentPMTAsyms = new QwGUIMainDetectorDataType(1100); if(!dCurrentPMTAsyms) return 0;
    dCurrentPMTAsyms->SetType("PMT Asym");
    dCurrentPMTAsyms->SetHistograms(dROOTCont,HELTree,temp1,GetRunNumber());
    
    dCurrentDETAsyms = new QwGUIMainDetectorDataType(1200); if(!dCurrentDETAsyms) return 0;
    dCurrentDETAsyms->SetType("Det. Asym");
    dCurrentDETAsyms->SetHistograms(dROOTCont,HELTree,temp3,GetRunNumber());
    
    dCurrentCMBAsyms = new QwGUIMainDetectorDataType(1300); if(!dCurrentCMBAsyms) return 0;
    dCurrentCMBAsyms->SetType("Comb. Asym");
    dCurrentCMBAsyms->SetHistograms(dROOTCont,HELTree,temp4,GetRunNumber());
    
    dCurrentMSCAsyms = new QwGUIMainDetectorDataType(1400); if(!dCurrentMSCAsyms) return 0;
    dCurrentMSCAsyms->SetType("Misc. Asym");
    dCurrentMSCAsyms->SetHistograms(dROOTCont,HELTree,temp2,GetRunNumber());

    dCurrentMSCYields = new QwGUIMainDetectorDataType(1500); if(!dCurrentMSCYields) return 0;
    dCurrentMSCYields->SetType("Misc. Yield");
    dCurrentMSCYields->SetHistograms(dROOTCont,MPSTree,MainDetectorMscNames,GetRunNumber());
    
    dCurrentSummary = new QwGUIMainDetectorDataType(1600,kTrue); if(!dCurrentSummary) return 0;
    dCurrentSummary->SetType("Summary");
    //Do not set a tree for this one!
   
  }

  dCurrentModeData.push_back(dCurrentYields);
  dCurrentModeData.push_back(dCurrentPMTAsyms);
  dCurrentModeData.push_back(dCurrentDETAsyms);
  dCurrentModeData.push_back(dCurrentCMBAsyms);
  dCurrentModeData.push_back(dCurrentMSCYields);
  dCurrentModeData.push_back(dCurrentMSCAsyms);
  
  dCurrentYields->LinkData(dCurrentModeData);
  dCurrentPMTAsyms->LinkData(dCurrentModeData);
  dCurrentDETAsyms->LinkData(dCurrentModeData);
  dCurrentCMBAsyms->LinkData(dCurrentModeData);
  dCurrentMSCYields->LinkData(dCurrentModeData);
  dCurrentMSCAsyms->LinkData(dCurrentModeData);
  dCurrentSummary->LinkData(dCurrentModeData);
  
  dCurrentModeData.push_back(dCurrentSummary);

  
  return 1;
}

Int_t QwGUIMainDetector::GetCurrentModeData(TTree *MPSTree, TTree *HELTree)
{

  Int_t evn = 0;
  Int_t Start = 0;
  Int_t Stop  = 0;

  Double_t tmp1;
  Double_t tmp2;

  Int_t HelStart = -1;
  Int_t HelStop = -1;

  if(ReadTreeEvents()){

    Start = (Int_t)GetStartEvent();
    Stop  = Start+(Int_t)GetNumEvents()-1;
    
    if(!MPSTree && !HELTree) return 0;

    Int_t N = MPSTree->GetEntries();
    Int_t M = HELTree->GetEntries();
    if(Start > MPSTree->GetEntries()) {Start = 0;}
    if(Stop > MPSTree->GetEntries()) {Stop = M+N;}
    if(Stop <= Start ) {Start = 0; Stop = M+N;}
    
    TLeaf *aLeaf = HELTree->GetLeaf("mps_counter");
    TLeaf *mLeaf = MPSTree->GetLeaf("mps_counter");

    aLeaf->SetAddress(&tmp1);
    mLeaf->SetAddress(&tmp2);

    InitProgressDlg("Finding Asymmetry Range","Quartet Asyms",0,0,0,M,0,0,1);
    for(int l = 0; l < M; l++){

      HELTree->GetEvent(l);

      if(tmp1 >= Start && HelStart < 0) {HelStart = l;}

      if(tmp1 >= Stop && HelStop < 0) {HelStop = l-1;}
      if(HelStart >= 0 && HelStop >= 0) break;

      evn++;
      if(dProcessHalt) { return 0;}
      IncreaseProgress(&evn,0,0,1000,0,0);
    }
    if(dProgrDlg)
      dProgrDlg->CloseWindow();

    if(HelStop < 0 ) HelStop = M-1;

//     Int_t NItems = GetNumEvents()+HelStop-HelStart+1; 

    InitProgressDlg("Detector Data Fill","MPS Events","Quartet Asyms",0,0,GetNumEvents(),HelStop-HelStart+1,0,2);

    aLeaf->ResetAddress(&tmp1);
    mLeaf->ResetAddress(&tmp2);

    evn = 0;
    for(int i = Start; i <= Stop; i++){
    
      MPSTree->GetEvent(i);
      dCurrentYields->FillData(tmp2);
      dCurrentMSCYields->FillData(tmp2);    
      
      evn++;
      if(dProcessHalt) { return 0;}
      IncreaseProgress(&evn,0,0,1000,0,0);
      
    }
    evn = 0;
    for(int i = HelStart; i <= HelStop; i++){

      HELTree->GetEvent(i);
      dCurrentPMTAsyms->FillData(tmp1);
      dCurrentDETAsyms->FillData(tmp1);
      dCurrentMSCAsyms->FillData(tmp1);
      dCurrentCMBAsyms->FillData(tmp1);      

      evn++;
      if(dProcessHalt) {return 0;}
      IncreaseProgress(0,&evn,0,0,1000,0);

    }

    if(dProgrDlg)
      dProgrDlg->CloseWindow();
  }
  
  //Load histograms only!
  dCurrentYields->ProcessData("(md1- -> md8+)");
  printf("dCurrentYields->ProcessData Done\n");
  
  dCurrentPMTAsyms->ProcessData("(md1- -> md8+)");
  printf("dCurrentPMTAsyms->ProcessData Done\n");
  
  dCurrentDETAsyms->ProcessData("(md1barsum -> md8barsum)");
  printf("dCurrentDETAsyms->ProcessData Done\n");
  
  dCurrentCMBAsyms->ProcessData("(1+5,2+6,3+7,4+8,odd,even,all)");
  printf("dCurrentCMBAsyms->ProcessData Done\n");
  
  dCurrentMSCYields->ProcessData("(PMT+LED,PMT,PMT+LG,md9-,md9+,I,V,Cage)");    
  printf("dCurrentMSCYields->ProcessData Done\n");
  
  dCurrentMSCAsyms->ProcessData("(PMT+LED,PMT,PMT+LG,md9-,md9+,I,V,Cage)");
  printf("dCurrentMSCAsyms->ProcessData Done\n");
  
  dCurrentSummary->ProcessData(""); //Must be called last!
  printf("dCurrentSummary->ProcessData Done\n");  

  //End filling root data

  return 1;
}


Int_t QwGUIMainDetector::SetupTrackingModeDataStructures()
{
//   dCurrentYields = new QwGUIMainDetectorDataType(MAIN_PMT_INDEX,1000,GetRunNumber());
//   dCurrentYields->SetName("PMTYield");
//   dCurrentPMTAsyms = new QwGUIMainDetectorData(MAIN_PMT_INDEX,1100,GetRunNumber());
//   dCurrentPMTAsyms->SetName("PMTAsym");
//   dCurrentDETAsyms = new QwGUIMainDetectorData(MAIN_DET_INDEX,1200,GetRunNumber());
//   dCurrentDETAsyms->SetName("DetAsym");
//   dCurrentCMBAsyms = new QwGUIMainDetectorData(MAIN_DET_COMBIND,1300,GetRunNumber());
//   dCurrentCMBAsyms->SetName("CMBAsym");
//   dCurrentMSCAsyms = new QwGUIMainDetectorData(MAIN_MSC_INDEX,1400,GetRunNumber());
//   dCurrentMSCAsyms->SetName("MSCAsym");
//   dCurrentMSCYields = new QwGUIMainDetectorData(MAIN_MSC_INDEX,1500,GetRunNumber());
//   dCurrentMSCYields->SetName("MSCYield");

//   NewDataInit();
//   ClearRootData();
//   ClearDFTData();
//   ClearDBData();

  return 1;
}

Int_t QwGUIMainDetector::GetTrackingModeData(TTree *tree)
{
  if(!tree) return 0;


  //etc ...

  return 1;
}

void QwGUIMainDetector::OnNewDataContainer(RDataContainer *cont)
{
  
  if(!cont) return;

  Connect(cont,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");

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
	MPSTree = (TTree*)obj;//->Clone();
      }
    }
    obj = dROOTCont->ReadData("Hel_Tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	HELTree = (TTree*)obj;//;->Clone();
      }
    }

    //Tracking Mode
    obj = dROOTCont->ReadData("tree");
    if(obj){
      if(obj->InheritsFrom("TTree")){
	HELTree = (TTree*)obj;//->Clone();
      }
    }

    if(HELTree || MPSTree){

      //PrintMemInfo("Before Processing");

      NewDataInit();
      if(LoadCurrentModeChannelMap()){
	if(SetupCurrentModeDataStructures(MPSTree,HELTree)){
	  if(GetCurrentModeData(MPSTree,HELTree)){
	    if(MakeCurrentModeTabs()){
	      SetCurrentModeOpen(kTrue);
	      SetTrackingModeOpen(kFalse);
	    }
	  }
	}
      }
      //PrintMemInfo("After all processing");

    }
    else if(tree){
      NewDataInit();
      LoadTrackingModeChannelMap();
      SetupTrackingModeDataStructures();
      GetTrackingModeData(tree);
      MakeTrackingModeTabs();
      SetTrackingModeOpen(kTrue);
      SetCurrentModeOpen(kFalse);
    }

  }
  else if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){
    //Start filling DB data
//     ClearDBData();
  }
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

Bool_t QwGUIMainDetector::PlotCurrentModeData(UInt_t tab)
{
  if(tab < 0 || tab >= dCurrentModeData.size()) return kFalse;

  TString type;
  QwGUIMainDetectorDataType *dataType = dCurrentModeData[tab];
  TCanvas *mc = NULL;
  Double_t rad = 1.0/sqrt(2.0)/2.0;

  if(dataType){


    Float_t TitleW = gStyle->GetTitleW();
    Float_t TitleH = gStyle->GetTitleH();
    gStyle->SetTitleW(0.6);
    gStyle->SetTitleH(0.12);             
    Float_t StatW = gStyle->GetStatW();
    Float_t StatH = gStyle->GetStatH();
    gStyle->SetStatW(0.3);
    gStyle->SetStatH(0.3);             
    
     
    if(!dataType->GetCanvas()) return kFalse;
    mc =  dataType->GetCanvas()->GetCanvas();
    type = dataType->GetType();
    
    for(uint k = 0; k < dataType->GetNumberOfPads(); k++){

      mc->cd(k+1);

      if(type.Contains("Det. Asym")){
	gPad->SetPad(0.5+rad*TMath::Sin(2*(k)*TMath::Pi()/8 - TMath::Pi()/2)-0.125,
		     0.5+rad*TMath::Cos(2*(k)*TMath::Pi()/8 - TMath::Pi()/2)-0.125,
		     0.5+rad*TMath::Sin(2*(k)*TMath::Pi()/8 - TMath::Pi()/2)+0.125,
		     0.5+rad*TMath::Cos(2*(k)*TMath::Pi()/8 - TMath::Pi()/2)+0.125);

      }
      gPad->SetLeftMargin(0.2);
      gPad->SetBottomMargin(0.2);
    }

    if(type.Contains("Det. Asym")){
      TPaveText *ref = new TPaveText(0.43,0.48,0.57,0.52);
      ref->AddText("Looking Downstream");
      ref->SetBorderSize(0);
      ref->SetFillColor(0);
      mc->cd();
      ref->Draw();
    }
    mc->Modified();
    mc->Update();

    dataType->PlotData();

    gStyle->SetTitleW(TitleW);
    gStyle->SetTitleH(TitleH);
    gStyle->SetStatW(StatW);             
    gStyle->SetStatH(StatH);             
    return kTrue;

  }

  return kFalse;
}

void QwGUIMainDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  QwGUIDataWindow *dDataWindow = GetSelectedDataWindow();
  Bool_t add = kFalse;
  TObject *plot = NULL;
  QwGUIMainDetectorDataStructure *detStr = NULL;
  Int_t leafInd;
  
  if(event == kButton1Double){

    if(!dCurrentModeData[GetActiveTab()]) return;
    
    plot = dCurrentModeData[GetActiveTab()]->GetSelectedPlot();
    if(!plot) return;
    detStr = dCurrentModeData[GetActiveTab()]->GetSelectedDetector();
    if(!dCurrentModeData[GetActiveTab()]->IsSummary() && !detStr) return;
    leafInd = dCurrentModeData[GetActiveTab()]->GetCurrentLeafIndex();
    
    if(plot->InheritsFrom("TH1")){

      if(!dDataWindow){
	dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					  "QwGUIMainDetector",((TH1D*)plot)->GetTitle(), PT_HISTO_1D,
					  DDT_MD,600,400);

	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
      }
      else
	add = kTrue;
      
      
      dDataWindow->SetStaticData(plot,DataWindowArray.GetLast());
      dDataWindow->SetPlotTitle((char*)((TH1D*)plot)->GetTitle());
      dDataWindow->DrawData(*((TH1D*)plot),add);
      
      SetLogMessage(Form("Looking at histogram %s\n",(char*)((TH1D*)plot)->GetTitle()),kTrue);
      Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
      Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
      Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");
      
      return;
    }
    if(plot->InheritsFrom("TGraphErrors")){

      if(!dDataWindow){
	dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					  "QwGUIMainDetector",((TGraphErrors*)plot)->GetTitle(), PT_GRAPH_ER,
					  DDT_MD,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
      }
      else
	add = kTrue;
            
      DataWindowArray.Add(dDataWindow);
      dDataWindow->SetPlotTitle((char*)((TGraphErrors*)plot)->GetTitle());
      if(!dCurrentModeData[GetActiveTab()]->IsSummary()){
	dDataWindow->SetPlotTitleX("Time [sec]");
	dDataWindow->SetPlotTitleY("Amplitude [V/#muA]");
	dDataWindow->SetPlotTitleOffset(1.25,1.8);
	dDataWindow->SetAxisMin(((TGraphErrors*)plot)->GetXaxis()->GetXmin(),
				detStr->GetTreeLeafMin(leafInd));
	dDataWindow->SetAxisMax(((TGraphErrors*)plot)->GetXaxis()->GetXmax(),
				detStr->GetTreeLeafMax(leafInd));
	dDataWindow->SetLimitsFlag(kTrue);
	dDataWindow->DrawData(*((TGraphErrors*)plot),add);
      }
      else{
	dDataWindow->DrawData(*((TGraphErrors*)plot),add);
	
      }
      SetLogMessage(Form("Looking at graph %s\n",(char*)((TGraphErrors*)plot)->GetTitle()),kTrue);

      Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
      Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
      Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");

      return;
    }
    if(plot->InheritsFrom("TGraph")){

      if(!dDataWindow){
	dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					  "QwGUIMainDetector",((TGraph*)plot)->GetTitle(), PT_GRAPH,
					  DDT_MD,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
      }
      else
	add = kTrue;
      
      DataWindowArray.Add(dDataWindow);
      dDataWindow->SetPlotTitle((char*)((TGraph*)plot)->GetTitle());
      if(!dCurrentModeData[GetActiveTab()]->IsSummary()){
// 	dDataWindow->SetPlotTitleX("Time [sec]");
// 	dDataWindow->SetPlotTitleY("Amplitude [V/#muA]");
	dDataWindow->SetPlotTitleOffset(1.25,1.8);
	dDataWindow->SetAxisMin(((TGraph*)plot)->GetXaxis()->GetXmin(),
				detStr->GetTreeLeafMin(leafInd));
	dDataWindow->SetAxisMax(((TGraph*)plot)->GetXaxis()->GetXmax(),
				detStr->GetTreeLeafMax(leafInd));
	dDataWindow->SetLimitsFlag(kTrue);
	dDataWindow->DrawData(*((TGraph*)plot),add);
      }
      else{
	dDataWindow->DrawData(*((TGraph*)plot),add);
	
      }
      SetLogMessage(Form("Looking at graph %s\n",(char*)((TGraphErrors*)plot)->GetTitle()),kTrue);

      Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
      Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
      Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");

    }
    if(plot->InheritsFrom("TProfile")){

      if(!dDataWindow){
	dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					  "QwGUIMainDetector",((TProfile*)plot)->GetTitle(), PT_PROFILE,
					  DDT_MD,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
      }
      else
	add = kTrue;

      DataWindowArray.Add(dDataWindow);
      dDataWindow->SetPlotTitle((char*)((TProfile*)plot)->GetTitle());
      dDataWindow->DrawData(*((TProfile*)plot));
      SetLogMessage(Form("Looking at DFT profile %s\n",(char*)((TProfile*)plot)->GetTitle()),add);

      Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
      Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
      Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");

    }
  }
}

Bool_t QwGUIMainDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//   MDMenuIdentifiers dtype;

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
      {
	dActiveTab = GetActiveTab();
// 	printf("Selecting tab: %d - %s \n",(dMDTab->GetTabTab(i)->GetText())->Data());

// 	TString TabName;
// 	for(uint i = 0; i < dCurrentModeData.size(); i++){
// 	  if(dCurrentModeData[i]){
// 	    TabName = dCurrentModeData[i]->GetType();
// 	    if(!dMDTab->GetCurrentTab()->GetText()->CompareTo(TabName)){
// 	      dActiveTab = i;
// 	    }
// 	  }
// 	}
      }
      break;
      
    case kCM_MENU:
      {
	if(dCurrentModeData[GetActiveTab()]){
	  
	  dCurrentModeData[GetActiveTab()]->SetCurrentMenuItemID(parm1);
	  PlotCurrentModeData(GetActiveTab());
	}
      }
      break;
      
    default:
      break;
    }

  default:
    break;
  }

  return kTRUE;
}


