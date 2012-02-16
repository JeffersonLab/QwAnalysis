/*/////////////////////////////////////////////////////////////////////////////
//  File:       HistSummary.C
//  Author(s):  Paul King <pking@jlab.org>
//  ChangeLog:  * Mon Oct 03 2011 Paul King <pking@jlab.org>
//              - Initial working version
//  Summary:    This just scours the specified rootfile and produces
//              a text summary of all directories, histograms, and
//              text objects in the rootfile.
//              This script does not enter TTrees, but will identify them
//              by name in the output.
//  Usage:      To use this script from a command line do:
//                root -b -q Extensions/Macros/HistSummary.C\(\"ROOT_File_Name\"\) \
//                     > file_output
//              To use from within ROOT (or QwROOT) do:
//                .x Extensions/Macros/HistSummary.C("ROOT_File_Name")
//              In either case, the $QW_ROOTFILES directory is searched for
//              the named ROOT file.
//
//              Then it is useful to just diff two different outputs and check
//              that no unintentional changes were made.
/////////////////////////////////////////////////////////////////////////////*/

#include <cstdlib>

#include <iostream>
#include <iomanip>

#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "TFile.h"


void PrintHeader(const char* first, const char* second);
void HistSummary(TString filename);
void Summarize_TH1(TObject* object);
void Summarize_TH2(TObject* object);
void Summarize_List(TObject* object);
void Summarize_Directory(TObject* object);
void HandleObject(TObject* object);


void PrintHeader(const char* first, const char* second){
  std::cout << std::setw(10) << first << " " 
	    << std::setw(28) << second;
}

template <class T> void Summarize(T* object){
  HandleObject(object);
}

template < > void Summarize(TFile* object){
  Summarize_Directory(object);
}

void HistSummary(TString filename){
  TString fileName = Form("%s/",getenv("QW_ROOTFILES"));
  fileName += filename;
  TFile object(fileName);
  Summarize(&object);
}

void Summarize_TH1(TObject* object){
  //  We ought to only enter this after we have
  //  identified the object as TH1 derived
  //  so just use a simple cast.
  TH1 *obj = (TH1*)(object);
  if (obj != NULL){
    PrintHeader(object->ClassName(),object->GetName());
    std::cout << " "
	      << obj->GetEntries() << " " 
	      << obj->GetMean() << " " 
	      << obj->GetRMS()  << " " 
	      << obj->GetMeanError() 
	      << std::endl;
  }
}

void Summarize_TH2(TObject* object){
  //  We ought to only enter this after we have
  //  identified the object as TH2 derived
  //  so just use a simple cast.
  TH2 *obj = (TH2*)(object);
  if (obj != NULL){
    PrintHeader(object->ClassName(),object->GetName());
    std::cout << " "
	      << obj->GetEntries()    << " " 
	      << obj->GetMean(1)      << " " 
	      << obj->GetRMS(1)       << " " 
	      << obj->GetMeanError(1) << " " 
	      << obj->GetMean(2)      << " " 
	      << obj->GetRMS(2)       << " " 
	      << obj->GetMeanError(2) 
	      << std::endl;
  }
}

void Summarize_Directory(TObject* object){
  TDirectory *dir = (TDirectory*)(object);
  if (dir != NULL){
    PrintHeader(object->ClassName(),object->GetName());
    std::cout << " Entering directory" << std::endl;
    TIter nextkey(dir->GetListOfKeys());
    TKey *key;
    TObject *obj;
    while ((key = (TKey*)nextkey())) {
      obj = key->ReadObj();
      HandleObject(obj);
    }
  }
}



void Summarize_List(TObject* object){
  const TString objstring_name("TObjString");
  TList *obj = (TList*)(object);
  if (obj != NULL){
    PrintHeader(object->ClassName(),object->GetName());
    std::cout << " Entering list" << std::endl;
    TString listname = obj->GetName();
    TIter next = obj->MakeIterator();
    TObject *new_obj;
    while ((new_obj = next())){
      if (new_obj->ClassName()==objstring_name){
	PrintHeader(listname, new_obj->GetName());
	std::cout << std::endl;
      } else
	HandleObject(new_obj);
    }
  }
}



//  Variable only used by HandleObject; declared global to make it static.
TString gClassname;

//  This function identifies the type of the object and
//  dispatches it to the proper Summarize function.
void HandleObject(TObject* object){
  //  We use the TObject's ClassName, because there seems to be problems
  //  where the dynamic_cast incorrectly returns a valid pointer when
  //  we use it within CINT to determine the object type.
  gClassname = object->ClassName();
  if (gClassname=="TH1F" 
      || gClassname=="TH1D" 
      || gClassname=="TProfile")
    Summarize_TH1(object);
  else if (gClassname=="TH2F" 
	   || gClassname=="TH2D" 
	   || gClassname=="TProfile2D") 
    Summarize_TH2(object);
  else if (gClassname=="TList")
    Summarize_List(object);
  else if (gClassname=="TDirectory" 
	   || gClassname=="TDirectoryFile")
    Summarize_Directory(object);
  else {
    PrintHeader(gClassname, object->GetName());
    std::cout << std::endl;
  }
}
