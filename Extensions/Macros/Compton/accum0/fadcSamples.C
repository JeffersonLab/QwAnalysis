#include <vector>
#include "TGraph.h"
#include "TROOT.h"
#include "TTree.h"
#include <iostream>

Int_t fadcSamples(int entry, int runnum, int runseg)
{
  TFile *file = TFile::Open(Form("%s/Compton_Pass1_%d.00%d.root",
				 getenv("QW_ROOTFILES"),runnum, runseg));
  if (file == 0){
    printf("File does not exist.\n");
    return 0;
  }
  TTree *mpsTree =(TTree*)file->Get("Mps_Tree");
  if (mpsTree == 0){
    printf("Tree does not exist.\n");
    return 0;
  }

  std::vector<QwSIS3320_Samples>* sampled_events = 0;
  mpsTree->SetBranchAddress("fadc_compton_samples",&sampled_events);

  mpsTree->GetEntry(entry);

  if (sampled_events->size() > 0) {
    TGraph* graph = sampled_events->at(0).GetGraph();
    graph->Draw("AL");
  } else {
    std::cout << "No samples in this event" << std::endl;
    return 0;
  }
  return 1;
}
