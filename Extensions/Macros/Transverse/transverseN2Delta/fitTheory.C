using namespace std;
#include "NurClass.h"

int main(Int_t argc,Char_t* argv[]){

  Bool_t THETA = 0;
  Bool_t THETA1 = 0;
  Bool_t THETA2 = 0;
  Bool_t THETA3 = 0;
  Bool_t THETA_ALL = 1;
  Bool_t ASYM = 0;

  Double_t figSize=1.5;


  ///* Canvas and Pad and Scale parameters */
  Int_t canvasSize[2] ={1200*figSize,650*figSize};
  Int_t canvasSize2[2] ={1000*figSize,650*figSize};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.925};
  Double_t textSize[2] = {0.035*figSize,0.045*figSize};
  Double_t markerSize[6] = {1.4,1.0,0.9,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.900,0.75,0.995};
  Double_t yScale[2] = {-10.0,10.0};
  Double_t waterMark[2] = {2.5,-1.0};

  Double_t x_lo_stat,y_lo_stat,x_hi_stat,y_hi_stat;
  x_lo_stat=0.81;y_lo_stat=0.64;x_hi_stat=0.99;y_hi_stat=0.95;

  std::ofstream MyfileTheory;

  const Int_t NUM = 100;
  const Int_t NUM2 = 10;
  int counter=0;
  int counter2=0;
  int counter3=0;
  ifstream asymTheory,BarbaraAsym,BarbaraAsym877MeV;
  Double_t zero[NUM],energy[NUM],asym[NUM],asymError[NUM],asymC[NUM],asymCError[NUM];
  Double_t thetaBarbara[NUM2],NBarbara[NUM2],DeltaBarbara[NUM2],SumBarbara[NUM2];
  Double_t thetaBarbara877MeV[NUM2],NBarbara877MeV[NUM2],DeltaBarbara877MeV[NUM2],SumBarbara877MeV[NUM2];

  Double_t x[NUM],errx[NUM],x2[NUM2],errx2[NUM2];

  for(Int_t i =0;i<NUM;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }
  for(Int_t j =0;j<NUM2;j++){
    x2[j] = j+1;   
    errx2[j] = 0.0;
  }

 //*************************************************//

  asymTheory.open(Form("dirPlot/C_Al_theta7p8deg_bn.dat"));
  printf(Form("dirPlot/C_Al_theta7p8deg_bn2.dat\n"));
 
  if (asymTheory.is_open()) {
   
    while(!asymTheory.eof()) {
      zero[counter]=0;
      asymTheory >> energy[counter];
      asymTheory >> asymError[counter];
      asymTheory >> asym[counter];
      asymTheory >> asymCError[counter];
      asymTheory >> asymC[counter];

      if (!asymTheory.good()) break;

      cout<<blue
	  <<energy[counter]<<"\t"<<asym[counter]<<"+-"<<asymError[counter]<<"\t"<<asymC[counter]<<"+-"<<asymCError[counter]<<"\t"
	  <<normal<<endl;
 
      counter++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymTheory.close();

 //*************************************************//

  BarbaraAsym.open(Form("dirPlot/data_tbsa_delta_enel1p160.dat"));
  printf(Form("dirPlot/data_tbsa_delta_enel1p160.dat\n"));
 
  if (BarbaraAsym.is_open()) {
   
    while(!BarbaraAsym.eof()) {
      zero[counter2]=0;
      BarbaraAsym >> thetaBarbara[counter2];
      BarbaraAsym >> NBarbara[counter2];
      BarbaraAsym >> DeltaBarbara[counter2];
      BarbaraAsym >> SumBarbara[counter2];

      if (!BarbaraAsym.good()) break;

      cout<<blue
	  <<thetaBarbara[counter2]<<"\t"<<NBarbara[counter2]<<"\t"<<DeltaBarbara[counter2]<<"\t"<<SumBarbara[counter2]<<"\t"
	  <<normal<<endl;
 
      counter2++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  BarbaraAsym.close();

 //*************************************************//

  BarbaraAsym877MeV.open(Form("dirPlot/data_tbsa_delta_enel0p877.dat"));
  printf(Form("dirPlot/data_tbsa_delta_enel0p877.dat\n"));
 
  if (BarbaraAsym877MeV.is_open()) {
   
    while(!BarbaraAsym877MeV.eof()) {
      BarbaraAsym877MeV >> thetaBarbara877MeV[counter3];
      BarbaraAsym877MeV >> NBarbara877MeV[counter3];
      BarbaraAsym877MeV >> DeltaBarbara877MeV[counter3];
      BarbaraAsym877MeV >> SumBarbara877MeV[counter3];

      if (!BarbaraAsym877MeV.good()) break;

      cout<<blue
	  <<thetaBarbara877MeV[counter3]<<"\t"<<NBarbara877MeV[counter3]<<"\t"<<DeltaBarbara877MeV[counter3]<<"\t"<<SumBarbara877MeV[counter3]<<"\t"
	  <<normal<<endl;
 
      counter3++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  BarbaraAsym877MeV.close();


 //*************************************************//


  int counterPasquiniDelta424=0;
  ifstream asymTheoryPasquiniDelta424;
  Double_t asymPasquiniDelta424[NUM],thetaPasquiniDelta424[NUM];

  asymTheoryPasquiniDelta424.open(Form("dirPlot/asymPasquiniDelta424.dat"));
  printf(Form("dirPlot/asymPasquiniDelta424.dat\n"));
 
  if (asymTheoryPasquiniDelta424.is_open()) {
   
    while(!asymTheoryPasquiniDelta424.eof()) {
      asymTheoryPasquiniDelta424 >> thetaPasquiniDelta424[counterPasquiniDelta424];
      asymTheoryPasquiniDelta424 >> asymPasquiniDelta424[counterPasquiniDelta424];

      if (!asymTheoryPasquiniDelta424.good()) break;

//       cout<<blue<<"\t"<<thetaPasquiniDelta424[counterPasquiniDelta424]<<"\t"<<asymPasquiniDelta424[counterPasquiniDelta424]<<normal<<endl;
 
      counterPasquiniDelta424++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymTheoryPasquiniDelta424.close();

  //*************************************************//

  int counterPasquiniDelta570=0;
  ifstream asymTheoryPasquiniDelta570;
  Double_t asymPasquiniDelta570[NUM],thetaPasquiniDelta570[NUM];

  asymTheoryPasquiniDelta570.open(Form("dirPlot/asymPasquiniDelta570.dat"));
  printf(Form("dirPlot/asymPasquiniDelta570.dat\n"));
 
  if (asymTheoryPasquiniDelta570.is_open()) {
   
    while(!asymTheoryPasquiniDelta570.eof()) {
      asymTheoryPasquiniDelta570 >> thetaPasquiniDelta570[counterPasquiniDelta570];
      asymTheoryPasquiniDelta570 >> asymPasquiniDelta570[counterPasquiniDelta570];

      if (!asymTheoryPasquiniDelta570.good()) break;

      cout<<blue<<"\t"<<thetaPasquiniDelta570[counterPasquiniDelta570]<<"\t"<<asymPasquiniDelta570[counterPasquiniDelta570]<<normal<<endl;
 
      counterPasquiniDelta570++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymTheoryPasquiniDelta570.close();

  //*************************************************//

  int counterPasquiniDelta855=0;
  ifstream asymTheoryPasquiniDelta855;
  Double_t asymPasquiniDelta855[NUM],thetaPasquiniDelta855[NUM];

  asymTheoryPasquiniDelta855.open(Form("dirPlot/asymPasquiniDelta855.dat"));
  printf(Form("dirPlot/asymPasquiniDelta855.dat\n"));
 
  if (asymTheoryPasquiniDelta855.is_open()) {
   
    while(!asymTheoryPasquiniDelta855.eof()) {
      asymTheoryPasquiniDelta855 >> thetaPasquiniDelta855[counterPasquiniDelta855];
      asymTheoryPasquiniDelta855 >> asymPasquiniDelta855[counterPasquiniDelta855];

      if (!asymTheoryPasquiniDelta855.good()) break;

//       cout<<blue<<"\t"<<thetaPasquiniDelta855[counterPasquiniDelta855]<<"\t"<<asymPasquiniDelta855[counterPasquiniDelta855]<<normal<<endl;
 
      counterPasquiniDelta855++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymTheoryPasquiniDelta855.close();

  //*************************************************//


  const Int_t NUM1 = 1;
  int counter1=0; int counter11=0;  int counter02=0;  int counter03=0;
  ifstream asymQweakFinal,asymQweakFinal877MeV,asymQweakFinal6000,asymQweakFinal7300;
  Double_t AnQweakFinal[NUM1],dAnQweakFinal_stat[NUM1],dAnQweakFinal_sys[NUM1],dAnQweakFinal[NUM1];
  Double_t BnQweakFinal877MeV[NUM1],dBnQweakFinal877MeV_stat[NUM1],dBnQweakFinal877MeV_sys[NUM1],dBnQweakFinal877MeV[NUM1];
  Double_t AnQweakFinal6000[NUM1],dAnQweakFinal6000_stat[NUM1],dAnQweakFinal6000_sys[NUM1],dAnQweakFinal6000[NUM1];
  Double_t AnQweakFinal7300[NUM1],dAnQweakFinal7300_stat[NUM1],dAnQweakFinal7300_sys[NUM1],dAnQweakFinal7300[NUM1];

  //*************************************************//
  asymQweakFinal.open(Form("dirPlot/resultText/n-to-delta_6700_HYDROGEN-CELL_MD_Final_Asymmetry_run2_pass5.txt"));
  printf(Form("dirPlot/resultText/n-to-delta_6700_HYDROGEN-CELL_MD_Final_Asymmetry_run2_pass5.txt\n"));

  if (asymQweakFinal.is_open()) {
   
    while(!asymQweakFinal.eof()) {
      asymQweakFinal >> AnQweakFinal[counter1];
      asymQweakFinal >> dAnQweakFinal_stat[counter1];
      asymQweakFinal >> dAnQweakFinal_sys[counter1];
      asymQweakFinal >> dAnQweakFinal[counter1];

      if (!asymQweakFinal.good()) break;

      cout<<blue
	  <<AnQweakFinal[counter1]<<"+-"<<dAnQweakFinal_stat[counter1]<<"(stat)\t+-"<<dAnQweakFinal_sys[counter1]<<"(sys)\t"<<dAnQweakFinal[counter1]<<"(total)"
	  <<normal<<endl;
 
      counter1++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymQweakFinal.close();

  //*************************************************//
  asymQweakFinal877MeV.open(Form("dirPlot/resultText/n-to-delta_6700_HYDROGEN-CELL_877MeV.txt"));
  printf(Form("dirPlot/resultText/n-to-delta_6700_HYDROGEN-CELL_877MeV.txt\n"));

  if (asymQweakFinal877MeV.is_open()) {
   
    while(!asymQweakFinal877MeV.eof()) {
      asymQweakFinal877MeV >> BnQweakFinal877MeV[counter11];
      asymQweakFinal877MeV >> dBnQweakFinal877MeV_stat[counter11];
      asymQweakFinal877MeV >> dBnQweakFinal877MeV_sys[counter11];
      asymQweakFinal877MeV >> dBnQweakFinal877MeV[counter11];

      if (!asymQweakFinal877MeV.good()) break;

      cout<<blue
	  <<BnQweakFinal877MeV[counter11]<<"+-"<<dBnQweakFinal877MeV_stat[counter11]<<"(stat)\t+-"<<dBnQweakFinal877MeV_sys[counter11]<<"(sys)\t"<<dBnQweakFinal877MeV[counter11]<<"(total)"
	  <<normal<<endl;
 
      counter11++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymQweakFinal877MeV.close();

  //*************************************************//
  asymQweakFinal6000.open(Form("dirPlot/resultText/n-to-delta_6000_HYDROGEN-CELL_MD_Final_Asymmetry_run2_pass5.txt"));
  printf(Form("dirPlot/resultText/n-to-delta_6000_HYDROGEN-CELL_MD_Final_Asymmetry_run2_pass5.txt\n"));

  if (asymQweakFinal6000.is_open()) {
   
    while(!asymQweakFinal6000.eof()) {
      asymQweakFinal6000 >> AnQweakFinal6000[counter02];
      asymQweakFinal6000 >> dAnQweakFinal6000_stat[counter02];
      asymQweakFinal6000 >> dAnQweakFinal6000_sys[counter02];
      asymQweakFinal6000 >> dAnQweakFinal6000[counter02];

      if (!asymQweakFinal6000.good()) break;

      cout<<blue
	  <<AnQweakFinal6000[counter02]<<"+-"<<dAnQweakFinal6000_stat[counter02]<<"(stat)\t+-"<<dAnQweakFinal6000_sys[counter02]<<"(sys)\t"<<dAnQweakFinal6000[counter02]<<"(total)"
	  <<normal<<endl;
 
      counter02++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymQweakFinal6000.close();

  //*************************************************//
  asymQweakFinal7300.open(Form("dirPlot/resultText/n-to-delta_7300_HYDROGEN-CELL_MD_Final_Asymmetry_run2_pass5.txt"));
  printf(Form("dirPlot/resultText/n-to-delta_7300_HYDROGEN-CELL_MD_Final_Asymmetry_run2_pass5.txt\n"));

  if (asymQweakFinal7300.is_open()) {
   
    while(!asymQweakFinal7300.eof()) {
      asymQweakFinal7300 >> AnQweakFinal7300[counter03];
      asymQweakFinal7300 >> dAnQweakFinal7300_stat[counter03];
      asymQweakFinal7300 >> dAnQweakFinal7300_sys[counter03];
      asymQweakFinal7300 >> dAnQweakFinal7300[counter03];

      if (!asymQweakFinal7300.good()) break;

      cout<<blue
	  <<AnQweakFinal7300[counter03]<<"+-"<<dAnQweakFinal7300_stat[counter03]<<"(stat)\t+-"<<dAnQweakFinal7300_sys[counter03]<<"(sys)\t"<<dAnQweakFinal7300[counter03]<<"(total)"
	  <<normal<<endl;
 
      counter03++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  asymQweakFinal7300.close();

  //*************************************************//




  //*************************************************//
  //*************************************************//
  TF1 *fitfunPasquiniDelta855 = new TF1("fitfunPasquiniDelta855","expo",4.0,41.0);
  TF1 *fitfunPasquiniDelta570 = new TF1("fitfunPasquiniDelta570","expo",4.0,41.0);
  TF1 *fitfunPasquiniDelta424 = new TF1("fitfunPasquiniDelta424","expo",4.0,41.0);
//   TF1 *fitfunPasquiniDelta855 = new TF1("fitfunPasquiniDelta855","[0]+[1]*x^0.5",20.0,41.0);

  fitfunPasquiniDelta424->SetParameters(6.1,-0.03);
  fitfunPasquiniDelta570->SetParameters(5.4,-0.03);
  fitfunPasquiniDelta855->SetParameters(4.3,-0.03);

  Double_t theta_qweak_elastic = 7.90;
  Double_t theta_qweak_elastic_error = 0.03;
  Double_t theta_qweak  = 8.348;
  Double_t theta_qweak_error = 1.337;
  Double_t theta_qweak2 = theta_qweak-0.25;
  Double_t theta_qweak3 = theta_qweak+0.25;

  //*************************************************//
 //*************************************************//

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
  gStyle->SetPadRightMargin(0.20);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.80);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.050,"x");
  gStyle->SetLabelSize(0.050,"y");
  gStyle->SetTitleSize(0.050,"x");
  gStyle->SetTitleSize(0.050,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.075);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gStyle->SetStatFormat("2.2f");
  gStyle->SetFitFormat("2.2f");

  gDirectory->Delete("*");

  TF1 *fitfun1 = new TF1("fitfun1","[0]+[1]*x^0.5",0.82,1.63);
  TF1 *fitfun2 = new TF1("fitfun2","[0]+[1]*x^1.0",0.82,1.63);
//   TF1 *fitfun01 = new TF1("fitfun01","[0] + exp(x*[1]+[2])",5.5,12.5);
  TF1 *fitfun01 = new TF1("fitfun01","[0] + ([1]/x)",5.5,12.5);
//   TF1 *fitfun01 = new TF1("fitfun01","([0]/x)",5.5,12.5);


  if(THETA){

  TCanvas * canvas1 = new TCanvas("canvas1", "canvas1",0,0,canvasSize[0],canvasSize[1]);
  canvas1->Draw();
  canvas1->SetBorderSize(0);
  canvas1->cd();
  TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  TLatex * t1 = new TLatex(0.06,0.3,Form("Enargy Dependence of Beam Normal Single Spin Asymmetry at #theta_{lab} = 7.8^{o}(Gorchtein et. al.)"));
  t1->SetTextSize(0.7);
  t1->Draw();
  pad2->cd();


  TGraphErrors *MyGraphC1 = new TGraphErrors(NUM,energy,asymC,errx,asymCError);
  MyGraphC1->SetName("MyGraphC1");
  MyGraphC1->Draw("goff");
  MyGraphC1->SetMarkerStyle(20);
  MyGraphC1->SetMarkerSize(markerSize[0]*figSize);
  MyGraphC1->SetMarkerColor(kOrange+7);
  MyGraphC1->SetLineColor(kOrange+7);
  MyGraphC1->Fit("fitfun1","R0EF");
  TF1* fitC1 = MyGraphC1->GetFunction("fitfun1");
//   fitC1->Draw("same");
  fitC1->SetLineColor(kOrange+9);
  fitC1->SetLineStyle(1);
  fitC1->SetLineWidth(1);
  fitC1->SetParNames("C_{0}^{0.5}","C_{1}^{0.5}");

  TGraphErrors *MyGraphC2 = new TGraphErrors(NUM,energy,asymC,errx,asymCError);
  MyGraphC2->SetName("MyGraphC2");
  MyGraphC2->Draw("goff");
  MyGraphC2->SetMarkerStyle(20);
  MyGraphC2->SetMarkerSize(markerSize[0]*figSize);
  MyGraphC2->SetMarkerColor(kOrange+7);
  MyGraphC2->SetLineColor(kOrange+7);
  MyGraphC2->Fit("fitfun2","R0EF");
  TF1* fitC2 = MyGraphC2->GetFunction("fitfun2");
//   fitC2->Draw("same");
  fitC2->SetLineColor(kBlack);
  fitC2->SetLineStyle(2);
  fitC2->SetLineWidth(1);
  fitC2->SetParNames("C_{0}^{1.0}","C_{1}^{1.0}");

//   gPad->SetGrid();
  TGraphErrors *MyGraph1 = new TGraphErrors(NUM,energy,asym,errx,asymError);
  MyGraph1->SetName("MyGraph1");
  MyGraph1->Draw("goff");
  MyGraph1->SetMarkerStyle(22);
  MyGraph1->SetMarkerSize(markerSize[0]*figSize);
  MyGraph1->SetMarkerColor(kMagenta);
  MyGraph1->SetLineColor(kMagenta);
  MyGraph1->Fit("fitfun1","R0EF");
  TF1* fit1 = MyGraph1->GetFunction("fitfun1");
//   fit1->Draw("same");
  fit1->SetLineColor(kMagenta+1);
  fit1->SetLineStyle(1);
  fit1->SetLineWidth(1);
  fit1->SetParNames("Al_{0}^{0.5}","Al_{1}^{0.5}");

  TGraphErrors *MyGraph2 = new TGraphErrors(NUM,energy,asym,errx,asymError);
  MyGraph2->SetName("MyGraph2");
  MyGraph2->Draw("goff");
  MyGraph2->SetMarkerStyle(22);
  MyGraph2->SetMarkerSize(markerSize[0]*figSize);
  MyGraph2->SetMarkerColor(kMagenta);
  MyGraph2->SetLineColor(kMagenta);
  MyGraph2->Fit("fitfun2","R0EF");
  TF1* fit2 = MyGraph2->GetFunction("fitfun2");
//   fit2->Draw("same");
  fit2->SetLineColor(kBlack);
  fit2->SetLineStyle(2);
  fit2->SetLineWidth(1);
  fit2->SetParNames("Al_{0}^{1.0}","Al_{1}^{1.0}");


  TMultiGraph * MyGraph = new TMultiGraph();
  MyGraph->Add(MyGraph1,"P");
  MyGraph->Add(MyGraph2,"P");
  MyGraph->Add(MyGraphC1,"P");
  MyGraph->Add(MyGraphC2,"P");
  MyGraph->Draw("A");
  MyGraph->SetTitle("");
  MyGraph->GetYaxis()->SetTitle("Beam Normal Spin Asymmetry [ppm]");
  MyGraph->GetYaxis()->CenterTitle();
  MyGraph->GetXaxis()->SetTitle("E_{beam} [GeV]");
  MyGraph->GetXaxis()->CenterTitle();
  MyGraph->GetYaxis()->SetTitleOffset(0.95);
  MyGraph->GetXaxis()->SetTitleOffset(1.00);
  MyGraph->GetXaxis()->SetNdivisions(9,5,0);
  MyGraph->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisMyGraph= MyGraph->GetXaxis();
  xaxisMyGraph->SetLimits(0.80,1.7);
  MyGraph->GetYaxis()->SetRangeUser(-14.0,-7.5);

  fit1->Draw("same");
  fit2->Draw("same");
  fitC1->Draw("same");
  fitC2->Draw("same");

  TPaveStats *stats1 = (TPaveStats*)MyGraph1->GetListOfFunctions()->FindObject("stats");
  stats1->SetTextColor(kMagenta);
  stats1->SetFillColor(kWhite); 
  stats1->SetX1NDC(x_lo_stat); stats1->SetX2NDC(x_hi_stat); stats1->SetY1NDC(0.81);stats1->SetY2NDC(0.99);

  TPaveStats *stats2 = (TPaveStats*)MyGraph2->GetListOfFunctions()->FindObject("stats");
  stats2->SetTextColor(kBlack);
  stats2->SetFillColor(kWhite); 
  stats2->SetX1NDC(x_lo_stat); stats2->SetX2NDC(x_hi_stat); stats2->SetY1NDC(0.62);stats2->SetY2NDC(0.80);


  TPaveStats *statsC1 = (TPaveStats*)MyGraphC1->GetListOfFunctions()->FindObject("stats");
  statsC1->SetTextColor(kOrange+9);
  statsC1->SetFillColor(kWhite); 
  statsC1->SetX1NDC(x_lo_stat); statsC1->SetX2NDC(x_hi_stat); statsC1->SetY1NDC(0.43);statsC1->SetY2NDC(0.61);

  TPaveStats *statsC2 = (TPaveStats*)MyGraphC2->GetListOfFunctions()->FindObject("stats");
  statsC2->SetTextColor(kBlack);
  statsC2->SetFillColor(kWhite); 
  statsC2->SetX1NDC(x_lo_stat); statsC2->SetX2NDC(x_hi_stat); statsC2->SetY1NDC(0.24);statsC2->SetY2NDC(0.42);

  TLegend *legend = new TLegend(x_lo_stat,0.05,x_hi_stat,0.23,"","brNDC");
//   legend->SetNColumns(2);
//   legend->AddEntry(MyGraph1, Form("Fit1: a_{1} + b_{1} E_{beam}^{0.5}"), "e");
//   legend->AddEntry(MyGraph2, Form("Fit2: a_{2} + b_{2} E_{beam}^{1.0}"), "e");
  legend->SetFillColor(0);
  legend->SetBorderSize(2);
  legend->SetTextSize(0.035);
  legend->Draw("");

  TLatex * fitFunDraw1 = new TLatex(1.75,-13.8,"Fit_{0.5}: T_{0}^{0.5} + T_{1}^{0.5} E_{beam}^{0.5}");
  fitFunDraw1->SetTextSize(0.035);
//   fitFunDraw1->SetTextColor(kMagenta+1);
  TLatex * fitFun2Draw = new TLatex(1.75,-14.2,"Fit_{1.0}: T_{0}^{1.0} + T_{1}^{1.0} E_{beam}^{1.0}");
  fitFun2Draw->SetTextSize(0.035);
  fitFun2Draw->SetTextColor(kBlack);

  TLine *line1 = new TLine(1.72,-13.7,1.74,-13.7);
//   line1->SetLineColor(kMagenta+1);
  TLine *line2 = new TLine(1.72,-14.1,1.74,-14.1);
  line2->SetLineColor(kBlack);
  line2->SetLineStyle(2);

  TLine *lineE = new TLine(1.155,-14.0,1.155,-7.5);
  lineE->SetLineColor(kGray+1);
  lineE->SetLineStyle(3);
  lineE->SetLineWidth(2);

  TLatex * tTextE = new TLatex(1.175,-9.5,"E_{Q-weak} = 1.155");
  tTextE->SetTextSize(0.035);
  tTextE->SetTextAngle(90);
  tTextE->SetTextColor(kBlack);

  TLatex * tTextAl = new TLatex(1.4,-11.5,"Al");
  tTextAl->SetTextSize(0.035);
  tTextAl->SetTextColor(kMagenta);
  TLatex * tTextC = new TLatex(1.4,-12.5,"C");
  tTextC->SetTextSize(0.035);
  tTextC->SetTextColor(kOrange+7);

  fitFunDraw1->Draw("same");
  fitFun2Draw->Draw("same");
  line1->Draw();
  line2->Draw();
  lineE->Draw();
  tTextE->Draw("same");tTextAl->Draw("same");tTextC->Draw("same");


  gPad->Update();


  TString savePlot = Form("dirPlot/resultPlot/theoryThetaDependenceOfAsymmetry");
  
  canvas1->Update();
  canvas1->Print(savePlot+".png");

  }

 //*************************************************//
 //*************************************************//
  // Fit and stat parameters
//   gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);
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
  //  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.80);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.060,"x");
  gStyle->SetLabelSize(0.060,"y");
  gStyle->SetTitleSize(0.065,"x");
  gStyle->SetTitleSize(0.065,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.075);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gStyle->SetStatFormat("2.2f");
  gStyle->SetFitFormat("2.2f");

  gDirectory->Delete("*");



  if(THETA2){

  TCanvas * canvas01 = new TCanvas("canvas01", "canvas01",0,0,canvasSize2[0],canvasSize2[1]);
  canvas01->Draw();
  canvas01->SetBorderSize(0);
  canvas01->cd();
  TPad*pad11 = new TPad("pad11","pad11",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad12 = new TPad("pad12","pad12",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad11->SetFillColor(kWhite);
  pad11->Draw();
  pad12->Draw();
  pad11->cd();
  TLatex * t01 = new TLatex(0.0,0.3,Form("Scattering Angle Dependence of Beam Normal Single Spin Asymmetry at E_{beam} = 1.160 GeV(B. Pasquini), Fit: p_{0} + p_{1}/#theta_{lab}"));
  t01->SetTextSize(0.5);
  t01->Draw();
  pad12->cd();

  gStyle->SetLineWidth(2.0);
//   gPad->SetGrid();
  TGraphErrors *MyGraph01 = new TGraphErrors(NUM2,thetaBarbara,NBarbara,errx2,errx2);
  MyGraph01->SetName("MyGraph01");
  MyGraph01->Draw("goff");
  MyGraph01->SetMarkerStyle(1);
  MyGraph01->SetMarkerSize(markerSize[0]*figSize);
  MyGraph01->SetMarkerColor(kGreen+2);
  MyGraph01->SetLineColor(kGreen+2);
  MyGraph01->Fit("fitfun01","R0EF");
  TF1* fit01 = MyGraph01->GetFunction("fitfun01");
  fit01->SetLineColor(kGreen+2);
  fit01->SetLineStyle(2);
  fit01->SetLineWidth(3);
  fit01->SetParNames("N_{p0}","N_{p1}","N_{p2}");

  TGraphErrors *MyGraph02 = new TGraphErrors(NUM2,thetaBarbara,DeltaBarbara,errx2,errx2);
  MyGraph02->SetName("MyGraph02");
  MyGraph02->Draw("goff");
  MyGraph02->SetMarkerStyle(1);
  MyGraph02->SetMarkerSize(markerSize[0]*figSize);
  MyGraph02->SetMarkerColor(kRed);
  MyGraph02->SetLineColor(kRed);
  MyGraph02->Fit("fitfun01","R0EF");
  TF1* fit02 = MyGraph02->GetFunction("fitfun01");
  fit02->SetLineColor(kRed);
  fit02->SetLineStyle(9);
  fit02->SetLineWidth(3);
  fit02->SetParNames("#Delta_{p0}","#Delta_{p1}","#Delta_{p2}");

  TGraphErrors *MyGraph03 = new TGraphErrors(NUM2,thetaBarbara,SumBarbara,errx2,errx2);
  MyGraph03->SetName("MyGraph03");
  MyGraph03->Draw("goff");
  MyGraph03->SetMarkerStyle(1);
  MyGraph03->SetMarkerSize(markerSize[0]*figSize);
  MyGraph03->SetMarkerColor(kBlack);
  MyGraph03->SetLineColor(kBlack);
  MyGraph03->Fit("fitfun01","R0EF");
  TF1* fit03 = MyGraph03->GetFunction("fitfun01");
  fit03->SetLineColor(kBlack);
  fit03->SetLineStyle(1);
  fit03->SetLineWidth(3);
  fit03->SetParNames("N+#Delta_{p0}","N+#Delta_{p1}","N+#Delta_{p2}");

  Double_t N_p0   = fit01->GetParameter(0);
  Double_t N_p0_e = fit01->GetParError(0);
  Double_t N_p1   = fit01->GetParameter(1);
  Double_t N_p1_e = fit01->GetParError(1);
  Double_t N_p2   = fit01->GetParameter(2);
  Double_t N_p2_e = fit01->GetParError(2);

  Double_t Delta_p0   = fit02->GetParameter(0);
  Double_t Delta_p0_e = fit02->GetParError(0);
  Double_t Delta_p1   = fit02->GetParameter(1);
  Double_t Delta_p1_e = fit02->GetParError(1);
  Double_t Delta_p2   = fit02->GetParameter(2);
  Double_t Delta_p2_e = fit02->GetParError(2);

  Double_t Sum_p0   = fit03->GetParameter(0);
  Double_t Sum_p0_e = fit03->GetParError(0);
  Double_t Sum_p1   = fit03->GetParameter(1);
  Double_t Sum_p1_e = fit03->GetParError(1);
  Double_t Sum_p2   = fit03->GetParameter(2);
  Double_t Sum_p2_e = fit03->GetParError(2);

  Double_t theta1[NUM2]={theta_qweak};
  Double_t theta2[NUM2]={theta_qweak2};
  Double_t theta3[NUM2]={theta_qweak3};

  TGraphErrors *qweakFinalAsymmetry_stat = new TGraphErrors(NUM2,theta1,AnQweakFinal,errx2,dAnQweakFinal_stat);
  qweakFinalAsymmetry_stat->SetName("qweakFinalAsymmetry_stat");
  qweakFinalAsymmetry_stat->Draw("goff");
  qweakFinalAsymmetry_stat->SetMarkerStyle(21);
  qweakFinalAsymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry_stat->SetMarkerColor(kBlue);
  qweakFinalAsymmetry_stat->SetLineColor(kBlue);
  qweakFinalAsymmetry_stat->SetLineWidth(2.0);

  TGraphErrors *qweakFinalAsymmetry = new TGraphErrors(NUM2,theta1,AnQweakFinal,errx2,dAnQweakFinal);
  qweakFinalAsymmetry->SetName("qweakFinalAsymmetry");
  qweakFinalAsymmetry->Draw("goff");
  qweakFinalAsymmetry->SetMarkerStyle(21);
  qweakFinalAsymmetry->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry->SetMarkerColor(kBlue);
  qweakFinalAsymmetry->SetLineColor(kBlue);
  qweakFinalAsymmetry->SetLineWidth(2.0);

//   TGraphErrors *qweakFinal6000Asymmetry_stat = new TGraphErrors(NUM2,theta2,AnQweakFinal6000,errx2,dAnQweakFinal6000_stat);
//   qweakFinal6000Asymmetry_stat->SetName("qweakFinal6000Asymmetry_stat");
//   qweakFinal6000Asymmetry_stat->Draw("goff");
//   qweakFinal6000Asymmetry_stat->SetMarkerStyle(25);
//   qweakFinal6000Asymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
//   qweakFinal6000Asymmetry_stat->SetMarkerColor(kBlue);
//   qweakFinal6000Asymmetry_stat->SetLineColor(kBlue);

//   TGraphErrors *qweakFinal6000Asymmetry = new TGraphErrors(NUM2,theta2,AnQweakFinal6000,errx2,dAnQweakFinal6000);
//   qweakFinal6000Asymmetry->SetName("qweakFinal6000Asymmetry");
//   qweakFinal6000Asymmetry->Draw("goff");
//   qweakFinal6000Asymmetry->SetMarkerStyle(25);
//   qweakFinal6000Asymmetry->SetMarkerSize(markerSize[0]*figSize);
//   qweakFinal6000Asymmetry->SetMarkerColor(kBlue);
//   qweakFinal6000Asymmetry->SetLineColor(kBlue);

//   TGraphErrors *qweakFinal7300Asymmetry_stat = new TGraphErrors(NUM2,theta3,AnQweakFinal7300,errx2,dAnQweakFinal7300_stat);
//   qweakFinal7300Asymmetry_stat->SetName("qweakFinal7300Asymmetry_stat");
//   qweakFinal7300Asymmetry_stat->Draw("goff");
//   qweakFinal7300Asymmetry_stat->SetMarkerStyle(25);
//   qweakFinal7300Asymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
//   qweakFinal7300Asymmetry_stat->SetMarkerColor(kBlue);
//   qweakFinal7300Asymmetry_stat->SetLineColor(kBlue);

//   TGraphErrors *qweakFinal7300Asymmetry = new TGraphErrors(NUM2,theta3,AnQweakFinal7300,errx2,dAnQweakFinal7300);
//   qweakFinal7300Asymmetry->SetName("qweakFinal7300Asymmetry");
//   qweakFinal7300Asymmetry->Draw("goff");
//   qweakFinal7300Asymmetry->SetMarkerStyle(25);
//   qweakFinal7300Asymmetry->SetMarkerSize(markerSize[0]*figSize);
//   qweakFinal7300Asymmetry->SetMarkerColor(kBlue);
//   qweakFinal7300Asymmetry->SetLineColor(kBlue);


  gStyle->SetLineWidth(1.0);

  TMultiGraph * MyGraphAll = new TMultiGraph();
  MyGraphAll->Add(MyGraph01,"P");
  MyGraphAll->Add(MyGraph02,"P");
  MyGraphAll->Add(MyGraph03,"P");
  MyGraphAll->Add(qweakFinalAsymmetry_stat,"[]");
  MyGraphAll->Add(qweakFinalAsymmetry,"P");
//   MyGraphAll->Add(qweakFinal6000Asymmetry_stat,"[]");
//   MyGraphAll->Add(qweakFinal6000Asymmetry,"P");
//   MyGraphAll->Add(qweakFinal7300Asymmetry_stat,"[]");
//   MyGraphAll->Add(qweakFinal7300Asymmetry,"P");
  MyGraphAll->Draw("A");
  MyGraphAll->SetTitle("");
  ////  MyGraphAll->GetYaxis()->SetTitle("Beam Normal Spin Asymmetry [ppm]");
  MyGraphAll->GetYaxis()->SetTitle("B_{n} [ppm]");
  MyGraphAll->GetYaxis()->CenterTitle();
  MyGraphAll->GetXaxis()->SetTitle("#theta_{lab} [degree]");
  MyGraphAll->GetXaxis()->CenterTitle();
  MyGraphAll->GetYaxis()->SetTitleOffset(0.72);
  MyGraphAll->GetXaxis()->SetTitleOffset(1.00);
  MyGraphAll->GetXaxis()->SetNdivisions(9,5,0);
  MyGraphAll->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisMyGraphAll= MyGraphAll->GetXaxis();
  xaxisMyGraphAll->SetLimits(5.0,13.0);
  MyGraphAll->GetYaxis()->SetRangeUser(0.0,70.0);

  fit01->Draw("same");
  fit02->Draw("same");
  fit03->Draw("same");


//   TPaveStats *stats01 = (TPaveStats*)MyGraph01->GetListOfFunctions()->FindObject("stats");
//   stats01->SetTextColor(kGreen+2);
//   stats01->SetFillColor(kWhite); 
//   stats01->SetTextSize(0.045); 
//   stats01->SetX1NDC(x_lo_stat); stats01->SetX2NDC(x_hi_stat); stats01->SetY1NDC(0.16);stats01->SetY2NDC(0.41);

//   TPaveStats *stats02 = (TPaveStats*)MyGraph02->GetListOfFunctions()->FindObject("stats");
//   stats02->SetTextColor(kRed);
//   stats02->SetFillColor(kWhite); 
//   stats02->SetTextSize(0.045); 
//   stats02->SetX1NDC(x_lo_stat); stats02->SetX2NDC(x_hi_stat); stats02->SetY1NDC(0.43);stats02->SetY2NDC(0.68);

//   TPaveStats *stats03 = (TPaveStats*)MyGraph03->GetListOfFunctions()->FindObject("stats");
//   stats03->SetTextColor(kBlack);
//   stats03->SetFillColor(kWhite); 
//   stats03->SetTextSize(0.045); 
//   stats03->SetX1NDC(x_lo_stat); stats03->SetX2NDC(x_hi_stat); stats03->SetY1NDC(0.70);stats03->SetY2NDC(0.95);


  TLegend *legend01 = new TLegend(0.730,0.800,0.942,0.940,"","brNDC");
  legend01->AddEntry(qweakFinalAsymmetry, Form("Q-weak"), "lp");
  legend01->AddEntry(MyGraph03, Form("B.Pasquini"), "lp");
  legend01->SetFillColor(0);
  legend01->SetBorderSize(2);
  legend01->SetTextSize(textSize[0]);
  legend01->Draw("");

  TLatex* tTextBarbara01 = new TLatex(11.5,NBarbara[3]*0.55,Form("N"));
  tTextBarbara01->SetTextSize(textSize[0]);
  tTextBarbara01->SetTextColor(kGreen+2);
  tTextBarbara01->Draw();

  TLatex* tTextBarbara02 = new TLatex(11.5,DeltaBarbara[3]*1.12,Form("#Delta"));
  tTextBarbara02->SetTextSize(textSize[0]);
  tTextBarbara02->SetTextColor(kRed);
  tTextBarbara02->Draw();

//   TColor *color = gROOT->GetColor(10);
//   color->SetRGB(0.1,0.2,0.3);
//   Int_t colorDarkGreen = 3;
//   TColor *color = new TColor(colorDarkGreen, 0.1,0.2,0.3);

  TLatex* tTextBarbara03 = new TLatex(11.0,SumBarbara[3]*1.12,Form("Sum (   +   )"));
  tTextBarbara03->SetTextSize(textSize[0]);
  tTextBarbara03->SetTextColor(kBlack);
  tTextBarbara03->Draw();

  TLatex* tTextBarbara03_1 = new TLatex(11.75,SumBarbara[3]*1.12,Form("N"));
  tTextBarbara03_1->SetTextSize(textSize[0]);
  tTextBarbara03_1->SetTextColor(kGreen+2);
  tTextBarbara03_1->Draw();

  TLatex* tTextBarbara03_2 = new TLatex(12.16,SumBarbara[3]*1.12,Form("#Delta"));
  tTextBarbara03_2->SetTextSize(textSize[0]);
  tTextBarbara03_2->SetTextColor(kRed);
  tTextBarbara03_2->Draw();

  Double_t BnBarbaraQweak   = Sum_p0 + Sum_p1/theta_qweak;
  Double_t BnBarbaraQweak_e = TMath::Abs((Sum_p1*theta_qweak_error)/(theta_qweak*theta_qweak));
//   Double_t BnBarbaraQweak   = Sum_p0 + exp(theta_qweak*Sum_p1+Sum_p2);

//   TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.01,0.75*BnBarbaraQweak,Form("B_{n}(from fit)_{#theta_{lab}=%1.2f^{o}} = %1.2f",theta_qweak,BnBarbaraQweak));
  TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.020,0.70*BnBarbaraQweak,Form("%0.2f (from fit)",BnBarbaraQweak));
  tTextAnBarbaraQweak->SetTextSize(textSize[0]);
  tTextAnBarbaraQweak->SetTextColor(kBlack);
  tTextAnBarbaraQweak->Draw();

  TLatex* tTextFinalQweak01 = new TLatex(theta_qweak*1.020,AnQweakFinal[0]*0.96,Form("%0.2f #pm %0.2f",AnQweakFinal[0],dAnQweakFinal[0]));
//   TLatex* tTextFinalQweak01 = new TLatex(theta_qweak*1.015,AnQweakFinal[0]*0.97,Form("%1.2f #pm %1.2f",AnQweakFinal[0],dAnQweakFinal[0]));
  tTextFinalQweak01->SetTextSize(textSize[0]);
  tTextFinalQweak01->SetTextColor(kBlue);
  tTextFinalQweak01->Draw();

//   TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,BnBarbaraQweak);
  TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,70.0);
  lineQweakTheta->SetLineColor(kGray+2);
  lineQweakTheta->SetLineStyle(3);
//   lineQweakTheta->Draw("");

  TArrow ar1(8.55,29.0,8.38,33.42,0.01,"|>");
  ar1.SetAngle(60);
  ar1.SetLineWidth(2);
  ar1.SetFillColor(kBlack);
  ar1.Draw("");

//   TF1 *funSigma = new TF1("funSigma",Form("(%f + (%f/x)) - ((%f + (%f/x))*0.683)",Sum_p0,Sum_p1,Sum_p0,Sum_p1),5.5,12.5);
  TF1 *funSigmaLow = new TF1("funSigmaLow",Form("(%f + (%f/x))*0.90",Sum_p0,Sum_p1),5.5,12.5);
  funSigmaLow->SetLineColor(kGray);
  funSigmaLow->SetLineStyle(2);
  funSigmaLow->SetLineWidth(2);
//   funSigmaLow->SetFillStyle(3002);
//   funSigmaLow->SetFillColor(kGreen+1);

  TF1 *funSigmaHigh = new TF1("funSigmaHigh",Form("(%f + (%f/x))*1.10",Sum_p0,Sum_p1),5.5,12.5);
  funSigmaHigh->SetLineColor(kGray);
  funSigmaHigh->SetLineStyle(2);
  funSigmaHigh->SetLineWidth(2);

//   funSigmaLow->Draw("same");
//   funSigmaHigh->Draw("same");

  gPad->Update();


  TString savePlot = Form("dirPlot/finalPlot/thetaDependenceOfAsymmetry");
  
  canvas01->Update();
  canvas01->Print(savePlot+".png");

  }

 //*************************************************//
 //*************************************************//

  if(THETA3){

  TCanvas * canvas02 = new TCanvas("canvas02", "canvas02",0,0,canvasSize2[0],canvasSize2[1]);
  canvas02->Draw();
  canvas02->SetBorderSize(0);
  canvas02->cd();
  TPad*pad11 = new TPad("pad11","pad11",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad12 = new TPad("pad12","pad12",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad11->SetFillColor(kWhite);
  pad11->Draw();
  pad12->Draw();
  pad11->cd();
  TLatex * t01 = new TLatex(0.0,0.3,Form("Scattering Angle Dependence of Beam Normal Single Spin Asymmetry at E_{beam} = 1.160 GeV(B. Pasquini), Fit: p_{0} + p_{1}/#theta_{lab}"));
//   TLatex * t01 = new TLatex(0.0,0.3,Form("Scattering Angle Dependence of Beam Normal Single Spin Asymmetry at E_{beam} = 1.160 GeV(B. Pasquini), Fit: p0 + exp(x*p1+p2)"));
  t01->SetTextSize(0.6);
  t01->Draw();
  pad12->cd();

  gStyle->SetLineWidth(2.0);
//   gPad->SetGrid();
  TGraphErrors *MyGraph01 = new TGraphErrors(NUM2,thetaBarbara,NBarbara,errx2,errx2);
  MyGraph01->SetName("MyGraph01");
  MyGraph01->Draw("goff");
  MyGraph01->SetMarkerStyle(1);
  MyGraph01->SetMarkerSize(markerSize[0]*figSize);
  MyGraph01->SetMarkerColor(kGreen+2);
  MyGraph01->SetLineColor(kGreen+2);
  MyGraph01->Fit("fitfun01","R0EF");
  TF1* fit01 = MyGraph01->GetFunction("fitfun01");
  fit01->SetLineColor(kGreen+2);
  fit01->SetLineStyle(2);
  fit01->SetLineWidth(3);
  fit01->SetParNames("N_{p0}","N_{p1}","N_{p2}");

  TGraphErrors *MyGraph02 = new TGraphErrors(NUM2,thetaBarbara,DeltaBarbara,errx2,errx2);
  MyGraph02->SetName("MyGraph02");
  MyGraph02->Draw("goff");
  MyGraph02->SetMarkerStyle(1);
  MyGraph02->SetMarkerSize(markerSize[0]*figSize);
  MyGraph02->SetMarkerColor(kRed);
  MyGraph02->SetLineColor(kRed);
  MyGraph02->Fit("fitfun01","R0EF");
  TF1* fit02 = MyGraph02->GetFunction("fitfun01");
  fit02->SetLineColor(kRed);
  fit02->SetLineStyle(9);
  fit02->SetLineWidth(3);
  fit02->SetParNames("#Delta_{p0}","#Delta_{p1}","#Delta_{p2}");

  TGraphErrors *MyGraph03 = new TGraphErrors(NUM2,thetaBarbara,SumBarbara,errx2,errx2);
  MyGraph03->SetName("MyGraph03");
  MyGraph03->Draw("goff");
  MyGraph03->SetMarkerStyle(1);
  MyGraph03->SetMarkerSize(markerSize[0]*figSize);
  MyGraph03->SetMarkerColor(kBlack);
  MyGraph03->SetLineColor(kBlack);
  MyGraph03->Fit("fitfun01","R0EF");
  TF1* fit03 = MyGraph03->GetFunction("fitfun01");
  fit03->SetLineColor(kBlack);
  fit03->SetLineStyle(1);
  fit03->SetLineWidth(3);
  fit03->SetParNames("N+#Delta_{p0}","N+#Delta_{p1}","N+#Delta_{p2}");

  Double_t N_p0   = fit01->GetParameter(0);
  Double_t N_p0_e = fit01->GetParError(0);
  Double_t N_p1   = fit01->GetParameter(1);
  Double_t N_p1_e = fit01->GetParError(1);
  Double_t N_p2   = fit01->GetParameter(2);
  Double_t N_p2_e = fit01->GetParError(2);

  Double_t Delta_p0   = fit02->GetParameter(0);
  Double_t Delta_p0_e = fit02->GetParError(0);
  Double_t Delta_p1   = fit02->GetParameter(1);
  Double_t Delta_p1_e = fit02->GetParError(1);
  Double_t Delta_p2   = fit02->GetParameter(2);
  Double_t Delta_p2_e = fit02->GetParError(2);

  Double_t Sum_p0   = fit03->GetParameter(0);
  Double_t Sum_p0_e = fit03->GetParError(0);
  Double_t Sum_p1   = fit03->GetParameter(1);
  Double_t Sum_p1_e = fit03->GetParError(1);
  Double_t Sum_p2   = fit03->GetParameter(2);
  Double_t Sum_p2_e = fit03->GetParError(2);

  Double_t theta1[NUM2]={theta_qweak};
  Double_t theta2[NUM2]={theta_qweak2};
  Double_t theta3[NUM2]={theta_qweak3};

  TGraphErrors *qweakFinalAsymmetry_stat = new TGraphErrors(NUM2,theta1,AnQweakFinal,errx2,dAnQweakFinal_stat);
  qweakFinalAsymmetry_stat->SetName("qweakFinalAsymmetry_stat");
  qweakFinalAsymmetry_stat->Draw("goff");
  qweakFinalAsymmetry_stat->SetMarkerStyle(21);
  qweakFinalAsymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry_stat->SetMarkerColor(kBlue);
  qweakFinalAsymmetry_stat->SetLineColor(kBlue);
  qweakFinalAsymmetry_stat->SetLineWidth(2.0);

  TGraphErrors *qweakFinalAsymmetry = new TGraphErrors(NUM2,theta1,AnQweakFinal,errx2,dAnQweakFinal);
  qweakFinalAsymmetry->SetName("qweakFinalAsymmetry");
  qweakFinalAsymmetry->Draw("goff");
  qweakFinalAsymmetry->SetMarkerStyle(21);
  qweakFinalAsymmetry->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry->SetMarkerColor(kBlue);
  qweakFinalAsymmetry->SetLineColor(kBlue);
  qweakFinalAsymmetry->SetLineWidth(2.0);

  TGraphErrors *qweakFinal6000Asymmetry_stat = new TGraphErrors(NUM2,theta2,AnQweakFinal6000,errx2,dAnQweakFinal6000_stat);
  qweakFinal6000Asymmetry_stat->SetName("qweakFinal6000Asymmetry_stat");
  qweakFinal6000Asymmetry_stat->Draw("goff");
  qweakFinal6000Asymmetry_stat->SetMarkerStyle(25);
  qweakFinal6000Asymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
  qweakFinal6000Asymmetry_stat->SetMarkerColor(kBlue);
  qweakFinal6000Asymmetry_stat->SetLineColor(kBlue);

  TGraphErrors *qweakFinal6000Asymmetry = new TGraphErrors(NUM2,theta2,AnQweakFinal6000,errx2,dAnQweakFinal6000);
  qweakFinal6000Asymmetry->SetName("qweakFinal6000Asymmetry");
  qweakFinal6000Asymmetry->Draw("goff");
  qweakFinal6000Asymmetry->SetMarkerStyle(25);
  qweakFinal6000Asymmetry->SetMarkerSize(markerSize[0]*figSize);
  qweakFinal6000Asymmetry->SetMarkerColor(kBlue);
  qweakFinal6000Asymmetry->SetLineColor(kBlue);

  TGraphErrors *qweakFinal7300Asymmetry_stat = new TGraphErrors(NUM2,theta3,AnQweakFinal7300,errx2,dAnQweakFinal7300_stat);
  qweakFinal7300Asymmetry_stat->SetName("qweakFinal7300Asymmetry_stat");
  qweakFinal7300Asymmetry_stat->Draw("goff");
  qweakFinal7300Asymmetry_stat->SetMarkerStyle(25);
  qweakFinal7300Asymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
  qweakFinal7300Asymmetry_stat->SetMarkerColor(kBlue);
  qweakFinal7300Asymmetry_stat->SetLineColor(kBlue);

  TGraphErrors *qweakFinal7300Asymmetry = new TGraphErrors(NUM2,theta3,AnQweakFinal7300,errx2,dAnQweakFinal7300);
  qweakFinal7300Asymmetry->SetName("qweakFinal7300Asymmetry");
  qweakFinal7300Asymmetry->Draw("goff");
  qweakFinal7300Asymmetry->SetMarkerStyle(25);
  qweakFinal7300Asymmetry->SetMarkerSize(markerSize[0]*figSize);
  qweakFinal7300Asymmetry->SetMarkerColor(kBlue);
  qweakFinal7300Asymmetry->SetLineColor(kBlue);


  gStyle->SetLineWidth(1.0);

  TMultiGraph * MyGraphAll = new TMultiGraph();
  MyGraphAll->Add(MyGraph01,"P");
  MyGraphAll->Add(MyGraph02,"P");
  MyGraphAll->Add(MyGraph03,"P");
  MyGraphAll->Add(qweakFinalAsymmetry_stat,"[]");
  MyGraphAll->Add(qweakFinalAsymmetry,"P");
  MyGraphAll->Add(qweakFinal6000Asymmetry_stat,"[]");
  MyGraphAll->Add(qweakFinal6000Asymmetry,"P");
  MyGraphAll->Add(qweakFinal7300Asymmetry_stat,"[]");
  MyGraphAll->Add(qweakFinal7300Asymmetry,"P");
  MyGraphAll->Draw("A");
  MyGraphAll->SetTitle("");
  ////  MyGraphAll->GetYaxis()->SetTitle("Beam Normal Spin Asymmetry [ppm]");
  MyGraphAll->GetYaxis()->SetTitle("B_{n} [ppm]");
  MyGraphAll->GetYaxis()->CenterTitle();
  MyGraphAll->GetXaxis()->SetTitle("#theta_{lab} [degree]");
  MyGraphAll->GetXaxis()->CenterTitle();
  MyGraphAll->GetYaxis()->SetTitleOffset(0.72);
  MyGraphAll->GetXaxis()->SetTitleOffset(1.00);
  MyGraphAll->GetXaxis()->SetNdivisions(9,5,0);
  MyGraphAll->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisMyGraphAll= MyGraphAll->GetXaxis();
  xaxisMyGraphAll->SetLimits(5.0,13.0);
  MyGraphAll->GetYaxis()->SetRangeUser(0.0,105.0);

  fit01->Draw("same");
  fit02->Draw("same");
  fit03->Draw("same");


//   TPaveStats *stats01 = (TPaveStats*)MyGraph01->GetListOfFunctions()->FindObject("stats");
//   stats01->SetTextColor(kGreen+2);
//   stats01->SetFillColor(kWhite); 
//   stats01->SetTextSize(0.045); 
//   stats01->SetX1NDC(x_lo_stat); stats01->SetX2NDC(x_hi_stat); stats01->SetY1NDC(0.16);stats01->SetY2NDC(0.41);

//   TPaveStats *stats02 = (TPaveStats*)MyGraph02->GetListOfFunctions()->FindObject("stats");
//   stats02->SetTextColor(kRed);
//   stats02->SetFillColor(kWhite); 
//   stats02->SetTextSize(0.045); 
//   stats02->SetX1NDC(x_lo_stat); stats02->SetX2NDC(x_hi_stat); stats02->SetY1NDC(0.43);stats02->SetY2NDC(0.68);

//   TPaveStats *stats03 = (TPaveStats*)MyGraph03->GetListOfFunctions()->FindObject("stats");
//   stats03->SetTextColor(kBlack);
//   stats03->SetFillColor(kWhite); 
//   stats03->SetTextSize(0.045); 
//   stats03->SetX1NDC(x_lo_stat); stats03->SetX2NDC(x_hi_stat); stats03->SetY1NDC(0.70);stats03->SetY2NDC(0.95);


  TLegend *legend01 = new TLegend(0.730,0.800,0.942,0.940,"","brNDC");
  legend01->AddEntry(qweakFinalAsymmetry, Form("Q-weak"), "lp");
  legend01->AddEntry(MyGraph03, Form("B.Pasquini"), "lp");
  legend01->SetFillColor(0);
  legend01->SetBorderSize(2);
  legend01->SetTextSize(textSize[0]);
  legend01->Draw("");

  TLatex* tTextBarbara01 = new TLatex(11.5,NBarbara[3]*0.55,Form("N"));
  tTextBarbara01->SetTextSize(textSize[0]);
  tTextBarbara01->SetTextColor(kGreen+2);
  tTextBarbara01->Draw();

  TLatex* tTextBarbara02 = new TLatex(11.5,DeltaBarbara[3]*1.12,Form("#Delta"));
  tTextBarbara02->SetTextSize(textSize[0]);
  tTextBarbara02->SetTextColor(kRed);
  tTextBarbara02->Draw();

//   TColor *color = gROOT->GetColor(10);
//   color->SetRGB(0.1,0.2,0.3);
//   Int_t colorDarkGreen = 3;
//   TColor *color = new TColor(colorDarkGreen, 0.1,0.2,0.3);

  TLatex* tTextBarbara03 = new TLatex(11.0,SumBarbara[3]*1.12,Form("Sum (   +   )"));
  tTextBarbara03->SetTextSize(textSize[0]);
  tTextBarbara03->SetTextColor(kBlack);
  tTextBarbara03->Draw();

  TLatex* tTextBarbara03_1 = new TLatex(11.75,SumBarbara[3]*1.12,Form("N"));
  tTextBarbara03_1->SetTextSize(textSize[0]);
  tTextBarbara03_1->SetTextColor(kGreen+2);
  tTextBarbara03_1->Draw();

  TLatex* tTextBarbara03_2 = new TLatex(12.15,SumBarbara[3]*1.12,Form("#Delta"));
  tTextBarbara03_2->SetTextSize(textSize[0]);
  tTextBarbara03_2->SetTextColor(kRed);
  tTextBarbara03_2->Draw();

  Double_t BnBarbaraQweak   = Sum_p0 + Sum_p1/theta_qweak;
  Double_t BnBarbaraQweak_e = TMath::Abs((Sum_p1*theta_qweak_error)/(theta_qweak*theta_qweak));
//   Double_t BnBarbaraQweak   = Sum_p0 + exp(theta_qweak*Sum_p1+Sum_p2);

//   TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.01,0.75*BnBarbaraQweak,Form("B_{n}(from fit)_{#theta_{lab}=%1.2f^{o}} = %1.2f",theta_qweak,BnBarbaraQweak));
  TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.025,0.65*BnBarbaraQweak,Form("%1.2f (from fit)",BnBarbaraQweak));
  tTextAnBarbaraQweak->SetTextSize(textSize[0]);
  tTextAnBarbaraQweak->SetTextColor(kBlack);
  tTextAnBarbaraQweak->Draw();

  TLatex* tTextFinalQweak01 = new TLatex(theta_qweak*1.045,AnQweakFinal[0]*0.97,Form("%1.2f #pm %1.2f",AnQweakFinal[0],dAnQweakFinal[0]));
//   TLatex* tTextFinalQweak01 = new TLatex(theta_qweak*1.015,AnQweakFinal[0]*0.97,Form("%1.2f #pm %1.2f",AnQweakFinal[0],dAnQweakFinal[0]));
  tTextFinalQweak01->SetTextSize(textSize[0]);
  tTextFinalQweak01->SetTextColor(kBlue);
  tTextFinalQweak01->Draw();

//   TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,BnBarbaraQweak);
  TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,105.0);
  lineQweakTheta->SetLineColor(kGray+2);
  lineQweakTheta->SetLineStyle(3);
  lineQweakTheta->Draw("");

  TArrow ar1(8.55,29.0,8.37,33.42,0.01,"|>");
  ar1.SetAngle(60);
  ar1.SetLineWidth(2);
  ar1.SetFillColor(kBlack);
  ar1.Draw("");


  gPad->Update();

  MyfileTheory.open("dirPlot/resultText/fitResultBarbara.txt");
  MyfileTheory<<BnBarbaraQweak<<"\t"<<0.0<<"\n"<<endl;

  TString savePlot = Form("dirPlot/finalPlot/thetaDependenceOfAsymmetryAllW");
  
  canvas02->Update();
  canvas02->Print(savePlot+".png");

  }

 //*************************************************//
 //*************************************************//



  if(ASYM){


  TCanvas * canvas11 = new TCanvas("canvas11", "canvas11",0,0,canvasSize[0],canvasSize[1]);
  canvas11->Draw();
  canvas11->SetBorderSize(0);
  canvas11->cd();
  TPad*pad11 = new TPad("pad11","pad11",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad12 = new TPad("pad12","pad12",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad11->SetFillColor(kWhite);
  pad11->Draw();
  pad12->Draw();
  pad11->cd();
  TLatex * t11 = new TLatex(0.06,0.3,Form("Beam Normal Single Spin Asymmetry vs #theta_{lab}(Pasquini et. al.)"));
  t11->SetTextSize(0.7);
  t11->Draw();
  pad12->cd();


//   gPad->SetGrid();
  TGraphErrors *MyGraphPasquiniDelta855 = new TGraphErrors(NUM,thetaPasquiniDelta855,asymPasquiniDelta855,errx,errx);
  MyGraphPasquiniDelta855 ->SetName("MyGraphPasquiniDelta855");
  MyGraphPasquiniDelta855->Draw("0");
  MyGraphPasquiniDelta855->SetMarkerStyle(22);
  MyGraphPasquiniDelta855->SetMarkerSize(markerSize[0]*figSize);
  MyGraphPasquiniDelta855->SetMarkerColor(kRed);
  MyGraphPasquiniDelta855->SetLineColor(kRed);
//   MyGraphPasquiniDelta855->Fit("expo","","",1.5,41.0);
  MyGraphPasquiniDelta855->Fit("fitfunPasquiniDelta855","R0FQ");
  TF1* fitPasquiniDelta855 = MyGraphPasquiniDelta855->GetFunction("fitfunPasquiniDelta855");
  fitPasquiniDelta855->SetLineColor(kRed+1);
  fitPasquiniDelta855->SetLineStyle(1);
  fitPasquiniDelta855->SetLineWidth(2);
  fitPasquiniDelta855->SetParNames("p_{0}^{0.855}","p_{1}^{0.855}");


  TGraphErrors *MyGraphPasquiniDelta570 = new TGraphErrors(NUM,thetaPasquiniDelta570,asymPasquiniDelta570,errx,errx);
  MyGraphPasquiniDelta570 ->SetName("MyGraphPasquiniDelta570");
  MyGraphPasquiniDelta570->Draw("0");
  MyGraphPasquiniDelta570->SetMarkerStyle(20);
  MyGraphPasquiniDelta570->SetMarkerSize(markerSize[0]*figSize);
  MyGraphPasquiniDelta570->SetMarkerColor(kMagenta);
  MyGraphPasquiniDelta570->SetLineColor(kMagenta);
//   MyGraphPasquiniDelta570->Fit("expo","","",1.5,41.0);
  MyGraphPasquiniDelta570->Fit("fitfunPasquiniDelta570","R0FQ");
  TF1* fitPasquiniDelta570 = MyGraphPasquiniDelta570->GetFunction("fitfunPasquiniDelta570");
  fitPasquiniDelta570->SetLineColor(kMagenta+1);
  fitPasquiniDelta570->SetLineStyle(1);
  fitPasquiniDelta570->SetLineWidth(2);
  fitPasquiniDelta570->SetParNames("p_{0}^{0.570}","p_{1}^{0.570}");


  TGraphErrors *MyGraphPasquiniDelta424 = new TGraphErrors(NUM,thetaPasquiniDelta424,asymPasquiniDelta424,errx,errx);
  MyGraphPasquiniDelta424 ->SetName("MyGraphPasquiniDelta424");
  MyGraphPasquiniDelta424->Draw("0");
  MyGraphPasquiniDelta424->SetMarkerStyle(23);
  MyGraphPasquiniDelta424->SetMarkerSize(markerSize[0]*figSize);
  MyGraphPasquiniDelta424->SetMarkerColor(kBlue);
  MyGraphPasquiniDelta424->SetLineColor(kBlue);
//   MyGraphPasquiniDelta424->Fit("expo","","",1.5,41.0);
  MyGraphPasquiniDelta424->Fit("fitfunPasquiniDelta424","R0FQ");
  TF1* fitPasquiniDelta424 = MyGraphPasquiniDelta424->GetFunction("fitfunPasquiniDelta424");
  fitPasquiniDelta424->SetLineColor(kBlue+1);
  fitPasquiniDelta424->SetLineStyle(1);
  fitPasquiniDelta424->SetLineWidth(2);
  fitPasquiniDelta424->SetParNames("p_{0}^{0.424}","p_{1}^{0.424}");



  TMultiGraph *MyGraphPasquini = new TMultiGraph();
  MyGraphPasquini->Add(MyGraphPasquiniDelta855,"P");
  MyGraphPasquini->Add(MyGraphPasquiniDelta570,"P");
  MyGraphPasquini->Add(MyGraphPasquiniDelta424,"P");
  MyGraphPasquini->Draw("A");
  MyGraphPasquini->SetTitle("");
  MyGraphPasquini->GetYaxis()->SetTitle("B_{n} [ppm]");
  MyGraphPasquini->GetYaxis()->CenterTitle();
  MyGraphPasquini->GetXaxis()->SetTitle("#theta_{lab} [deg]");
  MyGraphPasquini->GetXaxis()->CenterTitle();
  MyGraphPasquini->GetYaxis()->SetTitleOffset(0.95);
  MyGraphPasquini->GetXaxis()->SetTitleOffset(1.00);
  MyGraphPasquini->GetXaxis()->SetNdivisions(9,5,0);
  MyGraphPasquini->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisMyGraphPasquini= MyGraphPasquini->GetXaxis();
  xaxisMyGraphPasquini->SetLimits(1.0,45.0);
  MyGraphPasquini->GetYaxis()->SetRangeUser(0.0,400.0);

  fitPasquiniDelta424->DrawCopy("same");
  fitPasquiniDelta570->DrawCopy("same");
  fitPasquiniDelta855->DrawCopy("same");


  Double_t p0_424   = fitPasquiniDelta424->GetParameter(0);
  Double_t p0_424_e = fitPasquiniDelta424->GetParError(0);
  Double_t p1_424   = fitPasquiniDelta424->GetParameter(1);
  Double_t p1_424_e = fitPasquiniDelta424->GetParError(1);

  Double_t p0_570   = fitPasquiniDelta570->GetParameter(0);
  Double_t p0_570_e = fitPasquiniDelta570->GetParError(0);
  Double_t p1_570   = fitPasquiniDelta570->GetParameter(1);
  Double_t p1_570_e = fitPasquiniDelta570->GetParError(1);

  Double_t p0_855   = fitPasquiniDelta855->GetParameter(0);
  Double_t p0_855_e = fitPasquiniDelta855->GetParError(0);
  Double_t p1_855   = fitPasquiniDelta855->GetParameter(1);
  Double_t p1_855_e = fitPasquiniDelta855->GetParError(1);

  Double_t scattering_angle = 8.348;

  Double_t An_424_scattering_angle = exp(p0_424+scattering_angle*p1_424);
  Double_t An_570_scattering_angle = exp(p0_570+scattering_angle*p1_570);
  Double_t An_855_scattering_angle = exp(p0_855+scattering_angle*p1_855);

  cout<<red<<"For Theta ="<<scattering_angle<<" degrees"<<normal<<endl;
  cout<<red<<"At 0.424 GeV An = "<<An_424_scattering_angle<<normal<<endl;
  cout<<red<<"At 0.570 GeV An = "<<An_570_scattering_angle<<normal<<endl;
  cout<<red<<"At 0.855 GeV An = "<<An_855_scattering_angle<<normal<<endl;

  TLatex* tTextAn424 = new TLatex(8.0,1.01*An_424_scattering_angle,Form("B_{n}(0.424 GeV)|_{8.35} = %1.2f",An_424_scattering_angle));
  tTextAn424->SetTextSize(0.030);
  tTextAn424->SetTextColor(kBlue+1);
  TLatex* tTextAn570 = new TLatex(8.0,1.05*An_570_scattering_angle,Form("B_{n}(0.570 GeV)|_{8.35} = %1.2f",An_570_scattering_angle));
  tTextAn570->SetTextSize(0.030);
  tTextAn570->SetTextColor(kMagenta+1);
  TLatex* tTextAn855 = new TLatex(8.0,1.1*An_855_scattering_angle,Form("B_{n}(0.855 GeV)|_{8.35} = %1.2f",An_855_scattering_angle));
  tTextAn855->SetTextSize(0.030);
  tTextAn855->SetTextColor(kRed+1);
  tTextAn424->Draw();
  tTextAn570->Draw();
  tTextAn855->Draw();

  TLine *lineSCE = new TLine(scattering_angle,0.0,scattering_angle,400);
  lineSCE->SetLineColor(kGray+1);
  lineSCE->SetLineStyle(3);
  TLatex* tTextSCE = new TLatex(8.0,300,Form("#theta_{lab} = %1.2f",scattering_angle));
  tTextSCE->SetTextSize(0.030);
  tTextSCE->SetTextColor(kBlack);
  tTextSCE->SetTextAngle(90);
  tTextSCE->Draw();
  lineSCE->Draw();


  TPaveStats *statsPasquiniDelta424 = (TPaveStats*)MyGraphPasquiniDelta424->GetListOfFunctions()->FindObject("stats");
  statsPasquiniDelta424->SetTextColor(kBlue+1);
  statsPasquiniDelta424->SetFillColor(kWhite); 
  statsPasquiniDelta424->SetX1NDC(x_lo_stat); statsPasquiniDelta424->SetX2NDC(x_hi_stat); statsPasquiniDelta424->SetY1NDC(0.58);statsPasquiniDelta424->SetY2NDC(0.77);
  statsPasquiniDelta424->SetTextSize(0.035);

  TPaveStats *statsPasquiniDelta570 = (TPaveStats*)MyGraphPasquiniDelta570->GetListOfFunctions()->FindObject("stats");
  statsPasquiniDelta570->SetTextColor(kMagenta+1);
  statsPasquiniDelta570->SetFillColor(kWhite); 
  statsPasquiniDelta570->SetX1NDC(x_lo_stat); statsPasquiniDelta570->SetX2NDC(x_hi_stat); statsPasquiniDelta570->SetY1NDC(0.36);statsPasquiniDelta570->SetY2NDC(0.55);
  statsPasquiniDelta570->SetTextSize(0.035);

  TPaveStats *statsPasquiniDelta855 = (TPaveStats*)MyGraphPasquiniDelta855->GetListOfFunctions()->FindObject("stats");
  statsPasquiniDelta855->SetTextColor(kRed+1);
  statsPasquiniDelta855->SetFillColor(kWhite); 
  statsPasquiniDelta855->SetX1NDC(x_lo_stat); statsPasquiniDelta855->SetX2NDC(x_hi_stat); statsPasquiniDelta855->SetY1NDC(0.15);statsPasquiniDelta855->SetY2NDC(0.34);
  statsPasquiniDelta855->SetTextSize(0.035);


//   TLegend *legendPasquini = new TLegend(0.63,0.82,0.79,0.94,"","brNDC");
  TLegend *legendPasquini = new TLegend(x_lo_stat,0.79,x_hi_stat,0.95,"","brNDC");
  legendPasquini->AddEntry(MyGraphPasquiniDelta424, Form("E_{e} = 0.424 GeV"), "p");
  legendPasquini->AddEntry(MyGraphPasquiniDelta570, Form("E_{e} = 0.570 GeV"), "p");
  legendPasquini->AddEntry(MyGraphPasquiniDelta855, Form("E_{e} = 0.855 GeV"), "p");
  legendPasquini->SetFillColor(0);
  legendPasquini->SetBorderSize(2);
  legendPasquini->SetTextSize(0.035);
  legendPasquini->Draw("");


  TLatex* tText = new TLatex(36,380,Form("Fit: exp(p_{0} + p_{1} #theta_{lab}) "));
  tText->SetTextSize(0.035);
  tText->SetTextColor(kBlack);
  tText->Draw();

  gPad->Update();


  TString savePlot11 = Form("dirPlot/resultPlot/theoryAsymmetryPasquini");  
  canvas11->Update();
  canvas11->Print(savePlot11+".png");

  ////****************************************************//


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
//   gStyle->SetPadRightMargin(0.20);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.80);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.050,"x");
  gStyle->SetLabelSize(0.050,"y");
  gStyle->SetTitleSize(0.050,"x");
  gStyle->SetTitleSize(0.050,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.075);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gStyle->SetStatFormat("2.2f");
  gStyle->SetFitFormat("2.2f");

  gDirectory->Delete("*");

  TCanvas * canvas21 = new TCanvas("canvas21", "canvas21",0,0,canvasSize[0],canvasSize[1]);
  canvas21->Draw();
  canvas21->SetBorderSize(0);
  canvas21->cd();
  TPad*pad21 = new TPad("pad21","pad21",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad22 = new TPad("pad22","pad22",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad21->SetFillColor(kWhite);
  pad21->Draw();
  pad22->Draw();
  pad21->cd();
  TLatex * t21 = new TLatex(0.06,0.3,Form("Beam Normal Single Spin Asymmetry vs Energy at #theta_{lab} = 8.35 degrees (Pasquini et. al.)"));
  t21->SetTextSize(0.7);
  t21->Draw();
  pad22->cd();

  const Int_t n = 3;
  Double_t An_SCE_array[n]={An_424_scattering_angle,An_570_scattering_angle,An_855_scattering_angle};
  Double_t E_array[n]={0.424,0.570,0.855};
  Double_t erx[n];

  for(Int_t i =0;i<n;i++){
    erx[i] = 0.0;
  }

  TF1 *fitfunPasquiniEDependence = new TF1("fitfunPasquiniEDependence","expo",0.42,1.4);
//   TF1 *fitfunPasquiniEDependence = new TF1("fitfunPasquiniEDependence","[0]+[1]*x^0.5",0.42,1.4);
  fitfunPasquiniEDependence->SetParameters(7.9,-4.7);

  TGraphErrors *PasquiniEDependence = new TGraphErrors(n,E_array,An_SCE_array,erx,erx);
  PasquiniEDependence ->SetName("PasquiniEDependence");
  PasquiniEDependence->Draw("0");
  PasquiniEDependence->SetMarkerStyle(20);
  PasquiniEDependence->SetMarkerSize(markerSize[0]*figSize);
  PasquiniEDependence->SetMarkerColor(kGreen+1);
  PasquiniEDependence->SetLineColor(kGreen+1);
  PasquiniEDependence->Fit("fitfunPasquiniEDependence","R0EFQ");
  TF1* fitPasquiniEDependence = PasquiniEDependence->GetFunction("fitfunPasquiniEDependence");
  fitPasquiniEDependence->SetLineColor(kGreen+2);
  fitPasquiniEDependence->SetLineStyle(2);
  fitPasquiniEDependence->SetLineWidth(2);
  fitPasquiniEDependence->SetParNames("p_{0}","p_{1}");
//   PasquiniEDependence->SetTitle("");
//   PasquiniEDependence->GetYaxis()->SetTitle("B_{n} [ppm]");
//   PasquiniEDependence->GetYaxis()->CenterTitle();
//   PasquiniEDependence->GetXaxis()->SetTitle("Energy [GeV]");
//   PasquiniEDependence->GetXaxis()->CenterTitle();
//   PasquiniEDependence->GetYaxis()->SetTitleOffset(0.95);
//   PasquiniEDependence->GetXaxis()->SetTitleOffset(1.00);
//   PasquiniEDependence->GetXaxis()->SetNdivisions(9,5,0);
//   PasquiniEDependence->GetYaxis()->SetNdivisions(8,5,0);
//   TAxis *xaxisPasquiniEDependence= PasquiniEDependence->GetXaxis();
//   xaxisPasquiniEDependence->SetLimits(0.35,1.4);
//   PasquiniEDependence->GetYaxis()->SetRangeUser(0.0,400.0);


  Double_t p0_Energy   = fitPasquiniEDependence->GetParameter(0);
  Double_t p0_Energy_e = fitPasquiniEDependence->GetParError(0);
  Double_t p1_Energy   = fitPasquiniEDependence->GetParameter(1);
  Double_t p1_Energy_e = fitPasquiniEDependence->GetParError(1);
  Double_t beamEnergy  = 1.155;
  Double_t An_Energy   = exp(p0_Energy+beamEnergy*p1_Energy);


  const Int_t n1 = 1;
//   Double_t An1[n1]={40.02};
//   Double_t eAn1[n1]={14.32};
  Double_t E1[n1]={beamEnergy};
  Double_t errx1[n1];

  for(Int_t j =0;j<NUM1;j++){
    errx1[j] = 0.0;
  }

//   TGraphErrors *qweakFinalAsymmetry = new TGraphErrors(n1,E1,An1,errx1,eAn1);
//   qweakFinalAsymmetry ->SetName("qweakFinalAsymmetry");
//   qweakFinalAsymmetry->Draw("0");
//   qweakFinalAsymmetry->SetMarkerStyle(21);
//   qweakFinalAsymmetry->SetMarkerSize(markerSize[0]*figSize);
//   qweakFinalAsymmetry->SetMarkerColor(kRed);
//   qweakFinalAsymmetry->SetLineColor(kRed);

  TGraphErrors *qweakFinalAsymmetry = new TGraphErrors(NUM1,E1,AnQweakFinal,errx1,dAnQweakFinal);
  qweakFinalAsymmetry ->SetName("qweakFinalAsymmetry");
  qweakFinalAsymmetry->Draw("0");
  qweakFinalAsymmetry->SetMarkerStyle(21);
  qweakFinalAsymmetry->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry->SetMarkerColor(kRed);
  qweakFinalAsymmetry->SetLineColor(kRed);


  TMultiGraph *finalEDependence = new TMultiGraph();
  finalEDependence->Add(PasquiniEDependence,"P");
  finalEDependence->Add(qweakFinalAsymmetry,"P");
  finalEDependence->Draw("A");
  finalEDependence->SetTitle("");
  finalEDependence->GetYaxis()->SetTitle("B_{n} [ppm]");
  finalEDependence->GetYaxis()->CenterTitle();
  finalEDependence->GetXaxis()->SetTitle("Energy [GeV]");
  finalEDependence->GetXaxis()->CenterTitle();
  finalEDependence->GetYaxis()->SetTitleOffset(0.95);
  finalEDependence->GetXaxis()->SetTitleOffset(1.00);
  finalEDependence->GetXaxis()->SetNdivisions(9,5,0);
  finalEDependence->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisfinalEDependence= finalEDependence->GetXaxis();
  xaxisfinalEDependence->SetLimits(0.35,1.525);
  finalEDependence->GetYaxis()->SetRangeUser(0.0,400.0);

  fitPasquiniEDependence->DrawCopy("same");


  TPaveStats *statsAnEnergy = (TPaveStats*)PasquiniEDependence->GetListOfFunctions()->FindObject("stats");
  statsAnEnergy->SetTextColor(kGreen+2);
  statsAnEnergy->SetFillColor(kWhite);
//   statsAnEnergy->SetX1NDC(x_lo_stat-0.2); statsAnEnergy->SetX2NDC(x_hi_stat-0.20); statsAnEnergy->SetY1NDC(0.58);statsAnEnergy->SetY2NDC(0.77);
  statsAnEnergy->SetX1NDC(x_lo_stat-0.10); statsAnEnergy->SetX2NDC(x_hi_stat-0.05); statsAnEnergy->SetY1NDC(0.75);statsAnEnergy->SetY2NDC(0.94);
  statsAnEnergy->SetTextSize(textSize[0]);

  TLegend *legendAnEnergy = new TLegend(x_lo_stat-0.42,0.75,x_hi_stat-0.29,0.94,"","brNDC");
  legendAnEnergy->AddEntry(PasquiniEDependence, Form("B_{n} (E),Pasquini et. al."),"p");
  legendAnEnergy->AddEntry(qweakFinalAsymmetry, Form("B_{n} [Q-weak]"),"p");
  legendAnEnergy->SetFillColor(0);
  legendAnEnergy->SetBorderSize(2);
  legendAnEnergy->SetTextSize(textSize[0]);
  legendAnEnergy->Draw("");


  TLatex* tTextFinalQweak = new TLatex(beamEnergy*1.01,AnQweakFinal[0]*0.75,Form("%1.2f #pm %1.2f",AnQweakFinal[0],dAnQweakFinal[0]));
  tTextFinalQweak->SetTextSize(textSize[0]);
  tTextFinalQweak->SetTextColor(kRed);
  tTextFinalQweak->Draw();

//   TLatex* tTextAnEnergy = new TLatex(beamEnergy*1.01,1.45*An_Energy,Form("B_{n}(#theta_{lab}=%1.2f)|_{1.155} = %1.2f",scattering_angle,An_Energy));
  TLatex* tTextAnEnergy = new TLatex(beamEnergy*0.50,1.15*An_Energy,Form("B_{n}[from fit]_{Q-weak} = %1.2f",An_Energy));
  tTextAnEnergy->SetTextSize(textSize[0]);
  tTextAnEnergy->SetTextColor(kGreen+2);
  tTextAnEnergy->Draw();

  TLine *lineEnergy = new TLine(beamEnergy,0.0,beamEnergy,An_Energy);
  lineEnergy->SetLineColor(kGray+2);
  lineEnergy->SetLineStyle(3);
  TLatex* tTextEnergy = new TLatex(beamEnergy*0.99,300,Form("E_{Q-weak} = %1.3f",beamEnergy));
  tTextEnergy->SetTextSize(textSize[0]);
  tTextEnergy->SetTextColor(kBlack);
  tTextEnergy->SetTextAngle(90);

  TLine *lineEnergyH = new TLine(0.35,An_Energy,beamEnergy,An_Energy);
  lineEnergyH->SetLineColor(kGray+2);
  lineEnergyH->SetLineStyle(3);

//   tTextEnergy->Draw();
  lineEnergy->Draw();
  lineEnergyH->Draw();


  TLatex* tTextFitE = new TLatex(1.210,270,Form("Fit: exp(p_{0} + p_{1} E) "));
  tTextFitE->SetTextSize(textSize[0]);
  tTextFitE->SetTextColor(kGreen+2);
  tTextFitE->Draw();


  TString savePlot21 = Form("dirPlot/resultPlot/theoryAsymmetryPasquiniEnergyDependence");  
  canvas21->Update();
  canvas21->Print(savePlot21+".png");

  }




 //*************************************************//
 //*************************************************//
  // Fit and stat parameters
  gStyle->SetOptFit(1111);
//   gStyle->SetOptFit(0);
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
  gStyle->SetPadRightMargin(0.25);
  //  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.80);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.060,"x");
  gStyle->SetLabelSize(0.060,"y");
  gStyle->SetTitleSize(0.065,"x");
  gStyle->SetTitleSize(0.065,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.075);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gStyle->SetStatFormat("2.2f");
  gStyle->SetFitFormat("2.2f");

  gDirectory->Delete("*");



  if(THETA1){

  TCanvas * canvas001 = new TCanvas("canvas001", "canvas001",0,0,canvasSize2[0],canvasSize2[1]);
  canvas001->Draw();
  canvas001->SetBorderSize(0);
  canvas001->cd();
  TPad*pad011 = new TPad("pad011","pad011",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad012 = new TPad("pad012","pad012",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad011->SetFillColor(kWhite);
  pad011->Draw();
  pad012->Draw();
  pad011->cd();
  TLatex * t001 = new TLatex(0.0,0.3,Form("Scattering Angle Dependence of Beam Normal Single Spin Asymmetry at E_{beam} = 1.160 GeV(B. Pasquini), Fit: p_{0} + p_{1}/#theta_{lab}"));
  t001->SetTextSize(0.5);
  t001->Draw();
  pad012->cd();

  gStyle->SetLineWidth(2.0);
//   gPad->SetGrid();
  TGraphErrors *MyGraph001 = new TGraphErrors(NUM2,thetaBarbara,NBarbara,errx2,errx2);
  MyGraph001->SetName("MyGraph001");
  MyGraph001->Draw("goff");
  MyGraph001->SetMarkerStyle(20);
  MyGraph001->SetMarkerSize(markerSize[0]*figSize);
  MyGraph001->SetMarkerColor(kGreen+2);
  MyGraph001->SetLineColor(kGreen+2);
  MyGraph001->Fit("fitfun01","R0EF");
  TF1* fit001 = MyGraph001->GetFunction("fitfun01");
  fit001->SetLineColor(kGreen+2);
  fit001->SetLineStyle(2);
  fit001->SetLineWidth(3);
  fit001->SetParNames("N_{p0}","N_{p1}","N_{p2}");

  TGraphErrors *MyGraph002 = new TGraphErrors(NUM2,thetaBarbara,DeltaBarbara,errx2,errx2);
  MyGraph002->SetName("MyGraph002");
  MyGraph002->Draw("goff");
  MyGraph002->SetMarkerStyle(22);
  MyGraph002->SetMarkerSize(markerSize[0]*figSize);
  MyGraph002->SetMarkerColor(kRed);
  MyGraph002->SetLineColor(kRed);
  MyGraph002->Fit("fitfun01","R0EF");
  TF1* fit002 = MyGraph002->GetFunction("fitfun01");
  fit002->SetLineColor(kRed);
  fit002->SetLineStyle(9);
  fit002->SetLineWidth(3);
  fit002->SetParNames("#Delta_{p0}","#Delta_{p1}","#Delta_{p2}");

  TGraphErrors *MyGraph003 = new TGraphErrors(NUM2,thetaBarbara,SumBarbara,errx2,errx2);
  MyGraph003->SetName("MyGraph003");
  MyGraph003->Draw("goff");
  MyGraph003->SetMarkerStyle(21);
  MyGraph003->SetMarkerSize(markerSize[0]*figSize);
  MyGraph003->SetMarkerColor(kBlack);
  MyGraph003->SetLineColor(kBlack);
  MyGraph003->Fit("fitfun01","R0EF");
  TF1* fit003 = MyGraph003->GetFunction("fitfun01");
  fit003->SetLineColor(kBlack);
  fit003->SetLineStyle(1);
  fit003->SetLineWidth(3);
  fit003->SetParNames("N+#Delta_{p0}","N+#Delta_{p1}","N+#Delta_{p2}");

  Double_t N_p0   = fit001->GetParameter(0);
  Double_t N_p0_e = fit001->GetParError(0);
  Double_t N_p1   = fit001->GetParameter(1);
  Double_t N_p1_e = fit001->GetParError(1);
  Double_t N_p2   = fit001->GetParameter(2);
  Double_t N_p2_e = fit001->GetParError(2);

  Double_t Delta_p0   = fit002->GetParameter(0);
  Double_t Delta_p0_e = fit002->GetParError(0);
  Double_t Delta_p1   = fit002->GetParameter(1);
  Double_t Delta_p1_e = fit002->GetParError(1);
  Double_t Delta_p2   = fit002->GetParameter(2);
  Double_t Delta_p2_e = fit002->GetParError(2);

  Double_t Sum_p0   = fit003->GetParameter(0);
  Double_t Sum_p0_e = fit003->GetParError(0);
  Double_t Sum_p1   = fit003->GetParameter(1);
  Double_t Sum_p1_e = fit003->GetParError(1);
  Double_t Sum_p2   = fit003->GetParameter(2);
  Double_t Sum_p2_e = fit003->GetParError(2);

  Double_t theta1[NUM2]={theta_qweak};
  Double_t theta2[NUM2]={theta_qweak2};
  Double_t theta3[NUM2]={theta_qweak3};



  gStyle->SetLineWidth(1.0);

  TMultiGraph * MyGraphAll0 = new TMultiGraph();
  MyGraphAll0->Add(MyGraph001,"P");
  MyGraphAll0->Add(MyGraph002,"P");
  MyGraphAll0->Add(MyGraph003,"P");
  MyGraphAll0->Draw("A");
  MyGraphAll0->SetTitle("");
  ////  MyGraphAll->GetYaxis()->SetTitle("Beam Normal Spin Asymmetry [ppm]");
  MyGraphAll0->GetYaxis()->SetTitle("B_{n} [ppm]");
  MyGraphAll0->GetYaxis()->CenterTitle();
  MyGraphAll0->GetXaxis()->SetTitle("#theta_{lab} [degree]");
  MyGraphAll0->GetXaxis()->CenterTitle();
  MyGraphAll0->GetYaxis()->SetTitleOffset(0.72);
  MyGraphAll0->GetXaxis()->SetTitleOffset(1.00);
  MyGraphAll0->GetXaxis()->SetNdivisions(9,5,0);
  MyGraphAll0->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisMyGraphAll0= MyGraphAll0->GetXaxis();
  xaxisMyGraphAll0->SetLimits(5.0,13.0);
  MyGraphAll0->GetYaxis()->SetRangeUser(0.0,70.0);

  fit001->Draw("same");
  fit002->Draw("same");
  fit003->Draw("same");

  x_lo_stat=0.755;


  TPaveStats *stats001 = (TPaveStats*)MyGraph001->GetListOfFunctions()->FindObject("stats");
  stats001->SetTextColor(kGreen+2);
  stats001->SetFillColor(kWhite); 
  stats001->SetTextSize(0.045); 
  stats001->SetX1NDC(x_lo_stat); stats001->SetX2NDC(x_hi_stat); stats001->SetY1NDC(0.16);stats001->SetY2NDC(0.41);

  TPaveStats *stats002 = (TPaveStats*)MyGraph002->GetListOfFunctions()->FindObject("stats");
  stats002->SetTextColor(kRed);
  stats002->SetFillColor(kWhite); 
  stats002->SetTextSize(0.045); 
  stats002->SetX1NDC(x_lo_stat); stats002->SetX2NDC(x_hi_stat); stats002->SetY1NDC(0.43);stats002->SetY2NDC(0.68);

  TPaveStats *stats003 = (TPaveStats*)MyGraph003->GetListOfFunctions()->FindObject("stats");
  stats003->SetTextColor(kBlack);
  stats003->SetFillColor(kWhite); 
  stats003->SetTextSize(0.045); 
  stats003->SetX1NDC(x_lo_stat); stats003->SetX2NDC(x_hi_stat); stats003->SetY1NDC(0.70);stats003->SetY2NDC(0.95);


//   TLegend *legend001 = new TLegend(0.730,0.800,0.942,0.940,"","brNDC");
// //   legend001->AddEntry(qweakFinalAsymmetry, Form("Q-weak"), "lp");
//   legend001->AddEntry(MyGraph003, Form("B.Pasquini"), "lp");
//   legend001->SetFillColor(0);
//   legend001->SetBorderSize(2);
//   legend001->SetTextSize(textSize[0]);
//   legend001->Draw("");

//   TLatex* tTextBarbara001 = new TLatex(11.5,NBarbara[3]*0.55,Form("N"));
//   tTextBarbara001->SetTextSize(textSize[0]);
//   tTextBarbara001->SetTextColor(kGreen+2);
//   tTextBarbara001->Draw();

//   TLatex* tTextBarbara002 = new TLatex(11.5,DeltaBarbara[3]*1.12,Form("#Delta"));
//   tTextBarbara002->SetTextSize(textSize[0]);
//   tTextBarbara002->SetTextColor(kRed);
//   tTextBarbara002->Draw();

// //   TColor *color = gROOT->GetColor(10);
// //   color->SetRGB(0.1,0.2,0.3);
// //   Int_t colorDarkGreen = 3;
// //   TColor *color = new TColor(colorDarkGreen, 0.1,0.2,0.3);

  TLatex* tTextBarbara003 = new TLatex(11.0,SumBarbara[3]*1.12,Form("Sum (   +   )"));
  tTextBarbara003->SetTextSize(textSize[0]);
  tTextBarbara003->SetTextColor(kBlack);

  TLatex* tTextBarbara003_1 = new TLatex(12.00,SumBarbara[3]*1.12,Form("N"));
  tTextBarbara003_1->SetTextSize(textSize[0]);
  tTextBarbara003_1->SetTextColor(kGreen+2);

  TLatex* tTextBarbara003_2 = new TLatex(12.56,SumBarbara[3]*1.12,Form("#Delta"));
  tTextBarbara003_2->SetTextSize(textSize[0]);
  tTextBarbara003_2->SetTextColor(kRed);

  tTextBarbara003->Draw();
  tTextBarbara003_1->Draw();
  tTextBarbara003_2->Draw();


//   Double_t BnBarbaraQweakInelastic   = Sum_p0 + exp(theta_qweak*Sum_p1+Sum_p2);
//   Double_t BnBarbaraQweakInelastic_e = TMath::Abs(Sum_p1*exp(theta_qweak*Sum_p1+Sum_p2)*theta_qweak_error);
//   Double_t BnBarbaraQweakElastic     = Sum_p0 + exp(theta_qweak_elastic*Sum_p1+Sum_p2);
//   Double_t BnBarbaraQweakElastic_e   = TMath::Abs(exp(theta_qweak_elastic*Sum_p1+Sum_p2)*theta_qweak_elastic_error);

  Double_t BnBarbaraQweakInelastic   = Sum_p0 + Sum_p1/theta_qweak;
  Double_t BnBarbaraQweakInelastic_e = TMath::Abs((Sum_p1*theta_qweak_error)/(theta_qweak*theta_qweak));
  Double_t BnBarbaraQweakElastic     = Sum_p0 + Sum_p1/theta_qweak_elastic; 
  Double_t BnBarbaraQweakElastic_e   = TMath::Abs((Sum_p1*theta_qweak_elastic_error)/(theta_qweak_elastic*theta_qweak_elastic));

//   TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.01,0.75*BnBarbaraQweak,Form("B_{n}(from fit)_{#theta_{lab}=%1.2f^{o}} = %1.2f",theta_qweak,BnBarbaraQweak));
  TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.02,1.05*BnBarbaraQweakInelastic,Form("%0.2f #pm %0.2f",BnBarbaraQweakInelastic,BnBarbaraQweakInelastic_e));
  tTextAnBarbaraQweak->SetTextSize(textSize[0]);
  tTextAnBarbaraQweak->SetTextColor(kBlack);

  TLatex* tTextAnBarbaraQweakElastic = new TLatex(theta_qweak_elastic*0.720,0.90*BnBarbaraQweakElastic,Form("%0.2f #pm %0.2f",BnBarbaraQweakElastic,BnBarbaraQweakElastic_e));
  tTextAnBarbaraQweakElastic->SetTextSize(textSize[0]);
  tTextAnBarbaraQweakElastic->SetTextColor(kBlack);


//   TLatex* tTextFinalQweak001 = new TLatex(theta_qweak*1.020,AnQweakFinal[0]*0.96,Form("%0.2f #pm %0.2f",AnQweakFinal[0],dAnQweakFinal[0]));
// //   TLatex* tTextFinalQweak001 = new TLatex(theta_qweak*1.015,AnQweakFinal[0]*0.97,Form("%1.2f #pm %1.2f",AnQweakFinal[0],dAnQweakFinal[0]));
//   tTextFinalQweak001->SetTextSize(textSize[0]);
//   tTextFinalQweak001->SetTextColor(kBlue);
//   tTextFinalQweak001->Draw();

//   TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,BnBarbaraQweak);
  TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,70.0);
  lineQweakTheta->SetLineColor(kGray+2);
  lineQweakTheta->SetLineStyle(3);

  TLine *lineQweakThetaElastic = new TLine(theta_qweak_elastic,0.0,theta_qweak_elastic,70.0);
  lineQweakThetaElastic->SetLineColor(kGray+2);
  lineQweakThetaElastic->SetLineStyle(3);

  TLatex* tTextQweakTheta_elastic = new TLatex(theta_qweak_elastic*0.99,0.82*70.0,Form("#theta_{lab}=%0.2f^{o}",theta_qweak_elastic));
  tTextQweakTheta_elastic->SetTextSize(textSize[1]*0.5);
  tTextQweakTheta_elastic->SetTextAngle(90);
  tTextQweakTheta_elastic->SetTextColor(kBlack);

  TLatex* tTextQweakTheta_inelastic = new TLatex(theta_qweak*1.03,0.82*70.0,Form("#theta_{lab}=%0.2f^{o}",theta_qweak));
  tTextQweakTheta_inelastic->SetTextSize(textSize[1]*0.5);
  tTextQweakTheta_inelastic->SetTextAngle(90);
  tTextQweakTheta_inelastic->SetTextColor(kBlack);

  tTextAnBarbaraQweak->Draw();
  tTextAnBarbaraQweakElastic->Draw();
  lineQweakTheta->Draw("");
  lineQweakThetaElastic->Draw("");
  tTextQweakTheta_elastic->Draw();
  tTextQweakTheta_inelastic->Draw();

  TArrow ar1(7.70,33.6,7.88,36.00,0.01,"|>");
  ar1.SetAngle(45);
  ar1.SetLineWidth(2);
  ar1.SetFillColor(kBlack);

  TArrow ar2(8.55,36.1,8.38,34.70,0.01,"|>");
  ar2.SetAngle(30);
  ar2.SetLineWidth(2);
  ar2.SetFillColor(kBlack);

  ar1.Draw("");  ar2.Draw("");

  gPad->Update();


  TString savePlot001 = Form("dirPlot/summaryPlot/thetaDependenceOfAsymmetryTheory");
  
  canvas001->Update();
  canvas001->Print(savePlot001+".png");

  }

//   gStyle->SetOptFit(0);
 //*************************************************//
 //*************************************************//

 //*************************************************//
 //*************************************************//
  // Fit and stat parameters
//   gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);
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
  //  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.80);
  gStyle->SetTitleXOffset(0.95);
  gStyle->SetLabelSize(0.060,"x");
  gStyle->SetLabelSize(0.060,"y");
  gStyle->SetTitleSize(0.065,"x");
  gStyle->SetTitleSize(0.065,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.075);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gStyle->SetStatFormat("2.2f");
  gStyle->SetFitFormat("2.2f");

  gDirectory->Delete("*");


  if(THETA_ALL){

  TCanvas * canvas11 = new TCanvas("canvas11", "canvas11",0,0,canvasSize2[0],canvasSize2[1]);
  canvas11->Draw();
  canvas11->SetBorderSize(0);
  canvas11->cd();
  TPad*pad111 = new TPad("pad111","pad111",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad112 = new TPad("pad112","pad112",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad111->SetFillColor(kWhite);
  pad111->Draw();
  pad112->Draw();
  pad111->cd();
  TLatex * t11 = new TLatex(0.0,0.3,Form("Scattering Angle Dependence of Beam Normal Single Spin Asymmetry at E_{beam} = 1.160 GeV(B. Pasquini), Fit: p_{0} + p_{1}/#theta_{lab}"));
  t11->SetTextSize(0.5);
  t11->Draw();
  pad112->cd();

  gStyle->SetLineWidth(2.0);
//   gPad->SetGrid();
  TGraphErrors *MyGraph01 = new TGraphErrors(NUM2,thetaBarbara,NBarbara,errx2,errx2);
  MyGraph01->SetName("MyGraph01");
  MyGraph01->Draw("goff");
  MyGraph01->SetMarkerStyle(1);
  MyGraph01->SetMarkerSize(markerSize[0]*figSize);
  MyGraph01->SetMarkerColor(kGreen+2);
  MyGraph01->SetLineColor(kGreen+2);
  MyGraph01->Fit("fitfun01","R0EF");
  TF1* fit01 = MyGraph01->GetFunction("fitfun01");
  fit01->SetLineColor(kGreen+2);
  fit01->SetLineStyle(3);
  fit01->SetLineWidth(3);
  fit01->SetParNames("N_{p0}","N_{p1}","N_{p2}");

  TGraphErrors *MyGraph02 = new TGraphErrors(NUM2,thetaBarbara,DeltaBarbara,errx2,errx2);
  MyGraph02->SetName("MyGraph02");
  MyGraph02->Draw("goff");
  MyGraph02->SetMarkerStyle(1);
  MyGraph02->SetMarkerSize(markerSize[0]*figSize);
  MyGraph02->SetMarkerColor(kRed);
  MyGraph02->SetLineColor(kRed);
  MyGraph02->Fit("fitfun01","R0EF");
  TF1* fit02 = MyGraph02->GetFunction("fitfun01");
  fit02->SetLineColor(kRed);
  fit02->SetLineStyle(9);
  fit02->SetLineWidth(3);
  fit02->SetParNames("#Delta_{p0}","#Delta_{p1}","#Delta_{p2}");

  TGraphErrors *MyGraph03 = new TGraphErrors(NUM2,thetaBarbara,SumBarbara,errx2,errx2);
  MyGraph03->SetName("MyGraph03");
  MyGraph03->Draw("goff");
  MyGraph03->SetMarkerStyle(1);
  MyGraph03->SetMarkerSize(markerSize[0]*figSize);
  MyGraph03->SetMarkerColor(kBlack);
  MyGraph03->SetLineColor(kBlack);
  MyGraph03->Fit("fitfun01","R0EF");
  TF1* fit03 = MyGraph03->GetFunction("fitfun01");
  fit03->SetLineColor(kBlack);
  fit03->SetLineStyle(1);
  fit03->SetLineWidth(3);
  fit03->SetParNames("N+#Delta_{p0}","N+#Delta_{p1}","N+#Delta_{p2}");


  TGraphErrors *MyGraph877MeV01 = new TGraphErrors(NUM2,thetaBarbara877MeV,NBarbara877MeV,errx2,errx2);
  MyGraph877MeV01->SetName("MyGraph877MeV01");
  MyGraph877MeV01->Draw("goff");
  MyGraph877MeV01->SetMarkerStyle(1);
  MyGraph877MeV01->SetMarkerSize(markerSize[0]*figSize);
  MyGraph877MeV01->SetMarkerColor(kGreen+2);
  MyGraph877MeV01->SetLineColor(kGreen+2);
  MyGraph877MeV01->Fit("fitfun01","R0EF");
  TF1* fit877MeV01 = MyGraph877MeV01->GetFunction("fitfun01");
  fit877MeV01->SetLineColor(kGreen+2);
  fit877MeV01->SetLineStyle(4);
  fit877MeV01->SetLineWidth(3);
  fit877MeV01->SetParNames("N_{p0}","N_{p1}","N_{p2}");

  TGraphErrors *MyGraph877MeV02 = new TGraphErrors(NUM2,thetaBarbara877MeV,DeltaBarbara877MeV,errx2,errx2);
  MyGraph877MeV02->SetName("MyGraph877MeV02");
  MyGraph877MeV02->Draw("goff");
  MyGraph877MeV02->SetMarkerStyle(1);
  MyGraph877MeV02->SetMarkerSize(markerSize[0]*figSize);
  MyGraph877MeV02->SetMarkerColor(kRed);
  MyGraph877MeV02->SetLineColor(kRed);
  MyGraph877MeV02->Fit("fitfun01","R0EF");
  TF1* fit877MeV02 = MyGraph877MeV02->GetFunction("fitfun01");
  fit877MeV02->SetLineColor(kRed);
  fit877MeV02->SetLineStyle(10);
  fit877MeV02->SetLineWidth(3);
  fit877MeV02->SetParNames("#Delta_{p0}","#Delta_{p1}","#Delta_{p2}");

  TGraphErrors *MyGraph877MeV03 = new TGraphErrors(NUM2,thetaBarbara877MeV,SumBarbara877MeV,errx2,errx2);
  MyGraph877MeV03->SetName("MyGraph877MeV03");
  MyGraph877MeV03->Draw("goff");
  MyGraph877MeV03->SetMarkerStyle(1);
  MyGraph877MeV03->SetMarkerSize(markerSize[0]*figSize);
  MyGraph877MeV03->SetMarkerColor(kBlack);
  MyGraph877MeV03->SetLineColor(kBlack);
  MyGraph877MeV03->Fit("fitfun01","R0EF");
  TF1* fit877MeV03 = MyGraph877MeV03->GetFunction("fitfun01");
  fit877MeV03->SetLineColor(kBlack);
  fit877MeV03->SetLineStyle(2);
  fit877MeV03->SetLineWidth(3);
  fit877MeV03->SetParNames("N+#Delta_{p0}","N+#Delta_{p1}","N+#Delta_{p2}");


  Double_t N_p0   = fit01->GetParameter(0);
  Double_t N_p0_e = fit01->GetParError(0);
  Double_t N_p1   = fit01->GetParameter(1);
  Double_t N_p1_e = fit01->GetParError(1);
  Double_t N_p2   = fit01->GetParameter(2);
  Double_t N_p2_e = fit01->GetParError(2);

  Double_t Delta_p0   = fit02->GetParameter(0);
  Double_t Delta_p0_e = fit02->GetParError(0);
  Double_t Delta_p1   = fit02->GetParameter(1);
  Double_t Delta_p1_e = fit02->GetParError(1);
  Double_t Delta_p2   = fit02->GetParameter(2);
  Double_t Delta_p2_e = fit02->GetParError(2);

  Double_t Sum_p0   = fit03->GetParameter(0);
  Double_t Sum_p0_e = fit03->GetParError(0);
  Double_t Sum_p1   = fit03->GetParameter(1);
  Double_t Sum_p1_e = fit03->GetParError(1);
  Double_t Sum_p2   = fit03->GetParameter(2);
  Double_t Sum_p2_e = fit03->GetParError(2);

  Double_t N_p0_877MeV   = fit877MeV01->GetParameter(0);
  Double_t N_p0_e_877MeV = fit877MeV01->GetParError(0);
  Double_t N_p1_877MeV   = fit877MeV01->GetParameter(1);
  Double_t N_p1_e_877MeV = fit877MeV01->GetParError(1);
  Double_t N_p2_877MeV   = fit877MeV01->GetParameter(2);
  Double_t N_p2_e_877MeV = fit877MeV01->GetParError(2);

  Double_t Delta_p0_877MeV   = fit877MeV02->GetParameter(0);
  Double_t Delta_p0_e_877MeV = fit877MeV02->GetParError(0);
  Double_t Delta_p1_877MeV   = fit877MeV02->GetParameter(1);
  Double_t Delta_p1_e_877MeV = fit877MeV02->GetParError(1);
  Double_t Delta_p2_877MeV   = fit877MeV02->GetParameter(2);
  Double_t Delta_p2_e_877MeV = fit877MeV02->GetParError(2);

  Double_t Sum_p0_877MeV   = fit877MeV03->GetParameter(0);
  Double_t Sum_p0_e_877MeV = fit877MeV03->GetParError(0);
  Double_t Sum_p1_877MeV   = fit877MeV03->GetParameter(1);
  Double_t Sum_p1_e_877MeV = fit877MeV03->GetParError(1);
  Double_t Sum_p2_877MeV   = fit877MeV03->GetParameter(2);
  Double_t Sum_p2_e_877MeV = fit877MeV03->GetParError(2);


  Double_t theta1[NUM2]={theta_qweak};
  Double_t theta2[NUM2]={theta_qweak2};
  Double_t theta3[NUM2]={theta_qweak3};

  TGraphErrors *qweakFinalAsymmetry_stat = new TGraphErrors(NUM2,theta1,AnQweakFinal,errx2,dAnQweakFinal_stat);
  qweakFinalAsymmetry_stat->SetName("qweakFinalAsymmetry_stat");
  qweakFinalAsymmetry_stat->Draw("goff");
  qweakFinalAsymmetry_stat->SetMarkerStyle(21);
  qweakFinalAsymmetry_stat->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry_stat->SetMarkerColor(kBlue);
  qweakFinalAsymmetry_stat->SetLineColor(kBlue);
  qweakFinalAsymmetry_stat->SetLineWidth(2.0);

  TGraphErrors *qweakFinalAsymmetry = new TGraphErrors(NUM2,theta1,AnQweakFinal,errx2,dAnQweakFinal);
  qweakFinalAsymmetry->SetName("qweakFinalAsymmetry");
  qweakFinalAsymmetry->Draw("goff");
  qweakFinalAsymmetry->SetMarkerStyle(21);
  qweakFinalAsymmetry->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry->SetMarkerColor(kBlue);
  qweakFinalAsymmetry->SetLineColor(kBlue);
  qweakFinalAsymmetry->SetLineWidth(2.0);

  TGraphErrors *qweakFinalAsymmetry877MeV_stat = new TGraphErrors(NUM2,theta1,BnQweakFinal877MeV,errx2,dBnQweakFinal877MeV_stat);
  qweakFinalAsymmetry877MeV_stat->SetName("qweakFinalAsymmetry877MeV_stat");
  qweakFinalAsymmetry877MeV_stat->Draw("goff");
  qweakFinalAsymmetry877MeV_stat->SetMarkerStyle(20);
  qweakFinalAsymmetry877MeV_stat->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry877MeV_stat->SetMarkerColor(kBlue);
  qweakFinalAsymmetry877MeV_stat->SetLineColor(kBlue);
  qweakFinalAsymmetry877MeV_stat->SetLineWidth(2.0);

  TGraphErrors *qweakFinalAsymmetry877MeV = new TGraphErrors(NUM2,theta1,BnQweakFinal877MeV,errx2,dBnQweakFinal877MeV);
  qweakFinalAsymmetry877MeV->SetName("qweakFinalAsymmetry877MeV");
  qweakFinalAsymmetry877MeV->Draw("goff");
  qweakFinalAsymmetry877MeV->SetMarkerStyle(20);
  qweakFinalAsymmetry877MeV->SetMarkerSize(markerSize[0]*figSize);
  qweakFinalAsymmetry877MeV->SetMarkerColor(kBlue);
  qweakFinalAsymmetry877MeV->SetLineColor(kBlue);
  qweakFinalAsymmetry877MeV->SetLineWidth(2.0);


  gStyle->SetLineWidth(1.0);

  TMultiGraph * MyGraphAll = new TMultiGraph();
  MyGraphAll->Add(MyGraph01,"P");
  MyGraphAll->Add(MyGraph02,"P");
  MyGraphAll->Add(MyGraph03,"P");
  MyGraphAll->Add(MyGraph877MeV01,"P");
  MyGraphAll->Add(MyGraph877MeV02,"P");
  MyGraphAll->Add(MyGraph877MeV03,"P");
  MyGraphAll->Add(qweakFinalAsymmetry_stat,"[]");
  MyGraphAll->Add(qweakFinalAsymmetry,"P");
  MyGraphAll->Add(qweakFinalAsymmetry877MeV_stat,"[]");
  MyGraphAll->Add(qweakFinalAsymmetry877MeV,"P");
  MyGraphAll->Draw("A");
  MyGraphAll->SetTitle("");
  ////  MyGraphAll->GetYaxis()->SetTitle("Beam Normal Spin Asymmetry [ppm]");
  MyGraphAll->GetYaxis()->SetTitle("B_{n} [ppm]");
  MyGraphAll->GetYaxis()->CenterTitle();
  MyGraphAll->GetXaxis()->SetTitle("#theta_{lab} [degree]");
  MyGraphAll->GetXaxis()->CenterTitle();
  MyGraphAll->GetYaxis()->SetTitleOffset(0.72);
  MyGraphAll->GetXaxis()->SetTitleOffset(1.00);
  MyGraphAll->GetXaxis()->SetNdivisions(9,5,0);
  MyGraphAll->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisMyGraphAll= MyGraphAll->GetXaxis();
  xaxisMyGraphAll->SetLimits(5.0,13.0);
  MyGraphAll->GetYaxis()->SetRangeUser(0.0,140.0);

  fit01->Draw("same");
  fit02->Draw("same");
  fit03->Draw("same");
  fit877MeV01->Draw("same");
  fit877MeV02->Draw("same");
  fit877MeV03->Draw("same");

//   TLegend *legend01 = new TLegend(0.550,0.800,0.942,0.940,"","brNDC");
//   legend01->AddEntry(qweakFinalAsymmetry, Form("Q-weak 1.155 GeV"), "lp");
//   legend01->AddEntry(MyGraph03, Form("B.Pasquini 1.160 GeV"), "lp");
//   legend01->AddEntry(qweakFinalAsymmetry877MeV, Form("Q-weak 0.870 GeV"), "lp");
//   legend01->AddEntry(MyGraph877MeV03, Form("B.Pasquini 0.877 GeV"), "lp");

  TLegend *legend01 = new TLegend(0.550,0.760,0.942,0.900,"","brNDC");
  legend01->AddEntry(qweakFinalAsymmetry, Form("1.155 GeV"), "lp");
  legend01->AddEntry(MyGraph03, Form("1.160 GeV"), "lp");
  legend01->AddEntry(qweakFinalAsymmetry877MeV, Form("0.870 GeV"), "lp");
  legend01->AddEntry(MyGraph877MeV03, Form("0.877 GeV"), "l");
  legend01->SetNColumns(2);
  legend01->SetFillColor(0);
  legend01->SetBorderSize(0);
  legend01->SetTextSize(textSize[0]);

  TLatex* tlegend02 = new TLatex(9.80,132.0,Form("Q-weak"));
  tlegend02->SetTextSize(textSize[0]);
  tlegend02->SetTextColor(kBlack);
  TLatex* tlegend03 = new TLatex(11.60,132.0,Form("B.Pasquini"));
  tlegend03->SetTextSize(textSize[0]);
  tlegend03->SetTextColor(kBlack);

  legend01->Draw("");
  tlegend02->Draw("");
  tlegend03->Draw("");

  TLatex* tTextBarbara01 = new TLatex(12.0,NBarbara[3]*0.45,Form("N"));
  tTextBarbara01->SetTextSize(textSize[0]);
  tTextBarbara01->SetTextColor(kGreen+2);
  tTextBarbara01->Draw();

  TLatex* tTextBarbara02 = new TLatex(12.0,DeltaBarbara[3]*1.12,Form("#Delta"));
  tTextBarbara02->SetTextSize(textSize[0]);
  tTextBarbara02->SetTextColor(kRed);
  tTextBarbara02->Draw();

//   TColor *color = gROOT->GetColor(10);
//   color->SetRGB(0.1,0.2,0.3);
//   Int_t colorDarkGreen = 3;
//   TColor *color = new TColor(colorDarkGreen, 0.1,0.2,0.3);

//   TLatex* tTextBarbara03 = new TLatex(11.0,SumBarbara[3]*1.12,Form("Sum (   +   )"));
  TLatex* tTextBarbara03 = new TLatex(11.2,SumBarbara[3]*0.75,Form("Sum"));
  tTextBarbara03->SetTextSize(textSize[0]);
  tTextBarbara03->SetTextColor(kBlack);
  tTextBarbara03->Draw();

//   TLatex* tTextBarbara03_1 = new TLatex(11.75,SumBarbara[3]*1.12,Form("N"));
//   tTextBarbara03_1->SetTextSize(textSize[0]);
//   tTextBarbara03_1->SetTextColor(kGreen+2);
//   tTextBarbara03_1->Draw();

//   TLatex* tTextBarbara03_2 = new TLatex(12.16,SumBarbara[3]*1.12,Form("#Delta"));
//   tTextBarbara03_2->SetTextSize(textSize[0]);
//   tTextBarbara03_2->SetTextColor(kRed);
//   tTextBarbara03_2->Draw();

  Double_t BnBarbaraQweak   = Sum_p0 + Sum_p1/theta_qweak;
  Double_t BnBarbaraQweak_e = TMath::Abs((Sum_p1*theta_qweak_error)/(theta_qweak*theta_qweak));

  Double_t BnBarbaraQweak877MeV   = Sum_p0_877MeV + Sum_p1_877MeV/theta_qweak;
  Double_t BnBarbaraQweak877MeV_e = TMath::Abs((Sum_p1_877MeV*theta_qweak_error)/(theta_qweak*theta_qweak));

//   Double_t BnBarbaraQweak   = Sum_p0 + exp(theta_qweak*Sum_p1+Sum_p2);
  cout<<BnBarbaraQweak877MeV<<" +- "<<BnBarbaraQweak877MeV_e<<endl;

//   TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.01,0.75*BnBarbaraQweak,Form("B_{n}(from fit)_{#theta_{lab}=%1.2f^{o}} = %1.2f",theta_qweak,BnBarbaraQweak));
  TLatex* tTextAnBarbaraQweak = new TLatex(theta_qweak*1.020,0.70*BnBarbaraQweak,Form("%0.2f (from fit)",BnBarbaraQweak));
  tTextAnBarbaraQweak->SetTextSize(textSize[0]);
  tTextAnBarbaraQweak->SetTextColor(kBlack);
  tTextAnBarbaraQweak->Draw();

  TLatex* tTextFinalQweak01 = new TLatex(theta_qweak*1.020,AnQweakFinal[0]*0.96,Form("%0.2f #pm %0.2f",AnQweakFinal[0],dAnQweakFinal[0]));
//   TLatex* tTextFinalQweak01 = new TLatex(theta_qweak*1.015,AnQweakFinal[0]*0.97,Form("%1.2f #pm %1.2f",AnQweakFinal[0],dAnQweakFinal[0]));
  tTextFinalQweak01->SetTextSize(textSize[0]);
  tTextFinalQweak01->SetTextColor(kBlue);
  tTextFinalQweak01->Draw();

//   TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,BnBarbaraQweak);
  TLine *lineQweakTheta = new TLine(theta_qweak,0.0,theta_qweak,70.0);
  lineQweakTheta->SetLineColor(kGray+2);
  lineQweakTheta->SetLineStyle(3);
//   lineQweakTheta->Draw("");

  TArrow ar1(8.55,29.0,8.38,33.42,0.01,"|>");
  ar1.SetAngle(60);
  ar1.SetLineWidth(2);
  ar1.SetFillColor(kBlack);
  ar1.Draw("");

//   TF1 *funSigma = new TF1("funSigma",Form("(%f + (%f/x)) - ((%f + (%f/x))*0.683)",Sum_p0,Sum_p1,Sum_p0,Sum_p1),5.5,12.5);
  TF1 *funSigmaLow = new TF1("funSigmaLow",Form("(%f + (%f/x))*0.90",Sum_p0,Sum_p1),5.5,12.5);
  funSigmaLow->SetLineColor(kGray);
  funSigmaLow->SetLineStyle(2);
  funSigmaLow->SetLineWidth(2);
//   funSigmaLow->SetFillStyle(3002);
//   funSigmaLow->SetFillColor(kGreen+1);

  TF1 *funSigmaHigh = new TF1("funSigmaHigh",Form("(%f + (%f/x))*1.10",Sum_p0,Sum_p1),5.5,12.5);
  funSigmaHigh->SetLineColor(kGray);
  funSigmaHigh->SetLineStyle(2);
  funSigmaHigh->SetLineWidth(2);

//   funSigmaLow->Draw("same");
//   funSigmaHigh->Draw("same");

  gPad->Update();


  TString savePlot = Form("dirPlot/finalPlot/thetaDependenceOfAsymmetryAllEnergy");
  
  canvas11->Update();
  canvas11->Print(savePlot+".png");

  }











  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);
  theApp.Run();
  return(1);

}
