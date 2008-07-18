///  @file QwParameterFile.cc

#include "QwParameterFile.h"

#include <sstream>

std::vector<bfs::path> QwParameterFile::fSearchPaths;

void QwParameterFile::AppendToSearchPath(const TString &searchdir){
  bfs::path tmppath(searchdir.Data());
  if( bfs::exists(tmppath) && bfs::is_directory(tmppath)) {
    std::cout << tmppath.string()
	      << " is a directory; adding it to the search path\n";
    fSearchPaths.push_back(tmppath);
  } else if( bfs::exists(tmppath)) {
    std::cout<<tmppath.string()<<" exists but is not a directory.\n";
  } else {
    std::cout<<tmppath.string()<<" doesn't exist.\n";
  }
};

UInt_t QwParameterFile::GetUInt(const TString &varvalue){
  UInt_t value = 0;
  if (varvalue.IsDigit()){
    value = varvalue.Atoi();
  } else if (varvalue.BeginsWith("0x") || varvalue.BeginsWith("0X") 
	     || varvalue.BeginsWith("x") || varvalue.BeginsWith("X")
	     || varvalue.IsHex()){
    //any whitespace ?
    Int_t end = varvalue.Index(" ");
    std::istringstream stream1;
    if (end == -1){
      stream1.str(varvalue.Data());
    } else {
      //make temporary string, removing whitespace
      Int_t start = 0;
      TString tmp;
      //loop over all whitespace
      while (end > -1) {
	tmp += varvalue(start, end-start);
	start = end+1; 
	end = varvalue.Index(" ", start);
      }
      //finally add part from last whitespace to end of string
      end = varvalue.Length();
      tmp += varvalue(start, end-start);
      stream1.str(tmp.Data());
      }
    stream1 >> std::hex >> value;
  }
  return value;
};


QwParameterFile::QwParameterFile(const char *filename){
  bfs::path tmppath;
  if (filename[0] == '/'){
    tmppath = bfs::path(filename);
  } else {
    for (size_t i=0; i<fSearchPaths.size(); i++){
      tmppath = fSearchPaths.at(i).string() +"/"+ filename;
      if( bfs::exists(tmppath) && ! bfs::is_directory(tmppath)) {
	std::cout << "Found parameter file: " 
		  << tmppath.string()<<"\n";
	break;
      }
    }
  }
  fInputFile.open(tmppath.string().c_str());
};

void QwParameterFile::TrimWhitespace(TString::EStripType head_tail){
  //  If the first bit is set, this routine removes leading spaces from the
  //  line.  If the second bit is set, this routine removes trailing spaces
  //  from the line.  The default behavior is to remove both.
  TrimWhitespace(fLine, head_tail);
}

void QwParameterFile::TrimWhitespace(std::string &token, 
				     TString::EStripType head_tail){
  //  If the first bit is set, this routine removes leading spaces from the
  //  line.  If the second bit is set, this routine removes trailing spaces
  //  from the line.  The default behavior is to remove both.
  std::string   ws = " \t";
  size_t mypos;
  //  Remove leading spaces.  If this first test returns "npos", it means
  //  this line is all whitespace, so get rid of it all.
  //  If we're not removing leading spaces, lines which are all spaces
  //  will not be removed.
  mypos = token.find_first_not_of(ws);
  if (head_tail & TString::kLeading) token.erase(0,mypos);
  //  Remove trailing spaces
  mypos = token.find_last_not_of(ws);
  mypos = token.find_first_of(ws,mypos);
  if (mypos != std::string::npos && (head_tail & TString::kTrailing)){
    token.erase(mypos);
  }
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
      TrimWhitespace(varname,  TString::kBoth);
      TrimWhitespace(varvalue, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
};
  

std::string QwParameterFile::GetNextToken(std::string separatorchars){
  std::string tmpstring = "";
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
  return tmpstring;
};

