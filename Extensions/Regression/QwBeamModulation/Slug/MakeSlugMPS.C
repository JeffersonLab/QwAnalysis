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

// const TString qwstem = "QwPass5";

Bool_t FileSearch(TString, TChain *, 
				  Bool_t fSegInclude = false, 
				  Int_t fLowerSegment = 0, 
				  Int_t fUpperSegment = 0,
				  Int_t run_number = 0,
				  TString filestem = 0);

void LoadRootFile(TString, TChain *, 
				  Bool_t fSegInclude = false, 
				  Int_t fLowerSegment = 0, 
				  Int_t fUpperSegment = 0, 
				  Int_t run_number = 0,
				  TString qwstem = 0);

void sigint_handler(int);


Int_t MakeSlugMPS(
	Int_t run_number = -1,
	TString runlets = "-1",
	TString leaflistfilename = "",
	TString slugrootfilename = "",
	TString qwstem = "")
{
	Int_t fLowerSegment = 0;
	Int_t fUpperSegment = 0;
	Bool_t fSegInclude = false;

	TStopwatch timer;
	std::cout << "Input option: " << run_number << " "
			  << runlets << " " 
			  << leaflistfilename << " "
			  << slugrootfilename << std::endl;

	if (run_number == -1 ||  !leaflistfilename ||  !slugrootfilename || !qwstem) {
		printf("Usage:\n\t.x MakeSlug(run_number, leaflistfilename, slugrootfilename)\n\n");
		exit(1);
	}


	std::cout << "Analysis pass stem set to: " << qwstem << std::endl;
	// if(runlets.CompareTo("", TString::kExact) != 0){
	if(runlets.CompareTo("-1", TString::kExact) == 0){
		fSegInclude = false;
		runlets="full";
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
	// }
	}
	// else{
	// 	std::cerr << "Runlet argument is invalid.  You should probably fix that...." << std::endl;
	// 	exit(1);
	// }

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

	if (!leaflistFile) {
 		printf("Error opening file %s\n", leaflistfilename.Data());
		exit(1);
	}
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
			LoadRootFile(rootfilename, tree, true, fLowerSegment, fUpperSegment, run_number, qwstem);
		}
		else{
			TString rootfilename = Form("%s_%d*.trees.root", qwstem.Data(), run_number);
			std::cout << "Loading full rootfile" << std::endl;
			LoadRootFile(rootfilename, tree);
		}


		Double_t bcmforcuts = 0; 
		Double_t previousbcm = 0;
		
		tree->SetBranchStatus("*",0); 
		tree->SetBranchStatus("ErrorFlag", 1);				
		tree->SetBranchAddress("ErrorFlag",	&ErrorFlag);

		std::cout << "Reading leaves in.\n" << std::endl;

		for (Int_t leafnumber = 0; leafnumber < (numinputleaves + 1); leafnumber++) {
//
// Not sure why the below line gave a TRegExp error.  This line worked previously and has only recently given errors
// By enabling all leaves in tree though I seem to have patched it fo rthe moment - though someone should figure
// out WHY it stopped working. 
//
			// tree->SetBranchStatus(Form("%s", branchnamelist[leafnumber]), 1);
			tree->SetBranchStatus("*", 1);

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
   		 				// slugleafvalue[leafnumber-1] = -1e6;
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

	if(slugeventnumber <= 0){
		std::cerr << "Rootfile contains no good events.  Exiting." << std::endl;
		exit(1);
	}else{
		slugrootfile->Write(0,TObject::kOverwrite);
	}
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

Bool_t FileSearch(TString filename, TChain *chain, Bool_t fSegInclude, Int_t fLowerSegment, Int_t fUpperSegment, Int_t run_number, TString qwstem)
{

  TString file_directory;
  Bool_t c_status = false;

  file_directory = gSystem->Getenv("QW_ROOTFILES");

  if(fSegInclude){
	  c_status = true;
	  for(Int_t i = fLowerSegment; i <= fUpperSegment; i++){
		  filename = Form("%s_%d.%03d.trees.root", qwstem.Data(), run_number, i);
 		  std::cout << "Adding:: " 
 					<< filename << std::endl;
		  
		  if(!(chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()))) ){
			  std::cout << "Error chaining segment:\t" << filename << std::endl;
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

    while( (chain_element = (TChainElement*)next()) ){
// 		std::cout << "Adding :: "
// 				  << filename
// 				  << " to data chain"
// 				  << std::endl;
    }
  } 
    return c_status;

}

void LoadRootFile(TString filename, TChain *tree, Bool_t fSegInclude, Int_t fLowerSegment, Int_t fUpperSegment, Int_t run_number, TString qwstem)
{
	Bool_t found = FileSearch(filename, tree, fSegInclude, fLowerSegment, fUpperSegment, run_number, qwstem);
  
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
