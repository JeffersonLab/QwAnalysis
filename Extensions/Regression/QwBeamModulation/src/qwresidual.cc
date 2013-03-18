#include "headers.h"
#include "QwModulation.hh"
#include "TLeaf.h"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  TChain *mod_tree = new TChain("Mod_Tree");

  QwModulation *modulation = new QwModulation(mod_tree);

  modulation->output = gSystem->Getenv("BMOD_OUT");
  if(!gSystem->OpenDirectory(modulation->output)){
    modulation->PrintError("Cannot open output directory.\n");
    modulation->PrintError("Directory needs to be set as env variable and contain:\n\t slopes/ \n\t regression/ \n\t diagnostics/ \n\t rootfiles/"); 
    exit(1);
  }
  
  modulation->GetOptions(argv);

  if( !(modulation->fRunNumberSet) ){
    modulation->PrintError("Error Loading:  no run number specified");
    exit(1);
  }
  filename = Form("bmod_tree%s_%d.%s.root", modulation->fFileSegment.Data(), modulation->run_number, modulation->fSetStem.Data());
  std::cout << Form("%s/rootfiles/%s/%s",modulation->output.Data(), modulation->fSetStem.Data(), filename.Data())  << std::endl;
  mod_tree->Add(Form("%s/rootfiles/%s/%s",modulation->output.Data(), modulation->fSetStem.Data(), filename.Data()));

  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;

  modulation->ReadConfig("bmod");
  modulation->fNumberEvents = mod_tree->GetEntries();

  std::cout << "Number of entries: " << modulation->fNumberEvents << std::endl;

  Double_t ErrorFlag;
  Double_t nEvents = 0;

  TBranch *b_ErrorFlag;

  TMatrixD DD(modulation->fNDetector, modulation->fNDetector);
  TMatrixD DM(modulation->fNDetector, modulation->fNMonitor);
  TMatrixD MM(modulation->fNMonitor, modulation->fNMonitor);
  TMatrixD residual(modulation->fNDetector, modulation->fNMonitor);
  TMatrixD sigma(modulation->fNDetector, modulation->fNMonitor);

  DD.Zero();
  DM.Zero();
  MM.Zero();
  residual.Zero();
  sigma.Zero();

  std::vector <Double_t> DetectorMean(modulation->fNDetector);
  std::vector <Double_t> MonitorMean(modulation->fNMonitor);

  std::vector <Double_t> det_value(modulation->fNDetector);
  std::vector <Double_t> mon_value(modulation->fNMonitor);

  std::vector <TBranch *> b_det_value(modulation->fNDetector);
  std::vector <TBranch *> b_mon_value(modulation->fNMonitor);
  
  mod_tree->SetBranchStatus("*", 0);     
  mod_tree->SetBranchStatus("ErrorFlag", 1);   
  mod_tree->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
  
  for(Int_t det = 0; det < modulation->fNDetector; det++){
    mod_tree->SetBranchStatus(Form("corr_asym_%s", modulation->DetectorList[det].Data()), 1);   
    mod_tree->SetBranchAddress(Form("corr_asym_%s", modulation->DetectorList[det].Data()), 
			       &det_value[det], &b_det_value[det]);   
  }

  for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
    mod_tree->SetBranchStatus(Form("diff_%s", modulation->MonitorList[mon].Data()), 1);   
    mod_tree->SetBranchAddress(Form("diff_%s", modulation->MonitorList[mon].Data()), 
			       &mon_value[mon], &b_mon_value[mon]);   

  }

  for(Long64_t i = 0; i < modulation->fNumberEvents; i++){

    modulation->LoadTree(i);
    if(i < 0) break;
    modulation->fChain->GetEntry(i);
    
    if((i % 100000) == 0) std::cout << "Pass 1 processing: " << i << std::endl;

    if( (ErrorFlag == 0) || (ErrorFlag == 0x4018080) ){
      for(Int_t det = 0; det < modulation->fNDetector; det++){
	DetectorMean[det] += det_value[det];
      }
      for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
	MonitorMean[mon] += mon_value[mon];
      }
      nEvents++;
    }
    
  }

  for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
    MonitorMean[mon] /= nEvents;
  }
  for(Int_t det = 0; det < modulation->fNDetector; det++){
    DetectorMean[det] /= nEvents;
  }

  for(Long64_t i = 0; i < modulation->fNumberEvents; i++){

    modulation->LoadTree(i);
    if(i < 0) break;
    modulation->fChain->GetEntry(i);
    
    if((i % 100000) == 0) std::cout << "Pass 2 processing: " << i << std::endl;

//     if( (ErrorFlag == 0) || (ErrorFlag == 0x4018080) ){
    if( (ErrorFlag == 0x4018080) ){
      for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
	MM(mon, mon) += (mon_value[mon] -  MonitorMean[mon])*(mon_value[mon] -  MonitorMean[mon]);
      }
      
      for(Int_t det = 0; det < modulation->fNDetector; det++){
	for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
	  DM(det, mon) += (det_value[det] - DetectorMean[det])*(mon_value[mon] - MonitorMean[mon]);	
	}
      }
      for(Int_t det = 0; det < modulation->fNDetector; det++){
	DD(det, det) += (det_value[det] -  DetectorMean[det])*(det_value[det] -  DetectorMean[det]);
      } 
    }
  }
  
  for(Int_t det = 0; det < modulation->fNDetector; det++){
    for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
      residual(det, mon) = DM(det, mon)/(MM(mon, mon));
      sigma(det, mon) = TMath::Sqrt((DD(det, det) - (DM(det, mon)*DM(det, mon))/MM(mon, mon))/(MM(mon, mon)*(nEvents - 2 ))); 
    }
  }

  residual.Print();
  sigma.Print();

  std::cout << "Closing Mod_Tree" << std::endl;

  delete mod_tree;

  return 0;

}
