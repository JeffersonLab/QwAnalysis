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


// Since QwLog depends on QwOptions, we cannot have QwOptions depend on QwLog.
// Therefore, logging here is done using std::cout and std::cerr.  The header
// file <iostream> is therefore included directly.


// Statically defined option descriptions
po::options_description QwOptions::fOptions("Allowed options");

// Globally defined instance of the options object
QwOptions gQwOptions;


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
  fOptions.add_options()
    ("help",  "print this help message")
    ("usage", "print this help message")
  ;
}

/**
 * Destructor where we clean up locally allocated memory
 */
QwOptions::~QwOptions()
{
  // Clean up the copy of the command line arguments
  // Note: only the array of arguments is allocated, the arguments themselves
  // are still owned by main.
  if (fArgc > 0)
    delete[] fArgv;
}

/**
 * Make a local copy of the command line arguments so they are available.
 * @param argc Number of arguments
 * @param argv[] Array of arguments
 */
void QwOptions::SetCommandLine(int argc, char* argv[])
{
  fArgc = argc;
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
    po::store(po::command_line_parser(fArgc, fArgv).options(fOptions).allow_unregistered().run(), fVariablesMap);
  } catch (std::exception const& e) {
    std::cerr << "Warning: " << e.what() << " while parsing command line arguments" << std::endl;
  }
  po::notify(fVariablesMap);

  // If no arguments or option help/usage, print help text.
  if (fArgc == 1 || fVariablesMap.count("help") || fVariablesMap.count("usage")) {
    Usage();
    exit(1);
  }
}

/**
 * Parse the environment variables for options and notify the variables map.
 */
void QwOptions::ParseEnvironment()
{
  try {
    po::store(po::parse_environment(fOptions, "Qw"), fVariablesMap);
  } catch (std::exception const& e) {
    std::cerr << "Warning: " << e.what() << " while parsing environment variables" << std::endl;
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
      po::store(po::parse_config_file(configstream, fOptions), fVariablesMap);
    } catch (std::exception const& e) {
      std::cerr << "Warning: " << e.what()
                << " while parsing configuration file "
                << fConfigFiles.at(i) << std::endl;
    }
    po::notify(fVariablesMap);
  }
}



/**
 * Print usage instructions
 */
void QwOptions::Usage()
{
  std::cout << "Welcome to the Qweak analyzer code." << std::endl;
  std::cout << std::endl;
  std::cout << fOptions << "\n";
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
    std::cerr << "The first value must not be negative!"
              << std::endl;
    exit(1);
  } else if (mypair.first > mypair.second){
    std::cerr << "The first value must not be larger than the second value"
              << std::endl;
    exit(1);
  }

  //  Print the contents of the pair for debugging.
  std::cout << "The range goes from " << mypair.first
            << " to " << mypair.second
            << std::endl;

  return mypair;
};
