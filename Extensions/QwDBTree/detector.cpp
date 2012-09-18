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
        type = "charge_";
    else if (measurement_id == "y")
        type = "yield_";
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
    if(reg_type == "offoff") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1") table = "temp_table_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_" + reg_type;

    TString query;
    query = "SELECT\n";
    query += "DISTINCT runlet_id\n";
    if(measurement_id == "a" || measurement_id == "d")
    {
        query += ", md_data.value*1e6\n";
        query += ", md_data.error*1e6\n";
    }
    else
    {
        query += ", md_data.value\n";
        query += ", md_data.error\n";
    }
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
    if(reg_type == "offoff") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1") table = "temp_table_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_" + reg_type;

    TString query;
    query = "SELECT\n";
    query += "DISTINCT runlet_id\n";
    if(measurement_id == "a" || measurement_id == "d")
    {
        query += ", lumi_data.value*1e6\n";
        query += ", lumi_data.error*1e6\n";
    }
    else
    {
        query += ", lumi_data.value\n";
        query += ", lumi_data.error\n";
    }
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
    if(reg_type == "offoff") table = "temp_table_unreg_offoff";
    else if(reg_type == "on_5+1")  table = "temp_table_unreg_on_5p1";
    else if(reg_type == "off") table = "temp_table_unreg_on";
    else table = "temp_table_unreg_" + reg_type;

    TString query;
    query = "SELECT\n";
    query += "DISTINCT " + table + ".runlet_id\n";
    if(measurement_id == "a" || measurement_id == "d")
    {
        query += ", beam.value*1e6\n";
        query += ", beam.error*1e6\n";
    }
    else
    {
        query += ", beam.value\n";
        query += ", beam.error\n";
    }
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

