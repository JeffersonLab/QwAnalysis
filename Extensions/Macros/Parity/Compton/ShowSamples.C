#include <vector>

void ShowSamples(int entry)
{
  std::vector<MQwSIS3320_Samples>* sampled_events = 0;
  Mps_Tree->SetBranchAddress("fadc_compton_samples",&sampled_events);

  Mps_Tree->GetEntry(entry);

  if (sampled_events->size() > 0) {
    TGraph& graph = sampled_events->at(0).GetGraph();
    graph.Draw("AL");
  } else {
    std::cout << "No samples in this event" << std::endl;
  }
}
