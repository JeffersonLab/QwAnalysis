#define QwMpsOnly_cxx
#include "QwMpsOnly.hh"
#include "headers.h"
#include <TLeaf.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#ifdef QwMpsOnly_cxx

const char QwMpsOnly::red[8] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
const char QwMpsOnly::other[8] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
const char QwMpsOnly::normal[8] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

const Double_t fUnitConvert[5] = {1., 1.e6, 1., 1., 1.e6};

QwMpsOnly::QwMpsOnly(TChain *tree)
{
  
  fXModulation  = 0;
  fYModulation  = 3;
  fEModulation  = 2;
  fXPModulation = 1; 
  fYPModulation = 4;
  fNEvents = 0;
  fReduceMatrix_x  = 0;
  fReduceMatrix_y  = 0;
  fReduceMatrix_xp = 0;
  fReduceMatrix_yp = 0; 
  fReduceMatrix_e  = 0; 
  fSensHumanReadable = 0;
  fDegPerEntry = 0;
  fNModType = 5; 
  fPedestal = 0; 
  fNModEvents = 0; 
  fCurrentCut = 40; 
  fPreviousRampValue = -1.;
  fMaxRampNonLinearity = 3.;
  fXinit  = false; 
  fYinit  = false; 
  fEinit  = false; 
  fXPinit = false; 
  fYPinit = false; 
  fSingleCoil = false; 
  fRunNumberSet = false; 
  fPhaseConfig = false;
  
  f2DFit = true;

  fFileSegment = ""; 
  fFileStem = "mps_only"; 
  fSetStem = "std"; 

  fNewEbpm = 0;

  Init(tree);
}

QwMpsOnly::~QwMpsOnly()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

void QwMpsOnly::BuildDetectorData()
{
  for(Int_t i = 0; i < fNDetector; i++)
    DetectorData.push_back(std::vector <Double_t>());
  std::cout << "Detector block size: " << DetectorData.size() << std::endl;
  return;
}

void QwMpsOnly::BuildDetectorAvSlope()
{
  for(Int_t i = 0; i < fNModType; i++){
    AvDetectorSlope.push_back(std::vector <Double_t>());
    AvDetectorSlopeError.push_back(std::vector <Double_t>());
  }
  std::cout << "Average Detector vector size: " << AvDetectorSlope.size() << std::endl;
  return;
}

void QwMpsOnly::BuildCoilData()
 {
   for(Int_t i = 0; i < fNModType; i++)
     CoilData.push_back(std::vector <Double_t>());
   std::cout << "Coil block size: " << CoilData.size() << std::endl;
   
   return;
 }

void QwMpsOnly::BuildDetectorSlopeVector()
{
  // First 5 hold Sine components for each Mod Type
  // modType+5 holds Cosine Component 
  DetectorSlope.resize(10);
  DetectorSlopeError.resize(10);
  for(Int_t i = 0; i < 10; i++){
    DetectorSlope[i].resize(fNDetector);
    DetectorSlopeError[i].resize(fNDetector);
  }
  return;
}

void QwMpsOnly::BuildMonitorAvSlope()
{
  for(Int_t i = 0; i < fNModType; i++){
    AvMonitorSlope.push_back(std::vector <Double_t>());
    AvMonitorSlopeError.push_back(std::vector <Double_t>());
  }
  std::cout << "Average Monitor vector size: " << AvMonitorSlope.size() << std::endl;
  return;
}

void QwMpsOnly::BuildMonitorData()
{

  for(Int_t i = 0; i < fNMonitor; i++)
    MonitorData.push_back(std::vector <Double_t>());

  std::cout << "Monitor block size: " << MonitorData.size() << std::endl;

  return;
}

void QwMpsOnly::BuildMonitorSlopeVector()
{
  // First 5 hold Sine components for each Mod Type
  // modType+5 holds Cosine Component
  MonitorSlope.resize(10);
  MonitorSlopeError.resize(10);
  for(Int_t i = 0; i < 10; i++){
    MonitorSlope[i].resize(fNMonitor);
    MonitorSlopeError[i].resize(fNMonitor);
  }
  return;
}

void QwMpsOnly::BuildNewEBPM(){
  
  //create a new friendable tree
  TFile *newfile = new TFile(Form("%s/mps_only_newEbpm_%i.root",
				  gSystem->Getenv("MPS_ONLY_ROOTFILES"),
				  run_number),"recreate");
  TTree *newTree = new TTree("mps_slug", "mps_slug"); 

  Double_t newEnergyBPM=0;
  b_newEbpm = newTree->Branch("newEbpm", &newEnergyBPM, "newEbpm/D");

  Double_t alpha[] = {1.47521,-57239.6,1,-0.0423589,-4131.94}; // X,XP,E,Y,YP in setup.config
  
  for(Int_t i=0;i<fChain->GetEntries();i++){

    newEnergyBPM=0;
    fChain->GetEntry(i);

    // construct new energy bpm
    for(int inew=0;inew<5;inew++){

      newEnergyBPM += alpha[inew]*(fChain->GetLeaf(MonitorList[inew].Data())->GetValue());

      // if(i%100==0) 
      // 	std::cout << alpha[inew] <<"*"<< fChain->GetLeaf(MonitorList[inew].Data())->GetValue() << " + " ;
    }
    //    if(i%100==0) std::cout << " = "<< newEnergyBPM <<"\n"<< std::endl;

    newTree->Fill();
    if(i%10000==0)std::cout<<"Processing entry "<<i<<".\n";
  }
  newTree->Print();
  newfile->Write("",TObject::kOverwrite);
  //  newfile->Close();
}

void QwMpsOnly::Calculate2DSlope(Int_t fNModType, Int_t makePlots)
{
  Double_t sigma_cc = 0;
  Double_t sigma_dc = 0;
  Double_t d_mean = 0;
  Double_t c_mean = 0;
  Double_t slope = 0;
  Double_t sigma_cc2 = 0;
  Double_t sigma_dc2 = 0;
  Double_t c_mean2 = 0;
  Double_t slope2 = 0;
  
  if(!fPhaseConfig){
    Double_t temp[5]={0.26, 0.26, 0.0, 1.08, 1.08};              
    SetPhaseValues(temp); 
  }

  if(fNEvents < 3){
    std::cout << red <<"Error in run:: Number of good events too small, exiting." 
	      << normal << std::endl;
    return;
  }
  
  if(CoilData[fNModType].size() <= 0){
    std::cout << "!!!!!!!!!!!!!!!!! Illegal Coil vector length:\t" 
	      << CoilData[fNModType].size() << std::endl;
    return;
  }

  //*******************************
  ModulationEvents[fNModType] += fNEvents;
  //*******************************
  
  // 2D Sin+Cos+Const fit.
  TF1* fctn = new TF1("fctn", Form("[0] + [1] * TMath::Sin( %e * x) + "
				   "[2] * TMath::Cos(%e * x)", kDegToRad, 
				   kDegToRad), 0.0, 360.0); 
  fctn->SetParNames("Const.", "A_s", "A_c");
  fctn->SetParameters(0.0, 1.0, -0.1);
  fctn->SetLineColor(kBlack);
  
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);
  
  // Arrays for all fits. 
  Double_t x[fNEvents];				// Dependent
  Double_t y[fNEvents];				// Independent
  
  c_mean=0;
  c_mean2=0;
  for(Int_t evNum=0; evNum<fNEvents; evNum++) { // Dependent is always ramp
    x[evNum] = CoilData[fNModType][evNum] + phase[fNModType]/kDegToRad;
    if(x[evNum] >= 360.0)
      x[evNum] -= 360.0;
    c_mean += TMath::Sin(kDegToRad*x[evNum]);	
    c_mean2 += TMath::Cos(kDegToRad*x[evNum]);	
  }
  c_mean /= fNEvents;
  c_mean2 /= fNEvents;
  
  // Variables to hold fit parameters.
  Double_t Const = 0;
  //Double_t ConstError = 0;
  Double_t sinAmp = 0;
  Double_t sinAmpError = 0; 
  Double_t cosAmp = 0;
  Double_t cosAmpError = 0;  

  char  *ModName[fNModType];
  ModName[fXModulation] = "X";
  ModName[fXPModulation] = "XP";
  ModName[fEModulation] = "E";
  ModName[fYModulation] = "Y";
  ModName[fYPModulation] = "YP"; 
     
  //////////////
  // Monitors //
  //////////////
  TCanvas *cM;
  if(makePlots){
    cM = new TCanvas("cM", "canvas", 1200, 800);
    cM->Divide(3,2);
  }
  
  TGraph* mongr[fNMonitor];
  
  for(Int_t mon=0; mon<fNMonitor; mon++) {
    if(MonitorData[mon].size() <= 0){
      std::cout << "!!!!!!!!!!!!!!!!! Illegal Monitor vector length:\t" 
		<< MonitorData[mon].size() << std::endl;
      return;
    }
    
    sigma_cc=0;
    sigma_dc=0;
    sigma_cc2=0;
    sigma_dc2=0;
    d_mean =0;

    for(Int_t evNum=0; evNum<fNEvents; evNum++) {
      y[evNum] = MonitorData[mon][evNum];
      d_mean += y[evNum];      
    }
    d_mean /= fNEvents;
    
    for(Int_t evNum=0; evNum<fNEvents; evNum++) {
      Double_t val1 = TMath::Sin(kDegToRad*x[evNum]) - c_mean;
      Double_t val2 = y[evNum] - d_mean;

      Double_t val1c = TMath::Cos(kDegToRad*x[evNum]) - c_mean2;
      
      sigma_cc += val1*val1;
      sigma_dc += val1*val2;
      sigma_cc2 += val1c*val1c;
      sigma_dc2 += val1c*val2;
    }
    slope = sigma_dc/sigma_cc;
    slope2 = sigma_dc2/sigma_cc2;
    
    // Perform fit.    
    mongr[mon] = new TGraph(fNEvents, x, y);
    fctn->SetParameters(d_mean, slope, slope2);
    mongr[mon]->Fit("fctn", "qr");
    if(makePlots){
      cM->cd(mon+1);
      mongr[mon]->SetMarkerColor(kBlue);
      mongr[mon]->SetTitle(Form("%s vs. ramp", MonitorList[mon].Data()));
      mongr[mon]->GetXaxis()->SetTitle("ramp");
      mongr[mon]->GetXaxis()->SetTitle(Form("%s",MonitorList[mon].Data()));
      mongr[mon]->Draw("AP");
      cM->Update();
    }
    
    //Const = fctn->GetParameter(0);        
    //ConstError = fctn->GetParError(0);
    sinAmp = fctn->GetParameter(1);
    sinAmpError = fctn->GetParError(1);
    cosAmp = fctn->GetParameter(2);
    cosAmpError = fctn->GetParError(2);
    
    MonitorSlope[fNModType][mon].push_back(sinAmp);
    MonitorSlopeError[fNModType][mon].push_back(sinAmpError);
    MonitorSlope[fNModType+5][mon].push_back(cosAmp);
    MonitorSlopeError[fNModType+5][mon].push_back(cosAmpError);
    
  }
  
  if(makePlots){
    cM->cd(6);
    TPaveText* pt1 = new TPaveText(0.05, 0.05, 0.8, 0.8);
    pt1->AddText(Form("%s Modulation", ModName[fNModType]));
    pt1->SetFillColor(43);
    pt1->Draw();
    cM->cd();
    cM->Print(Form("MonitorPlot%i.png",fNModType));
    delete cM;
  }
  
  ///////////////
  // Detectors //
  ///////////////
  
  TCanvas *cD;
  
  if(makePlots){
    cD = new TCanvas("cD", "canvas", 2400, 2000);
    cD->Divide(6,5);
  }
  
  TGraph* detgr[fNDetector];
  
  for(Int_t det=0; det<fNDetector; det++) {
    if(DetectorData[det].size() <= 0){
      std::cout << "!!!!!!!!!!!!!!!!! Illegal Detector vector length:\t" 
		<< DetectorData[det].size() << std::endl;
      return;
    }
    
    sigma_cc=0;
    sigma_dc=0;
    sigma_cc2=0;
    sigma_dc2=0;
    d_mean =0;

    for(Int_t evNum=0; evNum<fNEvents; evNum++) {
      y[evNum] = DetectorData[det][evNum];
      d_mean += y[evNum];      
    }
    d_mean /= fNEvents;
    
    for(Int_t evNum=0; evNum<fNEvents; evNum++) {
      Double_t val1 = TMath::Sin(kDegToRad*x[evNum]) - c_mean;
      Double_t val2 = y[evNum] - d_mean;
      Double_t val1c = TMath::Cos(kDegToRad*x[evNum]) - c_mean2;
      
      sigma_cc += val1*val1;
      sigma_dc += val1*val2;
      sigma_cc2 += val1c*val1c;
      sigma_dc2 += val1c*val2;
    }
    slope = sigma_dc/sigma_cc;
    slope2 = sigma_dc2/sigma_cc2;
    
    // Perform fit.    
    detgr[det] = new TGraph(fNEvents, x, y);
    fctn->SetParameters(d_mean, slope, slope2);
    detgr[det]->Fit("fctn", "qr");
    if(makePlots){
      cD->cd(det+1);
      detgr[det]->SetMarkerColor(kBlue);
      detgr[det]->SetTitle(Form("%s vs. ramp", DetectorList[det].Data()));
      detgr[det]->GetXaxis()->SetTitle("ramp");
      detgr[det]->GetXaxis()->SetTitle(Form("%s", DetectorList[det].Data()));
      detgr[det]->Draw("AP");
      cD->Update();
    }
    
    Const = fctn->GetParameter(0);        
    //ConstError = fctn->GetParError(0);
    sinAmp = fctn->GetParameter(1);
    sinAmpError = fctn->GetParError(1);
    cosAmp = fctn->GetParameter(2);
    cosAmpError = fctn->GetParError(2);
    
    DetectorSlope[fNModType][det].push_back(sinAmp/Const);
    DetectorSlopeError[fNModType][det].push_back(sinAmpError);
    DetectorSlope[fNModType+5][det].push_back(cosAmp/Const);
    DetectorSlopeError[fNModType+5][det].push_back(cosAmpError);
    
  }
  
  if(makePlots){
    cD->cd(30);
    TPaveText* pt2 = new TPaveText(0.05, 0.05, 0.8, 0.8);
    pt2->AddText(Form("%s Modulation", ModName[fNModType]));
    pt2->SetFillColor(43);
    pt2->Draw();
    cD->cd();
    cD->Print(Form("DetectorPlot%i.png",fNModType));
    delete cD;
  }
  
  //  CoilData[fNModType].clear();
  
  // These need to be set so we know if we have a full set of modulation data
  if(fNModType == fXModulation)  fXinit = true;
  if(fNModType == fYModulation)  fYinit = true;
  if(fNModType == fEModulation)  fEinit = true;
  if(fNModType == fXPModulation) fXPinit = true;
  if(fNModType == fYPModulation) fYPinit = true;
  
  return;
}

void QwMpsOnly::CalculateSlope(Int_t fNModType)
{

  Double_t c_mean = 0;
  Double_t d_mean = 0;
  Double_t sigma_cc = 0;
  Double_t sigma_dc = 0;
  Double_t sigma_dd = 0;
  Double_t sigma_slope = 0;
  Double_t slope = 0;

  //  DetectorSlope[fNModType].resize(fNEvents);
  if(!fPhaseConfig){
    Double_t temp[5]={0.26, 0.26, 0.0, 1.08, 1.08};              
    SetPhaseValues(temp); 
  }

  if(fNEvents < 3){
    std::cout << red <<"Error in run:: Number of good events too small, exiting." 
	      << normal << std::endl;
    return;
  }
  
  if(CoilData[fNModType].size() <= 0){
    std::cout << "!!!!!!!!!!!!!!!!! Illegal Coil vector length:\t" 
	      << CoilData[fNModType].size() << std::endl;
    return;
  }

  //*******************************
  ModulationEvents[fNModType] += fNEvents;
  //*******************************
   
//  if(fNModType == 0)
//     std::cout<<"mod_type# "<<fNModType<<"\n";

  for(Int_t det = 0; det < fNDetector; det++){
    
    if(DetectorData[det].size() <= 0){
      std::cout << "!!!!!!!!!!!!!!!!! Illegal Detector vector length:\t" 
		<< DetectorData[det].size() << std::endl;
      return;
    }

  
    for(Int_t evNum = 0; evNum < fNEvents; evNum++) 
      c_mean += TMath::Sin( kDegToRad*CoilData[fNModType][evNum] + 
			    phase[fNModType]);
    c_mean /= fNEvents;
    
    for(Int_t evNum = 0; evNum < fNEvents; evNum++) 
      d_mean += DetectorData[det][evNum];
    d_mean /= fNEvents;

    for(Int_t evNum = 0; evNum < fNEvents; evNum++){
      Double_t val = (TMath::Sin( kDegToRad*CoilData[fNModType][evNum] 
			     + phase[fNModType] )- c_mean);
      sigma_cc += val * val;
      sigma_dc += (DetectorData[det][evNum] - d_mean) * val;
      sigma_dd += TMath::Power((DetectorData[det][evNum] - d_mean), 2);
      
      // Clear instances after computation
      //     DetectorData[det].clear();
    }
    
    slope = sigma_dc/sigma_cc;
    sigma_slope = TMath::Sqrt((sigma_dd - ( (sigma_dc*sigma_dc)/sigma_cc) )
			      /(sigma_cc*( fNEvents - 2 )));
    
    //
    // Load Yields in to make Yield Correction a little easier in the end.
    //
    if(fSensHumanReadable == 1){
      DetectorSlope[fNModType][det].push_back(1e6*slope
					      /( TMath::Abs(d_mean) ));
      DetectorSlopeError[fNModType][det].push_back(1e6*sigma_slope
						   /( TMath::Abs(d_mean) ));
      
    }else{
      DetectorSlope[fNModType][det].push_back(slope/( TMath::Abs(d_mean) ));
      DetectorSlopeError[fNModType][det].push_back(sigma_slope/( TMath::Abs(d_mean) ));
    }
    
    c_mean = 0;
    d_mean = 0;
    slope = 0;
    sigma_slope = 0;
    sigma_cc = 0;
    sigma_dc = 0;
    sigma_dd = 0;
  }
  
  for(Int_t mon = 0; mon < fNMonitor; mon++){
    if(MonitorData[mon].size() <= 0){
      std::cout << "!!!!!!!!!!!!!!!!! Illegal Monitor vector length:\t" 
		<< MonitorData[mon].size() << std::endl;
      return;
    }
    for(Int_t evNum = 0; evNum < fNEvents; evNum++) 
      c_mean += TMath::Sin( kDegToRad*CoilData[fNModType][evNum] 
			    + phase[fNModType] );
    c_mean /= fNEvents;
    
    for(Int_t evNum = 0; evNum < fNEvents; evNum++) 
      d_mean += MonitorData[mon][evNum];
    d_mean /=fNEvents;
    
    //Linearize Data --don  
    for(Int_t evNum = 0; evNum < fNEvents; evNum++){
      Double_t val = (TMath::Sin(kDegToRad*CoilData[fNModType][evNum] 
				 + phase[fNModType] ) - c_mean);
      sigma_cc += val*val;
      sigma_dc += val*(MonitorData[mon][evNum] - d_mean);
      sigma_dd += TMath::Power((MonitorData[mon][evNum] - d_mean),2);
      
      // Clear instances after computation
      //     MonitorData[mon].clear();
    }
    
    slope = sigma_dc/sigma_cc; 
    sigma_slope = TMath::Sqrt((sigma_dd - (sigma_dc*sigma_dc)/sigma_cc)
			      /(sigma_cc*(fNEvents -2 )));
    
    MonitorSlope[fNModType][mon].push_back(slope);
    MonitorSlopeError[fNModType][mon].push_back(sigma_slope);
    
    c_mean = 0;
    d_mean = 0;
    slope = 0;
    sigma_slope = 0;
    sigma_cc = 0;
    sigma_dc = 0;
    sigma_dd = 0;  
    
  }
  // Same as above.
  //  CoilData[fNModType].clear();
  
  // These need to be set so we know if we have a full set of modulation data
  
  if(fNModType == fXModulation)  fXinit = true;
  if(fNModType == fYModulation)  fYinit = true;
  if(fNModType == fEModulation)  fEinit = true;
  if(fNModType == fXPModulation) fXPinit = true;
  if(fNModType == fYPModulation) fYPinit = true;
  
  return;
}

void QwMpsOnly::CalculateWeightedSlope(Int_t verbose)
{

  Double_t mean = 0;
  Double_t mean_error = 0;

  for(Int_t i = 0; i < fNModType; i++){
    for(Int_t j = 0; j < fNDetector; j++){
      for(Int_t k = 0; k < (Int_t)DetectorSlope[i][j].size(); k++){
	mean += ( DetectorSlope[i][j][k]
		  * TMath::Power(DetectorSlopeError[i][j][k],-2) );

        mean_error += TMath::Power(DetectorSlopeError[i][j][k],-2);
      }
      if(mean_error > 0){
	mean /= mean_error;
	AvDetectorSlope[i].push_back(mean);
	AvDetectorSlopeError[i].push_back(TMath::Sqrt(1/mean_error));
	mean = 0;
	mean_error = 0;
      }
      else{ 
	mean = 0;
	mean_error = 0;
	std::cout << "\n[empty]Detector Weighted Mean:= "<< mean 
		  << " +/- " << mean_error << std::endl; 
	AvDetectorSlope[i].push_back(mean);
	AvDetectorSlopeError[i].push_back(mean_error);

      }
    }
  }
  for(Int_t i = 0; i < fNModType; i++){
    for(Int_t j = 0; j < fNMonitor; j++){
      for(Int_t k = 0; k < (Int_t)MonitorSlope[i][j].size(); k++){
	mean += ( MonitorSlope[i][j][k]
		  *TMath::Power(MonitorSlopeError[i][j][k],-2) );
	mean_error += TMath::Power(MonitorSlopeError[i][j][k],-2);
      }
      if(mean_error > 0){
	mean /= mean_error;
	AvMonitorSlope[i].push_back(mean);
	AvMonitorSlopeError[i].push_back(TMath::Sqrt(1/mean_error));

	mean = 0;
	mean_error = 0;
      }
      else{
	mean = 0;
	mean_error = 0;
	std::cout << "Monitor Weighted Mean:= "<< mean << " / " 
		  << mean_error << std::endl; 
	AvMonitorSlope[i].push_back(mean);
	AvMonitorSlopeError[i].push_back(mean_error);
      }
    }
  }
  if(verbose)
    PrintAverageSlopes();
  DetectorSlope.clear();
  DetectorSlopeError.clear();
  MonitorSlope.clear();
  MonitorSlopeError.clear();
  
  std::cout << "Weighted Averages calculated." << std::endl;
  
}


void QwMpsOnly::CheckFlags()
{

  if( !(fXinit && fYinit && fEinit && fXPinit && fYPinit) ){
    PrintError("Not enough modulation cycles in this run -- exiting");
    CleanFolders();
    exit(1);
  }
  return;

}

Int_t QwMpsOnly::CheckRampLinearity(TString type){

  TString prefix;

  if(type.CompareTo("hel_tree", TString::kExact) == 0){
    prefix = "yield_";
  }
  else{
    prefix = "";
  }

  double diff = TMath::Abs((ramp_block3+ramp_block0) - (ramp_block2+ramp_block1));

  return (diff < fMaxRampNonLinearity ? 0 : 1); 
}

void QwMpsOnly::Clean()
{

  // This function serves the purpose of deallocating 
  // memory for unused vectors in the QwMpsOnly Class.
  // Should be run after finishing with the slope calculation.
  //

  DetectorData.clear();
  MonitorData.clear();
  AvDetectorSlope.clear();
  AvDetectorSlopeError.clear();
  AvMonitorSlope.clear();
  AvMonitorSlopeError.clear();
  CoilData.clear();
  MonitorSlope.clear();
  MonitorSlopeError.clear();
  DetectorSlope.clear();
  DetectorSlopeError.clear();

  return;
}

void QwMpsOnly::CleanFolders()
{
  gSystem->Exec(Form("rm -rf %s/regression/regression_%i.%s.dat", 
		     gSystem->Getenv("BMOD_OUT"), run_number, fSetStem.Data()));
  gSystem->Exec(Form("rm -rf %s/slopes/slopes_%i.%s.dat",
		     gSystem->Getenv("BMOD_OUT"), run_number, fSetStem.Data()));

  return;
}

void QwMpsOnly::ComputeAsymmetryCorrections()
{

//**************************************************************
//
// Time to calculate some Corrections 
//
//**************************************************************

  TFile file(Form("%s/rootfiles/mps_bmod_tree%s_%i.%s.root", output.Data(), fFileSegment.Data(), run_number, fSetStem.Data()),"RECREATE");

  TTree *mod_tree = new TTree("Mod_Tree", "Modulation Analysis Results Tree");

  Int_t fEvCounter = 0;

  Double_t temp_correction = 0;
  Double_t monitor_correction[kNMaxDet][kNMaxMon];

  correction.resize(fNDetector);

  mod_tree->Branch("pattnum", &pattnum, "pattnum/D"); 
  mod_tree->Branch("yield_qwk_charge", &yield_qwk_charge_hw_sum, "yield_qwk_charge/D"); 

//   mod_tree->Branch("yield_ramp_block0", &yield_ramp_block0, "yield_ramp_block0/D"); 
//   mod_tree->Branch("yield_ramp_block1", &yield_ramp_block1, "yield_ramp_block1/D"); 
//   mod_tree->Branch("yield_ramp_block2", &yield_ramp_block2, "yield_ramp_block2/D"); 
//   mod_tree->Branch("yield_ramp_block3", &yield_ramp_block3, "yield_ramp_block3/D"); 

  mod_tree->Branch("yield_bm_pattern_number", &yield_bm_pattern_number, "yield_bm_pattern_number/D"); 
  mod_tree->Branch("yield_ramp", &yield_ramp_hw_sum, "yield_ramp_hw_sum/D"); 
  mod_tree->Branch("ErrorFlag", &ErrorFlag, "ErrorFlag/D"); 
  mod_tree->Branch("yield_qwk_mdallbars_Device_Error_Code", &yield_qwk_mdallbars_Device_Error_Code, "yield_qwk_mdallbars_Device_Error_Code/D"); 
  mod_tree->Branch("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, "yield_qwk_mdallbars/D"); 
  mod_tree->Branch("asym_qwk_charge", &asym_qwk_charge_hw_sum, "asym_qwk_charge/D"); 

  for(Int_t i = 0; i < fNDetector; i++){
    mod_tree->Branch(HDetectorList[i], &HDetBranch[i][0], Form("%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("raw_%s",HDetectorList[i].Data()), &HDetBranch[i][0], Form("raw_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("corr_%s", HDetectorList[i].Data()), &AsymmetryCorrection[i], Form("corr_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("raw_corr_%s", HDetectorList[i].Data()), &AsymmetryCorrection[i], Form("raw_corr_%s/D", HDetectorList[i].Data())); 
    if(fCharge){
      mod_tree->Branch(Form("corr_%s_charge", HDetectorList[i].Data()), &AsymmetryCorrectionQ[i], 
		       Form("corr_%s_charge/D", HDetectorList[i].Data())); 
    }
    mod_tree->Branch(Form("%s_Device_Error_Code", HDetectorList[i].Data()), &HDetBranch[i][kDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("correction_%s", DetectorList[i].Data()), &correction[i], Form("correction_%s/D", DetectorList[i].Data())); 

    if(fCharge){
      mod_tree->Branch(Form("correction_%s_charge", DetectorList[i].Data()), &correction[i], 
		       Form("correction_%s_charge/D", DetectorList[i].Data())); 
    }
    std::cout << HDetectorList[i] << std::endl;
  }

  for(Int_t j = 0; j < fNMonitor; j++){
    mod_tree->Branch(HMonitorList[j], &HMonBranch[j][0], Form("%s/D", HMonitorList[j].Data())); 
    mod_tree->Branch(Form("raw_%s", HMonitorList[j].Data()), &HMonBranch[j][0], Form("raw_%s/D", HMonitorList[j].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", HMonitorList[j].Data()), &HMonBranch[j][kDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HMonitorList[j].Data())); 

    mod_tree->Branch(YMonitorList[j], &YMonBranch[j][0], Form("%s/D", YMonitorList[j].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", YMonitorList[j].Data()), &YMonBranch[j][kDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", YMonitorList[j].Data())); 
    std::cout << HMonitorList[j] << "\t" << YMonitorList[j] << std::endl;
  }

  for(Int_t i = 0; i < fNDetector; i++){
    for(Int_t j = 0; j < fNMonitor; j++){
      mod_tree->Branch(Form("corr_%s_%s", HDetectorList[i].Data(), HMonitorList[j].Data()), &monitor_correction[i][j], 
		       Form("corr_%s_%s/D", HDetectorList[i].Data(), HMonitorList[j].Data())); 
    }
  }

  if(fChain == 0) return;
  Long64_t nentries = fChain->GetEntries();

  Int_t event_counter = 0;

  std::cout << other << "Entries in Hel_Tree:\t" 
	    << nentries << normal << std::endl;

  for(Long64_t i = 0; i < nentries; i++){
    LoadTree(i);
    if(i < 0) break;
    fChain->GetEntry(i);
    ++fEvCounter;

    if( (ErrorCodeCheck("hel_tree") == 0) && CheckRampLinearity("") == 0 ){
      for(Int_t j = 0; j < fNDetector; j++){
	HDetBranch[j][0] = fChain->GetLeaf(Form("%s", HDetectorList[j].Data()))->GetValue();
	HDetBranch[j][kDeviceErrorCode] = fChain->GetLeaf(Form("%s_Device_Error_Code", HDetectorList[j].Data()))->GetValue();

	for(Int_t k = 0; k < fNMonitor; k++){
	  HMonBranch[k][0] = fChain->GetLeaf(Form("%s", HMonitorList[k].Data()))->GetValue();
	  YMonBranch[k][0] = fChain->GetLeaf(Form("%s", HMonitorList[k].Data()))->GetValue();
	  HMonBranch[k][kDeviceErrorCode] = fChain->GetLeaf(Form("%s_Device_Error_Code", HMonitorList[k].Data()))->GetValue();
	  YMonBranch[k][kDeviceErrorCode] = fChain->GetLeaf(Form("%s_Device_Error_Code", HMonitorList[k].Data()))->GetValue();

  	  temp_correction += YieldSlope[j][k]*HMonBranch[k][0]; 
	  monitor_correction[j][k] = YieldSlope[j][k]*HMonBranch[k][0];
	  if(fCharge) 
	  if( (i % 100000) == 0 ){}
	}
   	correction[j] = temp_correction;                                  

	if(fCharge) correction_charge[j] = temp_correction + ChargeSensitivity[j]*asym_qwk_charge_hw_sum;                                  
	if(fCharge) AsymmetryCorrectionQ[j] = HDetBranch[j][0] - correction_charge[j];

	else
	  AsymmetryCorrection[j] = HDetBranch[j][0] - correction[j];

	temp_correction = 0;
      }
      mod_tree->Fill();
      event_counter++;
    }

    if( (i % 100000) == 0 )std::cout << "Processing:\t" << i << std::endl;
  }

  file.Write();
  file.Close();

  return;
}

void QwMpsOnly::ComputeErrors(TMatrixD Y, TMatrixD eY, TMatrixD A, TMatrixD eA)
{

//   Double_t conversion[] = {1., 1.e6, 1., 1., 1.e6};

  TMatrixD var(fNMonitor, fNModType);
  TMatrixD temp(fNModType, fNMonitor);
  TMatrixD Errorm(fNDetector, fNModType);  
  TMatrixD Errord(fNDetector, fNModType);  
  TMatrixD Error(fNDetector, fNModType);  

    for(Int_t i = 0; i < fNMonitor; i++){
      for(Int_t k = 0; k < fNModType; k++){
	for(Int_t n = 0; n < fNModType; n++){
	  for(Int_t j = 0; j < fNModType; j++){
	    var(i, k) += TMath::Power(A(i, n), 2)*TMath::Power(eA(n, j), 2)*TMath::Power(A(j, k), 2);
	  }
	}
      }
    }
       
    for(Int_t m = 0; m < fNDetector; m++){    

      for(Int_t i = 0; i < fNMonitor; i++){
	for(Int_t j = 0; j < fNModType; j++){
 	  Errorm(m, i) += var(j, i)*TMath::Power( Y(m, j),2);
	}
      }
 
       for(Int_t i = 0; i < fNModType; i++)
         Errorm(m, i) = TMath::Sqrt(Errorm(m, i));
       for(Int_t i = 0; i < fNMonitor; i++){
	 for(Int_t j = 0; j < fNModType; j++){
	   Errord(m, i) += TMath::Power( A(j, i),2)*TMath::Power(eY(m, j) ,2);
	 }
       }
    
       for(Int_t i = 0; i < fNModType; i++)
	 Errord(m,i) = TMath::Sqrt(Errord(m,i));
    
     for(Int_t i = 0; i < fNModType; i++){
       Error(m, i) = TMath::Power(Errord(m, i), 2) + TMath::Power(Errorm(m, i), 2);
       Error(m, i) = TMath::Sqrt(Error(m, i))*fUnitConvert[i];
       YieldSlopeError[m][i] = Error(m, i);
     }

    }
    std::cout << other << "Errors:" << normal << std::endl;
    Error.Print();
}

Int_t QwMpsOnly::ConvertPatternNumber(Int_t global)
{
  Int_t key[16] = {0, 1, 2, 3, 4, 0, 0, 0, 0, 
		   0, 0, 0, 1, 2, 3, 4};
  if(global < 0) return(-1);
  if(global < 11) fSingleCoil = true;

  return(key[global]);
}

Int_t QwMpsOnly::Cut(Long64_t entry)
{
   return 1;
}


Int_t QwMpsOnly::ErrorCodeCheck(TString type)
{

//   Double_t subblock = 0;

  Int_t bmodErrorFlag = 0;
  Bool_t typeFound = kFALSE;

  if( type.CompareTo("mps_tree", TString::kIgnoreCase) == 0 ){
    typeFound = kTRUE;

    if(qwk_charge_hw_sum < fCurrentCut){
      bmodErrorFlag = 1;
    }
//     double lin = CheckRampLinearity("");
//     if(lin != 0)
//       bmodErrorFlag = 1;

    if( (ramp_hw_sum > fPedestal) && ((UInt_t)ErrorFlag != 0x4018080)  ){

#ifdef __VERBOSE_ERRORS

      std::cout << red << "Mps Tree::Modulation ErrorFlag" << normal << std::endl;

#endif

      bmodErrorFlag = 1;
    }

    if( !(ramp_hw_sum > fPedestal) && ((UInt_t)ErrorFlag != 0) ){

#ifdef __VERBOSE_ERRORS

      std::cout << red << "Mps Tree::Natural Motion ErrorFlag" << normal << std::endl;

#endif

      bmodErrorFlag = 1;
    }

  }
  
  if( type.CompareTo("hel_tree", TString::kIgnoreCase) == 0 ){
    typeFound = kTRUE;

        
    if( ((UInt_t)ErrorFlag != 0) && ((UInt_t)ErrorFlag != 67207296) ){
      //     if( ((UInt_t)ErrorFlag != 0) ){
      bmodErrorFlag = 1;
    }
    
    if(yield_qwk_charge_hw_sum < fCurrentCut){
      bmodErrorFlag = 1;
    }
    
    
  }
  
  //   return (0);
  if(!typeFound) std::cout<<"Tree not found."<<std::endl;
  return( bmodErrorFlag );
}

Bool_t QwMpsOnly::FileSearch(TString filename, TChain *chain, Bool_t sluglet)
{

  TString file_directory;
  Bool_t c_status = kFALSE;

  if(sluglet)
    file_directory = gSystem->Getenv("SLUGLET_FILES");
  else
    file_directory = gSystem->Getenv("MPS_ONLY_ROOTFILES");

  if(fFileSegmentInclude){
    c_status = true;

    if(sluglet){
      std::cout << other << "Adding:: " 
		<< filename << normal << std::endl;
      if(!(chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()))) ){
	std::cout << red << "Error chaining segment:\t" << filename << 
	  normal << std::endl;
	exit(1);
      }
    }else{
      filename = Form("%s_%d_%d:%d.root", fFileStem.Data(), run_number,
		      fLowerSegment, fUpperSegment);
      std::cout << other << "Adding:: " 
		<< filename << normal << std::endl;
      if(!(chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()))) ){
	std::cout << red << "Error chaining segment:\t" << filename << 
	  normal << std::endl;
	exit(1);
      }
    }
  }
  else{
    c_status = chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()));
    std::cout << "Trying to open :: "
	      << Form("%s/%s",file_directory.Data(), filename.Data())
	      << std::endl;
  }

  if(c_status){
    TString chain_name = chain->GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;

    while((chain_element = (TChainElement*)next())){
      if(!fFileSegmentInclude){
	std::cout << "Adding :: "
		  << filename
		  << " to data chain"
		  << std::endl;
      }
    }
  } 
    return c_status;

}

Double_t QwMpsOnly::FindDegPerEntry()
{
  Int_t ent = fChain->GetEntries(), newCycle = 0;
  Double_t mean = 0, n = 0, ramp_prev = -9999;
  printf("Finding # of degrees per entry. Searching %i entries.\n", ent);
  for(int i = 0;i<ent;i++){
    fChain->GetEntry(i);
    Double_t nonLin = TMath::Abs((ramp_block3+ramp_block0) - 
				 (ramp_block2+ramp_block1));
    if(nonLin>fMaxRampNonLinearity||ramp_hw_sum<ramp_prev)newCycle = 1;
    if(ErrorFlag==67207296&&qwk_charge_hw_sum>40&&ramp_hw_sum>100){
      if(ramp_hw_sum<310&&!newCycle){
	mean += ramp_hw_sum - ramp_prev;
	//	std::cout<<ramp_hw_sum - ramp_prev<<std::endl;
	n++;
      }
      newCycle = 0;
    }
    ramp_prev = ramp_hw_sum;
  }
  mean = mean / n;
  fDegPerEntry = mean;
  std::cout<<"Degrees per entry = "<<mean<<std::endl;
  return  mean;
}

Int_t QwMpsOnly::FindRampPeriodAndOffset()
{
  TCut cut;
  Double_t parLim[4] = {0,175,350,360};
  Double_t par[4] = {-500,1000,4,360};
  TF1 *f = new TF1("f", "[0]+[1]*sin((x+[2])*2*TMath::Pi()/[3])",0,360);
  f->SetParameters(par);
  f->SetParLimits(2,parLim[0],parLim[1]);
  f->SetParLimits(3,parLim[2],parLim[3]);

  cut = TCut(Form("TMath::Abs(ramp_block0+ramp_block3-ramp_block2-ramp_block1)"
		  "<%f && ramp_block3>ramp_block2 && ramp_block2>ramp_block1 &&"
		  "ramp_block1>ramp_block0", fMaxRampNonLinearity));

  fChain->Draw("fgx1:ramp>>ht(1000,0,400,1000)",cut, "goff");
  TH2D *ht = (TH2D*)gDirectory->Get("ht");
  ht->Fit(f);
  fRampOffset = f->GetParameter(2);
  fRampPeriod = f->GetParameter(3);
  if(!(fRampOffset>parLim[0]&&fRampOffset<parLim[1])){
    std::cout<<"fRampOffset is at limit of fit parameters. Exiting program.\n";
    return 1;
  }
  if(!(fRampPeriod>parLim[2]&&fRampPeriod<parLim[3])){
    std::cout<<"fRampPeriod is at limit of fit parameters. Exiting program.\n";
    return 1;
  }
  std::cout<<"Ramp offset: "<<fRampOffset<<".  Ramp period: "<<fRampPeriod<<
    std::endl;
  return 0;
}

Int_t QwMpsOnly::FindRampRange()
{ //two parameters are need, the ramp length and maximum,
  // are needed to map the ramp_return to the phase gap
  Int_t n = 0, max = 10000;
  Double_t *rampold, *rampnew, rampprev = 0;
  rampold = new Double_t[max];
  rampnew = new Double_t[max];
  fRampMax = fChain->GetMaximum("ramp") - 1;
  if(!(fRampMax>325&&fRampMax<340)){
    std::cout<<"fRampMax not within specified bounds. Exiting program.\n";
    return 1;
  }

  for(Int_t i=0;i<fChain->GetEntries()&&n<max;i++){
    fChain->GetEntry(i);
    if(ramp_hw_sum<fRampMax && ramp_hw_sum>fRampMax-fDegPerEntry/2.0 &&
       TMath::Abs(ramp_block3+ramp_block0-ramp_block1-ramp_block2)
       >fMaxRampNonLinearity){
      rampold[n] = ramp_hw_sum;
      rampnew[n] = rampprev + fDegPerEntry;
      //      std::cout<<rampold[n]<<"\t"<<rampnew[n]<<"\n";
      n++;
    }
    rampprev = ramp_hw_sum;
  }
  TGraph gr = TGraph(n,rampold,rampnew);
  TF1 *myfit = new TF1("myfit","pol1");
  gr.Fit(myfit);
  fRampLength = fRampPeriod / (1 - myfit->GetParameter(1));
  fRampMax = myfit->GetParameter(0) * fRampLength / fRampPeriod;
  std::cout<<"RampLength: "<<fRampLength<<".  RampMax:"<<fRampMax<<std::endl;
  return 0; 
}

Int_t QwMpsOnly::GetCurrentCut()
{
  return(fCurrentCut);
}

Double_t QwMpsOnly::GetDegPerEntry()
{
  return fDegPerEntry;
}

Int_t QwMpsOnly::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

void QwMpsOnly::GetOptions(Char_t **options){
  Int_t i = 0;

  TString flag;

  while(options[i] != NULL){
    flag = options[i];

    if(flag.CompareTo("--run", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fRunNumberSet = true;
      run_number = atoi(options[i + 1]);

      std::cout << other << "Processing run number:\t" 
		<< run_number
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--phase-config", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fPhaseConfig = true;
      ReadPhaseConfig(options[i + 1]);
      std::cout << other << "Setting external phase values:\t" 
		<< options[i + 1] << normal << std::endl;
    }    

    if(flag.CompareTo("--charge-sens", TString::kExact) == 0){
      fCharge = true;
      flag.Clear();
      fChargeFile = Form("config/charge_sensitivity_%i.dat", run_number);
      std::cout << other << "Setting up pseudo 5+1 analysis:\t" << fChargeFile << normal << std::endl;
      ReadChargeSensitivity();
    }    

    if(flag.CompareTo("--file-segment", TString::kExact) == 0){
      std::string option(options[i+1]);
      fFileSegmentInclude = true;

      flag.Clear();
      fFileSegment = options[i + 1];
      Int_t index = option.find_first_of(":");
      fLowerSegment = atoi(option.substr(0, index).c_str());
      fUpperSegment = atoi(option.substr(index + 1, option.length()-index).c_str());

      std::cout << other << "Processing file segments:\t" 
		<< fLowerSegment << ":" 
		<< fUpperSegment << normal << std::endl;
    }

    if(flag.CompareTo("--file-stem", TString::kExact) == 0){
      std::string option(options[i+1]);

      flag.Clear();
      fFileStem = options[i + 1];

      std::cout << other << "Setting file stem to:\t" 
		<< fFileStem << ":" 
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--set-stem", TString::kExact) == 0){
      std::string option(options[i+1]);

      flag.Clear();
      fSetStem = options[i + 1];

      std::cout << other << "Setting set stem to:\t" 
		<< fSetStem << ":" 
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--ramp-pedestal", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fPedestal = atoi(options[i + 1]);

      std::cout << other << "Setting ramp pedestal to:\t" 
		<< fPedestal
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--ramp-max-nonlin", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fMaxRampNonLinearity = atoi(options[i + 1]);

      std::cout << other << "Setting ramp maximum non-linearity to:\t" 
		<< fMaxRampNonLinearity
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--charge", TString::kExact) == 0){
      fCharge = true;
      flag.Clear();
      if( IfExists(options[i + 1]) ){
	flag = options[i + 1];
	fChargeFile = flag;
	std::cout << other << "Setting up pseudo 5+1 analysis w/ external nonlinearity file:\t" << flag << normal << std::endl;
	ReadChargeSensitivity();
      }
      else{
	PrintError("Could not open inpust file.  Disabling charge sensitivity analysis");
	fCharge = false;
      }
    }
    
    if(flag.CompareTo("--current-cut", TString::kExact) == 0){
      flag.Clear();
      fCurrentCut = atoi(options[i + 1]);
      std::cout << other << "Setting current-cut to:\t" << fCurrentCut << normal << std::endl;
    }

    // flag to turn ON/OFF new energy bpm
    if(flag.CompareTo("--fNewEbpm", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();

      fNewEbpm = atoi(options[i + 1]);

      std::cout << other << "Setting use NEW energy bpm flag to " << fNewEbpm
		<< normal << std::endl;

    }  
  
    // Flag to turn ON 2D fit. (Default = OFF)
    if(flag.CompareTo("--2dfit", TString::kExact) == 0){
      flag.Clear();
      f2DFit = true;
      std::cout << "2D Fit Selected" << std::endl;
    } 

    if(flag.CompareTo("--help", TString::kExact) == 0){
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      printf("Usage: ./qwbeammod <run_number> <options>");
      printf("\n\t--charge-sens \t\tinclude charge sensitivity in overall correction to physics asymmetry.");
      printf("\n\t--charge \t\tsame as --chare-sesn except use can specify path of charge sensitivities.");
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      exit(0);
    }        
    i++;
  }
}

Bool_t QwMpsOnly::IfExists(const char *file)
{
  if(FILE *file_to_check = fopen(file, "r")){
    fclose(file_to_check);
    return true;
  }
  return false;
}

void QwMpsOnly::Init(TChain *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   Notify();
}

void QwMpsOnly::LoadRootFile(TString filename, TChain *tree, Bool_t slug)
{
  Bool_t found = FileSearch(filename, tree, slug);
  
    if(!found){
      std::cerr << "Unable to locate requested file :: "
		<< filename
		<< std::endl;
      exit(1);
    }
}

Long64_t QwMpsOnly::LoadTree(Long64_t entry)
{
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

Int_t QwMpsOnly::MakeRampFilled(Bool_t verbose)
{ //Take ramp values from return and map them to meaningful ramp values.
  //This is preformed using the following equation for the nonlinear points:
  //ramp_filled = (ramp_max-ramp)/ramp_length*(ramp_period-ramp_length)+ramp_max


  std::cout<<"::Making \"ramp_filled\" variable::\n";
  FindDegPerEntry();
  //create new friendable tree with new ramp
  TFile *newfile = new TFile(Form("%s/mps_only_friend_%i.root",
				  gSystem->Getenv("MPS_ONLY_ROOTFILES"),
				  run_number),"recreate");
  TTree *newTree = new TTree("mps_slug", "mps_slug"); 

  if(FindRampPeriodAndOffset())
    return 1;
  if(FindRampRange())
    return 1;

  Bool_t isLinear;
  Double_t ramp_f, lin;

  b_ramp_filled = newTree->Branch("ramp_filled", &ramp_f, "ramp_filled/D");
  
  for(Int_t i=0;i<fChain->GetEntries();i++){
    fChain->GetEntry(i);
    lin =  TMath::Abs(ramp_block0+ramp_block3-ramp_block2-ramp_block1);
    isLinear = lin < fMaxRampNonLinearity && ramp_block3>ramp_block2 && 
               ramp_block2>ramp_block1 && ramp_block1>ramp_block0;

    ramp_f = (isLinear ? ramp_hw_sum : (fRampMax-ramp_hw_sum)/ fRampLength *
	      (fRampPeriod - fRampLength) + fRampMax);


    //stretch and offset ramp before filling
    ramp_f = (ramp_f + fRampOffset) * 360.0 / fRampPeriod;
    ramp_f = ramp_f - ((Int_t)(ramp_f/360.0))*360.0;
    //    if(!isLinear)std::cout<<ramp_f<<std::endl;

    newTree->Fill();
    if(i%20000==0)std::cout<<"Processing entry "<<i<<".\n";
  }
  if(verbose)
    newTree->Print();
  newTree->Write("",TObject::kOverwrite);
  //  newTree->AutoSave();
  fChain->AddFriend("mps_slug", newfile);
  fChain->SetBranchAddress("ramp_filled", &ramp_filled);
  //  newfile->Close();
  //  delete newfile;
  return 0;
}

void QwMpsOnly::MatrixFill()
{

  TMatrixD AMatrix(fNDetector, fNModType);
  TMatrixD AMatrixE(fNDetector, fNModType);

  Double_t determinant;

  CheckFlags();

  diagnostic.open(Form("%s/diagnostics/diagnostic%s_%i.%s.dat", output.Data(), fFileSegment.Data(), run_number, fSetStem.Data()) , fstream::out);

  for(Int_t j = 0; j < fNDetector; j++){
    diagnostic << DetectorList[j] << std::endl;
    for(Int_t k = 0; k < fNModType; k++){
      AMatrix(j,k) = AvDetectorSlope[k][j];
      AMatrixE(j,k) = AvDetectorSlopeError[k][j];
      if( (diagnostic.is_open() && diagnostic.good()) ){
	diagnostic << AvDetectorSlope[k][j] << " +- " 
		   << AvDetectorSlopeError[k][j] << " ";
      }
    }
    diagnostic << std::endl;
  }
  diagnostic << std::endl;
  std::cout << "\t\t\t\t::A Matrix::"<< std::endl;
  AMatrix.Print("%11.10f");

  std::cout << "\t\t\t\t::A Matrix Error::"<< std::endl;
  AMatrixE.Print("%11.10f");

  TMatrixD RMatrix(fNMonitor, fNModType);
  TMatrixD RMatrixE(fNMonitor, fNModType);

  for(Int_t j = 0; j < fNMonitor; j++){
    for(Int_t k = 0; k < fNModType; k++){
      RMatrix(j,k) = AvMonitorSlope[k][j];
      RMatrixE(j,k) = AvMonitorSlopeError[k][j];

      if( (diagnostic.is_open() && diagnostic.good()) ){
	diagnostic << AvMonitorSlope[k][j] << " +- " 
		   << AvMonitorSlopeError[k][j] << " ";
      }
    }
    diagnostic << std::endl;
  }
  std::cout << "\t\t\t\t::R Matrix:: " << std::endl;
  RMatrix.Print("%11.10f");
  
  std::cout << "\t\t\t\t::R Matrix Error::"<< std::endl;
  RMatrixE.Print("%11.10f");
  
  TMatrixD RMatrixInv = RMatrix;
  RMatrixInv.Invert(&determinant);
  std::cout << "\t\t\t\t::R Matrix Inverse:: " << std::endl;
  
  RMatrixInv.Print("%11.10f");
  
  std::cout << determinant << std::endl;
  TMatrixD Identity(fNMonitor, fNMonitor);
  Identity.Mult(RMatrixInv, RMatrix);
  Identity.Print();

  TMatrixD SMatrix(fNDetector, fNModType);
  SMatrix.Mult(AMatrix, RMatrixInv);

  std::cout << "\n\n\t\t\t\t::SMatrix::\n" << std::endl;

  for(Int_t i = 0; i < fNDetector; i++){
    for(Int_t j = 0; j < fNModType; j++){
      SMatrix(i,j) *= fUnitConvert[j];
      YieldSlope[i][j] = SMatrix(i,j);
    }
  }
  SMatrix.Print();
  ComputeErrors(AMatrix, AMatrixE, RMatrixInv, RMatrixE);

  Write();

  if(fSensHumanReadable == 1){
    std::cout << "Exiting after sensitivities." << std::endl;
    exit(1);
  }
}

Bool_t QwMpsOnly::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

Int_t QwMpsOnly::PilferData()
{
  Int_t fEvCounter = 0, nonModEvts = 0;
  Int_t error[kNMaxCoil], good[kNMaxCoil];

  for(Int_t i=0;i<kNMaxCoil;i++){
    error[i] = 0;
    good[i] = 0;
  }
  fNEvents = 0;
  if (fChain == 0) return -1;
  Long64_t nentries = fChain->GetEntries();

  std::cout << "Number of entries: " << nentries << std::endl;

  for(Int_t i = 0; i < nentries; i++){
    fChain->GetEntry(i);
    Int_t pat = ConvertPatternNumber((Int_t)bm_pattern_number);
    if(pat > -1 && pat < 5)
      i =  ProcessMicroCycle(i, &fEvCounter, &error[0], &good[0]);
    else nonModEvts++;
  }

  std::cout << "Run "<<run_number<<std::endl;
  Int_t goodEv = 0;
  for(Int_t i = 0; i < kNMaxCoil; i++){
    std::cout<<" bm_pattern# "<<i<<":  Total Events Cut (" << error[i] <<")"
	      <<"   Good Events ("<<good[i]<<")\n";
    goodEv += good[i];
  }

  std::cout <<nonModEvts<<" non-modulation events." << std::endl;
  std::cout << "\n::Done with Pilfer::" << std::endl;

  return goodEv;
}

void QwMpsOnly::PrintAverageSlopes()
{
  printf("Monitor Slopes   |  Pattern 0   |  Pattern 1   |  Pattern 2   |"
	 "  Pattern 3   |  Pattern 4   |\n");
  printf("******************************************************************"
	 "***************************\n");

  for(Int_t i=0;i<fNMonitor;i++){
    TString mon = MonitorList[i];
    mon.Resize(16);
    printf("%s |",mon.Data());
    for(Int_t j=0;j<fNModType;j++){
      printf(" %+9.5e |",AvMonitorSlope[j][i]);
    }
    printf("\n");
  }
  printf("\n\n");
  printf("Detector Slopes  |  Pattern 0   |  Pattern 1   |  Pattern 2   |"
	 "  Pattern 3   |  Pattern 4   |\n");
  printf("******************************************************************"
	 "***************************\n");

  for(Int_t i=0;i<fNDetector;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    printf("%s |",det.Data());
    for(Int_t j=0;j<fNModType;j++){
      printf(" %+9.5e |",AvDetectorSlope[j][i]);
    }
    printf("\n");
  }
  printf("\n\n");
}

void QwMpsOnly::PrintError(TString error)
{
  std::cout << red << error << normal << std::endl;

  return;
}

Int_t QwMpsOnly::ProcessMicroCycle(Int_t i, Int_t *evCntr, Int_t *err, 
				   Int_t *good)
{
  Int_t nEnt = fChain->GetEntries();
  Int_t modType = -1, modNum = -1, nCut = 0, nErr = 0;
  //Double_t prev_ramp = 0;
  Int_t pattern = ConvertPatternNumber((Int_t)bm_pattern_number);
  fNEvents = 0;
  switch(pattern){
  case 0:
    modNum = 0;
    modType = fXModulation;
    break;
  case 1:
    modNum = 1;
    modType = fXPModulation;
    break;
  case 2:
    modNum = 2;
    modType = fEModulation;
    break;
  case 3:
    modNum = 3;
    modType = fYModulation;
    break;
  case 4:
    modNum = 4;
    modType = fYPModulation;
    break;
  default:
    modNum = -1;
    std::cout<<"Modulation type "<<pattern<<" unknown.\n"<<std::endl;
    break;
  }
  if(modNum==-1)return -1;
  std::cout<<"Modulation type "<<pattern<<" found at entry "<<i<<"\n";

  //Make sure arrays are clear of data
  CoilData[modType].clear();
  for(Int_t j = 0; j < fNMonitor; j++){
    MonitorData[j].clear();
  }
  for(Int_t j = 0; j < fNDetector; j++){
    DetectorData[j].clear();
  }

  while(pattern == modNum && i < nEnt){

    if(ErrorCodeCheck("mps_tree")!=0){
      nCut++;
      nErr++;
    }else{
      good[modNum]++;
      CoilData[modType].push_back(ramp_filled);

      for(Int_t j = 0; j < fNDetector; j++){
	Double_t val = fChain->GetLeaf(DetectorList[j].Data())->GetValue();
	DetectorData[j].push_back(val);
      }
      for(Int_t j = 0; j < fNMonitor; j++){
	Double_t val = fUnitConvert[j]*
	  (fChain->GetLeaf(MonitorList[j].Data())->GetValue());
	MonitorData[j].push_back(val);
      }
      ++evCntr;
      ++fNEvents;
    }
    i++;
    if(i==nEnt)break;

    fChain->GetEntry(i);
    pattern = ConvertPatternNumber((Int_t)bm_pattern_number);
    if(i%1000==0)std::cout<<i<<"  pattern "<<pattern<<std::endl;
  }
  i--;
  err[modNum] += nErr;
  std::cout<<fNEvents<< " good "<<pattern<<"-type modulation events found. ";
  std::cout<<nErr<<" errors -- "<<err[modNum]<<" total "<<pattern<<
    "-type errors.\n";

  if(f2DFit) {
    Calculate2DSlope(modType, 0);
  } else {
    CalculateSlope(modType);
  }
  
  fNEvents = 0;
  return i;
}

void QwMpsOnly::ReadChargeSensitivity(){

  std::string line;
  char *token;

  charge_sens.open(fChargeFile, fstream::in);

  if(!charge_sens.is_open()){
    PrintError("Error opening charge sensitivies file."); 
    exit(1);
  }

  while(charge_sens.good()){
    getline(charge_sens, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,");
    while(token){
      ChargeSensitivity.push_back(atof(token));
      token = strtok(NULL, " ,");
      ChargeSensitivityError.push_back(atof(token));
      token = strtok(NULL, " ,");
    }
  }
  charge_sens.close();
  return;

}

Int_t QwMpsOnly::ReadConfig(TString opt)
{
  std::string line;

  char *token;

  TString mon_prefix;
  TString det_prefix;

  Bool_t fHelTree = false;

  if(opt.CompareTo("bmod", TString::kExact) == 0){
    mon_prefix = "diff_";
    det_prefix = "asym_"; 
  }
  if(opt.CompareTo("hel", TString::kExact) == 0){
    mon_prefix = "diff_";
    det_prefix = "asym_"; 

    fHelTree = true;
    std::cout << other << "HelTree Configuration flag set" << normal << std::endl;
  }
  else{
    mon_prefix = "";
    det_prefix = ""; 
  }
  
  config.open(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")), 
	      std::ios_base::in);
  if(!config.is_open()){
    std::cout << red << "Error opening config file" << normal << std::endl;
    exit(1);
  }

  while(config.good()){
    getline(config, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,.");
    while(token){
      if(strcmp("mon", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
	if( !(fChain->GetBranch(Form("%s%s", mon_prefix.Data(), token))) ){
	  std::cout << other << token << ": Branch doesn't exist." << normal << std::endl;
	}
	else{
	  if(fHelTree) {
	    this->HMonitorList.push_back(Form("diff_%s", token));
	    fChain->SetBranchStatus(Form("diff_%s", token), 1);
	    fChain->SetBranchStatus(Form("diff_%s_Device_Error_Code", token), 1);

	    this->YMonitorList.push_back(Form("yield_%s", token));
	    fChain->SetBranchStatus(Form("yield_%s", token), 1);
	    fChain->SetBranchStatus(Form("yield_%s_Device_Error_Code", token), 1);
	    std::cout << HMonitorList.back() << "\t" << YMonitorList.back() << std::endl;
	  }
	  else{
	    std::cout << other << "\t\tMonitor is: " << token << normal << std::endl;
	    this->MonitorList.push_back(Form("%s%s", mon_prefix.Data(), token)); 
	    fChain->SetBranchStatus(Form("%s%s", mon_prefix.Data(), token), 1);
	    fChain->SetBranchStatus(Form("%s%s_Device_Error_Code", mon_prefix.Data(), token), 1);
	  }
	}
      }
      if(strcmp("det", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token
       	token = strtok(NULL, " .,"); 
	
	if( !(fChain->GetBranch(Form("%s%s", det_prefix.Data(), token))) ){
	  std::cout << other << token << ": Branch doesn't exist." << normal << std::endl;
	}
	else{
	  if(fHelTree) {
	    this->HDetectorList.push_back(Form("asym_%s", token));
	    fChain->SetBranchStatus(Form("asym_%s", token), 1);
	    fChain->SetBranchStatus(Form("asym_%s_Device_Error_Code", token), 1);
	    std::cout << HDetectorList.back() << std::endl;
	  }
	  else{
	    std::cout << other << "\t\tDetector is: " << token << normal << std::endl; 
	    this->DetectorList.push_back(Form("%s%s", det_prefix.Data(), token));
	    fChain->SetBranchStatus(Form("%s%s", det_prefix.Data(), token), 1);
	    fChain->SetBranchStatus(Form("%s%s_Device_Error_Code", det_prefix.Data(), token), 1);
	  }
	}
      }
      else 
	token = strtok(NULL, " .,"); 
      
    }
  }
  fNDetector = DetectorList.size();
  fNMonitor = MonitorList.size();
  
  if( (fNDetector > kNMaxDet) || (fNMonitor > kNMaxMon) )
    {
      std::cout << red << "Error :: Exceeded maximum number of detectors(monitors)" 
		<< red << "Detectors:\t" << fNDetector << "\tMax:\t" << kNMaxDet
		<< red << "Monitors:\t" << fNMonitor   << "\tMax:\t" << kNMaxMon
		<< normal << std::endl;
      exit(1);
    }
  
  config.close();
  
  // now create/update MonitorList with newEbpm, if needed
  if(fNewEbpm){

    BuildNewEBPM();
    
    TString fname = Form("%s/mps_only_newEbpm_%i.root",
			 gSystem->Getenv("MPS_ONLY_ROOTFILES"),
			 run_number);

    fChain->AddFriend("mps_slug", fname);
    fChain->SetBranchAddress("newEbpm", &newEbpm);

    // now set the new energy bpm as the energy bpm to be used
    for(int inew=0;inew<5;inew++){
      if(MonitorList[inew].CompareTo("qwk_bpm3c12X", TString::kExact) == 0)
	MonitorList[inew] = "newEbpm";
    }
  }

  return 0;
}

Int_t QwMpsOnly::ReadPhaseConfig(Char_t *file)
{

  std::string line;
  
  char *token;

  std::fstream fphase;


  fphase.open(file, std::ios_base::in);
  if(!fphase.is_open()){
    std::cout << red << "Error opening phase config file.  Using default values." 
	      << normal << std::endl;
    fPhaseConfig = false;
    return 1;
  }

  phase.resize(fNModType);
  while(fphase.good()){
    getline(fphase, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,.");
    while(token){
      if(strcmp("x", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tX phase is: " << token << normal << std::endl;
	phase[fXModulation] = atof(token); 
       }
      if(strcmp("xp", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tXP phase is: " << token << normal << std::endl;
	phase[fXPModulation] = atof(token); 
       }
      if(strcmp("e", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tE phase is: " << token << normal << std::endl;
	phase[fEModulation] = atof(token); 
       }
      if(strcmp("y", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tY phase is: " << token << normal << std::endl;
	phase[fYModulation] = atof(token); 
       }
      if(strcmp("yp", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tYP phase is: " << token << normal << std::endl;
	phase[fYPModulation] = atof(token); 
       }
      else 
       	token = strtok(NULL, " ,"); 
    }
    fPhaseConfig = true;
  }

  return 0;
}

void QwMpsOnly::ReduceMatrix(Int_t i)
{
  //
  //  i = 0, gives X, Y, E in the matrix
  //  i = 1, gives X, X',E in the matrix
  //
  switch(i){
  case 0:
    fReduceMatrix_xp = 1;
    fReduceMatrix_yp = 1;
    fYModulation = 1;
    fNModType = 3;
    break;

  case 1:
    fReduceMatrix_y  = 1;
    fReduceMatrix_yp = 1;
    fNModType = 3;
    break;

  default:
    exit(1);
    break;
  }
  return;
}

void QwMpsOnly::Scan()
{
                               
//    for(Int_t i = 0; i < (Int_t)fNDetector; i++){
//      fChain->SetBranchStatus(DetectorList[i], 1);
//      fChain->SetBranchAddress(DetectorList[i], &DetBranch[i]);
//    }
//    for(Int_t i = 0; i < (Int_t)fNMonitor; i++){
//      fChain->SetBranchStatus(MonitorList[i], 1);
//      fChain->SetBranchAddress(MonitorList[i], &MonBranch[i+1]);
//    }
}

void QwMpsOnly::SetDegPerEntry(Double_t deg)
{
  fDegPerEntry = deg;
}

void QwMpsOnly::SetFileName(TString & filename)
{
  fFileName = filename;
  return;
}

void QwMpsOnly::SetHuman()
{
  fSensHumanReadable = 1;
  std::cout << "Setting Human Readble Mode" << std::endl;
  return;
}

void QwMpsOnly::SetMaxRampNonLinearity(Double_t max){
   fMaxRampNonLinearity= max;
}

void QwMpsOnly::SetupHelBranchAddress()
{

  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("yield_qwk_charge", 1);
  fChain->SetBranchStatus("yield_qwk_charge_Device_Error_Code", 1);
//   fChain->SetBranchStatus("yield_bm_pattern_number", 1);
  fChain->SetBranchStatus("pattnum", 1);
  fChain->SetBranchStatus("yield_ramp", 1);
  fChain->SetBranchStatus("ErrorFlag", 1);
  fChain->SetBranchStatus("yield_qwk_charge", 1);
  fChain->SetBranchStatus("asym_qwk_charge", 1);

  fChain->SetBranchAddress("yield_qwk_charge",&yield_qwk_charge_hw_sum ,&b_yield_qwk_charge);
//   fChain->SetBranchAddress("yield_qwk_charge_Device_Error_Code",&yield_qwk_charge_Device_Error_Code ,&b_yield_qwk_charge_Device_Error_Code);
//   fChain->SetBranchAddress("yield_bm_pattern_number",&yield_bm_pattern_number,&b_yield_bm_pattern_number);
  fChain->SetBranchAddress("pattnum", &pattnum, &b_pattnum);
  fChain->SetBranchAddress("yield_ramp",&yield_ramp_hw_sum,&b_yield_ramp);
  fChain->SetBranchAddress("ErrorFlag",&ErrorFlag, &b_ErrorFlag);
  fChain->SetBranchAddress("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, &b_yield_qwk_mdallbars);
  fChain->SetBranchAddress("asym_qwk_charge", &asym_qwk_charge_hw_sum, &b_asym_qwk_charge);

//   for(Int_t i = 0; i < (Int_t)(DetectorList.size()); i++){
//     HDetectorList.push_back(Form("asym_%s", DetectorList[i].Data()));
//     fChain->SetBranchStatus(Form("asym_%s", DetectorList[i].Data()), 1);
//     fChain->SetBranchAddress(Form("asym_%s", DetectorList[i].Data()),&HDetBranch[i]);
//   }
//    for(Int_t i = 0; i < (Int_t)(MonitorList.size()); i++){
//     HMonitorList.push_back(Form("%s%s", fPrefix[i].Data(), MonitorList[i].Data()));
//     fChain->SetBranchStatus(Form("%s%s", fPrefix[i].Data(), MonitorList[i].Data()), 1);
//     fChain->SetBranchAddress(Form("%s%s", fPrefix[i].Data(), MonitorList[i].Data()),&HMonBranch[i]);

//     YMonitorList.push_back(Form("yield_%s", MonitorList[i].Data()));
//     fChain->SetBranchStatus(Form("yield_%s", MonitorList[i].Data()), 1);
//     fChain->SetBranchAddress(Form("yield_%s", MonitorList[i].Data()),&YMonBranch[i]);

//     std::cout << "Monitors: " << HMonitorList[i] << "\t" << YMonitorList[i] << std::endl;
//    }
}

void QwMpsOnly::SetupMpsBranchAddress()
{

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("qwk_charge", 1);
   fChain->SetBranchStatus("qwk_charge_Device_Error_Code", 1);
   fChain->SetBranchStatus("bm_pattern_number", 1);
   //   fChain->SetBranchStatus("mps_counter", 1);
   fChain->SetBranchStatus("ErrorFlag", 1);
   fChain->SetBranchStatus("ramp", 1);
   fChain->SetBranchStatus("fgx1", 1);
   fChain->SetBranchStatus("fgx2", 1);
   //   fChain->SetBranchStatus("fge", 1);
   fChain->SetBranchStatus("fgy2", 1);
   fChain->SetBranchStatus("fgy1", 1);

   fChain->SetBranchAddress("qwk_charge", &qwk_charge_hw_sum);
//    fChain->SetBranchAddress("qwk_charge_Device_Error_Code", &qwk_charge_Device_Error_Code, &b_qwk_charge_Device_Error_Code);
   fChain->SetBranchAddress("bm_pattern_number", &bm_pattern_number);
   //   fChain->SetBranchAddress("mps_counter", &event_number, &b_event_number);
   fChain->SetBranchAddress("ErrorFlag", &ErrorFlag);
   fChain->SetBranchAddress("fgx1", &fgx1_hw_sum);
   fChain->SetBranchAddress("fgy1", &fgy1_hw_sum);
   fChain->SetBranchAddress("fgx2", &fgx2_hw_sum);
   fChain->SetBranchAddress("fgy2", &fgy2_hw_sum);
   //   fChain->SetBranchAddress("fge", &fge_hw_sum, &b_fge);
   fChain->SetBranchAddress("ramp", &ramp_hw_sum);
   fChain->SetBranchAddress("ramp_block0", &ramp_block0);
   fChain->SetBranchAddress("ramp_block1", &ramp_block1);
   fChain->SetBranchAddress("ramp_block2", &ramp_block2);
   fChain->SetBranchAddress("ramp_block3", &ramp_block3);

}

void QwMpsOnly::SetPhaseValues(Double_t *val)
{

  phase.resize(5);
//   std::cout << other << "Default phase information:\t" << normal << std::endl;
  for(Int_t i = 0; i < fNModType; i++){
    phase[i] = val[i];
//     std::cout << other << phase[i] << normal << std::endl;
  }

  return;
}


void QwMpsOnly::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}



void QwMpsOnly::Write(){
  //*********************************************
  //
  //  Slopes.dat is really a deprecated file 
  //  at this point.  I will remove it soon.
  //
  //********************************************

  gSystem->Exec("umask 002");

  slopes.open(Form("%s/slopes/slopes%s_%i.%s.dat", output.Data(), fFileSegment.Data(), run_number, fSetStem.Data()) , fstream::out);
  regression = fopen(Form("%s/regression/regression%s_%i.%s.dat", output.Data(), fFileSegment.Data(), run_number, fSetStem.Data()), "w");

  if( (slopes.is_open() && slopes.good()) ){
    for(Int_t i = 0; i < fNDetector; i++){
      slopes << "det " << DetectorList[i] << std::endl;
      for(Int_t j = 0; j < fNModType; j++){
	slopes << YieldSlope[i][j] << "\t"
	       << YieldSlopeError[i][j] << std::endl;
      }
    }
  }

  else{
    std::cout << red << "Error opening slopes.dat" << normal << std::endl;
    std::cout << other << "Cleaning up output files....." << normal << std::endl;
    CleanFolders();
    exit(1);
  }

  if( (diagnostic.is_open() && diagnostic.good()) ){
    diagnostic << "\n#Modulation events in x,xp,e,y,yp\n" << std::endl;
    for(Int_t i = 0; i < fNModType; i++){
      diagnostic << ModulationEvents[i]  << std::endl;
      fNModEvents += ModulationEvents[i];
    }
    diagnostic << fNModEvents << std::endl;
  }


  //***************************************************************
  // Write output file for regression - This will go into the DB.  
  // All that is included here is the slopes(and errors soon...),
  // qwlibra must be run to get other run info.
  //***************************************************************

  fprintf(regression, "#run=%i\tevents=%i\n", run_number, fNModEvents);
  if( regression != NULL ){
    for(Int_t i = 0; i < fNDetector; i++){
      for(Int_t j = 0; j < fNModType; j++){
	fprintf(regression, "%s/%s : %i : %i : %e : %e\n", DetectorList[i].Data(), MonitorList[j].Data(), 
		ModulationEvents[j], j, YieldSlope[i][j], YieldSlopeError[i][j]);
      }
    }
  }

  else{
    std::cout << red << "Error opening regression.dat" << normal << std::endl;
    std::cout << other << "Cleaning up output files....." << normal << std::endl;
    gSystem->Exec(Form("rm -rf regression_%i", run_number));
    exit(1);
  }

  fclose(regression);
  slopes.close();
  diagnostic.close();
  
  return;
}

#endif

//  LocalWords:  SetupMpsBranchAddress
