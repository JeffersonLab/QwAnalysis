#include <iostream>
#include <fstream>
#include <TString.h>
#include <math.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TApplication.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TApplication.h>
#include <Rtypes.h>
#include <stdexcept>
#include <TLine.h>
#include <TPaveStats.h>
#include <stdio.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TText.h>
#include <TAxis.h>
#include <TPad.h>
#include <TVector.h>

using namespace std;

struct detector 
{
  // define vectors for calculating weighted averages of slugs
  vector<Int_t> slug_num;
  vector<Double_t> slug;
  vector<Double_t> slug_error;
  vector<TString> ihwp;
  vector<Double_t> slug_weighted_avg;
  vector<Double_t> slug_error_sum;
  void get_lumi (Int_t detector_id, const Char_t* table, TSQLServer* db);
};

void detector::get_lumi (Int_t detector_id, const Char_t* table, TSQLServer* db)
{

  // placeholder var that will become cmd line option
	TString data = Form("%s.lumi_data", table);
	TString analysis = Form("%s.analysis", table);
	TString runlet = Form("%s.runlet", table);
	TString run = Form("%s.run", table);
	TString plate = Form("%s.slow_controls_settings", table);
	TString beam = Form("%s.beam", table);
	TString values = "run.slug, data.value, data.error, plate.slow_helicity_plate";

	// MySQL query
	// select the data from the table
	TString query = "SELECT DISTINCT " + values + " FROM " + data + " AS data\n";
	// join analysis
	query += "JOIN " + analysis + " AS analysis\n";
	// conditions for analysis
	query += "ON data.analysis_id = analysis.analysis_id\n";
	// join runlet
	query += "JOIN " + runlet + " AS runlet\n";
	// conditions for runlet
	query += "ON analysis.runlet_id = runlet.runlet_id\n";
	// join run
	query += "JOIN " + run + " AS run\n";
	// conditions for run
	query += "ON runlet.run_id = run.run_id\n";
	// join beam data 1
	query += "JOIN " + beam + " AS beam_1\n";
	// conditions for beam data 1
	query += "ON data.analysis_id = beam_1.analysis_id\n";
	// join beam data 2
	query += "JOIN " + beam + " AS beam_2\n";
	// conditions for beam data 2
	query += "ON data.analysis_id = beam_2.analysis_id\n";
	// join plate
	query += "JOIN " + plate + " AS plate\n";
	// conditions for plate
	query += "ON runlet.runlet_id = plate.runlet_id\n";
	// select quality and slugs
	query += "WHERE (run.slug >= 42 AND run.slug <= 136)\n";
	query += "AND run.run_quality_id = 1\n";
	// select detector and cut on quartets
	query += "AND data.measurement_type_id = \"a\"\n";
	query += "AND data.subblock = 0\n";
	query += "AND data.lumi_detector_id = 162\n";
	query += "AND data.error != 1\n";
	query += "AND data.n > 50000\n";
	query += "AND run.run_quality_id = 1\n";
	// cut on beam data
	// quartets, beam asymmetry, and beam current
	query += "AND analysis.slope_correction = \"off\"\n";
	query += "AND beam_1.monitor_id = 2642\n";
	query += "AND beam_1.measurement_type_id = \"yq\"\n";
	query += "AND beam_1.subblock = 0\n";
	query += "AND beam_1.value > 120\n";
	query += "AND beam_1.error !=0\n";
	query += "AND beam_1.n > 50000\n";
	query += "AND beam_2.monitor_id = 2642\n";
	query += "AND beam_2.measurement_type_id = \"a\"\n";
	query += "AND beam_2.subblock = 0\n";
	query += "AND beam_2.value < 1e-5\n";
	query += "AND beam_2.error !=0\n";
	query += "AND beam_2.n > 50000\n";
	query += "ORDER BY runlet.runlet_id, run.run_number, runlet.segment_number;";
	cout << query << endl;
	
	TSQLStatement* stmt = db->Statement(query, 100);
	if (stmt->Process())
	{
		// store in buffer
		stmt->StoreResult();

		// print data
		while (stmt->NextResultRow())
		{
			Int_t field_0 = stmt->GetInt(0);
			Double_t field_1 = stmt->GetDouble(1);
			Double_t field_2 = stmt->GetDouble(2);
			TString field_3 = stmt->GetString(3);
			this->slug_num.push_back(field_0);
			this->slug.push_back(field_1);
			this->slug_error.push_back(field_2);
			this->ihwp.push_back(field_3);
		}
	}
}

Int_t main(int argc, char *argv[])
{
	// connect to qweak server
	TSQLServer *db = TSQLServer::Connect("mysql://127.0.0.1/qw_run1_pass3","qweak", "QweakQweak");
	
	// check if connection was successful
	if (0 == db || 1 != db->IsConnected())
	{
    cout << "Error opening data file" << endl;
    exit (0);
  }
  
  // test function
  detector test;
	test.get_lumi(162, "qw_run1_pass3", db);
	Int_t i;
	for(i = 0; i < test.slug.size(); i++)
	{
		cout << test.slug_num[i] << "   " << test.slug[i] << "   " << test.slug_error[i] << "   " << test.ihwp[i] << endl;
	}
	return 0;
}
