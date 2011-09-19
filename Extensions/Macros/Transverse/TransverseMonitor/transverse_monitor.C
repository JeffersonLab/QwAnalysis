/****************************************************************
Author: B. P. Waidyawansa
Data : August 22nd 2011.

This script should be used to perfrom weekly monitoring of the amount of residual transverse polarization left in the longitudinally polarized Qweak electron beam. It access the unregressed data from the qweak data base and performs a fit over the main detector octants to calculat the ratio of 

A_T_measured_during_longitudinal/A_T_transverse = amount of transevrse polarization in the beam.

and can be then reported to source group for corrections.

Inputs : The last analyzed slug number
         The number of slugs you want to look back at from the given slug. To be able to see a 3 sigma deviation from the zero , you need to have at least 3days worth of data ~ 9 slugs.
         Enable fit of individual angles. 

Output : Plot of md bar asymmetry vs azimuthal angle for IHWP IN, OUT and IN+OUT
         Plot of md bar asymmetry vs azimuthal angle for IN-OUT and IN-OUT sum of opposite octants.
         Plot of md bar1 (most sensitive to horizontal transverse) and md bar 3(most sensitive to vertical transverse) asymmetries vs run number.

The fit uses the amplitude of -4.75 ppm from the results of vertical transverse running described in Qweak-doc-1401-v3.

e.q. ./transevrse_monitor 9 1   (use 9 slugs and use fit1)
         
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

/*Opposite bar sums array*/
TString opposite_quartz_bar_sum[4]=
  {"qwk_md1_qwk_md5","qwk_md2_qwk_md6","qwk_md3_qwk_md7","qwk_md4_qwk_md8"};


/*Database connection*/
TSQLServer *db;

/*Other variables*/
Int_t slug_num = 0;
Double_t barvaluesin[8];
Double_t barerrorsin[8];
Double_t barvaluesout[8];
Double_t barerrorsout[8];

Double_t oppvaluesin[4];
Double_t opperrorsin[4];
Double_t oppvaluesout[4];
Double_t opperrorsout[4];

Int_t slug_first = 0;
Int_t slug_range = 9;

Bool_t usefit1 = false;

/*Function defintions*/
TString get_sum_query(TString device,TString ihwp, Int_t slug_last);
void get_data(TString devicelist[],Int_t size,TString ihwp,Int_t slug_last,
		     Double_t value[],Double_t error[]);
void get_device_data(TString device,TString ihwp,TString wien,Int_t slug_last,TVectorD* value,
		     TVectorD*error,TVectorD* runlets);
TString runletbased_query(TString device,TString ihwp,TString wien,Int_t slug_last);
void plot_md_data(TString device, Int_t slug_last);
void plot_n_fit_data(Int_t size, TString fit, Double_t valuein[],Double_t errorin[], 
	       Double_t valueout[],Double_t errorout[]);

/*main function*/
int main(Int_t argc,Char_t* argv[])
{
  TApplication theApp("App",&argc,argv);

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.10);
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
  gDirectory->Delete("*");


  if(argc==2){
    slug_range = atoi(argv[1]);
  } 
  if(argc==3){
    slug_range = atoi(argv[1]);
    if(atoi(argv[2])) usefit1 = true;
  }

  std::cout<<"###########################################################"<<std::endl;
  std::cout<<" \n Qweak Residual Transverse Polarization Monitoring tool\n"<<std::endl;
  std::cout<<"   Created by: Buddhini Waidyawansa                "<<std::endl;
  std::cout<<"   contact : buddhini@jlab.org                     "<<std::endl;
  std::cout<<"##########################################################"<<std::endl;
  std::cout<<"NOTE: "<<std::endl;
  std::cout<<"This macro looks only at LH2 longitudinal data that are marked as parity+production and  "<<std::endl;
  std::cout<<"suspect+good or good only.It is the users responsibilty to make sure all the runs in the  "<<std::endl;
  std::cout<<"given slug range have been analyzed in to the database. The script doesn't have checks for  "<<std::endl;
  std::cout<<"this. So it should not be interpreted that all the runs in the slug range was included in the  \n"<<std::endl;
  std::cout<<"given results.  \n"<<std::endl;

  std::cout<<" Please enter the last completed slug number:"<<std::endl;
  std::cin>>slug_num;
  slug_first =  slug_num-slug_range;

  
  /*connect to the data base*/
  // db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");
  // db = TSQLServer::Connect("mysql://127.0.0.1/qw_run1_pass3","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://qweakdb.jlab.org/qw_run1_pass3","qweak", "QweakQweak");


  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
  
  /*Create a canvas*/
  TCanvas * Canvas1 = new TCanvas("canvas1", "Aaymmetry vs octants",0,0,1000,800);
  TCanvas * Canvas2 = new TCanvas("canvas2", "Oppposite octant sum vs opposit octant pair",0,0,1000,800);
 
 
 /*Clear the other arrays*/
  for(Int_t i =0;i<8;i++){
    barvaluesin[i]  = 0.0;
    barvaluesout[i] = 0.0;
    barerrorsin[i]  = 0.0;
    barerrorsout[i] = 0.0;
   }
  
  /*Get data from database*/
  std::cout<<"IHWP 'in'.."<<std::endl;
  get_data(quartz_bar_sum,8,"in",slug_num,barvaluesin,barerrorsin);
  get_data(opposite_quartz_bar_sum,4,"in",slug_num,oppvaluesin,opperrorsin);

  std::cout<<"IHWP 'out'.."<<std::endl;
  get_data(quartz_bar_sum,8,"out",slug_num,barvaluesout,barerrorsout);
  get_data(opposite_quartz_bar_sum,4,"out",slug_num,oppvaluesout,opperrorsout);


  /*Fit octants*/
  Canvas1->Draw();
  Canvas1->cd();
  plot_n_fit_data(8, "cosfit",  barvaluesin,barerrorsin, barvaluesout,barerrorsout);
  Canvas1->Update();
  if(usefit1)
    Canvas1->Print(Form("transverse_monitor_slugs_all_fit1_%i_%i_plots.png",slug_first,slug_num));
  else
    Canvas1->Print(Form("transverse_monitor_slugs_all_%i_%i_plots.png",slug_first,slug_num));

  /*Fit opposite bar sums*/
  Canvas2->Draw();
  Canvas2->cd();
  plot_n_fit_data(4, "pol0",  oppvaluesin,opperrorsin, oppvaluesout,opperrorsout);
  Canvas2->Update();
  Canvas2->Print(Form("transverse_monitor_slugs_opposite_%i_%i_plots.png",slug_first,slug_num));

 
 
  // std::cout<<"On to plotting mdall, md3 and md5 asymmetry vs runlets "<<std::endl;
//   /*Create a canvas*/
//   TString title1 = Form("LH2: Regressed asymmetries of Main detectors from slugs %i to %i",slug_first,slug_num);
//   TCanvas * Canvas1 = new TCanvas("canvas1", title1,0,0,1000,1000);
//   Canvas1->Draw();
//   Canvas1->cd();

//   TPad*pad11 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
//   TPad*pad22 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
//   pad11->SetFillColor(20);
//   pad11->Draw();
//   pad22->Draw();

//   pad11->cd();
//   TString text1 = Form(title1);
//   TText*t11 = new TText(0.06,0.3,text1);
//   t11->SetTextSize(0.5);
//   t11->Draw();

//   pad22->cd();
//   pad22->Divide(1,3);
//   pad22->cd(1);
//   plot_md_data("qwk_mdallbarsum", slug_num);
//   pad22->cd(2);
//   plot_md_data("qwk_md3barsum", slug_num);
//   pad22->cd(3);
//   plot_md_data("qwk_md5barsum", slug_num);
//   Canvas1->Update();
//   Canvas1->Print(Form("transverse_monitor_runlet_plots_for_slugs_%i_%i.png",slug_first,slug_num));

  std::cout<<"Done! \n";
  db->Close();
  delete db;
  theApp.Run();
  return(1);
}


/*A function to create the mysql query*/
TString get_sum_query(TString device,TString ihwp, Int_t slug_last){

  Bool_t ldebug = false;

  std::cout<<"Getting regressed data for "<<device<<std::endl;
  TString datatable = "md_data_view";
  Int_t slug_first = slug_last-slug_range; // select the last 9 slugs including slug_last 
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
  TString slug_cut   = Form("(%s.slug >= %i and %s.slug <= %i)",
			  datatable.Data(),slug_first,datatable.Data(),slug_last);

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on' ",
			    datatable.Data(),datatable.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}


/*A function to get data from the database for the octants*/
void get_data(TString devicelist[],Int_t size,TString ihwp,Int_t slug_last,
		     Double_t value[], Double_t error[]){
  
  Bool_t ldebug = false;

  TString query;
  TSQLStatement* stmt = NULL;
  
  std::cout<<"Extracting average asymmetries from slug "<<(slug_last-slug_range)<<" to "<<slug_last<<std::endl;
  for(Int_t i=0 ; i<size ;i++){
    if(ldebug) printf("Getting data for %10s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    
    query = get_sum_query(Form("%s",devicelist[i].Data()),ihwp,slug_last);
    
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
      }
    }
    delete stmt;    
  }
}

/*A query to get runlet based regressed data from the database for given detector*/
TString runletbased_query(TString device,TString ihwp,TString wien, Int_t slug_last){

Bool_t ldebug = false;

 TString datatable = "md_data_view";
 Int_t slug_first = slug_last-slug_range; // select the last 9 slugs including slug_last 
 
 TString output = datatable+".value, "+datatable+".error, ("
   +datatable+".run_number+0.1*"+datatable+".segment_number) ";

 TString slug_cut   = Form("(%s.slug >= %i and %s.slug <= %i)",
			  datatable.Data(),slug_first,datatable.Data(),slug_last);

  TString run_quality =  Form("(%s.run_quality_id = '1')",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on' ",
			    datatable.Data(),datatable.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT distinct " +output+ ", (slow_controls_data.value<0)*-2+1 as wien_reversal FROM "
    +datatable+", run, slow_controls_settings,slow_controls_data WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".runlet_id =  slow_controls_data.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+
    "' AND ((slow_controls_data.value<0)*-2+1)*1 = "+wien+
    "  AND "+good_for+" AND "
    +datatable+".error != 0; ";
  
  if(ldebug) std::cout<<query<<std::endl;
  
  return query;
}


/*A function to get data from the database for a given device*/
void get_device_data(TString device,TString ihwp,TString wien,Int_t slug_last,TVectorD *value,
		     TVectorD *error,TVectorD *runlets){
  
  Bool_t ldebug = false;
  
  TString query;
  TSQLStatement* stmt = NULL;
  
  std::cout<<"Extracting asymmetries from slug "<<(slug_last-slug_range)<<" to "<<slug_last<<std::endl;
  if(ldebug) printf("Getting data for %10s ihwp %5s ", device.Data(), ihwp.Data());
    
  query = runletbased_query(device,ihwp,wien,slug_last);
  
  stmt = db->Statement(query,100);
  if(!stmt)  {
    db->Close();
    exit(1);
  }

  value->Clear();
  error->Clear();
  runlets->Clear();

  /* process statement */
  if (stmt->Process()) {
    /* store result of statement in buffer*/
    stmt->StoreResult();
    /*Process row after row*/
    Int_t i=0;
    while (stmt->NextResultRow()) {
      value->ResizeTo(i+1);
      error->ResizeTo(i+1);
      runlets->ResizeTo(i+1);
      value->operator()(i)   = (stmt->GetDouble(0))*1e6; // convert to  ppm
      error->operator()(i)   = (stmt->GetDouble(1))*1e6; // ppm
      runlets->operator()(i) = (stmt->GetDouble(2));   // run+0.1*runlet
      i++;
    }
  }
  delete stmt;    
}


/*A function to plot md_all, md3 and md1 asymmetry vs runlet*/
void plot_md_data(TString device, Int_t slug_last){

  TVectorD value_in_L;
  TVectorD error_in_L;
  TVectorD runlets_in_L;
  TVectorD runlets_err_in_L;
  TVectorD value_in_R;
  TVectorD error_in_R;
  TVectorD runlets_in_R;
  TVectorD runlets_err_in_R;
  TVectorD value_out_L;
  TVectorD error_out_L;
  TVectorD runlets_out_L;
  TVectorD runlets_err_out_L;
  TVectorD value_out_R;
  TVectorD error_out_R;
  TVectorD runlets_out_R;
  TVectorD runlets_err_out_R;
  gStyle->SetOptFit(0);
  

  value_in_L.Clear();
  value_in_L.ResizeTo(0);
  error_in_L.Clear();
  error_in_L.ResizeTo(0);
  runlets_in_L.Clear();
  runlets_in_L.ResizeTo(0);
  runlets_err_in_L.Clear();
  runlets_err_in_L.ResizeTo(0);
  value_in_R.Clear();
  value_in_R.ResizeTo(0);
  error_in_R.Clear();
  error_in_R.ResizeTo(0);
  runlets_in_R.Clear();
  runlets_in_R.ResizeTo(0);
  runlets_err_in_R.Clear();
  runlets_err_in_R.ResizeTo(0);
  value_out_L.Clear();
  value_out_L.ResizeTo(0);
  error_out_L.Clear();
  error_out_L.ResizeTo(0);
  runlets_out_L.Clear();
  runlets_out_L.ResizeTo(0);
  runlets_err_out_L.Clear();
  runlets_err_out_L.ResizeTo(0);
  value_out_R.Clear();
  error_out_R.ResizeTo(0);
  error_out_R.Clear();
  error_out_R.ResizeTo(0);
  runlets_out_R.Clear();
  runlets_out_R.ResizeTo(0);
  runlets_err_out_R.Clear();
  runlets_err_out_R.ResizeTo(0);


 
  // TPad* pad = (TPad*)(gPad->GetMother());


  /*Get data from database for md allbars*/
  std::cout<<"IHWP in' WIEN right."<<std::endl;
  get_device_data("qwk_mdallbars","in","1",slug_last,&value_in_R,&error_in_R,&runlets_in_R);
  std::cout<<"IHWP in' WIEN left."<<std::endl; 
  get_device_data("qwk_mdallbars","in","-1",slug_last,&value_in_L,&error_in_L,&runlets_in_L);
  std::cout<<"IHWP out' WIEN right."<<std::endl;
  get_device_data("qwk_mdallbars","out","1",slug_last,&value_out_R,&error_out_R,&runlets_out_R);
  std::cout<<"IHWP out' WIEN left."<<std::endl; 
  get_device_data("qwk_mdallbars","out","-1",slug_last,&value_out_L,&error_out_L,&runlets_out_L);

  for(Int_t i = 0;i<value_out_R.GetNoElements();i++)  {runlets_err_out_R.ResizeTo(i+1);runlets_err_out_R.operator()(i) = 0.0;}
  for(Int_t i = 0;i<value_out_L.GetNoElements();i++)  {runlets_err_out_L.ResizeTo(i+1);runlets_err_out_L.operator()(i) = 0.0;}
  for(Int_t i = 0;i<value_in_R.GetNoElements(); i++)  {runlets_err_in_R.ResizeTo(i+1) ;runlets_err_in_R.operator()(i)  = 0.0;}
  for(Int_t i = 0;i<value_in_L.GetNoElements(); i++)  {runlets_err_in_L.ResizeTo(i+1) ;runlets_err_in_L.operator()(i)  = 0.0;}

  

  /*Draw IN_R values*/
  TGraphErrors* grp_in_R   = new TGraphErrors(runlets_in_R,value_in_R,runlets_err_in_R,error_in_R);
  grp_in_R ->SetMarkerSize(0.6);
  grp_in_R ->SetMarkerStyle(21);
  grp_in_R ->SetMarkerColor(kBlue);
  grp_in_R->Fit("pol0");
  TF1* fit1 = grp_in_R->GetFunction("pol0");
  fit1->SetLineColor(kBlue);
  fit1->DrawCopy("same");


  TGraphErrors* grp_in_L   = new TGraphErrors(runlets_in_L,value_in_L,runlets_err_in_L,error_in_L);
  grp_in_L ->SetMarkerSize(0.6);
  grp_in_L ->SetMarkerStyle(21);
  grp_in_L ->SetMarkerColor(kOrange);
  grp_in_L->Fit("pol0");
  TF1* fit2 = grp_in_L->GetFunction("pol0");
  fit2->SetLineColor(kOrange);
  fit2->DrawCopy("same");

  TGraphErrors* grp_out_R  = new TGraphErrors(runlets_out_R,value_out_R,runlets_err_out_R,error_out_R);
  grp_out_R ->SetMarkerSize(0.6);
  grp_out_R ->SetMarkerStyle(21);
  grp_out_R ->SetMarkerColor(kRed);
  grp_out_R->Fit("pol0");
  TF1* fit3 = grp_out_R->GetFunction("pol0");
  fit3->SetLineColor(kRed);
  fit3->DrawCopy("same");

  TGraphErrors* grp_out_L  = new TGraphErrors(runlets_out_L,value_out_L,runlets_err_out_L,error_out_L);
  grp_out_L ->SetMarkerSize(0.6);
  grp_out_L ->SetMarkerStyle(21);
  grp_out_L ->SetMarkerColor(kViolet);
  grp_out_L->Fit("pol0");
  TF1* fit4 = grp_out_L->GetFunction("pol0");
  fit4->SetLineColor(kViolet);
  fit4->DrawCopy("same");


  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in_R);
  grp->Add(grp_out_R);
  grp->Add(grp_in_L);
  grp->Add(grp_out_L);
  grp->Draw("AP");

  grp->SetTitle(Form("%s asymmetry variation over the runs",device.Data()));
  grp->GetXaxis()->SetTitle("run+runlet");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend = new TLegend(0.8,0.7,0.99,0.95,"","brNDC");
  legend->AddEntry(grp_in_R, Form("<IN_R>  = %2.3f#pm%2.3f", 
				  fit1->GetParameter(0), fit1->GetParError(0)), "p");
  legend->AddEntry(grp_out_R,Form("<OUT_R> = %2.3f#pm%2.3f", 
				  fit3->GetParameter(0), fit3->GetParError(0)), "p");
  legend->AddEntry(grp_in_L, Form("<IN_L>  = %2.3f#pm%2.3f", 
				  fit2->GetParameter(0), fit2->GetParError(0)), "p");
  legend->AddEntry(grp_out_L,Form("<OUT_L> = %2.3f#pm%2.3f", 
				  fit4->GetParameter(0), fit4->GetParError(0)), "p");
  legend->SetFillColor(0);
  legend->Draw("");

}


/*Function to plot and fit data*/
void plot_n_fit_data(const Int_t size, TString fit, Double_t valuesin[],Double_t errorsin[], 
		     Double_t valuesout[],Double_t errorsout[]){

  TF1 *cosfit;
  TF1 *cosfit_diff;
  TString usefit="";
  Double_t phase_low;
  Double_t phase_up;

  Double_t x[size];
  Double_t errx[size];
  Double_t valuesum[size];
  Double_t valueerror[size];
  Double_t valuediff[size];
  Double_t errordiff[size];

  /*set X location and clear the other arrays*/
  for(Int_t i =0;i<size;i++){
    x[i]         = i+1;
    errx[i]      = 0.0;
    valuesum[i]  = 0.0;
    valueerror[i]= 0.0;
    valuediff[i] = 0.0;
    errordiff[i] = 0.0;
  }
  
  TPad* pad = (TPad*)(gPad->GetMother());
  pad->cd();
  pad->Clear();
  
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TString title;

  if(fit == "cosfit"){
    if(usefit1)
      title = Form("LH2: Regressed Main detector asymmetries from slugs %i to %i: Fit (-4.75)*p0*sin(p1)*[sin(p2)*cos(phi) - cos(p2)*sin(phi)]+ p3",slug_first,slug_num);
    else
      title = Form("LH2: Regressed Main detector asymmetries from slugs %i to %i: Fit (-4.75)*(p0*cos(phi) - p1*sin(phi))+ p2",slug_first,slug_num);
  }
  if(fit == "pol0")
    title = Form("LH2: Regressed slug averages of Opposite Main detector bar sum asymmetries from slugs %i to %i: Fit p0",slug_first,slug_num);


  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.35);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(1,2);
  
  pad2->cd(1);


  /*cosine fit is for the octants and constant fit is for the opposite octants*/
  if(fit == "pol0"){
    usefit = "pol0";
  }  
  if(fit =="cosfit"){

    usefit = "cosfit";
    // Define the cosine fit
    //    cosfit = new TF1("cosfit","[0]*-4.75*cos((pi/180)*(45*(x-1)+[1])) +[2]",1,8);
    
    if(usefit1){
      cosfit = new TF1("cosfit"," (-4.75)*[0]*sin((pi/180)*[1])*(sin((pi/180)*[2])*cos((pi/180)*(45*(x-1))) - cos((pi/180)*[2])*sin((pi/180)*(45*(x-1))))+ [3]",1,8);
      cosfit->SetParLimits(0,0,99999);
      cosfit->SetParLimits(1,0,99999);
      cosfit->SetParLimits(1,-180,180);
      cosfit->SetParLimits(2,-180,180);
    } 
    else {
      cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
    }
  }

  /*Draw IN values*/
  TGraphErrors* grp_in  = new TGraphErrors(size,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.8);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->SetLineColor(kBlue);
  grp_in->Fit(usefit,"EM");
  TF1* fit1 = grp_in->GetFunction(usefit);

  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
 

  if(fit =="cosfit"){

    // Define the cosine fit for out. Use the p0, p1 and p2 for initialization and limits of this fit.
    // We dont't want the phase to change with the IHWP. The sign of the amplitude is the onlything
    // that should change with IHWP. But we will allow a +-90 degree leverage for the phase to see 
    // if fo some reason, changing the IHWP has changed the verticle to horizontal transverse
    // or vise versa.
    
    if(usefit1){
      cosfit = new TF1("cosfit"," (-4.75)*[0]*sin((pi/180)*[1])*(sin((pi/180)*[2])*cos((pi/180)*(45*(x-1))) - cos((pi/180)*[2])*sin((pi/180)*(45*(x-1))))+ [3]",1,8);

      cosfit->SetParLimits(0,-99999,0);
      cosfit->SetParameter(0,-(fit1->GetParameter(0)));
      cosfit->SetParameter(1, (fit1->GetParameter(1)));
      cosfit->SetParameter(2, (fit1->GetParameter(2)));

      phase_low = fit1->GetParameter(1)-90;
      phase_up  = fit1->GetParameter(1)+90;
      cosfit->SetParLimits(1,phase_low,phase_up);
      phase_low = fit1->GetParameter(2)-90;
      phase_up  = fit1->GetParameter(2)+90;
      cosfit->SetParLimits(2,phase_low,phase_up);
    } 
    else {
      cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
      cosfit->SetParameter(0,-(fit1->GetParameter(0)));
      cosfit->SetParameter(1,-(fit1->GetParameter(1)));
      cosfit->SetParameter(2,-(fit1->GetParameter(2)));

    }
    
  }


   /*Draw OUT values*/
  TGraphErrors* grp_out  = new TGraphErrors(size,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.8);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out ->SetLineColor(kRed);

  grp_out->Fit(usefit,"EM");
  TF1* fit2 = grp_out->GetFunction(usefit);
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);

  /* Draw IN+OUT values */
  for(Int_t i =0;i<size;i++){
    valuesum[i]=(valuesin[i]+valuesout[i])/2;
    valueerror[i]= sqrt(pow(errorsin[i],2)+pow(errorsout[i],2))/2;
  }

  TGraphErrors* grp_sum  = new TGraphErrors(size,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.8);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum ->SetLineColor(kGreen-2);

  grp_sum->Fit("pol0");
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
  grp->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IHWP-IN", "p");
  legend->AddEntry(grp_out, "IHWP-OUT", "p");
  legend->AddEntry(grp_sum, "IN+OUT", "p");
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
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); 
  stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
  stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
  stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  pad2->cd(2);

  /* Draw IN-OUT values */
  for(Int_t i =0;i<size;i++){
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1.0/pow(errorsin[i],2)) + (1.0/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1.0/((1.0/(pow(errorsin[i],2)))+(1.0/pow(errorsout[i],2))));
  }

  TGraphErrors* grp_diff  = new TGraphErrors(size,x,valuediff,errx,errordiff);
  grp_diff ->SetMarkerSize(0.8);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta-2);
  grp_diff ->SetLineColor(kMagenta-2);

  if(usefit == "cosfit"){
    usefit = "cosfit_diff";
    if(usefit1){
      cosfit_diff = new TF1("cosfit_diff"," (-4.75)*[0]*sin((pi/180)*[1])*(sin((pi/180)*[2])*cos((pi/180)*(45*(x-1))) - cos((pi/180)*[2])*sin((pi/180)*(45*(x-1))))+ [3]",1,8);
      cosfit_diff->SetParLimits(0,0,99999);
      cosfit_diff->SetParLimits(1,0,99999);
      cosfit_diff->SetParLimits(1,-180,180);
      cosfit_diff->SetParLimits(2,-180,180);
    } 
    else {
      cosfit_diff = new TF1("cosfit_diff","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
    }
  } 
  else usefit = "pol0";

  grp_diff->Fit(usefit,"EM");
  grp_diff->SetTitle("Graph of IN-OUT");
  TF1* fit4 = grp_diff->GetFunction(usefit);
  if(fit4 != NULL){
    fit4->DrawCopy("same");
    fit4->SetLineColor(kMagenta-2);
    grp_diff->Draw("AP");
  }

  pad->Modified();
  pad->Update();
}
