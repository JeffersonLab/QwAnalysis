#include <TTree.h>
#include <TMath.h>
#include <TRandom2.h>
#include <TDatime.h>
#include <TDirectory.h>
#include <iostream>
#include <limits>

/*
 * running_rms.C
 * Rob Mahurin <rob@jlab.org> 2011-03-02
 *
 * Compute a "moving rms" (and other statistics) for a signal stored
 * in a tree.  Return a TTree suitable for adding as a Friend.  The
 * number of points used for the statistics is nsample; if nsample is
 * odd, then the n-th entry in the moving stats is computed from a
 * window centered around the n-th entry in the source tree.
 *
 * These functions assume that all leaves in the branches of interest
 * are doubles.
 */

/*
TTree* running_rms(TTree* serial, TString branchname, TString leafname,
		   unsigned int nsample = 3) {
  // Not implemented.  You can get another leaf if you know its number
  // in the sequence.
  return 0;
}
*/

TTree* running_rms(TTree* serial, TString branchname,
		   int nsample = 3, int leaf = 0) {
  if (nsample<3) { cout << __LINE__ << "\n"; return 0; }

  if (!gDirectory->IsWritable()) {
    std::cerr << "Error: current directory '" << gDirectory->GetName()
	      << "' is not writeable.\n";
    { cout << __LINE__ << "\n"; return 0; }
  }

  /*
   * What will we call the new tree when we make it?
   */
  TString new_treename = Form("rms%d_%s", nsample, branchname.Data());

  /*
   * Find the value we want in the tree.
   */
  TBranch* b = serial->GetBranch(branchname);
  if (!b) {
    // hmmm, maybe the branch is known by an alias.
    TList* loa = serial->GetListOfAliases();
    if (!loa) { cout << __LINE__ << "\n"; return 0; } // nope, no aliases
    for (int i = 0; i < loa->GetEntries(); ++i) {
      TString aliasname = loa->At(i)->GetName();
      TString aliastarget = loa->At(i)->GetTitle();
      if (aliasname == branchname) {
	branchname = aliastarget;
	b = serial->GetBranch(branchname);
	break;
      }
    }
    if (!b) { cout << __LINE__ << "\n"; return 0; } // nope, not found
  }
  if (b->GetNleaves() <= leaf)
    { cout << __LINE__ << "\n"; return 0; } // not enough leaves on this branch
  double data[ b->GetNleaves() ];
  double *x = &(data[leaf]);
  serial->SetBranchAddress(branchname, data);


  /*
   * A circular buffer to do the statistics.
   */
  double circ[ nsample ];

  /*
   * Contain the statistics in this new, friendable tree.
   */
  TTree* v = new TTree(new_treename.Data(),new_treename.Data());
  if (!v) { cout << __LINE__ << "\n"; return 0; }
  if (leaf) {
    TObjArray* list = b->GetListOfLeaves();
    v->SetName( Form("%s_%s", v->GetName(), list->At(leaf)->GetName()) );
    list->Delete();
  }
  double mean, std, diff;
  if (! (v->Branch("mean", &mean, "mean/D")
	 && v->Branch("std", &std, "std/D")
	 && v->Branch("diff", &diff, "diff/D")
	 ) ) { cout << __LINE__ << "\n"; return 0; }

  /*
   * Do the processing.
   */
  TDatime start, last;
  start.Print();
  TRandom2 rand(0);
  Long64_t current = 0, tree_entries = serial->GetEntries();

  // This many entries at start and end have incomplete statistics.
  int oneside = (nsample-1)/2;
  mean = std = diff = std::numeric_limits<double>::quiet_NaN();
  for (int i=0; i<oneside; ++i) v->Fill();

  // Fill the buffer
  while (current < nsample && serial->GetEntry(current++))
    circ[ current%nsample ] = *x;

  // Go through the tree
  do {
    circ[ current%nsample ] = *x;

    mean = TMath::Mean(nsample, circ);
    std  = TMath::RMS( nsample, circ);
    diff = circ[ (current-oneside+1) %nsample ]
         - circ[ (current-oneside-1) %nsample ];
    v->Fill();

    // No program this slow should run with no progress indicator.
    TDatime now;
    if (rand.Uniform() < 1e-2 &&
	now.Convert() - last.Convert() >= 10) {
      std::cout << Form("\rDone %lld of %lld (%.0f%%) at %06d\n",
			current, tree_entries, 100.0*current/tree_entries,
			now.GetTime() )	;
      last = now;
    }
  } while (serial->GetEntry(current++));

  // Make the trees the same size
  mean = std = diff = std::numeric_limits<double>::quiet_NaN();
  while (v->GetEntries() < serial->GetEntries()) v->Fill();

  /*
   * Clean up and return.
   */
  return v;
}
