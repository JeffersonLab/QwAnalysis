#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TCut.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <cmath>

struct result {
  double ac_baseline  , ac_amplitude  , dc_baseline  , dc_amplitude  ;
  double ac_baseline_e, ac_amplitude_e, dc_baseline_e, dc_amplitude_e;
};

void add_result_to_file(char* filename, result* r) {
  // logic to determine whether the file exists already, and add a
  // header to a new file, would be cute here
  std::ofstream f(filename, std::ios_base::app);
  if (!f) { cout << __LINE__ << "\n"; return; }
  f << r->ac_baseline << "\t"
    << r->ac_amplitude << "\t"
    << r->dc_baseline << "\t"
    << r->dc_amplitude << "\t"
    << r->ac_baseline_e << "\t"
    << r->ac_amplitude_e << "\t"
    << r->dc_baseline_e << "\t"
    << r->dc_amplitude_e << "\t"
    << "\n";
  f.close();
  cout << "Wrote a line to " << filename << "\n";
}

TCut* guess_stable_cuts(TTree* s,
			double scandata1,
			double min_time = 0,
			double max_time = 0,
			double max_stable_std = 0,
			double max_ac_off = 0 ) {
  TCut when = Form("(cleandata) && (scandata1 == %g)", scandata1);
  if (min_time) when = ( when && Form("%g < time", min_time) );
  if (max_time) when = ( when && Form("time < %g", max_time) );

  TH1F* stable_cuts = new TH1F("stable_cuts","stable_cuts",(1<<18),-10,10);
  if (!max_stable_std) {
    s->Draw("std >> stable_cuts",when,"goff");
    max_stable_std = stable_cuts->GetMean();
  }
  if (!max_ac_off) {
    s->Draw("pmt_ac >> stable_cuts",when,"goff");
    max_ac_off = stable_cuts->GetMean();
  }
  stable_cuts->Delete();

  TCut stable = Form("std < %g", max_stable_std);
  TCut ac_on  = Form("%g < pmt_ac", max_ac_off);

  TCut* cuts = new TCut[3];
  cuts[0] = when;
  cuts[1] = stable;
  cuts[2] = ac_on;

  return cuts;
}

result show_stable_cuts(TTree* s, TCut* cuts){
  TCut when = cuts[0];
  TCut stable = cuts[1];
  TCut ac_on  = cuts[2];
  TCanvas* c = (TCanvas*)gDirectory->FindObject("c_cuts");
  if (c) c->Close();
  c = new TCanvas("c_cuts","c_cuts", 800, 800);
  c->Divide(2,3);

  char* histnames[] = { "s_all", "s_unstable", "s_off", "s_on",
			"ac_all", "ac_unstable", "ac_off", "ac_on",
			"dc_all", "dc_unstable", "dc_off", "dc_on",
			0 };
  int nh = 0; while ( histnames[++nh] ) {} // nh = length(histnames)
  TH1F* h[nh];
  for (int i = 0; i < nh; ++i ) {
    h[i] = (TH1F*)gDirectory->FindObject( histnames[i] );
    if (h[i]) h[i]->Delete();
  }

  char* hist2names[] = { "vs_time", "vs_std",
			 "dc_vs_time",
			 0 };
  int nhh = 0; while ( hist2names[++nhh] ) {} // nhh = length(hist2names)
  TH2F* hh[nhh];
  for (int i = 0; i < nhh; ++i ) {
    hh[i] = (TH2F*)gDirectory->FindObject( hist2names[i] );
    if (hh[i]) hh[i]->Delete();
  }

  c->cd(3);
  s->Draw("pmt_ac : time >> vs_time", when, "colz");

  c->cd(2);
  s->Draw("pmt_ac : std >> vs_std", when, "colz");

  c->cd(4);
  gPad->SetLogy(1);
  s->Draw("std >> s_all",	when,				"");
  s->Draw("std >> s_unstable",	when && !stable,		"same");
  s->Draw("std >> s_off",	when && stable && !ac_on,	"same");
  s->Draw("std >> s_on",	when && stable && ac_on,	"same");

  c->cd(1);
  gPad->SetLogy(1);
  s->Draw("pmt_ac >> ac_all",	when,				"");
  s->Draw("pmt_ac >> ac_unstable", when && !stable,		"same");
  s->Draw("pmt_ac >> ac_off",	when && stable && !ac_on,	"same");
  s->Draw("pmt_ac >> ac_on",	when && stable && ac_on,	"same");

  c->cd(5);
  gPad->SetLogy(1);
  s->Draw("pmt_dc >> dc_all",	when,				"");
  s->Draw("pmt_dc >> dc_unstable", when && !stable,		"same");
  s->Draw("pmt_dc >> dc_off",	when && stable && !ac_on,	"same");
  s->Draw("pmt_dc >> dc_on",	when && stable && ac_on,	"same");

  // Make the 1D histograms have colors in the same pattern everywhere
  for (int i = 0; i < nh; ++i ) {
    h[i] = (TH1F*)gDirectory->FindObject( histnames[i] );
    if (!h[i]) continue;
    h[i]->SetLineColor((i%4)+1);
  }

  // Why on earth do the color changes not happen if this code goes
  // before the for loop?  Ugh.
  c->cd(6);
  s->Draw("pmt_dc : time >> dc_vs_time", when, "colz");

  // oh, there has GOT to be a better way to do name resolution than this shit.
  TH1F *h_ac_on = h[7],  *h_ac_off = h[6];
  TH1F *h_dc_on = h[11], *h_dc_off = h[10];

  result r;

  r.ac_baseline	= h_ac_off->GetMean();
  r.ac_baseline_e	= h_ac_off->GetMeanError();
  r.ac_amplitude	= h_ac_on->GetMean() - h_ac_off->GetMean();
  r.ac_amplitude_e = hypot(h_ac_on->GetMeanError(), h_ac_off->GetMeanError());

  r.dc_baseline	= h_dc_off->GetMean();
  r.dc_baseline_e	= h_dc_off->GetMeanError();
  r.dc_amplitude	= h_dc_on->GetMean() - h_dc_off->GetMean();
  r.dc_amplitude_e = hypot(h_dc_on->GetMeanError(), h_dc_off->GetMeanError());

  return r;
}
