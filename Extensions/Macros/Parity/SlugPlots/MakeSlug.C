#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"

#include "iostream"

Bool_t globalEXIT;

void sigint_handler(int sig)
{
	std::cout << "handling signal no. " << sig << "\n";
	globalEXIT=1;
}

Int_t MakeSlug(TString filelistfilename="0", TString leaflistfilename="0",  
			   TString slugrootfilename="0") {
	TStopwatch timer;
	if (filelistfilename=="0" ||  leaflistfilename == "0" ||  slugrootfilename == "0") {
		printf("Usage:\n\t.x MakeSlug(filelistfilename, leaflistfilename, slugrootfilename)\n\n");
		printf("Where: filelistfilename = \n");
		printf("       leaflistfilename = \n");
		printf("       slugrootfilename = \n");
		return 0;
	}
	const Int_t debug=1;

	//Read in file names
	const Int_t maxfiles=600;
	FILE * rootfilelistFile;
	rootfilelistFile = fopen (filelistfilename.Data(), "r");
	if (rootfilelistFile == NULL) printf("Error opening file %s\n",filelistfilename.Data());
	else printf("Opened %s\n",filelistfilename.Data());
	char inputfilechar[255];
	TString rootfilelist[maxfiles];
	Int_t counter=-1;
	while ( ! feof (rootfilelistFile) ) {
		fscanf (rootfilelistFile, "%s", inputfilechar);
		counter++;
		printf("%4i %s,  ",counter,inputfilechar);
		if ((counter+1)%4==0) printf("\n");
		if (counter+1 < maxfiles) rootfilelist[counter]=inputfilechar;
		else printf("Warning: maxfiles of %i exceeded\n",maxfiles);
	}
	Int_t numfiles = counter+1;
	fclose (rootfilelistFile);
	printf("\nRead %i file names\n",numfiles);	

	// created the file and tree to hold the slug data
	TFile *slugrootfile = TFile::Open(slugrootfilename.Data(),"RECREATE");
	if (slugrootfile == NULL) {
		printf("Error: opening file %s for output\n",slugrootfilename.Data());
		return -1;
	}
	TTree *slug = new TTree("slug", "Slug tree");

	//Read in leaf names
	//const Int_t baseleaves=1;
	const Int_t maxleaves=100;
	Double_t slugleafvalue[maxleaves];
	Double_t runinputleafvalue[maxleaves];
	Double_t ErrorFlag;
	Double_t slugeventnumber=0, runnumber=0, runletnumber=0;
	char fullleafnamelist[maxleaves][255], branchnamelist[maxleaves][255],
		newleafnamelist[maxleaves][255];
	TBranch *branches[maxleaves];
	FILE * leaflistFile;
	leaflistFile = fopen (leaflistfilename.Data(), "r");
	if (leaflistFile == NULL) printf("Error opening file %s\n",leaflistfilename.Data());
	else printf("Opened %s\n",leaflistfilename.Data());
	// First put the base leaves into the slug tree
	//branches[0] = 
	slug->Branch("pattnum", &slugeventnumber);
	slug->Branch("run", &runnumber);
	slug->Branch("runlet", &runletnumber);
	// Next put the list of leaves into the slug tree
	counter=-1;
	while ( ! feof (leaflistFile) ) {
		counter++;
		fscanf (leaflistFile, "%s %s", fullleafnamelist[counter], newleafnamelist[counter]);
		printf("%3i %30s  %15s,  ",counter,fullleafnamelist[counter],newleafnamelist[counter]);
		if ((counter+1)%3==0) printf("\n");
		if (counter+1 < maxleaves) {
			sprintf(branchnamelist[counter],"%s",fullleafnamelist[counter]);
			strtok(branchnamelist[counter],".: ");
			// this line creates the branch for the slug tree
			branches[counter] = slug->Branch(newleafnamelist[counter], &slugleafvalue[counter]);
			//printf("%s  %s,  ",fullleafnamelist[counter],branchnamelist[counter]);
		}
		else printf("Warning: maxleaves of %i exceeded\n",maxleaves);
	}
	Int_t numinputleaves = counter+1;
	fclose (leaflistFile);
	printf("\nRead %i leaf names\n",numinputleaves);
// 	for (Int_t leafnumber=1; leafnumber<=numinputleaves; leafnumber++) {
// 		printf("%s  %s\n",fullleafnamelist[leafnumber-1],branchnamelist[leafnumber-1]);
// 	}


	char rootfilename[255], *tokenpointer;
	for (Int_t filenumber=1; filenumber<=numfiles; filenumber++) {
		signal(SIGINT, sigint_handler);
		if (!globalEXIT) { 		// Allow ctrl-C escape 
			TFile *file = 0;
			sprintf(rootfilename,"%s",rootfilelist[filenumber-1].Data());
			file = TFile::Open(rootfilename);
			if (file==0) {
				printf("Warning: cannot open %s ... skipping.\n",rootfilename);
			} else {
				printf("Opened %3i: %s\n",filenumber,rootfilename);
				// tokenise the file name
				tokenpointer = strtok(rootfilename,"_.");
				// the second token is the run number
				tokenpointer = strtok (NULL, " ,.-");
				runnumber = atof(tokenpointer);
				// the third token is the runlet number			
				tokenpointer = strtok (NULL, " ,.-");
				runletnumber = atof(tokenpointer);
				if (debug>0) printf("run %.0f, runlet %03.0f\n",runnumber,runletnumber);
				TTree *tree = (TTree*)gROOT->FindObject("Hel_Tree");
				Double_t bcmforcuts=0, previousbcm=0;
				Int_t bcmarrpt=0;
				// **** Only enable the important branches
				tree->SetBranchStatus("*",0); // disable all branches
				tree->SetBranchStatus("ErrorFlag",1);				
				tree->SetBranchAddress("ErrorFlag",	&ErrorFlag);
				for (Int_t leafnumber=1; leafnumber<=numinputleaves; leafnumber++) {
					tree->SetBranchStatus(Form("%s*",branchnamelist[leafnumber-1]),1);				
					tree->SetBranchAddress(branchnamelist[leafnumber-1], 
										   &runinputleafvalue[leafnumber-1]);
					if (strcmp(branchnamelist[leafnumber-1],"yield_qwk_bcm1")==0) {
						bcmarrpt = leafnumber-1;
						printf("Found a bcm to use for the cuts: %s at array point %i\n",
							   branchnamelist[leafnumber-1],bcmarrpt);
					} else {
						if (debug>1) printf("not the bcm\n") ;
					}
					if (debug>1) printf("Associated %s with %p\n", 
										branchnamelist[leafnumber-1], 
										&runinputleafvalue[leafnumber-1]);
				}
				Bool_t cutevent=0;
				Int_t numcutbcm=0, numcutburp=0, numcuterr=0;
				//Step through the tree
				Int_t nentries = tree->GetEntries();
				printf("There are %i entries in the tree.\n",nentries);
				for (Int_t jentry=0; jentry<nentries; jentry++) {
					tree->GetEntry(jentry);
					// calculate cuts
// 					bcmforcuts = runinputleafvalue[bcmarrpt];
// 					if (bcmforcuts<40) {
// 						cutevent=1;
// 						if (debug>1) printf("bcm %.1f  ",bcmforcuts);
// 						numcutbcm++;
// 					}
// 					if (fabs(previousbcm-bcmforcuts)>1.0) {
// 						cutevent=1;
// 						if (debug>1) printf("dbcm %.1f %.1f  ",previousbcm,bcmforcuts);
// 						numcutburp++;
// 					}
// 					if (ErrorFlag!=0) {
// 						cutevent=1;
//  						if (debug>1) printf("e%.1f ",ErrorFlag);
//  						numcuterr++;
// 					}
					if (!cutevent) { // apply cuts
						slugeventnumber++;
						for (Int_t leafnumber=1; leafnumber<=numinputleaves; leafnumber++) {
							slugleafvalue[leafnumber-1]=runinputleafvalue[leafnumber-1];
							if (debug > 1 && jentry<10) 
								printf("%i %10f  \t",leafnumber-1,
									   slugleafvalue[leafnumber-1]);//,runinputleafvalue[leafnumber-1]);
						}
						if (debug > 1 && jentry<10) printf("\n");
						slug->Fill();
					}
					previousbcm = bcmforcuts;
					cutevent = 0;
// 					printf("cut %i below current events and/or %i burp events\n",numcutbcm,numcutburp);
// 					printf("cut %i from ErrorFlag\n",numcuterr);
					numcutbcm=numcutburp=numcuterr=0;
				}
			}
		}
	}
	slugrootfile->Write(0,TObject::kOverwrite);
	timer.Print();
	Double_t fillTime = timer.CpuTime();
	printf("Time: %f s per file, %f ms per event\n",fillTime/numfiles,fillTime/slugeventnumber*1000);
	return slugeventnumber;
}






/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
