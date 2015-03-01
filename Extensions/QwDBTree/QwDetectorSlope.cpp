#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwDetector.h"
#include "QwDetectorSlope.h"

QwDetectorSlope::QwDetectorSlope(TString name, TString id, TString type, TString wrt, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t ravg, Bool_t savg, Bool_t wavg):
    QwDetector(name, id, type, runlets, db_pointer, ravg, savg, wavg) {
        wrt_detector = wrt;
    }
