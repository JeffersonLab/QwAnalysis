/*
 * bcm_diffs.C
 * Analysis by John Leacock, some automation by Rob Mahurin 
 * 2010-12-11
 */
bcm_diffs(const int run = 8100, 
	  const int first = -1, const int last = -1) {

 const char* file = "QwPass1_%i.*.root";

 TChain chain("Hel_Tree");
 if (0 == chain.Add(Form(file, run)) ) {
   cout << "No matches to " << Form(file, run) << "\n";
   return;
 }

 TCut cut("ErrorFlag==0");
 if (-1 != first) cut = cut && TCut( Form("%d<pat_counter",first) );
 if (-1 != last)  cut = cut && TCut( Form("pat_counter<%d",last) );

 const char* tc1 = "BCM asymmetry differences, run %d";
 const char* tc2 = "BCM yield vs. pattern counter, run %d";
 const char* tc3 = "BCM asymmetries, run %d";
 const char* tc4 = "BCM 1 vs. BCM 2, run %d";

 TCanvas *c1 = new TCanvas("c1",Form(tc1,run),0,0,1024,768);
c1->Divide(2,3);
c1->cd(1);
chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm2)*1e6",cut);
c1->cd(2);
chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm5)*1e6",cut);
c1->cd(3);
chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm6)*1e6",cut);
c1->cd(4);
chain . Draw("(asym_qwk_bcm2-asym_qwk_bcm5)*1e6",cut);
c1->cd(5);
chain . Draw("(asym_qwk_bcm2-asym_qwk_bcm6)*1e6",cut);
c1->cd(6);
chain . Draw("(asym_qwk_bcm5-asym_qwk_bcm6)*1e6",cut);


 TCanvas *c2 = new TCanvas("c2",Form(tc2,run),0,0,1024,768);
c2->Divide(2,2);
c2->cd(1);
chain . Draw("yield_qwk_bcm1:pat_counter",cut);
c2->cd(2);
chain . Draw("yield_qwk_bcm2:pat_counter",cut);
c2->cd(3);
chain . Draw("yield_qwk_bcm5:pat_counter",cut);
c2->cd(4);
chain . Draw("yield_qwk_bcm6:pat_counter",cut);

 TCanvas *c3 = new TCanvas("c3",Form(tc3,run),0,0,1024,768);
c3->Divide(2,2);
c3->cd(1);
chain . Draw("asym_qwk_bcm1*1e6",cut);
c3->cd(2);
chain . Draw("asym_qwk_bcm2*1e6",cut);
c3->cd(3);
chain . Draw("asym_qwk_bcm5*1e6",cut);
c3->cd(4);
chain . Draw("asym_qwk_bcm6*1e6",cut);

 TCanvas *c4 = new TCanvas("c4",Form(tc4,run),0,0,1024,768);
gStyle->SetOptFit(1);
c4->Divide(1,2);
c4->cd(1);
chain . Draw("asym_qwk_bcm1:asym_qwk_bcm2",cut,"");
c4->cd(2);

chain . Draw("asym_qwk_bcm1:asym_qwk_bcm2>>h1",cut,"prof");
//h1->Fit("pol1","","",-0.02e-3,0.02e-3);
 TProfile *h1 = gDirectory->FindObject("h1");
 double fitlo = h1->GetMean() - h1->GetRMS();
 double fithi = h1->GetMean() + h1->GetRMS();
 h1->Fit("pol1","","", fitlo, fithi);

 // c1 -> SaveAs("c1.png"); c2 -> SaveAs("c2.png"); c3 -> SaveAs("c3.png"); c4 -> SaveAs("c4.png"); 
}
