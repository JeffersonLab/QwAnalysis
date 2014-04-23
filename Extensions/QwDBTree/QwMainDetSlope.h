#ifndef QWMAINDETSLOPE_H
#define QWMAINDETSLOPE_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwDetectorSlope.h"

class QwMainDetSlope : public QwDetectorSlope {
    public:
        QwMainDetSlope(TString, TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
        TString query(void);
};

#endif
