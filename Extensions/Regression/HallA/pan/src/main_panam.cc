//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           main_panam.cc
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: main_panam.cc,v 1.2 2003/07/31 16:12:00 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//  Main program for panam
//
////////////////////////////////////////////////////////////////////////

#include "TaPanam.hh"
#include <signal.h>

extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

extern "C" void signalhandler(int s);

TROOT root("Pan monitirong", "", initfuncs);

int main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);

  TaPanam mainWindow(gClient->GetRoot(), 400, 220);

  theApp.Run();

  return 0;
}

void signalhandler(int sig)
{  // To deal with the signal "kill -31 pid"
  cout << "Ending the online analysis"<<endl<<flush;
  //  mainWindow.EndPan();
  exit(1);
}















