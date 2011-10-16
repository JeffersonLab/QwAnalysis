/****************************************************************
Author: B. P. Waidyawansa
Data : September 6th 2011.

This script is used to track the changes in the transverse polarization phase(if there is any)
within the given slug range
Inputs (required) : Start slug, end slug (slug range) 
       (optional) : Turn on the drawing of individual fits by inserting 1.

e.g  ./slug_phase_tracker 42 51  
To print the individual plots for the slugs,
     ./slug_phase_tracker 42 51  1

Output : Plot of p1 fit parameter vs slug number for the fit of A = p0 sin(phi + p1) + p2
         *.txt file with the p0, p1 p3 values with their errors and the minimum chi-square
         for each slug.
         transverse_phase_in_slugs_*_*.png
         transverse_phase_in_slugs_*_*.txt

         When the plot slug option is selected, those plots will also be saved in 
         transverse_phase_in_slug_*_plot.png
         
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
#include <fstream>
#include <time.h>
#include <iomanip>

/*Main detector array (bar sums)*/
TString quartz_bar_sum[8]=
  {"qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum",
   "qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"};

/*Database connection*/
TSQLServer *db;

/*Other variables*/
Int_t slug_first = 0;
Int_t slug_last = 0;
Int_t end_slug = 0;
TVectorD x_i;
TVectorD xerr_i;
TVectorD x_o;
TVectorD xerr_o;
TVectorD p1_i;
TVectorD p1_ie;
TVectorD p1_o;
TVectorD p1_oe;
Double_t fit_value;
Double_t fit_error;
Double_t get_p0;
Double_t set_p0;
Double_t get_p1;
Double_t set_p1;
Double_t get_p2;
Double_t set_p2;
Double_t dummy=0;

/*Flag to turn on drawing of individual slug fit plots*/
Bool_t draw_slugs = false;
TCanvas * Canvas1;

/*Text file to store parameters*/
std::ofstream Myfile; 

/*Function defintions*/
TString data_query(TString device,Int_t slug,TString ihwp);
void fit_octants_data(TString devicelist[],Int_t slug, TString ihwp, 
		      Double_t set_p0, Double_t *get_p0, 
		      Double_t set_p1, Double_t *get_p1,
		      Double_t set_p2, Double_t *get_p2,
		      Double_t *fit_value,Double_t *fit_error);


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
  gStyle->SetStatH(0.2);

  
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
  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(0.8);
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

  /*Process command line arguments*/
  if(argc == 4){
    slug_first = atoi(argv[1]);
    slug_last = atoi(argv[2]);
    if(atoi(argv[3])== 1) draw_slugs = true;
    else {
      std::cout<<"Undefined option "<<argv[3]<<std::endl;
    }
  } else if(argc==3){
    slug_first = atoi(argv[1]);
    slug_last = atoi(argv[2]);
  } else {
    std::cout<<"The correct syntax is ./phase_tracker [firstslug(required)] [lastslug(required)] [draw_slugs(optional): 1 ]"<<std::endl;
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
  std::cout<<"given slug range have been analyzed in to the database. The script doesn't have checks for  "<<std::endl;
  std::cout<<"this. So it should not be interpreted that all the runs in the slug range was included in the given results."<<std::endl;


  
  /*connect to the data base*/
  //db = TSQLServer::Connect("mysql://localhost.jlab.org/qw_run1_pass3","qweak", "QweakQweak");
  // db = TSQLServer::Connect("mysql://127.0.0.1/qw_run1_pass3","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://qweakdb.jlab.org/qw_run1_pass3","qweak", "QweakQweak");


  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
 
  /*Create a text file to store the phase values and errors*/   
  Char_t  textfile[400];

  time_t theTime;
  time(&theTime);

  sprintf(textfile,"transverse_phase_in_slugs_%i_%i.txt",slug_first, slug_last); 
  Myfile.open(textfile);
  Myfile<<"##########################################################"<<std::endl;
  Myfile<<"#   Qweak Residual Transverse Polarization Phase Tracker "<<std::endl;
  Myfile<<"#   Created by: Buddhini Waidyawansa                       "<<std::endl;
  Myfile<<"#   contact : buddhini@jlab.org                            "<<std::endl;
  Myfile<<"##########################################################"<<std::endl;
  Myfile <<"# date of analysis ="<<ctime(&theTime)<<std::endl;
  Myfile <<"# slug range "<<slug_first<<" to "<<slug_last<<std::endl;
  Myfile <<"# Following are the phase results from fit to runlet average regressed MD asymmetry vs octant "<<std::endl;
  Myfile <<"# Fit : A = P1*cos(phi+p1) + p2"<<std::endl;
  Myfile <<"#  "<<std::endl;


 
 


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


  /*Get data from database for the given slug range, fit and extract the fit parameters*/

  Myfile <<"# ihwp\t\t  slug \t\t"<<setw(8)<<" p0(ppm)+-error(ppm) \t\t p1(degree)+-p1error(degree)  \t chisqure min "<<std::endl;
 
  for(Int_t i=slug_first;i<slug_last+1;i++){
    fit_octants_data(quartz_bar_sum,i,"in",set_p0, &get_p0,set_p1, &get_p1,set_p2, &get_p2,
		     &fit_value,&fit_error);
    
    if(fit_value == 0 && fit_error == 0) {
      /*do nothing. The slug number is not in the selected list*/
    } else {
      x_i.ResizeTo(ki+1);    x_i.operator()(ki)=i;
      xerr_i.ResizeTo(ki+1); xerr_i.operator()(ki)=0.0;
      p1_i.ResizeTo(ki+1);   p1_i.operator()(ki)=fit_value;
      p1_ie.ResizeTo(ki+1);  p1_ie.operator()(ki)=fit_error;
      ki++;
    }
    fit_value = 0.0;
    fit_error = 0.0;
    fit_octants_data(quartz_bar_sum,i,"out",get_p0,&dummy,get_p1,&dummy,get_p2,&dummy,
		     &fit_value,&fit_error);

    if(fit_value == 0 && fit_error == 0) {
      /*do nothing. The slug number is not in the selected list*/
    } else {
      x_o.ResizeTo(ko+1);    x_o.operator()(ko)=i;
      xerr_o.ResizeTo(ko+1); xerr_o.operator()(ko)=0.0;
      p1_o.ResizeTo(ko+1);   p1_o.operator()(ko)=fit_value;
      p1_oe.ResizeTo(ko+1);  p1_oe.operator()(ko)=fit_error;
      ko++;
    }
  }
 
  if(ko==0 && ki==0) {std::cout<<"No data!"<<std::endl;
    TString text2 = "Unable to find regressed values for the given slug range.";
    TText*t2 = new TText(0.06,0.5,text2);
    t2->SetTextSize(0.05);
    t2->Draw();
  }
  else{
    
    /*Draw p1 values*/
 /*Create a canvas*/
    TString title = Form("Variation of phase offset within slugs %i to %i in fit p0*-4.75*Cos(phi+p1)+p2",
			 slug_first,slug_last);
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
    t1->SetTextSize(0.7);
    t1->Draw();
    
    pad2->cd();

    TMultiGraph * grp = new TMultiGraph();
    TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
    TGraphErrors* grp_p1out = NULL;
    TGraphErrors* grp_p1in = NULL;
    TPaveStats *stats1;
    TPaveStats *stats2;
    
    if(ko!=0){
      grp_p1out  = new TGraphErrors(x_o,p1_o,xerr_o,p1_oe);
      grp_p1out ->SetMarkerSize(0.8);
      grp_p1out ->SetMarkerStyle(21);
      grp_p1out ->SetMarkerColor(kRed);
      grp_p1out ->SetLineColor(kRed);
      grp_p1out ->Fit("pol0");
      TF1* fit1 = grp_p1out->GetFunction("pol0");
      fit1->DrawCopy("same");
      fit1->SetLineColor(kRed);
      fit1->SetLineStyle(2);
      grp->Add(grp_p1out);
      legend->AddEntry(grp_p1out, "IHWP-OUT", "p");
    }

    if(ki!=0){
      grp_p1in  = new TGraphErrors(x_i,p1_i,xerr_i,p1_ie);
      grp_p1in ->SetMarkerSize(0.8);
      grp_p1in ->SetMarkerStyle(21);
      grp_p1in ->SetMarkerColor(kBlue);
      grp_p1in ->SetLineColor(kBlue);
      grp_p1in ->Fit("pol0");    
      TF1* fit2 = grp_p1in->GetFunction("pol0");
      fit2->DrawCopy("same");
      fit2->SetLineColor(kBlue);
      fit2->SetLineStyle(2);
      grp->Add(grp_p1in);
      legend->AddEntry(grp_p1in, "IHWP-IN", "p");
    }

    if(ko!=0 or ki!=0) {

      grp->Draw("AP");
      grp->GetXaxis()->SetTitle("slug number");
      grp->GetYaxis()->SetTitle("Phase (degrees)");
      legend->SetFillColor(0);
      legend->Draw();
     }

    if(ki!=0){
      stats1 = (TPaveStats*)grp_p1in->GetListOfFunctions()->FindObject("stats");
      stats1->SetTextColor(kBlue);
      stats1->SetFillColor(kWhite); 
      stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); 
      stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
    }

    if(ko!=0){
      stats2 = (TPaveStats*)grp_p1out->GetListOfFunctions()->FindObject("stats");
      stats2->SetTextColor(kRed);
      stats2->SetFillColor(kWhite); 
      stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
      stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
    }
    Canvas->Modified();
    Canvas->Update();
    Canvas->Print(Form("transverse_phase_in_slugs_%i_%i.png",slug_first,slug_last));
    Myfile.close();

  }
  std::cout<<"Done! \n";
  db->Close();
  delete db;
  theApp.Run();
  return(1);
}


/*A function to create the mysql query to grab the runlet average of a md detector*/
TString data_query(TString device,Int_t slug,TString ihwp){

  Bool_t ldebug = false;

  if(ldebug) std::cout<<"Getting regressed data for "<<device<<std::endl;
  TString datatable = "md_data_view";
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
 
  TString slug_cut   = Form("%s.slug = %i",
			   datatable.Data(),slug);

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
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}


/*A function to get data from the database for the octants*/
void fit_octants_data(TString devicelist[],Int_t slug, TString ihwp, 
		      Double_t set_p0, Double_t *get_p0, 
		      Double_t set_p1, Double_t *get_p1,
		      Double_t set_p2, Double_t *get_p2,
		      Double_t *fit_value,Double_t *fit_error){
  
  Bool_t ldebug = false;
  Bool_t empty  = false;
  TString query;
  TSQLStatement* stmt = NULL;
  Double_t value[8];
  Double_t error[8];
  Double_t x[8];
  Double_t xerr[8];

 /*set X location and clear the other arrays*/
  for(Int_t i =0;i<8;i++){
    x[i]         = i+1;
    xerr[i]      = 0.0;
    value[i]     = 0.0;
    error[i]     = 0.0;
  }
  std::cout<<"Extracting average asymmetries from slug "<<slug<<std::endl;
  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) printf("Getting data for %10s ", devicelist[i].Data());
    
    query = data_query(Form("%s",devicelist[i].Data()),slug,ihwp);
    
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
	if(ldebug) printf(" value = %3.3f +- %3.3f [ppm]\n", value[i], error[i]);
	if((error[i]==0) or (stmt->IsNull(0))) {
	  empty = true;
	  break;
	}
      }
    }
    delete stmt;    
  }

  /* Now fit the results if they are not empty */
  /* Define the cosine fit. Use the p0, p1 and p2 for initialization and limits of this fit.
     We dont't want the phase to change with the IHWP. The sign of the amplitude is the onlything
     that should change with IHWP. But we will allow a +-90 degree leverage for the phase to see 
     if fo some reason, changing the IHWP has changed the verticle to horizontal transverse
     or vise versa.
  */
  Double_t phase_up  = 90;
  Double_t phase_low =-90;

  TF1 *cosfit = new TF1("cosfit","[0]*-4.75*cos((pi/180)*(45*(x-1)+[1])) +[2]",1,8);
  cosfit->SetParLimits(1,-90,90);


  /*Without loss of generality, I picked IHWP IN as the first fit. So then I need to
    use the results from that case to initialize IHWP OUT case. Since I use the same function 
    in both cases, I have to use and if statement to check the ihwp state.*/
  if(ihwp == "out"){
    phase_low = set_p1 - 90;
    phase_up  = set_p1 + 90;
    cosfit->SetParLimits(1,phase_low,phase_up);
    cosfit->SetParameter(0,set_p0);
    cosfit->SetParameter(1,-set_p1);
    cosfit->SetParameter(2,set_p2);
  } 

 /*To make sure the sign of the amplitude flips from IN to OUT*/
  /*I chose to make IN + and OUT -*/
  if(ihwp == "in"){
    cosfit->SetParLimits(0,0,99999);
  } 

  if(ihwp == "out"){
    cosfit->SetParLimits(0,-99999,0);
  } 

  if(!empty){
    if(ldebug) std::cout<<"Fit data"<<std::endl;

    if(draw_slugs){
      TString title = Form("LH2: Regressed slug averages of Main detector asymmetries from slugs %i : Fit p0*-4.75*Cos(phi+p1)+p2",slug_first);
      Canvas1 = new TCanvas("canvas1", title,0,0,800,500);
      Canvas1->SetFillColor(kWhite);
    }
    TGraphErrors* grp1  = new TGraphErrors(8,x,value,xerr,error);
    if(draw_slugs) {
      grp1->Draw("AP");
      grp1->SetMarkerSize(0.8);
      grp1 ->SetMarkerStyle(21);
      gPad->SetGridy();
      gPad->SetGridx();
    }
    grp1->SetTitle(Form("%s Slug %i",ihwp.Data(),slug));
    /*Not a mistake. Based on experiance,sometimes need to fit couple of times to get the best fit. */
    grp1->Fit("cosfit","QVLEM");
    grp1->Fit("cosfit","QVLEM");


    TF1* fit1 = grp1->GetFunction("cosfit");
    if(fit1==NULL){
      std::cout<<"Empty fit."<<std::endl;
    } else {

      /*Get the fit parameters from the IHWP IN fit to initialize IHWP OUT case*/
      if(ihwp == "in") {
	*get_p0 = fit1->GetParameter(0);
	*get_p2 = fit1->GetParameter(1);
	*get_p1 = fit1->GetParameter(2);
      }
 
      *fit_value = fit1->GetParameter(1); //p1
      *fit_error = fit1->GetParError(1); //p1 error
      if(ldebug) {
	std::cout<<"p1 =" <<*fit_value<< "+-" <<*fit_error << std::endl;
      }

      Double_t min_chisquare = (fit1->GetChisquare())/(fit1->GetNDF());
      Myfile <<ihwp<<"\t\t"<<  slug  <<"\t\t "
	     <<setw(8)<<setiosflags(ios::fixed)<<setprecision(3)<<fit1->GetParameter(0) <<" +- " 
	     <<setw(8)<<setiosflags(ios::fixed)<<setprecision(3)<<fit1->GetParError(0)  <<"\t\t " 
	     <<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<fit1->GetParameter(1) <<" +- " 
	     <<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<fit1->GetParError(1)  <<"\t\t\t " 
	     <<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<min_chisquare<<std::endl;


    }

    if(draw_slugs){
      Canvas1->Update();
      Canvas1->Print(Form("transverse_phase_in_slug_%i_plot.png",slug));
    }
    delete fit1;
    delete grp1;
    delete cosfit;
  }
  if(empty){
    std::cout<<"Found 0 values in detectors. Ignore this slug "<<slug<<std::endl;
    *fit_value = 0.0;
    *fit_error = 0.0;    
  }

}

