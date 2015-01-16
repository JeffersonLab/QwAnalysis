#include "DetectorSlopes.h"
/////////////////////////////////////////////////////////////
//Class DetectorSlopes written to handle plotting of       //
//detector data such as sensitivities to monitors (slopes),//
//coils (coefficients), residual sensitivities to coils.   //
//                                                         //
//Author: Donald Jones                                     //
//January 2014                                             //
/////////////////////////////////////////////////////////////


DetectorSlopes::DetectorSlopes(){
  fNDet = 0;
  fLineColor[0] = kRed;
  fLineColor[1] = kBlue;
  fLineColor[2] = kBlack;
  fMarkerColor[0] = kRed;
  fMarkerColor[1] = kBlue;
  fMarkerColor[2] = kBlack;
  fMarkerStyle[0] = 1;
  fMarkerStyle[1] = 1;
  fMarkerStyle[2] = 1;
  sTitleX = "Run";
}

DetectorSlopes::DetectorSlopes(Int_t ndet)
{
  fNDet = ndet;
  fLineColor[0] = kRed;
  fLineColor[1] = kBlue;
  fLineColor[2] = kBlack;
  fMarkerColor[0] = kRed;
  fMarkerColor[1] = kBlue;
  fMarkerColor[2] = kBlack;
  sTitleX = "Run";
}

DetectorSlopes::~DetectorSlopes()
{
}

void DetectorSlopes::Plot(vector<Double_t>& vX, vector<Double_t>& vXErr, 
			  vector<vector<vector<Double_t > > > & vSlopes1,
			  vector<vector<vector<Double_t > > > & vSlopesErr1)
{
  int iplot = 0, idx = 0;
  for(int imon=0;imon<kNMon;++imon){
    int pad1[9] = {5,4,1,2,3,6,9,8,7}, pad2 = 1, pad3 = 1;
    cMD[iplot] = new TCanvas(Form("cMD_%i",iplot),
			     Form("cMD%s",sMonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i",iplot),
			      Form("cBkg%s",sMonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i",iplot),
			     Form("charge%s",sMonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);

    for(int idet=0;idet<fNDet;++idet){
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	pad3++;
      }
      Int_t size = (Int_t)vSlopes1[idet][imon].size();
      Double_t *x = vX.data();
      Double_t *xErr = vXErr.data();
      Double_t *slopes1 = vSlopes1[idet][imon].data();
      Double_t *slopesErr1 = vSlopesErr1[idet][imon].data();
      gr1[idx] = new TGraphErrors(size,x,slopes1,xErr,slopesErr1);
      gr1[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr1[idx]->SetMarkerColor(fMarkerColor[0]);
      gr1[idx]->SetLineColor(fLineColor[0]);
 
      mg[idx] = new TMultiGraph(Form("mg%i",idx),Form("mg%i",idx));

      mg[idx]->Add(gr1[idx]);
      mg[idx]->Draw("ap");
      mg[idx]->SetTitle(Form("Slope %s / %s",
			     sDetectorList[idet].Data(),
			     sMonitorList[imon].Data()));
      mg[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 sDetectorList[idet].Data(),
					 sMonitorList[imon].Data(),
					 (sMonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());
      double font_size = 0.049;
      if(sDetectorList[idet].Contains("pmt")){
	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }
      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }

}

void DetectorSlopes::Plot(vector<Double_t>& vX, vector<Double_t>& vXErr, 
			  vector<vector<vector<Double_t > > > & vSlopes1,
			  vector<vector<vector<Double_t > > > & vSlopesErr1,
			  vector<vector<vector<Double_t > > > & vSlopes2,
			  vector<vector<vector<Double_t > > > & vSlopesErr2)
{
  int iplot = 0, idx = 0;
  for(int imon=0;imon<kNMon;++imon){
    int pad1[9] = {5,4,1,2,3,6,9,8,7}, pad2 = 1, pad3 = 1;
    cMD[iplot] = new TCanvas(Form("cMD_%i",iplot),
			     Form("cMD%s",sMonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i",iplot),
			      Form("cBkg%s",sMonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i",iplot),
			     Form("charge%s",sMonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);

    for(int idet=0;idet<fNDet;++idet){
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	pad3++;
      }
      Int_t size = (Int_t)vSlopes1[idet][imon].size();
      Double_t *x = vX.data();
      Double_t *xErr = vXErr.data();
      Double_t *slopes1 = vSlopes1[idet][imon].data();
      Double_t *slopes2 = vSlopes2[idet][imon].data();
      Double_t *slopesErr1 = vSlopesErr1[idet][imon].data();
      Double_t *slopesErr2 = vSlopesErr2[idet][imon].data();
      gr1[idx] = new TGraphErrors(size,x,slopes1,xErr,slopesErr1);
      gr1[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr1[idx]->SetMarkerColor(fMarkerColor[0]);
      gr1[idx]->SetLineColor(fLineColor[0]);
      gr2[idx] = new TGraphErrors(size,x,slopes2,xErr,slopesErr2);
      gr2[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr2[idx]->SetMarkerColor(fMarkerColor[1]);
      gr2[idx]->SetLineColor(fLineColor[1]);

      mg[idx] = new TMultiGraph(Form("mg%i",idx),Form("mg%i",idx));

      mg[idx]->Add(gr1[idx]);
      mg[idx]->Add(gr2[idx]);
      mg[idx]->Draw("ap");
      mg[idx]->SetTitle(Form("Slope %s / %s",
			     sDetectorList[idet].Data(),
			     sMonitorList[imon].Data()));
      mg[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 sDetectorList[idet].Data(),
					 sMonitorList[imon].Data(),
					 (sMonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());
      double font_size = 0.049;
      if(sDetectorList[idet].Contains("pmt")){
	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }
      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }

}

void DetectorSlopes::Plot(vector<Double_t>& vX, vector<Double_t>& vXErr, 
			  vector<vector<vector<Double_t > > > & vSlopes1,
			  vector<vector<vector<Double_t > > > & vSlopesErr1,
			  vector<vector<vector<Double_t > > > & vSlopes2,
			  vector<vector<vector<Double_t > > > & vSlopesErr2,
			  vector<vector<vector<Double_t > > > & vSlopes3,
			  vector<vector<vector<Double_t > > > & vSlopesErr3)
{
  int iplot = 0, idx = 0;
  for(int imon=0;imon<kNMon;++imon){
    int pad1[9] = {5,4,1,2,3,6,9,8,7}, pad2 = 1, pad3 = 1;
    cMD[iplot] = new TCanvas(Form("cMD_%i",iplot),
			     Form("cMD%s",sMonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i",iplot),
			      Form("cBkg%s",sMonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i",iplot),
			     Form("charge%s",sMonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);

    for(int idet=0;idet<fNDet;++idet){
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	pad3++;
      }
      Int_t size = (Int_t)vSlopes1[idet][imon].size();
      Double_t *x = vX.data();
      Double_t *xErr = vXErr.data();
      Double_t *slopes1 = vSlopes1[idet][imon].data();
      Double_t *slopes2 = vSlopes2[idet][imon].data();
      Double_t *slopes3 = vSlopes3[idet][imon].data();
      Double_t *slopesErr1 = vSlopesErr1[idet][imon].data();
      Double_t *slopesErr2 = vSlopesErr2[idet][imon].data();
      Double_t *slopesErr3 = vSlopesErr3[idet][imon].data();
      gr1[idx] = new TGraphErrors(size,x,slopes1,xErr,slopesErr1);
      gr1[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr1[idx]->SetMarkerColor(fMarkerColor[0]);
      gr1[idx]->SetLineColor(fLineColor[0]);
      gr2[idx] = new TGraphErrors(size,x,slopes2,xErr,slopesErr2);
      gr2[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr2[idx]->SetMarkerColor(fMarkerColor[1]);
      gr2[idx]->SetLineColor(fLineColor[1]);
      gr3[idx] = new TGraphErrors(size,x,slopes3,xErr,slopesErr3);
      gr3[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr3[idx]->SetMarkerColor(fMarkerColor[2]);
      gr3[idx]->SetLineColor(fLineColor[2]);

      mg[idx] = new TMultiGraph(Form("mg%i",idx),Form("mg%i",idx));

      mg[idx]->Add(gr1[idx]);
      mg[idx]->Add(gr2[idx]);
      mg[idx]->Add(gr3[idx]);
      mg[idx]->Draw("ap");
      mg[idx]->SetTitle(Form("Slope %s / %s",
			     sDetectorList[idet].Data(),
			     sMonitorList[imon].Data()));
      mg[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 sDetectorList[idet].Data(),
					 sMonitorList[imon].Data(),
					 (sMonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());
      double font_size = 0.049;
      if(sDetectorList[idet].Contains("pmt")){
	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }
      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }

}

void DetectorSlopes::SetDetectorList(TString *list, Int_t ndet)
{
  fNDet = ndet;
  for(int idet=0;idet<ndet;++idet){
    sDetectorList[idet] = list[idet];
  }
}

void DetectorSlopes::SetLineColor(Int_t graph, Int_t color)
{
  fLineColor[graph] = color;
}

void DetectorSlopes::SetMarkerStyle(Int_t graph, Int_t style)
{
  fMarkerStyle[graph] = style;
}

void DetectorSlopes::SetMarkerColor(Int_t graph, Int_t color)
{
  fMarkerColor[graph] = color;
}

void DetectorSlopes::SetMonitorList(TString *list)
{
  for(int imon=0;imon<kNMon;++imon){
    sMonitorList[imon] = list[imon];
  }
}

void DetectorSlopes::SaveToFile()
{
  for(int imon=0;imon<kNMon;imon++){
    cMD[imon]->SaveAs(Form("%s/plots/MD_slopes_to_%s_vs_%s.png",
			   gSystem->Getenv("BMOD_OUT"),
			   sMonitorList[imon].Data(), sTitleX.Data()));
    cBkg[imon]->SaveAs(Form("%s/plots/Bkg_slopes_to_%s_vs_%s.png",
			    gSystem->Getenv("BMOD_OUT"),
			    sMonitorList[imon].Data(), sTitleX.Data()));
    cChg[imon]->SaveAs(Form("%s/plots/Charge_slopes_to_%s_vs_%s.png",
			    gSystem->Getenv("BMOD_OUT"),
			    sMonitorList[imon].Data(), sTitleX.Data()));
  }
}

void DetectorSlopes::SetDetectorNumber(Int_t ndet)
{
  fNDet = ndet;
}

void DetectorSlopes::SetTitleX(char *title)
{
  sTitleX = title;
}
