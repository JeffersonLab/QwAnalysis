/****************************************************************
Author: B. P. Waidyawansa
Data : November 09nd 2011.

Fit the pass3 data for one IHWP state within the given slug and from the data obtained,
removes the contribution from the horizontal compomenent.
With the remining data with the vertical contribution, sums over opposite bars to check the cancellation.

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
#include <TMath.h>

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


/*Function defintions*/
TString get_sum_query(TString device,TString ihwp, Int_t slug_last);
void get_data(TString devicelist[],Int_t size,TString ihwp,Int_t slug_last,
		     Double_t value[],Double_t error[]);
void get_device_data(TString device,TString ihwp,TString wien,Int_t slug_last,TVectorD* value,
		     TVectorD*error,TVectorD* runlets);
TString runletbased_query(TString device,TString ihwp,TString wien,Int_t slug_last);
void plot_md_data(TString device, Int_t slug_last);
void plot_n_fit_data(Int_t size, Double_t valuein[],Double_t errorin[], 
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
  }

 
  std::cout<<" Please enter the last completed slug number:"<<std::endl;
  std::cin>>slug_num;
  slug_first =  slug_num-slug_range;
  
  
  /*connect to the data base*/
  // db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");
  // db = TSQLServer::Connect("mysql://127.0.0.1/qw_run1_pass3","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://qweakdb.jlab.org/qw_run1_pass3","qweak", "QweakQweak");

  //db = TSQLServer::Connect("mysql://localhost.jlab.org/qw_run1_pass3","qweak", "QweakQweak");

  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
  
  /*Create a canvas*/
  TCanvas * Canvas1 = new TCanvas("canvas1", "Aaymmetry vs octants",0,0,1000,800);
 
 
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

  std::cout<<"IHWP 'out'.."<<std::endl;
  get_data(quartz_bar_sum,8,"out",slug_num,barvaluesout,barerrorsout);


  /*Fit octants*/
  Canvas1->Draw();
  Canvas1->cd();
  plot_n_fit_data(8,barvaluesin,barerrorsin, barvaluesout,barerrorsout);
  Canvas1->Update();

  Canvas1->Print(Form("remove_horizontal_from_slugs_%i_%i.png",slug_first,slug_num));

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
void plot_n_fit_data(const Int_t size, Double_t valuesin[],Double_t errorsin[], 
		     Double_t valuesout[],Double_t errorsout[]){

  TF1 *cosfit;
  TF1 *cosfit_diff;
  TF1 *cosfit_1;
  TString usefit="";
  Double_t phase_low;
  Double_t phase_up;

  Double_t x[size];
  Double_t errx[size];
  Double_t valuesum[size];
  Double_t valueerror[size];
  Double_t valuediff[size];
  Double_t errordiff[size];
  Double_t c_val_in[size];
  Double_t c_err_in[size];
  Double_t c_val_out[size];
  Double_t c_err_out[size];
  Double_t c_valuesum[size];
  Double_t c_valueerror[size];

  Double_t c_opp_in[4];
  Double_t c_opp_err_in[4];
  Double_t c_opp_out[4];
  Double_t c_opp_err_out[4];
  Double_t x_opp[4];
  Double_t errx_opp[4];
  Double_t c_opp_valuesum[size];
  Double_t c_opp_valueerror[size];

  Double_t in_ph=0;
  Double_t out_ph=0;
  Double_t in_p2=0;
  Double_t in_p2_e=0;
  Double_t out_p2=0;
  Double_t out_p2_e=0;
  Double_t in_ph_e=0;
  Double_t out_ph_e=0;

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
  title = Form("LH2: Regressed Main detector asymmetries from slugs %i to %i: Fit (-4.75)*(p0*cos(phi) - p1*sin(phi))+ p2",slug_first,slug_num);


  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.35);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(1,3);
  
  pad2->cd(1);


  /*cosine fit is for the octants and constant fit is for the opposite octants*/
  usefit = "cosfit";
  cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);

  /*Draw IN values*/
  TGraphErrors* grp_in  = new TGraphErrors(size,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.8);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->SetLineColor(kBlue);
  grp_in->Fit(usefit,"EM");
  TF1* fit1 = grp_in->GetFunction(usefit);
  in_ph= fit1->GetParameter(1);
  in_ph_e= fit1->GetParError(1);
  in_p2= fit1->GetParameter(2);
  in_p2_e= fit1->GetParError(2);

  std::cout<<"fit 1 parameters 1 ="<<in_ph<<std::endl;
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
 

  cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
  cosfit->SetParameter(0,-(fit1->GetParameter(0)));
  cosfit->SetParameter(1,-(fit1->GetParameter(1)));
  cosfit->SetParameter(2,-(fit1->GetParameter(2)));  
  

  /*Draw OUT values*/
  TGraphErrors* grp_out  = new TGraphErrors(size,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.8);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out ->SetLineColor(kRed);

  grp_out->Fit(usefit,"EM");
  TF1* fit2 = grp_out->GetFunction(usefit);
  out_ph= fit2->GetParameter(1);
  out_ph_e= fit2->GetParError(1);
  out_p2= fit2->GetParameter(2);
  out_p2_e= fit2->GetParError(2);

  std::cout<<"fit 2 parameters 1 ="<<out_ph<<std::endl;

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
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); 
  stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
  stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
  stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  pad2->cd(2);
  
  //Remove the horizontal component
  for(Int_t i=0;i<size;i++){
    c_val_in[i]= valuesin[i]-(4.75*in_ph*sin((TMath::Pi()/180)*(45*(i))))-in_p2; // I can live with the P2 term
    c_err_in[i]=sqrt( errorsin[i]*errorsin[i] + (sin((TMath::Pi()/180)*(45*(i)))*sin((TMath::Pi()/180)*(45*(i))))*(-4.75*-4.75*in_ph_e*in_ph_e + 0.12*0.12*in_ph*in_ph)- in_p2_e*in_p2_e);

    std::cout<<"\nValue in "<<valuesin[i]<<std::endl;
    std::cout<<"Corrected value in "<<c_val_in[i]<<std::endl;

    c_val_out[i]= valuesout[i]-(4.75*out_ph*sin((TMath::Pi()/180)*(45*(i))))-out_p2; // I can live with the P2 term
    c_err_out[i]=sqrt( errorsout[i]*errorsout[i] + (sin((TMath::Pi()/180)*(45*(i)))*sin((TMath::Pi()/180)*(45*(i-1))))*(-4.75*-4.75*out_ph_e*out_ph_e + 0.12*0.12*out_ph*out_ph) - out_p2_e*out_p2_e);

    std::cout<<"\nValue out "<<valuesout[i]<<std::endl;
    std::cout<<"Corrected value out"<<c_val_out[i]<<std::endl;
  }


  /*Draw IN values*/
  cosfit_1 = new TF1("cosfit_1","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);

  TGraphErrors * grp_in_1  = new TGraphErrors(size,x,c_val_in,errx,c_err_in);
  grp_in_1 ->SetMarkerSize(0.8);
  grp_in_1 ->SetMarkerStyle(21);
  grp_in_1 ->SetMarkerColor(kBlue);
  grp_in_1 ->SetLineColor(kBlue);
  grp_in_1->Fit(cosfit_1,"EM");
  TF1* fit11 = grp_in_1->GetFunction("cosfit_1");
  fit11->DrawCopy("same");
  fit11->SetLineColor(kBlue);
 


  /*Draw OUT values*/
  cosfit_1 = new TF1("cosfit_1","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
  cosfit_1->SetParameter(0,(fit2->GetParameter(0)));
  // cosfit_1->SetParameter(1,(fit2->GetParameter(1)));
  cosfit_1->SetParameter(2,(fit2->GetParameter(2)));  

  TGraphErrors * grp_out_1  = new TGraphErrors(size,x,c_val_out,errx,c_err_out);
  grp_out_1 ->SetMarkerSize(0.8);
  grp_out_1 ->SetMarkerStyle(21);
  grp_out_1 ->SetMarkerColor(kRed);
  grp_out_1 ->SetLineColor(kRed);
  grp_out_1->Fit(cosfit_1,"EM");
  TF1* fit22 = grp_out_1->GetFunction("cosfit_1");
  fit22->DrawCopy("same");
  fit22->SetLineColor(kRed);


  for(Int_t i =0;i<size;i++){
    c_valuesum[i]=(c_val_in[i]+c_val_out[i])/2;
    c_valueerror[i]= sqrt(pow(c_err_in[i],2)+pow(c_err_out[i],2))/2;
    std::cout<<"value sum = "<<c_valuesum[i]<<std::endl;
  }

  TGraphErrors* grp_sum_1  = new TGraphErrors(size,x,c_valuesum,errx,c_valueerror);
  grp_sum_1 ->SetMarkerSize(0.8);
  grp_sum_1 ->SetMarkerStyle(21);
  grp_sum_1 ->SetMarkerColor(kGreen-2);
  grp_sum_1 ->SetLineColor(kGreen-2);
  grp_sum_1->Fit("pol0");
  TF1* fit33 = grp_sum_1->GetFunction("pol0");
  fit33->DrawCopy("same");
  fit33->SetLineColor(kGreen-2);


  TMultiGraph * grp1 = new TMultiGraph();
  grp1->Add(grp_in_1);
  grp1->Add(grp_out_1);
  grp1->Add(grp_sum_1);
  grp1->Draw("AP");

  grp1->SetTitle("");
  grp1->GetXaxis()->SetTitle("Octant");
  grp1->GetXaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitle("Asymmetry w/o PH (ppm)");
  grp1->GetYaxis()->CenterTitle();
  grp1->GetYaxis()->SetTitleOffset(0.7);
  grp1->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend1 = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend1->AddEntry(grp_in_1, "IHWP-IN w/o Ph", "p");
  legend1->AddEntry(grp_out_1, "IHWP-OUT w/o Ph", "p");
  legend1->AddEntry(grp_sum_1, "(IN+OUT)/2 w/o Ph", "p");
  legend1->SetFillColor(0);
  legend1->Draw("");

  TPaveStats *stats11 = (TPaveStats*)grp_in_1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats22 = (TPaveStats*)grp_out_1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats33 = (TPaveStats*)grp_sum_1->GetListOfFunctions()->FindObject("stats");
  stats11->SetTextColor(kBlue);
  stats11->SetFillColor(kWhite); 
  stats22->SetTextColor(kRed);
  stats22->SetFillColor(kWhite); 
  stats33->SetTextColor(kGreen-2);
  stats33->SetFillColor(kWhite); 
  stats11->SetX1NDC(0.8); stats11->SetX2NDC(0.99); 
  stats11->SetY1NDC(0.7);stats11->SetY2NDC(0.95);
  stats22->SetX1NDC(0.8); stats22->SetX2NDC(0.99); 
  stats22->SetY1NDC(0.4);stats22->SetY2NDC(0.65);
  stats33->SetX1NDC(0.8); stats22->SetX2NDC(0.99); 
  stats33->SetY1NDC(0.1);stats33->SetY2NDC(0.35);

  pad2->cd(3);

  // Add in-Out
  for(Int_t i=0;i<8;i++){
    valuediff[i]=((c_val_in[i]/pow(c_err_in[i],2)) - (c_val_out[i]/pow(c_err_out[i],2))) /((1.0/pow(c_err_in[i],2)) + (1.0/pow(c_err_out[i],2)));
    errordiff[i]= sqrt(1.0/((1.0/(pow(c_err_in[i],2)))+(1.0/pow(c_err_out[i],2))));
  }
  
  for(Int_t i=0;i<4;i++){
    c_opp_in[i]= c_val_in[i]+ c_val_in[i+4];
    c_opp_out[i]= c_val_out[i]+ c_val_out[i+4];
     c_opp_err_in[i]= sqrt(pow(c_err_in[i],2)+pow(c_err_in[i+4],2))/2;
     c_opp_err_out[i]= sqrt(pow(c_err_out[i],2)+pow(c_err_out[i+4],2))/2;
     x_opp[i]=i+1;
     errx_opp[i]=0.0;
}

 /*Draw IN values*/
  TGraphErrors * grp_in_opp  = new TGraphErrors(4,x_opp,c_opp_in,errx_opp,c_opp_err_in);
  grp_in_opp ->SetMarkerSize(0.8);
  grp_in_opp ->SetMarkerStyle(21);
  grp_in_opp ->SetMarkerColor(kBlue);
  grp_in_opp ->SetLineColor(kBlue);
  grp_in_opp->Fit("pol0","EM");
  //grp_in_opp->Draw("AP");

  TF1* fit111 = grp_in_opp->GetFunction("pol0");
  fit111->DrawCopy("same");
  fit111->SetLineColor(kBlue);

  TGraphErrors * grp_out_opp  = new TGraphErrors(4,x_opp,c_opp_out,errx_opp,c_opp_err_out);
  grp_out_opp ->SetMarkerSize(0.8);
  grp_out_opp ->SetMarkerStyle(21);
  grp_out_opp ->SetMarkerColor(kRed);
  grp_out_opp ->SetLineColor(kRed);
  grp_out_opp->Fit("pol0","EM");
  //grp_out_opp->Draw("AP");

  TF1* fit1111 = grp_out_opp->GetFunction("pol0");
  fit1111->DrawCopy("same");
  fit1111->SetLineColor(kRed);


  // Draw opposite bar sums
  // Add oppsoite bar sum
  for(Int_t i =0;i<4;i++){
    c_opp_valuesum[i]= valuediff[i]+ valuediff[i+4];
    c_opp_valueerror[i]= sqrt(pow(errordiff[i],2)+pow(errordiff[i+4],2))/2;
  }

  TGraphErrors* grp_sum_opp  = new TGraphErrors(4,x_opp,c_opp_valuesum,errx_opp,c_opp_valueerror);
  grp_sum_opp ->SetMarkerSize(0.8);
  grp_sum_opp ->SetMarkerStyle(21);
  grp_sum_opp ->SetMarkerColor(kMagenta-2);
  grp_sum_opp ->SetLineColor(kMagenta-2);

  grp_sum_opp->Fit("pol0");
  TF1* fit333 = grp_sum_opp->GetFunction("pol0");
  fit333->SetLineColor(kMagenta-2);
 
  TMultiGraph * grp11 = new TMultiGraph();
  //  grp11->Add(grp_in_opp);
  //  grp11->Add(grp_out_opp);
  grp11->Add(grp_sum_opp);
  grp11->Draw("AP");

  grp11->SetTitle("");
  grp11->GetXaxis()->SetTitle("Octant");
  grp11->GetXaxis()->CenterTitle();
  grp11->GetYaxis()->SetTitle("Asymmetry w/o PH (ppm)");
  grp11->GetYaxis()->CenterTitle();
  grp11->GetYaxis()->SetTitleOffset(0.7);
  grp11->GetXaxis()->SetTitleOffset(0.8);



//   TPaveStats *stats111 = (TPaveStats*)grp_in_opp->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *stats222 = (TPaveStats*)grp_out_opp->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats333 = (TPaveStats*)grp_sum_opp->GetListOfFunctions()->FindObject("stats");
//   stats111->SetTextColor(kBlue);
//   stats111->SetFillColor(kWhite); 
//   stats222->SetTextColor(kRed);
//   stats222->SetFillColor(kWhite); 
  stats333->SetTextColor(kMagenta-2);
  stats333->SetFillColor(kWhite); 
//   stats111->SetX1NDC(0.8); stats111->SetX2NDC(0.99); 
//   stats111->SetY1NDC(0.7);stats111->SetY2NDC(0.95);
//   stats222->SetX1NDC(0.8); stats222->SetX2NDC(0.99); 
//   stats222->SetY1NDC(0.4);stats222->SetY2NDC(0.65);
  stats333->SetX1NDC(0.8); //stats222->SetX2NDC(0.99); 
  stats333->SetY1NDC(0.1);stats333->SetY2NDC(0.35);

  TLegend *legend11 = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend11->AddEntry(grp_in_opp, "IHWP-IN", "p");
  legend11->AddEntry(grp_out_opp, "IHWP-OUT", "p");
  legend11->AddEntry(grp_sum_opp, "(IN-OUT)/2", "p");
  legend11->SetFillColor(0);
  legend11->Draw("");

  pad->Modified();
  pad->Update();
}
