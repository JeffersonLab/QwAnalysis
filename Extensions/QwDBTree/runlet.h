#ifndef RUNLET_H
#define RUNLET_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>

/* Forward declare of QwParse. */
class QwParse;

/* Structure used to fill the tree. */
struct QwValues {
    vector<Int_t> runlet_properties;
    Double_t qtor_current;
    string good_for_id;
};

/* QwRunlet class which is used to store and query runlets. */
class QwRunlet {
    public:
    QwRunlet(TSQLServer*);
    void fill(QwParse&, QwParse&, Bool_t runavg);
    void fill_run(void);
    void branch(TTree*, QwValues&);
    vector<Int_t> get_runlets(void);
    vector<Int_t> get_runs(void);
    void get_data_for_runlet(Int_t, QwValues&);

    private:
    /* Pointer to database. */
    TSQLServer* db;

    /* Runlet level data. */
    vector<Int_t> runlet_id;
    vector<Int_t> run;
    vector<Int_t> slug;
    vector<Int_t> ihwp_setting;
    vector<Int_t> phwp_setting;
    vector<Int_t> wien_reversal;
    vector<Double_t> qtor_current;
    vector<string> good_for;

    /* Query generating methods. */
    TString runlet_query(vector<TString>, Bool_t runavg);
    TString run_query(void);
    TString runlet_temp_table_create(TString, vector<TString>);
    TString runlet_temp_table_unreg_create(TString, vector<TString>);
};

#endif
