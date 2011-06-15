//***************************************************
//***************************************************
//        Calibration of the bcm0l02 bcm                      
//***************************************************
//
// Author : B. Waidyawansa (buddhini@jlab.org)
// Date   : March 10, 2011
//********************************************************************//

void bcm_calibration(Int_t run_number)
{	  
  TString plotcommand;
  TString scut;

  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(1.8);
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.1);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);

  TCanvas *Canvas = new TCanvas("Canvas","BCM calibration",1200,1000);
  Canvas->Clear();
  Canvas->Divide(1,3);
  Canvas->cd(1);

  /*Open the rootfile/rootfiles*/
  TChain * tree      = new TChain("Mps_Tree");
  TString  Rfilename = Form("qwinjector_%i.000.root", run_number);
  TFile  * Rfile     = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

  if (Rfile->IsZombie()){
	  std::cout << "File exit failure."<<std::endl; 
	  tree = NULL;
  }
  else
	  tree->Add(Form("$QW_ROOTFILES/qwinjector_%i.000.root", run_number));
  
  if(tree == NULL){
	  std::cout<< "Unable to find the Mps_Tree." << std::endl; 
	  exit(1);
  }

  // Draw the current we set using scandata1 (Faraday cup 2)
  std::cout<<"Onto plotting current from Faraday cup"<<std::endl;
  scut = "scandata1>0 && cleandata == 1";
  plotcommand = "scandata1/1000>>htemp";
  tree->Draw(plotcommand,scut,"prof");

  TH1 *current=(TH1*)gROOT->FindObject("htemp");

  if(current == NULL){
      std::cout<<" scandata1 is empty!-> No current! \n Exiting program."<<std::endl;
      exit(1);
  }
  
  current->Draw();
  current->SetTitle("scandata1 (Current from Faraday Cup 2)");
  current->GetXaxis()->SetTitle("current #muA");
  current->DrawCopy();

  //Find the minmum and maximum current cut from the scandata plot 
  std::cout<<"Now find the minimum and maximum current used for the calibration."<<std::endl;
  
  /* get maximum and minimum range of the plot */
  Double_t tmp_max = current -> GetXaxis() -> GetXmax();
  Double_t tmp_min = current -> GetXaxis() -> GetXmin();
  Int_t nbins = current -> GetXaxis() -> GetNbins();

  /* now to get maximum and minimum range of current, */
  for(Int_t i=0;i<nbins; i++){
    if(current->GetBinContent(i)>1){
      tmp_min  = current-> GetBinLowEdge(i);
      break;  
    }
  }

  for(Int_t i=nbins;i>0;i--){
    if(current->GetBinContent(i)>1){
      tmp_max = current->GetXaxis()->GetBinUpEdge(i);
      break;
    }
  }

  std::cout<<"minimum current from scandata1 = "<<tmp_min<<" uA \n";
  std::cout<<"maximum current from scandata1 = "<<tmp_max<<" uA \n";
  std::cout<<"Now fit the current range to 20% to 90% of the ranage."<<std::endl;

  /*Calculate the fitting range for current based on the percent range
    given by user/default */
  Double_t range_length = tmp_max-tmp_min;
  Double_t fit_range_low = 0.0;
  Double_t fit_range_up = 0.9;

  fit_range_low *= range_length;
  fit_range_low += tmp_min; //lower limit of range

  fit_range_up *= range_length; 
  fit_range_up += tmp_min ; //upper limit of range

  scut = "scandata1>0 && cleandata == 1 && qwk_bcm0l02.Device_Error_Code == 0";
  plotcommand = "qwk_bcm0l02.hw_sum_raw/qwk_bcm0l02.num_samples:scandata1/1000>>htemp";

  Canvas->cd(2);
  tree->Draw(plotcommand,scut,"prof");
  TH2D *h=(TH2D*)gROOT->FindObject("htemp");

  if(h == NULL){
      std::cout<<" bcm0l02 data is empty!\n Exiting program."<<std::endl;
      exit(1);
  }
  
 
  h->Fit("pol1","Q","",fit_range_low,fit_range_up);
  TF1* f1 = h->GetFunction("pol1");
  if(f1 == NULL) exit(1);

  Double_t offset = f1->GetParameter(0);
  Double_t slope  = f1->GetParameter(1);
  f1->SetLineColor(kRed);

  // Moving on to plot residuals between scandata and current from bcm
  Canvas->cd(3);
  plotcommand = Form("(scandata1/1000)-((qwk_bcm0l02.hw_sum_raw/qwk_bcm0l02.num_samples)-%f)/%f:scandata1/1000 >> res",offset,slope); 
  std::cout<<plotcommand<<std::endl;
  tree->Draw(plotcommand,scut,"box");
  TH2D *res=(TH2D*)gROOT->FindObject("res");
  res->GetXaxis()->SetTitle("scandata1-(bcm0l02-offset)*gain");
  res->SetTitle("Residual plot (scandata1-bcm0l02)");
  res->SetLineColor(kRed);
  res->SetMarkerColor(kRed);
 

  std::cout <<"\nBCM 0l02\noffset=" 
			<< offset << "\n Gain=" 
			<< 1.0/slope<<"\n red chi2 = "<<(f1->GetChisquare())/(f1->GetNDF())
			<<"\nresidual (mean,rms) = ("<<Form("%5.2e",res->GetMean())
			<<" , "<<Form("%5.2e",res->GetRMS())<<") \n";
  
  zeroline=new TLine(tmp_min,0,tmp_max,0);
  zeroline->Draw("same");
    
  Canvas->Update();  

  // Save the canvas on to a ps file
  Canvas->Print(Form("%i_bcm_calibration.ps",run_number));
};

