/*
Tue Jan 25 13:50:05 EDT 2013 
Author Rakitha Beminiwattha
contact rakithab@jlab.org

Compute the SM prediction of the PV asymmetry and hadronic contribution (B term) based on Sachs form factors.
Extract the weak charge and weak mixing angle from acceptance corrected tree level asymmetry.
Properly compute errors for weak charge and weak mixing angle. The Q^2 error is also included.
Print summary of EW radiative corrections 
compile and execute after changing input parameters.
Input parameters from Wien0 data set are hard coded now.
*/


#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <new>
#include <cstdlib>
#include <math.h>
#include <stdexcept>
#include <time.h>
#include <cstdio>
#include <TMath.h>

#include "TRandom.h"

using namespace std;


//list of inputs
//constants
const Double_t mu_p  = 2.79; //magnetic moments of proton and neutron
const Double_t mu_n  =-1.91;
const Double_t G_F   = 1.166e-5;// (Gev)^{-2}
const Double_t alpha = 7.299e-3; //1/137
const Double_t M_p   = 0.938;//GeV
const Double_t sin2_theta_w_0 = 0.2387; //weak mixing angle at E=0 from PRD72,073003(2005)
const Double_t delta_sin2_theta_w_0 = 0.00016; //weak mixing angle at E=0 from PRD72,073003(2005)
const Double_t sin2_theta_w_Z = 0.2312; //weak mixing angle at E=Z-pole (\hat{s^2}_Z) from PDG 2012
//const Double_t Qwp = 0.0713;//weak charge of the proton from PRD72,073003(2005)
//const Double_t delta_Qwp = 0.0008;//weak charge of the proton from PRD72,073003(2005)
const Double_t Qwp = 0.0705;//weak charge of the proton from PRL107
const Double_t delta_Qwp = 0.0008;
const Double_t Qwn = -0.989;//weak charge of the neutron (e-log 796) orginal reference: C_1u, C_1d in the PDG 2012 Table 10.2
/*
C_1u = -0.1885
C_1d = 0.3414
 */
const Double_t Qwe = -0.0474;//weak charge of the electron computed using Qwe = -2 * C_2e in the PDG 2012 Table 10.2 

//list of inputs to compute weak mixing angle
const Double_t M_e   = 0.000510;//GeV
const Double_t M_Z   = 91.1876;//GeV
const Double_t hat_alpha = 7.816e-3; //1/127.944 alpha at Z-pole
const Double_t hat_s_2 = 0.2312; //weak mixing angle at E=Z-pole (\hat{s^2}_Z) from PDG 2012
const Double_t hat_c_2 = 0.7688;// 1 - hat_s_2

//list of reg. parameters to compute weak mixing angle
Double_t rho_NC;
Double_t Delta_e;
Double_t Delta_e_prime;
Double_t Box_WW;
Double_t Box_ZZ;
Double_t Box_gammaZ;


//qweak kinematics
const Double_t Q2=0.0250;//(GeV)^2
const Double_t delta_Q2=0.0006;//(GeV)^2
const Double_t Es=1.155;//GeV
const Double_t theta=0.13788;//rad (7.9 deg)
Double_t tau;
Double_t epsilon;
Double_t epsilonp;

//measured tree-level asymmetry
Double_t A_tree;//in ppm
Double_t delta_A_tree_stat;//stat error in ppm
Double_t delta_A_tree_syst;//syst error in ppm
Double_t delta_A_tree_thry;//theory error in ppm

//SM ep asymmetry parameters
Double_t A_LR_ep;//total ep asymmetry calculated using rakitha_dissertation chapter 2 eq 2.22, assumed G^(s)_M and G^(s)_E are zero
Double_t delta_A_LR_ep;//total ep asymmetry error
Double_t A_LR_0;//A_LR_0=G_F.Q2/(2.pi.alpha.sqrt(2) rakitha_dissertation chapter 2 eq 2.12
Double_t delta_A_LR_0;//to include the Q^2 error: delta_Q2*A_LR_0/Q2
Double_t A_LR_Qwp;//contribution from weak charge of the proton
Double_t delta_A_LR_Qwp;//contribution from weak charge of the proton error
Double_t A_LR_Had;//contribution from EM form factors (strange form factor contribution is neglected)
Double_t A_LR_Ax_Had;//Axial vector hadronic contribution
Double_t A_LR_Had_Total;//Total hadronic contribution
Double_t delta_A_LR_Had_Total;//Total hadronic contribution error

//Sachs EM form-factors
Double_t Gp_E;
Double_t Gp_M;
Double_t Gn_E;
Double_t Gn_M;

//axial-vector proton form factor
Double_t G_tild_p_A;
Double_t GA_D;//dipole term for the axial form factor (see rakitha_dissertation Appendix:"Elastic ep Asymmetry SM Prediction"
const Double_t G3_A_0=1.262/2;//at tau=0 (see rakitha_dissertation Appendix:"Elastic ep Asymmetry SM Prediction"
const Double_t lambdaA_D=3.32;
const Double_t dlambdaA_D_p=0.24;
const Double_t dlambdaA_D_n=-0.22;

Double_t xiT1_A;//coupling coefficient (\xi_A^{T=1}) (see rakitha_dissertation Appendix:"Elastic ep Asymmetry SM Prediction"
const Double_t RT1_A=-0.34;//higher order corrections to \xi_A^{T=1} (see rakitha_dissertation Appendix:"Elastic ep Asymmetry SM Prediction"

//dipole form factor function
Double_t GV_D;

//Gamma-Z box correction for  E=1.165 GeV to E=0
const Double_t gamma_Z_Box = 0.0040;//see rakitha_dissertation Appendix:"The Gamma_Z Box Correction to Qweak Results" ref. PRL 107
const Double_t delta_gamma_Z_Box = 0.0011;//error
Double_t Asym_gamma_Z_Box;//convert the correction term to ppm using A_LR_0
Double_t delta_Asym_gamma_Z_Box;//error (ppm)


//Weak charge results from Qweak
Double_t Asym_Qwp_msr;//The measured asymmetry from the weak charge (ppm)
Double_t delta_Asym_Qwp_msr_stat;//error (ppm)
Double_t delta_Asym_Qwp_msr_syst;//error (ppm)
Double_t delta_Asym_Qwp_msr_thry;//error (ppm)
Double_t Qwp_msr;//measured results of the weak charge of the proton (ppm)
Double_t delta_Qwp_msr_stat;// (ppm)
Double_t delta_Qwp_msr_syst;// (ppm) 
Double_t delta_Qwp_msr_thry;// (ppm)
Double_t delta_Qwp_msr_Q2;// (ppm)

//Weak mixing angle results from Qweak
Double_t sin2_theta_w_msr;//measured results of the weak mixing angle
Double_t delta_sin2_theta_w_msr_stat;//statistical error of the weak mixing angle
Double_t delta_sin2_theta_w_msr_syst;//systematic error of the weak mixing angle
Double_t delta_sin2_theta_w_msr_thry;//theory error of the weak mixing angle
Double_t delta_sin2_theta_w_msr_Q2;//error of the weak mixing angle from Q^2

void printInputs();//Print all the input values
void computeSM_Asym();//compute the SM prediction
void computeSM_Asym_Error();//compute the SM prediction
void computeGamma_Z_Correction();//compute the Gamma-Z correction to the measured asymmetry
void computeWeakCharge();//compute the weak charge using measured ep tree-level asymmetry
void computeWeakMixingAngle();//compute the weak mixing angle using measured weak charge 
void computeIsoCouplingBand();//compute the range of the isoscalar and isovector coupling using the measured weak charge

void printResults();//Print all the results





int main(Int_t argc,Char_t* argv[])
{
  A_tree = -0.2788;//ppm rakitha_dissertation end of the Analysis chapter
  delta_A_tree_stat = 0.0348;//ppm
  //  delta_A_tree_syst = 0.0294;//ppm
  delta_A_tree_syst = 0.0290;//ppm //updated with new cut dpdnt error
  delta_A_tree_thry = 0;//ppm


  //compute kinematic factors
  tau=Q2/(4*TMath::Power(M_p,2));
  epsilon=1/(1+2*(1+tau)*TMath::Power(TMath::Tan(theta/2),2));
  epsilonp=TMath::Sqrt(tau*(1+tau)*(1-TMath::Power(epsilon,2)));

  //form factor calculation
  GV_D=1/TMath::Power((1+4.97*tau),2);//dipole form factor function
  //EM form factors
  Gp_E=GV_D;
  Gp_M=mu_p*GV_D;
  Gn_E=-1*mu_n*GV_D*tau/(1+5.6*tau);
  Gn_M=mu_n*GV_D;

  //axial form factors
  xiT1_A=-2*(1+RT1_A);
  GA_D=1/TMath::Power((1+lambdaA_D*tau),2);
  G_tild_p_A=xiT1_A*G3_A_0*GA_D;

  //A_LR_0
  A_LR_0=G_F*Q2/(2*TMath::Pi()*alpha*TMath::Sqrt(2));

  printInputs();
  computeSM_Asym();
  computeSM_Asym_Error();
  computeGamma_Z_Correction();
  computeWeakCharge();
  computeWeakMixingAngle();
  printResults();
  //for generating the isoscalar/isovector plot
  computeIsoCouplingBand();
  return(1);
}

void printInputs(){//Print all the input values

  printf("********Input list***************\n");
  printf(" \n Qweak Kinematics \n");
  printf("Q2                             = %+6.4f +- %6.4f (GeV)^2\n",Q2,delta_Q2);
  printf("Es                             = %+6.4f (GeV) \n",Es);
  printf("theta                          = %+6.4f rad (7.9 deg) \n",theta);
  printf("tau                            = %+6.4f \n",tau);
  printf("epsilon                        = %+6.4f \n",epsilon);
  printf("epsilonp                       = %+6.4f \n",epsilonp);

  printf("********Standard Model Parameters***************\n");
  printf("Qwp                             = %+6.4f +- %6.4f \n",Qwp,delta_Qwp);
  printf("Qwn                             = %+6.4f  \n",Qwn);
  printf("Qwe                             = %+6.4f  \n",Qwe);
  printf("sin2_theta_w_0                  = %+6.4f +- %6.4f \n",sin2_theta_w_0,delta_sin2_theta_w_0);
  printf("sin2_theta_w_Z                  = %+6.4f \n",sin2_theta_w_Z);
  printf("Gamma-Z box correction          = %+6.4f +- %6.4f \n",gamma_Z_Box,delta_gamma_Z_Box);

  printf("********Asymmetry Inputs***************\n");
  printf("A_tree                         = %+6.4f +- %6.4f (stat) +- %6.4f (syst) ppm \n",A_tree,delta_A_tree_stat,delta_A_tree_syst);


  
  printf("\n ********End of Input list***************\n");
  printf("\n");
  printf("\n");

}
void computeSM_Asym(){
  A_LR_0=G_F*Q2/(2*TMath::Pi()*alpha*TMath::Sqrt(2));
  delta_A_LR_0=TMath::Abs(delta_Q2*A_LR_0/Q2);
  A_LR_Qwp = -1*A_LR_0*Qwp/2;//contribution from weak charge of the proton
  A_LR_Had = -1*A_LR_0*Qwn*(epsilon*Gp_E*Gn_E + tau*Gp_M*Gn_M)/(2*(epsilon*TMath::Power(Gp_E,2) + tau*TMath::Power(Gp_M,2)));//contribution from EM form factors (strange form factor contribution is neglected)
  A_LR_Ax_Had = -1*A_LR_0*Qwe*epsilonp*Gp_M*G_tild_p_A/(2*(epsilon*TMath::Power(Gp_E,2) + tau*TMath::Power(Gp_M,2)));//Axial vector hadronic contribution
  A_LR_Had_Total=A_LR_Had + A_LR_Ax_Had;//total hadronic asymmetry
  A_LR_ep = A_LR_Qwp + A_LR_Had + A_LR_Ax_Had;//total ep asymmetry calculated using rakitha_dissertation chapter 2 eq 2.22, assumed G^(s)_M and G^(s)_E are zero
};

void computeSM_Asym_Error(){//compute the SM prediction
  //Error on the asymmetry from the weak charge of the Proton
  delta_A_LR_Qwp=TMath::Abs(delta_Qwp*A_LR_Qwp/Qwp);//ppm scaled from SM error on the weak charge
  //Error on the asymmetry from the total hadronic contribution
  /*
    anticipated reative error on the weak charge of the proton from the hadronic contribution is 1.5%. I used SM prediction for A_LR_Qwp to get approximate error ont the total hadronic contribution
delta_AQwp^2 = delta_A_Tree^2 + delta_A_Had_Total^2 and
delta_AQwp/AQwp |_A_Had_Total = delta_A_Had_Total/AQwp = 1.5%
  */
  delta_A_LR_Had_Total=TMath::Abs(1.5 * A_LR_Qwp/100);
  delta_A_LR_ep=TMath::Sqrt(TMath::Power(delta_A_LR_Qwp,2)+TMath::Power(delta_A_LR_Had_Total,2));
};

void computeGamma_Z_Correction(){//compute the Gamma-Z correction to the measured asymmetry
  Asym_gamma_Z_Box=-1*A_LR_0*gamma_Z_Box/2*1e+6;//correction
  delta_Asym_gamma_Z_Box=TMath::Abs(delta_gamma_Z_Box*Asym_gamma_Z_Box/gamma_Z_Box);
  //correct the measured asymmetry
  A_tree-=Asym_gamma_Z_Box;
  delta_A_tree_thry=delta_Asym_gamma_Z_Box;
};

void computeWeakCharge(){
  A_LR_Had_Total *= 1e+6;
  delta_A_LR_Had_Total *= 1e+6;
  Asym_Qwp_msr = A_tree - A_LR_Had_Total; //substract the hadronic correction
  delta_Asym_Qwp_msr_stat = delta_A_tree_stat;
  delta_Asym_Qwp_msr_syst = delta_A_tree_syst;
  delta_Asym_Qwp_msr_thry = TMath::Sqrt(TMath::Power(delta_A_LR_Had_Total,2) + TMath::Power(delta_A_tree_thry,2));

  Qwp_msr = -2*Asym_Qwp_msr*1e-6/A_LR_0;//get the weak charge using A_Qwp_msr = -A_LR_0 * Qwp/2
  delta_Qwp_msr_stat = TMath::Abs(delta_Asym_Qwp_msr_stat*Qwp_msr/Asym_Qwp_msr);
  delta_Qwp_msr_syst = TMath::Abs(delta_Asym_Qwp_msr_syst*Qwp_msr/Asym_Qwp_msr);
  delta_Qwp_msr_thry = TMath::Abs(delta_Asym_Qwp_msr_thry*Qwp_msr/Asym_Qwp_msr);
  delta_Qwp_msr_Q2   = TMath::Abs(delta_A_LR_0*Qwp_msr/A_LR_0);

};
void computeWeakMixingAngle(){//compute the weak mixing angle using measured weak charge 
  //weak charge of the proton, Qwp(SM) = [rho_NC + Delta_e].[1 - 4.sin2_theta_w_0 + Delta_e_prime] + Box_WW + Box_ZZ + Box_gammaZ (from PRD86 eq. (4))
  /*
    I will first compute the term rho_NC using Qwp(SM), sin2_theta_w_0, Delta_e, Delta_e_prime, Box_WW, Box_ZZ,and Box_gammaZ
   */
  Delta_e = -1 * alpha / ( 2 * TMath::Pi());//from PRD86 eq. (5)
  Delta_e_prime = (-1 * alpha * (1 - 4 * hat_s_2) * (TMath::Log(TMath::Power(M_Z,2)/TMath::Power(M_e,2)) + 1/6))/( 3 * TMath::Pi());//from PRD86 eq. (5)
  Box_WW = (7 * hat_alpha) / (4 * TMath::Pi() * hat_s_2);//from PRD86 eq. (6)
  Box_ZZ = (hat_alpha * (9/4 - 5 * hat_s_2)*(1 - 4*hat_s_2 + 8 * TMath::Power(hat_s_2,2)))/(4 * TMath::Pi() * hat_s_2 * hat_c_2 );//from PRD86 eq. (6)
  Box_gammaZ = 0.0044;//PRL 107
  //compute the rho_NC
  rho_NC = (( Qwp - Box_WW - Box_ZZ - Box_gammaZ )/(1 - 4*sin2_theta_w_0 + Delta_e_prime)) - Delta_e;

  //Now using Qwp_msr, rho_NC, Delta_e, Delta_e_prime, Box_WW, Box_ZZ,and Box_gammaZ the weak mixing angle is computed: sin2_theta_w_msr
  sin2_theta_w_msr = ( 1 - (((Qwp_msr  - Box_WW - Box_ZZ - Box_gammaZ )/(rho_NC + Delta_e)) - Delta_e_prime) )/4;
  delta_sin2_theta_w_msr_stat = delta_Qwp_msr_stat/4;
  delta_sin2_theta_w_msr_syst = delta_Qwp_msr_syst/4;
  delta_sin2_theta_w_msr_thry = delta_Qwp_msr_thry/4;
  delta_sin2_theta_w_msr_Q2   = delta_Qwp_msr_Q2/4;
  /*
  sin2_theta_w_msr = (1 - Qwp_msr)/4;
  delta_sin2_theta_w_msr_stat = delta_Qwp_msr_stat/4;
  delta_sin2_theta_w_msr_syst = delta_Qwp_msr_syst/4;
  delta_sin2_theta_w_msr_thry = delta_Qwp_msr_thry/4;
  */

  
};
void computeIsoCouplingBand(){//compute the range of the isoscalar and isovector coupling using the measured weak charge
  Double_t X_up,Xp_up,Xm_up,X_down,Xp_down,Xm_down;//C_1u - C_1d isovector range
  Double_t delta;
  Double_t Y;//C_1u + C_1d isoscalar

  delta=TMath::Sqrt( TMath::Power(delta_Qwp_msr_stat,2) + TMath::Power(delta_Qwp_msr_syst,2) + TMath::Power(delta_Qwp_msr_thry,2));
  printf("*********************coupling range for Y=0.10 and Y=0.18 ***********************\n");
  Y=0.18;
  X_up = (Y + Qwp_msr/3)/-0.333;  //Y = -0.333*X - Qwp_msr/3 using Y = -1/3.X - Qwp/3;
  Y=0.10;
  X_down = (Y + Qwp_msr/3)/-0.333;  //Y = -0.333*X - Qwp_msr/3 using Y = -1/3.X - Qwp/3;
  Y=0.18;
  Xp_up = (Y + (Qwp_msr+delta)/3)/-0.333;  //Y = -0.333*X - Qwp_msr/3 using Y = -1/3.X - Qwp/3;
  Y=0.10;
  Xp_down = (Y + (Qwp_msr+delta)/3)/-0.333;  //Y = -0.333*X - Qwp_msr/3 using Y = -1/3.X - Qwp/3;
  Y=0.18;
  Xm_up = (Y + (Qwp_msr-delta)/3)/-0.333;  //Y = -0.333*X - Qwp_msr/3 using Y = -1/3.X - Qwp/3;
  Y=0.10;
  Xm_down = (Y + (Qwp_msr-delta)/3)/-0.333;  //Y = -0.333*X - Qwp_msr/3 using Y = -1/3.X - Qwp/3;
  printf("    Y=0.10   Y=0.18 \n");
  printf("Xp: %+6.4f %+6.4f \n",Xp_up,Xp_down);
  printf("X :  %+6.4f %+6.4f \n",X_up,X_down);
  printf("Xm:  %+6.4f %+6.4f \n",Xm_up,Xm_down);
  printf("*********************END***********************\n");
  printf("\n");
  printf("\n");
};



void printResults(){//Print all the results
  printf("*********************SM Asymmetry***********************\n");
  printf("A_LR_0                                       = %+6.4f +- %6.4f ppm \n",A_LR_0*1e+6,delta_A_LR_0*1e+6);
  printf("A_LR_Qwp                                     = %+6.4f +- %6.4f ppm \n",A_LR_Qwp*1e+6,delta_A_LR_Qwp*1e+6);
  printf("A_LR_Had                                     = %+6.4f ppm \n",A_LR_Had*1e+6);
  printf("A_LR_Ax_Had                                  = %+6.4f ppm \n",A_LR_Ax_Had*1e+6);
  printf("A_LR_Had_Total                               = %+6.4f +- %6.4f ppm \n",A_LR_Had_Total,delta_A_LR_Had_Total);
  printf("A_LR_ep = A_LR_Qwp + A_LR_Had + A_LR_Ax_Had  = %+6.5f +- %6.4f ppm \n",A_LR_ep*1e+6,delta_A_LR_ep*1e+6);
  printf("********Measured Asymmetry (A_msr) after Gamma-Z box correction***************\n");
  printf("A_tree_corrected          = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.) +- %6.4f (theory) ppm \n",A_tree,delta_A_tree_stat,delta_A_tree_syst,delta_A_tree_thry);
  printf("Gamma-Z box correction    = %+6.4f +- %6.4f ppm \n",Asym_gamma_Z_Box,delta_Asym_gamma_Z_Box);
  printf("********Measured Asymmetry after hadronic correction (only from the weak charge)***************\n");
  printf("Asym_Qwp_msr              = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.) +- %6.4f (theory) ppm \n",Asym_Qwp_msr,delta_Asym_Qwp_msr_stat,delta_Asym_Qwp_msr_syst,delta_Asym_Qwp_msr_thry);
  printf("\n");
  printf("\n");



  printf("*********************Weak Charge of the Proton Measurement***********************\n");
  printf("Qwp_msr           = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.) +- %6.4f (theory) +- %6.4f (Q^2)  \n",Qwp_msr,delta_Qwp_msr_stat,delta_Qwp_msr_syst,delta_Qwp_msr_thry,delta_Qwp_msr_Q2);
  printf("Qwp_msr           = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.+Q^2) +- %6.4f (theory) \n",Qwp_msr,delta_Qwp_msr_stat,TMath::Sqrt(TMath::Power(delta_Qwp_msr_syst,2)+TMath::Power(delta_Qwp_msr_Q2,2)),delta_Qwp_msr_thry);
  printf("Qwp_msr           = %+6.4f +- %6.4f  \n",Qwp_msr,TMath::Sqrt( TMath::Power(delta_Qwp_msr_stat,2) + TMath::Power(delta_Qwp_msr_syst,2) + TMath::Power(delta_Qwp_msr_thry,2)));
  printf("Qwp_msr           = %+6.4f +- %6.4f (with Q^2)  \n",Qwp_msr,TMath::Sqrt( TMath::Power(delta_Qwp_msr_stat,2) + TMath::Power(delta_Qwp_msr_syst,2) + TMath::Power(delta_Qwp_msr_thry,2)+ TMath::Power(delta_Qwp_msr_Q2,2)));
  printf("\n");
  printf("\n");

  printf("*********************Weak Mixing Angle Measurement***********************\n");
  printf("sin2_theta_w           = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.) +- %6.4f (theory) \n",sin2_theta_w_msr,delta_sin2_theta_w_msr_stat,delta_sin2_theta_w_msr_syst,delta_sin2_theta_w_msr_thry);
  printf("sin2_theta_w           = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.) +- %6.4f (theory) +- %6.4f (Q^2)\n",sin2_theta_w_msr,delta_sin2_theta_w_msr_stat,delta_sin2_theta_w_msr_syst,delta_sin2_theta_w_msr_thry,delta_sin2_theta_w_msr_Q2);
  printf("sin2_theta_w           = %+6.4f +- %6.4f (stat.) +- %6.4f (syst.+Q^2) +- %6.4f (theory) \n",sin2_theta_w_msr,delta_sin2_theta_w_msr_stat,TMath::Sqrt( TMath::Power(delta_sin2_theta_w_msr_syst,2) +TMath::Power(delta_sin2_theta_w_msr_Q2,2)),delta_sin2_theta_w_msr_thry);
  printf("sin2_theta_w_msr       = %+6.4f +- %6.4f  \n",sin2_theta_w_msr,TMath::Sqrt( TMath::Power(delta_sin2_theta_w_msr_stat,2) + TMath::Power(delta_sin2_theta_w_msr_syst,2) + TMath::Power(delta_sin2_theta_w_msr_thry,2)));
  printf("sin2_theta_w_msr       = %+6.4f +- %6.4f (with Q^2)  \n",sin2_theta_w_msr,TMath::Sqrt( TMath::Power(delta_sin2_theta_w_msr_stat,2) + TMath::Power(delta_sin2_theta_w_msr_syst,2) + TMath::Power(delta_sin2_theta_w_msr_thry,2) +TMath::Power(delta_sin2_theta_w_msr_Q2,2) ));
  printf("\n");
  printf("\n");
  printf("*********************Corrections to Weak Charge to Extract Mixing Angle***********************\n");
  printf(" rho_NC                    = %+6.4f \n",rho_NC);
  printf(" Delta_e                   = %+6.4f \n",Delta_e);
  printf(" Delta_e_prime             = %+6.4f \n",Delta_e_prime);
  printf(" Box_WW                    = %+6.4f \n",Box_WW);
  printf(" Box_ZZ                    = %+6.4f \n",Box_ZZ);
  printf(" Box_gammaZ                = %+6.4f \n",Box_gammaZ);
  printf(" QWp = %6.3f (1-4sin^2theta_w + %6.3f) + %6.3f \n",rho_NC+Delta_e,Delta_e_prime, (Box_WW+Box_ZZ+Box_gammaZ));
  printf("\n");
  printf("\n");
}
