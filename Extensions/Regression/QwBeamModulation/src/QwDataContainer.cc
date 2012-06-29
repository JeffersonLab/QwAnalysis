#define QwDataContainer_cxx
#include "../include/QwDataContainer.hh"
#include "../include/headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TMultiGraph.h"
#include "TLegend.h"

#ifdef QwDataContainer_cxx

QwDataContainer::QwDataContainer():fDBase(false), fRunRange(false), fNumberMonitor(1) 
{

  // Establish a connection to the database through QwDBConnection

  dbase.Connect();

  // Initialize Data arrays

  fSensitivity.resize(5);
  fCorrection.resize(5);
  fAsymmetry.resize(5);
  fPositionDiff.resize(5);
  frms.resize(5);

  // There needs to be a conversion factor between the number read in for the ascii db and the qweakdb

  GetBlackList();

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

  while(options[i] != NULL){
    fOptions.push_back(options[i]);
    std::string option (fOptions[i].Data());
    
    if(fOptions[i].CompareTo("--database", TString::kExact) == 0) fDBase = true;

    if(option.compare(0, 6, "--runs") == 0){
      fOptions.push_back(options[i + 1]);
      std::string option (fOptions[i + 1].Data());
      index = option.find_first_of(":", 1);
      fMysqlRunLower = atoi(option.substr(index - 5, 5).c_str());
      std::cout << "Index:\t" << index << std::endl;
      //      fMysqlRunLower = atoi(option.substr(0, index).c_str());
      fMysqlRunUpper = atoi(option.substr(index + 1, 5).c_str());
      fRunRange = true;
    }

    if(option.compare(0, 10, "--detector") == 0){
      fOptions.push_back(options[i + 1]);
      SetDetector(fOptions.back());
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
  fMysqlDet = detector;
  fDetector = detector;
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

void QwDataContainer::SetRunRange(Int_t lower, Int_t upper)
{
  fMysqlRunLower = lower;
  fMysqlRunUpper = upper;
}

TString QwDataContainer::BuildQuery(TString type)
{


  if(type.CompareTo("sens", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select md_slope_view.run_number ,value,error,slope from analysis,md_slope_view, slow_controls_settings, runlet as runlet0 where md_slope_view.run_number=runlet0.run_number && runlet0.segment_number=0 && analysis.analysis_id=md_slope_view.analysis_id and analysis .beam_mode='cp' and analysis.slope_calculation='off' and analysis.slope_correction='on' and md_slope_view.run_quality_id=1 and md_slope_view.good_for_id='1,3' and detector = '%s' and runlet0.runlet_id=slow_controls_settings.runlet_id && slow_controls_settings.slow_helicity_plate = '%s' md_slope_view.run_number > %i && md_slope_view.run_number < %i ", fMysqlDet.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("sens_all", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 5;
    fMysqlQuery = Form("select md_slope_view.run_number ,value,error,slope from analysis,md_slope_view, slow_controls_settings, runlet as runlet0 where md_slope_view.run_number=runlet0.run_number && runlet0.segment_number=0 && analysis.analysis_id=md_slope_view.analysis_id and analysis .beam_mode='cp' and analysis.slope_calculation='off' and analysis.slope_correction='on' and md_slope_view.run_quality_id=1 and md_slope_view.good_for_id='1,3' and detector = '%s' and runlet0.runlet_id=slow_controls_settings.runlet_id and md_slope_view.run_number > %i && md_slope_view.run_number < %i", fMysqlDet.Data() , fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("corrected", TString::kExact) == 0){
    dbase.fNumberColumns = 1;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select md_data_view.run_number,value,error,n from analysis,md_data_view, slow_controls_settings, runlet as runlet0 where md_data_view.run_number=runlet0.run_number && runlet0.segment_number=0 && analysis.analysis_id=md_data_view.analysis_id and analysis.beam_mode='cp' and analysis.slope_calculation='off' and analysis.slope_correction='on' and md_data_view.run_quality_id=1 and md_data_view.good_for_id='1,3' and detector = '%s' and runlet0.runlet_id=slow_controls_settings.runlet_id && slow_controls_settings.slow_helicity_plate = '%s' and md_data_view.run_number > %i && md_data_view.run_number < %i and error != 0", fMysqlDet.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("uncorrected", TString::kExact) == 0){
    dbase.fNumberColumns = 1;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select run_number, value, error, n, segment_number from md_data_view, slow_controls_settings where md_data_view.runlet_id = slow_controls_settings.runlet_id and good_for_id = '1,3' and run_quality_id = '1' and beam_mode = 'nbm' and slope_calculation = 'off' and slope_correction ='off' and detector = '%s' and slow_helicity_plate = '%s' and target_position ='HYDROGEN-CELL'  and subblock = 0 and measurement_type ='a' and run_number > %i and run_number < %i and error != 0", fMysqlDet.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);
  }
  if(type.CompareTo("runs", TString::kExact) == 0){
    dbase.fNumberColumns = 1;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select distinct(run_number)from analysis,md_slope_view, slow_controls_settings where analysis.analysis_id=md_slope_view.analysis_id and analysis.beam_mode='cp' and analysis.slope_calculation='off' and analysis.slope_correction='on' and run_quality_id=1 and good_for_id='1,3' and slow_controls_settings.slow_helicity_plate = '%s' and run_number > %i and run_number < %i", fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);

  }

  if(type.CompareTo("position", TString::kExact) == 0){
    dbase.fNumberColumns = 4;
    fNumberMonitor = 1;
    fMysqlQuery = Form("select run_number, value, error, segment_number from beam_view,  slow_controls_settings where  beam_view.runlet_id=slow_controls_settings.runlet_id and target_position  = 'HYDROGEN-CELL' and slope_correction = 'off' and slope_calculation =  'off' and beam_mode = 'nbm' and good_for_id = '1,3' and run_quality_id =  '1' and measurement_type = 'd' and monitor = '%s' and slow_controls_settings.slow_helicity_plate = '%s' and subblock  = 0 and run_number > %i and run_number < %i order by run_number, segment_number", fMysqlMon.Data(), fMysqlIHWP.Data(), fMysqlRunLower, fMysqlRunUpper);

  }

  fMysqlQuery = SetBlackList(fMysqlQuery, type);

  //  std::cout << "Query:\t" << fMysqlQuery << std::endl;

  return(fMysqlQuery);

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

void QwDataContainer::FillDataVector(TString type)
{
  Int_t mod = 0;
  Int_t index = 0;
  //  Int_t key[] = {2, 0, 1, 3, 4};

  
//   Double_t fConvFactor[5] = {1.e6, 1., 1.e6, 1.e6, 1.};
  Double_t fConvFactor[5] = {1.e6, 1.e6, 1., 1.e6, 1.};

  std::cout << "Filling data vector" << std::endl;

  if(type.CompareTo("sens_all", TString::kExact) == 0){
    for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){
      if(mod > 4){
	mod = 0;
	index++;
	}
      fRunNumber[index] = dbase.result[i][0];
      fSensitivity[mod][index] = fConvFactor[mod]*dbase.result[i][1];
//       std::cout << "run#:\t"   << dbase.result[i][0] 
// 		<< "\tsens:\t" << dbase.result[i][1] 
// 		<< "\t" << mod << "\t" << i << std::endl;
      mod++;
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
    std::cout << "Finished filling!" << std::endl;
    
    if(type.CompareTo("position", TString::kExact) == 0){



    }


  }
}

void QwDataContainer::FillDataVector(Int_t mod_type)
{

  for(Int_t i = 0; i < (Int_t)dbase.result.num_rows(); i++){
    fRunNumber[i] = dbase.result[i][0];
    fSensitivity[mod_type][i] = dbase.result[i][1];
  }
}

void QwDataContainer::PlotDBSensitivities()
{
  TString qtemp;

  const char *query;

  if(!fRunRange){
    std::cout << "Must specify run range!" << std::endl;
    exit(1);
  }

  SetRunRange(fMysqlRunLower, fMysqlRunUpper);
  SetDetector("qwk_mdallbars");
  SetIHWP("in");
  qtemp = BuildQuery("sens_all");

  query = qtemp.Data();

  ResizeDataElements( QueryDB(query)/fNumberMonitor );
  FillDataVector("sens_all");
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
  FillDataVector("sens_all");
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
//   std::cout << "Size " << size << std::endl;

  for(Int_t i = 0; i < 5; i++){
    fSensitivity[i].ResizeTo(size);
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

  TF1 *line0 = new TF1("line0", "[0]", 0, 1);

  TCanvas *canvas0 = new TCanvas("canvas0", "canvas0", 1200, 800);

  canvas0->Divide(3,2);
  canvas0->cd(1);

  gPad->SetGridx();
  gPad->SetGridy();

  gStyle->SetOptFit(1110);
  
  TGraphErrors *sensx = new TGraphErrors(fRunNumber, fSensitivity[var_x], fZero, fZero);

  sensx->SetMarkerColor(4);
  sensx->SetMarkerSize(0.8);
  sensx->SetMarkerStyle(21);
  sensx->SetTitle("X Sensitivities versus Run");
  sensx->GetXaxis()->SetTitle("Run number ");
  sensx->GetYaxis()->SetTitle("Sensitivity (per mm)");
  sensx->GetXaxis()->SetLabelSize(0.02);
  sensx->GetYaxis()->SetLabelSize(0.02);
  sensx->GetXaxis()->SetTitleSize(0.02);
  sensx->GetYaxis()->SetTitleSize(0.02);
  sensx->GetXaxis()->SetTitleOffset(1.5);
  sensx->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(sensx);
  sensx->Draw("AP");
  sensx->Fit("line0", "");

  canvas0->cd(2);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sensxp = new TGraphErrors(fRunNumber, fSensitivity[var_xp], fZero, fZero);

  sensxp->SetMarkerColor(5);
  sensxp->SetMarkerSize(0.8);
  sensxp->SetMarkerStyle(21);
  sensxp->SetTitle("XP Sensitivities versus Run");
  sensxp->GetXaxis()->SetTitle("Run number ");
  sensxp->GetYaxis()->SetTitle("Sensitivity (per urad)");
  sensxp->GetXaxis()->SetLabelSize(0.02);
  sensxp->GetYaxis()->SetLabelSize(0.02);
  sensxp->GetXaxis()->SetTitleSize(0.02);
  sensxp->GetYaxis()->SetTitleSize(0.02);
  sensxp->GetXaxis()->SetTitleOffset(1.5);
  sensxp->GetYaxis()->SetTitleOffset(1.5);
  ScaleTGraph(sensxp);
  sensxp->Draw("AP");
  sensxp->Fit("line0", "");

  canvas0->cd(3);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sense = new TGraphErrors(fRunNumber, 0.0041*fSensitivity[var_e], fZero, fZero);

  sense->SetMarkerColor(6);
  sense->SetMarkerSize(0.8);
  sense->SetMarkerStyle(21);
  sense->SetTitle("E Sensitivities versus Run");
  sense->GetXaxis()->SetTitle("Run number ");
  sense->GetYaxis()->SetTitle("Sensitivity (per mm)");
  sense->GetXaxis()->SetLabelSize(0.02);
  sense->GetYaxis()->SetLabelSize(0.02);
  sense->GetXaxis()->SetTitleSize(0.02);
  sense->GetYaxis()->SetTitleSize(0.02);
  sense->GetXaxis()->SetTitleOffset(1.5);
  sense->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(sense);
  sense->GetYaxis()->SetRangeUser(-10., 0.);
  sense->Draw("AP");
  sense->Fit("line0", "");

  canvas0->cd(4);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sensy = new TGraphErrors(fRunNumber, fSensitivity[var_y], fZero, fZero);

  sensy->SetMarkerColor(7);
  sensy->SetMarkerSize(0.8);
  sensy->SetMarkerStyle(21);
  sensy->SetTitle("Y Sensitivities versus Run");
  sensy->GetXaxis()->SetTitle("Run number ");
  sensy->GetYaxis()->SetTitle("Sensitivity (per mm)");
  sensy->GetXaxis()->SetLabelSize(0.02);
  sensy->GetYaxis()->SetLabelSize(0.02);
  sensy->GetXaxis()->SetTitleSize(0.02);
  sensy->GetYaxis()->SetTitleSize(0.02);
  sensy->GetXaxis()->SetTitleOffset(1.5);
  sensy->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(sensy);
  sensy->GetYaxis()->SetRangeUser(-3000., 3000.);
  sensy->Draw("AP");
  sensy->Fit("line0", "");

  canvas0->cd(5);

  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *sensyp = new TGraphErrors(fRunNumber, fSensitivity[var_yp], fZero, fZero);

  sensyp->SetMarkerColor(8);
  sensyp->SetMarkerSize(0.8);
  sensyp->SetMarkerStyle(21);
  sensyp->SetTitle("YP Sensitivities versus Run");
  sensyp->GetXaxis()->SetTitle("Run number ");
  sensyp->GetYaxis()->SetTitle("Sensitivity (per urad)");
  sensyp->GetXaxis()->SetLabelSize(0.02);
  sensyp->GetYaxis()->SetLabelSize(0.02);
  sensyp->GetXaxis()->SetTitleSize(0.02);
  sensyp->GetYaxis()->SetTitleSize(0.02);
  sensyp->GetXaxis()->SetTitleOffset(1.5);
  sensyp->GetYaxis()->SetTitleOffset(1.5);
  //  ScaleTGraph(sensyp);
  sensyp->GetYaxis()->SetRangeUser(-200., 200.);
  sensyp->Draw("AP");
  sensyp->Fit("line0", "");

  canvas0->SaveAs(Form("%s_sensitivities.pdf", fDetector.Data()));

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
