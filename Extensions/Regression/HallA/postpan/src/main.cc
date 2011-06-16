///////////////////////////////////////////////////////////////
// main.C
//    - Main program for ReD analysis.
//
//  Usage: redana <run number>
//
//
#define DEBUG
#include "Rtypes.h"
#include "TaRedAna.hh"
#include "TROOT.h"
#include "TRint.h"
#include <cstdlib>
#include <string.h>

extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0};
TaRedAna *redana;

static TROOT root( "Redana", 
		   "Regression-Dithering Analysis Interactive Interface",
		   initfuncs );

void usage();

int main(int argc, char **argv) 
{
  int runnum = 0;
  int choice = 0;
  char *cfilename;
  char *confFileName = 0;

  cout << endl;
  cout << "Regression and Dithering Analysis - B. Moffit and K. Paschke " 
       << endl;

  if(argc > 5) {
    cerr << "main Error: Wrong number of arguments" << endl;
    usage();
    return -1;
  }
  Int_t i = 1;

  while (i < argc)
    {
      if (strcasecmp(argv[i],"-r") == 0)
	{
	  if (i < argc-1)
	    {
	      runnum = atoi(argv[++i]);
	      choice = 1;
	    }
	  else
	    {
	      usage();
	      return 1;
	    }
	}
      else if (strcasecmp(argv[i],"-f") == 0)
	{
	  if (i < argc-1)
	    {
	      cfilename = new char[strlen(argv[++i])+1];
	      strcpy(cfilename,argv[i]);
	      choice = 2;
	    }
	  else
	    {
	      usage();
	      return 1;
	    }
	}
      else if (strcasecmp(argv[i],"-C") == 0)
	{
	  if (i < argc-1)
	    {
	      confFileName = new char[strlen(argv[++i])+1];
	      strcpy(confFileName,argv[i]);
	    }
	  else
	    {
	      usage();
	      return 1;
	    }
	}
      ++i;
    }

  TString fConfFileName = confFileName;
  redana = new TaRedAna(fConfFileName);

  if (choice == 0) {
    TRint *theApp = new TRint( "Regression-Dithering Analysis", &argc, argv,
			       NULL, 0, kFALSE);
    theApp->Run();
    delete theApp;
    return 0;
  } else if (choice == 1) {
    if (redana->Init(runnum) != 0) 
      return 1;
  } else if (choice == 2) {
    string filename = cfilename;
    if (redana->Init(filename) != 0) 
      return 1;
  }
  redana->Process();
  redana->End();

  return 0;
}

void usage() {
// Prints usage instructions

  cerr << "Usage:  redana [data source specifier]" << endl;
  cerr << "" << endl;
  cerr << "  where data source specifier is" << endl;
  cerr << "    -r runnum   to analyze run number <runnum>" << endl;
  cerr << "    -f filepath to analyze PAN file at <filepath>" << endl;
  cerr << endl;
}
