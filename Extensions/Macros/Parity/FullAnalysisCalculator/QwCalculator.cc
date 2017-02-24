/*

03-04-2014
Author Buddhini Waidyawansa (buddhini@jlab.org)
Coppied from compute_LH2_asym.cc to construct the class QwComputePhysAsym() required for the full analysis calculator.

*/

#include "QwCalculator.h"
#include "QwReadWrite.cc"

using namespace std;

const Int_t bkg_count = 4;
const Char_t *bkg_type[bkg_count] = {"Aluminum alloy windows","QTOR transport channel neutrals","Beamline bkg neutrals with W shutters installed","Non-signal electrons on detector"};

QwCalculator::QwCalculator(){
  
  // initialize output variables
  A_signal = -9999;
  dA_signal = -9999;
  dA_signal_stat = -9999;
  dA_signal_syst = -9999;

}

////////////////////////
// Set time independant corrections
///////////////////////
void QwCalculator::SetTimeIndependentQtys(){

  //Blinder
  BlindingSign   = +1.; //
  BlindingFactor = 0.0 ; //ppb 

  //Additional transverse corrections
  //  A_msr_trans   =  0.0; //ppbuiop
  //  dA_msr_trans  =  0.00;//ppb

  A_BCM_norm = 0.0;
  Amsr_trans = 0.0;

}

////////////////////////
// Set time dependant corrections
///////////////////////
void QwCalculator::SetTimeDependentQtys(QwReadFile &input){

  // Asign the asymmetry, polarization and background corrections
  //
  A_raw         = input.GetNextToken(","); //ppb
  dA_raw_stat   = input.GetNextToken(","); //ppb
  dAmsr_stat    = input.GetNextToken(","); //ppb
  dA_BCM_norm   = input.GetNextToken(","); //ppb
  A_beam        = input.GetNextToken(","); //ppb
  dA_beam_schem = input.GetNextToken(","); //ppb
  dA_beam_sens  = input.GetNextToken(","); //ppb
  A_BB    	= input.GetNextToken(","); //ppb
  dA_BB    	= input.GetNextToken(","); //ppb
  A_nonlin    	= input.GetNextToken(","); //ppb
  dA_nonlin    	= input.GetNextToken(","); //ppb
  dAmsr_trans   = input.GetNextToken(","); //ppb
  A_bias    	= input.GetNextToken(","); //ppb
  dA_bias    	= input.GetNextToken(","); //ppb


  Delta_A_reg   = A_BCM_norm + A_beam;
  dDelata_A_reg = TMath::Sqrt(TMath::Power(dA_BCM_norm,2)+TMath::Power(dA_beam_schem,2)+TMath::Power(dA_beam_sens,2));

  A_msr         = A_raw + Delta_A_reg + A_BB + A_nonlin + Amsr_trans - A_bias;


  dAmsr_syst    = TMath::Sqrt(TMath::Power(dA_BCM_norm,2)+TMath::Power(dA_beam_schem,2)+TMath::Power(dA_beam_sens,2)+TMath::Power(dA_BB,2)+TMath::Power(dA_nonlin,2)+TMath::Power(dAmsr_trans,2)+TMath::Power(dA_bias,2));

  //Polarization
  P           = input.GetNextToken(","); // Percent 
  dP          = input.GetNextToken(","); // Percent
  //  Change polarization from percent to to fractional values
  P  /= 100.;
  dP /= 100.;

  //b1: Bkg 1 Al windows
  Ab[0]       = input.GetNextToken(","); //ppb
  dAb[0]      = input.GetNextToken(","); //ppb
  fb[0]       = input.GetNextToken(",");
  dfb[0]      = input.GetNextToken(",");

  //b2: QTOR transport channel neutrals
  //  The A_b2 correctionis now handled by the A_BB correction
  Ab[1]       = 0.0;
  dAb[1]      = 0.0;
  fb[1]       = input.GetNextToken(","); 
  dfb[1]      = input.GetNextToken(","); 

  //b3: Beamline bkg neutrals with W shutters installed
  Ab[2]       = input.GetNextToken(","); //ppb 
  dAb[2]      = input.GetNextToken(","); //ppb
  fb[2]       = input.GetNextToken(","); 
  dfb[2]      = input.GetNextToken(","); 

  //b4: Non-signal electrons on detector
  Ab[3]       = input.GetNextToken(","); //ppb  
  dAb[3]      = input.GetNextToken(","); //ppb 
  fb[3]       = input.GetNextToken(",");  
  dfb[3]      = input.GetNextToken(","); 



  //Detector Bias Correction (Q^2 light wieghting at the detector)
  DetCorr    = input.GetNextToken(",");
  dDetCorr   = input.GetNextToken(",");

  //EM radiative correction factor
  EM_Rad_C   = input.GetNextToken(",");
  dEM_Rad_C  = input.GetNextToken(",");

  //acceptance correction
  AccCorr    = input.GetNextToken(",");
  dAccCorr   = input.GetNextToken(",");


  R_q2     = input.GetNextToken(",");
  dR_q2    = input.GetNextToken(",");

  //experimental bias correction
  Exp_Bias_C = DetCorr * EM_Rad_C * AccCorr * R_q2;
  dExp_Bias_C = TMath::Sqrt( TMath::Power(dDetCorr,2) 
			     + TMath::Power(dEM_Rad_C,2)
			     + TMath::Power(dAccCorr,2)
			     + TMath::Power(dR_q2,2) );

  //compute the total background dilution
  f_total=0;
  for (Int_t i=0;i<bkg_count;i++)
    f_total+=fb[i];
}

////////////////////////
// Compute physics asymmetry
///////////////////////
int QwCalculator::ComputePhysAsym()
{
  
  //Start computing final LH2 asymmetry
  UnblindMeasuredAsymmetry();
  ComputeSignalAsymmetryPartialErrors();
  ComputeSignalAsymmetry();
  ComputeMeasuredAsymmetryCorrections();
  
  return(1);
}

////////////////////////
// Compute physics asymmetry
///////////////////////
void QwCalculator::GetCorrectedAsymmetry(float *A, float *dA_stat, float*dA_sys){

  *A = A_signal;
  *dA_stat = dA_signal_stat;
  *dA_sys = dA_signal_syst;
}


////////////////////////
// Apply the blinding factor to the measured asymmetry
///////////////////////
void QwCalculator::UnblindMeasuredAsymmetry(){

  A_msr -= BlindingSign*BlindingFactor;

}

////////////////////////
// Compute the partial error contributions from A_msr, polarization and backgrounds
///////////////////////
void QwCalculator::ComputeSignalAsymmetryPartialErrors(){

  syst_errors.clear();
  
  dA_signal_stat=0;//final asymmetry systematic error
  dA_signal_syst=0;
  pdA_msr_stat = Exp_Bias_C * 1/P * 1/(1 - f_total) * dAmsr_stat;//measured asymmetry partial stat error on final asymmetry
  //Final statistic error
  dA_signal_stat = pdA_msr_stat;


  pdA_msr_syst = Exp_Bias_C * 1/P * 1/(1 - f_total) * dAmsr_syst;//measured asymmetry partial syst error on final asymmetry

  dA_signal_syst += TMath::Power(pdA_msr_syst,2);//add all the syst. error contributions quadratically
  syst_errors.push_back(pdA_msr_syst);

  pdA_msr      = TMath::Sqrt( TMath::Power(pdA_msr_stat,2) +  TMath::Power(pdA_msr_syst,2));//measured asymmetry partial error on final asymmetry

  pdP = Exp_Bias_C * -1 * A_msr * 1/(1 - f_total) * 1/TMath::Power(P,2) * dP;//polarization partial error on final asymmetry

  dA_signal_syst += TMath::Power(pdP,2);//add all the syst. error contributions quadratically
  syst_errors.push_back(pdP);
  
  //to get A_signal
  A_signal = A_msr/ (P * (1 - f_total));
  
  for (Int_t i=0;i<bkg_count;i++){
    if (i==1) continue;
    A_signal -= Ab[i]*fb[i]/(1 - f_total);
  }
  
  pdDetCorr  = TMath::Abs(dDetCorr * AccCorr * EM_Rad_C * R_q2 * A_signal);
  syst_errors.push_back(pdDetCorr);
  pdEM_Rad_C = TMath::Abs(dEM_Rad_C * AccCorr * DetCorr * R_q2 * A_signal);
  syst_errors.push_back(pdEM_Rad_C);
  pdAcc_C    = TMath::Abs(dAccCorr * EM_Rad_C * DetCorr * R_q2 * A_signal);
  syst_errors.push_back(pdAcc_C);
  pdR_q2     = TMath::Abs(dR_q2 * EM_Rad_C * DetCorr * AccCorr * A_signal);
  syst_errors.push_back(pdR_q2);

  pdExp_Bias_C = TMath::Sqrt( TMath::Power(pdDetCorr,2) + TMath::Power(pdEM_Rad_C,2) + TMath::Power(pdAcc_C,2)  + TMath::Power(pdR_q2,2));//Total partial error on exp. bias correction

  dA_signal_syst += TMath::Power(pdDetCorr,2) + TMath::Power(pdEM_Rad_C,2) + TMath::Power(pdAcc_C,2) +  TMath::Power(pdR_q2,2);

  for (Int_t i=0,j=1,k=2,l=3;i<bkg_count;i++,j=(i+1)%4,k=(i+2)%4,l=(i+3)%4){
    //printf("%i %i %i %i \n",i,j,k,l);//checking permutations for 4 backgrounds
    /* permutations for 4 backgrounds
      i j k l
      -------
      0 1 2 3
      1 2 3 0
      2 3 0 1
      3 0 1 2
     */
    pdAb[i] = Exp_Bias_C * fb[i]/(1 - f_total) * dAb[i]; //bkg asymmetry partial error on final asymmetry
    pdfb[i] = Exp_Bias_C * ( A_msr/P + ( -1 +  fb[j] +  fb[k] + fb[l] )*Ab[i] - Ab[j]*fb[j] - Ab[k]*fb[k] - Ab[l]*fb[l]) * dfb[i] * 1/TMath::Power(( 1 - f_total),2);//bkg dilution partial error on final asymmetry
    syst_errors.push_back(pdAb[i]);
    syst_errors.push_back(pdfb[i]);

    //compute the partial error for each background = sqrt(pdAb[i]^2 + pdfb[i]^2)
    pdb[i]=TMath::Sqrt( TMath::Power(pdAb[i],2) + TMath::Power(pdfb[i],2) );//bkg contribution (asymmetry+dilution) error on final asymmetry
    //printf("pdb[%i] = %6.4f \n",i,pdb[i]); printed for debug/double check partial bkg errors

    //background errors are added to systematic error contribution
    dA_signal_syst += TMath::Power(pdb[i],2);//add all the syst. error contributions quadratically
  }

  //Final systematic error
  dA_signal_syst = TMath::Sqrt(dA_signal_syst);
}

////////////////////////
// Compute the final corrected asymmetry
///////////////////////
void QwCalculator::ComputeSignalAsymmetry(){ 
  A_signal = A_msr/ (P * (1 - f_total));

  for (Int_t i=0;i<bkg_count;i++)
    A_signal -= Ab[i]*fb[i]/(1 - f_total);
  
  //Detector Bias + EM radiative Corrections
  A_signal = Exp_Bias_C * A_signal;

};


////////////////////////
// Compute the breakdown of corrections done to the A_msr by polarization effect and background dilutions 
///////////////////////
void QwCalculator::ComputeMeasuredAsymmetryCorrections(){
 
  dA_asymmetry_correction=0;
 
  asymmetry_correction_P = Exp_Bias_C * (A_msr/P -  A_msr)/(1 - f_total);//effective polarization correction on A_msr ppb

  asymmetry_total_correction = asymmetry_correction_P;//Add to the total Correction
  dA_asymmetry_correction += TMath::Power(pdP,2);//Polarization Correction error added quadratically
  for (Int_t i=0;i<bkg_count;i++){
    asymmetry_correction_b[i] = Exp_Bias_C * -1 * Ab[i]*fb[i]/(1 - f_total);
    asymmetry_total_correction += asymmetry_correction_b[i];
    dA_asymmetry_correction += TMath::Power(pdb[i],2);//add the error contributions quadratically
  }

  asymmetry_correction_Exp_Bias_C = Exp_Bias_C * A_msr/(1 - f_total) - A_msr;//effective Exp. bias correction  on A_msr ppb
  asymmetry_total_correction += asymmetry_correction_Exp_Bias_C;
  dA_asymmetry_correction += TMath::Power(pdExp_Bias_C,2);

  dA_total_asymmetry_correction=TMath::Sqrt(dA_asymmetry_correction);

  /*
    A_msr        = -0.2046 +/- 0.0305 (stat) +/- 0.0129 (syst)  ppb
    Total Correction to A_msr = -0.0771 +/- 0.0256 ppb
    A_msr + Total Correction to A_msr = -0.2817 ppb
    A_signal(<Q^2>) =-0.2812 +/- 0.0351 (stat) +/- 0.0296 (syst) ppb 
    The difference is the blinder (-0.2817 - (-0.2812) = 0.0005 ppb)
   */
};


///////////////////////
//Print functions
//////////////////////

void QwCalculator::PrintInputs(){
  printf("\n***************************************************************************");
  printf("\n***************************************************************************\n");
  printf("Input list\n");
  printf("***************************************************************************\n");
  printf(" \n Blinded Measured Asymmetry and Errors \n");
  printf("A_raw                             = %+6.4f +/- %6.4f (stat) ppb\n",A_raw,dA_raw_stat);
  printf("Reg correction Delta_A_reg        = %+6.4f +/- %6.4f (syst) ppb\n",Delta_A_reg,dDelata_A_reg);
  printf("A_dthr                            = %+6.4f +/- %6.4f (stat)  +/- %6.4f (syst) ppb\n",A_raw+Delta_A_reg,dAmsr_stat, dDelata_A_reg);
  printf("-----------------------------------\n");
  printf("dA_BCM_norm = %6.4f ppb\n",dA_BCM_norm);
  printf("A_beam = %6.4f ppb\n",A_beam);
  printf("dA_beam_schem = %6.4f ppb\n",dA_beam_schem);
  printf("dA_beam_sens = %6.4f ppb\n",dA_beam_sens);

  printf("\nA_BB = %6.4f ppb\n",A_BB);
  printf("dA_BB = %6.4f ppb\n",dA_BB);

  printf("A_nonlin = %6.4f ppb\n",A_nonlin);
  printf("dA_nonlin = %6.4f ppb\n",dA_nonlin);

  printf("A_bias = %6.4f ppb\n",A_bias);
  printf("dA_bias = %6.4f ppb\n",dA_bias);

  //  printf("dAmsr_inpout = %6.4f ppb\n",dAmsr_inpout);
  //  printf("dAmsr_reg    = %6.4f ppb\n",dAmsr_reg);
  //  printf("dAmsr_dpk    = %6.4f ppb\n",dAmsr_dpk);
  //  printf("dAmsr_nonlin = %6.4f ppb\n",dAmsr_nonlin);
  //  printf("dAmsr_cuts   = %6.4f ppb (updated results!!! \n",dAmsr_cuts);
  printf("dAmsr_trans  = %6.4f ppb\n",dAmsr_trans);
  printf("-----------------------------------\n");
  printf("A_msr        = %6.4f +/- %6.4f (stat) +/- %6.4f (syst)  ppb\n",A_msr,dAmsr_stat,dAmsr_syst);

  printf(" \n Beam Polarization and Error \n");
  printf("P = %3.4f +/- %3.4f\n",P,dP);

  printf("\n Detector Bias Correction and Error \n");
  printf("DetCorr = %3.4f +/- %3.4f \n",DetCorr,dDetCorr);

  printf("\n EM radiative correction factor and error \n");
  printf("EM_Rad_C = %3.4f +/- %3.4f \n",EM_Rad_C,dEM_Rad_C);

  printf("\n Acceptance correction factor \n");
  printf("AccCorr = %3.4f +/- %3.4f \n",AccCorr,dAccCorr);

  printf("\n Q2 correction factor \n");
  printf("R_q2 =  %3.4f +/- %3.4f\n",R_q2,dR_q2);

  printf("\n Experimenta Bias Corrections: EM radiative Correction, light weighting, acceptance correction and Error \n");
  printf("Exp_Bias_C = %3.4f +/- %3.4f \n",Exp_Bias_C,dExp_Bias_C);

  printf(" \n Background Asymmetries and Dilutions \n");
  for (Int_t i=0;i<bkg_count;i++){
    printf("%s \n",bkg_type[i]);
    printf("                  Ab[%i] = %6.4f +/- %6.4f (ppb) fb[%i] = %6.4f +/- %6.4f \n",i,Ab[i],dAb[i],i,fb[i],dfb[i]);
  }
  printf(" \n Total background dilution f_T = %6.4f \n",f_total);

}


void QwCalculator::PrintErrorContributions(){

  printf("\n***************************************************************************\n");
  printf("Error contributions to the final corrected asymmetry \n");
  printf("***************************************************************************\n");
  printf("%20s = %3.4f ppb\n","dA_msr_stat",dA_signal_stat);  
  printf("%20s = %3.4f ppb \n","dA_msr_syst",pdA_msr_syst); 
  printf("%20s = %3.4f ppb  \n","pdP",pdP); 
  printf("%20s = %3.4f ppb  \n","pdDetCorr",pdDetCorr); 
  printf("%20s = %3.4f ppb  \n","pdEM_Rad_C",pdEM_Rad_C);
  printf("%20s = %3.4f ppb  \n","pdAcc_C",pdAcc_C);

  for (Int_t i=0,j=1,k=2,l=3;i<bkg_count;i++,j=(i+1)%4,k=(i+2)%4,l=(i+3)%4){
    printf("\n---------------------------------");
    printf("\n%s\n",bkg_type[i]);
    printf("---------------------------------\n");
    printf("%10s pdAb[%i] = %3.4f ppb  \n","",i+1,TMath::Abs(pdAb[i])); 
    printf("%10s pdfb[%i] = %3.4f ppb  \n","",i+1,TMath::Abs(pdfb[i])); 
  }

}

void QwCalculator::PrintCorrectionBreakdown(){

  printf("\n\n\n***************************************************************************\n");
  printf("Breakdown of Corrections\n");
  printf("***************************************************************************\n");
  printf("%50s = %+6.4f +/- %6.4f ppb \n","Effective Polarization Correction",asymmetry_correction_P,pdP);
  for (Int_t i=0;i<bkg_count;i++){
    printf("%50s = %+6.4f +/- %6.4f ppb \n",bkg_type[i],asymmetry_correction_b[i],pdb[i]);
  }
  printf("%50s = %+6.4f +/- %6.4f ppb \n","Exp. bias  Corrections",asymmetry_correction_Exp_Bias_C,pdExp_Bias_C);
  printf("\t\t-----------------------------------------------------------\n");
  printf("%50s = %+6.4f +/- %6.4f ppb \n","Total Correction to A_msr",asymmetry_total_correction,dA_total_asymmetry_correction);
  printf("\t\t-----------------------------------------------------------\n");
}

void QwCalculator::PrintAcceltanceCorrectedAsym(){

  printf("\n***************************************************************************\n");
  printf("Final Acceptance  Corrected Asymmetry \n");
 printf("***************************************************************************\n");

  printf("A_signal(<Q^2>) =%6.4f +/- %6.4f (stat) +/- %6.4f (syst) ppb \n",A_signal,dA_signal_stat,dA_signal_syst);
  printf("A_signal(<Q^2>) =%6.4f +/- %6.4f ppb \n",A_signal,TMath::Sqrt(TMath::Power(dA_signal_stat,2)+TMath::Power(dA_signal_syst,2)));
  printf("Blinding correction was subtracted  %f ppb. \n",BlindingSign*BlindingFactor);


}
