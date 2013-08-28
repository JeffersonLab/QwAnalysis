#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwDetector.h"
#include "QwDetectorSlope.h"

QwDetectorSlope::QwDetectorSlope(TString name, TString id, TString type, TString wrt, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t avg):
    QwDetector(name, id, type, runlets, db_pointer, avg) {
        wrt_detector = wrt;
    }
