/*
 * bcm_diffs.C
 * Analysis by John Leacock, some automation by Rob Mahurin 
 * 2010-12-11
 * Added device error codes to the bcms and bpms 01-18-2011 Rakitha (rakithab)
 */
bcm_diffs(const int run = 8100, 
	  const int first = -1, const int last = -1) {

  char* file;
  int posrun;
  if (run < 0) {
    file = "$QW_ROOTFILES/first100k_%i.root";
    posrun=-1*run;
  } else {
    file = "$QW_ROOTFILES/QwPass1_%i.000.root";
    posrun=run;
  }

 TChain chain("Hel_Tree");

 if (0 == chain.Add(Form(file, posrun)) ) {
   cout << "No matches to " << Form(file, posrun) << "\n";
   return;
 }
     

 TCut cut("ErrorFlag==0");
 if (-1 != first) cut = cut && TCut( Form("%d<pat_counter",first) );
 if (-1 != last)  cut = cut && TCut( Form("pat_counter<%d",last) );

 TCut cut_bcm1("asym_qwk_bcm1.Device_Error_Code==0");//Device Error code is same for asym and yield
 TCut cut_bcm2("asym_qwk_bcm2.Device_Error_Code==0");//Device Error code is same for asym and yield
 TCut cut_bcm5("asym_qwk_bcm5.Device_Error_Code==0");//Device Error code is same for asym and yield
 TCut cut_bcm6("asym_qwk_bcm6.Device_Error_Code==0");//Device Error code is same for asym and yield
 TCut cut_bpm3h09b("asym_qwk_bpm3h09b_EffectiveCharge.Device_Error_Code==0");//Device Error code is same for asym and yield
 TCut cut_bpm3c12("asym_qwk_bpm3c12_EffectiveCharge.Device_Error_Code==0");//Device Error code is same for asym and yield

 const char* tc1 = "BCM asymmetry differences, run %d";
 const char* tc2 = "BCM yield vs. pattern counter, run %d";
 const char* tc3 = "BCM asymmetries, run %d";
 const char* tc4 = "BCM 1 vs. BCM 2, run %d";
 const char* tc5 = "BCM & BPM asymmetry differences, run %d";
 const char* tc6 = "BCM & BPM correlations, run %d";
 const char* tc7 = "BCM & BPM correlations vs. pattern counter, run %d";



 TCanvas *c4 = new TCanvas("c4",Form(tc4,posrun),0,0,1024,768);
gStyle->SetOptFit(1);
c4->Divide(1,2);
c4->cd(1);
chain . Draw("asym_qwk_bcm1:asym_qwk_bcm2",cut && cut_bcm1 && cut_bcm2,"");
c4->cd(2);
chain . Draw("asym_qwk_bcm1:asym_qwk_bcm2>>h1",cut && cut_bcm1 && cut_bcm2,"prof");
//h1->Fit("pol1","","",-0.02e-3,0.02e-3);
 TProfile *h1 = gDirectory->FindObject("h1");
 double fitlo = h1->GetMean() - 2*h1->GetRMS();
 double fithi = h1->GetMean() + 2*h1->GetRMS();
 h1->Fit("pol1","","", fitlo, fithi);

 TCanvas *c6 = new TCanvas("c6",Form(tc6,posrun),0,0,1024,768);
 TProfile *myprof;
 c6->Divide(2,5);
 c6->cd(1);
 chain . Draw("asym_qwk_bcm1:asym_qwk_bpm3h09b_EffectiveCharge",cut && cut_bcm1 && cut_bpm3h09b);
 c6->cd(2);
 chain . Draw("asym_qwk_bcm1:asym_qwk_bpm3h09b_EffectiveCharge>>prof1",cut && cut_bcm1 && cut_bpm3h09b,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof1");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(3);
 chain . Draw("asym_qwk_bcm2:asym_qwk_bpm3h09b_EffectiveCharge",cut && cut_bcm2 && cut_bpm3h09b);
 c6->cd(4);
 chain . Draw("asym_qwk_bcm2:asym_qwk_bpm3h09b_EffectiveCharge>>prof2",cut && cut_bcm2 && cut_bpm3h09b,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof2");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(5);
 chain . Draw("asym_qwk_bcm5:asym_qwk_bpm3h09b_EffectiveCharge",cut && cut_bcm5 && cut_bpm3h09b);
 c6->cd(6);
 chain . Draw("asym_qwk_bcm5:asym_qwk_bpm3h09b_EffectiveCharge>>prof5",cut && cut_bcm5 && cut_bpm3h09b,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof5");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(7);
 chain . Draw("asym_qwk_bcm6:asym_qwk_bpm3h09b_EffectiveCharge",cut && cut_bcm6 && cut_bpm3h09b);
 c6->cd(8);
 chain . Draw("asym_qwk_bcm6:asym_qwk_bpm3h09b_EffectiveCharge>>prof6",cut && cut_bcm6 && cut_bpm3h09b,"prof");
 myprof = (TProfile*)gDirectory->FindObject("prof6");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);
 c6->cd(9);
 chain . Draw("asym_qwk_bpm3c12_EffectiveCharge:asym_qwk_bpm3h09b_EffectiveCharge",cut  && cut_bpm3h09b && cut_bpm3c12);
 c6->cd(10);
 chain . Draw("asym_qwk_bpm3c12_EffectiveCharge:asym_qwk_bpm3h09b_EffectiveCharge>>profbpm",cut && cut_bpm3h09b && cut_bpm3c12,"prof");
 myprof = (TProfile*)gDirectory->FindObject("profbpm");
 double fitlo = myprof->GetMean() - 2*myprof->GetRMS();
 double fithi = myprof->GetMean() + 2*myprof->GetRMS();
 myprof->Fit("pol1","","", fitlo, fithi);

 TCanvas *c1 = new TCanvas("c1",Form(tc1,posrun),0,0,1024,768);
 std::cout << "                                 \t RMS\t"
	   << std::endl;
 c1->Divide(2,3);
 c1->cd(1);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm2)*1e6>>h11",cut && cut_bcm2 && cut_bcm1);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bcm2)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h11"))->GetRMS()
	   << std::endl;
 c1->cd(2);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm5)*1e6>>h12",cut && cut_bcm1 && cut_bcm5);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bcm5)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h12"))->GetRMS()
	   << std::endl;
 c1->cd(3);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bcm6)*1e6>>h13",cut && cut_bcm1 && cut_bcm6);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bcm6)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h13"))->GetRMS()
	   << std::endl;
 c1->cd(4);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bcm5)*1e6>>h14",cut && cut_bcm2 && cut_bcm5);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bcm5)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h14"))->GetRMS()
	   << std::endl;
 c1->cd(5);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bcm6)*1e6>>h15",cut && cut_bcm2 && cut_bcm6 );
 std::cout << "(asym_qwk_bcm2-asym_qwk_bcm6)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h15"))->GetRMS()
	   << std::endl;
 c1->cd(6);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bcm6)*1e6>>h16",cut && cut_bcm5 && cut_bcm6);
 std::cout << "(asym_qwk_bcm5-asym_qwk_bcm6)*1e6\t" 
	   << ((TH1*)gDirectory->FindObject("h16"))->GetRMS()
	   << std::endl;

 TCanvas *c5 = new TCanvas("c5",Form(tc5,posrun),0,0,1024,768);
 c5->Divide(2,5);
 c5->cd(1);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h51",cut && cut_bcm1 && cut_bpm3h09b);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h51"))->GetRMS()
	   << std::endl;
 c5->cd(2);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h52",cut && cut_bcm1 && cut_bpm3c12);
 std::cout << "(asym_qwk_bcm1-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h52"))->GetRMS()
	   << std::endl;
 c5->cd(3);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h53",cut && cut_bcm2 && cut_bpm3h09b);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h53"))->GetRMS()
	   << std::endl;
 c5->cd(4);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h54",cut && cut_bcm2 && cut_bpm3c12);
 std::cout << "(asym_qwk_bcm2-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h54"))->GetRMS()
	   << std::endl;
 c5->cd(5);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h55",cut && cut_bcm5 && cut_bpm3h09b);
 std::cout << "(asym_qwk_bcm5-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h55"))->GetRMS()
	   << std::endl;
 c5->cd(6);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h56",cut && cut_bcm5 && cut_bpm3c12);
 std::cout << "(asym_qwk_bcm5-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h56"))->GetRMS()
	   << std::endl;
 c5->cd(7);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3h09b_EffectiveCharge)*1e6>>h57",cut && cut_bcm6 && cut_bpm3h09b);
 std::cout << "(asym_qwk_bcm6-asym_qwk_bpm3h09b_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h57"))->GetRMS()
	   << std::endl;
 c5->cd(8);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h58",cut && cut_bcm6  && cut_bpm3c12);
 std::cout << "(asym_qwk_bcm6-asym_qwk_bpm3c12_EffectiveCharge)*1e6" 
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h58"))->GetRMS()
	   << std::endl;
 c5->cd(10);
 chain . Draw("(asym_qwk_bpm3h09b_EffectiveCharge-asym_qwk_bpm3c12_EffectiveCharge)*1e6>>h59",cut && cut_bpm3h09b && cut_bpm3c12);
 std::cout << "(asym_qwk_bpm3h09b_EffectiveCharge-asym_qwk_bpm3c12_EffectiveCharge)*1e6"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h59"))->GetRMS()
	   << std::endl;

 TCanvas *c7 = new TCanvas("c7",Form(tc7,posrun),0,0,1024,768);
 c7->Divide(2,5);
 c7->cd(1);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm1 && cut_bpm3h09b);
 c7->cd(2);
 chain . Draw("(asym_qwk_bcm1-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm1 && cut_bpm3c12);
 c7->cd(3);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm2 && cut_bpm3h09b);
 c7->cd(4);
 chain . Draw("(asym_qwk_bcm2-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm2 && cut_bpm3c12);
  c7->cd(5);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm5 && cut_bpm3h09b);
 c7->cd(6);
 chain . Draw("(asym_qwk_bcm5-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm5 && cut_bpm3c12);
  c7->cd(7);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3h09b_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm6 && cut_bpm3h09b);
 c7->cd(8);
 chain . Draw("(asym_qwk_bcm6-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut && cut_bcm6 && cut_bpm3c12);
 c7->cd(10);
 chain . Draw("(asym_qwk_bpm3h09b_EffectiveCharge-asym_qwk_bpm3c12_EffectiveCharge)*1e6:pattern_number",cut && cut_bpm3h09b && cut_bpm3c12);


 TCanvas *c2 = new TCanvas("c2",Form(tc2,posrun),0,0,1024,768);
c2->Divide(2,3);
c2->cd(1);
chain . Draw("yield_qwk_bcm1:pat_counter>>h60",cut && cut_bcm1);
 std::cout << "yield_qwk_bcm1"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h60"))->GetMean(2)
	   << std::endl;
c2->cd(2);
chain . Draw("yield_qwk_bcm2:pat_counter>>h61",cut && cut_bcm2);
 std::cout << "yield_qwk_bcm2"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h61"))->GetMean(2)
	   << std::endl;
c2->cd(3);
chain . Draw("yield_qwk_bcm5:pat_counter>>h62",cut && cut_bcm5);
 std::cout << "yield_qwk_bcm5"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h62"))->GetMean(2)
	   << std::endl;
c2->cd(4);
chain . Draw("yield_qwk_bcm6:pat_counter>>h63",cut && cut_bcm6);
 std::cout << "yield_qwk_bcm6"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h63"))->GetMean(2)
	   << std::endl;
c2->cd(5);
chain . Draw("yield_qwk_bpm3h09b_EffectiveCharge:pat_counter>>h64",cut && cut_bpm3h09b);
 std::cout << "yield_qwk_bpm3h09b_EffectiveCharge"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h64"))->GetMean(2)
	   << std::endl;
c2->cd(6);
chain . Draw("yield_qwk_bpm3c12_EffectiveCharge:pat_counter>>h65",cut && cut_bpm3c12);
 std::cout << "yield_qwk_bpm3c12_EffectiveCharge"
	   << "\t"
	   << ((TH1*)gDirectory->FindObject("h65"))->GetMean(2)
	   << std::endl;

 TCanvas *c3 = new TCanvas("c3",Form(tc3,posrun),0,0,1024,768);
c3->Divide(2,3);
c3->cd(1);
chain . Draw("asym_qwk_bcm1*1e6",cut && cut_bcm1);
c3->cd(2);
chain . Draw("asym_qwk_bcm2*1e6",cut && cut_bcm2);
c3->cd(3);
chain . Draw("asym_qwk_bcm5*1e6",cut && cut_bcm5);
c3->cd(4);
chain . Draw("asym_qwk_bcm6*1e6",cut && cut_bcm6);
c3->cd(5);
chain . Draw("asym_qwk_bpm3h09b_EffectiveCharge*1e6",cut && cut_bpm3h09b);
c3->cd(6);
chain . Draw("asym_qwk_bpm3c12_EffectiveCharge*1e6",cut && cut_bpm3c12);



 c1 -> SaveAs("bcmdiffs_1.png");
 c2 -> SaveAs("bcmdiffs_2.png");
 c3 -> SaveAs("bcmdiffs_3.png");
 c4 -> SaveAs("bcmdiffs_4.png"); 
 c5 -> SaveAs("bcmdiffs_5.png"); 
 c6 -> SaveAs("bcmdiffs_6.png"); 
 c7 -> SaveAs("bcmdiffs_7.png"); 
}
