#include "../include/headers.h"
#include <mysql++.h>
#include "../include/QwDBConnection.hh"
#include "../include/QwDataContainer.hh"

Int_t main(Int_t argc, Char_t *argv[])
{

  //   TApplication theApp("App", &argc, argv);
  QwDataContainer data;
  
  data.GetOptions(argv);
  
  data.ConnectDB();
  std::cout << "Using database to build plots." << std::endl;

  //        data.GetDBSensitivities("qwk_mdallbars", "in", "sens_all");
  data.PlotDBSensitivities();
  std::cout << "Done with analysis." << std::endl;
  //     theApp.Run();
  return 0;
}
