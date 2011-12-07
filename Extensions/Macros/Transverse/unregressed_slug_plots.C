//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : April 7th, 2011
//*****************************************************************************************************//
//
//
//  This macro will connect to the qw_fall2010_20101204 data base and averages of the 
//  the range given as inputs
//  
//  It will plot them in to a graph of parameter vs slugnumber 
//   e.g. use
//   ./unregressed_slug_plots
//
//   To compile this code do a gmake.
//    
//*****************************************************************************************************//


using namespace std;


#include <vector>
#include <iomanip>
#include <TVectorT.h>
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



// create vectors;


TVectorD valuein;
TVectorD valueout;
TVectorD errorin;
TVectorD errorout;
TVectorD slugin;
TVectorD slugout;

TSQLServer *db;

TPad * pad1, *pad2;
TText *t1;
TString CanvasTitle;
TString bpm,bpmdata,var1, target, polar,targ, goodfor, dettype,devname;
Int_t runnum;

Int_t opt =1;
Int_t datopt = 1;
Int_t detopt = 1;
Int_t lcut = 0;
Int_t ucut = 0;
Int_t size = 0;

TString type;
TString xunit, yunit, slopeunit;


void get_slug_data(TString device, TString detector_type,TString goodfor, 
		   TString target, UInt_t addvaluesin, UInt_t adderrorsin, 
		   UInt_t addvaluesout, UInt_t adderrorsout);

TString get_query(TString detector, TString detector_type,
		  TString goodfor, TString polar,Int_t slug);


int main(Int_t argc,Char_t* argv[])
{


  if(argc == 3 ){
    lcut = atoi(argv[1]);
    ucut = atoi(argv[2]);
  } else if (argc >3){
    std::cerr<<"The correct use is ./unregressed_slug_plots low up"<<std::endl;
    exit(1);
  }
  else if (argc < 2 ){
    std::cerr<<"Missing slug range. Please enter the limits"<<std::endl;
    exit(1);
  }
  //get slug range
  size = ucut-lcut;

  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \n Unregressed Slug Asymmetry Plots\n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cin>>opt;
  std::cout<<"Enter data type:"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Transverse "<<std::endl;
  std::cin>>datopt;
  std::cout<<"Enter detector type:"<<std::endl;
  std::cout<<"1. MD"<<std::endl;
  std::cout<<"2. LUMI "<<std::endl;
  std::cout<<"3. BCM "<<std::endl;
  std::cin>>detopt;
  std::cout<<"Enter detector name e.g. qwk_bcm1:"<<std::endl;
  std::cin>>devname;


  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 percent Al";
  }
  else{
    std::cout<<"Unknown target type!"<<std::endl;
    exit(1);
  }

  if(datopt == 1){
    polar = "longitudinal";
    goodfor = "1,3";
  }
  else if(datopt == 2){
    polar = "transverse";
    goodfor = "1,8";
  }
  else{
    std::cout<<"Unknown data type!"<<std::endl;
    exit(1);
  }
  
   if(detopt == 1){
    dettype = "MD";
   }
   else if(detopt == 2){
    dettype = "LUMI";
   }
   else if(detopt == 3){
    dettype = "BCM";
   }
   else{
    std::cout<<"Unknown detector type!"<<std::endl;
    exit(1);
  }
 

  std::cout<<"Getting slug averages .."<<std::endl;
  TApplication theApp("App",&argc,argv);

 
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
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  
  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");
  gStyle->SetTitleSize(0.04,"x");
  gStyle->SetTitleSize(0.04,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  TString title1 = Form("%s (%s): Unregressed individual slug averages of %s Asymmetries from slug  %d to %d",
			targ.Data(),polar.Data(),devname.Data(),lcut,ucut);
  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,1300,600);
  Canvas1->Draw();
  
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.940);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title1);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.6);
  t1->Draw();



  //connect to the data base
  //  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_linreg_20110125","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");

  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);



  //plot asymmetries
  pad2->cd();
  get_slug_data(devname, dettype,goodfor,target, (UInt_t)&valuein,(UInt_t) &errorin, (UInt_t)&valueout,(UInt_t) &errorout);
  Canvas1->Update();
  Canvas1->Print(polar+"_"+target+"_"+devname+"_unregressed_slug_summary_plots.gif");

 
  std::cout<<"Done plotting fits \n";
  db->Close();
  delete db;

  theApp.Run();
  return(1);

};

//***************************************************
//***************************************************
//         get query              
//***************************************************
//***************************************************
TString get_query(TString detector, TString detector_type,
		  TString goodfor, TString polar,Int_t slug){

  Bool_t ldebug = false;
  TString datatable;
  TString detcolumn;
  TString run_cut;
  TString slug_cut;
  TString plate_cut;

 
  if(detector_type == "MD"){
    datatable = "md_data_view";
    detcolumn = "detector";
  }
  if(detector_type == "LUMI"){
    datatable = "lumi_data_view";
    detcolumn = "detector";
  }
  if(detector_type == "BCM"){
    datatable = "beam_view";
    detcolumn = "monitor";
  }

  TString output = datatable+".slug, slow_controls_settings.slow_helicity_plate, sum( "+datatable+".value/(POWER("+datatable+".error,2)))/sum(1/(POWER("+datatable+".error,2))), SQRT(1/SUM(1/(POWER("+datatable+".error,2))))";  
  TString good_for_cut = Form("%s.good_for_id = '%s'",datatable.Data(),goodfor.Data());
  TString run_quality_cut = Form("%s.run_quality_id = 1",datatable.Data()); // good
 
  slug_cut =Form("( %s.slug=%d )",datatable.Data(),slug);
  

  TString query =" SELECT " + output
    + " FROM "+datatable+", analysis, run, runlet, slow_controls_settings "
    + " WHERE "+datatable+".analysis_id = analysis.analysis_id AND analysis.runlet_id = runlet.runlet_id AND "
    + " run.run_number = "+datatable+".run_number AND slow_controls_settings.runlet_id = runlet.runlet_id AND "
    +datatable+"."+detcolumn+"='"+detector+"' AND "
    +datatable+".subblock = 0 AND "
    +datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND "
    //+good_for_cut+" AND "
    //+run_quality_cut+ " AND "
    +slug_cut+" AND error !=0;";

			 
  if(ldebug)  std::cout<<query<<std::endl;

  return query;
}


//***************************************************
//***************************************************
//         get slug data                  
//***************************************************
//***************************************************

void get_slug_data(TString device, TString detector_type,TString goodfor, 
		   TString target, UInt_t addvaluesin, UInt_t adderrorsin, 
		   UInt_t addvaluesout, UInt_t adderrorsout)
{
  Bool_t ldebug = false;
  Int_t kin = 0;
  Int_t kout= 0;
  char * ihwp[10];


  if(ldebug) printf("\nDetector Type %s, number of slugs %2d\n", detector_type.Data(), size);

  for(Int_t i=0 ; i<size ;i++){
    if(ldebug) {
      printf("\n\nGetting data for %20s",device.Data());
    }
    TString query = get_query(device,detector_type,goodfor,polar,(lcut+i));
    if(ldebug)std::cout<<"\n"<<query<<std::endl;

    TSQLStatement* stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    // process statement
    if (stmt->Process()) {
      // store result of statement in buffer
      stmt->StoreResult();

      while (stmt->NextResultRow()) {
	if(strcmp(stmt->GetString(1),"in") == 0){
	  slugin.ResizeTo(kin+1);
	  (*(TVectorD*)addvaluesin).ResizeTo(kin+1);
	  (*(TVectorD*)adderrorsin).ResizeTo(kin+1); 
	  slugin.ResizeTo(kin+1); 
	  slugin[kin] = (Double_t)(stmt->GetDouble(0));
	  (*(TVectorD*)addvaluesin)[kin] = (Double_t)(stmt->GetDouble(2))*1e6; // convert to  ppm
	  (*(TVectorD*)adderrorsin)[kin] = (Double_t)(stmt->GetDouble(3))*1e6; // ppm
	  if(ldebug) printf(" value = %3.2f +- %3.2f [ppm]\n", (*(TVectorD*)addvaluesin)[kin], (*(TVectorD*)adderrorsin)[kin]);
	  std::cout<<"in slug ="<<slugin[kin]<<std::endl;
	  kin++;

	}
	if(strcmp(stmt->GetString(1),"out") == 0){
	  slugout.ResizeTo(kout+1);
	  (*(TVectorD*)addvaluesout).ResizeTo(kout+1);
	  (*(TVectorD*)adderrorsout).ResizeTo(kout+1); 
	  slugout.ResizeTo(kout+1); 
	  slugout[kout] = (Double_t)(stmt->GetDouble(0));
	  (*(TVectorD*)addvaluesout)[kout] = (Double_t)(stmt->GetDouble(2))*1e6; // convert to  ppm
	  (*(TVectorD*)adderrorsout)[kout] = (Double_t)(stmt->GetDouble(3))*1e6; // ppm
	  std::cout<<"out slug ="<<slugout[kout]<<std::endl;
	  if(ldebug) printf(" value = %3.2f +- %3.2f [ppm]\n", (*(TVectorD*)addvaluesout)[kout], (*(TVectorD*)adderrorsout)[kout]);	  	  
	  kout++;
	}
      }
    }
    delete stmt;    
  }


  TVectorD errxin(kin);
  TVectorD errxout(kout);
  TVectorD valuesum(1);
  TVectorD valueerror(1);
  TVectorD xsum(1);
  TVectorD errsum(1);
 
  // To Sum over the in and out half wave plate states
  Double_t intot = 0.0;
  Double_t outtot = 0.0;
  Double_t intoterr = 0.0;
  Double_t outtoterr = 0.0;

  for(Int_t i =0;i<kin;i++){
    errxin[i] = 0.0;
    intot+=errorin[i];
    intoterr+=pow((*(TVectorD*)adderrorsin)[i],2);
  }

  intoterr=intoterr/(kin*kin);
  for(Int_t i =0;i<kout;i++){
    errxout[i] = 0.0;
    outtot+=(*(TVectorD*)adderrorsin)[i];
    outtoterr+=pow((*(TVectorD*)adderrorsin)[i],2);
  }
  outtoterr=outtoterr/(kout*kout);

  valuesum[0] = (intot+outtot)/2;
  valueerror[0]= sqrt(intoterr+outtoterr)/2;
  xsum[0] = 1;
  errsum[0] = 0;

  TMultiGraph * grp = new TMultiGraph();
  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");

  TGraphErrors* grp_in  = new TGraphErrors(slugin,(*(TVectorD*)addvaluesin),errxin,(*(TVectorD*)adderrorsin));
  TGraphErrors* grp_out  = new TGraphErrors(slugout,(*(TVectorD*)addvaluesout),errxout,(*(TVectorD*)adderrorsout));
  TGraphErrors* grp_sum  = new TGraphErrors(xsum,valuesum,errsum,valueerror);
  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");  


  if(kin!=0){
    grp_in ->SetMarkerSize(0.8);
    grp_in ->SetMarkerStyle(21);
    grp_in ->SetMarkerColor(kBlue);
    grp_in->Fit("pol0");
    TF1* fit1 = grp_in->GetFunction("pol0");
    if(fit1 == NULL) exit(1);
    fit1->DrawCopy("same");
    fit1->SetLineColor(kBlue);
    grp->Add(grp_in);
    legend->AddEntry(grp_in, "IHWP-IN", "p");
  }

  if(kout!=0){
    grp_out ->SetMarkerSize(0.8);
    grp_out ->SetMarkerStyle(21);
    grp_out ->SetMarkerColor(kRed);
    grp_out->Fit("pol0");
    TF1* fit2 = grp_out->GetFunction("pol0");
    if(fit2 == NULL) exit(1);
    fit2->DrawCopy("same");
    fit2->SetLineColor(kRed);
    grp->Add(grp_out);
    legend->AddEntry(grp_out, "IHWP-OUT", "p");
   }

//   if((kin != 0)&&(kout!=0)){
//     grp_sum ->SetMarkerSize(0.8);
//     grp_sum ->SetMarkerStyle(21);
//     grp_sum ->SetMarkerColor(kGreen-2);
//     grp_sum->Fit("pol0");
//     TF1* fit3 = grp_sum->GetFunction("pol0");
//     if(fit3 == NULL) exit(1);
//     fit3->DrawCopy("same");
//     fit3->SetLineColor(kGreen-2);
//     grp->Add(grp_sum);
//     legend->AddEntry(grp_sum, "IN+OUT", "p");
//   }

  grp->Draw("AP");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Slug");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",devname.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(1.5);

  legend->SetFillColor(0);
  legend->Draw("");

//   if(kin!=0){
//     stats1->SetTextColor(kBlue); 
//     stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
//   }
  
//   if(kout!=0){
//      stats2->SetTextColor(kRed);
//      stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65); 
//   }

//   if((kin != 0)&&(kout!=0)){
//     stats3->SetTextColor(kGreen-2);
//     stats3->SetX1NDC(0.8); stats3->SetX2NDC(0.99); stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);
    
//   }


  
}



