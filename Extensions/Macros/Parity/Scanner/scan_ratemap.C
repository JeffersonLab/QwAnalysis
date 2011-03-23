//
// File:     scan_ratemap.C
// Author:   Jie Pan
// Date:     Mar. 18, 2011
//
// Function: draw scanner rate map
// Usage:    root scan_ratemap.C, then input rootfile prefix, run number,
//           data segment number and module type for position measurement

{

  Int_t rootfile_prefix, run_num, start_seg, end_seg, module, beam_current;
  TString cut;
  cout<<"Draw scanner rate maps and profiles\n";
  cout<<"Input root file prefix\n";
  cout<<"(0 - Qweak, 1 - QwPass1): ";
  cin>>rootfile_prefix;
  cout<<"Input run number, start/end data segment number\n";
  cout<<"run number: ";
  cin>>run_num;
  cout<<"start segment: ";
  cin>>start_seg;
  cout<<"end segment: ";
  cin>>end_seg;
  cout<<"Intput module type for scanner position measurement\n";
  cout<<"(0 - VQWK, 1 - QDC): ";
  cin>>module;
  cout<<"Intput beam current (min. 1 uA) for data cut\n";
  cout<<"min. beam current: ";
  cin>>beam_current;

  TString prefix;
  if(rootfile_prefix==0)
     prefix = "Qweak";
  else if(rootfile_prefix==1)
     prefix = "QwPass1";
  else
     {
       cout<<"unmatched rootfile prefix - "<<rootfile_prefix<<endl;
       return;
     }

  if(start_seg>end_seg || end_seg>9)
     {
       cout<<"wrong segment number :"<<start_seg<<" - "<<end_seg<<endl; 
       return;
     }
  if(module!=0 && module!=1)
     {
       cout<<"wrong module type - "<<module<<endl;
       return;
     }
  if(beam_current<=1)
     beam_current = 1;

  cut = Form("scanner.CoincidenceSCA>0 && qwk_bcm1>=%i", beam_current);

  cout<<"\nYou are using "<<prefix<<"_, run#"<<run_num<<", segment "<<start_seg<<" - segment "<<end_seg<<", module type "<<module<<endl;
  cout<<"to draw beam current normalized rate distributions (qwk_bcm1>="<<beam_current<<")\n";

  TChain files("Mps_Tree");
  for (Int_t ifile = start_seg; ifile <= end_seg; ifile++)
  {
    TString file_name = "$QW_ROOTFILES/"+prefix+Form("_%i.00%i.root", run_num,ifile);
    files.Add(file_name);
    cout<<"Chained root file: "<<file_name<<endl;
  }

  cout<<"Start processing data, please wait...\n";

  TProfile2D ratemap("ratemap","scanner rate map",100,-100,100,40,-340.0,-315.0);
  TProfile2D randomap("randomap","scanner accidential rate map",100,-100,100,40,-340.0,-315.0);
  TProfile2D singlesmap_front("singlesmap_front","scanner singles (front) rate map",100,-100,100,40,-340.0,-315.0);
  TProfile2D singlesmap_back("singlesmap_back","scanner singles (back) rate map",100,-100,100,40,-340.0,-315.0);
  TProfile2D rawmap("rawmap","scanner raw rate map",100,-100,100,40,-340.0,-315.0);

  gStyle->SetPalette(1);
  TCanvas *c1 = new TCanvas("c1", "Rate distribution acquired by scanner", 1200,800);
  c1->Divide(1,3);
  TCanvas *c2 = new TCanvas("c2", "Rate distribution acquired by scanner", 1200,800);
  c2->Divide(1,4);

  c1->cd(1);
  if(module==0)
    files.Draw("((scanner.CoincidenceSCA-scanner.randm_sca_raw)/qwk_bcm1):(scanner.PositionY_VQWK):(scanner.PositionX_VQWK)>>ratemap",cut,"prof");
  else if(module==1)
    files.Draw("((scanner.CoincidenceSCA-scanner.randm_sca_raw)/qwk_bcm1):(scanner.PositionY_QDC):(scanner.PositionX_QDC)>>ratemap",cut,"prof");

  ratemap.SetStats(0);
  ratemap.Draw("colz"); 

  c2->cd(2);
  if(module==0)
    files.Draw("(scanner.randm_sca_raw/qwk_bcm1):(scanner.PositionY_VQWK):(scanner.PositionX_VQWK)>>randomap",cut,"prof");
  else if(module==1)
    files.Draw("(scanner.randm_sca_raw/qwk_bcm1):(scanner.PositionY_QDC):(scanner.PositionX_QDC)>>randomap",cut,"prof");
  randomap.SetStats(0);
  randomap.Draw("colz");

  c2->cd(3);
  if(module==0)
    files.Draw("(scanner.FrontSCA/qwk_bcm1):(scanner.PositionY_VQWK):(scanner.PositionX_VQWK)>>singlesmap_front",cut,"prof");
  if(module==1)
    files.Draw("(scanner.FrontSCA/qwk_bcm1):(scanner.PositionY_QDC):(scanner.PositionX_QDC)>>singlesmap_front",cut,"prof");
  singlesmap_front.SetStats(0);
  singlesmap_front.Draw("colz");

  c2->cd(4);
  if(module==0)
    files.Draw("(scanner.BackSCA/qwk_bcm1):(scanner.PositionY_VQWK):(scanner.PositionX_VQWK)>>singlesmap_back",cut,"prof");
  if(module==1)
    files.Draw("(scanner.BackSCA/qwk_bcm1):(scanner.PositionY_QDC):(scanner.PositionX_QDC)>>singlesmap_back",cut,"prof");
  singlesmap_back.SetStats(0);
  singlesmap_back.Draw("colz");

  c2->cd(1);
  if(module==0)
    files.Draw("(scanner.CoincidenceSCA/qwk_bcm1):(scanner.PositionY_VQWK):(scanner.PositionX_VQWK)>>rawmap",cut,"prof");
  if(module==1)
    files.Draw("(scanner.CoincidenceSCA/qwk_bcm1):(scanner.PositionY_QDC):(scanner.PositionX_QDC)>>rawmap",cut,"prof");
  rawmap.SetStats(0);
  rawmap.Draw("colz");

  TH2D *rate_map_histo = (TH2D*) ratemap->ProjectionXY();
  TH1D *rate_x = (TH1D*) rate_map_histo->ProjectionX();
  TH1D *rate_y = (TH1D*) rate_map_histo->ProjectionY();

  c1->cd(2);
  rate_x->SetTitle("Rate Profile (X-direction)");
  rate_x->Draw();

  c1->cd(3);
  rate_y->SetTitle("Rate Profile (Y-direction)");
  rate_y->Draw();

  cout<<"Type .ls to see the histograms which are available for further analysis."<<endl;

}

