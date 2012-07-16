#include <TTree.h>
#include <Riostream.h>
#include <TSQLStatement.h>
#include <TSQLServer.h>
#include <TMath.h>
#include "detector.h"
#include "parse.h"
#include "string.h"

QwData::QwData(Double_t temp_value, Double_t temp_error, Long_t temp_n, Double_t temp_rms)
{
    value = temp_value;
    error = temp_error;
    n = temp_n;
    rms = temp_rms;
}

void QwData::fill_empty(void)
{
    value = -1e6;
    error = -1e6;
    rms = -1e6;
    n = 0;
}

void QwDetector::branch(TTree* tree, vector<QwData> &values, Int_t i)
{
    TString name;
    string type;
    string detector_str = detector_name.Data();
    Int_t pos;

    pos = detector_str.find("qwk_");

    if (pos !=string::npos)
    {
        pos += 4;
        name = detector_str.substr(pos);
    }
    else
    {
        name = detector_name;
    }

    if (measurement_id == "a")
        type = "asym_";
    else if (measurement_id == "d")
        type = "diff_";
    else if (measurement_id == "yq")
        type = "yq_";
    else if (measurement_id == "y")
        type = "y_";
    else
        type = "asym_";

    name = type + name;
    
    tree->Branch(name, &values[i], "value/D:err/D:rms/D:n/I");
}

void QwDetector::fill(void)
{
    cout << "querying " + detector_name << endl;
    TString sql_query = query();
    TSQLStatement* stmt = db->Statement(sql_query, 100);

    if((db!=0) && db->IsConnected())
    {
        if(stmt->Process())
        {
            // store in buffer
            stmt->StoreResult();

            Int_t i = 0;
            while(stmt->NextResultRow())
            {
                // get values from database
                Int_t temp_runlet_id = stmt->GetInt(0);
                Double_t temp_value = stmt->GetDouble(1);
                Double_t temp_error = stmt->GetDouble(2);
                Int_t temp_n = stmt->GetInt(3);

                runlet_id[temp_runlet_id] = i;
                QwData temp_data(temp_value, temp_error, temp_n, temp_error*sqrt(temp_n));
                data.push_back(temp_data);
                i++;
            }
        }
    }
}

void QwDetector::get_data_for_runlet(Int_t runlet, QwData &value)
{
    QwData temp_data;
    if(runlet_id.count(runlet) > 0)
    {
        temp_data = data[runlet_id[runlet]];
        value = temp_data;
    }
    else
    {
        temp_data.fill_empty();
        value = temp_data;
    }
}

// main detectors
TString QwMainDet::query(void)
{
    // figure out which temp table to use
    TString table;
    if(measurement_id == "y") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1") table = "temp_table_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_" + reg_type;

    TString query;
    query = "SELECT\n";
    query += "DISTINCT runlet_id\n";
    query += ", md_data.value*1e6\n";
    query += ", md_data.error*1e6\n";
    query += ", md_data.n\n";

    // join tables to analysis
    query += "FROM " + table + "\n";
    query += "JOIN md_data ON " + table+ ".analysis_id = md_data.analysis_id\n";
    query += "JOIN main_detector ON md_data.main_detector_id = main_detector.main_detector_id\n";

    // md cuts
    query += "WHERE md_data.subblock = 0\n";
    query += "AND md_data.measurement_type_id = \"" + measurement_id + "\"\n";
    query += "AND main_detector.quantity = \"" + detector_name + "\"\n";

    query += "ORDER BY runlet_id;\n";

    return query;
}

// lumi detectors
TString QwLumiDet::query(void)
{
    // figure out which temp table to use
    TString table;
    if(measurement_id == "y") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1") table = "temp_table_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_" + reg_type;

    TString query;
    query = "SELECT\n";
    query += "DISTINCT runlet_id\n";
    query += ", lumi_data.value*1e6\n";
    query += ", lumi_data.error*1e6\n";
    query += ", lumi_data.n\n";

    // join tables to analysis
    query += "FROM " + table + "\n";
    query += "JOIN lumi_data ON " + table + ".analysis_id = lumi_data.analysis_id\n";
    query += "JOIN lumi_detector ON lumi_data.lumi_detector_id = lumi_detector.lumi_detector_id\n";

    // lumi cuts
    query += "WHERE lumi_data.subblock = 0\n";
    query += "AND lumi_data.measurement_type_id = \"" + measurement_id + "\"\n";
    query += "AND lumi_detector.quantity = \"" + detector_name +"\"\n";

    query += "ORDER BY runlet_id;\n";

    return query;
}

// beam detectors
TString QwBeamDet::query(void)
{
    // figure out which temp table to use
    TString table;
    if(reg_type == "on_5+1")  table = "temp_table_unreg_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_unreg_" + reg_type;

    TString query;
    query = "SELECT\n";
    query += "DISTINCT " + table + ".runlet_id\n";
    query += ", beam.value*1e6\n";
    query += ", beam.error*1e6\n";
    query += ", beam.n\n";

    // join tables to analysis
    query += "FROM " + table + "\n";
    query += "JOIN beam ON " + table + ".analysis_id = beam.analysis_id\n";
    query += "JOIN monitor ON beam.monitor_id = monitor.monitor_id\n";

    // beam cuts
    query += "WHERE beam.subblock = 0\n";
    query += "AND beam.measurement_type_id = \"" + measurement_id + "\"\n";
    query += "AND monitor.quantity = \""+ detector_name + "\";";

    return query;
}

// runlet
TString QwRunlet::runlet_query(void)
{
    TString query;
    query = "SELECT\n";
    query += "runlet_id\n";
    query += ", run_number\n";
    query += ", slug\n";
    query += ", slow_helicity_plate\n";
    query += ", passive_helicity_plate\n";
    query += ", wien_reversal\n";
    query += "FROM temp_table_unreg_offoff;\n";

    return query;
}

TString QwRunlet::runlet_temp_table_create(TString reg_type)
{
    TString query;
    if(reg_type == "on_5+1") query = "CREATE TEMPORARY TABLE temp_table_on_5p1 AS(\n";
    else query = "CREATE TEMPORARY TABLE temp_table_" + reg_type +  " AS(\n";
    query += "SELECT\n";
    query += "DISTINCT analysis.analysis_id\n";
    query += ", analysis.runlet_id\n";
    query += ", run.run_number\n";
    query += ", run.slug\n";
    query += ", slow_controls_settings.slow_helicity_plate\n";
    query += ", slow_controls_settings.passive_helicity_plate\n";
    query += ", slow_controls_settings.wien_reversal\n";

    // join tables to analysis
    query += "FROM analysis\n";
    query += "JOIN runlet ON analysis.runlet_id = runlet.runlet_id\n";
    query += "JOIN run ON runlet.run_id = run.run_id\n";
    query += "JOIN slow_controls_settings ON runlet.runlet_id = slow_controls_settings.runlet_id\n";

    // runlet and analysis level cuts
    query += "WHERE analysis.slope_correction = \"" + reg_type + "\"\n";
    query += "AND runlet.runlet_quality_id = 1\n";
    query += "AND(run.good_for_id = \"1\" OR run.good_for_id = \"1,3\")\n";
    query += "AND slow_controls_settings.target_position = \"HYDROGEN-CELL\"\n";

    query += "ORDER BY runlet_id);\n";

    return query;
}

TString QwRunlet::runlet_temp_table_unreg_create(TString reg_type)
{
    TString query;
    if(reg_type == "on_5+1") query = "CREATE TEMPORARY TABLE temp_table_unreg_on_5p1 AS(\n";
    else query = "CREATE TEMPORARY TABLE temp_table_unreg_" + reg_type +  " AS(\n";
    query += "SELECT\n";
    query += "DISTINCT analysis.analysis_id\n";
    query += ", analysis.runlet_id\n";
    query += ", run.run_number\n";
    query += ", run.slug\n";
    query += ", slow_controls_settings.slow_helicity_plate\n";
    query += ", slow_controls_settings.passive_helicity_plate\n";
    query += ", slow_controls_settings.wien_reversal\n";

    // join tables to analysis
    query += "FROM analysis\n";
    query += "JOIN runlet ON analysis.runlet_id = runlet.runlet_id\n";
    query += "JOIN run ON runlet.run_id = run.run_id\n";
    query += "JOIN slow_controls_settings ON runlet.runlet_id = slow_controls_settings.runlet_id\n";

    // runlet and analysis level cuts
    query += "WHERE analysis.slope_correction = \"off\"\n";
    if(reg_type == "off") query += "AND analysis.slope_calculation = \"on\"\n";
    else if(reg_type == "offoff") query += "AND analysis.slope_calculation = \"off\"\n";
    else query += "AND analysis.slope_calculation = \"" + reg_type + "\"\n";
    query += "AND runlet.runlet_quality_id = 1\n";
    query += "AND(run.good_for_id = \"1\" OR run.good_for_id = \"1,3\")\n";
    query += "AND slow_controls_settings.target_position = \"HYDROGEN-CELL\"\n";

    query += "ORDER BY runlet_id);\n";

    return query;
}

void QwRunlet::fill(QwParse &reg_types)
{
    // create the fist temp table
    cout << "creating temp runlet tables" << endl;
    TString query;
    TSQLStatement *stmt;

    const Int_t num_regs = reg_types.num_detectors();
    for(Int_t i = 0; i < num_regs; i++)
    {
        cout << reg_types.detector(i) << endl;

        query = runlet_temp_table_create(reg_types.detector(i));
        stmt = db->Statement(query, 100);
        if((db!=0) && db->IsConnected())
        {
            stmt->Process();
            delete stmt;
        }
        else cout << "Failed to connect to the database while creating runlet temp table" << endl;
        
        query = runlet_temp_table_unreg_create(reg_types.detector(i));
        stmt = db->Statement(query, 100);
        if((db!=0) && db->IsConnected())
        {
            stmt->Process();
            delete stmt;
        }
        else cout << "Failed to connect to the database while creating runlet unreg temp table" << endl;
    }

    query = runlet_temp_table_unreg_create("offoff");
    stmt = db->Statement(query, 100);
    if((db!=0) && db->IsConnected())
    {
        stmt->Process();
        delete stmt;
    }
    else cout << "Failed to connect to the database while creating runlet unreg temp table" << endl;

    cout << "querying runlet data" << endl;
    query = runlet_query();
    stmt = db->Statement(query, 100);

    if((db!=0) && db->IsConnected())
    {
        if(stmt->Process())
        {
            // store in buffer
            stmt->StoreResult();

            while(stmt->NextResultRow())
            {
                // get values from database
                Int_t temp_runlet_id = stmt->GetInt(0);
                Int_t temp_run = stmt->GetInt(1);
                Int_t temp_slug = stmt->GetInt(2);
                TString temp_ihwp_setting = stmt->GetString(3);
                TString temp_phwp_setting = stmt->GetString(4);
                TString temp_wien_reversal = stmt->GetString(5);

                runlet_id.push_back(temp_runlet_id);
                run.push_back(temp_run);
                slug.push_back(temp_slug);

                // fill ihwp
                if(temp_ihwp_setting == "in") ihwp_setting.push_back(1);
                else if(temp_ihwp_setting == "out") ihwp_setting.push_back(0);
                else cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message: "
                    + temp_ihwp_setting << endl;

                // fill phwp
                if(temp_phwp_setting == "in") phwp_setting.push_back(1);
                else if(temp_phwp_setting == "out") phwp_setting.push_back(0);
                else if(temp_phwp_setting == "") phwp_setting.push_back(-1);
                else cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message: "
                    + temp_phwp_setting << endl;

                // fill wien
                if(temp_wien_reversal == "normal") wien_reversal.push_back(0);
                else if(temp_wien_reversal == "reverse") wien_reversal.push_back(1);
                else cout << "Please contact Wade Duvall at wsduvall@jlab.org with the following error message:\nwien_reversal"
                    + temp_wien_reversal << endl;
            }
        }
    }
}

void QwRunlet::branch(TTree* tree, vector<Int_t> &values)
{
    tree->Branch("runlet_id", &(values[0]));
    tree->Branch("run", &(values[1]));
    tree->Branch("slug", &(values[2]));
    tree->Branch("ihwp_setting", &(values[3]));
    tree->Branch("phwp_setting", &(values[4]));
    tree->Branch("wien_reversal", &(values[5]));
}

vector<Int_t> QwRunlet::get_runlets(void)
{
    return runlet_id;
}

void QwRunlet::get_data_for_runlet(Int_t j, vector<Int_t> &values)
{
    values[0] = runlet_id[j];
    values[1] = run[j];
    values[2] = slug[j];
    values[3] = ihwp_setting[j];
    values[4] = phwp_setting[j];
    values[5] = wien_reversal[j];
}
