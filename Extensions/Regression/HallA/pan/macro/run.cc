// Interactive root script example
// If one runs ./pan, the in the root shell .x run.cc

void run(UInt_t runnumber=0){

  if(runnumber==0) {
    cout << "Enter run number: "<< endl;
    cin >> runnumber;
  }
  
  TaAnalysisManager am;
  
  if (am.Init (runnumber) != 0)
    return 1;
  if (am.InitLastPass() != 0 ||
      am.Process() != 0 ||
      am.End() != 0)
    return 1;
  
  gROOT->LoadMacro("macro/open.macro");
  
  open(runnumber,"standard");
  //  TTree *raw = (TTree*)gROOT.FindObject("R");
  //   raw->Print();
  //  TTree *asy = (TTree*)gROOT.FindObject("P");
  //  asy->Print();
  
  //  raw->Draw("bcm1:ev_num");
  

}
