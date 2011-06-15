{
  const char* rootfile = "serialize.10234.root";
  const char* cutfile = "md9p_stable_10234.txt";

  const char* ac_sig = "qwk_md9neg";
  const char* ac_std = Form("rms3_%s.std", ac_sig);

  // list all signals to analyze here.  Ugh.
  const char* all_sig[] = { ac_sig,
			    "qwk_md9pos",
			    0 };

  double siglo = 0.5;
  double sighi = 3;

  double stdlo = 0.1;
  double stdhi = 1.2;

  // This assumes that the right running_rms tree has been saved in
  // the file as a friend.

  // "clean" cuts
  TCut aclo = Form( "%s < %g && %s < %g", ac_sig, siglo, ac_std, stdlo);
  TCut achi = Form( "%s > %g && %s < %g", ac_sig, sighi, ac_std, stdlo);
  TCut acmid= Form( "%g < %s && %s < %g && %s > %g",
		    siglo, ac_sig, ac_sig, sighi, ac_std, stdhi );
  // "dirty" points, points with changing neigbors
  TCut aclod = Form( "%s < %g && %s > %g", ac_sig, siglo, ac_std, stdlo);
  TCut achid = Form( "%s > %g && %s > %g", ac_sig, sighi, ac_std, stdlo);
  // shouldn't have anything in the middle of those plots
  TCut acbad= Form( "%g < %s && %s < %g && %g < %s && %s < %g",
		    siglo, ac_sig, ac_sig, sighi,
		    stdlo, ac_std, ac_std, stdhi );

  TFile f(rootfile,"READ");
  TTree* t = (TTree*)gDirectory->FindObjectAny("Mps_Tree_serial");

  TTree *dcstable = new TTree;
  dcstable->ReadFile(cutfile);
  double mpsrange[2];
  dcstable->SetBranchAddress("first",mpsrange);
  dcstable->SetBranchAddress("last",mpsrange+1);
}
