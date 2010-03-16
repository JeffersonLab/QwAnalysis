//********************************************************************//
// Author : J. Roche, B. Waidyawansa
// Date   : March 10, 2009
//********************************************************************//
//
// This macro determines the BPM and BCM pedestals. It writes the pedestals
// to a text file and saves the plots in to a .ps file.
//
// To compile this code do a gmake.
// To use the exe file from comman prompt type:
// ./pedestal_extractor runnumber mincurrent maxcurrent samplesize 
//
//  NOTE:  offset and gain are in units of counts/sample.
//         Multiply by SAMPLE_SIZE to put them back 
//         into "normal" Qweak parameter units, if you wish.
//  NOTE:  gain will convert BCM/SAMPLE_SIZE into the current units
//         scandata2 was expressed in (probably uA).
//         BCM*gain/SAMPLE_SIZE will have units of nC (if scandata 
//         was in uA).


#include <vector>
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>



Bool_t pol2BCM=kFALSE;
Bool_t pol2=kFALSE;
Double_t currentstep=0.5e-3;// this is a pretty important parameter 
// it defines the width of the bin for the profile histograms. The profile histograms
// are fit to find pedestal and slope. Make this currentstep to small and you can
// get pretty wrong results. The best way to make sure that this step is small enough
// is to check the stability of the results for an evem smaller step...
Int_t nbin;
TString directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration
//TString directory="~/Desktop/"; // the location of the rootfile used for calibration
TString SAMPLE_SIZE = "468";  //all results are "normalized" to the adc sample size
TString scut;
TString scut_residual;
Double_t scutmin;

Int_t BCMOfChoice=21; //this is the BCM against everyone elese is going to be calibrated
                      //input the number of the device in the devicelist
                      // remember the first device in the list has number 0

// List of bpm/bcm in the injector beamline.
// tO calibrate other types of devices, comment oout this list and create a list simmiler to this.
//  DO NOT remove this list.
TString devicelist[22]=
  {"qwk_1i02","qwk_1i04","qwk_0l01","qwk_1i06","qwk_0i02",
   "qwk_0i02a","qwk_0i05","qwk_0i07","qwk_0l02","qwk_0l03",
   "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
   "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
   "qwk_0r06","qwk_bcm0l02"};



Double_t max_current=50; // (uA) default value. can be set from the command line
Double_t min_current=5;  // (uA) default value. can be set from the command line
TLine *zeroline;
TTree *nt;
TCanvas *Canvas;
TString CanvasTitle;


std::ofstream Myfile;
void calibration(TString devnam);
void initial_bcm_calibration(TString bcm_titlSSyme);

int main(Int_t argc,Char_t* argv[])
{


  Char_t filename[200];
  Char_t textfile[100];
  TString device="";
  TString runnum;
  TFile *f;

 if(argc<2 || argc>5)
    {
      std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
      std::cerr<<" ./pedestal_extractor  runnumber mincurrent maxcurrent samplesize \n";
      std::cerr<<" mincurrent and maxcurrent define the range over which you want to perform the calibration.\n";
      std::cerr<<" the samplesize is the adc triumf sample size, the extracted pedestals are normalized by this value \n so they can be used correctely in QwAnalysis\n";

      std::cerr<<" You NEED to give a run number but all other values are pre-defined by default. You can change by using the full prompt \n";
      std::cerr<<" The default values are:\n";
      std::cerr<<" mincurrent ="<<min_current<<"\n";
      std::cerr<<" maxcurrent"<<max_current<<"\n";
      //  std::cerr<<" sample size="<< SAMPLE_SIZE<<"\n";
      exit(1);
    }
  else if (argc == 2)
    runnum=argv[1];
  else if (argc == 3)
    {
      runnum=argv[1];
      min_current=atof(argv[2]);
    }
  else if (argc == 4)
    {
      runnum=argv[1];
      min_current=atof(argv[2]);
      max_current=atof(argv[3]);
    }
  else if (argc == 5)
    {
      runnum=argv[1];
      min_current=atof(argv[2]);
      max_current=atof(argv[3]);
      //  SAMPLE_SIZE=atof(argv[4]);
    }
  
  gStyle->SetOptFit(1111);
  TApplication theApp("App",&argc,argv);
  gStyle->SetOptStat(0000000);

  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetTitleYOffset(1.8);
  gStyle->SetTitleYOffset(2.1);



   
  
  //Get the root file
  sprintf(filename,"%sQweak_BeamLine_%d.root",directory.Data(),atoi(runnum));
  f = new TFile(filename);
  if(!f->IsOpen())
      return 0;

  std::cout<<"Obtaining data from: "<<filename<<"\n";

    
  nt = (TTree*)f->Get("MPS_Tree"); //load the MPS_Tree

  //Open a text file to store results
  if(pol2)
    sprintf(textfile,"BeamlineCalibration%d_%2.0f_%2.0f_quadratic.txt",atoi(runnum),min_current,max_current); 
  else
    sprintf(textfile,"BeamlineCalibration%d_%2.0f_%2.0f_linear.txt",atoi(runnum),min_current,max_current); 
  if(pol2&&pol2BCM)
    sprintf(textfile,"BeamlineCalibration%d_%2.0f_%2.0f_quadquad.txt",atoi(runnum),min_current,max_current); 
    
  Myfile.open(textfile);
  Myfile <<"! Pedestal file of run  "<<runnum<<std::endl;
  // Myfile <<"! adc sample size  "<<SAMPLE_SIZE<<std::endl;
  //std::cout <<"! adc sample size  "<<SAMPLE_SIZE<<std::endl;
  Myfile <<"! curent range in calibration  "<<min_current<<" to "<<max_current<<std::endl;
  std::cout <<"! curent range in calibration  "<<min_current<<" to "<<max_current<<std::endl;
  if(pol2)
    {
      Myfile<<"! fit is  quadratic\n";
      std::cout<<"! fit is  quadratic \n";
    }
  else
    {
      Myfile<<"! fit is linear \n";
      std::cout<<"! fit is linear \n";
    }

  time_t theTime;
  time(&theTime);
  Myfile <<"! date of analysis ="<<ctime(&theTime)<<"\n";
  std::cout <<"! date of analysis ="<<ctime(&theTime)<<"\n";

  if(pol2)
    CanvasTitle="calibration_"+runnum+Form("_%2.0f_%2.0f",min_current,max_current)+"quadratic";
  else
    CanvasTitle="calibration_"+runnum+Form("_%2.0f_%2.0f",min_current,max_current)+"linear";
  if(pol2&&pol2BCM)
    CanvasTitle="calibration_"+runnum+Form("_%2.0f_%2.0f",min_current,max_current)+"quadquad";

  std::cout<<" canvas title ="<<CanvasTitle<<std::endl;

  Canvas= new TCanvas(CanvasTitle, CanvasTitle,0,0,1200,800);  
  

  initial_bcm_calibration(devicelist[BCMOfChoice]);
 
  for (Int_t dof = 0; dof < 22 ; dof++)
    {
      std::cout<<" onto calibrating "<<devicelist[dof]<<"\n";
      if(dof!=BCMOfChoice) 
	calibration(devicelist[dof]);
    }

  Myfile.close();
  // Canvas->Clear();
  Canvas->Print(CanvasTitle+".ps)");
  std::cout<<" done with calibration \n";

  theApp.Run();
  return(0);

};


//***************************************************
//***************************************************
//         Initial Calibration of one of the BCMs                        
//***************************************************
//***************************************************
void initial_bcm_calibration(TString bcm_title)
{	  

  SAMPLE_SIZE = Form("%s.num_samples",bcm_title.Data());
  
  std::cout << "Find the offset and gain for " << bcm_title.Data()
	    << "/"<<SAMPLE_SIZE.Data()<< std::endl;

  Canvas->Clear();
  Canvas->Divide(1,2);
  Canvas->cd(1);
  scut = Form("cleandata==1&&scandata2>%f && scandata2<%f && scandata2!=8",
	      -0.5+min_current, 0.5+max_current);

  nt->Draw("scandata2","cleandata==1");
  std::cout<<"herer"<<std::endl;

  TH1 *hscandata=(TH1*)gROOT->FindObject("htemp");

  scutmin=-0.5+min_current;
  for(int i=1;i<101;i++)
    if(hscandata->GetBinContent(i)!=0)
      {
	scutmin=hscandata->GetBinCenter(i)-hscandata->GetBinWidth(i);
	i=150;
      }
  std::cout<<"minimum scandata 2="<<scutmin<<"\n";
  scut_residual="cleandata==1 && scandata2!=8 && current>";
  scut_residual+=Form("%10.1f",(scutmin-0.5));
  // to get rid of beam trip

  nbin=(Int_t) (max_current+0.5)/currentstep;

  TString histname="prof_"+bcm_title;
  TProfile *hprofinit= new TProfile(histname, histname, nbin,0, max_current*1.1);

  TString plotcommand = Form("%s.hw_sum_raw/%s:scandata2>>%s && scandata2!=8",
			     bcm_title.Data(),SAMPLE_SIZE.Data(),histname.Data());       
  nt->Draw(plotcommand,scut,"hprof");

  TString xtitle = Form("scandata2>>%s",histname.Data()); 
  
  TString ytitle = Form("%s.hw_sum_raw/%s",
			     bcm_title.Data(),SAMPLE_SIZE.Data()); 
      
  hprofinit->SetXTitle(xtitle);
  hprofinit->SetYTitle(ytitle);
  

  hprofinit->SetMarkerStyle(20);
  hprofinit->SetMarkerColor(2);

  TF1 *f1;
  TString setalias;
  Double_t offset;
  Double_t gain;

  if(pol2BCM)
    {
      hprofinit->Fit("pol2","Q");
      f1 = hprofinit->GetFunction("pol2");
      Double_t a=f1->GetParameter(2);
      Double_t b=f1->GetParameter(1);
      Double_t c=f1->GetParameter(0);

      Double_t y=hprofinit->GetMean(2);
      Double_t branch=(-1*b+sqrt(b*b-4*a*(c-y)))/(2*a);
      TString varname=Form("%s.hw_sum_raw/%s ", bcm_title.Data(),SAMPLE_SIZE.Data());
      if(branch>0)
	setalias=Form("(-1*%f+sqrt(%f*%f-4*%f*(%f-%s)))/(2*%f)",
		      b,b,b,a,c,varname.Data(),a);
      else
	setalias=Form("(-1*%f-sqrt(%f*%f-4*%f*(%f-%s)))/(2*%f)",
		      b,b,b,a,c,varname.Data(),a);
      offset=c;
      gain=b;
    }
  else
    {
      hprofinit->Fit("pol1","Q");
      f1 = hprofinit->GetFunction("pol1");
      offset = (f1->GetParameter(0));
      gain   = 1./(f1->GetParameter(1));
      if(offset>0)
	setalias=Form("((%s.hw_sum_raw/%s - %f)*%f)", bcm_title.Data(),SAMPLE_SIZE.Data(), offset, gain);
      else
	setalias=Form("((%s.hw_sum_raw/%s + %f)*%f)", bcm_title.Data(),SAMPLE_SIZE.Data(), -1*offset, gain);
    }
  //  setalias="scandata2";
  nt->SetAlias("current",setalias);
  
  std::cout<<" alias current is defined as:"<<setalias<<"\n";
  
  f1->SetLineWidth(1);
  f1->SetLineColor(1);
  //  Canvas[ncanvas]->cd(2);
  Canvas->cd(2);


  plotcommand="current-scandata2:scandata2";       
  nt->Draw(plotcommand,scut_residual,"box");
  nt->SetMarkerColor(kRed);
  nt->SetLineColor(kRed);
  nt->Draw(plotcommand,scut,"boxsame");
  nt->SetMarkerColor(kBlack);
  TH1 *g2=nt->GetHistogram();

  std::cout << "BCM\t" << bcm_title.Data() << "\t offset=" 
 	    << offset << "\t Gain=" 
 	    << gain<<"\t red X2 = "<<(f1->GetChisquare())/(f1->GetNDF())
 	    <<" residual (mean,rms) = ("<<Form("%5.2f",g2->GetMean(2))
 	    <<" , "<<Form("%5.2f",g2->GetRMS(2))<<") \n";

  zeroline=new TLine(0,0,max_current,0);
  zeroline->Draw("same");
    
  Canvas->Update();  
  Canvas->Print(CanvasTitle+".ps(");

  Myfile <<bcm_title.Data() << " , " 
      << offset << " , " 
      << gain<<"\n";

};

//***************************************************
//***************************************************
//         Calibrating BPMs and BCMs                       
//***************************************************
//***************************************************

void calibration(TString devname) 
{  
  TString plotcommand[4];

  TString antenna[4]={"XP","XM","YP","YM"};
  Double_t par[3][4];
  Double_t epar[3][4];

  TF1 *f[4];
  TProfile *hprof[4];
  TProfile *hres1[4];
  TProfile *hres2[4];
  TH2F *h2res1[4];
  TH2F *h2res2[4];

  scut = Form("cleandata==1 && current>%f && current<%f",min_current, max_current);


  Canvas->Clear();
  Canvas->Divide(4,2);
  
     
  for(int i=0;i<4;i++)
    {		
     

      Canvas->cd(i*2+1);
      TString histname=Form("prof_%s%s",devname.Data(),antenna[i].Data());
      hprof[i]= new TProfile(histname, histname, nbin,0, max_current*1.1);

      SAMPLE_SIZE = Form("%s%s.num_samples",devname.Data(),antenna[i].Data());

      plotcommand[i] = Form("%s%s.hw_sum_raw/%s:current",
			 devname.Data(),antenna[i].Data(),SAMPLE_SIZE.Data());       
      plotcommand[i]+=">>";
      plotcommand[i]+=histname;

      nt->Draw(plotcommand[i],scut);
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

      TString ytitle0 =  Form("%s%s.hw_sum_raw/%s",
			      devname.Data(),antenna[i].Data(),SAMPLE_SIZE.Data());  
      TString xtitle0  = Form("cleandata==1&& I>%f && I<%f && scandata2!=8",
		  -0.5+min_current, 0.5+max_current);
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
      //      Canvas[ncanvas]->cd(2*(i+1));
      Canvas->cd(2*(i+1));
      
      SAMPLE_SIZE = Form("%s%s.num_samples",devname.Data(),antenna[i].Data());
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
      nt->Draw(plotcommand[i],scut_residual);	 
      h1 = (TH1*) gROOT->FindObject("htemp");
      if(h1->GetXaxis()->GetXmax()>Ymaxres)Ymaxres=h1->GetXaxis()->GetXmax();
      if(h1->GetXaxis()->GetXmin()<Yminres)Yminres=h1->GetXaxis()->GetXmin();
      h1->Clear();
    }
 std::cout<<" onto plotting the residuals \n";
  // and now the nice plot

 TH2F *h2[4][2];
  for(int i=0;i<4;i++)
    {
      Canvas->cd(2*(i+1));
      Int_t reducednbin=500;

      TString histname=Form("Res_%s%s",devname.Data(),antenna[i].Data());
      h2[i][0]=new TH2F(histname,histname,reducednbin,0, max_current*1.1,reducednbin,Yminres,Ymaxres);
      nt->Draw(plotcommand[i]+":current>>"+histname,scut_residual);
      h2[i][0]->Draw("box");



      histname=Form("Res_%s%s_restricted",devname.Data(),antenna[i].Data());
      h2[i][1]=new TH2F(histname,histname,reducednbin,0, max_current*1.1,reducednbin,Yminres,Ymaxres); 
      nt->Draw(plotcommand[i]+":current>>"+histname,scut);
      h2[i][1]->SetLineColor(kRed);
      h2[i][1]->Draw("boxsame");

      TString xtitle1  = Form("cleandata==1&& I>%10.1f",(scutmin-0.5));
	       
  
      h2[i][1]->SetYTitle(plotcommand[i]);
      h2[i][1]->SetXTitle(xtitle1);
 



      zeroline->Draw("same");

      std::cout << devname <<antenna[i]<< 
	" par[0]="<< Form("%5.0f",par[0][i])<<
	" par[1]="<<Form("%5.0f",par[1][i])<< 
	" par[2]="<< Form("%5.2f",par[2][i])<<
	" red X2 = "<<Form("%5.0f",(f[i]->GetChisquare())/(f[i]->GetNDF()))<<
	" residual (mean, res)= ("<<Form("%5.0f",h2[i][1]->GetMean(2))<<
 	" , "<<Form("%5.0f",h2[i][1]->GetRMS(2))<<
	" )\n";
 

    }
  Canvas->Print(CanvasTitle+".ps");
  return;
 
};


