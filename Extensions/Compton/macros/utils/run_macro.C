///////////////////////////////////////////////////////////////////////////////
// FileName:   run_macro.C
// Author(s):  Juan Carlos Cornejo <cornejo@jlab.org>
// Summary:    This is a generic script to run the auto-analyzed macros. It is
//             called by the autoanalyzer automatically and needs not be
//             called outside of that script.
///////////////////////////////////////////////////////////////////////////////
#include <TSystem.h>
#include <TROOT.h>
#include <TString.h>
void run_macro(const char* macroName, const char* weight,
      const char* localIncludes, Int_t runNumber, Bool_t isFirst100K, Bool_t compile)
{
   // First include the default directory
   //gSystem->SetIncludePath("shared/");
   gROOT->ProcessLine(".include shared/");

   // Then include the local directory
   //gSystem->AddIncludePath(Form(" -I%s ",localIncludes));
   gROOT->ProcessLine(Form(".include %s", localIncludes));

   // Load the macro
   if( compile )
      gROOT->LoadMacro(Form("macros.d/%s%s.C+",weight,macroName));
   else
      gROOT->LoadMacro(Form("macros.d/%s%s.C",weight,macroName));

   // Finally run the script
   TString first100;
   if ( isFirst100K )
      first100.Append("kTRUE");
   else
      first100.Append("kFALSE");
   TString command = Form("%s(%d,%s)",macroName,runNumber, first100.Data());
   gROOT->ProcessLine(command);
}
