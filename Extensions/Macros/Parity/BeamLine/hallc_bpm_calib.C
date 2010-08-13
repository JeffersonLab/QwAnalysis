//
// Author : Jeong Han Lee
// Date   : Thursday, August 12 20:09:51 EDT 2010
//
//      


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <getopt.h>
#include <sstream>
#include <vector>



#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TUnixSystem.h"
#include "TProfile.h"
#include "TLine.h"
#include "TBox.h"

const char* program_name;
TCanvas *Canvas;
Int_t nbin;

TString SAMPLE_SIZE = "468";  //Default. all results are "normalized" to the adc sample size
TString scut;
TString scut_residual;
Double_t scutmin;
Double_t scutmax;
Double_t max_current=50; // (uA) default value. can be set from the command line
Double_t min_current=5;  // (uA) default value. can be set from the command line
TLine *zeroline;
TTree *nt;
TString CanvasTitle;
std::ofstream Myfile;

void calibrate(TString devnam);

// Here calibrate function is defined in "bpm_bcm_calibration.cc (for injector)
// I want to reuse it for Hall C BPM calibrations.
void print_usage(FILE* stream, int exit_code);

Bool_t pol2BCM=kFALSE;
Bool_t pol2=kFALSE;


Int_t
main(int argc, char **argv)
{
 
  TApplication theApp("App", &argc, argv);
  char* run_number = NULL;
 
  Double_t fit_range[2] = {0.0};


  Bool_t file_flag = false;
  Bool_t fit_range_flag = false;
   
  program_name = argv[0];

  int cc = 0; 

  while ( (cc= getopt(argc, argv, "r:e:")) != -1)
    switch (cc)
      {
      case 'r':
	{
	  file_flag = true;
	  run_number = optarg;
	}
	break;
      case 'e':
	{
	  fit_range_flag = true;
	  char *c;
	  /*
	   * Allow the specification of alterations
	   * to the pty search range.  It is legal to
	   * specify only one, and not change the
	   * other from its default.
	   */
	  c = strchr(optarg, ':');
	  if (c) {
	    *c++ = '\0';
	    fit_range[1] = atof(c);
	  }
	  if (*optarg != '\0') {
	    fit_range[0] = atof(optarg);
	  }
	  if ((fit_range[0] > fit_range[1]) || (fit_range[0] < 0) || (fit_range[1] > 100.0) ) 
	    {
	      print_usage(stdout,0);
	    }
	}
	break;
      case '?':
	{
	  if (optopt == 'e') 
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'r')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr, "Unknown option character `\\x%x'.\n",  optopt);
	  print_usage(stdout,0);
	  return 1;
	}
	break;
      default:
	abort () ;
      }
  
  if(not fit_range_flag) {
    fit_range[0] = 0.10;
    fit_range[1] = 0.85;
  }
  else {
    fit_range[0] = fit_range[0]/100.0;
    fit_range[1] = fit_range[1]/100.0;
    
  }
 
  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }

  theApp.Run();
  return 0;
}




void calibrate(TString devname) 
{  
  TString plotcommand[4];

  TString antenna[4]={"XP","XM","YP","YM"};
  Double_t par[3][4];
  Double_t epar[3][4];

  TF1 *f[4];
  TProfile *hprof[4];

  Canvas->Clear();
  Canvas->Divide(4,2);

  // check to see if we have the device on the tree
     
  if((nt->FindBranch(Form("%sWSum",devname.Data()))) == NULL)
    {
      std::cout<<" Attempt to access non exsisting device "<< devname<<". I am Skippineg this!"<<std::endl;
      return;
    }
  else 
    {
      for(int i=0;i<4;i++)
	{		
	  
	  Canvas->cd(i*2+1);
	  TString histname=Form("prof_%s%s",devname.Data(),antenna[i].Data());
	  hprof[i]= new TProfile(histname, histname, nbin,0, max_current*1.1);
	  
	  SAMPLE_SIZE = Form("%s%s.num_samples",devname.Data(),antenna[i].Data());
	  
	  plotcommand[i] = Form("%s%s.hw_sum_raw/%s:current>>%s",
				devname.Data(),antenna[i].Data(),SAMPLE_SIZE.Data(),histname.Data()); 
	  
	  scut = Form("cleandata==1 && current>%f && current<%f",scutmin, scutmax);
	  
	  
	  nt->Draw(plotcommand[i],scut);
	  
	  if( hprof[i] == NULL)
	    {
	      std::cout<<" Attempt to plot NULL histogram - "<<plotcommand<<"\n Skipping device."<< devname<<std::endl;
	      return;
	    }
	  
	  hprof[i]->SetMarkerStyle(20);
	  hprof[i]->SetMarkerColor(2);
	  for(int jj=0;jj<3;jj++)
	    {
	      par[jj][i]=0;
	      epar[jj][i]=0;
	    }
	  if(pol2)
	    {
	      hprof[i]->Fit("pol2","Q");	  
	      f[i]= hprof[i]->GetFunction("pol2");
	      for(int jj=0;jj<3;jj++)
		{
		  par[jj][i]= f[i]->GetParameter(jj);
		  epar[jj][i]= f[i]->GetParError(jj);
		}
	    }
	  else
	    {
	      hprof[i]->Fit("pol1","Q");	  
	      f[i]= hprof[i]->GetFunction("pol1");
	      for(int jj=0;jj<2;jj++)
		{
		  par[jj][i]= f[i]->GetParameter(jj);
		  epar[jj][i]= f[i]->GetParError(jj);
		}
	      
	    }
	  
	  TString ytitle0 =  Form("%s%s.hw_sum_raw/%s",devname.Data(),antenna[i].Data(),SAMPLE_SIZE.Data());  
	  TString xtitle0  = Form("cleandata==1&& I>%f && I<%f",scutmin, scutmax);
	  
	  hprof[i]->SetYTitle(ytitle0);
	  hprof[i]->SetXTitle(xtitle0);
	  
	  Myfile <<devname <<antenna[i]<< " , " << par[0][i] <<" , " << "1.0" <<std::endl;    
	}
      
      //    little trick for nice plot
      Double_t Yminres=kMaxInt*1.;
      Double_t Ymaxres=kMinInt*1.;
      TH1 *h1;
	  
      for(int i=0;i<4;i++)
	{
	  Canvas->cd(2*(i+1));

	  nt->SetAlias("sample_size",SAMPLE_SIZE);
		  	  
	  plotcommand[i] = devname.Data(); 
	  plotcommand[i] +=antenna[i];
	  plotcommand[i] +=".hw_sum_raw/";
	  plotcommand[i] +="sample_size";
	  plotcommand[i] +=" -(";
	  plotcommand[i]+="(current*current*";
	  plotcommand[i] += par[2][i];
	  plotcommand[i] += ")+";
	  plotcommand[i]+="(current*";
	  plotcommand[i] += par[1][i];
	  plotcommand[i] += ")+";
	  plotcommand[i] +=par[0][i];
	  plotcommand[i] +=")";
	  
	  // To draw these plots we will use the current projected by the bcms in the initial bcm calibration.
	  nt->Draw(plotcommand[i],scut_residual);	 
	  h1 = (TH1*) gROOT->FindObject("htemp");
	  
	  if(h1 == NULL)
	    {
	      std::cout<<" Attempt to plot NULL histogram - "<<plotcommand<<"\n Exiting program."<<std::endl;
	      exit(1);
	    }
	  
	  if (h1->GetXaxis()->GetXmax() > Ymaxres) Ymaxres = h1->GetXaxis()->GetXmax();
	  if (h1->GetXaxis()->GetXmin() < Yminres) Yminres = h1->GetXaxis()->GetXmin();
	  h1->Delete();
	}
      std::cout<<" onto plotting the residuals \n";
      
      //and now the nice plot
      
      TH2D *h2[4][2];
      for(int i=0;i<4;i++)
	{
	  Canvas->cd(2*(i+1));
	  Int_t reducednbin=500;
	  
	  //Draw the responce of the bpms
	  TString histname=Form("Res_%s%s",devname.Data(),antenna[i].Data());
	  h2[i][0]=new TH2D(histname,histname,reducednbin,0, max_current*1.1,reducednbin,Yminres,Ymaxres);
	  nt->Draw(plotcommand[i]+":current>>"+histname,scut_residual);

	  h2[i][0]->Draw("box");
	  
	  // Draw the restricted responce of the bpms due to variation in beam current
	  histname=Form("Res_%s%s_restricted",devname.Data(),antenna[i].Data());
	  h2[i][1]=new TH2D(histname,histname,reducednbin,0, max_current*1.1,reducednbin,Yminres,Ymaxres); 
	  nt->Draw(plotcommand[i]+":current>>"+histname,scut);
      
	  h2[i][1]->SetLineColor(kRed);
	  h2[i][1]->Draw("boxsame");
	  
	  TString xtitle1  = Form("cleandata==1&& I>%10.1f",scutmin);
	  
	  h2[i][1]->SetYTitle(plotcommand[i]);
	  h2[i][1]->SetXTitle(xtitle1);
	  
	  zeroline->Draw("same");
	  
	  std::cout << devname <<antenna[i]<< 
	    " par[0]="<< Form("%5.0f",par[0][i])<<
	    " par[1]="<<Form("%5.0f",par[1][i])<< 
	    " par[2]="<< Form("%5.2f",par[2][i])<<
	    " red chi2 = "<<Form("%5.0f",(f[i]->GetChisquare())/(f[i]->GetNDF()))<<
	    " residual (mean, res)= ("<<Form("%5.0f",h2[i][1]->GetMean(2))<<
	    " , "<<Form("%5.0f",h2[i][1]->GetRMS(2))<<
	    " )\n";
	  
	}
    }
  Canvas->Update();

  Canvas->Print(CanvasTitle+".ps"); // the ) puts all the plots in to a one ps file
  return;
  
};



void 
print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "\n");
  fprintf (stream, "This program is used to do BPM calibrations.\n");
  fprintf (stream, "Usage: %s -r {run number} -i {n} -e {a:b} \n", program_name);
  fprintf (stream, 
	   " -r run number.\n"
	   " -i unser beam off offset (n*1e3) \n"
	   " -e fit percent range (default [0.01*a*unser_current_range, 0.01*b*unser_current_range]\n"
	   );
  fprintf (stream, "\n");
  exit (exit_code);
}
