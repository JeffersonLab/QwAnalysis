#ifndef PARSE_H
#define PARSE_H
#include <Riostream.h>

/* Class for reading in mapfiles and storing the data. */
class QwParse {
    public:
        QwParse(void);
        QwParse(TString);
        void parse(void);
        Int_t num_detectors(void);
        TString detector(Int_t);
        TString id_type(Int_t);
        vector<TString> ret_detector();
    private:
        /* Path to mapfile. */
        TString mapfile;
        /* Detector name and type (corresponding to MySQL) */
        vector<TString> detectors;
        vector<TString> type;
};

#endif
