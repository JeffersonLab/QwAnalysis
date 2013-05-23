#include <rootClass.h>
#include "comptonRunConstants.h"
//Int_t polRunRange(Int_t run1=23220, Int_t run2=23515)
Int_t polCERunRange(Int_t run1=23220, Int_t run2=23530)
{
  Bool_t debug=1,debug1=0,kPlsPrint=0,debug2=0;
  Bool_t bPolCompare=1,bPolErCompare=0,bCedgeCompare=0,bChiSqrCompare=0,bChiSqrHist=0;
  gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  ///lets focus on the region between 23220 to 23530;
  //const Int_t runBegin = 23200, runEnd = 24150;//runBegin = 22659, runEnd = 25546;
  //const Int_t runBegin = 23220, runEnd = 23514;//runBegin = 22659, runEnd = 25546;
  const Int_t runBegin = 23220, runEnd = 23530;//runBegin = 22659, runEnd = 25546;
  const Int_t numbRuns = runEnd - runBegin;//a non-variable is needed to set the size of an array length
  ifstream poltext,polRunlet;
  ofstream polLasCyc;
  TString destDir="/w/hallc/compton/users/narayan/my_scratch/www";
  TString runletFile = Form("/u/home/narayan/acquired/vladas/modFiles/polRunletCE_%d_%d.dat",run1,run2);//file updated by Vladas on 24Apr2013
  //Form("/u/home/narayan/acquired/vladas/modFiles/polEffWidth_%d_%d.dat",run1,run2);//file updated by Vladas on 24Apr2013
  //polRunlet.open("/u/home/narayan/acquired/vladas/modFiles/runletFCEPolRun2.txt");
  //polRunlet.open(Form("/u/home/narayan/acquired/vladas/modFiles/runletPol_%d_%d.txt",run1,run2));
  TString lasCycFile = destDir + Form("/polList_%d_%d.txt",run1,run2);
  //TString lasCycFile = Form("/w/hallc/qweak/narayan/polList_%d_%d.txt",run1,run2);
  TString dum1,dum2,dum3,dum4,dum5,dum6,dum7,dum8,dum9,dum10;
  Int_t runRange = run2-run1+1;
  TH1D *hPolEr = new TH1D("polEr laser Cycle", "polarization error laser cycle analysis", runRange, 0, 0.1);
  TH1D *hRunletPolEr = new TH1D("polEr runlet based", "polarization error runlet based analysis", runRange, 0, 0.1);
  TH1D *hPolDiff = new TH1D("diff. in pol%", "difference in polarization between the two analysis", runRange, 0, 0.1);
  TH1D *hChiSqLC = new TH1D("chiSqr laser Cycle", "chi sqr laser cycle analysis", runRange, 0, 0.1);
  TH1D *hChiSqRL = new TH1D("chiSqr runlet based", "chi sqr runlet based analysis", runRange, 0, 0.1);
  TF1 *linearFit = new TF1("linearFit", "pol0");

  hPolEr->SetBit(TH1::kCanRebin);
  hRunletPolEr->SetBit(TH1::kCanRebin);
  hPolDiff->SetBit(TH1::kCanRebin);
  hChiSqLC->SetBit(TH1::kCanRebin);
  hChiSqRL->SetBit(TH1::kCanRebin);

  std::vector< Double_t> runListLC,runListRL,polLC,polRL,polErLC,polErRL,comptEdgeLC,comptEdgeRL,chiSqLC,chiSqRL,edgeErRL,edgeErLC;

  ///write the lasCyc based polarization into one central file
  Int_t count1=0;
  polLasCyc.open(lasCycFile.Data());
  if (!polLasCyc.is_open()) cout<<"could not open file to assimilate pol"<<endl;
  polLasCyc<<"run\tabsPol\tpolEr\tchiSq/ndf\tCE\tCEEr\teffStrWid\teffStrWidEr\tpl"<<endl;
  for (Int_t r=run1; r<=run2; r++) {
    Double_t absPol[nPlanes],pol[nPlanes]={0.0},polEr[nPlanes]={0.0},chiSq[nPlanes],comptEdge[nPlanes],comptEdgeEr[nPlanes],effStrip[nPlanes],effStripEr[nPlanes];
    Int_t plane[nPlanes],NDF[nPlanes],runnum[nPlanes]; 
    filePrefix= Form("run_%d/edetLasCyc_%d_",r,r);
    poltext.open(Form("%s/%s/%sAcPol.txt",pPath,webDirectory,filePrefix.Data()));
    if(poltext.is_open()) {
      if(debug1) cout<<"opened file "<<Form("%s/%s/%sAcPol.txt",pPath,webDirectory,filePrefix.Data())<<endl;
      poltext >>dum1>>dum2>>dum3>>dum4>>dum5>>dum6>>dum7>>dum8>>dum9>>dum10;
      if(debug1) cout<<"plane\tpol\tpolEr"<<endl;
      for (Int_t p=startPlane; p<endPlane; p++) {
	poltext>>runnum[p]>>pol[p]>>polEr[p]>>chiSq[p]>>NDF[p]>>comptEdge[p]>>comptEdgeEr[p]>>effStrip[p]>>effStripEr[p]>>plane[p];
	if(debug1) cout<<plane[p]<<"\t"<<pol[p]<<"\t"<<polEr[p]<<endl;
      }
      poltext.close();
    } else if(debug) cout<<"*** Alert: could not open "<<Form("%s/%s/%sAcPol.txt",pPath,webDirectory,filePrefix.Data())<<endl;
    absPol[0] = fabs(pol[0]);
    kPlsPrint = (runnum[0]==r && polEr[0]<3.0 && absPol[0]<93.0 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <5.0);
    //kPlsPrint = (polEr[0]<4.0 && absPol[0]<95.0 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <6.0);
    //kPlsPrint = (runnum[0]==r && polEr[0]<1.5 && absPol[0]<91.5 && absPol[0]>50.0 && polEr[0]!=0.0 && comptEdge[0]>20.0 && (chiSq[0]/NDF[0]) <5.0);
    if (kPlsPrint) {
      polLasCyc<<Form("%5.0f\t%2.2f\t%1.2f\t%.2f\t%2.2f\t%2.2f\t%.3f\t%.3f\t%d\n",(Double_t)r,absPol[0],polEr[0],chiSq[0]/NDF[0],comptEdge[0],comptEdgeEr[0],effStrip[0],effStripEr[0],plane[0]);
      runListLC.push_back(r);
      polLC.push_back(absPol[0]);
      polErLC.push_back(polEr[0]);
      comptEdgeLC.push_back(comptEdge[0]);
      edgeErLC.push_back(comptEdgeEr[0]);
      chiSqLC.push_back(chiSq[0]/NDF[0]);
      count1++;///remains as a count of how many 'good' runs we have
    }
  }
  polLasCyc.close();
  cout<<red<<"found valid "<<runListLC.size()<<" analyzed runs in runListLC"<<normal<<endl;

  polRunlet.open(runletFile.Data());
  Double_t zero[numbRuns]={0};
  Int_t count2=0;
  if(polRunlet.is_open()) { //this is available only for plane 1
    while(polRunlet.good()) {
      runListRL.push_back(0.0);
      polRL.push_back(0.0);
      polErRL.push_back(0.0);
      comptEdgeRL.push_back(0.0);
      chiSqRL.push_back(0.0);
      edgeErRL.push_back(0.0);
      polRunlet>>runListRL[count2]>>polRL[count2]>>polErRL[count2]>>comptEdgeRL[count2]>>edgeErRL[count2]>>chiSqRL[count2];
      if(debug2) cout<<runListRL[count2]<<"\t"<<polRL[count2]<<"\t"<<polErRL[count2]<<endl;
      if(polRunlet.eof()) break;
      count2++;
    }
    polRunlet.close();
  } else cout<<"*** Alert: could not open runlet based pol.txt file ~/acquired/vladas/modFiles/runletFCEPolRun2.txt"<<endl;
  cout<<red<<"found valid "<<count2<<" analyzed runs in runListRL"<<normal<<endl;

  cout<<" runRange:"<<runRange<<", count1:"<<count1<<", count2:"<<count2<<endl;
  
  std::vector<Double_t> commonRuns,polDiff,polDiffEr,zero_1;
  for(Int_t r=0; r<(Int_t)runListRL.size(); r++) {
    Bool_t kFound = 0;
    for(Int_t r2=0; r2<(Int_t)runListLC.size(); r2++) {
      if(runListRL[r] == runListLC[r2]) { 
	kFound = 1;
	if((comptEdgeRL[r]-comptEdgeLC[r2])>0.5) {
	  cout<<green<<"CE was off by 0.5 strip for run "<<runListLC[r2]<<normal<<endl;
	} else if(fabs(polRL[r] - polLC[r2])>1.0) {
	  cout<<blue<<runListRL[r]<<"\t"<<polRL[r]<<" in RL, and "<<runListLC[r2]<<" "<<polLC[r2]<<" in LC"<<normal<<endl;
	} else {
	  polDiff.push_back(polLC[r2] - polRL[r]);
	  commonRuns.push_back(runListRL[r2]);
	  polDiffEr.push_back(polErLC[r]);
	  zero_1.push_back(0.0);
	  hPolDiff->Fill(polDiff.back());
	  hPolEr->Fill(polErLC[r2]);
	  hRunletPolEr->Fill(polErRL[r]);

	  hChiSqLC->Fill(chiSqLC[r2]);
	  hChiSqRL->Fill(chiSqRL[r]);
	  break;
	}
      }
    }
    if(!kFound) cout<<magenta<<"run "<<runListRL[r]<<" from RL not found in LC"<<normal<<endl;
  }

  if(bPolCompare) {
    //coordinates in TCanvas are in order :x,y of left top corner;x,y of right bottom corner
    TCanvas *polAvgP1 = new TCanvas("polAvgP1","Avg Polarization Plane 1",10,10,1000,900);
    TCanvas *cpolDiff = new TCanvas("cpolDiff","difference in evaluated Polarization",10,10,600,600);
    polAvgP1->Divide(1,2);
    polAvgP1->cd(1);
    TGraphErrors *grPolPlane1,*grRunletPolP1;
    TLegend *legPol= new TLegend(0.101,0.85,0.40,0.9);;

    //grPolPlane1 = new TGraphErrors(destDir+Form("/polList_%d_%d.txt",run1,run2),"%lg %lg %lg");
    grPolPlane1 = new TGraphErrors(runListLC.size(),runListLC.data(),polLC.data(),zero,polErLC.data());
    grRunletPolP1 = new TGraphErrors(runListRL.size(),runListRL.data(),polRL.data(),zero,polErRL.data());

    polAvgP1->GetPad(1)->SetGridx(1);
    //polAvgP1->SetGridx(1);
    grPolPlane1->SetMarkerStyle(kOpenCircle);
    grPolPlane1->SetMarkerColor(kBlue);
    grPolPlane1->SetLineColor(kBlue);
    grPolPlane1->SetFillColor(0);
    grPolPlane1->SetTitle();
    grPolPlane1->GetXaxis()->SetTitleSize(0.05);
    grPolPlane1->GetXaxis()->SetTitleOffset(0.84);
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

    grRunletPolP1->SetMarkerStyle(kOpenCircle);
    grRunletPolP1->SetMarkerColor(kRed);
    grRunletPolP1->SetLineColor(kRed);
    grRunletPolP1->Draw("P");

    legPol->AddEntry(grPolPlane1,"laser Cycle evaluation","lpe");
    //legPol->AddEntry(grPolPlane1,"polarization","lpe");
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
    grPolDiff->GetXaxis()->SetLimits(run1+5,run2+5); 
    grPolDiff->GetXaxis()->SetTitleSize(0.05);
    grPolDiff->SetMaximum(3.2); 
    grPolDiff->SetMinimum(-3.2);

    grPolDiff->GetYaxis()->SetTitleSize(0.05);
    grPolDiff->GetXaxis()->SetTitle("run number");
    grPolDiff->GetYaxis()->SetTitle("difference in polarization");

    cpolDiff->cd();
    hPolDiff->SetLineColor(kBlack);
    hPolDiff->Draw("H");
    cpolDiff->SaveAs(destDir+Form("/polDiff_%d_%d.png",run1,run2));
    //polAvgP1->SaveAs(Form("%s/%s/pol_%d_%d.png",pPath,webDirectory,run1,run2));
    //polAvgP1->SaveAs(Form("/w/hallc/qweak/narayan/APS2013Aprilpol_%d_%d.png",run1,run2));
    polAvgP1->SaveAs(destDir+Form("/polAvgP1_%d_%d.png",run1,run2));
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
    TLegend *legCedge= new TLegend(0.101,0.75,0.43,0.9);;
    cout<<" runRange:"<<runRange<<", count1:"<<count1<<", count2:"<<count2<<endl;

    ofstream fCedgeRunlet,fCedgeLasCyc;
    fCedgeRunlet.open("edgeRL_run1_run2.txt");
    for (Int_t r=0; r<count2; r++) {
      fCedgeRunlet<<Form("%5.0f\t%2.2f\t%.2f\n",runListRL[r],comptEdgeRL[r],edgeErRL[r]);
    }
    fCedgeRunlet.close();

    fCedgeLasCyc.open("edgeLC_run1_run2.txt");
    for (Int_t r=0; r<count1; r++) {
      //fCedgeLasCyc<<Form("%5.0f\t%2.2f\n",lasCycRunnum[r],lasCycComptEdge[r]);
      fCedgeLasCyc<<Form("%5.0f\t%2.2f\t%.2f\n",runListLC[r],comptEdgeLC[r],edgeErLC[r]);
    }
    fCedgeLasCyc.close();

    TGraphErrors *grLasCycCedge = new TGraphErrors("edgeLC_run1_run2.txt","%lg %lg %lg");
    cComptEdge->SetGridx(1);
    grLasCycCedge->SetTitle();
    grLasCycCedge->SetMarkerStyle(kOpenSquare);
    grLasCycCedge->SetMarkerColor(kBlue);
    Double_t runletCedgeInRange[numbRuns];
    Int_t count4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runListRL[r] >= run1) && (runListRL[r] <= run2)) {
	count4++;
	runletCedgeInRange[count4] = comptEdgeRL[r];
      }
    }
    //TGraph *grRunletCedge = new TGraph(count4,lasCycRunnum,runletCedgeInRange);
    TGraphErrors *grRunletCedge = new TGraphErrors("edgeRL_run1_run2.txt","%lg %lg %lg");
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
    //TGraph *grChiSqr = new TGraph(count1,runListLC,lasCycChiSq);
    TGraph *grChiSqr = new TGraph(count1,runListLC.data(),chiSqLC.data());
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
    linearFit->SetLineColor(kBlue);
    grChiSqr->Fit(linearFit,"ME");
    Double_t runletChiSqrInRange[numbRuns];
    Int_t count4=0;
    for (Int_t r=0; r<numbRuns; r++) {
      if((runListRL[r] >= run1) && (runListRL[r] <= run2)) {
	count4++;
	runletChiSqrInRange[count4] = chiSqRL[r];
      }
    }
    //TGraph *grRunletChiSqr = new TGraph(count4,lasCycRunnum,runletChiSqrInRange);
    TGraph *grRunletChiSqr = new TGraph(count4,runListLC.data(),runletChiSqrInRange);
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

    cChiSqr->SaveAs(Form("/w/hallc/qweak/narayan/chiSqr_%d_%d.png",run1,run2));
  }

  if(bChiSqrHist) {
    TCanvas *cChiSqr = new TCanvas("cChiSqrHist","chi-Sqr/ndf",10,10,1000,600);
    cChiSqr->Divide(2,1);

    cChiSqr->cd(1);
    hChiSqLC->SetLineColor(kBlue);
    hChiSqLC->Draw("H");

    cChiSqr->cd(2);
    hChiSqRL->SetLineColor(kRed);
    hChiSqRL->Draw("H");   
    cChiSqr->SaveAs(Form("/w/hallc/qweak/narayan/chiSqrHist_%d_%d.png",run1,run2));   
  }

  return (run2 - run1+1);//the number of runs processed
}

