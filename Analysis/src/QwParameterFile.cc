///  @file QwParameterFile.cc

#include "QwParameterFile.h"

// System headers
#include <sstream>

// Qweak headers
#include "QwLog.h"

// Initialize the list of search paths
std::vector<bfs::path> QwParameterFile::fSearchPaths;

// Set current run number to zero
UInt_t QwParameterFile::fCurrentRunNumber = 0;

/**
 * Append a directory to the list of search paths
 * @param searchdir Directory to be added
 */
void QwParameterFile::AppendToSearchPath(const TString& searchdir)
{
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

/**
 * Convert a string number value to an unsigned integer
 * @param varvalue String with decimal or hexadecimal number
 * @return Unsigned integer
 */
UInt_t QwParameterFile::GetUInt(const TString& varvalue)
{
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
  bfs::path tmppath = bfs::path(filename);
  if (filename[0] == '/') {
    tmppath = bfs::path(filename);
  } else {
    for (size_t i = 0; i < fSearchPaths.size(); i++) {
      tmppath = fSearchPaths.at(i).string() + "/" + filename;
      if (bfs::exists(tmppath) && ! bfs::is_directory(tmppath)) {
        break;
      }
    }
  }
  if (bfs::exists(tmppath)){
    QwMessage << "Opening parameter file: "
              << tmppath.string() << QwLog::endl;
    fFile.open(tmppath.string().c_str());
    fStream << fFile.rdbuf();
  } else {
    QwError << "Unable to open parameter file: "
            << tmppath.string() << QwLog::endl;
  }
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

void QwParameterFile::TrimWhitespace(TString &tok,
				     TString::EStripType head_tail){
  //  If the first bit is set, this routine removes leading spaces from the
  //  line.  If the second bit is set, this routine removes trailing spaces
  //  from the line.  The default behavior is to remove both.
  std::string token;
  token=tok.Data();
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
  tok=token;
}

void QwParameterFile::TrimComment(char commentchar){
  //  Remove everything after the comment character
  size_t mypos = fLine.find_first_of(commentchar);
  if (mypos != std::string::npos){
    fLine.erase(mypos);
  }
}

Bool_t QwParameterFile::HasValue(TString& vname){
  Bool_t status=kFALSE;
  TString vline;
  
  RewindToFileStart();
  while (ReadNextLine()){
    TrimWhitespace();
    vline=(GetLine()).c_str();
    
    vline.ToLower();
    TRegexp regexp(vline);
    vname.ToLower();
    if (vname.Contains(regexp)){
      QwMessage <<" QwParameterFile::HasValue  "<<vline<<" "<<vname<<QwLog::endl;
      status=kTRUE;
      break;
    }      
  }

  return status;
}


Bool_t QwParameterFile::HasVariablePair(std::string separatorchars, TString &varname, TString &varvalue)
{
  std::string tmpvar, tmpval;
  Bool_t status = HasVariablePair(separatorchars, tmpvar, tmpval);
  if (status){
    varname  = tmpvar.c_str();
    varvalue = tmpval.c_str();
  }
  return status;
};

Bool_t QwParameterFile::HasVariablePair(std::string separatorchars, std::string &varname, std::string &varvalue)
{
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

Bool_t QwParameterFile::FileHasVariablePair(
  std::string separatorchars, const TString& varname, TString& varvalue)
{
  std::string tmpval;
  Bool_t status = FileHasVariablePair(separatorchars, varname.Data(), tmpval);
  if (status) varvalue = tmpval.c_str();
  return status;
};

Bool_t QwParameterFile::FileHasVariablePair(
  std::string separatorchars, const std::string& varname, std::string& varvalue)
{
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_varname;
    if (HasVariablePair(separatorchars, this_varname, varvalue)) {
      if (this_varname == varname) return kTRUE;
    }
  }
  return false;
};


Bool_t QwParameterFile::LineHasSectionHeader(TString& secname)
{
  TrimComment('#');
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of('[');
  if (equiv_pos1 != std::string::npos) {
    size_t equiv_pos2 = fLine.find_first_of(']',equiv_pos1);
    if (equiv_pos2 != std::string::npos && equiv_pos2 - equiv_pos1 > 1) {
      secname = fLine.substr(equiv_pos1 + 1, equiv_pos2 - equiv_pos1 - 1);
      TrimWhitespace(secname, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
  //return LineHasSectionHeader(secname);
};

Bool_t QwParameterFile::LineHasSectionHeader(std::string& secname)
{
  TrimComment('#');
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of('[');
  if (equiv_pos1 != std::string::npos) {
    size_t equiv_pos2 = fLine.find_first_of(']',equiv_pos1);
    if (equiv_pos2 != std::string::npos && equiv_pos2 - equiv_pos1 > 1) {
      secname = fLine.substr(equiv_pos1 + 1, equiv_pos2 - equiv_pos1 - 1);
      TrimWhitespace(secname, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
};

Bool_t QwParameterFile::LineHasModuleHeader(TString& secname)
{
  TrimComment('#');
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of('<');
  if (equiv_pos1 != std::string::npos) {
    size_t equiv_pos2 = fLine.find_first_of('>',equiv_pos1);
    if (equiv_pos2 != std::string::npos && equiv_pos2 - equiv_pos1 > 1) {
      secname = fLine.substr(equiv_pos1 + 1, equiv_pos2 - equiv_pos1 - 1);
      TrimWhitespace(secname, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
 
};

Bool_t QwParameterFile::LineHasModuleHeader(std::string& secname)
{
  TrimComment('#');
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of('<');
  if (equiv_pos1 != std::string::npos) {
    size_t equiv_pos2 = fLine.find_first_of('>',equiv_pos1);
    if (equiv_pos2 != std::string::npos && equiv_pos2 - equiv_pos1 > 1) {
      secname = fLine.substr(equiv_pos1 + 1, equiv_pos2 - equiv_pos1 - 1);
      TrimWhitespace(secname, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
};


Bool_t QwParameterFile::FileHasSectionHeader(const TString& secname)
{

  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasSectionHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
};

Bool_t QwParameterFile::FileHasSectionHeader(const std::string& secname)
{
  
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasSectionHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
};

Bool_t QwParameterFile::FileHasModuleHeader(const TString& secname)
{

  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasModuleHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
};

Bool_t QwParameterFile::FileHasModuleHeader(const std::string& secname)
{
  
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasModuleHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
};

/**
 * Read from current position until next section header
 * @return Pointer to the parameter stream until next section
 */
QwParameterFile* QwParameterFile::ReadUntilNextSection()
{
  std::string nextheader; // dummy
  QwParameterFile* section = new QwParameterFile();
  while (ReadNextLine() && ! LineHasSectionHeader(nextheader)) {
    section->AddLine(GetLine());
  }
  return section;
}

/**
 * Read from current position until next module header
 * @return Pointer to the parameter stream until next module
 */
QwParameterFile* QwParameterFile::ReadUntilNextModule()
{
  std::string nextheader; // dummy
  QwParameterFile* section = new QwParameterFile();
  while (ReadNextLine() && ! LineHasModuleHeader(nextheader)) {
    section->AddLine(GetLine());
  }
  return section;
}

/**
 * Read the lines until the first header
 * @return Pointer to the parameter stream until first section
 */
QwParameterFile* QwParameterFile::ReadPreamble()
{
  RewindToFileStart();
  return ReadUntilNextSection();
}

/**
 * Read the lines of the next section
 * @param secname Name of the next section (returns)
 * @return Pointer to the parameter stream of the next section
 */
QwParameterFile* QwParameterFile::ReadNextSection(std::string &secname)
{
  if (IsEOF()) return 0;
  while (! LineHasSectionHeader(secname) && ReadNextLine()); // skip until header
  return ReadUntilNextSection();
}

QwParameterFile* QwParameterFile::ReadNextSection(TString &secname)
{
  if (IsEOF()) return 0;
  
  while (! LineHasSectionHeader(secname) && ReadNextLine()); // skip until header
  
  return ReadUntilNextSection();
}

/**
 * Read the lines of the next module
 * @param secname Name of the next module (returns)
 * @return Pointer to the parameter stream of the next module
 */
QwParameterFile* QwParameterFile::ReadNextModule(std::string &secname)
{
  if (IsEOF()) return 0;
  while (! LineHasModuleHeader(secname) && ReadNextLine()); // skip until header
  return ReadUntilNextModule();
}

QwParameterFile* QwParameterFile::ReadNextModule(TString &secname)
{
  if (IsEOF()) return 0;
  
  while (! LineHasModuleHeader(secname) && ReadNextLine()); // skip until header
  
  return ReadUntilNextModule();
}

std::string QwParameterFile::GetNextToken(std::string separatorchars)
{
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
  TrimWhitespace(tmpstring,TString::kBoth);
  return tmpstring;
};

ostream& operator<< (ostream& stream, const QwParameterFile& file)
{
  /// \todo TODO (wdc) operator<< on QwParameterFile requires RewindToFileStart
  std::string line;
  stream << file.fStream.rdbuf();
  return stream;
}
