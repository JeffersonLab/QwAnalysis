#ifndef PARSE_H
#define PARSE_H
#include <Riostream.h>

class QwParse
{
    private:
        // mapfile for parsing
        TString mapfile;
        // detector name and type (corresponding to MySQL)
        vector<TString> detectors;
        vector<TString> type;
    public:
        QwParse(TString filename) {mapfile = filename;};
        void parse(void);
        Int_t num_detectors(void);
        TString detector(Int_t num);
        TString id_type(Int_t num);
};

vector<TString> parse(TString mapfile);

#endif
