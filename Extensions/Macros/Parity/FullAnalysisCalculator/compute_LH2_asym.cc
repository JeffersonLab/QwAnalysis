/*
Tue Sep 25 13:50:05 EDT 2012 
Author Rakitha Beminiwattha
contact rakithab@jlab.org

03-04-2014
Buddhini Waidyawansa (buddhini@jlab.org)
Coppied from compute_LH2_asym.cc to construct the class compute_lh2_asym() required for the full analysis calculator.

*/

#include <compute_LH2_asym.h>

using namespace std;


//list of inputs
//This will eventually be read from a text file


// //Measured Asymmetry inputs
// Double_t A_raw; // unregressed asymmetry ppm 
// Double_t dA_raw_stat; // stat. error ppm
// Double_t Delta_A_reg; // regression correction ppm 
// Double_t dDelata_A_reg; // quadrature sum of dAmsr_reg,dAmsr_dpk and dAmsr_cuts  ppm

// Double_t A_msr; // ppm    
// Double_t dAmsr_inpout; //ppm
// Double_t dAmsr_stat; // ppm
// Double_t dAmsr_reg ; // ppm scheme dependance or "why-does-it-change-when-we-add-charge" and/or "whydoesitchangewhenweusetheTgtBPMvsapairofBPM" error
// Double_t dAmsr_nonlin; // ppm
// Double_t dAmsr_cuts;  // ppm
// Double_t dAmsr_dpk;//ppm double peaking error on the regression slopes e-log 772
// Double_t dAmsr_trans; // ppm
// Double_t dAmsr_syst ; // ppm (regression+nonlin.+cuts+transverse+dpk)

// //Blinder
// Double_t BlindingSign;
// Double_t BlindingFactor;

// //Additional transverse corrections
// Double_t A_msr_trans; //ppm
// Double_t dA_msr_trans;//ppm

// //Polarization
// Double_t P; 
// Double_t dP;

// //Detector Bias Correction (Q^2 light wieghting at the detector)
// Double_t DetCorr = 0.9905;
// Double_t dDetCorr = 0.9905;

// //EM radiative correction factor
// Double_t EM_Rad_C  = 1.0100;
// Double_t dEM_Rad_C = 0.0050;

// //acceptance correction
// Double_t AccCorr = 0.980;
// Double_t dAccCorr = 0.010;

// //EM+Det_bias+acceptance correction
// Double_t Exp_Bias_C;
// Double_t dExp_Bias_C;

// //Bacground Asymmetries and dilutions

// const Int_t bkg_count = 4;//no. of different bkg dilutions
// const Char_t *bkg_type[bkg_count] = {"Aluminum alloy windows","QTOR transport channel neutrals","Beamline bkg neutrals with W shutters installed","Non-signal electrons on detector"};
// Double_t Ab[bkg_count];//bkg asymmetry
// Double_t dAb[bkg_count];//bkg asymmetry error
// Double_t fb[bkg_count];//bkg dilution
// Double_t dfb[bkg_count];//bkg dilution error
// Double_t f_total;//total background dilution


// //Intermediate output parameters
// Double_t pdA_msr;//measured asymmetry partial error on final asymmetry
// Double_t pdA_msr_stat;//measured asymmetry partial stat error on final asymmetry
// Double_t pdA_msr_syst;//measured asymmetry partial syst error on final asymmetry
// Double_t asymmetry_correction_P; //effective polarization correction ppm
// Double_t pdP;//polarization partial error on final asymmetry
// Double_t asymmetry_correction_DetCorr; //effective Det. bias correction ppm
// Double_t pdDetCorr;//Det. bias correction partial error on final asymmetry ppm
// Double_t asymmetry_correction_EM_Rad_C; //effective EM raidiative correction ppm
// Double_t pdEM_Rad_C;//EM raidiative correction partial error on final asymmetry ppm
// Double_t asymmetry_correction_Acc_C; //effective acceptance correction ppm
// Double_t pdAcc_C;//acceptance correction partial error on final asymmetry ppm
// Double_t asymmetry_correction_Exp_Bias_C; //effective exp. bias correction  ppm
// Double_t pdExp_Bias_C;//Exp. bias correction partial error on final asymmetry ppm

// Double_t asymmetry_correction_b[bkg_count];//correction from bkg contribution on final asymmetry
// Double_t pdb[bkg_count];//bkg contribution (asymmetry+dilution) error on final asymmetry
// Double_t pdAb[bkg_count];//bkg asymmetry partial error on final asymmetry
// Double_t pdfb[bkg_count];//bkg dilution partial error on final asymmetry
// Double_t asymmetry_total_correction;//total corrections from polarization, backgrounds, EM rad and det. bias corrections
// Double_t dA_total_asymmetry_correction;//error on total corrections from polarization, backgrounds, EM rad and det. bias corrections

// constructor
compute_lh2_asym::compute_lh2_asym()
{
  // initialize output variables
  A_signal = -9999;
  dA_signal = -9999;
  dA_signal_stat = -9999;
  dA_signal_syst = -9999;

}



int compute_lh2_asym::computefinal()
{

  /////////////////////////////
  const Int_t bkg_count = 4;//no. of different bkg dilutions
  const Char_t *bkg_type[bkg_count] = {"Aluminum alloy windows","QTOR transport channel neutrals","Beamline bkg neutrals with W shutters installed","Non-signal electrons on detector"};
  ///////////////////////////////

  //values are set for the 25p data set
  A_raw         = -0.1698; // unregressed asymmetry ppm 
  dA_raw_stat   =  0.0306; // stat. error ppm

  //Measured Asymmetry inputs
  A_msr        = -0.2046; //ppm
  dAmsr_stat   =  0.0305; //ppm
  dAmsr_inpout =  0.0; //ppm
  dAmsr_reg    =  0.0070; // ppm
  dAmsr_dpk    =  0.0080; //ppm
  dAmsr_nonlin =  TMath::Abs(0.02*A_msr);//0.0040; // ppm
  //dAmsr_cuts   =  0.0042; // ppm from Kat's thesis   //final results A_signal(<Q^2>) =-0.2788 +/- 0.0348 (stat) +/- 0.0294 (syst) ppm
  dAmsr_cuts   =  0.0014; // ppm from Rak's thesis //final results A_signal(<Q^2>) =-0.2788 +/- 0.0348 (stat) +/- 0.0290 (syst) ppm 
  dAmsr_trans  =  0.00435; // ppm

  Delta_A_reg = A_msr - A_raw; // regression correction ppm 
  dDelata_A_reg = TMath::Sqrt(TMath::Power(dAmsr_reg,2) + TMath::Power(dAmsr_cuts,2) + TMath::Power(dAmsr_dpk,2));


  //inflate the stat error to accomodate large inpout result
  dAmsr_stat = TMath::Sqrt( TMath::Power(dAmsr_stat,2) +  TMath::Power(dAmsr_inpout,2));
  dAmsr_syst = TMath::Sqrt( TMath::Power(dAmsr_reg,2) + TMath::Power(dAmsr_dpk,2) + TMath::Power(dAmsr_nonlin,2) + TMath::Power(dAmsr_cuts,2) + TMath::Power(dAmsr_trans,2) );// ppm (regression+dpk+nonlin.+cuts+transverse)
 
  //Blinder
  BlindingSign   = +1.; //
  BlindingFactor = -0.0005079 ; //ppm 

  //Additional transverse corrections
  A_msr_trans   =  0.0; //ppm
  dA_msr_trans  =  0.00;//ppm

  //Polarization
  P  = 0.8895; 
  dP = 0.0183;

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


  //b1: Bkg 1 Al windows
  Ab[0]  =  1.7581;// ppm   
  dAb[0] =  0.2582;// ppm
  fb[0]  =  0.0323;
  dfb[0] =  0.00214; 

  //b2: QTOR transport channel neutrals
  Ab[1]  =  0.000;//  ppm
  dAb[1] =  0.2000;//  ppm   
  fb[1]  =  0.0019;//updated results
  dfb[1] =  0.0019;//
  //b3: Beamline bkg neutrals with W shutters installed
  Ab[2]  =  -5.500;//  ppm
  dAb[2] =  11.500;//  ppm   
  fb[2]  =  0.00193;
  dfb[2] =  0.00064;// 

  //b4: Non-signal electrons on detector
  Ab[3]  = -3.0200;//  ppm
  dAb[3] =  0.9700;//  ppm   
  fb[3]  =  0.0002;// 
  dfb[3] =  0.0002;// 

  f_total=0;

  //compute the total background dilution
  for (Int_t i=0;i<bkg_count;i++)
    f_total+=fb[i];

  //Print all the input values
  printInputs();
  //Start computing final LH2 asymmetry
  unblindMeasuredAsymmetry();
  computeSignalAsymmetryPartialErrors();
  computeSignalAsymmetry();
  computeMeasuredAsymmetryCorrections();
  //PlotAsymmetryCorrections();
  //theApp.Run();
  
  return(1);
}

void compute_lh2_asym::printInputs(){//Print all the input values
  printf("********Input list***************\n");
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
  printf("\n ********End of Input list***************\n");
  printf("\n");
  printf("\n");

}

void unblindMeasuredAsymmetry(){//apply the blinding factor to the measured asymmetry
  A_msr -= BlindingSign*BlindingFactor;
}



void compute_lh2_asym::computeSignalAsymmetryPartialErrors(){//compute the partial error contributions from A_msr, polarization and backgrounds
  printf("\n ************************************************\n");
  printf("Error contributions to final corrected asymmetry \n");


  dA_signal_stat=0;//final asymmetry systematic error
  dA_signal_syst=0;
  pdA_msr_stat = Exp_Bias_C * 1/P * 1/(1 - f_total) * dAmsr_stat;//measured asymmetry partial stat error on final asymmetry
  //Final statistic error
  dA_signal_stat = pdA_msr_stat;
  printf("%20s = %3.4f ppm\n","dA_msr_stat",dA_signal_stat);  

 

  pdA_msr_syst = Exp_Bias_C * 1/P * 1/(1 - f_total) * dAmsr_syst;//measured asymmetry partial syst error on final asymmetry
  printf("%20s = %3.4f ppm \n","dA_msr_syst",pdA_msr_syst); 

  dA_signal_syst += TMath::Power(pdA_msr_syst,2);//add all the syst. error contributions quadratically

  pdA_msr      = TMath::Sqrt( TMath::Power(pdA_msr_stat,2) +  TMath::Power(pdA_msr_syst,2));//measured asymmetry partial error on final asymmetry

  pdP = Exp_Bias_C * -1 * A_msr * 1/(1 - f_total) * 1/TMath::Power(P,2) * dP;//polarization partial error on final asymmetry
  printf("%20s = %3.4f ppm  \n","pdP",pdP); 

  dA_signal_syst += TMath::Power(pdP,2);//add all the syst. error contributions quadratically

//to get A_signal
  A_signal = A_msr/ (P * (1 - f_total));

  for (Int_t i=0;i<bkg_count;i++)
    A_signal -= Ab[i]*fb[i]/(1 - f_total);

  pdDetCorr = TMath::Abs(dDetCorr * AccCorr * EM_Rad_C * A_signal);
  printf("%20s = %3.4f ppm  \n","pdDetCorr",pdDetCorr); 
  pdEM_Rad_C = TMath::Abs(dEM_Rad_C * AccCorr * DetCorr * A_signal);
  printf("%20s = %3.4f ppm  \n","pdEM_Rad_C",pdEM_Rad_C);
  pdAcc_C = TMath::Abs(dAccCorr * EM_Rad_C * DetCorr * A_signal);
  printf("%20s = %3.4f ppm  \n","pdAcc_C",pdAcc_C);
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
    printf("\n %50s \n",bkg_type[i]);
    printf("%10s pdAb[%i] = %3.4f ppm  \n","",i+1,TMath::Abs(pdAb[i])); 
    printf("%10s pdfb[%i] = %3.4f ppm  \n","",i+1,TMath::Abs(pdfb[i])); 
    //compute the partial error for each background = sqrt(pdAb[i]^2 + pdfb[i]^2)
    pdb[i]=TMath::Sqrt( TMath::Power(pdAb[i],2) + TMath::Power(pdfb[i],2) );//bkg contribution (asymmetry+dilution) error on final asymmetry
    //printf("pdb[%i] = %6.4f \n",i,pdb[i]); printed for debug/double check partial bkg errors

    //background errors are added to systematic error contribution
    dA_signal_syst += TMath::Power(pdb[i],2);//add all the syst. error contributions quadratically
  }

  //Final systematic error
  dA_signal_syst = TMath::Sqrt(dA_signal_syst);

  //printf("Final Error %6.4f(stat) %6.4f(syst) ppm\n",dA_signal_stat,dA_signal_syst);//printed for debug/double check Final error
  printf("\n ********************End of Error contributions****************************\n");
}

void compute_lh2_asym::computeSignalAsymmetry(){ //compute the final corrected asymmetry
  A_signal = A_msr/ (P * (1 - f_total));

  for (Int_t i=0;i<bkg_count;i++)
    A_signal -= Ab[i]*fb[i]/(1 - f_total);

  //Detector Bias + EM radiative Corrections
  A_signal = Exp_Bias_C * A_signal;

  printf("\n********Final Acceptance  Corrected Asymmetry***************\n");
  printf("A_signal(<Q^2>) =%6.4f +/- %6.4f (stat) +/- %6.4f (syst) ppm \n",A_signal,dA_signal_stat,dA_signal_syst);
  printf("A_signal(<Q^2>) =%6.4f +/- %6.4f ppm \n",A_signal,TMath::Sqrt(TMath::Power(dA_signal_stat,2)+TMath::Power(dA_signal_syst,2)));
  printf("Blinding correction has subtracted  %f ppm. \n",BlindingSign*BlindingFactor);
  printf("************************************************\n");

};

void compute_lh2_asym::computeSignalAsymmetryTotalError(){//compute the total error for the final corrected asymmetry
  
}


void compute_lh2_asym::computeMeasuredAsymmetryCorrections(){//compute the breakdown of corrections done to the A_msr by polarization effect and background dilutions
  Double_t dA_asymmetry_correction=0;
  printf("\n");
  printf("\n");
  printf("************************************************\n");
  printf("Breakdown of Corrections:\n");
  asymmetry_correction_P = Exp_Bias_C * (A_msr/P -  A_msr)/(1 - f_total);//effective polarization correction on A_msr ppm
  printf("%50s = %+6.4f +/- %6.4f ppm \n","Effective Polarization Correction",asymmetry_correction_P,pdP);

  asymmetry_total_correction = asymmetry_correction_P;//Add to the total Correction
  dA_asymmetry_correction += TMath::Power(pdP,2);//Polarization Correction error added quadratically
  for (Int_t i=0;i<bkg_count;i++){
    asymmetry_correction_b[i] = Exp_Bias_C * -1 * Ab[i]*fb[i]/(1 - f_total);
    asymmetry_total_correction += asymmetry_correction_b[i];
    dA_asymmetry_correction += TMath::Power(pdb[i],2);//add the error contributions quadratically
    
    printf("%50s = %+6.4f +/- %6.4f ppm \n",bkg_type[i],asymmetry_correction_b[i],pdb[i]);
  }
  asymmetry_correction_Exp_Bias_C = Exp_Bias_C * A_msr/(1 - f_total) - A_msr;//effective Exp. bias correction  on A_msr ppm
  asymmetry_total_correction += asymmetry_correction_Exp_Bias_C;
  dA_asymmetry_correction += TMath::Power(pdExp_Bias_C,2);
  printf("%50s = %+6.4f +/- %6.4f ppm \n","Exp. bias  Corrections",asymmetry_correction_Exp_Bias_C,pdExp_Bias_C);


  dA_total_asymmetry_correction=TMath::Sqrt(dA_asymmetry_correction);
  printf("************************************************\n");
  printf("%50s = %+6.4f +/- %6.4f ppm \n","Total Correction to A_msr",asymmetry_total_correction,dA_total_asymmetry_correction);
  printf("************************************************\n");
  /*
    A_msr        = -0.2046 +/- 0.0305 (stat) +/- 0.0129 (syst)  ppm
    Total Correction to A_msr = -0.0771 +/- 0.0256 ppm
    A_msr + Total Correction to A_msr = -0.2817 ppm
    A_signal(<Q^2>) =-0.2812 +/- 0.0351 (stat) +/- 0.0296 (syst) ppm 
    The difference is the blinder (-0.2817 - (-0.2812) = 0.0005 ppm)
   */
};

void compute_lh2_asym::ComputeAsymmetryCorrections(){//Plot A_msr, all it's corrections and A-signal
  
};

void compute_lh2_asym::PlotAsymmetryCorrections(){//Plot A_msr, all it's corrections and A-signal
  const Int_t plot_count = 12;
  TString Xlabels[plot_count]={"A_raw","#DeltaReg","trans","nonlin","A_msr","Pol","Al","QTOR","B-L","N_#Delta","Exp_Bias","A_Tree"};
  Double_t dd_x[plot_count]={0,1,2,3,4,5,6,7,8,9,10,11};
  Double_t dd_ex[plot_count]={0,0,0,0,0,0,0,0,0,0,0,0};
  Double_t dd_y[plot_count];//={-0.2045,-0.0361,-0.0588,0.0021,-0.0009,0.0006,-0.0930,-0.2947};
  Double_t dd_ey[plot_count];//={0.0305,0.0049,0.0083,0.0022,0.0028,0.0003,0.0103,0.0359};

  dd_y[0]=A_raw;
  dd_ey[0]=dA_raw_stat;
  dd_y[1]=Delta_A_reg;
  dd_ey[1]=dDelata_A_reg;
  dd_y[2]=0;//no transverse corrections
  dd_ey[2]=dAmsr_trans;
  dd_y[3]=0;//no non-linear corrections
  dd_ey[3]=dAmsr_trans;

  dd_y[4]=A_msr;
  dd_ey[4]=TMath::Sqrt(TMath::Power(dAmsr_stat,2) + TMath::Power(dAmsr_syst,2));
  dd_y[5]=asymmetry_correction_P;
  dd_ey[5]=pdP;
  dd_y[6]=asymmetry_correction_b[0];
  dd_ey[6]=pdb[0];
  dd_y[7]=asymmetry_correction_b[1];
  dd_ey[7]=pdb[1];
  dd_y[8]=asymmetry_correction_b[2];
  dd_ey[8]=pdb[2];
  dd_y[9]=asymmetry_correction_b[3];
  dd_ey[9]=pdb[3];
  dd_y[10]=asymmetry_correction_Exp_Bias_C;
  dd_ey[10]=pdExp_Bias_C;
  dd_y[11]=A_signal;
  dd_ey[11]=TMath::Sqrt(TMath::Power(dA_signal_stat,2) + TMath::Power(dA_signal_syst,2));


  TGraphErrors * gre = new TGraphErrors(plot_count,dd_x,dd_y,dd_ex,dd_ey);
  //TH2F(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
  TH2F *h = new TH2F("Graph25","Wien 0: Measured Asymmetry, Corrections and Final Asymmetry",plot_count,-0.5,plot_count,10,-0.350,0.050);

  for(Int_t i=0;i<plot_count;i++){
    (h->GetXaxis())->SetBinLabel((h->GetXaxis())->FindBin(i),Xlabels[i].Data());
  }

  gre->SetName("Bar_Graph");
  gre->SetTitle("Wien 0: Measured Asymmetry Corrections");


  h->Draw();
  gre->Draw("b");
  
}
 
