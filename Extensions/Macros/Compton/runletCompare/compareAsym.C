#include <rootClass.h>
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/comptonRunConstants.h"
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/infoDAQ.C"
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/rhoToX.C"
const Bool_t kVladas_meth=0,kVladas_data=0;
Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end() ) { 
    cout<<red<<"ignored strip: "<<*thisStrip<<normal<<endl;
    TF1::RejectPoint();
    return 0;
  }
  if(!kVladas_meth) {//!my method
    xStrip = xCedge - (par[1] -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as Cedge itself
    rhoStrip = param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3];
  } else {  //!Vladas's numbers
    xStrip = 17.315 - 0.2*(par[1] - (*thisStrip))*par[0]; //for 2nd parameter as Cedge itself
    rhoStrip = 2.81648E-06 + xStrip*0.0602395 + xStrip*xStrip*(-0.000148674) + xStrip*xStrip*xStrip*1.84876E-07 + xStrip*xStrip*xStrip*xStrip*1.05068E-08 + xStrip*xStrip*xStrip*xStrip*xStrip*(-2.537E-10);
    //cout<<red<<(*thisStrip)<<"\t"<<xStrip<<"\t"<<rhoStrip<<normal<<endl;
  }
  rhoPlus = 1.0-rhoStrip*(1.0+a_const);
  rhoMinus = 1.0-rhoStrip*(1.0 - a_const);//just a term in eqn 24
  dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a_const)); // 2nd term of eqn 22
  dsdrho =((rhoStrip*(1.0 - a_const)*rhoStrip*(1.0 - a_const)/rhoMinus)+1.0+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a_const
  //Double_t calcAsym=((rhoPlus*(1.0-1.0/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  return (par[2]*(rhoPlus*(1.0-1.0/(rhoMinus*rhoMinus)))/dsdrho);//calcAsym;
}

const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
Int_t compareAsym(Int_t run1=runBegin, Int_t run2=runEnd) {
  gROOT->SetStyle("publication");
  Bool_t debug=0;
  Bool_t kDiff=0,kCombo=1;
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  ifstream fileAsymLC,fileAsymRL;
  ofstream fileCheck;
  const char *asymRLDir ="/w/hallc/compton/users/narayan/runletCompare/asymRL_Apr2013";
  //const char *asymLCDir ="/w/hallc/compton/users/narayan/my_scratch/www_27Jun14";
  //const char *asymLCDir ="/w/hallc/compton/users/narayan/my_scratch/www_15Jul14";
  const char *asymLCDir ="/w/hallc/compton/users/narayan/my_scratch/www";
  //const char *myDir ="/w/hallc/compton/users/narayan/makingPlots/plots";//my_scratch/www";
  Int_t runRange = run2-run1+1;

  Int_t c1=0;//track runs common between RL and LC
  Int_t c2=0,c5=0;//track runs which were lost due to inconcsistency
  Int_t c3=0;//track the no.of used strips;
  Int_t c4=0;//track the no.of used strips;

  char name[20];

  TH1D *hAsymRL[nStrips],*hAsymLC[nStrips],*hAsymRLEr[nStrips],*hAsymLCEr[nStrips];
  for (Int_t s=0; s<nStrips; s++) {
    sprintf(name,"hAsymRLStr%d",s+1);
    hAsymRL[s] = new TH1D(name,"asymRL", runRange, -0.0001, 0.0001); 
    hAsymRL[s]->SetBit(TH1::kCanRebin);

    sprintf(name,"hAsymRLErStr%d",s+1);
    hAsymRLEr[s] = new TH1D(name,"asymRLEr", runRange, -0.0001, 0.0001); 
    hAsymRLEr[s]->SetBit(TH1::kCanRebin);

    sprintf(name,"hAsymLCStr%d",s+1);
    hAsymLC[s] = new TH1D(name,"asymLC", runRange, -0.0001, 0.0001); 
    hAsymLC[s]->SetBit(TH1::kCanRebin);

    sprintf(name,"hAsymLCErStr%d",s+1);
    hAsymLCEr[s] = new TH1D(name,"asymLCEr", runRange, -0.0001, 0.0001); 
    hAsymLCEr[s]->SetBit(TH1::kCanRebin);
  }
  const Int_t chstr = 44;
  Double_t stripLC[nStrips]={0.0}; 
  Double_t stripRL[nStrips]={0.0};

  fileCheck.open(Form("listUsedRuns.txt"));
  for (Int_t r=run1; r<=run2; r++) {
    //for (Int_t r=run1+2; r<=run1+4; r++) {
    Double_t asymLC[nStrips]={0.0},asymLCEr[nStrips]={0.0};
    Double_t asymRL[nStrips]={0.0},asymRLEr[nStrips]={0.0};
    fileAsymRL.open(Form("%s/run_%d_asymRL.txt",asymRLDir,r));
    if(fileAsymRL.is_open()) {
      c4++;
      filePrefix= Form("run_%d/edetLasCyc_%d_",r,r);
      fileAsymLC.open(Form("%s/%sAcExpAsymP1.txt",asymLCDir,filePrefix.Data()));
      if(fileAsymLC.is_open()) {
        Int_t s1=0;
        while(fileAsymLC.good() && fileAsymRL.good()) {
          fileAsymRL>>stripRL[s1]>>asymRL[s1]>>asymRLEr[s1];
          fileAsymLC>>stripLC[s1]>>asymLC[s1]>>asymLCEr[s1];
          s1++;
        }
        if(s1>50) {
          fileCheck<<"using run "<<r<<endl;
          c1++;//counting no.of runs being compared finally
          if((asymRL[chstr]>0.0) && (asymLC[chstr]>0.0)) { ///asym around CE is positive
            //cout<<green<<asymRL[chstr]<<"\t"<<asymLC[chstr]<<"\t"<<asymLCEr[chstr]<<endl;
            for(Int_t s=0; s<nStrips; s++) {
              hAsymRL[s]->Fill(asymRL[s]);
              hAsymRLEr[s]->Fill(asymRLEr[s]);
              hAsymLC[s]->Fill(asymLC[s]);
              hAsymLCEr[s]->Fill(asymLCEr[s]);
            }
          } else if((asymRL[chstr]<0.0) && (asymLC[chstr]<0.0)) {///asym around CE is negative
            //cout<<blue<<asymRL[chstr]<<"\t"<<asymLC[chstr]<<"\t"<<asymLCEr[chstr]<<endl;
            for(Int_t s=0; s<nStrips; s++) {
              hAsymRL[s]->Fill(-asymRL[s]);
              hAsymRLEr[s]->Fill(asymRLEr[s]);
              hAsymLC[s]->Fill(-asymLC[s]);
              hAsymLCEr[s]->Fill(asymLCEr[s]);
            }
          } else if(asymRL[chstr]==0.0 && asymLC[chstr]==0.0) {
            cout<<blue<<"asym is null for run: "<<r<<normal<<endl;
            c5++;
          } else {
            cout<<"check run "<<r<<", with asyms: "<<asymRL[chstr]<<"\t"<< asymLC[chstr]<<endl;
            c2++;
          }        
        } else fileCheck<<"unable to use run "<<r<<endl;//cout<<red<<"unable to use run "<<r<<normal<<endl;
        //} else cout<<"file probably empty for run "<<r<<endl;
        fileAsymLC.close();
    } else if(debug) cout<<green<<"couldn't open lasCycAsym file for run "<<r<<normal<<endl;
    fileAsymRL.close();
  } else if(debug) cout<<blue<<"runletAsym file for run "<<r<<" couldn't be opened"<<endl;
  }
  fileCheck.close();
  cout<<red<<"opened runs in this range from RL: "<<c4<<normal<<endl;
  cout<<red<<"lost runs due to incocnsistency: "<<c2<<normal<<endl;
  cout<<red<<"using runs in this range: "<<c1<<normal<<endl;
  if (!maskSet) infoDAQ(23220);///to define skipStrip
  TF1 *polFitLC,*polFitRL,*linearFit;
  linearFit = new TF1("linearFit","pol0",1,50);
  polFitLC = new TF1("polFitLC",theoreticalAsym,1,49,3);
  polFitLC->SetParameters(1.0033,50,0.85);
  polFitLC->SetParLimits(0,1.0033,1.0033);
  polFitLC->SetParLimits(1,30.0,63.0);///allow the CE to vary between strip 30-63
  polFitLC->SetParLimits(2,-1.0,1.0);///allowing polarization to be - 100% to +100%
  polFitLC->SetParNames("effStrip","CE_LC","polLC");
  polFitLC->SetLineColor(kBlue);
  Cedge[0]= 49.0;//to mimic the CE assignment of plane1
  xCedge = rhoToX(0); ///this function should be called after determining the Cedge
  cout<<red<<"the maxdist used:"<<xCedge<<normal<<endl;

  polFitRL = new TF1("polFitRL",theoreticalAsym,1,49,3);
  polFitRL->SetParameters(1.0033,50,0.85);
  polFitRL->SetParLimits(0,1.0033,1.0033);
  polFitRL->SetParLimits(1,30.0,63.0);///allow the CE to vary between strip 30-63
  polFitRL->SetParLimits(2,-1.0,1.0);///allowing polarization to be - 100% to +100%
  polFitRL->SetParNames("effStrip","CE_RL","polRL");
  polFitRL->SetLineColor(kRed);

  std::vector<Double_t > avgAsymRL,avgAsymRLEr;
  std::vector<Double_t > avgAsymLC,avgAsymLCEr;
  //Double_t avgAsymDiff[nStrips],avgAsymDiffEr[nStrips];
  std::vector<Double_t > avgAsymDiff,avgAsymDiffEr,strip;
  std::vector<Double_t > relAsymDiff,relAsymDiffEr;
  Double_t zero[nStrips];

  ///////////////////////Temp/////////////
  //TCanvas *cTest = new TCanvas("cTest","Testing",100,200,1250,500);//x,y cordinates of left top corner
  //cTest->Divide(3,1);
  //cTest->cd(1);
  //hAsymRL[0]->Draw("H");
  //cTest->cd(2);
  //hAsymRL[1]->Draw("H");
  //cTest->cd(3);
  //hAsymRL[2]->Draw("H");
  ///////////////////////Temp/////////////

  for (Int_t s=0; s<nStrips; s++) {
    if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    //if(s==1||s==5||s==9||s==19||s==38) continue;
    hAsymRL[s]->Fit("gaus","Q 0");
    hAsymLC[s]->Fit("gaus","Q 0");
    avgAsymRL.push_back(hAsymRL[s]->GetMean(1));
    avgAsymRLEr.push_back(hAsymRL[s]->GetMeanError(1));
    avgAsymLC.push_back(hAsymLC[s]->GetMean(1));
    avgAsymLCEr.push_back(hAsymLC[s]->GetMeanError(1));
    zero[s]=0.0;
    strip.push_back((Double_t)s+1);
    avgAsymDiff.push_back(avgAsymRL.back()-avgAsymLC.back());
    avgAsymDiffEr.push_back(TMath::Sqrt(avgAsymRLEr.back()*avgAsymRLEr.back() + avgAsymLCEr.back()*avgAsymLCEr.back()));
    //printf("%f\t%f\t%f\n",stripLC[s],hAsymLC[s]->GetMean(1),hAsymLC[s]->GetMeanError(1));
    c3++;
    if(avgAsymRL.back()!=0.0) {
      relAsymDiff.push_back((avgAsymRL.back()-avgAsymLC.back())/avgAsymRL.back());
      //dum1 = avgAsymRLEr[s]*avgAsymLC/(avgAsymRL*avgAsymRL);
      relAsymDiffEr.push_back(TMath::Sqrt(pow(avgAsymRLEr.back()*avgAsymLC.back()/(avgAsymRL.back()*avgAsymRL.back()),2) + pow(avgAsymLCEr.back(),2)));
    }
    else cout<<"strip # "<<s+1<<", probably has avgAsymRL at zero"<<endl;
  }

  TLine *myline = new TLine(1,0,65,0);
  if(kDiff) {
    Bool_t relDiff = 0;
    TGraphErrors *grDiff;
    TCanvas *cDiff = new TCanvas("cDiff","asym Diff, RL-LC",100,200,900,500);//x,y cordinates of left top corner
    //cDiff->Divide(1,2);
    cDiff->cd();
    cDiff->SetGridx(1);
    if(relDiff) {
      grDiff = new TGraphErrors(c3, strip.data(), relAsymDiff.data(), zero, relAsymDiffEr.data());
      grDiff->SetMinimum(-0.01);
      grDiff->SetMaximum(+0.01);
      grDiff->GetYaxis()->SetTitle("(asymRL - asymLC)/asymRL");
    } else {
      grDiff = new TGraphErrors(nStrips-4, strip.data(), avgAsymDiff.data(), zero, avgAsymDiffEr.data());
      grDiff->SetMinimum(-0.001);
      grDiff->SetMaximum(+0.001);
      grDiff->GetYaxis()->SetTitle("asymRL - asymLC");
    }
    grDiff->SetMarkerStyle(20);//25:full square
    grDiff->SetMarkerColor(4);
    grDiff->SetMarkerSize(1);
    grDiff->Draw("AP");
    grDiff->Fit("linearFit","RME");//pol0");

    cDiff->Modified(); cDiff->Update();
    grDiff->SetTitle(0);
    grDiff->GetXaxis()->SetTitle("strip number");
    grDiff->GetXaxis()->SetLimits(1,65);
    if(relDiff) cDiff->SaveAs("relAsymDiffRL_LC.png");
    else cDiff->SaveAs("DiffOfAsymRL_LC.png");
  }

  if(kCombo) {
    TCanvas *cAsymRL = new TCanvas("cAsymRL","Asymmetry RL",0,0,900,500);//x,y cordinates of left top corner
    cAsymRL->cd();
    cAsymRL->SetGridx(1);
    TGraphErrors *gr1 = new TGraphErrors(c3,strip.data(),avgAsymRL.data(),zero,avgAsymRLEr.data());
    gr1->SetMarkerStyle(25);//25:open square
    gr1->SetMarkerColor(2);
    gr1->Fit("polFitRL","0 R M E");
    TGraphErrors *gr2 = new TGraphErrors(c3,strip.data(),avgAsymLC.data(),zero,avgAsymLCEr.data());
    gr2->Fit("polFitLC","0 R M E");
    gr2->SetMarkerStyle(21);//25:full square
    gr2->SetMarkerColor(4);
    gr2->SetMarkerSize(0.8);
    TMultiGraph *mgr = new TMultiGraph();
    mgr->Add(gr1);
    mgr->Add(gr2);
    mgr->Draw("AP");
    polFitRL->DrawCopy("same");
    polFitLC->DrawCopy("same");
    mgr->SetTitle(0);
    mgr->GetXaxis()->SetTitle("strip number");
    mgr->GetXaxis()->SetLimits(1,65);
    mgr->GetYaxis()->SetDecimals(3);
    mgr->GetYaxis()->SetTitle("asymmetry");

    TLegend *leg;
    leg = new TLegend(0.13,0.75,0.39,0.95);
    leg->AddEntry(gr1,"asym RL","lpe");///I just need the name
    leg->AddEntry(gr2,"asym LC","lpe");///I just need the name
    leg->AddEntry("polFitRL","QED fit to RL asym","lf");//"lpf");//
    leg->AddEntry("polFitLC","QED fit to LC asym","lf");//"lpf");//
    leg->SetFillColor(0);
    leg->Draw();
    TPaveStats *st1 = (TPaveStats*)gr1->GetListOfFunctions()->FindObject("stats");
    st1->SetTextColor(kRed);
    st1->SetX1NDC(0.76); st1->SetX2NDC(0.99); //new x end position
    st1->SetY1NDC(0.72); st1->SetY2NDC(0.95);
    TPaveStats *st2 = (TPaveStats*)gr2->GetListOfFunctions()->FindObject("stats");
    st2->SetTextColor(kBlue);
    st2->SetX1NDC(0.76); st2->SetX2NDC(0.99); //new x end position
    st2->SetY1NDC(0.47); st2->SetY2NDC(0.70);
    myline->Draw();
    cAsymRL->SaveAs("compareAsymRL_LC.png");
  }
  return c1;//(run2 - run1+1);//the number of runs processed
}

