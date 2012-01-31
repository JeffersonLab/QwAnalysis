/* Bardin et.al, Conceptual Design Report, hall A Compton
 * Thanks: Dipangkar
 * Compton kinematics, cross section, and asymmetry calculator
 */
#include <rootClass.h>
#include "comptonRunConstants.h"
Float_t theoryAsym(std::vector<Float_t> &asymStrip) 
{
  Bool_t debug=0,debug1=0,debug2=1;
  //  Int_t stripNum=50;
  Float_t gamma,k,a,kprimemax;
  Float_t dxprimemax,asymmax,rho0,k0prime,dx0prime;
  Float_t kStripprime,rhoStrip;
  Float_t asymStrip1;
  Float_t re,R_bend;
  Float_t dsdrho1,dsdrho;
  
  Float_t thetabend = 10*pi/180; //bend angle in Compton chicane (radians)
  //  Float_t thetaprime = 10.39*pi/180; //bend angle for the max deflected electrons

  if(debug) {
    printf("Qweak Compton Calculator:\n");
    printf("\tBeam Energy: %f GeV\n",E);
    cout<<"\tIncident Photon Wavelength: "<<lambda<<endl;
  }
  re = alpha*hbarc/me;
  if(debug) cout<<"\tClassical electron radius: "<<re<<endl;

  gamma=E/me; //electron gamma, eqn.20
  R_bend = (gamma*hbarc)/(2.0*xmuB*B_dipole);
  if(debug) cout<<"\tR_bend: "<<R_bend<<endl;

  k=2*pi*hbarc/(lambda); // incident photon energy (GeV)
  if(debug1) cout<<"\tincident photon energy: "<<k<<" GeV"<<endl;

  a=1/(1+4*k*gamma/me); // eqn.15 
  if(debug1) cout<<"\ta: "<<a<<endl;

  kprimemax=4*a*k*gamma*gamma; //eqn.16{max recoil photon energy} (GeV)
  if(debug1) cout<<"\tMaximum Scatt. Photon Energy: "<<kprimemax<<" GeV"<<endl;

  dxprimemax=(kprimemax/E)*zdrift*thetabend; // max displacement (m)
  if(debug1) cout<<"\tCompton edge @: "<<dxprimemax*1000<<" mm"<<" from beam"<<endl;

  asymmax=(1-a)*(1+a)/(1+a*a);
  if(debug1) cout<<"\tMaximum (theoretical) asymmetry: "<<asymmax<<endl;

  rho0=1/(1+a);
  if(debug1) cout<<"\n\tAsym Zero rho:  "<<rho0<<endl;

  k0prime=rho0*kprimemax;
  if(debug1) cout<<"\tAsym Zero Photon Energy: "<<k0prime<<"GeV"<<endl;

  dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)
  if(debug1) cout<<"\tAsym Zero e-displacement: "<<dx0prime*1000<<" mm"<<endl;

  if(debug2) cout<<"\nstrip#\tdis(mm)\trho\tk-photon\tasymStrip"<<endl;
  
  //for(Int_t p =0; p <=nPlanes; p++) {
  for(Int_t s =startStrip; s <=endStrip; s++) {
    kStripprime=((xgap+s*2E-4)/zdrift)*E/thetabend; //   scattered photon energy (GeV)
    rhoStrip=kStripprime/kprimemax; //  rho (dimensionless)
    dsdrho1 = (1-rhoStrip*(1+a))/(1-rhoStrip*(1-a)); // eqn 22
    dsdrho = 2*pi*re*re/100*a*((rhoStrip*rhoStrip*(1-a)*(1-a)/(1-rhoStrip*(1-a)))+1+dsdrho1*dsdrho1);//eqn. 22
    asymStrip1 = 1-rhoStrip*(1-a);
    asymStrip.push_back((2*pi*re*re/100*a/dsdrho)*(1-rhoStrip*(1+a))*(1-1/(asymStrip1*asymStrip1)));
    
    //    cout<<s<<"\t"<<(xgap+s*2E-4)*1E3<<"\t"<<rhoStrip<<"\t"<<kStripprime<<"\t"<<asymStrip.at(s)<<endl;
  }
  return asymStrip1;
}
/****** Comments:
 *Currently returned value is just for the sake of it 
 *I should return the array of theoretical asymStrip so that the main program can use it
 ******/
