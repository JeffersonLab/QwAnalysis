// Author: David Zou; dzou@jlab.org
// Date: Friday, November 27 17:52:09 EST 2010
//
// execute in root with the filename (without .root suffix)
//
// example:
// .x accumMacro.C("first100k_20390")
//

#include "TSystem.h"
#include "TString.h"


void

// TH1D*
// GetHisto(TTree *tree, const TString name, const TCut cut, Option_t* option = "")
// {
//   tree ->Draw(name, cut, option);
//   TH1D* tmp;
//   tmp = (TH1D*)  gPad -> GetPrimitive("htemp");
//   if(not tmp) {
//     return 0;
//   }
//   return tmp;
// }

accumMacro(TString filename = "Compton_3304")
{
 //change output_dir to location where png files should be saved
 TString output_dir;
 output_dir = "$QWSCRATCH/plots/";
 TFile *file;
 TTree *Hel;
// TTree *mps
 file = new TFile("$QW_ROOTFILES/"+filename+".root");
 Hel = (TTree*) file->Get("Hel_Tree");
// mps = (TTree*) file->Get("Mps_Tree");
 

 TCanvas * c0 = new TCanvas("c0", "c0", 1200, 800);;
 TCanvas * c1 = new TCanvas("c1", "c1", 1200, 800);;
 TCanvas * c2 = new TCanvas("c2", "c2", 1200, 800);;
 TCanvas * c3 = new TCanvas("c3", "c3", 1200, 800);;
//  TH1D * accum0_off = NULL; 
//  TH1D * accum0_on = NULL; 
//  TH1D * accum0_yield = NULL; 
//  TH1D * accum0_laser = NULL; 

  gStyle->SetOptStat(0);
  gStyle->SetOptStat(1000002210);
  gStyle->SetStatW(0.36); 
  gStyle->SetStatFontSize(0.054);

//  accum0_off = GetHisto(Hel, "asym_fadc_compton_accum0", "yield_sca_laser_PowT<2", "");

  c0->Divide(2,2);
  c0->cd(1);
  Hel->Draw("asym_fadc_compton_accum0", "yield_sca_laser_PowT<2");
  c0->cd(2);
  Hel->Draw("asym_fadc_compton_accum0", "yield_sca_laser_PowT>=2");
  c0->cd(3);
  Hel->Draw("yield_fadc_compton_accum0:pat_counter");
  c0->cd(4);
  Hel->Draw("yield_sca_laser_PowT:pat_counter");
  gPad->Update();

  c1->Divide(2,2);
  c1->cd(1);
  Hel->Draw("asym_fadc_compton_accum1", "yield_sca_laser_PowT<2");
  c1->cd(2);
  Hel->Draw("asym_fadc_compton_accum1", "yield_sca_laser_PowT>=2");
  c1->cd(3);
  Hel->Draw("yield_fadc_compton_accum1:pat_counter");
  c1->cd(4);
  Hel->Draw("yield_sca_laser_PowT:pat_counter");
  gPad->Update();

  c2->Divide(2,2);
  c2->cd(1);
  Hel->Draw("asym_fadc_compton_accum2", "yield_sca_laser_PowT<2");
  c2->cd(2);
  Hel->Draw("asym_fadc_compton_accum2", "yield_sca_laser_PowT>=2");
  c2->cd(3);
  Hel->Draw("yield_fadc_compton_accum2:pat_counter");
  c2->cd(4);
  Hel->Draw("yield_sca_laser_PowT:pat_counter");
  gPad->Update();

  c3->Divide(2,2);
  c3->cd(1);
  Hel->Draw("asym_fadc_compton_accum3", "yield_sca_laser_PowT<2");
  c3->cd(2);
  Hel->Draw("asym_fadc_compton_accum3", "yield_sca_laser_PowT>=2");
  c3->cd(3);
  Hel->Draw("yield_fadc_compton_accum3:pat_counter");
  c3->cd(4);
  Hel->Draw("yield_sca_laser_PowT:pat_counter");
  gPad->Update();

  

  image_c0 = filename+ "_Accum0.png";
  image_c1 = filename+ "_Accum1.png";
  image_c2 = filename+ "_Accum2.png";
  image_c3 = filename+ "_Accum3.png";

  c0 -> SaveAs(output_dir+image_c0);
  c1 -> SaveAs(output_dir+image_c1);
  c2 -> SaveAs(output_dir+image_c2);
  c3 -> SaveAs(output_dir+image_c3);



}