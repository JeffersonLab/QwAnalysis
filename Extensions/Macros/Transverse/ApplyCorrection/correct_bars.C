//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : July 20, 2012
//*****************************************************************************************************//
/*
  This macro applys corrections to individual bars
  ./correct_bars db 

*/

using namespace std;

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

// detector list
TString quartz_barsum[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum"
   ,"qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

// database info
TSQLServer *db;
TString database="qw_run2_pass1";
TString database_stem="run2_pass1";

// text file to store asymmetries and corrections
std::ofstream Myfile;    

// general variables in the order of usage
Int_t opt =1;
Int_t datopt = 1;
Int_t ropt = 1;
Int_t qtor_opt = 4;
TString target,targ, polar, good, good_for,qtor_current,qtor_stem,interaction,reg_set;
Int_t slug = 0;


//sub functions
TString get_query(TString detector, TString measurement, TString ihwp, Int_t slug_number);
void get_octant_data(TString devicelist[],TString ihwp,Int_t slug_number, Double_t value[], Double_t error[]);
void fit_and_correct();



// Main function
int main(Int_t argc,Char_t* argv[])
{

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
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
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
  //  gStyle->SetLegendFont(132);

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nCorrecting Bars for transverse effects   \n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;

  // second argument gives the database. Default is qw_run2_pass1
  if(argc>1) database = argv[1];
  std::cout<<"Using database "<<database<<std::endl;
  //connect to the data base
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");

  // For now, lets correct on consecative slugs
  // select the slugs 
  std::cout<<"Enter the starting slug with IHWP IN"<<std::endl;
  std::cin>>slug;
  std::cout<<"Using IN slug = "<<slug<<" and OUT slug ="<<slug+1<<std::endl;


  // select the target
  std::cout<<"Enter target type (default is LH2):"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cout<<"3. 1.6% DS Carbon "<<std::endl;
  std::cin>>opt;
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

  // select the polarization
  std::cout<<"Enter polarization type: (Default is Longitudinal)"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Vertical Transverse "<<std::endl;
  std::cout<<"3. Horizontal Transverse "<<std::endl;
  std::cin>>datopt;
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

  // select reaction
  std::cout<<"Enter reaction type: (default is elastic)"<<std::endl;
  std::cout<<"1. elastic"<<std::endl;
  std::cout<<"2. N->Delta "<<std::endl;
  std::cout<<"3. DIS "<<std::endl;
  std::cin>>ropt;
  if(ropt == 1){
    interaction = "elastic";
    good_for = "md_data_view.good_for_id = '1,"+good+"'";
    qtor_current=" (slow_controls_settings.qtor_current>8800 && slow_controls_settings.qtor_current<9000) ";
    qtor_stem = "8901";
  }
  else if(ropt == 2){
    good_for = "(md_data_view.good_for_id = '"+good+",18')";
    interaction = "n-to-delta"; 
  }
  else if(ropt == 3){
    good_for = "(md_data_view.good_for_id = '1,3,21')";
    qtor_current=" (slow_controls_settings.qtor_current>8990 && slow_controls_settings.qtor_current<9010) ";
    qtor_stem = "9000";
    interaction = "dis"; 
  }
  else{
    std::cout<<"Unknown interaction type!"<<std::endl;
    exit(1);
  }


  // for N->delta. select QTOR current
  if(ropt==2){
    std::cout<<"Enter QTOR current: (default is 1)"<<std::endl;
    std::cout<<"1.6000 A "<<std::endl;
    std::cout<<"2.6700 A "<<std::endl;
    std::cout<<"3.7300 A "<<std::endl;
    std::cin>>qtor_opt;
  }
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
 
  // use regression set
  reg_set="on_5+1";


  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Getting data from main detectors for "<<polar<<" "<<interaction
	   <<" ep scattering from "<<targ<<" with QTOR="<<qtor_stem<<std::endl;
  std::cout<<"###############################################"<<std::endl;

  // Open the txt file to store data
  Char_t  textfile[400];
  sprintf(textfile,"%s_%s_%s_%s_MD_corrections_%s_from_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 
  Myfile.open(textfile);
  Myfile<<" Slugs : in = "<<slug<<" and out = "<<slug+1<<std::endl;

  //Get the asymmetries from the detectors and calcualte IN-OUT 
  fit_and_correct();
  



  Myfile.close();

  std::cout<<"Done plotting fits \n";
  db->Close();
  delete db;

  theApp.Run();
  return(1);
}


//***************************************************
//***************************************************
//         make database query              
//***************************************************
//***************************************************
TString get_query(TString detector, TString measurement, TString ihwp, Int_t slug_number){

  
  Bool_t ldebug = false;
  TString datatable;

  datatable = "md_data_view";

  // std::cout<<"Getting regressed data for "<<detector<<std::endl;

  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = '%s' ",
			    datatable.Data(),datatable.Data(),reg_set.Data()); 

  TString slug_cut =  Form("%s.slug = %i",datatable.Data(),slug_number);

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
    TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = '"+target+"' AND "
    +regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "+slug_cut+" AND "
    +datatable+".error != 0 and "+qtor_current+"; ";

  if(ldebug) std::cout<<query<<std::endl;


  return query;
}


//***************************************************
//***************************************************
//         get data for each octant/detector                 
//***************************************************
//***************************************************

void get_octant_data(TString devicelist[],TString ihwp, Int_t slug_number, Double_t value[], Double_t error[])
{
  Bool_t ldebug = true;

  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    }
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",ihwp, slug_number);
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
	if(value[i]==NULL){
	  std::cout<<"Value is NULL and error is NULL; Probably wrong IHWP status. exiting.."<<std::endl;
	  exit(1);
	}
	if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
	Myfile<<setw(10)<<devicelist[i]<<" ihwp "<<setw(4)<<ihwp
	      <<setw(5)<<" value ="<<setw(10)<<value[i]
	      <<" +- "<<setw(10)<<error[i]<<std::endl;
      }
    }
    delete stmt;    
  }
}


//***************************************************
//***************************************************
//    Calcualte IN-OUT, fit and return fit results                
//***************************************************
//***************************************************

void fit_and_correct(){


  Double_t x[8];
  Double_t errx[8];
  Double_t valuesin[8]={0};
  Double_t errorsin[8]={0};
  Double_t valuesout[8]={0};
  Double_t errorsout[8]={0};
  Double_t cvalue[8]={0};
  Double_t in_out[8]={0};
  Double_t in_out_err[8]={0};
  Double_t fit[5]={0};

  for(Int_t i =0;i<8;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }

  // fit  function
  TF1 *fit_t = new TF1("fit_t","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]",1,8);
  fit_t->SetParName(0,"A_{V}");
  fit_t->SetParName(1,"A_{H}");
  fit_t->SetParName(2,"C");

  // get asymmetris from the two slugs
  get_octant_data(quartz_barsum,"in", slug,  valuesin, errorsin);
  get_octant_data(quartz_barsum,"out", slug+1,valuesout, errorsout);
 

  //Take the weighted difference in the IHWP in and out half wave plate values.
  //Here from IN+OUT ~ 0 we know that IN and OUT are a measurement of the same thing
  //so we can take the weighted error difference when we take IN-OUT.

  Myfile<<"! IN-OUT "<<std::endl;
  for(Int_t i =0;i<8;i++){
    in_out[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    in_out_err[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));
    Myfile<<"Value IN-OUT = "<<setw(10)<<in_out[i]<<" +-  "<<setw(10)<<in_out_err[i]<<std::endl;
  }

  // Create canvas
  TCanvas * c = new TCanvas("c","",0,0,1000,500);
  c->Draw();
  c->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TString text = Form("Corrected and un-corrected AVG(IN-OUT) of slug %i and slug %i", slug, slug+1);
  TText*t = new TText(0.06,0.3,text);
  t->SetTextSize(0.7);
  t->Draw();

  pad2->cd();
  pad2->SetFillColor(0);

  // draw uncorrected IN-OUT
  TGraphErrors* grp_in_out  = new TGraphErrors(8,x,in_out,errx,in_out_err);
  grp_in_out ->SetMarkerSize(0.8);
  grp_in_out ->SetMarkerStyle(21);
  grp_in_out ->SetMarkerColor(kRed);
  grp_in_out ->Fit("fit_t","B");
  TF1* fit_temp = grp_in_out->GetFunction("fit_t");
  fit_temp->SetLineColor(kRed);
  fit_temp->SetLineStyle(2);
  fit_temp->DrawCopy("same");

  // get fit results
  fit[0]=fit_temp->GetParameter(0);
  fit[1]=fit_temp->GetParError(0);
  fit[2]=fit_temp->GetParameter(1);
  fit[3]=fit_temp->GetParError(1);
  fit[4]=fit_temp->GetParameter(2);
  fit[5]=fit_temp->GetParError(2);
  Myfile<<"Fit results from uncorrected data: [0]cos(phi)-[1]sin(phi)+[2]"<<std::endl;
  Myfile<<"[0] = "<<setw(10)<<fit[0]<<" +- "<<setw(10)<<fit[1]<<std::endl;
  Myfile<<"[1] = "<<setw(10)<<fit[2]<<" +- "<<setw(10)<<fit[3]<<std::endl;
  Myfile<<"[2] = "<<setw(10)<<fit[3]<<" +- "<<setw(10)<<fit[5]<<std::endl;

  // Apply corrections
  Myfile<<"! Corrected values "<<std::endl;
  for(Int_t i =0;i<8;i++){
    cvalue[i]=in_out[i]-( fit[0]*TMath::Cos((TMath::Pi()/180)*(45*i)) - fit[2]*TMath::Sin((TMath::Pi()/180)*(45*i)));
    Myfile<<"Corrected IN-OUT value = "<<setw(10)<<cvalue[i]<<" +- "<<setw(10)<<in_out_err[i]<<std::endl;
  }
  
  // draw corrected data
  TGraphErrors* grp_corrected  = new TGraphErrors(8,x,cvalue,errx,in_out_err);
  grp_corrected ->SetMarkerSize(0.8);
  grp_corrected ->SetMarkerStyle(21);
  grp_corrected ->SetMarkerColor(kBlue);
  grp_corrected ->SetMarkerStyle(3);
  grp_corrected ->Fit("fit_t","B");
  TF1* fit_temp1 = grp_corrected->GetFunction("fit_t");
  fit_temp1->SetLineColor(kBlue);
  fit_temp1->SetLineStyle(2);
  fit_temp1->DrawCopy("same");

  // get fit results
  fit[0]=fit_temp1->GetParameter(0);
  fit[1]=fit_temp1->GetParError(0);
  fit[2]=fit_temp1->GetParameter(1);
  fit[3]=fit_temp1->GetParError(1);
  fit[4]=fit_temp1->GetParameter(2);
  fit[5]=fit_temp1->GetParError(2);
  Myfile<<"Fit results from corrected data: [0]cos(phi)-[1]sin(phi)+[2]"<<std::endl;
  Myfile<<"[0] = "<<setw(10)<<fit[0]<<" +- "<<setw(10)<<fit[1]<<std::endl;
  Myfile<<"[1] = "<<setw(10)<<fit[2]<<" +- "<<setw(10)<<fit[3]<<std::endl;
  Myfile<<"[2] = "<<setw(10)<<fit[3]<<" +- "<<setw(10)<<fit[5]<<std::endl;


  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in_out);
  grp->Add(grp_corrected);
  grp ->Draw("AP");
  grp ->SetTitle("");
  grp ->GetXaxis()->SetTitle("Octant");
  grp ->GetXaxis()->CenterTitle();
  grp ->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp ->GetYaxis()->CenterTitle();
  grp->GetXaxis()->SetTitleOffset(1.0);
  grp->GetYaxis()->SetTitleOffset(0.8);

  TPaveStats *stats1 = (TPaveStats*)grp_in_out->GetListOfFunctions()->FindObject("stats");
  stats1->SetTextColor(kRed);
  stats1->SetFillColor(kWhite); 
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);  

  TPaveStats *stats2 = (TPaveStats*)grp_corrected->GetListOfFunctions()->FindObject("stats");
  stats2->SetTextColor(kBlue);
  stats2->SetFillColor(kWhite); 
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.35);stats2->SetY2NDC(0.60);  

  TLegend *legend = new TLegend(0.1,0.75,0.25,0.85,"","brNDC");
  legend->AddEntry(fit_temp, "Un-corrected", "l");
  legend->AddEntry(fit_temp1, "Corrected", "l");
  legend->SetTextFont(132);
  legend->SetFillColor(0);
  legend->Draw("");

  
  // save canvas
  c->Modified();
  c->Update();

  c->Print(Form("slug_%i_%i_",slug,slug+1)+interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_octant_correction_plots_"+reg_set+"_"+database_stem+".C");
  c->Print(Form("slug_%i_%i_",slug,slug+1)+interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_octant_correction_plots_"+reg_set+"_"+database_stem+".png");


}
