/****************************************************************
Author: B. P. Waidyawansa
Data : September 20th 2011.

This script is used to track the changes in the transverse polarization X, Y components
within the given slug range.
./slug_transverse_tracker <start>  <end> <db (optional)>  <print (optional)>

Inputs (required) : Start slug, end slug (slug range) 
       (optional) : Data base. Default is qw_run1_pass4b
       (optional) : Turn on the drawing of individual fits by inserting 1.

e.g  ./slug_transverse_tracker 42 51  
To print the individual plots for the slugs,
     ./slug_transevrse_tracker 42 51  qw_run1_pass2

Output : Plot of % horizontal transerverse (p1*100)  vs slug number and plot of % vertical transerverse (p0*100)  vs slug number 
         for the fit of A = -4.80{ p0 cos(phi) - p1 sin(phi)} + p2.from pass4
         and
         *.txt file with the p0, p1  values with their errors and the minimum chi-square
         for each slug.
         *_transverse_in_slugs_*_*.png
         *_transverse_in_slugs_*_*.txt

         When the plot slug option is selected, those plots will also be saved in 
         *_transverse_in_slug_*_plot.png
         
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
#include <TLatex.h>

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
TString database = "qw_run1_pass4b";

TVectorD x_i;
TVectorD xerr_i;
TVectorD x_o;
TVectorD xerr_o;
TVectorD p0_i;
TVectorD p0_ie;
TVectorD p0_o;
TVectorD p0_oe;
TVectorD p1_i;
TVectorD p1_ie;
TVectorD p1_o;
TVectorD p1_oe;
TVectorD p1s;
TVectorD p1s_e;
TVectorD p0s;
TVectorD p0s_e;
TVectorD x1s;
TVectorD x1s_e;
TVectorD x0s;
TVectorD x0s_e;

Double_t fit_value0;
Double_t fit_error0;
Double_t fit_value1;
Double_t fit_error1;
Double_t get_p0;
Double_t set_p0;
Double_t get_p1;
Double_t set_p1;
Double_t get_p2;
Double_t set_p2;
Double_t dummy=0;

Int_t ki =0;
Int_t ko =0;
Int_t k0s =0;
Int_t k1s =0;



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
		      Double_t *fit_value0,Double_t *fit_error0,
		      Double_t *fit_value1,Double_t *fit_error1);

 void draw_plots(TVectorD value_in, TVectorD value_out,
		 TVectorD error_in, TVectorD error_out, 
		 TVectorD valuex_in, TVectorD errorx_in, 
		 TVectorD valuex_out, TVectorD errorx_out,
		 TVectorD value_suspect, TVectorD error_suspect,  
		 TVectorD x_suspect, TVectorD x_error_suspect,  
		 Int_t ko, Int_t ki,  Int_t ks, TString type);


 void draw_plots(TString type);

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
  gStyle->SetTitleXOffset(1.2);
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
  if(argc == 5){
    slug_first = atoi(argv[1]);
    slug_last = atoi(argv[2]);
    database = argv[3];
    if(atoi(argv[4])== 1) draw_slugs = true;
    else {
      std::cout<<"Undefined option "<<argv[4]<<std::endl;
    }
  } else if(argc==4){
    slug_first = atoi(argv[1]);
    slug_last = atoi(argv[2]);
    database = argv[3];
  } else if(argc==3){
    slug_first = atoi(argv[1]);
    slug_last = atoi(argv[2]);
  } else {
    std::cout<<"The correct syntax is ./slug_transverse_tracker [firstslug(required)] [lastslug(required)] [database(optional)] [draw_slugs(optional): 1 ]"<<std::endl;
    exit(1);
  }

  std::cout<<"##########################################################"<<std::endl;
  std::cout<<" \n Qweak Residual Transverse Polarization Tracker\n"<<std::endl;
  std::cout<<"   Created by: Buddhini Waidyawansa                       "<<std::endl;
  std::cout<<"   contact : buddhini@jlab.org                            "<<std::endl;
  std::cout<<"##########################################################"<<std::endl;
  std::cout<<"NOTE: "<<std::endl;
  std::cout<<"This macro looks only at LH2 longitudinal data that are marked as parity+production and  "<<std::endl;
  std::cout<<"run quality good only.It is the users responsibilty to make sure all the runs in the  "<<std::endl;
  std::cout<<"given slug range have been analyzed in to the database. The script doesn't have checks for  "<<std::endl;
  std::cout<<"this. So it should not be interpreted that all the runs in the slug range was included in the given results."<<std::endl;


  
  /*connect to the data base*/
  db = TSQLServer::Connect(Form("mysql://127.0.0.1/%s",database.Data()),"qweak", "QweakQweak");


  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
 
  /*Create a text file to store the phase values and errors*/   
  Char_t  textfile[400];

  time_t theTime;
  time(&theTime);

  sprintf(textfile,"transverse_in_slugs_%i_%i.txt",slug_first, slug_last); 
  Myfile.open(textfile);
  Myfile<<"##########################################################"<<std::endl;
  Myfile<<"#   Qweak Residual Transverse Polarization Tracker "<<std::endl;
  Myfile<<"#   Created by: Buddhini Waidyawansa                       "<<std::endl;
  Myfile<<"#   contact : buddhini@jlab.org                            "<<std::endl;
  Myfile<<"##########################################################"<<std::endl;
  Myfile <<"# date of analysis ="<<ctime(&theTime)<<std::endl;
  Myfile <<"# slug range "<<slug_first<<" to "<<slug_last<<std::endl;
  Myfile <<"# Following are the phase results from fit to runlet average regressed MD asymmetry vs octant "<<std::endl;
  Myfile <<"# Fit : A = -4.80*{ P_V*cos(phi) + P_H*sin(phi) }+ p2"<<std::endl;
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
  p0_i.Clear();
  p0_ie.ResizeTo(0);
  p0_o.Clear();
  p0_oe.ResizeTo(0);
  p1_i.Clear();
  p1_ie.ResizeTo(0);
  p1_o.Clear();
  p1_oe.ResizeTo(0);

  p1s.Clear();
  p1s.ResizeTo(0);
  p1s_e.Clear();
  p1s_e.ResizeTo(0);
  p0s.Clear();
  p0s.ResizeTo(0);
  p0s_e.Clear();
  p0s_e.ResizeTo(0);
  x1s.Clear();
  x1s.ResizeTo(0);
  x1s_e.Clear();
  x1s_e.ResizeTo(0);
  x0s.Clear();
  x0s.ResizeTo(0);
  x0s_e.Clear();
  x0s_e.ResizeTo(0);




  /*Get data from database for the given slug range, fit and extract the fit parameters*/

  Myfile <<"# ihwp\t\t  slug \t\t"<<setw(8)<<" P_V+-error \t\t P_H+-p1error  \t chisqure min "<<std::endl;
 
  for(Int_t i=slug_first;i<slug_last+1;i++){

    /*Fit data for IHWP  IN state*/
    fit_octants_data(quartz_bar_sum,i,"in",set_p0, &get_p0,set_p1, &get_p1,set_p2, &get_p2,
		     &fit_value0,&fit_error0,&fit_value1,&fit_error1);
    
    if(fit_value0 == 0 && fit_error0 == 0 && fit_value1 == 0 && fit_error1 == 0) {
      /*do nothing. The slug number is not in the selected list*/
    } 
    else if(fit_error0 <=0.08 && fit_error1 <=0.08) // to see a 5% transverse polarization with a 95% CL we need to have an error of at least 0.05/3 = 0.016 from the fit.
      {
	x_i.ResizeTo(ki+1);    x_i.operator()(ki)=i;
	xerr_i.ResizeTo(ki+1); xerr_i.operator()(ki)=0.0;
	p0_i.ResizeTo(ki+1);   p0_i.operator()(ki)=fit_value0*100;
	p0_ie.ResizeTo(ki+1);  p0_ie.operator()(ki)=fit_error0*100;
	p1_i.ResizeTo(ki+1);   p1_i.operator()(ki)=fit_value1*100;
	p1_ie.ResizeTo(ki+1);  p1_ie.operator()(ki)=fit_error1*100;
	ki++;
      }
    else{
      if (fit_error0 > 0.08)
	{
	  x0s.ResizeTo(k0s+1);   x0s.operator()(k0s)=i;
	  x0s_e.ResizeTo(k0s+1);  x0s_e.operator()(k0s)=0.0;
	  p0s.ResizeTo(k0s+1);   p0s.operator()(k0s)=fit_value0*100;
	  p0s_e.ResizeTo(k0s+1);  p0s_e.operator()(k0s)=fit_error0*100;
	  k0s++;
	}
      if(fit_error1 > 0.08)
	{
	  x1s.ResizeTo(k1s+1);   x1s.operator()(k1s)=i;
	  x1s_e.ResizeTo(k1s+1);  x1s_e.operator()(k1s)=0.0;
	      p1s.ResizeTo(k1s+1);   p1s.operator()(k1s)=fit_value1*100;
	      p1s_e.ResizeTo(k1s+1);  p1s_e.operator()(k1s)=fit_error1*100;
	      k1s++;		
	}
    }
    

    
    fit_value0 = 0.0;
    fit_error0 = 0.0;
    fit_value1 = 0.0;
    fit_error1 = 0.0;
    
    fit_octants_data(quartz_bar_sum,i,"out",get_p0,&dummy,get_p1,&dummy,get_p2,&dummy,
		     &fit_value0,&fit_error0,&fit_value1,&fit_error1);

    if(fit_value0 == 0 && fit_error0 == 0 && fit_value1 == 0 && fit_error1 == 0) {
      /*do nothing. The slug number is not in the selected list*/
    } 
    else if(fit_error0 <=0.08 or fit_error1 <=0.08)
      {
	x_o.ResizeTo(ko+1);    x_o.operator()(ko)=i;
	xerr_o.ResizeTo(ko+1); xerr_o.operator()(ko)=0.0;
	p0_o.ResizeTo(ko+1);   p0_o.operator()(ko)=fit_value0*100;
	p0_oe.ResizeTo(ko+1);  p0_oe.operator()(ko)=fit_error0*100;
	p1_o.ResizeTo(ko+1);   p1_o.operator()(ko)=fit_value1*100;
	p1_oe.ResizeTo(ko+1);  p1_oe.operator()(ko)=fit_error1*100;
	ko++;
      }
    else {
      if (fit_error0 > 0.08)
	{
	  x0s.ResizeTo(k0s+1);   x0s.operator()(k0s)=i;
	  x0s_e.ResizeTo(k0s+1);  x0s_e.operator()(k0s)=0.0;
	  p0s.ResizeTo(k0s+1);   p0s.operator()(k0s)=fit_value0*100;
	  p0s_e.ResizeTo(k0s+1);  p0s_e.operator()(k0s)=fit_error0*100;
	  k0s++;
	}
      if(fit_error1 > 0.08)
	{
	  x1s.ResizeTo(k1s+1);   x1s.operator()(k1s)=i;
	  x1s_e.ResizeTo(k1s+1);  x1s_e.operator()(k1s)=0.0;
	  p1s.ResizeTo(k1s+1);   p1s.operator()(k1s)=fit_value1*100;
	  p1s_e.ResizeTo(k1s+1);  p1s_e.operator()(k1s)=fit_error1*100;
	  k1s++;		
	}
    }
    
    
  }
 
  if(ko==0 && ki==0 && k1s==0 && k0s == 0) {std::cout<<"No data!"<<std::endl;
    TString text2 = "Unable to find regressed values for the given slug range.";
    TText*t2 = new TText(0.06,0.5,text2);
    t2->SetTextSize(0.05);
    t2->Draw();
  }
  else{
    
    /*Draw p1 values*/
    /*Create a canvas*/
    TString fit = "-4.80*( P_{V}*cos#phi -  P_{H}*sin#phi ) + constant";
    TString title = Form("Variation of transverse polarization in slugs %i to %i from fit %s",
			 slug_first,slug_last,fit.Data());
    TCanvas * Canvas = new TCanvas("canvas", title,0,0,1000,800);
    Canvas->Draw();
    Canvas->cd();
    
    TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
    TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
    pad1->SetFillColor(20);
    pad1->Draw();
    pad2->Draw();
    
    
    pad1->cd();
    TLatex*t1 = new TLatex(0.1,0.5,title);
    t1->SetTextSize(0.4);
    t1->Draw();
    
    pad2->cd();
    pad2->Divide(1,2);

    pad2->cd(1);
    draw_plots(p0_i, p0_o, p0_ie, p0_oe, x_i, xerr_i,x_o, xerr_o,p0s, p0s_e, x0s, x0s_e,ko,ki,k0s,"Y");

    pad2->cd(2);
    draw_plots(p1_i, p1_o,p1_ie, p1_oe, x_i, xerr_i,x_o, xerr_o,p1s, p1s_e,x1s, x1s_e,ko,ki,k1s,"X");

    Canvas->Modified();
    Canvas->Update();
    Canvas->Print(Form("%s_transverse_in_slugs_%i_%i.png",database.Data(),slug_first,slug_last));
    Canvas->Print(Form("%s_transverse_in_slugs_%i_%i.C",database.Data(),slug_first,slug_last));
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

  Bool_t ldebug = true;

  if(ldebug) std::cout<<"Getting regressed data for "<<device<<std::endl;
  TString datatable = "md_data_view";
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2))))),"
    +datatable+".error* SQRT("+datatable+".n) ";
 
  TString slug_cut   = Form("(%s.slug = %i ) ",
			    datatable.Data(),slug);

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on_5+1' ",
			    datatable.Data(),datatable.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+" , slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error* SQRT("+datatable+".n) < 0.000800 AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}


/*A function to get data from the database for the octants*/
void fit_octants_data(TString devicelist[],Int_t slug, TString ihwp, 
		      Double_t set_p0, Double_t *get_p0, 
		      Double_t set_p1, Double_t *get_p1,
		      Double_t set_p2, Double_t *get_p2,
		      Double_t *fit_value0,Double_t *fit_error0,
		      Double_t *fit_value1,Double_t *fit_error1){
  
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
  TF1 * cosfit = new TF1("cosfit","(-4.80)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);


  /*Without loss of generality, I picked IHWP IN as the first fit. So then I need to
    use the results from that case to initialize IHWP OUT case. Since I use the same function 
    in both cases, I have to use and if statement to check the ihwp state.*/
  if(ihwp == "out"){
    cosfit->SetParameter(2,-set_p2);
    cosfit->SetParameter(0,set_p0);
    cosfit->SetParameter(1,set_p1);
  } 


  if(!empty){
    if(ldebug) std::cout<<"Fit data"<<std::endl;

    /*To draw individual slug plots*/
    if(draw_slugs){
      TString fit = "-4.80*( P_{V}*cos#phi -  P_{H}*sin#phi ) + constant";
      TString title = Form("Variation of transverse polarization in slug %i. Fit %s",
			   slug_first,fit.Data());
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
 
      *fit_value0 = fit1->GetParameter(0); //p0
      *fit_error0 = fit1->GetParError(0); //p0 error
      *fit_value1 = fit1->GetParameter(1); //p1
      *fit_error1 = fit1->GetParError(1); //p1 error

      if(ldebug) {
	std::cout<<"p0 =" <<*fit_value0<< "+-" <<*fit_error0
		 <<" p1 =" <<*fit_value1<< "+-" <<*fit_error1 << std::endl;
      }

      Double_t min_chisquare = (fit1->GetChisquare())/(fit1->GetNDF());
      Myfile <<ihwp<<"\t\t"<<  slug  <<"\t\t "
	     <<setw(8)<<setiosflags(ios::fixed)<<setprecision(3)<<fit1->GetParameter(0) <<" +- " 
	     <<setw(8)<<setiosflags(ios::fixed)<<setprecision(3)<<fit1->GetParError(0)  <<"\t\t " 
	     <<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<fit1->GetParameter(1) <<" +- " 
	     <<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<fit1->GetParError(1)  <<"\t\t\t " 
	     <<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<min_chisquare<<std::endl;


    }

    /*To draw individual slug plots*/
    if(draw_slugs){
      Canvas1->Update();
      Canvas1->Print(Form("%s_transverse_in_slug_%i_plot.png",database.Data(),slug));
    }
    delete fit1;
    delete grp1;
    delete cosfit;
  }
  if(empty){
    std::cout<<"Found 0 values in detectors. Ignore this slug "<<slug<<std::endl;
    *fit_value0 = 0.0;
    *fit_error0 = 0.0;   
    *fit_value1 = 0.0;
    *fit_error1 = 0.0;     
  }

}


 void draw_plots(TVectorD value_in, TVectorD value_out,
		 TVectorD error_in, TVectorD error_out, 
		 TVectorD valuex_in, TVectorD errorx_in, 
		 TVectorD valuex_out, TVectorD errorx_out,
		 TVectorD value_suspect, TVectorD error_suspect,  
		 TVectorD x_suspect, TVectorD x_error_suspect,  
		 Int_t ko, Int_t ki,  Int_t ks, TString type){

     

   TString title;
   TString ytitle;

   if(type == "X") {
     title = "P_{H}";
     ytitle = "P_{H} (%)";
   }
   if(type == "Y") {
     title = "P_{V}";
     ytitle = "P_{V} (%)";
   }

   
   TMultiGraph * grp = new TMultiGraph();
   TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
   TGraphErrors* grp_out = NULL;
   TGraphErrors* grp_in = NULL;
   TGraphErrors* grp_suspect = NULL;

   TPaveStats *stats1;
   TPaveStats *stats2;
   

   if(ko!=0){
     grp_out  = new TGraphErrors(valuex_out,value_out,errorx_out,error_out);
     grp_out ->SetMarkerSize(0.8);
     grp_out ->SetMarkerStyle(21);
     grp_out ->SetMarkerColor(kRed);
     grp_out ->SetLineColor(kRed);
     grp_out ->Fit("pol0");
     TF1* fit1 = grp_out->GetFunction("pol0");
     fit1->SetParNames(title);
      if(fit1==NULL) {
	std::cout<<"Fit is empty for OUT data"<<std::endl;
	exit(1);
      }
      fit1->DrawCopy("same");
      fit1->SetLineColor(kRed);
      fit1->SetLineStyle(2);
      grp->Add(grp_out);
      legend->AddEntry(grp_out, "IHWP-OUT", "p");
    

    if(ki!=0){
      grp_in  = new TGraphErrors(valuex_in,value_in,errorx_in,error_in);
      grp_in ->SetMarkerSize(0.8);
      grp_in ->SetMarkerStyle(21);
      grp_in ->SetMarkerColor(kBlue);
      grp_in ->SetLineColor(kBlue);
      grp_in ->Fit("pol0");    
      TF1* fit2 = grp_in->GetFunction("pol0");
      fit2->SetParNames(title);

      if(fit2==NULL) {
	std::cout<<"Fit is empty for IN data"<<std::endl;
	exit(1);
      }
      fit2->DrawCopy("same");
      fit2->SetLineColor(kBlue);
      fit2->SetLineStyle(2);
      grp->Add(grp_in);
      legend->AddEntry(grp_in, "IHWP-IN", "p");
    }

    if(ks!=0){
      grp_suspect  = new TGraphErrors(x_suspect,value_suspect,x_error_suspect,error_suspect);
      grp_suspect ->SetMarkerSize(0.8);
      grp_suspect ->SetMarkerStyle(21);
      grp_suspect ->SetMarkerColor(kGreen-1);
      grp_suspect ->SetLineColor(kGreen-1);
      grp->Add(grp_suspect);
      legend->AddEntry(grp_suspect, "Low Stats", "p");

    }



   if(ko!=0 or ki!=0) {

      grp->Draw("AP");
      grp->GetXaxis()->SetTitle("slug number");
      grp->GetYaxis()->SetTitle(ytitle);
      grp->GetYaxis()->CenterTitle();
      grp->GetXaxis()->CenterTitle();
      grp->GetXaxis()->SetNdivisions(511);
      legend->SetFillColor(0);
      legend->Draw();
     }

    if(ki!=0){
      stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
      stats1->SetTextColor(kBlue);
      stats1->SetFillColor(kWhite); 
      stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); 
      stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
    }

    if(ko!=0){
      stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
      stats2->SetTextColor(kRed);
      stats2->SetFillColor(kWhite); 
      stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
      stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
    }
   }
 }
