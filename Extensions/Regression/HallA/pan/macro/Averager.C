//////////////////////////////////////////////////////////////////////
//
// Averager.C
//   Bryan Moffit - June 2005
//
//  Provides a means to show a given parameter (branch of a tree) vs
//  a specifed parameter (group).. could be runnumber, or slug!
//
//  Requires:
//     PAN Libraries, run in the input Tree.
//  Output:
//     TGraphErrors object
//  Usage:
//     TGraphErrors *gr = 
//        Integrated("TreeName","Parameter","Group",TCut usercut);
//

TGraphErrors *Averager(TTree *tree, 
			 TString parameter,
			 TString group="run",
			 TCut usercut="1==1") {

  // Check to make sure input arguments are okay.
  if(!tree) {
    cout << "Averager: " << endl
	 << "\t Input tree does not exist." << endl;
    return 0;
  }
//  if(!tree->FindBranch(parameter)) {
//    cout << "Integrated: " << endl
//	 << "\t Input parameter (" << parameter 
//	 << ") does not exist in tree." << endl;
//    return 0;
//  }

  cout << "Working on " << parameter << endl;

  // Get the smallest and largest runnumber, we'll loop through those
  UInt_t low_group = (UInt_t)tree->GetMinimum(group);
  UInt_t high_group = (UInt_t)tree->GetMaximum(group);
  
  TGraphErrors *graph = new TGraphErrors();
  UInt_t group_incr=0;
  TH1F* h1;
  for(UInt_t igroup=low_group; igroup<=high_group; igroup++) {
    TCut groupcut = group+Form("==%d",igroup);
    Long64_t drawres = tree->Draw(parameter+">>h1",groupcut+usercut,"goff");
    h1 = (TH1F*)gROOT->FindObject("h1");
    if(h1!=NULL) {
      if(h1->GetEntries()>0) {
	graph->SetPoint(group_incr,(double)igroup,h1->GetMean());
	graph->SetPointError(group_incr,0,h1->GetRMS()/sqrt(h1->GetEntries()-1));
	group_incr++;
      }
      delete h1;
    }
    h1=NULL;
    
  }
  // Set default titles for graph, x-axis, y-axis
  graph->SetTitle(parameter+" vs "+group+";"+group+";"+parameter);
  return graph;

}
