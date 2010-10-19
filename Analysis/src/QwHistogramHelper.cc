///  @file  QwHistogramHelper.cc
///  @brief Handler class for histogram/tree trim parameter files and
///         histogram and Tree branch creation
///
///  This class was originally written to be instantiated as a
///  global object, which all classes containing histograms would
///  use as an external object.
///  It could be repurposed to have a single instance per class,
///  but that would probably involve increasing the number of
///  histogram parameter files.

#include "QwHistogramHelper.h"

// Qweak headers
#include "QwLog.h"

///  Globally defined instance of the QwHistogramHelper class.
QwHistogramHelper gQwHists;


const Double_t QwHistogramHelper::fInvalidNumber  = -1.0e7;
const std::string QwHistogramHelper::fInvalidName = "EmptyEmpty";

void QwHistogramHelper::DefineOptions(QwOptions &options)
{
  // Define the histogram and tree options
  options.AddOptions()
    ("enable-tree-trim", po::value<bool>()->default_value(false)->zero_tokens(),
     "enable trimmed trees");

  options.AddOptions()(
		       "tree-trim-file",  
		       po::value<string>()->default_value("tree_trim.in"), 
		       "trimmed tree file name, default of tree_trim.in"
		       );
  
  // What about QwTracking ? 
  // Monday, October 18 23:19:09 EDT 2010, jhlee
  options.AddOptions()(
		       "histo-trim-file", 
		       po::value<string>()->default_value("parity_hist.in"),
		       "trimmed histo file name, default of parity_hist.in"
		       );

};

void QwHistogramHelper::ProcessOptions(QwOptions &options){
  //enable the tree trim when  --enable-tree-trim in offline mode or --enable-mapfile for real time mode
  fTrimDisable =!( options.GetValue<bool>("enable-tree-trim") || options.GetValue<bool>("enable-mapfile"));
  fTrimHistoEnable = options.GetValue<bool>("enable-mapfile");
  
  if (fTrimDisable)
    QwMessage <<"tree-trim is disabled"<<QwLog::endl;
  else
    QwMessage <<"tree-trim is enabled"<<QwLog::endl;

  if (fTrimHistoEnable)
    QwMessage <<"RT Mode: histo-trim is enabled "<<QwLog::endl;
  else
    QwMessage <<"histo-trim is disabled "<<QwLog::endl;

  // Process trim file options
  if (options.HasValue("tree-trim-file"))
    LoadTreeParamsFromFile(options.GetValue<string>("tree-trim-file"));
  if (options.HasValue("histo-trim-file"))
    LoadHistParamsFromFile(options.GetValue<string>("histo-trim-file"));
};


const QwHistogramHelper::HistParams QwHistogramHelper::GetHistParamsFromLine(QwParameterFile &mapstr)
{
  ///  Decodes the histogram parmeters from the current line of
  ///  a QwParameter file.
  ///  If the line cannot be docoded, the name is returned as
  ///  fInvalidName.
  HistParams tmpstruct;
  tmpstruct.name_title = fInvalidName;

  std::string tmpname, tmptype, tmpmin, tmpmax;

  tmpname = mapstr.GetNextToken(" \t");
  tmptype = mapstr.GetNextToken(" \t");
  if (tmptype != "TH1F" && tmptype != "TH2F"){
    std::cerr << "QwHistogramHelper::GetHistParamsFromFile:  Unrecognized histogram type: "
	      << tmptype << " (tmpname==" << tmpname <<")"<<  std::endl;
  } else {
    tmpstruct.name_title = tmpname;
    tmpstruct.type       = tmptype;
    tmpstruct.x_nbins    = atoi(mapstr.GetNextToken(" \t").c_str());
    tmpstruct.x_min      = atof(mapstr.GetNextToken(" \t").c_str());
    tmpstruct.x_max      = atof(mapstr.GetNextToken(" \t").c_str());
    if (tmptype=="TH2F"){
      tmpstruct.y_nbins    = atoi(mapstr.GetNextToken(" \t").c_str());
      tmpstruct.y_min      = atof(mapstr.GetNextToken(" \t").c_str());
      tmpstruct.y_max      = atof(mapstr.GetNextToken(" \t").c_str());
    } else {
      tmpstruct.y_nbins    = 0;
      tmpstruct.y_min      = 0.0;
      tmpstruct.y_max      = 0.0;
    }
    tmpstruct.xtitle     = mapstr.GetNextToken(" \t");
    tmpstruct.ytitle     = mapstr.GetNextToken(" \t");

    tmpmin               = mapstr.GetNextToken(" \t");
    if(tmpmin.find_first_not_of("-+1234567890.eE")==std::string::npos){
      //  tmpmin is a number
      tmpstruct.min = atof(tmpmin.c_str());
    } else {
      tmpstruct.min = fInvalidNumber;
    }
    tmpmax               = mapstr.GetNextToken(" \t");
    if(tmpmax.find_first_not_of("-+1234567890.eE")==std::string::npos){
      //  tmpmax is a number
      tmpstruct.max = atof(tmpmax.c_str());
    } else {
      tmpstruct.max = fInvalidNumber;
    }
  }
  return tmpstruct;
};



void  QwHistogramHelper::LoadHistParamsFromFile(const std::string filename)
{
  fInputFile = filename;
  HistParams tmpstruct;

  fDEBUG = 0;
  //fDEBUG = 1;

  if (fDEBUG) std::cout<< "file name "<<fInputFile<<std::endl;
  //Important to empty the fHistParams to reload the real time histo difinition file
  if (fTrimHistoEnable)
    fHistParams.clear();
  
  QwParameterFile mapstr(filename.c_str());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('#');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    tmpstruct = GetHistParamsFromLine(mapstr);
    if (tmpstruct.name_title != fInvalidName){
      fHistParams.push_back(tmpstruct);
      if (fDEBUG) {
        QwMessage << fHistParams.back() << QwLog::endl;
      }
    }
  }

  // Sort the histogram parameter definitions
  sort(fHistParams.begin(), fHistParams.end());
};


void  QwHistogramHelper::PrintHistParams() const 
{
  for (std::vector<HistParams>::const_iterator h = fHistParams.begin();
       h != fHistParams.end(); ++h) {
    QwMessage << *h << QwLog::endl;
  }
};


void  QwHistogramHelper::LoadTreeParamsFromFile(const std::string filename){
  TString devicename;
  TString moduletype;
  TString subsystemname;
  QwParameterFile *section;
  QwParameterFile *module;
  std::vector<TString> TrimmedList;//stores the list of elements for each module
  std::vector<std::vector<TString> > ModulebyTrimmedList;//stores the list of elements for each module
  std::vector<TString> ModuleList;//stores the list of modules for each subsystem
  fDEBUG = 0;
  //fDEBUG = 1;  
  if (fTrimDisable)
    return;
  QwMessage << "Tree trim definition file for Offline Engine"<< QwLog::endl;
  QwParameterFile mapstr(filename.c_str());  //Open the file
 
  fTreeTrimFileLoaded=!mapstr.IsEOF();  
  fSubsystemList.clear();
  fModuleList.clear();
  fVQWKTrimmedList.clear();
  
  while ( (section=mapstr.ReadNextSection(subsystemname)) ){
    if (subsystemname=="DEVICELIST")//done with VQWK element trimming
      break;
    fSubsystemList.push_back(subsystemname);
    QwMessage <<"Subsystem found "<<subsystemname<<QwLog::endl;  

    ModuleList.clear();
    ModulebyTrimmedList.clear();
    while ( (module=section->ReadNextModule(moduletype)) ){
 
      ModuleList.push_back(moduletype);
      QwMessage <<"Module found "<<moduletype<<QwLog::endl;
      TrimmedList.clear();
      while (module->ReadNextLine()){
	module->TrimComment('#');   // Remove everything after a '#' character.
	module->TrimWhitespace();   // Get rid of leading and trailing spaces.
	if (module->LineIsEmpty())  continue;
	devicename=(module->GetLine()).c_str();
	TrimmedList.push_back(devicename);
	if (fDEBUG) {
	QwMessage <<"data element "<<devicename<<QwLog::endl;	
	}
      }
      ModulebyTrimmedList.push_back(TrimmedList);
	  

    }
    fModuleList.push_back(ModuleList);
    fVQWKTrimmedList.push_back(ModulebyTrimmedList);

    
  }
  //Start decoding the device list in the section [DEVICELIST]
  fTreeParams.clear();
  while (section->ReadNextLine()){
    section->TrimComment('#');   // Remove everything after a '#' character.
    section->TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (section->LineIsEmpty())  continue;
    devicename=(section->GetLine()).c_str();
    fTreeParams.push_back(devicename);
    if (fDEBUG)
      QwMessage <<"device name "<<devicename<<QwLog::endl;	
    
  }

  //exit(1);

};


const QwHistogramHelper::HistParams QwHistogramHelper::GetHistParamsFromList(const std::string histname)
{
  HistParams tmpstruct, matchstruct;
  tmpstruct.name_title = fInvalidName;

  std::vector<int> matches;
  for (size_t i = 0; i < fHistParams.size(); i++) {
    if (DoesMatch(histname,fHistParams.at(i).name_title)) {
      matchstruct = fHistParams.at(i);
      if (tmpstruct.name_title == fInvalidName){
        tmpstruct = matchstruct;
        tmpstruct.name_title = histname;
        matches.push_back(i);
        break; // enabled (to get warnings for multiple definitions, disable)
      } else if (tmpstruct.nbins == matchstruct.nbins
              && tmpstruct.min   == matchstruct.min
              && tmpstruct.max   == matchstruct.max
              && tmpstruct.x_nbins == matchstruct.x_nbins
              && tmpstruct.x_min   == matchstruct.x_min
              && tmpstruct.x_max   == matchstruct.x_max
              && tmpstruct.y_nbins == matchstruct.y_nbins
              && tmpstruct.y_min   == matchstruct.y_min
              && tmpstruct.y_max   == matchstruct.y_max) {
        //matches.push_back(i); // disabled (to enable, also remove break above)
      }
    }
  }

  // Warn when multiple identical matches were found
  if (matches.size() > 1) {
    QwWarning << "Multiple identical matches for histogram " << histname << ":" << QwLog::endl;
    for (size_t i = 0; i < matches.size(); i++) {
      QwWarning << " " << fHistParams.at(matches.at(i)).name_title << QwLog::endl;
    }
  }

  fDEBUG = 0;
  if (fDEBUG) {
    QwMessage << "Finding histogram defination from: " << histname << QwLog::endl;
    QwMessage << tmpstruct << QwLog::endl;
  }
  if (tmpstruct.name_title == fInvalidName && !fTrimHistoEnable){
    std::cerr << "GetHistParamsFromList:  We haven't found a match of the histogram name: "
	      << histname << std::endl;
    std::cerr << "                        Please check the input file "
	      << fInputFile << std::endl;
    exit(1);
  }
  return tmpstruct;
};


const Bool_t QwHistogramHelper::MatchDeviceParamsFromList(const std::string devicename){
  Int_t matched;
  matched=0;
  if (!fTreeTrimFileLoaded || fTrimDisable){//if file is not loaded or trim tree is disable by cmd flag
    
    return kTRUE;//return true for all devices
  }
  for (size_t i = 0; i < fTreeParams.size(); i++) {
    if (DoesMatch(devicename,fTreeParams.at(i).Data())) {
      if (fDEBUG)
	QwMessage << " Branch name found "<<fTreeParams.at(i).Data()<<  QwLog::endl;
      matched++;
    }
  }

  // Warn when multiple identical matches were found
  if (matched > 1) {
    QwWarning << "Multiple identical matches for branch name " <<devicename  << ":" << QwLog::endl;
  }
  if (matched)
    return kTRUE;
  else
    return kFALSE;
};

const Bool_t QwHistogramHelper::MatchVQWKElementFromList(const std::string subsystemname, const std::string moduletype, const std::string elementname){
  Int_t matched;
  matched=0;
  if (!fTreeTrimFileLoaded || fTrimDisable){//if file is not loaded or trim tree is disable by cmd flag

    return kTRUE;//return true for all devices
  }
  
  for (size_t j = 0; j < fSubsystemList.size(); j++) {
    //    QwMessage << " Subsystem name "<< subsystemname<< " From List "<<fSubsystemList.at(j) <<  QwLog::endl;
    if (DoesMatch(fSubsystemList.at(j).Data(),subsystemname)){
      for (size_t i = 0; i < fModuleList.at(j).size(); i++) {
	if (DoesMatch(moduletype,fModuleList.at(j).at(i).Data())) {
	  for (size_t k = 0; k < fVQWKTrimmedList.at(j).at(i).size(); k++) {
	    if (DoesMatch(elementname,fVQWKTrimmedList.at(j).at(i).at(k).Data() )){
	      if (fDEBUG)
		QwMessage <<"Subsystem "<<fSubsystemList.at(j).Data()<<" Module Type "<<fModuleList.at(j).at(i).Data()<< " Element "<<fVQWKTrimmedList.at(j).at(i).at(k).Data()<<  QwLog::endl;
	      matched++;
	    }
	  }
	  break;
	}
      }
    }
  }

  
  // Warn when multiple identical matches were found
  if (matched > 1) {
    QwWarning << "Multiple identical matches for element name " <<elementname << ":" << QwLog::endl;
  }
  if (matched)
    return kTRUE;
  else
    return kFALSE;
};


const QwHistogramHelper::HistParams QwHistogramHelper::GetHistParamsFromFile(const std::string filename,
									    const std::string histname)
{
  //The idea is to look up the input file and get the needed histogram parameters
  //For each histogram we are going to scan the input file once, which
  //is not very efficent. But we only construct histograms once per run ...

  HistParams tmpstruct;
  tmpstruct.name_title = fInvalidName;

  //   Switch to using the QwParameterFile system...
  std::string tmpname, tmptype;

  QwParameterFile mapstr(filename.c_str());  //Open the file
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('#');   // Remove everything after a '#' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    tmpname = mapstr.GetNextToken(" \t");
    tmptype = mapstr.GetNextToken(" \t");
    if (tmptype != "TH1F" && tmptype != "TH2F"){
      std::cerr << "QwHistogramHelper::GetHistParamsFromFile:  Unrecognized histogram type: "
		<< tmptype << " (tmpname==" << tmpname <<")"<< std::endl;
      continue;
    }
    if (DoesMatch(histname,tmpname)){
      //  found a match
      mapstr.RewindToLineStart();
      tmpstruct = GetHistParamsFromLine(mapstr);
      tmpstruct.name_title=histname;
    }
  }
  if (fDEBUG) {
    QwMessage << tmpstruct << QwLog::endl;
  }
  if (tmpstruct.name_title == fInvalidName && !fTrimHistoEnable){
    std::cerr << "GetHistParamsFromFile:  We haven't found a match of the histogram name: "
	      << histname << std::endl;
    std::cerr << "                        Please check the input file "
	      << filename << std::endl;
    exit(1);
  }
  return tmpstruct;
};

Bool_t QwHistogramHelper::DoesMatch(const std::string s, const std::string s_wildcard)
{ //A very quick and dirty string matching routine using root
  //TString and TRegExp functions. Require the string and wildcard string
  //to have the SAME length to match (much risky if we don't require this),
  //so the only wildcard you want to use here is ".".

  if (s_wildcard.length()==0)
    return kFALSE;

  TString s1 = TString(s.c_str());
  TRegexp s2 = TRegexp(s_wildcard.c_str());
  Ssiz_t len = 0;
  if (s.length() == s_wildcard.length()
     && s2.Index(s1,&len) == 0 && len == s1.Length()) { // found a match!
    return kTRUE;
  } else if (s2.Index(s1,&len) == 0 && len == s1.Length()) { // found a match!
    //std::cout << "QwHistogramHelper:  Matching histogram \"" << s.c_str()<< "\" to wildcard pattern \"" << s_wildcard.c_str() << "\"." << std::endl;
    return kTRUE;
  }
  else return kFALSE;
};

/////////////////////////////////////////////////////////////////////////////////////////

TH2F* QwHistogramHelper::Construct2DHist(const std::string name_title)
{
  HistParams tmpstruct = GetHistParamsFromList(name_title);
  return Construct2DHist(tmpstruct);
};

/////////////////////////////////////////////////////////////////////////////////////////

TH1F* QwHistogramHelper::Construct1DHist(const std::string name_title)
{
  HistParams tmpstruct = GetHistParamsFromList(name_title);
  return Construct1DHist(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////

TH2F* QwHistogramHelper::Construct2DHist(const std::string inputfile, const std::string name_title)
{
  HistParams tmpstruct = GetHistParamsFromFile(inputfile, name_title);
  return Construct2DHist(tmpstruct);
};

/////////////////////////////////////////////////////////////////////////////////////////

TH1F* QwHistogramHelper::Construct1DHist(const std::string inputfile, const std::string name_title)
{
  HistParams tmpstruct = GetHistParamsFromFile(inputfile, name_title);
  return Construct1DHist(tmpstruct);
}

/////////////////////////////////////////////////////////////////////////////////////////
TH1F* QwHistogramHelper::Construct1DHist(const QwHistogramHelper::HistParams &params)
{
  TH1F* h1;
  std::string tmptitle;
  tmptitle = params.name_title; //now title=name
  //std::cout<<params.name_title.c_str()<<" : "<<params.unit.c_str()<<std::endl;
  //std::cout<<tmptitle<<std::endl;
  if (fTrimHistoEnable && tmptitle==fInvalidName){
    h1=NULL;
    return h1;
  }
    

  h1 = new TH1F(params.name_title.c_str(),
                tmptitle.c_str(),
                params.x_nbins,
                Double_t(params.x_min),Double_t(params.x_max));
  h1->SetXTitle(params.xtitle.c_str());
  h1->SetYTitle(params.ytitle.c_str());
  //  if(params.min!=fInvalidNumber) h1->SetMinimum(params.min);
  //  if(params.max!=fInvalidNumber) h1->SetMinimum(params.max);
  return h1;
};

TH2F* QwHistogramHelper::Construct2DHist(const QwHistogramHelper::HistParams &params)
{
  TH2F* h2;
  std::string tmptitle;
  tmptitle = params.name_title; //now title=name.
  //std::cout<<params.name_title.c_str()<<" : "<<params.unit.c_str()<<std::endl;
  //std::cout<<tmptitle<<std::endl;

  if (fTrimHistoEnable && tmptitle==fInvalidName){
    h2=NULL;
    return h2;
  }

  h2 = new TH2F(params.name_title.c_str(),
                tmptitle.c_str(),
                params.x_nbins,
                Double_t(params.x_min),Double_t(params.x_max),
                params.y_nbins,
                Double_t(params.y_min),Double_t(params.y_max));
  h2->SetXTitle(params.xtitle.c_str());
  h2->SetYTitle(params.ytitle.c_str());
  //  if(params.min!=fInvalidNumber) h2->SetMinimum(params.min);
  //  if(params.max!=fInvalidNumber) h2->SetMinimum(params.max);
  return h2;
};
