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
// 
// Updates
// 12-01-2011 B. Waidyawansa : Switched to using helicity rate from the Slow_Tree.
// 01-06-2012 B. Waidyawansa : Switched to using time derived from mps and helfreq instead of time per sample.
// 

void 
FFT(Int_t run_number, TString device, Int_t min, Int_t max)
{
  gDirectory->Delete("*") ;
  gStyle->SetOptStat(kFALSE);


  // Create trees
  TChain *tree = new TChain("Mps_Tree");
  TChain *slow_tree = new TChain("Slow_Tree");


  // Find the rootfiles
  TString filename = Form("QwPass1_%i.000.root", run_number);

  Bool_t found_hel = kFALSE;
  Bool_t found_slow = kFALSE;

  found_hel = FindFiles(filename, tree);
  found_slow = FindFiles(filename, slow_tree);
  
  if(!found_hel || !found_slow){
    filename = Form("first100k_%i.root", run_number);
    found_hel = FindFiles(filename, tree);
    found_slow = FindFiles(filename, slow_tree);
    if(!found_hel || ! found_slow){
      filename = Form("Qweak_%i.000.root", run_number);
      found_hel = FindFiles(filename, tree);
      found_slow = FindFiles(filename, slow_tree);
      if(!found_hel || !found_slow){
	std::cerr<<"Unable to find rootfile(s) for run "<<run_number<<std::endl;
	exit(1);
      }
    }
  }

  
  // Signal properties
  Int_t samples = (max-min);
  
  // sampling rate/event rate
  TH1*h=NULL;
  TString command = "HELFREQ>>htemp";
  slow_tree->Draw(command,"","goff");
  h = (TH1*)gDirectory->Get("htemp");
  if(h==NULL){
    std::cout<<"Unable to find HELFREQ. Check Slow_Tree!"<<std::endl;
    exit(1);
  }
  
  Double_t sampling_rate = h->GetMean();
  delete h;
  if((sampling_rate>961) || (sampling_rate< 959)) {
    std::cout<<"Sampling rate "<<sampling_rate<<"Hz is not acceptable as the default Qweak setting. The default sampling rate of Qweak ADCs should be ~ 960HZ!"<<std::endl;
    exit(1);
  }
  Double_t length = samples*1.0/sampling_rate; // signal length in seconds 
  
  Double_t up = (1.0*max)/sampling_rate; // uper limit of the signal in seconds
  Double_t low = (1.0*min)/sampling_rate; // lower limit of the signal in seconds


  std::cout<<" --- Signal = "<<device<<"\n";
  std::cout<<" --- Signal starting at "<<low<<"s and ending at "<<up<<"s "<<std::endl;
  std::cout<<" --- Length of the signal = "<<length<<" s\n";
  std::cout<<" --- Sampling Rate = "<<sampling_rate<<" Hz \n";
  std::cout<<" --- Sample Size = "<<(max-min)<<" events \n";
  


  // Open a canvas
  TCanvas *canvas = NULL;
  canvas = new TCanvas("canvas","Fast Fourier Transform",10,10,1000,680);
  canvas->Draw();
  canvas -> Divide(2,2);

  //***********************************
  //***** Plot the signal.
  //***********************************

  canvas -> cd(1);
  
  TProfile *profile = new TProfile("profile","Signal Profile",samples,low,up);  
  TString amplitude = Form("%s.hw_sum",device.Data());
  tree->SetAlias("amplitude",amplitude);
  TString scut = Form("mps_counter>%d && mps_counter<%d && %s.Device_Error_Code == 0 && ErrorFlag == 0",min,max,device.Data());
  tree->Draw(Form("amplitude:(mps_counter/%f)>>profile",sampling_rate),scut,"prof");

  profile = (TProfile*) gDirectory -> Get("profile");
  if(profile == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  
  gPad->Update();

  //***********************************
  //***** Remove the DC/zero frequency component 
  //***********************************

  Double_t dc_component = profile->GetMean(2);
  std::cout<<" --- Average signal (DC component)  ="<<dc_component<<"\n";
  TAxis *xa = profile -> GetXaxis();
  Double_t nbins_prof =  xa->GetNbins();
  TH1D *htemp = new TH1D("htemp","htemp",samples,low,up);
  htemp = (TH1D*)profile->ProjectionX("Signal");
  
  Double_t setvalue=0.0;
  xa = htemp -> GetXaxis();
  Double_t nbins =  xa->GetNbins();
  TH1D *hnoise = new TH1D("hnoise","hnoise",samples,low,up);  

  for(Int_t n = 0;n < nbins;n++){
    setvalue = (htemp->GetBinContent(n+1)-dc_component); // from each bin, subtract DC value
    if(htemp->GetBinContent(n+1) != 0){
      hnoise->SetBinContent(n+1,setvalue);
    }
  }
  
  hnoise->SetLineColor(2);
  hnoise -> SetTitle("Noise profile");
  hnoise -> GetXaxis() -> SetTitle("Time (s)");
  hnoise -> GetYaxis() -> SetTitle("Amplitude");
  hnoise->Draw();
  
  //***********************************
  //**** Get the magnitude of the fourier transform
  //***********************************

  canvas -> cd(2);
  gPad->SetLogy();
  TH1 * fftmag = NULL;
  TVirtualFFT::SetTransform(0);
  fftmag = ((TH1*)hnoise)->FFT(fftmag,"MAG");  
  
  // Rescale the x axis of the transform to get only the positive frequencies. 
  // Due to the periodicity of FFT in number of samples the FFT output will be a mirrored image.
   
  xa = fftmag -> GetXaxis();
  Double_t nbins_mag =  xa->GetNbins();
  Double_t frequency = sampling_rate;
  Double_t scale = 1.0/sqrt(samples);
  
  TH1D *hfft = new TH1D("hfft",Form("Frequency Spectrum of %s",device.Data()) ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    hfft->SetBinContent(n+1,scale*(fftmag->GetBinContent(n+1))); 
  }
  hfft -> GetXaxis() -> SetTitle("Frequency (Hz)");
  hfft -> GetYaxis() -> SetTitle("Amplitude");
  hfft->Draw();
  
  xa = hfft -> GetXaxis();
  xa -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
  hfft -> Draw();

  // Plot FFT with X and Y axis in log scale
  canvas->cd(3);
  gPad->SetLogy();
  gPad->SetLogx();
  hfft -> Draw();


  //***********************************
  //**** Draw the backward transform
  //***********************************

  // Get the current transform object:
  TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
  canvas -> cd(4);     
  
  Double_t re, im;
  
//   // Look at the DC component and the Nyquist harmonic:
//   fft->GetPointComplex(0, re, im);
//   printf(" --- DC frequency component: %f Hz\n", re);
//   fft->GetPointComplex((nbins/2-1), re, im); 
//   printf(" --- Nyquist harmonic: %f Hz\n",re);
  
  // Use the following method to get the full output/backward transform:
  Double_t *re_full = new Double_t[samples];
  Double_t *im_full = new Double_t[samples];
  fft->GetPointsComplex(re_full,im_full);
  

  TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &samples, "C2R M K");
  fft_back->SetPointsComplex(re_full,im_full);
  fft_back->Transform();
 
  TH1*hback = NULL;
  hback = TH1::TransformHisto(fft_back,hback,"Re");
  

  // Convert axis from Hz to s to get proper units
  xa = hback -> GetXaxis();
  Double_t nbinsb =  xa->GetNbins();
  Double_t scale = 1/nbinsb;
  TH1D *hbackt = new TH1D("hbackt","Backward Transform ",samples,low,up);  
  for(Int_t n = 0;n <nbinsb;n++){
    setvalue = scale * (hback->GetBinContent(n+1));
    hbackt->SetBinContent(n+1,setvalue-dc_component); 
  }
  hbackt -> GetXaxis() -> SetTitle("Time (s)");
  hbackt -> GetYaxis() -> SetTitle("Amplitude");
  hbackt->Draw();
  
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
  Int_t  chain_status = 0;

  file_dir = gSystem->Getenv("QW_ROOTFILES");
  // To make the chain actually check if a file exsists, have to set the number of entries<=0.
  chain_status = chain->Add(Form("%s/%s", file_dir.Data(), filename.Data()),0);
  
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


