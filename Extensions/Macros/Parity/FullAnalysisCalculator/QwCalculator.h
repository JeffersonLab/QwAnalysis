
#ifndef QWCALCULATOR_H
#define QWCALCULATOR_H

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

#include "QwReadWrite.h"

using namespace std;

const Int_t bkg_count = 5;

class QwCalculator{

 public:
  QwCalculator();
  ~QwCalculator(){};

  void SetTimeDependentQtys(QwReadFile &input);
  void SetTimeIndependentQtys();
  int  ComputePhysAsym();
  void UnblindMeasuredAsymmetry();
  void ComputeSignalAsymmetryPartialErrors();
  void ComputeSignalAsymmetry();
  void ComputeMeasuredAsymmetryCorrections();
  void GetCorrectedAsymmetry(float *A, float *dA_stat, float*dA_sys);
  void PrintInputs();
  void PrintErrorContributions();
  void PrintCorrectionBreakdown();
  void PrintAcceltanceCorrectedAsym();

  std::vector<Double_t> GetSystErrorList(){
    return(syst_errors);
  }

  std::vector<Double_t> syst_errors;

  //Measured Asymmetry inputs
  Double_t A_raw; // unregressed asymmetry ppm 
  Double_t dA_raw_stat; // stat. error ppm

  Double_t Delta_A_reg; // regression correction ppm 
  Double_t dDelata_A_reg; // quadrature sum of dAmsr_reg,dAmsr_dpk and dAmsr_cuts  ppm
  
  Double_t A_msr; // ppm    
  Double_t dAmsr_stat; // ppm

  //  Double_t dAmsr_reg ; // ppm scheme dependance or "why-does-it-change-when-we-add-charge" and/or "whydoesitchangewhenweusetheTgtBPMvsapairofBPM" error
  //  Double_t dAmsr_nonlin; // ppm
  //  Double_t dAmsr_cuts;  // ppm
  //  Double_t dAmsr_dpk;//ppm double peaking error on the regression slopes e-log 772

  Double_t  A_BCM_norm;
  Double_t  dA_BCM_norm;
  Double_t  A_beam;
  Double_t  dA_beam_schem;
  Double_t  dA_beam_sens;
  Double_t  A_BB;
  Double_t  dA_BB;
  Double_t  A_nonlin;
  Double_t  dA_nonlin;
  Double_t  A_bias;
  Double_t  dA_bias;

  Double_t Amsr_trans;  // ppm
  Double_t dAmsr_trans; // ppm

  Double_t dAmsr_syst ; // ppm (regression+nonlin.+cuts+transverse+dpk)
  
  //Blinder
  Double_t BlindingSign;
  Double_t BlindingFactor;
  
  //Additional transverse corrections
  //  Double_t A_msr_trans; //ppm
  //  Double_t dA_msr_trans;//ppm
  
  //Polarization
  Double_t P; 
  Double_t dP;
  
  //Detector Bias Correction (Q^2 light wieghting at the detector) */
  Double_t DetCorr;
  Double_t dDetCorr;
  
  //EM radiative correction factor
  Double_t EM_Rad_C;
  Double_t dEM_Rad_C;
  
  //acceptance correction
  Double_t AccCorr;
  Double_t dAccCorr;
  
  Double_t  R_q2;
  Double_t  dR_q2;

  //EM+Det_bias+acceptance correction
  Double_t Exp_Bias_C;
  Double_t dExp_Bias_C;

  //Bacground Asymmetries and dilutions

  Double_t Ab[bkg_count];//bkg asymmetry
  Double_t dAb[bkg_count];//bkg asymmetry error
  Double_t fb[bkg_count];//bkg dilution
  Double_t dfb[bkg_count];//bkg dilution error
  Double_t f_total;//total background dilution

    //Final output parameters
  Double_t A_signal; //final corrected asymmetry ppm
  Double_t dA_signal;//final corrected asymmetry error ppm
  Double_t dA_signal_stat;//final corrected asymmetry statistical error ppm
  Double_t dA_signal_syst;//final corrected asymmetry systematic error ppm


  //Intermediate output parameters
  Double_t pdA_msr;//measured asymmetry partial error on final asymmetry
  Double_t pdA_msr_stat;//measured asymmetry partial stat error on final asymmetry
  Double_t pdA_msr_syst;//measured asymmetry partial syst error on final asymmetry
  Double_t asymmetry_correction_P; //effective polarization correction ppm
  Double_t pdP;//polarization partial error on final asymmetry
  Double_t asymmetry_correction_DetCorr; //effective Det. bias correction ppm
  Double_t pdDetCorr;//Det. bias correction partial error on final asymmetry ppm
  Double_t asymmetry_correction_EM_Rad_C; //effective EM raidiative correction ppm
  Double_t pdEM_Rad_C;//EM raidiative correction partial error on final asymmetry ppm
  Double_t asymmetry_correction_Acc_C; //effective acceptance correction ppm
  Double_t pdAcc_C;//acceptance correction partial error on final asymmetry ppm
  Double_t asymmetry_correction_Exp_Bias_C; //effective exp. bias correction  ppm
  Double_t pdExp_Bias_C;//Exp. bias correction partial error on final asymmetry ppm
  Double_t pdR_q2;
  
  Double_t asymmetry_correction_b[bkg_count];//correction from bkg contribution on final asymmetry
  Double_t pdb[bkg_count];//bkg contribution (asymmetry+dilution) error on final asymmetry
  Double_t pdAb[bkg_count];//bkg asymmetry partial error on final asymmetry
  Double_t pdfb[bkg_count];//bkg dilution partial error on final asymmetry
  Double_t asymmetry_total_correction;//total corrections from polarization, backgrounds, EM rad and det. bias corrections
  Double_t dA_total_asymmetry_correction;//error on total corrections from polarization, backgrounds, EM rad and det. bias corrections
  Double_t dA_asymmetry_correction;

};


#endif
 
