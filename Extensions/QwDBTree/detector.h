#ifndef DETECTOR_H
#define DETECTOR_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>

class QwData
{
    private:
        // vector used to fill branch
        Double_t value;
        Double_t error;
        Double_t rms;
        Long_t n;

    public:
        // method to fill vector
        QwData() {};
        QwData(Double_t temp_value, Double_t temp_error, Long_t temp_n, Double_t temp_rms);
        void fill_empty(void);
};

class QwDetector
{
    protected:
        TString reg_type;
        // name and regression type of detector in database
        TString detector_name;
        // pointer to database
        TSQLServer *db;

        // runlet data
        // runlets for this detector
        map<Int_t, Int_t> runlet_id;
        // total runlets
        vector<Int_t> good_runlets;

        // data
        vector<QwData> data;

        // virtual method to query db
        virtual TString query(void) =0;

    public:
        // fuction to set the detector name
        QwDetector(TString name, TString type, vector<Int_t> runlets, TSQLServer* db_pointer)
        {detector_name = name; reg_type = type; good_runlets = runlets; db = db_pointer;};
        void branch(TTree* tree, vector<QwData> &values, Int_t i);
        void fill();
        void get_data_for_runlet(Int_t runlet, QwData &value);
};

class QwMainDet : public QwDetector
{
    public:
        // fuction to set the detector name
        QwMainDet(TString name, TString type, vector<Int_t> runlets, TSQLServer* db_pointer) : QwDetector(name, type, runlets, db_pointer) {};
        TString query(void);
};

class QwLumiDet : public QwDetector
{
    public:
        // fuction to set the detector name
        QwLumiDet(TString name, TString type, vector<Int_t> runlets, TSQLServer* db_pointer) : QwDetector(name, type, runlets, db_pointer) {};
        TString query(void);
};

class QwBeamDet : public QwDetector
{
    private:
        // measurement id for this detector
        TString measurement_id;

    public:
        // fuction to set the detector name
        QwBeamDet(TString name, TString id, TString type, vector<Int_t> runlets, TSQLServer* db_pointer) : QwDetector(name, type, runlets, db_pointer)
    {measurement_id = id;};
        TString query(void);
};

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
