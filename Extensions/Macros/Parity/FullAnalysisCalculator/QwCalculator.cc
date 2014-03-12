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
  BlindingFactor = -0.0005079 ; //ppm 
 
  //Detector Bias Correction (Q^2 light wieghting at the detector)
  DetCorr = 0.987;
  dDetCorr = 0.5*TMath::Abs(1.-DetCorr);//50% relative uncertainty for now

  //EM radiative correction factor
  EM_Rad_C = 1.0100;
  dEM_Rad_C = 0.5*TMath::Abs(1.-EM_Rad_C);//50% relative uncertainty for now

  //acceptance correction
  AccCorr = 0.980;
  dAccCorr = 0.5*TMath::Abs(1.- AccCorr);//50% relative uncertainty for now
  //experimental bias correction
  Exp_Bias_C = DetCorr * EM_Rad_C * AccCorr;
  dExp_Bias_C = TMath::Sqrt( TMath::Power(dDetCorr,2) + TMath::Power(dEM_Rad_C,2) );

  //Additional transverse corrections
  A_msr_trans   =  0.0; //ppm
  dA_msr_trans  =  0.00;//ppm

}

////////////////////////
// Set time dependant corrections
///////////////////////
void QwCalculator::SetTimeDependentQtys(QwReadFile &input){

  // Asign the asymmetry, polarization and background corrections
  //
  A_raw        = input.GetNextToken(","); //ppm 
  dA_raw_stat  = input.GetNextToken(","); //ppm
  A_msr        = input.GetNextToken(","); //ppm
  dAmsr_stat  = input.GetNextToken(","); //ppm
  dAmsr_reg   = input.GetNextToken(","); //ppm
  dAmsr_dpk   = input.GetNextToken(","); //ppm
  dAmsr_nonlin= TMath::Abs((input.GetNextToken(","))*A_msr); //ppm
  dAmsr_cuts  = input.GetNextToken(","); //ppm
  dAmsr_trans = input.GetNextToken(","); //ppm
  dAmsr_inpout =  0.0; //ppm

  Delta_A_reg   = A_msr - A_raw; // regression correction ppm
  dDelata_A_reg = TMath::Sqrt(TMath::Power(dAmsr_reg,2) + TMath::Power(dAmsr_cuts,2) + TMath::Power(dAmsr_dpk,2));

  //inflate the stat error to accomodate large inpout result
  dAmsr_stat    = TMath::Sqrt( TMath::Power(dAmsr_stat,2) +  TMath::Power(dAmsr_inpout,2));
  dAmsr_syst    = TMath::Sqrt( TMath::Power(dAmsr_reg,2) + TMath::Power(dAmsr_dpk,2) + TMath::Power(dAmsr_nonlin,2) + TMath::Power(dAmsr_cuts,2) + TMath::Power(dAmsr_trans,2) );// ppm (regression+dpk+nonlin.+cuts+transverse)

  //Polarization
  P           = input.GetNextToken(",");
  dP          = input.GetNextToken(",");

  //b1: Bkg 1 Al windows
  Ab[0]       = input.GetNextToken(","); //ppm
  dAb[0]      = input.GetNextToken(","); //ppm
  fb[0]       = input.GetNextToken(",");
  dfb[0]      = input.GetNextToken(",");

  //b2: QTOR transport channel neutrals
  Ab[1]       = input.GetNextToken(","); //ppm
  dAb[1]      = input.GetNextToken(","); //ppm 
  fb[1]       = input.GetNextToken(","); 
  dfb[1]      = input.GetNextToken(","); 

  //b3: Beamline bkg neutrals with W shutters installed
  Ab[2]       = input.GetNextToken(","); //ppm 
  dAb[2]      = input.GetNextToken(","); //ppm
  fb[2]       = input.GetNextToken(","); 
  dfb[2]      = input.GetNextToken(","); 

  //b4: Non-signal electrons on detector
  Ab[3]       = input.GetNextToken(","); //ppm  
  dAb[3]      = input.GetNextToken(","); //ppm 
  fb[3]       = input.GetNextToken(",");  
  dfb[3]      = input.GetNextToken(","); 

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
  
  dA_signal_stat=0;//final asymmetry systematic error
  dA_signal_syst=0;
  pdA_msr_stat = Exp_Bias_C * 1/P * 1/(1 - f_total) * dAmsr_stat;//measured asymmetry partial stat error on final asymmetry
  //Final statistic error
  dA_signal_stat = pdA_msr_stat;


  pdA_msr_syst = Exp_Bias_C * 1/P * 1/(1 - f_total) * dAmsr_syst;//measured asymmetry partial syst error on final asymmetry

  dA_signal_syst += TMath::Power(pdA_msr_syst,2);//add all the syst. error contributions quadratically

  pdA_msr      = TMath::Sqrt( TMath::Power(pdA_msr_stat,2) +  TMath::Power(pdA_msr_syst,2));//measured asymmetry partial error on final asymmetry

  pdP = Exp_Bias_C * -1 * A_msr * 1/(1 - f_total) * 1/TMath::Power(P,2) * dP;//polarization partial error on final asymmetry

  dA_signal_syst += TMath::Power(pdP,2);//add all the syst. error contributions quadratically
  
  //to get A_signal
  A_signal = A_msr/ (P * (1 - f_total));
  
  for (Int_t i=0;i<bkg_count;i++)
    A_signal -= Ab[i]*fb[i]/(1 - f_total);
  
  pdDetCorr = TMath::Abs(dDetCorr * AccCorr * EM_Rad_C * A_signal);
  pdEM_Rad_C = TMath::Abs(dEM_Rad_C * AccCorr * DetCorr * A_signal);
  pdAcc_C = TMath::Abs(dAccCorr * EM_Rad_C * DetCorr * A_signal);
  pdExp_Bias_C = TMath::Sqrt( TMath::Power(pdDetCorr,2) + TMath::Power(pdEM_Rad_C,2) + TMath::Power(pdAcc_C,2));//Total partial error on exp. bias correction

  dA_signal_syst += TMath::Power(pdDetCorr,2) + TMath::Power(pdEM_Rad_C,2) + TMath::Power(pdAcc_C,2);

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
 
  asymmetry_correction_P = Exp_Bias_C * (A_msr/P -  A_msr)/(1 - f_total);//effective polarization correction on A_msr ppm

  asymmetry_total_correction = asymmetry_correction_P;//Add to the total Correction
  dA_asymmetry_correction += TMath::Power(pdP,2);//Polarization Correction error added quadratically
  for (Int_t i=0;i<bkg_count;i++){
    asymmetry_correction_b[i] = Exp_Bias_C * -1 * Ab[i]*fb[i]/(1 - f_total);
    asymmetry_total_correction += asymmetry_correction_b[i];
    dA_asymmetry_correction += TMath::Power(pdb[i],2);//add the error contributions quadratically
  }

  asymmetry_correction_Exp_Bias_C = Exp_Bias_C * A_msr/(1 - f_total) - A_msr;//effective Exp. bias correction  on A_msr ppm
  asymmetry_total_correction += asymmetry_correction_Exp_Bias_C;
  dA_asymmetry_correction += TMath::Power(pdExp_Bias_C,2);

  dA_total_asymmetry_correction=TMath::Sqrt(dA_asymmetry_correction);

  /*
    A_msr        = -0.2046 +/- 0.0305 (stat) +/- 0.0129 (syst)  ppm
    Total Correction to A_msr = -0.0771 +/- 0.0256 ppm
    A_msr + Total Correction to A_msr = -0.2817 ppm
    A_signal(<Q^2>) =-0.2812 +/- 0.0351 (stat) +/- 0.0296 (syst) ppm 
    The difference is the blinder (-0.2817 - (-0.2812) = 0.0005 ppm)
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
  printf("A_raw                             = %+6.4f +/- %6.4f (stat) ppm\n",A_raw,dA_raw_stat);
  printf("Reg correction Delta_A_reg        = %+6.4f +/- %6.4f (stat) ppm\n",Delta_A_reg,dDelata_A_reg);
  printf("-----------------------------------\n");
  printf("dAmsr_stat   = %6.4f ppm\n",dAmsr_stat);
  printf("dAmsr_inpout = %6.4f ppm\n",dAmsr_inpout);
  printf("dAmsr_reg    = %6.4f ppm\n",dAmsr_reg);
  printf("dAmsr_dpk    = %6.4f ppm\n",dAmsr_dpk);
  printf("dAmsr_nonlin = %6.4f ppm\n",dAmsr_nonlin);
  printf("dAmsr_cuts   = %6.4f ppm (updated results!!! \n",dAmsr_cuts);
  printf("dAmsr_trans  = %6.4f ppm\n",dAmsr_trans);
  printf("dAmsr_syst   = %6.4f  ppm (reg-scheme+dpk+nonlin.+cuts+transverse) \n",dAmsr_syst);
  printf("A_msr        = %6.4f +/- %6.4f (stat) +/- %6.4f (syst)  ppm\n",A_msr,dAmsr_stat,dAmsr_syst);

  printf(" \n Beam Polarization and Error \n");
  printf("P = %3.4f +/- %3.4f\n",P,dP);

  printf("\n Detector Bias Correction and Error \n");
  printf("DetCorr = %3.4f +/- %3.4f \n",DetCorr,dDetCorr);

  printf("\n EM radiative correction factor and error \n");
  printf("EM_Rad_C = %3.4f +/- %3.4f \n",EM_Rad_C,dEM_Rad_C);

  printf("\n Acceptance correction factor \n");
  printf("AccCorr = %3.4f +/- %3.4f \n",AccCorr,dAccCorr);

  printf("\n Experimenta Bias Corrections: EM radiative Correction, light weighting, acceptance correction and Error \n");
  printf("Exp_Bias_C = %3.4f +/- %3.4f \n",Exp_Bias_C,dExp_Bias_C);

  printf(" \n Background Asymmetries and Dilutions \n");
  for (Int_t i=0;i<bkg_count;i++){
    printf("%s \n",bkg_type[i]);
    printf("                  Ab[%i] = %6.4f +/- %6.4f (ppm) fb[%i] = %6.4f +/- %6.4f \n",i,Ab[i],dAb[i],i,fb[i],dfb[i]);
  }
  printf(" \n Total background dilution f_T = %6.4f \n",f_total);

}


void QwCalculator::PrintErrorContributions(){

  printf("\n***************************************************************************\n");
  printf("Error contributions to the final corrected asymmetry \n");
  printf("***************************************************************************\n");
  printf("%20s = %3.4f ppm\n","dA_msr_stat",dA_signal_stat);  
  printf("%20s = %3.4f ppm \n","dA_msr_syst",pdA_msr_syst); 
  printf("%20s = %3.4f ppm  \n","pdP",pdP); 
  printf("%20s = %3.4f ppm  \n","pdDetCorr",pdDetCorr); 
  printf("%20s = %3.4f ppm  \n","pdEM_Rad_C",pdEM_Rad_C);
  printf("%20s = %3.4f ppm  \n","pdAcc_C",pdAcc_C);

  for (Int_t i=0,j=1,k=2,l=3;i<bkg_count;i++,j=(i+1)%4,k=(i+2)%4,l=(i+3)%4){
    printf("\n---------------------------------");
    printf("\n%s\n",bkg_type[i]);
    printf("---------------------------------\n");
    printf("%10s pdAb[%i] = %3.4f ppm  \n","",i+1,TMath::Abs(pdAb[i])); 
    printf("%10s pdfb[%i] = %3.4f ppm  \n","",i+1,TMath::Abs(pdfb[i])); 
  }

}

void QwCalculator::PrintCorrectionBreakdown(){

  printf("\n\n\n***************************************************************************\n");
  printf("Breakdown of Corrections\n");
  printf("***************************************************************************\n");
  printf("%50s = %+6.4f +/- %6.4f ppm \n","Effective Polarization Correction",asymmetry_correction_P,pdP);
  for (Int_t i=0;i<bkg_count;i++){
    printf("%50s = %+6.4f +/- %6.4f ppm \n",bkg_type[i],asymmetry_correction_b[i],pdb[i]);
  }
  printf("%50s = %+6.4f +/- %6.4f ppm \n","Exp. bias  Corrections",asymmetry_correction_Exp_Bias_C,pdExp_Bias_C);
  printf("\t\t-----------------------------------------------------------\n");
  printf("%50s = %+6.4f +/- %6.4f ppm \n","Total Correction to A_msr",asymmetry_total_correction,dA_total_asymmetry_correction);
  printf("\t\t-----------------------------------------------------------\n");
}

void QwCalculator::PrintAcceltanceCorrectedAsym(){

  printf("\n***************************************************************************\n");
  printf("Final Acceptance  Corrected Asymmetry \n");
 printf("***************************************************************************\n");

  printf("A_signal(<Q^2>) =%6.4f +/- %6.4f (stat) +/- %6.4f (syst) ppm \n",A_signal,dA_signal_stat,dA_signal_syst);
  printf("A_signal(<Q^2>) =%6.4f +/- %6.4f ppm \n",A_signal,TMath::Sqrt(TMath::Power(dA_signal_stat,2)+TMath::Power(dA_signal_syst,2)));
  printf("Blinding correction was subtracted  %f ppm. \n",BlindingSign*BlindingFactor);


}
