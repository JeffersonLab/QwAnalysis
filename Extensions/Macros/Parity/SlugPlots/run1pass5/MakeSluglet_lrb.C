#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TRegexp.h"
#include "TPRegexp.h"
#include <iostream>

Bool_t globalEXIT;

void sigint_handler(int sig)
{
	std::cout << "handling signal no. " << sig << "\n";
	globalEXIT=1;
}

struct runletSort{
	bool operator()(const std::pair<Int_t,Int_t> &left, const std::pair<Int_t,Int_t> &right) {
        return left.second < right.second;
    }

};



Int_t MakeSluglet_lrb(Int_t runnum=0,TString leaflistfilename="0", TString slugrootfilename="0",TString qwrootfiles="0", TString qwstem="",TString suffix="0")
{
	TStopwatch timer;
	if (runnum==0 ||  leaflistfilename == "0" ||  slugrootfilename == "0" || qwstem== "") {
		printf("Usage:\n\t.x MakeSlug(runnum, leaflistfilename, slugrootfilename, qwstem)\n\n");
		printf("Where: runnum = \n");
		printf("       leaflistfilename = \n");
		printf("       slugrootfilename = \n");
		printf("       qwrootfiles = \n");
		printf("       qwstem = \n");
		printf("       suffix = \n");
		return 0;
	}
	const Int_t debug=1;
	const Int_t debug2=1;
	
//        const TString qwrootfiles = TString(gSystem->Getenv("QW_ROOTFILES"));

////////////////////////////////////////////////////////////////////////////////////////////////
	if(debug2==1){
	std::cout<<"Fetching list of files from "<<qwrootfiles<<std::endl;
	}

	std::vector<std::pair<Int_t,Int_t> > runletlist; //store pairs of (run, runlet)

	TString filename;
	TString filenameformat;
	if(suffix!="0"){
		std::cout<<"Found suffix"<<std::endl;
		filenameformat = Form("%s_%i.[0-9][0-9][0-9].%s.root",qwstem.Data(),runnum,suffix.Data()); //to only pick out right rootfile types
	}
	else{
		filenameformat = Form("%s_%i.[0-9][0-9][0-9].root",qwstem.Data(),runnum); //to only pick out right rootfile types
	}
	TString runletformat = "\\.([0-9]{3})\\."; //regex string to match ONLY the runlet number
	Int_t counter=0;
	void *dir = gSystem->OpenDirectory(qwrootfiles);

	if(dir){
	  std::cout<<"Opened up directory to look for filenames"<<std::endl;
	  TPRegexp re(filenameformat);
	  TPRegexp seg(runletformat);

	  while((filename = gSystem->GetDirEntry(dir))!=""){
		if(!strcmp(filename,".") || !strcmp(filename,"..") || !strcmp(filename,""))continue; //skip directories .  and .. just in case
		if(debug2==1){std::cout<<"Checking file '"<<filename<<"'"<<std::endl;}
		if(filename.Index(re) != kNPOS){ //check if filename is appropriate
			if(debug2==1){std::cout<<"Found a match"<<std::endl;}
				TObjArray *tmp = seg.MatchS(filename); //returns an object array containing the matches. 0th entry is the full character array match, 1 is just the match, etc.
			const TString subStr = ((TObjString *)tmp->At(1))->GetString(); //grab the runlet number
			runletlist.push_back(pair<Int_t,Int_t>(runnum,atoi(subStr)));
			counter++;
        	}
	  }
	}
	Int_t numfiles = counter;
	
	std::sort(runletlist.begin(), runletlist.end(), runletSort());
	
	if(debug2==1){
		std::cout<<"Finished getting file list"<<std::endl;
	}





/////////////////////////////////////////////////////////////////////////////////////////////////

	// created the file and tree to hold the slug data
	TFile *slugrootfile = TFile::Open(slugrootfilename.Data(),"RECREATE");
	if (slugrootfile == NULL) {
		printf("Error: opening file %s for output\n",slugrootfilename.Data());
		return -1;
	}
	TTree *slug = new TTree("slug", "Slug tree");

	//Read in leaf names
	const Int_t maxleaves=1500;
	Double_t slugleafvalue[maxleaves]={0};
	TLeaf* inputleaves[maxleaves]={0};
	Double_t ErrorFlag=-1e6;
	Int_t islugevnum =0;
	Double_t slugeventnumber=0, runnumber=0, runletnumber=0;
	char fullleafnamelist[maxleaves][255], branchnamelist[maxleaves][255];
	char newleafnamelist[maxleaves][255],modfullleafnamelist[maxleaves][255];
	char dummy[maxleaves][255], dummy2[maxleaves][255];



	TBranch *branches[maxleaves];
	FILE * leaflistFile;
	FILE * leaflistFile2;
	leaflistFile = fopen (leaflistfilename.Data(), "r");
	leaflistFile2 = fopen (leaflistfilename.Data(), "r");
	if (leaflistFile == NULL) printf("Error opening file %s\n",leaflistfilename.Data());
	else printf("Opened %s\n",leaflistfilename.Data());
	// First put the base leaves into the slug tree
	slug->Branch("pattnum", &slugeventnumber);
	slug->Branch("run", &runnumber);
	slug->Branch("runlet", &runletnumber);
	// Next put the list of leaves into the slug tree
	counter=0;
	while ( ! feof (leaflistFile2) ) {
		fscanf (leaflistFile2, "%s %s", dummy[counter], dummy2[counter]);
		counter++;
	}
	for (int i=0;i<counter-1;i++) {
		fscanf (leaflistFile, "%s %s", fullleafnamelist[i], newleafnamelist[i]);
		printf("%3i %30s  %15s \n",i,fullleafnamelist[i],newleafnamelist[i]);
		if (i+1 < maxleaves) {
			sprintf(branchnamelist[i],"%s",fullleafnamelist[i]);
			strtok(branchnamelist[i],".: ");
			TString* tmpstr = new TString(fullleafnamelist[i]);
			tmpstr->ReplaceAll(".","/");
			sprintf(modfullleafnamelist[i],"%s",tmpstr->Data());
                        branches[i] = slug->Branch(newleafnamelist[i], &slugleafvalue[i]);
			if (debug>1) {
				cout << Form("%s   %s",fullleafnamelist[i],branchnamelist[i]);
			}
		}
		else printf("Warning: maxleaves of %i exceeded\n",maxleaves);
	}
	Int_t numinputleaves = counter-1;
	fclose (leaflistFile);
	printf("\nRead %i leaf names\n",numinputleaves);
	for (Int_t filenumber=1; filenumber<=numfiles; filenumber++) {
		signal(SIGINT, sigint_handler);
		if (!globalEXIT) { 		// Allow ctrl-C escape 
			TFile *file = 0;
			pair<Int_t,Int_t> runlet = runletlist[filenumber-1];
			runnumber = runlet.first;
			runletnumber = runlet.second;
			TString rootfilename;
			if(suffix=="0"){
			rootfilename = qwrootfiles + "/" + qwstem + Form("_%i.%03i.root",runlet.first,runlet.second);
			}
			else{
			rootfilename = qwrootfiles + "/" + qwstem + Form("_%i.%03i.%s.root",runlet.first,runlet.second,suffix.Data());
			}
			file = TFile::Open(rootfilename);
			if (file==0) {
				printf("Warning: cannot open %s ... skipping.\n",rootfilename.Data());
			} else {
				printf("Opened %3i: %s\n",filenumber,rootfilename.Data());

				if (debug>0) printf("run %.0f, runlet %03.0f\n",runnumber,runletnumber);
				TTree *tree = (TTree*)gROOT->FindObject("reg");
				Double_t bcmforcuts=0, previousbcm=0;
				Int_t bcmarrpt=0;
				// **** Only enable the important branches
				tree->SetBranchStatus("*",0); // disable all branches
				for (Int_t leafnumber=1; leafnumber<numinputleaves; leafnumber++) {
					tree->SetBranchStatus(Form("%s",branchnamelist[leafnumber-1]),1);
					inputleaves[leafnumber-1] = (TLeaf*) tree->GetLeaf(modfullleafnamelist[leafnumber-1]);
					if (debug>1) printf("Associated leaf %i  %s with %p\n",leafnumber-1,
										modfullleafnamelist[leafnumber-1], 
										inputleaves[leafnumber-1]);
				}

				Bool_t cutevent=0;
				Int_t numcutbcm=0, numcutburp=0, numcuterr=0;
				//Step through the tree
				Int_t nentries = tree->GetEntries();
				printf("There are %i entries in the tree.\n",nentries);
				for (Int_t jentry=0; jentry<nentries; jentry++) {
					tree->GetEntry(jentry);
					// calculate cuts
					if (!cutevent) { // apply cuts
						slugeventnumber++;
						for (Int_t leafnumber=1; leafnumber<=numinputleaves; leafnumber++) {
							if (inputleaves[leafnumber-1]) {
								slugleafvalue[leafnumber-1]=inputleaves[leafnumber-1]->GetValue();
							} else {
								slugleafvalue[leafnumber-1] = -1e6;
							}
							if (debug > 1 && jentry<10) 
								printf("%i %10f  \t",leafnumber-1,
									   slugleafvalue[leafnumber-1]);//,runinputleafvalue[leafnumber-1]);
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
	printf("Time: %f s per file, %f ms per event\n",fillTime/numfiles,fillTime/slugeventnumber*1000);
	islugevnum = (int) (slugeventnumber+0.1);
	return islugevnum;

}

