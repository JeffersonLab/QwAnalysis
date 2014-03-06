/**********************************************************\

 * Buddhini Waidyawansa
 * 03-04-2014

\**********************************************************/

#include "QwReadWrite.h"

// Set default comment, whitespace, section characters
const std::string QwReadWrite::kDefaultCommentChars = "#!;";
const std::string QwReadWrite::kDefaultWhitespaceChars = " \t\r";
const std::string QwReadWrite::kDefaultTokenSepChars = ", \t";


// constructor
QwReadWrite::QwReadWrite(const std::string& name)
  : fCommentChars(kDefaultCommentChars),
    fWhitespaceChars(kDefaultWhitespaceChars),
    fTokenSepChars(kDefaultTokenSepChars),
    fFilename(name),
{
  
  // try to open file
  fFile(name);
  if (!fFile.is_open()) {
    std::cout<<"Unable to open input file "<<name<<std::endl;
    exit(1);
  } else{
    // found file
    std::cout<<"Opened file "<<name<<std::endl;
    // Load into stream
    fStream << fFile.rdbuf();
  }
}

// remove comments
void QwReadWrite::TrimComment(const std::string& commentchars)
{
  // Remove everything after the comment character
  size_t mypos = fLine.find_first_of(commentchars);
  if (mypos != std::string::npos){
    fLine.erase(mypos);
  }
}

// get next token
Double_t QwReadWrite::GetNextToken(const std::string& separatorchars)
{
  std::string tmpstring = "";
  TString outstring="";

  if (fCurrentPos != std::string::npos){
    size_t pos1 = fCurrentPos;
    size_t pos2 = fLine.find_first_of(separatorchars.c_str(), pos1);
    if (pos2 == std::string::npos){
      fCurrentPos = pos2;
      tmpstring   = fLine.substr(pos1);
    } else {
      fCurrentPos = fLine.find_first_not_of(separatorchars.c_str(), pos2);
      tmpstring   = fLine.substr(pos1,pos2-pos1);
    }
  }
  //
  //TrimWhitespace(tmpstring,TString::kBoth);
  // TString wont mind having white spaces when it converts string to int/double/usign

  outstring = TString(tmpstring);
  return Atof(outstring);
}


