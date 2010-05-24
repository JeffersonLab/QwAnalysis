/*!
 * \file   QwOptions.h
 * \brief  An options class which parses command line, config file and environment
 *
 * \author Wouter Deconinck
 * \date   2009-12-01
 */

#ifndef QWOPTIONS_H
#define QWOPTIONS_H

// System headers
#include <iostream>
#include <string>
using std::string;
#include <vector>
using std::vector;

// Boost headers
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// Qweak headers
#include "QwLog.h"

// Helper function for safe environment variable parsing
inline const std::string getenv_safe (const char* name) {
 if (getenv(name))
   return std::string(getenv(name));
 else {
   QwWarning << "Environment variable " << name << " undefined!" << QwLog::endl;
   return std::string(".");
 }
}

/**
 *  \class QwOptions
 *  \ingroup QwAnalysis
 *  \brief An options class
 *
 * On instantiation of the global gQwOptions object the argc and argv are passed.
 * The filename is set to a default value, but on instantiation a check is done
 * for the option --config filename.  After this no parsing is done without the
 * user requesting a value.
 *
 * To use this class in your own programs, just include the header file.  This
 * will make the global object gQwOptions available.   Set the command line and
 * config file with the methods SetCommandLine() and SetConfigFile() in the main
 * routine.  In other classes you can add subsystem-specific config files with
 * the AddConfigFile() method.
 * \code
 * gQwOptions.SetCommandLine(argc, argv); // store command line arguments
 * gQwOptions.SetConfigFile("qwtracking.conf"); // set the default config file
 * gQwOptions.AddConfigFile("qweak-tracking.conf"); // add another config file
 * \endcode
 *
 * Define your own options by calling the AddOptions() method.  Look in the
 * documentation of boost::program_options for the allowed syntax if you need
 * more than just '--key value' pairs.  Positional arguments are supported
 * (untested), default values can and should preferably be given, and multiple
 * values can be put into a vector corresponding to a single key (untested).
 * \code
 * gQwOptions.AddOptions()("int,i", po::value<int>(), "integer-valued option");
 * gQwOptions.AddOptions()("bool,b", po::value<bool>(), "boolean-valued option");
 * gQwOptions.AddOptions()("float,f", po::value<float>(), "float-valued option");
 * gQwOptions.AddOptions()("string,s", po::value<string>(), "string-valued option");
 * gQwOptions.AddOptions()("range,r", po::value<string>(), "range-valued option");
 * gQwOptions.AddOptions()("def-int", po::value<int>()->default_value(1), "int-valued option, default of 1");
 * \endcode
 * For boolean-valued options you might want to use zero_tokens() to allow the
 * option to be specified as --bool instead of --bool yes.
 * \code
 * gQwOptions.AddOptions()("bool,b", po::value<bool>()->zero_tokens(), "boolean-valued option");
 * \endcode
 *
 * It is easiest if you define your options in a static function DefineOptions()
 * and then call that function in the QwOptionsTracking.h or QwOptionsParity.h
 * header files.  This ensures that only a single call to gQwOptions.DefineOptions()
 * will load all the options and provide the information on a --help call.
 *
 * To use the options, check first for the existence with HasValue(key), then
 * get their value using GetValue<type>(key).  Flags defined without type are
 * treated as 'existence-only' keys, so use HasValue(key) on them instead of
 * GetValue<bool>(key).  If you define an option as bool, you should still use
 * the regular GetValue<bool>(key) to retrieve the value.
 * \code
 * // Test for presence of the option before using its value
 * if (gQwOptions.HasValue("string")) string my_string = gQwOptions.GetValue<string>("string");
 * // Or use options with default values (preferable)
 * int my_int = gQwOptions.GetValue<int>("def-int");
 * \endcode
 *
 * To get the results of an integer range argument (strictly speaking a string
 * argument), you can use GetIntValuePair() which returns a pair<int,int>, or
 * the more specific GetIntValuePairFirst() and GetIntValuePairLast().
 * \code
 * std::pair<int,int> my_run_pair = gQwOptions.GetIntValuePair("run");
 * gQwOptions.GetIntValuePair("run").first == gQwOptions.GetIntValuePairFirst("run");
 * gQwOptions.GetIntValuePair("run").second == gQwOptions.GetIntValuePairLast("run");
 * \endcode
 *
 * The default allowed options are:
 * - --usage: print a help message
 * - --help, -h: print a help message
 * - --config, -c: configuration file to read
 */
class QwOptions {

  public:

    /// \brief Default constructor
    QwOptions();

    /// \brief Constructor with command line arguments
    QwOptions(int argc, char* argv[]) {
      SetCommandLine(argc, argv);
    };
    /// \brief Constructor with configuration file
    QwOptions(string configfile) {
      SetConfigFile(configfile);
    };
    /// \brief Constructor with command line arguments and configuration file
    QwOptions(int argc, char* argv[], string configfile) {
      SetCommandLine(argc, argv);
      SetConfigFile(configfile);
    };

    /// \brief Default destructor
    virtual ~QwOptions();


    /// \brief Add a default option
    po::options_description_easy_init AddDefaultOptions() {
      fParsed = false;
      return fDefaultOptions.add_options();
    };
    /// \brief Add an option
    po::options_description_easy_init AddOptions() {
      fParsed = false;
      return fSpecialOptions.add_options();
    };


    /// \brief Print usage information
    void Usage();

    /// \brief Define the options
    static void DefineOptions(QwOptions& options);

    /// \brief Set the command line arguments
    void SetCommandLine(int argc, char* argv[]);

    /// \brief Set a configuration file
    void SetConfigFile(string configfile) {
      fConfigFiles.clear();
      fConfigFiles.push_back(configfile);
      fParsed = false;
    };

    /// \brief Add a configuration file
    void AddConfigFile(string configfile) {
      fConfigFiles.push_back(configfile);
      fParsed = false;
    };

    /// \brief Add some configuration files
    void AddConfigFile(vector<string> configfiles) {
      for (size_t i = 0; i < configfiles.size(); i++)
        fConfigFiles.push_back(configfiles.at(i));
      fParsed = false;
    };

    /// \brief List the configuration files
    void ListConfigFiles() {
      for (size_t i = 0; i < fConfigFiles.size(); i++)
        QwMessage << fConfigFiles.at(i) << QwLog::endl;
    };


    /// \brief Parse all sources of options
    void Parse() {
      CombineOptions();
      ParseCommandLine();
      ParseEnvironment();
      ParseConfigFile();
      fParsed = true;
    };


    /// \brief Has this key been defined
    bool HasValue(string key) {
      if (fParsed == false) Parse();
      return (fVariablesMap.count(key) > 0);
    };

    /// \brief Get a templated value
    template < class T >
    T GetValue(const string key) {
      if (fParsed == false) Parse();
      if (fVariablesMap.count(key)) {
        QwVerbose << "Option " << key << ": "
                  << fVariablesMap[key].as<T>() << QwLog::endl;
        return fVariablesMap[key].as<T>();
      } else {
        QwError << "Variable " << key << " unknown" << QwLog::endl;
        return 0;
      }
    }

    /// \brief Get a pair of integer values
    std::pair<int,int> GetIntValuePair(string key);

    /// \brief Get the first of a pair of integer values
    int GetIntValuePairFirst(string key) {
      return GetIntValuePair(key).first;
    };

    /// \brief Get the last of a pair of integer values
    int GetIntValuePairLast(string key) {
      return GetIntValuePair(key).second;
    };

  private:

    /// \brief Combine the various option description in one
    void CombineOptions();

    /// \brief Parse the command line arguments
    void ParseCommandLine();

    /// \brief Parse the configuration file
    void ParseConfigFile();

    /// \brief Parse the environment variables
    void ParseEnvironment();

    /// \brief Parse a range of integers as #:# where either can be missing
    std::pair<int, int> ParseIntRange(string range);

    /// \brief Configuration file
    vector<string> fConfigFiles;

    /** \brief Command line arguments
     *
     * Because argc and argv are only available in the main routine, we need
     * to store a copy of them for later parsing.  The copy is static to have
     * them available in local copies of the options object.
     */
    static int fArgc;
    static char** fArgv;

    // Vector with option blocks
    vector<po::options_description> fOptions;
    // Options descriptions grouped by function
    static po::options_description fDefaultOptions;
    static po::options_description fSpecialOptions;
    // Options descriptions grouped by parser
    static po::options_description fCommandLineOptions;
    static po::options_description fEnvironmentOptions;
    static po::options_description fConfigFileOptions;
    po::variables_map fVariablesMap;

    bool fParsed;
};

extern QwOptions gQwOptions;

#endif // QWOPTIONS_H
