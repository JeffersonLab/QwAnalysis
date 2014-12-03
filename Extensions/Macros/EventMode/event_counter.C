//magee@jlab.org
//Jan 30, 2014

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TStyle.h>

//CodaEventType (1=mps, 2=TS)
void event_counter(int runNumber, double eventType=1, int eventLow=0, int eventHigh=4e6) {
  gROOT -> Reset();
  gROOT -> SetStyle("Modern");

  //open rootfile
  TString filename = Form("QweakRun2_%i.root",runNumber);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cout <<"Error opening ROOTFILE " << file->GetName() <<std::endl;
    return;
  }
  else {
    std::cout <<"Successfully opened ROOTFILE " <<file->GetName() <<std::endl;
  }

  int events_total    = 0;  //number of recorded events
  int events_recorded = 0;  //number of recorded events
  double delta        = 0;      //delta is the number of triggers skipped if
                                //not starting at event 0
  bool debug = false;
  TString trigger1 = "sca_trig01";
  TString trigger2 = "sca_trig02";

  //grab the tree and set all the addresses we want
  TTree   *tree   = (TTree*) file->Get("Mps_Tree");
  tree->ResetBranchAddresses();
  TBranch *sca_trig1 = (TBranch*) tree->GetBranch(trigger1.Data());
  TBranch *sca_trig2 = (TBranch*) tree->GetBranch(trigger2.Data());
  TLeaf *trig1 = sca_trig->GetLeaf("value");
  TLeaf *trig2 = sca_trig->GetLeaf("value");
  TLeaf *type = tree->GetLeaf("CodaEventType");

  int n_entries  = (int) tree->GetEntries();  //get the number of entries

  /* We want to compare the number of mps events recorded
   * with the total number of mps events in all.
   * The general idea is to count each mps trigger and
   * at the end number_of_triggers/total counter
   *
   * To account for beam trips at the beginning of the run,
   * the "offset" is delta. For instance, lets say there is a
   * beam trip from events 1-2000. The total events counted would
   * be (total_number-delta), with delta=2000.
   */
  if (debug) printf("Events recorded \tEventType \ttotal\n");
  eventHigh = (n_entries<eventHigh) ? n_entries : eventHigh;
  for(int j=0; j<n_entries; j++) {
    tree->GetEntry(j);
    sca_trig->GetEntry(j);
    if (j<eventLow) {continue;}
    if (j>eventHigh) {break;}
    if (j % 100000==0) {
      printf("Processing event #%i out of %i\n",j,n_entries);
    }

    //ignore if not the appropriate trigger
    if (eventType!=type->GetValue()) {
      continue;
    }

    /* delta must be defined as the first good event of its type.
     * Therefore, it must be determined *before* any previous
     * events are recorded. delta=0 if we start with event 0.
     * Lastly, if delta>0, it's already been determined.
     *
     * I know I don't need to explicitly put eventLow && delta==0
     * in my if statement, but I wanted it logically clear what I was
     * doing and why I was doing it.
     */
    if (events_recorded==0 && eventLow!=0 && delta==0)
    { delta = trig->GetValue(); }
    events_recorded++;
    events_total = trig->GetValue();
    if (debug)
      printf("%i \t\t%i \t\t%d\n",events_recorded,int(eventType),events_total);
  }
  events_total = events_total-delta;
  double quotient = double(events_recorded)/events_total;
  std::cout <<"Events recorded: " <<events_recorded <<" \t"
    <<"Total: " <<events_total <<" \t" <<std::endl
    <<"LT estimate: " <<quotient <<" \t"
    <<"for EventType = " <<eventType <<std::endl;
}



