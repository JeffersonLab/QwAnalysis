//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : October 27th, 2011
//*****************************************************************************************************//
/*

This macro simulates the effect of the non-zero offest seen in bar 1+5 for longitudinal data correction.

Inputs : time in hours, P_V % , P_H %


e.g ./sim_15_effect_on_correction 8 5 0  

The outputs of the 8 md distributions and the octant fits are saved in to *.png files.
*/
//*****************************************************************************************************//

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
#include <iostream>
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


 Double_t run_time = 0; 
 Double_t PV = 0; 
 Double_t PH = 0;
 Double_t PL = 0;
 Double_t A_m = 0.0;

Bool_t ldebug= false;


int main(Int_t argc,Char_t* argv[]){

  // Fit and stat parameters
  gStyle->SetOptStat(11);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.2);
  gStyle->SetOptFit(1111);
 
  
  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");
  
  TApplication theApp("App",&argc,argv);
  
  using namespace std;

  const Int_t iterate = 10000;
  const Double_t md_bar_rms = 660; //ppm
  const Double_t event_rate = 960; //Hz
  
  const Double_t A_PV = -0.234; //ppm
  const Double_t offset_in_bar1  = 0.7; //ppm
  const Double_t offset_in_bar5  = 0.0; //ppm



  const Double_t A_Tm = -4.75; //ppm
  // Now -4.75 is what we measured from the vertical transverse measurement.
  // But according to the simulations, if there is an offset in md1 or md5
  // it changes by
  // md1+0.7 gave p0 = 0.963 and p1 = 0.002
  // md5+0.7 gave p0 = 1.037 and p1 = 0.000
  // so to get what it ought to be I have to take this difference in to consideration,

  //the factor I need to change the measured with to get the actual magnitude
  const Double_t factor = 0.963;  
  Double_t A_T = A_Tm/0.963;

  Double_t mean;
  Double_t error;
  Double_t md_mean[8];
  Double_t md_error[8];
  Double_t md_corrected[8];
  Double_t md_corrected_error[8];
    
  Double_t x[8];
  Double_t x_error[8];
  Double_t angle = 0;
  
  TH1D* correction_p0 = new TH1D("correction_p0","correction_p0",1000, -1.5e-10,1.5e-10);

  TRandom1 * random = new TRandom1();
  
  TCanvas * c1= new TCanvas("c1", "Fitted MD asymmetries",0,0,1000,500);
  TString title = Form("LH2: Simulated Main detector asymmetries  A_m(phi) = %1.2f x (%1.2f Cos(phi) - %1.2f Sin(phi)) + %2.3f x %1.2f",A_T,PV,PH,PL,A_PV);
  
  if(argc==4){
    run_time = atoi(argv[1]);
    PV  = (atoi(argv[2]))*0.01;
    PH  = (atoi(argv[3]))*0.01;
  } 
  else  if(argc<4 || argc>4){
    std::cout<<" Correct usage is ./sim_15_effetc_on_corrections <time in hrs> <P_V> <P_H>"<<std::endl;
    exit(1);
  }
  
  // Calculate the amount of longitudinal polarization
  PL = TMath::Sqrt(1- (PV*PV)- (PH*PH));
  
  Double_t patterns             = run_time*3600*event_rate/4.0;
  Double_t md_bar_stat_error    = md_bar_rms/sqrt(patterns); //ppm
    
  Double_t para_p0=0.0;
  Double_t para_p1=0.0;
  Double_t para_p2=0.0;
  Double_t para_p0_err=0.0;
  Double_t para_p1_err=0.0;
  Double_t para_p2_err=0.0;  

 std::cout<<"Simulating the effect of 1+5 not being zero"<<std::endl;
 std::cout<<"Using A_T  = "<<A_T<<" ppm"<<std::endl;
 std::cout<<"Using A_PV = "<<A_PV<<" ppm"<<std::endl;
 std::cout<<"Statistical error per bar for "<<run_time<<" hour  = "<<md_bar_stat_error<<std::endl;
 std::cout<<"Number of patterns = "<<patterns<<std::endl;
 std::cout<<"Induced horizontal transverse polarization "<<PH*100<<" "<<std::endl;
 std::cout<<"Induced vertical transverse polarization "<<PV*100<<" "<<std::endl;
 printf("Induced longitudinal polarization %f \n",PL);

 TF1*trans_fit =  new TF1("trans_fit","-4.75*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
 random->SetSeed(0);

 /*Extract the longitudinal data with the transverse asymmetry of  A_T=-4.75*/
 for(Int_t k=0;k<iterate;k++){
  
   /*simulate measurement*/
   for(Int_t i=0;i<8;i++){   
     angle =(TMath::Pi()/180)*(45*(i));
     A_m = A_T*(PV*TMath::Cos(angle) - PH*TMath::Sin(angle)) + PL*A_PV; // ppm

     /*Generate a random mean from a gaussian distribution with mean = Am and sigma = md_bar_stat_error*/
     mean = random->Gaus(A_m,md_bar_stat_error); //ppm

     error =  md_bar_stat_error; //ppm

     if(i==0)
       md_mean[i] = mean+offset_in_bar1;
     else if (i==4)
       md_mean[i] = mean+offset_in_bar5;
     else
       md_mean[i] = mean;

     md_error[i] = error;
     x[i] = i+1;
     x_error[i]=0.0;      
   }


   // Fit the simulated octant asymmetries using the measured A_T fit
   TGraphErrors* grp  = new TGraphErrors(8,x,md_mean,x_error,md_error);
   
   grp->Fit("trans_fit");
   TF1 *  fit = grp->GetFunction("trans_fit");
   if(fit==NULL) exit(1);
   
   /*So from the fit, my measured asymmetry changes are*/
   para_p0=(fit->GetParameter(0));
   para_p1=(fit->GetParameter(1));
   para_p2=(fit->GetParameter(2));
   para_p0_err=(fit->GetParError(0));
   para_p1_err=(fit->GetParError(1));
   para_p2_err=(fit->GetParError(2));
   //grp->Draw("APE");

   /*Get the corrections*/
   for(Int_t i=0;i<8;i++){ 
     angle =(TMath::Pi()/180)*(45*(i));  
     md_corrected[i] = (md_mean[i] - (para_p0*TMath::Cos(angle) - para_p1*TMath::Sin(angle) + para_p2)); //ppm
     md_corrected_error[i] = sqrt(md_error[i]*md_error[i] +  TMath::Cos(angle)*TMath::Cos(angle)*para_p0_err*para_p0_err + TMath::Sin(angle)*TMath::Sin(angle)*para_p1_err*para_p1_err + para_p2_err*para_p2_err); //ppm
     if(ldebug) std::cout<<"set value = "<<md_mean[i]<<"+-"<<md_error[i]<< " and from fit= "<<(para_p0*TMath::Cos(angle) - para_p1*TMath::Sin(angle) + para_p2)
			 <<" and the corrected = "<<md_corrected[i]<<"+-"<<md_corrected_error[i]<<std::endl;
   }
   
   // Draw the corrected values and do a constant fit
   TGraphErrors* grp_c  = new TGraphErrors(8,x,md_corrected,x_error,md_corrected_error);
   grp_c->Fit("pol0");
   // grp_c->Draw("AP");
   grp_c->GetFunction("pol0");
   TF1 *  fit_c = grp_c->GetFunction("pol0");
   if(fit_c==NULL) {
     std::cout<<"Fit is empty!\n";
     exit(1);
   }
   correction_p0->Fill((fit_c->GetParameter(0))); //ppm
   if(ldebug) std::cout<<"P0 from constant fit "<<(fit_c->GetParameter(0))<< "+- "<<fit_c->GetParError(0)<<std::endl;
     
 }
 
 gPad->SetLogy();
 correction_p0->Draw();
 correction_p0->Fit("gaus");
 correction_p0->SetTitle(Form("%1.0f_P_V_%i_P_H_%i_md1_%2.2f_md5_%2.2f",run_time,(atoi(argv[2])),(atoi(argv[3])), offset_in_bar1, offset_in_bar5));



 c1->SaveAs(Form("sim_15_effect_on_correction_%1.0f_P_V_%i_P_H_%i_md1_%2.2f_md5_%2.2f.png",run_time,(atoi(argv[2])),(atoi(argv[3])), offset_in_bar1, offset_in_bar5));

 theApp.Run();
 return(1);
}
