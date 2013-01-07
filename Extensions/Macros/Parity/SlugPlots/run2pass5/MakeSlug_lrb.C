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
	globalEXIT=1;
}

Int_t MakeSlug_lrb(
	TString runletlistfilename="0",
	TString leaflistfilename="0",
	TString slugrootfilename="0",
  const char* set="0")
{
	TStopwatch timer;
	if (runletlistfilename=="0" ||  leaflistfilename == "0" ||  slugrootfilename == "0") {
		printf("Usage:\n\t.x MakeSlug(runletlistfilename, leaflistfilename, slugrootfilename)\n\n");
		printf("Where: runletlistfilename = \n");
		printf("       leaflistfilename = \n");
		printf("       slugrootfilename = \n");
		return 0;
	}
	const Int_t debug=1;

	//Read in file names
	const Int_t maxfiles=600;
	FILE * runletlistFile;
	runletlistFile = fopen (runletlistfilename.Data(), "r");
	if (runletlistFile == NULL) printf("Error opening file %s\n",runletlistfilename.Data());
	else printf("Opened %s\n",runletlistfilename.Data());
	pair<Int_t,Int_t> runletlist[maxfiles];
	Int_t counter=-1;
	while ( ! feof (runletlistFile) ) {
		Int_t run = 0, segment = 0;
		if (fscanf(runletlistFile,"%d %d",&run,&segment) != 2) {
			char tmp[255];
			fscanf(runletlistFile,"%s",tmp);
			continue;
		}
		counter++;
		printf("%4i %d.%03d,  ",counter,run,segment);
		if ((counter+1)%4==0) printf("\n");
		if (counter+1 < maxfiles) runletlist[counter] = pair<Int_t,Int_t>(run,segment);
		else {
			printf("Warning: maxfiles of %i exceeded\n",maxfiles);
			break;
		}
	}
	Int_t numfiles = counter+1;
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
	const Int_t maxleaves=500;
	Double_t slugleafvalue[maxleaves]={0};
//	Double_t runinputleafvalue[maxleaves];
	TLeaf* inputleaves[maxleaves]={0};
//	Double_t ErrorFlag=-1e6;
//	Int_t ErrorFlag =0;
	Int_t islugevnum =0;
	Double_t slugeventnumber=0, runnumber=0, runletnumber=0;
//	char fullleafnamelist[maxleaves][255], branchnamelist[maxleaves][255],
//		newleafnamelist[maxleaves][255],modfullleafnamelist[maxleaves][255];
	char fullleafnamelist[maxleaves][255], branchnamelist[maxleaves][255],
                newleafnamelist[maxleaves][255],modfullleafnamelist[maxleaves][255],
                dummy[maxleaves][255],dummy2[maxleaves][255];



	TBranch *branches[maxleaves];
        FILE * leaflistFile;
        FILE * leaflistFile2;
        leaflistFile = fopen (leaflistfilename.Data(), "r");
        leaflistFile2 = fopen (leaflistfilename.Data(), "r");
	if (leaflistFile == NULL) printf("Error opening file %s\n",leaflistfilename.Data());
	else printf("Opened %s\n",leaflistfilename.Data());
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
		}
		else printf("Warning: maxleaves of %i exceeded\n",maxleaves);
	}
	Int_t numinputleaves = counter-1;
	fclose (leaflistFile);
	printf("\nRead %i leaf names\n",numinputleaves);
// 	for (Int_t leafnumber=1; leafnumber<=numinputleaves; leafnumber++) {
// 		printf("%s  %s\n",fullleafnamelist[leafnumber-1],branchnamelist[leafnumber-1]);
// 	}
//	const TString qwstem = "QwPass1";
//	const TString qwstem = "Qweak";
//	const TString qwstem = "reg_Qweak";
//      const TString qwstem = "reg_HallA_charge_d_0_Qweak";
//	const TString qwstem = "reg_HallA_charge_2sig_Qweak";
//	const TString qwstem = "reg_HallA_nocharge_Qweak";

  const TString qwstem = "reg_QwPass5"; //for pass5 - rakitha (rakithab@jalb.org) 08-07-2012

//  const TString qwrootfiles = "/home/leacock/linRegBlue/inel_rootfiles";
//	const TString qwrootfiles = "/home/leacock/scratch/rootfiles";
//	const TString qwrootfiles = "/data/disk1/leacock/linRegBlue/lrb_rootfiles";
//	const TString qwrootfiles = TString(gSystem->Getenv("QW_ROOTFILES"));
//  const TString qwrootfiles = Form("/data/disk1/leacock/slugplots/25percent/lrb_links/%s",set);
//  const TString qwrootfiles = Form("/volatile/hallc/qweak/leacock/25percent/lrb_links/%s",set);

//const TString qwrootfiles = "/work/hallc/qweak/QwAnalysis/run2/pass5regression/lrb_rootfiles"; //for pass5 - rakitha (rakithab@jalb.org) 08-07-2012
const TString qwrootfiles = "/cache/mss/hallc/qweak/rootfiles/pass5/lrb_rootfiles/"; //for pass5 redo of slug34 - smacewan 09-25-12

	for (Int_t filenumber=1; filenumber<=numfiles; filenumber++) {
		signal(SIGINT, sigint_handler);
		if (!globalEXIT) { 		// Allow ctrl-C escape 
			TFile *file = 0;
			pair<Int_t,Int_t> runlet = runletlist[filenumber-1];
			runnumber = runlet.first;
			runletnumber = runlet.second;
//			TString rootfilename = qwrootfiles + "/" + qwstem + Form("_%i.%03i.root",runlet.first,runlet.second);
			TString rootfilename = qwrootfiles + "/" + Form("%s_%s_%i.%03i.root",set,qwstem.Data(),runlet.first,runlet.second);
			file = TFile::Open(rootfilename);
			if (file==0) {
				printf("Warning: cannot open %s ... skipping.\n",rootfilename.Data());
			} else {
				printf("Opened %3i: %s\n",filenumber,rootfilename.Data());

				if (debug>0) printf("run %.0f, runlet %03.0f\n",runnumber,runletnumber);
				//TTree *tree = (TTree*)gROOT->FindObject("Hel_Tree");
				TTree *tree = (TTree*)gROOT->FindObject("reg");
				Double_t bcmforcuts=0, previousbcm=0;
				//Int_t bcmarrpt=0;
				// **** Only enable the important branches
				tree->SetBranchStatus("*",0); // disable all branches
//				tree->SetBranchStatus("ok_cut",1);
//				tree->SetBranchAddress("ok_cut",	&ErrorFlag);
				for (Int_t leafnumber=1; leafnumber<numinputleaves; leafnumber++) {
//					tree->SetBranchStatus(Form("%s*",branchnamelist[leafnumber-1]),1);				
//					tree->SetBranchStatus(Form("%s",branchnamelist[leafnumber-1]),1);
					tree->SetBranchStatus(Form("%s",fullleafnamelist[leafnumber-1]),1);
					//inputleaves[leafnumber-1] = (TLeaf*) tree->GetLeaf(modfullleafnamelist[leafnumber-1]);
                                        inputleaves[leafnumber-1] = (TLeaf*) tree->GetLeaf(fullleafnamelist[leafnumber-1]);
// 					tree->SetBranchAddress(branchnamelist[leafnumber-1], 
// 										   &runinputleafvalue[leafnumber-1]);
					if (strcmp(branchnamelist[leafnumber-1],"yield_qwk_bcm1")==0) {
//						bcmarrpt = leafnumber-1;
// 						printf("Found a bcm to use for the cuts: %s at array point %i\n",
// 							   branchnamelist[leafnumber-1],bcmarrpt);
//						printf("Found a bcm to use for the cuts: %s at array point %i\n",
//							   branchnamelist[leafnumber-1],bcmarrpt);
					} else {
						if (debug>3) printf("not the bcm\n") ;
					}
// 					if (debug>1) printf("Associated leaf %i  %s with %p\n",leafnumber-1,
// 										branchnamelist[leafnumber-1], 
// 										&runinputleafvalue[leafnumber-1]);
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
// 					if (int(ErrorFlag)!=0) {
// 						cutevent=1;
// 						if (debug>1) printf("e%.1f ",ErrorFlag);
//  						numcuterr++;
// 					}
					if (!cutevent) { // apply cuts
						slugeventnumber++;
						for (Int_t leafnumber=1; leafnumber<=numinputleaves; leafnumber++) {
//							slugleafvalue[leafnumber-1]=runinputleafvalue[leafnumber-1];
							if (inputleaves[leafnumber-1]) {
								slugleafvalue[leafnumber-1]=inputleaves[leafnumber-1]->GetValue();
                                                                //if (slugeventnumber==1000) cout<<fullleafnamelist[leafnumber-1]<<"   "<<inputleaves[leafnumber-1]->GetValue()*1e6<<endl;
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
	islugevnum = (int) (slugeventnumber+0.1);
//	return slugeventnumber;
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
