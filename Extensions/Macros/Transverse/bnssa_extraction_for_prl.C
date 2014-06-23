/*
June 10th, 2014
B.Waidyawansa

Coppied from bnssa_extraction.C script I used exactly an year ago to get the final
elastic e+p BNSSA asymmetry presented in my thesis (Qweak-doc-1886-v1)
This time around, it will have a better non-linearity error estimate based on the analysis presented i
Qweak-doc-2013-v7.
*/


/**************************************************
 Input parameters
***************************************************/
// Beam polarization
// pg. 157
// Run II (needed for inelastic asymmetry extraction)
const Double_t P_II  = 0.8750;
const Double_t dP_II = 0.0079;
// Run I and Run II (average)
const Double_t P_b  = 0.8804;
const Double_t dP_b = 0.0087;

// Aluminum dilution
// pg. 160
const Double_t f_al      = 0.033;
const Double_t df_al     = 0.002;

// Elastic dilution at the inelastic peak (at 6700A)
// pg. 162
const Double_t f_e  = 0.669;
const Double_t df_e = 0.067;

// Inelastic dilution at the elastic peak (8901A)
// pg. 162
const Double_t f_ie  = 0.0002;
const Double_t df_ie = 0.0001;

// Detector bias and radiative correction
// pg. 167
const Double_t R  = 0.9979;
const Double_t dR = 0.0032;

// Acceptance averaging correction factor
// pg. 148
const Double_t f_acc = 0.9938; // assign a 50% error
// Non-linearity factors
// elog 1166
const Double_t f_lh2_nonlin = 0.003;
const Double_t f_al_nonlin  = 0.047; 

// General analysis systematic errors
// pg. 153
const Double_t dA_reg = 0.029;// ppm  regression scheme dependance

// US and DS Al target window acceptance difference
// pg 158
const Double_t f_us = 0.8;

// Measured asymmetries 
// pg. 145
const Double_t A_msr_ep       = -4.805; 
const Double_t dA_msr_ep_stat =  0.057;
// pg. 157 
const Double_t A_msr_dsal       = -8.929; 
const Double_t dA_msr_dsal_stat =  0.340; 
// pg. 161
const Double_t A_lh2_ie_meas       = 5.046; //ppm 
const Double_t dA_lh2_ie_meas      = 0.445; //ppm  
// pg. 268
const Double_t A_al_ds_ie_meas     = 8.419; //ppm  
const Double_t dA_al_ds_ie_meas    = 0.984; //ppm  

// Rough estimate for the elastic BNSSA. This is needed
// for the inelastic BNSSA calculation
const Double_t Bn_e_temp = -5.0; //ppm

/**************************************************
 To be determined (all are in ppm)
***************************************************/
// Aluminum asymmetry 
Double_t dA_al_ep  = 0.0; 
Double_t A_al_ep   = 0.0;
Double_t dA_al_ie  = 0.0; 
Double_t A_al_ie   = 0.0;

//aray to load calculated asymmetry and error from functions
Double_t* A;

// Inelastic LH2 cell asymmetry 
Double_t A_lh2_ie  = 0.0;
Double_t dA_lh2_ie = 0.0;
Double_t Bn_e_6700 = 0.0;
Double_t dBn_e_6700 = 0.0;

// Inelastic ep asymmetry
Double_t dA_ie_ep  = 0.0;
Double_t A_ie_ep   = 0.0;

// final extracted elastic BNSSA
Double_t B_n       = 0.0; 
Double_t B_n_msr   = 0.0;

Double_t dA_msr_ep_sys =  0.0;

/////////////////////////////////////////////
// Main function to extrac elastic e+p BNSSA
/////////////////////////////////////////////

void elastic_bnsa_extraction(){

  gDirectory->Clear();

  // Double_t Bmsr = -4.835;//ppm 
  // Double_t dBmsr_stat = 0.057; //ppm
  // Double_t dBmsr_non_linear = 0.096; //ppm i.e. 4.805x0.02
  // Double_t dBmsr_reg_scheme = 0.029; //ppm
  // Double_t dBmsr_acc_avg = 0.015; //ppm


  std::cout<<"######################################################################"<<std::endl;
  std::cout<<"Elastic aluminum Background Correction "<<std::endl;
  std::cout<<"######################################################################"<<std::endl;
  //No systematic correction for reg-scheme dependance.
  A = aluminum_asymmetry(A_msr_dsal,dA_msr_dsal_stat,f_al_nonlin,P_b,dP_b);
  A_al_ep = *(A);
  dA_al_ep = *(A+1);
  Double_t al_corr = -A_al_ep*f_al;
  std::cout<<"Aluminum background correction(-fA) = "<<al_corr<<" ppm"<<std::endl;

  std::cout<<"\n######################################################################"<<std::endl;
  std::cout<<" Inelastic e+p Background Correction "<<std::endl;
  std::cout<<"######################################################################"<<std::endl;
  std::cout<<"\nInelastic Aluminum asymmetry"<<std::endl;
  std::cout<<"==============================="<<std::endl;
  A = aluminum_asymmetry(A_al_ds_ie_meas,dA_al_ds_ie_meas,f_al_nonlin,P_II,dP_II);
  A_al_ie = *(A);
  dA_al_ie = *(A+1);
 
  std::cout<<"\nInelastic LH2-cell asymmetry"<<std::endl;
  std::cout<<"==============================="<<std::endl;
  inelastic_asymmetry();
  

  // Final elastic e+p asymmetry
  std::cout<<"\n######################################################################"<<std::endl;
  std::cout<<" Final asymmetry "<<std::endl;
  std::cout<<"######################################################################"<<std::endl;

  // correct for acceptance averaging
  Double_t A_msr_ep_acc  = A_msr_ep/f_acc;
  std::cout<<"Acceptance corrected measured LH2-cell asymmetry (ppm)= "<<A_msr_ep_acc <<std::endl; 

  // systematics for non-linearity and acceptance averaging
  Double_t dA_msr_ep_acc    = (A_msr_ep-A_msr_ep_acc)*0.5; //50% error on acceptance correction
  Double_t dA_msr_ep_nonlin = (A_msr_ep*f_lh2_nonlin);
  Double_t dA_msr_ep_sys    =  sqrt(pow(dA_msr_ep_nonlin,2) + pow(dA_reg,2) + pow(dA_msr_ep_acc,2));

  A = two_bkg_correction(P_b, dP_b,
			 A_msr_ep_acc, dA_msr_ep_stat,
			 dA_reg,  dA_msr_ep_acc, dA_msr_ep_nonlin,
			 A_al_ep, dA_al_ep,
			 f_al, df_al,
			 A_lh2_ie, dA_lh2_ie,
			 f_ie, df_ie,
			 R, dR);


}


/**************************************************
 Extract the Aluminum background asymmetry
***************************************************/
Double_t * aluminum_asymmetry(Double_t A_ds4al_reg, Double_t dA_ds4al_stat,
			 Double_t non_linearity, Double_t P, Double_t dP){

  static Double_t  param[2];
  // correct for acceptance averaging
  Double_t A_ds4al_acc = A_ds4al_reg/f_acc;
  std::cout<<"Acceptance corrected measured 4% DS aluminum asymmetry (ppm)= "<<A_ds4al_acc <<std::endl; 


  // correction for qcceptance difference between upstream and downstream target windows
  // pg. 166
  Double_t A_usal = sqrt( f_us)*A_ds4al_acc;
  std::cout<<"asymmetry contribution from upstream target window (ppm)= "<<A_usal<<ste::endl;

  // systematic errors on non-linearity, acceptance averaging, reg-scheme dependance
  Double_t dA_ds4al_nonlin = (A_ds4al_reg*non_linearity);
  Double_t dA_ds4al_acc = (A_ds4al_reg-A_ds4al_acc)*0.5; 
  Double_t dA_ds4al_sys = sqrt(pow(dA_ds4al_nonlin,2)+pow(dA_ds4al_acc,2));
  Double_t dA_ds4al_tot = sqrt(pow(dA_ds4al_stat,2)+pow(dA_ds4al_sys,2));
  std::cout<<"\tstatistical error (ppm)= "<<dA_ds4al_stat<<std::endl;
  std::cout<<"\tsystematic error (ppm)= "<<dA_ds4al_sys<<std::endl;
  std::cout<<"\ttotal error (ppm)= "<<dA_ds4al_tot<<std::endl;

  // Correct for upstream downstream acceptance difference
  Double_t A_al_phys = ( A_usal +  A_ds4al_acc)/2;
  // apply a 50% error on the correction
  Double_t dA_us_ds_acceptance = (A_al_phys- A_ds4al_acc)*0.5; 


  Double_t dA_al_phys = sqrt(pow(dA_ds4al_tot,2)+pow(dA_us_ds_acceptance,2));
  std::cout<< "Aluminum physics asymmetry (ppm)= "<<A_al_phys<<std::endl;
  std::cout<< "\terror (ppm)= "<<dA_al_phys<<std::endl;

  // extract Aluminum e+p elastic asymmetry
  Double_t A_al = A_al_phys/P;
  std::cout<< "Effective aluminum ep asymmetry (ppm)= "<<A_al<<std::endl;
  Double_t dA_al = sqrt(pow((dA_al_phys/P),2)+pow(((A_al*dP)/P),2));
  std::cout<<"\terror (ppm)= "<<dA_al<<std::endl;

  // Fill the array
  param[0]= A_al;
  param[1]= dA_al;

  return param;

}

/**************************************************
 Extract the LH2 cell inelastic e+p asymmetry
***************************************************/
void inelastic_asymmetry(){

  // correct for acceptance averaging
  Double_t A_lh2_ie_acc = A_lh2_ie_meas/f_acc;
  std::cout<<"Acceptance corrected asymmetry (ppm) = "<<A_lh2_ie_acc<<std::endl;

  // systematics for non-linearity and acceptance averaging
  Double_t dA_lh2_ie_nonlin = (A_lh2_ie_meas*f_lh2_nonlin);
  Double_t dA_lh2_ie_acc = (A_lh2_ie_meas-A_lh2_ie_acc)*0.5; //50% error on correction
  Double_t dA_lh2_ie_sys = sqrt(pow(dA_lh2_ie_nonlin,2)+pow(dA_lh2_ie_acc,2));
  dA_lh2_ie = sqrt(pow(dA_lh2_ie_meas,2)+pow(dA_lh2_ie_sys,2));

  std::cout<<"\tstatistical error (ppm) = "<<dA_lh2_ie_meas<<std::endl;
  std::cout<<"\tsystematic error (ppm) = "<<dA_lh2_ie_sys<<std::endl;
  std::cout<<"\ttotal error (ppm) = "<<dA_lh2_ie<<std::endl; 

  elastic_asym_at_inelastic_peak(Bn_e_temp);

  A = two_bkg_correction(P_II, dP_II,
			 A_lh2_ie_acc, dA_lh2_ie_meas, 0, dA_lh2_ie_nonlin, dA_lh2_ie_acc,
			 A_al_ie, dA_al_ie, 
			 f_al, df_al,
			 Bn_e_6700, dBn_e_6700,
			 f_e, df_e,
			 1.0, 0.0); // No Q2 bias error for inelastics

  A_lh2_ie = *A;
  dA_lh2_ie = *(A+1);

  std::cout<<A_lh2_ie<<std::endl;
}

/**************************************************
 Given the elastic BNSSA at the elastic peak,
 extract the elastic e+p BNSSA at the inelastic
 peak (QTOR = 6700A).
***************************************************/
void elastic_asym_at_inelastic_peak(Double_t Bn_e){

  // Use the ratio in the Q2 at elastic and inelastic peaks
  // to scale the asymmetry since at low Q2, Bn ~ Q (theory).

  Bn_e_6700 = Bn_e*sqrt(0.021/0.025);
  std::cout<<"Elastic BNSSA at the inelastic peak ="<<Bn_e_6700<<std::endl;

  // assign an error of 50%
  dBn_e_6700 = Bn_e_6700*0.5;
  std::cout<<"\ttotal error (ppm) = "<<dBn_e_6700<<std::endl; 

}

/**************************************************
 Apply a two background correction
 Errors are seperated into stat, sys1(from reg), 
 sys2(from acceptance) and sys3 (from non-linearity)
***************************************************/
Double_t * two_bkg_correction(Double_t P_beam,Double_t dP_beam,  
			      Double_t A_p,Double_t dA_p_stat, 
			      Double_t dA_p_sys1, Double_t dA_p_sys2, Double_t dA_p_sys3,
			      Double_t A_b1, Double_t dA_b1,
			      Double_t fb1,Double_t dfb1,
			      Double_t A_b2, Double_t dA_b2,
			      Double_t fb2, Double_t dfb2, 
			      Double_t RQ, Double_t dRQ){

  // beam polarization of the physics measurement
  std::cout<<"P_beam = "<<P_beam<<"+-"<<dP_beam<<std::endl;

  // measured physics asymmetry(after regression, phi acceptance and non-linearity corrections)
  // errors are seperated into stat, sys1(from reg), sys2(from acceptance) and sys3 (from non-linearity)
  std::cout<<"A_p  = "<<A_p<<" +- "<<dA_p_stat<<" +- "<<dA_p_sys1<<"+-"<<dA_p_sys2<<"+-"<<dA_p_sys3<<std::endl;

  // backgrounds
  std::cout<<"A_b1 = "<<A_b1<<" +- "<<dA_b1<<std::endl;
  std::cout<<"A_b2 = "<<A_b2<<" +- "<<dA_b2<<std::endl;
  std::cout<<"f_b1 = "<<fb1<<" +- "<<dfb1<<std::endl;
  std::cout<<"f_b1 = "<<fb2<<" +- "<<dfb2<<std::endl;

  // kinematics+detector bias
  std::cout<<"RQ   = "<<RQ<<" +- "<<dRQ<<std::endl;

  static Double_t  param[2];

  Double_t A_corrected = RQ*(((A_p/P_beam)-A_b1*fb1-A_b2*fb2)/(1-fb1-fb2));
  std::cout<<"Polarization and background corrected asymmetry (ppm) = "<<A_corrected<<std::endl;

  Double_t denom = (1-fb1-fb2);
  std::cout<<"(1-f1-f2) ="<<denom<<std::endl;

  Double_t partial_P = (-(A_p*RQ)/(pow(P_beam,2)*denom))*dP_beam;
  std::cout<<"partial_P dP = "<<partial_P<<std::endl;

  Double_t partial_A_p_stat = (RQ/(P_beam*denom))*dA_p_stat;
  std::cout<<"partial_A_p_stat dA_p_stat = "<<partial_A_p_stat<<std::endl;

  Double_t partial_A_p_sys1 = (RQ/(P_beam*denom))*dA_p_sys1;
  std::cout<<"partial_A_p_sys1 dA_p_sys1 = "<<partial_A_p_sys1<<std::endl;
  
  Double_t partial_A_p_sys2 = (RQ/(P_beam*denom))*dA_p_sys2;
  std::cout<<"partial_A_p_sys2 dA_p_sys2 = "<<partial_A_p_sys2<<std::endl;
  
  Double_t partial_A_p_sys3 = (RQ/(P_beam*denom))*dA_p_sys3;
  std::cout<<"partial_A_p_sys3 dA_p_sys3 = "<<partial_A_p_sys3<<std::endl;

  Double_t partial_A_b1 = ((-fb1*RQ)/denom)*dA_b1;
  std::cout<<"partial_A_b1 dAb1 = "<<partial_A_b1<<std::endl;

  Double_t partial_A_b2 = ((-fb2*RQ)/denom)*dA_b2;
  std::cout<<"partial_A_b2 dAb2 = "<<partial_A_b2<<std::endl;

  Double_t partial_fb1 = (RQ*((A_p/P_beam)+(-1+fb2)*A_b1-fb2*A_b2)/pow(denom,2))*dfb1;
  std::cout<<"partial_fb1 dfb1 = "<<partial_fb1<<std::endl;

  Double_t partial_fb2 = (RQ*((A_p/P_beam)+(-1+fb1)*A_b2-fb1*A_b1)/pow(denom,2))*dfb2;

  std::cout<<"partial_fb2 dfb2 = "<<partial_fb2<<std::endl;

  // Double_t partial_fb2 = (-A_b2*(1-fb1)*RQ*dfb2)/(denom*denom);
  // std::cout<<"partial_fb2 dfb2 = "<<partial_fb2<<std::endl;

  Double_t partial_RQ = (A_p/RQ)*dRQ;
  std::cout<<"partial_RQ dRQ = "<<partial_RQ<<std::endl;

  Double_t  dA_c_sys = sqrt( pow(partial_P,2)+pow(partial_RQ,2)
			     + pow(partial_A_p_sys1,2)+ pow(partial_A_p_sys2,2)+ pow(partial_A_p_sys3,2)
			     + pow(partial_A_b1,2)+pow(partial_A_b2,2)
			     +pow(partial_fb1,2)+pow(partial_fb2,2));

  Double_t dA_c = sqrt( pow(partial_A_p_stat,2)+ pow(dA_c_sys,2));

  std::cout<<"\tStat error = "<<partial_A_p_stat<<std::endl;
  std::cout<<"\tSys error = "<<dA_c_sys<<std::endl;
  std::cout<<"\tTotal Error = "<<dA_c<<std::endl;

  std::cout<<"\n###################################"<<std::endl;
  std::cout<<" Relative Errors % "<<std::endl;
  std::cout<<"###################################"<<std::endl;
  std::cout<<"Statistics = "<< TMath::Abs(partial_A_p_stat/A_corrected)*100<<std::endl;
  std::cout<<"Polarization = "<< TMath::Abs(partial_P/A_corrected)*100<<std::endl;
  std::cout<<"System non-linearity = "<<TMath::Abs(partial_A_p_sys3/A_corrected)*100<<std::endl;
  std::cout<<"Reg. scheme dependance = "<<TMath::Abs(partial_A_p_sys1/A_corrected)*100<<std::endl;
  std::cout<<"Q2 bias = "<< TMath::Abs(partial_RQ/A_corrected)*100<<std::endl;
  std::cout<<"Acceptance average = "<< TMath::Abs(partial_A_p_sys2/A_corrected)*100<<std::endl;
  std::cout<<"Aluminum = "<< TMath::Abs(partial_A_b1/A_corrected)*100<<std::endl;
  std::cout<<"Aluminum dilution = "<< TMath::Abs(partial_fb1/A_corrected)*100<<std::endl;
  std::cout<<"Inelastic dilution= "<< TMath::Abs(partial_fb2/A_corrected)*100<<std::endl;
  std::cout<<"Inelastic = "<< TMath::Abs(partial_A_b2/A_corrected)*100<<std::endl;


  
  std::cout<<"\n###################################"<<std::endl;
  std::cout<<" Individual corrections (ppm) "<<std::endl;
  std::cout<<"###################################"<<std::endl;

  Double_t delta_A_polar = (RQ/denom)*((A_p/P_beam)-A_p);
  std::cout<<" Beam polarization correction delta_A_polar = "
	   <<delta_A_polar<<std::endl;

  Double_t delta_A_b1 = (RQ/denom)*(-fb1*A_b1);
  std::cout<<" Aluminum background correction delta_A_b1 = "
	   <<delta_A_b1<<std::endl;

  Double_t delta_A_b2 = (RQ/denom)*(-fb2*A_b2);
  std::cout<<" Inelastic background correction delta_A_b2 = "
	   <<delta_A_b2<<std::endl;

  Double_t delta_A_Q = ((RQ*A_p)/denom)-A_p;
  std::cout<<" Q2 bias correction = "
	   <<delta_A_Q<<std::endl;

  param[0] = A_corrected;
  param[1] = dA_c;

  return param;

}

