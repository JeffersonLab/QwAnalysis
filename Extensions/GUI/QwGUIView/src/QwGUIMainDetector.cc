#include <QwGUIMainDetector.h>

ClassImp(QwGUIMainDetector);

using namespace QwParityDB;

const char *QwGUIMainDetector::MainDetectorDataNames[MAIN_DET_INDEX] = 
  {"md1neg","md2neg","md3neg","md4neg","md5neg","md6neg","md7neg","md8neg",
   "md1pos","md2pos","md3pos","md4pos","md5pos","md6pos","md7pos","md8pos"};

QwGUIMainDetector::QwGUIMainDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
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
  ClearDFTData();
  ClearDBData();
  fftopts.changeFlag  = kFalse;
  fftopts.cancelFlag  = kFalse;
  fftopts.Start       = 0;
  fftopts.Length      = 0;
  fftopts.TotalLength = 0;
  SetDFTCalculated(kFalse);
  
}

QwGUIMainDetector::~QwGUIMainDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
  ClearRootData();
  ClearDFTData();
  ClearDBData();
  fftopts.changeFlag  = kFalse;
  fftopts.cancelFlag  = kFalse;
  fftopts.Start       = 0;
  fftopts.Length      = 0;
  fftopts.TotalLength = 0;
  SetDFTCalculated(kFalse);
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

  dMenuData = new TGPopupMenu(fClient->GetRoot());
  dMenuData->AddEntry("&PMT Yields", M_DATA_PMT_YIELD);
  dMenuData->AddEntry("&Detector Yields", M_DATA_DET_YIELD);
  dMenuData->AddEntry("&Total Yield", M_DATA_ALL_YIELD);
//   dMenuData->AddPopup("&Yield Combinations", dMenuYieldComb, dMenuBarItemLayout);
//   dMenuYieldComb->AddEntry("")
  dMenuData->AddSeparator();
  dMenuData->AddEntry("&PMT Asymmetry", M_DATA_PMT_ASYM);
  dMenuData->AddEntry("&Detector Asymmetry", M_DATA_DET_ASYM);
  dMenuData->AddEntry("&Total Asymmetry", M_DATA_ALL_ASYM);

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Plot Type", dMenuPlot, dMenuBarItemLayout);
  dMenuBar->AddPopup("&Data Type", dMenuData, dMenuBarItemLayout);

  dTabFrame->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuData->Associate(this);
  dMenuPlot->Associate(this);

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);     
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);

  dMenuPlot->CheckEntry(M_PLOT_HISTO);
  dMenuData->CheckEntry(M_DATA_PMT_YIELD);


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
  TTree *tree;
  TH1D *hst;
  TGraph *grp;
  //  TProfile *prf;
  
  Double_t min = 5e9;
  Double_t max = 0;
  Double_t sum = 0;
  Double_t sumsq = 0;
  Double_t mean = 0;
  Double_t rms = 0;
  Int_t events = 0;
  

  if(!strcmp(cont->GetDataName(),"ROOT") && dROOTCont){

    obj = dROOTCont->ReadData("MPS_Tree");
    if(obj){
       if(obj->InheritsFrom("TTree")){
	 tree = (TTree*)obj->Clone();

	 ClearRootData();  
	 ClearDFTData();
	 fftopts.changeFlag  = kFalse;
	 fftopts.cancelFlag  = kFalse;
	 fftopts.Start       = 0;
	 fftopts.Length      = 0;
	 fftopts.TotalLength = 0;
	 SetDFTCalculated(kFalse);
	
	 for(int i = 0; i < MAIN_DET_INDEX; i++){

// 	  if ( tree -> FindLeaf(Form("%s.block2_raw",MainDetectorDataNames[i])) ) 
	  if ( tree -> FindLeaf(Form("%s.hw_sum_raw",MainDetectorDataNames[i])) ) 
 	  {
	    min = 5e9;
	    max = 0;
	    sum = 0;  
	    sumsq = 0;
	    events = 0;

	    tree->Draw(Form("%s.hw_sum_raw",MainDetectorDataNames[i]),"","goff",tree->GetEntries(),0);
	    
	    for(int j = 0; j < tree->GetEntries(); j++){
	      dCurrentData[i].push_back(tree->GetV1()[j]*76.3e-6/480);// + 4.0*TMath::Sin(2*TMath::Pi()*(j+1)*1.1e-3*20*(i+1)));

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

	    hst = new TH1D(Form("hst%02d",i),Form("%s.hw_sum_raw",MainDetectorDataNames[i]),1000,-4*rms,+4*rms);
	    grp = new TGraph();
	    grp->SetTitle(Form("%s.hw_sum_raw",MainDetectorDataNames[i]));
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
    }

    fftopts.TotalLength = tree->GetEntries();
    PlotHistograms();
  }

  if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){

    ClearDBData();      

    dDatabaseCont->Connect();

    mysqlpp::Query query = dDatabaseCont->Query();


    query << "SELECT * FROM summary_da_calc where subblock = 0 and detector='combinationallmd' ORDER BY run_number, segment_number";
    vector<summary_dy_calc> res;
    query.storein(res);

    printf("Number of rows returned:  %ld\n",res.size());

    Int_t res_size = res.size();

    // Loop over all rows
    TVectorF x(res_size), xerr(res_size), y(res_size), yerr(res_size);
    Float_t run_number;
    Int_t segment_number;
    Int_t i = 0;
    vector<summary_dy_calc>::iterator it;
    for (it = res.begin(); it != res.end(); ++it) {
      run_number = it->run_number;
      if (!it->segment_number.is_null) 
        segment_number = it->segment_number.data;
      x[i] = run_number + segment_number/100;
      xerr[i] = 0;
      if (!it->value.is_null)
        y[i] = it->value.data;
      if (!it->error.is_null)
        yerr[i] = it->error.data;

      printf("i = %d, x = %f, xerr = %f, y = %f, yerr = %f \n", i, x[i], xerr[i], y[i], yerr[i]);
      i++;
    }

    dDatabaseCont->Disconnect(); 

    grp = new TGraphErrors(x, y, xerr, yerr);

	   grp->GetXaxis()->SetTitle("Run Number");
	    grp->GetXaxis()->CenterTitle();
	    grp->GetXaxis()->SetTitleSize(0.04);
	    grp->GetXaxis()->SetLabelSize(0.04);
	    grp->GetXaxis()->SetTitleOffset(1.25);
	    grp->GetYaxis()->SetTitle("Asymmetry [ppb]");
	    grp->GetYaxis()->CenterTitle();
	    grp->GetYaxis()->SetTitleSize(0.04);
	    grp->GetYaxis()->SetLabelSize(0.04);
	    grp->GetYaxis()->SetTitleOffset(1.5);


    GraphArray.Add(grp);

    PlotGraphs();

//     if (res.size()!=1) {
//       QwError << "Unable to find unique run number " << 1000 << " in database." << QwLog::endl;
//       QwError << "Run number query returned " << res.size() << "rows." << QwLog::endl;
//       QwError << "Please make sure that the database contains one unique entry for this run." << QwLog::endl;
//     }

//     printf("Run ID = %d\n",res.at(0).run_id);


//      vector<QwParityDB::md_data> entrylist;

//     query.storein(entrylist);

// //     QwDBInterface interface;
// //     vector<QwParityDB::md_data> entrylist;
// //     QwParityDB::md_data row = interface.MainDetectorDBClone();
//     Double_t value = 0;

// //     if (mysqlpp::StoreQueryResult res = query.store()) {

// //       grp = new TGraph();
// //       grp->SetTitle(Form("%s.hw_sum_raw",MainDetectorDataNames[i]));
// //       grp->SetName(Form("grp%02d",i));
      

// //       for (size_t i = 0; i < res.num_rows(); ++i) {	
// // 	value = res[i]["value"];
// // 	printf("%f\n",value);
// //       }
// //     }
    
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
  
  printf("Received Messager From: %s\n",obj);
}

void QwGUIMainDetector::ClearDBData()
{
 
}

void QwGUIMainDetector::ClearRootData()
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

  for(int i = 0; i < MAIN_DET_INDEX; i++){
    dCurrentData[i].clear();
    dCurrentData[i].resize(0);
  }  
}

void QwGUIMainDetector::ClearDFTData()
{
  TObject *obj;
  TIter nextp(ProfileArray.MakeIterator());
  obj = nextp();
  while(obj){    
    delete obj;
    obj = nextp();
  }

  TIter nextd(DFTArray.MakeIterator());
  obj = nextd();
  while(obj){    
    delete obj;
    obj = nextd();
  }
  ProfileArray.Clear();
  DFTArray.Clear();

}

void QwGUIMainDetector::PlotHistograms()
{
  Int_t ind = 1;
  Double_t rad = 1.0/sqrt(2.0)/2.0;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(HistArray.MakeIterator());
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
    gPad->SetLogy(0);
    ((TGraph*)obj)->Draw("ap");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

  SetPlotDataType(PLOT_TYPE_GRAPH);
}

void QwGUIMainDetector::PlotDFTProfile()
{
  if(!IsDFTCalculated()) return;

  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(ProfileArray.MakeIterator());
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

  SetPlotDataType(PLOT_TYPE_PROFILE);
}

void QwGUIMainDetector::PlotDFT()
{
//   if(!IsDFTCalculated())
  CalculateDFT();
  if(!IsDFTCalculated()) return;

  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(DFTArray.MakeIterator());
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

  SetPlotDataType(PLOT_TYPE_DFT);
  
}

void QwGUIMainDetector::CalculateDFT()
{
  if(dCurrentData[0].size() == 0) return;

  TProfile *prf = NULL;
  TH1D *hst = NULL;
  TH1D *fft = NULL;
  Int_t gc = 0;
  Int_t mc = 0;

  new QwGUIFFTWindowSelectionDialog(fClient->GetRoot(), GetMain(), "sdlg","QwGUIMainDetector",&fftopts);

  if(fftopts.cancelFlag) return;
  if(fftopts.changeFlag || !IsDFTCalculated()){

    if(fftopts.Length <= 0) return;
    if(fftopts.Start < 0 || fftopts.Start >= dCurrentData[0].size()) return;
    if(fftopts.Start + fftopts.Length > dCurrentData[0].size()) return;

//     InitProgressDlg("FFT Calculation","Detectors","Data",0,MAIN_DET_INDEX,fftopts.Length,0,2);
    
    ClearDFTData();
    for(int det = 0; det < MAIN_DET_INDEX; det++){      
      
      prf = new TProfile(Form("prf%02d",det),Form("%s.hw_sum_raw",MainDetectorDataNames[det]),
			 fftopts.Length,1.0*fftopts.Start/SAMPLING_RATE,1.0*(fftopts.Start+fftopts.Length)/SAMPLING_RATE);
      
//       dProgrDlg->ResetRange(0,fftopts.Length,0);
      mc = 0;
      for(int n = fftopts.Start; n < fftopts.Length+fftopts.Start; n++){ 
	if(dCurrentData[det].at(n) > 0)
	  prf->Fill(1.0*n/SAMPLING_RATE,dCurrentData[det].at(n)-dCurrentDataMean[det]);
// 	mc++;
// 	if(dProcessHalt) {SetDFTCalculated(kFalse); return;};
// 	IncreaseProgress(0,&mc,0,0,1000,0);	
      }      

      hst = prf->ProjectionX(Form("%s-prj",MainDetectorDataNames[det]));

      TH1 *fftmag = NULL;
      TVirtualFFT::SetTransform(0);
      fftmag = ((TH1*)hst)->FFT(fftmag,"MAG");
      fftmag->SetName(Form("%s-fft-mag",MainDetectorDataNames[det]));
      fftmag->SetTitle("Magnitude of the transform");
      
      fft = new TH1D(Form("%s_fft",MainDetectorDataNames[det]),
		     Form("%s FFT",MainDetectorDataNames[det]),
		     (Int_t)(fftopts.Length/2.0)-1,0.0,(Int_t)(SAMPLING_RATE/2.0));

      for(int i = 0; i < (Int_t)(fftopts.Length/2); i++) fft->SetBinContent(i,fftmag->GetBinContent(i)/fftopts.Length);

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

      DFTArray.Add(fft);
      ProfileArray.Add(prf);
      delete hst;
      delete fftmag;
      prf->SetDirectory(0);

//       gc++;
//       if(dProcessHalt) {SetDFTCalculated(kFalse); return;};
//       IncreaseProgress(&gc,0,0,1,0,0);	
    }
//     if(dProgrDlg)
//       dProgrDlg->CloseWindow();
    
  }
  SetDFTCalculated(kTrue);
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
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= MAIN_DET_INDEX){
      if(GetPlotDataType() == PLOT_TYPE_HISTO){

	QwGUIDataWindow *dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
							   "QwGUIMainDetector",HistArray[pad-1]->GetTitle(), PT_HISTO_1D,
							   DDT_MD,600,400);
// 	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
// 						     GetNewWindowName(),"QwGUIMainDetector",
// 						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
	dDataWindow->SetStaticData(HistArray[pad-1],DataWindowArray.GetLast());

	Connect(dDataWindow,"IsClosing(char*)","QwGUIMainDetector",(void*)this,"OnObjClose(char*)");
	Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIMainDetector",(void*)this,"OnReceiveMessage(char*)");
	Connect(dDataWindow,"UpdatePlot(char*)","QwGUIMainDetector",(void*)this,"OnUpdatePlot(char *)");      

	dDataWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
	dDataWindow->DrawData(*((TH1D*)HistArray[pad-1]));
	SetLogMessage(Form("Looking at histogram %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);
	
	return;
      }
      else if(GetPlotDataType() == PLOT_TYPE_GRAPH){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
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
      else if(GetPlotDataType() == PLOT_TYPE_PROFILE){

	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIMainDetector",
						     ProfileArray[pad-1]->GetTitle(), PT_PROFILE,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)ProfileArray[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TProfile*)ProfileArray[pad-1]));
	SetLogMessage(Form("Looking at DFT profile %s\n",(char*)ProfileArray[pad-1]->GetTitle()),kTrue);
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
	PlotHistograms();
	break;

      case M_PLOT_GRAPH:
	PlotGraphs();
	break;

      case M_PLOT_DFT:
	PlotDFT();
	break;	

      case M_PLOT_DFTPROF:
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
