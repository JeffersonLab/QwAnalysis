/**
 *  \file   QwRunCondition.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Thursday, September  9 21:42:26 EDT 2010
 */

#include "QwRunCondition.h"
#include "QwSVNVersion.h"

// ClassImp(QwRunCondition);

const Int_t QwRunCondition::fCharLength = 127;

QwRunCondition::QwRunCondition(Int_t argc, Char_t* argv[])
{

  fROCFlagFileName = "qwvmets.flags";

  fRunConditionList = new TList;
  fRunConditionList -> SetOwner(true);
  this->Set(argc, argv);

};


QwRunCondition::~QwRunCondition()
{
  if(fRunConditionList) delete fRunConditionList;
};


void
QwRunCondition::Set(Int_t argc, Char_t* argv[])
{

  // get ROOT release version, date, svn revision and branch
  // RVersion.h (ROOT)
  TString root_version = "V";
  root_version += ROOT_RELEASE;
  root_version += ", Date : ";
  root_version += ROOT_RELEASE_DATE;
  root_version += ", SVN : ";
  root_version += ROOT_SVN_REVISION;
  root_version += " ";
  root_version += ROOT_SVN_BRANCH;

  // get hostname and user name
  char host_string[fCharLength];
  char user_string[fCharLength];

  gethostname(host_string, fCharLength);
  getlogin_r (user_string, fCharLength);

  TString host_name = host_string;
  TString user_name = user_string;

  // get program name and its arguments (options)
  TString program_name = argv[0];
  TString argv_list;
  for (Int_t i=1; i<argc; i++) argv_list += argv[i];

  // get current time when a ROOT file is created
  TTimeStamp time_stamp;
  TString current_time = time_stamp.AsString("l"); // local time

  // get svn revision, url, and last changed revision
  // QwSVNVersion.h (GNUmakefile)
  TString svn_revision;
  TString svn_url;
  TString svn_lc_revision;
  svn_revision    = QWANA_SVN_REVISION;
  svn_url         = QWANA_SVN_URL;
  svn_lc_revision = QWANA_SVN_LASTCHANGEDREVISION;

  // get current ROC flags 
  TString roc_flags;
  // if one of the cdaq cluster AND the user must be a "cdaq", 
  if( (host_name.Contains("cdaql")) and (not user_name.CompareTo("cdaq", TString::kExact)) )  {
    roc_flags = this->GetROCFlags();
  }
  else {
    roc_flags = "Invalid, because the system used to create ROOT file is not one of the cdaq cluster with the cdaq account.";
  }
    

  // insert some comments at the beginning of strings...
  root_version.Insert   (0, "ROOT Version : ");
  current_time.Insert   (0, "ROOT file creating time : ");
  host_name.Insert      (0, "ROOT file created on Hostname : ");
  user_name.Insert      (0, "ROOT file created by the user : ");

  program_name.Insert   (0, "QwAnalyzer Name : ");
  argv_list.Insert      (0, "QwAnalyzer Options : ");
  svn_revision.Insert   (0, "QwAnalyzer SVN Revision : ");
  svn_url.Insert        (0, "QwAnalyzer SVN URL : ");
  svn_lc_revision.Insert(0, "QwAnalyzer SVN Last Changed Revision : ");

  roc_flags.Insert      (0, "DAQ ROC flags when QwAnalyzer runs : ");

  // add them into list to be returned to main program.

  this -> Add(root_version);
  this -> Add(program_name);
  this -> Add(host_name);
  this -> Add(user_name);
  this -> Add(argv_list);
  this -> Add(current_time);
  this -> Add(svn_revision);
  this -> Add(svn_url);
  this -> Add(svn_lc_revision);
  this -> Add(roc_flags);

  return;
};


void
QwRunCondition::Add(TString in)
{
  fRunConditionList -> Add(new TObjString(in));
  return;
};


TList *
QwRunCondition::Get()
{
  return fRunConditionList;
};


TString
QwRunCondition::GetROCFlags()
{
  
  Bool_t local_debug = false;
  TString flags;

  ifstream flag_file;
  flag_file.clear();
  
  fROCFlagFileName.Insert(0, "/home/cdaq/qweak/Settings/");

  flag_file.open(fROCFlagFileName);

  if(not flag_file.is_open()) {
    std::cout << "There is no flag file, which you try to access "
	      << fROCFlagFileName  
	      << std::endl;
    flags = fROCFlagFileName;
    flags += " is not found";

  }
  else {
    while (not flag_file.eof() ) 
      {
	TString line;
	line.ReadLine(flag_file);   
	if(not line.IsNull()) {
	  if(local_debug) { 
	    std::cout << line << std::endl;
	  }
	  if(not line.Contains(";")) {
	    flags = line;
	  }
	} // if(not line.IsNull()) {
      } //   while (not flag_file.eof() ) {
  }
  flag_file.close();

  return flags;

};
