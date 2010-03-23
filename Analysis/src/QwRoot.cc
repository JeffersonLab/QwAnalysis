#include "QwRoot.h"

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TROOT.h>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"

// Global pointers
QwRoot* gQwRoot = NULL;

// Pointer to self
QwRoot* QwRoot::fExists = NULL;


//--------------------------------------------------------------------------
QwRoot::QwRoot (const char* appClassName, int* argc, char** argv,
		      void* options, int numOptions, bool noLogo)
: TRint (appClassName, argc, argv, options, numOptions, noLogo)
{
  gQwRoot = this;

  // Qw-Root command prompt
  SetPrompt("Qw-Root [%d] ");

  // Pointer to self
  fExists = this;
}

//---------------------------------------------------------------------------
QwRoot::~QwRoot()
{
  // Reset point to self
  if (fExists == this)
    fExists = NULL;
}
