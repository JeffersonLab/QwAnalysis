
void showCuts()
{
  TPaveText *pt = new TPaveText(0.05,0.05,0.95,0.95,"brNDC");
//   pt->AddText("Trying this");
  TTree *rawtree=NULL, *pairtree=NULL, *multiplettree=NULL;;
  //rawtree= (TTree*)gROOT->FindObject("R");
  pairtree= (TTree*)gROOT->FindObject("P");
  multiplettree= (TTree*)gROOT->FindObject("M");
  if(pairtree==NULL || multiplettree==NULL) {
    if(pairtree==NULL) {
      pt->AddText("NO pair tree");
    }
    if(multiplettree==NULL) {
      pt->AddText("NO multiplet tree");
    }
    pt->Draw();
    return;
  }
  //rawtree->Refresh();
  pairtree->Refresh();
  multiplettree->Refresh();
  //Long64_t total_ev = rawtree->GetEntries();
  Long64_t total = pairtree->GetEntries();
  Long64_t total_m = pairtree->GetEntries();
  if(total==0 || total_m==0) { 
    if(total==0) {
      pt->AddText("NO pairs");
    }
    if (total_m==0) {
      pt->AddText("NO multiplets");
    }
    pt->Draw();
    return;
  }
  Long64_t n_lobeam = pairtree->Draw("ok_cut","cut_low_beam[1]||cut_low_beam[0]", "goff");
  Long64_t n_ok_cut = pairtree->Draw("ok_cut","ok_cut","goff");
  Long64_t n_ok_cut_m = multiplettree->Draw("ok_cut","ok_cut","goff");
  //Long64_t n_detsat_cut = pairtree->Draw("ok_cut",
  //				  "cut_det_saturate[1]||cut_det_saturate[0]",
  //				  "goff");
  // Monitors will saturate when beam is ramping.. 
  //  remove the beam ramping from that tally
  TString moncut = "(cut_mon_saturate[1]||cut_mon_saturate[0])";
  moncut += "&&(cut_low_beam[1]==0||cut_low_beam[0]==0)";
  moncut += "&&(cut_beam_burp[1]==0||cut_beam_burp[0]==0)";
  Long64_t n_monsat_cut = 
    (Double_t)pairtree->Draw("ok_cut",
			     moncut,
			     "goff");

  TString burpcut = "(cut_beam_burp[1]||cut_beam_burp[0])";
  burpcut += "&&(cut_low_beam[1]==0||cut_low_beam[0]==0)";
  Long64_t n_burp_cut = 
    (Double_t)pairtree->Draw("ok_cut",
			     burpcut,
			     "goff");


  Long64_t n_helicity_cut = (Double_t)pairtree->Draw("ok_cut",
				  "cut_pair_seq[1]||cut_pair_seq[0]",
				  "goff");

  pairtree->Draw("avg_bcm1>>hcurrent(100)","ok_cut","goff");
  Double_t current = hcurrent->GetMean()/1700;
  Double_t charge = current*n_ok_cut/120000;


  //pt->AddText(Form("Total Events (no cuts) = %d",total_ev));
  pt->AddText(Form("Average Current = %.1f uA",current));
  pt->AddText(Form("Total Charge = %.1f mC",charge));
  pt->AddText(Form("Total Pairs = %d (no cuts)",total));
  //  pt->AddText(Form("***** OK pairs = %d *****",n_ok_cut));
  pt->AddText(Form("OK pairs = %.0f (%.1f\%)",(double)n_ok_cut,
		   100.0*(double)n_ok_cut/(double)total));
  pt->AddText(Form("** OK multiplets ** = %.0f (%.1f\%)",(double)n_ok_cut_m,
		   100.0*(double)n_ok_cut/(double)total));
  pt->AddText("-------- Pair Cut Info --------");
  pt->AddText(Form("Low Beam = %.0f (%.1f\%)",(double)n_lobeam,
		   100.0*(double)n_lobeam/(double)total));
  pt->AddText(Form("BeamBurp = %.0f (%.1f\%)",(double)n_burp_cut,
		   100.0*(double)n_burp_cut/(double)total));
  TText *sattext = pt->AddText(Form("BPM saturated = %.0f (%.1f\%)",(double)n_monsat_cut,
		   100.0*(double)n_monsat_cut/(double)total));
  if (n_monsat_cut>0) {
    sattext->SetTextColor(kRed);
    TText *sattext1 = pt->AddText(Form("Phone MCC to release gains!"));
    sattext1->SetTextColor(kRed);
  }
  else sattext->SetTextColor(kBlack);
  pt->AddText(Form("Helicity Errors = %.0f (%.1f\%)",(double)n_helicity_cut,
		   100.0*(double)n_helicity_cut/(double)total));
  pt->AddText("");
  pt->Draw();


}
