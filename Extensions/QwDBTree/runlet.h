#ifndef RUNLET_H
#define RUNLET_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>

/* Forward declare of QwParse. */
class QwParse;

/* Structure used to fill the tree. */
struct QwValues {
    std::vector<Int_t> runlet_properties;
    Double_t qtor_current;
    Double_t run_number_decimal;
    Int_t runlet_quality_id;
    Int_t raster_size;
};

/* QwRunlet class which is used to store and query runlets. */
class QwRunlet {
    public:
    QwRunlet(TSQLServer*);
    void fill(QwParse&, QwParse&, TString, Bool_t, Bool_t, Bool_t, Bool_t, Bool_t, Int_t);
    void fill_run(void);
    void branch(TTree*, QwValues&);
    std::vector<Int_t> get_runlets(void);
    std::vector<Int_t> get_runs(void);
    std::vector<Int_t> get_slugs(void);
    std::vector<Int_t> get_wiens(void);
    void get_data_for_runlet(Int_t, QwValues&);

    private:
    /* Pointer to database. */
    TSQLServer* db;

    /* Runlet level data. */
    std::vector<Int_t> runlet_id;
    std::vector<Int_t> run;
    std::vector<Double_t> run_number_decimal;
    std::vector<Int_t> slug;
    /* Wien slug is what the wien number is called in the database. */
    std::vector<Int_t> wien_slug;
    std::vector<Int_t> ihwp_setting;
    std::vector<Int_t> phwp_setting;
    std::vector<Int_t> wien_reversal;
    std::vector<Int_t> precession_reversal;
    std::vector<Int_t> sign_correction;
    std::vector<Double_t> qtor_current;
    std::vector<Int_t> runlet_quality_id;
    std::vector<Int_t> raster_size;

    /* Query generating methods. */
    TString runlet_query(std::vector<TString>, Bool_t, Bool_t, Bool_t);
    TString run_query(void);
    TString slug_query(void);
    TString wien_query(void);
    TString runlet_temp_table_create(TString, std::vector<TString>, TString, Bool_t, Int_t, TString);
    TString runlet_temp_table_unreg_create(TString, std::vector<TString>, TString, Bool_t, Int_t, TString);
};

#endif
