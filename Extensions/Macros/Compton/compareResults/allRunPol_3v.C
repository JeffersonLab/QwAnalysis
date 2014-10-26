#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "cluster.C"
#include "simpleAvg.C"

const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
//const Int_t runBegin = 24000, runEnd = 24500;
//const Int_t runBegin = 24250, runEnd = 24450;

Int_t allRunPol_3v() {
  gROOT->SetStyle("publication");
  Bool_t bRatio = 1;
  Bool_t bDiff = 0;
  Bool_t bRateScan = 0;
  Bool_t bPolSc = 0, bPolAc =0;
  Bool_t bCorrDT = 0;
  ifstream fIn;
  ofstream fCheck;
  TString ver1, ver2, file;

  std::vector<Double_t> finalRuns,polCol1, polCol1Er, polCol2, polCol2Er, polCol3, polCol3Er, acTr, p4On, p1ScH1BgdSub;
  std::vector<Double_t> polS1, polErS1, polS2, polErS2, polS3, polErS3; ///polarization in various colums
  Double_t d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
  TString s1, s2, s3, s4, s5, s6, s7, s8, s9, s0;
  TF1 *lFitV1 = new TF1("lFitV1", "pol0",runBegin,runEnd);
  TF1 *lFitV2 = new TF1("lFitV2", "pol0",runBegin,runEnd);
  lFitV1->SetLineColor(kBlack);
  lFitV2->SetLineColor(kBlue);
  fCheck.open("runsExcluded.txt");

  ///read in the good runs' list
  //Double_t tRun;
  //std::vector<Double_t> goodRuns;
  //fIn.open("/w/hallc/compton/users/narayan/my_scratch/data/goodList_longPol.txt");
  //if(fIn.is_open()) {
  //  while(fIn>>tRun && fIn.good()) {
  //    goodRuns.push_back(tRun);
  //  }
  //  fIn.close();
  //} else cout<<red<<"couldn't open the list of good runs"<<normal<<endl;
  //cout<<blue<<goodRuns.size()<<" runs found in the goodList"<<endl;

  //fIn.open("allRun2.info");
  //fIn.open("combFiles_30Sep14.info");
  file = "combFiles_Ver001.info";
  fIn.open(file);
  if(fIn.is_open()) {
    fIn>>s0>>s1>>s2>>s3>>s4>>s5>>s6>>s7>>s8>>s9;
    while(fIn>>d1>>d2>>d3>>d4>>d5>>d6>>d7>>d8>>d9>>d10 && fIn.good()) {
      if(d1>=runBegin && d1<=runEnd) {///limiting run range
        if(d2>80.0 && d4>80 && d6>80) {
          if(d3<1.5 && d5<1.5 && d7<1.5) {///excluding polEr > 1.5% in Ac_noDT
            if(d3>0.01 && d5>0.01 && d7>0.01) {
              finalRuns.push_back(d1);//run#
              polCol1.push_back(d2);//polAc_noDT
              polCol1Er.push_back(d3);//polErAc_noDT
              polCol2.push_back(d4);//polAc_2parDT 
              polCol2Er.push_back(d5);//polErAc_2parDT
              polCol3.push_back(d6);//polSc_noDT
              polCol3Er.push_back(d7);//polErSc_noDT
              acTr.push_back(d8);//acTrig
              p4On.push_back(d9);//plane 4 rates
              p1ScH1BgdSub.push_back(d10);//plane1 Bgd Sub Scalar aggregate
            } else fCheck<<d1<<"\t"<<d2<<"\t"<<d3<<"\t"<<d4<<"\t"<<d5<<"\t"<<d6<<"\t"<<d7<<endl;
          } else fCheck<<d1<<"\t"<<d2<<"\t"<<d3<<"\t"<<d4<<"\t"<<d5<<"\t"<<d6<<"\t"<<d7<<endl;
        } else fCheck<<d1<<"\t"<<d2<<"\t"<<d3<<"\t"<<d4<<"\t"<<d5<<"\t"<<d6<<"\t"<<d7<<endl;
      }
    }
    fIn.close();
    fCheck.close();
  } else cout<<red<<"couldn't open the list of good runs"<<normal<<endl;
  cout<<blue<<finalRuns.size()<<" runs found in the final List"<<endl;
  cout<<"runsExcluded.txt has the list of runs that got excluded"<<endl;

  ///Only one of these can be turned On
  //pick and choose what needs to be compared from below
  Bool_t bS3S2 =0;
  Bool_t bS1S2 =0;
  Bool_t bS1S3 =1;
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
  Int_t group = 30;
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
    } else if((acTr[i]==2) && p4On[i]==0) {
      runs_23.push_back(finalRuns[i]);
      p1Agg_23.push_back(p1ScH1BgdSub[i]);

      polV1_23.push_back(polS1[i]);
      polErV1_23.push_back(polErS1[i]);

      polV2_23.push_back(polS2[i]);
      polErV2_23.push_back(polErS2[i]);
    } else if((acTr[i]==3) && p4On[i]>1000) {
      runs_34.push_back(finalRuns[i]);
      p1Agg_34.push_back(p1ScH1BgdSub[i]);

      polV1_34.push_back(polS1[i]);
      polErV1_34.push_back(polErS1[i]);

      polV2_34.push_back(polS2[i]);
      polErV2_34.push_back(polErS2[i]);
    } else if((acTr[i]==3) && p4On[i]==0) {
      runs_33.push_back(finalRuns[i]);
      p1Agg_33.push_back(p1ScH1BgdSub[i]);

      polV1_33.push_back(polS1[i]);
      polErV1_33.push_back(polErS1[i]);

      polV2_33.push_back(polS2[i]);
      polErV2_33.push_back(polErS2[i]);
    }
  }

  std::vector<Double_t> rV1V2_23, rV1V2_33, rV1V2_24, rV1V2_34;
  std::vector<Double_t> rV1V2Er_24, rV1V2Er_34, rV1V2Er_23, rV1V2Er_33;
  std::vector<Double_t> diffV1V2_23, diffV1V2_24, diffV1V2_33, diffV1V2_34;
  TGraphErrors *grV1_24, *grV1_23, *grV1_33, *grV1_34;
  TGraphErrors *grV2_24, *grV2_23, *grV2_33, *grV2_34;
  TGraphErrors *grR_24, *grR_23, *grR_33, *grR_34;
  TGraphErrors *grDiff_24, *grDiff_23, *grDiff_33, *grDiff_34;

  if((Int_t)runs_23.size()>group) {
    cluster(group,runs_23,polV1_23,polErV1_23,wmRuns_23,  wmPolV1_23,wmPolErV1_23);
    cluster(group,runs_23,polV2_23,polErV2_23,wmRuns_23_0,wmPolV2_23,wmPolErV2_23);

    for(int r=0; r<(int)wmRuns_23.size(); r++) {
      diffV1V2_23.push_back( wmPolV1_23.at(r) - wmPolV2_23.at(r) );
    }
    cout<<"size of runs_23 is: "<<runs_23.size()<<", and wmPolV1 for 2/3 is "<<wmPolV1_23.size()<<endl;
    for(Int_t r=0; r<(Int_t)wmRuns_23.size(); r++) {
      rV1V2_23.push_back(wmPolV1_23.at(r)/wmPolV2_23.at(r));
      rV1V2Er_23.push_back( TMath::Sqrt(pow(wmPolErV1_23.at(r)/wmPolV2_23.at(r),2) + pow((wmPolV1_23.at(r)/(pow(wmPolV2_23.at(r),2)))*wmPolErV2_23.at(r), 2) ));
      //rV1V2Er_23.push_back(wmPolErV1_23.at(r));
    }
    grV1_23 = new TGraphErrors((Int_t)wmRuns_23.size(), wmRuns_23.data(), wmPolV1_23.data(), 0, wmPolErV1_23.data());
    grV1_23->SetMarkerStyle(kFullSquare);
    grV1_23->SetMarkerColor(kBlue);
    grV1_23->SetLineColor(kBlue);
    grV2_23 = new TGraphErrors((Int_t)wmRuns_23.size(), wmRuns_23.data(), wmPolV2_23.data(), 0, wmPolErV2_23.data());
    grV2_23->SetMarkerStyle(kOpenSquare);
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
      //rV1V2Er_24.push_back(wmPolErV1_24.at(r));
    }
    grV1_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), wmPolV1_24.data(), 0, wmPolErV1_24.data());
    grV1_24->SetMarkerStyle(kFullSquare);
    grV1_24->SetMarkerColor(kGreen);
    grV1_24->SetLineColor(kGreen);
    grV2_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), wmPolV2_24.data(), 0, wmPolErV2_24.data());
    grV2_24->SetMarkerStyle(kFullSquare);
    grV2_24->SetMarkerColor(kGreen);
    grV2_24->SetLineColor(kGreen);
    grR_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), rV1V2_24.data(), 0, rV1V2Er_24.data());
    grR_24->SetMarkerStyle(kFullSquare);
    grR_24->SetMarkerColor(kGreen);
    grR_24->SetLineColor(kGreen);
    ///Difference of grouped value of individual pol% sets
    grDiff_24 = new TGraphErrors((Int_t)wmRuns_24.size(), wmRuns_24.data(), diffV1V2_24.data(), 0, wmPolErV1_24.data());
    grDiff_24->SetMarkerStyle(kFullSquare);
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
      //rV1V2Er_33.push_back(wmPolErV1_33.at(r));
    }
    grV1_33 = new TGraphErrors((Int_t)wmRuns_33.size(), wmRuns_33.data(), wmPolV1_33.data(), 0, wmPolErV1_33.data());
    grV1_33->SetMarkerStyle(kFullSquare);
    grV1_33->SetMarkerColor(kRed);
    grV1_33->SetLineColor(kRed);
    grV2_33 = new TGraphErrors((Int_t)wmRuns_33.size(), wmRuns_33.data(), wmPolV2_33.data(), 0, wmPolErV2_33.data());
    grV2_33->SetMarkerStyle(kOpenSquare);
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
    }
    grV1_34 = new TGraphErrors((Int_t)wmRuns_34.size(), wmRuns_34.data(), wmPolV1_34.data(), 0, wmPolErV1_34.data());
    grV1_34->SetMarkerStyle(kFullSquare);
    grV1_34->SetMarkerColor(6);
    grV1_34->SetLineColor(6);
    grV2_34 = new TGraphErrors((Int_t)wmRuns_34.size(), wmRuns_34.data(), wmPolV2_34.data(), 0, wmPolErV2_34.data());
    grV2_34->SetMarkerStyle(kOpenSquare);
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

  if(bCorrDT) {
    TCanvas *cDT = new TCanvas("cDT","DT corr",0,400,800,400);

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

    ////!temp
    //for(int i=0; i<(int)runs_33.size(); i++) {
    //  printf("%.0f\n",runs_33.at(i));
    //}

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
    //cout<<rGrH1L1_23.size()<<endl;
    //cout<<rGrH1L1_24.size()<<endl;
    //cout<<rGrH1L1_34.size()<<endl;
    //cout<<rListDT_33.size()<<endl;

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
    TCanvas *cV1V2 = new TCanvas("cV1V2","time dependence",0,0,800,400);
    cV1V2->cd();
    TMultiGraph *grRatio = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grRatio->Add(grR_23);
    if((Int_t)runs_33.size()>group) grRatio->Add(grR_33);
    if((Int_t)runs_24.size()>group) grRatio->Add(grR_24);
    if((Int_t)runs_34.size()>group) grRatio->Add(grR_34);

    grRatio->Draw("AP");
    grRatio->GetXaxis()->SetTitle("Run number"); 
    grRatio->GetYaxis()->SetTitle("pol Ratio(#frac{"+ver1+"}{"+ver2+"})");
    grRatio->SetTitle();
    grRatio->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    //grRatio->SetMinimum(1.02);
    //grRatio->SetMaximum(1.055);

    //if((Int_t)runs_23.size()>group) grR_23->Fit(lFitV1,"EM");

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
    cV1V2->SetGridx();
    cV1V2->SaveAs(Form("polRatio_"+ver1+"_"+ver2+"_G%d.png",group));
    //cV1V2->SaveAs(Form("ratioV1V2_%d_%d.png",runBegin,runEnd));
  }

  if(bDiff) {
    TCanvas *cDiff = new TCanvas("cDiff","Ac Pol%",100,100,800,800);
    cDiff->Divide(1,2);
    cDiff->cd(1);
    cDiff->GetPad(1)->SetGridx(1);
    //TLegend *legAll = new TLegend(0.12,0.82,0.38,0.95);
    TLegend *legAll = new TLegend(0.12,0.72,0.42,0.95);
    TMultiGraph *grAll = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grAll->Add(grV1_23);
    if((Int_t)runs_33.size()>group) grAll->Add(grV1_33);
    if((Int_t)runs_24.size()>group) grAll->Add(grV1_24);
    if((Int_t)runs_34.size()>group) grAll->Add(grV1_34);

    //if((Int_t)runs_23.size()>group) grAll->Add(grV2_23);
    //if((Int_t)runs_33.size()>group) grAll->Add(grV2_33);
    //if((Int_t)runs_24.size()>group) grAll->Add(grV2_24);
    //if((Int_t)runs_34.size()>group) grAll->Add(grV2_34);
    grAll->Draw("AP");
    grAll->GetXaxis()->SetTitle("Run number"); 
    grAll->GetYaxis()->SetTitle("polarization %");
    grAll->SetTitle();
    grAll->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    legAll->AddEntry(grV1_23,ver1,"lpe");
    legAll->AddEntry(grV1_24,ver1,"lpe");
    legAll->AddEntry(grV1_33,ver1,"lpe");
    legAll->AddEntry(grV1_34,ver1,"lpe");
    //legAll->AddEntry(grV2_33,ver2,"lpe");
    //legAll->AddEntry(grV2_24,ver2,"lpe");
    //legAll->AddEntry(grV2_23,ver2,"lpe");
    //legAll->AddEntry(grV2_34,ver2,"lpe");
    legAll->Draw();

    cDiff->cd(2);
    cDiff->GetPad(2)->SetGridx(1);
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

    //if((Int_t)runs_23.size()>group) grDiff_23->Fit(lFitV2,"E");
    cDiff->SaveAs(Form("diff_%s_%s_%d_%d.png",ver1.Data(),ver2.Data(),runBegin,runEnd)); 

  }

  if(bPolAc) {
    TCanvas *cAcPol = new TCanvas("cAcPol","Ac Pol%",100,100,800,400);
    cAcPol->cd();
    cAcPol->SetGridx();
    TMultiGraph *grV1 = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grV1->Add(grV1_23);
    if((Int_t)runs_33.size()>group) grV1->Add(grV1_33);
    if((Int_t)runs_24.size()>group) grV1->Add(grV1_24);
    if((Int_t)runs_34.size()>group) grV1->Add(grV1_34);
    grV1->Draw("AP");
    grV1->GetXaxis()->SetTitle("Run number"); 
    grV1->GetYaxis()->SetTitle("pol Ac");
    grV1->SetTitle();
    grV1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cAcPol->SaveAs(Form("PolAc_%d_%d.png",runBegin,runEnd)); 
  }

  if(bPolSc) {
    TCanvas *cScPol = new TCanvas("cScPol","Sc Pol%",200,200,800,400);
    cScPol->cd();
    cScPol->SetGridx();
    TMultiGraph *grV2 = new TMultiGraph();
    if((Int_t)runs_23.size()>group) grV2->Add(grV2_23);
    if((Int_t)runs_33.size()>group) grV2->Add(grV2_33);
    if((Int_t)runs_24.size()>group) grV2->Add(grV2_24);
    if((Int_t)runs_34.size()>group) grV2->Add(grV2_34);
    grV2->Draw("AP");
    grV2->GetXaxis()->SetTitle("Run number"); 
    grV2->GetYaxis()->SetTitle("pol Sc");
    grV2->SetTitle();
    grV2->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cScPol->SaveAs(Form("PolSc_%d_%d.png",runBegin,runEnd)); 
  }


  ////////////////////////////////////////////////////////
  if(bRateScan) {
    TCanvas *c10 = new TCanvas("c10","rates Vs runs",300,300,800,400);
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
