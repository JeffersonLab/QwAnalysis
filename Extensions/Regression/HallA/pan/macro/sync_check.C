////////////////////////////////////////////////////////////////////
//
// sync_check.C
//   - B. Moffit (May 2004)
//  ROOT macro that checks the synchronization between crates.
//

void sync_check(UInt_t runnumber) {
  open(runnumber,"standard");
  TCut goodcut = "(ev_num>0)";

  // Definition of the integration gate (in microSeconds)
  TString int_time = "(integtime*2.5)";

  // Defintion of the reference frequency (in MHz)
  TString ref = "(chsync0/"+int_time+")";

  // Measured gate times in each crate
  TString CH1 = "("+int_time + "-(chsync1)/"+ref+")";
  TString CH2 = "("+int_time + "-(chsync2)/"+ref+")";
  TString R1 = "("+int_time + "-(rsync1)/"+ref+")";
  TString R2 = "("+int_time + "-(rsync2)/"+ref+")";
  TString L1 = "("+int_time + "-(lsync1)/"+ref+")";
  TString L2 = "("+int_time + "-(lsync2)/"+ref+")";

  TCanvas *c1 = new TCanvas("c1","Sync Difference plots",1200,800);
  c1->Divide(3,2);

//   gStyle->SetOptLogy(0);
//   R->SetMarkerColor(2);
//   R->SetMarkerStyle(3);
//   R->SetMarkerSize(0.5);
  TString drawopt = "box";

  // Sychronization Difference plots.
  //  The difference between a given gate (e.g. 1) should be the
  //  same for all crates (within the bit error of the scaler).
  //  Therefore... all of these plots should read zero.

  c1->cd(1);
  R->Draw(CH1+"-"+R1+":pitadac",goodcut,drawopt);
  htemp->SetTitle("CH1-R1");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c1->cd(2);
  R->Draw(CH1+"-"+L1+":pitadac",goodcut,drawopt);
  htemp->SetTitle("CH1-L1");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c1->cd(3);
  R->Draw(L1+"-"+R1+":pitadac",goodcut,drawopt);
  htemp->SetTitle("L1-R1");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c1->cd(4);
  R->Draw(CH2+"-"+R2+":pitadac",goodcut,drawopt);
  htemp->SetTitle("CH2-R2");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c1->cd(5);
  R->Draw(CH2+"-"+L2+":pitadac",goodcut,drawopt);
  htemp->SetTitle("CH2-L2");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c1->cd(6);
  R->Draw(L2+"-"+R2+":pitadac",goodcut,drawopt);
  htemp->SetTitle("L2-R2");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c1->cd();
  c1->Update();

  // Synchronization Sum Plots.
  //  The idea here is that Gated sync1 + Gated sync2 should be equal
  //  to the integration gate.  Therefore... all of these plots should
  //  read zero (scaler bit error shows up as 3 non-zero bands. -2,-1,1).

  TCanvas *c2 = new TCanvas("c2","Sync SumCheck plots",1200,400);
  c2->Divide(3,1);

  c2->cd(1);
  R->Draw(CH1+"+"+CH2+"-"+int_time+":pitadac",goodcut,drawopt);
  htemp->SetTitle("CH1+CH2-IntegrationTime");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c2->cd(2);
  R->Draw(L1+"+"+L2+"-"+int_time+":pitadac",goodcut,drawopt);
  htemp->SetTitle("L1+L2-IntegrationTime");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c2->cd(3);
  R->Draw(R1+"+"+R2+"-"+int_time+":pitadac",goodcut,drawopt);
  htemp->SetTitle("R1+R2-IntegrationTime");
  htemp->GetXaxis()->SetTitle("Input frequency (DAC)");
  htemp->GetYaxis()->SetTitle("Time difference (#mus)");
  
  c2->cd();
  c2->Update();

  // Injector synchronization plot
  //  The Injector receives a difference reference frequency than the
  //  counting house..  However, it they still should proportional to
  //  each other.  This plot should be linear.... it shows that an
  //  event in the counting house is the same as an event in the
  //  injector.

  TCanvas *c3 = new TCanvas("c3","Injector Sync plot",400,400);
  c3->cd();
  R->Draw("chsync0:isync0",goodcut,drawopt);
  htemp->SetTitle("Counting House vs. Injector Synchronization");
  

  // Spit out details to standard out:
  //   - making sure all syncs are within bit error
  cout << endl;

  TCanvas *c4 = new TCanvas("c4","nothing");
  c4->cd();
  cout << "*****************************************************" << endl;
  cout << "*      Check for non-bit error synchronization      *" << endl;
  cout << "*****************************************************" << endl;

  UInt_t errcode,errcode2;
  errcode  = R->Draw("chsync1-rsync1",goodcut+
		     "(abs(chsync1-rsync1)>2)","goff");
  errcode2 = R->Draw("chsync2-rsync2",goodcut+
		     "(abs(chsync1-rsync1)>2)","goff");
  if (errcode==0 && errcode2==0) {
    cout << endl
	 << "  - Counting House is synchronized with Right Spectrometer" 
	 << endl;
  } else {
    cout << endl
	 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "!                SYNCHRONIZATION ERROR" << endl;
    cout << "! Counting House out of synch with Right Spectrometer" << endl;
    TString whichgate;
    if(errcode==0) {
      whichgate="Gate 1";
    } else {
      whichgate="Gate 2";
    }
    cout << "!    Check " << whichgate << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  }

  errcode  = R->Draw("chsync1-lsync1",goodcut+
		     "(abs(chsync1-lsync1)>2)","goff");
  errcode2 = R->Draw("chsync2-lsync2",goodcut+
		     "(abs(chsync1-lsync1)>2)","goff");
  if (errcode==0 && errcode2==0) {
    cout << endl
	 << "  - Counting House is synchronized with Left Spectrometer" 
	 << endl;
  } else {
    cout << endl
	 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "!                SYNCHRONIZATION ERROR" << endl;
    cout << "! Counting House out of synch with Left Spectrometer" << endl;
    TString whichgate;
    if(errcode==0) {
      whichgate="Gate 1";
    } else {
      whichgate="Gate 2";
    }
    cout << "!    Check " << whichgate << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  }

  errcode  = R->Draw("rsync1-lsync1",goodcut+
		     "(abs(rsync1-lsync1)>2)","goff");
  errcode2 = R->Draw("rsync2-lsync2",goodcut+
		     "(abs(rsync1-lsync1)>2)","goff");
  if (errcode==0 && errcode2==0) {
    cout << endl
	 << "  - Right Spectrometer is synchronized with Left Spectrometer" 
	 << endl;
  } else {
    cout << endl
	 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "!                SYNCHRONIZATION ERROR" << endl;
    cout << "! Right Spectrometer out of synch with Left Spectrometer" << endl;
    TString whichgate;
    if(errcode==0) {
      whichgate="Gate 1";
    } else {
      whichgate="Gate 2";
    }
    cout << "!    Check " << whichgate << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  }
  cout << endl;

  // Software check.  

  // This checks to make sure all of the pitadac readouts (from each
  // timing board) readout the same number.  Only output something if
  // there's a non-equal pitadac.

  errcode  = 
    R->Draw("ev_num"
	    ,goodcut&&
	    "((pitadac!=pitadac1)||(pitadac!=pitadac2)||(pitadac!=pitadac3))"
	    ,"goff");
  if(errcode!=0) {
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "! PITADACs ARE NOT IN SYNC" << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << endl;
  }
  delete c4;
}
