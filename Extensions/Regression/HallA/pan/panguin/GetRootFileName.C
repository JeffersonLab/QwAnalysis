// GetRootFileName.C
//
// Helper macro for OnlineGUI to determine ROOT Filename
//   from a given runnumber
//
#include "../src/TaFileName.hh"

TString GetRootFileName(UInt_t runnumber)
{
  TaFileName::Setup (runnumber, "standard");

  TString filename = (TaFileName ("root")).Tstring();
  return filename;

}
