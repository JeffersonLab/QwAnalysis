#include <rootClass.h>
#include "comptonRunConstants.h"
//Int_t polRunRange(Int_t run1=23220, Int_t run2=23515)
Int_t polRunRange(Int_t run1=23220, Int_t run2=23530)
{
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  ///lets focus on the region between 23220 to 23530;
  //const Int_t runBegin = 23200, runEnd = 24150;//runBegin = 22659, runEnd = 25546;
  //const Int_t runBegin = 23220, runEnd = 23514;//runBegin = 22659, runEnd = 25546;
  const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
  const Int_t numbRuns = runEnd - runBegin;// - 1110;//temporarily added to avoid the excluded runs by vladas
  ifstream poltext,polRunlet,polLasCyc;
  ofstream polAll;
  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10;
  Bool_t debug=0,debug1=0,debug2=0,kPlsPrint=0;
  Bool_t bPolCompare=1,bPolErCompare=1,bCedgeCompare=1,bChiSqrCompare=1;
  Int_t runRange = run2-run1+1;
  Int_t count1=0;
  TH1D *hPolEr = new TH1D("polEr laser Cycle", "polarization error laser cycle analysis", numbRuns, 0, 0.1);
  TH1D *hRunletPolEr = new TH1D("polEr runlet based", "polarization error runlet based analysis", numbRuns, 0, 0.1);
  TH1D *hPolDiff = new TH1D("diff. in pol%", "difference in polarization between the two analysis", numbRuns, 0, 0.1);
  hPolEr->SetBit(TH1::kCanRebin);
  hRunletPolEr->SetBit(TH1::kCanRebin);
  hPolDiff->SetBit(TH1::kCanRebin);

  Double_t lasCycPol[runRange],lasCycPolEr[runRange],lasCycChiSq[runRange],lasCycComptEdge[runRange],lasCycEffStrip[runRange];
  Double_t lasCycEffStripEr[runRange],lasCycPlane[runRange],lasCycRunnum[runRange];
  Int_t count2=0;
  //polAll.open(Form("%s/%s/polList_%d_%d.txt",pPath,webDirectory,run1,run2)); 100% disk space full
  //polAll.open(Form("$MYLOCAL/polList_%d_%d.txt",run1,run2));
  polAll.open(Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2));
  if (!polAll.is_open()) cout<<"could not open file to assimilate pol"<<endl;
  //cout<<Form("%s/%s/polList_%d_%d.txt",pPath,webDirectory,run1,run2)<<endl;
  
  for (Int_t r=run1; r<=run2; r++) {
    Double_t absPol[nPlanes],pol[nPlanes]={0.0},polEr[nPlanes]={0.0},chiSq[nPlanes],comptEdge[nPlanes],comptEdgeEr[nPlanes],effStrip[nPlanes],effStripEr[nPlanes];
    Int_t plane[nPlanes],NDF[nPlanes],runnum[nPlanes]; 
    filePrefix= Form("run_%d/edetLasCyc_%d_",r,r);
    poltext.open(Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data()));
    if(poltext.is_open()) {
      if(debug) cout<<"opened file "<<Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data())<<endl;
      poltext >>dum1>>dum2>>dum3>>dum4>>dum5>>dum6>>dum7>>dum8>>dum9>>dum10;
      if(debug) cout<<"plane\tpol\tpolEr"<<endl;
      for (Int_t p=startPlane; p<endPlane; p++) {
	poltext>>runnum[p]>>pol[p]>>polEr[p]>>chiSq[p]>>NDF[p]>>comptEdge[p]>>comptEdgeEr[p]>>effStrip[p]>>effStripEr[p]>>plane[p];
	if(debug) cout<<plane[p]<<"\t"<<pol[p]<<"\t"<<polEr[p]<<endl;
      }
      poltext.close();
    } else if(debug) cout<<"*** Alert: could not open "<<Form("%s/%s/%spol.txt",pPath,webDirectory,filePrefix.Data())<<endl;
    absPol[0] = fabs(pol[0]);
    kPlsPrint = (runnum[0]==r && polEr[0]<3.0 && absPol[0]<95.0 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <5.0);
    if (kPlsPrint) {
      polAll<<Form("%5.0f\t%2.2f\t%1.2f\t%.2f\t%2.2f\t%.3f\t%.3f\t%d\n",(Float_t)r,absPol[0],polEr[0],chiSq[0]/NDF[0],comptEdge[0],effStrip[0],effStripEr[0],plane[0]);
      hPolEr->Fill(polEr[0]);
    }
    count1++;
  }
  polAll.close();

  polLasCyc.open(Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2));
  if(polLasCyc.is_open()) { //this is available only for plane 1
    //    for (Int_t r=0; r<runRange; r++) {
    while(polLasCyc.good()) {// break;
      polLasCyc>>lasCycRunnum[count2]>>lasCycPol[count2]>>lasCycPolEr[count2]>>lasCycChiSq[count2]>>lasCycComptEdge[count2]>>lasCycEffStrip[count2]>>lasCycEffStripEr[count2]>>lasCycPlane[count2];
      if(debug2) cout<<lasCycRunnum[count2]<<"\t"<<lasCycPol[count2]<<"\t"<<lasCycPolEr[count2]<<"\t"<<count2<<endl;
      if(polLasCyc.eof()) break;
      count2++;//by placing the counter here, I avoid its extraneous addition at the end
    }
    polLasCyc.close();
    //    }
  } else cout<<"*** Alert: could not open lasCyc based file /w/hallc/qweak/narayan/polList_* "<<endl;

  //polRunlet.open("/u/home/narayan/acquired/vladas/modFiles/runletFCEPolRun2.txt");
  polRunlet.open("/u/home/narayan/acquired/vladas/modFiles/runletPol_23220_23530.txt");
  Double_t runletPol[numbRuns]={0.0},runletPolEr[numbRuns]={0.0},runletChiSq[numbRuns],runletComptEdge[numbRuns],runletEffStrip[numbRuns];
  Double_t runletEffStripEr[numbRuns],runletPlane[numbRuns],zero[numbRuns]={0},IHWP[numbRuns];//runletRunnum[numbRuns]={},
  std::vector <Double_t> runletRunnum;
  Int_t count3=0;
  if(polRunlet.is_open()) { //this is available only for plane 1
    //    for (Int_t r=0; r<numbRuns; r++) {
    while(polRunlet.good()) {
      runletRunnum.push_back(0.0);
      polRunlet>>runletRunnum[count3]>>runletPol[count3]>>runletPolEr[count3]>>runletComptEdge[count3]>>runletChiSq[count3]>>runletPlane[count3]>>IHWP[count3]>>runletEffStrip[count3]>>runletEffStripEr[count3];
      if(debug1) cout<<runletRunnum[count3]<<"\t"<<runletPol[count3]<<"\t"<<runletPolEr[count3]<<endl;
      if(polRunlet.eof()) break;
      count3++;
    }
    polRunlet.close();
    //    }
  } else cout<<"*** Alert: could not open runlet based pol.txt file ~/acquired/vladas/modFiles/runletFCEPolRun2.txt"<<endl;

  if(bPolCompare) {
    TCanvas *polAvgP1 = new TCanvas("polAvgP1","Avg Polarization Plane 1",10,10,1000,900);
    TCanvas *cpolDiff = new TCanvas("cpolDiff","difference in evaluated Polarization",10,10,600,600);
    Double_t polDiff[runRange];
    polAvgP1->Divide(1,2);
    polAvgP1->cd(1);
    TGraphErrors *grPolPlane1,*grRunletPolP1;
    grPolPlane1 = new TGraphErrors(Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2),"%lg %lg %lg");
    //grRunletPolP1 = new TGraphErrors(Form("~/acquired/vladas/modFiles/runletPol_22659_23001.txt"),"%lg %lg %lg");
    grRunletPolP1 = new TGraphErrors(numbRuns,runletRunnum.data(),runletPol,zero,runletPolEr);

    //TBox *bCedge1;//,*bCedge2,*bCedge3;
    //bCedge1 = new TBox(run1,60,run2,96);
    //bCedge2 = new TBox(24062,60,24400,96);
    //bCedge3 = new TBox(24062,60,24400,96);
    polAvgP1->GetPad(1)->SetGridx(1);
    //polAvgP1->SetGridx(1);
    grPolPlane1->SetMarkerStyle(kOpenCircle);
    grPolPlane1->SetMarkerColor(kBlue);
    grPolPlane1->SetLineColor(kBlue);
    grPolPlane1->SetFillColor(0);
    //grPolPlane1->SetTitle("Compton electron detector: Laser cycle based analysis");
    grPolPlane1->SetTitle();
    grPolPlane1->GetXaxis()->SetTitleSize(0.05);
    grPolPlane1->GetXaxis()->SetTitleOffset(0.84);
    //grPolPlane1->GetXaxis()->CenterTitle();
    grPolPlane1->GetXaxis()->SetLabelSize(0.05);
    grPolPlane1->GetXaxis()->SetTitle("Run number");
    grPolPlane1->GetXaxis()->SetLimits(run1+5,run2+5); 

    grPolPlane1->GetYaxis()->SetTitleSize(0.05);
    grPolPlane1->GetYaxis()->SetTitleOffset(0.9);
    grPolPlane1->GetYaxis()->CenterTitle();
    grPolPlane1->GetYaxis()->SetLabelSize(0.06);
    grPolPlane1->GetYaxis()->SetTitle("polarization (%)");
    grPolPlane1->GetYaxis()->SetDecimals(1);
    grPolPlane1->Draw("AP");
    //bCedge1->SetFillStyle(0);  
    //bCedge1->SetLineColor(kBlue); 
    //bCedge1->Draw("0 l");

    grRunletPolP1->SetMarkerStyle(kOpenCircle);
    grRunletPolP1->SetMarkerColor(kRed);
    grRunletPolP1->SetLineColor(kRed);
    grRunletPolP1->Draw("P");

    Double_t check=0.0;
    polAvgP1->cd(2);
    polAvgP1->GetPad(2)->SetGridx(1);
    for(Int_t r=0; r<count2; r++) {
      polDiff[r]= 12.0;
      check = lasCycRunnum[r];
      for(Int_t r2=0; r2<count3; r2++) {
	if(check == runletRunnum[r2]) { 
	  polDiff[r]= (lasCycPol[r] - runletPol[r2]);
	  break;
	}
      }
    }

    TGraph *grPolDiff = new TGraph(count2,lasCycRunnum,polDiff);
    grPolDiff->SetMarkerStyle(kOpenCircle);
    grPolDiff->SetMarkerColor(kBlack);
    grPolDiff->Draw("AP");
    grPolDiff->SetTitle();
    grPolDiff->GetXaxis()->SetLimits(run1+5,run2+5); 
    grPolDiff->GetXaxis()->SetTitleSize(0.05);
    grPolDiff->SetMaximum(3.2); 
    grPolDiff->SetMinimum(-3.2);

    grPolDiff->GetYaxis()->SetTitleSize(0.05);
    grPolDiff->GetXaxis()->SetTitle("run number");
    grPolDiff->GetYaxis()->SetTitle("difference in polarization");
   
    cpolDiff->cd();
    for(Int_t r=0; r<count2; r++) {
      if(polDiff[r] != 12.0) hPolDiff->Fill(polDiff[r]);
    }
    hPolDiff->SetLineColor(kBlack);
    hPolDiff->Draw("H");
    cpolDiff->SaveAs(Form("/w/hallc/qweak/narayan/polDiff_%d_%d.png",run1,run2));
    //polAvgP1->SaveAs(Form("%s/%s/pol_%d_%d.png",pPath,webDirectory,run1,run2));
    polAvgP1->SaveAs(Form("/w/hallc/qweak/narayan/polCompare_%d_%d.png",run1,run2));
  }

  if(bPolErCompare) {
    TCanvas *cPolErr = new TCanvas("cPolErr","polarization error",10,10,1000,600);
    cPolErr->Divide(2,1);

    cPolErr->cd(1);
    //hPolEr->SetFillColor(41);
    hPolEr->SetLineColor(kBlue);
    //hPolEr->SetTitle(Form("error on Polarization for Qweak phase-1 data"));
    //hPolEr->GetYaxis()->SetTitle("polarization error (statistical)");
    hPolEr->Draw("H");
   
    for (Int_t r=0; r<numbRuns; r++) {
      if((runletRunnum[r] >= run1) && (runletRunnum[r] <= run2))  hRunletPolEr->Fill(runletPolEr[r]);
    }
    cPolErr->cd(2);
    //hPolEr->SetFillColor(41);
    hRunletPolEr->SetLineColor(kRed);
    //hRunletPolEr->SetTitle("");//Form("error on Polarization for Qweak phase-1 data"));
    //hRunletPolEr->GetYaxis()->SetTitle("polarization error (statistical)");
    hRunletPolEr->Draw("H");   
    cPolErr->SaveAs(Form("/w/hallc/qweak/narayan/polEr_%d_%d.png",run1,run2));
  }

  if(bCedgeCompare) {
    TCanvas *cComptEdge = new TCanvas("cComptEdge","Compton edge",10,10,1000,500);
    //cComptEdge->Divide(1,2);
    //cComptEdge->cd(1);
    //TGraphErrors *grCedge = new TGraphErrors(count2,lasCycRunnum,lasCycComptEdge);
    TLegend *legCedge= new TLegend(0.101,0.75,0.43,0.9);;
    cout<<" numbRuns:"<<numbRuns<<", count1:"<<count1<<", count2:"<<count2<<", count3:"<<count3<<endl;

    ofstream fCedgeRunlet,fCedgeLasCyc;
    fCedgeRunlet.open("CedgeRunlet_run1_run2.txt");
    for (Int_t r=0; r<count3; r++) {
      fCedgeRunlet<<Form("%5.0f\t%2.2f\n",runletRunnum[r],runletComptEdge[r]);
    }
    fCedgeRunlet.close();

    fCedgeLasCyc.open("CedgeLasCyc_run1_run2.txt");
    for (Int_t r=0; r<count2; r++) {
      fCedgeLasCyc<<Form("%5.0f\t%2.2f\n",lasCycRunnum[r],lasCycComptEdge[r]);
    }
    fCedgeLasCyc.close();

    TGraph *grLasCycCedge = new TGraph("CedgeLasCyc_run1_run2.txt","%lg %lg");
    cComptEdge->SetGridx(1);
    grLasCycCedge->SetTitle();
    grLasCycCedge->SetMarkerStyle(kOpenSquare);
    grLasCycCedge->SetMarkerColor(kBlue);
    Double_t runletCedgeInRange[runRange];
    Int_t count4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runletRunnum[r] >= run1) && (runletRunnum[r] <= run2)) {
	count4++;
	runletCedgeInRange[count4] = runletComptEdge[r];
      }
    }
    //TGraph *grRunletCedge = new TGraph(count4,lasCycRunnum,runletCedgeInRange);
    TGraph *grRunletCedge = new TGraph("CedgeRunlet_run1_run2.txt","%lg %lg");
    grRunletCedge->SetMarkerStyle(kFullSquare);//(kFullSquare);
    grRunletCedge->SetMarkerColor(kRed);
    grRunletCedge->SetMarkerSize(0.7);

    TMultiGraph *grCedge = new TMultiGraph();
    grCedge->Add(grRunletCedge);
    grCedge->Add(grLasCycCedge);
    grCedge->Draw("AP");
    grCedge->SetTitle("Compton edge found by signal over noise comparition");
    grCedge->GetXaxis()->SetTitle("Run number");
    grCedge->GetYaxis()->SetTitle("Compton edge (strip number)");
    grCedge->GetXaxis()->SetLimits(run1+5,run2+5); 

    legCedge->AddEntry(grLasCycCedge,"laser Cycle evaluation","p");
    legCedge->AddEntry(grRunletCedge,"runlet based evaluation","p");
    legCedge->SetFillColor(0);
    legCedge->Draw();

    cComptEdge->SaveAs(Form("/w/hallc/qweak/narayan/comptEdge_%d_%d.png",run1,run2));
  }


  if(bChiSqrCompare) {
    TCanvas *cChiSqr = new TCanvas("cChiSqr","Chi-sqr / ndf",10,10,1000,500);
    TGraphErrors *grChiSqr = new TGraphErrors(count2,lasCycRunnum,lasCycChiSq);
    TLegend *legCedge= new TLegend(0.6,0.75,0.9,0.9);;
    cChiSqr->SetGridx(1);
    grChiSqr->SetTitle();
    grChiSqr->SetMarkerStyle(kOpenCircle);
    grChiSqr->SetTitle("chiSqr/ndf in asymmetry fit by laser Cyc evaluation");
    grChiSqr->GetXaxis()->SetTitle("Run number");
    grChiSqr->GetYaxis()->SetTitle("chiSqr / ndf");
    grChiSqr->SetMarkerColor(kBlue);
    grChiSqr->SetLineColor(kBlue);
    grChiSqr->GetXaxis()->SetLimits(run1+5,run2+5); 
    grChiSqr->Draw("AP");
    Double_t runletChiSqrInRange[runRange];
    Int_t count4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runletRunnum[r] >= run1) && (runletRunnum[r] <= run2)) {
	count4++;
	runletChiSqrInRange[count4] = runletChiSq[r];
      }
    }
    TGraph *grRunletChiSqr = new TGraph(count4,lasCycRunnum,runletChiSqrInRange);
    grRunletChiSqr->SetMarkerStyle(kOpenCircle);
    grRunletChiSqr->SetMarkerColor(kRed);
    grRunletChiSqr->SetLineColor(kRed);
    grRunletChiSqr->Draw("P");

    legCedge->AddEntry(grChiSqr,"laser Cycle evaluation","p");
    legCedge->AddEntry(grRunletChiSqr,"runlet based evaluation","p");
    legCedge->SetFillColor(0);
    legCedge->Draw();

    cChiSqr->SaveAs(Form("/w/hallc/qweak/narayan/chiSqr_%d_%d.png",run1,run2));
  }
  return (run2 - run1+1);//the number of runs processed
}

