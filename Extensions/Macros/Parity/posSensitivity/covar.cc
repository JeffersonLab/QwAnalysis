{
  //#include <SensitivityClass.h>

  gROOT->Reset();

  UInt_t run_number = 0, part = 0;
  Double_t raster;
  cout << "Enter run number and raster size (mm): " ;
  cin >> run_number;
  cin >> raster;

  UInt_t fileloc = 0; 
  char *prefix;
  cout << "Enter 0 if root files are on scratch disk" << endl;
  cout << "Enter 1 if root files are on silo" << endl;
  cout << "Enter 2 for first100k file" << endl;
  cin >> fileloc;

  if (fileloc == 1) {
    prefix = "/w/cache/mss/hallc/qweak/rootfiles/pass0/";
    cout << "Looking on the silo..." << endl;
  }
  else if (fileloc == 0 || fileloc == 2) {
    prefix = "$QWSCRATCH/rootfiles/";
    cout << "Looking on cdaq..." << endl;
  }

  // Full Pass1 root files on scratch disk or silo
  if (fileloc == 0 || fileloc == 1) {
    TFile f(Form("%sQwPass1_%d.000.root", prefix,run_number));
    if(!f->IsOpen()) {
      std::cout<<"Unable to find Rootfile for Run # "
	       << run_number
	       <<". Please insert a correct Run #. Exiting the program! "<<std::endl;
      exit(1);
    }
    TChain chain("Hel_Tree");
    chain.Add(Form("%sQwPass1_%d.000.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.001.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.002.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.003.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.004.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.005.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.006.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.007.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.008.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.009.root",prefix,run_number));
    chain.Add(Form("%sQwPass1_%d.010.root",prefix,run_number)); 
  }
  // first100k root files on scratch disk
  else {
    TFile f(Form("%sfirst100k_%d.root",prefix,run_number));
    if (!f->IsOpen()) {
      cout << "File not found!" << endl;
      exit(1);
    }
    TChain chain("Hel_Tree");
    chain.Add(Form("%sfirst100k_%d.root",prefix,run_number));
  }

  gStyle->Reset();
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetTitleYOffset(1.5);
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetTitleSize(0.035);
  gStyle->SetTitleYSize(0.035);
  gStyle->SetTitleXSize(0.035);
  gStyle->SetOptStat("neMmRr");
  gStyle->SetOptFit(1111);

  new TCanvas("c1",Form("%d:mpscounter",run_number),1200,700);
  c1->Divide(1,2);
  c1->cd(1);
  //  chain->Draw("yield_qwk_targetX:mps_counter","cleandata");
  chain->Draw("yield_qwk_bcm1:mps_counter","ErrorFlag==0");
  c1->cd(2);
  chain->Draw("yield_qwk_mdallbars:mps_counter","ErrorFlag==0");
  c1->Update();
  //  c1->SaveAs(Form("./slopes/run%dmpscounter"),run_number);

  UInt_t cutlow, cuthigh;
  
  cout << "Enter cuts on mps_counter: ";
  cin >> cutlow;
  cin >> cuthigh;
  char *mpscut = Form("mps_counter > %d && mps_counter < %d && ErrorFlag==0",cutlow,cuthigh);
  //  char *Xcut = Form("diff_qwk_targetX > -0.15 && diff_qwk_targetX < 0.15");
  //  char *Ycut = Form("diff_qwk_targetY > -0.15 && diff_qwk_targetY < 0.15");
  char *allcut = Form("%s",mpscut);
  
  new TCanvas("c2",Form("%d: BPM positions",run_number),1200,700);
  c2->Divide(2,2);
  c2->cd(1);
  chain->Draw("yield_qwk_targetX>>tgtX",allcut);
  tgtX->GetXaxis()->SetTitle("Position on target in x (mm)");
  c2->cd(2);
  chain->Draw("yield_qwk_targetY>>tgtY",allcut);
  tgtY->GetXaxis()->SetTitle("Position on target in y (mm)");
  c2->cd(3);
  chain->Draw("yield_qwk_targetXSlope*1.e6>>tgtXslope",allcut);
  tgtXslope->GetXaxis()->SetTitle("Angle in x (#murad)");
  c2->cd(4);
  chain->Draw("yield_qwk_targetYSlope*1.e6>>tgtYslope",allcut);
  tgtYslope->GetXaxis()->SetTitle("Angle in y (#murad)");
  c2->Update();
  c2->SaveAs(Form("./slopes/run%dcovarbpm.png",run_number));

  Double_t Xtgtmean   = tgtX->GetMean();
  Double_t Ytgtmean   = tgtY->GetMean();
  Double_t Xthtgtmean = tgtXslope->GetMean();
  Double_t Ythtgtmean = tgtYslope->GetMean();

  cout << "At target: X  Y  Xth Yth" << endl;
  cout << Form("%10.3f %10.3f %10.3f %10.3f",Xtgtmean, Ytgtmean, Xthtgtmean, Ythtgtmean) << endl;

  //Check that the cuts worked!                                                                    
  /*  char *OK = "n";
  cout << "Cuts OK? (y/n): ";
  cin >> OK;
  if (OK == "n") {
    cout << "Enter new mps_counter cut: ";
    cin >> cutlow;
    cin >> cuthigh;
    char *allcut = Form("mps_counter > %d && mps_counter < %d",cutlow,cuthigh);
    } */

  // Extract sensitivities for the whole main detector
  new TCanvas("c100",Form("%d:Correlations",run_number),1200,700);
  c100->Divide(2,2);
  c100->cd(1);
  chain->Draw("asym_qwk_mdallbars*1.e6:diff_qwk_targetX >> epsX",allcut,"cont");
  c100->cd(2);
  chain->Draw("asym_qwk_mdallbars*1.e6:diff_qwk_targetY >> epsY",allcut,"cont");
  c100->cd(3);
  chain->Draw("asym_qwk_mdallbars*1.e6:diff_qwk_targetXSlope*1.e6 >> epsXslope",allcut,"cont");
  c100->cd(4);
  chain->Draw("asym_qwk_mdallbars*1.e6:diff_qwk_targetYSlope*1.e6 >> epsYslope",allcut,"cont");
  c100->Update();

  Double_t allentries=      epsX->GetEntries();
  Double_t covXeps   =      epsX->GetCovariance(1,2);
  Double_t covYeps   =      epsY->GetCovariance(1,2);
  Double_t covXtheps = epsXslope->GetCovariance(1,2);
  Double_t covYtheps = epsYslope->GetCovariance(1,2);

  Double_t vareps    =      epsX->GetCovariance(2,2);
  Double_t varX      =      epsX->GetCovariance(1,1);
  Double_t varY      =      epsY->GetCovariance(1,1);
  Double_t varXth    = epsXslope->GetCovariance(1,1);
  Double_t varYth    = epsYslope->GetCovariance(1,1);

  Double_t epsmean   =      epsX->GetMean(2);
  Double_t Xmean     =      epsX->GetMean(1);
  Double_t Ymean     =      epsY->GetMean(1);
  Double_t Xthmean   = epsXslope->GetMean(1);
  Double_t Ythmean   = epsYslope->GetMean(1);

  Double_t corr_Xeps   = covXeps/sqrt(vareps*varX);
  Double_t corr_Yeps   = covYeps/sqrt(vareps*varY);
  Double_t corr_Xtheps = covXtheps/sqrt(varXth*vareps);
  Double_t corr_Ytheps = covYtheps/sqrt(varYth*vareps);

  /*  delete epsX;
  delete epsY;
  delete epsXslope;
  delete epsYslope; */

  cout << "Finished correlations with asymmetry" << endl;

  c100->cd(1);
  chain->Draw("diff_qwk_targetX:diff_qwk_targetY >> XY",allcut,"cont");
  c100->cd(2);
  chain->Draw("diff_qwk_targetX:diff_qwk_targetXSlope*1.e6 >> XXslope",allcut,"cont");
  c100->cd(3);
  chain->Draw("diff_qwk_targetX:diff_qwk_targetYSlope*1.e6 >> XYslope",allcut,"cont");
  c100->Update();

  Double_t covXY     =      XY->GetCovariance(1,2);
  Double_t covXXslope= XXslope->GetCovariance(1,2);
  Double_t covXYslope= XYslope->GetCovariance(1,2);
  Double_t varXslope = XXslope->GetCovariance(1,1);
  Double_t varYslope = XYslope->GetCovariance(1,1);

  Double_t corr_XY      = covXY/sqrt(varX*varY);
  Double_t corr_XXslope = covXXslope/sqrt(varX*varXslope);
  Double_t corr_XYslope = covXYslope/sqrt(varX*varYslope);

  /*  delete XY;
  delete XXslope;
  delete XYslope; */

  cout << "Finished correlations with targetX" << endl;

  c100->cd(1);
  chain->Draw("diff_qwk_targetY:diff_qwk_targetXSlope*1.e6 >> YXslope",allcut,"cont");
  c100->cd(2);
  chain->Draw("diff_qwk_targetY:diff_qwk_targetYSlope*1.e6 >> YYslope",allcut,"cont");
  c100->cd(3);
  chain->Draw("diff_qwk_targetXSlope*1.e6:diff_qwk_targetYSlope*1.e6 >> XslopeYslope",allcut,"cont");
  c100->Update();

  Double_t covYXslope      =      YXslope->GetCovariance(1,2);
  Double_t covYYslope      =      YYslope->GetCovariance(1,2);
  Double_t covXslopeYslope = XslopeYslope->GetCovariance(1,2);

  Double_t corr_YXslope      = covYXslope/sqrt(varY*varXslope);
  Double_t corr_YYslope      = covYYslope/sqrt(varY*varYslope);
  Double_t corr_XslopeYslope = covXslopeYslope/sqrt(varXslope*varYslope);
  
  Double_t Delta = varX*varY - covXY*covXY;
  Double_t aall = (covXeps*varY - covYeps*covXY)/Delta;
  Double_t ball = (covYeps*varX - covXeps*covXY)/Delta;

  Double_t siga = sqrt(varY*vareps/(allentries*Delta));
  Double_t sigb = sqrt(varX*vareps/(allentries*Delta));

  //  Double_t call = (covXeps - aall*varX - ball*covXY)/covXXY;
  //  cout << Form("All bars: %10.3f %10.3f %10.4f", aall, ball, call) << endl;
  cout << endl;
  cout << "All bars, 2 parameter fit: x y err_x err_y" << endl;
  cout << Form("%10.3f %10.3f %10.3f %10.3f", aall, ball, siga, sigb) << endl;
  Double_t Xasym, Yasym, errXasym, errYasym;
  Xasym = Xmean * aall;
  Yasym = Ymean * ball;
  errXasym = Xmean * siga;
  errYasym = Ymean * sigb;
  cout << "Contributions to false asymmetry (X, Y, Xerr, Yerr)" << endl;
  cout << Form("%10.3f %10.3f %10.3f %10.3f",Xasym, Yasym, errXasym, errYasym) << endl;
  
  Double_t corr_Xeps = covXeps/sqrt(varX*vareps);
  Double_t corr_Yeps = covYeps/sqrt(varY*vareps);
  Double_t corr_XY   = covXY/sqrt(varX*varY); 

  /*  delete YXslope;
  delete YYslope;
  delete XslopeYslope; */

  cout << "Correlations for all bars" << endl;
  char *blanks = "      ";
  cout << "          eps        X          Y          thX        thY" << endl;
  cout << Form("eps %10.3f %10.3f %10.3f %10.3f %10.3f", vareps/vareps, corr_Xeps, corr_Yeps, corr_Xtheps, corr_Ytheps) << endl;
  cout << Form("X   %s     %10.3f %10.3f %10.3f %10.3f", blanks, vareps/vareps, corr_XY, corr_XXslope, corr_XYslope)    << endl;
  cout << Form("Y   %s     %s     %10.3f %10.3f %10.3f", blanks, blanks, vareps/vareps, corr_YXslope, corr_YYslope)     << endl; 
  cout << Form("thX %s     %s     %s     %10.3f %10.3f", blanks, blanks, blanks, vareps/vareps, corr_XslopeYslope)      << endl;
  cout << Form("thY %s     %s     %s     %s     %10.3f", blanks, blanks, blanks, blanks, vareps/vareps) << endl;
  cout << endl;

  cout << "Finished correlations with targetY and targetXSlope" << endl;
  cout << endl;
  cout << "Sensitivities to X, Y, Xth, Yth";
  cout << endl; 

  // Start covariance analysis of main detector array
  TMatrixD covs(4,4), epscovs(4,4), covscopy(4,4);
  TArrayD array(16);
  array[0] = varX;
  array[1] = covXY;
  array[2] = covXXslope;
  array[3] = covXYslope;

  array[4] = covXY;
  array[5] = varY;
  array[6] = covYXslope;
  array[7] = covYYslope;

  array[8] = covXXslope;
  array[9] = covYXslope;
  array[10]= varXslope;
  array[11]= covXslopeYslope;

  array[12]= covXYslope;
  array[13]= covYYslope;
  array[14]= covXslopeYslope;
  array[15]= varYslope;  

  covs.SetMatrixArray(array.GetArray());
  covscopy = covs;  

  Double_t num, denom, coeff[4];
  
  /*  Double_t dummy[] = {covXeps,covYeps,covXtheps,covYtheps};
  TVectorD epsvec;
  epsvec.Use(4,dummy); 
  
  TDecompSVD svd(covscopy);
  Bool_t ok;
  const TVectorD c_svd = svd.Solve(epsvec,ok);
  for (UInt_t i = 0; i<4; i++) {
    coeff[i] = c_svd[i];
    cout << Form("%d %10.3f", i+1, coeff[i]) << endl; 
    } */

  covs.Invert(&denom);
  for (UInt_t i = 0; i<4; i++) {
    epscovs = covscopy;
    epscovs(0,i) = covXeps;
    epscovs(1,i) = covYeps;
    epscovs(2,i) = covXtheps;
    epscovs(3,i) = covYtheps; 

    epscovs.Invert(&num); 
    coeff[i] = num/denom; 
  } 
  cout << Form("%10.3f %10.3f %10.3f %10.3f", coeff[0], coeff[1], coeff[2], coeff[3]) << endl;
  Double_t Xcorr   = coeff[0]*Xmean;
  Double_t Ycorr   = coeff[1]*Ymean;
  Double_t Xthcorr = coeff[2]*Xthmean;
  Double_t Ythcorr = coeff[3]*Ythmean;
  cout << "Contributions to average asymmetry (X, Y, Xth, Yth):" << endl;
  cout << Form("%10.3f %10.3f %10.3f %10.3f",Xcorr,Ycorr,Xthcorr,Ythcorr) << endl;

  // Covariance analysis of asymmetry and positions for separate bars
  Double_t a[8], b[8], c[8], d[8], da[8], db[8];

  //  TVectorD epsvec;
  //  Bool_t ok;
  cout << "Sensitivities of individual bars to motion in X and Y" << endl;
  Double_t asym[8], asym_err[8], entries;
  for (UInt_t i = 1; i < 9; i++) {
    cout << Form("%d",i) << endl;
    c100->cd(1);
    chain->Draw(Form("asym_qwk_md%dbarsum*1.e6:diff_qwk_targetX >> epsX2",i),allcut,"cont");
    c100->cd(2);
    chain->Draw(Form("asym_qwk_md%dbarsum*1.e6:diff_qwk_targetY >> epsY2",i),allcut,"cont");
    /*    c100->cd(3);
	  chain->Draw(Form("asym_qwk_md%dbarsum*1.e6:diff_qwk_targetXSlope*1.e6 >> epsXslope",i),allcut,"cont"); 
    c100->cd(4);
    chain->Draw(Form("asym_qwk_md%dbarsum*1.e6:diff_qwk_targetYSlope*1.e6 >> epsYslope",i),allcut,"cont"); */
    c100->Update();
    
    covXeps       = epsX2->GetCovariance(1,2);
    covYeps       = epsY2->GetCovariance(1,2);
    vareps        = epsX2->GetCovariance(2,2);

    asym[i-1]     = epsX2->GetMean(2);
    asym_err[i-1] = epsX2->GetRMS(2);
    entries       = epsX2->GetEntries();
    asym_err[i-1] /= sqrt(entries);

    //    covXtheps = epsXslope->GetCovariance(1,2);
    //    covYtheps = epsYslope->GetCovariance(1,2);

    /*    Double_t dummy[] = {covXeps,covYeps,covXtheps,covYtheps};
    epsvec.Use(4,dummy); 
  
    covscopy = covs;
    const TDecompSVD svd(covscopy);
    c_svd = svd.Solve(epsvec,ok);
    for (UInt_t j = 0; j<4; j++) {
      coeff[j] = c_svd[j];
      //      cout << Form("%d %10.3f", i+1, coeff[i]) << endl; 
      } */


    /*    for (UInt_t j = 0; j<4; j++) {
      epscovs = covscopy;
      epscovs(0,j) = covXeps;
      epscovs(1,j) = covYeps;
      epscovs(2,j) = covXtheps;
      epscovs(3,j) = covYtheps; 
      
      epscovs.Invert(&num); 
      coeff[j] = num/denom; 
      // cout << Form("%d %10.3f", j+1, coeff[j]) << endl;
      } */
    /*    a[i-1] = coeff[0];
    b[i-1] = coeff[1];
    c[i-1] = coeff[2];
    d[i-1] = coeff[3]; */
    
    // Errors of a and b for 2 parameter fit
    Delta = varX*varY - covXY*covXY;
    da[i-1] = sqrt(varY*vareps/(entries*Delta));
    db[i-1] = sqrt(varX*vareps/(entries*Delta));

    a[i-1] = (covXeps*varY - covYeps*covXY)/Delta;
    b[i-1] = (covYeps*varX - covXeps*covXY)/Delta; 
//    cout << Form("%d %10.3f %10.3f %10.3f %10.3f", i, a[i-1], b[i-1], c[i-1], d[i-1]) << endl;

    //    corr_Xeps = covXeps/sqrt(varX*vareps);
    //    corr_Yeps = covYeps/sqrt(varY*vareps);
  }
  for (i = 0; i<8; i++)
    cout << Form("%d %10.3f %10.3f %10.3f %10.3f", i+1, a[i], b[i], da[i], db[i]) << endl;
  cout << " " << endl;
  
  // Plot out bar asymmetries
  new TCanvas("c12",Form("%d:Bar fits",run_number),1200,600);
  c12->Divide(2,1);
  c12->cd(1);
  const Int_t n = 8;
  Double_t x[n] = {1,2,3,4,5,6,7,8};
  Double_t ex[n]= {0,0,0,0,0,0,0,0};
  TGraph *gr1 = new TGraphErrors(n,x,a,ex,da);
  gr1->SetTitle(Form("%d:motion in x",run_number));
  gr1->SetMarkerStyle(21);
  gr1->SetMarkerColor(kRed);
  gr1->SetLineColor(kRed);
  gr1->Draw("APE");
  gr1->GetXaxis()->SetTitle("Bar number");
  gr1->GetYaxis()->SetTitle("X sensitivity (ppm/mm)");
  gr1->Draw("APE");
  TF1 *f11 = new TF1("f11","[0] + [1]*cos((x-1)*pi/4)",1,8);
  TF1 *f12 = new TF1("f12","[0] + [1]*cos((x-1)*pi/4 + [2])",1,8);
  f12->SetLineColor(kRed-3);
  f12->SetParLimits(2,-2.,2.);
  f12->SetParameter(2,0.);
  gr1.Fit("f12");
  c12->Update();

  c12->cd(2);
  TGraph *gr2 = new TGraphErrors(n,x,b,ex,db);
  gr2->SetTitle(Form("%d:motion in y",run_number));
  gr2->SetMarkerStyle(21);
  gr2->SetMarkerColor(kGreen);
  gr2->SetLineColor(kGreen);
  gr2->Draw("APE");
  gr2->GetXaxis()->SetTitle("Bar number");
  gr2->GetYaxis()->SetTitle("Y sensitivity (ppm/mm)");
  gr2->Draw("APE");
  TF1 *f21 = new TF1("f21","[0] + [1]*sin((x-1)*pi/4)",1,8);
  TF1 *f22 = new TF1("f22","[0] + [1]*sin((x-1)*pi/4 + [2])",1,8);
  f22->SetLineColor(kGreen+2);
  f22->SetParLimits(2,-2.,2.);
  f22->SetParameter(2,0.);
  gr2.Fit("f22");
  c12->Update();

  Double_t Xbar[3], XbarErr[3], Ybar[3], YbarErr[3];
  TF1 *Xbars = gr1->GetFunction("f12");
  TF1 *Ybars = gr2->GetFunction("f22");
  for (i = 0; i < 3; i++) {
    Xbar[i]    = Xbars->GetParameter(i);
    Ybar[i]    = Ybars->GetParameter(i);
    XbarErr[i] = Xbars->GetParError(i);
    YbarErr[i] = Ybars->GetParError(i);
  }

  new TCanvas("c1011",Form("%d:Asymmetry vs bar number",run_number),1200,700);
  c1011->Divide(1,2);
  c1011->cd(1);
  TGraph *gr3 = new TGraphErrors(n,x,asym,ex,asym_err);
  gr3->SetTitle(Form("%d:Asymmetry vs bar number",run_number));
  gr3->SetMarkerStyle(21);
  gr3->Draw("APE");
  gr3->GetXaxis()->SetTitle("Bar number");
  gr3->GetYaxis()->SetTitle("Asymmetry (ppm)");
  gr3->Draw("APE");
  f12->SetParLimits(2,-3.,3.);
  f12->SetParameter(2,0.);
  gr3.Fit("f12");
  c1011->Update();

  // Correct asymmetry for motion in x and y

  //  Double_t asym_corr[8];
  for (UInt_t i = 0; i<8; i++) 
    asym[i] = asym[i] - a[i]*Xmean - b[i]*Ymean; 
  
  TGraph *gr4 = new TGraphErrors(n,x,asym,ex,asym_err);
  c1011->cd(2);
  gr4->SetTitle(Form("%d:Asymmetry corrected for beam motion vs bar number",run_number));
  gr4->SetMarkerStyle(21);
  gr4->Draw("APE");
  gr4->GetXaxis()->SetTitle("Bar number");
  gr4->GetYaxis()->SetTitle("Asymmetry (ppm)");
  gr4->Draw("APE");
  f12->SetParLimits(2,-3.,3.);
  f12->SetParameter(2,0.);
  gr4.Fit("f12");
  c1011->Update(); 

  Double_t pol[3], polerr[3];
  TF1 *PolFit = gr4->GetFunction("f12");
  for (i = 0; i < 3; i++) {
    pol[i]    = PolFit->GetParameter(i);
    polerr[i] = PolFit->GetParError(i);
  }

  new TCanvas("1012",Form("%d:Average beam current",run_number),800,600);
  chain->Draw("yield_qwk_bcm1 >> current","ErrorFlag==0");
  Double_t Iavg = current->GetMean();
  cout << Form("Avg current: %10.3f", Iavg) << endl;

  // Save the fits

  ofstream barfit(Form("slopes/run%dsummary.dat",run_number));
  barfit << Form("Avg current: %10.3f", Iavg) << endl;
  barfit << "Position + angle of beam on target: Xtgt Ytgt Xth Yth raster" << endl;
  barfit << Form("%10.3f %10.3f %10.3f %10.3f %10.3f", Xtgtmean, Ytgtmean, Xthtgtmean, Ythtgtmean, raster) << endl;
  barfit << "Sensitivity offset, max value and phase for fit to 8 bars" << endl;
  barfit << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number, Xbar[0],XbarErr[0],Xbar[1],XbarErr[1],Xbar[2],XbarErr[2],Xtgtmean,Xthtgtmean,raster) << endl;
  barfit << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number, Ybar[0], YbarErr[0], Ybar[1], YbarErr[1], Ybar[2], YbarErr[2],Ytgtmean,Ythtgtmean) << endl;
  barfit << " " << endl;

  barfit << Form("All bars: %10.3f %10.3f %10.3f %10.3f", aall, ball, siga, sigb) << endl;
  barfit << "Contributions to false asymmetry (X, Y)" << endl;
  cout << "Contributions to false asymmetry (X, Y)" << endl;
  cout << Form("%10.3f %10.3f %10.3f %10.3f",Xasym, Yasym, errXasym, errYasym) << endl;

  barfit << "Correlations for all bars" << endl;
  barfit << "          eps        X          Y          thX        thY" << endl;
  barfit << Form("eps %10.3f %10.3f %10.3f %10.3f %10.3f", vareps/vareps, corr_Xeps, corr_Yeps, corr_Xtheps, corr_Ytheps) << endl;
  barfit << Form("X   %s     %10.3f %10.3f %10.3f %10.3f", blanks, vareps/vareps, corr_XY, corr_XXslope, corr_XYslope)    << endl;
  barfit << Form("Y   %s     %s     %10.3f %10.3f %10.3f", blanks, blanks, vareps/vareps, corr_YXslope, corr_YYslope)     << endl; 
  barfit << Form("thX %s     %s     %s     %10.3f %10.3f", blanks, blanks, blanks, vareps/vareps, corr_XslopeYslope)      << endl;
  barfit << Form("thY %s     %s     %s     %s     %10.3f", blanks, blanks, blanks, blanks, vareps/vareps) << endl;
  barfit << endl;

  barfit << "Sensitivities to X, Y, Xth, Yth, covariance analysis" << endl;
  barfit << Form("%10.3f %10.3f %10.3f %10.3f", coeff[0], coeff[1], coeff[2], coeff[3]) << endl;
  barfit << endl;
  barfit << "Contributions to average asymmetry (X, Y, Xth, Yth):" << endl;
  barfit << Form("%10.3f %10.3f %10.3f %10.3f",Xcorr,Ycorr,Xthcorr,Ythcorr) << endl;

  barfit << "Sensitivities for individual bars, X, Y only, with errors" << endl;
  for (UInt_t i = 0; i<8; i++)
    barfit << Form("%d %10.3f %10.3f", i+1, a[i], b[i], da[i], db[i]) << endl;

  cout << "Fit to asymmetry vs bar number: run, avg. asymm, max. asymm, phase, with errors" << endl;
  cout << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number,pol[0],polerr[0],pol[1],polerr[1],pol[2],polerr[2]) << endl;

  barfit << "Fit to asymmetry vs bar number: run, avg. asymm, max. asymm, phase, with errors" << endl;
  barfit << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number,pol[0],polerr[0],pol[1],polerr[1],pol[2],polerr[2]) << endl;

  barfit.close();

  ofstream fits(Form("./slopes/%dfit.dat",run_number));
  fits << "Sensitivity offset, max value and phase for fit to 8 bars, position, angle, raster" << endl;
  fits << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number,Xbar[0],XbarErr[0],Xbar[1],XbarErr[1],Xbar[2],XbarErr[2], Xtgtmean, Xthtgtmean, raster) << endl;
  fits << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number, Ybar[0], YbarErr[0], Ybar[1], YbarErr[1], Ybar[2],  YbarErr[2], Ytgtmean, Ythtgtmean) << endl;	   
  fits << Form("%d %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",run_number, coeff[0], coeff[1], coeff[2], coeff[3], aall, ball, siga, sigb) << endl;
  fits.close();

  c12->SaveAs(Form("./slopes/%dcovarfit.png",run_number));
  c1011->SaveAs(Form("./slopes/%dasymmetry.png",run_number));

  //  cout << "Finished!" << endl;

}
