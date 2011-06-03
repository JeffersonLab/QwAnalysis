//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : March 9th, 2011
//*****************************************************************************************************//
//
//
//  This macro will connect to theqw_linreg_20110125 data base and get the slug averages from the 
//  regressed data and plot 
//  them in to three plots for pmt+, pmt- and bar sum, us lumi and ds lumi asymmetries. 
//   e.g. use
//   ./slug_average_regressed
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
TString bpm,bpmdata,var1, target, polar,targ, goodfor;
Int_t runnum;

Int_t opt =1;
Int_t datopt = 1;
TTree * nt;

std::ofstream Myfile;
Char_t textfile[100];


Double_t p0[8] ={0.0};
Double_t ep0[8] ={0.0};
Double_t p1[8] ={0.0};
Int_t i = -1;
Double_t ep1[8] ={0.0};

Double_t value1[8] ={0.0};
Double_t err1[8] ={0.0};
Double_t value2[8] ={0.0};
Double_t err2[8] ={0.0};
Double_t value3[8] ={0.0};
Double_t err3[8] ={0.0};
Double_t value11[8] ={0.0};
Double_t err11[8] ={0.0};
Double_t value22[8] ={0.0};
Double_t err22[8] ={0.0};
Double_t value33[8] ={0.0};
Double_t err33[8] ={0.0};

Double_t value111[4] ={0.0};
Double_t err111[4] ={0.0};
Double_t value222[4] ={0.0};
Double_t err222[4] ={0.0};

Double_t valuein[8] ={0.0};
Double_t errin[8] ={0.0};
Double_t valueout[8] ={0.0};
Double_t errout[8] ={0.0};

Double_t valuesum[8] ={0.0};
Double_t valueerror[8] ={0.0};
Double_t valuesum1[8] ={0.0};
Double_t valueerror1[8] ={0.0};
Double_t valuesum11[8] ={0.0};
Double_t valueerror11[8] ={0.0};

TString xunit, yunit, slopeunit;

void get_octant_data(Int_t size,TString devicelist[], TString detector_type, TString goodfor, TString target, TString ihwp, Double_t value[], Double_t error[]);
TString get_query(TString detector, TString measurement, TString target, TString ihwp, TString detector_type, TString goodfor, TString polar);
void plot_octant(Int_t size,TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[]);

// void get_octant_data(Int_t size,TString devicelist[], TString detector_type, TString target, TString ihwp, Double_t value[], Double_t error[]);
// TString get_query(TString detector, TString measurement, TString target, TString ihwp, TString detector_type);
// void plot_octant(Int_t size,TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[]);


int main(Int_t argc,Char_t* argv[])
{


  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nSlug averages of MD and LUMI asymmetries\n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;
  std::cout<<"Enter target type:"<<std::endl;
  std::cout<<"1. Liquid Hydrogen"<<std::endl;
  std::cout<<"2. 4% DS Al "<<std::endl;
  std::cin>>opt;
  std::cout<<"Enter data type:"<<std::endl;
  std::cout<<"1. Longitudinal"<<std::endl;
  std::cout<<"2. Transverse "<<std::endl;
  std::cin>>datopt;


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
  db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_linreg_20110125","qweak", "QweakQweak");
  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);

  // clear arrays;
  for(Int_t i =0;i<8;i++){
    value1[i] = 0.0;
    err1[i] = 0.0;
    value2[i] =0.0;
    err2[i]=0.0;
    value3[i]=0.0;
    err3[i]=0.0;
    value11[i]=0.0;
    err11[i]=0.0;
    value22[i]=0.0;
    err22[i]=0.0;
    value33[i]=0.0;
    err33[i]=0.0;
    valuein[i]= 0.0;
    errin[i]=0.0;
    valueout[i]=0.0;
    errout[i]=0.0;

  
  }

  for(Int_t i =0;i<4;i++){

    value111[i] = 0.0;
    err111[i] = 0.0;
    value222[i] =0.0;
    err222[i]=0.0;

  
  }

  //plot MD asymmetries
  TString title1 = Form("%s (%s): Regressed slug averages of Main detector asymmetries. TOP FIT = p0*cos(phi + p1) + p2 and BOTTOM FIT = pol0",targ.Data(),polar.Data());
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
  get_octant_data(8,quartz_bar_SUM, "MD", goodfor, target, "out", value3,  err3);
  get_octant_data(8,quartz_bar_SUM, "MD", goodfor, target, "in",  value33, err33);
  plot_octant(8,"MD BAR SUM", value33,err33,value3,err3);
  gPad->Update();


  Canvas1->Update();
  Canvas1->Print(polar+"_"+target+"_md_regressed_slug_summary_plots.gif");

  // plot US LUMI asymmetries
  TString title2 = targ+"("+polar+"): Regressed slug averages of US Lumi asymmetries. TOP FIT = p0*cos(phi + p1) + p2 and BOTTOM FIT = pol0";
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
  get_octant_data(4,us_lumi,"LUMI", goodfor, target, "in", value111,err111);
  get_octant_data(4,us_lumi,"LUMI", goodfor, target, "out", value222,err222);
  plot_octant(4,"US LUMI", value111,err111,value222,err222);
  gPad->Update();

  Canvas2-> Update();
  Canvas2->Print(polar+"_"+target+"_uslumi_regressed_slug_summary_plots.gif");

  // plot DS LUMI asymmetries
  TString title3 = targ+"("+polar+"): Regressed slug averages of DS LUMI asymmetries. TOP FIT = p0*cos(phi + p1) + p2 and BOTTOM FIT = pol0";
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
  get_octant_data(5,DS_lumi,"LUMI", goodfor, target, "in", valuein,errin);
  get_octant_data(5,DS_lumi,"LUMI", goodfor, target, "out", valueout,errout);
  plot_octant(5,"DS_LUMI",valuein,errin,valueout,errout);
  gPad->Update();

  Canvas3-> Update();
  Canvas3->Print(polar+"_"+target+"_dslumi_regressed_slug_summary_plots.gif");


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
TString get_query(TString detector, TString measurement, TString target, TString ihwp, TString detector_type, TString goodfor, TString polar){



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

 
  //TString good_for_cut = "run.run_type ='parity' AND md_data_view.good_for_id = NULL || ((md_data_view.good_for_id = 'parity' || md_data_view.good_for_id = 'production') && md_data_view.good_for_id != 'commissioning'))";

  //TString run_quality_cut = "(md_data_view.run_quality_id = NULL || md_data_view.run_quality_id != 'bad')";

  //  TString good_for_cut = Form("%s.good_for_id = '1,8' OR %s.good_for_id = 8",datatable.Data(),datatable.Data());
  
  TString good_for_cut = Form("%s.good_for_id = '%s'",datatable.Data(),goodfor.Data());
  TString run_quality_cut = Form("%s.run_quality_id = 1",datatable.Data()); // good

// To get rid of runs that have large charge asymmetries
  TString run_cut = Form("%s.run_number != 9831 AND %s.run_number != 9837 AND %s.run_number != 9853 AND %s.run_number != 9888  AND %s.run_number != 9885 AND %s.run_number != 9880 AND %s.run_number != 9851 AND  ",datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data(),datatable.Data()); 


  /*************************
  longitudinal

  **************************/
  if(polar == "longitudinal"){
    if(target == "HYDROGEN-CELL"){
      run_cut = "";
      if(ihwp == "out"){
	//plate_cut = "("+datatable+".run_number > 9270 AND "+datatable+".run_number < 9276)"; // slug 25
	//plate_cut = "("+datatable+".run_number > 9296 AND "+datatable+".run_number < 9344)"; // slug 27
	plate_cut = "("+datatable+".run_number > 9803 AND "+datatable+".run_number < 9813)"; // slug 40
	//plate_cut = "("+datatable+".run_number > 9749 AND "+datatable+".run_number < 9761)"; // slug 36
	//plate_cut = "("+datatable+".run_number > 9773 AND "+datatable+".run_number < 9789)"; //slug 33
	//plate_cut = "("+datatable+".run_number > 9590 AND "+datatable+".run_number < 9620)"; // slug 31
	// plate_cut = "("+datatable+".run_number > 9083 AND "+datatable+".run_number < 9107)"; // slug 21 / 9083-9106 
	//plate_cut = "("+datatable+".run_number > 9704 AND "+datatable+".run_number < 9735)"; // slug 34 / 9704 - 9734
	//plate_cut = "("+datatable+".run_number > 9749 AND "+datatable+".run_number < 9762)"; // slug 36 / 9749 - 9761 
	//plate_cut = "("+datatable+".run_number > 9773 AND "+datatable+".run_number < 9789)"; // slug 38/9773 - 9788 
	//plate_cut = "("+datatable+".run_number > 9803 AND "+datatable+".run_number < 9813)"; // slug 40/9803 - 9812
	// plate_cut = "("+datatable+".run_number > 9969 AND "+datatable+".run_number < 9982)"; // slug 43/	9969-9982
	//plate_cut = "("+datatable+".run_number > 9991 AND "+datatable+".run_number < 10004)"; // slug 45/	9991-10003 
	
      }
      if(ihwp == "in"){
	//plate_cut = "("+datatable+".run_number > 9277 AND "+datatable+".run_number < 9295) "; //slug 26 part
	//plate_cut = "("+datatable+".run_number > 9413 AND "+datatable+".run_number < 9421) "; //slug 29 part
	plate_cut = "("+datatable+".run_number > 9789 AND "+datatable+".run_number < 9796) "; //slug 39 part
	//plate_cut = "("+datatable+".run_number > 9735 AND "+datatable+".run_number < 9749) "; //slug 35 9060-9082
	//plate_cut = "("+datatable+".run_number > 9060 AND "+datatable+".run_number < 9083) "; //slug 20/ 9060-9082
	// plate_cut = "("+datatable+".run_number > 9735 AND "+datatable+".run_number < 9749) "; //slug 35/9735 - 9748
	//plate_cut = "("+datatable+".run_number > 9762 AND "+datatable+".run_number < 9773) "; // slug-37/ 9762 - 9772
	// plate_cut = "("+datatable+".run_number > 9789 AND "+datatable+".run_number < 9796) "; // slug-39/ 9789-9795
	//plate_cut = "("+datatable+".run_number > 9890 AND "+datatable+".run_number < 9906) "; // slug-41/ 9890-9906
	//plate_cut = "("+datatable+".run_number > 9983 AND "+datatable+".run_number < 9991) "; // slug-44/ 9983-9990
	//plate_cut = "("+datatable+".run_number > 10005 AND "+datatable+".run_number < 10030) "; // slug-46/ 10005-10029
	
	
      }
    }
    
    if(target == "DS-4%-Aluminum"){
      //run_cut = datatable+".run_number > 9847 AND "+datatable+".run_number < 9859 "; // since the db reads the range as lower to upper-1 to give the proper range I add 1 to the upper cut.
      if(ihwp == "out"){
	plate_cut = "("+datatable+".run_number > 9442 AND "+datatable+".run_number < 9448)"; // slug 1011
      }
      if(ihwp == "in"){
	plate_cut = "("+datatable+".run_number > 9500 AND "+datatable+".run_number < 9503)"; // slug 1012

      }
    }
  }
  

  /*************************
   Transverse

  **************************/

  if(polar == "transverse"){
    if(target == "HYDROGEN-CELL"){
      if(ihwp == "out")
	plate_cut = "("+datatable+".run_number > 9863 AND "+datatable+".run_number < 9879)";// OR ("+datatable+".run_number > 9853 AND "+datatable+".run_number < 9888)";
      if(ihwp == "in")
	plate_cut = "("+datatable+".run_number > 9881 AND "+datatable+".run_number < 9885)";// OR ("+datatable+".run_number > 9837 AND "+datatable+".run_number < 9853)";
      
    }
    
    if(target == "DS-4%-Aluminum"){
      if(ihwp == "out")
	plate_cut = "("+datatable+".run_number > 9853 AND "+datatable+".run_number < 9859)";
      if(ihwp == "in")
	plate_cut = "("+datatable+".run_number > 9847 AND "+datatable+".run_number < 9853)";
    }
  }
  

  TString query =" SELECT " + output
    + " FROM "+datatable+", analysis, run, runlet "
    + " WHERE "+datatable+".analysis_id = analysis.analysis_id AND analysis.runlet_id = runlet.runlet_id AND"
    + " run.run_number = "+datatable+".run_number AND "
    +datatable+".detector = '"+detector+"' AND "
    +datatable+".subblock = 0 AND "
    +run_cut+
    +datatable+".measurement_type = '"+measurement+"' AND "+datatable+".slope_correction = 'on' AND "+datatable+".slope_calculation = 'off' AND "
    //+good_for_cut+" AND "
    // +run_quality_cut+ " AND "
    +plate_cut+" AND error !=0;";

 			 
  if(ldebug)  std::cout<<query<<std::endl;

  return query;
}


//***************************************************
//***************************************************
//         get octant data                  
//***************************************************
//***************************************************

void get_octant_data(Int_t size, TString devicelist[], TString detector_type,TString goodfor, TString target, TString ihwp, Double_t value[], Double_t error[])
{
  Bool_t ldebug = false;

  if(ldebug) printf("\nDetector Type %s, size %2d\n", detector_type.Data(), size);
  for(Int_t i=0 ; i<size ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    }
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",target,ihwp,detector_type,goodfor,polar);
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

void plot_octant(Int_t size,TString device, Double_t valuesin[],Double_t errorsin[],Double_t valuesout[],Double_t errorsout[])  
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


  TGraphErrors* grp_in  = new TGraphErrors(k,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.6);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in->Fit("cosfit");
  TF1* fit1 = grp_in->GetFunction("cosfit");
  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);

  TGraphErrors* grp_out  = new TGraphErrors(k,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.6);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out->Fit("cosfit");
  TF1* fit2 = grp_out->GetFunction("cosfit");
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);


 
  // Sum over the in and out half wave plate states
  for(Int_t i =0;i<8;i++){
    valuesum[i]=(valuesin[i]+valuesout[i])/2;
    valueerror[i]= sqrt(pow(errorsin[i],2)+pow(errorsout[i],2))/2;
  }


  TGraphErrors* grp_sum  = new TGraphErrors(k,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.8);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
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
  grp->GetYaxis()->SetTitle(Form("%s Asymmetry (ppm)",device.Data()));
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
  stats2->SetTextColor(kRed); 
  stats3->SetTextColor(kGreen-2);
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  
  pad->Update();
  pad->cd(2);


}



