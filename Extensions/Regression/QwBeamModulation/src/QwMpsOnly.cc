#define QwMpsOnly_cxx
#include "QwMpsOnly.hh"
#include "headers.h"
#include <functional> 
#include <TLeaf.h>
#include <TMatrixDSym.h>
#include <TMatrixDEigen.h>
#include <map>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#ifdef QwMpsOnly_cxx

const char QwMpsOnly::red[8] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
const char QwMpsOnly::other[8] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
const char QwMpsOnly::normal[8] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

//const Double_t fUnitConvert[5] = {1., 1.e6, 1., 1., 1.e6};

QwMpsOnly::QwMpsOnly(TChain *tree)
{
  fXModulation  = 0;
  fYModulation  = 3;
  fEModulation  = 2;
  fXPModulation = 1; 
  fYPModulation = 4;
  fUseDataTertile = 0;
  fNEvents = 0;
  fFirstEntry = 0;
  fLastEntry = 0;
  fMacroCycleNum = 0;
  fCutNonlinearRegions = 0;
  for(Int_t imod = 0;imod<kNMod;imod++)
    fCycleNum[imod] = 0;
  fReduceMatrix_x  = 0;
  fReduceMatrix_y  = 0;
  fReduceMatrix_xp = 0;
  fReduceMatrix_yp = 0; 
  fReduceMatrix_e  = 0; 
  fSensHumanReadable = 0;
  fDegPerMPS = 0;
  fPedestal = 0; 
  fNCoil = 10; 
  fNModEvents = 0; 
  fCurrentCut = 40; 
  fPreviousRampValue = -1.;
  fMaxRampNonLinearity = 10.;
  fFractionalDetectorSlopes = true;
  fMakeFriendTree = false;
  fTransverseData = false;
  fFullCycle = false;
  fXinit  = false; 
  fYinit  = false; 
  fEinit  = false; 
  fXPinit = false; 
  fYPinit = false; 
  fSingleCoil = false; 
  fRunNumberSet = false; 
  fPhaseConfig = false;
  fFileSegmentInclude = false;
  f2DFit = false;//use MINUIT to find coefficients--very slow
  fChiSquareMinimization = false;
  fFileSegment = ""; 
  fFileStem =  "mps_only"; 
  fSetStem = "std"; 
  configFileName = Form("%s/config/setup_mpsonly.config",
			gSystem->Getenv("BMOD_SRC"));
  fLowRamp = 0;
  fHighRamp = 360;
  fNewEbpm = 0;
  Init(tree);
}

QwMpsOnly::~QwMpsOnly()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

int QwMpsOnly::AddFriendTree()
{
  TFriendElement *f;
  f = fChain->AddFriend("mps_slug", Form("%s/%smps_only_friend_%i.root",
					 gSystem->Getenv("MPS_ONLY_ROOTFILES"),
					 (fTransverseData ? "transverse/" : ""),
					 run_number));
  fChain->SetBranchStatus("ramp_filled",1);
  if(fChain->GetBranchStatus("ramp_filled")==0){
    std::cout<<"Warning. Failed to add friend tree.\n";
    return 1;
  }else
    return 0;

}

void QwMpsOnly::BuildCoilData()
{
  for(Int_t i = 0; i < kNMod; i++){
    CoilData_copy.push_back(std::vector <Double_t>());
    CoilData.push_back(std::vector <Double_t>());
  }
  std::cout << "Coil block size: " << CoilData.size() << std::endl;
  
  return;
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
  for(Int_t i = 0; i < kNCoil; i++){
    AvDetectorSlope.push_back(std::vector <Double_t>());
    AvDetectorSlopeError.push_back(std::vector <Double_t>());
  }
  std::cout << "Average Detector vector size: " << AvDetectorSlope.size() << 
    std::endl;
  return;
}

void QwMpsOnly::BuildDetectorSlopeVector()
{
  // First 5 hold Sine components for each Mod Type
  // modType+5 holds Cosine Component 
  DetectorData_copy.resize(kNMod);
  DetectorSlope.resize(kNCoil);
  DetectorSlopeError.resize(kNCoil);
  for(Int_t i = 0; i < kNCoil; i++){
    if(i<kNMod)
      DetectorData_copy[i].resize(fNDetector);
    DetectorSlope[i].resize(fNDetector);
    DetectorSlopeError[i].resize(fNDetector);
  }
  return;
}

void QwMpsOnly::BuildDetectorResidualVector()
{
  // First 5 hold Sine components for each Mod Type
  // modType+5 holds Cosine Component 
  DetectorSinResidual.resize(kNMod);
  DetectorSinResidualError.resize(kNMod);
  DetectorCosResidual.resize(kNMod);
  DetectorCosResidualError.resize(kNMod);
  for(Int_t i = 0; i < kNMod; i++){
    DetectorSinResidual[i].resize(fNDetector);
    DetectorSinResidualError[i].resize(fNDetector);
    DetectorCosResidual[i].resize(fNDetector);
    DetectorCosResidualError[i].resize(fNDetector);
  }
  return;
}

void QwMpsOnly::BuildMonitorAvSlope()
{
  for(Int_t i = 0; i < kNCoil; i++){
    AvMonitorSlope.push_back(std::vector <Double_t>());
    AvMonitorSlopeError.push_back(std::vector <Double_t>());
  }
  std::cout << "Average Monitor vector size: " << AvMonitorSlope.size() << 
    std::endl;
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
  MonitorData_copy.resize(kNMod);
  MonitorSlope.resize(kNCoil);
  MonitorSlopeError.resize(kNCoil);
  for(Int_t i = 0; i < kNCoil; i++){
    if(i<kNMod)
      MonitorData_copy[i].resize(fNMonitor);
    MonitorSlope[i].resize(fNMonitor);
    MonitorSlopeError[i].resize(fNMonitor);
  }
  return;
}

void QwMpsOnly::BuildNewEBPM(){
  
  //create a new friendable tree
  TFile *newfile = new TFile(Form("%s/%smps_only_newEbpm_%i.root",
				  gSystem->Getenv("MPS_ONLY_ROOTFILES"),
				  (fTransverseData ? "transverse/" : ""),
				  run_number),"recreate");
  TTree *newTree = new TTree("mps_slug", "mps_slug"); 

  Double_t newEnergyBPM=0;
  b_newEbpm = newTree->Branch("newEbpm", &newEnergyBPM, "newEbpm/D");

  Double_t alpha[] = {1.47521,-57239.6,1,-0.0423589,-4131.94}; // X,XP,E,Y,YP in 
                                                               // setup.config
  
  for(Int_t i=0;i<fChain->GetEntries();i++){

    newEnergyBPM=0;
    fChain->GetEntry(i);

    // construct new energy bpm
    for(int inew=0;inew<5;inew++){

      newEnergyBPM += alpha[inew]*(fChain->GetLeaf(MonitorList[inew].Data())->
				   GetValue());

    }

    newTree->Fill();
    if(i%10000==0)std::cout<<"Processing entry "<<i<<".\n";
  }
  newTree->Print();
  newfile->Write("",TObject::kOverwrite);
  //  newfile->Close();
}

void QwMpsOnly::Calculate2DSlope(Int_t modType, Int_t makePlots)
{
  if(!fPhaseConfig){
    Double_t temp[5]={0.,0.,0.,0.,0.};//{0.26, 0.26, 0.0, 1.08, 1.08};         
    SetPhaseValues(temp); 
  }
  Int_t minFullBins = 10;
  Int_t minEntriesPerBin = 10;
  //  Int_t minEntries = 100;
  Int_t minEntries = (fUseDataTertile!=0 ? 3850 : 100);
  Double_t sigma_cc = 0;
  Double_t sigma_dc = 0;
  Double_t d_mean = 0;
  Double_t c_mean = 0;
  Double_t slope = 0;
  Double_t sigma_cc2 = 0;
  Double_t sigma_dc2 = 0;
  Double_t c_mean2 = 0;
  Double_t slope2 = 0;
  Double_t slopeSin = 0, slopeSinErr = 0;
  Double_t slopeCos = 0, slopeCosErr = 0;
 
  fCycleNum[modType]++;
  if(fNEvents < minEntries){
    std::cout<< red << "Number of good events (" << fNEvents 
	     <<") too small in microcycle. Exiting."<< normal << std::endl;
    return;
  }
  
  if(CoilData[modType].size() <= 0){
    std::cout << "!!!!!!!!!!!!!!!!! Illegal Coil vector length:\t" 
	      << CoilData[modType].size() << std::endl;
    return;
  }
  
  //This tool allows one to use only one third of the data in a given cycle
  //It is useful for comparing if the sensitivities are constant over a 
  //modulation cycle. This is assumed to be true for modulation corrections to 
  //be correct.
  bool local_debug = 0;
  if(fUseDataTertile>0 && fUseDataTertile<4){
    int start = int(fNEvents / 3.0) * (fUseDataTertile - 1);
    int end = start + int(fNEvents / 3.0);
    int size = end - start;
    std::cout<<"Using tertile "<<fUseDataTertile<<" events "<<start<<" to "<<
      end<<".\n";
    if(local_debug)
      std::cout<<"CoilData before: "<<CoilData[modType][start]<<" "
	       <<CoilData[modType][end-1]<<std::endl;
    for(int i=0;i<size && fUseDataTertile>1;++i){
      CoilData[modType][i] = CoilData[modType].at(i+start);
    }
    CoilData[modType].resize(size);
    if(local_debug){
      std::cout<<"CoilData after: "<<CoilData[modType][0]<<" "
	       <<CoilData[modType][size-1]<<std::endl;

      std::cout<<"MonitorData before: "<<MonitorData[0][start]<<" "
	       <<MonitorData[0][end-1]<<std::endl;
    }
    for(int imon=0;imon<fNMonitor;++imon){
      for(int i=0;i<size && fUseDataTertile>1;++i)
	MonitorData[imon][i] = MonitorData[imon].at(i+start);
      MonitorData[imon].resize(size);
    }
    if(local_debug){
      std::cout<<"MonitorData after: "<<MonitorData[0][0]<<" "
	       <<MonitorData[0][size-1]<<std::endl;
      
      std::cout<<"DetectorData before: "<<DetectorData[0][start]<<" "
	       <<DetectorData[0][end-1]<<std::endl;
    }
    for(int idet=0;idet<fNDetector;++idet){
      for(int i=0;i<size && fUseDataTertile>1;++i)
	DetectorData[idet][i] =  DetectorData[idet].at(i+start);
      DetectorData[idet].resize(size);
    }
    if(local_debug)
      std::cout<<"DetectorData after: "<<DetectorData[0][0]<<" "
	       <<DetectorData[0][size-1]<<std::endl;

    fNEvents = size;
  }
  ////////////////////////////////////////


  //*******************************
  ModulationEvents[modType] += fNEvents;
  //*******************************

  // 2D Sin+Cos+Const fit.
  TF1* fctn = new TF1("fctn", Form("[0] + [1] * TMath::Sin( %e * x) + "
				   "[2] * TMath::Cos(%e * x)", kDegToRad, 
				   kDegToRad), fLowRamp, fHighRamp); 
  fctn->SetParNames("Const.", "A_s", "A_c");
  fctn->SetLineColor(kBlack);
  
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);
  
  // Arrays for all fits. 
  Double_t *x, *y;
  x = new Double_t[fNEvents];			
  y = new Double_t[fNEvents];				
  c_mean=0;
  c_mean2=0;
  for(Int_t evNum=0; evNum<fNEvents; evNum++) { 
    x[evNum] = CoilData[modType][evNum] + phase[modType]/kDegToRad;
    if(x[evNum] >= 360.0){
      std::cout<<"Degrees greater than 360. Resetting "<<x[evNum]<<" to "<<
	x[evNum]-360<<std::endl;;
      x[evNum] -= 360.0;
    }
    c_mean += TMath::Sin(kDegToRad*x[evNum]);	
    c_mean2 += TMath::Cos(kDegToRad*x[evNum]);	
  }
  c_mean /= fNEvents;
  c_mean2 /= fNEvents;

  // Variables to hold fit parameters.
  Double_t mean = 0, meanError = 0;
  Double_t sinAmp = 0, sinAmpError = 0; 
  Double_t cosAmp = 0, cosAmpError = 0;  

  char  *ModName[modType];
  ModName[0] = "X";
  ModName[1] = "Y";
  ModName[2] = "E";
  ModName[3] = "XP";
  ModName[4] = "YP"; 

     
  //////////////
  // Monitors //
  //////////////
  TCanvas *cM;
  if(makePlots){
    cM = new TCanvas("cM", "canvas", 1200, 800);
    cM->Divide(3,2);
  }
  
  TGraph mongr[fNMonitor];
  //Write sensitivities to coils to file as they are determined
   if(coil_sens.is_open()&&coil_sens.good()){
     coil_sens << Form("Coil %i                     Constant       Error"
		       "            Sine           Error            Cosine"
		       "         Error          ChiSq/NDF          nEvents\n",
		       modType);
  }
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

    for(Int_t evNum=0; evNum<fNEvents; ++evNum) {
      y[evNum] = MonitorData[mon][evNum];
      d_mean += y[evNum];     
    }
    d_mean /= (Double_t)fNEvents;
    
    for(Int_t evNum=0; evNum<fNEvents; ++evNum) {
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
    mongr[mon] = TGraph(fNEvents, x, y);
    fctn->SetParameters(d_mean, slope, slope2);
    mongr[mon].Fit("fctn", "QRMN0");

    if(makePlots){
      cM->cd(mon+1);
      mongr[mon].SetMarkerColor(kBlue);
      mongr[mon].SetTitle(Form("%s vs. ramp", MonitorList[mon].Data()));
      mongr[mon].GetXaxis()->SetTitle("ramp");
      mongr[mon].GetXaxis()->SetTitle(Form("%s",MonitorList[mon].Data()));
      mongr[mon].Draw("AP");
      cM->ForceUpdate();
    }
    
    mean = fctn->GetParameter(0);
    meanError = fctn->GetParError(0);
    sinAmp = fctn->GetParameter(1);
    sinAmpError = fctn->GetParError(1);
    cosAmp = fctn->GetParameter(2);
    cosAmpError = fctn->GetParError(2);
//     printf("%s mean: %16.10e Guess Slopes %e  %e  Real Slopes %e  %e\n",
// 	   MonitorList[mon].Data(), d_mean, slope, slope2, sinAmp, cosAmp );
    
    slopeSin = sinAmp;
    slopeSinErr = sinAmpError;
    slopeCos = cosAmp;
    slopeCosErr = cosAmpError;

    MonitorMean[modType][mon] = mean;
    MonitorMeanError[modType][mon] = meanError;
    MonitorSlope[modType][mon].push_back(slopeSin);
    MonitorSlopeError[modType][mon].push_back(slopeSinErr);
    MonitorSlope[modType+5][mon].push_back(slopeCos);
    MonitorSlopeError[modType+5][mon].push_back(slopeCosErr);
 
    if(coil_sens.is_open()&&coil_sens.good()){
      coil_sens << MonitorList[mon].Data() <<
	(MonitorList[mon].Contains("Slope") ? "\t" : "\t\t")<<
	Form("%+13.6e  %11.4e ",mean,meanError)<<"\t"<<
	Form("%+13.6e  %11.4e ",slopeSin,slopeSinErr)<<"\t"<<
	Form("%+13.6e  %11.4e\t %f\n",slopeCos,slopeCosErr, 
	     fctn->GetChisquare()/fctn->GetNDF());
    }else printf("Coil sensitivities file not opened. Cannot write data.\n");
  }
  
  if(makePlots){
    cM->cd(6);
    TPaveText pt1 = TPaveText(0.05, 0.05, 0.8, 0.8);
    pt1.AddText(Form("%s Modulation", ModName[modType]));
    pt1.SetFillColor(43);
    pt1.Draw();
    gPad->Update();
    cM->cd();
    cM->Print(Form("MonitorPlot%i.png",modType));
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
  
  TGraph detgr[fNDetector];
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
    d_mean /= (Double_t)fNEvents;
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
    if(fNEvents < minFullBins * minEntriesPerBin){
      std::cout<< red << "Number of good events too small in microcycle."<<
	" Exiting."<< normal << std::endl;
      return;
    }
    detgr[det] = TGraph(fNEvents, x, y);
    fctn->SetParameters(d_mean, slope, slope2);
    detgr[det].Fit("fctn", "QRMN0");
    if(makePlots){
      cD->cd(det+1);
      detgr[det].SetMarkerColor(kBlue);
      detgr[det].SetTitle(Form("%s vs. ramp", DetectorList[det].Data()));
      detgr[det].GetXaxis()->SetTitle("ramp");
      detgr[det].GetXaxis()->SetTitle(Form("%s", DetectorList[det].Data()));
      detgr[det].Draw("AP");
      cD->ForceUpdate();
    }
    
    mean = TMath::Abs(fctn->GetParameter(0));        
    meanError = (fctn->GetParError(0));
    DetectorMean[modType][det] = mean;
    DetectorMeanError[modType][det] = meanError;
    if(!fFractionalDetectorSlopes){
      mean = 1.0;
      meanError = 0;
    }

    sinAmp = (fctn->GetParameter(1));
    sinAmpError = (fctn->GetParError(1));
    cosAmp = (fctn->GetParameter(2));
    cosAmpError = (fctn->GetParError(2));
    slopeSin = sinAmp/mean;
    slopeSinErr = TMath::Abs(sinAmp/mean) *
       TMath::Sqrt(TMath::Power(sinAmpError/sinAmp,2) +
		   TMath::Power(meanError/mean,2));
    slopeCos = cosAmp/mean;
    slopeCosErr = TMath::Abs(cosAmp/mean) * 
      TMath::Sqrt(TMath::Power(cosAmpError/cosAmp,2) +
		  TMath::Power(meanError/mean,2));

//     printf("%s mean: %16.10e Guess Slopes %e  %e  Real Slopes %e  %e\n",
// 	   DetectorList[det].Data(), d_mean, slope, slope2, slopeSin, slopeCos );

    DetectorSlope[modType][det].push_back(slopeSin);
    DetectorSlopeError[modType][det].push_back(slopeSinErr);
    DetectorSlope[modType+5][det].push_back(slopeCos);
    DetectorSlopeError[modType+5][det].push_back(slopeCosErr);

    //Write sensitivities to coils to file
    if(coil_sens.is_open()&&coil_sens.good()){
      coil_sens << DetectorList[det].Data() <<"\t\t"<<
	Form("%+13.6e  %11.4e ",mean,meanError)<<"\t"<<
	Form("%+13.6e  %11.4e \t", slopeSin,slopeSinErr)<<
	Form("%+13.6e  %11.4e\t %f\t %i\n",slopeCos,slopeCosErr, 
	     fctn->GetChisquare()/fctn->GetNDF(), fNEvents);
    }    
  }

  if(makePlots){
    cD->cd(30);
    TPaveText pt2 = TPaveText(0.05, 0.05, 0.8, 0.8);
    pt2.AddText(Form("%s Modulation", ModName[modType]));
    pt2.SetFillColor(43);
    pt2.Draw();
    cD->cd();
    cD->Print(Form("DetectorPlot%i.png",modType));
    delete cD;
  }

  //Copy data to storage vectors for future residual calculations
  CopyDataVectors(modType);
  
  // These need to be set so we know if we have a full set of modulation data
  if(modType == fXModulation)  fXinit = true;
  if(modType == fYModulation)  fYinit = true;
  if(modType == fEModulation)  fEinit = true;
  if(modType == fXPModulation) fXPinit = true;
  if(modType == fYPModulation) fYPinit = true;

//   printf("Coil %i: %i entries\n", modType, DetectorData[0].size());
//   if(fXinit && fYinit && fEinit && fXPinit &&fYPinit) printf("\n\n");

  delete [] x;
  delete [] y;
  if(coil_sens.is_open()&&coil_sens.good())
    coil_sens <<"\n";
  return;
}

void QwMpsOnly::CalculateCoefficients(Int_t modType)
{
  //More efficient version of Calculate2DSlope() using linear algebra instead 
  //of Minuit fits to find sine and cosine coefficients
  if(!fPhaseConfig){
    Double_t temp[5]={0.,0.,0.,0.,0.};//{0.26, 0.26, 0.0, 1.08, 1.08};         
    SetPhaseValues(temp); 
  }
  Int_t minEntries = (fUseDataTertile!=0 ? 3850 : 100);
 
  fCycleNum[modType]++;
  if(fNEvents < minEntries){
    std::cout<< red << "Number of good events (" << fNEvents 
	     <<") too small in microcycle. Exiting."<< normal << std::endl;
    return;
  }
  
  if(CoilData[modType].size() <= 0){
    std::cout << "!!!!!!!!!!!!!!!!! Illegal Coil vector length:\t" 
	      << CoilData[modType].size() << std::endl;
    return;
  }
  
  //This tool allows one to use only one third of the data in a given cycle
  //It is useful for comparing if the sensitivities are constant over a 
  //modulation cycle. This is assumed to be true for modulation corrections to 
  //be correct.
  bool local_debug = 0;
  if(fUseDataTertile>0 && fUseDataTertile<4){
    int start = int(fNEvents / 3.0) * (fUseDataTertile - 1);
    int end = start + int(fNEvents / 3.0);
    int size = end - start;
    std::cout<<"Using tertile "<<fUseDataTertile<<" events "<<start<<" to "<<
      end<<".\n";
    if(local_debug)
      std::cout<<"CoilData before: "<<CoilData[modType][start]<<" "
	       <<CoilData[modType][end-1]<<std::endl;
    for(int i=0;i<size && fUseDataTertile>1;++i){
      CoilData[modType][i] = CoilData[modType].at(i+start);
    }
    CoilData[modType].resize(size);
    if(local_debug){
      std::cout<<"CoilData after: "<<CoilData[modType][0]<<" "
	       <<CoilData[modType][size-1]<<std::endl;

      std::cout<<"MonitorData before: "<<MonitorData[0][start]<<" "
	       <<MonitorData[0][end-1]<<std::endl;
    }
    for(int imon=0;imon<fNMonitor;++imon){
      for(int i=0;i<size && fUseDataTertile>1;++i)
	MonitorData[imon][i] = MonitorData[imon].at(i+start);
      MonitorData[imon].resize(size);
    }
    if(local_debug){
      std::cout<<"MonitorData after: "<<MonitorData[0][0]<<" "
	       <<MonitorData[0][size-1]<<std::endl;
      
      std::cout<<"DetectorData before: "<<DetectorData[0][start]<<" "
	       <<DetectorData[0][end-1]<<std::endl;
    }
    for(int idet=0;idet<fNDetector;++idet){
      for(int i=0;i<size && fUseDataTertile>1;++i)
	DetectorData[idet][i] =  DetectorData[idet].at(i+start);
      DetectorData[idet].resize(size);
    }
    if(local_debug)
      std::cout<<"DetectorData after: "<<DetectorData[0][0]<<" "
	       <<DetectorData[0][size-1]<<std::endl;

    fNEvents = size;
  }
  ////////////////////////////////////////


  //*******************************
  ModulationEvents[modType] += fNEvents;
  //*******************************



     
  ///////////////////////////////////////////////
  //Accumulate data and calculate coefficients //
  ///////////////////////////////////////////////
 
  double X00, X10, X01, X11, X20, X02;
  X00 = X10 = X01 = X11 = X20 = X02 = 0;
  std::map<TString, double> Y00, Y10, Y01;
  std::map<TString, double> Z00;
  for (int imon = 0; imon < (int)MonitorList.size(); ++imon) {
    Y00[MonitorList[imon]] = 0;
    Y01[MonitorList[imon]] = 0;
    Y10[MonitorList[imon]] = 0;
    Z00[MonitorList[imon]] = 0;
  }
  for (int idet = 0; idet < (int)DetectorList.size(); ++idet) {
    Y00[DetectorList[idet]] = 0;
    Y01[DetectorList[idet]] = 0;
    Y10[DetectorList[idet]] = 0;
    Z00[DetectorList[idet]] = 0;
  }

  for (int i=0; i < (int)CoilData[modType].size(); ++i) {
    double costheta = cos(CoilData[modType][i] * kDegToRad);
    double sintheta = sin(CoilData[modType][i] * kDegToRad);

    X00 += 1;
    X10 += costheta;
    X01 += sintheta;
    X11 += costheta * sintheta;
    X20 += costheta * costheta;
    X02 += sintheta * sintheta;
    for (int imon = 0; imon < (int)MonitorList.size(); ++imon) {
      double y = MonitorData[imon][i];
      Y00[MonitorList[imon]] += y;
      Y10[MonitorList[imon]] += y * costheta;
      Y01[MonitorList[imon]] += y * sintheta;
      Z00[MonitorList[imon]] += y * y;
    }
    for (int idet = 0; idet < (int)DetectorList.size(); ++idet) {
      double y = DetectorData[idet][i];
      Y00[DetectorList[idet]] += y;
      Y10[DetectorList[idet]] += y * costheta;
      Y01[DetectorList[idet]] += y * sintheta;
      Z00[DetectorList[idet]] += y * y;
    }
  } 
  TMatrixDSym M(3);
  M(0,0) = X20;
  M(0,1) = M(1,0) = X11;
  M(0,2) = M(2,0) = X10;
  M(1,1) = X02;
  M(1,2) = M(2,1) = X01;
  M(2,2) = X00;
  TMatrixDSym MI(M);
  double determ;
  MI.Invert(&determ);

  
  ///////////////
  // Monitors //
  ///////////////
  for (int imon = 0; imon < (int)MonitorList.size(); ++imon) {
    double B[3];
    B[0] = Y10[MonitorList[imon]];
    B[1] = Y01[MonitorList[imon]];
    B[2] = Y00[MonitorList[imon]];
    double c_cos = MI(0,0) * B[0] + MI(0,1) * B[1] + MI(0,2) * B[2];
    double c_sin = MI(1,0) * B[0] + MI(1,1) * B[1] + MI(1,2) * B[2];
    double c_cons = MI(2,0) * B[0] + MI(2,1) * B[1] + MI(2,2) * B[2];
    double chisqr = Z00[MonitorList[imon]]
      - 2 * (c_cos * B[0] + c_sin * B[1] + c_cons * B[2])
      + c_cos * c_cos * M(0,0) + c_sin * c_sin * M(1,1)
      + c_cons * c_cons * M(2,2)
      + 2 * (c_cos * c_sin * M(0,1) 
	     + c_cos * c_cons * M(0,2)
	     + c_sin * c_cons * M(1,2));
    double varresid = chisqr / (X00 - 3);
    double c_cos_err = sqrt(MI(0,0) * varresid);
    double c_sin_err = sqrt(MI(1,1) * varresid);
    double c_cons_err = sqrt(MI(2,2) * varresid);
    //std::cout<<MonitorList[imon].Data()<<" "<<c_sin<<" "<<c_sin_err<<" "
    //	     <<c_cos<<" "<<c_cos_err<<" "<<c_cons<<" "<<c_cons_err<<std::endl;
    MonitorMean[modType][imon] = c_cons;
    MonitorMeanError[modType][imon] = c_cons_err;
    MonitorSlope[modType][imon].push_back(c_sin);
    MonitorSlopeError[modType][imon].push_back(c_sin_err);
    MonitorSlope[modType+5][imon].push_back(c_cos);
    MonitorSlopeError[modType+5][imon].push_back(c_cos_err);

  }
   
  ///////////////
  // Detectors //
  ///////////////
  for (int idet = 0; idet < (int)DetectorList.size(); ++idet) {
    double B[3];
    B[0] = Y10[DetectorList[idet]];
    B[1] = Y01[DetectorList[idet]];
    B[2] = Y00[DetectorList[idet]];
    double c_cos = MI(0,0) * B[0] + MI(0,1) * B[1] + MI(0,2) * B[2];
    double c_sin = MI(1,0) * B[0] + MI(1,1) * B[1] + MI(1,2) * B[2];
    double c_cons = MI(2,0) * B[0] + MI(2,1) * B[1] + MI(2,2) * B[2];
    double chisqr = Z00[DetectorList[idet]]
      - 2 * (c_cos * B[0] + c_sin * B[1] + c_cons * B[2])
      + c_cos * c_cos * M(0,0) + c_sin * c_sin * M(1,1)
      + c_cons * c_cons * M(2,2)
      + 2 * (c_cos * c_sin * M(0,1) 
	     + c_cos * c_cons * M(0,2)
	     + c_sin * c_cons * M(1,2));
    double varresid = chisqr / (X00 - 3);
    double c_cos_err = sqrt(MI(0,0) * varresid);
    double c_sin_err = sqrt(MI(1,1) * varresid);
    double c_cons_err = sqrt(MI(2,2) * varresid);
    DetectorMean[modType][idet] = c_cons;
    DetectorMeanError[modType][idet] = c_cons_err;
    double abs_cons = TMath::Abs(c_cons);
    if(fFractionalDetectorSlopes){
      DetectorSlope[modType][idet].push_back(c_sin/abs_cons);
      double s_err = TMath::Abs(c_sin / c_cons) *
	sqrt(pow(c_sin_err/c_sin,2) + pow(c_cons_err/c_cons,2));
      DetectorSlopeError[modType][idet].push_back(s_err);
      DetectorSlope[modType+5][idet].push_back(c_cos/abs_cons);
      double c_err = TMath::Abs(c_cos / c_cons) *
	sqrt(pow(c_cos_err/c_cos,2) + pow(c_cons_err/c_cons,2));
      DetectorSlopeError[modType+5][idet].push_back(c_err);
    }else{
      DetectorSlope[modType][idet].push_back(c_sin);
      DetectorSlopeError[modType][idet].push_back(c_sin_err);
      DetectorSlope[modType+5][idet].push_back(c_cos);
      DetectorSlopeError[modType+5][idet].push_back(c_cos_err);
    }
    //    std::cout<<DetectorList[idet].Data()<<" "<<c_sin/abs_cons<<" "
    //<<s_err<<" "<<c_cos/abs_cons<<" "<<c_err<<" "<<c_cons<<" "
    //<<c_cons_err<<std::endl;
  }

  //Copy data to storage vectors for future residual calculations
  CopyDataVectors(modType);
   
  // These need to be set so we know if we have a full set of modulation data
  if(modType == fXModulation)  fXinit = true;
  if(modType == fYModulation)  fYinit = true;
  if(modType == fEModulation)  fEinit = true;
  if(modType == fXPModulation) fXPinit = true;
  if(modType == fYPModulation) fYPinit = true;

  return;
}

void QwMpsOnly::CalculateResiduals()
{

  //////////////////////////////////////////////////////
  //Use slopes to correct data and find the residuals //
  //////////////////////////////////////////////////////

  //Correction using fractional slopes:
  //corrected_det = det/mean_det*(1-mean_det/det*sum(slope*(mon-mon_mean))
  //Data normalized to approximately unit mean
  for(int imod=0;imod<kNMod;++imod){
    if(fFractionalDetectorSlopes){
      for(int i=0;i<(int)DetectorData_copy[imod][0].size();++i){
	for(int idet=0;idet<fNDetector;++idet){
	  DetectorData_copy[imod][idet][i] /= DetectorMean[imod][idet];
	  double correction = 0;
	  for(int imon=0;imon<fNMonitor;++imon){
	    double slope = YieldSlope[idet][imon] / fUnitConvert[imon];
	    correction += (slope / DetectorData_copy[imod][idet][i]) *
	      (MonitorData_copy[imod][imon][i] - MonitorMean[imod][imon]);
// 	    std::cout<<"Correction "<<MonitorList[imon].Data()<<" "
// 		     <<YieldSlope[idet][imon]<<"/"
// 		     <<fUnitConvert[imon]<<"/"<<DetectorData_copy[imod][idet][i]
// 		     <<"*("<<MonitorData_copy[imod][imon][i]<<"-"
// 		     <<MonitorMean[imod][imon]<<")\n";
	  }
	  DetectorData_copy[imod][idet][i] *= (1 - correction);
	  // 	if(idet==0&&i%100==0)
	  // 	  std::cout<<"h "<<DetectorData_copy[imod][idet][i]<<std::endl;
	}
      }
    }  
    //Correction using absolute slopes:
    //corrected_det = det-sum(slope*(mon-mon_mean))
    else{
      for(int i=0;i<(int)DetectorData_copy[imod][0].size();++i){
	for(int idet=0;idet<fNDetector;++idet){
	  for(int imon=0;imon<fNMonitor;++imon){
	    double slope = YieldSlope[idet][imon] / fUnitConvert[imon];
	    DetectorData_copy[imod][idet][i] -= slope * 
	      (MonitorData_copy[imod][imon][i] - MonitorMean[imod][imon]);
	  }
	}
      }
    }

    double X00, X10, X01, X11, X20, X02;
    X00 = X10 = X01 = X11 = X20 = X02 = 0;
    std::map<TString, double> Y00, Y10, Y01;
    std::map<TString, double> Z00;
    for (int imon = 0; imon < (int)MonitorList.size(); ++imon) {
      Y00[MonitorList[imon]] = 0;
      Y01[MonitorList[imon]] = 0;
      Y10[MonitorList[imon]] = 0;
      Z00[MonitorList[imon]] = 0;
    }
    for (int idet = 0; idet < (int)DetectorList.size(); ++idet) {
      Y00[DetectorList[idet]] = 0;
      Y01[DetectorList[idet]] = 0;
      Y10[DetectorList[idet]] = 0;
      Z00[DetectorList[idet]] = 0;
    }

    for (int i=0; i < (int)CoilData_copy[imod].size(); ++i) {
      double costheta = cos(CoilData_copy[imod][i] * kDegToRad);
      double sintheta = sin(CoilData_copy[imod][i] * kDegToRad);

      X00 += 1;
      X10 += costheta;
      X01 += sintheta;
      X11 += costheta * sintheta;
      X20 += costheta * costheta;
      X02 += sintheta * sintheta;
      for (int imon = 0; imon < (int)MonitorList.size(); ++imon) {
	double y = MonitorData_copy[imod][imon][i];
	Y00[MonitorList[imon]] += y;
	Y10[MonitorList[imon]] += y * costheta;
	Y01[MonitorList[imon]] += y * sintheta;
	Z00[MonitorList[imon]] += y * y;
      }
      for (int idet = 0; idet < (int)DetectorList.size(); ++idet) {
	double y = DetectorData_copy[imod][idet][i];
	Y00[DetectorList[idet]] += y;
	Y10[DetectorList[idet]] += y * costheta;
	Y01[DetectorList[idet]] += y * sintheta;
	Z00[DetectorList[idet]] += y * y;
      }
    } 
    TMatrixDSym M(3);
    M(0,0) = X20;
    M(0,1) = M(1,0) = X11;
    M(0,2) = M(2,0) = X10;
    M(1,1) = X02;
    M(1,2) = M(2,1) = X01;
    M(2,2) = X00;
    TMatrixDSym MI(M);
    double determ;
    MI.Invert(&determ);

   
    ///////////////
    // Detectors //
    ///////////////
    for (int idet = 0; idet < (int)DetectorList.size(); ++idet) {
      double B[3];
      B[0] = Y10[DetectorList[idet]];
      B[1] = Y01[DetectorList[idet]];
      B[2] = Y00[DetectorList[idet]];
      double c_cos = MI(0,0) * B[0] + MI(0,1) * B[1] + MI(0,2) * B[2];
      double c_sin = MI(1,0) * B[0] + MI(1,1) * B[1] + MI(1,2) * B[2];
      double c_cons = MI(2,0) * B[0] + MI(2,1) * B[1] + MI(2,2) * B[2];
      double chisqr = Z00[DetectorList[idet]]
	- 2 * (c_cos * B[0] + c_sin * B[1] + c_cons * B[2])
	+ c_cos * c_cos * M(0,0) + c_sin * c_sin * M(1,1)
	+ c_cons * c_cons * M(2,2)
	+ 2 * (c_cos * c_sin * M(0,1) 
	       + c_cos * c_cons * M(0,2)
	       + c_sin * c_cons * M(1,2));
      double varresid = chisqr / (X00 - 3);
      double c_cos_err = sqrt(MI(0,0) * varresid);
      double c_sin_err = sqrt(MI(1,1) * varresid);
      double c_cons_err = sqrt(MI(2,2) * varresid);
      double abs_cons = TMath::Abs(c_cons);

      if(fFractionalDetectorSlopes){
	DetectorSinResidual[imod][idet].push_back(c_sin/abs_cons);
	double s_err = TMath::Abs(c_sin / c_cons) *
	  sqrt(pow(c_sin_err/c_sin,2) + pow(c_cons_err/c_cons,2));
	DetectorSinResidualError[imod][idet].push_back(s_err);

	DetectorCosResidual[imod][idet].push_back(c_cos/abs_cons);
	double c_err = TMath::Abs(c_cos / c_cons) *
	  sqrt(pow(c_cos_err/c_cos,2) + pow(c_cons_err/c_cons,2));
	DetectorCosResidualError[imod][idet].push_back(c_err);
      }else{
	DetectorSinResidual[imod][idet].push_back(c_sin);
	DetectorSinResidualError[imod][idet].push_back(c_sin_err);
	DetectorCosResidual[imod][idet].push_back(c_cos);
	DetectorCosResidualError[imod][idet].push_back(c_cos_err);
      }
      //    std::cout<<DetectorList[idet].Data()<<" "<<c_sin/abs_cons<<" "
      //<<s_err<<" "<<c_cos/abs_cons<<" "<<c_err<<" "<<c_cons<<" "
      //<<c_cons_err<<std::endl;
    }
  }
  return;
}
Int_t QwMpsOnly::CalculateWeightedSlope(Int_t verbose)
{

  Double_t mean = 0;
  Double_t mean_error = 0;
  Int_t nMod = kNMod; 
  for(Int_t i = 0; i < kNCoil; i++){
    AvDetectorSlope[i].clear();
    AvDetectorSlopeError[i].clear();
    for(Int_t j = 0; j < fNDetector; j++){
      mean = 0, mean_error = 0;
      for(Int_t k = 0; k < (Int_t)DetectorSlope[i][j].size(); k++){
	mean += ( DetectorSlope[i][j][k]
		  * TMath::Power(DetectorSlopeError[i][j][k],-2) );

	mean_error += TMath::Power(DetectorSlopeError[i][j][k],-2);
      }
      if(mean_error > 0){
	mean /= mean_error;
	AvDetectorSlope[i].push_back(mean);
	AvDetectorSlopeError[i].push_back(TMath::Sqrt(1.0/mean_error));
      }
      else{ 
	//Don't terminate loop if one detector fails.
	std::cout <<mean_error<< "\nUndefined "<<DetectorList[j].Data()<<
	  " Error Weighted Mean:= "<< mean << " +/- " << mean_error <<std::endl;
 	AvDetectorSlope[i].push_back(mean);
 	AvDetectorSlopeError[i].push_back(mean_error);

      }
    }
  }

  for(Int_t i = 0; i < nMod; i++){
    AvMonitorSlope[i].clear();
    AvMonitorSlopeError[i].clear();
    for(Int_t j = 0; j < fNMonitor; j++){
      mean = 0, mean_error = 0;
      for(Int_t k = 0; k < (Int_t)MonitorSlope[i][j].size(); k++){
	mean += ( MonitorSlope[i][j][k]
		  *TMath::Power(MonitorSlopeError[i][j][k],-2) );
	mean_error += TMath::Power(MonitorSlopeError[i][j][k],-2);
      }
      if(mean_error > 0){
	mean /= mean_error;
	AvMonitorSlope[i].push_back(mean);
	AvMonitorSlopeError[i].push_back(TMath::Sqrt(1.0/mean_error));

      }
      else{
	//Terminate loop if even one monitor fails since all are necessary.
	std::cout << "Error! Monitor Weighted Mean:= "<< mean << " / " 
		  << mean_error << std::endl; 
	AvMonitorSlope[i].push_back(mean);
	AvMonitorSlopeError[i].push_back(mean_error);
	return -1;
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
  return 0;
}


void QwMpsOnly::CheckFlags()
{

  if( !fFullCycle ){
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
  gSystem->Exec(Form("rm -rf %s/regression/regression_%i%s.%s.dat", 
		     output.Data(), run_number, 
		     (fChiSquareMinimization ? "_ChiSqMin" : ""),
		     fSetStem.Data()));
				   
  gSystem->Exec(Form("rm -rf %s/slopes/slopes_%i%s.%s.dat",
		     output.Data(), run_number, 
		     (fChiSquareMinimization ? "_ChiSqMin" : ""),
		     fSetStem.Data()));

  return;
}

void QwMpsOnly::ComputeAsymmetryCorrections()
{

  //**************************************************************
  //
  // Time to calculate some Corrections 
  //
  //**************************************************************

  TFile file(Form("%s/rootfiles/mps_bmod_tree%s_%i.%s.root", output.Data(), 
		  fFileSegment.Data(), run_number, fSetStem.Data()),"RECREATE");

  TTree *mod_tree = new TTree("Mod_Tree", "Modulation Analysis Results Tree");

  Int_t fEvCounter = 0;

  Double_t temp_correction = 0;
  Double_t monitor_correction[kNMaxDet][kNMaxMon];

  correction.resize(fNDetector);

  mod_tree->Branch("pattnum", &pattnum, "pattnum/D"); 
  mod_tree->Branch("yield_qwk_charge", &yield_qwk_charge_hw_sum, 
		   "yield_qwk_charge/D"); 
  mod_tree->Branch("yield_bm_pattern_number", &yield_bm_pattern_number, 
		   "yield_bm_pattern_number/D"); 
  mod_tree->Branch("yield_ramp", &yield_ramp_hw_sum, "yield_ramp_hw_sum/D"); 
  mod_tree->Branch("ErrorFlag", &ErrorFlag, "ErrorFlag/D"); 
  mod_tree->Branch("yield_qwk_mdallbars_Device_Error_Code", 
		   &yield_qwk_mdallbars_Device_Error_Code, 
		   "yield_qwk_mdallbars_Device_Error_Code/D"); 
  mod_tree->Branch("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, 
		   "yield_qwk_mdallbars/D"); 
  mod_tree->Branch("asym_qwk_charge", &asym_qwk_charge_hw_sum, 
		   "asym_qwk_charge/D"); 

  for(Int_t i = 0; i < fNDetector; i++){
    mod_tree->Branch(HDetectorList[i], &HDetBranch[i][0], 
		     Form("%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("raw_%s",HDetectorList[i].Data()), &HDetBranch[i][0], 
		     Form("raw_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("corr_%s", HDetectorList[i].Data()), 
		     &AsymmetryCorrection[i], 
		     Form("corr_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("raw_corr_%s", HDetectorList[i].Data()), 
		     &AsymmetryCorrection[i], 
		     Form("raw_corr_%s/D", HDetectorList[i].Data())); 
    if(fCharge){
      mod_tree->Branch(Form("corr_%s_charge", HDetectorList[i].Data()), 
		       &AsymmetryCorrectionQ[i], 
		       Form("corr_%s_charge/D", HDetectorList[i].Data())); 
    }
    mod_tree->Branch(Form("%s_Device_Error_Code", HDetectorList[i].Data()), 
		     &HDetBranch[i][kDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("correction_%s", DetectorList[i].Data()),
		     &correction[i],
		     Form("correction_%s/D", DetectorList[i].Data())); 

    if(fCharge){
      mod_tree->Branch(Form("correction_%s_charge", DetectorList[i].Data()), 
		       &correction[i], 
		       Form("correction_%s_charge/D", DetectorList[i].Data())); 
    }
    std::cout << HDetectorList[i] << std::endl;
  }

  for(Int_t j = 0; j < fNMonitor; j++){
    mod_tree->Branch(HMonitorList[j], &HMonBranch[j][0], 
		     Form("%s/D", HMonitorList[j].Data())); 
    mod_tree->Branch(Form("raw_%s", HMonitorList[j].Data()), &HMonBranch[j][0], 
		     Form("raw_%s/D", HMonitorList[j].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", HMonitorList[j].Data()), 
		     &HMonBranch[j][kDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HMonitorList[j].Data())); 

    mod_tree->Branch(YMonitorList[j], &YMonBranch[j][0], 
		     Form("%s/D", YMonitorList[j].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", YMonitorList[j].Data()), 
		     &YMonBranch[j][kDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", YMonitorList[j].Data())); 
    std::cout << HMonitorList[j] << "\t" << YMonitorList[j] << std::endl;
  }

  for(Int_t i = 0; i < fNDetector; i++){
    for(Int_t j = 0; j < fNMonitor; j++){
      mod_tree->Branch(Form("corr_%s_%s", HDetectorList[i].Data(), 
			    HMonitorList[j].Data()), &monitor_correction[i][j], 
		       Form("corr_%s_%s/D", HDetectorList[i].Data(), 
			    HMonitorList[j].Data())); 
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
	HDetBranch[j][0] = fChain->GetLeaf(Form("%s", HDetectorList[j].Data()))->
	  GetValue();
	HDetBranch[j][kDeviceErrorCode] = 
	  fChain->GetLeaf(Form("%s_Device_Error_Code", 
			       HDetectorList[j].Data()))->GetValue();

	for(Int_t k = 0; k < fNMonitor; k++){
	  HMonBranch[k][0] = 
	    fChain->GetLeaf(Form("%s", HMonitorList[k].Data()))->GetValue();
	  YMonBranch[k][0] = 
	    fChain->GetLeaf(Form("%s", HMonitorList[k].Data()))->GetValue();
	  HMonBranch[k][kDeviceErrorCode] = 
	    fChain->GetLeaf(Form("%s_Device_Error_Code", 
				 HMonitorList[k].Data()))->GetValue();
	  YMonBranch[k][kDeviceErrorCode] = 
	    fChain->GetLeaf(Form("%s_Device_Error_Code", 
				 HMonitorList[k].Data()))->GetValue();

  	  temp_correction += YieldSlope[j][k]*HMonBranch[k][0]; 
	  monitor_correction[j][k] = YieldSlope[j][k]*HMonBranch[k][0];
	  if(fCharge) 
	    if( (i % 100000) == 0 ){}
	}
   	correction[j] = temp_correction;                                  

	if(fCharge) correction_charge[j] = temp_correction + 
	  ChargeSensitivity[j]*asym_qwk_charge_hw_sum;  
	if(fCharge) AsymmetryCorrectionQ[j] = HDetBranch[j][0] - 
	  correction_charge[j];

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

  TMatrixD var(fNMonitor, fNMonitor);
  TMatrixD temp(fNMonitor, fNMonitor);
  TMatrixD Errorm(fNDetector, fNMonitor);  
  TMatrixD Errord(fNDetector, fNMonitor);  
  TMatrixD Error(fNDetector, fNMonitor);  

  for(Int_t i = 0; i < fNMonitor; i++){
    for(Int_t k = 0; k < fNMonitor; k++){
      for(Int_t n = 0; n < fNMonitor; n++){
	for(Int_t j = 0; j < fNMonitor; j++){
	  var(i, k) += TMath::Power(A(i, n), 2) * TMath::Power(eA(n, j), 2)
	    * TMath::Power(A(j, k), 2);
	}
      }
    }
  }
       
  for(Int_t m = 0; m < fNDetector; m++){    

    for(Int_t i = 0; i < fNMonitor; i++){
      for(Int_t j = 0; j < fNMonitor; j++){
	Errorm(m, i) += var(j, i)*TMath::Power( Y(m, j),2);
      }
    }
 
    for(Int_t i = 0; i < fNMonitor; i++)
      Errorm(m, i) = TMath::Sqrt(Errorm(m, i));
    for(Int_t i = 0; i < fNMonitor; i++){
      for(Int_t j = 0; j < fNMonitor; j++){
	Errord(m, i) += TMath::Power( A(j, i),2)*TMath::Power(eY(m, j) ,2);
      }
    }
    
    for(Int_t i = 0; i < fNMonitor; i++)
      Errord(m,i) = TMath::Sqrt(Errord(m,i));
    
    for(Int_t i = 0; i < fNMonitor; i++){
      Error(m, i) = TMath::Power(Errord(m, i), 2) + 
	TMath::Power(Errorm(m, i), 2);
      Error(m, i) = TMath::Sqrt(Error(m, i))*fUnitConvert[i];
      YieldSlopeError[m][i] = Error(m, i);
    }

  }
  std::cout << other << "Errors:" << normal << std::endl;
  Error.Print();
}

void QwMpsOnly::ComputeSlopeErrors(TMatrixD A, TMatrixD AErr, 
				   TMatrixD RInv, TMatrixD RErr)
{//Using derivation in http://arxiv.org/abs/hep-ex/9909031
  //from equation 10 we must first square each element of A, AErr, RInv, and RErr
  //Implemented as A=S*R ==> S=A*RInv where S is desired matrix of slopes dDet/dMon
  //A=fNDetector x fNMonitor
  //R=fNMonitor x fNMonitor 
  //S=fNDetector x fNMonitor

  TMatrixD RInvSq(fNMonitor, fNMonitor);
  TMatrixD RErrSq(fNMonitor, fNMonitor);
  TMatrixD ASq(fNDetector, fNMonitor);
  TMatrixD AErrSq(fNDetector, fNMonitor);
  TMatrixD Error(fNDetector, fNMonitor);  

  //square each matrix element
  for(int irow=0;irow<fNMonitor;irow++){
    for(int icol=0;icol<fNMonitor;icol++){
      RInvSq(irow, icol) = TMath::Power(RInv(irow,icol),2);
      RErrSq(irow, icol) = TMath::Power(RErr(irow,icol),2);
    }
  }
  for(int irow=0;irow<fNDetector;irow++){
    for(int icol=0;icol<fNMonitor;icol++){
      ASq(irow, icol) = TMath::Power(A(irow,icol),2);
      AErrSq(irow, icol) = TMath::Power(AErr(irow,icol),2);
    }
  }

  //Find matrix of errors of elements in RInvSq
  TMatrixD RInvSqErr(fNMonitor, fNMonitor); 
  for(int alpha=0;alpha<fNMonitor;alpha++){
    for(int beta=0;beta<fNMonitor;beta++){
      Double_t err = 0;
      for(int i=0;i<fNMonitor;i++){
	Double_t sum = 0;
	for(int j=0;j<fNMonitor;j++){
	  sum = RErrSq(i, j)*RInvSq(j, beta);
	}
	err += RInvSq(alpha, i) * sum;
      }
      RInvSqErr(alpha, beta) = err;
    }
  }

  for(int idet=0;idet<fNDetector;idet++){
    for(int imon=0;imon<fNMonitor;imon++){
      Double_t sum = 0;
      for(int jmon=0;jmon<fNMonitor;jmon++){
	sum += AErrSq(idet,jmon) * RInvSq(jmon,imon) + 
	       ASq(idet,jmon) * RInvSqErr(jmon, imon);
      }
      Error(idet, imon) = TMath::Sqrt(sum) * fUnitConvert[imon];
      YieldSlopeError[idet][imon] = Error(idet, imon);
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

void QwMpsOnly::CopyDataVectors(int modType){

  //Copy data to vectors for future calculation of residuals
  CoilData_copy[modType].clear();
  for(int imon=0;imon<fNMonitor;++imon)
    MonitorData_copy[modType][imon].clear();
  for(int idet=0;idet<fNDetector;++idet)
    DetectorData_copy[modType][idet].clear();

  for(int i=0;i<(int)CoilData[modType].size();++i){
    CoilData_copy[modType].push_back(CoilData[modType][i]);

    for(int imon=0;imon<fNMonitor;++imon){
      MonitorData_copy[modType][imon].push_back(MonitorData[imon][i]);
    }

    for(int idet=0;idet<fNDetector;++idet){
      DetectorData_copy[modType][idet].push_back(DetectorData[idet][i]);
    }

  }

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

    if(fChain->GetLeaf("qwk_charge")->GetValue() < fCurrentCut){
      bmodErrorFlag = 1;
      printf("Current cut %f %f\n", qwk_charge_hw_sum, fChain->GetLeaf("qwk_charge")->GetValue());
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

      std::cout << red << "Mps Tree::Natural Motion ErrorFlag" << normal << 
	std::endl;

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
      std::cout<<"Current cut "<<yield_qwk_charge_hw_sum<<" <  "<<
	fCurrentCut<<"\n";
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

  if(fTransverseData){
    file_directory += "/transverse";
  }

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

Double_t QwMpsOnly::FindChiSquareMinAMatrix(Int_t row, Int_t col){

  Double_t index = 0;
  for(Int_t icoil=0;icoil<kNCoil;icoil++){
    bool good_coil = true;
    for(int i=0;i<(int)fOmitCoil.size();++i){
      if(icoil == fOmitCoil[i]){
	good_coil = false;
      }
    }
    if(!good_coil) continue;

    index += AvDetectorSlope[icoil][row] * AvMonitorSlope[icoil][col];
  }
  return index;
}

Double_t QwMpsOnly::FindChiSquareMinAMatrixError(Int_t row, Int_t col){

  Double_t index = 0;
  for(Int_t icoil=0;icoil<kNCoil;icoil++){
    bool good_coil = true;
    for(int i=0;i<(int)fOmitCoil.size();++i){
      if(icoil == fOmitCoil[i]){
	good_coil = false;
      }
    }
    if(!good_coil) continue;

    index += TMath::Power(AvMonitorSlope[icoil][col] *
			  AvDetectorSlopeError[icoil][row],2);
    index += TMath::Power(AvDetectorSlope[icoil][row] * 
			  AvMonitorSlopeError[icoil][col],2);
  }
  return TMath::Sqrt(index);
}

Double_t QwMpsOnly::FindChiSquareMinRMatrix(Int_t row, Int_t col){

  Double_t index = 0;
  for(Int_t icoil=0;icoil<kNCoil;icoil++){
    bool good_coil = true;
    for(int i=0;i<(int)fOmitCoil.size();++i){
      if(icoil == fOmitCoil[i]){
	good_coil = false;
      }
    }
    if(!good_coil) continue;

    index += AvMonitorSlope[icoil][row] * AvMonitorSlope[icoil][col];
  }
  return index;
}

Double_t QwMpsOnly::FindChiSquareMinRMatrixError(Int_t row, Int_t col){

  Double_t index = 0;
  for(Int_t icoil=0;icoil<kNCoil;icoil++){
    bool good_coil = true;
    for(int i=0;i<(int)fOmitCoil.size();++i){
      if(icoil == fOmitCoil[i]){
	good_coil = false;
      }
    }
    if(!good_coil) continue;

    index += TMath::Power(AvMonitorSlope[icoil][col] * 
			  AvMonitorSlopeError[icoil][row],2);
    index += TMath::Power(AvMonitorSlope[icoil][row] *
			  AvMonitorSlopeError[icoil][col],2);
  }
  return TMath::Sqrt(index);
}

Double_t QwMpsOnly::FindDegPerMPS()
{
  SetupMpsBranchAddresses(0,0,1);
  Int_t ent = fChain->GetEntries(), newCycle = 0;
  Double_t mean = 0, n = 0, ramp_prev = -9999;
  printf("Finding # of degrees per MPS. Searching %i entries.\n", ent);

  for(int i = 0;i<ent;i++){
    fChain->GetEntry(i);
    Double_t nonLin = TMath::Abs((ramp_block3+ramp_block0) - 
				 (ramp_block2+ramp_block1));
    // std::cout<<nonLin<<" "<<fMaxRampNonLinearity<<"  "<<ramp_hw_sum<<std::endl;
    if(nonLin>fMaxRampNonLinearity||ramp_hw_sum<ramp_prev)newCycle = 1;
    qwk_charge_hw_sum = fChain->GetLeaf("qwk_charge")->GetValue();
    if(ErrorFlag==67207296 && qwk_charge_hw_sum>fCurrentCut && ramp_hw_sum>100){
      if(ramp_hw_sum<310 && !newCycle){
	mean += ramp_hw_sum - ramp_prev;
	//std::cout<<ramp_hw_sum - ramp_prev<<std::endl;
	n++;
      }
      newCycle = 0;
    }
    ramp_prev = ramp_hw_sum;
  }
  mean = mean / n;
  fDegPerMPS = mean;
  std::cout<<"Degrees per MPS = "<<mean<<std::endl;
  return  mean;
}

Int_t QwMpsOnly::FindRampExcludedRegions()
{//used in non_linearity study for cutting sections of ramp
  //where largest deviations in position on target occur
  TF1 *f = new TF1("f",Form("[0]+[1]*cos((x-[2])*%e)", kDegToRad),fLowRamp,
		   fHighRamp);
  for(int imod=0;imod<kNMod;++imod){
    Double_t rampPeaks = 0;
    f->SetParameter(0,1);
    f->SetParameter(1,1);
    f->SetParameter(2,1);
    TString monit = ((imod == 1 || imod == 4) ? "qwk_targetY":"qwk_targetX");
    fChain->Draw(Form("%s:ramp_filled>>pr%i",monit.Data(), imod),
		 Form("bm_pattern_number==%i || bm_pattern_number==%i",
		      imod, 11+imod), "prof");
    TProfile *pr = (TProfile*)gDirectory->Get(Form("pr%i",imod));
    pr->Approximate(1);
    Int_t fitResult = pr->Fit(f,"R");
    if(fitResult != 0){
      std::cout<<"Problem finding ramp peak for pattern "<<imod<<". Exiting.\n";
      delete f;
      return -1;
    }
    rampPeaks = f->GetParameter(2); 
    if(rampPeaks >=0)rampPeaks = fmod(rampPeaks, 180.0);
    else rampPeaks = 180 + fmod(rampPeaks, 180.0);
    //    std::cout<<"Ramp peak pattern "<<imod<<": "<<rampPeaks<<std::endl; 
    rampExcludedRegion[0][imod] = rampPeaks - kDegreesExcluded;
    rampExcludedRegion[1][imod] = rampPeaks + kDegreesExcluded;
    rampExcludedRegion[2][imod] = rampExcludedRegion[0][imod] + 180;
    rampExcludedRegion[3][imod] = rampExcludedRegion[1][imod] + 180;
  }
  for(int imod=0;imod<kNMod;++imod)
    printf("Ramp excluded regions: %i to %i  and  %i to %i\n",
	   (int)rampExcludedRegion[0][imod],(int)rampExcludedRegion[1][imod],
	   (int)rampExcludedRegion[2][imod],(int)rampExcludedRegion[3][imod]);
  delete f;
  return 0;
}

Int_t QwMpsOnly::FindRampPeriodAndOffset()
{
  SetupMpsBranchAddresses(0,0,1);
  TCut cut;
  Double_t parLim[4] = {-90,90,350,370};
  Double_t par[4] = {-500,1000,4,360};
  //define function generator signals used
  //right now fge missing from bmod_only files, but may add later
  fg[0] = "fgx1";
  fg[1] = "fgy1";
  fg[2] = "fgx2";
  fg[3] = "fgy2";

  Int_t bm_pat_num[8] = {0, 1, 3, 4, 11, 12, 14, 15};
  Double_t temp_offset[4], temp_period[4];
  Double_t temp_offset_err[4], temp_period_err[4];

  cut = TCut(Form("TMath::Abs(ramp_block0+ramp_block3-ramp_block2-ramp_block1)"
		  "<%f && ramp_block3>ramp_block2 && ramp_block2>ramp_block1 &&"
		  "ramp_block1>ramp_block0&&ramp>%f", fMaxRampNonLinearity,
		  (double)fPedestal));

  //find periods and offsets from 4 function generator types
  Double_t numerPeriod = 0, numerOffset = 0;
  Double_t denomPeriod = 0, denomOffset = 0;
  Bool_t used[4] = {0,0,0,0};
  Int_t numGood = 0;
  //  cut.Print();
  for(int i=0;i<4;i++){
    fChain->Draw(Form("%s:ramp>>ht(1000,0,400,1000)",fg[i]),cut && 
	       Form("(bm_pattern_number==%i || bm_pattern_number==%i)",
		    bm_pat_num[i],bm_pat_num[4+i]),"goff");

    TH2D *ht = (TH2D*)gDirectory->Get("ht");
    TF1 *f = new TF1("f", "[0]+[1]*sin((x+[2])*2*TMath::Pi()/[3])",fLowRamp,
		     fHighRamp);
    f->SetParameters(par);

    Int_t fitResult = ht->Fit(f,"R");

    if(fitResult != 0 || TMath::Abs(f->GetParameter(2))>90){
      f->SetParameters(par);
      f->SetParameter(1, f->GetParameter(1) * (-1.0));
      fitResult = ht->Fit(f,"R");
    }

    temp_offset[i] = f->GetParameter(2);
    temp_period[i] = f->GetParameter(3);
    temp_offset_err[i] = f->GetParError(2);
    temp_period_err[i] = f->GetParError(3);
    if(fitResult == 0 && f->GetChisquare()/f->GetNDF()<150){
      numGood++;
      used[i] = 1;
      numerPeriod += temp_period[i] * TMath::Power(temp_period_err[i], -2);
      numerOffset += temp_offset[i] * TMath::Power(temp_offset_err[i], -2);
      denomPeriod += TMath::Power(temp_period_err[i], -2);
      denomOffset += TMath::Power(temp_offset_err[i], -2);
      rampPeriodFitRslt[i] = temp_period[i];
      rampPeriodFitRsltErr[i] = temp_period_err[i];
      rampOffsetFitRslt[i] = temp_offset[i];
      rampOffsetFitRsltErr[i] = temp_offset_err[i];
    }else{
      std::cout<<"Fit to "<<fg[i]<<" vs ramp failed or has bad Chi-Square/NDF."
	" Excluding from calculation of ramp period and offset.\n";
      rampPeriodFitRslt[i] = -999999;
      rampPeriodFitRsltErr[i] = -999999;
      rampOffsetFitRslt[i] = -999999;
      rampOffsetFitRsltErr[i] = -999999;
    }
  }

  if(numGood<1 || denomPeriod==0.0 || denomOffset==0.0){
    std::cout<<"No good fit results for ramp period and offset."
      " Exiting program.\n";
    return 1;
  }else{
    //use error weighted mean
    fRampPeriod = numerPeriod / denomPeriod;
    fRampOffset = numerOffset / denomOffset;
  }

  Bool_t allGood = 1;
  Double_t maxDev = 0.8; //maximum deviation allowed from mean
  for(int i=0;i<4;i++){//kill execution if one or more fits not close to average
    if(used[i]){
      if(TMath::Abs(fRampPeriod - temp_period[i]) > maxDev &&
	 TMath::Abs(fRampPeriod - temp_period[i]) > 3 * temp_period_err[i]){
	printf("Ramp period from %s fit = %f not in close enough agreement with"
	       " average ramp period %f\n", fg[i], temp_period[i], fRampPeriod);
	allGood = 0;
      }
      if(TMath::Abs(fRampOffset - temp_offset[i]) > maxDev &&
	 TMath::Abs(fRampOffset - temp_offset[i]) > 3 * temp_offset_err[i]){
	printf("Ramp offset from %s fit = %f not in close enough agreement with"
	       " average ramp offset %f\n", fg[i], temp_offset[i], fRampOffset);
	allGood = 0;
      }
    }
  }

  if(!allGood){
    printf("Exiting program.\n");
    return 1;
  }

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
  TCut cut;
  cut = TCut(Form("TMath::Abs(ramp_block0+ramp_block3-ramp_block2-ramp_block1)"
		  "<%f && ramp_block3>ramp_block2 && ramp_block2>ramp_block1 &&"
		  "ramp_block1>ramp_block0", fMaxRampNonLinearity));

  SetupMpsBranchAddresses(0,0,1);
  fChain->Draw("ramp>>hRamp(100)", cut);
  TH1D *hRamp = (TH1D*)gDirectory->Get("hRamp");
  int lBin = hRamp->FindFirstBinAbove(0.0005*hRamp->GetEntries());

  //Some runs have non-modulation data leaked even when the modulation flag is
  //set. Try to discriminate between these data and modulation data. 
  if(hRamp->GetBinCenter(lBin)<fPedestal){
    for(int i=0;i<5;++i){
      if(hRamp->GetBinContent(i+lBin)<0.0005*hRamp->GetEntries() &&
	 lBin<hRamp->GetNbinsX()){
	while(hRamp->GetBinContent(i+lBin)<0.0005*hRamp->GetEntries()){
	  ++lBin;
	}
	lBin += i;
	break;
      }
    }

  }
  int hBin = hRamp->FindLastBinAbove(0.0005*hRamp->GetEntries());
  double width = hRamp->GetBinWidth(1);
  double avg_entries = 0, n = 0;
  for(int i = lBin+3;i<hBin-3;++i){
    avg_entries += hRamp->GetBinContent(i);
    ++n;
  }
  avg_entries /= n;
  double min = hRamp->GetBinLowEdge(lBin+1)-hRamp->GetBinContent(lBin)/avg_entries*width;
  fRampMax = hRamp->GetBinLowEdge(hBin)+hRamp->GetBinContent(hBin)/avg_entries*width;
  fRampLength = fRampMax - min;

  std::cout<<"RampLength: "<<fRampLength<<".  RampMax:"<<fRampMax<<std::endl;
  return 0; 
}

Int_t QwMpsOnly::GetCurrentCut()
{
  return(fCurrentCut);
}

Double_t QwMpsOnly::GetDegPerMPS()
{
  return fDegPerMPS;
}

Int_t QwMpsOnly::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

void QwMpsOnly::GetOptions(Int_t n, Char_t **options){
  Int_t i = 0;
  TString flag;
  while(i < n){
    flag = options[i];
    
    if(flag.CompareTo("--phase-config", TString::kExact) == 0){
      fPhaseConfig = true;
      ReadPhaseConfig(options[i + 1]);
      if(fPhaseConfig)
	std::cout << other << "Setting phase from file "<<options[i + 1]<<"."<<
	  normal<<std::endl;
    }    
    
    if(flag.CompareTo("--energy-coefficient-correction-factor", TString::kExact) == 0){
      fEnergyCoefficientCorrectionFactor = atof(options[i + 1]);;
      std::cout << other << "Setting energy coefficient correction factor "
		<<fEnergyCoefficientCorrectionFactor<<"."<<normal<<std::endl;
    }
    
    if(flag.CompareTo("--setup-config", TString::kExact) == 0){
      configFileName = options[i + 1];
      std::cout << other << "Using specified setup configuration file: "
		<<options[i + 1]<<"."<<  normal<<std::endl;
    }    


    if(flag.CompareTo("--transverse-data", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fTransverseData = atoi(options[i + 1]);
      if(fTransverseData)
	std::cout <<"Processing transversely polarized data.\n"<<std::endl;
    }    

    if(flag.CompareTo("--charge-sens", TString::kExact) == 0){
      fCharge = true;
      //      flag.Clear();
      fChargeFile = Form("config/charge_sensitivity_%i.dat", run_number);
      std::cout << other << "Setting up pseudo 5+1 analysis:\t" << fChargeFile 
		<< normal << std::endl;
      ReadChargeSensitivity();
    }    

    if(flag.CompareTo("--file-segment", TString::kExact) == 0){
      std::string option(options[i+1]);
      fFileSegmentInclude = true;

      //      flag.Clear();
      fFileSegment = options[i + 1];
      Int_t index = option.find_first_of(":");
      fLowerSegment = atoi(option.substr(0, index).c_str());
      fUpperSegment = atoi(option.substr(index + 1, 
					 option.length()-index).c_str());

      std::cout << other << "Processing file segments:\t" 
		<< fLowerSegment << ":" 
		<< fUpperSegment << normal << std::endl;
    }


    if(flag.CompareTo("--cut-nonlinear-regions", TString::kExact) == 0){
      //      flag.Clear();
      fCutNonlinearRegions = (Bool_t)atoi(options[i + 1]);

      std::cout << other << (fCutNonlinearRegions ? 
			     Form("Applying nonlinearity cut to +/- %i degrees"
				  " around peak excursions in target variables.", 
				  (int)kDegreesExcluded) : 
			     "No nonlinearity cut applied.")<<
	normal << std::endl;
    }    

    if(flag.CompareTo("--set-stem", TString::kExact) == 0){
      //      std::string option(options[i+1]);

      //      flag.Clear();
      fSetStem = options[i + 1];

      std::cout << other << "Setting set stem to:\t" 
		<< fSetStem.Data() << normal << std::endl;
    }    

    if(flag.CompareTo("--use-data-tertile", TString::kExact) == 0){
      //      std::string option(options[i+1]);

      //      flag.Clear();
      fUseDataTertile = atoi(options[i + 1]);
      if(fUseDataTertile > 0 && fUseDataTertile<4){
	char *tert = (char*)(fUseDataTertile == 1 ? "first" : 
		      (fUseDataTertile == 2 ? "second" : "third"));
	std::cout << other << "Analysis will use "<< tert 
		  <<" tertile of data from each pattern."<< std::endl;
      }else
	fUseDataTertile = 0;
    }    

    if(flag.CompareTo("--file-stem", TString::kExact) == 0){
      //      std::string option(options[i+1]);

      //      flag.Clear();
      fFileStem = options[i + 1];

      std::cout << other << "Setting file stem to:\t" 
		<< fFileStem.Data() << normal << std::endl;
    }    

    if(flag.CompareTo("--omit-coil", TString::kExact) == 0){
      //      std::string option(options[i+1]);

      //      flag.Clear();
      int coil = atoi(options[i + 1]);
      if(coil >= 0 && coil < kNCoil){
	fOmitCoil.push_back(coil);
	std::cout << other << "Omitting coil " <<fOmitCoil.back()
		  << " from analysis." << normal << std::endl;
	--fNCoil;
      }else{
	std::cout << other << "Invalid coil omission selection."
		  <<" Performing analysis with full coil set."<<std::endl;
      }
    }    

    if(flag.CompareTo("--set-output", TString::kExact) == 0){
      output = options[i + 1];
      std::cout << other << "Setting output directory to:\t" 
		<< output.Data() << normal << std::endl;
    }    

    if(flag.CompareTo("--ramp-pedestal", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fPedestal = atoi(options[i + 1]);

      std::cout << other << "Setting ramp pedestal to:\t" 
		<< fPedestal
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--set-low-ramp-fit-bound", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fLowRamp = atoi(options[i + 1]);

      std::cout << other << "Bounding ramp fit to region >:\t" 
		<< fLowRamp<<" degrees"
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--set-high-ramp-fit-bound", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fHighRamp = atoi(options[i + 1]);

      std::cout << other << "Bounding ramp fit to region <:\t" 
		<< fHighRamp<<" degrees"
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--chi-square-min", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fChiSquareMinimization = (atoi(options[i + 1])==0 ? 0 : 1);

      if(fChiSquareMinimization)      
	std::cout << other  <<"Using Chi Square minimization."
		  << normal << std::endl;
    }    

    if(flag.CompareTo("--ramp-max-nonlin", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fMaxRampNonLinearity = atoi(options[i + 1]);

      std::cout << other << "Setting ramp maximum non-linearity to:\t" 
		<< fMaxRampNonLinearity
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--charge", TString::kExact) == 0){
      fCharge = true;
      //      flag.Clear();
      if( IfExists(options[i + 1]) ){
	flag = options[i + 1];
	fChargeFile = flag;
	std::cout << other << "Setting up pseudo 5+1 analysis w/ "
	  "external nonlinearity file:\t" << flag << normal << std::endl;
	ReadChargeSensitivity();
      }
      else{
	PrintError("Could not open input file.  "
		   "Disabling charge sensitivity analysis");
	fCharge = false;
      }
    }
    
    if(flag.CompareTo("--current-cut", TString::kExact) == 0){
      //      flag.Clear();
      fCurrentCut = atoi(options[i + 1]);
      std::cout << other << "Setting current-cut to:\t" 
		<< fCurrentCut << normal << std::endl;
    }
    
    if(flag.CompareTo("--fractional-detector-slopes", TString::kExact) == 0){
      //      flag.Clear();
      fFractionalDetectorSlopes = atoi(options[i + 1]);
      if( fFractionalDetectorSlopes ){
	std::cout << other << "Calculating fractional detector slopes" 
		  << normal << std::endl;
      }else{
	std::cout << other << "Calculating absolute detector slopes" 
		  << normal << std::endl;
      }
    }

    
    if(flag.CompareTo("--make-friend-tree", TString::kExact) == 0){
      //      flag.Clear();
      fMakeFriendTree = atoi(options[i + 1]);
      if(fMakeFriendTree){
	std::cout << other << "Making friend tree with corrected ramp variables." 
		  << normal << std::endl;
      }else{
	std::cout << other << "Not re-creating friend tree."
		  << normal << std::endl;
      }
    }

    // flag to turn ON/OFF new energy bpm
    if(flag.CompareTo("--fNewEbpm", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();

      fNewEbpm = atoi(options[i + 1]);

      std::cout << other << "Setting use NEW energy bpm flag to " << fNewEbpm
		<< normal << std::endl;

    }  
  
    // Flag to turn ON 2D fit. (Default = OFF)
    if(flag.CompareTo("--2Dfit", TString::kExact) == 0){
      //      flag.Clear();
      f2DFit = atoi(options[i + 1]);
      std::cout <<(f2DFit ? "MINUIT Fit Selected" : 
		   "Finding coefficients using linearization routine")
		<< std::endl;
    } 


    if(flag.CompareTo("--run", TString::kExact) == 0){
      //      std::string option(options[i+1]);
      //      flag.Clear();
      fRunNumberSet = true;
      run_number = atoi(options[i + 1]);

      std::cout << other << "Processing run number:\t" 
		<< run_number
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--help", TString::kExact) == 0){
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      printf("Usage: ./qwbeammod <run_number> <options>");
      printf("\n\t--charge-sens \t\tinclude charge sensitivity in overall"
	     " correction to physics asymmetry.");
      printf("\n\t--charge \t\tsame as --chare-sesn except use can specify"
	     " path of charge sensitivities.");
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      exit(0);
    } 
    flag.Clear();       
    i++;
  }
}

TString  QwMpsOnly::GetOutput()
{
  return output;
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

Bool_t QwMpsOnly::LoadRootFile(TString filename, TChain *tree, Bool_t slug)
{
  Bool_t found = FileSearch(filename, tree, slug);
  
    if(!found){
      std::cerr << "Unable to locate requested file :: "
		<< filename
		<< std::endl;
      return 0;
    }else
      return 1;
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

Int_t QwMpsOnly::MakeFriendTree(Bool_t verbose)
{ //Take ramp values from return and map them to meaningful ramp values.
  //This is preformed using the following equation for the nonlinear points:
  //ramp_filled = (ramp_max-ramp)/ramp_length*(ramp_period-ramp_length)+ramp_max
  //If using redefined monitors create new monitor variables as linear combos
  //of the target variables.
  std::cout<<"::Making \"ramp_filled\" variable::\n";
  FindDegPerMPS();
  if(FindRampPeriodAndOffset())
    return 1;
  if(FindRampRange())
    return 1;

  //Find periods where ramp needs to be cut: first and last entry of each cycle
  //and where function generator not functioning properly and ramp<fPedestal

  double min_diff = (run_number<13000 ? 100 : 100);//arbitrary values reflecting
                                //that this cut is not really needed in Run 2
  double fgx1, fgx2, fgy1, fgy2, fge, pat;
  bool fge_exists = bool(fChain->GetLeaf("fge"))&& run_number<13000;

  fChain->SetBranchStatus("fgx1",1);
  fChain->SetBranchAddress("fgx1",&fgx1);
  fChain->SetBranchStatus("fgx2",1);
  fChain->SetBranchAddress("fgx2",&fgx2);
  fChain->SetBranchStatus("fgy1",1);
  fChain->SetBranchAddress("fgy1",&fgy1);
  fChain->SetBranchStatus("fgy2",1);
  fChain->SetBranchAddress("fgy2",&fgy2);
  if(fge_exists){
    fChain->SetBranchStatus("fge",1);
    fChain->SetBranchAddress("fge",&fge);
  }
  fChain->SetBranchStatus("bm_pattern_number",1);
  fChain->SetBranchAddress("bm_pattern_number",&pat);
  double prev_pat, prev_x1, prev_x2, prev_y1, prev_y2, prev_e,
    pprev_x1, pprev_x2, pprev_y1, pprev_y2, pprev_e;
  fChain->GetEntry(0);
  prev_x1 = fgx1;
  prev_x2 = fgx2;
  prev_y1 = fgy1;
  prev_y2 = fgy2;
  prev_e = fge;
  std::vector<int>v_ramp_good;
  v_ramp_good.push_back(0);
  fChain->GetEntry(1);
  prev_pat = pat;
  pprev_x1 =   prev_x1;
  pprev_x2 =   prev_x2;
  pprev_y1 =   prev_y1;
  pprev_y2 =   prev_y2;
  pprev_e  =   prev_e;
  v_ramp_good.push_back(0);
  for(int i=2;i<fChain->GetEntries();++i){
    fChain->GetEntry(i);
    //mark first and last entries of each cycle and all entries 
    //with ramp<pedestal as bad
    if(pat != prev_pat ){
      v_ramp_good.pop_back();
      v_ramp_good.pop_back();
      v_ramp_good.push_back(0);
      v_ramp_good.push_back(0);
      v_ramp_good.push_back(0);
      pprev_x1 =   prev_x1;
      pprev_x2 =   prev_x2;
      pprev_y1 =   prev_y1;
      pprev_y2 =   prev_y2;
      pprev_e  =   prev_e;
      
      prev_pat = pat;
      prev_x1 = fgx1;
      prev_x2 = fgx2;
      prev_y1 = fgy1;
      prev_y2 = fgy2;
      prev_e = fge;
      continue;
    }
    bool fg_off = 0;
    if(pat==11 || pat==14){
      fg_off = ( TMath::Abs(fgx1 - prev_x1) < min_diff && 
	      TMath::Abs(pprev_x1 - prev_x1) < min_diff ) 
	|| ( TMath::Abs(fgx2 - prev_x2) < min_diff && 
	     TMath::Abs(pprev_x2 - prev_x2) < min_diff ); 
    }else if(pat==12 || pat==15){
      fg_off = (TMath::Abs(fgy1 - prev_y1) < min_diff && 
	     TMath::Abs(pprev_y1 - prev_y1) < min_diff )
	|| (TMath::Abs(fgy2 - prev_y2) < min_diff && 
	    TMath::Abs(pprev_y2 - prev_y2) < min_diff );
    }else if(pat==13 && fge_exists){
      fg_off = ( TMath::Abs(fge - prev_e) < min_diff && 
	      TMath::Abs(pprev_e - prev_e) < min_diff );
    }else if(pat==0){
      fg_off = ( TMath::Abs(fgx1 - prev_x1) < min_diff && 
	      TMath::Abs(pprev_x1 - prev_x1) < min_diff ); 
    }else if(pat==3){
      fg_off = ( TMath::Abs(fgx2 - prev_x2) < min_diff && 
	      TMath::Abs(pprev_x2 - prev_x2) < min_diff ); 
    }else if(pat==1){
      fg_off = (TMath::Abs(fgy1 - prev_y1) < min_diff && 
	     TMath::Abs(pprev_y1 - prev_y1) < min_diff );
    }else if(pat==4){
      fg_off = (TMath::Abs(fgy2 - prev_y2) < min_diff && 
	    TMath::Abs(pprev_y2 - prev_y2) < min_diff );
    }else if((pat==13 || pat==2) && fge_exists){
      fg_off = ( TMath::Abs(fge - prev_e) < min_diff && 
	      TMath::Abs(pprev_e - prev_e) < min_diff );
    }
    if(fg_off){
      //replace previous 3 as well since difference 
      //test is on 3 consecutive events
      int n = (int)(v_ramp_good.size()<3 ? v_ramp_good.size() : 3);
      for(int j=0;j<n;++j){
	v_ramp_good.pop_back();
      }
      for(int j=0;j<n+1;++j){
	v_ramp_good.push_back(0);
      }
    }else if(i == fChain->GetEntries()-1){
      v_ramp_good.push_back(0);
    }else if(ramp_hw_sum < fPedestal){
      v_ramp_good.push_back(0);
    }//flag individual events that respond to no medication as bad 
    else if( (run_number==10056&&i==88516 ) ||
	     (run_number==11047&&i==405   ) ||
	     (run_number==14263&&i==43075 ) ||
	     (run_number==18849&&i==57206 ) ||
	     (run_number==18849&&i==67015 ) ||
	     (run_number==18693&&i==86128 ) ||
	     (run_number==18667&&i==104223) ||
	     (run_number==18664&&i==87867 ) ||
	     (run_number==11044&&i==3640  )   ){

      v_ramp_good.push_back(0);
    }else{
      v_ramp_good.push_back(1);
    }
    pprev_x1 =   prev_x1;
    pprev_x2 =   prev_x2;
    pprev_y1 =   prev_y1;
    pprev_y2 =   prev_y2;
    pprev_e  =   prev_e;
    
    prev_pat = pat;
    prev_x1 = fgx1;
    prev_x2 = fgx2;
    prev_y1 = fgy1;
    prev_y2 = fgy2;
    prev_e = fge;
  }
  if(fChain->GetEntries()!=(Long_t)v_ramp_good.size()){
    std::cout<<"Incorrect number of entries in cut flag. "<< fChain->GetEntries()
	     <<v_ramp_good.size()<<" Exiting.\n";
    return -1;
  }

  //create new friendable tree with new ramp
  TFile *newfile = new TFile(Form("%s/%smps_only_friend_%i.root",
				  gSystem->Getenv("MPS_ONLY_ROOTFILES"),
				  (fTransverseData ? "transverse/" : ""),
				  run_number),"recreate");
  TTree *newTree = new TTree("mps_slug", "mps_slug");
 
  Bool_t isLinear;
  Double_t ramp_f, lin, x, y, xp, yp, x1, y1, x2, y2;
  Double_t x1_dec, y1_dec, x2_dec, y2_dec;
  Double_t scale_xp, scale_yp;

  b_ramp_filled = newTree->Branch("ramp_filled", &ramp_f, "ramp_filled/D");
  ifstream scale_file(Form("%s/config/new_monitors.config",
			   gSystem->Getenv("BMOD_SRC")));
  if(!(scale_file.is_open() && scale_file.good())){
    std::cout<<"New monitor scale factor file not found. Exiting.\n";
    return -1;
  }
  std::string temp_line;
  while(scale_file.peek()=='!')
    getline(scale_file, temp_line);
  char temp[255], arun1[255], arun2[255], brun1[255], brun2[255];
  scale_file>>temp>>arun1>>arun2;
  scale_file>>temp>>brun1>>brun2;
  scale_xp = 1000 * (run_number < 13000 ? atof(arun1) : atof(arun2));
  scale_yp = 1000 * (run_number < 13000 ? atof(brun1) : atof(brun2));

  b_ramp_good =  newTree->Branch("ramp_good", &ramp_good, "ramp_good/I");
  b_mx1 = newTree->Branch("MX1", &x1, "MX1/D");
  b_mx2 = newTree->Branch("MX2", &x2, "MX2/D");
  b_my1 = newTree->Branch("MY1", &y1, "MY1/D");
  b_my2 = newTree->Branch("MY2", &y2, "MY2/D");
  b_mx1_dec = newTree->Branch("MX1_Device_Error_Code", &x1_dec, 
			      "MX1_Device_Error_Code/D");
  b_mx2_dec = newTree->Branch("MX2_Device_Error_Code", &x2_dec, 
			      "MX2_Device_Error_Code/D");
  b_my1_dec = newTree->Branch("MY1_Device_Error_Code", &y1_dec, 
			      "MY1_Device_Error_Code/D");
  b_my2_dec = newTree->Branch("MY2_Device_Error_Code", &y2_dec, 
			      "MY2_Device_Error_Code/D");

  fChain->SetBranchStatus("qwk_targetX", 1);
  fChain->SetBranchStatus("qwk_targetXSlope", 1);
  fChain->SetBranchStatus("qwk_targetY", 1);
  fChain->SetBranchStatus("qwk_targetYSlope", 1);
  fChain->SetBranchStatus("qwk_targetX_Device_Error_Code", 1);
  fChain->SetBranchStatus("qwk_targetXSlope_Device_Error_Code", 1);
  fChain->SetBranchStatus("qwk_targetY_Device_Error_Code", 1);
  fChain->SetBranchStatus("qwk_targetYSlope_Device_Error_Code", 1);
  fChain->SetBranchAddress("qwk_targetX", &x);
  fChain->SetBranchAddress("qwk_targetXSlope", &xp);
  fChain->SetBranchAddress("qwk_targetY", &y);
  fChain->SetBranchAddress("qwk_targetYSlope", &yp);
  fChain->SetBranchAddress("qwk_targetX_Device_Error_Code", &x1_dec);
  fChain->SetBranchAddress("qwk_targetXSlope_Device_Error_Code", &x2_dec);
  fChain->SetBranchAddress("qwk_targetY_Device_Error_Code", &y1_dec);
  fChain->SetBranchAddress("qwk_targetYSlope_Device_Error_Code", &y2_dec);

  for(Int_t i=0;i<fChain->GetEntries();i++){
    fChain->GetEntry(i);
    x1 = (x + (scale_xp * xp));
    x2 = (x - (scale_xp * xp));
    y1 = (y + (scale_yp * yp));
    y2 = (y - (scale_yp * yp));

    lin =  TMath::Abs(ramp_block0+ramp_block3-ramp_block2-ramp_block1);
    isLinear = lin < fMaxRampNonLinearity && ramp_block3>ramp_block2 && 
               ramp_block2>ramp_block1 && ramp_block1>ramp_block0;

    ramp_f = (isLinear ? ramp_hw_sum : (fRampMax-ramp_hw_sum)/ fRampLength *
	      (fRampPeriod - fRampLength) + fRampMax);


    //stretch and offset ramp before filling
    ramp_f = (ramp_f + fRampOffset) * 360.0 / fRampPeriod;
    ramp_f = ramp_f - ((Int_t)(ramp_f/360.0))*360.0;

    ramp_good = v_ramp_good.at(i);

    //last ditch effort to kill noise in ramp
    if(isLinear && lin>fMaxRampNonLinearity/3.0) ramp_good =0;

    newTree->Fill();
    if(i%100000==0)std::cout<<"Processing entry "<<i<<".\n";
  }

  if(verbose)
    newTree->Print();
  newTree->Write("",TObject::kOverwrite);
  //  newTree->AutoSave();
  newfile->Close();
  //  delete newfile;
  //  delete newTree;
  fChain->ResetBranchAddresses();
  return 0;
}

void QwMpsOnly::MatrixFill(Bool_t run_avg)
{ //Treat differently if analyzing full run average than if single full cycle
  //For analyzing a single cycle, copy cycle slopes into AverageSlope vectors
  //and proceed as usual.

  if(!run_avg){
    for(Int_t icoil=0;icoil<kNCoil;icoil++){
      AvDetectorSlope[icoil].clear();
      AvDetectorSlopeError[icoil].clear();
      //      std::cout<<icoil<<": "<<std::endl;
      for(Int_t idet=0;idet<fNDetector;idet++){
	AvDetectorSlope[icoil].push_back(DetectorSlope[icoil][idet].back());
	AvDetectorSlopeError[icoil].push_back(DetectorSlopeError[icoil][idet].back());
	//printf("%s: %14.9e\n",DetectorList[idet].Data(),
	//DetectorSlope[icoil][idet].back());
      }
      AvMonitorSlope[icoil].clear();
      AvMonitorSlopeError[icoil].clear();
      for(Int_t imon=0;imon<fNMonitor;imon++){
	AvMonitorSlope[icoil].push_back(MonitorSlope[icoil][imon].back());
	AvMonitorSlopeError[icoil].push_back(MonitorSlopeError[icoil][imon].back());
	//printf("%s: %14.9e\n",MonitorList[imon].Data(),
	//MonitorSlope[icoil][imon].back());
      }
    }
  }


  TMatrixD AMatrix(fNDetector, fNMonitor);
  TMatrixD AMatrixE(fNDetector, fNMonitor);

  Double_t determinant;

  CheckFlags();
  if(run_avg){
    diagnostic.open(Form("%s/diagnostics/diagnostic%s_%i%s.%s.dat", output.Data(),
			 fFileSegment.Data(), run_number,
			 (fChiSquareMinimization ? "_ChiSqMin" : ""), 
			 fSetStem.Data()), fstream::out);
  }

  for(Int_t j = 0; j < fNDetector; j++){
    if(diagnostic.is_open()&&diagnostic.good() && run_avg)
      diagnostic << DetectorList[j] << std::endl;
    for(Int_t k = 0; k < fNMonitor; k++){
      AMatrix(j,k) = (fChiSquareMinimization ? FindChiSquareMinAMatrix(j,k): 
		      AvDetectorSlope[k][j]);
      AMatrixE(j,k) = (fChiSquareMinimization ? FindChiSquareMinAMatrixError(j,k):
		       AvDetectorSlopeError[k][j]);
      if( (diagnostic.is_open() && diagnostic.good()) && run_avg ){
	diagnostic << AvDetectorSlope[k][j] << " +- " 
		   << AvDetectorSlopeError[k][j] << " ";
      }
    }
    if( (diagnostic.is_open() && diagnostic.good()) && run_avg )
      diagnostic << std::endl;
  }
  if( (diagnostic.is_open() && diagnostic.good()) && run_avg ) 
    diagnostic << std::endl;
  std::cout << "\t\t\t\t::A Matrix::"<< std::endl;
  AMatrix.Print("%11.10f");

  std::cout << "\t\t\t\t::A Matrix Error::"<< std::endl;
  AMatrixE.Print("%11.10f");

  TMatrixD RMatrix(fNMonitor, fNMonitor);
  TMatrixD RMatrixE(fNMonitor, fNMonitor);

  for(Int_t imon = 0; imon < fNMonitor; imon++){
    for(Int_t icoil = 0; icoil < fNMonitor; icoil++){
      RMatrix(imon,icoil) = (fChiSquareMinimization ? 
			     FindChiSquareMinRMatrix(imon,icoil) :
			     AvMonitorSlope[icoil][imon]);
      RMatrixE(imon,icoil) = (fChiSquareMinimization ? 
			     FindChiSquareMinRMatrixError(imon,icoil) :
			     AvMonitorSlopeError[icoil][imon]);

      if( (diagnostic.is_open() && diagnostic.good() && run_avg) ){
	diagnostic << AvMonitorSlope[icoil][imon] << " +- " 
		   << AvMonitorSlopeError[icoil][imon] << " ";
      }
    }
    if( (diagnostic.is_open() && diagnostic.good()) && run_avg )
      diagnostic << std::endl;
  }
  std::cout << "\t\t\t\t::R Matrix:: " << std::endl;
  RMatrix.Print("%11.10f");
  
  std::cout << "\t\t\t\t::R Matrix Error::"<< std::endl;
  RMatrixE.Print("%11.10f");

  //get eigenvalues and eigenvectors of RMatrix
  TVectorD RMatrixEigenvalues(fNMonitor);
  TMatrixD RMatrixEigenvectors(fNMonitor, fNMonitor);
  RMatrixEigenvectors = RMatrix.EigenVectors(RMatrixEigenvalues);
  std::cout<<"Eigenvalues:\n";
  RMatrixEigenvalues.Print();
  std::cout<<"Eigenvectors:\n";
  RMatrixEigenvectors.Print();
  if( (diagnostic.is_open() && diagnostic.good()) && run_avg){
    diagnostic <<"\n\nRMatrix Eigenvalues:\n";
    for(int row=0;row<fNMonitor;row++){
      diagnostic <<Form("%+0.13f   ",RMatrixEigenvalues(row));
    }
    diagnostic <<"\n\nRMatrix Eigenvectors:\n";
    for(int row=0;row<fNMonitor;row++){
      for(int col=0;col<fNMonitor;col++){
	diagnostic<<Form("%+0.8f   ",RMatrixEigenvectors(row,col));
      }
      diagnostic<<std::endl;
    }

  }
  

  
  TMatrixD RMatrixInv = RMatrix;
  RMatrixInv.Invert(&determinant);
  std::cout << "\t\t\t\t::R Matrix Inverse:: " << std::endl;
  
  RMatrixInv.Print("%11.10f");
  
  std::cout << determinant << std::endl;
  TMatrixD Identity(fNMonitor, fNMonitor);
  Identity.Mult(RMatrixInv, RMatrix);
  Identity.Print();

  TMatrixD SMatrix(fNDetector, fNMonitor);
  SMatrix.Mult(AMatrix, RMatrixInv);

  std::cout << "\n\n\t\t\t\t::SMatrix::\n" << std::endl;

  for(Int_t idet = 0; idet < fNDetector; idet++){
    for(Int_t imon = 0; imon < fNMonitor; imon++){
      SMatrix(idet,imon) *= fUnitConvert[imon];
      YieldSlope[idet][imon] = SMatrix(idet,imon);
    }
  }
  SMatrix.Print();
  CalculateResiduals();
  if(fChiSquareMinimization)
    ComputeSlopeErrors(AMatrix, AMatrixE, RMatrixInv, RMatrixE);
  else
    ComputeErrors(AMatrix, AMatrixE, RMatrixInv, RMatrixE);

  Write(run_avg);

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
  SetupMpsBranchAddresses(1,1,0);
  Int_t fEvCounter = 0, nonModEvts = 0;
  Int_t errors[kNMod], good[kNMod];
  for(Int_t i=0;i<kNMod;i++){
    errors[i] = 0;
    good[i] = 0;
  }

  fNEvents = 0;
  if (fChain == 0) return -1;
  Long64_t nentries = fChain->GetEntries();

  std::cout << "Number of entries: " << nentries << std::endl;
  for(Int_t i = 0; i < nentries; i++){
    fChain->GetEntry(i);
    Int_t pat = ConvertPatternNumber((Int_t)bm_pattern_number);
    if(pat > -1 && pat < 5){
      if(fXinit==0 && fYinit==0 && fEinit==0 && fXPinit==0 && fYPinit==0){
	fFirstEntry = i;
      }
      i =  ProcessMicroCycle(i, &fEvCounter, errors, good);
    }else nonModEvts++;

    //Invert matrix each full cycle
    if(fXinit && fYinit && fEinit && fXPinit && fYPinit){
      fMacroCycleNum ++;
      fFullCycle = true;
      fLastEntry = i;//last entry of macrocycle
      MatrixFill(0);
      //reset all flags
      fXinit = 0;
      fYinit = 0;
      fEinit = 0;
      fXPinit = 0;
      fYPinit = 0;
    }
  }

  std::cout << "Run "<<run_number<<std::endl;
  Int_t goodEv = 0;
  for(Int_t i = 0; i < kNMod; i++){
    std::cout<<" bm_pattern# "<<i<<":  Total Events Cut (" << errors[i] <<")"
	      <<"   Good Events ("<<good[i]<<")\n";
    goodEv += good[i];
  }

  std::cout <<nonModEvts<<" non-modulation events." << std::endl;
  std::cout << "\n::Done with Pilfer::" << std::endl;

  return goodEv;
}

void QwMpsOnly::PrintAverageSlopes()
{
  FILE *mon_coil_coeff = fopen(Form("%s/slopes/mon_coil_coeff_%i%s.%s.dat",
				    output.Data(), run_number,
				    (fChiSquareMinimization ? "_ChiSqMin" : ""),
				    fSetStem.Data()),"w");
  FILE *det_coil_coeff = fopen(Form("%s/slopes/det_coil_coeff_%i%s.%s.dat",
				    output.Data(), run_number,
				    (fChiSquareMinimization ? "_ChiSqMin" : ""),
				    fSetStem.Data()),"w");
  printf("Monitor Coeffs   |       Xmod   |       XPmod  |       Emod   |"
	 "      Ymod    |      YPmod   |\n");
  printf("******************************************************************"
	 "***************************\n");
  Int_t nMod = kNCoil;

  for(Int_t i=0;i<fNMonitor;i++){
    TString mon = MonitorList[i];
    mon.Resize(16);
    fprintf(mon_coil_coeff,"%s\n",MonitorList[i].Data());
    printf("%s |",mon.Data());
    for(Int_t j=0;j<nMod;j++){
      printf(" %+9.5e |",AvMonitorSlope[j][i]);
      fprintf(mon_coil_coeff,"%12.6e\t%12.6e\n", AvMonitorSlope[j][i],
	      AvMonitorSlopeError[j][i]);
    }
    printf("\n");
  }
  fclose(mon_coil_coeff);

  printf("\n\n");
  printf("Detector Coeffs  |       Xmod   |       XPmod  |       Emod   |"
	 "      Ymod    |      YPmod   |\n");
  printf("******************************************************************"
	 "***************************\n");

  for(Int_t i=0;i<fNDetector;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    printf("%s |",det.Data());
    fprintf(det_coil_coeff, "%s\n", DetectorList[i].Data());
    for(Int_t j=0;j<nMod;j++){
      printf(" %+9.5e |",AvDetectorSlope[j][i]);
      fprintf(det_coil_coeff,"%12.6e\t%12.6e\n",AvDetectorSlope[j][i],
	      AvDetectorSlopeError[j][i]);
    }
    printf("\n");
  }
  printf("\n\n");
  fclose(det_coil_coeff);
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
  Int_t modType = -1, nCut = 0, nErr = 0;
  //Double_t prev_ramp = 0;
  Int_t pattern = ConvertPatternNumber((Int_t)bm_pattern_number);
  fNEvents = 0;

  //choosing modType as pattern means monitor and detector coefficients
  //are calculated and stored in the same order they were set up in the
  //rootfiles i.e. time ordering: X, Y, E, XP, YP.
  modType =  pattern;

  if(modType<0||modType>4)return -1;
  std::cout<<"Modulation type "<<pattern<<" found at entry "<<i<<"\n";

  //Make sure arrays are clear of data
  CoilData[modType].clear();
  for(Int_t imon = 0; imon < fNMonitor; imon++){
    MonitorData[imon].clear();
  }
  for(Int_t idet = 0; idet < fNDetector; idet++){
    DetectorData[idet].clear();
  }

  while(pattern == modType && i < nEnt){
    Bool_t inCutRegion = 0;

    if(fCutNonlinearRegions){
      if(rampExcludedRegion[0][modType]>=0){
	inCutRegion = ramp_filled > rampExcludedRegion[0][modType] && 
	              ramp_filled < rampExcludedRegion[1][modType];
      }else{
	inCutRegion = ramp_filled > 360.0 + rampExcludedRegion[0][modType] || 
	              ramp_filled < rampExcludedRegion[1][modType];
      }

      if(rampExcludedRegion[3][modType]<=360){
	inCutRegion = inCutRegion ||
	  (ramp_filled > rampExcludedRegion[2][modType] && 
	   ramp_filled < rampExcludedRegion[3][modType]);
      }else{
	inCutRegion = inCutRegion ||
	  (ramp_filled > rampExcludedRegion[2][modType] || 
	   ramp_filled < rampExcludedRegion[3][modType] - 360);
      }

    }

    if(inCutRegion&&!fCutNonlinearRegions){
      std::cout<<"Error! Cutting regions of ramp that are supposed"
	       <<" to be included!\n";
      return -1;
    }

    if(ErrorCodeCheck("mps_tree")!=0 || inCutRegion || !ramp_good){
      ++nCut;
      if(ErrorCodeCheck("mps_tree")!=0)++nErr;
    }else{
      good[modType]++;
      CoilData[modType].push_back(ramp_filled);


      for(Int_t idet = 0; idet < fNDetector; ++idet){
	Double_t val = fChain->GetLeaf(DetectorList[idet].Data())->GetValue();
	DetectorData[idet].push_back(val);
      }
      for(Int_t imon = 0; imon < fNMonitor; imon++){
	Double_t val = fUnitConvert[imon]*
	  (fChain->GetLeaf(MonitorList[imon].Data())->GetValue());
	MonitorData[imon].push_back(val);
      }
      ++evCntr;
      ++fNEvents;
    }
    ++i;
    if(i==nEnt)break;

    fChain->GetEntry(i);
    //    std::cout<<i<<" bm_pattern_number:"<<bm_pattern_number<<std::endl;
    pattern = ConvertPatternNumber((Int_t)bm_pattern_number);
    if(i%10000==0)std::cout<<i<<"  pattern "<<pattern<<std::endl;
  }
  i--;
  std::cout<<fNEvents<< " good "<<modType<<"-type modulation events found. ";
  err[modType] += nErr;
  std::cout<<nErr<<" errors -- "<<err[modType]<<" total "<<modType<<
    "-type errors.\n";

  if(f2DFit) {
    Calculate2DSlope(modType, 0);
  } else {
    CalculateCoefficients(modType);
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
  
  config.open(configFileName.Data(), std::ios_base::in);
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
	  std::cout << other << token << ": Branch doesn't exist." << normal 
		    << std::endl;
	}
	else{
	  if(fHelTree) {
	    this->HMonitorList.push_back(Form("diff_%s", token));
	    fChain->SetBranchStatus(Form("diff_%s", token), 1);
	    fChain->SetBranchStatus(Form("diff_%s_Device_Error_Code", token), 1);

	    this->YMonitorList.push_back(Form("yield_%s", token));
	    fChain->SetBranchStatus(Form("yield_%s", token), 1);
	    fChain->SetBranchStatus(Form("yield_%s_Device_Error_Code", token), 1);
	    std::cout << HMonitorList.back() << "\t" << YMonitorList.back() 
		      << std::endl;
	  }
	  else{
	    std::cout << other << "\t\tMonitor is: " << token << normal 
		      <<std::endl;
	    this->MonitorList.push_back(Form("%s%s", mon_prefix.Data(), token)); 
// 	    fChain->SetBranchStatus(Form("%s%s", mon_prefix.Data(), token), 1);
// 	    fChain->SetBranchStatus(Form("%s%s_Device_Error_Code", 
// 					 mon_prefix.Data(), token), 1);
	  }
	}
      }
      if(strcmp("det", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token
       	token = strtok(NULL, " .,"); 
	
	if( !(fChain->GetBranch(Form("%s%s", det_prefix.Data(), token))) ){
	  std::cout << other << token << ": Branch doesn't exist." << normal 
		    << std::endl;
	}
	else{
	  if(fHelTree) {
	    this->HDetectorList.push_back(Form("asym_%s", token));
	    fChain->SetBranchStatus(Form("asym_%s", token), 1);
	    fChain->SetBranchStatus(Form("asym_%s_Device_Error_Code", token), 1);
	    std::cout << HDetectorList.back() << std::endl;
	  }
	  else{
	    std::cout << other << "\t\tDetector is: " << token << normal 
		      << std::endl; 
	    this->DetectorList.push_back(Form("%s%s", det_prefix.Data(), token));
// 	    fChain->SetBranchStatus(Form("%s%s", det_prefix.Data(), token), 1);
// 	    fChain->SetBranchStatus(Form("%s%s_Device_Error_Code", 
// 					 det_prefix.Data(), token), 1);
	  }
	}
      }
      else 
	token = strtok(NULL, " .,"); 
      
    }
  }
  fNDetector = DetectorList.size();
  fNMonitor = MonitorList.size();
  if(fNMonitor<fNCoil){
    fChiSquareMinimization = 1;
    std::cout<<"Fewer monitors than coils. Using Chi Square minimization.\n";
  }else if(fNMonitor > fNCoil){
    std::cout<<"More monitors than coils. Program not set up to accomodate this.";
    std::cout<<" Exiting.\n";
    exit(1);
  }else{
    fChiSquareMinimization = 0;
    std::cout<<"Equal number of coils and monitors. ";
    std::cout<<"Not using Chi Square minimization.\n";
  }
  
  if( (fNDetector > kNMaxDet) || (fNMonitor > kNMaxMon) )
    {
      std::cout<<red<<"Error :: Exceeded maximum number of detectors(monitors)" 
	       <<red<<"Detectors:\t" << fNDetector << "\tMax:\t" << kNMaxDet
	       <<red<<"Monitors:\t" << fNMonitor   << "\tMax:\t" << kNMaxMon
	       <<normal<< std::endl;
      exit(1);
    }
  
  config.close();
  
  // now create/update MonitorList with newEbpm, if needed
  if(fNewEbpm){

    BuildNewEBPM();
    
    TString fname = Form("%s/%smps_only_newEbpm_%i.root",
			 gSystem->Getenv("MPS_ONLY_ROOTFILES"),
			 (fTransverseData ? "transverse/" : ""),
			 run_number);

    fChain->AddFriend("mps_slug", fname);
    fChain->SetBranchAddress("newEbpm", &newEbpm);

    // now set the new energy bpm as the energy bpm to be used
    for(int inew=0;inew<5;inew++){
      if(MonitorList[inew].CompareTo("qwk_bpm3c12X", TString::kExact) == 0)
	MonitorList[inew] = "newEbpm";
    }
  }

  //set the unit conversion array
  std::cout<<"Unit conversion array: ";
  for(int i=0; i<fNMonitor;i++){
    fUnitConvert[i] = (MonitorList[i].Contains("Slope") ? 1.0e4 : 1.0);
    std::cout<<fUnitConvert[i]<<"  ";
  }
  std::cout<<std::endl;

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

  phase.resize(kNMod);
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
    fNCoil = 3;
    break;

  case 1:
    fReduceMatrix_y  = 1;
    fReduceMatrix_yp = 1;
    fNCoil = 3;
    break;

  default:
    exit(1);
    break;
  }
  return;
}

void QwMpsOnly::Scan(Bool_t set_status)
{
                               
   for(Int_t i = 0; i < (Int_t)fNDetector; i++){
     if(set_status)
       fChain->SetBranchStatus(DetectorList[i], 1);
     if(!set_status){
       fChain->SetBranchAddress(DetectorList[i], &DetBranch[i]);
     }
   }
   for(Int_t i = 0; i < (Int_t)fNMonitor; i++){
     if(set_status)
       fChain->SetBranchStatus(MonitorList[i], 1);
     if(!set_status)
       fChain->SetBranchAddress(MonitorList[i], &MonBranch[i]);
   }
}

void QwMpsOnly::SetDegPerMPS(Double_t deg)
{
  fDegPerMPS = deg;
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

void QwMpsOnly::SetOutput(char *out)
{
  output = out;
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

  fChain->SetBranchAddress("yield_qwk_charge",&yield_qwk_charge_hw_sum ,
			   &b_yield_qwk_charge);
  fChain->SetBranchAddress("pattnum", &pattnum, &b_pattnum);
  fChain->SetBranchAddress("yield_ramp",&yield_ramp_hw_sum,&b_yield_ramp);
  fChain->SetBranchAddress("ErrorFlag",&ErrorFlag, &b_ErrorFlag);
  fChain->SetBranchAddress("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, 
			   &b_yield_qwk_mdallbars);
  fChain->SetBranchAddress("asym_qwk_charge", &asym_qwk_charge_hw_sum, 
			   &b_asym_qwk_charge);

}
void QwMpsOnly::SetupMpsBranchStatuses(Bool_t includeFriendTreeLeaves,
				       Bool_t includeMonitorsAndDetectors,
				       Bool_t includeBlocksAndFuncGen = 1){
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("qwk_charge", 1);
  fChain->SetBranchStatus("qwk_charge_Device_Error_Code", 1);
  fChain->SetBranchStatus("bm_pattern_number", 1);
  fChain->SetBranchStatus("ErrorFlag", 1);
  fChain->SetBranchStatus("ramp", 1);
  if( includeBlocksAndFuncGen ){
    fChain->SetBranchStatus("fgx1", 1);
    fChain->SetBranchStatus("fgx2", 1);
    fChain->SetBranchStatus("fgy2", 1);
    fChain->SetBranchStatus("fgy1", 1);
    fChain->SetBranchStatus("ramp_block0", 1);
    fChain->SetBranchStatus("ramp_block1", 1);
    fChain->SetBranchStatus("ramp_block2", 1);
    fChain->SetBranchStatus("ramp_block3", 1);
  }
  if(includeFriendTreeLeaves){
    fChain->SetBranchStatus("ramp_good", 1);
    fChain->SetBranchStatus("ramp_filled", 1);
    fChain->SetBranchStatus("MX1", 1);
    fChain->SetBranchStatus("MX2", 1);
    fChain->SetBranchStatus("MY1", 1);
    fChain->SetBranchStatus("MY2", 1);
    fChain->SetBranchStatus("MX1_Device_Error_Code", 1);
    fChain->SetBranchStatus("MX2_Device_Error_Code", 1);
    fChain->SetBranchStatus("MY1_Device_Error_Code", 1);
    fChain->SetBranchStatus("MY2_Device_Error_Code", 1);
  }
}

void QwMpsOnly::SetupMpsBranchAddresses(Bool_t includeFriendTreeLeaves,
					Bool_t includeMonitorsAndDetectors,
					Bool_t includeBlocksAndFuncGen = 1)
{
  fChain->ResetBranchAddresses();
  SetupMpsBranchStatuses(includeFriendTreeLeaves, includeMonitorsAndDetectors,
			 includeBlocksAndFuncGen);
  fChain->SetBranchAddress("bm_pattern_number", &bm_pattern_number);
  fChain->SetBranchAddress("ErrorFlag", &ErrorFlag);
  fChain->SetBranchAddress("ramp", &ramp_hw_sum);
  if( includeBlocksAndFuncGen ){
    fChain->SetBranchAddress("fgx1", &fgx1_hw_sum);
    fChain->SetBranchAddress("fgy1", &fgy1_hw_sum);
    fChain->SetBranchAddress("fgx2", &fgx2_hw_sum);
    fChain->SetBranchAddress("fgy2", &fgy2_hw_sum);
    fChain->SetBranchAddress("ramp_block0", &ramp_block0);
    fChain->SetBranchAddress("ramp_block1", &ramp_block1);
    fChain->SetBranchAddress("ramp_block2", &ramp_block2);
    fChain->SetBranchAddress("ramp_block3", &ramp_block3);
  }
  if(includeFriendTreeLeaves){
    fChain->SetBranchAddress("ramp_filled", &ramp_filled);
    fChain->SetBranchAddress("ramp_good", &ramp_good);
  }

  if(includeMonitorsAndDetectors){
    Scan(1);
    Scan(0);
  }
}

void QwMpsOnly::SetPhaseValues(Double_t *val)
{

  phase.resize(5);
  for(Int_t i = 0; i < kNMod; i++){
    phase[i] = val[i];
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



void QwMpsOnly::Write(Bool_t run_avg){
  //Write single cycle slopes to different directory and files 
  //than run averaged slopes.

  gSystem->Exec("umask 002");

  if(!run_avg){

    //Write output to individual macrocycle coefficient files here.
    //////////////////////////////////////////////////////////////
    Int_t nMod = kNCoil;
    if(!macrocycle_coeffs.is_open()){
      macrocycle_coeffs.open(Form("%s/macrocycle_slopes/"
				  "coil_coeffs%s_%i%s.%s.dat", 
				  output.Data(), fFileSegment.Data(), run_number, 
				  (fChiSquareMinimization ? "_ChiSqMin" : ""),
				  fSetStem.Data()), fstream::out);
    }
    if(macrocycle_coeffs.is_open() && macrocycle_coeffs.good()){
      macrocycle_coeffs<<"Macrocycle "<<fMacroCycleNum<<" \t";
      for(Int_t imod = 0;imod<kNMod;imod++){
	macrocycle_coeffs<<"Mod"<<imod<<" "<<fCycleNum[imod]<<" \t";
      }
      macrocycle_coeffs<<"Id "<<run_number<<fMacroCycleNum<<"  first_entry "<<
	fFirstEntry<<"  last_entry "<<fLastEntry<<std::endl;
      for(Int_t imon = 0; imon < fNMonitor; imon++){
	macrocycle_coeffs << "mon " << MonitorList[imon].Data() << std::endl;
	for(Int_t imod = 0; imod < nMod; imod++){
	  Double_t sl = MonitorSlope[imod][imon].back();
	  Double_t slerr = MonitorSlopeError[imod][imon].back();
	  Double_t mn = MonitorMean[imod%kNMod][imon];
	  Double_t mnerr = MonitorMeanError[imod%kNMod][imon];
	  macrocycle_coeffs << Form("%+14.7e\t%12.5e   %+14.7e\t%12.5e\n",
				    sl, slerr, mn, mnerr);
	}
      }

      for(Int_t idet = 0; idet < fNDetector; idet++){
	macrocycle_coeffs << "det " << DetectorList[idet].Data() << std::endl;
	for(Int_t imod = 0; imod < nMod; imod++){
	  Double_t sl = DetectorSlope[imod][idet].back();
	  Double_t slerr = DetectorSlopeError[imod][idet].back();
	  Double_t mn = DetectorMean[imod%kNMod][idet];
	  Double_t mnerr = DetectorMeanError[imod%kNMod][idet];
	  macrocycle_coeffs << Form("%+14.7e\t%12.5e   %+14.7e\t%12.5e\n",
				    sl, slerr, mn, mnerr);
	}
      }

    }else std::cout<<"Macrocycle coefficients file not opened.\n";
    /////////////////////////////////////////////////////////////


    //Write sine and cosine residuals to file by macrocycle here.
    /////////////////////////////////////////////////////////////
    if(!macrocycle_sin_res.is_open()){
      macrocycle_sin_res.open(Form("%s/macrocycle_slopes/"
				  "run%iSineAmpl%s.%s.dat", 
				  output.Data(), run_number, 
				  (fChiSquareMinimization ? "_ChiSqMin" : ""),
				  fSetStem.Data()), fstream::out);
    }
    if(macrocycle_sin_res.is_open() && macrocycle_sin_res.good()){
      macrocycle_sin_res<<"Macrocycle "<<fMacroCycleNum<<" \t";
      for(Int_t imod = 0;imod<kNMod;imod++){
	macrocycle_sin_res<<"Mod"<<imod<<" "<<fCycleNum[imod]<<" \t";
      }
      macrocycle_sin_res<<"Id "<<run_number<<fMacroCycleNum<<"  first_entry "<<
	fFirstEntry<<"  last_entry "<<fLastEntry<<std::endl;

      for(int idet=0; idet<fNDetector;++idet){
	macrocycle_sin_res<<Form("%-16s", DetectorList[idet].Data());
	for(int imod = 0; imod<kNMod;++imod){
	  macrocycle_sin_res<<Form("% 10.3e  %10.3e   ",
				   DetectorSinResidual[imod][idet].back(),
				   DetectorSinResidualError[imod][idet].back());
	}
	macrocycle_sin_res<<std::endl;
      }
    }else{
      std::cout<<"Failed to open residuals file. Exiting.\n";
      exit(0);
    }

    if(!macrocycle_cos_res.is_open()){
      macrocycle_cos_res.open(Form("%s/macrocycle_slopes/"
				  "run%iCosineAmpl%s.%s.dat", 
				  output.Data(), run_number, 
				  (fChiSquareMinimization ? "_ChiSqMin" : ""),
				  fSetStem.Data()), fstream::out);
    }
    if(macrocycle_cos_res.is_open() && macrocycle_cos_res.good()){
      macrocycle_cos_res<<"Macrocycle "<<fMacroCycleNum<<" \t";
      for(Int_t imod = 0;imod<kNMod;imod++){
	macrocycle_cos_res<<"Mod"<<imod<<" "<<fCycleNum[imod]<<" \t";
      }
      macrocycle_cos_res<<"Id "<<run_number<<fMacroCycleNum<<"  first_entry "<<
	fFirstEntry<<"  last_entry "<<fLastEntry<<std::endl;

      for(int idet=0; idet<fNDetector;++idet){
	macrocycle_cos_res<<Form("%-16s",DetectorList[idet].Data());
	for(int imod = 0; imod<kNMod;++imod){
	  macrocycle_cos_res<<Form("% 10.3e  %10.3e   ",
				   DetectorCosResidual[imod][idet].back(),
				   DetectorCosResidualError[imod][idet].back());
	}
	macrocycle_cos_res<<std::endl;
      }
    }else{
      std::cout<<"Failed to open residuals file. Exiting.\n";
      exit(0);
    }

    //Write output to individual macrocycle slopes files here.
    /////////////////////////////////////////////////////////
    if(!macrocycle_slopes.is_open()){
      macrocycle_slopes.open(Form("%s/macrocycle_slopes/"
				  "macrocycle_slopes%s_%i%s.%s.dat", 
				  output.Data(), fFileSegment.Data(), run_number, 
				  (fChiSquareMinimization ? "_ChiSqMin" : ""),
				  fSetStem.Data()), fstream::out);
    }
    if(macrocycle_slopes.is_open() && macrocycle_slopes.good()){
      macrocycle_slopes<<"Macrocycle "<<fMacroCycleNum<<" \t";
      for(Int_t imod = 0;imod<kNMod;imod++){
	macrocycle_slopes<<"Mod"<<imod<<" "<<fCycleNum[imod]<<" \t";
      }
      macrocycle_slopes<<"Id "<<run_number<<fMacroCycleNum<<"  first_entry "<<
	fFirstEntry<<"  last_entry "<<fLastEntry<<std::endl;

      for(Int_t idet = 0; idet < fNDetector; idet++){
	macrocycle_slopes << "det " << DetectorList[idet].Data() << std::endl;
	for(Int_t imon = 0; imon < fNMonitor; imon++){
	  Double_t sl = YieldSlope[idet][imon];
	  Double_t err = YieldSlopeError[idet][imon];
	  macrocycle_slopes << Form("%+14.7e\t%12.5e\n",sl, err);
	}
      }
    }else std::cout<<"Macrocycle slopes file not open.\n";
    /////////////////////////////////////////////////////////
 

   return;
  }

  slopes.open(Form("%s/slopes/slopes%s_%i%s.%s.dat", output.Data(), 
		   fFileSegment.Data(), run_number, 
		   (fChiSquareMinimization ? "_ChiSqMin" : ""),
		   fSetStem.Data()), fstream::out);
  regression = fopen(Form("%s/regression/regression%s_%i%s.%s.dat", output.Data(),
			  fFileSegment.Data(), run_number,
			  (fChiSquareMinimization ? "_ChiSqMin" : ""),
			  fSetStem.Data()), "w");

  if( (slopes.is_open() && slopes.good()) ){
    for(Int_t idet = 0; idet < fNDetector; idet++){
      slopes << "det " << DetectorList[idet] << std::endl;
      for(Int_t imon = 0; imon < fNMonitor; imon++){
	slopes << Form("%+14.7e",YieldSlope[idet][imon]) << "\t"
	       << Form("%12.5e",YieldSlopeError[idet][imon]) << std::endl;
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
    for(Int_t i = 0; i < kNMod; i++){
      diagnostic << ModulationEvents[i]  << std::endl;
      fNModEvents += ModulationEvents[i];
    }
    diagnostic << fNModEvents << std::endl;
  }
  diagnostic<<"\n\nRamp information\n";
  diagnostic <<"ramp phase offset\t"<<fRampOffset<<std::endl;
  diagnostic <<"ramp period\t"<<fRampPeriod<<std::endl;
  diagnostic <<"ramp maximum\t"<<fRampMax<<std::endl;
  diagnostic <<"ramp length\t"<<fRampLength<<std::endl;
  diagnostic <<"degrees per mps\t"<<fDegPerMPS<<std::endl;

  diagnostic <<"\nRamp period fit results for function generator signals\n";
  for(int i=0;i<4;i++){
    diagnostic <<fg[i]<<"\t"<<Form("%13.6e",rampPeriodFitRslt[i])<<"\t"<<
      Form("%12.5e",rampPeriodFitRsltErr[i])<<std::endl;
  }

  diagnostic <<"\nRamp offset fit results for function generator signals\n";
    for(int i=0;i<4;i++){
      diagnostic <<fg[i]<<"\t"<<Form("%13.6e",rampOffsetFitRslt[i])<<"\t"<<
	Form("%12.5e",rampOffsetFitRsltErr[i])<<std::endl;
  }



  //***************************************************************
  // Write output file for regression - This will go into the DB.  
  // All that is included here is the slopes(and errors soon...),
  // qwlibra must be run to get other run info.
  //***************************************************************

  fprintf(regression, "#run=%i\tevents=%i\n", run_number, fNModEvents);
  if( regression != NULL ){
    for(Int_t i = 0; i < fNDetector; i++){
      for(Int_t j = 0; j < fNMonitor; j++){
	fprintf(regression, "%s/%s : %i : %i : %e : %e\n", DetectorList[i].Data(),
		MonitorList[j].Data(), 
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
  macrocycle_slopes.close();
  macrocycle_coeffs.close();
  macrocycle_cos_res.close();
  macrocycle_sin_res.close();
  diagnostic.close();
  
  return;
}

#endif

//  LocalWords:  SetupMpsBranchAddress
