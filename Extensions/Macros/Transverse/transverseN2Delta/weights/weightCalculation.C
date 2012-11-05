/*
  Weights in this file correspond to the Run1 weightfiles in QwAnalysis_3.02
  --smacewan
  A version of Scott's script for run1, run2 and n2delta transverse.
  --Nuruzzaman
*/

#include "NurClass.h"

void weightCalculation(){

  Bool_t rel = 0;
  gStyle->SetOptStat(0);
  static const Int_t numWReg = 14; //number of weighting regions
  // static const Int_t numWReg = 4; //number of weighting regions

  Double_t markerSize = 0.05;

  //9 Weight Regions for Run1 and Run2 corresponding to runCut[i] regions
  Double_t weights[numWReg][16] = {
    {24.9405790703649, 26.4195906548624, 35.5628894136391, 21.7630698115753, 21.8073495129328, 42.8557347401442, 37.5978012805811, 24.2025853201639, 25.4945951458291, 21.2396297507742, 24.661520629362, 29.5209923776798, 29.3853179197546, 25.2873272559457, 24.0467276010744, 21.1281145481858},
    {26.2745108342945, 27.4378532623608, 36.763489443364, 22.4319358985, 22.5643478790641, 42.3246384417761, 38.9097488375713, 25.1344694113507, 26.3868255857216, 21.9789397799029, 25.4995359084465, 31.0830258704024, 30.4363970610615, 26.2921956875541, 24.8415728690278, 21.7426281619217},
    {25.7898130238556, 26.1604788414047, 36.0585157593743, 22.0166357699878, 22.1025736236727, 41.7983314106101, 38.0015732651332, 24.7686607089781, 25.917210064171, 21.3877208816874, 24.9405168672716, 28.9658608364182, 29.8326685620355, 25.5872268563533, 24.3657592857909, 21.4353541870749},
    {26.4511077723935, 25.8966722776123, 36.244096742743, 22.0314300380923, 22.244714099812, 42.0342915750669, 38.1270469458329, 24.7910117707724, 26.0575454834456, 21.3927538464171, 24.9993125189057, 29.3204401584477, 29.8583222608722, 25.5414793624847, 24.3408497877478, 21.5396545039418},
    {28.4275849913722, 26.8885871391888, 38.1408618309139, 23.0891425616018, 23.4090068494754, 44.0952103782487, 40.1772620802983, 26.2001640130267, 27.6787493633888, 22.486109206038, 26.3272210301842, 30.8666744862242, 31.6900962111321, 27.0469101609832, 25.7267153930656, 22.8601734629962},
    {28.4275849913722, 26.8885871391888, 38.1408618309139, 23.0891425616018, 23.4090068494754, 44.0952103782487, 40.1772620802983, 26.2001640130267, 27.6787493633888, 22.486109206038, 26.3272210301842, 30.8666744862242, 31.6900962111321, 27.0469101609832, 25.7267153930656, 22.8601734629962},
    {29.227716058584, 25.2887980740051, 37.4559892126751, 22.5365825075554, 22.8495176466825, 43.1464087086711, 38.6212223616877, 26.0936501102457, 27.3339802375323, 21.8245307725884, 25.5240730294778, 30.1095989401421, 30.727063783239, 26.0665113102593, 25.0227081076077, 22.7124580103433},
    {28.4275849913722, 26.8885871391888, 38.1408618309139, 23.0891425616018, 23.4090068494754, 44.0952103782487, 40.1772620802983, 26.2001640130267, 27.6787493633888, 22.486109206038, 26.3272210301842, 30.8666744862242, 31.6900962111321, 27.0469101609832, 25.7267153930656, 22.8601734629962},
    {29.227716058584, 25.2887980740051, 37.4559892126751, 22.5365825075554, 22.8495176466825, 43.1464087086711, 38.6212223616877, 26.0936501102457, 27.3339802375323, 21.8245307725884, 25.5240730294778, 30.1095989401421, 30.727063783239, 26.0665113102593, 25.0227081076077, 22.7124580103433},

    {29.227716058584, 25.2887980740051, 37.4559892126751, 22.5365825075554, 22.8495176466825, 43.1464087086711, 38.6212223616877, 26.0936501102457, 27.3339802375323, 21.8245307725884, 25.5240730294778, 30.1095989401421, 30.727063783239, 26.0665113102593, 25.0227081076077, 22.7124580103433},
    {29.227716058584, 25.2887980740051, 37.4559892126751, 22.5365825075554, 22.8495176466825, 43.1464087086711, 38.6212223616877, 26.0936501102457, 27.3339802375323, 21.8245307725884, 25.5240730294778, 30.1095989401421, 30.727063783239, 26.0665113102593, 25.0227081076077, 22.7124580103433},
    {29.6787864937778, 25.8767026870368, 35.6876474345935, 22.2787111320036, 22.9753543374023, 42.8537145599781, 39.8849717414975, 26.4928032300026, 27.8917465532774, 22.1697046551946, 25.7475147211415, 30.6723062813816, 31.071339796172, 26.5869766353649, 24.9529013986101, 22.7891660304691},
    {29.6787864937778, 25.8767026870368, 35.6876474345935, 22.2787111320036, 22.9753543374023, 42.8537145599781, 39.8849717414975, 26.4928032300026, 27.8917465532774, 22.1697046551946, 25.7475147211415, 30.6723062813816, 31.071339796172, 26.5869766353649, 24.9529013986101, 22.7891660304691}
    {29.6787864937778, 25.8767026870368, 35.6876474345935, 22.2787111320036, 22.9753543374023, 42.8537145599781, 39.8849717414975, 26.4928032300026, 27.8917465532774, 22.1697046551946, 25.7475147211415, 30.6723062813816, 31.071339796172, 26.5869766353649, 24.9529013986101, 22.7891660304691}
  };

  //detector names
  TString dets[16] = {"md1neg","md1pos","md2neg","md2pos","md3neg","md3pos","md4neg","md4pos",
		      "md5neg","md5pos","md6neg","md6pos","md7neg","md7pos","md8neg","md8pos"};

  //colour sequence for plot to distinguish between weight periods
  // Int_t colour[numWReg] = {kRed, kGreen, kBlue, kRed, kGreen, kBlue, kRed, kGreen, kBlue, kRed, kGreen, kBlue, kRed};

  Int_t colour[numWReg] = {2, 3, 4, 2, 3, 4, 2, 3, 4, 2, 3, 4, 2, 3};

  // //colour sequence for plot to distinguish between weight periods
  // Int_t colour[numWReg] = {kRed, kGreen, kBlue, kRed};

//   //open dbrootfile and grab "tree"
//   Char_t filename[100];
//   sprintf(filename,"%s",rootfile.Data());
//   f = new TFile(filename);
//   if(!f->IsOpen()){
//     std::cerr<<"Error opening ROOTFile "<<filename<<".\n"<<endl;
//     return 0;
//   }
//   std::cout<<"Successfully opened ROOTFile "<<filename<<".\n"<<endl;

  TString rootfileDir = "/net/cdaq/cdaql5data/qweak/db_rootfiles";

  TChain * tree = new TChain("tree");
  tree->Add(Form("%s/y_run1/offoff_tree.root",rootfileDir.Data()));
  tree->Add(Form("%s/y_run2/offoff_tree.root",rootfileDir.Data()));
 
  // TTree *tree = f->Get("tree");

  //Instantiate strings
  TString denom[numWReg];     //denominator for relative quantities
  TString runCut[numWReg];    //run number cuts
  TString cut ="";            //n>0 cuts
  for(Int_t j=0;j<numWReg;j++){
    denom[j]="";
    runCut[j]="";
  }

  Int_t scale_viewXlow=9400;
  Int_t scale_viewXhigh =19400;
  std::string in_scale_viewXlow,in_scale_viewXhigh;
  std::cout<<Form("Enter Run Range(Just Hit ENTER to choose default):")<<std::endl;
  std::cout<<Form("Low run range (deafult = 9400):")<<std::endl;
  std::getline(std::cin,in_scale_viewXlow);
  if ( !in_scale_viewXlow.empty() ) {
    std::istringstream stream(in_scale_viewXlow);
    stream>>scale_viewXlow;
  }
  std::cout<<Form("High run range (deafult = 19500):")<<std::endl;
  std::getline(std::cin,in_scale_viewXhigh);
  if ( !in_scale_viewXhigh.empty() ) {
    std::istringstream stream(in_scale_viewXhigh);
    stream>>scale_viewXhigh;
  }

  TString nameString = Form("%d-%d",scale_viewXlow,scale_viewXhigh);

  //  Double_t scale_viewX[2] = {9400,19500};
  //  Double_t scale_viewX[2] = {15050,16165};
  //  Double_t scale_viewX[2] = {13000,15700};
  if(rel){
  Double_t scale_viewY[2] = {0.90,1.15};
  }
  else{
  Double_t scale_viewY[2] = {0.80,1.15};
  }

  //Run range cuts for run1 
  runCut[0] += "run>9418 && run<9594";
  runCut[1] += "run>9594 && run<9703";
  runCut[2] += "run>9703 && run<9810";
  runCut[3] += "run>9810 && run<10213"; 
  runCut[4] += "run>10213 && run<11647";
  runCut[5] += "run>11647 && run<12033";
  runCut[6] += "run>12033 && run<12087";
  runCut[7] += "run>12087 && run<12097";
  runCut[8] += "run>12097 && run<12163";
  //Run range cuts for run2
  runCut[9] += "run>12163 && run<13336"; 
  runCut[10] += "run>13336 && run<14017";
  runCut[11] += "run>14017 && run<14889";
  runCut[12] += "run>14889 && run<15592";
  runCut[13] += "run>15592 && run<19400"; 
  // runCut[13] += "run>15992 && run<16066";



  //construct denom if plotting relative weigthed yields
  if(rel){
    for(Int_t j=0; j<numWReg; j++){
      denom[j] += Form("yield_%s * %f",dets[0].Data(), weights[j][0]);
      for(Int_t i=1; i<16; i++){
	denom[j] += Form("+ yield_%s * %f",dets[i].Data(), weights[j][i]);
      }//for each tubes
    }//for each region
  }

  //Construct n>0 cuts
  for(Int_t i=0; i<16; i++){
    cut += Form("&& yield_%s.n>0",dets[i].Data()); //&& in front okay because this cut comes second
  }

  gStyle->SetTitleSize(0.060);
  gStyle->SetTitleYSize(0.060);
  gStyle->SetTitleXSize(0.060);
  gStyle->SetTitleYOffset(0.85);
  gStyle->SetTitleXOffset(0.72);
  gStyle->SetLabelSize(0.060);
  gStyle->SetLabelSize(0.060,"y");
  gStyle->SetLabelSize(0.060,"x");
  //  gStyle->SetNdivisions(510,"x");
  gStyle->SetNdivisions(-505,"x");

  TString title;
  if(rel){
  title = Form("%s Normalized Weighted Yield",nameString.Data());
  }
  else{
  title = Form("%s Weighted Yield",nameString.Data());
  }

  TCanvas *c1 = new TCanvas("c1",Form("%s",title.Data()),1600,1000);
  c1->Divide(4,4);
//   c1->Modified();
//   c1->Update();
  TH2F *hh[16];
  for(Int_t i=0; i<16; i++){
    c1->cd(i+1);
    //std::cout<<"Plotting "<<dets[i].Data()<<std::endl;
    if(rel){
      hh[i] = new TH2F(Form("hh_%s",dets[i].Data()),Form("hh_%s",dets[i].Data()),3000,scale_viewXlow,scale_viewXhigh,100, scale_viewY[0],scale_viewY[1]);
    }
    else{
      hh[i] = new TH2F(Form("hh_%s",dets[i].Data()),Form("hh_%s",dets[i].Data()),3000,scale_viewXlow,scale_viewXhigh,100, scale_viewY[0],scale_viewY[1]);
    }



    for(Int_t j=0; j<numWReg; j++){
      if(j==0)//first plot on histogram	
	if(rel){
	  tree->Draw(Form("(16 * yield_%s * %f)/(%s):run>>hh_%s",dets[i].Data(),weights[j][i],denom[j].Data(),dets[i].Data()),
		     Form("%s %s",runCut[j].Data(),cut.Data()));
	}
	else{
	  tree->Draw(Form("(yield_%s * %f):run>>hh_%s",dets[i].Data(),weights[j][i],dets[i].Data()),
		     Form("%s %s",runCut[j].Data(),cut.Data()));
	}
      else//add plot to same histogram
	if(rel){
	  tree->Draw(Form("(16 * yield_%s * %f)/(%s):run>>hh_%s",dets[i].Data(),weights[j][i],denom[j].Data(),dets[i].Data()),
		     Form("%s %s",runCut[j].Data(),cut.Data()),"SAME");
	}
	else{
	  tree->Draw(Form("(yield_%s * %f):run>>hh_%s",dets[i].Data(),weights[j][i],dets[i].Data()),
		     Form("%s %s",runCut[j].Data(),cut.Data()),"SAME");
	}
		
      TH2F *htemp = (TH2F*)gPad->GetPrimitive(Form("hh_%s",dets[i].Data()));
      gStyle->SetMarkerSize(markerSize);
      tree->SetMarkerColor(colour[j]);
      htemp->SetMarkerColor(colour[j]);

      if (rel){
	htemp->SetTitle(Form("%s normalized weighted yield over time",dets[i].Data()));
// 	htemp->SetMarkerColor(colour[j]);
	htemp->SetMarkerStyle(20);
	htemp->SetMarkerSize(markerSize);
	htemp->GetYaxis()->SetTitle(Form("%s",dets[i].Data()));
	htemp->GetXaxis()->SetTitle(Form("Run Number"));
	//  			htemp->GetXaxis()->SetNdivisions(510);
      }
      else{
	htemp->SetTitle(Form("%s weighted yield over time (wien0)",dets[i].Data()));
// 	htemp->SetMarkerColor(colour[j]);
	htemp->SetMarkerStyle(20);
	htemp->SetMarkerSize(markerSize);
	htemp->GetYaxis()->SetTitle(Form("%s",dets[i].Data()));
	htemp->GetXaxis()->SetTitle(Form("Run Number"));
	//  			htemp->GetXaxis()->SetNdivisions(510);
      }

    }//for each region

  }//for each tube

//   c1->Modified();
  c1->Update();
  if(rel){
  c1->Print(Form("%srelWeightedYield.png",nameString.Data()));
  }
  else{
  c1->Print(Form("%sweightedYield.png",nameString.Data()));
  }


}
