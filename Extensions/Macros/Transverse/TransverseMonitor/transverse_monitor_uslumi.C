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
  vector<Int_t> asym_num;
  vector<Double_t> asym;
  vector<Double_t> asym_error;
  vector<TString> ihwp;
  vector<Int_t> slug_num;
  vector<Double_t> slug;
  vector<Double_t> slug_error;
  vector<TString> ihwp_slug;
  vector<Double_t> slug_IN;
  vector<Double_t> slug_IN_error;
  vector<Double_t> slug_OUT;
  vector<Double_t> slug_OUT_error;
  vector<Double_t> wien_IN;
  vector<Double_t> wien_IN_error;
  vector<Double_t> wien_OUT;
  vector<Double_t> wien_OUT_error;
  void get_lumi (Int_t detector_id, const Char_t* table, TSQLServer* db);
  void slug_avg (void);
  void wien_avg (void);

};

void detector::get_lumi (Int_t detector_id, const Char_t* table, TSQLServer* db)
{

  // placeholder var that will become cmd line option
	TString data = Form("%s.lumi_data", table);
	TString data_reg = Form("%s.lumi_data", table);
	TString analysis = Form("%s.analysis", table);
	TString analysis_reg = Form("%s.analysis", table);
	TString runlet = Form("%s.runlet", table);
	TString run = Form("%s.run", table);
	TString plate = Form("%s.slow_controls_settings", table);
	TString beam = Form("%s.beam", table);
	TString values = "run.slug, data_reg.value, data_reg.error, plate.slow_helicity_plate";
	TString detector = Form("%i",detector_id);

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
	// join regressed data
	// join regressed analysis ids
	query += "JOIN " + analysis_reg + " AS analysis_reg\n";
	// conditions for analysis_reg
	query += "ON analysis.slope_correction != analysis_reg.slope_correction\n";
	query += "AND analysis.runlet_id = analysis_reg.runlet_id\n";
	// join regressed data
	query += "JOIN " + data_reg + " AS data_reg\n";
	// conditions for regressed data
	query += "ON analysis_reg.analysis_id = data_reg.analysis_id\n";
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
	query += "WHERE (run.slug >= 117 AND run.slug <= 136)\n";
	query += "AND run.run_quality_id = 1\n";
	// select detector and cut on quartets
	query += "AND data_reg.measurement_type_id = \"a\"\n";
	query += "AND data_reg.subblock = 0\n";
	query += "AND data_reg.lumi_detector_id = " + detector + "\n";
	query += "AND data_reg.error != 1\n";
	query += "AND data_reg.n > 50000\n";
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
	
	// debug
	//cout << query << endl;
	
	TSQLStatement* stmt = db->Statement(query, 100);
	if (stmt->Process())
	{
		// store in buffer
		stmt->StoreResult();

		// print data
		while (stmt->NextResultRow())
		{
			Int_t field_0 = stmt->GetInt(0);
			Double_t field_1 = stmt->GetDouble(1)*1.0e6;
			Double_t field_2 = stmt->GetDouble(2)*1.0e6;
			TString field_3 = stmt->GetString(3);
			this->asym_num.push_back(field_0);
			this->asym.push_back(field_1);
			this->asym_error.push_back(field_2);
			this->ihwp.push_back(field_3);
		}
	}
}

void detector::slug_avg (void)
{
  UInt_t i;
  vector<Double_t> sum;
	// set slug_num and ihwp by slug instead of by run
	for (i = 0; i < this->asym_num.size(); i++)
  {
    // skip null values of asym_error, and run 63
    if (TMath::IsNaN(this->asym_error[i])) continue;
    if (this->asym_num[i] == 63) continue;
    // for the initial value
    if (0 == this->slug_num.size())
    {
    	this->slug_num.push_back(this->asym_num[i]);
    	this->ihwp_slug.push_back(this->ihwp[i]);
    } 
    // for all other values where the most recent slug number is
    // different than the current slug number
    else if (this->slug_num.back() != this->asym_num[i])
    {
    	this->slug_num.push_back(this->asym_num[i]);
    	this->ihwp_slug.push_back(this->ihwp[i]);
    }
  }
  // initialize the slug and slug_error vectors
  for (i = 0; i < this->slug_num.size(); i++)
  {
    	this->slug.push_back(0);
    	this->slug_error.push_back(0);
	}
	// calculate the weighted averages by slug
	for (i = 0; i < this->asym_num.size(); i++)
  {
    // skip null values of asym_error, and run 63
    if (TMath::IsNaN(this->asym_error[i])) continue;
    if (this->asym_num[i] == 63) continue;
    // sum all the runs for a particular slug
    // slug numbers are normalized to 0
		this->slug[this->asym_num[i] - this->asym_num.front()] += this->asym[i] / (this->asym_error[i] * this->asym_error[i]);
  	this->slug_error[this->asym_num[i] - this->asym_num.front()] += 1 / (this->asym_error[i] * this->asym_error[i]);
  	//cout << this->asym_num[i] << "   " << this->slug[this->asym_num[i] - this->asym_num.front()] << "   " << this->slug_error[this->asym_num[i] - this->asym_num.front()] << endl;
  }
  // finish calculating weighted average
  for (i = 0; i < slug.size (); i++)
  {
  	// ignore empty slugs
  	if (0 == this->slug[i])
  	{
  		slug.erase(this->slug.begin()+i);
  		slug_error.erase(this->slug_error.begin()+i);
  		continue;
  	}
    this->slug[i] = this->slug[i] / this->slug_error[i];
    this->slug_error[i] = 1 / sqrt(this->slug_error[i]);
  }
}

void detector::wien_avg (void)
{
  UInt_t i;
  for (i = 0; i < this->slug_num.size(); i++)
  {
  	if (this->ihwp_slug[i] == "in")	{
  		slug_IN.push_back(this->slug[i]);
  		slug_IN_error.push_back(this->slug_error[i]);
  	}
  	else 
  	{
  		slug_OUT.push_back(this->slug[i]);
  		slug_OUT_error.push_back(this->slug_error[i]);
  	}
  }

  wien_IN.push_back(0);
  wien_IN_error.push_back(0);
  wien_OUT.push_back(0);
  wien_OUT_error.push_back(0);


  for (i = 0; i < this->slug_IN.size(); i++)
  {
  	// in
  	wien_IN[1] += this->slug_IN[i] / (this->slug_IN_error[i] * this->slug_IN_error[i]);
  	wien_IN_error[1] += 1 / (this->slug_IN_error[i] * this->slug_IN_error[i]);
  }

  for (i = 0; i < this->slug_OUT.size(); i++)
  {
  	// out
  	wien_OUT[1] += this->slug_OUT[i] / (this->slug_OUT_error[i] * this->slug_OUT_error[i]);
  	wien_OUT_error[1] += 1 / (this->slug_OUT_error[i] * this->slug_OUT_error[i]);

  }

  // debug
  //cout << "IN   " << this->wien_IN[1] / this->wien_IN_error[1] << "   " << 1 / sqrt(this->wien_IN_error[1]) << endl;
  //cout << "OUT   " << this->wien_OUT[1] / this->wien_OUT_error[1] << "   " << 1 / sqrt(this->wien_OUT_error[1]) << endl;
  
  wien_IN[1] = this->wien_IN[1] / this->wien_IN_error[1];
  wien_OUT[1] = this->wien_OUT[1] / this->wien_OUT_error[1];

	wien_IN_error[1] = 1 / sqrt(this->wien_IN_error[1]);
	wien_OUT_error[1] = 1 / sqrt(this->wien_OUT_error[1]);
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
  
  // test function for pull
  detector uslumi_1, uslumi_3, uslumi_5, uslumi_7;
	uslumi_1.get_lumi (162, "qw_run1_pass3", db);
	uslumi_3.get_lumi (172, "qw_run1_pass3", db);
	uslumi_5.get_lumi (182, "qw_run1_pass3", db);
	uslumi_7.get_lumi (192, "qw_run1_pass3", db);

	Int_t i;
	// debug
	for (i = 0; i < uslumi_1.asym.size(); i++)
	{
		//cout << test.asym_num[i] << "   " << test.asym[i] << "   " << test.asym_error[i] << "   " << test.ihwp[i] << endl;
	}
	
	// test function for slug avg
	uslumi_1.slug_avg();
	uslumi_3.slug_avg();
	uslumi_5.slug_avg();
	uslumi_7.slug_avg();
	// debug
	for (i = 0; i < uslumi_1.slug_num.size(); i++)
	{
		//cout << "1   " << uslumi_1.slug_num[i] << "   " << uslumi_1.slug[i] << "   " << uslumi_1.slug_error[i] << "   " << uslumi_1.ihwp_slug[i] << endl;
		//cout << "3   " << uslumi_3.slug_num[i] << "   " << uslumi_3.slug[i] << "   " << uslumi_3.slug_error[i] << "   " << uslumi_3.ihwp_slug[i] << endl;
		//cout << "5   " << uslumi_5.slug_num[i] << "   " << uslumi_5.slug[i] << "   " << uslumi_5.slug_error[i] << "   " << uslumi_5.ihwp_slug[i] << endl;
		//cout << "7   " << uslumi_7.slug_num[i] << "   " << uslumi_7.slug[i] << "   " << uslumi_7.slug_error[i] << "   " << uslumi_7.ihwp_slug[i] << endl;
	}

	// calculate the avg wien
	uslumi_1.wien_avg();
	uslumi_3.wien_avg();
	uslumi_5.wien_avg();
	uslumi_7.wien_avg();

	cout << "1   " << uslumi_1.wien_IN[1] - uslumi_1.wien_OUT[1] << "   " << uslumi_1.wien_IN_error[1] << endl;
	cout << "3   " << uslumi_3.wien_IN[1] - uslumi_3.wien_OUT[1] << "   " << uslumi_3.wien_IN_error[1] << endl;
	cout << "5   " << uslumi_5.wien_IN[1] - uslumi_5.wien_OUT[1] << "   " << uslumi_5.wien_IN_error[1] << endl;
	cout << "7   " << uslumi_7.wien_IN[1] - uslumi_7.wien_OUT[1] << "   " << uslumi_7.wien_IN_error[1] << endl;

  vector<Double_t> wien;
  vector<Double_t> wien_error;
  vector<Double_t> uslumi_number;
  vector<Double_t> error;

	wien.push_back(uslumi_1.wien_IN[1] - uslumi_1.wien_OUT[1]);
	wien.push_back(uslumi_3.wien_IN[1] - uslumi_3.wien_OUT[1]);
	wien.push_back(uslumi_5.wien_IN[1] - uslumi_5.wien_OUT[1]);
	wien.push_back(uslumi_7.wien_IN[1] - uslumi_7.wien_OUT[1]);
	wien_error.push_back(uslumi_1.wien_IN_error[1] + uslumi_1.wien_OUT_error[i]);
	wien_error.push_back(uslumi_3.wien_IN_error[1] + uslumi_3.wien_OUT_error[i]);
	wien_error.push_back(uslumi_5.wien_IN_error[1] + uslumi_5.wien_OUT_error[i]);
	wien_error.push_back(uslumi_7.wien_IN_error[1] + uslumi_7.wien_OUT_error[i]);
	uslumi_number.push_back(1);
	uslumi_number.push_back(3);
	uslumi_number.push_back(5);
	uslumi_number.push_back(7);
	error.push_back(0);
	error.push_back(0);
	error.push_back(0);
	error.push_back(0);


	// create ROOT application for graphs
	TApplication *app = new TApplication("transverse polerization uslumi", &argc, argv);

	// create ROOT canvas
	TCanvas *c1 = new TCanvas("c1", "transverse polerization uslumi", 800, 800);

	// graph wien
	TGraphErrors *gr1 = new TGraphErrors (wien.size(), &(uslumi_number[0]), &(wien[0]), &(error[0]), &(wien_error[0]));
	gr1->Draw("AP");
	app->Run();


	// close the db
	db->Close();

	return 0;
}
