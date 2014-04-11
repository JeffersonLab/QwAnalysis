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
#include <THStack.h>

#include <QwEvent.h>
#include <QwHit.h>

#include "PlotLib.h"

using namespace std;
using std::vector;

//CodaEventType (1=mps, 2=TS)
void event_counter(int runNumber, double eventType=1, int eventLow=0, int eventHigh=4e6) {
  gROOT -> Reset();
  gROOT -> SetStyle("Modern");

  //open rootfile
  TString filename = Form("Qweak_%i.root",runNumber);
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
//  double eventType    = 1;    //CodaEventType (1=mps, 2=TS)
  double delta        = 0;      //delta is the number of triggers skipped if
                                //not starting at event 0
  bool debug = false;
  TString trigger = Form("sca_trig0%i",int(eventType));

  //grab the tree and set all the addresses we want
  TTree   *tree   = (TTree*) file->Get("Mps_Tree");
  tree->ResetBranchAddresses();
  TBranch *sca_trig = (TBranch*) tree->GetBranch(trigger.Data());
  TLeaf *trig = sca_trig->GetLeaf("value");
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


void livetime_counter(int runNum, double eventType=1, int eventLow=0, int eventHigh=4e6) {
  gROOT -> Reset();
  gROOT -> SetStyle("Modern");

  //open rootfile
  TString filename = Form("Qweak_%i.original.root",runNum);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cout <<"Error opening ROOTFILE " << file->GetName() <<std::endl;
    return;
  }
  else {
    std::cout <<"Successfully opened ROOTFILE " <<file->GetName() <<std::endl;
  }

  std::vector<double> liveVec;
  std::vector<double> totalVec;
  std::vector<double> ratioVec;

  bool debug = false;
  TString trigger = Form("sca_trig0%i",int(eventType));

  //grab the tree and set all the addresses we want
  TTree   *tree   = (TTree*) file->Get("Mps_Tree");
  tree->ResetBranchAddresses();
  TLeaf *type = tree->GetLeaf("CodaEventType");

  TBranch *sca_livetime = (TBranch*) tree->GetBranch("sca_livetime");
  TLeaf *livetime = sca_livetime->GetLeaf("value");

  TBranch *sca_totaltime = (TBranch*) tree->GetBranch("sca_totaltime");
  TLeaf *totaltime = sca_totaltime->GetLeaf("value");

  int n_entries  = (int) tree->GetEntries();  //get the number of entries

  if (debug) printf("Events recorded \tEventType \ttotal\n");
  eventHigh = (n_entries<eventHigh) ? n_entries : eventHigh;
  for(int j=0; j<n_entries; j++) {
    tree->GetEntry(j);
    sca_livetime->GetEntry(j);
    sca_totaltime->GetEntry(j);
    if (j<eventLow) {continue;}
    if (j>eventHigh) {break;}
    if (j % 100000==0) {
      printf("Processing event #%i out of %i\n",j,n_entries);
    }

    //ignore if not the appropriate trigger
    if (eventType!=type->GetValue()) {
      continue;
    }

    if( livetime->GetValue() != 0 && totaltime->GetValue() != 0 ) {
      liveVec.push_back(livetime->GetValue());
      totalVec.push_back(totaltime->GetValue());
      ratioVec.push_back(  (livetime->GetValue()/totaltime->GetValue() ) );
    }

  }

  TH1F *hLive = new TH1F("rLive","title",400,-100e3,300e3);
  TH1F *hTotal = new TH1F("rTotal","title",400,-100e3,300e3);
  TH1F *hRatio = new TH1F("hRatio","title",200,0.0,1.1);
//  TH1F *hCompare = new TH1F("hCompare","title",400,-100e3,300e3);

  hLive->FillN(liveVec.size(),liveVec.data(),NULL);
  hTotal->FillN(totalVec.size(),totalVec.data(),NULL);

  blueHisto(hLive);
  redHisto(hTotal);

  THStack *stack = new THStack("stack","title");
  stack->Add(hLive,"sames");
  stack->Add(hTotal,"sames");

  TCanvas *c1 = new TCanvas("c1");
  c1->cd();
  stack->Draw("nostack");

  gPad->SetLogy();
  stack->SetTitle(Form("Run %i: Livetime scaler clock {events>%i && events<%i}",runNum,eventLow,eventHigh));
  stack->GetXaxis()->SetTitle("sca_livetime (counts)");

  gPad->Modified();
  gPad->Update();

  hRatio->FillN(ratioVec.size(),ratioVec.data(),NULL);

  blueHisto(hRatio);

  TCanvas *c2 = new TCanvas("c2");
  c2->cd();
  hRatio->Draw();
  hRatio->GetXaxis()->SetTitle("Calculated livetime(unitless)");
  gPad->Modified();
  gPad->Update();


}



