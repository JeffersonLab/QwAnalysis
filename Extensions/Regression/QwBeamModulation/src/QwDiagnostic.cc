#define QwDiagnostic_cxx
#include "QwDiagnostic.hh"
#include "headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#ifdef QwDiagnostic_cxx

const char QwDiagnostic::red[8] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
const char QwDiagnostic::other[8] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
const char QwDiagnostic::normal[8] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

QwDiagnostic::QwDiagnostic(TChain *tree):
  kXModulation(0),kYModulation(3),kEModulation(2),
  kXPModulation(1),kYPModulation(4), fNEvents(0),
  fNModType(5)
{
   Init(tree);
}

QwDiagnostic::~QwDiagnostic()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Bool_t QwDiagnostic::IfExists(const char *file)
{
  if(FILE *file_to_check = fopen(file, "r")){
    fclose(file_to_check);
    return true;
  }
  return false;
}

void QwDiagnostic::ReadSensitivities(void)
{

  std::string line;
  std::fstream sens_file;  
  char *token;
  Int_t n = -1;

  sens_file.open(Form("%s/slopes/slopes_%d.dat", output.Data(), run_number));
  if(!sens_file.is_open()){
    std::cout << red << "Error opening slopes file" << normal << std::endl;
    exit(1);
  }

  Sens.resize(fNDetector);
  SensError.resize(fNDetector);

  while(sens_file.good()){
    getline(sens_file, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,\t");
    while(token){
      if(strncmp(token, "det", 1) == 0){
	token = strtok(NULL, " ,\t");
	n++;
	token = strtok(NULL, " ,\t");
      }
      else{
	if(n > -1){
          Sens[n].push_back(atof(token) );
 	  token = strtok(NULL, " ,\t");
	  SensError[n].push_back(atof(token) );
	}
        token = strtok(NULL, " ,\t");
      }
    }
  }
  sens_file.close();
  
  return;
}

void QwDiagnostic::Write(void){

  slope_diagnostic = fopen(Form("%s/diagnostics/%s_diagnostic/slope_diagnostic.txt", output.Data(), fFileName.Data()),"w");
  correction = fopen(Form("%s/diagnostics/%s_diagnostic/correction.txt", output.Data(),  fFileName.Data()), "w");  
  charge_asym_diagnostic = fopen(Form("%s/diagnostics/%s_diagnostic/charge_asym_diagnostic.txt", output.Data(), fFileName.Data()), "w");  

  //***************************************************** 
  //  Write to DB file - This file should exist already,
  //  if not DIE!
  //*****************************************************

  ReadSensitivities();

  if( !IfExists( Form("%s/regression/regression_%i.dat", output.Data(), run_number) ) ){
    std::cout << red << "Error finding regression file.  Cleaning up directories..." << normal << std::endl;
    CleanFolders();
    exit(1);
  }
  regression = fopen(Form("%s/regression/regression_%i.dat", output.Data(), run_number), "a");

  // Uncorrected Slopes during modulation

//   for(Int_t k = 0; k < fNDetector; k++){  
//     fprintf(regression, "\n# uncorrected sensitivities %s (during modulation)\n", DetectorList[k].Data() );
//     for(Int_t i = 0; i < fNMonitor; i++){  
//       for(Int_t j = 0; j < GetModNumber(); j++){
// 	fprintf(regression, "%s : %i : %-5.5e : %-5.5e : %-5.5e \n", 
// 		MonitorList[i].Data(), j,Slope[k][i][j]->slope, Slope[k][i][j]->error, Slope[k][i][j]->ChiSquareNDF);
//       }
//     }
//   }

  // Corrected Slopes during modulation

//   for(Int_t k = 0; k < fNDetector; k++){  
//     fprintf(regression, "\n# corrected sensitivities %s (during modulation)\n", DetectorList[k].Data() );
//     for(Int_t i = 0; i < fNMonitor; i++){  
//       for(Int_t j = 0; j < GetModNumber(); j++){
// 	fprintf(regression, "%s : %i : %-5.5e : %-5.5e : %-5.5e \n", 
// 		MonitorList[i].Data(), j,SlopeCorr[k][i][j]->slope, SlopeCorr[k][i][j]->error, SlopeCorr[k][i][j]->ChiSquareNDF);
//       }
//     }
//   }

  // Corrections

  Double_t total = 0;
  Double_t rms = 0;
  Int_t total_entries = 0;

  for(Int_t k = 0; k < fNDetector; k++){
    fprintf(regression, "\n# corrections %s\n", DetectorList[k].Data());
    for(Int_t i = 0; i < fNMonitor; i++){
      fprintf(regression, "%s :%d: %-5.5e : %-5.5e \n", MonitorList[i].Data(), Correction[k][i]->entries, Correction[k][i]->slope, Correction[k][i]->error);
//       total += Correction[k][i]->slope;
//       rms += TMath::Power( Correction[k][i]->GetRMS(), 2);
//       total_entries += Correction[k][i]->entries;
    }
//     rms = TMath::Sqrt(rms);
//     fprintf(regression, "%s total:%d: %-5.5e : %-5.5e: %-5.5e : %-5.5e \n", DetectorList[k].Data(), 
//  	    total_entries, total, rms, TotalCorrection[k]->slope, TotalCorrection[k]->GetRMS());
    rms = 0;
    total = 0;
    total_entries = 0;
  }

  // Differences

//     fprintf(regression, "\n# differences\n");
//     for(Int_t i = 0; i < fNMonitor; i++){
//       fprintf(regression, "%s :%d: %-5.5e : %-5.5e \n", MonitorList[i].Data(), DiffSlope[i]->entries, DiffSlope[i]->slope, DiffSlope[i]->GetRMS());
//     }

    // Corrected Asymmetry
    fprintf(regression, "\n# corrected asymmetry \n");
    for(Int_t i = 0; i < fNDetector; i++){
      fprintf(regression, "%s :%d:%-5.5e : %-5.5e \n", DetectorList[i].Data(), (Int_t)AsymEntries[i], AsymMean[i], AsymRMS[i]/TMath::Sqrt(AsymEntries[i]) );
    }

  //******************** 
  //      Slopes 
  //********************


    for(Int_t k = 0; k < fNDetector; k++){  

    fprintf(slope_diagnostic, "#uncorrected slopes %s\n", DetectorList[k].Data());
    for(Int_t i = 0; i < fNMonitor; i++){  
      for(Int_t j = 0; j < GetModNumber(); j++){
	fprintf(slope_diagnostic, "%-8.6e +- %-8.6e\t", Slope[k][i][j]->slope, Slope[k][i][j]->error);
      }
      fprintf(slope_diagnostic, "\n");
    }
    
    fprintf(slope_diagnostic, "\n\n#uncorrected slopes X2NDF %s\n", DetectorList[k].Data());
    for(Int_t i = 0; i < fNMonitor; i++){  
      for(Int_t j = 0; j < GetModNumber(); j++){
	fprintf(slope_diagnostic, "%-8.6e\t", Slope[k][i][j]->ChiSquareNDF);
      }
      fprintf(slope_diagnostic, "\n");
    }

    fprintf(slope_diagnostic, "\n\n#corrected slopes %s\n", DetectorList[k].Data());
    for(Int_t i = 0; i < fNMonitor; i++){  
      for(Int_t j = 0; j < GetModNumber(); j++){
	fprintf(slope_diagnostic, "%-8.6e +- %-8.6e\t", SlopeCorr[k][i][j]->slope, SlopeCorr[k][i][j]->error);
      }
      fprintf(slope_diagnostic, "\n");
    }
    
    fprintf(slope_diagnostic, "\n\n#corrected slopes X2NDF %s\n", DetectorList[k].Data());
    for(Int_t i = 0; i < fNMonitor; i++){  
      for(Int_t j = 0; j < GetModNumber(); j++){
	fprintf(slope_diagnostic, "%-8.6e\t", SlopeCorr[k][i][j]->ChiSquareNDF);
      }
      fprintf(slope_diagnostic, "\n");
    }
  }


  //******************** 
  //    corrections
  //********************
    Double_t conv_key[5] = {1.e6, 1., 1.e6, 1.e6, 1.};
    Double_t total_correction = 0; 
    Double_t corr_rms = 0;

    for(Int_t k = 0; k < fNDetector; k++){
      fprintf(correction,"# Detector: %s\n", DetectorList[k].Data());
      fprintf(correction,"#monitor\t   sensitivity(ppm/[mm,urad])     difference([mm,rad])\t\t  Asym_Corr(ppm)\n");
      for(Int_t i = 0; i < fNMonitor; i++){
	fprintf(correction, "%-20s\t %-8.6e\t\t %-8.6e +- %-8.6e\t%-8.6e +- %-8.6e\n", MonitorList[i].Data(), conv_key[i]*Sens[k][i],
		DiffSlope[i]->slope, DiffSlope[i]->GetRMS(), 1e6*Correction[k][i]->slope, 1e6*Correction[k][i]->GetRMS());
	total_correction += 1e6*Correction[k][i]->slope;
	corr_rms += TMath::Power( 1e6*Correction[k][i]->GetRMS(), 2);
      }
      corr_rms = TMath::Sqrt(corr_rms);
      fprintf(correction,"\nasym_raw(ppm)\t\t\t\t    correction(ppm)[SOM]\t\t\t  correction(ppm)[MOS]\t\t\t\t   asym_final(ppm)\n");
      fprintf(correction, "%-8.6e +- %-8.6e\t\t  %-8.6e +- %-8.6e\t\t %-8.6e +- %-8.6e\t\t %-8.6e +- %-8.6e\t\t\n", 
	      RawAsymMean[k], RawAsymRMS[k], total_correction, corr_rms, 1e6*TotalCorrection[k]->slope, 1e6*TotalCorrection[k]->GetRMS(), AsymMean[k], AsymRMS[k]);

      total_correction = 0;
      corr_rms = 0;
    }

  //*********************** 
  //    charge asymmetry
  //***********************

    fprintf(charge_asym_diagnostic,"\n\n#monitor\t\t  mod type\t\t\t   Chi2NDF\n");
    for(Int_t i = 0; i < fNMonitor; i++){
      fprintf(charge_asym_diagnostic, "%-20s\t%-8.6e +- %-8.6e\t%-8.6e\n", MonitorList[i].Data(),
	      ChargeAsym[i]->slope, ChargeAsym[i]->error, ChargeAsym[i]->ChiSquareNDF);
    }

  fclose(slope_diagnostic);
  fclose(correction);
  fclose(charge_asym_diagnostic);
  fclose(regression);
  
}

void QwDiagnostic::BuildVectors(){
  Slope.resize(fNDetector);
  SlopeCorr.resize(fNDetector);  
  for(Int_t j= 0; j < fNDetector; j++){
    for(Int_t i= 0; i < fNModType; i++){
      Slope[j].push_back(std::vector <QwData *>());
      SlopeCorr[j].push_back(std::vector <QwData *>());  
    }
  }
  Correction.resize(fNDetector);
  NatSlope.resize(fNDetector);

  return;
}

Int_t QwDiagnostic::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t QwDiagnostic::LoadTree(Long64_t entry)
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

void QwDiagnostic::Init(TChain *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   Notify();
}

Bool_t QwDiagnostic::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void QwDiagnostic::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t QwDiagnostic::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


void QwDiagnostic::SetFileName(TString & filename)
{
  fFileName = filename;
  return;
}

void QwDiagnostic::Clean()
{
  //
  // This function serves the purpose of deallocating 
  // memory for unused vectors in the QwDiagnostic Class.
  // Should be run after finishing with the slope calculation.
  //

    return;
}

void QwDiagnostic::ReadSlopes()
{
}

Int_t QwDiagnostic::ReadConfig(QwDiagnostic *meteor)
{
  std::string line;

  char *token;

  char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
  char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };


  config.open("config/setup.config", std::ios_base::in);

  if(!config.is_open()){
    std::cout << red << "Error opening config file" << normal << std::endl;
    CleanFolders();
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
       	std::cout << red << "\t\tMonitor is: " << token << normal << std::endl;
	meteor->MonitorList.push_back(token); 
       }
      if(strcmp("det", token) == 0){

       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
       	std::cout << red << "\t\tDetector is: " << token << normal << std::endl; 
	meteor->DetectorList.push_back(token);
      }
      else 
       	token = strtok(NULL, " .,"); 
    }
  }
  fNDetector = DetectorList.size();
  fNMonitor = MonitorList.size();
  BuildVectors();

  config.close();
  return 0;
}

Bool_t QwDiagnostic::FileSearch(TString filename, TChain *chain)
{

  TString file_directory;
  Bool_t c_status = kFALSE;

  //  file_directory = gSystem->Getenv("QW_ROOTFILES");

  output = gSystem->Getenv("BMOD_OUT");
  if(!gSystem->OpenDirectory(output)){
    PrintError("Cannot open output directory.\n");
    PrintError("Directory needs to be set as env variable and contain:\n\t slopes/ \n\t regression/ \n\t diagnostics/ \n\t rootfiles/"); 
    exit(1);
  }

  c_status = chain->Add(Form("%s/rootfiles/%s", output.Data(), filename.Data()));
  std::cout << "Trying to open :: "
            << Form("%s/rootfiles/%s", output.Data(), filename.Data())
            << std::endl;

  if(c_status){
    TString chain_name = chain->GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;

    while((chain_element = (TChainElement*)next())){
      std::cout << "Adding :: "
		<< filename
		<< " to data chain"
		<< std::endl;
    }
  } 
    return c_status;

}

void QwDiagnostic::LoadRootFile(TString filename, TChain *tree)
{
  Bool_t found = FileSearch(filename, tree);
  
  if(!found){
    filename = Form("Qweak_%d*.root", run_number);
    found = FileSearch(filename, tree);
    std::cerr << "Couldn't find QwPass1_*.root trying "
	      << filename
	      << std::endl;
    if(!found){
      std::cerr << "Unable to locate requested file :: "
		<< filename
		<< std::endl;
      exit(1);
    }
  }
}

Int_t QwDiagnostic::GetModNumber(){return fNModType;}

void QwDiagnostic::PrintError(TString error){

  std::cout << red << error << normal << std::endl;

  return;
}

void QwDiagnostic::CleanFolders()
{
  gSystem->Exec(Form("rm -rf %s/diagnostics/%s_diagnostic", output.Data(), fFileName.Data()));
  return;
}

#endif

//  LocalWords:  SetupMpsBranchAddress
