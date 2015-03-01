#ifndef __RHOTOX_F
#define __RHOTOX_F
#include <TString.h>
#include "comptonRunConstants.h"
#include "infoDAQ.C"
///This function ought to be called after auto-determination of compton edge was successful
Double_t rhoToX(Int_t runnum, Double_t Cedge)//now plane is a variable set in the constants file
{
  cout<<"\nStarting into rhoToX.C with Cedge "<<Cedge<<"\n"<<endl;
  filePre = Form(filePrefix,runnum,runnum);
  Bool_t debug =0;
  Double_t xPrime[nPoints]={0.0},rho[nPoints]={0.0},dsdx[nPoints]={},asym[nPoints]={},dsdx_0[nPoints]={}; 
  ofstream QEDasym;
  if(retInfoDAQ ==0) {
    retInfoDAQ = infoDAQ(runnum); ///needed eEnergy for rhoToX.C
    cout<<"calling infoDAQ from asymFit.C to get energy value"<<endl;
  } else cout<<"parameters from infoDAQ already available hence not invoking that macro (daqcheck="<<retInfoDAQ<<")"<<endl;
  if(retInfoDAQ<0) {
    cout<<red<<"\nreturned error from infoDAQ.C hence exiting\n"<<normal<<endl;
    return -2;
  }
  Double_t kk,x1,CedgeToDetBot,zdrift;
  Double_t thetabend = asin(light*B_dipole*lmag/eEnergy);// 10.131*pi/180 ! bend angle in Compton chicane (radians)
  Double_t det_angle = th_det*pi/180;//(radians)
 
  CedgeToDetBot = (Cedge + 4)*stripWidth + 0.5*stripWidth;///elog 399
  zdrift = ldet[plane-1] - CedgeToDetBot*sin(det_angle);
  if(debug) {
    cout<<blue<<"using the following parameters for rhoToX conversion"<<endl;
    cout<<Form("CedgeToDetBot: %f = (%f + 4)*%f + 0.5*%f\n",  CedgeToDetBot,Cedge,stripWidth,stripWidth);
    cout<<"beam energy: "<<eEnergy<<" +/- "<<eEnergyEr<<endl;
    cout<<"field:    "<<B_dipole<<endl;
    cout<<Form("zdrift:%f = %f - %f * %f",zdrift,ldet[plane-1],CedgeToDetBot,sin(det_angle))<<normal<<endl;
  }
  re = alpha*hbarc/me;
  gamma_my=eEnergy/me; //electron gamma, eqn.20
  R_bend = eEnergy/(light*B_dipole);
  eLaser =2*pi*hbarc/(lambda); // incident photon energy (GeV)
  a_const =1/(1+4*eLaser*gamma_my/me); // eqn.15 
  kprimemax=4*a_const*eLaser*gamma_my*gamma_my; //eqn.16{max recoil photon energy} (GeV)
  asymmax=(1-a_const)*(1+a_const)/(1+a_const*a_const);
  rho0=1/(1+a_const);
  k0prime=rho0*kprimemax;
  Double_t th_prime = asin(light*B_dipole*lmag/(eEnergy+eLaser-kprimemax));
  //  dx0prime=(k0prime/eEnergy)*zdrift*thetabend; //  displacement at asym 0 (m)

  p_beam =sqrt(eEnergy*eEnergy - me*me);///momentum of beam electrons
  //r =(p_beam/me)*(hbarc/(2*xmuB*B_dipole));///radius of curvature of beam due to dipole-3.
  h = R_bend*(1 - cos(thetabend));
  kk =zdrift*tan(thetabend);
  x1 =(kk + h);
  Double_t r_max = ((eEnergy+eLaser-kprimemax))/(light*B_dipole);///just for comparision,not needed really
  
  kDummy = kprimemax; ///initiating 
  Double_t r_dummy,th_dummy,x1_old,x2_old,x1_new,x2_new,dxPrime_old;
  x1_old= R_bend*(1-cos(thetabend)) + zdrift*tan(thetabend);
  x2_old= r_max* (1-cos(th_prime)) +  zdrift*tan(th_prime);
  dxPrime_old = x2_old - x1_old;
///max vertical displacement of unscattered beam (m)
// x1_new = R_bend*(1-cos(thetabend)) + (zdrift + dxPrime_old*tan(det_angle))*tan(thetabend); 
  x1_new = R_bend*(1-cos(thetabend)) + (zdrift)*tan(thetabend);

  QEDasym.open(Form("%s/%s/%sQEDasymP%d.txt",pPath, txt,filePre.Data(),plane));
  for (Int_t i = 0; i < nPoints; i++) {
    rho[i] = (Double_t)i/(nPoints-1);
    kDummy = rho[i]*kprimemax;
    r_dummy = (eEnergy-kDummy+eLaser)/(light*B_dipole); 
    th_dummy = asin(light*B_dipole*lmag/(eEnergy+eLaser-kDummy));
//  max displacement of the beam with ith (m) 
//  x2_new = r_dummy*(1-cos(th_dummy)) + (zdrift + dxPrime_old*(((Double_t)nPoints-i)/nPoints)*tan(det_angle))*tan(th_dummy); // max displacement (m)
    x2_new = r_dummy*(1-cos(th_dummy)) + (zdrift)*tan(th_dummy); 

    xPrime[i] = (x2_new - x1_new)*cos(thetabend)/cos(det_angle-thetabend);

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
    cout<<"xCedge:"<<xCedge<<"\n"<<endl;
    printf("\nR_bend:%f, thetabend:%f degree\n",R_bend,thetabend*180/pi);
    cout<<"th_prime: "<<th_prime*180/pi<<endl;
    cout<<"r_max: "<<r_max<<"dx1: "<<x1_new<<endl;
    cout<<"dxprimemax: "<<dxPrime_old<<endl;
    cout<<"max. recoil ph.energy: "<<kprimemax<<normal<<endl;
    // TCanvas *cQED = new TCanvas("cQED","fit for QED parameter",10,10,300,300);
    // cQED->cd();
  }
  TGraph *grtheory = new TGraph(Form("%s/%s/%sQEDasymP%d.txt",pPath, txt,filePre.Data(),plane), "%lg %lg");
  grtheory->GetXaxis()->SetTitle("dist from compton scattered electrons(m)");
  grtheory->GetYaxis()->SetTitle("#rho");
  grtheory->GetYaxis()->CenterTitle();
  grtheory->SetTitle("#rho to x");
  grtheory->SetMarkerStyle(20);
  grtheory->SetLineColor(2);
  grtheory->SetMarkerColor(2);
  
  TF1 *fn0 = new TF1("fn0","pol3");
  grtheory->Fit("fn0","0 Q");//,"0","goff");
  fn0->GetParameters(param);
  // if(debug) {
  //   grtheory->Draw("AP");
  //   fn0->Draw("same");
  // }

  ofstream checkfile;
  checkfile.open(Form("%s/%s/%scheckfileP%d.txt",pPath, txt,filePre.Data(),plane));
   if(checkfile.is_open()) {
     if(debug) cout<<"\nwriting into file the parameters for rho to X fitting for plane "<<plane<<endl;
     checkfile<<param[0]<<"\t"<<param[1]<<"\t"<<param[2]<<"\t"<<param[3]<<endl;
   } else cout<<"**Alert: couldn't open file to write QED fit parameters**\n"<<endl;
  checkfile.close();
  return xCedge;
}
/*Comment: Both plane and compton edge independently affect the rho-to-x conversion 
 *.. hence they both need to be passed on to this function
 */
#endif
