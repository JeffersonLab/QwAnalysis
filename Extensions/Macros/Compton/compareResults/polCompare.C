#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
///lets focus on the region between 23220 to 23530;
//const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
const Int_t runBegin = 22659, runEnd = 25546;
Int_t polCompare(Int_t run1=runBegin, Int_t run2=runEnd)
{
  gROOT->SetStyle("publication");
  gStyle->SetOptFit(111);
  //gStyle->SetOptStat(0);
  Bool_t debug=1,debug1=0;
  Bool_t kPrintV1=0, kPrintV2=0; 
  Bool_t bRateScan = 0, bAbsPol=0,bPolCompare=0, bPolErCompare=0, bCECompare=0;
  Bool_t bScAc = 1, bDAQ = 1;
  Bool_t bChiSqrCompare=0,bChiSqrHist=0;
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  const Int_t numbRuns = runEnd - runBegin;//a non-variable is needed to set the size of an array length
  ifstream inFileRunsV1,inFileV2,inFileV1, fDAQ;
  ofstream outFileV1;
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www";///newest results
  const TString destDir="/w/hallc/compton/users/narayan/svn/Compton/compareResults/";
  //TString ver1 = "1parDT";//"cleanLC";///run from a folder named cleanOnlyLC
  TString ver1 = "Ac";//"cleanLC";///run from a folder named cleanOnlyLC
  //TString ver1 = "noDT";//"cleanLC";///run from a folder named cleanOnlyLC
  //TString fileV1 = destDir + Form("pol_onlyCleanLC.info");
  //TString fileV1 = destDir + Form("pol_1parDT.info");
  TString fileV1 = destDir + Form("polAc_noDTRun2.info");
  inFileV1.open(fileV1.Data());

  //TString ver2 = "2parDT";//"allLC";///currently in SVN revision
  TString ver2 = "Sc";//"allLC";///currently in SVN revision
  //TString fileV2 = destDir + Form("pol_www.info");
  //TString fileV2 = destDir + Form("pol_2parDT.info");
  TString fileV2 = destDir + Form("polSc_noDTRun2.info");
  inFileV2.open(fileV2.Data());

  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10,dum11;
  Int_t runRange = run2-run1+1;
  TH1D *hPolV1 = new TH1D("polV1", ver1, runRange, 80, 95);
  TH1D *hPolV2 = new TH1D("polV2", ver2, runRange, 80, 95);
  TH1D *hPolV1Er = new TH1D("polEr "+ver1,"pol error "+ver1,runRange,0,0.1);
  TH1D *hPolV2Er = new TH1D("polEr "+ver2,"pol error "+ver2,runRange,0,0.1);
  TH1D *hPolDiff = new TH1D("diff. in pol%","difference in polarization between the two analysis",runRange,0,0.1);
  TH1D *hCEDiff = new TH1D("diff. in CE", "difference in CE between the two analysis", runRange, 0, 0.1);
  TH1D *hChiSqV1 = new TH1D("chiSqr "+ver1, "chi sqr "+ver1, runRange, 0, 0.1);
  TH1D *hChiSqV2 = new TH1D("chiSqr "+ver2, "chi sqr "+ver2, runRange, 0, 0.1);
  TF1 *linearFit = new TF1("linearFit", "pol0",runBegin,runEnd);
  TF1 *linearFitV1 = new TF1("linearFitV1", "pol0",runBegin,runEnd);
  TF1 *linearFitV2 = new TF1("linearFitV2", "pol0",runBegin,runEnd);

  //hPolV2->SetBit(TH1::kCanRebin);
  //hPolV1->SetBit(TH1::kCanRebin);
  hPolV1Er->SetBit(TH1::kCanRebin);
  hPolV2Er->SetBit(TH1::kCanRebin);
  hPolDiff->SetBit(TH1::kCanRebin);
  hCEDiff->SetBit(TH1::kCanRebin);
  hChiSqV1->SetBit(TH1::kCanRebin);
  hChiSqV2->SetBit(TH1::kCanRebin);

  std::vector< Double_t> runListV1,runListV2,polV1,polV2,polErV1,polErV2,comptEdgeV1,comptEdgeV2,chiSqV1,chiSqV2,edgeErV2,edgeErV1;
  std::vector<Int_t> goodCycV1,goodCycV2;
  ///write the lasCyc based polarization into one central file
  Int_t runnum,plane,goodCyc;
  Double_t comptEdge,comptEdgeEr,effStrip,effStripEr;
  const Float_t loLimit=0.1;
  const Float_t hiLimit=8.5;
  if (inFileV1.is_open()) { 
    while(inFileV1>>runnum>>pol>>polEr>>chiSq>>NDF>>comptEdge>>comptEdgeEr>>effStrip>>effStripEr>>plane>>goodCyc && inFileV1.good()) {
      //cout<<runnum<<"\t"<<pol<<"\t"<<polEr<<"\t"<<chiSq<<"\t"<<comptEdge<<"\t"<<comptEdgeEr<<"\t"<<effStrip<<"\t"<<effStripEr<<"\t"<<plane<<"\t"<<goodCyc<<endl;
      kPrintV1 = (polEr<3.0 && polEr>loLimit && comptEdge>30.0 && comptEdgeEr<0.8);// && chiSq/NDF<5.0);
      if (kPrintV1) {
        runListV1.push_back((Double_t)runnum);
        polV1.push_back(fabs(pol));
        polErV1.push_back(polEr);
        comptEdgeV1.push_back(comptEdge);
        edgeErV1.push_back(comptEdgeEr);
        chiSqV1.push_back(chiSq/NDF);
        goodCycV1.push_back(goodCyc);
        //chiSqV1.push_back(chiSq);
      } else if(debug1) cout<<"for runum:"<<runnum<<", pol:"<<pol<<" +/- "<<polEr<<", CE:"<<comptEdge<<endl;
    }
    inFileV1.close();
  } else cout<<red<<"*** Could not open file with V1 pol info"<<normal<<endl; 
  if(debug)cout<<red<<"found valid "<<runListV1.size()<<" analyzed runs in "+ver1<<normal<<endl;

  //ofstream runsV2;
  //runsV2.open("runsByVladas.txt");
  Double_t zero[numbRuns]={0};
  Double_t tempRun,tempPol,tempPolEr,tempChiSq,tempCE,tempCEEr;
  Int_t tempNDF;
  Double_t dummy2,dummy3,dummy4,dummy5;
  std::vector<Double_t > NDF_V2;
  if(inFileV2.is_open()) { //this is available only for plane 1
    while(inFileV2>>tempRun>>tempPol>>tempPolEr>>tempChiSq>>tempNDF>>tempCE>>tempCEEr>>dummy2>>dummy3>>dummy4>>dummy5 && inFileV2.good()) {
      kPrintV2 = (tempPolEr<3.0 && tempPolEr>loLimit && tempCE>30 && tempCEEr<0.8);
      if (kPrintV2) {
        runListV2.push_back(tempRun);
        polV2.push_back(fabs(tempPol));
        polErV2.push_back(tempPolEr);
        comptEdgeV2.push_back(tempCE);
        edgeErV2.push_back(tempCEEr);
        NDF_V2.push_back(tempNDF);///if reading from file generated by me
        chiSqV2.push_back(tempChiSq/tempNDF);///if reading from file generated by me
        goodCycV2.push_back(dummy5);
      } else if(debug1) cout<<blue<<"in V2, runnum:"<<tempRun<<", polV2:"<<tempPol<<"+/-"<<tempPolEr<<normal<<endl;
      //cout<<runListV2[c2]<<"\t"<<polV2[c2]<<"\t"<<polErV2[c2]<<"\t"<<comptEdgeV2[c2]<<"\t"<<edgeErV2[c2]<<endl;
    }
    inFileV2.close();
  } else cout<<"*** Alert: could not open runlet based pol file"<<endl;
  if(debug) cout<<red<<"found valid "<<runListV2.size()<<" analyzed runs in "<<ver2<<normal<<endl;
  //runsV2.close();

  std::vector<Double_t> runDAQ, acTrigRun2;
  char *firmW;
  if(bDAQ) {
    fDAQ.open(destDir + "run2DAQ.info");
    if (fDAQ.is_open()) { 
      while(fDAQ>>runnum>>acTrig>>evTrig>>minWidth>>firmW>>pwtl1>>pwtl2>>holdOff>>pipelineDelay && fDAQ.good()) {
        runDAQ.push_back((Double_t)runnum);
        acTrigRun2.push_back(acTrig);
        if(debug1) cout<<"for runum:"<<runDAQ.back()<<", acTrig:"<<acTrigRun2.back()<<endl;
      }
      fDAQ.close();
    } else cout<<red<<"*** Could not open file with DAQ info"<<normal<<endl; 
    if(debug)cout<<red<<"found valid "<<runDAQ.size()<<" entries in DAQ file"<<normal<<endl;
  }

  std::vector<Double_t> commonRuns,polDiff,CEDiff,polDiffEr,CEDiffEr,zero_1,lostCyc;
  std::vector<Double_t> newPolV1, newPolV2, newPolErV1, newPolErV2;
  //std::vector<Double_t> PolV1_3by3, PolV2_3by3, PolErV1_3by3, PolErV2_3by3;
  //std::vector<Double_t> PolV1_2by3, PolV2_2by3, PolErV1_2by3, PolErV2_2by3;
  std::vector<Double_t> newEdgeV1, newEdgeV2, newEdgeErV1, newEdgeErV2;
  std::vector<Double_t> badRuns, cutPolV2, cutPolErV2, cutPolV1, cutPolErV1;
  std::vector<Double_t> runs2by3, polV22by3, polErV22by3, polV12by3, polErV12by3;
  std::vector<Double_t> newRunR_33, newRate1_33, newRate2_33;
  std::vector<Double_t> newRunR_23, newRate1_23, newRate2_23;
  //Double_t polErV2Sqr_inv,wmNrPolV2,wmDrPolV2;
  //Double_t polErV1Sqr_inv,wmNrPolV1,wmDrPolV1;
  //Double_t wmPolV2,wmPolErV2, wmPolV1,wmPolErV1;

  //ofstream fileCheck;
  //fileCheck.open("fileCheck.txt");
  //fileCheck.open("bestRunsPolV2.txt");
  /////the bestRunsPolV2.txt was created as a filtered list of runs that qualifies some cuts in V2 data.
  /////runs2by3.txt has a list of runs with 2/3 accum trigger in this run range
  std::vector<Double_t> rates1, rates2, rates3, rates4, runR;
  if(bRateScan) {
    Double_t rate1, rate2, rate3, rate4, runn;
    ifstream fRates;
    //goodList.open("bestRunsPolV2.txt");
    fRates.open("/w/hallc/compton/users/narayan/my_scratch/data/aggregate_ratescan.dat");
    if(fRates.is_open()) {
      while(fRates>>runn>>rate1>>rate2>>rate3>>rate4 && fRates.good()) {
        runR.push_back(runn), rates1.push_back(rate1), rates2.push_back(rate2), rates3.push_back(rate3), rates4.push_back(rate4);
        if(debug1) cout<<runR.back()<<"  "<<rates4.back()<<endl;
      }
      fRates.close();
    } else cout<<red<<"could not open list of rates"<<normal<<endl;
    cout<<blue<<"no.of runs in rates list: "<<rates1.size()<<normal<<endl;
  }
  //Int_t dum;
  //std::vector<Int_t> goodRuns;
  //ifstream goodList;
  ////goodList.open("bestRunsPolV2.txt");
  //goodList.open("runs2by3.txt");
  //if(goodList.is_open()) {
  //  while(goodList>>dum && goodList.good()) {
  //    goodRuns.push_back(dum);
  //    if(debug1) cout<<goodRuns.back()<<endl;
  //  }
  //  goodList.close();
  //} else cout<<red<<"could not open list of good runs"<<normal<<endl;
  //cout<<blue<<"no.of runs in good list: "<<goodRuns.size()<<normal<<endl;

  Int_t c1=0, c2 = 0, c3=0;
  if(debug) cout<<blue<<"assigning values to runs that are common between two analysis"<<normal<<endl;
  for(Int_t r2=0; r2<(Int_t)runListV2.size(); r2++) {
    for(Int_t r1=0; r1<(Int_t)runListV1.size(); r1++) {
      if(runListV2[r2] == runListV1[r1]) {
        if( (polErV2[r2]>=1.5) || polErV1[r1]>=1.5 || polV1[r1]<82.0 || polV2[r2]<82.0) {
          if(debug1) cout<<blue<<"polErV2:"<<polErV2[r2]<<" for run "<<runListV2[r2]<<normal<<endl;
          badRuns.push_back(runListV2[r2]);
          cutPolV2.push_back(polV2[r2]);
          cutPolV1.push_back(polV1[r1]);
          cutPolErV2.push_back(polErV2[r2]);
          cutPolErV1.push_back(polErV1[r1]);
          //} else if(polV2[r2]-polV1[r1]<=loLimit) {
          //  c3++;
      } else if(polV2[r2]-polV1[r1]>hiLimit) {
        c1++;
        //cout<<"\nfor run: "<<runListV1[r1]<<",polDiff is:"<<polV2[r2]-polV1[r1]<<endl;
        //cout<<"ver1 has: "<<runListV1[r1]<<"\t"<<polV1[r1]<<"\t"<<polErV1[r1] <<"\t"<<comptEdgeV1[r1]<<"\t"<<edgeErV1[r1]<<"\t"<<chiSqV1[r1]<<"\t"<<goodCycV1[r1]<<endl;
        //cout<<"ver2 has: "<<runListV2[r2]<<"\t"<<polV2[r2]<<"\t"<<polErV2[r2] <<"\t"<<comptEdgeV2[r2]<<"\t"<<edgeErV2[r2]<<"\t"<<chiSqV2[r2]<<"\t"<<goodCycV2[r2]<<endl;
      } else if(polErV1[r1]<loLimit || polErV2[r2]<loLimit || edgeErV1[r1]< 0.05 || edgeErV2[r2]<0.05) {
        //cout<<runListV1[r1] <<"\t"<<polV1[r1]<<"+/-"<<polErV1[r1]<<"\t"<<polV2[r2]<<"+/-"<< polErV2[r2]<<"\t"<< edgeErV1[r1]<<"\t"<< edgeErV2[r2]<<endl;
        c2++;
        //} else if(std::find(goodRuns.begin(),goodRuns.end(),runListV1[r1])==goodRuns.end()) {
        //cout<<"run "<<runListV2[r2]<<" is not in 2/3 list"<<endl;
        //} else if(std::find(goodRuns.begin(),goodRuns.end(),runListV1[r1])!=goodRuns.end()) { ///is the run included? 
        //  //cout<<"run "<<runListV2[r2]<<" is in 2/3 list"<<endl;
        //  if(bRateScan) {
        //    for(Int_t r3=0; r3<(Int_t)runR.size(); r3++) {
        //      if(runR[r3]==runListV2[r2]) {
        //        //printf("%d\t%f\t%f\n",r3,runR[r3],runListV2[r2]);
        //        newRunR_23.push_back(runR[r3]);
        //        newRate1_23.push_back(rates1[r3]);
        //        newRate2_23.push_back(rates2[r3]);
        //        continue;
        //      }
        //    }
        //  }//if(bRateScan) 
        //  continue;
        //} else if(runListV2[r2]< (Double_t)23370) {
        //    continue;
      } else {
        if(bRateScan) {
          for(Int_t r3=0; r3<(Int_t)runR.size(); r3++) {
            if(runR[r3]==runListV2[r2]) {
              //printf("%d\t%f\t%f\n",r3,runR[r3],runListV2[r2]);
              newRunR_33.push_back(runR[r3]);
              newRate1_33.push_back(rates1[r3]);
              newRate2_33.push_back(rates2[r3]);
              continue;
            }
          }//for(Int_t r3=0; r3<(Int_t)runR.size(); r3++) 
        }//if(bRateScan) 
        if(bDAQ) {///based on DAQinfo separate some runs
          for(Int_t r3=0; r3<(Int_t)runDAQ.size(); r3++) {
            if(runDAQ.at(r3)==runListV2[r2]) {
              //printf("%d\t%f\t%f\n",r3,runR[r3],runListV2[r2]);
              if(acTrigRun2.at(r3)==2) {
                runs2by3.push_back(runListV2[r2]);
                polV22by3.push_back(polV2[r2]);
                polErV22by3.push_back(polErV2[r2]);
                polV12by3.push_back(polV1[r1]);
                polErV12by3.push_back(polErV1[r1]);
                break;
              } else if(acTrigRun2.at(r3)==3) {
                newPolV2.push_back(polV2[r2]);
                newPolErV2.push_back(polErV2[r2]);
                newEdgeV2.push_back(comptEdgeV2[r2]);
                newEdgeErV2.push_back(edgeErV2[r2]);
                commonRuns.push_back(runListV2[r2]);
                polDiff.push_back(polV1[r1] - polV2[r2]);
                polDiffEr.push_back(polErV1[r1]);//TMath::Sqrt(polErV1[r1]*polErV1[r1] + polErV2[r2]*polErV2[r2]));
                CEDiff.push_back(comptEdgeV1[r1]-comptEdgeV2[r2]);
                //CEDiffEr.push_back(TMath::Sqrt(edgeErV1[r1]*edgeErV1[r1]+edgeErV2[r2]*edgeErV2[r2]));
                CEDiffEr.push_back(edgeErV2[r2]);///because the data set is same, quad sum is not okay
                zero_1.push_back(0.0);          
                newPolV1.push_back(polV1[r1]);
                newPolErV1.push_back(polErV1[r1]);
                newEdgeV1.push_back(comptEdgeV1[r1]);
                newEdgeErV1.push_back(edgeErV1[r1]);
                lostCyc.push_back(goodCycV2[r2] - goodCycV1[r1]);
                hPolV2->Fill(polV2[r2]);//,1.0/(polErV2[r2]*polErV2[r2]));
                hPolV1->Fill(polV1[r1]);//,1.0/(polErV1[r1]*polErV1[r1]));
                hPolDiff->Fill(polDiff.back());
                hPolV1Er->Fill(polErV1[r1]);
                hPolV2Er->Fill(polErV2[r2]);
                hCEDiff->Fill(CEDiff.back());
                hChiSqV1->Fill(chiSqV1[r1]);
                hChiSqV2->Fill(chiSqV2[r2]);
              } else {
                cout<<"no trigger information for run "<<runListV2[r2]<<endl;
                c3++;
              }
            }
          }//for(Int_t r3=0; r3<(Int_t)runDAQ.size(); r3++) 
        }//if(bDAQ)
      }///process the final 'else' loop
      //break;
      }
    }
  }//for(Int_t r2=0; r2<(Int_t)runListV2.size(); r2++) 
  //cout<<red<<"newRunR.size() = "<<newRunR.size()<<endl;
  //cout<<red<<"commonRuns.size() = "<<commonRuns.size()<<normal<<endl;
  //fileCheck.close();
  //cout<<magenta<<c3<<" runs with diff. in pol\% < "<<loLimit<<endl;
  cout<<magenta<<c2<<" runs with error < "<<loLimit<<endl;
  cout<<badRuns.size()<<" runs being ignored and tagged as 'bad runs'"<<endl;
  cout<<c1<<" runs with Diff. in pol\% > "<<hiLimit<<endl;
  cout<<c3<<" runs with no trigger info"<<endl;
  cout<<runs2by3.size()<<" runs with 2by3 trigger"<<endl;
  cout<<commonRuns.size()<<" runs with 3by3 trig"<<normal<<endl;

  if(bScAc) { ///v1:Ac; v2:Sc
    TCanvas *cScAc = new TCanvas("cScAc","time dependence",0,0,800,400);
    TMultiGraph *grRatio = new TMultiGraph();
    cScAc->cd();
    std::vector<Double_t> rScAc_2by3, rScAc_3by3, rScAcEr_2by3, rScAcEr_3by3, zero2, zero3;
    for(Int_t r=0; r<(Int_t)commonRuns.size(); r++) {
      rScAc_3by3.push_back(newPolV1.at(r)/newPolV2.at(r));
      rScAcEr_3by3.push_back( TMath::Sqrt(pow(newPolErV1.at(r)/newPolV2.at(r),2) + pow((newPolV1.at(r)/(pow(newPolV2.at(r),2)))*newPolErV2.at(r), 2) ));
      zero3.push_back(0.0);
    }
    for(Int_t r=0; r<(Int_t)runs2by3.size(); r++) {
      rScAc_2by3.push_back(polV12by3.at(r)/polV22by3.at(r));
      rScAcEr_2by3.push_back( TMath::Sqrt(pow(polErV12by3.at(r)/polV22by3.at(r),2) + pow((polV12by3.at(r)/(pow(polV22by3.at(r),2)))*polErV22by3.at(r), 2) ));
      zero2.push_back(0.0);
    }
    //TGraphErrors *grAc = new TGraphErrors((Int_t)commonRuns.size(), commonRuns.data(), newPolV1.data(), zero2.data(), newPolErV1.data());
    //TGraphErrors *grSc = new TGraphErrors((Int_t)commonRuns.size(), commonRuns.data(), newPolV2.data(), zero2.data(), newPolErV2.data());
    //TGraphErrors *grRatio = new TGraphErrors((Int_t)commonRuns.size(), commonRuns.data(), rScAc.data(), zero2.data(), newPolErV2.data());
    TGraphErrors *gr2by3 = new TGraphErrors((Int_t)runs2by3.size(), runs2by3.data(), rScAc_2by3.data(), zero2.data(), rScAcEr_2by3.data());
    gr2by3->SetMarkerStyle(kOpenSquare);
    gr2by3->SetMarkerColor(kBlue);
    gr2by3->SetLineColor(kBlue);
    TGraphErrors *gr3by3 = new TGraphErrors((Int_t)commonRuns.size(), commonRuns.data(), rScAc_3by3.data(), zero3.data(), rScAcEr_3by3.data());
    gr3by3->SetMarkerStyle(kOpenSquare);
    gr3by3->SetMarkerColor(kRed);
    gr3by3->SetLineColor(kRed);
    grRatio->Add(gr2by3);
    grRatio->Add(gr3by3);
    grRatio->Draw("AP");
    linearFitV1->SetLineColor(kBlue);
    gr2by3->Fit(linearFitV1,"EM");
    linearFitV2->SetLineColor(kRed);
    gr3by3->Fit(linearFitV2,"EM");
    grRatio->GetXaxis()->SetTitle("Run number"); 
    grRatio->GetYaxis()->SetTitle("polarization Ratio");
    grRatio->SetTitle();
    grRatio->GetXaxis()->SetLimits(run1-5,run2+5); 
    TLegend *legRa = new TLegend(0.12,0.75,0.3,0.95);
    legRa->AddEntry(gr2by3,"Ac(2by3)/Sc","lpe");
    legRa->AddEntry(gr3by3,"Ac(3by3)/Sc","lpe");
    legRa->SetFillColor(0);
    legRa->Draw();

    cScAc->Update();///forces the generation of 'stats' box
    TPaveStats *ps1 = (TPaveStats*)gr2by3->GetListOfFunctions()->FindObject("stats");
    ps1->SetX1NDC(0.4); ps1->SetX2NDC(0.6);
    ps1->SetTextColor(kRed);
    TPaveStats *ps2 = (TPaveStats*)gr3by3->GetListOfFunctions()->FindObject("stats");
    ps2->SetX1NDC(0.65); ps2->SetX2NDC(0.85);
    ps2->SetTextColor(kBlue);
    cScAc->Modified();

    cScAc->SaveAs("ratio_ScAc.png");
  }

  if(bRateScan) {
    TCanvas *c10 = new TCanvas("c10","rates Vs runs",0,0,800,400);
    c10->cd();
    TGraph *grR23 = new TGraph((Int_t)newRunR_23.size(), newRunR_23.data(), newRate1_23.data());
    TGraph *grR23_2 = new TGraph((Int_t)newRunR_23.size(), newRunR_23.data(), newRate2_23.data());
    grR23->SetMarkerColor(kRed);
    grR23_2->SetMarkerColor(kRed-2);

    TGraph *grR33 = new TGraph((Int_t)newRunR_33.size(), newRunR_33.data(), newRate1_33.data());
    TGraph *grR33_2 = new TGraph((Int_t)newRunR_33.size(), newRunR_33.data(), newRate2_33.data());
    grR33->SetMarkerColor(kBlue);
    grR33_2->SetMarkerColor(kBlue-2);

    TMultiGraph *grRate = new TMultiGraph();
    grRate->Add(grR23);
    grRate->Add(grR33);
    grRate->Add(grR23_2);
    grRate->Add(grR33_2);
    ////grR->SetMarkerStyle(kSquare);
    grRate->Draw("AP");
    grRate->GetXaxis()->SetTitle("Run number"); 
    grRate->GetYaxis()->SetTitle("aggregate Rate");
    TLegend *legR = new TLegend(0.72,0.7,0.95,0.95);
    legR->AddEntry(grR23,"2/3 H+,LasOn","lpe");
    legR->AddEntry(grR23_2,"2/3 H+,LasOff","lpe");
    legR->AddEntry(grR33,"3/3 H+,LasOn","lpe");
    legR->AddEntry(grR33_2,"3/3 H+,LasOff","lpe");
    legR->SetFillColor(0);
    legR->Draw();
    c10->SaveAs("aggregate_rates.png");
  }



  if(bAbsPol) {
    if(debug) cout<<blue<<"Histogramming Absolute polarization"<<normal<<endl; 
    TCanvas *cAbsPol = new TCanvas("cAbsPol", "absolute polarization", 0,0,450,600);
    cAbsPol->Divide(1,2);
    cAbsPol->cd(1);
    hPolV2->Draw("H");
    hPolV2->Fit("gaus");
    cAbsPol->cd(2);
    hPolV1->Draw("H");
    hPolV1->Fit("gaus");
    cAbsPol->SaveAs(destDir+Form("absPolRunSet1.png"));

    //wmPolV2 = wmNrPolV2/wmDrPolV2;
    //wmPolV1 = wmNrPolV1/wmDrPolV1;
    //wmPolErV2 = TMath::Sqrt(1.0/wmDrPolV2);
    //wmPolErV1 = TMath::Sqrt(1.0/wmDrPolV1);
    //cout<<blue<<"the weighted mean of polV2 is: "<<wmPolV2<<"+/-"<<wmPolErV2<<endl;
    //cout<<"the weighted mean of polV1 is: "<<wmPolV1<<"+/-"<<wmPolErV1<<normal<<endl;
  }

  if(bPolCompare) {
    if(debug) cout<<blue<<"Comparing Polarization from the two methods"<<normal<<endl; 
    //coordinates in TCanvas are in order :x,y of left top corner;x,y of right bottom corner
    TCanvas *polAvgP1 = new TCanvas("polAvgP1","Polarization trend",0,0,800,800);
    polAvgP1->Divide(1,2);
    polAvgP1->cd(1);
    polAvgP1->GetPad(1)->SetGridx(1);
    TGraphErrors *grPolV1,*grPolV2;
    //TGraphErrors *grBadV2,*grBadV1;
    TLegend *legPolV2 = new TLegend(0.12,0.1,0.38,0.35);
    TLegend *legPolV1 = new TLegend(0.12,0.82,0.38,0.95);

    //grPolV1 = new TGraphErrors(destDir+Form("polList_%d_%d.txt",run1,run2),"%lg %lg %lg");
    //grPolV1 = new TGraphErrors(runListV1.size(),runListV1.data(),polV1.data(),zero,polErV1.data());
    //grPolV2 = new TGraphErrors(runListV2.size(),runListV2.data(),polV2.data(),zero,polErV2.data());
    grPolV2 = new TGraphErrors((Int_t)newPolV2.size(),commonRuns.data(),newPolV2.data(),zero,newPolErV2.data());
    grPolV1 = new TGraphErrors((Int_t)newPolV1.size(),commonRuns.data(),newPolV1.data(),zero,newPolErV1.data());
    //grBadV2 = new TGraphErrors((Int_t)badRuns.size(),badRuns.data(),cutPolV2.data(),zero,cutPolErV2.data());
    //grBadV1 = new TGraphErrors((Int_t)badRuns.size(),badRuns.data(),cutPolV1.data(),zero,cutPolErV1.data());

    grPolV2->SetMarkerStyle(kOpenCircle);
    grPolV2->SetMarkerColor(kRed);
    grPolV2->SetLineColor(kRed);
    grPolV2->GetXaxis()->SetTitle("Run number");
    grPolV2->GetYaxis()->SetTitle(" polarization (%)");
    grPolV2->GetXaxis()->SetLimits(run1-5,run2+5); 
    grPolV2->Draw("AP");
    linearFitV2->SetLineColor(kRed);
    grPolV2->Fit(linearFitV2,"EMR");

    polAvgP1->GetPad(2)->SetGridx(1);
    grPolV1->SetMarkerStyle(kOpenCircle);
    grPolV1->SetMarkerColor(kBlue);
    grPolV1->SetLineColor(kBlue);
    grPolV1->GetXaxis()->SetTitle("Run number");
    grPolV1->GetYaxis()->SetTitle(" polarization (%)");
    grPolV1->GetXaxis()->SetLimits(run1-5,run2+5); 
    grPolV1->Draw("P");
    linearFitV1->SetLineColor(kBlue);
    grPolV1->Fit(linearFitV1,"EMR");

    polAvgP1->GetPad(1)->Update();///forces the generation of 'stats' box
    TPaveStats *ps1 = (TPaveStats*)grPolV2->GetListOfFunctions()->FindObject("stats");
    ps1->SetX1NDC(0.4); ps1->SetX2NDC(0.6);
    ps1->SetTextColor(kRed);
    TPaveStats *ps2 = (TPaveStats*)grPolV1->GetListOfFunctions()->FindObject("stats");
    ps2->SetX1NDC(0.65); ps2->SetX2NDC(0.85);
    ps2->SetTextColor(kBlue);
    polAvgP1->GetPad(1)->Modified();

    legPolV2->AddEntry(grPolV2,ver2,"lpe");
    legPolV2->AddEntry(grPolV1,ver1,"lpe");
    legPolV2->AddEntry(linearFitV2,"linear fit of "+ver2,"l");
    legPolV2->AddEntry(linearFitV1,"linear fit of "+ver1,"l");
    legPolV2->SetFillColor(0);
    legPolV2->Draw();

    polAvgP1->cd(2);
    TGraphErrors *grPolDiff = new TGraphErrors((Int_t)polDiff.size(),commonRuns.data(),polDiff.data(),zero_1.data(),polDiffEr.data());
    grPolDiff->SetMarkerStyle(kOpenCircle);
    grPolDiff->SetMarkerColor(kBlack);
    grPolDiff->Draw("AP");
    linearFit->SetLineColor(kBlack);
    grPolDiff->Fit(linearFit,"EM");
    grPolDiff->SetTitle(0);
    grPolDiff->GetXaxis()->SetLimits(run1-5,run2+5); 
    grPolDiff->GetYaxis()->SetDecimals(1);
    legPolV1->AddEntry(grPolDiff,"pol Diff","lpe");
    legPolV1->AddEntry(linearFit,"linear fit ","l");
    legPolV1->SetFillColor(0);
    legPolV1->Draw();

    //grPolDiff->SetMaximum(1.5);
    //grPolDiff->SetMinimum(-2.0);
    grPolDiff->GetXaxis()->SetTitle("run number");
    grPolDiff->GetYaxis()->SetTitle("pol_"+ver1+" - "+"pol_"+ver2);
    polAvgP1->SaveAs(destDir+Form("polAvg_%s_%s.png",ver1.Data(),ver2.Data()));

    //TCanvas *cpolDiff = new TCanvas("cpolDiff","difference in evaluated Polarization",10,10,450,450);
    //cpolDiff->cd();
    //hPolDiff->SetLineColor(kBlack);
    //hPolDiff->Draw("H");
    //hPolDiff->Fit("gaus");
    //cpolDiff->Modified();
    //cpolDiff->Update();
    ////cpolDiff->SaveAs(destDir+Form("polDiff_%d_%d.png",run1,run2));
    //cpolDiff->SaveAs(destDir+Form("polDiff.png"));
  }

  if(bPolErCompare) {
    if(debug) cout<<blue<<"Comparing Pol-error from the two methods"<<normal<<endl; 
    TCanvas *cPolErr = new TCanvas("cPolErr","polarization error",10,10,1000,600);
    cPolErr->Divide(2,1);

    cPolErr->cd(1);
    //hPolV1Er->SetFillColor(41);
    hPolV1Er->SetLineColor(kBlue);
    //hPolV1Er->SetTitle(Form("error on Polarization for Qweak phase-1 data"));
    //hPolV1Er->GetYaxis()->SetTitle("polarization error (statistical)");
    hPolV1Er->Draw("H");
    hPolV1Er->Fit("gaus");

    cPolErr->cd(2);
    //hPolV1Er->SetFillColor(41);
    hPolV2Er->SetLineColor(kRed);
    //hPolV2Er->SetTitle("");//Form("error on Polarization for Qweak phase-1 data"));
    //hPolV2Er->GetYaxis()->SetTitle("polarization error (statistical)");
    hPolV2Er->Draw("H");   
    hPolV2Er->Fit("gaus");    
    cPolErr->SaveAs(Form(destDir + "polEr_%d_%d.png",run1,run2));
  }

  if(bCECompare) {
    if(debug) cout<<blue<<"Comparing Compton edge from V2 and V1 methods"<<normal<<endl; 
    TCanvas *cComptEdge = new TCanvas("cComptEdge","Compton edge",10,10,1000,900);
    TCanvas *cCEDiff = new TCanvas("cCEDiff","difference in Compton edge",60,0,500,500);
    ///coordinates of legend are:x,y coordinates of left bottom corner; followed by x,y coordinates of top right corner
    TLegend *legCedge= new TLegend(0.1,0.1,0.30,0.25);;
    cComptEdge->Divide(1,2);
    cComptEdge->cd(1);

    cout<<blue<<"size of commonRuns:"<<commonRuns.size()<<", newEdgeV2:"<<newEdgeV2.size()<<", newEdgeV1:"<<newEdgeV1.size()<<normal<<endl;

    ofstream fCedgeRunlet,fCedgeLasCyc;
    fCedgeRunlet.open("edgeV2_run1_run2.txt");
    for (Int_t r=0; r<(Int_t)commonRuns.size(); r++) {
      fCedgeRunlet<<Form("%5.0f\t%2.2f\t%.2f\n",commonRuns[r],newEdgeV2[r],newEdgeErV2[r]);
    }
    fCedgeRunlet.close();

    fCedgeLasCyc.open("edgeV1_run1_run2.txt");
    for (Int_t r=0; r<(Int_t)commonRuns.size(); r++) {
      fCedgeLasCyc<<Form("%5.0f\t%2.2f\t%.2f\n",commonRuns[r],newEdgeV1[r],newEdgeErV1[r]);
    }
    fCedgeLasCyc.close();

    TGraphErrors *grLasCycCedge = new TGraphErrors("edgeV1_run1_run2.txt","%lg %lg %lg");
    cComptEdge->GetPad(1)->SetGridx(1);
    grLasCycCedge->SetTitle();
    grLasCycCedge->SetMarkerStyle(kOpenSquare);
    grLasCycCedge->SetMarkerColor(kBlue);
    Double_t runletCedgeInRange[numbRuns];
    Int_t c4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runListV2[r] >= run1) && (runListV2[r] <= run2)) {
        c4++;
        runletCedgeInRange[c4] = comptEdgeV2[r];
      }
    }
    //TGraph *grRunletCedge = new TGraph(c4,lasCycRunnum,runletCedgeInRange);
    TGraphErrors *grRunletCedge = new TGraphErrors("edgeV2_run1_run2.txt","%lg %lg %lg");
    grRunletCedge->SetMarkerStyle(kFullSquare);//(kFullSquare);
    grRunletCedge->SetMarkerColor(kRed);
    grRunletCedge->SetMarkerSize(0.7);
    TMultiGraph *grCedge = new TMultiGraph();
    grCedge->Add(grRunletCedge);
    grCedge->Add(grLasCycCedge);
    grCedge->Draw("AP");
    grCedge->SetTitle("fitted Compton edge");
    grCedge->GetXaxis()->SetTitle("Run number");
    grCedge->GetYaxis()->SetTitle("Compton edge (strip number)");
    grCedge->GetXaxis()->SetLimits(run1-5,run2+5); 

    legCedge->AddEntry(grLasCycCedge,"V1 evaluation","p");
    legCedge->AddEntry(grRunletCedge,"V2 evaluation","p");
    legCedge->SetFillColor(0);
    legCedge->Draw();

    cComptEdge->cd(2);
    cComptEdge->GetPad(2)->SetGridx(1);
    TGraphErrors *grCEDiff = new TGraphErrors((Int_t)CEDiff.size(),commonRuns.data(),CEDiff.data(),zero_1.data(),CEDiffEr.data());
    grCEDiff->SetMarkerStyle(kOpenCircle);
    grCEDiff->SetMarkerColor(kBlack);
    grCEDiff->Draw("AP");
    grCEDiff->Fit(linearFit,"EM");
    grCEDiff->SetTitle();
    grCEDiff->GetXaxis()->SetLimits(run1-5,run2+5); 
    grCEDiff->SetMaximum(3.2); 
    grCEDiff->SetMinimum(-3.2);

    //grCEDiff->GetYaxis()->SetTitleSize(0.05);
    grCEDiff->GetXaxis()->SetTitle("run number");
    grCEDiff->GetYaxis()->SetTitle("CE_V1 - CE_V2");

    cCEDiff->cd();
    hCEDiff->Draw("H");
    hCEDiff->Fit("gaus");
    //////cComptEdge->SaveAs(Form("/w/hallc/qweak/narayan/comptEdge_%d_%d.png",run1,run2));
    //cComptEdge->SaveAs(destDir+Form("CE_%d_%d.png",run1,run2));
    //cCEDiff->SaveAs(destDir+Form("CEDiff_%d_%d.png",run1,run2));    
  }

  if(bChiSqrCompare) {
    if(debug) cout<<blue<<"Comparing ChiSqr per Degree of freedom of V2 & V1"<<normal<<endl; 
    TCanvas *cChiSqr = new TCanvas("cChiSqr","Chi-sqr / ndf",10,10,1000,500);
    TGraph *grChiSqr = new TGraph((Int_t)runListV1.size(),runListV1.data(),chiSqV1.data());
    TLegend *legCedge= new TLegend(0.1,0.75,0.4,0.9);;
    cChiSqr->SetGridx(1);
    grChiSqr->SetTitle();
    grChiSqr->SetMarkerStyle(kOpenCircle);
    grChiSqr->SetTitle("chiSqr/ndf in asymmetry fit by laser Cyc evaluation");
    grChiSqr->GetXaxis()->SetTitle("Run number");
    grChiSqr->GetYaxis()->SetTitle("chiSqr / ndf");
    grChiSqr->SetMarkerColor(kBlue);
    grChiSqr->SetLineColor(kBlue);
    grChiSqr->GetXaxis()->SetLimits(run1-5,run2+5); 
    grChiSqr->Draw("AP");
    linearFit->SetLineColor(kBlue);
    grChiSqr->Fit(linearFit,"ME");
    Double_t runletChiSqrInRange[numbRuns];
    Int_t c4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runListV2[r] >= run1) && (runListV2[r] <= run2)) {
        c4++;
        runletChiSqrInRange[c4] = chiSqV2[r];
      }
    }
    TGraph *grRunletChiSqr = new TGraph(c4,runListV2.data(),chiSqV2.data());
    grRunletChiSqr->SetMarkerStyle(kOpenCircle);
    grRunletChiSqr->SetMarkerColor(kRed);
    grRunletChiSqr->SetLineColor(kRed);
    grRunletChiSqr->Draw("P");
    linearFit->SetLineColor(kRed);
    grRunletChiSqr->Fit(linearFit,"ME");
    legCedge->AddEntry(grChiSqr,"laser Cycle evaluation","p");
    legCedge->AddEntry(grRunletChiSqr,"runlet based evaluation","p");
    legCedge->SetFillColor(0);
    legCedge->Draw();

    cChiSqr->SaveAs(Form(destDir + "chiSqr_%d_%d.png",run1,run2));
  }

  if(bChiSqrHist) {
    if(debug) cout<<blue<<"Histogramming ChiSqr per Degree of freedom of V2 & V1"<<normal<<endl; 
    TCanvas *cChiSqr = new TCanvas("cChiSqrHist","chi-Sqr/ndf",10,10,1000,600);
    cChiSqr->Divide(2,1);

    cChiSqr->cd(1);
    hChiSqV1->SetLineColor(kBlue);
    hChiSqV1->Draw("H");
    hChiSqV1->Fit("gaus");
    cChiSqr->cd(2);
    hChiSqV2->SetLineColor(kRed);
    hChiSqV2->Draw("H");  
    hChiSqV2->Fit("gaus");
    cChiSqr->SaveAs(Form(destDir + "chiSqrHist_%d_%d.png",run1,run2));   
  }
  //cout<<red<<"\nTemporarily using polV2_radCor.info into fileV1 variable\n"<<normal<<endl;//!Temp
  return (Int_t)polDiff.size();//the number of runs used
}

