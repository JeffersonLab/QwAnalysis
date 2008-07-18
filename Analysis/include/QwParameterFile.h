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
#include <fstream>
#include <string>
#include "Rtypes.h"
#include "TString.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace bfs = boost::filesystem;

class QwParameterFile{
 public:
  static UInt_t GetUInt(const TString &varvalue);


 public:
  QwParameterFile(const char *filename);
  ~QwParameterFile(){ };

  static void AppendToSearchPath(const TString &searchdir);

  Bool_t ReadNextLine(){fCurrentPos=0;  return getline(fInputFile, fLine);};
  void TrimWhitespace(TString::EStripType head_tail = TString::kBoth);
  void TrimComment(char commentchar);
  Bool_t LineIsEmpty(){return fLine.empty();};

  std::string GetNextToken(std::string separatorchars);
  std::string GetLine(){return fLine;};

  void RewindToLineStart(){fCurrentPos=0;};
  
  Bool_t HasVariablePair(std::string separatorchars, std::string &varname, std::string &varvalue);
  Bool_t HasVariablePair(std::string separatorchars, TString &varname, TString &varvalue);
  
 protected:
  void TrimWhitespace(std::string  &token, TString::EStripType head_tail);

  static std::vector<bfs::path> fSearchPaths;

  ifstream    fInputFile;
  
  std::string fLine;      /// Internal line storage
  size_t fCurrentPos;     /// Current position in the line
  
 private:
  QwParameterFile(){};

};

#endif
