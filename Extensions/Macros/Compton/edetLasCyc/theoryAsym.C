/* Bardin et.al, Conceptual Design Report, hall A Compton
 * Thanks: Dipangkar
 * Compton kinematics, cross section, and asymmetry calculator.   
 */
#include <rootClass.h>
#include "comptonRunConstants.h"
Float_t theoryAsym() 
{
  Bool_t debug=1;
  Float_t gamma,k,a,kprimemax;
  Float_t dxprimemax,asymmax,rho0,k0prime,dx0prime;
  Float_t kgapprime,rhogap;
  Float_t asymgap1,asymgap;
  Float_t re,R_bend;
  Float_t dsdrho1,dsdrho;
  
  Float_t thetabend = 10*pi/180; //bend angle in Compton chicane (radians)
  //  Float_t thetaprime = 10.39*pi/180; //bend angle for the max deflected electrons

  re = alpha*hbarc/me;
//   re = alpha*hbarc*1E-15/me;//old
  if(debug) cout<<"\tClassical electron radius: "<<re<<endl;

  gamma=E/me; //electron gamma, eqn.20
  R_bend = (gamma*hbarc)/(2.0*xmuB*B_dipole);
  if(debug) cout<<"\tR_bend: "<<R_bend<<endl;

//   k=2*pi*hbarc/(lambda*(1E6)); // old photon energy (GeV)
  k=2*pi*hbarc/(lambda); // photon energy (GeV)
  if(debug) cout<<"\tk: "<<k<<endl;

  a=1/(1+4*k*gamma/me); // eqn.15 
  if(debug) cout<<"\ta: "<<a<<endl;

  kprimemax=4*a*k*gamma*gamma; //eqn.16{max recoil photon energy} (GeV)
  if(debug) cout<<"\tMaximum Scatt. Photon Energy: "<<kprimemax<<"GeV"<<endl;

  dxprimemax=(kprimemax/E)*zdrift*thetabend; // !max displacement (m)
  if(debug) cout<<"\tdxprimemax: "<<dxprimemax<<" m"<<endl;

  asymmax=(1-a)*(1+a)/(1+a*a);
  if(debug) cout<<"\tasymmax: "<<asymmax<<endl;

  rho0=1/(1+a);
  k0prime=rho0*kprimemax;
  dx0prime=(k0prime/E)*zdrift*thetabend; //  !displacement at asym 0 (m)
  if(debug) cout<<"\tdisplacement at asym 0: "<<dx0prime<<" m"<<endl;

  kgapprime=(xgap/zdrift)*E/thetabend; //  ! scattered photon energy (GeV)
  if(debug) cout<<"scattered photon energy: "<<kgapprime<<" GeV"<<endl;

  rhogap=kgapprime/kprimemax; //  ! rho (dimensionless)
  if(debug) cout<<"rhogap: "<<rhogap<<endl;

  dsdrho1 = (1-rhogap*(1+a))/(1-rhogap*(1-a)); // eqn 22
  dsdrho = 2*pi*re*re/100*a*((rhogap*rhogap*(1-a)*(1-a)/(1-rhogap*(1-a)))+1+dsdrho1*dsdrho1);//eqn. 22

  asymgap1 = 1-rhogap*(1-a);
  asymgap = (2*pi*re*re/100*a/dsdrho)*(1-rhogap*(1+a))*(1-1/(asymgap1*asymgap1)); //
  printf("Qweak Compton Calculator:\n");
  printf("\tBeam Energy: %f GeV\n",E);
  cout<<"\tPhoton Wavelength: "<<lambda<<endl;
  printf("\tBend angle: %f degree\n",thetabend*180/pi);
  printf("\tMaximum Electron Disp. %f m\n",dxprimemax);
  cout<<"\tMaximum Asymmetry:" << asymmax<<endl;
  cout<<"\trho at Asymmetry Zero:  "<<rho0<<endl;
  cout<<"\tPhoton Energy at Asym Zero: "<<k0prime<<"GeV"<<endl;
  cout<<"\tElectron Disp at Asym Zero: "<<dx0prime<<" m"<<endl;
  cout<<"\tAt the min detector distance of "<<xgap<<" m"<<endl; 
  cout<<"\tthe photon energy is "<<kgapprime<<" GeV"<<endl;
  cout<<"\tthe rho is "<<rhogap<<endl;
  cout<<"\tand the asymmetry is "<<asymgap<<endl;

  return asymgap;
}
