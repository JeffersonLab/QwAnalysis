#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>


void bkgd_summary(TString file){
gROOT->Reset();
gROOT->SetStyle("Plain");


 Char_t filename[100];
 sprintf(filename,"$QW_ROOTFILES/%s",file.Data());
 f = new TFile(filename);
 if(!f->IsOpen()){
   std::cerr<<"Error opening ROOTFile "<<filename<<".\n"<<endl;
   return 0;
 }
 std::cout<<"Successfully opened ROOTFile "<<filename<<".\n"<<endl;
 
 TTree *mps_tree = f->Get("Mps_Tree");
 TTree *hel_tree = f->Get("Hel_Tree");
  
 gStyle->SetOptStat("neMmRr");
 gStyle->SetStatW(0.32);
 
 
 TCanvas *c1 = new TCanvas("c1", "pmtltg and pmtonly",1400,1100);
 c1->Divide(2,3);
 
 c1->cd(1);
 Mps_Tree->Draw("qwk_charge:mps_counter","ErrorFlag==0");
 c1->cd(3);
 Mps_Tree->Draw("qwk_pmtltg:mps_counter","qwk_pmtltg.Device_Error_Code==0&&ErrorFlag==0");
 c1->cd(5);
 Mps_Tree->Draw("qwk_pmtonl:mps_counter","qwk_pmtonl.Device_Error_Code==0&&ErrorFlag==0");
 
 c1->cd(4);
 Hel_Tree->Draw("1e6*asym_qwk_pmtltg","asym_qwk_pmtltg.Device_Error_Code==0&&ErrorFlag==0");
 c1->cd(6);
 Hel_Tree->Draw("1e6*asym_qwk_pmtonl","asym_qwk_pmtonl.Device_Error_Code==0&&ErrorFlag==0");


 TCanvas *c2 = new TCanvas("c2", "MD9",1400,1100);
 c2->Divide(2,3);

 c2->cd(1);
 Mps_Tree->Draw("qwk_charge:mps_counter","ErrorFlag==0");
 c2->cd(3);
 Mps_Tree->Draw("qwk_md9neg:mps_counter","qwk_md9neg.Device_Error_Code==0&&ErrorFlag==0");
 c2->cd(5);
 Mps_Tree->Draw("qwk_md9pos:mps_counter","qwk_md9pos.Device_Error_Code==0&&ErrorFlag==0");
 
 c2->cd(4);
 Hel_Tree->Draw("1e6*asym_qwk_md9neg","asym_qwk_md9neg.Device_Error_Code==0&&ErrorFlag==0");
 c2->cd(6);
 Hel_Tree->Draw("1e6*asym_qwk_md9pos","asym_qwk_md9pos.Device_Error_Code==0&&ErrorFlag==0");
} // end of macro


