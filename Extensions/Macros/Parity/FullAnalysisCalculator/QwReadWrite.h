/**********************************************************\
 * QwReadWrite.h

 * Buddhini Waidyawansa (buddhini@jlab.org)
 * 03-04-2014
 * A class to read/write data from/to text input/output files.

\**********************************************************/

#ifdef QWREADWRITE_H
#define QWREADWRITE_H

// System headers
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <set>

// ROOT headers
#include "Rtypes.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TMacro.h"
#include "TList.h"

class QwReadWrite {

 public:

  QwReadWrite(const std::string& filename);
  virtual ~QwReadWrite() { };


  Bool_t ReadNextDataLine() {
    fCurrentPos = 0;
    std::string tmp;
    return ReadNextLine(tmp);
  };
  
  //  Keep reading until a non-comment, non-empty line has been found.
  Bool_t ReadNextLine(std::string &varvalue) {
    fCurrentPos = 0;
    if (! getline(fStream, fLine))
      // No next line
      return 0;
    else{
      // Copy next line
      varvalue = fLine;
      return 1;
    }
  };
  
  void TrimComment(const std::string& commentchars);
  void AddLine(const std::string& line) { 
    fStream << line << std::endl; 
  };

  // Get next token as a string
  Double_t GetNextToken(const std::string& separatorchars);
  Double_t GetNextToken() { 
    return GetNextToken(fTokenSepChars); 
  };


 protected:
  // Current line and position
  std::string fLine;      /// Internal line storage
  size_t fCurrentPos;     /// Current position in the line

  // File and stream
  const std::string fFilename;
  std::ifstream fFile;

  // Local comment, whitespace, section, module characters
  std::string fCommentChars;
  std::string fWhitespaceChars;
  std::string fTokenSepChars;

  // Default comment, whitespace, section, module characters
  static const std::string kDefaultCommentChars;
  static const std::string kDefaultWhitespaceChars;
  static const std::string kDefaultTokenSepChars;

}


#endif 




