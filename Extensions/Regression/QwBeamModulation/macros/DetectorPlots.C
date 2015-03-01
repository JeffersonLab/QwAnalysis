#include "DetectorPlots.h"
/////////////////////////////////////////////////////////////
//Class DetectorSlopes written to handle plotting of       //
//detector data such as sensitivities to monitors (slopes),//
//coils (coefficients), residual sensitivities to coils.   //
//                                                         //
//Author: Donald Jones                                     //
//January 2014                                             //
/////////////////////////////////////////////////////////////


DetectorPlots::DetectorPlots(){
  fName = "";
  fNDet = 0;
  fDrawLegend = false;
  fRunPeriod = 0;
  fCoilType = 0;
  fPlotType = 0;
  fLineColor[0] = kRed;
  fLineColor[1] = kBlue;
  fLineColor[2] = kBlack;
  fMarkerColor[0] = kRed;
  fMarkerColor[1] = kBlue;
  fMarkerColor[2] = kBlack;
  fMarkerStyle[0] = 0;
  fMarkerStyle[1] = 0;
  fMarkerStyle[2] = 0;
  sTitleX = "Run";
}

DetectorPlots::DetectorPlots(Int_t ndet, Int_t plot_type)
{
  fName = "";
  fNDet = ndet;
  fDrawLegend = false;
  fRunPeriod = 0;
  fCoilType = 0;
  fPlotType = plot_type;//0-det to mon slopes 1-det to coil 2-residual det to coil
  fLineColor[0] = kRed;
  fLineColor[1] = kBlue;
  fLineColor[2] = kBlack;
  fMarkerColor[0] = kRed;
  fMarkerColor[1] = kBlue;
  fMarkerColor[2] = kBlack;
  fMarkerStyle[0] = 7;
  fMarkerStyle[1] = 7;
  fMarkerStyle[2] = 7;
  sTitleX = "Run";
}

DetectorPlots::~DetectorPlots()
{
}

void DetectorPlots::AppendToFileName(char *name){
  fName = name;
}

void DetectorPlots::GetTitle(TString &title, Int_t det, Int_t n)
{
  switch(fPlotType)
    {
    case 0:
      title = Form("%s/%s vs %s", sDetectorList[det].Data(),
		   sMonitorList[n].Data(), sTitleX.Data());
      break;

    case 1:
      title =  Form("%s %s/Coil%i vs %s",(fCoilType==0 ? "Sine" : "Cosine"),
		    sDetectorList[det].Data(), n, sTitleX.Data());
      break;

    case 2:
      title =  Form("Residual %s %s/Coil%i vs %s",
		    (fCoilType==0 ? "Sine" : "Cosine"),
		    sDetectorList[det].Data(), n, sTitleX.Data());
      break;

    default:
      title = "?";
    }
  return;
}


void DetectorPlots::GetTitleY(TString &title, Int_t det, Int_t n)
{
  switch(fPlotType)
    {
    case 0:
      title = Form("%s/%s (%s)", sDetectorList[det].Data(),sMonitorList[n].Data(),
		   (sMonitorList[n].Contains("Slope") ? "ppm/#murad":"ppm/#mum"));
      break;

    case 1:
      title =  Form("%s/Coil%i (%s)",sDetectorList[det].Data(),n,"ppm");
      break;

    case 2:
      title =  Form("Residual %s/Coil%i (%s)",sDetectorList[det].Data(),n,"ppm");
      break;

    default:
      title = "?";
    }
  return;
}

void DetectorPlots::Plot(vector<Double_t>& vX, vector<Double_t>& vXErr, 
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
      int selected = 0;
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	selected = 1;
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	selected = 2;
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
      TString myTitle;
      GetTitle(myTitle, idet, imon);
      mg[idx]->SetTitle(myTitle.Data());
      GetTitleY(myTitle, idet, imon);
      mg[idx]->GetYaxis()->SetTitle(myTitle.Data());
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());
      double font_size = 0.049;
      if(sDetectorList[idet].Contains("pmt")){
	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
	if(fPlotType){
	  for(int i=0;i<kNMod;++i)	 
	    val[i] =8000;
	}
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }else if(selected == 2){
	Double_t val[kNMod] = {5,40,5,5,40}; 
	if(fPlotType){
	  for(int i=0;i<kNMod;++i)
	    val[i] =100;
	}
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);

      }

      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      if(fDrawLegend){
	fPaveText[0]->Draw("same");
      }
      gPad->Update();
      idx++;
    }
    iplot++;
  }

}

void DetectorPlots::Plot(vector<vector<Double_t> >& vX, 
			 vector<vector<Double_t> >& vXErr, 
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
      int selected = 0;
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	selected = 1;
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	selected = 2;
	pad3++;
      }
      Int_t size = (Int_t)vSlopes1[idet][imon].size();
      Double_t *x = vX[imon].data();
      Double_t *xErr = vXErr[imon].data();
      Double_t *slopes1 = vSlopes1[idet][imon].data();
      Double_t *slopesErr1 = vSlopesErr1[idet][imon].data();
      gr1[idx] = new TGraphErrors(size,x,slopes1,xErr,slopesErr1);
      gr1[idx]->SetMarkerStyle(fMarkerStyle[0]);
      gr1[idx]->SetMarkerColor(fMarkerColor[0]);
      gr1[idx]->SetLineColor(fLineColor[0]);
 
      mg[idx] = new TMultiGraph(Form("mg%i",idx),Form("mg%i",idx));

      mg[idx]->Add(gr1[idx]);
      mg[idx]->Draw("ap");
      TString myTitle;
      GetTitle(myTitle, idet, imon);
      mg[idx]->SetTitle(myTitle.Data());
      GetTitleY(myTitle, idet, imon);
      mg[idx]->GetYaxis()->SetTitle(myTitle.Data());
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());

      double font_size = 0.049;
      if(sDetectorList[idet].Contains("pmt") && 0){
	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
	if(fPlotType){
	  for(int i=0;i<kNMod;++i)	 
	    val[i] =8000;
	}
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }else if(sDetectorList[idet].Contains("md")){
	mg[idx]->GetYaxis()->SetRangeUser(-200,150);

      }else if(selected==1){
	mg[idx]->GetYaxis()->SetRangeUser(-1500,1500);

      }else if(selected == 2){
	Double_t val[kNMod] = {5,40,5,5,40}; 
	if(fPlotType){
	  for(int i=0;i<kNMod;++i)
	    val[i] =100;
	}
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);

      }

      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      if(fDrawLegend){
	fPaveText[0]->Draw("same");
      }
      gPad->Update();
      idx++;
    }
    iplot++;
  }

}

void DetectorPlots::Plot(vector<Double_t>& vX, vector<Double_t>& vXErr, 
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
      int selected = 0;
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	selected = 1;
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	selected= 2;
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
      TString myTitle;
      GetTitle(myTitle, idet, imon);
      mg[idx]->SetTitle(myTitle.Data());
      GetTitleY(myTitle, idet, imon);
      mg[idx]->GetYaxis()->SetTitle(myTitle.Data());
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());
      double font_size = 0.049;
      if(sDetectorList[idet].Contains("pmt")){
	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
	if(fPlotType){
	  for(int i=0;i<kNMod;++i)	 
	    val[i] =8000;
	}
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }else if(selected == 2){
	Double_t val[kNMod] = {5,40,5,5,40}; 
	if(fPlotType){
	  for(int i=0;i<kNMod;++i)
	    val[i] =100;
	}
	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);

      }
      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      if(fDrawLegend){
	fPaveText[0]->Draw("same");
	fPaveText[1]->Draw("same");
      }
      gPad->Update();
      idx++;
    }
    iplot++;
  }

}

void DetectorPlots::Plot(vector<Double_t>& vX, vector<Double_t>& vXErr, 
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
    cMD[iplot] = new TCanvas(Form("cMD_%i_%i_%i",iplot, fPlotType, fCoilType),
			     Form("cMD%s",sMonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i_%i_%i",iplot, fPlotType, fCoilType),
			      Form("cBkg%s",sMonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i_%i_%i",iplot, fPlotType,fCoilType),
			     Form("charge%s",sMonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);

    for(int idet=0;idet<fNDet;++idet){
      int selected = 0;
      if(sDetectorList[idet].Contains("md")&& !sDetectorList[idet].Contains("9")){
	cMD[iplot]->ForceUpdate();
	cMD[iplot]->cd(pad1[idet])->SetGrid();
      }else if(sDetectorList[idet].Contains("lumi")||
	       sDetectorList[idet].Contains("pmt")||
	       sDetectorList[idet].Contains("md9")){
	cBkg[iplot]->ForceUpdate();
	cBkg[iplot]->cd(pad2)->SetGrid();
	selected = 1;
	pad2++;
      }else{
	cChg[iplot]->ForceUpdate();
	cChg[iplot]->cd(pad3)->SetGrid();
	selected = 2;
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
      TString myTitle;
      GetTitle(myTitle, idet, imon);
      mg[idx]->SetTitle(myTitle.Data());
      GetTitleY(myTitle, idet, imon);
      mg[idx]->GetYaxis()->SetTitle(myTitle.Data());
      mg[idx]->GetXaxis()->SetTitle(sTitleX.Data());
      double font_size = 0.049;
//       if(sDetectorList[idet].Contains("pmt")){
// 	Double_t val[kNMon]={400,4000, 50, 140, 3000}; 
// 	if(fPlotType){
// 	  for(int i=0;i<kNMod;++i)	 
// 	    val[i] =8000;
// 	}
// 	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
// 			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
// 	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
//       }else if(selected == 2){
// 	Double_t val[kNMod] = {5,40,5,5,40}; 
// 	if(fPlotType){
// 	  for(int i=0;i<kNMod;++i)
// 	    val[i] =100;
// 	}
// 	cout<<"Resetting axis range of "<<sDetectorList[idet].Data()<<"_"<<
// 			      sMonitorList[imon].Data()<<"to +/-"<<val<<endl;
// 	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);

//       }
      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      if(fDrawLegend){
	fPaveText[0]->Draw("same");
	fPaveText[1]->Draw("same");
	fPaveText[2]->Draw("same");
      }
      gPad->Update();
      idx++;
    }
    iplot++;
  }

}

void DetectorPlots::SaveToFile(TString stem)
{
  TString output = Form("/net/data1/paschkedata1/bmod_out%s", stem.Data());
  for(int i=0;i<kNMon;i++){
    if(fPlotType==0){
      cMD[i]->SaveAs(Form("%s/plots/%sMD_slopes_to_%s_vs_%s%s.png",
			  output.Data(),
			  (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			  sMonitorList[i].Data(), sTitleX.Data(),
			  fName.Data()));
      cBkg[i]->SaveAs(Form("%s/plots/%sBkg_slopes_to_%s_vs_%s%s.png",
			   output.Data(),
			  (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			   sMonitorList[i].Data(), sTitleX.Data(),
			   fName.Data()));
      cChg[i]->SaveAs(Form("%s/plots/%sCharge_slopes_to_%s_vs_%s%s.png",
			   output.Data(),
			  (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			   sMonitorList[i].Data(), sTitleX.Data(),
			   fName.Data()));
    }else if(fPlotType==1){
      cMD[i]->SaveAs(Form("%s/plots/%sMD_%s_sensitivities_to_Coil%i_vs_%s%s.png",
			  output.Data(),
			  (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			  (fCoilType ? "cosine":"sine"),i, sTitleX.Data(),
			  fName.Data()));
      cBkg[i]->SaveAs(Form("%s/plots/%sBkg_%s_sensitivities_to_Coil%i_vs_%s%s.png",
			   output.Data(),
			   (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			   (fCoilType ? "cosine":"sine"),i, sTitleX.Data(),
			   fName.Data()));
      cChg[i]->SaveAs(Form("%s/plots/%sCharge_%s_sensitivities_to_Coil%i_vs_%s%s."
			   "png",output.Data(),
			   (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			   (fCoilType ? "cosine":"sine"),i, sTitleX.Data(),
			   fName.Data()));
    }else{
      cMD[i]->SaveAs(Form("%s/plots/%sMD_%s_residuals_to_Coil%i_vs_%s%s.png",
			  output.Data(),
			  (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			  (fCoilType ? "cosine":"sine"),i, sTitleX.Data(),
			  fName.Data()));
      cBkg[i]->SaveAs(Form("%s/plots/%sBkg_%s_residuals_to_Coil%i_vs_%s%s.png",
			   output.Data(),
			   (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			   (fCoilType ? "cosine":"sine"),i, sTitleX.Data(),
			   fName.Data()));
      cChg[i]->SaveAs(Form("%s/plots/%sCharge_%s_residuals_to_Coil%i_vs_%s%s.png",
			   output.Data(),
			   (fRunPeriod==1 ? "Run1_" :(fRunPeriod==2 ? "Run2_":"")),
			   (fCoilType ? "cosine":"sine"),i, sTitleX.Data(),
			   fName.Data()));
    }
  }
}

void DetectorPlots::SetCoilType(Bool_t is_cosine)
{
  fCoilType = is_cosine;
  //  cout<<"Coil type set to "<<fCoilType<<".\n";
}

void DetectorPlots::SetDetectorList(TString *list, Int_t ndet)
{
  fNDet = ndet;
  sDetectorList = new TString[fNDet];
  for(int idet=0;idet<ndet;++idet){
    sDetectorList[idet] = list[idet].Data();
    //    cout<<sDetectorList[idet].Data()<<endl;
  }
}

void DetectorPlots::SetDetectorNumber(Int_t ndet)
{
  fNDet = ndet;
}

void DetectorPlots::SetDrawLegend(TString *legend, int n)
{
  fDrawLegend = true;
  double x1 = 0.84, x2 = 0.99, y1 = 0.88, y2 = 0.95;
  for(int i=0; i<n; ++i){
    fPaveText[i] = new TPaveText(x1, y1, x2, y2, "NDC");
    fPaveText[i]->AddText(legend[i].Data());
    fPaveText[i]->SetTextColor(fMarkerColor[i]);
    fPaveText[i]->SetFillColor(0);
    fPaveText[i]->SetShadowColor(0);
    y2 = y1;
    y1 -= 0.07;
  }
}

void DetectorPlots::SetLineColor(Int_t graph, Int_t color)
{
  fLineColor[graph] = color;
}

void DetectorPlots::SetMarkerStyle(Int_t graph, Int_t style)
{
  fMarkerStyle[graph] = style;
}

void DetectorPlots::SetMarkerColor(Int_t graph, Int_t color)
{
  fMarkerColor[graph] = color;
}

void DetectorPlots::SetMonitorList(TString *list)
{
  sMonitorList = new TString[kNMon];
  for(int imon=0;imon<kNMon;++imon){
    sMonitorList[imon] = list[imon];
    //    cout<<sMonitorList[imon].Data()<<endl;
  }
}

void DetectorPlots::SetPlotType(Int_t type)
{
  fPlotType = type;
}

void DetectorPlots::SetRunPeriod(int run){
  fRunPeriod = run;
}

void DetectorPlots::SetTitleX(char *title)
{
  sTitleX = title;
}

void DetectorPlots::SetTitleX(const char *title)
{
  sTitleX = title;
}

