/**********************************************************\

 * Buddhini Waidyawansa
 * 03-04-2014

\**********************************************************/

#include "QwReadWrite.h"
#include <cstdlib>

QwReadFile::QwReadFile(const char* name){
  // try to open file
  std::ifstream fRfile(name);

  if (!fRfile.is_open()) {
    std::cout<<"Unable to open file "<<name<<std::endl;
    return;
  } else{
    // found file
    std::cout<<"Opened file "<<name<<std::endl;
    
    // Load into stream
    fStream << fRfile.rdbuf();

  }
}


// remove comments
void QwReadFile::TrimComment(const std::string& commentchars){
  // Remove everything after the comment character
  size_t mypos = fLine.find_first_of(commentchars);
  if (mypos != std::string::npos){
    fLine.erase(mypos);
  }
}

// get next token
float QwReadFile::GetNextToken(const std::string& separatorchars){
  std::string tmpstring;

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

  return std::atof(tmpstring.c_str());
}

// get pair of tokens. In this case, value and error.
void QwReadFile::GetTokenPair(const std::string& separatorchars,
			      float*value, 
			      float*error){
  *value = GetNextToken(separatorchars);
  *error = GetNextToken(separatorchars);  
}



