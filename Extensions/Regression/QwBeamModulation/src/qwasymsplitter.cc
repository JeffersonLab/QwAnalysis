#include "headers.h"
#include "QwModulation.hh"
#include "TLeaf.h"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  TChain *mod_tree = new TChain("Mod_Tree");

  QwModulation *modulation = new QwModulation(mod_tree);

  FILE *regression;

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

  regression = fopen(Form("%s/regression/%s/regression%s_%i.%s.dat", modulation->output.Data(), modulation->fSetStem.Data(),
			  modulation->fFileSegment.Data(), modulation->run_number, modulation->fSetStem.Data()), "a");
  if(regression == NULL){
    std::cerr << "Failure to open regression file." << std::endl;
    exit(1);
  }
  

  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;

  modulation->ReadConfig("bmod");
  modulation->fNumberEvents = mod_tree->GetEntries();

  std::cout << "Number of entries: " << modulation->fNumberEvents << std::endl;

  Double_t bpm_limit[5] = {1.0, 1.0e-5, 1.0, 1.0, 1.0e-5};
  Double_t ErrorFlag;

  TBranch *b_ErrorFlag;

  std::vector <Double_t> leaf_value(modulation->fNDetector);
  std::vector <Double_t> raw_leaf_value(modulation->fNDetector);
  std::vector <Double_t> bpm_leaf_value(modulation->fNDetector);

  std::vector <TBranch *> b_leaf_value(modulation->fNDetector);
  std::vector <TBranch *> b_raw_leaf_value(modulation->fNDetector);
  std::vector <TBranch *> b_bpm_leaf_value(modulation->fNMonitor);

  std::vector <TH1F *> mod_hist(modulation->fNDetector);
  std::vector <TH1F *> nbm_hist(modulation->fNDetector);
  std::vector <TH1F *> raw_hist(modulation->fNDetector);
  std::vector <TH1F *> bpm_hist(modulation->fNMonitor);
  
  mod_tree->SetBranchStatus("*", 0);     
  mod_tree->SetBranchStatus("ErrorFlag", 1);   
  mod_tree->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);

  for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
    mod_tree->SetBranchStatus(Form("diff_%s", modulation->MonitorList[mon].Data()), 1);   
    mod_tree->SetBranchAddress(Form("diff_%s", modulation->MonitorList[mon].Data()), 
			       &bpm_leaf_value[mon], &b_bpm_leaf_value[mon]);   
    bpm_hist[mon] = new TH1F(Form("hist_%s", modulation->MonitorList[mon].Data()),
			     Form("hist_%s", modulation->MonitorList[mon].Data()), 10000, -bpm_limit[mon], bpm_limit[mon]);
  }  

  for(Int_t det = 0; det < modulation->fNDetector; det++){
    mod_tree->SetBranchStatus(Form("corr_asym_%s", modulation->DetectorList[det].Data()), 1);   
    mod_tree->SetBranchStatus(Form("asym_%s", modulation->DetectorList[det].Data()), 1);   

    mod_tree->SetBranchAddress(Form("corr_asym_%s", modulation->DetectorList[det].Data()), 
			       &leaf_value[det], &b_leaf_value[det]);   
    mod_tree->SetBranchAddress(Form("asym_%s", modulation->DetectorList[det].Data()), 
			       &raw_leaf_value[det], &b_raw_leaf_value[det]);   

    mod_hist[det] = new TH1F(Form("hist_%s", modulation->DetectorList[det].Data()),
			     Form("hist_%s", modulation->DetectorList[det].Data()), 10000, -1e-2, 1e-2);
    nbm_hist[det] = new TH1F(Form("nbm_hist_%s", modulation->DetectorList[det].Data()),
			     Form("nbm_hist_%s", modulation->DetectorList[det].Data()), 10000, -1e-2, 1e-2);
    raw_hist[det] = new TH1F(Form("raw_hist_%s", modulation->DetectorList[det].Data()),
			     Form("raw_hist_%s", modulation->DetectorList[det].Data()), 10000, -1e-2, 1e-2);
  }

  for(Long64_t i = 0; i < modulation->fNumberEvents; i++){

    modulation->LoadTree(i);
    if(i < 0) break;
    modulation->fChain->GetEntry(i);
    
    if((i % 100000) == 0) std::cout << "processing: " << i << std::endl;
    
    if((UInt_t)ErrorFlag == 0x4018080){
      for(Int_t det = 0; det < modulation->fNDetector; det++){
	mod_hist[det]->Fill(leaf_value[det]);      
	raw_hist[det]->Fill(raw_leaf_value[det]);      
      }
      for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
	bpm_hist[mon]->Fill(bpm_leaf_value[mon]);
      }
    }
    if(ErrorFlag == 0){
      for(Int_t det = 0; det < modulation->fNDetector; det++){
	nbm_hist[det]->Fill(leaf_value[det]);      
      }
    }
  }

  std::cout << modulation->fNDetector << std::endl;
  std::cout << "Closing Mod_Tree" << std::endl;

  fprintf(regression, "\n# cp corrected asymmetry \n");
  for(Int_t det = 0; det < modulation->fNDetector; det++){
    fprintf(regression, "%s :%d:%-5.5e : %-5.5e \n", modulation->DetectorList[det].Data(), 
	    (Int_t)(mod_hist[det]->GetEntries()), mod_hist[det]->GetMean(), 
	    (mod_hist[det]->GetRMS())/TMath::Sqrt(mod_hist[det]->GetEntries()) );
  }
  fprintf(regression, "\n# cp bpm differences \n");
  for(Int_t mon = 0; mon < modulation->fNMonitor; mon++){
    fprintf(regression, "%s :%d:%-5.5e : %-5.5e \n", modulation->MonitorList[mon].Data(), 
	    (Int_t)(bpm_hist[mon]->GetEntries()), bpm_hist[mon]->GetMean(), 
	    (bpm_hist[mon]->GetRMS())/TMath::Sqrt(bpm_hist[mon]->GetEntries()) );
  }  

  fprintf(regression, "\n# nbm corrected asymmetry \n");
  for(Int_t det = 0; det < modulation->fNDetector; det++){
    fprintf(regression, "%s :%d:%-5.5e : %-5.5e \n", modulation->DetectorList[det].Data(), 
	    (Int_t)(nbm_hist[det]->GetEntries()), nbm_hist[det]->GetMean(),
	    (nbm_hist[det]->GetRMS())/TMath::Sqrt(nbm_hist[det]->GetEntries()) );
  }

  fprintf(regression, "\n# cp raw asymmetry \n");
  for(Int_t det = 0; det < modulation->fNDetector; det++){
    fprintf(regression, "%s :%d:%-5.5e : %-5.5e \n", modulation->DetectorList[det].Data(), 
	    (Int_t)(raw_hist[det]->GetEntries()), raw_hist[det]->GetMean(),
	    (raw_hist[det]->GetRMS())/TMath::Sqrt(raw_hist[det]->GetEntries()) );
  }

  delete mod_tree;

  return 0;

}
