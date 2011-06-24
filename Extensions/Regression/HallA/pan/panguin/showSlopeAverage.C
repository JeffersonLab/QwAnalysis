//////////////////////////////////////////////////////////////////////
//
// showSlopeAverage.C
//   Bryan Moffit - July 2005
//  
//  helper routine for PANGUIN.  Plots the run average of a given
//  quantity
//

void showSlopeAverage(TString treename, TString given_quantity, 
		    TCut given_cut) {

  gROOT->LoadMacro("macro/Averager.C");
  TTree *T = (TTree*)gROOT->FindObject(treename);

  TPaveText *pt = new TPaveText(0.20,0.15,0.80,0.25,"NDC");

  //Get the largest slug number, and remove it
  // because it's usually after the one we want anyway
  UInt_t notthisslug = (UInt_t)T->GetMaximum("slug");
  cout << "Not including slug " << notthisslug << endl;
  TCut usethiscut = given_cut;
  usethiscut += Form("(slug!=0&&slug!=%d)",notthisslug);
  
  TGraphErrors *gr_slope = Averager(T, given_quantity, 
				    "slug",usethiscut);
  TF1 *slopefit = new TF1("slopefit","pol0",0,notthisslug);
  slopefit->SetLineColor(2);
  if(gr_slope!=NULL) {
    gr_slope->SetMarkerStyle(23);
    gr_slope->SetMarkerColor(1);
    gr_slope->Fit(slopefit,"q");
    gr_slope->SetMaximum(abs(0.125*gr_slope->GetYaxis()->GetXmax())
		   +gr_slope->GetYaxis()->GetXmax());
    gr_slope->SetMinimum(-abs(0.125*gr_slope->GetYaxis()->GetXmin())
		   +gr_slope->GetYaxis()->GetXmin());
    pt->AddText(Form("#mu = %.2f #pm %.2f ppm/#mum  #chi^{2}/ndf = %.1f/%d",
		     slopefit->GetParameter(0),
		     slopefit->GetParError(0),
		     slopefit->GetChisquare(),
		     slopefit->GetNDF()));
  }

//   gPad->SetGridy(1);
  gPad->SetTickx(1);
  gPad->SetTicky(1);
  gr_slope->Draw("ap");
  pt->Draw();


}
