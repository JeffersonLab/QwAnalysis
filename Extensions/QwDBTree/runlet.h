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
    Double_t run_number_decimal;
    string good_for_id;
};

/* QwRunlet class which is used to store and query runlets. */
class QwRunlet {
    public:
    QwRunlet(TSQLServer*);
    void fill(QwParse&, QwParse&, TString, Bool_t, Bool_t);
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
    vector<Double_t> run_number_decimal;
    vector<Int_t> slug;
    /* Wien slug is what the wien number is called in the database. */
    vector<Int_t> wien_slug;
    vector<Int_t> ihwp_setting;
    vector<Int_t> phwp_setting;
    vector<Int_t> wien_reversal;
    vector<Int_t> precession_reversal;
    vector<Int_t> sign_correction;
    vector<Double_t> qtor_current;
    vector<string> good_for;

    /* Query generating methods. */
    TString runlet_query(vector<TString>, Bool_t runavg);
    TString run_query(void);
    TString runlet_temp_table_create(TString, vector<TString>, TString, Bool_t);
    TString runlet_temp_table_unreg_create(TString, vector<TString>, TString, Bool_t);
};

#endif
