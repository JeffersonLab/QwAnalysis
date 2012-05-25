#include <NurClass2.h>

void n2delta(){

  Bool_t RUNLET_PLOT = kTRUE; Bool_t RUNLET_PLOT_DB = kTRUE; Bool_t RUNLET_PLOT_RUNLET_COR = kTRUE; Bool_t RUNLET_PLOT_SLUG_COR = kTRUE;
  Bool_t RUNLET_PLOT_SLUGAVG_COR = kTRUE; Bool_t RUNLET_PLOT_WIDTH = kTRUE; Bool_t RUNLET_PLOT_WIDTH_DIFF = kTRUE;
  Bool_t RUN_PLOT = kTRUE; Bool_t RUN_PLOT_DB = kTRUE; Bool_t RUN_PLOT_RUN_COR = kTRUE; Bool_t RUN_PLOT_SLUG_COR = kTRUE; Bool_t RUN_PLOT_SLUGAVG_COR = kTRUE;
  Bool_t RUNLET_SENSITIVITY_PLOT = kTRUE; Bool_t RUNLET_CORRELATION_PLOT = kTRUE; Bool_t RUNLET_DIFF_PLOT = kTRUE; Bool_t RUNLET_CORRECTION_PLOT = kTRUE;
  Bool_t RUN_SENSITIVITY_PLOT = kTRUE; Bool_t RUN_CORRELATION_PLOT = kTRUE; Bool_t RUN_DIFF_PLOT = kTRUE; Bool_t RUN_CORRECTION_PLOT = kTRUE;
  Bool_t OCTANT_VARIATION = kTRUE; Bool_t OCTANT_VARIATION_SEN = kTRUE; Bool_t OCTANT_VARIATION_COR = kTRUE; Bool_t OCTANT_VARIATION_COR_DUB = kTRUE;
  Bool_t OCTANT_PLOT = kTRUE; Bool_t OCTANT_PLOT_INOUT = kTRUE; Bool_t SUMMARY_PLOT = kTRUE;
  Bool_t PROFILE_PLOT = kTRUE; Bool_t RUNLET_CORRELATION_PLOT2 = kTRUE;

  /////////////////////////////////////////////////////////////////////

//   Bool_t RUNLET_PLOT = kFALSE;
//   Bool_t RUNLET_PLOT_DB = kFALSE;
//   Bool_t RUNLET_PLOT_RUNLET_COR = kFALSE;
//   Bool_t RUNLET_PLOT_SLUG_COR = kFALSE;
//   Bool_t RUNLET_PLOT_SLUGAVG_COR = kFALSE;
//   Bool_t RUNLET_PLOT_WIDTH = kFALSE;
//   Bool_t RUNLET_PLOT_WIDTH_DIFF = kFALSE;

  Bool_t RUN_PLOT = kFALSE;
  Bool_t RUN_PLOT_DB = kFALSE;
  Bool_t RUN_PLOT_RUN_COR = kFALSE;
  Bool_t RUN_PLOT_SLUG_COR = kFALSE;
  Bool_t RUN_PLOT_SLUGAVG_COR = kFALSE;

  Bool_t RUNLET_SENSITIVITY_PLOT = kFALSE;
  Bool_t RUNLET_CORRELATION_PLOT = kFALSE;
  Bool_t RUNLET_DIFF_PLOT = kFALSE;
  Bool_t RUNLET_CORRECTION_PLOT = kFALSE;

  Bool_t RUN_SENSITIVITY_PLOT = kFALSE;
  Bool_t RUN_CORRELATION_PLOT = kFALSE;
  Bool_t RUN_DIFF_PLOT = kFALSE;
  Bool_t RUN_CORRECTION_PLOT = kFALSE;

  Bool_t OCTANT_VARIATION = kFALSE;
  Bool_t OCTANT_VARIATION_SEN = kFALSE;
  Bool_t OCTANT_VARIATION_COR = kFALSE;
  Bool_t OCTANT_VARIATION_COR_DUB = kFALSE;

  Bool_t OCTANT_PLOT = kFALSE;
  Bool_t OCTANT_PLOT_INOUT = kFALSE;

  Bool_t PROFILE_PLOT = kFALSE;
  Bool_t RUNLET_CORRELATION_PLOT2 = kFALSE;

  Bool_t SUMMARY_PLOT = kFALSE;

  /////////////////////////////////////////////////////////////////////

  Int_t deviceID=0,schemeID=0;

  deviceID = 0;
  schemeID = 0;

  char savePlot[255],plotTitle[255],dataInfo[255],regScheme[255],regTitle[255];
  Double_t y_scale[2],y_scaleW[2],y_scale_off[2],y_scaleW_off[2];
  
  sprintf(dataInfo,"Run-I Slug 3 & 4"); 

  switch(deviceID)
    {
    case    0: sprintf(savePlot,"MD");     sprintf(plotTitle,"MD");      y_scale[0]=-10;y_scale[1]=10; y_scaleW[0]=655;y_scaleW[1]=675; break;
    case    1: sprintf(savePlot,"DSLumi"); sprintf(plotTitle,"DS Lumi"); y_scale[0]=-05;y_scale[1]=25; y_scaleW[0]=180;y_scaleW[1]=350; break;
    case    2: sprintf(savePlot,"USLumi"); sprintf(plotTitle,"US Lumi"); y_scale[0]=-20;y_scale[1]=20; y_scaleW[0]=500;y_scaleW[1]=1200; break;
    default  : printf("%sPlease Insert correct Detectors information. %sExiting program !!!%s\n",blue,red,normal); exit(1); break;
    }

  switch(deviceID)
    {
    case    0: y_scale_off[0]=2;y_scale_off[1]=3.5; y_scaleW_off[0]=10;y_scaleW_off[1]=25; break;
    case    1: y_scale_off[0]=2;y_scale_off[1]=4.0; y_scaleW_off[0]=10;y_scaleW_off[1]=20; break;
    case    2: y_scale_off[0]=2;y_scale_off[1]=4.0; y_scaleW_off[0]=1.0;y_scaleW_off[1]=3.0; break;
    default  : printf("%sPlease Insert correct Detectors information. %sExiting program !!!%s\n",blue,red,normal); exit(1); break;
    }

  switch(schemeID)
    { 
    case    0: sprintf(regScheme,"Reg5");    sprintf(regTitle,"5 Par.Reg: ");   break;
    case    1: sprintf(regScheme,"Reg5p1");  sprintf(regTitle,"5+1 Par.Reg: "); break;
    case    2: sprintf(regScheme,"RegSet3"); sprintf(regTitle,"Set-3.Reg: ");   break;
    case    3: sprintf(regScheme,"RegSet4"); sprintf(regTitle,"Set-4.Reg: ");   break;
    case    4: sprintf(regScheme,"RegSet5"); sprintf(regTitle,"Set-5.Reg: ");   break;
    case    5: sprintf(regScheme,"RegSet6"); sprintf(regTitle,"Set-6.Reg: ");   break;
    case    6: sprintf(regScheme,"RegSet7"); sprintf(regTitle,"Set-7.Reg: ");   break;
    case    7: sprintf(regScheme,"RegSet8"); sprintf(regTitle,"Set-8.Reg: ");   break;
    case    8: sprintf(regScheme,"RegSet9"); sprintf(regTitle,"Set-9.Reg: ");   break;
    default  : printf("%sPlease Insert correct Regression Information. %sExiting program !!!%s\n",blue,red,normal); exit(1); break;
    }

  printf("%sStarting analysis for N-2-Delta %s for %s with %s !!!%s\n",red,dataInfo,plotTitle,regTitle,normal);
  TString n2deltaAnalysisDir = "/w/hallc/qweak/nur/n2delta";

  /*************************************************/

  Int_t csizx,csizy,csiz2x,csiz2y,csiz3x,csiz3y,cx1,cy1,cx2,cx3;
  csizx=1300;csizy=900;csiz3x=1100;csiz3y=780;cx1=0;cy1=0;cx2=300;cx3=600;
  Double_t tsiz,tsiz3,tsiz4,tsiz5,tll,tlr,ps1,ps2,ps3,ps4,ps5;
  //   tsiz=0.45;tsiz3=0.40;tsiz4=0.35;tsiz5=0.30;tll=0.012;tlr=0.4;ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;ps5=0.99;
  tsiz=0.45;tsiz3=0.50;tsiz4=0.35;tsiz5=0.30;tll=0.012;tlr=0.4;ps1=0.005;ps2=0.935;ps3=0.945;ps4=0.995;ps5=0.99;

  Double_t x_lo_stat_in,y_lo_stat_in,x_hi_stat_in,y_hi_stat_in,x_lo_stat_out,y_lo_stat_out,x_hi_stat_out,y_hi_stat_out;
  //   x_lo_stat_in=0.61;y_lo_stat_in=0.74;x_hi_stat_in=0.80;y_hi_stat_in=0.99;
  //   x_lo_stat_out=0.80;y_lo_stat_out=0.74;x_hi_stat_out=0.99;y_hi_stat_out=0.99;
  x_lo_stat_in=0.8;y_lo_stat_in=0.7;x_hi_stat_in=0.99;y_hi_stat_in=0.95;
  x_lo_stat_out=0.8;y_lo_stat_out=0.44;x_hi_stat_out=0.99;y_hi_stat_out=0.69;
  Double_t x_lo_stat_in2,y_lo_stat_in2,x_hi_stat_in2,y_hi_stat_in2,x_lo_stat_out2,y_lo_stat_out2,x_hi_stat_out2,y_hi_stat_out2;
//   x_lo_stat_in2=0.10;y_lo_stat_in2=0.85;x_hi_stat_in2=0.35;y_hi_stat_in2=0.99;
//   x_lo_stat_out2=0.35;y_lo_stat_out2=0.85;x_hi_stat_out2=0.60;y_hi_stat_out2=0.99;
  x_lo_stat_in2=0.16;y_lo_stat_in2=0.80;x_hi_stat_in2=0.41;y_hi_stat_in2=0.99;
  x_lo_stat_out2=0.41;y_lo_stat_out2=0.80;x_hi_stat_out2=0.66;y_hi_stat_out2=0.99;
  Double_t x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg,x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2,x_lo_leg3,y_lo_leg3,x_hi_leg3,y_hi_leg3;
  //   x_lo_leg=0.43;y_lo_leg=0.74;x_hi_leg=0.61;y_hi_leg=0.99;x_lo_leg2=0.38;y_lo_leg2=0.74;x_hi_leg2=0.61;y_hi_leg2=0.99;
  x_lo_leg=0.8;y_lo_leg=0.16;x_hi_leg=0.99;y_hi_leg=0.43;
//   x_lo_leg2=0.60;y_lo_leg2=0.85;x_hi_leg2=0.94;y_hi_leg2=0.99;
  x_lo_leg2=0.66;y_lo_leg2=0.80;x_hi_leg2=0.98;y_hi_leg2=0.99;
  x_lo_leg3=0.70;y_lo_leg3=0.85;x_hi_leg3=0.94;y_hi_leg3=0.99;

  Double_t x_lo_stat_in4,y_lo_stat_in4,x_hi_stat_in4,y_hi_stat_in4,x_lo_stat_out4,y_lo_stat_out4,x_hi_stat_out4,y_hi_stat_out4;
  x_lo_stat_in4=0.80;y_lo_stat_in4=0.64;x_hi_stat_in4=0.99;y_hi_stat_in4=0.95;
  x_lo_stat_out4=0.80;y_lo_stat_out4=0.32;x_hi_stat_out4=0.99;y_hi_stat_out4=0.63;
  Double_t x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4;
  x_lo_leg4=0.80;y_lo_leg4=0.10;x_hi_leg4=0.99;y_hi_leg4=0.31;

  /*************************************************/
  char titleMean[255];
  sprintf(titleMean,Form("%sN to Delta Measurement for %s using Pass3 5 Parameter Regression",dataInfo));


  Double_t det_range[2],fit_range[2],run_range[2],run_fit_range[2];
  det_range[0] = 0.5; det_range[1] = 8.5;
  fit_range[0] = 0.8; fit_range[1] = 8.2;
  run_range[0] = 11495.0; run_range[1] = 11515.0;
  run_fit_range[0] = 11495.5; run_fit_range[1] = 11514.5;

  TF1* fitRunGraphUnRegIn1  =  new TF1("fitRunGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphUnRegOut1 =  new TF1("fitRunGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegIn1  =  new TF1("fitRunGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegOut1 =  new TF1("fitRunGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegIn2  =  new TF1("fitRunGraphRegIn2", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegOut2 =  new TF1("fitRunGraphRegOut2","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegIn3  =  new TF1("fitRunGraphRegIn3", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegOut3 =  new TF1("fitRunGraphRegOut3","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegIn4  =  new TF1("fitRunGraphRegIn4", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphRegOut4 =  new TF1("fitRunGraphRegOut4","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphCorIn1  =  new TF1("fitRunGraphCorIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunGraphCorOut1 =  new TF1("fitRunGraphCorOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitRunAnaGraphUnRegIn1  =  new TF1("fitRunAnaGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunAnaGraphUnRegOut1 =  new TF1("fitRunAnaGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunAnaGraphRegIn1  =  new TF1("fitRunAnaGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunAnaGraphRegOut1 =  new TF1("fitRunAnaGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitSlugGraphUnRegIn1  =  new TF1("fitSlugGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugGraphUnRegOut1 =  new TF1("fitSlugGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugGraphRegIn1  =  new TF1("fitSlugGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugGraphRegOut1 =  new TF1("fitSlugGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugGraphCorIn1  =  new TF1("fitSlugGraphCorIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugGraphCorOut1 =  new TF1("fitSlugGraphCorOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugAvgGraphCorIn1  =  new TF1("fitSlugAvgGraphCorIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugAvgGraphCorOut1 =  new TF1("fitSlugAvgGraphCorOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitSlugAnaGraphUnRegIn1  =  new TF1("fitSlugAnaGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugAnaGraphUnRegOut1 =  new TF1("fitSlugAnaGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugAnaGraphRegIn1  =  new TF1("fitSlugAnaGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugAnaGraphRegOut1 =  new TF1("fitSlugAnaGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitWNormUnRegGraphIn1  =  new TF1("fitWNormUnRegGraphIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWNormUnRegGraphOut1  =  new TF1("fitWNormUnRegGraphOut1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWNormRegGraphIn1  =  new TF1("fitWNormRegGraphIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWNormRegGraphOut1  =  new TF1("fitWNormRegGraphOut1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWUnRegGraphIn1  =  new TF1("fitWUnRegGraphIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWUnRegGraphOut1  =  new TF1("fitWUnRegGraphOut1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWRegGraphIn1  =  new TF1("fitWRegGraphIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWRegGraphOut1  =  new TF1("fitWRegGraphOut1", "pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitWDiffGraphIn1  =  new TF1("fitWDiffGraphIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWDiffGraphOut1  =  new TF1("fitWDiffGraphOut1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWNormDiffGraphIn1  =  new TF1("fitWNormDiffGraphIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitWNormDiffGraphOut1  =  new TF1("fitWNormDiffGraphOut1", "pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitRunSenXGraphUnRegIn1  =  new TF1("fitRunSenXGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenXGraphUnRegOut1 =  new TF1("fitRunSenXGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenXSlopeGraphUnRegIn1  =  new TF1("fitRunSenXSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenXSlopeGraphUnRegOut1 =  new TF1("fitRunSenXSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenYGraphUnRegIn1  =  new TF1("fitRunSenYGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenYGraphUnRegOut1 =  new TF1("fitRunSenYGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenYSlopeGraphUnRegIn1  =  new TF1("fitRunSenYSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenYSlopeGraphUnRegOut1 =  new TF1("fitRunSenYSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenEGraphUnRegIn1  =  new TF1("fitRunSenEGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenEGraphUnRegOut1 =  new TF1("fitRunSenEGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenChargeGraphUnRegIn1  =  new TF1("fitRunSenChargeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenChargeGraphUnRegOut1 =  new TF1("fitRunSenChargeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitRunSenCorrectionXGraphUnRegIn1  =  new TF1("fitRunSenCorrectionXGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionXGraphUnRegOut1 =  new TF1("fitRunSenCorrectionXGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionXSlopeGraphUnRegIn1  =  new TF1("fitRunSenCorrectionXSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionXSlopeGraphUnRegOut1 =  new TF1("fitRunSenCorrectionXSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionYGraphUnRegIn1  =  new TF1("fitRunSenCorrectionYGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionYGraphUnRegOut1 =  new TF1("fitRunSenCorrectionYGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionYSlopeGraphUnRegIn1  =  new TF1("fitRunSenCorrectionYSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionYSlopeGraphUnRegOut1 =  new TF1("fitRunSenCorrectionYSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionEGraphUnRegIn1  =  new TF1("fitRunSenCorrectionEGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionEGraphUnRegOut1 =  new TF1("fitRunSenCorrectionEGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionChargeGraphUnRegIn1  =  new TF1("fitRunSenCorrectionChargeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionChargeGraphUnRegOut1 =  new TF1("fitRunSenCorrectionChargeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionTotalGraphUnRegIn1  =  new TF1("fitRunSenCorrectionTotalGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunSenCorrectionTotalGraphUnRegOut1 =  new TF1("fitRunSenCorrectionTotalGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitSlugSenCorrectionXGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionXGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionXGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionXGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionXSlopeGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionXSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionXSlopeGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionXSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionYGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionYGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionYGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionYGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionYSlopeGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionYSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionYSlopeGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionYSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionEGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionEGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionEGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionEGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionChargeGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionChargeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionChargeGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionChargeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionTotalGraphUnRegIn1  =  new TF1("fitSlugSenCorrectionTotalGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitSlugSenCorrectionTotalGraphUnRegOut1 =  new TF1("fitSlugSenCorrectionTotalGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitRunDiffXGraphUnRegIn1  =  new TF1("fitRunDiffXGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffXGraphUnRegOut1 =  new TF1("fitRunDiffXGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffXSlopeGraphUnRegIn1  =  new TF1("fitRunDiffXSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffXSlopeGraphUnRegOut1 =  new TF1("fitRunDiffXSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYGraphUnRegIn1  =  new TF1("fitRunDiffYGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYGraphUnRegOut1 =  new TF1("fitRunDiffYGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYSlopeGraphUnRegIn1  =  new TF1("fitRunDiffYSlopeGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYSlopeGraphUnRegOut1 =  new TF1("fitRunDiffYSlopeGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffEGraphUnRegIn1  =  new TF1("fitRunDiffEGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffEGraphUnRegOut1 =  new TF1("fitRunDiffEGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunChargeAsymGraphUnRegIn1  =  new TF1("fitRunChargeAsymGraphUnRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunChargeAsymGraphUnRegOut1 =  new TF1("fitRunChargeAsymGraphUnRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitRunDiffXGraphRegIn1  =  new TF1("fitRunDiffXGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffXGraphRegOut1 =  new TF1("fitRunDiffXGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffXSlopeGraphRegIn1  =  new TF1("fitRunDiffXSlopeGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffXSlopeGraphRegOut1 =  new TF1("fitRunDiffXSlopeGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYGraphRegIn1  =  new TF1("fitRunDiffYGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYGraphRegOut1 =  new TF1("fitRunDiffYGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYSlopeGraphRegIn1  =  new TF1("fitRunDiffYSlopeGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffYSlopeGraphRegOut1 =  new TF1("fitRunDiffYSlopeGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffEGraphRegIn1  =  new TF1("fitRunDiffEGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunDiffEGraphRegOut1 =  new TF1("fitRunDiffEGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunChargeAsymGraphRegIn1  =  new TF1("fitRunChargeAsymGraphRegIn1", "pol0",run_fit_range[0],run_fit_range[1]);
  TF1* fitRunChargeAsymGraphRegOut1 =  new TF1("fitRunChargeAsymGraphRegOut1","pol0",run_fit_range[0],run_fit_range[1]);

  TF1* fitDetGraphUnRegIn1  =  new TF1("fitDetGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitDetGraphUnRegOut1 =  new TF1("fitDetGraphUnRegOut1","pol0",fit_range[0],fit_range[1]);
  TF1* fitDetGraphRegIn1  =  new TF1("fitDetGraphRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitDetGraphRegOut1 =  new TF1("fitDetGraphRegOut1","pol0",fit_range[0],fit_range[1]);
  TF1* fitDetGraphCorIn1  =  new TF1("fitDetGraphCorIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitDetGraphCorOut1 =  new TF1("fitDetGraphCorOut1","pol0",fit_range[0],fit_range[1]);

  TF1* fitDetInOutGraphUnRegIn1  =  new TF1("fitDetInOutGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitDetInOutGraphUnRegOut1 =  new TF1("fitDetInOutGraphUnRegOut1","pol0",fit_range[0],fit_range[1]);
  TF1* fitDetInOutGraphRegIn1  =  new TF1("fitDetInOutGraphRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitDetInOutGraphRegOut1 =  new TF1("fitDetInOutGraphRegOut1","pol0",fit_range[0],fit_range[1]);
  TF1* fitDetInOutGraphCorIn1  =  new TF1("fitDetInOutGraphCorIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitDetInOutGraphCorOut1 =  new TF1("fitDetInOutGraphCorOut1","pol0",fit_range[0],fit_range[1]);

  TF1* fitRunSenCorXXSlopeGraphUnRegIn1  =  new TF1("fitRunSenCorXXSlopeGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXXSlopeGraphUnRegOut1  =  new TF1("fitRunSenCorXXSlopeGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorYYSlopeGraphUnRegIn1  =  new TF1("fitRunSenCorYYSlopeGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorYYSlopeGraphUnRegOut1  =  new TF1("fitRunSenCorYYSlopeGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXYGraphUnRegIn1  =  new TF1("fitRunSenCorXYGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXYGraphUnRegOut1  =  new TF1("fitRunSenCorXYGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXYSlopeGraphUnRegIn1  =  new TF1("fitRunSenCorXYSlopeGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXYSlopeGraphUnRegOut1  =  new TF1("fitRunSenCorXYSlopeGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorYXSlopeGraphUnRegIn1  =  new TF1("fitRunSenCorYXSlopeGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorYXSlopeGraphUnRegOut1  =  new TF1("fitRunSenCorYXSlopeGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXEGraphUnRegIn1  =  new TF1("fitRunSenCorXEGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorXEGraphUnRegOut1  =  new TF1("fitRunSenCorXEGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorYEGraphUnRegIn1  =  new TF1("fitRunSenCorYEGraphUnRegIn1", "pol0",fit_range[0],fit_range[1]);
  TF1* fitRunSenCorYEGraphUnRegOut1  =  new TF1("fitRunSenCorYEGraphUnRegOut1", "pol0",fit_range[0],fit_range[1]);

  TF1* fitOctantVariationXIn  = new TF1("fitOctantVariationXIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationXOut = new TF1("fitOctantVariationXOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationXSlopeIn  = new TF1("fitOctantVariationXSlopeIn","[0] + [1]*cos((x-1)*pi/4 + [2])",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationXSlopeOut = new TF1("fitOctantVariationXSlopeOut","[0] + [1]*cos((x-1)*pi/4 + [2])",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationYIn  = new TF1("fitOctantVariationYIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationYOut = new TF1("fitOctantVariationYOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationYSlopeIn  = new TF1("fitOctantVariationYSlopeIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationYSlopeOut = new TF1("fitOctantVariationYSlopeOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationEIn  = new TF1("fitOctantVariationEIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationEOut = new TF1("fitOctantVariationEOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);

  TF1* fitOctantVariationCorXIn  = new TF1("fitOctantVariationCorXIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorXOut = new TF1("fitOctantVariationCorXOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorXSlopeIn  = new TF1("fitOctantVariationCorXSlopeIn","[0] + [1]*cos((x-1)*pi/4 + [2])",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorXSlopeOut = new TF1("fitOctantVariationCorXSlopeOut","[0] + [1]*cos((x-1)*pi/4 + [2])",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorYIn  = new TF1("fitOctantVariationCorYIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorYOut = new TF1("fitOctantVariationCorYOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorYSlopeIn  = new TF1("fitOctantVariationCorYSlopeIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorYSlopeOut = new TF1("fitOctantVariationCorYSlopeOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorEIn  = new TF1("fitOctantVariationCorEIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorEOut = new TF1("fitOctantVariationCorEOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorTotalIn  = new TF1("fitOctantVariationCorTotalIn","[0] + [1]*cos((x)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorTotalOut = new TF1("fitOctantVariationCorTotalOut","[0] + [1]*cos((x)*pi/4)",fit_range[0],fit_range[1]);

  TF1* fitOctantVariationCorDoubletXIn  = new TF1("fitOctantVariationCorDoubletXIn","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorDoubletXOut = new TF1("fitOctantVariationCorDoubletXOut","[0] + [1]*cos((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorDoubletYIn  = new TF1("fitOctantVariationCorDoubletYIn","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);
  TF1* fitOctantVariationCorDoubletYOut = new TF1("fitOctantVariationCorDoubletYOut","[0] + [1]*sin((x-1)*pi/4)",fit_range[0],fit_range[1]);



  const Int_t NUM = 10000;

  Double_t runNumberUnRegIn[NUM],mdAsymUnRegIn[NUM],emdAsymUnRegIn[NUM],mdAsymWUnRegIn[NUM];
  Double_t diffXUnRegIn[NUM],ediffXUnRegIn[NUM],diffXSlopeUnRegIn[NUM],ediffXSlopeUnRegIn[NUM];
  Double_t diffYUnRegIn[NUM],ediffYUnRegIn[NUM],diffYSlopeUnRegIn[NUM],ediffYSlopeUnRegIn[NUM];
  Double_t diffEUnRegIn[NUM],ediffEUnRegIn[NUM],chargeUnRegIn[NUM],echargeUnRegIn[NUM],bcm1YUnRegIn[NUM],ebcm1YUnRegIn[NUM];

  Double_t runNumberUnRegOut[NUM], mdAsymUnRegOut[NUM],emdAsymUnRegOut[NUM],mdAsymWUnRegOut[NUM];
  Double_t diffXUnRegOut[NUM],ediffXUnRegOut[NUM],diffXSlopeUnRegOut[NUM],ediffXSlopeUnRegOut[NUM];
  Double_t diffYUnRegOut[NUM],ediffYUnRegOut[NUM],diffYSlopeUnRegOut[NUM],ediffYSlopeUnRegOut[NUM];
  Double_t diffEUnRegOut[NUM],ediffEUnRegOut[NUM],chargeUnRegOut[NUM],echargeUnRegOut[NUM],bcm1YUnRegOut[NUM],ebcm1YUnRegOut[NUM];

  Double_t diffXCalcUnRegIn[NUM],diffXCalcUnRegOut[NUM],diffXCalcUnRegInVal[NUM],diffXCalcUnRegOutVal[NUM];
  Double_t ediffXCalcUnRegIn[NUM],ediffXCalcUnRegOut[NUM],ediffXCalcUnRegInVal[NUM],ediffXCalcUnRegOutVal[NUM];
  Double_t diffXSlopeCalcUnRegIn[NUM],diffXSlopeCalcUnRegOut[NUM],diffXSlopeCalcUnRegInVal[NUM],diffXSlopeCalcUnRegOutVal[NUM];
  Double_t ediffXSlopeCalcUnRegIn[NUM],ediffXSlopeCalcUnRegOut[NUM],ediffXSlopeCalcUnRegInVal[NUM],ediffXSlopeCalcUnRegOutVal[NUM];
  Double_t diffYCalcUnRegIn[NUM],diffYCalcUnRegOut[NUM],diffYCalcUnRegInVal[NUM],diffYCalcUnRegOutVal[NUM];
  Double_t ediffYCalcUnRegIn[NUM],ediffYCalcUnRegOut[NUM],ediffYCalcUnRegInVal[NUM],ediffYCalcUnRegOutVal[NUM];
  Double_t diffYSlopeCalcUnRegIn[NUM],diffYSlopeCalcUnRegOut[NUM],diffYSlopeCalcUnRegInVal[NUM],diffYSlopeCalcUnRegOutVal[NUM];
  Double_t ediffYSlopeCalcUnRegIn[NUM],ediffYSlopeCalcUnRegOut[NUM],ediffYSlopeCalcUnRegInVal[NUM],ediffYSlopeCalcUnRegOutVal[NUM];
  Double_t diffECalcUnRegIn[NUM],diffECalcUnRegOut[NUM],diffECalcUnRegInVal[NUM],diffECalcUnRegOutVal[NUM];
  Double_t ediffECalcUnRegIn[NUM],ediffECalcUnRegOut[NUM],ediffECalcUnRegInVal[NUM],ediffECalcUnRegOutVal[NUM];

  Double_t runNumberUnRegInVal[NUM];
  Double_t mdAsymUnRegInVal[NUM];
  Double_t emdAsymUnRegInVal[NUM];
  Double_t mdAsymWUnRegInVal[NUM];
  Double_t mdAsymWNormUnRegInVal[NUM];
  Double_t diffXUnRegInVal[NUM];
  Double_t ediffXUnRegInVal[NUM];
  Double_t diffXSlopeUnRegInVal[NUM];
  Double_t ediffXSlopeUnRegInVal[NUM];
  Double_t diffYUnRegInVal[NUM];
  Double_t ediffYUnRegInVal[NUM];
  Double_t diffYSlopeUnRegInVal[NUM];
  Double_t ediffYSlopeUnRegInVal[NUM];
  Double_t diffEUnRegInVal[NUM];
  Double_t ediffEUnRegInVal[NUM];
  Double_t chargeUnRegInVal[NUM];
  Double_t echargeUnRegInVal[NUM];
  Double_t bcm1YRegIn[NUM];
  Double_t ebcm1YRegIn[NUM];

  Double_t runNumberUnRegOutVal[NUM];
  Double_t mdAsymUnRegOutVal[NUM];
  Double_t emdAsymUnRegOutVal[NUM];
  Double_t mdAsymWUnRegOutVal[NUM];
  Double_t mdAsymWNormUnRegOutVal[NUM];
  Double_t diffXUnRegOutVal[NUM];
  Double_t ediffXUnRegOutVal[NUM];
  Double_t diffXSlopeUnRegOutVal[NUM];
  Double_t ediffXSlopeUnRegOutVal[NUM];
  Double_t diffYUnRegOutVal[NUM];
  Double_t ediffYUnRegOutVal[NUM];
  Double_t diffYSlopeUnRegOutVal[NUM];
  Double_t ediffYSlopeUnRegOutVal[NUM];
  Double_t diffEUnRegOutVal[NUM];
  Double_t ediffEUnRegOutVal[NUM];
  Double_t chargeUnRegOutVal[NUM];
  Double_t echargeUnRegOutVal[NUM];
  Double_t bcm1YRegOut[NUM];
  Double_t ebcm1YRegOut[NUM];

  Double_t chargeRegOut[NUM],chargeRegIn[NUM],chargeRegOutVal[NUM],chargeRegInVal[NUM];
  Double_t echargeRegOut[NUM],echargeRegIn[NUM],echargeRegOutVal[NUM],echargeRegInVal[NUM];

  Double_t runNumberRegIn[NUM];
  Double_t mdAsymRegIn[NUM];
  Double_t emdAsymRegIn[NUM];
  Double_t mdAsymWRegIn[NUM];
  Double_t mdAsymCalcUnRegIn[NUM];
  Double_t emdAsymCalcUnRegIn[NUM];
  Double_t mdAsymWCalcUnRegIn[NUM];
  Double_t diffXRegIn[NUM];
  Double_t ediffXRegIn[NUM];
  Double_t diffXSlopeRegIn[NUM];
  Double_t ediffXSlopeRegIn[NUM];
  Double_t diffYRegIn[NUM];
  Double_t ediffYRegIn[NUM];
  Double_t diffYSlopeRegIn[NUM];
  Double_t ediffYSlopeRegIn[NUM];
  Double_t diffERegIn[NUM];
  Double_t ediffERegIn[NUM];
  Double_t senXRegIn[NUM];
  Double_t esenXRegIn[NUM];
  Double_t senXSlopeRegIn[NUM];
  Double_t esenXSlopeRegIn[NUM];
  Double_t senYRegIn[NUM];
  Double_t esenYRegIn[NUM];
  Double_t senYSlopeRegIn[NUM];
  Double_t esenYSlopeRegIn[NUM];
  Double_t senERegIn[NUM];
  Double_t esenERegIn[NUM];

  Double_t runNumberRegOut[NUM];
  Double_t mdAsymRegOut[NUM];
  Double_t emdAsymRegOut[NUM];
  Double_t mdAsymWRegOut[NUM];
  Double_t mdAsymCalcUnRegOut[NUM];
  Double_t emdAsymCalcUnRegOut[NUM];
  Double_t mdAsymWCalcUnRegOut[NUM];
  Double_t diffXRegOut[NUM];
  Double_t ediffXRegOut[NUM];
  Double_t diffXSlopeRegOut[NUM];
  Double_t ediffXSlopeRegOut[NUM];
  Double_t diffYRegOut[NUM];
  Double_t ediffYRegOut[NUM];
  Double_t diffYSlopeRegOut[NUM];
  Double_t ediffYSlopeRegOut[NUM];
  Double_t diffERegOut[NUM];
  Double_t ediffERegOut[NUM];
  Double_t senXRegOut[NUM];
  Double_t esenXRegOut[NUM];
  Double_t senXSlopeRegOut[NUM];
  Double_t esenXSlopeRegOut[NUM];
  Double_t senYRegOut[NUM];
  Double_t esenYRegOut[NUM];
  Double_t senYSlopeRegOut[NUM];
  Double_t esenYSlopeRegOut[NUM];
  Double_t senERegOut[NUM];
  Double_t esenERegOut[NUM];

  Double_t runNumberRegInVal[NUM];
  Double_t mdAsymRegInVal[NUM];
  Double_t emdAsymRegInVal[NUM];
  Double_t mdAsymWRegInVal[NUM];
  Double_t mdAsymCalcUnRegInVal[NUM];
  Double_t emdAsymCalcUnRegInVal[NUM];
  Double_t mdAsymWCalcUnRegInVal[NUM];
  Double_t diffXRegInVal[NUM];
  Double_t ediffXRegInVal[NUM];
  Double_t diffXSlopeRegInVal[NUM];
  Double_t ediffXSlopeRegInVal[NUM];
  Double_t diffYRegInVal[NUM];
  Double_t ediffYRegInVal[NUM];
  Double_t diffYSlopeRegInVal[NUM];
  Double_t ediffYSlopeRegInVal[NUM];
  Double_t diffERegInVal[NUM];
  Double_t ediffERegInVal[NUM];
  Double_t senXRegInVal[NUM];
  Double_t esenXRegInVal[NUM];
  Double_t senXSlopeRegInVal[NUM];
  Double_t esenXSlopeRegInVal[NUM];
  Double_t senYRegInVal[NUM];
  Double_t esenYRegInVal[NUM];
  Double_t senYSlopeRegInVal[NUM];
  Double_t esenYSlopeRegInVal[NUM];
  Double_t senERegInVal[NUM];
  Double_t esenERegInVal[NUM];

  Double_t runNumberRegOutVal[NUM];
  Double_t mdAsymRegOutVal[NUM];
  Double_t emdAsymRegOutVal[NUM];
  Double_t mdAsymWRegOutVal[NUM];
  Double_t mdAsymCalcUnRegOutVal[NUM];
  Double_t emdAsymCalcUnRegOutVal[NUM];
  Double_t mdAsymWCalcUnRegOutVal[NUM];
  Double_t diffXRegOutVal[NUM];
  Double_t ediffXRegOutVal[NUM];
  Double_t diffXSlopeRegOutVal[NUM];
  Double_t ediffXSlopeRegOutVal[NUM];
  Double_t diffYRegOutVal[NUM];
  Double_t ediffYRegOutVal[NUM];
  Double_t diffYSlopeRegOutVal[NUM];
  Double_t ediffYSlopeRegOutVal[NUM];
  Double_t diffERegOutVal[NUM];
  Double_t ediffERegOutVal[NUM];
  Double_t senXRegOutVal[NUM];
  Double_t esenXRegOutVal[NUM];
  Double_t senXSlopeRegOutVal[NUM];
  Double_t esenXSlopeRegOutVal[NUM];
  Double_t senYRegOutVal[NUM];
  Double_t esenYRegOutVal[NUM];
  Double_t senYSlopeRegOutVal[NUM];
  Double_t esenYSlopeRegOutVal[NUM];
  Double_t senERegOutVal[NUM];
  Double_t esenERegOutVal[NUM];

  Double_t corRunletXRegInVal[NUM];
  Double_t ecorRunletXRegInVal[NUM];
  Double_t corRunletXSlopeRegInVal[NUM];
  Double_t ecorRunletXSlopeRegInVal[NUM];
  Double_t corRunletYRegInVal[NUM];
  Double_t ecorRunletYRegInVal[NUM];
  Double_t corRunletYSlopeRegInVal[NUM];
  Double_t ecorRunletYSlopeRegInVal[NUM];
  Double_t corRunletERegInVal[NUM];
  Double_t ecorRunletERegInVal[NUM];
  Double_t corRunletTotalRegInVal[NUM];
  Double_t ecorRunletTotalRegInVal[NUM];

  Double_t corRunletXRegOutVal[NUM];
  Double_t ecorRunletXRegOutVal[NUM];
  Double_t corRunletXSlopeRegOutVal[NUM];
  Double_t ecorRunletXSlopeRegOutVal[NUM];
  Double_t corRunletYRegOutVal[NUM];
  Double_t ecorRunletYRegOutVal[NUM];
  Double_t corRunletYSlopeRegOutVal[NUM];
  Double_t ecorRunletYSlopeRegOutVal[NUM];
  Double_t corRunletERegOutVal[NUM];
  Double_t ecorRunletERegOutVal[NUM];
  Double_t corRunletTotalRegOutVal[NUM];
  Double_t ecorRunletTotalRegOutVal[NUM];

  Double_t corSlugXRegInVal[NUM];
  Double_t ecorSlugXRegInVal[NUM];
  Double_t corSlugXSlopeRegInVal[NUM];
  Double_t ecorSlugXSlopeRegInVal[NUM];
  Double_t corSlugYRegInVal[NUM];
  Double_t ecorSlugYRegInVal[NUM];
  Double_t corSlugYSlopeRegInVal[NUM];
  Double_t ecorSlugYSlopeRegInVal[NUM];
  Double_t corSlugERegInVal[NUM];
  Double_t ecorSlugERegInVal[NUM];
  Double_t corSlugTotalRegInVal[NUM];
  Double_t ecorSlugTotalRegInVal[NUM];

  Double_t corSlugXRegOutVal[NUM];
  Double_t ecorSlugXRegOutVal[NUM];
  Double_t corSlugXSlopeRegOutVal[NUM];
  Double_t ecorSlugXSlopeRegOutVal[NUM];
  Double_t corSlugYRegOutVal[NUM];
  Double_t ecorSlugYRegOutVal[NUM];
  Double_t corSlugYSlopeRegOutVal[NUM];
  Double_t ecorSlugYSlopeRegOutVal[NUM];
  Double_t corSlugERegOutVal[NUM];
  Double_t ecorSlugERegOutVal[NUM];
  Double_t corSlugTotalRegOutVal[NUM];
  Double_t ecorSlugTotalRegOutVal[NUM];

  Double_t mdAsymRunletCorIn[NUM];
  Double_t emdAsymRunletCorIn[NUM];
  Double_t mdAsymRunletCorOut[NUM];
  Double_t emdAsymRunletCorOut[NUM];

  Double_t mdAsymSlugCorIn[NUM];
  Double_t emdAsymSlugCorIn[NUM];
  Double_t mdAsymSlugCorOut[NUM];
  Double_t emdAsymSlugCorOut[NUM];

  Double_t mdAsymSlugAvgCorIn[NUM];
  Double_t emdAsymSlugAvgCorIn[NUM];
  Double_t mdAsymSlugAvgCorOut[NUM];
  Double_t emdAsymSlugAvgCorOut[NUM];

  Double_t mdAsymWNormUnRegInVal[NUM];
  Double_t emdAsymWNormUnRegInVal[NUM];
  Double_t mdAsymWNormUnRegOutVal[NUM];
  Double_t emdAsymWNormUnRegOutVal[NUM];
  Double_t mdAsymWCalcNormUnRegInVal[NUM];
  Double_t emdAsymWCalcNormUnRegInVal[NUM];
  Double_t mdAsymWCalcNormUnRegOutVal[NUM];
  Double_t emdAsymWCalcNormUnRegOutVal[NUM];
  Double_t mdAsymWNormRegInVal[NUM];
  Double_t emdAsymWNormRegInVal[NUM];
  Double_t mdAsymWNormRegOutVal[NUM];
  Double_t emdAsymWNormRegOutVal[NUM];

  Double_t mdAsymWNormDiffInVal[NUM];
  Double_t emdAsymWNormDiffInVal[NUM];
  Double_t mdAsymWNormDiffOutVal[NUM];
  Double_t emdAsymWNormDiffOutVal[NUM];
  Double_t mdAsymWCalcDiffInVal[NUM];
  Double_t emdAsymWCalcDiffInVal[NUM];
  Double_t mdAsymWCalcDiffOutVal[NUM];
  Double_t emdAsymWCalcDiffOutVal[NUM];

  Double_t zeroUnReg[NUM],zeroReg[NUM],zeroUnRegIn[NUM],zeroRegIn[NUM],zeroUnRegOut[NUM],zeroRegOut[NUM];

  const Int_t NUMC = 10;

  Double_t avgDiffXIn[NUMC],avgDiffXSlopeIn[NUMC],avgDiffYIn[NUMC],avgDiffYSlopeIn[NUMC],avgDiffEIn[NUMC],avgChargeIn[NUMC],zeroAvg[NUMC];
  Double_t eavgDiffXIn[NUMC],eavgDiffXSlopeIn[NUMC],eavgDiffYIn[NUMC],eavgDiffYSlopeIn[NUMC],eavgDiffEIn[NUMC],eavgChargeIn[NUMC];
  Double_t avgSenXIn[NUMC],avgSenXSlopeIn[NUMC],avgSenYIn[NUMC],avgSenYSlopeIn[NUMC],avgSenEIn[NUMC],avgSenChargeIn[NUMC];
  Double_t eavgSenXIn[NUMC],eavgSenXSlopeIn[NUMC],eavgSenYIn[NUMC],eavgSenYSlopeIn[NUMC],eavgSenEIn[NUMC],eavgSenChargeIn[NUMC];
  Double_t avgCorXIn[NUMC],avgCorXSlopeIn[NUMC],avgCorYIn[NUMC],avgCorYSlopeIn[NUMC],avgCorEIn[NUMC],avgCorChargeIn[NUMC],avgCorTotalIn[NUMC];
  Double_t eavgCorXIn[NUMC],eavgCorXSlopeIn[NUMC],eavgCorYIn[NUMC],eavgCorYSlopeIn[NUMC],eavgCorEIn[NUMC],eavgCorChargeIn[NUMC],eavgCorTotalIn[NUMC];

  Double_t avgDiffXOut[NUMC],avgDiffXSlopeOut[NUMC],avgDiffYOut[NUMC],avgDiffYSlopeOut[NUMC],avgDiffEOut[NUMC],avgChargeOut[NUMC];
  Double_t eavgDiffXOut[NUMC],eavgDiffXSlopeOut[NUMC],eavgDiffYOut[NUMC],eavgDiffYSlopeOut[NUMC],eavgDiffEOut[NUMC],eavgChargeOut[NUMC];
  Double_t avgSenXOut[NUMC],avgSenXSlopeOut[NUMC],avgSenYOut[NUMC],avgSenYSlopeOut[NUMC],avgSenEOut[NUMC],avgSenChargeOut[NUMC];
  Double_t eavgSenXOut[NUMC],eavgSenXSlopeOut[NUMC],eavgSenYOut[NUMC],eavgSenYSlopeOut[NUMC],eavgSenEOut[NUMC],eavgSenChargeOut[NUMC];
  Double_t avgCorXOut[NUMC],avgCorXSlopeOut[NUMC],avgCorYOut[NUMC],avgCorYSlopeOut[NUMC],avgCorEOut[NUMC],avgCorChargeOut[NUMC],avgCorTotalOut[NUMC];
  Double_t eavgCorXOut[NUMC],eavgCorXSlopeOut[NUMC],eavgCorYOut[NUMC],eavgCorYSlopeOut[NUMC],eavgCorEOut[NUMC],eavgCorChargeOut[NUMC],eavgCorTotalOut[NUMC];


  int counterAvg=0;
  ifstream inDiffAvgUnReg,outDiffAvgUnReg,inSenAvg,outSenAvg,inCorAvg,outCorAvg;
  inDiffAvgUnReg.open((Form("%s/plots/%s/summary%sRunletUnRegDiffAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
  outDiffAvgUnReg.open((Form("%s/plots/%s/summary%sRunletUnRegDiffAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
  inSenAvg.open((Form("%s/plots/%s/summary%sRunletSenAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
  outSenAvg.open((Form("%s/plots/%s/summary%sRunletSenAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
  inCorAvg.open((Form("%s/plots/%s/summary%sRunletCorrectionAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
  outCorAvg.open((Form("%s/plots/%s/summary%sRunletCorrectionAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));

  while(1) {
    zeroAvg[counterAvg]=0;

    inDiffAvgUnReg >> avgDiffXIn[counterAvg];
    inDiffAvgUnReg >> eavgDiffXIn[counterAvg];
    inDiffAvgUnReg >> avgDiffXSlopeIn[counterAvg];
    inDiffAvgUnReg >> eavgDiffXSlopeIn[counterAvg];
    inDiffAvgUnReg >> avgDiffYIn[counterAvg];
    inDiffAvgUnReg >> eavgDiffYIn[counterAvg];
    inDiffAvgUnReg >> avgDiffYSlopeIn[counterAvg];
    inDiffAvgUnReg >> eavgDiffYSlopeIn[counterAvg];
    inDiffAvgUnReg >> avgDiffEIn[counterAvg];
    inDiffAvgUnReg >> eavgDiffEIn[counterAvg];
    inDiffAvgUnReg >> avgChargeIn[counterAvg];
    inDiffAvgUnReg >> eavgChargeIn[counterAvg];

    if (!inDiffAvgUnReg.good()) break;

    outDiffAvgUnReg >> avgDiffXOut[counterAvg];
    outDiffAvgUnReg >> eavgDiffXOut[counterAvg];
    outDiffAvgUnReg >> avgDiffXSlopeOut[counterAvg];
    outDiffAvgUnReg >> eavgDiffXSlopeOut[counterAvg];
    outDiffAvgUnReg >> avgDiffYOut[counterAvg];
    outDiffAvgUnReg >> eavgDiffYOut[counterAvg];
    outDiffAvgUnReg >> avgDiffYSlopeOut[counterAvg];
    outDiffAvgUnReg >> eavgDiffYSlopeOut[counterAvg];
    outDiffAvgUnReg >> avgDiffEOut[counterAvg];
    outDiffAvgUnReg >> eavgDiffEOut[counterAvg];
    outDiffAvgUnReg >> avgChargeOut[counterAvg];
    outDiffAvgUnReg >> eavgChargeOut[counterAvg];

    if (!outDiffAvgUnReg.good()) break;

    inSenAvg >> avgSenXIn[counterAvg];
    inSenAvg >> eavgSenXIn[counterAvg];
    inSenAvg >> avgSenXSlopeIn[counterAvg];
    inSenAvg >> eavgSenXSlopeIn[counterAvg];
    inSenAvg >> avgSenYIn[counterAvg];
    inSenAvg >> eavgSenYIn[counterAvg];
    inSenAvg >> avgSenYSlopeIn[counterAvg];
    inSenAvg >> eavgSenYSlopeIn[counterAvg];
    inSenAvg >> avgSenEIn[counterAvg];
    inSenAvg >> eavgSenEIn[counterAvg];
    inSenAvg >> avgChargeIn[counterAvg];
    inSenAvg >> eavgChargeIn[counterAvg];

    if (!inSenAvg.good()) break;

    outSenAvg >> avgSenXOut[counterAvg];
    outSenAvg >> eavgSenXOut[counterAvg];
    outSenAvg >> avgSenXSlopeOut[counterAvg];
    outSenAvg >> eavgSenXSlopeOut[counterAvg];
    outSenAvg >> avgSenYOut[counterAvg];
    outSenAvg >> eavgSenYOut[counterAvg];
    outSenAvg >> avgSenYSlopeOut[counterAvg];
    outSenAvg >> eavgSenYSlopeOut[counterAvg];
    outSenAvg >> avgSenEOut[counterAvg];
    outSenAvg >> eavgSenEOut[counterAvg];
    outSenAvg >> avgChargeOut[counterAvg];
    outSenAvg >> eavgChargeOut[counterAvg];

    if (!outSenAvg.good()) break;

    inCorAvg >> avgCorXIn[counterAvg];
    inCorAvg >> eavgCorXIn[counterAvg];
    inCorAvg >> avgCorXSlopeIn[counterAvg];
    inCorAvg >> eavgCorXSlopeIn[counterAvg];
    inCorAvg >> avgCorYIn[counterAvg];
    inCorAvg >> eavgCorYIn[counterAvg];
    inCorAvg >> avgCorYSlopeIn[counterAvg];
    inCorAvg >> eavgCorYSlopeIn[counterAvg];
    inCorAvg >> avgCorEIn[counterAvg];
    inCorAvg >> eavgCorEIn[counterAvg];
    inCorAvg >> avgCorTotalIn[counterAvg];
    inCorAvg >> eavgCorTotalIn[counterAvg];

    if (!inCorAvg.good()) break;

    outCorAvg >> avgCorXOut[counterAvg];
    outCorAvg >> eavgCorXOut[counterAvg];
    outCorAvg >> avgCorXSlopeOut[counterAvg];
    outCorAvg >> eavgCorXSlopeOut[counterAvg];
    outCorAvg >> avgCorYOut[counterAvg];
    outCorAvg >> eavgCorYOut[counterAvg];
    outCorAvg >> avgCorYSlopeOut[counterAvg];
    outCorAvg >> eavgCorYSlopeOut[counterAvg];
    outCorAvg >> avgCorEOut[counterAvg];
    outCorAvg >> eavgCorEOut[counterAvg];
    outCorAvg >> avgCorTotalOut[counterAvg];
    outCorAvg >> eavgCorTotalOut[counterAvg];

    if (!outCorAvg.good()) break;


    counterAvg++;
  }
  inDiffAvgUnReg.close();
  outDiffAvgUnReg.close();
  inSenAvg.close();
  outSenAvg.close();

  int counterUnReg=0,counterReg=0,counterUnRegIn=0,counterRegIn=0,counterUnRegOut=0,counterRegOut=0;
  ifstream inAllUnReg,outAllUnReg,inAllReg,outAllReg;
  inAllUnReg.open((Form("%s/dbText/%sallUnRegIn.txt",n2deltaAnalysisDir.Data(),savePlot)));
  outAllUnReg.open((Form("%s/dbText/%sallUnRegOut.txt",n2deltaAnalysisDir.Data(),savePlot)));
  inAllReg.open((Form("%s/dbText/%sall%sIn.txt",n2deltaAnalysisDir.Data(),savePlot,regScheme)));
  outAllReg.open((Form("%s/dbText/%sall%sOut.txt",n2deltaAnalysisDir.Data(),savePlot,regScheme)));

  while(1) {
    zeroUnRegIn[counterUnRegIn]=0;

    inAllUnReg >> runNumberUnRegIn[counterUnRegIn];
    inAllUnReg >> mdAsymUnRegIn[counterUnRegIn];
    inAllUnReg >> emdAsymUnRegIn[counterUnRegIn];
    inAllUnReg >> mdAsymWUnRegIn[counterUnRegIn];
    inAllUnReg >> diffXUnRegIn[counterUnRegIn];
    inAllUnReg >> ediffXUnRegIn[counterUnRegIn];
    inAllUnReg >> diffXSlopeUnRegIn[counterUnRegIn];
    inAllUnReg >> ediffXSlopeUnRegIn[counterUnRegIn];
    inAllUnReg >> diffYUnRegIn[counterUnRegIn];
    inAllUnReg >> ediffYUnRegIn[counterUnRegIn];
    inAllUnReg >> diffYSlopeUnRegIn[counterUnRegIn];
    inAllUnReg >> ediffYSlopeUnRegIn[counterUnRegIn];
    inAllUnReg >> diffEUnRegIn[counterUnRegIn];
    inAllUnReg >> ediffEUnRegIn[counterUnRegIn];
    inAllUnReg >> chargeUnRegIn[counterUnRegIn];
    inAllUnReg >> echargeUnRegIn[counterUnRegIn];
    inAllUnReg >> bcm1YUnRegIn[counterUnRegIn];
    inAllUnReg >> ebcm1YUnRegIn[counterUnRegIn];
 
    if (!inAllUnReg.good()) break;

    mdAsymUnRegInVal[counterUnRegIn]          =   mdAsymUnRegIn[counterUnRegIn]*1e6;
    emdAsymUnRegInVal[counterUnRegIn]         =   emdAsymUnRegIn[counterUnRegIn]*1e6;
    mdAsymWUnRegInVal[counterUnRegIn]         =   mdAsymWUnRegIn[counterUnRegIn]*1e6;
    diffXUnRegInVal[counterUnRegIn]           =   diffXUnRegIn[counterUnRegIn];
    ediffXUnRegInVal[counterUnRegIn]          =   ediffXUnRegIn[counterUnRegIn];
    diffXSlopeUnRegInVal[counterUnRegIn]      =   diffXSlopeUnRegIn[counterUnRegIn]*1e6;
    ediffXSlopeUnRegInVal[counterUnRegIn]     =   ediffXSlopeUnRegIn[counterUnRegIn]*1e6;
    diffYUnRegInVal[counterUnRegIn]           =   diffYUnRegIn[counterUnRegIn];
    ediffYUnRegInVal[counterUnRegIn]          =   ediffYUnRegIn[counterUnRegIn];
    diffYSlopeUnRegInVal[counterUnRegIn]      =   diffYSlopeUnRegIn[counterUnRegIn]*1e6;
    ediffYSlopeUnRegInVal[counterUnRegIn]     =   ediffYSlopeUnRegIn[counterUnRegIn]*1e6;
    diffEUnRegInVal[counterUnRegIn]           =   diffEUnRegIn[counterUnRegIn];
    ediffEUnRegInVal[counterUnRegIn]          =   ediffEUnRegIn[counterUnRegIn];
    chargeUnRegInVal[counterUnRegIn]          =   chargeUnRegIn[counterUnRegIn]*1e6;
    echargeUnRegInVal[counterUnRegIn]         =   echargeUnRegIn[counterUnRegIn]*1e6;
    mdAsymWNormUnRegInVal[counterUnRegIn]     =   mdAsymWUnRegInVal[counterUnRegIn]/TMath::Sqrt(bcm1YUnRegIn[counterUnRegIn]/160);

    counterUnRegIn++;
  }
  inAllUnReg.close();

  while(1) {
    zeroUnRegOut[counterUnRegOut]=0;

    outAllUnReg >> runNumberUnRegOut[counterUnRegOut];
    outAllUnReg >> mdAsymUnRegOut[counterUnRegOut];
    outAllUnReg >> emdAsymUnRegOut[counterUnRegOut];
    outAllUnReg >> mdAsymWUnRegOut[counterUnRegOut];
    outAllUnReg >> diffXUnRegOut[counterUnRegOut];
    outAllUnReg >> ediffXUnRegOut[counterUnRegOut];
    outAllUnReg >> diffXSlopeUnRegOut[counterUnRegOut];
    outAllUnReg >> ediffXSlopeUnRegOut[counterUnRegOut];
    outAllUnReg >> diffYUnRegOut[counterUnRegOut];
    outAllUnReg >> ediffYUnRegOut[counterUnRegOut];
    outAllUnReg >> diffYSlopeUnRegOut[counterUnRegOut];
    outAllUnReg >> ediffYSlopeUnRegOut[counterUnRegOut];
    outAllUnReg >> diffEUnRegOut[counterUnRegOut];
    outAllUnReg >> ediffEUnRegOut[counterUnRegOut];
    outAllUnReg >> chargeUnRegOut[counterUnRegOut];
    outAllUnReg >> echargeUnRegOut[counterUnRegOut];
    outAllUnReg >> bcm1YUnRegOut[counterUnRegOut];
    outAllUnReg >> ebcm1YUnRegOut[counterUnRegOut];

    if (!outAllUnReg.good()) break;

    mdAsymUnRegOutVal[counterUnRegOut]         =   mdAsymUnRegOut[counterUnRegOut]*1e6;
    emdAsymUnRegOutVal[counterUnRegOut]        =   emdAsymUnRegOut[counterUnRegOut]*1e6;
    mdAsymWUnRegOutVal[counterUnRegOut]        =   mdAsymWUnRegOut[counterUnRegOut]*1e6;
    diffXUnRegOutVal[counterUnRegOut]          =   diffXUnRegOut[counterUnRegOut];
    ediffXUnRegOutVal[counterUnRegOut]         =   ediffXUnRegOut[counterUnRegOut];
    diffXSlopeUnRegOutVal[counterUnRegOut]     =   diffXSlopeUnRegOut[counterUnRegOut]*1e6;
    ediffXSlopeUnRegOutVal[counterUnRegOut]    =   ediffXSlopeUnRegOut[counterUnRegOut]*1e6;
    diffYUnRegOutVal[counterUnRegOut]          =   diffYUnRegOut[counterUnRegOut];
    ediffYUnRegOutVal[counterUnRegOut]         =   ediffYUnRegOut[counterUnRegOut];
    diffYSlopeUnRegOutVal[counterUnRegOut]     =   diffYSlopeUnRegOut[counterUnRegOut]*1e6;
    ediffYSlopeUnRegOutVal[counterUnRegOut]    =   ediffYSlopeUnRegOut[counterUnRegOut]*1e6;
    diffEUnRegOutVal[counterUnRegOut]          =   diffEUnRegOut[counterUnRegOut];
    ediffEUnRegOutVal[counterUnRegOut]         =   ediffEUnRegOut[counterUnRegOut];
    chargeUnRegOutVal[counterUnRegOut]         =   chargeUnRegOut[counterUnRegOut]*1e6;
    echargeUnRegOutVal[counterUnRegOut]        =   echargeUnRegOut[counterUnRegOut]*1e6;
    mdAsymWNormUnRegOutVal[counterUnRegOut]    =   mdAsymWUnRegOutVal[counterUnRegOut]/TMath::Sqrt(bcm1YUnRegOut[counterUnRegOut]/160);

    counterUnRegOut++;
  }
  outAllUnReg.close();

  /*********************************************************************************/

//   checkXXp = new TProfile("checkXXp","Checking var X",200,1000,6000,-200,200);


  while(1) {
    zeroRegIn[counterRegIn]=0;

    inAllReg >> runNumberRegIn[counterRegIn];
    inAllReg >> mdAsymRegIn[counterRegIn];
    inAllReg >> emdAsymRegIn[counterRegIn];
    inAllReg >> mdAsymWRegIn[counterRegIn];
    inAllReg >> mdAsymCalcUnRegIn[counterRegIn];
    inAllReg >> emdAsymCalcUnRegIn[counterRegIn];
    inAllReg >> mdAsymWCalcUnRegIn[counterRegIn];
    inAllReg >> diffXCalcUnRegIn[counterRegIn];
    inAllReg >> ediffXCalcUnRegIn[counterRegIn];
    inAllReg >> diffXSlopeCalcUnRegIn[counterRegIn];
    inAllReg >> ediffXSlopeCalcUnRegIn[counterRegIn];
    inAllReg >> diffYCalcUnRegIn[counterRegIn];
    inAllReg >> ediffYCalcUnRegIn[counterRegIn];
    inAllReg >> diffYSlopeCalcUnRegIn[counterRegIn];
    inAllReg >> ediffYSlopeCalcUnRegIn[counterRegIn];
    inAllReg >> diffECalcUnRegIn[counterRegIn];
    inAllReg >> ediffECalcUnRegIn[counterRegIn];
    inAllReg >> diffXRegIn[counterRegIn];
    inAllReg >> ediffXRegIn[counterRegIn];
    inAllReg >> diffXSlopeRegIn[counterRegIn];
    inAllReg >> ediffXSlopeRegIn[counterRegIn];
    inAllReg >> diffYRegIn[counterRegIn];
    inAllReg >> ediffYRegIn[counterRegIn];
    inAllReg >> diffYSlopeRegIn[counterRegIn];
    inAllReg >> ediffYSlopeRegIn[counterRegIn];
    inAllReg >> diffERegIn[counterRegIn];
    inAllReg >> ediffERegIn[counterRegIn];
    inAllReg >> senXRegIn[counterRegIn];
    inAllReg >> esenXRegIn[counterRegIn];
    inAllReg >> senXSlopeRegIn[counterRegIn];
    inAllReg >> esenXSlopeRegIn[counterRegIn];
    inAllReg >> senYRegIn[counterRegIn];
    inAllReg >> esenYRegIn[counterRegIn];
    inAllReg >> senYSlopeRegIn[counterRegIn];
    inAllReg >> esenYSlopeRegIn[counterRegIn];
    inAllReg >> senERegIn[counterRegIn];
    inAllReg >> esenERegIn[counterRegIn];
    inAllReg >> chargeRegIn[counterRegIn];
    inAllReg >> echargeRegIn[counterRegIn];
    inAllReg >> bcm1YRegIn[counterRegIn];
    inAllReg >> ebcm1YRegIn[counterRegIn];

    if (!inAllReg.good()) break;

    mdAsymRegInVal[counterRegIn]                =   mdAsymRegIn[counterRegIn]*1e6;
    emdAsymRegInVal[counterRegIn]               =   emdAsymRegIn[counterRegIn]*1e6;
    mdAsymWRegInVal[counterRegIn]               =   mdAsymWRegIn[counterRegIn]*1e6;
    mdAsymCalcUnRegInVal[counterRegIn]          =   mdAsymCalcUnRegIn[counterRegIn]*1e6;
    emdAsymCalcUnRegInVal[counterRegIn]         =   emdAsymCalcUnRegIn[counterRegIn]*1e6;
    mdAsymWCalcUnRegInVal[counterRegIn]         =   mdAsymWCalcUnRegIn[counterRegIn]*1e6;
    diffXCalcUnRegInVal[counterRegIn]           =   diffXCalcUnRegIn[counterRegIn];
    ediffXCalcUnRegInVal[counterRegIn]          =   ediffXCalcUnRegIn[counterRegIn];
    diffXSlopeCalcUnRegInVal[counterRegIn]      =   diffXSlopeCalcUnRegIn[counterRegIn]*1e6;
    ediffXSlopeCalcUnRegInVal[counterRegIn]     =   ediffXSlopeCalcUnRegIn[counterRegIn]*1e6;
    diffYCalcUnRegInVal[counterRegIn]           =   diffYCalcUnRegIn[counterRegIn];
    ediffYCalcUnRegInVal[counterRegIn]          =   ediffYCalcUnRegIn[counterRegIn];
    diffYSlopeCalcUnRegInVal[counterRegIn]      =   diffYSlopeCalcUnRegIn[counterRegIn]*1e6;
    ediffYSlopeCalcUnRegInVal[counterRegIn]     =   ediffYSlopeCalcUnRegIn[counterRegIn]*1e6;
    diffECalcUnRegInVal[counterRegIn]           =   diffECalcUnRegIn[counterRegIn];
    ediffECalcUnRegInVal[counterRegIn]          =   ediffECalcUnRegIn[counterRegIn];
    diffXRegInVal[counterRegIn]                 =   diffXRegIn[counterRegIn];
    ediffXRegInVal[counterRegIn]                =   ediffXRegIn[counterRegIn];
    diffXSlopeRegInVal[counterRegIn]            =   diffXSlopeRegIn[counterRegIn]*1e6;
    ediffXSlopeRegInVal[counterRegIn]           =   ediffXSlopeRegIn[counterRegIn]*1e6;
    diffYRegInVal[counterRegIn]                 =   diffYRegIn[counterRegIn];
    ediffYRegInVal[counterRegIn]                =   ediffYRegIn[counterRegIn];
    diffYSlopeRegInVal[counterRegIn]            =   diffYSlopeRegIn[counterRegIn]*1e6;
    ediffYSlopeRegInVal[counterRegIn]           =   ediffYSlopeRegIn[counterRegIn]*1e6;
    diffERegInVal[counterRegIn]                 =   diffERegIn[counterRegIn];
    ediffERegInVal[counterRegIn]                =   ediffERegIn[counterRegIn];
    senXRegInVal[counterRegIn]                  =   senXRegIn[counterRegIn]*1e6;
    esenXRegInVal[counterRegIn]                 =   esenXRegIn[counterRegIn]*1e6;
    senXSlopeRegInVal[counterRegIn]             =   senXSlopeRegIn[counterRegIn];
    esenXSlopeRegInVal[counterRegIn]            =   esenXSlopeRegIn[counterRegIn];
    senYRegInVal[counterRegIn]                  =   senYRegIn[counterRegIn]*1e6;
    esenYRegInVal[counterRegIn]                 =   esenYRegIn[counterRegIn]*1e6;
    senYSlopeRegInVal[counterRegIn]             =   senYSlopeRegIn[counterRegIn];
    esenYSlopeRegInVal[counterRegIn]            =   esenYSlopeRegIn[counterRegIn];
    senERegInVal[counterRegIn]                  =   senERegIn[counterRegIn];
    esenERegInVal[counterRegIn]                 =   esenERegIn[counterRegIn];
    chargeRegInVal[counterRegIn]                =   chargeRegIn[counterRegIn];
    echargeRegInVal[counterRegIn]               =   echargeRegIn[counterRegIn];
    mdAsymWNormRegInVal[counterRegIn]           =   mdAsymWRegInVal[counterRegIn]/TMath::Sqrt(bcm1YRegIn[counterRegIn]/160);
    mdAsymWCalcNormUnRegInVal[counterRegIn]     =   mdAsymWCalcUnRegInVal[counterRegIn]/TMath::Sqrt(bcm1YRegIn[counterRegIn]/160);

    mdAsymWCalcDiffInVal[counterRegIn]          = mdAsymWCalcUnRegInVal[counterRegIn] - mdAsymWRegInVal[counterRegIn];
    mdAsymWNormDiffInVal[counterRegIn]          = mdAsymWCalcNormUnRegInVal[counterRegIn] - mdAsymWNormRegInVal[counterRegIn];

    corRunletXRegInVal[counterRegIn]           =   diffXCalcUnRegInVal[counterRegIn]*senXRegInVal[counterRegIn];
    ecorRunletXRegInVal[counterRegIn]          =   TMath::Sqrt(pow(diffXCalcUnRegInVal[counterRegIn]*esenXRegInVal[counterRegIn],2)+pow(ediffXCalcUnRegInVal[counterRegIn]*senXRegInVal[counterRegIn],2));
    corRunletXSlopeRegInVal[counterRegIn]      =   diffXSlopeCalcUnRegInVal[counterRegIn]*senXSlopeRegInVal[counterRegIn];
    ecorRunletXSlopeRegInVal[counterRegIn]     =   TMath::Sqrt(pow(diffXSlopeCalcUnRegInVal[counterRegIn]*esenXSlopeRegInVal[counterRegIn],2)+pow(ediffXSlopeCalcUnRegInVal[counterRegIn]*senXSlopeRegInVal[counterRegIn],2));
    corRunletYRegInVal[counterRegIn]           =   diffYCalcUnRegInVal[counterRegIn]*senYRegInVal[counterRegIn];
    ecorRunletYRegInVal[counterRegIn]          =   TMath::Sqrt(pow(diffYCalcUnRegInVal[counterRegIn]*esenYRegInVal[counterRegIn],2)+pow(ediffYCalcUnRegInVal[counterRegIn]*senYRegInVal[counterRegIn],2));
    corRunletYSlopeRegInVal[counterRegIn]      =   diffYSlopeCalcUnRegInVal[counterRegIn]*senYSlopeRegInVal[counterRegIn];
    ecorRunletYSlopeRegInVal[counterRegIn]     =   TMath::Sqrt(pow(diffYSlopeCalcUnRegInVal[counterRegIn]*esenYSlopeRegInVal[counterRegIn],2)+pow(ediffYSlopeCalcUnRegInVal[counterRegIn]*senYSlopeRegInVal[counterRegIn],2));
    corRunletERegInVal[counterRegIn]           =   diffECalcUnRegInVal[counterRegIn]*senERegInVal[counterRegIn]*1e6;
    ecorRunletERegInVal[counterRegIn]          =   TMath::Sqrt(pow(diffECalcUnRegInVal[counterRegIn]*esenERegInVal[counterRegIn]*1e6,2)+pow(ediffECalcUnRegInVal[counterRegIn]*senERegInVal[counterRegIn]*1e6,2));
    corRunletTotalRegInVal[counterRegIn]       =   corRunletXRegInVal[counterRegIn]+corRunletXSlopeRegInVal[counterRegIn]+corRunletYRegInVal[counterRegIn]+corRunletYSlopeRegInVal[counterRegIn]+corRunletERegInVal[counterRegIn]+chargeRegInVal[counterRegIn];
    ecorRunletTotalRegInVal[counterRegIn]      =  0;
 //    ecorRunletTotalRegInVal[counterRegIn]      =  TMath::Sqrt(pow(ecorRunletXRegInVal[counterRegIn],2)+pow(ecorRunletXSlopeRegInVal[counterRegIn],2)+pow(ecorRunletYRegInVal[counterRegIn],2)+pow(ecorRunletYSlopeRegInVal[counterRegIn],2)+pow(ecorRunletERegInVal[counterRegIn],2)+pow(echargeRegInVal[counterRegIn],2));

    corSlugXRegInVal[counterRegIn]           =   diffXCalcUnRegInVal[counterRegIn]*avgSenXIn[counterAvg-1];
    ecorSlugXRegInVal[counterRegIn]          =   TMath::Sqrt(pow((diffXCalcUnRegInVal[counterRegIn]*eavgSenXIn[counterAvg-1]),2)+pow((ediffXCalcUnRegInVal[counterRegIn]*avgSenXIn[counterAvg-1]),2));
    corSlugXSlopeRegInVal[counterRegIn]      =   diffXSlopeCalcUnRegInVal[counterRegIn]*avgSenXSlopeIn[counterAvg-1];
    ecorSlugXSlopeRegInVal[counterRegIn]     =   TMath::Sqrt(pow(diffXSlopeCalcUnRegInVal[counterRegIn]*eavgSenXSlopeIn[counterAvg-1],2)+pow(ediffXSlopeCalcUnRegInVal[counterRegIn]*avgSenXSlopeIn[counterAvg-1],2));
    corSlugYRegInVal[counterRegIn]           =   diffYCalcUnRegInVal[counterRegIn]*avgSenYIn[counterAvg-1];
    ecorSlugYRegInVal[counterRegIn]          =   TMath::Sqrt(pow(diffYCalcUnRegInVal[counterRegIn]*eavgSenYIn[counterAvg-1],2)+pow(ediffYCalcUnRegInVal[counterRegIn]*avgSenYIn[counterAvg-1],2));
    corSlugYSlopeRegInVal[counterRegIn]      =   diffYSlopeCalcUnRegInVal[counterRegIn]*avgSenYSlopeIn[counterAvg-1];
    ecorSlugYSlopeRegInVal[counterRegIn]     =   TMath::Sqrt(pow(diffYSlopeCalcUnRegInVal[counterRegIn]*eavgSenYSlopeIn[counterAvg-1],2)+pow(ediffYSlopeCalcUnRegInVal[counterRegIn]*avgSenYSlopeIn[counterAvg-1],2));
    corSlugERegInVal[counterRegIn]           =   diffECalcUnRegInVal[counterRegIn]*avgSenEIn[counterAvg-1]*1e6;
    ecorSlugERegInVal[counterRegIn]          =   TMath::Sqrt(pow(diffECalcUnRegInVal[counterRegIn]*eavgSenEIn[counterAvg-1]*1e6,2)+pow(ediffECalcUnRegInVal[counterRegIn]*avgSenEIn[counterAvg-1]*1e6,2));
    corSlugTotalRegInVal[counterRegIn]       =   corSlugXRegInVal[counterRegIn]+corSlugXSlopeRegInVal[counterRegIn]+corSlugYRegInVal[counterRegIn]+corSlugYSlopeRegInVal[counterRegIn]+corSlugERegInVal[counterRegIn]+avgChargeIn[counterAvg-1];
    ecorSlugTotalRegInVal[counterRegIn]      =  0;
 //    ecorSlugTotalRegInVal[counterRegIn]      =  TMath::Sqrt(pow(ecorSlugXRegInVal[counterRegIn],2)+pow(ecorSlugXSlopeRegInVal[counterRegIn],2)+pow(ecorSlugYRegInVal[counterRegIn],2)+pow(ecorSlugYSlopeRegInVal[counterRegIn],2)+pow(ecorSlugERegInVal[counterRegIn],2)+pow(eavgChargeIn[counterAvg-1],2));

    mdAsymRunletCorIn[counterRegIn]          =   mdAsymCalcUnRegInVal[counterRegIn]-corRunletTotalRegInVal[counterRegIn];
    emdAsymRunletCorIn[counterRegIn]         =   emdAsymRegInVal[counterRegIn];

    mdAsymSlugCorIn[counterRegIn]            =   mdAsymCalcUnRegInVal[counterRegIn]-corSlugTotalRegInVal[counterRegIn];
    emdAsymSlugCorIn[counterRegIn]           =   emdAsymRegInVal[counterRegIn];

    mdAsymSlugAvgCorIn[counterRegIn]         =   mdAsymCalcUnRegInVal[counterRegIn]-avgCorTotalIn[counterAvg-1];
//     mdAsymSlugAvgCorIn[counterRegIn]         =   mdAsymCalcUnRegInVal[counterRegIn]-((avgSenXIn[counterAvg-1]*avgDiffXIn[counterAvg-1])+ (avgSenXSlopeIn[counterAvg-1]*avgDiffXSlopeIn[counterAvg-1])+ (avgSenYIn[counterAvg-1]*avgDiffYIn[counterAvg-1]) + (avgSenYSlopeIn[counterAvg-1]*avgDiffYSlopeIn[counterAvg-1]) + (avgSenEIn[counterAvg-1]*avgDiffEIn[counterAvg-1]) );
    emdAsymSlugAvgCorIn[counterRegIn]        =   emdAsymRegInVal[counterRegIn];

//     checkXXp->Fill(senXRegInVal[counterRegIn],senXSlopeRegInVal[counterRegIn],1);

    counterRegIn++;
  }
  inAllReg.close();

  while(1) {
    zeroRegOut[counterRegOut]=0;

    outAllReg >> runNumberRegOut[counterRegOut];
    outAllReg >> mdAsymRegOut[counterRegOut];
    outAllReg >> emdAsymRegOut[counterRegOut];
    outAllReg >> mdAsymWRegOut[counterRegOut];
    outAllReg >> mdAsymCalcUnRegOut[counterRegOut];
    outAllReg >> emdAsymCalcUnRegOut[counterRegOut];
    outAllReg >> mdAsymWCalcUnRegOut[counterRegOut];
    outAllReg >> diffXCalcUnRegOut[counterRegOut];
    outAllReg >> ediffXCalcUnRegOut[counterRegOut];
    outAllReg >> diffXSlopeCalcUnRegOut[counterRegOut];
    outAllReg >> ediffXSlopeCalcUnRegOut[counterRegOut];
    outAllReg >> diffYCalcUnRegOut[counterRegOut];
    outAllReg >> ediffYCalcUnRegOut[counterRegOut];
    outAllReg >> diffYSlopeCalcUnRegOut[counterRegOut];
    outAllReg >> ediffYSlopeCalcUnRegOut[counterRegOut];
    outAllReg >> diffECalcUnRegOut[counterRegOut];
    outAllReg >> ediffECalcUnRegOut[counterRegOut];
    outAllReg >> diffXRegOut[counterRegOut];
    outAllReg >> ediffXRegOut[counterRegOut];
    outAllReg >> diffXSlopeRegOut[counterRegOut];
    outAllReg >> ediffXSlopeRegOut[counterRegOut];
    outAllReg >> diffYRegOut[counterRegOut];
    outAllReg >> ediffYRegOut[counterRegOut];
    outAllReg >> diffYSlopeRegOut[counterRegOut];
    outAllReg >> ediffYSlopeRegOut[counterRegOut];
    outAllReg >> diffERegOut[counterRegOut];
    outAllReg >> ediffERegOut[counterRegOut];
    outAllReg >> senXRegOut[counterRegOut];
    outAllReg >> esenXRegOut[counterRegOut];
    outAllReg >> senXSlopeRegOut[counterRegOut];
    outAllReg >> esenXSlopeRegOut[counterRegOut];
    outAllReg >> senYRegOut[counterRegOut];
    outAllReg >> esenYRegOut[counterRegOut];
    outAllReg >> senYSlopeRegOut[counterRegOut];
    outAllReg >> esenYSlopeRegOut[counterRegOut];
    outAllReg >> senERegOut[counterRegOut];
    outAllReg >> esenERegOut[counterRegOut];
    outAllReg >> chargeRegOut[counterRegOut];
    outAllReg >> echargeRegOut[counterRegOut];
    outAllReg >> bcm1YRegOut[counterRegOut];
    outAllReg >> ebcm1YRegOut[counterRegOut];

    if (!outAllReg.good()) break;

    mdAsymRegOutVal[counterRegOut]         =   mdAsymRegOut[counterRegOut]*1e6;
    emdAsymRegOutVal[counterRegOut]        =   emdAsymRegOut[counterRegOut]*1e6;
    mdAsymWRegOutVal[counterRegOut]        =   mdAsymWRegOut[counterRegOut]*1e6;
    mdAsymCalcUnRegOutVal[counterRegOut]   =   mdAsymCalcUnRegOut[counterRegOut]*1e6;
    emdAsymCalcUnRegOutVal[counterRegOut]  =   emdAsymCalcUnRegOut[counterRegOut]*1e6;
    mdAsymWCalcUnRegOutVal[counterRegOut]  =   mdAsymWCalcUnRegOut[counterRegOut]*1e6;
    diffXCalcUnRegOutVal[counterRegOut]           =   diffXCalcUnRegOut[counterRegOut];
    ediffXCalcUnRegOutVal[counterRegOut]          =   ediffXCalcUnRegOut[counterRegOut];
    diffXSlopeCalcUnRegOutVal[counterRegOut]      =   diffXSlopeCalcUnRegOut[counterRegOut]*1e6;
    ediffXSlopeCalcUnRegOutVal[counterRegOut]     =   ediffXSlopeCalcUnRegOut[counterRegOut]*1e6;
    diffYCalcUnRegOutVal[counterRegOut]           =   diffYCalcUnRegOut[counterRegOut];
    ediffYCalcUnRegOutVal[counterRegOut]          =   ediffYCalcUnRegOut[counterRegOut];
    diffYSlopeCalcUnRegOutVal[counterRegOut]      =   diffYSlopeCalcUnRegOut[counterRegOut]*1e6;
    ediffYSlopeCalcUnRegOutVal[counterRegOut]     =   ediffYSlopeCalcUnRegOut[counterRegOut]*1e6;
    diffECalcUnRegOutVal[counterRegOut]           =   diffECalcUnRegOut[counterRegOut];
    ediffECalcUnRegOutVal[counterRegOut]          =   ediffECalcUnRegOut[counterRegOut];
    diffXRegOutVal[counterRegOut]          =   diffXRegOut[counterRegOut];
    ediffXRegOutVal[counterRegOut]         =   ediffXRegOut[counterRegOut];
    diffXSlopeRegOutVal[counterRegOut]     =   diffXSlopeRegOut[counterRegOut]*1e6;
    ediffXSlopeRegOutVal[counterRegOut]    =   ediffXSlopeRegOut[counterRegOut]*1e6;
    diffYRegOutVal[counterRegOut]          =   diffYRegOut[counterRegOut];
    ediffYRegOutVal[counterRegOut]         =   ediffYRegOut[counterRegOut];
    diffYSlopeRegOutVal[counterRegOut]     =   diffYSlopeRegOut[counterRegOut]*1e6;
    ediffYSlopeRegOutVal[counterRegOut]    =   ediffYSlopeRegOut[counterRegOut]*1e6;
    diffERegOutVal[counterRegOut]          =   diffERegOut[counterRegOut];
    ediffERegOutVal[counterRegOut]         =   ediffERegOut[counterRegOut];
    senXRegOutVal[counterRegOut]           =   senXRegOut[counterRegOut]*1e6;
    esenXRegOutVal[counterRegOut]          =   esenXRegOut[counterRegOut]*1e6;
    senXSlopeRegOutVal[counterRegOut]      =   senXSlopeRegOut[counterRegOut];
    esenXSlopeRegOutVal[counterRegOut]     =   esenXSlopeRegOut[counterRegOut];
    senYRegOutVal[counterRegOut]           =   senYRegOut[counterRegOut]*1e6;
    esenYRegOutVal[counterRegOut]          =   esenYRegOut[counterRegOut]*1e6;
    senYSlopeRegOutVal[counterRegOut]      =   senYSlopeRegOut[counterRegOut];
    esenYSlopeRegOutVal[counterRegOut]     =   esenYSlopeRegOut[counterRegOut];
    senERegOutVal[counterRegOut]           =   senERegOut[counterRegOut];
    esenERegOutVal[counterRegOut]          =   esenERegOut[counterRegOut];
    chargeRegOutVal[counterRegOut]         =   chargeRegOut[counterRegOut];
    echargeRegOutVal[counterRegOut]        =   echargeRegOut[counterRegOut];
    mdAsymWNormRegOutVal[counterRegOut]    =   mdAsymWRegOutVal[counterRegOut]/TMath::Sqrt(bcm1YRegOut[counterRegOut]/160);
    mdAsymWCalcNormUnRegOutVal[counterRegOut]     =   mdAsymWCalcUnRegOutVal[counterRegOut]/TMath::Sqrt(bcm1YRegOut[counterRegOut]/160);

    mdAsymWCalcDiffOutVal[counterRegOut] = mdAsymWCalcUnRegOutVal[counterRegOut] - mdAsymWRegOutVal[counterRegOut];
    mdAsymWNormDiffOutVal[counterRegOut] = mdAsymWCalcNormUnRegOutVal[counterRegOut] - mdAsymWNormRegOutVal[counterRegOut];

    corRunletXRegOutVal[counterRegOut]           =   diffXCalcUnRegOutVal[counterRegOut]*senXRegOutVal[counterRegOut];
    ecorRunletXRegOutVal[counterRegOut]          =   TMath::Sqrt(diffXCalcUnRegOutVal[counterRegOut]*esenXRegOutVal[counterRegOut]*diffXCalcUnRegOutVal[counterRegOut]*esenXRegOutVal[counterRegOut]+ediffXCalcUnRegOutVal[counterRegOut]*senXRegOutVal[counterRegOut]*ediffXCalcUnRegOutVal[counterRegOut]*senXRegOutVal[counterRegOut]);
    corRunletXSlopeRegOutVal[counterRegOut]      =   diffXSlopeCalcUnRegOutVal[counterRegOut]*senXSlopeRegOutVal[counterRegOut];
    ecorRunletXSlopeRegOutVal[counterRegOut]     =   TMath::Sqrt(diffXSlopeCalcUnRegOutVal[counterRegOut]*esenXSlopeRegOutVal[counterRegOut]*diffXSlopeCalcUnRegOutVal[counterRegOut]*esenXSlopeRegOutVal[counterRegOut]+ediffXSlopeCalcUnRegOutVal[counterRegOut]*senXSlopeRegOutVal[counterRegOut]*ediffXSlopeCalcUnRegOutVal[counterRegOut]*senXSlopeRegOutVal[counterRegOut]);
    corRunletYRegOutVal[counterRegOut]           =   diffYCalcUnRegOutVal[counterRegOut]*senYRegOutVal[counterRegOut];
    ecorRunletYRegOutVal[counterRegOut]          =   TMath::Sqrt(diffYCalcUnRegOutVal[counterRegOut]*esenYRegOutVal[counterRegOut]*diffYCalcUnRegOutVal[counterRegOut]*esenYRegOutVal[counterRegOut]+ediffYCalcUnRegOutVal[counterRegOut]*senYRegOutVal[counterRegOut]*ediffYCalcUnRegOutVal[counterRegOut]*senYRegOutVal[counterRegOut]);
    corRunletYSlopeRegOutVal[counterRegOut]      =   diffYSlopeCalcUnRegOutVal[counterRegOut]*senYSlopeRegOutVal[counterRegOut];
    ecorRunletYSlopeRegOutVal[counterRegOut]     =   TMath::Sqrt(diffYSlopeCalcUnRegOutVal[counterRegOut]*esenYSlopeRegOutVal[counterRegOut]*diffYSlopeCalcUnRegOutVal[counterRegOut]*esenYSlopeRegOutVal[counterRegOut]+ediffYSlopeCalcUnRegOutVal[counterRegOut]*senYSlopeRegOutVal[counterRegOut]*ediffYSlopeCalcUnRegOutVal[counterRegOut]*senYSlopeRegOutVal[counterRegOut]);
    corRunletERegOutVal[counterRegOut]           =   diffECalcUnRegOutVal[counterRegOut]*senERegOutVal[counterRegOut]*1e6;
    ecorRunletERegOutVal[counterRegOut]          =   TMath::Sqrt(diffECalcUnRegOutVal[counterRegOut]*esenERegOutVal[counterRegOut]*diffECalcUnRegOutVal[counterRegOut]*esenERegOutVal[counterRegOut]*1e12+ediffECalcUnRegOutVal[counterRegOut]*senERegOutVal[counterRegOut]*ediffECalcUnRegOutVal[counterRegOut]*senERegOutVal[counterRegOut]*1e12);
    corRunletTotalRegOutVal[counterRegOut]       =   corRunletXRegOutVal[counterRegOut]+corRunletXSlopeRegOutVal[counterRegOut]+corRunletYRegOutVal[counterRegOut]+corRunletYSlopeRegOutVal[counterRegOut]+corRunletERegOutVal[counterRegOut]+chargeRegOutVal[counterRegOut];
    ecorRunletTotalRegOutVal[counterRegOut]      =  0;
//     ecorRunletTotalRegOutVal[counterRegOut]      =  TMath::Sqrt(pow(ecorRunletXRegOutVal[counterRegOut],2)+pow(ecorRunletXSlopeRegOutVal[counterRegOut],2)+pow(ecorRunletYRegOutVal[counterRegOut],2)+pow(ecorRunletYSlopeRegOutVal[counterRegOut],2)+pow(ecorRunletERegOutVal[counterRegOut],2)+pow(echargeRegOutVal[counterRegOut],2));

    corSlugXRegOutVal[counterRegOut]           =   diffXCalcUnRegOutVal[counterRegOut]*avgSenXOut[counterAvg-1];
    ecorSlugXRegOutVal[counterRegOut]          =   TMath::Sqrt(pow(diffXCalcUnRegOutVal[counterRegOut]*eavgSenXOut[counterAvg-1],2)+pow(ediffXCalcUnRegOutVal[counterRegOut]*avgSenXOut[counterAvg-1],2));
    corSlugXSlopeRegOutVal[counterRegOut]      =   diffXSlopeCalcUnRegOutVal[counterRegOut]*avgSenXSlopeOut[counterAvg-1];
    ecorSlugXSlopeRegOutVal[counterRegOut]     =   TMath::Sqrt(pow(diffXSlopeCalcUnRegOutVal[counterRegOut]*eavgSenXSlopeOut[counterAvg-1],2)+pow(ediffXSlopeCalcUnRegOutVal[counterRegOut]*avgSenXSlopeOut[counterAvg-1],2));
    corSlugYRegOutVal[counterRegOut]           =   diffYCalcUnRegOutVal[counterRegOut]*avgSenYOut[counterAvg-1];
    ecorSlugYRegOutVal[counterRegOut]          =   TMath::Sqrt(pow(diffYCalcUnRegOutVal[counterRegOut]*eavgSenYOut[counterAvg-1],2)+pow(ediffYCalcUnRegOutVal[counterRegOut]*avgSenYOut[counterAvg-1],2));
    corSlugYSlopeRegOutVal[counterRegOut]      =   diffYSlopeCalcUnRegOutVal[counterRegOut]*avgSenYSlopeOut[counterAvg-1];
    ecorSlugYSlopeRegOutVal[counterRegOut]     =   TMath::Sqrt(pow(diffYSlopeCalcUnRegOutVal[counterRegOut]*eavgSenYSlopeOut[counterAvg-1],2)+pow(ediffYSlopeCalcUnRegOutVal[counterRegOut]*avgSenYSlopeOut[counterAvg-1],2));
    corSlugERegOutVal[counterRegOut]           =   diffECalcUnRegOutVal[counterRegOut]*avgSenEOut[counterAvg-1]*1e6;
    ecorSlugERegOutVal[counterRegOut]          =   TMath::Sqrt(pow(diffECalcUnRegOutVal[counterRegOut]*eavgSenEOut[counterAvg-1]*1e6,2)+pow(ediffECalcUnRegOutVal[counterRegOut]*avgSenEOut[counterAvg-1]*1e6,2));
    corSlugTotalRegOutVal[counterRegOut]       =   corSlugXRegOutVal[counterRegOut]+corSlugXSlopeRegOutVal[counterRegOut]+corSlugYRegOutVal[counterRegOut]+corSlugYSlopeRegOutVal[counterRegOut]+corSlugERegOutVal[counterRegOut]+avgChargeOut[counterAvg-1];
    ecorSlugTotalRegOutVal[counterRegOut]      =  0;
//     ecorSlugTotalRegOutVal[counterRegOut]      =  TMath::Sqrt(pow(ecorSlugXRegOutVal[counterRegOut],2)+pow(ecorSlugXSlopeRegOutVal[counterRegOut],2)+pow(ecorSlugYRegOutVal[counterRegOut],2)+pow(ecorSlugYSlopeRegOutVal[counterRegOut],2)+pow(ecorSlugERegOutVal[counterRegOut],2)+pow(eavgChargeOut[counterAvg-1],2));

    mdAsymRunletCorOut[counterRegOut]          =   mdAsymCalcUnRegOutVal[counterRegOut]-corRunletTotalRegOutVal[counterRegOut];
    emdAsymRunletCorOut[counterRegOut]         =   emdAsymRegOutVal[counterRegOut];

    mdAsymSlugCorOut[counterRegOut]            =   mdAsymCalcUnRegOutVal[counterRegOut]-corSlugTotalRegOutVal[counterRegOut];
    emdAsymSlugCorOut[counterRegOut]           =   emdAsymRegOutVal[counterRegOut];

//     mdAsymSlugAvgCorOut[counterRegOut]         =   mdAsymCalcUnRegOutVal[counterRegOut]-0.874355;        //Error Weighted
    mdAsymSlugAvgCorOut[counterRegOut]         =   mdAsymCalcUnRegOutVal[counterRegOut]-avgCorTotalOut[counterAvg-1];        //Error Un-Weighted
    emdAsymSlugAvgCorOut[counterRegOut]        =   emdAsymRegOutVal[counterRegOut];

    counterRegOut++;
  }
  outAllReg.close();

  /*********************************************************************************/

  const Int_t NUM = 10000;
  int counter=0;
  ifstream inSenCorXXSlope,outSenCorXXSlope,inSenCorYYSlope,outSenCorYYSlope,inSenCorXE,outSenCorXE,inOctantAvgSen,outOctantAvgSen,inAvgDiff,outAvgDiff;
  inOctantAvgSen.open((Form("%s/dbText/%sOctantSensitivityReg5In.txt",n2deltaAnalysisDir.Data(),savePlot)));
  outOctantAvgSen.open((Form("%s/dbText/%sOctantSensitivityReg5Out.txt",n2deltaAnalysisDir.Data(),savePlot)));
  inAvgDiff.open((Form("%s/dataBaseAnalysis/%sOctantDiffUnRegIn.txt",n2deltaAnalysisDir.Data(),savePlot)));
  outAvgDiff.open((Form("%s/dataBaseAnalysis/%sOctantDiffUnRegOut.txt",n2deltaAnalysisDir.Data(),savePlot)));

  Double_t zero[NUM],run_number_in[NUM],det_number_in[NUM]; 
  Double_t run_number_out[NUM],det_number_out[NUM];
  Double_t X_in[NUM],eX_in[NUM],XSlope_in[NUM],eXSlope_in[NUM];
  Double_t X_out[NUM],eX_out[NUM],XSlope_out[NUM],eXSlope_out[NUM];
  Double_t Y_in[NUM],eY_in[NUM],YSlope_in[NUM],eYSlope_in[NUM];
  Double_t Y_out[NUM],eY_out[NUM],YSlope_out[NUM],eYSlope_out[NUM];
  Double_t E_in[NUM],eE_in[NUM];
  Double_t E_out[NUM],eE_out[NUM];
  Double_t Charge_in[NUM],eCharge_in[NUM];
  Double_t Charge_out[NUM],eCharge_out[NUM];
  Double_t X_inAmp[NUM],eX_inAmp[NUM],XSlope_inAmp[NUM],eXSlope_inAmp[NUM];
  Double_t X_outAmp[NUM],eX_outAmp[NUM],XSlope_outAmp[NUM],eXSlope_outAmp[NUM];
  Double_t Y_inAmp[NUM],eY_inAmp[NUM],YSlope_inAmp[NUM],eYSlope_inAmp[NUM];
  Double_t Y_outAmp[NUM],eY_outAmp[NUM],YSlope_outAmp[NUM],eYSlope_outAmp[NUM];
  Double_t E_inAmp[NUM],eE_inAmp[NUM];
  Double_t E_outAmp[NUM],eE_outAmp[NUM];
  Double_t Charge_inAmp[NUM],eCharge_inAmp[NUM];
  Double_t Charge_outAmp[NUM],eCharge_outAmp[NUM];

  Double_t XDiff_in[NUM],eXDiff_in[NUM],XSlopeDiff_in[NUM],eXSlopeDiff_in[NUM];
  Double_t XDiff_out[NUM],eXDiff_out[NUM],XSlopeDiff_out[NUM],eXSlopeDiff_out[NUM];
  Double_t YDiff_in[NUM],eYDiff_in[NUM],YSlopeDiff_in[NUM],eYSlopeDiff_in[NUM];
  Double_t YDiff_out[NUM],eYDiff_out[NUM],YSlopeDiff_out[NUM],eYSlopeDiff_out[NUM];
  Double_t EDiff_in[NUM],eEDiff_in[NUM];

  Double_t X_inCor[NUM],eX_inCor[NUM],XSlope_inCor[NUM],eXSlope_inCor[NUM];
  Double_t X_outCor[NUM],eX_outCor[NUM],XSlope_outCor[NUM],eXSlope_outCor[NUM];
  Double_t Y_inCor[NUM],eY_inCor[NUM],YSlope_inCor[NUM],eYSlope_inCor[NUM];
  Double_t Y_outCor[NUM],eY_outCor[NUM],YSlope_outCor[NUM],eYSlope_outCor[NUM];
  Double_t E_inCor[NUM],eE_inCor[NUM];
  Double_t E_outCor[NUM],eE_outCor[NUM];
  Double_t Charge_inCor[NUM],eCharge_inCor[NUM];
  Double_t Charge_outCor[NUM],eCharge_outCor[NUM];
  Double_t Total_inCor[NUM],eTotal_inCor[NUM];
  Double_t Total_outCor[NUM],eTotal_outCor[NUM];

  Double_t X_inCorDoublet[NUM],eX_inCorDoublet[NUM];
  Double_t X_outCorDoublet[NUM],eX_outCorDoublet[NUM];
  Double_t Y_inCorDoublet[NUM],eY_inCorDoublet[NUM];
  Double_t Y_outCorDoublet[NUM],eY_outCorDoublet[NUM];

  while(1) {
    zero[counter]=0;

    inOctantAvgSen >> det_number_in[counter];
    inOctantAvgSen >> X_in[counter];
    inOctantAvgSen >> eX_in[counter];
    inOctantAvgSen >> XSlope_in[counter];
    inOctantAvgSen >> eXSlope_in[counter];
    inOctantAvgSen >> Y_in[counter];
    inOctantAvgSen >> eY_in[counter];
    inOctantAvgSen >> YSlope_in[counter];
    inOctantAvgSen >> eYSlope_in[counter];
    inOctantAvgSen >> E_in[counter];
    inOctantAvgSen >> eE_in[counter];
    inOctantAvgSen >> Charge_in[counter];
    inOctantAvgSen >> eCharge_in[counter];
 
    if (!inOctantAvgSen.good()) break;

    outOctantAvgSen >> det_number_out[counter];
    outOctantAvgSen >> X_out[counter];
    outOctantAvgSen >> eX_out[counter];
    outOctantAvgSen >> XSlope_out[counter];
    outOctantAvgSen >> eXSlope_out[counter];
    outOctantAvgSen >> Y_out[counter];
    outOctantAvgSen >> eY_out[counter];
    outOctantAvgSen >> YSlope_out[counter];
    outOctantAvgSen >> eYSlope_out[counter];
    outOctantAvgSen >> E_out[counter];
    outOctantAvgSen >> eE_out[counter];
    outOctantAvgSen >> Charge_out[counter];
    outOctantAvgSen >> eCharge_out[counter];

   if (!outOctantAvgSen.good()) break;


   X_inAmp[counter]         =    X_in[counter]*1e6;
   X_outAmp[counter]        =    X_out[counter]*1e6;
   Y_inAmp[counter]         =    Y_in[counter]*1e6;
   Y_outAmp[counter]        =    Y_out[counter]*1e6;
   XSlope_inAmp[counter]    =    XSlope_in[counter];
   XSlope_outAmp[counter]   =    XSlope_out[counter];
   YSlope_inAmp[counter]    =    YSlope_in[counter];
   YSlope_outAmp[counter]   =    YSlope_out[counter];
   E_inAmp[counter]         =    E_in[counter];
   E_outAmp[counter]        =    E_out[counter];
   Charge_inAmp[counter]    =    Charge_in[counter];
   Charge_outAmp[counter]   =    Charge_out[counter];

   eX_inAmp[counter]        =    eX_in[counter]*1e6;
   eX_outAmp[counter]       =    eX_out[counter]*1e6;
   eY_inAmp[counter]        =    eY_in[counter]*1e6;
   eY_outAmp[counter]       =    eY_out[counter]*1e6;
   eXSlope_inAmp[counter]   =    eXSlope_in[counter];
   eXSlope_outAmp[counter]  =    eXSlope_out[counter];
   eYSlope_inAmp[counter]   =    eYSlope_in[counter];
   eYSlope_outAmp[counter]  =    eYSlope_out[counter];
   eE_inAmp[counter]        =    eE_in[counter];
   eE_outAmp[counter]       =    eE_out[counter];
   eCharge_inAmp[counter]   =    eCharge_in[counter];
   eCharge_outAmp[counter]  =    eCharge_out[counter];

   X_inCor[counter]         =    X_inAmp[counter]*avgDiffXIn[counterAvg-1];
   X_outCor[counter]        =    X_outAmp[counter]*avgDiffXOut[counterAvg-1];
   XSlope_inCor[counter]    =    XSlope_inAmp[counter]*avgDiffXSlopeIn[counterAvg-1];
   XSlope_outCor[counter]   =    XSlope_outAmp[counter]*avgDiffXSlopeOut[counterAvg-1];
   Y_inCor[counter]         =    Y_inAmp[counter]*avgDiffYIn[counterAvg-1];
   Y_outCor[counter]        =    Y_outAmp[counter]*avgDiffYOut[counterAvg-1];
   YSlope_inCor[counter]    =    YSlope_inAmp[counter]*avgDiffYSlopeIn[counterAvg-1];
   YSlope_outCor[counter]   =    YSlope_outAmp[counter]*avgDiffYSlopeOut[counterAvg-1];
   E_inCor[counter]         =    E_inAmp[counter]*avgDiffEIn[counterAvg-1]*1e6;
   E_outCor[counter]        =    E_outAmp[counter]*avgDiffEOut[counterAvg-1]*1e6;
   Charge_inCor[counter]    =    Charge_inAmp[counter];
   Charge_outCor[counter]   =    Charge_outAmp[counter];

   Total_inCor[counter]     =    X_inCor[counter]+Y_inCor[counter]+XSlope_inCor[counter]+YSlope_inCor[counter]+E_inCor[counter]+Charge_inCor[counter];
   Total_outCor[counter]    =    X_outCor[counter]+Y_outCor[counter]+XSlope_outCor[counter]+YSlope_outCor[counter]+E_outCor[counter]+Charge_outCor[counter];

   eX_inCor[counter]        =    TMath::Sqrt(pow(eX_inAmp[counter]*avgDiffXIn[counterAvg-1],2)+pow(X_inAmp[counter]*eavgDiffXIn[counterAvg-1],2));
   eX_outCor[counter]       =    TMath::Sqrt(pow(eX_outAmp[counter]*avgDiffXOut[counterAvg-1],2)+pow(X_outAmp[counter]*eavgDiffXOut[counterAvg-1],2));
   eXSlope_inCor[counter]   =    TMath::Sqrt(pow(eXSlope_inAmp[counter]*avgDiffXSlopeIn[counterAvg-1],2)+pow(XSlope_inAmp[counter]*eavgDiffXSlopeIn[counterAvg-1],2));
   eXSlope_outCor[counter]  =    TMath::Sqrt(pow(eXSlope_outAmp[counter]*avgDiffXSlopeIn[counterAvg-1],2)+pow(XSlope_outAmp[counter]*eavgDiffXSlopeIn[counterAvg-1],2));
   eY_inCor[counter]        =    TMath::Sqrt(pow(eY_inAmp[counter]*avgDiffYIn[counterAvg-1],2)+pow(Y_inAmp[counter]*eavgDiffYIn[counterAvg-1],2));
   eY_outCor[counter]       =    TMath::Sqrt(pow(eY_outAmp[counter]*avgDiffYOut[counterAvg-1],2)+pow(Y_outAmp[counter]*eavgDiffYOut[counterAvg-1],2));
   eYSlope_inCor[counter]   =    TMath::Sqrt(pow(eYSlope_inAmp[counter]*avgDiffYSlopeIn[counterAvg-1],2)+pow(YSlope_inAmp[counter]*eavgDiffXSlopeIn[counterAvg-1],2));
   eYSlope_outCor[counter]  =    TMath::Sqrt(pow(eYSlope_outAmp[counter]*avgDiffYSlopeOut[counterAvg-1],2)+pow(YSlope_outAmp[counter]*eavgDiffXSlopeOut[counterAvg-1],2));
   eE_inCor[counter]        =    TMath::Sqrt(pow(eE_inAmp[counter]*avgDiffEIn[counterAvg-1],2)+pow(E_inAmp[counter]*eavgDiffEIn[counterAvg-1],2));
   eE_outCor[counter]       =    TMath::Sqrt(pow(eE_outAmp[counter]*avgDiffEOut[counterAvg-1],2)+pow(E_outAmp[counter]*eavgDiffEOut[counterAvg-1],2));
   eTotal_inCor[counter]    =    TMath::Sqrt(pow(eX_inCor[counter],2)+pow(eXSlope_inCor[counter],2)+pow(eY_inCor[counter],2)+pow(eYSlope_inCor[counter],2)+pow(eE_inCor[counter],2)+pow(eCharge_inCor[counter],2));
   eTotal_outCor[counter]   =    TMath::Sqrt(pow(eX_outCor[counter],2)+pow(eXSlope_outCor[counter],2)+pow(eY_outCor[counter],2)+pow(eYSlope_outCor[counter],2)+pow(eE_outCor[counter],2)+pow(eCharge_outCor[counter],2));

   X_inCorDoublet[counter]   = X_inCor[counter]+XSlope_inCor[counter];
   Y_inCorDoublet[counter]   = Y_inCor[counter]+YSlope_inCor[counter];
   eX_inCorDoublet[counter]  = TMath::Sqrt(pow(eX_inCor[counter],2)+pow(eXSlope_inCor[counter],2));
   eY_inCorDoublet[counter]  = TMath::Sqrt(pow(eY_inCor[counter],2)+pow(eYSlope_inCor[counter],2));
   X_outCorDoublet[counter]  = X_outCor[counter]+XSlope_outCor[counter];
   Y_outCorDoublet[counter]  = Y_outCor[counter]+YSlope_outCor[counter];
   eX_outCorDoublet[counter] = TMath::Sqrt(pow(eX_outCor[counter],2)+pow(eXSlope_outCor[counter],2));
   eY_outCorDoublet[counter] = TMath::Sqrt(pow(eY_outCor[counter],2)+pow(eYSlope_outCor[counter],2));

    counter++;
  }

  inOctantAvgSen.close();
  outOctantAvgSen.close();
  inAvgDiff.close();
  outAvgDiff.close();

  /*********************************************************************************/

  Double_t octantNumberIn[NUM],zeroOctantIn[NUM];
  Double_t mdAsymUnRegOctantIn[NUM];
  Double_t emdAsymUnRegOctantIn[NUM];
  Double_t mdAsymRegOctantIn[NUM];
  Double_t emdAsymRegOctantIn[NUM];
  Double_t octantNumberOut[NUM],zeroOctantOut[NUM];
  Double_t mdAsymUnRegOctantOut[NUM];
  Double_t emdAsymUnRegOctantOut[NUM];
  Double_t mdAsymRegOctantOut[NUM];
  Double_t emdAsymRegOctantOut[NUM];

  Double_t mdAsymUnRegOctantInVal[NUM];
  Double_t emdAsymUnRegOctantInVal[NUM];
  Double_t mdAsymRegOctantInVal[NUM];
  Double_t emdAsymRegOctantInVal[NUM];
  Double_t mdAsymUnRegOctantOutVal[NUM];
  Double_t emdAsymUnRegOctantOutVal[NUM];
  Double_t mdAsymRegOctantOutVal[NUM];
  Double_t emdAsymRegOctantOutVal[NUM];

  Double_t mdAsymUnRegOctantInpOut[NUM];
  Double_t emdAsymUnRegOctantInpOut[NUM];
  Double_t mdAsymRegOctantInpOut[NUM];
  Double_t emdAsymRegOctantInpOut[NUM];
  Double_t mdAsymUnRegOctantInmOut[NUM];
  Double_t emdAsymUnRegOctantInmOut[NUM];
  Double_t mdAsymRegOctantInmOut[NUM];
  Double_t emdAsymRegOctantInmOut[NUM];

  Double_t mdAsymCorOctantIn[NUM];
  Double_t emdAsymCorOctantIn[NUM];
  Double_t mdAsymCorOctantOut[NUM];
  Double_t emdAsymCorOctantOut[NUM];
  Double_t mdAsymCorOctantInpOut[NUM];
  Double_t emdAsymCorOctantInpOut[NUM];
  Double_t mdAsymCorOctantInmOut[NUM];
  Double_t emdAsymCorOctantInmOut[NUM];

  int counterOctantOut=0,counterOctantIn=0;
  ifstream inAllOctant,outAllOctant;
  inAllOctant.open((Form("%s/dbText/%sallOctantIn%s.txt",n2deltaAnalysisDir.Data(),savePlot,regScheme)));
  outAllOctant.open((Form("%s/dbText/%sallOctantOut%s.txt",n2deltaAnalysisDir.Data(),savePlot,regScheme)));

  while(1) {
    zeroOctantIn[counterOctantIn]=0;

    inAllOctant >> octantNumberIn[counterOctantIn];
    inAllOctant >> mdAsymUnRegOctantIn[counterOctantIn];
    inAllOctant >> emdAsymUnRegOctantIn[counterOctantIn];
    inAllOctant >> mdAsymRegOctantIn[counterOctantIn];
    inAllOctant >> emdAsymRegOctantIn[counterOctantIn];

    if (!inAllOctant.good()) break;

    mdAsymUnRegOctantInVal[counterOctantIn]      =   mdAsymUnRegOctantIn[counterOctantIn];
    emdAsymUnRegOctantInVal[counterOctantIn]     =   emdAsymUnRegOctantIn[counterOctantIn];
    mdAsymRegOctantInVal[counterOctantIn]        =   mdAsymRegOctantIn[counterOctantIn];
    emdAsymRegOctantInVal[counterOctantIn]       =   emdAsymRegOctantIn[counterOctantIn];

    mdAsymCorOctantIn[counterOctantIn]           =   mdAsymUnRegOctantInVal[counterOctantIn]-Total_inCor[counterOctantIn];
    emdAsymCorOctantIn[counterOctantIn]          =   emdAsymRegOctantInVal[counterOctantIn];

    counterOctantIn++;
  }
  inAllOctant.close();

  while(1) {
    zeroOctantOut[counterOctantOut]=0;

    outAllOctant >> octantNumberOut[counterOctantOut];
    outAllOctant >> mdAsymUnRegOctantOut[counterOctantOut];
    outAllOctant >> emdAsymUnRegOctantOut[counterOctantOut];
    outAllOctant >> mdAsymRegOctantOut[counterOctantOut];
    outAllOctant >> emdAsymRegOctantOut[counterOctantOut];

    if (!outAllOctant.good()) break;

    mdAsymUnRegOctantOutVal[counterOctantOut]      =   mdAsymUnRegOctantOut[counterOctantOut];
    emdAsymUnRegOctantOutVal[counterOctantOut]     =   emdAsymUnRegOctantOut[counterOctantOut];
    mdAsymRegOctantOutVal[counterOctantOut]        =   mdAsymRegOctantOut[counterOctantOut];
    emdAsymRegOctantOutVal[counterOctantOut]       =   emdAsymRegOctantOut[counterOctantOut];

    mdAsymCorOctantOut[counterOctantOut]           =   mdAsymUnRegOctantOutVal[counterOctantOut]-Total_outCor[counterOctantOut];
    emdAsymCorOctantOut[counterOctantOut]          =   emdAsymRegOctantOutVal[counterOctantOut];

    counterOctantOut++;
  }
  outAllOctant.close();

  Int_t counterOctantInOut2=0;
  for(Int_t counterOctantInOut=0; counterOctantInOut<8; counterOctantInOut++) {

    counterOctantInOut2 = counterOctantInOut+1;

    mdAsymUnRegOctantInpOut[counterOctantInOut]   = (mdAsymUnRegOctantInVal[counterOctantInOut]+mdAsymUnRegOctantOutVal[counterOctantInOut])/2;
    mdAsymRegOctantInpOut[counterOctantInOut]     = (mdAsymRegOctantInVal[counterOctantInOut]+mdAsymRegOctantOutVal[counterOctantInOut])/2;

    mdAsymUnRegOctantInmOut[counterOctantInOut]   = ((mdAsymUnRegOctantInVal[counterOctantInOut]*(1/(emdAsymUnRegOctantInVal[counterOctantInOut]*emdAsymUnRegOctantInVal[counterOctantInOut])))-(mdAsymUnRegOctantOutVal[counterOctantInOut]*(1/(emdAsymUnRegOctantOutVal[counterOctantInOut]*emdAsymUnRegOctantOutVal[counterOctantInOut]))))/((1/(emdAsymUnRegOctantInVal[counterOctantInOut]*emdAsymUnRegOctantInVal[counterOctantInOut]))+(1/(emdAsymUnRegOctantOutVal[counterOctantInOut]*emdAsymUnRegOctantOutVal[counterOctantInOut])));
    mdAsymRegOctantInmOut[counterOctantInOut]     = ((mdAsymRegOctantInVal[counterOctantInOut]*(1/(emdAsymRegOctantInVal[counterOctantInOut]*emdAsymRegOctantInVal[counterOctantInOut])))-(mdAsymRegOctantOutVal[counterOctantInOut]*(1/(emdAsymRegOctantOutVal[counterOctantInOut]*emdAsymRegOctantOutVal[counterOctantInOut]))))/((1/(emdAsymRegOctantInVal[counterOctantInOut]*emdAsymRegOctantInVal[counterOctantInOut]))+(1/(emdAsymRegOctantOutVal[counterOctantInOut]*emdAsymRegOctantOutVal[counterOctantInOut])));

    emdAsymUnRegOctantInpOut[counterOctantInOut]  = TMath::Sqrt(emdAsymUnRegOctantInVal[counterOctantInOut]*emdAsymUnRegOctantInVal[counterOctantInOut]+emdAsymUnRegOctantOutVal[counterOctantInOut]*emdAsymUnRegOctantOutVal[counterOctantInOut])/2;
    emdAsymRegOctantInpOut[counterOctantInOut]    = TMath::Sqrt(emdAsymRegOctantInVal[counterOctantInOut]*emdAsymRegOctantInVal[counterOctantInOut]+emdAsymRegOctantOutVal[counterOctantInOut]*emdAsymRegOctantOutVal[counterOctantInOut])/2;

    emdAsymUnRegOctantInmOut[counterOctantInOut]  = 1/TMath::Sqrt(1/(emdAsymUnRegOctantInVal[counterOctantInOut]*emdAsymUnRegOctantInVal[counterOctantInOut])+1/(emdAsymUnRegOctantOutVal[counterOctantInOut]*emdAsymUnRegOctantOutVal[counterOctantInOut]));
    emdAsymRegOctantInmOut[counterOctantInOut]    = 1/TMath::Sqrt(1/(emdAsymRegOctantInVal[counterOctantInOut]*emdAsymRegOctantInVal[counterOctantInOut])+1/(emdAsymRegOctantOutVal[counterOctantInOut]*emdAsymRegOctantOutVal[counterOctantInOut]));


    mdAsymCorOctantInpOut[counterOctantInOut]     = (mdAsymCorOctantIn[counterOctantInOut]+mdAsymCorOctantOut[counterOctantInOut])/2;
    mdAsymCorOctantInmOut[counterOctantInOut]     = ((mdAsymCorOctantIn[counterOctantInOut]*(1/(emdAsymCorOctantIn[counterOctantInOut]*emdAsymCorOctantIn[counterOctantInOut])))-(mdAsymCorOctantOut[counterOctantInOut]*(1/(emdAsymCorOctantOut[counterOctantInOut]*emdAsymCorOctantOut[counterOctantInOut]))))/((1/(emdAsymCorOctantIn[counterOctantInOut]*emdAsymCorOctantIn[counterOctantInOut]))+(1/(emdAsymCorOctantOut[counterOctantInOut]*emdAsymCorOctantOut[counterOctantInOut])));
    emdAsymCorOctantInpOut[counterOctantInOut]    = TMath::Sqrt(emdAsymCorOctantIn[counterOctantInOut]*emdAsymCorOctantIn[counterOctantInOut]+emdAsymCorOctantOut[counterOctantInOut]*emdAsymCorOctantOut[counterOctantInOut])/2;
    emdAsymCorOctantInmOut[counterOctantInOut]    = 1/TMath::Sqrt(1/(emdAsymCorOctantIn[counterOctantInOut]*emdAsymCorOctantIn[counterOctantInOut])+1/(emdAsymCorOctantOut[counterOctantInOut]*emdAsymCorOctantOut[counterOctantInOut]));

//     printf("UnReg %d\t In+Out=%4.2f+-%4.2f\t In-Out=%4.2f+-%4.2f\n",counterOctantInOut2,mdAsymUnRegOctantInpOut[counterOctantInOut],emdAsymUnRegOctantInpOut[counterOctantInOut],mdAsymUnRegOctantInmOut[counterOctantInOut],emdAsymUnRegOctantInmOut[counterOctantInOut]);
//     printf("Reg %d\t In+Out=%4.2f+-%4.2f\t In-Out=%4.2f+-%4.2f\n",counterOctantInOut2,mdAsymRegOctantInpOut[counterOctantInOut],emdAsymRegOctantInpOut[counterOctantInOut],mdAsymRegOctantInmOut[counterOctantInOut],emdAsymRegOctantInmOut[counterOctantInOut]);
//     printf("Cor %d\t In+Out=%4.2f+-%4.2f\t In-Out=%4.2f+-%4.2f\n",counterOctantInOut2,mdAsymCorOctantInpOut[counterOctantInOut],emdAsymCorOctantInpOut[counterOctantInOut],mdAsymCorOctantInmOut[counterOctantInOut],emdAsymCorOctantInmOut[counterOctantInOut]);

  }

  /*********************************************************************************/

//   Double_t senOctant8XRegIn[NUM];
//   Double_t esenOctant8XRegIn[NUM];
//   Double_t senOctant8XSlopeRegIn[NUM];
//   Double_t esenOctant8XSlopeRegIn[NUM];
//   Double_t senOctant8YRegIn[NUM];
//   Double_t esenOctant8YRegIn[NUM];
//   Double_t senOctant8YSlopeRegIn[NUM];
//   Double_t esenOctant8YSlopeRegIn[NUM];
//   Double_t senOctant8ERegIn[NUM];
//   Double_t esenOctant8ERegIn[NUM];

//   Double_t senOctant8XRegInVal[NUM];
//   Double_t esenOctant8XRegInVal[NUM];
//   Double_t senOctant8XSlopeRegInVal[NUM];
//   Double_t esenOctant8XSlopeRegInVal[NUM];
//   Double_t senOctant8YRegInVal[NUM];
//   Double_t esenOctant8YRegInVal[NUM];
//   Double_t senOctant8YSlopeRegInVal[NUM];
//   Double_t esenOctant8YSlopeRegInVal[NUM];
//   Double_t senOctant8ERegInVal[NUM];
//   Double_t esenOctant8ERegInVal[NUM];

//   int counterOctant8In=0,counterOctant8Out=0;
//   ifstream inOctant8,outOctant8;
//   inOctant8.open((Form("%s/dataBaseAnalysis/Octant8In.txt",n2deltaAnalysisDir.Data())));
//   outOctant8.open((Form("%s/dataBaseAnalysis/Octant8Out.txt",n2deltaAnalysisDir.Data())));

//   while(1) {
//     zeroOctant8In[counterUnOctant8In]=0;

//     inOctant8Octant8 >> runNumberOctant8In[counterOctant8In];


//   }

  /*********************************************************************************/
  /*********************************************************************************/

  Double_t runNumberRunIn[NUM],zeroRunIn[NUM];
  Double_t mdAsymUnRegRunIn[NUM],emdAsymUnRegRunIn[NUM],mdAsymWUnRegRunIn[NUM],mdAsymWNormUnRegRunIn[NUM],mdAsymWDiffRunIn[NUM],mdAsymWNormDiffRunIn[NUM];
  Double_t mdAsymRegRunIn[NUM],emdAsymRegRunIn[NUM],mdAsymWRegRunIn[NUM],mdAsymWNormRegRunIn[NUM];
  Double_t diffXUnRegRunIn[NUM],diffXSlopeUnRegRunIn[NUM],diffYUnRegRunIn[NUM],diffYSlopeUnRegRunIn[NUM],diffEUnRegRunIn[NUM];
  Double_t ediffXUnRegRunIn[NUM],ediffXSlopeUnRegRunIn[NUM],ediffYUnRegRunIn[NUM],ediffYSlopeUnRegRunIn[NUM],ediffEUnRegRunIn[NUM];
  Double_t diffXRegRunIn[NUM],diffXSlopeRegRunIn[NUM],diffYRegRunIn[NUM],diffYSlopeRegRunIn[NUM],diffERegRunIn[NUM];
  Double_t ediffXRegRunIn[NUM],ediffXSlopeRegRunIn[NUM],ediffYRegRunIn[NUM],ediffYSlopeRegRunIn[NUM],ediffERegRunIn[NUM];
  Double_t senXRunIn[NUM],senXSlopeRunIn[NUM],senYRunIn[NUM],senYSlopeRunIn[NUM],senERunIn[NUM];
  Double_t esenXRunIn[NUM],esenXSlopeRunIn[NUM],esenYRunIn[NUM],esenYSlopeRunIn[NUM],esenERunIn[NUM];
  Double_t chargeRunIn[NUM],echargeRunIn[NUM],bcm1YRunIn[NUM],ebcm1YRunIn[NUM];

  Double_t runNumberRunOut[NUM],zeroRunOut[NUM];
  Double_t mdAsymUnRegRunOut[NUM],emdAsymUnRegRunOut[NUM],mdAsymWUnRegRunOut[NUM],mdAsymWNormUnRegRunOut[NUM],mdAsymWDiffRunOut[NUM],mdAsymWNormDiffRunOut[NUM];
  Double_t mdAsymRegRunOut[NUM],emdAsymRegRunOut[NUM],mdAsymWRegRunOut[NUM],mdAsymWNormRegRunOut[NUM];
  Double_t diffXUnRegRunOut[NUM],diffXSlopeUnRegRunOut[NUM],diffYUnRegRunOut[NUM],diffYSlopeUnRegRunOut[NUM],diffEUnRegRunOut[NUM];
  Double_t ediffXUnRegRunOut[NUM],ediffXSlopeUnRegRunOut[NUM],ediffYUnRegRunOut[NUM],ediffYSlopeUnRegRunOut[NUM],ediffEUnRegRunOut[NUM];
  Double_t diffXRegRunOut[NUM],diffXSlopeRegRunOut[NUM],diffYRegRunOut[NUM],diffYSlopeRegRunOut[NUM],diffERegRunOut[NUM];
  Double_t ediffXRegRunOut[NUM],ediffXSlopeRegRunOut[NUM],ediffYRegRunOut[NUM],ediffYSlopeRegRunOut[NUM],ediffERegRunOut[NUM];
  Double_t senXRunOut[NUM],senXSlopeRunOut[NUM],senYRunOut[NUM],senYSlopeRunOut[NUM],senERunOut[NUM];
  Double_t esenXRunOut[NUM],esenXSlopeRunOut[NUM],esenYRunOut[NUM],esenYSlopeRunOut[NUM],esenERunOut[NUM];
  Double_t chargeRunOut[NUM],echargeRunOut[NUM],bcm1YRunOut[NUM],ebcm1YRunOut[NUM];

  Double_t mdAsymUnRegRunInVal[NUM],emdAsymUnRegRunInVal[NUM],mdAsymWUnRegRunInVal[NUM];
  Double_t mdAsymRegRunInVal[NUM],emdAsymRegRunInVal[NUM],mdAsymWRegRunInVal[NUM];
  Double_t diffXUnRegRunInVal[NUM],diffXSlopeUnRegRunInVal[NUM],diffYUnRegRunInVal[NUM],diffYSlopeUnRegRunInVal[NUM],diffEUnRegRunInVal[NUM];
  Double_t ediffXUnRegRunInVal[NUM],ediffXSlopeUnRegRunInVal[NUM],ediffYUnRegRunInVal[NUM],ediffYSlopeUnRegRunInVal[NUM],ediffEUnRegRunInVal[NUM];
  Double_t diffXRegRunInVal[NUM],diffXSlopeRegRunInVal[NUM],diffYRegRunInVal[NUM],diffYSlopeRegRunInVal[NUM],diffERegRunInVal[NUM];
  Double_t ediffXRegRunInVal[NUM],ediffXSlopeRegRunInVal[NUM],ediffYRegRunInVal[NUM],ediffYSlopeRegRunInVal[NUM],ediffERegRunInVal[NUM];
  Double_t senXRunInVal[NUM],senXSlopeRunInVal[NUM],senYRunInVal[NUM],senYSlopeRunInVal[NUM],senERunInVal[NUM];
  Double_t esenXRunInVal[NUM],esenXSlopeRunInVal[NUM],esenYRunInVal[NUM],esenYSlopeRunInVal[NUM],esenERunInVal[NUM];
  Double_t chargeRunInVal[NUM],echargeRunInVal[NUM],bcm1YRunInVal[NUM],ebcm1YRunInVal[NUM];

  Double_t mdAsymUnRegRunOutVal[NUM],emdAsymUnRegRunOutVal[NUM],mdAsymWUnRegRunOutVal[NUM];
  Double_t mdAsymRegRunOutVal[NUM],emdAsymRegRunOutVal[NUM],mdAsymWRegRunOutVal[NUM];
  Double_t diffXUnRegRunOutVal[NUM],diffXSlopeUnRegRunOutVal[NUM],diffYUnRegRunOutVal[NUM],diffYSlopeUnRegRunOutVal[NUM],diffEUnRegRunOutVal[NUM];
  Double_t ediffXUnRegRunOutVal[NUM],ediffXSlopeUnRegRunOutVal[NUM],ediffYUnRegRunOutVal[NUM],ediffYSlopeUnRegRunOutVal[NUM],ediffEUnRegRunOutVal[NUM];
  Double_t diffXRegRunOutVal[NUM],diffXSlopeRegRunOutVal[NUM],diffYRegRunOutVal[NUM],diffYSlopeRegRunOutVal[NUM],diffERegRunOutVal[NUM];
  Double_t ediffXRegRunOutVal[NUM],ediffXSlopeRegRunOutVal[NUM],ediffYRegRunOutVal[NUM],ediffYSlopeRegRunOutVal[NUM],ediffERegRunOutVal[NUM];
  Double_t senXRunOutVal[NUM],senXSlopeRunOutVal[NUM],senYRunOutVal[NUM],senYSlopeRunOutVal[NUM],senERunOutVal[NUM];
  Double_t esenXRunOutVal[NUM],esenXSlopeRunOutVal[NUM],esenYRunOutVal[NUM],esenYSlopeRunOutVal[NUM],esenERunOutVal[NUM];
  Double_t chargeRunOutVal[NUM],echargeRunOutVal[NUM],bcm1YRunOutVal[NUM],ebcm1YRunOutVal[NUM];

  Double_t corXRunIn[NUM],corXSlopeRunIn[NUM],corYRunIn[NUM],corYSlopeRunIn[NUM],corERunIn[NUM],corChargeRunIn[NUM];
  Double_t ecorXRunIn[NUM],ecorXSlopeRunIn[NUM],ecorYRunIn[NUM],ecorYSlopeRunIn[NUM],ecorERunIn[NUM],ecorChargeRunIn[NUM];
  Double_t corXRunOut[NUM],corXSlopeRunOut[NUM],corYRunOut[NUM],corYSlopeRunOut[NUM],corERunOut[NUM],corChargeRunOut[NUM];
  Double_t ecorXRunOut[NUM],ecorXSlopeRunOut[NUM],ecorYRunOut[NUM],ecorYSlopeRunOut[NUM],ecorERunOut[NUM],ecorChargeRunOut[NUM];

  Double_t corTotalRunIn[NUM],ecorTotalRunIn[NUM],corTotalRunOut[NUM],ecorTotalRunOut[NUM];
  Double_t corTotalSlugIn[NUM],ecorTotalSlugIn[NUM],corTotalSlugOut[NUM],ecorTotalSlugOut[NUM];
  Double_t corTotalSlugAvgIn[NUM],ecorTotalSlugAvgIn[NUM],corTotalSlugAvgOut[NUM],ecorTotalSlugAvgOut[NUM];

  Double_t corXSlugIn[NUM],corXSlopeSlugIn[NUM],corYSlugIn[NUM],corYSlopeSlugIn[NUM],corESlugIn[NUM],corChargeSlugIn[NUM];
  Double_t ecorXSlugIn[NUM],ecorXSlopeSlugIn[NUM],ecorYSlugIn[NUM],ecorYSlopeSlugIn[NUM],ecorESlugIn[NUM],ecorChargeSlugIn[NUM];
  Double_t corXSlugOut[NUM],corXSlopeSlugOut[NUM],corYSlugOut[NUM],corYSlopeSlugOut[NUM],corESlugOut[NUM],corChargeSlugOut[NUM];
  Double_t ecorXSlugOut[NUM],ecorXSlopeSlugOut[NUM],ecorYSlugOut[NUM],ecorYSlopeSlugOut[NUM],ecorESlugOut[NUM],ecorChargeSlugOut[NUM];

  Double_t corXSlugAvgIn[NUM],corXSlopeSlugAvgIn[NUM],corYSlugAvgIn[NUM],corYSlopeSlugAvgIn[NUM],corESlugAvgIn[NUM],corChargeSlugAvgIn[NUM];
  Double_t ecorXSlugAvgIn[NUM],ecorXSlopeSlugAvgIn[NUM],ecorYSlugAvgIn[NUM],ecorYSlopeSlugAvgIn[NUM],ecorESlugAvgIn[NUM],ecorChargeSlugAvgIn[NUM];
  Double_t corXSlugAvgOut[NUM],corXSlopeSlugAvgOut[NUM],corYSlugAvgOut[NUM],corYSlopeSlugAvgOut[NUM],corESlugAvgOut[NUM],corChargeSlugAvgOut[NUM];
  Double_t ecorXSlugAvgOut[NUM],ecorXSlopeSlugAvgOut[NUM],ecorYSlugAvgOut[NUM],ecorYSlopeSlugAvgOut[NUM],ecorESlugAvgOut[NUM],ecorChargeSlugAvgOut[NUM];

  Double_t mdAsymCorRunIn[NUM],emdAsymCorRunIn[NUM];
  Double_t mdAsymCorRunSlugIn[NUM],emdAsymCorRunSlugIn[NUM];
  Double_t mdAsymCorRunSlugAvgIn[NUM],emdAsymCorRunSlugAvgIn[NUM];
  Double_t mdAsymCorRunOut[NUM],emdAsymCorRunOut[NUM];
  Double_t mdAsymCorRunSlugOut[NUM],emdAsymCorRunSlugOut[NUM];
  Double_t mdAsymCorRunSlugAvgOut[NUM],emdAsymCorRunSlugAvgOut[NUM];


  Int_t counterRunIn=0,counterRunOut=0;
  ifstream inMDRun,outMDRun;
  inMDRun.open((Form("%s/dbText/%sRun%sIn.txt",n2deltaAnalysisDir.Data(),savePlot,regScheme)));
  outMDRun.open((Form("%s/dbText/%sRun%sOut.txt",n2deltaAnalysisDir.Data(),savePlot,regScheme)));

  while(1) {
    zeroRunIn[counterRunIn]=0;

    inMDRun >> runNumberRunIn[counterRunIn];
    inMDRun >> mdAsymRegRunIn[counterRunIn];
    inMDRun >> emdAsymRegRunIn[counterRunIn];
    inMDRun >> mdAsymWRegRunIn[counterRunIn];
    inMDRun >> mdAsymUnRegRunIn[counterRunIn];
    inMDRun >> emdAsymUnRegRunIn[counterRunIn];
    inMDRun >> mdAsymWUnRegRunIn[counterRunIn];
    inMDRun >> diffXUnRegRunIn[counterRunIn];
    inMDRun >> ediffXUnRegRunIn[counterRunIn];
    inMDRun >> diffXSlopeUnRegRunIn[counterRunIn];
    inMDRun >> ediffXSlopeUnRegRunIn[counterRunIn];
    inMDRun >> diffYUnRegRunIn[counterRunIn];
    inMDRun >> ediffYUnRegRunIn[counterRunIn];
    inMDRun >> diffYSlopeUnRegRunIn[counterRunIn];
    inMDRun >> ediffYSlopeUnRegRunIn[counterRunIn];
    inMDRun >> diffEUnRegRunIn[counterRunIn];
    inMDRun >> ediffEUnRegRunIn[counterRunIn];
    inMDRun >> diffXRegRunIn[counterRunIn];
    inMDRun >> ediffXRegRunIn[counterRunIn];
    inMDRun >> diffXSlopeRegRunIn[counterRunIn];
    inMDRun >> ediffXSlopeRegRunIn[counterRunIn];
    inMDRun >> diffYRegRunIn[counterRunIn];
    inMDRun >> ediffYRegRunIn[counterRunIn];
    inMDRun >> diffYSlopeRegRunIn[counterRunIn];
    inMDRun >> ediffYSlopeRegRunIn[counterRunIn];
    inMDRun >> diffERegRunIn[counterRunIn];
    inMDRun >> ediffERegRunIn[counterRunIn];
    inMDRun >> senXRunIn[counterRunIn];
    inMDRun >> esenXRunIn[counterRunIn];
    inMDRun >> senXSlopeRunIn[counterRunIn];
    inMDRun >> esenXSlopeRunIn[counterRunIn];
    inMDRun >> senYRunIn[counterRunIn];
    inMDRun >> esenYRunIn[counterRunIn];
    inMDRun >> senYSlopeRunIn[counterRunIn];
    inMDRun >> esenYSlopeRunIn[counterRunIn];
    inMDRun >> senERunIn[counterRunIn];
    inMDRun >> esenERunIn[counterRunIn];
    inMDRun >> chargeRunIn[counterRunIn];
    inMDRun >> echargeRunIn[counterRunIn];
    inMDRun >> bcm1YRunIn[counterRunIn];
    inMDRun >> ebcm1YRunIn[counterRunIn];
 
    if (!inMDRun.good()) break;

    mdAsymUnRegRunInVal[counterRunIn]      =  mdAsymUnRegRunIn[counterRunIn]*1e6;
    emdAsymUnRegRunInVal[counterRunIn]     =  emdAsymUnRegRunIn[counterRunIn]*1e6;
    mdAsymWUnRegRunInVal[counterRunIn]     =  mdAsymWUnRegRunIn[counterRunIn]*1e6;
    mdAsymRegRunInVal[counterRunIn]        =  mdAsymRegRunIn[counterRunIn]*1e6;
    emdAsymRegRunInVal[counterRunIn]       =  emdAsymRegRunIn[counterRunIn]*1e6;
    mdAsymWRegRunInVal[counterRunIn]       =  mdAsymWRegRunIn[counterRunIn]*1e6;

    diffXUnRegRunInVal[counterRunIn]       =  diffXUnRegRunIn[counterRunIn];
    ediffXUnRegRunInVal[counterRunIn]      =  ediffXUnRegRunIn[counterRunIn];
    diffXSlopeUnRegRunInVal[counterRunIn]  =  diffXSlopeUnRegRunIn[counterRunIn]*1e6;
    ediffXSlopeUnRegRunInVal[counterRunIn] =  ediffXSlopeUnRegRunIn[counterRunIn]*1e6;
    diffYUnRegRunInVal[counterRunIn]       =  diffYUnRegRunIn[counterRunIn];
    ediffYUnRegRunInVal[counterRunIn]      =  ediffYUnRegRunIn[counterRunIn];
    diffYSlopeUnRegRunInVal[counterRunIn]  =  diffYSlopeUnRegRunIn[counterRunIn]*1e6;
    ediffYSlopeUnRegRunInVal[counterRunIn] =  ediffYSlopeUnRegRunIn[counterRunIn]*1e6;
    diffEUnRegRunInVal[counterRunIn]       =  diffEUnRegRunIn[counterRunIn];
    ediffEUnRegRunInVal[counterRunIn]      =  ediffEUnRegRunIn[counterRunIn];

    diffXRegRunInVal[counterRunIn]         =  diffXRegRunIn[counterRunIn];
    ediffXRegRunInVal[counterRunIn]        =  ediffXRegRunIn[counterRunIn];
    diffXSlopeRegRunInVal[counterRunIn]    =  diffXSlopeRegRunIn[counterRunIn]*1e6;
    ediffXSlopeRegRunInVal[counterRunIn]   =  ediffXSlopeRegRunIn[counterRunIn]*1e6;
    diffYRegRunInVal[counterRunIn]         =  diffYRegRunIn[counterRunIn];
    ediffYRegRunInVal[counterRunIn]        =  ediffYRegRunIn[counterRunIn];
    diffYSlopeRegRunInVal[counterRunIn]    =  diffYSlopeRegRunIn[counterRunIn]*1e6;
    ediffYSlopeRegRunInVal[counterRunIn]   =  ediffYSlopeRegRunIn[counterRunIn]*1e6;
    diffERegRunInVal[counterRunIn]         =  diffERegRunIn[counterRunIn];
    ediffERegRunInVal[counterRunIn]        =  ediffERegRunIn[counterRunIn];

    senXRunInVal[counterRunIn]             =  senXRunIn[counterRunIn]*1e6;
    esenXRunInVal[counterRunIn]            =  esenXRunIn[counterRunIn]*1e6;
    senXSlopeRunInVal[counterRunIn]        =  senXSlopeRunIn[counterRunIn];
    esenXSlopeRunInVal[counterRunIn]       =  esenXSlopeRunIn[counterRunIn];
    senYRunInVal[counterRunIn]             =  senYRunIn[counterRunIn]*1e6;
    esenYRunInVal[counterRunIn]            =  esenYRunIn[counterRunIn]*1e6;
    senYSlopeRunInVal[counterRunIn]        =  senYSlopeRunIn[counterRunIn];
    esenYSlopeRunInVal[counterRunIn]       =  esenYSlopeRunIn[counterRunIn];
    senERunInVal[counterRunIn]             =  senERunIn[counterRunIn];
    esenERunInVal[counterRunIn]            =  esenERunIn[counterRunIn] ;

    chargeRunInVal[counterRunIn]           =  chargeRunIn[counterRunIn]*1e6;
    echargeRunInVal[counterRunIn]          =  echargeRunIn[counterRunIn]*1e6;
    bcm1YRunInVal[counterRunIn]            =  bcm1YRunIn[counterRunIn];
    ebcm1YRunInVal[counterRunIn]           =  ebcm1YRunIn[counterRunIn];

    mdAsymWNormUnRegRunIn[counterRunIn]    =  mdAsymWUnRegRunInVal[counterRunIn]/TMath::Sqrt(bcm1YRunIn[counterRunIn]/160);
    mdAsymWNormRegRunIn[counterRunIn]      =  mdAsymWRegRunInVal[counterRunIn]/TMath::Sqrt(bcm1YRunIn[counterRunIn]/160);
    mdAsymWDiffRunIn[counterRunIn]         =  mdAsymWUnRegRunInVal[counterRunIn]-mdAsymWRegRunInVal[counterRunIn];
    mdAsymWNormDiffRunIn[counterRunIn]     =  mdAsymWNormUnRegRunIn[counterRunIn]-mdAsymWNormRegRunIn[counterRunIn];

    corXRunIn[counterRunIn]                =  diffXUnRegRunInVal[counterRunIn]*senXRunInVal[counterRunIn];
    corXSlopeRunIn[counterRunIn]           =  diffXSlopeUnRegRunInVal[counterRunIn]*senXSlopeRunInVal[counterRunIn];
    corYRunIn[counterRunIn]                =  diffYUnRegRunInVal[counterRunIn]*senYRunInVal[counterRunIn];
    corYSlopeRunIn[counterRunIn]           =  diffYSlopeUnRegRunInVal[counterRunIn]*senYSlopeRunInVal[counterRunIn];
    corERunIn[counterRunIn]                =  diffEUnRegRunInVal[counterRunIn]*senERunInVal[counterRunIn]*1e6;
    corChargeRunIn[counterRunIn]           =  chargeRunInVal[counterRunIn];
    corTotalRunIn[counterRunIn]            =  corXRunIn[counterRunIn]+corXSlopeRunIn[counterRunIn]+corYRunIn[counterRunIn]+corYSlopeRunIn[counterRunIn]+corERunIn[counterRunIn]+corChargeRunIn[counterRunIn];

    ecorXRunIn[counterRunIn]               =  TMath::Sqrt(pow(diffXUnRegRunInVal[counterRunIn]*esenXRunInVal[counterRunIn],2)+pow(ediffXUnRegRunInVal[counterRunIn]*senXRunInVal[counterRunIn],2));
    ecorXSlopeRunIn[counterRunIn]          =  TMath::Sqrt(pow(diffXSlopeUnRegRunInVal[counterRunIn]*esenXSlopeRunInVal[counterRunIn],2)+pow(ediffXSlopeUnRegRunInVal[counterRunIn]*senXSlopeRunInVal[counterRunIn],2));
    ecorYRunIn[counterRunIn]               =  TMath::Sqrt(pow(diffYUnRegRunInVal[counterRunIn]*esenYRunInVal[counterRunIn],2)+pow(ediffYUnRegRunInVal[counterRunIn]*senYRunInVal[counterRunIn],2));
    ecorYSlopeRunIn[counterRunIn]          =  TMath::Sqrt(pow(diffYSlopeUnRegRunInVal[counterRunIn]*esenYSlopeRunInVal[counterRunIn],2)+pow(ediffYSlopeUnRegRunInVal[counterRunIn]*senYSlopeRunInVal[counterRunIn],2));
    ecorERunIn[counterRunIn]               =  TMath::Sqrt(pow(diffEUnRegRunInVal[counterRunIn]*esenERunInVal[counterRunIn],2)+pow(ediffEUnRegRunInVal[counterRunIn]*senERunInVal[counterRunIn],2));
    ecorChargeRunIn[counterRunIn]          =  echargeRunInVal[counterRunIn];
    ecorTotalRunIn[counterRunIn]           = TMath::Sqrt(pow(ecorXRunIn[counterRunIn],2)+pow(ecorXSlopeRunIn[counterRunIn],2)+pow(ecorYRunIn[counterRunIn],2)+pow(ecorYSlopeRunIn[counterRunIn],2)+pow(ecorERunIn[counterRunIn],2)+pow(ecorChargeRunIn[counterRunIn],2));

    corXSlugIn[counterRunIn]               =  diffXUnRegRunInVal[counterRunIn]*avgSenXIn[counterAvg-1];
    corXSlopeSlugIn[counterRunIn]          =  diffXSlopeUnRegRunInVal[counterRunIn]*avgSenXSlopeIn[counterAvg-1];
    corYSlugIn[counterRunIn]               =  diffYUnRegRunInVal[counterRunIn]*avgSenYIn[counterAvg-1];
    corYSlopeSlugIn[counterRunIn]          =  diffYSlopeUnRegRunInVal[counterRunIn]*avgSenYSlopeIn[counterAvg-1];
    corESlugIn[counterRunIn]               =  diffEUnRegRunInVal[counterRunIn]*avgSenEIn[counterAvg-1]*1e6;
    corChargeSlugIn[counterRunIn]          =  0;
    corTotalSlugIn[counterRunIn]           =  corXSlugIn[counterRunIn]+corXSlopeSlugIn[counterRunIn]+corYSlugIn[counterRunIn]+corYSlopeSlugIn[counterRunIn]+corESlugIn[counterRunIn]+corChargeSlugIn[counterRunIn];

    ecorXSlugIn[counterRunIn]              =  TMath::Sqrt(pow(diffXUnRegRunInVal[counterRunIn]*eavgSenXIn[counterAvg-1],2)+pow(ediffXUnRegRunInVal[counterRunIn]*avgSenXIn[counterAvg-1],2));
    ecorXSlopeSlugIn[counterRunIn]         =  TMath::Sqrt(pow(diffXSlopeUnRegRunInVal[counterRunIn]*eavgSenXSlopeIn[counterAvg-1],2)+pow(ediffXSlopeUnRegRunInVal[counterRunIn]*avgSenXSlopeIn[counterAvg-1],2));
    ecorYSlugIn[counterRunIn]              =  TMath::Sqrt(pow(diffYUnRegRunInVal[counterRunIn]*eavgSenYIn[counterAvg-1],2)+pow(ediffYUnRegRunInVal[counterRunIn]*avgSenYIn[counterAvg-1],2));
    ecorYSlopeSlugIn[counterRunIn]         =  TMath::Sqrt(pow(diffYSlopeUnRegRunInVal[counterRunIn]*eavgSenYSlopeIn[counterAvg-1],2)+pow(ediffYSlopeUnRegRunInVal[counterRunIn]*avgSenYSlopeIn[counterAvg-1],2));
    ecorESlugIn[counterRunIn]              =  TMath::Sqrt(pow(diffEUnRegRunInVal[counterRunIn]*eavgSenEIn[counterAvg-1],2)+pow(ediffEUnRegRunInVal[counterRunIn]*avgSenEIn[counterAvg-1],2));
    ecorChargeSlugIn[counterRunIn]          =  0;
    ecorTotalSlugIn[counterRunIn]          = TMath::Sqrt(pow(ecorXSlugIn[counterRunIn],2)+pow(ecorXSlopeSlugIn[counterRunIn],2)+pow(ecorYSlugIn[counterRunIn],2)+pow(ecorYSlopeSlugIn[counterRunIn],2)+pow(ecorESlugIn[counterRunIn],2));

    corXSlugAvgIn[counterRunIn]               =  (0.000507)*(4042);
    corXSlopeSlugAvgIn[counterRunIn]          =  (0.01682)*(-38.18);
    corYSlugAvgIn[counterRunIn]               =  (0.004926)*(-1493);
    corYSlopeSlugAvgIn[counterRunIn]          =  (0.01811)*(11.63);
    corESlugAvgIn[counterRunIn]               =  (-8.065e-5)*1e6*(-0.0009098);
    corTotalSlugAvgIn[counterRunIn]           =  corXSlugAvgIn[counterRunIn]+corXSlopeSlugAvgIn[counterRunIn]+corYSlugAvgIn[counterRunIn]+corYSlopeSlugAvgIn[counterRunIn]+corESlugAvgIn[counterRunIn];

    ecorXSlugAvgIn[counterRunIn]              =  TMath::Sqrt((0.000507)*(89.44)*(0.000507)*(89.44)+(7.517e-6)*(4042)*(7.517e-6)*(4042));
    ecorXSlopeSlugAvgIn[counterRunIn]         =  TMath::Sqrt((0.01682)*(2.858)*(0.01682)*(2.858)+(0.000216)*(-38.18)*(0.000216)*(-38.18));
    ecorYSlugAvgIn[counterRunIn]              =  TMath::Sqrt((0.004926)*(83.3)*(0.004926)*(83.3)+(5.03e-6)*(-1493)*(5.03e-6)*(-1493));
    ecorYSlopeSlugAvgIn[counterRunIn]         =  TMath::Sqrt((0.01811)*(2.36)*(0.01811)*(2.36)+(0.0001911)*(11.63)*(0.0001911)*(11.63));
    ecorESlugAvgIn[counterRunIn]              =  TMath::Sqrt((-8.065e-5)*(0.0001618)*(-8.065e-5)*(0.0001618)*1e6+(1.517e-6)*(-0.0009098)*(1.517e-6)*(-0.0009098)*1e6);
    ecorTotalSlugAvgIn[counterRunIn]          =  TMath::Sqrt(ecorXSlugAvgIn[counterRunIn]*ecorXSlugAvgIn[counterRunIn]+ecorXSlopeSlugAvgIn[counterRunIn]*ecorXSlopeSlugAvgIn[counterRunIn]+ecorYSlugAvgIn[counterRunIn]*ecorYSlugAvgIn[counterRunIn]+ecorYSlopeSlugAvgIn[counterRunIn]*ecorYSlopeSlugAvgIn[counterRunIn]+ecorESlugAvgIn[counterRunIn]*ecorESlugAvgIn[counterRunIn]);

    mdAsymCorRunIn[counterRunIn]              = mdAsymUnRegRunInVal[counterRunIn]-corTotalRunIn[counterRunIn];
    mdAsymCorRunSlugIn[counterRunIn]          = mdAsymUnRegRunInVal[counterRunIn]-corTotalSlugIn[counterRunIn];
    mdAsymCorRunSlugAvgIn[counterRunIn]       = mdAsymUnRegRunInVal[counterRunIn]-avgCorTotalIn[counterAvg-1];

    emdAsymCorRunIn[counterRunIn]             = emdAsymRegRunInVal[counterRunIn];
    emdAsymCorRunSlugIn[counterRunIn]         = emdAsymRegRunInVal[counterRunIn];
    emdAsymCorRunSlugAvgIn[counterRunIn]      = emdAsymRegRunInVal[counterRunIn];


    counterRunIn++;
  }
  inMDRun.close();

  /*********************************************************************************/

  while(1) {
    zeroRunOut[counterRunOut]=0;

    outMDRun >> runNumberRunOut[counterRunOut];
    outMDRun >> mdAsymRegRunOut[counterRunOut];
    outMDRun >> emdAsymRegRunOut[counterRunOut];
    outMDRun >> mdAsymWRegRunOut[counterRunOut];
    outMDRun >> mdAsymUnRegRunOut[counterRunOut];
    outMDRun >> emdAsymUnRegRunOut[counterRunOut];
    outMDRun >> mdAsymWUnRegRunOut[counterRunOut];
    outMDRun >> diffXUnRegRunOut[counterRunOut];
    outMDRun >> ediffXUnRegRunOut[counterRunOut];
    outMDRun >> diffXSlopeUnRegRunOut[counterRunOut];
    outMDRun >> ediffXSlopeUnRegRunOut[counterRunOut];
    outMDRun >> diffYUnRegRunOut[counterRunOut];
    outMDRun >> ediffYUnRegRunOut[counterRunOut];
    outMDRun >> diffYSlopeUnRegRunOut[counterRunOut];
    outMDRun >> ediffYSlopeUnRegRunOut[counterRunOut];
    outMDRun >> diffEUnRegRunOut[counterRunOut];
    outMDRun >> ediffEUnRegRunOut[counterRunOut];
    outMDRun >> diffXRegRunOut[counterRunOut];
    outMDRun >> ediffXRegRunOut[counterRunOut];
    outMDRun >> diffXSlopeRegRunOut[counterRunOut];
    outMDRun >> ediffXSlopeRegRunOut[counterRunOut];
    outMDRun >> diffYRegRunOut[counterRunOut];
    outMDRun >> ediffYRegRunOut[counterRunOut];
    outMDRun >> diffYSlopeRegRunOut[counterRunOut];
    outMDRun >> ediffYSlopeRegRunOut[counterRunOut];
    outMDRun >> diffERegRunOut[counterRunOut];
    outMDRun >> ediffERegRunOut[counterRunOut];
    outMDRun >> senXRunOut[counterRunOut];
    outMDRun >> esenXRunOut[counterRunOut];
    outMDRun >> senXSlopeRunOut[counterRunOut];
    outMDRun >> esenXSlopeRunOut[counterRunOut];
    outMDRun >> senYRunOut[counterRunOut];
    outMDRun >> esenYRunOut[counterRunOut];
    outMDRun >> senYSlopeRunOut[counterRunOut];
    outMDRun >> esenYSlopeRunOut[counterRunOut];
    outMDRun >> senERunOut[counterRunOut];
    outMDRun >> esenERunOut[counterRunOut];
    outMDRun >> chargeRunOut[counterRunOut];
    outMDRun >> echargeRunOut[counterRunOut];
    outMDRun >> bcm1YRunOut[counterRunOut];
    outMDRun >> ebcm1YRunOut[counterRunOut];
 
    if (!outMDRun.good()) break;

    mdAsymUnRegRunOutVal[counterRunOut]      =  mdAsymUnRegRunOut[counterRunOut]*1e6;
    emdAsymUnRegRunOutVal[counterRunOut]     =  emdAsymUnRegRunOut[counterRunOut]*1e6;
    mdAsymWUnRegRunOutVal[counterRunOut]     =  mdAsymWUnRegRunOut[counterRunOut]*1e6;
    mdAsymRegRunOutVal[counterRunOut]        =  mdAsymRegRunOut[counterRunOut]*1e6;
    emdAsymRegRunOutVal[counterRunOut]       =  emdAsymRegRunOut[counterRunOut]*1e6;
    mdAsymWRegRunOutVal[counterRunOut]       =  mdAsymWRegRunOut[counterRunOut]*1e6;

    diffXUnRegRunOutVal[counterRunOut]       =  diffXUnRegRunOut[counterRunOut];
    ediffXUnRegRunOutVal[counterRunOut]      =  ediffXUnRegRunOut[counterRunOut];
    diffXSlopeUnRegRunOutVal[counterRunOut]  =  diffXSlopeUnRegRunOut[counterRunOut]*1e6;
    ediffXSlopeUnRegRunOutVal[counterRunOut] =  ediffXSlopeUnRegRunOut[counterRunOut]*1e6;
    diffYUnRegRunOutVal[counterRunOut]       =  diffYUnRegRunOut[counterRunOut];
    ediffYUnRegRunOutVal[counterRunOut]      =  ediffYUnRegRunOut[counterRunOut];
    diffYSlopeUnRegRunOutVal[counterRunOut]  =  diffYSlopeUnRegRunOut[counterRunOut]*1e6;
    ediffYSlopeUnRegRunOutVal[counterRunOut] =  ediffYSlopeUnRegRunOut[counterRunOut]*1e6;
    diffEUnRegRunOutVal[counterRunOut]       =  diffEUnRegRunOut[counterRunOut];
    ediffEUnRegRunOutVal[counterRunOut]      =  ediffEUnRegRunOut[counterRunOut];

    diffXRegRunOutVal[counterRunOut]         =  diffXRegRunOut[counterRunOut];
    ediffXRegRunOutVal[counterRunOut]        =  ediffXRegRunOut[counterRunOut];
    diffXSlopeRegRunOutVal[counterRunOut]    =  diffXSlopeRegRunOut[counterRunOut]*1e6;
    ediffXSlopeRegRunOutVal[counterRunOut]   =  ediffXSlopeRegRunOut[counterRunOut]*1e6;
    diffYRegRunOutVal[counterRunOut]         =  diffYRegRunOut[counterRunOut];
    ediffYRegRunOutVal[counterRunOut]        =  ediffYRegRunOut[counterRunOut];
    diffYSlopeRegRunOutVal[counterRunOut]    =  diffYSlopeRegRunOut[counterRunOut]*1e6;
    ediffYSlopeRegRunOutVal[counterRunOut]   =  ediffYSlopeRegRunOut[counterRunOut]*1e6;
    diffERegRunOutVal[counterRunOut]         =  diffERegRunOut[counterRunOut];
    ediffERegRunOutVal[counterRunOut]        =  ediffERegRunOut[counterRunOut];

    senXRunOutVal[counterRunOut]             =  senXRunOut[counterRunOut]*1e6;
    esenXRunOutVal[counterRunOut]            =  esenXRunOut[counterRunOut]*1e6;
    senXSlopeRunOutVal[counterRunOut]        =  senXSlopeRunOut[counterRunOut];
    esenXSlopeRunOutVal[counterRunOut]       =  esenXSlopeRunOut[counterRunOut];
    senYRunOutVal[counterRunOut]             =  senYRunOut[counterRunOut]*1e6;
    esenYRunOutVal[counterRunOut]            =  esenYRunOut[counterRunOut]*1e6;
    senYSlopeRunOutVal[counterRunOut]        =  senYSlopeRunOut[counterRunOut];
    esenYSlopeRunOutVal[counterRunOut]       =  esenYSlopeRunOut[counterRunOut];
    senERunOutVal[counterRunOut]             =  senERunOut[counterRunOut];
    esenERunOutVal[counterRunOut]            =  esenERunOut[counterRunOut] ;

    chargeRunOutVal[counterRunOut]           =  chargeRunOut[counterRunOut]*1e6;
    echargeRunOutVal[counterRunOut]          =  echargeRunOut[counterRunOut]*1e6;
    bcm1YRunOutVal[counterRunOut]            =  bcm1YRunOut[counterRunOut];
    ebcm1YRunOutVal[counterRunOut]           =  ebcm1YRunOut[counterRunOut];

    mdAsymWNormUnRegRunOut[counterRunOut]    =  mdAsymWUnRegRunOutVal[counterRunOut]/TMath::Sqrt(bcm1YRunOut[counterRunOut]/160);
    mdAsymWNormRegRunOut[counterRunOut]      =  mdAsymWRegRunOutVal[counterRunOut]/TMath::Sqrt(bcm1YRunOut[counterRunOut]/160);
    mdAsymWDiffRunOut[counterRunOut]         =  mdAsymWUnRegRunOutVal[counterRunOut]-mdAsymWRegRunOutVal[counterRunOut];
    mdAsymWNormDiffRunOut[counterRunOut]     =  mdAsymWNormUnRegRunOut[counterRunOut]-mdAsymWNormRegRunOut[counterRunOut];

    corXRunOut[counterRunOut]                =  diffXUnRegRunOutVal[counterRunOut]*senXRunOutVal[counterRunOut];
    corXSlopeRunOut[counterRunOut]           =  diffXSlopeUnRegRunOutVal[counterRunOut]*senXSlopeRunOutVal[counterRunOut];
    corYRunOut[counterRunOut]                =  diffYUnRegRunOutVal[counterRunOut]*senYRunOutVal[counterRunOut];
    corYSlopeRunOut[counterRunOut]           =  diffYSlopeUnRegRunOutVal[counterRunOut]*senYSlopeRunOutVal[counterRunOut];
    corERunOut[counterRunOut]                =  diffEUnRegRunOutVal[counterRunOut]*senERunOutVal[counterRunOut]*1e6;
    corChargeRunOut[counterRunOut]           =  chargeRunOutVal[counterRunOut];
    corTotalRunOut[counterRunOut]            =  corXRunOut[counterRunOut]+corXSlopeRunOut[counterRunOut]+corYRunOut[counterRunOut]+corYSlopeRunOut[counterRunOut]+corERunOut[counterRunOut]+corChargeRunOut[counterRunOut];

    ecorXRunOut[counterRunOut]               =  TMath::Sqrt(pow(diffXUnRegRunOutVal[counterRunOut]*esenXRunOutVal[counterRunOut],2)+pow(ediffXUnRegRunOutVal[counterRunOut]*senXRunOutVal[counterRunOut],2));
    ecorXSlopeRunOut[counterRunOut]          =  TMath::Sqrt(pow(diffXSlopeUnRegRunOutVal[counterRunOut]*esenXSlopeRunOutVal[counterRunOut],2)+pow(ediffXSlopeUnRegRunOutVal[counterRunOut]*senXSlopeRunOutVal[counterRunOut],2));
    ecorYRunOut[counterRunOut]               =  TMath::Sqrt(pow(diffYUnRegRunOutVal[counterRunOut]*esenYRunOutVal[counterRunOut],2)+pow(ediffYUnRegRunOutVal[counterRunOut]*senYRunOutVal[counterRunOut],2));
    ecorYSlopeRunOut[counterRunOut]          =  TMath::Sqrt(pow(diffYSlopeUnRegRunOutVal[counterRunOut]*esenYSlopeRunOutVal[counterRunOut],2)+pow(ediffYSlopeUnRegRunOutVal[counterRunOut]*senYSlopeRunOutVal[counterRunOut],2));
    ecorERunOut[counterRunOut]               =  TMath::Sqrt(pow(diffEUnRegRunOutVal[counterRunOut]*esenERunOutVal[counterRunOut],2)+pow(ediffEUnRegRunOutVal[counterRunOut]*senERunOutVal[counterRunOut],2));
    ecorChargeRunOut[counterRunOut]          =  echargeRunOutVal[counterRunOut];
    ecorTotalRunOut[counterRunOut]           = TMath::Sqrt(pow(ecorXRunOut[counterRunOut],2)+pow(ecorXSlopeRunOut[counterRunOut],2)+pow(ecorYRunOut[counterRunOut],2)+pow(ecorYSlopeRunOut[counterRunOut],2)+pow(ecorERunOut[counterRunOut],2)+pow(ecorChargeRunOut[counterRunOut],2));

    corXSlugOut[counterRunOut]               =  diffXUnRegRunOutVal[counterRunOut]*avgSenXOut[counterAvg-1];
    corXSlopeSlugOut[counterRunOut]          =  diffXSlopeUnRegRunOutVal[counterRunOut]*avgSenXSlopeOut[counterAvg-1];
    corYSlugOut[counterRunOut]               =  diffYUnRegRunOutVal[counterRunOut]*avgSenYOut[counterAvg-1];
    corYSlopeSlugOut[counterRunOut]          =  diffYSlopeUnRegRunOutVal[counterRunOut]*avgSenYSlopeOut[counterAvg-1];
    corESlugOut[counterRunOut]               =  diffEUnRegRunOutVal[counterRunOut]*avgSenEOut[counterAvg-1]*1e6;
    corChargeSlugOut[counterRunOut]          =  0;
    corTotalSlugOut[counterRunOut]           =  corXSlugOut[counterRunOut]+corXSlopeSlugOut[counterRunOut]+corYSlugOut[counterRunOut]+corYSlopeSlugOut[counterRunOut]+corESlugOut[counterRunOut]+corChargeSlugOut[counterRunOut];

    ecorXSlugOut[counterRunOut]              =  TMath::Sqrt(pow(diffXUnRegRunOutVal[counterRunOut]*eavgSenXOut[counterAvg-1],2)+pow(ediffXUnRegRunOutVal[counterRunOut]*avgSenXOut[counterAvg-1],2));
    ecorXSlopeSlugOut[counterRunOut]         =  TMath::Sqrt(pow(diffXSlopeUnRegRunOutVal[counterRunOut]*eavgSenXSlopeOut[counterAvg-1],2)+pow(ediffXSlopeUnRegRunOutVal[counterRunOut]*avgSenXSlopeOut[counterAvg-1],2));
    ecorYSlugOut[counterRunOut]              =  TMath::Sqrt(pow(diffYUnRegRunOutVal[counterRunOut]*eavgSenYOut[counterAvg-1],2)+pow(ediffYUnRegRunOutVal[counterRunOut]*avgSenYOut[counterAvg-1],2));
    ecorYSlopeSlugOut[counterRunOut]         =  TMath::Sqrt(pow(diffYSlopeUnRegRunOutVal[counterRunOut]*eavgSenYSlopeOut[counterAvg-1],2)+pow(ediffYSlopeUnRegRunOutVal[counterRunOut]*avgSenYSlopeOut[counterAvg-1],2));
    ecorESlugOut[counterRunOut]              =  TMath::Sqrt(pow(diffEUnRegRunOutVal[counterRunOut]*eavgSenEOut[counterAvg-1],2)+pow(ediffEUnRegRunOutVal[counterRunOut]*avgSenEOut[counterAvg-1],2));
    ecorChargeSlugOut[counterRunOut]         =  0;
    ecorTotalSlugOut[counterRunOut]          = TMath::Sqrt(pow(ecorXSlugOut[counterRunOut],2)+pow(ecorXSlopeSlugOut[counterRunOut],2)+pow(ecorYSlugOut[counterRunOut],2)+pow(ecorYSlopeSlugOut[counterRunOut],2)+pow(ecorESlugOut[counterRunOut],2));

    corXSlugAvgOut[counterRunOut]               =  (0.000507)*(4042);
    corXSlopeSlugAvgOut[counterRunOut]          =  (0.01682)*(-38.18);
    corYSlugAvgOut[counterRunOut]               =  (0.004926)*(-1493);
    corYSlopeSlugAvgOut[counterRunOut]          =  (0.01811)*(11.63);
    corESlugAvgOut[counterRunOut]               =  (-8.065e-5)*1e6*(-0.0009098);
    corTotalSlugAvgOut[counterRunOut]           =  corXSlugAvgOut[counterRunOut]+corXSlopeSlugAvgOut[counterRunOut]+corYSlugAvgOut[counterRunOut]+corYSlopeSlugAvgOut[counterRunOut]+corESlugAvgOut[counterRunOut];

    ecorXSlugAvgOut[counterRunOut]              =  TMath::Sqrt((0.000507)*(89.44)*(0.000507)*(89.44)+(7.517e-6)*(4042)*(7.517e-6)*(4042));
    ecorXSlopeSlugAvgOut[counterRunOut]         =  TMath::Sqrt((0.01682)*(2.858)*(0.01682)*(2.858)+(0.000216)*(-38.18)*(0.000216)*(-38.18));
    ecorYSlugAvgOut[counterRunOut]              =  TMath::Sqrt((0.004926)*(83.3)*(0.004926)*(83.3)+(5.03e-6)*(-1493)*(5.03e-6)*(-1493));
    ecorYSlopeSlugAvgOut[counterRunOut]         =  TMath::Sqrt((0.01811)*(2.36)*(0.01811)*(2.36)+(0.0001911)*(11.63)*(0.0001911)*(11.63));
    ecorESlugAvgOut[counterRunOut]              =  TMath::Sqrt((-8.065e-5)*(0.0001618)*(-8.065e-5)*(0.0001618)*1e6+(1.517e-6)*(-0.0009098)*(1.517e-6)*(-0.0009098)*1e6);
    ecorTotalSlugAvgOut[counterRunOut]          =  TMath::Sqrt(ecorXSlugAvgOut[counterRunOut]*ecorXSlugAvgOut[counterRunOut]+ecorXSlopeSlugAvgOut[counterRunOut]*ecorXSlopeSlugAvgOut[counterRunOut]+ecorYSlugAvgOut[counterRunOut]*ecorYSlugAvgOut[counterRunOut]+ecorYSlopeSlugAvgOut[counterRunOut]*ecorYSlopeSlugAvgOut[counterRunOut]+ecorESlugAvgOut[counterRunOut]*ecorESlugAvgOut[counterRunOut]);

    mdAsymCorRunOut[counterRunOut]              = mdAsymUnRegRunOutVal[counterRunOut]-corTotalRunOut[counterRunOut];
    mdAsymCorRunSlugOut[counterRunOut]          = mdAsymUnRegRunOutVal[counterRunOut]-corTotalSlugOut[counterRunOut];
    mdAsymCorRunSlugAvgOut[counterRunOut]       = mdAsymUnRegRunOutVal[counterRunOut]-avgCorTotalOut[counterAvg-1];

    emdAsymCorRunOut[counterRunOut]             = emdAsymRegRunOutVal[counterRunOut];
    emdAsymCorRunSlugOut[counterRunOut]         = emdAsymRegRunOutVal[counterRunOut];
    emdAsymCorRunSlugAvgOut[counterRunOut]      = emdAsymRegRunOutVal[counterRunOut];


    counterRunOut++;

  }
  outMDRun.close();




  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(OCTANT_VARIATION){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  if(OCTANT_VARIATION_SEN){

  TCanvas *c19 = new TCanvas("c19","c19",cx1,cy1,1200,950);
  TPad * pad190 = new TPad("pad190","pad190",ps1,ps2,ps4,ps4);
  TPad * pad191 = new TPad("pad191","pad191",ps1,ps1,ps4,ps3);
  pad190->Draw();
  pad191->Draw();
  pad190->cd();
  TText * ct190 = new TText(tll,tlr,Form("%sN to Delta %s Variation of %s Sensitivity with Octant",regTitle,dataInfo,plotTitle));
  ct190->SetTextSize(tsiz);
  ct190->Draw();
  pad191->cd();
  pad191->Divide(2,3);


  pad191->cd(1);

  TGraphErrors * runSenOctXGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,X_inAmp,zero,eX_inAmp);
  runSenOctXGraphUnRegIn1->SetName("runSenOctXGraphUnRegIn1");
  runSenOctXGraphUnRegIn1->SetMarkerColor(kRed);
  runSenOctXGraphUnRegIn1->SetLineColor(kRed);
  runSenOctXGraphUnRegIn1->SetMarkerStyle(21);
  runSenOctXGraphUnRegIn1->SetMarkerSize(0.5);
  runSenOctXGraphUnRegIn1->SetLineWidth(1);
  runSenOctXGraphUnRegIn1->Fit("fitOctantVariationXIn","E M R F 0 Q");
  fitOctantVariationXIn->SetLineStyle(1);
  fitOctantVariationXIn->SetLineWidth(2);
  fitOctantVariationXIn->SetLineColor(kRed);

  TGraphErrors * runSenOctXGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,X_outAmp,zero,eX_outAmp);
  runSenOctXGraphUnRegOut1->SetName("runSenOctXGraphUnRegOut1");
  runSenOctXGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenOctXGraphUnRegOut1->SetLineColor(kBlack);
  runSenOctXGraphUnRegOut1->SetMarkerStyle(21);
  runSenOctXGraphUnRegOut1->SetMarkerSize(0.5);
  runSenOctXGraphUnRegOut1->SetLineWidth(1);
  runSenOctXGraphUnRegOut1->Fit("fitOctantVariationXOut","E M R F 0 Q");
  fitOctantVariationXOut->SetLineStyle(1);
  fitOctantVariationXOut->SetLineWidth(2);
  fitOctantVariationXOut->SetLineColor(kBlack);

  TMultiGraph *runSenOctXGraphUnReg1 = new TMultiGraph();
  runSenOctXGraphUnReg1->Add(runSenOctXGraphUnRegIn1);
  runSenOctXGraphUnReg1->Add(runSenOctXGraphUnRegOut1);
  runSenOctXGraphUnReg1->Draw("AP");
  runSenOctXGraphUnReg1->SetTitle("");
  runSenOctXGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runSenOctXGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runSenOctXGraphUnReg1->GetYaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenOctXGraphUnReg1->GetXaxis()->CenterTitle();
  runSenOctXGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisSenOctXGraphUnReg1= runSenOctXGraphUnReg1->GetXaxis();
  xaxisSenOctXGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenOctXUnRegIn1 =(TPaveStats*)runSenOctXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenOctXUnRegOut1 =(TPaveStats*)runSenOctXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSenOctXUnRegIn1->SetTextColor(kRed);
  statsSenOctXUnRegIn1->SetFillColor(kWhite); 
  statsSenOctXUnRegOut1->SetTextColor(kBlack);
  statsSenOctXUnRegOut1->SetFillColor(kWhite);
  statsSenOctXUnRegIn1->SetTextSize(0.045);
  statsSenOctXUnRegOut1->SetTextSize(0.045);
  statsSenOctXUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsSenOctXUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsSenOctXUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsSenOctXUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsSenOctXUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsSenOctXUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsSenOctXUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsSenOctXUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenOctXUnRegInp0        =   fitOctantVariationXIn->GetParameter(0);
  Double_t calc_SenOctXUnRegInp1        =   fitOctantVariationXIn->GetParameter(1);
  Double_t calc_SenOctXUnRegInp2        =   fitOctantVariationXIn->GetParameter(2);
  Double_t calc_eSenOctXUnRegInp0       =   fitOctantVariationXIn->GetParError(0);
  Double_t calc_eSenOctXUnRegInp1       =   fitOctantVariationXIn->GetParError(1);
  Double_t calc_eSenOctXUnRegInp2       =   fitOctantVariationXIn->GetParError(2);
  Double_t calc_SenOctXUnRegInChisquare =   fitOctantVariationXIn->GetChisquare();
  Double_t calc_SenOctXUnRegInNDF       =   fitOctantVariationXIn->GetNDF();

  Double_t calc_SenOctXUnRegOutp0        =   fitOctantVariationXOut->GetParameter(0);
  Double_t calc_SenOctXUnRegOutp1        =   fitOctantVariationXOut->GetParameter(1);
  Double_t calc_SenOctXUnRegOutp2        =   fitOctantVariationXOut->GetParameter(2);
  Double_t calc_eSenOctXUnRegOutp0       =   fitOctantVariationXOut->GetParError(0);
  Double_t calc_eSenOctXUnRegOutp1       =   fitOctantVariationXOut->GetParError(1);
  Double_t calc_eSenOctXUnRegOutp2       =   fitOctantVariationXOut->GetParError(2);
  Double_t calc_SenOctXUnRegOutChisquare =   fitOctantVariationXOut->GetChisquare();
  Double_t calc_SenOctXUnRegOutNDF       =   fitOctantVariationXOut->GetNDF();

  TLegend *legRunSenOctXUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunSenOctXUnReg1->AddEntry("runSenOctXGraphUnRegIn1",  Form("In X= %4.1f#pm%4.1f",calc_SenOctXUnRegInp0,calc_eSenOctXUnRegInp0),"lp");
  legRunSenOctXUnReg1->AddEntry("runSenOctXGraphUnRegOut1",  Form("Out X= %4.1f#pm%4.1f",calc_SenOctXUnRegOutp0,calc_eSenOctXUnRegOutp0),"lp");
  legRunSenOctXUnReg1->SetTextSize(0.040);
  legRunSenOctXUnReg1->SetFillColor(0);
  legRunSenOctXUnReg1->SetBorderSize(2);
  legRunSenOctXUnReg1->Draw();

  fitOctantVariationXIn->Draw("same");
  fitOctantVariationXOut->Draw("same");

  gPad->Update();


  pad191->cd(2);

  TGraphErrors * runSenOctXSlopeGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,XSlope_inAmp,zero,eXSlope_inAmp);
  runSenOctXSlopeGraphUnRegIn1->SetName("runSenOctXSlopeGraphUnRegIn1");
  runSenOctXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenOctXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenOctXSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runSenOctXSlopeGraphUnRegIn1->SetMarkerSize(0.6);
  runSenOctXSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenOctXSlopeGraphUnRegIn1->Fit("fitOctantVariationXSlopeIn","E M R F 0 Q");
  fitOctantVariationXSlopeIn->SetLineStyle(1);
  fitOctantVariationXSlopeIn->SetLineWidth(2);
  fitOctantVariationXSlopeIn->SetLineColor(kRed);

  TGraphErrors * runSenOctXSlopeGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,XSlope_outAmp,zero,eXSlope_outAmp);
  runSenOctXSlopeGraphUnRegOut1->SetName("runSenOctXSlopeGraphUnRegOut1");
  runSenOctXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenOctXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenOctXSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runSenOctXSlopeGraphUnRegOut1->SetMarkerSize(0.6);
  runSenOctXSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenOctXSlopeGraphUnRegOut1->Fit("fitOctantVariationXSlopeOut","E M R F 0 Q");
  fitOctantVariationXSlopeOut->SetLineStyle(1);
  fitOctantVariationXSlopeOut->SetLineWidth(2);
  fitOctantVariationXSlopeOut->SetLineColor(kBlack);


  TMultiGraph *runSenOctXSlopeGraphUnReg1 = new TMultiGraph();
  runSenOctXSlopeGraphUnReg1->Add(runSenOctXSlopeGraphUnRegIn1);
  runSenOctXSlopeGraphUnReg1->Add(runSenOctXSlopeGraphUnRegOut1);
  runSenOctXSlopeGraphUnReg1->Draw("AP");
  runSenOctXSlopeGraphUnReg1->SetTitle("");
  runSenOctXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runSenOctXSlopeGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runSenOctXSlopeGraphUnReg1->GetYaxis()->SetTitle("XSlope Sensitivity [ppm/#murad]");
  runSenOctXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenOctXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisSenOctXSlopeGraphUnReg1= runSenOctXSlopeGraphUnReg1->GetXaxis();
  xaxisSenOctXSlopeGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenOctXSlopeUnRegIn1 =(TPaveStats*)runSenOctXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenOctXSlopeUnRegOut1 =(TPaveStats*)runSenOctXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSenOctXSlopeUnRegIn1->SetTextColor(kRed);
  statsSenOctXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsSenOctXSlopeUnRegOut1->SetTextColor(kBlack);
  statsSenOctXSlopeUnRegOut1->SetFillColor(kWhite);
  statsSenOctXSlopeUnRegIn1->SetTextSize(0.045);
  statsSenOctXSlopeUnRegOut1->SetTextSize(0.045);
  statsSenOctXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsSenOctXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsSenOctXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsSenOctXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsSenOctXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsSenOctXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsSenOctXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsSenOctXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenOctXSlopeUnRegInp0        =   fitOctantVariationXSlopeIn->GetParameter(0);
  Double_t calc_SenOctXSlopeUnRegInp1        =   fitOctantVariationXSlopeIn->GetParameter(1);
  Double_t calc_SenOctXSlopeUnRegInp2        =   fitOctantVariationXSlopeIn->GetParameter(2);
  Double_t calc_eSenOctXSlopeUnRegInp0       =   fitOctantVariationXSlopeIn->GetParError(0);
  Double_t calc_eSenOctXSlopeUnRegInp1       =   fitOctantVariationXSlopeIn->GetParError(1);
  Double_t calc_eSenOctXSlopeUnRegInp2       =   fitOctantVariationXSlopeIn->GetParError(2);
  Double_t calc_SenOctXSlopeUnRegInChisquare =   fitOctantVariationXSlopeIn->GetChisquare();
  Double_t calc_SenOctXSlopeUnRegInNDF       =   fitOctantVariationXSlopeIn->GetNDF();

  Double_t calc_SenOctXSlopeUnRegOutp0        =   fitOctantVariationXSlopeOut->GetParameter(0);
  Double_t calc_SenOctXSlopeUnRegOutp1        =   fitOctantVariationXSlopeOut->GetParameter(1);
  Double_t calc_SenOctXSlopeUnRegOutp2        =   fitOctantVariationXSlopeOut->GetParameter(2);
  Double_t calc_eSenOctXSlopeUnRegOutp0       =   fitOctantVariationXSlopeOut->GetParError(0);
  Double_t calc_eSenOctXSlopeUnRegOutp1       =   fitOctantVariationXSlopeOut->GetParError(1);
  Double_t calc_eSenOctXSlopeUnRegOutp2       =   fitOctantVariationXSlopeOut->GetParError(2);
  Double_t calc_SenOctXSlopeUnRegOutChisquare =   fitOctantVariationXSlopeOut->GetChisquare();
  Double_t calc_SenOctXSlopeUnRegOutNDF       =   fitOctantVariationXSlopeOut->GetNDF();

  TLegend *legRunSenOctXSlopeUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunSenOctXSlopeUnReg1->AddEntry("runSenOctXSlopeGraphUnRegIn1",  Form("In X'= %4.1f#pm%4.1f",calc_SenOctXSlopeUnRegInp0,calc_eSenOctXSlopeUnRegInp0),"lp");
  legRunSenOctXSlopeUnReg1->AddEntry("runSenOctXSlopeGraphUnRegOut1",  Form("Out X'= %4.1f#pm%4.1f",calc_SenOctXSlopeUnRegOutp0,calc_eSenOctXSlopeUnRegOutp0),"lp");
  legRunSenOctXSlopeUnReg1->SetTextSize(0.040);
  legRunSenOctXSlopeUnReg1->SetFillColor(0);
  legRunSenOctXSlopeUnReg1->SetBorderSize(2);
  legRunSenOctXSlopeUnReg1->Draw();

  fitOctantVariationXSlopeIn->Draw("same");
  fitOctantVariationXSlopeOut->Draw("same");

  gPad->Update();

  pad191->cd(3);

  TGraphErrors * runSenOctYGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,Y_inAmp,zero,eY_inAmp);
  runSenOctYGraphUnRegIn1->SetName("runSenOctYGraphUnRegIn1");
  runSenOctYGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenOctYGraphUnRegIn1->SetLineColor(kGreen);
  runSenOctYGraphUnRegIn1->SetMarkerStyle(21);
  runSenOctYGraphUnRegIn1->SetMarkerSize(0.5);
  runSenOctYGraphUnRegIn1->SetLineWidth(1);
  runSenOctYGraphUnRegIn1->Fit("fitOctantVariationYIn","E M R F 0 Q");
  fitOctantVariationYIn->SetLineStyle(1);
  fitOctantVariationYIn->SetLineWidth(2);
  fitOctantVariationYIn->SetLineColor(kGreen);

  TGraphErrors * runSenOctYGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,Y_outAmp,zero,eY_outAmp);
  runSenOctYGraphUnRegOut1->SetName("runSenOctYGraphUnRegOut1");
  runSenOctYGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenOctYGraphUnRegOut1->SetLineColor(kBlack);
  runSenOctYGraphUnRegOut1->SetMarkerStyle(21);
  runSenOctYGraphUnRegOut1->SetMarkerSize(0.5);
  runSenOctYGraphUnRegOut1->SetLineWidth(1);
  runSenOctYGraphUnRegOut1->Fit("fitOctantVariationYOut","E M R F 0 Q");
  fitOctantVariationYOut->SetLineStyle(1);
  fitOctantVariationYOut->SetLineWidth(2);
  fitOctantVariationYOut->SetLineColor(kBlack);

  TMultiGraph *runSenOctYGraphUnReg1 = new TMultiGraph();
  runSenOctYGraphUnReg1->Add(runSenOctYGraphUnRegIn1);
  runSenOctYGraphUnReg1->Add(runSenOctYGraphUnRegOut1);
  runSenOctYGraphUnReg1->Draw("AP");
  runSenOctYGraphUnReg1->SetTitle("");
  runSenOctYGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runSenOctYGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runSenOctYGraphUnReg1->GetYaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenOctYGraphUnReg1->GetXaxis()->CenterTitle();
  runSenOctYGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisSenOctYGraphUnReg1= runSenOctYGraphUnReg1->GetXaxis();
  xaxisSenOctYGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  fitOctantVariationYIn->Draw("same");
  fitOctantVariationYOut->Draw("same");

  TPaveStats *statsSenOctYUnRegIn1 =(TPaveStats*)runSenOctYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenOctYUnRegOut1 =(TPaveStats*)runSenOctYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSenOctYUnRegIn1->SetTextColor(kGreen);
  statsSenOctYUnRegIn1->SetFillColor(kWhite); 
  statsSenOctYUnRegOut1->SetTextColor(kBlack);
  statsSenOctYUnRegOut1->SetFillColor(kWhite);
  statsSenOctYUnRegIn1->SetTextSize(0.045);
  statsSenOctYUnRegOut1->SetTextSize(0.045);
  statsSenOctYUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsSenOctYUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsSenOctYUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsSenOctYUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsSenOctYUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsSenOctYUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsSenOctYUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsSenOctYUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenOctYUnRegInp0        =   fitOctantVariationYIn->GetParameter(0);
  Double_t calc_SenOctYUnRegInp1        =   fitOctantVariationYIn->GetParameter(1);
  Double_t calc_SenOctYUnRegInp2        =   fitOctantVariationYIn->GetParameter(2);
  Double_t calc_eSenOctYUnRegInp0       =   fitOctantVariationYIn->GetParError(0);
  Double_t calc_eSenOctYUnRegInp1       =   fitOctantVariationYIn->GetParError(1);
  Double_t calc_eSenOctYUnRegInp2       =   fitOctantVariationYIn->GetParError(2);
  Double_t calc_SenOctYUnRegInChisquare =   fitOctantVariationYIn->GetChisquare();
  Double_t calc_SenOctYUnRegInNDF       =   fitOctantVariationYIn->GetNDF();

  Double_t calc_SenOctYUnRegOutp0        =   fitOctantVariationYOut->GetParameter(0);
  Double_t calc_SenOctYUnRegOutp1        =   fitOctantVariationYOut->GetParameter(1);
  Double_t calc_SenOctYUnRegOutp2        =   fitOctantVariationYOut->GetParameter(2);
  Double_t calc_eSenOctYUnRegOutp0       =   fitOctantVariationYOut->GetParError(0);
  Double_t calc_eSenOctYUnRegOutp1       =   fitOctantVariationYOut->GetParError(1);
  Double_t calc_eSenOctYUnRegOutp2       =   fitOctantVariationYOut->GetParError(2);
  Double_t calc_SenOctYUnRegOutChisquare =   fitOctantVariationYOut->GetChisquare();
  Double_t calc_SenOctYUnRegOutNDF       =   fitOctantVariationYOut->GetNDF();

  TLegend *legRunSenOctYUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunSenOctYUnReg1->AddEntry("runSenOctYGraphUnRegIn1",  Form("In Y= %4.1f#pm%4.1f",calc_SenOctYUnRegInp0,calc_eSenOctYUnRegInp0),"lp");
  legRunSenOctYUnReg1->AddEntry("runSenOctYGraphUnRegOut1",  Form("Out Y= %4.1f#pm%4.1f",calc_SenOctYUnRegOutp0,calc_eSenOctYUnRegOutp0),"lp");
  legRunSenOctYUnReg1->SetTextSize(0.040);
  legRunSenOctYUnReg1->SetFillColor(0);
  legRunSenOctYUnReg1->SetBorderSize(2);
  legRunSenOctYUnReg1->Draw();

  gPad->Update();


  pad191->cd(4);

  TGraphErrors * runSenOctYSlopeGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,YSlope_inAmp,zero,eYSlope_inAmp);
  runSenOctYSlopeGraphUnRegIn1->SetName("runSenOctYSlopeGraphUnRegIn1");
  runSenOctYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenOctYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenOctYSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runSenOctYSlopeGraphUnRegIn1->SetMarkerSize(0.6);
  runSenOctYSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenOctYSlopeGraphUnRegIn1->Fit("fitOctantVariationYSlopeIn","E M R F 0 Q");
  fitOctantVariationYSlopeIn->SetLineStyle(1);
  fitOctantVariationYSlopeIn->SetLineWidth(2);
  fitOctantVariationYSlopeIn->SetLineColor(kGreen);

  TGraphErrors * runSenOctYSlopeGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,YSlope_outAmp,zero,eYSlope_outAmp);
  runSenOctYSlopeGraphUnRegOut1->SetName("runSenOctYSlopeGraphUnRegOut1");
  runSenOctYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenOctYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenOctYSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runSenOctYSlopeGraphUnRegOut1->SetMarkerSize(0.6);
  runSenOctYSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenOctYSlopeGraphUnRegOut1->Fit("fitOctantVariationYSlopeOut","E M R F 0 Q");
  fitOctantVariationYSlopeOut->SetLineStyle(1);
  fitOctantVariationYSlopeOut->SetLineWidth(2);
  fitOctantVariationYSlopeOut->SetLineColor(kBlack);

  TMultiGraph *runSenOctYSlopeGraphUnReg1 = new TMultiGraph();
  runSenOctYSlopeGraphUnReg1->Add(runSenOctYSlopeGraphUnRegIn1);
  runSenOctYSlopeGraphUnReg1->Add(runSenOctYSlopeGraphUnRegOut1);
  runSenOctYSlopeGraphUnReg1->Draw("AP");
  runSenOctYSlopeGraphUnReg1->SetTitle("");
  runSenOctYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runSenOctYSlopeGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runSenOctYSlopeGraphUnReg1->GetYaxis()->SetTitle("YSlope Sensitivity [ppm/#murad]");
  runSenOctYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenOctYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisSenOctYSlopeGraphUnReg1= runSenOctYSlopeGraphUnReg1->GetXaxis();
  xaxisSenOctYSlopeGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenOctYSlopeUnRegIn1 =(TPaveStats*)runSenOctYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenOctYSlopeUnRegOut1 =(TPaveStats*)runSenOctYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSenOctYSlopeUnRegIn1->SetTextColor(kGreen);
  statsSenOctYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsSenOctYSlopeUnRegOut1->SetTextColor(kBlack);
  statsSenOctYSlopeUnRegOut1->SetFillColor(kWhite);
  statsSenOctYSlopeUnRegIn1->SetTextSize(0.045);
  statsSenOctYSlopeUnRegOut1->SetTextSize(0.045);
  statsSenOctYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsSenOctYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsSenOctYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsSenOctYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsSenOctYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsSenOctYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsSenOctYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsSenOctYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenOctYSlopeUnRegInp0        =   fitOctantVariationYSlopeIn->GetParameter(0);
  Double_t calc_SenOctYSlopeUnRegInp1        =   fitOctantVariationYSlopeIn->GetParameter(1);
  Double_t calc_SenOctYSlopeUnRegInp2        =   fitOctantVariationYSlopeIn->GetParameter(2);
  Double_t calc_eSenOctYSlopeUnRegInp0       =   fitOctantVariationYSlopeIn->GetParError(0);
  Double_t calc_eSenOctYSlopeUnRegInp1       =   fitOctantVariationYSlopeIn->GetParError(1);
  Double_t calc_eSenOctYSlopeUnRegInp2       =   fitOctantVariationYSlopeIn->GetParError(2);
  Double_t calc_SenOctYSlopeUnRegInChisquare =   fitOctantVariationYSlopeIn->GetChisquare();
  Double_t calc_SenOctYSlopeUnRegInNDF       =   fitOctantVariationYSlopeIn->GetNDF();

  Double_t calc_SenOctYSlopeUnRegOutp0        =   fitOctantVariationYSlopeOut->GetParameter(0);
  Double_t calc_SenOctYSlopeUnRegOutp1        =   fitOctantVariationYSlopeOut->GetParameter(1);
  Double_t calc_SenOctYSlopeUnRegOutp2        =   fitOctantVariationYSlopeOut->GetParameter(2);
  Double_t calc_eSenOctYSlopeUnRegOutp0       =   fitOctantVariationYSlopeOut->GetParError(0);
  Double_t calc_eSenOctYSlopeUnRegOutp1       =   fitOctantVariationYSlopeOut->GetParError(1);
  Double_t calc_eSenOctYSlopeUnRegOutp2       =   fitOctantVariationYSlopeOut->GetParError(2);
  Double_t calc_SenOctYSlopeUnRegOutChisquare =   fitOctantVariationYSlopeOut->GetChisquare();
  Double_t calc_SenOctYSlopeUnRegOutNDF       =   fitOctantVariationYSlopeOut->GetNDF();

  TLegend *legRunSenOctYSlopeUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunSenOctYSlopeUnReg1->AddEntry("runSenOctYSlopeGraphUnRegIn1",  Form("In Y'= %4.1f#pm%4.1f",calc_SenOctYSlopeUnRegInp0,calc_eSenOctYSlopeUnRegInp0),"lp");
  legRunSenOctYSlopeUnReg1->AddEntry("runSenOctYSlopeGraphUnRegOut1",  Form("Out Y'= %4.1f#pm%4.1f",calc_SenOctYSlopeUnRegOutp0,calc_eSenOctYSlopeUnRegOutp0),"lp");
  legRunSenOctYSlopeUnReg1->SetTextSize(0.040);
  legRunSenOctYSlopeUnReg1->SetFillColor(0);
  legRunSenOctYSlopeUnReg1->SetBorderSize(2);
  legRunSenOctYSlopeUnReg1->Draw();

  fitOctantVariationYSlopeIn->Draw("same");
  fitOctantVariationYSlopeOut->Draw("same");

  gPad->Update();

  pad191->cd(5);

  TGraphErrors * runSenOctEGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,E_inAmp,zero,eE_inAmp);
  runSenOctEGraphUnRegIn1->SetName("runSenOctEGraphUnRegIn1");
  runSenOctEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenOctEGraphUnRegIn1->SetLineColor(kBlue);
  runSenOctEGraphUnRegIn1->SetMarkerStyle(20);
  runSenOctEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenOctEGraphUnRegIn1->SetLineWidth(1);
  runSenOctEGraphUnRegIn1->Fit("fitOctantVariationEIn","E M R F 0 Q");
  fitOctantVariationEIn->SetLineStyle(1);
  fitOctantVariationEIn->SetLineWidth(2);
  fitOctantVariationEIn->SetLineColor(kBlue);

  TGraphErrors * runSenOctEGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,E_outAmp,zero,eE_outAmp);
  runSenOctEGraphUnRegOut1->SetName("runSenOctEGraphUnRegOut1");
  runSenOctEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenOctEGraphUnRegOut1->SetLineColor(kBlack);
  runSenOctEGraphUnRegOut1->SetMarkerStyle(20);
  runSenOctEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenOctEGraphUnRegOut1->SetLineWidth(1);
  runSenOctEGraphUnRegOut1->Fit("fitOctantVariationEOut","E M R F 0 Q");
  fitOctantVariationEOut->SetLineStyle(1);
  fitOctantVariationEOut->SetLineWidth(2);
  fitOctantVariationEOut->SetLineColor(kBlack);

  TMultiGraph *runSenOctEGraphUnReg1 = new TMultiGraph();
  runSenOctEGraphUnReg1->Add(runSenOctEGraphUnRegIn1);
  runSenOctEGraphUnReg1->Add(runSenOctEGraphUnRegOut1);
  runSenOctEGraphUnReg1->Draw("AP");
  runSenOctEGraphUnReg1->SetTitle("");
  runSenOctEGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runSenOctEGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runSenOctEGraphUnReg1->GetYaxis()->SetTitle("E Sensitivity [dim less]");
  runSenOctEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenOctEGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisSenOctEGraphUnReg1= runSenOctEGraphUnReg1->GetXaxis();
  xaxisSenOctEGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsSenOctEUnRegIn1 =(TPaveStats*)runSenOctEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSenOctEUnRegOut1 =(TPaveStats*)runSenOctEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSenOctEUnRegIn1->SetTextColor(kBlue);
  statsSenOctEUnRegIn1->SetFillColor(kWhite); 
  statsSenOctEUnRegOut1->SetTextColor(kBlack);
  statsSenOctEUnRegOut1->SetFillColor(kWhite);
  statsSenOctEUnRegIn1->SetTextSize(0.035);
  statsSenOctEUnRegOut1->SetTextSize(0.035);
  statsSenOctEUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsSenOctEUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsSenOctEUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsSenOctEUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsSenOctEUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsSenOctEUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsSenOctEUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsSenOctEUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_SenOctEUnRegInp0        =   fitOctantVariationEIn->GetParameter(0);
  Double_t calc_SenOctEUnRegInp1        =   fitOctantVariationEIn->GetParameter(1);
  Double_t calc_SenOctEUnRegInp2        =   fitOctantVariationEIn->GetParameter(2);
  Double_t calc_eSenOctEUnRegInp0       =   fitOctantVariationEIn->GetParError(0);
  Double_t calc_eSenOctEUnRegInp1       =   fitOctantVariationEIn->GetParError(1);
  Double_t calc_eSenOctEUnRegInp2       =   fitOctantVariationEIn->GetParError(2);
  Double_t calc_SenOctEUnRegInChisquare =   fitOctantVariationEIn->GetChisquare();
  Double_t calc_SenOctEUnRegInNDF       =   fitOctantVariationEIn->GetNDF();

  Double_t calc_SenOctEUnRegOutp0        =   fitOctantVariationEOut->GetParameter(0);
  Double_t calc_SenOctEUnRegOutp1        =   fitOctantVariationEOut->GetParameter(1);
  Double_t calc_SenOctEUnRegOutp2        =   fitOctantVariationEOut->GetParameter(2);
  Double_t calc_eSenOctEUnRegOutp0       =   fitOctantVariationEOut->GetParError(0);
  Double_t calc_eSenOctEUnRegOutp1       =   fitOctantVariationEOut->GetParError(1);
  Double_t calc_eSenOctEUnRegOutp2       =   fitOctantVariationEOut->GetParError(2);
  Double_t calc_SenOctEUnRegOutChisquare =   fitOctantVariationEOut->GetChisquare();
  Double_t calc_SenOctEUnRegOutNDF       =   fitOctantVariationEOut->GetNDF();

  TLegend *legRunSenOctEUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunSenOctEUnReg1->AddEntry("runSenOctEGraphUnRegIn1",  Form("In E= %2.2g#pm%2.2g",calc_SenOctEUnRegInp0,calc_eSenOctEUnRegInp0),"lp");
  legRunSenOctEUnReg1->AddEntry("runSenOctEGraphUnRegOut1",  Form("Out E= %2.2g#pm%2.2g",calc_SenOctEUnRegOutp0,calc_eSenOctEUnRegOutp0),"lp");
  legRunSenOctEUnReg1->SetTextSize(0.035);
  legRunSenOctEUnReg1->SetFillColor(0);
  legRunSenOctEUnReg1->SetBorderSize(2);
  legRunSenOctEUnReg1->Draw();

  fitOctantVariationEIn->Draw("same");
  fitOctantVariationEOut->Draw("same");

  gPad->Update();

  c19->Update();
  c19->SaveAs(Form("%s/plots/%s/%sOctantSensitivity%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  
  }
  /*********************************************************************************/
  /*********************************************************************************/

  if(OCTANT_VARIATION_COR){

  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(0.8);

  TCanvas *c20 = new TCanvas("c20","c20",cx1,cy1,1200,950);
  TPad * pad200 = new TPad("pad200","pad200",ps1,ps2,ps4,ps4);
  TPad * pad201 = new TPad("pad201","pad201",ps1,ps1,ps4,ps3);
  pad200->Draw();
  pad201->Draw();
  pad200->cd();
  TText * ct200 = new TText(tll,tlr,Form("%sN to Delta %s Variation of %s Correction with Octant",regTitle,dataInfo,plotTitle));
  ct200->SetTextSize(tsiz);
  ct200->Draw();
  pad201->cd();
  pad201->Divide(2,3);


  pad201->cd(1);

  TGraphErrors * runCorOctXGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,X_inCor,zero,eX_inCor);
  runCorOctXGraphUnRegIn1->SetName("runCorOctXGraphUnRegIn1");
  runCorOctXGraphUnRegIn1->SetMarkerColor(kRed);
  runCorOctXGraphUnRegIn1->SetLineColor(kRed);
  runCorOctXGraphUnRegIn1->SetMarkerStyle(21);
  runCorOctXGraphUnRegIn1->SetMarkerSize(0.7);
  runCorOctXGraphUnRegIn1->SetLineWidth(1);
  runCorOctXGraphUnRegIn1->Fit("fitOctantVariationCorXIn","E M R F 0 Q");
  fitOctantVariationCorXIn->SetLineStyle(3);
  fitOctantVariationCorXIn->SetLineWidth(2);
  fitOctantVariationCorXIn->SetLineColor(kRed);

  TGraphErrors * runCorOctXGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,X_outCor,zero,eX_outCor);
  runCorOctXGraphUnRegOut1->SetName("runCorOctXGraphUnRegOut1");
  runCorOctXGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctXGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctXGraphUnRegOut1->SetMarkerStyle(21);
  runCorOctXGraphUnRegOut1->SetMarkerSize(0.7);
  runCorOctXGraphUnRegOut1->SetLineWidth(1);
  runCorOctXGraphUnRegOut1->Fit("fitOctantVariationCorXOut","E M R F 0 Q");
  fitOctantVariationCorXOut->SetLineStyle(3);
  fitOctantVariationCorXOut->SetLineWidth(2);
  fitOctantVariationCorXOut->SetLineColor(kBlack);

  TMultiGraph *runCorOctXGraphUnReg1 = new TMultiGraph();
  runCorOctXGraphUnReg1->Add(runCorOctXGraphUnRegIn1);
  runCorOctXGraphUnReg1->Add(runCorOctXGraphUnRegOut1);
  runCorOctXGraphUnReg1->Draw("AP");
  runCorOctXGraphUnReg1->SetTitle("");
  runCorOctXGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctXGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctXGraphUnReg1->GetYaxis()->SetTitle("X Correction [ppm]");
  runCorOctXGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctXGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctXGraphUnReg1= runCorOctXGraphUnReg1->GetXaxis();
  xaxisCorOctXGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorOctXUnRegIn1 =(TPaveStats*)runCorOctXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctXUnRegOut1 =(TPaveStats*)runCorOctXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctXUnRegIn1->SetTextColor(kRed);
  statsCorOctXUnRegIn1->SetFillColor(kWhite); 
  statsCorOctXUnRegOut1->SetTextColor(kBlack);
  statsCorOctXUnRegOut1->SetFillColor(kWhite);
  statsCorOctXUnRegIn1->SetTextSize(0.045);
  statsCorOctXUnRegOut1->SetTextSize(0.045);
  statsCorOctXUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctXUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctXUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctXUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctXUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctXUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctXUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctXUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctXUnRegInp0        =   fitOctantVariationCorXIn->GetParameter(0);
  Double_t calc_CorOctXUnRegInp1        =   fitOctantVariationCorXIn->GetParameter(1);
  Double_t calc_CorOctXUnRegInp2        =   fitOctantVariationCorXIn->GetParameter(2);
  Double_t calc_eCorOctXUnRegInp0       =   fitOctantVariationCorXIn->GetParError(0);
  Double_t calc_eCorOctXUnRegInp1       =   fitOctantVariationCorXIn->GetParError(1);
  Double_t calc_eCorOctXUnRegInp2       =   fitOctantVariationCorXIn->GetParError(2);
  Double_t calc_CorOctXUnRegInChisquare =   fitOctantVariationCorXIn->GetChisquare();
  Double_t calc_CorOctXUnRegInNDF       =   fitOctantVariationCorXIn->GetNDF();

  Double_t calc_CorOctXUnRegOutp0        =   fitOctantVariationCorXOut->GetParameter(0);
  Double_t calc_CorOctXUnRegOutp1        =   fitOctantVariationCorXOut->GetParameter(1);
  Double_t calc_CorOctXUnRegOutp2        =   fitOctantVariationCorXOut->GetParameter(2);
  Double_t calc_eCorOctXUnRegOutp0       =   fitOctantVariationCorXOut->GetParError(0);
  Double_t calc_eCorOctXUnRegOutp1       =   fitOctantVariationCorXOut->GetParError(1);
  Double_t calc_eCorOctXUnRegOutp2       =   fitOctantVariationCorXOut->GetParError(2);
  Double_t calc_CorOctXUnRegOutChisquare =   fitOctantVariationCorXOut->GetChisquare();
  Double_t calc_CorOctXUnRegOutNDF       =   fitOctantVariationCorXOut->GetNDF();

  TLegend *legRunCorOctXUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctXUnReg1->AddEntry("runCorOctXGraphUnRegIn1",  Form("In X= %4.2f#pm%4.2f",calc_CorOctXUnRegInp0,calc_eCorOctXUnRegInp0),"lp");
  legRunCorOctXUnReg1->AddEntry("runCorOctXGraphUnRegOut1",  Form("Out X= %4.2f#pm%4.2f",calc_CorOctXUnRegOutp0,calc_eCorOctXUnRegOutp0),"lp");
  legRunCorOctXUnReg1->SetTextSize(0.040);
  legRunCorOctXUnReg1->SetFillColor(0);
  legRunCorOctXUnReg1->SetBorderSize(2);
  legRunCorOctXUnReg1->Draw();

  fitOctantVariationCorXIn->Draw("same");
  fitOctantVariationCorXOut->Draw("same");

  gPad->Update();


  pad201->cd(2);

  TGraphErrors * runCorOctXSlopeGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,XSlope_inCor,zero,eXSlope_inCor);
  runCorOctXSlopeGraphUnRegIn1->SetName("runCorOctXSlopeGraphUnRegIn1");
  runCorOctXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runCorOctXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runCorOctXSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runCorOctXSlopeGraphUnRegIn1->SetMarkerSize(0.9);
  runCorOctXSlopeGraphUnRegIn1->SetLineWidth(1);
  runCorOctXSlopeGraphUnRegIn1->Fit("fitOctantVariationCorXSlopeIn","E M R F 0 Q");
  fitOctantVariationCorXSlopeIn->SetLineStyle(2);
  fitOctantVariationCorXSlopeIn->SetLineWidth(2);
  fitOctantVariationCorXSlopeIn->SetLineColor(kRed);

  TGraphErrors * runCorOctXSlopeGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,XSlope_outCor,zero,eXSlope_outCor);
  runCorOctXSlopeGraphUnRegOut1->SetName("runCorOctXSlopeGraphUnRegOut1");
  runCorOctXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctXSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runCorOctXSlopeGraphUnRegOut1->SetMarkerSize(0.9);
  runCorOctXSlopeGraphUnRegOut1->SetLineWidth(1);
  runCorOctXSlopeGraphUnRegOut1->Fit("fitOctantVariationCorXSlopeOut","E M R F 0 Q");
  fitOctantVariationCorXSlopeOut->SetLineStyle(2);
  fitOctantVariationCorXSlopeOut->SetLineWidth(2);
  fitOctantVariationCorXSlopeOut->SetLineColor(kBlack);


  TMultiGraph *runCorOctXSlopeGraphUnReg1 = new TMultiGraph();
  runCorOctXSlopeGraphUnReg1->Add(runCorOctXSlopeGraphUnRegIn1);
  runCorOctXSlopeGraphUnReg1->Add(runCorOctXSlopeGraphUnRegOut1);
  runCorOctXSlopeGraphUnReg1->Draw("AP");
  runCorOctXSlopeGraphUnReg1->SetTitle("");
  runCorOctXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctXSlopeGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctXSlopeGraphUnReg1->GetYaxis()->SetTitle("XSlope Correction [ppm]");
  runCorOctXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctXSlopeGraphUnReg1= runCorOctXSlopeGraphUnReg1->GetXaxis();
  xaxisCorOctXSlopeGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorOctXSlopeUnRegIn1 =(TPaveStats*)runCorOctXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctXSlopeUnRegOut1 =(TPaveStats*)runCorOctXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctXSlopeUnRegIn1->SetTextColor(kRed);
  statsCorOctXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsCorOctXSlopeUnRegOut1->SetTextColor(kBlack);
  statsCorOctXSlopeUnRegOut1->SetFillColor(kWhite);
  statsCorOctXSlopeUnRegIn1->SetTextSize(0.045);
  statsCorOctXSlopeUnRegOut1->SetTextSize(0.045);
  statsCorOctXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctXSlopeUnRegInp0        =   fitOctantVariationCorXSlopeIn->GetParameter(0);
  Double_t calc_CorOctXSlopeUnRegInp1        =   fitOctantVariationCorXSlopeIn->GetParameter(1);
  Double_t calc_CorOctXSlopeUnRegInp2        =   fitOctantVariationCorXSlopeIn->GetParameter(2);
  Double_t calc_eCorOctXSlopeUnRegInp0       =   fitOctantVariationCorXSlopeIn->GetParError(0);
  Double_t calc_eCorOctXSlopeUnRegInp1       =   fitOctantVariationCorXSlopeIn->GetParError(1);
  Double_t calc_eCorOctXSlopeUnRegInp2       =   fitOctantVariationCorXSlopeIn->GetParError(2);
  Double_t calc_CorOctXSlopeUnRegInChisquare =   fitOctantVariationCorXSlopeIn->GetChisquare();
  Double_t calc_CorOctXSlopeUnRegInNDF       =   fitOctantVariationCorXSlopeIn->GetNDF();

  Double_t calc_CorOctXSlopeUnRegOutp0        =   fitOctantVariationCorXSlopeOut->GetParameter(0);
  Double_t calc_CorOctXSlopeUnRegOutp1        =   fitOctantVariationCorXSlopeOut->GetParameter(1);
  Double_t calc_CorOctXSlopeUnRegOutp2        =   fitOctantVariationCorXSlopeOut->GetParameter(2);
  Double_t calc_eCorOctXSlopeUnRegOutp0       =   fitOctantVariationCorXSlopeOut->GetParError(0);
  Double_t calc_eCorOctXSlopeUnRegOutp1       =   fitOctantVariationCorXSlopeOut->GetParError(1);
  Double_t calc_eCorOctXSlopeUnRegOutp2       =   fitOctantVariationCorXSlopeOut->GetParError(2);
  Double_t calc_CorOctXSlopeUnRegOutChisquare =   fitOctantVariationCorXSlopeOut->GetChisquare();
  Double_t calc_CorOctXSlopeUnRegOutNDF       =   fitOctantVariationCorXSlopeOut->GetNDF();

  TLegend *legRunCorOctXSlopeUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctXSlopeUnReg1->AddEntry("runCorOctXSlopeGraphUnRegIn1",  Form("In X'= %4.2f#pm%4.2f",calc_CorOctXSlopeUnRegInp0,calc_eCorOctXSlopeUnRegInp0),"lp");
  legRunCorOctXSlopeUnReg1->AddEntry("runCorOctXSlopeGraphUnRegOut1",  Form("Out X'= %4.2f#pm%4.2f",calc_CorOctXSlopeUnRegOutp0,calc_eCorOctXSlopeUnRegOutp0),"lp");
  legRunCorOctXSlopeUnReg1->SetTextSize(0.040);
  legRunCorOctXSlopeUnReg1->SetFillColor(0);
  legRunCorOctXSlopeUnReg1->SetBorderSize(2);
  legRunCorOctXSlopeUnReg1->Draw();

  fitOctantVariationCorXSlopeIn->Draw("same");
  fitOctantVariationCorXSlopeOut->Draw("same");

  gPad->Update();

  pad201->cd(3);

  TGraphErrors * runCorOctYGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,Y_inCor,zero,eY_inCor);
  runCorOctYGraphUnRegIn1->SetName("runCorOctYGraphUnRegIn1");
  runCorOctYGraphUnRegIn1->SetMarkerColor(kGreen);
  runCorOctYGraphUnRegIn1->SetLineColor(kGreen);
  runCorOctYGraphUnRegIn1->SetMarkerStyle(21);
  runCorOctYGraphUnRegIn1->SetMarkerSize(0.7);
  runCorOctYGraphUnRegIn1->SetLineWidth(1);
  runCorOctYGraphUnRegIn1->Fit("fitOctantVariationCorYIn","E M R F 0 Q");
  fitOctantVariationCorYIn->SetLineStyle(3);
  fitOctantVariationCorYIn->SetLineWidth(2);
  fitOctantVariationCorYIn->SetLineColor(kGreen);

  TGraphErrors * runCorOctYGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,Y_outCor,zero,eY_outCor);
  runCorOctYGraphUnRegOut1->SetName("runCorOctYGraphUnRegOut1");
  runCorOctYGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctYGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctYGraphUnRegOut1->SetMarkerStyle(21);
  runCorOctYGraphUnRegOut1->SetMarkerSize(0.7);
  runCorOctYGraphUnRegOut1->SetLineWidth(1);
  runCorOctYGraphUnRegOut1->Fit("fitOctantVariationCorYOut","E M R F 0 Q");
  fitOctantVariationCorYOut->SetLineStyle(3);
  fitOctantVariationCorYOut->SetLineWidth(2);
  fitOctantVariationCorYOut->SetLineColor(kBlack);

  TMultiGraph *runCorOctYGraphUnReg1 = new TMultiGraph();
  runCorOctYGraphUnReg1->Add(runCorOctYGraphUnRegIn1);
  runCorOctYGraphUnReg1->Add(runCorOctYGraphUnRegOut1);
  runCorOctYGraphUnReg1->Draw("AP");
  runCorOctYGraphUnReg1->SetTitle("");
  runCorOctYGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctYGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctYGraphUnReg1->GetYaxis()->SetTitle("X Correction [ppm]");
  runCorOctYGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctYGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctYGraphUnReg1= runCorOctYGraphUnReg1->GetXaxis();
  xaxisCorOctYGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  fitOctantVariationCorYIn->Draw("same");
  fitOctantVariationCorYOut->Draw("same");

  TPaveStats *statsCorOctYUnRegIn1 =(TPaveStats*)runCorOctYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctYUnRegOut1 =(TPaveStats*)runCorOctYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctYUnRegIn1->SetTextColor(kGreen);
  statsCorOctYUnRegIn1->SetFillColor(kWhite); 
  statsCorOctYUnRegOut1->SetTextColor(kBlack);
  statsCorOctYUnRegOut1->SetFillColor(kWhite);
  statsCorOctYUnRegIn1->SetTextSize(0.045);
  statsCorOctYUnRegOut1->SetTextSize(0.045);
  statsCorOctYUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctYUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctYUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctYUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctYUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctYUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctYUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctYUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctYUnRegInp0        =   fitOctantVariationCorYIn->GetParameter(0);
  Double_t calc_CorOctYUnRegInp1        =   fitOctantVariationCorYIn->GetParameter(1);
  Double_t calc_CorOctYUnRegInp2        =   fitOctantVariationCorYIn->GetParameter(2);
  Double_t calc_eCorOctYUnRegInp0       =   fitOctantVariationCorYIn->GetParError(0);
  Double_t calc_eCorOctYUnRegInp1       =   fitOctantVariationCorYIn->GetParError(1);
  Double_t calc_eCorOctYUnRegInp2       =   fitOctantVariationCorYIn->GetParError(2);
  Double_t calc_CorOctYUnRegInChisquare =   fitOctantVariationCorYIn->GetChisquare();
  Double_t calc_CorOctYUnRegInNDF       =   fitOctantVariationCorYIn->GetNDF();

  Double_t calc_CorOctYUnRegOutp0        =   fitOctantVariationCorYOut->GetParameter(0);
  Double_t calc_CorOctYUnRegOutp1        =   fitOctantVariationCorYOut->GetParameter(1);
  Double_t calc_CorOctYUnRegOutp2        =   fitOctantVariationCorYOut->GetParameter(2);
  Double_t calc_eCorOctYUnRegOutp0       =   fitOctantVariationCorYOut->GetParError(0);
  Double_t calc_eCorOctYUnRegOutp1       =   fitOctantVariationCorYOut->GetParError(1);
  Double_t calc_eCorOctYUnRegOutp2       =   fitOctantVariationCorYOut->GetParError(2);
  Double_t calc_CorOctYUnRegOutChisquare =   fitOctantVariationCorYOut->GetChisquare();
  Double_t calc_CorOctYUnRegOutNDF       =   fitOctantVariationCorYOut->GetNDF();

  TLegend *legRunCorOctYUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctYUnReg1->AddEntry("runCorOctYGraphUnRegIn1",  Form("In Y= %4.2f#pm%4.2f",calc_CorOctYUnRegInp0,calc_eCorOctYUnRegInp0),"lp");
  legRunCorOctYUnReg1->AddEntry("runCorOctYGraphUnRegOut1",  Form("Out Y= %4.2f#pm%4.2f",calc_CorOctYUnRegOutp0,calc_eCorOctYUnRegOutp0),"lp");
  legRunCorOctYUnReg1->SetTextSize(0.040);
  legRunCorOctYUnReg1->SetFillColor(0);
  legRunCorOctYUnReg1->SetBorderSize(2);
  legRunCorOctYUnReg1->Draw();

  gPad->Update();


  pad201->cd(4);

  TGraphErrors * runCorOctYSlopeGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,YSlope_inCor,zero,eYSlope_inCor);
  runCorOctYSlopeGraphUnRegIn1->SetName("runCorOctYSlopeGraphUnRegIn1");
  runCorOctYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runCorOctYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runCorOctYSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runCorOctYSlopeGraphUnRegIn1->SetMarkerSize(0.9);
  runCorOctYSlopeGraphUnRegIn1->SetLineWidth(1);
  runCorOctYSlopeGraphUnRegIn1->Fit("fitOctantVariationCorYSlopeIn","E M R F 0 Q");
  fitOctantVariationCorYSlopeIn->SetLineStyle(2);
  fitOctantVariationCorYSlopeIn->SetLineWidth(2);
  fitOctantVariationCorYSlopeIn->SetLineColor(kGreen);

  TGraphErrors * runCorOctYSlopeGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,YSlope_outCor,zero,eYSlope_outCor);
  runCorOctYSlopeGraphUnRegOut1->SetName("runCorOctYSlopeGraphUnRegOut1");
  runCorOctYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctYSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runCorOctYSlopeGraphUnRegOut1->SetMarkerSize(0.9);
  runCorOctYSlopeGraphUnRegOut1->SetLineWidth(1);
  runCorOctYSlopeGraphUnRegOut1->Fit("fitOctantVariationCorYSlopeOut","E M R F 0 Q");
  fitOctantVariationCorYSlopeOut->SetLineStyle(2);
  fitOctantVariationCorYSlopeOut->SetLineWidth(2);
  fitOctantVariationCorYSlopeOut->SetLineColor(kBlack);

  TMultiGraph *runCorOctYSlopeGraphUnReg1 = new TMultiGraph();
  runCorOctYSlopeGraphUnReg1->Add(runCorOctYSlopeGraphUnRegIn1);
  runCorOctYSlopeGraphUnReg1->Add(runCorOctYSlopeGraphUnRegOut1);
  runCorOctYSlopeGraphUnReg1->Draw("AP");
  runCorOctYSlopeGraphUnReg1->SetTitle("");
  runCorOctYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctYSlopeGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctYSlopeGraphUnReg1->GetYaxis()->SetTitle("YSlope Correction [ppm]");
  runCorOctYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctYSlopeGraphUnReg1= runCorOctYSlopeGraphUnReg1->GetXaxis();
  xaxisCorOctYSlopeGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorOctYSlopeUnRegIn1 =(TPaveStats*)runCorOctYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctYSlopeUnRegOut1 =(TPaveStats*)runCorOctYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctYSlopeUnRegIn1->SetTextColor(kGreen);
  statsCorOctYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsCorOctYSlopeUnRegOut1->SetTextColor(kBlack);
  statsCorOctYSlopeUnRegOut1->SetFillColor(kWhite);
  statsCorOctYSlopeUnRegIn1->SetTextSize(0.045);
  statsCorOctYSlopeUnRegOut1->SetTextSize(0.045);
  statsCorOctYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctYSlopeUnRegInp0        =   fitOctantVariationCorYSlopeIn->GetParameter(0);
  Double_t calc_CorOctYSlopeUnRegInp1        =   fitOctantVariationCorYSlopeIn->GetParameter(1);
  Double_t calc_CorOctYSlopeUnRegInp2        =   fitOctantVariationCorYSlopeIn->GetParameter(2);
  Double_t calc_eCorOctYSlopeUnRegInp0       =   fitOctantVariationCorYSlopeIn->GetParError(0);
  Double_t calc_eCorOctYSlopeUnRegInp1       =   fitOctantVariationCorYSlopeIn->GetParError(1);
  Double_t calc_eCorOctYSlopeUnRegInp2       =   fitOctantVariationCorYSlopeIn->GetParError(2);
  Double_t calc_CorOctYSlopeUnRegInChisquare =   fitOctantVariationCorYSlopeIn->GetChisquare();
  Double_t calc_CorOctYSlopeUnRegInNDF       =   fitOctantVariationCorYSlopeIn->GetNDF();

  Double_t calc_CorOctYSlopeUnRegOutp0        =   fitOctantVariationCorYSlopeOut->GetParameter(0);
  Double_t calc_CorOctYSlopeUnRegOutp1        =   fitOctantVariationCorYSlopeOut->GetParameter(1);
  Double_t calc_CorOctYSlopeUnRegOutp2        =   fitOctantVariationCorYSlopeOut->GetParameter(2);
  Double_t calc_eCorOctYSlopeUnRegOutp0       =   fitOctantVariationCorYSlopeOut->GetParError(0);
  Double_t calc_eCorOctYSlopeUnRegOutp1       =   fitOctantVariationCorYSlopeOut->GetParError(1);
  Double_t calc_eCorOctYSlopeUnRegOutp2       =   fitOctantVariationCorYSlopeOut->GetParError(2);
  Double_t calc_CorOctYSlopeUnRegOutChisquare =   fitOctantVariationCorYSlopeOut->GetChisquare();
  Double_t calc_CorOctYSlopeUnRegOutNDF       =   fitOctantVariationCorYSlopeOut->GetNDF();

  TLegend *legRunCorOctYSlopeUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctYSlopeUnReg1->AddEntry("runCorOctYSlopeGraphUnRegIn1",  Form("In Y'= %4.2f#pm%4.2f",calc_CorOctYSlopeUnRegInp0,calc_eCorOctYSlopeUnRegInp0),"lp");
  legRunCorOctYSlopeUnReg1->AddEntry("runCorOctYSlopeGraphUnRegOut1",  Form("Out Y'= %4.2f#pm%4.2f",calc_CorOctYSlopeUnRegOutp0,calc_eCorOctYSlopeUnRegOutp0),"lp");
  legRunCorOctYSlopeUnReg1->SetTextSize(0.040);
  legRunCorOctYSlopeUnReg1->SetFillColor(0);
  legRunCorOctYSlopeUnReg1->SetBorderSize(2);
  legRunCorOctYSlopeUnReg1->Draw();

  fitOctantVariationCorYSlopeIn->Draw("same");
  fitOctantVariationCorYSlopeOut->Draw("same");

  gPad->Update();

  pad201->cd(5);

  TGraphErrors * runCorOctEGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,E_inCor,zero,eE_inCor);
  runCorOctEGraphUnRegIn1->SetName("runCorOctEGraphUnRegIn1");
  runCorOctEGraphUnRegIn1->SetMarkerColor(kBlue);
  runCorOctEGraphUnRegIn1->SetLineColor(kBlue);
  runCorOctEGraphUnRegIn1->SetMarkerStyle(20);
  runCorOctEGraphUnRegIn1->SetMarkerSize(0.8);
  runCorOctEGraphUnRegIn1->SetLineWidth(1);
  runCorOctEGraphUnRegIn1->Fit("fitOctantVariationCorEIn","E M R F 0 Q");
  fitOctantVariationCorEIn->SetLineStyle(4);
  fitOctantVariationCorEIn->SetLineWidth(2);
  fitOctantVariationCorEIn->SetLineColor(kBlue);

  TGraphErrors * runCorOctEGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,E_outCor,zero,eE_outCor);
  runCorOctEGraphUnRegOut1->SetName("runCorOctEGraphUnRegOut1");
  runCorOctEGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctEGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctEGraphUnRegOut1->SetMarkerStyle(20);
  runCorOctEGraphUnRegOut1->SetMarkerSize(0.8);
  runCorOctEGraphUnRegOut1->SetLineWidth(1);
  runCorOctEGraphUnRegOut1->Fit("fitOctantVariationCorEOut","E M R F 0 Q");
  fitOctantVariationCorEOut->SetLineStyle(4);
  fitOctantVariationCorEOut->SetLineWidth(2);
  fitOctantVariationCorEOut->SetLineColor(kBlack);

  TMultiGraph *runCorOctEGraphUnReg1 = new TMultiGraph();
  runCorOctEGraphUnReg1->Add(runCorOctEGraphUnRegIn1);
  runCorOctEGraphUnReg1->Add(runCorOctEGraphUnRegOut1);
  runCorOctEGraphUnReg1->Draw("AP");
  runCorOctEGraphUnReg1->SetTitle("");
  runCorOctEGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctEGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  runCorOctEGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctEGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctEGraphUnReg1= runCorOctEGraphUnReg1->GetXaxis();
  xaxisCorOctEGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorOctEUnRegIn1 =(TPaveStats*)runCorOctEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctEUnRegOut1 =(TPaveStats*)runCorOctEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctEUnRegIn1->SetTextColor(kBlue);
  statsCorOctEUnRegIn1->SetFillColor(kWhite); 
  statsCorOctEUnRegOut1->SetTextColor(kBlack);
  statsCorOctEUnRegOut1->SetFillColor(kWhite);
  statsCorOctEUnRegIn1->SetTextSize(0.035);
  statsCorOctEUnRegOut1->SetTextSize(0.035);
  statsCorOctEUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctEUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctEUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctEUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctEUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctEUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctEUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctEUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctEUnRegInp0        =   fitOctantVariationCorEIn->GetParameter(0);
  Double_t calc_CorOctEUnRegInp1        =   fitOctantVariationCorEIn->GetParameter(1);
  Double_t calc_CorOctEUnRegInp2        =   fitOctantVariationCorEIn->GetParameter(2);
  Double_t calc_eCorOctEUnRegInp0       =   fitOctantVariationCorEIn->GetParError(0);
  Double_t calc_eCorOctEUnRegInp1       =   fitOctantVariationCorEIn->GetParError(1);
  Double_t calc_eCorOctEUnRegInp2       =   fitOctantVariationCorEIn->GetParError(2);
  Double_t calc_CorOctEUnRegInChisquare =   fitOctantVariationCorEIn->GetChisquare();
  Double_t calc_CorOctEUnRegInNDF       =   fitOctantVariationCorEIn->GetNDF();

  Double_t calc_CorOctEUnRegOutp0        =   fitOctantVariationCorEOut->GetParameter(0);
  Double_t calc_CorOctEUnRegOutp1        =   fitOctantVariationCorEOut->GetParameter(1);
  Double_t calc_CorOctEUnRegOutp2        =   fitOctantVariationCorEOut->GetParameter(2);
  Double_t calc_eCorOctEUnRegOutp0       =   fitOctantVariationCorEOut->GetParError(0);
  Double_t calc_eCorOctEUnRegOutp1       =   fitOctantVariationCorEOut->GetParError(1);
  Double_t calc_eCorOctEUnRegOutp2       =   fitOctantVariationCorEOut->GetParError(2);
  Double_t calc_CorOctEUnRegOutChisquare =   fitOctantVariationCorEOut->GetChisquare();
  Double_t calc_CorOctEUnRegOutNDF       =   fitOctantVariationCorEOut->GetNDF();

  TLegend *legRunCorOctEUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctEUnReg1->AddEntry("runCorOctEGraphUnRegIn1",  Form("In E= %2.2g#pm%2.2g",calc_CorOctEUnRegInp0,calc_eCorOctEUnRegInp0),"lp");
  legRunCorOctEUnReg1->AddEntry("runCorOctEGraphUnRegOut1",  Form("Out E= %2.2g#pm%2.2g",calc_CorOctEUnRegOutp0,calc_eCorOctEUnRegOutp0),"lp");
  legRunCorOctEUnReg1->SetTextSize(0.035);
  legRunCorOctEUnReg1->SetFillColor(0);
  legRunCorOctEUnReg1->SetBorderSize(2);
  legRunCorOctEUnReg1->Draw();

  fitOctantVariationCorEIn->Draw("same");
  fitOctantVariationCorEOut->Draw("same");

  gPad->Update();

  pad201->cd(6);

  TGraphErrors * runCorOctTotalGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,Total_inCor,zero,eTotal_inCor);
  runCorOctTotalGraphUnRegIn1->SetName("runCorOctTotalGraphUnRegIn1");
  runCorOctTotalGraphUnRegIn1->SetMarkerColor(kCyan+2);
  runCorOctTotalGraphUnRegIn1->SetLineColor(kCyan+2);
  runCorOctTotalGraphUnRegIn1->SetMarkerStyle(21);
  runCorOctTotalGraphUnRegIn1->SetMarkerSize(0.7);
  runCorOctTotalGraphUnRegIn1->SetLineWidth(1);
  runCorOctTotalGraphUnRegIn1->Fit("fitOctantVariationCorTotalIn","E M R F 0 Q");
  fitOctantVariationCorTotalIn->SetLineStyle(1);
  fitOctantVariationCorTotalIn->SetLineWidth(2);
  fitOctantVariationCorTotalIn->SetLineColor(kCyan+2);

  TGraphErrors * runCorOctTotalGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,Total_outCor,zero,eTotal_outCor);
  runCorOctTotalGraphUnRegOut1->SetName("runCorOctTotalGraphUnRegOut1");
  runCorOctTotalGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctTotalGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctTotalGraphUnRegOut1->SetMarkerStyle(21);
  runCorOctTotalGraphUnRegOut1->SetMarkerSize(0.7);
  runCorOctTotalGraphUnRegOut1->SetLineWidth(1);
  runCorOctTotalGraphUnRegOut1->Fit("fitOctantVariationCorTotalOut","E M R F 0 Q");
  fitOctantVariationCorTotalOut->SetLineStyle(1);
  fitOctantVariationCorTotalOut->SetLineWidth(2);
  fitOctantVariationCorTotalOut->SetLineColor(kBlack);

  TMultiGraph *runCorOctTotalGraphUnReg1 = new TMultiGraph();
  runCorOctTotalGraphUnReg1->Add(runCorOctTotalGraphUnRegIn1);
  runCorOctTotalGraphUnReg1->Add(runCorOctTotalGraphUnRegOut1);
  runCorOctTotalGraphUnReg1->Draw("AP");
  runCorOctTotalGraphUnReg1->SetTitle("");
  runCorOctTotalGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctTotalGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctTotalGraphUnReg1->GetYaxis()->SetTitle("Total Correction [ppm]");
  runCorOctTotalGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctTotalGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctTotalGraphUnReg1= runCorOctTotalGraphUnReg1->GetXaxis();
  xaxisCorOctTotalGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorOctTotalUnRegIn1 =(TPaveStats*)runCorOctTotalGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctTotalUnRegOut1 =(TPaveStats*)runCorOctTotalGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctTotalUnRegIn1->SetTextColor(kCyan+2);
  statsCorOctTotalUnRegIn1->SetFillColor(kWhite); 
  statsCorOctTotalUnRegOut1->SetTextColor(kBlack);
  statsCorOctTotalUnRegOut1->SetFillColor(kWhite);
  statsCorOctTotalUnRegIn1->SetTextSize(0.035);
  statsCorOctTotalUnRegOut1->SetTextSize(0.035);
  statsCorOctTotalUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctTotalUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctTotalUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctTotalUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctTotalUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctTotalUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctTotalUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctTotalUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctTotalUnRegInp0        =   fitOctantVariationCorTotalIn->GetParameter(0);
  Double_t calc_CorOctTotalUnRegInp1        =   fitOctantVariationCorTotalIn->GetParameter(1);
  Double_t calc_CorOctTotalUnRegInp2        =   fitOctantVariationCorTotalIn->GetParameter(2);
  Double_t calc_eCorOctTotalUnRegInp0       =   fitOctantVariationCorTotalIn->GetParError(0);
  Double_t calc_eCorOctTotalUnRegInp1       =   fitOctantVariationCorTotalIn->GetParError(1);
  Double_t calc_eCorOctTotalUnRegInp2       =   fitOctantVariationCorTotalIn->GetParError(2);
  Double_t calc_CorOctTotalUnRegInChisquare =   fitOctantVariationCorTotalIn->GetChisquare();
  Double_t calc_CorOctTotalUnRegInNDF       =   fitOctantVariationCorTotalIn->GetNDF();

  Double_t calc_CorOctTotalUnRegOutp0        =   fitOctantVariationCorTotalOut->GetParameter(0);
  Double_t calc_CorOctTotalUnRegOutp1        =   fitOctantVariationCorTotalOut->GetParameter(1);
  Double_t calc_CorOctTotalUnRegOutp2        =   fitOctantVariationCorTotalOut->GetParameter(2);
  Double_t calc_eCorOctTotalUnRegOutp0       =   fitOctantVariationCorTotalOut->GetParError(0);
  Double_t calc_eCorOctTotalUnRegOutp1       =   fitOctantVariationCorTotalOut->GetParError(1);
  Double_t calc_eCorOctTotalUnRegOutp2       =   fitOctantVariationCorTotalOut->GetParError(2);
  Double_t calc_CorOctTotalUnRegOutChisquare =   fitOctantVariationCorTotalOut->GetChisquare();
  Double_t calc_CorOctTotalUnRegOutNDF       =   fitOctantVariationCorTotalOut->GetNDF();

  TLegend *legRunCorOctTotalUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctTotalUnReg1->AddEntry("runCorOctTotalGraphUnRegIn1",  Form("In Total= %2.2g#pm%2.2g",calc_CorOctTotalUnRegInp0,calc_eCorOctTotalUnRegInp0),"lp");
  legRunCorOctTotalUnReg1->AddEntry("runCorOctTotalGraphUnRegOut1",  Form("Out Total= %2.2g#pm%2.2g",calc_CorOctTotalUnRegOutp0,calc_eCorOctTotalUnRegOutp0),"lp");
  legRunCorOctTotalUnReg1->SetTextSize(0.035);
  legRunCorOctTotalUnReg1->SetFillColor(0);
  legRunCorOctTotalUnReg1->SetBorderSize(2);
  legRunCorOctTotalUnReg1->Draw();

  fitOctantVariationCorTotalIn->Draw("same");
  fitOctantVariationCorTotalOut->Draw("same");

  gPad->Update();

  ofstream outfileOctantCor(Form("%s/plots/%s/summary%sOctantCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileOctantCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",calc_CorOctTotalUnRegInp0,calc_eCorOctTotalUnRegInp0,calc_CorOctTotalUnRegOutp0,calc_eCorOctTotalUnRegOutp0) <<endl;
  outfileOctantCor.close();

  c20->Update();
  c20->SaveAs(Form("%s/plots/%s/%sOctantCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));

  }

  /*********************************************************************************/
  /*********************************************************************************/
  if(OCTANT_VARIATION_COR_DUB){
  gStyle->SetTitleYOffset(0.6);
  gStyle->SetTitleXOffset(0.8);

  TCanvas *c21 = new TCanvas("c21","c21",cx1,cy1,1200,950);
  TPad * pad210 = new TPad("pad210","pad210",ps1,ps2,ps4,ps4);
  TPad * pad211 = new TPad("pad211","pad211",ps1,ps1,ps4,ps3);
  pad210->Draw();
  pad211->Draw();
  pad210->cd();
  TText * ct210 = new TText(tll,tlr,Form("%sN to Delta %s Variation of %s Correction Doublet with Octant",regTitle,dataInfo,plotTitle));
  ct210->SetTextSize(tsiz);
  ct210->Draw();
  pad211->cd();
  pad211->Divide(1,3);


  pad211->cd(1);

  TGraphErrors * runCorDoubletOctXGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,X_inCorDoublet,zero,eX_inCorDoublet);
  runCorDoubletOctXGraphUnRegIn1->SetName("runCorDoubletOctXGraphUnRegIn1");
  runCorDoubletOctXGraphUnRegIn1->SetMarkerColor(kRed);
  runCorDoubletOctXGraphUnRegIn1->SetLineColor(kRed);
  runCorDoubletOctXGraphUnRegIn1->SetMarkerStyle(21);
  runCorDoubletOctXGraphUnRegIn1->SetMarkerSize(0.7);
  runCorDoubletOctXGraphUnRegIn1->SetLineWidth(1);
  runCorDoubletOctXGraphUnRegIn1->Fit("fitOctantVariationCorDoubletXIn","E M R F 0 Q");
  fitOctantVariationCorDoubletXIn->SetLineStyle(1);
  fitOctantVariationCorDoubletXIn->SetLineWidth(2);
  fitOctantVariationCorDoubletXIn->SetLineColor(kRed);

  TGraphErrors * runCorDoubletOctXGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,X_outCorDoublet,zero,eX_outCorDoublet);
  runCorDoubletOctXGraphUnRegOut1->SetName("runCorDoubletOctXGraphUnRegOut1");
  runCorDoubletOctXGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorDoubletOctXGraphUnRegOut1->SetLineColor(kBlack);
  runCorDoubletOctXGraphUnRegOut1->SetMarkerStyle(21);
  runCorDoubletOctXGraphUnRegOut1->SetMarkerSize(0.7);
  runCorDoubletOctXGraphUnRegOut1->SetLineWidth(1);
  runCorDoubletOctXGraphUnRegOut1->Fit("fitOctantVariationCorDoubletXOut","E M R F 0 Q");
  fitOctantVariationCorDoubletXOut->SetLineStyle(1);
  fitOctantVariationCorDoubletXOut->SetLineWidth(2);
  fitOctantVariationCorDoubletXOut->SetLineColor(kBlack);

  TMultiGraph *runCorDoubletOctXGraphUnReg1 = new TMultiGraph();
  runCorDoubletOctXGraphUnReg1->Add(runCorDoubletOctXGraphUnRegIn1);
  runCorDoubletOctXGraphUnReg1->Add(runCorDoubletOctXGraphUnRegOut1);
  runCorDoubletOctXGraphUnReg1->Draw("AP");
  runCorDoubletOctXGraphUnReg1->SetTitle("");
  runCorDoubletOctXGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorDoubletOctXGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorDoubletOctXGraphUnReg1->GetYaxis()->SetTitle("X Correction Doublet [ppm]");
  runCorDoubletOctXGraphUnReg1->GetXaxis()->CenterTitle();
  runCorDoubletOctXGraphUnReg1->GetYaxis()->CenterTitle();
  runCorDoubletOctXGraphUnReg1->GetYaxis()->SetRangeUser(-3,6);
  TAxis *xaxisCorDoubletOctXGraphUnReg1= runCorDoubletOctXGraphUnReg1->GetXaxis();
  xaxisCorDoubletOctXGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorDoubletOctXUnRegIn1 =(TPaveStats*)runCorDoubletOctXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorDoubletOctXUnRegOut1 =(TPaveStats*)runCorDoubletOctXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorDoubletOctXUnRegIn1->SetTextColor(kRed);
  statsCorDoubletOctXUnRegIn1->SetFillColor(kWhite); 
  statsCorDoubletOctXUnRegOut1->SetTextColor(kBlack);
  statsCorDoubletOctXUnRegOut1->SetFillColor(kWhite);
  statsCorDoubletOctXUnRegIn1->SetTextSize(0.050);
  statsCorDoubletOctXUnRegOut1->SetTextSize(0.050);
  statsCorDoubletOctXUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorDoubletOctXUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorDoubletOctXUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorDoubletOctXUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorDoubletOctXUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorDoubletOctXUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorDoubletOctXUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorDoubletOctXUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorDoubletOctXUnRegInp0        =   fitOctantVariationCorDoubletXIn->GetParameter(0);
  Double_t calc_CorDoubletOctXUnRegInp1        =   fitOctantVariationCorDoubletXIn->GetParameter(1);
  Double_t calc_CorDoubletOctXUnRegInp2        =   fitOctantVariationCorDoubletXIn->GetParameter(2);
  Double_t calc_eCorDoubletOctXUnRegInp0       =   fitOctantVariationCorDoubletXIn->GetParError(0);
  Double_t calc_eCorDoubletOctXUnRegInp1       =   fitOctantVariationCorDoubletXIn->GetParError(1);
  Double_t calc_eCorDoubletOctXUnRegInp2       =   fitOctantVariationCorDoubletXIn->GetParError(2);
  Double_t calc_CorDoubletOctXUnRegInChisquare =   fitOctantVariationCorDoubletXIn->GetChisquare();
  Double_t calc_CorDoubletOctXUnRegInNDF       =   fitOctantVariationCorDoubletXIn->GetNDF();

  Double_t calc_CorDoubletOctXUnRegOutp0        =   fitOctantVariationCorDoubletXOut->GetParameter(0);
  Double_t calc_CorDoubletOctXUnRegOutp1        =   fitOctantVariationCorDoubletXOut->GetParameter(1);
  Double_t calc_CorDoubletOctXUnRegOutp2        =   fitOctantVariationCorDoubletXOut->GetParameter(2);
  Double_t calc_eCorDoubletOctXUnRegOutp0       =   fitOctantVariationCorDoubletXOut->GetParError(0);
  Double_t calc_eCorDoubletOctXUnRegOutp1       =   fitOctantVariationCorDoubletXOut->GetParError(1);
  Double_t calc_eCorDoubletOctXUnRegOutp2       =   fitOctantVariationCorDoubletXOut->GetParError(2);
  Double_t calc_CorDoubletOctXUnRegOutChisquare =   fitOctantVariationCorDoubletXOut->GetChisquare();
  Double_t calc_CorDoubletOctXUnRegOutNDF       =   fitOctantVariationCorDoubletXOut->GetNDF();

  TLegend *legRunCorDoubletOctXUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorDoubletOctXUnReg1->AddEntry("runCorDoubletOctXGraphUnRegIn1",  Form("In X= %4.2f#pm%4.2f ppm",calc_CorDoubletOctXUnRegInp0,calc_eCorDoubletOctXUnRegInp0),"lp");
  legRunCorDoubletOctXUnReg1->AddEntry("runCorDoubletOctXGraphUnRegOut1",  Form("Out X= %4.2f#pm%4.2f ppm",calc_CorDoubletOctXUnRegOutp0,calc_eCorDoubletOctXUnRegOutp0),"lp");
  legRunCorDoubletOctXUnReg1->SetTextSize(0.050);
  legRunCorDoubletOctXUnReg1->SetFillColor(0);
  legRunCorDoubletOctXUnReg1->SetBorderSize(2);
  legRunCorDoubletOctXUnReg1->Draw();

  fitOctantVariationCorXIn->SetLineStyle(3);
  fitOctantVariationCorXOut->SetLineStyle(3);
  fitOctantVariationCorXSlopeIn->SetLineStyle(2);
  fitOctantVariationCorXSlopeOut->SetLineStyle(2);
  fitOctantVariationCorXIn->Draw("same");
  fitOctantVariationCorXOut->Draw("same");
  fitOctantVariationCorXSlopeIn->Draw("same");
  fitOctantVariationCorXSlopeOut->Draw("same");

  fitOctantVariationCorDoubletXIn->Draw("same");
  fitOctantVariationCorDoubletXOut->Draw("same");

  gPad->Update();

  pad211->cd(2);

  TGraphErrors * runCorDoubletOctYGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,Y_inCorDoublet,zero,eY_inCorDoublet);
  runCorDoubletOctYGraphUnRegIn1->SetName("runCorDoubletOctYGraphUnRegIn1");
  runCorDoubletOctYGraphUnRegIn1->SetMarkerColor(kGreen);
  runCorDoubletOctYGraphUnRegIn1->SetLineColor(kGreen);
  runCorDoubletOctYGraphUnRegIn1->SetMarkerStyle(21);
  runCorDoubletOctYGraphUnRegIn1->SetMarkerSize(0.7);
  runCorDoubletOctYGraphUnRegIn1->SetLineWidth(1);
  runCorDoubletOctYGraphUnRegIn1->Fit("fitOctantVariationCorDoubletYIn","E M R F 0 Q");
  fitOctantVariationCorDoubletYIn->SetLineStyle(1);
  fitOctantVariationCorDoubletYIn->SetLineWidth(2);
  fitOctantVariationCorDoubletYIn->SetLineColor(kGreen);

  TGraphErrors * runCorDoubletOctYGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,Y_outCorDoublet,zero,eY_outCorDoublet);
  runCorDoubletOctYGraphUnRegOut1->SetName("runCorDoubletOctYGraphUnRegOut1");
  runCorDoubletOctYGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorDoubletOctYGraphUnRegOut1->SetLineColor(kBlack);
  runCorDoubletOctYGraphUnRegOut1->SetMarkerStyle(21);
  runCorDoubletOctYGraphUnRegOut1->SetMarkerSize(0.7);
  runCorDoubletOctYGraphUnRegOut1->SetLineWidth(1);
  runCorDoubletOctYGraphUnRegOut1->Fit("fitOctantVariationCorDoubletYOut","E M R F 0 Q");
  fitOctantVariationCorDoubletYOut->SetLineStyle(1);
  fitOctantVariationCorDoubletYOut->SetLineWidth(2);
  fitOctantVariationCorDoubletYOut->SetLineColor(kBlack);

  TMultiGraph *runCorDoubletOctYGraphUnReg1 = new TMultiGraph();
  runCorDoubletOctYGraphUnReg1->Add(runCorDoubletOctYGraphUnRegIn1);
  runCorDoubletOctYGraphUnReg1->Add(runCorDoubletOctYGraphUnRegOut1);
  runCorDoubletOctYGraphUnReg1->Draw("AP");
  runCorDoubletOctYGraphUnReg1->SetTitle("");
  runCorDoubletOctYGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorDoubletOctYGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorDoubletOctYGraphUnReg1->GetYaxis()->SetTitle("Y Correction Doublet [ppm]");
  runCorDoubletOctYGraphUnReg1->GetXaxis()->CenterTitle();
  runCorDoubletOctYGraphUnReg1->GetYaxis()->CenterTitle();
  runCorDoubletOctYGraphUnReg1->GetYaxis()->SetRangeUser(-3.5,2);
  TAxis *xaxisCorDoubletOctYGraphUnReg1= runCorDoubletOctYGraphUnReg1->GetXaxis();
  xaxisCorDoubletOctYGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorDoubletOctYUnRegIn1 =(TPaveStats*)runCorDoubletOctYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorDoubletOctYUnRegOut1 =(TPaveStats*)runCorDoubletOctYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorDoubletOctYUnRegIn1->SetTextColor(kGreen);
  statsCorDoubletOctYUnRegIn1->SetFillColor(kWhite); 
  statsCorDoubletOctYUnRegOut1->SetTextColor(kBlack);
  statsCorDoubletOctYUnRegOut1->SetFillColor(kWhite);
  statsCorDoubletOctYUnRegIn1->SetTextSize(0.050);
  statsCorDoubletOctYUnRegOut1->SetTextSize(0.050);
  statsCorDoubletOctYUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorDoubletOctYUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorDoubletOctYUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorDoubletOctYUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorDoubletOctYUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorDoubletOctYUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorDoubletOctYUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorDoubletOctYUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorDoubletOctYUnRegInp0        =   fitOctantVariationCorDoubletYIn->GetParameter(0);
  Double_t calc_CorDoubletOctYUnRegInp1        =   fitOctantVariationCorDoubletYIn->GetParameter(1);
  Double_t calc_CorDoubletOctYUnRegInp2        =   fitOctantVariationCorDoubletYIn->GetParameter(2);
  Double_t calc_eCorDoubletOctYUnRegInp0       =   fitOctantVariationCorDoubletYIn->GetParError(0);
  Double_t calc_eCorDoubletOctYUnRegInp1       =   fitOctantVariationCorDoubletYIn->GetParError(1);
  Double_t calc_eCorDoubletOctYUnRegInp2       =   fitOctantVariationCorDoubletYIn->GetParError(2);
  Double_t calc_CorDoubletOctYUnRegInChisquare =   fitOctantVariationCorDoubletYIn->GetChisquare();
  Double_t calc_CorDoubletOctYUnRegInNDF       =   fitOctantVariationCorDoubletYIn->GetNDF();

  Double_t calc_CorDoubletOctYUnRegOutp0        =   fitOctantVariationCorDoubletYOut->GetParameter(0);
  Double_t calc_CorDoubletOctYUnRegOutp1        =   fitOctantVariationCorDoubletYOut->GetParameter(1);
  Double_t calc_CorDoubletOctYUnRegOutp2        =   fitOctantVariationCorDoubletYOut->GetParameter(2);
  Double_t calc_eCorDoubletOctYUnRegOutp0       =   fitOctantVariationCorDoubletYOut->GetParError(0);
  Double_t calc_eCorDoubletOctYUnRegOutp1       =   fitOctantVariationCorDoubletYOut->GetParError(1);
  Double_t calc_eCorDoubletOctYUnRegOutp2       =   fitOctantVariationCorDoubletYOut->GetParError(2);
  Double_t calc_CorDoubletOctYUnRegOutChisquare =   fitOctantVariationCorDoubletYOut->GetChisquare();
  Double_t calc_CorDoubletOctYUnRegOutNDF       =   fitOctantVariationCorDoubletYOut->GetNDF();

  TLegend *legRunCorDoubletOctYUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorDoubletOctYUnReg1->AddEntry("runCorDoubletOctYGraphUnRegIn1",  Form("In Y= %4.2f#pm%4.2f ppm",calc_CorDoubletOctYUnRegInp0,calc_eCorDoubletOctYUnRegInp0),"lp");
  legRunCorDoubletOctYUnReg1->AddEntry("runCorDoubletOctYGraphUnRegOut1",  Form("Out Y= %4.2f#pm%4.2f ppm",calc_CorDoubletOctYUnRegOutp0,calc_eCorDoubletOctYUnRegOutp0),"lp");
  legRunCorDoubletOctYUnReg1->SetTextSize(0.050);
  legRunCorDoubletOctYUnReg1->SetFillColor(0);
  legRunCorDoubletOctYUnReg1->SetBorderSize(2);
  legRunCorDoubletOctYUnReg1->Draw();

  fitOctantVariationCorYIn->SetLineStyle(3);
  fitOctantVariationCorYOut->SetLineStyle(3);
  fitOctantVariationCorYSlopeIn->SetLineStyle(2);
  fitOctantVariationCorYSlopeOut->SetLineStyle(2);
  fitOctantVariationCorYIn->Draw("same");
  fitOctantVariationCorYOut->Draw("same");
  fitOctantVariationCorYSlopeIn->Draw("same");
  fitOctantVariationCorYSlopeOut->Draw("same");

  fitOctantVariationCorDoubletYIn->Draw("same");
  fitOctantVariationCorDoubletYOut->Draw("same");

  gPad->Update();

  pad211->cd(3);

  TGraphErrors * runCorOctTotalGraphUnRegIn1 = new TGraphErrors(counter,det_number_in,Total_inCor,zero,eTotal_inCor);
  runCorOctTotalGraphUnRegIn1->SetName("runCorOctTotalGraphUnRegIn1");
  runCorOctTotalGraphUnRegIn1->SetMarkerColor(kCyan+2);
  runCorOctTotalGraphUnRegIn1->SetLineColor(kCyan+2);
  runCorOctTotalGraphUnRegIn1->SetMarkerStyle(21);
  runCorOctTotalGraphUnRegIn1->SetMarkerSize(0.7);
  runCorOctTotalGraphUnRegIn1->SetLineWidth(1);
  runCorOctTotalGraphUnRegIn1->Fit("fitOctantVariationCorTotalIn","E M R F 0 Q");
  fitOctantVariationCorTotalIn->SetLineStyle(1);
  fitOctantVariationCorTotalIn->SetLineWidth(2);
  fitOctantVariationCorTotalIn->SetLineColor(kCyan+2);

  TGraphErrors * runCorOctTotalGraphUnRegOut1 = new TGraphErrors(counter,det_number_out,Total_outCor,zero,eTotal_outCor);
  runCorOctTotalGraphUnRegOut1->SetName("runCorOctTotalGraphUnRegOut1");
  runCorOctTotalGraphUnRegOut1->SetMarkerColor(kBlack);
  runCorOctTotalGraphUnRegOut1->SetLineColor(kBlack);
  runCorOctTotalGraphUnRegOut1->SetMarkerStyle(21);
  runCorOctTotalGraphUnRegOut1->SetMarkerSize(0.7);
  runCorOctTotalGraphUnRegOut1->SetLineWidth(1);
  runCorOctTotalGraphUnRegOut1->Fit("fitOctantVariationCorTotalOut","E M R F 0 Q");
  fitOctantVariationCorTotalOut->SetLineStyle(1);
  fitOctantVariationCorTotalOut->SetLineWidth(2);
  fitOctantVariationCorTotalOut->SetLineColor(kBlack);

  TMultiGraph *runCorOctTotalGraphUnReg1 = new TMultiGraph();
  runCorOctTotalGraphUnReg1->Add(runCorOctTotalGraphUnRegIn1);
  runCorOctTotalGraphUnReg1->Add(runCorOctTotalGraphUnRegOut1);
  runCorOctTotalGraphUnReg1->Draw("AP");
  runCorOctTotalGraphUnReg1->SetTitle("");
  runCorOctTotalGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  runCorOctTotalGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  runCorOctTotalGraphUnReg1->GetYaxis()->SetTitle("Total Correction [ppm]");
  runCorOctTotalGraphUnReg1->GetXaxis()->CenterTitle();
  runCorOctTotalGraphUnReg1->GetYaxis()->CenterTitle();
  TAxis *xaxisCorOctTotalGraphUnReg1= runCorOctTotalGraphUnReg1->GetXaxis();
  xaxisCorOctTotalGraphUnReg1->SetLimits(det_range[0],det_range[1]);

  TPaveStats *statsCorOctTotalUnRegIn1 =(TPaveStats*)runCorOctTotalGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsCorOctTotalUnRegOut1 =(TPaveStats*)runCorOctTotalGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsCorOctTotalUnRegIn1->SetTextColor(kCyan+2);
  statsCorOctTotalUnRegIn1->SetFillColor(kWhite); 
  statsCorOctTotalUnRegOut1->SetTextColor(kBlack);
  statsCorOctTotalUnRegOut1->SetFillColor(kWhite);
  statsCorOctTotalUnRegIn1->SetTextSize(0.050);
  statsCorOctTotalUnRegOut1->SetTextSize(0.050);
  statsCorOctTotalUnRegIn1->SetX1NDC(x_lo_stat_in4);    statsCorOctTotalUnRegIn1->SetX2NDC(x_hi_stat_in4); 
  statsCorOctTotalUnRegIn1->SetY1NDC(y_lo_stat_in4);    statsCorOctTotalUnRegIn1->SetY2NDC(y_hi_stat_in4);
  statsCorOctTotalUnRegOut1->SetX1NDC(x_lo_stat_out4);  statsCorOctTotalUnRegOut1->SetX2NDC(x_hi_stat_out4); 
  statsCorOctTotalUnRegOut1->SetY1NDC(y_lo_stat_out4);  statsCorOctTotalUnRegOut1->SetY2NDC(y_hi_stat_out4);

  Double_t calc_CorOctTotalUnRegInp0        =   fitOctantVariationCorTotalIn->GetParameter(0);
  Double_t calc_CorOctTotalUnRegInp1        =   fitOctantVariationCorTotalIn->GetParameter(1);
  Double_t calc_CorOctTotalUnRegInp2        =   fitOctantVariationCorTotalIn->GetParameter(2);
  Double_t calc_eCorOctTotalUnRegInp0       =   fitOctantVariationCorTotalIn->GetParError(0);
  Double_t calc_eCorOctTotalUnRegInp1       =   fitOctantVariationCorTotalIn->GetParError(1);
  Double_t calc_eCorOctTotalUnRegInp2       =   fitOctantVariationCorTotalIn->GetParError(2);
  Double_t calc_CorOctTotalUnRegInChisquare =   fitOctantVariationCorTotalIn->GetChisquare();
  Double_t calc_CorOctTotalUnRegInNDF       =   fitOctantVariationCorTotalIn->GetNDF();

  Double_t calc_CorOctTotalUnRegOutp0        =   fitOctantVariationCorTotalOut->GetParameter(0);
  Double_t calc_CorOctTotalUnRegOutp1        =   fitOctantVariationCorTotalOut->GetParameter(1);
  Double_t calc_CorOctTotalUnRegOutp2        =   fitOctantVariationCorTotalOut->GetParameter(2);
  Double_t calc_eCorOctTotalUnRegOutp0       =   fitOctantVariationCorTotalOut->GetParError(0);
  Double_t calc_eCorOctTotalUnRegOutp1       =   fitOctantVariationCorTotalOut->GetParError(1);
  Double_t calc_eCorOctTotalUnRegOutp2       =   fitOctantVariationCorTotalOut->GetParError(2);
  Double_t calc_CorOctTotalUnRegOutChisquare =   fitOctantVariationCorTotalOut->GetChisquare();
  Double_t calc_CorOctTotalUnRegOutNDF       =   fitOctantVariationCorTotalOut->GetNDF();

  TLegend *legRunCorOctTotalUnReg1 = new TLegend(x_lo_leg4,y_lo_leg4,x_hi_leg4,y_hi_leg4);
  legRunCorOctTotalUnReg1->AddEntry("runCorOctTotalGraphUnRegIn1",  Form("In Total= %2.2g#pm%2.2g ppm",calc_CorOctTotalUnRegInp0,calc_eCorOctTotalUnRegInp0),"lp");
  legRunCorOctTotalUnReg1->AddEntry("runCorOctTotalGraphUnRegOut1",  Form("Out Total= %2.2g#pm%2.2g ppm",calc_CorOctTotalUnRegOutp0,calc_eCorOctTotalUnRegOutp0),"lp");
  legRunCorOctTotalUnReg1->SetTextSize(0.050);
  legRunCorOctTotalUnReg1->SetFillColor(0);
  legRunCorOctTotalUnReg1->SetBorderSize(2);
  legRunCorOctTotalUnReg1->Draw();

  fitOctantVariationCorTotalIn->Draw("same");
  fitOctantVariationCorTotalOut->Draw("same");

  gPad->Update();

  /*********************************************************************************/
  c21->Update();
  c21->SaveAs(Form("%s/plots/%s/%sOctantCorrectionDoublet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }  
  /*********************************************************************************/
  
  }


  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/


  if(RUNLET_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.6);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  /*********************************************************************************/
  if(RUNLET_PLOT_DB){
  TCanvas *c0 = new TCanvas("c0","c0",cx1,cy1,1200,950);
  TPad * pad00 = new TPad("pad00","pad00",ps1,ps2,ps4,ps4);
  TPad * pad01 = new TPad("pad01","pad01",ps1,ps1,ps4,ps3);
  pad00->Draw();
  pad01->Draw();
  pad00->cd();
  TText * ct00 = new TText(tll,tlr,Form("%sN to Delta %s %s Asym. using Standard DB Regression",regTitle,dataInfo,plotTitle));
  ct00->SetTextSize(tsiz);
  ct00->Draw();
  pad01->cd();
  pad01->Divide(1,2);

  pad01->cd(1);

  TGraphErrors * runGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymCalcUnRegInVal,zeroRegIn,emdAsymCalcUnRegInVal);
  runGraphUnRegIn1->SetName("runGraphUnRegIn1");
  runGraphUnRegIn1->SetMarkerColor(kRed);
  runGraphUnRegIn1->SetLineColor(kRed);
  runGraphUnRegIn1->SetMarkerStyle(25);
  runGraphUnRegIn1->SetMarkerSize(0.5);
  runGraphUnRegIn1->SetLineWidth(1);
//   runGraphUnRegIn1->Draw("AP");
  runGraphUnRegIn1->Fit("fitRunGraphUnRegIn1","E M R F 0 Q");
  fitRunGraphUnRegIn1->SetLineColor(kRed);
  fitRunGraphUnRegIn1->SetLineWidth(2);
  fitRunGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunUnRegIn1            =  fitRunGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunUnRegIn1Error       =  fitRunGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunUnRegIn1Chisquare   =  fitRunGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunUnRegIn1NDF         =  fitRunGraphUnRegIn1->GetNDF();
  Double_t fitResultRunUnRegIn1Prob        =  fitRunGraphUnRegIn1->GetProb();


  TGraphErrors * runGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymCalcUnRegOutVal,zeroRegOut,emdAsymCalcUnRegOutVal);
  runGraphUnRegOut1->SetName("runGraphUnRegOut1");
  runGraphUnRegOut1->SetMarkerColor(kBlack);
  runGraphUnRegOut1->SetLineColor(kBlack);
  runGraphUnRegOut1->SetMarkerStyle(25);
  runGraphUnRegOut1->SetMarkerSize(0.5);
  runGraphUnRegOut1->SetLineWidth(1);
//   runGraphUnRegOut1->Draw("AP");
  runGraphUnRegOut1->Fit("fitRunGraphUnRegOut1","E M R F 0 Q");
  fitRunGraphUnRegOut1->SetLineColor(kBlack);
  fitRunGraphUnRegOut1->SetLineWidth(2);
  fitRunGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunUnRegOut1           =  fitRunGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunUnRegOut1Error      =  fitRunGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunUnRegOut1Chisquare  =  fitRunGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunUnRegOut1NDF        =  fitRunGraphUnRegOut1->GetNDF();
  Double_t fitResultRunUnRegOut1Prob       =  fitRunGraphUnRegOut1->GetProb();

  TMultiGraph *runGraphUnReg1 = new TMultiGraph();
  runGraphUnReg1->Add(runGraphUnRegIn1);
  runGraphUnReg1->Add(runGraphUnRegOut1);
  runGraphUnReg1->Draw("AP");
  runGraphUnReg1->SetTitle("");
  runGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runGraphUnReg1->GetYaxis()->SetTitle(Form("%s Asym UnReg [ppm]",plotTitle));
  runGraphUnReg1->GetXaxis()->CenterTitle();
  runGraphUnReg1->GetYaxis()->CenterTitle();
  runGraphUnReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphUnReg1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisRunGraphUnReg1= runGraphUnReg1->GetXaxis();
  xaxisRunGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphUnRegIn1->Draw("same");
  fitRunGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunUnRegIn1 =(TPaveStats*)runGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunUnRegOut1=(TPaveStats*)runGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunUnRegIn1->SetTextColor(kRed);
  statsRunUnRegIn1->SetFillColor(kWhite); 
  statsRunUnRegOut1->SetTextColor(kBlack);
  statsRunUnRegOut1->SetFillColor(kWhite); 
  statsRunUnRegIn1->SetX1NDC(x_lo_stat_in);    statsRunUnRegIn1->SetX2NDC(x_hi_stat_in); 
  statsRunUnRegIn1->SetY1NDC(y_lo_stat_in);    statsRunUnRegIn1->SetY2NDC(y_hi_stat_in);
  statsRunUnRegOut1->SetX1NDC(x_lo_stat_out);  statsRunUnRegOut1->SetX2NDC(x_hi_stat_out); 
  statsRunUnRegOut1->SetY1NDC(y_lo_stat_out);  statsRunUnRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultRunUnRegInpOut1       =  (fitResultRunUnRegIn1+fitResultRunUnRegOut1)/2;
  Double_t resultRunUnRegInmOut1       =  (1/(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error)*(fitResultRunUnRegIn1)-1/(fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)*(fitResultRunUnRegOut1))/(1/(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error)+1/(fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error));
//   Double_t resultRunUnRegInmOut1       =  (fitResultRunUnRegIn1-fitResultRunUnRegOut1)/2;
  Double_t resultRunUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error+fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)/2;
  Double_t resultRunUnRegInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error)+1/(fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)));
//   Double_t resultRunUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error+fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)/2;


  TLegend *legRunUnReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunUnReg1->AddEntry("runGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunUnRegIn1,fitResultRunUnRegIn1Error),"lp");
  legRunUnReg1->AddEntry("runGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunUnRegOut1,fitResultRunUnRegOut1Error),"lp");
  legRunUnReg1->AddEntry("runGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunUnRegInpOut1,resultRunUnRegInpOut1Error),"");
  legRunUnReg1->AddEntry("runGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunUnRegInmOut1,resultRunUnRegInmOut1Error),"");
  legRunUnReg1->SetTextSize(0.035);
  legRunUnReg1->SetFillColor(0);
  legRunUnReg1->SetBorderSize(2);
  legRunUnReg1->Draw();

  TLatex* tRunGraphUnRegIn1=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunUnRegIn1Chisquare/fitResultRunUnRegIn1NDF,fitResultRunUnRegIn1Prob));
  tRunGraphUnRegIn1->SetTextSize(0.05);
  tRunGraphUnRegIn1->SetTextColor(kRed);
  TLatex* tRunGraphUnRegOut1=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunUnRegOut1Chisquare/fitResultRunUnRegOut1NDF,fitResultRunUnRegOut1Prob));
  tRunGraphUnRegOut1->SetTextSize(0.05);
  tRunGraphUnRegOut1->SetTextColor(kBlack);
  tRunGraphUnRegIn1->Draw();
  tRunGraphUnRegOut1->Draw();

  gPad->Update();


  pad01->cd(2);

  TGraphErrors * runGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymRegInVal,zeroRegIn,emdAsymRegInVal);
  runGraphRegIn1->SetName("runGraphRegIn1");
  runGraphRegIn1->SetMarkerColor(kRed);
  runGraphRegIn1->SetLineColor(kRed);
  runGraphRegIn1->SetMarkerStyle(21);
  runGraphRegIn1->SetMarkerSize(0.5);
  runGraphRegIn1->SetLineWidth(1);
  runGraphRegIn1->Fit("fitRunGraphRegIn1","E M R F 0 Q");
  fitRunGraphRegIn1->SetLineColor(kRed);
  fitRunGraphRegIn1->SetLineWidth(2);
  fitRunGraphRegIn1->SetLineStyle(1);

  Double_t fitResultRunRegIn1            =  fitRunGraphRegIn1->GetParameter(0);
  Double_t fitResultRunRegIn1Error       =  fitRunGraphRegIn1->GetParError(0);
  Double_t fitResultRunRegIn1Chisquare   =  fitRunGraphRegIn1->GetChisquare();
  Double_t fitResultRunRegIn1NDF         =  fitRunGraphRegIn1->GetNDF();
  Double_t fitResultRunRegIn1Prob        =  fitRunGraphRegIn1->GetProb();


  TGraphErrors * runGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymRegOutVal,zeroRegOut,emdAsymRegOutVal);
  runGraphRegOut1->SetName("runGraphRegOut1");
  runGraphRegOut1->SetMarkerColor(kBlack);
  runGraphRegOut1->SetLineColor(kBlack);
  runGraphRegOut1->SetMarkerStyle(21);
  runGraphRegOut1->SetMarkerSize(0.5);
  runGraphRegOut1->SetLineWidth(1);
  runGraphRegOut1->Fit("fitRunGraphRegOut1","E M R F 0 Q");
  fitRunGraphRegOut1->SetLineColor(kBlack);
  fitRunGraphRegOut1->SetLineWidth(2);
  fitRunGraphRegOut1->SetLineStyle(1);

  Double_t fitResultRunRegOut1           =  fitRunGraphRegOut1->GetParameter(0);
  Double_t fitResultRunRegOut1Error      =  fitRunGraphRegOut1->GetParError(0);
  Double_t fitResultRunRegOut1Chisquare  =  fitRunGraphRegOut1->GetChisquare();
  Double_t fitResultRunRegOut1NDF        =  fitRunGraphRegOut1->GetNDF();
  Double_t fitResultRunRegOut1Prob       =  fitRunGraphRegOut1->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn1);
  runGraphReg1->Add(runGraphRegOut1);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn1->Draw("same");
  fitRunGraphRegOut1->Draw("same");

  TPaveStats *statsRunRegIn1 =(TPaveStats*)runGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut1=(TPaveStats*)runGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn1->SetTextColor(kRed);
  statsRunRegIn1->SetFillColor(kWhite); 
  statsRunRegOut1->SetTextColor(kBlack);
  statsRunRegOut1->SetFillColor(kWhite); 
  statsRunRegIn1->SetX1NDC(x_lo_stat_in);    statsRunRegIn1->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn1->SetY1NDC(y_lo_stat_in);    statsRunRegIn1->SetY2NDC(y_hi_stat_in);
  statsRunRegOut1->SetX1NDC(x_lo_stat_out);  statsRunRegOut1->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut1->SetY1NDC(y_lo_stat_out);  statsRunRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut1       =  (fitResultRunRegIn1+fitResultRunRegOut1)/2;
  Double_t resultRunRegInmOut1       =  (1/(fitResultRunRegIn1Error*fitResultRunRegIn1Error)*(fitResultRunRegIn1)-1/(fitResultRunRegOut1Error*fitResultRunRegOut1Error)*(fitResultRunRegOut1))/(1/(fitResultRunRegIn1Error*fitResultRunRegIn1Error)+1/(fitResultRunRegOut1Error*fitResultRunRegOut1Error));
//   Double_t resultRunRegInmOut1       =  (fitResultRunRegIn1-fitResultRunRegOut1)/2;
  Double_t resultRunRegInpOut1Error  =  TMath::Sqrt(fitResultRunRegIn1Error*fitResultRunRegIn1Error+fitResultRunRegOut1Error*fitResultRunRegOut1Error)/2;
  Double_t resultRunRegInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn1Error*fitResultRunRegIn1Error)+1/(fitResultRunRegOut1Error*fitResultRunRegOut1Error)));
//   Double_t resultRunRegInmOut1Error  =  TMath::Sqrt(fitResultRunRegIn1Error*fitResultRunRegIn1Error+fitResultRunRegOut1Error*fitResultRunRegOut1Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn1",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn1,fitResultRunRegIn1Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut1",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut1,fitResultRunRegOut1Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut1,resultRunRegInpOut1Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut1,resultRunRegInmOut1Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn1=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn1Chisquare/fitResultRunRegIn1NDF,fitResultRunRegIn1Prob));
  tRunGraphRegIn1->SetTextSize(0.05);
  tRunGraphRegIn1->SetTextColor(kRed);
  TLatex* tRunGraphRegOut1=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut1Chisquare/fitResultRunRegOut1NDF,fitResultRunRegOut1Prob));
  tRunGraphRegOut1->SetTextSize(0.05);
  tRunGraphRegOut1->SetTextColor(kBlack);
  tRunGraphRegIn1->Draw();
  tRunGraphRegOut1->Draw();

  gPad->Update();

  ofstream outfileUnReg(Form("%s/plots/%s/summary%sRunletUnReg.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot));
  outfileUnReg << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultRunUnRegOut1,fitResultRunUnRegOut1Error,fitResultRunUnRegIn1,fitResultRunUnRegIn1Error) <<endl;
  outfileUnReg.close();
  ofstream outfileReg(Form("%s/plots/%s/summary%sRunlet%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileReg << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultRunRegOut1,fitResultRunRegOut1Error,fitResultRunRegIn1,fitResultRunRegIn1Error) <<endl;
  outfileReg.close();

  c0->Update();
  c0->SaveAs(Form("%s/plots/%s/%sAsymRunletRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /*********************************************************************************/
  /*********************************************************************************/
  if(RUNLET_PLOT_RUNLET_COR){
  TCanvas *c14 = new TCanvas("c14","c14",cx1,cy1,1200,950);
  TPad * pad140 = new TPad("pad140","pad140",ps1,ps2,ps4,ps4);
  TPad * pad141 = new TPad("pad141","pad141",ps1,ps1,ps4,ps3);
  pad140->Draw();
  pad141->Draw();
  pad140->cd();
  TText * ct140 = new TText(tll,tlr,Form("%sN to Delta %s Corrected %s Asym. using Runlet Sensitivity and Differences",regTitle,dataInfo,plotTitle));
  ct140->SetTextSize(tsiz);
  ct140->Draw();
  pad141->cd();
  pad141->Divide(1,2);

  pad141->cd(1);

  TGraphErrors * runGraphRegIn2 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymRegInVal,zeroRegIn,emdAsymRegInVal);
  runGraphRegIn2->SetName("runGraphRegIn2");
  runGraphRegIn2->SetMarkerColor(kRed);
  runGraphRegIn2->SetLineColor(kRed);
  runGraphRegIn2->SetMarkerStyle(21);
  runGraphRegIn2->SetMarkerSize(0.5);
  runGraphRegIn2->SetLineWidth(1);
//   runGraphRegIn2->Draw("AP");
  runGraphRegIn2->Fit("fitRunGraphRegIn2","E M R F 0 Q");
  fitRunGraphRegIn2->SetLineColor(kRed);
  fitRunGraphRegIn2->SetLineWidth(2);
  fitRunGraphRegIn2->SetLineStyle(1);

  Double_t fitResultRunRegIn2            =  fitRunGraphRegIn2->GetParameter(0);
  Double_t fitResultRunRegIn2Error       =  fitRunGraphRegIn2->GetParError(0);
  Double_t fitResultRunRegIn2Chisquare   =  fitRunGraphRegIn2->GetChisquare();
  Double_t fitResultRunRegIn2NDF         =  fitRunGraphRegIn2->GetNDF();
  Double_t fitResultRunRegIn2Prob        =  fitRunGraphRegIn2->GetProb();

  TGraphErrors * runGraphRegOut2 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymRegOutVal,zeroRegOut,emdAsymRegOutVal);
  runGraphRegOut2->SetName("runGraphRegOut2");
  runGraphRegOut2->SetMarkerColor(kBlack);
  runGraphRegOut2->SetLineColor(kBlack);
  runGraphRegOut2->SetMarkerStyle(21);
  runGraphRegOut2->SetMarkerSize(0.5);
  runGraphRegOut2->SetLineWidth(1);
//   runGraphRegOut2->Draw("AP");
  runGraphRegOut2->Fit("fitRunGraphRegOut2","E M R F 0 Q");
  fitRunGraphRegOut2->SetLineColor(kBlack);
  fitRunGraphRegOut2->SetLineWidth(2);
  fitRunGraphRegOut2->SetLineStyle(1);

  Double_t fitResultRunRegOut2           =  fitRunGraphRegOut2->GetParameter(0);
  Double_t fitResultRunRegOut2Error      =  fitRunGraphRegOut2->GetParError(0);
  Double_t fitResultRunRegOut2Chisquare  =  fitRunGraphRegOut2->GetChisquare();
  Double_t fitResultRunRegOut2NDF        =  fitRunGraphRegOut2->GetNDF();
  Double_t fitResultRunRegOut2Prob       =  fitRunGraphRegOut2->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn2);
  runGraphReg1->Add(runGraphRegOut2);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn2->Draw("same");
  fitRunGraphRegOut2->Draw("same");

  TPaveStats *statsRunRegIn2 =(TPaveStats*)runGraphRegIn2->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut2=(TPaveStats*)runGraphRegOut2->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn2->SetTextColor(kRed);
  statsRunRegIn2->SetFillColor(kWhite); 
  statsRunRegOut2->SetTextColor(kBlack);
  statsRunRegOut2->SetFillColor(kWhite); 
  statsRunRegIn2->SetX1NDC(x_lo_stat_in);    statsRunRegIn2->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn2->SetY1NDC(y_lo_stat_in);    statsRunRegIn2->SetY2NDC(y_hi_stat_in);
  statsRunRegOut2->SetX1NDC(x_lo_stat_out);  statsRunRegOut2->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut2->SetY1NDC(y_lo_stat_out);  statsRunRegOut2->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut2       =  (fitResultRunRegIn2+fitResultRunRegOut2)/2;
  Double_t resultRunRegInmOut2       =  (1/(fitResultRunRegIn2Error*fitResultRunRegIn2Error)*(fitResultRunRegIn2)-1/(fitResultRunRegOut2Error*fitResultRunRegOut2Error)*(fitResultRunRegOut2))/(1/(fitResultRunRegIn2Error*fitResultRunRegIn2Error)+1/(fitResultRunRegOut2Error*fitResultRunRegOut2Error));
//   Double_t resultRunRegInmOut2       =  (fitResultRunRegIn2-fitResultRunRegOut2)/2;
  Double_t resultRunRegInpOut2Error  =  TMath::Sqrt(fitResultRunRegIn2Error*fitResultRunRegIn2Error+fitResultRunRegOut2Error*fitResultRunRegOut2Error)/2;
  Double_t resultRunRegInmOut2Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn2Error*fitResultRunRegIn2Error)+1/(fitResultRunRegOut2Error*fitResultRunRegOut2Error)));
//   Double_t resultRunRegInmOut2Error  =  TMath::Sqrt(fitResultRunRegIn2Error*fitResultRunRegIn2Error+fitResultRunRegOut2Error*fitResultRunRegOut2Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn2",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn2,fitResultRunRegIn2Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut2",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut2,fitResultRunRegOut2Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut2",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut2,resultRunRegInpOut2Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut2", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut2,resultRunRegInmOut2Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn2=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn2Chisquare/fitResultRunRegIn2NDF,fitResultRunRegIn2Prob));
  tRunGraphRegIn2->SetTextSize(0.05);
  tRunGraphRegIn2->SetTextColor(kRed);
  TLatex* tRunGraphRegOut2=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut2Chisquare/fitResultRunRegOut2NDF,fitResultRunRegOut2Prob));
  tRunGraphRegOut2->SetTextSize(0.05);
  tRunGraphRegOut2->SetTextColor(kBlack);
  tRunGraphRegIn2->Draw();
  tRunGraphRegOut2->Draw();

  gPad->Update();

  pad141->cd(2);

  TGraphErrors * runGraphCorIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymRunletCorIn,zeroRegIn,emdAsymRunletCorIn);
  runGraphCorIn1->SetName("runGraphCorIn1");
  runGraphCorIn1->SetMarkerColor(kOrange+2);
  runGraphCorIn1->SetLineColor(kOrange+2);
  runGraphCorIn1->SetMarkerStyle(21);
  runGraphCorIn1->SetMarkerSize(0.5);
  runGraphCorIn1->SetLineWidth(1);
  runGraphCorIn1->Fit("fitRunGraphCorIn1","E M R F 0 Q");
  fitRunGraphCorIn1->SetLineColor(kOrange+2);
  fitRunGraphCorIn1->SetLineWidth(2);
  fitRunGraphCorIn1->SetLineStyle(1);

  Double_t fitResultRunCorIn1            =  fitRunGraphCorIn1->GetParameter(0);
  Double_t fitResultRunCorIn1Error       =  fitRunGraphCorIn1->GetParError(0);
  Double_t fitResultRunCorIn1Chisquare   =  fitRunGraphCorIn1->GetChisquare();
  Double_t fitResultRunCorIn1NDF         =  fitRunGraphCorIn1->GetNDF();
  Double_t fitResultRunCorIn1Prob        =  fitRunGraphCorIn1->GetProb();


  TGraphErrors * runGraphCorOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymRunletCorOut,zeroRegOut,emdAsymRunletCorOut);
  runGraphCorOut1->SetName("runGraphCorOut1");
  runGraphCorOut1->SetMarkerColor(kGray+2);
  runGraphCorOut1->SetLineColor(kGray+2);
  runGraphCorOut1->SetMarkerStyle(21);
  runGraphCorOut1->SetMarkerSize(0.5);
  runGraphCorOut1->SetLineWidth(1);
  runGraphCorOut1->Fit("fitRunGraphCorOut1","E M R F 0 Q");
  fitRunGraphCorOut1->SetLineColor(kGray+2);
  fitRunGraphCorOut1->SetLineWidth(2);
  fitRunGraphCorOut1->SetLineStyle(1);

  Double_t fitResultRunCorOut1           =  fitRunGraphCorOut1->GetParameter(0);
  Double_t fitResultRunCorOut1Error      =  fitRunGraphCorOut1->GetParError(0);
  Double_t fitResultRunCorOut1Chisquare  =  fitRunGraphCorOut1->GetChisquare();
  Double_t fitResultRunCorOut1NDF        =  fitRunGraphCorOut1->GetNDF();
  Double_t fitResultRunCorOut1Prob       =  fitRunGraphCorOut1->GetProb();

  TMultiGraph *runGraphCor1 = new TMultiGraph();
  runGraphCor1->Add(runGraphCorIn1);
  runGraphCor1->Add(runGraphCorOut1);
  runGraphCor1->Draw("AP");
  runGraphCor1->SetTitle("");
  runGraphCor1->GetXaxis()->SetTitle("Run Number");
  runGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Runlet Cor [ppm]",plotTitle));
  runGraphCor1->GetXaxis()->CenterTitle();
  runGraphCor1->GetYaxis()->CenterTitle();
  runGraphCor1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphCor1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisRunGraphCor1= runGraphCor1->GetXaxis();
  xaxisRunGraphCor1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphCorIn1->Draw("same");
  fitRunGraphCorOut1->Draw("same");

  TPaveStats *statsRunCorIn1 =(TPaveStats*)runGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunCorOut1=(TPaveStats*)runGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsRunCorIn1->SetTextColor(kOrange+7);
  statsRunCorIn1->SetFillColor(kWhite); 
  statsRunCorOut1->SetTextColor(kGray+2);
  statsRunCorOut1->SetFillColor(kWhite); 
  statsRunCorIn1->SetX1NDC(x_lo_stat_in);    statsRunCorIn1->SetX2NDC(x_hi_stat_in); 
  statsRunCorIn1->SetY1NDC(y_lo_stat_in);    statsRunCorIn1->SetY2NDC(y_hi_stat_in);
  statsRunCorOut1->SetX1NDC(x_lo_stat_out);  statsRunCorOut1->SetX2NDC(x_hi_stat_out); 
  statsRunCorOut1->SetY1NDC(y_lo_stat_out);  statsRunCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultRunCorInpOut1       =  (fitResultRunCorIn1+fitResultRunCorOut1)/2;
  Double_t resultRunCorInmOut1       =  (1/(fitResultRunCorIn1Error*fitResultRunCorIn1Error)*(fitResultRunCorIn1)-1/(fitResultRunCorOut1Error*fitResultRunCorOut1Error)*(fitResultRunCorOut1))/(1/(fitResultRunCorIn1Error*fitResultRunCorIn1Error)+1/(fitResultRunCorOut1Error*fitResultRunCorOut1Error));
//   Double_t resultRunCorInmOut1       =  (fitResultRunCorIn1-fitResultRunCorOut1)/2;
  Double_t resultRunCorInpOut1Error  =  TMath::Sqrt(fitResultRunCorIn1Error*fitResultRunCorIn1Error+fitResultRunCorOut1Error*fitResultRunCorOut1Error)/2;
  Double_t resultRunCorInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunCorIn1Error*fitResultRunCorIn1Error)+1/(fitResultRunCorOut1Error*fitResultRunCorOut1Error)));
//   Double_t resultRunCorInmOut1Error  =  TMath::Sqrt(fitResultRunCorIn1Error*fitResultRunCorIn1Error+fitResultRunCorOut1Error*fitResultRunCorOut1Error)/2;


  TLegend *legRunCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunCor1->AddEntry("runGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultRunCorIn1,fitResultRunCorIn1Error),"lp");
  legRunCor1->AddEntry("runGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultRunCorOut1,fitResultRunCorOut1Error),"lp");
  legRunCor1->AddEntry("runGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultRunCorInpOut1,resultRunCorInpOut1Error),"");
  legRunCor1->AddEntry("runGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultRunCorInmOut1,resultRunCorInmOut1Error),"");
  legRunCor1->SetTextSize(0.035);
  legRunCor1->SetFillColor(0);
  legRunCor1->SetBorderSize(2);
  legRunCor1->Draw();

  TLatex* tRunGraphCorIn1=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunCorIn1Chisquare/fitResultRunCorIn1NDF,fitResultRunCorIn1Prob));
  tRunGraphCorIn1->SetTextSize(0.05);
  tRunGraphCorIn1->SetTextColor(kOrange+2);
  TLatex* tRunGraphCorOut1=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunCorOut1Chisquare/fitResultRunCorOut1NDF,fitResultRunCorOut1Prob));
  tRunGraphCorOut1->SetTextSize(0.05);
  tRunGraphCorOut1->SetTextColor(kGray+2);
  tRunGraphCorIn1->Draw();
  tRunGraphCorOut1->Draw();

  gPad->Update();

  ofstream outfileRunletCor(Form("%s/plots/%s/summary%sRunletCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRunletCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultRunCorOut1,fitResultRunCorOut1Error,fitResultRunCorIn1,fitResultRunCorIn1Error) <<endl;
  outfileRunletCor.close();

  c14->Update();
  c14->SaveAs(Form("%s/plots/%s/%sAsymRunletRunletbyRunletCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }

  /***********************************************************************************/
  if(RUNLET_PLOT_SLUG_COR){
  TCanvas *c10 = new TCanvas("c10","c10",cx1,cy1,1200,950);
  TPad * pad100 = new TPad("pad100","pad100",ps1,ps2,ps4,ps4);
  TPad * pad101 = new TPad("pad101","pad101",ps1,ps1,ps4,ps3);
  pad100->Draw();
  pad101->Draw();
  pad100->cd();
  TText * ct100 = new TText(tll,tlr,Form("%sN to Delta %s Corrected %s Asym. using Slug Avg. Sensitivity and Runlet Differences",regTitle,dataInfo,plotTitle));
  ct100->SetTextSize(tsiz);
  ct100->Draw();
  pad101->cd();
  pad101->Divide(1,2);


  pad101->cd(1);

  TGraphErrors * runGraphRegIn3 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymRegInVal,zeroRegIn,emdAsymRegInVal);
  runGraphRegIn3->SetName("runGraphRegIn3");
  runGraphRegIn3->SetMarkerColor(kRed);
  runGraphRegIn3->SetLineColor(kRed);
  runGraphRegIn3->SetMarkerStyle(21);
  runGraphRegIn3->SetMarkerSize(0.5);
  runGraphRegIn3->SetLineWidth(1);
//   runGraphRegIn3->Draw("AP");
  runGraphRegIn3->Fit("fitRunGraphRegIn3","E M R F 0 Q");
  fitRunGraphRegIn3->SetLineColor(kRed);
  fitRunGraphRegIn3->SetLineWidth(2);
  fitRunGraphRegIn3->SetLineStyle(1);

  Double_t fitResultRunRegIn3            =  fitRunGraphRegIn3->GetParameter(0);
  Double_t fitResultRunRegIn3Error       =  fitRunGraphRegIn3->GetParError(0);
  Double_t fitResultRunRegIn3Chisquare   =  fitRunGraphRegIn3->GetChisquare();
  Double_t fitResultRunRegIn3NDF         =  fitRunGraphRegIn3->GetNDF();
  Double_t fitResultRunRegIn3Prob        =  fitRunGraphRegIn3->GetProb();

  TGraphErrors * runGraphRegOut3 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymRegOutVal,zeroRegOut,emdAsymRegOutVal);
  runGraphRegOut3->SetName("runGraphRegOut3");
  runGraphRegOut3->SetMarkerColor(kBlack);
  runGraphRegOut3->SetLineColor(kBlack);
  runGraphRegOut3->SetMarkerStyle(21);
  runGraphRegOut3->SetMarkerSize(0.5);
  runGraphRegOut3->SetLineWidth(1);
//   runGraphRegOut3->Draw("AP");
  runGraphRegOut3->Fit("fitRunGraphRegOut3","E M R F 0 Q");
  fitRunGraphRegOut3->SetLineColor(kBlack);
  fitRunGraphRegOut3->SetLineWidth(2);
  fitRunGraphRegOut3->SetLineStyle(1);

  Double_t fitResultRunRegOut3           =  fitRunGraphRegOut3->GetParameter(0);
  Double_t fitResultRunRegOut3Error      =  fitRunGraphRegOut3->GetParError(0);
  Double_t fitResultRunRegOut3Chisquare  =  fitRunGraphRegOut3->GetChisquare();
  Double_t fitResultRunRegOut3NDF        =  fitRunGraphRegOut3->GetNDF();
  Double_t fitResultRunRegOut3Prob       =  fitRunGraphRegOut3->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn3);
  runGraphReg1->Add(runGraphRegOut3);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn3->Draw("same");
  fitRunGraphRegOut3->Draw("same");

  TPaveStats *statsRunRegIn3 =(TPaveStats*)runGraphRegIn3->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut3=(TPaveStats*)runGraphRegOut3->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn3->SetTextColor(kRed);
  statsRunRegIn3->SetFillColor(kWhite); 
  statsRunRegOut3->SetTextColor(kBlack);
  statsRunRegOut3->SetFillColor(kWhite); 
  statsRunRegIn3->SetX1NDC(x_lo_stat_in);    statsRunRegIn3->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn3->SetY1NDC(y_lo_stat_in);    statsRunRegIn3->SetY2NDC(y_hi_stat_in);
  statsRunRegOut3->SetX1NDC(x_lo_stat_out);  statsRunRegOut3->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut3->SetY1NDC(y_lo_stat_out);  statsRunRegOut3->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut3       =  (fitResultRunRegIn3+fitResultRunRegOut3)/2;
  Double_t resultRunRegInmOut3       =  (1/(fitResultRunRegIn3Error*fitResultRunRegIn3Error)*(fitResultRunRegIn3)-1/(fitResultRunRegOut3Error*fitResultRunRegOut3Error)*(fitResultRunRegOut3))/(1/(fitResultRunRegIn3Error*fitResultRunRegIn3Error)+1/(fitResultRunRegOut3Error*fitResultRunRegOut3Error));
//   Double_t resultRunRegInmOut3       =  (fitResultRunRegIn3-fitResultRunRegOut3)/2;
  Double_t resultRunRegInpOut3Error  =  TMath::Sqrt(fitResultRunRegIn3Error*fitResultRunRegIn3Error+fitResultRunRegOut3Error*fitResultRunRegOut3Error)/2;
  Double_t resultRunRegInmOut3Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn3Error*fitResultRunRegIn3Error)+1/(fitResultRunRegOut3Error*fitResultRunRegOut3Error)));
//   Double_t resultRunRegInmOut3Error  =  TMath::Sqrt(fitResultRunRegIn3Error*fitResultRunRegIn3Error+fitResultRunRegOut3Error*fitResultRunRegOut3Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn3",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn3,fitResultRunRegIn3Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut3",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut3,fitResultRunRegOut3Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut3",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut3,resultRunRegInpOut3Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut3", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut3,resultRunRegInmOut3Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn3=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn3Chisquare/fitResultRunRegIn3NDF,fitResultRunRegIn3Prob));
  tRunGraphRegIn3->SetTextSize(0.05);
  tRunGraphRegIn3->SetTextColor(kRed);
  TLatex* tRunGraphRegOut3=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut3Chisquare/fitResultRunRegOut3NDF,fitResultRunRegOut3Prob));
  tRunGraphRegOut3->SetTextSize(0.05);
  tRunGraphRegOut3->SetTextColor(kBlack);
  tRunGraphRegIn3->Draw();
  tRunGraphRegOut3->Draw();

  gPad->Update();

  pad101->cd(2);

  TGraphErrors * slugGraphCorIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymSlugCorIn,zeroRegIn,emdAsymSlugCorIn);
  slugGraphCorIn1->SetName("slugGraphCorIn1");
  slugGraphCorIn1->SetMarkerColor(kMagenta);
  slugGraphCorIn1->SetLineColor(kMagenta);
  slugGraphCorIn1->SetMarkerStyle(21);
  slugGraphCorIn1->SetMarkerSize(0.5);
  slugGraphCorIn1->SetLineWidth(1);
  slugGraphCorIn1->Fit("fitSlugGraphCorIn1","E M R F 0 Q");
  fitSlugGraphCorIn1->SetLineColor(kMagenta);
  fitSlugGraphCorIn1->SetLineWidth(2);
  fitSlugGraphCorIn1->SetLineStyle(1);

  Double_t fitResultSlugCorIn1            =  fitSlugGraphCorIn1->GetParameter(0);
  Double_t fitResultSlugCorIn1Error       =  fitSlugGraphCorIn1->GetParError(0);
  Double_t fitResultSlugCorIn1Chisquare   =  fitSlugGraphCorIn1->GetChisquare();
  Double_t fitResultSlugCorIn1NDF         =  fitSlugGraphCorIn1->GetNDF();
  Double_t fitResultSlugCorIn1Prob        =  fitSlugGraphCorIn1->GetProb();


  TGraphErrors * slugGraphCorOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymSlugCorOut,zeroRegOut,emdAsymSlugCorOut);
  slugGraphCorOut1->SetName("slugGraphCorOut1");
  slugGraphCorOut1->SetMarkerColor(kCyan+2);
  slugGraphCorOut1->SetLineColor(kCyan+2);
  slugGraphCorOut1->SetMarkerStyle(21);
  slugGraphCorOut1->SetMarkerSize(0.5);
  slugGraphCorOut1->SetLineWidth(1);
  slugGraphCorOut1->Fit("fitSlugGraphCorOut1","E M R F 0 Q");
  fitSlugGraphCorOut1->SetLineColor(kCyan+2);
  fitSlugGraphCorOut1->SetLineWidth(2);
  fitSlugGraphCorOut1->SetLineStyle(1);

  Double_t fitResultSlugCorOut1           =  fitSlugGraphCorOut1->GetParameter(0);
  Double_t fitResultSlugCorOut1Error      =  fitSlugGraphCorOut1->GetParError(0);
  Double_t fitResultSlugCorOut1Chisquare  =  fitSlugGraphCorOut1->GetChisquare();
  Double_t fitResultSlugCorOut1NDF        =  fitSlugGraphCorOut1->GetNDF();
  Double_t fitResultSlugCorOut1Prob       =  fitSlugGraphCorOut1->GetProb();

  TMultiGraph *slugGraphCor1 = new TMultiGraph();
  slugGraphCor1->Add(slugGraphCorIn1);
  slugGraphCor1->Add(slugGraphCorOut1);
  slugGraphCor1->Draw("AP");
  slugGraphCor1->SetTitle("");
  slugGraphCor1->GetXaxis()->SetTitle("Run Number");
  slugGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Slug Cor [ppm]",plotTitle));
  slugGraphCor1->GetXaxis()->CenterTitle();
  slugGraphCor1->GetYaxis()->CenterTitle();
  slugGraphCor1->GetXaxis()->SetNdivisions(10,2,0);
  slugGraphCor1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisSlugGraphCor1= slugGraphCor1->GetXaxis();
  xaxisSlugGraphCor1->SetLimits(run_range[0],run_range[1]);
  fitSlugGraphCorIn1->Draw("same");
  fitSlugGraphCorOut1->Draw("same");

  TPaveStats *statsSlugCorIn1 =(TPaveStats*)slugGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugCorOut1=(TPaveStats*)slugGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugCorIn1->SetTextColor(kMagenta);
  statsSlugCorIn1->SetFillColor(kWhite); 
  statsSlugCorOut1->SetTextColor(kCyan+2);
  statsSlugCorOut1->SetFillColor(kWhite); 
  statsSlugCorIn1->SetX1NDC(x_lo_stat_in);    statsSlugCorIn1->SetX2NDC(x_hi_stat_in); 
  statsSlugCorIn1->SetY1NDC(y_lo_stat_in);    statsSlugCorIn1->SetY2NDC(y_hi_stat_in);
  statsSlugCorOut1->SetX1NDC(x_lo_stat_out);  statsSlugCorOut1->SetX2NDC(x_hi_stat_out); 
  statsSlugCorOut1->SetY1NDC(y_lo_stat_out);  statsSlugCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultSlugCorInpOut1       =  (fitResultSlugCorIn1+fitResultSlugCorOut1)/2;
  Double_t resultSlugCorInmOut1       =  (1/(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error)*(fitResultSlugCorIn1)-1/(fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)*(fitResultSlugCorOut1))/(1/(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error)+1/(fitResultSlugCorOut1Error*fitResultSlugCorOut1Error));
//   Double_t resultSlugCorInmOut1       =  (fitResultSlugCorIn1-fitResultSlugCorOut1)/2;
  Double_t resultSlugCorInpOut1Error  =  TMath::Sqrt(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error+fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)/2;
  Double_t resultSlugCorInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error)+1/(fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)));
//   Double_t resultSlugCorInmOut1Error  =  TMath::Sqrt(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error+fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)/2;


  TLegend *legSlugCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legSlugCor1->AddEntry("slugGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultSlugCorIn1,fitResultSlugCorIn1Error),"lp");
  legSlugCor1->AddEntry("slugGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultSlugCorOut1,fitResultSlugCorOut1Error),"lp");
  legSlugCor1->AddEntry("slugGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultSlugCorInpOut1,resultSlugCorInpOut1Error),"");
  legSlugCor1->AddEntry("slugGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultSlugCorInmOut1,resultSlugCorInmOut1Error),"");
  legSlugCor1->SetTextSize(0.035);
  legSlugCor1->SetFillColor(0);
  legSlugCor1->SetBorderSize(2);
  legSlugCor1->Draw();

  TLatex* tSlugGraphCorIn1=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugCorIn1Chisquare/fitResultSlugCorIn1NDF,fitResultSlugCorIn1Prob));
  tSlugGraphCorIn1->SetTextSize(0.05);
  tSlugGraphCorIn1->SetTextColor(kMagenta);
  TLatex* tSlugGraphCorOut1=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugCorOut1Chisquare/fitResultSlugCorOut1NDF,fitResultSlugCorOut1Prob));
  tSlugGraphCorOut1->SetTextSize(0.05);
  tSlugGraphCorOut1->SetTextColor(kCyan+2);
  tSlugGraphCorIn1->Draw();
  tSlugGraphCorOut1->Draw();

  gPad->Update();

  ofstream outfileSlugCor(Form("%s/plots/%s/summary%sRunletSlugCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSlugCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultSlugCorOut1,fitResultSlugCorOut1Error,fitResultSlugCorIn1,fitResultSlugCorIn1Error) <<endl;
  outfileSlugCor.close();

  c10->Update();
  c10->SaveAs(Form("%s/plots/%s/%sAsymRunletSlugbySlugCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /***********************************************************************************/
  /***********************************************************************************/
  if(RUNLET_PLOT_SLUGAVG_COR){
  TCanvas *c13 = new TCanvas("c13","c13",cx1,cy1,1200,950);
  TPad * pad130 = new TPad("pad130","pad130",ps1,ps2,ps4,ps4);
  TPad * pad131 = new TPad("pad131","pad131",ps1,ps1,ps4,ps3);
  pad130->Draw();
  pad131->Draw();
  pad130->cd();
  TText * ct130 = new TText(tll,tlr,Form("%sN to Delta %s Corrected %s Asym. using Un-Weighted Slug Avg. Sensitivity and Differences",regTitle,dataInfo,plotTitle));
  ct130->SetTextSize(tsiz);
  ct130->Draw();
  pad131->cd();
  pad131->Divide(1,2);


  pad131->cd(1);

  TGraphErrors * runGraphRegIn4 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymRegInVal,zeroRegIn,emdAsymRegInVal);
  runGraphRegIn4->SetName("runGraphRegIn4");
  runGraphRegIn4->SetMarkerColor(kRed);
  runGraphRegIn4->SetLineColor(kRed);
  runGraphRegIn4->SetMarkerStyle(21);
  runGraphRegIn4->SetMarkerSize(0.5);
  runGraphRegIn4->SetLineWidth(1);
//   runGraphRegIn4->Draw("AP");
  runGraphRegIn4->Fit("fitRunGraphRegIn4","E M R F 0 Q");
  fitRunGraphRegIn4->SetLineColor(kRed);
  fitRunGraphRegIn4->SetLineWidth(2);
  fitRunGraphRegIn4->SetLineStyle(1);

  Double_t fitResultRunRegIn4            =  fitRunGraphRegIn4->GetParameter(0);
  Double_t fitResultRunRegIn4Error       =  fitRunGraphRegIn4->GetParError(0);
  Double_t fitResultRunRegIn4Chisquare   =  fitRunGraphRegIn4->GetChisquare();
  Double_t fitResultRunRegIn4NDF         =  fitRunGraphRegIn4->GetNDF();
  Double_t fitResultRunRegIn4Prob        =  fitRunGraphRegIn4->GetProb();

  TGraphErrors * runGraphRegOut4 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymRegOutVal,zeroRegOut,emdAsymRegOutVal);
  runGraphRegOut4->SetName("runGraphRegOut4");
  runGraphRegOut4->SetMarkerColor(kBlack);
  runGraphRegOut4->SetLineColor(kBlack);
  runGraphRegOut4->SetMarkerStyle(21);
  runGraphRegOut4->SetMarkerSize(0.5);
  runGraphRegOut4->SetLineWidth(1);
//   runGraphRegOut4->Draw("AP");
  runGraphRegOut4->Fit("fitRunGraphRegOut4","E M R F 0 Q");
  fitRunGraphRegOut4->SetLineColor(kBlack);
  fitRunGraphRegOut4->SetLineWidth(2);
  fitRunGraphRegOut4->SetLineStyle(1);

  Double_t fitResultRunRegOut4           =  fitRunGraphRegOut4->GetParameter(0);
  Double_t fitResultRunRegOut4Error      =  fitRunGraphRegOut4->GetParError(0);
  Double_t fitResultRunRegOut4Chisquare  =  fitRunGraphRegOut4->GetChisquare();
  Double_t fitResultRunRegOut4NDF        =  fitRunGraphRegOut4->GetNDF();
  Double_t fitResultRunRegOut4Prob       =  fitRunGraphRegOut4->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn4);
  runGraphReg1->Add(runGraphRegOut4);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn4->Draw("same");
  fitRunGraphRegOut4->Draw("same");

  TPaveStats *statsRunRegIn4 =(TPaveStats*)runGraphRegIn4->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut4=(TPaveStats*)runGraphRegOut4->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn4->SetTextColor(kRed);
  statsRunRegIn4->SetFillColor(kWhite); 
  statsRunRegOut4->SetTextColor(kBlack);
  statsRunRegOut4->SetFillColor(kWhite); 
  statsRunRegIn4->SetX1NDC(x_lo_stat_in);    statsRunRegIn4->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn4->SetY1NDC(y_lo_stat_in);    statsRunRegIn4->SetY2NDC(y_hi_stat_in);
  statsRunRegOut4->SetX1NDC(x_lo_stat_out);  statsRunRegOut4->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut4->SetY1NDC(y_lo_stat_out);  statsRunRegOut4->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut4       =  (fitResultRunRegIn4+fitResultRunRegOut4)/2;
  Double_t resultRunRegInmOut4       =  (1/(fitResultRunRegIn4Error*fitResultRunRegIn4Error)*(fitResultRunRegIn4)-1/(fitResultRunRegOut4Error*fitResultRunRegOut4Error)*(fitResultRunRegOut4))/(1/(fitResultRunRegIn4Error*fitResultRunRegIn4Error)+1/(fitResultRunRegOut4Error*fitResultRunRegOut4Error));
//   Double_t resultRunRegInmOut4       =  (fitResultRunRegIn4-fitResultRunRegOut4)/2;
  Double_t resultRunRegInpOut4Error  =  TMath::Sqrt(fitResultRunRegIn4Error*fitResultRunRegIn4Error+fitResultRunRegOut4Error*fitResultRunRegOut4Error)/2;
  Double_t resultRunRegInmOut4Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn4Error*fitResultRunRegIn4Error)+1/(fitResultRunRegOut4Error*fitResultRunRegOut4Error)));
//   Double_t resultRunRegInmOut4Error  =  TMath::Sqrt(fitResultRunRegIn4Error*fitResultRunRegIn4Error+fitResultRunRegOut4Error*fitResultRunRegOut4Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn4",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn4,fitResultRunRegIn4Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut4",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut4,fitResultRunRegOut4Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut4",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut4,resultRunRegInpOut4Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut4", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut4,resultRunRegInmOut4Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn4=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn4Chisquare/fitResultRunRegIn4NDF,fitResultRunRegIn4Prob));
  tRunGraphRegIn4->SetTextSize(0.05);
  tRunGraphRegIn4->SetTextColor(kRed);
  TLatex* tRunGraphRegOut4=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut4Chisquare/fitResultRunRegOut4NDF,fitResultRunRegOut4Prob));
  tRunGraphRegOut4->SetTextSize(0.05);
  tRunGraphRegOut4->SetTextColor(kBlack);
  tRunGraphRegIn4->Draw();
  tRunGraphRegOut4->Draw();

  gPad->Update();


  pad131->cd(2);

  TGraphErrors * slugAvgGraphCorIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymSlugAvgCorIn,zeroRegIn,emdAsymSlugAvgCorIn);
  slugAvgGraphCorIn1->SetName("slugAvgGraphCorIn1");
  slugAvgGraphCorIn1->SetMarkerColor(kGreen);
  slugAvgGraphCorIn1->SetLineColor(kGreen);
  slugAvgGraphCorIn1->SetMarkerStyle(21);
  slugAvgGraphCorIn1->SetMarkerSize(0.5);
  slugAvgGraphCorIn1->SetLineWidth(1);
  slugAvgGraphCorIn1->Fit("fitSlugAvgGraphCorIn1","E M R F 0 Q");
  fitSlugAvgGraphCorIn1->SetLineColor(kGreen);
  fitSlugAvgGraphCorIn1->SetLineWidth(2);
  fitSlugAvgGraphCorIn1->SetLineStyle(1);

  Double_t fitResultSlugAvgCorIn1            =  fitSlugAvgGraphCorIn1->GetParameter(0);
  Double_t fitResultSlugAvgCorIn1Error       =  fitSlugAvgGraphCorIn1->GetParError(0);
  Double_t fitResultSlugAvgCorIn1Chisquare   =  fitSlugAvgGraphCorIn1->GetChisquare();
  Double_t fitResultSlugAvgCorIn1NDF         =  fitSlugAvgGraphCorIn1->GetNDF();
  Double_t fitResultSlugAvgCorIn1Prob        =  fitSlugAvgGraphCorIn1->GetProb();


  TGraphErrors * slugAvgGraphCorOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymSlugAvgCorOut,zeroRegOut,emdAsymSlugAvgCorOut);
  slugAvgGraphCorOut1->SetName("slugAvgGraphCorOut1");
  slugAvgGraphCorOut1->SetMarkerColor(kBlue-7);
  slugAvgGraphCorOut1->SetLineColor(kBlue-7);
  slugAvgGraphCorOut1->SetMarkerStyle(21);
  slugAvgGraphCorOut1->SetMarkerSize(0.5);
  slugAvgGraphCorOut1->SetLineWidth(1);
  slugAvgGraphCorOut1->Fit("fitSlugAvgGraphCorOut1","E M R F 0 Q");
  fitSlugAvgGraphCorOut1->SetLineColor(kBlue-7);
  fitSlugAvgGraphCorOut1->SetLineWidth(2);
  fitSlugAvgGraphCorOut1->SetLineStyle(1);

  Double_t fitResultSlugAvgCorOut1           =  fitSlugAvgGraphCorOut1->GetParameter(0);
  Double_t fitResultSlugAvgCorOut1Error      =  fitSlugAvgGraphCorOut1->GetParError(0);
  Double_t fitResultSlugAvgCorOut1Chisquare  =  fitSlugAvgGraphCorOut1->GetChisquare();
  Double_t fitResultSlugAvgCorOut1NDF        =  fitSlugAvgGraphCorOut1->GetNDF();
  Double_t fitResultSlugAvgCorOut1Prob       =  fitSlugAvgGraphCorOut1->GetProb();

  TMultiGraph *slugAvgGraphCor1 = new TMultiGraph();
  slugAvgGraphCor1->Add(slugAvgGraphCorIn1);
  slugAvgGraphCor1->Add(slugAvgGraphCorOut1);
  slugAvgGraphCor1->Draw("AP");
  slugAvgGraphCor1->SetTitle("");
  slugAvgGraphCor1->GetXaxis()->SetTitle("Run Number");
  slugAvgGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Slug Avg Cor [ppm]",plotTitle));
  slugAvgGraphCor1->GetXaxis()->CenterTitle();
  slugAvgGraphCor1->GetYaxis()->CenterTitle();
  slugAvgGraphCor1->GetXaxis()->SetNdivisions(10,2,0);
  slugAvgGraphCor1->GetYaxis()->SetRangeUser(y_scale[0],y_scale[1]);
  TAxis *xaxisSlugAvgGraphCor1= slugAvgGraphCor1->GetXaxis();
  xaxisSlugAvgGraphCor1->SetLimits(run_range[0],run_range[1]);
  fitSlugAvgGraphCorIn1->Draw("same");
  fitSlugAvgGraphCorOut1->Draw("same");

  TPaveStats *statsSlugAvgCorIn1 =(TPaveStats*)slugAvgGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugAvgCorOut1=(TPaveStats*)slugAvgGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugAvgCorIn1->SetTextColor(kGreen);
  statsSlugAvgCorIn1->SetFillColor(kWhite); 
  statsSlugAvgCorOut1->SetTextColor(kBlue-7);
  statsSlugAvgCorOut1->SetFillColor(kWhite); 
  statsSlugAvgCorIn1->SetX1NDC(x_lo_stat_in);    statsSlugAvgCorIn1->SetX2NDC(x_hi_stat_in); 
  statsSlugAvgCorIn1->SetY1NDC(y_lo_stat_in);    statsSlugAvgCorIn1->SetY2NDC(y_hi_stat_in);
  statsSlugAvgCorOut1->SetX1NDC(x_lo_stat_out);  statsSlugAvgCorOut1->SetX2NDC(x_hi_stat_out); 
  statsSlugAvgCorOut1->SetY1NDC(y_lo_stat_out);  statsSlugAvgCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultSlugAvgCorInpOut1       =  (fitResultSlugAvgCorIn1+fitResultSlugAvgCorOut1)/2;
  Double_t resultSlugAvgCorInmOut1       =  (((1/(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error))*(fitResultSlugAvgCorIn1))-((1/(fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error))*(fitResultSlugAvgCorOut1)))/((1/(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error))+(1/(fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error)));
//   Double_t resultSlugAvgCorInmOut1       =  (fitResultSlugAvgCorIn1-fitResultSlugAvgCorOut1)/2;
  Double_t resultSlugAvgCorInpOut1Error  =  TMath::Sqrt(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error+fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error)/2;
  Double_t resultSlugAvgCorInmOut1Error  = 1/(TMath::Sqrt((1/(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error))+(1/(fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error))));
//   Double_t resultSlugAvgCorInmOut1Error  =  TMath::Sqrt(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error+fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error)/2;


  TLegend *legSlugAvgCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legSlugAvgCor1->AddEntry("slugAvgGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultSlugAvgCorIn1,fitResultSlugAvgCorIn1Error),"lp");
  legSlugAvgCor1->AddEntry("slugAvgGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultSlugAvgCorOut1,fitResultSlugAvgCorOut1Error),"lp");
  legSlugAvgCor1->AddEntry("slugAvgGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultSlugAvgCorInpOut1,resultSlugAvgCorInpOut1Error),"");
  legSlugAvgCor1->AddEntry("slugAvgGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultSlugAvgCorInmOut1,resultSlugAvgCorInmOut1Error),"");
  legSlugAvgCor1->SetTextSize(0.035);
  legSlugAvgCor1->SetFillColor(0);
  legSlugAvgCor1->SetBorderSize(2);
  legSlugAvgCor1->Draw();

  TLatex* tSlugAvgGraphCorIn1=new TLatex(11496,y_scale[1]-y_scale_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugAvgCorIn1Chisquare/fitResultSlugAvgCorIn1NDF,fitResultSlugAvgCorIn1Prob));
  tSlugAvgGraphCorIn1->SetTextSize(0.05);
  tSlugAvgGraphCorIn1->SetTextColor(kGreen);
  TLatex* tSlugAvgGraphCorOut1=new TLatex(11496,y_scale[1]-y_scale_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugAvgCorOut1Chisquare/fitResultSlugAvgCorOut1NDF,fitResultSlugAvgCorOut1Prob));
  tSlugAvgGraphCorOut1->SetTextSize(0.05);
  tSlugAvgGraphCorOut1->SetTextColor(kBlue-7);
  tSlugAvgGraphCorIn1->Draw();
  tSlugAvgGraphCorOut1->Draw();

  gPad->Update();

  ofstream outfileSlugAvgCor(Form("%s/plots/%s/summary%sRunletSlugAvgCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSlugAvgCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultSlugAvgCorOut1,fitResultSlugAvgCorOut1Error,fitResultSlugAvgCorIn1,fitResultSlugAvgCorIn1Error) <<endl;
  outfileSlugAvgCor.close();

  c13->Update();
  c13->SaveAs(Form("%s/plots/%s/%sAsymRunletSlugAvgbySlugAvgCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }

  /***********************************************************************************/
  /*********************************************************************************/
  if(RUNLET_PLOT_WIDTH){
  TCanvas *c16 = new TCanvas("c16","c16",cx1,cy1,1200,950);
  TPad * pad160 = new TPad("pad160","pad160",ps1,ps2,ps4,ps4);
  TPad * pad161 = new TPad("pad161","pad161",ps1,ps1,ps4,ps3);
  pad160->Draw();
  pad161->Draw();
  pad160->cd();
  TText * ct160 = new TText(tll,tlr,Form("%sN to Delta %s Runlet-by-Runlet %s Asym. Width",regTitle,dataInfo,plotTitle));
  ct160->SetTextSize(tsiz);
  ct160->Draw();
  pad161->cd();
  pad161->Divide(1,2);

  pad161->cd(1);

  TGraphErrors * runGraphWUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymWCalcUnRegInVal,zeroRegIn,zeroRegIn);
  runGraphWUnRegIn1->SetName("runGraphWUnRegIn1");
  runGraphWUnRegIn1->SetMarkerColor(kRed);
  runGraphWUnRegIn1->SetLineColor(kRed);
  runGraphWUnRegIn1->SetMarkerStyle(25);
  runGraphWUnRegIn1->SetMarkerSize(0.5);
  runGraphWUnRegIn1->SetLineWidth(1);
//   runGraphWUnRegIn1->Draw("AP");
  runGraphWUnRegIn1->Fit("fitWUnRegGraphIn1","E M R F 0 Q");
  fitWUnRegGraphIn1->SetLineColor(kRed);
  fitWUnRegGraphIn1->SetLineWidth(2);
  fitWUnRegGraphIn1->SetLineStyle(3);

  Double_t fitResultWUnRegIn1            =  fitWUnRegGraphIn1->GetParameter(0);
  Double_t fitResultWUnRegIn1Error       =  fitWUnRegGraphIn1->GetParError(0);
  Double_t fitResultWUnRegIn1Chisquare   =  fitWUnRegGraphIn1->GetChisquare();
  Double_t fitResultWUnRegIn1NDF         =  fitWUnRegGraphIn1->GetNDF();
  Double_t fitResultWUnRegIn1Prob        =  fitWUnRegGraphIn1->GetProb();

  TGraphErrors * runGraphWUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymWCalcUnRegOutVal,zeroRegOut,zeroRegOut);
  runGraphWUnRegOut1->SetName("runGraphWUnRegOut1");
  runGraphWUnRegOut1->SetMarkerColor(kGray+2);
  runGraphWUnRegOut1->SetLineColor(kGray+2);
  runGraphWUnRegOut1->SetMarkerStyle(25);
  runGraphWUnRegOut1->SetMarkerSize(0.5);
  runGraphWUnRegOut1->SetLineWidth(1);
//   runGraphWUnRegOut1->Draw("AP");
  runGraphWUnRegOut1->Fit("fitWUnRegGraphOut1","E M R F 0 Q");
  fitWUnRegGraphOut1->SetLineColor(kGray+2);
  fitWUnRegGraphOut1->SetLineWidth(2);
  fitWUnRegGraphOut1->SetLineStyle(3);

  Double_t fitResultWUnRegOut1            =  fitWUnRegGraphOut1->GetParameter(0);
  Double_t fitResultWUnRegOut1Error       =  fitWUnRegGraphOut1->GetParError(0);
  Double_t fitResultWUnRegOut1Chisquare   =  fitWUnRegGraphOut1->GetChisquare();
  Double_t fitResultWUnRegOut1NDF         =  fitWUnRegGraphOut1->GetNDF();
  Double_t fitResultWUnRegOut1Prob        =  fitWUnRegGraphOut1->GetProb();

  TGraphErrors * runGraphWRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymWRegInVal,zeroRegIn,zeroRegIn);
  runGraphWRegIn1->SetName("runGraphWRegIn1");
  runGraphWRegIn1->SetMarkerColor(kRed+2);
  runGraphWRegIn1->SetLineColor(kRed+2);
  runGraphWRegIn1->SetMarkerStyle(21);
  runGraphWRegIn1->SetMarkerSize(0.5);
  runGraphWRegIn1->SetLineWidth(1);
//   runGraphWRegIn1->Draw("AP");
  runGraphWRegIn1->Fit("fitWRegGraphIn1","E M R F 0 Q");
  fitWRegGraphIn1->SetLineColor(kRed+2);
  fitWRegGraphIn1->SetLineWidth(2);
  fitWRegGraphIn1->SetLineStyle(1);

  Double_t fitResultWRegIn1            =  fitWRegGraphIn1->GetParameter(0);
  Double_t fitResultWRegIn1Error       =  fitWRegGraphIn1->GetParError(0);
  Double_t fitResultWRegIn1Chisquare   =  fitWRegGraphIn1->GetChisquare();
  Double_t fitResultWRegIn1NDF         =  fitWRegGraphIn1->GetNDF();
  Double_t fitResultWRegIn1Prob        =  fitWRegGraphIn1->GetProb();

  TGraphErrors * runGraphWRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymWRegOutVal,zeroRegOut,zeroRegOut);
  runGraphWRegOut1->SetName("runGraphWRegOut1");
  runGraphWRegOut1->SetMarkerColor(kBlack);
  runGraphWRegOut1->SetLineColor(kBlack);
  runGraphWRegOut1->SetMarkerStyle(21);
  runGraphWRegOut1->SetMarkerSize(0.5);
  runGraphWRegOut1->SetLineWidth(1);
//   runGraphWRegOut1->Draw("AP");
  runGraphWRegOut1->Fit("fitWRegGraphOut1","E M R F 0 Q");
  fitWRegGraphOut1->SetLineColor(kBlack);
  fitWRegGraphOut1->SetLineWidth(2);
  fitWRegGraphOut1->SetLineStyle(1);

  Double_t fitResultWRegOut1            =  fitWRegGraphOut1->GetParameter(0);
  Double_t fitResultWRegOut1Error       =  fitWRegGraphOut1->GetParError(0);
  Double_t fitResultWRegOut1Chisquare   =  fitWRegGraphOut1->GetChisquare();
  Double_t fitResultWRegOut1NDF         =  fitWRegGraphOut1->GetNDF();
  Double_t fitResultWRegOut1Prob        =  fitWRegGraphOut1->GetProb();


  TMultiGraph *runGraphW1 = new TMultiGraph();
  runGraphW1->Add(runGraphWUnRegIn1);
  runGraphW1->Add(runGraphWUnRegOut1);
  runGraphW1->Add(runGraphWRegIn1);
  runGraphW1->Add(runGraphWRegOut1);
  runGraphW1->Draw("AP");
  runGraphW1->SetTitle("");
  runGraphW1->GetXaxis()->SetTitle("Run Number");
  runGraphW1->GetYaxis()->SetTitle(Form("%s Asym Width [ppm]",plotTitle));
  runGraphW1->GetXaxis()->CenterTitle();
  runGraphW1->GetYaxis()->CenterTitle();
  runGraphW1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphW1->GetYaxis()->SetRangeUser(y_scaleW[0],y_scaleW[1]);
  TAxis *xaxisRunGraphW1= runGraphW1->GetXaxis();
  xaxisRunGraphW1->SetLimits(run_range[0],run_range[1]);
  fitWUnRegGraphIn1->Draw("same");
  fitWUnRegGraphOut1->Draw("same");
  fitWRegGraphIn1->Draw("same");
  fitWRegGraphOut1->Draw("same");

  TPaveStats *statsWUnRegIn1 =(TPaveStats*)runGraphWUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWUnRegOut1=(TPaveStats*)runGraphWUnRegOut1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWRegIn1 =(TPaveStats*)runGraphWRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWRegOut1=(TPaveStats*)runGraphWRegOut1->GetListOfFunctions()->FindObject("stats");
  statsWUnRegIn1->SetTextColor(kRed);
  statsWUnRegIn1->SetFillColor(kWhite); 
  statsWUnRegOut1->SetTextColor(kGray+2);
  statsWUnRegOut1->SetFillColor(kWhite); 
  statsWRegIn1->SetTextColor(kRed+2);
  statsWRegIn1->SetFillColor(kWhite); 
  statsWRegOut1->SetTextColor(kBlack);
  statsWRegOut1->SetFillColor(kWhite); 
  statsWUnRegIn1->SetX1NDC(0.80);    statsWUnRegIn1->SetX2NDC(0.99); 
  statsWUnRegIn1->SetY1NDC(0.80);    statsWUnRegIn1->SetY2NDC(0.99);
  statsWUnRegOut1->SetX1NDC(0.80);  statsWUnRegOut1->SetX2NDC(0.99); 
  statsWUnRegOut1->SetY1NDC(0.60);  statsWUnRegOut1->SetY2NDC(0.79);
  statsWRegIn1->SetX1NDC(0.80);    statsWRegIn1->SetX2NDC(0.99); 
  statsWRegIn1->SetY1NDC(0.40);    statsWRegIn1->SetY2NDC(0.59);
  statsWRegOut1->SetX1NDC(0.80);  statsWRegOut1->SetX2NDC(0.99); 
  statsWRegOut1->SetY1NDC(0.20);  statsWRegOut1->SetY2NDC(0.39);

  TLegend *legRunW1 = new TLegend(0.80,0.01,0.99,0.19);
  legRunW1->AddEntry("runGraphWUnRegIn1",  Form("In UnReg=%4.1f ppm",fitResultWUnRegIn1),"lp");
  legRunW1->AddEntry("runGraphWUnRegOut1",  Form("Out UnReg=%4.1f ppm",fitResultWUnRegOut1),"lp");
  legRunW1->AddEntry("runGraphWRegIn1",  Form("In Reg=%4.1f ppm",fitResultWRegIn1),"lp");
  legRunW1->AddEntry("runGraphWRegOut1",  Form("Out Reg=%4.1f ppm",fitResultWRegOut1),"lp");
  legRunW1->SetTextSize(0.035);
  legRunW1->SetFillColor(0);
  legRunW1->SetBorderSize(2);
  legRunW1->Draw();

  TLatex* trunGraphWUnRegIn1=new TLatex(11496,y_scaleW[1]-y_scaleW_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWUnRegIn1Chisquare/fitResultWUnRegIn1NDF,fitResultWUnRegIn1Prob));
  trunGraphWUnRegIn1->SetTextSize(0.05);
  trunGraphWUnRegIn1->SetTextColor(kRed);
  TLatex* trunGraphWUnRegOut1=new TLatex(11496,y_scaleW[1]-y_scaleW_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWUnRegOut1Chisquare/fitResultWUnRegOut1NDF,fitResultWUnRegOut1Prob));
  trunGraphWUnRegOut1->SetTextSize(0.05);
  trunGraphWUnRegOut1->SetTextColor(kGray+2);
  TLatex* trunGraphWRegIn1=new TLatex(11496,y_scaleW[0]+y_scaleW_off[1],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWRegIn1Chisquare/fitResultWRegIn1NDF,fitResultWRegIn1Prob));
  trunGraphWRegIn1->SetTextSize(0.05);
  trunGraphWRegIn1->SetTextColor(kRed+2);
  TLatex* trunGraphWRegOut1=new TLatex(11496,y_scaleW[0]+y_scaleW_off[0],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWRegOut1Chisquare/fitResultWRegOut1NDF,fitResultWRegOut1Prob));
  trunGraphWRegOut1->SetTextSize(0.05);
  trunGraphWRegOut1->SetTextColor(kBlack);
  trunGraphWUnRegIn1->Draw();
  trunGraphWUnRegOut1->Draw();
  trunGraphWRegIn1->Draw();
  trunGraphWRegOut1->Draw();

  gPad->Update();


  pad161->cd(2);

  TGraphErrors * runGraphWNormUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymWCalcNormUnRegInVal,zeroRegIn,zeroRegIn);
  runGraphWNormUnRegIn1->SetName("runGraphWNormUnRegIn1");
  runGraphWNormUnRegIn1->SetMarkerColor(kOrange);
  runGraphWNormUnRegIn1->SetLineColor(kOrange);
  runGraphWNormUnRegIn1->SetMarkerStyle(25);
  runGraphWNormUnRegIn1->SetMarkerSize(0.5);
  runGraphWNormUnRegIn1->SetLineWidth(1);
//   runGraphWNormUnRegIn1->Draw("AP");
  runGraphWNormUnRegIn1->Fit("fitWNormUnRegGraphIn1","E M R F 0 Q");
  fitWNormUnRegGraphIn1->SetLineColor(kOrange);
  fitWNormUnRegGraphIn1->SetLineWidth(2);
  fitWNormUnRegGraphIn1->SetLineStyle(3);

  Double_t fitResultWNormUnRegIn1            =  fitWNormUnRegGraphIn1->GetParameter(0);
  Double_t fitResultWNormUnRegIn1Error       =  fitWNormUnRegGraphIn1->GetParError(0);
  Double_t fitResultWNormUnRegIn1Chisquare   =  fitWNormUnRegGraphIn1->GetChisquare();
  Double_t fitResultWNormUnRegIn1NDF         =  fitWNormUnRegGraphIn1->GetNDF();
  Double_t fitResultWNormUnRegIn1Prob        =  fitWNormUnRegGraphIn1->GetProb();

  TGraphErrors * runGraphWNormUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymWCalcNormUnRegOutVal,zeroRegOut,zeroRegOut);
  runGraphWNormUnRegOut1->SetName("runGraphWNormUnRegOut1");
  runGraphWNormUnRegOut1->SetMarkerColor(kGray+2);
  runGraphWNormUnRegOut1->SetLineColor(kGray+2);
  runGraphWNormUnRegOut1->SetMarkerStyle(25);
  runGraphWNormUnRegOut1->SetMarkerSize(0.5);
  runGraphWNormUnRegOut1->SetLineWidth(1);
//   runGraphWNormUnRegOut1->Draw("AP");
  runGraphWNormUnRegOut1->Fit("fitWNormUnRegGraphOut1","E M R F 0 Q");
  fitWNormUnRegGraphOut1->SetLineColor(kGray+2);
  fitWNormUnRegGraphOut1->SetLineWidth(2);
  fitWNormUnRegGraphOut1->SetLineStyle(3);

  Double_t fitResultWNormUnRegOut1            =  fitWNormUnRegGraphOut1->GetParameter(0);
  Double_t fitResultWNormUnRegOut1Error       =  fitWNormUnRegGraphOut1->GetParError(0);
  Double_t fitResultWNormUnRegOut1Chisquare   =  fitWNormUnRegGraphOut1->GetChisquare();
  Double_t fitResultWNormUnRegOut1NDF         =  fitWNormUnRegGraphOut1->GetNDF();
  Double_t fitResultWNormUnRegOut1Prob        =  fitWNormUnRegGraphOut1->GetProb();

  TGraphErrors * runGraphWNormRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymWNormRegInVal,zeroRegIn,zeroRegIn);
  runGraphWNormRegIn1->SetName("runGraphWNormRegIn1");
  runGraphWNormRegIn1->SetMarkerColor(kOrange+7);
  runGraphWNormRegIn1->SetLineColor(kOrange+7);
  runGraphWNormRegIn1->SetMarkerStyle(21);
  runGraphWNormRegIn1->SetMarkerSize(0.5);
  runGraphWNormRegIn1->SetLineWidth(1);
//   runGraphWNormRegIn1->Draw("AP");
  runGraphWNormRegIn1->Fit("fitWNormRegGraphIn1","E M R F 0 Q");
  fitWNormRegGraphIn1->SetLineColor(kOrange+7);
  fitWNormRegGraphIn1->SetLineWidth(2);
  fitWNormRegGraphIn1->SetLineStyle(1);

  Double_t fitResultWNormRegIn1            =  fitWNormRegGraphIn1->GetParameter(0);
  Double_t fitResultWNormRegIn1Error       =  fitWNormRegGraphIn1->GetParError(0);
  Double_t fitResultWNormRegIn1Chisquare   =  fitWNormRegGraphIn1->GetChisquare();
  Double_t fitResultWNormRegIn1NDF         =  fitWNormRegGraphIn1->GetNDF();
  Double_t fitResultWNormRegIn1Prob        =  fitWNormRegGraphIn1->GetProb();

  TGraphErrors * runGraphWNormRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymWNormRegOutVal,zeroRegOut,zeroRegOut);
  runGraphWNormRegOut1->SetName("runGraphWNormRegOut1");
  runGraphWNormRegOut1->SetMarkerColor(kBlack);
  runGraphWNormRegOut1->SetLineColor(kBlack);
  runGraphWNormRegOut1->SetMarkerStyle(21);
  runGraphWNormRegOut1->SetMarkerSize(0.5);
  runGraphWNormRegOut1->SetLineWidth(1);
//   runGraphWNormRegOut1->Draw("AP");
  runGraphWNormRegOut1->Fit("fitWNormRegGraphOut1","E M R F 0 Q");
  fitWNormRegGraphOut1->SetLineColor(kBlack);
  fitWNormRegGraphOut1->SetLineWidth(2);
  fitWNormRegGraphOut1->SetLineStyle(1);

  Double_t fitResultWNormRegOut1            =  fitWNormRegGraphOut1->GetParameter(0);
  Double_t fitResultWNormRegOut1Error       =  fitWNormRegGraphOut1->GetParError(0);
  Double_t fitResultWNormRegOut1Chisquare   =  fitWNormRegGraphOut1->GetChisquare();
  Double_t fitResultWNormRegOut1NDF         =  fitWNormRegGraphOut1->GetNDF();
  Double_t fitResultWNormRegOut1Prob        =  fitWNormRegGraphOut1->GetProb();


  TMultiGraph *runGraphWNorm1 = new TMultiGraph();
  runGraphWNorm1->Add(runGraphWNormUnRegIn1);
  runGraphWNorm1->Add(runGraphWNormUnRegOut1);
  runGraphWNorm1->Add(runGraphWNormRegIn1);
  runGraphWNorm1->Add(runGraphWNormRegOut1);
  runGraphWNorm1->Draw("AP");
  runGraphWNorm1->SetTitle("");
  runGraphWNorm1->GetXaxis()->SetTitle("Run Number");
  runGraphWNorm1->GetYaxis()->SetTitle(Form("MD Asym Norm Width [ppm]",plotTitle));
  runGraphWNorm1->GetXaxis()->CenterTitle();
  runGraphWNorm1->GetYaxis()->CenterTitle();
  runGraphWNorm1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphWNorm1->GetYaxis()->SetRangeUser(y_scaleW[0],y_scaleW[1]);
  TAxis *xaxisRunGraphWNorm1= runGraphWNorm1->GetXaxis();
  xaxisRunGraphWNorm1->SetLimits(run_range[0],run_range[1]);
  fitWNormUnRegGraphIn1->Draw("same");
  fitWNormUnRegGraphOut1->Draw("same");
  fitWNormRegGraphIn1->Draw("same");
  fitWNormRegGraphOut1->Draw("same");

  TPaveStats *statsWNormUnRegIn1 =(TPaveStats*)runGraphWNormUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWNormUnRegOut1=(TPaveStats*)runGraphWNormUnRegOut1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWNormRegIn1 =(TPaveStats*)runGraphWNormRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWNormRegOut1=(TPaveStats*)runGraphWNormRegOut1->GetListOfFunctions()->FindObject("stats");
  statsWNormUnRegIn1->SetTextColor(kOrange);
  statsWNormUnRegIn1->SetFillColor(kWhite); 
  statsWNormUnRegOut1->SetTextColor(kGray+2);
  statsWNormUnRegOut1->SetFillColor(kWhite); 
  statsWNormRegIn1->SetTextColor(kOrange+7);
  statsWNormRegIn1->SetFillColor(kWhite); 
  statsWNormRegOut1->SetTextColor(kBlack);
  statsWNormRegOut1->SetFillColor(kWhite); 
  statsWNormUnRegIn1->SetX1NDC(0.80);    statsWNormUnRegIn1->SetX2NDC(0.99); 
  statsWNormUnRegIn1->SetY1NDC(0.80);    statsWNormUnRegIn1->SetY2NDC(0.99);
  statsWNormUnRegOut1->SetX1NDC(0.80);  statsWNormUnRegOut1->SetX2NDC(0.99); 
  statsWNormUnRegOut1->SetY1NDC(0.60);  statsWNormUnRegOut1->SetY2NDC(0.79);
  statsWNormRegIn1->SetX1NDC(0.80);    statsWNormRegIn1->SetX2NDC(0.99); 
  statsWNormRegIn1->SetY1NDC(0.40);    statsWNormRegIn1->SetY2NDC(0.59);
  statsWNormRegOut1->SetX1NDC(0.80);  statsWNormRegOut1->SetX2NDC(0.99); 
  statsWNormRegOut1->SetY1NDC(0.20);  statsWNormRegOut1->SetY2NDC(0.39);

  TLegend *legRunWNorm1 = new TLegend(0.80,0.01,0.99,0.19);
  legRunWNorm1->AddEntry("runGraphWNormUnRegIn1",  Form("In Norm UnReg=%4.1f ppm",fitResultWNormUnRegIn1),"lp");
  legRunWNorm1->AddEntry("runGraphWNormUnRegOut1",  Form("Out Norm UnReg=%4.1f ppm",fitResultWNormUnRegOut1),"lp");
  legRunWNorm1->AddEntry("runGraphWNormRegIn1",  Form("In Norm Reg=%4.1f ppm",fitResultWNormRegIn1),"lp");
  legRunWNorm1->AddEntry("runGraphWNormRegOut1",  Form("Out Norm Reg=%4.1f ppm",fitResultWNormRegOut1),"lp");
  legRunWNorm1->SetTextSize(0.035);
  legRunWNorm1->SetFillColor(0);
  legRunWNorm1->SetBorderSize(2);
  legRunWNorm1->Draw();

  TLatex* trunGraphWNormUnRegIn1=new TLatex(11496,y_scaleW[1]-y_scaleW_off[0],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWNormUnRegIn1Chisquare/fitResultWNormUnRegIn1NDF,fitResultWNormUnRegIn1Prob));
  trunGraphWNormUnRegIn1->SetTextSize(0.05);
  trunGraphWNormUnRegIn1->SetTextColor(kOrange);
  TLatex* trunGraphWNormUnRegOut1=new TLatex(11496,y_scaleW[1]-y_scaleW_off[1],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWNormUnRegOut1Chisquare/fitResultWNormUnRegOut1NDF,fitResultWNormUnRegOut1Prob));
  trunGraphWNormUnRegOut1->SetTextSize(0.05);
  trunGraphWNormUnRegOut1->SetTextColor(kGray+2);
  TLatex* trunGraphWNormRegIn1=new TLatex(11496,y_scaleW[0]+y_scaleW_off[1],Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWNormRegIn1Chisquare/fitResultWNormRegIn1NDF,fitResultWNormRegIn1Prob));
  trunGraphWNormRegIn1->SetTextSize(0.05);
  trunGraphWNormRegIn1->SetTextColor(kOrange+7);
  TLatex* trunGraphWNormRegOut1=new TLatex(11496,y_scaleW[0]+y_scaleW_off[0],Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWNormRegOut1Chisquare/fitResultWNormRegOut1NDF,fitResultWNormRegOut1Prob));
  trunGraphWNormRegOut1->SetTextSize(0.05);
  trunGraphWNormRegOut1->SetTextColor(kBlack);
  trunGraphWNormUnRegIn1->Draw();
  trunGraphWNormUnRegOut1->Draw();
  trunGraphWNormRegIn1->Draw();
  trunGraphWNormRegOut1->Draw();

  gPad->Update();

  c16->Update();
  c16->SaveAs(Form("%s/plots/%s/%sAsymWidthRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /*********************************************************************************/
  /***********************************************************************************/

  if(RUNLET_PLOT_WIDTH_DIFF){

  TCanvas *c17 = new TCanvas("c17","c17",cx1,cy1,1200,950);
  TPad * pad170 = new TPad("pad170","pad170",ps1,ps2,ps4,ps4);
  TPad * pad171 = new TPad("pad171","pad171",ps1,ps1,ps4,ps3);
  pad170->Draw();
  pad171->Draw();
  pad170->cd();
  TText * ct170 = new TText(tll,tlr,Form("%sN to Delta %s Runlet-by-Runlet %s Asym. Width Correction",regTitle,dataInfo,plotTitle));
  ct170->SetTextSize(tsiz);
  ct170->Draw();
  pad171->cd();
  pad171->Divide(1,2);

  pad171->cd(1);

  TGraphErrors * runGraphWDiffIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymWCalcDiffInVal,zeroRegIn,zeroRegIn);
  runGraphWDiffIn1->SetName("runGraphWDiffIn1");
  runGraphWDiffIn1->SetMarkerColor(kRed);
  runGraphWDiffIn1->SetLineColor(kRed);
  runGraphWDiffIn1->SetMarkerStyle(21);
  runGraphWDiffIn1->SetMarkerSize(0.5);
  runGraphWDiffIn1->SetLineWidth(1);
//   runGraphWDiffIn1->Draw("AP");
  runGraphWDiffIn1->Fit("fitWDiffGraphIn1","E M R F 0 Q");
  fitWDiffGraphIn1->SetLineColor(kRed);
  fitWDiffGraphIn1->SetLineWidth(2);
  fitWDiffGraphIn1->SetLineStyle(1);

  Double_t fitResultWDiffIn1            =  fitWDiffGraphIn1->GetParameter(0);
  Double_t fitResultWDiffIn1Error       =  fitWDiffGraphIn1->GetParError(0);
  Double_t fitResultWDiffIn1Chisquare   =  fitWDiffGraphIn1->GetChisquare();
  Double_t fitResultWDiffIn1NDF         =  fitWDiffGraphIn1->GetNDF();
  Double_t fitResultWDiffIn1Prob        =  fitWDiffGraphIn1->GetProb();

  TGraphErrors * runGraphWDiffOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymWCalcDiffOutVal,zeroRegOut,zeroRegOut);
  runGraphWDiffOut1->SetName("runGraphWDiffOut1");
  runGraphWDiffOut1->SetMarkerColor(kBlack);
  runGraphWDiffOut1->SetLineColor(kBlack);
  runGraphWDiffOut1->SetMarkerStyle(21);
  runGraphWDiffOut1->SetMarkerSize(0.5);
  runGraphWDiffOut1->SetLineWidth(1);
//   runGraphWDiffOut1->Draw("AP");
  runGraphWDiffOut1->Fit("fitWDiffGraphOut1","E M R F 0 Q");
  fitWDiffGraphOut1->SetLineColor(kBlack);
  fitWDiffGraphOut1->SetLineWidth(2);
  fitWDiffGraphOut1->SetLineStyle(1);

  Double_t fitResultWDiffOut1            =  fitWDiffGraphOut1->GetParameter(0);
  Double_t fitResultWDiffOut1Error       =  fitWDiffGraphOut1->GetParError(0);
  Double_t fitResultWDiffOut1Chisquare   =  fitWDiffGraphOut1->GetChisquare();
  Double_t fitResultWDiffOut1NDF         =  fitWDiffGraphOut1->GetNDF();
  Double_t fitResultWDiffOut1Prob        =  fitWDiffGraphOut1->GetProb();

  TMultiGraph *runGraphWDiff1 = new TMultiGraph();
  runGraphWDiff1->Add(runGraphWDiffIn1);
  runGraphWDiff1->Add(runGraphWDiffOut1);
  runGraphWDiff1->Draw("AP");
  runGraphWDiff1->SetTitle("");
  runGraphWDiff1->GetXaxis()->SetTitle("Run Number");
  runGraphWDiff1->GetYaxis()->SetTitle(Form("%s Asym Width [ppm]",plotTitle));
  runGraphWDiff1->GetXaxis()->CenterTitle();
  runGraphWDiff1->GetYaxis()->CenterTitle();
  runGraphWDiff1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphWDiff1->GetYaxis()->SetRangeUser(0,2.3*y_scaleW_off[1]);
  TAxis *xaxisRunGraphWDiff1= runGraphWDiff1->GetXaxis();
  xaxisRunGraphWDiff1->SetLimits(run_range[0],run_range[1]);

  TPaveStats *statsWDiffIn1 =(TPaveStats*)runGraphWDiffIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWDiffOut1=(TPaveStats*)runGraphWDiffOut1->GetListOfFunctions()->FindObject("stats");
  statsWDiffIn1->SetTextColor(kRed);
  statsWDiffIn1->SetFillColor(kWhite); 
  statsWDiffOut1->SetTextColor(kBlack);
  statsWDiffOut1->SetFillColor(kWhite); 
  statsWDiffIn1->SetX1NDC(x_lo_stat_in);    statsWDiffIn1->SetX2NDC(x_hi_stat_in); 
  statsWDiffIn1->SetY1NDC(y_lo_stat_in);    statsWDiffIn1->SetY2NDC(y_hi_stat_in);
  statsWDiffOut1->SetX1NDC(x_lo_stat_out);  statsWDiffOut1->SetX2NDC(x_hi_stat_out); 
  statsWDiffOut1->SetY1NDC(y_lo_stat_out);  statsWDiffOut1->SetY2NDC(y_hi_stat_out);

  TLegend *legRunWDiff1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunWDiff1->AddEntry("runGraphWDiffIn1",  Form("In =%4.1f ppm",fitResultWDiffIn1),"lp");
  legRunWDiff1->AddEntry("runGraphWDiffOut1",  Form("Out =%4.1f ppm",fitResultWDiffOut1),"lp");
  legRunWDiff1->SetTextSize(0.045);
  legRunWDiff1->SetFillColor(0);
  legRunWDiff1->SetBorderSize(2);
  legRunWDiff1->Draw();

  TLatex* trunGraphWDiffIn1=new TLatex(11496,2.3*y_scaleW_off[1]-3,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWDiffIn1Chisquare/fitResultWDiffIn1NDF,fitResultWDiffIn1Prob));
  trunGraphWDiffIn1->SetTextSize(0.05);
  trunGraphWDiffIn1->SetTextColor(kRed);
  TLatex* trunGraphWDiffOut1=new TLatex(11496,2.3*y_scaleW_off[1]-6,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWDiffOut1Chisquare/fitResultWDiffOut1NDF,fitResultWDiffOut1Prob));
  trunGraphWDiffOut1->SetTextSize(0.05);
  trunGraphWDiffOut1->SetTextColor(kBlack);
  trunGraphWDiffIn1->Draw();
  trunGraphWDiffOut1->Draw();

  fitWDiffGraphIn1->Draw("same");
  fitWDiffGraphOut1->Draw("same");

  gPad->Update();


  pad171->cd(2);

  TGraphErrors * runGraphWNormDiffIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,mdAsymWNormDiffInVal,zeroRegIn,zeroRegIn);
  runGraphWNormDiffIn1->SetName("runGraphWNormDiffIn1");
  runGraphWNormDiffIn1->SetMarkerColor(kOrange+7);
  runGraphWNormDiffIn1->SetLineColor(kOrange+7);
  runGraphWNormDiffIn1->SetMarkerStyle(21);
  runGraphWNormDiffIn1->SetMarkerSize(0.5);
  runGraphWNormDiffIn1->SetLineWidth(1);
//   runGraphWNormDiffIn1->Draw("AP");
  runGraphWNormDiffIn1->Fit("fitWNormDiffGraphIn1","E M R F 0 Q");
  fitWNormDiffGraphIn1->SetLineColor(kOrange+7);
  fitWNormDiffGraphIn1->SetLineWidth(2);
  fitWNormDiffGraphIn1->SetLineStyle(1);

  Double_t fitResultWNormDiffIn1            =  fitWNormDiffGraphIn1->GetParameter(0);
  Double_t fitResultWNormDiffIn1Error       =  fitWNormDiffGraphIn1->GetParError(0);
  Double_t fitResultWNormDiffIn1Chisquare   =  fitWNormDiffGraphIn1->GetChisquare();
  Double_t fitResultWNormDiffIn1NDF         =  fitWNormDiffGraphIn1->GetNDF();
  Double_t fitResultWNormDiffIn1Prob        =  fitWNormDiffGraphIn1->GetProb();

  TGraphErrors * runGraphWNormDiffOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,mdAsymWNormDiffOutVal,zeroRegOut,zeroRegOut);
  runGraphWNormDiffOut1->SetName("runGraphWNormDiffOut1");
  runGraphWNormDiffOut1->SetMarkerColor(kGray+2);
  runGraphWNormDiffOut1->SetLineColor(kGray+2);
  runGraphWNormDiffOut1->SetMarkerStyle(21);
  runGraphWNormDiffOut1->SetMarkerSize(0.5);
  runGraphWNormDiffOut1->SetLineWidth(1);
//   runGraphWNormDiffOut1->Draw("AP");
  runGraphWNormDiffOut1->Fit("fitWNormDiffGraphOut1","E M R F 0 Q");
  fitWNormDiffGraphOut1->SetLineColor(kGray+2);
  fitWNormDiffGraphOut1->SetLineWidth(2);
  fitWNormDiffGraphOut1->SetLineStyle(1);

  Double_t fitResultWNormDiffOut1            =  fitWNormDiffGraphOut1->GetParameter(0);
  Double_t fitResultWNormDiffOut1Error       =  fitWNormDiffGraphOut1->GetParError(0);
  Double_t fitResultWNormDiffOut1Chisquare   =  fitWNormDiffGraphOut1->GetChisquare();
  Double_t fitResultWNormDiffOut1NDF         =  fitWNormDiffGraphOut1->GetNDF();
  Double_t fitResultWNormDiffOut1Prob        =  fitWNormDiffGraphOut1->GetProb();


  TMultiGraph *runGraphWNormDiff1 = new TMultiGraph();
  runGraphWNormDiff1->Add(runGraphWNormDiffIn1);
  runGraphWNormDiff1->Add(runGraphWNormDiffOut1);
  runGraphWNormDiff1->Draw("AP");
  runGraphWNormDiff1->SetTitle("");
  runGraphWNormDiff1->GetXaxis()->SetTitle("Run Number");
  runGraphWNormDiff1->GetYaxis()->SetTitle(Form("%s Asym Norm Width [ppm]",plotTitle));
  runGraphWNormDiff1->GetXaxis()->CenterTitle();
  runGraphWNormDiff1->GetYaxis()->CenterTitle();
  runGraphWNormDiff1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphWNormDiff1->GetYaxis()->SetRangeUser(0,2.3*y_scaleW_off[1]);
  TAxis *xaxisRunGraphWNormDiff1= runGraphWNormDiff1->GetXaxis();
  xaxisRunGraphWNormDiff1->SetLimits(run_range[0],run_range[1]);
  fitWNormDiffGraphIn1->Draw("same");
  fitWNormDiffGraphOut1->Draw("same");

  TPaveStats *statsWNormDiffIn1 =(TPaveStats*)runGraphWNormDiffIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsWNormDiffOut1=(TPaveStats*)runGraphWNormDiffOut1->GetListOfFunctions()->FindObject("stats");
  statsWNormDiffIn1->SetTextColor(kOrange+7);
  statsWNormDiffIn1->SetFillColor(kWhite); 
  statsWNormDiffOut1->SetTextColor(kGray+2);
  statsWNormDiffOut1->SetFillColor(kWhite); 
  statsWNormDiffIn1->SetX1NDC(x_lo_stat_in);    statsWNormDiffIn1->SetX2NDC(x_hi_stat_in); 
  statsWNormDiffIn1->SetY1NDC(y_lo_stat_in);    statsWNormDiffIn1->SetY2NDC(y_hi_stat_in);
  statsWNormDiffOut1->SetX1NDC(x_lo_stat_out);  statsWNormDiffOut1->SetX2NDC(x_hi_stat_out); 
  statsWNormDiffOut1->SetY1NDC(y_lo_stat_out);  statsWNormDiffOut1->SetY2NDC(y_hi_stat_out);

  TLegend *legRunWNormDiff1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunWNormDiff1->AddEntry("runGraphWNormDiffIn1",  Form("In Norm =%4.1f ppm",fitResultWNormDiffIn1),"lp");
  legRunWNormDiff1->AddEntry("runGraphWNormDiffOut1",  Form("Out Norm =%4.1f ppm",fitResultWNormDiffOut1),"lp");
  legRunWNormDiff1->SetTextSize(0.045);
  legRunWNormDiff1->SetFillColor(0);
  legRunWNormDiff1->SetBorderSize(2);
  legRunWNormDiff1->Draw();

  TLatex* trunGraphWNormDiffIn1=new TLatex(11496,2.3*y_scaleW_off[1]-3,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWNormDiffIn1Chisquare/fitResultWNormDiffIn1NDF,fitResultWNormDiffIn1Prob));
  trunGraphWNormDiffIn1->SetTextSize(0.05);
  trunGraphWNormDiffIn1->SetTextColor(kOrange+7);
  TLatex* trunGraphWNormDiffOut1=new TLatex(11496,2.3*y_scaleW_off[1]-6,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultWNormDiffOut1Chisquare/fitResultWNormDiffOut1NDF,fitResultWNormDiffOut1Prob));
  trunGraphWNormDiffOut1->SetTextSize(0.05);
  trunGraphWNormDiffOut1->SetTextColor(kGray+2);
  trunGraphWNormDiffIn1->Draw();
  trunGraphWNormDiffOut1->Draw();

  gPad->Update();


  c17->Update();
  c17->SaveAs(Form("%s/plots/%s/%sAsymWidthDiffRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /*********************************************************************************/

  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(RUN_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.6);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  /*********************************************************************************/
  if(RUN_PLOT_DB){
  TCanvas *c0 = new TCanvas("c0","c0",cx1,cy1,1200,950);
  TPad * pad00 = new TPad("pad00","pad00",ps1,ps2,ps4,ps4);
  TPad * pad01 = new TPad("pad01","pad01",ps1,ps1,ps4,ps3);
  pad00->Draw();
  pad01->Draw();
  pad00->cd();
  TText * ct00 = new TText(tll,tlr,Form("%sN to Delta %s %s Asym. using Standard DB Regression",regTitle,dataInfo,plotTitle));
  ct00->SetTextSize(tsiz);
  ct00->Draw();
  pad01->cd();
  pad01->Divide(1,2);

  pad01->cd(1);

  TGraphErrors * runGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymUnRegRunInVal,zeroRunIn,emdAsymUnRegRunInVal);
  runGraphUnRegIn1->SetName("runGraphUnRegIn1");
  runGraphUnRegIn1->SetMarkerColor(kRed);
  runGraphUnRegIn1->SetLineColor(kRed);
  runGraphUnRegIn1->SetMarkerStyle(25);
  runGraphUnRegIn1->SetMarkerSize(0.5);
  runGraphUnRegIn1->SetLineWidth(1);
//   runGraphUnRegIn1->Draw("AP");
  runGraphUnRegIn1->Fit("fitRunGraphUnRegIn1","E M R F 0 Q");
  fitRunGraphUnRegIn1->SetLineColor(kRed);
  fitRunGraphUnRegIn1->SetLineWidth(2);
  fitRunGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunUnRegIn1            =  fitRunGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunUnRegIn1Error       =  fitRunGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunUnRegIn1Chisquare   =  fitRunGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunUnRegIn1NDF         =  fitRunGraphUnRegIn1->GetNDF();
  Double_t fitResultRunUnRegIn1Prob        =  fitRunGraphUnRegIn1->GetProb();


  TGraphErrors * runGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymUnRegRunOutVal,zeroRunOut,emdAsymUnRegRunOutVal);
  runGraphUnRegOut1->SetName("runGraphUnRegOut1");
  runGraphUnRegOut1->SetMarkerColor(kBlack);
  runGraphUnRegOut1->SetLineColor(kBlack);
  runGraphUnRegOut1->SetMarkerStyle(25);
  runGraphUnRegOut1->SetMarkerSize(0.5);
  runGraphUnRegOut1->SetLineWidth(1);
//   runGraphUnRegOut1->Draw("AP");
  runGraphUnRegOut1->Fit("fitRunGraphUnRegOut1","E M R F 0 Q");
  fitRunGraphUnRegOut1->SetLineColor(kBlack);
  fitRunGraphUnRegOut1->SetLineWidth(2);
  fitRunGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunUnRegOut1           =  fitRunGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunUnRegOut1Error      =  fitRunGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunUnRegOut1Chisquare  =  fitRunGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunUnRegOut1NDF        =  fitRunGraphUnRegOut1->GetNDF();
  Double_t fitResultRunUnRegOut1Prob       =  fitRunGraphUnRegOut1->GetProb();

  TMultiGraph *runGraphUnReg1 = new TMultiGraph();
  runGraphUnReg1->Add(runGraphUnRegIn1);
  runGraphUnReg1->Add(runGraphUnRegOut1);
  runGraphUnReg1->Draw("AP");
  runGraphUnReg1->SetTitle("");
  runGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runGraphUnReg1->GetYaxis()->SetTitle(Form("%s Asym UnReg [ppm]",plotTitle));
  runGraphUnReg1->GetXaxis()->CenterTitle();
  runGraphUnReg1->GetYaxis()->CenterTitle();
  runGraphUnReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphUnReg1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunGraphUnReg1= runGraphUnReg1->GetXaxis();
  xaxisRunGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphUnRegIn1->Draw("same");
  fitRunGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunUnRegIn1 =(TPaveStats*)runGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunUnRegOut1=(TPaveStats*)runGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunUnRegIn1->SetTextColor(kRed);
  statsRunUnRegIn1->SetFillColor(kWhite); 
  statsRunUnRegOut1->SetTextColor(kBlack);
  statsRunUnRegOut1->SetFillColor(kWhite); 
  statsRunUnRegIn1->SetX1NDC(x_lo_stat_in);    statsRunUnRegIn1->SetX2NDC(x_hi_stat_in); 
  statsRunUnRegIn1->SetY1NDC(y_lo_stat_in);    statsRunUnRegIn1->SetY2NDC(y_hi_stat_in);
  statsRunUnRegOut1->SetX1NDC(x_lo_stat_out);  statsRunUnRegOut1->SetX2NDC(x_hi_stat_out); 
  statsRunUnRegOut1->SetY1NDC(y_lo_stat_out);  statsRunUnRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultRunUnRegInpOut1       =  (fitResultRunUnRegIn1+fitResultRunUnRegOut1)/2;
  Double_t resultRunUnRegInmOut1       =  (1/(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error)*(fitResultRunUnRegIn1)-1/(fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)*(fitResultRunUnRegOut1))/(1/(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error)+1/(fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error));
//   Double_t resultRunUnRegInmOut1       =  (fitResultRunUnRegIn1-fitResultRunUnRegOut1)/2;
  Double_t resultRunUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error+fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)/2;
  Double_t resultRunUnRegInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error)+1/(fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)));
//   Double_t resultRunUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunUnRegIn1Error*fitResultRunUnRegIn1Error+fitResultRunUnRegOut1Error*fitResultRunUnRegOut1Error)/2;


  TLegend *legRunUnReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunUnReg1->AddEntry("runGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunUnRegIn1,fitResultRunUnRegIn1Error),"lp");
  legRunUnReg1->AddEntry("runGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunUnRegOut1,fitResultRunUnRegOut1Error),"lp");
  legRunUnReg1->AddEntry("runGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunUnRegInpOut1,resultRunUnRegInpOut1Error),"");
  legRunUnReg1->AddEntry("runGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunUnRegInmOut1,resultRunUnRegInmOut1Error),"");
  legRunUnReg1->SetTextSize(0.035);
  legRunUnReg1->SetFillColor(0);
  legRunUnReg1->SetBorderSize(2);
  legRunUnReg1->Draw();

  TLatex* tRunGraphUnRegIn1=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunUnRegIn1Chisquare/fitResultRunUnRegIn1NDF,fitResultRunUnRegIn1Prob));
  tRunGraphUnRegIn1->SetTextSize(0.05);
  tRunGraphUnRegIn1->SetTextColor(kRed);
  TLatex* tRunGraphUnRegOut1=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunUnRegOut1Chisquare/fitResultRunUnRegOut1NDF,fitResultRunUnRegOut1Prob));
  tRunGraphUnRegOut1->SetTextSize(0.05);
  tRunGraphUnRegOut1->SetTextColor(kBlack);
  tRunGraphUnRegIn1->Draw();
  tRunGraphUnRegOut1->Draw();

  gPad->Update();


  pad01->cd(2);

  TGraphErrors * runGraphRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymRegRunInVal,zeroRunIn,emdAsymRegRunInVal);
  runGraphRegIn1->SetName("runGraphRegIn1");
  runGraphRegIn1->SetMarkerColor(kRed);
  runGraphRegIn1->SetLineColor(kRed);
  runGraphRegIn1->SetMarkerStyle(21);
  runGraphRegIn1->SetMarkerSize(0.5);
  runGraphRegIn1->SetLineWidth(1);
  runGraphRegIn1->Fit("fitRunGraphRegIn1","E M R F 0 Q");
  fitRunGraphRegIn1->SetLineColor(kRed);
  fitRunGraphRegIn1->SetLineWidth(2);
  fitRunGraphRegIn1->SetLineStyle(1);

  Double_t fitResultRunRegIn1            =  fitRunGraphRegIn1->GetParameter(0);
  Double_t fitResultRunRegIn1Error       =  fitRunGraphRegIn1->GetParError(0);
  Double_t fitResultRunRegIn1Chisquare   =  fitRunGraphRegIn1->GetChisquare();
  Double_t fitResultRunRegIn1NDF         =  fitRunGraphRegIn1->GetNDF();
  Double_t fitResultRunRegIn1Prob        =  fitRunGraphRegIn1->GetProb();


  TGraphErrors * runGraphRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymRegRunOutVal,zeroRunOut,emdAsymRegRunOutVal);
  runGraphRegOut1->SetName("runGraphRegOut1");
  runGraphRegOut1->SetMarkerColor(kBlack);
  runGraphRegOut1->SetLineColor(kBlack);
  runGraphRegOut1->SetMarkerStyle(21);
  runGraphRegOut1->SetMarkerSize(0.5);
  runGraphRegOut1->SetLineWidth(1);
  runGraphRegOut1->Fit("fitRunGraphRegOut1","E M R F 0 Q");
  fitRunGraphRegOut1->SetLineColor(kBlack);
  fitRunGraphRegOut1->SetLineWidth(2);
  fitRunGraphRegOut1->SetLineStyle(1);

  Double_t fitResultRunRegOut1           =  fitRunGraphRegOut1->GetParameter(0);
  Double_t fitResultRunRegOut1Error      =  fitRunGraphRegOut1->GetParError(0);
  Double_t fitResultRunRegOut1Chisquare  =  fitRunGraphRegOut1->GetChisquare();
  Double_t fitResultRunRegOut1NDF        =  fitRunGraphRegOut1->GetNDF();
  Double_t fitResultRunRegOut1Prob       =  fitRunGraphRegOut1->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn1);
  runGraphReg1->Add(runGraphRegOut1);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn1->Draw("same");
  fitRunGraphRegOut1->Draw("same");

  TPaveStats *statsRunRegIn1 =(TPaveStats*)runGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut1=(TPaveStats*)runGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn1->SetTextColor(kRed);
  statsRunRegIn1->SetFillColor(kWhite); 
  statsRunRegOut1->SetTextColor(kBlack);
  statsRunRegOut1->SetFillColor(kWhite); 
  statsRunRegIn1->SetX1NDC(x_lo_stat_in);    statsRunRegIn1->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn1->SetY1NDC(y_lo_stat_in);    statsRunRegIn1->SetY2NDC(y_hi_stat_in);
  statsRunRegOut1->SetX1NDC(x_lo_stat_out);  statsRunRegOut1->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut1->SetY1NDC(y_lo_stat_out);  statsRunRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut1       =  (fitResultRunRegIn1+fitResultRunRegOut1)/2;
  Double_t resultRunRegInmOut1       =  (1/(fitResultRunRegIn1Error*fitResultRunRegIn1Error)*(fitResultRunRegIn1)-1/(fitResultRunRegOut1Error*fitResultRunRegOut1Error)*(fitResultRunRegOut1))/(1/(fitResultRunRegIn1Error*fitResultRunRegIn1Error)+1/(fitResultRunRegOut1Error*fitResultRunRegOut1Error));
//   Double_t resultRunRegInmOut1       =  (fitResultRunRegIn1-fitResultRunRegOut1)/2;
  Double_t resultRunRegInpOut1Error  =  TMath::Sqrt(fitResultRunRegIn1Error*fitResultRunRegIn1Error+fitResultRunRegOut1Error*fitResultRunRegOut1Error)/2;
  Double_t resultRunRegInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn1Error*fitResultRunRegIn1Error)+1/(fitResultRunRegOut1Error*fitResultRunRegOut1Error)));
//   Double_t resultRunRegInmOut1Error  =  TMath::Sqrt(fitResultRunRegIn1Error*fitResultRunRegIn1Error+fitResultRunRegOut1Error*fitResultRunRegOut1Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn1",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn1,fitResultRunRegIn1Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut1",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut1,fitResultRunRegOut1Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut1,resultRunRegInpOut1Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut1,resultRunRegInmOut1Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn1=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn1Chisquare/fitResultRunRegIn1NDF,fitResultRunRegIn1Prob));
  tRunGraphRegIn1->SetTextSize(0.05);
  tRunGraphRegIn1->SetTextColor(kRed);
  TLatex* tRunGraphRegOut1=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut1Chisquare/fitResultRunRegOut1NDF,fitResultRunRegOut1Prob));
  tRunGraphRegOut1->SetTextSize(0.05);
  tRunGraphRegOut1->SetTextColor(kBlack);
  tRunGraphRegIn1->Draw();
  tRunGraphRegOut1->Draw();

  gPad->Update();

  ofstream outfileUnRegRun(Form("%s/plots/%s/summary%sRunUnReg.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot));
  outfileUnRegRun << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultRunUnRegOut1,fitResultRunUnRegOut1Error,fitResultRunUnRegIn1,fitResultRunUnRegIn1Error) <<endl;
  outfileUnRegRun.close();
  ofstream outfileRegRun(Form("%s/plots/%s/summary%sRun%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRegRun << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultRunRegOut1,fitResultRunRegOut1Error,fitResultRunRegIn1,fitResultRunRegIn1Error) <<endl;
  outfileRegRun.close();

  c0->Update();
  c0->SaveAs(Form("%s/plots/%s/%sAsymRunRunbyRun%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /*********************************************************************************/
  /*********************************************************************************/
  if(RUN_PLOT_RUN_COR){
  TCanvas *c14 = new TCanvas("c14","c14",cx1,cy1,1200,950);
  TPad * pad140 = new TPad("pad140","pad140",ps1,ps2,ps4,ps4);
  TPad * pad141 = new TPad("pad141","pad141",ps1,ps1,ps4,ps3);
  pad140->Draw();
  pad141->Draw();
  pad140->cd();
  TText * ct140 = new TText(tll,tlr,Form("%sN to Delta %s Corrected %s Asym. using Run Sensitivity and Differences",regTitle,dataInfo,plotTitle));
  ct140->SetTextSize(tsiz);
  ct140->Draw();
  pad141->cd();
  pad141->Divide(1,2);

  pad141->cd(1);

  TGraphErrors * runGraphRegIn2 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymRegRunInVal,zeroRunIn,emdAsymRegRunInVal);
  runGraphRegIn2->SetName("runGraphRegIn2");
  runGraphRegIn2->SetMarkerColor(kRed);
  runGraphRegIn2->SetLineColor(kRed);
  runGraphRegIn2->SetMarkerStyle(21);
  runGraphRegIn2->SetMarkerSize(0.5);
  runGraphRegIn2->SetLineWidth(1);
//   runGraphRegIn2->Draw("AP");
  runGraphRegIn2->Fit("fitRunGraphRegIn2","E M R F 0 Q");
  fitRunGraphRegIn2->SetLineColor(kRed);
  fitRunGraphRegIn2->SetLineWidth(2);
  fitRunGraphRegIn2->SetLineStyle(1);

  Double_t fitResultRunRegIn2            =  fitRunGraphRegIn2->GetParameter(0);
  Double_t fitResultRunRegIn2Error       =  fitRunGraphRegIn2->GetParError(0);
  Double_t fitResultRunRegIn2Chisquare   =  fitRunGraphRegIn2->GetChisquare();
  Double_t fitResultRunRegIn2NDF         =  fitRunGraphRegIn2->GetNDF();
  Double_t fitResultRunRegIn2Prob        =  fitRunGraphRegIn2->GetProb();

  TGraphErrors * runGraphRegOut2 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymRegRunOutVal,zeroRunOut,emdAsymRegRunOutVal);
  runGraphRegOut2->SetName("runGraphRegOut2");
  runGraphRegOut2->SetMarkerColor(kBlack);
  runGraphRegOut2->SetLineColor(kBlack);
  runGraphRegOut2->SetMarkerStyle(21);
  runGraphRegOut2->SetMarkerSize(0.5);
  runGraphRegOut2->SetLineWidth(1);
//   runGraphRegOut2->Draw("AP");
  runGraphRegOut2->Fit("fitRunGraphRegOut2","E M R F 0 Q");
  fitRunGraphRegOut2->SetLineColor(kBlack);
  fitRunGraphRegOut2->SetLineWidth(2);
  fitRunGraphRegOut2->SetLineStyle(1);

  Double_t fitResultRunRegOut2           =  fitRunGraphRegOut2->GetParameter(0);
  Double_t fitResultRunRegOut2Error      =  fitRunGraphRegOut2->GetParError(0);
  Double_t fitResultRunRegOut2Chisquare  =  fitRunGraphRegOut2->GetChisquare();
  Double_t fitResultRunRegOut2NDF        =  fitRunGraphRegOut2->GetNDF();
  Double_t fitResultRunRegOut2Prob       =  fitRunGraphRegOut2->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn2);
  runGraphReg1->Add(runGraphRegOut2);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn2->Draw("same");
  fitRunGraphRegOut2->Draw("same");

  TPaveStats *statsRunRegIn2 =(TPaveStats*)runGraphRegIn2->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut2=(TPaveStats*)runGraphRegOut2->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn2->SetTextColor(kRed);
  statsRunRegIn2->SetFillColor(kWhite); 
  statsRunRegOut2->SetTextColor(kBlack);
  statsRunRegOut2->SetFillColor(kWhite); 
  statsRunRegIn2->SetX1NDC(x_lo_stat_in);    statsRunRegIn2->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn2->SetY1NDC(y_lo_stat_in);    statsRunRegIn2->SetY2NDC(y_hi_stat_in);
  statsRunRegOut2->SetX1NDC(x_lo_stat_out);  statsRunRegOut2->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut2->SetY1NDC(y_lo_stat_out);  statsRunRegOut2->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut2       =  (fitResultRunRegIn2+fitResultRunRegOut2)/2;
  Double_t resultRunRegInmOut2       =  (1/(fitResultRunRegIn2Error*fitResultRunRegIn2Error)*(fitResultRunRegIn2)-1/(fitResultRunRegOut2Error*fitResultRunRegOut2Error)*(fitResultRunRegOut2))/(1/(fitResultRunRegIn2Error*fitResultRunRegIn2Error)+1/(fitResultRunRegOut2Error*fitResultRunRegOut2Error));
//   Double_t resultRunRegInmOut2       =  (fitResultRunRegIn2-fitResultRunRegOut2)/2;
  Double_t resultRunRegInpOut2Error  =  TMath::Sqrt(fitResultRunRegIn2Error*fitResultRunRegIn2Error+fitResultRunRegOut2Error*fitResultRunRegOut2Error)/2;
  Double_t resultRunRegInmOut2Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn2Error*fitResultRunRegIn2Error)+1/(fitResultRunRegOut2Error*fitResultRunRegOut2Error)));
//   Double_t resultRunRegInmOut2Error  =  TMath::Sqrt(fitResultRunRegIn2Error*fitResultRunRegIn2Error+fitResultRunRegOut2Error*fitResultRunRegOut2Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn2",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn2,fitResultRunRegIn2Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut2",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut2,fitResultRunRegOut2Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut2",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut2,resultRunRegInpOut2Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut2", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut2,resultRunRegInmOut2Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn2=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn2Chisquare/fitResultRunRegIn2NDF,fitResultRunRegIn2Prob));
  tRunGraphRegIn2->SetTextSize(0.05);
  tRunGraphRegIn2->SetTextColor(kRed);
  TLatex* tRunGraphRegOut2=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut2Chisquare/fitResultRunRegOut2NDF,fitResultRunRegOut2Prob));
  tRunGraphRegOut2->SetTextSize(0.05);
  tRunGraphRegOut2->SetTextColor(kBlack);
  tRunGraphRegIn2->Draw();
  tRunGraphRegOut2->Draw();

  gPad->Update();

  pad141->cd(2);

  TGraphErrors * runGraphCorIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymCorRunIn,zeroRunIn,emdAsymRegRunInVal);
  runGraphCorIn1->SetName("runGraphCorIn1");
  runGraphCorIn1->SetMarkerColor(kOrange+2);
  runGraphCorIn1->SetLineColor(kOrange+2);
  runGraphCorIn1->SetMarkerStyle(21);
  runGraphCorIn1->SetMarkerSize(0.5);
  runGraphCorIn1->SetLineWidth(1);
  runGraphCorIn1->Fit("fitRunGraphCorIn1","E M R F 0 Q");
  fitRunGraphCorIn1->SetLineColor(kOrange+2);
  fitRunGraphCorIn1->SetLineWidth(2);
  fitRunGraphCorIn1->SetLineStyle(1);

  Double_t fitResultRunCorIn1            =  fitRunGraphCorIn1->GetParameter(0);
  Double_t fitResultRunCorIn1Error       =  fitRunGraphCorIn1->GetParError(0);
  Double_t fitResultRunCorIn1Chisquare   =  fitRunGraphCorIn1->GetChisquare();
  Double_t fitResultRunCorIn1NDF         =  fitRunGraphCorIn1->GetNDF();
  Double_t fitResultRunCorIn1Prob        =  fitRunGraphCorIn1->GetProb();


  TGraphErrors * runGraphCorOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymCorRunOut,zeroRunOut,emdAsymRegRunOutVal);
  runGraphCorOut1->SetName("runGraphCorOut1");
  runGraphCorOut1->SetMarkerColor(kGray+2);
  runGraphCorOut1->SetLineColor(kGray+2);
  runGraphCorOut1->SetMarkerStyle(21);
  runGraphCorOut1->SetMarkerSize(0.5);
  runGraphCorOut1->SetLineWidth(1);
  runGraphCorOut1->Fit("fitRunGraphCorOut1","E M R F 0 Q");
  fitRunGraphCorOut1->SetLineColor(kGray+2);
  fitRunGraphCorOut1->SetLineWidth(2);
  fitRunGraphCorOut1->SetLineStyle(1);

  Double_t fitResultRunCorOut1           =  fitRunGraphCorOut1->GetParameter(0);
  Double_t fitResultRunCorOut1Error      =  fitRunGraphCorOut1->GetParError(0);
  Double_t fitResultRunCorOut1Chisquare  =  fitRunGraphCorOut1->GetChisquare();
  Double_t fitResultRunCorOut1NDF        =  fitRunGraphCorOut1->GetNDF();
  Double_t fitResultRunCorOut1Prob       =  fitRunGraphCorOut1->GetProb();

  TMultiGraph *runGraphCor1 = new TMultiGraph();
  runGraphCor1->Add(runGraphCorIn1);
  runGraphCor1->Add(runGraphCorOut1);
  runGraphCor1->Draw("AP");
  runGraphCor1->SetTitle("");
  runGraphCor1->GetXaxis()->SetTitle("Run Number");
  runGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Run Cor [ppm]",plotTitle));
  runGraphCor1->GetXaxis()->CenterTitle();
  runGraphCor1->GetYaxis()->CenterTitle();
  runGraphCor1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphCor1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunGraphCor1= runGraphCor1->GetXaxis();
  xaxisRunGraphCor1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphCorIn1->Draw("same");
  fitRunGraphCorOut1->Draw("same");

  TPaveStats *statsRunCorIn1 =(TPaveStats*)runGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunCorOut1=(TPaveStats*)runGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsRunCorIn1->SetTextColor(kOrange+7);
  statsRunCorIn1->SetFillColor(kWhite); 
  statsRunCorOut1->SetTextColor(kGray+2);
  statsRunCorOut1->SetFillColor(kWhite); 
  statsRunCorIn1->SetX1NDC(x_lo_stat_in);    statsRunCorIn1->SetX2NDC(x_hi_stat_in); 
  statsRunCorIn1->SetY1NDC(y_lo_stat_in);    statsRunCorIn1->SetY2NDC(y_hi_stat_in);
  statsRunCorOut1->SetX1NDC(x_lo_stat_out);  statsRunCorOut1->SetX2NDC(x_hi_stat_out); 
  statsRunCorOut1->SetY1NDC(y_lo_stat_out);  statsRunCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultRunCorInpOut1       =  (fitResultRunCorIn1+fitResultRunCorOut1)/2;
  Double_t resultRunCorInmOut1       =  (1/(fitResultRunCorIn1Error*fitResultRunCorIn1Error)*(fitResultRunCorIn1)-1/(fitResultRunCorOut1Error*fitResultRunCorOut1Error)*(fitResultRunCorOut1))/(1/(fitResultRunCorIn1Error*fitResultRunCorIn1Error)+1/(fitResultRunCorOut1Error*fitResultRunCorOut1Error));
//   Double_t resultRunCorInmOut1       =  (fitResultRunCorIn1-fitResultRunCorOut1)/2;
  Double_t resultRunCorInpOut1Error  =  TMath::Sqrt(fitResultRunCorIn1Error*fitResultRunCorIn1Error+fitResultRunCorOut1Error*fitResultRunCorOut1Error)/2;
  Double_t resultRunCorInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunCorIn1Error*fitResultRunCorIn1Error)+1/(fitResultRunCorOut1Error*fitResultRunCorOut1Error)));
//   Double_t resultRunCorInmOut1Error  =  TMath::Sqrt(fitResultRunCorIn1Error*fitResultRunCorIn1Error+fitResultRunCorOut1Error*fitResultRunCorOut1Error)/2;


  TLegend *legRunCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunCor1->AddEntry("runGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultRunCorIn1,fitResultRunCorIn1Error),"lp");
  legRunCor1->AddEntry("runGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultRunCorOut1,fitResultRunCorOut1Error),"lp");
  legRunCor1->AddEntry("runGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultRunCorInpOut1,resultRunCorInpOut1Error),"");
  legRunCor1->AddEntry("runGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultRunCorInmOut1,resultRunCorInmOut1Error),"");
  legRunCor1->SetTextSize(0.035);
  legRunCor1->SetFillColor(0);
  legRunCor1->SetBorderSize(2);
  legRunCor1->Draw();

  TLatex* tRunGraphCorIn1=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunCorIn1Chisquare/fitResultRunCorIn1NDF,fitResultRunCorIn1Prob));
  tRunGraphCorIn1->SetTextSize(0.05);
  tRunGraphCorIn1->SetTextColor(kOrange+2);
  TLatex* tRunGraphCorOut1=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunCorOut1Chisquare/fitResultRunCorOut1NDF,fitResultRunCorOut1Prob));
  tRunGraphCorOut1->SetTextSize(0.05);
  tRunGraphCorOut1->SetTextColor(kGray+2);
  tRunGraphCorIn1->Draw();
  tRunGraphCorOut1->Draw();

  gPad->Update();

  ofstream outfileRunCor(Form("%s/plots/%s/summary%sRunCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRunCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultRunCorOut1,fitResultRunCorOut1Error,fitResultRunCorIn1,fitResultRunCorIn1Error) <<endl;
  outfileRunCor.close();

  c14->Update();
  c14->SaveAs(Form("%s/plots/%s/%sAsymRunRunbyRunCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }

  /***********************************************************************************/
  if(RUN_PLOT_SLUG_COR){
  TCanvas *c10 = new TCanvas("c10","c10",cx1,cy1,1200,950);
  TPad * pad100 = new TPad("pad100","pad100",ps1,ps2,ps4,ps4);
  TPad * pad101 = new TPad("pad101","pad101",ps1,ps1,ps4,ps3);
  pad100->Draw();
  pad101->Draw();
  pad100->cd();
  TText * ct100 = new TText(tll,tlr,Form("%sN to Delta %s Corrected %s Asym. using Slug Avg. Sensitivity and Run Differences",regTitle,dataInfo,plotTitle));
  ct100->SetTextSize(tsiz);
  ct100->Draw();
  pad101->cd();
  pad101->Divide(1,2);


  pad101->cd(1);

  TGraphErrors * runGraphRegIn3 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymRegRunInVal,zeroRegIn,emdAsymRegRunInVal);
  runGraphRegIn3->SetName("runGraphRegIn3");
  runGraphRegIn3->SetMarkerColor(kRed);
  runGraphRegIn3->SetLineColor(kRed);
  runGraphRegIn3->SetMarkerStyle(21);
  runGraphRegIn3->SetMarkerSize(0.5);
  runGraphRegIn3->SetLineWidth(1);
//   runGraphRegIn3->Draw("AP");
  runGraphRegIn3->Fit("fitRunGraphRegIn3","E M R F 0 Q");
  fitRunGraphRegIn3->SetLineColor(kRed);
  fitRunGraphRegIn3->SetLineWidth(2);
  fitRunGraphRegIn3->SetLineStyle(1);

  Double_t fitResultRunRegIn3            =  fitRunGraphRegIn3->GetParameter(0);
  Double_t fitResultRunRegIn3Error       =  fitRunGraphRegIn3->GetParError(0);
  Double_t fitResultRunRegIn3Chisquare   =  fitRunGraphRegIn3->GetChisquare();
  Double_t fitResultRunRegIn3NDF         =  fitRunGraphRegIn3->GetNDF();
  Double_t fitResultRunRegIn3Prob        =  fitRunGraphRegIn3->GetProb();

  TGraphErrors * runGraphRegOut3 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymRegRunOutVal,zeroRunOut,emdAsymRegRunOutVal);
  runGraphRegOut3->SetName("runGraphRegOut3");
  runGraphRegOut3->SetMarkerColor(kBlack);
  runGraphRegOut3->SetLineColor(kBlack);
  runGraphRegOut3->SetMarkerStyle(21);
  runGraphRegOut3->SetMarkerSize(0.5);
  runGraphRegOut3->SetLineWidth(1);
//   runGraphRegOut3->Draw("AP");
  runGraphRegOut3->Fit("fitRunGraphRegOut3","E M R F 0 Q");
  fitRunGraphRegOut3->SetLineColor(kBlack);
  fitRunGraphRegOut3->SetLineWidth(2);
  fitRunGraphRegOut3->SetLineStyle(1);

  Double_t fitResultRunRegOut3           =  fitRunGraphRegOut3->GetParameter(0);
  Double_t fitResultRunRegOut3Error      =  fitRunGraphRegOut3->GetParError(0);
  Double_t fitResultRunRegOut3Chisquare  =  fitRunGraphRegOut3->GetChisquare();
  Double_t fitResultRunRegOut3NDF        =  fitRunGraphRegOut3->GetNDF();
  Double_t fitResultRunRegOut3Prob       =  fitRunGraphRegOut3->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn3);
  runGraphReg1->Add(runGraphRegOut3);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn3->Draw("same");
  fitRunGraphRegOut3->Draw("same");

  TPaveStats *statsRunRegIn3 =(TPaveStats*)runGraphRegIn3->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut3=(TPaveStats*)runGraphRegOut3->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn3->SetTextColor(kRed);
  statsRunRegIn3->SetFillColor(kWhite); 
  statsRunRegOut3->SetTextColor(kBlack);
  statsRunRegOut3->SetFillColor(kWhite); 
  statsRunRegIn3->SetX1NDC(x_lo_stat_in);    statsRunRegIn3->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn3->SetY1NDC(y_lo_stat_in);    statsRunRegIn3->SetY2NDC(y_hi_stat_in);
  statsRunRegOut3->SetX1NDC(x_lo_stat_out);  statsRunRegOut3->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut3->SetY1NDC(y_lo_stat_out);  statsRunRegOut3->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut3       =  (fitResultRunRegIn3+fitResultRunRegOut3)/2;
  Double_t resultRunRegInmOut3       =  (1/(fitResultRunRegIn3Error*fitResultRunRegIn3Error)*(fitResultRunRegIn3)-1/(fitResultRunRegOut3Error*fitResultRunRegOut3Error)*(fitResultRunRegOut3))/(1/(fitResultRunRegIn3Error*fitResultRunRegIn3Error)+1/(fitResultRunRegOut3Error*fitResultRunRegOut3Error));
//   Double_t resultRunRegInmOut3       =  (fitResultRunRegIn3-fitResultRunRegOut3)/2;
  Double_t resultRunRegInpOut3Error  =  TMath::Sqrt(fitResultRunRegIn3Error*fitResultRunRegIn3Error+fitResultRunRegOut3Error*fitResultRunRegOut3Error)/2;
  Double_t resultRunRegInmOut3Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn3Error*fitResultRunRegIn3Error)+1/(fitResultRunRegOut3Error*fitResultRunRegOut3Error)));
//   Double_t resultRunRegInmOut3Error  =  TMath::Sqrt(fitResultRunRegIn3Error*fitResultRunRegIn3Error+fitResultRunRegOut3Error*fitResultRunRegOut3Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn3",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn3,fitResultRunRegIn3Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut3",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut3,fitResultRunRegOut3Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut3",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut3,resultRunRegInpOut3Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut3", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut3,resultRunRegInmOut3Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn3=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn3Chisquare/fitResultRunRegIn3NDF,fitResultRunRegIn3Prob));
  tRunGraphRegIn3->SetTextSize(0.05);
  tRunGraphRegIn3->SetTextColor(kRed);
  TLatex* tRunGraphRegOut3=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut3Chisquare/fitResultRunRegOut3NDF,fitResultRunRegOut3Prob));
  tRunGraphRegOut3->SetTextSize(0.05);
  tRunGraphRegOut3->SetTextColor(kBlack);
  tRunGraphRegIn3->Draw();
  tRunGraphRegOut3->Draw();

  gPad->Update();

  pad101->cd(2);

  TGraphErrors * slugGraphCorIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymCorRunSlugIn,zeroRunIn,emdAsymRegRunInVal);
  slugGraphCorIn1->SetName("slugGraphCorIn1");
  slugGraphCorIn1->SetMarkerColor(kMagenta);
  slugGraphCorIn1->SetLineColor(kMagenta);
  slugGraphCorIn1->SetMarkerStyle(21);
  slugGraphCorIn1->SetMarkerSize(0.5);
  slugGraphCorIn1->SetLineWidth(1);
  slugGraphCorIn1->Fit("fitSlugGraphCorIn1","E M R F 0 Q");
  fitSlugGraphCorIn1->SetLineColor(kMagenta);
  fitSlugGraphCorIn1->SetLineWidth(2);
  fitSlugGraphCorIn1->SetLineStyle(1);

  Double_t fitResultSlugCorIn1            =  fitSlugGraphCorIn1->GetParameter(0);
  Double_t fitResultSlugCorIn1Error       =  fitSlugGraphCorIn1->GetParError(0);
  Double_t fitResultSlugCorIn1Chisquare   =  fitSlugGraphCorIn1->GetChisquare();
  Double_t fitResultSlugCorIn1NDF         =  fitSlugGraphCorIn1->GetNDF();
  Double_t fitResultSlugCorIn1Prob        =  fitSlugGraphCorIn1->GetProb();


  TGraphErrors * slugGraphCorOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymCorRunSlugOut,zeroRunOut,emdAsymRegRunOutVal);
  slugGraphCorOut1->SetName("slugGraphCorOut1");
  slugGraphCorOut1->SetMarkerColor(kCyan+2);
  slugGraphCorOut1->SetLineColor(kCyan+2);
  slugGraphCorOut1->SetMarkerStyle(21);
  slugGraphCorOut1->SetMarkerSize(0.5);
  slugGraphCorOut1->SetLineWidth(1);
  slugGraphCorOut1->Fit("fitSlugGraphCorOut1","E M R F 0 Q");
  fitSlugGraphCorOut1->SetLineColor(kCyan+2);
  fitSlugGraphCorOut1->SetLineWidth(2);
  fitSlugGraphCorOut1->SetLineStyle(1);

  Double_t fitResultSlugCorOut1           =  fitSlugGraphCorOut1->GetParameter(0);
  Double_t fitResultSlugCorOut1Error      =  fitSlugGraphCorOut1->GetParError(0);
  Double_t fitResultSlugCorOut1Chisquare  =  fitSlugGraphCorOut1->GetChisquare();
  Double_t fitResultSlugCorOut1NDF        =  fitSlugGraphCorOut1->GetNDF();
  Double_t fitResultSlugCorOut1Prob       =  fitSlugGraphCorOut1->GetProb();

  TMultiGraph *slugGraphCor1 = new TMultiGraph();
  slugGraphCor1->Add(slugGraphCorIn1);
  slugGraphCor1->Add(slugGraphCorOut1);
  slugGraphCor1->Draw("AP");
  slugGraphCor1->SetTitle("");
  slugGraphCor1->GetXaxis()->SetTitle("Run Number");
  slugGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Slug Cor [ppm]",plotTitle));
  slugGraphCor1->GetXaxis()->CenterTitle();
  slugGraphCor1->GetYaxis()->CenterTitle();
  slugGraphCor1->GetXaxis()->SetNdivisions(10,2,0);
  slugGraphCor1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisSlugGraphCor1= slugGraphCor1->GetXaxis();
  xaxisSlugGraphCor1->SetLimits(run_range[0],run_range[1]);
  fitSlugGraphCorIn1->Draw("same");
  fitSlugGraphCorOut1->Draw("same");

  TPaveStats *statsSlugCorIn1 =(TPaveStats*)slugGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugCorOut1=(TPaveStats*)slugGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugCorIn1->SetTextColor(kMagenta);
  statsSlugCorIn1->SetFillColor(kWhite); 
  statsSlugCorOut1->SetTextColor(kCyan+2);
  statsSlugCorOut1->SetFillColor(kWhite); 
  statsSlugCorIn1->SetX1NDC(x_lo_stat_in);    statsSlugCorIn1->SetX2NDC(x_hi_stat_in); 
  statsSlugCorIn1->SetY1NDC(y_lo_stat_in);    statsSlugCorIn1->SetY2NDC(y_hi_stat_in);
  statsSlugCorOut1->SetX1NDC(x_lo_stat_out);  statsSlugCorOut1->SetX2NDC(x_hi_stat_out); 
  statsSlugCorOut1->SetY1NDC(y_lo_stat_out);  statsSlugCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultSlugCorInpOut1       =  (fitResultSlugCorIn1+fitResultSlugCorOut1)/2;
  Double_t resultSlugCorInmOut1       =  (1/(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error)*(fitResultSlugCorIn1)-1/(fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)*(fitResultSlugCorOut1))/(1/(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error)+1/(fitResultSlugCorOut1Error*fitResultSlugCorOut1Error));
//   Double_t resultSlugCorInmOut1       =  (fitResultSlugCorIn1-fitResultSlugCorOut1)/2;
  Double_t resultSlugCorInpOut1Error  =  TMath::Sqrt(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error+fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)/2;
  Double_t resultSlugCorInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error)+1/(fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)));
//   Double_t resultSlugCorInmOut1Error  =  TMath::Sqrt(fitResultSlugCorIn1Error*fitResultSlugCorIn1Error+fitResultSlugCorOut1Error*fitResultSlugCorOut1Error)/2;


  TLegend *legSlugCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legSlugCor1->AddEntry("slugGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultSlugCorIn1,fitResultSlugCorIn1Error),"lp");
  legSlugCor1->AddEntry("slugGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultSlugCorOut1,fitResultSlugCorOut1Error),"lp");
  legSlugCor1->AddEntry("slugGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultSlugCorInpOut1,resultSlugCorInpOut1Error),"");
  legSlugCor1->AddEntry("slugGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultSlugCorInmOut1,resultSlugCorInmOut1Error),"");
  legSlugCor1->SetTextSize(0.035);
  legSlugCor1->SetFillColor(0);
  legSlugCor1->SetBorderSize(2);
  legSlugCor1->Draw();

  TLatex* tSlugGraphCorIn1=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugCorIn1Chisquare/fitResultSlugCorIn1NDF,fitResultSlugCorIn1Prob));
  tSlugGraphCorIn1->SetTextSize(0.05);
  tSlugGraphCorIn1->SetTextColor(kMagenta);
  TLatex* tSlugGraphCorOut1=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugCorOut1Chisquare/fitResultSlugCorOut1NDF,fitResultSlugCorOut1Prob));
  tSlugGraphCorOut1->SetTextSize(0.05);
  tSlugGraphCorOut1->SetTextColor(kCyan+2);
  tSlugGraphCorIn1->Draw();
  tSlugGraphCorOut1->Draw();

  gPad->Update();

  ofstream outfileSlugCor(Form("%s/plots/%s/summary%sRunSlugCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSlugCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultSlugCorOut1,fitResultSlugCorOut1Error,fitResultSlugCorIn1,fitResultSlugCorIn1Error) <<endl;
  outfileSlugCor.close();

  c10->Update();
  c10->SaveAs(Form("%s/plots/%s/%sAsymRunSlugbySlugCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /***********************************************************************************/
  /***********************************************************************************/
  if(RUN_PLOT_SLUGAVG_COR){
  TCanvas *c13 = new TCanvas("c13","c13",cx1,cy1,1200,950);
  TPad * pad130 = new TPad("pad130","pad130",ps1,ps2,ps4,ps4);
  TPad * pad131 = new TPad("pad131","pad131",ps1,ps1,ps4,ps3);
  pad130->Draw();
  pad131->Draw();
  pad130->cd();
  TText * ct130 = new TText(tll,tlr,Form("%sN to Delta %s Corrected %s Asym. using Un-Weighted Slug Avg. Sensitivity and Differences",regTitle,dataInfo,plotTitle));
  ct130->SetTextSize(tsiz);
  ct130->Draw();
  pad131->cd();
  pad131->Divide(1,2);


  pad131->cd(1);

  TGraphErrors * runGraphRegIn4 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymRegRunInVal,zeroRunIn,emdAsymRegRunInVal);
  runGraphRegIn4->SetName("runGraphRegIn4");
  runGraphRegIn4->SetMarkerColor(kRed);
  runGraphRegIn4->SetLineColor(kRed);
  runGraphRegIn4->SetMarkerStyle(21);
  runGraphRegIn4->SetMarkerSize(0.5);
  runGraphRegIn4->SetLineWidth(1);
//   runGraphRegIn4->Draw("AP");
  runGraphRegIn4->Fit("fitRunGraphRegIn4","E M R F 0 Q");
  fitRunGraphRegIn4->SetLineColor(kRed);
  fitRunGraphRegIn4->SetLineWidth(2);
  fitRunGraphRegIn4->SetLineStyle(1);

  Double_t fitResultRunRegIn4            =  fitRunGraphRegIn4->GetParameter(0);
  Double_t fitResultRunRegIn4Error       =  fitRunGraphRegIn4->GetParError(0);
  Double_t fitResultRunRegIn4Chisquare   =  fitRunGraphRegIn4->GetChisquare();
  Double_t fitResultRunRegIn4NDF         =  fitRunGraphRegIn4->GetNDF();
  Double_t fitResultRunRegIn4Prob        =  fitRunGraphRegIn4->GetProb();

  TGraphErrors * runGraphRegOut4 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymRegRunOutVal,zeroRunOut,emdAsymRegRunOutVal);
  runGraphRegOut4->SetName("runGraphRegOut4");
  runGraphRegOut4->SetMarkerColor(kBlack);
  runGraphRegOut4->SetLineColor(kBlack);
  runGraphRegOut4->SetMarkerStyle(21);
  runGraphRegOut4->SetMarkerSize(0.5);
  runGraphRegOut4->SetLineWidth(1);
//   runGraphRegOut4->Draw("AP");
  runGraphRegOut4->Fit("fitRunGraphRegOut4","E M R F 0 Q");
  fitRunGraphRegOut4->SetLineColor(kBlack);
  fitRunGraphRegOut4->SetLineWidth(2);
  fitRunGraphRegOut4->SetLineStyle(1);

  Double_t fitResultRunRegOut4           =  fitRunGraphRegOut4->GetParameter(0);
  Double_t fitResultRunRegOut4Error      =  fitRunGraphRegOut4->GetParError(0);
  Double_t fitResultRunRegOut4Chisquare  =  fitRunGraphRegOut4->GetChisquare();
  Double_t fitResultRunRegOut4NDF        =  fitRunGraphRegOut4->GetNDF();
  Double_t fitResultRunRegOut4Prob       =  fitRunGraphRegOut4->GetProb();

  TMultiGraph *runGraphReg1 = new TMultiGraph();
  runGraphReg1->Add(runGraphRegIn4);
  runGraphReg1->Add(runGraphRegOut4);
  runGraphReg1->Draw("AP");
  runGraphReg1->SetTitle("");
  runGraphReg1->GetXaxis()->SetTitle("Run Number");
  runGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  runGraphReg1->GetXaxis()->CenterTitle();
  runGraphReg1->GetYaxis()->CenterTitle();
  runGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
  runGraphReg1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunGraphReg1= runGraphReg1->GetXaxis();
  xaxisRunGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunGraphRegIn4->Draw("same");
  fitRunGraphRegOut4->Draw("same");

  TPaveStats *statsRunRegIn4 =(TPaveStats*)runGraphRegIn4->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunRegOut4=(TPaveStats*)runGraphRegOut4->GetListOfFunctions()->FindObject("stats");
  statsRunRegIn4->SetTextColor(kRed);
  statsRunRegIn4->SetFillColor(kWhite); 
  statsRunRegOut4->SetTextColor(kBlack);
  statsRunRegOut4->SetFillColor(kWhite); 
  statsRunRegIn4->SetX1NDC(x_lo_stat_in);    statsRunRegIn4->SetX2NDC(x_hi_stat_in); 
  statsRunRegIn4->SetY1NDC(y_lo_stat_in);    statsRunRegIn4->SetY2NDC(y_hi_stat_in);
  statsRunRegOut4->SetX1NDC(x_lo_stat_out);  statsRunRegOut4->SetX2NDC(x_hi_stat_out); 
  statsRunRegOut4->SetY1NDC(y_lo_stat_out);  statsRunRegOut4->SetY2NDC(y_hi_stat_out);

  Double_t resultRunRegInpOut4       =  (fitResultRunRegIn4+fitResultRunRegOut4)/2;
  Double_t resultRunRegInmOut4       =  (1/(fitResultRunRegIn4Error*fitResultRunRegIn4Error)*(fitResultRunRegIn4)-1/(fitResultRunRegOut4Error*fitResultRunRegOut4Error)*(fitResultRunRegOut4))/(1/(fitResultRunRegIn4Error*fitResultRunRegIn4Error)+1/(fitResultRunRegOut4Error*fitResultRunRegOut4Error));
//   Double_t resultRunRegInmOut4       =  (fitResultRunRegIn4-fitResultRunRegOut4)/2;
  Double_t resultRunRegInpOut4Error  =  TMath::Sqrt(fitResultRunRegIn4Error*fitResultRunRegIn4Error+fitResultRunRegOut4Error*fitResultRunRegOut4Error)/2;
  Double_t resultRunRegInmOut4Error  = 1/(TMath::Sqrt(1/(fitResultRunRegIn4Error*fitResultRunRegIn4Error)+1/(fitResultRunRegOut4Error*fitResultRunRegOut4Error)));
//   Double_t resultRunRegInmOut4Error  =  TMath::Sqrt(fitResultRunRegIn4Error*fitResultRunRegIn4Error+fitResultRunRegOut4Error*fitResultRunRegOut4Error)/2;


  TLegend *legRunReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legRunReg1->AddEntry("runGraphRegIn4",  Form("In Reg=%4.2f#pm%4.2f",fitResultRunRegIn4,fitResultRunRegIn4Error),"lp");
  legRunReg1->AddEntry("runGraphRegOut4",  Form("Out Reg=%4.2f#pm%4.2f",fitResultRunRegOut4,fitResultRunRegOut4Error),"lp");
  legRunReg1->AddEntry("runGraphRegInpOut4",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInpOut4,resultRunRegInpOut4Error),"");
  legRunReg1->AddEntry("runGraphRegInmOut4", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultRunRegInmOut4,resultRunRegInmOut4Error),"");
  legRunReg1->SetTextSize(0.035);
  legRunReg1->SetFillColor(0);
  legRunReg1->SetBorderSize(2);
  legRunReg1->Draw();

  TLatex* tRunGraphRegIn4=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegIn4Chisquare/fitResultRunRegIn4NDF,fitResultRunRegIn4Prob));
  tRunGraphRegIn4->SetTextSize(0.05);
  tRunGraphRegIn4->SetTextColor(kRed);
  TLatex* tRunGraphRegOut4=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunRegOut4Chisquare/fitResultRunRegOut4NDF,fitResultRunRegOut4Prob));
  tRunGraphRegOut4->SetTextSize(0.05);
  tRunGraphRegOut4->SetTextColor(kBlack);
  tRunGraphRegIn4->Draw();
  tRunGraphRegOut4->Draw();

  gPad->Update();


  pad131->cd(2);

  TGraphErrors * slugAvgGraphCorIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,mdAsymCorRunSlugAvgIn,zeroRunIn,emdAsymRegRunInVal);
  slugAvgGraphCorIn1->SetName("slugAvgGraphCorIn1");
  slugAvgGraphCorIn1->SetMarkerColor(kGreen);
  slugAvgGraphCorIn1->SetLineColor(kGreen);
  slugAvgGraphCorIn1->SetMarkerStyle(21);
  slugAvgGraphCorIn1->SetMarkerSize(0.5);
  slugAvgGraphCorIn1->SetLineWidth(1);
  slugAvgGraphCorIn1->Fit("fitSlugAvgGraphCorIn1","E M R F 0 Q");
  fitSlugAvgGraphCorIn1->SetLineColor(kGreen);
  fitSlugAvgGraphCorIn1->SetLineWidth(2);
  fitSlugAvgGraphCorIn1->SetLineStyle(1);

  Double_t fitResultSlugAvgCorIn1            =  fitSlugAvgGraphCorIn1->GetParameter(0);
  Double_t fitResultSlugAvgCorIn1Error       =  fitSlugAvgGraphCorIn1->GetParError(0);
  Double_t fitResultSlugAvgCorIn1Chisquare   =  fitSlugAvgGraphCorIn1->GetChisquare();
  Double_t fitResultSlugAvgCorIn1NDF         =  fitSlugAvgGraphCorIn1->GetNDF();
  Double_t fitResultSlugAvgCorIn1Prob        =  fitSlugAvgGraphCorIn1->GetProb();


  TGraphErrors * slugAvgGraphCorOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,mdAsymCorRunSlugAvgOut,zeroRunOut,emdAsymRegRunOutVal);
  slugAvgGraphCorOut1->SetName("slugAvgGraphCorOut1");
  slugAvgGraphCorOut1->SetMarkerColor(kBlue-7);
  slugAvgGraphCorOut1->SetLineColor(kBlue-7);
  slugAvgGraphCorOut1->SetMarkerStyle(21);
  slugAvgGraphCorOut1->SetMarkerSize(0.5);
  slugAvgGraphCorOut1->SetLineWidth(1);
  slugAvgGraphCorOut1->Fit("fitSlugAvgGraphCorOut1","E M R F 0 Q");
  fitSlugAvgGraphCorOut1->SetLineColor(kBlue-7);
  fitSlugAvgGraphCorOut1->SetLineWidth(2);
  fitSlugAvgGraphCorOut1->SetLineStyle(1);

  Double_t fitResultSlugAvgCorOut1           =  fitSlugAvgGraphCorOut1->GetParameter(0);
  Double_t fitResultSlugAvgCorOut1Error      =  fitSlugAvgGraphCorOut1->GetParError(0);
  Double_t fitResultSlugAvgCorOut1Chisquare  =  fitSlugAvgGraphCorOut1->GetChisquare();
  Double_t fitResultSlugAvgCorOut1NDF        =  fitSlugAvgGraphCorOut1->GetNDF();
  Double_t fitResultSlugAvgCorOut1Prob       =  fitSlugAvgGraphCorOut1->GetProb();

  TMultiGraph *slugAvgGraphCor1 = new TMultiGraph();
  slugAvgGraphCor1->Add(slugAvgGraphCorIn1);
  slugAvgGraphCor1->Add(slugAvgGraphCorOut1);
  slugAvgGraphCor1->Draw("AP");
  slugAvgGraphCor1->SetTitle("");
  slugAvgGraphCor1->GetXaxis()->SetTitle("Run Number");
  slugAvgGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Slug Avg Cor [ppm]",plotTitle));
  slugAvgGraphCor1->GetXaxis()->CenterTitle();
  slugAvgGraphCor1->GetYaxis()->CenterTitle();
  slugAvgGraphCor1->GetXaxis()->SetNdivisions(10,2,0);
  slugAvgGraphCor1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisSlugAvgGraphCor1= slugAvgGraphCor1->GetXaxis();
  xaxisSlugAvgGraphCor1->SetLimits(run_range[0],run_range[1]);
  fitSlugAvgGraphCorIn1->Draw("same");
  fitSlugAvgGraphCorOut1->Draw("same");

  TPaveStats *statsSlugAvgCorIn1 =(TPaveStats*)slugAvgGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugAvgCorOut1=(TPaveStats*)slugAvgGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugAvgCorIn1->SetTextColor(kGreen);
  statsSlugAvgCorIn1->SetFillColor(kWhite); 
  statsSlugAvgCorOut1->SetTextColor(kBlue-7);
  statsSlugAvgCorOut1->SetFillColor(kWhite); 
  statsSlugAvgCorIn1->SetX1NDC(x_lo_stat_in);    statsSlugAvgCorIn1->SetX2NDC(x_hi_stat_in); 
  statsSlugAvgCorIn1->SetY1NDC(y_lo_stat_in);    statsSlugAvgCorIn1->SetY2NDC(y_hi_stat_in);
  statsSlugAvgCorOut1->SetX1NDC(x_lo_stat_out);  statsSlugAvgCorOut1->SetX2NDC(x_hi_stat_out); 
  statsSlugAvgCorOut1->SetY1NDC(y_lo_stat_out);  statsSlugAvgCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultSlugAvgCorInpOut1       =  (fitResultSlugAvgCorIn1+fitResultSlugAvgCorOut1)/2;
  Double_t resultSlugAvgCorInmOut1       =  (((1/(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error))*(fitResultSlugAvgCorIn1))-((1/(fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error))*(fitResultSlugAvgCorOut1)))/((1/(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error))+(1/(fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error)));
//   Double_t resultSlugAvgCorInmOut1       =  (fitResultSlugAvgCorIn1-fitResultSlugAvgCorOut1)/2;
  Double_t resultSlugAvgCorInpOut1Error  =  TMath::Sqrt(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error+fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error)/2;
  Double_t resultSlugAvgCorInmOut1Error  = 1/(TMath::Sqrt((1/(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error))+(1/(fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error))));
//   Double_t resultSlugAvgCorInmOut1Error  =  TMath::Sqrt(fitResultSlugAvgCorIn1Error*fitResultSlugAvgCorIn1Error+fitResultSlugAvgCorOut1Error*fitResultSlugAvgCorOut1Error)/2;


  TLegend *legSlugAvgCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legSlugAvgCor1->AddEntry("slugAvgGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultSlugAvgCorIn1,fitResultSlugAvgCorIn1Error),"lp");
  legSlugAvgCor1->AddEntry("slugAvgGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultSlugAvgCorOut1,fitResultSlugAvgCorOut1Error),"lp");
  legSlugAvgCor1->AddEntry("slugAvgGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultSlugAvgCorInpOut1,resultSlugAvgCorInpOut1Error),"");
  legSlugAvgCor1->AddEntry("slugAvgGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultSlugAvgCorInmOut1,resultSlugAvgCorInmOut1Error),"");
  legSlugAvgCor1->SetTextSize(0.035);
  legSlugAvgCor1->SetFillColor(0);
  legSlugAvgCor1->SetBorderSize(2);
  legSlugAvgCor1->Draw();

  TLatex* tSlugAvgGraphCorIn1=new TLatex(11496,8,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugAvgCorIn1Chisquare/fitResultSlugAvgCorIn1NDF,fitResultSlugAvgCorIn1Prob));
  tSlugAvgGraphCorIn1->SetTextSize(0.05);
  tSlugAvgGraphCorIn1->SetTextColor(kGreen);
  TLatex* tSlugAvgGraphCorOut1=new TLatex(11496,6.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugAvgCorOut1Chisquare/fitResultSlugAvgCorOut1NDF,fitResultSlugAvgCorOut1Prob));
  tSlugAvgGraphCorOut1->SetTextSize(0.05);
  tSlugAvgGraphCorOut1->SetTextColor(kBlue-7);
  tSlugAvgGraphCorIn1->Draw();
  tSlugAvgGraphCorOut1->Draw();

  gPad->Update();

  ofstream outfileSlugAvgCor(Form("%s/plots/%s/summary%sRunSlugAvgCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSlugAvgCor << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f",fitResultSlugAvgCorOut1,fitResultSlugAvgCorOut1Error,fitResultSlugAvgCorIn1,fitResultSlugAvgCorIn1Error) <<endl;
  outfileSlugAvgCor.close();

  c13->Update();
  c13->SaveAs(Form("%s/plots/%s/%sAsymRunSlugAvgbySlugAvgCorrection%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  }
  /*********************************************************************************/

  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/




//   /*********************************************************************************/

//   TCanvas *c11 = new TCanvas("c11","c11",cx1,cy1,1200,950);
//   TPad * pad110 = new TPad("pad110","pad110",ps1,ps2,ps4,ps4);
//   TPad * pad111 = new TPad("pad111","pad111",ps1,ps1,ps4,ps3);
//   pad110->Draw();
//   pad111->Draw();
//   pad110->cd();
//   TText * ct110 = new TText(tll,tlr,"%sN to Delta Slug 3 and 4 Corrected %s Asymmetry using Slug Averaged Sensitivity");
//   ct110->SetTextSize(tsiz);
//   ct110->Draw();
//   pad111->cd();
//   pad111->Divide(1,2);


//   pad111->cd(1);

//   TGraphErrors * runAnaGraphRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymRegRunIn.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   runAnaGraphRegIn1->SetName("runAnaGraphRegIn1");
//   runAnaGraphRegIn1->SetMarkerColor(kRed);
//   runAnaGraphRegIn1->SetLineColor(kRed);
//   runAnaGraphRegIn1->SetMarkerStyle(21);
//   runAnaGraphRegIn1->SetMarkerSize(0.5);
//   runAnaGraphRegIn1->SetLineWidth(1);
// //   runAnaGraphRegIn1->Draw("AP");
//   runAnaGraphRegIn1->Fit("fitRunAnaGraphRegIn1","E M R F 0 Q");
//   fitRunAnaGraphRegIn1->SetLineColor(kRed);
//   fitRunAnaGraphRegIn1->SetLineWidth(2);
//   fitRunAnaGraphRegIn1->SetLineStyle(3);

//   Double_t fitResultRunAnaRegIn1            =  fitRunAnaGraphRegIn1->GetParameter(0);
//   Double_t fitResultRunAnaRegIn1Error       =  fitRunAnaGraphRegIn1->GetParError(0);
//   Double_t fitResultRunAnaRegIn1Chisquare   =  fitRunAnaGraphRegIn1->GetChisquare();
//   Double_t fitResultRunAnaRegIn1NDF         =  fitRunAnaGraphRegIn1->GetNDF();
//   Double_t fitResultRunAnaRegIn1Prob        =  fitRunAnaGraphRegIn1->GetProb();


//   TGraphErrors * runAnaGraphRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymRegRunOut.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   runAnaGraphRegOut1->SetName("runAnaGraphRegOut1");
//   runAnaGraphRegOut1->SetMarkerColor(kBlack);
//   runAnaGraphRegOut1->SetLineColor(kBlack);
//   runAnaGraphRegOut1->SetMarkerStyle(21);
//   runAnaGraphRegOut1->SetMarkerSize(0.5);
//   runAnaGraphRegOut1->SetLineWidth(1);
// //   runAnaGraphRegOut1->Draw("AP");
//   runAnaGraphRegOut1->Fit("fitRunAnaGraphRegOut1","E M R F 0 Q");
//   fitRunAnaGraphRegOut1->SetLineColor(kBlack);
//   fitRunAnaGraphRegOut1->SetLineWidth(2);
//   fitRunAnaGraphRegOut1->SetLineStyle(3);

//   Double_t fitResultRunAnaRegOut1           =  fitRunAnaGraphRegOut1->GetParameter(0);
//   Double_t fitResultRunAnaRegOut1Error      =  fitRunAnaGraphRegOut1->GetParError(0);
//   Double_t fitResultRunAnaRegOut1Chisquare  =  fitRunAnaGraphRegOut1->GetChisquare();
//   Double_t fitResultRunAnaRegOut1NDF        =  fitRunAnaGraphRegOut1->GetNDF();
//   Double_t fitResultRunAnaRegOut1Prob       =  fitRunAnaGraphRegOut1->GetProb();

//   TMultiGraph *runAnaGraphReg1 = new TMultiGraph();
//   runAnaGraphReg1->Add(runAnaGraphRegIn1);
//   runAnaGraphReg1->Add(runAnaGraphRegOut1);
//   runAnaGraphReg1->Draw("AP");
//   runAnaGraphReg1->SetTitle("");
//   runAnaGraphReg1->GetXaxis()->SetTitle("Run Number");
//   runAnaGraphReg1->GetYaxis()->SetTitle("MD Asym Reg [ppm]");
//   runAnaGraphReg1->GetXaxis()->CenterTitle();
//   runAnaGraphReg1->GetYaxis()->CenterTitle();
//   runAnaGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
//   runAnaGraphReg1->GetYaxis()->SetRangeUser(-10,10);
//   TAxis *xaxisRunAnaGraphReg1= runAnaGraphReg1->GetXaxis();
//   xaxisRunAnaGraphReg1->SetLimits(run_range[0],run_range[1]);
//   fitRunAnaGraphRegIn1->Draw("same");
//   fitRunAnaGraphRegOut1->Draw("same");

//   TPaveStats *statsRunAnaRegIn1 =(TPaveStats*)runAnaGraphRegIn1->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsRunAnaRegOut1=(TPaveStats*)runAnaGraphRegOut1->GetListOfFunctions()->FindObject("stats");
//   statsRunAnaRegIn1->SetTextColor(kRed);
//   statsRunAnaRegIn1->SetFillColor(kWhite); 
//   statsRunAnaRegOut1->SetTextColor(kBlack);
//   statsRunAnaRegOut1->SetFillColor(kWhite); 
//   statsRunAnaRegIn1->SetX1NDC(x_lo_stat_in);    statsRunAnaRegIn1->SetX2NDC(x_hi_stat_in); 
//   statsRunAnaRegIn1->SetY1NDC(y_lo_stat_in);    statsRunAnaRegIn1->SetY2NDC(y_hi_stat_in);
//   statsRunAnaRegOut1->SetX1NDC(x_lo_stat_out);  statsRunAnaRegOut1->SetX2NDC(x_hi_stat_out); 
//   statsRunAnaRegOut1->SetY1NDC(y_lo_stat_out);  statsRunAnaRegOut1->SetY2NDC(y_hi_stat_out);

//   Double_t resultRunAnaRegInpOut1       =  (fitResultRunAnaRegIn1+fitResultRunAnaRegOut1)/2;
//   Double_t resultRunAnaRegInmOut1       =  (1/(fitResultRunAnaRegIn1Error*fitResultRunAnaRegIn1Error)*(fitResultRunAnaRegIn1)-1/(fitResultRunAnaRegOut1Error*fitResultRunAnaRegOut1Error)*(fitResultRunAnaRegOut1))/(1/(fitResultRunAnaRegIn1Error*fitResultRunAnaRegIn1Error)+1/(fitResultRunAnaRegOut1Error*fitResultRunAnaRegOut1Error));
// //   Double_t resultRunAnaRegInmOut1       =  (fitResultRunAnaRegIn1-fitResultRunAnaRegOut1)/2;
//   Double_t resultRunAnaRegInpOut1Error  =  TMath::Sqrt(fitResultRunAnaRegIn1Error*fitResultRunAnaRegIn1Error+fitResultRunAnaRegOut1Error*fitResultRunAnaRegOut1Error)/2;
//   Double_t resultRunAnaRegInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunAnaRegIn1Error*fitResultRunAnaRegIn1Error)+1/(fitResultRunAnaRegOut1Error*fitResultRunAnaRegOut1Error)));
// //   Double_t resultRunAnaRegInmOut1Error  =  TMath::Sqrt(fitResultRunAnaRegIn1Error*fitResultRunAnaRegIn1Error+fitResultRunAnaRegOut1Error*fitResultRunAnaRegOut1Error)/2;


//   TLegend *legRunAnaReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
//   legRunAnaReg1->AddEntry("runAnaGraphRegIn1",  Form("In Reg= %4.2f#pm%4.2f",fitResultRunAnaRegIn1,fitResultRunAnaRegIn1Error),"lp");
//   legRunAnaReg1->AddEntry("runAnaGraphRegOut1",  Form("Out Reg= %4.2f#pm%4.2f",fitResultRunAnaRegOut1,fitResultRunAnaRegOut1Error),"lp");
//   legRunAnaReg1->AddEntry("runAnaGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg= %4.2f#pm%4.2f",resultRunAnaRegInpOut1,resultRunAnaRegInpOut1Error),"");
//   legRunAnaReg1->AddEntry("runAnaGraphRegInmOut1", Form("#frac{In-Out}{2} Reg= %4.2f#pm%4.2f",resultRunAnaRegInmOut1,resultRunAnaRegInmOut1Error),"");
//   legRunAnaReg1->SetTextSize(0.035);
//   legRunAnaReg1->SetFillColor(0);
//   legRunAnaReg1->SetBorderSize(2);
//   legRunAnaReg1->Draw();

//   TLatex* tRunGraphAnaRegIn1=new TLatex(11496,8,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunAnaRegIn1Chisquare/fitResultRunAnaRegIn1NDF,fitResultRunAnaRegIn1Prob));
//   tRunGraphAnaRegIn1->SetTextSize(0.05);
//   tRunGraphAnaRegIn1->SetTextColor(kRed);
//   TLatex* tRunGraphAnaRegOut1=new TLatex(11496,6.5,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunAnaRegOut1Chisquare/fitResultRunAnaRegOut1NDF,fitResultRunAnaRegOut1Prob));
//   tRunGraphAnaRegOut1->SetTextSize(0.05);
//   tRunGraphAnaRegOut1->SetTextColor(kBlack);
//   tRunGraphAnaRegIn1->Draw();
//   tRunGraphAnaRegOut1->Draw();

//   gPad->Update();


//   pad111->cd(2);

//   TGraphErrors * slugAnaGraphRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymUnRegRunSlugCorIn6.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   slugAnaGraphRegIn1->SetName("slugAnaGraphRegIn1");
//   slugAnaGraphRegIn1->SetMarkerColor(kMagenta);
//   slugAnaGraphRegIn1->SetLineColor(kMagenta);
//   slugAnaGraphRegIn1->SetMarkerStyle(21);
//   slugAnaGraphRegIn1->SetMarkerSize(0.5);
//   slugAnaGraphRegIn1->SetLineWidth(1);
//   slugAnaGraphRegIn1->Fit("fitSlugAnaGraphRegIn1","E M R F 0 Q");
//   fitSlugAnaGraphRegIn1->SetLineColor(kMagenta);
//   fitSlugAnaGraphRegIn1->SetLineWidth(2);
//   fitSlugAnaGraphRegIn1->SetLineStyle(1);

//   Double_t fitResultSlugAnaIn1            =  fitSlugAnaGraphRegIn1->GetParameter(0);
//   Double_t fitResultSlugAnaIn1Error       =  fitSlugAnaGraphRegIn1->GetParError(0);
//   Double_t fitResultSlugAnaIn1Chisquare   =  fitSlugAnaGraphRegIn1->GetChisquare();
//   Double_t fitResultSlugAnaIn1NDF         =  fitSlugAnaGraphRegIn1->GetNDF();
//   Double_t fitResultSlugAnaIn1Prob        =  fitSlugAnaGraphRegIn1->GetProb();


//   TGraphErrors * slugAnaGraphRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymUnRegRunSlugCorOut6.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   slugAnaGraphRegOut1->SetName("slugAnaGraphRegOut1");
//   slugAnaGraphRegOut1->SetMarkerColor(kMagenta+4);
//   slugAnaGraphRegOut1->SetLineColor(kMagenta+4);
//   slugAnaGraphRegOut1->SetMarkerStyle(21);
//   slugAnaGraphRegOut1->SetMarkerSize(0.5);
//   slugAnaGraphRegOut1->SetLineWidth(1);
//   slugAnaGraphRegOut1->Fit("fitSlugAnaGraphRegOut1","E M R F 0 Q");
//   fitSlugAnaGraphRegOut1->SetLineColor(kMagenta+4);
//   fitSlugAnaGraphRegOut1->SetLineWidth(2);
//   fitSlugAnaGraphRegOut1->SetLineStyle(1);

//   Double_t fitResultSlugAnaOut1           =  fitSlugAnaGraphRegOut1->GetParameter(0);
//   Double_t fitResultSlugAnaOut1Error      =  fitSlugAnaGraphRegOut1->GetParError(0);
//   Double_t fitResultSlugAnaOut1Chisquare  =  fitSlugAnaGraphRegOut1->GetChisquare();
//   Double_t fitResultSlugAnaOut1NDF        =  fitSlugAnaGraphRegOut1->GetNDF();
//   Double_t fitResultSlugAnaOut1Prob       =  fitSlugAnaGraphRegOut1->GetProb();

//   TMultiGraph *slugAnaGraphReg1 = new TMultiGraph();
//   slugAnaGraphReg1->Add(slugAnaGraphRegIn1);
//   slugAnaGraphReg1->Add(slugAnaGraphRegOut1);
//   slugAnaGraphReg1->Draw("AP");
//   slugAnaGraphReg1->SetTitle("");
//   slugAnaGraphReg1->GetXaxis()->SetTitle("Slug Number");
//   slugAnaGraphReg1->GetYaxis()->SetTitle("MD Asym Slug Cor [ppm]");
//   slugAnaGraphReg1->GetXaxis()->CenterTitle();
//   slugAnaGraphReg1->GetYaxis()->CenterTitle();
//   slugAnaGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
//   slugAnaGraphReg1->GetYaxis()->SetRangeUser(-10,10);
//   TAxis *xaxisSlugAnaGraphReg1= slugAnaGraphReg1->GetXaxis();
//   xaxisSlugAnaGraphReg1->SetLimits(run_range[0],run_range[1]);
//   fitSlugAnaGraphRegIn1->Draw("same");
//   fitSlugAnaGraphRegOut1->Draw("same");

//   TPaveStats *statsSlugAnaIn1 =(TPaveStats*)slugAnaGraphRegIn1->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsSlugAnaOut1=(TPaveStats*)slugAnaGraphRegOut1->GetListOfFunctions()->FindObject("stats");
//   statsSlugAnaIn1->SetTextColor(kMagenta);
//   statsSlugAnaIn1->SetFillColor(kWhite); 
//   statsSlugAnaOut1->SetTextColor(kMagenta+4);
//   statsSlugAnaOut1->SetFillColor(kWhite); 
//   statsSlugAnaIn1->SetX1NDC(x_lo_stat_in);    statsSlugAnaIn1->SetX2NDC(x_hi_stat_in); 
//   statsSlugAnaIn1->SetY1NDC(y_lo_stat_in);    statsSlugAnaIn1->SetY2NDC(y_hi_stat_in);
//   statsSlugAnaOut1->SetX1NDC(x_lo_stat_out);  statsSlugAnaOut1->SetX2NDC(x_hi_stat_out); 
//   statsSlugAnaOut1->SetY1NDC(y_lo_stat_out);  statsSlugAnaOut1->SetY2NDC(y_hi_stat_out);

//   Double_t resultSlugAnaInpOut1       =  (fitResultSlugAnaIn1+fitResultSlugAnaOut1)/2;
//   Double_t resultSlugAnaInmOut1       =  (1/(fitResultSlugAnaIn1Error*fitResultSlugAnaIn1Error)*(fitResultSlugAnaIn1)-1/(fitResultSlugAnaOut1Error*fitResultSlugAnaOut1Error)*(fitResultSlugAnaOut1))/(1/(fitResultSlugAnaIn1Error*fitResultSlugAnaIn1Error)+1/(fitResultSlugAnaOut1Error*fitResultSlugAnaOut1Error));
// //   Double_t resultSlugAnaInmOut1       =  (fitResultSlugAnaIn1-fitResultSlugAnaOut1)/2;
//   Double_t resultSlugAnaInpOut1Error  =  TMath::Sqrt(fitResultSlugAnaIn1Error*fitResultSlugAnaIn1Error+fitResultSlugAnaOut1Error*fitResultSlugAnaOut1Error)/2;
//   Double_t resultSlugAnaInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultSlugAnaIn1Error*fitResultSlugAnaIn1Error)+1/(fitResultSlugAnaOut1Error*fitResultSlugAnaOut1Error)));
// //   Double_t resultSlugAnaInmOut1Error  =  TMath::Sqrt(fitResultSlugAnaIn1Error*fitResultSlugAnaIn1Error+fitResultSlugAnaOut1Error*fitResultSlugAnaOut1Error)/2;


//   TLegend *legSlugAna1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
//   legSlugAna1->AddEntry("slugAnaGraphRegIn1",  Form("In Cor= %4.2f#pm%4.2f",fitResultSlugAnaIn1,fitResultSlugAnaIn1Error),"lp");
//   legSlugAna1->AddEntry("slugAnaGraphRegOut1",  Form("Out Cor= %4.2f#pm%4.2f",fitResultSlugAnaOut1,fitResultSlugAnaOut1Error),"lp");
//   legSlugAna1->AddEntry("slugAnaGraphRegInpOut1",  Form("#frac{In+Out}{2} Cor= %4.2f#pm%4.2f",resultSlugAnaInpOut1,resultSlugAnaInpOut1Error),"");
//   legSlugAna1->AddEntry("slugAnaGraphRegInmOut1", Form("#frac{In-Out}{2} Cor= %4.2f#pm%4.2f",resultSlugAnaInmOut1,resultSlugAnaInmOut1Error),"");
//   legSlugAna1->SetTextSize(0.035);
//   legSlugAna1->SetFillColor(0);
//   legSlugAna1->SetBorderSize(2);
//   legSlugAna1->Draw();

//   TLatex* tGraphSlugAnaIn1=new TLatex(11496,8,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugAnaIn1Chisquare/fitResultSlugAnaIn1NDF,fitResultSlugAnaIn1Prob));
//   tGraphSlugAnaIn1->SetTextSize(0.05);
//   tGraphSlugAnaIn1->SetTextColor(kMagenta);
//   TLatex* tGraphSlugAnaOut1=new TLatex(11496,6.5,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultSlugAnaOut1Chisquare/fitResultSlugAnaOut1NDF,fitResultSlugAnaOut1Prob));
//   tGraphSlugAnaOut1->SetTextSize(0.05);
//   tGraphSlugAnaOut1->SetTextColor(kMagenta+4);
//   tGraphSlugAnaIn1->Draw();
//   tGraphSlugAnaOut1->Draw();

//   gPad->Update();

//   c11->Update();
//   c11->SaveAs(Form("%s/plots/%s/n2deltaSlug34MdAllbarsAsymRunbyRunAnalysisSlugCorrected6%s.png",n2deltaAnalysisDir.Data(),regScheme));

//   /**********************************************************************************/
//   /**********************************************************************************/

//   TCanvas *c12 = new TCanvas("c12","c12",cx1,cy1,1200,950);
//   TPad * pad120 = new TPad("pad120","pad120",ps1,ps2,ps4,ps4);
//   TPad * pad121 = new TPad("pad121","pad121",ps1,ps1,ps4,ps3);
//   pad120->Draw();
//   pad121->Draw();
//   pad120->cd();
//   TText * ct120 = new TText(tll,tlr,"%sN to Delta Slug 3 and 4 %s Asymmetry using Standard DB Regression");
//   ct120->SetTextSize(tsiz);
//   ct120->Draw();
//   pad121->cd();
//   pad121->Divide(1,2);


//   pad121->cd(1);

//   TGraphErrors * runAnaGraphUnRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymUnRegRunIn.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   runAnaGraphUnRegIn1->SetName("runAnaGraphUnRegIn1");
//   runAnaGraphUnRegIn1->SetMarkerColor(kRed);
//   runAnaGraphUnRegIn1->SetLineColor(kRed);
//   runAnaGraphUnRegIn1->SetMarkerStyle(25);
//   runAnaGraphUnRegIn1->SetMarkerSize(0.5);
//   runAnaGraphUnRegIn1->SetLineWidth(1);
// //   runAnaGraphUnRegIn1->Draw("AP");
//   runAnaGraphUnRegIn1->Fit("fitRunAnaGraphUnRegIn1","E M R F 0 Q");
//   fitRunAnaGraphUnRegIn1->SetLineColor(kRed);
//   fitRunAnaGraphUnRegIn1->SetLineWidth(2);
//   fitRunAnaGraphUnRegIn1->SetLineStyle(3);

//   Double_t fitResultRunAnaUnRegIn1            =  fitRunAnaGraphUnRegIn1->GetParameter(0);
//   Double_t fitResultRunAnaUnRegIn1Error       =  fitRunAnaGraphUnRegIn1->GetParError(0);
//   Double_t fitResultRunAnaUnRegIn1Chisquare   =  fitRunAnaGraphUnRegIn1->GetChisquare();
//   Double_t fitResultRunAnaUnRegIn1NDF         =  fitRunAnaGraphUnRegIn1->GetNDF();
//   Double_t fitResultRunAnaUnRegIn1Prob        =  fitRunAnaGraphUnRegIn1->GetProb();


//   TGraphErrors * runAnaGraphUnRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymUnRegRunOut.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   runAnaGraphUnRegOut1->SetName("runAnaGraphUnRegOut1");
//   runAnaGraphUnRegOut1->SetMarkerColor(kBlack);
//   runAnaGraphUnRegOut1->SetLineColor(kBlack);
//   runAnaGraphUnRegOut1->SetMarkerStyle(25);
//   runAnaGraphUnRegOut1->SetMarkerSize(0.5);
//   runAnaGraphUnRegOut1->SetLineWidth(1);
// //   runAnaGraphUnRegOut1->Draw("AP");
//   runAnaGraphUnRegOut1->Fit("fitRunAnaGraphUnRegOut1","E M R F 0 Q");
//   fitRunAnaGraphUnRegOut1->SetLineColor(kBlack);
//   fitRunAnaGraphUnRegOut1->SetLineWidth(2);
//   fitRunAnaGraphUnRegOut1->SetLineStyle(3);

//   Double_t fitResultRunAnaUnRegOut1           =  fitRunAnaGraphUnRegOut1->GetParameter(0);
//   Double_t fitResultRunAnaUnRegOut1Error      =  fitRunAnaGraphUnRegOut1->GetParError(0);
//   Double_t fitResultRunAnaUnRegOut1Chisquare  =  fitRunAnaGraphUnRegOut1->GetChisquare();
//   Double_t fitResultRunAnaUnRegOut1NDF        =  fitRunAnaGraphUnRegOut1->GetNDF();
//   Double_t fitResultRunAnaUnRegOut1Prob       =  fitRunAnaGraphUnRegOut1->GetProb();

//   TMultiGraph *runAnaGraphUnReg1 = new TMultiGraph();
//   runAnaGraphUnReg1->Add(runAnaGraphUnRegIn1);
//   runAnaGraphUnReg1->Add(runAnaGraphUnRegOut1);
//   runAnaGraphUnReg1->Draw("AP");
//   runAnaGraphUnReg1->SetTitle("");
//   runAnaGraphUnReg1->GetXaxis()->SetTitle("Run Number");
//   runAnaGraphUnReg1->GetYaxis()->SetTitle("MD Asym UnReg [ppm]");
//   runAnaGraphUnReg1->GetXaxis()->CenterTitle();
//   runAnaGraphUnReg1->GetYaxis()->CenterTitle();
//   runAnaGraphUnReg1->GetXaxis()->SetNdivisions(10,2,0);
//   runAnaGraphUnReg1->GetYaxis()->SetRangeUser(-10,10);
//   TAxis *xaxisRunAnaGraphUnReg1= runAnaGraphUnReg1->GetXaxis();
//   xaxisRunAnaGraphUnReg1->SetLimits(run_range[0],run_range[1]);
//   fitRunAnaGraphUnRegIn1->Draw("same");
//   fitRunAnaGraphUnRegOut1->Draw("same");

//   TPaveStats *statsRunAnaUnRegIn1 =(TPaveStats*)runAnaGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsRunAnaUnRegOut1=(TPaveStats*)runAnaGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
//   statsRunAnaUnRegIn1->SetTextColor(kRed);
//   statsRunAnaUnRegIn1->SetFillColor(kWhite); 
//   statsRunAnaUnRegOut1->SetTextColor(kBlack);
//   statsRunAnaUnRegOut1->SetFillColor(kWhite); 
//   statsRunAnaUnRegIn1->SetX1NDC(x_lo_stat_in);    statsRunAnaUnRegIn1->SetX2NDC(x_hi_stat_in); 
//   statsRunAnaUnRegIn1->SetY1NDC(y_lo_stat_in);    statsRunAnaUnRegIn1->SetY2NDC(y_hi_stat_in);
//   statsRunAnaUnRegOut1->SetX1NDC(x_lo_stat_out);  statsRunAnaUnRegOut1->SetX2NDC(x_hi_stat_out); 
//   statsRunAnaUnRegOut1->SetY1NDC(y_lo_stat_out);  statsRunAnaUnRegOut1->SetY2NDC(y_hi_stat_out);

//   Double_t resultRunAnaUnRegInpOut1       =  (fitResultRunAnaUnRegIn1+fitResultRunAnaUnRegOut1)/2;
//   Double_t resultRunAnaUnRegInmOut1       =  (1/(fitResultRunAnaUnRegIn1Error*fitResultRunAnaUnRegIn1Error)*(fitResultRunAnaUnRegIn1)-1/(fitResultRunAnaUnRegOut1Error*fitResultRunAnaUnRegOut1Error)*(fitResultRunAnaUnRegOut1))/(1/(fitResultRunAnaUnRegIn1Error*fitResultRunAnaUnRegIn1Error)+1/(fitResultRunAnaUnRegOut1Error*fitResultRunAnaUnRegOut1Error));
// //   Double_t resultRunAnaUnRegInmOut1       =  (fitResultRunAnaUnRegIn1-fitResultRunAnaUnRegOut1)/2;
//   Double_t resultRunAnaUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunAnaUnRegIn1Error*fitResultRunAnaUnRegIn1Error+fitResultRunAnaUnRegOut1Error*fitResultRunAnaUnRegOut1Error)/2;
//   Double_t resultRunAnaUnRegInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunAnaUnRegIn1Error*fitResultRunAnaUnRegIn1Error)+1/(fitResultRunAnaUnRegOut1Error*fitResultRunAnaUnRegOut1Error)));
// //   Double_t resultRunAnaUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunAnaUnRegIn1Error*fitResultRunAnaUnRegIn1Error+fitResultRunAnaUnRegOut1Error*fitResultRunAnaUnRegOut1Error)/2;


//   TLegend *legRunAnaUnReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
//   legRunAnaUnReg1->AddEntry("runAnaGraphUnRegIn1",  Form("In UnReg= %4.2f#pm%4.2f",fitResultRunAnaUnRegIn1,fitResultRunAnaUnRegIn1Error),"lp");
//   legRunAnaUnReg1->AddEntry("runAnaGraphUnRegOut1",  Form("Out UnReg= %4.2f#pm%4.2f",fitResultRunAnaUnRegOut1,fitResultRunAnaUnRegOut1Error),"lp");
//   legRunAnaUnReg1->AddEntry("runAnaGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg= %4.2f#pm%4.2f",resultRunAnaUnRegInpOut1,resultRunAnaUnRegInpOut1Error),"");
//   legRunAnaUnReg1->AddEntry("runAnaGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg= %4.2f#pm%4.2f",resultRunAnaUnRegInmOut1,resultRunAnaUnRegInmOut1Error),"");
//   legRunAnaUnReg1->SetTextSize(0.035);
//   legRunAnaUnReg1->SetFillColor(0);
//   legRunAnaUnReg1->SetBorderSize(2);
//   legRunAnaUnReg1->Draw();

//   TLatex* tRunGraphAnaUnRegIn1=new TLatex(11496,8,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunAnaUnRegIn1Chisquare/fitResultRunAnaUnRegIn1NDF,fitResultRunAnaUnRegIn1Prob));
//   tRunGraphAnaUnRegIn1->SetTextSize(0.05);
//   tRunGraphAnaUnRegIn1->SetTextColor(kRed);
//   TLatex* tRunGraphAnaUnRegOut1=new TLatex(11496,6.5,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunAnaUnRegOut1Chisquare/fitResultRunAnaUnRegOut1NDF,fitResultRunAnaUnRegOut1Prob));
//   tRunGraphAnaUnRegOut1->SetTextSize(0.05);
//   tRunGraphAnaUnRegOut1->SetTextColor(kBlack);
//   tRunGraphAnaUnRegIn1->Draw();
//   tRunGraphAnaUnRegOut1->Draw();

//   gPad->Update();


//   pad121->cd(2);

//   TGraphErrors * runAnaGraphRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymRegRunIn.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   runAnaGraphRegIn1->SetName("runAnaGraphRegIn1");
//   runAnaGraphRegIn1->SetMarkerColor(kRed);
//   runAnaGraphRegIn1->SetLineColor(kRed);
//   runAnaGraphRegIn1->SetMarkerStyle(21);
//   runAnaGraphRegIn1->SetMarkerSize(0.5);
//   runAnaGraphRegIn1->SetLineWidth(1);
//   runAnaGraphRegIn1->Fit("fitRunAnaGraphRegIn1","E M R F 0 Q");
//   fitRunAnaGraphRegIn1->SetLineColor(kRed);
//   fitRunAnaGraphRegIn1->SetLineWidth(2);
//   fitRunAnaGraphRegIn1->SetLineStyle(1);

//   Double_t fitResultRunAnaIn1            =  fitRunAnaGraphRegIn1->GetParameter(0);
//   Double_t fitResultRunAnaIn1Error       =  fitRunAnaGraphRegIn1->GetParError(0);
//   Double_t fitResultRunAnaIn1Chisquare   =  fitRunAnaGraphRegIn1->GetChisquare();
//   Double_t fitResultRunAnaIn1NDF         =  fitRunAnaGraphRegIn1->GetNDF();
//   Double_t fitResultRunAnaIn1Prob        =  fitRunAnaGraphRegIn1->GetProb();


//   TGraphErrors * runAnaGraphRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testMdAllbarsAsymRegRunOut.txt",n2deltaAnalysisDir.Data(),regScheme),"%lg %lg %lg");
//   runAnaGraphRegOut1->SetName("runAnaGraphRegOut1");
//   runAnaGraphRegOut1->SetMarkerColor(kBlack);
//   runAnaGraphRegOut1->SetLineColor(kBlack);
//   runAnaGraphRegOut1->SetMarkerStyle(21);
//   runAnaGraphRegOut1->SetMarkerSize(0.5);
//   runAnaGraphRegOut1->SetLineWidth(1);
//   runAnaGraphRegOut1->Fit("fitRunAnaGraphRegOut1","E M R F 0 Q");
//   fitRunAnaGraphRegOut1->SetLineColor(kBlack);
//   fitRunAnaGraphRegOut1->SetLineWidth(2);
//   fitRunAnaGraphRegOut1->SetLineStyle(1);

//   Double_t fitResultRunAnaOut1           =  fitRunAnaGraphRegOut1->GetParameter(0);
//   Double_t fitResultRunAnaOut1Error      =  fitRunAnaGraphRegOut1->GetParError(0);
//   Double_t fitResultRunAnaOut1Chisquare  =  fitRunAnaGraphRegOut1->GetChisquare();
//   Double_t fitResultRunAnaOut1NDF        =  fitRunAnaGraphRegOut1->GetNDF();
//   Double_t fitResultRunAnaOut1Prob       =  fitRunAnaGraphRegOut1->GetProb();

//   TMultiGraph *runAnaGraphReg1 = new TMultiGraph();
//   runAnaGraphReg1->Add(runAnaGraphRegIn1);
//   runAnaGraphReg1->Add(runAnaGraphRegOut1);
//   runAnaGraphReg1->Draw("AP");
//   runAnaGraphReg1->SetTitle("");
//   runAnaGraphReg1->GetXaxis()->SetTitle("Run Number");
//   runAnaGraphReg1->GetYaxis()->SetTitle("MD Asym Reg [ppm]");
//   runAnaGraphReg1->GetXaxis()->CenterTitle();
//   runAnaGraphReg1->GetYaxis()->CenterTitle();
//   runAnaGraphReg1->GetXaxis()->SetNdivisions(10,2,0);
//   runAnaGraphReg1->GetYaxis()->SetRangeUser(-10,10);
//   TAxis *xaxisRunAnaGraphReg1= runAnaGraphReg1->GetXaxis();
//   xaxisRunAnaGraphReg1->SetLimits(run_range[0],run_range[1]);
//   fitRunAnaGraphRegIn1->Draw("same");
//   fitRunAnaGraphRegOut1->Draw("same");

//   TPaveStats *statsRunAnaIn1 =(TPaveStats*)runAnaGraphRegIn1->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsRunAnaOut1=(TPaveStats*)runAnaGraphRegOut1->GetListOfFunctions()->FindObject("stats");
//   statsRunAnaIn1->SetTextColor(kRed);
//   statsRunAnaIn1->SetFillColor(kWhite); 
//   statsRunAnaOut1->SetTextColor(kBlack);
//   statsRunAnaOut1->SetFillColor(kWhite); 
//   statsRunAnaIn1->SetX1NDC(x_lo_stat_in);    statsRunAnaIn1->SetX2NDC(x_hi_stat_in); 
//   statsRunAnaIn1->SetY1NDC(y_lo_stat_in);    statsRunAnaIn1->SetY2NDC(y_hi_stat_in);
//   statsRunAnaOut1->SetX1NDC(x_lo_stat_out);  statsRunAnaOut1->SetX2NDC(x_hi_stat_out); 
//   statsRunAnaOut1->SetY1NDC(y_lo_stat_out);  statsRunAnaOut1->SetY2NDC(y_hi_stat_out);

//   Double_t resultRunAnaInpOut1       =  (fitResultRunAnaIn1+fitResultRunAnaOut1)/2;
//   Double_t resultRunAnaInmOut1       =  (1/(fitResultRunAnaIn1Error*fitResultRunAnaIn1Error)*(fitResultRunAnaIn1)-1/(fitResultRunAnaOut1Error*fitResultRunAnaOut1Error)*(fitResultRunAnaOut1))/(1/(fitResultRunAnaIn1Error*fitResultRunAnaIn1Error)+1/(fitResultRunAnaOut1Error*fitResultRunAnaOut1Error));
// //   Double_t resultRunAnaInmOut1       =  (fitResultRunAnaIn1-fitResultRunAnaOut1)/2;
//   Double_t resultRunAnaInpOut1Error  =  TMath::Sqrt(fitResultRunAnaIn1Error*fitResultRunAnaIn1Error+fitResultRunAnaOut1Error*fitResultRunAnaOut1Error)/2;
//   Double_t resultRunAnaInmOut1Error  = 1/(TMath::Sqrt(1/(fitResultRunAnaIn1Error*fitResultRunAnaIn1Error)+1/(fitResultRunAnaOut1Error*fitResultRunAnaOut1Error)));
// //   Double_t resultRunAnaInmOut1Error  =  TMath::Sqrt(fitResultRunAnaIn1Error*fitResultRunAnaIn1Error+fitResultRunAnaOut1Error*fitResultRunAnaOut1Error)/2;


//   TLegend *legRunAna1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
//   legRunAna1->AddEntry("runAnaGraphRegIn1",  Form("In Reg= %4.2f#pm%4.2f",fitResultRunAnaIn1,fitResultRunAnaIn1Error),"lp");
//   legRunAna1->AddEntry("runAnaGraphRegOut1",  Form("Out Reg= %4.2f#pm%4.2f",fitResultRunAnaOut1,fitResultRunAnaOut1Error),"lp");
//   legRunAna1->AddEntry("runAnaGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg= %4.2f#pm%4.2f",resultRunAnaInpOut1,resultRunAnaInpOut1Error),"");
//   legRunAna1->AddEntry("runAnaGraphRegInmOut1", Form("#frac{In-Out}{2} Reg= %4.2f#pm%4.2f",resultRunAnaInmOut1,resultRunAnaInmOut1Error),"");
//   legRunAna1->SetTextSize(0.035);
//   legRunAna1->SetFillColor(0);
//   legRunAna1->SetBorderSize(2);
//   legRunAna1->Draw();

//   TLatex* tRunGraphAnaIn1=new TLatex(11496,8,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunAnaIn1Chisquare/fitResultRunAnaIn1NDF,fitResultRunAnaIn1Prob));
//   tRunGraphAnaIn1->SetTextSize(0.05);
//   tRunGraphAnaIn1->SetTextColor(kRed);
//   TLatex* tRunGraphAnaOut1=new TLatex(11496,6.5,Form("#chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultRunAnaOut1Chisquare/fitResultRunAnaOut1NDF,fitResultRunAnaOut1Prob));
//   tRunGraphAnaOut1->SetTextSize(0.05);
//   tRunGraphAnaOut1->SetTextColor(kBlack);
//   tRunGraphAnaIn1->Draw();
//   tRunGraphAnaOut1->Draw();

//   gPad->Update();

//   c12->Update();
//   c12->SaveAs(Form("%s/plots/%s/n2deltaSlug34MdAllbarsAsymRunbyRunAnalysis%s.png",n2deltaAnalysisDir.Data(),regScheme));


  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(RUNLET_SENSITIVITY_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TCanvas *c2 = new TCanvas("c2","c2",cx1,cy1,1200,950);
  TPad * pad20 = new TPad("pad20","pad20",ps1,ps2,ps4,ps4);
  TPad * pad21 = new TPad("pad21","pad21",ps1,ps1,ps4,ps3);
  pad20->Draw();
  pad21->Draw();
  pad20->cd();
  TText * ct20 = new TText(tll,tlr,Form("%sN to Delta %s %s Sensitivities",regTitle,dataInfo,plotTitle));
  ct20->SetTextSize(tsiz);
  ct20->Draw();
  pad21->cd();
  pad21->Divide(2,3);

  /*********************************************************************************/

  pad21->cd(1);

  TGraphErrors * runSenXGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,senXRegInVal,zeroRegIn,esenXRegInVal);
  runSenXGraphUnRegIn1->SetName("runSenXGraphUnRegIn1");
  runSenXGraphUnRegIn1->SetMarkerColor(kRed);
  runSenXGraphUnRegIn1->SetLineColor(kRed);
  runSenXGraphUnRegIn1->SetMarkerStyle(25);
  runSenXGraphUnRegIn1->SetMarkerSize(0.5);
  runSenXGraphUnRegIn1->SetLineWidth(1);
  runSenXGraphUnRegIn1->Fit("fitRunSenXGraphUnRegIn1","E M R F 0 Q");
  fitRunSenXGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenXGraphUnRegIn1->SetLineWidth(2);
  fitRunSenXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenXUnRegIn1            =  fitRunSenXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenXUnRegIn1Error       =  fitRunSenXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenXUnRegIn1Chisquare   =  fitRunSenXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenXUnRegIn1NDF         =  fitRunSenXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenXUnRegIn1Prob        =  fitRunSenXGraphUnRegIn1->GetProb();


  TGraphErrors * runSenXGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,senXRegOutVal,zeroRegOut,esenXRegOutVal);
  runSenXGraphUnRegOut1->SetName("runSenXGraphUnRegOut1");
  runSenXGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenXGraphUnRegOut1->SetLineColor(kBlack);
  runSenXGraphUnRegOut1->SetMarkerStyle(25);
  runSenXGraphUnRegOut1->SetMarkerSize(0.5);
  runSenXGraphUnRegOut1->SetLineWidth(1);
  runSenXGraphUnRegOut1->Fit("fitRunSenXGraphUnRegOut1","E M R F 0 Q");
  fitRunSenXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenXGraphUnRegOut1->SetLineWidth(2);
  fitRunSenXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenXUnRegOut1           =  fitRunSenXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenXUnRegOut1Error      =  fitRunSenXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenXUnRegOut1Chisquare  =  fitRunSenXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenXUnRegOut1NDF        =  fitRunSenXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenXUnRegOut1Prob       =  fitRunSenXGraphUnRegOut1->GetProb();

  TMultiGraph *runSenXGraphUnReg1 = new TMultiGraph();
  runSenXGraphUnReg1->Add(runSenXGraphUnRegIn1);
  runSenXGraphUnReg1->Add(runSenXGraphUnRegOut1);
  runSenXGraphUnReg1->Draw("AP");
  runSenXGraphUnReg1->SetTitle("");
  runSenXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenXGraphUnReg1->GetYaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenXGraphUnReg1->GetXaxis()->CenterTitle();
  runSenXGraphUnReg1->GetYaxis()->CenterTitle();
  runSenXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runSenXGraphUnReg1->GetYaxis()->SetRangeUser(-0,8000);
  TAxis *xaxisRunSenXGraphUnReg1= runSenXGraphUnReg1->GetXaxis();
  xaxisRunSenXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenXGraphUnRegIn1->Draw("same");
  fitRunSenXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenXUnRegIn1 =(TPaveStats*)runSenXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenXUnRegOut1=(TPaveStats*)runSenXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenXUnRegIn1->SetTextColor(kRed);
  statsRunSenXUnRegIn1->SetFillColor(kWhite); 
  statsRunSenXUnRegOut1->SetTextColor(kBlack);
  statsRunSenXUnRegOut1->SetFillColor(kWhite); 
  statsRunSenXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenXUnRegInpOut1       =  (fitResultRunSenXUnRegIn1+fitResultRunSenXUnRegOut1)/2;
  Double_t resultRunSenXUnRegInmOut1       =  (fitResultRunSenXUnRegIn1-fitResultRunSenXUnRegOut1)/2;
  Double_t resultRunSenXUnRegInpOut1Error  =  TMath::Sqrt(pow(fitResultRunSenXUnRegIn1Error,2)+pow(fitResultRunSenXUnRegOut1Error,2))/2;
  Double_t resultRunSenXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenXUnRegIn1Error*fitResultRunSenXUnRegIn1Error+fitResultRunSenXUnRegOut1Error*fitResultRunSenXUnRegOut1Error)/2;

  TLegend *legRunSenXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenXUnReg1->AddEntry("runSenXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenXUnRegIn1,fitResultRunSenXUnRegIn1Error),"lp");
  legRunSenXUnReg1->AddEntry("runSenXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenXUnRegOut1,fitResultRunSenXUnRegOut1Error),"lp");
//   legRunSenXUnReg1->AddEntry("runSenXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenXUnRegInpOut1,resultRunSenXUnRegInpOut1Error),"");
//   legRunSenXUnReg1->AddEntry("runSenXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenXUnRegInmOut1,resultRunSenXUnRegInmOut1Error),"");
  legRunSenXUnReg1->SetTextSize(0.045);
  legRunSenXUnReg1->SetFillColor(0);
  legRunSenXUnReg1->SetBorderSize(2);
  legRunSenXUnReg1->Draw();

  gPad->Update();


  pad21->cd(2);

  TGraphErrors * runSenXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,senXSlopeRegInVal,zeroRegIn,esenXSlopeRegInVal);
  runSenXSlopeGraphUnRegIn1->SetName("runSenXSlopeGraphUnRegIn1");
  runSenXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenXSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runSenXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenXSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenXSlopeGraphUnRegIn1->Fit("fitRunSenXSlopeGraphUnRegIn1","E M R F 0 Q");
  fitRunSenXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenXSlopeUnRegIn1            =  fitRunSenXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenXSlopeUnRegIn1Error       =  fitRunSenXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenXSlopeUnRegIn1Chisquare   =  fitRunSenXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenXSlopeUnRegIn1NDF         =  fitRunSenXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenXSlopeUnRegIn1Prob        =  fitRunSenXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,senXSlopeRegOutVal,zeroRegOut,esenXSlopeRegOutVal);
  runSenXSlopeGraphUnRegOut1->SetName("runSenXSlopeGraphUnRegOut1");
  runSenXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenXSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runSenXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenXSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenXSlopeGraphUnRegOut1->Fit("fitRunSenXSlopeGraphUnRegOut1","E M R F 0 Q");
  fitRunSenXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenXSlopeUnRegOut1           =  fitRunSenXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenXSlopeUnRegOut1Error      =  fitRunSenXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenXSlopeUnRegOut1Chisquare  =  fitRunSenXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenXSlopeUnRegOut1NDF        =  fitRunSenXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenXSlopeUnRegOut1Prob       =  fitRunSenXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenXSlopeGraphUnReg1 = new TMultiGraph();
  runSenXSlopeGraphUnReg1->Add(runSenXSlopeGraphUnRegIn1);
  runSenXSlopeGraphUnReg1->Add(runSenXSlopeGraphUnRegOut1);
  runSenXSlopeGraphUnReg1->Draw("AP");
  runSenXSlopeGraphUnReg1->SetTitle("");
  runSenXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Sensitivity [ppm/#murad]");
  runSenXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runSenXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-120,60);
  TAxis *xaxisRunSenXSlopeGraphUnReg1= runSenXSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenXSlopeGraphUnRegIn1->Draw("same");
  fitRunSenXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenXSlopeUnRegIn1 =(TPaveStats*)runSenXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenXSlopeUnRegOut1=(TPaveStats*)runSenXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunSenXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenXSlopeUnRegInpOut1       =  (fitResultRunSenXSlopeUnRegIn1+fitResultRunSenXSlopeUnRegOut1)/2;
  Double_t resultRunSenXSlopeUnRegInmOut1       =  (fitResultRunSenXSlopeUnRegIn1-fitResultRunSenXSlopeUnRegOut1)/2;
  Double_t resultRunSenXSlopeUnRegInpOut1Error  =  TMath::Sqrt(pow(fitResultRunSenXSlopeUnRegIn1Error,2)+pow(fitResultRunSenXSlopeUnRegOut1Error,2))/2;
  Double_t resultRunSenXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenXSlopeUnRegIn1Error*fitResultRunSenXSlopeUnRegIn1Error+fitResultRunSenXSlopeUnRegOut1Error*fitResultRunSenXSlopeUnRegOut1Error)/2;


  TLegend *legRunSenXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenXSlopeUnRegIn1,fitResultRunSenXSlopeUnRegIn1Error),"lp");
  legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenXSlopeUnRegOut1,fitResultRunSenXSlopeUnRegOut1Error),"lp");
//   legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenXSlopeUnRegInpOut1,resultRunSenXSlopeUnRegInpOut1Error),"");
//   legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenXSlopeUnRegInmOut1,resultRunSenXSlopeUnRegInmOut1Error),"");
  legRunSenXSlopeUnReg1->SetTextSize(0.045);
  legRunSenXSlopeUnReg1->SetFillColor(0);
  legRunSenXSlopeUnReg1->SetBorderSize(2);
  legRunSenXSlopeUnReg1->Draw();

  gPad->Update();


  pad21->cd(3);

  TGraphErrors * runSenYGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,senYRegInVal,zeroRegIn,esenYRegInVal);
  runSenYGraphUnRegIn1->SetName("runSenYGraphUnRegIn1");
  runSenYGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenYGraphUnRegIn1->SetLineColor(kGreen);
  runSenYGraphUnRegIn1->SetMarkerStyle(25);
  runSenYGraphUnRegIn1->SetMarkerSize(0.5);
  runSenYGraphUnRegIn1->SetLineWidth(1);
  runSenYGraphUnRegIn1->Fit("fitRunSenYGraphUnRegIn1","E M R F 0 Q");
  fitRunSenYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenYGraphUnRegIn1->SetLineWidth(2);
  fitRunSenYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenYUnRegIn1            =  fitRunSenYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenYUnRegIn1Error       =  fitRunSenYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenYUnRegIn1Chisquare   =  fitRunSenYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenYUnRegIn1NDF         =  fitRunSenYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenYUnRegIn1Prob        =  fitRunSenYGraphUnRegIn1->GetProb();


  TGraphErrors * runSenYGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,senYRegOutVal,zeroRegOut,esenYRegOutVal);
  runSenYGraphUnRegOut1->SetName("runSenYGraphUnRegOut1");
  runSenYGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenYGraphUnRegOut1->SetLineColor(kBlack);
  runSenYGraphUnRegOut1->SetMarkerStyle(25);
  runSenYGraphUnRegOut1->SetMarkerSize(0.5);
  runSenYGraphUnRegOut1->SetLineWidth(1);
  runSenYGraphUnRegOut1->Fit("fitRunSenYGraphUnRegOut1","E M R F 0 Q");
  fitRunSenYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenYGraphUnRegOut1->SetLineWidth(2);
  fitRunSenYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenYUnRegOut1           =  fitRunSenYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenYUnRegOut1Error      =  fitRunSenYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenYUnRegOut1Chisquare  =  fitRunSenYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenYUnRegOut1NDF        =  fitRunSenYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenYUnRegOut1Prob       =  fitRunSenYGraphUnRegOut1->GetProb();

  TMultiGraph *runSenYGraphUnReg1 = new TMultiGraph();
  runSenYGraphUnReg1->Add(runSenYGraphUnRegIn1);
  runSenYGraphUnReg1->Add(runSenYGraphUnRegOut1);
  runSenYGraphUnReg1->Draw("AP");
  runSenYGraphUnReg1->SetTitle("");
  runSenYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenYGraphUnReg1->GetYaxis()->SetTitle("Y Sensitivity [ppm/mm]");
  runSenYGraphUnReg1->GetXaxis()->CenterTitle();
  runSenYGraphUnReg1->GetYaxis()->CenterTitle();
  runSenYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runSenYGraphUnReg1->GetYaxis()->SetRangeUser(-5000,4000);
  TAxis *xaxisRunSenYGraphUnReg1= runSenYGraphUnReg1->GetXaxis();
  xaxisRunSenYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenYGraphUnRegIn1->Draw("same");
  fitRunSenYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenYUnRegIn1 =(TPaveStats*)runSenYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenYUnRegOut1=(TPaveStats*)runSenYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenYUnRegIn1->SetTextColor(kGreen);
  statsRunSenYUnRegIn1->SetFillColor(kWhite); 
  statsRunSenYUnRegOut1->SetTextColor(kBlack);
  statsRunSenYUnRegOut1->SetFillColor(kWhite); 
  statsRunSenYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenYUnRegInpOut1       =  (fitResultRunSenYUnRegIn1+fitResultRunSenYUnRegOut1)/2;
  Double_t resultRunSenYUnRegInmOut1       =  (fitResultRunSenYUnRegIn1-fitResultRunSenYUnRegOut1)/2;
  Double_t resultRunSenYUnRegInpOut1Error  =  TMath::Sqrt(pow(fitResultRunSenYUnRegIn1Error,2)+pow(fitResultRunSenYUnRegOut1Error,2))/2;
  Double_t resultRunSenYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenYUnRegIn1Error*fitResultRunSenYUnRegIn1Error+fitResultRunSenYUnRegOut1Error*fitResultRunSenYUnRegOut1Error)/2;


  TLegend *legRunSenYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenYUnReg1->AddEntry("runSenYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenYUnRegIn1,fitResultRunSenYUnRegIn1Error),"lp");
  legRunSenYUnReg1->AddEntry("runSenYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenYUnRegOut1,fitResultRunSenYUnRegOut1Error),"lp");
//   legRunSenYUnReg1->AddEntry("runSenYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenYUnRegInpOut1,resultRunSenYUnRegInpOut1Error),"");
//   legRunSenYUnReg1->AddEntry("runSenYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenYUnRegInmOut1,resultRunSenYUnRegInmOut1Error),"");
  legRunSenYUnReg1->SetTextSize(0.045);
  legRunSenYUnReg1->SetFillColor(0);
  legRunSenYUnReg1->SetBorderSize(2);
  legRunSenYUnReg1->Draw();

  gPad->Update();


  pad21->cd(4);

  TGraphErrors * runSenYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,senYSlopeRegInVal,zeroRegIn,esenYSlopeRegInVal);
  runSenYSlopeGraphUnRegIn1->SetName("runSenYSlopeGraphUnRegIn1");
  runSenYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenYSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runSenYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenYSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenYSlopeGraphUnRegIn1->Fit("fitRunSenYSlopeGraphUnRegIn1","E M R F 0 Q");
  fitRunSenYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenYSlopeUnRegIn1            =  fitRunSenYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenYSlopeUnRegIn1Error       =  fitRunSenYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenYSlopeUnRegIn1Chisquare   =  fitRunSenYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenYSlopeUnRegIn1NDF         =  fitRunSenYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenYSlopeUnRegIn1Prob        =  fitRunSenYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,senYSlopeRegOutVal,zeroRegOut,esenYSlopeRegOutVal);
  runSenYSlopeGraphUnRegOut1->SetName("runSenYSlopeGraphUnRegOut1");
  runSenYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenYSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runSenYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenYSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenYSlopeGraphUnRegOut1->Fit("fitRunSenYSlopeGraphUnRegOut1","E M R F 0 Q");
  fitRunSenYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenYSlopeUnRegOut1           =  fitRunSenYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenYSlopeUnRegOut1Error      =  fitRunSenYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenYSlopeUnRegOut1Chisquare  =  fitRunSenYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenYSlopeUnRegOut1NDF        =  fitRunSenYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenYSlopeUnRegOut1Prob       =  fitRunSenYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenYSlopeGraphUnReg1 = new TMultiGraph();
  runSenYSlopeGraphUnReg1->Add(runSenYSlopeGraphUnRegIn1);
  runSenYSlopeGraphUnReg1->Add(runSenYSlopeGraphUnRegOut1);
  runSenYSlopeGraphUnReg1->Draw("AP");
  runSenYSlopeGraphUnReg1->SetTitle("");
  runSenYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Sensitivity [ppm/#murad]");
  runSenYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runSenYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-60,120);
  TAxis *xaxisRunSenYSlopeGraphUnReg1= runSenYSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenYSlopeGraphUnRegIn1->Draw("same");
  fitRunSenYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenYSlopeUnRegIn1 =(TPaveStats*)runSenYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenYSlopeUnRegOut1=(TPaveStats*)runSenYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunSenYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenYSlopeUnRegInpOut1       =  (fitResultRunSenYSlopeUnRegIn1+fitResultRunSenYSlopeUnRegOut1)/2;
  Double_t resultRunSenYSlopeUnRegInmOut1       =  (fitResultRunSenYSlopeUnRegIn1-fitResultRunSenYSlopeUnRegOut1)/2;
  Double_t resultRunSenYSlopeUnRegInpOut1Error  =  TMath::Sqrt(pow(fitResultRunSenYSlopeUnRegIn1Error,2)+pow(fitResultRunSenYSlopeUnRegOut1Error,2))/2;
  Double_t resultRunSenYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenYSlopeUnRegIn1Error*fitResultRunSenYSlopeUnRegIn1Error+fitResultRunSenYSlopeUnRegOut1Error*fitResultRunSenYSlopeUnRegOut1Error)/2;


  TLegend *legRunSenYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenYSlopeUnRegIn1,fitResultRunSenYSlopeUnRegIn1Error),"lp");
  legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenYSlopeUnRegOut1,fitResultRunSenYSlopeUnRegOut1Error),"lp");
//   legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenYSlopeUnRegInpOut1,resultRunSenYSlopeUnRegInpOut1Error),"");
//   legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenYSlopeUnRegInmOut1,resultRunSenYSlopeUnRegInmOut1Error),"");
  legRunSenYSlopeUnReg1->SetTextSize(0.045);
  legRunSenYSlopeUnReg1->SetFillColor(0);
  legRunSenYSlopeUnReg1->SetBorderSize(2);
  legRunSenYSlopeUnReg1->Draw();

  gPad->Update();


  pad21->cd(5);

  TGraphErrors * runSenEGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,senERegInVal,zeroRegIn,esenERegInVal);
  runSenEGraphUnRegIn1->SetName("runSenEGraphUnRegIn1");
  runSenEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenEGraphUnRegIn1->SetLineColor(kBlue);
  runSenEGraphUnRegIn1->SetMarkerStyle(24);
  runSenEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenEGraphUnRegIn1->SetLineWidth(1);
//   runSenEGraphUnRegIn1->Draw("AP");
  runSenEGraphUnRegIn1->Fit("fitRunSenEGraphUnRegIn1","E M R F 0 Q");
  fitRunSenEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunSenEGraphUnRegIn1->SetLineWidth(2);
  fitRunSenEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenEUnRegIn1            =  fitRunSenEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenEUnRegIn1Error       =  fitRunSenEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenEUnRegIn1Chisquare   =  fitRunSenEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenEUnRegIn1NDF         =  fitRunSenEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenEUnRegIn1Prob        =  fitRunSenEGraphUnRegIn1->GetProb();


  TGraphErrors * runSenEGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,senERegOutVal,zeroRegOut,esenERegOutVal);
  runSenEGraphUnRegOut1->SetName("runSenEGraphUnRegOut1");
  runSenEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenEGraphUnRegOut1->SetLineColor(kBlack);
  runSenEGraphUnRegOut1->SetMarkerStyle(24);
  runSenEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenEGraphUnRegOut1->SetLineWidth(1);
//   runSenEGraphUnRegOut1->Draw("AP");
  runSenEGraphUnRegOut1->Fit("fitRunSenEGraphUnRegOut1","E M R F 0 Q");
  fitRunSenEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenEGraphUnRegOut1->SetLineWidth(2);
  fitRunSenEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenEUnRegOut1           =  fitRunSenEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenEUnRegOut1Error      =  fitRunSenEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenEUnRegOut1Chisquare  =  fitRunSenEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenEUnRegOut1NDF        =  fitRunSenEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenEUnRegOut1Prob       =  fitRunSenEGraphUnRegOut1->GetProb();

  TMultiGraph *runSenEGraphUnReg1 = new TMultiGraph();
  runSenEGraphUnReg1->Add(runSenEGraphUnRegIn1);
  runSenEGraphUnReg1->Add(runSenEGraphUnRegOut1);
  runSenEGraphUnReg1->Draw("AP");
  runSenEGraphUnReg1->SetTitle("");
  runSenEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenEGraphUnReg1->GetYaxis()->SetTitle("E Sensitivity [ppm]");
  runSenEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenEGraphUnReg1->GetYaxis()->CenterTitle();
  runSenEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runSenEGraphUnReg1->GetYaxis()->SetRangeUser(-10,10);
  TAxis *xaxisRunSenEGraphUnReg1= runSenEGraphUnReg1->GetXaxis();
  xaxisRunSenEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenEGraphUnRegIn1->Draw("same");
  fitRunSenEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenEUnRegIn1 =(TPaveStats*)runSenEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenEUnRegOut1=(TPaveStats*)runSenEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenEUnRegIn1->SetTextColor(kBlue);
  statsRunSenEUnRegIn1->SetFillColor(kWhite); 
  statsRunSenEUnRegOut1->SetTextColor(kBlack);
  statsRunSenEUnRegOut1->SetFillColor(kWhite); 
  statsRunSenEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenEUnRegInpOut1       =  (fitResultRunSenEUnRegIn1+fitResultRunSenEUnRegOut1)/2;
  Double_t resultRunSenEUnRegInmOut1       =  (fitResultRunSenEUnRegIn1-fitResultRunSenEUnRegOut1)/2;
  Double_t resultRunSenEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenEUnRegIn1Error*fitResultRunSenEUnRegIn1Error+fitResultRunSenEUnRegOut1Error*fitResultRunSenEUnRegOut1Error)/2;
  Double_t resultRunSenEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenEUnRegIn1Error*fitResultRunSenEUnRegIn1Error+fitResultRunSenEUnRegOut1Error*fitResultRunSenEUnRegOut1Error)/2;


  TLegend *legRunSenEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenEUnReg1->AddEntry("runSenEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenEUnRegIn1,fitResultRunSenEUnRegIn1Error),"lp");
  legRunSenEUnReg1->AddEntry("runSenEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenEUnRegOut1,fitResultRunSenEUnRegOut1Error),"lp");
//   legRunSenEUnReg1->AddEntry("runSenEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenEUnRegInpOut1,resultRunSenEUnRegInpOut1Error),"");
//   legRunSenEUnReg1->AddEntry("runSenEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenEUnRegInmOut1,resultRunSenEUnRegInmOut1Error),"");
  legRunSenEUnReg1->SetTextSize(0.045);
  legRunSenEUnReg1->SetFillColor(0);
  legRunSenEUnReg1->SetBorderSize(2);
  legRunSenEUnReg1->Draw();

  gPad->Update();


  pad21->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,chargeRegInVal,zeroUnRegIn,echargeRegInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,chargeRegOutVal,zeroUnRegOut,echargeRegOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-20,20);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.045);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();


  ofstream outfileSenAvgIn(Form("%s/plots/%s/summary%sRunletSenAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSenAvgIn << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f"
			  ,fitResultRunSenXUnRegIn1,fitResultRunSenXUnRegIn1Error
			  ,fitResultRunSenXSlopeUnRegIn1,fitResultRunSenXSlopeUnRegIn1Error
			  ,fitResultRunSenYUnRegIn1,fitResultRunSenYUnRegIn1Error
			  ,fitResultRunSenYSlopeUnRegIn1,fitResultRunSenYSlopeUnRegIn1Error
			  ,fitResultRunSenEUnRegIn1,fitResultRunSenEUnRegIn1Error
			  ,fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error) 
		  <<endl;
  outfileSenAvgIn.close();

  ofstream outfileSenAvgOut(Form("%s/plots/%s/summary%sRunletSenAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSenAvgOut << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f"
			  ,fitResultRunSenXUnRegOut1,fitResultRunSenXUnRegOut1Error
			  ,fitResultRunSenXSlopeUnRegOut1,fitResultRunSenXSlopeUnRegOut1Error
			  ,fitResultRunSenYUnRegOut1,fitResultRunSenYUnRegOut1Error
			  ,fitResultRunSenYSlopeUnRegOut1,fitResultRunSenYSlopeUnRegOut1Error
			  ,fitResultRunSenEUnRegOut1,fitResultRunSenEUnRegOut1Error
			  ,fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error) 
		  <<endl;
  outfileSenAvgOut.close();

  c2->Update();
  c2->SaveAs(Form("%s/plots/%s/%sSensitivityRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c4 = new TCanvas("c4","c4",cx1,cy1,1200,950);
  TPad * pad40 = new TPad("pad40","pad40",ps1,ps2,ps4,ps4);
  TPad * pad41 = new TPad("pad41","pad41",ps1,ps1,ps4,ps3);
  pad40->Draw();
  pad41->Draw();
  pad40->cd();
  TText * ct40 = new TText(tll,tlr,Form("%sN to Delta %s Corrections using %s Runlet Sensitivities and Differences",regTitle,dataInfo,plotTitle));
  ct40->SetTextSize(tsiz);
  ct40->Draw();
  pad41->cd();
  pad41->Divide(2,3);


  pad41->cd(1);

  TGraphErrors * runSenCorrectionXGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corRunletXRegInVal,zeroRegIn,ecorRunletXRegInVal);
  runSenCorrectionXGraphUnRegIn1->SetName("runSenCorrectionXGraphUnRegIn1");
  runSenCorrectionXGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  runSenCorrectionXGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorrectionXGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionXGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionXGraphUnRegIn1->Fit("fitRunSenCorrectionXGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenCorrectionXGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXUnRegIn1            =  fitRunSenCorrectionXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXUnRegIn1Error       =  fitRunSenCorrectionXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionXUnRegIn1Chisquare   =  fitRunSenCorrectionXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionXUnRegIn1NDF         =  fitRunSenCorrectionXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionXUnRegIn1Prob        =  fitRunSenCorrectionXGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionXGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corRunletXRegOutVal,zeroRegOut,ecorRunletXRegOutVal);
  runSenCorrectionXGraphUnRegOut1->SetName("runSenCorrectionXGraphUnRegOut1");
  runSenCorrectionXGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionXGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorrectionXGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionXGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionXGraphUnRegOut1->Fit("fitRunSenCorrectionXGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionXGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXUnRegOut1           =  fitRunSenCorrectionXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXUnRegOut1Error      =  fitRunSenCorrectionXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionXUnRegOut1Chisquare  =  fitRunSenCorrectionXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionXUnRegOut1NDF        =  fitRunSenCorrectionXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionXUnRegOut1Prob       =  fitRunSenCorrectionXGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionXGraphUnReg1 = new TMultiGraph();
  runSenCorrectionXGraphUnReg1->Add(runSenCorrectionXGraphUnRegIn1);
  runSenCorrectionXGraphUnReg1->Add(runSenCorrectionXGraphUnRegOut1);
  runSenCorrectionXGraphUnReg1->Draw("AP");
  runSenCorrectionXGraphUnReg1->SetTitle("");
  runSenCorrectionXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionXGraphUnReg1->GetYaxis()->SetTitle("X Correction [ppm]");
  runSenCorrectionXGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionXGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionXGraphUnReg1->GetYaxis()->SetRangeUser(3,10);
  TAxis *xaxisRunSenCorrectionXGraphUnReg1= runSenCorrectionXGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionXGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionXUnRegIn1 =(TPaveStats*)runSenCorrectionXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionXUnRegOut1=(TPaveStats*)runSenCorrectionXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionXUnRegIn1->SetTextColor(kRed);
  statsRunSenCorrectionXUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionXUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionXUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionXUnRegInpOut1       =  (fitResultRunSenCorrectionXUnRegIn1+fitResultRunSenCorrectionXUnRegOut1)/2;
  Double_t resultRunSenCorrectionXUnRegInmOut1       =  (fitResultRunSenCorrectionXUnRegIn1-fitResultRunSenCorrectionXUnRegOut1)/2;
  Double_t resultRunSenCorrectionXUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXUnRegIn1Error*fitResultRunSenCorrectionXUnRegIn1Error+fitResultRunSenCorrectionXUnRegOut1Error*fitResultRunSenCorrectionXUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXUnRegIn1Error*fitResultRunSenCorrectionXUnRegIn1Error+fitResultRunSenCorrectionXUnRegOut1Error*fitResultRunSenCorrectionXUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionXUnRegIn1,fitResultRunSenCorrectionXUnRegIn1Error),"lp");
  legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionXUnRegOut1,fitResultRunSenCorrectionXUnRegOut1Error),"lp");
//   legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionXUnRegInpOut1,resultRunSenCorrectionXUnRegInpOut1Error),"");
//   legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionXUnRegInmOut1,resultRunSenCorrectionXUnRegInmOut1Error),"");
  legRunSenCorrectionXUnReg1->SetTextSize(0.045);
  legRunSenCorrectionXUnReg1->SetFillColor(0);
  legRunSenCorrectionXUnReg1->SetBorderSize(2);
  legRunSenCorrectionXUnReg1->Draw();

  gPad->Update();


  pad41->cd(2);

  TGraphErrors * runSenCorrectionXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corRunletXSlopeRegInVal,zeroRegIn,ecorRunletXSlopeRegInVal);
  runSenCorrectionXSlopeGraphUnRegIn1->SetName("runSenCorrectionXSlopeGraphUnRegIn1");
  runSenCorrectionXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenCorrectionXSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runSenCorrectionXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionXSlopeGraphUnRegIn1->Fit("fitRunSenCorrectionXSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1            =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1Error       =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1Chisquare   =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1NDF         =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1Prob        =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corRunletXSlopeRegOutVal,zeroRegOut,ecorRunletXSlopeRegOutVal);
  runSenCorrectionXSlopeGraphUnRegOut1->SetName("runSenCorrectionXSlopeGraphUnRegOut1");
  runSenCorrectionXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionXSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runSenCorrectionXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionXSlopeGraphUnRegOut1->Fit("fitRunSenCorrectionXSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1           =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1Error      =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1Chisquare  =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1NDF        =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1Prob       =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionXSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorrectionXSlopeGraphUnReg1->Add(runSenCorrectionXSlopeGraphUnRegIn1);
  runSenCorrectionXSlopeGraphUnReg1->Add(runSenCorrectionXSlopeGraphUnRegOut1);
  runSenCorrectionXSlopeGraphUnReg1->Draw("AP");
  runSenCorrectionXSlopeGraphUnReg1->SetTitle("");
  runSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Correction [ppm]");
  runSenCorrectionXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-7,0);
  TAxis *xaxisRunSenCorrectionXSlopeGraphUnReg1= runSenCorrectionXSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionXSlopeGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionXSlopeUnRegIn1 =(TPaveStats*)runSenCorrectionXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionXSlopeUnRegOut1=(TPaveStats*)runSenCorrectionXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunSenCorrectionXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionXSlopeUnRegInpOut1       =  (fitResultRunSenCorrectionXSlopeUnRegIn1+fitResultRunSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionXSlopeUnRegInmOut1       =  (fitResultRunSenCorrectionXSlopeUnRegIn1-fitResultRunSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXSlopeUnRegIn1Error*fitResultRunSenCorrectionXSlopeUnRegIn1Error+fitResultRunSenCorrectionXSlopeUnRegOut1Error*fitResultRunSenCorrectionXSlopeUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXSlopeUnRegIn1Error*fitResultRunSenCorrectionXSlopeUnRegIn1Error+fitResultRunSenCorrectionXSlopeUnRegOut1Error*fitResultRunSenCorrectionXSlopeUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionXSlopeUnRegIn1,fitResultRunSenCorrectionXSlopeUnRegIn1Error),"lp");
  legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionXSlopeUnRegOut1,fitResultRunSenCorrectionXSlopeUnRegOut1Error),"lp");
//   legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionXSlopeUnRegInpOut1,resultRunSenCorrectionXSlopeUnRegInpOut1Error),"");
//   legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionXSlopeUnRegInmOut1,resultRunSenCorrectionXSlopeUnRegInmOut1Error),"");
  legRunSenCorrectionXSlopeUnReg1->SetTextSize(0.045);
  legRunSenCorrectionXSlopeUnReg1->SetFillColor(0);
  legRunSenCorrectionXSlopeUnReg1->SetBorderSize(2);
  legRunSenCorrectionXSlopeUnReg1->Draw();

  gPad->Update();


  pad41->cd(3);

  TGraphErrors * runSenCorrectionYGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corRunletYRegInVal,zeroRegIn,ecorRunletYRegInVal);
  runSenCorrectionYGraphUnRegIn1->SetName("runSenCorrectionYGraphUnRegIn1");
  runSenCorrectionYGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorrectionYGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorrectionYGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionYGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionYGraphUnRegIn1->Fit("fitRunSenCorrectionYGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenCorrectionYGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYUnRegIn1            =  fitRunSenCorrectionYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYUnRegIn1Error       =  fitRunSenCorrectionYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionYUnRegIn1Chisquare   =  fitRunSenCorrectionYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionYUnRegIn1NDF         =  fitRunSenCorrectionYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionYUnRegIn1Prob        =  fitRunSenCorrectionYGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionYGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corRunletYRegOutVal,zeroRegOut,ecorRunletYRegOutVal);
  runSenCorrectionYGraphUnRegOut1->SetName("runSenCorrectionYGraphUnRegOut1");
  runSenCorrectionYGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionYGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorrectionYGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionYGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionYGraphUnRegOut1->Fit("fitRunSenCorrectionYGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionYGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYUnRegOut1           =  fitRunSenCorrectionYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYUnRegOut1Error      =  fitRunSenCorrectionYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionYUnRegOut1Chisquare  =  fitRunSenCorrectionYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionYUnRegOut1NDF        =  fitRunSenCorrectionYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionYUnRegOut1Prob       =  fitRunSenCorrectionYGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionYGraphUnReg1 = new TMultiGraph();
  runSenCorrectionYGraphUnReg1->Add(runSenCorrectionYGraphUnRegIn1);
  runSenCorrectionYGraphUnReg1->Add(runSenCorrectionYGraphUnRegOut1);
  runSenCorrectionYGraphUnReg1->Draw("AP");
  runSenCorrectionYGraphUnReg1->SetTitle("");
  runSenCorrectionYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionYGraphUnReg1->GetYaxis()->SetTitle("Y Correction [ppm]");
  runSenCorrectionYGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionYGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionYGraphUnReg1->GetYaxis()->SetRangeUser(-0,7);
  TAxis *xaxisRunSenCorrectionYGraphUnReg1= runSenCorrectionYGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionYGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionYUnRegIn1 =(TPaveStats*)runSenCorrectionYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionYUnRegOut1=(TPaveStats*)runSenCorrectionYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionYUnRegIn1->SetTextColor(kGreen);
  statsRunSenCorrectionYUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionYUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionYUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionYUnRegInpOut1       =  (fitResultRunSenCorrectionYUnRegIn1+fitResultRunSenCorrectionYUnRegOut1)/2;
  Double_t resultRunSenCorrectionYUnRegInmOut1       =  (fitResultRunSenCorrectionYUnRegIn1-fitResultRunSenCorrectionYUnRegOut1)/2;
  Double_t resultRunSenCorrectionYUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYUnRegIn1Error*fitResultRunSenCorrectionYUnRegIn1Error+fitResultRunSenCorrectionYUnRegOut1Error*fitResultRunSenCorrectionYUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYUnRegIn1Error*fitResultRunSenCorrectionYUnRegIn1Error+fitResultRunSenCorrectionYUnRegOut1Error*fitResultRunSenCorrectionYUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionYUnRegIn1,fitResultRunSenCorrectionYUnRegIn1Error),"lp");
  legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionYUnRegOut1,fitResultRunSenCorrectionYUnRegOut1Error),"lp");
//   legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionYUnRegInpOut1,resultRunSenCorrectionYUnRegInpOut1Error),"");
//   legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionYUnRegInmOut1,resultRunSenCorrectionYUnRegInmOut1Error),"");
  legRunSenCorrectionYUnReg1->SetTextSize(0.045);
  legRunSenCorrectionYUnReg1->SetFillColor(0);
  legRunSenCorrectionYUnReg1->SetBorderSize(2);
  legRunSenCorrectionYUnReg1->Draw();

  gPad->Update();


  pad41->cd(4);

  TGraphErrors * runSenCorrectionYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corRunletYSlopeRegInVal,zeroRegIn,ecorRunletYSlopeRegInVal);
  runSenCorrectionYSlopeGraphUnRegIn1->SetName("runSenCorrectionYSlopeGraphUnRegIn1");
  runSenCorrectionYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorrectionYSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runSenCorrectionYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionYSlopeGraphUnRegIn1->Fit("fitRunSenCorrectionYSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1            =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1Error       =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1Chisquare   =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1NDF         =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1Prob        =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corRunletYSlopeRegOutVal,zeroRegOut,ecorRunletYSlopeRegOutVal);
  runSenCorrectionYSlopeGraphUnRegOut1->SetName("runSenCorrectionYSlopeGraphUnRegOut1");
  runSenCorrectionYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionYSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runSenCorrectionYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionYSlopeGraphUnRegOut1->Fit("fitRunSenCorrectionYSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1           =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1Error      =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1Chisquare  =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1NDF        =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1Prob       =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionYSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorrectionYSlopeGraphUnReg1->Add(runSenCorrectionYSlopeGraphUnRegIn1);
  runSenCorrectionYSlopeGraphUnReg1->Add(runSenCorrectionYSlopeGraphUnRegOut1);
  runSenCorrectionYSlopeGraphUnReg1->Draw("AP");
  runSenCorrectionYSlopeGraphUnReg1->SetTitle("");
  runSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  runSenCorrectionYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-3.5,3.5);
  TAxis *xaxisRunSenCorrectionYSlopeGraphUnReg1= runSenCorrectionYSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionYSlopeGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionYSlopeUnRegIn1 =(TPaveStats*)runSenCorrectionYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionYSlopeUnRegOut1=(TPaveStats*)runSenCorrectionYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunSenCorrectionYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionYSlopeUnRegInpOut1       =  (fitResultRunSenCorrectionYSlopeUnRegIn1+fitResultRunSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionYSlopeUnRegInmOut1       =  (fitResultRunSenCorrectionYSlopeUnRegIn1-fitResultRunSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYSlopeUnRegIn1Error*fitResultRunSenCorrectionYSlopeUnRegIn1Error+fitResultRunSenCorrectionYSlopeUnRegOut1Error*fitResultRunSenCorrectionYSlopeUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYSlopeUnRegIn1Error*fitResultRunSenCorrectionYSlopeUnRegIn1Error+fitResultRunSenCorrectionYSlopeUnRegOut1Error*fitResultRunSenCorrectionYSlopeUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionYSlopeUnRegIn1,fitResultRunSenCorrectionYSlopeUnRegIn1Error),"lp");
  legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionYSlopeUnRegOut1,fitResultRunSenCorrectionYSlopeUnRegOut1Error),"lp");
//   legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionYSlopeUnRegInpOut1,resultRunSenCorrectionYSlopeUnRegInpOut1Error),"");
//   legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionYSlopeUnRegInmOut1,resultRunSenCorrectionYSlopeUnRegInmOut1Error),"");
  legRunSenCorrectionYSlopeUnReg1->SetTextSize(0.045);
  legRunSenCorrectionYSlopeUnReg1->SetFillColor(0);
  legRunSenCorrectionYSlopeUnReg1->SetBorderSize(2);
  legRunSenCorrectionYSlopeUnReg1->Draw();

  gPad->Update();


  pad41->cd(5);

  TGraphErrors * runSenCorrectionEGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corRunletERegInVal,zeroRegIn,ecorRunletERegInVal);
  runSenCorrectionEGraphUnRegIn1->SetName("runSenCorrectionEGraphUnRegIn1");
  runSenCorrectionEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  runSenCorrectionEGraphUnRegIn1->SetMarkerStyle(20);
  runSenCorrectionEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionEGraphUnRegIn1->SetLineWidth(1);
//   runSenCorrectionEGraphUnRegIn1->Draw("AP");
  runSenCorrectionEGraphUnRegIn1->Fit("fitRunSenCorrectionEGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunSenCorrectionEGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionEUnRegIn1            =  fitRunSenCorrectionEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionEUnRegIn1Error       =  fitRunSenCorrectionEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionEUnRegIn1Chisquare   =  fitRunSenCorrectionEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionEUnRegIn1NDF         =  fitRunSenCorrectionEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionEUnRegIn1Prob        =  fitRunSenCorrectionEGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionEGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corRunletERegOutVal,zeroRegOut,ecorRunletERegOutVal);
  runSenCorrectionEGraphUnRegOut1->SetName("runSenCorrectionEGraphUnRegOut1");
  runSenCorrectionEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionEGraphUnRegOut1->SetMarkerStyle(20);
  runSenCorrectionEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionEGraphUnRegOut1->SetLineWidth(1);
//   runSenCorrectionEGraphUnRegOut1->Draw("AP");
  runSenCorrectionEGraphUnRegOut1->Fit("fitRunSenCorrectionEGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionEGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionEUnRegOut1           =  fitRunSenCorrectionEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionEUnRegOut1Error      =  fitRunSenCorrectionEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionEUnRegOut1Chisquare  =  fitRunSenCorrectionEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionEUnRegOut1NDF        =  fitRunSenCorrectionEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionEUnRegOut1Prob       =  fitRunSenCorrectionEGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionEGraphUnReg1 = new TMultiGraph();
  runSenCorrectionEGraphUnReg1->Add(runSenCorrectionEGraphUnRegIn1);
  runSenCorrectionEGraphUnReg1->Add(runSenCorrectionEGraphUnRegOut1);
  runSenCorrectionEGraphUnReg1->Draw("AP");
  runSenCorrectionEGraphUnReg1->SetTitle("");
  runSenCorrectionEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  runSenCorrectionEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionEGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionEGraphUnReg1->GetYaxis()->SetRangeUser(-3.5,3.5);
  TAxis *xaxisRunSenCorrectionEGraphUnReg1= runSenCorrectionEGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionEGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionEUnRegIn1 =(TPaveStats*)runSenCorrectionEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionEUnRegOut1=(TPaveStats*)runSenCorrectionEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionEUnRegIn1->SetTextColor(kBlue);
  statsRunSenCorrectionEUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionEUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionEUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionEUnRegInpOut1       =  (fitResultRunSenCorrectionEUnRegIn1+fitResultRunSenCorrectionEUnRegOut1)/2;
  Double_t resultRunSenCorrectionEUnRegInmOut1       =  (fitResultRunSenCorrectionEUnRegIn1-fitResultRunSenCorrectionEUnRegOut1)/2;
  Double_t resultRunSenCorrectionEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionEUnRegIn1Error*fitResultRunSenCorrectionEUnRegIn1Error+fitResultRunSenCorrectionEUnRegOut1Error*fitResultRunSenCorrectionEUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionEUnRegIn1Error*fitResultRunSenCorrectionEUnRegIn1Error+fitResultRunSenCorrectionEUnRegOut1Error*fitResultRunSenCorrectionEUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionEUnRegIn1,fitResultRunSenCorrectionEUnRegIn1Error),"lp");
  legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionEUnRegOut1,fitResultRunSenCorrectionEUnRegOut1Error),"lp");
//   legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionEUnRegInpOut1,resultRunSenCorrectionEUnRegInpOut1Error),"");
//   legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionEUnRegInmOut1,resultRunSenCorrectionEUnRegInmOut1Error),"");
  legRunSenCorrectionEUnReg1->SetTextSize(0.045);
  legRunSenCorrectionEUnReg1->SetFillColor(0);
  legRunSenCorrectionEUnReg1->SetBorderSize(2);
  legRunSenCorrectionEUnReg1->Draw();

  gPad->Update();


  pad41->cd(6);

  TGraphErrors * runSenCorrectionTotalGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corRunletTotalRegInVal,zeroRegIn,ecorRunletTotalRegInVal);
  runSenCorrectionTotalGraphUnRegIn1->SetName("runSenCorrectionTotalGraphUnRegIn1");
  runSenCorrectionTotalGraphUnRegIn1->SetMarkerColor(kCyan);
  runSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  runSenCorrectionTotalGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorrectionTotalGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionTotalGraphUnRegIn1->SetLineWidth(1);
//   runSenCorrectionTotalGraphUnRegIn1->Draw("AP");
  runSenCorrectionTotalGraphUnRegIn1->Fit("fitRunSenCorrectionTotalGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  fitRunSenCorrectionTotalGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionTotalGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionTotalUnRegIn1            =  fitRunSenCorrectionTotalGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionTotalUnRegIn1Error       =  fitRunSenCorrectionTotalGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionTotalUnRegIn1Chisquare   =  fitRunSenCorrectionTotalGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionTotalUnRegIn1NDF         =  fitRunSenCorrectionTotalGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionTotalUnRegIn1Prob        =  fitRunSenCorrectionTotalGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionTotalGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corRunletTotalRegOutVal,zeroRegOut,ecorRunletTotalRegOutVal);
  runSenCorrectionTotalGraphUnRegOut1->SetName("runSenCorrectionTotalGraphUnRegOut1");
  runSenCorrectionTotalGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionTotalGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorrectionTotalGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionTotalGraphUnRegOut1->SetLineWidth(1);
//   runSenCorrectionTotalGraphUnRegOut1->Draw("AP");
  runSenCorrectionTotalGraphUnRegOut1->Fit("fitRunSenCorrectionTotalGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionTotalGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionTotalGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionTotalUnRegOut1           =  fitRunSenCorrectionTotalGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionTotalUnRegOut1Error      =  fitRunSenCorrectionTotalGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionTotalUnRegOut1Chisquare  =  fitRunSenCorrectionTotalGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionTotalUnRegOut1NDF        =  fitRunSenCorrectionTotalGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionTotalUnRegOut1Prob       =  fitRunSenCorrectionTotalGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionTotalGraphUnReg1 = new TMultiGraph();
  runSenCorrectionTotalGraphUnReg1->Add(runSenCorrectionTotalGraphUnRegIn1);
  runSenCorrectionTotalGraphUnReg1->Add(runSenCorrectionTotalGraphUnRegOut1);
  runSenCorrectionTotalGraphUnReg1->Draw("AP");
  runSenCorrectionTotalGraphUnReg1->SetTitle("");
  runSenCorrectionTotalGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionTotalGraphUnReg1->GetYaxis()->SetTitle("Total Correction [ppm]");
  runSenCorrectionTotalGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionTotalGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionTotalGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionTotalGraphUnReg1->GetYaxis()->SetRangeUser(2,9);
  TAxis *xaxisRunSenCorrectionTotalGraphUnReg1= runSenCorrectionTotalGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionTotalGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionTotalGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionTotalGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionTotalUnRegIn1 =(TPaveStats*)runSenCorrectionTotalGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionTotalUnRegOut1=(TPaveStats*)runSenCorrectionTotalGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionTotalUnRegIn1->SetTextColor(kCyan);
  statsRunSenCorrectionTotalUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionTotalUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionTotalUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionTotalUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionTotalUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionTotalUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionTotalUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionTotalUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionTotalUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionTotalUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionTotalUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionTotalUnRegInpOut1       =  (fitResultRunSenCorrectionTotalUnRegIn1+fitResultRunSenCorrectionTotalUnRegOut1)/2;
  Double_t resultRunSenCorrectionTotalUnRegInmOut1       =  (fitResultRunSenCorrectionTotalUnRegIn1-fitResultRunSenCorrectionTotalUnRegOut1)/2;
  Double_t resultRunSenCorrectionTotalUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionTotalUnRegIn1Error*fitResultRunSenCorrectionTotalUnRegIn1Error+fitResultRunSenCorrectionTotalUnRegOut1Error*fitResultRunSenCorrectionTotalUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionTotalUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionTotalUnRegIn1Error*fitResultRunSenCorrectionTotalUnRegIn1Error+fitResultRunSenCorrectionTotalUnRegOut1Error*fitResultRunSenCorrectionTotalUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionTotalUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionTotalUnRegIn1,fitResultRunSenCorrectionTotalUnRegIn1Error),"lp");
  legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionTotalUnRegOut1,fitResultRunSenCorrectionTotalUnRegOut1Error),"lp");
//   legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionTotalUnRegInpOut1,resultRunSenCorrectionTotalUnRegInpOut1Error),"");
//   legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionTotalUnRegInmOut1,resultRunSenCorrectionTotalUnRegInmOut1Error),"");
  legRunSenCorrectionTotalUnReg1->SetTextSize(0.045);
  legRunSenCorrectionTotalUnReg1->SetFillColor(0);
  legRunSenCorrectionTotalUnReg1->SetBorderSize(2);
  legRunSenCorrectionTotalUnReg1->Draw();

  gPad->Update();

  ofstream outfileCorrectionAvgIn(Form("%s/plots/%s/summary%sRunletCorrectionAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileCorrectionAvgIn << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f"
				 ,fitResultRunSenCorrectionXUnRegIn1,fitResultRunSenCorrectionXUnRegIn1Error
				 ,fitResultRunSenCorrectionXSlopeUnRegIn1,fitResultRunSenCorrectionXSlopeUnRegIn1Error
				 ,fitResultRunSenCorrectionYUnRegIn1,fitResultRunSenCorrectionYUnRegIn1Error
				 ,fitResultRunSenCorrectionYSlopeUnRegIn1,fitResultRunSenCorrectionYSlopeUnRegIn1Error
				 ,fitResultRunSenCorrectionEUnRegIn1,fitResultRunSenCorrectionEUnRegIn1Error
				 ,fitResultRunSenCorrectionTotalUnRegIn1,fitResultRunSenCorrectionTotalUnRegIn1Error)
			 <<endl;
  outfileCorrectionAvgIn.close();

  ofstream outfileCorrectionAvgOut(Form("%s/plots/%s/summary%sRunletCorrectionAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileCorrectionAvgOut << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f"
				 ,fitResultRunSenCorrectionXUnRegOut1,fitResultRunSenCorrectionXUnRegOut1Error
				 ,fitResultRunSenCorrectionXSlopeUnRegOut1,fitResultRunSenCorrectionXSlopeUnRegOut1Error
				 ,fitResultRunSenCorrectionYUnRegOut1,fitResultRunSenCorrectionYUnRegOut1Error
				 ,fitResultRunSenCorrectionYSlopeUnRegOut1,fitResultRunSenCorrectionYSlopeUnRegOut1Error
				 ,fitResultRunSenCorrectionEUnRegOut1,fitResultRunSenCorrectionEUnRegOut1Error
				 ,fitResultRunSenCorrectionTotalUnRegOut1,fitResultRunSenCorrectionTotalUnRegOut1Error)
			 <<endl;
  outfileCorrectionAvgOut.close();



  /*********************************************************************************/
  c4->Update();
  c4->SaveAs(Form("%s/plots/%s/%sCorrectionRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c41 = new TCanvas("c41","c41",cx1,cy1,1200,950);
  TPad * pad410 = new TPad("pad410","pad410",ps1,ps2,ps4,ps4);
  TPad * pad411 = new TPad("pad411","pad411",ps1,ps1,ps4,ps3);
  pad410->Draw();
  pad411->Draw();
  pad410->cd();
  TText * ct410 = new TText(tll,tlr,Form("%sN to Delta %s Corrections using %s Slug Averaged Sensitivities and Runlet Differences",regTitle,dataInfo,plotTitle));
  ct410->SetTextSize(tsiz);
  ct410->Draw();
  pad411->cd();
  pad411->Divide(2,3);


  pad411->cd(1);

  TGraphErrors * slugSenCorrectionXGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corSlugXRegInVal,zeroRegIn,ecorRunletXRegInVal);
  slugSenCorrectionXGraphUnRegIn1->SetName("slugSenCorrectionXGraphUnRegIn1");
  slugSenCorrectionXGraphUnRegIn1->SetMarkerColor(kRed);
  slugSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  slugSenCorrectionXGraphUnRegIn1->SetMarkerStyle(21);
  slugSenCorrectionXGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionXGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionXGraphUnRegIn1->Fit("fitSlugSenCorrectionXGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  fitSlugSenCorrectionXGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXUnRegIn1            =  fitSlugSenCorrectionXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXUnRegIn1Error       =  fitSlugSenCorrectionXGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXUnRegIn1Chisquare   =  fitSlugSenCorrectionXGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXUnRegIn1NDF         =  fitSlugSenCorrectionXGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionXUnRegIn1Prob        =  fitSlugSenCorrectionXGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionXGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corSlugXRegOutVal,zeroRegOut,ecorRunletXRegOutVal);
  slugSenCorrectionXGraphUnRegOut1->SetName("slugSenCorrectionXGraphUnRegOut1");
  slugSenCorrectionXGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionXGraphUnRegOut1->SetMarkerStyle(21);
  slugSenCorrectionXGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionXGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionXGraphUnRegOut1->Fit("fitSlugSenCorrectionXGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionXGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXUnRegOut1           =  fitSlugSenCorrectionXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXUnRegOut1Error      =  fitSlugSenCorrectionXGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXUnRegOut1Chisquare  =  fitSlugSenCorrectionXGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXUnRegOut1NDF        =  fitSlugSenCorrectionXGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionXUnRegOut1Prob       =  fitSlugSenCorrectionXGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionXGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionXGraphUnReg1->Add(slugSenCorrectionXGraphUnRegIn1);
  slugSenCorrectionXGraphUnReg1->Add(slugSenCorrectionXGraphUnRegOut1);
  slugSenCorrectionXGraphUnReg1->Draw("AP");
  slugSenCorrectionXGraphUnReg1->SetTitle("");
  slugSenCorrectionXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionXGraphUnReg1->GetYaxis()->SetTitle("X Correction [ppm]");
  slugSenCorrectionXGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionXGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionXGraphUnReg1->GetYaxis()->SetRangeUser(3,10);
  TAxis *xaxisSlugSenCorrectionXGraphUnReg1= slugSenCorrectionXGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionXGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionXGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionXUnRegIn1 =(TPaveStats*)slugSenCorrectionXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionXUnRegOut1=(TPaveStats*)slugSenCorrectionXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionXUnRegIn1->SetTextColor(kRed);
  statsSlugSenCorrectionXUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionXUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionXUnRegInpOut1       =  (fitResultSlugSenCorrectionXUnRegIn1+fitResultSlugSenCorrectionXUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXUnRegInmOut1       =  (fitResultSlugSenCorrectionXUnRegIn1-fitResultSlugSenCorrectionXUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXUnRegIn1Error*fitResultSlugSenCorrectionXUnRegIn1Error+fitResultSlugSenCorrectionXUnRegOut1Error*fitResultSlugSenCorrectionXUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionXUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXUnRegIn1Error*fitResultSlugSenCorrectionXUnRegIn1Error+fitResultSlugSenCorrectionXUnRegOut1Error*fitResultSlugSenCorrectionXUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionXUnRegIn1,fitResultSlugSenCorrectionXUnRegIn1Error),"lp");
  legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionXUnRegOut1,fitResultSlugSenCorrectionXUnRegOut1Error),"lp");
//   legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionXUnRegInpOut1,resultSlugSenCorrectionXUnRegInpOut1Error),"");
//   legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionXUnRegInmOut1,resultSlugSenCorrectionXUnRegInmOut1Error),"");
  legSlugSenCorrectionXUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionXUnReg1->SetFillColor(0);
  legSlugSenCorrectionXUnReg1->SetBorderSize(2);
  legSlugSenCorrectionXUnReg1->Draw();

  gPad->Update();


  pad411->cd(2);

  TGraphErrors * slugSenCorrectionXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corSlugXSlopeRegInVal,zeroRegIn,ecorRunletXSlopeRegInVal);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetName("slugSenCorrectionXSlopeGraphUnRegIn1");
  slugSenCorrectionXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetMarkerStyle(22);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionXSlopeGraphUnRegIn1->Fit("fitSlugSenCorrectionXSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1            =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1Error       =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1Chisquare   =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1NDF         =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1Prob        =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corSlugXSlopeRegOutVal,zeroRegOut,ecorRunletXSlopeRegOutVal);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetName("slugSenCorrectionXSlopeGraphUnRegOut1");
  slugSenCorrectionXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetMarkerStyle(22);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionXSlopeGraphUnRegOut1->Fit("fitSlugSenCorrectionXSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1           =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1Error      =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1Chisquare  =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1NDF        =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1Prob       =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionXSlopeGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionXSlopeGraphUnReg1->Add(slugSenCorrectionXSlopeGraphUnRegIn1);
  slugSenCorrectionXSlopeGraphUnReg1->Add(slugSenCorrectionXSlopeGraphUnRegOut1);
  slugSenCorrectionXSlopeGraphUnReg1->Draw("AP");
  slugSenCorrectionXSlopeGraphUnReg1->SetTitle("");
  slugSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Correction [ppm]");
  slugSenCorrectionXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-7,0);
  TAxis *xaxisSlugSenCorrectionXSlopeGraphUnReg1= slugSenCorrectionXSlopeGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionXSlopeUnRegIn1 =(TPaveStats*)slugSenCorrectionXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionXSlopeUnRegOut1=(TPaveStats*)slugSenCorrectionXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionXSlopeUnRegIn1->SetTextColor(kRed);
  statsSlugSenCorrectionXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXSlopeUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionXSlopeUnRegInpOut1       =  (fitResultSlugSenCorrectionXSlopeUnRegIn1+fitResultSlugSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXSlopeUnRegInmOut1       =  (fitResultSlugSenCorrectionXSlopeUnRegIn1-fitResultSlugSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXSlopeUnRegIn1Error*fitResultSlugSenCorrectionXSlopeUnRegIn1Error+fitResultSlugSenCorrectionXSlopeUnRegOut1Error*fitResultSlugSenCorrectionXSlopeUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXSlopeUnRegIn1Error*fitResultSlugSenCorrectionXSlopeUnRegIn1Error+fitResultSlugSenCorrectionXSlopeUnRegOut1Error*fitResultSlugSenCorrectionXSlopeUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionXSlopeUnRegIn1,fitResultSlugSenCorrectionXSlopeUnRegIn1Error),"lp");
  legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionXSlopeUnRegOut1,fitResultSlugSenCorrectionXSlopeUnRegOut1Error),"lp");
//   legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionXSlopeUnRegInpOut1,resultSlugSenCorrectionXSlopeUnRegInpOut1Error),"");
//   legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionXSlopeUnRegInmOut1,resultSlugSenCorrectionXSlopeUnRegInmOut1Error),"");
  legSlugSenCorrectionXSlopeUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionXSlopeUnReg1->SetFillColor(0);
  legSlugSenCorrectionXSlopeUnReg1->SetBorderSize(2);
  legSlugSenCorrectionXSlopeUnReg1->Draw();

  gPad->Update();


  pad411->cd(3);

  TGraphErrors * slugSenCorrectionYGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corSlugYRegInVal,zeroRegIn,ecorRunletYRegInVal);
  slugSenCorrectionYGraphUnRegIn1->SetName("slugSenCorrectionYGraphUnRegIn1");
  slugSenCorrectionYGraphUnRegIn1->SetMarkerColor(kGreen);
  slugSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  slugSenCorrectionYGraphUnRegIn1->SetMarkerStyle(21);
  slugSenCorrectionYGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionYGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionYGraphUnRegIn1->Fit("fitSlugSenCorrectionYGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  fitSlugSenCorrectionYGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYUnRegIn1            =  fitSlugSenCorrectionYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYUnRegIn1Error       =  fitSlugSenCorrectionYGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYUnRegIn1Chisquare   =  fitSlugSenCorrectionYGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYUnRegIn1NDF         =  fitSlugSenCorrectionYGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionYUnRegIn1Prob        =  fitSlugSenCorrectionYGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionYGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corSlugYRegOutVal,zeroRegOut,ecorRunletYRegOutVal);
  slugSenCorrectionYGraphUnRegOut1->SetName("slugSenCorrectionYGraphUnRegOut1");
  slugSenCorrectionYGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionYGraphUnRegOut1->SetMarkerStyle(21);
  slugSenCorrectionYGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionYGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionYGraphUnRegOut1->Fit("fitSlugSenCorrectionYGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionYGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYUnRegOut1           =  fitSlugSenCorrectionYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYUnRegOut1Error      =  fitSlugSenCorrectionYGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYUnRegOut1Chisquare  =  fitSlugSenCorrectionYGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYUnRegOut1NDF        =  fitSlugSenCorrectionYGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionYUnRegOut1Prob       =  fitSlugSenCorrectionYGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionYGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionYGraphUnReg1->Add(slugSenCorrectionYGraphUnRegIn1);
  slugSenCorrectionYGraphUnReg1->Add(slugSenCorrectionYGraphUnRegOut1);
  slugSenCorrectionYGraphUnReg1->Draw("AP");
  slugSenCorrectionYGraphUnReg1->SetTitle("");
  slugSenCorrectionYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionYGraphUnReg1->GetYaxis()->SetTitle("Y Correction [ppm]");
  slugSenCorrectionYGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionYGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionYGraphUnReg1->GetYaxis()->SetRangeUser(-0,7);
  TAxis *xaxisSlugSenCorrectionYGraphUnReg1= slugSenCorrectionYGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionYGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionYGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionYUnRegIn1 =(TPaveStats*)slugSenCorrectionYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionYUnRegOut1=(TPaveStats*)slugSenCorrectionYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionYUnRegIn1->SetTextColor(kGreen);
  statsSlugSenCorrectionYUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionYUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionYUnRegInpOut1       =  (fitResultSlugSenCorrectionYUnRegIn1+fitResultSlugSenCorrectionYUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYUnRegInmOut1       =  (fitResultSlugSenCorrectionYUnRegIn1-fitResultSlugSenCorrectionYUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYUnRegIn1Error*fitResultSlugSenCorrectionYUnRegIn1Error+fitResultSlugSenCorrectionYUnRegOut1Error*fitResultSlugSenCorrectionYUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionYUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYUnRegIn1Error*fitResultSlugSenCorrectionYUnRegIn1Error+fitResultSlugSenCorrectionYUnRegOut1Error*fitResultSlugSenCorrectionYUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionYUnRegIn1,fitResultSlugSenCorrectionYUnRegIn1Error),"lp");
  legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionYUnRegOut1,fitResultSlugSenCorrectionYUnRegOut1Error),"lp");
//   legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionYUnRegInpOut1,resultSlugSenCorrectionYUnRegInpOut1Error),"");
//   legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionYUnRegInmOut1,resultSlugSenCorrectionYUnRegInmOut1Error),"");
  legSlugSenCorrectionYUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionYUnReg1->SetFillColor(0);
  legSlugSenCorrectionYUnReg1->SetBorderSize(2);
  legSlugSenCorrectionYUnReg1->Draw();

  gPad->Update();


  pad411->cd(4);

  TGraphErrors * slugSenCorrectionYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corSlugYSlopeRegInVal,zeroRegIn,ecorRunletYSlopeRegInVal);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetName("slugSenCorrectionYSlopeGraphUnRegIn1");
  slugSenCorrectionYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetMarkerStyle(22);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionYSlopeGraphUnRegIn1->Fit("fitSlugSenCorrectionYSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1            =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1Error       =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1Chisquare   =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1NDF         =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1Prob        =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corSlugYSlopeRegOutVal,zeroRegOut,ecorRunletYSlopeRegOutVal);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetName("slugSenCorrectionYSlopeGraphUnRegOut1");
  slugSenCorrectionYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetMarkerStyle(22);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionYSlopeGraphUnRegOut1->Fit("fitSlugSenCorrectionYSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1           =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1Error      =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1Chisquare  =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1NDF        =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1Prob       =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionYSlopeGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionYSlopeGraphUnReg1->Add(slugSenCorrectionYSlopeGraphUnRegIn1);
  slugSenCorrectionYSlopeGraphUnReg1->Add(slugSenCorrectionYSlopeGraphUnRegOut1);
  slugSenCorrectionYSlopeGraphUnReg1->Draw("AP");
  slugSenCorrectionYSlopeGraphUnReg1->SetTitle("");
  slugSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  slugSenCorrectionYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-3.5,3.5);
  TAxis *xaxisSlugSenCorrectionYSlopeGraphUnReg1= slugSenCorrectionYSlopeGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionYSlopeUnRegIn1 =(TPaveStats*)slugSenCorrectionYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionYSlopeUnRegOut1=(TPaveStats*)slugSenCorrectionYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionYSlopeUnRegIn1->SetTextColor(kGreen);
  statsSlugSenCorrectionYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYSlopeUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionYSlopeUnRegInpOut1       =  (fitResultSlugSenCorrectionYSlopeUnRegIn1+fitResultSlugSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYSlopeUnRegInmOut1       =  (fitResultSlugSenCorrectionYSlopeUnRegIn1-fitResultSlugSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYSlopeUnRegIn1Error*fitResultSlugSenCorrectionYSlopeUnRegIn1Error+fitResultSlugSenCorrectionYSlopeUnRegOut1Error*fitResultSlugSenCorrectionYSlopeUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYSlopeUnRegIn1Error*fitResultSlugSenCorrectionYSlopeUnRegIn1Error+fitResultSlugSenCorrectionYSlopeUnRegOut1Error*fitResultSlugSenCorrectionYSlopeUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionYSlopeUnRegIn1,fitResultSlugSenCorrectionYSlopeUnRegIn1Error),"lp");
  legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionYSlopeUnRegOut1,fitResultSlugSenCorrectionYSlopeUnRegOut1Error),"lp");
//   legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionYSlopeUnRegInpOut1,resultSlugSenCorrectionYSlopeUnRegInpOut1Error),"");
//   legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionYSlopeUnRegInmOut1,resultSlugSenCorrectionYSlopeUnRegInmOut1Error),"");
  legSlugSenCorrectionYSlopeUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionYSlopeUnReg1->SetFillColor(0);
  legSlugSenCorrectionYSlopeUnReg1->SetBorderSize(2);
  legSlugSenCorrectionYSlopeUnReg1->Draw();

  gPad->Update();


  pad411->cd(5);

  TGraphErrors * slugSenCorrectionEGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corSlugERegInVal,zeroRegIn,ecorRunletERegInVal);
  slugSenCorrectionEGraphUnRegIn1->SetName("slugSenCorrectionEGraphUnRegIn1");
  slugSenCorrectionEGraphUnRegIn1->SetMarkerColor(kBlue);
  slugSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  slugSenCorrectionEGraphUnRegIn1->SetMarkerStyle(20);
  slugSenCorrectionEGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionEGraphUnRegIn1->SetLineWidth(1);
//   slugSenCorrectionEGraphUnRegIn1->Draw("AP");
  slugSenCorrectionEGraphUnRegIn1->Fit("fitSlugSenCorrectionEGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  fitSlugSenCorrectionEGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionEUnRegIn1            =  fitSlugSenCorrectionEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionEUnRegIn1Error       =  fitSlugSenCorrectionEGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionEUnRegIn1Chisquare   =  fitSlugSenCorrectionEGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionEUnRegIn1NDF         =  fitSlugSenCorrectionEGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionEUnRegIn1Prob        =  fitSlugSenCorrectionEGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionEGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corSlugERegOutVal,zeroRegOut,ecorRunletERegOutVal);
  slugSenCorrectionEGraphUnRegOut1->SetName("slugSenCorrectionEGraphUnRegOut1");
  slugSenCorrectionEGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionEGraphUnRegOut1->SetMarkerStyle(20);
  slugSenCorrectionEGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionEGraphUnRegOut1->SetLineWidth(1);
//   slugSenCorrectionEGraphUnRegOut1->Draw("AP");
  slugSenCorrectionEGraphUnRegOut1->Fit("fitSlugSenCorrectionEGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionEGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionEUnRegOut1           =  fitSlugSenCorrectionEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionEUnRegOut1Error      =  fitSlugSenCorrectionEGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionEUnRegOut1Chisquare  =  fitSlugSenCorrectionEGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionEUnRegOut1NDF        =  fitSlugSenCorrectionEGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionEUnRegOut1Prob       =  fitSlugSenCorrectionEGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionEGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionEGraphUnReg1->Add(slugSenCorrectionEGraphUnRegIn1);
  slugSenCorrectionEGraphUnReg1->Add(slugSenCorrectionEGraphUnRegOut1);
  slugSenCorrectionEGraphUnReg1->Draw("AP");
  slugSenCorrectionEGraphUnReg1->SetTitle("");
  slugSenCorrectionEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  slugSenCorrectionEGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionEGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionEGraphUnReg1->GetYaxis()->SetRangeUser(-3.5,3.5);
  TAxis *xaxisSlugSenCorrectionEGraphUnReg1= slugSenCorrectionEGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionEGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionEGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionEUnRegIn1 =(TPaveStats*)slugSenCorrectionEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionEUnRegOut1=(TPaveStats*)slugSenCorrectionEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionEUnRegIn1->SetTextColor(kBlue);
  statsSlugSenCorrectionEUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionEUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionEUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionEUnRegInpOut1       =  (fitResultSlugSenCorrectionEUnRegIn1+fitResultSlugSenCorrectionEUnRegOut1)/2;
  Double_t resultSlugSenCorrectionEUnRegInmOut1       =  (fitResultSlugSenCorrectionEUnRegIn1-fitResultSlugSenCorrectionEUnRegOut1)/2;
  Double_t resultSlugSenCorrectionEUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionEUnRegIn1Error*fitResultSlugSenCorrectionEUnRegIn1Error+fitResultSlugSenCorrectionEUnRegOut1Error*fitResultSlugSenCorrectionEUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionEUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionEUnRegIn1Error*fitResultSlugSenCorrectionEUnRegIn1Error+fitResultSlugSenCorrectionEUnRegOut1Error*fitResultSlugSenCorrectionEUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionEUnRegIn1,fitResultSlugSenCorrectionEUnRegIn1Error),"lp");
  legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionEUnRegOut1,fitResultSlugSenCorrectionEUnRegOut1Error),"lp");
//   legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionEUnRegInpOut1,resultSlugSenCorrectionEUnRegInpOut1Error),"");
//   legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionEUnRegInmOut1,resultSlugSenCorrectionEUnRegInmOut1Error),"");
  legSlugSenCorrectionEUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionEUnReg1->SetFillColor(0);
  legSlugSenCorrectionEUnReg1->SetBorderSize(2);
  legSlugSenCorrectionEUnReg1->Draw();

  gPad->Update();


  pad411->cd(6);

  TGraphErrors * slugSenCorrectionTotalGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,corSlugTotalRegInVal,zeroRegIn,ecorRunletTotalRegInVal);
  slugSenCorrectionTotalGraphUnRegIn1->SetName("slugSenCorrectionTotalGraphUnRegIn1");
  slugSenCorrectionTotalGraphUnRegIn1->SetMarkerColor(kCyan);
  slugSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  slugSenCorrectionTotalGraphUnRegIn1->SetMarkerStyle(21);
  slugSenCorrectionTotalGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionTotalGraphUnRegIn1->SetLineWidth(1);
//   slugSenCorrectionTotalGraphUnRegIn1->Draw("AP");
  slugSenCorrectionTotalGraphUnRegIn1->Fit("fitSlugSenCorrectionTotalGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  fitSlugSenCorrectionTotalGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionTotalGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionTotalUnRegIn1            =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1Error       =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1Chisquare   =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1NDF         =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1Prob        =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionTotalGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,corSlugTotalRegOutVal,zeroRegOut,ecorRunletTotalRegOutVal);
  slugSenCorrectionTotalGraphUnRegOut1->SetName("slugSenCorrectionTotalGraphUnRegOut1");
  slugSenCorrectionTotalGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionTotalGraphUnRegOut1->SetMarkerStyle(21);
  slugSenCorrectionTotalGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionTotalGraphUnRegOut1->SetLineWidth(1);
//   slugSenCorrectionTotalGraphUnRegOut1->Draw("AP");
  slugSenCorrectionTotalGraphUnRegOut1->Fit("fitSlugSenCorrectionTotalGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionTotalGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionTotalGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionTotalUnRegOut1           =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1Error      =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1Chisquare  =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1NDF        =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1Prob       =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionTotalGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionTotalGraphUnReg1->Add(slugSenCorrectionTotalGraphUnRegIn1);
  slugSenCorrectionTotalGraphUnReg1->Add(slugSenCorrectionTotalGraphUnRegOut1);
  slugSenCorrectionTotalGraphUnReg1->Draw("AP");
  slugSenCorrectionTotalGraphUnReg1->SetTitle("");
  slugSenCorrectionTotalGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionTotalGraphUnReg1->GetYaxis()->SetTitle("Total Correction [ppm]");
  slugSenCorrectionTotalGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionTotalGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionTotalGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionTotalGraphUnReg1->GetYaxis()->SetRangeUser(2,9);
  TAxis *xaxisSlugSenCorrectionTotalGraphUnReg1= slugSenCorrectionTotalGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionTotalGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionTotalGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionTotalGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionTotalUnRegIn1 =(TPaveStats*)slugSenCorrectionTotalGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionTotalUnRegOut1=(TPaveStats*)slugSenCorrectionTotalGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionTotalUnRegIn1->SetTextColor(kCyan);
  statsSlugSenCorrectionTotalUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionTotalUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionTotalUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionTotalUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionTotalUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionTotalUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionTotalUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionTotalUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionTotalUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionTotalUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionTotalUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionTotalUnRegInpOut1       =  (fitResultSlugSenCorrectionTotalUnRegIn1+fitResultSlugSenCorrectionTotalUnRegOut1)/2;
  Double_t resultSlugSenCorrectionTotalUnRegInmOut1       =  (fitResultSlugSenCorrectionTotalUnRegIn1-fitResultSlugSenCorrectionTotalUnRegOut1)/2;
  Double_t resultSlugSenCorrectionTotalUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionTotalUnRegIn1Error*fitResultSlugSenCorrectionTotalUnRegIn1Error+fitResultSlugSenCorrectionTotalUnRegOut1Error*fitResultSlugSenCorrectionTotalUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionTotalUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionTotalUnRegIn1Error*fitResultSlugSenCorrectionTotalUnRegIn1Error+fitResultSlugSenCorrectionTotalUnRegOut1Error*fitResultSlugSenCorrectionTotalUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionTotalUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionTotalUnRegIn1,fitResultSlugSenCorrectionTotalUnRegIn1Error),"lp");
  legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionTotalUnRegOut1,fitResultSlugSenCorrectionTotalUnRegOut1Error),"lp");
//   legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionTotalUnRegInpOut1,resultSlugSenCorrectionTotalUnRegInpOut1Error),"");
//   legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionTotalUnRegInmOut1,resultSlugSenCorrectionTotalUnRegInmOut1Error),"");
  legSlugSenCorrectionTotalUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionTotalUnReg1->SetFillColor(0);
  legSlugSenCorrectionTotalUnReg1->SetBorderSize(2);
  legSlugSenCorrectionTotalUnReg1->Draw();

  gPad->Update();


  ofstream outfileSlugCorrectionAvgIn(Form("%s/plots/%s/summary%sSlugCorrectionAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSlugCorrectionAvgIn << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f"
				     ,fitResultSlugSenCorrectionXUnRegIn1,fitResultSlugSenCorrectionXUnRegIn1Error
				     ,fitResultSlugSenCorrectionXSlopeUnRegIn1,fitResultSlugSenCorrectionXSlopeUnRegIn1Error
				     ,fitResultSlugSenCorrectionYUnRegIn1,fitResultSlugSenCorrectionYUnRegIn1Error
				     ,fitResultSlugSenCorrectionYSlopeUnRegIn1,fitResultSlugSenCorrectionYSlopeUnRegIn1Error
				     ,fitResultSlugSenCorrectionEUnRegIn1,fitResultSlugSenCorrectionEUnRegIn1Error
				     ,fitResultSlugSenCorrectionTotalUnRegIn1,fitResultSlugSenCorrectionTotalUnRegIn1Error)
			     <<endl;
  outfileSlugCorrectionAvgIn.close();
  
  ofstream outfileSlugCorrectionAvgOut(Form("%s/plots/%s/summary%sSlugCorrectionAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileSlugCorrectionAvgOut << Form("%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f\t%7.7f"
				      ,fitResultSlugSenCorrectionXUnRegOut1,fitResultSlugSenCorrectionXUnRegOut1Error
				      ,fitResultSlugSenCorrectionXSlopeUnRegOut1,fitResultSlugSenCorrectionXSlopeUnRegOut1Error
				      ,fitResultSlugSenCorrectionYUnRegOut1,fitResultSlugSenCorrectionYUnRegOut1Error
				      ,fitResultSlugSenCorrectionYSlopeUnRegOut1,fitResultSlugSenCorrectionYSlopeUnRegOut1Error
				      ,fitResultSlugSenCorrectionEUnRegOut1,fitResultSlugSenCorrectionEUnRegOut1Error
				      ,fitResultSlugSenCorrectionTotalUnRegOut1,fitResultSlugSenCorrectionTotalUnRegOut1Error)
			      <<endl;
  outfileSlugCorrectionAvgOut.close();


  /*********************************************************************************/
  c41->Update();
  c41->SaveAs(Form("%s/plots/%s/%sCorrectionSlugbySlug%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/


  }



  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(RUN_SENSITIVITY_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TCanvas *c2 = new TCanvas("c2","c2",cx1,cy1,1200,950);
  TPad * pad20 = new TPad("pad20","pad20",ps1,ps2,ps4,ps4);
  TPad * pad21 = new TPad("pad21","pad21",ps1,ps1,ps4,ps3);
  pad20->Draw();
  pad21->Draw();
  pad20->cd();
  TText * ct20 = new TText(tll,tlr,Form("%sN to Delta %s %s Sensitivities",regTitle,dataInfo,plotTitle));
  ct20->SetTextSize(tsiz);
  ct20->Draw();
  pad21->cd();
  pad21->Divide(2,3);

  /*********************************************************************************/

  pad21->cd(1);

  TGraphErrors * runSenXGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,senXRunInVal,zeroRunIn,esenXRunInVal);
  runSenXGraphUnRegIn1->SetName("runSenXGraphUnRegIn1");
  runSenXGraphUnRegIn1->SetMarkerColor(kRed);
  runSenXGraphUnRegIn1->SetLineColor(kRed);
  runSenXGraphUnRegIn1->SetMarkerStyle(25);
  runSenXGraphUnRegIn1->SetMarkerSize(0.5);
  runSenXGraphUnRegIn1->SetLineWidth(1);
  runSenXGraphUnRegIn1->Fit("fitRunSenXGraphUnRegIn1","E M R F 0 Q");
  fitRunSenXGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenXGraphUnRegIn1->SetLineWidth(2);
  fitRunSenXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenXUnRegIn1            =  fitRunSenXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenXUnRegIn1Error       =  fitRunSenXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenXUnRegIn1Chisquare   =  fitRunSenXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenXUnRegIn1NDF         =  fitRunSenXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenXUnRegIn1Prob        =  fitRunSenXGraphUnRegIn1->GetProb();


  TGraphErrors * runSenXGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,senXRunOutVal,zeroRunOut,esenXRunOutVal);
  runSenXGraphUnRegOut1->SetName("runSenXGraphUnRegOut1");
  runSenXGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenXGraphUnRegOut1->SetLineColor(kBlack);
  runSenXGraphUnRegOut1->SetMarkerStyle(25);
  runSenXGraphUnRegOut1->SetMarkerSize(0.5);
  runSenXGraphUnRegOut1->SetLineWidth(1);
  runSenXGraphUnRegOut1->Fit("fitRunSenXGraphUnRegOut1","E M R F 0 Q");
  fitRunSenXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenXGraphUnRegOut1->SetLineWidth(2);
  fitRunSenXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenXUnRegOut1           =  fitRunSenXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenXUnRegOut1Error      =  fitRunSenXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenXUnRegOut1Chisquare  =  fitRunSenXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenXUnRegOut1NDF        =  fitRunSenXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenXUnRegOut1Prob       =  fitRunSenXGraphUnRegOut1->GetProb();

  TMultiGraph *runSenXGraphUnReg1 = new TMultiGraph();
  runSenXGraphUnReg1->Add(runSenXGraphUnRegIn1);
  runSenXGraphUnReg1->Add(runSenXGraphUnRegOut1);
  runSenXGraphUnReg1->Draw("AP");
  runSenXGraphUnReg1->SetTitle("");
  runSenXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenXGraphUnReg1->GetYaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenXGraphUnReg1->GetXaxis()->CenterTitle();
  runSenXGraphUnReg1->GetYaxis()->CenterTitle();
  runSenXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenXGraphUnReg1->GetYaxis()->SetRangeUser(2000,8000);
  TAxis *xaxisRunSenXGraphUnReg1= runSenXGraphUnReg1->GetXaxis();
  xaxisRunSenXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenXGraphUnRegIn1->Draw("same");
  fitRunSenXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenXUnRegIn1 =(TPaveStats*)runSenXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenXUnRegOut1=(TPaveStats*)runSenXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenXUnRegIn1->SetTextColor(kRed);
  statsRunSenXUnRegIn1->SetFillColor(kWhite); 
  statsRunSenXUnRegOut1->SetTextColor(kBlack);
  statsRunSenXUnRegOut1->SetFillColor(kWhite); 
  statsRunSenXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenXUnRegInpOut1       =  (fitResultRunSenXUnRegIn1+fitResultRunSenXUnRegOut1)/2;
  Double_t resultRunSenXUnRegInmOut1       =  (fitResultRunSenXUnRegIn1-fitResultRunSenXUnRegOut1)/2;
  Double_t resultRunSenXUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenXUnRegIn1Error*fitResultRunSenXUnRegIn1Error+fitResultRunSenXUnRegOut1Error*fitResultRunSenXUnRegOut1Error)/2;
  Double_t resultRunSenXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenXUnRegIn1Error*fitResultRunSenXUnRegIn1Error+fitResultRunSenXUnRegOut1Error*fitResultRunSenXUnRegOut1Error)/2;


  TLegend *legRunSenXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenXUnReg1->AddEntry("runSenXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenXUnRegIn1,fitResultRunSenXUnRegIn1Error),"lp");
  legRunSenXUnReg1->AddEntry("runSenXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenXUnRegOut1,fitResultRunSenXUnRegOut1Error),"lp");
//   legRunSenXUnReg1->AddEntry("runSenXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenXUnRegInpOut1,resultRunSenXUnRegInpOut1Error),"");
//   legRunSenXUnReg1->AddEntry("runSenXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenXUnRegInmOut1,resultRunSenXUnRegInmOut1Error),"");
  legRunSenXUnReg1->SetTextSize(0.045);
  legRunSenXUnReg1->SetFillColor(0);
  legRunSenXUnReg1->SetBorderSize(2);
  legRunSenXUnReg1->Draw();

  gPad->Update();


  pad21->cd(2);

  TGraphErrors * runSenXSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,senXSlopeRunInVal,zeroRunIn,esenXSlopeRunInVal);
  runSenXSlopeGraphUnRegIn1->SetName("runSenXSlopeGraphUnRegIn1");
  runSenXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenXSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runSenXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenXSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenXSlopeGraphUnRegIn1->Fit("fitRunSenXSlopeGraphUnRegIn1","E M R F 0 Q");
  fitRunSenXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenXSlopeUnRegIn1            =  fitRunSenXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenXSlopeUnRegIn1Error       =  fitRunSenXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenXSlopeUnRegIn1Chisquare   =  fitRunSenXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenXSlopeUnRegIn1NDF         =  fitRunSenXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenXSlopeUnRegIn1Prob        =  fitRunSenXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenXSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,senXSlopeRunOutVal,zeroRunOut,esenXSlopeRunOutVal);
  runSenXSlopeGraphUnRegOut1->SetName("runSenXSlopeGraphUnRegOut1");
  runSenXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenXSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runSenXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenXSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenXSlopeGraphUnRegOut1->Fit("fitRunSenXSlopeGraphUnRegOut1","E M R F 0 Q");
  fitRunSenXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenXSlopeUnRegOut1           =  fitRunSenXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenXSlopeUnRegOut1Error      =  fitRunSenXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenXSlopeUnRegOut1Chisquare  =  fitRunSenXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenXSlopeUnRegOut1NDF        =  fitRunSenXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenXSlopeUnRegOut1Prob       =  fitRunSenXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenXSlopeGraphUnReg1 = new TMultiGraph();
  runSenXSlopeGraphUnReg1->Add(runSenXSlopeGraphUnRegIn1);
  runSenXSlopeGraphUnReg1->Add(runSenXSlopeGraphUnRegOut1);
  runSenXSlopeGraphUnReg1->Draw("AP");
  runSenXSlopeGraphUnReg1->SetTitle("");
  runSenXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Sensitivity [ppm/#murad]");
  runSenXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-80,0);
  TAxis *xaxisRunSenXSlopeGraphUnReg1= runSenXSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenXSlopeGraphUnRegIn1->Draw("same");
  fitRunSenXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenXSlopeUnRegIn1 =(TPaveStats*)runSenXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenXSlopeUnRegOut1=(TPaveStats*)runSenXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunSenXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenXSlopeUnRegInpOut1       =  (fitResultRunSenXSlopeUnRegIn1+fitResultRunSenXSlopeUnRegOut1)/2;
  Double_t resultRunSenXSlopeUnRegInmOut1       =  (fitResultRunSenXSlopeUnRegIn1-fitResultRunSenXSlopeUnRegOut1)/2;
  Double_t resultRunSenXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenXSlopeUnRegIn1Error*fitResultRunSenXSlopeUnRegIn1Error+fitResultRunSenXSlopeUnRegOut1Error*fitResultRunSenXSlopeUnRegOut1Error)/2;
  Double_t resultRunSenXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenXSlopeUnRegIn1Error*fitResultRunSenXSlopeUnRegIn1Error+fitResultRunSenXSlopeUnRegOut1Error*fitResultRunSenXSlopeUnRegOut1Error)/2;


  TLegend *legRunSenXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenXSlopeUnRegIn1,fitResultRunSenXSlopeUnRegIn1Error),"lp");
  legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenXSlopeUnRegOut1,fitResultRunSenXSlopeUnRegOut1Error),"lp");
//   legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenXSlopeUnRegInpOut1,resultRunSenXSlopeUnRegInpOut1Error),"");
//   legRunSenXSlopeUnReg1->AddEntry("runSenXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenXSlopeUnRegInmOut1,resultRunSenXSlopeUnRegInmOut1Error),"");
  legRunSenXSlopeUnReg1->SetTextSize(0.045);
  legRunSenXSlopeUnReg1->SetFillColor(0);
  legRunSenXSlopeUnReg1->SetBorderSize(2);
  legRunSenXSlopeUnReg1->Draw();

  gPad->Update();


  pad21->cd(3);

  TGraphErrors * runSenYGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,senYRunInVal,zeroRunIn,esenYRunInVal);
  runSenYGraphUnRegIn1->SetName("runSenYGraphUnRegIn1");
  runSenYGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenYGraphUnRegIn1->SetLineColor(kGreen);
  runSenYGraphUnRegIn1->SetMarkerStyle(25);
  runSenYGraphUnRegIn1->SetMarkerSize(0.5);
  runSenYGraphUnRegIn1->SetLineWidth(1);
  runSenYGraphUnRegIn1->Fit("fitRunSenYGraphUnRegIn1","E M R F 0 Q");
  fitRunSenYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenYGraphUnRegIn1->SetLineWidth(2);
  fitRunSenYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenYUnRegIn1            =  fitRunSenYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenYUnRegIn1Error       =  fitRunSenYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenYUnRegIn1Chisquare   =  fitRunSenYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenYUnRegIn1NDF         =  fitRunSenYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenYUnRegIn1Prob        =  fitRunSenYGraphUnRegIn1->GetProb();


  TGraphErrors * runSenYGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,senYRunOutVal,zeroRunOut,esenYRunOutVal);
  runSenYGraphUnRegOut1->SetName("runSenYGraphUnRegOut1");
  runSenYGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenYGraphUnRegOut1->SetLineColor(kBlack);
  runSenYGraphUnRegOut1->SetMarkerStyle(25);
  runSenYGraphUnRegOut1->SetMarkerSize(0.5);
  runSenYGraphUnRegOut1->SetLineWidth(1);
  runSenYGraphUnRegOut1->Fit("fitRunSenYGraphUnRegOut1","E M R F 0 Q");
  fitRunSenYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenYGraphUnRegOut1->SetLineWidth(2);
  fitRunSenYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenYUnRegOut1           =  fitRunSenYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenYUnRegOut1Error      =  fitRunSenYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenYUnRegOut1Chisquare  =  fitRunSenYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenYUnRegOut1NDF        =  fitRunSenYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenYUnRegOut1Prob       =  fitRunSenYGraphUnRegOut1->GetProb();

  TMultiGraph *runSenYGraphUnReg1 = new TMultiGraph();
  runSenYGraphUnReg1->Add(runSenYGraphUnRegIn1);
  runSenYGraphUnReg1->Add(runSenYGraphUnRegOut1);
  runSenYGraphUnReg1->Draw("AP");
  runSenYGraphUnReg1->SetTitle("");
  runSenYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenYGraphUnReg1->GetYaxis()->SetTitle("Y Sensitivity [ppm/mm]");
  runSenYGraphUnReg1->GetXaxis()->CenterTitle();
  runSenYGraphUnReg1->GetYaxis()->CenterTitle();
  runSenYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenYGraphUnReg1->GetYaxis()->SetRangeUser(-4500,100);
  TAxis *xaxisRunSenYGraphUnReg1= runSenYGraphUnReg1->GetXaxis();
  xaxisRunSenYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenYGraphUnRegIn1->Draw("same");
  fitRunSenYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenYUnRegIn1 =(TPaveStats*)runSenYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenYUnRegOut1=(TPaveStats*)runSenYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenYUnRegIn1->SetTextColor(kGreen);
  statsRunSenYUnRegIn1->SetFillColor(kWhite); 
  statsRunSenYUnRegOut1->SetTextColor(kBlack);
  statsRunSenYUnRegOut1->SetFillColor(kWhite); 
  statsRunSenYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenYUnRegInpOut1       =  (fitResultRunSenYUnRegIn1+fitResultRunSenYUnRegOut1)/2;
  Double_t resultRunSenYUnRegInmOut1       =  (fitResultRunSenYUnRegIn1-fitResultRunSenYUnRegOut1)/2;
  Double_t resultRunSenYUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenYUnRegIn1Error*fitResultRunSenYUnRegIn1Error+fitResultRunSenYUnRegOut1Error*fitResultRunSenYUnRegOut1Error)/2;
  Double_t resultRunSenYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenYUnRegIn1Error*fitResultRunSenYUnRegIn1Error+fitResultRunSenYUnRegOut1Error*fitResultRunSenYUnRegOut1Error)/2;


  TLegend *legRunSenYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenYUnReg1->AddEntry("runSenYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenYUnRegIn1,fitResultRunSenYUnRegIn1Error),"lp");
  legRunSenYUnReg1->AddEntry("runSenYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenYUnRegOut1,fitResultRunSenYUnRegOut1Error),"lp");
//   legRunSenYUnReg1->AddEntry("runSenYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenYUnRegInpOut1,resultRunSenYUnRegInpOut1Error),"");
//   legRunSenYUnReg1->AddEntry("runSenYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenYUnRegInmOut1,resultRunSenYUnRegInmOut1Error),"");
  legRunSenYUnReg1->SetTextSize(0.045);
  legRunSenYUnReg1->SetFillColor(0);
  legRunSenYUnReg1->SetBorderSize(2);
  legRunSenYUnReg1->Draw();

  gPad->Update();


  pad21->cd(4);

  TGraphErrors * runSenYSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,senYSlopeRunInVal,zeroRunIn,esenYSlopeRunInVal);
  runSenYSlopeGraphUnRegIn1->SetName("runSenYSlopeGraphUnRegIn1");
  runSenYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenYSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runSenYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenYSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenYSlopeGraphUnRegIn1->Fit("fitRunSenYSlopeGraphUnRegIn1","E M R F 0 Q");
  fitRunSenYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenYSlopeUnRegIn1            =  fitRunSenYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenYSlopeUnRegIn1Error       =  fitRunSenYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenYSlopeUnRegIn1Chisquare   =  fitRunSenYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenYSlopeUnRegIn1NDF         =  fitRunSenYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenYSlopeUnRegIn1Prob        =  fitRunSenYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenYSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,senYSlopeRunOutVal,zeroRunOut,esenYSlopeRunOutVal);
  runSenYSlopeGraphUnRegOut1->SetName("runSenYSlopeGraphUnRegOut1");
  runSenYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenYSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runSenYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenYSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenYSlopeGraphUnRegOut1->Fit("fitRunSenYSlopeGraphUnRegOut1","E M R F 0 Q");
  fitRunSenYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenYSlopeUnRegOut1           =  fitRunSenYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenYSlopeUnRegOut1Error      =  fitRunSenYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenYSlopeUnRegOut1Chisquare  =  fitRunSenYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenYSlopeUnRegOut1NDF        =  fitRunSenYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenYSlopeUnRegOut1Prob       =  fitRunSenYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenYSlopeGraphUnReg1 = new TMultiGraph();
  runSenYSlopeGraphUnReg1->Add(runSenYSlopeGraphUnRegIn1);
  runSenYSlopeGraphUnReg1->Add(runSenYSlopeGraphUnRegOut1);
  runSenYSlopeGraphUnReg1->Draw("AP");
  runSenYSlopeGraphUnReg1->SetTitle("");
  runSenYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Sensitivity [ppm/#murad]");
  runSenYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-40,120);
  TAxis *xaxisRunSenYSlopeGraphUnReg1= runSenYSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenYSlopeGraphUnRegIn1->Draw("same");
  fitRunSenYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenYSlopeUnRegIn1 =(TPaveStats*)runSenYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenYSlopeUnRegOut1=(TPaveStats*)runSenYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunSenYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenYSlopeUnRegInpOut1       =  (fitResultRunSenYSlopeUnRegIn1+fitResultRunSenYSlopeUnRegOut1)/2;
  Double_t resultRunSenYSlopeUnRegInmOut1       =  (fitResultRunSenYSlopeUnRegIn1-fitResultRunSenYSlopeUnRegOut1)/2;
  Double_t resultRunSenYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenYSlopeUnRegIn1Error*fitResultRunSenYSlopeUnRegIn1Error+fitResultRunSenYSlopeUnRegOut1Error*fitResultRunSenYSlopeUnRegOut1Error)/2;
  Double_t resultRunSenYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenYSlopeUnRegIn1Error*fitResultRunSenYSlopeUnRegIn1Error+fitResultRunSenYSlopeUnRegOut1Error*fitResultRunSenYSlopeUnRegOut1Error)/2;


  TLegend *legRunSenYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenYSlopeUnRegIn1,fitResultRunSenYSlopeUnRegIn1Error),"lp");
  legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenYSlopeUnRegOut1,fitResultRunSenYSlopeUnRegOut1Error),"lp");
//   legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenYSlopeUnRegInpOut1,resultRunSenYSlopeUnRegInpOut1Error),"");
//   legRunSenYSlopeUnReg1->AddEntry("runSenYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenYSlopeUnRegInmOut1,resultRunSenYSlopeUnRegInmOut1Error),"");
  legRunSenYSlopeUnReg1->SetTextSize(0.045);
  legRunSenYSlopeUnReg1->SetFillColor(0);
  legRunSenYSlopeUnReg1->SetBorderSize(2);
  legRunSenYSlopeUnReg1->Draw();

  gPad->Update();


  pad21->cd(5);

  TGraphErrors * runSenEGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,senERunInVal,zeroRunIn,esenERunInVal);
  runSenEGraphUnRegIn1->SetName("runSenEGraphUnRegIn1");
  runSenEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenEGraphUnRegIn1->SetLineColor(kBlue);
  runSenEGraphUnRegIn1->SetMarkerStyle(24);
  runSenEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenEGraphUnRegIn1->SetLineWidth(1);
//   runSenEGraphUnRegIn1->Draw("AP");
  runSenEGraphUnRegIn1->Fit("fitRunSenEGraphUnRegIn1","E M R F 0 Q");
  fitRunSenEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunSenEGraphUnRegIn1->SetLineWidth(2);
  fitRunSenEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenEUnRegIn1            =  fitRunSenEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenEUnRegIn1Error       =  fitRunSenEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenEUnRegIn1Chisquare   =  fitRunSenEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenEUnRegIn1NDF         =  fitRunSenEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenEUnRegIn1Prob        =  fitRunSenEGraphUnRegIn1->GetProb();


  TGraphErrors * runSenEGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,senERunOutVal,zeroRunOut,esenERunOutVal);
  runSenEGraphUnRegOut1->SetName("runSenEGraphUnRegOut1");
  runSenEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenEGraphUnRegOut1->SetLineColor(kBlack);
  runSenEGraphUnRegOut1->SetMarkerStyle(24);
  runSenEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenEGraphUnRegOut1->SetLineWidth(1);
//   runSenEGraphUnRegOut1->Draw("AP");
  runSenEGraphUnRegOut1->Fit("fitRunSenEGraphUnRegOut1","E M R F 0 Q");
  fitRunSenEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenEGraphUnRegOut1->SetLineWidth(2);
  fitRunSenEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenEUnRegOut1           =  fitRunSenEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenEUnRegOut1Error      =  fitRunSenEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenEUnRegOut1Chisquare  =  fitRunSenEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenEUnRegOut1NDF        =  fitRunSenEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenEUnRegOut1Prob       =  fitRunSenEGraphUnRegOut1->GetProb();

  TMultiGraph *runSenEGraphUnReg1 = new TMultiGraph();
  runSenEGraphUnReg1->Add(runSenEGraphUnRegIn1);
  runSenEGraphUnReg1->Add(runSenEGraphUnRegOut1);
  runSenEGraphUnReg1->Draw("AP");
  runSenEGraphUnReg1->SetTitle("");
  runSenEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenEGraphUnReg1->GetYaxis()->SetTitle("E Sensitivity [dim less]");
  runSenEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenEGraphUnReg1->GetYaxis()->CenterTitle();
  runSenEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenEGraphUnReg1->GetYaxis()->SetRangeUser(-5,5);
  TAxis *xaxisRunSenEGraphUnReg1= runSenEGraphUnReg1->GetXaxis();
  xaxisRunSenEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenEGraphUnRegIn1->Draw("same");
  fitRunSenEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenEUnRegIn1 =(TPaveStats*)runSenEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenEUnRegOut1=(TPaveStats*)runSenEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenEUnRegIn1->SetTextColor(kBlue);
  statsRunSenEUnRegIn1->SetFillColor(kWhite); 
  statsRunSenEUnRegOut1->SetTextColor(kBlack);
  statsRunSenEUnRegOut1->SetFillColor(kWhite); 
  statsRunSenEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenEUnRegInpOut1       =  (fitResultRunSenEUnRegIn1+fitResultRunSenEUnRegOut1)/2;
  Double_t resultRunSenEUnRegInmOut1       =  (fitResultRunSenEUnRegIn1-fitResultRunSenEUnRegOut1)/2;
  Double_t resultRunSenEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenEUnRegIn1Error*fitResultRunSenEUnRegIn1Error+fitResultRunSenEUnRegOut1Error*fitResultRunSenEUnRegOut1Error)/2;
  Double_t resultRunSenEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenEUnRegIn1Error*fitResultRunSenEUnRegIn1Error+fitResultRunSenEUnRegOut1Error*fitResultRunSenEUnRegOut1Error)/2;


  TLegend *legRunSenEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenEUnReg1->AddEntry("runSenEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenEUnRegIn1,fitResultRunSenEUnRegIn1Error),"lp");
  legRunSenEUnReg1->AddEntry("runSenEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenEUnRegOut1,fitResultRunSenEUnRegOut1Error),"lp");
//   legRunSenEUnReg1->AddEntry("runSenEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenEUnRegInpOut1,resultRunSenEUnRegInpOut1Error),"");
//   legRunSenEUnReg1->AddEntry("runSenEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenEUnRegInmOut1,resultRunSenEUnRegInmOut1Error),"");
  legRunSenEUnReg1->SetTextSize(0.045);
  legRunSenEUnReg1->SetFillColor(0);
  legRunSenEUnReg1->SetBorderSize(2);
  legRunSenEUnReg1->Draw();

  gPad->Update();
  
  pad21->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,chargeRunInVal,zeroRunIn,echargeRunInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,chargeRunOutVal,zeroRunOut,echargeRunOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-15,15);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();


//   TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testChargeAsymUnRegIn.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
//   runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
//   runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
//   runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
//   runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
//   runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
//   runChargeAsymGraphUnRegIn1->SetLineWidth(1);
// //   runChargeAsymGraphUnRegIn1->Draw("AP");
//   runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q");
//   fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
//   fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
//   fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

//   Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
//   Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
//   Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
//   Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
//   Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


//   TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testChargeAsymUnRegOut.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
//   runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
//   runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
//   runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
//   runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
//   runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
//   runChargeAsymGraphUnRegOut1->SetLineWidth(1);
// //   runChargeAsymGraphUnRegOut1->Draw("AP");
//   runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q");
//   fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
//   fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
//   fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

//   Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
//   Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
//   Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
//   Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
//   Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

//   TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
//   runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
//   runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
//   runChargeAsymGraphUnReg1->Draw("AP");
//   runChargeAsymGraphUnReg1->SetTitle("");
//   runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
//   runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
//   runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
//   runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
//   runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
//   runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-20,20);
//   TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
//   xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
//   fitRunChargeAsymGraphUnRegIn1->Draw("same");
//   fitRunChargeAsymGraphUnRegOut1->Draw("same");

//   TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
//   TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
//   statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
//   statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
//   statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
//   statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
//   statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
//   statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
//   statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
//   statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

//   Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
//   Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
//   Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
//   Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


//   TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
//   legRunChargeAsymUnReg1->SetTextSize(0.030);
//   legRunChargeAsymUnReg1->SetFillColor(0);
//   legRunChargeAsymUnReg1->SetBorderSize(2);
//   legRunChargeAsymUnReg1->Draw();

//   gPad->Update();


  c2->Update();
  c2->SaveAs(Form("%s/plots/%s/%sSensitivityRunbyRun%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));

  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c4 = new TCanvas("c4","c4",cx1,cy1,1200,950);
  TPad * pad40 = new TPad("pad40","pad40",ps1,ps2,ps4,ps4);
  TPad * pad41 = new TPad("pad41","pad41",ps1,ps1,ps4,ps3);
  pad40->Draw();
  pad41->Draw();
  pad40->cd();
  TText * ct40 = new TText(tll,tlr,Form("%sN to Delta %s Corrections using %s Run Sensitivities and Differences",regTitle,dataInfo,plotTitle));
  ct40->SetTextSize(tsiz);
  ct40->Draw();
  pad41->cd();
  pad41->Divide(2,3);


  pad41->cd(1);

  TGraphErrors * runSenCorrectionXGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corXRunIn,zeroRunIn,ecorXRunIn);
  runSenCorrectionXGraphUnRegIn1->SetName("runSenCorrectionXGraphUnRegIn1");
  runSenCorrectionXGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  runSenCorrectionXGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorrectionXGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionXGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionXGraphUnRegIn1->Fit("fitRunSenCorrectionXGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenCorrectionXGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXUnRegIn1            =  fitRunSenCorrectionXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXUnRegIn1Error       =  fitRunSenCorrectionXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionXUnRegIn1Chisquare   =  fitRunSenCorrectionXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionXUnRegIn1NDF         =  fitRunSenCorrectionXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionXUnRegIn1Prob        =  fitRunSenCorrectionXGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionXGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corXRunOut,zeroRunOut,ecorXRunOut);
  runSenCorrectionXGraphUnRegOut1->SetName("runSenCorrectionXGraphUnRegOut1");
  runSenCorrectionXGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionXGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorrectionXGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionXGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionXGraphUnRegOut1->Fit("fitRunSenCorrectionXGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionXGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXUnRegOut1           =  fitRunSenCorrectionXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXUnRegOut1Error      =  fitRunSenCorrectionXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionXUnRegOut1Chisquare  =  fitRunSenCorrectionXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionXUnRegOut1NDF        =  fitRunSenCorrectionXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionXUnRegOut1Prob       =  fitRunSenCorrectionXGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionXGraphUnReg1 = new TMultiGraph();
  runSenCorrectionXGraphUnReg1->Add(runSenCorrectionXGraphUnRegIn1);
  runSenCorrectionXGraphUnReg1->Add(runSenCorrectionXGraphUnRegOut1);
  runSenCorrectionXGraphUnReg1->Draw("AP");
  runSenCorrectionXGraphUnReg1->SetTitle("");
  runSenCorrectionXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionXGraphUnReg1->GetYaxis()->SetTitle("X Correction [ppm]");
  runSenCorrectionXGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionXGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionXGraphUnReg1->GetYaxis()->SetRangeUser(0,4);
  TAxis *xaxisRunSenCorrectionXGraphUnReg1= runSenCorrectionXGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionXGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionXUnRegIn1 =(TPaveStats*)runSenCorrectionXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionXUnRegOut1=(TPaveStats*)runSenCorrectionXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionXUnRegIn1->SetTextColor(kRed);
  statsRunSenCorrectionXUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionXUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionXUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionXUnRegInpOut1       =  (fitResultRunSenCorrectionXUnRegIn1+fitResultRunSenCorrectionXUnRegOut1)/2;
  Double_t resultRunSenCorrectionXUnRegInmOut1       =  (fitResultRunSenCorrectionXUnRegIn1-fitResultRunSenCorrectionXUnRegOut1)/2;
  Double_t resultRunSenCorrectionXUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXUnRegIn1Error*fitResultRunSenCorrectionXUnRegIn1Error+fitResultRunSenCorrectionXUnRegOut1Error*fitResultRunSenCorrectionXUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXUnRegIn1Error*fitResultRunSenCorrectionXUnRegIn1Error+fitResultRunSenCorrectionXUnRegOut1Error*fitResultRunSenCorrectionXUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionXUnRegIn1,fitResultRunSenCorrectionXUnRegIn1Error),"lp");
  legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionXUnRegOut1,fitResultRunSenCorrectionXUnRegOut1Error),"lp");
//   legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionXUnRegInpOut1,resultRunSenCorrectionXUnRegInpOut1Error),"");
//   legRunSenCorrectionXUnReg1->AddEntry("runSenCorrectionXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionXUnRegInmOut1,resultRunSenCorrectionXUnRegInmOut1Error),"");
  legRunSenCorrectionXUnReg1->SetTextSize(0.045);
  legRunSenCorrectionXUnReg1->SetFillColor(0);
  legRunSenCorrectionXUnReg1->SetBorderSize(2);
  legRunSenCorrectionXUnReg1->Draw();

  gPad->Update();


  pad41->cd(2);

  TGraphErrors * runSenCorrectionXSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corXSlopeRunIn,zeroRunIn,ecorXSlopeRunIn);
  runSenCorrectionXSlopeGraphUnRegIn1->SetName("runSenCorrectionXSlopeGraphUnRegIn1");
  runSenCorrectionXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenCorrectionXSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runSenCorrectionXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionXSlopeGraphUnRegIn1->Fit("fitRunSenCorrectionXSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1            =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1Error       =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1Chisquare   =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1NDF         =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionXSlopeUnRegIn1Prob        =  fitRunSenCorrectionXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionXSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corXSlopeRunOut,zeroRunOut,ecorXSlopeRunOut);
  runSenCorrectionXSlopeGraphUnRegOut1->SetName("runSenCorrectionXSlopeGraphUnRegOut1");
  runSenCorrectionXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionXSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runSenCorrectionXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionXSlopeGraphUnRegOut1->Fit("fitRunSenCorrectionXSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1           =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1Error      =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1Chisquare  =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1NDF        =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionXSlopeUnRegOut1Prob       =  fitRunSenCorrectionXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionXSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorrectionXSlopeGraphUnReg1->Add(runSenCorrectionXSlopeGraphUnRegIn1);
  runSenCorrectionXSlopeGraphUnReg1->Add(runSenCorrectionXSlopeGraphUnRegOut1);
  runSenCorrectionXSlopeGraphUnReg1->Draw("AP");
  runSenCorrectionXSlopeGraphUnReg1->SetTitle("");
  runSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Correction [ppm]");
  runSenCorrectionXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisRunSenCorrectionXSlopeGraphUnReg1= runSenCorrectionXSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionXSlopeGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionXSlopeUnRegIn1 =(TPaveStats*)runSenCorrectionXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionXSlopeUnRegOut1=(TPaveStats*)runSenCorrectionXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunSenCorrectionXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionXSlopeUnRegInpOut1       =  (fitResultRunSenCorrectionXSlopeUnRegIn1+fitResultRunSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionXSlopeUnRegInmOut1       =  (fitResultRunSenCorrectionXSlopeUnRegIn1-fitResultRunSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXSlopeUnRegIn1Error*fitResultRunSenCorrectionXSlopeUnRegIn1Error+fitResultRunSenCorrectionXSlopeUnRegOut1Error*fitResultRunSenCorrectionXSlopeUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionXSlopeUnRegIn1Error*fitResultRunSenCorrectionXSlopeUnRegIn1Error+fitResultRunSenCorrectionXSlopeUnRegOut1Error*fitResultRunSenCorrectionXSlopeUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionXSlopeUnRegIn1,fitResultRunSenCorrectionXSlopeUnRegIn1Error),"lp");
  legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionXSlopeUnRegOut1,fitResultRunSenCorrectionXSlopeUnRegOut1Error),"lp");
//   legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionXSlopeUnRegInpOut1,resultRunSenCorrectionXSlopeUnRegInpOut1Error),"");
//   legRunSenCorrectionXSlopeUnReg1->AddEntry("runSenCorrectionXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionXSlopeUnRegInmOut1,resultRunSenCorrectionXSlopeUnRegInmOut1Error),"");
  legRunSenCorrectionXSlopeUnReg1->SetTextSize(0.045);
  legRunSenCorrectionXSlopeUnReg1->SetFillColor(0);
  legRunSenCorrectionXSlopeUnReg1->SetBorderSize(2);
  legRunSenCorrectionXSlopeUnReg1->Draw();

  gPad->Update();


  pad41->cd(3);

  TGraphErrors * runSenCorrectionYGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corYRunIn,zeroRunIn,ecorYRunIn);
  runSenCorrectionYGraphUnRegIn1->SetName("runSenCorrectionYGraphUnRegIn1");
  runSenCorrectionYGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorrectionYGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorrectionYGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionYGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionYGraphUnRegIn1->Fit("fitRunSenCorrectionYGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenCorrectionYGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYUnRegIn1            =  fitRunSenCorrectionYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYUnRegIn1Error       =  fitRunSenCorrectionYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionYUnRegIn1Chisquare   =  fitRunSenCorrectionYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionYUnRegIn1NDF         =  fitRunSenCorrectionYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionYUnRegIn1Prob        =  fitRunSenCorrectionYGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionYGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corYRunOut,zeroRunOut,ecorYRunOut);
  runSenCorrectionYGraphUnRegOut1->SetName("runSenCorrectionYGraphUnRegOut1");
  runSenCorrectionYGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionYGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorrectionYGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionYGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionYGraphUnRegOut1->Fit("fitRunSenCorrectionYGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionYGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYUnRegOut1           =  fitRunSenCorrectionYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYUnRegOut1Error      =  fitRunSenCorrectionYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionYUnRegOut1Chisquare  =  fitRunSenCorrectionYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionYUnRegOut1NDF        =  fitRunSenCorrectionYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionYUnRegOut1Prob       =  fitRunSenCorrectionYGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionYGraphUnReg1 = new TMultiGraph();
  runSenCorrectionYGraphUnReg1->Add(runSenCorrectionYGraphUnRegIn1);
  runSenCorrectionYGraphUnReg1->Add(runSenCorrectionYGraphUnRegOut1);
  runSenCorrectionYGraphUnReg1->Draw("AP");
  runSenCorrectionYGraphUnReg1->SetTitle("");
  runSenCorrectionYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionYGraphUnReg1->GetYaxis()->SetTitle("Y Correction [ppm]");
  runSenCorrectionYGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionYGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionYGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisRunSenCorrectionYGraphUnReg1= runSenCorrectionYGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionYGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionYUnRegIn1 =(TPaveStats*)runSenCorrectionYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionYUnRegOut1=(TPaveStats*)runSenCorrectionYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionYUnRegIn1->SetTextColor(kGreen);
  statsRunSenCorrectionYUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionYUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionYUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionYUnRegInpOut1       =  (fitResultRunSenCorrectionYUnRegIn1+fitResultRunSenCorrectionYUnRegOut1)/2;
  Double_t resultRunSenCorrectionYUnRegInmOut1       =  (fitResultRunSenCorrectionYUnRegIn1-fitResultRunSenCorrectionYUnRegOut1)/2;
  Double_t resultRunSenCorrectionYUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYUnRegIn1Error*fitResultRunSenCorrectionYUnRegIn1Error+fitResultRunSenCorrectionYUnRegOut1Error*fitResultRunSenCorrectionYUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYUnRegIn1Error*fitResultRunSenCorrectionYUnRegIn1Error+fitResultRunSenCorrectionYUnRegOut1Error*fitResultRunSenCorrectionYUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionYUnRegIn1,fitResultRunSenCorrectionYUnRegIn1Error),"lp");
  legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionYUnRegOut1,fitResultRunSenCorrectionYUnRegOut1Error),"lp");
//   legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionYUnRegInpOut1,resultRunSenCorrectionYUnRegInpOut1Error),"");
//   legRunSenCorrectionYUnReg1->AddEntry("runSenCorrectionYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionYUnRegInmOut1,resultRunSenCorrectionYUnRegInmOut1Error),"");
  legRunSenCorrectionYUnReg1->SetTextSize(0.045);
  legRunSenCorrectionYUnReg1->SetFillColor(0);
  legRunSenCorrectionYUnReg1->SetBorderSize(2);
  legRunSenCorrectionYUnReg1->Draw();

  gPad->Update();


  pad41->cd(4);

  TGraphErrors * runSenCorrectionYSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corYSlopeRunIn,zeroRunIn,ecorYSlopeRunIn);
  runSenCorrectionYSlopeGraphUnRegIn1->SetName("runSenCorrectionYSlopeGraphUnRegIn1");
  runSenCorrectionYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorrectionYSlopeGraphUnRegIn1->SetMarkerStyle(22);
  runSenCorrectionYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(1);
  runSenCorrectionYSlopeGraphUnRegIn1->Fit("fitRunSenCorrectionYSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1            =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1Error       =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1Chisquare   =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1NDF         =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionYSlopeUnRegIn1Prob        =  fitRunSenCorrectionYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionYSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corYSlopeRunOut,zeroRunOut,ecorYSlopeRunOut);
  runSenCorrectionYSlopeGraphUnRegOut1->SetName("runSenCorrectionYSlopeGraphUnRegOut1");
  runSenCorrectionYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionYSlopeGraphUnRegOut1->SetMarkerStyle(22);
  runSenCorrectionYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(1);
  runSenCorrectionYSlopeGraphUnRegOut1->Fit("fitRunSenCorrectionYSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1           =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1Error      =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1Chisquare  =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1NDF        =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionYSlopeUnRegOut1Prob       =  fitRunSenCorrectionYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionYSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorrectionYSlopeGraphUnReg1->Add(runSenCorrectionYSlopeGraphUnRegIn1);
  runSenCorrectionYSlopeGraphUnReg1->Add(runSenCorrectionYSlopeGraphUnRegOut1);
  runSenCorrectionYSlopeGraphUnReg1->Draw("AP");
  runSenCorrectionYSlopeGraphUnReg1->SetTitle("");
  runSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  runSenCorrectionYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisRunSenCorrectionYSlopeGraphUnReg1= runSenCorrectionYSlopeGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionYSlopeGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionYSlopeUnRegIn1 =(TPaveStats*)runSenCorrectionYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionYSlopeUnRegOut1=(TPaveStats*)runSenCorrectionYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunSenCorrectionYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionYSlopeUnRegInpOut1       =  (fitResultRunSenCorrectionYSlopeUnRegIn1+fitResultRunSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionYSlopeUnRegInmOut1       =  (fitResultRunSenCorrectionYSlopeUnRegIn1-fitResultRunSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultRunSenCorrectionYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYSlopeUnRegIn1Error*fitResultRunSenCorrectionYSlopeUnRegIn1Error+fitResultRunSenCorrectionYSlopeUnRegOut1Error*fitResultRunSenCorrectionYSlopeUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionYSlopeUnRegIn1Error*fitResultRunSenCorrectionYSlopeUnRegIn1Error+fitResultRunSenCorrectionYSlopeUnRegOut1Error*fitResultRunSenCorrectionYSlopeUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionYSlopeUnRegIn1,fitResultRunSenCorrectionYSlopeUnRegIn1Error),"lp");
  legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunSenCorrectionYSlopeUnRegOut1,fitResultRunSenCorrectionYSlopeUnRegOut1Error),"lp");
//   legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionYSlopeUnRegInpOut1,resultRunSenCorrectionYSlopeUnRegInpOut1Error),"");
//   legRunSenCorrectionYSlopeUnReg1->AddEntry("runSenCorrectionYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunSenCorrectionYSlopeUnRegInmOut1,resultRunSenCorrectionYSlopeUnRegInmOut1Error),"");
  legRunSenCorrectionYSlopeUnReg1->SetTextSize(0.045);
  legRunSenCorrectionYSlopeUnReg1->SetFillColor(0);
  legRunSenCorrectionYSlopeUnReg1->SetBorderSize(2);
  legRunSenCorrectionYSlopeUnReg1->Draw();

  gPad->Update();


  pad41->cd(5);

  TGraphErrors * runSenCorrectionEGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corERunIn,zeroRunIn,ecorERunIn);
  runSenCorrectionEGraphUnRegIn1->SetName("runSenCorrectionEGraphUnRegIn1");
  runSenCorrectionEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  runSenCorrectionEGraphUnRegIn1->SetMarkerStyle(20);
  runSenCorrectionEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionEGraphUnRegIn1->SetLineWidth(1);
//   runSenCorrectionEGraphUnRegIn1->Draw("AP");
  runSenCorrectionEGraphUnRegIn1->Fit("fitRunSenCorrectionEGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunSenCorrectionEGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionEUnRegIn1            =  fitRunSenCorrectionEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionEUnRegIn1Error       =  fitRunSenCorrectionEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionEUnRegIn1Chisquare   =  fitRunSenCorrectionEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionEUnRegIn1NDF         =  fitRunSenCorrectionEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionEUnRegIn1Prob        =  fitRunSenCorrectionEGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionEGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corERunOut,zeroRunOut,ecorERunOut);
  runSenCorrectionEGraphUnRegOut1->SetName("runSenCorrectionEGraphUnRegOut1");
  runSenCorrectionEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionEGraphUnRegOut1->SetMarkerStyle(20);
  runSenCorrectionEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionEGraphUnRegOut1->SetLineWidth(1);
//   runSenCorrectionEGraphUnRegOut1->Draw("AP");
  runSenCorrectionEGraphUnRegOut1->Fit("fitRunSenCorrectionEGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionEGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionEUnRegOut1           =  fitRunSenCorrectionEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionEUnRegOut1Error      =  fitRunSenCorrectionEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionEUnRegOut1Chisquare  =  fitRunSenCorrectionEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionEUnRegOut1NDF        =  fitRunSenCorrectionEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionEUnRegOut1Prob       =  fitRunSenCorrectionEGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionEGraphUnReg1 = new TMultiGraph();
  runSenCorrectionEGraphUnReg1->Add(runSenCorrectionEGraphUnRegIn1);
  runSenCorrectionEGraphUnReg1->Add(runSenCorrectionEGraphUnRegOut1);
  runSenCorrectionEGraphUnReg1->Draw("AP");
  runSenCorrectionEGraphUnReg1->SetTitle("");
  runSenCorrectionEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  runSenCorrectionEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionEGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionEGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisRunSenCorrectionEGraphUnReg1= runSenCorrectionEGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionEGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionEUnRegIn1 =(TPaveStats*)runSenCorrectionEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionEUnRegOut1=(TPaveStats*)runSenCorrectionEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionEUnRegIn1->SetTextColor(kBlue);
  statsRunSenCorrectionEUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionEUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionEUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionEUnRegInpOut1       =  (fitResultRunSenCorrectionEUnRegIn1+fitResultRunSenCorrectionEUnRegOut1)/2;
  Double_t resultRunSenCorrectionEUnRegInmOut1       =  (fitResultRunSenCorrectionEUnRegIn1-fitResultRunSenCorrectionEUnRegOut1)/2;
  Double_t resultRunSenCorrectionEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionEUnRegIn1Error*fitResultRunSenCorrectionEUnRegIn1Error+fitResultRunSenCorrectionEUnRegOut1Error*fitResultRunSenCorrectionEUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionEUnRegIn1Error*fitResultRunSenCorrectionEUnRegIn1Error+fitResultRunSenCorrectionEUnRegOut1Error*fitResultRunSenCorrectionEUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionEUnRegIn1,fitResultRunSenCorrectionEUnRegIn1Error),"lp");
  legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionEUnRegOut1,fitResultRunSenCorrectionEUnRegOut1Error),"lp");
//   legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionEUnRegInpOut1,resultRunSenCorrectionEUnRegInpOut1Error),"");
//   legRunSenCorrectionEUnReg1->AddEntry("runSenCorrectionEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionEUnRegInmOut1,resultRunSenCorrectionEUnRegInmOut1Error),"");
  legRunSenCorrectionEUnReg1->SetTextSize(0.045);
  legRunSenCorrectionEUnReg1->SetFillColor(0);
  legRunSenCorrectionEUnReg1->SetBorderSize(2);
  legRunSenCorrectionEUnReg1->Draw();

  gPad->Update();


  pad41->cd(6);

  TGraphErrors * runSenCorrectionTotalGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corTotalRunIn,zeroRunIn,ecorTotalRunIn);
  runSenCorrectionTotalGraphUnRegIn1->SetName("runSenCorrectionTotalGraphUnRegIn1");
  runSenCorrectionTotalGraphUnRegIn1->SetMarkerColor(kCyan);
  runSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  runSenCorrectionTotalGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorrectionTotalGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorrectionTotalGraphUnRegIn1->SetLineWidth(1);
//   runSenCorrectionTotalGraphUnRegIn1->Draw("AP");
  runSenCorrectionTotalGraphUnRegIn1->Fit("fitRunSenCorrectionTotalGraphUnRegIn1","E M R F 0 Q W");
  fitRunSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  fitRunSenCorrectionTotalGraphUnRegIn1->SetLineWidth(2);
  fitRunSenCorrectionTotalGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionTotalUnRegIn1            =  fitRunSenCorrectionTotalGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunSenCorrectionTotalUnRegIn1Error       =  fitRunSenCorrectionTotalGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunSenCorrectionTotalUnRegIn1Chisquare   =  fitRunSenCorrectionTotalGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunSenCorrectionTotalUnRegIn1NDF         =  fitRunSenCorrectionTotalGraphUnRegIn1->GetNDF();
  Double_t fitResultRunSenCorrectionTotalUnRegIn1Prob        =  fitRunSenCorrectionTotalGraphUnRegIn1->GetProb();


  TGraphErrors * runSenCorrectionTotalGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corTotalRunOut,zeroRunOut,ecorTotalRunOut);
  runSenCorrectionTotalGraphUnRegOut1->SetName("runSenCorrectionTotalGraphUnRegOut1");
  runSenCorrectionTotalGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorrectionTotalGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorrectionTotalGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorrectionTotalGraphUnRegOut1->SetLineWidth(1);
//   runSenCorrectionTotalGraphUnRegOut1->Draw("AP");
  runSenCorrectionTotalGraphUnRegOut1->Fit("fitRunSenCorrectionTotalGraphUnRegOut1","E M R F 0 Q W");
  fitRunSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  fitRunSenCorrectionTotalGraphUnRegOut1->SetLineWidth(2);
  fitRunSenCorrectionTotalGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunSenCorrectionTotalUnRegOut1           =  fitRunSenCorrectionTotalGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunSenCorrectionTotalUnRegOut1Error      =  fitRunSenCorrectionTotalGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunSenCorrectionTotalUnRegOut1Chisquare  =  fitRunSenCorrectionTotalGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunSenCorrectionTotalUnRegOut1NDF        =  fitRunSenCorrectionTotalGraphUnRegOut1->GetNDF();
  Double_t fitResultRunSenCorrectionTotalUnRegOut1Prob       =  fitRunSenCorrectionTotalGraphUnRegOut1->GetProb();

  TMultiGraph *runSenCorrectionTotalGraphUnReg1 = new TMultiGraph();
  runSenCorrectionTotalGraphUnReg1->Add(runSenCorrectionTotalGraphUnRegIn1);
  runSenCorrectionTotalGraphUnReg1->Add(runSenCorrectionTotalGraphUnRegOut1);
  runSenCorrectionTotalGraphUnReg1->Draw("AP");
  runSenCorrectionTotalGraphUnReg1->SetTitle("");
  runSenCorrectionTotalGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runSenCorrectionTotalGraphUnReg1->GetYaxis()->SetTitle("Total Correction [ppm]");
  runSenCorrectionTotalGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorrectionTotalGraphUnReg1->GetYaxis()->CenterTitle();
  runSenCorrectionTotalGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runSenCorrectionTotalGraphUnReg1->GetYaxis()->SetRangeUser(0,4);
  TAxis *xaxisRunSenCorrectionTotalGraphUnReg1= runSenCorrectionTotalGraphUnReg1->GetXaxis();
  xaxisRunSenCorrectionTotalGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunSenCorrectionTotalGraphUnRegIn1->Draw("same");
  fitRunSenCorrectionTotalGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunSenCorrectionTotalUnRegIn1 =(TPaveStats*)runSenCorrectionTotalGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunSenCorrectionTotalUnRegOut1=(TPaveStats*)runSenCorrectionTotalGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunSenCorrectionTotalUnRegIn1->SetTextColor(kCyan);
  statsRunSenCorrectionTotalUnRegIn1->SetFillColor(kWhite); 
  statsRunSenCorrectionTotalUnRegOut1->SetTextColor(kBlack);
  statsRunSenCorrectionTotalUnRegOut1->SetFillColor(kWhite); 
  statsRunSenCorrectionTotalUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunSenCorrectionTotalUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunSenCorrectionTotalUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunSenCorrectionTotalUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunSenCorrectionTotalUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunSenCorrectionTotalUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunSenCorrectionTotalUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunSenCorrectionTotalUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunSenCorrectionTotalUnRegInpOut1       =  (fitResultRunSenCorrectionTotalUnRegIn1+fitResultRunSenCorrectionTotalUnRegOut1)/2;
  Double_t resultRunSenCorrectionTotalUnRegInmOut1       =  (fitResultRunSenCorrectionTotalUnRegIn1-fitResultRunSenCorrectionTotalUnRegOut1)/2;
  Double_t resultRunSenCorrectionTotalUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionTotalUnRegIn1Error*fitResultRunSenCorrectionTotalUnRegIn1Error+fitResultRunSenCorrectionTotalUnRegOut1Error*fitResultRunSenCorrectionTotalUnRegOut1Error)/2;
  Double_t resultRunSenCorrectionTotalUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunSenCorrectionTotalUnRegIn1Error*fitResultRunSenCorrectionTotalUnRegIn1Error+fitResultRunSenCorrectionTotalUnRegOut1Error*fitResultRunSenCorrectionTotalUnRegOut1Error)/2;


  TLegend *legRunSenCorrectionTotalUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionTotalUnRegIn1,fitResultRunSenCorrectionTotalUnRegIn1Error),"lp");
  legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunSenCorrectionTotalUnRegOut1,fitResultRunSenCorrectionTotalUnRegOut1Error),"lp");
//   legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionTotalUnRegInpOut1,resultRunSenCorrectionTotalUnRegInpOut1Error),"");
//   legRunSenCorrectionTotalUnReg1->AddEntry("runSenCorrectionTotalGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunSenCorrectionTotalUnRegInmOut1,resultRunSenCorrectionTotalUnRegInmOut1Error),"");
  legRunSenCorrectionTotalUnReg1->SetTextSize(0.045);
  legRunSenCorrectionTotalUnReg1->SetFillColor(0);
  legRunSenCorrectionTotalUnReg1->SetBorderSize(2);
  legRunSenCorrectionTotalUnReg1->Draw();

  gPad->Update();


  /*********************************************************************************/
  c4->Update();
  c4->SaveAs(Form("%s/plots/%s/%sCorrectionRunbyRun%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c41 = new TCanvas("c41","c41",cx1,cy1,1200,950);
  TPad * pad410 = new TPad("pad410","pad410",ps1,ps2,ps4,ps4);
  TPad * pad411 = new TPad("pad411","pad411",ps1,ps1,ps4,ps3);
  pad410->Draw();
  pad411->Draw();
  pad410->cd();
  TText * ct410 = new TText(tll,tlr,Form("%sN to Delta %s Corrections using %s Slug Averaged Sensitivities and Runlet Differences",regTitle,dataInfo,plotTitle));
  ct410->SetTextSize(tsiz);
  ct410->Draw();
  pad411->cd();
  pad411->Divide(2,3);


  pad411->cd(1);

  TGraphErrors * slugSenCorrectionXGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corXSlugIn,zeroRunIn,ecorXSlugIn);
  slugSenCorrectionXGraphUnRegIn1->SetName("slugSenCorrectionXGraphUnRegIn1");
  slugSenCorrectionXGraphUnRegIn1->SetMarkerColor(kRed);
  slugSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  slugSenCorrectionXGraphUnRegIn1->SetMarkerStyle(21);
  slugSenCorrectionXGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionXGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionXGraphUnRegIn1->Fit("fitSlugSenCorrectionXGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionXGraphUnRegIn1->SetLineColor(kRed);
  fitSlugSenCorrectionXGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXUnRegIn1            =  fitSlugSenCorrectionXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXUnRegIn1Error       =  fitSlugSenCorrectionXGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXUnRegIn1Chisquare   =  fitSlugSenCorrectionXGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXUnRegIn1NDF         =  fitSlugSenCorrectionXGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionXUnRegIn1Prob        =  fitSlugSenCorrectionXGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionXGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corXSlugOut,zeroRunOut,ecorXSlugOut);
  slugSenCorrectionXGraphUnRegOut1->SetName("slugSenCorrectionXGraphUnRegOut1");
  slugSenCorrectionXGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionXGraphUnRegOut1->SetMarkerStyle(21);
  slugSenCorrectionXGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionXGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionXGraphUnRegOut1->Fit("fitSlugSenCorrectionXGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionXGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionXGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXUnRegOut1           =  fitSlugSenCorrectionXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXUnRegOut1Error      =  fitSlugSenCorrectionXGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXUnRegOut1Chisquare  =  fitSlugSenCorrectionXGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXUnRegOut1NDF        =  fitSlugSenCorrectionXGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionXUnRegOut1Prob       =  fitSlugSenCorrectionXGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionXGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionXGraphUnReg1->Add(slugSenCorrectionXGraphUnRegIn1);
  slugSenCorrectionXGraphUnReg1->Add(slugSenCorrectionXGraphUnRegOut1);
  slugSenCorrectionXGraphUnReg1->Draw("AP");
  slugSenCorrectionXGraphUnReg1->SetTitle("");
  slugSenCorrectionXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionXGraphUnReg1->GetYaxis()->SetTitle("X Correction [ppm]");
  slugSenCorrectionXGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionXGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionXGraphUnReg1->GetYaxis()->SetRangeUser(0,4);
  TAxis *xaxisSlugSenCorrectionXGraphUnReg1= slugSenCorrectionXGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionXGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionXGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionXUnRegIn1 =(TPaveStats*)slugSenCorrectionXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionXUnRegOut1=(TPaveStats*)slugSenCorrectionXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionXUnRegIn1->SetTextColor(kRed);
  statsSlugSenCorrectionXUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionXUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionXUnRegInpOut1       =  (fitResultSlugSenCorrectionXUnRegIn1+fitResultSlugSenCorrectionXUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXUnRegInmOut1       =  (fitResultSlugSenCorrectionXUnRegIn1-fitResultSlugSenCorrectionXUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXUnRegIn1Error*fitResultSlugSenCorrectionXUnRegIn1Error+fitResultSlugSenCorrectionXUnRegOut1Error*fitResultSlugSenCorrectionXUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionXUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXUnRegIn1Error*fitResultSlugSenCorrectionXUnRegIn1Error+fitResultSlugSenCorrectionXUnRegOut1Error*fitResultSlugSenCorrectionXUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionXUnRegIn1,fitResultSlugSenCorrectionXUnRegIn1Error),"lp");
  legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionXUnRegOut1,fitResultSlugSenCorrectionXUnRegOut1Error),"lp");
//   legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionXUnRegInpOut1,resultSlugSenCorrectionXUnRegInpOut1Error),"");
//   legSlugSenCorrectionXUnReg1->AddEntry("slugSenCorrectionXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionXUnRegInmOut1,resultSlugSenCorrectionXUnRegInmOut1Error),"");
  legSlugSenCorrectionXUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionXUnReg1->SetFillColor(0);
  legSlugSenCorrectionXUnReg1->SetBorderSize(2);
  legSlugSenCorrectionXUnReg1->Draw();

  gPad->Update();


  pad411->cd(2);

  TGraphErrors * slugSenCorrectionXSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corXSlopeSlugIn,zeroRunIn,ecorXSlopeSlugIn);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetName("slugSenCorrectionXSlopeGraphUnRegIn1");
  slugSenCorrectionXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetMarkerStyle(22);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionXSlopeGraphUnRegIn1->Fit("fitSlugSenCorrectionXSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1            =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1Error       =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1Chisquare   =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1NDF         =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegIn1Prob        =  fitSlugSenCorrectionXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionXSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corXSlopeSlugOut,zeroRunOut,ecorXSlopeSlugOut);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetName("slugSenCorrectionXSlopeGraphUnRegOut1");
  slugSenCorrectionXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetMarkerStyle(22);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionXSlopeGraphUnRegOut1->Fit("fitSlugSenCorrectionXSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1           =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1Error      =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1Chisquare  =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1NDF        =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionXSlopeUnRegOut1Prob       =  fitSlugSenCorrectionXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionXSlopeGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionXSlopeGraphUnReg1->Add(slugSenCorrectionXSlopeGraphUnRegIn1);
  slugSenCorrectionXSlopeGraphUnReg1->Add(slugSenCorrectionXSlopeGraphUnRegOut1);
  slugSenCorrectionXSlopeGraphUnReg1->Draw("AP");
  slugSenCorrectionXSlopeGraphUnReg1->SetTitle("");
  slugSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Correction [ppm]");
  slugSenCorrectionXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisSlugSenCorrectionXSlopeGraphUnReg1= slugSenCorrectionXSlopeGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionXSlopeGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionXSlopeUnRegIn1 =(TPaveStats*)slugSenCorrectionXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionXSlopeUnRegOut1=(TPaveStats*)slugSenCorrectionXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionXSlopeUnRegIn1->SetTextColor(kRed);
  statsSlugSenCorrectionXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXSlopeUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionXSlopeUnRegInpOut1       =  (fitResultSlugSenCorrectionXSlopeUnRegIn1+fitResultSlugSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXSlopeUnRegInmOut1       =  (fitResultSlugSenCorrectionXSlopeUnRegIn1-fitResultSlugSenCorrectionXSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXSlopeUnRegIn1Error*fitResultSlugSenCorrectionXSlopeUnRegIn1Error+fitResultSlugSenCorrectionXSlopeUnRegOut1Error*fitResultSlugSenCorrectionXSlopeUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionXSlopeUnRegIn1Error*fitResultSlugSenCorrectionXSlopeUnRegIn1Error+fitResultSlugSenCorrectionXSlopeUnRegOut1Error*fitResultSlugSenCorrectionXSlopeUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionXSlopeUnRegIn1,fitResultSlugSenCorrectionXSlopeUnRegIn1Error),"lp");
  legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionXSlopeUnRegOut1,fitResultSlugSenCorrectionXSlopeUnRegOut1Error),"lp");
//   legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionXSlopeUnRegInpOut1,resultSlugSenCorrectionXSlopeUnRegInpOut1Error),"");
//   legSlugSenCorrectionXSlopeUnReg1->AddEntry("slugSenCorrectionXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionXSlopeUnRegInmOut1,resultSlugSenCorrectionXSlopeUnRegInmOut1Error),"");
  legSlugSenCorrectionXSlopeUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionXSlopeUnReg1->SetFillColor(0);
  legSlugSenCorrectionXSlopeUnReg1->SetBorderSize(2);
  legSlugSenCorrectionXSlopeUnReg1->Draw();

  gPad->Update();


  pad411->cd(3);

  TGraphErrors * slugSenCorrectionYGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corYSlugIn,zeroRunIn,ecorYSlugIn);
  slugSenCorrectionYGraphUnRegIn1->SetName("slugSenCorrectionYGraphUnRegIn1");
  slugSenCorrectionYGraphUnRegIn1->SetMarkerColor(kGreen);
  slugSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  slugSenCorrectionYGraphUnRegIn1->SetMarkerStyle(21);
  slugSenCorrectionYGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionYGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionYGraphUnRegIn1->Fit("fitSlugSenCorrectionYGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionYGraphUnRegIn1->SetLineColor(kGreen);
  fitSlugSenCorrectionYGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYUnRegIn1            =  fitSlugSenCorrectionYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYUnRegIn1Error       =  fitSlugSenCorrectionYGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYUnRegIn1Chisquare   =  fitSlugSenCorrectionYGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYUnRegIn1NDF         =  fitSlugSenCorrectionYGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionYUnRegIn1Prob        =  fitSlugSenCorrectionYGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionYGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corYSlugOut,zeroRunOut,ecorYSlugOut);
  slugSenCorrectionYGraphUnRegOut1->SetName("slugSenCorrectionYGraphUnRegOut1");
  slugSenCorrectionYGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionYGraphUnRegOut1->SetMarkerStyle(21);
  slugSenCorrectionYGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionYGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionYGraphUnRegOut1->Fit("fitSlugSenCorrectionYGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionYGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionYGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYUnRegOut1           =  fitSlugSenCorrectionYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYUnRegOut1Error      =  fitSlugSenCorrectionYGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYUnRegOut1Chisquare  =  fitSlugSenCorrectionYGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYUnRegOut1NDF        =  fitSlugSenCorrectionYGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionYUnRegOut1Prob       =  fitSlugSenCorrectionYGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionYGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionYGraphUnReg1->Add(slugSenCorrectionYGraphUnRegIn1);
  slugSenCorrectionYGraphUnReg1->Add(slugSenCorrectionYGraphUnRegOut1);
  slugSenCorrectionYGraphUnReg1->Draw("AP");
  slugSenCorrectionYGraphUnReg1->SetTitle("");
  slugSenCorrectionYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionYGraphUnReg1->GetYaxis()->SetTitle("Y Correction [ppm]");
  slugSenCorrectionYGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionYGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionYGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisSlugSenCorrectionYGraphUnReg1= slugSenCorrectionYGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionYGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionYGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionYUnRegIn1 =(TPaveStats*)slugSenCorrectionYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionYUnRegOut1=(TPaveStats*)slugSenCorrectionYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionYUnRegIn1->SetTextColor(kGreen);
  statsSlugSenCorrectionYUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionYUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionYUnRegInpOut1       =  (fitResultSlugSenCorrectionYUnRegIn1+fitResultSlugSenCorrectionYUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYUnRegInmOut1       =  (fitResultSlugSenCorrectionYUnRegIn1-fitResultSlugSenCorrectionYUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYUnRegIn1Error*fitResultSlugSenCorrectionYUnRegIn1Error+fitResultSlugSenCorrectionYUnRegOut1Error*fitResultSlugSenCorrectionYUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionYUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYUnRegIn1Error*fitResultSlugSenCorrectionYUnRegIn1Error+fitResultSlugSenCorrectionYUnRegOut1Error*fitResultSlugSenCorrectionYUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionYUnRegIn1,fitResultSlugSenCorrectionYUnRegIn1Error),"lp");
  legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionYUnRegOut1,fitResultSlugSenCorrectionYUnRegOut1Error),"lp");
//   legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionYUnRegInpOut1,resultSlugSenCorrectionYUnRegInpOut1Error),"");
//   legSlugSenCorrectionYUnReg1->AddEntry("slugSenCorrectionYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionYUnRegInmOut1,resultSlugSenCorrectionYUnRegInmOut1Error),"");
  legSlugSenCorrectionYUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionYUnReg1->SetFillColor(0);
  legSlugSenCorrectionYUnReg1->SetBorderSize(2);
  legSlugSenCorrectionYUnReg1->Draw();

  gPad->Update();


  pad411->cd(4);

  TGraphErrors * slugSenCorrectionYSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corYSlopeSlugIn,zeroRunIn,ecorYSlopeSlugIn);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetName("slugSenCorrectionYSlopeGraphUnRegIn1");
  slugSenCorrectionYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetMarkerStyle(22);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(1);
  slugSenCorrectionYSlopeGraphUnRegIn1->Fit("fitSlugSenCorrectionYSlopeGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1            =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1Error       =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1Chisquare   =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1NDF         =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegIn1Prob        =  fitSlugSenCorrectionYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionYSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corYSlopeSlugOut,zeroRunOut,ecorYSlopeSlugOut);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetName("slugSenCorrectionYSlopeGraphUnRegOut1");
  slugSenCorrectionYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetMarkerStyle(22);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(1);
  slugSenCorrectionYSlopeGraphUnRegOut1->Fit("fitSlugSenCorrectionYSlopeGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1           =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1Error      =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1Chisquare  =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1NDF        =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionYSlopeUnRegOut1Prob       =  fitSlugSenCorrectionYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionYSlopeGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionYSlopeGraphUnReg1->Add(slugSenCorrectionYSlopeGraphUnRegIn1);
  slugSenCorrectionYSlopeGraphUnReg1->Add(slugSenCorrectionYSlopeGraphUnRegOut1);
  slugSenCorrectionYSlopeGraphUnReg1->Draw("AP");
  slugSenCorrectionYSlopeGraphUnReg1->SetTitle("");
  slugSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  slugSenCorrectionYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisSlugSenCorrectionYSlopeGraphUnReg1= slugSenCorrectionYSlopeGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionYSlopeGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionYSlopeUnRegIn1 =(TPaveStats*)slugSenCorrectionYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionYSlopeUnRegOut1=(TPaveStats*)slugSenCorrectionYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionYSlopeUnRegIn1->SetTextColor(kGreen);
  statsSlugSenCorrectionYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYSlopeUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionYSlopeUnRegInpOut1       =  (fitResultSlugSenCorrectionYSlopeUnRegIn1+fitResultSlugSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYSlopeUnRegInmOut1       =  (fitResultSlugSenCorrectionYSlopeUnRegIn1-fitResultSlugSenCorrectionYSlopeUnRegOut1)/2;
  Double_t resultSlugSenCorrectionYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYSlopeUnRegIn1Error*fitResultSlugSenCorrectionYSlopeUnRegIn1Error+fitResultSlugSenCorrectionYSlopeUnRegOut1Error*fitResultSlugSenCorrectionYSlopeUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionYSlopeUnRegIn1Error*fitResultSlugSenCorrectionYSlopeUnRegIn1Error+fitResultSlugSenCorrectionYSlopeUnRegOut1Error*fitResultSlugSenCorrectionYSlopeUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionYSlopeUnRegIn1,fitResultSlugSenCorrectionYSlopeUnRegIn1Error),"lp");
  legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultSlugSenCorrectionYSlopeUnRegOut1,fitResultSlugSenCorrectionYSlopeUnRegOut1Error),"lp");
//   legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionYSlopeUnRegInpOut1,resultSlugSenCorrectionYSlopeUnRegInpOut1Error),"");
//   legSlugSenCorrectionYSlopeUnReg1->AddEntry("slugSenCorrectionYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultSlugSenCorrectionYSlopeUnRegInmOut1,resultSlugSenCorrectionYSlopeUnRegInmOut1Error),"");
  legSlugSenCorrectionYSlopeUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionYSlopeUnReg1->SetFillColor(0);
  legSlugSenCorrectionYSlopeUnReg1->SetBorderSize(2);
  legSlugSenCorrectionYSlopeUnReg1->Draw();

  gPad->Update();


  pad411->cd(5);

  TGraphErrors * slugSenCorrectionEGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corESlugIn,zeroRunIn,ecorESlugIn);
  slugSenCorrectionEGraphUnRegIn1->SetName("slugSenCorrectionEGraphUnRegIn1");
  slugSenCorrectionEGraphUnRegIn1->SetMarkerColor(kBlue);
  slugSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  slugSenCorrectionEGraphUnRegIn1->SetMarkerStyle(20);
  slugSenCorrectionEGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionEGraphUnRegIn1->SetLineWidth(1);
//   slugSenCorrectionEGraphUnRegIn1->Draw("AP");
  slugSenCorrectionEGraphUnRegIn1->Fit("fitSlugSenCorrectionEGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionEGraphUnRegIn1->SetLineColor(kBlue);
  fitSlugSenCorrectionEGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionEUnRegIn1            =  fitSlugSenCorrectionEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionEUnRegIn1Error       =  fitSlugSenCorrectionEGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionEUnRegIn1Chisquare   =  fitSlugSenCorrectionEGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionEUnRegIn1NDF         =  fitSlugSenCorrectionEGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionEUnRegIn1Prob        =  fitSlugSenCorrectionEGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionEGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corESlugOut,zeroRunOut,ecorESlugOut);
  slugSenCorrectionEGraphUnRegOut1->SetName("slugSenCorrectionEGraphUnRegOut1");
  slugSenCorrectionEGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionEGraphUnRegOut1->SetMarkerStyle(20);
  slugSenCorrectionEGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionEGraphUnRegOut1->SetLineWidth(1);
//   slugSenCorrectionEGraphUnRegOut1->Draw("AP");
  slugSenCorrectionEGraphUnRegOut1->Fit("fitSlugSenCorrectionEGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionEGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionEGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionEUnRegOut1           =  fitSlugSenCorrectionEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionEUnRegOut1Error      =  fitSlugSenCorrectionEGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionEUnRegOut1Chisquare  =  fitSlugSenCorrectionEGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionEUnRegOut1NDF        =  fitSlugSenCorrectionEGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionEUnRegOut1Prob       =  fitSlugSenCorrectionEGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionEGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionEGraphUnReg1->Add(slugSenCorrectionEGraphUnRegIn1);
  slugSenCorrectionEGraphUnReg1->Add(slugSenCorrectionEGraphUnRegOut1);
  slugSenCorrectionEGraphUnReg1->Draw("AP");
  slugSenCorrectionEGraphUnReg1->SetTitle("");
  slugSenCorrectionEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  slugSenCorrectionEGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionEGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionEGraphUnReg1->GetYaxis()->SetRangeUser(-2,2);
  TAxis *xaxisSlugSenCorrectionEGraphUnReg1= slugSenCorrectionEGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionEGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionEGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionEUnRegIn1 =(TPaveStats*)slugSenCorrectionEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionEUnRegOut1=(TPaveStats*)slugSenCorrectionEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionEUnRegIn1->SetTextColor(kBlue);
  statsSlugSenCorrectionEUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionEUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionEUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionEUnRegInpOut1       =  (fitResultSlugSenCorrectionEUnRegIn1+fitResultSlugSenCorrectionEUnRegOut1)/2;
  Double_t resultSlugSenCorrectionEUnRegInmOut1       =  (fitResultSlugSenCorrectionEUnRegIn1-fitResultSlugSenCorrectionEUnRegOut1)/2;
  Double_t resultSlugSenCorrectionEUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionEUnRegIn1Error*fitResultSlugSenCorrectionEUnRegIn1Error+fitResultSlugSenCorrectionEUnRegOut1Error*fitResultSlugSenCorrectionEUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionEUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionEUnRegIn1Error*fitResultSlugSenCorrectionEUnRegIn1Error+fitResultSlugSenCorrectionEUnRegOut1Error*fitResultSlugSenCorrectionEUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionEUnRegIn1,fitResultSlugSenCorrectionEUnRegIn1Error),"lp");
  legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionEUnRegOut1,fitResultSlugSenCorrectionEUnRegOut1Error),"lp");
//   legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionEUnRegInpOut1,resultSlugSenCorrectionEUnRegInpOut1Error),"");
//   legSlugSenCorrectionEUnReg1->AddEntry("slugSenCorrectionEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionEUnRegInmOut1,resultSlugSenCorrectionEUnRegInmOut1Error),"");
  legSlugSenCorrectionEUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionEUnReg1->SetFillColor(0);
  legSlugSenCorrectionEUnReg1->SetBorderSize(2);
  legSlugSenCorrectionEUnReg1->Draw();

  gPad->Update();


  pad411->cd(6);

  TGraphErrors * slugSenCorrectionTotalGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,corTotalSlugIn,zeroRunIn,ecorTotalSlugIn);
  slugSenCorrectionTotalGraphUnRegIn1->SetName("slugSenCorrectionTotalGraphUnRegIn1");
  slugSenCorrectionTotalGraphUnRegIn1->SetMarkerColor(kCyan);
  slugSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  slugSenCorrectionTotalGraphUnRegIn1->SetMarkerStyle(21);
  slugSenCorrectionTotalGraphUnRegIn1->SetMarkerSize(0.5);
  slugSenCorrectionTotalGraphUnRegIn1->SetLineWidth(1);
//   slugSenCorrectionTotalGraphUnRegIn1->Draw("AP");
  slugSenCorrectionTotalGraphUnRegIn1->Fit("fitSlugSenCorrectionTotalGraphUnRegIn1","E M R F 0 Q W");
  fitSlugSenCorrectionTotalGraphUnRegIn1->SetLineColor(kCyan);
  fitSlugSenCorrectionTotalGraphUnRegIn1->SetLineWidth(2);
  fitSlugSenCorrectionTotalGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionTotalUnRegIn1            =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1Error       =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetParError(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1Chisquare   =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetChisquare();
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1NDF         =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetNDF();
  Double_t fitResultSlugSenCorrectionTotalUnRegIn1Prob        =  fitSlugSenCorrectionTotalGraphUnRegIn1->GetProb();


  TGraphErrors * slugSenCorrectionTotalGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,corTotalSlugOut,zeroRunOut,ecorTotalSlugOut);
  slugSenCorrectionTotalGraphUnRegOut1->SetName("slugSenCorrectionTotalGraphUnRegOut1");
  slugSenCorrectionTotalGraphUnRegOut1->SetMarkerColor(kBlack);
  slugSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  slugSenCorrectionTotalGraphUnRegOut1->SetMarkerStyle(21);
  slugSenCorrectionTotalGraphUnRegOut1->SetMarkerSize(0.5);
  slugSenCorrectionTotalGraphUnRegOut1->SetLineWidth(1);
//   slugSenCorrectionTotalGraphUnRegOut1->Draw("AP");
  slugSenCorrectionTotalGraphUnRegOut1->Fit("fitSlugSenCorrectionTotalGraphUnRegOut1","E M R F 0 Q W");
  fitSlugSenCorrectionTotalGraphUnRegOut1->SetLineColor(kBlack);
  fitSlugSenCorrectionTotalGraphUnRegOut1->SetLineWidth(2);
  fitSlugSenCorrectionTotalGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultSlugSenCorrectionTotalUnRegOut1           =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetParameter(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1Error      =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetParError(0);
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1Chisquare  =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetChisquare();
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1NDF        =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetNDF();
  Double_t fitResultSlugSenCorrectionTotalUnRegOut1Prob       =  fitSlugSenCorrectionTotalGraphUnRegOut1->GetProb();

  TMultiGraph *slugSenCorrectionTotalGraphUnReg1 = new TMultiGraph();
  slugSenCorrectionTotalGraphUnReg1->Add(slugSenCorrectionTotalGraphUnRegIn1);
  slugSenCorrectionTotalGraphUnReg1->Add(slugSenCorrectionTotalGraphUnRegOut1);
  slugSenCorrectionTotalGraphUnReg1->Draw("AP");
  slugSenCorrectionTotalGraphUnReg1->SetTitle("");
  slugSenCorrectionTotalGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  slugSenCorrectionTotalGraphUnReg1->GetYaxis()->SetTitle("Total Correction [ppm]");
  slugSenCorrectionTotalGraphUnReg1->GetXaxis()->CenterTitle();
  slugSenCorrectionTotalGraphUnReg1->GetYaxis()->CenterTitle();
  slugSenCorrectionTotalGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  slugSenCorrectionTotalGraphUnReg1->GetYaxis()->SetRangeUser(0,4);
  TAxis *xaxisSlugSenCorrectionTotalGraphUnReg1= slugSenCorrectionTotalGraphUnReg1->GetXaxis();
  xaxisSlugSenCorrectionTotalGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitSlugSenCorrectionTotalGraphUnRegIn1->Draw("same");
  fitSlugSenCorrectionTotalGraphUnRegOut1->Draw("same");

  TPaveStats *statsSlugSenCorrectionTotalUnRegIn1 =(TPaveStats*)slugSenCorrectionTotalGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsSlugSenCorrectionTotalUnRegOut1=(TPaveStats*)slugSenCorrectionTotalGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsSlugSenCorrectionTotalUnRegIn1->SetTextColor(kCyan);
  statsSlugSenCorrectionTotalUnRegIn1->SetFillColor(kWhite); 
  statsSlugSenCorrectionTotalUnRegOut1->SetTextColor(kBlack);
  statsSlugSenCorrectionTotalUnRegOut1->SetFillColor(kWhite); 
  statsSlugSenCorrectionTotalUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsSlugSenCorrectionTotalUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsSlugSenCorrectionTotalUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsSlugSenCorrectionTotalUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsSlugSenCorrectionTotalUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsSlugSenCorrectionTotalUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsSlugSenCorrectionTotalUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsSlugSenCorrectionTotalUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultSlugSenCorrectionTotalUnRegInpOut1       =  (fitResultSlugSenCorrectionTotalUnRegIn1+fitResultSlugSenCorrectionTotalUnRegOut1)/2;
  Double_t resultSlugSenCorrectionTotalUnRegInmOut1       =  (fitResultSlugSenCorrectionTotalUnRegIn1-fitResultSlugSenCorrectionTotalUnRegOut1)/2;
  Double_t resultSlugSenCorrectionTotalUnRegInpOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionTotalUnRegIn1Error*fitResultSlugSenCorrectionTotalUnRegIn1Error+fitResultSlugSenCorrectionTotalUnRegOut1Error*fitResultSlugSenCorrectionTotalUnRegOut1Error)/2;
  Double_t resultSlugSenCorrectionTotalUnRegInmOut1Error  =  TMath::Sqrt(fitResultSlugSenCorrectionTotalUnRegIn1Error*fitResultSlugSenCorrectionTotalUnRegIn1Error+fitResultSlugSenCorrectionTotalUnRegOut1Error*fitResultSlugSenCorrectionTotalUnRegOut1Error)/2;


  TLegend *legSlugSenCorrectionTotalUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionTotalUnRegIn1,fitResultSlugSenCorrectionTotalUnRegIn1Error),"lp");
  legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultSlugSenCorrectionTotalUnRegOut1,fitResultSlugSenCorrectionTotalUnRegOut1Error),"lp");
//   legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionTotalUnRegInpOut1,resultSlugSenCorrectionTotalUnRegInpOut1Error),"");
//   legSlugSenCorrectionTotalUnReg1->AddEntry("slugSenCorrectionTotalGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultSlugSenCorrectionTotalUnRegInmOut1,resultSlugSenCorrectionTotalUnRegInmOut1Error),"");
  legSlugSenCorrectionTotalUnReg1->SetTextSize(0.045);
  legSlugSenCorrectionTotalUnReg1->SetFillColor(0);
  legSlugSenCorrectionTotalUnReg1->SetBorderSize(2);
  legSlugSenCorrectionTotalUnReg1->Draw();

  gPad->Update();

  /*********************************************************************************/
  c41->Update();
  c41->SaveAs(Form("%s/plots/%s/%sCorrectionRunSlugbySlug%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/



  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/


  if(RUNLET_CORRELATION_PLOT){

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat("eMr");
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.30);
  gStyle->SetStatH(0.20);
  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatColor(0);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TFile *f = new TFile("/w/hallc/qweak/nur/n2delta/dataBaseAnalysis/testAllSen.root");

  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);

  TString type  = "prof";
//   TString type  = "";

  const Int_t NUM = 500;
  int counter=0;
  ifstream inSenCorXXSlope,outSenCorXXSlope,inSenCorYYSlope,outSenCorYYSlope,inSenCorXE,outSenCorXE,inAllSen,outAllSen;
  inAllSen.open((Form("%s/dataBaseAnalysis/testAllSenIn.txt",n2deltaAnalysisDir.Data())));
  outAllSen.open((Form("%s/dataBaseAnalysis/testAllSenOut.txt",n2deltaAnalysisDir.Data())));

  Double_t zero[NUM],run_number_in[NUM],X_in[NUM],eX_in[NUM],XSlope_in[NUM],eXSlope_in[NUM];
  Double_t run_number_out[NUM],X_out[NUM],eX_out[NUM],XSlope_out[NUM],eXSlope_out[NUM];
  Double_t Y_in[NUM],eY_in[NUM],YSlope_in[NUM],eYSlope_in[NUM];
  Double_t Y_out[NUM],eY_out[NUM],YSlope_out[NUM],eYSlope_out[NUM];
  Double_t E_in[NUM],eE_in[NUM];
  Double_t E_out[NUM],eE_out[NUM];

  while(1) {
    zero[counter]=0;


    inAllSen >> run_number_in[counter];
    inAllSen >> X_in[counter];
    inAllSen >> eX_in[counter];
    inAllSen >> XSlope_in[counter];
    inAllSen >> eXSlope_in[counter];
    inAllSen >> Y_in[counter];
    inAllSen >> eY_in[counter];
    inAllSen >> YSlope_in[counter];
    inAllSen >> eYSlope_in[counter];
    inAllSen >> E_in[counter];
    inAllSen >> eE_in[counter];
 
    if (!inAllSen.good()) break;

    outAllSen >> run_number_out[counter];
    outAllSen >> X_out[counter];
    outAllSen >> eX_out[counter];
    outAllSen >> XSlope_out[counter];
    outAllSen >> eXSlope_out[counter];
    outAllSen >> Y_out[counter];
    outAllSen >> eY_out[counter];
    outAllSen >> YSlope_out[counter];
    outAllSen >> eYSlope_out[counter];
    outAllSen >> E_out[counter];
    outAllSen >> eE_out[counter];

   if (!outAllSen.good()) break;



//     std::cout << run_number_in[counter] << X_in[counter] << " X In\t" << eX_in[counter] << XSlope_in[counter] << eXSlope_in[counter] << std::endl;
//     std::cout << run_number_out[counter] << X_out[counter] << "X Out\t" << eX_out[counter] << XSlope_out[counter] << eXSlope_out[counter] << std::endl;
//     std::cout << run_number_in[counter] << Y_in[counter] << " Y In\t" << eY_in[counter] << YSlope_in[counter] << eYSlope_in[counter] << std::endl;
//     std::cout << run_number_out[counter] << Y_out[counter] << "Y Out\t" << eY_out[counter] << YSlope_out[counter] << eYSlope_out[counter] << std::endl;


    counter++;
  }

  inAllSen.close();
  outAllSen.close();

  TCanvas *c9 = new TCanvas("c9","c9",cx1,cy1,1200,950);
  TPad * pad90 = new TPad("pad90","pad90",ps1,ps2,ps4,ps4);
  TPad * pad91 = new TPad("pad91","pad91",ps1,ps1,ps4,ps3);
  pad90->Draw();
  pad91->Draw();
  pad90->cd();
  TText * ct90 = new TText(tll,tlr,Form("%sN to Delta %s %s Sensitivity Correlations",regTitle,dataInfo,plotTitle));
  ct90->SetTextSize(tsiz);
  ct90->Draw();
  pad91->cd();
  pad91->Divide(2,4);


  pad91->cd(1);

  TGraphErrors * runSenCorXXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,senXRegInVal,senXSlopeRegInVal,esenXRegInVal,esenXSlopeRegInVal);
  runSenCorXXSlopeGraphUnRegIn1->SetName("runSenCorXXSlopeGraphUnRegIn1");
  runSenCorXXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorXXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenCorXXSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorXXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorXXSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorXXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,senXRegOutVal,senXSlopeRegOutVal,esenXRegOutVal,esenXSlopeRegOutVal);
  runSenCorXXSlopeGraphUnRegOut1->SetName("runSenCorXXSlopeGraphUnRegOut1");
  runSenCorXXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorXXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorXXSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorXXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorXXSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorXXSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorXXSlopeGraphUnReg1->Add(runSenCorXXSlopeGraphUnRegIn1);
  runSenCorXXSlopeGraphUnReg1->Add(runSenCorXXSlopeGraphUnRegOut1);
  runSenCorXXSlopeGraphUnReg1->Draw("AP");
  runSenCorXXSlopeGraphUnReg1->SetTitle("");
  runSenCorXXSlopeGraphUnReg1->GetXaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenCorXXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Sensitivity [ppm/#murad]");
  runSenCorXXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorXXSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorXXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorXXSlopeUnReg1->AddEntry("runSenCorXXSlopeGraphUnRegIn1",  Form("In Sen X vs XSlope"),"lp");
  legRunSenCorXXSlopeUnReg1->AddEntry("runSenCorXXSlopeGraphUnRegOut1",  Form("Out Sen X vs XSlope"),"lp");
  legRunSenCorXXSlopeUnReg1->SetTextSize(0.055);
  legRunSenCorXXSlopeUnReg1->SetFillColor(0);
  legRunSenCorXXSlopeUnReg1->SetBorderSize(2);
  legRunSenCorXXSlopeUnReg1->Draw();

  gPad->Update();


  pad91->cd(2);

  TGraphErrors * runSenCorYYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,senYRegInVal,senYSlopeRegInVal,esenYRegInVal,esenYSlopeRegInVal);
  runSenCorYYSlopeGraphUnRegIn1->SetName("runSenCorYYSlopeGraphUnRegIn1");
  runSenCorYYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorYYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorYYSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorYYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorYYSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorYYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,senYRegOutVal,senYSlopeRegOutVal,esenYRegOutVal,esenYSlopeRegOutVal);
  runSenCorYYSlopeGraphUnRegOut1->SetName("runSenCorYYSlopeGraphUnRegOut1");
  runSenCorYYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorYYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorYYSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorYYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorYYSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorYYSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorYYSlopeGraphUnReg1->Add(runSenCorYYSlopeGraphUnRegIn1);
  runSenCorYYSlopeGraphUnReg1->Add(runSenCorYYSlopeGraphUnRegOut1);
  runSenCorYYSlopeGraphUnReg1->Draw("AP");
  runSenCorYYSlopeGraphUnReg1->SetTitle("");
  runSenCorYYSlopeGraphUnReg1->GetXaxis()->SetTitle("Y Sensitivity [ppm/mm]");
  runSenCorYYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Sensitivity [ppm/#murad]");
  runSenCorYYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorYYSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorYYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorYYSlopeUnReg1->AddEntry("runSenCorYYSlopeGraphUnRegIn1",  Form("In Sen Y vs YSlope"),"lp");
  legRunSenCorYYSlopeUnReg1->AddEntry("runSenCorYYSlopeGraphUnRegOut1",  Form("Out Sen Y vs YSlope"),"lp");
  legRunSenCorYYSlopeUnReg1->SetTextSize(0.055);
  legRunSenCorYYSlopeUnReg1->SetFillColor(0);
  legRunSenCorYYSlopeUnReg1->SetBorderSize(2);
  legRunSenCorYYSlopeUnReg1->Draw();

  gPad->Update();

  pad91->cd(3);

  TGraphErrors * runSenCorXEGraphUnRegIn1 = new TGraphErrors(counterRegIn,senXRegInVal,senERegInVal,esenXRegInVal,esenERegInVal);
  runSenCorXEGraphUnRegIn1->SetName("runSenCorXEGraphUnRegIn1");
  runSenCorXEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenCorXEGraphUnRegIn1->SetLineColor(kBlue);
  runSenCorXEGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorXEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorXEGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorXEGraphUnRegOut1 = new TGraphErrors(counterRegOut,senXRegOutVal,senERegOutVal,esenXRegOutVal,esenERegOutVal);
  runSenCorXEGraphUnRegOut1->SetName("runSenCorXEGraphUnRegOut1");
  runSenCorXEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorXEGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorXEGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorXEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorXEGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorXEGraphUnReg1 = new TMultiGraph();
  runSenCorXEGraphUnReg1->Add(runSenCorXEGraphUnRegIn1);
  runSenCorXEGraphUnReg1->Add(runSenCorXEGraphUnRegOut1);
  runSenCorXEGraphUnReg1->Draw("AP");
  runSenCorXEGraphUnReg1->SetTitle("");
  runSenCorXEGraphUnReg1->GetXaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenCorXEGraphUnReg1->GetYaxis()->SetTitle("E Sensitivity [dim. less]");
  runSenCorXEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorXEGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorXEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorXEUnReg1->AddEntry("runSenCorXEGraphUnRegIn1",  Form("In Sen X vs E"),"lp");
  legRunSenCorXEUnReg1->AddEntry("runSenCorXEGraphUnRegOut1",  Form("Out Sen X vs E"),"lp");
  legRunSenCorXEUnReg1->SetTextSize(0.055);
  legRunSenCorXEUnReg1->SetFillColor(0);
  legRunSenCorXEUnReg1->SetBorderSize(2);
  legRunSenCorXEUnReg1->Draw();

  gPad->Update();


  pad91->cd(4);

  TGraphErrors * runSenCorYEGraphUnRegIn1 = new TGraphErrors(counterRegIn,senYRegInVal,senERegInVal,esenYRegInVal,esenERegInVal);
  runSenCorYEGraphUnRegIn1->SetName("runSenCorYEGraphUnRegIn1");
  runSenCorYEGraphUnRegIn1->SetMarkerColor(kBlue);
  runSenCorYEGraphUnRegIn1->SetLineColor(kBlue);
  runSenCorYEGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorYEGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorYEGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorYEGraphUnRegOut1 = new TGraphErrors(counterRegOut,senYRegOutVal,senERegOutVal,esenYRegOutVal,esenERegOutVal);
  runSenCorYEGraphUnRegOut1->SetName("runSenCorYEGraphUnRegOut1");
  runSenCorYEGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorYEGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorYEGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorYEGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorYEGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorYEGraphUnReg1 = new TMultiGraph();
  runSenCorYEGraphUnReg1->Add(runSenCorYEGraphUnRegIn1);
  runSenCorYEGraphUnReg1->Add(runSenCorYEGraphUnRegOut1);
  runSenCorYEGraphUnReg1->Draw("AP");
  runSenCorYEGraphUnReg1->SetTitle("");
  runSenCorYEGraphUnReg1->GetXaxis()->SetTitle("Y Sensitivity [ppm/mm]");
  runSenCorYEGraphUnReg1->GetYaxis()->SetTitle("E Sensitivity [dim. less]");
  runSenCorYEGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorYEGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorYEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorYEUnReg1->AddEntry("runSenCorYEGraphUnRegIn1",  Form("In Sen Y vs E"),"lp");
  legRunSenCorYEUnReg1->AddEntry("runSenCorYEGraphUnRegOut1",  Form("Out Sen Y vs E"),"lp");
  legRunSenCorYEUnReg1->SetTextSize(0.055);
  legRunSenCorYEUnReg1->SetFillColor(0);
  legRunSenCorYEUnReg1->SetBorderSize(2);
  legRunSenCorYEUnReg1->Draw();

  gPad->Update();


  pad91->cd(5);

  TGraphErrors * runSenCorXYGraphUnRegIn1 = new TGraphErrors(counterRegIn,senXRegInVal,senYRegInVal,esenXRegInVal,esenYRegInVal);
  runSenCorXYGraphUnRegIn1->SetName("runSenCorXYGraphUnRegIn1");
  runSenCorXYGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorXYGraphUnRegIn1->SetLineColor(kRed);
  runSenCorXYGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorXYGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorXYGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorXYGraphUnRegOut1 = new TGraphErrors(counterRegOut,senXRegOutVal,senYRegOutVal,esenXRegOutVal,esenYRegOutVal);
  runSenCorXYGraphUnRegOut1->SetName("runSenCorXYGraphUnRegOut1");
  runSenCorXYGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorXYGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorXYGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorXYGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorXYGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorXYGraphUnReg1 = new TMultiGraph();
  runSenCorXYGraphUnReg1->Add(runSenCorXYGraphUnRegIn1);
  runSenCorXYGraphUnReg1->Add(runSenCorXYGraphUnRegOut1);
  runSenCorXYGraphUnReg1->Draw("AP");
  runSenCorXYGraphUnReg1->SetTitle("");
  runSenCorXYGraphUnReg1->GetXaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenCorXYGraphUnReg1->GetYaxis()->SetTitle("Y Sensitivity [ppm/mm]");
  runSenCorXYGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorXYGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorXYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorXYUnReg1->AddEntry("runSenCorXYGraphUnRegIn1",  Form("In Sen X vs Y"),"lp");
  legRunSenCorXYUnReg1->AddEntry("runSenCorXYGraphUnRegOut1",  Form("Out Sen X vs Y"),"lp");
  legRunSenCorXYUnReg1->SetTextSize(0.055);
  legRunSenCorXYUnReg1->SetFillColor(0);
  legRunSenCorXYUnReg1->SetBorderSize(2);
  legRunSenCorXYUnReg1->Draw();

  gPad->Update();

  pad91->cd(6);

  TGraphErrors * runSenCorXSlopeYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,senXSlopeRegInVal,senYSlopeRegInVal,esenXSlopeRegInVal,esenYSlopeRegInVal);
  runSenCorXSlopeYSlopeGraphUnRegIn1->SetName("runSenCorXSlopeYSlopeGraphUnRegIn1");
  runSenCorXSlopeYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorXSlopeYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorXSlopeYSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorXSlopeYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorXSlopeYSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorXSlopeYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,senXSlopeRegOutVal,senYSlopeRegOutVal,esenXSlopeRegOutVal,esenYSlopeRegOutVal);
  runSenCorXSlopeYSlopeGraphUnRegOut1->SetName("runSenCorXSlopeYSlopeGraphUnRegOut1");
  runSenCorXSlopeYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorXSlopeYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorXSlopeYSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorXSlopeYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorXSlopeYSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorXSlopeYSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorXSlopeYSlopeGraphUnReg1->Add(runSenCorXSlopeYSlopeGraphUnRegIn1);
  runSenCorXSlopeYSlopeGraphUnReg1->Add(runSenCorXSlopeYSlopeGraphUnRegOut1);
  runSenCorXSlopeYSlopeGraphUnReg1->Draw("AP");
  runSenCorXSlopeYSlopeGraphUnReg1->SetTitle("");
  runSenCorXSlopeYSlopeGraphUnReg1->GetXaxis()->SetTitle("X Slope Sensitivity [ppm/#murad]");
  runSenCorXSlopeYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Sensitivity [ppm/#murad]");
  runSenCorXSlopeYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorXSlopeYSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorXSlopeYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorXSlopeYSlopeUnReg1->AddEntry("runSenCorXSlopeYSlopeGraphUnRegIn1",  Form("In Sen XSlope vs YSlope"),"lp");
  legRunSenCorXSlopeYSlopeUnReg1->AddEntry("runSenCorXSlopeYSlopeGraphUnRegOut1",  Form("Out Sen XSlope vs YSlope"),"lp");
  legRunSenCorXSlopeYSlopeUnReg1->SetTextSize(0.055);
  legRunSenCorXSlopeYSlopeUnReg1->SetFillColor(0);
  legRunSenCorXSlopeYSlopeUnReg1->SetBorderSize(2);
  legRunSenCorXSlopeYSlopeUnReg1->Draw();

  gPad->Update();

  pad91->cd(7);

  TGraphErrors * runSenCorXYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,senXRegInVal,senYSlopeRegInVal,esenXRegInVal,esenYSlopeRegInVal);
  runSenCorXYSlopeGraphUnRegIn1->SetName("runSenCorXYSlopeGraphUnRegIn1");
  runSenCorXYSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runSenCorXYSlopeGraphUnRegIn1->SetLineColor(kRed);
  runSenCorXYSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorXYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorXYSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorXYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,senXRegOutVal,senYSlopeRegOutVal,esenXRegOutVal,esenYSlopeRegOutVal);
  runSenCorXYSlopeGraphUnRegOut1->SetName("runSenCorXYSlopeGraphUnRegOut1");
  runSenCorXYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorXYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorXYSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorXYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorXYSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorXYSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorXYSlopeGraphUnReg1->Add(runSenCorXYSlopeGraphUnRegIn1);
  runSenCorXYSlopeGraphUnReg1->Add(runSenCorXYSlopeGraphUnRegOut1);
  runSenCorXYSlopeGraphUnReg1->Draw("AP");
  runSenCorXYSlopeGraphUnReg1->SetTitle("");
  runSenCorXYSlopeGraphUnReg1->GetXaxis()->SetTitle("X Sensitivity [ppm/mm]");
  runSenCorXYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Sensitivity [ppm/#murad]");
  runSenCorXYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorXYSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorXYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorXYSlopeUnReg1->AddEntry("runSenCorXYSlopeGraphUnRegIn1",  Form("In Sen X vs YSlope"),"lp");
  legRunSenCorXYSlopeUnReg1->AddEntry("runSenCorXYSlopeGraphUnRegOut1",  Form("Out Sen X vs YSlope"),"lp");
  legRunSenCorXYSlopeUnReg1->SetTextSize(0.055);
  legRunSenCorXYSlopeUnReg1->SetFillColor(0);
  legRunSenCorXYSlopeUnReg1->SetBorderSize(2);
  legRunSenCorXYSlopeUnReg1->Draw();

  gPad->Update();

  pad91->cd(8);

  TGraphErrors * runSenCorYXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,senYRegInVal,senXSlopeRegInVal,esenYRegInVal,esenXSlopeRegInVal);
  runSenCorYXSlopeGraphUnRegIn1->SetName("runSenCorYXSlopeGraphUnRegIn1");
  runSenCorYXSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runSenCorYXSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runSenCorYXSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runSenCorYXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runSenCorYXSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runSenCorYXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,senYRegOutVal,senXSlopeRegOutVal,esenYRegOutVal,esenXSlopeRegOutVal);
  runSenCorYXSlopeGraphUnRegOut1->SetName("runSenCorYXSlopeGraphUnRegOut1");
  runSenCorYXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runSenCorYXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runSenCorYXSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runSenCorYXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runSenCorYXSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runSenCorYXSlopeGraphUnReg1 = new TMultiGraph();
  runSenCorYXSlopeGraphUnReg1->Add(runSenCorYXSlopeGraphUnRegIn1);
  runSenCorYXSlopeGraphUnReg1->Add(runSenCorYXSlopeGraphUnRegOut1);
  runSenCorYXSlopeGraphUnReg1->Draw("AP");
  runSenCorYXSlopeGraphUnReg1->SetTitle("");
  runSenCorYXSlopeGraphUnReg1->GetXaxis()->SetTitle("Y Sensitivity [ppm/mm]");
  runSenCorYXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Sensitivity [ppm/#murad]");
  runSenCorYXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runSenCorYXSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunSenCorYXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunSenCorYXSlopeUnReg1->AddEntry("runSenCorYXSlopeGraphUnRegIn1",  Form("In Sen Y vs XSlope"),"lp");
  legRunSenCorYXSlopeUnReg1->AddEntry("runSenCorYXSlopeGraphUnRegOut1",  Form("Out Sen Y vs XSlope"),"lp");
  legRunSenCorYXSlopeUnReg1->SetTextSize(0.055);
  legRunSenCorYXSlopeUnReg1->SetFillColor(0);
  legRunSenCorYXSlopeUnReg1->SetBorderSize(2);
  legRunSenCorYXSlopeUnReg1->Draw();

  gPad->Update();


  c9->Update();
  c9->SaveAs(Form("%s/plots/%s/%sSensitivityCorrelation%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/

  const Int_t NUM = 500;
  int counter_cor=0;
  ifstream inMdSenCorXXSlope,outMdSenCorXXSlope,inMdSenCorYYSlope,outMdSenCorYYSlope,inMdSenCorXE,outMdSenCorXE,inAllMdSen,outAllMdSen;
  inAllMdSen.open((Form("%s/dataBaseAnalysis/testAllMdSenCorIn.txt",n2deltaAnalysisDir.Data())));
  outAllMdSen.open((Form("%s/dataBaseAnalysis/testAllMdSenCorOut.txt",n2deltaAnalysisDir.Data())));

  Double_t zero[NUM],run_number_cor_in[NUM],X_cor_in[NUM],eX_cor_in[NUM],XSlope_cor_in[NUM],eXSlope_cor_in[NUM];
  Double_t run_number_cor_out[NUM],X_cor_out[NUM],eX_cor_out[NUM],XSlope_cor_out[NUM],eXSlope_cor_out[NUM];
  Double_t Y_cor_in[NUM],eY_cor_in[NUM],YSlope_cor_in[NUM],eYSlope_cor_in[NUM];
  Double_t Y_cor_out[NUM],eY_cor_out[NUM],YSlope_cor_out[NUM],eYSlope_cor_out[NUM];
  Double_t E_cor_in[NUM],eE_cor_in[NUM];
  Double_t E_cor_out[NUM],eE_cor_out[NUM];
  Double_t Total_cor_in[NUM],eTotal_cor_in[NUM];
  Double_t Total_cor_out[NUM],eTotal_cor_out[NUM];

  while(1) {
    zero[counter_cor]=0;


    inAllMdSen >> run_number_cor_in[counter_cor];
    inAllMdSen >> X_cor_in[counter_cor];
    inAllMdSen >> eX_cor_in[counter_cor];
    inAllMdSen >> XSlope_cor_in[counter_cor];
    inAllMdSen >> eXSlope_cor_in[counter_cor];
    inAllMdSen >> Y_cor_in[counter_cor];
    inAllMdSen >> eY_cor_in[counter_cor];
    inAllMdSen >> YSlope_cor_in[counter_cor];
    inAllMdSen >> eYSlope_cor_in[counter_cor];
    inAllMdSen >> E_cor_in[counter_cor];
    inAllMdSen >> eE_cor_in[counter_cor];
    inAllMdSen >> Total_cor_in[counter_cor];
    inAllMdSen >> eTotal_cor_in[counter_cor];
 
    if (!inAllMdSen.good()) break;

    outAllMdSen >> run_number_cor_out[counter_cor];
    outAllMdSen >> X_cor_out[counter_cor];
    outAllMdSen >> eX_cor_out[counter_cor];
    outAllMdSen >> XSlope_cor_out[counter_cor];
    outAllMdSen >> eXSlope_cor_out[counter_cor];
    outAllMdSen >> Y_cor_out[counter_cor];
    outAllMdSen >> eY_cor_out[counter_cor];
    outAllMdSen >> YSlope_cor_out[counter_cor];
    outAllMdSen >> eYSlope_cor_out[counter_cor];
    outAllMdSen >> E_cor_out[counter_cor];
    outAllMdSen >> eE_cor_out[counter_cor];
    outAllMdSen >> Total_cor_out[counter_cor];
    outAllMdSen >> eTotal_cor_out[counter_cor];

   if (!outAllMdSen.good()) break;



//     std::cout << run_number_cor_in[counter_cor] << X_cor_in[counter_cor] << " X In\t" << eX_cor_in[counter_cor] << XSlope_cor_in[counter_cor] << eXSlope_cor_in[counter_cor] << std::endl;
//     std::cout << run_number_cor_out[counter_cor] << X_cor_out[counter_cor] << "X Out\t" << eX_cor_out[counter_cor] << XSlope_cor_out[counter_cor] << eXSlope_cor_out[counter_cor] << std::endl;
//     std::cout << run_number_cor_in[counter_cor] << Y_cor_in[counter_cor] << " Y In\t" << eY_cor_in[counter_cor] << YSlope_cor_in[counter_cor] << eYSlope_cor_in[counter_cor] << std::endl;
//     std::cout << run_number_cor_out[counter_cor] << Y_cor_out[counter_cor] << "Y Out\t" << eY_cor_out[counter_cor] << YSlope_cor_out[counter_cor] << eYSlope_cor_out[counter_cor] << std::endl;


    counter_cor++;
  }

  inAllMdSen.close();
  outAllMdSen.close();

  TCanvas *c12 = new TCanvas("c12","c12",cx1,cy1,1200,950);
  TPad * pad120 = new TPad("pad120","pad120",ps1,ps2,ps4,ps4);
  TPad * pad121 = new TPad("pad121","pad121",ps1,ps1,ps4,ps3);
  pad120->Draw();
  pad121->Draw();
  pad120->cd();
  TText * ct120 = new TText(tll,tlr,Form("%sN to Delta %s %s Runlet Based Correction Correlations",regTitle,dataInfo,plotTitle));
  ct120->SetTextSize(tsiz);
  ct120->Draw();
  pad121->cd();
  pad121->Divide(2,4);


  pad121->cd(1);

  TGraphErrors * runMdSenCorXXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletXRegInVal,corRunletXSlopeRegInVal,ecorRunletXRegInVal,ecorRunletXSlopeRegInVal);
  runMdSenCorXXSlopeGraphUnRegIn1->SetName("runMdSenCorXXSlopeGraphUnRegIn1");
  runMdSenCorXXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runMdSenCorXXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runMdSenCorXXSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorXXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorXXSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorXXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletXRegOutVal,corRunletXSlopeRegOutVal,ecorRunletXRegOutVal,ecorRunletXSlopeRegOutVal);
  runMdSenCorXXSlopeGraphUnRegOut1->SetName("runMdSenCorXXSlopeGraphUnRegOut1");
  runMdSenCorXXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorXXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorXXSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorXXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorXXSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorXXSlopeGraphUnReg1 = new TMultiGraph();
  runMdSenCorXXSlopeGraphUnReg1->Add(runMdSenCorXXSlopeGraphUnRegIn1);
  runMdSenCorXXSlopeGraphUnReg1->Add(runMdSenCorXXSlopeGraphUnRegOut1);
  runMdSenCorXXSlopeGraphUnReg1->Draw("AP");
  runMdSenCorXXSlopeGraphUnReg1->SetTitle("");
  runMdSenCorXXSlopeGraphUnReg1->GetXaxis()->SetTitle("X Correction [ppm]");
  runMdSenCorXXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Correction [ppm]");
  runMdSenCorXXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorXXSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorXXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorXXSlopeUnReg1->AddEntry("runMdSenCorXXSlopeGraphUnRegIn1",  Form("In Cor X vs XSlope"),"lp");
  legRunMdSenCorXXSlopeUnReg1->AddEntry("runMdSenCorXXSlopeGraphUnRegOut1",  Form("Out Cor X vs XSlope"),"lp");
  legRunMdSenCorXXSlopeUnReg1->SetTextSize(0.055);
  legRunMdSenCorXXSlopeUnReg1->SetFillColor(0);
  legRunMdSenCorXXSlopeUnReg1->SetBorderSize(2);
  legRunMdSenCorXXSlopeUnReg1->Draw();

  gPad->Update();


  pad121->cd(2);

  TGraphErrors * runMdSenCorYYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletYRegInVal,corRunletYSlopeRegInVal,ecorRunletYRegInVal,ecorRunletYSlopeRegInVal);
  runMdSenCorYYSlopeGraphUnRegIn1->SetName("runMdSenCorYYSlopeGraphUnRegIn1");
  runMdSenCorYYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runMdSenCorYYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runMdSenCorYYSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorYYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorYYSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorYYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletYRegOutVal,corRunletYSlopeRegOutVal,ecorRunletYRegOutVal,ecorRunletYSlopeRegOutVal);
  runMdSenCorYYSlopeGraphUnRegOut1->SetName("runMdSenCorYYSlopeGraphUnRegOut1");
  runMdSenCorYYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorYYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorYYSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorYYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorYYSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorYYSlopeGraphUnReg1 = new TMultiGraph();
  runMdSenCorYYSlopeGraphUnReg1->Add(runMdSenCorYYSlopeGraphUnRegIn1);
  runMdSenCorYYSlopeGraphUnReg1->Add(runMdSenCorYYSlopeGraphUnRegOut1);
  runMdSenCorYYSlopeGraphUnReg1->Draw("AP");
  runMdSenCorYYSlopeGraphUnReg1->SetTitle("");
  runMdSenCorYYSlopeGraphUnReg1->GetXaxis()->SetTitle("Y Correction [ppm]");
  runMdSenCorYYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  runMdSenCorYYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorYYSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorYYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorYYSlopeUnReg1->AddEntry("runMdSenCorYYSlopeGraphUnRegIn1",  Form("In Cor Y vs YSlope"),"lp");
  legRunMdSenCorYYSlopeUnReg1->AddEntry("runMdSenCorYYSlopeGraphUnRegOut1",  Form("Out Cor Y vs YSlope"),"lp");
  legRunMdSenCorYYSlopeUnReg1->SetTextSize(0.055);
  legRunMdSenCorYYSlopeUnReg1->SetFillColor(0);
  legRunMdSenCorYYSlopeUnReg1->SetBorderSize(2);
  legRunMdSenCorYYSlopeUnReg1->Draw();

  gPad->Update();

  pad121->cd(3);

  TGraphErrors * runMdSenCorXEGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletXRegInVal,corRunletERegInVal,ecorRunletXRegInVal,ecorRunletERegInVal);
  runMdSenCorXEGraphUnRegIn1->SetName("runMdSenCorXEGraphUnRegIn1");
  runMdSenCorXEGraphUnRegIn1->SetMarkerColor(kBlue);
  runMdSenCorXEGraphUnRegIn1->SetLineColor(kBlue);
  runMdSenCorXEGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorXEGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorXEGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorXEGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletXRegOutVal,corRunletERegOutVal,ecorRunletXRegOutVal,ecorRunletERegOutVal);
  runMdSenCorXEGraphUnRegOut1->SetName("runMdSenCorXEGraphUnRegOut1");
  runMdSenCorXEGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorXEGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorXEGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorXEGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorXEGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorXEGraphUnReg1 = new TMultiGraph();
  runMdSenCorXEGraphUnReg1->Add(runMdSenCorXEGraphUnRegIn1);
  runMdSenCorXEGraphUnReg1->Add(runMdSenCorXEGraphUnRegOut1);
  runMdSenCorXEGraphUnReg1->Draw("AP");
  runMdSenCorXEGraphUnReg1->SetTitle("");
  runMdSenCorXEGraphUnReg1->GetXaxis()->SetTitle("X Correction [ppm]");
  runMdSenCorXEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  runMdSenCorXEGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorXEGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorXEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorXEUnReg1->AddEntry("runMdSenCorXEGraphUnRegIn1",  Form("In Cor X vs E"),"lp");
  legRunMdSenCorXEUnReg1->AddEntry("runMdSenCorXEGraphUnRegOut1",  Form("Out Cor X vs E"),"lp");
  legRunMdSenCorXEUnReg1->SetTextSize(0.055);
  legRunMdSenCorXEUnReg1->SetFillColor(0);
  legRunMdSenCorXEUnReg1->SetBorderSize(2);
  legRunMdSenCorXEUnReg1->Draw();

  gPad->Update();


  pad121->cd(4);

  TGraphErrors * runMdSenCorYEGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletYRegInVal,corRunletERegInVal,ecorRunletYRegInVal,ecorRunletERegInVal);
  runMdSenCorYEGraphUnRegIn1->SetName("runMdSenCorYEGraphUnRegIn1");
  runMdSenCorYEGraphUnRegIn1->SetMarkerColor(kBlue);
  runMdSenCorYEGraphUnRegIn1->SetLineColor(kBlue);
  runMdSenCorYEGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorYEGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorYEGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorYEGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletYRegOutVal,corRunletERegOutVal,ecorRunletYRegOutVal,ecorRunletERegOutVal);
  runMdSenCorYEGraphUnRegOut1->SetName("runMdSenCorYEGraphUnRegOut1");
  runMdSenCorYEGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorYEGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorYEGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorYEGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorYEGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorYEGraphUnReg1 = new TMultiGraph();
  runMdSenCorYEGraphUnReg1->Add(runMdSenCorYEGraphUnRegIn1);
  runMdSenCorYEGraphUnReg1->Add(runMdSenCorYEGraphUnRegOut1);
  runMdSenCorYEGraphUnReg1->Draw("AP");
  runMdSenCorYEGraphUnReg1->SetTitle("");
  runMdSenCorYEGraphUnReg1->GetXaxis()->SetTitle("Y Correction [ppm]");
  runMdSenCorYEGraphUnReg1->GetYaxis()->SetTitle("E Correction [ppm]");
  runMdSenCorYEGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorYEGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorYEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorYEUnReg1->AddEntry("runMdSenCorYEGraphUnRegIn1",  Form("In Cor Y vs E"),"lp");
  legRunMdSenCorYEUnReg1->AddEntry("runMdSenCorYEGraphUnRegOut1",  Form("Out Cor Y vs E"),"lp");
  legRunMdSenCorYEUnReg1->SetTextSize(0.055);
  legRunMdSenCorYEUnReg1->SetFillColor(0);
  legRunMdSenCorYEUnReg1->SetBorderSize(2);
  legRunMdSenCorYEUnReg1->Draw();

  gPad->Update();


  pad121->cd(5);

  TGraphErrors * runMdSenCorXYGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletXRegInVal,corRunletYRegInVal,ecorRunletXRegInVal,ecorRunletYRegInVal);
  runMdSenCorXYGraphUnRegIn1->SetName("runMdSenCorXYGraphUnRegIn1");
  runMdSenCorXYGraphUnRegIn1->SetMarkerColor(kRed);
  runMdSenCorXYGraphUnRegIn1->SetLineColor(kRed);
  runMdSenCorXYGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorXYGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorXYGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorXYGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletXRegOutVal,corRunletYRegOutVal,ecorRunletXRegOutVal,ecorRunletYRegOutVal);
  runMdSenCorXYGraphUnRegOut1->SetName("runMdSenCorXYGraphUnRegOut1");
  runMdSenCorXYGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorXYGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorXYGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorXYGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorXYGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorXYGraphUnReg1 = new TMultiGraph();
  runMdSenCorXYGraphUnReg1->Add(runMdSenCorXYGraphUnRegIn1);
  runMdSenCorXYGraphUnReg1->Add(runMdSenCorXYGraphUnRegOut1);
  runMdSenCorXYGraphUnReg1->Draw("AP");
  runMdSenCorXYGraphUnReg1->SetTitle("");
  runMdSenCorXYGraphUnReg1->GetXaxis()->SetTitle("X Correction [ppm]");
  runMdSenCorXYGraphUnReg1->GetYaxis()->SetTitle("Y Correction [ppm]");
  runMdSenCorXYGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorXYGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorXYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorXYUnReg1->AddEntry("runMdSenCorXYGraphUnRegIn1",  Form("In Cor X vs Y"),"lp");
  legRunMdSenCorXYUnReg1->AddEntry("runMdSenCorXYGraphUnRegOut1",  Form("Out Cor X vs Y"),"lp");
  legRunMdSenCorXYUnReg1->SetTextSize(0.055);
  legRunMdSenCorXYUnReg1->SetFillColor(0);
  legRunMdSenCorXYUnReg1->SetBorderSize(2);
  legRunMdSenCorXYUnReg1->Draw();

  gPad->Update();

  pad121->cd(6);

  TGraphErrors * runMdSenCorXSlopeYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletXSlopeRegInVal,corRunletYSlopeRegInVal,ecorRunletXSlopeRegInVal,ecorRunletYSlopeRegInVal);
  runMdSenCorXSlopeYSlopeGraphUnRegIn1->SetName("runMdSenCorXSlopeYSlopeGraphUnRegIn1");
  runMdSenCorXSlopeYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runMdSenCorXSlopeYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runMdSenCorXSlopeYSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorXSlopeYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorXSlopeYSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorXSlopeYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletXSlopeRegOutVal,corRunletYSlopeRegOutVal,ecorRunletXSlopeRegOutVal,ecorRunletYSlopeRegOutVal);
  runMdSenCorXSlopeYSlopeGraphUnRegOut1->SetName("runMdSenCorXSlopeYSlopeGraphUnRegOut1");
  runMdSenCorXSlopeYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorXSlopeYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorXSlopeYSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorXSlopeYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorXSlopeYSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorXSlopeYSlopeGraphUnReg1 = new TMultiGraph();
  runMdSenCorXSlopeYSlopeGraphUnReg1->Add(runMdSenCorXSlopeYSlopeGraphUnRegIn1);
  runMdSenCorXSlopeYSlopeGraphUnReg1->Add(runMdSenCorXSlopeYSlopeGraphUnRegOut1);
  runMdSenCorXSlopeYSlopeGraphUnReg1->Draw("AP");
  runMdSenCorXSlopeYSlopeGraphUnReg1->SetTitle("");
  runMdSenCorXSlopeYSlopeGraphUnReg1->GetXaxis()->SetTitle("X Slope Correction [ppm]");
  runMdSenCorXSlopeYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  runMdSenCorXSlopeYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorXSlopeYSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorXSlopeYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorXSlopeYSlopeUnReg1->AddEntry("runMdSenCorXSlopeYSlopeGraphUnRegIn1",  Form("In Cor XSlope vs YSlope"),"lp");
  legRunMdSenCorXSlopeYSlopeUnReg1->AddEntry("runMdSenCorXSlopeYSlopeGraphUnRegOut1",  Form("Out Cor XSlope vs YSlope"),"lp");
  legRunMdSenCorXSlopeYSlopeUnReg1->SetTextSize(0.055);
  legRunMdSenCorXSlopeYSlopeUnReg1->SetFillColor(0);
  legRunMdSenCorXSlopeYSlopeUnReg1->SetBorderSize(2);
  legRunMdSenCorXSlopeYSlopeUnReg1->Draw();

  gPad->Update();

  pad121->cd(7);

  TGraphErrors * runMdSenCorXYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletXRegInVal,corRunletYSlopeRegInVal,ecorRunletXRegInVal,ecorRunletYSlopeRegInVal);
  runMdSenCorXYSlopeGraphUnRegIn1->SetName("runMdSenCorXYSlopeGraphUnRegIn1");
  runMdSenCorXYSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runMdSenCorXYSlopeGraphUnRegIn1->SetLineColor(kRed);
  runMdSenCorXYSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorXYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorXYSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorXYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletXRegOutVal,corRunletYSlopeRegOutVal,ecorRunletXRegOutVal,ecorRunletYSlopeRegOutVal);
  runMdSenCorXYSlopeGraphUnRegOut1->SetName("runMdSenCorXYSlopeGraphUnRegOut1");
  runMdSenCorXYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorXYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorXYSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorXYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorXYSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorXYSlopeGraphUnReg1 = new TMultiGraph();
  runMdSenCorXYSlopeGraphUnReg1->Add(runMdSenCorXYSlopeGraphUnRegIn1);
  runMdSenCorXYSlopeGraphUnReg1->Add(runMdSenCorXYSlopeGraphUnRegOut1);
  runMdSenCorXYSlopeGraphUnReg1->Draw("AP");
  runMdSenCorXYSlopeGraphUnReg1->SetTitle("");
  runMdSenCorXYSlopeGraphUnReg1->GetXaxis()->SetTitle("X Correction [ppm]");
  runMdSenCorXYSlopeGraphUnReg1->GetYaxis()->SetTitle("Y Slope Correction [ppm]");
  runMdSenCorXYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorXYSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorXYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorXYSlopeUnReg1->AddEntry("runMdSenCorXYSlopeGraphUnRegIn1",  Form("In Cor X vs YSlope"),"lp");
  legRunMdSenCorXYSlopeUnReg1->AddEntry("runMdSenCorXYSlopeGraphUnRegOut1",  Form("Out Cor X vs YSlope"),"lp");
  legRunMdSenCorXYSlopeUnReg1->SetTextSize(0.055);
  legRunMdSenCorXYSlopeUnReg1->SetFillColor(0);
  legRunMdSenCorXYSlopeUnReg1->SetBorderSize(2);
  legRunMdSenCorXYSlopeUnReg1->Draw();

  gPad->Update();

  pad121->cd(8);

  TGraphErrors * runMdSenCorYXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,corRunletYRegInVal,corRunletXSlopeRegInVal,ecorRunletYRegInVal,ecorRunletXSlopeRegInVal);
  runMdSenCorYXSlopeGraphUnRegIn1->SetName("runMdSenCorYXSlopeGraphUnRegIn1");
  runMdSenCorYXSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runMdSenCorYXSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runMdSenCorYXSlopeGraphUnRegIn1->SetMarkerStyle(21);
  runMdSenCorYXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runMdSenCorYXSlopeGraphUnRegIn1->SetLineWidth(1);

  TGraphErrors * runMdSenCorYXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,corRunletYRegOutVal,corRunletXSlopeRegOutVal,ecorRunletYRegOutVal,ecorRunletXSlopeRegOutVal);
  runMdSenCorYXSlopeGraphUnRegOut1->SetName("runMdSenCorYXSlopeGraphUnRegOut1");
  runMdSenCorYXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runMdSenCorYXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runMdSenCorYXSlopeGraphUnRegOut1->SetMarkerStyle(21);
  runMdSenCorYXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runMdSenCorYXSlopeGraphUnRegOut1->SetLineWidth(1);

  TMultiGraph *runMdSenCorYXSlopeGraphUnReg1 = new TMultiGraph();
  runMdSenCorYXSlopeGraphUnReg1->Add(runMdSenCorYXSlopeGraphUnRegIn1);
  runMdSenCorYXSlopeGraphUnReg1->Add(runMdSenCorYXSlopeGraphUnRegOut1);
  runMdSenCorYXSlopeGraphUnReg1->Draw("AP");
  runMdSenCorYXSlopeGraphUnReg1->SetTitle("");
  runMdSenCorYXSlopeGraphUnReg1->GetXaxis()->SetTitle("Y Correction [ppm]");
  runMdSenCorYXSlopeGraphUnReg1->GetYaxis()->SetTitle("X Slope Correction [ppm]");
  runMdSenCorYXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runMdSenCorYXSlopeGraphUnReg1->GetYaxis()->CenterTitle();

  TLegend *legRunMdSenCorYXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunMdSenCorYXSlopeUnReg1->AddEntry("runMdSenCorYXSlopeGraphUnRegIn1",  Form("In Cor Y vs XSlope"),"lp");
  legRunMdSenCorYXSlopeUnReg1->AddEntry("runMdSenCorYXSlopeGraphUnRegOut1",  Form("Out Cor Y vs XSlope"),"lp");
  legRunMdSenCorYXSlopeUnReg1->SetTextSize(0.055);
  legRunMdSenCorYXSlopeUnReg1->SetFillColor(0);
  legRunMdSenCorYXSlopeUnReg1->SetBorderSize(2);
  legRunMdSenCorYXSlopeUnReg1->Draw();

  gPad->Update();


  c12->Update();
  c12->SaveAs(Form("%s/plots/%s/%sCorrectionCorrelationRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  /*********************************************************************************/

  }


  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(PROFILE_PLOT){

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat("eMr");
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.30);
  gStyle->SetStatH(0.20);
  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatColor(0);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

//   gDirectory->Delete("*");


  if(RUNLET_CORRELATION_PLOT2){


  TCanvas *c12 = new TCanvas("c12","c12",cx1,cy1,1200,950);
  TPad * pad120 = new TPad("pad120","pad120",ps1,ps2,ps4,ps4);
  TPad * pad121 = new TPad("pad121","pad121",ps1,ps1,ps4,ps3);
  pad120->Draw();
  pad121->Draw();
  pad120->cd();
  TText * ct120 = new TText(tll,tlr,Form("%sN to Delta %s %s Runlet Based Correction Correlations",regTitle,dataInfo,plotTitle));
  ct120->SetTextSize(tsiz);
  ct120->Draw();
  pad121->cd();
  pad121->Divide(2,4);


  pad121->cd(1);


  checkXXp->Draw();
  checkXXp->SetMarkerStyle(1);
  checkXXp->SetMarkerColor(kRed);
  checkXXp->SetLineColor(kRed);
  checkXXp->Fit("pol1");

  gPad->Update();

  }




  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(RUNLET_DIFF_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TCanvas *c6 = new TCanvas("c6","c6",cx1,cy1,1200,950);
  TPad * pad60 = new TPad("pad60","pad60",ps1,ps2,ps4,ps4);
  TPad * pad61 = new TPad("pad61","pad61",ps1,ps1,ps4,ps3);
  pad60->Draw();
  pad61->Draw();
  pad60->cd();
  TText * ct60 = new TText(tll,tlr,Form("%sN to Delta %s Un-Regressed Position and Energy Differences",regTitle,dataInfo));
  ct60->SetTextSize(tsiz);
  ct60->Draw();
  pad61->cd();
  pad61->Divide(2,3);


  pad61->cd(1);

  TGraphErrors * runDiffXGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffXCalcUnRegInVal,zeroUnRegIn,ediffXCalcUnRegInVal);
//   TGraphErrors * runDiffXGraphUnRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testDiffXUnRegIn.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
  runDiffXGraphUnRegIn1->SetName("runDiffXGraphUnRegIn1");
  runDiffXGraphUnRegIn1->SetMarkerColor(kRed);
  runDiffXGraphUnRegIn1->SetLineColor(kRed);
  runDiffXGraphUnRegIn1->SetMarkerStyle(25);
  runDiffXGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffXGraphUnRegIn1->SetLineWidth(1);
//   runDiffXGraphUnRegIn1->Draw("AP");
  runDiffXGraphUnRegIn1->Fit("fitRunDiffXGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffXGraphUnRegIn1->SetLineColor(kRed);
  fitRunDiffXGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXUnRegIn1            =  fitRunDiffXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXUnRegIn1Error       =  fitRunDiffXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffXUnRegIn1Chisquare   =  fitRunDiffXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffXUnRegIn1NDF         =  fitRunDiffXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffXUnRegIn1Prob        =  fitRunDiffXGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffXGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffXCalcUnRegOutVal,zeroUnRegOut,ediffXCalcUnRegOutVal);
//   TGraphErrors * runDiffXGraphUnRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testDiffXUnRegOut.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
  runDiffXGraphUnRegOut1->SetName("runDiffXGraphUnRegOut1");
  runDiffXGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffXGraphUnRegOut1->SetLineColor(kBlack);
  runDiffXGraphUnRegOut1->SetMarkerStyle(25);
  runDiffXGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffXGraphUnRegOut1->SetLineWidth(1);
//   runDiffXGraphUnRegOut1->Draw("AP");
  runDiffXGraphUnRegOut1->Fit("fitRunDiffXGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffXGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXUnRegOut1           =  fitRunDiffXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXUnRegOut1Error      =  fitRunDiffXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffXUnRegOut1Chisquare  =  fitRunDiffXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffXUnRegOut1NDF        =  fitRunDiffXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffXUnRegOut1Prob       =  fitRunDiffXGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffXGraphUnReg1 = new TMultiGraph();
  runDiffXGraphUnReg1->Add(runDiffXGraphUnRegIn1);
  runDiffXGraphUnReg1->Add(runDiffXGraphUnRegOut1);
  runDiffXGraphUnReg1->Draw("AP");
  runDiffXGraphUnReg1->SetTitle("");
  runDiffXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXGraphUnReg1->GetYaxis()->SetTitle("Target X Diff [mm]");
  runDiffXGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffXGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXGraphUnReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffXGraphUnReg1= runDiffXGraphUnReg1->GetXaxis();
  xaxisRunDiffXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXGraphUnRegIn1->Draw("same");
  fitRunDiffXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffXUnRegIn1 =(TPaveStats*)runDiffXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXUnRegOut1=(TPaveStats*)runDiffXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXUnRegIn1->SetTextColor(kRed);
  statsRunDiffXUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffXUnRegOut1->SetTextColor(kBlack);
  statsRunDiffXUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXUnRegInpOut1       =  (fitResultRunDiffXUnRegIn1+fitResultRunDiffXUnRegOut1)/2;
  Double_t resultRunDiffXUnRegInmOut1       =  (fitResultRunDiffXUnRegIn1-fitResultRunDiffXUnRegOut1)/2;
  Double_t resultRunDiffXUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXUnRegIn1Error*fitResultRunDiffXUnRegIn1Error+fitResultRunDiffXUnRegOut1Error*fitResultRunDiffXUnRegOut1Error)/2;
  Double_t resultRunDiffXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXUnRegIn1Error*fitResultRunDiffXUnRegIn1Error+fitResultRunDiffXUnRegOut1Error*fitResultRunDiffXUnRegOut1Error)/2;


  TLegend *legRunDiffXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffXUnRegIn1,fitResultRunDiffXUnRegIn1Error),"lp");
  legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffXUnRegOut1,fitResultRunDiffXUnRegOut1Error),"lp");
//   legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXUnRegInpOut1,resultRunDiffXUnRegInpOut1Error),"");
//   legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXUnRegInmOut1,resultRunDiffXUnRegInmOut1Error),"");
  legRunDiffXUnReg1->SetTextSize(0.045);
  legRunDiffXUnReg1->SetFillColor(0);
  legRunDiffXUnReg1->SetBorderSize(2);
  legRunDiffXUnReg1->Draw();

  gPad->Update();


  pad61->cd(2);

  TGraphErrors * runDiffXSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffXSlopeCalcUnRegInVal,zeroUnRegIn,ediffXSlopeCalcUnRegInVal);
  runDiffXSlopeGraphUnRegIn1->SetName("runDiffXSlopeGraphUnRegIn1");
  runDiffXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runDiffXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runDiffXSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runDiffXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffXSlopeGraphUnRegIn1->SetLineWidth(1);
//   runDiffXSlopeGraphUnRegIn1->Draw("AP");
  runDiffXSlopeGraphUnRegIn1->Fit("fitRunDiffXSlopeGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunDiffXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeUnRegIn1            =  fitRunDiffXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeUnRegIn1Error       =  fitRunDiffXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffXSlopeUnRegIn1Chisquare   =  fitRunDiffXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffXSlopeUnRegIn1NDF         =  fitRunDiffXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffXSlopeUnRegIn1Prob        =  fitRunDiffXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffXSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffXSlopeCalcUnRegOutVal,zeroUnRegOut,ediffXSlopeCalcUnRegOutVal);
  runDiffXSlopeGraphUnRegOut1->SetName("runDiffXSlopeGraphUnRegOut1");
  runDiffXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runDiffXSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runDiffXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffXSlopeGraphUnRegOut1->SetLineWidth(1);
//   runDiffXSlopeGraphUnRegOut1->Draw("AP");
  runDiffXSlopeGraphUnRegOut1->Fit("fitRunDiffXSlopeGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeUnRegOut1           =  fitRunDiffXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeUnRegOut1Error      =  fitRunDiffXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffXSlopeUnRegOut1Chisquare  =  fitRunDiffXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffXSlopeUnRegOut1NDF        =  fitRunDiffXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffXSlopeUnRegOut1Prob       =  fitRunDiffXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffXSlopeGraphUnReg1 = new TMultiGraph();
  runDiffXSlopeGraphUnReg1->Add(runDiffXSlopeGraphUnRegIn1);
  runDiffXSlopeGraphUnReg1->Add(runDiffXSlopeGraphUnRegOut1);
  runDiffXSlopeGraphUnReg1->Draw("AP");
  runDiffXSlopeGraphUnReg1->SetTitle("");
  runDiffXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXSlopeGraphUnReg1->GetYaxis()->SetTitle("Target X Slope Diff [#murad]");
  runDiffXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffXSlopeGraphUnReg1= runDiffXSlopeGraphUnReg1->GetXaxis();
  xaxisRunDiffXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXSlopeGraphUnRegIn1->Draw("same");
  fitRunDiffXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffXSlopeUnRegIn1 =(TPaveStats*)runDiffXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXSlopeUnRegOut1=(TPaveStats*)runDiffXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunDiffXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunDiffXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXSlopeUnRegInpOut1       =  (fitResultRunDiffXSlopeUnRegIn1+fitResultRunDiffXSlopeUnRegOut1)/2;
  Double_t resultRunDiffXSlopeUnRegInmOut1       =  (fitResultRunDiffXSlopeUnRegIn1-fitResultRunDiffXSlopeUnRegOut1)/2;
  Double_t resultRunDiffXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeUnRegIn1Error*fitResultRunDiffXSlopeUnRegIn1Error+fitResultRunDiffXSlopeUnRegOut1Error*fitResultRunDiffXSlopeUnRegOut1Error)/2;
  Double_t resultRunDiffXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeUnRegIn1Error*fitResultRunDiffXSlopeUnRegIn1Error+fitResultRunDiffXSlopeUnRegOut1Error*fitResultRunDiffXSlopeUnRegOut1Error)/2;


  TLegend *legRunDiffXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeUnRegIn1,fitResultRunDiffXSlopeUnRegIn1Error),"lp");
  legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeUnRegOut1,fitResultRunDiffXSlopeUnRegOut1Error),"lp");
//   legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXSlopeUnRegInpOut1,resultRunDiffXSlopeUnRegInpOut1Error),"");
//   legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXSlopeUnRegInmOut1,resultRunDiffXSlopeUnRegInmOut1Error),"");
  legRunDiffXSlopeUnReg1->SetTextSize(0.045);
  legRunDiffXSlopeUnReg1->SetFillColor(0);
  legRunDiffXSlopeUnReg1->SetBorderSize(2);
  legRunDiffXSlopeUnReg1->Draw();

  gPad->Update();


  pad61->cd(3);

  TGraphErrors * runDiffYGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffYCalcUnRegInVal,zeroUnRegIn,ediffYCalcUnRegInVal);
  runDiffYGraphUnRegIn1->SetName("runDiffYGraphUnRegIn1");
  runDiffYGraphUnRegIn1->SetMarkerColor(kGreen);
  runDiffYGraphUnRegIn1->SetLineColor(kGreen);
  runDiffYGraphUnRegIn1->SetMarkerStyle(25);
  runDiffYGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffYGraphUnRegIn1->SetLineWidth(1);
//   runDiffYGraphUnRegIn1->Draw("AP");
  runDiffYGraphUnRegIn1->Fit("fitRunDiffYGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunDiffYGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYUnRegIn1            =  fitRunDiffYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYUnRegIn1Error       =  fitRunDiffYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffYUnRegIn1Chisquare   =  fitRunDiffYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffYUnRegIn1NDF         =  fitRunDiffYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffYUnRegIn1Prob        =  fitRunDiffYGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffYGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffYCalcUnRegOutVal,zeroUnRegOut,ediffYCalcUnRegOutVal);
  runDiffYGraphUnRegOut1->SetName("runDiffYGraphUnRegOut1");
  runDiffYGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffYGraphUnRegOut1->SetLineColor(kBlack);
  runDiffYGraphUnRegOut1->SetMarkerStyle(25);
  runDiffYGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffYGraphUnRegOut1->SetLineWidth(1);
//   runDiffYGraphUnRegOut1->Draw("AP");
  runDiffYGraphUnRegOut1->Fit("fitRunDiffYGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffYGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYUnRegOut1           =  fitRunDiffYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYUnRegOut1Error      =  fitRunDiffYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffYUnRegOut1Chisquare  =  fitRunDiffYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffYUnRegOut1NDF        =  fitRunDiffYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffYUnRegOut1Prob       =  fitRunDiffYGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffYGraphUnReg1 = new TMultiGraph();
  runDiffYGraphUnReg1->Add(runDiffYGraphUnRegIn1);
  runDiffYGraphUnReg1->Add(runDiffYGraphUnRegOut1);
  runDiffYGraphUnReg1->Draw("AP");
  runDiffYGraphUnReg1->SetTitle("");
  runDiffYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYGraphUnReg1->GetYaxis()->SetTitle("Target Y Diff [mm]");
  runDiffYGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffYGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYGraphUnReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffYGraphUnReg1= runDiffYGraphUnReg1->GetXaxis();
  xaxisRunDiffYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYGraphUnRegIn1->Draw("same");
  fitRunDiffYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffYUnRegIn1 =(TPaveStats*)runDiffYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYUnRegOut1=(TPaveStats*)runDiffYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYUnRegIn1->SetTextColor(kGreen);
  statsRunDiffYUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffYUnRegOut1->SetTextColor(kBlack);
  statsRunDiffYUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYUnRegInpOut1       =  (fitResultRunDiffYUnRegIn1+fitResultRunDiffYUnRegOut1)/2;
  Double_t resultRunDiffYUnRegInmOut1       =  (fitResultRunDiffYUnRegIn1-fitResultRunDiffYUnRegOut1)/2;
  Double_t resultRunDiffYUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYUnRegIn1Error*fitResultRunDiffYUnRegIn1Error+fitResultRunDiffYUnRegOut1Error*fitResultRunDiffYUnRegOut1Error)/2;
  Double_t resultRunDiffYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYUnRegIn1Error*fitResultRunDiffYUnRegIn1Error+fitResultRunDiffYUnRegOut1Error*fitResultRunDiffYUnRegOut1Error)/2;


  TLegend *legRunDiffYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffYUnRegIn1,fitResultRunDiffYUnRegIn1Error),"lp");
  legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffYUnRegOut1,fitResultRunDiffYUnRegOut1Error),"lp");
//   legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYUnRegInpOut1,resultRunDiffYUnRegInpOut1Error),"");
//   legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYUnRegInmOut1,resultRunDiffYUnRegInmOut1Error),"");
  legRunDiffYUnReg1->SetTextSize(0.045);
  legRunDiffYUnReg1->SetFillColor(0);
  legRunDiffYUnReg1->SetBorderSize(2);
  legRunDiffYUnReg1->Draw();

  gPad->Update();


  pad61->cd(4);

  TGraphErrors * runDiffYSlopeGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffYSlopeCalcUnRegInVal,zeroUnRegIn,ediffYSlopeCalcUnRegInVal);
  runDiffYSlopeGraphUnRegIn1->SetName("runDiffYSlopeGraphUnRegIn1");
  runDiffYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runDiffYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runDiffYSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runDiffYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffYSlopeGraphUnRegIn1->SetLineWidth(1);
//   runDiffYSlopeGraphUnRegIn1->Draw("AP");
  runDiffYSlopeGraphUnRegIn1->Fit("fitRunDiffYSlopeGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunDiffYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeUnRegIn1            =  fitRunDiffYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeUnRegIn1Error       =  fitRunDiffYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffYSlopeUnRegIn1Chisquare   =  fitRunDiffYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffYSlopeUnRegIn1NDF         =  fitRunDiffYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffYSlopeUnRegIn1Prob        =  fitRunDiffYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffYSlopeGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffYSlopeCalcUnRegOutVal,zeroUnRegOut,ediffYSlopeCalcUnRegOutVal);
  runDiffYSlopeGraphUnRegOut1->SetName("runDiffYSlopeGraphUnRegOut1");
  runDiffYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runDiffYSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runDiffYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffYSlopeGraphUnRegOut1->SetLineWidth(1);
//   runDiffYSlopeGraphUnRegOut1->Draw("AP");
  runDiffYSlopeGraphUnRegOut1->Fit("fitRunDiffYSlopeGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeUnRegOut1           =  fitRunDiffYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeUnRegOut1Error      =  fitRunDiffYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffYSlopeUnRegOut1Chisquare  =  fitRunDiffYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffYSlopeUnRegOut1NDF        =  fitRunDiffYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffYSlopeUnRegOut1Prob       =  fitRunDiffYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffYSlopeGraphUnReg1 = new TMultiGraph();
  runDiffYSlopeGraphUnReg1->Add(runDiffYSlopeGraphUnRegIn1);
  runDiffYSlopeGraphUnReg1->Add(runDiffYSlopeGraphUnRegOut1);
  runDiffYSlopeGraphUnReg1->Draw("AP");
  runDiffYSlopeGraphUnReg1->SetTitle("");
  runDiffYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYSlopeGraphUnReg1->GetYaxis()->SetTitle("Target Y Slope Diff [#murad]");
  runDiffYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffYSlopeGraphUnReg1= runDiffYSlopeGraphUnReg1->GetXaxis();
  xaxisRunDiffYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYSlopeGraphUnRegIn1->Draw("same");
  fitRunDiffYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffYSlopeUnRegIn1 =(TPaveStats*)runDiffYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYSlopeUnRegOut1=(TPaveStats*)runDiffYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunDiffYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunDiffYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYSlopeUnRegInpOut1       =  (fitResultRunDiffYSlopeUnRegIn1+fitResultRunDiffYSlopeUnRegOut1)/2;
  Double_t resultRunDiffYSlopeUnRegInmOut1       =  (fitResultRunDiffYSlopeUnRegIn1-fitResultRunDiffYSlopeUnRegOut1)/2;
  Double_t resultRunDiffYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeUnRegIn1Error*fitResultRunDiffYSlopeUnRegIn1Error+fitResultRunDiffYSlopeUnRegOut1Error*fitResultRunDiffYSlopeUnRegOut1Error)/2;
  Double_t resultRunDiffYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeUnRegIn1Error*fitResultRunDiffYSlopeUnRegIn1Error+fitResultRunDiffYSlopeUnRegOut1Error*fitResultRunDiffYSlopeUnRegOut1Error)/2;


  TLegend *legRunDiffYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeUnRegIn1,fitResultRunDiffYSlopeUnRegIn1Error),"lp");
  legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeUnRegOut1,fitResultRunDiffYSlopeUnRegOut1Error),"lp");
//   legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYSlopeUnRegInpOut1,resultRunDiffYSlopeUnRegInpOut1Error),"");
//   legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYSlopeUnRegInmOut1,resultRunDiffYSlopeUnRegInmOut1Error),"");
  legRunDiffYSlopeUnReg1->SetTextSize(0.045);
  legRunDiffYSlopeUnReg1->SetFillColor(0);
  legRunDiffYSlopeUnReg1->SetBorderSize(2);
  legRunDiffYSlopeUnReg1->Draw();

  gPad->Update();


  pad61->cd(5);

  TGraphErrors * runDiffEGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,diffECalcUnRegInVal,zeroUnRegIn,ediffECalcUnRegInVal);
  runDiffEGraphUnRegIn1->SetName("runDiffEGraphUnRegIn1");
  runDiffEGraphUnRegIn1->SetMarkerColor(kBlue);
  runDiffEGraphUnRegIn1->SetLineColor(kBlue);
  runDiffEGraphUnRegIn1->SetMarkerStyle(24);
  runDiffEGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffEGraphUnRegIn1->SetLineWidth(1);
//   runDiffEGraphUnRegIn1->Draw("AP");
  runDiffEGraphUnRegIn1->Fit("fitRunDiffEGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunDiffEGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffEUnRegIn1            =  fitRunDiffEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffEUnRegIn1Error       =  fitRunDiffEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffEUnRegIn1Chisquare   =  fitRunDiffEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffEUnRegIn1NDF         =  fitRunDiffEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffEUnRegIn1Prob        =  fitRunDiffEGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffEGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,diffECalcUnRegOutVal,zeroUnRegOut,ediffECalcUnRegOutVal);
  runDiffEGraphUnRegOut1->SetName("runDiffEGraphUnRegOut1");
  runDiffEGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffEGraphUnRegOut1->SetLineColor(kBlack);
  runDiffEGraphUnRegOut1->SetMarkerStyle(24);
  runDiffEGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffEGraphUnRegOut1->SetLineWidth(1);
//   runDiffEGraphUnRegOut1->Draw("AP");
  runDiffEGraphUnRegOut1->Fit("fitRunDiffEGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffEGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffEUnRegOut1           =  fitRunDiffEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffEUnRegOut1Error      =  fitRunDiffEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffEUnRegOut1Chisquare  =  fitRunDiffEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffEUnRegOut1NDF        =  fitRunDiffEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffEUnRegOut1Prob       =  fitRunDiffEGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffEGraphUnReg1 = new TMultiGraph();
  runDiffEGraphUnReg1->Add(runDiffEGraphUnRegIn1);
  runDiffEGraphUnReg1->Add(runDiffEGraphUnRegOut1);
  runDiffEGraphUnReg1->Draw("AP");
  runDiffEGraphUnReg1->SetTitle("");
  runDiffEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffEGraphUnReg1->GetYaxis()->SetTitle("Energy Diff [ppm]");
  runDiffEGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffEGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffEGraphUnReg1->GetYaxis()->SetRangeUser(-0.0e-6,0.15e-6);
  TAxis *xaxisRunDiffEGraphUnReg1= runDiffEGraphUnReg1->GetXaxis();
  xaxisRunDiffEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffEGraphUnRegIn1->Draw("same");
  fitRunDiffEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffEUnRegIn1 =(TPaveStats*)runDiffEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffEUnRegOut1=(TPaveStats*)runDiffEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffEUnRegIn1->SetTextColor(kBlue);
  statsRunDiffEUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffEUnRegOut1->SetTextColor(kBlack);
  statsRunDiffEUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffEUnRegInpOut1       =  (fitResultRunDiffEUnRegIn1+fitResultRunDiffEUnRegOut1)/2;
  Double_t resultRunDiffEUnRegInmOut1       =  (fitResultRunDiffEUnRegIn1-fitResultRunDiffEUnRegOut1)/2;
  Double_t resultRunDiffEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffEUnRegIn1Error*fitResultRunDiffEUnRegIn1Error+fitResultRunDiffEUnRegOut1Error*fitResultRunDiffEUnRegOut1Error)/2;
  Double_t resultRunDiffEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffEUnRegIn1Error*fitResultRunDiffEUnRegIn1Error+fitResultRunDiffEUnRegOut1Error*fitResultRunDiffEUnRegOut1Error)/2;


  TLegend *legRunDiffEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffEUnRegIn1,fitResultRunDiffEUnRegIn1Error),"lp");
  legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffEUnRegOut1,fitResultRunDiffEUnRegOut1Error),"lp");
//   legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffEUnRegInpOut1,resultRunDiffEUnRegInpOut1Error),"");
//   legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffEUnRegInmOut1,resultRunDiffEUnRegInmOut1Error),"");
  legRunDiffEUnReg1->SetTextSize(0.045);
  legRunDiffEUnReg1->SetFillColor(0);
  legRunDiffEUnReg1->SetBorderSize(2);
  legRunDiffEUnReg1->Draw();

  gPad->Update();

  pad61->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,chargeRegInVal,zeroUnRegIn,echargeRegInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q ");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,chargeRegOutVal,zeroUnRegOut,echargeRegOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q ");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-20,20);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();

  ofstream outfileUnRegDiffAvgIn(Form("%s/plots/%s/summary%sRunletUnRegDiffAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileUnRegDiffAvgIn << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
				,fitResultRunDiffXUnRegIn1,fitResultRunDiffXUnRegIn1Error
				,fitResultRunDiffXSlopeUnRegIn1,fitResultRunDiffXSlopeUnRegIn1Error
				,fitResultRunDiffYUnRegIn1,fitResultRunDiffYUnRegIn1Error
				,fitResultRunDiffYSlopeUnRegIn1,fitResultRunDiffYSlopeUnRegIn1Error
				,fitResultRunDiffEUnRegIn1,fitResultRunDiffEUnRegIn1Error
				,fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error)
			<<endl;
  outfileUnRegDiffAvgIn.close();

  ofstream outfileUnRegDiffAvgOut(Form("%s/plots/%s/summary%sRunletUnRegDiffAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileUnRegDiffAvgOut << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
				,fitResultRunDiffXUnRegOut1,fitResultRunDiffXUnRegOut1Error
				,fitResultRunDiffXSlopeUnRegOut1,fitResultRunDiffXSlopeUnRegOut1Error
				,fitResultRunDiffYUnRegOut1,fitResultRunDiffYUnRegOut1Error
				,fitResultRunDiffYSlopeUnRegOut1,fitResultRunDiffYSlopeUnRegOut1Error
				,fitResultRunDiffEUnRegOut1,fitResultRunDiffEUnRegOut1Error
				,fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error)
			<<endl;
  outfileUnRegDiffAvgOut.close();


  c6->Update();
  c6->SaveAs(Form("%s/plots/%s/TargetDiffUnRegRunletbyRunlet.png",n2deltaAnalysisDir.Data(),regScheme));


  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c7 = new TCanvas("c7","c7",cx1,cy1,1200,950);
  TPad * pad70 = new TPad("pad70","pad70",ps1,ps2,ps4,ps4);
  TPad * pad71 = new TPad("pad71","pad71",ps1,ps1,ps4,ps3);
  pad70->Draw();
  pad71->Draw();
  pad70->cd();
  TText * ct70 = new TText(tll,tlr,Form("%sN to Delta %s Regressed Position and Energy Differences",regTitle,dataInfo));
  ct70->SetTextSize(tsiz);
  ct70->Draw();
  pad71->cd();
  pad71->Divide(2,3);


  pad71->cd(1);

  TGraphErrors * runDiffXGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffXRegInVal,zeroRegIn,ediffXRegInVal);
  runDiffXGraphRegIn1->SetName("runDiffXGraphRegIn1");
  runDiffXGraphRegIn1->SetMarkerColor(kRed);
  runDiffXGraphRegIn1->SetLineColor(kRed);
  runDiffXGraphRegIn1->SetMarkerStyle(21);
  runDiffXGraphRegIn1->SetMarkerSize(0.5);
  runDiffXGraphRegIn1->SetLineWidth(1);
//   runDiffXGraphRegIn1->Draw("AP");
  runDiffXGraphRegIn1->Fit("fitRunDiffXGraphRegIn1","E M R F 0 Q W");
  fitRunDiffXGraphRegIn1->SetLineColor(kRed);
  fitRunDiffXGraphRegIn1->SetLineWidth(2);
  fitRunDiffXGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXRegIn1            =  fitRunDiffXGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXRegIn1Error       =  fitRunDiffXGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffXRegIn1Chisquare   =  fitRunDiffXGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffXRegIn1NDF         =  fitRunDiffXGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffXRegIn1Prob        =  fitRunDiffXGraphRegIn1->GetProb();


  TGraphErrors * runDiffXGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffXRegOutVal,zeroRegOut,ediffXRegOutVal);
  runDiffXGraphRegOut1->SetName("runDiffXGraphRegOut1");
  runDiffXGraphRegOut1->SetMarkerColor(kBlack);
  runDiffXGraphRegOut1->SetLineColor(kBlack);
  runDiffXGraphRegOut1->SetMarkerStyle(21);
  runDiffXGraphRegOut1->SetMarkerSize(0.5);
  runDiffXGraphRegOut1->SetLineWidth(1);
//   runDiffXGraphRegOut1->Draw("AP");
  runDiffXGraphRegOut1->Fit("fitRunDiffXGraphRegOut1","E M R F 0 Q W");
  fitRunDiffXGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffXGraphRegOut1->SetLineWidth(2);
  fitRunDiffXGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXRegOut1           =  fitRunDiffXGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXRegOut1Error      =  fitRunDiffXGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffXRegOut1Chisquare  =  fitRunDiffXGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffXRegOut1NDF        =  fitRunDiffXGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffXRegOut1Prob       =  fitRunDiffXGraphRegOut1->GetProb();

  TMultiGraph *runDiffXGraphReg1 = new TMultiGraph();
  runDiffXGraphReg1->Add(runDiffXGraphRegIn1);
  runDiffXGraphReg1->Add(runDiffXGraphRegOut1);
  runDiffXGraphReg1->Draw("AP");
  runDiffXGraphReg1->SetTitle("");
  runDiffXGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXGraphReg1->GetYaxis()->SetTitle("Target X Diff [mm]");
  runDiffXGraphReg1->GetXaxis()->CenterTitle();
  runDiffXGraphReg1->GetYaxis()->CenterTitle();
  runDiffXGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXGraphReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffXGraphReg1= runDiffXGraphReg1->GetXaxis();
  xaxisRunDiffXGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXGraphRegIn1->Draw("same");
  fitRunDiffXGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffXRegIn1 =(TPaveStats*)runDiffXGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXRegOut1=(TPaveStats*)runDiffXGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXRegIn1->SetTextColor(kRed);
  statsRunDiffXRegIn1->SetFillColor(kWhite); 
  statsRunDiffXRegOut1->SetTextColor(kBlack);
  statsRunDiffXRegOut1->SetFillColor(kWhite); 
  statsRunDiffXRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXRegInpOut1       =  (fitResultRunDiffXRegIn1+fitResultRunDiffXRegOut1)/2;
  Double_t resultRunDiffXRegInmOut1       =  (fitResultRunDiffXRegIn1-fitResultRunDiffXRegOut1)/2;
  Double_t resultRunDiffXRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXRegIn1Error*fitResultRunDiffXRegIn1Error+fitResultRunDiffXRegOut1Error*fitResultRunDiffXRegOut1Error)/2;
  Double_t resultRunDiffXRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXRegIn1Error*fitResultRunDiffXRegIn1Error+fitResultRunDiffXRegOut1Error*fitResultRunDiffXRegOut1Error)/2;


  TLegend *legRunDiffXReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXReg1->AddEntry("runDiffXGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffXRegIn1,fitResultRunDiffXRegIn1Error),"lp");
  legRunDiffXReg1->AddEntry("runDiffXGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffXRegOut1,fitResultRunDiffXRegOut1Error),"lp");
//   legRunDiffXReg1->AddEntry("runDiffXGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXRegInpOut1,resultRunDiffXRegInpOut1Error),"");
//   legRunDiffXReg1->AddEntry("runDiffXGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXRegInmOut1,resultRunDiffXRegInmOut1Error),"");
  legRunDiffXReg1->SetTextSize(0.045);
  legRunDiffXReg1->SetFillColor(0);
  legRunDiffXReg1->SetBorderSize(2);
  legRunDiffXReg1->Draw();

  gPad->Update();


  pad71->cd(2);

  TGraphErrors * runDiffXSlopeGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffXSlopeRegInVal,zeroRegIn,ediffXSlopeRegInVal);
  runDiffXSlopeGraphRegIn1->SetName("runDiffXSlopeGraphRegIn1");
  runDiffXSlopeGraphRegIn1->SetMarkerColor(kRed);
  runDiffXSlopeGraphRegIn1->SetLineColor(kRed);
  runDiffXSlopeGraphRegIn1->SetMarkerStyle(22);
  runDiffXSlopeGraphRegIn1->SetMarkerSize(0.5);
  runDiffXSlopeGraphRegIn1->SetLineWidth(1);
//   runDiffXSlopeGraphRegIn1->Draw("AP");
  runDiffXSlopeGraphRegIn1->Fit("fitRunDiffXSlopeGraphRegIn1","E M R F 0 Q W");
  fitRunDiffXSlopeGraphRegIn1->SetLineColor(kRed);
  fitRunDiffXSlopeGraphRegIn1->SetLineWidth(2);
  fitRunDiffXSlopeGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeRegIn1            =  fitRunDiffXSlopeGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeRegIn1Error       =  fitRunDiffXSlopeGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffXSlopeRegIn1Chisquare   =  fitRunDiffXSlopeGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffXSlopeRegIn1NDF         =  fitRunDiffXSlopeGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffXSlopeRegIn1Prob        =  fitRunDiffXSlopeGraphRegIn1->GetProb();


  TGraphErrors * runDiffXSlopeGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffXSlopeRegOutVal,zeroRegOut,ediffXSlopeRegOutVal);
  runDiffXSlopeGraphRegOut1->SetName("runDiffXSlopeGraphRegOut1");
  runDiffXSlopeGraphRegOut1->SetMarkerColor(kBlack);
  runDiffXSlopeGraphRegOut1->SetLineColor(kBlack);
  runDiffXSlopeGraphRegOut1->SetMarkerStyle(22);
  runDiffXSlopeGraphRegOut1->SetMarkerSize(0.5);
  runDiffXSlopeGraphRegOut1->SetLineWidth(1);
//   runDiffXSlopeGraphRegOut1->Draw("AP");
  runDiffXSlopeGraphRegOut1->Fit("fitRunDiffXSlopeGraphRegOut1","E M R F 0 Q W");
  fitRunDiffXSlopeGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffXSlopeGraphRegOut1->SetLineWidth(2);
  fitRunDiffXSlopeGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeRegOut1           =  fitRunDiffXSlopeGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeRegOut1Error      =  fitRunDiffXSlopeGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffXSlopeRegOut1Chisquare  =  fitRunDiffXSlopeGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffXSlopeRegOut1NDF        =  fitRunDiffXSlopeGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffXSlopeRegOut1Prob       =  fitRunDiffXSlopeGraphRegOut1->GetProb();

  TMultiGraph *runDiffXSlopeGraphReg1 = new TMultiGraph();
  runDiffXSlopeGraphReg1->Add(runDiffXSlopeGraphRegIn1);
  runDiffXSlopeGraphReg1->Add(runDiffXSlopeGraphRegOut1);
  runDiffXSlopeGraphReg1->Draw("AP");
  runDiffXSlopeGraphReg1->SetTitle("");
  runDiffXSlopeGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXSlopeGraphReg1->GetYaxis()->SetTitle("Target X Slope Diff [#murad]");
  runDiffXSlopeGraphReg1->GetXaxis()->CenterTitle();
  runDiffXSlopeGraphReg1->GetYaxis()->CenterTitle();
  runDiffXSlopeGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXSlopeGraphReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffXSlopeGraphReg1= runDiffXSlopeGraphReg1->GetXaxis();
  xaxisRunDiffXSlopeGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXSlopeGraphRegIn1->Draw("same");
  fitRunDiffXSlopeGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffXSlopeRegIn1 =(TPaveStats*)runDiffXSlopeGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXSlopeRegOut1=(TPaveStats*)runDiffXSlopeGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXSlopeRegIn1->SetTextColor(kRed);
  statsRunDiffXSlopeRegIn1->SetFillColor(kWhite); 
  statsRunDiffXSlopeRegOut1->SetTextColor(kBlack);
  statsRunDiffXSlopeRegOut1->SetFillColor(kWhite); 
  statsRunDiffXSlopeRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXSlopeRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXSlopeRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXSlopeRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXSlopeRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXSlopeRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXSlopeRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXSlopeRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXSlopeRegInpOut1       =  (fitResultRunDiffXSlopeRegIn1+fitResultRunDiffXSlopeRegOut1)/2;
  Double_t resultRunDiffXSlopeRegInmOut1       =  (fitResultRunDiffXSlopeRegIn1-fitResultRunDiffXSlopeRegOut1)/2;
  Double_t resultRunDiffXSlopeRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeRegIn1Error*fitResultRunDiffXSlopeRegIn1Error+fitResultRunDiffXSlopeRegOut1Error*fitResultRunDiffXSlopeRegOut1Error)/2;
  Double_t resultRunDiffXSlopeRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeRegIn1Error*fitResultRunDiffXSlopeRegIn1Error+fitResultRunDiffXSlopeRegOut1Error*fitResultRunDiffXSlopeRegOut1Error)/2;


  TLegend *legRunDiffXSlopeReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeRegIn1,fitResultRunDiffXSlopeRegIn1Error),"lp");
  legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeRegOut1,fitResultRunDiffXSlopeRegOut1Error),"lp");
//   legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXSlopeRegInpOut1,resultRunDiffXSlopeRegInpOut1Error),"");
//   legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXSlopeRegInmOut1,resultRunDiffXSlopeRegInmOut1Error),"");
  legRunDiffXSlopeReg1->SetTextSize(0.045);
  legRunDiffXSlopeReg1->SetFillColor(0);
  legRunDiffXSlopeReg1->SetBorderSize(2);
  legRunDiffXSlopeReg1->Draw();

  gPad->Update();


  pad71->cd(3);

  TGraphErrors * runDiffYGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffYRegInVal,zeroRegIn,ediffYRegInVal);
  runDiffYGraphRegIn1->SetName("runDiffYGraphRegIn1");
  runDiffYGraphRegIn1->SetMarkerColor(kGreen);
  runDiffYGraphRegIn1->SetLineColor(kGreen);
  runDiffYGraphRegIn1->SetMarkerStyle(21);
  runDiffYGraphRegIn1->SetMarkerSize(0.5);
  runDiffYGraphRegIn1->SetLineWidth(1);
//   runDiffYGraphRegIn1->Draw("AP");
  runDiffYGraphRegIn1->Fit("fitRunDiffYGraphRegIn1","E M R F 0 Q W");
  fitRunDiffYGraphRegIn1->SetLineColor(kGreen);
  fitRunDiffYGraphRegIn1->SetLineWidth(2);
  fitRunDiffYGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYRegIn1            =  fitRunDiffYGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYRegIn1Error       =  fitRunDiffYGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffYRegIn1Chisquare   =  fitRunDiffYGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffYRegIn1NDF         =  fitRunDiffYGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffYRegIn1Prob        =  fitRunDiffYGraphRegIn1->GetProb();


  TGraphErrors * runDiffYGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffYRegOutVal,zeroRegOut,ediffYRegOutVal);
  runDiffYGraphRegOut1->SetName("runDiffYGraphRegOut1");
  runDiffYGraphRegOut1->SetMarkerColor(kBlack);
  runDiffYGraphRegOut1->SetLineColor(kBlack);
  runDiffYGraphRegOut1->SetMarkerStyle(21);
  runDiffYGraphRegOut1->SetMarkerSize(0.5);
  runDiffYGraphRegOut1->SetLineWidth(1);
//   runDiffYGraphRegOut1->Draw("AP");
  runDiffYGraphRegOut1->Fit("fitRunDiffYGraphRegOut1","E M R F 0 Q W");
  fitRunDiffYGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffYGraphRegOut1->SetLineWidth(2);
  fitRunDiffYGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYRegOut1           =  fitRunDiffYGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYRegOut1Error      =  fitRunDiffYGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffYRegOut1Chisquare  =  fitRunDiffYGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffYRegOut1NDF        =  fitRunDiffYGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffYRegOut1Prob       =  fitRunDiffYGraphRegOut1->GetProb();

  TMultiGraph *runDiffYGraphReg1 = new TMultiGraph();
  runDiffYGraphReg1->Add(runDiffYGraphRegIn1);
  runDiffYGraphReg1->Add(runDiffYGraphRegOut1);
  runDiffYGraphReg1->Draw("AP");
  runDiffYGraphReg1->SetTitle("");
  runDiffYGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYGraphReg1->GetYaxis()->SetTitle("Target Y Diff [mm]");
  runDiffYGraphReg1->GetXaxis()->CenterTitle();
  runDiffYGraphReg1->GetYaxis()->CenterTitle();
  runDiffYGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYGraphReg1->GetYaxis()->SetRangeUser(-0,0.0012);
  TAxis *xaxisRunDiffYGraphReg1= runDiffYGraphReg1->GetXaxis();
  xaxisRunDiffYGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYGraphRegIn1->Draw("same");
  fitRunDiffYGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffYRegIn1 =(TPaveStats*)runDiffYGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYRegOut1=(TPaveStats*)runDiffYGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYRegIn1->SetTextColor(kGreen);
  statsRunDiffYRegIn1->SetFillColor(kWhite); 
  statsRunDiffYRegOut1->SetTextColor(kBlack);
  statsRunDiffYRegOut1->SetFillColor(kWhite); 
  statsRunDiffYRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYRegInpOut1       =  (fitResultRunDiffYRegIn1+fitResultRunDiffYRegOut1)/2;
  Double_t resultRunDiffYRegInmOut1       =  (fitResultRunDiffYRegIn1-fitResultRunDiffYRegOut1)/2;
  Double_t resultRunDiffYRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYRegIn1Error*fitResultRunDiffYRegIn1Error+fitResultRunDiffYRegOut1Error*fitResultRunDiffYRegOut1Error)/2;
  Double_t resultRunDiffYRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYRegIn1Error*fitResultRunDiffYRegIn1Error+fitResultRunDiffYRegOut1Error*fitResultRunDiffYRegOut1Error)/2;


  TLegend *legRunDiffYReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYReg1->AddEntry("runDiffYGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffYRegIn1,fitResultRunDiffYRegIn1Error),"lp");
  legRunDiffYReg1->AddEntry("runDiffYGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffYRegOut1,fitResultRunDiffYRegOut1Error),"lp");
//   legRunDiffYReg1->AddEntry("runDiffYGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYRegInpOut1,resultRunDiffYRegInpOut1Error),"");
//   legRunDiffYReg1->AddEntry("runDiffYGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYRegInmOut1,resultRunDiffYRegInmOut1Error),"");
  legRunDiffYReg1->SetTextSize(0.045);
  legRunDiffYReg1->SetFillColor(0);
  legRunDiffYReg1->SetBorderSize(2);
  legRunDiffYReg1->Draw();

  gPad->Update();


  pad71->cd(4);

  TGraphErrors * runDiffYSlopeGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffYSlopeRegInVal,zeroRegIn,ediffYSlopeRegInVal);
  runDiffYSlopeGraphRegIn1->SetName("runDiffYSlopeGraphRegIn1");
  runDiffYSlopeGraphRegIn1->SetMarkerColor(kGreen);
  runDiffYSlopeGraphRegIn1->SetLineColor(kGreen);
  runDiffYSlopeGraphRegIn1->SetMarkerStyle(22);
  runDiffYSlopeGraphRegIn1->SetMarkerSize(0.5);
  runDiffYSlopeGraphRegIn1->SetLineWidth(1);
//   runDiffYSlopeGraphRegIn1->Draw("AP");
  runDiffYSlopeGraphRegIn1->Fit("fitRunDiffYSlopeGraphRegIn1","E M R F 0 Q W");
  fitRunDiffYSlopeGraphRegIn1->SetLineColor(kGreen);
  fitRunDiffYSlopeGraphRegIn1->SetLineWidth(2);
  fitRunDiffYSlopeGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeRegIn1            =  fitRunDiffYSlopeGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeRegIn1Error       =  fitRunDiffYSlopeGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffYSlopeRegIn1Chisquare   =  fitRunDiffYSlopeGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffYSlopeRegIn1NDF         =  fitRunDiffYSlopeGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffYSlopeRegIn1Prob        =  fitRunDiffYSlopeGraphRegIn1->GetProb();


  TGraphErrors * runDiffYSlopeGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffYSlopeRegOutVal,zeroRegOut,ediffYSlopeRegOutVal);
  runDiffYSlopeGraphRegOut1->SetName("runDiffYSlopeGraphRegOut1");
  runDiffYSlopeGraphRegOut1->SetMarkerColor(kBlack);
  runDiffYSlopeGraphRegOut1->SetLineColor(kBlack);
  runDiffYSlopeGraphRegOut1->SetMarkerStyle(22);
  runDiffYSlopeGraphRegOut1->SetMarkerSize(0.5);
  runDiffYSlopeGraphRegOut1->SetLineWidth(1);
//   runDiffYSlopeGraphRegOut1->Draw("AP");
  runDiffYSlopeGraphRegOut1->Fit("fitRunDiffYSlopeGraphRegOut1","E M R F 0 Q W");
  fitRunDiffYSlopeGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffYSlopeGraphRegOut1->SetLineWidth(2);
  fitRunDiffYSlopeGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeRegOut1           =  fitRunDiffYSlopeGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeRegOut1Error      =  fitRunDiffYSlopeGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffYSlopeRegOut1Chisquare  =  fitRunDiffYSlopeGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffYSlopeRegOut1NDF        =  fitRunDiffYSlopeGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffYSlopeRegOut1Prob       =  fitRunDiffYSlopeGraphRegOut1->GetProb();

  TMultiGraph *runDiffYSlopeGraphReg1 = new TMultiGraph();
  runDiffYSlopeGraphReg1->Add(runDiffYSlopeGraphRegIn1);
  runDiffYSlopeGraphReg1->Add(runDiffYSlopeGraphRegOut1);
  runDiffYSlopeGraphReg1->Draw("AP");
  runDiffYSlopeGraphReg1->SetTitle("");
  runDiffYSlopeGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYSlopeGraphReg1->GetYaxis()->SetTitle("Target Y Slope Diff [#murad]");
  runDiffYSlopeGraphReg1->GetXaxis()->CenterTitle();
  runDiffYSlopeGraphReg1->GetYaxis()->CenterTitle();
  runDiffYSlopeGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYSlopeGraphReg1->GetYaxis()->SetRangeUser(-0.0,0.04);
  TAxis *xaxisRunDiffYSlopeGraphReg1= runDiffYSlopeGraphReg1->GetXaxis();
  xaxisRunDiffYSlopeGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYSlopeGraphRegIn1->Draw("same");
  fitRunDiffYSlopeGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffYSlopeRegIn1 =(TPaveStats*)runDiffYSlopeGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYSlopeRegOut1=(TPaveStats*)runDiffYSlopeGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYSlopeRegIn1->SetTextColor(kGreen);
  statsRunDiffYSlopeRegIn1->SetFillColor(kWhite); 
  statsRunDiffYSlopeRegOut1->SetTextColor(kBlack);
  statsRunDiffYSlopeRegOut1->SetFillColor(kWhite); 
  statsRunDiffYSlopeRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYSlopeRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYSlopeRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYSlopeRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYSlopeRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYSlopeRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYSlopeRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYSlopeRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYSlopeRegInpOut1       =  (fitResultRunDiffYSlopeRegIn1+fitResultRunDiffYSlopeRegOut1)/2;
  Double_t resultRunDiffYSlopeRegInmOut1       =  (fitResultRunDiffYSlopeRegIn1-fitResultRunDiffYSlopeRegOut1)/2;
  Double_t resultRunDiffYSlopeRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeRegIn1Error*fitResultRunDiffYSlopeRegIn1Error+fitResultRunDiffYSlopeRegOut1Error*fitResultRunDiffYSlopeRegOut1Error)/2;
  Double_t resultRunDiffYSlopeRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeRegIn1Error*fitResultRunDiffYSlopeRegIn1Error+fitResultRunDiffYSlopeRegOut1Error*fitResultRunDiffYSlopeRegOut1Error)/2;


  TLegend *legRunDiffYSlopeReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeRegIn1,fitResultRunDiffYSlopeRegIn1Error),"lp");
  legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeRegOut1,fitResultRunDiffYSlopeRegOut1Error),"lp");
//   legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYSlopeRegInpOut1,resultRunDiffYSlopeRegInpOut1Error),"");
//   legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYSlopeRegInmOut1,resultRunDiffYSlopeRegInmOut1Error),"");
  legRunDiffYSlopeReg1->SetTextSize(0.045);
  legRunDiffYSlopeReg1->SetFillColor(0);
  legRunDiffYSlopeReg1->SetBorderSize(2);
  legRunDiffYSlopeReg1->Draw();

  gPad->Update();


  pad71->cd(5);

  TGraphErrors * runDiffEGraphRegIn1 = new TGraphErrors(counterRegIn,runNumberRegIn,diffERegInVal,zeroRegIn,ediffERegInVal);
  runDiffEGraphRegIn1->SetName("runDiffEGraphRegIn1");
  runDiffEGraphRegIn1->SetMarkerColor(kBlue);
  runDiffEGraphRegIn1->SetLineColor(kBlue);
  runDiffEGraphRegIn1->SetMarkerStyle(20);
  runDiffEGraphRegIn1->SetMarkerSize(0.5);
  runDiffEGraphRegIn1->SetLineWidth(1);
//   runDiffEGraphRegIn1->Draw("AP");
  runDiffEGraphRegIn1->Fit("fitRunDiffEGraphRegIn1","E M R F 0 Q W");
  fitRunDiffEGraphRegIn1->SetLineColor(kBlue);
  fitRunDiffEGraphRegIn1->SetLineWidth(2);
  fitRunDiffEGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffERegIn1            =  fitRunDiffEGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffERegIn1Error       =  fitRunDiffEGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffERegIn1Chisquare   =  fitRunDiffEGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffERegIn1NDF         =  fitRunDiffEGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffERegIn1Prob        =  fitRunDiffEGraphRegIn1->GetProb();


  TGraphErrors * runDiffEGraphRegOut1 = new TGraphErrors(counterRegOut,runNumberRegOut,diffERegOutVal,zeroRegOut,ediffERegOutVal);
  runDiffEGraphRegOut1->SetName("runDiffEGraphRegOut1");
  runDiffEGraphRegOut1->SetMarkerColor(kBlack);
  runDiffEGraphRegOut1->SetLineColor(kBlack);
  runDiffEGraphRegOut1->SetMarkerStyle(20);
  runDiffEGraphRegOut1->SetMarkerSize(0.5);
  runDiffEGraphRegOut1->SetLineWidth(1);
//   runDiffEGraphRegOut1->Draw("AP");
  runDiffEGraphRegOut1->Fit("fitRunDiffEGraphRegOut1","E M R F 0 Q W");
  fitRunDiffEGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffEGraphRegOut1->SetLineWidth(2);
  fitRunDiffEGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffERegOut1           =  fitRunDiffEGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffERegOut1Error      =  fitRunDiffEGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffERegOut1Chisquare  =  fitRunDiffEGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffERegOut1NDF        =  fitRunDiffEGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffERegOut1Prob       =  fitRunDiffEGraphRegOut1->GetProb();

  TMultiGraph *runDiffEGraphReg1 = new TMultiGraph();
  runDiffEGraphReg1->Add(runDiffEGraphRegIn1);
  runDiffEGraphReg1->Add(runDiffEGraphRegOut1);
  runDiffEGraphReg1->Draw("AP");
  runDiffEGraphReg1->SetTitle("");
  runDiffEGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffEGraphReg1->GetYaxis()->SetTitle("Energy Diff [ppm]");
  runDiffEGraphReg1->GetXaxis()->CenterTitle();
  runDiffEGraphReg1->GetYaxis()->CenterTitle();
  runDiffEGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffEGraphReg1->GetYaxis()->SetRangeUser(-0.0e-6,0.15e-6);
  TAxis *xaxisRunDiffEGraphReg1= runDiffEGraphReg1->GetXaxis();
  xaxisRunDiffEGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffEGraphRegIn1->Draw("same");
  fitRunDiffEGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffERegIn1 =(TPaveStats*)runDiffEGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffERegOut1=(TPaveStats*)runDiffEGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffERegIn1->SetTextColor(kBlue);
  statsRunDiffERegIn1->SetFillColor(kWhite); 
  statsRunDiffERegOut1->SetTextColor(kBlack);
  statsRunDiffERegOut1->SetFillColor(kWhite); 
  statsRunDiffERegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffERegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffERegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffERegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffERegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffERegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffERegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffERegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffERegInpOut1       =  (fitResultRunDiffERegIn1+fitResultRunDiffERegOut1)/2;
  Double_t resultRunDiffERegInmOut1       =  (fitResultRunDiffERegIn1-fitResultRunDiffERegOut1)/2;
  Double_t resultRunDiffERegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffERegIn1Error*fitResultRunDiffERegIn1Error+fitResultRunDiffERegOut1Error*fitResultRunDiffERegOut1Error)/2;
  Double_t resultRunDiffERegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffERegIn1Error*fitResultRunDiffERegIn1Error+fitResultRunDiffERegOut1Error*fitResultRunDiffERegOut1Error)/2;


  TLegend *legRunDiffEReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffEReg1->AddEntry("runDiffEGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffERegIn1,fitResultRunDiffERegIn1Error),"lp");
  legRunDiffEReg1->AddEntry("runDiffEGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffERegOut1,fitResultRunDiffERegOut1Error),"lp");
//   legRunDiffEReg1->AddEntry("runDiffEGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffERegInpOut1,resultRunDiffERegInpOut1Error),"");
//   legRunDiffEReg1->AddEntry("runDiffEGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffERegInmOut1,resultRunDiffERegInmOut1Error),"");
  legRunDiffEReg1->SetTextSize(0.045);
  legRunDiffEReg1->SetFillColor(0);
  legRunDiffEReg1->SetBorderSize(2);
  legRunDiffEReg1->Draw();

  gPad->Update();

  pad71->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRegIn,runNumberUnRegIn,chargeRegInVal,zeroUnRegIn,echargeRegInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberUnRegOut,chargeRegOutVal,zeroUnRegOut,echargeRegOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-20,20);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();

  ofstream outfileRegDiffAvgIn(Form("%s/plots/%s/summary%sRunletRegDiffAvgIn%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRegDiffAvgIn << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
			      ,fitResultRunDiffXRegIn1,fitResultRunDiffXRegIn1Error
			      ,fitResultRunDiffXSlopeRegIn1,fitResultRunDiffXSlopeRegIn1Error
			      ,fitResultRunDiffYRegIn1,fitResultRunDiffYRegIn1Error
			      ,fitResultRunDiffYSlopeRegIn1,fitResultRunDiffYSlopeRegIn1Error
			      ,fitResultRunDiffERegIn1,fitResultRunDiffERegIn1Error
			      ,fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error)
			<<endl;
  outfileRegDiffAvgIn.close();

  ofstream outfileRegDiffAvgOut(Form("%s/plots/%s/summary%sRunletRegDiffAvgOut%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
  outfileRegDiffAvgOut << Form("%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f\t%7.9f"
			       ,fitResultRunDiffXRegOut1,fitResultRunDiffXRegOut1Error
			       ,fitResultRunDiffXSlopeRegOut1,fitResultRunDiffXSlopeRegOut1Error
			       ,fitResultRunDiffYRegOut1,fitResultRunDiffYRegOut1Error
			       ,fitResultRunDiffYSlopeRegOut1,fitResultRunDiffYSlopeRegOut1Error
			       ,fitResultRunDiffERegOut1,fitResultRunDiffERegOut1Error
			       ,fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error)
			<<endl;
  outfileRegDiffAvgOut.close();

  c7->Update();
  c7->SaveAs(Form("%s/plots/%s/TargetDiffRegRunletbyRunlet%s.png",n2deltaAnalysisDir.Data(),regScheme,regScheme));


  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(RUN_DIFF_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadLeftMargin(0.10);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TCanvas *c6 = new TCanvas("c6","c6",cx1,cy1,1200,950);
  TPad * pad60 = new TPad("pad60","pad60",ps1,ps2,ps4,ps4);
  TPad * pad61 = new TPad("pad61","pad61",ps1,ps1,ps4,ps3);
  pad60->Draw();
  pad61->Draw();
  pad60->cd();
  TText * ct60 = new TText(tll,tlr,Form("%sN to Delta %s Un-Regressed Position and Energy Differences",regTitle,dataInfo));
  ct60->SetTextSize(tsiz);
  ct60->Draw();
  pad61->cd();
  pad61->Divide(2,3);


  pad61->cd(1);

  TGraphErrors * runDiffXGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffXUnRegRunInVal,zeroRunIn,ediffXUnRegRunInVal);
//   TGraphErrors * runDiffXGraphUnRegIn1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testDiffXUnRegIn.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
  runDiffXGraphUnRegIn1->SetName("runDiffXGraphUnRegIn1");
  runDiffXGraphUnRegIn1->SetMarkerColor(kRed);
  runDiffXGraphUnRegIn1->SetLineColor(kRed);
  runDiffXGraphUnRegIn1->SetMarkerStyle(25);
  runDiffXGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffXGraphUnRegIn1->SetLineWidth(1);
//   runDiffXGraphUnRegIn1->Draw("AP");
  runDiffXGraphUnRegIn1->Fit("fitRunDiffXGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffXGraphUnRegIn1->SetLineColor(kRed);
  fitRunDiffXGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffXGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXUnRegIn1            =  fitRunDiffXGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXUnRegIn1Error       =  fitRunDiffXGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffXUnRegIn1Chisquare   =  fitRunDiffXGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffXUnRegIn1NDF         =  fitRunDiffXGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffXUnRegIn1Prob        =  fitRunDiffXGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffXGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffXUnRegRunOutVal,zeroRunOut,ediffXUnRegRunOutVal);
//   TGraphErrors * runDiffXGraphUnRegOut1 = new TGraphErrors(Form("%s/dataBaseAnalysis/testDiffXUnRegOut.txt",n2deltaAnalysisDir.Data()),"%lg %lg %lg");
  runDiffXGraphUnRegOut1->SetName("runDiffXGraphUnRegOut1");
  runDiffXGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffXGraphUnRegOut1->SetLineColor(kBlack);
  runDiffXGraphUnRegOut1->SetMarkerStyle(25);
  runDiffXGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffXGraphUnRegOut1->SetLineWidth(1);
//   runDiffXGraphUnRegOut1->Draw("AP");
  runDiffXGraphUnRegOut1->Fit("fitRunDiffXGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffXGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffXGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffXGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXUnRegOut1           =  fitRunDiffXGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXUnRegOut1Error      =  fitRunDiffXGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffXUnRegOut1Chisquare  =  fitRunDiffXGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffXUnRegOut1NDF        =  fitRunDiffXGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffXUnRegOut1Prob       =  fitRunDiffXGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffXGraphUnReg1 = new TMultiGraph();
  runDiffXGraphUnReg1->Add(runDiffXGraphUnRegIn1);
  runDiffXGraphUnReg1->Add(runDiffXGraphUnRegOut1);
  runDiffXGraphUnReg1->Draw("AP");
  runDiffXGraphUnReg1->SetTitle("");
  runDiffXGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXGraphUnReg1->GetYaxis()->SetTitle("Target X Diff [mm]");
  runDiffXGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffXGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffXGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXGraphUnReg1->GetYaxis()->SetRangeUser(-0,0.0008);
  TAxis *xaxisRunDiffXGraphUnReg1= runDiffXGraphUnReg1->GetXaxis();
  xaxisRunDiffXGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXGraphUnRegIn1->Draw("same");
  fitRunDiffXGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffXUnRegIn1 =(TPaveStats*)runDiffXGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXUnRegOut1=(TPaveStats*)runDiffXGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXUnRegIn1->SetTextColor(kRed);
  statsRunDiffXUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffXUnRegOut1->SetTextColor(kBlack);
  statsRunDiffXUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffXUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXUnRegInpOut1       =  (fitResultRunDiffXUnRegIn1+fitResultRunDiffXUnRegOut1)/2;
  Double_t resultRunDiffXUnRegInmOut1       =  (fitResultRunDiffXUnRegIn1-fitResultRunDiffXUnRegOut1)/2;
  Double_t resultRunDiffXUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXUnRegIn1Error*fitResultRunDiffXUnRegIn1Error+fitResultRunDiffXUnRegOut1Error*fitResultRunDiffXUnRegOut1Error)/2;
  Double_t resultRunDiffXUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXUnRegIn1Error*fitResultRunDiffXUnRegIn1Error+fitResultRunDiffXUnRegOut1Error*fitResultRunDiffXUnRegOut1Error)/2;


  TLegend *legRunDiffXUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffXUnRegIn1,fitResultRunDiffXUnRegIn1Error),"lp");
  legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffXUnRegOut1,fitResultRunDiffXUnRegOut1Error),"lp");
//   legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXUnRegInpOut1,resultRunDiffXUnRegInpOut1Error),"");
//   legRunDiffXUnReg1->AddEntry("runDiffXGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXUnRegInmOut1,resultRunDiffXUnRegInmOut1Error),"");
  legRunDiffXUnReg1->SetTextSize(0.045);
  legRunDiffXUnReg1->SetFillColor(0);
  legRunDiffXUnReg1->SetBorderSize(2);
  legRunDiffXUnReg1->Draw();

  gPad->Update();


  pad61->cd(2);

  TGraphErrors * runDiffXSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffXSlopeUnRegRunInVal,zeroRunIn,ediffXSlopeUnRegRunInVal);
  runDiffXSlopeGraphUnRegIn1->SetName("runDiffXSlopeGraphUnRegIn1");
  runDiffXSlopeGraphUnRegIn1->SetMarkerColor(kRed);
  runDiffXSlopeGraphUnRegIn1->SetLineColor(kRed);
  runDiffXSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runDiffXSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffXSlopeGraphUnRegIn1->SetLineWidth(1);
//   runDiffXSlopeGraphUnRegIn1->Draw("AP");
  runDiffXSlopeGraphUnRegIn1->Fit("fitRunDiffXSlopeGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffXSlopeGraphUnRegIn1->SetLineColor(kRed);
  fitRunDiffXSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffXSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeUnRegIn1            =  fitRunDiffXSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeUnRegIn1Error       =  fitRunDiffXSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffXSlopeUnRegIn1Chisquare   =  fitRunDiffXSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffXSlopeUnRegIn1NDF         =  fitRunDiffXSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffXSlopeUnRegIn1Prob        =  fitRunDiffXSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffXSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffXSlopeUnRegRunOutVal,zeroRunOut,ediffXSlopeUnRegRunOutVal);
  runDiffXSlopeGraphUnRegOut1->SetName("runDiffXSlopeGraphUnRegOut1");
  runDiffXSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runDiffXSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runDiffXSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffXSlopeGraphUnRegOut1->SetLineWidth(1);
//   runDiffXSlopeGraphUnRegOut1->Draw("AP");
  runDiffXSlopeGraphUnRegOut1->Fit("fitRunDiffXSlopeGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffXSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffXSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffXSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeUnRegOut1           =  fitRunDiffXSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeUnRegOut1Error      =  fitRunDiffXSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffXSlopeUnRegOut1Chisquare  =  fitRunDiffXSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffXSlopeUnRegOut1NDF        =  fitRunDiffXSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffXSlopeUnRegOut1Prob       =  fitRunDiffXSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffXSlopeGraphUnReg1 = new TMultiGraph();
  runDiffXSlopeGraphUnReg1->Add(runDiffXSlopeGraphUnRegIn1);
  runDiffXSlopeGraphUnReg1->Add(runDiffXSlopeGraphUnRegOut1);
  runDiffXSlopeGraphUnReg1->Draw("AP");
  runDiffXSlopeGraphUnReg1->SetTitle("");
  runDiffXSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXSlopeGraphUnReg1->GetYaxis()->SetTitle("Target X Slope Diff [#murad]");
  runDiffXSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffXSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffXSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-0.0,0.03);
  TAxis *xaxisRunDiffXSlopeGraphUnReg1= runDiffXSlopeGraphUnReg1->GetXaxis();
  xaxisRunDiffXSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXSlopeGraphUnRegIn1->Draw("same");
  fitRunDiffXSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffXSlopeUnRegIn1 =(TPaveStats*)runDiffXSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXSlopeUnRegOut1=(TPaveStats*)runDiffXSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXSlopeUnRegIn1->SetTextColor(kRed);
  statsRunDiffXSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffXSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunDiffXSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffXSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXSlopeUnRegInpOut1       =  (fitResultRunDiffXSlopeUnRegIn1+fitResultRunDiffXSlopeUnRegOut1)/2;
  Double_t resultRunDiffXSlopeUnRegInmOut1       =  (fitResultRunDiffXSlopeUnRegIn1-fitResultRunDiffXSlopeUnRegOut1)/2;
  Double_t resultRunDiffXSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeUnRegIn1Error*fitResultRunDiffXSlopeUnRegIn1Error+fitResultRunDiffXSlopeUnRegOut1Error*fitResultRunDiffXSlopeUnRegOut1Error)/2;
  Double_t resultRunDiffXSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeUnRegIn1Error*fitResultRunDiffXSlopeUnRegIn1Error+fitResultRunDiffXSlopeUnRegOut1Error*fitResultRunDiffXSlopeUnRegOut1Error)/2;


  TLegend *legRunDiffXSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeUnRegIn1,fitResultRunDiffXSlopeUnRegIn1Error),"lp");
  legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeUnRegOut1,fitResultRunDiffXSlopeUnRegOut1Error),"lp");
//   legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXSlopeUnRegInpOut1,resultRunDiffXSlopeUnRegInpOut1Error),"");
//   legRunDiffXSlopeUnReg1->AddEntry("runDiffXSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffXSlopeUnRegInmOut1,resultRunDiffXSlopeUnRegInmOut1Error),"");
  legRunDiffXSlopeUnReg1->SetTextSize(0.045);
  legRunDiffXSlopeUnReg1->SetFillColor(0);
  legRunDiffXSlopeUnReg1->SetBorderSize(2);
  legRunDiffXSlopeUnReg1->Draw();

  gPad->Update();


  pad61->cd(3);

  TGraphErrors * runDiffYGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffYUnRegRunInVal,zeroRunIn,ediffYUnRegRunInVal);
  runDiffYGraphUnRegIn1->SetName("runDiffYGraphUnRegIn1");
  runDiffYGraphUnRegIn1->SetMarkerColor(kGreen);
  runDiffYGraphUnRegIn1->SetLineColor(kGreen);
  runDiffYGraphUnRegIn1->SetMarkerStyle(25);
  runDiffYGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffYGraphUnRegIn1->SetLineWidth(1);
//   runDiffYGraphUnRegIn1->Draw("AP");
  runDiffYGraphUnRegIn1->Fit("fitRunDiffYGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffYGraphUnRegIn1->SetLineColor(kGreen);
  fitRunDiffYGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffYGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYUnRegIn1            =  fitRunDiffYGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYUnRegIn1Error       =  fitRunDiffYGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffYUnRegIn1Chisquare   =  fitRunDiffYGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffYUnRegIn1NDF         =  fitRunDiffYGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffYUnRegIn1Prob        =  fitRunDiffYGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffYGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffYUnRegRunOutVal,zeroRunOut,ediffYUnRegRunOutVal);
  runDiffYGraphUnRegOut1->SetName("runDiffYGraphUnRegOut1");
  runDiffYGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffYGraphUnRegOut1->SetLineColor(kBlack);
  runDiffYGraphUnRegOut1->SetMarkerStyle(25);
  runDiffYGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffYGraphUnRegOut1->SetLineWidth(1);
//   runDiffYGraphUnRegOut1->Draw("AP");
  runDiffYGraphUnRegOut1->Fit("fitRunDiffYGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffYGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffYGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffYGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYUnRegOut1           =  fitRunDiffYGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYUnRegOut1Error      =  fitRunDiffYGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffYUnRegOut1Chisquare  =  fitRunDiffYGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffYUnRegOut1NDF        =  fitRunDiffYGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffYUnRegOut1Prob       =  fitRunDiffYGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffYGraphUnReg1 = new TMultiGraph();
  runDiffYGraphUnReg1->Add(runDiffYGraphUnRegIn1);
  runDiffYGraphUnReg1->Add(runDiffYGraphUnRegOut1);
  runDiffYGraphUnReg1->Draw("AP");
  runDiffYGraphUnReg1->SetTitle("");
  runDiffYGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYGraphUnReg1->GetYaxis()->SetTitle("Target Y Diff [mm]");
  runDiffYGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffYGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffYGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYGraphUnReg1->GetYaxis()->SetRangeUser(-0,0.0008);
  TAxis *xaxisRunDiffYGraphUnReg1= runDiffYGraphUnReg1->GetXaxis();
  xaxisRunDiffYGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYGraphUnRegIn1->Draw("same");
  fitRunDiffYGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffYUnRegIn1 =(TPaveStats*)runDiffYGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYUnRegOut1=(TPaveStats*)runDiffYGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYUnRegIn1->SetTextColor(kGreen);
  statsRunDiffYUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffYUnRegOut1->SetTextColor(kBlack);
  statsRunDiffYUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffYUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYUnRegInpOut1       =  (fitResultRunDiffYUnRegIn1+fitResultRunDiffYUnRegOut1)/2;
  Double_t resultRunDiffYUnRegInmOut1       =  (fitResultRunDiffYUnRegIn1-fitResultRunDiffYUnRegOut1)/2;
  Double_t resultRunDiffYUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYUnRegIn1Error*fitResultRunDiffYUnRegIn1Error+fitResultRunDiffYUnRegOut1Error*fitResultRunDiffYUnRegOut1Error)/2;
  Double_t resultRunDiffYUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYUnRegIn1Error*fitResultRunDiffYUnRegIn1Error+fitResultRunDiffYUnRegOut1Error*fitResultRunDiffYUnRegOut1Error)/2;


  TLegend *legRunDiffYUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffYUnRegIn1,fitResultRunDiffYUnRegIn1Error),"lp");
  legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffYUnRegOut1,fitResultRunDiffYUnRegOut1Error),"lp");
//   legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYUnRegInpOut1,resultRunDiffYUnRegInpOut1Error),"");
//   legRunDiffYUnReg1->AddEntry("runDiffYGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYUnRegInmOut1,resultRunDiffYUnRegInmOut1Error),"");
  legRunDiffYUnReg1->SetTextSize(0.045);
  legRunDiffYUnReg1->SetFillColor(0);
  legRunDiffYUnReg1->SetBorderSize(2);
  legRunDiffYUnReg1->Draw();

  gPad->Update();


  pad61->cd(4);

  TGraphErrors * runDiffYSlopeGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffYSlopeUnRegRunInVal,zeroRunIn,ediffYSlopeUnRegRunInVal);
  runDiffYSlopeGraphUnRegIn1->SetName("runDiffYSlopeGraphUnRegIn1");
  runDiffYSlopeGraphUnRegIn1->SetMarkerColor(kGreen);
  runDiffYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  runDiffYSlopeGraphUnRegIn1->SetMarkerStyle(26);
  runDiffYSlopeGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffYSlopeGraphUnRegIn1->SetLineWidth(1);
//   runDiffYSlopeGraphUnRegIn1->Draw("AP");
  runDiffYSlopeGraphUnRegIn1->Fit("fitRunDiffYSlopeGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffYSlopeGraphUnRegIn1->SetLineColor(kGreen);
  fitRunDiffYSlopeGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffYSlopeGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeUnRegIn1            =  fitRunDiffYSlopeGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeUnRegIn1Error       =  fitRunDiffYSlopeGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffYSlopeUnRegIn1Chisquare   =  fitRunDiffYSlopeGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffYSlopeUnRegIn1NDF         =  fitRunDiffYSlopeGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffYSlopeUnRegIn1Prob        =  fitRunDiffYSlopeGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffYSlopeGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffYSlopeUnRegRunOutVal,zeroRunOut,ediffYSlopeUnRegRunOutVal);
  runDiffYSlopeGraphUnRegOut1->SetName("runDiffYSlopeGraphUnRegOut1");
  runDiffYSlopeGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  runDiffYSlopeGraphUnRegOut1->SetMarkerStyle(26);
  runDiffYSlopeGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffYSlopeGraphUnRegOut1->SetLineWidth(1);
//   runDiffYSlopeGraphUnRegOut1->Draw("AP");
  runDiffYSlopeGraphUnRegOut1->Fit("fitRunDiffYSlopeGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffYSlopeGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffYSlopeGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffYSlopeGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeUnRegOut1           =  fitRunDiffYSlopeGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeUnRegOut1Error      =  fitRunDiffYSlopeGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffYSlopeUnRegOut1Chisquare  =  fitRunDiffYSlopeGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffYSlopeUnRegOut1NDF        =  fitRunDiffYSlopeGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffYSlopeUnRegOut1Prob       =  fitRunDiffYSlopeGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffYSlopeGraphUnReg1 = new TMultiGraph();
  runDiffYSlopeGraphUnReg1->Add(runDiffYSlopeGraphUnRegIn1);
  runDiffYSlopeGraphUnReg1->Add(runDiffYSlopeGraphUnRegOut1);
  runDiffYSlopeGraphUnReg1->Draw("AP");
  runDiffYSlopeGraphUnReg1->SetTitle("");
  runDiffYSlopeGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYSlopeGraphUnReg1->GetYaxis()->SetTitle("Target Y Slope Diff [#murad]");
  runDiffYSlopeGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffYSlopeGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffYSlopeGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYSlopeGraphUnReg1->GetYaxis()->SetRangeUser(-0.0,0.03);
  TAxis *xaxisRunDiffYSlopeGraphUnReg1= runDiffYSlopeGraphUnReg1->GetXaxis();
  xaxisRunDiffYSlopeGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYSlopeGraphUnRegIn1->Draw("same");
  fitRunDiffYSlopeGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffYSlopeUnRegIn1 =(TPaveStats*)runDiffYSlopeGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYSlopeUnRegOut1=(TPaveStats*)runDiffYSlopeGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYSlopeUnRegIn1->SetTextColor(kGreen);
  statsRunDiffYSlopeUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffYSlopeUnRegOut1->SetTextColor(kBlack);
  statsRunDiffYSlopeUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffYSlopeUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYSlopeUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYSlopeUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYSlopeUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYSlopeUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYSlopeUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYSlopeUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYSlopeUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYSlopeUnRegInpOut1       =  (fitResultRunDiffYSlopeUnRegIn1+fitResultRunDiffYSlopeUnRegOut1)/2;
  Double_t resultRunDiffYSlopeUnRegInmOut1       =  (fitResultRunDiffYSlopeUnRegIn1-fitResultRunDiffYSlopeUnRegOut1)/2;
  Double_t resultRunDiffYSlopeUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeUnRegIn1Error*fitResultRunDiffYSlopeUnRegIn1Error+fitResultRunDiffYSlopeUnRegOut1Error*fitResultRunDiffYSlopeUnRegOut1Error)/2;
  Double_t resultRunDiffYSlopeUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeUnRegIn1Error*fitResultRunDiffYSlopeUnRegIn1Error+fitResultRunDiffYSlopeUnRegOut1Error*fitResultRunDiffYSlopeUnRegOut1Error)/2;


  TLegend *legRunDiffYSlopeUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeUnRegIn1,fitResultRunDiffYSlopeUnRegIn1Error),"lp");
  legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeUnRegOut1,fitResultRunDiffYSlopeUnRegOut1Error),"lp");
//   legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYSlopeUnRegInpOut1,resultRunDiffYSlopeUnRegInpOut1Error),"");
//   legRunDiffYSlopeUnReg1->AddEntry("runDiffYSlopeGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffYSlopeUnRegInmOut1,resultRunDiffYSlopeUnRegInmOut1Error),"");
  legRunDiffYSlopeUnReg1->SetTextSize(0.045);
  legRunDiffYSlopeUnReg1->SetFillColor(0);
  legRunDiffYSlopeUnReg1->SetBorderSize(2);
  legRunDiffYSlopeUnReg1->Draw();

  gPad->Update();


  pad61->cd(5);

  TGraphErrors * runDiffEGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffEUnRegRunInVal,zeroRunIn,ediffEUnRegRunInVal);
  runDiffEGraphUnRegIn1->SetName("runDiffEGraphUnRegIn1");
  runDiffEGraphUnRegIn1->SetMarkerColor(kBlue);
  runDiffEGraphUnRegIn1->SetLineColor(kBlue);
  runDiffEGraphUnRegIn1->SetMarkerStyle(24);
  runDiffEGraphUnRegIn1->SetMarkerSize(0.5);
  runDiffEGraphUnRegIn1->SetLineWidth(1);
//   runDiffEGraphUnRegIn1->Draw("AP");
  runDiffEGraphUnRegIn1->Fit("fitRunDiffEGraphUnRegIn1","E M R F 0 Q ");
  fitRunDiffEGraphUnRegIn1->SetLineColor(kBlue);
  fitRunDiffEGraphUnRegIn1->SetLineWidth(2);
  fitRunDiffEGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffEUnRegIn1            =  fitRunDiffEGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunDiffEUnRegIn1Error       =  fitRunDiffEGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunDiffEUnRegIn1Chisquare   =  fitRunDiffEGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunDiffEUnRegIn1NDF         =  fitRunDiffEGraphUnRegIn1->GetNDF();
  Double_t fitResultRunDiffEUnRegIn1Prob        =  fitRunDiffEGraphUnRegIn1->GetProb();


  TGraphErrors * runDiffEGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffEUnRegRunOutVal,zeroRunOut,ediffEUnRegRunOutVal);
  runDiffEGraphUnRegOut1->SetName("runDiffEGraphUnRegOut1");
  runDiffEGraphUnRegOut1->SetMarkerColor(kBlack);
  runDiffEGraphUnRegOut1->SetLineColor(kBlack);
  runDiffEGraphUnRegOut1->SetMarkerStyle(24);
  runDiffEGraphUnRegOut1->SetMarkerSize(0.5);
  runDiffEGraphUnRegOut1->SetLineWidth(1);
//   runDiffEGraphUnRegOut1->Draw("AP");
  runDiffEGraphUnRegOut1->Fit("fitRunDiffEGraphUnRegOut1","E M R F 0 Q ");
  fitRunDiffEGraphUnRegOut1->SetLineColor(kBlack);
  fitRunDiffEGraphUnRegOut1->SetLineWidth(2);
  fitRunDiffEGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffEUnRegOut1           =  fitRunDiffEGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunDiffEUnRegOut1Error      =  fitRunDiffEGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunDiffEUnRegOut1Chisquare  =  fitRunDiffEGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunDiffEUnRegOut1NDF        =  fitRunDiffEGraphUnRegOut1->GetNDF();
  Double_t fitResultRunDiffEUnRegOut1Prob       =  fitRunDiffEGraphUnRegOut1->GetProb();

  TMultiGraph *runDiffEGraphUnReg1 = new TMultiGraph();
  runDiffEGraphUnReg1->Add(runDiffEGraphUnRegIn1);
  runDiffEGraphUnReg1->Add(runDiffEGraphUnRegOut1);
  runDiffEGraphUnReg1->Draw("AP");
  runDiffEGraphUnReg1->SetTitle("");
  runDiffEGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runDiffEGraphUnReg1->GetYaxis()->SetTitle("Energy Diff [ppm]");
  runDiffEGraphUnReg1->GetXaxis()->CenterTitle();
  runDiffEGraphUnReg1->GetYaxis()->CenterTitle();
  runDiffEGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffEGraphUnReg1->GetYaxis()->SetRangeUser(-0.000,0.00000018);
  TAxis *xaxisRunDiffEGraphUnReg1= runDiffEGraphUnReg1->GetXaxis();
  xaxisRunDiffEGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffEGraphUnRegIn1->Draw("same");
  fitRunDiffEGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunDiffEUnRegIn1 =(TPaveStats*)runDiffEGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffEUnRegOut1=(TPaveStats*)runDiffEGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffEUnRegIn1->SetTextColor(kBlue);
  statsRunDiffEUnRegIn1->SetFillColor(kWhite); 
  statsRunDiffEUnRegOut1->SetTextColor(kBlack);
  statsRunDiffEUnRegOut1->SetFillColor(kWhite); 
  statsRunDiffEUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffEUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffEUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffEUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffEUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffEUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffEUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffEUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffEUnRegInpOut1       =  (fitResultRunDiffEUnRegIn1+fitResultRunDiffEUnRegOut1)/2;
  Double_t resultRunDiffEUnRegInmOut1       =  (fitResultRunDiffEUnRegIn1-fitResultRunDiffEUnRegOut1)/2;
  Double_t resultRunDiffEUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffEUnRegIn1Error*fitResultRunDiffEUnRegIn1Error+fitResultRunDiffEUnRegOut1Error*fitResultRunDiffEUnRegOut1Error)/2;
  Double_t resultRunDiffEUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffEUnRegIn1Error*fitResultRunDiffEUnRegIn1Error+fitResultRunDiffEUnRegOut1Error*fitResultRunDiffEUnRegOut1Error)/2;


  TLegend *legRunDiffEUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegIn1",  Form("In UnReg=%4.3g#pm%4.3g",fitResultRunDiffEUnRegIn1,fitResultRunDiffEUnRegIn1Error),"lp");
  legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegOut1",  Form("Out UnReg=%4.3g#pm%4.3g",fitResultRunDiffEUnRegOut1,fitResultRunDiffEUnRegOut1Error),"lp");
//   legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffEUnRegInpOut1,resultRunDiffEUnRegInpOut1Error),"");
//   legRunDiffEUnReg1->AddEntry("runDiffEGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.3g#pm%4.3g",resultRunDiffEUnRegInmOut1,resultRunDiffEUnRegInmOut1Error),"");
  legRunDiffEUnReg1->SetTextSize(0.045);
  legRunDiffEUnReg1->SetFillColor(0);
  legRunDiffEUnReg1->SetBorderSize(2);
  legRunDiffEUnReg1->Draw();

  gPad->Update();

  pad61->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,chargeRunInVal,zeroRunIn,echargeRunInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q ");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,chargeRunOutVal,zeroRunOut,echargeRunOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q ");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-15,15);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();


  c6->Update();
  c6->SaveAs(Form("%s/plots/%s/TargetDiffUnRegRunbyRun.png",n2deltaAnalysisDir.Data(),regScheme));


  /*********************************************************************************/
  /*********************************************************************************/

  TCanvas *c7 = new TCanvas("c7","c7",cx1,cy1,1200,950);
  TPad * pad70 = new TPad("pad70","pad70",ps1,ps2,ps4,ps4);
  TPad * pad71 = new TPad("pad71","pad71",ps1,ps1,ps4,ps3);
  pad70->Draw();
  pad71->Draw();
  pad70->cd();
  TText * ct70 = new TText(tll,tlr,Form("%sN to Delta %s Regressed Position and Energy Differences",regTitle,dataInfo));
  ct70->SetTextSize(tsiz);
  ct70->Draw();
  pad71->cd();
  pad71->Divide(2,3);


  pad71->cd(1);

  TGraphErrors * runDiffXGraphRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffXRegRunInVal,zeroRunIn,ediffXRegRunInVal);
  runDiffXGraphRegIn1->SetName("runDiffXGraphRegIn1");
  runDiffXGraphRegIn1->SetMarkerColor(kRed);
  runDiffXGraphRegIn1->SetLineColor(kRed);
  runDiffXGraphRegIn1->SetMarkerStyle(21);
  runDiffXGraphRegIn1->SetMarkerSize(0.5);
  runDiffXGraphRegIn1->SetLineWidth(1);
//   runDiffXGraphRegIn1->Draw("AP");
  runDiffXGraphRegIn1->Fit("fitRunDiffXGraphRegIn1","E M R F 0 Q W");
  fitRunDiffXGraphRegIn1->SetLineColor(kRed);
  fitRunDiffXGraphRegIn1->SetLineWidth(2);
  fitRunDiffXGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXRegIn1            =  fitRunDiffXGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXRegIn1Error       =  fitRunDiffXGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffXRegIn1Chisquare   =  fitRunDiffXGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffXRegIn1NDF         =  fitRunDiffXGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffXRegIn1Prob        =  fitRunDiffXGraphRegIn1->GetProb();


  TGraphErrors * runDiffXGraphRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffXRegRunOutVal,zeroRunOut,ediffXRegRunOutVal);
  runDiffXGraphRegOut1->SetName("runDiffXGraphRegOut1");
  runDiffXGraphRegOut1->SetMarkerColor(kBlack);
  runDiffXGraphRegOut1->SetLineColor(kBlack);
  runDiffXGraphRegOut1->SetMarkerStyle(21);
  runDiffXGraphRegOut1->SetMarkerSize(0.5);
  runDiffXGraphRegOut1->SetLineWidth(1);
//   runDiffXGraphRegOut1->Draw("AP");
  runDiffXGraphRegOut1->Fit("fitRunDiffXGraphRegOut1","E M R F 0 Q W");
  fitRunDiffXGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffXGraphRegOut1->SetLineWidth(2);
  fitRunDiffXGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXRegOut1           =  fitRunDiffXGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXRegOut1Error      =  fitRunDiffXGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffXRegOut1Chisquare  =  fitRunDiffXGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffXRegOut1NDF        =  fitRunDiffXGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffXRegOut1Prob       =  fitRunDiffXGraphRegOut1->GetProb();

  TMultiGraph *runDiffXGraphReg1 = new TMultiGraph();
  runDiffXGraphReg1->Add(runDiffXGraphRegIn1);
  runDiffXGraphReg1->Add(runDiffXGraphRegOut1);
  runDiffXGraphReg1->Draw("AP");
  runDiffXGraphReg1->SetTitle("");
  runDiffXGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXGraphReg1->GetYaxis()->SetTitle("Target X Diff [mm]");
  runDiffXGraphReg1->GetXaxis()->CenterTitle();
  runDiffXGraphReg1->GetYaxis()->CenterTitle();
  runDiffXGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXGraphReg1->GetYaxis()->SetRangeUser(-0,0.0008);
  TAxis *xaxisRunDiffXGraphReg1= runDiffXGraphReg1->GetXaxis();
  xaxisRunDiffXGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXGraphRegIn1->Draw("same");
  fitRunDiffXGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffXRegIn1 =(TPaveStats*)runDiffXGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXRegOut1=(TPaveStats*)runDiffXGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXRegIn1->SetTextColor(kRed);
  statsRunDiffXRegIn1->SetFillColor(kWhite); 
  statsRunDiffXRegOut1->SetTextColor(kBlack);
  statsRunDiffXRegOut1->SetFillColor(kWhite); 
  statsRunDiffXRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXRegInpOut1       =  (fitResultRunDiffXRegIn1+fitResultRunDiffXRegOut1)/2;
  Double_t resultRunDiffXRegInmOut1       =  (fitResultRunDiffXRegIn1-fitResultRunDiffXRegOut1)/2;
  Double_t resultRunDiffXRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXRegIn1Error*fitResultRunDiffXRegIn1Error+fitResultRunDiffXRegOut1Error*fitResultRunDiffXRegOut1Error)/2;
  Double_t resultRunDiffXRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXRegIn1Error*fitResultRunDiffXRegIn1Error+fitResultRunDiffXRegOut1Error*fitResultRunDiffXRegOut1Error)/2;


  TLegend *legRunDiffXReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXReg1->AddEntry("runDiffXGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffXRegIn1,fitResultRunDiffXRegIn1Error),"lp");
  legRunDiffXReg1->AddEntry("runDiffXGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffXRegOut1,fitResultRunDiffXRegOut1Error),"lp");
//   legRunDiffXReg1->AddEntry("runDiffXGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXRegInpOut1,resultRunDiffXRegInpOut1Error),"");
//   legRunDiffXReg1->AddEntry("runDiffXGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXRegInmOut1,resultRunDiffXRegInmOut1Error),"");
  legRunDiffXReg1->SetTextSize(0.045);
  legRunDiffXReg1->SetFillColor(0);
  legRunDiffXReg1->SetBorderSize(2);
  legRunDiffXReg1->Draw();

  gPad->Update();


  pad71->cd(2);

  TGraphErrors * runDiffXSlopeGraphRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffXSlopeRegRunInVal,zeroRunIn,ediffXSlopeRegRunInVal);
  runDiffXSlopeGraphRegIn1->SetName("runDiffXSlopeGraphRegIn1");
  runDiffXSlopeGraphRegIn1->SetMarkerColor(kRed);
  runDiffXSlopeGraphRegIn1->SetLineColor(kRed);
  runDiffXSlopeGraphRegIn1->SetMarkerStyle(22);
  runDiffXSlopeGraphRegIn1->SetMarkerSize(0.5);
  runDiffXSlopeGraphRegIn1->SetLineWidth(1);
//   runDiffXSlopeGraphRegIn1->Draw("AP");
  runDiffXSlopeGraphRegIn1->Fit("fitRunDiffXSlopeGraphRegIn1","E M R F 0 Q W");
  fitRunDiffXSlopeGraphRegIn1->SetLineColor(kRed);
  fitRunDiffXSlopeGraphRegIn1->SetLineWidth(2);
  fitRunDiffXSlopeGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeRegIn1            =  fitRunDiffXSlopeGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeRegIn1Error       =  fitRunDiffXSlopeGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffXSlopeRegIn1Chisquare   =  fitRunDiffXSlopeGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffXSlopeRegIn1NDF         =  fitRunDiffXSlopeGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffXSlopeRegIn1Prob        =  fitRunDiffXSlopeGraphRegIn1->GetProb();


  TGraphErrors * runDiffXSlopeGraphRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffXSlopeRegRunOutVal,zeroRunOut,ediffXSlopeRegRunOutVal);
  runDiffXSlopeGraphRegOut1->SetName("runDiffXSlopeGraphRegOut1");
  runDiffXSlopeGraphRegOut1->SetMarkerColor(kBlack);
  runDiffXSlopeGraphRegOut1->SetLineColor(kBlack);
  runDiffXSlopeGraphRegOut1->SetMarkerStyle(22);
  runDiffXSlopeGraphRegOut1->SetMarkerSize(0.5);
  runDiffXSlopeGraphRegOut1->SetLineWidth(1);
//   runDiffXSlopeGraphRegOut1->Draw("AP");
  runDiffXSlopeGraphRegOut1->Fit("fitRunDiffXSlopeGraphRegOut1","E M R F 0 Q W");
  fitRunDiffXSlopeGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffXSlopeGraphRegOut1->SetLineWidth(2);
  fitRunDiffXSlopeGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffXSlopeRegOut1           =  fitRunDiffXSlopeGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffXSlopeRegOut1Error      =  fitRunDiffXSlopeGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffXSlopeRegOut1Chisquare  =  fitRunDiffXSlopeGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffXSlopeRegOut1NDF        =  fitRunDiffXSlopeGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffXSlopeRegOut1Prob       =  fitRunDiffXSlopeGraphRegOut1->GetProb();

  TMultiGraph *runDiffXSlopeGraphReg1 = new TMultiGraph();
  runDiffXSlopeGraphReg1->Add(runDiffXSlopeGraphRegIn1);
  runDiffXSlopeGraphReg1->Add(runDiffXSlopeGraphRegOut1);
  runDiffXSlopeGraphReg1->Draw("AP");
  runDiffXSlopeGraphReg1->SetTitle("");
  runDiffXSlopeGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffXSlopeGraphReg1->GetYaxis()->SetTitle("Target X Slope Diff [#murad]");
  runDiffXSlopeGraphReg1->GetXaxis()->CenterTitle();
  runDiffXSlopeGraphReg1->GetYaxis()->CenterTitle();
  runDiffXSlopeGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffXSlopeGraphReg1->GetYaxis()->SetRangeUser(-0.0,0.03);
  TAxis *xaxisRunDiffXSlopeGraphReg1= runDiffXSlopeGraphReg1->GetXaxis();
  xaxisRunDiffXSlopeGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffXSlopeGraphRegIn1->Draw("same");
  fitRunDiffXSlopeGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffXSlopeRegIn1 =(TPaveStats*)runDiffXSlopeGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffXSlopeRegOut1=(TPaveStats*)runDiffXSlopeGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffXSlopeRegIn1->SetTextColor(kRed);
  statsRunDiffXSlopeRegIn1->SetFillColor(kWhite); 
  statsRunDiffXSlopeRegOut1->SetTextColor(kBlack);
  statsRunDiffXSlopeRegOut1->SetFillColor(kWhite); 
  statsRunDiffXSlopeRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffXSlopeRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffXSlopeRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffXSlopeRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffXSlopeRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffXSlopeRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffXSlopeRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffXSlopeRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffXSlopeRegInpOut1       =  (fitResultRunDiffXSlopeRegIn1+fitResultRunDiffXSlopeRegOut1)/2;
  Double_t resultRunDiffXSlopeRegInmOut1       =  (fitResultRunDiffXSlopeRegIn1-fitResultRunDiffXSlopeRegOut1)/2;
  Double_t resultRunDiffXSlopeRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeRegIn1Error*fitResultRunDiffXSlopeRegIn1Error+fitResultRunDiffXSlopeRegOut1Error*fitResultRunDiffXSlopeRegOut1Error)/2;
  Double_t resultRunDiffXSlopeRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffXSlopeRegIn1Error*fitResultRunDiffXSlopeRegIn1Error+fitResultRunDiffXSlopeRegOut1Error*fitResultRunDiffXSlopeRegOut1Error)/2;


  TLegend *legRunDiffXSlopeReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeRegIn1,fitResultRunDiffXSlopeRegIn1Error),"lp");
  legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffXSlopeRegOut1,fitResultRunDiffXSlopeRegOut1Error),"lp");
//   legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXSlopeRegInpOut1,resultRunDiffXSlopeRegInpOut1Error),"");
//   legRunDiffXSlopeReg1->AddEntry("runDiffXSlopeGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffXSlopeRegInmOut1,resultRunDiffXSlopeRegInmOut1Error),"");
  legRunDiffXSlopeReg1->SetTextSize(0.045);
  legRunDiffXSlopeReg1->SetFillColor(0);
  legRunDiffXSlopeReg1->SetBorderSize(2);
  legRunDiffXSlopeReg1->Draw();

  gPad->Update();


  pad71->cd(3);

  TGraphErrors * runDiffYGraphRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffYRegRunInVal,zeroRunIn,ediffYRegRunInVal);
  runDiffYGraphRegIn1->SetName("runDiffYGraphRegIn1");
  runDiffYGraphRegIn1->SetMarkerColor(kGreen);
  runDiffYGraphRegIn1->SetLineColor(kGreen);
  runDiffYGraphRegIn1->SetMarkerStyle(21);
  runDiffYGraphRegIn1->SetMarkerSize(0.5);
  runDiffYGraphRegIn1->SetLineWidth(1);
//   runDiffYGraphRegIn1->Draw("AP");
  runDiffYGraphRegIn1->Fit("fitRunDiffYGraphRegIn1","E M R F 0 Q W");
  fitRunDiffYGraphRegIn1->SetLineColor(kGreen);
  fitRunDiffYGraphRegIn1->SetLineWidth(2);
  fitRunDiffYGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYRegIn1            =  fitRunDiffYGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYRegIn1Error       =  fitRunDiffYGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffYRegIn1Chisquare   =  fitRunDiffYGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffYRegIn1NDF         =  fitRunDiffYGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffYRegIn1Prob        =  fitRunDiffYGraphRegIn1->GetProb();


  TGraphErrors * runDiffYGraphRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffYRegRunOutVal,zeroRunOut,ediffYRegRunOutVal);
  runDiffYGraphRegOut1->SetName("runDiffYGraphRegOut1");
  runDiffYGraphRegOut1->SetMarkerColor(kBlack);
  runDiffYGraphRegOut1->SetLineColor(kBlack);
  runDiffYGraphRegOut1->SetMarkerStyle(21);
  runDiffYGraphRegOut1->SetMarkerSize(0.5);
  runDiffYGraphRegOut1->SetLineWidth(1);
//   runDiffYGraphRegOut1->Draw("AP");
  runDiffYGraphRegOut1->Fit("fitRunDiffYGraphRegOut1","E M R F 0 Q W");
  fitRunDiffYGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffYGraphRegOut1->SetLineWidth(2);
  fitRunDiffYGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYRegOut1           =  fitRunDiffYGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYRegOut1Error      =  fitRunDiffYGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffYRegOut1Chisquare  =  fitRunDiffYGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffYRegOut1NDF        =  fitRunDiffYGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffYRegOut1Prob       =  fitRunDiffYGraphRegOut1->GetProb();

  TMultiGraph *runDiffYGraphReg1 = new TMultiGraph();
  runDiffYGraphReg1->Add(runDiffYGraphRegIn1);
  runDiffYGraphReg1->Add(runDiffYGraphRegOut1);
  runDiffYGraphReg1->Draw("AP");
  runDiffYGraphReg1->SetTitle("");
  runDiffYGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYGraphReg1->GetYaxis()->SetTitle("Target Y Diff [mm]");
  runDiffYGraphReg1->GetXaxis()->CenterTitle();
  runDiffYGraphReg1->GetYaxis()->CenterTitle();
  runDiffYGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYGraphReg1->GetYaxis()->SetRangeUser(-0,0.0008);
  TAxis *xaxisRunDiffYGraphReg1= runDiffYGraphReg1->GetXaxis();
  xaxisRunDiffYGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYGraphRegIn1->Draw("same");
  fitRunDiffYGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffYRegIn1 =(TPaveStats*)runDiffYGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYRegOut1=(TPaveStats*)runDiffYGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYRegIn1->SetTextColor(kGreen);
  statsRunDiffYRegIn1->SetFillColor(kWhite); 
  statsRunDiffYRegOut1->SetTextColor(kBlack);
  statsRunDiffYRegOut1->SetFillColor(kWhite); 
  statsRunDiffYRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYRegInpOut1       =  (fitResultRunDiffYRegIn1+fitResultRunDiffYRegOut1)/2;
  Double_t resultRunDiffYRegInmOut1       =  (fitResultRunDiffYRegIn1-fitResultRunDiffYRegOut1)/2;
  Double_t resultRunDiffYRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYRegIn1Error*fitResultRunDiffYRegIn1Error+fitResultRunDiffYRegOut1Error*fitResultRunDiffYRegOut1Error)/2;
  Double_t resultRunDiffYRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYRegIn1Error*fitResultRunDiffYRegIn1Error+fitResultRunDiffYRegOut1Error*fitResultRunDiffYRegOut1Error)/2;


  TLegend *legRunDiffYReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYReg1->AddEntry("runDiffYGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffYRegIn1,fitResultRunDiffYRegIn1Error),"lp");
  legRunDiffYReg1->AddEntry("runDiffYGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffYRegOut1,fitResultRunDiffYRegOut1Error),"lp");
//   legRunDiffYReg1->AddEntry("runDiffYGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYRegInpOut1,resultRunDiffYRegInpOut1Error),"");
//   legRunDiffYReg1->AddEntry("runDiffYGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYRegInmOut1,resultRunDiffYRegInmOut1Error),"");
  legRunDiffYReg1->SetTextSize(0.045);
  legRunDiffYReg1->SetFillColor(0);
  legRunDiffYReg1->SetBorderSize(2);
  legRunDiffYReg1->Draw();

  gPad->Update();


  pad71->cd(4);

  TGraphErrors * runDiffYSlopeGraphRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffYSlopeRegRunInVal,zeroRunIn,ediffYSlopeRegRunInVal);
  runDiffYSlopeGraphRegIn1->SetName("runDiffYSlopeGraphRegIn1");
  runDiffYSlopeGraphRegIn1->SetMarkerColor(kGreen);
  runDiffYSlopeGraphRegIn1->SetLineColor(kGreen);
  runDiffYSlopeGraphRegIn1->SetMarkerStyle(22);
  runDiffYSlopeGraphRegIn1->SetMarkerSize(0.5);
  runDiffYSlopeGraphRegIn1->SetLineWidth(1);
//   runDiffYSlopeGraphRegIn1->Draw("AP");
  runDiffYSlopeGraphRegIn1->Fit("fitRunDiffYSlopeGraphRegIn1","E M R F 0 Q W");
  fitRunDiffYSlopeGraphRegIn1->SetLineColor(kGreen);
  fitRunDiffYSlopeGraphRegIn1->SetLineWidth(2);
  fitRunDiffYSlopeGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeRegIn1            =  fitRunDiffYSlopeGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeRegIn1Error       =  fitRunDiffYSlopeGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffYSlopeRegIn1Chisquare   =  fitRunDiffYSlopeGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffYSlopeRegIn1NDF         =  fitRunDiffYSlopeGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffYSlopeRegIn1Prob        =  fitRunDiffYSlopeGraphRegIn1->GetProb();


  TGraphErrors * runDiffYSlopeGraphRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffYSlopeRegRunOutVal,zeroRunOut,ediffYSlopeRegRunOutVal);
  runDiffYSlopeGraphRegOut1->SetName("runDiffYSlopeGraphRegOut1");
  runDiffYSlopeGraphRegOut1->SetMarkerColor(kBlack);
  runDiffYSlopeGraphRegOut1->SetLineColor(kBlack);
  runDiffYSlopeGraphRegOut1->SetMarkerStyle(22);
  runDiffYSlopeGraphRegOut1->SetMarkerSize(0.5);
  runDiffYSlopeGraphRegOut1->SetLineWidth(1);
//   runDiffYSlopeGraphRegOut1->Draw("AP");
  runDiffYSlopeGraphRegOut1->Fit("fitRunDiffYSlopeGraphRegOut1","E M R F 0 Q W");
  fitRunDiffYSlopeGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffYSlopeGraphRegOut1->SetLineWidth(2);
  fitRunDiffYSlopeGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffYSlopeRegOut1           =  fitRunDiffYSlopeGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffYSlopeRegOut1Error      =  fitRunDiffYSlopeGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffYSlopeRegOut1Chisquare  =  fitRunDiffYSlopeGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffYSlopeRegOut1NDF        =  fitRunDiffYSlopeGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffYSlopeRegOut1Prob       =  fitRunDiffYSlopeGraphRegOut1->GetProb();

  TMultiGraph *runDiffYSlopeGraphReg1 = new TMultiGraph();
  runDiffYSlopeGraphReg1->Add(runDiffYSlopeGraphRegIn1);
  runDiffYSlopeGraphReg1->Add(runDiffYSlopeGraphRegOut1);
  runDiffYSlopeGraphReg1->Draw("AP");
  runDiffYSlopeGraphReg1->SetTitle("");
  runDiffYSlopeGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffYSlopeGraphReg1->GetYaxis()->SetTitle("Target Y Slope Diff [#murad]");
  runDiffYSlopeGraphReg1->GetXaxis()->CenterTitle();
  runDiffYSlopeGraphReg1->GetYaxis()->CenterTitle();
  runDiffYSlopeGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffYSlopeGraphReg1->GetYaxis()->SetRangeUser(-0.0,0.03);
  TAxis *xaxisRunDiffYSlopeGraphReg1= runDiffYSlopeGraphReg1->GetXaxis();
  xaxisRunDiffYSlopeGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffYSlopeGraphRegIn1->Draw("same");
  fitRunDiffYSlopeGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffYSlopeRegIn1 =(TPaveStats*)runDiffYSlopeGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffYSlopeRegOut1=(TPaveStats*)runDiffYSlopeGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffYSlopeRegIn1->SetTextColor(kGreen);
  statsRunDiffYSlopeRegIn1->SetFillColor(kWhite); 
  statsRunDiffYSlopeRegOut1->SetTextColor(kBlack);
  statsRunDiffYSlopeRegOut1->SetFillColor(kWhite); 
  statsRunDiffYSlopeRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffYSlopeRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffYSlopeRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffYSlopeRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffYSlopeRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffYSlopeRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffYSlopeRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffYSlopeRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffYSlopeRegInpOut1       =  (fitResultRunDiffYSlopeRegIn1+fitResultRunDiffYSlopeRegOut1)/2;
  Double_t resultRunDiffYSlopeRegInmOut1       =  (fitResultRunDiffYSlopeRegIn1-fitResultRunDiffYSlopeRegOut1)/2;
  Double_t resultRunDiffYSlopeRegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeRegIn1Error*fitResultRunDiffYSlopeRegIn1Error+fitResultRunDiffYSlopeRegOut1Error*fitResultRunDiffYSlopeRegOut1Error)/2;
  Double_t resultRunDiffYSlopeRegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffYSlopeRegIn1Error*fitResultRunDiffYSlopeRegIn1Error+fitResultRunDiffYSlopeRegOut1Error*fitResultRunDiffYSlopeRegOut1Error)/2;


  TLegend *legRunDiffYSlopeReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeRegIn1,fitResultRunDiffYSlopeRegIn1Error),"lp");
  legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffYSlopeRegOut1,fitResultRunDiffYSlopeRegOut1Error),"lp");
//   legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYSlopeRegInpOut1,resultRunDiffYSlopeRegInpOut1Error),"");
//   legRunDiffYSlopeReg1->AddEntry("runDiffYSlopeGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffYSlopeRegInmOut1,resultRunDiffYSlopeRegInmOut1Error),"");
  legRunDiffYSlopeReg1->SetTextSize(0.045);
  legRunDiffYSlopeReg1->SetFillColor(0);
  legRunDiffYSlopeReg1->SetBorderSize(2);
  legRunDiffYSlopeReg1->Draw();

  gPad->Update();


  pad71->cd(5);

  TGraphErrors * runDiffEGraphRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,diffERegRunInVal,zeroRunIn,ediffERegRunInVal);
  runDiffEGraphRegIn1->SetName("runDiffEGraphRegIn1");
  runDiffEGraphRegIn1->SetMarkerColor(kBlue);
  runDiffEGraphRegIn1->SetLineColor(kBlue);
  runDiffEGraphRegIn1->SetMarkerStyle(20);
  runDiffEGraphRegIn1->SetMarkerSize(0.5);
  runDiffEGraphRegIn1->SetLineWidth(1);
//   runDiffEGraphRegIn1->Draw("AP");
  runDiffEGraphRegIn1->Fit("fitRunDiffEGraphRegIn1","E M R F 0 Q W");
  fitRunDiffEGraphRegIn1->SetLineColor(kBlue);
  fitRunDiffEGraphRegIn1->SetLineWidth(2);
  fitRunDiffEGraphRegIn1->SetLineStyle(3);

  Double_t fitResultRunDiffERegIn1            =  fitRunDiffEGraphRegIn1->GetParameter(0);
  Double_t fitResultRunDiffERegIn1Error       =  fitRunDiffEGraphRegIn1->GetParError(0);
  Double_t fitResultRunDiffERegIn1Chisquare   =  fitRunDiffEGraphRegIn1->GetChisquare();
  Double_t fitResultRunDiffERegIn1NDF         =  fitRunDiffEGraphRegIn1->GetNDF();
  Double_t fitResultRunDiffERegIn1Prob        =  fitRunDiffEGraphRegIn1->GetProb();


  TGraphErrors * runDiffEGraphRegOut1 = new TGraphErrors(counterRunOut,runNumberRunOut,diffERegRunOutVal,zeroRunOut,ediffERegRunOutVal);
  runDiffEGraphRegOut1->SetName("runDiffEGraphRegOut1");
  runDiffEGraphRegOut1->SetMarkerColor(kBlack);
  runDiffEGraphRegOut1->SetLineColor(kBlack);
  runDiffEGraphRegOut1->SetMarkerStyle(20);
  runDiffEGraphRegOut1->SetMarkerSize(0.5);
  runDiffEGraphRegOut1->SetLineWidth(1);
//   runDiffEGraphRegOut1->Draw("AP");
  runDiffEGraphRegOut1->Fit("fitRunDiffEGraphRegOut1","E M R F 0 Q W");
  fitRunDiffEGraphRegOut1->SetLineColor(kBlack);
  fitRunDiffEGraphRegOut1->SetLineWidth(2);
  fitRunDiffEGraphRegOut1->SetLineStyle(3);

  Double_t fitResultRunDiffERegOut1           =  fitRunDiffEGraphRegOut1->GetParameter(0);
  Double_t fitResultRunDiffERegOut1Error      =  fitRunDiffEGraphRegOut1->GetParError(0);
  Double_t fitResultRunDiffERegOut1Chisquare  =  fitRunDiffEGraphRegOut1->GetChisquare();
  Double_t fitResultRunDiffERegOut1NDF        =  fitRunDiffEGraphRegOut1->GetNDF();
  Double_t fitResultRunDiffERegOut1Prob       =  fitRunDiffEGraphRegOut1->GetProb();

  TMultiGraph *runDiffEGraphReg1 = new TMultiGraph();
  runDiffEGraphReg1->Add(runDiffEGraphRegIn1);
  runDiffEGraphReg1->Add(runDiffEGraphRegOut1);
  runDiffEGraphReg1->Draw("AP");
  runDiffEGraphReg1->SetTitle("");
  runDiffEGraphReg1->GetXaxis()->SetTitle("Run Number");
  runDiffEGraphReg1->GetYaxis()->SetTitle("Energy Diff [ppm]");
  runDiffEGraphReg1->GetXaxis()->CenterTitle();
  runDiffEGraphReg1->GetYaxis()->CenterTitle();
  runDiffEGraphReg1->GetXaxis()->SetNdivisions(5,2,0);
  runDiffEGraphReg1->GetYaxis()->SetRangeUser(-0.000,0.00000018);
  TAxis *xaxisRunDiffEGraphReg1= runDiffEGraphReg1->GetXaxis();
  xaxisRunDiffEGraphReg1->SetLimits(run_range[0],run_range[1]);
  fitRunDiffEGraphRegIn1->Draw("same");
  fitRunDiffEGraphRegOut1->Draw("same");

  TPaveStats *statsRunDiffERegIn1 =(TPaveStats*)runDiffEGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunDiffERegOut1=(TPaveStats*)runDiffEGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunDiffERegIn1->SetTextColor(kBlue);
  statsRunDiffERegIn1->SetFillColor(kWhite); 
  statsRunDiffERegOut1->SetTextColor(kBlack);
  statsRunDiffERegOut1->SetFillColor(kWhite); 
  statsRunDiffERegIn1->SetX1NDC(x_lo_stat_in2);    statsRunDiffERegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunDiffERegIn1->SetY1NDC(y_lo_stat_in2);    statsRunDiffERegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunDiffERegOut1->SetX1NDC(x_lo_stat_out2);  statsRunDiffERegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunDiffERegOut1->SetY1NDC(y_lo_stat_out2);  statsRunDiffERegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunDiffERegInpOut1       =  (fitResultRunDiffERegIn1+fitResultRunDiffERegOut1)/2;
  Double_t resultRunDiffERegInmOut1       =  (fitResultRunDiffERegIn1-fitResultRunDiffERegOut1)/2;
  Double_t resultRunDiffERegInpOut1Error  =  TMath::Sqrt(fitResultRunDiffERegIn1Error*fitResultRunDiffERegIn1Error+fitResultRunDiffERegOut1Error*fitResultRunDiffERegOut1Error)/2;
  Double_t resultRunDiffERegInmOut1Error  =  TMath::Sqrt(fitResultRunDiffERegIn1Error*fitResultRunDiffERegIn1Error+fitResultRunDiffERegOut1Error*fitResultRunDiffERegOut1Error)/2;


  TLegend *legRunDiffEReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunDiffEReg1->AddEntry("runDiffEGraphRegIn1",  Form("In Reg=%4.3g#pm%4.3g",fitResultRunDiffERegIn1,fitResultRunDiffERegIn1Error),"lp");
  legRunDiffEReg1->AddEntry("runDiffEGraphRegOut1",  Form("Out Reg=%4.3g#pm%4.3g",fitResultRunDiffERegOut1,fitResultRunDiffERegOut1Error),"lp");
//   legRunDiffEReg1->AddEntry("runDiffEGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffERegInpOut1,resultRunDiffERegInpOut1Error),"");
//   legRunDiffEReg1->AddEntry("runDiffEGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.3g#pm%4.3g",resultRunDiffERegInmOut1,resultRunDiffERegInmOut1Error),"");
  legRunDiffEReg1->SetTextSize(0.045);
  legRunDiffEReg1->SetFillColor(0);
  legRunDiffEReg1->SetBorderSize(2);
  legRunDiffEReg1->Draw();

  gPad->Update();

  pad71->cd(6);

  TGraphErrors * runChargeAsymGraphUnRegIn1 = new TGraphErrors(counterRunIn,runNumberRunIn,chargeRunInVal,zeroRunIn,echargeRunInVal);
  runChargeAsymGraphUnRegIn1->SetName("runChargeAsymGraphUnRegIn1");
  runChargeAsymGraphUnRegIn1->SetMarkerColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  runChargeAsymGraphUnRegIn1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegIn1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegIn1->SetLineWidth(1);
//   runChargeAsymGraphUnRegIn1->Draw("AP");
  runChargeAsymGraphUnRegIn1->Fit("fitRunChargeAsymGraphUnRegIn1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegIn1->SetLineColor(kMagenta);
  fitRunChargeAsymGraphUnRegIn1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegIn1            =  fitRunChargeAsymGraphUnRegIn1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegIn1Error       =  fitRunChargeAsymGraphUnRegIn1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegIn1Chisquare   =  fitRunChargeAsymGraphUnRegIn1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegIn1NDF         =  fitRunChargeAsymGraphUnRegIn1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegIn1Prob        =  fitRunChargeAsymGraphUnRegIn1->GetProb();


  TGraphErrors * runChargeAsymGraphUnRegOut1 = new TGraphErrors(counterRegOut,runNumberRunOut,chargeRunOutVal,zeroRunOut,echargeRunOutVal);
  runChargeAsymGraphUnRegOut1->SetName("runChargeAsymGraphUnRegOut1");
  runChargeAsymGraphUnRegOut1->SetMarkerColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  runChargeAsymGraphUnRegOut1->SetMarkerStyle(30);
  runChargeAsymGraphUnRegOut1->SetMarkerSize(0.7);
  runChargeAsymGraphUnRegOut1->SetLineWidth(1);
//   runChargeAsymGraphUnRegOut1->Draw("AP");
  runChargeAsymGraphUnRegOut1->Fit("fitRunChargeAsymGraphUnRegOut1","E M R F 0 Q W");
  fitRunChargeAsymGraphUnRegOut1->SetLineColor(kBlack);
  fitRunChargeAsymGraphUnRegOut1->SetLineWidth(2);
  fitRunChargeAsymGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultRunChargeAsymUnRegOut1           =  fitRunChargeAsymGraphUnRegOut1->GetParameter(0);
  Double_t fitResultRunChargeAsymUnRegOut1Error      =  fitRunChargeAsymGraphUnRegOut1->GetParError(0);
  Double_t fitResultRunChargeAsymUnRegOut1Chisquare  =  fitRunChargeAsymGraphUnRegOut1->GetChisquare();
  Double_t fitResultRunChargeAsymUnRegOut1NDF        =  fitRunChargeAsymGraphUnRegOut1->GetNDF();
  Double_t fitResultRunChargeAsymUnRegOut1Prob       =  fitRunChargeAsymGraphUnRegOut1->GetProb();

  TMultiGraph *runChargeAsymGraphUnReg1 = new TMultiGraph();
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegIn1);
  runChargeAsymGraphUnReg1->Add(runChargeAsymGraphUnRegOut1);
  runChargeAsymGraphUnReg1->Draw("AP");
  runChargeAsymGraphUnReg1->SetTitle("");
  runChargeAsymGraphUnReg1->GetXaxis()->SetTitle("Run Number");
  runChargeAsymGraphUnReg1->GetYaxis()->SetTitle("Charge Asym [ppm]");
  runChargeAsymGraphUnReg1->GetXaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetYaxis()->CenterTitle();
  runChargeAsymGraphUnReg1->GetXaxis()->SetNdivisions(5,2,0);
  runChargeAsymGraphUnReg1->GetYaxis()->SetRangeUser(-18,18);
  TAxis *xaxisRunChargeAsymGraphUnReg1= runChargeAsymGraphUnReg1->GetXaxis();
  xaxisRunChargeAsymGraphUnReg1->SetLimits(run_range[0],run_range[1]);
  fitRunChargeAsymGraphUnRegIn1->Draw("same");
  fitRunChargeAsymGraphUnRegOut1->Draw("same");

  TPaveStats *statsRunChargeAsymUnRegIn1 =(TPaveStats*)runChargeAsymGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsRunChargeAsymUnRegOut1=(TPaveStats*)runChargeAsymGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsRunChargeAsymUnRegIn1->SetTextColor(kMagenta);
  statsRunChargeAsymUnRegIn1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegOut1->SetTextColor(kBlack);
  statsRunChargeAsymUnRegOut1->SetFillColor(kWhite); 
  statsRunChargeAsymUnRegIn1->SetX1NDC(x_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetX2NDC(x_hi_stat_in2); 
  statsRunChargeAsymUnRegIn1->SetY1NDC(y_lo_stat_in2);    statsRunChargeAsymUnRegIn1->SetY2NDC(y_hi_stat_in2);
  statsRunChargeAsymUnRegOut1->SetX1NDC(x_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetX2NDC(x_hi_stat_out2); 
  statsRunChargeAsymUnRegOut1->SetY1NDC(y_lo_stat_out2);  statsRunChargeAsymUnRegOut1->SetY2NDC(y_hi_stat_out2);

  Double_t resultRunChargeAsymUnRegInpOut1       =  (fitResultRunChargeAsymUnRegIn1+fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInmOut1       =  (fitResultRunChargeAsymUnRegIn1-fitResultRunChargeAsymUnRegOut1)/2;
  Double_t resultRunChargeAsymUnRegInpOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;
  Double_t resultRunChargeAsymUnRegInmOut1Error  =  TMath::Sqrt(fitResultRunChargeAsymUnRegIn1Error*fitResultRunChargeAsymUnRegIn1Error+fitResultRunChargeAsymUnRegOut1Error*fitResultRunChargeAsymUnRegOut1Error)/2;


  TLegend *legRunChargeAsymUnReg1 = new TLegend(x_lo_leg2,y_lo_leg2,x_hi_leg2,y_hi_leg2);
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegIn1,fitResultRunChargeAsymUnRegIn1Error),"lp");
  legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultRunChargeAsymUnRegOut1,fitResultRunChargeAsymUnRegOut1Error),"lp");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInpOut1,resultRunChargeAsymUnRegInpOut1Error),"");
//   legRunChargeAsymUnReg1->AddEntry("runChargeAsymGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultRunChargeAsymUnRegInmOut1,resultRunChargeAsymUnRegInmOut1Error),"");
  legRunChargeAsymUnReg1->SetTextSize(0.030);
  legRunChargeAsymUnReg1->SetFillColor(0);
  legRunChargeAsymUnReg1->SetBorderSize(2);
  legRunChargeAsymUnReg1->Draw();

  gPad->Update();


  c7->Update();
  c7->SaveAs(Form("%s/plots/%s/TargetDiffRegRunbyRun%s.png",n2deltaAnalysisDir.Data(),regScheme,regScheme));


  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/








  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/

  if(OCTANT_PLOT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.5);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TCanvas *c1 = new TCanvas("c1","c1",cx1,cy1,1200,950);
  TPad * pad10 = new TPad("pad10","pad10",ps1,ps2,ps4,ps4);
  TPad * pad11 = new TPad("pad11","pad11",ps1,ps1,ps4,ps3);
  pad10->Draw();
  pad11->Draw();
  pad10->cd();
  TText * ct10 = new TText(tll,tlr,Form("%sN to Delta %s %s Octant Aymmetry",regTitle,dataInfo,plotTitle));
  ct10->SetTextSize(tsiz);
  ct10->Draw();
  pad11->cd();
  pad11->Divide(1,3);


  pad11->cd(1);

  TGraphErrors * detGraphUnRegIn1 = new TGraphErrors(counterOctantIn,octantNumberIn,mdAsymUnRegOctantInVal,zeroOctantIn,emdAsymUnRegOctantInVal);
  detGraphUnRegIn1->SetName("detGraphUnRegIn1");
  detGraphUnRegIn1->SetMarkerColor(kRed);
  detGraphUnRegIn1->SetLineColor(kRed);
  detGraphUnRegIn1->SetMarkerStyle(25);
  detGraphUnRegIn1->SetMarkerSize(0.6);
  detGraphUnRegIn1->SetLineWidth(1);
  detGraphUnRegIn1->Fit("fitDetGraphUnRegIn1","E M R F 0 Q");
  fitDetGraphUnRegIn1->SetLineColor(kRed);
  fitDetGraphUnRegIn1->SetLineWidth(2);
  fitDetGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultDetUnRegIn1            =  fitDetGraphUnRegIn1->GetParameter(0);
  Double_t fitResultDetUnRegIn1Error       =  fitDetGraphUnRegIn1->GetParError(0);
  Double_t fitResultDetUnRegIn1Chisquare   =  fitDetGraphUnRegIn1->GetChisquare();
  Double_t fitResultDetUnRegIn1NDF         =  fitDetGraphUnRegIn1->GetNDF();
  Double_t fitResultDetUnRegIn1Prob        =  fitDetGraphUnRegIn1->GetProb();


  TGraphErrors * detGraphUnRegOut1 = new TGraphErrors(counterOctantOut,octantNumberOut,mdAsymUnRegOctantOutVal,zeroOctantOut,emdAsymUnRegOctantOutVal);
  detGraphUnRegOut1->SetName("detGraphUnRegOut1");
  detGraphUnRegOut1->SetMarkerColor(kBlack);
  detGraphUnRegOut1->SetLineColor(kBlack);
  detGraphUnRegOut1->SetMarkerStyle(25);
  detGraphUnRegOut1->SetMarkerSize(0.6);
  detGraphUnRegOut1->SetLineWidth(1);
  detGraphUnRegOut1->Fit("fitDetGraphUnRegOut1","E M R F 0 Q");
  fitDetGraphUnRegOut1->SetLineColor(kBlack);
  fitDetGraphUnRegOut1->SetLineWidth(2);
  fitDetGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultDetUnRegOut1           =  fitDetGraphUnRegOut1->GetParameter(0);
  Double_t fitResultDetUnRegOut1Error      =  fitDetGraphUnRegOut1->GetParError(0);
  Double_t fitResultDetUnRegOut1Chisquare  =  fitDetGraphUnRegOut1->GetChisquare();
  Double_t fitResultDetUnRegOut1NDF        =  fitDetGraphUnRegOut1->GetNDF();
  Double_t fitResultDetUnRegOut1Prob       =  fitDetGraphUnRegOut1->GetProb();

  TMultiGraph *detGraphUnReg1 = new TMultiGraph();
  detGraphUnReg1->Add(detGraphUnRegIn1);
  detGraphUnReg1->Add(detGraphUnRegOut1);
  detGraphUnReg1->Draw("AP");
  detGraphUnReg1->SetTitle("");
  detGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  detGraphUnReg1->GetYaxis()->SetTitle(Form("%s Asym UnReg [ppm]",plotTitle));
  detGraphUnReg1->GetXaxis()->CenterTitle();
  detGraphUnReg1->GetYaxis()->CenterTitle();
  detGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  detGraphUnReg1->GetYaxis()->SetRangeUser(-8,8);
  TAxis *xaxisDetGraphUnReg1= detGraphUnReg1->GetXaxis();
  xaxisDetGraphUnReg1->SetLimits(det_range[0],det_range[1]);
  fitDetGraphUnRegIn1->Draw("same");
  fitDetGraphUnRegOut1->Draw("same");

  TPaveStats *statsDetUnRegIn1 =(TPaveStats*)detGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDetUnRegOut1=(TPaveStats*)detGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsDetUnRegIn1->SetTextColor(kRed);
  statsDetUnRegIn1->SetFillColor(kWhite); 
  statsDetUnRegOut1->SetTextColor(kBlack);
  statsDetUnRegOut1->SetFillColor(kWhite); 
  statsDetUnRegIn1->SetX1NDC(x_lo_stat_in);    statsDetUnRegIn1->SetX2NDC(x_hi_stat_in); 
  statsDetUnRegIn1->SetY1NDC(y_lo_stat_in);    statsDetUnRegIn1->SetY2NDC(y_hi_stat_in);
  statsDetUnRegOut1->SetX1NDC(x_lo_stat_out);  statsDetUnRegOut1->SetX2NDC(x_hi_stat_out); 
  statsDetUnRegOut1->SetY1NDC(y_lo_stat_out);  statsDetUnRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultDetUnRegInpOut1       =  (fitResultDetUnRegIn1+fitResultDetUnRegOut1)/2;
//   Double_t resultDetUnRegInmOut1       =  (fitResultDetUnRegIn1-fitResultDetUnRegOut1)/2;
  Double_t resultDetUnRegInmOut1       =  (((1/(pow(fitResultDetUnRegIn1Error,2)))*(fitResultDetUnRegIn1))-((1/(pow(fitResultDetUnRegOut1Error,2)))*(fitResultDetUnRegOut1)))/(1/(pow(fitResultDetUnRegIn1Error,2))+1/(pow(fitResultDetUnRegOut1Error,2)));
  Double_t resultDetUnRegInpOut1Error  =  TMath::Sqrt(pow(fitResultDetUnRegIn1Error,2)+pow(fitResultDetUnRegOut1Error,2))/2;
//   Double_t resultDetUnRegInmOut1Error  =  TMath::Sqrt(fitResultDetUnRegIn1Error*fitResultDetUnRegIn1Error+fitResultDetUnRegOut1Error*fitResultDetUnRegOut1Error)/2;
  Double_t resultDetUnRegInmOut1Error  = 1/(TMath::Sqrt(1/(pow(fitResultDetUnRegIn1Error,2))+1/(pow(fitResultDetUnRegOut1Error,2))));

  TLegend *legDetUnReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legDetUnReg1->AddEntry("detGraphUnRegIn1",  Form("In UnReg=%4.2f#pm%4.2f",fitResultDetUnRegIn1,fitResultDetUnRegIn1Error),"lp");
  legDetUnReg1->AddEntry("detGraphUnRegOut1",  Form("Out UnReg=%4.2f#pm%4.2f",fitResultDetUnRegOut1,fitResultDetUnRegOut1Error),"lp");
  legDetUnReg1->AddEntry("detGraphUnRegInpOut1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",resultDetUnRegInpOut1,resultDetUnRegInpOut1Error),"");
  legDetUnReg1->AddEntry("detGraphUnRegInmOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",resultDetUnRegInmOut1,resultDetUnRegInmOut1Error),"");
  legDetUnReg1->SetTextSize(0.045);
  legDetUnReg1->SetFillColor(0);
  legDetUnReg1->SetBorderSize(2);
  legDetUnReg1->Draw();

  TLatex* toctantGraphUnRegIn1=new TLatex(0.8,6.0,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetUnRegIn1Chisquare/fitResultDetUnRegIn1NDF,fitResultDetUnRegIn1Prob));
  toctantGraphUnRegIn1->SetTextSize(0.068);
  toctantGraphUnRegIn1->SetTextColor(kRed);
  TLatex* toctantGraphUnRegOut1=new TLatex(0.8,4.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetUnRegOut1Chisquare/fitResultDetUnRegOut1NDF,fitResultDetUnRegOut1Prob));
  toctantGraphUnRegOut1->SetTextSize(0.068);
  toctantGraphUnRegOut1->SetTextColor(kBlack);
  toctantGraphUnRegIn1->Draw();
  toctantGraphUnRegOut1->Draw();

  gPad->Update();


  pad11->cd(2);

  TGraphErrors * detGraphRegIn1 = new TGraphErrors(counterOctantIn,octantNumberIn,mdAsymRegOctantInVal,zeroOctantIn,emdAsymRegOctantInVal);
  detGraphRegIn1->SetName("detGraphRegIn1");
  detGraphRegIn1->SetMarkerColor(kRed);
  detGraphRegIn1->SetLineColor(kRed);
  detGraphRegIn1->SetMarkerStyle(21);
  detGraphRegIn1->SetMarkerSize(0.6);
  detGraphRegIn1->SetLineWidth(1);
  detGraphRegIn1->Fit("fitDetGraphRegIn1","E M R F 0 Q");
  fitDetGraphRegIn1->SetLineColor(kRed);
  fitDetGraphRegIn1->SetLineWidth(2);
  fitDetGraphRegIn1->SetLineStyle(1);

  Double_t fitResultDetRegIn1            =  fitDetGraphRegIn1->GetParameter(0);
  Double_t fitResultDetRegIn1Error       =  fitDetGraphRegIn1->GetParError(0);
  Double_t fitResultDetRegIn1Chisquare   =  fitDetGraphRegIn1->GetChisquare();
  Double_t fitResultDetRegIn1NDF         =  fitDetGraphRegIn1->GetNDF();
  Double_t fitResultDetRegIn1Prob        =  fitDetGraphRegIn1->GetProb();


  TGraphErrors * detGraphRegOut1 = new TGraphErrors(counterOctantOut,octantNumberOut,mdAsymRegOctantOutVal,zeroOctantOut,emdAsymRegOctantOutVal);
  detGraphRegOut1->SetName("detGraphRegOut1");
  detGraphRegOut1->SetMarkerColor(kBlack);
  detGraphRegOut1->SetLineColor(kBlack);
  detGraphRegOut1->SetMarkerStyle(21);
  detGraphRegOut1->SetMarkerSize(0.6);
  detGraphRegOut1->SetLineWidth(1);
  detGraphRegOut1->Fit("fitDetGraphRegOut1","E M R F 0 Q");
  fitDetGraphRegOut1->SetLineColor(kBlack);
  fitDetGraphRegOut1->SetLineWidth(2);
  fitDetGraphRegOut1->SetLineStyle(1);

  Double_t fitResultDetRegOut1           =  fitDetGraphRegOut1->GetParameter(0);
  Double_t fitResultDetRegOut1Error      =  fitDetGraphRegOut1->GetParError(0);
  Double_t fitResultDetRegOut1Chisquare  =  fitDetGraphRegOut1->GetChisquare();
  Double_t fitResultDetRegOut1NDF        =  fitDetGraphRegOut1->GetNDF();
  Double_t fitResultDetRegOut1Prob       =  fitDetGraphRegOut1->GetProb();

  TMultiGraph *detGraphReg1 = new TMultiGraph();
  detGraphReg1->Add(detGraphRegIn1);
  detGraphReg1->Add(detGraphRegOut1);
  detGraphReg1->Draw("AP");
  detGraphReg1->SetTitle("");
  detGraphReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  detGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  detGraphReg1->GetXaxis()->CenterTitle();
  detGraphReg1->GetYaxis()->CenterTitle();
  detGraphReg1->GetXaxis()->SetNdivisions(8,0,0);
  detGraphReg1->GetYaxis()->SetRangeUser(-8,8);
  TAxis *xaxisDetGraphReg1= detGraphReg1->GetXaxis();
  xaxisDetGraphReg1->SetLimits(det_range[0],det_range[1]);
  fitDetGraphRegIn1->Draw("same");
  fitDetGraphRegOut1->Draw("same");

  TPaveStats *statsDetRegIn1 =(TPaveStats*)detGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDetRegOut1=(TPaveStats*)detGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsDetRegIn1->SetTextColor(kRed);
  statsDetRegIn1->SetFillColor(kWhite); 
  statsDetRegOut1->SetTextColor(kBlack);
  statsDetRegOut1->SetFillColor(kWhite); 
  statsDetRegIn1->SetX1NDC(x_lo_stat_in);    statsDetRegIn1->SetX2NDC(x_hi_stat_in); 
  statsDetRegIn1->SetY1NDC(y_lo_stat_in);    statsDetRegIn1->SetY2NDC(y_hi_stat_in);
  statsDetRegOut1->SetX1NDC(x_lo_stat_out);  statsDetRegOut1->SetX2NDC(x_hi_stat_out); 
  statsDetRegOut1->SetY1NDC(y_lo_stat_out);  statsDetRegOut1->SetY2NDC(y_hi_stat_out);

//   Double_t resultDetRegInpOut1       =  (fitResultDetRegIn1+fitResultDetRegOut1)/2;
//   Double_t resultDetRegInmOut1       =  (fitResultDetRegIn1-fitResultDetRegOut1)/2;
//   Double_t resultDetRegInpOut1Error  =  TMath::Sqrt(pow(fitResultDetRegIn1Error,2)+pow(fitResultDetRegOut1Error,2))/2;
//   Double_t resultDetRegInmOut1Error  =  TMath::Sqrt(fitResultDetRegIn1Error*fitResultDetRegIn1Error+fitResultDetRegOut1Error*fitResultDetRegOut1Error)/2;

  Double_t resultDetRegInpOut1       =  (fitResultDetRegIn1+fitResultDetRegOut1)/2;
  Double_t resultDetRegInmOut1       =  (((1/(pow(fitResultDetRegIn1Error,2)))*(fitResultDetRegIn1))-((1/(pow(fitResultDetRegOut1Error,2)))*(fitResultDetRegOut1)))/(1/(pow(fitResultDetRegIn1Error,2))+1/(pow(fitResultDetRegOut1Error,2)));
  Double_t resultDetRegInpOut1Error  =  TMath::Sqrt(pow(fitResultDetRegIn1Error,2)+pow(fitResultDetRegOut1Error,2))/2;
  Double_t resultDetRegInmOut1Error  = 1/(TMath::Sqrt(1/(pow(fitResultDetRegIn1Error,2))+1/(pow(fitResultDetRegOut1Error,2))));



  TLegend *legDetReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legDetReg1->AddEntry("detGraphRegIn1",  Form("In Reg=%4.2f#pm%4.2f",fitResultDetRegIn1,fitResultDetRegIn1Error),"lp");
  legDetReg1->AddEntry("detGraphRegOut1",  Form("Out Reg=%4.2f#pm%4.2f",fitResultDetRegOut1,fitResultDetRegOut1Error),"lp");
  legDetReg1->AddEntry("detGraphRegInpOut1",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",resultDetRegInpOut1,resultDetRegInpOut1Error),"");
  legDetReg1->AddEntry("detGraphRegInmOut1", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",resultDetRegInmOut1,resultDetRegInmOut1Error),"");
  legDetReg1->SetTextSize(0.045);
  legDetReg1->SetFillColor(0);
  legDetReg1->SetBorderSize(2);
  legDetReg1->Draw();

  TLatex* toctantGraphRegIn1=new TLatex(0.8,6.0,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetRegIn1Chisquare/fitResultDetRegIn1NDF,fitResultDetRegIn1Prob));
  toctantGraphRegIn1->SetTextSize(0.068);
  toctantGraphRegIn1->SetTextColor(kRed);
  TLatex* toctantGraphRegOut1=new TLatex(0.8,4.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetRegOut1Chisquare/fitResultDetRegOut1NDF,fitResultDetRegOut1Prob));
  toctantGraphRegOut1->SetTextSize(0.068);
  toctantGraphRegOut1->SetTextColor(kBlack);
  toctantGraphRegIn1->Draw();
  toctantGraphRegOut1->Draw();

  gPad->Update();


  pad11->cd(3);

  TGraphErrors * detGraphCorIn1 = new TGraphErrors(counterOctantIn,octantNumberIn,mdAsymCorOctantIn,zeroOctantIn,emdAsymCorOctantIn);
  detGraphCorIn1->SetName("detGraphCorIn1");
  detGraphCorIn1->SetMarkerColor(kOrange+2);
  detGraphCorIn1->SetLineColor(kOrange+2);
  detGraphCorIn1->SetMarkerStyle(21);
  detGraphCorIn1->SetMarkerSize(0.6);
  detGraphCorIn1->SetLineWidth(1);
  detGraphCorIn1->Fit("fitDetGraphCorIn1","E M R F 0 Q");
  fitDetGraphCorIn1->SetLineColor(kOrange+2);
  fitDetGraphCorIn1->SetLineWidth(2);
  fitDetGraphCorIn1->SetLineStyle(1);

  Double_t fitResultDetCorIn1            =  fitDetGraphCorIn1->GetParameter(0);
  Double_t fitResultDetCorIn1Error       =  fitDetGraphCorIn1->GetParError(0);
  Double_t fitResultDetCorIn1Chisquare   =  fitDetGraphCorIn1->GetChisquare();
  Double_t fitResultDetCorIn1NDF         =  fitDetGraphCorIn1->GetNDF();
  Double_t fitResultDetCorIn1Prob        =  fitDetGraphCorIn1->GetProb();


  TGraphErrors * detGraphCorOut1 = new TGraphErrors(counterOctantOut,octantNumberOut,mdAsymCorOctantOut,zeroOctantOut,emdAsymCorOctantOut);
  detGraphCorOut1->SetName("detGraphCorOut1");
  detGraphCorOut1->SetMarkerColor(kGray+2);
  detGraphCorOut1->SetLineColor(kGray+2);
  detGraphCorOut1->SetMarkerStyle(21);
  detGraphCorOut1->SetMarkerSize(0.6);
  detGraphCorOut1->SetLineWidth(1);
  detGraphCorOut1->Fit("fitDetGraphCorOut1","E M R F 0 Q");
  fitDetGraphCorOut1->SetLineColor(kGray+2);
  fitDetGraphCorOut1->SetLineWidth(2);
  fitDetGraphCorOut1->SetLineStyle(1);

  Double_t fitResultDetCorOut1           =  fitDetGraphCorOut1->GetParameter(0);
  Double_t fitResultDetCorOut1Error      =  fitDetGraphCorOut1->GetParError(0);
  Double_t fitResultDetCorOut1Chisquare  =  fitDetGraphCorOut1->GetChisquare();
  Double_t fitResultDetCorOut1NDF        =  fitDetGraphCorOut1->GetNDF();
  Double_t fitResultDetCorOut1Prob       =  fitDetGraphCorOut1->GetProb();

  TMultiGraph *detGraphCor1 = new TMultiGraph();
  detGraphCor1->Add(detGraphCorIn1);
  detGraphCor1->Add(detGraphCorOut1);
  detGraphCor1->Draw("AP");
  detGraphCor1->SetTitle("");
  detGraphCor1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  detGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Cor [ppm]",plotTitle));
  detGraphCor1->GetXaxis()->CenterTitle();
  detGraphCor1->GetYaxis()->CenterTitle();
  detGraphCor1->GetXaxis()->SetNdivisions(8,0,0);
  detGraphCor1->GetYaxis()->SetRangeUser(-8,8);
  TAxis *xaxisDetGraphCor1= detGraphCor1->GetXaxis();
  xaxisDetGraphCor1->SetLimits(det_range[0],det_range[1]);
  fitDetGraphCorIn1->Draw("same");
  fitDetGraphCorOut1->Draw("same");

  TPaveStats *statsDetCorIn1 =(TPaveStats*)detGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDetCorOut1=(TPaveStats*)detGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsDetCorIn1->SetTextColor(kOrange+2);
  statsDetCorIn1->SetFillColor(kWhite); 
  statsDetCorOut1->SetTextColor(kGray+2);
  statsDetCorOut1->SetFillColor(kWhite); 
  statsDetCorIn1->SetX1NDC(x_lo_stat_in);    statsDetCorIn1->SetX2NDC(x_hi_stat_in); 
  statsDetCorIn1->SetY1NDC(y_lo_stat_in);    statsDetCorIn1->SetY2NDC(y_hi_stat_in);
  statsDetCorOut1->SetX1NDC(x_lo_stat_out);  statsDetCorOut1->SetX2NDC(x_hi_stat_out); 
  statsDetCorOut1->SetY1NDC(y_lo_stat_out);  statsDetCorOut1->SetY2NDC(y_hi_stat_out);

//   Double_t resultDetCorInpOut1       =  (fitResultDetCorIn1+fitResultDetCorOut1)/2;
//   Double_t resultDetCorInmOut1       =  (fitResultDetCorIn1-fitResultDetCorOut1)/2;
//   Double_t resultDetCorInpOut1Error  =  TMath::Sqrt(pow(fitResultDetCorIn1Error,2)+pow(fitResultDetCorOut1Error,2))/2;
//   Double_t resultDetCorInmOut1Error  =  TMath::Sqrt(fitResultDetCorIn1Error*fitResultDetCorIn1Error+fitResultDetCorOut1Error*fitResultDetCorOut1Error)/2;

  Double_t resultDetCorInpOut1       =  (fitResultDetCorIn1+fitResultDetCorOut1)/2;
  Double_t resultDetCorInmOut1       =  (((1/(pow(fitResultDetCorIn1Error,2)))*(fitResultDetCorIn1))-((1/(pow(fitResultDetCorOut1Error,2)))*(fitResultDetCorOut1)))/(1/(pow(fitResultDetCorIn1Error,2))+1/(pow(fitResultDetCorOut1Error,2)));
  Double_t resultDetCorInpOut1Error  =  TMath::Sqrt(pow(fitResultDetCorIn1Error,2)+pow(fitResultDetCorOut1Error,2))/2;
  Double_t resultDetCorInmOut1Error  = 1/(TMath::Sqrt(1/(pow(fitResultDetCorIn1Error,2))+1/(pow(fitResultDetCorOut1Error,2))));



  TLegend *legDetCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legDetCor1->AddEntry("detGraphCorIn1",  Form("In Cor=%4.2f#pm%4.2f",fitResultDetCorIn1,fitResultDetCorIn1Error),"lp");
  legDetCor1->AddEntry("detGraphCorOut1",  Form("Out Cor=%4.2f#pm%4.2f",fitResultDetCorOut1,fitResultDetCorOut1Error),"lp");
  legDetCor1->AddEntry("detGraphCorInpOut1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",resultDetCorInpOut1,resultDetCorInpOut1Error),"");
  legDetCor1->AddEntry("detGraphCorInmOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",resultDetCorInmOut1,resultDetCorInmOut1Error),"");
  legDetCor1->SetTextSize(0.045);
  legDetCor1->SetFillColor(0);
  legDetCor1->SetBorderSize(2);
  legDetCor1->Draw();

  TLatex* toctantGraphCorIn1=new TLatex(0.8,6.0,Form("IN: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetCorIn1Chisquare/fitResultDetCorIn1NDF,fitResultDetCorIn1Prob));
  toctantGraphCorIn1->SetTextSize(0.068);
  toctantGraphCorIn1->SetTextColor(kOrange+2);
  TLatex* toctantGraphCorOut1=new TLatex(0.8,4.5,Form("OUT: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetCorOut1Chisquare/fitResultDetCorOut1NDF,fitResultDetCorOut1Prob));
  toctantGraphCorOut1->SetTextSize(0.068);
  toctantGraphCorOut1->SetTextColor(kGray+2);
  toctantGraphCorIn1->Draw();
  toctantGraphCorOut1->Draw();

  gPad->Update();

  c1->Update();
  c1->SaveAs(Form("%s/plots/%s/%sOctantAsym%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
//   c1->SaveAs(Form("%s/plots/%s/n2deltaSlug34MdOctantAsym2%s.png",n2deltaAnalysisDir.Data(),regScheme));
//   c1->SaveAs(Form("%s/plots/%s/n2deltaSlug34MdOctantAsymUnWeighted%s.png",n2deltaAnalysisDir.Data(),regScheme));


  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/


  if(OCTANT_PLOT_INOUT){

  // Fit and stat parameters
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
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.5);
  gStyle->SetTitleXOffset(1.0);
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

  gDirectory->Delete("*");

  TCanvas *c3 = new TCanvas("c3","c3",cx1,cy1,1200,950);
  TPad * pad30 = new TPad("pad30","pad30",ps1,ps2,ps4,ps4);
  TPad * pad31 = new TPad("pad31","pad31",ps1,ps1,ps4,ps3);
  pad30->Draw();
  pad31->Draw();
  pad30->cd();
  TText * ct30 = new TText(tll,tlr,Form("%sN to Delta %s %s Octant Aymmetry",regTitle,dataInfo,plotTitle));
  ct30->SetTextSize(tsiz);
  ct30->Draw();
  pad31->cd();
  pad31->Divide(1,3);


  pad31->cd(1);

  TGraphErrors * detInOutGraphUnRegIn1 = new TGraphErrors(counterOctantInOut2,octantNumberIn,mdAsymUnRegOctantInpOut,zeroOctantIn,emdAsymUnRegOctantInpOut);
  detInOutGraphUnRegIn1->SetName("detInOutGraphUnRegIn1");
  detInOutGraphUnRegIn1->SetMarkerColor(kRed+2);
  detInOutGraphUnRegIn1->SetLineColor(kRed+2);
  detInOutGraphUnRegIn1->SetMarkerStyle(26);
  detInOutGraphUnRegIn1->SetMarkerSize(0.7);
  detInOutGraphUnRegIn1->SetLineWidth(1);
  detInOutGraphUnRegIn1->Fit("fitDetInOutGraphUnRegIn1","E M R F 0 Q");
  fitDetInOutGraphUnRegIn1->SetLineColor(kRed+2);
  fitDetInOutGraphUnRegIn1->SetLineWidth(2);
  fitDetInOutGraphUnRegIn1->SetLineStyle(3);

  Double_t fitResultDetInOutUnRegIn1            =  fitDetInOutGraphUnRegIn1->GetParameter(0);
  Double_t fitResultDetInOutUnRegIn1Error       =  fitDetInOutGraphUnRegIn1->GetParError(0);
  Double_t fitResultDetInOutUnRegIn1Chisquare   =  fitDetInOutGraphUnRegIn1->GetChisquare();
  Double_t fitResultDetInOutUnRegIn1NDF         =  fitDetInOutGraphUnRegIn1->GetNDF();
  Double_t fitResultDetInOutUnRegIn1Prob        =  fitDetInOutGraphUnRegIn1->GetProb();


  TGraphErrors * detInOutGraphUnRegOut1 = new TGraphErrors(counterOctantInOut2,octantNumberIn,mdAsymUnRegOctantInmOut,zeroOctantIn,emdAsymUnRegOctantInmOut);
  detInOutGraphUnRegOut1->SetName("detInOutGraphUnRegOut1");
  detInOutGraphUnRegOut1->SetMarkerColor(kRed-7);
  detInOutGraphUnRegOut1->SetLineColor(kRed-7);
  detInOutGraphUnRegOut1->SetMarkerStyle(24);
  detInOutGraphUnRegOut1->SetMarkerSize(0.7);
  detInOutGraphUnRegOut1->SetLineWidth(1);
  detInOutGraphUnRegOut1->Fit("fitDetInOutGraphUnRegOut1","E M R F 0 Q");
  fitDetInOutGraphUnRegOut1->SetLineColor(kRed-7);
  fitDetInOutGraphUnRegOut1->SetLineWidth(2);
  fitDetInOutGraphUnRegOut1->SetLineStyle(3);

  Double_t fitResultDetInOutUnRegOut1           =  fitDetInOutGraphUnRegOut1->GetParameter(0);
  Double_t fitResultDetInOutUnRegOut1Error      =  fitDetInOutGraphUnRegOut1->GetParError(0);
  Double_t fitResultDetInOutUnRegOut1Chisquare  =  fitDetInOutGraphUnRegOut1->GetChisquare();
  Double_t fitResultDetInOutUnRegOut1NDF        =  fitDetInOutGraphUnRegOut1->GetNDF();
  Double_t fitResultDetInOutUnRegOut1Prob       =  fitDetInOutGraphUnRegOut1->GetProb();

  TMultiGraph *detInOutGraphUnReg1 = new TMultiGraph();
  detInOutGraphUnReg1->Add(detInOutGraphUnRegIn1);
  detInOutGraphUnReg1->Add(detInOutGraphUnRegOut1);
  detInOutGraphUnReg1->Draw("AP");
  detInOutGraphUnReg1->SetTitle("");
  detInOutGraphUnReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  detInOutGraphUnReg1->GetYaxis()->SetTitle(Form("%s Asym UnReg [ppm]",plotTitle));
  detInOutGraphUnReg1->GetXaxis()->CenterTitle();
  detInOutGraphUnReg1->GetYaxis()->CenterTitle();
  detInOutGraphUnReg1->GetXaxis()->SetNdivisions(8,0,0);
  detInOutGraphUnReg1->GetYaxis()->SetRangeUser(-8,8);
  TAxis *xaxisDetInOutGraphUnReg1= detInOutGraphUnReg1->GetXaxis();
  xaxisDetInOutGraphUnReg1->SetLimits(det_range[0],det_range[1]);
  fitDetInOutGraphUnRegIn1->Draw("same");
  fitDetInOutGraphUnRegOut1->Draw("same");

  TPaveStats *statsDetInOutUnRegIn1 =(TPaveStats*)detInOutGraphUnRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDetInOutUnRegOut1=(TPaveStats*)detInOutGraphUnRegOut1->GetListOfFunctions()->FindObject("stats");
  statsDetInOutUnRegIn1->SetTextColor(kRed+2);
  statsDetInOutUnRegIn1->SetFillColor(kWhite); 
  statsDetInOutUnRegOut1->SetTextColor(kRed-7);
  statsDetInOutUnRegOut1->SetFillColor(kWhite); 
  statsDetInOutUnRegIn1->SetX1NDC(x_lo_stat_in);    statsDetInOutUnRegIn1->SetX2NDC(x_hi_stat_in); 
  statsDetInOutUnRegIn1->SetY1NDC(y_lo_stat_in);    statsDetInOutUnRegIn1->SetY2NDC(y_hi_stat_in);
  statsDetInOutUnRegOut1->SetX1NDC(x_lo_stat_out);  statsDetInOutUnRegOut1->SetX2NDC(x_hi_stat_out); 
  statsDetInOutUnRegOut1->SetY1NDC(y_lo_stat_out);  statsDetInOutUnRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultDetInOutUnRegInpOut1       =  (fitResultDetInOutUnRegIn1+fitResultDetInOutUnRegOut1)/2;
  Double_t resultDetInOutUnRegInmOut1       =  (fitResultDetInOutUnRegIn1-fitResultDetInOutUnRegOut1)/2;
  Double_t resultDetInOutUnRegInpOut1Error  =  TMath::Sqrt(pow(fitResultDetInOutUnRegIn1Error,2)+pow(fitResultDetInOutUnRegOut1Error,2))/2;
  Double_t resultDetInOutUnRegInmOut1Error  =  TMath::Sqrt(fitResultDetInOutUnRegIn1Error*fitResultDetInOutUnRegIn1Error+fitResultDetInOutUnRegOut1Error*fitResultDetInOutUnRegOut1Error)/2;


  TLegend *legDetInOutUnReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legDetInOutUnReg1->AddEntry("detInOutGraphUnRegIn1",  Form("#frac{In+Out}{2} UnReg=%4.2f#pm%4.2f",fitResultDetInOutUnRegIn1,fitResultDetInOutUnRegIn1Error),"lp");
  legDetInOutUnReg1->AddEntry("detInOutGraphUnRegOut1", Form("#frac{In-Out}{2} UnReg=%4.2f#pm%4.2f",fitResultDetInOutUnRegOut1,fitResultDetInOutUnRegOut1Error),"lp");
  legDetInOutUnReg1->SetTextSize(0.055);
  legDetInOutUnReg1->SetFillColor(0);
  legDetInOutUnReg1->SetBorderSize(2);
  legDetInOutUnReg1->Draw();

  TLatex* toctantInOutGraphUnRegIn1=new TLatex(0.8,6.0,Form("(IN+OUT)/2: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetInOutUnRegIn1Chisquare/fitResultDetInOutUnRegIn1NDF,fitResultDetInOutUnRegIn1Prob));
  toctantInOutGraphUnRegIn1->SetTextSize(0.068);
  toctantInOutGraphUnRegIn1->SetTextColor(kRed+2);
  TLatex* toctantInOutGraphUnRegOut1=new TLatex(0.8,4.5,Form("(IN-OUT)/2: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetInOutUnRegOut1Chisquare/fitResultDetInOutUnRegOut1NDF,fitResultDetInOutUnRegOut1Prob));
  toctantInOutGraphUnRegOut1->SetTextSize(0.068);
  toctantInOutGraphUnRegOut1->SetTextColor(kRed-7);
  toctantInOutGraphUnRegIn1->Draw();
  toctantInOutGraphUnRegOut1->Draw();

  gPad->Update();


  pad31->cd(2);

  TGraphErrors * detInOutGraphRegIn1 = new TGraphErrors(counterOctantInOut2,octantNumberIn,mdAsymRegOctantInpOut,zeroOctantIn,emdAsymRegOctantInpOut);
  detInOutGraphRegIn1->SetName("detInOutGraphRegIn1");
  detInOutGraphRegIn1->SetMarkerColor(kRed+2);
  detInOutGraphRegIn1->SetLineColor(kRed+2);
  detInOutGraphRegIn1->SetMarkerStyle(22);
  detInOutGraphRegIn1->SetMarkerSize(0.7);
  detInOutGraphRegIn1->SetLineWidth(1);
  detInOutGraphRegIn1->Fit("fitDetInOutGraphRegIn1","E M R F 0 Q");
  fitDetInOutGraphRegIn1->SetLineColor(kRed+2);
  fitDetInOutGraphRegIn1->SetLineWidth(2);
  fitDetInOutGraphRegIn1->SetLineStyle(1);

  Double_t fitResultDetInOutRegIn1            =  fitDetInOutGraphRegIn1->GetParameter(0);
  Double_t fitResultDetInOutRegIn1Error       =  fitDetInOutGraphRegIn1->GetParError(0);
  Double_t fitResultDetInOutRegIn1Chisquare   =  fitDetInOutGraphRegIn1->GetChisquare();
  Double_t fitResultDetInOutRegIn1NDF         =  fitDetInOutGraphRegIn1->GetNDF();
  Double_t fitResultDetInOutRegIn1Prob        =  fitDetInOutGraphRegIn1->GetProb();


  TGraphErrors * detInOutGraphRegOut1 = new TGraphErrors(counterOctantInOut2,octantNumberIn,mdAsymRegOctantInmOut,zeroOctantIn,emdAsymRegOctantInmOut);
  detInOutGraphRegOut1->SetName("detInOutGraphRegOut1");
  detInOutGraphRegOut1->SetMarkerColor(kRed-7);
  detInOutGraphRegOut1->SetLineColor(kRed-7);
  detInOutGraphRegOut1->SetMarkerStyle(20);
  detInOutGraphRegOut1->SetMarkerSize(0.7);
  detInOutGraphRegOut1->SetLineWidth(1);
  detInOutGraphRegOut1->Fit("fitDetInOutGraphRegOut1","E M R F 0 Q");
  fitDetInOutGraphRegOut1->SetLineColor(kRed-7);
  fitDetInOutGraphRegOut1->SetLineWidth(2);
  fitDetInOutGraphRegOut1->SetLineStyle(1);

  Double_t fitResultDetInOutRegOut1           =  fitDetInOutGraphRegOut1->GetParameter(0);
  Double_t fitResultDetInOutRegOut1Error      =  fitDetInOutGraphRegOut1->GetParError(0);
  Double_t fitResultDetInOutRegOut1Chisquare  =  fitDetInOutGraphRegOut1->GetChisquare();
  Double_t fitResultDetInOutRegOut1NDF        =  fitDetInOutGraphRegOut1->GetNDF();
  Double_t fitResultDetInOutRegOut1Prob       =  fitDetInOutGraphRegOut1->GetProb();

  TMultiGraph *detInOutGraphReg1 = new TMultiGraph();
  detInOutGraphReg1->Add(detInOutGraphRegIn1);
  detInOutGraphReg1->Add(detInOutGraphRegOut1);
  detInOutGraphReg1->Draw("APL");
  detInOutGraphReg1->SetTitle("");
  detInOutGraphReg1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  detInOutGraphReg1->GetYaxis()->SetTitle(Form("%s Asym Reg [ppm]",plotTitle));
  detInOutGraphReg1->GetXaxis()->CenterTitle();
  detInOutGraphReg1->GetYaxis()->CenterTitle();
  detInOutGraphReg1->GetXaxis()->SetNdivisions(8,0,0);
  detInOutGraphReg1->GetYaxis()->SetRangeUser(-8,8);
  TAxis *xaxisDetInOutGraphReg1= detInOutGraphReg1->GetXaxis();
  xaxisDetInOutGraphReg1->SetLimits(det_range[0],det_range[1]);
  fitDetInOutGraphRegIn1->Draw("same");
  fitDetInOutGraphRegOut1->Draw("same");

  TPaveStats *statsDetInOutRegIn1 =(TPaveStats*)detInOutGraphRegIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDetInOutRegOut1=(TPaveStats*)detInOutGraphRegOut1->GetListOfFunctions()->FindObject("stats");
  statsDetInOutRegIn1->SetTextColor(kRed+2);
  statsDetInOutRegIn1->SetFillColor(kWhite); 
  statsDetInOutRegOut1->SetTextColor(kRed-7);
  statsDetInOutRegOut1->SetFillColor(kWhite); 
  statsDetInOutRegIn1->SetX1NDC(x_lo_stat_in);    statsDetInOutRegIn1->SetX2NDC(x_hi_stat_in); 
  statsDetInOutRegIn1->SetY1NDC(y_lo_stat_in);    statsDetInOutRegIn1->SetY2NDC(y_hi_stat_in);
  statsDetInOutRegOut1->SetX1NDC(x_lo_stat_out);  statsDetInOutRegOut1->SetX2NDC(x_hi_stat_out); 
  statsDetInOutRegOut1->SetY1NDC(y_lo_stat_out);  statsDetInOutRegOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultDetInOutRegInpOut1       =  (fitResultDetInOutRegIn1+fitResultDetInOutRegOut1)/2;
  Double_t resultDetInOutRegInmOut1       =  (fitResultDetInOutRegIn1-fitResultDetInOutRegOut1)/2;
  Double_t resultDetInOutRegInpOut1Error  =  TMath::Sqrt(pow(fitResultDetInOutRegIn1Error,2)+pow(fitResultDetInOutRegOut1Error,2))/2;
  Double_t resultDetInOutRegInmOut1Error  =  TMath::Sqrt(fitResultDetInOutRegIn1Error*fitResultDetInOutRegIn1Error+fitResultDetInOutRegOut1Error*fitResultDetInOutRegOut1Error)/2;


  TLegend *legDetInOutReg1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legDetInOutReg1->AddEntry("detInOutGraphRegIn1",  Form("#frac{In+Out}{2} Reg=%4.2f#pm%4.2f",fitResultDetInOutRegIn1,fitResultDetInOutRegIn1Error),"lp");
  legDetInOutReg1->AddEntry("detInOutGraphRegOut1", Form("#frac{In-Out}{2} Reg=%4.2f#pm%4.2f",fitResultDetInOutRegOut1,fitResultDetInOutRegOut1Error),"lp");
  legDetInOutReg1->SetTextSize(0.055);
  legDetInOutReg1->SetFillColor(0);
  legDetInOutReg1->SetBorderSize(2);
  legDetInOutReg1->Draw();

  TLatex* toctantInOutGraphRegIn1=new TLatex(0.8,6.0,Form("(IN+OUT)/2: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetInOutRegIn1Chisquare/fitResultDetInOutRegIn1NDF,fitResultDetInOutRegIn1Prob));
  toctantInOutGraphRegIn1->SetTextSize(0.068);
  toctantInOutGraphRegIn1->SetTextColor(kRed+2);
  TLatex* toctantInOutGraphRegOut1=new TLatex(0.8,4.5,Form("(IN-OUT)/2: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetInOutRegOut1Chisquare/fitResultDetInOutRegOut1NDF,fitResultDetInOutRegOut1Prob));
  toctantInOutGraphRegOut1->SetTextSize(0.068);
  toctantInOutGraphRegOut1->SetTextColor(kRed-7);
  toctantInOutGraphRegIn1->Draw();
  toctantInOutGraphRegOut1->Draw();

  gPad->Update();


  pad31->cd(3);

  TGraphErrors * detInOutGraphCorIn1 = new TGraphErrors(counterOctantInOut2,octantNumberIn,mdAsymCorOctantInpOut,zeroOctantIn,emdAsymCorOctantInpOut);
  detInOutGraphCorIn1->SetName("detInOutGraphCorIn1");
  detInOutGraphCorIn1->SetMarkerColor(kOrange+4);
  detInOutGraphCorIn1->SetLineColor(kOrange+4);
  detInOutGraphCorIn1->SetMarkerStyle(22);
  detInOutGraphCorIn1->SetMarkerSize(0.7);
  detInOutGraphCorIn1->SetLineWidth(1);
  detInOutGraphCorIn1->Fit("fitDetInOutGraphCorIn1","E M R F 0 Q");
  fitDetInOutGraphCorIn1->SetLineColor(kOrange+4);
  fitDetInOutGraphCorIn1->SetLineWidth(2);
  fitDetInOutGraphCorIn1->SetLineStyle(1);

  Double_t fitResultDetInOutCorIn1            =  fitDetInOutGraphCorIn1->GetParameter(0);
  Double_t fitResultDetInOutCorIn1Error       =  fitDetInOutGraphCorIn1->GetParError(0);
  Double_t fitResultDetInOutCorIn1Chisquare   =  fitDetInOutGraphCorIn1->GetChisquare();
  Double_t fitResultDetInOutCorIn1NDF         =  fitDetInOutGraphCorIn1->GetNDF();
  Double_t fitResultDetInOutCorIn1Prob        =  fitDetInOutGraphCorIn1->GetProb();


  TGraphErrors * detInOutGraphCorOut1 = new TGraphErrors(counterOctantInOut2,octantNumberIn,mdAsymCorOctantInmOut,zeroOctantIn,emdAsymCorOctantInmOut);
  detInOutGraphCorOut1->SetName("detInOutGraphCorOut1");
  detInOutGraphCorOut1->SetMarkerColor(kOrange+10);
  detInOutGraphCorOut1->SetLineColor(kOrange+10);
  detInOutGraphCorOut1->SetMarkerStyle(20);
  detInOutGraphCorOut1->SetMarkerSize(0.7);
  detInOutGraphCorOut1->SetLineWidth(1);
  detInOutGraphCorOut1->Fit("fitDetInOutGraphCorOut1","E M R F 0 Q");
  fitDetInOutGraphCorOut1->SetLineColor(kOrange+10);
  fitDetInOutGraphCorOut1->SetLineWidth(2);
  fitDetInOutGraphCorOut1->SetLineStyle(1);

  Double_t fitResultDetInOutCorOut1           =  fitDetInOutGraphCorOut1->GetParameter(0);
  Double_t fitResultDetInOutCorOut1Error      =  fitDetInOutGraphCorOut1->GetParError(0);
  Double_t fitResultDetInOutCorOut1Chisquare  =  fitDetInOutGraphCorOut1->GetChisquare();
  Double_t fitResultDetInOutCorOut1NDF        =  fitDetInOutGraphCorOut1->GetNDF();
  Double_t fitResultDetInOutCorOut1Prob       =  fitDetInOutGraphCorOut1->GetProb();

  TMultiGraph *detInOutGraphCor1 = new TMultiGraph();
  detInOutGraphCor1->Add(detInOutGraphCorIn1);
  detInOutGraphCor1->Add(detInOutGraphCorOut1);
  detInOutGraphCor1->Draw("AP");
  detInOutGraphCor1->SetTitle("");
  detInOutGraphCor1->GetXaxis()->SetTitle(Form("%s Bar Number",plotTitle));
  detInOutGraphCor1->GetYaxis()->SetTitle(Form("%s Asym Cor [ppm]",plotTitle));
  detInOutGraphCor1->GetXaxis()->CenterTitle();
  detInOutGraphCor1->GetYaxis()->CenterTitle();
  detInOutGraphCor1->GetXaxis()->SetNdivisions(8,0,0);
  detInOutGraphCor1->GetYaxis()->SetRangeUser(-8,8);
  TAxis *xaxisDetInOutGraphCor1= detInOutGraphCor1->GetXaxis();
  xaxisDetInOutGraphCor1->SetLimits(det_range[0],det_range[1]);
  fitDetInOutGraphCorIn1->Draw("same");
  fitDetInOutGraphCorOut1->Draw("same");

  TPaveStats *statsDetInOutCorIn1 =(TPaveStats*)detInOutGraphCorIn1->GetListOfFunctions()->FindObject("stats");
  TPaveStats *statsDetInOutCorOut1=(TPaveStats*)detInOutGraphCorOut1->GetListOfFunctions()->FindObject("stats");
  statsDetInOutCorIn1->SetTextColor(kOrange+4);
  statsDetInOutCorIn1->SetFillColor(kWhite); 
  statsDetInOutCorOut1->SetTextColor(kOrange+10);
  statsDetInOutCorOut1->SetFillColor(kWhite); 
  statsDetInOutCorIn1->SetX1NDC(x_lo_stat_in);    statsDetInOutCorIn1->SetX2NDC(x_hi_stat_in); 
  statsDetInOutCorIn1->SetY1NDC(y_lo_stat_in);    statsDetInOutCorIn1->SetY2NDC(y_hi_stat_in);
  statsDetInOutCorOut1->SetX1NDC(x_lo_stat_out);  statsDetInOutCorOut1->SetX2NDC(x_hi_stat_out); 
  statsDetInOutCorOut1->SetY1NDC(y_lo_stat_out);  statsDetInOutCorOut1->SetY2NDC(y_hi_stat_out);

  Double_t resultDetInOutCorInpOut1       =  (fitResultDetInOutCorIn1+fitResultDetInOutCorOut1)/2;
  Double_t resultDetInOutCorInmOut1       =  (fitResultDetInOutCorIn1-fitResultDetInOutCorOut1)/2;
  Double_t resultDetInOutCorInpOut1Error  =  TMath::Sqrt(pow(fitResultDetInOutCorIn1Error,2)+pow(fitResultDetInOutCorOut1Error,2))/2;
  Double_t resultDetInOutCorInmOut1Error  =  TMath::Sqrt(fitResultDetInOutCorIn1Error*fitResultDetInOutCorIn1Error+fitResultDetInOutCorOut1Error*fitResultDetInOutCorOut1Error)/2;


  TLegend *legDetInOutCor1 = new TLegend(x_lo_leg,y_lo_leg,x_hi_leg,y_hi_leg);
  legDetInOutCor1->AddEntry("detInOutGraphCorIn1",  Form("#frac{In+Out}{2} Cor=%4.2f#pm%4.2f",fitResultDetInOutCorIn1,fitResultDetInOutCorIn1Error),"lp");
  legDetInOutCor1->AddEntry("detInOutGraphCorOut1", Form("#frac{In-Out}{2} Cor=%4.2f#pm%4.2f",fitResultDetInOutCorOut1,fitResultDetInOutCorOut1Error),"lp");
  legDetInOutCor1->SetTextSize(0.055);
  legDetInOutCor1->SetFillColor(0);
  legDetInOutCor1->SetBorderSize(2);
  legDetInOutCor1->Draw();

  TLatex* toctantInOutGraphCorIn1=new TLatex(0.8,6.0,Form("(IN+OUT)/2: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetInOutCorIn1Chisquare/fitResultDetInOutCorIn1NDF,fitResultDetInOutCorIn1Prob));
  toctantInOutGraphCorIn1->SetTextSize(0.068);
  toctantInOutGraphCorIn1->SetTextColor(kOrange+4);
  TLatex* toctantInOutGraphCorOut1=new TLatex(0.8,4.5,Form("(IN-OUT)/2: #chi^{2}/DOF = %2.2f, Prob = %2.2f",fitResultDetInOutCorOut1Chisquare/fitResultDetInOutCorOut1NDF,fitResultDetInOutCorOut1Prob));
  toctantInOutGraphCorOut1->SetTextSize(0.068);
  toctantInOutGraphCorOut1->SetTextColor(kOrange+10);
  toctantInOutGraphCorIn1->Draw();
  toctantInOutGraphCorOut1->Draw();

  gPad->Update();


  c3->Update();
  c3->SaveAs(Form("%s/plots/%s/%sOctantAsymInOut%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));
//   c3->SaveAs(Form("%s/plots/%s/n2deltaSlug34MdOctantAsymInOut2%s.png",n2deltaAnalysisDir.Data(),regScheme));
//   c3->SaveAs(Form("%s/plots/%s/n2deltaSlug34MdOctantAsymInOutUnWeighted%s.png",n2deltaAnalysisDir.Data(),regScheme));
  /*********************************************************************************/

  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/


  if(SUMMARY_PLOT){

    const Int_t sNUM=1;
    Double_t sZero[sNUM];
    Double_t yJLUnRegOut[sNUM],yJLUnRegIn[sNUM],yJLRegOut[sNUM],yJLRegIn[sNUM],eyJLUnRegOut[sNUM],eyJLUnRegIn[sNUM],eyJLRegOut[sNUM],eyJLRegIn[sNUM];
    Double_t yUnRegOut[sNUM],yUnRegIn[sNUM],yRegOut[sNUM],yRegIn[sNUM],eyUnRegOut[sNUM],eyUnRegIn[sNUM],eyRegOut[sNUM],eyRegIn[sNUM];
    Double_t yRunletCorOut[sNUM],yRunletCorIn[sNUM],eyRunletCorOut[sNUM],eyRunletCorIn[sNUM];
    Double_t ySlugCorOut[sNUM],ySlugCorIn[sNUM],eySlugCorOut[sNUM],eySlugCorIn[sNUM];
    Double_t ySlugAvgCorOut[sNUM],ySlugAvgCorIn[sNUM],eySlugAvgCorOut[sNUM],eySlugAvgCorIn[sNUM];

    Double_t yJLUnRegInpOut[sNUM],yJLUnRegInmOut[sNUM],yJLRegInpOut[sNUM],yJLRegInmOut[sNUM],eyJLUnRegInpOut[sNUM],eyJLUnRegInmOut[sNUM],eyJLRegInpOut[sNUM],eyJLRegInmOut[sNUM];
    Double_t yUnRegInpOut[sNUM],yUnRegInmOut[sNUM],yRegInpOut[sNUM],yRegInmOut[sNUM],eyUnRegInpOut[sNUM],eyUnRegInmOut[sNUM],eyRegInpOut[sNUM],eyRegInmOut[sNUM];
    Double_t yRunletCorInpOut[sNUM],yRunletCorInmOut[sNUM],eyRunletCorInpOut[sNUM],eyRunletCorInmOut[sNUM];
    Double_t ySlugCorInpOut[sNUM],ySlugCorInmOut[sNUM],eySlugCorInpOut[sNUM],eySlugCorInmOut[sNUM];
    Double_t ySlugAvgCorInpOut[sNUM],ySlugAvgCorInmOut[sNUM],eySlugAvgCorInpOut[sNUM],eySlugAvgCorInmOut[sNUM];

  const Int_t n1 = 1;

    Double_t x1[sNUM]           = {1.0};
    Double_t x2[sNUM]           = {2.0};
    Double_t x3[sNUM]           = {3.0};
    Double_t x4[sNUM]           = {4.0};
    Double_t x5[sNUM]           = {1.1};
    Double_t x6[sNUM]           = {2.1};
    Double_t x7[sNUM]           = {3.1};
    Double_t x8[sNUM]           = {4.1};
    
    Double_t x10[sNUM]          = {0.9};
    Double_t x11[sNUM]          = {1.9};
    Double_t x12[sNUM]          = {2.9};
    Double_t x13[sNUM]          = {3.9};

    Double_t x20[sNUM]          = {1.2};
    Double_t x21[sNUM]          = {2.2};
    Double_t x22[sNUM]          = {3.2};
    Double_t x23[sNUM]          = {4.2};

    Double_t ex[sNUM]           = {0.0};

    int sCounter=0;
    ifstream JLUnReg,JLReg,UnReg,Reg,RunletCor,SlugCor,SlugAvgCor;
    JLUnReg.open((Form("%s/dataBaseAnalysis/summaryJLUnReg.txt",n2deltaAnalysisDir.Data())));
    JLReg.open((Form("%s/dataBaseAnalysis/summaryJLReg.txt",n2deltaAnalysisDir.Data())));
    UnReg.open((Form("%s/plots/%s/summary%sRunletUnReg.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot)));
    Reg.open((Form("%s/plots/%s/summary%sRunlet%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
    RunletCor.open((Form("%s/plots/%s/summary%sRunletCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
    SlugCor.open((Form("%s/plots/%s/summary%sRunletSlugCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));
    SlugAvgCor.open((Form("%s/plots/%s/summary%sRunletSlugAvgCor%s.txt",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme)));

    while(1) {
      sZero[sCounter]=0;

      JLUnReg >> yJLUnRegOut[sCounter];
      JLUnReg >> eyJLUnRegOut[sCounter];
      JLUnReg >> yJLUnRegIn[sCounter];
      JLUnReg >> eyJLUnRegIn[sCounter];
      if (!JLUnReg.good()) break;

      JLReg   >> yJLRegOut[sCounter];
      JLReg   >> eyJLRegOut[sCounter];
      JLReg   >> yJLRegIn[sCounter];
      JLReg   >> eyJLRegIn[sCounter];
      if (!JLReg.good()) break;

      UnReg >> yUnRegOut[sCounter];
      UnReg >> eyUnRegOut[sCounter];
      UnReg >> yUnRegIn[sCounter];
      UnReg >> eyUnRegIn[sCounter];
      if (!UnReg.good()) break;

      Reg   >> yRegOut[sCounter];
      Reg   >> eyRegOut[sCounter];
      Reg   >> yRegIn[sCounter];
      Reg   >> eyRegIn[sCounter];
      if (!Reg.good()) break;

      RunletCor   >> yRunletCorOut[sCounter];
      RunletCor   >> eyRunletCorOut[sCounter];
      RunletCor   >> yRunletCorIn[sCounter];
      RunletCor   >> eyRunletCorIn[sCounter];
      if (!RunletCor.good()) break;

      SlugCor   >> ySlugCorOut[sCounter];
      SlugCor   >> eySlugCorOut[sCounter];
      SlugCor   >> ySlugCorIn[sCounter];
      SlugCor   >> eySlugCorIn[sCounter];
      if (!SlugCor.good()) break;

      SlugAvgCor   >> ySlugAvgCorOut[sCounter];
      SlugAvgCor   >> eySlugAvgCorOut[sCounter];
      SlugAvgCor   >> ySlugAvgCorIn[sCounter];
      SlugAvgCor   >> eySlugAvgCorIn[sCounter];
      if (!SlugAvgCor.good()) break;



      yJLUnRegInpOut[sCounter]    =  (yJLUnRegIn[sCounter]+yJLUnRegOut[sCounter])/2;
      eyJLUnRegInpOut[sCounter]   =  TMath::Sqrt(eyJLUnRegIn[sCounter]*eyJLUnRegIn[sCounter]+eyJLUnRegOut[sCounter]*eyJLUnRegOut[sCounter])/2;
      yJLUnRegInmOut[sCounter]    =  ((yJLUnRegIn[sCounter]*(1/(eyJLUnRegIn[sCounter]*eyJLUnRegIn[sCounter])))-(yJLUnRegOut[sCounter]*(1/(eyJLUnRegOut[sCounter]*eyJLUnRegOut[sCounter]))))/((1/(eyJLUnRegIn[sCounter]*eyJLUnRegIn[sCounter]))+(1/(eyJLUnRegOut[sCounter]*eyJLUnRegOut[sCounter])));
      eyJLUnRegInmOut[sCounter]   =  1/TMath::Sqrt(1/(eyJLUnRegIn[sCounter]*eyJLUnRegIn[sCounter])+1/(eyJLUnRegOut[sCounter]*eyJLUnRegOut[sCounter]));

      yJLRegInpOut[sCounter]      =  (yJLRegIn[sCounter]+yJLRegOut[sCounter])/2;
      eyJLRegInpOut[sCounter]     =  TMath::Sqrt(eyJLRegIn[sCounter]*eyJLRegIn[sCounter]+eyJLRegOut[sCounter]*eyJLRegOut[sCounter])/2;
      yJLRegInmOut[sCounter]      =  ((yJLRegIn[sCounter]*(1/(eyJLRegIn[sCounter]*eyJLRegIn[sCounter])))-(yJLRegOut[sCounter]*(1/(eyJLRegOut[sCounter]*eyJLRegOut[sCounter]))))/((1/(eyJLRegIn[sCounter]*eyJLRegIn[sCounter]))+(1/(eyJLRegOut[sCounter]*eyJLRegOut[sCounter])));
      eyJLRegInmOut[sCounter]     =  1/TMath::Sqrt(1/(eyJLRegIn[sCounter]*eyJLRegIn[sCounter])+1/(eyJLRegOut[sCounter]*eyJLRegOut[sCounter]));

      yUnRegInpOut[sCounter]      =  (yUnRegIn[sCounter]+yUnRegOut[sCounter])/2;
      eyUnRegInpOut[sCounter]     =  TMath::Sqrt(eyUnRegIn[sCounter]*eyUnRegIn[sCounter]+eyUnRegOut[sCounter]*eyUnRegOut[sCounter])/2;
      yUnRegInmOut[sCounter]      =  ((yUnRegIn[sCounter]*(1/(eyUnRegIn[sCounter]*eyUnRegIn[sCounter])))-(yUnRegOut[sCounter]*(1/(eyUnRegOut[sCounter]*eyUnRegOut[sCounter]))))/((1/(eyUnRegIn[sCounter]*eyUnRegIn[sCounter]))+(1/(eyUnRegOut[sCounter]*eyUnRegOut[sCounter])));
      eyUnRegInmOut[sCounter]     =  1/TMath::Sqrt(1/(eyUnRegIn[sCounter]*eyUnRegIn[sCounter])+1/(eyUnRegOut[sCounter]*eyUnRegOut[sCounter]));

      yRegInpOut[sCounter]        =  (yRegIn[sCounter]+yRegOut[sCounter])/2;
      eyRegInpOut[sCounter]       =  TMath::Sqrt(eyRegIn[sCounter]*eyRegIn[sCounter]+eyRegOut[sCounter]*eyRegOut[sCounter])/2;
      yRegInmOut[sCounter]        =  ((yRegIn[sCounter]*(1/(eyRegIn[sCounter]*eyRegIn[sCounter])))-(yRegOut[sCounter]*(1/(eyRegOut[sCounter]*eyRegOut[sCounter]))))/((1/(eyRegIn[sCounter]*eyRegIn[sCounter]))+(1/(eyRegOut[sCounter]*eyRegOut[sCounter])));
      eyRegInmOut[sCounter]       =  1/TMath::Sqrt(1/(eyRegIn[sCounter]*eyRegIn[sCounter])+1/(eyRegOut[sCounter]*eyRegOut[sCounter]));

      yRunletCorInpOut[sCounter]  =  (yRunletCorIn[sCounter]+yRunletCorOut[sCounter])/2;
      eyRunletCorInpOut[sCounter] =  TMath::Sqrt(eyRunletCorIn[sCounter]*eyRunletCorIn[sCounter]+eyRunletCorOut[sCounter]*eyRunletCorOut[sCounter])/2;
      yRunletCorInmOut[sCounter]  =  ((yRunletCorIn[sCounter]*(1/(eyRunletCorIn[sCounter]*eyRunletCorIn[sCounter])))-(yRunletCorOut[sCounter]*(1/(eyRunletCorOut[sCounter]*eyRunletCorOut[sCounter]))))/((1/(eyRunletCorIn[sCounter]*eyRunletCorIn[sCounter]))+(1/(eyRunletCorOut[sCounter]*eyRunletCorOut[sCounter])));
      eyRunletCorInmOut[sCounter] =  1/TMath::Sqrt(1/(eyRunletCorIn[sCounter]*eyRunletCorIn[sCounter])+1/(eyRunletCorOut[sCounter]*eyRunletCorOut[sCounter]));

      ySlugCorInpOut[sCounter]    =  (ySlugCorIn[sCounter]+ySlugCorOut[sCounter])/2;
      eySlugCorInpOut[sCounter]   =  TMath::Sqrt(eySlugCorIn[sCounter]*eySlugCorIn[sCounter]+eySlugCorOut[sCounter]*eySlugCorOut[sCounter])/2;
      ySlugCorInmOut[sCounter]    =  ((ySlugCorIn[sCounter]*(1/(eySlugCorIn[sCounter]*eySlugCorIn[sCounter])))-(ySlugCorOut[sCounter]*(1/(eySlugCorOut[sCounter]*eySlugCorOut[sCounter]))))/((1/(eySlugCorIn[sCounter]*eySlugCorIn[sCounter]))+(1/(eySlugCorOut[sCounter]*eySlugCorOut[sCounter])));
      eySlugCorInmOut[sCounter]   =  1/TMath::Sqrt(1/(eySlugCorIn[sCounter]*eySlugCorIn[sCounter])+1/(eySlugCorOut[sCounter]*eySlugCorOut[sCounter]));

      ySlugAvgCorInpOut[sCounter]  =  (ySlugAvgCorIn[sCounter]+ySlugAvgCorOut[sCounter])/2;
      eySlugAvgCorInpOut[sCounter] =  TMath::Sqrt(eySlugAvgCorIn[sCounter]*eySlugAvgCorIn[sCounter]+eySlugAvgCorOut[sCounter]*eySlugAvgCorOut[sCounter])/2;
      ySlugAvgCorInmOut[sCounter]  =  ((ySlugAvgCorIn[sCounter]*(1/(eySlugAvgCorIn[sCounter]*eySlugAvgCorIn[sCounter])))-(ySlugAvgCorOut[sCounter]*(1/(eySlugAvgCorOut[sCounter]*eySlugAvgCorOut[sCounter]))))/((1/(eySlugAvgCorIn[sCounter]*eySlugAvgCorIn[sCounter]))+(1/(eySlugAvgCorOut[sCounter]*eySlugAvgCorOut[sCounter])));
      eySlugAvgCorInmOut[sCounter] =  1/TMath::Sqrt(1/(eySlugAvgCorIn[sCounter]*eySlugAvgCorIn[sCounter])+1/(eySlugAvgCorOut[sCounter]*eySlugAvgCorOut[sCounter]));

      sCounter++;
    }
    JLUnReg.close();
    JLReg.close();
    UnReg.close();
    Reg.close();
    RunletCor.close();
    SlugCor.close();
    SlugAvgCor.close();

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.10);
  gStyle->SetStatH(0.3);
  
  //Pad parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetTitleBorderSize(0);

  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(0); 
  gStyle->SetStatColor(0);
  //   gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.12);
  gStyle->SetPadLeftMargin(0.08);

  gStyle->SetNdivisions(507,"y");

  // histo parameters
  gStyle->SetTitleYOffset(0.7);
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.07);

  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");

  gDirectory->Delete("*");



//   Double_t yUnRegOut[n1]    = {-2.08};
//   Double_t yRegOut[n1]      = {-2.95};
//   Double_t yCorOut[n1]    = {-2.98};
//   Double_t yUnRegIn[n1]     = {0.61};
//   Double_t yRegIn[n1]       = {-0.47};
//   Double_t yCorIn[n1]     = {-0.47};

//   Double_t eyUnRegOut[n1]   = {0.30};
//   Double_t eyRegOut[n1]     = {0.30};
//   Double_t eyCorOut[n1]     = {0.31};
//   Double_t eyUnRegIn[n1]    = {0.36};
//   Double_t eyRegIn[n1]      = {0.36};
//   Double_t eyCorIn[n1]      = {0.37};

//   Double_t yUnRegInpOut[n1] = {-0.74}; 
//   Double_t yUnRegInmOut[n1] = {1.48};
//   Double_t eyUnRegInOut[n1] = {0.23259621};
//   Double_t eyUnRegInpOut[n1] = {0.23};
//   Double_t eyUnRegInmOut[n1] = {0.23};
//   Double_t yRegInpOut[n1]   = {-1.71};
//   Double_t yRegInmOut[n1]   = {1.55};
//   Double_t eyRegInOut[n1]   = {0.236362715};
//   Double_t eyRegInpOut[n1] = {0.23};
//   Double_t eyRegInmOut[n1] = {0.23};
//   Double_t yCorInpOut[n1]   = {-1.73};
//   Double_t yCorInmOut[n1]   = {1.56};
//   Double_t eyCorInOut[n1]   = {0.240351129579875367};
//   Double_t eyCorInpOut[n1]   = {0.24};
//   Double_t eyCorInmOut[n1]   = {0.24};

//   Double_t yJLUnRegOut[n1]  = {-2.04};
//   Double_t yJLRegOut[n1]    = {-2.84};
//   Double_t yJLUnRegIn[n1]   = {0.87};
//   Double_t yJLRegIn[n1]     = {0.012};
//   Double_t yJLUnRegInpOut[n1]  = {-0.59};
//   Double_t yJLUnRegInmOut[n1]  = {1.49};
//   Double_t yJLRegInpOut[n1]    = {-1.41};
//   Double_t yJLRegInmOut[n1]    = {1.52};
//   Double_t eyJLUnRegOut[n1] = {0.30};
//   Double_t eyJLRegOut[n1]   = {0.30};
//   Double_t eyJLUnRegIn[n1]  = {0.32};
//   Double_t eyJLRegIn[n1]    = {0.32};
//   Double_t eyJLUnRegInOut[n1] = {0.22};
//   Double_t eyJLUnRegInpOut[n1] = {0.22};
//   Double_t eyJLUnRegInmOut[n1] = {0.22};
//   Double_t eyJLRegInOut[n1] = {0.22};
//   Double_t eyJLRegInpOut[n1] = {0.22};
//   Double_t eyJLRegInmOut[n1] = {0.22};

  Double_t tx1          = 1.14;
  Double_t tx2          = 2.18;
  Double_t tx3          = 3.18;
  Double_t tx4          = 4.14;
  Double_t tx5          = 1.15;
  Double_t tx6          = 2.15;
  Double_t tx7          = 3.15;
  Double_t tx8          = 4.15;
  Double_t tx10         = 0.35;
  Double_t tx11         = 1.35;
  Double_t tx12         = 2.36;
  Double_t tx13         = 3.39;



//   Double_t tUnRegOut   = yUnRegOut->GetValue();
//   Double_t etUnRegOut   = eyUnRegOut->GetValue();

//   Double_t tUnRegOut    = -2.0793391;
//   Double_t tRegOut      = -2.9492920;
//   Double_t tUnRegIn     = 0.6055315;
//   Double_t tRegIn       = -0.4693686;

//   Double_t etUnRegOut   = 0.2996620;
//   Double_t etRegOut     = 0.3047041;
//   Double_t etUnRegIn    = 0.3558214;
//   Double_t etRegIn      = 0.3614205;

//   Double_t tRunletCorOut    = -2.9781577;
//   Double_t tRunletCorIn     = -0.4692505;
//   Double_t etRunletCorOut     = 0.3047041;
//   Double_t etRunletCorIn      = 0.3614205;

//   Double_t tSlugCorOut    = -2.9702116;
//   Double_t tSlugCorIn     = -0.4549556;
//   Double_t etSlugCorOut     = 0.3047041;
//   Double_t etSlugCorIn      = 0.3614205;

//   Double_t tSlugAvgCorOut    = -2.9697652;
//   Double_t tSlugAvgCorIn     = -0.4799399;
//   Double_t etSlugAvgCorOut     = 0.3047041;
//   Double_t etSlugAvgCorIn      = 0.3614205;

//   Double_t tSlugAvgCorInpOut    = -1.7249;
//   Double_t tSlugAvgCorInmOut     = 1.5365;
//   Double_t etSlugAvgCorInpOut     = 0.2364;
//   Double_t etSlugAvgCorInmOut      = 0.2330;



//   Double_t tUnRegInpOut = -0.7369; 
//   Double_t tUnRegInmOut = 1.4678;
//   Double_t etUnRegInOut = 0.23259621;
//   Double_t etUnRegInpOut = 0.2326;
//   Double_t etUnRegInmOut = 0.2292;
//   Double_t tRegInpOut   = -1.7093;
//   Double_t tRegInmOut   = 1.5289;
//   Double_t etRegInOut   = 0.236362715;
//   Double_t etRegInpOut = 0.2364;
//   Double_t etRegInmOut = 0.2330;
//   Double_t tRunletCorInpOut   = -1.73;
//   Double_t tRunletCorInmOut   = 1.56;
//   Double_t etRunletCorInOut   = 0.240351129579875367;
//   Double_t etRunletCorInpOut   = 0.24;
//   Double_t etRunletCorInmOut   = 0.24;

//   Double_t tJLUnRegOut  = -2.03604;
//   Double_t tJLRegOut    = -2.84229;
//   Double_t tJLUnRegIn   = 0.867209;
//   Double_t tJLRegIn     = 0.0192177;
//   Double_t tJLUnRegInpOut  = -0.59;
//   Double_t tJLUnRegInmOut  = 1.49;
//   Double_t tJLRegInpOut    = -1.41;
//   Double_t tJLRegInmOut    = 1.52;
//   Double_t etJLUnRegOut = 0.295501193161952;
//   Double_t etJLRegOut   = 0.295312603936708;
//   Double_t etJLUnRegIn  = 0.316939747804492;
//   Double_t etJLRegIn    = 0.316734632593579;
//   Double_t etJLUnRegInpOut = 0.22;
//   Double_t etJLUnRegInmOut = 0.22;
//   Double_t etJLRegInpOut = 0.22;
//   Double_t etJLRegInmOut = 0.22;

  /***********************************************/
  TCanvas *c5 = new TCanvas("c5","c5",cx1,cy1,800,600);
  TPad * pad50 = new TPad("pad50","pad50",ps1,ps2,ps4,ps4);
  TPad * pad51 = new TPad("pad51","pad51",ps1,ps1,ps4,ps3);
  pad50->Draw();
  pad51->Draw();
  pad50->cd();
  TText * ct50 = new TText(tll,tlr,Form("%sN to Delta %s %s Aymmetry",regTitle,dataInfo,plotTitle));
  ct50->SetTextSize(tsiz3);
  ct50->Draw();
  pad51->cd();
  /***********************************************/

  TGraphErrors *grUnRegOut = new TGraphErrors(sNUM,x10,yUnRegOut,ex,eyUnRegOut);
  grUnRegOut->SetName("grUnRegOut");
  grUnRegOut->SetMarkerColor(kBlack);
  grUnRegOut->SetMarkerStyle(25);
  grUnRegOut->Draw("0");

  TGraphErrors *grUnRegIn = new TGraphErrors(sNUM,x11,yUnRegIn,ex,eyUnRegIn);
  grUnRegIn->SetName("grUnRegIn");
  grUnRegIn->SetMarkerColor(kRed);
  grUnRegIn->SetMarkerStyle(25);
  grUnRegIn->Draw("0");

  TGraphErrors *grUnRegInpOut = new TGraphErrors(sNUM,x12,yUnRegInpOut,ex,eyUnRegInpOut);
  grUnRegInpOut->SetName("grUnRegInpOut");
  grUnRegInpOut->SetMarkerColor(kRed+2);
  grUnRegInpOut->SetMarkerStyle(26);
  grUnRegInpOut->Draw("0");

  TGraphErrors *grUnRegInmOut = new TGraphErrors(sNUM,x13,yUnRegInmOut,ex,eyUnRegInmOut);
  grUnRegInmOut->SetName("grUnRegInmOut");
  grUnRegInmOut->SetMarkerColor(kRed-7);
  grUnRegInmOut->SetMarkerStyle(24);
  grUnRegInmOut->Draw("0");

  TGraphErrors *grRegOut = new TGraphErrors(sNUM,x10,yRegOut,ex,eyRegOut);
  grRegOut->SetName("grRegOut");
  grRegOut->SetMarkerColor(kBlack);
  grRegOut->SetMarkerStyle(21);
  grRegOut->Draw("0");

  TGraphErrors *grRegIn = new TGraphErrors(sNUM,x11,yRegIn,ex,eyRegIn);
  grRegIn->SetName("grRegIn");
  grRegIn->SetMarkerColor(kRed);
  grRegIn->SetMarkerStyle(21);
  grRegIn->Draw("0");

  TGraphErrors *grRegInpOut = new TGraphErrors(sNUM,x12,yRegInpOut,ex,eyRegInpOut);
  grRegInpOut->SetName("grRegInpOut");
  grRegInpOut->SetMarkerColor(kRed+2);
  grRegInpOut->SetMarkerStyle(22);
  grRegInpOut->Draw("0");

  TGraphErrors *grRegInmOut = new TGraphErrors(sNUM,x13,yRegInmOut,ex,eyRegInmOut);
  grRegInmOut->SetName("grRegInmOut");
  grRegInmOut->SetMarkerColor(kRed-7);
  grRegInmOut->SetMarkerStyle(20);
  grRegInmOut->Draw("0");

  /***********************************************/

  TGraphErrors *grJLUnRegOut = new TGraphErrors(sNUM,x10,yJLUnRegOut,ex,eyJLUnRegOut);
  grJLUnRegOut->SetName("grJLUnRegOut");
  grJLUnRegOut->SetMarkerColor(kGreen);
  grJLUnRegOut->SetMarkerStyle(25);
  grJLUnRegOut->Draw("0");

  TGraphErrors *grJLUnRegIn = new TGraphErrors(sNUM,x11,yJLUnRegIn,ex,eyJLUnRegIn);
  grJLUnRegIn->SetName("grJLUnRegIn");
  grJLUnRegIn->SetMarkerColor(kBlue);
  grJLUnRegIn->SetMarkerStyle(25);
  grJLUnRegIn->Draw("0");

  TGraphErrors *grJLUnRegInpOut = new TGraphErrors(sNUM,x12,yJLUnRegInpOut,ex,eyJLUnRegInpOut);
  grJLUnRegInpOut->SetName("grJLUnRegInpOut");
  grJLUnRegInpOut->SetMarkerColor(kCyan+2);
  grJLUnRegInpOut->SetMarkerStyle(26);
  grJLUnRegInpOut->Draw("0");

  TGraphErrors *grJLUnRegInmOut = new TGraphErrors(sNUM,x13,yJLUnRegInmOut,ex,eyJLUnRegInmOut);
  grJLUnRegInmOut->SetName("grJLUnRegInmOut");
  grJLUnRegInmOut->SetMarkerColor(kCyan-7);
  grJLUnRegInmOut->SetMarkerStyle(24);
  grJLUnRegInmOut->Draw("0");

  TGraphErrors *grJLRegOut = new TGraphErrors(sNUM,x10,yJLRegOut,ex,eyJLRegOut);
  grJLRegOut->SetName("grJLRegOut");
  grJLRegOut->SetMarkerColor(kGreen);
  grJLRegOut->SetMarkerStyle(21);
  grJLRegOut->Draw("0");

  TGraphErrors *grJLRegIn = new TGraphErrors(sNUM,x11,yJLRegIn,ex,eyJLRegIn);
  grJLRegIn->SetName("grJLRegIn");
  grJLRegIn->SetMarkerColor(kBlue);
  grJLRegIn->SetMarkerStyle(21);
  grJLRegIn->Draw("0");

  TGraphErrors *grJLRegInpOut = new TGraphErrors(sNUM,x12,yJLRegInpOut,ex,eyJLRegInpOut);
  grJLRegInpOut->SetName("grJLRegInpOut");
  grJLRegInpOut->SetMarkerColor(kCyan+2);
  grJLRegInpOut->SetMarkerStyle(22);
  grJLRegInpOut->Draw("0");

  TGraphErrors *grJLRegInmOut = new TGraphErrors(sNUM,x13,yJLRegInmOut,ex,eyJLRegInmOut);
  grJLRegInmOut->SetName("grJLRegInmOut");
  grJLRegInmOut->SetMarkerColor(kCyan-7);
  grJLRegInmOut->SetMarkerStyle(20);
  grJLRegInmOut->Draw("0");

  /***********************************************/

  TGraphErrors *grRunletCorOut = new TGraphErrors(sNUM,x1,yRunletCorOut,ex,eyRunletCorOut);
  grRunletCorOut->SetName("grRunletCorOut");
  grRunletCorOut->SetMarkerColor(kGray);
  grRunletCorOut->SetMarkerStyle(21);
  grRunletCorOut->Draw("0");

  TGraphErrors *grRunletCorIn = new TGraphErrors(sNUM,x2,yRunletCorIn,ex,eyRunletCorIn);
  grRunletCorIn->SetName("grRunletCorIn");
  grRunletCorIn->SetMarkerColor(kOrange);
  grRunletCorIn->SetMarkerStyle(21);
  grRunletCorIn->Draw("0");

  TGraphErrors *grRunletCorInpOut = new TGraphErrors(sNUM,x3,yRunletCorInpOut,ex,eyRunletCorInpOut);
  grRunletCorInpOut->SetName("grRunletCorInpOut");
  grRunletCorInpOut->SetMarkerColor(kOrange+8);
  grRunletCorInpOut->SetMarkerStyle(22);
  grRunletCorInpOut->Draw("0");

  TGraphErrors *grRunletCorInmOut = new TGraphErrors(sNUM,x4,yRunletCorInmOut,ex,eyRunletCorInmOut);
  grRunletCorInmOut->SetName("grRunletCorInmOut");
  grRunletCorInmOut->SetMarkerColor(kOrange-4);
  grRunletCorInmOut->SetMarkerStyle(20);
  grRunletCorInmOut->Draw("0");

  /***********************************************/

  TGraphErrors *grSlugCorOut = new TGraphErrors(sNUM,x1,ySlugCorOut,ex,eySlugCorOut);
  grSlugCorOut->SetName("grSlugCorOut");
  grSlugCorOut->SetMarkerColor(kCyan+2);
  grSlugCorOut->SetMarkerStyle(21);
  grSlugCorOut->Draw("0");

  TGraphErrors *grSlugCorIn = new TGraphErrors(sNUM,x2,ySlugCorIn,ex,eySlugCorIn);
  grSlugCorIn->SetName("grSlugCorIn");
  grSlugCorIn->SetMarkerColor(kMagenta);
  grSlugCorIn->SetMarkerStyle(21);
  grSlugCorIn->Draw("0");

  TGraphErrors *grSlugCorInpOut = new TGraphErrors(sNUM,x3,ySlugCorInpOut,ex,eySlugCorInpOut);
  grSlugCorInpOut->SetName("grSlugCorInpOut");
  grSlugCorInpOut->SetMarkerColor(kMagenta+2);
  grSlugCorInpOut->SetMarkerStyle(22);
  grSlugCorInpOut->Draw("0");

  TGraphErrors *grSlugCorInmOut = new TGraphErrors(sNUM,x4,ySlugCorInmOut,ex,eySlugCorInmOut);
  grSlugCorInmOut->SetName("grSlugCorInmOut");
  grSlugCorInmOut->SetMarkerColor(kMagenta-9);
  grSlugCorInmOut->SetMarkerStyle(20);
  grSlugCorInmOut->Draw("0");

  /***********************************************/

  //  TGraphErrors *grSlugAvgCorOut = new TGraphErrors(sNUM,x20,ySlugAvgCorOut,ex,eySlugAvgCorOut);
  TGraphErrors *grSlugAvgCorOut = new TGraphErrors(sNUM,x1,ySlugAvgCorOut,ex,eySlugAvgCorOut);
  grSlugAvgCorOut->SetName("grSlugAvgCorOut");
  grSlugAvgCorOut->SetMarkerColor(kBlue-9);
  grSlugAvgCorOut->SetMarkerStyle(21);
  grSlugAvgCorOut->Draw("0");

  //  TGraphErrors *grSlugAvgCorIn = new TGraphErrors(sNUM,x21,ySlugAvgCorIn,ex,eySlugAvgCorIn);
  TGraphErrors *grSlugAvgCorIn = new TGraphErrors(sNUM,x2,ySlugAvgCorIn,ex,eySlugAvgCorIn);
  grSlugAvgCorIn->SetName("grSlugAvgCorIn");
  grSlugAvgCorIn->SetMarkerColor(kGreen);
  grSlugAvgCorIn->SetMarkerStyle(21);
  grSlugAvgCorIn->Draw("0");

  //  TGraphErrors *grSlugAvgCorInpOut = new TGraphErrors(sNUM,x22,ySlugAvgCorInpOut,ex,eySlugAvgCorInpOut);
  TGraphErrors *grSlugAvgCorInpOut = new TGraphErrors(sNUM,x3,ySlugAvgCorInpOut,ex,eySlugAvgCorInpOut);
  grSlugAvgCorInpOut->SetName("grSlugAvgCorInpOut");
  grSlugAvgCorInpOut->SetMarkerColor(kGreen+2);
  grSlugAvgCorInpOut->SetMarkerStyle(22);
  grSlugAvgCorInpOut->Draw("0");

  //  TGraphErrors *grSlugAvgCorInmOut = new TGraphErrors(sNUM,x23,ySlugAvgCorInmOut,ex,eySlugAvgCorInmOut);
  TGraphErrors *grSlugAvgCorInmOut = new TGraphErrors(sNUM,x4,ySlugAvgCorInmOut,ex,eySlugAvgCorInmOut);
  grSlugAvgCorInmOut->SetName("grSlugAvgCorInmOut");
  grSlugAvgCorInmOut->SetMarkerColor(kGreen-9);
  grSlugAvgCorInmOut->SetMarkerStyle(20);
  grSlugAvgCorInmOut->Draw("0");



  /***********************************************/

  TMultiGraph *detGraph = new TMultiGraph();
  detGraph->Add(grUnRegOut);
  detGraph->Add(grUnRegIn);
  detGraph->Add(grUnRegInpOut);
  detGraph->Add(grUnRegInmOut);
  detGraph->Add(grRegOut);
  detGraph->Add(grRegIn);
  detGraph->Add(grRegInpOut);
  detGraph->Add(grRegInmOut);
//   detGraph->Add(grJLUnRegOut);
//   detGraph->Add(grJLUnRegIn);
//   detGraph->Add(grJLUnRegInpOut);
//   detGraph->Add(grJLUnRegInmOut);
//   detGraph->Add(grJLRegOut);
//   detGraph->Add(grJLRegIn);
//   detGraph->Add(grJLRegInpOut);
//   detGraph->Add(grJLRegInmOut);

//   detGraph->Add(grRunletCorOut);
//   detGraph->Add(grRunletCorIn);
//   detGraph->Add(grRunletCorInpOut);
//   detGraph->Add(grRunletCorInmOut);

  detGraph->Add(grSlugCorOut);
  detGraph->Add(grSlugCorIn);
  detGraph->Add(grSlugCorInpOut);
  detGraph->Add(grSlugCorInmOut);
//   detGraph->Add(grSlugAvgCorOut);
//   detGraph->Add(grSlugAvgCorIn);
//   detGraph->Add(grSlugAvgCorInpOut);
//   detGraph->Add(grSlugAvgCorInmOut);

  detGraph->Draw("AP");
  detGraph->SetTitle("");
//   detGraph->GetXaxis()->SetTitle("Slug Number");
  detGraph->GetYaxis()->SetTitle(Form("%s Asym [ppm]",plotTitle));
  detGraph->GetXaxis()->CenterTitle();
  detGraph->GetYaxis()->CenterTitle();
  detGraph->GetXaxis()->SetNdivisions(5,0,0);
  detGraph->GetYaxis()->SetRangeUser(-4,4);
  TAxis *xaxisDetGraphReg1= detGraph->GetXaxis();
  xaxisDetGraphReg1->SetLimits(0.2,4.7);
  detGraph->GetXaxis()->SetLabelColor(0);

  /***********************************************/
  TLegend *legJLUnRegDet = new TLegend(0.10,0.77,0.30,0.98);
  legJLUnRegDet->AddEntry("grJLUnRegOut","JL Un-Reg Out","lp");
  legJLUnRegDet->AddEntry("grJLUnRegIn","JL Un-Reg In","lp");
  legJLUnRegDet->AddEntry("grJLUnRegInpOut","JL Un-Reg #frac{In+Out}{2}","lp");
  legJLUnRegDet->AddEntry("grJLUnRegInmOut","JL Un-Reg #frac{In-Out}{2}","lp");
  legJLUnRegDet->SetTextSize(0.028);
  legJLUnRegDet->SetFillColor(0);
  legJLUnRegDet->SetBorderSize(2);

  TLegend *legJLRegDet = new TLegend(0.30,0.77,0.49,0.98);
  legJLRegDet->AddEntry("grJLRegOut","JL Reg Out","lp");
  legJLRegDet->AddEntry("grJLRegIn","JL Reg In","lp");
  legJLRegDet->AddEntry("grJLRegInpOut","JL Reg #frac{In+Out}{2}","lp");
  legJLRegDet->AddEntry("grJLRegInmOut","JL Reg #frac{In-Out}{2}","lp");
  legJLRegDet->SetTextSize(0.028);
  legJLRegDet->SetFillColor(0);
  legJLRegDet->SetBorderSize(2);
  /***********************************************/
  TLegend *legDet = new TLegend(0.10,0.77,0.27,0.98);
  legDet->AddEntry("grUnRegOut","Un-Reg Out","lp");
  legDet->AddEntry("grUnRegIn","Un-Reg In","lp");
  legDet->AddEntry("grUnRegInpOut","Un-Reg #frac{In+Out}{2}","lp");
  legDet->AddEntry("grUnRegInmOut","Un-Reg #frac{In-Out}{2}","lp");
  legDet->SetTextSize(0.028);
  legDet->SetFillColor(0);
  legDet->SetBorderSize(2);

  TLegend *legRegDet = new TLegend(0.27,0.77,0.39,0.98);
  legRegDet->AddEntry("grRegOut","Reg Out","lp");
  legRegDet->AddEntry("grRegIn","Reg In","lp");
  legRegDet->AddEntry("grRegInpOut","Reg #frac{In+Out}{2}","lp");
  legRegDet->AddEntry("grRegInmOut","Reg #frac{In-Out}{2}","lp");
  legRegDet->SetTextSize(0.028);
  legRegDet->SetFillColor(0);
  legRegDet->SetBorderSize(2);
  /***********************************************/
  TLegend *legRunletCorDet = new TLegend(0.39,0.77,0.59,0.98);
  legRunletCorDet->AddEntry("grRunletCorOut","Runlet Cor Out","lp");
  legRunletCorDet->AddEntry("grRunletCorIn","Runlet Cor In","lp");
  legRunletCorDet->AddEntry("grRunletCorInpOut","Runlet Cor #frac{In+Out}{2}","lp");
  legRunletCorDet->AddEntry("grRunletCorInmOut","Runlet Cor #frac{In-Out}{2}","lp");
  legRunletCorDet->SetTextSize(0.028);
  legRunletCorDet->SetFillColor(0);
  legRunletCorDet->SetBorderSize(2);

  /***********************************************/
//   TLegend *legSlugCorDet = new TLegend(0.59,0.77,0.77,0.98);
  TLegend *legSlugCorDet = new TLegend(0.39,0.77,0.62,0.98);
  legSlugCorDet->AddEntry("grSlugCorOut","Slug Cor Out","lp");
  legSlugCorDet->AddEntry("grSlugCorIn","Slug Cor In","lp");
  legSlugCorDet->AddEntry("grSlugCorInpOut","Slug Cor #frac{In+Out}{2}","lp");
  legSlugCorDet->AddEntry("grSlugCorInmOut","Slug Cor #frac{In-Out}{2}","lp");
  legSlugCorDet->SetTextSize(0.028);
  legSlugCorDet->SetFillColor(0);
  legSlugCorDet->SetBorderSize(2);

  /***********************************************/
  //  TLegend *legSlugAvgCorDet = new TLegend(0.77,0.77,0.99,0.98);
  TLegend *legSlugAvgCorDet = new TLegend(0.39,0.77,0.62,0.98);
  legSlugAvgCorDet->AddEntry("grSlugAvgCorOut","SlugAvg Cor Out","lp");
  legSlugAvgCorDet->AddEntry("grSlugAvgCorIn","SlugAvg Cor In","lp");
  legSlugAvgCorDet->AddEntry("grSlugAvgCorInpOut","SlugAvg Cor #frac{In+Out}{2}","lp");
  legSlugAvgCorDet->AddEntry("grSlugAvgCorInmOut","SlugAvg Cor #frac{In-Out}{2}","lp");
  legSlugAvgCorDet->SetTextSize(0.028);
  legSlugAvgCorDet->SetFillColor(0);
  legSlugAvgCorDet->SetBorderSize(2);

//   legJLRegDet->Draw();
//   legJLUnRegDet->Draw();
  legDet->Draw();
  legRegDet->Draw();
//   legRunletCorDet->Draw();
  legSlugCorDet->Draw();
//   legSlugAvgCorDet->Draw();
  /***********************************************/

  TLatex* textUnRegOut=new TLatex(tx10-0.05,yUnRegOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yUnRegOut[sCounter-1],eyUnRegOut[sCounter-1]));
  textUnRegOut->SetTextSize(0.03);
  textUnRegOut->SetTextColor(kBlack);
  TLatex* textUnRegIn=new TLatex(tx11-0.05,yUnRegIn[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yUnRegIn[sCounter-1],eyUnRegIn[sCounter-1]));
  textUnRegIn->SetTextSize(0.03);
  textUnRegIn->SetTextColor(kRed);
  TLatex* textUnRegInpOut=new TLatex(tx12-0.05,yUnRegInpOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yUnRegInpOut[sCounter-1],eyUnRegInpOut[sCounter-1]));
  textUnRegInpOut->SetTextSize(0.03);
  textUnRegInpOut->SetTextColor(kRed+2);
  TLatex* textUnRegInmOut=new TLatex(tx13-0.05,yUnRegInmOut[sCounter-1]-0.2,Form("%4.2f#pm%4.2f",yUnRegInmOut[sCounter-1],eyUnRegInmOut[sCounter-1]));
  textUnRegInmOut->SetTextSize(0.03);
  textUnRegInmOut->SetTextColor(kRed-7);
  TLatex* textRegOut=new TLatex(tx10-0.05,yRegOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yRegOut[sCounter-1],eyRegOut[sCounter-1]));
  textRegOut->SetTextSize(0.03);
  textRegOut->SetTextColor(kBlack);
  TLatex* textRegIn=new TLatex(tx11-0.05,yRegIn[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yRegIn[sCounter-1],eyRegIn[sCounter-1]));
  textRegIn->SetTextSize(0.03);
  textRegIn->SetTextColor(kRed);
  TLatex* textRegInpOut=new TLatex(tx12-0.05,yRegInpOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yRegInpOut[sCounter-1],eyRegInpOut[sCounter-1]));
  textRegInpOut->SetTextSize(0.03);
  textRegInpOut->SetTextColor(kRed+2);
  TLatex* textRegInmOut=new TLatex(tx13-0.05,yRegInmOut[sCounter-1]+0.1,Form("%4.2f#pm%4.2f",yRegInmOut[sCounter-1],eyRegInmOut[sCounter-1]));
  textRegInmOut->SetTextSize(0.03);
  textRegInmOut->SetTextColor(kRed-7);

  TLatex* textJLUnRegOut=new TLatex(tx10,yJLUnRegOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yJLUnRegOut[sCounter-1],eyJLUnRegOut[sCounter-1]));
  textJLUnRegOut->SetTextSize(0.03);
  textJLUnRegOut->SetTextColor(kGreen);
  TLatex* textJLUnRegIn=new TLatex(tx11,yJLUnRegIn[sCounter-1]-0.10,Form("%4.2f#pm%4.2f",yJLUnRegIn[sCounter-1],eyJLUnRegIn[sCounter-1]));
  textJLUnRegIn->SetTextSize(0.03);
  textJLUnRegIn->SetTextColor(kBlue);
  TLatex* textJLUnRegInpOut=new TLatex(tx12,yJLUnRegInpOut[sCounter-1]-0.35,Form("%4.2f#pm%4.2f",yJLUnRegInpOut[sCounter-1],eyJLUnRegInpOut[sCounter-1]));
  textJLUnRegInpOut->SetTextSize(0.03);
  textJLUnRegInpOut->SetTextColor(kCyan+2);
  TLatex* textJLUnRegInmOut=new TLatex(tx13,yJLUnRegInmOut[sCounter-1]+0.05,Form("%4.2f#pm%4.2f",yJLUnRegInmOut[sCounter-1],eyJLUnRegInmOut[sCounter-1]));
  textJLUnRegInmOut->SetTextSize(0.03);
  textJLUnRegInmOut->SetTextColor(kCyan-7);
  TLatex* textJLRegOut=new TLatex(tx10,yJLRegOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yJLRegOut[sCounter-1],eyJLRegOut[sCounter-1]));
  textJLRegOut->SetTextSize(0.03);
  textJLRegOut->SetTextColor(kGreen);
  TLatex* textJLRegIn=new TLatex(tx11,yJLRegIn[sCounter-1]+0.05,Form("%4.2f#pm%4.2f",yJLRegIn[sCounter-1],eyJLRegIn[sCounter-1]));
  textJLRegIn->SetTextSize(0.03);
  textJLRegIn->SetTextColor(kBlue);
  TLatex* textJLRegInpOut=new TLatex(tx12,yJLRegInpOut[sCounter-1]-0.1,Form("%4.2f#pm%4.2f",yJLRegInpOut[sCounter-1],eyJLRegInpOut[sCounter-1]));
  textJLRegInpOut->SetTextSize(0.03);
  textJLRegInpOut->SetTextColor(kCyan+2);
  TLatex* textJLRegInmOut=new TLatex(tx13,yJLRegInmOut[sCounter-1]-0.20,Form("%4.2f#pm%4.2f",yJLRegInmOut[sCounter-1],eyJLRegInmOut[sCounter-1]));
  textJLRegInmOut->SetTextSize(0.03);
  textJLRegInmOut->SetTextColor(kCyan-7);


  TLatex* textRunletCorOut=new TLatex(tx1,yRunletCorOut[sCounter-1]-0.4,Form("%4.2f#pm%4.2f",yRunletCorOut[sCounter-1],eyRunletCorOut[sCounter-1]));
  textRunletCorOut->SetTextSize(0.03);
  textRunletCorOut->SetTextColor(kGray);
  TLatex* textRunletCorIn=new TLatex(tx2,yRunletCorIn[sCounter-1]-0.20,Form("%4.2f#pm%4.2f",yRunletCorIn[sCounter-1],eyRunletCorIn[sCounter-1]));
  textRunletCorIn->SetTextSize(0.03);
  textRunletCorIn->SetTextColor(kOrange);
  TLatex* textRunletCorInpOut=new TLatex(tx3,yRunletCorInpOut[sCounter-1]-0.4,Form("%4.2f#pm%4.2f",yRunletCorInpOut[sCounter-1],eyRunletCorInpOut[sCounter-1]));
  textRunletCorInpOut->SetTextSize(0.03);
  textRunletCorInpOut->SetTextColor(kOrange+8);
  TLatex* textRunletCorInmOut=new TLatex(tx4,yRunletCorInmOut[sCounter-1]-0.65,Form("%4.2f#pm%4.2f",yRunletCorInmOut[sCounter-1],eyRunletCorInmOut[sCounter-1]));
  textRunletCorInmOut->SetTextSize(0.03);
  textRunletCorInmOut->SetTextColor(kOrange-4);

  TLatex* textSlugCorOut=new TLatex(tx1-0.08,ySlugCorOut[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugCorOut[sCounter-1],eySlugCorOut[sCounter-1]));
  textSlugCorOut->SetTextSize(0.03);
  textSlugCorOut->SetTextColor(kCyan+2);
  TLatex* textSlugCorIn=new TLatex(tx2-0.08,ySlugCorIn[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugCorIn[sCounter-1],eySlugCorIn[sCounter-1]));
  textSlugCorIn->SetTextSize(0.03);
  textSlugCorIn->SetTextColor(kMagenta);
  TLatex* textSlugCorInpOut=new TLatex(tx3-0.08,ySlugCorInpOut[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugCorInpOut[sCounter-1],eySlugCorInpOut[sCounter-1]));
  textSlugCorInpOut->SetTextSize(0.03);
  textSlugCorInpOut->SetTextColor(kMagenta+2);
  TLatex* textSlugCorInmOut=new TLatex(tx4-0.08,ySlugCorInmOut[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugCorInmOut[sCounter-1],eySlugCorInmOut[sCounter-1]));
  textSlugCorInmOut->SetTextSize(0.03);
  textSlugCorInmOut->SetTextColor(kMagenta-9);

  TLatex* textSlugAvgCorOut=new TLatex(tx1-0.08,ySlugAvgCorOut[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugAvgCorOut[sCounter-1],eySlugAvgCorOut[sCounter-1]));
  textSlugAvgCorOut->SetTextSize(0.03);
  textSlugAvgCorOut->SetTextColor(kBlue-9);
  TLatex* textSlugAvgCorIn=new TLatex(tx2-0.08,ySlugAvgCorIn[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugAvgCorIn[sCounter-1],eySlugAvgCorIn[sCounter-1]));
  textSlugAvgCorIn->SetTextSize(0.03);
  textSlugAvgCorIn->SetTextColor(kGreen);
  TLatex* textSlugAvgCorInpOut=new TLatex(tx3-0.08,ySlugAvgCorInpOut[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugAvgCorInpOut[sCounter-1],eySlugAvgCorInpOut[sCounter-1]));
  textSlugAvgCorInpOut->SetTextSize(0.03);
  textSlugAvgCorInpOut->SetTextColor(kGreen+2);
  TLatex* textSlugAvgCorInmOut=new TLatex(tx4-0.08,ySlugAvgCorInmOut[sCounter-1]-0.08,Form("%4.2f#pm%4.2f",ySlugAvgCorInmOut[sCounter-1],eySlugAvgCorInmOut[sCounter-1]));
  textSlugAvgCorInmOut->SetTextSize(0.03);
  textSlugAvgCorInmOut->SetTextColor(kGreen-9);


  textUnRegOut->Draw();
  textUnRegIn->Draw();
  textUnRegInpOut->Draw();
  textUnRegInmOut->Draw();
  textRegOut->Draw();
  textRegIn->Draw();
  textRegInpOut->Draw();
  textRegInmOut->Draw();

//   textJLUnRegOut->Draw();
//   textJLUnRegIn->Draw();
//   textJLUnRegInpOut->Draw();
//   textJLUnRegInmOut->Draw();
//   textJLRegOut->Draw();
//   textJLRegIn->Draw();
//   textJLRegInpOut->Draw();
//   textJLRegInmOut->Draw();

//   textRunletCorOut->Draw();
//   textRunletCorIn->Draw();
//   textRunletCorInpOut->Draw();
//   textRunletCorInmOut->Draw();

  textSlugCorOut->Draw();
  textSlugCorIn->Draw();
  textSlugCorInpOut->Draw();
  textSlugCorInmOut->Draw();

//   textSlugAvgCorOut->Draw();
//   textSlugAvgCorIn->Draw();
//   textSlugAvgCorInpOut->Draw();
//   textSlugAvgCorInmOut->Draw();

  /***********************************************/

  TLatex* text1=new TLatex(0.85,-4.6,Form("Out,R"));
  text1->SetTextSize(0.04);
  TLatex* text2=new TLatex(1.90,-4.6,Form("In,R"));
  text2->SetTextSize(0.04);
  TLatex* text3=new TLatex(2.85,-4.6,Form("#frac{In+Out}{2}"));
  text3->SetTextSize(0.04);
  TLatex* text4=new TLatex(3.92,-4.6,Form("A_{m}"));
  text4->SetTextSize(0.04);

  text1->Draw();
  text2->Draw();
  text3->Draw();
  text4->Draw();

  gStyle->SetLineStyle(1);
  gStyle->SetLineWidth(3);
  gStyle->SetLineColor(kBlack);
  TLine * l = new TLine(0.2, 0, 4.7, 0); 
  l->SetLineStyle(3);
  l->Draw();
  gStyle->SetLineStyle(1);
  gStyle->SetLineWidth(1); 

  gPad->Update();
  c5->Update();
  c5->SaveAs(Form("%s/plots/%s/%sSummaryPlot%s.png",n2deltaAnalysisDir.Data(),regScheme,savePlot,regScheme));


  }

  /*********************************************************************************/
  /*********************************************************************************/
  /*********************************************************************************/




  }
