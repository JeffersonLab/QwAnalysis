/*!
 * \file   QwOptions.cc
 * \brief  An options class which parses command line, config file and environment
 *
 * \author Wouter Deconinck
 * \date   2009-12-01
 */

#include "QwOptions.h"
#include "QwParameterFile.h"

// System headers
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <climits>

// Statically defined option descriptions grouped by parser
po::options_description QwOptions::fCommandLineOptions("Command line options");
po::options_description QwOptions::fEnvironmentOptions("Environment options");
po::options_description QwOptions::fConfigFileOptions("Config file options");

// Globally defined instance of the options object
QwOptions gQwOptions;

// Qweak headers
#include "QwLog.h"
#include "QwSubsystemArray.h"
#include "QwEventBuffer.h"
#include "QwDatabase.h"

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

  // Declare the default options
  AddDefaultOptions()("usage",  "print this help message");
  AddDefaultOptions()("help,h", "print this help message");
  AddDefaultOptions()("config,c", po::value<std::string>(), "configuration file to read");
}

/**
 * Define standard options on the specified options object
 * @param options Options object
 */
void QwOptions::DefineOptions(QwOptions& options)
{
  // Define logging options (Note: only QwLog takes a pointer argument!!!)
  QwLog::DefineOptions(&options);

  // Define execution options
  QwEventBuffer::DefineOptions(options);
  // Define database options
  QwDatabase::DefineOptions(options);
  // Define subsystem array options
  QwSubsystemArray::DefineOptions(options);
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

  // Delete the option blocks
  for (size_t i = 0; i < fOptionBlock.size(); i++)
    delete fOptionBlock.at(i);
  fOptionBlock.clear();
}

/**
 * Make a local copy of the command line arguments so they are available
 * for later parsing.
 * @param argc Number of arguments
 * @param argv[] Array of arguments
 */
void QwOptions::SetCommandLine(int argc, char* argv[])
{
  // Copy command line options
  fArgc = argc;
  if (fArgv) delete[] fArgv;
  fArgv = new char*[fArgc];
  for (int i = 0; i < argc; i++) {
    fArgv[i] = argv[i];
  }
  fParsed = false;
}



/**
 * Combine the options of the various option descriptions in one object for
 * parsing at once.  This avoids having to try/catch every single option
 * description
 */
void QwOptions::CombineOptions()
{
  // TODO The options could be grouped in a smarter way by subsystem or
  // class, by defining a vector fOptions of options_description objects.
  // Each entry could have a name and would show up as a separate section
  // in the usage information.
  for (size_t i = 0; i < fOptionBlockName.size(); i++) {
    // Right now every parser gets access to all options
    fCommandLineOptions.add(*fOptionBlock.at(i));
    fEnvironmentOptions.add(*fOptionBlock.at(i));
    fConfigFileOptions.add(*fOptionBlock.at(i));
  }
};

/**
 * Parse the command line arguments for options and warn when encountering
 * an unknown option, then notify the variables map.  Print usage instructions
 * when no options are given, or when explicitly asked for.
 */
void QwOptions::ParseCommandLine()
{
#if BOOST_VERSION >= 103300
  //  Boost versions starting with 1.33.00 allow unrecognized options to be
  //  passed through the parser.
  try {
    po::store(po::command_line_parser(fArgc, fArgv).options(fCommandLineOptions).allow_unregistered().run(), fVariablesMap);
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing command line arguments" << QwLog::endl;
    exit(10);
  }
#endif

#if BOOST_VERSION < 103300
  //  Boost versions before 1.33.00 do not allow unrecognized options to be
  //  passed through the parser.
  try {
    //  Boost versions before 1.33.00 do not recognize "allow_unregistered".
    po::store(po::command_line_parser(fArgc, fArgv).options(fCommandLineOptions).run(), fVariablesMap);
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing command line arguments" << QwLog::endl;
    QwWarning << "All command line arguments may have been ignored!" << QwLog::endl;
    exit(10);
  }
#endif

  // Notify of new options
  po::notify(fVariablesMap);

  // If option help/usage, print help text.
  if (fVariablesMap.count("help") || fVariablesMap.count("usage")) {
    Usage();
    exit(0);
  }

  // If a configuration file is specified, load it.
  if (fVariablesMap.count("config") > 0) {
    QwWarning << "Using configuration file "
              << fVariablesMap["config"].as<std::string>() << QwLog::endl;
    SetConfigFile(fVariablesMap["config"].as<std::string>());
  }
}

/**
 * Parse the environment variables for options and notify the variables map.
 */
void QwOptions::ParseEnvironment()
{
  try {
    po::store(po::parse_environment(fEnvironmentOptions, "Qw"), fVariablesMap);
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
    QwParameterFile configfile(fConfigFiles.at(i).c_str());
    std::stringstream configstream;
    configstream << configfile.rdbuf();

    try {
#if BOOST_VERSION >= 103500
      // Boost version after 1.35 have bool allow_unregistered = false in
      // their signature.  This allows for unknown options in the config file.
      po::store(po::parse_config_file(configstream, fConfigFileOptions, true),
		fVariablesMap);
#else
      // Boost versions before 1.35 cannot handle files with unregistered
      // options.
      po::store(po::parse_config_file(configstream, fConfigFileOptions),
		fVariablesMap);
#endif
    } catch (std::exception const& e) {
      QwWarning << e.what() << " while parsing configuration file "
                << fConfigFiles.at(i) << QwLog::endl;
#if BOOST_VERSION < 103500
      QwWarning << "The entire configuration file was ignored!" << QwLog::endl;
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
  QwMessage << QwLog::endl;
  QwMessage << "Welcome to the Qweak analyzer code." << QwLog::endl;
  QwMessage << QwLog::endl;
  for (size_t i = 0; i < fOptionBlock.size(); i++)
    QwMessage << *(fOptionBlock.at(i)) << QwLog::endl;
}


/**
 * Get a pair of integers specified as a colon-separated range
 * @param key Option key
 * @return Pair of integers
 */
std::pair<int,int> QwOptions::GetIntValuePair(std::string key)
{
  std::pair<int, int> mypair;
  mypair.first = 0;
  mypair.second = 0;

  if (fParsed == false) Parse();
  if (fVariablesMap.count(key)) {
    std::string range = fVariablesMap[key].as<std::string>();
    mypair = ParseIntRange(range);
  }

  return mypair;
}

std::pair<int, int> QwOptions::ParseIntRange(std::string range)
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
