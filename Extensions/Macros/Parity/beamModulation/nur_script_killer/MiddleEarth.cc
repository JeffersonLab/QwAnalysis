#define MiddleEarth_cxx
#include "MiddleEarth.hh"
#include "headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#ifdef MiddleEarth_cxx

MiddleEarth::MiddleEarth(TChain *tree):
  fXinit(false), fYinit(false), fEinit(false), 
  fXPinit(false), fYPinit(false)
{
   Init(tree);
}

MiddleEarth::~MiddleEarth()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MiddleEarth::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MiddleEarth::LoadTree(Long64_t entry)
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

void MiddleEarth::Init(TChain *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   Notify();

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("event_number", 1);
   fChain->SetBranchStatus("ramp", 1);
   fChain->SetBranchStatus("bm_pattern_number", 1);
   fChain->SetBranchStatus("ErrorFlag", 1);

   fChain->SetBranchAddress("event_number", &event_number, &b_event_number);
   fChain->SetBranchAddress("ramp", &ramp_hw_sum, &b_ramp);
   fChain->SetBranchAddress("bm_pattern_number", &bm_pattern_number, &b_bm_pattern_number);
   fChain->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);

}

Bool_t MiddleEarth::Notify()
{
   return kTRUE;
}

void MiddleEarth::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MiddleEarth::Cut(Long64_t entry)
{
   return 1;
}

#endif // #ifdef Middle_Earth_cxx

Bool_t MiddleEarth::FileSearch(TString filename, TChain *chain)
{

  TString file_directory;
  Bool_t c_status = kFALSE;

  file_directory = gSystem->Getenv("QW_ROOTFILES");

  c_status = chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()));
  std::cout << "Trying to open :: "
            << Form("%s/%s",file_directory.Data(), filename.Data())
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

void MiddleEarth::LoadRootFile(TString filename, TChain *tree)
{
  Bool_t found = FileSearch(filename, tree);
  
  if(!found){
    filename = Form("Qweak_%d.00[0-1].trees.root", run_number);
    found = FileSearch(filename, tree);
    std::cerr << "Couldn't find QwPass1_*.trees.root trying "
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

void MiddleEarth::ScanData(void)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntries();
   std::cout << "Here we will scan through the data for modulation." << std::endl;
   std::cout << "This run contains(entries):\t" << nentries << std::endl;
   for (Long64_t i = 0; i < nentries; i++) {
     //      Long64_t ientry = LoadTree(i);
//       if (ientry < 0) break;
      fChain->GetEntry(i);   
    
      if( (i % 10000) == 0) std::cout << "Processing:\t" << i << std::endl; 
      
      if(bm_pattern_number == 11 && ErrorFlag == 0x4018080){
	std::cout << "Found X modulation" << std::endl;
	do{
	  fChain->GetEntry(i);
	  if(ErrorFlag != 0x4018080){
	    std::cout << "::Error::\t" << i << std::endl;
	    i++;
	    continue;
	  }
	  fXinit = true;
	  i++;
	}while(bm_pattern_number == 11 && ramp_hw_sum > 0);
      }

      if(bm_pattern_number == 12 && ErrorFlag == 0x4018080){
	std::cout << "Found Y modulation" << std::endl;
	do{
	  fChain->GetEntry(i);
	  if(ErrorFlag != 0x4018080){ 
	    std::cout << "::Error::\t" << i << std::endl;
	    i++; 
	    continue;
	  }
	  fYinit = true;
	  i++;
	}while(bm_pattern_number == 12 && ramp_hw_sum > 0);
      }
      if(bm_pattern_number == 13 && ErrorFlag == 0x4018080){
	std::cout << "Found E modulation" << std::endl;
	do{
	  fChain->GetEntry(i);
	  if(ErrorFlag != 0x4018080){
	    std::cout << "::Error::\t" << i << std::endl;
	    i++;
	    continue;
	  }
	  fEinit = true;
	  i++;
	}while(bm_pattern_number == 13 && ramp_hw_sum > 0);
      }
      if(bm_pattern_number == 14 && ErrorFlag == 0x4018080){
	std::cout << "Found XP modulation" << std::endl;
	do{
	  fChain->GetEntry(i);
	  if(ErrorFlag != 0x4018080){
	    i++;
	    std::cout << "::Error::\t" << i << std::endl;
	    continue;
	  }

	  fXPinit = true;
	  i++;
	}while(bm_pattern_number == 14 && ramp_hw_sum > 0);
      }
      if(bm_pattern_number == 15 && ErrorFlag == 0x4018080){
	std::cout << "Found YP modulation" << std::endl;
	do{
	  fChain->GetEntry(i);
	  if(ErrorFlag != 0x4018080){
	    std::cout << "::Error::\t" << i << std::endl;
	    i++;
	    continue;
	  }
	  fYPinit = true;
	  i++;
	}while(bm_pattern_number == 15 && ramp_hw_sum > 0);
      }
   }

}

Bool_t MiddleEarth::CheckFlags(void){

  if(fXinit && fYinit && fEinit && fXPinit && fYPinit) return true;
  else 
    return false;

}
