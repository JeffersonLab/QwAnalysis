//
// Sarah K. Phillips                  November 11, 2010
//
// This script plots the x and y beam positions (yields) 
// and differences for the Qweak beamline, from the arc 
// to the target, for diagnostic purposes.
//
// --------------------------------------------------------
//



// All my standard ROOT includes...
#include <TString.h>
#include <string> 
#include <fstream>
#include <iostream> 
#include <TCanvas.h> 
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TH3F.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h> 
#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TPaveLabel.h>
#include <vector>
#include <TProfile.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TGraph.h>
#include <TDatime.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TGaxis.h>
#include <TPad.h>
#include <TLatex.h>
#include <TRandom.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <TMath.h>
#include <TMinuit.h>
#include <TStopwatch.h>
#include <TSpectrum.h>
#include "TMath.h"
#include <sstream> 

#include "TSystem.h"

// Interactive includes
#include <TRint.h>
#include <TApplication.h>

#include <TGX11.h>
#include <TVirtualX.h>
#include <TFriendElement.h>


using namespace std ;







// ************  The main program starts here. ****************//

int main(int argc, char** argv)
{
  Int_t debug = 1;

  // Initialize ROOT
  // This creates an application environment. The application environment
  // provides an interface to the graphics system and eventloop  
  TApplication theApp("App", &argc, argv);
  // gStyle->SetOptFit(0111);
  gROOT->SetStyle("Plain");
  gStyle->SetFillColor(10); 

  gStyle->SetTextFont(132);




  // -----  Our arrays
  TString bpms[24] = {"qwk_bpm3c07", "qwk_bpm3c07a", "qwk_bpm3c08", "qwk_bpm3c11", "qwk_bpm3c12", "qwk_bpm3c14", "qwk_bpm3c16", "qwk_bpm3c17", "qwk_bpm3c18", "qwk_bpm3c19", "qwk_bpm3c20", "qwk_bpm3c21", "qwk_bpm3h02", "qwk_bpm3h04", "qwk_bpm3h07a", "qwk_bpm3h07b", "qwk_bpm3h07c", "qwk_bpm3h08", "qwk_bpm3h09", "qwk_bpm3h09b", "qwk_bpm3p02a", "qwk_bpm3p02b", "qwk_bpm3p03a", "qwk_target"};



  TString run_number; 
  TString rootfile;
  TString cuts = " ";  // Put in a simple cut
  TString devicecut1[24]; // Cut on bpm Device Error Code
  TString devicecut2[24];
  TString devicecut3[24];
  TString devicecut4[24];
  TString tempcuts = " "; // Cut to hold total desired cut (devicecut)

  cout << "Enter the root file name (i.e. first100k_6854.root): " << endl; 

  cin >> run_number;

  rootfile="$QW_ROOTFILES/"+run_number;

  // // This is the original for the 100k files
  //  cout << "Enter the run number: " << endl; 
  //  cin >> run_number;
  //  rootfile="$QW_ROOTFILES/first100k_"+run_number+".root";


  cout << "Enter your cut in root format (i.e. yield_qwk_bcm1>50).  If you don't want any cuts, just enter 1:" << endl;

  cin >> cuts;

  cout << "Plotting things for " << rootfile << "..." << endl;

  TFile *file0 = TFile::Open(rootfile);



  TH1F* h1[24];  // for y differences
  TH1F* h2[24];  // for x differences
  TH1F* h3[24];  // for y positions (not used right now)
  TH1F* h4[24];  // for x positions (not used right now)


  TString histname1[24];   // for y differences
  TString histname2[24];   // for x differences
  TString histname3[24];   // for y positions 
  TString histname4[24];   // for x positions

  TString hist1[24];    // for y differences
  TString hist2[24];    // for x differences
  TString hist3[24];    // for y positions 
  TString hist4[24];    // for x positions


  string num;  // This has to be a string, because TString doesn't have clear.
  std::ostringstream tmp1[24];
  std::ostringstream tmp2[24];
  std::ostringstream tmp3[24];
  std::ostringstream tmp4[24];

  TString plot1[24];
  TString plot2[24];
  TString plot3[24];
  TString plot4[24];


  // a kludge to make it not put (1) for no cuts!
  TString comparison = "1";



  for(int i=0;i<24;i++)
    {
      // Y differences
      histname1[i] = "diff_"+bpms[i]+"Y"; 
      tmp1[i] << i;
      num = tmp1[i].str();
      hist1[i] = "h1["+num+"]";
      devicecut1[i] = "((diff_"+bpms[i]+"Y.Device_Error_Code&0xfc)==0)";
      //cout << histname1[i] << endl;
      //cout << hist1[i] << endl;
      
      if(cuts.CompareTo(comparison)!=0){
	h1[i] = new TH1F(hist1[i],histname1[i]+" ("+cuts+")",100,-0.07,0.072);
      }
      else{
	h1[i] = new TH1F(hist1[i],histname1[i],100,-0.07,0.072);
      }

      plot1[i] = histname1[i]+">>"+hist1[i];
      //cout << plot1[i] << endl;

      //num.clear(); // doesn't work for some reason!
      //num.assign(""); // this also doesn't work!!
 
      
      // X differences
      histname2[i] = "diff_"+bpms[i]+"X";
      tmp2[i] << i;
      num = tmp2[i].str();
      hist2[i] = "h2["+num+"]";
      devicecut2[i] = "((diff_"+bpms[i]+"X.Device_Error_Code&0xfc)==0)";
      //cout << histname2[i] << endl;
      //cout << hist2[i] << endl;

      if(cuts.CompareTo(comparison)!=0){
	h2[i] = new TH1F(hist2[i],histname2[i]+" ("+cuts+")",100,-0.07,0.072);
      }
      else{
	h2[i] = new TH1F(hist2[i],histname2[i],100,-0.07,0.072);
      }

	plot2[i] = histname2[i]+">>"+hist2[i];


      // Y position
      histname3[i] = "yield_"+bpms[i]+"Y";
      devicecut3[i] = "((yield_"+bpms[i]+"Y.Device_Error_Code&0xfc)==0)";
      plot3[i] = histname3[i];  // I'm not regulating the scale of these
      //plot3[i] = histname3[i]+">>"+hist3[i];


      // X position
      histname4[i] = "yield_"+bpms[i]+"X";
      devicecut4[i] = "((yield_"+bpms[i]+"X.Device_Error_Code&0xfc)==0)";
      plot4[i] = histname4[i];  // I'm not regulating the scale of these
      //plot4[i] = histname4[i]+">>"+hist4[i];

    }  // end of for(int i=0;i<24;i++) over bpm names



  




  TTree *Hel_Tree;
  Hel_Tree = (TTree*)file0->Get("Hel_Tree");
 

  TString diffhist1;    // for y target diff title
  TString diffhist2;    // for x target diff title



  TString output_dir;

  output_dir = gSystem->Getenv("QWSCRATCH");
  output_dir += "/plots/";
  
  // ----  Y differences

  TString cname11 = run_number+"  BPMs (Y differences)";
  TCanvas *c11 = new TCanvas(Form("%s_BPMY_diff_c11", run_number.Data()),cname11,100,10,1200,900);
  //TCanvas *c11 = new TCanvas("c11","BPMs (Y differences)",100,10,1200,900);
  c11->Divide(4,3); 
  c11->cd(1); 

 
  // The first 12 bpms (keeping the canvas somewhat readable)
  for(int i=0;i<12;i++) 
    {  
      c11->cd(i+1);
      tempcuts = cuts+" && "+devicecut1[i];
      Hel_Tree->Draw(plot1[i],tempcuts);
      
      //h1f[0]->Draw();
      //c12->Update();

    }
  
  
  c11->SaveAs(Form("%s%s.png", output_dir.Data(), c11->GetName()));

  TString cname12 = run_number+"  BPMs (Y differences)";
  TCanvas *c12 = new TCanvas(Form("%s_BPMY_diff_c12", run_number.Data()),cname12,100,10,1200,900);
  //TCanvas *c12 = new TCanvas("c12","BPMs (Y differences)",100,10,1200,900);
  c12->Divide(4,3); 
  c12->cd(1);

  // The second 12 bpms (keeping the canvas somewhat readable)
  for(int i=12;i<24;i++) 
    {  
      c12->cd(i-11); // a kludge since I divided the canvas into two
      tempcuts = cuts+" && "+devicecut1[i]; 
     
      if(i<23){
        Hel_Tree->Draw(plot1[i],tempcuts);
      }
      else{  // I want the target prejection on a difference scale
	//diffhist1 = histname[i]+" (Different Scale) "; // give it a different title
	
	Hel_Tree->Draw(histname1[i],tempcuts);
      }
    }

  c12->SaveAs(Form("%s%s.png", output_dir.Data(), c12->GetName()));

  // ----  X differences

  TString cname21 = run_number+"  BPMs (X differences)";
  TCanvas *c21 = new TCanvas(Form("%s_BPMX_diff_c21", run_number.Data()),cname21,100,10,1200,900);
  //TCanvas *c21 = new TCanvas("c21","BPMs (X differences)",100,10,1200,900);
  c21->Divide(4,3); 
  c21->cd(1); 

 
  // The first 12 bpms (keeping the canvas somewhat readable)
  for(int i=0;i<12;i++) 
    {  
      c21->cd(i+1);
      tempcuts = cuts+" && "+devicecut2[i];
      Hel_Tree->Draw(plot2[i],tempcuts);
      
    }
  
  c21->SaveAs(Form("%s%s.png", output_dir.Data(), c21->GetName()));
  
  TString cname22 = run_number+"  BPMs (X differences)";
  TCanvas *c22 = new TCanvas(Form("%s_BPMX_diff_c22", run_number.Data()),cname22,100,10,1200,900);
  //TCanvas *c22 = new TCanvas("c22","BPMs (X differences)",100,10,1200,900);
  c22->Divide(4,3); 
  c22->cd(1);

  // The second 12 bpms (keeping the canvas somewhat readable)
  for(int i=12;i<24;i++) 
    {  
      c22->cd(i-11); // a kludge since I divided the canvas into two
      tempcuts = cuts+" && "+devicecut2[i];

      if(i<23){
	Hel_Tree->Draw(plot2[i],tempcuts);
      }
      else{  // I want the target prejection on a difference scale
	Hel_Tree->Draw(histname2[i],tempcuts);
      }
    }


  c22->SaveAs(Form("%s%s.png", output_dir.Data(), c22->GetName()));

  // ----  Y positions

  TString cname31 = run_number+"  BPMs (Y Positions)";
  TCanvas *c31 = new TCanvas(Form("%s_BPMY_pos_c31", run_number.Data()),cname31,100,10,1200,900);
  //TCanvas *c31 = new TCanvas("c31","BPMs (Y Yields)",100,10,1200,900);
  c31->Divide(4,3); 
  c31->cd(1); 

 
  // The first 12 bpms (keeping the canvas somewhat readable)
  for(int i=0;i<12;i++) 
    {  
      c31->cd(i+1);
      tempcuts = cuts+" && "+devicecut3[i];
      Hel_Tree->Draw(plot3[i],tempcuts);
      
    }
  
  c31->SaveAs(Form("%s%s.png", output_dir.Data(), c31->GetName()));
  
  TString cname32 = run_number+"  BPMs (Y Positions)";
  TCanvas *c32 = new TCanvas(Form("%s_BPMY_pos_c32", run_number.Data()),cname32,100,10,1200,900);
  //TCanvas *c32 = new TCanvas("c32","BPMs (Y Yields)",100,10,1200,900);
  c32->Divide(4,3); 
  c32->cd(1);

  // The second 12 bpms (keeping the canvas somewhat readable)
  for(int i=12;i<24;i++) 
    {  
      c32->cd(i-11); // a kludge since I divided the canvas into two
      tempcuts = cuts+" && "+devicecut3[i];

      if(i<23){
	Hel_Tree->Draw(plot3[i],tempcuts);
      }
      else{  // I want the target prejection on a difference scale
	Hel_Tree->Draw(histname3[i],tempcuts);
      }
    }


  c32->SaveAs(Form("%s%s.png", output_dir.Data(), c32->GetName()));

  // ----  X positions

  TString cname41 = run_number+"  BPMs (X Positions)";
  TCanvas *c41 = new TCanvas(Form("%s_BPMX_pos_c41", run_number.Data()),cname41,100,10,1200,900);
  //TCanvas *c41 = new TCanvas("c41","BPMs (X Yields)",100,10,1200,900);
  c41->Divide(4,3); 
  c41->cd(1); 

 
  // The first 12 bpms (keeping the canvas somewhat readable)
  for(int i=0;i<12;i++) 
    {  
      c41->cd(i+1);
      tempcuts = cuts+" && "+devicecut4[i];
      Hel_Tree->Draw(plot4[i],tempcuts);
      
    }
  
  c41->SaveAs(Form("%s%s.png", output_dir.Data(), c41->GetName()));
  
  TString cname42 = run_number+"  BPMs (X Positions)";
  TCanvas *c42 = new TCanvas(Form("%s_BPMX_pos_c42", run_number.Data()),cname42,100,10,1200,900);
  //TCanvas *c42 = new TCanvas("c42","BPMs (X Yields)",100,10,1200,900);
  c42->Divide(4,3); 
  c42->cd(1);

  // The second 12 bpms (keeping the canvas somewhat readable)
  for(int i=12;i<24;i++) 
    {  
      c42->cd(i-11); // a kludge since I divided the canvas into two
      tempcuts = cuts+" && "+devicecut4[i];

      if(i<23){
	Hel_Tree->Draw(plot4[i],tempcuts);
      }
      else{  // I want the target prejection on a difference scale
	Hel_Tree->Draw(histname4[i],tempcuts);
      }
    }


  c42->SaveAs(Form("%s%s.png", output_dir.Data(), c42->GetName()));
  

  cout << "Blah!!!!!!" << endl;






  // ---------------------------------------------------

  // Main application eventloop. Calls system dependent eventloop
  // -> runs an interactive interface 
  theApp.Run();


  return(0); 


}

/**********  End of main *************/

