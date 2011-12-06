#include <sys/stat.h> 
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cmath>

#include "TCut.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TIterator.h"
#include "TCollection.h"
#include "TChainElement.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TDirectory.h"
#include "THStack.h"
#include "TF1.h"
#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"

#include "QwSIS3320_Samples.h"

#include "getChain.C"

void stepthru_mps(Int_t runnumber = 0, Int_t maxevents = 0)
{
	if (runnumber==0) {
		printf("\n\nUseage:\n\t .x stepthru_mps.C(runnumber, [maxevents])\n");
		printf("\t .x stepthru_mps_DEVEL.C(runnumber, [maxevents])\n\n");
		return;
	}
	// **********
	// **** Set Globals
	//TH1::AddDirectory(kFALSE);
	// **********
	// **** Start the timer
	TStopwatch timer;
	// **********
	// **** Set general constants
	const Int_t debug=0;
	//const Int_t maxsnapshots=0;
	Int_t maxpeakposcut_min=62, maxpeakposcut_max=90;
	const Float_t currentCut=5;
	const Int_t offsetentry = 2100;
//	const Int_t numstrips = 64;
	//Int_t multcutincl = 2;

	TString rootoutfilename = Form("summary/Compton_NewSummary_%i.root",runnumber);
	printf("opening %s for output\n",rootoutfilename.Data());
	TFile *file_out = TFile::Open(rootoutfilename.Data(),"RECREATE");

	TChain *Mps_Chain = new TChain("Mps_Tree");
	char namestring[255], rootfilesdir[255];
	sprintf(rootfilesdir,"%s", getenv("QW_ROOTFILES"));
	sprintf(namestring,"%s/Compton_Pass?_%d.*.root",rootfilesdir,runnumber);
	printf("finding %s\n",namestring);
	Bool_t chainExists = Mps_Chain->Add(namestring);
//	Bool_t chainExists = Mps_Chain->Add(Form("%s/Compton_Pass?_%d.000.root", getenv("QW_ROOTFILES"),runnumber));
	if (!chainExists) {
		printf("Could not find data files.\n");
		return;
	}  else {
		TObjArray *fileElements=Mps_Chain->GetListOfFiles();
		TIter next(fileElements);
		TChainElement *chEl=0;
		while (( chEl=(TChainElement*)next() )) {
			TFile f(chEl->GetTitle());
			printf("%s\n",chEl->GetTitle());
		}
	}
//	TFile *_file1 = TFile::Open(Form("%s/first100k_%d.root", getenv("QW_ROOTFILES"),runnumber));
//	TTree *Mps_Chain = (TTree*)_file1->Get("Mps_Tree");



	Int_t nentries = Mps_Chain->GetEntries();
	printf("There are %i entries\n",nentries);
	if (maxevents > 0) {
		nentries=std::min(maxevents+offsetentry,nentries);
		printf("Analysing the first %i entries\n",nentries);
	}
	// **********
	// **** Set constants required for histograms
	//Int_t bcm_nbins = 100;
	//Double_t bcm_axismin = 0;
	//Double_t bcm_axismax = 0;
	Int_t type_nbins = 4;
	Float_t type_axismin = 0;
	Float_t type_axismax = 4;
	Int_t diff_nbins = 1000;
	Double_t diff_axismin = -4000000;
	Double_t diff_axismax = 4000000;

	Int_t sampxpos_nbins = 256;
	Double_t sampxpos_axismin = 0;
	Double_t sampxpos_axismax = 256;
	Int_t maxnbins = 400;
	Double_t maxmin = -100;
	Double_t maxmax = 3900;
	Int_t minnbins = 200;
	Double_t min_axismin = -300;
	Double_t min_axismax = 100;
	Int_t pedval_nbins = 400;
	Double_t pedval_axismin = -100; //-20
	Double_t pedval_axismax = 300; //280
	Int_t integnbins=1000, yieldnbins=1000;
	Double_t integmin = 0.0, integmax = 0.0;

	//Int_t lowint, highint;
 	// **** read parameter file
	int i_run = -1, i_lowint = -1, i_upint = -1;
	size_t found;
	Int_t linecounter = 0, tokcounter;
	char linechar[255], params[255][10], *token;
 	char infilename[255];
	//char readout[255];
	sprintf(infilename,"paramfile.ini");
	printf("\nReading parameter input file %s\n",infilename);
	std::string linestr = "";
//	char a_run[255], a_lowint[255], a_upint[255];
	ifstream runlist(infilename);
	//runlist>>skipws;//skip whitespaces
	if(!runlist.is_open())
	{
		printf("Parameter file not found: %s\n",infilename);
		return;
	} else {
		while (!runlist.eof() &&  linecounter<200) {
			linecounter++;
			getline(runlist, linestr);
			found = linestr.find("!#",0,1);
			if (found!=std::string::npos) {
				std::cout << linestr << std::endl;
			} else {
				strcpy (linechar, linestr.c_str());
				// linechar now contains a c-string copy of linestr
				tokcounter=0;
				token=strtok (linechar," ");
				sprintf(params[tokcounter],"%s",token);
				while (token!=NULL) {
					//std::cout << tokcounter << " " << params[tokcounter] << ",  ";
					tokcounter++;
					token=strtok(NULL," ");
					sprintf(params[tokcounter],"%s",token);
				}
				//std::cout << std::endl;
			}
			Int_t temprun = atoi(params[0]);
			if (temprun > 20000 && temprun <= runnumber) {
				// i_ = atoi(a_);
				i_run = atoi(params[0]);
				integnbins = atoi(params[1]);	
				integmin = atoi(params[2]);	
				integmax = atoi(params[3]);	
				i_lowint = atoi(params[4]);			
				i_upint = atoi(params[5]);	
				maxnbins = atoi(params[6]);	
				maxmin = atoi(params[7]);	
				maxmax = atoi(params[8]);
				maxpeakposcut_min = atoi(params[9]);	
				maxpeakposcut_max = atoi(params[10]);
			}
		}
	}
 	printf("\nUsing the following values from parameter file:\n");
	printf("run number:                    %8i  \n", i_run);
	printf("number of integral bins:       %8i  \n", integnbins);
	printf("minimum expected integral:     %8f  \n", integmin);
	printf("maximum expected integra:      %8f  \n", integmax);
	printf("integral start from max:       %8i  \n", i_lowint);
	printf("integral end from max:         %8i  \n", i_upint);
	printf("number of max bins:            %8i  \n", maxnbins);
	printf("minimum expected max:          %8f  \n", maxmin);
	printf("maximum expected max:          %8f  \n", maxmax);
	printf("timing cut min sample:         %8i  \n", maxpeakposcut_min);
	printf("timing cut max sample:         %8i  \n", maxpeakposcut_max);


	// **** Determine the max and min for filling various quantities
	Mps_Chain->Draw("sca_laser_PowT.hw_sum>>laserpowinit(300)","Entry$>2100","goff");
	TH1F *laserpowinit = (TH1F*)gDirectory->Get("laserpowinit");
	laserpowinit->SetDirectory(file_out);
	Int_t nbins = laserpowinit->GetNbinsX();
	Float_t laserpowmax = laserpowinit->GetXaxis()->GetBinUpEdge(nbins);
	Float_t laseroncut = 0.7*laserpowmax;
	printf("max of sca_laser_PowT is %.1f, using %.1f as the cut position\n",laserpowmax,laseroncut);

	Mps_Chain->Draw("fadc_compton_accum0.hw_sum>>accum0init(300)","Entry$>2100","goff");
	TH1F *accum0init = (TH1F*)gDirectory->Get("accum0init");
	accum0init->SetDirectory(file_out);
	nbins = accum0init->GetNbinsX();
	Double_t  yield_RMS = accum0init->GetRMS();
	Double_t  yield_mean = accum0init->GetMean();
	Double_t  yield_supermin = yield_mean - 4 * yield_RMS;
	Double_t  yield_supermax = yield_mean + 4 * yield_RMS;
	Double_t  yield_axismin = accum0init->GetXaxis()->GetBinLowEdge(1);
	Double_t  yield_axismax = accum0init->GetXaxis()->GetBinUpEdge(nbins);
	printf("yield_mean %9.3g, yield_RMS %9.3g, yield_axismin %9.3g, yield_axismax %9.3g, yield_supermin %9.3g, yield_supermax %9.3g\n",
		   yield_mean, yield_RMS, yield_axismin, yield_axismax, yield_supermin, yield_supermax);
	yield_axismin = std::max(yield_axismin,yield_supermin);
	yield_axismax = std::min(yield_axismax,yield_supermax);

	Int_t timebins = (nentries-offsetentry)/960;
	Double_t starttime = offsetentry/960.0;
	Double_t endtime = nentries/960.0;
	printf("integral histo vals:\t(%4i,%9.3g,%9.3g)\n", integnbins,integmin,integmax);
	printf("yield histo vals:   \t(%4i,%9.3g,%9.3g)\n", yieldnbins,yield_axismin,yield_axismax);
	printf("time histo vals:    \t(%4i,%9.3g,%9.3g)\n", timebins,starttime,endtime);

	Mps_Chain->SetBranchStatus("*", 0);
	Mps_Chain->SetBranchStatus("fadc_compton_samples*", 1);
	Mps_Chain->SetBranchStatus("sca_laser_PowT", 1);
	Mps_Chain->SetBranchStatus("fadc_compton_accum0*", 1);
	Mps_Chain->SetBranchStatus("bcm*", 1);
	Mps_Chain->SetBranchStatus("actual_helicity*", 1);

// 	for (Int_t stripnum=0; stripnum < numstrips; stripnum++) {
// 		Mps_Chain->SetBranchStatus(Form("p1s%iRawAc",stripnum), 1);
// 		Mps_Chain->SetBranchStatus(Form("p2s%iRawAc",stripnum), 1);
// 		Mps_Chain->SetBranchStatus(Form("p3s%iRawAc",stripnum), 1);
// 	}

	Double_t bcm1, bcm2, bcm17;
	std::vector<QwSIS3320_Samples>* sampled_events = 0;
	Double_t sca_laser_PowT;
	Double_t fadc_compton_accum0[2];
	Double_t actual_helicity;
// 	Double_t plane1[numstrips];
// 	Double_t plane2[numstrips];
// 	Double_t plane3[numstrips];

	Mps_Chain->SetBranchAddress("bcm1_3h05",&bcm1);
	Mps_Chain->SetBranchAddress("bcm2_3h05a",&bcm2);
	Mps_Chain->SetBranchAddress("bcm6_3c17",&bcm17);
	Mps_Chain->SetBranchAddress("fadc_compton_samples",&sampled_events);
	Mps_Chain->SetBranchAddress("sca_laser_PowT",&sca_laser_PowT);
	Mps_Chain->SetBranchAddress("fadc_compton_accum0", &fadc_compton_accum0);
	Mps_Chain->SetBranchAddress("actual_helicity", &actual_helicity);
// 	for (Int_t stripnum=0; stripnum < numstrips; stripnum++) {
// 		Mps_Chain->SetBranchAddress(Form("p1s%iRawAc",stripnum),&plane1[stripnum]);
// 		Mps_Chain->SetBranchAddress(Form("p2s%iRawAc",stripnum),&plane2[stripnum]);
// 		Mps_Chain->SetBranchAddress(Form("p3s%iRawAc",stripnum),&plane3[stripnum]);
// 	}

	// **********
	// **** Define the histograms
	TH1F *snaphist = new TH1F("snaphist","Snapshot density vs time;time  (seconds);Snapshots/second",
							  timebins,starttime,endtime);
	snaphist->SetDirectory(file_out);
	TH2F *laser = new TH2F("laser","laser",300,0,300,type_nbins,type_axismin,type_axismax);
	laser->SetDirectory(file_out);
    TH2F *accum0 = new TH2F("accum0","accum0 yield",yieldnbins,yield_axismin,yield_axismax,type_nbins,type_axismin,type_axismax);
	accum0->SetDirectory(file_out);
	TH3F *lasertime = new TH3F("lasertime","Laser Power vs time;time  (seconds)",
							   timebins,starttime,endtime,300,0,300, type_nbins, type_axismin, type_axismax);
	lasertime->SetDirectory(file_out);
	TH3F *bcmtime = new TH3F("bcmtime","Beam Current vs time;time  (seconds)",
							 timebins,starttime,endtime,180,0,180, type_nbins, type_axismin, type_axismax);
	bcmtime->SetDirectory(file_out);
	TH3F *accum0time = new TH3F("accum0time","accum0 yield vs time;time  (seconds)",
							   timebins,starttime,endtime,yieldnbins,yield_axismin,yield_axismax, 
							   type_nbins, type_axismin, type_axismax);
	accum0time->SetDirectory(file_out);

	//TH1F *samp_bcm1 = new TH1F("samp_bcm1","bcm 1",bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_bcm2 = new TH1F("samp_bcm2","bcm 2",bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_bcm17 = new TH1F("samp_bcm17","bcm 17",bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_laserON_bcm1_hel0  = new TH1F("samp_laserON_bcm1_hel0", "bcm 1 hel0", bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_laserON_bcm2_hel0  = new TH1F("samp_laserON_bcm2_hel0", "bcm 2 hel0", bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_laserON_bcm17_hel0 = new TH1F("samp_laserON_bcm17_hel0","bcm 17 hel0",bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_laserON_bcm1_hel1  = new TH1F("samp_laserON_bcm1_hel1", "bcm 1 hel1", bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_laserON_bcm2_hel1  = new TH1F("samp_laserON_bcm2_hel1", "bcm 2 hel1", bcm_nbins,bcm_axismin,bcm_axismax);
	//TH1F *samp_laserON_bcm17_hel1 = new TH1F("samp_laserON_bcm17_hel1","bcm 17 hel1",bcm_nbins,bcm_axismin,bcm_axismax);

	TH2F *accum0_diff_pair = new TH2F("accum0_diff_pair","accum0 difference width pairs;difference in accum0;event type",
									  diff_nbins, diff_axismin, diff_axismax, type_nbins, type_axismin, type_axismax);
	accum0_diff_pair->SetDirectory(file_out);
	TH2F *accum0_diff_quad = new TH2F("accum0_diff_quad","accum0 difference width quads;difference in accum0;event type",
									  diff_nbins, diff_axismin, diff_axismax, type_nbins, type_axismin, type_axismax);
	accum0_diff_quad->SetDirectory(file_out);
	TH2F *accum0_diff_32 = new TH2F("accum0_diff_32","accum0 difference width 32s;difference in accum0;event type",
									diff_nbins, diff_axismin, diff_axismax, type_nbins, type_axismin, type_axismax);
	accum0_diff_32->SetDirectory(file_out);
	TH2F *samp_pedave = new TH2F("samp_pedave","samp 1 to 5 ave (max pos cut);pedestal ave;type",
								 pedval_nbins,pedval_axismin,pedval_axismax,type_nbins,type_axismin,type_axismax);
	samp_pedave->SetDirectory(file_out);
	TH2F *samp_subintegral = new TH2F("samp_subintegral"," integral (max pos cut, peak cut);type",
									  integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_subintegral->SetDirectory(file_out);
	TH2F *samp_pedprepeakave = new TH2F("samp_pedprepeakave","samp 10-6 before max ave;pedestal ave;type",
										pedval_nbins,pedval_axismin,pedval_axismax,type_nbins,type_axismin,type_axismax);
	samp_pedprepeakave->SetDirectory(file_out);
	TH2F *samp_integral = new TH2F("samp_integral"," integral;integral;type",
								   integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_integral->SetDirectory(file_out);
	TH2F *samp_max = new TH2F("samp_max"," max;max",
							  maxnbins,maxmin,maxmax,type_nbins,type_axismin,type_axismax);
	samp_max->SetDirectory(file_out);
// 	TH2F *samp_peakintegral = new TH2F("samp_peakintegral"," integral (peak cut);type",
// 									  integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
// 	samp_peakintegral->SetDirectory(file_out);

	TH2F *samp_max_hel0 = new TH2F("samp_max_hel0","max helicity 0;max",
							  maxnbins,maxmin,maxmax,type_nbins,type_axismin,type_axismax);
	samp_max_hel0->SetDirectory(file_out);
	TH2F *samp_max_hel1 = new TH2F("samp_max_hel1","max helicity 1;max",
							  maxnbins,maxmin,maxmax,type_nbins,type_axismin,type_axismax);
	samp_max_hel1->SetDirectory(file_out);

	TH2F *samp_maxpos = new TH2F("samp_maxpos"," max position;max position;type",
								 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,type_nbins,type_axismin,type_axismax);
	samp_maxpos->SetDirectory(file_out);
	TH2F *samp_min = new TH2F("samp_min"," min;min;type",
							  minnbins,min_axismin,min_axismax,type_nbins,type_axismin,type_axismax);
	samp_min->SetDirectory(file_out);
	TH2F *samp_minpos = new TH2F("samp_minpos"," min position; min position;type",
								 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,type_nbins,type_axismin,type_axismax);
	samp_minpos->SetDirectory(file_out);


	TH3F *samp_pedVSpedpp = new 
		TH3F("samp_pedVSpedpp",
			 "pre peak ped ave vs ped ave;samp 1-5 ave;samp 10-6 before max",
			 pedval_nbins,pedval_axismin,pedval_axismax,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVSpedpp->SetDirectory(file_out);
	TH3F *samp_maxVSmaxpos = new 
		TH3F("samp_maxVSmaxpos", "sample max vs max position;max position;max",
			 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,maxnbins,maxmin,maxmax,
			 type_nbins,type_axismin,type_axismax);
	samp_maxVSmaxpos->SetDirectory(file_out);
	TH3F *samp_minVSminpos = new 
		TH3F("samp_minVSminpos","sample min vs min position;min position;min",
			 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,minnbins,min_axismin,min_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_minVSminpos ->SetDirectory(file_out);
	TH3F *samp_pedVSyield = new 
		TH3F("samp_pedVSyield", "pedestal vs yield (max pos cut);accum0;ped min",
			 yieldnbins,yield_axismin,yield_axismax,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVSyield->SetDirectory(file_out);
	TH3F *samp_pedVSmin = new 
		TH3F("samp_pedVSmin", "pedestal vs min;min;ped min",
			 minnbins,min_axismin,min_axismax,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVSmin->SetDirectory(file_out);
	TH3F *samp_pedVStime = new 
		TH3F("samp_pedVStime","pedestal vs time;time  (seconds);ped min",
			 timebins,starttime,endtime,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVStime->SetDirectory(file_out);


	TH3F *samp_yieldVSsubint = new 
		TH3F("samp_yieldVSsubint",
			 "yield vs sample integral (max pos cut);snapshot integral;accum0 yield",
			 integnbins,integmin,integmax,yieldnbins,yield_axismin,yield_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_yieldVSsubint->SetDirectory(file_out);
	TH3F *samp_yieldVSsubint_hel1 = new 
		TH3F("samp_yieldVSsubint_hel1",
			 "yield vs sample integral hel 1;snapshot integral;accum0 yield",
			 integnbins,integmin,integmax,yieldnbins,yield_axismin,yield_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_yieldVSsubint_hel1->SetDirectory(file_out);
	TH3F *samp_yieldVSsubint_hel0 = new 
		TH3F("samp_yieldVSsubint_hel0",
			 "yield vs sample integral hel 0;snapshot integral;accum0 yield",
			 integnbins,integmin,integmax,yieldnbins,yield_axismin,yield_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_yieldVSsubint_hel0->SetDirectory(file_out);

// 	TH3F *samp_maxVSstrip_p1 = new TH3F("samp_maxVSstrip_p1","photon max vs strip, plane 1;photon max;strip",
// 									  maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_p1->SetDirectory(file_out);
// 	TH3F *samp_maxVSstrip_p2 = new TH3F("samp_maxVSstrip_p2","photon max vs strip, plane 2;photon max;strip",
// 										maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_p2->SetDirectory(file_out);
// 	TH3F *samp_maxVSstrip_p3 = new TH3F("samp_maxVSstrip_p3","photon max vs strip, plane 3;photon max;strip",
// 										maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_p3->SetDirectory(file_out);
// 	TH3F *samp_maxVSstrip_track_p1 = new TH3F("samp_maxVSstrip_track_p1","photon max vs strip, plane 1;photon max;strip",
// 											  maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_track_p1->SetDirectory(file_out);
// 	TH3F *samp_maxVSstrip_track_p2 = new TH3F("samp_maxVSstrip_track_p2","photon max vs strip, plane 2;photon max;strip",
// 											  maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_track_p2->SetDirectory(file_out);
// 	TH3F *samp_maxVSstrip_track_p3 = new TH3F("samp_maxVSstrip_track_p3","photon max vs strip, plane 3;photon max;strip",
// 											  maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_track_p3->SetDirectory(file_out);
// 	TH3F *samp_maxVStrack = new TH3F("samp_maxVStrack","photon max vs track position;photon max;track",
// 											  maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVStrack->SetDirectory(file_out);


// 	TH2F *plane1_mult = new TH2F("plane1_mult","plane 1 multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	plane1_mult->SetDirectory(file_out);
// 	TH2F *plane2_mult = new TH2F("plane2_mult","plane 2 multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	plane2_mult->SetDirectory(file_out);
// 	TH2F *plane3_mult = new TH2F("plane3_mult","plane 3 multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	plane3_mult->SetDirectory(file_out);
// 	TH2F *tracks_mult = new TH2F("tracks_mult","tracks multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	tracks_mult->SetDirectory(file_out);

// 	TH2F *plane12_offset = new TH2F("plane12_offset","plane 1 - plane 2 offset",9,-4.5,4.5,type_nbins,type_axismin,type_axismax); 
// 	plane12_offset->SetDirectory(file_out);
// 	TH2F *plane23_offset = new TH2F("plane23_offset","plane 2 - plane 3 offset",9,-4.5,4.5,type_nbins,type_axismin,type_axismax); 
// 	plane23_offset->SetDirectory(file_out);
// 	TH2F *plane31_offset = new TH2F("plane31_offset","plane 3 - plane 1 offset",9,-4.5,4.5,type_nbins,type_axismin,type_axismax); 
// 	plane31_offset->SetDirectory(file_out);

	TGraph *graphslaserON[20];
	TGraph *graphslaserOFF[20];
	TGraph *graphsbeamOFF[20];
	Int_t num_graphslaserON=0, num_graphslaserOFF=0, num_graphsbeamOFF=0;
	for(Int_t i=0; i<20; i++) {
		graphslaserON[i]=NULL;
		graphslaserOFF[i]=NULL;
		graphsbeamOFF[i]=NULL;
	}

	// **********
	// **** Define all the variables for the loop
	std::string text = "";
	Double_t bcm1cal, bcm2cal, bcm17cal, eventtime;
	//Int_t warningnum_accum=0;
	Int_t num_no_samples=0;
	Int_t num_badsnaps=0, num_badhelicity=0, num_badbcm=0, num_badmaxpos=0;
	//Int_t numsamp=0, numsnapshots=0;
	Int_t num_beamon_laseron=0, num_beamon_laseroff=0, num_beamon_laserbad=0, num_beamoff=0;
	Bool_t beamon, laseron, laseroff, printsample, drawsnapshot, screwedup, maxposgood, no_pileup;
	Int_t paircount=0, quadcount=0, octoquadcount=0;
	Double_t pairsum=0, pairdiff=0, /*pairasym=0,*/ quaddiff=0, octoquaddiff=0;
	Int_t type = 0;
	Double_t samp_min_val, samp_max_val;
	Int_t samp_minpos_val, samp_maxpos_val;
	Double_t samp_integral_val;
	Double_t samp_ped1, samp_ped2, samp_ped3, samp_ped4, samp_ped5, xgpos;
	Double_t samp_pedave_val, samp_pedprepeakave_val;
	Double_t samp_ped_maxm6, samp_ped_maxm7, samp_ped_maxm8, samp_ped_maxm9, samp_ped_maxm10;
	Double_t samp_peakintegral_val, samp_pointval;

// 	Double_t plane1sum, plane2sum, plane3sum;
// 	Bool_t goodtrack;
	// **********
	// **** Print initialisation time
	Double_t cputime = timer.CpuTime();
	Double_t realtime = timer.RealTime();
	printf ("The initial setup took %.2f seconds (%.2f s of cpu)\n",realtime,cputime);
	timer.Start(0);
	// **********
	// **** This loop steps through all the events
	for (Int_t jentry=offsetentry+1; jentry<nentries; jentry++) {
		//	printf("%i\n",jentry);
		Mps_Chain->GetEntry(jentry);
		if (bcm1==0) { // Must have a good BCM value to be useful 
			num_badbcm++;
		} else  {
			if (jentry==1) {
				printf("%s %s %s %s %s %s %s %s %s %s %s %s\n","    jentry ","  laser", " bcm1", "  type",
					   "      max ", "     maxpos", "   min ", "  minpos", "  pedave", 
					   " samp_int", "  accum0", "    comment");
			}
			eventtime = jentry/960.;
			bcm1cal  = 150.*(bcm1 -234.)/(505.-234.);
			bcm2cal  = 150.*(bcm2 -234.)/(485.-234.);
			bcm17cal = 150.*(bcm17-234.)/(380.-234.);
			// **********
			// **** Set the switches for the event
			laseron = sca_laser_PowT>laseroncut;
			laseroff = sca_laser_PowT<10;
			beamon = bcm17cal > currentCut;
			printsample = 0;
			screwedup = 0;
			drawsnapshot = 0;
			maxposgood = 0;
			no_pileup = 1;
// 			plane1sum=0; plane2sum=0; plane3sum=0;
// 			goodtrack=0;
			if (beamon && laseron) {
				type=0;
			}
			if (beamon && laseroff) {
				type=1;	      
			}
			if (!beamon) {
				type=2;
			}
			if (beamon && !laseroff && !laseron) {
				type=3;
			}
			switch(type)
			{
			case 0:
				num_beamon_laseron++;
				break; 
			case 1:
				num_beamon_laseroff++;
				break;
			case 2: 
				num_beamoff++;
				break; 
			case 3:
				num_beamon_laserbad++;
				break; 
			}

			if (actual_helicity==0) {      
			} else {
				if (actual_helicity==1) {
				} else {
					num_badhelicity++;
					if (num_badhelicity%100 == 0) {
						printf("Event %i is %i with bad helicity: %f\n",jentry,num_badhelicity,actual_helicity);
					}
				}
			}    
			lasertime->Fill(eventtime,sca_laser_PowT,type);
			bcmtime->Fill(eventtime,bcm1cal,type);
			accum0time->Fill(eventtime,fadc_compton_accum0[0],type);
			accum0->Fill(fadc_compton_accum0[0],type);
			// ****************
			// **** Here calculate the widths for various pattern lengths
			if (actual_helicity >= 0) {
				if (paircount==0) { 
					pairdiff =  fadc_compton_accum0[0];
					pairsum =  fadc_compton_accum0[0];
// 					       printf("%3i  %3i  %3i  %3i  accum0=%10.4g,  pairsum=%10.4g, pairdiff=%10.4g,",
// 								  jentry, paircount, quadcount, octoquadcount, fadc_compton_accum0[0], pairsum, pairdiff);
					paircount++;
				} else { 
					pairdiff -=  fadc_compton_accum0[0];
					pairsum +=  fadc_compton_accum0[0];
					//pairdiff = 100*pairdiff/pairsum;
					accum0_diff_pair->Fill(pairdiff,type);
					// 	if (pairdiff==0) {
// 					 	  printf("%3i  %3i  %3i  %3i  accum0=%10.4g,  pairsum=%10.4g, pairdiff=%10.4g, pairdiff=%8.4f %%,",
// 								 jentry, paircount, quadcount, octoquadcount, fadc_compton_accum0[0], pairsum, pairdiff, pairdiff);
					// 	}
					if (quadcount==0) {
						quaddiff = pairdiff/2.0;
						quadcount++;
					} else {
						quaddiff += pairdiff/2.0;
						accum0_diff_quad->Fill(quaddiff,type);
						//	printf(" quaddiff=%8.4f %%,", quaddiff);
						if (octoquadcount==0) {
							octoquaddiff = quaddiff/8.0;
							octoquadcount++;
						} else {
							octoquaddiff += quaddiff/8.0;
							octoquadcount++;
							if (octoquadcount==8) {
								accum0_diff_32->Fill(octoquaddiff,type);
								//  printf(" octoquaddiff=%8.4f %%", octoquaddiff);
								octoquadcount=0;
							}
						}
						quadcount=0;
					}
					pairdiff = 0;
					paircount = 0;
				}
				//printf("%3i  %3i  %3i  %3i  accum0=%10.4g,  pairsum=%10.4g, pairdiff=%10.4g, pairdiff=%8.4f %%, quaddiff=%8.4f %%, octoquaddiff=%8.4f %%\n",
				//	   jentry, paircount, quadcount, octoquadcount, fadc_compton_accum0[0], pairsum, pairdiff, pairdiff, quaddiff, octoquaddiff);

			}
//			printf("\n");
			// ****************
			// **** Moving on to snapshots we need to actually have one
			// **** But they can be done even with bad helicity
			if (sampled_events->size() <= 0) { // Did we get samples?
				num_no_samples++;
				if (debug>0) printf("Warning:%4i: no samples for event %i\n",num_no_samples,jentry);
			} else {
				samp_max_val = sampled_events->at(0).GetMaxSample();
				samp_min_val = sampled_events->at(0).GetMinSample();
				samp_maxpos_val = sampled_events->at(0).GetMaxIndex();
				samp_minpos_val = sampled_events->at(0).GetMinIndex();
				// samp_pedestal;// = sampled_events->at(0).GetPedestal();
				samp_integral_val = sampled_events->at(0).GetSum();
				TGraph* thegraph = sampled_events->at(0).GetGraph();
				char sampname[255], samptitle[255];
				thegraph->GetPoint(1,xgpos,samp_ped1);
				thegraph->GetPoint(2,xgpos,samp_ped2);
				thegraph->GetPoint(3,xgpos,samp_ped3);
				thegraph->GetPoint(4,xgpos,samp_ped4);
				thegraph->GetPoint(5,xgpos,samp_ped5);
				samp_pedave_val = (samp_ped1+samp_ped2+samp_ped3+samp_ped4+samp_ped5)/5.;
				samp_peakintegral_val = 0, samp_pointval = 0;
				if (samp_maxpos_val>12) {
					thegraph->GetPoint(samp_maxpos_val-6,xgpos,samp_ped_maxm6);
					thegraph->GetPoint(samp_maxpos_val-7,xgpos,samp_ped_maxm7);
					thegraph->GetPoint(samp_maxpos_val-8,xgpos,samp_ped_maxm8);
					thegraph->GetPoint(samp_maxpos_val-9,xgpos,samp_ped_maxm9);
					thegraph->GetPoint(samp_maxpos_val-10,xgpos,samp_ped_maxm10);
// 					for (Int_t i=samp_maxpos_val + i_lowint; i<=samp_maxpos_val + i_upint; i++) {
// 						thegraph->GetPoint(i,xgpos,samp_pointval);
// 						samp_peakintegral_val += samp_pointval;
// 						printf("%i  %.0f  %.0f,  ",i,samp_pointval,samp_peakintegral_val);
// 					}
// 					printf("\n");
				} else {
					samp_ped_maxm6=samp_ped1;
					samp_ped_maxm7=samp_ped2;
					samp_ped_maxm8=samp_ped3;
					samp_ped_maxm9=samp_ped4;
					samp_ped_maxm10=samp_ped5;
				}
				Double_t minped = std::min(std::min(std::min(samp_ped1,samp_ped2),std::min(samp_ped3,samp_ped4)),
						std::min(std::min(samp_ped_maxm6,samp_ped_maxm7),std::min(samp_ped_maxm8,samp_ped_maxm9)));
				samp_pedprepeakave_val = (samp_ped_maxm10+samp_ped_maxm6+samp_ped_maxm7+samp_ped_maxm8+samp_ped_maxm9)/5.;

				if (jentry%10000 == 0) printsample=1;
				if (printsample) {
					printf("%8i %3i%% %3.0f %6.1f  %i,%i,%i,%i %10.2f %8i %8.0f %i %8.0f %10.2f %10.4g %s\n",
						   jentry, 100*jentry/nentries, sca_laser_PowT, bcm1cal, laseron, laseroff, beamon, type,
						   samp_max_val, samp_maxpos_val, samp_min_val, samp_minpos_val, samp_pedave_val, samp_integral_val, 
						   fadc_compton_accum0[0], text.c_str());
					//       printf("%8i %3i%%\n",
					// 	     jentry, 100*jentry/nentries);
					printsample=0;
				}

				// this makes no sense, samp_min is a pointer to a histogram (wdc)
				//if (samp_min < -60000) {  // They can't be the overflow kind
				//	text = "screwed up";
				//	screwedup = 1;
				//	num_badsnaps++;
				//}
				if(samp_maxpos_val > maxpeakposcut_min && samp_maxpos_val < maxpeakposcut_max) {
					maxposgood=1;
				}
				if (!screwedup) {
					snaphist->Fill(eventtime);
					if (num_graphslaserON<20 && type == 0) {
						sprintf(samptitle,"Entry %i",jentry);
						sprintf(sampname,"laserON%02i",num_graphslaserON);
						graphslaserON[num_graphslaserON] = sampled_events->at(0).GetGraph();
						graphslaserON[num_graphslaserON]->SetTitle(samptitle);
						graphslaserON[num_graphslaserON]->SetName(sampname);
						//graphslaserON[num_graphslaserON]->SetDirectory(file_out);
						//printf("Writing shapshot  %s  %s\n",sampname,samptitle);
						file_out->cd();
						graphslaserON[num_graphslaserON]->Write();
						num_graphslaserON++;
					}
					if (num_graphslaserOFF<20 && type == 1) {
						sprintf(samptitle,"Entry %i",jentry);
						sprintf(sampname,"laserOFF%02i",num_graphslaserOFF);
						graphslaserOFF[num_graphslaserOFF] = sampled_events->at(0).GetGraph();
						graphslaserOFF[num_graphslaserOFF]->SetTitle(samptitle);
						graphslaserOFF[num_graphslaserOFF]->SetName(sampname);
						//graphslaserOFF[num_graphslaserOFF]->SetDirectory(file_out);
						//printf("Writing shapshot  %s  %s\n",sampname,samptitle);
						file_out->cd();
						graphslaserOFF[num_graphslaserOFF]->Write();
						num_graphslaserOFF++;
					}
					if (num_graphsbeamOFF<20 && type == 2) {
						sprintf(samptitle,"Entry %i",jentry);
						sprintf(sampname,"beamOFF%02i",num_graphsbeamOFF);
						graphsbeamOFF[num_graphsbeamOFF] = sampled_events->at(0).GetGraph();
						graphsbeamOFF[num_graphsbeamOFF]->SetTitle(samptitle);
						graphsbeamOFF[num_graphsbeamOFF]->SetName(sampname);
						//graphsbeamOFF[num_graphsbeamOFF]->SetDirectory(file_out);
						//printf("Writing shapshot   %s  %s\n",sampname,samptitle);
						file_out->cd();
						graphsbeamOFF[num_graphsbeamOFF]->Write();
						num_graphsbeamOFF++;
					}
					samp_integral->Fill(samp_integral_val,type);
					samp_max->Fill(samp_max_val,type);
					samp_maxpos->Fill(samp_maxpos_val,type);
					samp_maxVSmaxpos->Fill(samp_maxpos_val,samp_max_val,type);
					samp_min->Fill(samp_min_val,type);
					samp_minpos->Fill(samp_minpos_val,type);
					samp_minVSminpos->Fill(samp_minpos_val,samp_min_val,type);
					if (!maxposgood) {
						num_badmaxpos++;
					} else {
						Int_t maxpoint = thegraph->GetN();
						maxpoint = std::min(maxpoint, samp_maxpos_val + i_upint); // Use the last point if the length is too long.
						for (Int_t i=samp_maxpos_val + i_lowint; i<=maxpoint; i++) {
							thegraph->GetPoint(i,xgpos,samp_pointval);
							samp_peakintegral_val += samp_pointval;
							//printf("%i  %.0f  %.0f,  ",i,samp_pointval,samp_peakintegral_val);
						}
						//printf("%i  %i  %.0f\n",  jentry, samp_maxpos_val, samp_peakintegral_val);
						samp_pedprepeakave->Fill(samp_pedprepeakave_val,type);
						samp_pedave->Fill(samp_pedave_val,type);
						samp_pedVSpedpp->Fill(samp_pedave_val,samp_pedprepeakave_val,type);
						//samp_peakintegral->Fill(samp_peakintegral_val,type);
						samp_subintegral->Fill(samp_peakintegral_val,type);
						samp_yieldVSsubint->Fill(samp_peakintegral_val,fadc_compton_accum0[0],type);
						if (actual_helicity==0) {
							samp_max_hel0->Fill(samp_max_val,type);
							//samp_yieldVSsubint_hel0->Fill(samp_integral_val,fadc_compton_accum0[0]);
							// 	      samp_bcm1_hel0->Fill(bcm1cal);
							// 	      samp_bcm2_hel0->Fill(bcm2cal);
							// 	      samp_bcm17_hel0->Fill(bcm17cal); 
						} else {
							if (actual_helicity==1) {
								samp_max_hel1->Fill(samp_max_val,type);
								//samp_yieldVSsubint_hel1->Fill(samp_integral_val,fadc_compton_accum0[0]);
								// 		samp_bcm1_hel1->Fill(bcm1cal); 
								// 		samp_bcm2_hel1->Fill(bcm2cal); 
								// 		samp_bcm17_hel1->Fill(bcm17cal);
							}
						}
						samp_pedVSyield->Fill(fadc_compton_accum0[0],minped,type);
//						samp_pedVSyield->Fill(fadc_compton_accum0[0],minpedsamp_pedprepeakave_val,type);
						samp_pedVSmin->Fill(samp_min_val,minped,type);
						samp_pedVStime->Fill(eventtime,minped,type);
					}
				}
			}
		}
	}
	cputime = timer.CpuTime();
	realtime = timer.RealTime();
	
// 	file_out->cd();
// 	for (Int_t i=0; i<20; i++) {
// 		std::cout << "Writing beam OFF " << i << "  " << graphsbeamOFF[i] << std::endl;
// 		if (graphsbeamOFF[i]) {
// 			//graphsbeamOFF[i]->SetDirectory(file_out);	
// 			graphsbeamOFF[i]->Write();	
// 		}
// 	}
// 	for (Int_t i=0; i<20; i++) {
// 		std::cout << "Writing laser ON " << i << "  " << graphslaserON[i] << std::endl;
// 		if (graphslaserON[i]){
// 			//	graphslaserON[i]->SetDirectory(file_out);	
// 			graphslaserON[i]->Write();	
// 		}
// 	}
// 	for (Int_t i=0; i<20; i++) {
// 		if (graphslaserOFF[i]){
// 			//	graphslaserOFF[i]->SetDirectory(file_out);	
// 			graphslaserOFF[i]->Write();	
// 		}
// 	}
	
	file_out->Write();
	file_out->Close();
	
	char textfilename[255];
	sprintf(textfilename,"www/run_%i/stepthru_mps_time.log", runnumber);
	printf("Writing time log to %s\n",textfilename);
	FILE *outfile;
	time_t rawtime;
	time ( &rawtime );
	outfile = fopen(textfilename,"a");
	fprintf(outfile," Analysis at took %8.2f s \t(%8.2f s of cpu), %i entries, done on %s",
			realtime,cputime,nentries,ctime(&rawtime));
	fclose(outfile);
  Int_t systemReturnValue = 0;
	systemReturnValue = system(Form("cat %s",textfilename));
	
	sprintf(textfilename,"www/run_%i/stepthru_mps.log", runnumber);
	printf("Writing log to %s\n",textfilename);
	outfile = fopen(textfilename,"w");
	fprintf(outfile," Total events:           %8i     \n", nentries);
	fprintf(outfile," Snaps, beam and laser:  %8i     \n", num_beamon_laseron);
	fprintf(outfile," Snaps, beam no laser:   %8i     \n", num_beamon_laseroff);
	fprintf(outfile," Snaps, beam some laser: %8i     \n", num_beamon_laserbad);
	fprintf(outfile," Snaps, no beam:         %8i     \n", num_beamoff);
	fprintf(outfile," No snapshots:           %8i     \n", num_no_samples);
	fprintf(outfile," Bad snapshots:          %8i     \n", num_badsnaps);
	fprintf(outfile," Bad helicity:           %8i     \n", num_badhelicity);
	fprintf(outfile," Bad BCM:                %8i     \n", num_badbcm);
	fprintf(outfile," Bad Max position:       %8i     \n", num_badmaxpos);
	fclose(outfile);
	systemReturnValue = system(Form("cat %s",textfilename));
	
}

/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
