#include <TTree.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TString.h>
#include <TRandom2.h>
#include <iostream>

/*
 * serialize.C
 * Rob Mahurin <rob@jlab.org> 2011-02-27
 *
 * These functions read in a several branches from a tree where ADC
 * information is stored in sub-banks, and create a new tree where
 * each shorter ADC reading is a seperate entry.
 *
 * The code requires that all leaves in the branches of interest be
 * stored in doubles.
 */

TTree* serialize (const char* tree_name, TString branch_list);
TTree* serialize (TTree* tree, TString branch_list);

/*
 * Configuration information for converting to physical values.
 * It'd be nicer to get this information from the data, but I'll cross
 * that bridge when I come to it.
 */
const double mps_freq = 960; // Hz
const double adc_freq = 500e3; // Hz; adc sample time is 2 µs
const double num_samples = 116; // now this one _is_ in the data.  But
				// in practice these three change together.
// 18 bit ADC, 20 V range, re-accumulated
const double volts_per_channel = 20./(1<<18) /num_samples;

TTree* serialize (const char* tree_name, TString branch_list) {
  TTree* tree = (TTree*)gDirectory->FindObject(tree_name);
  return serialize(tree, branch_list);
}

TTree* serialize (TTree* tree, TString branch_list) {
  if (!tree) {cout << __LINE__ << "\n"; return 0; }

  /*
   * branch_list is a list of whitespace-separated branch names.
   */
  // This TObjArray must be deleted when we are done with it.
  TObjArray* toaBranchList = branch_list.Tokenize(" ");
  int nbranch = toaBranchList->GetEntries();
  if (!nbranch) {cout << __LINE__ << "\n"; return 0; }

  /*
   * Set up the trees for copying.
   */
  tree->SetBranchStatus("*",0);
  TFile* file = new TFile(Form("%s_serial.root", tree->GetName()), "recreate");
  if (!file->IsOpen()) {cout << __LINE__ << "\n"; return 0; }
  TTree* serial = new TTree(Form("%s_serial", tree->GetName()),
			    Form("%s_serial", tree->GetName()));
  if (!serial) {cout << __LINE__ << "\n"; return 0; }

  /*
   * Set up data variables for reading from the old tree and writing
   * to the new tree.
   */
  const int blocks_to_serialize = 4;
  TString leaves_to_serialize[ blocks_to_serialize ] =
    { "block0_raw", "block1_raw", "block2_raw", "block3_raw", };

  // how far in the list are the blocks we'll serialize?
  int lts_offset[ nbranch ];

  double *leaves[ nbranch ];	// store the data from the old tree
  double vadc[ nbranch ];	// store the data for the new tree

  double mps_counter, mpsb, time;	// position in the run.
  tree->SetBranchStatus("mps_counter",1);
  tree->SetBranchAddress("mps_counter",&mps_counter);
  serial->Branch("mpsb",&mpsb,"mpsb/D");
  serial->Branch("time",&time,"time/D");

  double cleandata, scandata1, scandata2; // data from green monster
  tree->SetBranchStatus("cleandata",1);
  tree->SetBranchAddress("cleandata",&cleandata);
  tree->SetBranchStatus("scandata1",1);
  tree->SetBranchAddress("scandata1",&scandata1);
  tree->SetBranchStatus("scandata2",1);
  tree->SetBranchAddress("scandata2",&scandata2);
  serial->Branch("cleandata",&cleandata,"cleandata/D");
  serial->Branch("scandata1",&scandata1,"scandata1/D");
  serial->Branch("scandata2",&scandata2,"scandata2/D");

  /*
   * Set up the branches in the old and new trees.
   *
   * We really only want a few leaves from each branch.  However we
   * apparently can't use GetLeaf() across files in a TChain --- at
   * least, that crashed, and there is a comment in the manual about
   * using chain->SetBranchAddress() rather than branch->SetAddress(),
   * so I am reasoning by analogy.
   *
   * The rest of this crap is because someday I'll probably use this
   * one some strange tree with a different leaf list, so it's more
   * efficient in the long run to let the code find the leaves than to
   * count the offset myself.
   */
  for (int b=0; b < nbranch; ++b) {
    // Thank God for a clear and straightforward typing system!
    TString branchname = ((TObjString*)toaBranchList->At(b))->GetString();
    tree->SetBranchStatus(branchname, 1);
    TBranch* branch = tree->GetBranch(branchname);
    if (!branch) {cout << __LINE__ << "\n"; return 0; }

    // How many leaves?  Make sure there's space to read them all in.
    leaves[b] = new double[ branch->GetNleaves() ];
    if (!leaves[b]) {cout << __LINE__ << "\n"; return 0; }

    // Okay, now we can set up the data addresses.
    tree->SetBranchAddress(branchname, leaves[b]);
    serial->Branch(branchname, vadc+b, "vadc/D");
    serial->Branch(branchname + "_full", leaves[b], branch->GetTitle());

    /*
     * A second thing to be done once for each branch: Where in the
     * data array does the data to be serialized start?
     */
    TObjArray* leaflist = branch->GetListOfLeaves(); // no delete needed
    int l;
    for (l=0; l < branch->GetNleaves(); ++l) {
      if (leaves_to_serialize[0] == leaflist->At(l)->GetName()) break;
    }
    lts_offset[b] = l;
    if (l == branch->GetNleaves()) {
      cout << "error at " << __LINE__ << ": "
	   << "branch \"" << branch->GetName() << "\" "
	   << "has no leaves to serialize"
	   << "\n";
      return 0;
    }
    for (l=0; l < blocks_to_serialize; ++l) {
      if (leaves_to_serialize[l] !=
	  leaflist->At(lts_offset[b] + l)->GetName()) {
	// The data we want aren't consecutive.  When this happens,
	// write code to deal with it.  For now, bail out.
	{cout << __LINE__ << "\n"; return 0; }
      }
    }
  }

  /*
   * Now start processing the tree.  Print the time before going
   * through the data (n.b. GetEntries() goes through the data).
   */
  TDatime start, last;
  start.Print();
  TRandom2 rand(0);
  Long64_t current = 0, tree_entries = tree->GetEntries();

  while (tree->GetEntry(current++)) {
    // Each entry in "tree" becomes several entries in "serial"
    for (int block=0; block < blocks_to_serialize; ++block) {
      mpsb = mps_counter + block/4.;
      time = mps_counter/mps_freq + block/adc_freq*num_samples;
      for (int branch=0; branch < nbranch; ++branch) {
	vadc[branch] = leaves[branch][lts_offset[branch] + block]
	  * volts_per_channel;
      }
      serial->Fill();
    }

    // No program this slow should run with no progress indicator.
    TDatime now;
    if (rand.Uniform() < 1e-2 &&
	now.Convert() - last.Convert() >= 10) {
      std::cout << Form("\rDone %lld of %lld (%.0f%%) at %06d\n",
			current, tree_entries, 100.0*current/tree_entries,
			now.GetTime() )	;
      last = now;
    }

    /*
     * Write to disk once per runlet or so --- don't store too much in memory.
     *
     * Not clear whether this is necessary.  If so, kOverWrite is
     * warranted (also for the final write, below).
     */
    // if (rand.Uniform() < 1./400e3) serial->Write("",TObject::kOverwrite);
  }
  // Done processing the chain.  Final write.
  serial->Write("",TObject::kOverwrite);

  /*
   * Undo all the mess we made.
   */
  tree->SetBranchStatus("*",1);
  for (int b=0; b < nbranch; ++b) delete[] leaves[b];
  toaBranchList->Delete();

  /*
   * All done.
   */
  return serial;
}
