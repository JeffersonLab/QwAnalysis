/*!
 * \file   QwOptions.cc
 * \brief  An options class which parses command line, config file and environment
 *
 * \author Wouter Deconinck
 * \date   2009-12-01
 */

#include "QwOptions.h"

// System headers
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <climits>

// Statically defined option descriptions
po::options_description QwOptions::fDefaultOptions("Default options");
po::options_description QwOptions::fOptions("Specialized options");

// Globally defined instance of the options object
QwOptions gQwOptions;

// Qweak headers
#include "QwLog.h"

// Initialize the static command line arguments to zero
int QwOptions::fArgc = 0;
char** QwOptions::fArgv = 0;

/**
 * The default constructor sets up the options description object with some
 * options that should always be there.  The other options can be setup
 * wherever this object is accessible.
 */
QwOptions::QwOptions()
  : fParsed(false)
{
  // No default config files
  fConfigFiles.clear();

  // Declare the generic options
  AddDefaultOptions()("usage",  "print this help message");
  AddDefaultOptions()("help,h", "print this help message");
  AddDefaultOptions()("config,c", po::value<string>(), "configuration file to read");

  // Define the execution options
  AddDefaultOptions()("online", po::value<bool>()->default_value(false)->zero_tokens(),
                      "use online data stream");
  AddDefaultOptions()("run,r", po::value<string>()->default_value("0:0"),
                      "run range in format #[:#]");
  AddDefaultOptions()("event,e", po::value<string>()->default_value("0:"),
                      "event range in format #[:#]");

  // Define the logging options
  AddOptions()("QwLog.logfile", po::value<string>(), "log file");
  AddOptions()("QwLog.loglevel-file", po::value<int>()->default_value(4),
               "log level for file output");
  AddOptions()("QwLog.loglevel-screen", po::value<int>()->default_value(2),
               "log level for screen output");
}

/**
 * Destructor where we clean up locally allocated memory
 */
QwOptions::~QwOptions()
{
  // Clean up the copy of the command line arguments
  // Note: only the array of arguments is allocated, the arguments themselves
  // are still owned by main.
  if (fArgv)
    delete[] fArgv;
}

/**
 * Make a local copy of the command line arguments so they are available
 * for later parsing.
 * @param argc Number of arguments
 * @param argv[] Array of arguments
 */
void QwOptions::SetCommandLine(int argc, char* argv[])
{
  fArgc = argc;
  if (fArgv) delete[] fArgv;
  fArgv = new char*[fArgc];
  for (int i = 0; i < argc; i++) {
    fArgv[i] = argv[i];
  }
  fParsed = false;
}


/**
 * Parse the command line arguments for options and warn when encountering
 * an unknown option, then notify the variables map.  Print usage instructions
 * when no options are given, or when explicitly asked for.
 */
void QwOptions::ParseCommandLine()
{
  try {
#if BOOST_VERSION >= 103300
    //  Boost version after 1.33.00 allow unrecognized options to be 
    //  passed through the parser.
    po::store(po::command_line_parser(fArgc, fArgv).options(fDefaultOptions).allow_unregistered().run(), fVariablesMap);
    po::store(po::command_line_parser(fArgc, fArgv).options(fOptions).allow_unregistered().run(), fVariablesMap);
#else
    //  Boost versions before 1.33.00 do not recognize "allow_unregistered".
    po::store(po::command_line_parser(fArgc, fArgv).options(fDefaultOptions).run(), fVariablesMap);
    po::store(po::command_line_parser(fArgc, fArgv).options(fOptions).run(), fVariablesMap);
#endif
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing command line arguments" << QwLog::endl;
#if BOOST_VERSION < 103300
    QwWarning << "ALL command line arguments may have been ignored!" << QwLog::endl;
#endif
  }
  po::notify(fVariablesMap);

  // If no arguments or option help/usage, print help text.
  if (fArgc == 1 || fVariablesMap.count("help") || fVariablesMap.count("usage")) {
    Usage();
    exit(1);
  }

  // If a configuration file is specified, load it.
  if (fVariablesMap.count("config") > 0) {
    QwWarning << "Using configuration file "
              << fVariablesMap["config"].as<string>() << QwLog::endl;
    SetConfigFile(fVariablesMap["config"].as<string>());
  }
}

/**
 * Parse the environment variables for options and notify the variables map.
 */
void QwOptions::ParseEnvironment()
{
  try {
    po::store(po::parse_environment(fDefaultOptions, "Qw"), fVariablesMap);
    po::store(po::parse_environment(fOptions, "Qw"), fVariablesMap);
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing environment variables" << QwLog::endl;
  }
  po::notify(fVariablesMap);
}

/**
 * Parse the configuration file for options and warn when encountering
 * an unknown option, then notify the variables map.
 */
void QwOptions::ParseConfigFile()
{
  for (size_t i = 0; i < fConfigFiles.size(); i++) {
    std::ifstream configfile(fConfigFiles.at(i).c_str());
    std::stringstream configstream;
    configstream << configfile.rdbuf();

    try {
#if BOOST_VERSION >= 103500
      // Boost version after 1.35 have bool allow_unregistered = false in
      // their signature.  This allows for unknown options in the config file.
      po::store(po::parse_config_file(configstream, fDefaultOptions, true), fVariablesMap);
      po::store(po::parse_config_file(configstream, fOptions, true), fVariablesMap);
#else
      // Boost versions before 1.35 cannot handle files with unregistered options.
      po::store(po::parse_config_file(configstream, fDefaultOptions), fVariablesMap);
      po::store(po::parse_config_file(configstream, fOptions), fVariablesMap);
#endif
    } catch (std::exception const& e) {
      QwWarning << e.what() << " while parsing configuration file "
                << fConfigFiles.at(i) << QwLog::endl;
#if BOOST_VERSION < 103500
      QwWarning << "the entire configuration file was ignored!" << QwLog::endl;
#endif
    }
    po::notify(fVariablesMap);
  }
}



/**
 * Print usage information
 */
void QwOptions::Usage()
{
  QwMessage << "Welcome to the Qweak analyzer code." << QwLog::endl;
  QwMessage << QwLog::endl;
  QwMessage << fDefaultOptions << QwLog::endl;
  QwMessage << fOptions << QwLog::endl;
}


/**
 * Get a pair of integers specified as a colon-separated range
 * @param key Option key
 * @return Pair of integers
 */
std::pair<int,int> QwOptions::GetIntValuePair(string key)
{
  std::pair<int, int> mypair;
  mypair.first = 0;
  mypair.second = 0;

  if (fParsed == false) Parse();
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
      mypair.second = INT_MAX;
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
