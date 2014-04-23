#ifndef QWBEAMDET_H
#define QWBEAMDET_H
#include <Riostream.h>
#include <TSQLServer.h>

class QwBeamDet : public QwDetector {
    public:
        QwBeamDet(TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
        TString query(void);
};

#endif
