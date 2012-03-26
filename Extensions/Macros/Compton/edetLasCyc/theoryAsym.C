/* Reference: Bardin et.al, Conceptual Design Report, hall A Compton
 * Thanks: V.Tvaskis, Dipangkar
 * Compton kinematics, cross section, and asymmetry calculator
 */
#include <rootClass.h>
#include "comptonRunConstants.h"

Double_t polynomial(Double_t *x, Double_t *par) {
  return par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0];
}

void theoryAsym(Int_t comptEdge,Double_t par[]) //Float_t *calcAsym) //
{
  const Int_t nPoints=800;///for now arbitrarily chosen the number of points I want to generate the theoretical asymmetry curve
  gStyle->SetOptFit(1);
  Bool_t debug=0;
  //Double_t par[4];
  Double_t xPrime[nPoints],rho[nPoints];
  Float_t rhoStrip; 
  Float_t calcAsym1;
  Float_t xStrip,dsdrho1,dsdrho;  
  Float_t thetabend = chicaneBend*pi/180; //(radians)
  //  Float_t thetaprime = 10.39*pi/180; //bend angle for the max deflected electrons
  ofstream theoreticalAsym,QEDasym;
  Float_t calcAsym[nStrips];

  Float_t re = alpha*hbarc/me;
  Float_t gamma=E/me; //electron gamma, eqn.20
  Float_t R_bend = (gamma*hbarc)/(2.0*xmuB*B_dipole);
  Float_t k =2*pi*hbarc/(lambda); // incident photon energy (GeV)
  Float_t a =1/(1+4*k*gamma/me); // eqn.15 
  Float_t kprimemax=4*a*k*gamma*gamma; //eqn.16{max recoil photon energy} (GeV)
  Float_t asymmax=(1-a)*(1+a)/(1+a*a);

  Float_t rho0=1/(1+a);
  Float_t k0prime=rho0*kprimemax;
  Float_t dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)

  Float_t p_beam =sqrt(E*E - me*me);
  Float_t r =p_beam/me*(hbarc/(2*xmuB*B_dipole));
  Float_t h =(r*tan(thetabend)-lmag)/tan(thetabend);
  Float_t kk =ldet*tan(thetabend);
  Float_t x1 =kk+h;
  Float_t kDummy = kprimemax; ///initiating 
  Float_t p_edge,r_edge,th_edge,hprime,kprime,x2;//,maxdist,rho;
  if(debug) {
    printf("Qweak Compton Calculator:\n");
    printf("\tBeam Energy: %f GeV\n",E);
    cout<<"\tIncident Photon Wavelength: "<<lambda<<endl;
    cout<<"\tR_bend: "<<R_bend<<endl;
    cout<<"\tClassical electron radius: "<<re<<endl;
    cout<<"\tincident photon energy: "<<k<<" GeV"<<endl;
    cout<<"\ta: "<<a<<endl;
    cout<<"\tMaximum Scatt. Photon Energy: "<<kprimemax<<" GeV"<<endl;
    cout<<"\tMaximum (theoretical) asymmetry: "<<asymmax<<endl;
    cout<<"\n\tAsym Zero rho: "<<rho0<<endl;
    cout<<"\tAsym Zero Photon Energy: "<<k0prime<<"GeV"<<endl;
    cout<<"\tAsym Zero e-displacement: "<<dx0prime*1000<<" mm"<<endl;
  }

  QEDasym.open("QEDasym.txt"); //!I don't really need to write this to a file
  for (Int_t i = 0; i <nPoints; i++) {//xPrime[nPoints],rho[nPoints];
    xPrime[i]=0.0; ///initialize
    rho[i]=0.0;
    p_edge=p_beam-kDummy;
    r_edge=p_edge/me*(hbarc/(2*xmuB*B_dipole));
    th_edge=asin((p_beam/p_edge)*sin(thetabend));
    hprime=(r_edge*tan(th_edge)-lmag)/tan(th_edge);
    kprime=ldet*tan(th_edge);
    x2= kprime + hprime;
    xPrime[i]=(x2-x1);
    rho[i]=kDummy/kprimemax;
    kDummy=kDummy-0.00005;
    if(QEDasym.is_open())
      QEDasym<<xPrime[i]<<"\t"<<rho[i]<<endl;
  }
  QEDasym.close();

  TCanvas *cTheoAsym = new TCanvas("theoAsym","Theoretical asymmetry",10,20,400,400);
  TGraph *grtheory = new TGraph(Form("QEDasym.txt"), "%lg %lg");
  grtheory->GetXaxis()->SetTitle("dist compton scattered electrons(m)");
  grtheory->GetYaxis()->SetTitle("#rho");
  grtheory->GetYaxis()->CenterTitle();
  grtheory->SetTitle("rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(2);
  grtheory->SetMarkerColor(2);
  //grtheory->Fit("pol3");
  
  TF1 *fitFcn= new TF1("fitFcn",polynomial,dx0prime,xPrime[0],4); ///limiting my fit to be upto Cedge
  fitFcn->SetParameters(1,1,1,1);
  grtheory->Fit("fitFcn","RV+","ep",dx0prime,xPrime[0]); ///the fit would happen only between zero-crossing and Cedge
  grtheory->Draw("AP");
  fitFcn->GetParameters(par);
  printf("param[0]:%f,param[1]:%f,param[2]:%f\n",par[0],par[1],par[2]);
  theoreticalAsym.open(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,comptEdge));
  if (theoreticalAsym.is_open()) 
    cout<<"theoretical asymmetry file "<<Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,comptEdge)<<" opened"<<endl;
  else 
    cout<<"\ncouldn't open the theoretical asymmetry file "<<Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,comptEdge)<<endl;
  ///on purpose this is dropped to the webDirectory
  for(Int_t s =startStrip; s <=comptEdge; s++) { //!sure?
    xStrip = xPrime[0] - (comptEdge - s)*2E-4; ///xPrime[0] corresponds to Cedge distance
    rhoStrip = grtheory->Eval(xStrip);
    if(rhoStrip>1.0) cout<<"**Alert: rho for strip "<<s<<" is greater than 1 !"<<endl;
    dsdrho1 = (1-rhoStrip*(1+a))/(1-rhoStrip*(1-a)); // 2nd term of eqn 22
    dsdrho = 2*pi*re*re/100*a*((rhoStrip*rhoStrip*(1-a)*(1-a)/(1-rhoStrip*(1-a)))+1+dsdrho1*dsdrho1);//eqn. 22
    calcAsym1 = 1-rhoStrip*(1-a);
    calcAsym[s]=(-1*(Int_t)IHWP_in)*((2*pi*re*re/100*a/dsdrho)*(1-rhoStrip*(1+a))*(1-1/(calcAsym1*calcAsym1)));
    theoreticalAsym<<Form("%2.0f\t%f\n",(Float_t)s+1,calcAsym[s]);
  }
  theoreticalAsym.close();
  cTheoAsym->Update();
}

/****************************
!Comments
 * ?should there be a s+1 in xStrip evaluation?, 
 *..this was added because otherwise the Cedge would be literally be taken as the true Cedge 
 *..even though it is so only in the C++ counting
 * Make sure to delete the "old" file if you want this to be reevaluated
 * In doing it this way, are we effectively projecting
 *..the strip to the vertical plane, hence decresing the
 *..strip-width from 180 um to 180*cos(thetaDetector)?;
 * Revisit with Bardin's equations
 *********************/
