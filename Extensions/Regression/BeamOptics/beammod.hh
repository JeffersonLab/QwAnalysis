#ifndef BEAMMOD_HH
#define BEAMMOD_HH
#include "headers.h"

using namespace std;

Int_t run_number;

Bool_t found;

Bool_t FileSearch(TString filename, TChain *chain)
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

void LoadRootFile(TString filename, TChain *tree, Int_t run_number){
  found = FileSearch(filename, tree);

  if(!found){
      filename = Form("Qweak_%d*.trees.root", run_number);
      found = FileSearch(filename, tree);
      std::cerr << "Couldn't find QwPass1_*.root trying "
		<< filename
		<< std::endl;
    }
  else if(!found){
      filename = Form("first100k_%d*.root", run_number);
      found = FileSearch(filename, tree);
      std::cerr << "Couldn't find Qweak*.root trying "
		<< filename
		<< std::endl;
    }
  else if(!found){
      std::cerr << "Unable to locate requested file :: "
		<< filename
		<< std::endl;
      exit(1);
    }
}

#endif
