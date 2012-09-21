#ifndef TREE_FILL_H
#define TREE_FILL_H
#include <TROOT.h>
#include "detector.h"
#include "runlet.h"

int tree_fill(TString reg_type, TSQLServer *db, QwRunlet &runlets, TString mapdir, TString outdir);
#endif
