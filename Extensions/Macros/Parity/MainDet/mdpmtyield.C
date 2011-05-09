void mdpmtyield(  bool use_Hel_Tree = kTRUE,
		  bool plot_both_tubes = kTRUE ) {
  cout << __FILE__ << " start at " << TDatime(time(0)).GetTime() << "\n";

  if (!use_Hel_Tree) {
    cerr << "Sorry, that's broken\n"; // and slow, when it works.
    return;
  }

  double position[9] = {0, 4, 1,2,3, 6, 9,8,7};
  char* tube[3] = {"barsum", "pos", "neg"};

  int firsttube = 1, lasttube = 2;
  if (!plot_both_tubes) firsttube = lasttube = 0;

  TCanvas* c1 = new TCanvas;
  c1->Divide(3,3);

  for (int det = 1; det <= 8; ++det) {
    TVirtualPad* csub = c1->cd(position[det]);
    csub->Divide(1,2);
    for (int t = firsttube; t <= lasttube; ++t) {
      csub->cd(t);
      Hel_Tree -> Draw
	( Form( " yield_qwk_md%d%s : pattern_number >> md%d%s(1024) ", 
		det, tube[t], det, tube[t] ), 
	  Form( " !yield_qwk_md%d%s.Device_Error_Code ", det, tube[t] ), 
	  "colz") ;
    }
    // c1->Update();
    cout << "Drew detector " << det
	 << " at " << TDatime(time(0)).GetTime()
	 << "\n"; 
  }
  c1->cd(5);
  // FIXME: should come from Hel_Tree
  Mps_Tree -> Draw( "qwk_bcm1 : mps_counter >> histbcm1(128,0,0,128) " ,
		    "qwk_bcm1 > 0.001 ",
		    "colz" );

  cout << __FILE__ << " end at " << TDatime(time(0)).GetTime() << "\n";
}
