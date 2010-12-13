/*
 * bcm_diffs.C
 * Analysis by John Leacock, some automation by Rob Mahurin 
 * 2010-12-11
 */
bcm_diffs(const int run = 8100, 
	  const int first = -1, const int last = -1) {

 const char* file = "$QW_ROOTFILES/QwPass1_%i.*.root";

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
 const char* tc5 = "BCM & BPM asymmetry differences, run %d";
 const char* tc6 = "BCM & BPM correlations, run %d";
 const char* tc7 = "BCM & BPM correlations vs. pattern counter, run %d";



 TCanvas *c4 = new TCanvas("c4",Form(tc4,run),0,0,1024,768);
gStyle->SetOptFit(1);
c4->Divide(1,2);
c4->cd(1);
chain . Draw("asym_qwk_bcm1:asym_qwk_bcm2",cut,"");
c4->cd(2);
chain . Draw("asym_qwk_bcm1:asym_qwk_bcm2>>h1",cut,"prof");
//h1->Fit("pol1","","",-0.02e-3,0.02e-3);
 TProfile *h1 = gDirectory->FindObject("h1");
 double fitlo = h1->GetMean() - 2*h1->GetRMS();
 double fithi = h1->GetMean() + 2*h1->GetRMS();
 h1->Fit("pol1","","", fitlo, fithi);

 TCanvas *c6 = new TCanvas("c6",Form(tc6,run),0,0,1024,768);
 TProfile *myprof;
 c6->Divide(2,5);
 c6->cd(1);
 chain . Draw("asym_qwk_bcm1:asym_qwk_bpm3h09b_EffectiveCharge",cut);
 c6->cd(2);
 chain . Draw("asym_qwk_bcm1:asym_qwk_bpm3h09b_EffectiveCharge>>prof1",cut,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof1");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(3);
 chain . Draw("asym_qwk_bcm2:asym_qwk_bpm3h09b_EffectiveCharge",cut);
 c6->cd(4);
 chain . Draw("asym_qwk_bcm2:asym_qwk_bpm3h09b_EffectiveCharge>>prof2",cut,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof2");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(5);
 chain . Draw("asym_qwk_bcm5:asym_qwk_bpm3h09b_EffectiveCharge",cut);
 c6->cd(6);
 chain . Draw("asym_qwk_bcm5:asym_qwk_bpm3h09b_EffectiveCharge>>prof5",cut,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof5");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(7);
 chain . Draw("asym_qwk_bcm6:asym_qwk_bpm3h09b_EffectiveCharge",cut);
 c6->cd(8);
 chain . Draw("asym_qwk_bcm6:asym_qwk_bpm3h09b_EffectiveCharge>>prof6",cut,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof6");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(9);
 chain . Draw("asym_qwk_bpm3c12_EffectiveCharge:asym_qwk_bpm3h09b_EffectiveCharge",cut);
 c6->cd(10);
 chain . Draw("asym_qwk_bpm3c12_EffectiveCharge:asym_qwk_bpm3h09b_EffectiveCharge>>profbpm",cut,"prof");
 myprof = (TProfile*)gDirectory->FindObject("profbpm");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);

 TCanvas *c1 = new TCanvas("c1",Form(tc1,run),0,0,1024,768);
 std::cout << "                                 \t RMS\t"
	   << std::endl;
 c1->Divide(2,3);
 c1->cd(1);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm2)*1e6>>h11",cut);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bcm2)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h11"))->GetRMS()
	   << std::endl;
 c1->cd(2);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm5)*1e6>>h12",cut);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bcm5)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h12"))->GetRMS()
	   << std::endl;
 c1->cd(3);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm6)*1e6>>h13",cut);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bcm6)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h13"))->GetRMS()
	   << std::endl;
 c1->cd(4);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bcm5)*1e6>>h14",cut);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bcm5)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h14"))->GetRMS()
	   << std::endl;
 c1->cd(5);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bcm6)*1e6>>h15",cut);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bcm6)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h15"))->GetRMS()
	   << std::endl;
 c1->cd(6);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bcm6)*1e6>>h16",cut);
 std::cout << "(asym_qwk_bcm5-asym_qwk_bcm6)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h16"))->GetRMS()
	   << std::endl;

 TCanvas *c5 = new TCanvas("c5",Form(tc5,run),0,0,1024,768);
 c5->Divide(2,5);
 c5->cd(1);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h51",cut);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h51"))->GetRMS()
	   << std::endl;
 c5->cd(2);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h52",cut);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h52"))->GetRMS()
	   << std::endl;
 c5->cd(3);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h53",cut);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h53"))->GetRMS()
	   << std::endl;
 c5->cd(4);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h54",cut);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h54"))->GetRMS()
	   << std::endl;
 c5->cd(5);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h55",cut);
 std::cout << "(asym_qwk_bcm5-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h55"))->GetRMS()
	   << std::endl;
 c5->cd(6);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h56",cut);
 std::cout << "(asym_qwk_bcm5-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h56"))->GetRMS()
	   << std::endl;
 c5->cd(7);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h57",cut);
 std::cout << "(asym_qwk_bcm6-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h57"))->GetRMS()
	   << std::endl;
 c5->cd(8);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h58",cut);
 std::cout << "(asym_qwk_bcm6-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h58"))->GetRMS()
	   << std::endl;
 c5->cd(10);
 chain . Draw("(asym_qwk_bpm3h09b_EffectiveCharge-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h59",cut);
 std::cout << "(asym_qwk_bpm3h09b_EffectiveCharge-asym_qwk_bpm3c12_EffectiveCharge)*1e6"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h59"))->GetRMS()
	   << std::endl;

 TCanvas *c7 = new TCanvas("c7",Form(tc7,run),0,0,1024,768);
 c7->Divide(2,5);
 c7->cd(1);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut);
 c7->cd(2);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut);
 c7->cd(3);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut);
 c7->cd(4);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut);
  c7->cd(5);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut);
 c7->cd(6);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut);
  c7->cd(7);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut);
 c7->cd(8);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut);
 c7->cd(10);
 chain . Draw("(asym_qwk_bpm3h09b_EffectiveCharge-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut);


 TCanvas *c2 = new TCanvas("c2",Form(tc2,run),0,0,1024,768);
c2->Divide(2,3);
c2->cd(1);
chain . Draw("yield_qwk_bcm1:pat_counter",cut);
c2->cd(2);
chain . Draw("yield_qwk_bcm2:pat_counter",cut);
c2->cd(3);
chain . Draw("yield_qwk_bcm5:pat_counter",cut);
c2->cd(4);
chain . Draw("yield_qwk_bcm6:pat_counter",cut);
c2->cd(5);
chain . Draw("yield_qwk_bpm3h09b_EffectiveCharge:pat_counter",cut);
c2->cd(6);
chain . Draw("yield_qwk_bpm3c12_EffectiveCharge:pat_counter",cut);

 TCanvas *c3 = new TCanvas("c3",Form(tc3,run),0,0,1024,768);
c3->Divide(2,3);
c3->cd(1);
chain . Draw("asym_qwk_bcm1*1e6",cut);
c3->cd(2);
chain . Draw("asym_qwk_bcm2*1e6",cut);
c3->cd(3);
chain . Draw("asym_qwk_bcm5*1e6",cut);
c3->cd(4);
chain . Draw("asym_qwk_bcm6*1e6",cut);
c3->cd(5);
chain . Draw("asym_qwk_bpm3h09b_EffectiveCharge*1e6",cut);
c3->cd(6);
chain . Draw("asym_qwk_bpm3c12_EffectiveCharge*1e6",cut);



 c1 -> SaveAs("bcmdiffs_1.png");
 c2 -> SaveAs("bcmdiffs_2.png");
 c3 -> SaveAs("bcmdiffs_3.png");
 c4 -> SaveAs("bcmdiffs_4.png"); 
 c5 -> SaveAs("bcmdiffs_5.png"); 
 c6 -> SaveAs("bcmdiffs_6.png"); 
 c7 -> SaveAs("bcmdiffs_7.png"); 
}
