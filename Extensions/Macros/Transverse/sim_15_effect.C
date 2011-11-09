//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : October 10th, 2011
//*****************************************************************************************************//
/*

This macro simulates the effect of the non-zero offest seen in bar 1+5 for 10000 measurements 
with each measurement being <time in hours> hours.

Required inputs : time in hours, P_V % , P_H %

<time_in_hours> Length of each measurement in hours.
P_V% = amount of vertical transverse polarization
P_H% = amount of horizontal transverse polarization


e.g .To simulate 10000 of 8 hour long measurements with P_V = 100% and P_H = 0 % do
 
/sim_15_effect 8 100 0  

For each measurement, the asymmetry on each bar is simulated using a gaussian random generator with 
a mean A_m = A_T*(PV*TMath::Cos(angle) - PH*TMath::Sin(angle)) + PL*A_PV
and a sigma = 660/sqrt(<time_in_ours> * 3600 * 960 /4)
For bar 1 and 5, offests can be added by changing the variables

 const Double_t offset_in_bar1  (ppm)
 const Double_t offset_in_bar5  (ppm)

The output of the script is two distributions of P_V% and P_H% each fitted with a gaussian fit.
The plots are saved as * png format.
e.g.
sim_15_effect_8_P_V_0_P_H_0_md1_0.0_md5_0.0.png
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
 const Double_t A_T  = -4.75; //ppm

 // Simulate the precision of the offset
 TRandom1 * random1 = new TRandom1();
 random1->SetSeed(0);

 Double_t offset_in_bar1=0.0;//ppm
 Double_t offset_in_bar5=0.0; //ppm
 Double_t offset_error = 0.24; //ppm

 Double_t mean;
 Double_t error;
 Double_t md_mean[8];
 Double_t md_error[8];
 Double_t x[8];
 Double_t x_error[8];
 Double_t angle = 0;


 TRandom1 * random = new TRandom1();
 random->SetSeed(0);

 TCanvas * c1= new TCanvas("c1", "Fitted MD asymmetries",0,0,1000,500);
 TString title = Form("LH2: Simulated Main detector asymmetries  A_m(phi) = %1.2f x (%1.2f Cos(phi) - %1.2f Sin(phi)) + %2.3f x %1.2f",A_T,PV,PH,PL,A_PV);

 if(argc==4){
   run_time = atoi(argv[1]);
   PV  = (atoi(argv[2]))*0.01;
   PH  = (atoi(argv[3]))*0.01;
 } 
 else  if(argc<4 || argc>4){
   std::cout<<" Correct usage is ./sim_15_effetc <time in hrs> <P_V> <P_H>"<<std::endl;
   exit(1);
  }

 PL = TMath::Sqrt(1- (PV*PV)- (PH*PH));

 Double_t patterns             = run_time*3600*event_rate/4.0;
 Double_t md_bar_stat_error    = md_bar_rms/sqrt(patterns);
 
 // to set histogram x axis limits
 Double_t p0_u = PV+0.3;
 Double_t p0_l = PV-0.3;
 Double_t p1_u = PH+0.3;
 Double_t p1_l = PH-0.3;

 TH1D* para_p0 = new TH1D("p0","% P_V",1000, p0_l,p0_u);
 TH1D* para_p1 = new TH1D("p1","% P_H",1000, p1_l,p1_u);

 std::cout<<"Simulating the effect of 1+5 not being zero"<<std::endl;
 std::cout<<"Using A_T  = "<<A_T<<" ppm"<<std::endl;
 std::cout<<"Using A_PV = "<<A_PV<<" ppm"<<std::endl;
 std::cout<<"Statistical error per bar for "<<run_time<<" hour  = "<<md_bar_stat_error<<std::endl;
 std::cout<<"Number of patterns = "<<patterns<<std::endl;
 std::cout<<"Induced horizontal transverse polarization "<<PH*100<<" "<<std::endl;
 std::cout<<"Induced vertical transverse polarization "<<PV*100<<" "<<std::endl;
 printf("Induced longitudinal polarization %f \n",PL);

 TF1*trans_fit =  new TF1("trans_fit","-4.75*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);


 /*Fill the asymmetry histograms for iterate number of measurements*/
 for(Int_t k=0;k<iterate;k++){
  
   offset_in_bar5=random1->Gaus(0.7,offset_error);
   offset_in_bar1=0.0; //ppm

   /*On measurement*/
   for(Int_t i=0;i<8;i++){   
     angle =(TMath::Pi()/180)*(45*(i));
     A_m = A_T*(PV*TMath::Cos(angle) - PH*TMath::Sin(angle)) + PL*A_PV;

     /*Add the offset to md1*/
     mean = random->Gaus(A_m,md_bar_stat_error);
     error =  md_bar_stat_error;
 
     if(i==0){
       md_mean[i] = mean+offset_in_bar1;
       //md_error[i] = TMath::Sqrt(md_bar_stat_error*md_bar_stat_error+offset_error*offset_error);
     }
     else if (i==4){
       md_mean[i] = mean+offset_in_bar5;
       // md_error[i] = TMath::Sqrt(md_bar_stat_error*md_bar_stat_error+offset_error*offset_error);
     }
     else
       md_mean[i] = mean;
     
     md_error[i] = error;
       
     x[i] = i+1;
     x_error[i]=0.0; 
   }

   TGraph* grp  = new TGraph(8,x,md_mean);

   grp->Fit("trans_fit");
   TF1 *  fit = grp->GetFunction("trans_fit");
   if(fit==NULL) exit(1);
   para_p0->Fill((fit->GetParameter(0)));
   para_p1->Fill((fit->GetParameter(1)));
   if((k%100) == 0) std::cout<<k<<std::endl;

 }
 


 c1->Divide(1,2);
 c1->cd(1);
 para_p0->Draw();
 para_p0->Fit("gaus");
 c1->cd(2);
 para_p1->Draw();
 para_p1->Fit("gaus");


 c1->SaveAs(Form("sim_15_effect_%1.0f_P_V_%i_P_H_%i_md1_%2.2f_md5_%2.2f.png",run_time,(atoi(argv[2])),(atoi(argv[3])), offset_in_bar1, offset_in_bar5));

 theApp.Run();
 return(1);
}
