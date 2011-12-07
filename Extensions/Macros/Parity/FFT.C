//************************************************************************************************************************************************//
//                                                                                                                                                //
//    This macros performs the Fast Fourier Transform of a signal and gives out,                                                                  //
//           1. The power spectrum                                                                                                                //
//           2. The phase of the transform                                                                                                        // 
//           2. The backward transform.                                                                                                           // 
//    The reuslts are saved in a xxxx.gif file.                                                                                                   // 
//                                                                                                                                                //
//    Author   : B. Waidyawansa                                                                                                                   //
//    contact  : buddhini@jlab.org                                                                                                                //
//    Reference: http://root.cern.ch/root/html/tutorials/fft/FFT.C.html                                                                           //
//                                                                                                                                                //
//************************************************************************************************************************************************//
//                                                                                                                                                //
//                                                                                                                                                //
//     run_number        : Number of the run containing the signal                                                                                //
//     device            : The device signal which is to be FFT'd. eg, "bcm0l02".                                                             //
//     min               : The lower limit of the event cut.                                                                                      //
//     max               : The upper limit of the event cut                                                                                       //
//                                                                                                                                                //
//************************************************************************************************************************************************// 
// Hw to use: e.g.
//root [6] FFT(6206,"qwk_bpm3h09bX",5000,30000)
// --- Signal = qwk_bpm3h09bX
// --- Length of the signal = 24s
// --- Sampling Rate = 1041.67
// --- Sample Size = 25000
// --- Average signal =-4.68837
// DC component: -336356.167598 Hz
// Nyquist harmonic: 3.649824 Hz
//Info in <TCanvas::Print>: GIF file 6206_FFT_of_qwk_bpm3h09bX.gif has been created
//
void 
FFT(Int_t run_number, TString device, Int_t min, Int_t max)
{
  gDirectory->Delete("*") ;
  gStyle->SetOptStat(kFALSE);

  TCanvas *canvas = NULL;

  canvas = new TCanvas("canvas","Fast Fourier Transform",10,10,1000,680);
  
  //******* ADC settings
  const Double_t time_per_sample = 2e-06;//s
  const Double_t t_settle = 70e-6;//s
  
  //******* Signal Settings
  // get num samples from bcm1. 
  TString time    = Form("mps_counter*(((%s.num_samples)*%f)+%f)",device.Data(),time_per_sample,t_settle);// units seconds.
  TString amplitude; 



  //******** get tree
  TChain *tree = new TChain("Mps_Tree");
  TChain *slow_tree = new TChain("Slow_Tree");

  TString filename = Form("QwPass1_%i.000.trees.root", run_number);
  Bool_t found = kFALSE;

  found = FindFiles(filename, tree);
  if(!found){
    filename = Form("first100k_%i.root", run_number);
    found = FindFiles(filename, tree);
    if(!found){
      std::cerr<<"Unable to find rootfile(s) for run "<<run_number<<std::endl;
      exit(1);
    }
  }

  found = FindFiles(filename, slow_tree);
  if(!found){
    filename = Form("first100k_%i.root", run_number);
    found = FindFiles(filename, tree);
    if(!found){
      std::cerr<<"Unable to find rootfile(s) for run "<<run_number<<std::endl;
      exit(1);
    }
  }
  

  // if (file->IsZombie()) {
  //   std::cout << "Error opening root file chain " << filename << std::endl;
  //   filename = Form("QwPass1_%i.root", run_number);
  //   std::cout << "Try to open chain " << filename << std::endl;
  //   file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
  //   if (file->IsZombie()) {
  //     std::cout << "Error opening root file chain " << filename << std::endl;
  //     filename = Form("Qweak_%i.000.root", run_number);
  //     std::cout << "Try to open chain " << filename << std::endl;
  //     file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
  //     if(file->IsZombie()){
  // 	std::cout << "Error opening root file chain " << filename << std::endl;
  // 	filename = Form("first100k_%i.root", run_number);
  // 	std::cout << "Try to open chain " << filename << std::endl;
  // 	file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
  // 	if (file->IsZombie()) {
  // 	  tree = NULL;
  // 	  exit(-1);
  // 	}
  // 	else
  // 	  tree->Add(Form("$QW_ROOTFILES/first100k_%i.root", run_number));	
  //     }
  //     else
  // 	tree->Add(Form("$QW_ROOTFILES/Qweak_%i.*.root", run_number));	
  //   }
  //   else 
  //     tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.root", run_number));
  // }
  // else 
  //     tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root", run_number));
  
  tree->SetAlias("time",time);
  
  
  //************ Signal properties
  Int_t samples = (max-min);
  
  // get sampling rate/event rate
  TH1*h=NULL;
  TString command = "HELFREQ>>htemp";
  slow_tree->Draw(command);
  h = (TH1*)gDirectory->Get("htemp");
  if(h==NULL){
    std::cout<<"Unable to get num_samples!"<<std::endl;
    exit(1);
  }
  Double_t sampling_rate = h->GetMean();
  std::cout<<sampling_rate<<std::endl;
  if((sampling_rate>961) || (sampling_rate< 959)) {
    std::cout<<"Sampling rate "<<sampling_rate<<"Hz is not realistic. The sampling rate of Qweak ADCs should be ~ 960HZ!"<<std::endl;
    exit(1);
  }
  Double_t length = samples*1.0/sampling_rate; 
  
  std::cout<<" --- Signal = "<<device<<"\n";
  std::cout<<" --- Length of the signal = "<<length<<"s\n";
  std::cout<<" --- Sampling Rate = "<<sampling_rate<<"\n";
  std::cout<<" --- Sample Size = "<<(max-min)<<"\n";
  
  delete h;
  canvas->Draw();
  canvas -> Divide(2,2);

  //************  Plot the signal.
  canvas -> cd(1);
  Double_t up = (1.0*max)/sampling_rate;
  Double_t low = (1.0*min)/sampling_rate;
  
  
  TProfile *profile1 = new TProfile("profile1","Signal Profile1",samples,low,up);
  TProfile *profile2 = new TProfile("profile2","Signal Profile2",samples,low,up);
  TProfile *profile3 = new TProfile("profile3","Signal Profile3",samples,low,up);
  TProfile *profile4 = new TProfile("profile4","Signal Profile4",samples,low,up);
  
  TH1D *htemp0 = new TH1D("htemp0","htemp0",samples,low,up);
  TH1D *htemp1 = new TH1D("htemp1","htemp1",samples,low,up);
  TH1D *htemp2 = new TH1D("htemp2","htemp2",samples,low,up);
  TH1D *htemp3 = new TH1D("htemp3","htemp3",samples,low,up);


  TString scut = Form("time>%f && time<%f && %s.Device_Error_Code == 0 && ErrorFlag == 0",low,up,device.Data());
  
  amplitude = Form("%s.hw_sum",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile1",scut,"prof");
  profile1 = (TProfile*) gDirectory -> Get("profile1");
  if(profile1 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile1 -> Draw();
  Double_t m = profile1->GetMean(2);
  htemp0 = (TH1D*)profile1->ProjectionX("Signal block0");
//   //htemp0->DrawCopy();

//   amplitude = Form("qwk_%s.block1",device.Data());
//   tree->SetAlias("amplitude",amplitude);
//   tree->Draw("amplitude:time>>profile2",scut,"");
//   profile2 = (TProfile*) gDirectory -> Get("profile2");
//   if(profile2 == NULL){
//     std::cout<<"Unable to plot "<<amplitude<<std::endl;
//     exit(1);
//   }
//   profile2 -> Draw();
//   m+=profile2->GetMean(2);
//   htemp1 = (TH1D*)profile2->ProjectionX("Signal block1");
//   //  htemp1->DrawCopy();
   
//   amplitude = Form("qwk_%s.block2",device.Data());
//   tree->SetAlias("amplitude",amplitude);
//   tree->Draw("amplitude:time>>profile3",scut,"");
//   profile3 = (TProfile*) gDirectory -> Get("profile3");
//   if(profile3 == NULL){
//     std::cout<<"Unable to plot "<<amplitude<<std::endl;
//     exit(1);
//   }
//   profile3 -> Draw();
//   m+=profile3->GetMean(2);
//   htemp2 = (TH1D*)profile3->ProjectionX("Signal block2");
//   //htemp2->DrawCopy();
    
//   amplitude = Form("qwk_%s.block3",device.Data());
//   tree->SetAlias("amplitude",amplitude);
//   tree->Draw("amplitude:time>>profile4",scut,"");
//   profile4 = (TProfile*) gDirectory -> Get("profile4");
//   if(profile4 == NULL){
//     std::cout<<"Unable to plot "<<amplitude<<std::endl;
//     exit(1);
//   }
//   profile4 -> Draw();
//   m+=profile4->GetMean(2);
//   htemp3 = (TH1D*)profile4->ProjectionX("Signal block3");
//   //htemp3->DrawCopy();

//   htemp0->Add(htemp1,1);
//   htemp0->Add(htemp2,1);
//   htemp0->Add(htemp3,1);
//   htemp0->Scale(1.0/4);
//  htemp0->Draw();

  std::cout<<" --- Average signal ="<<m<<"\n";
  
  canvas->cd(1);
  //Remove the DC/zero frequency component 
  TH1D *h2 = new TH1D("h2","noise profile",samples,low,up);  
  TH1D *h2_1 = new TH1D("h2_1","noise profile",samples,low,up);  

  Double_t setvalue;
  TAxis *xa = htemp0 -> GetXaxis();
  Double_t nbins =  xa->GetNbins();
  for(Int_t n = 0;n < nbins;n++){
    setvalue = (htemp0->GetBinContent(n+1)-m);
    // to get rid of the dc component 
    if(htemp0->GetBinContent(n+1) ! = 0){
      h2_1->SetBinContent(n+1,setvalue);
    }
    h2->SetBinContent(n+1,setvalue);
  }
  h2_1->SetLineColor(2);
  h2_1 -> SetTitle("Profile of the noise");
  h2_1 -> GetXaxis() -> SetTitle("Time (s)");
  h2_1 -> GetYaxis() -> SetTitle("Amplitude");

  h2_1->Draw();
  
  // //Get the magnitude of the fourier transform
  canvas -> cd(2);
  gPad->SetLogy();
  TH1 * fftmag = NULL;
  TVirtualFFT::SetTransform(0);
  fftmag = ((TH1*)h2_1)->FFT(fftmag,"MAG");
  fftmag -> SetTitle("Magnitude of the transform");
  fftmag -> Draw();
  
  
  // rescale the x axis of the transform to get only the positive frequencies. 
  // Due to the periodicity of FFT in number of samples the FFT output will be a mirrored image.
  
 
  TAxis *xa = fftmag -> GetXaxis();
  Double_t nbins =  xa->GetNbins();
  
  Double_t frequency = sampling_rate;
  Double_t scale = 1.0/sqrt(samples);
  
  TH1D *h3 = new TH1D("h3",Form("Frequency Spectrum of %s",device.Data()) ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    h3->SetBinContent(n+1,scale*fftmag->GetBinContent(n+1)); 
  }
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetYaxis() -> SetTitle("Amplitude");
  h3->Draw();
  
  TAxis *xa = h3 -> GetXaxis();
  xa -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
  h3 -> Draw();
  
  //Get the phase of the transform
  canvas -> cd(3);
  TH1 *fftphase = NULL; 
  fftphase = htemp0->FFT(fftphase,"PH");
  
  fftphase->SetTitle("Phase of the transform");
  //  fftphase->Draw();
  
  TAxis *xp = fftphase -> GetXaxis();
  Double_t nbinsp =  xp->GetNbins(); 
  TH1D *hp = new TH1D("","Phase of the Transform",nbinsp,0,frequency);  
  
  for(Int_t n = 0;n <nbins;n++){
    Double_t setp = (fftphase->GetBinContent(n));
    hp->SetBinContent(n,setp); 
  }
  hp -> GetXaxis() -> SetTitle("Frequency (Hz)");
  hp -> GetYaxis() -> SetTitle("Phase (radians)");
  hp->Draw();
  TAxis *xp = hp -> GetXaxis();
  xp -> SetRangeUser(xp->GetBinLowEdge(1), xp->GetBinUpEdge(xp->GetNbins()/2.0));
  hp -> Draw();
  
  //Plot FFT with log-log scales
  gPad->SetLogy();
  gPad->SetLogx();
  h3 -> Draw();


  //Get the current transform object:
  TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
  canvas -> cd(4);     
  
  //Look at the DC component and the Nyquist harmonic:
  Double_t re, im;
  
  // *********************** Use the following method to get just one point of the output
  fft->GetPointComplex(0, re, im);
  printf(" DC component: %f Hz\n", re);
  fft->GetPointComplex(samples/2+1, re, im); 
  printf(" Nyquist harmonic: %f Hz\n",re);
  
  // ***********************   Use the following method to get the full output:
  canvas -> cd(4);  
  Double_t *re_full = new Double_t[samples];
  Double_t *im_full = new Double_t[samples];
  fft->GetPointsComplex(re_full,im_full);
  
  
  // // ***********************  Get the backward transform
  TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &samples, "C2R M K");
  fft_back->SetPointsComplex(re_full,im_full);
  fft_back->Transform();
 
  TH1 *hb = 0;
  TH1D *hb_1 = new TH1D("hb_1","backward transform",samples,low,up);  
  hb = TH1::TransformHisto(fft_back,hb,"Re");
  

  //Convert axis from Hz to s to get proper units
  TAxis *xb = hb -> GetXaxis();
  Double_t nbinsb =  xb->GetNbins();
  Double_t scale = 1/nbinsb;
  TH1D *h4 = new TH1D("h4","Backward Transform ",nbinsb,low,up);  
  for(Int_t n = 0;n <nbinsb;n++){
    Double_t setvalue = scale * (hb->GetBinContent(n+1));
    //0.016 ~ 0 frequenzy component.	
    if(fabs(setvalue) > 0.016 && fabs(setvalue) != 0.0)
      h4->SetBinContent(n+1,setvalue-m); 
  }
  h4 -> GetXaxis() -> SetTitle("Time (s)");
  h4 -> GetYaxis() -> SetTitle("Amplitude");
  h4->Draw();
  
  canvas -> Print(Form("%d_FFT_of_%s.gif",run_number,device.Data()));
  canvas -> Modified();
  canvas -> Update();
  return;
}

// Adapted from hallc_bcm_calib.C GetTree function written by J.H.Lee.
// This function use the chain to add all of the files with name 
// "filename" and then use that chain to get the corresponding file list.
// it return s 1 if files are found. 0 if not.

Bool_t FindFiles(TString filename, TChain * chain)
{
  TString file_dir;
  Bool_t  chain_status = kFALSE;

  //file_dir = gSystem->Getenv("QW_ROOTFILES/");
  //if (!file_dir) 
  file_dir = "$QW_ROOTFILES/";
  std::cout<<file_dir<<std::endl;
  chain_status = chain->Add(Form("%s%s", file_dir.Data(), filename.Data()));
  
  if(chain_status) {
    
    TString chain_name = chain -> GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;

    while (( chain_element=(TChainElement*)next() )){      
      std::cout << "File:  " 
		<< chain_element->GetTitle() 
		<< " is added into the file list with the name"<< std::endl;
    }
  }
  else {
    std::cout << "There is (are) no "
	      << filename 
	      << "."
	      << std::endl;
  }
  return chain_status;
};


