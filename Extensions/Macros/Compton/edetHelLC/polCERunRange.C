#include <rootClass.h>
#include "comptonRunConstants.h"
///lets focus on the region between 23220 to 23530;
const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
//Int_t polRunRange(Int_t run1=23220, Int_t run2=23515)
Int_t polCERunRange(Int_t run1=runBegin, Int_t run2=runEnd)
{
  //gROOT->SetStyle("publication");
  Bool_t debug=1,debug1=0,debug2=0;
  Bool_t kPrintLC=0, kPrintRL=0; 
  Bool_t bAbsPol=1,bPolCompare=1,bPolErCompare=0,bCedgeCompare=1;
  Bool_t bChiSqrCompare=0,bChiSqrHist=0;
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  const Int_t numbRuns = runEnd - runBegin;//a non-variable is needed to set the size of an array length
  ifstream inFileRunsLC,inFileRL;
  ofstream outFileLC;
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www_27Jun14";///analysis throwing any cycle with beam trip
  //const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www_15Jul14";///taking all lasCyc,closer to RL analysis
  const char *myDir="/w/hallc/compton/users/narayan/my_scratch/www";///noise subtraction,pol5 fit of QED asym,fixed problems
  //TString destDir="/w/hallc/compton/users/narayan/svn/edetHelLC";//"/w/hallc/compton/users/narayan/my_scratch/www";
  const TString destDir="/w/hallc/compton/users/narayan/runletCompare/";
  //TString fileRL = Form("/u/home/narayan/acquired/vladas/modFiles/polRunletCE_%d_%d.dat",run1,run2);//file updated by Vladas on 24Apr2013
  //TString fileRL = destDir + Form("polRL_%d_%d_s39excluded.info",run1,run2);//file analyzed by A.Narayan
  TString fileRL = destDir + Form("polRL_%d_%d_allStrips.info",run1,run2);//file analyzed by A.Narayan
  //Form("/u/home/narayan/acquired/vladas/modFiles/polEffWidth_%d_%d.dat",run1,run2);//file updated by Vladas on 24Apr2013
  //polRunlet.open("/u/home/narayan/acquired/vladas/modFiles/runletFCEPolRun2.txt");
  //polRunlet.open(Form("/u/home/narayan/acquired/vladas/modFiles/runletPol_%d_%d.txt",run1,run2));
  TString fileLC = destDir + Form("polLC_%d_%d.txt",run1,run2);
  //TString fileLC = Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2);
  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10,dum11;
  inFileRL.open(fileRL.Data());

  Int_t runRange = run2-run1+1;
  TH1D *hPolRL = new TH1D("polRL", "polarization runlet analysis", runRange, 80, 95);
  TH1D *hPolLC = new TH1D("polLC", "polarization laser cycle analysis", runRange, 80, 95);
  TH1D *hPolLCEr = new TH1D("polEr laser Cycle", "polarization error laser cycle analysis", runRange, 0, 0.1);
  TH1D *hPolRLEr = new TH1D("polEr runlet based", "polarization error runlet based analysis", runRange, 0, 0.1);
  TH1D *hPolDiff = new TH1D("diff. in pol%", "difference in polarization between the two analysis", runRange, 0, 0.1);
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
  outFileLC.open(fileLC.Data());
  if (!outFileLC.is_open()) cout<<"could not open file to assimilate pol"<<endl;
  outFileLC<<"run\tabsPol\tpolEr\tchiSq/ndf\tCE\tCEEr\tss2\tss2Er\tpl\tgoodCyc"<<endl;
  for (Int_t r=run1; r<=run2; r++) {
    Double_t absPol[nPlanes],pol[nPlanes]={0.0},polEr[nPlanes]={0.0},chiSq[nPlanes],comptEdge[nPlanes],comptEdgeEr[nPlanes],effStrip[nPlanes],effStripEr[nPlanes];
    Int_t plane[nPlanes],NDF[nPlanes],runnum[nPlanes],goodCyc[nPlanes]; 
    filePre = Form(filePrefix,runnum,runnum);
    inFileRunsLC.open(Form("%s/%sAcPol.txt",myDir,filePre.Data()));
    if(inFileRunsLC.is_open()) {
      if(debug1) cout<<"opened file "<<Form("%s/%s/%sAcPol.txt",pPath,webDirectory,filePre.Data())<<endl;
      inFileRunsLC >>dum1>>dum2>>dum3>>dum4>>dum5>>dum6>>dum7>>dum8>>dum9>>dum10>>dum11;
      if(debug1) cout<<"runnum\tplane\tpol\tpolEr"<<endl;
      for (Int_t p=startPlane; p<endPlane; p++) {
        inFileRunsLC>>runnum[p]>>pol[p]>>polEr[p]>>chiSq[p]>>NDF[p]>>comptEdge[p]>>comptEdgeEr[p]>>effStrip[p]>>effStripEr[p]>>plane[p]>>goodCyc[p];
        if(debug1) cout<<r<<"\t"<<plane[p]<<"\t"<<pol[p]<<"\t"<<polEr[p]<<endl;
      }
      inFileRunsLC.close();
    } else if(debug1) cout<<"***couldn't open "<<Form("%s/%s/%sAcPol.txt",pPath,webDirectory,filePre.Data())<<endl;
    absPol[0] = fabs(pol[0]);//fabs(pol[0]);
    kPrintLC = (runnum[0]==r && polEr[0]<5.0 && absPol[0]<93.0 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0);// && (chiSq[0]/NDF[0]) <5.0);///often chiSqr/NDF is not on expected lines
    //kPrintLC = (polEr[0]<4.0 && absPol[0]<95.0 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <6.0);
    //kPrintLC = (runnum[0]==r && polEr[0]<1.5 && absPol[0]<91.5 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <5.0);
    if (kPrintLC) {
      outFileLC<<Form("%5.0f\t%2.2f\t%1.2f\t%.2f\t%2.2f\t%2.2f\t%.3f\t%.3f\t%d\t%d\n",(Double_t)r,absPol[0],polEr[0],chiSq[0]/NDF[0],comptEdge[0],comptEdgeEr[0],effStrip[0],effStripEr[0],plane[0],goodCyc[0]);
      runListLC.push_back(r);
      polLC.push_back(absPol[0]);
      polErLC.push_back(polEr[0]);
      comptEdgeLC.push_back(comptEdge[0]);
      edgeErLC.push_back(comptEdgeEr[0]);
      chiSqLC.push_back(chiSq[0]/NDF[0]);
      c1++;///remains as a count of how many 'good' runs we have
    }
  }
  outFileLC.close();
  cout<<red<<"found valid "<<runListLC.size()<<" analyzed runs in runListLC"<<normal<<endl;

  Double_t zero[numbRuns]={0};
  Int_t c2=0;
  Double_t tempRun,tempPol,tempPolEr,tempChiSq,tempCE,tempCEEr;
  Int_t tempNDF;
  Double_t dummy2,dummy3,dummy4,dummy5;
  std::vector<Double_t > NDF_RL;
  if(inFileRL.is_open()) { //this is available only for plane 1
    while(inFileRL.good()) {
      inFileRL>>tempRun>>tempPol>>tempPolEr>>tempChiSq>>tempNDF>>tempCE>>tempCEEr>>dummy2>>dummy3>>dummy4>>dummy5;
      ///when reading from RL file created by me
      kPrintRL = (tempPolEr!=0.0 && tempPolEr<5.0);
      if (kPrintRL) {
      runListRL.push_back(tempRun);
      polRL.push_back(fabs(tempPol));
      polErRL.push_back(tempPolEr);
      comptEdgeRL.push_back(tempCE);
      NDF_RL.push_back(tempNDF);
      chiSqRL.push_back(tempChiSq/tempNDF);
      edgeErRL.push_back(tempCEEr);
      //inFileRL>>runListRL[c2]>>polRL[c2]>>polErRL[c2]>>chiSqRL[c2]>>NDF_RL[c2]>>comptEdgeRL[c2]>>edgeErRL[c2]>>dummy2>>dummy3>>dummy4>>dummy5;///when reading from RL file created by me
      c2++;
      }
      if(debug2) cout<<runListRL[c2]<<"\t"<<polRL[c2]<<"\t"<<polErRL[c2]<<"\t"<<comptEdgeRL[c2]<<"\t"<<edgeErRL[c2]<<endl;
      if(inFileRL.eof()) break;
    }
    inFileRL.close();
  } else cout<<"*** Alert: could not open runlet based pol file"<<endl;
  cout<<red<<"found valid "<<c2<<" analyzed runs in runListRL"<<normal<<endl;

  cout<<" runRange:"<<runRange<<", c1:"<<c1<<", c2:"<<c2<<endl;

  std::vector<Double_t> commonRuns,polDiff,CEDiff,polDiffEr,CEDiffEr,zero_1;
  for(Int_t r=0; r<(Int_t)runListRL.size(); r++) {
    //Bool_t kFound = 0;
    for(Int_t r2=0; r2<(Int_t)runListLC.size(); r2++) {
      if(runListRL[r] == runListLC[r2]) { 
        //printf("%f\t%f\t%f\n",runListRL[r],polRL[r],polLC[r2]);//temp !
        if((comptEdgeRL[r]-comptEdgeLC[r2])>1.0) {
          cout<<green<<"CE was off by >1.0 strip for run "<<runListLC[r2]<<normal<<endl;
        } else if(fabs(polRL[r] - polLC[r2])>2.0) {
          cout<<red<<runListRL[r]<<" in RL: "<<polRL[r]<<"+/-"<<polErRL[r]<<"; in LC: "<<polLC[r2]<<"+/-"<<polErLC[r2]<<normal<<endl;
        } else {
          //kFound = 1;
          polDiff.push_back(polLC[r2] - polRL[r]);
          commonRuns.push_back(runListRL[r2]);
          polDiffEr.push_back(TMath::Sqrt(polErLC[r2]*polErLC[r2] + polErRL[r]*polErRL[r]));
          zero_1.push_back(0.0);          
          CEDiff.push_back(comptEdgeLC[r2]-comptEdgeRL[r]);
          CEDiffEr.push_back(edgeErLC[r2]+edgeErRL[r]);

          hPolRL->Fill(polRL[r],1.0/(polErRL[r]*polErRL[r]));
          hPolLC->Fill(polLC[r2],1.0/(polErLC[r2]*polErLC[r2]));
          hPolDiff->Fill(polDiff.back());
          hPolLCEr->Fill(polErLC[r2]);
          hPolRLEr->Fill(polErRL[r]);
          hCEDiff->Fill(CEDiff.back());
          hChiSqLC->Fill(chiSqLC[r2]);
          hChiSqRL->Fill(chiSqRL[r]);
          break;
        }
      }
    }
    //if(!kFound) cout<<magenta<<"run "<<runListRL[r]<<" from RL not found in LC"<<normal<<endl;
  }
  cout<<green<<c3<<" entries in runlet file had a run with null polarization"<<normal<<endl;

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
    cAbsPol->SaveAs(destDir+Form("/absPol_%d_%d.png",run1,run2));
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

    //grPolPlane1 = new TGraphErrors(destDir+Form("/polList_%d_%d.txt",run1,run2),"%lg %lg %lg");
    grPolPlane1 = new TGraphErrors(runListLC.size(),runListLC.data(),polLC.data(),zero,polErLC.data());
    grRunletPolP1 = new TGraphErrors(runListRL.size(),runListRL.data(),polRL.data(),zero,polErRL.data());

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
    grPolDiff->SetMaximum(3.2); 
    grPolDiff->SetMinimum(-3.2);

    grPolDiff->GetXaxis()->SetTitle("run number");
    grPolDiff->GetYaxis()->SetTitle("difference in polarization");

    cpolDiff->cd();
    hPolDiff->SetLineColor(kBlack);
    hPolDiff->Draw("H");
    hPolDiff->Fit("gaus");
    cpolDiff->Modified();
    cpolDiff->Update();
    cpolDiff->SaveAs(destDir+Form("/polDiff_%d_%d.png",run1,run2));
    //polAvgP1->SaveAs(Form("%s/%s/pol_%d_%d.png",pPath,webDirectory,run1,run2));
    //polAvgP1->SaveAs(Form("/w/hallc/qweak/narayan/APS2013Aprilpol_%d_%d.png",run1,run2));
    polAvgP1->SaveAs(destDir+Form("/polAvgP1_%d_%d.png",run1,run2));
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
    cPolErr->SaveAs(Form(destDir + "/polEr_%d_%d.png",run1,run2));
  }

  if(bCedgeCompare) {
    if(debug) cout<<blue<<"Comparing Compton edge from RL and LC methods"<<normal<<endl; 
    TCanvas *cComptEdge = new TCanvas("cComptEdge","Compton edge",10,10,1000,900);
    TCanvas *cCEDiff = new TCanvas("cCEDiff","difference in Compton edge",60,0,500,500);
    ///coordinates of legend are:x,y coordinates of left bottom corner; followed by x,y coordinates of top right corner
    //TLegend *legCedge= new TLegend(0.12,0.95,0.30,0.25);;
    TLegend *legCedge= new TLegend(0.1,0.1,0.30,0.25);;
    cout<<" runRange:"<<runRange<<", c1:"<<c1<<", c2:"<<c2<<endl;
    cComptEdge->Divide(1,2);
    cComptEdge->cd(1);

    ofstream fCedgeRunlet,fCedgeLasCyc;
    fCedgeRunlet.open("edgeRL_run1_run2.txt");
    for (Int_t r=0; r<c2; r++) {
      fCedgeRunlet<<Form("%5.0f\t%2.2f\t%.2f\n",runListRL[r],comptEdgeRL[r],edgeErRL[r]);
    }
    fCedgeRunlet.close();

    fCedgeLasCyc.open("edgeLC_run1_run2.txt");
    for (Int_t r=0; r<c1; r++) {
      //fCedgeLasCyc<<Form("%5.0f\t%2.2f\n",lasCycRunnum[r],lasCycComptEdge[r]);
      fCedgeLasCyc<<Form("%5.0f\t%2.2f\t%.2f\n",runListLC[r],comptEdgeLC[r],edgeErLC[r]);
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
    //grCEDiff->GetXaxis()->SetTitleSize(0.05);
    grCEDiff->SetMaximum(3.2); 
    grCEDiff->SetMinimum(-3.2);

    //grCEDiff->GetYaxis()->SetTitleSize(0.05);
    grCEDiff->GetXaxis()->SetTitle("run number");
    grCEDiff->GetYaxis()->SetTitle("CE_LC - CE_RL");

    cCEDiff->cd();
    hCEDiff->Draw("H");
    hCEDiff->Fit("gaus");
    //cComptEdge->SaveAs(Form("/w/hallc/qweak/narayan/comptEdge_%d_%d.png",run1,run2));
    cComptEdge->SaveAs(destDir+Form("/CE_%d_%d.png",run1,run2));
    cCEDiff->SaveAs(destDir+Form("/CEDiff_%d_%d.png",run1,run2));    
  }

  if(bChiSqrCompare) {
    if(debug) cout<<blue<<"Comparing ChiSqr per Degree of freedom of RL & LC"<<normal<<endl; 
    TCanvas *cChiSqr = new TCanvas("cChiSqr","Chi-sqr / ndf",10,10,1000,500);
    TGraph *grChiSqr = new TGraph(c1,runListLC.data(),chiSqLC.data());
    TLegend *legCedge= new TLegend(0.6,0.75,0.9,0.9);;
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

    cChiSqr->SaveAs(Form(destDir + "/chiSqr_%d_%d.png",run1,run2));
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
    cChiSqr->SaveAs(Form(destDir + "/chiSqrHist_%d_%d.png",run1,run2));   
  }

  return (run2 - run1+1);//the number of runs processed
}

