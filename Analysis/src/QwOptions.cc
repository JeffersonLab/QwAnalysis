#include "QwOptions.h"

// System headers
#include <iostream>

// Qweak headers
#include "QwLog.h"


po::options_description QwOptions::fDescription("Allowed options");

QwOptions::QwOptions(int argc, char* argv[], string file)
{
  // Declare the supported options
  fDescription.add_options()
    ("help",  "print this help message")
    ("usage", "print this help message")
    ("verbose", "print verbose messages")
    ("logfile", po::value<string>(), "log file")
    ("loglevel-screen", po::value<int>()->default_value(2), "log level for screen output")
    ("loglevel-file",   po::value<int>()->default_value(4), "log level for file output")
    ("run",   po::value<string>(), "run range in format #[:#]")
    ("event", po::value<string>(), "event range in format #[:#]")
  ;

  po::store(po::parse_command_line(argc, argv, fDescription), fVariablesMap);
  po::notify(fVariablesMap);

  // If no arguments or option help/usage, print help text.
  if (argc == 1 || fVariablesMap.count("help") || fVariablesMap.count("usage")) {
    Usage();
    exit(1);
  }
}


QwOptions::~QwOptions()
{
}


void QwOptions::Usage()
{
  // TODO Could add some more info here about QwAnalysis in general...
  std::cout << fDescription << "\n";
}

double QwOptions::GetDoubleValue(string key)
{
  if (fVariablesMap.count(key))
    return fVariablesMap[key].as<double>();
  else
    return 0.0;
}

string QwOptions::GetStringValue(string key)
{
  if (fVariablesMap.count(key))
    return fVariablesMap[key].as<string>();
  else
    return "";
}

bool QwOptions::GetBoolValue(string key)
{
  if (fVariablesMap.count(key))
    return true;
  else
    return false;
}

int QwOptions::GetIntValue(string key)
{
  if (fVariablesMap.count(key))
    return fVariablesMap[key].as<int>();
  else
    return 0;
}

std::pair<int,int> QwOptions::GetIntValuePair(string key)
{
  std::pair<int, int> mypair;
  mypair.first = 0;
  mypair.second = 0;

  if (fVariablesMap.count(key)) {
    string range = fVariablesMap[key].as<string>();
    mypair = ParseIntRange(range);
  }

  return mypair;
}

std::pair<int, int> QwOptions::ParseIntRange(string range)
{
  /** @brief Separates a colon separated range of integers into a pair of values
   *
   *  @param range String containing two integers separated by a colon,
   *               or a single value.
   *               If the string begins with a colon, the first value is taken
   *               as zero.  If the string ends with a colon, the second value
   *               is taken as kMaxInt.
   *
   *  @return  Pair of integers of the first and last values of the range.
   *           If the range contains a single value, the two integers will
   *           be identical.
   */
  std::pair<int, int> mypair;
  size_t pos = range.find(":");
  if (pos == std::string::npos) {
    //  Separator not found.
    mypair.first  = atoi(range.substr(0,range.length()).c_str());
    mypair.second = mypair.first;
  } else {
    size_t end = range.length() - pos - 1;
    if (pos == 0) {
      // Separator is the first character
      mypair.first  = 0;
      mypair.second = atoi(range.substr(pos+1, end).c_str());
    } else if (pos == range.length()-1) {
      // Separator is the last character
      mypair.first  = atoi(range.substr(0,pos).c_str());
      mypair.second = kMaxInt;
    } else {
      mypair.first  = atoi(range.substr(0,pos).c_str());
      mypair.second = atoi(range.substr(pos+1, end).c_str());
    }
  }

  //  Check the values for common errors.
  if (mypair.first < 0){
    QwError << "The first value must not be negative!"
            << QwLog::endl;
    exit(1);
  } else if (mypair.first > mypair.second){
    QwError << "The first value must not be larger than the second value"
            << QwLog::endl;
    exit(1);
  }

  //  Print the contents of the pair for debugging.
  QwVerbose << "The range goes from " << mypair.first
            << " to " << mypair.second
            << QwLog::endl;

  return mypair;
};
