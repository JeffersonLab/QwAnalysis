//*****************************************************************************************************//
// Author      : Nuruzzaman on 07/16/2013
// Last Update : 07/23/2013
// 
//*****************************************************************************************************//
/*
  This macro is to extract final physics asymmetry for Transverse N-to-Delta.

******************************************************************************************************/


using namespace std;
#include "NurClass.h"


int main(Int_t argc,Char_t* argv[]){

  Bool_t FIGURE = 0;
  Bool_t SUMMARY_PLOT = 1;
  Bool_t ERROR_PLOT = 0;


  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //   printf ( "%sThe current date/time is: %s%s%s",blue,red,asctime(timeinfo),normal);

  //   TString database="qw_run2_pass1";
  //   TString database_stem="run2_pass1";
  TString database="qw_run2_pass5";
  TString database_stem="run2_pass5";
  //   TString deviceTitle = "Barsum"; TString deviceName = "Barsum";
  TString deviceTitle = "PMTavg"; TString deviceName = "PMTavg";

  std::ofstream MyfileFinal;


  TString target, polar,targ, goodfor, reg_set, reg_calc;

  Int_t opt =1;
  Int_t ropt = 2;
  Int_t qtor_opt = 2;

  TString good_for;
  TString qtor_current;
  TString qtor_stem;
  TString good;
  TString interaction,interaction2;

  /* Canvas and Pad and Scale parameters */
  Int_t canvasSize[4] ={1600,1000,1200,1000};
  Double_t pad1x[2] = {0.005,0.995};
  Double_t pad1y[2] = {0.935,0.995};
  Double_t pad2x[2] = {0.005,0.995};
  Double_t pad2y[2] = {0.005,0.945};


  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form(" \nSlug averages of Main Detector Asymmetries \n")<<std::endl;
  std::cout<<Form("###############################################")<<std::endl;
  std::cout<<Form("Enter target type (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  std::cout<<Form("1. %sLiquid Hydrogen (deafult)%s",blue,normal)<<std::endl;
  std::cout<<Form("2. 4% DS Al ")<<std::endl;
  std::cout<<Form("3. 1.6% DS Carbon ")<<std::endl;
  //   std::cin>>opt;
  std::string input_opt;
  std::getline( std::cin, input_opt );
  if ( !input_opt.empty() ) {
    std::istringstream stream( input_opt );
    stream >> opt;
  }


  ropt = 2;
  //   if(ropt==2){
  //     std::cout<<Form("Enter QTOR current (%sJust Hit ENTER to choose default%s):",blue,normal)<<std::endl;
  //     std::cout<<Form("1.6000 A ")<<std::endl;
  //     std::cout<<Form("2.%s6700 A (deafult)%s",blue,normal)<<std::endl;
  //     std::cout<<Form("3.7300 A ")<<std::endl;
  //     //   std::cin>>qtor_opt;
  //     std::string input_qtor_opt;
  //     std::getline( std::cin, input_qtor_opt );
  //     if ( !input_qtor_opt.empty() ) {
  //       std::istringstream stream( input_qtor_opt );
  //       stream >> qtor_opt;
  //     }
  //   }


  if(argc>1) database = argv[1];

  // select the target
  if(opt == 1){
    target = "HYDROGEN-CELL";
    targ = "HYDROGEN-CELL";
  }
  else if(opt == 2){
    target = "DS-4%-Aluminum";
    targ = "DS 4 %% Al";
  }
  else if(opt == 3){
    target = "DS-1.6%-C";
    targ = "DS 1.6 %% Carbon";
  }
  else{
    std::cout<<Form("Unknown target type!")<<std::endl;
    exit(1);
  }

  
  // Select the interaction
  if(ropt == 1){
    good_for = "(md_data_view.good_for_id = '3' or md_data_view.good_for_id = '1,"+good+"')";
    interaction = "elastic";
    qtor_current=" (slow_controls_settings.qtor_current>8800 && slow_controls_settings.qtor_current<9000) ";
    qtor_stem = "8901";
  }
  else if(ropt == 2){
    good_for = "(md_data_view.good_for_id = '"+good+",18')";
    interaction  = "n-to-delta"; 
    interaction2 = "N-to-#Delta"; 
  }
  else if(ropt == 3){
    good_for = "(md_data_view.good_for_id = '1,3,21')";
    qtor_current=" (slow_controls_settings.qtor_current>8990 && slow_controls_settings.qtor_current<9010) ";
    qtor_stem = "9000";
    interaction = "dis"; 
  }
  else{
    std::cout<<Form("Unknown interaction type!")<<std::endl;
    exit(1);
  }

  // QTOR current cut
  if(qtor_opt == 1){
    qtor_current=" (slow_controls_settings.qtor_current>5900 && slow_controls_settings.qtor_current<6100) ";
    qtor_stem = "6000";
  }
  else if(qtor_opt == 2){
    qtor_current=" (slow_controls_settings.qtor_current>6600 && slow_controls_settings.qtor_current<6800) ";
    qtor_stem = "6700";
  }
  else if(qtor_opt == 3){
    qtor_current=" (slow_controls_settings.qtor_current>7200 && slow_controls_settings.qtor_current<7400) ";
    qtor_stem = "7300";
  }



  TApplication theApp("App",&argc,argv);

  Char_t  textfileFinal[400];
  sprintf(textfileFinal,"dirPlot/resultText/%s_%s_%s_MD_Final_%s.txt"
	  ,interaction.Data(),qtor_stem.Data(),target.Data(),database_stem.Data());


  /********************************************/
  /*            Initilize variables           */
  /********************************************/

  Double_t A_msr_v,dAmsr_v,dAmsr_stat_v,dAmsr_syst_v;
  Double_t A_msr_h,dAmsr_h,dAmsr_stat_h,dAmsr_syst_h;
  Double_t BlindingFactor,BlindingCorrection;
  Double_t A_msr,dAmsr_stat,dAmsr_syst,dAmsr;
  Double_t dAmsr_P,dAmsr_P_h,dAmsr_P_v;
  Double_t dAmsr_regscheme,dAmsr_regscheme_h,dAmsr_regscheme_v;
  Double_t dAmsr_reg_time,dAmsr_reg_time_h,dAmsr_reg_time_v;
  Double_t dAmsr_nonlin,dAmsr_nonlin_h,dAmsr_nonlin_v;
  Double_t dAmsr_cuts,dAmsr_cuts_h,dAmsr_cuts_v;
  Double_t dAmsr_fit,dAmsr_fit_h,dAmsr_fit_v;
  Double_t dAmsr_q2_acceptance,dAmsr_q2_acceptance_h,dAmsr_q2_acceptance_v;
  Double_t dAmsr_trans_h,dAmsr_trans_v,dAmsr_trans;
  Double_t P,dP;
  Double_t Q2,dQ2;
  Double_t Theta,dTheta;
  Double_t A_b1,dA_b1,f_b1,df_b1,c_b1;
  Double_t A_b2,dA_b2,f_b2,df_b2,c_b2;
  Double_t A_b3,dA_b3,f_b3,df_b3,c_b3;
  Double_t A_b4,dA_b4,f_b4,df_b4,c_b4;
  Double_t DetCorr,dDetCorr;
  Double_t RadCorr,dRadCorr;
  Double_t BinCenterCorr,dBinCenterCorr;
  Double_t Q2Corr,dQ2Corr;
  Double_t TotalCorr,f_Total;
  Double_t A_msr_unreg,RegCorr,A_msr_unreg_v,RegCorr_v,A_msr_unreg_h,RegCorr_h;
  Double_t dA_msr_unreg,dA_msr_unreg_v,dA_msr_unreg_h;
//   Double_t dAmsr_dblpeak_h,dAmsr_dblpeak_v,dAmsr_dblpeak;
  Double_t A_phys,dAphys,dAphys_stat,dAphys_sys;
  Double_t dAphys_Amsr,dAphys_P,dAphys_RC,dAphys_Det,dAphys_Bin,dAphys_Q2;
  Double_t dAphys_Ab1,dAphys_Ab2,dAphys_Ab3,dAphys_Ab4;
  Double_t dAphys_fb1,dAphys_fb2,dAphys_fb3,dAphys_fb4;

  TString line;
  line  = Form("*********************************************************************");


  // *************************************
  // Vertical and Horizontal Measured Asymmetry
  // *************************************
  // 

  A_msr_v          = 4.5250;
  dAmsr_stat_v     = 0.8064;
//   dAmsr_syst_v     = 0;
//   dAmsr_v          = TMath::Sqrt( pow(dAmsr_stat_v,2) + pow(dAmsr_syst_v,2) );

  A_msr_h          = 5.3432;
  dAmsr_stat_h     = 0.5325;
//   dAmsr_syst_h     = 0;
//   dAmsr_h          = TMath::Sqrt( pow(dAmsr_stat_h,2) + pow(dAmsr_syst_h,2) );


  // *****************************************************************************
  // BEAM POLARIZATION
  // *****************************************************************************
  // Official result: Dave Gaskell elog https://qweak.jlab.org/elog/Moller/109
  // See comments on Wien0 polarization value in Hydrogen elastic section.
  // Since the same polarization value is used for Al and elastic H in Wien0, 
  // this error is correlated but we plan to ignore that. 
  P = 0.879;
  dP = 0.018;

  // ***************************************
  // DS Aluminum Window Asymmetry for Wien 0
  // ***************************************
  // asymmetry units are ppm
  // no blinding factor for Aluminum!

  BlindingFactor = 0.000;
  BlindingCorrection = 0.000;

  // ******************
  // MEASURED ASYMMETRY
  // *******************
  // Kamyers's elog 743 Wien 0 pass5 analysis
  // https://qweak.jlab.org/elog/Analysis+%26+Simulation/743
  // mdallpmtavg, Std regression, slugs 1013-1019

  //   A_msr = 1.6438;
  //   dAmsr_stat = 0.1700;
  A_msr = ((1/pow(dAmsr_stat_v,2))*(A_msr_v) + (1/pow(dAmsr_stat_h,2))*(A_msr_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  dAmsr_stat = 1/TMath::Sqrt( (1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)) );

 // Uncertainty from Polarization
  dAmsr_P_h = (dP/P)*A_msr_h;
  dAmsr_P_v = (dP/P)*A_msr_v;
  dAmsr_P   = (dP/P)*A_msr;

  // Regression Scheme Uncertainty
  // ibid. see table of regression corrections for slugs 1013-1019

  dAmsr_regscheme_h = 0.0040;
  dAmsr_regscheme_v = 0.0060;
  dAmsr_regscheme   = ((1/pow(dAmsr_stat_v,2))*(dAmsr_regscheme_v) + (1/pow(dAmsr_stat_h,2))*(dAmsr_regscheme_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  //dAmsr_regscheme   = 0.005;


  // Regression Time Dependence Uncertainty
  dAmsr_reg_time_h = 0.0060;
  dAmsr_reg_time_v = 0.0081;
  dAmsr_reg_time   = ((1/pow(dAmsr_stat_v,2))*(dAmsr_reg_time_v) + (1/pow(dAmsr_stat_h,2))*(dAmsr_reg_time_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  //dAmsr_reg_time   = 0.0101;

  // Fit Scheme Dependence Uncertainty
  dAmsr_fit_h = 0.0390;
  dAmsr_fit_v = 0.0830;
  dAmsr_fit   = ((1/pow(dAmsr_stat_v,2))*(dAmsr_fit_v) + (1/pow(dAmsr_stat_h,2))*(dAmsr_fit_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  //dAmsr_fit   = 0.0524;

  // MD whole detector asymmetry scheme dependence
  // ** ignore for now **
  // ibid. Katherine recommended assigning an uncertainty of 0.0022 ppm
  // but I'm taking the position that the MDall scheme dependence is
  // consistent with transverse leakage and statistical shifts from 
  // weighting errors. The issue isn't closed. If I'm right, 
  // Rakitha will find significant weighting deficiencies in the Al
  // data, and improving these weights will bring all 3 schemes into
  // even better agreement. 

  // Nonlinearity Correction Uncertainty 
  // In https://qweak.jlab.org/elog/Analysis+%26+Simulation/743 , 
  // Katherine gives an average value for the nonlinearity of -1.1% for slugs 1013-1019. 
  // reminder: the following calculation for Al is not contingent upon any blinding factor 

  dAmsr_nonlin_h = TMath::Abs(-0.01*A_msr_h);
  dAmsr_nonlin_v = TMath::Abs(-0.01*A_msr_v);
  dAmsr_nonlin   = ((1/pow(dAmsr_stat_v,2))*(dAmsr_nonlin_v) + (1/pow(dAmsr_stat_h,2))*(dAmsr_nonlin_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  //dAmsr_nonlin   = TMath::Abs(-0.01*A_msr);

  // Cuts Dependence Ambiguity
  // Katherine argues for 3.6 ppb in https://qweak.jlab.org/elog/Analysis+%26+Simulation/743 .
  // This seems appropriately smaller than the pass4 value in her thesis p. 150 of 14.9 ppb.
  // She also noted a shift of 108 ppb if the slugs with ridiculous nonlinearities were included.
  // We really need to track down the reason for periods of apparently bogus nonlinearity. 

  dAmsr_cuts_h = 0.0640;
  dAmsr_cuts_v = 0.0680;
  dAmsr_cuts   = ((1/pow(dAmsr_stat_v,2))*(dAmsr_cuts_v) + (1/pow(dAmsr_stat_h,2))*(dAmsr_cuts_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  //dAmsr_cuts = 0.0654138356957523;



  // Q2 acceptance for transverse N-to-Delta
  dAmsr_q2_acceptance_h = 0.064;
  dAmsr_q2_acceptance_v = 0.054;
  dAmsr_q2_acceptance   = ((1/pow(dAmsr_stat_v,2))*(dAmsr_q2_acceptance_v) + (1/pow(dAmsr_stat_h,2))*(dAmsr_q2_acceptance_h))/((1/pow(dAmsr_stat_v,2)) + (1/pow(dAmsr_stat_h,2)));
  //dAmsr_q2_acceptance   = 0.0611;

  // Transverse Asymmetry Leakage Uncertainty
  // For the purpose of interpreting the Al asymmetry, there is a reasonable estimate 
  // (as an upper limit) in Kamyers's thesis p. 160 (pass4) of +- 5 ppb.
  // However, for the purpose of Wien0 Al bkg subtraction, she convinced me that any 
  // transverse leakage into the PV Al asymmetry is a feature not a bug. Since Al runs were spread
  // throughout Wien0, I'll assign zero error.

  dAmsr_trans_h = 0.000;
  dAmsr_trans_v = 0.000;

  dAmsr_trans = 0.000;
  //      dAmsr_trans = 0.005;


  // Systematic calculation 
  //   dAmsr_syst = TMath::Sqrt(pow(dAmsr_regscheme,2) + pow(dAmsr_nonlin,2) + pow(dAmsr_trans,2) );

  dAmsr_syst_h = TMath::Sqrt(  pow(dAmsr_P_h,2) + pow(dAmsr_regscheme_h,2) + pow(dAmsr_reg_time_h,2) + pow(dAmsr_nonlin_h,2) + pow(dAmsr_cuts_h,2) + pow(dAmsr_q2_acceptance_h,2) + pow(dAmsr_fit_h,2) );
  dAmsr_syst_v = TMath::Sqrt(  pow(dAmsr_P_v,2) + pow(dAmsr_regscheme_v,2) + pow(dAmsr_reg_time_v,2) + pow(dAmsr_nonlin_v,2) + pow(dAmsr_cuts_v,2) + pow(dAmsr_q2_acceptance_v,2) + pow(dAmsr_fit_v,2) );

//   dAmsr_syst_h = dAmsr_regscheme_h + dAmsr_reg_time_h + dAmsr_nonlin_h + dAmsr_trans_h + dAmsr_q2_acceptance_h;
//   dAmsr_syst_v = dAmsr_regscheme_v + dAmsr_reg_time_v + dAmsr_nonlin_v + dAmsr_trans_v + dAmsr_q2_acceptance_v;

  dAmsr_syst = TMath::Sqrt(  pow(dAmsr_P,2) + pow(dAmsr_regscheme,2) + pow(dAmsr_reg_time,2) + pow(dAmsr_nonlin,2) + pow(dAmsr_cuts,2) + pow(dAmsr_q2_acceptance,2) + pow(dAmsr_fit,2) );


  dAmsr_h = TMath::Sqrt(  pow(dAmsr_stat_h,2) + pow(dAmsr_syst_h,2) );
  dAmsr_v = TMath::Sqrt(  pow(dAmsr_stat_v,2) + pow(dAmsr_syst_v,2) );
  dAmsr   = TMath::Sqrt(  pow(dAmsr_stat,2) + pow(dAmsr_syst,2) );

  // *****************************************************************************
  // BACKGROUNDS
  // *****************************************************************************
  // **  In accordance with Eqn 4 in the version 2 Qweak tech note at 
  // https://qweak.jlab.org/doc-private/ShowDocument?docid=965
  // background asymmetries are implicitly understood to have been corrected by
  // the appropriate factor of 1/P. **

  // Bkg 1 Al windows
  // ** Set asymmetry and dilution to zero. Does not exist for 4% DS Al target.**
  A_b1 = 8.43084185469048;
  dA_b1 = 0.985267452607854;
  // Al window bkg dilution 
  f_b1 = 0.03299;         
  df_b1 = 0.00221585198062;           

  c_b1 = A_b1*f_b1;

  // The neutral backgrounds will be partitioned as per Mark Pitt's suggestion:
  // i.e., that the majority of the neutral background will carry an elastic-like asymmetry
  // while only the shutter-closed background will carry the large asymmetry implied by MD9, etc.

  // Bkg 2 QTOR transport channel neutrals               
  // ** Simulation is needed to estimate the asymmetry of the trivial (scraping) component. **
  // Given the lack of supporting simulations, Kent pointed out that assuming Moeller dominance 
  // would yield a less conservative assumption than elastic dominance. We'll keep the error
  // bar from the elastic dominance assumption however.
  A_b2 =  0.000;          
  //      A_b2 =  1.6;          
  dA_b2 = 0.200;            
  // dilution = Rakitha-style all neutral bkg minus the shutter-based beamline bkg  
  // Unfortunately, no Rakitha-style result exists for Al from which we can subtract the 
  // shutter-based beamline bkg. Until it does, I'll use the Hydrogen elastic result below
  // as a guide and assign a 100% relative uncertainty. (WAG. No reference.) 
//   f_b2 = 0.052;      
//   df_b2 = 0.004;         
  f_b2 = 0.04205;      
  df_b2 = 0.00954594;

  c_b2 = A_b2*f_b2;

  // Bkg 3 beamline background neutrals
  // Kamyers's elog https://qweak.jlab.org/elog/Analysis+%26+Simulation/793
  // proposes we need to be more conservative knowledge of the beamline asymmetry
  // during Al running for Wien0. Constraints from Al data taking in Run I
  // are much weaker than they are for Hydrogen elastic. Run II data will help. 
  // (Hey, we agreed we wanted to go beyond the 0.5+-1ppm assumption from p. 154 and Fig 5.19
  // of Katherine's thesis. We implemented that for Hydrogen recently. This is the other shoe 
  // dropping just as we were about to drift off to sleep.) 
  A_b3 = -5.500;
  dA_b3 = 11.500;
  // obsolete      A_b3 = 0.5;
  // obsolete      dA_b3 = 1.5;
  // ** The following lead wall plug dilutions should be superseded with more precise 
  // W shutter measurements if they exist for Al targets. **
  // Impossible! That would be like asking for sharks with laser beams on their heads. 
  // So we're stuck using the value from Kamyers's thesis p. 104
  f_b3 = 0.00193;
  df_b3 = 0.00064;

  c_b3 = A_b3*f_b3;

  // Bkg 4 Non-signal Electrons in QTOR transport channel (N to Delta in this case)          
  // ** Set to zero for now. ** Here, inelastics as part of the measured Al so don't want 
  // to subtract them. If for some reason there are significant differences between the 
  // inelastic bkg fractions in the 4% DS Al and actual thin window targets, then that 
  // should probably be handled separately as a "radiative" correction. 

  A_b4 = -5.305; 
  dA_b4 = 0.166;

  f_b4 = 0.701007;
  df_b4 = 0.0130575673504083;

  c_b4 = A_b4*f_b4;

//     A_b2 = 0.0; dA_b2 = 0.0; f_b2 = 0.0; df_b2 = 0.0;
//     A_b3 = 0.0; dA_b3 = 0.0; f_b3 = 0.0; df_b3 = 0.0;


  f_Total = f_b1 + f_b2 + f_b3 + f_b4;

  // *****************************************************************************
  // Detector Bias Correction 
  // *****************************************************************************

  // ** No radiative correction for Al data as a background. Only need
  // that for physics interpretation. **
  RadCorr = 1.000;                       
  dRadCorr = 0.0*TMath::Abs(1.-RadCorr); //  no uncertainty on the correction


  // ** No such correction is needed for purposes of bkg subtraction. **
  // This can be revisited later when precision improves or if we
  // try to interpret the Al physics asymmetry. 

  DetCorr  = 1.0000;
  dDetCorr = 0.0000;

 
  // ** No bin centering correction for Al data as a background. Only need
  // that for physics interpretation. **
  BinCenterCorr  = 1.000;
  dBinCenterCorr = 0.0*TMath::Abs(1.-BinCenterCorr);

  // ** No Q2 for precision calibration of Q2 yet.
  Q2Corr  = 1.000;
  dQ2Corr = 0.0*TMath::Abs(1.-Q2Corr);

  TotalCorr = RadCorr*DetCorr*BinCenterCorr*Q2Corr;

  // *****************************************************************************
  // MISC Inputs
  // *****************************************************************************

  // ** Q2 from geant3 by Nur. Units are (GeV/c)^2. J. Leacock had 0.02078+- 0.0005. 
  // Error bar set to be 2.4% of Q2 using 
  // Siyuan's Col talk DocDB 1646/ Mark Pitt ELOG 774.
  Q2  = 0.02088;
  dQ2 = 0.024*Q2;

  // ** Theta from geant3 by Nur. Units are (degree) 
  // Error bar set to RMS of the distribution for now. 
  Theta  = 8.348;
  dTheta = 1.337;

  // ******************************************
  // Interpretational Error from the Acceptance
  // *******************************************
  // ** No such correction is needed for the purposes of bkg subtraction. **


  // *********************************************
  // Interpretational Error from 2-photon Exchange
  // **********************************************
  // ** No such correction is needed for the purposes of bkg subtraction. **


  // *****************************************************************************
  // Information only: Unregressed Asymmetry and Size of Std Regression Correction
  // ******************************************************************************
  // Kamyers's elog 743 Wien 0 pass5 analysis
  // https://qweak.jlab.org/elog/Analysis+%26+Simulation/743
  // mdallpmtavg, slugs 1013-1019

  //   A_msr_unreg = 1.6308;
  A_msr_unreg_v    = 4.6020;
  A_msr_unreg_h    = 5.3390;

  dA_msr_unreg_v   = 0.8070;
  dA_msr_unreg_h   = 0.5330;

  A_msr_unreg      = ((1/pow(dA_msr_unreg_v,2))*(A_msr_unreg_v) + (1/pow(dA_msr_unreg_h,2))*(A_msr_unreg_h))/((1/pow(dA_msr_unreg_v,2)) + (1/pow(dA_msr_unreg_h,2)));
  dA_msr_unreg     = 1/TMath::Sqrt( (1/pow(dA_msr_unreg_v,2)) + (1/pow(dA_msr_unreg_h,2)) );

  RegCorr_v        = TMath::Abs(A_msr_unreg_v - A_msr_v);  // note A_msr = A_msr_unreg + RegCorr
  RegCorr_h        = TMath::Abs(A_msr_unreg_h - A_msr_h);
  RegCorr          = TMath::Sqrt( pow(RegCorr_v,2) + pow(RegCorr_h,2) );

  // donotuseforAluminum      A_msr_unreg = A_msr_unreg - BlindingCorrection


  // *****************************************************************************
  // Extraction of physics asymmetry
  // ******************************************************************************

  A_phys       = TotalCorr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*f_b3 - A_b4*f_b4 )/(1 - f_Total) );

  dAphys_Amsr  = TMath::Abs(TotalCorr*( ((dAmsr_stat/P))/(1 - f_Total) ));
  dAphys_P     = TMath::Abs(TotalCorr*( ((A_msr/P)*(dP/P))/(1 - f_Total) ));
  dAphys_Ab1   = TMath::Abs(TotalCorr*( ( - dA_b1*f_b1 )/(1 - f_Total) ));
  dAphys_Ab2   = TMath::Abs(TotalCorr*( ( - dA_b2*f_b2 )/(1 - f_Total) ));
  dAphys_Ab3   = TMath::Abs(TotalCorr*( ( - dA_b3*f_b3 )/(1 - f_Total) ));
  dAphys_Ab4   = TMath::Abs(TotalCorr*( ( - dA_b4*f_b4 )/(1 - f_Total) ));

  dAphys_fb1   = TMath::Abs(TotalCorr*( ((A_msr/P) - A_b1*(1-f_b2-f_b3-f_b4) - A_b2*f_b2 - A_b3*f_b3 - A_b4*f_b4 )*df_b1 /pow((1 - f_Total),2) ));
  dAphys_fb2   = TMath::Abs(TotalCorr*( ((A_msr/P) - A_b1*f_b1 - A_b2*(1-f_b1-f_b3-f_b4) - A_b3*f_b3 - A_b4*f_b4 )*df_b2 /pow((1 - f_Total),2) ));
  dAphys_fb3   = TMath::Abs(TotalCorr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*(1-f_b1-f_b2-f_b4) - A_b4*f_b4 )*df_b3 /pow((1 - f_Total),2) ));
  dAphys_fb4   = TMath::Abs(TotalCorr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*f_b3 - A_b4*(1-f_b1-f_b2-f_b3) )*df_b4 /pow((1 - f_Total),2) ));

  dAphys_RC    = TMath::Abs( dRadCorr*DetCorr*BinCenterCorr*Q2Corr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*f_b3 - A_b4*f_b4 )/(1 - f_Total) ));
  dAphys_Det   = TMath::Abs( RadCorr*dDetCorr*BinCenterCorr*Q2Corr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*f_b3 - A_b4*f_b4 )/(1 - f_Total) ));
  dAphys_Bin   = TMath::Abs( RadCorr*DetCorr*dBinCenterCorr*Q2Corr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*f_b3 - A_b4*f_b4 )/(1 - f_Total) ));
  dAphys_Q2    = TMath::Abs( RadCorr*DetCorr*BinCenterCorr*dQ2Corr*( ((A_msr/P) - A_b1*f_b1 - A_b2*f_b2 - A_b3*f_b3 - A_b4*f_b4 )/(1 - f_Total) ));

  dAphys_stat  =  TMath::Abs(dAphys_Amsr);

  dAphys_sys   =  TMath::Sqrt(pow(dAphys_P,2) + pow(dAphys_Ab1,2) + pow(dAphys_Ab2,2) + pow(dAphys_Ab3,2) + pow(dAphys_Ab4,2) + pow(dAphys_fb1,2) + pow(dAphys_fb2,2) + pow(dAphys_fb3,2) + pow(dAphys_fb4,2) + pow(dAphys_RC,2) + pow(dAphys_Det,2) + pow(dAphys_Bin,2) + pow(dAphys_Q2,2) );

  dAphys       =  TMath::Sqrt( pow(dAphys_Amsr,2) + pow(dAphys_P,2) + pow(dAphys_Ab1,2) + pow(dAphys_Ab2,2) + pow(dAphys_Ab3,2) + pow(dAphys_Ab4,2) + pow(dAphys_fb1,2) + pow(dAphys_fb2,2) + pow(dAphys_fb3,2) + pow(dAphys_fb4,2) + pow(dAphys_RC,2) + pow(dAphys_Det,2) + pow(dAphys_Bin,2) + pow(dAphys_Q2,2) );

  /*****************************************************************************/
  /*                                    Output                                 */
  /*****************************************************************************/



  cout <<"\n"<<line<<endl;

  TString unitPPM = "ppm";

  /*********************************************/

  TString measuredAsymmetry_h, measuredAsymmetry_h1, measuredAsymmetry_h2, measuredAsymmetry_h3;
  TString measuredAsymmetry_h4, measuredAsymmetry_h5, measuredAsymmetry_h6, measuredAsymmetry_h7;
  TString measuredAsymmetry_h8, measuredAsymmetry_h9, measuredAsymmetry_h10, measuredAsymmetry_h11;
  TString measuredAsymmetry_h12, measuredAsymmetry_h13, measuredAsymmetry_h14, measuredAsymmetry_h15;

  measuredAsymmetry_h1  = Form("Blinded Measured Asymmetry and Errors for Horizontal Transverse:");
  measuredAsymmetry_h2  = Form("Amsr_h(unregressed) \t = %2.3f ppm \tRegression Correction \t = %2.3f ppm ", A_msr_unreg_h,RegCorr_h);
  measuredAsymmetry_h3  = Form("Amsr_h \t\t\t = %2.3f ppm ", A_msr_h);
  measuredAsymmetry_h4  = Form("dAmsr_stat_h \t\t = %2.3f ppm ", dAmsr_stat_h);
  measuredAsymmetry_h5  = Form("dAmsr_regscheme_h \t = %2.3f ppm ", dAmsr_regscheme_h);
  measuredAsymmetry_h6  = Form("dAmsr_reg_time_h \t = %2.3f ppm ", dAmsr_reg_time_h);
  measuredAsymmetry_h7  = Form("dAmsr_cuts_h \t\t = %2.3f ppm ", dAmsr_cuts_h);
//   measuredAsymmetry_h8  = Form("dAmsr_dblpeak_h \t = %2.3f ppm ", dAmsr_dblpeak_h);
//   measuredAsymmetry_h9  = Form("dAmsr_trans_h \t\t = %2.3f ppm ", dAmsr_trans_h);
  measuredAsymmetry_h10  = Form("dAmsr_nonlin_h \t\t = %2.3f ppm ", dAmsr_nonlin_h);
  measuredAsymmetry_h11 = Form("dAmsr_syst_h \t\t = %2.3f ppm (total reg.+nonlin.+transverse)", dAmsr_syst_h);
  measuredAsymmetry_h12  = Form("Amsr_h = %2.3f +- %2.3f (stat) +- %2.3f (syst) ppm ", A_msr_h, dAmsr_stat_h, dAmsr_syst_h);

  cout <<"\n"<<measuredAsymmetry_h1<<"\n"<<measuredAsymmetry_h2<<"\n"<<measuredAsymmetry_h3 
       <<"\n"<<measuredAsymmetry_h4<<"\n"<<measuredAsymmetry_h5<<"\n"<<measuredAsymmetry_h6 
       <<"\n"<<measuredAsymmetry_h7<<"\n"<<measuredAsymmetry_h8<<"\n"<<measuredAsymmetry_h9 
       <<"\n"<<measuredAsymmetry_h10<<"\n\n"<<measuredAsymmetry_h11<<"\n"<<measuredAsymmetry_h12<<endl;


  TString measuredAsymmetry_v, measuredAsymmetry_v1, measuredAsymmetry_v2, measuredAsymmetry_v3;
  TString measuredAsymmetry_v4, measuredAsymmetry_v5, measuredAsymmetry_v6, measuredAsymmetry_v7;
  TString measuredAsymmetry_v8, measuredAsymmetry_v9, measuredAsymmetry_v10, measuredAsymmetry_v11;
  TString measuredAsymmetry_v12, measuredAsymmetry_v13, measuredAsymmetry_v14, measuredAsymmetry_v15;

  measuredAsymmetry_v1  = Form("Blinded Measured Asymmetry and Errors for Vertical Transverse:");
  measuredAsymmetry_v2  = Form("Amsr_v(unregressed) \t = %2.3f ppm \tRegression Correction \t = %2.3f ppm ", A_msr_unreg_v,RegCorr_v);
  measuredAsymmetry_v3  = Form("Amsr_v \t\t\t = %2.3f ppm ", A_msr_v);
  measuredAsymmetry_v4  = Form("dAmsr_stat_v \t\t = %2.3f ppm ", dAmsr_stat_v);
  measuredAsymmetry_v5  = Form("dAmsr_regscheme_v \t = %2.3f ppm ", dAmsr_regscheme_v);
  measuredAsymmetry_v6  = Form("dAmsr_reg_time_v \t = %2.3f ppm ", dAmsr_reg_time_v);
  measuredAsymmetry_v7  = Form("dAmsr_cuts_v \t\t = %2.3f ppm ", dAmsr_cuts_v);
//   measuredAsymmetry_v8  = Form("dAmsr_dblpeak_v \t = %2.3f ppm ", dAmsr_dblpeak_v);
//   measuredAsymmetry_v9  = Form("dAmsr_trans_v \t\t = %2.3f ppm ", dAmsr_trans_v);
  measuredAsymmetry_v10 = Form("dAmsr_nonlin_v \t\t = %2.3f ppm ", dAmsr_nonlin_v);
  measuredAsymmetry_v11 = Form("dAmsr_syst_v \t\t = %2.3f ppm (total reg.+nonlin.+transverse)", dAmsr_syst_v);
  measuredAsymmetry_v12 = Form("Amsr_v = %2.3f +- %2.3f (stat) +- %2.3f (syst) ppm ", A_msr_v, dAmsr_stat_v, dAmsr_syst_v);

  cout <<"\n"<<measuredAsymmetry_v1<<"\n"<<measuredAsymmetry_v2<<"\n"<<measuredAsymmetry_v3 
       <<"\n"<<measuredAsymmetry_v4<<"\n"<<measuredAsymmetry_v5<<"\n"<<measuredAsymmetry_v6 
       <<"\n"<<measuredAsymmetry_v7<<"\n"<<measuredAsymmetry_v8<<"\n"<<measuredAsymmetry_v9 
       <<"\n"<<measuredAsymmetry_v10<<"\n\n"<<measuredAsymmetry_v11<<"\n"<<measuredAsymmetry_v12<<endl;


  TString measuredAsymmetry, measuredAsymmetry1, measuredAsymmetry2, measuredAsymmetry3;
  TString measuredAsymmetry4, measuredAsymmetry5, measuredAsymmetry6, measuredAsymmetry7;
  TString measuredAsymmetry8, measuredAsymmetry9, measuredAsymmetry10, measuredAsymmetry11;
  TString measuredAsymmetry12, measuredAsymmetry13, measuredAsymmetry14, measuredAsymmetry15;

  measuredAsymmetry1  = Form("Blinded Measured Asymmetry and Errors (Combined Horizontal and Vertical):");
  measuredAsymmetry2  = Form("Amsr (unregressed) \t = %2.3f ppm, \tRegression Correction \t = %2.3f ppm ", A_msr_unreg,RegCorr);
  measuredAsymmetry3  = Form("Amsr \t\t\t = %2.3f ppm ", A_msr);
  measuredAsymmetry4  = Form("dAmsr_{stat} \t\t = %2.3f ppm ", dAmsr_stat);
  measuredAsymmetry5  = Form("dAmsr_{P} \t = %2.3f ppm ", dAmsr_P);
  measuredAsymmetry6  = Form("dAmsr_{regscheme} \t = %2.3f ppm ", dAmsr_regscheme);
  measuredAsymmetry7  = Form("dAmsr_{reg_time} \t = %2.3f ppm ", dAmsr_reg_time);
  measuredAsymmetry8  = Form("dAmsr_{nonlin} \t\t = %2.3f ppm ", dAmsr_nonlin);
  measuredAsymmetry9  = Form("dAmsr_{cuts} \t\t = %2.3f ppm ", dAmsr_cuts);
  measuredAsymmetry10 = Form("dAmsr_{q2_acceptance} \t = %2.3f ppm ", dAmsr_q2_acceptance);
  measuredAsymmetry11 = Form("dAmsr_{fit} \t\t = %2.3f ppm ", dAmsr_fit);
  measuredAsymmetry12 = Form("dAmsr_{sys} \t\t = %2.3f ppm (quadrature sum of all sys.)", dAmsr_syst);
  measuredAsymmetry13 = Form("dAmsr \t\t= %2.3f ppm ", dAmsr);
  measuredAsymmetry14 = Form("A_{M}^{in} = %2.3f #pm %2.3f (stat) #pm %2.3f (sys) ppm ", A_msr, dAmsr_stat, dAmsr_syst);



  cout <<"\n"<<measuredAsymmetry1<<"\n"<<measuredAsymmetry2<<"\n"<<measuredAsymmetry3 
       <<"\n"<<measuredAsymmetry4<<"\n"<<measuredAsymmetry5<<"\n"<<measuredAsymmetry6 
       <<"\n"<<measuredAsymmetry7<<"\n"<<measuredAsymmetry8<<"\n"<<measuredAsymmetry9 
       <<"\n"<<measuredAsymmetry10<<"\n\n"<<measuredAsymmetry11<<"\n"<<measuredAsymmetry13
       <<"\n"<<measuredAsymmetry14<<endl;


  TString msrAsym, msrAsym1, msrAsym2, msrAsym3;
  TString msrAsym4, msrAsym5, msrAsym6, msrAsym7;
  TString msrAsym8, msrAsym9, msrAsym10, msrAsym11;
  TString msrAsym12, msrAsym13, msrAsym14, msrAsym15;

  msrAsym1  = Form("Blinded Measured Asymmetry and Errors (Combined Horizontal and Vertical):");
  msrAsym2  = Form("Amsr (unregressed)");
  msrAsym3  = Form("Amsr");
  msrAsym4  = Form("dAmsr_{stat}");
  msrAsym5  = Form("dAmsr_{P}");
  msrAsym6  = Form("dAmsr_{regscheme}");
  msrAsym7  = Form("dAmsr_{reg_time}");
  msrAsym8  = Form("dAmsr_{nonlin}");
  msrAsym9  = Form("dAmsr_{cuts}");
  msrAsym10 = Form("dAmsr_{q2_acceptance}");
  msrAsym11 = Form("dAmsr_{fit}");
  msrAsym12 = Form("dAmsr_{sys}");
  msrAsym13 = Form("dAmsr");
  msrAsym14 = Form("A_{M}^{in}");



  cout <<"\n"<<msrAsym1<<"\n"<<msrAsym2<<"\n"<<msrAsym3 
       <<"\n"<<msrAsym4<<"\n"<<msrAsym5<<"\n"<<msrAsym6 
       <<"\n"<<msrAsym7<<"\n"<<msrAsym8<<"\n"<<msrAsym9 
       <<"\n"<<msrAsym10<<"\n\n"<<msrAsym11<<"\n"<<msrAsym13
       <<"\n"<<msrAsym14<<endl;



  /*********************************************/

  TString polarization1,polarization2,polar2;

  polarization1  = Form("Beam polarization:");
  polarization2  = Form("P \t\t\t = %2.3f +- %2.3f ",P,dP);
  polar2         = Form("P");

  cout <<"\n"<<polarization1<<"\n"<<polarization2<<endl;


  /*********************************************/

  TString bkgTitleMain,bkgTitle1,bkgTitle2,bkgTitle3,bkgTitle4;
  TString bkgAsymmetry1,bkgAsymmetry2,bkgAsymmetry3,bkgAsymmetry4;
  TString bkgDilution1,bkgDilution2,bkgDilution3,bkgDilution4;

  bkgTitleMain   = Form("Background Asymmetries, Dilutions and Corrections:");

  bkgTitle1      = Form("Aluminum target windows:");
  bkgAsymmetry1  = Form("A_{b1} \t = %2.3f #pm %2.3f",A_b1,dA_b1);
  bkgDilution1   = Form("f_{b1} = %2.3f #pm %2.3f",f_b1,df_b1);

  bkgTitle2      = Form("Other neutral bkg:");
  bkgAsymmetry2  = Form("A_{b2} \t = %2.3f #pm %2.3f",A_b2,dA_b2);
  bkgDilution2   = Form("f_{b2} = %2.3f #pm %2.3f",f_b2,df_b2);

  //   bkgTitle3      = Form("Beamline bkg neutrals with W shutters installed:");
  bkgTitle3      = Form("Beamline scattering:");
  bkgAsymmetry3  = Form("A_{b3} \t = %2.3f #pm %2.3f",A_b3,dA_b3);
  bkgDilution3   = Form("f_{b3} = %2.3f #pm %2.3f",f_b3,df_b3);

  bkgTitle4      = Form("Elastics:");
  bkgAsymmetry4  = Form("A_{b4} \t = %2.3f #pm %2.3f",A_b4,dA_b4);
  bkgDilution4   = Form("f_{b4} = %2.3f #pm %2.3f",f_b4,df_b4);

  cout <<"\n"<<bkgTitleMain
       <<"\n"<<bkgTitle1<<"\n"<<bkgAsymmetry1<<"\t"<<bkgDilution1
       <<"\n"<<bkgTitle2<<"\n"<<bkgAsymmetry2<<"\t"<<bkgDilution2
       <<"\n"<<bkgTitle3<<"\n"<<bkgAsymmetry3<<"\t"<<bkgDilution3
       <<"\n"<<bkgTitle4<<"\n"<<bkgAsymmetry4<<"\t"<<bkgDilution4
       <<endl;


  /*********************************************/

  TString multiplicative_title,multiplicative_RC_title,multiplicative_RC,multiplicative_dRC;
  TString multiplicative_Det_title,multiplicative_Det,multiplicative_dDet;
  TString multiplicative_Bin_title,multiplicative_Bin,multiplicative_dBin;
  TString multiplicative_Q2_title,multiplicative_Q2,multiplicative_dQ2;

  multiplicative_title      = Form("Multiplicative Corrections:");

  multiplicative_RC_title   = Form("Radiative correction");
  multiplicative_RC         = Form("%2.3f",RadCorr);
  multiplicative_dRC        = Form("%2.3f",dRadCorr);
  multiplicative_Det_title  = Form("Detector bias");
  multiplicative_Det        = Form("%2.3f",DetCorr);
  multiplicative_dDet       = Form("%2.3f ",dDetCorr);
  multiplicative_Bin_title  = Form("Eective kinematics correction");
  multiplicative_Bin        = Form("%2.3f",BinCenterCorr);
  multiplicative_dBin       = Form("%2.3f",dBinCenterCorr);
  multiplicative_Q2_title   = Form("Q^{2} calibration");
  multiplicative_Q2         = Form("%2.3f",Q2Corr);
  multiplicative_dQ2        = Form("%2.3f",dQ2Corr);

  cout <<"\n"<<multiplicative_title
       <<"\n"<<multiplicative_RC_title<<"\n"<<multiplicative_RC<<" +- "<<multiplicative_dRC
       <<"\n"<<multiplicative_Det_title<<"\n"<<multiplicative_Det<<" +- "<<multiplicative_dDet
       <<"\n"<<multiplicative_Bin_title<<"\n"<<multiplicative_Bin<<" +- "<<multiplicative_dBin
       <<"\n"<<multiplicative_Q2_title<<"\n"<<multiplicative_Q2<<" +- "<<multiplicative_dQ2
       <<endl;

  /*********************************************/
  
  TString correctedAsymmetry1,correctedAsymmetry2,correctedAsymmetry3;

  correctedAsymmetry1  = Form("*********************Final Corrected Asymmetry***********************");
  correctedAsymmetry2  = Form("A_PHYS \t\t = %2.3f #pm %2.3f (stat) #pm %2.3f (syst) ppm ",A_phys,dAphys_stat,dAphys_sys);
  correctedAsymmetry3  = Form("(Blinding correction has subtracted  %2.3f ppm.)",BlindingCorrection);

  cout <<"\n"<<correctedAsymmetry1<<"\n"<<correctedAsymmetry2<<"\n"<<correctedAsymmetry3<<endl;

  /*********************************************/

  TString correction1,correction2,correction3,correction4,correction5,correction6;
  TString correction7,correction8,correction9,correction10,correction11;

  correction1  = Form("%s",line.Data());
  correction2  = Form("Breakdown of Corrections (neglects compounding):");
  correction3  = Form("Effective Polarization Correction \t = %2.3f +- %2.3f ppm ");
  correction4  = Form("Window Bkg Correction \t\t\t = %2.3f +- %2.3f ppm ");
  correction5  = Form("Effective Regression Correction \t = %2.3f +- %2.3f ppm ");
  correction6  = Form("Effective Transverse Correction \t = %2.3f +- %2.3f ppm ");
  correction7  = Form("QTOR Channel Neutral Bkg Correction \t = %2.3f +- %2.3f ppm ");
  correction8  = Form("Beamline Bkg Neutrals Correction \t = %2.3f +- %2.3f ppm ");
  correction9  = Form("Non-signal Electron Bkg Correction \t = %2.3f +- %2.3f ppm ");
  correction10 = Form("Effective Detector Bias Correction \t = %2.3f +- %2.3f ppm ");
  correction11 = Form("Effective Radiative Correction \t\t = %2.3f +- %2.3f ppm ");

  cout <<"\n"<<correction1<<"\n"<<correction2<<"\n"<<correction3 
       <<"\n"<<correction4<<"\n"<<correction5<<"\n"<<correction6 
       <<"\n"<<correction7<<"\n"<<correction8<<"\n"<<correction9 
       <<"\n"<<correction10<<"\n"<<correction11<<endl;

  /*********************************************/

  TString errorContribution1,errorContribution2,errorContribution3,errorContribution4;
  TString errorContribution5,errorContribution6,errorContribution7,errorContribution8;
  TString errorContribution9,errorContribution10,errorContribution11,errorContribution12;
  TString errorContribution13,errorContribution14,errorContribution15,errorContribution16;

  errorContribution1  = Form("Error contributions to final corrected asymmetry:");
  errorContribution2  = Form("dAmsr_stat \t: %2.3f ppm");
  errorContribution3  = Form("dAmsr_syst \t: %2.3f ppm");
  errorContribution4  = Form("dP \t\t: %2.3f ppm");
  errorContribution5  = Form("Aluminum alloy windows:");
  errorContribution6  = Form("dA1 \t\t: %2.3f ppm");
  errorContribution7  = Form("df1 \t\t: %2.3f ppm");
  errorContribution8  = Form("QTOR transport channel neutrals:");
  errorContribution9  = Form("dA2 \t\t: %2.3f ppm");
  errorContribution10 = Form("df2 \t\t: %2.3f ppm");
  errorContribution11 = Form("Beamline bkg neutrals with W shutters installed:");
  errorContribution12 = Form("dA3 \t\t: %2.3f ppm");
  errorContribution13 = Form("df3 \t\t: %2.3f ppm");
  errorContribution14 = Form("Non-signal electrons on detector:");
  errorContribution15 = Form("dA4 \t\t: %2.3f ppm");
  errorContribution16 = Form("df4 \t\t: %2.3f ppm");

  cout <<"\n"<<errorContribution1<<"\n"<<errorContribution2<<"\n"<<errorContribution3 
       <<"\n"<<errorContribution4<<"\n"<<errorContribution5<<"\n"<<errorContribution6 
       <<"\n"<<errorContribution7<<"\n"<<errorContribution8<<"\n"<<errorContribution9 
       <<"\n"<<errorContribution10<<"\n"<<errorContribution11<<"\n"<<errorContribution12 
       <<"\n"<<errorContribution13<<"\n"<<errorContribution14<<"\n"<<errorContribution15 
       <<"\n"<<errorContribution16<<endl;

  cout <<"\n"<<line<<endl;

  /*********************************************/
  TString misc_title;
  TString Q2_title,Q2_value,Q2_error;
  TString Theta_title,Theta_value,Theta_error;

  misc_title      = Form("Miscellaneous:");

  Q2_title      = Form("4-Momentum Transferd Square");
  Q2_value      = Form("%2.3f",Q2);
  Q2_error      = Form("%2.3f ",dQ2);

  Theta_title   = Form("Scattering Angle");
  Theta_value   = Form("%2.3f",Theta);
  Theta_error   = Form("%2.3f ",dTheta);


  cout<<"Q2 = "<<Q2<<"+-"<<dQ2<<endl;
  cout<<"Theta = "<<Theta<<"+-"<<dTheta<<endl;

  cout <<"\n"<<line<<endl;

  /*********************************************/
  //  Error contributions to final corrected asymmetry:
  //  dAmsr_stat :  0.1948 ppm
  //  dAmsr_syst :  0.0211 ppm
  //          dP :  0.0388 ppm
  //  Aluminum alloy windows:
  //         dA1 :  0.0000 ppm
  //         df1 :  0.0000 ppm
  //  QTOR transport channel neutrals:
  //         dA2 :  0.0163 ppm
  //         df2 :  0.0192 ppm
  //  Beamline bkg neutrals with W shutters installed:
  //         dA3 :  0.1855 ppm
  //         df3 :  0.0021 ppm
  //  Non-signal electrons on detector:
  //         dA4 :  0.0000 ppm
  //         df4 :  0.0000 ppm

  //   Canvas2->Update();

  MyfileFinal.open(textfileFinal);

  MyfileFinal <<"\n"<<line<<endl;

  MyfileFinal <<"\n"<<measuredAsymmetry1<<"\n"<<measuredAsymmetry2<<"\n"<<measuredAsymmetry3 
	      <<"\n"<<measuredAsymmetry4<<"\n"<<measuredAsymmetry5<<"\n"<<measuredAsymmetry6 
	      <<"\n"<<measuredAsymmetry7<<"\n"<<measuredAsymmetry8<<"\n"<<measuredAsymmetry9 
	      <<"\n"<<measuredAsymmetry10<<"\n\n"<<measuredAsymmetry11<<endl;

  MyfileFinal <<"\n"<<polarization1<<"\n"<<polarization2<<endl;

  MyfileFinal <<"\n"<<bkgTitleMain
	      <<"\n"<<bkgTitle1<<"\n"<<bkgAsymmetry1<<"\t"<<bkgDilution1
	      <<"\n"<<bkgTitle2<<"\n"<<bkgAsymmetry2<<"\t"<<bkgDilution2
	      <<"\n"<<bkgTitle3<<"\n"<<bkgAsymmetry3<<"\t"<<bkgDilution3
	      <<"\n"<<bkgTitle4<<"\n"<<bkgAsymmetry4<<"\t"<<bkgDilution4
	      <<endl;

  //   MyfileFinal <<"\n"<<background1<<"\n"<<background2<<"\n"<<background3 
  // 	      <<"\n"<<background4<<"\n"<<background5<<"\n"<<background6 
  // 	      <<"\n"<<background7<<"\n"<<background8<<"\n"<<background9<<endl;

  MyfileFinal <<"\n"<<multiplicative_title
	      <<"\n"<<multiplicative_RC_title<<"\n"<<multiplicative_RC<<" +- "<<multiplicative_dRC
	      <<"\n"<<multiplicative_Det_title<<"\n"<<multiplicative_Det<<" +- "<<multiplicative_dDet
	      <<"\n"<<multiplicative_Bin_title<<"\n"<<multiplicative_Bin<<" +- "<<multiplicative_dBin
	      <<endl;

//   MyfileFinal <<"\n"<<multiplicative1<<"\n"<<multiplicative2<<"\n"<<multiplicative3<<endl;

  MyfileFinal <<"\n"<<correctedAsymmetry1<<"\n"<<correctedAsymmetry2<<"\n"<<correctedAsymmetry3<<endl;

  MyfileFinal <<"\n"<<correction1<<"\n"<<correction2<<"\n"<<correction3 
	      <<"\n"<<correction4<<"\n"<<correction5<<"\n"<<correction6 
	      <<"\n"<<correction7<<"\n"<<correction8<<"\n"<<correction9 
	      <<"\n"<<correction10<<"\n"<<correction11<<endl;

  MyfileFinal <<"\n"<<errorContribution1<<"\n"<<errorContribution2<<"\n"<<errorContribution3 
	      <<"\n"<<errorContribution4<<"\n"<<errorContribution5<<"\n"<<errorContribution6 
	      <<"\n"<<errorContribution7<<"\n"<<errorContribution8<<"\n"<<errorContribution9 
	      <<"\n"<<errorContribution10<<"\n"<<errorContribution11<<"\n"<<errorContribution12 
	      <<"\n"<<errorContribution13<<"\n"<<errorContribution14<<"\n"<<errorContribution15 
	      <<"\n"<<errorContribution16<<endl;

  MyfileFinal <<"\n"<<line<<endl;

  MyfileFinal.close();



  /*****************************************************************************/
  /*                                    Plot                                   */
  /*****************************************************************************/


  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.5);
  
  //Pad parameters
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.8);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);

  //Set fonts
  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");
  
  //Delete all the objects stored in the current directory memmory
  gDirectory->Delete("*");

  if(SUMMARY_PLOT){

  TString title1;
  TString titleSummary = Form("(%s) Extraction of Physics Asym Using Regressed 5+1 MD PMTavg for Transverse %s"
			      ,targ.Data(),interaction2.Data());

  title1= Form("%s",titleSummary.Data());


  TCanvas * canvas1 = new TCanvas("canvas1", title1,0,0,canvasSize[0],canvasSize[1]);
  canvas1->Draw();
  canvas1->SetBorderSize(0);
  canvas1->cd();

  TPad*pad1 = new TPad("pad1","pad1",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad2 = new TPad("pad2","pad2",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad1->SetFillColor(kWhite);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();
  TString text = Form("%s",title1.Data());
  TLatex *t1 = new TLatex(0.02,0.3,text);
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->cd();

  gStyle->SetTextSize(0.02);


//   Double_t cor[2] = {0.25,1.25};
//   Double_t corGaph[3] = {0.03,0.35,0.25};



 // Draw the lower case letters
  TLatex TlA;
  TlA.SetTextAlign(12);
  float y, x1, x2, x3, x4, x5;
  y = 0.92; x1 = 0.02; x2 = x1+0.17; x3 = x1+0.34;  x4 = x1+0.38;

  TlA.DrawLatex(x1, y, Form("%s", msrAsym1.Data()));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym2.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm, \t Regression Correction \t = %2.3f ppm",A_msr_unreg,RegCorr));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym3.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm",A_msr));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s %s", polarization1.Data(),polar2.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f #pm %2.3f", P,dP)); 
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym4.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_stat));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym5.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_P));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym6.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_regscheme));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym7.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_reg_time));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym8.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_nonlin));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym9.Data())); TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_cuts));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym10.Data()));TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_q2_acceptance));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym11.Data()));TlA.DrawLatex(x2, y, Form("= %2.3f ppm", dAmsr_fit));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym12.Data()));TlA.DrawLatex(x2, y, Form("= %2.3f ppm (quadrature sum of all sys)", dAmsr_syst));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym13.Data()));TlA.DrawLatex(x2, y, Form("= %2.3f ppm (Total: stat + sys)", dAmsr));
  y -= 0.0250; TlA.DrawLatex(x1, y, Form("%s", msrAsym14.Data()));TlA.DrawLatex(x2, y, Form("= %2.3f #pm %2.3f #pm %2.3f ppm", A_msr,dAmsr_stat,dAmsr_syst));


  /*********************************************/

  // Draw the lower case letters
  TLatex Tl;
  Tl.SetTextAlign(12);
//   float y, x1, x2, x3, x4;
  x1 = 0.02; x2 = x1+0.15; x3 = x1+0.26;  x4 = x1+0.29; x5 = x1+0.46;
  y -= 0.0750;
  Tl.DrawLatex(x1, y, Form("%s",bkgTitleMain.Data()));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",bkgTitle1.Data())); Tl.SetTextColor(kGreen+2);
  Tl.DrawLatex(x2, y, Form("%s",bkgAsymmetry1.Data())); 
  Tl.DrawLatex(x3, y, Form("%s",unitPPM.Data())); Tl.SetTextColor(kOrange+7);
  Tl.DrawLatex(x4, y, Form("%s    c_{b1} = %2.3f",bkgDilution1.Data(),c_b1)); Tl.SetTextColor(kBlack);
  Tl.DrawLatex(x5, y, Form("DocDB 1819"));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",bkgTitle2.Data()));  Tl.SetTextColor(kRed);
  Tl.DrawLatex(x2, y, Form("%s",bkgAsymmetry2.Data())); 
  Tl.DrawLatex(x3, y, Form("%s",unitPPM.Data()));  Tl.SetTextColor(kOrange+7);
  Tl.DrawLatex(x4, y, Form("%s    c_{b2} = %2.3f",bkgDilution2.Data(),c_b2)); Tl.SetTextColor(kBlack);
  Tl.DrawLatex(x5, y, Form("Elog 714, DocDB 1549"));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",bkgTitle3.Data())); Tl.SetTextColor(kRed);
  Tl.DrawLatex(x2, y, Form("%s",bkgAsymmetry3.Data())); 
  Tl.DrawLatex(x3, y, Form("%s",unitPPM.Data())); 
  Tl.DrawLatex(x4, y, Form("%s    c_{b3} = %2.3f",bkgDilution3.Data(),c_b3)); Tl.SetTextColor(kBlack);
  Tl.DrawLatex(x5, y, Form("Elog 782, 784"));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",bkgTitle4.Data())); Tl.SetTextColor(kGreen+2);
  Tl.DrawLatex(x2, y, Form("%s",bkgAsymmetry4.Data()));
  Tl.DrawLatex(x3, y, Form("%s",unitPPM.Data())); Tl.SetTextColor(kOrange+7);
  Tl.DrawLatex(x4, y, Form("%s    c_{b4} = %2.3f",bkgDilution4.Data(),c_b4)); Tl.SetTextColor(kBlack);
  Tl.DrawLatex(x5, y, Form("DocDB 1601"));

  /*********************************************/
  x1 = 0.02; x2 = x1+0.15; x3 = x1+0.26;  x4 = x1+0.29; x5 = x1+0.46;
  y -= 0.0750; 
  Tl.DrawLatex(x1, y, Form("%s",multiplicative_title.Data()));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",multiplicative_RC_title.Data())); Tl.SetTextColor(kRed);
  Tl.DrawLatex(x2, y, Form("R_{RC} = %s #pm %s",multiplicative_RC.Data(),multiplicative_dRC.Data())); Tl.SetTextColor(kBlack); 
  Tl.DrawLatex(x5, y, Form("Place holder"));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",multiplicative_Det_title.Data())); Tl.SetTextColor(kRed);
  Tl.DrawLatex(x2, y, Form("R_{Det} = %s #pm %s",multiplicative_Det.Data(),multiplicative_dDet.Data())); Tl.SetTextColor(kBlack); 
  Tl.DrawLatex(x5, y, Form("Place holder"));
  y -= 0.0250;
  Tl.DrawLatex(x1, y, Form("%s",multiplicative_Bin_title.Data())); Tl.SetTextColor(kRed);
  Tl.DrawLatex(x2, y, Form("R_{Bin} = %s #pm %s",multiplicative_Bin.Data(),multiplicative_dBin.Data())); Tl.SetTextColor(kBlack); 
  Tl.DrawLatex(x5, y, Form("Place holder"));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",multiplicative_Q2_title.Data())); Tl.SetTextColor(kRed);
  Tl.DrawLatex(x2, y, Form("R_{Q2} = %s #pm %s",multiplicative_Q2.Data(),multiplicative_dQ2.Data())); Tl.SetTextColor(kBlack); 
  Tl.DrawLatex(x5, y, Form("Place holder"));

  /*********************************************/

  x1 = 0.02; x2 = x1+0.155; x3 = x1+0.26;  x4 = x1+0.29; x5 = x1+0.46;
  y -= 0.0750; 
  Tl.DrawLatex(x1, y, Form("%s",misc_title.Data()));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",Q2_title.Data())); Tl.SetTextColor(kOrange+7);
  Tl.DrawLatex(x2, y, Form("Q^{2} = %s #pm %s (GeV/c)^{2}",Q2_value.Data(),Q2_error.Data())); Tl.SetTextColor(kBlack); 
  Tl.DrawLatex(x5, y, Form("Elog XXX"));
  y -= 0.0250; 
  Tl.DrawLatex(x1, y, Form("%s",Theta_title.Data())); Tl.SetTextColor(kOrange+7);
  Tl.DrawLatex(x2, y, Form("#theta   = %s #pm %s degree",Theta_value.Data(),Theta_error.Data())); Tl.SetTextColor(kBlack); 
  Tl.DrawLatex(x5, y, Form("Elog XXX"));

  /*********************************************/


//   y = 0.90; x1 = 0.61; x2 = x1+0.18; x3 = x1+0.30;  x4 = x1+0.34; x5 = x1+0.45;

//   TLatex * tPhysAsym = new TLatex(x1,y,Form("A_{PHYS}^{in} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %2.3f #pm %2.3f",A_phys,dAphys));

//   tPhysAsym->Draw();
//   tPhysAsym->SetTextColor(kRed);


//   gStyle->SetTextSize(0.018);

//   y -= 0.0650;
//   TLatex * tdPhysAsym_Amsr = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{M}^{in}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #frac{dA_{M}^{in}}{P} #left[ #frac{1}{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Amsr));
//   y -= 0.0450;
//   TLatex * tdPhysAsym_P = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{P} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #frac{A_{M}^{in}}{P} #frac{dP}{P} #left[ #frac{1}{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_P));
//   y -= 0.0450;
//   TLatex * tdPhysAsym_Ab1 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b1}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ - dA_{b1}f_{b1} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Ab1));
//   y -= 0.0450;
//   TLatex * tdPhysAsym_Ab2 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b2}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ - dA_{b2}f_{b2} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Ab2));
//   y -= 0.0450;
//   TLatex * tdPhysAsym_Ab3 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b3}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ - dA_{b3}f_{b3} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Ab3));
//   y -= 0.0450;
//   TLatex * tdPhysAsym_Ab4 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b4}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ - dA_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Ab4));

//   y -= 0.0750;
//   TLatex * tdPhysAsym_fb1 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b1}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} df_{b1} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}(1 - f_{b2} - f_{b3} - f_{b4}) - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{ (1 - f_{b1} - f_{b2} - f_{b3} - f_{b4})^{2} } #right] = %4.3f ",dAphys_fb1));
//   y -= 0.0750;
//   TLatex * tdPhysAsym_fb2 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b2}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} df_{b2} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}(1 - f_{b1} - f_{b3} - f_{b4}) - A_{b3}f_{b3} - A_{b4}f_{b4} }{ (1 - f_{b1} - f_{b2} - f_{b3} - f_{b4})^{2} } #right] = %4.3f ",dAphys_fb2));
//   y -= 0.0750;
//   TLatex * tdPhysAsym_fb3 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b3}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} df_{b3} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}(1 - f_{b1} - f_{b2} - f_{b3}) - A_{b4}f_{b4} }{ (1 - f_{b1} - f_{b2} - f_{b3} - f_{b4})^{2} } #right] = %4.3f ",dAphys_fb3));
//   y -= 0.0750;
//   TLatex * tdPhysAsym_fb4 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b4}} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} df_{b4} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}(1 - f_{b1} - f_{b2} - f_{b3}) }{ (1 - f_{b1} - f_{b2} - f_{b3} - f_{b4})^{2} } #right] = %4.3f ",dAphys_fb4));

//   y -= 0.0650;
//   TLatex * tdPhysAsym_RC = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{RC}} =  dR_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_RC));
//   y -= 0.0650;
//   TLatex * tdPhysAsym_Det = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{Det}} =  R_{RC}dR_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Det));
//   y -= 0.0650;
//   TLatex * tdPhysAsym_Bin = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{Bin}} =  R_{RC}R_{Det}dR_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Bin));
//   y -= 0.0650;
//   TLatex * tdPhysAsym_Q2 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{Q^{2}}} =  R_{RC}R_{Det}R_{Bin}dR_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %4.3f ",dAphys_Q2));

  y = 0.90; x1 = 0.61; x2 = x1+0.23; x3 = x1+0.30;  x4 = x1+0.34; x5 = x1+0.45;
   TLatex * tPhysAsym = new TLatex(x1,y,Form("A_{PHYS}^{in} =  R_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{b1} - f_{b2} - f_{b3} - f_{b4}} #right] = %2.3f #pm %2.3f",A_phys,dAphys));
//   TLatex * tPhysAsym = new TLatex(x1,y,Form("A_{PHYS}^{in} =  R_{Total} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{Total}} #right] = %2.3f #pm %2.3f",A_phys,dAphys));

  tPhysAsym->Draw();
  tPhysAsym->SetTextColor(kRed);

  gStyle->SetTextSize(0.018);

  y -= 0.0650;
  TLatex * tdPhysAsym_Amsr = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{M}^{in}} =  R_{Total} #frac{dA_{M}^{in}}{P} #left[ #frac{1}{1 - f_{Total}} #right] = %4.3f ",dAphys_Amsr));
  y -= 0.0450;
  TLatex * tdPhysAsym_P = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{P} =  R_{Total} #frac{A_{M}^{in}}{P} #frac{dP}{P} #left[ #frac{1}{1 - f_{Total}} #right] = %4.3f ",dAphys_P));
  y -= 0.0450;
  TLatex * tdPhysAsym_Ab1 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b1}} =  R_{Total} #left[ #frac{ - dA_{b1}f_{b1} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Ab1));
  y -= 0.0450;
  TLatex * tdPhysAsym_Ab2 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b2}} =  R_{Total} #left[ #frac{ - dA_{b2}f_{b2} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Ab2));
  y -= 0.0450;
  TLatex * tdPhysAsym_Ab3 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b3}} =  R_{Total} #left[ #frac{ - dA_{b3}f_{b3} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Ab3));
  y -= 0.0450;
  TLatex * tdPhysAsym_Ab4 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{A_{b4}} =  R_{Total} #left[ #frac{ - dA_{b4}f_{b4} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Ab4));

  y -= 0.0750;
  TLatex * tdPhysAsym_fb1 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b1}} =  R_{Total} df_{b1} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}(1 - f_{b2} - f_{b3} - f_{b4}) - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{ (1 - f_{Total})^{2} } #right] = %4.3f ",dAphys_fb1));
  y -= 0.0750;
  TLatex * tdPhysAsym_fb2 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b2}} =  R_{Total} df_{b2} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}(1 - f_{b1} - f_{b3} - f_{b4}) - A_{b3}f_{b3} - A_{b4}f_{b4} }{ (1 - f_{Total})^{2} } #right] = %4.3f ",dAphys_fb2));
  y -= 0.0750;
  TLatex * tdPhysAsym_fb3 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b3}} =  R_{Total} df_{b3} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}(1 - f_{b1} - f_{b2} - f_{b3}) - A_{b4}f_{b4} }{ (1 - f_{Total})^{2} } #right] = %4.3f ",dAphys_fb3));
  y -= 0.0750;
  TLatex * tdPhysAsym_fb4 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{f_{b4}} =  R_{Total} df_{b4} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}(1 - f_{b1} - f_{b2} - f_{b3}) }{ (1 - f_{Total})^{2} } #right] = %4.3f ",dAphys_fb4));

  y -= 0.0650;
  TLatex * tdPhysAsym_RC = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{RC}} =  dR_{RC}R_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{Total}} #right] = %4.3f ",dAphys_RC));
  y -= 0.0650;
  TLatex * tdPhysAsym_Det = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{Det}} =  R_{RC}dR_{Det}R_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Det));
  y -= 0.0650;
  TLatex * tdPhysAsym_Bin = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{Bin}} =  R_{RC}R_{Det}dR_{Bin}R_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Bin));
  y -= 0.0650;
  TLatex * tdPhysAsym_Q2 = new TLatex(x1,y,Form("(dA_{PHYS}^{in})_{R_{Q^{2}}} =  R_{RC}R_{Det}R_{Bin}dR_{Q^{2}} #left[ #frac{ #frac{A_{M}^{in}}{P} - A_{b1}f_{b1} - A_{b2}f_{b2} - A_{b3}f_{b3} - A_{b4}f_{b4} }{1 - f_{Total}} #right] = %4.3f ",dAphys_Q2));

  tdPhysAsym_Amsr->Draw();
  tdPhysAsym_P->Draw();
  tdPhysAsym_Ab1->Draw();
  tdPhysAsym_Ab2->Draw();
  tdPhysAsym_Ab3->Draw();
  tdPhysAsym_Ab4->Draw();

  tdPhysAsym_fb1->Draw();
  tdPhysAsym_fb2->Draw();
  tdPhysAsym_fb3->Draw();
  tdPhysAsym_fb4->Draw();

  tdPhysAsym_RC->Draw();
  tdPhysAsym_Det->Draw();
  tdPhysAsym_Bin->Draw();
  tdPhysAsym_Q2->Draw();

  y += 0.8950;
  TLatex * tPhysAsymRDefination = new TLatex(x2,y-0.1,Form("R_{Total} = R_{RC}R_{Det}R_{Bin}R_{Q^{2}}"));
  TLatex * tPhysAsymfDefination = new TLatex(x2,y-0.135,Form("f_{Total} = f_{b1} + f_{b2} + f_{b3} + f_{b4}"));
  tPhysAsymRDefination->Draw();
  tPhysAsymfDefination->Draw();


  gPad->Update();

  TString saveSummaryPlot = Form("dirPlot/resultPlot/%s_%s_%s_MD_Final_%s"
				 ,interaction.Data(),qtor_stem.Data(),target.Data(),database_stem.Data());

  canvas1->Update();
  canvas1->Print(saveSummaryPlot+".png");
  if(FIGURE){
    canvas1->Print(saveSummaryPlot+".svg");
    canvas1->Print(saveSummaryPlot+".C");
  }

  }


  if(ERROR_PLOT){

  const Int_t n = 9;
//   Double_t xn[n], yn[n];

  Double_t x_n[n] = {1,2,3,4,5,6,7,8,9};
  Double_t x2_n[n] = {0.85,1.93,2.55,3.70,4.65,5.85,6.50,7.65,8.80};

  Double_t y_n[n] = {dAmsr_stat,dAmsr_P,dAmsr_regscheme,dAmsr_reg_time,dAmsr_nonlin,dAmsr_cuts,dAmsr_q2_acceptance,dAmsr_fit,dAmsr};
  Double_t y_n_h[n] = {dAmsr_stat_h,dAmsr_P_h,dAmsr_regscheme_h,dAmsr_reg_time_h,dAmsr_nonlin_h,dAmsr_cuts_h,dAmsr_q2_acceptance_h,dAmsr_fit_h,dAmsr_h};
  Double_t y_n_v[n] = {dAmsr_stat_v,dAmsr_P_v,dAmsr_regscheme_v,dAmsr_reg_time_v,dAmsr_nonlin_v,dAmsr_cuts_v,dAmsr_q2_acceptance_v,dAmsr_fit_v,dAmsr_v};

  TString title_n[n] = {"stat","P","reg. scheme","reg. time","nonlinearity","cuts","Q^{2} acceptance","fit scheme","Total"};

  Double_t yScale[2] = {0.0,0.95};

  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.10);
  gStyle->SetStatH(0.3);

  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatColor(0);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.06);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");
  gStyle->SetNdivisions(000,"x");

  // histo parameters
  gStyle->SetTitleYOffset(0.75);
  gStyle->SetTitleXOffset(0.90);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gStyle->SetBarWidth(0.5); // set the bar width of the bar plot
  //  gStyle->SetBarOffset(0.4);

  gDirectory->Delete("*");


  TString title2;
  TString titleError = Form("(%s) Summary of Uncertainties for Transverse %s",targ.Data(),interaction2.Data());
  title2= Form("%s",titleError.Data());

  TCanvas * canvas2 = new TCanvas("canvas2", title2,0,0,canvasSize[2],canvasSize[3]);
  canvas2->Draw();
  canvas2->SetBorderSize(0);
  canvas2->cd();
  TPad*pad21 = new TPad("pad21","pad21",pad1x[0],pad1y[0],pad1x[1],pad1y[1]);
  TPad*pad22 = new TPad("pad22","pad22",pad2x[0],pad2y[0],pad2x[1],pad2y[1]);
  pad21->SetFillColor(kWhite);
  pad21->Draw();
  pad22->Draw();
  pad21->cd();
  TString text2 = Form("%s",title2.Data());
  TLatex *t2 = new TLatex(0.10,0.3,text2);
  t2->SetTextSize(0.5);
  t2->Draw();
  pad22->cd();
  pad22->Divide(1,3);

  //  pad22->cd(1)->SetGridy();
  pad22->cd(1);
  TGraph * gr_h = new TGraph(n,x_n,y_n_h);
  gr_h->SetFillColor(kRed-7);
  gr_h->SetFillStyle(3007);
  gr_h->SetTitle("");
  gr_h->GetXaxis()->CenterTitle();
  gr_h->GetYaxis()->SetTitle(Form("Uncertainty [ppm]"));
  gr_h->GetYaxis()->CenterTitle();
  gr_h->Draw("AB");
  gr_h->GetXaxis()->SetLabelColor(0);
  gr_h->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  TLatex* tText_h = new TLatex(4.0,yScale[1]*0.85,Form("Horizontal Transverse"));
  tText_h->SetTextSize(0.075);
  tText_h->SetTextColor(kRed-4);
  tText_h->Draw();
//   y -= 0.0450;
  for(Int_t i=0; i<n; i++){
  TLatex* tAxis = new TLatex(x2_n[i],-0.10,Form("%s",title_n[i].Data()));
  tAxis->SetTextSize(0.055);
  tAxis->SetTextAngle(0);
  tAxis->Draw();
  TLatex* tValue_h = new TLatex(x_n[i]-0.19,y_n_h[i]+0.02,Form("%0.3f",y_n_h[i]));
  tValue_h->SetTextSize(0.055);
  tValue_h->SetTextAngle(0);
  tValue_h->Draw();
  }

  pad22->cd(2);
  TGraph * gr_v = new TGraph(n,x_n,y_n_v);
  gr_v->SetFillColor(kBlue-7);
  gr_v->SetFillStyle(3006);
  gr_v->SetTitle("");
  gr_v->GetXaxis()->CenterTitle();
  gr_v->GetYaxis()->SetTitle(Form("Uncertainty [ppm]"));
  gr_v->GetYaxis()->CenterTitle();
  gr_v->Draw("AB");
  gr_v->GetXaxis()->SetLabelColor(0);
  gr_v->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  TLatex* tText_v = new TLatex(4.2,yScale[1]*0.85,Form("Vertical Transverse"));
  tText_v->SetTextSize(0.075);
  tText_v->SetTextColor(kBlue-4);
  tText_v->Draw();
  for(Int_t i=0; i<n; i++){
  TLatex* tAxis = new TLatex(x2_n[i],-0.10,Form("%s",title_n[i].Data()));
  tAxis->SetTextSize(0.055);
  tAxis->SetTextAngle(0);
  tAxis->Draw();
  TLatex* tValue_v=new TLatex(x_n[i]-0.19,y_n_v[i]+0.02,Form("%0.3f",y_n_v[i]));
  tValue_v->SetTextSize(0.055);
  tValue_v->SetTextAngle(0);
  tValue_v->Draw();
  }


  pad22->cd(3);
  TGraph * gr = new TGraph(n,x_n,y_n);
  gr->SetFillColor(kGreen+1);
  //gr->SetFillStyle(3001);
  gr->SetTitle("");
  gr->GetXaxis()->CenterTitle();
  gr->GetYaxis()->SetTitle(Form("Uncertainty [ppm]"));
  gr->GetYaxis()->CenterTitle();
  gr->Draw("AB");
  gr->GetXaxis()->SetLabelColor(0);
  gr->GetYaxis()->SetRangeUser(yScale[0],yScale[1]);
  TLatex* tText = new TLatex(3.5,yScale[1]*0.85,Form("Combined Horizontal & Vertical"));
  tText->SetTextSize(0.075);
  tText->SetTextColor(kGreen+2);
  tText->Draw();
  for(Int_t i=0; i<n; i++){
  TLatex* tAxis = new TLatex(x2_n[i],-0.10,Form("%s",title_n[i].Data()));
  tAxis->SetTextSize(0.055);
  tAxis->SetTextAngle(0);
  tAxis->Draw();
  TLatex* tValue = new TLatex(x_n[i]-0.19,y_n[i]+0.02,Form("%0.3f",y_n[i]));
  tValue->SetTextSize(0.055);
  tValue->SetTextAngle(0);
  tValue->Draw();
  }


  TString errorPlot = Form("dirPlot/resultPlot/%s_%s_%s_MD_Error_Chart_%s"
				 ,interaction.Data(),qtor_stem.Data(),target.Data(),database_stem.Data());
  
  canvas2->Update();
  canvas2->Print(errorPlot+".png");
  if(FIGURE){
    canvas2->Print(errorPlot+".svg");
    canvas2->Print(errorPlot+".C");
  }


  }

  /*****************************************************************************/
  /*****************************************************************************/
  /*****************************************************************************/

  printf("\n%sDone with everything. Do CTRL+C to exit%s\n",red,normal);

  theApp.Run();
  return(1);
}


