#ifndef RUNLET_H
#define RUNLET_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>

// forward declare
class QwParse;

class QwRunlet
{
    // pointer to database
    TSQLServer* db;

    // runlet data
    vector<Int_t> runlet_id;
    vector<Int_t> run;
    vector<Int_t> slug;
    vector<Int_t> ihwp_setting;
    vector<Int_t> phwp_setting;
    vector<Int_t> wien_reversal;

    // methods
    TString runlet_query(void);
    TString runlet_temp_table_create(TString reg_type);
    TString runlet_temp_table_unreg_create(TString reg_type);

    public:
    // fuction to set the detector name
    QwRunlet(TSQLServer* db_pointer) {db = db_pointer;};
    void fill(QwParse &reg_type);
    void branch(TTree* tree, vector<Int_t> &values);
    vector<Int_t> get_runlets(void);
    void get_data_for_runlet(Int_t j, vector<Int_t> &values);
};

#endif
