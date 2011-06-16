// GetRunNumber.C
//
// Helper macro for OnlineGUI to determine runnumber
//
#include "../src/TaDataBase.hh"

UInt_t GetRunNumber()
{
  // This is specific to the Hall-A C++/ROOT Analyzer
  //  Must use "analyzer" to obtain this info.

  UInt_t runnumber=0;
  TaRootRep *rr = (TaRootRep*)gROOT->FindObject("TaRootRep;1");
  if(rr!=NULL) runnumber = rr->GetRunNum();

  return runnumber;

}
