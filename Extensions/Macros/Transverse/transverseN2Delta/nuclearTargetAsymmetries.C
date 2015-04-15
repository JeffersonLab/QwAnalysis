using namespace std;
#include "NurClass.h"

int main(Int_t argc,Char_t* argv[]){

  Bool_t ASYM = 1;

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
  Double_t yScale[2];
  Double_t waterMark[2] = {2.5,-1.0};
  TString chooseAsym,yTitle,imageTitle,canvasTitle;

  Double_t x_lo_stat,y_lo_stat,x_hi_stat,y_hi_stat;
  x_lo_stat=0.81;y_lo_stat=0.64;x_hi_stat=0.99;y_hi_stat=0.95;


  //*************************************************//
 //*************************************************//

  Int_t opt = 2;
  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form(" \nSlug averages of Main Detector Asymmetries \n")<<std::endl;
  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form("Enter plot type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. QTor current ")<<std::endl;
  std::cout<<Form("2. %sW (deafult)%s",blue,normal)<<std::endl;
//   std::cout<<Form("2. %sW (deafult)%s",blue,normal)<<std::endl;
  //   std::cin>>opt;
  std::string input_opt;
  std::getline( std::cin, input_opt );
  if ( !input_opt.empty() ) {
    std::istringstream stream( input_opt );
    stream >> opt;
  }

  Int_t opt_asym = 1;
//   std::cout<<Form("###############################################")<<std::endl;
//   std::cout<<Form(" \nSlug averages of Main Detector Asymmetries \n")<<std::endl;
//   std::cout<<Form("###############################################")<<std::endl;
//   std::cout<<Form("Enter plot type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
//   std::cout<<Form("1. %sBNSSA (deafult)%s",blue,normal)<<std::endl;
//   std::cout<<Form("2. Regressed Asym. ")<<std::endl;
//   //   std::cin>>opt_asym;
  std::string input_opt_asym;
//   std::getline( std::cin, input_opt_asym );
  if ( !input_opt_asym.empty() ) {
    std::istringstream stream( input_opt_asym );
    stream >> opt_asym;
  }


  if(opt_asym == 1){
    chooseAsym = "Final";
    yScale[0]=0.0;yScale[1]=120.0;
    yTitle = "B_{n} [ppm]";
    imageTitle = "Bn";
    canvasTitle = "Beam Normal Single Spin Asymmetry at E_{beam} = 1.155 GeV";
  }
  else if(opt_asym == 2){
    chooseAsym = "Reg";
    yScale[0]=-2.5;yScale[1]=13.0;
    yTitle = "#epsilon_{reg} [ppm]";
    imageTitle = "RegAsym";
    canvasTitle = "Beam Normal Single Spin Asymmetry (NO Correction for Polarization and Backgrounds) at E_{beam} = 1.155 GeV";
  }
  else{
    std::cout<<Form("Unknown target type!")<<std::endl;
    exit(1);
  }


  TApplication theApp("App",&argc,argv);

  //*************************************************//


  const Int_t NUM1 = 1;
  int counter1=0;
  ifstream textBn6000ALH2,textBn6700ALH2,textBn7300ALH2;
  ifstream textBn6700AAl,textBn7300AAl,textBn6700AC;
  ifstream textTheory;
  Double_t AnQweakFinal[NUM1],dAnQweakFinal_stat[NUM1],dAnQweakFinal_sys[NUM1],dAnQweakFinal[NUM1];
  Double_t Bn6000ALH2[NUM1],dBn6000ALH2_stat[NUM1],dBn6000ALH2_sys[NUM1],dBn6000ALH2[NUM1];
  Double_t Bn6700ALH2[NUM1],dBn6700ALH2_stat[NUM1],dBn6700ALH2_sys[NUM1],dBn6700ALH2[NUM1];
  Double_t Bn7300ALH2[NUM1],dBn7300ALH2_stat[NUM1],dBn7300ALH2_sys[NUM1],dBn7300ALH2[NUM1];
  Double_t Bn6700AAl[NUM1],dBn6700AAl_stat[NUM1],dBn6700AAl_sys[NUM1],dBn6700AAl[NUM1];
  Double_t Bn7300AAl[NUM1],dBn7300AAl_stat[NUM1],dBn7300AAl_sys[NUM1],dBn7300AAl[NUM1];
  Double_t Bn6700AC[NUM1],dBn6700AC_stat[NUM1],dBn6700AC_sys[NUM1],dBn6700AC[NUM1];
  Double_t BnTheory[NUM1],dBnTheory[NUM1];

  Double_t RegAsym6000ALH2[NUM1],dRegAsym6000ALH2_stat[NUM1],dRegAsym6000ALH2_sys[NUM1],dRegAsym6000ALH2[NUM1];
  Double_t RegAsym6700ALH2[NUM1],dRegAsym6700ALH2_stat[NUM1],dRegAsym6700ALH2_sys[NUM1],dRegAsym6700ALH2[NUM1];
  Double_t RegAsym7300ALH2[NUM1],dRegAsym7300ALH2_stat[NUM1],dRegAsym7300ALH2_sys[NUM1],dRegAsym7300ALH2[NUM1];
  Double_t RegAsym6700AAl[NUM1],dRegAsym6700AAl_stat[NUM1],dRegAsym6700AAl_sys[NUM1],dRegAsym6700AAl[NUM1];
  Double_t RegAsym7300AAl[NUM1],dRegAsym7300AAl_stat[NUM1],dRegAsym7300AAl_sys[NUM1],dRegAsym7300AAl[NUM1];
  Double_t RegAsym6700AC[NUM1],dRegAsym6700AC_stat[NUM1],dRegAsym6700AC_sys[NUM1],dRegAsym6700AC[NUM1];

  textBn6000ALH2.open(Form("dirPlot/resultText/n-to-delta_6000_HYDROGEN-CELL_MD_%s_Asymmetry_run2_pass5.txt",chooseAsym.Data()));
  printf(Form("dirPlot/resultText/n-to-delta_6000_HYDROGEN-CELL_MD_%s_Asymmetry_run2_pass5.txt\n",chooseAsym.Data()));

  textBn6700ALH2.open(Form("dirPlot/resultText/n-to-delta_6700_HYDROGEN-CELL_MD_%s_Asymmetry_run2_pass5.txt",chooseAsym.Data()));
  printf(Form("dirPlot/resultText/n-to-delta_6700_HYDROGEN-CELL_MD_%s_Asymmetry_run2_pass5.txt\n",chooseAsym.Data()));

  textBn7300ALH2.open(Form("dirPlot/resultText/n-to-delta_7300_HYDROGEN-CELL_MD_%s_Asymmetry_run2_pass5.txt",chooseAsym.Data()));
  printf(Form("dirPlot/resultText/n-to-delta_7300_HYDROGEN-CELL_MD_%s_Asymmetry_run2_pass5.txt\n",chooseAsym.Data()));

  textBn6700AAl.open(Form("dirPlot/resultText/n-to-delta_6700_DS-4-Aluminum_MD_%s_Asymmetry_run2_pass5.txt",chooseAsym.Data()));
  printf(Form("dirPlot/resultText/n-to-delta_6700_DS-4-Aluminum_MD_%s_Asymmetry_run2_pass5.txt\n",chooseAsym.Data()));

  textBn7300AAl.open(Form("dirPlot/resultText/n-to-delta_7300_DS-4-Aluminum_MD_%s_Asymmetry_run2_pass5.txt",chooseAsym.Data()));
  printf(Form("dirPlot/resultText/n-to-delta_7300_DS-4-Aluminum_MD_%s_Asymmetry_run2_pass5.txt\n",chooseAsym.Data()));

  textBn6700AC.open(Form("dirPlot/resultText/n-to-delta_6700_DS-1.6-C_MD_%s_Asymmetry_run2_pass5.txt",chooseAsym.Data()));
  printf(Form("dirPlot/resultText/n-to-delta_6700_DS-1.6-C_MD_%s_Asymmetry_run2_pass5.txt\n",chooseAsym.Data()));

  textTheory.open(Form("dirPlot/resultText/fitResultBarbara.txt"));
  printf(Form("dirPlot/resultText/fitResultBarbara.txt\n"));

  if ( textBn6000ALH2.is_open() && textBn6700ALH2.is_open() && textBn7300ALH2.is_open() && textBn6700AAl.is_open() && textBn7300AAl.is_open() && textBn6700AC.is_open() && textTheory.is_open()) {
   
    while(!textBn6000ALH2.eof() && !textBn6700ALH2.eof() && !textBn7300ALH2.eof() && !textBn6700AAl.eof() && !textBn7300AAl.eof() && !textBn6700AC.eof() ) {
      textBn6000ALH2 >> Bn6000ALH2[counter1] >> dBn6000ALH2_stat[counter1] >> dBn6000ALH2_sys[counter1] >> dBn6000ALH2[counter1];
      textBn6700ALH2 >> Bn6700ALH2[counter1] >> dBn6700ALH2_stat[counter1] >> dBn6700ALH2_sys[counter1] >> dBn6700ALH2[counter1];
      textBn7300ALH2 >> Bn7300ALH2[counter1] >> dBn7300ALH2_stat[counter1] >> dBn7300ALH2_sys[counter1] >> dBn7300ALH2[counter1];
      textBn6700AAl  >> Bn6700AAl[counter1]  >> dBn6700AAl_stat[counter1]  >> dBn6700AAl_sys[counter1]  >> dBn6700AAl[counter1];
      textBn7300AAl  >> Bn7300AAl[counter1]  >> dBn7300AAl_stat[counter1]  >> dBn7300AAl_sys[counter1]  >> dBn7300AAl[counter1];
      textBn6700AC   >> Bn6700AC[counter1]   >> dBn6700AC_stat[counter1]   >> dBn6700AC_sys[counter1]   >> dBn6700AC[counter1];

      textTheory     >> BnTheory[counter1]   >> dBnTheory[counter1];

      if ( !textBn6000ALH2.good() && !textBn6700ALH2.good() && !textBn7300ALH2.good() && !textBn6700AAl.good() && !textBn7300AAl.good() && !textBn6700AC.good() ) break;

      cout<<blue<<"LH2 6000 A \t"<<Bn6000ALH2[counter1]<<"+-"<<dBn6000ALH2_stat[counter1]<<"(stat)\t+-"<<dBn6000ALH2_sys[counter1]<<"(sys)\t"<<dBn6000ALH2[counter1]<<"(total)"<<normal<<endl;
      cout<<blue<<"LH2 6700 A \t"<<Bn6700ALH2[counter1]<<"+-"<<dBn6700ALH2_stat[counter1]<<"(stat)\t+-"<<dBn6700ALH2_sys[counter1]<<"(sys)\t"<<dBn6700ALH2[counter1]<<"(total)"<<normal<<endl;
      cout<<blue<<"LH2 7300 A \t"<<Bn7300ALH2[counter1]<<"+-"<<dBn7300ALH2_stat[counter1]<<"(stat)\t+-"<<dBn7300ALH2_sys[counter1]<<"(sys)\t"<<dBn7300ALH2[counter1]<<"(total)"<<normal<<endl;
      cout<<blue<<"Al 6700 A \t"<<Bn6700AAl[counter1]<<"+-"<<dBn6700AAl_stat[counter1]<<"(stat)\t+-"<<dBn6700AAl_sys[counter1]<<"(sys)\t"<<dBn6700AAl[counter1]<<"(total)"<<normal<<endl;
      cout<<blue<<"Al 7300 A \t"<<Bn7300AAl[counter1]<<"+-"<<dBn7300AAl_stat[counter1]<<"(stat)\t+-"<<dBn7300AAl_sys[counter1]<<"(sys)\t"<<dBn7300AAl[counter1]<<"(total)"<<normal<<endl;
      cout<<blue<<"C 6700 A \t"<<Bn6700AC[counter1]<<"+-"<<dBn6700AC_stat[counter1]<<"(stat)\t+-"<<dBn6700AC_sys[counter1]<<"(sys)\t"<<dBn6700AC[counter1]<<"(total)"<<normal<<endl;
      cout<<blue<<"Theory \t"<<BnTheory[counter1]<<"+-"<<dBnTheory[counter1]<<normal<<endl;
 
      counter1++;
    }
  }
  else {
    printf("%sNo textfile exist for the configuration. Exiting program!\n%s",red,normal);
    return -1;
  }
  textBn6000ALH2.close();
  textBn6700ALH2.close();
  textBn7300ALH2.close();
  textBn6700AAl.close();
  textBn7300AAl.close();
  textBn6700AC.close();
  textTheory.close();


  //*************************************************//
  //*************************************************//

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

  if(ASYM){

  TCanvas * canvas21 = new TCanvas("canvas21", "canvas21",0,0,canvasSize2[0],canvasSize[1]);
  canvas21->Draw();
  canvas21->SetBorderSize(0);
  canvas21->cd();
  TPad*pad21 = new TPad("pad21","pad21",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad22 = new TPad("pad22","pad22",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad21->SetFillColor(kWhite);
  pad21->Draw();
  pad22->Draw();
  pad21->cd();
  TLatex * t21 = new TLatex(0.06,0.3,Form("%s",canvasTitle.Data()));
  t21->SetTextSize(0.6);
  t21->Draw();
  pad22->cd();


  Double_t qtorCurrents6000ALH2,qtorCurrents6700ALH2,qtorCurrents7300ALH2,offsetQTor,theoryOffset;
  Double_t qtorCurrents6700AAl,qtorCurrents7300AAl,qtorCurrents6700AC,qtorCurrentsTheory;
  Double_t xAxisScale[2],textOffset[3];
  TString textToken,xAxisTitle;

  qtorCurrents6000ALH2  = 6000;
  qtorCurrents6700ALH2  = 6700;
  qtorCurrents7300ALH2  = 7300;
  offsetQTor = 25;
  theoryOffset = 40;
  xAxisScale[0] = 0.875;  xAxisScale[1] = 1.140;
  textOffset[0] = 25; textOffset[1] = 280; textOffset[2] = 25;
  textToken = "VsQTor";
  xAxisTitle = "QTor Current [A]";
  
  if(opt==2){
    qtorCurrents6000ALH2  = 1.276;
    qtorCurrents6700ALH2  = 1.219;
    qtorCurrents7300ALH2  = 1.168;
    offsetQTor = 0.0015;
    theoryOffset = 0.0035;
    textOffset[0] = 0.0017; textOffset[1] = 0.0265; textOffset[2] = 0.0017;
//     Double_t xAxisScale[2] = {0.94,1.075};
    xAxisScale[0] = 0.930; xAxisScale[1] = 1.073;
  textToken = "VsW";
  xAxisTitle = "W [GeV]";
  }
  
  qtorCurrents6700AAl  = qtorCurrents6700ALH2-offsetQTor;
  qtorCurrents7300AAl  = qtorCurrents7300ALH2-offsetQTor;
  qtorCurrents6700AC   = qtorCurrents6700ALH2+offsetQTor;
  qtorCurrentsTheory   = qtorCurrents6700ALH2-offsetQTor;

  const Int_t n1 = 1;
  Double_t errX[n1];
  Double_t qtor6000ALH2[n1]={qtorCurrents6000ALH2};
  Double_t qtor6700ALH2[n1]={qtorCurrents6700ALH2};
  Double_t qtor7300ALH2[n1]={qtorCurrents7300ALH2};
  Double_t qtor6700AAl[n1]={qtorCurrents6700AAl};
  Double_t qtor7300AAl[n1]={qtorCurrents7300AAl};
  Double_t qtor6700AC[n1]={qtorCurrents6700AC};
  Double_t qtorTheory[n1]={qtorCurrentsTheory};

  for(Int_t j =0;j<NUM1;j++){
    errX[j] = 0.0;
  }

  gStyle->SetLineWidth(2.0);

  TGraphErrors *grBn6000ALH2_stat = new TGraphErrors(NUM1,qtor6000ALH2,Bn6000ALH2,errX,dBn6000ALH2_stat);
  grBn6000ALH2_stat->SetName("grBn6000ALH2_stat");
  grBn6000ALH2_stat->Draw("0");
  grBn6000ALH2_stat->SetMarkerStyle(21);
  grBn6000ALH2_stat->SetMarkerSize(markerSize[0]*figSize);
  grBn6000ALH2_stat->SetMarkerColor(kBlue);
  grBn6000ALH2_stat->SetLineColor(kBlue);

  TGraphErrors *grBn6000ALH2 = new TGraphErrors(NUM1,qtor6000ALH2,Bn6000ALH2,errX,dBn6000ALH2);
  grBn6000ALH2->SetName("grBn6000ALH2");
  grBn6000ALH2->Draw("0");
  grBn6000ALH2->SetMarkerStyle(21);
  grBn6000ALH2->SetMarkerSize(markerSize[0]*figSize);
  grBn6000ALH2->SetMarkerColor(kBlue);
  grBn6000ALH2->SetLineColor(kBlue);

  TGraphErrors *grBn6700ALH2_stat = new TGraphErrors(NUM1,qtor6700ALH2,Bn6700ALH2,errX,dBn6700ALH2_stat);
  grBn6700ALH2_stat->SetName("grBn6700ALH2_stat");
  grBn6700ALH2_stat->Draw("0");
  grBn6700ALH2_stat->SetMarkerStyle(21);
  grBn6700ALH2_stat->SetMarkerSize(markerSize[0]*figSize);
  grBn6700ALH2_stat->SetMarkerColor(kBlue);
  grBn6700ALH2_stat->SetLineColor(kBlue);

  TGraphErrors *grBn6700ALH2 = new TGraphErrors(NUM1,qtor6700ALH2,Bn6700ALH2,errX,dBn6700ALH2);
  grBn6700ALH2->SetName("grBn6700ALH2");
  grBn6700ALH2->Draw("0");
  grBn6700ALH2->SetMarkerStyle(21);
  grBn6700ALH2->SetMarkerSize(markerSize[0]*figSize);
  grBn6700ALH2->SetMarkerColor(kBlue);
  grBn6700ALH2->SetLineColor(kBlue);

  TGraphErrors *grBn7300ALH2_stat = new TGraphErrors(NUM1,qtor7300ALH2,Bn7300ALH2,errX,dBn7300ALH2_stat);
  grBn7300ALH2_stat->SetName("grBn7300ALH2_stat");
  grBn7300ALH2_stat->Draw("0");
  grBn7300ALH2_stat->SetMarkerStyle(21);
  grBn7300ALH2_stat->SetMarkerSize(markerSize[0]*figSize);
  grBn7300ALH2_stat->SetMarkerColor(kBlue);
  grBn7300ALH2_stat->SetLineColor(kBlue);

  TGraphErrors *grBn7300ALH2 = new TGraphErrors(NUM1,qtor7300ALH2,Bn7300ALH2,errX,dBn7300ALH2);
  grBn7300ALH2->SetName("grBn7300ALH2");
  grBn7300ALH2->Draw("0");
  grBn7300ALH2->SetMarkerStyle(21);
  grBn7300ALH2->SetMarkerSize(markerSize[0]*figSize);
  grBn7300ALH2->SetMarkerColor(kBlue);
  grBn7300ALH2->SetLineColor(kBlue);

  TGraphErrors *grBn6700AAl_stat = new TGraphErrors(NUM1,qtor6700AAl,Bn6700AAl,errX,dBn6700AAl_stat);
  grBn6700AAl_stat->SetName("grBn6700AAl_stat");
  grBn6700AAl_stat->Draw("0");
  grBn6700AAl_stat->SetMarkerStyle(22);
  grBn6700AAl_stat->SetMarkerSize(markerSize[0]*figSize);
  grBn6700AAl_stat->SetMarkerColor(kMagenta);
  grBn6700AAl_stat->SetLineColor(kMagenta);

  TGraphErrors *grBn6700AAl = new TGraphErrors(NUM1,qtor6700AAl,Bn6700AAl,errX,dBn6700AAl);
  grBn6700AAl->SetName("grBn6700AAl");
  grBn6700AAl->Draw("0");
  grBn6700AAl->SetMarkerStyle(22);
  grBn6700AAl->SetMarkerSize(markerSize[0]*figSize);
  grBn6700AAl->SetMarkerColor(kMagenta);
  grBn6700AAl->SetLineColor(kMagenta);

  TGraphErrors *grBn7300AAl_stat = new TGraphErrors(NUM1,qtor7300AAl,Bn7300AAl,errX,dBn7300AAl_stat);
  grBn7300AAl_stat->SetName("grBn7300AAl_stat");
  grBn7300AAl_stat->Draw("0");
  grBn7300AAl_stat->SetMarkerStyle(22);
  grBn7300AAl_stat->SetMarkerSize(markerSize[0]*figSize);
  grBn7300AAl_stat->SetMarkerColor(kMagenta);
  grBn7300AAl_stat->SetLineColor(kMagenta);

  TGraphErrors *grBn7300AAl = new TGraphErrors(NUM1,qtor7300AAl,Bn7300AAl,errX,dBn7300AAl);
  grBn7300AAl->SetName("grBn7300AAl");
  grBn7300AAl->Draw("0");
  grBn7300AAl->SetMarkerStyle(22);
  grBn7300AAl->SetMarkerSize(markerSize[0]*figSize);
  grBn7300AAl->SetMarkerColor(kMagenta);
  grBn7300AAl->SetLineColor(kMagenta);

  TGraphErrors *grBn6700AC_stat = new TGraphErrors(NUM1,qtor6700AC,Bn6700AC,errX,dBn6700AC_stat);
  grBn6700AC_stat->SetName("grBn6700AC_stat");
  grBn6700AC_stat->Draw("0");
  grBn6700AC_stat->SetMarkerStyle(20);
  grBn6700AC_stat->SetMarkerSize(markerSize[0]*figSize);
  grBn6700AC_stat->SetMarkerColor(kOrange+2);
  grBn6700AC_stat->SetLineColor(kOrange+2);

  TGraphErrors *grBn6700AC = new TGraphErrors(NUM1,qtor6700AC,Bn6700AC,errX,dBn6700AC);
  grBn6700AC->SetName("grBn6700AC");
  grBn6700AC->Draw("0");
  grBn6700AC->SetMarkerStyle(20);
  grBn6700AC->SetMarkerSize(markerSize[0]*figSize);
  grBn6700AC->SetMarkerColor(kOrange+2);
  grBn6700AC->SetLineColor(kOrange+2);

  TGraphErrors *grBnTheory = new TGraphErrors(NUM1,qtorTheory,BnTheory,errX,dBnTheory);
  grBnTheory->SetName("grBnTheory");
  grBnTheory->Draw("0");
  grBnTheory->SetMarkerStyle(24);
  grBnTheory->SetMarkerSize(markerSize[0]*figSize);
  grBnTheory->SetMarkerColor(kBlack);
  grBnTheory->SetLineColor(kBlack);

  gStyle->SetLineWidth(1.0);


  TMultiGraph *grBnAll = new TMultiGraph();
  grBnAll->Add(grBn6000ALH2_stat,"[]");
  grBnAll->Add(grBn6000ALH2,"P");
  grBnAll->Add(grBn6700ALH2_stat,"[]");
  grBnAll->Add(grBn6700ALH2,"P");
  grBnAll->Add(grBn7300ALH2_stat,"[]");
  grBnAll->Add(grBn7300ALH2,"P");
//   grBnAll->Add(grBn6700AAl_stat,"[]");
//   grBnAll->Add(grBn6700AAl,"P");
//   grBnAll->Add(grBn7300AAl_stat,"[]");
//   grBnAll->Add(grBn7300AAl,"P");
//   grBnAll->Add(grBn6700AC_stat,"[]");
//   grBnAll->Add(grBn6700AC,"P");
//   grBnAll->Add(grBnTheory,"P");
  grBnAll->Draw("A");
  grBnAll->SetTitle("");
  grBnAll->GetYaxis()->SetTitle(Form("%s",yTitle.Data()));
  grBnAll->GetYaxis()->CenterTitle();
  grBnAll->GetXaxis()->SetTitle(Form("%s",xAxisTitle.Data()));
  grBnAll->GetXaxis()->CenterTitle();
  grBnAll->GetYaxis()->SetTitleOffset(0.72);
  grBnAll->GetXaxis()->SetTitleOffset(1.00);
  grBnAll->GetXaxis()->SetNdivisions(9,5,0);
  grBnAll->GetYaxis()->SetNdivisions(8,5,0);
  TAxis *xaxisgrBnAll= grBnAll->GetXaxis();
  xaxisgrBnAll->SetLimits(qtorCurrents6700ALH2*xAxisScale[0],qtorCurrents6700ALH2*xAxisScale[1]);
  grBnAll->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);


//   TLegend *legBn = new TLegend(x_lo_stat-0.42,0.75,x_hi_stat-0.29,0.94,"","brNDC");
//   TLegend *legBn = new TLegend(0.730,0.800,0.942,0.940,"","brNDC");
  TLegend *legBn = new TLegend(0.120,0.800,0.323,0.940,"","brNDC");
//   legBn->AddEntry(grBn6700ALH2, Form("LH_{2}"),"lp");
//   legBn->AddEntry(grBn6700AAl, Form("Al"),"lp");
//   legBn->AddEntry(grBn6700AC, Form("^{12}C"),"lp");
  legBn->AddEntry(grBn6700ALH2, Form("Q-weak"),"lp");
  legBn->AddEntry(grBnTheory, Form("B.Pasquini"),"l");
  legBn->SetFillColor(0);
  legBn->SetBorderSize(2);
  legBn->SetTextSize(textSize[0]);
  legBn->Draw("");


  TLatex* textBn6000ALH2 = new TLatex(qtor6000ALH2[0]+textOffset[0],Bn6000ALH2[0]*0.95,Form("%1.2f #pm{}^{%0.2f}_{%0.2f}",Bn6000ALH2[0],dBn6000ALH2_stat[0],dBn6000ALH2_sys[0]));
  textBn6000ALH2->SetTextSize(textSize[0]);
  textBn6000ALH2->SetTextColor(kBlue);
  TLatex* textBn6700ALH2 = new TLatex(qtor6700ALH2[0]+textOffset[0]*1.4,Bn6700ALH2[0]*0.95,Form("%1.2f #pm{}^{%0.2f(stat)}_{%0.2f(sys)}",Bn6700ALH2[0],dBn6700ALH2_stat[0],dBn6700ALH2_sys[0]));
  textBn6700ALH2->SetTextSize(textSize[0]);
  textBn6700ALH2->SetTextColor(kBlue);
  TLatex* textBn7300ALH2 = new TLatex(qtor7300ALH2[0]+textOffset[0],Bn7300ALH2[0]*0.95,Form("%1.2f #pm{}^{%0.2f}_{%0.2f}",Bn7300ALH2[0],dBn7300ALH2_stat[0],dBn7300ALH2_sys[0]));
  textBn7300ALH2->SetTextSize(textSize[0]);
  textBn7300ALH2->SetTextColor(kBlue);

  TLatex* textBn6700AAl = new TLatex(qtor6700AAl[0]-textOffset[1],Bn6700AAl[0]*0.95,Form("%1.2f #pm{}^{%0.2f}_{%0.2f}",Bn6700AAl[0],dBn6700AAl_stat[0],dBn6700AAl_sys[0]));
  textBn6700AAl->SetTextSize(textSize[0]);
  textBn6700AAl->SetTextColor(kMagenta);
  TLatex* textBn7300AAl = new TLatex(qtor7300AAl[0]-textOffset[1],Bn7300AAl[0]*0.95,Form("%1.2f #pm{}^{%0.2f}_{%0.2f}",Bn7300AAl[0],dBn7300AAl_stat[0],dBn7300AAl_sys[0]));
  textBn7300AAl->SetTextSize(textSize[0]);
  textBn7300AAl->SetTextColor(kMagenta);

  TLatex* textBn6700AC = new TLatex(qtor6700AC[0]+textOffset[2],Bn6700AC[0]*0.95,Form("%1.2f #pm{}^{%0.2f}_{%0.2f}",Bn6700AC[0],dBn6700AC_stat[0],dBn6700AC_sys[0]));
  textBn6700AC->SetTextSize(textSize[0]);
  textBn6700AC->SetTextColor(kOrange+2);

  TLine *lineTheory = new TLine(qtorCurrents6700ALH2-theoryOffset,BnTheory[0],qtorCurrents6700ALH2+theoryOffset,BnTheory[0]);
  lineTheory->SetLineColor(kBlack);
  lineTheory->SetLineStyle(1);
  lineTheory->SetLineWidth(3);
  lineTheory->Draw("");


  textBn6000ALH2->Draw();  textBn6700ALH2->Draw();  textBn7300ALH2->Draw();
//   textBn6700AAl->Draw();  textBn7300AAl->Draw();
//   textBn6700AC->Draw();

  TString savePlot21 = Form("dirPlot/finalPlot/nuclearTarge%sN2Delta%s",imageTitle.Data(),textToken.Data());  
  canvas21->Update();
  canvas21->Print(savePlot21+".png");

  }










  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);
  theApp.Run();
  return(1);

}
