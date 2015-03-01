#ifndef QWDETECTORSLOPE_H
#define QWDETECTORSLOPE_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwDetector.h"

/* QwDetector class, from which specific detectors inherit. */
class QwDetectorSlope : public QwDetector {
    public:
        QwDetectorSlope(TString, TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
    protected:
        TString wrt_detector;
};

#endif
