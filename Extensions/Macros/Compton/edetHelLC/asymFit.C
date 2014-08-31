//attempting to fit the theoretically evaluated asymmetry to experimentally measured asymmetry
#ifndef __asymFit_F
#define __asymFit_F
#endif

#include <rootClass.h>
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "infoDAQ.C"
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

///3 parameter method
const Bool_t kRadCor=1;
Double_t theoreticalAsym(Double_t *thisStrip, Double_t *par)
{
  //itStrip = skipStrip.find(*thisStrip); //for some reason the std::set did not work !
  itStrip = find(skipStrip.begin(),skipStrip.end(),*thisStrip);
  if(itStrip != skipStrip.end() ) { 
    //cout<<red<<"ignored strip: "<<*thisStrip<<normal<<endl;
    TF1::RejectPoint();
    return 0;
  }
  //xStrip = xCedge - (tempCedge + par[1] - (*thisStrip))*stripWidth*par[0];//for 2nd parameter as Cedge offset
  //xStrip = xCedge + par[1] - (tempCedge -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as actual position inside Cedge strip
  //xStrip = xCedge + par[1]*stripWidth - (*thisStrip)*stripWidth*par[0];//Guruji's method of fitting!!didn't work
  //xStrip = xCedge -0.5*stripWidth - (par[1] -(*thisStrip))*stripWidth*par[0]; //for 2nd parameter as Cedge itself
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
  dsdrho =((rhoStrip*(1.0 - a_const)*rhoStrip*(1.0 - a_const)/rhoMinus)+1.0+dsdrho1*dsdrho1);//eqn.22,without factor 2*pi*(re^2)/a_const
  //Double_t calcAsym=(par[0]*(-1*IHWP)*(rhoPlus*(1-1/(rhoMinus*rhoMinus)))/dsdrho);//eqn.24,without factor 2*pi*(re^2)/a
  return (radCor*(par[2]*(rhoPlus*(1.0-1.0/(rhoMinus*rhoMinus)))/dsdrho));//calcAsym;
}

Int_t asymFit(Int_t runnum=24519,TString dataType="Ac")
{
  cout<<"\nStarting into asymFit.C **************\n"<<endl;
  if(kRadCor) cout<<blue<<"Radiative correction being applied"<<normal<<endl;
  else cout<<blue<<"radiative correction turned OFF"<<normal<<endl;
  //  time_t tStart = time(0), tEnd; 
  //  div_t div_output;
  //gStyle->SetOptFit(1);
  gStyle->SetPalette(1);
  gStyle->SetPadBorderSize(3);
  gStyle->SetFrameLineWidth(3);
  gStyle->SetTitleOffset(1.05,"X");
  gStyle->SetTitleSize(0.05,"X");
  gStyle->SetTitleOffset(0.6,"Y");
  gStyle->SetTitleSize(0.06,"Y");
  gStyle->SetLabelSize(0.06,"xyz");

  filePrefix = Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  Bool_t debug=1,debug1=1,debug2=1;
  Bool_t polSign,kYieldFit=0,kYield=1,kResidual=1, kBgdAsym=1;
  Bool_t kFitEffWidth=0;///choose if you want to fit the effective strip width parameter or the CE as the second parameter
  Bool_t kFoundCE[nPlanes]={0};
  TPaveText *pt[nPlanes], *ptRes[nPlanes];
  TLegend *leg[nPlanes],*legYield[nPlanes];
  TLine *myline = new TLine(1,0,65,0);
  TF1 *polFit;

  if (!maskSet) infoDAQ(runnum);

  ifstream paramfile,infileL0Yield, expAsymPWTL1, infileYield, fBgdAsym;
  ofstream polList,fitInfo;
  std::vector<std::vector <Double_t> > activeStrip;
  std::vector<Double_t > trueStrip,asym,asymEr;
  std::vector<Double_t > yieldL1,yieldL1Er,asymNr;
  std::vector<Double_t > yieldL0,yieldL0Er,zero;
  std::vector<Double_t > stripCE, zero1;
  Int_t usedStrips=0;///no.of used strips upto the CE
  Double_t dum1,dum2,dum3,dum4;

  ///Read in all files that are needed
  for(Int_t p =startPlane; p <endPlane; p++) {
    expAsymPWTL1.open(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if(expAsymPWTL1.is_open()) {
      if(debug2) cout<<"Reading the expAsym corresponding to PWTL1 for Plane "<<p+1<<endl;
      if(debug2) cout<<"strip\t"<<"stripAsym\t"<<"stripAsymEr"<<endl;
      while( expAsymPWTL1>>dum1>>dum2>>dum3 && expAsymPWTL1.good() ) {
        if (std::find(skipStrip.begin(),skipStrip.end(),dum1)!=skipStrip.end()) {
          cout<<blue<<"skipping strip "<<dum1<<normal<<endl;
          continue; 
        }
        trueStrip.push_back(dum1),asym.push_back(dum2),asymEr.push_back(dum3);
        if(debug2) cout<<blue<<trueStrip.back()<<"\t"<<asym.back()<<"\t"<<asymEr.back()<<normal<<endl;
      }
      expAsymPWTL1.close();
    } else cout<<"did not find the expAsym file "<<Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<endl;

    infileL0Yield.open(Form("%s/%s/%s%sLasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),dataType.Data(),p+1));
    if(infileL0Yield.is_open()) {
      if(debug1) cout<<"Reading the qNorm Laser Off Yield for Plane "<<p+1<<endl;
      while(infileL0Yield>>dum1>>dum2>>dum3 && infileL0Yield.good()) {
        if (std::find(skipStrip.begin(),skipStrip.end(),dum1)!=skipStrip.end()) {
          cout<<magenta<<"skipping strip "<<dum1<<normal<<endl;
          continue; 
        }
        yieldL0.push_back(dum2), yieldL0Er.push_back(dum3), zero.push_back(0.0);
      }
      infileL0Yield.close();
    }
    infileYield.open(Form("%s/%s/%s%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),dataType.Data(),p+1));
    if(infileYield.is_open()) {
      if(debug1) cout<<"Reading the qNorm Laser On Yield for Plane "<<p+1<<endl;
      while(infileYield>>dum1>>dum2>>dum3>>dum4 && infileYield.good()) {
        if (std::find(skipStrip.begin(),skipStrip.end(),dum1)!=skipStrip.end()) {
          cout<<blue<<"skipping strip "<<dum1<<normal<<endl;
          continue; 
        }
        yieldL1.push_back(dum2), yieldL1Er.push_back(dum3), asymNr.push_back(dum4);
      }
      infileYield.close();
    }
    ///Estimate the compton edge location
    for(Int_t s=0; s<=(Int_t)trueStrip.size(); s++) {
      //Int_t s = trueStrip[i];///actual strip number
      //if((((yieldL1[s]-yieldL0[s]))< (yieldL0[s]+100.0*yieldL0Er[s])) && trueStrip[s]>edgeClamp) {
      if( (yieldL1.at(s)-yieldL0.at(s) < 100.0*yieldL0Er.at(s) ) && s>25) {
        ///there are cases when strip-1 has beamOff counts higher than bgd sub counts due to halo eg.run23246
        ///hence the above condition of s>25 is applied to ensure that trueStrip.at(s-1) does not access strip-1
        Cedge[p] = trueStrip.at(s-1); ///since the above condition is fulfiled after crossing Cedge
        cout<<"probable Cedge : "<<Cedge[p]<<endl;
        kFoundCE[p] = 1;
        break;
      }
    }
    if(kFoundCE[p]) cout<<"\nCompton edge for plane "<<p+1<<" auto-determined to strip "<<Cedge[p]<<"\n"<<endl;
    else cout<<red<<"**** Alert: Did not find Cedge for plane "<<p+1<<" in run # "<<runnum<<" till the last strip"<<normal<<endl;
    }///for(Int_t p =startPlane; p <endPlane; p++)

    TCanvas *cAsym;
    TGraphErrors *grAsym[nPlanes];

    cAsym = new TCanvas("cAsym","Asymmetry and Strip number",10,10,1000,300*endPlane);
    cAsym->Divide(1,endPlane); 

    for (Int_t p =startPlane; p <endPlane; p++) {  
      xCedge = rhoToX(p); ///this function should be called after determining the Cedge

      paramfile.open(Form("%s/%s/checkfileP%d.txt",pPath,webDirectory,p+1));
      cout<<"reading in the rho to X fitting parameters for plane "<<p+1<<", they were:" <<endl;
      paramfile>>param[0]>>param[1]>>param[2]>>param[3]>>param[4]>>param[5];
      paramfile.close();
      if(debug) printf("%g\t%g\t%g\t%g\t%g\t%g\n",param[0],param[1],param[2],param[3],param[4],param[5]);

      cAsym->cd(p+1);  
      cAsym->GetPad(p+1)->SetGridx(1);

      grAsym[p]=new TGraphErrors((Int_t)trueStrip.size(),trueStrip.data(),asym.data(),zero.data(),asymEr.data()); 
      //grAsym[p]=new TGraphErrors("/home/narayan/acquired/vladas/r24519_lasCycAsym_runletErr.txt","%lg %lg %lg");  
      //else grAsym[p]=new TGraphErrors(Form("%s/%s/%s"+dataType+"ExpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");

      grAsym[p]->GetXaxis()->SetTitle("Strip number");
      grAsym[p]->GetYaxis()->SetTitle("asymmetry");   
      //grAsym[p]->SetTitle(Form(dataType+" Mode Asymmetry, Plane %d",p+1));//Form("experimental asymmetry Run: %d, Plane %d",runnum,p+1));
      grAsym[p]->SetMarkerStyle(kFullCircle);
      grAsym[p]->SetLineColor(kRed);
      grAsym[p]->SetMarkerColor(kRed); ///kRed+2 = Maroon
      grAsym[p]->SetMaximum(0.048); 
      grAsym[p]->SetMinimum(-0.048);
      grAsym[p]->GetXaxis()->SetLimits(1,65); 
      //grAsym[p]->GetXaxis()->SetNdivisions(416, kFALSE);

      grAsym[p]->Draw("AP");
      tempCedge = Cedge[p];//-0.5;///this should be equated before the declaration of TF1

      ///3 parameter fit
      polFit = new TF1("polFit",theoreticalAsym,startStrip+1,tempCedge+1,3);
      //polFit = new TF1("polFit",theoreticalAsym,startStrip+1,endStrip,3);
      //TF1 *polFit = new TF1("polFit",theoreticalAsym,startStrip+10,Cedge[p],3);//use strips after the first 10 strips
      polFit->SetParameters(1.0033,tempCedge,0.89);//begin the fitting from stripWidth parameter = 1, Cedge=auto-determined, polarization=85%
      if (kFitEffWidth) {
        polFit->SetParLimits(0,0.8,1.8);///allowing the strip width to be either 80% or 180% of its real pitch   
        polFit->SetParLimits(1,tempCedge,tempCedge);///fixed compton edge
        cout<<blue<<"using effective strip width and pol as the two fit parameters"<<normal<<endl;
        cout<<blue<<"CE fixed at strip "<<normal<<tempCedge<<endl;
      } else {
        //polFit->SetParLimits(0,1.0,1.0);///fix the effective strip width to 1.0
        polFit->SetParLimits(0,1.0033,1.0033);///fix the effective strip width to 1.0033 !changed to match the runlet analysis output
        polFit->SetParLimits(1,30.0,60.0);///allow the CE to vary between strip 30-63
        cout<<blue<<"using CE and pol as the two fit parameters"<<normal<<endl;
        //cout<<blue<<"effective strip width fixed at "<<normal<<<GetParameter[0]<endl;
      }
      polFit->SetParLimits(2,-0.93,0.93);///allowing polarization to be - 100% to +100%
      polFit->SetParNames("effStrip","comptonEdge","polarization");
      polFit->SetLineColor(kBlue);
      cout<<red<<"the maxdist used:"<<xCedge<<normal<<endl;
      TVirtualFitter::SetMaxIterations( 10000 );
      grAsym[p]->Fit("polFit","N R M E");
      polFit->DrawCopy("same");
      cEdge = polFit->GetParameter(1);
      cEdgeEr = polFit->GetParError(1);
      pol = polFit->GetParameter(2);
      polEr = polFit->GetParError(2);

      effStripWidth = polFit->GetParameter(0);
      effStripWidthEr = polFit->GetParError(0);

      chiSq = polFit->GetChisquare();
      NDF = polFit->GetNDF();

      if(debug) cout<<"\nwriting the polarization relevant values to file "<<endl;
      polList.open(Form("%s/%s/%s"+dataType+"Pol.txt",pPath,webDirectory,filePrefix.Data()));
      polList<<";run\tpol\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\teffStrip\teffStripEr\tplane\tgoodCycles"<<endl;
      polList<<Form("%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%2.3f\t%.3f\t%d\t%d\n",(Double_t)runnum,pol*100,polEr*100,chiSq,NDF,cEdge,cEdgeEr,effStripWidth,effStripWidthEr,p+1,asymflag);
      if(debug) {
        cout<<Form("runnum\tpol.\tpolEr\tchiSq\tNDF\tCedge\tCedgeEr\teffStripWidth effStripWidthEr plane");
        cout<<Form("\n%5.0f\t%2.2f\t%.2f\t%.2f\t%d\t%2.2f\t%.2f\t%2.3f\t%.3f\t%d\n\n",(Double_t)runnum,pol*100,polEr*100,chiSq,NDF,cEdge,cEdgeEr,effStripWidth,effStripWidthEr,p+1);      
      }
      leg[p] = new TLegend(0.101,0.73,0.30,0.9);
      //leg[p]->AddEntry(grAsym[0],"experimental asymmetry","lpe");///I just need the name
      leg[p]->AddEntry(grAsym[0],"measured","lpe");///I just need the name
      leg[p]->AddEntry("polFit","QED fit","l");//"lpf");//
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
      pt[p]->AddText(Form("Compton Edge : %2.2f #pm %2.2f",cEdge,cEdgeEr));
      pt[p]->AddText(Form("Polarization      : %2.2f #pm %2.2f",pol*100.0,polEr*100.0));
      pt[p]->Draw();
      myline->Draw();
    }//for (Int_t p =startPlane; p <endPlane; p++)
    polList.close();

    for (Int_t s = startStrip; s < (Int_t)trueStrip.size(); s++) {///trueStrip will be the no.of used strips including post CE
      if (trueStrip.at(s) < cEdge) usedStrips++;  ///cEdge determined in preceeding section 
    }
    if(debug) cout<<blue<<"used a total of "<<usedStrips<<" strips upto the CE"<<normal<<endl;
    TF1 *linearFit = new TF1("linearFit", "pol0",startStrip+1,cEdge);
    linearFit->SetLineColor(kRed);

    if(kBgdAsym) {
      //!This is not meant to be able to deal with different planes, the need wasn't felt
      TGraphErrors *grBgd; 
      TCanvas *cBgd = new TCanvas("cBgd","Bgd Asymmetry",10,650,900,300*endPlane);
      TPaveText *ptBgd;

      //ifstream fBgdAsym;
      std::vector <Double_t> bgdAsym, bgdAsymEr;
      ///Read in the background asymmetry file
      cBgd->cd();
      for (Int_t p =startPlane; p <endPlane; p++) {
        fBgdAsym.open(Form("%s/%s/%s"+dataType+"BkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
        //fBgdAsym.open(Form("%s/%s/%s"+dataType+"BkgdAymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
        if(fBgdAsym.is_open()) {
          if(debug2) cout<<"Reading the bgdAsym file for Plane "<<p+1<<endl;
          if(debug2) cout<<"strip\t"<<"bgdAsym\t"<<"bgdAsymEr"<<endl;
          while( fBgdAsym>>dum1>>dum2>>dum3 && fBgdAsym.good() ) {
            if (std::find(skipStrip.begin(),skipStrip.end(),dum1)!=skipStrip.end()) {
              if(debug1) cout<<green<<"skipping strip "<<dum1<<normal<<endl;
              continue; 
            }
            bgdAsym.push_back(dum2),bgdAsymEr.push_back(dum3);
            if(debug2) cout<<blue<<dum1<<"\t"<<bgdAsym.back()<<"\t"<<bgdAsymEr.back()<<normal<<endl;
          }
          fBgdAsym.close();
        } else cout<<red<<"did not find the bgdAsym file "<<Form("%s/%s/%s"+dataType+"BkgdAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<normal<<endl;
        //cout<<(Form("%s/%s/%s"+dataType+"BkgdAymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1))<<endl;

        grBgd = new TGraphErrors((Int_t)trueStrip.size(),trueStrip.data(),bgdAsym.data(),zero.data(),bgdAsymEr.data());
        grBgd->Draw("AP");
        grBgd->GetXaxis()->SetTitle("Strip number");
        grBgd->GetYaxis()->SetTitle("bgd asymmetry");   
        grBgd->SetTitle(0);
        grBgd->SetMarkerStyle(kFullCircle);
        grBgd->SetLineColor(kBlue);
        grBgd->SetMarkerColor(kBlue); ///kRed+2 = Maroon
        //grBgd->GetXaxis()->SetLimits(1,65); 
        grBgd->Fit(linearFit,"RE");//q:quiet mode

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
      }
    }

    if(kResidual) {
      TCanvas *cResidual;
      TGraphErrors *grResiduals[nPlanes]; 
      cResidual = new TCanvas("cResidual","residual (Asymmetry - Fit)",10,500,900,300*endPlane);
      cResidual->Divide(1,endPlane); 
      std::vector<Double_t> residueEr; 

      for (Int_t p =startPlane; p <endPlane; p++) {
        std::vector<Double_t> fitResidue;
        ///determining the residue of the above fit
        if(debug2) cout<<"strip\tfitResidue.at(s)\tstripAsym.at(s)\tpolFit->Eval(s)"<<endl;
        for (Int_t s = startStrip; s < usedStrips; s++) {
          stripCE.push_back(trueStrip.at(s));
          residueEr.push_back(asymEr.at(s));
          zero1.push_back(0.0);
          fitResidue.push_back( asym.at(s) - polFit->Eval(trueStrip.at(s)) );
          if(debug2) cout<<trueStrip.at(s)<<"\t"<<fitResidue.at(s)<<"\t"<<asym.at(s)<<"\t"<<polFit->Eval(trueStrip.at(s))<<endl;
        }

        cResidual->cd(p+1);  
        cResidual->GetPad(p+1)->SetGridx(1);
        grResiduals[p]=new TGraphErrors((Int_t)stripCE.size(),stripCE.data(),fitResidue.data(),zero1.data(),residueEr.data());
        grResiduals[p]->SetMarkerStyle(kOpenCircle);
        //grResiduals[p]->SetMaximum(0.012);/// half of asymmetry axis 
        //grResiduals[p]->SetMinimum(-0.012);/// half of asymmetry axis 
        //grResiduals[p]->GetXaxis()->SetLimits(1,Cedge[p]); 
        //grResiduals[p]->SetTitle(Form(dataType+" Mode Asymmetry Fit Residual, Plane %d",p+1));
        grResiduals[p]->SetTitle(0);
        grResiduals[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
        grResiduals[p]->GetYaxis()->SetTitle("asymmetry - Fit");

        grResiduals[p]->Draw("AP");
        grResiduals[p]->Fit(linearFit,"REq");//q:quiet mode

        resFit = linearFit->GetParameter(0);
        resFitEr = linearFit->GetParError(0);
        chiSqResidue = linearFit->GetChisquare();
        resFitNDF = linearFit->GetNDF();

        ptRes[p] = new TPaveText(0.67,0.67,0.9,0.9,"brNDC");///x1,y1,x2,y2
        ptRes[p]->SetTextSize(0.048);//0.028); 
        ptRes[p]->SetBorderSize(1);
        ptRes[p]->SetTextAlign(12);
        ptRes[p]->SetFillColor(0);
        ptRes[p]->SetShadowColor(-1);
        ptRes[p]->AddText(Form("chi Sq / ndf  : %0.1f / %d",chiSqResidue,resFitNDF));
        ptRes[p]->AddText(Form("linear fit    : %f #pm %f",resFit,resFitEr));
        ptRes[p]->Draw();
      }//for (Int_t p =startPlane; p <endPlane; p++)
      cResidual->SaveAs(Form("%s/%s/%s"+dataType+"AsymFitResidual.png",pPath,webDirectory,filePrefix.Data()));
    }
      fitInfo.open(Form("%s/%s/%s"+dataType+"FitInfo.txt",pPath,webDirectory,filePrefix.Data()));
      fitInfo<<";run\tresFit\tresFitEr\tchiSqRes\tresNDF\tbgdAsymFit\tbgdAsymFitEr\tchiSqBgd\tbgdNDF"<<endl;
      fitInfo<<Form("%5.0f\t%.6f\t%.6f\t%.2f\t%d\t%.6f\t%.6f\t%.2f\t%d\n",(Double_t)runnum,resFit,resFitEr,chiSqResidue,resFitNDF,bgdAsymFit,bgdAsymFitEr,chiSqBgdAsym,bgdAsymFitNDF);
      fitInfo.close();

    if (kYield) { ///determine yield
      TCanvas *cYield;
      TGraphErrors *grYieldPlane[nPlanes],*grB1L0[nPlanes];
      TMultiGraph *grAsymDrAll[nPlanes];
      cYield = new TCanvas("cYield","detector yield",300,200,1000,300*endPlane);
      cYield->Divide(1,endPlane); 

      for (Int_t p =startPlane; p <endPlane; p++) {
        cYield->cd(p+1);
        cYield->GetPad(p+1)->SetGridx(1);
        grYieldPlane[p] = new TGraphErrors((Int_t)trueStrip.size(), trueStrip.data(), yieldL1.data(), zero.data(), yieldL1Er.data());
        //grYieldPlane[p]=new TGraphErrors(Form("%s/%s/%s"+dataType+"YieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1),"%lg %lg %lg");
        grYieldPlane[p]->SetLineColor(kGreen);
        grYieldPlane[p]->SetFillColor(kGreen);   
        grYieldPlane[p]->SetMarkerColor(kGreen); ///kRed+2 = Maroon

        ///3 parameter fit for cross section
        if(kYieldFit) {
          TF1 *crossSecFit = new TF1("crossSecFit",theoCrossSec,startStrip+1,Cedge[p]-1,3);///three parameter fit
          crossSecFit->SetParameters(1,Cedge[p],20.0);//begin the fitting from the generic Cedge
          crossSecFit->SetParLimits(0,0.2,2.0);
          crossSecFit->SetParLimits(1,Cedge[p],Cedge[p]); //effectively fixing the Compton edge    
          crossSecFit->SetLineColor(kRed);
          grYieldPlane[p]->Fit("crossSecFit","0 R M E q");
        }
        //grB1L0[p] = new TGraphErrors(Form("%s/%s/%s"+dataType+"LasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1), "%lg %lg %lg");
        grB1L0[p] = new TGraphErrors((Int_t)trueStrip.size(), trueStrip.data(), yieldL0.data(), zero.data(), yieldL0Er.data());
        grB1L0[p]->SetFillColor(kBlue);
        grB1L0[p]->SetLineColor(kBlue);
        grB1L0[p]->SetMarkerColor(kBlue);

        grAsymDrAll[p] = new TMultiGraph();
        grAsymDrAll[p]->Add(grYieldPlane[p]);
        grAsymDrAll[p]->Add(grB1L0[p]);

        //grAsymDrAll[p]->SetTitle(Form(dataType+" Mode Yield, Plane %d",p+1));
        grAsymDrAll[p]->Draw("AB");//for some reason!:Tmultigraph wants the axis settings to come after having drawn the graph

        grAsymDrAll[p]->GetXaxis()->SetTitle("Compton electron detector strip number");
        grAsymDrAll[p]->GetYaxis()->SetTitle("qNorm Counts (Hz/uA)");
        //grAsymDrAll[p]->GetXaxis()->SetNdivisions(416, kFALSE);
        grAsymDrAll[p]->GetXaxis()->SetLimits(1,65); 

        legYield[p] = new TLegend(0.101,0.75,0.44,0.9);
        legYield[p]->AddEntry(grYieldPlane[p],"background corrected detector yield","lpe");
        if(kYieldFit) legYield[p]->AddEntry("crossSecFit","theoretical cross-section","l");
        legYield[p]->AddEntry(grB1L0[p],"background(Hz/uA)","lpe");
        legYield[p]->SetFillColor(0);
        legYield[p]->Draw();
      }//for (Int_t p =startPlane; p <endPlane; p++)
      cYield->SaveAs(Form("%s/%s/%s"+dataType+"YieldFit.png",pPath,webDirectory,filePrefix.Data()));
    }

    cAsym->SaveAs(Form("%s/%s/%s"+dataType+"AsymFit.png",pPath,webDirectory,filePrefix.Data()));
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
