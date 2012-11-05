//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : Sepetember 25th, 2012
//*****************************************************************************************************//
/*

This macro calculates the residual transverse polarization in the beam using AVG(IN-OUT) from consecative
slugs and creates a .txt file with PV and PH for a given Wien. 
Consecative IN, OUT slugs are read from a input text file store in the parent directory names wien*.txt (* = number).
To run this do:
./residual_transverse_distributions <wien> <database (optiona)>
 
wien represents the textfile that contains the list of IN and OUT slugs. For e.g. wien0.txt.
To run this macro on the wien0.txt slugs do
./residual_transverse_distributions 0 

The default database is qw_run2_pass1. 
The extracted residual polarization values are stored in a text file of the form:e.g.
 wien1_elastic_8901_longitudinal_HYDROGEN-CELL_residual_distributions_on_5+1_from_run2_pass1.txt

*/

#include "functions.h"
#include "functions.C"

using namespace std;

std::vector <double> in;
std::vector <double> out;
TString wien_number;
Bool_t status_in  = true;
Bool_t status_out = true;

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

  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");
  gROOT->SetBatch(true);

  std::cout<<"###############################################"<<std::endl;
  std::cout<<" \nResidual transverse distributions         \n"<<std::endl;
  std::cout<<"###############################################"<<std::endl;

  // third argument gives the database. Default is qw_run2_pass1
  if(argc==3){
    wien_number = argv[1];
    database = argv[2];
  }
  else if (argc>1)
    wien_number = argv[1];
  else if (argc==1){
    std::cout<<"Proper usage is /residual_transverse_distributions <wien number> <data base(optional)>"<<std::endl;
    std::cout<<"Please enter the wien number to use the appropriate .txt input file with slug numbers"<<std::endl;
    exit(1);
  }


  std::cout<<"Using database "<<database<<std::endl;
  //connect to the data base
  std::cout<<" Connecting to "<<Form("mysql://qweakdb.jlab.org/%s",database.Data())<<std::endl;
  db = TSQLServer::Connect(Form("mysql://qweakdb.jlab.org/%s",database.Data()),"qweak", "QweakQweak");

  // For now, lets correct on consecative slugs
  // select the slugs 
  //Get the IHWP numbers from a textfile.
  std::cout<<"Reading Slug numbers IN, OUT"<<std::endl;
  read_slug_numbers(Form("wien%s.txt",wien_number.Data()));

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

  // Open the txt file to store residuals
  Char_t  textfile[400];
  sprintf(textfile,"wien%s_%s_%s_%s_%s_residual_distributions_%s_from_%s.txt"
	  ,wien_number.Data(),interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 
  MyfileRes.open(textfile);
  MyfileRes<<"Fit results from uncorrected data: [0]cos(phi)-[1]sin(phi)+[2]"<<std::endl;
  MyfileRes<<" in \t out \t patterns\t Pv\t  error Pv\t \t min. chi square Ph\t error Ph \t min. chi square C\t error C "<<std::endl;

  // Open the txt file to store IN, OUT, AVG(IN-OUT) values
  sprintf(textfile,"wien%s_%s_%s_%s_%s_asymmetries_%s_from_%s.txt"
	  ,wien_number.Data(),interaction.Data(),qtor_stem.Data(),polar.Data(),target.Data(),reg_set.Data(),database_stem.Data()); 
  Myfile.open(textfile);


  // For each slug pair, get the asymmetries from the detectors and calcualte IN-OUT 
  for(size_t i=0;i<in.size();i++){
    std::cout<<" in = "<<in.at(i)<<" and out = "<<out.at(i)<<std::endl; 
    Myfile<<" in = "<<in.at(i)<<" and out = "<<out.at(i)<<std::endl; 
    get_residual_polarization(in.at(i),out.at(i));
  }


  Myfile.close();
  MyfileRes.close();

  std::cout<<"Done ! \n";
  db->Close();
  delete db;

  return(1);
}


//***************************************************
//***************************************************
//    Open and read a text file with IHWP numbers               
//***************************************************
//***************************************************

void read_slug_numbers(TString filename){

  string line;
  ifstream txtfile (filename);
  size_t found;

  if (txtfile.is_open())
  {
    while ( txtfile.good())
    {
      getline (txtfile,line);
      cout << line << endl;
      found=line.find_first_of(",");
      if (found!=string::npos){
	out.push_back(atof((line.substr(found+1)).c_str())); //find the substring before ","
	in.push_back(atof((line.erase (found,string::npos)).c_str())); //remove characters till end starting from "," to get "out"
      }
    }
    txtfile.close();
  }
  else cout << "Unable to open file"; 

}



//***************************************************
//***************************************************
//    Calcualte IN-OUT, fit and return fit results                
//***************************************************
//***************************************************

void get_residual_polarization(Double_t in_slug, Double_t out_slug){

  Double_t x[8];
  Double_t errx[8];
  Double_t valuesin[8]={0};
  Double_t errorsin[8]={0};
  Double_t valuesout[8]={0};
  Double_t errorsout[8]={0};
  Double_t in_out[8]={0};
  Double_t in_out_err[8]={0};
  Double_t fit[5]={0};
  Double_t tot_patterns=0;

  for(Int_t i =0;i<8;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }

  // fit  function
  TF1 *fit_t = new TF1("fit_t","-4.80*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1)))) + [2]",1,8);
  fit_t->SetParName(0,"P_{V}");
  fit_t->SetParName(1,"P_{H}");
  fit_t->SetParName(2,"C");

  // get asymmetris from the two slugs

  status_in = get_octant_data(quartz_barsum,"in", Int_t(in_slug),  valuesin, errorsin);
  status_out = get_octant_data(quartz_barsum,"out",Int_t(out_slug),valuesout, errorsout);
 
  if(status_in and status_out){
    //Take the weighted difference in the IHWP in and out half wave plate values.
    //Here, from IN+OUT ~ 0 we know that IN and OUT are a measurement of the same thing
    //so we can take the weighted error difference when we take IN-OUT.
    TString text;

    // even Wiens are Right flipped. So to get physics we should take OUT-IN 
    if(atoi(wien_number)%2==0){
      Myfile<<"! Even/Right flipped wien "<<wien_number<<" : OUT-IN "<<std::endl;
      text = Form(" AVG(OUT-IN) of in slug %f and out slug %f", in_slug, out_slug);
      for(Int_t i =0;i<8;i++){
	in_out[i]=((valuesout[i]/pow(errorsout[i],2)) - (valuesin[i]/pow(errorsin[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
	in_out_err[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));
	Myfile<<"Value OUT-IN = "<<setw(10)<<in_out[i]<<" +-  "<<setw(10)<<in_out_err[i]<<std::endl;
      }
    }
    else{
      text = Form(" AVG(IN-OUT) of in slug %f and out slug %f", in_slug, out_slug);
      Myfile<<"! Odd/Left flipped wien "<<wien_number<<" : IN-OUT "<<std::endl;
      text = Form(" AVG(IN-OUT) of in slug %f and out slug %f", in_slug, out_slug);
      for(Int_t i =0;i<8;i++){
	in_out[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
	in_out_err[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));
	Myfile<<"Value IN-OUT = "<<setw(10)<<in_out[i]<<" +-  "<<setw(10)<<in_out_err[i]<<std::endl;
      }
    }
    
    // Create canvas
    TCanvas * c = new TCanvas("c","",0,0,1000,500);
    c->Draw("goff");
    c->cd();
    
    TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
    TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
    pad1->SetFillColor(20);
    pad1->Draw();
    pad2->Draw();
    
    pad1->cd();
    std::cout<<text<<std::endl;
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
    grp_in_out ->Fit("fit_t","BQ");
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

    // Get total number of patterns from the two ihwp states
    tot_patterns= get_patterns(in_slug,out_slug);

    MyfileRes<<setw(5)<<in_slug<<", "<<setw(5)<<out_slug<<", "<<fixed<<tot_patterns<<", "
	     <<setw(10)<<fit[0]<<", "<<setw(10)<<fit[1]<<", "
	     <<setw(10)<<fit[2]<<", "<<setw(10)<<fit[3]<<", "
	     <<setw(10)<<fit[4]<<", "<<setw(10)<<fit[5]<<", "
	     <<setw(10)<<(fit_temp->GetChisquare() /fit_temp->GetNDF())<<std::endl;
    

    TMultiGraph * grp = new TMultiGraph();
    grp ->Add(grp_in_out);
    grp ->Draw("AP");
    grp ->SetTitle("");
    grp ->GetXaxis()->SetTitle("Octant");
    grp ->GetXaxis()->CenterTitle();
    grp ->GetYaxis()->SetTitle("Asymmetry (ppm)");
    grp ->GetYaxis()->CenterTitle();
    grp ->GetXaxis()->SetTitleOffset(1.0);
    grp ->GetYaxis()->SetTitleOffset(0.8);
    
    TPaveStats *stats1 = (TPaveStats*)grp_in_out->GetListOfFunctions()->FindObject("stats");
    stats1->SetTextColor(kRed);
    stats1->SetFillColor(kWhite); 
    stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);  
    
    // save canvas
    c->Modified();
    c->Update();
    
    c->Print(Form("slug_%i_%i_",Int_t(in_slug),Int_t(out_slug))+interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_octant_fit_"+reg_set+"_"+database_stem+".C");
    c->Print(Form("slug_%i_%i_",Int_t(in_slug),Int_t(out_slug))+interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_octant_fit_"+reg_set+"_"+database_stem+".png");
  }
  else{
    MyfileRes<<setw(5)<<in_slug<<", "<<setw(5)<<out_slug<<", either one or all of the detectors returned NULL results "<<std::endl; 
  
  }
    
}


//***************************************************
//***************************************************
//    Get number of patterns in the two slugs                
//***************************************************
//***************************************************

Double_t get_patterns(Double_t in_slug, Double_t out_slug){

  Double_t n_in  = 0;
  Double_t n_out = 0;
  Double_t n_tot = 0;

  n_in  = get_patterns("qwk_mdallbars", "a", "in", in_slug);
  n_out = get_patterns("qwk_mdallbars", "a", "out", out_slug);
  n_tot =n_in+n_out;

  return n_tot;
}
