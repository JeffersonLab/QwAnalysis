#ifndef QWLUMIDETSLOPE_H
#define QWLUMIDETSLOPE_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwDetectorSlope.h"

class QwLumiDetSlope : public QwDetectorSlope {
    public:
        QwLumiDetSlope(TString, TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
        TString query(void);
};

#endif
