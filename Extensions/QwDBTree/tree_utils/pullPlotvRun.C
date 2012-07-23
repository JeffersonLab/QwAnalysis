void pullPlotvRun(TString var="asym_cagesr", Int_t start = 9418, Int_t stop = 9812){

gStyle->SetOptStat("eMR");
gStyle->SetOptLogy(1);


Double_t mean;
tree->Draw(Form("%s>>hh",var.Data()),Form("%s.n>0",var.Data()),"goff");

mean = hh->GetMean();

TCanvas *c1 = new TCanvas();
c1->Divide(1,2);

c1->cd(1);
tree->Draw(Form("(%s-%f)/%s.err>>h1",var.Data(),mean,var.Data()),Form("%s.n>0 && run>%d && run<%d",var.Data(), start, stop));

c1->cd(2);

tree->Draw(Form("((%s-%f)/%s.err):run>>h2",var.Data(),mean,var.Data()),Form("%s.n>0 && run>%d && run<%d",var.Data(), start, stop));

}
