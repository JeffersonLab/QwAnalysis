//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : October 12th 2012
//*****************************************************************************************************//

#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <TPaveStats.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TObject.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TText.h>
#include <vector>
#include <iomanip>
#include <TRandom.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <string.h>
#include <TVectorT.h>

TString get_query(TString detector, Int_t run_start, Int_t run_end);

void plot_weighted_yield_differences(Int_t run_start, Int_t run_end){

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.5);
  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.16);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadLeftMargin(0.14);

  // histo parameters
  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(1.5);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  
  //Set text
  gStyle->SetLabelFont(132,"x");
  gStyle->SetLabelFont(132,"y");
  gStyle->SetTitleFont(132,"x");
  gStyle->SetTitleFont(132,"y");
  gStyle->SetStatFont(132);
  gStyle->SetTitleFont(132,"1");

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  TSQLServer *db;
  TString query;
  TSQLStatement* stmt;

  std::vector <double> pmtpos;
  std::vector <double> pmtneg;
  std::vector <string> run_number;
  std::vector <double> run;
  TVectorD difference;
  TVectorD counter;


  TGraph * gr[8];
  TGraph * gp[8];
  TGraph * gn[8];

  Int_t counts=0;
  Int_t j=0;

  TCanvas * c = new TCanvas("c","",0,0,1500,500);
  c->Draw();
  c->SetFillColor(kWhite);
  c->cd();

  //////////////////////////////////////////////////////////////////////
  // For Run2 : Derived from From the weights in map file 14017-.map
  //////////////////////////////////////////////////////////////////////
  // Goes like 1neg, 1pos, 2neg, 2pos ...
  // old weights
  Double_t weights[16]={29.68,25.88,35.69,22.28,22.98,42.85,39.88,26.49,27.89,22.17,25.75,30.67,31.07,26.59,24.95,22.79};

  // new weights
  // from 16058
  Double_t elh2v_weights[16]={31.2,25.8,36.1,22.5,23.1,41.7,39.3,27.4,28.8,22.0,26.1,30.7,31.7,26.4,25.3,23.9};
  // 16096
  Double_t elh2h_weights[16]={31.9,26.0,36.5,22.8,23.4,42.5,39.8,27.8,29.3,22.2,26.4,31.1,32.1,26.6,25.6,24.1};
  // 16065
  Double_t n2dlh2v_weights[16]={330.5,321.5,445.5,279.3,287.6,516.7,483.9,336.6,356.8,275.8,325.2,375.8,397.9,334.0,314.1,298.8};
  // 16134
  Double_t n2dlh2h_weights[16]={269.7,261.4,358.9,225.2,231.2,425.9,391.4,271.9,288.5,222.2,263.0,304.2,321.6,269.8,253.1,240.3};
  // 16132
  Double_t n2dlh2h1_weights[16]={331.4,323.0,444.8,279.1,286.4,523.1,484.1,335.8,356.5,275.2,324.6,375.2,396.6,333.4,313.5,298.3};
  // 16152
  Double_t n2dlh2h2_weights[16]={477.7,472.5,644.9,404.0,415.0,728.0,704.7,487.7,517.9,401.9,474.1,546.8,576.3,486.4,456.6,435.3};

  // 16070
  Double_t ealv_weights[16]={71.3,68.7,98.6,60.9,63.1,109.9,106.1,73.7,77.9,59.3,69.6,80.6,84.1,70.0,67.4,63.5};
  // 16106
  Double_t ealh_weights[16]={72.6,69.3,100.5,62.0,64.2,119.6,107.8,75.0,79.4,60.1,70.8,82.4,85.6,71.1,68.6,64.7};
  // 16112
  Double_t ealh2_weights[16]={72.1,69.2,99.1,61.3,63.5,110.4,106.8,74.2,78.3,59.6,70.2,81.4,84.5,70.4,67.8,64.1};

  // 16067
  Double_t n2dal_weights[16]={563.1,567.0,791.2,494.0,508.7,887.1,854.4,595.2,632.2,488.5,577.3,662.4,691.1,582.1,552.1,528.1};
  //run 16160
  Double_t n2dalh_weights[16]={476.9,474.5,665.1,415.5,428.0,746.7,723.4,501.2,530.5,411.7,487.6,565.3,582.9,489.5,464.5,444.9};
 
  // 16145
  Double_t ec_weights[16]={96.9,93.0,132.9,82.2,84.8,147.7,142.7,99.5,104.9,79.9,93.9,109.6,113.9,95.0,91.7,86.6};
  // 16148
  Double_t n2dc_weights[16]={1205.9,1216.4,1681.8,1050.1,1081.4,1888.9,1818.9,1251.2,1343.8,1039.6,1219.9,1415.7,1473.6,1250.6,1183.2,1127.1};



  //////////////////////////////////
  // For Run1 : from 9810-.map file 
  //////////////////////////////////
  // Double_t weights[16]={26.5,25.9,36.2,22.0,22.2,42.0,38.1,24.8,26.1,21.4,25.0,29.3,29.9,25.6,24.3,21.5};
  // new weights from run 9847
  Double_t al_weights[16]={68.0,73.9,106.4,64.1,65.5,118.0,110.7,71.7,75.3,61.7,71.4,82.2,85.0,72.5,69.5,62.1};



  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nPlot weighted yield differences in a bar  \n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;

  TString database = "qw_run2_pass1";
  std::cout<<"Using database "<<database<<std::endl;
  //connect to the data base
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");


  // For each detector get pmt values, weight then and get the difference
  Double_t diff;
  for(size_t i=0;i<8;i++){
    std::cout<<" Getting data for bar = "<<i+1<<std::endl;
    // clear the vectors
     diff=0;

     pmtpos.clear();
     pmtneg.clear();
     difference.Clear();
     counter.Clear();
     j=0;

    //get negative yields
    query = get_query(Form("qwk_md%dneg",i+1),run_start, run_end);
    //std::cout<<Form("qwk_pmt%dneg",i+1)<<std::endl;
    stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    // process statement
    if (stmt->Process()) {
      // store result of statement in buffer
	stmt->StoreResult();

	while (stmt->NextResultRow()) {
	  if(stmt->IsNull(0)){
	    std::cout<<"Value is NULL skipping this detector.."<<i+1<<std::endl;
	    exit(1);
	  }else{
	    pmtneg.push_back((Double_t)(stmt->GetDouble(2)));
	    //	    printf(" neg value = %16.8lf \n", pmtneg[i]);
	  }
	}
    }
    //delete stmt;    
    
    counts= (Int_t)(pmtneg.size());
    std::cout<<" entries = "<<counts<<std::endl;
    counter.ResizeTo(counts);

    // get positive yields
    query = get_query(Form("qwk_md%ipos",i+1),run_start, run_end);
    //std::cout<<Form("qwk_pmt%dpos",i+1)<<std::endl;
    stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    // process statement
    if (stmt->Process()) {
      // store result of statement in buffer
	stmt->StoreResult();
	
	while (stmt->NextResultRow()) {
	  if(stmt->IsNull(0)){
	    std::cout<<"Value is NULL skipping this detector.."<<i+1<<std::endl;
	    exit(1);
	  }else{
	    pmtpos.push_back((Double_t)(stmt->GetDouble(2)));
	    // only fill for one iteration
	    counter.operator()(j) = j;
	    if(i==0){
	      run_number.push_back(stmt->GetString(0));
	      run.push_back(stmt->GetDouble(0));
	      //printf(" run_number = %s \n",stmt->GetString(0));
	    }
	    j++;
	  }
	}
    }
    delete stmt;    

     difference.ResizeTo(counts);

     for(size_t k=0;k<counts;k++){
       // std::cout<<"\nrun ="<<run[k]<<std::endl;
       // std::cout<<"pmt neg "<<pmtneg.at(k)<<" pmt pos "<<pmtpos.at(k)<<std::endl;

       ////////////////
       /// For Run1

       if((run[k]>9845) and (run[k]<9859)){
	 ///////////////////////////////////
	 // For Al in run 1 with new weights
	 diff = 100*2*((pmtpos.at(k)*al_weights[2*i+1]-pmtneg.at(k)*al_weights[2*i])/(pmtpos.at(k)*al_weights[2*i+1]+pmtneg.at(k)*al_weights[2*i]));
       }
       else{
	 diff = 100*2*((pmtpos.at(k)*weights[2*i+1]-pmtneg.at(k)*weights[2*i])/(pmtpos.at(k)*weights[2*i+1]+pmtneg.at(k)*weights[2*i]));
       }
 

       ///////////////////////////////////
       // For run 2 
       if((run[k]>16057) and (run[k]<16162)){
     
// 	 //------------try 1-------------------basic region using weighte from the first run in each region

// 	 if(((run[k]>=16057) and (run[k]<=16064)) or ((run[k]>=16073) and (run[k]<=16105)) or ((run[k]>=16125) and (run[k]<=16128)) or ((run[k]>=16138) and (run[k]<=16143))){
// 	   // elastic hydrogen
// 	   diff = 100*2*((pmtpos.at(k)*elh2v_weights[2*i+1]-pmtneg.at(k)*elh2v_weights[2*i])/(pmtpos.at(k)*elh2v_weights[2*i+1]+pmtneg.at(k)*elh2v_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16070) and (run[k]<=16072)) or (run[k]>=16106) and (run[k]<=16114)){
// 	   // elastic aluminum
// 	   diff = 100*2*((pmtpos.at(k)*ealv_weights[2*i+1]-pmtneg.at(k)*ealv_weights[2*i])/(pmtpos.at(k)*ealv_weights[2*i+1]+pmtneg.at(k)*ealv_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16144) and (run[k]<=16147))){
// 	   // elastic carbon
// 	   diff = 100*2*((pmtpos.at(k)*ec_weights[2*i+1]-pmtneg.at(k)*ec_weights[2*i])/(pmtpos.at(k)*ec_weights[2*i+1]+pmtneg.at(k)*ec_weights[2*i]));
// 	 }
// 	 else if (((run[k]>=16065) and (run[k]<=16066)) or ((run[k]>=16129) and (run[k]<=16137)) or ((run[k]>=16152) and (run[k]<=16158))){
// 	   // n->delta hydrogen
// 	   diff = 100*2*((pmtpos.at(k)*n2dlh2v_weights[2*i+1]-pmtneg.at(k)*n2dlh2v_weights[2*i])/(pmtpos.at(k)*n2dlh2v_weights[2*i+1]+pmtneg.at(k)*n2dlh2v_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16067) and (run[k]<=16069)) or ((run[k]>=16115) and (run[k]<=16124)) or ((run[k]>=16160) and (run[k]<=16161))){
// 	   // n->delta aluminum
// 	   diff = 100*2*((pmtpos.at(k)*n2dal_weights[2*i+1]-pmtneg.at(k)*n2dal_weights[2*i])/(pmtpos.at(k)*n2dal_weights[2*i+1]+pmtneg.at(k)*n2dal_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16148) and (run[k]<=16151))){
// 	 // n->delta carbon
// 	   diff = 100*2*((pmtpos.at(k)*n2dc_weights[2*i+1]-pmtneg.at(k)*n2dc_weights[2*i])/(pmtpos.at(k)*n2dc_weights[2*i+1]+pmtneg.at(k)*n2dc_weights[2*i]));
// 	 }
       

// 	 //------------try 2------------------- break lh2 elastic to 2 regions
// 	 if(((run[k]>=16057) and (run[k]<=16064)) or ((run[k]>=16073) and (run[k]<=16095)) or ((run[k]>=16125) and (run[k]<=16128)) or ((run[k]>=16138) and (run[k]<=16143))){
// 	   // elastic hydrogen 1
// 	   diff = 100*2*((pmtpos.at(k)*elh2v_weights[2*i+1]-pmtneg.at(k)*elh2v_weights[2*i])/(pmtpos.at(k)*elh2v_weights[2*i+1]+pmtneg.at(k)*elh2v_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16096) and (run[k]<=16105))){
// 	   // elastic hydrogen 2
// 	   diff = 100*2*((pmtpos.at(k)*elh2h_weights[2*i+1]-pmtneg.at(k)*elh2h_weights[2*i])/(pmtpos.at(k)*elh2h_weights[2*i+1]+pmtneg.at(k)*elh2h_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16070) and (run[k]<=16072)) or (run[k]>=16106) and (run[k]<=16114)){
// 	   // elastic aluminum1
// 	   diff = 100*2*((pmtpos.at(k)*ealv_weights[2*i+1]-pmtneg.at(k)*ealv_weights[2*i])/(pmtpos.at(k)*ealv_weights[2*i+1]+pmtneg.at(k)*ealv_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16144) and (run[k]<=16147))){
// 	   // elastic carbon
// 	   diff = 100*2*((pmtpos.at(k)*ec_weights[2*i+1]-pmtneg.at(k)*ec_weights[2*i])/(pmtpos.at(k)*ec_weights[2*i+1]+pmtneg.at(k)*ec_weights[2*i]));
// 	 }
// 	 else if (((run[k]>=16065) and (run[k]<=16066)) or ((run[k]>=16129) and (run[k]<=16137)) or ((run[k]>=16152) and (run[k]<=16158))){
// 	   // n->delta hydrogen
// 	   diff = 100*2*((pmtpos.at(k)*n2dlh2v_weights[2*i+1]-pmtneg.at(k)*n2dlh2v_weights[2*i])/(pmtpos.at(k)*n2dlh2v_weights[2*i+1]+pmtneg.at(k)*n2dlh2v_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16067) and (run[k]<=16069)) or ((run[k]>=16115) and (run[k]<=16124)) or ((run[k]>=16160) and (run[k]<=16161))){
// 	   // n->delta aluminum
// 	   diff = 100*2*((pmtpos.at(k)*n2dal_weights[2*i+1]-pmtneg.at(k)*n2dal_weights[2*i])/(pmtpos.at(k)*n2dal_weights[2*i+1]+pmtneg.at(k)*n2dal_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16148) and (run[k]<=16151))){
// 	 // n->delta carbon
// 	   diff = 100*2*((pmtpos.at(k)*n2dc_weights[2*i+1]-pmtneg.at(k)*n2dc_weights[2*i])/(pmtpos.at(k)*n2dc_weights[2*i+1]+pmtneg.at(k)*n2dc_weights[2*i]));
// 	 }


// 	 //------------try 3------------------- break elastic aluminum to 2 regions
// 	 if(((run[k]>=16057) and (run[k]<=16064)) or ((run[k]>=16073) and (run[k]<=16095)) or ((run[k]>=16125) and (run[k]<=16128)) or ((run[k]>=16138) and (run[k]<=16143))){
// 	   // elastic hydrogen 1
// 	   diff = 100*2*((pmtpos.at(k)*elh2v_weights[2*i+1]-pmtneg.at(k)*elh2v_weights[2*i])/(pmtpos.at(k)*elh2v_weights[2*i+1]+pmtneg.at(k)*elh2v_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16096) and (run[k]<=16105))){
// 	   // elastic hydrogen 2
// 	   diff = 100*2*((pmtpos.at(k)*elh2h_weights[2*i+1]-pmtneg.at(k)*elh2h_weights[2*i])/(pmtpos.at(k)*elh2h_weights[2*i+1]+pmtneg.at(k)*elh2h_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16070) and (run[k]<=16072)) or (run[k]>=16112) and (run[k]<=16114)){
// 	   // elastic aluminum1
// 	   diff = 100*2*((pmtpos.at(k)*ealv_weights[2*i+1]-pmtneg.at(k)*ealv_weights[2*i])/(pmtpos.at(k)*ealv_weights[2*i+1]+pmtneg.at(k)*ealv_weights[2*i]));
// 	 }
// 	 else if((run[k]>=16106) and (run[k]<=16111)){
// 	   // elastic aluminum2
// 	   diff = 100*2*((pmtpos.at(k)*ealh_weights[2*i+1]-pmtneg.at(k)*ealh_weights[2*i])/(pmtpos.at(k)*ealh_weights[2*i+1]+pmtneg.at(k)*ealh_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16144) and (run[k]<=16147))){
// 	   // elastic carbon
// 	   diff = 100*2*((pmtpos.at(k)*ec_weights[2*i+1]-pmtneg.at(k)*ec_weights[2*i])/(pmtpos.at(k)*ec_weights[2*i+1]+pmtneg.at(k)*ec_weights[2*i]));
// 	 }
// 	 else if (((run[k]>=16065) and (run[k]<=16066)) or ((run[k]>=16129) and (run[k]<=16137)) or ((run[k]>=16152) and (run[k]<=16158))){
// 	   // n->delta hydrogen
// 	   diff = 100*2*((pmtpos.at(k)*n2dlh2v_weights[2*i+1]-pmtneg.at(k)*n2dlh2v_weights[2*i])/(pmtpos.at(k)*n2dlh2v_weights[2*i+1]+pmtneg.at(k)*n2dlh2v_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16067) and (run[k]<=16069)) or ((run[k]>=16115) and (run[k]<=16124)) or ((run[k]>=16160) and (run[k]<=16161))){
// 	   // n->delta aluminum
// 	   diff = 100*2*((pmtpos.at(k)*n2dal_weights[2*i+1]-pmtneg.at(k)*n2dal_weights[2*i])/(pmtpos.at(k)*n2dal_weights[2*i+1]+pmtneg.at(k)*n2dal_weights[2*i]));
// 	 }
// 	 else if(((run[k]>=16148) and (run[k]<=16151))){
// 	 // n->delta carbon
// 	   diff = 100*2*((pmtpos.at(k)*n2dc_weights[2*i+1]-pmtneg.at(k)*n2dc_weights[2*i])/(pmtpos.at(k)*n2dc_weights[2*i+1]+pmtneg.at(k)*n2dc_weights[2*i]));
// 	 }
       
	 //------------try 5------------------- break inelastiv hydrogen in to 3 regions from 16134
	 if(((run[k]>=16057) and (run[k]<=16064)) or ((run[k]>=16073) and (run[k]<=16095)) ){
	   // elastic hydrogen 1
	   diff = 100*2*((pmtpos.at(k)*elh2v_weights[2*i+1]-pmtneg.at(k)*elh2v_weights[2*i])/(pmtpos.at(k)*elh2v_weights[2*i+1]+pmtneg.at(k)*elh2v_weights[2*i]));
	 }
	 else if(((run[k]>=16096) and (run[k]<=16105))or ((run[k]>=16125) and (run[k]<=16128)) or ((run[k]>=16138) and (run[k]<=16143)) or run[k]==16159){
	   // elastic hydrogen 2
	   diff = 100*2*((pmtpos.at(k)*elh2h_weights[2*i+1]-pmtneg.at(k)*elh2h_weights[2*i])/(pmtpos.at(k)*elh2h_weights[2*i+1]+pmtneg.at(k)*elh2h_weights[2*i]));
	 }
	 else if(((run[k]>=16070) and (run[k]<=16072)) or (run[k]>=16112) and (run[k]<=16114)){
	   // elastic aluminum1
	   diff = 100*2*((pmtpos.at(k)*ealv_weights[2*i+1]-pmtneg.at(k)*ealv_weights[2*i])/(pmtpos.at(k)*ealv_weights[2*i+1]+pmtneg.at(k)*ealv_weights[2*i]));
	 }
	 else if((run[k]>=16106) and (run[k]<=16111)){
	   // elastic aluminum2
	   diff = 100*2*((pmtpos.at(k)*ealh_weights[2*i+1]-pmtneg.at(k)*ealh_weights[2*i])/(pmtpos.at(k)*ealh_weights[2*i+1]+pmtneg.at(k)*ealh_weights[2*i]));
	 }
	 else if(((run[k]>=16144) and (run[k]<=16147))){
	   // elastic carbon
	   diff = 100*2*((pmtpos.at(k)*ec_weights[2*i+1]-pmtneg.at(k)*ec_weights[2*i])/(pmtpos.at(k)*ec_weights[2*i+1]+pmtneg.at(k)*ec_weights[2*i]));
	 }
	 else if (((run[k]>=16065) and (run[k]<=16066))){
	   // n->delta hydrogen1
	   diff = 100*2*((pmtpos.at(k)*n2dlh2v_weights[2*i+1]-pmtneg.at(k)*n2dlh2v_weights[2*i])/(pmtpos.at(k)*n2dlh2v_weights[2*i+1]+pmtneg.at(k)*n2dlh2v_weights[2*i]));
	 }
	 else if (((run[k]>=16129) and (run[k]<=16133))){
	   // n->delta hydrogen2
	   diff = 100*2*((pmtpos.at(k)*n2dlh2h1_weights[2*i+1]-pmtneg.at(k)*n2dlh2h1_weights[2*i])/(pmtpos.at(k)*n2dlh2h1_weights[2*i+1]+pmtneg.at(k)*n2dlh2h1_weights[2*i]));
	 }
	 else if (((run[k]>=16134) and (run[k]<=16137))){
	   // n->delta hydrogen3
	   diff = 100*2*((pmtpos.at(k)*n2dlh2h_weights[2*i+1]-pmtneg.at(k)*n2dlh2h_weights[2*i])/(pmtpos.at(k)*n2dlh2h_weights[2*i+1]+pmtneg.at(k)*n2dlh2h_weights[2*i]));
	 }
	 else if (((run[k]>=16152) and (run[k]<=16158))){
	   // n->delta hydrogen4
	   diff = 100*2*((pmtpos.at(k)*n2dlh2h2_weights[2*i+1]-pmtneg.at(k)*n2dlh2h2_weights[2*i])/(pmtpos.at(k)*n2dlh2h2_weights[2*i+1]+pmtneg.at(k)*n2dlh2h2_weights[2*i]));
	 }
	 else if(((run[k]>=16067) and (run[k]<=16069)) or ((run[k]>=16115) and (run[k]<=16124))){
	   // n->delta aluminum1
	   diff = 100*2*((pmtpos.at(k)*n2dal_weights[2*i+1]-pmtneg.at(k)*n2dal_weights[2*i])/(pmtpos.at(k)*n2dal_weights[2*i+1]+pmtneg.at(k)*n2dal_weights[2*i]));
	 }
	 else if((run[k]>=16160) and(run[k]<=16161)) {
	   // n->delta aluminum2
	   diff = 100*2*((pmtpos.at(k)*n2dalh_weights[2*i+1]-pmtneg.at(k)*n2dalh_weights[2*i])/(pmtpos.at(k)*n2dalh_weights[2*i+1]+pmtneg.at(k)*n2dalh_weights[2*i]));
	 }
	 else if(((run[k]>=16148) and (run[k]<=16151))){
	 // n->delta carbon
	   diff = 100*2*((pmtpos.at(k)*n2dc_weights[2*i+1]-pmtneg.at(k)*n2dc_weights[2*i])/(pmtpos.at(k)*n2dc_weights[2*i+1]+pmtneg.at(k)*n2dc_weights[2*i]));
	 }
	 else diff = 0;
       }
       
       difference.operator()(k)=diff;
       
     }
     
     gr[i] = new TGraph(counter, difference);
     gr[i]->SetMarkerStyle(20);
     gr[i]->SetMarkerSize(1);
     gr[i]->SetMarkerColor(i+1);
     
  }
  
  
  // Create a multigraph
  
  TMultiGraph * mg = new TMultiGraph();
  for(Int_t i=0;i<8;i++)
    mg->Add(gr[i]);

  mg->Draw("AP"); 
  mg->GetXaxis()->SetTitle("Run Number");
  mg->SetTitle(" Difference in the weighted PMT yields vs Run number ");
  mg->GetYaxis()->SetTitle("100#times2#times#frac{(Y_{pos}W_{pos}-Y_{neg}W_{neg})}{(Y_{pos}W_{pos}+Y_{neg}W_{neg})} %");
  
  TLegend *legend = new TLegend(0.90,0.50,0.99,0.99,"","brNDC");
  for(Int_t i=0;i<8;i++)
    legend->AddEntry(gr[i], Form("md%d", i+1), "p");

  legend->Draw();
  char* label;

  for (size_t k=1;k<j+1;k++) {
    label = (char*)(run_number[k-1].c_str());
    (mg->GetXaxis())->SetBinLabel((mg->GetXaxis())->FindBin(counter(k-1)),label);
  }
  gPad->Update();
  c->Modified();
  c->Print(Form("%d_%d_weighted_yield_differences.png",run_start,run_end));

  std::cout<<"here \n";


  std::cout<<"Done ! \n";
  db->Close();
  delete db;


}


TString get_query(TString detector, Int_t run_start, Int_t run_end){
 
  
  TString datatable;

  TString output = " SELECT (run_number+(segment_number/100)), n, value, error ";
   
  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString run_cut =  Form(" run_number >= %d AND run_number<= %d ",run_start, run_end);

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT run_number, n, sum(value/(POWER(error,2)))/sum(1/(POWER(error,2))) FROM md_data_view  "
    " WHERE md_data_view.detector = '"+detector+"' AND md_data_view.subblock = 0 AND "
    " md_data_view.measurement_type = 'y' AND "
    " slope_calculation = 'off' AND slope_correction ='off' AND "+run_cut+
    " AND md_data_view.error != 0 GROUP BY run_number; ";
  
  // std::cout<<query<<std::endl;
 

  return query;
}

