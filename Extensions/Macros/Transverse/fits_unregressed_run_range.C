//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : May 11th, 2011
//*****************************************************************************************************//
//
//
//  This macro will connect to the qw_fall2010_20101204 data base and get the run averages from the given run range
//  unregressed data and plot 
//  them in to three plots for pmt+, pmt- and bar sum, us lumi and ds lumi asymmetries. 
//   e.g. use
//   ./fits_uregressed_run_range run1 run2
//
//   To compile this code do a gmake.
//    
//*****************************************************************************************************//


using namespace std;


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


//  For now instead of the main detector PMTs I will use 8 bpm wires.
//  Later these should be replaced by the actual ones.

// Left PMTs
TString quartz_bar_POS[8]=
  {"qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos","qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};

// Right PMTs
TString quartz_bar_NEG[8]=
  {"qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg","qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg"};

TString quartz_bar_SUM[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum","qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};



// us lumi sum
TString us_lumi[4]=
  {"uslumi1_sum","uslumi3_sum","uslumi5_sum","uslumi7_sum"};

// ds lumi sum
TString DS_lumi[5]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5"};

//  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8"};

TSQLServer *db;

TPad * pad1, *pad2;
TText *t1;
TTree *tree;
TString CanvasTitle;
TString target, polar,targ, goodfor, hwp;
Int_t runnum;

Int_t opt =1;
Int_t datopt = 1;
Int_t ihwpopt = 1;
Int_t run1= 0;
Int_t run2= 0;

TTree * nt;

Char_t textfile[100];

Double_t value1[8] ={0.0};
Double_t err1[8] ={0.0};
Double_t value3[8] ={0.0};
Double_t err3[8] ={0.0};

Double_t value2[4] ={0.0};
Double_t err2[4] ={0.0};


TString get_query(TString detector, TString measurement, TString detector_type, TString goodfor, TString tgt, TString hwp, Int_t run1, Int_t run2);
void plot_octant(Int_t size,TString device,Double_t value[],Double_t error[]);
void get_octant_data(Int_t size, TString devicelist[], TString detector_type,TString goodfor, TString tgt,  TString hwp, Int_t run1, Int_t run2, Double_t value[], Double_t error[]);


int main(Int_t argc,Char_t* argv[])
{


  if(argc<3 || argc> 3){
    std::cout<<"The correct usage of this script is:"<<std::endl;
    std::cout<<"./fit_unregressed_run_range run1 run2"<<std::endl;
    std::cout<<"run 1 = first run in range"<<std::endl;
    std::cout<<"run 2 = last run in range run"<<std::endl;
    exit(1);
  } else {
    run1 = atoi(argv[1]);
    run2 = atoi(argv[2]);
  }

  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nRun range averages of MD and LUMI asymmetries\n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cin>>opt;
  std::cout<<"Enter data type:"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Transverse "<<std::endl;
  std::cin>>datopt;
  std::cout<<"Enter IHWP state:"<<std::endl;
  std::cout<<"1. IN"<<std::endl;
  std::cout<<"2. OUT "<<std::endl;
  std::cin>>ihwpopt;

  if(ihwpopt == 1)
    hwp = "in";
  else if (ihwpopt == 2)  
    hwp = "out";
  else
    hwp = "UNKNOWN";
  
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
  

  std::cout<<"Getting slug averages of octants in MD PMT-, PMT+, barsum, us lumi and ds lumi"<<std::endl;
  TApplication theApp("App",&argc,argv);

 
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.12);
  gStyle->SetStatH(0.3);

  
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
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

//connect to the data base
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");

  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);

  // clear arrays;
  for(Int_t i =0;i<8;i++){
    value1[i] = 0.0;
    err1[i] = 0.0;
    value3[i]=0.0;
    err3[i]=0.0;
    }

  for(Int_t i =0;i<4;i++){
    value2[i] =0.0;
    err2[i]=0.0;
  }

  //plot MD asymmetries
  TString title1 = Form("%s (%s): %i - %i Unregressed run averages of Main detector asymmetries. FIT = p0*cos(phi + p1) + p2",targ.Data(),polar.Data(),run1,run2);
 
  TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,1000,500);
  Canvas1->Draw();
  Canvas1->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title1);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();
  get_octant_data(8,quartz_bar_SUM, "MD", goodfor, target, hwp,run1,run2, value1,err1);
  plot_octant(8,"MD BAR SUM",value1,err1);
  gPad->Update();


  Canvas1->Update();
  Canvas1->Print(Form("%i_%i_md_unregressed_avg_fits_plots.png",run1,run2));

  // plot US LUMI asymmetries
  TString title2 = Form("%s (%s): %i - %i Unregressed run averages of US Lumi asymmetries. FIT = p0*cos(phi + p1) + p2",targ.Data(),polar.Data(),run1,run2);

  TCanvas * Canvas2 = new TCanvas("canvas2", title2,0,0,1000,500);   
  Canvas2->Draw();
  Canvas2->cd();

  TPad*pad11 = new TPad("pad11","pad11",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(20);
  pad11->Draw();
  pad22->Draw();

  pad11->cd();
  TString text2 = Form(title2);
  TText*t11 = new TText(0.06,0.3,text2);
  t11->SetTextSize(0.5);
  t11->Draw();

  pad22->cd();
  get_octant_data(4,us_lumi,"LUMI", goodfor,target,hwp,run1,run2,value2,err2);
  plot_octant(4,"US LUMI",value2,err2);
  gPad->Update();

  Canvas2-> Update();
  Canvas2->Print(Form("%i_%i_uslumi_unregressed_avg_fits_plots.png",run1,run2));

  // plot DS LUMI asymmetries
  TString title3 = Form("%s (%s): %i - %i Unregressed run averages of DS Lumi asymmetries. FIT = p0*cos(phi + p1) + p2",targ.Data(),polar.Data(),run1,run2);

  TCanvas * Canvas3 = new TCanvas("canvas1", title1,0,0,1000,500);
  Canvas3->Draw();

  TPad*pad111 = new TPad("pad111","pad111",0.005,0.935,0.995,0.995);
  TPad*pad222 = new TPad("pad222","pad222",0.005,0.005,0.995,0.945);
  pad111->SetFillColor(20);
  pad111->Draw();
  pad222->Draw();


  pad111->cd();
  TString text3 = Form(title3);
  TText*t111 = new TText(0.06,0.3,text3);
  t111->SetTextSize(0.5);
  t111->Draw();

  pad222->cd();
  get_octant_data(5,DS_lumi,"LUMI", goodfor,target, hwp,run1,run2,value3,err3);
  plot_octant(5,"DS_LUMI",value3,err3);
  gPad->Update();

  Canvas3-> Update();
  Canvas3->Print(Form("%i_%i_dslumi_unregressed_avg_fits_plots.png",run1,run2));


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
TString get_query(TString detector, TString measurement, TString detector_type, TString goodfor, TString tgt,  TString hwp, Int_t run1, Int_t run2){



  Bool_t ldebug = true;
  TString datatable;
  TString plate_cut;

 
  if(detector_type == "MD")
    datatable = "md_data_view";
  if(detector_type == "LUMI")
    datatable = "lumi_data_view";
  if(detector_type == "BEAM")
    datatable = "beam_view";

  TString output = " sum( "+datatable+".value/(POWER("+datatable+".error,2)))/sum(1/(POWER("+datatable+".error,2))), SQRT(1/SUM(1/(POWER("+datatable+".error,2))))";

  TString run_quality_cut = "(md_data_view.run_quality_id = NULL || md_data_view.run_quality_id != 'bad')";
  
  TString good_for_cut = Form("%s.good_for_id = '1,8' OR %s.good_for_id = 8",datatable.Data(),datatable.Data());
  

  // To get rid of runs that have large charge asymmetries
  TString bad_run_cut = Form("%s.run_number != 9831 AND %s.run_number != 9837 AND %s.run_number != 9853 AND %s.run_number != 9888  AND %s.run_number != 9885 AND %s.run_number != 9880 AND %s.run_number != 9851 AND  ",datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data()); 
  
  TString run_cut = Form("%s.run_number >= %i AND %s.run_number <= %i AND ",datatable.Data(),run1,datatable.Data(),run2); 

  
  TString query =" SELECT " + output
    + " FROM "+datatable+", analysis, run, runlet, slow_controls_settings  "
    + " WHERE "+datatable+".analysis_id = analysis.analysis_id AND analysis.runlet_id = runlet.runlet_id AND slow_controls_settings.runlet_id =runlet.runlet_id AND "
    + " run.run_number = "+datatable+".run_number AND slow_controls_settings.target_position ='"+tgt+"' AND slow_controls_settings.slow_helicity_plate ='"+hwp+"' AND "
    +datatable+".detector = '"+detector+"' AND "
    +datatable+".subblock = 0 AND "+bad_run_cut+run_cut
    +datatable+".measurement_type = '"+measurement+"' AND "
    //+datatable+".slope_correction = 'off' AND "+datatable+".slope_calculation = 'off' AND "
    +datatable+".error !=0 AND "+datatable+".value != 'NULL' AND "+datatable+".value != 0 AND "+datatable+".error != 'NULL';";

 			 
  if(ldebug)  std::cout<<"\n"<<query<<std::endl;

  return query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(Int_t size, TString devicelist[], TString detector_type,TString goodfor, TString tgt, TString hwp,  Int_t run1, Int_t run2, Double_t value[], Double_t error[])
{
  Bool_t ldebug = false;

  if(ldebug) printf("\nDetector Type %s, size %2d\n", detector_type.Data(), size);
  for(Int_t i=0 ; i<size ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), hwp.Data());
    }
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",detector_type,goodfor,tgt,hwp,run1,run2);
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
	value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
      }
    }
    delete stmt;    
  }
}

//***************************************************
//***************************************************
//         Plor octant multi graph                   
//***************************************************
//***************************************************

void plot_octant(Int_t size,TString device,Double_t value[],Double_t error[])  
{

  const int k = size;
  std::cout<<" Plotting multi size "<<k<<std::endl;
  
  Double_t x[k];
  Double_t errx[k];
  
  for(Int_t i =0;i<k;i++){
    if(device.Contains("US"))
       x[i] = 2*i+1;
    else if(device.Contains("DS"))
      x[i] = i+1;
    else
      x[i] = i+1;

    errx[i] = 0.0;
  }

  TPad* pad = (TPad*)(gPad->GetMother());
  std::cout<<"size = "<<k<<std::endl;
 // Sin fit 
  TF1 *sinfit = new TF1("sinfit","[0]*sin((pi/180)*(45*(x-1) + [1]))+[2]",1,8);
  sinfit->SetParameter(0,0);
  sinfit->SetParameter(1,0);
  sinfit->SetParameter(2,0);
  
  // cos fit (if we want we can switch to this)
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(1,0);
  cosfit->SetParameter(2,0);


  TGraphErrors* grp  = new TGraphErrors(k,x,value,errx,error);
  grp ->SetMarkerSize(0.6);
  grp ->SetMarkerStyle(21);
  grp ->SetMarkerColor(kBlue);
  grp->Draw("AP");

  grp->Fit("cosfit");
  TF1* fit1 = grp->GetFunction("cosfit");
  if(!fit1) exit(1);
  fit1->SetLineColor(kBlue);
  fit1->DrawCopy("same");

  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",device.Data()));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

//   TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
//   legend->AddEntry(grp_in, "IHWP-IN", "p");
//   legend->AddEntry(grp_out, "IHWP-OUT", "p");
//   legend->AddEntry(grp_sum, "IN+OUT", "p");
//   legend->SetFillColor(0);
//   legend->Draw("");

//   stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);

//   stats1->Draw();
//   stats1->SetTextColor(kBlue); 
//   std::cout<<"herere\n";

  
  pad->Update();
  pad->cd(2);
  std::cout<<"herere\n";


}



