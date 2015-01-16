{
  gROOT->SetStyle("Plain");
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.3);
  gStyle->SetTitleBorderSize(0);
  int slide = 2;

  //*****slide 1

  if(slide==1){
    TFile *file = TFile::Open("/net/data2/paschkelab2/DB_rootfiles/run1/HYDROGEN-CELL_offoff_tree.root");
    TTree *tr = (TTree*)file->Get("tree");
    tr->AddFriend("reduced_tree","/net/data2/paschkelab2/DB_rootfiles/run1/hydrogen_cell_reduced_tree.root");
    TCanvas *c1 = new TCanvas("c1","Entries Per Runlet", 0,0,1500,1000);
    c1->Divide(3,2);
    c1->cd(1);
    TPaveText *pt1 = new TPaveText(0,0.6,0.98,1);
    pt1->SetTextColor(kRed);
    pt1->AddText("Run 1 DB Rootfiles ( Good Parity Prod LH2)");
    //    pt1->AddLine(0,0.75,0.98,0.75);
    pt1->AddText("181 runlets in with slug==0 or slug>5e5");
    pt1->AddText("223 further runlets with 0 entries");
    pt1->SetFillColor(0);
    pt1->SetShadowColor(0);
    pt1->Draw();

    TPaveText *pt2 = new TPaveText(0,0.2,0.98,0.6);
    pt2->SetTextColor(kBlue);
    pt2->AddText("Run 1 Reduced Sluglets (Good Parity Prod LH#_2)");
    //    pt2->AddLine(0,0.75,0.98,0.75);
    pt2->AddText("63 runlets missing from tape");
    pt2->AddText("2 runlets corrupted on tape");
    pt2->AddText("2 runlets don't agree on #entries, 1 on value");
    pt2->SetFillColor(0);
    pt2->SetShadowColor(0);
    pt2->Draw();

    TPaveText *pt3 = new TPaveText(0,0.01,0.98,0.2);
    pt3->SetTextColor(kMagenta);
    pt3->AddText("6299 runlets, 472 cut");
    pt3->SetFillColor(0);
    pt3->SetShadowColor(0);
    pt3->Draw();

    TPaveText *pt4 = new TPaveText(0,0.5,0.98,0.98);
    pt4->SetTextColor(kRed);
    pt4->AddText("Difference in Values");
    //   pt4->AddText("(DB rootfile - Sluglet)");
    pt4->SetFillColor(0);
    pt4->SetShadowColor(0);

    TPaveText *pt5 = new TPaveText(0,0.5,0.98,0.98);
    pt5->SetTextColor(kRed);
    pt5->AddText("Difference in RMS");
    //    pt5->AddText("(DB rootfile - Sluglet)");
    pt5->SetFillColor(0);
    pt5->SetShadowColor(0);

    TPaveText *pt6 = new TPaveText(0,0.02,0.98,0.5);
    pt6->AddText("      (All units are DB*1e6)      ");
    pt6->AddText("->Asym: PPM");
    pt6->AddText("->Diff: nm ");
    pt6->SetFillColor(0);
    pt6->SetShadowColor(0);

    TString det[5] = {"asym_qwk_mdallbars", "asym_uslumi_sum", "asym_qwk_md9barsum",
		      "diff_qwk_bpm3c12X", "diff_qwk_bpm3h09bX"};
    TString var[3] = {".value",".rms",".n"};
    TH1D *h[5];
    for(int i=0;i<5;i++){
      c1->cd(i+2)->SetLogy();
      tree->Draw(Form("%s%s-hydrogen_cell_reduced_tree.%s%s>>h%i",det[i].Data(),var[2].Data(),det[i].Data(),var[2].Data(), i), 
		 "good");
      h[i] = (TH1D*)gDirectory->Get(Form("h%i",i));
      h[i]->SetMarkerColor(kBlue);
      h[i]->SetTitle("DB Rootfile - Sluglets");
      TString title = det[i]+var[2];
      h[i]->GetXaxis()->SetTitle(title.Data());
    }

    TCanvas *c2 = new TCanvas("c2","Asymmetries and Differences", 0,0,1500,1000);
    c2->Divide(3,2);
    c2->cd(1);
    pt4->Draw();
    pt6->Draw();
    TH1D *h1[5];
    for(int i=0;i<5;i++){
      c2->cd(i+2)->SetLogy();
      tree->Draw(Form("%s%s-hydrogen_cell_reduced_tree.%s%s>>h1%i",det[i].Data(),var[0].Data(),det[i].Data(),var[0].Data(), i), 
		 "good");
      h1[i] = (TH1D*)gDirectory->Get(Form("h1%i",i));
      h1[i]->SetMarkerColor(kBlue);
      h1[i]->SetTitle("DB Rootfile - Sluglets");
      TString title = det[i]+var[0] + (det[i].Contains("asym")? "(ppm)" : "(nm)");
      h1[i]->GetXaxis()->SetTitle(title.Data());
    }

    TCanvas *c3 = new TCanvas("c3","RMS", 0,0,1500,1000);
    c3->Divide(3,2);
    c3->cd(1);
    pt5->Draw();
    pt6->Draw();
    TH1D *h2[5];
    for(int i=0;i<5;i++){
      c3->cd(i+2)->SetLogy();
      tree->Draw(Form("%s%s-hydrogen_cell_reduced_tree.%s%s>>h2%i",det[i].Data(),var[1].Data(),det[i].Data(),var[1].Data(), i), 
		 "good");
      h2[i] = (TH1D*)gDirectory->Get(Form("h2%i",i));
      h2[i]->SetMarkerColor(kBlue);
      h2[i]->SetTitle("DB Rootfile - Sluglets");
      TString title = det[i]+var[1] + (det[i].Contains("asym")? "(ppm)" : "(nm)");
      h2[i]->GetXaxis()->SetTitle(title.Data());
    }

    c1->SaveAs("~/plots/temp/DBminusSluglet_N.png");
    c2->SaveAs("~/plots/temp/DBminusSluglet_value.png");
    c3->SaveAs("~/plots/temp/DBminusSluglet_rms.png");
  }



  //*****slide2 2

  if(slide==2){
    TFile *file = TFile::Open("/net/data2/paschkelab2/DB_rootfiles/run1/HYDROGEN-CELL_offoff_tree.root");
    TTree *tr = (TTree*)file->Get("tree");
    int i = tr->AddFriend("reduced_tree","/net/data2/paschkelab2/DB_rootfiles/run1/hydrogen_cell_reduced_tree.root");
    cout<<i<<endl;
    i = tr->AddFriend("corrected_tree","/net/data2/paschkelab2/DB_rootfiles/run1/hydrogen_cell_corrected_tree.root");
    cout<<i<<endl;
    i = tr->AddFriend("on_tree=tree","/net/data2/paschkelab2/DB_rootfiles/run1/HYDROGEN-CELL_on_tree.root");
    cout<<i<<endl;
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);
    TCanvas *c = new TCanvas("c","c",0,0,1500,1000);
    c->Divide(2,2);
    c->cd(1);
    tr->Draw("(asym_qwk_mdallbars.value-corrected_asym_qwk_mdallbars.value)*sign_correction:(asym_qwk_mdallbars.value-on_tree.asym_qwk_mdallbars.value)*sign_correction>>h(100,-1,1,100,-1,1)","good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist","prof");
    TProfile *pr = (TProfile*)gDirectory->Get("h");
    pr->Approximate(1);
    pr->Fit("pol1");
    pr->SetTitle("Dithering Correction vs Regression Correction (Runlet Level Sign Corrected)");
    pr->GetXaxis()->SetTitle("Regression Correction (ppm)");
    pr->GetYaxis()->SetTitle("Dithering Correction (ppm)");
    c->cd(2);
    tr->Draw("sign_correction*(asym_qwk_mdallbars.value-corrected_asym_qwk_mdallbars.value):sign_correction*(asym_qwk_mdallbars.value-on_tree.asym_qwk_mdallbars.value)>>h1(1000,-1,1,1000,-1,1)","good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist");
    TH2D *h1 = (TH2D*)gDirectory->Get("h1");
    h1->Fit("pol1");
    h1->SetTitle("Dithering Correction vs Regression Correction (Runlet Level Sign Corrected)");
    h1->GetXaxis()->SetTitle("Regression Correction (ppm)");
    h1->GetYaxis()->SetTitle("Dithering Correction (ppm)");

    c->cd(3);
    tr->Draw("sign_correction*(asym_qwk_mdallbars.value-corrected_asym_qwk_mdallbars.value):run_number_decimal>>h2","good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist");
    TH2D *h2 = (TH2D*)gDirectory->Get("h2");
    h2->SetTitle("Dithering(red) and Regression(blue) Corrections vs Run (Sign Corrected)");
    h2->GetXaxis()->SetTitle("Run Number");
    h2->GetYaxis()->SetTitle("Correction (ppm)");
    h2->SetMarkerColor(kRed);
    gPad->Update();

    tr->Draw("sign_correction*(asym_qwk_mdallbars.value-on_tree.asym_qwk_mdallbars.value):run_number_decimal>>h3","good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist","same");
    TH2D *h3 = (TH2D*)gDirectory->Get("h3");
    h3->SetMarkerColor(kBlue);
    gPad->Update();

    c->cd(4);

    tr->Draw("sign_correction*(asym_qwk_mdallbars.value-corrected_asym_qwk_mdallbars.value)-sign_correction*(asym_qwk_mdallbars.value-on_tree.asym_qwk_mdallbars.value):run_number_decimal>>h4","good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist");
    TH2D *h4 = (TH2D*)gDirectory->Get("h4");
    h4->SetTitle("(Dithering Correction - Regression Correction) vs Run (Sign Corrected)");
    h4->GetXaxis()->SetTitle("Run Number");
    h4->GetYaxis()->SetTitle("#DeltaCorrection (ppm)");
    TCanvas *c1 = new TCanvas("c1","c1",0,0,800,600);

    tr->Draw("sign_correction*(asym_qwk_mdallbars.value-corrected_asym_qwk_mdallbars.value)-sign_correction*(asym_qwk_mdallbars.value-on_tree.asym_qwk_mdallbars.value)>>h5","good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist");

    TH2D *h5 = (TH2D*)gDirectory->Get("h5");
    h5->SetTitle("(Dithering Correction - Regression Correction)*sign_correction");
    h5->GetXaxis()->SetTitle("#DeltaCorrection (ppm)");

  }


  //*****slide 3

  if(slide==3){


    for(int i=0;i<8;++i){

    }

  }

}
