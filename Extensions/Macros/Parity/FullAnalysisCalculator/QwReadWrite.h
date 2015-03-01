/**********************************************************\
 * QwReadWrite.h

 * Buddhini Waidyawansa (buddhini@jlab.org)
 * 03-04-2014
 * A class to read/write data from/to text input/output files.

\**********************************************************/

#ifndef QWREADWRITE_H
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
#include <Rtypes.h>
#include <TString.h>


using namespace std;

class QwReadFile {

 public:
  QwReadFile(){};
  QwReadFile(const char* name);
  ~QwReadFile(){};

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
  Bool_t LineIsEmpty(){return fLine.empty();};
  float GetNextToken(const std::string& separatorchars);
  void GetTokenPair(const std::string& separatorchars,
		    float*value,float*error);


 protected:
  // Current line and position
  std::string fLine;      /// Internal line storage
  size_t fCurrentPos;     /// Current position in the line

  // File and stream
  const std::string fRfilename;
  std::filebuf* inbuf;
  std::stringstream fStream;

};


#endif 




