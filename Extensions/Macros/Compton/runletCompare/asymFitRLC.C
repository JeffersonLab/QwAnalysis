//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry
#ifndef __asymFit_F
#define __asymFit_F
#endif

#include <rootClass.h>
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/comptonRunConstants.h"
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/rhoToX.C"
#include "/w/hallc/compton/users/narayan/svn/edetHelLC/infoDAQ.C"
const Bool_t kVladas_meth=0,kVladas_data=1;
const Bool_t kRadCor=1;//1:turn On radiation correction
///3 parameter method
Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par) {
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end() ) { 
    cout<<red<<"ignored strip: "<<*thisStrip<<normal<<endl;
    TF1::RejectPoint();
    return 0;
  }
  xStrip = xCedge - (par[1] -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as Cedge itself
  rhoStrip = param[0]+ xStrip*param[1]+ xStrip*xStrip*param[2]+ xStrip*xStrip*xStrip*param[3]+ xStrip*xStrip*xStrip*xStrip*param[4]+ xStrip*xStrip*xStrip*xStrip*xStrip*param[5];
  if(kRadCor) {
    eGamma = rhoStrip* kprimemax;
    betaBar=TMath::Sqrt(1.0 - (me/E)*(me/E)) ;
    betaCM = (betaBar*E-eLaser)/(E+eLaser) ; //eq. 2.10
    sEq211 = me*me + 2*eLaser*E*(1.0+betaCM) ;//eq. 2.11 
    gammaCM = (E+eLaser)/TMath::Sqrt(sEq211) ;
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
  dsdrho =((rhoStrip*(1.0 - a_const)*rhoStrip*(1.0 - a_const)/rhoMinus)+1.0+dsdrho1*dsdrho1);//eqn.22,no factor
  //cout<<"asym QED is: "<<(radCor*(par[2]*(rhoPlus*(1.0-1.0/(rhoMinus*rhoMinus)))/dsdrho))<<endl;
  return (radCor*(par[2]*(rhoPlus*(1.0-1.0/(rhoMinus*rhoMinus)))/dsdrho));//calcAsym;
}

Int_t asymFitRLC(Int_t runnum=23220,TString dataType="Ac")
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  //time_t tStart = time(0), tEnd; 
  //div_t div_output;
  //gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  gStyle->SetTitleOffset(1.05,"X");
  gStyle->SetTitleSize(0.05,"X");
  gStyle->SetTitleOffset(0.6,"Y");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetLabelSize(0.06,"xyz");

  const char *destDir = "/w/hallc/compton/users/narayan/runletCompare";
  //const char *sourceDir = "/u/home/narayan/acquired/vladas/FOR-AMRENDRA";
  const char *sourceDir = "/w/hallc/compton/users/narayan/runletCompare/asymRL_Apr2013";

  Double_t offset[nPlanes],offsetEr[nPlanes];
  //Double_t resFit[nPlanes],resFitEr[nPlanes], chiSqResidue[nPlanes];
  filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=1,debug2=0;
  Bool_t polSign;//,kResidual=1;
  Bool_t kFitEffWidth=0;///choose if you want to fit the effective strip width parameter or the CE as the second parameter
  TPaveText *pt[nPlanes];//, *ptRes[nPlanes];
  TLegend *leg[nPlanes];
  TLine *myline = new TLine(1,0,65,0);
  TF1 *polFit,*linearFit;

  if (!maskSet) infoDAQ(runnum);

  ifstream paramfile, expAsymPWTL1;
  ofstream polList;
  std::vector<std::vector <Double_t> > activeStrip,qNormCntsB1L0,qNormCntsB1L0Er;
  std::vector<Double_t > trueStrip,asym,asymEr;
  //Int_t numbGoodStrips[nPlanes]={0};
  Double_t stripNum[nPlanes][nStrips],qNormCntsB1L1[nPlanes][nStrips],qNormCntsB1L1Er[nPlanes][nStrips];

  ///Note: the 's' in this section of the routine does not necessarily represent strip number
  for(Int_t p =startPlane; p <endPlane; p++) {
    for(Int_t s = startStrip; s <endStrip; s++) {
      stripNum[p][s]=0.0,qNormCntsB1L1[p][s]=0.0,qNormCntsB1L1Er[p][s]=0.0;
    }
  }
  ///determine the compton edge
  Cedge[0] = 49.0; ///since the above condition is fulfiled after crossing Cedge

  TCanvas *cAsym;
  TGraphErrors *grAsymPlane[nPlanes];

  cAsym = new TCanvas("cAsym","Asymmetry and Strip number",10,10,1000,300*endPlane);
  cAsym->Divide(1,endPlane); 

  Double_t zero[nStrips];

  polList.open(Form("%s/asymRL"+dataType+"PolRadCor_%d.txt",destDir,runnum));
  polList<<";run\tpol\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\teffStrip\teffStripEr\tplane\tgoodCycles"<<endl;

  for (Int_t p =startPlane; p <endPlane; p++) {  
    if (!kVladas_meth) xCedge = rhoToX(p); ///this function should be called after determining the Cedge
    cout<<blue<<"maximum energy kprimemax: "<<kprimemax<<normal<<endl;
    paramfile.open(Form("%s/%s/checkfileP%d.txt",pPath,webDirectory,p+1));
    cout<<"reading in the rho to X fitting parameters for plane "<<p+1<<", they were:" <<endl;
    paramfile>>param[0]>>param[1]>>param[2]>>param[3]>>param[4]>>param[5];
    paramfile.close();
    if(debug) printf("%g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3]);

    cAsym->cd(p+1);  
    cAsym->GetPad(p+1)->SetGridx(1);

    if(kVladas_data) {
      expAsymPWTL1.open(Form("%s/run_%d_asymRL.txt",sourceDir,runnum));
      cout<<red<<"\nUsing asymmetry provided by Vladas\n"<<normal<<endl;
    } else expAsymPWTL1.open(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)); 
    //if(kVladas_data) expAsymPWTL1.open(Form("%s/as.plane.1.run.%d",sourceDir,runnum));
    
    Int_t c1=0;
    Double_t dummy1,dummy2,dummy3;
    if(expAsymPWTL1.is_open()) {
      if(debug2) cout<<"Reading the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug2) cout<<"stripNum\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      for(Int_t s =startStrip ; s < endStrip; s++) {
        if (std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) {
          //cout<<red<<"skipping strip "<<s+1<<normal<<endl;
          expAsymPWTL1>>dummy1>>dummy2>>dummy3;
          continue; 
        }
        trueStrip.push_back(0.0),asym.push_back(0.0),asymEr.push_back(0.0);
        expAsymPWTL1>>trueStrip[c1]>>asym[c1]>>asymEr[c1];
        if(debug2) cout<<blue<<trueStrip[c1]<<"\t"<<asym[c1]<<"\t"<<asymEr[c1]<<normal<<endl;
        c1++;
      }
      expAsymPWTL1.close();
    } else cout<<red<<"\nDidn't find the expAsym file for run "<<runnum<<" at the location"<<normal<<endl;

    grAsymPlane[p]=new TGraphErrors(nStrips,trueStrip.data(),asym.data(),zero,asymEr.data());
    grAsymPlane[p]->GetXaxis()->SetTitle("Strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");   
    grAsymPlane[p]->SetTitle(Form(dataType+" Mode Asymmetry, Plane %d",p+1));//Form("experimental asymmetry Run: %d, Plane %d",runnum,p+1));
    grAsymPlane[p]->SetMarkerStyle(kFullCircle);
    grAsymPlane[p]->SetLineColor(kRed);
    grAsymPlane[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
    grAsymPlane[p]->SetMaximum(0.048); 
    grAsymPlane[p]->SetMinimum(-0.048);
    grAsymPlane[p]->GetXaxis()->SetLimits(1,65); 
    grAsymPlane[p]->GetXaxis()->SetNdivisions(416, kFALSE);

    grAsymPlane[p]->Draw("AP");  
    tempCedge = Cedge[p];//-0.5;///this should be equated before the declaration of TF1
    linearFit = new TF1("linearFit", "pol0",startStrip+1,tempCedge);
    linearFit->SetLineColor(kRed);

    ///3 parameter fit
    polFit = new TF1("polFit",theoreticalAsym,startStrip+1,tempCedge+1,3);
    //polFit = new TF1("polFit",theoreticalAsym,startStrip+1,endStrip,3);
    //TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+10,Cedge[p],3);//use strips after the first 10 strips
    polFit->SetParameters(1.0033,tempCedge,0.85);//begin the fitting from stripWidth parameter = 1, Cedge=auto-determined, polarization=85%
    if (kFitEffWidth) {
      polFit->SetParLimits(0,0.8,1.8);///allowing the strip width to be either 80% or 180% of its real pitch   
      polFit->SetParLimits(1,tempCedge,tempCedge);///fixed compton edge
      cout<<blue<<"using effective strip width and pol as the two fit parameters"<<normal<<endl;
      cout<<blue<<"CE fixed at strip "<<normal<<tempCedge<<endl;
    } else {
      //polFit->SetParLimits(0,1.0,1.0);///fix the effective strip width to 1.0
      polFit->SetParLimits(0,1.0033,1.0033);///fix the effective strip width to 1.0033 !changed to match the runlet analysis output
      polFit->SetParLimits(1,45.0,55.0);///allow the CE to vary between strip 30-63
      cout<<blue<<"using CE and pol as the two fit parameters"<<normal<<endl;
      //cout<<blue<<"effective strip width fixed at "<<normal<<<GetParameter[0]<endl;
    }
    polFit->SetParLimits(2,-0.92,0.92);///allowing polarization to be - 100% to +100%
    polFit->SetParNames("effStrip","comptonEdge","polarization");
    polFit->SetLineColor(kBlue);
    cout<<red<<"the maxdist used:"<<xCedge<<normal<<endl;
    grAsymPlane[p]->Fit("polFit","0 R M E");
    polFit->DrawCopy("same");
    offset[p] = polFit->GetParameter(1);
    offsetEr[p] = polFit->GetParError(1);
    pol = polFit->GetParameter(2);
    polEr = polFit->GetParError(2);

    effStripWidth = polFit->GetParameter(0);
    effStripWidthEr = polFit->GetParError(0);

    chiSq = polFit->GetChisquare();
    NDF = polFit->GetNDF();

    if(debug) cout<<"\nwrote the polarization relevant values to file "<<endl;
    polList<<Form("%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%2.3f\t%.3f\t%d\t%d\n",(Double_t)runnum,pol*100,polEr*100,chiSq,NDF,offset[p],offsetEr[p],effStripWidth,effStripWidthEr,p+1,asymflag);
    if(debug) {
      cout<<Form("runnum\tpol.\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\teffStripWidth effStripWidthEr plane");
      cout<<Form("\n%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%2.3f\t%.3f\t%d\n\n",(Double_t)runnum,pol*100,polEr*100,chiSq,NDF,offset[p],offsetEr[p],effStripWidth,effStripWidthEr,p+1);      
    }
    leg[p] = new TLegend(0.101,0.75,0.43,0.9);
    leg[p]->AddEntry(grAsymPlane[0],"experimental asymmetry","lpe");///I just need the name
    leg[p]->AddEntry("polFit","QED asymmetry fit to exp. asymmetry","l");//"lpf");//
    leg[p]->SetFillColor(0);
    leg[p]->Draw();

    polSign = pol > 0 ? 1 : 0;
    if (polSign) pt[p] = new TPaveText(0.44,0.12,0.68,0.48,"brNDC");///left edge,bottom edge,right edge, top edge
    else  pt[p] = new TPaveText(0.44,0.52,0.68,0.88,"brNDC");
    if(debug) cout<<"polSign is: "<<polSign<<endl;

    pt[p]->SetTextSize(0.060);//0.028); 
    pt[p]->SetBorderSize(1);
    pt[p]->SetTextAlign(12);
    pt[p]->SetFillColor(-1);
    pt[p]->SetShadowColor(-1);

    pt[p]->AddText(Form("chi Sq / ndf       : %.3f",chiSq/NDF));
    //pt[p]->AddText(Form("Compton Edge      : %f #pm %f",Cedge[p]+offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Compton Edge : %2.2f #pm %2.2f",offset[p],offsetEr[p]));
    pt[p]->AddText(Form("Polarization      : %2.2f #pm %2.2f",pol*100.0,polEr*100.0));
    pt[p]->Draw();
    myline->Draw();
  }//for (Int_t p =startPlane; p <endPlane; p++)

  //if(kResidual) {
  //  Double_t fitResidue[nPlanes][nStrips];
  //  TCanvas *cResidual;
  //  TGraphErrors *grResiduals[nPlanes]; 
  //  cResidual = new TCanvas("cResidual","residual (Asymmetry - Fit)",10,500,900,300*endPlane);
  //  cResidual->Divide(1,endPlane); 

  //  for (Int_t p =startPlane; p <endPlane; p++) {
  //    ///determining the residue of the above fit
  //    for (Int_t s = startStrip; s < endStrip; s++) {//ensure that all variables gets initiated by zero
  //      qNormCntsB1L1[p][s]=0.0,stripAsymNr[p][s]=0.0,qNormCntsB1L1Er[p][s]=0.0;
  //      zero[s] = 0.0;
  //    }

  //    if(debug2) cout<<"fitResidue[p][s]\tstripAsym[p][s]\tpolFit->Eval(s+1)"<<endl;
  //    for (Int_t s = startStrip; s <= Cedge[p]; s++) {
  //      //if (!mask[p][s]) continue;
  //      fitResidue[p][s] = asym[s] - polFit->Eval(s+1);
  //      if(debug2) cout<<fitResidue[p][s]<<"\t"<<asym[s]<<"\t"<<polFit->Eval(s+1)<<endl;
  //    }
  //    cResidual->cd(p+1);  
  //    cResidual->GetPad(p+1)->SetGridx(1);
  //    grResiduals[p]=new TGraphErrors((Int_t)Cedge[p],trueStrip.data(),fitResidue[p],zero,asymEr.data());
  //    grResiduals[p]->SetMarkerStyle(kOpenCircle);
  //    grResiduals[p]->SetMaximum(0.012);/// half of asymmetry axis 
  //    grResiduals[p]->SetMinimum(-0.012);/// half of asymmetry axis 
  //    grResiduals[p]->GetXaxis()->SetLimits(1,Cedge[p]); 
  //    grResiduals[p]->SetTitle(Form(dataType+" Mode Asymmetry Fit Residual, Plane %d",p+1));
  //    grResiduals[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
  //    grResiduals[p]->GetYaxis()->SetTitle("asymmetry - Fit");

  //    grResiduals[p]->Draw("AP");
  //    grResiduals[p]->Fit(linearFit,"REq");//q:quiet mode

  //    resFit[p] = linearFit->GetParameter(0);
  //    resFitEr[p] = linearFit->GetParError(0);
  //    chiSqResidue[p] = linearFit->GetChisquare();
  //    resFitNDF = linearFit->GetNDF();

  //    ptRes[p] = new TPaveText(0.67,0.67,0.9,0.9,"brNDC");///x1,y1,x2,y2
  //    ptRes[p]->SetTextSize(0.048);//0.028); 
  //    ptRes[p]->SetBorderSize(1);
  //    ptRes[p]->SetTextAlign(12);
  //    ptRes[p]->SetFillColor(0);
  //    ptRes[p]->SetShadowColor(-1);
  //    ptRes[p]->AddText(Form("chi Sq / ndf  : %0.1f / %d",chiSqResidue[p],resFitNDF));
  //    ptRes[p]->AddText(Form("linear fit    : %f #pm %f",resFit[p],resFitEr[p]));
  //    ptRes[p]->Draw();
  //  }//for (Int_t p =startPlane; p <endPlane; p++)
  //  cResidual->SaveAs(Form("%s/%s/%s"+dataType+"AsymFitResidual.png",pPath,webDirectory,filePrefix.Data()));
  //}

  polList.close();

  //if(kVladas_data && !kVladas_meth) cAsym->SaveAs(Form("%s/r%d_asymFitRLData_anMeth.png",destDir,runnum));

  //  tEnd = time(0);
  //  div_output = div((Int_t)difftime(tEnd, tStart),60);
  //  printf("\n it took %d minutes %d seconds to execute asymFit.C\n",div_output.quot,div_output.rem );  
  return 1;
}

