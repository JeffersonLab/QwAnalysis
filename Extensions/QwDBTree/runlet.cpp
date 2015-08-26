#include <TTree.h>
#include <Riostream.h>
#include <TSQLStatement.h>
#include <TSQLServer.h>
#include <TMath.h>
#include "runlet.h"
#include "QwDetector.h"
#include "parse.h"

/* QwRunlet constructor. */
QwRunlet::QwRunlet(TSQLServer* db_pointer) {
    db = db_pointer;
}

/*
 * Generate query to get the runlet level data out of temporary table. offoff
 * is used because it is backwards compatible with pass4b.
 */
TString QwRunlet::runlet_query(vector<TString> runlist, Bool_t runavg, Bool_t slugavg, Bool_t wienavg) {
    TString query;
    query = "SELECT\n";
    query += "runlet_id\n";
    query += ", run_number\n";
    query += ", slug\n";
    query += ", wien_slug\n";
    query += ", segment_number\n";
    query += ", slow_helicity_plate\n";
    query += ", passive_helicity_plate\n";
    query += ", wien_reversal\n";
    query += ", precession_reversal\n";
    query += ", qtor_current\n";
    query += ", runlet_quality_id\n";
    query += ", raster_size\n";
    query += "FROM temp_table_unreg_offoff\n"; /* Group by run_number for run averaging */
    if(runavg) {
        query += "GROUP BY run_number;";
    }
    else if(slugavg) {
        query += "GROUP BY slug;";
    }
    else if(wienavg) {
        query += "GROUP BY wien_slug;";
    }

    return query;
}

/* Generate query to get the run level data out of temporary table. */
TString QwRunlet::run_query(void) {
    TString query;
    query = "SELECT\n";
    query += "DISTINCT run_number\n";
    query += "FROM temp_table_unreg_offoff;\n";
    //query += "ORDER BY run_number;\n";

    return query;
}

/* Generate query to get the slug level data out of temporary table. */
TString QwRunlet::slug_query(void) {
    TString query;
    query = "SELECT\n";
    query += "DISTINCT slug\n";
    query += "FROM temp_table_unreg_offoff;\n";
    //query += "ORDER BY run_number;\n";

    return query;
}

/* Generate query to get the wien level data out of temporary table. */
TString QwRunlet::wien_query(void) {
    TString query;
    query = "SELECT\n";
    query += "DISTINCT wien_slug\n";
    query += "FROM temp_table_unreg_offoff;\n";
    //query += "ORDER BY run_number;\n";

    return query;
}

/*
 * Generate query to create temporary tables. It creates a temporary table for
 * a given regression type, and if needed, will only do so for a specific
 * number of runs, specified in runlist.
 *
 * Temporary tables reduce the complexity of the doing 4 JOINs many times.
 * Instead, you do 3 of the JOINs once, and just JOIN this temporary table to
 * your data.
 */
TString QwRunlet::runlet_temp_table_create(TString reg_type, vector<TString> runlist, TString target, Bool_t ignore_quality, Int_t run_quality, TString db_name) {
    /* query holds the MySQL query in a TString. */
    TString query;
    /*
     * Since MySQL does not allow + in the table name, change the temp table
     * name. It is important to not forget this elsewhere.
     */
    if(reg_type == "on_5+1") query = "CREATE TEMPORARY TABLE temp_table_on_5p1 AS(\n";
    else query = "CREATE TEMPORARY TABLE temp_table_" + reg_type +  " AS(\n";
    query += "SELECT\n";
    query += "DISTINCT analysis.analysis_id\n";
    query += ", analysis.runlet_id\n";
    query += ", run.run_number\n";
    query += ", run.slug\n";
    query += ", run.wien_slug\n";
    query += ", runlet.segment_number\n";
    query += ", slow_controls_settings.slow_helicity_plate\n";
    query += ", slow_controls_settings.passive_helicity_plate\n";
    query += ", slow_controls_settings.wien_reversal\n";
    query += ", slow_controls_settings.precession_reversal\n";
    query += ", slow_controls_settings.qtor_current\n";
    query += ", runlet.runlet_quality_id\n";
    query += ", raster.value AS raster_size\n";
    query += "FROM analysis\n";

    /*
     * Join together the analysis table, runlet table and
     * slow_controls_settings table.
     */
    query += "JOIN runlet ON analysis.runlet_id = runlet.runlet_id\n";
    query += "JOIN run ON runlet.run_id = run.run_id\n";
    query += "JOIN slow_controls_settings ON runlet.runlet_id = slow_controls_settings.runlet_id\n";
    query += "JOIN slow_controls_data ON runlet.runlet_id = slow_controls_data.runlet_id\n";
    query += "JOIN slow_controls_data raster ON runlet.runlet_id = raster.runlet_id\n";
    query += "JOIN sc_detector raster_detector ON raster_detector.sc_detector_id = raster.sc_detector_id\n";

    /* Runlet and analysis level cuts are done here. */
    query += "WHERE analysis.slope_correction = \"" + reg_type + "\"\n";
    query += "AND raster_detector.name = \"EHCFR_LIXWidth\"\n";

    /* set target */
    if(target != "") query += "AND slow_controls_settings.target_position = \"" + target + "\"\n";

    /* set run quality checks */
    if(!ignore_quality) {
        query += Form("AND runlet.runlet_quality_id = \"%d\"\n", run_quality);
        query += "AND FIND_IN_SET('1',run.good_for_id)\n";
        query += "AND FIND_IN_SET('3',run.good_for_id)\n";
    }

    /* if using runlist, cut on run number */
    if(0 != runlist.size()) {
        query += "AND run.run_number IN (";
        for(UInt_t i = 0; i < runlist.size(); i++) {
            if(i == runlist.size() - 1) {
                query += runlist[i] + ")\n";
            }
            else query += runlist[i] + ", ";
        }
    }

    query += "ORDER BY runlet.run_number, runlet.segment_number);\n";

    return query;
}

/* Generate query to create temporary table for unregressed values. */
TString QwRunlet::runlet_temp_table_unreg_create(TString reg_type, vector<TString> runlist, TString target, Bool_t ignore_quality, Int_t run_quality, TString db_name) {
    /* query holds the MySQL query in a TString. */
    TString query;
    /*
     * Since MySQL does not allow + in the table name, change the temp table
     * name. It is important to not forget this elsewhere.
     */
    if(reg_type == "on_5+1") query = "CREATE TEMPORARY TABLE temp_table_unreg_on_5p1 AS(\n";
    else query = "CREATE TEMPORARY TABLE temp_table_unreg_" + reg_type +  " AS(\n";
    query += "SELECT\n";
    query += "DISTINCT analysis.analysis_id\n";
    query += ", analysis.runlet_id\n";
    query += ", run.run_number\n";
    query += ", run.slug\n";
    query += ", run.wien_slug\n";
    query += ", runlet.segment_number\n";
    query += ", slow_controls_settings.slow_helicity_plate\n";
    query += ", slow_controls_settings.passive_helicity_plate\n";
    query += ", slow_controls_settings.wien_reversal\n";
    query += ", slow_controls_settings.precession_reversal\n";
    query += ", slow_controls_settings.qtor_current\n";
    query += ", runlet.runlet_quality_id\n";
    query += ", raster.value AS raster_size\n";
    query += "FROM analysis\n";
    /*
     * Join together the analysis table, runlet table and
     * slow_controls_settings table.
     */
    query += "JOIN runlet ON analysis.runlet_id = runlet.runlet_id\n";
    query += "JOIN run ON runlet.run_id = run.run_id\n";
    query += "JOIN slow_controls_settings ON runlet.runlet_id = slow_controls_settings.runlet_id\n";
    query += "JOIN slow_controls_data ON runlet.runlet_id = slow_controls_data.runlet_id\n";
    query += "JOIN slow_controls_data raster ON runlet.runlet_id = raster.runlet_id\n";
    query += "JOIN sc_detector raster_detector ON raster_detector.sc_detector_id = raster.sc_detector_id\n";

    /* Runlet and analysis level cuts are done here. The regression type is set
     * based on the mapfile.
     */
    query += "WHERE analysis.slope_correction = \"off\"\n";
    query += "AND raster_detector.name = \"EHCFR_LIXWidth\"\n";

    if(reg_type == "off") query += "AND analysis.slope_calculation = \"on\"\n";
    else if(reg_type == "offoff") query += "AND analysis.slope_calculation = \"off\"\n";
    else query += "AND analysis.slope_calculation = \"" + reg_type + "\"\n";

    /* set target */
    if(target != "") query += "AND slow_controls_settings.target_position = \"" + target + "\"\n";

    /* set run quality checks */
    if(!ignore_quality) {
        query += Form("AND runlet.runlet_quality_id = \"%d\"\n", run_quality);
        query += "AND FIND_IN_SET('1',run.good_for_id)\n";
        query += "AND FIND_IN_SET('3',run.good_for_id)\n";
    }

    /* if using runlist, cut on run number */
    if(0 != runlist.size()) {
        query += "AND run.run_number IN (";
        for(UInt_t i = 0; i < runlist.size(); i++) {
            if(i == runlist.size() - 1) {
                query += runlist[i] + ")\n";
            }
            else query += runlist[i] + ", ";
        }
    }

    query += "ORDER BY runlet.run_number, runlet.segment_number);\n";

    return query;
}

/* Method to get runs from the temporary table. */
vector<Int_t> QwRunlet::get_runs(void) {
    TString query = run_query();
    TSQLStatement *stmt = db->Statement(query, 100);
    vector<Int_t> run_number;

    if((db!=0) && db->IsConnected()) {
        if(stmt->Process()) {
            stmt->StoreResult();

            while(stmt->NextResultRow()) {
                /* Get values from database. */
                Int_t temp_run_number = stmt->GetInt(0);

                run_number.push_back(temp_run_number);
            }
        }
    }

    return run_number;
}

/* Method to get slugs from the temporary table. */
vector<Int_t> QwRunlet::get_slugs(void) {
    TString query = slug_query();
    TSQLStatement *stmt = db->Statement(query, 100);
    vector<Int_t> slug;

    if((db!=0) && db->IsConnected()) {
        if(stmt->Process()) {
            stmt->StoreResult();

            while(stmt->NextResultRow()) {
                /* Get values from database. */
                Int_t temp_slug = stmt->GetInt(0);

                slug.push_back(temp_slug);
            }
        }
    }

    return slug;
}

/* Method to get wiens from the temporary table. */
vector<Int_t> QwRunlet::get_wiens(void) {
    TString query = wien_query();
    TSQLStatement *stmt = db->Statement(query, 100);
    vector<Int_t> wien;

    if((db!=0) && db->IsConnected()) {
        if(stmt->Process()) {
            stmt->StoreResult();

            while(stmt->NextResultRow()) {
                /* Get values from database. */
                Int_t temp_wien = stmt->GetInt(0);

                wien.push_back(temp_wien);
            }
        }
    }

    return wien;
}

/*
 * This fuction creates all the temporary tables for use later. First, it loops
 * over all regression types,
 */
void QwRunlet::fill(QwParse &reg_types, QwParse &runlist, TString target, Bool_t runavg, Bool_t slugavg, Bool_t wienavg, Bool_t ignore_quality, Int_t run_quality)
{
    // create the fist temp table
    cout << "creating temp runlet tables" << endl;
    TString query;
    TSQLStatement *stmt;

    const Int_t num_regs = reg_types.num_detectors();
    for(Int_t i = 0; i < num_regs; i++) {
        cout << reg_types.detector(i) << endl;

        TString db_name = db->GetDB();
        query = runlet_temp_table_create(reg_types.detector(i), runlist.ret_detector(), target, ignore_quality, run_quality, db_name);
        stmt = db->Statement(query, 100);
        if((db!=0) && db->IsConnected()) {
            stmt->Process();
            delete stmt;
        }
        else cout << "Failed to connect to the database while creating runlet temp table" << endl;
        
        query = runlet_temp_table_unreg_create(reg_types.detector(i), runlist.ret_detector(), target, ignore_quality, run_quality, db_name);
        stmt = db->Statement(query, 100);
        if((db!=0) && db->IsConnected()) {
            stmt->Process();
            delete stmt;
        }
        else cout << "Failed to connect to the database while creating runlet unreg temp table" << endl;
    }

    cout << "querying runlet data" << endl;
    query = runlet_query(runlist.ret_detector(), runavg, slugavg, wienavg);
    stmt = db->Statement(query, 100);

    if((db!=0) && db->IsConnected()) {
        if(stmt->Process()) {
            // store in buffer
            stmt->StoreResult();

            while(stmt->NextResultRow()) {
                // get values from database
                Int_t temp_runlet_id = stmt->GetInt(0);
                Int_t temp_run = stmt->GetInt(1);
                Int_t temp_slug = stmt->GetInt(2);
                Int_t temp_wien_slug = stmt->GetInt(3);
                Int_t temp_segment_number = stmt->GetInt(4);
                TString temp_ihwp_setting = stmt->GetString(5);
                TString temp_phwp_setting = stmt->GetString(6);
                TString temp_wien_reversal = stmt->GetString(7);
                TString temp_precession_reversal = stmt->GetString(8);
                Double_t temp_qtor_current = stmt->GetDouble(9);
                Int_t temp_runlet_quality_id = stmt->GetInt(10);
		Int_t temp_raster_size = stmt->GetInt(11);
                /* FIXME: Add some sort of good for support... */
                //string temp_good_for = stmt->GetString(7);

                runlet_id.push_back(temp_runlet_id);
                run.push_back(temp_run);
                slug.push_back(temp_slug);
                wien_slug.push_back(temp_wien_slug);
                run_number_decimal.push_back((Double_t)temp_run + (Double_t)temp_segment_number/20.0);

                /* Fill ihwp. */
                if(temp_ihwp_setting == "in") ihwp_setting.push_back(1);
                else if(temp_ihwp_setting == "out") ihwp_setting.push_back(0);
                else cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message: "
                    + temp_ihwp_setting << endl;

                /* Fill phwp. */
                if(temp_phwp_setting == "in") phwp_setting.push_back(1);
                else if(temp_phwp_setting == "out") phwp_setting.push_back(0);
                else if(temp_phwp_setting == "") phwp_setting.push_back(-1);
                else cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message: "
                    + temp_phwp_setting << endl;

                /* Fill wien reversal. */
                if(temp_wien_reversal == "normal") wien_reversal.push_back(0);
                else if(temp_wien_reversal == "reverse") wien_reversal.push_back(1);
                else {
                    cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message:\nwien_reversal" + temp_wien_reversal << endl;
                    wien_reversal.push_back(2);
                }

                /* Fill precession reversal. */
                if(temp_precession_reversal == "normal") precession_reversal.push_back(0);
                else if(temp_precession_reversal == "reverse") precession_reversal.push_back(1);
                else cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message: "
                    + temp_precession_reversal << endl;

                /* Fill qtor current. */
                qtor_current.push_back(temp_qtor_current);

                /* Fill sign correction using perscription described in A&S
                 * elog 619 by J. Leacock.
                 *
                 * normal = right flip
                 * reverse = left flip
                 */
                if(temp_precession_reversal == "normal") {
                    if(temp_wien_reversal == "normal") {
                        if(temp_ihwp_setting == "out") {
                            sign_correction.push_back(1);
                        }
                        else if(temp_ihwp_setting == "in") {
                            sign_correction.push_back(-1);
                        }
                    }
                    if(temp_wien_reversal == "reverse") {
                        if(temp_ihwp_setting == "out") {
                            sign_correction.push_back(-1);
                        }
                        else if(temp_ihwp_setting == "in") {
                            sign_correction.push_back(1);
                        }
                    }
                }
                else if(temp_precession_reversal == "reverse") {
                    if(temp_wien_reversal == "normal") {
                        if(temp_ihwp_setting == "out") {
                            sign_correction.push_back(-1);
                        }
                        else if(temp_ihwp_setting == "in") {
                            sign_correction.push_back(1);
                        }
                    }
                    if(temp_wien_reversal == "reverse") {
                        if(temp_ihwp_setting == "out") {
                            sign_correction.push_back(1);
                        }
                        else if(temp_ihwp_setting == "in") {
                            sign_correction.push_back(-1);
                        }
                    }
                }
                else cout << "FAILBOAT" << endl;

                /* Fill the run quality id. */
                runlet_quality_id.push_back((Int_t)temp_runlet_quality_id);
		raster_size.push_back(temp_raster_size);
            }
        }
    }
}

/* Method to branch for the runlet variables. */
void QwRunlet::branch(TTree* tree, QwValues &values) {
    tree->Branch("runlet_id", &(values.runlet_properties[0]));
    tree->Branch("run_number", &(values.runlet_properties[1]));
    tree->Branch("run_number_decimal", &(values.run_number_decimal));
    tree->Branch("slug", &(values.runlet_properties[2]));
    tree->Branch("wien_slug", &(values.runlet_properties[3]));
    tree->Branch("ihwp_setting", &(values.runlet_properties[4]));
    tree->Branch("phwp_setting", &(values.runlet_properties[5]));
    tree->Branch("wien_reversal", &(values.runlet_properties[6]));
    tree->Branch("precession_reversal", &(values.runlet_properties[7]));
    tree->Branch("sign_correction", &(values.runlet_properties[8]));
    tree->Branch("qtor_current", &(values.qtor_current));
    tree->Branch("runlet_quality_id", &(values.runlet_quality_id));
    tree->Branch("raster_size", &(values.raster_size));
}

/* Method to return runlet id. */
vector<Int_t> QwRunlet::get_runlets(void) {
    return runlet_id;
}

/* Method to get the data from QwRunlet and put it into QwData */
void QwRunlet::get_data_for_runlet(Int_t j, QwValues &values) {
    values.runlet_properties[0] = runlet_id[j];
    values.runlet_properties[1] = run[j];
    values.runlet_properties[2] = slug[j];
    values.runlet_properties[3] = wien_slug[j];
    values.runlet_properties[4] = ihwp_setting[j];
    values.runlet_properties[5] = phwp_setting[j];
    values.runlet_properties[6] = wien_reversal[j];
    values.runlet_properties[7] = precession_reversal[j];
    values.runlet_properties[8] = sign_correction[j];
    values.qtor_current = qtor_current[j];
    values.run_number_decimal = run_number_decimal[j];
    values.runlet_quality_id = runlet_quality_id[j];
    values.raster_size = raster_size[j];
}
