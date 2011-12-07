{
  /*
   * extrema.C
   *
   * Fits to AC hi, AC low parts of each range of events in a
   * linearity test.
   */
  TTree* e = new TTree("extrema","fits to far LED on/off data");
  e->Branch("mpsrange",mpsrange,"first/D:last/D");

  TCut mpscut;
  e->Branch("mpscut",&mpscut);

  int signals = 0;
  while (all_sig[++signals]); // this line counts the names in the list.

  /*
   * Each signal gets two fits: to the low samples and to the high
   * samples.  From these two values, extract meanlo and meanhi; then
   * also store meandc = (meanhi+meanlo)/2 and meanac = (meanhi-meanlo)/2.
   */
  TFitResult fit_hi[signals], fit_lo[signals];
  double mean[signals][4];
  for (int i=0; i<signals; ++i) {
    e->Branch(all_sig[i], mean[i], "meandc/D:meanac/D:meanlo/D:meanhi/D");
    e->Branch( Form("%s_fit_hi", all_sig[i]), fit_hi+i );
    e->Branch( Form("%s_fit_lo", all_sig[i]), fit_lo+i );
  }

  int current = 0;
  while (dcstable->GetEntry(current++)) {
    mpscut = Form("%g < mpsb && mpsb < %g", mpsrange[0], mpsrange[1]);
    for (int i=0; i<signals; ++i) {
      cout << "cut " << current << ", " << all_sig[i] << endl;

      TString hl = Form("hist_lo_%d_%d", i, current);
      TString hh = Form("hist_hi_%d_%d", i, current);

      t->Draw( Form("%s >> %s(4096,0,10)", all_sig[i], hl.Data()),
	       mpscut && aclo, "goff");
      t->Draw( Form("%s >> %s(4096,0,10)", all_sig[i], hh.Data()),
	       mpscut && achi, "goff");

      TH1F* hist_lo = (TH1F*)gDirectory->FindObjectAny(hl.Data());
      TH1F* hist_hi = (TH1F*)gDirectory->FindObjectAny(hh.Data());

      fit_lo[i] = *hist_lo->Fit("gaus","S");
      fit_hi[i] = *hist_hi->Fit("gaus","S");

      mean[i][3] = fit_hi[i].Parameters()[1];
      mean[i][2] = fit_lo[i].Parameters()[1];

      mean[i][0] = (mean[i][3] + mean[i][2])/2;
      mean[i][1] = (mean[i][3] - mean[i][2])/2;

      hist_lo->Delete();
      hist_hi->Delete();
    }
    cout << "done with cut " << current << endl;
    e->Fill();
  }
}
