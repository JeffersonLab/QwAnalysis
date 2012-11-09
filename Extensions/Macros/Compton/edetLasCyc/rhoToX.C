#ifndef __RHOTOX_F
#define __RHOTOX_F

#include "comptonRunConstants.h"

void rhoToX()//Double_t param[4]) //!this function as of now, may not work,some lines were commented for some quick work
{
  //Double_t param[4];
  Double_t xPrime[nPoints],rho[nPoints];
  ofstream QEDasym,paramOutFile;//,energyDisp;
  Float_t re,R_bend,kprimemax,asymmax,rho0,k0prime,p_beam,r,h,kk,x1,kDummy;//k,gamma,a
  Float_t p_edge,r_edge,th_edge,hprime,kprime,x2;//,maxdist,rho;
  Float_t thetabend = chicaneBend*pi/180; //(radians)
  Float_t det_angle = th_det*3.1416/180;//(radians)

  re = alpha*hbarc/me;
  gamma_my=E/me; //electron gamma, eqn.20
  R_bend = (gamma_my*hbarc)/(2.0*xmuB*B_dipole);
  k =2*pi*hbarc/(lambda); // incident photon energy (GeV)
//   a =1/(1+4*k*gamma_my/me); // eqn.15 
  kprimemax=4*a_const*k*gamma_my*gamma_my; //eqn.16{max recoil photon energy} (GeV)
  asymmax=(1-a_const)*(1+a_const)/(1+a_const*a_const);

  rho0=1/(1+a_const);
  k0prime=rho0*kprimemax;
  //  dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)

  p_beam =sqrt(E*E - me*me);
  r =p_beam/me*(hbarc/(2*xmuB*B_dipole));///radius of curvature of electrons due to dipole-3.
  h = r - lmag/tan(thetabend);
  kk =ldet*tan(thetabend);
  x1 =(kk+h);
  kDummy = kprimemax; ///initiating 

  //energyDisp.open(Form("%s/%s/energyDisplacement.txt",pPath,webDirectory));
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
    x2= (kprime + hprime);
    if(x2>x1) {
      xPrime[i]=(x2-x1)*cos(det_angle);
      rho[i]=kDummy/kprimemax;
      kDummy=kDummy-0.00005;
      if(QEDasym.is_open()) {
	QEDasym<<xPrime[i]<<"\t"<<rho[i]<<endl;
	//energyDisp<<xPrime[i]<<"\t"<<p_edge<<endl;
      }
    }    
    else break;
  }
  QEDasym.close();
  //energyDisp.close();
  //TCanvas *cTheoAsym = new TCanvas("theoAsym","Theoretical asymmetry",10,20,400,400);
  //cTheoAsym->cd();
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
  //grtheory->Draw("AP");
  //fn0->Draw("same");

  paramOutFile.open(Form("%s/%s/paramFile.txt",pPath,webDirectory));
  if(paramOutFile.is_open()) {
    cout<<"\nthe parameters for rho to X fitting are: "<<endl;
    paramOutFile<<param[0]<<"\t"<<param[1]<<"\t"<<param[2]<<"\t"<<param[3]<<endl;
  }
  paramOutFile.close();
  //  return *param;
}

#endif
