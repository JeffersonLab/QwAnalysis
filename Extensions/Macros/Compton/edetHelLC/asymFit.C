//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry
#ifndef __asymFit_F
#define __asymFit_F

#include "rootClass.h"
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "stripMask.C"
#include "infoDAQ.C"
#include "read3Cfile.C"

Double_t theoCrossSec(Double_t *thisStrip, Double_t *parCx)//3 parameter fit for cross section
{///parCx[1]: to be found Cedge
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end()) {
    TF1::RejectPoint();
    return 0;
  }
  xStrip = xCedge - (parCx[1] - (*thisStrip))*stripWidth*parCx[0];
  rhoStrip = (param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]);
  rhoPlus = 1.0-rhoStrip*(1.0+a_const);
  rhoMinus = 1.0-rhoStrip*(1.0 - a_const);//just a term in eqn 24
  dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a_const)); // 2nd term of eqn 22
  return (parCx[2]*((rhoStrip*(1.0 - a_const)*rhoStrip*(1.0 - a_const)/rhoMinus)+1.0+dsdrho1*dsdrho1));//eqn.22,without factor 2*pi*(re^2)/a_const
}

Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end() ) { 
    //cout<<red<<"ignored strip: "<<*thisStrip<<normal<<endl;
    TF1::RejectPoint();
    return 0;
  }
  //xStrip = xCedge - (tempCedge + par[1] - (*thisStrip))*stripWidth*par[0];//for 2nd parameter as Cedge offset
  //xStrip = xCedge + par[1] - (tempCedge -(*thisStrip))*stripWidth*par[0]; //2nd parameter as position inside CE strip
  //xStrip = xCedge + par[1]*stripWidth - (*thisStrip)*stripWidth*par[0];//G's method of fitting!!didn't work
  xStrip = xCedge - (par[0] -(*thisStrip))*stripWidth*effStripWidth; //for 2nd parameter as Cedge itself
  rhoStrip = param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]+ xStrip*xStrip*xStrip*xStrip*param[4]+ xStrip*xStrip*xStrip*xStrip*xStrip*param[5];
  if(kRadCor) {
    eGamma = rhoStrip* kprimemax;
    betaBar=TMath::Sqrt(1.0 - (me/eEnergy)*(me/eEnergy)) ;
    betaCM = (betaBar*eEnergy-eLaser)/(eEnergy+eLaser) ; //eq. 2.10
    sEq211 = me*me + 2*eLaser*eEnergy*(1.0+betaCM) ;//eq. 2.11 
    gammaCM = (eEnergy+eLaser)/TMath::Sqrt(sEq211) ;
    eLaserCM = eLaser*gammaCM*(1.0+betaCM) ;
    eBeamCM = TMath::Sqrt(eLaser*eLaser + me*me) ; 

    eBetaCM = eLaserCM/eBeamCM;// eqn. 2.2
    costhcm=(gammaCM*eLaserCM - eGamma)/(eLaserCM*gammaCM*betaCM); // eqn. 2.13 

    if(rhoStrip<=1.0) delta=alpha/pi *(3.0*costhcm-1.0)/(4*(eBetaCM + costhcm));//  eqn 3.9 
    else delta = 0.0;///leave A_QED unchanged
    radCor = (1.0+delta);
  }
  rhoPlus = 1.0-rhoStrip*(1.0+a_const);
  rhoMinus = 1.0-rhoStrip*(1.0 - a_const);//just a term in eqn 24
  dsdrho1 = rhoPlus/rhoMinus;//(1-rhoStrip*(1-a_const)); // 2nd term of eqn 22
  dsdrho =((rhoStrip*(1.0 - a_const)*rhoStrip*(1.0 - a_const)/rhoMinus)+1.0+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a_const
  //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  return (radCor*(par[1]*(rhoPlus*(1.0-1.0/(rhoMinus*rhoMinus)))/dsdrho));//calcAsym;
}

Int_t asymFit(Int_t runnum=24519,TString dataType="Ac")
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  if(kRadCor) cout<<blue<<"Radiative correction being applied"<<normal<<endl;
  else cout<<blue<<"radiative correction turned OFF"<<normal<<endl;
  //  time_t tStart = time(0), tEnd; 
  //  div_t div_output;
  gStyle->SetOptFit(0);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  gStyle->SetTitleSize(0.06,"XY");
  gStyle->SetTitleOffset(1.05,"X");
  gStyle->SetTitleOffset(0.6,"Y");

  //gStyle->SetTextFont(132);
  gStyle->SetTextSize(0.08);
  gStyle->SetLabelFont(132,"xyz");
  gStyle->SetLabelSize(0.06,"xyz");
  //gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.07);
  gStyle->SetPadBottomMargin(0.16);
  //gStyle->SetPadLeftMargin(0.12);

  filePre = Form(filePrefix,runnum,runnum);

  Bool_t debug=1,debug1=0;
  Bool_t kYieldFit=0,kYield=1,kResidual=1, kBgdAsym=1;
  Bool_t kFoundCE=0;
  Int_t status=1;///1: fit not converged; 0: converged
  TPaveText *pt, *ptRes;
  TLegend *leg,*legYield;
  TLine *myline = new TLine(1,0,65,0);
  TF1 *polFit;
  TString file;

  if(daqflag == 0) {///stripMask not called yet 
    daqflag = stripMask();
    if(daqflag < 0) {///exited because one of the variables determining sign of pol flipped in the middle of run
      cout<<red<<"\nreturned error from skipStrip, hence exiting\n"<<normal<<endl;
      return -1;
    }
  }
  if(daqCheck ==0) {
    daqCheck = infoDAQ(runnum, dataType); ///needed eEnergy for rhoToX.C
    cout<<"calling infoDAQ from asymFit.C to get energy value"<<endl;
  } else cout<<"parameters from infoDAQ already available hence not invoking that macro (daqcheck="<<daqCheck<<")"<<endl;
  if(daqCheck<0) {
    cout<<red<<"\nreturned error from infoDAQ.C hence exiting\n"<<normal<<endl;
    return -2;
  }

  ifstream paramfile;
  ofstream polList,fitInfo;
  std::vector<std::vector <Double_t> > activeStrip;
  std::vector<Double_t > trueStrip,asym,asymEr;
  std::vector<Double_t > yieldL1,yieldL1Er,asymNr;
  std::vector<Double_t > yieldL0,yieldL0Er;
  std::vector<Double_t > stripCE, dummy;
  Int_t usedStrips=0;///no.of used strips upto the CE

  ///Read in all files that are needed
  Int_t fOpen;
  file = Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath, txt,filePre.Data(),plane);
  fOpen = read3Cfile(file, trueStrip, asym, asymEr);

  file = Form("%s/%s/%s%sLasOffBkgdP%d.txt",pPath, txt,filePre.Data(),dataType.Data(),plane);
  fOpen = read3Cfile(file, dummy, yieldL0, yieldL0Er); 

  file = Form("%s/%s/%s%sYieldP%d.txt",pPath, txt,filePre.Data(),dataType.Data(),plane);
  fOpen = read3Cfile(file, dummy, yieldL1, yieldL1Er);

  ///Estimate the compton edge location
  for(Int_t s=0; s<=(Int_t)trueStrip.size(); s++) {
    //Int_t s = trueStrip[i];///actual strip number
    //if((((yieldL1[s]-yieldL0[s]))< (yieldL0[s]+100.0*yieldL0Er[s])) && trueStrip[s]>edgeClamp) {
    if( (yieldL1.at(s)-yieldL0.at(s) < 100.0*yieldL0Er.at(s) ) && s>25) {
      ///there are cases when strip-1 has beamOff counts higher than bgd sub counts due to halo eg.run23246
      ///hence the above condition of s>25 is applied to ensure that trueStrip.at(s-1) does not access strip-1
      Cedge = trueStrip.at(s-1); ///since the above condition is fulfiled after crossing Cedge
      cout<<"probable Cedge : "<<Cedge<<endl;
      polSign = asym.at(s-1) > 0 ? 1 : 0;
      kFoundCE = 1;
      break;
    }
  }

  if(kFoundCE) {
    cout<<"\nCompton edge for plane "<<plane<<" auto-determined to strip "<<Cedge<<endl;
    if(polSign) cout<<"sign of polarization is positive\n"<<endl;
    else cout<<"sign of polarization is negative\n"<<endl;
  } else cout<<red<<"**** Alert: Did not find Cedge for plane "<<plane<<" in run # "<<runnum<<" till the last strip"<<normal<<endl;

  TCanvas *cAsym;
  TGraphErrors *grAsym;

  cAsym = new TCanvas("cAsym","Asymmetry and Strip number",10,10,900,300);

  xCedge = rhoToX(runnum); ///this function should be called after determining the Cedge

  paramfile.open(Form("%s/%s/%scheckfileP%d.txt",pPath, txt,filePre.Data(),plane));
  cout<<"reading in the rho to X fitting parameters for plane "<<plane<<", they were:" <<endl;
  paramfile>>param[0]>>param[1]>>param[2]>>param[3]>>param[4]>>param[5];
  paramfile.close();
  if(debug) printf("%g\t%g\t%g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3],param[4],param[5]);

  cAsym->cd();  
  //cAsym->GetPad(1)->SetGridx(1);
  cAsym->SetGridx(1);

  grAsym=new TGraphErrors((Int_t)trueStrip.size(),trueStrip.data(),asym.data(),0,asymEr.data()); 
  grAsym->GetXaxis()->SetTitle("Strip number");
  grAsym->GetYaxis()->SetTitle("asymmetry");  
  grAsym->SetTitle();
  //grAsym->SetTitle(Form(dataType+" Mode Asymmetry, Plane %d",plane));//Form("experimental asymmetry Run: %d, Plane %d",runnum,plane));
  grAsym->SetMarkerStyle(kFullCircle);
  grAsym->SetLineColor(kRed);
  grAsym->SetMarkerColor(kRed); ///kRed+2 = Maroon
  grAsym->SetMaximum(0.048); 
  grAsym->SetMinimum(-0.048);
  grAsym->GetXaxis()->SetLimits(0,65); 
  //grAsym->GetXaxis()->SetNdivisions(416, kFALSE);

  grAsym->Draw("AP");
  tempCedge = Cedge;//-0.5;///this should be equated before the declaration of TF1

  ///2 parameter fit
  polFit = new TF1("polFit",theoreticalAsym,startStrip+1,tempCedge+1,2);
  //TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+10,Cedge,3);//use strips after the first 10 strips
  if(polSign) { //positive
    polFit->SetParameters(tempCedge,0.89);//begin fitting with effStrWid=1, CE=auto-determined, polarization=89%
    polFit->SetParLimits(0,40.0,62.0);///run2: allow the CE to vary between these strip
    polFit->SetParLimits(1,0.68,0.94);///allowing polarization to be 50% to 93%
  } else {     //negative
    polFit->SetParameters(tempCedge,-0.89);//begin fitting with effStrWid=1, CE=auto-determined, polarization=89%
    polFit->SetParLimits(0,40.0,62.0);///run2: allow the CE to vary between these strip
    polFit->SetParLimits(1,-0.94,-0.68);
  }
  cout<<blue<<"using CE and pol as the two fit parameters"<<normal<<endl;

  polFit->SetParNames("comptonEdge","polarization");
  polFit->SetLineColor(kBlue);
  cout<<red<<"the maxdist used:"<<xCedge<<normal<<endl;
  TVirtualFitter::SetMaxIterations(10000);

  Int_t numbIterations = 0;
  TFitResultPtr fitr = grAsym->Fit("polFit","RES 0");///1st attempt
  status = int (fitr);
  cout<<green<<"the polarization fit status "<<status<<normal<<endl;

  if(status==0) { ///if not converged, reuse results of previous fit to refit
    cout<<blue<<"fit converged, hence applying MINOS for better errors"<<normal<<endl;
    cEdge = polFit->GetParameter(0);
    cEdgeEr = polFit->GetParError(0);
    pol = polFit->GetParameter(1);
    polEr = polFit->GetParError(1);
    chiSq = polFit->GetChisquare();
    NDF = polFit->GetNDF();
    cout<<green<<"pol%: "<<pol*100<<"+/-"<<polEr*100<<"\t CE: "<<cEdge<<"+/-"<<cEdgeEr<<normal<<endl;
    //polFit->SetParameters(cEdge,pol);
    //polFit->SetParLimits(0, cEdge-2, cEdge+2);///allowing CE to change by +/- 1 strip
    //polFit->SetParLimits(1, pol-0.02, pol+0.02);///allowing pol% to change by +/- 2%
    //polFit->SetParLimits(0, cEdge-cEdgeEr, cEdge+cEdgeEr);
    //polFit->SetParLimits(1, pol-polEr, pol+polEr);
    fitr = grAsym->Fit("polFit","MRES 0");///2nd attempt
    status = int (fitr);
    numbIterations++;
    if (status!=0) { ///fit failed after attempting to apply MINOS on a successful fit
      cout<<magenta<<"failed MINOS though the initial fit was successful"<<endl;
      cout<<"do not update the preceeding fit parameters"<<normal<<endl;
      status = 0;
      //fitr = grAsym->Fit("polFit","RES 0");
      //status = int (fitr);
      //After failing once, an attempt to refit even without MINOS fails, hence leaving the parameters
      //.. untouched instead and setting back the fit status =0, so that subsequent attempt is avoided
    } else {
      cout<<magenta<<"updated the fit parameters with the MINOS fit results"<<normal<<endl;
      cEdge = polFit->GetParameter(0);
      cEdgeEr = polFit->GetParError(0);
      pol = polFit->GetParameter(1);
      polEr = polFit->GetParError(1);
      chiSq = polFit->GetChisquare();
      NDF = polFit->GetNDF();
    }
  } 
  ///sometimes (eg.22773) the fit fails after I try to apply MINOS, though it succeeded earlier
  //..so the following while loop should operate independent of the preceeding 'if' loop
  //
  while (status!=0) {
    numbIterations++;
    cout<<blue<<"repeating attempt to fit without MINOS, attempt# "<<numbIterations<<normal<<endl;
    //ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Simplex");///failed r22987
    //ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Minimize");//failed r22987
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Scan");
    fitr = grAsym->Fit("polFit","RES 0");
    status = int (fitr);
    if(numbIterations>maxIterations) {
      cout<<magenta<<"maximum no.of iterations ("<<maxIterations<<") attempted, hence exiting"<<normal<<endl;
      break;
    }
    cEdge = polFit->GetParameter(0);
    cEdgeEr = polFit->GetParError(0);
    pol = polFit->GetParameter(1);
    polEr = polFit->GetParError(1);
    chiSq = polFit->GetChisquare();
    NDF = polFit->GetNDF();
  }

  polFit->DrawCopy("same");

  if(debug) cout<<"\nwriting the polarization relevant values to file "<<endl;
  polList.open(Form("%s/%s/%s"+dataType+"Pol.txt",pPath, txt,filePre.Data()));
  polList<<";run\tpol\tpolEr\tchiSq\tNDF\tCE\tCE_Er\tplane\tfitStatus\tgoodCyc"<<endl;
  polList<< Form("%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%d\t%d\t%d",(Double_t)runnum,pol*100,polEr*100,chiSq,NDF,cEdge,cEdgeEr,plane,status,asymflag)<<endl;
  if(debug) {
    cout<<Form("runnum\tpol.\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\tplane\tfitStatus\tgoodCyc");
    cout<<Form("\n%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%d\t%d\t%d\n",(Double_t)runnum,pol*100,polEr*100,chiSq,NDF,cEdge,cEdgeEr,plane,status,asymflag)<<endl;
  }
  leg = new TLegend(0.101,0.73,0.30,0.9);
  leg->AddEntry(grAsym,"measured","lpe");///I just need the name
  leg->AddEntry("polFit","QED fit","l");//"lpf");//
  leg->SetFillColor(0);
  leg->Draw();

  if (polSign) pt = new TPaveText(0.44,0.17,0.69,0.52,"brNDC");///left edge,bottom edge,right edge, top edge
  else  pt = new TPaveText(0.44,0.51,0.69,0.88,"brNDC");

  pt->SetTextSize(0.060);//0.028); 
  pt->SetBorderSize(1);
  pt->SetTextAlign(12);
  pt->SetFillColor(-1);
  pt->SetShadowColor(-1);

  pt->AddText(Form("Chi Sq / ndf       : %.3f",chiSq/NDF));
  pt->AddText(Form("Compton edge : %2.2f #pm %2.2f",cEdge,cEdgeEr));
  pt->AddText(Form("Polarization      : %2.2f #pm %2.2f",pol*100.0,polEr*100.0));
  pt->Draw();
  myline->Draw();
  polList.close();
  cAsym->SaveAs(Form("%s/%s/%s"+dataType+"AsymFit.png",pPath,www,filePre.Data()));
  //delete myline;
  //delete cAsym;
  //delete pt;
  //delete leg;
  //delete grAsym;
  //delete polFit;

  for (Int_t s = startStrip; s < (Int_t)trueStrip.size(); s++) {///trueStrip will be the no.of used strips including post CE
    if (trueStrip.at(s) < cEdge) usedStrips++;  ///cEdge determined in preceeding section 
  }
  if(debug) cout<<blue<<"used a total of "<<usedStrips<<" strips upto the CE"<<normal<<endl;
  TF1 *linearFit = new TF1("linearFit", "pol0",startStrip+1);
  linearFit->SetLineColor(kRed);

  //!This is not meant to be able to deal with different planes, the need wasn't felt
  if(kBgdAsym) {
    TGraphErrors *grBgd; 
    TCanvas *cBgd = new TCanvas("cBgd","Bgd Asymmetry",10,650,900,300);
    TPaveText *ptBgd;

    std::vector <Double_t> bgdAsym, bgdAsymEr;
    ///Read in the background asymmetry file
    cBgd->cd();
    cBgd->SetGridx();
    file = Form("%s/%s/%s%sBkgdAsymP%d.txt",pPath, txt,filePre.Data(),dataType.Data(),plane);
    fOpen = read3Cfile(file, dummy, bgdAsym, bgdAsymEr);

    //Trying to debug the pattern in bgd asym
    if(debug1) {
      for(int i=0; i<(Int_t)trueStrip.size(); i++) {
        printf("%f\t%f\t%f\n",trueStrip[i],bgdAsym[i],bgdAsymEr[i]);
      }
    }

    grBgd = new TGraphErrors((Int_t)trueStrip.size(),trueStrip.data(),bgdAsym.data(),0,bgdAsymEr.data());
    grBgd->Draw("AP");
    grBgd->GetXaxis()->SetTitle("Strip number");
    grBgd->GetYaxis()->SetTitle("bgd asymmetry");   
    grBgd->SetTitle(0);
    grBgd->SetMarkerStyle(kFullCircle);
    grBgd->SetLineColor(kBlue);
    grBgd->SetMarkerColor(kBlue); ///kRed+2 = Maroon
    grBgd->GetXaxis()->SetLimits(0,65); 
    grBgd->Fit(linearFit,"ME");//q:quiet mode

    bgdAsymFit = linearFit->GetParameter(0);
    bgdAsymFitEr = linearFit->GetParError(0);
    chiSqBgdAsym = linearFit->GetChisquare();
    bgdAsymFitNDF = linearFit->GetNDF();

    ptBgd = new TPaveText(0.67,0.67,0.9,0.9,"brNDC");///x1,y1,x2,y2
    ptBgd->SetTextSize(0.048);//0.028); 
    ptBgd->SetBorderSize(1);
    ptBgd->SetTextAlign(12);
    ptBgd->SetFillColor(0);
    ptBgd->SetShadowColor(-1);
    ptBgd->AddText(Form("chi Sq / ndf  : %0.1f / %d",chiSqBgdAsym,bgdAsymFitNDF));
    ptBgd->AddText(Form("linear fit    : %f #pm %f",bgdAsymFit, bgdAsymFitEr));
    ptBgd->Draw();
    cBgd->SaveAs(Form("%s/%s/%s"+dataType+"BgdAsym.png",pPath,www,filePre.Data()));
    //delete grBgd;
    //delete cBgd;
    //delete ptBgd;
  }

  if(kResidual) {
    TCanvas *cResidual;
    TGraphErrors *grResiduals; 
    cResidual = new TCanvas("cResidual","residual (Asymmetry - Fit)",10,500,900,300);
    std::vector<Double_t> residueEr; 

    std::vector<Double_t> fitResidue;
    ///determining the residue of the above fit
    if(debug1) cout<<"strip\tfitResidue.at(s)\tstripAsym.at(s)\tpolFit->Eval(s)"<<endl;
    for (Int_t s = startStrip; s < usedStrips; s++) {
      stripCE.push_back(trueStrip.at(s));
      residueEr.push_back(asymEr.at(s));
      fitResidue.push_back( asym.at(s) - polFit->Eval(trueStrip.at(s)) );
      if(debug1) cout<<trueStrip.at(s)<<"\t"<<fitResidue.at(s)<<"\t"<<asym.at(s)<<"\t"<<polFit->Eval(trueStrip.at(s))<<endl;
    }

    cResidual->cd();  
    cResidual->SetGridx(1);
    grResiduals=new TGraphErrors((Int_t)stripCE.size(),stripCE.data(),fitResidue.data(),0,residueEr.data());
    grResiduals->SetMarkerStyle(kOpenCircle);
    //grResiduals->SetMaximum(0.012);/// half of asymmetry axis 
    //grResiduals->SetMinimum(-0.012);/// half of asymmetry axis 
    grResiduals->GetXaxis()->SetLimits(startStrip,endStrip+1); 
    //grResiduals->SetTitle(Form(dataType+" Mode Asymmetry Fit Residual, Plane %d",plane));
    grResiduals->SetTitle(0);
    grResiduals->GetXaxis()->SetTitle("Compton electron detector strip number");
    grResiduals->GetYaxis()->SetTitle("asymmetry - Fit");

    grResiduals->Draw("AP");
    grResiduals->Fit(linearFit,"REq");//q:quiet mode

    resFit = linearFit->GetParameter(0);
    resFitEr = linearFit->GetParError(0);
    chiSqResidue = linearFit->GetChisquare();
    resFitNDF = linearFit->GetNDF();

    ptRes = new TPaveText(0.67,0.67,0.9,0.9,"brNDC");///x1,y1,x2,y2
    ptRes->SetTextSize(0.048);//0.028); 
    ptRes->SetBorderSize(1);
    ptRes->SetTextAlign(12);
    ptRes->SetFillColor(0);
    ptRes->SetShadowColor(-1);
    ptRes->AddText(Form("chi Sq / ndf  : %0.1f / %d",chiSqResidue,resFitNDF));
    ptRes->AddText(Form("linear fit    : %f #pm %f",resFit,resFitEr));
    ptRes->Draw();
    cResidual->SaveAs(Form("%s/%s/%s"+dataType+"AsymFitResidual.png",pPath,www,filePre.Data()));
    //delete cResidual;
    //delete grResiduals;
    //delete ptRes;
  }
  //delete linearFit;
  fitInfo.open(Form("%s/%s/%s"+dataType+"FitInfo.txt",pPath, txt,filePre.Data()));
  fitInfo<<";run\tresFit\tresFitEr\tchiSqRes\tresNDF\tbgdAsymFit\tbgdAsymFitEr\tchiSqBgd\tbgdNDF"<<endl;
  fitInfo<<Form("%5.0f\t%.6f\t%.6f\t%.2f\t%d\t%.6f\t%.6f\t%.2f\t%d\n",(Double_t)runnum,resFit,resFitEr,chiSqResidue,resFitNDF,bgdAsymFit,bgdAsymFitEr,chiSqBgdAsym,bgdAsymFitNDF);
  fitInfo.close();

  if (kYield) { ///determine yield
    TCanvas *cYield;
    TGraphErrors *grYield,*grB1L0;
    TMultiGraph *grAsymDrAll;
    cYield = new TCanvas("cYield","detector yield",300,200,900,300);

    cYield->cd();
    cYield->SetGridx(1);
    grYield = new TGraphErrors((Int_t)trueStrip.size(), trueStrip.data(), yieldL1.data(), 0, yieldL1Er.data());
    grYield->SetLineColor(kGreen);
    grYield->SetFillColor(kGreen);   
    grYield->SetMarkerColor(kGreen); ///kRed+2 = Maroon

    ///3 parameter fit for cross section
    if(kYieldFit) {
      TF1 *crossSecFit = new TF1("crossSecFit",theoCrossSec,startStrip+1,Cedge-1,3);///three parameter fit
      crossSecFit->SetParameters(1,Cedge,20.0);//begin the fitting from the generic Cedge
      crossSecFit->SetParLimits(0,0.2,2.0);
      crossSecFit->SetParLimits(1,Cedge,Cedge); //effectively fixing the Compton edge    
      crossSecFit->SetLineColor(kRed);
      grYield->Fit("crossSecFit","0 R M E q");
    }
    grB1L0 = new TGraphErrors((Int_t)trueStrip.size(), trueStrip.data(), yieldL0.data(), 0, yieldL0Er.data());
    grB1L0->SetFillColor(kBlue);
    grB1L0->SetLineColor(kBlue);
    grB1L0->SetMarkerColor(kBlue);

    grAsymDrAll = new TMultiGraph();
    grAsymDrAll->Add(grYield);
    grAsymDrAll->Add(grB1L0);

    //grAsymDrAll->SetTitle(Form(dataType+" Mode Yield, Plane %d",plane));
    grAsymDrAll->Draw("AB");//for some reason!:Tmultigraph wants the axis settings to come after having drawn the graph

    grAsymDrAll->GetXaxis()->SetTitle("Compton electron detector strip number");
    grAsymDrAll->GetYaxis()->SetTitle("qNorm Counts (Hz/uA)");
    //grAsymDrAll->GetXaxis()->SetNdivisions(416, kFALSE);
    grAsymDrAll->GetXaxis()->SetLimits(startStrip,endStrip+1); 

    legYield = new TLegend(0.101,0.75,0.44,0.9);
    legYield->AddEntry(grYield,"background corrected detector yield","lpe");
    if(kYieldFit) legYield->AddEntry("crossSecFit","theoretical cross-section","l");
    legYield->AddEntry(grB1L0,"background(Hz/uA)","lpe");
    legYield->SetFillColor(0);
    legYield->Draw();
    cYield->SaveAs(Form("%s/%s/%s"+dataType+"YieldFit.png",pPath,www,filePre.Data()));
    //delete cYield;
    //delete grYield;
    //delete grB1L0;
    //delete grAsymDrAll;
    //delete legYield;
  }

  //  tEnd = time(0);
  //  div_output = div((Int_t)difftime(tEnd, tStart),60);
  //  printf("\n it took %d minutes %d seconds to execute asymFit.C\n",div_output.quot,div_output.rem );  
  return 1;
  }

  /*Comments
   *In auto determination of Compton edge, I'm using (L1 - L0)/L0 instead of (L1 - L0)/L1 
   *..because in the latter both Nr and Dr reduce at the Compton edge, hence the former should be more pronounced
   *the use of qNormScBkgdSubSigB1[p][s]/qNormScB1L0[p][s] <= qNormBkgdSubSigToBkgdRatioLow ; is a better comparision
   *..for identifying compton edge compared to yieldL1[p][s]/qNormScB1L0[p][s] <= qNormBkgdSubSigToBkgdRatioLow
   *..since the former puts the magnitude of signal-over-bkgd in the perspective of the background; while the later 
   *..simply does a numerical comparision of the signal-over-bkgd.

   *The acceptance limit for the ratio of the background subtracted Signal over Signal is set at 10%
   *StripNum is a 2D array so as to hold the different set of strips that may be unmasked
   */
#endif

