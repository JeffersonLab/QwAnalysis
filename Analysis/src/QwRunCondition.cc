
#include "QwRunCondition.h"

/**
 *  \file   QwRunCondition.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   
 */

ClassImp(QwRunCondition);

QwRunCondition::QwRunCondition()
{
  // fRevision    = NULL;
  // fProgName    = NULL;
  // fRunNumber   = NULL;
  // fHostName    = NULL;
  // fUserName    = NULL;
  // fCreatedDate = NULL;
  // fRunCommand  = NULL;
  // fPath        = NULL;

  fRunConditionList = new TList;
  fRunConditionList -> SetOwner(true);

  this->SetRunCondition();

};

// QwRunCondition::QwRunCondition(Int_t argc, Char_t* argv[])
// {
//   fRevision    = NULL;
//   fProgName    = NULL;
//   fRunNumber   = NULL;
//   fHostName    = NULL;
//   fUserName    = NULL;
//   fCreatedDate = NULL;
//   fRunCommand  = NULL;

//   fRunConditionList = new TList;
//   fRunConditionList -> SetOwner(true);

//   //  this->SetRunCondition(argc, argv);

// }

QwRunCondition::~QwRunCondition()
{
  if(fRunConditionList) delete fRunConditionList;
}


void
QwRunCondition::SetRunCondition()
{
  

  char host_string[127];  
  
  gethostname(host_string, 127);
  //  std::cout << "hostname " << hostname << std::endl;
  //

  TString hostname = host_string;

  this -> Set(hostname);

  return;
}


void
QwRunCondition::Set(TString in)
{
  std::cout << in << std::endl;

  TObjString *temp = new TObjString(in);
  fRunConditionList -> Add(temp);

  return;

};


TList *
QwRunCondition::GetCondition()
{
  return fRunConditionList;
}
