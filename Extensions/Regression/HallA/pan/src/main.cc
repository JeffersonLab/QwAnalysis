//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           main.cc
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: main.cc,v 1.13 2009/08/06 18:36:04 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//  Main program
//  Usage:  pan [-r runnum] [-f filename] [-o]
//
////////////////////////////////////////////////////////////////////////

#include "TaAnalysisManager.hh"
#include "TROOT.h"
#include "TRint.h"
#include <cstdlib>
#include <signal.h>

extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
TaAnalysisManager *am;
int global_kill31_flag = 0;
extern "C" void signalhandler(int s);

static TROOT root( "Pan", "Parity Analyzer Interactive Interface", initfuncs );

void usage();

int main(int argc, char **argv) 
{

  clog << "Pan $Name:  $:$Id: main.cc,v 1.13 2009/08/06 18:36:04 rsholmes Exp $\n" << endl;

  int runnum = 0;
  int choice = 0;
  Bool_t interactive(true); // hand off to TRint or not?
  char *cfilename;
  Bool_t twopass(false);
  vector<string> dbcommand;
  dbcommand.clear();

  signal(31, signalhandler);

  int i = 1;
  while (i < argc) 
    {
      if (strcasecmp(argv[i],"-r") == 0) 
	{
	  if (i < argc-1) 
	    {
	      runnum = atoi(argv[++i]);
	      choice = 1;
	      interactive = false;
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
	      interactive = false;
	    }
	  else
	    {
	      usage();
	      return 1;
	    }
	}
      else if (strcasecmp(argv[i],"-o") == 0) 
	{
	  choice = 3;
	  interactive = false;
	}
      else if (strcasecmp(argv[i],"-t") == 0) 
	{
	  twopass = true;
	  interactive = false;
	}
  
      else if (strcasecmp(argv[i],"-D") == 0) {
// Pre-parse the database command line.
	interactive = false;
        if (i > argc-1) {
           usage();
           return 1;
	}        
	int k = i;
        int n = argc-i-1;
        for (int j = 0; j < n; j++) {
          ++k;
          if (strstr(argv[k],"-") != NULL) {
// Put all the other "-" cases here.  Add them here if you invent more.
            if (strcasecmp(argv[k],"-r") == 0 ||
                strcasecmp(argv[k],"-f") == 0 || 
                strcasecmp(argv[k],"-o") == 0 ||
                strcasecmp(argv[k],"-t") == 0) goto cont1;
          }
          dbcommand.push_back(argv[k]);
          ++i;
	}
      }
      else if (strcasecmp(argv[i],"-h") == 0 || strcasecmp(argv[i],"-?") == 0) 
	{
	  usage();
	  return 1;
	}
cont1:
      ++i;
    }

  if (twopass && (choice == 0 || choice == 3))
    {
      usage();
      return 1;
    }

  // Done processing command line.  Hand off to analysis manager.  

  TROOT pan ( "pan", "Parity analyzer" );

  if (interactive)
    {
      // Interactive use with ROOT prompt
      TRint *theApp = new TRint( "The Hall A parity analyzer", &argc, argv, NULL, 0, kFALSE );
      theApp->Run();
      delete theApp;
      return 0;
    }
  else 
    {

      am = new TaAnalysisManager();
      am->SetDbCommand(dbcommand);

      // Command line use with run number, file name or online specified
      if (choice == 1) 
	{
	  if (am->Init(runnum) != 0)
	    return 1;
	}
      else if (choice == 2) 
	{
	  string filename = cfilename;
	  if (am->Init(filename) != 0)
	    return 1;
	}
      else if (choice == 3) 
	{
#ifdef ONLINE
	  if (am->Init() != 0)
	    return 1;
#else
	  usage();
	  return 1;
#endif
	}
      else
	{
	  usage();
	  return 1;
	}
      if (twopass)
	{
	  if (am->Process() != 0 || 
	      am->EndPass1() != 0 || 
	      am->InitPass2() != 0)
	    return 1;
	}
      if (am->InitLastPass() != 0 ||
	  am->Process() != 0 ||
	  am->End() != 0)
	return 1;
      return 0;
    }
}


void usage() {
// Prints usage instructions

  cerr << "Usage:  pan [data source specifier] [-t] [-D data]" << endl;
  cerr << "" << endl;
  cerr << "  where data source specifier is" << endl;
  cerr << "    -r runnum   to analyze run number <runnum>" << endl;
  cerr << "    -f filepath to analyze CODA file at <filepath>" << endl;
#ifdef ONLINE
  cerr << "    -o          to analyze online data" << endl;
#else
  cerr << "                [Recompile with ONLINE flag in Makefile to allow" << endl;
  cerr << "                analysis of online data]" << endl;
#endif
  cerr << "  Other options:" << endl;
  cerr << "    -t             Do 2-pass analysis (with -r or -f only)." << endl;
  cerr << "    -D table data  Database command line override" << endl;
  cerr << "" << endl;
  cerr << "  Interactive prompt will be given if none of the above options is specified." << endl;
  cerr << "  In that case you can use the usual ROOT options:" << endl;
  cerr << "    -b : run in batch mode without graphics" << endl;
  cerr << "    -n : do not execute logon and logoff macros as specified in .rootrc" << endl;
  cerr << "    -q : exit after processing command line macro files" << endl;
  cerr << "    -l : do not show splash screen" << endl;
  cerr << "    dir : if dir is a valid directory cd to it before executing" << endl;
  cerr << "" << endl;
  cerr << "  Valid examples:" << endl;
  cerr << "     ./pan -r 1824 -t" << endl;
  cerr << "     ./pan -f /work/halla/parity/rom/parity01_1824.dat" << endl;
#ifdef ONLINE
  cerr << "     ./pan -o " << endl;
#endif
  cerr << "     ./pan" << endl;
  cerr << "     ./pan -b -q macro.C" << endl;
  cerr << "     ./pan -r 1824 -D lobeam 500 pairtype pair" << endl;
  cerr << "     ./pan -r 1824 -D control.db evint 5610  5824 8 1" << endl;
  cerr << "     ./pan -r 1824 -D mysql -D maxevents 50000" << endl;
  cerr << "     ./pan -r 1824 -D useroot parity02_1821.root" << endl;
}

void signalhandler(int sig)
{  // To deal with the signal "kill -31 pid"
   // global_kill31_flag forces am->End()
  //  which you do NOT want to do here.
cout << "Ending the online analysis"<<endl<<flush;
global_kill31_flag = 1; 
}


