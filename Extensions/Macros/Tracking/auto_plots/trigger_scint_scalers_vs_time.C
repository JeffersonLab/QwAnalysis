/***********************************************************
 Programmer: Valerie Gray
 Purpose: To graph the two trigger scintillator scaler values vs time as event number
 Entry Conditions: the run number
 Date: 02-03-2012
 Modified:07-13-2012
 Assisted By: Wouter Deconinck
 *********************************************************/

#include "auto_shared_includes.h"
#include "TH2D.h"
#include "TLeaf.h"
#include "QwEvent.h"

void trigger_scint_scalers_vs_time(int runnum, bool is100k)
{

  //changed the outputPREFIX so that it is compatble with both Root
  //and writing to a file by setting the enviromnet properly
  TString PREFIX = Form(TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_", runnum,
      runnum);

  // groups root files for a run together
  TChain* chain = new TChain("event_tree");

  //add the root files to the chain the Mps_Tree branches
  chain->Add(Form("$QW_ROOTFILES/Qweak_%d*.root", runnum));

  //create a vector of vectors of TH2D histogram pointer
  std::vector<TH2D*> h;
  //Size of h
  h.resize(3);

  for (int j = 1; j < 3; j++)
  {
    h[j] = new TH2D(Form("h[%d]", j),
        Form("Trigger Scintillator scalers - Package %d", j), 30, 1.0, 0.0, 30,
        1.0, 0.0);
    h[j]->GetYaxis()->SetTitle(
        Form("Trigger Scintillator scaler valuse for package %d", j));
    h[j]->GetXaxis()->SetTitle("Time");

  }

//start with looping over all the events and putting them in the correct histogram 

  //figure out how many evernts are in the rootfile so I know how long to have my loop go for
  Int_t nevents = chain->GetEntries();

  //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
  QwEvent* fEvent = 0;

  //Now I have to get a pointer to the events branch to loop through

  //Start by setting the event_tree branch to be on
  chain->SetBranchStatus("events", 1);
  chain->SetBranchStatus("trigscint", 1);

  //now get the event branch of the event_tree branch and call it event_branch creativly
  TBranch* event_branch = chain->GetBranch("events");
  TBranch* trig_branch = chain->GetBranch("trigscint");
  event_branch->SetAddress(&fEvent);

  TLeaf* ts1mt = trig_branch->GetLeaf("ts1mt_sca");
  TLeaf* ts2mt = trig_branch->GetLeaf("ts2mt_sca");

  //Loop through this and fill all the graphs at once

  for (int i = 0; i < nevents; i++)
  {
    //Get the ith entry form the event tree
    event_branch->GetEntry(i);
    trig_branch->GetEntry(i);

    if (ts1mt->GetValue() > 0)
    {
      h[1]->Fill(fEvent->fEventHeader->fEventNumber, ts1mt->GetValue());
    }
    if (ts2mt->GetValue() > 0)
    {
      h[2]->Fill(fEvent->fEventHeader->fEventNumber, ts2mt->GetValue());
    }
  }

  //Create a canvas
  TCanvas c1("c1", "Trigger Scintillator Vs. Event Number", 900, 400);

  //Divide that canvas into 2 rows and 1 columns
  c1.Divide(1, 2);

  for (int i = 1; i <= 2; i++)
  {
    //select pad 1
    c1.cd(i);

    h[i]->Draw();

    //Trigger scintilator vs. Event number -
    /*Form(trigscint.ts%dmt_sca:CodaEventNumber","trigscint.ts%dmt_sca>0",i,i) - a function to
     insert the variable, i in to the string, where one wants it the int is represtented by a %d */

    //Note I am not sure that the data is getting printed here however the print statment is working
//			chain->Draw(Form("trigscint.ts%ddmt_sca:CodaEventNumber>>v%d",i,i),Form("trigscint.ts%ddmt_sca>0",i));
    //cout <<"I printed " << i <<endl;
  }

  //save the canvas as a png file - right now it goes to the rootfile directory
  c1.SaveAs(PREFIX + "trigger_scint_scalers_vs_time.png");
  c1.SaveAs(PREFIX + "trigger_scint_scalers_vs_time.C");

  return;

}
