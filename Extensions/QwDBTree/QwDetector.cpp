#include <TTree.h>
#include <Riostream.h>
#include <TSQLStatement.h>
#include <TSQLServer.h>
#include <TMath.h>
#include "QwDetector.h"
#include "string.h"

/* Constructor for QwDetector. */
QwDetector::QwDetector(TString name, TString id, TString type, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t ravg, Bool_t savg, Bool_t wavg) {
    detector_name = name;
    measurement_id = id;
    reg_type = type;
    good_runlets = runlets;
    db = db_pointer;
    runavg = ravg;
    slugavg = savg;
    wienavg = wavg;
}

/*
 * Method to branch 
 */
void QwDetector::branch(TTree* tree, vector<QwData> &values, Int_t i) {
    TString name;
    string type;
    string detector_str = detector_name.Data();
    UInt_t pos;

    pos = detector_str.find("qwk_");

    /* See FIXME below. This is to be removed due to confusion. Sorry Manolis! */
    if (pos !=string::npos) {
        pos += 4;
        //name = detector_str.substr(pos);
        name = detector_name;
    }
    else {
        name = detector_name;
    }

    /* Use the same naming scheme as the Qweak analysers output rootfiles. */
    if (measurement_id == "a")
        type = "asym_";
    else if (measurement_id == "d")
        type = "diff_";
    /* FIXME: Can only look at either y or yq for a variable <_<. */
    else if (measurement_id == "y" || measurement_id == "yq" || measurement_id == "yp")
        type = "yield_";
    else
        type = "asym_";

    /* This in the name that goes into the database */
    name = type + name;
    
    /*
     * Branch command. Uses the name in tree, an object, and the name of the
     * values in the object.
     *
     * FIXME: fix the detector_name vs name discrepency. Not a functional fix,
     * but makes the code more readable.
     */
    tree->Branch(name, &values[i], "value/D:err/D:rms/D:n/I");
}

/*
 * Method to fill QwDetector object from the database. First it generates a
 * query, then fills the object. 
 */
void QwDetector::fill(void) {
    cout << "querying " + detector_name << endl;
    TString sql_query = query();
    TSQLStatement* stmt = db->Statement(sql_query, 100);

    if((db!=0) && db->IsConnected()) {
        if(stmt->Process()) {
            // store in buffer
            stmt->StoreResult();
 
            /* Index for runlet number. */
            UInt_t i = 0;
            while(stmt->NextResultRow()) {
                /* Get the values from the database */
                Int_t temp_runlet_id = stmt->GetInt(0);
                Double_t temp_value = stmt->GetDouble(1);
                Double_t temp_error = stmt->GetDouble(2);
                Int_t temp_n = stmt->GetInt(3);

                /*
                 * runlet_id is a map that maps runlet_id to the number that
                 * runlet occpuies in the rootfile.
                 *
                 * This is a simple way to look up the position in the array of
                 * an arbitrary runlet.
                 */
                runlet_id[temp_runlet_id] = i;
                QwData temp_data(temp_value, temp_error, temp_n, temp_error*sqrt(temp_n));
                data.push_back(temp_data);
                i++;
            }
        }
    }
}

/* Method that gets the runlet data and puts it into a QwData object */
void QwDetector::get_data_for_runlet(Int_t runlet, QwData &value) {
    QwData temp_data;
    /* Fill value with data if there are a nonzero number of runlets*/
    if(runlet_id.count(runlet) > 0) {
        temp_data = data[runlet_id[runlet]];
        value = temp_data;
    }
    /* Otherwise, fill with 'zero'. See QwData::fill_empty() for details. */
    else {
        temp_data.fill_empty();
        value = temp_data;
    }
}
