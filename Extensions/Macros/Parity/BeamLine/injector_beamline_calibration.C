//********************************************************************//
// Author : P. King, J. Roche, B. Waidyawansa (buddhini@jlab.org)
// Date   : March 10, 2009
//********************************************************************//
//
// This macro determines the BPM and BCM pedestals. It writes the pedestals
// to a text file and saves the plots in to a .ps file.
//
// To compile this code do a gmake.
// To use the exe file from command prompt type:
// ./pedestal_extractor runnumber  mincurrent maxcurrent variable(optional)
// 
//  variable - deafault is scandata1
//
//  NOTE:  offset and gain are in units of counts/sample.
//         Multiply by SAMPLE_SIZE to put them bhaack 
//         into "normal" Qweak parameter units, if you wish.
//  NOTE:  gain will convert BCM/SAMPLE_SIZE into the current units
//         scandata2 was expressed in (probably uA).
//         BCM*gain/SAMPLE_SIZE will have units of nC (if scandata 
//         was in uA).

#include <cstdlib>

#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <stdexcept>
#include <time.h>


#include "TMath.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TApplication.h"


#include "TH2F.h"
#include "TCanvas.h"
#include "TTree.h"

#include "TF1.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"
#include "TProfile.h"
#include "TString.h"
#include "TLine.h"
#include "TBox.h"


Bool_t pol2BCM=kFALSE;
Bool_t pol2=kFALSE;
Double_t currentstep=0.5e-3;// this is a pretty important parameter 
// it defines the width of the bin for the profile histograms. The profile histograms
// are fit to find pedestal and slope. Make this currentstep to0 small and you can
// get pretty wrong results. The best way to make sure that this step is small enough
// is to check the stability of the results for an even smaller step...
Int_t nbin;
TString directory="$QW_ROOTFILES/"; // the location of the rootfile used for calibration

TString  SAMPLE_SIZE = "468";  //Default. all results are "normalized" to the adc sample size
TString  SCANDATA = "scandata1/1000";
TString  scut;
TString  scut_residual;
Double_t scutmin;
Double_t scutmax;

const Int_t ndevices = 26;
Int_t select_devices = 7;
Int_t BCMOfChoice=0; //this is the BCM against everyone elese is going to be calibrated
                      //input the number of the device in the devicelist
                      // remember the first device in the list has number 0

//  List of bpm/bcm in the injector beamline.
//  tO calibrate other types of devices, comment out this list and create a list simmiler to this.
//  DO NOT remove this list.

TString devicelist[ndevices]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05","qwk_0i07","qwk_0l01","qwk_0l02",
   "qwk_bcm0l02", // BCM
   "qwk_0l03","qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07",
   "qwk_0l08","qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02",
   "qwk_0r03","qwk_0r04","qwk_0r05","qwk_0r06","qwk_0r07",
   "qwk_1IL02"};



Double_t max_current=50; // (uA) default value. can be set from the command line
Double_t min_current=5;  // (uA) default value. can be set from the command line
TLine *zeroline;
TTree *nt;
TCanvas *Canvas;
TString CanvasTitle;


std::ofstream Myfile;
void calibrate(TString devnam);
void initial_bcm_calibration(TString bcm_titlSSyme);

int main(Int_t argc,Char_t* argv[])
{
  Char_t  filename[200];
  Char_t  textfile[100];
  TString device="";
  TString runnum;
  TFile *f;

 if(argc<2 || argc>5)
    {
      std::cerr<<"!!  Not enough arguments to run this code, the correct syntax is \n";
      std::cerr<<" ./pedestal_extractor  runnumber mincurrent maxcurrent scandata_normalization\n";
      std::cerr<<" mincurrent and maxcurrent define the range over which you want to perform the calibration.\n";
      std::cerr<<"scandata_normalization is the value you want to normalize scandata with to get the correct magnitued of current."<<"\n";
      std::cerr<<" You NEED to give a run number but all other values are pre-defined by default. You can change by using the full prompt \n";
      std::cerr<<" The default values are:\n";
      std::cerr<<" mincurrent ="<<min_current<<"\n";
      std::cerr<<" maxcurrent"<<max_current<<"\n";
      std::cerr<<" scandata_normalization = 1000"<<"\n";
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
      SCANDATA=Form("scandata/%s",argv[4]);
    }

  TApplication theApp("App",&argc,argv);

  // Fit and stat parameters
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.35);
  gStyle->SetStatY(0.5);
  gStyle->SetStatX(0.9);     
  gStyle->SetOptStat(0); 
 
  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(0);

  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);

  // histo parameters
  gStyle->SetTitleYOffset(1.2);
  gStyle->SetTitleXOffset(1.8);
  //gStyle->SetTitleX(0.1);
  //tyle->SetTitleW(0.9);
  //tyle->SetTitleSize(0.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gROOT->ForceStyle();

  
  //Get the root file
  sprintf(filename,"%sqwinjector_%d.000.root",directory.Data(),atoi(runnum));
  f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";


  //load the MPS_Tree. It has data based on individul events
  nt = (TTree*)f->Get("Mps_Tree"); 
  nt->SetAlias("scandata",SCANDATA);

  //Open a text file to store results
  if(pol2)
    sprintf(textfile,"InjectorBPMCalibration_%d_%2.0f_%2.0f_quadratic.txt",atoi(runnum),min_current,max_current); 
  else
    sprintf(textfile,"InjectorBPMCalibration_%d_%2.0f_%2.0f_linear.txt",atoi(runnum),min_current,max_current); 
  if(pol2&&pol2BCM)
    sprintf(textfile,"InjectorBPMCalibration_%d_%2.0f_%2.0f_quadquad.txt",atoi(runnum),min_current,max_current);    
 

  Myfile.open(textfile);
  Myfile <<"! Pedestal file of run  "<<runnum<<std::endl;
  Myfile <<"! adc sample size  "<<SAMPLE_SIZE<<std::endl;
  std::cout <<"! adc sample size  "<<SAMPLE_SIZE<<std::endl;
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


  Canvas= new TCanvas(CanvasTitle, CanvasTitle,0,0,1000,600);  
  Canvas->SetFillColor(10);

  // Calibrate the bcm: 
  // This is needed because the current that we set via scandata and the current that we read in via the bcm are not fixed.
  // e.g. when we set scandata = 10, the bcm will read a current range that may vary from 9.3 to 10.8 with the average being
  // 10.05 != 10. So we have to calibrate the bcm before we use it to calibrate the bpms.


  std::cout<<"Calibrating bcm "<<devicelist[BCMOfChoice]<<std::endl;
  initial_bcm_calibration(devicelist[BCMOfChoice]);

 


  for (Int_t dof = 0; dof < select_devices-1 ; dof++)
    {
      std::cout<<" onto calibrating "<<devicelist[dof]<<"\n";
      if(devicelist[dof] != "qwk_bcm0l02") //skip calibrating the bcm.
	calibrate(devicelist[dof]);
    }

  Myfile.close();
 
 // Print the bpm calibration polts on to pdf file
  Canvas->Print(CanvasTitle+".ps)");// the ) puts all the plots in to a one ps file
  std::cout<<" done with calibration \n";
  
  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  //theApp.Run();
  return(0);

};



//***************************************************
//***************************************************
//         Initial Calibration of one of the BCMs                        
//***************************************************
//***************************************************

void initial_bcm_calibration(TString bcm_title)
{	  
  TString plotcommand;
  TString device_cut;

  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(1.8);
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.1);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadLeftMargin(0.1);

  // Read the sample size from the tree.
  // If we are using the 4-wire sum of a bpm as a reference bcm then
  if(bcm_title != "qwk_bcm0l02")
    SAMPLE_SIZE = Form("%s_EffectiveCharge.num_samples",bcm_title.Data());
  else
    SAMPLE_SIZE = Form("%s.num_samples",bcm_title.Data());
  
  std::cout << "Find the offset and gain for " << bcm_title.Data()
	    << "/"<<SAMPLE_SIZE.Data()<< std::endl;
  
  Canvas->Clear();
  Canvas->Divide(1,2);
  Canvas->cd(1);

 
  // Draw the current we set using scandata
  scut = Form("scandata>%d && scandata < %d && cleandata == 1",min_current,max_current);
  nt->Draw("scandata",scut);

  TH1 *set_current=(TH1*)gROOT->FindObject("htemp");

  if(set_current == NULL)
    {
      std::cout<<" scandata is empty!-> No current! \n Exiting program."<<std::endl;
      exit(1);
    }

  //Find the minmum and maximum current cut from the scandata plot 
  Double_t content=0.0;
  Int_t i;
  for(i=1;i<set_current->GetNbinsX();i++){
    content = set_current-> GetBinLowEdge(i);
    if(content!=0){
      scutmin = content;
      break;  
    }
  };
  
  for(i=set_current->GetNbinsX();i>0;i--){
    content = set_current->GetBinLowEdge(i);
    if(content!=0){
      scutmax = content + set_current->GetBinWidth(i);
      break;  
    }
  };
  
  scutmin = min_current;
  std::cout<<"minimum current from scandata = "<<scutmin<<" uA \n";
  std::cout<<"maximum current from scandata = "<<scutmax<<" uA \n";
  
  
  // to get a fine bin size
  nbin = (Int_t)( (scutmax + 0.5)/currentstep);

  TString histname=Form("prof_%s",bcm_title.Data());
  TProfile *hprofinit= new TProfile(histname, histname, nbin,(scutmin/1.1), (scutmax*1.1));

  // If we are using the 4-wire sum of a bpm as a reference bcm then
  if(bcm_title != "qwk_bcm0l02"){
    plotcommand = Form("%s_EffectiveCharge.hw_sum_raw/%s:scandata>>%s ",
		       bcm_title.Data(),SAMPLE_SIZE.Data(),histname.Data());
    device_cut = Form("%s_EffectiveCharge.Device_Error_Code == 0",bcm_title.Data());
  }
    else{
    plotcommand = Form("%s.hw_sum_raw/%s:scandata>>%s ",
		       bcm_title.Data(),SAMPLE_SIZE.Data(),histname.Data()); 
    device_cut = Form("%s.Device_Error_Code == 0",bcm_title.Data());
    }
  
  // Apply the event cuts
  scut = Form("cleandata==1 && scandata>%f && scandata<%f && %s", scutmin, scutmax,device_cut.Data());
  
  // Draw the bcm current
  nt->Draw(plotcommand,scut,"prof");

  //If the histogram in null, exit program. WE can't do a calibration if bcm data is empty!
  if(((TH1*)gROOT->FindObject(histname)) == NULL)
    {
      std::cout<<" Attempt to plot NULL histogram "<<plotcommand<<"\n Exiting program."<<std::endl;
      exit(1);
    }

  TString xtitle = Form("scandata>>%s",histname.Data()); 
  TString ytitle;
  if(bcm_title != "qwk_bcm0l02")
    ytitle = Form("%s_EffectiveCharge.hw_sum_raw/sample size",bcm_title.Data()); 
  else
    ytitle = Form("%s.hw_sum_raw/%s",bcm_title.Data(),SAMPLE_SIZE.Data()); 
      
  hprofinit->SetXTitle(xtitle);
  hprofinit->SetYTitle(ytitle);
  hprofinit->SetMarkerStyle(20);
  hprofinit->SetMarkerColor(2);

  TF1 *f1;
  TString setalias;
  Double_t offset;
  Double_t gain;
  TString varname;
 
  if(pol2BCM)
    {
      // Fit with a polynomial fo order 2 (curve).
      hprofinit->Fit("pol2","Q");
      f1 = hprofinit->GetFunction("pol2");
      if(f1 == NULL) exit(1);
      Double_t a=f1->GetParameter(2);
      Double_t b=f1->GetParameter(1);
      Double_t c=f1->GetParameter(0);

      Double_t y=hprofinit->GetMean(2);
      Double_t branch=(-1*b+sqrt(b*b-4*a*(c-y)))/(2*a);
     
      // If we are using the 4-wire sum of a bpm as a reference bcm then
      if(bcm_title != "qwk_bcm0l02")
	varname = Form("%s_EffectiveCharge.hw_sum_raw/%s ", bcm_title.Data(),SAMPLE_SIZE.Data());
      else
	varname=Form("%s.hw_sum_raw/%s ", bcm_title.Data(),SAMPLE_SIZE.Data());

      if(branch>0)
	setalias=Form("(-1*%f+sqrt(%f*%f-4*%f*(%f-%s)))/(2*%f)",
		      b,b,b,a,c,varname.Data(),a);
      else
	setalias=Form("(-1*%f-sqrt(%f*%f-4*%f*(%f-%s)))/(2*%f)",
		      b,b,b,a,c,varname.Data(),a);
      offset=c;
      gain=1.0/b;
    }
  else
    {
      // Fit with a polynomial of order 1 (line). 
      hprofinit->Fit("pol1","Q");
      f1 = hprofinit->GetFunction("pol1");
      if(f1 == NULL) exit(1);

      offset = (f1->GetParameter(0));
      gain   = 1.0/(f1->GetParameter(1));

      // If we are using the 4-wire sum of a bpm as a reference bcm then
      if(bcm_title != "qwk_bcm0l02")
	setalias=Form("((%s_EffectiveCharge.hw_sum_raw/%s - %f)*%f)", bcm_title.Data(),SAMPLE_SIZE.Data(), offset, gain);
      else
	setalias=Form("((%s.hw_sum_raw/%s - %f)*%f)", bcm_title.Data(),SAMPLE_SIZE.Data(), offset, gain);
    }

  hprofinit->GetXaxis()->SetTitle(SCANDATA);
  hprofinit->GetYaxis()->SetTitle(bcm_title);
      
  nt->SetAlias("current",setalias); // current from the fit to bcm data
  nt->Draw("current:scandata1/1000>>htemp2",scut,"prof");
  TH1 *h2=(TH1*)gDirectory->Get("htemp2");
  h2->Fit("pol1");
  h2->Draw();

  // Moving on to plot residuals between scandata and current from bcm
  Canvas->cd(2);
  plotcommand = "(current - scandata):scandata"; 

  // The difference between scut and scut_residual is scut_residual is a cut on current projected by the fit to bcm data and scut is a cut on scandata.
  scut_residual= Form("current>%10.1f && current<%10.1f && %s",scutmin,scutmax, scut.Data());

  nt->Draw(plotcommand,scut_residual,"box");
 
  TH1 *g2=nt->GetHistogram();
  g2->GetXaxis()->SetTitle(SCANDATA);
  g2->SetTitle("Residual plot (current - scandata)");
  g2->SetLineColor(kRed);
  g2->SetMarkerColor(kRed);
 

  std::cout << "BCM\t" << bcm_title.Data() << "\t offset=" 
 	    << offset << "\t Gain=" 
 	    << gain<<"\t red chi2 = "<<(f1->GetChisquare())/(f1->GetNDF())
 	    <<" residual (mean,rms) = ("<<Form("%5.2f",g2->GetMean(2))
 	    <<" , "<<Form("%5.2f",g2->GetRMS(2))<<") \n";

  zeroline=new TLine(scutmin,0,scutmax,0);
  zeroline->Draw("same");
    
  Canvas->Update();  

  // Save the canvas on to a pdf file
  Canvas->Print(CanvasTitle+".ps(");
  Canvas->Clear();

  Myfile <<bcm_title.Data() << " , " 
      << offset << " , " 
      << gain<<"\n";
};

//***************************************************
//***************************************************
//         Calibrating BPMs                       
//***************************************************
//***************************************************

void calibrate(TString devname) 
{  
  TString plotcommand[4];

  TString antenna[4]={"XP","XM","YP","YM"};
  Double_t par[3][4];
  Double_t epar[3][4];

  TF1 *f[4];
  TProfile *hprof[4];

  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.35);
  gStyle->SetTitleYOffset(2.0);
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");
 
  Canvas->Clear();
  Canvas->Divide(4,2);

  // check to see if we have the device on the tree
     
  if((nt->FindBranch(Form("%s_EffectiveCharge",devname.Data()))) == NULL)
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
	  hprof[i]= new TProfile(histname, histname, nbin,scutmin/1.1,scutmax*1.1);
	  
	  SAMPLE_SIZE = Form("%s%s.num_samples",devname.Data(),antenna[i].Data());
	  
	  plotcommand[i] = Form("%s%s.hw_sum_raw/%s:current>>%s",
				devname.Data(),antenna[i].Data(),SAMPLE_SIZE.Data(),histname.Data()); 
	  
	  scut = Form("cleandata==1 && current>%f && current<%f && %s%s.Device_Error_Code == 0",scutmin, scutmax, devname.Data(),antenna[i].Data());	    
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
	  
	  TString ytitle0 =  Form("%s%s_raw/samples",devname.Data(),antenna[i].Data());  
	  TString xtitle0  = Form(" I >%10.1f && I <%10.1f (#muA)",scutmin, scutmax);
	  
	  hprof[i]->SetYTitle(ytitle0);
	  hprof[i]->SetXTitle(xtitle0);
	  hprof[i]->GetXaxis()->CenterTitle();
	  hprof[i]->GetYaxis()->CenterTitle();

	  
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
	  TString cut = Form("%s && %s%s.Device_Error_Code == 0",scut_residual.Data(),devname.Data(),antenna[i].Data());
	  nt->Draw(plotcommand[i],cut);	 
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
      
      TH2F *h2[4][2];
      for(int i=0;i<4;i++)
	{
	  Canvas->cd(2*(i+1));
	  Int_t reducednbin=500;
	  
	  //Draw the responce of the bpms
	  TString histname=Form("Res_%s%s",devname.Data(),antenna[i].Data());
	  h2[i][0]=new TH2F(histname,histname,reducednbin,0, max_current*1.1,reducednbin,Yminres,Ymaxres);
	  nt->Draw(plotcommand[i]+":current>>"+histname,scut_residual);

	  h2[i][0]->Draw("box");
	  
	  // Draw the restricted responce of the bpms due to variation in beam current
	  histname=Form("Residual_%s%s",devname.Data(),antenna[i].Data());
	  h2[i][1]=new TH2F(histname,histname,reducednbin,scutmin/1.1, scutmax*1.1,reducednbin,Yminres,Ymaxres); 
	  nt->Draw(plotcommand[i]+":current>>"+histname,scut);
      
	  h2[i][1]->SetLineColor(kRed);
	  h2[i][1]->Draw("boxsame");
	  
	  TString xtitle1  = Form("current >%10.1f #muA",scutmin);
	  
	  h2[i][1]->SetYTitle(Form("residual (fit - actual )"));
	  h2[i][1]->SetXTitle(xtitle1);
	  h2[i][1]->GetXaxis()->CenterTitle();
	  h2[i][1]->GetYaxis()->CenterTitle();

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


