#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"

#include <iostream>

Bool_t globalEXIT;

void sigint_handler(int sig)
{
	std::cout << "handling signal no. " << sig << "\n";
	globalEXIT = 1;
}

Int_t MakeSlug(
	TString runletlistfilename = "0",
	TString leaflistfilename = "0",
	TString slugrootfilename = "0")
{
	TStopwatch timer;
	if (runletlistfilename == "0" ||  leaflistfilename == "0" ||  slugrootfilename == "0") {
		printf("Usage:\n\t.x MakeSlug(runletlistfilename, leaflistfilename, slugrootfilename)\n\n");
		printf("Where: runletlistfilename = \n");
		printf("       leaflistfilename = \n");
		printf("       slugrootfilename = \n");
		return 0;
	}
	const Int_t debug = 1;

	const TString qwstem = "QwPass5";
// 	const TString qwrootfiles = "/scratch/jhoskins/scratch/rootfiles";
	const TString qwrootfiles = "/volatile/hallc/qweak/QwAnalysis/run1/rootfiles";

	//Read in file names
	const Int_t maxfiles = 600;
	FILE * runletlistFile;

	runletlistFile = fopen (runletlistfilename.Data(), "r");

	if (runletlistFile == NULL) 
		printf("Error opening file %s\n",runletlistfilename.Data());
	else 
		printf("Opened %s\n",runletlistfilename.Data());

	pair<Int_t,Int_t> runletlist[maxfiles];

	Int_t counter = -1;

	while ( ! feof (runletlistFile) ) {
		Int_t run = 0;
		Int_t segment = 0;

		if (fscanf(runletlistFile,"%d %d",&run, &segment) != 2) {
			char tmp[255];
			fscanf(runletlistFile,"%s", tmp);
			continue;
		}
		counter++;
		printf("%4i %d.%03d,  ",counter,run,segment);
		if ((counter + 1) % 4 == 0) printf("\n");
		if ((counter + 1) < maxfiles) runletlist[counter] = pair<Int_t,Int_t>(run,segment);
		else {
			printf("Warning: maxfiles of %i exceeded\n",maxfiles);
			break;
		}
	}
	Int_t numfiles = counter + 1;

	fclose (runletlistFile);
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

	const Int_t maxleaves=1000;

	Double_t slugleafvalue[maxleaves] = {0};
	Double_t ErrorFlag;
	Double_t slugeventnumber = 0;
	Double_t runnumber = 0; 
	Double_t runletnumber = 0;

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

	if (leaflistFile == NULL) 
		printf("Error opening file %s\n", leaflistfilename.Data());
	else 
		printf("Opened %s\n", leaflistfilename.Data());

	// First put the base leaves into the slug tree
	//branches[0] = 

	slug->Branch("pattnum", &slugeventnumber);
	slug->Branch("run", &runnumber);
	slug->Branch("runlet", &runletnumber);

	// Next put the list of leaves into the slug tree
	counter=0;

	while ( ! feof (leaflistFile2) ) {
	fscanf (leaflistFile2, "%s %s", dummy[counter], dummy2[counter]);
		counter++;
	}

	for (int i = 0; i < counter - 1; i++) {
		fscanf (leaflistFile, "%s %s", fullleafnamelist[i], newleafnamelist[i]);
		printf("%3i %30s  %15s \n",i,fullleafnamelist[i], newleafnamelist[i]);
		if (i + 1 < maxleaves) {
			sprintf(branchnamelist[i],"%s",fullleafnamelist[i]);
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

	for (Int_t filenumber = 1; filenumber <= numfiles; filenumber++) {
		signal(SIGINT, sigint_handler);

		if (!globalEXIT) { 	   

			TFile *file = 0;
			pair<Int_t,Int_t> runlet = runletlist[filenumber - 1];

			runnumber = runlet.first;
			runletnumber = runlet.second;

			TString rootfilename = qwrootfiles + "/" + qwstem + Form("_%i.%03i.trees.root",runlet.first,runlet.second);

			file = TFile::Open(rootfilename);
			if (file==0) {
				printf("Warning: cannot open %s ... skipping.\n",rootfilename.Data());
			} else {
				printf("Opened %3i: %s\n",filenumber,rootfilename.Data());

				if (debug > 0) printf("run %.0f, runlet %03.0f\n",runnumber,runletnumber);

				TTree *tree = (TTree*)gROOT->FindObject("Mps_Tree");

				Double_t bcmforcuts = 0; 
				Double_t previousbcm = 0;

				Int_t bcmarrpt = 0;

				tree->SetBranchStatus("*",0); 
				tree->SetBranchStatus("ErrorFlag",1);				
				tree->SetBranchAddress("ErrorFlag",	&ErrorFlag);

				for (Int_t leafnumber = 1; leafnumber < numinputleaves; leafnumber++) {
					tree->SetBranchStatus(Form("%s", branchnamelist[leafnumber - 1]),1);
					inputleaves[leafnumber - 1] = (TLeaf*) tree->GetLeaf(modfullleafnamelist[leafnumber-1]);
					if (strcmp(branchnamelist[leafnumber-1],"qwk_bcm1")==0) {
					} else {
						if (debug > 3) printf("not the bcm\n") ;
					}
					if (debug>1) printf("Associated leaf %i  %s with %p\n",leafnumber-1,
										modfullleafnamelist[leafnumber-1], 
										inputleaves[leafnumber-1]);
				}

				Bool_t cutevent=0;

				Int_t numcutbcm = 0; 
				Int_t numcutburp = 0;
				Int_t numcuterr = 0;
				Int_t nentries = tree->GetEntries();

				printf("There are %i entries in the tree.\n",nentries);

				for (Int_t jentry=0; jentry<nentries; jentry++) {
					tree->GetEntry(jentry);

 					if ((Int_t)(ErrorFlag) != 0x4018080) {
 						cutevent = 1;
 						if (debug > 1) printf("e%.1f ",ErrorFlag);
  						numcuterr++;
 					}

					if (!cutevent) { 
						slugeventnumber++;
						for (Int_t leafnumber = 1; leafnumber <= numinputleaves; leafnumber++) {
							if (inputleaves[leafnumber-1]) {
								slugleafvalue[leafnumber - 1] = inputleaves[leafnumber - 1]->GetValue();
							} else {
								slugleafvalue[leafnumber-1] = -1e6;
							}
							if (debug > 1 && jentry<10) 
								printf("%i %10f  \t",leafnumber-1,
									   slugleafvalue[leafnumber-1]);
						}
						if (debug > 1 && jentry<10) printf("\n");
						slug->Fill();
					}
					previousbcm = bcmforcuts;
					cutevent = 0;
					numcutbcm=numcutburp=numcuterr=0;
				}
			}
		}
	}
	slugrootfile->Write(0,TObject::kOverwrite);
	timer.Print();

	Double_t fillTime = timer.CpuTime();

	printf("Time: %f s per file, %f ms per event\n", fillTime/numfiles, fillTime/slugeventnumber*1000);
	islugevnum = (int) (slugeventnumber+0.1);

	return islugevnum;
}






/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
