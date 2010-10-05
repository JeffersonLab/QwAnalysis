///  @file QwParameterFile.cc

#include "QwParameterFile.h"

// System headers
#include <sstream>
#include <climits>

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


/**
 * Constructor
 * @param name Name of the file to be opened
 *
 * If file starts with an explicit slash ('/'), it is assumed to be a full path.
 */
QwParameterFile::QwParameterFile(const std::string& name)
{
  // Create a file from the name
  bfs::path file(name);

  // Immediately try to open absolute paths and return
  if (name.find("/") == 0) {
    if (! OpenFile(file))
      QwWarning << "Constructor could not open absolute path " << name << ". "
                << "Parameter file will remain empty." << QwLog::endl;
    return;

  // Else, loop through search path and files
  } else {

#if BOOST_VERSION >= 103600
    // Separate file in stem and extension
    std::string file_stem = file.stem();
    std::string file_ext = file.extension();
#else
    // Separate file in stem and extension
    std::string file_stem = bfs::basename(file);
    std::string file_ext = bfs::extension(file);
#endif

    // Find the best match
    int best_score = 0;
    bfs::path best_path;
    for (size_t i = 0; i < fSearchPaths.size(); i++) {

      bfs::path path;
      int score = FindFile(fSearchPaths[i], file_stem, file_ext, path);
      if (score > best_score) {
        // Found file with better score
        best_score = score;
        best_path  = path;
      } else if (score == best_score) {
        // Found file with identical score
        QwWarning << "Equally likely parameter files encountered: " << best_path.string()
                  << " and " << path.string() << QwLog::endl;
      }

    } // end of loop over search paths
    if (OpenFile(best_path) == false)
      QwError << "Could not open parameter file "
              << best_path.string() << QwLog::endl;
  }
};


/**
 * Open a file at the specified location
 * @param path Path to file to be opened
 * @return False if the file could not be opened
 */
bool QwParameterFile::OpenFile(const bfs::path& file)
{
  bool status = false;

  // Check whether path exists and is a regular file
#if BOOST_VERSION >= 103400
  if (bfs::exists(file) && bfs::is_regular_file(file)) {
#else
  if (bfs::exists(file) /* pray */ ) {
#endif
    QwMessage << "Opening parameter file: "
              << file.string() << QwLog::endl;
    // Open file
    fFile.open(file.string().c_str());
    if (! fFile.good())
      QwError << "Unable to read parameter file "
              << file.string() << QwLog::endl;
    // Load into stream
    fStream << fFile.rdbuf();
    status = true;

  } else {

    // File does not exist or is not a regular file
    QwError << "Unable to open parameter file "
            << file.string() << QwLog::endl;
    status = false;
  }

  return status;
}


/**
 * Find the file in a directory with highest-scoring run label
 * @param directory Directory to search in
 * @param file_stem File name stem to search for
 * @param file_ext File name extensions to search for
 * @param best_path (returns) Path to the highest-scoring file
 * @return Score of file
 */
int QwParameterFile::FindFile(
        const bfs::path&   directory,
        const std::string& file_stem,
        const std::string& file_ext,
        bfs::path&         best_path)
{
  // Return false if the directory does not exist
  if (! bfs::exists(directory)) return false;

  // Default score indicates no match found
  int best_score = -1;
  int score = -1;
  // Multiple overlapping open-ended ranges
  int open_ended_latest_start = 0;
  int open_ended_range_score = 0;

  // Loop over all files in the directory
  // note: default iterator constructor yields past-the-end
  bfs::directory_iterator end_iterator;
  for (bfs::directory_iterator file_iterator(directory);
       file_iterator != end_iterator;
       file_iterator++) {

    // Match the stem and extension
    // note: filename() returns only the file name, not the path
#if BOOST_VERSION >= 103600
    std::string file_name = file_iterator->filename();
#else
    std::string file_name = file_iterator->leaf();
#endif
    // stem
    size_t pos_stem = file_name.find(file_stem);
    if (pos_stem != 0) continue;
    // extension (reverse find)
    size_t pos_ext = file_name.rfind(file_ext);
    if (pos_ext != file_name.length() - file_ext.length()) continue;

    // Determine run label length
    size_t label_length = pos_ext - file_stem.length();
    // no run label
    if (label_length == 0) {
      score = 10;
    } else {
      // run label starts after dot ('.') and that dot is included in the label length
      if (file_name.at(pos_stem + file_stem.length()) == '.') {
        std::string label = file_name.substr(pos_stem + file_stem.length() + 1, label_length - 1);
        std::pair<int,int> range = ParseIntRange("-",label);
        int run = fCurrentRunNumber;
        if ((range.first <= run) && (run <= range.second)) {
          // run is in single-value range
          if (range.first == range.second) score = 1000;
          // run is in double-value range
          else if (range.second < INT_MAX) score = 100;
          // run is in open-ended range
          else if (range.second == INT_MAX) {
            // each matching open-ended range
            if (range.first > open_ended_latest_start) {
              open_ended_latest_start = range.first;
              open_ended_range_score++;
              score = 10 + open_ended_range_score;
              // 90 open-ended range files should be enough for anyone ;-)
            } else score = 10;
          }
        } else
          // run not in range
          score = -1;
      } else
        // run label does not start with a dot (i.e. partial match of stem)
        score = -1;
    }

    // Look for the match with highest score
    if (score > best_score) {
      best_path = *file_iterator;
      best_score = score;
    }
  }
  return best_score;
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
  return tmpstring;
};

ostream& operator<< (ostream& stream, const QwParameterFile& file)
{
  /// \todo TODO (wdc) operator<< on QwParameterFile requires RewindToFileStart
  std::string line;
  stream << file.fStream.rdbuf();
  return stream;
}


/** @brief Separate a separated range of integers into a pair of values
 *
 *  @param separatorchars String with possible separator characters to consider.
 *  @param range String containing two integers separated by a separator character,
 *               or a single value.
 *               If the string begins with the separator character, the first value
 *               is taken as zero.  If the string ends with the separator character,
 *               the second value is taken as kMaxInt.
 *
 *  @return  Pair of integers of the first and last values of the range.
 *           If the range contains a single value, the two integers will
 *           be identical.
 */
std::pair<int,int> QwParameterFile::ParseIntRange(const std::string& separatorchars, const std::string& range)
{
  std::pair<int,int> mypair;
  size_t pos = range.find_first_of(separatorchars);
  if (pos == std::string::npos) {
    //  Separator not found.
    mypair.first  = atoi(range.substr(0,range.length()).c_str());
    mypair.second = mypair.first;
  } else {
    size_t end = range.length() - pos - 1;
    if (pos == 0) {
      // Separator is the first character
      mypair.first  = 0;
      mypair.second = atoi(range.substr(pos+1, end).c_str());
    } else if (pos == range.length() - 1) {
      // Separator is the last character
      mypair.first  = atoi(range.substr(0,pos).c_str());
      mypair.second = INT_MAX;
    } else {
      mypair.first  = atoi(range.substr(0,pos).c_str());
      mypair.second = atoi(range.substr(pos+1, end).c_str());
    }
  }

  //  Check the values for common errors.
  if (mypair.first < 0){
    QwError << "The first value must not be negative!" << QwLog::endl;
    return std::pair<int,int>(INT_MAX,INT_MAX);
  } else if (mypair.first > mypair.second){
    QwError << "The first value must not be larger than the second value"
            << QwLog::endl;
    return std::pair<int,int>(INT_MAX,INT_MAX);
  }

  //  Print the contents of the pair for debugging.
  QwVerbose << "The range goes from " << mypair.first
            << " to " << mypair.second << QwLog::endl;

  return mypair;
};


