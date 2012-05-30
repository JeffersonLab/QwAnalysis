{
  int run_number, dc_pmt;
  if (!run_number) {
    run_number	= 18995;
    dc_pmt	= +7;
  }
  char* pmt_dc = StrDup( Form("qwk_md%d%s", abs(dc_pmt),
			      ( dc_pmt > 0 ? "pos" : "neg" ) ) );
  char* pmt_ac = StrDup( Form("qwk_md%d%s", abs(dc_pmt),
			      ( dc_pmt > 0 ? "neg" : "pos" ) ) );

  cout << "Getting ready to process pmt " << pmt_dc
       << " from run " << run_number << "\n";

  gROOT->LoadMacro("serialize.C+");
  gROOT->LoadMacro("running_rms.C+");
  gROOT->LoadMacro("all_scandata1.C+");
  gROOT->LoadMacro("show_stable_cuts.C+");

  char* canvas_fname_fmt = "run%d.dc%02g.plots.pdf";

  // strdup() these to avoid the rotten 4k circular buffer in TString::Form().
  char* txt_fname = StrDup( Form( "run%d.%s.txt",
				  run_number, pmt_dc ) );
  char* treepattern = StrDup( Form( "$QW_ROOTFILES/Qweak_%d.*.trees.root",
				    run_number) );
  char* serialized_tree = StrDup( Form( "run%d.Mps_Tree_serial.root",
					run_number ) );

  TFile *f;
  f = new TFile(serialized_tree,"read");
  if (!f || !f->IsOpen()) {
    TChain* mps = new TChain("Mps_Tree");
    int nfiles = mps->Add( treepattern );
    if (!nfiles) return cout << "couldn't add any files to chain\n";
    TTree* s = serialize(mps, Form( "%s %s", pmt_ac, pmt_dc ) );
    if (!s) return cout << "serialize failed.\n";
    s->SetAlias("pmt_ac", pmt_ac);
    s->SetAlias("pmt_dc", pmt_dc);

    // Something about the new ROOTfile created by serialize() is
    // invalid, but gets repaired when the file is closed.  So, here is
    // an annoying kludge: close it and open it again.
    gDirectory->Write(); // there was a stealth path change in serialize()
    gDirectory->Close();
    rename( "Mps_Tree_serial.root", serialized_tree );
  } else {
    f->Close();
  }
  f = new TFile( serialized_tree,"update");
  if (!f) return cout << "failed at line " << __LINE__ << "\n";

  TTree* s   = (TTree*) f->FindObjectAny("Mps_Tree_serial");
  TTree* rms = (TTree*) f->FindObjectAny("rms3_pmt_ac");
  if (!s) return cout << "failed at line " << __LINE__ << "\n";
  if (!rms) {
    rms = running_rms(s, "pmt_ac");
    if (!rms) return cout << "failed at line " << __LINE__ << "\n";
    f->Write();
  }

  if ( (!s->GetListOfFriends()) ||
       (!s->GetListOfFriends()->Contains( rms->GetName() )) ) {
    s->AddFriend(rms);
    f->Write();
  }

  #include <set>
  std::set<Double_t> scanpoints = all_scandata1( s );
  print_set( scanpoints );

  #include <unistd.h>
  if (0 == unlink( txt_fname )) {
    cout << "Clobbered old output file " << txt_fname << "\n";
  }

  for( std::set<Double_t>::iterator it = scanpoints.begin();
       it != scanpoints.end(); ++it ) {
    cout << "Drawing for scandata == " << *it << "\n";
    TCut* cuts = guess_stable_cuts(s, *it);
    result r = show_stable_cuts(s, cuts);
    TCanvas* c = gPad->GetCanvas();
    if (!c) return cout << "failed at line " << __LINE__ << "\n";
    c->SaveAs( Form( canvas_fname_fmt, run_number, *it ) );
    add_result_to_file( txt_fname, &r );
  }
}
