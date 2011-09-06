/****************************************************************
Author: B. P. Waidyawansa
Data : September 9th 2011.

This script is used to track the changes in the transverse polarization phase(if there is any)
within the given run range.
Inputs : Start run, end run (run range)
         

Output : Plot of fit parameter vs runlet number for the fit of A = p0 sin(phi + p1) + p2

         
****************************************************************/

using namespace std;

#include <TString.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TApplication.h>
#include <math.h>
#include <TCanvas.h>
#include <iostream>
#include <Rtypes.h>
#include <TROOT.h>
#include <stdexcept>
#include <TLine.h>
#include <TPaveStats.h>
#include <stdio.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TText.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TPad.h>
#include <TVector.h>

/*Main detector array (bar sums)*/
TString quartz_bar_sum[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum",
   "qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

/*Database connection*/
TSQLServer *db;

/*Other variables*/
Int_t run_first = 0;
Int_t run_last = 0;
Int_t end_run = 0;
TVectorD x_i;
TVectorD xerr_i;
TVectorD x_o;
TVectorD xerr_o;
TVectorD p1_i;
TVectorD p1_ie;
TVectorD p1_o;
TVectorD p1_oe;
Double_t *fit_value;
Double_t *fit_error;

/*Function defintions*/
TString data_query(TString device,Int_t run,TString ihwp);
void fit_octants_data(TString devicelist[],Int_t run,  TString ihwp, Double_t *fit_value,
		      Double_t *fit_error);


/*main function*/
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
  gStyle->SetTitleYOffset(0.5);
  gStyle->SetTitleXOffset(0.5);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  gDirectory->Delete("*");


  if(argc==3){
    run_first = atoi(argv[1]);
    run_last = atoi(argv[2]);
  } else {
    std::cout<<"The correct syntax is ./phase_tracker firstrun lastrun"<<std::endl;
    exit(1);
  }

  std::cout<<"##########################################################"<<std::endl;
  std::cout<<" \n Qweak Residual Transverse Polarization Phase Tracker\n"<<std::endl;
  std::cout<<"   Created by: Buddhini Waidyawansa                       "<<std::endl;
  std::cout<<"   contact : buddhini@jlab.org                            "<<std::endl;
  std::cout<<"##########################################################"<<std::endl;
  std::cout<<"NOTE: "<<std::endl;
  std::cout<<"This macro looks only at LH2 longitudinal data that are marked as parity+production and  "<<std::endl;
  std::cout<<"run quality good only.It is the users responsibilty to make sure all the runs in the  "<<std::endl;
  std::cout<<"given run range have been analyzed in to the database. The script doesn't have checks for  "<<std::endl;
  std::cout<<"this. So it should not be interpreted that all the runs in the slug range was included in the given results."<<std::endl;


  
  /*connect to the data base*/
  //db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");
  //db = TSQLServer::Connect("mysql://127.0.0.1/qw_run1_pass3","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://qweakdb.jlab.org/qw_run1_pass3","qweak", "QweakQweak");


  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
  
  /*Create a canvas*/
  TString title = Form("Variation of fit parameters within runs %i to %i: Fit p0*-4.75*Cos(phi+p1)+p2",
		       run_first,run_last);
  TCanvas * Canvas = new TCanvas("canvas", title,0,0,1000,800);
  Canvas->Draw();
  Canvas->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();

 
  pad1->cd();
  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.35);
  t1->Draw();

  pad2->cd();



 /*set X location and clear the other arrays*/
  x_o.Clear();
  x_o.ResizeTo(0);
  x_i.Clear();
  x_i.ResizeTo(0);
  xerr_o.Clear();
  xerr_o.ResizeTo(0);
  xerr_i.Clear();
  xerr_i.ResizeTo(0);
  p1_i.Clear();
  p1_ie.ResizeTo(0);
  p1_o.Clear();
  p1_oe.ResizeTo(0);

  Int_t ki =0;
  Int_t ko =0;


  /*Get data from database for the given run range, fit and extract the fit parameters*/
  for(Int_t i=run_first;i<run_last;i++){

    fit_octants_data(quartz_bar_sum,i,"in",fit_value,fit_error);
    if(fit_value == 0 && fit_error == 0) {
      /*do nothing. The run number is not in the selected list*/
    } else {
      x_i.ResizeTo(ki+1);    x_i.operator()(ki)=i;
      xerr_i.ResizeTo(ki+1); xerr_i.operator()(ki)=0.0;
      p1_i.ResizeTo(ki+1);   p1_i.operator()(ki)=*fit_value;
      p1_ie.ResizeTo(ki+1);  p1_ie.operator()(ki)=*fit_error;
      ki++;
    }
    *fit_value = 0.0;
    *fit_error = 0.0;
    fit_octants_data(quartz_bar_sum,i,"out",fit_value,fit_error);
    if(fit_value == 0 && fit_error == 0) {
      /*do nothing. The run number is not in the selected list*/
    } else {
      x_o.ResizeTo(ko+1);    x_o.operator()(ko)=i;
      xerr_o.ResizeTo(ko+1); xerr_o.operator()(ko)=0.0;
      p1_o.ResizeTo(ko+1);   p1_o.operator()(ko)=*fit_value;
      p1_oe.ResizeTo(ko+1);  p1_oe.operator()(ko)=*fit_error;
      ko++;
    }

  }
 
  if(ko==0 && ki==0)std::cout<<"Not data!"<<std::endl;
    else{

  /*Draw p1 values*/
  pad2->cd(1);
  TGraphErrors* grp_p1out  = new TGraphErrors(x_o,p1_o,xerr_o,p1_oe);
  grp_p1out ->SetMarkerSize(0.6);
  grp_p1out ->SetMarkerStyle(21);
  grp_p1out ->SetMarkerColor(kRed);

  TGraphErrors* grp_p1in  = new TGraphErrors(x_i,p1_i,xerr_i,p1_ie);
  grp_p1in ->SetMarkerSize(0.6);
  grp_p1in ->SetMarkerStyle(21);
  grp_p1in ->SetMarkerColor(kRed);

  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_p1in);
  grp->Add(grp_p1out);
  grp->Draw("AP");

  Canvas->Update();
  Canvas->Print(Form("transverse_fit_plots_for_runs_%i_%i.png",run_first,run_last));
    }
  std::cout<<"Done! \n";
  db->Close();
  delete db;
  theApp.Run();
  return(1);
}


/*A function to create the mysql query to grab the runlet average of a md detector*/
TString data_query(TString device,Int_t run,TString ihwp){

  Bool_t ldebug = false;

  if(ldebug) std::cout<<"Getting regressed data for "<<device<<std::endl;
  TString datatable = "md_data_view";
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
 
  TString run_cut   = Form("%s.run_number = %i",
			   datatable.Data(),run);

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on' ",
			    datatable.Data(),datatable.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+" , slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +run_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}


/*A function to get data from the database for the octants*/
void fit_octants_data(TString devicelist[],Int_t run, TString ihwp, Double_t *fit_value, 
		      Double_t *fit_error){
  
  Bool_t ldebug = true;
  Bool_t empty = false;
  TString query;
  TSQLStatement* stmt = NULL;
  Double_t value[8];
  Double_t error[8];
  Double_t x[8];
  Double_t xerr[8];
  Int_t k = 0; //counter to count how many detectors report 0 values in one run.

 /*set X location and clear the other arrays*/
  for(Int_t i =0;i<8;i++){
    x[i]         = i+1;
    xerr[i]      = 0.0;
    value[i]     = 0.0;
    error[i]     = 0.0;
  }
  std::cout<<"Extracting average asymmetries from run "<<run<<std::endl;
  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) printf("Getting data for %10s ", devicelist[i].Data());
    
    query = data_query(Form("%s",devicelist[i].Data()),run,ihwp);
    
    stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    /* process statement */
    if (stmt->Process()) {
      /* store result of statement in buffer*/
      stmt->StoreResult();
      while (stmt->NextResultRow()) {
	value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
	if((value[i]==0) and (error[i]==0)) {
	  empty = true;
	}
      }
    }
    delete stmt;    
  }

  /*Now fit the results if they are not empty*/
 // Define the cosine fit
  TF1 *cosfit = new TF1("cosfit","[0]*cos((pi/180)*(45*(x-1)+[1])) +[2]",1,8);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(2,0);
  cosfit->SetParLimits(1,-45,135);
  
  if (!empty){
    if(ldebug) std::cout<<"Fit data"<<std::endl;
    TGraphErrors* grp  = new TGraphErrors(8,x,value,xerr,error);
    grp->Fit("cosfit","QV");
    TF1* fit1 = grp->GetFunction("cosfit");
    *fit_value = fit1->GetParameter(1); //p1
    *fit_error = fit1->GetParError(1); //p1 error
    
    if(ldebug) {
      std::cout<<"p1 =" <<*fit_value<< "+-" <<*fit_error << std::endl;
    }
  }
  else std::cout<<"Found 0 values in detectors. Ignore this run."<<std::endl;
}

