#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

Bool_t globalEXIT;

const TString qwstem = "QwPass4b";

Bool_t FileSearch(TString, TChain *, 
				  Bool_t fSegInclude = false, 
				  Int_t fLowerSegment = 0, 
				  Int_t fUpperSegment = 0,
				  Int_t run_number = 0);

void LoadRootFile(TString, TChain *, 
				  Bool_t fSegInclude = false, 
				  Int_t fLowerSegment = 0, 
				  Int_t fUpperSegment = 0, 
				  Int_t run_number = 0);

void sigint_handler(int);


Int_t MakeSlugMPS(
	Int_t run_number = -1,
	TString runlets = "-1",
	TString leaflistfilename = "",
	TString slugrootfilename = "")
{
	Int_t fLowerSegment = 0;
	Int_t fUpperSegment = 0;
	Bool_t fSegInclude = false;

	TStopwatch timer;
	if (run_number == -1 ||  !leaflistfilename ||  !slugrootfilename) {
		printf("Usage:\n\t.x MakeSlug(run_number, leaflistfilename, slugrootfilename)\n\n");
		exit(1);
	}
	if(runlets.CompareTo("", TString::kExact) != 0){
		if(runlets.CompareTo("-1", TString::kExact) == 0){
			fSegInclude = false;
			std::cout << "Processing all runlets.\n" << std::endl;
		} 
		else{
			std::string option(runlets);

			Int_t index = option.find_first_of(":");
			Int_t tmp1 = index + 1;                // Using tmp# variables here is absolutely ridiculous, 
			Int_t tmp2 = option.size() - index;    // but CInt doesn't seem to want it any other way...
			fLowerSegment = atoi( option.substr(0, index).c_str() );
			fUpperSegment = atoi( option.substr(tmp1, tmp2).c_str() );
			fSegInclude = true;

			std::cout << "Runlet range specified to be:\t" 
					  << fLowerSegment << "-"
					  << fUpperSegment << std::endl;
		}
	}
	else{
		std::cerr << "Runlet argument is invalid.  You should probably fix that...." << std::endl;
		exit(1);
	}

	const Int_t debug = 1;

	TFile *slugrootfile = TFile::Open(slugrootfilename.Data(),"RECREATE");

	if (!slugrootfile) {
		printf("Error: opening file %s for output\n",slugrootfilename.Data());
		exit(1);
	}
	TTree *slug = new TTree("mps_slug", "MPS Slug tree");

	const Int_t maxleaves=1000;

	Double_t slugleafvalue[maxleaves] = {0};
	Double_t ErrorFlag;
	Double_t slugeventnumber = 0;

	TLeaf* inputleaves[maxleaves]={0};

	Int_t islugevnum =0;

	char fullleafnamelist[maxleaves][255]; 
	char branchnamelist[maxleaves][255];
	char newleafnamelist[maxleaves][255];
	char modfullleafnamelist[maxleaves][255];
	char dummy[maxleaves][255];
	char dummy2[maxleaves][255];

	TBranch *branches[maxleaves];

	FILE * leaflistFile;
	FILE * leaflistFile2;

	leaflistFile = fopen (leaflistfilename.Data(), "r");
	leaflistFile2 = fopen (leaflistfilename.Data(), "r");

	if (!leaflistFile) 
		printf("Error opening file %s\n", leaflistfilename.Data());
	else 
		printf("Opened %s\n", leaflistfilename.Data());
	
	slug->Branch("pattnum", &slugeventnumber);
	
	Int_t counter = 0;

	while ( ! feof (leaflistFile2) ) {
	fscanf (leaflistFile2, "%s %s", dummy[counter], dummy2[counter]);
		counter++;
	}
	std::cout << "Counter: " << counter << std::endl;

	for (int i = 0; i < counter; i++) {
		fscanf (leaflistFile, "%s %s", fullleafnamelist[i], newleafnamelist[i]);
		if (i + 1 < maxleaves) {
			sprintf(branchnamelist[i],"%s", fullleafnamelist[i]);
			strtok(branchnamelist[i],".: ");

			TString* tmpstr = new TString(fullleafnamelist[i]);

 			tmpstr->ReplaceAll(".","/");
			sprintf(modfullleafnamelist[i],"%s",tmpstr->Data());
			branches[i] = slug->Branch(newleafnamelist[i], &slugleafvalue[i]);
		}
		else printf("Warning: maxleaves of %i exceeded\n",maxleaves);
	}

	Int_t numinputleaves = counter-1;

	fclose (leaflistFile);
	printf("\nRead %i leaf names\n",numinputleaves);


	signal(SIGINT, sigint_handler);
	
	if (!globalEXIT) { 	   

// #
// # Chain files here
// # 
		TChain *tree = new TChain("Mps_Tree");
		
		if(fSegInclude){
			TString rootfilename = Form("%s_%d*.trees.root", qwstem.Data(), run_number);
			LoadRootFile(rootfilename, tree, true, fLowerSegment, fUpperSegment, run_number);
		}
		else{
			TString rootfilename = Form("%s_%d*.trees.root", qwstem.Data(), run_number);
			LoadRootFile(rootfilename, tree);
		}

		return -1;

		Double_t bcmforcuts = 0; 
		Double_t previousbcm = 0;
		
		tree->SetBranchStatus("*",0); 
		tree->SetBranchStatus("ErrorFlag", 1);				
		tree->SetBranchAddress("ErrorFlag",	&ErrorFlag);

		std::cout << "Reading leaves in.\n" << std::endl;

		for (Int_t leafnumber = 0; leafnumber < (numinputleaves + 1); leafnumber++) {
			tree->SetBranchStatus(Form("%s", branchnamelist[leafnumber]), 1);

			if (strcmp(branchnamelist[leafnumber],"qwk_bcm1")==0) {
			} else {
				if (debug > 3) printf("not the bcm\n") ;
			}
			if (debug>1) printf("Associated leaf %i  %s with %p\n",leafnumber-1,
								modfullleafnamelist[leafnumber], 
								inputleaves[leafnumber]);

		}

		Bool_t cutevent = 0;
		
		Int_t numcutbcm = 0; 
		Int_t numcutburp = 0;
		Int_t numcuterr = 0;
		Int_t nentries = tree->GetEntries();

		printf("There are %i entries in the tree.\n", nentries);
		std::cout << "Number of input leaves: " << numinputleaves << std::endl;

		for (Int_t jentry = 0; jentry < nentries; jentry++) {
			tree->GetEntry(jentry);
			if((jentry % 100000) == 0)
				std::cout << "Entry:\t" << jentry << std::endl;
			
			if ((Int_t)(ErrorFlag) != 0x4018080) {
				cutevent = 1;
				numcuterr++;
			}
			
 			if (!cutevent) {
				slugeventnumber++;
				for (Int_t leafnumber = 0; leafnumber < (numinputleaves + 1); leafnumber++) {
 					if( (tree->GetLeaf(modfullleafnamelist[leafnumber])) ) {
						slugleafvalue[leafnumber] = tree->GetLeaf(modfullleafnamelist[leafnumber])->GetValue();
 					} 
					else {
   						slugleafvalue[leafnumber-1] = -1e6;
 					}
				}
				slug->Fill();
			}
			previousbcm = bcmforcuts;
			cutevent = 0;
			numcutbcm=numcutburp=numcuterr=0;
		}
		std::cout << "Finished processing entries" << std::endl;
	}
	
	slugrootfile->Write(0,TObject::kOverwrite);
	timer.Print();
	
	Double_t fillTime = timer.CpuTime();
	
	printf("Time: %f s per file, %f ms per event\n", fillTime, fillTime/slugeventnumber*1000);
	islugevnum = (int) (slugeventnumber+0.1);
	
	return islugevnum;
}

void sigint_handler(int sig)
{
	std::cout << "handling signal no. " << sig << "\n";
	globalEXIT = 1;
}

Bool_t FileSearch(TString filename, TChain *chain, Bool_t fSegInclude, Int_t fLowerSegment, Int_t fUpperSegment, Int_t run_number)
{

  TString file_directory;
  Bool_t c_status = false;

  file_directory = gSystem->Getenv("QW_ROOTFILES");

  if(fSegInclude){
    for(Int_t i = fLowerSegment; i <= fUpperSegment; i++){
		filename = Form("%s_%d.*%d.trees.root", qwstem.Data(), run_number, i);
		std::cout << "Adding:: " 
				  << filename << std::endl;

		if(!(chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()))) ){
			std::cout << "Error chaining segment:\t" << filename << std::endl;
			exit(1);
		}
    }
  }
  c_status = chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()));
  std::cout << "Trying to open :: "
            << Form("%s/%s",file_directory.Data(), filename.Data())
            << std::endl;

  if(c_status){
    TString chain_name = chain->GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;

    while( (chain_element = (TChainElement*)next()) ){
		std::cout << "Adding :: "
				  << filename
				  << " to data chain"
				  << std::endl;
    }
  } 
    return c_status;

}

void LoadRootFile(TString filename, TChain *tree, Bool_t fSegInclude, Int_t fLowerSegment, Int_t fUpperSegment, Int_t run_number)
{
	Bool_t found = FileSearch(filename, tree, fSegInclude, fLowerSegment, fUpperSegment, run_number);
  
	if(!found){
		std::cerr << "Unable to locate requested file :: "
				  << filename
				  << std::endl;
		exit(1);
	}
}




/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
