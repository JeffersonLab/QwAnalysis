#include <vector>

#include "QwLog.h"
#include "QwOptions.h"
#include "QwOptionsParity.h"
#include "QwDatabase.h"
//#include "QwSSQLS.h"

int main(int argc,char* argv[]) {
  // Setup screen and file logging
  gQwLog.InitLogFile("qwdb_test.log");
//  gQwLog.SetFileThreshold(QwLog::kDebug);
//  gQwLog.SetScreenThreshold(QwLog::kDebug);

  // Set up command line and file options processing
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwdatabase.conf");
  // Get errors about not recognizing options when use config file
  //gQwOptions.SetConfigFile("Parity/prminput/qwdatabase.conf");
  gQwOptions.DefineOptions();

  // Start testing
  QwDebug << "qwdb_test:  Hello there!" << QwLog::endl;

  gQwDatabase.Connect();
  QwMessage << "Database server version is " << gQwDatabase.GetServerVersion() << QwLog::endl;

  try {
    mysqlpp::Query query = gQwDatabase.Query("select * from run limit 10");
    std::vector<run> res;
    query.storein(res);

    QwMessage << "We have:" << QwLog::endl;
    std::vector<run>::iterator it;
    for (it = res.begin(); it != res.end(); ++it) {
      QwMessage << '\t' << it->run_id << '\t';
      QwMessage << it->run_number << QwLog::endl;
    }
    gQwDatabase.Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    return(-1);
  }

  const UInt_t runnum = 5;
  gQwDatabase.SetRunNumber(runnum);

}
