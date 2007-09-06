/**********************************************************\
* File: QwParameterFile.h                              *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWPARAMETERFILE__
#define __QWPARAMETERFILE__

#include <iostream>
#include <fstream>
#include <string>
#include "Rtypes.h"
#include "TString.h"


class QwParameterFile{
 public:
  QwParameterFile(const char *filename);
  ~QwParameterFile(){ };

  Bool_t ReadNextLine(){fCurrentPos=0;  return getline(fInputFile, fLine);};
  void TrimWhitespace(Int_t head_tail);
  void TrimComment(char commentchar);
  Bool_t LineIsEmpty(){return fLine.empty();};

  std::string GetNextToken(std::string separatorchars);

  std::string GetLine(){return fLine;};
  
  Bool_t HasVariablePair(std::string separatorchars, std::string &varname, std::string &varvalue);
  Bool_t HasVariablePair(std::string separatorchars, TString &varname, TString &varvalue);
  
  

 protected:
  ifstream    fInputFile;
  std::string fLine;      // Internal line storage

  Int_t fCurrentPos;

 private:
  QwParameterFile(){};

};

QwParameterFile::QwParameterFile(const char *filename):fInputFile(filename){};


void QwParameterFile::TrimWhitespace(Int_t head_tail=3){
  //  If the first bit is set, this routine removes leading spaces from the
  //  line.  If the second bit is set, this routine removes trailing spaces
  //  from the line.  The default behavior is to remove both.
  std::string   ws = " ";
  size_t mypos;
  //  Remove leading spaces.  If this first test returns "npos", it means
  //  this line is all whitespace, so get rid of it all.
  //  If we're not removing leading spaces, lines which are all spaces
  //  will not be removed.
  mypos = fLine.find_first_not_of(ws);
  if ((head_tail&1==1)) fLine.erase(0,mypos);
  //  Remove trailing spaces
  mypos = fLine.find_last_not_of(ws);
  mypos = fLine.find_first_of(ws,mypos);
  if (mypos != std::string::npos && (head_tail&2==2)) fLine.erase(mypos);
}

void QwParameterFile::TrimComment(char commentchar){
  //  Remove everything after the comment character
  size_t mypos = fLine.find_first_of(commentchar);
  if (mypos != std::string::npos){
    fLine.erase(mypos);
  }
}

Bool_t QwParameterFile::HasVariablePair(std::string separatorchars, TString &varname, TString &varvalue){
  std::string tmpvar, tmpval;
  Bool_t status = HasVariablePair(separatorchars, tmpvar, tmpval);
  if (status){
    varname  = tmpvar.c_str();
    varvalue = tmpval.c_str();
  }
  return status;  
};

Bool_t QwParameterFile::HasVariablePair(std::string separatorchars, std::string &varname, std::string &varvalue){
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of(separatorchars);
  if (equiv_pos1 != std::string::npos){
    size_t equiv_pos2 = fLine.find_first_not_of(separatorchars,equiv_pos1);  
    if (equiv_pos2 != std::string::npos){
      varname  = fLine.substr(0,equiv_pos1);
      varvalue = fLine.substr(equiv_pos2);
      status = kTRUE;
    }
  }
  return status;
};
  

std::string QwParameterFile::GetNextToken(std::string separatorchars){
  size_t pos1 = fCurrentPos;
  size_t pos2 = fLine.find_first_of(separatorchars, pos1);
  fCurrentPos = fLine.find_first_not_of(separatorchars, pos2);
  return fLine.substr(pos1,pos2);
};


#endif
