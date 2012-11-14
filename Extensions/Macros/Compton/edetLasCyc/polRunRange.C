#include <rootClass.h>
#include "comptonRunConstants.h"
Int_t polRunRange(Int_t run1, Int_t run2)
{
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  const Int_t runBegin = 22659, runEnd = 25546;
  const Int_t numbRuns = runEnd - runBegin - 1110;//temporarily added to avoid the excluded runs by vladas
  ifstream poltext,polRunlet,polLasCyc;
  ofstream polAll;
  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10;
  Bool_t debug=0,debug1=0,debug2=0,kPlsPrint=0;
  Bool_t bPolCompare=0,bPolErCompare=0,bCedgeCompare=1;
  Int_t runRange = run2-run1+1;
  Int_t count1=0;
  TH1D *hPolEr = new TH1D("polEr laser Cycle", "polarization error laser cycle analysis", numbRuns, 0, 0.1);
  TH1D *hRunletPolEr = new TH1D("polEr runlet based", "polarization error runlet based analysis", numbRuns, 0, 0.1);
  hPolEr->SetBit(TH1::kCanRebin);
  hRunletPolEr->SetBit(TH1::kCanRebin);

  //polAll.open(Form("%s/%s/polList_%d_%d.txt",pPath,webDirectory,run1,run2)); 100% disk space full
  //polAll.open(Form("$MYLOCAL/polList_%d_%d.txt",run1,run2));
  polAll.open(Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2));
  if (!polAll.is_open()) cout<<"could not open file to assimilate pol"<<endl;
  //cout<<Form("%s/%s/polList_%d_%d.txt",pPath,webDirectory,run1,run2)<<endl;
  
  for (Int_t r=run1; r<=run2; r++) {
    Double_t absPol[nPlanes],pol[nPlanes]={0.0},polEr[nPlanes]={0.0},chiSq[nPlanes],comptEdge[nPlanes],comptEdgeEr[nPlanes],effStrip[nPlanes],effStripEr[nPlanes];
    Int_t plane[nPlanes],NDF[nPlanes],runnum[nPlanes]; 
    TString filePrefix= Form("run_%d/edetLasCyc_%d_",r,r);
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
  Double_t lasCycPol[runRange],lasCycPolEr[runRange],lasCycChiSq[runRange],lasCycComptEdge[runRange],lasCycEffStrip[runRange];
  Double_t lasCycEffStripEr[runRange],lasCycPlane[runRange],lasCycRunnum[runRange];
  Int_t count2=0;
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

  polRunlet.open("/u/home/narayan/acquired/vladas/modFiles/runletFCEPolRun2.txt");
  Double_t runletPol[numbRuns]={0.0},runletPolEr[numbRuns]={0.0},runletChiSq[numbRuns],runletComptEdge[numbRuns],runletEffStrip[numbRuns];
  Double_t runletEffStripEr[numbRuns],runletPlane[numbRuns],zero[numbRuns]={0},runletRunnum[numbRuns]={},IHWP[numbRuns];

  if(polRunlet.is_open()) { //this is available only for plane 1
    for (Int_t r=0; r<numbRuns; r++) {
      polRunlet>>runletRunnum[r]>>runletPol[r]>>runletPolEr[r]>>runletComptEdge[r]>>runletChiSq[r]>>runletPlane[r]>>IHWP[r]>>runletEffStrip[r]>>runletEffStripEr[r];
      if(debug1) cout<<runletRunnum[r]<<"\t"<<runletPol[r]<<"\t"<<runletPolEr[r]<<endl;
    }
  } else cout<<"*** Alert: could not open runlet based pol.txt file ~/acquired/vladas/modFiles/runletFCEPolRun2.txt"<<endl;
  polRunlet.close();

  if(bPolCompare) {
    TCanvas *polAvgP1 = new TCanvas("polAvgP1","Avg Polarization Plane 1",10,10,1000,500);
    // polAvgP1->Divide(1,2);
    // polAvgP1->cd(1);
    TGraphErrors *grPolPlane1,*grRunletPolP1;
    grPolPlane1 = new TGraphErrors(Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2),"%lg %lg %lg");
    //grRunletPolP1 = new TGraphErrors(Form("~/acquired/vladas/modFiles/runletPol_22659_23001.txt"),"%lg %lg %lg");
    grRunletPolP1 = new TGraphErrors(numbRuns,runletRunnum,runletPol,zero,runletPolEr);

    //TBox *bCedge1;//,*bCedge2,*bCedge3;
    //bCedge1 = new TBox(run1,60,run2,96);
    //bCedge2 = new TBox(24062,60,24400,96);
    //bCedge3 = new TBox(24062,60,24400,96);
    //polAvgP1->GetPad(1)->SetGridx(1);
    polAvgP1->SetGridx(1);
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

    //polAvgP1->cd(2);
    //polAvgP1->GetPad(2)->SetGridx(1);
    grRunletPolP1->SetMarkerStyle(kOpenCircle);
    grRunletPolP1->SetMarkerColor(kRed);
    grRunletPolP1->SetLineColor(kRed);
    // grRunletPolP1->SetFillColor(0);
    // grRunletPolP1->SetTitle("Compton electron detector: Runlet based analysis");
    // grRunletPolP1->GetXaxis()->SetLimits(run1+5,run2+5); 
    // grRunletPolP1->GetXaxis()->SetTitle("Run number");
    // grRunletPolP1->GetYaxis()->SetTitle("polarization (%)");
    // grRunletPolP1->Draw("AP");
    grRunletPolP1->Draw("P");

    //polAvgP1->SaveAs(Form("%s/%s/pol_%d_%d.png",pPath,webDirectory,run1,run2));
    polAvgP1->SaveAs(Form("/w/hallc/qweak/narayan/pol_%d_%d.png",run1,run2));
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
    TGraphErrors *grCedge = new TGraphErrors(count2,lasCycRunnum,lasCycComptEdge);
    TLegend *legCedge= new TLegend(0.101,0.75,0.43,0.9);;

    grCedge->SetTitle();
    grCedge->SetMarkerStyle(kOpenSquare);
    grCedge->SetTitle("Compton edge found by signal over noise comparition");
    grCedge->GetXaxis()->SetTitle("Run number");
    grCedge->GetYaxis()->SetTitle("Compton edge (strip number)");
    grCedge->SetMarkerColor(kBlue);
    grCedge->SetLineColor(kBlue);
    grCedge->Draw("AP");
    Double_t runletCedgeInRange[runRange];
    Int_t count3=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runletRunnum[r] >= run1) && (runletRunnum[r] <= run2)) {
	count3++;
	runletCedgeInRange[count3] = runletComptEdge[r];
      }
    }
    //cComptEdge->cd(2);
    TGraph *grRunletCedge = new TGraph(count3,lasCycRunnum,runletCedgeInRange);
    grRunletCedge->SetMarkerStyle(kOpenSquare);//(kFullSquare);
    grRunletCedge->SetMarkerColor(kRed);
    grRunletCedge->SetLineColor(kRed);
    grRunletCedge->Draw("P");

    legCedge->AddEntry(grCedge,"laser Cycle evaluation","p");
    legCedge->AddEntry(grRunletCedge,"runlet based evaluation","p");
    legCedge->SetFillColor(0);
    legCedge->Draw();

    cComptEdge->SaveAs(Form("/w/hallc/qweak/narayan/comptEdge_%d_%d.png",run1,run2));
  }
  return (run2 - run1+1);//the number of runs processed
}
