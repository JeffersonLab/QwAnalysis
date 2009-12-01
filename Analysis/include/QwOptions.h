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

    /** \brief Set the configuration file
     */
    void SetConfigFile(string configfile) {
      fConfigFile = configfile;
      fParsed = false;
    };


    /** \brief Parse all sources of options
     */
    void Parse() {
      ParseCommandLine();
      ParseConfigFile();
      ParseEnvironment();
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
      if (fVariablesMap.count(key))
        return fVariablesMap[key].as<T>();
      else
        return 0;
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
    void ParseEnvironment() { };

    /** \brief Parse a range of integers as #:# where either can be missing
     */
    std::pair<int, int> ParseIntRange(string value);


    /** \brief Configuration file
     */
    string fConfigFile;

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
