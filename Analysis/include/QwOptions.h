#ifndef QWOPTIONS_H
#define QWOPTIONS_H

// System headers
#include <cstdlib>
#include <string>
using std::string;

// Boost headers
#include <boost/program_options.hpp>
namespace po = boost::program_options;

/**
 @author Wouter Deconinck <wdconinc@mit.edu>

 On instantiation of the global gQwOptions object the argc and argv are passed.
 The filename is set to a default value, but on instantiation a check is done
 for the option --config filename.  After this no parsing is done without the
 user requesting it.

*/
class QwOptions{

  public:

    QwOptions(int argc, char* argv[], string configfile = "");
    ~QwOptions();

    void Usage();

    string GetStringValue(string key);
    bool GetBoolValue(string key);

    int GetIntValue(string key);
    std::pair<int,int> GetIntValuePair(string key);
    int GetIntValuePairFirst(string key) {
      return GetIntValuePair(key).first;
    };
    int GetIntValuePairLast(string key) {
      return GetIntValuePair(key).second;
    };

    double GetDoubleValue(string key);

  private:

    static po::options_description fDescription;
    po::variables_map fVariablesMap;

    std::pair<int, int> ParseIntRange(string value);

};

extern QwOptions gQwOptions;

#endif // QWOPTIONS
