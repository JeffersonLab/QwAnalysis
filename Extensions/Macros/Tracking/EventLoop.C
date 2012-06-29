#include <fstream>
#include <iostream>

#include "TSystem.h"
#include "TChain.h"
#include "TF1.h"

#include "QwEvent.h"

Int_t oct1 = 5;
Int_t oct2 = 5;

Bool_t (*SelectEvent)(const QwEvent*, const Double_t*) = 0;
void  (*ProcessEvent)(const QwEvent*) = 0;

Bool_t (*SelectTrack)(const QwTrack*, const Double_t*) = 0;
void  (*ProcessTrack)(const QwTrack*) = 0;


// Set the select event function
void SetSelectEvent(Bool_t (*f)(const QwEvent*, const Double_t*)) {
  SelectEvent = f;
}

void EventLoop(Int_t run) {

  // Create and load the chain
  TChain *event_tree = new TChain("event_tree");
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",run));

  // Get octant
  //event_tree->Draw("events.fQwPartialTracks.fOctant>>h","events.fQwPartialTracks.fRegion==2 && events.fQwPartialTracks.fPackage==2");
  //oct2 = Int_t(h->GetMean());
  //oct1 = (oct2 + 4) % 8;

  // Setup event branch
  QwEvent* event = 0;
  event_tree->SetBranchStatus("events",1);
  event_tree->SetBranchAddress("events",&event);

  // Setup maindet branch
  Double_t maindet[4];
  event_tree->SetBranchAddress(Form("maindet.md%dm_f1",oct1),&maindet[0]);
  event_tree->SetBranchAddress(Form("maindet.md%dp_f1",oct1),&maindet[1]);
  event_tree->SetBranchAddress(Form("maindet.md%dm_f1",oct2),&maindet[2]);
  event_tree->SetBranchAddress(Form("maindet.md%dp_f1",oct2),&maindet[3]);


  // Loop over events
  for (int i = 0; i < event_tree->GetEntries(); i++) {

    // Get event
    event_tree->GetEntry(i);

    // Select event
    if (SelectEvent && SelectEvent(event,maindet)) {

      if (ProcessEvent) ProcessEvent(event);

      // Loop over tracks
      for (int j = 0; j < event->GetNumberOfTracks(); j++) {
        const QwTrack* track = event->GetTrack(j);

        // Select track
        if (SelectTrack && SelectTrack(track,maindet)) {

          if (ProcessTrack) ProcessTrack(track);

        } // end of select of tracks

      } // end of loop over tracks

    } // end of select of events

  } // end of loop over events

}
