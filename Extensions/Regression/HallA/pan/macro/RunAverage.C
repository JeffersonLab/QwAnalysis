//////////////////////////////////////////////////////////////////////
//
// RunAverage.C
//   Bryan Moffit - June 2005
//
//  Provides a means to show a given parameter (branch of a tree) vs
//  runnumber.
//
//  Requires:
//     PAN Libraries, run in the input Tree.
//  Output:
//     TGraphErrors object
//  Usage:
//     TGraphErrors *gr = 
//        Integrated("TreeName","Parameter",TCut usercut);
//

TGraphErrors *RunAverage(TTree *tree, 
			 TString parameter,
			 TCut usercut=1) {

  // Check to make sure input arguments are okay.
  if(!tree) {
    cout << "Integrated: " << endl
	 << "\t Input tree does not exist." << endl;
    return 0;
  }
//  if(!tree->FindBranch(parameter)) {
//    cout << "Integrated: " << endl
//	 << "\t Input parameter (" << parameter 
//	 << ") does not exist in tree." << endl;
//    return 0;
//  }

  // Get the smallest and largest runnumber, we'll loop through those
  UInt_t low_run = (UInt_t)tree->GetMinimum("run");
  UInt_t high_run = (UInt_t)tree->GetMaximum("run");
  
  TGraphErrors *graph = new TGraphErrors();
  UInt_t run_incr=0;
  for(UInt_t irun=low_run; irun<=high_run; irun++) {
    TCut runcut = Form("run==%d",irun);
    Long64_t drawres = tree->Draw(parameter+">>h1",runcut+usercut,"goff");
    if(drawres>0) {
      TH1F *h1 = (TH1F*)gROOT->FindObject("h1");
      graph->SetPoint(run_incr,(double)irun,h1->GetMean());
      graph->SetPointError(run_incr,0,h1->GetRMS()/sqrt(h1->GetEntries()-1));
      run_incr++;
    }
    
  }
  // Set default titles for graph, x-axis, y-axis
  graph->SetTitle(parameter+" vs run;run ;"+parameter);

  return graph;

}
