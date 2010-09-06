/**
 *  \file   QwRunCondition.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Sunday, September  5 01:12:23 EDT 2010
 */

#include "QwRunCondition.h"
#include "TTimeStamp.h"

ClassImp(QwRunCondition);

const Int_t QwRunCondition::fCharLength = 127;

QwRunCondition::QwRunCondition(Int_t argc, Char_t* argv[])
{
  fROCFlagFileName = "g0vmets.flags";

  fRunConditionList = new TList;
  fRunConditionList -> SetOwner(true);
  this->SetRunCondition(argc, argv);
};


QwRunCondition::~QwRunCondition()
{
  if(fRunConditionList) delete fRunConditionList;
}


void
QwRunCondition::SetRunCondition(Int_t argc, Char_t* argv[])
{

  // get hostname and user name
  char host_string[fCharLength];
  char user_string [fCharLength];

  gethostname(host_string, fCharLength);
  getlogin_r (user_string, fCharLength);

  TString host_name = host_string;
  TString user_name = user_string;

  // get program name and its arguments (options)
  TString program_name = argv[0];
  TString argv_list;
  for (Int_t i=1; i<argc; i++) argv_list += argv[i];

  // get current time to create ROOT file 
  TTimeStamp time_stamp;
  TString current_time = time_stamp.AsString("l");

  // get svn revision (flag, version, and so on...)
  TString svn_revision;
  svn_revision = this->GetSvnRevision();


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

  program_name.Insert(0, " * Executed Program Name : ");
  host_name.Insert   (0, " * Created on Hostname : ");
  user_name.Insert   (0, " * Created by the user : ");
  argv_list.Insert   (0, " * Executed Program Options : ");
  current_time.Insert(0, " * ROOT file creating time : ");
  svn_revision.Insert(0, " * Analyzer SVN Revision : ");
  roc_flags.Insert   (0, " * Current ROC flags : ");

  // add them into list to be returned to main program.

  this -> Set(program_name);
  this -> Set(host_name);
  this -> Set(user_name);
  this -> Set(argv_list);
  this -> Set(current_time);
  this -> Set(svn_revision);
  this -> Set(roc_flags);

  return;
}


void
QwRunCondition::Set(TString in)
{
  fRunConditionList -> Add(new TObjString(in));
  return;
};


TList *
QwRunCondition::Get()
{
  return fRunConditionList;
}

TString
QwRunCondition::GetSvnRevision()
{
  TString svn_revision;
  
  // there are some code to get svn revision
  // Do we need API or just read .svn/entries file?
  // I will try to read .svn/entries file directly
  // Sunday, September  5 01:11:10 EDT 2010, jhlee

  svn_revision = "not yet implemented";
  
  return svn_revision;
};


TString
QwRunCondition::GetROCFlags()
{
  Bool_t local_debug = true;
  TString flags;

  ifstream flag_file;
  flag_file.clear();
  
  fROCFlagFileName.Insert(0, "$HOME/qweak/coda26/crl/");
  flag_file.open(fROCFlagFileName);

  if(not flag_file.is_open()) {
    std::cout << "There is no flag file, which you try to access "
	      << fROCFlagFileName  
	      << std::endl;
    flags = fROCFlagFileName;
    flags += "not found";

  }
  else {
    TString line;
    while (not flag_file.eof() ) {
      line.ReadLine(flag_file);   
      if(not line.IsNull()) {
	if(local_debug) { 
	  std::cout << line << std::endl;
	}
	if(not line.Contains(";")) {
	  flags = line;
	}
	line.Clear();
      } // if(not line.IsNull()) {
    } //   while (not flag_file.eof() ) {
    line.Clear();
  }

  flag_file.close();

  return flags;
};
