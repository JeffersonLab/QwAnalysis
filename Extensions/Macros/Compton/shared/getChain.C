#include <iostream>

#include "TChain.h"

#ifndef _GETCHAIN_C_
#define _GETCHAIN_C_

TChain* getChain(const TString& name, Int_t runnumber = 0, Bool_t isFirst100k = kFALSE)
{
  TString rootfilesdir(getenv("QW_ROOTFILES"));
  rootfilesdir += "/";

  TString searchstring = "";
  if (isFirst100k) {
    searchstring = Form("Compton_first100k_%d.root",runnumber);
  } else {
    searchstring = Form("Compton_Pass?_%d.*.root",runnumber);
  }

  TChain *chain = new TChain(name);
  std::cout << "Adding " << rootfilesdir + searchstring << "..." << std::endl;
  Bool_t chainExists = chain->Add(rootfilesdir + searchstring);

  // Check local folder in case running locally
  if (! chainExists) {
    chainExists = chain->Add(searchstring);
  }

  if (! chainExists) {
    std::cout << "Could not find data files." << std::endl;
    delete chain;
    return 0;
  }

  return chain;
}

TChain* getHelChain(Int_t runnumber = 0, Bool_t isFirst100k = kFALSE)
{
  return getChain("Hel_Tree",runnumber,isFirst100k);
}

TChain* getMpsChain(Int_t runnumber = 0, Bool_t isFirst100k = kFALSE)
{
  return getChain("Mps_Tree",runnumber,isFirst100k);
}

#endif

