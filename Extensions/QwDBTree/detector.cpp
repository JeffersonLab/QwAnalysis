#include <TTree.h>
#include <Riostream.h>
#include <TSQLStatement.h>
#include <TSQLServer.h>
#include <TMath.h>
#include "detector.h"
#include "parse.h"
#include "string.h"

/*
 * QwData constructor is overloaded to allow creation without paramaters and
 * creation while filling.
 */
QwData::QwData(void) {}
/* Constructor for QwData. Fills all data for the object. */
QwData::QwData(Double_t temp_value, Double_t temp_error, Long_t temp_n, Double_t temp_rms) {
    value = temp_value;
    error = temp_error;
    n = temp_n;
    rms = temp_rms;
}

/* Constructor for QwDetector. */
QwDetector::QwDetector(TString name, TString id, TString type, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t avg) {
    detector_name = name;
    measurement_id = id;
    reg_type = type;
    good_runlets = runlets;
    db = db_pointer;
    runavg = avg;
}

/* Constructor for QwMainDet. */
QwMainDet::QwMainDet(TString name, TString id, TString type, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t avg):
    QwDetector(name, id, type, runlets, db_pointer, avg) {}

/* Constructor for QwLumiDet. */
QwLumiDet::QwLumiDet(TString name, TString id, TString type, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t avg):
    QwDetector(name, id, type, runlets, db_pointer, avg) {}

/* Constructor for QwBeamDet. */
QwBeamDet::QwBeamDet(TString name, TString id, TString type, vector<Int_t> runlets, TSQLServer* db_pointer, Bool_t avg):
    QwDetector(name, id, type, runlets, db_pointer, avg) {};

/*
 * Method to fill empty runlets. Mostly exists for compatibility with pass4b
 * and filling empty detectors.
 *
 * FIXME: empty detectors should not be filled.
 */ 
void QwData::fill_empty(void) {
    /*
     * -1e6 is used instead of zero so that it is distinguishable from small
     * asymmetries
     */
    value = -1e6;
    error = -1e6;
    rms = -1e6;
    n = 0;
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
    /* FIXME: gotta be a better way to name this. */
    else if (measurement_id == "yq")
        type = "charge_";
    else if (measurement_id == "y")
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
                 * FIXME: Where is this used?
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

/*
 * Method to generate a query for main detectors. It uses the temporary tables
 * generated by QwRunlet::fill(), depending on which regession scheme is being
 * used. There are also similar methods for the lumins and the beam detectors,
 * but that join their specific table from the database.
 */
TString QwMainDet::query(void) {
    /* table holds the temporary table to query. */
    TString table;
    /* Figure out which table to use, based on regression type. */
    if(reg_type == "offoff") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1") table = "temp_table_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_" + reg_type;

    /* query holds the actualy query as a TString */
    TString query;
    query = "SELECT\n";
    query += "DISTINCT run_number\n";
    /*
     * If run averaging is enabled, take the weighted average.
     * 
     * FIXME: Change the weighting scheme to be weighted by qwk_mdallbars for
     * all variables. Currently it is doing a error weighted average.
     *
     * value weight: n * mdallbars_width^(-2)
     * error weight: mdallbars_width/sqrt(n)
     */
    if(runavg) {
        if(measurement_id == "a" || measurement_id == "d") {
            query += ", SUM(md_data.value/POWER(md_data.error,2))/SUM(1/POWER(md_data.error,2))*1e6\n";
            query += ", SQRT(1/SUM(1/POWER(md_data.error,2)))*1e6\n";
        }
        else {
            query += ", SUM(md_data.value/POWER(md_data.error,2))/SUM(1/POWER(md_data.error,2))\n";
            query += ", SQRT(1/SUM(1/POWER(md_data.error,2)))\n";
        }
        query += ", SUM(md_data.n)\n";
    }
    /* Otherwise, grab runlets with asymmetries and diffs in ppm. */
    else {
        if(measurement_id == "a" || measurement_id == "d") {
            /* Convert values of asymmetries to ppm */
            query += ", md_data.value*1e6\n";
            query += ", md_data.error*1e6\n";
        }
        else {
            query += ", md_data.value\n";
            query += ", md_data.error\n";
        }
        query += ", md_data.n\n";
    }
    query += "FROM " + table + "\n";

    /* Join the temorary table to the main detector table */
    query += "JOIN md_data ON " + table+ ".analysis_id = md_data.analysis_id\n";
    /* Join the detector name to the detector number. */
    query += "JOIN main_detector ON md_data.main_detector_id = main_detector.main_detector_id\n";

    /*
     * main detector level cuts. Runlet level cuts are made when generating
     * temporary tables
     */
    query += "WHERE md_data.subblock = 0\n";
    query += "AND md_data.measurement_type_id = \"" + measurement_id + "\"\n";
    query += "AND main_detector.quantity = \"" + detector_name + "\"\n";

    /* Organize the data. */
    if(runavg) query += "GROUP BY run_number;\n";
    else query += "ORDER BY runlet_id;\n";

    return query;
}
/* Method to generate a query for lumis. */
TString QwLumiDet::query(void) {
    /* table holds the temporary table to query. */
    TString table;
    /* Figure out which table to use, based on regression type. */
    if(reg_type == "offoff") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1") table = "temp_table_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_" + reg_type;

    /* query holds the actualy query as a TString */
    TString query;
    query = "SELECT\n";
    query += "DISTINCT run_number\n";
    /*
     * If run averaging is enabled, take the weighted average.
     * 
     * FIXME: Change the weighting scheme to be weighted by qwk_mdallbars for
     * all variables. Currently it is doing a error weighted average.
     *
     * value weight: n * mdallbars_width^(-2)
     * error weight: mdallbars_width/sqrt(n)
     */
    if(runavg) {
        if(measurement_id == "a" || measurement_id == "d") {
            query += ", SUM(lumi_data.value/POWER(lumi_data.error,2))/SUM(1/POWER(lumi_data.error,2))*1e6\n";
            query += ", SQRT(1/SUM(1/POWER(lumi_data.error,2)))*1e6\n";
        }
        else {
            query += ", SUM(lumi_data.value/POWER(lumi_data.error,2))/SUM(1/POWER(lumi_data.error,2))\n";
            query += ", SQRT(1/SUM(1/POWER(lumi_data.error,2)))\n";
        }
        query += ", SUM(lumi_data.n)\n";
    }
    /* Otherwise, grab runlets with asymmetries and diffs in ppm. */
    else {
        if(measurement_id == "a" || measurement_id == "d") {
            /* Convert values of asymmetries to ppm */
            query += ", lumi_data.value*1e6\n";
            query += ", lumi_data.error*1e6\n";
        }
        else {
            query += ", lumi_data.value\n";
            query += ", lumi_data.error\n";
        }
        query += ", lumi_data.n\n";
    }
    query += "FROM " + table + "\n";

    /* Join the temorary table to the main detector table */
    query += "JOIN lumi_data ON " + table + ".analysis_id = lumi_data.analysis_id\n";
    /* Join the detector name to the detector number. */
    query += "JOIN lumi_detector ON lumi_data.lumi_detector_id = lumi_detector.lumi_detector_id\n";

    /*
     * lumi level cuts. Runlet level cuts are made when generating temporary
     * tables
     */
    query += "WHERE lumi_data.subblock = 0\n";
    query += "AND lumi_data.measurement_type_id = \"" + measurement_id + "\"\n";
    query += "AND lumi_detector.quantity = \"" + detector_name +"\"\n";

    /* Organize the data. */
    if(runavg) query += "GROUP BY run_number;\n";
    else query += "ORDER BY runlet_id;\n";

    return query;
}

/* Method to generate a query for lumis. */
TString QwBeamDet::query(void) {
    /* table holds the temporary table to query. */
    TString table;
    /* Figure out which table to use, based on regression type. */
    if(reg_type == "offoff") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1")  table = "temp_table_unreg_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_unreg_" + reg_type;

    /* query holds the actualy query as a TString */
    TString query;
    query = "SELECT\n";
    query += "DISTINCT " + table + ".run_number\n";
    /*
     * If run averaging is enabled, take the weighted average.
     * 
     * FIXME: Change the weighting scheme to be weighted by qwk_mdallbars for
     * all variables. Currently it is doing a error weighted average.
     *
     * value weight: n * mdallbars_width^(-2)
     * error weight: mdallbars_width/sqrt(n)
     */
    if(runavg) {
        if(measurement_id == "a" || measurement_id == "d") {
            query += ", SUM(beam.value/POWER(beam.error,2))/SUM(1/POWER(beam.error,2))*1e6\n";
            query += ", SQRT(1/SUM(1/POWER(beam.error,2)))*1e6\n";
        }
        else {
            query += ", SUM(beam.value/POWER(beam.error,2))/SUM(1/POWER(beam.error,2))\n";
            query += ", SQRT(1/SUM(1/POWER(beam.error,2)))\n";
        }
        query += ", SUM(beam.n)\n";
    }
    /* Otherwise, grab runlets with asymmetries and diffs in ppm. */
    else {
        if(measurement_id == "a" || measurement_id == "d") {
            query += ", beam.value*1e6\n";
            query += ", beam.error*1e6\n";
        }
        else {
            query += ", beam.value\n";
            query += ", beam.error\n";
        }
        query += ", beam.n\n";
    }
    query += "FROM " + table + "\n";

    /* Join the temorary table to the main detector table */
    query += "JOIN beam ON " + table + ".analysis_id = beam.analysis_id\n";
    /* Join the detector name to the detector number. */
    query += "JOIN monitor ON beam.monitor_id = monitor.monitor_id\n";

    /*
     * beam detector level cuts. Runlet level cuts are made when generating
     * temporary tables
     */
    query += "WHERE beam.subblock = 0\n";
    query += "AND beam.measurement_type_id = \"" + measurement_id + "\"\n";
    query += "AND monitor.quantity = \""+ detector_name + "\"\n";

    /* Organize the data. */
    if(runavg) query += "GROUP BY run_number;\n";
    else query += "ORDER BY runlet_id;\n";

    return query;
}
