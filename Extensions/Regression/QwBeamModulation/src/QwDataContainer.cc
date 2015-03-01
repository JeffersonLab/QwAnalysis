#define QwDataContainer_cxx
#include "../include/QwDataContainer.hh"
#include "../include/headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TMultiGraph.h"
#include "TLegend.h"

#ifdef QwDataContainer_cxx

QwDataContainer::QwDataContainer():fMysqlBeamMode("nbm"), fSegmentFlag("segment_number=0"), 
				   fDBase(false), fNDelta(false), fRunRange(false), fModType(false), 
				   fSegmentNumber(false), fMysqlSetStem(false), 
				   fMysqlSetDet(false), fNumberMonitor(1) 
{

  // Establish a connection to the database through QwDBConnection

  dbase.Connect();

  // Initialize Data arrays

  fSensitivity.resize(5);
  fSensitivityError.resize(5);
  fCorrection.resize(5);
  fAsymmetry.resize(5);
  fPositionDiff.resize(5);
  frms.resize(5);

  // There needs to be a conversion factor between the number read in for the ascii db and the qweakdb

//   GetBlackList();

}

void QwDataContainer::ConnectDB()
{
  // Establish a connection to the database through QwDBConnection

  dbase.Connect();

}

QwDataContainer::~QwDataContainer()
{
  dbase.Close();
}

void QwDataContainer::SetFileName(TString filename)
{
  fFileName = filename;
  return;
}

void QwDataContainer::GetBlackList()
{
  Int_t temp;
  
  fstream black_list;
  
  black_list.open("config/run.blacklist", fstream::in | fstream::out);
  if(!black_list.is_open()){
    std::cout << "Error opening: run.blacklist\t" << std::endl;
    exit(1);
  }
  
  while(!black_list.eof()){
    black_list >> temp;
    fBlackList.push_back(temp);    
  }

  black_list.close();
 
}

void QwDataContainer::GetOptions(Char_t **options)
{
  Int_t i = 0;
  size_t index = 0;
  TString flag;

  while(options[i] != NULL){
    flag = options[i];
    fOptions.push_back(options[i]);

    if(fOptions[i].CompareTo("--database", TString::kExact) == 0) fDBase = true;
    if(fOptions[i].CompareTo("--ndelta", TString::kExact) == 0){
      fNDelta = true;
    }

    if(flag.CompareTo("--run") == 0){
      fRunRange = true;
      flag.Clear();
      fOptions.push_back(options[i + 1]);
      SetRunRange(atoi(fOptions.back()));
    }

    if(flag.CompareTo("--segment", TString::kExact) == 0){
      fSegmentNumber = true;
      flag.Clear();
      fOptions.push_back(options[i + 1]);
      SetSegNumber(atoi(fOptions.back()));
    }

    if(flag.CompareTo("--runs", TString::kExact) == 0){
      std::string option(options[i+1]);

      flag.Clear();
      index = option.find_first_of(":", 1);
//       std::cout << "Index:\t" << index << std::endl;
      fMysqlRunLower = atoi(option.substr(0, index).c_str());
      fMysqlRunUpper = atoi(option.substr(index + 1, (index + 1) - option.length()).c_str());
      std::cout << "RunRange:\t" << fMysqlRunLower << ":" << fMysqlRunUpper << std::endl;
      fRunRange = true;

    }
    if(flag.CompareTo("--set-stem", TString::kExact) == 0){
      flag.Clear();
      std::cout << "Set: " << options[i+1] << std::endl;
      SetSlopeCorrection(options[i+1]);
      if(fMysqlSlopeCorr.CompareTo("on_beammod_1", TString::kExact) == 0)
	fMysqlBeamMode = "cp";
      if(fMysqlSlopeCorr.CompareTo("on_beammod_2", TString::kExact) == 0)
	fMysqlBeamMode = "cp";
      if(fMysqlSlopeCorr.CompareTo("on_beammod_3", TString::kExact) == 0)
	fMysqlBeamMode = "cp";
      if(fMysqlSlopeCorr.CompareTo("on_beammod_4", TString::kExact) == 0)
	fMysqlBeamMode = "cp";

    }

    if(flag.CompareTo("--detector", TString::kExact) == 0){
      fOptions.push_back(options[i + 1]);
      SetDetector(fOptions.back());
    }

    if(flag.CompareTo("--pass5a", TString::kExact) == 0){
//       fSegmentFlag="&& segment_number IS NULL";
    }

    if(flag.CompareTo("--modulation-type", TString::kExact) == 0){
      std::cout << options[i] << std::endl;
      fOptions.push_back(options[i + 1]);
      SetModulationType(fOptions.back());
      fModType = true;
    }
    i++;
  }

  return;
}

Int_t QwDataContainer::QueryDB(const char* request)
{
  return( dbase.Query(request) );
}

void QwDataContainer::Close()
{
  dbase.Close();
}

void QwDataContainer::Print()
{
  dbase.Print();
}

Double_t QwDataContainer::GetSlope()
{
  return(slope);
}

Double_t QwDataContainer::GetError()
{
  return(error);
}

Double_t QwDataContainer::GetChiNDF()
{
  return(chisquare_ndf);
}

void QwDataContainer::SetSlope(Double_t val)
{
  slope = val;
  return;
}

void QwDataContainer::SetError(Double_t val)
{
  error = val;
  return;
}

void QwDataContainer::SetChiNDF(Double_t val)
{
  chisquare_ndf = val;
  return;
}

Double_t QwDataContainer::GetMaximum(TVectorD array)
{
  Double_t temp = array[0];

  for(Int_t i = 0; i < array.GetNrows(); i++){
    if(array[i] > temp) temp = array[i];
  }
  return(temp);
}

Double_t QwDataContainer::GetMaximum(Double_t *array)
{
  Double_t temp = array[0];

  Int_t nlines = sizeof(array)/sizeof(Double_t);

  for(Int_t i = 0; i < nlines; i++){
    if(array[i] > temp) temp = array[i];
  }
  return(temp);
}

Double_t QwDataContainer::GetMinimum(TVectorD array)
{
  Double_t temp = array[0];

  for(Int_t i = 0; i < array.GetNrows(); i++){
    if(array[i] < temp) temp = array[i];
  }
  return(temp);
}

Double_t QwDataContainer::GetMinimum(Double_t *array)
{
  Double_t temp = array[0];

  Int_t nlines = sizeof(array)/sizeof(Double_t);

  for(Int_t i = 0; i < nlines; i++){
    if(array[i] < temp) temp = array[i];
  }
  return(temp);
}

void QwDataContainer::ScaleTGraph(TGraphErrors *tgraph)
{

  Double_t max = GetMaximum(tgraph->GetY());
  Double_t min = GetMinimum(tgraph->GetY());

  std::cout << max << " " << min << std::endl;

  if(tgraph == NULL) return;

  if( max < 0){
    max -= 2.0*max;
  } else{
    max += 2.0*max;
  }   
  if( min < 0){
    min += 2.0*min;
  } else{
    min -= 2.0*min;
  }   
  tgraph->GetYaxis()->SetRangeUser(min, max);
  return;
}

void QwDataContainer::SetDetector(TString detector)
{
  fMysqlSetDet = true;
  fMysqlDet = detector;
  fDetector = detector;
}

void QwDataContainer::SetSlopeCorrection(TString set)
{
  fMysqlSetStem = true;
  fMysqlSlopeCorr = set;
}

void QwDataContainer::SetSlope(TString slope)
{
  fMysqlSlope = slope;
}

void QwDataContainer::SetIHWP(TString ihwp)
{
  fMysqlIHWP = ihwp;
}

void QwDataContainer::SetMonitor(TString monitor)
{
  fMysqlMon = monitor;
}

void QwDataContainer::SetModulationType(TString modulation)
{
  fModulationType = modulation;
}

void QwDataContainer::SetRunRange(Int_t lower, Int_t upper)
{
    fMysqlRunLower = lower;
    fMysqlRunUpper = upper;
}

void QwDataContainer::SetRunRange(Int_t run)
{
    fMysqlRunNumber = run;
}

void QwDataContainer::SetSegNumber(Int_t segment)
{
    fMysqlSegNumber = segment;
}

TString QwDataContainer::BuildQuery(TString type)
{
  //
  // - In the queries below the runlet0.segment=0 has been changed to <table>.segment_number IS NULL
  // - Also I have removed the segment number from the query output since it is NULL in pass5a and 
  //   breaks the rest of the script.
  //

  if(type.CompareTo("sens", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select md_slope_view.run_number ,value,error,slope from analysis,md_slope_view, slow_controls_settings, runlet as runlet0 where md_slope_view.run_number=runlet0.run_number && md_slope_view.segment_number IS NULL && analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='%s' and analysis.slope_calculation='off' and analysis.slope_correction='on' and md_slope_view.run_quality_id=1 and md_slope_view.good_for_id='1,3' and detector = '%s' and runlet0.runlet_id=slow_controls_settings.runlet_id && slow_controls_settings.slow_helicity_plate = '%s' md_slope_view.run_number >= %i && md_slope_view.run_number <= %i ", fMysqlBeamMode.Data(), fMysqlDet.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }

  if(type.CompareTo("sens_all", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select md_slope_view.run_number, md_slope_view.segment_number, value, error, slope from analysis,md_slope_view, slow_controls_settings, runlet where slow_controls_settings.runlet_id=runlet.runlet_id AND runlet.run_number=md_slope_view.run_number AND runlet.segment_number=0 AND slow_controls_settings.target_position='HYDROGEN-CELL' AND analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='%s' and analysis.slope_calculation= '%s' and analysis.slope_correction='off' and md_slope_view.run_quality_id=1 and md_slope_view.good_for_id='1,3' and detector = '%s' and md_slope_view.run_number >= %i && md_slope_view.run_number <= %i; ", fMysqlBeamMode.Data(), fMysqlSlopeCorr.Data(), fMysqlDet.Data() , fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("sens_all_ndelta", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select md_slope_view.run_number, md_slope_view.segment_number, value, error, slope from analysis,md_slope_view, slow_controls_settings, runlet where slow_controls_settings.runlet_id=runlet.runlet_id AND runlet.run_number=md_slope_view.run_number AND runlet.segment_number=0 AND slow_controls_settings.target_position='HYDROGEN-CELL' AND analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='%s' and analysis.slope_calculation= '%s' and analysis.slope_correction='off' and md_slope_view.run_quality_id=1 and md_slope_view.good_for_id='1,3,18' and detector = '%s' and md_slope_view.run_number >= %i && md_slope_view.run_number <= %i; ", fMysqlBeamMode.Data(), fMysqlSlopeCorr.Data(), fMysqlDet.Data() , fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("sens_all_bcm", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select beam_slope_view.run_number, md_slope_view.segment_number, value, error, slope from analysis,beam_slope_view, slow_controls_settings, runlet where slow_controls_settings.runlet_id=runlet.runlet_id AND runlet.run_number=beam_slope_view.run_number AND runlet.segment_number=0 AND slow_controls_settings.target_position='HYDROGEN-CELL' AND beam_slope_view.segment_number IS NULL && analysis.analysis_id=beam_slope_view.analysis_id and analysis.beam_mode='%s' and analysis.slope_calculation= '%s' and analysis.slope_correction='off' and beam_slope_view.run_quality_id=1 and beam_slope_view.good_for_id='1,3' and detector = '%s' and beam_slope_view.run_number >= %i && beam_slope_view.run_number <= %i; ", fMysqlBeamMode.Data(), fMysqlSlopeCorr.Data(), fMysqlDet.Data() , fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("corrections_all", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select analysis_view.run_number, analysis_view.segment_number, slope, md_correction.value, md_correction.error, analysis_view.slope_correction,main_detector.quantity as detector from analysis_view,slope_type,main_detector,md_correction  where md_correction.slope_type_id=slope_type.slope_type_id && md_correction.main_detector_id=main_detector.main_detector_id && analysis_view.beam_mode='nbm' and main_detector.quantity='%s' and analysis_view.segment_number=0 and analysis_view.analysis_id=md_correction.analysis_id and analysis_view.slope_correction='%s' and analysis_view.run_number >= %i and analysis_view.run_number <= %i;", fMysqlDet.Data() , fMysqlSlopeCorr.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("sens_all_lumi", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select lumi_slope_view.run_number, md_slope_view.run_number, value, error, slope, slow_controls_settings.slow_helicity_plate  from analysis,lumi_slope_view, slow_controls_settings, runlet as runlet0 where runlet0.segment_number=0 AND lumi_slope_view.run_number=runlet0.run_number && lumi_slope_view.segment_number IS NULL && analysis.analysis_id=lumi_slope_view.analysis_id and analysis.beam_mode='%s' and analysis.slope_calculation= '%s' and analysis.slope_correction='off' and lumi_slope_view.run_quality_id=1 and lumi_slope_view.good_for_id='1,3' and detector = '%s' and runlet0.runlet_id=slow_controls_settings.runlet_id && lumi_slope_view.run_number >= %i && lumi_slope_view.run_number <= %i; ", fMysqlBeamMode.Data(), fMysqlSlopeCorr.Data(), fMysqlDet.Data() , fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("corrected", TString::kExact) == 0){
    dbase.fNumberColumns = 1;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select distinct(md_data_view.run_number),value,error,n from analysis,md_data_view, slow_controls_settings, runlet as runlet0 where md_data_view.run_number=runlet0.run_number && runlet0.segment_number=0 && analysis.analysis_id=md_data_view.analysis_id and analysis.beam_mode='cp' and analysis.slope_calculation='off' and analysis.slope_correction='on' and md_data_view.run_quality_id=1 and md_data_view.good_for_id='1,3' and detector = '%s' and runlet0.runlet_id=slow_controls_settings.runlet_id && slow_controls_settings.slow_helicity_plate = '%s' and md_data_view.run_number > %i && md_data_view.run_number < %i and error != 0", fMysqlDet.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("uncorrected", TString::kExact) == 0){
    dbase.fNumberColumns = 1;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select run_number, value, error, n, segment_number from md_data_view, slow_controls_settings where md_data_view.runlet_id = slow_controls_settings.runlet_id and good_for_id = '1,3' and run_quality_id = '1' and beam_mode = 'nbm' and slope_calculation = 'off' and slope_correction ='off' and detector = '%s' and slow_helicity_plate = '%s' and target_position ='HYDROGEN-CELL'  and subblock = 0 and measurement_type ='a' and run_number >= %i and run_number <= %i and error != 0", fMysqlDet.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
 if(type.CompareTo("runs", TString::kExact) == 0){
    dbase.fNumberColumns = 1;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select distinct(run_number) from analysis, md_slope_view where analysis.slope_calculation='%s' and analysis.slope_correction='off' and analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='%s' and run_quality_id=1 and good_for_id='1,3' and run_number >= %i and run_number <= %i", fMysqlSlopeCorr.Data(), fMysqlBeamMode.Data(), fMysqlRunLower, fMysqlRunUpper);
  }

  if(type.CompareTo("position", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select run_number, value, error, segment_number from beam_view,  slow_controls_settings where  beam_view.runlet_id=slow_controls_settings.runlet_id and target_position  = 'HYDROGEN-CELL' and slope_correction = 'off' and slope_calculation =  'off' and beam_mode = 'nbm' and good_for_id = '1,3' and run_quality_id =  '1' and measurement_type = 'd' and monitor = '%s' and slow_controls_settings.slow_helicity_plate = '%s' and subblock  = 0 and run_number >= %i and run_number <= %i order by run_number, segment_number", fMysqlMon.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("optics", TString::kExact) == 0){
    dbase.fNumberColumns = 2;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select amplitude, a_error from beam_optics_view where beam_mode = 'nbm' and slope_calculation='off' and monitor='%sX' and modulation_type='%s' and run_number=%i and segment_number = %i", fMysqlMon.Data(), fModulationType.Data(), fMysqlRunNumber, fMysqlSegNumber);
  }

//   fMysqlQuery = SetBlackList(fMysqlQuery, type);

  std::cout << "== Query:  " << fMysqlQuery << std::endl;

  return(fMysqlQuery);

}

void QwDataContainer::ReadMonitorList()
{

  std::ifstream monitor_list;
  std::string line;

  char *token;

  monitor_list.open("config/beamline.config");
  if(!monitor_list.is_open()){
    std::cout << "Error opening config file" << std::endl;
    exit(1);
  }

  while(monitor_list.good()){
    getline(monitor_list, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,.");
    while(token){
      if(strcmp("mon", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token
	
       	token = strtok(NULL, " .,"); 
	fMonitorList.push_back(token); 

// 	std::cout << "Monitor:\t" << token << std::endl;
       	token = strtok(NULL, " .,");
// 	std::cout << "BPM index:\t" << token << std::endl;
	fMonitorIndexMap.insert(std::pair<TString, Int_t>(fMonitorList.back(), atoi(token))); 
       	token = strtok(NULL, " .,");
// 	std::cout << "Z position :\t" << token << std::endl;
	fMonitorPositionMap.insert(std::pair<TString, Double_t>(fMonitorList.back(), atol(token)));
      }
      else 
	token = strtok(NULL, " .,"); 
    }
  }
}

TString QwDataContainer::SetBlackList(TString q_string, TString type)
{

  if(type.CompareTo("sens", TString::kExact) == 0){

    for(Int_t i = 0; i < (Int_t)fBlackList.size(); i++){
      q_string += Form(" && md_slope_view.run_number != %i", fBlackList[i]);
    }
    q_string += " order by md_slope_view.run_number, md_slope_view.slope";
  }
  if(type.CompareTo("sens_all", TString::kExact) == 0){

    for(Int_t i = 0; i < (Int_t)fBlackList.size(); i++){
      q_string += Form(" && md_slope_view.run_number != %i", fBlackList[i]);
    }
    q_string += " order by md_slope_view.run_number, md_slope_view.slope";
  }
  if(type.CompareTo("corrected", TString::kExact) == 0){
    
    for(Int_t i = 0; i < (Int_t)fBlackList.size(); i++){
      q_string += Form(" && md_data_view.run_number != %i", fBlackList[i]);
    }
  }
  std::cout << q_string << std::endl;
  
  return(q_string);
}

void QwDataContainer::CalculateAverage()
{

  Double_t wavg = 0;
  Double_t var = 0;
  
  fAvAsym.ResizeTo(1);
  fAvError.ResizeTo(1);
  
  for(Int_t i = 0; i < fAsym.GetNrows(); i++){
    
    wavg += (fAsym[i])/TMath::Power((fError[i]),2);
    var += TMath::Power(1/(fError[i]),2);
  }
  
  fAvAsym[0] = wavg/var;
  fAvError[0] = TMath::Sqrt(1/var);
  
  wavg = 0;
  var = 0;

//   for(Int_t i = 0; i < fRawAsym.GetNrows(); i++){
//     wavg += (fRawAsym[i])/TMath::Power((fRawError[i]),2);
//     var += TMath::Power(1/(fRawError[i]),2);
//   }
//   fAvRawAsym[0] = wavg/var;
//   fAvRawError[0] = TMath::Sqrt(1/var);

//   std::cout << "Average Raw Asymmetry:\t" << fAvRawAsym[0]
// 	    << " +- " << fAvRawError[0]
// 	    << std::endl;
}

Int_t QwDataContainer::GetMonitorIndex(TString bpm)
{
  
  return(fMonitorIndexMap.find(bpm)->second);
  
}

Double_t QwDataContainer::GetMonitorPosition(TString bpm)
{
  std::cout << "Monitor: "<< bpm << " " << fMonitorPositionMap.find(bpm)->second << std::endl;
  return(fMonitorPositionMap.find(bpm)->second);
  
}

Int_t QwDataContainer::FillDBRunList()
{
  
  TString qtemp;
  
  const char *query;
  Int_t query_result = 0;
  
  enum {run, segment};

  if(!fRunRange){
    std::cerr << "Run range not specified: Try again." << std::endl;
    exit(1);
  }
  SetRunRange(fMysqlRunLower, fMysqlRunUpper);
  
  if(!fMysqlSetStem){
    std::cerr << "Need to specify modulation set. Use --set-stem <set>" << std::endl;
    exit(1);
  }
  
  query = BuildQuery("runs").Data();
  if((query_result = QueryDB(query)) == 0){
    std::cerr << "No results returned." << std::endl;
    exit(1);
  }
  fRunNumber.ResizeTo(query_result);
  for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){
    fRunNumber[i] = (Double_t)dbase.result[i][run];
    //     fSegmentNumber[i] = (Double_t)(0.01*dbase.result[i][segment]);
  }
  
  return(query_result);
}

void QwDataContainer::FillDataVector(TString type)
{
  Int_t mod = 0;
  Int_t index = 0;
  
  Double_t fConvFactor[5] = {1.e6, 1., 1.e6, 1.e6, 1.};

  enum {run, segment, value, error, slope, ihwp};
  // enum {run, value, error, slope, ihwp};
  enum {X, XP, E, Y, YP}; 

  std::cout << "Filling data vector" << std::endl;

  if(type.Contains("sens_all", TString::kExact)){
    for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){
      if(mod > 4){
	mod = 0;
	index++;
	}
      fRunNumber[index] = (Double_t)dbase.result[i][run] + (Double_t)(0.01*dbase.result[i][segment]);
      // fRunNumber[index] = (Double_t)dbase.result[i][run];

      std::string slopewrt(dbase.result[i][slope]);
      if(slopewrt.compare("wrt_diff_targetX") == 0){
	fSensitivity[X][index] = fConvFactor[X]*dbase.result[i][value];
 	fSensitivityError[X][index] = fConvFactor[X]*dbase.result[i][error];
//  	fSensitivityError[X][index] = 0;
      }
      if(slopewrt.compare("wrt_diff_targetXSlope") == 0){
 	fSensitivity[XP][index] = fConvFactor[XP]*dbase.result[i][value];
  	fSensitivityError[XP][index] = fConvFactor[XP]*dbase.result[i][error];
// 	fSensitivityError[XP][index] = 0;
      }
      if(slopewrt.compare("wrt_diff_bpm3c12X") == 0){
 	fSensitivity[E][index] = fConvFactor[E]*dbase.result[i][value];
  	fSensitivityError[E][index] = fConvFactor[E]*dbase.result[i][error];
// 	fSensitivityError[E][index] = 0;
      }
      if(slopewrt.compare("wrt_diff_energy") == 0){
//  	fSensitivity[E][index] = dbase.result[i][value];
  	fSensitivityError[E][index] = dbase.result[i][error];
	fSensitivityError[E][index] = 0;
      }
      if(slopewrt.compare("wrt_diff_targetY") == 0){
 	fSensitivity[Y][index] = fConvFactor[Y]*dbase.result[i][value];
  	fSensitivityError[Y][index] = fConvFactor[Y]*dbase.result[i][error];
// 	fSensitivityError[Y][index] = 0;
      }
      if(slopewrt.compare("wrt_diff_targetYSlope") == 0){
 	fSensitivity[YP][index] = fConvFactor[YP]*dbase.result[i][value];
  	fSensitivityError[YP][index] = fConvFactor[YP]*dbase.result[i][error];
// 	fSensitivityError[YP][index] = 0;
      }
      mod++;
    }
  }

  if(type.CompareTo("optics", TString::kExact) == 0){
    for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){
      index = GetMonitorIndex(fMysqlMon);
      fAmplitude[index] = dbase.result[i][0];
      fAmplitudeError[index] = dbase.result[i][1];
      fMonitorPosition[index] = GetMonitorPosition(fMysqlMon);
      //      std::cout << index << " "
      //  		<< fMonitorPosition[index] << " " 
      //  		<< fAmplitude[index] << " +- "
      //  		<< fAmplitudeError[index] << std::endl;
    }
  }

  if(type.CompareTo("corrected", TString::kExact) == 0){
    for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){

      fRunNumber[i] = dbase.result[i][0];
      fAsym[i] = dbase.result[i][1];
      fError[i] =dbase.result[i][2];
      fNumberEntries[i] = dbase.result[i][3];
      fRMS[i] = fError[i]*TMath::Sqrt(fNumberEntries[i]);
    }
  }

  if(type.CompareTo("uncorrected", TString::kExact) == 0){
    std::vector <Double_t> data;
    std::vector <Double_t> error;

    Double_t wavg = 0;
    Double_t var = 0;

    Int_t nentries = 0;

    Bool_t finished = false;

    for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){

      ResizeDataElements(index + 1);	

      fRunNumber[index] = dbase.result[i][0];
      fNumberEntries[index] = nentries;
      data.push_back(1e6*dbase.result[i][1]);
      error.push_back(1e6*dbase.result[i][2]);
      nentries += (Int_t)dbase.result[i][3];

      //
      // Peek ahead to see what is coming: If starting a new run --> compute average
      //                                   If run is ending     -- > compute average
      //
      // Yeah I know this is may not the most clever way to do this but it works damn it!
      //

      if((i + 1) == (Int_t)dbase.result.num_rows()) finished = true;
      else if((Int_t)dbase.result[i + 1][4] == 0 ) finished = true;

      if(finished){
	  
	// ************************************************ //
	// Calculate Error Weighted average for run
	for(Int_t j = 0; j < (Int_t)data.size(); j++){
	  if(error[j] != 0){	  
	    wavg += (data[j])/TMath::Power((error[j]),2);
	    var += TMath::Power(1/(error[j]),2);
	  }
	}
	fAsym[index] = wavg/var;
	fError[index] = TMath::Sqrt(1/var);
	fRMS[index] = TMath::Sqrt(1/var)*TMath::Sqrt(nentries);

	wavg = 0;
	var = 0;
	
	// ************************************************ //
	data.clear();
	error.clear();
	nentries = 0;
	index++;
	finished = false;
      }
    }
       
    if(type.CompareTo("position", TString::kExact) == 0){
    }

    std::cout << "Finished filling!" << std::endl;
  }
}

void QwDataContainer::FillDataVector(Int_t mod_type)
{

  for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){
    fRunNumber[i] = dbase.result[i][0];
    fSensitivity[mod_type][i] = dbase.result[i][1];
  }
}

void QwDataContainer::PlotDBOptics()
{
  TString qtemp;

  const char *query;

  if(!fRunRange){
    std::cerr << "Must specify run range!" << std::endl;
    exit(1);
  }

  if(!fSegmentNumber){
    std::cerr << "Must specify segment number.  Use --segment <segment #>" << std::endl;
    exit(1);
  }
  if(!fModType){
    std::cerr << "Must specify modulation type.  Use --modulation-type <modtype>" 
	      << "\t Where modtype:\n"
	      << "\tx1 = X  modulation\n"
	      << "\tx2 = XP modulation\n" 
	      << "\te =  E  modulation\n" 
	      << "\ty1 = Y  modulation\n" 
	      << "\tx1 = YP modulation\n" << std::endl;
    exit(1);
  }

  std::cout << "Setting run number..." << std::endl;
  SetRunRange(fMysqlRunNumber);
  std::cout << "Reading Monitor List..." << std::endl;
  ReadMonitorList();

  fAmplitude.ResizeTo(fMonitorList.size());
  fAmplitudeError.ResizeTo(fMonitorList.size());
  fMonitorPosition.ResizeTo(fMonitorList.size());
  fZero.ResizeTo(fMonitorList.size());
  fZero.Zero();

  for(Int_t i = 0; i < (Int_t)fMonitorList.size(); i++){
    SetMonitor(fMonitorList[i]);
    qtemp = BuildQuery("optics");
    query = qtemp.Data();
    if(QueryDB(query) == 0){
      std::cerr << "No results returned." << std::endl;
      exit(1);
    }
    
    FillDataVector("optics");
    PlotBeamlineOptics();
  }
  exit(1);
}

void QwDataContainer::PlotBeamlineOptics()
{

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1150, 800);
  canvas->cd();

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *tgraph = new TGraphErrors(fMonitorPosition, fAmplitude, fZero, fAmplitudeError);
  tgraph->SetMarkerColor(4);
  tgraph->SetMarkerSize(0.8);
  tgraph->SetMarkerStyle(24);
  tgraph->SetTitle(Form("Beamline Plot: %s", fModulationType.Data()));
  tgraph->GetXaxis()->SetTitle("Z(cm) ");
  tgraph->GetYaxis()->SetTitle("Response Amplitude(mm)");
  tgraph->GetXaxis()->SetLabelSize(0.02);
  tgraph->GetYaxis()->SetLabelSize(0.02);
  tgraph->GetXaxis()->SetTitleSize(0.02);
  tgraph->GetYaxis()->SetTitleSize(0.02);
  tgraph->GetXaxis()->SetTitleOffset(1.5);
  tgraph->GetYaxis()->SetTitleOffset(1.5);
  tgraph->GetYaxis()->SetRangeUser(-0.5, 0.5);
  tgraph->Draw("AP");

  canvas->SaveAs(Form("beamline_optics_%s_%d.%03d.C", fModulationType.Data(), fMysqlRunNumber, fMysqlSegNumber));
}

void QwDataContainer::PlotDBSensitivities()
{
  TString qtemp;
  TString query_flag;

  const char *query;
  Int_t query_result = 0;

  if(!fRunRange){
    std::cerr << "Must specify run range!" << std::endl;
    exit(1);
  }

  SetRunRange(fMysqlRunLower, fMysqlRunUpper);

  if(!fMysqlSetStem){
    std::cerr << "Need to specify modulation set. Use --set-stem <set>" << std::endl;
    exit(1);
  }
  if(!fMysqlSetDet){
    std::cerr << "Must set detector. Use --detector <detector>" << std::endl;
    exit(1);
  }

  if(fMysqlDet.Contains("lumi", TString::kExact)){
    std::cout << "Building Lumi Query..." << std::endl;
    qtemp = BuildQuery("sens_all_lumi");
    query_flag = "sens_all_lumi";
  }
  else if(fMysqlDet.Contains("bcm", TString::kExact)){
    std::cout << "Building BCM Query..." << std::endl;
    qtemp = BuildQuery("sens_all_bcm");
    query_flag = "sens_all_bcm";
  }
  // else if(fMysqlDet.CompareTo("sens_all", TString::kExact)){
  else if(fMysqlDet.Contains("qwk_", TString::kExact)){
    if(fNDelta){
      std::cout << "Searching to N->Delta data..." << std::endl;
      qtemp = BuildQuery("sens_all_ndelta");
    }
    else{
      qtemp = BuildQuery("sens_all");
    }

    std::cout << "Building Detector Query..." << std::endl;
    query_flag = "sens_all";
  }
  else{
    std::cout << "Unknown query type.  Exiting." << std::endl;
    exit(1);
  }
  query = qtemp.Data();
  if((query_result = QueryDB(query)) == 0){
    std::cerr << "No results returned." << std::endl;
    exit(1);
  }

  ResizeDataElements( query_result/fNumberMonitor );
  FillDataVector(query_flag);

  std::cout << "Entering PlotSensitivities()" << std::endl;
  PlotSensitivities();

}

void QwDataContainer::GetDBSensitivities(TString detector, TString ihwp, TString option)
{
  TString qtemp;

  const char *query;

  if(!fRunRange){
    std::cout << "Must specify run range!" << std::endl;
    exit(1);
  }

  SetRunRange(fMysqlRunLower, fMysqlRunUpper);
  SetDetector(detector.Data());
  SetIHWP(ihwp.Data());
  qtemp = BuildQuery(option.Data());

  query = qtemp.Data();

  ResizeDataElements( QueryDB(query)/fNumberMonitor );
  if(!option)
    FillDataVector("sens_all");
  else
    FillDataVector(option);

  //  PlotSensitivities();
}

void QwDataContainer::GetDBAsymmetry(TString detector, TString ihwp, TString option)
{
  TString qtemp;

  const char *query;

  if(!fRunRange){
    std::cout << "Must specify run range!" << std::endl;
    exit(1);
  }

  SetRunRange(fMysqlRunLower, fMysqlRunUpper);
  SetDetector(detector.Data());
  SetIHWP(ihwp.Data());
  qtemp = BuildQuery(option.Data());

  query = qtemp.Data();

  ResizeDataElements( QueryDB(query));
  FillDataVector(option.Data());
  //  PlotSensitivities();
}

void QwDataContainer::ReadDataFile(TString filename)
{

  Int_t i = 0;
  Int_t nlines = 0;

  TString temp;

  std::fstream file;


  SetFileName(filename);
  nlines = GetFileSize(fFileName);
  ResizeDataElements(nlines);

  file.open(Form("data/%s", filename.Data()), fstream::in | fstream::out); 
  if(!file.is_open()){
    std::cout << "Error opening:\t" << filename.Data() << std::endl;
    exit(1);
  }

  while(!file.eof()){
    if(i > nlines) break;

    file >> fDetector   >> fRunNumber[i]
 	 >> fMonitor    >> fSensitivity[var_x][i]    >> fCorrection[var_x][i]
 	 >> fMonitor    >> fSensitivity[var_xp][i]   >> fCorrection[var_xp][i]
 	 >> fMonitor    >> fSensitivity[var_e][i]    >> fCorrection[var_e][i]
 	 >> fMonitor    >> fSensitivity[var_y][i]    >> fCorrection[var_y][i]
 	 >> fMonitor    >> fSensitivity[var_yp][i]   >> fCorrection[var_yp][i]
 	 >> fAsymmetry[raw][i] >> frms[raw][i]   >> fTotalCorr[i]
 	 >> temp        >> fAsymmetry[corr][i] >> frms[corr][i]; 

    fZero.Zero();
    i++;
  }
   file.close();
}

Int_t QwDataContainer::GetFileSize(TString filename)
{

  Int_t nlines = 0;
  std::string line;

  ifstream file;

  file.open(Form("data/%s", filename.Data()), fstream::in | fstream::out); 
  if(!file.is_open()){
    std::cout << "Error opening:\t" << filename.Data() << std::endl;
    exit(1);
  }

  while(!file.eof()){
    getline(file, line);
    if(line.size() == 0){
      continue;
    }
    ++nlines;
  }
  file.close();
  fNumberLines = nlines;
  return(nlines);
}

void QwDataContainer::ResizeDataElements(Int_t size)
{

  fNumberLines = size;
  fRunNumber.ResizeTo(size);
  std::cout << "Size " << size << std::endl;

  for(Int_t i = 0; i < 5; i++){
    fSensitivity[i].ResizeTo(size);
    fSensitivityError[i].ResizeTo(size);
    fCorrection[i].ResizeTo(size);
  }
  for(Int_t i = 0; i < 2; i++){
    fAsymmetry[i].ResizeTo(size);
    frms[i].ResizeTo(size);
  }
  fAsym.ResizeTo(size);
  fError.ResizeTo(size);
  fRMS.ResizeTo(size);

  fNumberEntries.ResizeTo(size);
  fTotalCorr.ResizeTo(size);
  fZero.ResizeTo(size);

}

void QwDataContainer::PlotSensitivities()
{
  TCanvas *canvas0 = new TCanvas("canvas0", "canvas0", 1200, 720);
  TF1 *line0 = new TF1("line0", "[0]", 0, 1);

  canvas0->Divide(3,2);
  canvas0->cd(1);

  gPad->SetGridx();
  gPad->SetGridy();

  gStyle->SetOptFit(1110);
  
  TGraphErrors *sensx = new TGraphErrors(fRunNumber, fSensitivity[var_x], fZero, fSensitivityError[var_x]);
  sensx->SetMarkerColor(4);
  sensx->SetMarkerSize(0.6);
  sensx->SetMarkerStyle(21);
  sensx->SetTitle(Form("X Sensitivity vs Run:%s", fMysqlSlopeCorr.Data()));
  sensx->GetXaxis()->SetTitle("Run number ");
  sensx->GetYaxis()->SetTitle("Sensitivity (per mm)");
  sensx->GetXaxis()->SetLabelSize(0.02);
  sensx->GetYaxis()->SetLabelSize(0.02);
  sensx->GetXaxis()->SetTitleSize(0.02);
  sensx->GetYaxis()->SetTitleSize(0.02);
  sensx->GetXaxis()->SetTitleOffset(1.5);
  sensx->GetYaxis()->SetTitleOffset(1.8);
//   ScaleTGraph(sensx);
  sensx->Draw("AP");
  sensx->Fit("line0", "");

  canvas0->cd(2);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sensxp = new TGraphErrors(fRunNumber, fSensitivity[var_xp], fZero, fSensitivityError[var_xp]);

  sensxp->SetMarkerColor(5);
  sensxp->SetMarkerSize(0.6);
  sensxp->SetMarkerStyle(21);
  sensxp->SetTitle(Form("XP Sensitivities vs Run:%s", fMysqlSlopeCorr.Data()));
  sensxp->GetXaxis()->SetTitle("Run number ");
  sensxp->GetYaxis()->SetTitle("Sensitivity (per urad)");
  sensxp->GetXaxis()->SetLabelSize(0.02);
  sensxp->GetYaxis()->SetLabelSize(0.02);
  sensxp->GetXaxis()->SetTitleSize(0.02);
  sensxp->GetYaxis()->SetTitleSize(0.02);
  sensxp->GetXaxis()->SetTitleOffset(1.5);
  sensxp->GetYaxis()->SetTitleOffset(1.8);
//   ScaleTGraph(sensxp);
  sensxp->Draw("AP");
  sensxp->Fit("line0", "");

  canvas0->cd(3);

  gPad->SetGridx();
  gPad->SetGridy();

//   TGraphErrors *sense = new TGraphErrors(fRunNumber, 0.0041*fSensitivity[var_e], fZero, 0.0041*fSensitivityError[var_e]);
  TGraphErrors *sense = new TGraphErrors(fRunNumber, fSensitivity[var_e], fZero, fSensitivityError[var_e]);

  sense->SetMarkerColor(6);
  sense->SetMarkerSize(0.6);
  sense->SetMarkerStyle(21);
  sense->SetTitle(Form("E(bpm3c12X) Sensitivities vs Run:%s", fMysqlSlopeCorr.Data()));
  sense->GetXaxis()->SetTitle("Run number ");
  sense->GetYaxis()->SetTitle("Sensitivity (ppm/mm)");
  sense->GetXaxis()->SetLabelSize(0.02);
  sense->GetYaxis()->SetLabelSize(0.02);
  sense->GetXaxis()->SetTitleSize(0.02);
  sense->GetYaxis()->SetTitleSize(0.02);
  sense->GetXaxis()->SetTitleOffset(1.5);
  sense->GetYaxis()->SetTitleOffset(1.8);
//   ScaleTGraph(sense);
//   sense->GetYaxis()->SetRangeUser(-10., 0.);
  sense->Draw("AP");
  sense->Fit("line0", "");

  canvas0->cd(4);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sensy = new TGraphErrors(fRunNumber, fSensitivity[var_y], fZero, fSensitivityError[var_y]);

  sensy->SetMarkerColor(7);
  sensy->SetMarkerSize(0.6);
  sensy->SetMarkerStyle(21);
  sensy->SetTitle(Form("Y Sensitivities vs Run:%s", fMysqlSlopeCorr.Data()));
  sensy->GetXaxis()->SetTitle("Run number ");
  sensy->GetYaxis()->SetTitle("Sensitivity (per mm)");
  sensy->GetXaxis()->SetLabelSize(0.02);
  sensy->GetYaxis()->SetLabelSize(0.02);
  sensy->GetXaxis()->SetTitleSize(0.02);
  sensy->GetYaxis()->SetTitleSize(0.02);
  sensy->GetXaxis()->SetTitleOffset(1.5);
  sensy->GetYaxis()->SetTitleOffset(1.8);
//   ScaleTGraph(sensy);
//   sensy->GetYaxis()->SetRangeUser(-3000., 3000.);
  sensy->Draw("AP");
  sensy->Fit("line0", "");

  canvas0->cd(5);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sensyp = new TGraphErrors(fRunNumber, fSensitivity[var_yp], fZero, fSensitivityError[var_yp]);

  sensyp->SetMarkerColor(8);
  sensyp->SetMarkerSize(0.6);
  sensyp->SetMarkerStyle(21);
  sensyp->SetTitle(Form("YP Sensitivities vs Run:%s", fMysqlSlopeCorr.Data()));
  sensyp->GetXaxis()->SetTitle("Run number ");
  sensyp->GetYaxis()->SetTitle("Sensitivity (per urad)");
  sensyp->GetXaxis()->SetLabelSize(0.02);
  sensyp->GetYaxis()->SetLabelSize(0.02);
  sensyp->GetXaxis()->SetTitleSize(0.02);
  sensyp->GetYaxis()->SetTitleSize(0.02);
  sensyp->GetXaxis()->SetTitleOffset(1.5);
  sensyp->GetYaxis()->SetTitleOffset(1.8);
//   ScaleTGraph(sensyp);
//   sensyp->GetYaxis()->SetRangeUser(-200., 200.);
  sensyp->Draw("AP");
  sensyp->Fit("line0", "");

  canvas0->SaveAs(Form("%s_sensitivities_%s.pdf", fDetector.Data(), fMysqlSlopeCorr.Data()));
  canvas0->SaveAs(Form("%s_sensitivities_%s.C", fDetector.Data(), fMysqlSlopeCorr.Data()));
  canvas0->SaveAs(Form("%s_sensitivities_%s.png", fDetector.Data(), fMysqlSlopeCorr.Data()));

  //  delete line0;
  //  delete canvas0;
}

void QwDataContainer::PlotCorrections()
{


  TCanvas *canvas1 = new TCanvas("canvas1", "canvas1", 1200, 800);
  TF1 *line0 = new TF1("line0", "[0]", 0, 1);

  canvas1->Divide(3,2);
  canvas1->cd(1);

  gStyle->SetOptFit(1110);

  gPad->SetGridx();
  gPad->SetGridy();
  
  TGraphErrors *corrx = new TGraphErrors(fRunNumber, fCorrection[var_x], fZero, fZero);

  corrx->SetMarkerColor(4);
  corrx->SetMarkerSize(0.8);
  corrx->SetMarkerStyle(21);
  corrx->SetTitle("X Corrections versus Run");
  corrx->GetXaxis()->SetTitle("Run number ");
  corrx->GetYaxis()->SetTitle("Correction (ppm)");
  corrx->GetXaxis()->SetLabelSize(0.02);
  corrx->GetYaxis()->SetLabelSize(0.02);
  corrx->GetXaxis()->SetTitleSize(0.02);
  corrx->GetYaxis()->SetTitleSize(0.02);
  corrx->GetXaxis()->SetTitleOffset(1.5);
  corrx->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(corrx);
  corrx->GetYaxis()->SetRangeUser(-0.5, 0.5);
  corrx->Draw("AP");
  corrx->Fit("line0","");

  canvas1->cd(2);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *corrxp = new TGraphErrors(fRunNumber, fCorrection[var_xp], fZero, fZero);

  corrxp->SetMarkerColor(5);
  corrxp->SetMarkerSize(0.8);
  corrxp->SetMarkerStyle(21);
  corrxp->SetTitle("XP Corrections versus Run");
  corrxp->GetXaxis()->SetTitle("Run number ");
  corrxp->GetYaxis()->SetTitle("Correction (ppm)");
  corrxp->GetXaxis()->SetLabelSize(0.02);
  corrxp->GetYaxis()->SetLabelSize(0.02);
  corrxp->GetXaxis()->SetTitleSize(0.02);
  corrxp->GetYaxis()->SetTitleSize(0.02);
  corrxp->GetXaxis()->SetTitleOffset(1.5);
  corrxp->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(corrxp);
  corrxp->GetYaxis()->SetRangeUser(-0.5, 0.5);
  corrxp->Draw("AP");
  corrxp->Fit("line0","");

  canvas1->cd(3);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *corre = new TGraphErrors(fRunNumber, fCorrection[var_e], fZero, fZero);

  corre->SetMarkerColor(6);
  corre->SetMarkerSize(0.8);
  corre->SetMarkerStyle(21);
  corre->SetTitle("E Corrections versus Run");
  corre->GetXaxis()->SetTitle("Run number ");
  corre->GetYaxis()->SetTitle("Correction (ppm)");
  corre->GetXaxis()->SetLabelSize(0.02);
  corre->GetYaxis()->SetLabelSize(0.02);
  corre->GetXaxis()->SetTitleSize(0.02);
  corre->GetYaxis()->SetTitleSize(0.02);
  corre->GetXaxis()->SetTitleOffset(1.5);
  corre->GetYaxis()->SetTitleOffset(1.5);
  corre->GetYaxis()->SetRangeUser(-0.5, 0.5);
  corre->Draw("AP");
  corre->Fit("line0","");

  canvas1->cd(4);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *corry = new TGraphErrors(fRunNumber, fCorrection[var_y], fZero, fZero);

  corry->SetMarkerColor(7);
  corry->SetMarkerSize(0.8);
  corry->SetMarkerStyle(21);
  corry->SetTitle("Y Corrections versus Run");
  corry->GetXaxis()->SetTitle("Run number ");
  corry->GetYaxis()->SetTitle("Correction (ppm)");
  corry->GetXaxis()->SetLabelSize(0.02);
  corry->GetYaxis()->SetLabelSize(0.02);
  corry->GetXaxis()->SetTitleSize(0.02);
  corry->GetYaxis()->SetTitleSize(0.02);
  corry->GetXaxis()->SetTitleOffset(1.5);
  corry->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(corry);
  corry->GetYaxis()->SetRangeUser(-0.5, 0.5);
  corry->Draw("AP");
  corry->Fit("line0","");

  canvas1->cd(5);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *corryp = new TGraphErrors(fRunNumber, fCorrection[var_yp], fZero, fZero);

  corryp->SetMarkerColor(8);
  corryp->SetMarkerSize(0.8);
  corryp->SetMarkerStyle(21);
  corryp->SetTitle("YP Corrections versus Run");
  corryp->GetXaxis()->SetTitle("Run number ");
  corryp->GetYaxis()->SetTitle("Correction (ppm)");
  corryp->GetXaxis()->SetLabelSize(0.02);
  corryp->GetYaxis()->SetLabelSize(0.02);
  corryp->GetXaxis()->SetTitleSize(0.02);
  corryp->GetYaxis()->SetTitleSize(0.02);
  corryp->GetXaxis()->SetTitleOffset(1.5);
  corryp->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(corryp);
  corryp->GetYaxis()->SetRangeUser(-0.5, 0.5);
  corryp->Draw("AP");
  corryp->Fit("line0","");

  canvas1->cd(6);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *corrtot = new TGraphErrors(fRunNumber, fTotalCorr, fZero, fZero);

  corrtot->SetMarkerColor(9);
  corrtot->SetMarkerSize(0.8);
  corrtot->SetMarkerStyle(21);
  corrtot->SetTitle("Total Corrections versus Run");
  corrtot->GetXaxis()->SetTitle("Run number ");
  corrtot->GetYaxis()->SetTitle("Correction (ppm)");
  corrtot->GetXaxis()->SetLabelSize(0.02);
  corrtot->GetYaxis()->SetLabelSize(0.02);
  corrtot->GetXaxis()->SetTitleSize(0.02);
  corrtot->GetYaxis()->SetTitleSize(0.02);
  corrtot->GetXaxis()->SetTitleOffset(1.5);
  corrtot->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(corrtot);
  corrtot->GetYaxis()->SetRangeUser(-0.5, 0.5);
  corrtot->Draw("AP");
  corrtot->Fit("line0","");

  canvas1->SaveAs(Form("%s_corrections.pdf", fDetector.Data()));

  delete canvas1;
  delete line0;

}

void QwDataContainer::PlotDetectorRMS()
{


  TF1 *line0 = new TF1("line0", "[0]", 0, 1);
  TF1 *line1 = new TF1("line1", "[0]", 0, 1);

  TCanvas *canvas2 = new TCanvas("canvas2", "canvas2", 1200, 800);

  canvas2->cd(1);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *raw_rms = new TGraphErrors(fRunNumber, frms[raw], fZero, fZero);

  raw_rms->SetMarkerColor(4);
  raw_rms->SetMarkerSize(0.8);
  raw_rms->SetMarkerStyle(21);
  raw_rms->SetTitle("Raw RMS versus Run");
  raw_rms->GetXaxis()->SetTitle("Run number");
  raw_rms->GetYaxis()->SetTitle("RMS (ppm)");
  raw_rms->Fit("line0","N");

  Double_t max = GetMaximum(raw_rms->GetY());
  Double_t min = GetMinimum(raw_rms->GetY());

  TGraphErrors *corr_rms   = new TGraphErrors(fRunNumber, frms[corr], fZero, fZero);

  corr_rms->SetMarkerColor(5);
  corr_rms->SetMarkerSize(0.8);
  corr_rms->SetMarkerStyle(21);
  corr_rms->SetTitle("Corrected RMS versus Run");
  corr_rms->GetXaxis()->SetTitle("Run number ");
  corr_rms->GetYaxis()->SetTitle("RMS (ppm)");
  corr_rms->Fit("line1","N");

  if( GetMaximum(corr_rms->GetY()) > max) max = GetMaximum(corr_rms->GetY());
  if( GetMaximum(corr_rms->GetY()) < min) max = GetMinimum(corr_rms->GetY());

  TMultiGraph *mgraph = new TMultiGraph();

  mgraph->Add(raw_rms);
  mgraph->Add(corr_rms);

  mgraph->Draw("AP");
  mgraph->GetYaxis()->SetRangeUser(min*0.9, max*1.2);

  canvas2->BuildLegend( 0.75, 0.85, 0.99, 0.99, Form("Detector: %s", fDetector.Data()));

  TLegend* legend = new TLegend(0.1, 0.75, .45, .99);

  legend->AddEntry((TObject *)0, Form("Raw RMS: %3.3f +- %3.3f", line0->GetParameter(0), line0->GetParError(0)), "");
  legend->AddEntry((TObject *)0, Form("Corrected RMS: %3.3f +- %3.3f", line1->GetParameter(0), line1->GetParError(0)), "");
  legend->Draw();

  canvas2->SaveAs(Form("%s_RMS.pdf", fDetector.Data()));

  delete legend;
  delete canvas2;
  delete line1;
  delete line0;


}
#endif
