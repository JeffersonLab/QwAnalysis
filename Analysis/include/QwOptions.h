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

/**
 @author Wouter Deconinck <wdconinc@mit.edu>

 On instantiation of the global gQwOptions object the argc and argv are passed.
 The filename is set to a default value, but on instantiation a check is done
 for the option --config filename.  After this no parsing is done without the
 user requesting it.

 To use this class in your own programs, just include the header file.  This
 will make the global object gQwOptions available.   Set the command line and
 config file with the methods SetCommandLine() and SetConfigFile() in the main
 routine.  In other classes you can add subsystem-specific config files with
 the AddConfigFile() method.

 Define your own options by calling the AddOptions() method.  Look in the
 documentation of boost::program_options for the allowed syntax if you need
 more than just '--key value' pairs.  Positional arguments are supported (but
 untested), default values can and probably should be given, and multiple values
 can be put into a vector corresponding to a single key (composing).

 To use the options, check first for the existence with HasValue(key), then
 get their value using GetValue<type>(key).  Flags defined without type are
 treated as 'existence-only' keys, so use HasValue(key) on them instead of
 GetValue<bool>(key).  If you define an options as bool, you should still use
 the regular GetValue<bool>(key) to retrieve the value.

*/
class QwOptions {

  public:

    /** \brief Default constructor
     */
    QwOptions();

    /** \brief Constructor with command line arguments
     */
    QwOptions(int argc, char* argv[]) {
      SetCommandLine(argc, argv);
    };
    /** \brief Constructor with configuration file
     */
    QwOptions(string configfile) {
      SetConfigFile(configfile);
    };
    /** \brief Constructor with command line arguments and configuration file
     */
    QwOptions(int argc, char* argv[], string configfile) {
      SetCommandLine(argc, argv);
      SetConfigFile(configfile);
    };

    /** \brief Default destructor
     */
    virtual ~QwOptions();


    /** \brief Add a recognized option
     */
    po::options_description_easy_init AddOptions() {
      fParsed = false;
      return fOptions.add_options();
    };


    /** \brief Print usage information
     */
    void Usage();


    /** \brief Set the command line arguments
     */
    void SetCommandLine(int argc, char* argv[]);

    /** \brief Set a configuration file
     */
    void SetConfigFile(string configfile) {
      fConfigFiles.clear();
      fConfigFiles.push_back(configfile);
      fParsed = false;
    };

    /** \brief Add a configuration file
     */
    void AddConfigFile(string configfile) {
      fConfigFiles.push_back(configfile);
      fParsed = false;
    };

    /** \brief Add some configuration files
     */
    void AddConfigFile(vector<string> configfiles) {
      for (size_t i = 0; i < configfiles.size(); i++)
        fConfigFiles.push_back(configfiles.at(i));
      fParsed = false;
    };

    /** \brief List the configuration files
     */
    void ListConfigFiles() {
      for (size_t i = 0; i < fConfigFiles.size(); i++)
        QwMessage << fConfigFiles.at(i) << QwLog::endl;
    };


    /** \brief Parse all sources of options
     */
    void Parse() {
      ParseCommandLine();
      ParseEnvironment();
      ParseConfigFile();
      fParsed = true;
    };


    /** \brief Has this key been defined
     */
    bool HasValue(string key) {
      if (fParsed == false) Parse();
      return (fVariablesMap.count(key) > 0);
    };

    /** \brief Get a templated value
     */
    template < class T >
    T GetValue(string key) {
      if (fParsed == false) Parse();
      if (fVariablesMap.count(key)) {
        QwVerbose << "Option " << key << ": "
                  << fVariablesMap[key].as<T>() << QwLog::endl;
        return fVariablesMap[key].as<T>();
      } else {
        QwWarning << "Variable " << key << " unknown" << QwLog::endl;
        return 0;
      }
    }

    /** \brief Get a pair of integer values
     */
    std::pair<int,int> GetIntValuePair(string key);

    /** \brief Get the first of a pair of integer values
     */
    int GetIntValuePairFirst(string key) {
      return GetIntValuePair(key).first;
    };

    /** \brief Get the last of a pair of integer values
     */
    int GetIntValuePairLast(string key) {
      return GetIntValuePair(key).second;
    };

  private:

    /** \brief Parse the command line arguments
     */
    void ParseCommandLine();

    /** \brief Parse the configuration file
     */
    void ParseConfigFile();

    /** \brief Parse the environment variables
     */
    void ParseEnvironment();

    /** \brief Parse a range of integers as #:# where either can be missing
     */
    std::pair<int, int> ParseIntRange(string value);


    /** \brief Configuration file
     */
    vector<string> fConfigFiles;

    /** \brief Command line arguments
     *
     * Because argc and argv are only available in the main routine, we need
     * to store a copy of them for later parsing.
     */
    int fArgc;
    char** fArgv;

    static po::options_description fOptions;
    po::variables_map fVariablesMap;

    bool fParsed;
};

extern QwOptions gQwOptions;

#endif // QWOPTIONS
