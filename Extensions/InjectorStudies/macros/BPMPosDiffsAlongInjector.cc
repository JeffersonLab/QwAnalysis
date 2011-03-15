//********************************************************************//
// Author : B. Waidyawansa (buddhini@jlab.org)
// Date   : December 15th, 2010
//********************************************************************//

//*******************************************************************//
// Modified by : Emmanouil Kargiantoulakis
// Date        : 03/04/2011
//******************************************************************//

/*
 This macro plots the BPM position differences along the injector vs BPM
 using the full root file, ie combining the run segments up to rseg.
 You can choose the direction, X or Y, along which to get 
 position difference data from the BPMs.
 It has 5 optional inputs.
 1. Run number
 2. Run segment up to which to combine the root files. Default = 0.
 3. Direction. Default = "X" . Enter "Y" for Y-position differences.
 4. Lower limit to difference in nm. Default = 0 (no limit).
 5. Upper limit to difference in nm. Default =0 (no limit).

To use this macro,  load it in ROOT and do (e.g)
root [10] .L BPMPosDiffsAlongInjector.cc
root [11] PlotBPM_PosDiffs(8231,1,"Y",-200,200)

*/

//********************************************
// Main function
//********************************************


#include "TMath.h"
#include <vector>
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <TVector.h>
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
#include <TPaveLabel.h>
#include <TGraphErrors.h>  
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TChain.h>
#include <cstdlib>


void PlotBPM_PosDiffs(Int_t runnum, Int_t rseg=0, TString direction="X",
		      Int_t llimit=0, Int_t ulimit=0)
{
    
  // Combine the Hel_Tree of the run segments into a chain.
  // The rootfiles will be looked for at $QW_ROOTFILES.
  // You can change the path as an argument to openchain.
  // Look at the openchain function after the main function.
  // Make sure you are using the correct stem for the rootfile.
  openchain(runnum, rseg, "qwinjector"); 


  if(Hel_Tree == NULL){
    std::cout<< "Unable to find the root file." << std::endl; 
    exit(1);
  }


  const Int_t ndevices = 21;
  
  TString devicelist[ndevices]=
  {
    "1i02","1i04","1i06","0i02","0i02a","0i05","0i07","0l01","0l02",
    "0l03","0l04","0l05","0l06","0l07","0l08","0l09","0l10","0r03",
    "0r04","0r05","0r06"
  };

  for (Int_t i=0;i<ndevices;i++)  devicelist[i] += direction;


  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetOptStat(1); 


  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.07);  
  gStyle->SetPadRightMargin(0.05);  
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.04,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);

  
  TString ctitle = Form("X Position Differences Variation (%i < Dx < %i nm) Across Injector in run %i",
			llimit,ulimit,runnum);
  TCanvas *c = new TCanvas("c",ctitle,1200,600);


  c->SetFillColor(0);

  TPad * pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  TPad * pad2 = new TPad("pad2","pad2",0.01,0.02,0.99,0.94);
  pad1->SetFillColor(11);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TText * t1 = new TText(0.02,0.3,ctitle);
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->Divide(1,2);

  TString command;
  TString cut;
  std::vector<TString>  names;
  TVectorD  mean(0);
  TVectorD  width(0);	
  TVectorD  meanerr(0);
  TVectorD  fakeerr(0);
  TVectorD  fakename(0);
  TVectorD  badname(0);
  names.clear();

  Int_t k = 0;
  Int_t j=0;

  for(Int_t i=0; i<ndevices; i++) {  
      command = Form("diff_qwk_%s.hw_sum*1e6>>htemp", devicelist[i].Data());
      cut =  Form("diff_qwk_%s.Device_Error_Code<1 && ErrorFlag == 0",
		   devicelist[i].Data());
      Hel_Tree->Draw(command, cut, "goff");

      TH1 * h1 = (TH1D*)gDirectory->Get("htemp");
      if(!h1) exit(1);
      mean.ResizeTo(k+1);
      width.ResizeTo(k+1);	
      meanerr.ResizeTo(k+1);
      fakeerr.ResizeTo(k+1);
      fakename.ResizeTo(k+1);

      Double_t meanl = h1->GetMean();

      fakename.operator()(k) =(k+1);
      names.push_back(devicelist[i]);
	
      if((TMath::Abs(ulimit)>0) && (TMath::Abs(llimit)>0)){
	if((meanl > llimit) && (meanl < ulimit) ){
	  std::cout<<"Getting data from : "<<devicelist[i]<<std::endl;
	  mean.operator()(k) = meanl;
          width.operator()(k) = h1->GetRMS();
	  meanerr.operator()(k) = (h1->GetMeanError());
	  fakeerr.operator()(k) = (0.0);
	}
	else {
	  std::cout<<devicelist[i]<<" has X position diff out of the given limits"<<std::endl;
	  std::cout<<"Ingored this device. "<<std::endl;
	  badname.ResizeTo(j+1);
	  badname.operator()(j) =(k);
	  j++;
	}
      }
      if(ulimit == 0 && llimit == 0){
	std::cout<<"Getting data from : "<<devicelist[i]<<std::endl;
	mean.operator()(i) = (h1->GetMean());
	width.operator() (i) = (h1->GetRMS());
	meanerr.operator()(i) = (h1->GetMeanError());
	fakeerr.operator()(i) = (0.0);
      }

      k++;

      delete h1;
  }

  Int_t size = mean.GetNoElements();
  Int_t nbad = badname.GetNoElements();

  if(mean.Norm1() == 0  ) {
    std::cout<<"There are no data of the type requested in this run!"<<std::endl;
    exit(1);
  };

  TGraphErrors* grp = new TGraphErrors(fakename, mean, fakeerr,meanerr);
  grp->SetMarkerColor(2);
  grp->SetMarkerStyle(21);
  grp->SetMarkerSize(1);
  grp->SetTitle("");
  grp->GetYaxis()->SetTitle("X Position Difference (nm)");
  grp->GetYaxis()->SetTitleOffset(0.9);

  TGraph* grw = new TGraph(fakename, width);
  grw->SetMarkerColor(2);
  grw->SetMarkerStyle(21);
  grw->SetMarkerSize(1);
  grw->SetTitle("");
  grw->GetYaxis()->SetTitle("Width (nm)");
  grw->GetYaxis()->SetTitleOffset(0.9);

  pad2->cd(1);
  TAxis *axis = grp->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  Double_t ypos =  grp->GetHistogram()->GetMaximum()-grp->GetHistogram()->GetMinimum();
  Double_t ylabel = grp->GetHistogram()->GetMinimum() - 0.01*ypos;
  grp->Draw("AEP");

  TText t;
  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);
  for (Int_t i=0;i<size;i++){
    for (Int_t ik=0;ik<nbad;ik++){
      if (i==badname[ik]){ 
	t.SetTextColor(2);
      }
    }
    t.DrawText(fakename[i],ylabel,names[i]);
    t.SetTextColor(1);
  }
  c->Modified();
  c->Update();

  pad2->cd(2);
  axis = grw->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  ypos =  grw->GetHistogram()->GetMaximum()-grw->GetHistogram()->GetMinimum();
  ylabel = grp->GetHistogram()->GetMinimum() - 0.01*ypos;
  grw->Draw("APB");

  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);
  for (Int_t i=0;i<size;i++){
    for (Int_t ik=0;ik<nbad;ik++){
      if (i==badname[ik]){ 
	t.SetTextColor(2);
      }
    }
    t.DrawText(fakename[i],ylabel,names[i]);
    t.SetTextColor(1);
  }

  c->Modified();
  c->Update();
  
  // Save the canvas on to a .gif file
  // c->Print(Form("plots/%i_bpm_x_posdiffs_%i_to_%i.gif",runnum,llimit,ulimit));
  std::cout<<"Done plotting!\n";
}





void openchain(Int_t irun, Int_t rseg=0, TString prefix = "QwPass1", TString path = "$QW_ROOTFILES/"){

// for old files you can look at :
// "/cache/mss/hallc/qweak/rootfiles/pass0/"

  TChain *Mch1, *Mch2;
  Mch1 = new TChain("Hel_Tree");
  Mch2 = new TChain("Mps_Tree");

  TFile *f1;

  for(Int_t ir=0; ir<=rseg; ir++) {
    f1 = new TFile(path+prefix+Form("_%4d.00%1d.root",irun,ir));
    if (! f1->IsZombie()){
      Mch1->Add(path+prefix+Form("_%4d.00%1d.root",irun, ir));
      Mch2->Add(path+prefix+Form("_%4d.00%1d.root",irun, ir));
      cout<<"Opened rootfile "<<prefix.Data()<<"_"<<irun<<".00"<<ir
	  <<".root"<<endl;
    }
  }

}


