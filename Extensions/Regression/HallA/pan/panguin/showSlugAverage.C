//////////////////////////////////////////////////////////////////////
//
// showSlugAverage.C
//   Bryan Moffit - July 2005
//  
//  helper routine for PANGUIN.  Plots the run average of a given
//  quantity
//

void showSlugAverage(TString treename, TString given_quantity, 
		    TCut given_cut) {

  gROOT->LoadMacro("macro/Averager.C");
  TTree *T = (TTree*)gROOT->FindObject(treename);

  TMultiGraph *mg = new TMultiGraph("mg",given_quantity+";slug");
  TLegend *leg = new TLegend(0.50,0.7,0.95,0.95);
  // First, do the slowsign==1
  TGraphErrors *gr_out = Averager(T, given_quantity, 
				    "slug",given_cut+"sign==1");
  TF1 *fit_out = new TF1("fit_out","pol0");
  fit_out->SetLineColor(1);
  if(gr_out!=NULL) {
    gr_out->SetMarkerStyle(23);
    gr_out->SetMarkerColor(1);
    gr_out->Fit(fit_out,"q");
    mg->Add(gr_out,"p");
    leg->AddEntry(gr_out,
		  Form("OUT: #mu = %.3f #pm %.3f ppm  #chi^{2}/ndf = %.2f/%d  prob = %.3f",
		       fit_out->GetParameter(0),
		       fit_out->GetParError(0),
		       fit_out->GetChisquare(),
		       fit_out->GetNDF(),
		       fit_out->GetProb()),"p");
  }

  // Now, do the slowsign==-1
  TGraphErrors *gr_in = Averager(T, given_quantity, 
				    "slug",given_cut+"sign==-1");
  TF1 *fit_in = new TF1("fit_in","pol0");
  fit_in->SetLineColor(2);
  if(gr_in!=NULL) {
    gr_in->SetMarkerStyle(21);
    gr_in->SetMarkerColor(2);
    gr_in->SetLineColor(2);
    gr_in->Fit(fit_in,"q");
    mg->Add(gr_in,"p");
    leg->AddEntry(gr_in,
		  Form("IN: #mu = %.3f #pm %.3f ppm  #chi^{2}/ndf = %.2f/%d  prob = %.3f",
		       fit_in->GetParameter(0),
		       fit_in->GetParError(0),
		       fit_in->GetChisquare(),
		       fit_in->GetNDF(),
		       fit_in->GetProb()),"p");
  }

  // Now, do the slowsign==-1
  TGraphErrors *gr_all = Averager(T, given_quantity+"*sign", 
				    "slug",given_cut);
  TF1 *fit_all = new TF1("fit_all","pol0");
  if(gr_all!=NULL) {
    gr_all->Fit(fit_all,"q");
    leg->AddEntry(gr_all,
		  Form("MEAN = %.3f #pm %.3f ppm  #chi^{2}/ndf = %.2f/%d  prob = %.3f",
		       fit_all->GetParameter(0),
		       fit_all->GetParError(0),
		       fit_all->GetChisquare(),
		       fit_all->GetNDF(),
		       fit_all->GetProb()),"");
  }

  gPad->SetGridy(1);
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  mg->Draw("a");
  leg->Draw();


}
