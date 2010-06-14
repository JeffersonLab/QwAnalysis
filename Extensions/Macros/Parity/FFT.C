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
//     device            : The device signal which is to be FFT'd. eg, "qwk_bcm0l02".                                                             //
//     min               : The lower limit of the event cut.                                                                                      //
//     max               : The upper limit of the event cut                                                                                       //
//     sampling_rate     : sampling rate of the ADC. In the integrating mode, this is equal to 4/(hold off time+helicity window)                  //
//                         ( The factor of 4 comes from the oversampling of TRIUMF ADCs)                                                          //
//     samples per block : The number of samples read within an ADC block. This can be obtained using,                                            //
//                         samples_per_block = (length of the helicity window)/(4 * time between the samples).                                    //
//                         Ususally the time between the samples is 2us.                                                                          //
//                                                                                                                                                //
//************************************************************************************************************************************************// 

void 
FFT(Int_t run_number, TString device, Int_t min, Int_t max, Int_t sampling_rate, Int_t samples_per_block)
{
  gDirectory->Delete("*") ;
  gROOT -> ProcessLine(".L ~/QwAnalysis_New/Extensions/Macros/Tracking/TSuperCanvas.cc+");

  TSuperCanvas *canvas = NULL;

  canvas = new TSuperCanvas("canvas","Fast Fourier Transform",10,10,1000,680);
  
  //******* ADC settings
  const Double_t count_to_voltage = 0.00007629; //conversion factor
 
  
  //******* Signal Settings
  TString time    = Form("event_number*(1.0/%d)",sampling_rate);// units seconds.
  TString amplitude=""; 

  Int_t samples = (max-min);
  Double_t length = samples*1.0/sampling_rate;
  std::cout<<" --- Signal = "<<device<<"\n";
  std::cout<<" --- Length of the signal = "<<length<<"s\n";
  std::cout<<" --- Sampling Rate = "<<sampling_rate<<"\n";
  std::cout<<" --- Sample Size = "<<(max-min)<<"\n";
  



  //******* Open Root file
  TFile *file =  new TFile(Form("~/scratch/rootfiles/Qweak_%d.root",run_number));
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete canvas;canvas = NULL;
      return;
    }
  else
    {
            
      TTree * mpstree = (TTree *) file->Get("MPS_Tree");
      mpstree->SetAlias("time",time);
      canvas -> Divide(2,2);  

  
      //************  Plot the signal.
      canvas -> cd(1);
      Double_t up = (1.0*max)/sampling_rate;
      Double_t low = (1.0*min)/sampling_rate;
      

      TProfile *profile1 = new TProfile("profile1","Signal Profile1",samples,low,up);
      TProfile *profile2 = new TProfile("profile2","Signal Profile2",samples,low,up);
      TProfile *profile3 = new TProfile("profile3","Signal Profile3",samples,low,up);
      TProfile *profile4 = new TProfile("profile4","Signal Profile4",samples,low,up);

      TH1D *htemp[4];

      TString scut = Form("time>%f&&time<%f",low,up);

      mpstree->SetAlias("scut",scut);


      amplitude = Form("%s.bclock0*%e/(4.0*%d)",device.Data(),count_to_voltage,samples_per_block);
      mpstree->SetAlias("amplitude",amplitude);
      mpstree->Draw("amplitude:time>>profile1","scut","");
      profile1 = (TProfile*) gDirectory -> Get("profile1");
      profile1 -> Draw();
      Double_t m = profile1->GetMean(2);
      htemp[0] = profile1->ProjectionX("Signal block0");
      htemp[0]->Draw();
    

      amplitude = Form("%s.bclock1*%e/(4.0*%d)",device.Data(),count_to_voltage,samples_per_block);
      mpstree->SetAlias("amplitude",amplitude);
      mpstree->Draw("amplitude:time>>profile2","scut","");
      profile2 = (TProfile*) gDirectory -> Get("profile2");
      profile2 -> Draw();
      m+=profile2->GetMean(2);
      htemp[1] = profile2->ProjectionX("Signal block1");
      htemp[1]->Draw();
   

      amplitude = Form("%s.bclock2*%e/(4.0*%d)",device.Data(),count_to_voltage,samples_per_block);
      mpstree->SetAlias("amplitude",amplitude);
      mpstree->Draw("amplitude:time>>profile3","scut","");
      profile3 = (TProfile*) gDirectory -> Get("profile3");
      profile3 -> Draw();
      m+=profile3->GetMean(2);
      htemp[2] = profile3->ProjectionX("Signal block2");
      htemp[2]->Draw();
    

      amplitude = Form("%s.bclock3*%e/(4.0*%d)",device.Data(),count_to_voltage,samples_per_block);
      mpstree->SetAlias("amplitude",amplitude);
      mpstree->Draw("amplitude:time>>profile4","scut","");
      profile4 = (TProfile*) gDirectory -> Get("profile4");
      profile4 -> Draw();
      m+=profile4->GetMean(2);
      htemp[3] = profile4->ProjectionX("Signal block3");
      htemp[3]->Draw();


      htemp[0]->Add(htemp[1],1);
      htemp[0]->Add(htemp[2],1);
      htemp[0]->Add(htemp[3],1);
      htemp[0]->Draw();

     

      std::cout<<" --- Average signal ="<<m/4<<"V \n";


      //Remove the DC/zero frequency component 

      TH1D *h2 = new TH1D("h2","noise profile",samples,low,up);  
      Double_t setvalue;
      TAxis *xa = htemp[0] -> GetXaxis();
      Double_t nbins =  xa->GetNbins();
      for(Int_t n = 0;n < nbins;n++){
	setvalue = (htemp[0]->GetBinContent(n+1)-m)/4;
	h2->SetBinContent(n+1,setvalue); 
      }
      h2->SetLineColor(2);
      h2 -> SetTitle("Profile of the noise");
      h2 -> GetXaxis() -> SetTitle("Time (s)");
      h2 -> GetYaxis() -> SetTitle("Amplitude (V)");
      canvas->cd(1);
      h2->Draw();

      //Get the magnitude of the fourier transform
      canvas -> cd(2);
      TH1 * fftmag = NULL;
      TVirtualFFT::SetTransform(0);
      fftmag = ((TH1*)h2)->FFT(fftmag,"MAG");
      fftmag -> SetTitle("Magnitude of the transform");
      fftmag -> Draw();


      // rescale the x axis of the transform to get only the positive frequencies. 
      // Due to the periodicity of FFT in number of samples the FFT output will be a mirrored image.


      TAxis *xa = fftmag -> GetXaxis();
      Double_t nbins =  xa->GetNbins();
     
      Double_t frequency = sampling_rate;
      Double_t scale = 1.0/sqrt(samples);

      TH1D *h3 = new TH1D("h3","Power Spectrum ",nbins,0,frequency);  
      
      for(Int_t n = 0;n <nbins;n++){
	Double_t set = fftmag->GetBinContent(n);
	h3->SetBinContent(n,set*scale); 
      }
      h3 -> GetXaxis() -> SetTitle("Frequency (Hz)");
      h3 -> GetYaxis() -> SetTitle("Power (V^2)");
      h3->Draw();

      TAxis *xa = h3 -> GetXaxis();
      xa -> SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins()/2.0));
      h3 -> Draw();

      //Get the phase of the transform
      canvas -> cd(3);
      TH1 *fftphase = NULL; 
      fftphase = htemp[0]->FFT(fftphase,"PH");

      fftphase->SetTitle("Phase of the transform");
      fftphase->Draw();

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


      // ***********************  Get the backward transform
      TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &samples, "C2R M K");
      fft_back->SetPointsComplex(re_full,im_full);
      fft_back->Transform();
      TH1 *hb = 0;
      hb = TH1::TransformHisto(fft_back,hb,"Re");
      hb->SetTitle("The backward transform");
      hb->Draw();


      
      //Convert axis to get proper units
      TAxis *xb = hb -> GetXaxis();
      Double_t nbinsb =  xb->GetNbins();
      Double_t scale = 1/nbinsb;
      TH1D *h4 = new TH1D("h4","Backward Transform ",nbinsb,low,up);  
      
      for(Int_t n = 0;n <nbinsb;n++){
	Double_t set = (hb->GetBinContent(n))+m;
	h4->SetBinContent(n,set*scale); 
      }
      h4 -> GetXaxis() -> SetTitle("Time (s)");
      h4 -> GetYaxis() -> SetTitle("Amplitude (V)");
      h4->Draw();


      canvas->Print(Form("%d_FFT_j28.gif",run_number));
      canvas -> Modified();
      canvas -> Update();
      return;
    }
}



