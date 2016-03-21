///////////////////////////////////////////////////////////////////////////////
// FileName:   run_macro.C
// Author(s):  Juan Carlos Cornejo <cornejo@jlab.org>
// Summary:    This is a generic script to run the auto-analyzed macros. It is
//             called by the autoanalyzer automatically and needs not be
//             called outside of that script.
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <TSystem.h>
#include <TROOT.h>
#include <TString.h>

void run_macro(const char* macroName, const char* functionName,
      const char* localIncludes, Int_t runNumber, Bool_t isFirst100k, Bool_t compile)
{
   // Base directory for macros
//   TString macroDir = TString(getenv("QWANALYSIS")) + "/Extensions/Macros/Tracking";
   TString macroDir = TString(getenv("MACRODIR"));

   // First include the default directory
   gROOT->ProcessLine(".include " + macroDir + "/shared");

   // Then include the local directory
   gROOT->ProcessLine(".include " + macroDir + "/" + localIncludes);

   // Load the macro
   TString macro = macroDir + Form("/%s",macroName);
   std::cout << "Loading macro " << macro << std::endl;
   if (compile)
      gROOT->LoadMacro(macro + "+g");
   else
      gROOT->LoadMacro(macro);

   // Finally run the script
   TString first100;
   if (isFirst100k)
      first100.Append("kTRUE");
   else
      first100.Append("kFALSE");
   TString command = Form("%s(%d,%s)",functionName,runNumber, first100.Data());
   gROOT->ProcessLine(command);
}
