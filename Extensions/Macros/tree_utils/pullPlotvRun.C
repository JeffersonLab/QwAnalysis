/*
pullPlotvRun v1.1 --smacewan (July 2012)
pullPlotvRun should be run while in a ROOT session with a QWeak dbROOTfile loaded into memory.
*/


void pullPlotvRun(TString var="asym_mdallbars", Int_t start = 9418, Int_t stop = 9812){
gStyle->SetOptStat("eMR");

Double_t mean_in, mean_out;

TString cut = Form("%s.n>0 && run>=%d && run<=%d",var.Data(),start,stop);

//Get means for pull calculation
tree->Draw(Form("%s",var.Data()),Form("ihwp_setting==0 && %s",cut.Data()),"goff");
mean_out = htemp->GetMean();
tree->Draw(Form("%s",var.Data()),Form("ihwp_setting==1 && %s",cut.Data()),"goff");
mean_in = htemp->GetMean();

TCanvas *c1 = new TCanvas("c1","Pull Plots",1200,750);
c1->Divide(2,2);

//Plots
c1->cd(1);
tree->Draw(Form("(%s-%f)/%s.err>>hin",var.Data(),mean_in,var.Data()),Form("ihwp_setting==1 && %s",cut.Data()));
c1->cd(3);
tree->Draw(Form("((%s-%f)/%s.err):runlet_id>>hhin",var.Data(),mean_in,var.Data()),Form("ihwp_setting==1 && %s",cut.Data()));

c1->cd(2);
tree->Draw(Form("(%s-%f)/%s.err>>hout",var.Data(),mean_out,var.Data()),Form("ihwp_setting==0 && %s",cut.Data()));
c1->cd(4);
tree->Draw(Form("((%s-%f)/%s.err):runlet_id>>hhout",var.Data(),mean_out,var.Data()),Form("ihwp_setting==0 && %s",cut.Data()));


//Set Titles
hin->SetTitle(Form("Pull Plot for %s [IHWP IN]",var.Data()));
hhin->SetTitle(Form("Pull versus Runlet_id for %s [IHWP IN]",var.Data()));
hout->SetTitle(Form("Pull Plot for %s [IHWP OUT]",var.Data()));
hhout->SetTitle(Form("Pull versus Runlet_id for %s [IHWP OUT]",var.Data()));

hin->GetXaxis()->SetTitle("Pull");
hhin->GetXaxis()->SetTitle("Runlet_id");
hout->GetXaxis()->SetTitle("Pull");
hhout->GetXaxis()->SetTitle("Runlet_id");

hin->GetYaxis()->SetTitle("Entries");
hhin->GetYaxis()->SetTitle("Pull");
hout->GetYaxis()->SetTitle("Entries");
hhout->GetYaxis()->SetTitle("Pull");


//Set Style
hin->SetLineColor(kBlue);
hhin->SetMarkerStyle(21);
hhin->SetMarkerColor(kBlue);
hhin->SetMarkerSize(0.3);

hout->SetLineColor(kRed);
hhout->SetMarkerStyle(21);
hhout->SetMarkerColor(kRed);
hhout->SetMarkerSize(0.3);

//Redraw plots to get formatting right...didn't want to define histograms before drawing
c1->cd(1);hin->Draw();
c1->cd(2);hout->Draw();
c1->cd(3);hhin->Draw();
c1->cd(4);hhout->Draw();
}
