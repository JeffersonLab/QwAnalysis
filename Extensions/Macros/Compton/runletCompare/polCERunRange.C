#include <rootClass.h>
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/comptonRunConstants.h"
///lets focus on the region between 23220 to 23530;
const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
//Int_t polRunRange(Int_t run1=23220, Int_t run2=23515)
Int_t polCERunRange(Int_t run1=runBegin, Int_t run2=runEnd)
{
  gROOT->SetStyle("publication");
  Bool_t debug=0,debug1=0,debug2=0;
  Bool_t kPrintLC=0, kPrintRL=0; 
  Bool_t bAbsPol=1,bPolCompare=1,bPolErCompare=0,bCECompare=1;
  Bool_t bChiSqrCompare=0,bChiSqrHist=0;
  Bool_t bMyRLOut = 0;///1:using pol% from my analysis of RL asymmetries; 0: using polRL provided by Vladas
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  const Int_t numbRuns = runEnd - runBegin;//a non-variable is needed to set the size of an array length
  ifstream inFileRunsLC,inFileRL,inFileLC;
  ofstream outFileLC;
  TString fileRL;
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www_27Jun14";///analysis throwing cycles with beam trip
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www_15Jul14";///taking all lasCyc,closer to RL analysis
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www";///noise subtraction,pol5 fit of QED asym
  const TString destDir="/w/hallc/compton/users/narayan/runletCompare/";
  if(bMyRLOut) {
    fileRL = destDir + Form("polRL_radCor.info");//file analyzed by A.Narayan
    //fileRL = destDir + Form("polRL_%d_%d_allStrips.info",run1,run2);//file analyzed by A.Narayan
    //fileRL = destDir + Form("polRL_%d_%d_s39excluded.info",run1,run2);//file analyzed by A.Narayan
    //fileRL = destDir + Form("polRL_%d_%d_5stripsMasked.info",run1,run2);//file analyzed by A.Narayan
    cout<<blue<<"for RL polarization using the output of my analysis from Runlet asymmetries"<<normal<<endl;
  } else fileRL = Form("/u/home/narayan/acquired/vladas/modFiles/polRunletCE_%d_%d.dat",run1,run2);//updated by Vladas on 24Apr2013

 //Form("/u/home/narayan/acquired/vladas/modFiles/polEffWidth_%d_%d.dat",run1,run2);//file updated by Vladas on 24Apr2013
  //polRunlet.open("/u/home/narayan/acquired/vladas/modFiles/runletFCEPolRun2.txt");
  inFileRL.open(fileRL.Data());
  //TString fileLC = destDir + Form("polRL_radCor.info");
  //cout<<red<<"\nTemporarily using polRL_radCor.info into fileLC variable\n"<<normal<<endl;
  TString fileLC = destDir + Form("polLC_radCor.info");
  //TString fileLC = destDir + "polLC_23220_23530.txt";
  inFileLC.open(fileLC.Data());
  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10,dum11;
  Int_t runRange = run2-run1+1;
  TH1D *hPolRL = new TH1D("polRL", "polarization runlet analysis", runRange, 80, 95);
  TH1D *hPolLC = new TH1D("polLC", "polarization laser cycle analysis", runRange, 80, 95);
  TH1D *hPolLCEr = new TH1D("polEr laser Cycle","polarization error laser cycle analysis",runRange,0,0.1);
  TH1D *hPolRLEr = new TH1D("polEr runlet based","polarization error runlet based analysis",runRange,0,0.1);
  TH1D *hPolDiff = new TH1D("diff. in pol%","difference in polarization between the two analysis",runRange,0,0.1);
  TH1D *hCEDiff = new TH1D("diff. in CE", "difference in CE between the two analysis", runRange, 0, 0.1);
  TH1D *hChiSqLC = new TH1D("chiSqr laser Cycle", "chi sqr laser cycle analysis", runRange, 0, 0.1);
  TH1D *hChiSqRL = new TH1D("chiSqr runlet based", "chi sqr runlet based analysis", runRange, 0, 0.1);
  TF1 *linearFit = new TF1("linearFit", "pol0");

  //hPolRL->SetBit(TH1::kCanRebin);
  //hPolLC->SetBit(TH1::kCanRebin);
  hPolLCEr->SetBit(TH1::kCanRebin);
  hPolRLEr->SetBit(TH1::kCanRebin);
  hPolDiff->SetBit(TH1::kCanRebin);
  hCEDiff->SetBit(TH1::kCanRebin);
  hChiSqLC->SetBit(TH1::kCanRebin);
  hChiSqRL->SetBit(TH1::kCanRebin);

  std::vector< Double_t> runListLC,runListRL,polLC,polRL,polErLC,polErRL,comptEdgeLC,comptEdgeRL,chiSqLC,chiSqRL,edgeErRL,edgeErLC;
  ///write the lasCyc based polarization into one central file
  Int_t c1=0,c3=0;
  Int_t runnum,plane,goodCyc;
  Double_t comptEdge,comptEdgeEr,effStrip,effStripEr;
  if (inFileLC.is_open()) { 
    //inFileLC >>dum1>>dum2>>dum3>>dum4>>dum5>>dum6>>dum7>>dum8>>dum9>>dum10;
    while(inFileLC.good()) {
      //inFileLC>>runnum>>pol>>polEr>>chiSq>>comptEdge>>comptEdgeEr>>effStrip>>effStripEr>>plane>>goodCyc;
      inFileLC>>runnum>>pol>>polEr>>chiSq>>NDF>>comptEdge>>comptEdgeEr>>effStrip>>effStripEr>>plane>>goodCyc;
      if(debug1) cout<<runnum<<"\t"<<pol<<"\t"<<polEr<<"\t"<<chiSq<<"\t"<<comptEdge<<"\t"<<comptEdgeEr<<"\t"<<effStrip<<"\t"<<effStripEr<<"\t"<<plane<<"\t"<<goodCyc<<endl;
      kPrintLC = 1;//(polEr<5.0 && absPol<93.0 && absPol>60.0 && polEr!=0.0 && comptEdge>30.0);// && chiSq/NDF<5.0);
      if (kPrintLC) {
        runListLC.push_back((Double_t)runnum);
        polLC.push_back(fabs(pol));
        polErLC.push_back(polEr);
        comptEdgeLC.push_back(comptEdge);
        edgeErLC.push_back(comptEdgeEr);
        chiSqLC.push_back(chiSq/NDF);
        //chiSqLC.push_back(chiSq);
        c1++;///remains as a count of how many 'good' runs we have
      }
      if(inFileLC.eof()) break;
    }
    inFileLC.close();
  } else cout<<red<<"*** Could not open file with LC pol info"<<normal<<endl; 
  cout<<red<<"found valid "<<runListLC.size()<<" analyzed runs in runListLC"<<normal<<endl;

  //ofstream fileCheck;
  //fileCheck.open("fileCheck.txt");
  //fileCheck.open("bestRunsPolRL.txt");
  Double_t zero[numbRuns]={0};
  Int_t c2=0;
  Double_t tempRun,tempPol,tempPolEr,tempChiSq,tempCE,tempCEEr;
  Int_t tempNDF;
  Double_t dummy2,dummy3,dummy4,dummy5;
  std::vector<Double_t > NDF_RL;
  if(inFileRL.is_open()) { //this is available only for plane 1
    while(inFileRL.good()) {
      if(bMyRLOut) inFileRL>>tempRun>>tempPol>>tempPolEr>>tempChiSq>>tempNDF>>tempCE>>tempCEEr>>dummy2>>dummy3>>dummy4>>dummy5;
      else inFileRL>>tempRun>>tempPol>>tempPolEr>>tempCE>>tempCEEr>>tempChiSq;
      ///when reading from RL file created by me
      kPrintRL = (tempPolEr!=0.0 && tempPolEr<5.0);
      if (kPrintRL) {
        runListRL.push_back(tempRun);
        polRL.push_back(fabs(tempPol));
        polErRL.push_back(tempPolEr);
        comptEdgeRL.push_back(tempCE);
        edgeErRL.push_back(tempCEEr);
        if(bMyRLOut) NDF_RL.push_back(tempNDF);///if reading from file generated by me
        if(bMyRLOut) chiSqRL.push_back(tempChiSq/tempNDF);///if reading from file generated by me
        else chiSqRL.push_back(tempChiSq);
        c2++;
      }
      if(debug2) cout<<runListRL[c2]<<"\t"<<polRL[c2]<<"\t"<<polErRL[c2]<<"\t"<<comptEdgeRL[c2]<<"\t"<<edgeErRL[c2]<<endl;
      if(inFileRL.eof()) break;
    }
    inFileRL.close();
  } else cout<<"*** Alert: could not open runlet based pol file"<<endl;
  cout<<red<<"found valid "<<runListRL.size()<<" analyzed runs in runListRL"<<normal<<endl;

  std::vector<Double_t> commonRuns,polDiff,CEDiff,polDiffEr,CEDiffEr,zero_1;
  std::vector<Double_t> newPolLC, newPolRL, newPolErLC, newPolErRL;
  std::vector<Double_t> newEdgeLC, newEdgeRL, newEdgeErLC, newEdgeErRL;
  Double_t polErRLSqr_inv,wmNrPolRL,wmDrPolRL;
  Double_t polErLCSqr_inv,wmNrPolLC,wmDrPolLC;
  Double_t wmPolRL,wmPolErRL;
  Double_t wmPolLC,wmPolErLC;

  /////the bestRunsPolRL.txt was created as a filtered list of runs that qualifies some cuts in RL data.
  //Int_t dum;
  //std::vector<Int_t> goodRuns;
  //ifstream goodList;
  //goodList.open("bestRunsPolRL.txt");
  //if(goodList.is_open()) {
  //  while(goodList.good()) {
  //    goodList>>dum;
  //    if(goodList.eof()) break;
  //    goodRuns.push_back(dum);
  //    if(debug) cout<<goodRuns.back()<<endl;
  //  }
  //  goodList.close();
  //} else cout<<red<<"could not open list of good runs"<<normal<<endl;
  //cout<<blue<<"no.of runs in good list: "<<goodRuns.size()<<normal<<endl;

  for(Int_t r=0; r<(Int_t)runListRL.size()-1; r++) {
    for(Int_t r2=0; r2<(Int_t)runListLC.size()-1; r2++) {
      if(runListRL[r] == runListLC[r2]) {
        if(polErRL[r]>=1.5) cout<<red<<"polErRL>1.5 for run "<<runListRL[r]<<normal<<endl;
        else if(edgeErLC[r2]>=0.8) cout<<red<<"edgeErLC is > 1.0 for run "<<runListLC[r2]<<normal<<endl;
        else {
        //if(std::find(goodRuns.begin(),goodRuns.end(),runListRL[r])==goodRuns.end()) {
        //    cout<<"skipping run "<<runListRL[r]<<". It isn't in goodList"<<endl;
        //    continue;
        //} else {
          //fileCheck<<runListRL[r]<<endl;
          newPolLC.push_back(polLC[r2]);
          newPolRL.push_back(polRL[r]);
          newPolErLC.push_back(polErLC[r2]);
          newPolErRL.push_back(polErRL[r]);
          polDiff.push_back(polRL[r] - polLC[r2]);
          commonRuns.push_back(runListRL[r2]);
          polDiffEr.push_back(TMath::Sqrt(polErLC[r2]*polErLC[r2] + polErRL[r]*polErRL[r]));
          zero_1.push_back(0.0);          
          newEdgeLC.push_back(comptEdgeLC[r2]);
          newEdgeRL.push_back(comptEdgeRL[r]);
          newEdgeErLC.push_back(edgeErLC[r2]);
          newEdgeErRL.push_back(edgeErRL[r]);
          CEDiff.push_back(comptEdgeLC[r2]-comptEdgeRL[r]);
          CEDiffEr.push_back(TMath::Sqrt(edgeErLC[r2]*edgeErLC[r2]+edgeErRL[r]*edgeErRL[r]));

          if(polErRL[r]>=0.01) {
            polErRLSqr_inv = 1.0/(polErRL[r]*polErRL[r]);
            wmNrPolRL += polRL[r]*polErRLSqr_inv; ///Numerator 
            wmDrPolRL += polErRLSqr_inv; ///Denominator
          } else cout<<red<<"zero polErRL for run "<<runListRL[r]<<normal<<endl;
          
          if(polErLC[r2]>=0.01) {
            polErLCSqr_inv = 1.0/(polErLC[r2]*polErLC[r2]);
            wmNrPolLC += polLC[r2]*polErLCSqr_inv; ///Numerator 
            wmDrPolLC += polErLCSqr_inv; ///Denominator
          } else cout<<red<<"zero polErLC for run "<<runListLC[r2]<<normal<<endl;

          hPolRL->Fill(polRL[r]);//,1.0/(polErRL[r]*polErRL[r]));
          hPolLC->Fill(polLC[r2]);//,1.0/(polErLC[r2]*polErLC[r2]));
          //hPolRL->Fill(polRL[r],1.0/(polErRL[r]*polErRL[r]));
          //hPolLC->Fill(polLC[r2],1.0/(polErLC[r2]*polErLC[r2]));
          hPolDiff->Fill(polDiff.back());
          hPolLCEr->Fill(polErLC[r2]);
          hPolRLEr->Fill(polErRL[r]);
          hCEDiff->Fill(CEDiff.back());
          hChiSqLC->Fill(chiSqLC[r2]);
          hChiSqRL->Fill(chiSqRL[r]);
          break;///mission accomplished, don't scan the list any further
        }
      }
    }
  }
  //fileCheck.close();

  cout<<green<<c3<<" entries in runlet file had a run with null polarization"<<normal<<endl;
  cout<<magenta<<commonRuns.size()<<" entries common between RL and LC"<<normal<<endl;

  if(bAbsPol) {
    if(debug) cout<<blue<<"Histogramming Absolute polarization"<<normal<<endl; 
    TCanvas *cAbsPol = new TCanvas("cAbsPol", "absolute polarization", 0,0,450,600);
    cAbsPol->Divide(1,2);
    cAbsPol->cd(1);
    hPolRL->Draw("H");
    hPolRL->Fit("gaus");
    cAbsPol->cd(2);
    hPolLC->Draw("H");
    hPolLC->Fit("gaus");
    cAbsPol->SaveAs(destDir+Form("absPol_%d_%d.png",run1,run2));

    wmPolRL = wmNrPolRL/wmDrPolRL;
    wmPolLC = wmNrPolLC/wmDrPolLC;
    wmPolErRL = TMath::Sqrt(1.0/wmDrPolRL);
    wmPolErLC = TMath::Sqrt(1.0/wmDrPolLC);
    cout<<blue<<"the weighted mean of polRL is: "<<wmPolRL<<"+/-"<<wmPolErRL<<endl;
    cout<<"the weighted mean of polLC is: "<<wmPolLC<<"+/-"<<wmPolErLC<<normal<<endl;
  }

  if(bPolCompare) {
    if(debug) cout<<blue<<"Comparing Polarization from the two methods"<<normal<<endl; 
    //coordinates in TCanvas are in order :x,y of left top corner;x,y of right bottom corner
    TCanvas *polAvgP1 = new TCanvas("polAvgP1","Polarization trend P1",10,10,1000,900);
    TCanvas *cpolDiff = new TCanvas("cpolDiff","difference in evaluated Polarization",10,10,450,450);
    polAvgP1->Divide(1,2);
    polAvgP1->cd(1);
    TGraphErrors *grPolPlane1,*grRunletPolP1;
    TLegend *legPol= new TLegend(0.101,0.85,0.40,0.95);;

    //grPolPlane1 = new TGraphErrors(destDir+Form("polList_%d_%d.txt",run1,run2),"%lg %lg %lg");
    //grPolPlane1 = new TGraphErrors(runListLC.size(),runListLC.data(),polLC.data(),zero,polErLC.data());
    //grRunletPolP1 = new TGraphErrors(runListRL.size(),runListRL.data(),polRL.data(),zero,polErRL.data());
    grPolPlane1 = new TGraphErrors(newPolLC.size(),commonRuns.data(),newPolLC.data(),zero,newPolErLC.data());
    grRunletPolP1 = new TGraphErrors(newPolRL.size(),commonRuns.data(),newPolRL.data(),zero,newPolErRL.data());

    polAvgP1->GetPad(1)->SetGridx(1);
    grPolPlane1->SetMarkerStyle(kOpenCircle);
    grPolPlane1->SetMarkerColor(kBlue);
    grPolPlane1->SetLineColor(kBlue);
    grPolPlane1->SetFillColor(0);
    grPolPlane1->SetTitle("Polarization trend");
    grPolPlane1->GetXaxis()->SetTitle("Run number");
    grPolPlane1->GetXaxis()->SetLimits(run1-5,run2+5); 

    grPolPlane1->GetYaxis()->SetTitle("polarization (%)");
    grPolPlane1->GetYaxis()->SetDecimals(1);
    grPolPlane1->Draw("AP");

    grRunletPolP1->SetMarkerStyle(kOpenCircle);
    grRunletPolP1->SetMarkerColor(kRed);
    grRunletPolP1->SetLineColor(kRed);
    grRunletPolP1->Draw("P");

    legPol->AddEntry(grPolPlane1,"laser Cycle evaluation","lpe");
    legPol->AddEntry(grRunletPolP1,"runlet based evaluation","lpe");
    legPol->SetFillColor(0);
    legPol->Draw();

    polAvgP1->cd(2);
    polAvgP1->GetPad(2)->SetGridx(1);

    TGraphErrors *grPolDiff = new TGraphErrors((Int_t)polDiff.size(),commonRuns.data(),polDiff.data(),zero_1.data(),polDiffEr.data());
    grPolDiff->SetMarkerStyle(kOpenCircle);
    grPolDiff->SetMarkerColor(kBlack);
    grPolDiff->Draw("AP");
    grPolDiff->Fit(linearFit,"EM");
    grPolDiff->SetTitle();
    grPolDiff->GetXaxis()->SetLimits(run1-5,run2+5); 
    //grPolDiff->SetMaximum(1.5);
    //grPolDiff->SetMinimum(-2.0);

    grPolDiff->GetXaxis()->SetTitle("run number");
    grPolDiff->GetYaxis()->SetTitle("polRL - polLC");

    cpolDiff->cd();
    hPolDiff->SetLineColor(kBlack);
    hPolDiff->Draw("H");
    hPolDiff->Fit("gaus");
    cpolDiff->Modified();
    cpolDiff->Update();
    cpolDiff->SaveAs(destDir+Form("polDiff_%d_%d.png",run1,run2));
    //polAvgP1->SaveAs(Form("%s/%s/pol_%d_%d.png",pPath,webDirectory,run1,run2));
    //polAvgP1->SaveAs(Form("/w/hallc/qweak/narayan/APS2013Aprilpol_%d_%d.png",run1,run2));
    polAvgP1->SaveAs(destDir+Form("polAvgP1_%d_%d.png",run1,run2));
  }

  if(bPolErCompare) {
    if(debug) cout<<blue<<"Comparing Pol-error from the two methods"<<normal<<endl; 
    TCanvas *cPolErr = new TCanvas("cPolErr","polarization error",10,10,1000,600);
    cPolErr->Divide(2,1);

    cPolErr->cd(1);
    //hPolLCEr->SetFillColor(41);
    hPolLCEr->SetLineColor(kBlue);
    //hPolLCEr->SetTitle(Form("error on Polarization for Qweak phase-1 data"));
    //hPolLCEr->GetYaxis()->SetTitle("polarization error (statistical)");
    hPolLCEr->Draw("H");
    hPolLCEr->Fit("gaus");

    cPolErr->cd(2);
    //hPolLCEr->SetFillColor(41);
    hPolRLEr->SetLineColor(kRed);
    //hPolRLEr->SetTitle("");//Form("error on Polarization for Qweak phase-1 data"));
    //hPolRLEr->GetYaxis()->SetTitle("polarization error (statistical)");
    hPolRLEr->Draw("H");   
    hPolRLEr->Fit("gaus");    
    cPolErr->SaveAs(Form(destDir + "polEr_%d_%d.png",run1,run2));
  }

  if(bCECompare) {
    if(debug) cout<<blue<<"Comparing Compton edge from RL and LC methods"<<normal<<endl; 
    TCanvas *cComptEdge = new TCanvas("cComptEdge","Compton edge",10,10,1000,900);
    TCanvas *cCEDiff = new TCanvas("cCEDiff","difference in Compton edge",60,0,500,500);
    ///coordinates of legend are:x,y coordinates of left bottom corner; followed by x,y coordinates of top right corner
    TLegend *legCedge= new TLegend(0.1,0.1,0.30,0.25);;
    cout<<" runRange:"<<runRange<<", c1:"<<c1<<", c2:"<<c2<<endl;
    cComptEdge->Divide(1,2);
    cComptEdge->cd(1);

    ofstream fCedgeRunlet,fCedgeLasCyc;
    fCedgeRunlet.open("edgeRL_run1_run2.txt");
    for (Int_t r=0; r<(Int_t)commonRuns.size()-1; r++) {
      fCedgeRunlet<<Form("%5.0f\t%2.2f\t%.2f\n",commonRuns[r],newEdgeRL[r],newEdgeErRL[r]);
    }
    fCedgeRunlet.close();

    fCedgeLasCyc.open("edgeLC_run1_run2.txt");
    for (Int_t r=0; r<(Int_t)commonRuns.size()-1; r++) {
      fCedgeLasCyc<<Form("%5.0f\t%2.2f\t%.2f\n",commonRuns[r],newEdgeLC[r],newEdgeErLC[r]);
    }
    fCedgeLasCyc.close();

    TGraphErrors *grLasCycCedge = new TGraphErrors("edgeLC_run1_run2.txt","%lg %lg %lg");
    cComptEdge->GetPad(1)->SetGridx(1);
    grLasCycCedge->SetTitle();
    grLasCycCedge->SetMarkerStyle(kOpenSquare);
    grLasCycCedge->SetMarkerColor(kBlue);
    Double_t runletCedgeInRange[numbRuns];
    Int_t c4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runListRL[r] >= run1) && (runListRL[r] <= run2)) {
        c4++;
        runletCedgeInRange[c4] = comptEdgeRL[r];
      }
    }
    //TGraph *grRunletCedge = new TGraph(c4,lasCycRunnum,runletCedgeInRange);
    TGraphErrors *grRunletCedge = new TGraphErrors("edgeRL_run1_run2.txt","%lg %lg %lg");
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

    legCedge->AddEntry(grLasCycCedge,"LC evaluation","p");
    legCedge->AddEntry(grRunletCedge,"RL evaluation","p");
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
    grCEDiff->GetYaxis()->SetTitle("CE_LC - CE_RL");

    cCEDiff->cd();
    hCEDiff->Draw("H");
    hCEDiff->Fit("gaus");
    //cComptEdge->SaveAs(Form("/w/hallc/qweak/narayan/comptEdge_%d_%d.png",run1,run2));
    cComptEdge->SaveAs(destDir+Form("CE_%d_%d.png",run1,run2));
    cCEDiff->SaveAs(destDir+Form("CEDiff_%d_%d.png",run1,run2));    
  }

  if(bChiSqrCompare) {
    if(debug) cout<<blue<<"Comparing ChiSqr per Degree of freedom of RL & LC"<<normal<<endl; 
    TCanvas *cChiSqr = new TCanvas("cChiSqr","Chi-sqr / ndf",10,10,1000,500);
    TGraph *grChiSqr = new TGraph(c1,runListLC.data(),chiSqLC.data());
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
      if((runListRL[r] >= run1) && (runListRL[r] <= run2)) {
        c4++;
        runletChiSqrInRange[c4] = chiSqRL[r];
      }
    }
    TGraph *grRunletChiSqr = new TGraph(c4,runListRL.data(),chiSqRL.data());
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
    if(debug) cout<<blue<<"Histogramming ChiSqr per Degree of freedom of RL & LC"<<normal<<endl; 
    TCanvas *cChiSqr = new TCanvas("cChiSqrHist","chi-Sqr/ndf",10,10,1000,600);
    cChiSqr->Divide(2,1);

    cChiSqr->cd(1);
    hChiSqLC->SetLineColor(kBlue);
    hChiSqLC->Draw("H");
    hChiSqLC->Fit("gaus");
    cChiSqr->cd(2);
    hChiSqRL->SetLineColor(kRed);
    hChiSqRL->Draw("H");  
    hChiSqRL->Fit("gaus");
    cChiSqr->SaveAs(Form(destDir + "chiSqrHist_%d_%d.png",run1,run2));   
  }
  //cout<<red<<"\nTemporarily using polRL_radCor.info into fileLC variable\n"<<normal<<endl;//!Temp
  return (Int_t)polDiff.size();//the number of runs used
}

