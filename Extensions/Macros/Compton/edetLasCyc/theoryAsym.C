/* Reference: Bardin et.al, Conceptual Design Report, hall A Compton
 * Thanks: V.Tvaskis, Dipangkar
 * Compton kinematics, cross section, and asymmetry calculator
 */
#include <rootClass.h>
#include "comptonRunConstants.h"
#include "maskedStrips.C"

Double_t polynomial(Double_t *x, Double_t *par) {
  return par[0] + par[1]*x[0] + par[2]*x[0]*x[0] + par[3]*x[0]*x[0]*x[0];
}

void theoryAsym(Int_t comptEdge)//,Double_t par[]) //Float_t *calcAsym) //
{
  const Int_t nPoints=1000;///for now arbitrarily chosen the number of points I want to generate the theoretical asymmetry curve
  gStyle->SetOptFit(1);
  Bool_t debug=1;
  //  Double_t par[4];
  Double_t xPrime[nPoints],rho[nPoints];
  Float_t rhoStrip,calcAsym1,xStrip,dsdrho1,dsdrho;  
  ofstream theoreticalAsym,QEDasym;
  Float_t k,re,gamma,R_bend,a,kprimemax,asymmax,rho0,k0prime,dx0prime,p_beam,r,h,kk,x1,kDummy;
  Float_t p_edge,r_edge,th_edge,hprime,kprime,x2;//,maxdist,rho;

  Float_t thetabend = chicaneBend*pi/180; //(radians)
  Float_t calcAsym[nStrips];

  re = alpha*hbarc/me;
  gamma=E/me; //electron gamma, eqn.20
  R_bend = (gamma*hbarc)/(2.0*xmuB*B_dipole);
  k =2*pi*hbarc/(lambda); // incident photon energy (GeV)
  a =1/(1+4*k*gamma/me); // eqn.15 
  kprimemax=4*a*k*gamma*gamma; //eqn.16{max recoil photon energy} (GeV)
  asymmax=(1-a)*(1+a)/(1+a*a);

  rho0=1/(1+a);
  k0prime=rho0*kprimemax;
  dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)

  p_beam =sqrt(E*E - me*me);
  r =p_beam/me*(hbarc/(2*xmuB*B_dipole));
  h = r - lmag/tan(thetabend);
  kk =ldet*tan(thetabend);
  x1 =kk+h;
  kDummy = kprimemax; ///initiating 
 
  QEDasym.open("QEDasym.txt"); //!I don't really need to write this to a file
  for (Int_t i = 0; i <nPoints; i++) {//xPrime[nPoints],rho[nPoints];
    xPrime[i]=0.0; ///initialize
    rho[i]=0.0;
    p_edge=p_beam-kDummy;
    r_edge=p_edge/me*(hbarc/(2*xmuB*B_dipole));
    th_edge=asin((p_beam/p_edge)*sin(thetabend));
    hprime = r_edge - lmag/tan(th_edge);
    //hprime = r_edge*(1-cos(th_edge));
    kprime=ldet*tan(th_edge);
    x2= kprime + hprime;
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

  //TCanvas *cTheoAsym = new TCanvas("theoAsym","Theoretical asymmetry",10,20,400,400);
  TGraph *grtheory = new TGraph(Form("QEDasym.txt"), "%lg %lg");
  grtheory->GetXaxis()->SetTitle("dist from compton scattered electrons(m)");
  grtheory->GetYaxis()->SetTitle("#rho");
  grtheory->GetYaxis()->CenterTitle();
  grtheory->SetTitle("#rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(2);
  grtheory->SetMarkerColor(2);
  grtheory->Fit("pol3");
  
//   TF1 *fitFcn= new TF1("fitFcn",polynomial,dx0prime,xPrime[0],4); ///limiting my fit to be upto Cedge
//   fitFcn->SetParameters(1,1,1,1);
//   grtheory->Fit("fitFcn","RV+","ep",dx0prime,xPrime[0]); ///the fit would happen only between zero-crossing and Cedge
//   grtheory->Draw("AP");
//   fitFcn->GetParameters(par);
//   printf("param[0]:%f,param[1]:%f,param[2]:%f\n",par[0],par[1],par[2]);

  theoreticalAsym.open(Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,comptEdge));
  if (theoreticalAsym.is_open()) {
    cout<<"theoretical asymmetry file "<<Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,comptEdge)<<" opened"<<endl;
    if(debug) cout<<"rhoStrip\txStrip\t\ts\tcalcAsym"<<endl;
    for(Int_t s =comptEdge; s >=startStrip; s--) { //this loop would simply stop at strip-1;notice that s:0::strip:1
      //if (maskedStrips(0,s)) continue;///I don't want to loop on plane here but I do need to skip the irrelevant strips
      xStrip = xPrime[0] - (comptEdge - s)*2E-4; ///xPrime[0] corresponds to Cedge distance
      rhoStrip = grtheory->Eval(xStrip);
      dsdrho1 = (1-rhoStrip*(1+a))/(1-rhoStrip*(1-a)); // 2nd term of eqn 22
      dsdrho =((rhoStrip*rhoStrip*(1-a)*(1-a)/(1-rhoStrip*(1-a)))+1+dsdrho1*dsdrho1);//eqn. 22 (cross-section without 2*pi*re*re/100*a* factor)
      calcAsym1 = 1-rhoStrip*(1-a);//just a term in eqn 24
      calcAsym[s]=(-1*IHWP)*((1-rhoStrip*(1+a))*(1-1/(calcAsym1*calcAsym1)))/dsdrho;//eqn. 24
      theoreticalAsym<<Form("%2.0f\t%f\n",(Float_t)s,calcAsym[s]);//!notice the reverse order
      //if(debug) cout<<gamma<<"\t"<<re<<"\t"<<k<<"\t"<<a<<"\t"<<rhoStrip<<"\t"<<dsdrho<<"\t"<<calcAsym[s]<<endl;
      if(debug) cout<<rhoStrip<<"\t"<<xStrip<<"\t"<<s<<"\t"<<calcAsym[s]<<endl;
    }
    theoreticalAsym.close();
  }
  else cout<<"\ncouldn't open the theoretical asymmetry file "<<Form("%s/%s/theoryAsymForCedge_%d.txt",pPath,webDirectory,comptEdge)<<endl;
  
  //cTheoAsym->Update();

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
    cout<<"\tcompton edge is on strip "<<comptEdge<<" at x="<<xPrime[0]<<endl;
    cout<<"\tx1= "<<x1<<" and x2="<<x2<<endl;
  }
}

/****************************
!Comments
 * notice that I am printing the strip numbers in theoryAsym file in human counts
 * ?should there be a s+1 in xStrip evaluation?, 
 *..this was added because otherwise the Cedge would be literally be taken as the true Cedge 
 *..even though it is so only in the C++ counting
 * Make sure to delete the "old" file if you want this to be reevaluated
 * In doing it this way, are we effectively projecting
 *..the strip to the vertical plane, hence decresing the
 *..strip-width from 180 um to 180*cos(thetaDetector)?;
 * Revisit with Bardin's equations
 *
 *I don't need to worry about masked strips here, the downstream usage should ensure 
 *..that the theoretical asymmetry has the asymmetry corresponding to each strip and is plane independent
 *********************/
