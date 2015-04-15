//*****************************************************************************************************//
// Author      : Nuruzzaman on 08/06/2012
// Last Update : 08/07/2012
// 
//*****************************************************************************************************//

using namespace std;
#include "NurClass.h"

//usLumiAsymLongitudinal()

int main(Int_t argc,Char_t* argv[]){

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
//   printf ( "%sThe current date/time is: %s%s%s",blue,red,asctime(timeinfo),normal);

  // }
  // void plotTransverseN2Delta(){


  Double_t figSize = 2.0;

  Bool_t FIGURE1 = 1;
  Bool_t FIGURE2 = 1;
  Bool_t FIGURE3 = 1;

  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[4] ={1200*figSize,1000*figSize,1200*figSize,650*figSize};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t legendCoordinates[4] = {0.1,0.900,0.75,0.995};
  Double_t yScale[2] = {-10.0,10.0};

//   TFile *f = new TFile(Form("/home/nur/scratch/rootfiles/HYDROGEN-CELL_on_5+1_tree.root"));
//   if (!f->IsOpen()) {printf("%sFile not found. Exiting the program!\n%s",red,normal); exit(1); 
//   }

  TChain * tree = new TChain("tree");
  if(tree == NULL) {printf("%sUnable to find %stree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}

  tree->Add("/home/nur/scratch/rootfiles/HYDROGEN-CELL_on_5+1_tree.root");

  TChain * Hel_Tree = new TChain("Hel_Tree");
  if(Hel_Tree == NULL) {printf("%sUnable to find %stree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}
  Hel_Tree->Add("dirPlot/n-to-delta_6700_h_transverse_HYDROGEN-CELL_USLumiSum_Sum_modified_regression_off_on_5+1_RunletAvg_asym_run2_pass5.root");


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
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatColor(0);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.70);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gDirectory->Delete("*");

  /********************************************/
  /********************************************/


  Double_t hist_bin[1] = {100};
  Double_t hist_run[2] = {15700,16500};
  Double_t hist_runTrans[2] = {16128,16133};
  Double_t hist_y[2] = {-25.0,25};
  Double_t hist_y_ind[2] = {-40.0,40};
  Double_t markerSize[5] = {0.38,0.55,0.8,0.065,0.065};
  Double_t lineWidth[2] = {5.0,4.0};
  Int_t markerStyle[5] = {21,22,20,24,25};
  Double_t fit_range[6] = {15780,15850,15870,16000,16160,16400};


  TH2D* hist1  = new TH2D("hist1", "hist1", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y_ind[0],hist_y_ind[1]);
  TH2D* hist3  = new TH2D("hist3", "hist3", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y_ind[0],hist_y_ind[1]);
  TH2D* hist5  = new TH2D("hist5", "hist5", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y_ind[0],hist_y_ind[1]);
  TH2D* hist7  = new TH2D("hist7", "hist7", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y_ind[0],hist_y_ind[1]);

  TH2D* hist15  = new TH2D("hist15", "hist15", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y[0],hist_y[1]);
  TH2D* hist37  = new TH2D("hist37", "hist37", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y[0],hist_y[1]);
  TH2D* histSum  = new TH2D("histSum", "histSum", hist_bin[0], hist_run[0], hist_run[1], hist_bin[0],hist_y[0],hist_y[1]);

  TF1* fit1Hist1 =  new TF1("fit1Hist1","pol0",fit_range[0], fit_range[1]);
  TF1* fit1Hist2 =  new TF1("fit1Hist2","pol0",fit_range[2], fit_range[3]);
  TF1* fit1Hist3 =  new TF1("fit1Hist3","pol0",fit_range[4], fit_range[5]);

  TF1* fit3Hist1 =  new TF1("fit3Hist1","pol0",fit_range[0], fit_range[1]);
  TF1* fit3Hist2 =  new TF1("fit3Hist2","pol0",fit_range[2], fit_range[3]);
  TF1* fit3Hist3 =  new TF1("fit3Hist3","pol0",fit_range[4], fit_range[5]);

  TF1* fit5Hist1 =  new TF1("fit5Hist1","pol0",fit_range[0], fit_range[1]);
  TF1* fit5Hist2 =  new TF1("fit5Hist2","pol0",fit_range[2], fit_range[3]);
  TF1* fit5Hist3 =  new TF1("fit5Hist3","pol0",fit_range[4], fit_range[5]);

  TF1* fit7Hist1 =  new TF1("fit7Hist1","pol0",fit_range[0], fit_range[1]);
  TF1* fit7Hist2 =  new TF1("fit7Hist2","pol0",fit_range[2], fit_range[3]);
  TF1* fit7Hist3 =  new TF1("fit7Hist3","pol0",fit_range[4], fit_range[5]);

  TF1* fit15Hist1 =  new TF1("fit15Hist1","pol0",fit_range[0], fit_range[1]);
  TF1* fit15Hist2 =  new TF1("fit15Hist2","pol0",fit_range[2], fit_range[3]);
  TF1* fit15Hist3 =  new TF1("fit15Hist3","pol0",fit_range[4], fit_range[5]);

  TF1* fit37Hist1 =  new TF1("fit37Hist1","pol0",fit_range[0], fit_range[1]);
  TF1* fit37Hist2 =  new TF1("fit37Hist2","pol0",fit_range[2], fit_range[3]);
  TF1* fit37Hist3 =  new TF1("fit37Hist3","pol0",fit_range[4], fit_range[5]);

  TF1* fitSumHist1 =  new TF1("fitSumHist1","pol0",fit_range[0], fit_range[1]);
  TF1* fitSumHist2 =  new TF1("fitSumHist2","pol0",fit_range[2], fit_range[3]);
  TF1* fitSumHist3 =  new TF1("fitSumHist3","pol0",fit_range[4], fit_range[5]);


  Double_t fit1Para1_p0,fit1Para1_p0Error,fit1Para1_Chi,fit1Para1_NDF,fit1Para1_Prob;
  Double_t fit1Para2_p0,fit1Para2_p0Error,fit1Para2_Chi,fit1Para2_NDF,fit1Para2_Prob;
  Double_t fit1Para3_p0,fit1Para3_p0Error,fit1Para3_Chi,fit1Para3_NDF,fit1Para3_Prob;

  Double_t fit3Para1_p0,fit3Para1_p0Error,fit3Para1_Chi,fit3Para1_NDF,fit3Para1_Prob;
  Double_t fit3Para2_p0,fit3Para2_p0Error,fit3Para2_Chi,fit3Para2_NDF,fit3Para2_Prob;
  Double_t fit3Para3_p0,fit3Para3_p0Error,fit3Para3_Chi,fit3Para3_NDF,fit3Para3_Prob;

  Double_t fit5Para1_p0,fit5Para1_p0Error,fit5Para1_Chi,fit5Para1_NDF,fit5Para1_Prob;
  Double_t fit5Para2_p0,fit5Para2_p0Error,fit5Para2_Chi,fit5Para2_NDF,fit5Para2_Prob;
  Double_t fit5Para3_p0,fit5Para3_p0Error,fit5Para3_Chi,fit5Para3_NDF,fit5Para3_Prob;

  Double_t fit7Para1_p0,fit7Para1_p0Error,fit7Para1_Chi,fit7Para1_NDF,fit7Para1_Prob;
  Double_t fit7Para2_p0,fit7Para2_p0Error,fit7Para2_Chi,fit7Para2_NDF,fit7Para2_Prob;
  Double_t fit7Para3_p0,fit7Para3_p0Error,fit7Para3_Chi,fit7Para3_NDF,fit7Para3_Prob;

  Double_t fit15Para1_p0,fit15Para1_p0Error,fit15Para1_Chi,fit15Para1_NDF,fit15Para1_Prob;
  Double_t fit15Para2_p0,fit15Para2_p0Error,fit15Para2_Chi,fit15Para2_NDF,fit15Para2_Prob;
  Double_t fit15Para3_p0,fit15Para3_p0Error,fit15Para3_Chi,fit15Para3_NDF,fit15Para3_Prob;

  Double_t fit37Para1_p0,fit37Para1_p0Error,fit37Para1_Chi,fit37Para1_NDF,fit37Para1_Prob;
  Double_t fit37Para2_p0,fit37Para2_p0Error,fit37Para2_Chi,fit37Para2_NDF,fit37Para2_Prob;
  Double_t fit37Para3_p0,fit37Para3_p0Error,fit37Para3_Chi,fit37Para3_NDF,fit37Para3_Prob;

  Double_t fitSumPara1_p0,fitSumPara1_p0Error,fitSumPara1_Chi,fitSumPara1_NDF,fitSumPara1_Prob;
  Double_t fitSumPara2_p0,fitSumPara2_p0Error,fitSumPara2_Chi,fitSumPara2_NDF,fitSumPara2_Prob;
  Double_t fitSumPara3_p0,fitSumPara3_p0Error,fitSumPara3_Chi,fitSumPara3_NDF,fitSumPara3_Prob;

  /********************************************/

  TH2D* histTrans15  = new TH2D("histTrans15", "histTrans15", hist_bin[0], hist_runTrans[0], hist_runTrans[1], hist_bin[0],hist_y[0],hist_y[1]);
  TH2D* histTrans37  = new TH2D("histTrans37", "histTrans37", hist_bin[0], hist_runTrans[0], hist_runTrans[1], hist_bin[0],hist_y[0],hist_y[1]);
  TH2D* histTransSum  = new TH2D("histTransSum", "histTransSum", hist_bin[0], hist_runTrans[0], hist_runTrans[1], hist_bin[0],hist_y[0],hist_y[1]);

  TF1* fit15HistTrans =  new TF1("fit15HistTrans","pol0",hist_runTrans[0], hist_runTrans[1]);
  TF1* fit37HistTrans =  new TF1("fit37HistTrans","pol0",hist_runTrans[0], hist_runTrans[1]);
  TF1* fitSumHistTrans =  new TF1("fitSumHistTrans","pol0",hist_runTrans[0], hist_runTrans[1]);

  Double_t fitTrans15Para_p0,fitTrans15Para_p0Error,fitTrans15Para_Chi,fitTrans15Para_NDF,fitTrans15Para_Prob;
  Double_t fitTrans37Para_p0,fitTrans37Para_p0Error,fitTrans37Para_Chi,fitTrans37Para_NDF,fitTrans37Para_Prob;
  Double_t fitTransSumPara_p0,fitTransSumPara_p0Error,fitTransSumPara_Chi,fitTransSumPara_NDF,fitTransSumPara_Prob;



  /********************************************/
  /********************************************/
  if(FIGURE1){

  TString title1 = Form("USLumi Asymmetries from Longitudinal Running");

  TCanvas * canvas1 = new TCanvas("canvas1", title1,0,0,canvasSize[0],canvasSize[1]);
  canvas1->Draw();
  canvas1->SetBorderSize(0);
  canvas1->cd();
  TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  TLatex * t1 = new TLatex(0.06,0.3,Form("%s",title1.Data()));
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();
  pad2->Divide(1,2);


  pad2->cd(1);

  tree->Draw("asym_uslumi1_sum.value-asym_uslumi5_sum.value:run_number>>hist15",Form("asym_uslumi1_sum.n>0 && asym_uslumi5_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  hist15 = (TH2D *)gDirectory->Get("hist15");
  hist15->SetTitle("");
  hist15->SetXTitle("Run Number");
  hist15->SetYTitle("USLumi 1 - 5 Asym. [ppm]");
  hist15->GetYaxis()->CenterTitle();
  hist15->GetXaxis()->CenterTitle();
  hist15->GetYaxis()->SetRangeUser(hist_y[0], hist_y[1]);
  hist15->SetMarkerColor(kMagenta);
  hist15->SetMarkerStyle(markerStyle[0]);
  hist15->SetMarkerSize(markerSize[0]);
  hist15->Draw("");
  hist15->Fit("fit15Hist1","QR","");
  hist15->Fit("fit15Hist2","QR+","");
  hist15->Fit("fit15Hist3","QR+","");

  TF1* fit_15Hist1 = hist15->GetFunction("fit15Hist1");
  fit_15Hist1->SetLineColor(kRed+1);
  fit_15Hist1->SetLineWidth(lineWidth[0]);
  TF1* fit_15Hist2 = hist15->GetFunction("fit15Hist2");
  fit_15Hist2->SetLineColor(kGreen+2);
  fit_15Hist2->SetLineWidth(lineWidth[0]);
  TF1* fit_15Hist3 = hist15->GetFunction("fit15Hist3");
  fit_15Hist3->SetLineColor(kBlue+1);
  fit_15Hist3->SetLineWidth(lineWidth[0]);

  hist15->SetStats(0);

  fit15Para1_p0          =  fit15Hist1->GetParameter(0);
  fit15Para1_p0Error     =  fit15Hist1->GetParError(0);
  fit15Para1_Chi         =  fit15Hist1->GetChisquare();
  fit15Para1_NDF         =  fit15Hist1->GetNDF();
  fit15Para1_Prob        =  fit15Hist1->GetProb();

  fit15Para2_p0          =  fit15Hist2->GetParameter(0);
  fit15Para2_p0Error     =  fit15Hist2->GetParError(0);
  fit15Para2_Chi         =  fit15Hist2->GetChisquare();
  fit15Para2_NDF         =  fit15Hist2->GetNDF();
  fit15Para2_Prob        =  fit15Hist2->GetProb();

  fit15Para3_p0          =  fit15Hist3->GetParameter(0);
  fit15Para3_p0Error     =  fit15Hist3->GetParError(0);
  fit15Para3_Chi         =  fit15Hist3->GetChisquare();
  fit15Para3_NDF         =  fit15Hist3->GetNDF();
  fit15Para3_Prob        =  fit15Hist3->GetProb();

  cout<<red<<"fit15Para1 : "<<fit15Para1_p0<<"+-"<<fit15Para1_p0Error<<","<<fit15Para1_Chi<<","<<fit15Para1_NDF<<","<<fit15Para1_Prob<<normal<<endl;
  cout<<green<<"fit15Para2 : "<<fit15Para2_p0<<"+-"<<fit15Para2_p0Error<<","<<fit15Para2_Chi<<","<<fit15Para2_NDF<<","<<fit15Para2_Prob<<normal<<endl;
  cout<<blue<<"fit15Para3 : "<<fit15Para3_p0<<"+-"<<fit15Para3_p0Error<<","<<fit15Para3_Chi<<","<<fit15Para3_NDF<<","<<fit15Para3_Prob<<normal<<endl;

  TLatex* t15A_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.88,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fit15Para1_p0,fit15Para1_p0Error));
  t15A_1->SetTextSize(markerSize[3]);
  t15A_1->SetTextColor(kRed+1);
  TLatex* t15P_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit15Para1_Chi,fit15Para1_NDF,fit15Para1_Prob));
  t15P_1->SetTextSize(markerSize[3]);
  t15P_1->SetTextColor(kRed+1);

  TLatex* t15A_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.71,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fit15Para2_p0,fit15Para2_p0Error));
  t15A_2->SetTextSize(markerSize[3]);
  t15A_2->SetTextColor(kGreen+2);
  TLatex* t15P_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit15Para2_Chi,fit15Para2_NDF,fit15Para2_Prob));
  t15P_2->SetTextSize(markerSize[3]);
  t15P_2->SetTextColor(kGreen+2);

  TLatex* t15A_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.88,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fit15Para3_p0,fit15Para3_p0Error));
  t15A_3->SetTextSize(markerSize[3]);
  t15A_3->SetTextColor(kBlue+1);
  TLatex* t15P_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit15Para3_Chi,fit15Para3_NDF,fit15Para3_Prob));
  t15P_3->SetTextSize(markerSize[3]);
  t15P_3->SetTextColor(kBlue+1);

  t15A_1->Draw("same"); t15P_1->Draw("same");
  t15A_2->Draw("same"); t15P_2->Draw("same");
  t15A_3->Draw("same"); t15P_3->Draw("same");

  gPad->Update();



  pad2->cd(2);

//   tree->Draw("asym_uslumi3_sum.value-asym_uslumi7_sum.value:run_number",Form("asym_uslumi3_sum.n>0 && asym_uslumi7_sum.n>0 && run_number>15800 && run_number<16400"));
  tree->Draw("asym_uslumi3_sum.value-asym_uslumi7_sum.value:run_number>>hist37",Form("asym_uslumi3_sum.n>0 && asym_uslumi7_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  hist37 = (TH2D *)gDirectory->Get("hist37");
  hist37->SetTitle("");
  hist37->SetXTitle("Run Number");
  hist37->SetYTitle("USLumi 3 - 7 Asym. [ppm]");
  hist37->GetYaxis()->CenterTitle();
  hist37->GetXaxis()->CenterTitle();
  hist37->GetYaxis()->SetRangeUser(hist_y[0], hist_y[1]);
  hist37->SetMarkerColor(kOrange+9);
  hist37->SetMarkerStyle(markerStyle[1]);
  hist37->SetMarkerSize(markerSize[1]);
  hist37->Draw("");
  hist37->Fit("fit37Hist1","QR","");
  hist37->Fit("fit37Hist2","QR+","");
  hist37->Fit("fit37Hist3","QR+","");

  TF1* fit_37Hist1 = hist37->GetFunction("fit37Hist1");
  fit_37Hist1->SetLineColor(kRed+1);
  fit_37Hist1->SetLineWidth(lineWidth[0]);
  TF1* fit_37Hist2 = hist37->GetFunction("fit37Hist2");
  fit_37Hist2->SetLineColor(kGreen+2);
  fit_37Hist2->SetLineWidth(lineWidth[0]);
  TF1* fit_37Hist3 = hist37->GetFunction("fit37Hist3");
  fit_37Hist3->SetLineColor(kBlue+1);
  fit_37Hist3->SetLineWidth(lineWidth[0]);

  hist37->SetStats(0);

  fit37Para1_p0          =  fit37Hist1->GetParameter(0);
  fit37Para1_p0Error     =  fit37Hist1->GetParError(0);
  fit37Para1_Chi         =  fit37Hist1->GetChisquare();
  fit37Para1_NDF         =  fit37Hist1->GetNDF();
  fit37Para1_Prob        =  fit37Hist1->GetProb();

  fit37Para2_p0          =  fit37Hist2->GetParameter(0);
  fit37Para2_p0Error     =  fit37Hist2->GetParError(0);
  fit37Para2_Chi         =  fit37Hist2->GetChisquare();
  fit37Para2_NDF         =  fit37Hist2->GetNDF();
  fit37Para2_Prob        =  fit37Hist2->GetProb();

  fit37Para3_p0          =  fit37Hist3->GetParameter(0);
  fit37Para3_p0Error     =  fit37Hist3->GetParError(0);
  fit37Para3_Chi         =  fit37Hist3->GetChisquare();
  fit37Para3_NDF         =  fit37Hist3->GetNDF();
  fit37Para3_Prob        =  fit37Hist3->GetProb();

  cout<<red<<"fit37Para1 : "<<fit37Para1_p0<<"+-"<<fit37Para1_p0Error<<","<<fit37Para1_Chi<<","<<fit37Para1_NDF<<","<<fit37Para1_Prob<<normal<<endl;
  cout<<green<<"fit37Para2 : "<<fit37Para2_p0<<"+-"<<fit37Para2_p0Error<<","<<fit37Para2_Chi<<","<<fit37Para2_NDF<<","<<fit37Para2_Prob<<normal<<endl;
  cout<<blue<<"fit37Para3 : "<<fit37Para3_p0<<"+-"<<fit37Para3_p0Error<<","<<fit37Para3_Chi<<","<<fit37Para3_NDF<<","<<fit37Para3_Prob<<normal<<endl;

  TLatex* t37A_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.88,Form("A_{USLumi3-7} = %1.2f #pm %1.2f",fit37Para1_p0,fit37Para1_p0Error));
  t37A_1->SetTextSize(markerSize[3]);
  t37A_1->SetTextColor(kRed+1);
  TLatex* t37P_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit37Para1_Chi,fit37Para1_NDF,fit37Para1_Prob));
  t37P_1->SetTextSize(markerSize[3]);
  t37P_1->SetTextColor(kRed+1);

  TLatex* t37A_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.71,Form("A_{USLumi3-7} = %1.2f #pm %1.2f",fit37Para2_p0,fit37Para2_p0Error));
  t37A_2->SetTextSize(markerSize[3]);
  t37A_2->SetTextColor(kGreen+2);
  TLatex* t37P_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit37Para2_Chi,fit37Para2_NDF,fit37Para2_Prob));
  t37P_2->SetTextSize(markerSize[3]);
  t37P_2->SetTextColor(kGreen+2);

  TLatex* t37A_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.88,Form("A_{USLumi3-7} = %1.2f #pm %1.2f",fit37Para3_p0,fit37Para3_p0Error));
  t37A_3->SetTextSize(markerSize[3]);
  t37A_3->SetTextColor(kBlue+1);
  TLatex* t37P_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit37Para3_Chi,fit37Para3_NDF,fit37Para3_Prob));
  t37P_3->SetTextSize(markerSize[3]);
  t37P_3->SetTextColor(kBlue+1);

  t37A_1->Draw("same"); t37P_1->Draw("same");
  t37A_2->Draw("same"); t37P_2->Draw("same");
  t37A_3->Draw("same"); t37P_3->Draw("same");


  gPad->Update();


  TString saveSummaryPlot1 = Form("dirPlot/resultPlot/USLumiOctantAsymLongitudinal");

  canvas1->Update();
  canvas1->Print(saveSummaryPlot1+".png");

  }

  /********************************************/
  /********************************************/


  if(FIGURE2){


  TString title2 = Form("USLumi Asymmetries from Longitudinal Running");

  TCanvas * canvas2 = new TCanvas("canvas2", title2,0,0,canvasSize[0],canvasSize[1]);
  canvas2->Draw();
  canvas2->SetBorderSize(0);
  canvas2->cd();
  TPad*pad21 = new TPad("pad21","pad21",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad22 = new TPad("pad22","pad22",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad21->SetFillColor(kWhite);
  pad21->Draw();
  pad22->Draw();
  pad21->cd();
  TLatex * t2 = new TLatex(0.06,0.3,Form("%s",title2.Data()));
  t2->SetTextSize(0.5);
  t2->Draw();
  pad22->cd();
  pad22->Divide(2,3);



  pad22->cd(1);

  tree->Draw("asym_uslumi1_sum.value:run_number>>hist1",Form("asym_uslumi1_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  hist1 = (TH2D *)gDirectory->Get("hist1");
  hist1->SetTitle("");
  hist1->SetXTitle("Run Number");
  hist1->SetYTitle("USLumi 1 Asym. [ppm]");
  hist1->GetYaxis()->CenterTitle();
  hist1->GetXaxis()->CenterTitle();
  hist1->GetYaxis()->SetRangeUser(hist_y_ind[0], hist_y_ind[1]);
  hist1->SetMarkerColor(kBlack);
  hist1->SetMarkerStyle(markerStyle[2]);
  hist1->SetMarkerSize(markerSize[0]);
  hist1->Draw("");
  hist1->Fit("fit1Hist1","QR","");
  hist1->Fit("fit1Hist2","QR+","");
  hist1->Fit("fit1Hist3","QR+","");

  TF1* fit_1Hist1 = hist1->GetFunction("fit1Hist1");
  fit_1Hist1->SetLineColor(kRed+1);
  fit_1Hist1->SetLineWidth(lineWidth[0]);
  TF1* fit_1Hist2 = hist1->GetFunction("fit1Hist2");
  fit_1Hist2->SetLineColor(kGreen+2);
  fit_1Hist2->SetLineWidth(lineWidth[0]);
  TF1* fit_1Hist3 = hist1->GetFunction("fit1Hist3");
  fit_1Hist3->SetLineColor(kBlue+1);
  fit_1Hist3->SetLineWidth(lineWidth[0]);

  hist1->SetStats(0);

  fit1Para1_p0          =  fit1Hist1->GetParameter(0);
  fit1Para1_p0Error     =  fit1Hist1->GetParError(0);
  fit1Para1_Chi         =  fit1Hist1->GetChisquare();
  fit1Para1_NDF         =  fit1Hist1->GetNDF();
  fit1Para1_Prob        =  fit1Hist1->GetProb();

  fit1Para2_p0          =  fit1Hist2->GetParameter(0);
  fit1Para2_p0Error     =  fit1Hist2->GetParError(0);
  fit1Para2_Chi         =  fit1Hist2->GetChisquare();
  fit1Para2_NDF         =  fit1Hist2->GetNDF();
  fit1Para2_Prob        =  fit1Hist2->GetProb();

  fit1Para3_p0          =  fit1Hist3->GetParameter(0);
  fit1Para3_p0Error     =  fit1Hist3->GetParError(0);
  fit1Para3_Chi         =  fit1Hist3->GetChisquare();
  fit1Para3_NDF         =  fit1Hist3->GetNDF();
  fit1Para3_Prob        =  fit1Hist3->GetProb();

  cout<<red<<"fit1Para1 : "<<fit1Para1_p0<<"+-"<<fit1Para1_p0Error<<","<<fit1Para1_Chi<<","<<fit1Para1_NDF<<","<<fit1Para1_Prob<<normal<<endl;
  cout<<green<<"fit1Para2 : "<<fit1Para2_p0<<"+-"<<fit1Para2_p0Error<<","<<fit1Para2_Chi<<","<<fit1Para2_NDF<<","<<fit1Para2_Prob<<normal<<endl;
  cout<<blue<<"fit1Para3 : "<<fit1Para3_p0<<"+-"<<fit1Para3_p0Error<<","<<fit1Para3_Chi<<","<<fit1Para3_NDF<<","<<fit1Para3_Prob<<normal<<endl;

  TLatex* t1A_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.88,Form("A_{USLumi1} = %1.2f #pm %1.2f",fit1Para1_p0,fit1Para1_p0Error));
  t1A_1->SetTextSize(markerSize[3]);
  t1A_1->SetTextColor(kRed+1);
  TLatex* t1P_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit1Para1_Chi,fit1Para1_NDF,fit1Para1_Prob));
  t1P_1->SetTextSize(markerSize[3]);
  t1P_1->SetTextColor(kRed+1);

  TLatex* t1A_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.71,Form("A_{USLumi1} = %1.2f #pm %1.2f",fit1Para2_p0,fit1Para2_p0Error));
  t1A_2->SetTextSize(markerSize[3]);
  t1A_2->SetTextColor(kGreen+2);
  TLatex* t1P_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit1Para2_Chi,fit1Para2_NDF,fit1Para2_Prob));
  t1P_2->SetTextSize(markerSize[3]);
  t1P_2->SetTextColor(kGreen+2);

  TLatex* t1A_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.88,Form("A_{USLumi1} = %1.2f #pm %1.2f",fit1Para3_p0,fit1Para3_p0Error));
  t1A_3->SetTextSize(markerSize[3]);
  t1A_3->SetTextColor(kBlue+1);
  TLatex* t1P_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit1Para3_Chi,fit1Para3_NDF,fit1Para3_Prob));
  t1P_3->SetTextSize(markerSize[3]);
  t1P_3->SetTextColor(kBlue+1);

  t1A_1->Draw("same"); t1P_1->Draw("same");
  t1A_2->Draw("same"); t1P_2->Draw("same");
  t1A_3->Draw("same"); t1P_3->Draw("same");

  gPad->Update();



  pad22->cd(2);

  tree->Draw("asym_uslumi3_sum.value:run_number>>hist3",Form("asym_uslumi3_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  hist3 = (TH2D *)gDirectory->Get("hist3");
  hist3->SetTitle("");
  hist3->SetXTitle("Run Number");
  hist3->SetYTitle("USLumi 3 Asym. [ppm]");
  hist3->GetYaxis()->CenterTitle();
  hist3->GetXaxis()->CenterTitle();
  hist3->GetYaxis()->SetRangeUser(hist_y_ind[0], hist_y_ind[1]);
  hist3->SetMarkerColor(kBlack);
  hist3->SetMarkerStyle(markerStyle[2]);
  hist3->SetMarkerSize(markerSize[0]);
  hist3->Draw("");
  hist3->Fit("fit3Hist1","QR","");
  hist3->Fit("fit3Hist2","QR+","");
  hist3->Fit("fit3Hist3","QR+","");

  TF1* fit_3Hist1 = hist3->GetFunction("fit3Hist1");
  fit_3Hist1->SetLineColor(kRed+1);
  fit_3Hist1->SetLineWidth(lineWidth[0]);
  TF1* fit_3Hist2 = hist3->GetFunction("fit3Hist2");
  fit_3Hist2->SetLineColor(kGreen+2);
  fit_3Hist2->SetLineWidth(lineWidth[0]);
  TF1* fit_3Hist3 = hist3->GetFunction("fit3Hist3");
  fit_3Hist3->SetLineColor(kBlue+1);
  fit_3Hist3->SetLineWidth(lineWidth[0]);

  hist3->SetStats(0);

  fit3Para1_p0          =  fit3Hist1->GetParameter(0);
  fit3Para1_p0Error     =  fit3Hist1->GetParError(0);
  fit3Para1_Chi         =  fit3Hist1->GetChisquare();
  fit3Para1_NDF         =  fit3Hist1->GetNDF();
  fit3Para1_Prob        =  fit3Hist1->GetProb();

  fit3Para2_p0          =  fit3Hist2->GetParameter(0);
  fit3Para2_p0Error     =  fit3Hist2->GetParError(0);
  fit3Para2_Chi         =  fit3Hist2->GetChisquare();
  fit3Para2_NDF         =  fit3Hist2->GetNDF();
  fit3Para2_Prob        =  fit3Hist2->GetProb();

  fit3Para3_p0          =  fit3Hist3->GetParameter(0);
  fit3Para3_p0Error     =  fit3Hist3->GetParError(0);
  fit3Para3_Chi         =  fit3Hist3->GetChisquare();
  fit3Para3_NDF         =  fit3Hist3->GetNDF();
  fit3Para3_Prob        =  fit3Hist3->GetProb();

  cout<<red<<"fit3Para1 : "<<fit3Para1_p0<<"+-"<<fit3Para1_p0Error<<","<<fit3Para1_Chi<<","<<fit3Para1_NDF<<","<<fit3Para1_Prob<<normal<<endl;
  cout<<green<<"fit3Para2 : "<<fit3Para2_p0<<"+-"<<fit3Para2_p0Error<<","<<fit3Para2_Chi<<","<<fit3Para2_NDF<<","<<fit3Para2_Prob<<normal<<endl;
  cout<<blue<<"fit3Para3 : "<<fit3Para3_p0<<"+-"<<fit3Para3_p0Error<<","<<fit3Para3_Chi<<","<<fit3Para3_NDF<<","<<fit3Para3_Prob<<normal<<endl;

  TLatex* t3A_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.88,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit3Para1_p0,fit3Para1_p0Error));
  t3A_1->SetTextSize(markerSize[3]);
  t3A_1->SetTextColor(kRed+1);
  TLatex* t3P_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit3Para1_Chi,fit3Para1_NDF,fit3Para1_Prob));
  t3P_1->SetTextSize(markerSize[3]);
  t3P_1->SetTextColor(kRed+1);

  TLatex* t3A_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.71,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit3Para2_p0,fit3Para2_p0Error));
  t3A_2->SetTextSize(markerSize[3]);
  t3A_2->SetTextColor(kGreen+2);
  TLatex* t3P_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit3Para2_Chi,fit3Para2_NDF,fit3Para2_Prob));
  t3P_2->SetTextSize(markerSize[3]);
  t3P_2->SetTextColor(kGreen+2);

  TLatex* t3A_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.88,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit3Para3_p0,fit3Para3_p0Error));
  t3A_3->SetTextSize(markerSize[3]);
  t3A_3->SetTextColor(kBlue+1);
  TLatex* t3P_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit3Para3_Chi,fit3Para3_NDF,fit3Para3_Prob));
  t3P_3->SetTextSize(markerSize[3]);
  t3P_3->SetTextColor(kBlue+1);

  t3A_1->Draw("same"); t3P_1->Draw("same");
  t3A_2->Draw("same"); t3P_2->Draw("same");
  t3A_3->Draw("same"); t3P_3->Draw("same");

  gPad->Update();



  pad22->cd(3);

  tree->Draw("asym_uslumi5_sum.value:run_number>>hist5",Form("asym_uslumi5_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  hist5 = (TH2D *)gDirectory->Get("hist5");
  hist5->SetTitle("");
  hist5->SetXTitle("Run Number");
  hist5->SetYTitle("USLumi 5 Asym. [ppm]");
  hist5->GetYaxis()->CenterTitle();
  hist5->GetXaxis()->CenterTitle();
  hist5->GetYaxis()->SetRangeUser(hist_y_ind[0], hist_y_ind[1]);
  hist5->SetMarkerColor(kBlack);
  hist5->SetMarkerStyle(markerStyle[2]);
  hist5->SetMarkerSize(markerSize[0]);
  hist5->Draw("");
  hist5->Fit("fit5Hist1","QR","");
  hist5->Fit("fit5Hist2","QR+","");
  hist5->Fit("fit5Hist3","QR+","");

  TF1* fit_5Hist1 = hist5->GetFunction("fit5Hist1");
  fit_5Hist1->SetLineColor(kRed+1);
  fit_5Hist1->SetLineWidth(lineWidth[0]);
  TF1* fit_5Hist2 = hist5->GetFunction("fit5Hist2");
  fit_5Hist2->SetLineColor(kGreen+2);
  fit_5Hist2->SetLineWidth(lineWidth[0]);
  TF1* fit_5Hist3 = hist5->GetFunction("fit5Hist3");
  fit_5Hist3->SetLineColor(kBlue+1);
  fit_5Hist3->SetLineWidth(lineWidth[0]);

  hist5->SetStats(0);

  fit5Para1_p0          =  fit5Hist1->GetParameter(0);
  fit5Para1_p0Error     =  fit5Hist1->GetParError(0);
  fit5Para1_Chi         =  fit5Hist1->GetChisquare();
  fit5Para1_NDF         =  fit5Hist1->GetNDF();
  fit5Para1_Prob        =  fit5Hist1->GetProb();

  fit5Para2_p0          =  fit5Hist2->GetParameter(0);
  fit5Para2_p0Error     =  fit5Hist2->GetParError(0);
  fit5Para2_Chi         =  fit5Hist2->GetChisquare();
  fit5Para2_NDF         =  fit5Hist2->GetNDF();
  fit5Para2_Prob        =  fit5Hist2->GetProb();

  fit5Para3_p0          =  fit5Hist3->GetParameter(0);
  fit5Para3_p0Error     =  fit5Hist3->GetParError(0);
  fit5Para3_Chi         =  fit5Hist3->GetChisquare();
  fit5Para3_NDF         =  fit5Hist3->GetNDF();
  fit5Para3_Prob        =  fit5Hist3->GetProb();

  cout<<red<<"fit5Para1 : "<<fit5Para1_p0<<"+-"<<fit5Para1_p0Error<<","<<fit5Para1_Chi<<","<<fit5Para1_NDF<<","<<fit5Para1_Prob<<normal<<endl;
  cout<<green<<"fit5Para2 : "<<fit5Para2_p0<<"+-"<<fit5Para2_p0Error<<","<<fit5Para2_Chi<<","<<fit5Para2_NDF<<","<<fit5Para2_Prob<<normal<<endl;
  cout<<blue<<"fit5Para3 : "<<fit5Para3_p0<<"+-"<<fit5Para3_p0Error<<","<<fit5Para3_Chi<<","<<fit5Para3_NDF<<","<<fit5Para3_Prob<<normal<<endl;

  TLatex* t5A_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.88,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit5Para1_p0,fit5Para1_p0Error));
  t5A_1->SetTextSize(markerSize[3]);
  t5A_1->SetTextColor(kRed+1);
  TLatex* t5P_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit5Para1_Chi,fit5Para1_NDF,fit5Para1_Prob));
  t5P_1->SetTextSize(markerSize[3]);
  t5P_1->SetTextColor(kRed+1);

  TLatex* t5A_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.71,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit5Para2_p0,fit5Para2_p0Error));
  t5A_2->SetTextSize(markerSize[3]);
  t5A_2->SetTextColor(kGreen+2);
  TLatex* t5P_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit5Para2_Chi,fit5Para2_NDF,fit5Para2_Prob));
  t5P_2->SetTextSize(markerSize[3]);
  t5P_2->SetTextColor(kGreen+2);

  TLatex* t5A_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.88,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit5Para3_p0,fit5Para3_p0Error));
  t5A_3->SetTextSize(markerSize[3]);
  t5A_3->SetTextColor(kBlue+1);
  TLatex* t5P_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit5Para3_Chi,fit5Para3_NDF,fit5Para3_Prob));
  t5P_3->SetTextSize(markerSize[3]);
  t5P_3->SetTextColor(kBlue+1);

  t5A_1->Draw("same"); t5P_1->Draw("same");
  t5A_2->Draw("same"); t5P_2->Draw("same");
  t5A_3->Draw("same"); t5P_3->Draw("same");

  gPad->Update();


  pad22->cd(4);

  tree->Draw("asym_uslumi7_sum.value:run_number>>hist7",Form("asym_uslumi7_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  hist7 = (TH2D *)gDirectory->Get("hist7");
  hist7->SetTitle("");
  hist7->SetXTitle("Run Number");
  hist7->SetYTitle("USLumi 7 Asym. [ppm]");
  hist7->GetYaxis()->CenterTitle();
  hist7->GetXaxis()->CenterTitle();
  hist7->GetYaxis()->SetRangeUser(hist_y_ind[0], hist_y_ind[1]);
  hist7->SetMarkerColor(kBlack);
  hist7->SetMarkerStyle(markerStyle[2]);
  hist7->SetMarkerSize(markerSize[0]);
  hist7->Draw("");
  hist7->Fit("fit7Hist1","QR","");
  hist7->Fit("fit7Hist2","QR+","");
  hist7->Fit("fit7Hist3","QR+","");

  TF1* fit_7Hist1 = hist7->GetFunction("fit7Hist1");
  fit_7Hist1->SetLineColor(kRed+1);
  fit_7Hist1->SetLineWidth(lineWidth[0]);
  TF1* fit_7Hist2 = hist7->GetFunction("fit7Hist2");
  fit_7Hist2->SetLineColor(kGreen+2);
  fit_7Hist2->SetLineWidth(lineWidth[0]);
  TF1* fit_7Hist3 = hist7->GetFunction("fit7Hist3");
  fit_7Hist3->SetLineColor(kBlue+1);
  fit_7Hist3->SetLineWidth(lineWidth[0]);

  hist7->SetStats(0);

  fit7Para1_p0          =  fit7Hist1->GetParameter(0);
  fit7Para1_p0Error     =  fit7Hist1->GetParError(0);
  fit7Para1_Chi         =  fit7Hist1->GetChisquare();
  fit7Para1_NDF         =  fit7Hist1->GetNDF();
  fit7Para1_Prob        =  fit7Hist1->GetProb();

  fit7Para2_p0          =  fit7Hist2->GetParameter(0);
  fit7Para2_p0Error     =  fit7Hist2->GetParError(0);
  fit7Para2_Chi         =  fit7Hist2->GetChisquare();
  fit7Para2_NDF         =  fit7Hist2->GetNDF();
  fit7Para2_Prob        =  fit7Hist2->GetProb();

  fit7Para3_p0          =  fit7Hist3->GetParameter(0);
  fit7Para3_p0Error     =  fit7Hist3->GetParError(0);
  fit7Para3_Chi         =  fit7Hist3->GetChisquare();
  fit7Para3_NDF         =  fit7Hist3->GetNDF();
  fit7Para3_Prob        =  fit7Hist3->GetProb();

  cout<<red<<"fit7Para1 : "<<fit7Para1_p0<<"+-"<<fit7Para1_p0Error<<","<<fit7Para1_Chi<<","<<fit7Para1_NDF<<","<<fit7Para1_Prob<<normal<<endl;
  cout<<green<<"fit7Para2 : "<<fit7Para2_p0<<"+-"<<fit7Para2_p0Error<<","<<fit7Para2_Chi<<","<<fit7Para2_NDF<<","<<fit7Para2_Prob<<normal<<endl;
  cout<<blue<<"fit7Para3 : "<<fit7Para3_p0<<"+-"<<fit7Para3_p0Error<<","<<fit7Para3_Chi<<","<<fit7Para3_NDF<<","<<fit7Para3_Prob<<normal<<endl;

  TLatex* t7A_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.88,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit7Para1_p0,fit7Para1_p0Error));
  t7A_1->SetTextSize(markerSize[3]);
  t7A_1->SetTextColor(kRed+1);
  TLatex* t7P_1 = new TLatex(fit_range[0]-30,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit7Para1_Chi,fit7Para1_NDF,fit7Para1_Prob));
  t7P_1->SetTextSize(markerSize[3]);
  t7P_1->SetTextColor(kRed+1);

  TLatex* t7A_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.71,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit7Para2_p0,fit7Para2_p0Error));
  t7A_2->SetTextSize(markerSize[3]);
  t7A_2->SetTextColor(kGreen+2);
  TLatex* t7P_2 = new TLatex(fit_range[2]-00,hist_y_ind[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit7Para2_Chi,fit7Para2_NDF,fit7Para2_Prob));
  t7P_2->SetTextSize(markerSize[3]);
  t7P_2->SetTextColor(kGreen+2);

  TLatex* t7A_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.88,Form("A_{USLumi3} = %1.2f #pm %1.2f",fit7Para3_p0,fit7Para3_p0Error));
  t7A_3->SetTextSize(markerSize[3]);
  t7A_3->SetTextColor(kBlue+1);
  TLatex* t7P_3 = new TLatex(fit_range[4]+20,hist_y_ind[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit7Para3_Chi,fit7Para3_NDF,fit7Para3_Prob));
  t7P_3->SetTextSize(markerSize[3]);
  t7P_3->SetTextColor(kBlue+1);

  t7A_1->Draw("same"); t7P_1->Draw("same");
  t7A_2->Draw("same"); t7P_2->Draw("same");
  t7A_3->Draw("same"); t7P_3->Draw("same");

  gPad->Update();


  pad22->cd(5);

  tree->Draw("asym_uslumi_sum.value:run_number>>histSum",Form("asym_uslumi_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
  histSum = (TH2D *)gDirectory->Get("histSum");
  histSum->SetTitle("");
  histSum->SetXTitle("Run Number");
  histSum->SetYTitle("USLumi Sum Asym. [ppm]");
  histSum->GetYaxis()->CenterTitle();
  histSum->GetXaxis()->CenterTitle();
  histSum->GetYaxis()->SetRangeUser(hist_y[0], hist_y[1]);
  histSum->SetMarkerColor(kBlack);
  histSum->SetMarkerStyle(markerStyle[0]);
  histSum->SetMarkerSize(markerSize[0]);
  histSum->Draw("");
  histSum->Fit("fitSumHist1","QR","");
  histSum->Fit("fitSumHist2","QR+","");
  histSum->Fit("fitSumHist3","QR+","");

  TF1* fit_SumHist1 = histSum->GetFunction("fitSumHist1");
  fit_SumHist1->SetLineColor(kRed+1);
  fit_SumHist1->SetLineWidth(lineWidth[0]);
  TF1* fit_SumHist2 = histSum->GetFunction("fitSumHist2");
  fit_SumHist2->SetLineColor(kGreen+2);
  fit_SumHist2->SetLineWidth(lineWidth[0]);
  TF1* fit_SumHist3 = histSum->GetFunction("fitSumHist3");
  fit_SumHist3->SetLineColor(kBlue+1);
  fit_SumHist3->SetLineWidth(lineWidth[0]);

  histSum->SetStats(0);

  fitSumPara1_p0          =  fitSumHist1->GetParameter(0);
  fitSumPara1_p0Error     =  fitSumHist1->GetParError(0);
  fitSumPara1_Chi         =  fitSumHist1->GetChisquare();
  fitSumPara1_NDF         =  fitSumHist1->GetNDF();
  fitSumPara1_Prob        =  fitSumHist1->GetProb();

  fitSumPara2_p0          =  fitSumHist2->GetParameter(0);
  fitSumPara2_p0Error     =  fitSumHist2->GetParError(0);
  fitSumPara2_Chi         =  fitSumHist2->GetChisquare();
  fitSumPara2_NDF         =  fitSumHist2->GetNDF();
  fitSumPara2_Prob        =  fitSumHist2->GetProb();

  fitSumPara3_p0          =  fitSumHist3->GetParameter(0);
  fitSumPara3_p0Error     =  fitSumHist3->GetParError(0);
  fitSumPara3_Chi         =  fitSumHist3->GetChisquare();
  fitSumPara3_NDF         =  fitSumHist3->GetNDF();
  fitSumPara3_Prob        =  fitSumHist3->GetProb();

  cout<<red<<"fitSumPara1 : "<<fitSumPara1_p0<<"+-"<<fitSumPara1_p0Error<<","<<fitSumPara1_Chi<<","<<fitSumPara1_NDF<<","<<fitSumPara1_Prob<<normal<<endl;
  cout<<green<<"fitSumPara2 : "<<fitSumPara2_p0<<"+-"<<fitSumPara2_p0Error<<","<<fitSumPara2_Chi<<","<<fitSumPara2_NDF<<","<<fitSumPara2_Prob<<normal<<endl;
  cout<<blue<<"fitSumPara3 : "<<fitSumPara3_p0<<"+-"<<fitSumPara3_p0Error<<","<<fitSumPara3_Chi<<","<<fitSumPara3_NDF<<","<<fitSumPara3_Prob<<normal<<endl;

  TLatex* tSumA_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.88,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fitSumPara1_p0,fitSumPara1_p0Error));
  tSumA_1->SetTextSize(markerSize[3]);
  tSumA_1->SetTextColor(kRed+1);
  TLatex* tSumP_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fitSumPara1_Chi,fitSumPara1_NDF,fitSumPara1_Prob));
  tSumP_1->SetTextSize(markerSize[3]);
  tSumP_1->SetTextColor(kRed+1);

  TLatex* tSumA_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.71,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fitSumPara2_p0,fitSumPara2_p0Error));
  tSumA_2->SetTextSize(markerSize[3]);
  tSumA_2->SetTextColor(kGreen+2);
  TLatex* tSumP_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fitSumPara2_Chi,fitSumPara2_NDF,fitSumPara2_Prob));
  tSumP_2->SetTextSize(markerSize[3]);
  tSumP_2->SetTextColor(kGreen+2);

  TLatex* tSumA_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.88,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fitSumPara3_p0,fitSumPara3_p0Error));
  tSumA_3->SetTextSize(markerSize[3]);
  tSumA_3->SetTextColor(kBlue+1);
  TLatex* tSumP_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fitSumPara3_Chi,fitSumPara3_NDF,fitSumPara3_Prob));
  tSumP_3->SetTextSize(markerSize[3]);
  tSumP_3->SetTextColor(kBlue+1);

  tSumA_1->Draw("same"); tSumP_1->Draw("same");
  tSumA_2->Draw("same"); tSumP_2->Draw("same");
  tSumA_3->Draw("same"); tSumP_3->Draw("same");

  gPad->Update();




  /********************************************/

  const int k =4;

  Double_t x[k],errx[k];
  Double_t value_1[k] = {fit1Para1_p0,fit3Para1_p0,fit5Para1_p0,fit7Para1_p0};
  Double_t error_1[k] = {fit1Para1_p0Error,fit3Para1_p0Error,fit5Para1_p0Error,fit7Para1_p0Error};
  Double_t value_2[k] = {fit1Para2_p0,fit3Para2_p0,fit5Para2_p0,fit7Para2_p0};
  Double_t error_2[k] = {fit1Para2_p0Error,fit3Para2_p0Error,fit5Para2_p0Error,fit7Para2_p0Error};
  Double_t value_3[k] = {fit1Para3_p0,fit3Para3_p0,fit5Para3_p0,fit7Para3_p0};
  Double_t error_3[k] = {fit1Para3_p0Error,fit3Para3_p0Error,fit5Para3_p0Error,fit7Para3_p0Error};

  for(Int_t i =0;i<k;i++){
    x[i] = i+1; errx[i] = 0.0;
  }

  Double_t yCor[2] = {-10,15};


  // // fit
  TF1 *fit_fun = new TF1("fit_fun","[0]*sin((pi/180)*(45*(x-1) + [1])) + [2]",0.9,4.1);
//   TF1 *fit_fun = new TF1("fit_fun","[0]*sin((pi/180)*(45*(x-1) )) + [1]",1,4);
//   TF1 *fit_fun = new TF1("fit_fun","[0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))) + [2]",1,4);
  TF1 *fit_fun3 = new TF1("fit_fun3","[0]*sin(x + [1]) + [2]",0.9,4.1);
  fit_fun->SetParameters(0.0,0.0,0.0);
  fit_fun->SetParNames("#DeltaA","#phi","C");
  fit_fun3->SetParameters(-0.2,-20.0,-5.0);
  fit_fun3->SetParLimits(1,0.9,4.1);

  pad22->cd(6);

  TGraphErrors* grp_1  = new TGraphErrors(k,x,value_1,errx,error_1);
  grp_1 ->SetName("grp_1");
  grp_1 ->Draw("goff");
  grp_1->SetTitle("");
  grp_1 ->SetMarkerSize(markerSize[0]*1.3);
  grp_1 ->SetLineWidth(0);
  grp_1 ->SetMarkerStyle(markerStyle[1]);
  grp_1 ->SetMarkerColor(kRed);
  grp_1 ->SetLineColor(kRed);
  grp_1->Fit("fit_fun","E M R F Q");
  TF1* fit_1 = grp_1->GetFunction("fit_fun");
  fit_1->DrawCopy("same");
  fit_1->SetLineColor(kRed+2);
  fit_1->SetLineStyle(2);
  fit_1->SetLineWidth(4);
  Double_t p_1   =  fit_1->GetParameter(0);
  Double_t ep_1  =  fit_1->GetParError(0);

  TGraphErrors* grp_2  = new TGraphErrors(k,x,value_2,errx,error_2);
  grp_2 ->SetName("grp_2");
  grp_2 ->Draw("goff");
  grp_2->SetTitle("");
  grp_2 ->SetMarkerSize(markerSize[0]*1.3);
  grp_2 ->SetLineWidth(0);
  grp_2 ->SetMarkerStyle(markerStyle[2]);
  grp_2 ->SetMarkerColor(kGreen+2);
  grp_2 ->SetLineColor(kGreen+2);
  grp_2->Fit("fit_fun","E M R F Q");
  TF1* fit_2 = grp_2->GetFunction("fit_fun");
  fit_2->DrawCopy("same");
  fit_2->SetLineColor(kGreen+2);
  fit_2->SetLineStyle(2);
  fit_2->SetLineWidth(4);
  Double_t p_2   =  fit_2->GetParameter(0);
  Double_t ep_2  =  fit_2->GetParError(0);

  TGraphErrors* grp_3  = new TGraphErrors(k,x,value_3,errx,error_3);
  grp_3 ->SetName("grp_3");
  grp_3 ->Draw("goff");
  grp_3->SetTitle("");
  grp_3 ->SetMarkerSize(markerSize[0]*1.3);
  grp_3 ->SetLineWidth(0);
  grp_3 ->SetMarkerStyle(markerStyle[0]);
  grp_3 ->SetMarkerColor(kBlue);
  grp_3 ->SetLineColor(kBlue);
  grp_3->Fit("fit_fun","E M R F Q");
  TF1* fit_3 = grp_3->GetFunction("fit_fun");
  fit_3->DrawCopy("same");
  fit_3->SetLineColor(kBlue);
  fit_3->SetLineStyle(2);
  fit_3->SetLineWidth(4);
  Double_t p_3   =  fit_3->GetParameter(0);
  Double_t ep_3  =  fit_3->GetParError(0);

  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_1,"P");
  grp->Add(grp_2,"P");
  grp->Add(grp_3,"P");
  grp->Draw("A");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle(Form("USLumi Asymmetry [ppm]"));
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);
  grp->GetXaxis()->SetNdivisions(8,0,0);
  grp->GetYaxis()->SetNdivisions(8,5,0);
  grp->GetXaxis()->SetLabelColor(0);
  grp->GetYaxis()->SetRangeUser(yCor[0],yCor[1]);


  Double_t statCorY[2] = {0.75,0.99};
  Double_t statCorX[6] = {0.15,0.41,0.42,0.70,0.71,0.99};

//   gStyle->SetFitFormat("%2.2g");
  TPaveStats *stats_1 = (TPaveStats*)grp_1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats_2 = (TPaveStats*)grp_2->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats_3 = (TPaveStats*)grp_3->GetListOfFunctions()->FindObject("stats");
  stats_1->SetTextColor(kRed+2);
  stats_1->SetFillColor(kWhite);
  stats_1->SetTextSize(markerSize[3]);
  //  stats_1->SetStatFormat("%2.1g");
  stats_1->SetFitFormat("2.2f");
  stats_2->SetTextColor(kGreen+2);
  stats_2->SetFillColor(kWhite); 
  stats_2->SetTextSize(markerSize[3]);
  stats_2->SetFitFormat("2.2f");
  stats_3->SetTextColor(kBlue);
  stats_3->SetFillColor(kWhite); 
  stats_3->SetTextSize(markerSize[3]);
  stats_3->SetFitFormat("2.2f");
  stats_1->SetX1NDC(statCorX[0]); stats_1->SetX2NDC(statCorX[1]); stats_1->SetY1NDC(statCorY[0]);stats_1->SetY2NDC(statCorY[1]);
  stats_2->SetX1NDC(statCorX[2]); stats_2->SetX2NDC(statCorX[3]); stats_2->SetY1NDC(statCorY[0]);stats_2->SetY2NDC(statCorY[1]);
  stats_3->SetX1NDC(statCorX[4]); stats_3->SetX2NDC(statCorX[5]); stats_3->SetY1NDC(statCorY[0]);stats_3->SetY2NDC(statCorY[1]);

  TLatex* text1=new TLatex(1,yCor[0]*1.2,Form("1"));
  text1->SetTextSize(markerSize[3]);
  TLatex* text2=new TLatex(2,yCor[0]*1.2,Form("3"));
  text2->SetTextSize(markerSize[3]);
  TLatex* text3=new TLatex(3,yCor[0]*1.2,Form("5"));
  text3->SetTextSize(markerSize[3]);
  TLatex* text4=new TLatex(4,yCor[0]*1.2,Form("7"));
  text4->SetTextSize(markerSize[3]);

  TLatex* fitText=new TLatex(0.98,yCor[0]*0.92,Form("FIT: #DeltaA sin[ #frac{#pi}{4}(x-1) + #phi] + C"));
  fitText->SetTextSize(markerSize[3]);
  fitText->Draw();

  text1->Draw();
  text2->Draw();
  text3->Draw();
  text4->Draw();

  cout<<red<<"Beamline background asymmetry = "<<fitSumPara3_p0<<" +- "<<fitSumPara3_p0Error<<normal<<endl;


  TString saveSummaryPlot2 = Form("dirPlot/resultPlot/USLumiSumAsymLongitudinal");

  canvas2->Update();
  canvas2->Print(saveSummaryPlot2+".png");


  }

  /********************************************/
  /********************************************/


  if(FIGURE3){

  TString title3 = Form("USLumi Asymmetries from Horizontal Transverse Running");

  TCanvas * canvas3 = new TCanvas("canvas3", title3,0,0,canvasSize[0],canvasSize[1]);
  canvas3->Draw();
  canvas3->SetBorderSize(0);
  canvas3->cd();
  TPad*pad31 = new TPad("pad31","pad31",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad32 = new TPad("pad32","pad32",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad31->SetFillColor(kWhite);
  pad31->Draw();
  pad32->Draw();
  pad31->cd();
  TLatex * t3 = new TLatex(0.06,0.3,Form("%s",title3.Data()));
  t3->SetTextSize(0.5);
  t3->Draw();
  pad32->cd();
  pad32->Divide(1,2);


  pad32->cd(1);

  Hel_Tree->Draw("uslumi1_sum.value-uslumi5_sum.value:run_number>>histTrans15",Form(""));
  histTrans15 = (TH2D *)gDirectory->Get("histTrans15");
  histTrans15->SetTitle("");
  histTrans15->SetXTitle("Run Number");
  histTrans15->SetYTitle("USLumi 1 - 5 Asym. [ppm]");
  histTrans15->GetYaxis()->CenterTitle();
  histTrans15->GetXaxis()->CenterTitle();
  histTrans15->GetYaxis()->SetRangeUser(hist_y[0], hist_y[1]);
  histTrans15->SetMarkerColor(kMagenta);
  histTrans15->SetMarkerStyle(markerStyle[0]);
  histTrans15->SetMarkerSize(markerSize[0]);
  histTrans15->Draw("");
  histTrans15->Fit("fit15HistTrans","QR","");

  TF1* fit_15HistTrans = histTrans15->GetFunction("fit15HistTrans");
  fit_15HistTrans->SetLineColor(kBlack);
  fit_15HistTrans->SetLineWidth(lineWidth[0]);

  histTrans15->SetStats(0);

  fitTrans15Para_p0          =  fit15HistTrans->GetParameter(0);
  fitTrans15Para_p0Error     =  fit15HistTrans->GetParError(0);
  fitTrans15Para_Chi         =  fit15HistTrans->GetChisquare();
  fitTrans15Para_NDF         =  fit15HistTrans->GetNDF();
  fitTrans15Para_Prob        =  fit15HistTrans->GetProb();

  cout<<red<<"fitTrans15Para : "<<fitTrans15Para_p0<<"+-"<<fitTrans15Para_p0Error<<","<<fitTrans15Para_Chi<<","<<fitTrans15Para_NDF<<","<<fitTrans15Para_Prob<<normal<<endl;

  TLatex* t15A = new TLatex(hist_runTrans[0]+0.2,hist_y[1]*0.88,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fitTrans15Para_p0,fitTrans15Para_p0Error));
  t15A->SetTextSize(markerSize[3]);
  t15A->SetTextColor(kMagenta      );
  TLatex* t15P = new TLatex(hist_runTrans[0]+0.2,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fitTrans15Para_Chi,fitTrans15Para_NDF,fitTrans15Para_Prob));
  t15P->SetTextSize(markerSize[3]);
  t15P->SetTextColor(kMagenta      );
  t15A->Draw("same"); t15P->Draw("same");

  gPad->Update();


  pad32->cd(2);

  Hel_Tree->Draw("uslumi3_sum.value-uslumi7_sum.value:run_number>>histTrans37",Form(""));
  histTrans37 = (TH2D *)gDirectory->Get("histTrans37");
  histTrans37->SetTitle("");
  histTrans37->SetXTitle("Run Number");
  histTrans37->SetYTitle("USLumi 3 - 7 Asym. [ppm]");
  histTrans37->GetYaxis()->CenterTitle();
  histTrans37->GetXaxis()->CenterTitle();
  histTrans37->GetYaxis()->SetRangeUser(hist_y[0], hist_y[1]);
  histTrans37->SetMarkerColor(kOrange+9);
  histTrans37->SetMarkerStyle(markerStyle[1]);
  histTrans37->SetMarkerSize(markerSize[1]);
  histTrans37->Draw("");
  histTrans37->Fit("fit37HistTrans","QR","");

  TF1* fit_37HistTrans = histTrans37->GetFunction("fit37HistTrans");
  fit_37HistTrans->SetLineColor(kBlack      );
  fit_37HistTrans->SetLineWidth(lineWidth[0]);

  histTrans37->SetStats(0);

  fitTrans37Para_p0          =  fit37HistTrans->GetParameter(0);
  fitTrans37Para_p0Error     =  fit37HistTrans->GetParError(0);
  fitTrans37Para_Chi         =  fit37HistTrans->GetChisquare();
  fitTrans37Para_NDF         =  fit37HistTrans->GetNDF();
  fitTrans37Para_Prob        =  fit37HistTrans->GetProb();

  cout<<red<<"fitTrans37Para : "<<fitTrans37Para_p0<<"+-"<<fitTrans37Para_p0Error<<","<<fitTrans37Para_Chi<<","<<fitTrans37Para_NDF<<","<<fitTrans37Para_Prob<<normal<<endl;

  TLatex* t37A = new TLatex(hist_runTrans[0]+0.2,hist_y[1]*0.88,Form("A_{USLumi1-5} = %1.2f #pm %1.2f",fitTrans37Para_p0,fitTrans37Para_p0Error));
  t37A->SetTextSize(markerSize[3]);
  t37A->SetTextColor(kOrange+9      );
  TLatex* t37P = new TLatex(hist_runTrans[0]+0.2,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fitTrans37Para_Chi,fitTrans37Para_NDF,fitTrans37Para_Prob));
  t37P->SetTextSize(markerSize[3]);
  t37P->SetTextColor(kOrange+9      );
  t37A->Draw("same"); t37P->Draw("same");


  gPad->Update();

//   pad32->cd(2);

// //   tree->Draw("asym_uslumi3_sum.value-asym_uslumi7_sum.value:run_number",Form("asym_uslumi3_sum.n>0 && asym_uslumi7_sum.n>0 && run_number>15800 && run_number<16400"));
//   tree->Draw("asym_uslumi3_sum.value-asym_uslumi7_sum.value:run_number>>hist37",Form("asym_uslumi3_sum.n>0 && asym_uslumi7_sum.n>0 && run_number>(%f+100) && run_number<(%f-100)",hist_run[0],hist_run[1]));
//   hist37 = (TH2D *)gDirectory->Get("hist37");
//   hist37->SetTitle("");
//   hist37->SetXTitle("Run Number");
//   hist37->SetYTitle("USLumi 3 - 7 Asym. [ppm]");
//   hist37->GetYaxis()->CenterTitle();
//   hist37->GetXaxis()->CenterTitle();
//   hist37->GetYaxis()->SetRangeUser(hist_y[0], hist_y[1]);
//   hist37->SetMarkerColor(kOrange+9);
//   hist37->SetMarkerStyle(markerStyle[1]);
//   hist37->SetMarkerSize(markerSize[1]);
//   hist37->Draw("");
//   hist37->Fit("fit37Hist1","QR","");
//   hist37->Fit("fit37Hist2","QR+","");
//   hist37->Fit("fit37Hist3","QR+","");

//   TF1* fit_37Hist1 = hist37->GetFunction("fit37Hist1");
//   fit_37Hist1->SetLineColor(kRed+1);
//   fit_37Hist1->SetLineWidth(lineWidth[0]);
//   TF1* fit_37Hist2 = hist37->GetFunction("fit37Hist2");
//   fit_37Hist2->SetLineColor(kGreen+2);
//   fit_37Hist2->SetLineWidth(lineWidth[0]);
//   TF1* fit_37Hist3 = hist37->GetFunction("fit37Hist3");
//   fit_37Hist3->SetLineColor(kBlue+1);
//   fit_37Hist3->SetLineWidth(lineWidth[0]);

//   hist37->SetStats(0);

//   fit37Para1_p0          =  fit37Hist1->GetParameter(0);
//   fit37Para1_p0Error     =  fit37Hist1->GetParError(0);
//   fit37Para1_Chi         =  fit37Hist1->GetChisquare();
//   fit37Para1_NDF         =  fit37Hist1->GetNDF();
//   fit37Para1_Prob        =  fit37Hist1->GetProb();

//   fit37Para2_p0          =  fit37Hist2->GetParameter(0);
//   fit37Para2_p0Error     =  fit37Hist2->GetParError(0);
//   fit37Para2_Chi         =  fit37Hist2->GetChisquare();
//   fit37Para2_NDF         =  fit37Hist2->GetNDF();
//   fit37Para2_Prob        =  fit37Hist2->GetProb();

//   fit37Para3_p0          =  fit37Hist3->GetParameter(0);
//   fit37Para3_p0Error     =  fit37Hist3->GetParError(0);
//   fit37Para3_Chi         =  fit37Hist3->GetChisquare();
//   fit37Para3_NDF         =  fit37Hist3->GetNDF();
//   fit37Para3_Prob        =  fit37Hist3->GetProb();

//   cout<<red<<"fit37Para1 : "<<fit37Para1_p0<<"+-"<<fit37Para1_p0Error<<","<<fit37Para1_Chi<<","<<fit37Para1_NDF<<","<<fit37Para1_Prob<<normal<<endl;
//   cout<<green<<"fit37Para2 : "<<fit37Para2_p0<<"+-"<<fit37Para2_p0Error<<","<<fit37Para2_Chi<<","<<fit37Para2_NDF<<","<<fit37Para2_Prob<<normal<<endl;
//   cout<<blue<<"fit37Para3 : "<<fit37Para3_p0<<"+-"<<fit37Para3_p0Error<<","<<fit37Para3_Chi<<","<<fit37Para3_NDF<<","<<fit37Para3_Prob<<normal<<endl;

//   TLatex* t37A_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.88,Form("A_{USLumi3-7} = %1.2f #pm %1.2f",fit37Para1_p0,fit37Para1_p0Error));
//   t37A_1->SetTextSize(markerSize[3]);
//   t37A_1->SetTextColor(kRed+1);
//   TLatex* t37P_1 = new TLatex(fit_range[0]-30,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit37Para1_Chi,fit37Para1_NDF,fit37Para1_Prob));
//   t37P_1->SetTextSize(markerSize[3]);
//   t37P_1->SetTextColor(kRed+1);

//   TLatex* t37A_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.71,Form("A_{USLumi3-7} = %1.2f #pm %1.2f",fit37Para2_p0,fit37Para2_p0Error));
//   t37A_2->SetTextSize(markerSize[3]);
//   t37A_2->SetTextColor(kGreen+2);
//   TLatex* t37P_2 = new TLatex(fit_range[2]-00,hist_y[0]*0.88,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit37Para2_Chi,fit37Para2_NDF,fit37Para2_Prob));
//   t37P_2->SetTextSize(markerSize[3]);
//   t37P_2->SetTextColor(kGreen+2);

//   TLatex* t37A_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.88,Form("A_{USLumi3-7} = %1.2f #pm %1.2f",fit37Para3_p0,fit37Para3_p0Error));
//   t37A_3->SetTextSize(markerSize[3]);
//   t37A_3->SetTextColor(kBlue+1);
//   TLatex* t37P_3 = new TLatex(fit_range[4]+20,hist_y[1]*0.71,Form("#chi^{2} %1.2g/%1.0f, Prob %1.1f",fit37Para3_Chi,fit37Para3_NDF,fit37Para3_Prob));
//   t37P_3->SetTextSize(markerSize[3]);
//   t37P_3->SetTextColor(kBlue+1);

//   t37A_1->Draw("same"); t37P_1->Draw("same");
//   t37A_2->Draw("same"); t37P_2->Draw("same");
//   t37A_3->Draw("same"); t37P_3->Draw("same");


  gPad->Update();


  TString saveSummaryPlot3 = Form("dirPlot/resultPlot/USLumiOctantAsymTransverse");

  canvas3->Update();
  canvas3->Print(saveSummaryPlot3+".png");

  }

  /********************************************/
















  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


