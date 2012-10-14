#ifndef __MASKEDSTRIPS_F
#define __MASKEDSTRIPS_F

#include "comptonRunConstants.h"

// ///True for MCM calibration runs
// Bool_t maskedStrips(Int_t plane,Int_t strip)
// {
//   if(plane==0&&(strip==1)) return kTRUE;//skip masked strip of plane1
//   else if(plane==1&&(strip==1||strip==11)) return kTRUE;//skip masked strip of plane2
//   else if(plane==2&&(strip==38||strip==52||strip==63)) return kTRUE;//skip masked strip of plane3
//   else return kFALSE;
// }

///True for beam current variation runs[run# 23142 - 23168]
Bool_t maskedStrips(Int_t plane,Int_t strip)
{
  if(plane==0&&(strip==5||strip==19||strip==62)) return kTRUE;//skip masked strip of plane1
  else if(plane==1&&(strip==1||strip==11)) return kTRUE;//skip masked strip of plane2
  else if(plane==2&&(strip==38||strip==52||strip==63)) return kTRUE;//skip masked strip of plane3
  else return kFALSE;
}

///This was true for early run2 (eg.24519)
// Bool_t maskedStrips(Int_t plane,Int_t strip)
// { ///planes as well as strip in C++ counting
//   if(plane==0&& (strip==1 || strip==5 || strip==19)) return kTRUE;//skip masked strip of plane1
//   else if(plane==1&& (strip==11)) return kTRUE;//skip masked strip of plane2
//   else if(plane==2&& (strip==23 || strip==38 || strip==52 || strip==63)) return kTRUE;//skip masked strip of plane3
//   else return kFALSE;
// }

void rhoToX()//Double_t param[4]) //!this function as of now, may not work,some lines were commented for some quick work
{
  Double_t param[4];
  Double_t xPrime[nPoints],rho[nPoints];
  ofstream QEDasym,paramOutFile;
  Float_t re,R_bend,kprimemax,asymmax,rho0,k0prime,p_beam,r,h,kk,x1,kDummy;//k,gamma,a
  Float_t p_edge,r_edge,th_edge,hprime,kprime,x2;//,maxdist,rho;
  Float_t thetabend = chicaneBend*pi/180; //(radians)
  Float_t det_angle = th_det*3.1416/180;//(radians)

  re = alpha*hbarc/me;
  gamma_my=E/me; //electron gamma, eqn.20
  R_bend = (gamma_my*hbarc)/(2.0*xmuB*B_dipole);
  k =2*pi*hbarc/(lambda); // incident photon energy (GeV)
//   a =1/(1+4*k*gamma_my/me); // eqn.15 
  kprimemax=4*a*k*gamma_my*gamma_my; //eqn.16{max recoil photon energy} (GeV)
  asymmax=(1-a)*(1+a)/(1+a*a);

  rho0=1/(1+a);
  k0prime=rho0*kprimemax;
  //  dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)

  p_beam =sqrt(E*E - me*me);
  r =p_beam/me*(hbarc/(2*xmuB*B_dipole));//!?
  h = r - lmag/tan(thetabend);
  kk =ldet*tan(thetabend);
  x1 =(kk+h)*cos(det_angle);
  kDummy = kprimemax; ///initiating 
 
  QEDasym.open(Form("%s/%s/QEDasym.txt",pPath,webDirectory));
  for (Int_t i = 0; i <nPoints; i++) {//xPrime[nPoints],rho[nPoints];
    xPrime[i]=0.0; ///initialize
    rho[i]=0.0;
    p_edge=p_beam-kDummy;
    r_edge=(p_edge/me)*(hbarc/(2*xmuB*B_dipole));
    th_edge=asin((p_beam/p_edge)*sin(thetabend));
    hprime = r_edge - lmag/tan(th_edge);
    //hprime = r_edge*(1-cos(th_edge));
    kprime=ldet*tan(th_edge);
    x2= (kprime + hprime)*cos(det_angle);
    if(x2>x1) {
      xPrime[i]=(x2-x1);
      rho[i]=kDummy/kprimemax;
      kDummy=kDummy-0.00005;
      if(QEDasym.is_open())
	QEDasym<<xPrime[i]<<"\t"<<rho[i]<<endl;
    }
    else break;
  }
  QEDasym.close();

  TCanvas *cTheoAsym = new TCanvas("theoAsym","Theoretical asymmetry",10,20,400,400);
  cTheoAsym->cd();
  TGraph *grtheory = new TGraph(Form("%s/%s/QEDasym.txt",pPath,webDirectory), "%lg %lg");
  grtheory->GetXaxis()->SetTitle("dist from compton scattered electrons(m)");
  grtheory->GetYaxis()->SetTitle("#rho");
  grtheory->GetYaxis()->CenterTitle();
  grtheory->SetTitle("#rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(2);
  grtheory->SetMarkerColor(2);
  
  TF1 *fn0 = new TF1("fn0","pol3");
  grtheory->Fit("fn0","0");//,"0","goff");
  fn0->GetParameters(param);
  grtheory->Draw("AP");
  fn0->Draw("same");

  paramOutFile.open(Form("%s/%s/paramFile.txt",pPath,webDirectory));
  if(paramOutFile.is_open()) {
    paramOutFile<<param[0]<<"\t"<<param[1]<<"\t"<<param[2]<<"\t"<<param[3]<<endl;
  }
  paramOutFile.close();
  //  return *param;
}

#endif

/************* Comments *******
 *This code is entered in C++ counting, which starts from 0.
 *plane 4 is altogether ignored in this function for now
 *For strip masking function run # 24519 onwards, I need to add strip 23 to the plane-3 mask
 ******************************/

/*******************
 *the maskedStrips routine should use the 'true' information
 *from the subbank written at prestart, rather than this 
 *current method of masking strips based on eyeballing the hits-spectra
 *******************/
