//************************************************************************************************************************************************//
//                                                                                                                                                //
//    This macros performs the Fast Fourier Transform of an array of devices and gives out,                                                       //
//    the FFT of the devices.                                                                                                                     // 
//    The reuslts are saved in to two  xxxx.gif files.                                                                                                   // 
//                                                                                                                                                //
//    Author   : B. Waidyawansa                                                                                                                   //
//    contact  : buddhini@jlab.org                                                                                                                //
//    Reference: http://root.cern.ch/root/html/tutorials/fft/FFT.C.html                                                                           //
//                                                                                                                                                //
//************************************************************************************************************************************************//
//                                                                                                                                                //
//                                                                                                                                                //
//     run_number        : Number of the run containing the signal                                                                                //
//     device list       : The device signal which is to be FFT'd. eg, "hcbpmx","hcbpmy","injbpmx" and injbpmy"                                  //
//                         Right now there are the only two device arrays implemented.                                                            //
//     min               : The lower limit of the event cut.                                                                                      //
//     max               : The upper limit of the event cut                                                                                       //
//                                                                                                                                                //
//************************************************************************************************************************************************// 
// Hw to use: e.g.
//root [6] FFT_multi(6206,"bpmx",5000,30000)
// --- Signal = bpmx 
// --- Length of the signal = 24s
// --- Sampling Rate = 1041.67
// --- Sample Size = 25000
// --- Average signal =-4.68837
// DC component: -336356.167598 Hz
// Nyquist harmonic: 3.649824 Hz
//Info in <TCanvas::Print>: GIF file 6206_FFT_of_bpmx_1.gif has been created
//Info in <TCanvas::Print>: GIF file 6206_FFT_of_bpmx_2.gif has been created
//
void 
FFT_mutli(Int_t run_number, TString devicelist, Int_t min, Int_t max)
{
  gDirectory->Delete("*") ;
  //stats
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);     
  gStyle->SetOptStat(0); 

  
  // histo parameters
  //  gStyle->SetTitleYOffset(1);
  gStyle->SetTitleXOffset(0.9);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleSize(0.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(18);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");

  gROOT->ForceStyle();




  //******* ADC settings
  //   const Double_t count_to_voltage = 0.00007629; //conversion factor
  const Double_t time_per_sample = 2e-06;//s
  const Double_t t_settle = 50e-06;//s
  
  //******* Signal Settings
  // get num samples from bcm1. 
  TString time    = Form("mps_counter*((qwk_1i02XP.num_samples*%f)+%f)",time_per_sample,t_settle);// units seconds.

  //*******BPM list
  const Int_t nbpms = 22;
  const Int_t injbpms = 21;

  TString axis[2] ={"X","Y"};
  TString bpms[nbpms]= {
    "qwk_bpm3c07","qwk_bpm3c08","qwk_bpm3c11","qwk_bpm3c12","qwk_bpm3c14","qwk_bpm3c16",
    "qwk_bpm3c17","qwk_bpm3c18","qwk_bpm3c19","qwk_bpm3p02a","qwk_bpm3p02b","qwk_bpm3p03a",
    "qwk_bpm3c20","qwk_bpm3c21","qwk_bpm3h02","qwk_bpm3h04","qwk_bpm3h07a","qwk_bpm3h07b",
    "qwk_bpm3h07c","qwk_bpm3h08","qwk_bpm3h09","qwk_bpm3h09b"
  };

  TString injbpm[injbpms]={
    "qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
    "qwk_0i05" ,"qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
    "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
    "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
    "qwk_0r06"
  };
  
  
  TCanvas *canvas = NULL;
  canvas = new TCanvas("canvas","Fast Fourier Transform",10,10,1550,1180);
  canvas->Draw();


  //******** get tree
  TChain *tree = new TChain("Mps_Tree");

  TString filename = Form("QwPass1_%i.000.root", run_number);
  TFile *file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
  
  if (file->IsZombie()) {
    std::cout << "Error opening root file chain " << filename << std::endl;
    filename = Form("QwPass1_%i.root", run_number);
    std::cout << "Try to open chain " << filename << std::endl;
    file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
    if (file->IsZombie()) {
      std::cout << "Error opening root file chain " << filename << std::endl;
      filename = Form("Qweak_%i.000.root", run_number);
      std::cout << "Try to open chain " << filename << std::endl;
      file = new TFile(Form("$QW_ROOTFILES/%s", filename.Data()));
      if(file->IsZombie()){
	tree = NULL;
	exit(-1);
      }
      else
	tree->Add(Form("$QW_ROOTFILES/Qweak_%i.*.root", run_number));	
    }
    else 
      tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.root", run_number));
  }
  else 
    tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root", run_number));
  
  tree->SetAlias("time",time);
  
  
  //************ Signal properties
  Int_t samples = (max-min);
  
  // get sampling rate/event rate
  // for this I am going to use the bcm1 num spales.
  TH1*h=NULL;
  tree->Draw("qwk_1i02XP.num_samples>>htemp","qwk_1i02XP.Device_Error_Code == 0");
  h = (TH1*)gDirectory->Get("htemp");
  if(h==NULL){
    std::cout<<"Unable to get num_samples using bcm1!"<<std::endl;
    exit(1);
  }
  std::cout<<h->GetMean()<<std::endl;
  Double_t sampling_rate = 1.0/(h->GetMean()*time_per_sample+t_settle);
  Double_t length = samples*1.0/sampling_rate; 
  
  std::cout<<" --- Signal = "<<devicelist<<"\n";
  std::cout<<" --- Length of the signal = "<<length<<"s\n";
  std::cout<<" --- Sampling Rate = "<<sampling_rate<<"\n";
  std::cout<<" --- Sample Size = "<<(max-min)<<"\n";
  
  delete h;

  //************  Plot the signal.
  Double_t up = (1.0*max)/sampling_rate;
  Double_t low = (1.0*min)/sampling_rate;
  
  if(devicelist == "bpmx"||devicelist =="injbpmx")
    TString useaxis = axis[0];
  else if(devicelist == "bpmy"||devicelist =="injbpmy")
    TString useaxis = axis[1];
  else{
    std::cout<<"Unknown device type "<<devicelist<<std::endl;
    exit(1);
  }; 
  
  Int_t k =1;
  canvas -> Divide(4,3);
  //     canvas -> Print(Form("%d_FFT_of_%s%s.ps(",run_number,devicelist.Data(),axis[j].Data()));

  // Do FFT on hallC bpms
  if(devicelist.Contains("hc")){
    for(Int_t i=0;i<nbpms;i++){
      canvas -> cd(k);
      TString device = Form("%s%s",bpms[i].Data(),useaxis.Data());      
      std::cout<<"Get FFT of "<<device<<std::endl;
      get_fft(run_number, tree, device, samples, up, low, sampling_rate); 
      canvas -> Modified();
      canvas -> Update();
      k++;
      if(k==13){
	k=1;
	canvas -> Print(Form("%d_FFT_of_%s_1.gif",run_number,devicelist.Data()));
      }
    };
    
    canvas->cd(11);
    gPad->Clear();
    canvas->cd(12);
    gPad->Clear();
  }


  // Do FFT on injector bpms
   if(devicelist.Contains("inj")){
     for(Int_t i=0;i<injbpms;i++){
       canvas -> cd(k);
       TString device = Form("%s%s",injbpm[i].Data(),useaxis.Data());
       std::cout<<"Get FFT of "<<device<<std::endl;
       get_fft(run_number, tree, device, samples, up, low, sampling_rate); 
       canvas -> Modified();
       canvas -> Update();
       k++;
       if(k==13){
	 k=1;
	 canvas -> Print(Form("%d_FFT_of_%s_1.gif",run_number,devicelist.Data()));
       }
     };
     canvas->cd(10);
     gPad->Clear();
     canvas->cd(11);
     gPad->Clear();
     canvas->cd(12);
     gPad->Clear();

   }
   canvas -> Print(Form("%d_FFT_of_%s_2.gif",run_number,devicelist.Data()));
   
   return;
}


void get_fft(Int_t runnumber, TChain* tree, TString device, Int_t samples, Double_t up, Double_t low, Double_t sampling_rate)
{
  
 
  TProfile *profile1 = new TProfile("profile1","Signal Profile1",samples,low,up);
  TProfile *profile2 = new TProfile("profile2","Signal Profile2",samples,low,up);
  TProfile *profile3 = new TProfile("profile3","Signal Profile3",samples,low,up);
  TProfile *profile4 = new TProfile("profile4","Signal Profile4",samples,low,up);
  
  TH1D *htemp0 = new TH1D("htemp0","htemp0",samples,low,up);
  TH1D *htemp1 = new TH1D("htemp1","htemp1",samples,low,up);
  TH1D *htemp2 = new TH1D("htemp2","htemp2",samples,low,up);
  TH1D *htemp3 = new TH1D("htemp3","htemp3",samples,low,up);

  TString scut = Form("time>%f && time<%f && %s.Device_Error_Code == 0",low,up,device.Data());
      
  TString amplitude = Form("%s.bclock0",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile1",scut,"prof");
  profile1 = (TProfile*) gDirectory -> Get("profile1");
  if(profile1 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile1 -> DrawCopy();
  Double_t m = profile1->GetMean(2);
  htemp0 = (TH1D*)profile1->ProjectionX("Signal block0");
  //htemp0->DrawCopy();
  delete profile1;

  amplitude = Form("%s.bclock1",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile2",scut,"");
  profile2 = (TProfile*) gDirectory -> Get("profile2");
  if(profile2 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile2 -> DrawCopy();
  m+=profile2->GetMean(2);
  htemp1 = (TH1D*)profile2->ProjectionX("Signal block1");
  //  htemp1->DrawCopy();
  delete profile2;

  amplitude = Form("%s.bclock2",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile3",scut,"");
  profile3 = (TProfile*) gDirectory -> Get("profile3");
  if(profile3 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile3 -> DrawCopy();
  m+=profile3->GetMean(2);
  htemp2 = (TH1D*)profile3->ProjectionX("Signal block2");
  //htemp2->DrawCopy();
  delete profile3;
      
  amplitude = Form("%s.bclock3",device.Data());
  tree->SetAlias("amplitude",amplitude);
  tree->Draw("amplitude:time>>profile4",scut,"");
  profile4 = (TProfile*) gDirectory -> Get("profile4");
  if(profile4 == NULL){
    std::cout<<"Unable to plot "<<amplitude<<std::endl;
    exit(1);
  }
  profile4 -> DrawCopy();
  m+=profile4->GetMean(2);
  htemp3 = (TH1D*)profile4->ProjectionX("Signal block3");
  //htemp3->DrawCopy();
  delete profile4;
 
  htemp0->Add(htemp1,1);
  htemp0->Add(htemp2,1);
  htemp0->Add(htemp3,1);
  htemp0->Scale(1.0/4);
  //  htemp0->DrawCopy();
  
  
  std::cout<<" --- Average signal ="<<m/4<<"\n";
      
  
  //Remove the DC/zero frequency component 
  TH1D *h2 = new TH1D("h2","noise profile",samples,low,up);    
  Double_t setvalue;
  
  TAxis *xa = htemp0 -> GetXaxis();
  Double_t nbins =  xa->GetNbins();
  for(Int_t n = 0;n < nbins;n++){
    setvalue = (htemp0->GetBinContent(n+1)-m/4);
    // to get rid of the dc component 
    h2->SetBinContent(n+1,setvalue);
  }

  // //Get the magnitude of the fourier transform
  TH1 * fftmag = NULL;
  TVirtualFFT::SetTransform(0);
  fftmag = ((TH1*)h2)->FFT(fftmag,"MAG");
  //  fftmag -> SetTitle(Form("FFT of %s in %i",device.Data(), runnumber));
  fftmag -> Draw();
  
  // rescale the x axis of the transform to get only the positive frequencies. 
  // Due to the periodicity of FFT in number of samples the FFT output will be a mirrored image.
  
  TAxis *xa = fftmag -> GetXaxis();
  Double_t nbins =  xa->GetNbins();
  
  Double_t frequency = sampling_rate;
  Double_t scale = 1.0/sqrt(samples);
  
  TH1D *h3 = new TH1D("h3",Form("FFT of %s in run %i",device.Data(), runnumber) ,nbins,1,frequency);  
  for(Int_t n = 0;n<nbins;n++){
    h3->SetBinContent(n+1,scale*fftmag->GetBinContent(n+1)); 
  }
  h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
  h3 -> GetXaxis() -> SetTitleSize(0.05);
  h3 -> GetYaxis() -> SetTitle("Amplitude");
  h3->Draw();
  
  TAxis *xa = h3 -> GetXaxis();
  xa -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
  h3 -> DrawCopy();

  delete htemp0;  
  delete htemp1;
  delete htemp2;
  delete htemp3;
  delete h2;    
  delete h3;
  delete fftmag;
  
  
}
