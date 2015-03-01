#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TCut.h"
#include "TROOT.h"
#include "TH1.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TSystem.h"

#include <vector>
#include "math.h"
#include <iostream>
#include <fstream>
#include <utility>

void getErrorWeightedMean(Double_t *x, Double_t *xe, Double_t *mean, Int_t n){
  Double_t sum = 0, esqinv = 0; 
  if(n==1){
    mean[0] = x[0];
    mean[1] = xe[0];
    return;
  }
  for(int i=0;i<n;i++){
    sum += x[i] / (xe[i] * xe[i]);
    esqinv += 1 / (xe[i] * xe[i]);
  }
  mean[0] = sum / esqinv;
  mean[1] = sqrt(1 / esqinv);
}

Int_t MakeRun1WienPlots()
{
  gStyle->SetOptFit(1111);

    TFile *file = TFile::Open("/net/data2/paschkelab2/DB_rootfiles/run1/HYDROGEN-CELL_offoff_tree.root");
    TTree *tr = (TTree*)file->Get("tree");
    tr->AddFriend("corrected_tree","/net/data2/paschkelab2/DB_rootfiles/run1/hydrogen_cell_corrected_tree.root");
    tr->AddFriend("on_tree=tree","/net/data2/paschkelab2/DB_rootfiles/run1/HYDROGEN-CELL_on_tree.root");

    FILE *file1 = fopen("/net/data2/paschkelab2/DB_rootfiles/run1/slug_average.dat","w+");
    if(file==NULL)
      cout<<"Warning. File not opened.!\n";

    vector<Double_t> slug, dbAsym, bmAsym, dbAsymErr, 
      bmAsymErr, asym, asymErr, ihwp, sign, slugErr;

    Int_t min_entries = 5, id=0;
    printf("slug  entries  hwp  sign        raw asym           reg asym             bmod asym\n");  
    for(int islug = 42;islug<137;++islug){
      TH1D *h;
      TCut cut = Form("good&&on_tree.asym_qwk_mdallbars.value>-9e5&&slopes_exist&&slug==%i",islug);
      tr->Draw(">>list",cut);
      TEventList *list = (TEventList*)gDirectory->Get("list");
      int entries = list->GetN();
      if(list->GetN()<min_entries){
	cout<<islug<<": too few entries "<<entries<<endl;
	continue;
      }else{
	slug.push_back((double)islug);
	slugErr.push_back(0);
	tr->GetEntry(list->GetEntry(0));
	sign.push_back(tr->GetLeaf("sign_correction")->GetValue());
	ihwp.push_back(tr->GetLeaf("ihwp_setting")->GetValue());
	tr->Draw(Form("asym_qwk_mdallbars.value>>h%i",id),cut,"goff");
	h = (TH1D*)gDirectory->Get(Form("h%i",id));
	id++;
	asym.push_back(h->GetMean());
	asymErr.push_back(h->GetMeanError());
	tr->Draw(Form("on_tree.asym_qwk_mdallbars.value>>h%i",id),cut,"goff");
	h = (TH1D*)gDirectory->Get(Form("h%i",id));
	id++;
	dbAsym.push_back(h->GetMean());
	dbAsymErr.push_back(h->GetMeanError());
	tr->Draw(Form("corrected_asym_qwk_mdallbars.value>>h%i",id),cut,"goff");
	h = (TH1D*)gDirectory->Get(Form("h%i",id));
	id++;
	bmAsym.push_back(h->GetMean());
	bmAsymErr.push_back(h->GetMeanError());
      }
      printf("%3i    %3i     %+i   %+i   %+10.7f %10.7f   %+10.7f %10.7f   "
	     "%+10.7f %10.7f\n",
	     (int)slug.back(), entries, (int)ihwp.back(), (int)sign.back(), asym.back(), 
	     asymErr.back(), dbAsym.back(), dbAsymErr.back(), 
	     bmAsym.back(), bmAsymErr.back());
    }

    //plot null
    int size = (int)slug.size(), s = 0;;
    cout<<"\n\nSize:  "<<size<<endl;
    vector<Double_t> slugAvg, dbAsymnull, bmAsymnull, dbAsymErrnull, 
      bmAsymErrnull, asymnull, asymErrnull, slugAvgErr;
    for(int n=0;n<size;++n){
      vector< vector<double> >temp, temperr, tempbm, tempbmerr, tempdb, tempdberr;
      temp.resize(2);
      temperr.resize(2);
      tempbm.resize(2);
      tempbmerr.resize(2);
      tempdb.resize(2);
      tempdberr.resize(2);
      for(int j=0;j<4;++j){
	int hwp = int(ihwp.at(n));
	if(hwp!=0&&hwp!=1){
	  cout<<"Error. IHWP="<<hwp<<endl;
	  return -1;
	}
	temp[hwp].push_back(asym[n]);
	temperr[hwp].push_back(asymErr[n]);
	tempbm[hwp].push_back(bmAsym[n]);
	tempbmerr[hwp].push_back(bmAsymErr[n]);
	tempdb[hwp].push_back(dbAsym[n]);
	tempdberr[hwp].push_back(dbAsymErr[n]);
	n++;
	if(n>=size)break;
      }
      n--;
      slugAvg.push_back(0.5*(slug.at(n-3)+(slug.at(n))));
      slugAvgErr.push_back(0);
      double meanIn[2], meanOut[2];
      if(temp[0].size()==0 ||temp[1].size()==0 ){
	cout<<(int)slug.at(n-3)<<"-"<<(int)slug.at(n)<<", In: "<<temp[1].size()<<
	  " , Out:"<<temp[1].size()<<endl;
	return -1;
      }
      getErrorWeightedMean(temp[0].data(), temperr[0].data(), meanOut, temp[0].size());
      //      cout<<"EW temp[0]:"<<meanOut[0]<<"+/-"<< meanOut[1]<<endl;
      getErrorWeightedMean(temp[1].data(), temperr[1].data(), meanIn, temp[1].size()); 
      //      cout<<"EW temp[1]:"<<meanIn[0]<<"+/-"<< meanIn[1]<<endl;
      asymnull.push_back(0.5*(meanIn[0]+meanOut[0]));
      asymErrnull.push_back(0.5*pow(meanOut[1]*meanOut[1] + meanIn[1]*meanIn[1],0.5));
      printf("Raw %3i-%3i: %3i  %3i   %+0.8f %0.8f\n",(int)slug.at(n-3), 
	     (int)slug.at(n),(int)temp[0].size(),(int)temp[1].size(), asymnull.back(),
	     asymErrnull.back());

      getErrorWeightedMean(tempbm[0].data(), tempbmerr[0].data(), meanOut, 
			   tempbm[0].size());
      //      cout<<"EW temp[0]:"<<meanOut[0]<<"+/-"<< meanOut[1]<<endl;
      getErrorWeightedMean(tempbm[1].data(), tempbmerr[1].data(), meanIn, 
			   tempbm[1].size()); 
      //      cout<<"EW temp[1]:"<<meanIn[0]<<"+/-"<< meanIn[1]<<endl;
      bmAsymnull.push_back(0.5*(meanIn[0]+meanOut[0]));
      bmAsymErrnull.push_back(0.5*pow(meanOut[1]*meanOut[1] + meanIn[1]*meanIn[1],0.5));

      printf("BMod %3i-%3i: %3i  %3i   %+0.8f %0.8f\n",(int)slug.at(n-3), 
	     (int)slug.at(n),(int)tempbm[0].size(),(int)tempbm[1].size(), 
	     bmAsymnull.back(), bmAsymErrnull.back());
   

      getErrorWeightedMean(tempdb[0].data(), tempdberr[0].data(), meanOut, 
			   tempdb[0].size());
      //      cout<<"EW temp[0]:"<<meanOut[0]<<"+/-"<< meanOut[1]<<endl;
      getErrorWeightedMean(tempdb[1].data(), tempdberr[1].data(), meanIn, 
			   tempdb[1].size()); 
      //      cout<<"EW temp[1]:"<<meanIn[0]<<"+/-"<< meanIn[1]<<endl;
      dbAsymnull.push_back(0.5*(meanIn[0]+meanOut[0]));
      dbAsymErrnull.push_back(0.5*pow(meanOut[1]*meanOut[1] + meanIn[1]*meanIn[1],0.5));

      printf("Reg %3i-%3i: %3i  %3i   %+0.8f %0.8f\n",(int)slug.at(n-3), 
	     (int)slug.at(n),(int)tempdb[0].size(),(int)tempdb[1].size(), 
	     dbAsymnull.back(), dbAsymErrnull.back());
      ++s;
    }
    cout<<"\n\nS:  "<<s<<endl;

    //sign correct and plot results
    for(int i=0;i<size;++i){
      asym[i] *= sign[i];
      dbAsym[i] *= sign[i];
      bmAsym[i] *= sign[i];
    }
    TCanvas *c = new TCanvas("c","c",0,0,1800,1000);
    c->Divide(3,2);
    c->cd(1);
    TGraphErrors *grOff = new TGraphErrors(size,slug.data(),asym.data(),slugErr.data(),
					   asymErr.data());
    grOff->Draw("ap");
    grOff->SetTitle("Sign Corrected Raw Asym_mdallbars vs Slug");
    grOff->GetXaxis()->SetTitle("Slug");
    grOff->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grOff->SetLineWidth(2);
    grOff->SetMarkerStyle(7);
    grOff->Fit("pol0");

    c->cd(2);
    TGraphErrors *grOn = new TGraphErrors(size,slug.data(),dbAsym.data(),slugErr.data(),
					  dbAsymErr.data());
    grOn->Draw("ap");
    grOn->SetTitle("Sign Corrected Regressed Asym_mdallbars vs Slug");
    grOn->GetXaxis()->SetTitle("Slug");
    grOn->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grOn->SetLineWidth(2);
    grOn->SetLineColor(kBlue);
    grOn->SetMarkerStyle(7);
    grOn->SetMarkerColor(kBlue);
    grOn->Fit("pol0");

    c->cd(3);
    TGraphErrors *grBM = new TGraphErrors(size,slug.data(),bmAsym.data(),slugErr.data(),
					  bmAsymErr.data());
    grBM->Draw("ap");
    grBM->SetTitle("Sign and Dither Corrected Asym_mdallbars vs Slug");
    grBM->GetXaxis()->SetTitle("Slug");
    grBM->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grBM->SetLineWidth(2);
    grBM->SetMarkerStyle(7);
    grBM->SetMarkerColor(kRed);
    grBM->SetLineColor(kRed);
    grBM->Fit("pol0");
    size = (int)slugAvg.size();
    c->cd(4);
    TGraphErrors *grnull = new TGraphErrors(size,slugAvg.data(),asymnull.data(),
					  slugAvgErr.data(), asymErrnull.data());
    grnull->Draw("ap");
    grnull->SetTitle("Raw Null Asymmetry vs Slug");
    grnull->GetXaxis()->SetTitle("Slug");
    grnull->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grnull->SetLineWidth(2);
    grnull->SetMarkerStyle(7);
    grnull->SetMarkerColor(kBlack);
    grnull->SetLineColor(kBlack);
    grnull->Fit("pol0");

    c->cd(5);
    TGraphErrors *grDBnull = new TGraphErrors(size,slugAvg.data(),dbAsymnull.data(),
					  slugAvgErr.data(), dbAsymErrnull.data());
    grDBnull->Draw("ap");
    grDBnull->SetTitle("Regressed Null Asymmetry vs Slug");
    grDBnull->GetXaxis()->SetTitle("Slug");
    grDBnull->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grDBnull->SetLineWidth(2);
    grDBnull->SetMarkerStyle(7);
    grDBnull->SetMarkerColor(kBlue);
    grDBnull->SetLineColor(kBlue);
    grDBnull->Fit("pol0");

    c->cd(6);
    TGraphErrors *grBMnull = new TGraphErrors(size,slugAvg.data(),bmAsymnull.data(),
					  slugAvgErr.data(), bmAsymErrnull.data());
    grBMnull->Draw("ap");
    grBMnull->SetTitle("Dithering Null Asymmetry vs Slug");
    grBMnull->GetXaxis()->SetTitle("Slug");
    grBMnull->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grBMnull->SetLineWidth(2);
    grBMnull->SetMarkerStyle(7);
    grBMnull->SetMarkerColor(kRed);
    grBMnull->SetLineColor(kRed);
    grBMnull->Fit("pol0");

    //sign correct corrections by slug
    size = (int)slug.size();
    for(int i=0;i<size;++i){
      dbAsym[i] = asym[i]-dbAsym[i];
      bmAsym[i] = asym[i]-bmAsym[i];
    }

    TCanvas *c1 = new TCanvas("c1","c1",0,0,1500,700);
    c1->Divide(2,1);
    c1->cd(1);
    TGraphErrors *grBMCorr = new TGraphErrors(size,slug.data(),bmAsym.data(),slugErr.data(),
					   bmAsymErr.data());
    grBMCorr->Draw("ap");
    grBMCorr->SetTitle("Dithering Correction vs Slug (Sign Corrected)");
    grBMCorr->GetXaxis()->SetTitle("Slug");
    grBMCorr->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grBMCorr->SetLineWidth(2);
    grBMCorr->SetMarkerStyle(7);
    grBMCorr->Fit("pol0");

    c1->cd(2);
    TGraphErrors *grDBCorr = new TGraphErrors(size,slug.data(),dbAsym.data(),
					      slugErr.data(), dbAsymErr.data());
    grDBCorr->Draw("ap");
    grDBCorr->SetTitle("Regression Correction vs Slug (Sign Corrected)");
    grDBCorr->GetXaxis()->SetTitle("Slug");
    grDBCorr->GetYaxis()->SetTitle("Asym_mdallbars (ppm)");
    grDBCorr->SetLineWidth(2);
    grDBCorr->SetMarkerStyle(7);
    grDBCorr->Fit("pol0");


    return 0;


}

