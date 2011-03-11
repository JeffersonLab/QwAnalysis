//**********************************************************************************************//
// Author : B. Waidyawansa
// Date   : March 10, 2009
//**********************************************************************************************//
//
// This script plots the variaiton of the  position differences across beamline and 
// charge asymmtery of a bcm.
// The resulting plots are printed on to * .gif file.
// One plot contains the charge asymmetry and the variation in the position differences,
// the other plot contains all the plots of X and Y differences.
// The position differences in X and Y and their resoltuions are stored in to *.txt file.
//
// To use this script type
//
//       gmake  position_differnces.
//
// This should create the exe position_differnces.
//
// To run the exe type
//
// ./position_differnces run number bcm
//   run_num - run number
//   bcm     - bcm that was used for current monitoring. If a bcm is not specified,
//             the default bcm qwk_bcm0l02 will be used.
//
// e.g.
// ./position_differnces 1672 0l02
//



//********************************************
// Main function
//********************************************



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
#include <TPaveText.h>

#include <cstdlib>


Double_t maxy1=0.0, maxy2=0.0;
TString runnum;
TString bcm; 
TString current;
TString tempname;

Char_t filename[300];

TString directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration

const Int_t ndevices = 21;
TString devicelist[ndevices]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05" ,"qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
   "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
   "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
   "qwk_0r06"};


Double_t relx[ndevices], rely[ndevices],erx[ndevices],
  ery[ndevices],err[ndevices],name[ndevices+1];

Double_t offset = 0.5;
Double_t min_range = - offset;
Double_t max_range = ndevices - offset ; 



int main(Int_t argc,Char_t* argv[])
{
  
  gDirectory->Delete("*") ;
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetPadBottomMargin(0.08);
  gStyle->SetPadLeftMargin(0.08);  
  gStyle->SetPadRightMargin(0.08);  
  gStyle->SetPaperSize(10,12);

  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetTitleSize(0.08);


  TApplication theApp("App",&argc,argv);
 
  // Read in the arguments.
  // The first argument should be the program name itself.
  // The second argument should be the run number
  // The third argument should be the BCM to be used with.

  if(argc<2 || argc>3)
    {
      std::cout<<"Please enter a run number and a valid bcm"<<std::endl;
      exit(1);
    }
  else if(argc == 2)
    {
      std::cout<<"Using the default bcm : qwk_bcm0l02."<<std::endl;
      runnum = argv[1];
      bcm = "qwk_bcm0l02";
    }
  else if(argc == 3) 
    {
      runnum = argv[1];
      bcm = argv[2];
    }


  //Open a file to store data
  std::ofstream Myfile;
  TString textfile = Form("%s_deltaXY_variations_%s.txt",runnum.Data(),bcm.Data()); 
  Myfile.open(textfile);
  Myfile <<"Positon difference variations of run "<<runnum<<std::endl;
  Myfile <<"BPM\t\t diff x\t\t\t diff y\n";
  

  //Get the root file
  sprintf(filename,"%sQweak_BeamLine_%s.root",directory.Data(),runnum.Data());
  TFile * f = new TFile(filename);
  if(!f->IsOpen())
      return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";


  //load the HEL_Tree
  TTree * nt = (TTree*)f->Get("HEL_Tree"); 


  //Create a new canvas
  TString c1title = Form("Relative X & Y Position Differences for  %s",runnum.Data());
  TCanvas *c1 = new TCanvas("c1",c1title,1200,1000);
  c1->Draw();
  c1->Clear();

  //The title pad
  TPad * pad1 = new TPad("pad1","pad1",0.02,0.02,0.98,0.93);
  pad1->SetFillColor(11);
  pad1->Draw();
  pad1->Divide(6,8);  // to plot 42  plots..


  TPaveText * pt = new TPaveText(0.02, 0.94, 0.98, 0.98, "br");
  pt->SetBorderSize(1);pt->SetFillColor(10);
  pt->SetTextAlign(12);pt->SetTextFont(22);
  pt->SetTextColor(32);
  pt->AddText(c1title.Data());
  pt->Draw();

  //Apply a general cut.
  TString scut = "ha_cleandata==1";
  nt->SetAlias("scut",scut);
  

  // Draw histograms
  TH1D * h = NULL; //clear the contents
  TH1D * dXVariation = new TH1D("dxvar", "#Delta X Variation", ndevices, min_range, max_range);
  TH1D * dYVariation = new TH1D("dyvar", "#Delta Y variation", ndevices, min_range, max_range);



  for(Int_t i=0; i<ndevices; i++)    
    {  
      pad1->cd(i+1);
      nt->Draw(Form("diff_%sRelX.hw_sum*1000",devicelist[i].Data()), "scut");
      // std::cout<<Form("diff_%sRelX.hw_sum*1000",devicelist[i].Data())<<std::endl;
      h = (TH1D*) gPad->GetPrimitive("htemp");

      // Avoids the program from crashing when trying to plot NULL histograms
      if(h == NULL) 
	{
	  std::cout<<"Cannot draw "<<Form("diff_%sRelX.hw_sum*1000",devicelist[i].Data())<<std::endl;
	  std::cout<<"Check to see if the HEL tree is filled."<<std::endl;
exit(1);
	}

      h->Draw();
      h->SetName(devicelist[i]);
      tempname = h -> GetName();
      tempname.Replace(0,4," ");
      tempname.ReplaceAll("RelX.hw_sum", "#Delta X");
      dXVariation -> SetBinContent(i+1, h->GetMean());
      dXVariation -> SetBinError  (i+1, h->GetMeanError());
      if((h->GetMeanError())> maxy1) maxy1=(h->GetMeanError());
      dXVariation -> GetXaxis()->SetBinLabel(i+1, tempname);
      
      // Print the mean and error on to file
      Myfile <<devicelist[i].Data()<<"\t\t"<< h->GetMean() <<"+-" << h->GetMeanError()<<"\t";
      
      pad1->cd(i+25);
      nt->Draw(Form("diff_%sRelY.hw_sum*1000",devicelist[i].Data()), "scut");
      //std::cout<<Form("diff_%sRelY.hw_sum*1000",devicelist[i].Data())<<std::endl;
      h = (TH1D*) gPad->GetPrimitive("htemp");
      
      // Avoids the program from crashing when trying to plot NULL histograms
      if(h == NULL) 
	{
	  std::cout<<"Cannot draw "<<Form("diff_%sRelY.hw_sum*1000",devicelist[i].Data())<<std::endl;
	  std::cout<<"Check to see if the HEL tree is filled."<<std::endl;
	  exit(1);
	}
      h->Draw();
      h->SetName(devicelist[i]);
      tempname = h -> GetName();
      tempname.Replace(0,4," ");
      tempname.ReplaceAll("RelY.hw_sum", "#Delta Y");
      dYVariation -> SetBinContent(i+1, h->GetMean());
      dYVariation -> SetBinError  (i+1, h->GetMeanError());
      if((h->GetMeanError())> maxy2) maxy2=(h->GetMeanError());
      dYVariation -> GetXaxis()->SetBinLabel(i+1, tempname);

      // Print the mean and error on to file
      Myfile <<"\t\t"<<h->GetMean()<<"+-"<<h->GetMeanError()<<std::endl;
    }



  
  TString c2title = Form("Relative X & Y Difference Variation Across Beamline for %s",runnum.Data());
  TCanvas *c2 = new TCanvas("c2",c2title,1200,1000);
  c2->Draw();
  c2->Divide(1,3);
  c2->SetFillColor(11);

  c2->cd(1);
  if(bcm.Contains("bcm0l02")==1)
    current = Form("asym_qwk_bcm0l02.hw_sum");
  else
    current = Form("asym_qwk_%sWSum.hw_sum",bcm.Data());
 
  nt->SetAlias("current",current);
  nt->Draw(current);


  // Plot the variation in X differences along the beamline
  c2->cd(2);
  dXVariation->SetStats(kFALSE);
  dXVariation->Draw();

  //now draw lines to represent aparutus
  TLine *  A1 = new TLine(5.5,-maxy1-20.0,5.5,maxy1+20.0);
  TLine *  A2 = new TLine(6.5,-maxy1-20.0,6.5,maxy1+20.0);
  TLine *  A3 = new TLine(7.5,-maxy1-20.0,7.5,maxy1+20.0);
  TLine *  A4 = new TLine(8.5,-maxy1-20.0,8.5,maxy1+20.0);
  TLine *  A5 = new TLine(9.5,-maxy1-20.0,9.5,maxy1+20.0);

  A1->Draw("same");
  A2->Draw("same");
  A3->Draw("same");
  A4->Draw("same");
  A5->Draw("same");

  // Plot the variation in Y differences along the beamline
  c2->cd(3);
  dYVariation->SetStats(kFALSE);
  dYVariation->Draw();


  //now draw lines to represent aparutus
  TLine * A11 = new TLine(5.5,-maxy2-20.0,5.5,maxy2+20.0);
  TLine * A22 = new TLine(6.5,-maxy2-20.0,6.5,maxy2+20.0);
  TLine * A33 = new TLine(7.5,-maxy2-20.0,7.5,maxy2+20.0);
  TLine * A44 = new TLine(8.5,-maxy2-20.0,8.5,maxy2+20.0);
  TLine * A55 = new TLine(9.5,-maxy2-20.0,9.5,maxy2+20.0);

  A11->Draw("same");
  A22->Draw("same");
  A33->Draw("same");
  A44->Draw("same");
  A55->Draw("same");
 
  c2->Update(); 
  c1->Update();
    
  TString impod= Form("%s_poss_diff_%s.gif",runnum.Data(),bcm.Data());
  TString impodva = Form("%s_poss_diff_variations_%s.gif",runnum.Data(),bcm.Data());

  c1->Print(impod.Data());
  c2->Print(impodva.Data());  

  std::cout<<"Done plotting! "<<std::endl;
  Myfile.close();
  //theApp.Run();
  return(0);

};




