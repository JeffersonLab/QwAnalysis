{
  // Load the tree
  TFile file("hitlist.root");
  TTree* tree = (TTree*) file.Get("event_tree");
  QwHitRootContainer* rootlist = 0;
  tree->SetBranchAddress("hits",&rootlist);

  // Loop over the events in the file
  int fEntries = tree->GetEntries();
  for (int fEvtNum = 0; fEvtNum < fEntries; fEvtNum++) {

    // Print event number
    std::cout << "Event: " << fEvtNum << std::endl;

    // Read next event into QwHitRootContainer format
    tree->GetEntry(fEvtNum);

    // Convert into the QwHitContainer format
    QwHitContainer* hitlist = rootlist->Convert();

    // Print the hitlist
    hitlist->Print();

    // Clear rootlist
    rootlist->Clear();

    // Delete the hitlist
    delete hitlist;
  }

  // Delete the rootlist
  delete rootlist;

  // Close file
  file.Close();
}
