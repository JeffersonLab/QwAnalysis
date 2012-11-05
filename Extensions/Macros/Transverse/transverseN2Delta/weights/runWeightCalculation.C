/*
###################################################
  A macro to calculate weights for Main Detectors.
  --Nuruzzaman
###################################################

To get the weights of MD PMTs for a run(e.g; 16130).

Open a terminal and do:
root -l 'runWeightCalculation.C(16130)'

###################################################
*/

void runWeightCalculation(Int_t run_number){


  Bool_t PMTYIELD=1; Bool_t PMTYIELDVARIATION=1; Bool_t ALLBARSWEIGHT=1; Bool_t BARSUMWEIGHT=1; Bool_t PMTWEIGHT=1; Bool_t PMTWEIGHTDIFF=1;

  TString rootfileDir = "$QW_ROOTFILES";
  TString textfileDir = "$QWANALYSIS/Extensions/Macros/Parity/MainDet";

  const Int_t NUM = 10;

  Int_t counter = 0;
  ifstream weightsMD;
  Double_t zero[NUM];
  Double_t pmtNeg1Wt[NUM],pmtNeg2Wt[NUM],pmtNeg3Wt[NUM],pmtNeg4Wt[NUM],pmtNeg5Wt[NUM],pmtNeg6Wt[NUM],pmtNeg7Wt[NUM],pmtNeg8Wt[NUM];
  Double_t pmtPos1Wt[NUM],pmtPos2Wt[NUM],pmtPos3Wt[NUM],pmtPos4Wt[NUM],pmtPos5Wt[NUM],pmtPos6Wt[NUM],pmtPos7Wt[NUM],pmtPos8Wt[NUM];

  string mystr;

//   cout << "Enter file name to read for weights (do not insert extra space)\n";
//   getline (cin, mystr);
//   mystr = "weightAliasCalc_14017-.txt";

  if(run_number>9418 && run_number<9594)   {mystr = "weightAliasCalc_9418-.txt";}
  if(run_number>9594 && run_number<9703)   {mystr = "weightAliasCalc_9594-.txt";}
  if(run_number>9703 && run_number<9810)   {mystr = "weightAliasCalc_9703-.txt";}
  if(run_number>9810 && run_number<10213)  {mystr = "weightAliasCalc_9810-.txt";}
  if(run_number>10213 && run_number<11647) {mystr = "weightAliasCalc_10213-.txt";}
  if(run_number>11647 && run_number<12033) {mystr = "weightAliasCalc_10213-.txt";}
  if(run_number>12033 && run_number<12087) {mystr = "weightAliasCalc_12033-13336.txt";}
  if(run_number>12087 && run_number<12097) {mystr = "weightAliasCalc_12087-12096.txt";}
  if(run_number>12097 && run_number<12163) {mystr = "weightAliasCalc_12033-13336.txt";}

  if(run_number>12163 && run_number<13336) {mystr = "weightAliasCalc_12033-13336.txt";}
  if(run_number>13336 && run_number<14017) {mystr = "weightAliasCalc_13337-.txt";}
  if(run_number>14017 && run_number<14889) {mystr = "weightAliasCalc_14017-.txt";}
  if(run_number>14889 && run_number<15592) {mystr = "weightAliasCalc_14889-15592.txt";}
  if(run_number>15592 && run_number<19500) {mystr = "weightAliasCalc_14017-.txt";}
  else{printf("No such proper run range. Exiting program!\n");}


//   weightsMD.open(Form("%s/weightAliasCalc_14017-.txt",textfileDir.Data()));
//   weightsMD.open("weightAliasCalc_14017-.txt");
  weightsMD.open(Form("%s",mystr));
  cout << "File for weights is " << mystr << ".\n";

  if (weightsMD.is_open()) {

    while(!weightsMD.eof()) {
//       zero=0;
      weightsMD >> pmtNeg1Wt[counter];
      weightsMD >> pmtNeg2Wt[counter];
      weightsMD >> pmtNeg3Wt[counter];
      weightsMD >> pmtNeg4Wt[counter];
      weightsMD >> pmtNeg5Wt[counter];
      weightsMD >> pmtNeg6Wt[counter];
      weightsMD >> pmtNeg7Wt[counter];
      weightsMD >> pmtNeg8Wt[counter];
      weightsMD >> pmtPos1Wt[counter];
      weightsMD >> pmtPos2Wt[counter];
      weightsMD >> pmtPos3Wt[counter];
      weightsMD >> pmtPos4Wt[counter];
      weightsMD >> pmtPos5Wt[counter];
      weightsMD >> pmtPos6Wt[counter];
      weightsMD >> pmtPos7Wt[counter];
      weightsMD >> pmtPos8Wt[counter];
   
      if (!weightsMD.good()) break;

      printf("MD1Neg = %4.4f\tMD1Pos = %4.4f\n",pmtNeg1Wt[counter],pmtPos1Wt[counter]);
      printf("MD2Neg = %4.4f\tMD2Pos = %4.4f\n",pmtNeg2Wt[counter],pmtPos2Wt[counter]);
      printf("MD3Neg = %4.4f\tMD3Pos = %4.4f\n",pmtNeg3Wt[counter],pmtPos3Wt[counter]);
      printf("MD4Neg = %4.4f\tMD4Pos = %4.4f\n",pmtNeg4Wt[counter],pmtPos4Wt[counter]);
      printf("MD5Neg = %4.4f\tMD5Pos = %4.4f\n",pmtNeg5Wt[counter],pmtPos5Wt[counter]);
      printf("MD6Neg = %4.4f\tMD6Pos = %4.4f\n",pmtNeg6Wt[counter],pmtPos6Wt[counter]);
      printf("MD7Neg = %4.4f\tMD7Pos = %4.4f\n",pmtNeg7Wt[counter],pmtPos7Wt[counter]);
      printf("MD8Neg = %4.4f\tMD8Pos = %4.4f\n",pmtNeg8Wt[counter],pmtPos8Wt[counter]);

      counter++;
    }
  }

  else {
    printf("No such textfile exist. Exiting program!\n");
    return -1;
  }
  weightsMD.close();

//   Double_t barsum1Wt[0] = (pmtNeg1Wt[0]+pmtPos1Wt[0])/2;
//   Double_t barsum2Wt[0] = (pmtNeg2Wt[0]+pmtPos2Wt[0])/2;
//   Double_t barsum3Wt[0] = (pmtNeg3Wt[0]+pmtPos3Wt[0])/2;
//   Double_t barsum4Wt[0] = (pmtNeg4Wt[0]+pmtPos4Wt[0])/2;
//   Double_t barsum5Wt[0] = (pmtNeg5Wt[0]+pmtPos5Wt[0])/2;
//   Double_t barsum6Wt[0] = (pmtNeg6Wt[0]+pmtPos6Wt[0])/2;
//   Double_t barsum7Wt[0] = (pmtNeg7Wt[0]+pmtPos7Wt[0])/2;
//   Double_t barsum8Wt[0] = (pmtNeg8Wt[0]+pmtPos8Wt[0])/2;

//   Double_t allbarsumWt[0] = (pmtNeg1Wt[0]+pmtPos1Wt[0]+pmtNeg2Wt[0]+pmtPos2Wt[0]+pmtNeg3Wt[0]+pmtPos3Wt[0]+pmtNeg4Wt[0]+pmtPos4Wt[0]+pmtNeg5Wt[0]+pmtPos5Wt[0]+pmtNeg6Wt[0]+pmtPos6Wt[0]+pmtNeg7Wt[0]+pmtPos7Wt[0]+pmtNeg8Wt[0]+pmtPos8Wt[0])/16;

//   printf("%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f\n",barsum1Wt[0],barsum2Wt[0],barsum3Wt[0],barsum4Wt[0],barsum5Wt[0],barsum6Wt[0],barsum7Wt[0],barsum8Wt[0]);

  Double_t pmtWeightPos[8],pmtWeightNeg[8];
  Double_t barsumWeight[8];
  Double_t allbarsWeight;

    pmtWeightPos[0] = pmtPos1Wt[0];
    pmtWeightPos[1] = pmtPos2Wt[0];
    pmtWeightPos[2] = pmtPos3Wt[0];
    pmtWeightPos[3] = pmtPos4Wt[0];
    pmtWeightPos[4] = pmtPos5Wt[0];
    pmtWeightPos[5] = pmtPos6Wt[0];
    pmtWeightPos[6] = pmtPos7Wt[0];
    pmtWeightPos[7] = pmtPos8Wt[0];

    pmtWeightNeg[0] = pmtNeg1Wt[0];
    pmtWeightNeg[1] = pmtNeg2Wt[0];
    pmtWeightNeg[2] = pmtNeg3Wt[0];
    pmtWeightNeg[3] = pmtNeg4Wt[0];
    pmtWeightNeg[4] = pmtNeg5Wt[0];
    pmtWeightNeg[5] = pmtNeg6Wt[0];
    pmtWeightNeg[6] = pmtNeg7Wt[0];
    pmtWeightNeg[7] = pmtNeg8Wt[0];


    allbarsWeight = (pmtWeightPos[0]+pmtWeightPos[1]+pmtWeightPos[2]+pmtWeightPos[3]+pmtWeightPos[4]+pmtWeightPos[5]+pmtWeightPos[6]+pmtWeightPos[7]+pmtWeightNeg[0]+pmtWeightNeg[1]+pmtWeightNeg[2]+pmtWeightNeg[3]+pmtWeightNeg[4]+pmtWeightNeg[5]+pmtWeightNeg[6]+pmtWeightNeg[7])/16;

  for(Int_t i=0; i<8; i++){

    barsumWeight[i] =  (pmtWeightPos[i] + pmtWeightNeg[i])/2;

//     printf("%d %5.7f\n",i+1,barsumWeight[i]);

  }
//   printf("%5.7f\n",allbarsWeight);


//   TString rootFileName = "QwPass1";
  TString rootFileName = "Qweak";

  TFile *f = new TFile(Form("%s/%s_%d.000.trees.root",rootfileDir.Data(),rootFileName.Data(),run_number));
  if (!f->IsOpen()) {printf("File not found. Exiting the program!\n"); 

    exit(1); 
  }

  TChain * Hel_Tree = new TChain("Hel_Tree");
  if(Hel_Tree == NULL) {printf("Unable to find Hel_Tree. Exiting the program!\n"); exit(1);}

  for(Int_t j=0; j<2; j++){ 
    Hel_Tree->Add(Form("%s/%s_%d.00%d.trees.root",rootfileDir.Data(),rootFileName.Data(),run_number,j));
  }


//   //#### WEIGHTED YIELDS ####
//   Hel_Tree->SetAlias("wt_yield_qwk_md1neg" , Form("yield_qwk_md1neg*%f",pmtNeg1Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md2neg" , Form("yield_qwk_md2neg*%f",pmtNeg2Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md3neg" , Form("yield_qwk_md3neg*%f",pmtNeg3Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md4neg" , Form("yield_qwk_md4neg*%f",pmtNeg4Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md5neg" , Form("yield_qwk_md5neg*%f",pmtNeg5Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md6neg" , Form("yield_qwk_md6neg*%f",pmtNeg6Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md7neg" , Form("yield_qwk_md7neg*%f",pmtNeg7Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md8neg" , Form("yield_qwk_md8neg*%f",pmtNeg8Wt[0]));

//   Hel_Tree->SetAlias("wt_yield_qwk_md1pos" , Form("yield_qwk_md1pos*%f",pmtPos1Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md2pos" , Form("yield_qwk_md2pos*%f",pmtPos2Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md3pos" , Form("yield_qwk_md3pos*%f",pmtPos3Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md4pos" , Form("yield_qwk_md4pos*%f",pmtPos4Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md5pos" , Form("yield_qwk_md5pos*%f",pmtPos5Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md6pos" , Form("yield_qwk_md6pos*%f",pmtPos6Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md7pos" , Form("yield_qwk_md7pos*%f",pmtPos7Wt[0]));
//   Hel_Tree->SetAlias("wt_yield_qwk_md8pos" , Form("yield_qwk_md8pos*%f",pmtPos8Wt[0]));


//   //#### NORMALIZED WEIGHTED YIELDS :: BARSUMS####
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md1neg" , Form("wt_yield_qwk_md1neg/%f",barsum1Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md2neg" , Form("wt_yield_qwk_md2neg/%f",barsum2Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md3neg" , Form("wt_yield_qwk_md3neg/%f",barsum3Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md4neg" , Form("wt_yield_qwk_md4neg/%f",barsum4Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md5neg" , Form("wt_yield_qwk_md5neg/%f",barsum5Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md6neg" , Form("wt_yield_qwk_md6neg/%f",barsum6Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md7neg" , Form("wt_yield_qwk_md7neg/%f",barsum7Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md8neg" , Form("wt_yield_qwk_md8neg/%f",barsum8Wt[0]));

//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md1pos" , Form("wt_yield_qwk_md1pos/%f",barsum1Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md2pos" , Form("wt_yield_qwk_md2pos/%f",barsum2Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md3pos" , Form("wt_yield_qwk_md3pos/%f",barsum3Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md4pos" , Form("wt_yield_qwk_md4pos/%f",barsum4Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md5pos" , Form("wt_yield_qwk_md5pos/%f",barsum5Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md6pos" , Form("wt_yield_qwk_md6pos/%f",barsum6Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md7pos" , Form("wt_yield_qwk_md7pos/%f",barsum7Wt[0]));
//   Hel_Tree->SetAlias("barnorm_wt_yield_qwk_md8pos" , Form("wt_yield_qwk_md8pos/%f",barsum8Wt[0]));


//   //#### NORMALIZED WEIGHTED YIELDS :: ALL TUBES####
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md1neg" , Form("wt_yield_qwk_md1neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md2neg" , Form("wt_yield_qwk_md2neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md3neg" , Form("wt_yield_qwk_md3neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md4neg" , Form("wt_yield_qwk_md4neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md5neg" , Form("wt_yield_qwk_md5neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md6neg" , Form("wt_yield_qwk_md6neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md7neg" , Form("wt_yield_qwk_md7neg/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md8neg" , Form("wt_yield_qwk_md8neg/%f",allbarsumWt[0]));

//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md1pos" , Form("wt_yield_qwk_md1pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md2pos" , Form("wt_yield_qwk_md2pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md3pos" , Form("wt_yield_qwk_md3pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md4pos" , Form("wt_yield_qwk_md4pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md5pos" , Form("wt_yield_qwk_md5pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md6pos" , Form("wt_yield_qwk_md6pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md7pos" , Form("wt_yield_qwk_md7pos/%f",allbarsumWt[0]));
//   Hel_Tree->SetAlias("norm_wt_yield_qwk_md8pos" , Form("wt_yield_qwk_md8pos/%f",allbarsumWt[0]));


  gStyle->SetTitleSize(0.060);
  gStyle->SetTitleYSize(0.060);
  gStyle->SetTitleXSize(0.060);
  gStyle->SetTitleYOffset(0.90);
  gStyle->SetTitleXOffset(0.80);
  gStyle->SetLabelSize(0.060);
  gStyle->SetLabelSize(0.060,"y");
  gStyle->SetLabelSize(0.060,"x");
  //  gStyle->SetNdivisions(510,"x");
  gStyle->SetNdivisions(-505,"x");
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.32);
  gStyle->SetStatH(0.42);
  gStyle->SetTitleSize(0.1,"t"); 

  int position1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  int position2[8] = { 9, 10, 11, 12, 13, 14, 15, 16 };

  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[2] ={1200,600};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.945,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.955};
  Double_t markerSize[6] = {0.9,0.6,0.7,0.5,1.2,0.8};
  Double_t legendCoordinates[4] = {0.1,0.83,0.65,0.95};
  Double_t yScale[2] = {-10.0,10.0};
  Double_t waterMark[2] = {2.5,-1.0};

  TCanvas *check1 = new TCanvas("check1",Form("%d: Yield vs Time for Main Detector1 Pos PMT",run_number),1000,500);
  check1->Draw();
  check1->SetBorderSize(0);
  check1->cd();

  Hel_Tree->Draw(Form("yield_qwk_md1pos:mps_counter"));

  check1->Update();
  check1->SaveAs(Form("%dCutCheck.png",run_number));

  Double_t opt1 =1700;
  Double_t opt2 =2100;

  std::cout<<Form("Please insert cuts after the looking at first plot")<<std::endl;
  std::cout<<Form("Insert Left ")<<std::endl;
  //   std::cin>>opt;
  std::string input_opt1,input_opt2;
  std::getline( std::cin, input_opt1 );
  if ( !input_opt1.empty() ) {
    std::istringstream stream( input_opt1 );
    stream >> opt1;
  }
  std::cout<<Form("Insert Right")<<std::endl;
  std::getline( std::cin, input_opt2 );
  if ( !input_opt2.empty() ) {
    std::istringstream stream( input_opt2 );
    stream >> opt2;
  }

  TString cut = Form("mps_counter<%f*1e3 || mps_counter>%f*1e3",opt1,opt2);


  /************************************/
  /************************************/

  if(PMTYIELDVARIATION){

    TCanvas *c01 = new TCanvas("c01",Form("%d: Yield vs Time for Main Detector PMTs",run_number),1600,1000);
    c01->Draw();
    c01->SetBorderSize(0);
    c01->cd();

    TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad1->SetFillColor(kWhite);
    pad1->Draw();
    pad2->Divide(4,4);
    pad2->Draw();
    pad1->cd();
    TString text = Form("%d: Yield vs Time for Main Detector PMTs",run_number);
    t1 = new TText(0.32,0.3,text);
    t1->SetTextSize(0.5);
    t1->Draw();
    pad2->cd();

    for(Int_t i=0; i<8; i++){ 

      pad2->cd(position1[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dpos:mps_counter",i+1),Form("%s",cut.Data()));

      pad2->cd(position2[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dneg:mps_counter",i+1),Form("%s",cut.Data()));


    }

    c01->Update();
    c01->SaveAs(Form("%dyieldsVariation.png",run_number));

  }

  /************************************/
  /************************************/

  if(PMTYIELD){

    TH1* histYpmtpos; TH1* histYpmtneg;

    Double_t calcYpmtposMean[8],calcYpmtnegMean[8],calcYpmtposRMS[8],calcYpmtnegRMS[8];
//     TString cut[8];

    std::ofstream MyfileYieldPos,MyfileYieldNeg;
    Char_t  textfileYieldPos[400],textfileYieldNeg[400];
    sprintf(textfileYieldPos,"%dtextfileYieldPos.txt",run_number);
    sprintf(textfileYieldNeg,"%dtextfileYieldNeg.txt",run_number);
    MyfileYieldPos.open(textfileYieldPos);
    MyfileYieldNeg.open(textfileYieldNeg);

    TCanvas *c00 = new TCanvas("c00",Form("%d: Yield for Main Detector PMTs",run_number),1600,1000);
    c00->Draw();
    c00->SetBorderSize(0);
    c00->cd();

    TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad1->SetFillColor(kWhite);
    pad1->Draw();
    pad2->Divide(4,4);
    pad2->Draw();
    pad1->cd();
    TString text = Form("%d: Yield for Main Detector PMTs",run_number);
    t1 = new TText(0.32,0.3,text);
    t1->SetTextSize(0.5);
    t1->Draw();
    pad2->cd();

    for(Int_t i=0; i<8; i++){ 

      pad2->cd(position1[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dpos>>hYpmtpos%d",i+1,i+1),Form("%s",cut.Data()));
      histYpmtpos = (TH1F *)gDirectory->Get(Form("hYpmtpos%d",i+1));
      histYpmtpos->SetLineColor(kRed);
      histYpmtpos->Draw();
      histYpmtpos->GetXaxis()->SetTitle(Form("MD%d Pos",i+1));
      histYpmtpos->SetTitle(Form("Yield of Pos PMT for MD%d ",i+1));

      pad2->cd(position2[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dneg>>hYpmtneg%d",i+1,i+1),Form("%s",cut.Data()));
      histYpmtneg = (TH1F *)gDirectory->Get(Form("hYpmtneg%d",i+1));
      histYpmtneg->SetLineColor(kBlack);
      histYpmtneg->Draw();
      histYpmtneg->GetXaxis()->SetTitle(Form("MD%d Neg",i+1));
      histYpmtneg->SetTitle(Form("Yield of Neg PMT for MD%d ",i+1));

      calcYpmtposMean[i] =  histYpmtpos->GetMean();
      calcYpmtposRMS[i]  =  histYpmtpos->GetRMS();

      calcYpmtnegMean[i] =  histYpmtneg->GetMean();
      calcYpmtnegRMS[i]  =  histYpmtneg->GetRMS();

//       cut[i] = Form("abs(yield_qwk_md%dpos-%f)<%f*0.1",i+1,calcYpmtposMean[i],calcYpmtposMean[i]);

//       printf("%5.7f %5.7f, %s\n",calcYpmtposMean[i],calcYpmtposRMS[i],cut[i].Data());

    }

      MyfileYieldPos<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcYpmtposMean[0],calcYpmtposMean[1],calcYpmtposMean[2],calcYpmtposMean[3]
			       ,calcYpmtposMean[4],calcYpmtposMean[5],calcYpmtposMean[6],calcYpmtposMean[7])<<endl;
      MyfileYieldNeg<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcYpmtnegMean[0],calcYpmtnegMean[1],calcYpmtnegMean[2],calcYpmtnegMean[3]
			       ,calcYpmtnegMean[4],calcYpmtnegMean[5],calcYpmtnegMean[6],calcYpmtnegMean[7])<<endl;
      MyfileYieldPos.close();
      MyfileYieldNeg.close();


    c00->Update();
    c00->SaveAs(Form("%dyields.png",run_number));

  }
  /************************************/
  /************************************/


  if(ALLBARSWEIGHT){

    TH1* histallbarspos; TH1* histallbarsneg;

    Double_t calcAllbarsWtNeg[8],calcAllbarsWtPos[8];

    std::ofstream MyfileAllbarsWtPos,MyfileAllbarsWtNeg;
    Char_t  textfileAllbarsWtPos[400],textfileAllbarsWtNeg[400];
    sprintf(textfileAllbarsWtPos,"%dtextfileAllbarsWtPos.txt",run_number);
    sprintf(textfileAllbarsWtNeg,"%dtextfileAllbarsWtNeg.txt",run_number);
    MyfileAllbarsWtPos.open(textfileAllbarsWtPos);
    MyfileAllbarsWtNeg.open(textfileAllbarsWtNeg);


    TCanvas *c1 = new TCanvas("c1",Form("%d: Normalized Yield of PMTs w.r.t Allbars for Main Detectors",run_number),1600,1000);
    c1->Draw();
    c1->SetBorderSize(0);
    c1->cd();

    TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad1->SetFillColor(kWhite);
    pad1->Draw();
    pad2->Divide(4,4);
    pad2->Draw();
    pad1->cd();
    TString text = Form("%d: Normalized Yield of PMTs w.r.t Allbars for Main Detectors {16*yield_mdipos(neg)/SUM(wi)}",run_number);
    t1 = new TText(0.17,0.3,text);
    t1->SetTextSize(0.5);
    t1->Draw();
    pad2->cd();

    for(Int_t i=0; i<8; i++){ 

      pad2->cd(position1[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dneg/%f>>hallbarsneg%d",i+1,allbarsWeight,i+1),Form("%s",cut.Data()));
      histallbarsneg = (TH1F *)gDirectory->Get(Form("hallbarsneg%d",i+1));
      histallbarsneg->Draw();
      histallbarsneg->GetXaxis()->SetTitle(Form("MD%d Neg",i+1));
      histallbarsneg->SetTitle(Form("Normalized Yield of Neg PMT w.r.t Allbars for MD%d ",i+1));

      pad2->cd(position2[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dpos/%f>>hallbarspos%d",i+1,allbarsWeight,i+1),Form("%s",cut.Data()));
      histallbarspos = (TH1F *)gDirectory->Get(Form("hallbarspos%d",i+1));
      histallbarspos->SetLineColor(kRed);
      histallbarspos->Draw();
      histallbarspos->GetXaxis()->SetTitle(Form("MD%d pos",i+1));
      histallbarspos->SetTitle(Form("Normalized Yield of Pos PMT w.r.t Allbars for MD%d ",i+1));


      calcAllbarsWtNeg[i] = histallbarsneg->GetMean();
      calcAllbarsWtPos[i] = histallbarspos->GetMean();

    }

      printf("%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f\n"
	     ,calcAllbarsWtNeg[0],calcAllbarsWtNeg[1],calcAllbarsWtNeg[2],calcAllbarsWtNeg[3],calcAllbarsWtNeg[4],calcAllbarsWtNeg[5],calcAllbarsWtNeg[6],calcAllbarsWtNeg[7]);

      MyfileAllbarsWtPos<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcAllbarsWtPos[0],calcAllbarsWtPos[1],calcAllbarsWtPos[2],calcAllbarsWtPos[3]
			       ,calcAllbarsWtPos[4],calcAllbarsWtPos[5],calcAllbarsWtPos[6],calcAllbarsWtPos[7])<<endl;
      MyfileAllbarsWtNeg<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcAllbarsWtNeg[0],calcAllbarsWtNeg[1],calcAllbarsWtNeg[2],calcAllbarsWtNeg[3]
			       ,calcAllbarsWtNeg[4],calcAllbarsWtNeg[5],calcAllbarsWtNeg[6],calcAllbarsWtNeg[7])<<endl;
      MyfileAllbarsWtPos.close();
      MyfileAllbarsWtNeg.close();

    c1->Modified();
    c1->Update();
    c1->SaveAs(Form("%dnormalizedWeightedYieldsAllTubes.png",run_number));
  }
  /************************************/
  /************************************/

  if(BARSUMWEIGHT){
    TH1* histbarsumpos; TH1* histbarsumneg;

    Double_t calcBarsumWtNeg[8],calcBarsumWtPos[8];

    std::ofstream MyfileBarsumWtPos,MyfileBarsumWtNeg;
    Char_t  textfileBarsumWtPos[400],textfileBarsumWtNeg[400];
    sprintf(textfileBarsumWtPos,"%dtextfileBarsumWtPos.txt",run_number);
    sprintf(textfileBarsumWtNeg,"%dtextfileBarsumWtNeg.txt",run_number);
    MyfileBarsumWtPos.open(textfileBarsumWtPos);
    MyfileBarsumWtNeg.open(textfileBarsumWtNeg);

    TCanvas *c2 = new TCanvas("c2",Form("%d: Normalized Yield of PMTs w.r.t Barsum for Main Detectors",run_number),1600,1000);
    c2->Draw();
    c2->SetBorderSize(0);
    c2->cd();

    TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad1->SetFillColor(kWhite);
    pad1->Draw();
    pad2->Divide(4,4);
    pad2->Draw();
    pad1->cd();
    TString text = Form("%d: Normalized Yield of PMTs w.r.t Barsum for Main Detectors {2*yield_mdipos(neg)/(wipos + wineg)}",run_number);
    t1 = new TText(0.17,0.3,text);
    t1->SetTextSize(0.5);
    t1->Draw();
    pad2->cd();

    for(Int_t i=0; i<8; i++){ 

      pad2->cd(position1[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dneg/%f>>hbarsumneg%d",i+1,barsumWeight[i],i+1),Form("%s",cut.Data()));
      histbarsumneg = (TH1F *)gDirectory->Get(Form("hbarsumneg%d",i+1));
      histbarsumneg->Draw();
      histbarsumneg->GetXaxis()->SetTitle(Form("MD%d Neg",i+1));
      histbarsumneg->SetTitle(Form("Normalized Yield of Neg PMT w.r.t Barsum for MD%d ",i+1));

      pad2->cd(position2[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dpos/%f>>hbarsumpos%d",i+1,barsumWeight[i],i+1),Form("%s",cut.Data()));
      histbarsumpos = (TH1F *)gDirectory->Get(Form("hbarsumpos%d",i+1));
      histbarsumpos->SetLineColor(kRed);
      histbarsumpos->Draw();
      histbarsumpos->GetXaxis()->SetTitle(Form("MD%d Pos",i+1));
      histbarsumpos->SetTitle(Form("Normalized Yield of Pos PMT w.r.t Barsum for MD%d ",i+1));

      calcBarsumWtNeg[i] = histbarsumneg->GetMean();
      calcBarsumWtPos[i] = histbarsumpos->GetMean();

    }

      printf("%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f\n"
	     ,calcBarsumWtNeg[0],calcBarsumWtNeg[1],calcBarsumWtNeg[2],calcBarsumWtNeg[3],calcBarsumWtNeg[4],calcBarsumWtNeg[5],calcBarsumWtNeg[6],calcBarsumWtNeg[7]);


      MyfileBarsumWtPos<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcBarsumWtPos[0],calcBarsumWtPos[1],calcBarsumWtPos[2],calcBarsumWtPos[3]
			       ,calcBarsumWtPos[4],calcBarsumWtPos[5],calcBarsumWtPos[6],calcBarsumWtPos[7])<<endl;
      MyfileBarsumWtNeg<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcBarsumWtNeg[0],calcBarsumWtNeg[1],calcBarsumWtNeg[2],calcBarsumWtNeg[3]
			       ,calcBarsumWtNeg[4],calcBarsumWtNeg[5],calcBarsumWtNeg[6],calcBarsumWtNeg[7])<<endl;
      MyfileBarsumWtPos.close();
      MyfileBarsumWtNeg.close();

    c2->Modified();
    c2->Update();
    c2->SaveAs(Form("%dnormalizedWeightedYieldsBarsum.png",run_number));
  }
  /************************************/
  /************************************/

  if(PMTWEIGHT){
    TH1* histpmtpos; TH1* histpmtneg;

    Double_t calcWtNeg[8],calcWtPos[8];

    std::ofstream MyfileWtPos,MyfileWtNeg;
    Char_t  textfileWtPos[400],textfileWtNeg[400];
    sprintf(textfileWtPos,"%dtextfileWtPos.txt",run_number);
    sprintf(textfileWtNeg,"%dtextfileWtNeg.txt",run_number);
    MyfileWtPos.open(textfileWtPos);
    MyfileWtNeg.open(textfileWtNeg);

    TCanvas *c3 = new TCanvas("c3",Form("%d: Weighted Yield of PMTs for Main Detectors",run_number),1600,1000);
    c3->Draw();
    c3->SetBorderSize(0);
    c3->cd();

    TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad1->SetFillColor(kWhite);
    pad1->Draw();
    pad2->Divide(4,4);
    pad2->Draw();
    pad1->cd();
    TString text = Form("%d: Weighted Yield of PMTs for Main Detectors {yield_mdipos(neg)*wi}",run_number);
    t1 = new TText(0.22,0.3,text);
    t1->SetTextSize(0.5);
    t1->Draw();
    pad2->cd();


    for(Int_t i=0; i<8; i++){ 

      pad2->cd(position1[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dneg*%f>>hpmtneg%d",i+1,pmtWeightNeg[i],i+1),Form("%s",cut.Data()));
      histpmtneg = (TH1F *)gDirectory->Get(Form("hpmtneg%d",i+1));
      histpmtneg->Draw();
      histpmtneg->GetXaxis()->SetTitle(Form("MD%d Neg",i+1));
      histpmtneg->SetTitle(Form("Weighted Yield of Neg PMT for MD%d ",i+1));


      pad2->cd(position2[i]);
      Hel_Tree->Draw(Form("yield_qwk_md%dpos*%f>>hpmtpos%d",i+1,pmtWeightPos[i],i+1),Form("%s",cut.Data()));
      histpmtpos = (TH1F *)gDirectory->Get(Form("hpmtpos%d",i+1));
      histpmtpos->SetLineColor(kRed);
      histpmtpos->Draw();
      histpmtpos->GetXaxis()->SetTitle(Form("MD%d Pos",i+1));
      histpmtpos->SetTitle(Form("Weighted Yield of Pos PMT for MD%d ",i+1));

      gPad->Update();

      calcWtNeg[i] = histpmtneg->GetMean();
      calcWtPos[i] = histpmtpos->GetMean();

    }

      printf("%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f\n"
	     ,calcWtNeg[0],calcWtNeg[1],calcWtNeg[2],calcWtNeg[3],calcWtNeg[4],calcWtNeg[5],calcWtNeg[6],calcWtNeg[7]);

      MyfileWtPos<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcWtPos[0],calcWtPos[1],calcWtPos[2],calcWtPos[3]
			       ,calcWtPos[4],calcWtPos[5],calcWtPos[6],calcWtPos[7])<<endl;
      MyfileWtNeg<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcWtNeg[0],calcWtNeg[1],calcWtNeg[2],calcWtNeg[3]
			       ,calcWtNeg[4],calcWtNeg[5],calcWtNeg[6],calcWtNeg[7])<<endl;
      MyfileWtPos.close();
      MyfileWtNeg.close();

    c3->Modified();
    c3->Update();
    c3->SaveAs(Form("%dweightedYields.png",run_number));
  }

  /************************************/
  /************************************/

  if(PMTWEIGHTDIFF){
    TH1* histpmtdiff;

    Double_t calcWtDiff[8];

    std::ofstream MyfileWtDiff;
    Char_t  textfileWtDiff[400];
    sprintf(textfileWtDiff,"%dtextfileWtDiff.txt",run_number);
    MyfileWtDiff.open(textfileWtDiff);

    TCanvas *c4 = new TCanvas("c4",Form("%d: Fractional Weighted Yield Diffrence of Pos and Neg PMT for Main Detectors",run_number),1600,1000);
    c4->Draw();
    c4->SetBorderSize(0);
    c4->cd();

    TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
    TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
    pad1->SetFillColor(kWhite);
    pad1->Draw();
    pad2->Divide(2,4);
    pad2->Draw();
    pad1->cd();
    TString text = Form("%d: Yield Diffrence of Pos and Neg PMT for MDs {2*(yield_mdineg*wineg - yield_mdipos*wipos)/(yield_mdineg*wineg + yield_mdipos*wipos)}",run_number);
    t1 = new TText(0.00,0.3,text);
    t1->SetTextSize(0.5);
    t1->Draw();
    pad2->cd();



    for(Int_t i=0; i<8; i++){ 

      pad2->cd(position1[i]);
      Hel_Tree->Draw(Form("(yield_qwk_md%dneg*%f - yield_qwk_md%dpos*%f)/((yield_qwk_md%dneg*%f + yield_qwk_md%dpos*%f)/2)>>hpmtdiff%d",i+1,pmtWeightNeg[i],i+1,pmtWeightPos[i],i+1,pmtWeightNeg[i],i+1,pmtWeightPos[i],i+1),Form("%s",cut.Data()));
      histpmtdiff = (TH1F *)gDirectory->Get(Form("hpmtdiff%d",i+1));
      histpmtdiff->Draw();
      histpmtdiff->GetXaxis()->SetTitle(Form("(MD%d Neg - MD%d Pos)/(MD%d Neg + MD%d Pos)/2",i+1,i+1,i+1,i+1));
      histpmtdiff->SetTitle(Form("Yield Diffrence of Pos and Neg PMT for MD%d ",i+1));
      gPad->Update();

      calcWtDiff[i] = histpmtdiff->GetMean();

    }

      printf("%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f\n"
	     ,calcWtDiff[0],calcWtDiff[1],calcWtDiff[2],calcWtDiff[3],calcWtDiff[4],calcWtDiff[5],calcWtDiff[6],calcWtDiff[7]);

      MyfileWtDiff<<Form("%d\t%5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f %5.7f"
			       ,run_number,calcWtDiff[0],calcWtDiff[1],calcWtDiff[2],calcWtDiff[3]
			       ,calcWtDiff[4],calcWtDiff[5],calcWtDiff[6],calcWtDiff[7])<<endl;
      MyfileWtDiff.close();

    c4->Update();
    c4->SaveAs(Form("%dweightedYieldsDifference.png",run_number));
  }
  /************************************/
  /************************************/







}
