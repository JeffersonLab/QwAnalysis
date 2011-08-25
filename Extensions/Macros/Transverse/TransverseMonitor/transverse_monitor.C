/****************************************************************
Author: B. P. Waidyawansa
Data : August 22nd 2011.

This script should be used to perfrom weekly monitoring of the amount of residula transverse polarization left in the longitudinally polarized Qweak electron beam. It access the unregressed data from the qweak data base and performs a fit over the main detector octants to calculat the ratio of 

A_T_measured_during_longitudinal/A_T_transverse = amount of transevrse polarization in the beam.

and can be then reported to source group for corrections.

****************************************************************/

using namespace std;

#include<TString.h>
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

/*Main detector array (bar sums)*/
TString quartz_bar_sum[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum",
   "qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

/*Database connection*/
TSQLServer *db;

/*Other variables*/
Int_t slug_num = 0;
Double_t valuesin[8];
Double_t errorsin[8];
Double_t valuesout[8];
Double_t errorsout[8];
Double_t valuesum[8];
Double_t valueerror[8];
Double_t valuediff[8];
Double_t errordiff[8];
Double_t x[8];
Double_t errx[8];
Int_t slug_first = 0;

/*Function defintions*/
TString get_query(TString device,TString ihwp, Int_t slug_last);
void get_octant_data(TString devicelist[],TString ihwp,Int_t slug_last,
		     Double_t value[],Double_t error[]);
//void plot_octant(Int_t slug_last);

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

  std::cout<<"####################################################"<<std::endl;
  std::cout<<" \n Qweak Residual Transverse Polarization Monitor\n"<<std::endl;
  std::cout<<"   Created by: Buddhini Waidyawansa                "<<std::endl;
  std::cout<<"   contact : buddhini@jlab.org                     "<<std::endl;
  std::cout<<"####################################################"<<std::endl;
  std::cout<<"NOTE: "<<std::endl;
  std::cout<<"This macro looks only at LH2 longitudinal data that are marked as parity+production and  "<<std::endl;
  std::cout<<"suspect+good or good only.It is the users responsibilty to make sure all the runs in the  "<<std::endl;
  std::cout<<"given slug range have been analyzed in to the database. The script doesn't have checks for  "<<std::endl;
  std::cout<<"this. So it should not be interpreted that all the runs in the slug range was included in the  \n"<<std::endl;
  std::cout<<"given results.  \n"<<std::endl;

  std::cout<<" Please enter the last completed slug number:"<<std::endl;
  std::cin>>slug_num;
  slug_first =  slug_num-9;

  /*connect to the data base*/
  //  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_run1_pass3","qweak", "QweakQweak");

  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
  
  /*Create a canvas*/
  TString title = Form("LH2: Regressed slug averages of Main detector asymmetries from slugs %i to %i: Fit p0*-4.75*Cos(phi+p1)+p2",slug_first,slug_num);
  TCanvas * Canvas = new TCanvas("canvas", title,0,0,1000,500);
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
  t1->SetTextSize(0.5);
  t1->Draw();

  pad2->cd();

 /*set X location and clear the other arrays*/
  for(Int_t i =0;i<8;i++){
    x[i]         = i+1;
    errx[i]      = 0.0;
    valuesin[i]  = 0.0;
    valuesout[i] = 0.0;
    errorsin[i]  = 0.0;
    errorsout[i] = 0.0;
    valuesum[i]  = 0.0;
    valueerror[i]= 0.0;
    valuediff[i] = 0.0;
    errordiff[i] = 0.0;
  }
  
  /*Get data from database*/
  std::cout<<"IHWP 'in'.."<<std::endl;
  get_octant_data(quartz_bar_sum,"in",slug_num,valuesin,errorsin);
  std::cout<<"IHWP 'out'.."<<std::endl;
  get_octant_data(quartz_bar_sum,"out",slug_num,valuesout,errorsout);



  // Define the cosine fit
  TF1 *cosfit = new TF1("cosfit","[0]*-4.75*cos((pi/180)*(45*(x-1)+[1])) +[2]",1,8);
  cosfit->SetParameter(0,0);
  cosfit->SetParameter(1,0);
  cosfit->SetParameter(2,0);
  cosfit->SetParLimits(1,0,180);
  cosfit->SetParLimits(3,0,180);

  /*Draw IN values*/
  TGraphErrors* grp_in  = new TGraphErrors(8,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("cosfit");
  TF1* fit1 = grp_in->GetFunction("cosfit");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  /*Draw OUT values*/
  TGraphErrors* grp_out  = new TGraphErrors(8,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out->Fit("cosfit");
  TF1* fit2 = grp_out->GetFunction("cosfit");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);

  /* Draw IN+OUT values */
  for(Int_t i =0;i<8;i++){
    valuesum[i]=valuesin[i]+valuesout[i];
    valueerror[i]= sqrt(pow(errorsin[i],2)+pow(errorsout[i],2));
  }

  TGraphErrors* grp_sum  = new TGraphErrors(8,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.6);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum->Fit("cosfit");
  TF1* fit3 = grp_sum->GetFunction("cosfit");
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
  
  Canvas->Update();
  Canvas->Print(Form("transverse_monitor_slugs_%i_%i_plots.png",slug_first,slug_num));

  std::cout<<"Done! \n";
  db->Close();
  delete db;
  theApp.Run();
  return(1);
}


/*A function to create the mysql query*/
TString get_query(TString device,TString ihwp, Int_t slug_last){

  Bool_t ldebug = false;

  std::cout<<"Getting unregressed data for "<<device<<std::endl;
  TString datatable = "md_data_view";
  Int_t slug_first = slug_last-9; // select the last 9 slugs including slug_last 
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
  TString slug_cut   = Form("(%s.slug >= %i and %s.slug <= %i)",
			  datatable.Data(),slug_first,datatable.Data(),slug_last);

  TString run_quality =  Form("(%s.run_quality_id = '1' or %s.run_quality_id = '1,3')",
			   datatable.Data(),datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on' ",
			    datatable.Data(),datatable.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", run, slow_controls_settings WHERE "
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
void get_octant_data(TString devicelist[],TString ihwp,Int_t slug_last,
		     Double_t value[], Double_t error[]){
  
  Bool_t ldebug = false;

  TString query;
  TSQLStatement* stmt = NULL;
  
  std::cout<<"Extracting average asymmetries from slug "<<(slug_last-9)<<" to "<<slug_last<<std::endl;
  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) printf("Getting data for %10s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    
    query = get_query(Form("%s",devicelist[i].Data()),ihwp,slug_last);
    
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


