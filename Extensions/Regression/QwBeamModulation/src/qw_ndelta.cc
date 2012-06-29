#include "../include/headers.h"
#include <mysql++.h>
#include "../include/QwDBConnection.hh"
#include "../include/QwDataContainer.hh"
#include "../include/QwPlotHelper.hh"

Int_t main(Int_t argc, Char_t *argv[])
{

  TApplication theApp("App", &argc, argv);

  QwDataContainer raw_data_in;
  QwDataContainer raw_data_out;
  QwDataContainer corr_data_in;
  QwDataContainer corr_data_out;

  QwPlotHelper sensx;
  QwPlotHelper asym;  

  raw_data_in.GetOptions(argv);
  raw_data_out.GetOptions(argv);
  corr_data_in.GetOptions(argv);
  corr_data_out.GetOptions(argv);

  std::cout << "Using database to build plots." << std::endl;

  raw_data_in.GetDBAsymmetry("qwk_mdallbars", "in", "uncorrected");
  raw_data_out.GetDBAsymmetry("qwk_mdallbars", "out", "uncorrected");  

//   corr_data_in.SetRunRange(16000, 17000);
//   corr_data_out.SetRunRange(16000, 17000);

  corr_data_in.GetDBAsymmetry("qwk_mdallbars", "in", "corrected");
  corr_data_out.GetDBAsymmetry("qwk_mdallbars", "out", "corrected");  

  std::cout << "Finished loading from database." << std::endl;
//   asym.SetPlotTitle("asymmetry", "Run Number", "Asymmetry (ppm)");
//   asym.InOutErrorPlotAsym(corr_data_in, corr_data_out, "fit");
  //  asym.InOutErrorPlotAsym(raw_data_in, raw_data_out, "fit");
  asym.InOutErrorPlotAvAsym(raw_data_in, raw_data_out, corr_data_in, corr_data_out, "fit");

  std::cout << "Done with analysis." << std::endl;
  
  theApp.Run();
  return 0;
}
