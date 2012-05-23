#include "../include/headers.h"
#include <mysql++.h>
#include "../include/QwDBConnection.hh"
#include "../include/QwDataContainer.hh"
#include "../include/QwPlotHelper.hh"

Int_t main(Int_t argc, Char_t *argv[])
{

  TApplication theApp("App", &argc, argv);

  QwDataContainer data_in;
  QwDataContainer data_out;

  QwPlotHelper sensx;
  QwPlotHelper asym;  

  data_in.GetOptions(argv);
  data_out.GetOptions(argv);

  std::cout << "Using database to build plots." << std::endl;

//   data_in.GetDBSensitivities("qwk_mdallbars", "in", "sens");
//   data_out.GetDBSensitivities("qwk_mdallbars", "out", "sens");  
  data_in.GetDBAsymmetry("qwk_mdallbars", "in", "corrected");
  data_out.GetDBAsymmetry("qwk_mdallbars", "out", "corrected");  

  std::cout << "Finished loading from database." << std::endl;
//   sensx.SetPlotTitle("x sensitivities", "Run Number", "Sensitivity (ppm/mm)");
//   sensx.InOutErrorPlotSens(data_in, data_out, QwDataContainer::var_x, "fit");

  asym.SetPlotTitle("asymmetry", "Run Number", "Asymmetry (ppm)");
  asym.InOutErrorPlotAsym(data_in, data_out, "fit");

//    asym.InOutErrorPlotAvAsym(data_in, data_out, "");

  std::cout << "Done with analysis." << std::endl;
  
  theApp.Run();
  return 0;
}
