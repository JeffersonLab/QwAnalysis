#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
//#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "cluster.C"
#include "simpleAvg.C"
#include "slug.C"
const char *pPath = getenv("QWSCRATCH");
const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
//const Int_t runBegin = 24000, runEnd = 24500;
//const Int_t runBegin = 24250, runEnd = 24450;///stable scalers

Int_t allRunPol_3v(Int_t group = 30) {
  gROOT->SetStyle("publication");
  Bool_t bRatio =0;
  Bool_t bDiff = 0;
  Bool_t bRateScan = 0;
  Bool_t bPolV2 = 0, bPolV1 =0;
  Bool_t bCorrDT = 0;
  Bool_t bIndepCorrDT = 0;
  Bool_t bScint = 0;
  Bool_t bBeam = 0;
  Bool_t bEnergy = 0;
  Bool_t bSecular = 0;
  Bool_t bSlugRatio = 1;
  ifstream fIn;
  ofstream fOut, fCheck;
  TString ver1, ver2, file;
  std::vector<Double_t> dummy;
  std::vector<Double_t> finalRuns,polCol1, polCol1Er, polCol2, polCol2Er, polCol3, polCol3Er, acTr, p4On, p1ScH1BgdSub;
  std::vector<Double_t> polS1, polErS1, polS2, polErS2, polS3, polErS3; ///polarization in various colums
  Double_t d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
  Double_t d11, d12, d13, d14;
  TString s1, s2, s3, s4, s5, s6, s7, s8, s9, s0;
  TF1 *lFit = new TF1("lFit", "pol0", runBegin, runEnd);
  lFit->SetLineColor(kBlack);
  TF1 *lFitV1 = new TF1("lFitV1", "pol0", 22373, 23000);
  lFitV1->SetLineColor(kGreen);
  //TF1 *lFitV2 = new TF1("lFitV2", "pol0", runBegin, runEnd);
  TF1 *lFitV2 = new TF1("lFitV2", "pol0", 23200, 23700);
  lFitV2->SetLineColor(kMagenta);
  TF1 *lFitV3 = new TF1("lFitV3", "pol0", 24200, 25000);
  lFitV3->SetLineColor(kBlue);
  fCheck.open("allRunPol_excluded.txt");

  std::vector<Double_t> scintRate, scintRateEr, scintRMS;
  std::vector<Double_t> meanE;
  //fIn.open("combFiles_30Sep14.info");
  //file = "combFiles_Ver001.info";
  //file = "combFiles_Ver001_31Oct14.info";
  //file = "combFiles_ver001_02Nov14.info";
  //file = "combFiles_ver004_05Nov14.info";///only for range of stable scaler rates
  //file = "combFiles_ver001_06Nov14.info";
  //file = "combFiles_energyEffect.info";
  //file = "combFiles_ver001_11Nov14.info";
  file = "combFiles_ver005_12Nov14.info";
  fIn.open(file);
  if(fIn.is_open()) {
    fIn>>s0>>s1>>s2>>s3>>s4>>s5>>s6>>s7>>s8>>s9;
    while(fIn>>d1>>d2>>d3>>d4>>d5>>d6>>d7>>d8>>d9>>d10>>d11>>d12>>d13>>d14 && fIn.good()) {
      if(d1>=runBegin && d1<=runEnd) {///limiting run range
        //if(d2>80.0 && d4>80 && d6>80) {
        //  if(d3<1.5 && d5<1.5 && d7<1.5) {///excluding polEr > 1.5% in Ac_noDT
        //    if(d3>0.01 && d5>0.01 && d7>0.01) {
        if(d9>0 && d9<=1000) cout<<red<<d1<<"   "<<d9<<normal<<endl;
        finalRuns.push_back(d1);//run#
        polCol1.push_back(d2);//polAc
        polCol1Er.push_back(d3);//polErAc
        polCol2.push_back(d4);//polSc
        polCol2Er.push_back(d5);//polErSc
        polCol3.push_back(d6);//polSc_noDT
        polCol3Er.push_back(d7);//polErSc_noDT
        acTr.push_back(d8);//acTrig
        p4On.push_back(d9);//plane 4 rates
        p1ScH1BgdSub.push_back(d10);//plane1 Bgd Sub Scalar aggregate
        scintRate.push_back(d11);
        scintRateEr.push_back(d12);
        scintRMS.push_back(d13);
        if(d14!=0.0) meanE.push_back(d14);
        else meanE.push_back(1157.53);
        //    } else fCheck<<d1<<"\t"<<d2<<"\t"<<d3<<"\t"<<d4<<"\t"<<d5<<"\t"<<d6<<"\t"<<d7<<endl;
        //  } else fCheck<<d1<<"\t"<<d2<<"\t"<<d3<<"\t"<<d4<<"\t"<<d5<<"\t"<<d6<<"\t"<<d7<<endl;
        //} else fCheck<<d1<<"\t"<<d2<<"\t"<<d3<<"\t"<<d4<<"\t"<<d5<<"\t"<<d6<<"\t"<<d7<<endl;
      }
    }
    fIn.close();
    fCheck.close();
  } else {
    cout<<red<<"couldn't open "<<file<<normal<<endl;
    return -1;
  }
  cout<<blue<<finalRuns.size()<<" runs found in the final List"<<endl;
  cout<<"allRunPol_excluded.txt has the list of runs that got excluded"<<endl;

  ///Only one of these can be turned On
  //pick and choose what needs to be compared from below
  Bool_t bS1S2 =1;
  Bool_t bS3S2 =0;
  Bool_t bS2S3 =0;
  Bool_t bS1S3 =0;
  if(bS1S2) {//keeping the noDT version in denominator
    polS1 =   polCol1;  
    polErS1 = polCol1Er;
    polS2 =    polCol2;   
    polErS2 =  polCol2Er;
    ver1 = s1;//"Ac2parDT";
    ver2 = s3;//"AcNoDT";
  } else if(bS3S2) {
    polS1 =   polCol3;
    polErS1 = polCol3Er;
    polS2 =   polCol2;
    polErS2 = polCol2Er;
    ver1 = s5;//"Ac2parDT";
    ver2 = s3;//"ScNoDT";
  } else if(bS2S3) {
    polS1 =   polCol2;
    polErS1 = polCol2Er;
    polS2 =   polCol3;
    polErS2 = polCol3Er;
    ver1 = s3;//"Ac2parDT";
    ver2 = s5;//"ScNoDT";
  } else if(bS1S3) { 
    polS1 =  polCol1; 
    polErS1 = polCol1Er;
    polS2 =   polCol3;
    polErS2 = polCol3Er;
    ver1 = s1;//"AcNoDT";
    ver2 = s5;//"ScNoDT";
  }///Only one of the above can be turned On
  cout<<magenta<<"comparing "<<ver1<<" and "<<ver2<<normal<<endl;
  ///Lets group them in terms of their trigger
  std::vector<Double_t> runs_23, polV1_23, polErV1_23, polV2_23, polErV2_23;
  std::vector<Double_t> runs_24, polV1_24, polErV1_24, polV2_24, polErV2_24;
  std::vector<Double_t> runs_33, polV1_33, polErV1_33, polV2_33, polErV2_33;
  std::vector<Double_t> runs_34, polV1_34, polErV1_34, polV2_34, polErV2_34;
  std::vector<Double_t> scint_23, scint_24, scint_33, scint34;
  std::vector<Double_t> scintN, scintNEr;
  std::vector<Double_t> scintEr_23, scintEr_24, scintEr_33, scintEr_34;
  std::vector<Double_t> beamE;
  std::vector<Double_t> wmRuns_23, wmPolV2_23, wmPolErV2_23, wmPolV1_23, wmPolErV1_23;
  std::vector<Double_t> wmRuns_33, wmPolV2_33, wmPolErV2_33, wmPolV1_33, wmPolErV1_33;
  std::vector<Double_t> wmRuns_24, wmPolV2_24, wmPolErV2_24, wmPolV1_24, wmPolErV1_24;
  std::vector<Double_t> wmRuns_34, wmPolV2_34, wmPolErV2_34, wmPolV1_34, wmPolErV1_34;
  std::vector<Double_t> wmRuns_23_0, wmRuns_33_0; //dummy needed for passing to the function 
  std::vector<Double_t> p1Agg_23, p1Agg_24, p1Agg_33, p1Agg_34;

  for(int i=0; i<(int)finalRuns.size() ; i++) {
    if((acTr[i]==2) && p4On[i]>1000) {/// trig: 2/4
      runs_24.push_back(finalRuns[i]);
      p1Agg_24.push_back(p1ScH1BgdSub[i]);

      polV1_24.push_back(polS1[i]);
      polErV1_24.push_back(polErS1[i]);

      polV2_24.push_back(polS2[i]);
      polErV2_24.push_back(polErS2[i]);

      scintN.push_back(scintRate[i]);
      scintNEr.push_back(scintRateEr[i]);

      beamE.push_back(meanE[i]);
    } else if((acTr[i]==2) && p4On[i]==0) {
      runs_23.push_back(finalRuns[i]);
      p1Agg_23.push_back(p1ScH1BgdSub[i]);

      polV1_23.push_back(polS1[i]);
      polErV1_23.push_back(polErS1[i]);

      polV2_23.push_back(polS2[i]);
      polErV2_23.push_back(polErS2[i]);

      scintN.push_back(scintRate[i]);
      scintNEr.push_back(scintRateEr[i]);

      beamE.push_back(meanE[i]);
    } else if((acTr[i]==3) && p4On[i]>1000) {
      runs_34.push_back(finalRuns[i]);
      p1Agg_34.push_back(p1ScH1BgdSub[i]);

      polV1_34.push_back(polS1[i]);
      polErV1_34.push_back(polErS1[i]);

      polV2_34.push_back(polS2[i]);
      polErV2_34.push_back(polErS2[i]);

      scintN.push_back(scintRate[i]);
      scintNEr.push_back(scintRateEr[i]);

      beamE.push_back(meanE[i]);
    } else if((acTr[i]==3) && p4On[i]==0) {
      runs_33.push_back(finalRuns[i]);
      p1Agg_33.push_back(p1ScH1BgdSub[i]);

      polV1_33.push_back(polS1[i]);
      polErV1_33.push_back(polErS1[i]);

      polV2_33.push_back(polS2[i]);
      polErV2_33.push_back(polErS2[i]);

      scintN.push_back(scintRate[i]);
      scintNEr.push_back(scintRateEr[i]);

      beamE.push_back(meanE[i]);
    }
  }

  std::vector<Double_t> rV1V2_23, rV1V2_33, rV1V2_24, rV1V2_34;
  std::vector<Double_t> rV1V2Er_24, rV1V2Er_34, rV1V2Er_23, rV1V2Er_33;
  std::vector<Double_t> diffV1V2_23, diffV1V2_24, diffV1V2_33, diffV1V2_34;
  TGraphErrors *grV1_24, *grV1_23, *grV1_33, *grV1_34;
  TGraphErrors *grV2_24, *grV2_23, *grV2_33, *grV2_34;
  TGraphErrors *grR_24, *grR_23, *grR_33, *grR_34;
  TGraphErrors *grDiff_24, *grDiff_23, *grDiff_33, *grDiff_34;

  file = Form("ratio_"+ver1+"_"+ver2+"_G%d.txt",group);
  fOut.open(file);
  if((Int_t)runs_23.size()>group) {
    cluster(group,runs_23,polV1_23,polErV1_23,wmRuns_23,  wmPolV1_23,wmPolErV1_23);
    cluster(group,runs_23,polV2_23,polErV2_23,wmRuns_23_0,wmPolV2_23,wmPolErV2_23);

    //!!temp to test
    //for(int i=0; i<(int)wmRuns_23.size(); i++) {
    //  cout<<i<<"\t"<<wmRuns_23[i]<<"\t"<<wmPolV1_23[i]<<endl;
    //}
    for(int r=0; r<(int)wmRuns_23.size(); r++) {
      diffV1V2_23.push_back( wmPolV1_23.at(r) - wmPolV2_23.at(r) );
    }
    cout<<"size of runs_23 is: "<<runs_23.size()<<", and wmPolV1 for 2/3 is "<<wmPolV1_23.size()<<endl;
    for(Int_t r=0; r<(Int_t)wmRuns_23.size(); r++) {
      rV1V2_23.push_back(wmPolV1_23.at(r)/wmPolV2_23.at(r));
      rV1V2Er_23.push_back( TMath::Sqrt(pow(wmPolErV1_23.at(r)/wmPolV2_23.at(r),2) + pow((wmPolV1_23.at(r)/(pow(wmPolV2_23.at(r),2)))*wmPolErV2_23.at(r), 2) ));
      //rV1V2Er_23.push_back(0.0);
      fOut<<wmRuns_23[r]<<"\t"<<rV1V2_23.back()<<"\t"<<rV1V2Er_23.back()<<endl;
      //rV1V2Er_23.push_back(wmPolErV1_23.at(r));
    }
    grV1_23 = new TGraphErrors((Int_t)wmRuns_23.size(), wmRuns_23.data(), wmPolV1_23.data(), 0, wmPolErV1_23.data());
    grV1_23->SetMarkerStyle(kOpenSquare);
    grV1_23->SetMarkerColor(kBlue);
    grV1_23->SetLineColor(kBlue);
    grV2_23 = new TGraphErrors((Int_t)wmRuns_23.size(), wmRuns_23.data(), wmPolV2_23.data(), 0, wmPolErV2_23.data());
    //grV2_23->SetMarkerStyle(kOpenSquare);
    grV2_23->SetMarkerStyle(kFullSquare);
    grV2_23->SetMarkerColor(kBlue);
    grV2_23->SetLineColor(kBlue);
    grR_23 = new TGraphErrors((Int_t)wmRuns_23.size(), wmRuns_23.data(), rV1V2_23.data(), 0, rV1V2Er_23.data());
    grR_23->SetMarkerStyle(kOpenSquare);
    grR_23->SetMarkerColor(kBlue);
    grR_23->SetLineColor(kBlue);
    grDiff_23 = new TGraphErrors((Int_t)wmRuns_23.size(), wmRuns_23.data(), diffV1V2_23.data(), 0, wmPolErV1_23.data());
    grDiff_23->SetMarkerStyle(kOpenSquare);
    grDiff_23->SetMarkerColor(kBlue);
    grDiff_23->SetLineColor(kBlue);
  }
  if((Int_t)runs_24.size()>group) {
    cluster(group,runs_24,polV1_24,polErV1_24,wmRuns_24,  wmPolV1_24,wmPolErV1_24);///Ac_2parDT
    cluster(group,runs_24,polV2_24,polErV2_24,wmRuns_23_0,wmPolV2_24,wmPolErV2_24);///Ac_noDT

    for(int r=0; r<(int)wmRuns_24.size(); r++) {
      diffV1V2_24.push_back( wmPolV1_24.at(r) - wmPolV2_24.at(r) );
    }
    cout<<"size of runs_24 is: "<<runs_24.size()<<", and wmPolV1 for 2/4 is "<<wmPolV1_24.size()<<endl;
    for(Int_t r=0; r<(Int_t)wmRuns_24.size(); r++) {
      rV1V2_24.push_back(wmPolV1_24.at(r)/wmPolV2_24.at(r));
      rV1V2Er_24.push_back( TMath::Sqrt(pow(wmPolErV1_24.at(r)/wmPolV2_24.at(r),2) + pow((wmPolV1_24.at(r)/(pow(wmPolV2_24.at(r),2)))*wmPolErV2_24.at(r), 2) ));
      //rV1V2Er_24.push_back(0.0);
      fOut<<wmRuns_24[r]<<"\t"<<rV1V2_24.back()<<"\t"<<rV1V2Er_24.back()<<endl;
    }
    grV1_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), wmPolV1_24.data(), 0, wmPolErV1_24.data());
    grV1_24->SetMarkerStyle(kOpenSquare);
    grV1_24->SetMarkerColor(kGreen);
    grV1_24->SetLineColor(kGreen);
    grV2_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), wmPolV2_24.data(), 0, wmPolErV2_24.data());
    //grV2_24->SetMarkerStyle(kOpenSquare);
    grV2_24->SetMarkerStyle(kFullSquare);
    grV2_24->SetMarkerColor(kGreen);
    grV2_24->SetLineColor(kGreen);
    grR_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), rV1V2_24.data(), 0, rV1V2Er_24.data());
    grR_24->SetMarkerStyle(kOpenSquare);
    grR_24->SetMarkerColor(kGreen);
    grR_24->SetLineColor(kGreen);
    ///Difference of grouped value of individual pol% sets
    grDiff_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), diffV1V2_24.data(), 0, wmPolErV1_24.data());
    grDiff_24->SetMarkerStyle(kOpenSquare);
    grDiff_24->SetMarkerColor(kGreen);
    grDiff_24->SetLineColor(kGreen);
  }
  cout<<"size of runs_33 is: "<<runs_33.size()<<endl;
  if((Int_t)runs_33.size()>group) {
    cluster(group, runs_33,polV1_33, polErV1_33, wmRuns_33,   wmPolV1_33, wmPolErV1_33);
    cluster(group, runs_33,polV2_33, polErV2_33, wmRuns_33_0, wmPolV2_33, wmPolErV2_33);

    for(int r=0; r<(int)wmRuns_33.size(); r++) {
      diffV1V2_33.push_back( wmPolV1_33.at(r) - wmPolV2_33.at(r) );
    }
    cout<<"wmPolV1 for 3/3 is "<<wmPolV1_33.size()<<endl;
    //cout<<"last value V1, 3/3:"<<wmRuns_33.back()<<"\t"<<wmPolV1_33.back()<<"+/-"<<wmPolErV1_33.back()<<endl;
    //cout<<"last value V2, 3/3:"<<wmRuns_33.back()<<"\t"<<wmPolV2_33.back()<<"+/-"<<wmPolErV2_33.back()<<endl;
    for(Int_t r=0; r<(Int_t)wmRuns_33.size(); r++) {
      rV1V2_33.push_back(wmPolV1_33.at(r)/wmPolV2_33.at(r));
      rV1V2Er_33.push_back( TMath::Sqrt(pow(wmPolErV1_33.at(r)/wmPolV2_33.at(r),2) + pow((wmPolV1_33.at(r)/(pow(wmPolV2_33.at(r),2)))*wmPolErV2_33.at(r), 2) ));
      //rV1V2Er_33.push_back(0.0);
      fOut<<wmRuns_33[r]<<"\t"<<rV1V2_33.back()<<"\t"<<rV1V2Er_33.back()<<endl;
      //rV1V2Er_33.push_back(wmPolErV1_33.at(r));
    }
    grV1_33 = new TGraphErrors((Int_t)wmRuns_33.size(), wmRuns_33.data(), wmPolV1_33.data(), 0, wmPolErV1_33.data());
    grV1_33->SetMarkerStyle(kOpenSquare);
    grV1_33->SetMarkerColor(kRed);
    grV1_33->SetLineColor(kRed);
    grV2_33 = new TGraphErrors((Int_t)wmRuns_33.size(), wmRuns_33.data(), wmPolV2_33.data(), 0, wmPolErV2_33.data());
    //grV2_33->SetMarkerStyle(kOpenSquare);
    grV2_33->SetMarkerStyle(kFullSquare);
    grV2_33->SetMarkerColor(kRed);
    grV2_33->SetLineColor(kRed);
    grR_33 = new TGraphErrors((Int_t)wmRuns_33.size(), wmRuns_33.data(), rV1V2_33.data(), 0, rV1V2Er_33.data());
    grR_33->SetMarkerStyle(kOpenSquare);
    grR_33->SetMarkerColor(kRed);
    grR_33->SetLineColor(kRed);
    grDiff_33 = new TGraphErrors((Int_t)wmRuns_33.size(), wmRuns_33.data(), diffV1V2_33.data(), 0, wmPolErV1_33.data());
    grDiff_33->SetMarkerStyle(kOpenSquare);
    grDiff_33->SetMarkerColor(kRed);
    grDiff_33->SetLineColor(kRed);
  }
  if((Int_t)runs_34.size()>group) {
    cluster(group, runs_34,polV1_34, polErV1_34, wmRuns_34,   wmPolV1_34, wmPolErV1_34);
    cluster(group, runs_34,polV2_34, polErV2_34, wmRuns_33_0, wmPolV2_34, wmPolErV2_34);

    for(int r=0; r<(int)wmRuns_34.size(); r++) {
      diffV1V2_34.push_back( wmPolV1_34.at(r) - wmPolV2_34.at(r) );
    }
    cout<<"size of runs_34 is: "<<runs_34.size()<<", and wmPolV1 for 3/4 is "<<wmPolV1_34.size()<<endl;
    //cout<<"last value V1, 3/3:"<<wmRuns_34.back()<<"\t"<<wmPolV1_34.back()<<"+/-"<<wmPolErV1_34.back()<<endl;
    //cout<<"last value V2, 3/3:"<<wmRuns_34.back()<<"\t"<<wmPolV2_34.back()<<"+/-"<<wmPolErV2_34.back()<<endl;
    for(Int_t r=0; r<(Int_t)wmRuns_34.size(); r++) {
      rV1V2_34.push_back(wmPolV1_34.at(r)/wmPolV2_34.at(r));
      //rV1V2Er_34.push_back(wmPolErV1_34.at(r));//error of any one of the two measurements
      rV1V2Er_34.push_back( TMath::Sqrt(pow(wmPolErV1_34.at(r)/wmPolV2_34.at(r),2) + pow((wmPolV1_34.at(r)/(pow(wmPolV2_34.at(r),2)))*wmPolErV2_34.at(r), 2) ));
      //rV1V2Er_34.push_back(0.0);
      fOut<<wmRuns_34[r]<<"\t"<<rV1V2_34.back()<<"\t"<<rV1V2Er_34.back()<<endl;
    }
    grV1_34 = new TGraphErrors((Int_t)wmRuns_34.size(), wmRuns_34.data(), wmPolV1_34.data(), 0, wmPolErV1_34.data());
    grV1_34->SetMarkerStyle(kOpenSquare);
    grV1_34->SetMarkerColor(6);
    grV1_34->SetLineColor(6);
    grV2_34 = new TGraphErrors((Int_t)wmRuns_34.size(), wmRuns_34.data(), wmPolV2_34.data(), 0, wmPolErV2_34.data());
    //grV2_34->SetMarkerStyle(kOpenSquare);
    grV2_34->SetMarkerStyle(kFullSquare);
    grV2_34->SetMarkerColor(6);
    grV2_34->SetLineColor(6);
    grR_34 = new TGraphErrors((Int_t)wmRuns_34.size(), wmRuns_34.data(), rV1V2_34.data(), 0, rV1V2Er_34.data());
    grR_34->SetMarkerStyle(kOpenSquare);
    grR_34->SetMarkerColor(6);
    grR_34->SetLineColor(6);
    grDiff_34 = new TGraphErrors((Int_t)wmRuns_34.size(), wmRuns_34.data(), diffV1V2_34.data(), 0, wmPolErV1_34.data());
    grDiff_34->SetMarkerStyle(kOpenSquare);
    grDiff_34->SetMarkerColor(6);
    grDiff_34->SetLineColor(6);
  }
  fOut.close();
  cout<<"wrote all ratios into "<<file<<endl;

  if(bScint) {
    TCanvas *cScint = new TCanvas("cScint","Scintillator rates",100,100,900,400);
    cScint->SetGridx();
    std::vector<Double_t> wmRunsScint, wmScint, wmScintEr;

    //cluster(group,finalRuns, scintRate, scintRateEr, wmRunsScint, wmScint, wmScintEr);
    cluster(group,finalRuns, scintN, scintNEr, wmRunsScint, wmScint, wmScintEr);
    cout<<"size of scintRate "<<scintRate.size()<<" and wmRunsScint is "<<wmRunsScint.size()<<endl;
    //for(int i=0; i<(int)wmRunsScint.size(); i++) {
    //  cout<<i<<"\t"<<finalRuns[i*group]<<"\t"<<wmRunsScint[i]<<"\t"<<wmScint[i]<<endl;
    //}
    TGraphErrors *grScint = new TGraphErrors((Int_t)wmRunsScint.size(), wmRunsScint.data(), wmScint.data(), 0, wmScintEr.data());
    grScint->Draw("AP");
    grScint->GetXaxis()->SetTitle("Run number"); 
    grScint->GetYaxis()->SetTitle("scintillator rate");
    grScint->SetTitle();
    grScint->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cScint->SaveAs(Form("scint_G%d.png",group)); 
  }

  if(bEnergy) {
    TCanvas *c1 = new TCanvas("Energy","beam energy",200,100,900,400);
    c1->SetGridx();
    std::vector<Double_t> wmRuns, wmEnergy;
    simpleAvg(group, finalRuns, beamE, wmRuns, wmEnergy);
    ////!!temp to test
    //for(int i=0; i<(int)wmRuns.size(); i++) {
    //  cout<<i<<"\t"<<wmRuns[i]<<"\t"<<wmEnergy[i]<<endl;
    //}
    cout<<blue<<wmRuns.size()<<" runs after averaging energy"<<normal<<endl;
    TGraph *gr1 = new TGraph((Int_t)wmRuns.size(), wmRuns.data(), wmEnergy.data());
    gr1->SetMarkerStyle(kFullSquare);
    gr1->SetMarkerColor(kRed);
    gr1->Draw("AP");

    gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    gr1->GetXaxis()->SetTitle("Run number"); 
    gr1->GetYaxis()->SetTitle("Energy (MeV)"); 
    c1->SaveAs(Form("energy_G%d.png",group));
  }

  if(bBeam) {
    std::vector<Double_t> rListB, beamMax, beamMean, beamEr, beamRMS, trip, time;
    file = "infoBeamLas_Ver001.info";
    fIn.open(file);
    if(fIn.is_open()) {
      while(fIn>>d1>>d2>>d3>>d4>>d5>>d6>>d7>>d8>>d9 && fIn.good()) {
        if(std::find(finalRuns.begin(), finalRuns.end(), d1) == finalRuns.end()) continue;
        //"runnum\tbeamMax\tbeamRMS\tbeamMean\tbeamMeanEr\tnBeamTrips\tlasMax\tnLasCycles\tnEntries"
        rListB.push_back(d1);
        beamMax.push_back(d2);
        beamRMS.push_back(d3);
        beamMean.push_back(d4);
        beamEr.push_back(d5);
        trip.push_back(d6);
        time.push_back(d9/240);
      }
      fIn.close();
    }
    cout<<blue<<rListB.size()<<" runs found in "<<file<<normal<<endl;

    std::vector<Double_t> wmBeam, wmBeamMax, wmBeamMean, wmBeamEr;
    cluster(group, rListB, beamMean, beamEr, wmBeam, wmBeamMean, wmBeamEr);

    std::vector<Double_t> grListB, grBeamMax, grTrip, grTime, grRMS;
    simpleAvg(group, rListB, beamMax, grListB, grBeamMax);
    simpleAvg(group, rListB, trip, dummy, grTrip);
    simpleAvg(group, rListB, time, dummy, grTime);
    simpleAvg(group, rListB, beamRMS, dummy, grRMS);

    //!temp
    for(int i=0; i<(int) wmBeam.size(); i++) {
      printf("%.0f\t%.0f\n", wmBeam.at(i), wmBeamMean.at(i));
    }

    //TCanvas *c1 = new TCanvas("c1","beam mean",0,0,900,400);
    //c1->SetGridx();
    //TGraphErrors *grBMean = new TGraphErrors((Int_t)wmBeam.size(), wmBeam.data(), wmBeamMean.data(), 0, wmBeamEr.data());
    //grBMean->SetMarkerColor(kRed);
    //grBMean->Draw("AP");
    //grBMean->GetXaxis()->SetTitle("Run number"); 
    //grBMean->GetYaxis()->SetTitle("beam current (#mu A)");
    //grBMean->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 

    //TGraph *grBMax = new TGraph((Int_t)grListB.size(), grListB.data(), grBeamMax.data());
    //grBMax->SetMarkerStyle(kOpenSquare);
    //grBMax->SetMarkerColor(kRed);
    //grBMax->Draw("P");
    //TLegend *leg = new TLegend(0.4, 0.2, 0.6, 0.35);
    //leg->AddEntry(grBMean, "mean current", "lpe");
    //leg->AddEntry(grBMax, "max current", "p");
    //leg->Draw();
    //c1->SaveAs(Form("beamMeanRun2_G%d.png",group)); 

    //TCanvas *c2 = new TCanvas("c2","c2",0,400,900,400);
    //c2->SetGridx();
    //TGraph *gr3 = new TGraph((Int_t)grTrip.size(), grListB.data(), grTrip.data());
    //gr3->Draw("AP");
    //gr3->GetXaxis()->SetTitle("Run number"); 
    //gr3->GetYaxis()->SetTitle("number of beam trips");
    //gr3->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    //c2->SaveAs(Form("beamTripsRun2_%d.png",group));

    //TCanvas *c3 = new TCanvas("c3","c3",400,0,900,400);
    //c3->SetGridx();
    //TGraph *gr4 = new TGraph((Int_t)grListB.size(), grListB.data(), grRMS.data());
    //gr4->SetMarkerColor(kMagenta);
    //gr4->Draw("AP");
    //gr4->GetXaxis()->SetTitle("Run number"); 
    //gr4->GetYaxis()->SetTitle("beam RMS");
    //gr4->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    //c3->SaveAs(Form("beamRmsRun2_%d.png",group));

    //TCanvas *c4 = new TCanvas("c4","c4",400,400,900,400);
    //c4->SetGridx();
    //TGraph *gr5 = new TGraph((Int_t)grListB.size(), grListB.data(), grTime.data());
    //gr5->SetMarkerColor(kBlue);
    //gr5->Draw("AP");
    //gr5->GetXaxis()->SetTitle("Run number"); 
    //gr5->GetYaxis()->SetTitle("run time (s)");
    //gr5->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    //c4->SaveAs(Form("runTimeRun2_%d.png",group));
  }///if(bBeam)

  if(bIndepCorrDT) {
    TCanvas *cDT = new TCanvas("cDT","DT corr",0,400,900,400);

    std::vector<Double_t> rListDT_23, corrH1L1_23, corrH1L0_23, corrH0L1_23, corrH0L0_23;
    std::vector<Double_t> rListDT_24, corrH1L1_24, corrH1L0_24, corrH0L1_24, corrH0L0_24;
    file = Form("%s/data/newdtcorr_2by3p0.dat",pPath);
    fIn.open(file);
    if(fIn.is_open()) {
      while(fIn >>d1 >>d2 >>d3 >>d4 >>d5 && fIn.good()) {
        if(std::find(runs_23.begin(), runs_23.end(), d1) != runs_23.end()) {///populate only if the run exists in goodList
          rListDT_23.push_back(d1);
          corrH1L1_23.push_back(d2);
          corrH1L0_23.push_back(d3);
          corrH0L1_23.push_back(d4);
          corrH0L0_23.push_back(d5);
        } else if(std::find(runs_24.begin(), runs_24.end(), d1) != runs_24.end()) {///populate only if the run exists in goodList
          rListDT_24.push_back(d1);
          corrH1L1_24.push_back(d2);
          corrH1L0_24.push_back(d3);
          corrH0L1_24.push_back(d4);
          corrH0L0_24.push_back(d5);
        }
      }
      fIn.close();
    }

    cout<<"size of rListDT_23: "<<rListDT_23.size()<<endl;
    cout<<"size of rListDT_24: "<<rListDT_24.size()<<endl;

    std::vector<Double_t> rListDT_33, corrH1L1_33, corrH1L0_33, corrH0L1_33, corrH0L0_33;
    std::vector<Double_t> rListDT_34, corrH1L1_34, corrH1L0_34, corrH0L1_34, corrH0L0_34;
    file = Form("%s/data/newdtcorr_3by3p0.dat",pPath);
    fIn.open(file);
    if(fIn.is_open()) {
      cout<<"opened file "<<file<<endl;
      while(fIn >>d1 >>d2 >>d3 >>d4 >>d5 && fIn.good()) {
        //printf("%f\t%f\t%f\t%f\t%f\n",d1, d2, d3, d4, d5);
        if(std::find(runs_33.begin(), runs_33.end(), d1) != runs_33.end()) {///populate only if the run exists in goodList
          //cout<<"found run in 3/3 list: "<<d1<<endl;
          rListDT_33.push_back(d1);
          corrH1L1_33.push_back(d2);
          corrH1L0_33.push_back(d3);
          corrH0L1_33.push_back(d4);
          corrH0L0_33.push_back(d5);
        } else if(std::find(runs_34.begin(), runs_34.end(), d1) != runs_34.end()) {///populate only if the run exists in goodList
          //cout<<magenta<<"found run in 3/4 list: "<<d1<<normal<<endl;
          rListDT_34.push_back(d1);
          corrH1L1_34.push_back(d2);
          corrH1L0_34.push_back(d3);
          corrH0L1_34.push_back(d4);
          corrH0L0_34.push_back(d5);
        }
      }
      fIn.close();
    } else cout<<red<<file<<" not opened"<<normal<<endl;

    cout<<"size of rListDT_33: "<<rListDT_33.size()<<endl;
    cout<<"size of rListDT_34: "<<rListDT_34.size()<<endl;

    std::vector<Double_t> rGrH1L1_23, grDTH1L1_23;
    std::vector<Double_t> rGrH1L1_24, grDTH1L1_24;
    std::vector<Double_t> rGrH1L1_34, grDTH1L1_34;
    //std::vector<Double_t> rGrH1L0_23, grDTH1L0_23;
    simpleAvg(group, rListDT_23, corrH1L1_23, rGrH1L1_23, grDTH1L1_23);
    simpleAvg(group, rListDT_24, corrH1L1_24, rGrH1L1_24, grDTH1L1_24);
    simpleAvg(group, rListDT_34, corrH1L1_34, rGrH1L1_34, grDTH1L1_34);
    //simpleAvg(group, rListDT_23, corrH1L0_23, rGrH1L0_23, grDTH1L0_23);

    TMultiGraph *grCorrDT = new TMultiGraph();
    TGraph *gr3 = new TGraph((Int_t)rGrH1L1_23.size(), rGrH1L1_23.data(), grDTH1L1_23.data());
    gr3->SetMarkerColor(kBlue);

    TGraph *gr2 = new TGraph((Int_t)rGrH1L1_24.size(), rGrH1L1_24.data(), grDTH1L1_24.data());
    gr2->SetMarkerColor(kGreen);

    //TGraph *gr2 = new TGraph((Int_t)rListDT_23.size(), rListDT_23.data(), corrH1L0_23.data());
    //TGraph *gr1 = new TGraph((Int_t)rListDT_23.size(), rListDT_23.data(), corrH0L1_23.data());
    //TGraph *gr0 = new TGraph((Int_t)rListDT_23.size(), rListDT_23.data(), corrH0L0_23.data());
    TGraph *gr1 = new TGraph((Int_t)rGrH1L1_34.size(), rGrH1L1_34.data(), grDTH1L1_34.data());
    gr1->SetMarkerColor(kMagenta);

    grCorrDT->Add(gr3);
    grCorrDT->Add(gr2);
    grCorrDT->Add(gr1);
    //grCorrDT->Add(gr0);
    grCorrDT->Draw("AP");
    grCorrDT->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cDT->SetGridx();
    cDT->SaveAs("corrDT.png");
  }

  if(bRatio) {
    TCanvas *cV1V2 = new TCanvas("cV1V2","time dependence",0,0,900,400);
    cV1V2->cd();
    TMultiGraph *grRatio = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grRatio->Add(grR_23);
    if((Int_t)runs_33.size()>group) grRatio->Add(grR_33);
    if((Int_t)runs_24.size()>group) grRatio->Add(grR_24);
    if((Int_t)runs_34.size()>group) grRatio->Add(grR_34);

    if(bCorrDT) {
      std::vector<Double_t> rListDT_23, corrH1L1_23, corrH1L0_23, corrH0L1_23, corrH0L0_23;
      std::vector<Double_t> rListDT_24, corrH1L1_24, corrH1L0_24, corrH0L1_24, corrH0L0_24;
      file = Form("%s/data/newdtcorr_2by3p0.dat",pPath);
      fIn.open(file);
      if(fIn.is_open()) {
        while(fIn >>d1 >>d2 >>d3 >>d4 >>d5 && fIn.good()) {
          if(std::find(runs_23.begin(), runs_23.end(), d1) != runs_23.end()) {///populate only if the run exists in goodList
            rListDT_23.push_back(d1);
            corrH1L1_23.push_back(d2);
            corrH1L0_23.push_back(d3);
            corrH0L1_23.push_back(d4);
            corrH0L0_23.push_back(d5);
          } else if(std::find(runs_24.begin(), runs_24.end(), d1) != runs_24.end()) {///populate only if the run exists in goodList
            rListDT_24.push_back(d1);
            corrH1L1_24.push_back(d2);
            corrH1L0_24.push_back(d3);
            corrH0L1_24.push_back(d4);
            corrH0L0_24.push_back(d5);
          }
        }
        fIn.close();
      }
      cout<<"size of rListDT_23: "<<rListDT_23.size()<<endl;
      cout<<"size of rListDT_24: "<<rListDT_24.size()<<endl;

      std::vector<Double_t> rListDT_33, corrH1L1_33, corrH1L0_33, corrH0L1_33, corrH0L0_33;
      std::vector<Double_t> rListDT_34, corrH1L1_34, corrH1L0_34, corrH0L1_34, corrH0L0_34;
      file = Form("%s/data/newdtcorr_3by3p0.dat",pPath);
      fIn.open(file);
      if(fIn.is_open()) {
        cout<<"opened file "<<file<<endl;
        while(fIn >>d1 >>d2 >>d3 >>d4 >>d5 && fIn.good()) {
          //printf("%f\t%f\t%f\t%f\t%f\n",d1, d2, d3, d4, d5);
          if(std::find(runs_33.begin(), runs_33.end(), d1) != runs_33.end()) {///populate only if the run exists in 
            rListDT_33.push_back(d1);
            corrH1L1_33.push_back(d2);
            corrH1L0_33.push_back(d3);
            corrH0L1_33.push_back(d4);
            corrH0L0_33.push_back(d5);
          } else if(std::find(runs_34.begin(), runs_34.end(), d1) != runs_34.end()) {///populate only if the run exists 
            rListDT_34.push_back(d1);
            corrH1L1_34.push_back(d2);
            corrH1L0_34.push_back(d3);
            corrH0L1_34.push_back(d4);
            corrH0L0_34.push_back(d5);
          }
        }
        fIn.close();
      } else cout<<red<<file<<" not opened"<<normal<<endl;

      cout<<"size of rListDT_33: "<<rListDT_33.size()<<endl;
      cout<<"size of rListDT_34: "<<rListDT_34.size()<<endl;

      std::vector<Double_t> rGrH1L1_23, grDTH1L1_23;
      std::vector<Double_t> rGrH1L1_24, grDTH1L1_24;
      std::vector<Double_t> rGrH1L1_34, grDTH1L1_34;
      //std::vector<Double_t> rGrH1L0_23, grDTH1L0_23;
      simpleAvg(group, rListDT_23, corrH1L1_23, rGrH1L1_23, grDTH1L1_23);
      simpleAvg(group, rListDT_24, corrH1L1_24, rGrH1L1_24, grDTH1L1_24);
      simpleAvg(group, rListDT_34, corrH1L1_34, rGrH1L1_34, grDTH1L1_34);
      //simpleAvg(group, rListDT_23, corrH1L0_23, rGrH1L0_23, grDTH1L0_23);

      TGraph *gr3 = new TGraph((Int_t)rGrH1L1_23.size(), rGrH1L1_23.data(), grDTH1L1_23.data());
      gr3->SetMarkerColor(kBlue);

      TGraph *gr2 = new TGraph((Int_t)rGrH1L1_24.size(), rGrH1L1_24.data(), grDTH1L1_24.data());
      gr2->SetMarkerColor(kGreen);

      TGraph *gr1 = new TGraph((Int_t)rGrH1L1_34.size(), rGrH1L1_34.data(), grDTH1L1_34.data());
      gr1->SetMarkerColor(kMagenta);

      grRatio->Add(gr3);
      grRatio->Add(gr2);
      grRatio->Add(gr1);
    }

    grRatio->Draw("AP");
    grRatio->GetXaxis()->SetTitle("Run number"); 
    grRatio->GetYaxis()->SetTitle("Ratio(#frac{"+ver1+"}{"+ver2+"})");
    grRatio->SetTitle();
    grRatio->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    grRatio->SetMinimum(0.95);
    grRatio->SetMaximum(1.15);

    //grRatio->SetMinimum(1.02);
    //grRatio->SetMaximum(1.055);
    //if((Int_t)runs_24.size()>group) grR_24->Fit(lFitV1,"ERM");
    //cV1V2->Update();///forces the generation of 'stats' box
    //TPaveStats *ps1 = (TPaveStats*)grR_24->GetListOfFunctions()->FindObject("stats");
    //ps1->SetX1NDC(0.17); ps1->SetX2NDC(0.4);
    //ps1->SetTextColor(kGreen);
    ////ps1->SetFillStyle(0);//transparent

    //if((Int_t)runs_34.size()>group) grR_34->Fit(lFitV2,"ERM");
    //cV1V2->Update();///forces the generation of 'stats' box
    //TPaveStats *ps2 = (TPaveStats*)grR_34->GetListOfFunctions()->FindObject("stats");
    //ps2->SetX1NDC(0.4); ps2->SetX2NDC(0.62);
    //ps2->SetTextColor(kMagenta);
    ////ps2->SetFillStyle(0);//transparent

    //if((Int_t)runs_23.size()>group) grR_23->Fit(lFitV3,"ERM");
    //cV1V2->Update();///forces the generation of 'stats' box
    //TPaveStats *ps3 = (TPaveStats*)grR_23->GetListOfFunctions()->FindObject("stats");
    //ps3->SetX1NDC(0.63); ps3->SetX2NDC(0.85);
    //ps3->SetTextColor(kBlue);
    //ps3->SetFillStyle(0);//transparent
    //TLegend *legRa = new TLegend(0.12,0.75,0.3,0.99);
    //legRa->AddEntry(grR_23,"Ac(_23)/Sc","lpe");
    //legRa->AddEntry(grR_24,"Ac(_24)/Sc","lpe");
    //legRa->AddEntry(lFitV1,"fit _23","l");
    //if((Int_t)runs_33.size()>group) {
    //  legRa->AddEntry(grR_33,"Ac(_33)/Sc","lpe");
    //  //legRa->AddEntry(lFitV2,"fit _33","l");
    //}
    //if((int)runs_34.size()>group) {
    //  legRa->AddEntry(grR_34,"Ac(_34)/Sc","lpe");
    //}
    //legRa->Draw();
    cV1V2->Update();///forces the generation of 'stats' box
    cV1V2->SetGridx();
    cV1V2->SaveAs(Form("ratio_"+ver1+"_"+ver2+"_G%d.png",group));
    //cV1V2->SaveAs(Form("ratioV1V2_%d_%d.png",runBegin,runEnd));
  }

  if(bDiff) {
    TCanvas *cDiff = new TCanvas("cDiff","Ac Pol%",100,100,900,400);
    //cDiff->Divide(1,2);
    //cDiff->cd(1);
    //cDiff->GetPad(1)->SetGridx(1);
    //TMultiGraph *grAll = new TMultiGraph();
    //if((Int_t)runs_23.size()>group) grAll->Add(grV1_23);
    //if((Int_t)runs_33.size()>group) grAll->Add(grV1_33);
    //if((Int_t)runs_24.size()>group) grAll->Add(grV1_24);
    //if((Int_t)runs_34.size()>group) grAll->Add(grV1_34);

    ////if((Int_t)runs_23.size()>group) grAll->Add(grV2_23);
    ////if((Int_t)runs_33.size()>group) grAll->Add(grV2_33);
    ////if((Int_t)runs_24.size()>group) grAll->Add(grV2_24);
    ////if((Int_t)runs_34.size()>group) grAll->Add(grV2_34);
    //grAll->Draw("AP");
    //grAll->GetXaxis()->SetTitle("Run number"); 
    //grAll->GetYaxis()->SetTitle("polarization %");
    //grAll->SetTitle();
    //grAll->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 

    //TLegend *legAll = new TLegend(0.15,0.82,0.30,0.95);
    //if((Int_t)runs_23.size()>group) legAll->AddEntry(grV1_23,ver1,"lpe");
    //if((Int_t)runs_24.size()>group) legAll->AddEntry(grV1_24,ver1,"lpe");
    //if((Int_t)runs_33.size()>group) legAll->AddEntry(grV1_33,ver1,"lpe");
    //if((Int_t)runs_34.size()>group) legAll->AddEntry(grV1_34,ver1,"lpe");
    //if((Int_t)runs_23.size()>group) legAll->AddEntry(grV2_23,ver2,"lpe");
    //if((Int_t)runs_24.size()>group) legAll->AddEntry(grV2_24,ver2,"lpe");
    //if((Int_t)runs_33.size()>group) legAll->AddEntry(grV2_33,ver2,"lpe");
    //if((Int_t)runs_34.size()>group) legAll->AddEntry(grV2_34,ver2,"lpe");
    //legAll->Draw();

    //cDiff->cd(2);
    //cDiff->GetPad(2)->SetGridx(1);
    cDiff->SetGridx();
    TMultiGraph *grDiff = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grDiff->Add(grDiff_23);
    if((Int_t)runs_33.size()>group) grDiff->Add(grDiff_33);
    if((Int_t)runs_24.size()>group) grDiff->Add(grDiff_24);
    if((Int_t)runs_34.size()>group) grDiff->Add(grDiff_34);

    grDiff->Draw("AP");
    grDiff->GetXaxis()->SetTitle("Run number"); 
    grDiff->GetYaxis()->SetTitle("Diff ("+ver1+" - "+ver2+")");
    grDiff->SetTitle();
    grDiff->GetXaxis()->SetLimits(runBegin-5,runEnd+5);

    grDiff->Fit(lFit,"EM");
    //if((Int_t)runs_23.size()>group) grDiff_23->Fit(lFitV3,"EM");
    //if((Int_t)runs_24.size()>group) grR_24->Fit(lFitV1,"ERM");
    //gPad->Update();///forces the generation of 'stats' box
    //TPaveStats *ps1 = (TPaveStats*)grR_24->GetListOfFunctions()->FindObject("stats");
    //ps1->SetX1NDC(0.17); ps1->SetX2NDC(0.4);
    //ps1->SetTextColor(kGreen);
    //ps1->SetFillStyle(0);//transparent

    //if((Int_t)runs_34.size()>group) grR_34->Fit(lFitV2,"ERM");
    //cDiff->Update();///forces the generation of 'stats' box
    //TPaveStats *ps2 = (TPaveStats*)grR_34->GetListOfFunctions()->FindObject("stats");
    //ps2->SetX1NDC(0.4); ps2->SetX2NDC(0.62);
    //ps2->SetTextColor(kMagenta);
    ////ps2->SetFillStyle(0);//transparent

    //if((Int_t)runs_23.size()>group) grR_23->Fit(lFitV3,"ERM");
    //cDiff->Update();///forces the generation of 'stats' box
    //TPaveStats *ps3 = (TPaveStats*)grR_23->GetListOfFunctions()->FindObject("stats");
    //ps3->SetX1NDC(0.63); ps3->SetX2NDC(0.85);
    //ps3->SetTextColor(kBlue);
    cDiff->SaveAs(Form("diff_%s_%s_G%d.png",ver1.Data(),ver2.Data(),group)); 

  }

  if(bSlugRatio) {
    TCanvas *cSlug = new TCanvas("cSlug","secular ratio",400,400,900,400);
    cSlug->SetGridx();
    std::vector<Double_t> runS1In, polS1In, polS1InEr; 
    std::vector<Double_t> runS1Out, polS1Out, polS1OutEr; 
    slug(finalRuns, polS1, polS1, polErS1, runS1In, polS1In, polS1InEr, runS1Out, polS1Out, polS1OutEr, 1);
    cout<<magenta<<runS1In.size()<<" slugs with HWP IN"<<normal<<endl;
    cout<<blue<<runS1Out.size()<<" slugs with HWP Out"<<normal<<endl;

    std::vector<Double_t> runS2In, polS2In, polS2InEr; 
    std::vector<Double_t> runS2Out, polS2Out, polS2OutEr; 
    slug(finalRuns, polS2, polS2, polErS2, runS2In, polS2In, polS2InEr, runS2Out, polS2Out, polS2OutEr, 1);
    cout<<magenta<<runS2In.size()<<" slugs with HWP IN"<<normal<<endl;
    cout<<blue<<runS2Out.size()<<" slugs with HWP Out"<<normal<<endl;

    TGraphErrors *gr1, *gr2;
    TMultiGraph *gr = new TMultiGraph();
    std::vector<Double_t> rSlugIn, rSlugInEr;
    std::vector<Double_t> rSlugOut, rSlugOutEr;
    if((Int_t)polS1In.size()>group) {
      for(Int_t r=0; r<(Int_t)runS1In.size(); r++) {
        rSlugIn.push_back(polS1In.at(r)/polS2In.at(r));
        rSlugInEr.push_back( TMath::Sqrt(pow(polS1InEr.at(r)/polS2In.at(r),2) + pow((polS1In.at(r)/(pow(polS2In.at(r),2)))*polS2InEr.at(r), 2) ));
      }

      gr1 = new TGraphErrors((Int_t)runS1In.size(), runS1In.data(), rSlugIn.data(), 0, rSlugInEr.data());
      gr1->SetMarkerColor(kRed);
    }
    if((Int_t)polS1Out.size()>group) {
      for(Int_t r=0; r<(Int_t)runS1Out.size(); r++) {
        rSlugOut.push_back(polS1Out.at(r)/polS2Out.at(r));
        rSlugOutEr.push_back( TMath::Sqrt(pow(polS1OutEr.at(r)/polS2Out.at(r),2) + pow((polS1Out.at(r)/(pow(polS2Out.at(r),2)))*polS2OutEr.at(r), 2) ));
      }

      gr2 = new TGraphErrors((Int_t)runS1Out.size(), runS1Out.data(), rSlugOut.data(), 0, rSlugOutEr.data());
      gr2->SetMarkerColor(kBlue);
    }

    gr->Add(gr1);
    gr->Add(gr2);
    gr->Draw("AP");
    gr->GetXaxis()->SetTitle("Run number"); 
    gr->GetYaxis()->SetTitle("Ratio(#frac{"+ver1+"}{"+ver2+"})");
    gr->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cout<<blue<<runS1In.size()<<" runs after averaging energy"<<normal<<endl;
    cSlug->SaveAs(Form("slug_%s_%s.png",ver1.Data(),ver2.Data())); 
    TLegend *legR = new TLegend(0.8,0.78,0.90,0.95);
    legR->AddEntry(gr1,"HWP In","lpe");
    legR->AddEntry(gr2,"HWP Out","lpe");
    legR->Draw();
  }

  if(bSecular) {
    TCanvas *cSecular = new TCanvas("cSecular","secular ratio",400,400,900,400);
    cSecular->SetGridx();
    std::vector<Double_t> wmRuns, wmPolS1, wmPolErS1, wmPolS2, wmPolErS2;
    std::vector<Double_t> rSecular, rSecularEr;
    if((Int_t)polS1.size()>group) {
      cluster(group, finalRuns, polS1, polErS1, wmRuns,  wmPolS1, wmPolErS1);
      cluster(group, finalRuns, polS2, polErS2, dummy,   wmPolS2, wmPolErS2);

      for(Int_t r=0; r<(Int_t)wmRuns.size(); r++) {
        rSecular.push_back(wmPolS1.at(r)/wmPolS2.at(r));
        rSecularEr.push_back( TMath::Sqrt(pow(wmPolErS1.at(r)/wmPolS2.at(r),2) + pow((wmPolS1.at(r)/(pow(wmPolS2.at(r),2)))*wmPolErS2.at(r), 2) ));
      }
      ////!!temp to test
      for(int i=0; i<(int)wmRuns.size(); i++) {
        cout<<i<<"\t"<<wmRuns[i]<<"\t"<<rSecular[i]<<"\t"<<rSecularEr[i]<<endl;
      }
      TGraphErrors *gr1 = new TGraphErrors((Int_t)rSecular.size(), wmRuns.data(), rSecular.data(), rSecularEr.data());
      gr1->GetXaxis()->SetTitle("Run number"); 
      gr1->GetYaxis()->SetTitle("Ratio(#frac{"+ver1+"}{"+ver2+"})");
      gr1->SetMarkerColor(kBlue);
      gr1->Draw("AP");
      gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    }
    cout<<blue<<wmRuns.size()<<" runs after averaging energy"<<normal<<endl;
    cSecular->SaveAs(Form("secular_%s_%s_G%d.png",ver1.Data(),ver2.Data(),group)); 
  }

  if(bPolV1) {
    TCanvas *cPolV1 = new TCanvas("cPolV1",ver1+" Pol%",100,100,900,400);
    cPolV1->cd();
    cPolV1->SetGridx();
    TMultiGraph *grV1 = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grV1->Add(grV1_23);
    if((Int_t)runs_33.size()>group) grV1->Add(grV1_33);
    if((Int_t)runs_24.size()>group) grV1->Add(grV1_24);
    if((Int_t)runs_34.size()>group) grV1->Add(grV1_34);
    grV1->Draw("AP");
    grV1->GetXaxis()->SetTitle("Run number"); 
    grV1->GetYaxis()->SetTitle(ver1);
    grV1->SetTitle();
    grV1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    //cPolV1->SaveAs(Form(ver1+"Pol_%d_%d.png",runBegin,runEnd)); 
    cPolV1->SaveAs(Form("pol_"+ver1+"_"+ver2+"_G%d.png", group)); 
  }

  if(bPolV2) {
    TCanvas *cPolV2 = new TCanvas("cPolV2",ver2+" Pol%",200,200,900,400);
    cPolV2->cd();
    cPolV2->SetGridx();
    TMultiGraph *grV2 = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grV2->Add(grV2_23);
    if((Int_t)runs_33.size()>group) grV2->Add(grV2_33);
    if((Int_t)runs_24.size()>group) grV2->Add(grV2_24);
    if((Int_t)runs_34.size()>group) grV2->Add(grV2_34);
    grV2->Draw("AP");
    grV2->GetXaxis()->SetTitle("Run number"); 
    grV2->GetYaxis()->SetTitle(ver2);
    grV2->SetTitle();
    grV2->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cPolV2->SaveAs(Form("pol_"+ver1+"_"+ver2+"_G%d.png", group)); 
    //cPolV2->SaveAs(Form(ver2+"Pol_%d_%d.png",runBegin,runEnd)); 
  }


  ////////////////////////////////////////////////////////
  if(bRateScan) {
    TCanvas *c10 = new TCanvas("c10","rates Vs runs",300,300,900,400);
    c10->cd();
    std::vector<Double_t> rGroupRuns23, rGroupRuns33;
    std::vector<Double_t> rGroupRuns24, rGroupRuns34;
    std::vector<Double_t> groupedSc1_23,groupedSc1_24;
    std::vector<Double_t> groupedSc1_33,groupedSc1_34;
    std::vector<Double_t> bgdSubAcH1, bgdSubScH1;

    //Int_t simpleAvg(Int_t group, std::vector<Double_t> rList, std::vector<Double_t> val1, std::vector<Double_t> &groupList, std::vector<Double_t> &groupVal1_33) 
    simpleAvg(group,runs_23,p1Agg_23,rGroupRuns23,groupedSc1_23);
    simpleAvg(group,runs_24,p1Agg_24,rGroupRuns24,groupedSc1_24);

    simpleAvg(group,runs_33,p1Agg_33,rGroupRuns33,groupedSc1_33);
    simpleAvg(group,runs_34,p1Agg_34,rGroupRuns34,groupedSc1_34);

    cout<<blue<<"size of rGroupRuns23 : "<<rGroupRuns23.size()<<endl;
    cout<<blue<<"size of rGroupRuns33 : "<<rGroupRuns33.size()<<endl;

    //for(Int_t i=0; i<(Int_t)rGroupRuns33.size(); i++) {
    //  rRateV1V2_33.push_back((groupedAc1_33.at(i)-groupedAc2_33.at(i))/(groupedSc1_33.at(i)-groupedSc2_33.at(i)));
    //}

    //for(Int_t i=0; i<(Int_t)rGroupRuns23.size(); i++) {
    //  rRateV1V2_23.push_back((groupedAc1_23.at(i)-groupedAc2_23.at(i))/(groupedSc1_23.at(i)-groupedSc2_23.at(i)));
    //}
    //TGraph *grR23 = new TGraph((Int_t)rGroupRuns23.size(), rGroupRuns23.data(), rRateV1V2_23.data());
    TGraph *grR23 = new TGraph((Int_t)rGroupRuns23.size(), rGroupRuns23.data(), groupedSc1_23.data());
    grR23->SetMarkerStyle(kOpenCircle);
    grR23->SetMarkerColor(kBlue);

    TGraph *grR24 = new TGraph((Int_t)rGroupRuns24.size(), rGroupRuns24.data(), groupedSc1_24.data());
    grR24->SetMarkerStyle(kOpenCircle);
    grR24->SetMarkerColor(kGreen);
    //grR23_2->SetMarkerColor(6);

    //TGraph *grR33 = new TGraph((Int_t)rGroupRuns33.size(), rGroupRuns33.data(), rRateV1V2_33.data());
    TGraph *grR33 = new TGraph((Int_t)rGroupRuns33.size(), rGroupRuns33.data(), groupedSc1_33.data());
    grR33->SetMarkerStyle(kOpenCircle);
    grR33->SetMarkerColor(kRed);

    TGraph *grR34 = new TGraph((Int_t)rGroupRuns34.size(), rGroupRuns34.data(), groupedSc1_34.data());
    grR34->SetMarkerStyle(kOpenCircle);
    grR34->SetMarkerColor(6);
    //grR33_2->SetMarkerColor(kGreen);

    TMultiGraph *grRate = new TMultiGraph();
    grRate->Add(grR23);
    grRate->Add(grR33);
    grRate->Add(grR24);
    grRate->Add(grR34);
    ////grR->SetMarkerStyle(kSquare);
    grRate->Draw("AP");
    grRate->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    grRate->GetXaxis()->SetTitle("Run number"); 
    //grRate->GetYaxis()->SetTitle("Ac/Sc ratio of aggregate rate");
    grRate->GetYaxis()->SetTitle("Sc aggregate rate");
    c10->SetGridx(1);
    TLegend *legR = new TLegend(0.8,0.78,0.90,0.95);
    legR->AddEntry(grR23,"2/3 trig","lp");
    legR->AddEntry(grR24,"2/4 trig","lp");
    legR->AddEntry(grR33,"3/3 trig","lp");
    legR->AddEntry(grR34,"3/4 trig","lp");
    legR->Draw();
    //c10->SaveAs("aggregateRatioV1V2.png");
    c10->SaveAs(Form("aggRateSc_%d_%d.png",runBegin,runEnd));
  }
  return 1;
}
