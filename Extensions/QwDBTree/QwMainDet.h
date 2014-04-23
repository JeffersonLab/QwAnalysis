#ifndef QWMAINDET_H
#define QWMAINDET_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwDetector.h"

class QwMainDet : public QwDetector {
    public:
        QwMainDet(TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
        TString query(void);
};

#endif
