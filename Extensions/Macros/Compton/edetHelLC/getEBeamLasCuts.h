#ifndef GETEBEAMLASCUTS_H
#define GETEBEAMLASCUTS_H

#include "rootbasic.h"

class TChain;

Int_t getEBeamLasCuts(std::vector<Int_t> &cutL, std::vector<Int_t> &cutE, TChain *chain, Int_t runnum);

#endif // GETEBEAMLASCUTS_H
