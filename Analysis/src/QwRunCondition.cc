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



  // insert some comments at the beginning of strings...

  program_name.Insert(0, " * Executed Program Name : ");
  host_name.Insert   (0, " * Created on Hostname : ");
  user_name.Insert   (0, " * Created by the user : ");
  argv_list.Insert   (0, " * Executed Program Options : ");
  current_time.Insert(0, " * ROOT file creating time : ");
  svn_revision.Insert(0, " * Analyzer SVN Revision : ");

  // add them into list to be returned to main program.

  this -> Set(program_name);
  this -> Set(host_name);
  this -> Set(user_name);
  this -> Set(argv_list);
  this -> Set(current_time);
  this -> Set(svn_revision);

  return;
}


void
QwRunCondition::Set(TString in)
{
  fRunConditionList -> Add(new TObjString(in));
  return;
};


TList *
QwRunCondition::GetCondition()
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
