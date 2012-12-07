#ifndef __RHOTOX_F
#define __RHOTOX_F

#include "comptonRunConstants.h"
///This function ought to be called after auto-determination of compton edge was successful
Double_t rhoToX(Int_t plane)
{
  Bool_t debug=0;
  Double_t xPrime[nPoints]={0.0},rho[nPoints]={0.0},dsdx[nPoints]={},asym[nPoints]={},dsdx_0[nPoints]={}; 
  ofstream QEDasym;
  Double_t re,R_bend,kprimemax,asymmax,rho0,k0prime,p_beam,r,h,kDummy;
  Double_t kk,x1,CedgeToDetBot,zdrift;
  Double_t thetabend = asin(0.3*B_dipole*lmag/E);// 10.131*pi/180 ! bend angle in Compton chicane (radians)
  Double_t det_angle = th_det*pi/180;//(radians)
 
  CedgeToDetBot = (Cedge[plane] + 4)*stripWidth + 0.5*stripWidth;///elog 399
  zdrift = ldet[plane] - CedgeToDetBot*sin(det_angle);
  if(debug) {
    cout<<red<<Form("CedgeToDetBot: %f = (%f + 4)*%f + 0.5*%f\n",  CedgeToDetBot,Cedge[plane],stripWidth,stripWidth);
    cout<<Form("zdrift:%f = %f - %f * %f",zdrift,ldet[plane],CedgeToDetBot,sin(det_angle))<<normal<<endl;
  }
  re = alpha*hbarc/me;
  gamma_my=E/me; //electron gamma, eqn.20
  R_bend = (gamma_my*hbarc)/(2.0*xmuB*B_dipole);
  k =2*pi*hbarc/(lambda); // incident photon energy (GeV)
  a_const =1/(1+4*k*gamma_my/me); // eqn.15 
  kprimemax=4*a_const*k*gamma_my*gamma_my; //eqn.16{max recoil photon energy} (GeV)
  asymmax=(1-a_const)*(1+a_const)/(1+a_const*a_const);
  rho0=1/(1+a_const);
  k0prime=rho0*kprimemax;
  Double_t th_prime = asin(0.3*B_dipole*lmag/(E+k-kprimemax));
  //  dx0prime=(k0prime/E)*zdrift*thetabend; //  displacement at asym 0 (m)

  p_beam =sqrt(E*E - me*me);///momentum of beam electrons
  r =(p_beam/me)*(hbarc/(2*xmuB*B_dipole));///radius of curvature of beam due to dipole-3.
  h = r*(1 - cos(thetabend));
  kk =zdrift*tan(thetabend);
  x1 =(kk + h);
  Double_t r_max = ((E+k-kprimemax)/me)*hbarc/(2*xmuB*B_dipole);///just for comparision,not needed really

  
  kDummy = kprimemax; ///initiating 
  Double_t r_dummy,th_dummy,x1_old,x2_old,x1_new,x2_new,dxPrime_old;
  x1_old= R_bend*(1-cos(thetabend)) + zdrift*tan(thetabend);
  x2_old= r_max* (1-cos(th_prime)) +  zdrift*tan(th_prime);
  //x1_new= R_bend*(1-cos(thetabend))+(ldet[plane] + CedgeToDetBot*tan(det_angle))*tan(thetabend);
  dxPrime_old = x2_old - x1_old;

  QEDasym.open(Form("%s/%s/QEDasymP%d.txt",pPath,webDirectory,plane+1));
  for (Int_t i = 1; i <=nPoints; i++) {
    rho[i] = (Double_t)i/nPoints;
    kDummy = rho[i]*kprimemax;
    r_dummy = ((E-kDummy+k)/me)*(hbarc/(2*xmuB*B_dipole)); //!still to change
    th_dummy = asin(0.3*B_dipole*lmag/(E+k-kDummy));
    
    x2_new = r_dummy*(1-cos(th_dummy))+
      (zdrift + dxPrime_old*(((Double_t)nPoints-i)/nPoints)*tan(det_angle))*tan(th_dummy); // max displacement (m)
      
    x1_new = R_bend*(1-cos(thetabend)) + (zdrift + dxPrime_old*tan(det_angle))*tan(thetabend); // max displacement (m)

    xPrime[i] = (x2_new - x1_new)/cos(det_angle);

    dsdx_0[i]=((1.0-rho[i]*(1.0+a_const))/(1.0-rho[i]*(1.0-a_const)));
    dsdx[i] = 2*pi*re*re/100.0*a_const*(rho[i]*rho[i]*(1-a_const)*(1-a_const)/(1-rho[i]*(1.0-a_const))+1.0+(dsdx_0[i] *dsdx_0[i]));
    asym[i] = 2*pi*re*re/100.0*a_const/dsdx[i]*(1-rho[i]*(1+a_const))*(1.0-1.0/(pow((1.0-rho[i]*(1.0-a_const)),2))) ;
    if(QEDasym.is_open()) {
      QEDasym<<xPrime[i]<<"\t"<<rho[i]<<"\t"<<asym[i]<<"\t"<<dsdx[i]<<endl;
    } else cout<<"**Alert: couldn't open file to write QEDasym**\n"<<endl;
  }

  QEDasym.close();
  xCedge = xPrime[nPoints-1]; ///'xCedge' is used in determining QED asym, hence this should be evaluated before calling the function to fit theoretical asym

  if(debug) {
    cout<<red<<"\nfor plane: "<<plane+1<<", CedgeToDetBot: "<<CedgeToDetBot<<", zdrift: "<<zdrift<<", xCedge:"<<xCedge<<"\n"<<endl;
    printf("\nR_bend:%f, thetabend:%f, r:%f\n",R_bend,thetabend*180/3.1415,r);
    cout<<"th_prime: "<<th_prime*180/3.1415<<endl;
    cout<<"r_max: "<<r_max<<"dx1: "<<x1_new<<endl;
    cout<<"dxprimemax: "<<dxPrime_old<<endl;
    cout<<"max. recoil ph.energy: "<<kprimemax<<normal<<endl;
    // TCanvas *cQED = new TCanvas("cQED","fit for QED parameter",10,10,300,300);
    // cQED->cd();
  }
  TGraph *grtheory = new TGraph(Form("%s/%s/QEDasymP%d.txt",pPath,webDirectory,plane+1), "%lg %lg");
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
  // if(debug) {
  //   grtheory->Draw("AP");
  //   fn0->Draw("same");
  // }

  ofstream checkfile;
  checkfile.open(Form("%s/%s/checkfileP%d.txt",pPath,webDirectory,plane+1));
   if(checkfile.is_open()) {
     cout<<"\nwriting into file the parameters for rho to X fitting for plane "<<plane+1<<endl;
     checkfile<<param[0]<<"\t"<<param[1]<<"\t"<<param[2]<<"\t"<<param[3]<<endl;
   } else cout<<"**Alert: couldn't open file to write QED fit parameters**\n"<<endl;
  checkfile.close();
  return xCedge;
}
/*Comment: Both plane and compton edge independently affect the rho-to-x conversion 
 *.. hence they both need to be passed on to this function
 */
#endif
