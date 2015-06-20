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

/* Function to determine the livetime for different trigger types in the
 * QwAnalysis framework. Previous versions available in the QwAnalysis revision
 * repository allow one to specify and select the trigger type. This revised
 * script only focuses on EventTypes 1 and 2, which are for the MPS and Trigger
 * Scintillator triggered events only.
 */
void event_counter2(int runNumber, double eventLow=0, double eventHigh=4e6) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");

  //open rootfile
  TString filename = Form("Qweak_%i.root",runNumber);
  TFile *file = new TFile(filename);
  if ( file->IsOpen() ) {
    std::cout <<"Successfully opened ROOTFILE " <<file->GetName() <<std::endl;
  }
  else {
    std::cout <<"Error opening ROOTFILE " << file->GetName() <<std::endl;
    return;
  }

  /* We are interested in determining the total number of events for a given
   * CodaEventRange. We are interested in both MPS and trigger scintillator
   * event types - these are CodaEventType==1 and ==2, respectively.
   * To do this we need 3 things for each type:
   *    first_event  == first event
   *    last_event   == last event
   *    num_recorded == number of events recorded
   *    tot_events   == total number of events
   * Convention:
   *    Append each of the above with _type1 (MPS) or _type2 (TS)
   */

  double first_event_type1  = 0;
  double last_event_type1   = 0;
  double num_recorded_type1 = 0;
  double tot_events_type1   = 0;

  double first_event_type2  = 0;
  double last_event_type2   = 0;
  double num_recorded_type2 = 0;
  double tot_events_type2   = 0;

  double eventType = 0;

  double clock_first = 0;
  double clock_last  = 0;

  //grab the TTree and configure TLeaf's
  TTree *tree = (TTree*) file->Get("Mps_Tree");
  tree->ResetBranchAddresses();
  TBranch *trig01_branch = (TBranch*) tree->GetBranch("sca_trig01");
  TBranch *trig02_branch = (TBranch*) tree->GetBranch("sca_trig02");
  TBranch *time_branch   = (TBranch*) tree->GetBranch("sca_totaltime");
  TLeaf *trig01 = trig01_branch->GetLeaf("value");
  TLeaf *trig02 = trig02_branch->GetLeaf("value");
  TLeaf *type   = tree->GetLeaf("CodaEventType");
  TLeaf *clock  = time_branch->GetLeaf("raw");

  int n_entries = (int) tree->GetEntries(); //number of events
  for(int j=eventLow; j<n_entries; j++) {
    if (j<eventLow) {continue;}
    if (j>eventHigh) {break;}
    if (j % 100000==0) {
      printf("Processing event #%i out of %i\n",j,n_entries);
    }

    tree->GetEntry(j);
    trig01_branch->GetEntry(j);
    trig02_branch->GetEntry(j);
    time_branch->GetEntry(j);

    //determine event trigger type
    eventType = type->GetValue();
    //we only care about these two eventTypes
//    if (eventType!=1 && eventType!=2) {
//      continue;
//    }


    if (eventType==1) {
      //if the first_event was never filled, this must be the first event!
      if (first_event_type1==0) {
        first_event_type1 = trig01->GetValue();
        if (clock_first==0)
          clock_first = clock->GetValue();
      }
      //we can keep repopulating the last event - it's the easiest way to
      //determine the last event of this type
      last_event_type1 = trig01->GetValue();
      num_recorded_type1++; //increment the number of recorded events
    }

    if (eventType==2) {
      //if the first_event was never filled, this must be the first event!
      if (first_event_type2==0) {
        first_event_type2 = trig02->GetValue();
        if (clock_first ==0)
          clock_first = clock->GetValue();
      }
      //we can keep repopulating the last event - it's the easiest way to
      //determine the last event of this type
      last_event_type2 = trig02->GetValue();
      num_recorded_type2++; //increment the number of recorded events
    }

    clock_last = clock->GetValue();

  } //end loop over events

  tot_events_type1 = last_event_type1 - first_event_type1;
  double livetime_type1 = double(num_recorded_type1)/tot_events_type1;

  tot_events_type2 = last_event_type2 - first_event_type2;
  double livetime_type2 = double(num_recorded_type2)/tot_events_type2;

  printf("MPS events: \t first: \t%.f\n",first_event_type1);
  printf("\t\t last: \t\t%.f\n",last_event_type1);
  printf("\t\t recorded: \t%.f\n",num_recorded_type1);
  printf("\t\t total num: \t%.f\n",tot_events_type1);
  printf("\t\t MPS LT: \t%f\n\n",livetime_type1);

  printf("TS events: \t first: \t%.f\n",first_event_type2);
  printf("\t\t last: \t\t%.f\n",last_event_type2);
  printf("\t\t recorded: \t%.f\n",num_recorded_type2);
  printf("\t\t total num: \t%.f\n",tot_events_type2);
  printf("\t\t TS LT: \t%f\n\n",livetime_type2);

  printf("Time estimate:   mps: \t%.2f\n",double(tot_events_type1)/10);
  printf("Clock: \t\t first: %.f\n",clock_first);
  printf("\t\t last: \t%.f\n",clock_last);
  printf("\t\t diff: \t%.f\n",double(clock_last-clock_first));
  printf("\t\t time (sec): \t%.5f\n\n",double(clock_last-clock_first)/195480);

} //end event_counter function




