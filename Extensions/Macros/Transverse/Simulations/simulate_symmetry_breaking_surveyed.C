//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : November 8th, 2012
//*****************************************************************************************************//
/*

This macro simulates the transverse symmetry breaking effect on the main detector array using surveyed
positions of the PMTs to get phi location of the octants.

<time_in_hours> Length of each measurement in hours.
 
/symmetry_breaking_surveyed <time>

For each measurement, the asymmetry on each bar is simulated using a gaussian random generator with 
a mean A_m = A_T*PH*TMath::sin(angle)) + PL*A_PV
and a sigma = 660/sqrt(<time_in_ours> * 3600 * 960 /4)

The output of the script is two distributions of P_V% and P_H% each fitted with a gaussian fit.
The plots are saved as * png format.

e.g. output:

buddhini@ou-qweak6 ~/QwAnalysisSVN/Extensions/Macros/Transverse/Simulations$ ./symmetry_breaking_surveyed 20
Simulating azimuthal symmetry breaking using surveyed data
Using A_T  = -4.8 ppm
Using A_PV = 0 ppm (to get pure transvesre leakage)
Statistical error per bar for 20 hour  = 0.158771
Number of patterns = 1.728e+07
Induced horizontal transverse polarization 100
Induced vertical transverse polarization 0



*/
//*****************************************************************************************************//
#include <iostream>
#include <fstream>
#include <TRandom1.h>
#include <stdio.h>
#include <TMath.h>
#include <TString.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TApplication.h>
#include <math.h>
#include <TCanvas.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <stdexcept>
#include <TLine.h>
#include <TPaveStats.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TText.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TPad.h>
#include <TVector.h>
#include <TH1.h>
#include <TFile.h>

using namespace std;

Double_t run_time = 0; 
Double_t PV = 0; 
Double_t PH = 1;
Double_t A_m = 0.0;
Double_t A_r = 0.0;


const Int_t iterate = 20000;
const Double_t md_bar_rms = 660; //ppm
const Double_t event_rate = 960; //Hz

const Double_t A_PV = -0.234; //ppm
const Double_t A_T  = -4.8; //ppm


//Calculated from surveyed pmt X and Y locations w.r.t QTOR coordinate system given in https://hallcweb.jlab.org/hclog/1006_archive/100617142707.html
//Double_t PMT_phi[8] = {-0.02651,45.0203,89.94823,135.0593,180.02331,225.2028,269.9432,314.81275};

// Calcualted from surveyed X and Y locations w.r.t QTOR as of 25 October 2012
Double_t PMT_phi[8] = {-0.06459,44.98115,90.0113,135.0999,179.9409,225.2329,270.0297,314.78325};

int main(Int_t argc,Char_t* argv[]){
  
  // Fit and stat parameters
  gStyle->SetOptStat(11);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.2);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(kWhite);

  // set pad style
  gStyle->SetPadTopMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetTitleFillColor(kWhite);

  // set hito style
  gStyle->SetTitleFont(132,"xy");
  gStyle->SetTitleFont(132,"1");
  gStyle->SetLabelFont(132,"xy");
  gStyle->SetStatFont(132);
  gStyle->SetTitleSize(0.05,"xy");
  gStyle->SetLabelSize(0.05,"xy");
  gStyle->SetEndErrorSize(3);


  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");
  
  TApplication theApp("App",&argc,argv);
  
  using namespace std;
  
  // Simulate the precision of the offset
  TRandom1 * random1 = new TRandom1();
  random1->SetSeed(0);

  Double_t offset_error = 0.0; //ppm
  
  
  Double_t md_mean[8];
  Double_t md_error[8];
  Double_t x[8];
  Double_t x_error[8];
  Double_t angle = 0;
  
  
  TRandom1 * random = new TRandom1();
  random->SetSeed(0);
  
  TCanvas * c1= new TCanvas("c1", "Fitted MD asymmetries",0,0,1000,800);
  //TString title = Form("LH2: Simulated Main detector asymmetries  A_m(phi) = %1.2f Sin(#phi+#phi_{0})) ",A_T,PH);
  TString title = Form("LH2: Simulated Main detector asymmetries  A_m(phi) = %1.2f Cos(#phi+#phi_{0})) ",A_T,PH);

  c1->SetFillColor(kWhite);

  if(argc==2){
    run_time = atoi(argv[1]);
  } 
  else  if(argc<2 || argc>2){
    std::cout<<" Correct usage is ./symmetry_breaking_surveyed <time>  "<<std::endl;
    exit(1);
  }
  
  //calculate bar asymmetry stat error
 Double_t patterns             = run_time*3600*event_rate/4.0;
 Double_t md_bar_stat_error    = md_bar_rms/sqrt(patterns);

 // to set histogram x axis limits
 Double_t p0_u = PV+0.3;
 Double_t p0_l = PV-0.3;
 Double_t p1_u = PH+0.3;
 Double_t p1_l = PH-0.3;


 TH1D* para_p0 = new TH1D("p0","% P_H",1000, 0.04,0.04);
 TH1D* para_p1 = new TH1D("p1","#phi",1000, -0.1,0.1);
 TH1D* para_p2 = new TH1D("p2","symmetry breaking term",1000, -0.3,0.3);

 std::cout<<"Simulating azimuthal symmetry breaking using surveyed data "<<std::endl;
 std::cout<<"Using A_T  = "<<A_T<<" ppm"<<std::endl;
 std::cout<<"Using A_PV = 0 ppm (to get pure transvesre leakage)"<<std::endl;
 std::cout<<"Statistical error per bar for "<<run_time<<" hour  = "<<md_bar_stat_error<<std::endl;
 std::cout<<"Number of patterns = "<<patterns<<std::endl;
 std::cout<<"Induced horizontal transverse polarization "<<PH*100<<" "<<std::endl;
 std::cout<<"Induced vertical transverse polarization "<<PV*100<<" "<<std::endl;

 TF1*trans_fit =  new TF1("trans_fit","-4.8*[0]*sin((pi/180)*(45*(x-1))+[1])+ [2]",1,8);
 //TF1*trans_fit =  new TF1("trans_fit","-4.8*[0]*cos((pi/180)*(45*(x-1))+[1])+ [2]",1,8);
 

 /*Fill the asymmetry histograms for iterate number of measurements*/
 for(Int_t k=0;k<iterate;k++){
      
   /*Simulate measured value*/
   for(Int_t i=1;i<9;i++){   
	
     angle =(TMath::Pi()/180)*(PMT_phi[i-1]);
     //angle =(TMath::Pi()/180)*(45*(i-1));

     // A_m = A_T*PV*TMath::Cos(angle);
     A_m = A_T*PH*TMath::Sin(angle);
	
     //std::cout<<" asymmetry "<<A_m<<" bar "<<i<<std::endl;
     /*Add the offset to md1*/
     md_mean[i-1] = random->Gaus(A_m,md_bar_stat_error);
     md_error[i-1] =  md_bar_stat_error;
        
     x[i-1] = i;
     x_error[i-1]=0.0; 
   }
   
   TGraph* grp  = new TGraph(8,x,md_mean);
   // Fit simulated value assuming perfect phi location of bars
   grp->Fit("trans_fit","Q");
   TF1 * fit = grp->GetFunction("trans_fit");
   if(fit==NULL) exit(1);
   para_p1->Fill((fit->GetParameter(1)));
   para_p2->Fill((fit->GetParameter(2)));
   
   if((k%5000) == 0) std::cout<<k<<std::endl;
   
 }

 c1->Divide(1,2);
 c1->cd(1);
 para_p1->Draw();
 para_p1->Fit("gaus","Q");
 para_p1->SetTitle(" Phase offset");
 para_p1->GetXaxis()->SetTitle("#phi_{0}");
 para_p1->GetYaxis()->SetTitle("No.of Measurements");

 c1->cd(2);
 para_p2->Draw();
 para_p2->Fit("gaus","Q");
 para_p2->SetTitle(" Symmetry breaking at current (as surveyed w.r.t QTOR) #phi locations ");
 para_p2->GetXaxis()->SetTitle("Symmetry breaking (ppm)");
 para_p2->GetYaxis()->SetTitle("No.of Measurements");

 c1->SaveAs(Form("sim_symmetry_breaking_%1.0f_P_V_%1.0f_P_H_%1.0f_surveyed_locations.C"
		 ,run_time,PV,PH));
 c1->SaveAs(Form("sim_symmetry_breaking_%1.0f_P_V_%1.0f_P_H_%1.0f_surveyed_locations.png"
		 ,run_time,PV,PH));



 std::cout<<"Done!";
 theApp.Run();
 return(1);
}



