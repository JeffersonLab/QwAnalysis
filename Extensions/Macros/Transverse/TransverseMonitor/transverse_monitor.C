/****************************************************************
Author: B. P. Waidyawansa
Data : August 22nd 2011.

This script should be used to perfrom weekly monitoring of the amount of residual transverse polarization 
left in the longitudinally polarized Qweak electron beam. It access the unregressed data from the current 
qweak data base and performs a fit over the main detector octants to calculat the ratio of 
(A_T_measured_during_longitudinal/A_T_transverse = amount of transevrse polarization in the beam.)
This value indicating the residual transverse polarization in the beam should be reported to source 
group for corrections. 

The fit uses the amplitude of (-4.80+-0.06)ppm from the preliminary hydrogen 
transverse results described in Qweak-doc-1655-v2 (https://qweak.jlab.org/doc-private/ShowDocument?docid=1655)

USAGE:
./transevrse_monitor <slugs> <db> <wien_flip>
e.q. ./transevrse_monitor 9

Inputs : slugs > Number of slugs to be analyzed. You can enter the last slug in the range at the prompt.
                 For typical monitorung, to be able to see a 3 sigma deviation from the zero , you need to have at least 3days worth of data ~ 9 slugs.

         db >    qweak db to use. Default is qw_run1pass5

Output : Plot of md bar asymmetry vs azimuthal angle for IHWP IN, OUT and IN+OUT
         Plot of md bar asymmetry vs azimuthal angle for IN-OUT and IN-OUT sum of opposite octants.


         
****************************************************************/
// 02-10-2012 Buddhini Changed the At to -4.723 ppm based on pass4 results
// 07-23-2012 Buddhini Changed the At to -4.80 ppm based on the preliminary results from run1 and run2.
//                     See Qweak-doc-1655-v2
// 03-29-2013 Buddhini Changed default DB to qw_run1_pass5. Added option to input wien flip.
//                     Left flip AVG(IN-OUT), Right flip AVG(OUT-IN)
//

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
TString use_db="qw_run1_pass5";

/*Other variables*/
Int_t slug_num = 0;
Int_t wien_num = 0;
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
//TString reg_set="on_5+1";
TString reg_set="on";

Bool_t usefit1 = false;

/*Function defintions*/
TString get_sum_query(TString device,TString ihwp, Int_t slug_last);
void get_data(TString devicelist[],Int_t size,TString ihwp,Int_t slug_last,
		     Double_t value[],Double_t error[]);
void get_device_data(TString device,TString ihwp,TString wien,Int_t slug_last,TVectorD* value,
		     TVectorD*error,TVectorD* runlets);
TString runletbased_query(TString device,TString ihwp,TString wien,Int_t slug_last);
void plot_md_data(TString device, Int_t slug_last);
void plot_n_fit_data(const size_t size, TString fit, Double_t valuein[],Double_t errorin[], 
	       Double_t valueout[],Double_t errorout[]);

/*main function*/
int main(Int_t argc,Char_t* argv[])
{
  // TApplication theApp("App",&argc,argv);

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



  std::cout<<"###########################################################"<<std::endl;
  std::cout<<" \n Qweak Residual Transverse Polarization Monitoring tool\n"<<std::endl;
  std::cout<<"   Created by: Buddhini Waidyawansa                "<<std::endl;
  std::cout<<"   contact : buddhini@jlab.org                     "<<std::endl;
  std::cout<<"##########################################################"<<std::endl;
  std::cout<<"NOTE: "<<std::endl;
  std::cout<<"This macro looks only at LH2 longitudinal data that are marked as parity+production and  "<<std::endl;
  std::cout<<"suspect+good or good only.It is the users responsibilty to make sure all the runs in the  "<<std::endl;
  std::cout<<"given slug range have been analyzed in to the database. The script doesn't have checks for  "<<std::endl;
  std::cout<<"this. So it should not be interpreted that all the runs in the slug range was included in the "<<std::endl;
  std::cout<<"given results.  \n"<<std::endl;

  
  if(argc==2){
    slug_range = atoi(argv[1]);
  } 
  else if(argc==3){
    slug_range = atoi(argv[1]);
    use_db = argv[2];
  }
  else{
    std::cout<<"Proper usage:"<<std::endl;
    std::cout<<"./transverse_monitor <number of slugs> <database (default qw_run1_pass5)>:"<<std::endl;
    exit(1);
  }

  /*connect to the data base*/
  db = TSQLServer::Connect(Form("mysql://127.0.0.1/%s",use_db.Data()),"qweak", "QweakQweak");
  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);

  /*Get the starting slug number*/
  std::cout<<" Please enter the last finished slug number in your range:"<<std::endl;
  std::cin>>slug_num;
  slug_first =  Int_t((slug_num-slug_range)-1.0);
  /*Get Wien flip to correct for natural spin
    left IN and Right OUT has the right spin sign 
  */
  std::cout<<" Please enter the wien flip direction 1. left 2. right"<<std::endl;
  std::cin>>wien_num;

  std::cout<<" Grabing data from slug"<<slug_first<<" to "<<slug_num<<" with Wien correction."<<std::endl;

  
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
  std::cout<<"Getting data for IHWP 'in'.."<<std::endl;
  get_data(quartz_bar_sum,8,"in",slug_num,barvaluesin,barerrorsin);
  get_data(opposite_quartz_bar_sum,4,"in",slug_num,oppvaluesin,opperrorsin);

  std::cout<<"Getting data for IHWP 'out'.."<<std::endl;
  get_data(quartz_bar_sum,8,"out",slug_num,barvaluesout,barerrorsout);
  get_data(opposite_quartz_bar_sum,4,"out",slug_num,oppvaluesout,opperrorsout);


  /*Fit octants*/
  Canvas1->Draw();
  Canvas1->cd();
  std::cout<<"\n############   Plot n Fit octants\n"<<std::endl;
  plot_n_fit_data(8, "modfit",  barvaluesin,barerrorsin, barvaluesout,barerrorsout);
  Canvas1->Update();
  Canvas1->Print(Form("%s_transverse_monitor_on_slugs_%i_to_%i_reg_%s_barsums.png",
		      use_db.Data(),slug_first,slug_num,reg_set.Data()));

  /*Fit opposite bar sums*/
  Canvas2->Draw();
  Canvas2->cd();
  std::cout<<"\n############   Plot n Fit opposite octants\n"<<std::endl;
  plot_n_fit_data(4, "pol0",  oppvaluesin,opperrorsin,oppvaluesout,opperrorsout);
  Canvas2->Update();
  Canvas2->Print(Form("%s_transverse_monitor_on_slugs_%i_to_%i_reg_%s_opposite_sums.png",
		      use_db.Data(),slug_first,slug_num,reg_set.Data()));


  std::cout<<"Done! \n";
  db->Close();
  delete db;
  //  theApp.Run();
  return(1);
}


/*A function to create the mysql query*/
TString get_sum_query(TString device,TString ihwp, Int_t slug_last){

  Bool_t ldebug = true;

  std::cout<<"Getting regressed data for "<<device<<std::endl;
  TString datatable = "md_data_view";
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2))))),"
    +datatable+".error* SQRT("+datatable+".n) ";

 
  TString slug_cut   = Form("(%s.slug >= %i and %s.slug <= %i) ",
 			    datatable.Data(),slug_first,datatable.Data(),slug_last);

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = '%s' ",
			    datatable.Data(),datatable.Data(),reg_set.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    //+datatable+".error* SQRT("+datatable+".n) < 0.000700 AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}


/*A function to get fill the values and errors from the database for each octants*/
void get_data(TString devicelist[],Int_t size, TString ihwp,Int_t slug_last,
		     Double_t value[], Double_t error[]){
  
  Bool_t ldebug = false;

  TString query;
  TSQLStatement* stmt = NULL;
  
  std::cout<<"Extracting average asymmetries from slug "<<slug_first<<" to "<<slug_last<<std::endl;
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



/*Function to plot and fit octant data*/
void plot_n_fit_data(const size_t size, TString fit, Double_t valuesin[],Double_t errorsin[], 
		     Double_t valuesout[],Double_t errorsout[]){

  TF1 *modfit;
  TString usefit="";

  Double_t x[size];
  Double_t errx[size];
  Double_t valuesum[size];
  Double_t valueerror[size];
  Double_t valuediff[size];
  Double_t errordiff[size];

  /*set X location and clear the other arrays*/
  for(size_t i =0;i<size;i++){
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

  if(fit == "modfit"){
      usefit = "modfit";
      modfit = new TF1("modfit","(-4.80)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
      modfit->SetParName(0,"P_{V}");
      modfit->SetParName(1,"P_{H}");
      modfit->SetParName(2,"C");

      title = Form("LH2: Regressed Main detector asymmetries from slugs %i to %i: Fit (-4.80)*[p0*cos(phi) - p1**sin(phi)]+ p2",slug_first,slug_num);
  }
  if(fit == "pol0"){
    usefit = "pol0";
    title = Form("LH2: Regressed slug averages of Opposite Main detector asymmetries from slugs %i to %i: Fit p0",slug_first,slug_num);
  }

  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.35);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(1,2);
  pad2->cd(1);


  /*Draw IN values*/
  for(size_t i=0;i<size;i++) std::cout<<" IN values : "<<i+1<<" = "<<valuesin[i]<<", error = "<<errorsin[i]<<std::endl;

  TGraphErrors* grp_in  = new TGraphErrors(size,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.8);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->SetLineColor(kBlue);
  grp_in->Fit(usefit,"EM");
  TF1* fit1 = grp_in->GetFunction(usefit);
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);


   /*Draw OUT values*/

  for(size_t i=0;i<size;i++) std::cout<<" out values : "<<i+1<<" = "<<valuesout[i]<<", error = "<<errorsout[i]<<std::endl;

  TGraphErrors* grp_out  = new TGraphErrors(size,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.8);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out ->SetLineColor(kRed);

  grp_out->Fit(usefit,"EM");
  TF1* fit2 = grp_out->GetFunction(usefit);
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);

  /* Draw (IN+OUT)/2 values */
  for(size_t i =0;i<size;i++){
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


  /* Draw IN-OUT/OUT-IN weighted average values */
  pad2->cd(2);

  for(size_t i =0;i<size;i++){

    if(wien_num == 1) // left flip wien IHWP IN has the natuaral spin sign
      valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1.0/pow(errorsin[i],2)) + (1.0/pow(errorsout[i],2)));
    if(wien_num == 2) // right flip wien IHWP OUT has the natuaral spin sign
      valuediff[i]=( (valuesout[i]/pow(errorsout[i],2))- (valuesin[i]/pow(errorsin[i],2))) /( (1.0/pow(errorsout[i],2)) + (1.0/pow(errorsin[i],2)) );

    errordiff[i]= sqrt(1.0/((1.0/(pow(errorsin[i],2)))+(1.0/pow(errorsout[i],2))));
  }

  TGraphErrors* grp_diff  = new TGraphErrors(size,x,valuediff,errx,errordiff);
  grp_diff ->SetMarkerSize(0.8);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta-2);
  grp_diff ->SetLineColor(kMagenta-2);

  grp_diff->Fit(usefit,"EM");
  if(wien_num == 1)  grp_diff->SetTitle("Graph of AVG(IN-OUT)");
  if(wien_num == 2)  grp_diff->SetTitle("Graph of AVG(OUT-IN)");

  TF1* fit4 = grp_diff->GetFunction(usefit);
  if(fit4 != NULL){
    fit4->DrawCopy("same");
    fit4->SetLineColor(kMagenta-2);
  }

  grp_diff->Draw("AP");
  grp_diff->GetXaxis()->SetTitle("Octant");
  grp_diff->GetXaxis()->CenterTitle();
  grp_diff->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp_diff->GetYaxis()->CenterTitle();
  grp_diff->GetYaxis()->SetTitleOffset(0.7);
  grp_diff->GetXaxis()->SetTitleOffset(0.8);

  pad->Modified();
  pad->Update();
}
