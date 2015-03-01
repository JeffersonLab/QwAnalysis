#ifndef QWLUMIDET_H
#define QWLUMIDET_H
#include <Riostream.h>
#include <TSQLServer.h>

class QwLumiDet : public QwDetector {
    public:
        QwLumiDet(TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
        TString query(void);
};

#endif
