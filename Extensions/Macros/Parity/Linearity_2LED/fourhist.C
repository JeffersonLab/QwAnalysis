#include <TProfile.h>
#include <TTree.h>
#include <TCut.h>
#include <TMath.h>
#include <TDirectory.h>
#include <iostream>

const double mps_freq = 960; // Hz

/*
 * Use profile histograms to find one-second averages for data with
 * the far LED on and off.  Find DC and AC amplitudes for the signal.
 */
TH1** fourhist(TTree* t, TString signal, TCut aclo, TCut achi) {
  int nbins = t->GetMaximum("time");

  const TString ts_aclo = Form("%s_aclo", signal.Data());
  const TString ts_achi = Form("%s_achi", signal.Data());
  const TString ts_ac   = Form("%s_ac"  , signal.Data());
  const TString ts_dc   = Form("%s_dc"  , signal.Data());
  const char* s_aclo = ts_aclo.Data();
  const char* s_achi = ts_achi.Data();

  const char* fmt = "%s : time >> %s(%d,0,%d)";

  t->Draw( Form(fmt, signal.Data(), s_aclo, nbins, nbins),
	   aclo, "prof, goff");
  t->Draw( Form(fmt, signal.Data(), s_achi, nbins, nbins),
	   achi, "prof, goff");

  TProfile* lo = (TProfile*)gDirectory->FindObject(s_aclo);
  TProfile* hi = (TProfile*)gDirectory->FindObject(s_achi);
  TH1F* dc = new TH1F(ts_dc, ts_dc, nbins,0,nbins);
  TH1F* ac = new TH1F(ts_ac, ts_ac, nbins,0,nbins);

  if (!(lo||hi||dc||ac)) return 0;

  for (int i=0; i<= nbins+1; ++i) {
    double l = lo->GetBinContent(i);
    double h = hi->GetBinContent(i);
    double e = TMath::Hypot( lo->GetBinError(i), hi->GetBinError(i) );

    dc->SetBinContent(i, (h+l)/2);
    ac->SetBinContent(i, (h-l));

    dc->SetBinError(i, e);
    ac->SetBinError(i, e);
  }

  TH1** results = new TH1*[4];
  results[0] = lo;
  results[1] = hi;
  results[2] = dc;
  results[3] = ac;
  return results;
}

/*
 * Now take those signals, computed using profile histograms, and put
 * them into a tree so they can be plotted against each other.
 * Include a leaf "mpsb" so that event number cuts will work.
 */
TTree* extrema(TH1** h, TString signal) {
  if (!gDirectory->IsWritable()) {
    std::cerr << "Error: current directory '" << gDirectory->GetName()
	      << "' is not writeable.\n";
    return 0;
  }

  double time[2];
  double val[4][2];
  TTree* e = new TTree(Form("%s_extrema",signal.Data()),"");
  if (!e) return 0;

  e->Branch("time",time,"time/D:mpsb/D");
  for (int i=0; i<4; ++i) {
    const char* name = h[i]->GetName();
    e->Branch(name, val[i], "value/D:error/D");
  }

  for (int t=0; t<= h[0]->GetNbinsX()+1; ++t) {
    time[0] = h[0]->GetBinLowEdge( t );
    time[1] = time[0] * mps_freq;

    for (int i=0; i<4; ++i) {
      val[i][0] = h[i]->GetBinContent( t );
      val[i][1] = h[i]->GetBinError  ( t );
    }

    e->Fill();
  }

  return e;
}

TTree* extrema(TTree* t, TString signal, TCut aclo, TCut achi) {
  TH1** h = fourhist(t,signal,aclo,achi);
  if (!h) return 0;
  TTree* e = extrema(h,signal);
  delete[] h;
  return e;
}
