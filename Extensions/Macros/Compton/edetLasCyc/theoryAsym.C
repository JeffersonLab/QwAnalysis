/* Bardin et.al, Conceptual Design Report, hall A Compton
 * Thanks: V.Tvaskis, Dipangkar
 * Compton kinematics, cross section, and asymmetry calculator
 */
#include <rootClass.h>
#include "comptonRunConstants.h"

void theoryAsym(Float_t *calcAsym) 
{
  Int_t nPoints=800;
  gStyle->SetOptFit(1);
  Bool_t debug=0,debug1=0;
  Double_t xPrime[nPoints],rho[nPoints];
  Float_t kStripprime,rhoStrip;
  Float_t calcAsym1;
  Float_t xStrip,dsdrho1,dsdrho;  
  Float_t thetabend = chicaneBend*pi/180; //(radians)
  //  Float_t thetaprime = 10.39*pi/180; //bend angle for the max deflected electrons
  ofstream QEDasym;
  //Float_t *calcAsym;

  if(debug) {
    printf("Qweak Compton Calculator:\n");
    printf("\tBeam Energy: %f GeV\n",E);
    cout<<"\tIncident Photon Wavelength: "<<lambda<<endl;
  }
  Float_t re = alpha*hbarc/me;
  if(debug) cout<<"\tClassical electron radius: "<<re<<endl;

  Float_t gamma=E/me; //electron gamma, eqn.20
  Float_t R_bend = (gamma*hbarc)/(2.0*xmuB*B_dipole);
  if(debug) cout<<"\tR_bend: "<<R_bend<<endl;

  Float_t k=2*pi*hbarc/(lambda); // incident photon energy (GeV)
  if(debug1) cout<<"\tincident photon energy: "<<k<<" GeV"<<endl;

  Float_t a =1/(1+4*k*gamma/me); // eqn.15 
  if(debug1) cout<<"\ta: "<<a<<endl;

  Float_t kprimemax=4*a*k*gamma*gamma; //eqn.16{max recoil photon energy} (GeV)
  if(debug1) cout<<"\tMaximum Scatt. Photon Energy: "<<kprimemax<<" GeV"<<endl;

  Float_t asymmax=(1-a)*(1+a)/(1+a*a);
  if(debug1) cout<<"\tMaximum (theoretical) asymmetry: "<<asymmax<<endl;

  Float_t rho0=1/(1+a);
  if(debug1) cout<<"\n\tAsym Zero rho: "<<rho0<<endl;

  Float_t k0prime=rho0*kprimemax;
  if(debug1) cout<<"\tAsym Zero Photon Energy: "<<k0prime<<"GeV"<<endl;

  Float_t dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)
  if(debug1) cout<<"\tAsym Zero e-displacement: "<<dx0prime*1000<<" mm"<<endl;

  Float_t p_beam=sqrt(E*E - me*me);
  Float_t r=p_beam/me*(hbarc/(2*xmuB*B_dipole));
  Float_t h=(r*tan(thetabend)-lmag)/tan(thetabend);
  Float_t kk=ldet*tan(thetabend);
  Float_t x1=kk+h;
 	  
  Float_t kkk = kprimemax;
  Float_t p_edge,r_edge,th_edge,hprime,kprime,x2;//,maxdist,rho;

  QEDasym.open("QEDasym.txt");
  for (Int_t i = 0; i <nPoints; i++) {//xPrime[nPoints],rhoPrime[nPoints];
    xPrime[i]=0.0; ///initialize
    rho[i]=0.0;
    p_edge=p_beam-kkk;
    r_edge=p_edge/me*(hbarc/(2*xmuB*B_dipole));
    th_edge=asin((p_beam/p_edge)*sin(thetabend));
    hprime=(r_edge*tan(th_edge)-lmag)/tan(th_edge);
    kprime=ldet*tan(th_edge);
    x2= kprime + hprime;
    xPrime[i]=(x2-x1);
    rho[i]=kkk/kprimemax;
    kkk=kkk-0.00005;
    if(QEDasym.is_open())
      QEDasym<<xPrime[i]<<"\t"<<rho[i]<<endl;
  }
  QEDasym.close();

  TGraph *grtheory = new TGraph(Form("QEDasym.txt"), "%lg %lg");
  grtheory->GetXaxis()->SetTitle("dist compton scattered electrons(m)");
  grtheory->GetYaxis()->SetTitle("rho");
  grtheory->SetTitle("rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(2);
  grtheory->SetMarkerColor(2);
  grtheory->Fit("pol3");
  //grtheory->Draw("AP");

  for(Int_t s =startStrip; s <=endStrip; s++) {
    xStrip = xPrime[0] - (Cedge - s)*2E-4; ///xPrime[0] corresponds to Cedge distance
    rhoStrip = grtheory->Eval(xStrip);
    dsdrho1 = (1-rhoStrip*(1+a))/(1-rhoStrip*(1-a)); // eqn 22
    dsdrho = 2*pi*re*re/100*a*((rhoStrip*rhoStrip*(1-a)*(1-a)/(1-rhoStrip*(1-a)))+1+dsdrho1*dsdrho1);//eqn. 22
    calcAsym1 = 1-rhoStrip*(1-a);
    calcAsym[s]=((2*pi*re*re/100*a/dsdrho)*(1-rhoStrip*(1+a))*(1-1/(calcAsym1*calcAsym1)));
    if(debug1) cout<<s<<"\t"<<(xgap+s*2E-4)*1E3<<"\t"<<rhoStrip<<"\t"<<kStripprime<<"\t"<<calcAsym[s]<<endl;
  }  
}

/****Comments*****************
 * In doing it this way, are we effectively projecting
 *..the strip to the vertical plane, hence decresing the
 *..strip-width from 180 um to 180*cos(thetaDetector)?;
 * Revisit with Bardin's equations
 *********************/
