//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : February 8th, 2012
//*****************************************************************************************************//
/*
  This macro was adapted from transverse_md_fit_slugs.C macro to plot only the lumi data.
  It will connect to the qw_run1_pass4(or the given) data base and get the slug averages from the lumi
  regressed data and plot them in to three plots of in,out,in+out/2 , in-out and sum of oppsite octants.
  You have the option to change the database at the command line. Just type:

  To compile this code do a gmake.
  
  e.g. use
  ./transverse_lumi_fit_slugs "qw_run1_pass4"
  
  ###############################################
  
  Slug averages of Lumi  Asymmetries
  
  ###############################################
  Enter lumi type:
  1. Upstream
  2. Downstream
  1
  Enter target type:
  1. Liquid Hydrogen
  2. 4% DS Al
  3. 1.6% DS Carbon
  1
  Enter data type:
  1. Longitudinal
  2. Vertical Transverse
  3. Horizontal Transverse
  3
  Enter reaction type:
  1. elastic
  2. N->Delta
  1
  Getting slug averages of lumi detectors
  ..
  ..

  Updates:
  2012-02-23 B.Waidyawansa        Added the option to change database.
******************************************************************************************************/

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


// us lumi sum
TString us_lumi[4]=
  {"uslumi1_sum","uslumi3_sum","uslumi5_sum","uslumi7_sum"};

// ds lumi
TString ds_lumi[8]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5","qwk_dslumi6"
   ,"qwk_dslumi7","qwk_dslumi8"};

// ds lumi reduced
TString ds_lumi_reduced[5]=
  {"qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4","qwk_dslumi5"};


TSQLServer *db;
TString database="qw_run2_pass1";
TString database_stem="run2_pass1";

std::ofstream Myfile;    

TText *t1;
TString target, polar,targ, goodfor, reg_set;

Int_t opt =1;
Int_t datopt = 1;
Int_t lumiopt = 1;
Int_t ropt = 1;
Int_t qtor_opt = 4;

Char_t textfile[100];

TString good_for;
TString qtor_current;
TString qtor_stem;
TString good;
TString interaction;

Double_t value111[4] ={0.0};
Double_t err111[4] ={0.0};
Double_t value222[4] ={0.0};
Double_t err222[4] ={0.0};

Double_t valuein1[8] ={0.0};
Double_t errin1[8] ={0.0};
Double_t valueout1[8] ={0.0};
Double_t errout1[8] ={0.0};

Double_t valuein[5] ={0.0};
Double_t errin[5] ={0.0};
Double_t valueout[5] ={0.0};
Double_t errout[5] ={0.0};


TString get_query(TString detector, TString measurement, TString ihwp);
void get_octant_data(TString devicelist[],TString ihwp, Double_t value[], Double_t error[]);
void plot_octant(Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[]);
void get_opposite_octant_average(Double_t valuesin[],Double_t errorsin[],
				 Double_t valuesout[],Double_t errorsout[]);

int main(Int_t argc,Char_t* argv[])
{


  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nSlug averages of Lumi  Asymmetries \n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter lumi type:"<<std::endl;
  std::cout<<"1. Upstream"<<std::endl;
  std::cout<<"2. Downstream(1-8) "<<std::endl;
  std::cout<<"3. Downstream reduced(1-5)"<<std::endl;
  std::cin>>lumiopt;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cout<<"3. 1.6% DS Carbon "<<std::endl;
  std::cin>>opt;
  std::cout<<"Enter data type:"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Vertical Transverse "<<std::endl;
  std::cout<<"3. Horizontal Transverse "<<std::endl;
  std::cin>>datopt;
  std::cout<<"Enter reaction type:"<<std::endl;
  std::cout<<"1. elastic"<<std::endl;
  std::cout<<"2. N->Delta "<<std::endl;
  std::cin>>ropt;
  if(ropt==2){
    std::cout<<"Enter QTOR current:"<<std::endl;
    std::cout<<"1.6000 A "<<std::endl;
    std::cout<<"2.6700 A "<<std::endl;
    std::cout<<"3.7300 A "<<std::endl;
    std::cin>>qtor_opt;
  }


  // database option?
  if(argc>1) database = argv[1];
  
   // select the target
  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 percent Al";
  }
  else if(opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 percent Carbon";
  }
  else{
    std::cout<<"Unknown target type!"<<std::endl;
    exit(1);
  }

  // Select the polarization
  if(datopt == 1){
    polar = "longitudinal";
    good = "3";
  }
  else if(datopt == 2){
    polar = "v_transverse";
    good = "8";
  }
  else if(datopt == 3){
    polar = "h_transverse";
    good = "9";
  }
  else{
    std::cout<<"Unknown polarization type!"<<std::endl;
    exit(1);
  }
  

  // Select the interaction
  if(ropt == 1){
    good_for = "(lumi_data_view.good_for_id = '%s' or lumi_data_view.good_for_id = '1,"+good+"')";
    interaction = "elastic";
  }
  else if(ropt == 2){
    good_for = "(lumi_data_view.good_for_id = '"+good+",18')";
    interaction = "n-to-delta"; 
  }
  else{
    std::cout<<"Unknown interaction type!"<<std::endl;
    exit(1);
  }

  // QTOR current cut
  if(qtor_opt == 1){
    qtor_current=" (slow_controls_settings.qtor_current>5900 && slow_controls_settings.qtor_current<6100) ";
    qtor_stem = "6000";
  }
  else if(qtor_opt == 2){
    qtor_current=" (slow_controls_settings.qtor_current>6600 && slow_controls_settings.qtor_current<6800) ";
    qtor_stem = "6700";
  }
  else if(qtor_opt == 3){
    qtor_current=" (slow_controls_settings.qtor_current>7200 && slow_controls_settings.qtor_current<7400) ";
    qtor_stem = "7300";
  }
  else if(qtor_opt == 4){
    qtor_current=" (slow_controls_settings.qtor_current>8800 && slow_controls_settings.qtor_current<9000) ";
    qtor_stem = "8901";
  }
  else{
    std::cout<<"Unknown QTOR current "<<std::endl;
    exit(1);
  }
 

  std::cout<<"Getting slug averages of lumi detectors"<<std::endl;
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
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");


  // regression set
  reg_set="on_5+1";

  if(db){
    printf("Database server info : %s\n", db->ServerInfo());
  }
  else
    exit(1);
  
  // clear arrays;
 
  for(Int_t i =0;i<4;i++){
    value111[i] = 0.0;
    err111[i] = 0.0;
    value222[i] =0.0;
    err222[i]=0.0;  
  }

 for(Int_t i =0;i<8;i++){
    valuein1[i] = 0.0;
    errin1[i] = 0.0;
    valueout1[i]=0.0;
    errout1[i]=0.0;
  }

  for(Int_t i =0;i<5;i++){
    valuein[i] = 0.0;
    errin[i] = 0.0;
    valueout[i]=0.0;
    errout[i]=0.0;
  }

 
  // Open a txt file to store data
  Char_t  textfile[400];
  if(lumiopt==1)
    sprintf(textfile,"%s_%s_%s_%s_USLUMI_regressed_%s_in_out_values_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 
  else if(lumiopt==2)
    sprintf(textfile,"%s_%s_%s_%s_DSLUMI18_regressed_%s_in_out_values_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 
  else if(lumiopt==3)
    sprintf(textfile,"%s_%s_%s_%s_DSLUMI15_regressed_%s_in_out_values_%s.txt"
	    ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 

  Myfile.open(textfile);

  //plot Lumi asymmetries
  TString title1;
  if(lumiopt==1) 
    title1= Form("%s (%s): Regressed averages of Upstream Lumi asymmetries. FIT = p0*cos(phi + p1) + p2"
		 ,targ.Data(),polar.Data());
  else if(lumiopt==2) 
    title1= Form("%s (%s): Regressed averages of Downstream Lumi 1-8  asymmetries. FIT = p0*cos(phi + p1) + p2"
		 ,targ.Data(),polar.Data());

  else if(lumiopt==3) 
    title1= Form("%s (%s): Regressed averages of Downstream Lumi 1-5  asymmetries. FIT = p0*cos(phi + p1) + p2"
		 ,targ.Data(),polar.Data());
  
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
  Myfile << " \n======================================"<<std::endl;
  Myfile << " Lumis "<<std::endl;
  Myfile << " ========================================"<<std::endl;

  if(lumiopt==1){
    get_octant_data(us_lumi,"out", value111, err111);
    get_octant_data(us_lumi,"in",  value222, err222);
    plot_octant(value222,err222,value111,err111);
    gPad->Update();
  } 
  if(lumiopt==2){
    get_octant_data(ds_lumi,"out", valueout1, errout1);
    get_octant_data(ds_lumi,"in",  valuein1, errin1);
    plot_octant(valuein1,errin1,valueout1,errout1);
    gPad->Update();
  }  
  if(lumiopt==3){
    get_octant_data(ds_lumi,"out", valueout, errout);
    get_octant_data(ds_lumi,"in",  valuein, errin);
    plot_octant(valuein,errin,valueout,errout);
    gPad->Update();
  }    

  TString lumi;
  if(lumiopt == 1) lumi = "USLUMI";
  if(lumiopt == 2) lumi = "DSLUMI18";
  if(lumiopt == 3) lumi = "DSLUMI15";

  Canvas1->Update();
  Canvas1->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+lumi+"_regressed_"+reg_set+"_slug_summary_plots_"+database_stem+".png");
  Canvas1->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+lumi+"_regressed_"+reg_set+"_slug_summary_plots_"+database_stem+".svg");
  Canvas1->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+lumi+"_regressed_"+reg_set+"_slug_summary_plots_"+database_stem+".C");
 
  if(lumiopt==2){
    // Calculate sum of opposite octants for dslumi 1-8
    get_opposite_octant_average(valuein1,errin1,valueout1,errout1);
  }

  Myfile.close();

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
TString get_query(TString detector, TString measurement, TString ihwp){

  
  Bool_t ldebug = true;
  TString datatable;

  datatable = "lumi_data_view";

  // std::cout<<"Getting regressed data for "<<detector<<std::endl;

  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = '%s' ",
			    datatable.Data(),datatable.Data(),reg_set.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = '"+target+"' AND "
    +regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0 and run_number>16000 and "+qtor_current+"; ";


  if(ldebug) std::cout<<query<<std::endl;


  return query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(TString devicelist[],TString ihwp, Double_t value[], Double_t error[])
{
  Bool_t ldebug = true;
  Int_t size = 0;
  if(lumiopt == 1) size = 4;
  else if(lumiopt == 2) size = 8;
  else size = 5;
 

  for(Int_t i=0 ; i<size ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    }
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",ihwp);
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

void plot_octant(Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[])  
{

  Int_t k =0;
  if(lumiopt == 1) k = 4;
  else if(lumiopt == 2) k = 8;
  else k = 5;


  Double_t valuesum[k];
  Double_t valueerror[k];
  Double_t valuediff[k];
  Double_t errordiff[k];
  
  Double_t x[k];
  Double_t errx[k];
  
  for(Int_t i =0;i<k;i++){
    if(lumiopt==1)
      x[i] = 2*i+1;
    else
      x[i] = i+1;

    errx[i] = 0.0;
  }


  TPad* pad = (TPad*)(gPad->GetMother());
 
  // cos fit in
  TF1 *cosfit_in = new TF1("cosfit_in","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
//   cosfit_in->SetParameter(0,0);
//   cosfit_in->SetParLimits(0,-99999,0);
//   cosfit_in->SetParameter(1,0);
//   //cosfit_in->SetParLimits(1, -1, 180);
//   cosfit_in->SetParameter(2,0);

  //Take the average over the in and out half wave plate values. 
  // Here we take just the average and not the weighted average because we are testing a hypothesis
  // IN+OUT=? 0. Since we are not sure if IN ~ OUT we can't consider them as a measurement of the
  // same value to do the weighted average.
  Myfile<<"######################"<<std::endl;
  Myfile<<"IN+OUT "<<std::endl;
  for(Int_t i =0;i<k;i++){
    valuesum[i]=(valuesin[i]+valuesout[i])/2.0;
    valueerror[i]= (sqrt(pow(errorsin[i],2)+pow(errorsout[i],2)))/2.0;
    Myfile<<"Valuein = "<<valuesin[i]<<" Errorin = "<< errorsin[i]<<std::endl;
    Myfile<<"Valueout = "<<valuesout[i]<<" Errorout = "<< errorsout[i]<<std::endl;
    Myfile<<"Value (IN+OUT)/2= "<<valuesum[i]<<" Error (IN+OUT)/2= "<< valueerror[i]<<std::endl;
  }
  std::cout<<"######################\n"<<std::endl;

  //Take the weighted difference in the IHWP in and out half wave plate values.
  //Here from IN+OUT ~ 0 we know that IN and OUT are a measurement of the same thing
  //so we can take the weighted error difference when we take IN-OUT.

  Myfile<<"IN-OUT "<<std::endl;
  for(Int_t i =0;i<k;i++){
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));

    Myfile<<"Valuein = "<<valuesin[i]<<" Errorin = "<< errorsin[i]<<std::endl;
    Myfile<<"Valueout = "<<valuesout[i]<<" Errorout = "<< errorsout[i]<<std::endl;
    Myfile<<"Value IN-OUT = "<<valuesum[i]<<" Error IN-OUT= "<< valueerror[i]<<std::endl;
  }
  std::cout<<"######################"<<std::endl;



  // Draw IN values
  TGraphErrors* grp_in  = new TGraphErrors(k,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("cosfit_in","B");
  TF1* fit1 = grp_in->GetFunction("cosfit_in");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  // Draw OUT values
  TGraphErrors* grp_out  = new TGraphErrors(k,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);

  TF1 *cosfit_out = new TF1("cosfit_out","[0]*cos((pi/180)*(45*(x-1) + [1])) + [2]",1,8);
  /*Initialize this fit with the results from the previous fit*/
//   cosfit_out->SetParameter(0,-1*(fit1->GetParameter(0)));
//   cosfit_out->SetParLimits(0,0,99999);
//   // cosfit_out->SetParLimits(1,-1,180);
//   cosfit_out->SetParameter(1, fit1->GetParameter(2));
//   cosfit_out->SetParameter(2, -(fit1->GetParameter(2)));

  grp_out->Fit("cosfit_out","B");
  TF1* fit2 = grp_out->GetFunction("cosfit_out");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);

  // Draw IN+OUT values
  TGraphErrors* grp_sum  = new TGraphErrors(k,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.6);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum->Fit("pol0","B");
  TF1* fit3 = grp_sum->GetFunction("pol0");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen-2);

 
  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in);
  grp->Add(grp_out);
  grp->Add(grp_sum);
  grp->Draw("AP");
  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle("Lumi Asymmetry (ppm)");
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IHWP-IN", "p");
  legend->AddEntry(grp_out, "IHWP-OUT", "p");
  legend->AddEntry(grp_sum, "(IN+OUT)/2", "p");
  legend->SetFillColor(0);
  legend->Draw("");

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");

  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 

  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
 
  stats3->SetTextColor(kGreen-2);
  stats3->SetFillColor(kWhite); 

  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  pad->Update();
  
  // Difference over the in and out half wave plate values
  TString title = targ+"("+polar+"): IN-OUT of regressed Lumi asymmetries. FIT = p0*cos(phi + p1) + p2";

  TCanvas * Canvas11 = new TCanvas("canvas11",title,0,0,1000,500);
  Canvas11->Draw();
  Canvas11->cd();

  TPad*pad11 = new TPad("pad11","pad11",0.005,0.935,0.995,0.995);
  TPad*pad22 = new TPad("pad22","pad22",0.005,0.005,0.995,0.945);
  pad11->SetFillColor(20);
  pad11->Draw();
  pad22->Draw();
  

  pad11->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad22->cd();
  pad22->SetFillColor(0);

  // Draw In-Out
  TGraphErrors* grp_diff  = new TGraphErrors(k,x,valuediff,errx,errordiff);
  grp_diff ->SetMarkerSize(0.6);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kBlack-2);
  grp_diff->Fit("cosfit_in","B");
 
  TF1* fit4 = grp_diff->GetFunction("cosfit_in");
  fit4->DrawCopy("same");
  fit4->SetLineColor(kMagenta+1);

  TMultiGraph * grp1 = new TMultiGraph();
  grp1->Add(grp_diff);
  grp1->Draw("AP");

  grp1->SetTitle("");
  grp1->GetXaxis()->SetTitle("Octant");
  grp1->GetXaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitle("Lumi Asymmetry (ppm)");
  grp1->GetYaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitleSize(0.03);
  grp1->GetYaxis()->SetTitleOffset(0.7);
  grp1->GetXaxis()->SetTitleOffset(0.8);


  TPaveStats *stats11 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");

  stats11->SetTextColor(kBlack);
  stats11->SetFillColor(kWhite); 
  stats11->SetX1NDC(0.8); stats11->SetX2NDC(0.99); stats11->SetY1NDC(0.7);stats11->SetY2NDC(0.95);  

  TString lumi;
  if(lumiopt == 1) lumi = "USLUMI";
  if(lumiopt == 2) lumi = "DSLUMI18";
  if(lumiopt == 3) lumi = "DSLUMI15";


  Canvas11-> Update();
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+lumi+"_regressed_"+reg_set+"_in_out_plots_"+database_stem+".png");
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+lumi+"_regressed_"+reg_set+"_in_out_plots_"+database_stem+".svg");
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_"+lumi+"_regressed_"+reg_set+"_in_out_plots_"+database_stem+".C");

}




//***************************************************
//***************************************************
//         get error weighted average of opposite octants                
//***************************************************
//***************************************************

void get_opposite_octant_average( Double_t valuesin[],Double_t errorsin[],
				 Double_t valuesout[],Double_t errorsout[])  
{
  Bool_t ldebug = false;

  Double_t valuesum[8];
  Double_t errorsum[8];

  Double_t valuediff[8];
  Double_t errordiff[8];
  
  Double_t valuesumopp[4];
  Double_t errorsumopp[4];  
  Double_t valuediffopp[4];
  Double_t errordiffopp[4];
  Double_t x[4];
  Double_t errx[4];
  
  for(Int_t i =0;i<4;i++){
    x[i]=i+1;
    errx[i]=0.0;
  }

  if(ldebug) printf("Summing over the opposite octants of Lumi\n");

  // Calculated weighted average for in-out and in+out
  for(Int_t i =0;i<8;i++){
    valuesum[i]=valuesin[i]+valuesout[i];
    errorsum[i]= sqrt(pow(errorsin[i],2)+pow(errorsout[i],2));
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));

  }

  // Sum and difference of the opposite octants.
  // Sum is normal average.
  // Difference is weighted.

  Myfile<<"\n##############################"<<std::endl;
  Myfile<<"Sum and difference of opposite octants "<<std::endl;
  for(Int_t i =0;i<4;i++){
    valuesumopp[i]= valuesum[i]+valuesum[i+4];
    errorsumopp[i]= sqrt(pow(errorsum[i],2)+pow(errorsum[i+4],2));
    valuediffopp[i]=valuediff[i]+valuediff[i+4];
    errordiffopp[i]=sqrt(pow(errordiff[i],2)+pow(errordiff[i+4],2));   

    Myfile<<"Value sum = "<<valuesumopp[i]<<" Error sum = "<< errorsumopp[i]<<std::endl;
    Myfile<<"Value diff = "<<valuediffopp[i]<<" Error diff = "<< errordiffopp[i]<<std::endl;
  }
  Myfile<<"##############################\n"<<std::endl;


  TString title = targ+"("+polar+"): Regressed slug based opposite octant averages of DSLumis. FIT = p0*cos(phi + p1) + p2";
  
  TCanvas * Canvas11 = new TCanvas("canvas11",title,0,0,1000,500);
  Canvas11->Draw();
  Canvas11->cd();
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();


  pad1->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();

  TGraphErrors* grp_sum  = new TGraphErrors(4,x,valuesumopp,errx,errorsumopp);
  grp_sum ->SetMarkerSize(1.2);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  //grp_sum->Fit("pol0");
  //TF1* fit1 = grp_sum->GetFunction("pol0");
  //fit1->DrawCopy("same");
  //fit1->SetLineColor(kGreen-2);
 

  TGraphErrors* grp_diff  = new TGraphErrors(4,x,valuediffopp,errx,errordiffopp);
  grp_diff ->SetMarkerSize(1.2);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta+1);
  //  grp_diff->Fit("pol0");
  // TF1* fit2 = grp_diff->GetFunction("pol0");
  //fit2->DrawCopy("same");
  //fit2->SetLineColor(kRed);

  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_sum);
  grp->Add(grp_diff);
  grp->Draw("AP");

  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant+opposite Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle("Lumi Opposite Octant AVG Asymmetry (ppm)");
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleSize(0.03);
  grp->GetYaxis()->SetTitleOffset(0.8);
  grp->GetXaxis()->SetTitleOffset(0.9);
  // Double_t xmin = grp->GetXaxis()->GetXmin();
  //Double_t xmax = grp->GetXaxis()->GetXmax();
  //TLine * line2 = new TLine(xmin,0,xmax,0);
  //line2->SetLineColor(kBlack);
  //line2->Draw("");


  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_diff, "IN-OUT", "p");
  legend->AddEntry(grp_sum, "IN+OUT", "p");
  legend->SetFillColor(0);
  legend->Draw("");

  // TPaveStats *stats1 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");
  // TPaveStats *stats2 = (TPaveStats*)grp_diff->GetListOfFunctions()->FindObject("stats");

  // stats1->SetTextColor(kGreen-2);
  // stats1->SetFillColor(kWhite); 

  // stats2->SetTextColor(kMagenta+1);
  // stats2->SetFillColor(kWhite); 

  // stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  // stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);

  Canvas11-> Update();
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_DSLUMI18_regressed_opposite_octant_plots_"+reg_set+"_"+database_stem+".png");
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_DSLUMI18_regressed_opposite_octant_plots_"+reg_set+"_"+database_stem+".svg");
  Canvas11->Print(interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_DSLUMI18_regressed_opposite_octant_plots_"+reg_set+"_"+database_stem+".C");
  
}

