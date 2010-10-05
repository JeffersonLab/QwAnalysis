/**********************************************************\
* File: QwParameterFile.h                              *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWPARAMETERFILE__
#define __QWPARAMETERFILE__


#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "Rtypes.h"
#include "TString.h"
#include "TRegexp.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace bfs = boost::filesystem;

///
/// \ingroup QwAnalysis
class QwParameterFile {

  public:

    static UInt_t GetUInt(const TString &varvalue);


  public:

    QwParameterFile(const std::string& filename);
    QwParameterFile(const std::stringstream& stream) {
      fStream << stream.rdbuf();
    };
    virtual ~QwParameterFile() { };

    /// Access the streambuf pointer in the same way as on a std::ifstream
    std::streambuf* rdbuf() const {return fStream.rdbuf(); };

    /// Add a directory to the search path
    static void AppendToSearchPath(const TString& searchdir);

    /// Set the current run number for looking up the appropriate parameter file
    static void SetCurrentRunNumber(const UInt_t runnumber) { fCurrentRunNumber = runnumber; };

    Bool_t ReadNextLine() {
      fCurrentPos = 0;
      std::string tmp;
      return ReadNextLine(tmp);
    };
    Bool_t ReadNextLine(std::string &varvalue) {
      fCurrentPos = 0;
      if (! getline(fStream, fLine))
        // No next line
        return 0;
      else {
        // Copy next line
        varvalue = fLine;
        // Allow 'append'
        std::string tmpname, tmpvalue;
        if (HasVariablePair(" ",tmpname,tmpvalue)) {
          if (tmpname == "append") {
            // Test for recursion in file nesting
            static int nested_depth = 0;
            if (nested_depth++ > 5) {
              std::cout << "Parameter file recursion not allowed!" << std::endl;
              return 0;
            }
            // Stream nested file into this file
            QwParameterFile nested_file(tmpvalue.c_str());
            fStream << nested_file.rdbuf();
            // Read line from appended file
            return ReadNextLine(varvalue);
          }
        }
        return 1;
      }
    };

    void TrimWhitespace(TString::EStripType head_tail = TString::kBoth);
    void TrimComment(char commentchar);
    Bool_t LineIsEmpty(){return fLine.empty();};
    Bool_t IsEOF(){ return fStream.eof();};

    std::string GetNextToken(std::string separatorchars);
    std::string GetLine() { return fLine; };
    void AddLine(const std::string& line) { fStream << line << std::endl; };

    void RewindToFileStart() { fStream.clear(); fStream.seekg(0, std::ios::beg); };
    void RewindToLineStart() { fCurrentPos = 0; };

    Bool_t HasValue(TString& vname);

    Bool_t HasVariablePair(std::string separatorchars, std::string& varname, std::string& varvalue);
    Bool_t HasVariablePair(std::string separatorchars, TString& varname, TString& varvalue);

    Bool_t FileHasVariablePair(std::string separatorchars, const std::string& varname, std::string& varvalue);
    Bool_t FileHasVariablePair(std::string separatorchars, const TString& varname, TString& varvalue);

    Bool_t LineHasSectionHeader(std::string& secname);
    Bool_t LineHasSectionHeader(TString& secname);

    Bool_t LineHasModuleHeader(std::string& secname);
    Bool_t LineHasModuleHeader(TString& secname);

    Bool_t FileHasSectionHeader(const std::string& secname);
    Bool_t FileHasSectionHeader(const TString& secname);

    Bool_t FileHasModuleHeader(const std::string& secname);
    Bool_t FileHasModuleHeader(const TString& secname);


    /// \brief Rewinds to the start and read until it's finds next header
    QwParameterFile* ReadPreamble();
    QwParameterFile* ReadUntilNextSection();
    QwParameterFile* ReadNextSection(std::string &secname);
    QwParameterFile* ReadNextSection(TString &secname);

    QwParameterFile* ReadUntilNextModule();
    QwParameterFile* ReadNextModule(std::string &secname);
    QwParameterFile* ReadNextModule(TString &secname);

    friend ostream& operator<< (ostream& stream, const QwParameterFile& file);


  protected:

    void TrimWhitespace(std::string  &token, TString::EStripType head_tail);


  private:

    /// Find the first file in a directory that conforms to the run label
    int FindFile(const bfs::path& dir_path,    // in this directory,
                 const std::string& file_stem, // search for this stem,
                 const std::string& file_ext,  // search for this extension,
                 bfs::path& path_found);       // placing path here if found

    /// Open a file
    bool OpenFile(const bfs::path& path_found);

  public:

    /// \brief Parse a range of integers as #:# where either can be missing
    static std::pair<int,int> ParseIntRange(const std::string& separatorchars, const std::string& range);


  protected:

    // List of search paths
    static std::vector<bfs::path> fSearchPaths;

    // Current run number
    static UInt_t fCurrentRunNumber;

    // File and stream
    ifstream          fFile;
    std::stringstream fStream;

    // Current line and position
    std::string fLine;      /// Internal line storage
    size_t fCurrentPos;     /// Current position in the line


  private:

    // Private constructor
    QwParameterFile() { };

}; // class QwParameterFile


#endif // __QWPARAMETERFILE__
