/*
June 4th, 2013
B.Waidyawansa
The code used to extract elastic BNSSA from the LH2 data with inelastic and
aluminum background corrections. 
This code is meant to perform a task. Not one of my elegant coding work.
So don't expect it to be perfect : )
*/

/////////////////////////////////////////////
// Input parameters
/////////////////////////////////////////////
// Beam polarization
// Run II (needed for inelastic asymmetry extraction)
Double_t P_II = 0.8778;
Double_t dP_II = 0.0099;

// Run I and Run II (total)
Double_t P_b = 0.8804;
Double_t dP_b = 0.0087;

// Aluminum asymmetry and dilution
Double_t dA_al_ep=0.0;
Double_t A_al_ep=0.0;
Double_t fal = 0.033;
Double_t dfal = 0.002;

// Inelastic LH2 cell asymmetry and dilution
Double_t A_ie_acc=0.0;
Double_t dA_ie_acc=0.0;
Double_t fe = 0.669;
Double_t dfe = 0.067;
Double_t Ae_6700 = 0.0;

// Inelastic ep asymmetry
Double_t dA_ie_ep=0.0;
Double_t A_ie_ep=0.0;
Double_t fie = 0.0002;
Double_t dfie = 0.0001;

// Detector bias and radiative correction
Double_t R_q2 = 1.00;

// Acceptance averaging correction
Double_t acceptance = 0.9938; // assign a 50% error

// General analysis systematic errors
Double_t dA_reg = 0.00;// ppm  regression scheme dependance


/////////////////////////////////////////////
// Main function to extrac elastic e+p BNSSA
/////////////////////////////////////////////

void elastic_bnsa_extraction(){

  Double_t Bn = 0.0; //to be determined
  Double_t Bmsr = -4.835;//ppm 
  Double_t dBmsr_stat = 0.057; //ppm
  Double_t dBmsr_non_linear = 0.096; //ppm i.e. 4.805x0.02
  Double_t dBmsr_reg_scheme = 0.029; //ppm
  Double_t dBmsr_acc_avg = 0.015; //ppm


  std::cout<<"###################################"<<std::endl;
  std::cout<<"Elastic aluminum Background Correction "<<std::endl;
  std::cout<<"###################################"<<std::endl;
  //Aluminum measured asymmetry = -8.929 +-0340 (stat)
  //Non-linearity = 0.040 x asymmetry
  //No systematic correction for reg-scheme dependance.
  aluminum_correction(-8.929,0.340,0.04,P_b,dP_b);
  Double_t al_corr = -A_al_ep*fal;
  std::cout<<"Aluminum background correction(-fA) = "<<al_corr<<std::endl;

  std::cout<<"\n###################################"<<std::endl;
  std::cout<<" Inelastic e+p Background Correction "<<std::endl;
  std::cout<<"###################################"<<std::endl;
  std::cout<<"\nInelastic Aluminum"<<std::endl;
  std::cout<<"==================="<<std::endl;
  aluminum_correction(8.419,0.984,0.04,P_II,dP_II);
  std::cout<<"\nInelastic LH2-cell asymmetry"<<std::endl;
  std::cout<<"==================="<<std::endl;
  inelastic_asymmetry();
  elastic_asym_at_inelastic_peak(-5);
  // error on the elastic asymmetry at the inelastic peak is 50%
  Double_t dAe_6700 = Ae_6700*0.5;
  two_bkg_correction(P_II,  A_ie_acc, 9.141, fal,  Ae_6700, fe, 1.0,
		     dP_II, 0.455,0.103, 1.156,0,0
		     , dfal, dAe_6700, dfe, 0.0); // No Q2 bias for inelastics

 // Final elastic e+p asymmetry
  std::cout<<"\n###################################"<<std::endl;
  std::cout<<" Final asymmetry "<<std::endl;
  std::cout<<"###################################"<<std::endl;
  two_bkg_correction(P_b,  Bmsr, -9.667, fal, 28.85,fie,0.9979,
		     dP_b, dBmsr_stat,dBmsr_acc_avg,dBmsr_reg_scheme,dBmsr_non_linear
		     ,0.603, dfal, 6.39433, dfie,0.004);



}


/////////////////////////////////////////////
// Function to extract aluminum correction
////////////////////////////////////////////

void aluminum_correction(Double_t A_ds4al_reg, Double_t dA_ds4al_stat,
			 Double_t non_linearity, Double_t P, Double_t dP){

  // correct for acceptance averaging
  Double_t A_ds4al_acc = A_ds4al_reg/acceptance;
  std::cout<<"Acceptance corrected DS4% aluminum asymmetry = "<<A_ds4al_acc <<std::endl; 

  // systematics for non-linearity and acceptance averaging and reg-scheme dependance
  Double_t dA_ds4al_non_lin = (A_ds4al_reg*non_linearity);
  Double_t dA_ds4al_acc = (A_ds4al_reg-A_ds4al_acc)*0.5;
  Double_t dA_ds4al_sys = sqrt(pow(dA_ds4al_non_lin,2)+pow(dA_ds4al_acc,2)+pow(dA_reg,2));
  Double_t dA_ds4al_tot = sqrt(pow(dA_ds4al_stat,2)+pow(dA_ds4al_sys,2));
  std::cout<<"\tstatistical error ="<<dA_ds4al_stat<<std::endl;
  std::cout<<"\tsystematic error ="<<dA_ds4al_sys<<std::endl;
  std::cout<<"\ttotal error ="<<dA_ds4al_tot<<std::endl;

  // acceptance correction for Q2_US = 0.8 Q2_DS
  Double_t A_usal = sqrt(0.8)*A_ds4al_acc;
  std::cout<<"asymmetry from US window ="<<A_usal<<ste::endl;
  Double_t A_al_phys = ( A_usal +  A_ds4al_acc)/2;
  Double_t dA_us_ds_acceptance = (A_al_phys- A_ds4al_acc)*0.5; // 50% uncertaitny on the correction for taking Bn = sqrt(Q2)

  // systematic errors
  Double_t dA_al_phys_stat = dA_ds4al_tot*(1+sqrt(0.8))/2;
  Double_t dA_al_phys = sqrt(pow(dA_us_ds_acceptance,2)+pow( dA_al_phys_stat,2));
  std::cout<< "Aluminum physics asymmetry = "<<A_al_phys<<std::endl;
  std::cout<< "\terror = "<<dA_al_phys<<std::endl;

  // polarization correction
  A_al_ep = A_al_phys/P;
  std::cout<< "Effective aluminum ep asymmetry = "<<A_al_ep<<std::endl;
  Double_t pow_dA_al_ep = (dA_al_phys/P)*(dA_al_phys/P)+((A_al_ep*dP)/P)*((A_al_ep*dP)/P);
  dA_al_ep = sqrt(pow_dA_al_ep);
  std::cout<<"\terror ="<<dA_al_ep<<std::endl;
}

void inelastic_asymmetry(){

  Double_t A_ie_reg = 5.046; //ppm 5.088
  Double_t dA_ie_stat = 0.445; //ppm 0.455
  Double_t non_linearity = 0.015; // for lh2-cell
  Double_t acceptance = 0.9938; // assign 50% error

  // correct for acceptance averaging
  A_ie_acc = A_ie_reg/acceptance;
  std::cout<<"Acceptance corrected asymmetry = "<<A_ie_acc <<std::endl;

  // systematics for non-linearity and acceptance averaging
  Double_t dA_ie_non_lin = (A_ie_reg*non_linearity);
  Double_t dA_ie_acc = (A_ie_reg-A_ie_acc)*0.5;
  Double_t dA_ie_sys = sqrt(pow(dA_ie_non_lin,2)+pow(dA_ie_acc,2));
  dA_ie_acc = sqrt(pow(dA_ie_stat,2)+pow(dA_ie_non_lin,2)+pow(dA_ie_acc,2));
  std::cout<<"\tstatistical error ="<<dA_ie_stat<<std::endl;
  std::cout<<"\tsystematic error ="<<dA_ie_sys<<std::endl;
  std::cout<<"\ttotal error ="<<dA_ie_acc<<std::endl;


  
}

/////////////////////////////////////////////
// Function to extract elastic asym. at inelastic peak.
////////////////////////////////////////////
void elastic_asym_at_inelastic_peak(Double_t A_e){

  Ae_6700 = A_e*sqrt(0.021/0.025);
  std::cout<<"Elastic asymmetry at the inelastic peak ="<<Ae_6700<<std::endl;
}

/////////////////////////////////////////////
// A general function to apply a two background correction.
////////////////////////////////////////////
void two_bkg_correction(Double_t P_beam, Double_t A_p,
			Double_t A_b1, Double_t fb1,
			Double_t A_b2, Double_t fb2, Double_t RQ,
			Double_t dP_beam, 
			Double_t dA_p_stat, Double_t dA_p_sys1, Double_t dA_p_sys2, Double_t dA_p_sys3,
			Double_t dA_b1, Double_t dfb1,
			Double_t dA_b2, Double_t dfb2, Double_t dRQ){

  std::cout<<"P_beam "<<P_beam<<"+-"<<dP_beam<<std::endl;
  std::cout<<"A_p "<<A_p<<" +- "<<dA_p_stat<<" +- "<<dA_p_sys1<<"+-"<<dA_p_sys2<<"+-"<<dA_p_sys3<<std::endl;
  std::cout<<"A_b1 "<<A_b1<<" +- "<<dA_b1<<std::endl;
  std::cout<<"A_b2 "<<A_b2<<" +- "<<dA_b2<<std::endl;
  std::cout<<"A_b1 "<<fb1<<" +- "<<dfb1<<std::endl;
  std::cout<<"A_b1 "<<fb2<<" +- "<<dfb2<<std::endl;
  std::cout<<"RQ "<<RQ<<" +- "<<dRQ<<std::endl;


  Double_t A_corrected = RQ*(((A_p/P_beam)-A_b1*fb1-A_b2*fb2)/(1-fb1-fb2));
  std::cout<<"Polarization and background corrected asymmetry "<<A_corrected<<std::endl;

  Double_t denom = (1-fb1-fb2);
  std::cout<<"(1-f1-f2) ="<<denom<<std::endl;

  Double_t partial_P = (A_p*dP_beam*RQ)/(P_beam*P_beam*denom);
  std::cout<<"partial_P dP = "<<partial_P<<std::endl;

  Double_t partial_A_p_stat = (dA_p_stat*RQ)/(P_beam*denom);
  std::cout<<"partial_A_p_stat dA_p_stat = "<<partial_A_p_stat<<std::endl;

  Double_t partial_A_p_sys1 = (dA_p_sys1*RQ)/(P_beam*denom);
  std::cout<<"partial_A_p_sys1 dA_p_sys1 = "<<partial_A_p_sys1<<std::endl;
  
  Double_t partial_A_p_sys2 = (dA_p_sys2*RQ)/(P_beam*denom);
  std::cout<<"partial_A_p_sys2 dA_p_sys2 = "<<partial_A_p_sys2<<std::endl;
  
  Double_t partial_A_p_sys3 = (dA_p_sys3*RQ)/(P_beam*denom);
  std::cout<<"partial_A_p_sys3 dA_p_sys3 = "<<partial_A_p_sys3<<std::endl;

  Double_t partial_A_b1 = (-fb1*dA_b1*RQ)/(denom);
  std::cout<<"partial_A_b1 dAb1 = "<<partial_A_b1<<std::endl;

  Double_t partial_A_b2 = (-fb2*dA_b2*RQ)/(denom);
  std::cout<<"partial_A_b2 dAb2 = "<<partial_A_b2<<std::endl;

  Double_t partial_fb1 = (-A_b1*(1-fb2)*RQ*dfb1)/(denom*denom);
  std::cout<<"partial_fb1 dfb1 = "<<partial_fb1<<std::endl;

  Double_t partial_fb2 = (-A_b2*(1-fb1)*RQ*dfb2)/(denom*denom);
  std::cout<<"partial_fb2 dfb2 = "<<partial_fb2<<std::endl;

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
  std::cout<<"Statistics = "<< (partial_A_p_stat/A_corrected)*100<<std::endl;
  std::cout<<"Acceptance average = "<< (partial_A_p_sys1/A_corrected)*100<<std::endl;
  std::cout<<"Scheme dependance = "<< (partial_A_p_sys2/A_corrected)*100<<std::endl;
  std::cout<<"Non-linearity = "<< (partial_A_p_sys3/A_corrected)*100<<std::endl;
  std::cout<<"Polarization = "<< (partial_P/A_corrected)*100<<std::endl;
  std::cout<<"Q2 bias = "<< (partial_RQ/A_corrected)*100<<std::endl;
  std::cout<<"Aluminum = "<< (partial_A_b1/A_corrected)*100<<std::endl;
  std::cout<<"Aluminum dilution = "<< (partial_fb1/A_corrected)*100<<std::endl;
  std::cout<<"Inelastic = "<< (partial_A_b2/A_corrected)*100<<std::endl;
  std::cout<<"Inelastic dilution= "<< (partial_fb2/A_corrected)*100<<std::endl;


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

}

