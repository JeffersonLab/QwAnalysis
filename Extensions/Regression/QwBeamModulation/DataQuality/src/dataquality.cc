#include "headers.h"
#include "QwDataQuality.hh"

Int_t main(Int_t argc, Char_t *argv[])
{
  // TApplication theApp("App", &argc, argv);

  TString filename;

  TChain *tree = new TChain("tree");

  QwDataQuality *qwdata = new QwDataQuality(tree);

  if(!argv[1]){
    std::cerr << "Must specify filename.  Exiting." << std::endl;
    exit(1);
  }

  filename = argv[1];

  std::cout << "Opening: " << filename << std::endl;

  TString file_directory =  gSystem->Getenv("QW_ROOTFILES");

  if(!(gSystem->OpenDirectory(file_directory))){
    std::cerr << "You must set $QW_ROOTFILES enviroment variable. Exiting." << std::endl;
    exit(1);
  }
  qwdata->SetRunPeriod("Run1");
  qwdata->LoadRootFile(filename, tree);

  /*
  // *********************************
  //           BCM DD Plots
  // *********************************

  // BCMDD12 central value vs runlet

  qwdata->SetTitle("BCM12DD by Runlet");
  qwdata->SetXaxisTitle("runlet");
  qwdata->SetYaxisTitle("bcmdd12");
  qwdata->SetSignCorr(true);
  qwdata->SetOutputFilename("bcmdd12_central_value_vs_runlet");
  qwdata->PlotByWien("diff_bcmdd12/value");
  qwdata->ClearVectors();

  // BCMDD12 Histogram by Wien

  qwdata->SetTitle("BCM12DD Histo by Wien");
  qwdata->SetXaxisTitle("bcm12dd");
  qwdata->SetYaxisTitle("counts");
  qwdata->SetSignCorr(true);
  qwdata->SetOutputFilename("bcmdd12_histo_by_wien");
  qwdata->HistoByWien("diff_bcmdd12/value");
  qwdata->ClearVectors();
  
  // BCMDD12 Pull by Wien

  qwdata->SetTitle("BCM12DD Pull by Wien");
  qwdata->SetXaxisTitle("bcm12dd");
  qwdata->SetYaxisTitle("counts");
  qwdata->SetSignCorr(true);
  qwdata->SetOutputFilename("bcmdd12_pull_by_wien");
  qwdata->PullByWien("diff_bcmdd12/value");
  qwdata->ClearVectors();

  // BCMDD12 RMS by runlet

  qwdata->SetTitle("BCM12DD RMS by Runlet");
  qwdata->SetXaxisTitle("runlet");
  qwdata->SetYaxisTitle("bcm12dd rms");
  qwdata->SetSignCorr(false);
  qwdata->SetOutputFilename("bcmdd12_rms_by_runlet");
  qwdata->PlotByWien("diff_bcmdd12/rms");
  qwdata->ClearVectors();

  // BCMDD12 RMS Histo by Wien

  qwdata->SetTitle("BCM12DD RMS by Wien");
  qwdata->SetXaxisTitle("bcm12dd rms");
  qwdata->SetYaxisTitle("counts");
  qwdata->SetOutputFilename("bcmdd12_rms_by_wien");
  qwdata->SetSignCorr(false);
  qwdata->SetXmaximum(400);
  qwdata->SetXminimum(-100);
  qwdata->HistoByWien("diff_bcmdd12/rms");
  qwdata->ClearVectors();

  // Setting bounds back to default
  qwdata->SetXmaximum(5);
  qwdata->SetXminimum(-5);

  // BCMDD12 NULL Test 1

  qwdata->SetTitle("BCM12DD Weighted by Error");
  qwdata->SetXaxisTitle("bcm12dd weighted by bcm12dd error");
  qwdata->SetYaxisTitle("counts");
  qwdata->SetSignCorr(true);
  qwdata->SetOutputFilename("bcmdd12_null_test_1");
  qwdata->HistoByWien("diff_bcmdd12/value", "diff_bcmdd12/err");
  qwdata->ClearVectors();

  // BCMDD12 NULL Test 2

  qwdata->SetTitle("BCM12DD Weighted by MDALLPMTAVG Error");
  qwdata->SetXaxisTitle("bcm12dd weighted by qwk_mdallpmtavg");
  qwdata->SetYaxisTitle("counts");
  qwdata->SetSignCorr(true);
  qwdata->SetOutputFilename("bcmdd12_null_test_2");
  qwdata->HistoByWien("diff_bcmdd12/value", "asym_mdallpmtavg/err");
  qwdata->ClearVectors();


  // ***********************************
  //     BCMDD12 vs MDALLPMTAVG
  // ***********************************

  std::vector <Double_t> detector;
  std::vector <Double_t> monitor;
  std::vector <Double_t> detector_error;
  std::vector <Double_t> monitor_error;

  qwdata->FillDataVector(detector, "asym_mdallpmtavg/value");
  qwdata->FillDataVector(monitor, "diff_bcmdd12/value");
  qwdata->FillDataVector(detector_error, "asym_mdallpmtavg/err");
  qwdata->FillDataVector(monitor_error, "diff_bcmdd12/err");

  qwdata->SetTitle("Correlation of BCMDD12 with MDALLPMTAVG");
  qwdata->SetXaxisTitle("MDALLPMTAVG");
  qwdata->SetYaxisTitle("BCMDD12");
  qwdata->SetSignCorr(true);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("bcmdd12_vs_asym_mdallpmtavg");
  qwdata->PlotCorrelation(detector, monitor, detector_error, monitor_error);
  */

  // ********************************
  //          BPM Data Plots
  // ********************************

  // Target X Position Difference

  qwdata->SetTitle("Target X Beam Position Difference");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("Target X Difference");
  qwdata->SetSignCorr(true);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("target_x_position_difference_w_stat_error");
  qwdata->PlotByWien("diff_qwk_targetX/value");
  qwdata->ClearVectors();
  /*
  // Target Y Position Difference

  qwdata->SetTitle("Target Y Beam Position Difference");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("Target Y Difference");
  qwdata->SetSignCorr(true);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("target_y_position_difference_w_stat_error");
  qwdata->PlotByWien("diff_qwk_targetY/value");
  qwdata->ClearVectors();

  // // Target X' Position Difference

  // qwdata->SetTitle("Target X Slope Beam Position Difference");
  // qwdata->SetXaxisTitle("runlets");
  // qwdata->SetYaxisTitle("Target X Slope Difference");
  // qwdata->SetSignCorr(true);
  // qwdata->SetStatisticalError(true);
  // qwdata->SetOutputFilename("target_xp_position_difference_w_stat_error");
  // qwdata->PlotByWien("diff_qwk_targetXSlope/value");
  // qwdata->ClearVectors();

  // // Target Y' Position Difference

  // qwdata->SetTitle("Target Y Slope Beam Position Difference");
  // qwdata->SetXaxisTitle("runlets");
  // qwdata->SetYaxisTitle("Target Y Slope Difference");
  // qwdata->SetSignCorr(true);
  // qwdata->SetStatisticalError(true);
  // qwdata->SetOutputFilename("target_yp_position_difference_w_stat_error");
  // qwdata->PlotByWien("diff_qwk_targetYSlope/value");
  // qwdata->ClearVectors();

  // BPM3c12X Position Difference

  qwdata->SetTitle("BPM3C12X Beam Position Difference");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("BPM3C12X Difference");
  qwdata->SetSignCorr(true);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("bpm3c12X_position_difference_w_stat_error");
  qwdata->PlotByWien("diff_qwk_bpm3c12X/value");
  qwdata->ClearVectors();

  // Target X RMS

  qwdata->SetTitle("Target X Position Difference RMS");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("Target X Difference RMS");
  qwdata->SetOutputFilename("target_X_position_difference_rms");
  qwdata->PlotByWien("diff_qwk_targetX/rms");
  qwdata->ClearVectors();

  // Target Y RMS

  qwdata->SetTitle("Target Y Position Difference RMS");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("Target Y Difference RMS");
  qwdata->SetOutputFilename("target_Y_position_difference_rms");
  qwdata->PlotByWien("diff_qwk_targetY/rms");
  qwdata->ClearVectors();

  // BPM3C12X RMS

  qwdata->SetTitle("BPM3C12X Position Difference RMS");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("BPM3C12X Difference RMS");
  qwdata->SetOutputFilename("bpm3c12X_position_difference_rms");
  qwdata->PlotByWien("diff_qwk_bpm3c12X/rms");
  qwdata->ClearVectors();
  */
  // Target X no sign correction

  qwdata->SetTitle("Target X Beam Position Difference: no sign correction");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("Target X Difference");
  qwdata->SetSignCorr(false);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("target_x_position_difference_w_stat_error_no_sign");
  qwdata->PlotByWien("diff_qwk_targetX/value");
  qwdata->ClearVectors();
  /*
  // Target Y no sign correction

  qwdata->SetTitle("Target Y Beam Position Difference: no sign correction");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("Target Y Difference");
  qwdata->SetSignCorr(false);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("target_y_position_difference_w_stat_error_no_sign");
  qwdata->PlotByWien("diff_qwk_targetY/value");
  qwdata->ClearVectors();

  // BPM3c12X Position Difference no sign correction

  qwdata->SetTitle("BPM3C12X Beam Position Difference: no sign correction");
  qwdata->SetXaxisTitle("runlets");
  qwdata->SetYaxisTitle("BPM3C12X Difference");
  qwdata->SetSignCorr(false);
  qwdata->SetStatisticalError(true);
  qwdata->SetOutputFilename("bpm3c12X_position_difference_w_stat_error_no_sign");
  qwdata->PlotByWien("diff_qwk_bpm3c12X/value");
  qwdata->ClearVectors();

  */
  // theApp.Run();
  
  return 0;

}
