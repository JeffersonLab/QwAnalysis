#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TROOT.h"
#include "iostream"

Bool_t globalEXIT;

void sigint_handler(int sig)
{
	std::cout << "handling signal no. " << sig << "\n";
	globalEXIT=1;
}

Int_t MakeSlugPlots(TString plotlistfilename="0", TString slugrootfilename="0", TString plotdirstr="0") {
	TStopwatch timer;
	if (plotlistfilename=="0" ||  slugrootfilename == "0" || plotdirstr=="0") {
		printf("Usage:\n\t.x MakeSlug(plotlistfilename, slugrootfilename, plotdir)\n\n");
		printf("Where: plotlistfilename = \n");
		printf("       slugrootfilename = \n");
		return 0;
	}
	const Int_t debug=1;

	system(Form("mkdir %s",plotdirstr.Data()));

	// open the slug file and tree
	TFile *slugrootfile = TFile::Open(slugrootfilename.Data(),"read");
	if (slugrootfile == NULL) {
		printf("Error: opening file %s for output\n",slugrootfilename.Data());
		return -1;
	}
	TTree *slug = (TTree*)gROOT->FindObject("slug");

	//Read in plots
	const Int_t maxplots=100;
	char fullplotcmdlist[maxplots][255], cutcmdlist[maxplots][255];

	Int_t failcount=0;
	FILE * plotlistFile;
	plotlistFile = fopen (plotlistfilename.Data(), "r");
	if (plotlistFile == NULL) printf("Error opening file %s\n",plotlistfilename.Data());
	else printf("Opened %s\n",plotlistfilename.Data());
	// Create the list of plots
	Int_t counter=-1;
	while ( ! feof (plotlistFile) ) {
		counter++;
		if (counter+1 < maxplots) {
			fscanf (plotlistFile, "%s %s", fullplotcmdlist[counter], cutcmdlist[counter]);
			printf("%3i %30s  %20s,  ",counter,fullplotcmdlist[counter],cutcmdlist[counter]);
			if ((counter+1)%3==0) printf("\n");
		}
		else {
			printf("Warning: maxplots of %i exceeded\n",maxplots);
			failcount++;
			if (failcount>100) {
				return -1;
			}
		}
	}
	Int_t numplots = counter+1;
	fclose (plotlistFile);
	printf("\nRead %i plot names\n",numplots);

	gROOT->SetStyle("Plain");
	TCanvas *canvas = new TCanvas("canvas","slug canvas",800,600);

	for (Int_t plotnumber=1; plotnumber<=numplots; plotnumber++) {
		signal(SIGINT, sigint_handler);
		if (!globalEXIT) { 		// Allow ctrl-C escape 
			if (debug>0) printf("%s  %s\n",fullplotcmdlist[plotnumber-1],cutcmdlist[plotnumber-1]);
			slug->Draw(fullplotcmdlist[plotnumber-1],cutcmdlist[plotnumber-1]);
			canvas->Print(Form("%s/slugplot_%03i.png",plotdirstr.Data(),plotnumber));
		}
	}

	timer.Print();
	Double_t fillTime = timer.CpuTime();
	printf("Time: %f s per plot\n",fillTime/numplots);
	return numplots;
}






/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
