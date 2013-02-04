#include "Snapshots.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

#include "TCanvas.h"
#include "TChainElement.h"
#include "TChain.h"
#include "TCollection.h"
#include "TCut.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "THStack.h"
#include "TIterator.h"
#include "TLatex.h"
#include "TPad.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TStyle.h"
#include "TSystem.h"

#include "globals.C"
#include "plotting.C"
#include "stepthru_mps.C"

#include "compton_config.h"


#include "QwSIS3320_Samples.h"


#include "getChain.C"

// Notes:
// Run this script as follows
// source ~/compton/env.sh
// .x /home/cdaq/compton/QwAnalysis/Extensions/Compton/macros/utils/run_macro.C("snapshots/snapshots.C","snapshots","snapshots",23507,kTRUE,kTRUE)
// log file at $QWSCRATCH/log/snapshot_23507.log
//qwroot -l -b -q '/home/cdaq/compton/QwAnalysis/Extensions/Compton/macros/utils/run_macro.C("snapshots/snapshots.C","snapshots","snapshots",23507,kTRUE,kTRUE)'

//void snapshots(Int_t runnum() = 0, Bool_t isFirst100k = kFALSE, Int_t maxevents = 0)

void Snapshots::init( ComptonSession* session ) {
  VComptonMacro::init(session);
  VComptonMacro::SetName("Snapshots");
}

void Snapshots::run()
{
  /* removed, no longer needed (?) (jc2)
	if (runnum()==0) {
		printf("\n\nUseage:\n\t .x stepthru_mps.C(runnum(), [first100k, [maxevents]])\n");
		printf("\t .x stepthru_mps_DEVEL.C(runnum(), [first100k, [maxevents]])\n\n");
		return;
	}
  */
	// **********
	// **** Start the timer
	TStopwatch timer;
	// **********
	// **** Set general constants
	const Int_t debug=0;
	//const Int_t maxsnapshots=0;
	Int_t maxpeakposcut_min=62, maxpeakposcut_max=90;
	Float_t current_beamon=150;
	Float_t current_beamoff=10;
	const Int_t offsetentry = 2100;
//	const Int_t numstrips = 64;
	//Int_t multcutincl = 2;

  TString rootoutdir = GetStorageDir() + "/summary/";
	//TString rootoutdir  = TString(getenv("QW_ROOTFILES")) + "/summary/";
	gSystem->mkdir(rootoutdir.Data(),kTRUE);
	TString rootoutname = rootoutdir + Form("Compton_NewSummary_%i.root",runnum());
	TFile*  rootoutfile = TFile::Open(rootoutname,"RECREATE");

	//TChain *Mps_Chain = getMpsChain(runnum(),isFirst100k);
  TChain *Mps_Chain = chain(0);


	Int_t nentries = Mps_Chain->GetEntries();
  Int_t maxevents = nentries;
	printf("There are %i entries\n",nentries);
	if (maxevents > 0) {
		nentries=std::min(maxevents+offsetentry,nentries);
		printf("Analysing the first %i entries\n",nentries);
	}
	// **********
	// **** Set constants required for histograms
	Int_t bcm_nbins = 200;
	Float_t bcm_axismin = 0.0;
	Float_t bcm_axismax = 200.0;
	Int_t laser_nbins = 100;
	Float_t laser_axismin = 0.0;
	Float_t laser_axismax = 200000.0;
	Int_t type_nbins = numtypes;
	Float_t type_axismin = 0;
	Float_t type_axismax = numtypes;
	Int_t diff_nbins = 1000;
	Float_t diff_axismin = -4000000;
	Float_t diff_axismax = 4000000;

	Int_t sampxpos_nbins = 256;
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
	Int_t intnormintminchan = 0;
	Int_t intnormintmaxchan = 0;
	Int_t maxnormintminchan = 0;
	Int_t maxnormintmaxchan = 0;
	Int_t intrebinnum = 1;

	Int_t maxcomptonedge = 250;
	Int_t intcomptonedge = 1000;
	Int_t bigintoffset = 1500;
	Float_t bigsnapmin = 3000;

 	// **** read parameter file
	int i_run = -1, i_lowint = -1, i_upint = -1;
	char params[255][20];
	TString infiledir = TString(getenv("QWANALYSIS")) + "/Extensions/Macros/Compton/snapshots/";
	TString infilename = infiledir + "paramfile.ini";
	std::cout << "Reading parameter input file " << infilename << std::endl;
	ifstream runlist(infilename.Data());
	if (!runlist.is_open())
	{
		printf("Parameter file not found: %s\n",infilename.Data());
		return;
	} else {
		Int_t linecounter = 0;
		while (!runlist.eof() &&  linecounter<200) {
			linecounter++;
			std::string linestr = "";
			getline(runlist, linestr);
			size_t found = linestr.find("!#",0,1);
			if (found != std::string::npos) {
				//std::cout << linestr << " (ignored)" << std::endl;
			} else {
				//std::cout << linestr << std::endl;
				char linechar[255];
				strcpy (linechar, linestr.c_str());
				// linechar now contains a c-string copy of linestr
				Int_t tokcounter = 0;
				char* token = strtok (linechar," ");
				while (token != NULL) {
					sprintf(params[tokcounter],"%s",token);
					//std::cout << tokcounter << " " << params[tokcounter] << ",  ";
					tokcounter++;
					token = strtok(NULL," ");
				}
				//std::cout << std::endl;
			}
			Int_t temprun = atoi(params[0]);
			if (temprun > 20000 && temprun <= runnum()) {
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
				sampxpos_nbins = atoi(params[9]);
				maxpeakposcut_min = atoi(params[10]);	
				maxpeakposcut_max = atoi(params[11]);
				intnormintminchan = atoi(params[12]);
				intnormintmaxchan = atoi(params[13]);
				maxnormintminchan = atoi(params[14]);
				maxnormintmaxchan = atoi(params[15]);
				current_beamon = atoi(params[16]);
				current_beamon = atoi(params[17]);
				intrebinnum = atoi(params[18]);
			}
		}
	}
 	printf("\nUsing the following values from parameter file:\n");
	printf("run number:                                        %8i  \n", i_run);
	printf("number of integral bins:                           %8i  \n", integnbins);
	printf("minimum expected integral:                         %8.0f  \n", integmin);
	printf("maximum expected integral:                         %8.0f  \n", integmax);
	printf("sample peak integral start (offset from max pos):  %8i  \n", i_lowint);
	printf("sample peak integral end (offset from max pos):    %8i  \n", i_upint);
	printf("number of bins for sample max:                     %8i  \n", maxnbins);
	printf("low bin for sample max:                            %8.0f  \n", maxmin);
	printf("high bin for sample max:                           %8.0f  \n", maxmax);
	printf("number of samples per snapshot:                    %8i  \n", sampxpos_nbins);
	printf("timing cut min sample:                             %8i  \n", maxpeakposcut_min);
	printf("timing cut max sample:                             %8i  \n", maxpeakposcut_max);
	printf("integral norm min channel                          %8i  \n", intnormintminchan);
	printf("maximum norm max channel                           %8i  \n", intnormintmaxchan);
	printf("integral norm min channel                          %8i  \n", maxnormintminchan);
	printf("maximum norm max channel                           %8i  \n", maxnormintmaxchan);
	printf("beam current zero threshold                        %8.0f  \n", current_beamoff);
	printf("beam current minimum                               %8.0f  \n", current_beamon);
	printf("number of bins to combine for asymmetry analysis   %8i  \n", intrebinnum);

	Double_t sampxpos_axismin = 0;
	Double_t sampxpos_axismax = sampxpos_nbins;
	Int_t maxpeakposcut_width =  maxpeakposcut_max - maxpeakposcut_min;
	// **** Really important number for pedestal correction
	Int_t numsamples = i_upint - i_lowint;

	// **** Determine the max and min for filling various quantities
	Mps_Chain->Draw(Form("sca_laser_PowT.value>>laserpowinit(300,%f,%f)",laser_axismin,laser_axismax),"Entry$>2100","goff");
	TH1F *laserpowinit = (TH1F*)gDirectory->Get("laserpowinit");
	laserpowinit->SetDirectory(rootoutfile);
	Int_t highbin = laserpowinit->FindLastBinAbove(0, 1);
	Float_t laserpowmax = laserpowinit->GetXaxis()->GetBinUpEdge(highbin);
	Float_t laseroncut = 0.7*laserpowmax;
	Float_t laseroffcut = 0.1*laserpowmax;
	printf("max of sca_laser_PowT is %.1f, using %.1f as the cut position\n",laserpowmax,laseroncut);

	Mps_Chain->Draw("fadc_compton_accum0.hw_sum>>accum0init(300)","Entry$>2100","goff");
	TH1F *accum0init = (TH1F*)gDirectory->Get("accum0init");
	accum0init->SetDirectory(rootoutfile);
	Int_t nbins = accum0init->GetNbinsX();
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

	// make the minimum good beam current the greter of the min from the param file or 85% of the highest beam current
	Mps_Chain->Draw("sca_bcm1.hw_sum>>bcminit(200,0,200)","Entry$>2100","goff");
	TH1F *bcminit = (TH1F*)gDirectory->Get("bcminit");
	bcminit->SetDirectory(rootoutfile);
	highbin = bcminit->FindLastBinAbove(0, 1);
	Float_t bcmmax = bcminit->GetXaxis()->GetBinUpEdge(highbin);
	printf ("bcmmax=%f\n",bcmmax);
	if (current_beamon < 0.85*bcmmax) current_beamon = 0.85*bcmmax;
//	current_beamon = std::max(current_beamon,0.85*bcmmax);

	Int_t timebins = (nentries-offsetentry)/960;
	Double_t starttime = offsetentry/960.0;
	Double_t endtime = nentries/960.0;
	printf("integral histo vals:\t(%4i,%9.3g,%9.3g)\n", integnbins,integmin,integmax);
	printf("yield histo vals:   \t(%4i,%9.3g,%9.3g)\n", yieldnbins,yield_axismin,yield_axismax);
	printf("time histo vals:    \t(%4i,%9.3g,%9.3g)\n", timebins,starttime,endtime);
	printf("bcm histo vals:     \t(%4i,%9.3g,%9.3g)\n", bcm_nbins,bcm_axismin,bcm_axismax);

	Mps_Chain->SetBranchStatus("*", 0);
	Mps_Chain->SetBranchStatus("fadc_compton_samples*", 1);
	Mps_Chain->SetBranchStatus("sca_laser_PowT*", 1);
	Mps_Chain->SetBranchStatus("fadc_compton_accum0*", 1);
	Mps_Chain->SetBranchStatus("sca_bcm*", 1);
	Mps_Chain->SetBranchStatus("actual_helicity*", 1);

// 	for (Int_t stripnum=0; stripnum < numstrips; stripnum++) {
// 		Mps_Chain->SetBranchStatus(Form("p1s%iRawAc",stripnum), 1);
// 		Mps_Chain->SetBranchStatus(Form("p2s%iRawAc",stripnum), 1);
// 		Mps_Chain->SetBranchStatus(Form("p3s%iRawAc",stripnum), 1);
// 	}

	Double_t bcm1[3], bcm2[3], bcm6[3];
	std::vector<QwSIS3320_Samples>* sampled_events = 0;
	Double_t sca_laser_PowT[3];
	Double_t fadc_compton_accum0[2];
	Double_t actual_helicity;
// 	Double_t plane1[numstrips];
// 	Double_t plane2[numstrips];
// 	Double_t plane3[numstrips];

	Mps_Chain->SetBranchAddress("sca_bcm1",&bcm1);
	Mps_Chain->SetBranchAddress("sca_bcm2",&bcm2);
	Mps_Chain->SetBranchAddress("sca_bcm6",&bcm6);
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
	snaphist->SetDirectory(rootoutfile);

	TH2F *laser = new TH2F("laser","laser",
				300, 0, laserpowmax,
				type_nbins, type_axismin, type_axismax);
	laser->SetDirectory(rootoutfile);

	TH2F *accum0 = new TH2F("accum0","accum0 yield",
				yieldnbins, yield_axismin, yield_axismax,
				type_nbins, type_axismin, type_axismax);
	accum0->SetDirectory(rootoutfile);

	TH3F *lasertime = new TH3F("lasertime","Laser Power vs time;time  (seconds)",
				timebins, starttime, endtime,
				laser_nbins, laser_axismin, laser_axismax,
				type_nbins, type_axismin, type_axismax);
	lasertime->SetDirectory(rootoutfile);

	TH3F *bcmtime = new TH3F("bcmtime","Beam Current vs time;time  (seconds)",
				timebins, starttime, endtime,
				bcm_nbins, bcm_axismin, bcm_axismax,
				type_nbins, type_axismin, type_axismax);
	bcmtime->SetDirectory(rootoutfile);

	TH3F *accum0time = new TH3F("accum0time","accum0 yield vs time;time  (seconds)",
				timebins, starttime, endtime,
				yieldnbins, yield_axismin, yield_axismax, 
				type_nbins, type_axismin, type_axismax);
	accum0time->SetDirectory(rootoutfile);

	TH2F *samp_bcm1 = new TH2F("samp_bcm1","bcm 1;beam current  (uA);type",
							   bcm_nbins,bcm_axismin,bcm_axismax, type_nbins, type_axismin, type_axismax);
	samp_bcm1->SetDirectory(rootoutfile);
	TH2F *samp_bcm2 = new TH2F("samp_bcm2","bcm 2;beam current  (uA);type",
							   bcm_nbins,bcm_axismin,bcm_axismax, type_nbins, type_axismin, type_axismax);
	samp_bcm2->SetDirectory(rootoutfile);
	TH2F *samp_bcm6 = new TH2F("samp_bcm6","bcm 6;beam current  (uA);type",
								bcm_nbins,bcm_axismin,bcm_axismax, type_nbins, type_axismin, type_axismax);
	samp_bcm6->SetDirectory(rootoutfile);

// 	TH1F *samp_bcm1 = new TH1F("samp_bcm1","bcm 1", bcm_nbins, bcm_axismin, bcm_axismax);
// 	TH1F *samp_bcm2 = new TH1F("samp_bcm2","bcm 2", bcm_nbins, bcm_axismin, bcm_axismax);
// 	TH1F *samp_bcm6 = new TH1F("samp_bcm6","bcm 6", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm1_hel0 = new TH1F("samp_bcm1_hel0", "bcm 1 hel0", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm2_hel0 = new TH1F("samp_bcm2_hel0", "bcm 2 hel0", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm6_hel0 = new TH1F("samp_bcm6_hel0", "bcm 6 hel0", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm1_hel1 = new TH1F("samp_bcm1_hel1", "bcm 1 hel1", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm2_hel1 = new TH1F("samp_bcm2_hel1", "bcm 2 hel1", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm6_hel1 = new TH1F("samp_bcm6_hel1", "bcm 6 hel1", bcm_nbins, bcm_axismin, bcm_axismax);

	TH2F *accum0_diff_pair = new TH2F("accum0_diff_pair","accum0 difference width pairs;difference in accum0;event type",
									  diff_nbins, diff_axismin, diff_axismax, type_nbins, type_axismin, type_axismax);
	accum0_diff_pair->SetDirectory(rootoutfile);
	TH2F *accum0_diff_quad = new TH2F("accum0_diff_quad","accum0 difference width quads;difference in accum0;event type",
									  diff_nbins, diff_axismin, diff_axismax, type_nbins, type_axismin, type_axismax);
	accum0_diff_quad->SetDirectory(rootoutfile);
	TH2F *accum0_diff_32 = new TH2F("accum0_diff_32","accum0 difference width 32s;difference in accum0;event type",
									diff_nbins, diff_axismin, diff_axismax, type_nbins, type_axismin, type_axismax);
	accum0_diff_32->SetDirectory(rootoutfile);
	TH2F *samp_pedave = new TH2F("samp_pedave","samp 1 to 5 ave (max pos cut);pedestal ave;type",
								 pedval_nbins,pedval_axismin,pedval_axismax,type_nbins,type_axismin,type_axismax);
	samp_pedave->SetDirectory(rootoutfile);
	TH2F *samp_subintegral = new TH2F("samp_subintegral",Form("integral around maximum sample, timing cut;sum of %i samples;type",numsamples),
									  integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_subintegral->SetDirectory(rootoutfile);
	TH2F *samp_timing_integral = new TH2F("samp_timing_integral",Form("integral within timing region, no cuts;sum of %i samples;type",maxpeakposcut_width),
									  integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_timing_integral->SetDirectory(rootoutfile);
	TH2F *samp_pedprepeakave = new TH2F("samp_pedprepeakave","samp 10-6 before max ave;pedestal ave;type",
										pedval_nbins,pedval_axismin,pedval_axismax,type_nbins,type_axismin,type_axismax);
	samp_pedprepeakave->SetDirectory(rootoutfile);
	TH2F *samp_integral = new TH2F("samp_integral",Form("full snapshot integral, no cuts;sum of %i samples;type",sampxpos_nbins),
								   integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_integral->SetDirectory(rootoutfile);
	TH2F *samp_max = new TH2F("samp_max","snapshot maximum sample, no cuts;max",
							  maxnbins,maxmin,maxmax,type_nbins,type_axismin,type_axismax);
	samp_max->SetDirectory(rootoutfile);
// 	TH2F *samp_peakintegral = new TH2F("samp_peakintegral"," integral (peak cut);type",
// 									  integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
// 	samp_peakintegral->SetDirectory(rootoutfile);

	TH2F *samp_max_hel0 = new TH2F("samp_max_hel0","max helicity 0;max",
							  maxnbins,maxmin,maxmax,type_nbins,type_axismin,type_axismax);
	samp_max_hel0->SetDirectory(rootoutfile);
	TH2F *samp_max_hel1 = new TH2F("samp_max_hel1","max helicity 1;max",
							  maxnbins,maxmin,maxmax,type_nbins,type_axismin,type_axismax);
	samp_max_hel1->SetDirectory(rootoutfile);

	TH2F *samp_maxpos = new TH2F("samp_maxpos"," max position;max position;type",
								 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,type_nbins,type_axismin,type_axismax);
	samp_maxpos->SetDirectory(rootoutfile);
	TH2F *samp_min = new TH2F("samp_min"," min;min;type",
							  minnbins,min_axismin,min_axismax,type_nbins,type_axismin,type_axismax);
	samp_min->SetDirectory(rootoutfile);
	TH2F *samp_minpos = new TH2F("samp_minpos"," min position; min position;type",
								 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,type_nbins,type_axismin,type_axismax);
	samp_minpos->SetDirectory(rootoutfile);


	TH3F *samp_pedVSpedpp = new 
		TH3F("samp_pedVSpedpp",
			 "pre peak ped ave vs ped ave;samp 1-5 ave;samp 10-6 before max",
			 pedval_nbins,pedval_axismin,pedval_axismax,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVSpedpp->SetDirectory(rootoutfile);
	TH3F *samp_maxVSmaxpos = new 
		TH3F("samp_maxVSmaxpos", "sample max vs max position;max position;max",
			 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,maxnbins,maxmin,maxmax,
			 type_nbins,type_axismin,type_axismax);
	samp_maxVSmaxpos->SetDirectory(rootoutfile);
	TH3F *samp_minVSminpos = new 
		TH3F("samp_minVSminpos","sample min vs min position;min position;min",
			 sampxpos_nbins,sampxpos_axismin,sampxpos_axismax,minnbins,min_axismin,min_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_minVSminpos ->SetDirectory(rootoutfile);
	TH3F *samp_pedVSyield = new 
		TH3F("samp_pedVSyield", "pedestal vs yield (max pos cut);accum0;ped min",
			 yieldnbins,yield_axismin,yield_axismax,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVSyield->SetDirectory(rootoutfile);
	TH3F *samp_pedVSmin = new 
		TH3F("samp_pedVSmin", "pedestal vs min;min;ped min",
			 minnbins,min_axismin,min_axismax,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVSmin->SetDirectory(rootoutfile);
	TH3F *samp_pedVStime = new 
		TH3F("samp_pedVStime","pedestal vs time;time  (seconds);ped min",
			 timebins,starttime,endtime,pedval_nbins,pedval_axismin,pedval_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_pedVStime->SetDirectory(rootoutfile);


	TH3F *samp_yieldVSsubint = new 
		TH3F("samp_yieldVSsubint",
			 "yield vs sample integral (max pos cut);snapshot integral;accum0 yield",
			 integnbins,integmin,integmax,yieldnbins,yield_axismin,yield_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_yieldVSsubint->SetDirectory(rootoutfile);
	TH3F *samp_yieldVSsubint_hel1 = new 
		TH3F("samp_yieldVSsubint_hel1",
			 "yield vs sample integral hel 1;snapshot integral;accum0 yield",
			 integnbins,integmin,integmax,yieldnbins,yield_axismin,yield_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_yieldVSsubint_hel1->SetDirectory(rootoutfile);
	TH3F *samp_yieldVSsubint_hel0 = new 
		TH3F("samp_yieldVSsubint_hel0",
			 "yield vs sample integral hel 0;snapshot integral;accum0 yield",
			 integnbins,integmin,integmax,yieldnbins,yield_axismin,yield_axismax,
			 type_nbins,type_axismin,type_axismax);
	samp_yieldVSsubint_hel0->SetDirectory(rootoutfile);

	TH3F *samp_intVSmax = new 
		TH3F("samp_intVSmax", "integral vs max (no cuts);snapshot maximum;snapshot integral",
			 maxnbins,maxmin,maxmax,integnbins,integmin,integmax,
			 type_nbins,type_axismin,type_axismax);
	samp_intVSmax->SetDirectory(rootoutfile);
	TH3F *samp_subintVSmax = new 
		TH3F("samp_subintVSmax", "integral within timing region vs max;snapshot maximum;snapshot integral",
			 maxnbins,maxmin,maxmax,integnbins,integmin,integmax,
			 type_nbins,type_axismin,type_axismax);
	samp_subintVSmax->SetDirectory(rootoutfile);
	TH3F *samp_timingintVSmax = new 
		TH3F("samp_timingintVSmax", "integral vs max, timing cut;snapshot maximum;snapshot integral",
			 maxnbins,maxmin,maxmax,integnbins,integmin,integmax,
			 type_nbins,type_axismin,type_axismax);
	samp_timingintVSmax->SetDirectory(rootoutfile);



// 	TH3F *samp_maxVSstrip_p1 = new TH3F("samp_maxVSstrip_p1","photon max vs strip, plane 1;photon max;strip",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_p1->SetDirectory(rootoutfile);
// 	TH3F *samp_maxVSstrip_p2 = new TH3F("samp_maxVSstrip_p2","photon max vs strip, plane 2;photon max;strip",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_p2->SetDirectory(rootoutfile);
// 	TH3F *samp_maxVSstrip_p3 = new TH3F("samp_maxVSstrip_p3","photon max vs strip, plane 3;photon max;strip",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_p3->SetDirectory(rootoutfile);
// 	TH3F *samp_maxVSstrip_track_p1 = new TH3F("samp_maxVSstrip_track_p1","photon max vs strip, plane 1;photon max;strip",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_track_p1->SetDirectory(rootoutfile);
// 	TH3F *samp_maxVSstrip_track_p2 = new TH3F("samp_maxVSstrip_track_p2","photon max vs strip, plane 2;photon max;strip",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_track_p2->SetDirectory(rootoutfile);
// 	TH3F *samp_maxVSstrip_track_p3 = new TH3F("samp_maxVSstrip_track_p3","photon max vs strip, plane 3;photon max;strip",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVSstrip_track_p3->SetDirectory(rootoutfile);
// 	TH3F *samp_maxVStrack = new TH3F("samp_maxVStrack","photon max vs track position;photon max;track",
// 					maxnbins,maxmin,maxmax,numstrips,0,numstrips,type_nbins,type_axismin,type_axismax);
// 	samp_maxVStrack->SetDirectory(rootoutfile);


// 	TH2F *plane1_mult = new TH2F("plane1_mult","plane 1 multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	plane1_mult->SetDirectory(rootoutfile);
// 	TH2F *plane2_mult = new TH2F("plane2_mult","plane 2 multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	plane2_mult->SetDirectory(rootoutfile);
// 	TH2F *plane3_mult = new TH2F("plane3_mult","plane 3 multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	plane3_mult->SetDirectory(rootoutfile);
// 	TH2F *tracks_mult = new TH2F("tracks_mult","tracks multiplicity",40,0,40,type_nbins,type_axismin,type_axismax); 
// 	tracks_mult->SetDirectory(rootoutfile);

// 	TH2F *plane12_offset = new TH2F("plane12_offset","plane 1 - plane 2 offset",9,-4.5,4.5,type_nbins,type_axismin,type_axismax); 
// 	plane12_offset->SetDirectory(rootoutfile);
// 	TH2F *plane23_offset = new TH2F("plane23_offset","plane 2 - plane 3 offset",9,-4.5,4.5,type_nbins,type_axismin,type_axismax); 
// 	plane23_offset->SetDirectory(rootoutfile);
// 	TH2F *plane31_offset = new TH2F("plane31_offset","plane 3 - plane 1 offset",9,-4.5,4.5,type_nbins,type_axismin,type_axismax); 
// 	plane31_offset->SetDirectory(rootoutfile);


	const Int_t numwaveforms = 36;
	TGraph *graphslaserON[numwaveforms];
	TGraph *graphslaserONtimingcut[numwaveforms];
	TGraph *graphslaserOFF[numwaveforms];
	TGraph *graphsbeamOFF[numwaveforms];
	TGraph *graphsbig[numwaveforms];
	Int_t num_graphslaserON=0, num_graphslaserOFF=0, num_graphsbeamOFF=0, num_graphsbig=0;
	for(Int_t i=0; i<numwaveforms; i++) {
		graphslaserON[i]=NULL;
		graphsbig[i]=NULL;
		graphslaserONtimingcut[i]=NULL;
		graphslaserOFF[i]=NULL;
		graphsbeamOFF[i]=NULL;
	}

	// **********
	// **** Define all the variables for the loop
	std::string mytext = "";
	Double_t eventtime;
	//Int_t warningnum_accum=0;
	Int_t num_no_samples=0;
	Int_t num_badsnaps=0, num_badhelicity=0, num_badbcm=0, num_badmaxpos=0;
	//Int_t numsamp=0, numsnapshots=0;
	Int_t num_beamon_laseron=0, num_beamon_laseroff=0, num_beamon_laserbad=0, num_beamoff=0, num_beamramp=0;
	Bool_t beamon, beamoff, laseron, laseroff, printsample, drawsnapshot, screwedup, maxposgood, no_pileup;
	Int_t paircount=0, quadcount=0, octoquadcount=0;
	Double_t pairsum=0, pairdiff=0, /*pairasym=0,*/ quaddiff=0, octoquaddiff=0;
	Int_t type = 0;
	Double_t samp_min_val, samp_max_val;
	Int_t samp_minpos_val, samp_maxpos_val;
	Double_t samp_integral_val;
	Double_t samp_ped1, samp_ped2, samp_ped3, samp_ped4, samp_ped5, xgpos;
	Double_t samp_pedave_val, samp_pedprepeakave_val;
	Double_t samp_ped_maxm6, samp_ped_maxm7, samp_ped_maxm8, samp_ped_maxm9, samp_ped_maxm10;
	Double_t samp_peakintegral_val, samp_pointval, samp_pointer;
	Double_t cutbcmval;
	Bool_t goodsample;

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
		cutbcmval = bcm1[0];
		if (cutbcmval==0) { // Must have a good BCM value to be useful 
			num_badbcm++;
		} else  {
			if (jentry==1) {
				printf("%s %s %s %s %s %s %s %s %s %s %s %s\n","    jentry ","  laser", " bcm1", "  type",
					   "      max ", "     maxpos", "   min ", "  minpos", "  pedave", 
					   " samp_int", "  accum0", "    comment");
			}
			eventtime = jentry/960.;
			// **********
			// **** Set the switches for the event
			laseron = sca_laser_PowT[0] > laseroncut;
			laseroff = sca_laser_PowT[0] < laseroffcut;
			beamoff = cutbcmval < current_beamoff;
			beamon = cutbcmval > current_beamon;
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
			if (!beamon && !beamoff) {
				type=4;
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
			case 4:
				num_beamramp++;
				break; 
			}

			if (actual_helicity==0) {      
			} else {
				if (actual_helicity==1) {
				} else {
					num_badhelicity++;
					if (num_badhelicity%1000 == 0) {
						printf("Event %i is %i with bad helicity: %f\n",jentry,num_badhelicity,actual_helicity);
					}
				}
			}    
			lasertime->Fill(eventtime,sca_laser_PowT[0],type);
			bcmtime->Fill(eventtime,cutbcmval,type);
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
				goodsample = 1;
				if (!goodsample) {
					printf("cutting one\n");
				} else {
					samp_max_val = sampled_events->at(0).GetMaxSample();
					samp_min_val = sampled_events->at(0).GetMinSample();
					samp_maxpos_val = sampled_events->at(0).GetMaxIndex();
					samp_minpos_val = sampled_events->at(0).GetMinIndex();
					// samp_pedestal;// = sampled_events->at(0).GetPedestal();
					samp_integral_val = sampled_events->at(0).GetSum();
					samp_pointer = sampled_events->at(0).GetSamplePointer();
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

					if (jentry%20000 == 0) printsample=1;
					if (printsample) {
						printf("%8i %3i%% %8.0f %6.1f  %i,%i,%i,%i %10.2f %6i %6.0f %6i %6.0f %10.2f %10.4g %s\n",
							   jentry, 100*jentry/nentries, sca_laser_PowT[0], cutbcmval, laseron, laseroff, beamon, type,
							   samp_max_val, samp_maxpos_val, samp_min_val, samp_minpos_val, samp_pedave_val, samp_integral_val, 
							   fadc_compton_accum0[0], mytext.c_str());
						//       printf("%8i %3i%%\n",
						// 	     jentry, 100*jentry/nentries);
						printsample=0;
					}

					// this makes no sense, samp_min is a pointer to a histogram (wdc)
					//if (samp_min < -60000) {  // They can't be the overflow kind
					//	mytext = "screwed up";
					//	screwedup = 1;
					//	num_badsnaps++;
					//}
					if(samp_maxpos_val > maxpeakposcut_min && samp_maxpos_val < maxpeakposcut_max) {
						maxposgood=1;
					}
					if (!screwedup) {
						snaphist->Fill(eventtime);
						if (num_graphslaserON<numwaveforms && type == 0) {
							sprintf(samptitle,"Entry %i",jentry);
							sprintf(sampname,"laserON%02i",num_graphslaserON);
							graphslaserON[num_graphslaserON] = sampled_events->at(0).GetGraph();
							graphslaserON[num_graphslaserON]->SetTitle(samptitle);
							graphslaserON[num_graphslaserON]->SetName(sampname);
							//graphslaserON[num_graphslaserON]->SetDirectory(rootoutfile);
							//printf("Writing shapshot  %s  %s\n",sampname,samptitle);
							rootoutfile->cd();
							graphslaserON[num_graphslaserON]->Write();
							num_graphslaserON++;
						}
// 					if (num_graphslaserONtimingcut<numwaveforms && type == 0) {
// 						sprintf(samptitle,"Entry %i",jentry);
// 						sprintf(sampname,"laserONtimed%02i",num_graphslaserONtimingcut);
// 						graphslaserONtimingcut[num_graphslaserONtimingcut] = sampled_events->at(0).GetGraph();
// 						graphslaserONtimingcut[num_graphslaserONtimingcut]->SetTitle(samptitle);
// 						graphslaserONtimingcut[num_graphslaserONtimingcut]->SetName(sampname);
// 						//graphslaserON[num_graphslaserON]->SetDirectory(rootoutfile);
// 						//printf("Writing shapshot  %s  %s\n",sampname,samptitle);
// 						rootoutfile->cd();
// 						graphslaserONtimingcut[num_graphslaserONtimingcut]->Write();
// 						num_graphslaserONtimingcut++;
// 					}
						
						if (num_graphsbig<numwaveforms && samp_max_val>bigsnapmin) {
							sprintf(samptitle,"Entry %i",jentry);
							sprintf(sampname,"big%02i",num_graphsbig);
							graphsbig[num_graphsbig] = sampled_events->at(0).GetGraph();
							graphsbig[num_graphsbig]->SetTitle(samptitle);
							graphsbig[num_graphsbig]->SetName(sampname);
							//graphsbig[num_graphsbig]->SetDirectory(rootoutfile);
							//printf("Writing shapshot  %s  %s\n",sampname,samptitle);
							rootoutfile->cd();
							graphsbig[num_graphsbig]->Write();
							num_graphsbig++;
						}
						if (num_graphslaserOFF<numwaveforms && type == 1) {
							sprintf(samptitle,"Entry %i",jentry);
							sprintf(sampname,"laserOFF%02i",num_graphslaserOFF);
							graphslaserOFF[num_graphslaserOFF] = sampled_events->at(0).GetGraph();
							graphslaserOFF[num_graphslaserOFF]->SetTitle(samptitle);
							graphslaserOFF[num_graphslaserOFF]->SetName(sampname);
							//graphslaserOFF[num_graphslaserOFF]->SetDirectory(rootoutfile);
							//printf("Writing shapshot  %s  %s\n",sampname,samptitle);
							rootoutfile->cd();
							graphslaserOFF[num_graphslaserOFF]->Write();
							num_graphslaserOFF++;
						}
						if (num_graphsbeamOFF<numwaveforms && type == 2) {
							sprintf(samptitle,"Entry %i",jentry);
							sprintf(sampname,"beamOFF%02i",num_graphsbeamOFF);
							graphsbeamOFF[num_graphsbeamOFF] = sampled_events->at(0).GetGraph();
							graphsbeamOFF[num_graphsbeamOFF]->SetTitle(samptitle);
							graphsbeamOFF[num_graphsbeamOFF]->SetName(sampname);
							//graphsbeamOFF[num_graphsbeamOFF]->SetDirectory(rootoutfile);
							//printf("Writing shapshot   %s  %s\n",sampname,samptitle);
							rootoutfile->cd();
							graphsbeamOFF[num_graphsbeamOFF]->Write();
							num_graphsbeamOFF++;
						}
						samp_intVSmax->Fill(samp_max_val,samp_integral_val,type);
						samp_integral->Fill(samp_integral_val,type);
						samp_max->Fill(samp_max_val,type);
						samp_maxpos->Fill(samp_maxpos_val,type);
						samp_maxVSmaxpos->Fill(samp_maxpos_val,samp_max_val,type);
						samp_min->Fill(samp_min_val,type);
						samp_minpos->Fill(samp_minpos_val,type);
						samp_minVSminpos->Fill(samp_minpos_val,samp_min_val,type); 
						// intergrate over cut window
						for (Int_t i=maxpeakposcut_min; i<=maxpeakposcut_max; i++) {
							thegraph->GetPoint(i,xgpos,samp_pointval);
							samp_peakintegral_val += samp_pointval;
							//printf("%i  %.0f  %.0f,  ",i,samp_pointval,samp_peakintegral_val);
						}
						samp_timing_integral->Fill(samp_peakintegral_val,type);
						samp_timingintVSmax->Fill(samp_max_val,samp_peakintegral_val,type);
						samp_peakintegral_val = 0;
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
							samp_subintVSmax->Fill(samp_max_val,samp_peakintegral_val,type);
							laser->Fill(sca_laser_PowT[0],type);
							samp_bcm1->Fill(bcm1[0],type);
							samp_bcm2->Fill(bcm2[0],type);
							samp_bcm6->Fill(bcm6[0],type); 
							if (actual_helicity==0) {
								samp_max_hel0->Fill(samp_max_val,type);
							// (jc2) : Had to add an extra parameter, as it seems
							// ROOT changed the API of TH3 to make Fill(double,double)
							// be protected now.

								samp_yieldVSsubint_hel0->Fill(samp_integral_val,fadc_compton_accum0[0],1.0);
								samp_bcm1_hel0->Fill(bcm1[0]);
								samp_bcm2_hel0->Fill(bcm2[0]);
								samp_bcm6_hel0->Fill(bcm6[0]); 
							} else {
								if (actual_helicity==1) {
									samp_max_hel1->Fill(samp_max_val,type);
									// (jc2) : Had to add an extra parameter, as it seems
									// ROOT changed the API of TH3 to make Fill(double,double)
                  							// be protected now.
									samp_yieldVSsubint_hel1->Fill(samp_integral_val,fadc_compton_accum0[0],1.0);
									samp_bcm1_hel1->Fill(bcm1[0]);
									samp_bcm2_hel1->Fill(bcm2[0]);
									samp_bcm6_hel1->Fill(bcm6[0]);
								}
							}
							samp_pedVSyield->Fill(fadc_compton_accum0[0],minped,type);
//						samp_pedVSyield->Fill(fadc_compton_accum0[0],minpedsamp_pedprepeakave_val,type);
							samp_pedVSmin->Fill(samp_min_val,minped,type);
							samp_pedVStime->Fill(eventtime,minped,type);
						}
					}
				} // endif goodsample
			}
		}
	}
	cputime = timer.CpuTime();
	realtime = timer.RealTime();
	
	//TString logdir = TString(getenv("QWSCRATCH")) + Form("/www/run_%i/",runnum());
  TString logdir = GetStorageDir()+"/";

	TString logfilename = logdir + "stepthru_mps_time.log";
	std::cout << "Writing time log to " << logfilename << std::endl;
	time_t rawtime;
	time ( &rawtime );
	FILE* logfile = 0;
	if (!(logfile = fopen(logfilename,"a"))) {
		std::cout << "Could not open file " << logfilename << " for writing." << std::endl;
		return;
	}
	fprintf(logfile," Analysis at took %8.2f s \t(%8.2f s of cpu), %i entries, done on %s",
			realtime,cputime,nentries,ctime(&rawtime));
	fclose(logfile);

	logfilename = logdir + "stepthru_mps.log";
	std::cout << "Writing mps log to " << logfilename << std::endl;
	if (!(logfile = fopen(logfilename,"w"))) {
		std::cout << "Could not open file " << logfilename << " for writing." << std::endl;
		return;
	}
	fprintf(logfile," Total events:                %8i     \n", nentries);
	fprintf(logfile," Snaps, full beam and laser:  %8i     \n", num_beamon_laseron);
	fprintf(logfile," Snaps, full beam no laser:   %8i     \n", num_beamon_laseroff);
	fprintf(logfile," Snaps, full beam some laser: %8i     \n", num_beamon_laserbad);
	fprintf(logfile," Snaps, beam ramp:            %8i     \n", num_beamramp);
	fprintf(logfile," Snaps, no beam:              %8i     \n", num_beamoff);
	fprintf(logfile," No snapshots:                %8i     \n", num_no_samples);
	fprintf(logfile," Bad snapshots:               %8i     \n", num_badsnaps);
	fprintf(logfile," Bad helicity:                %8i     \n", num_badhelicity);
	fprintf(logfile," Bad BCM:                     %8i     \n", num_badbcm);
	fprintf(logfile," Bad Max position:            %8i     \n", num_badmaxpos);
	fclose(logfile);



	// *************************** 
	// ***** Plot MPS summary histos
	// ***************************
	Bool_t do_mps = kTRUE;
	Bool_t do_samples_correl = kTRUE;
	Bool_t do_samples_pedestal = kTRUE;
	Bool_t do_samples_pedcorr = kTRUE;
	Bool_t do_pedcorrection = kTRUE;
	Bool_t do_samples = kTRUE;
	Bool_t do_samples_asymmetry = kTRUE;
	Bool_t do_samples_responsefunc = kTRUE;
	Bool_t do_waveforms = kTRUE;
	Bool_t do_maxandint = kTRUE;

	std::cout << "do_mps: " << do_mps << std::endl;
	std::cout << "do_samples_correl " << do_samples_correl << std::endl;
	std::cout << "do_samples_pedestal " << do_samples_pedestal << std::endl;
	std::cout << "do_samples_pedcorr " << do_samples_pedcorr << std::endl;
	std::cout << "do_pedcorrection " << do_pedcorrection << std::endl;
	std::cout << "do_samples " << do_samples << std::endl;
	std::cout << "do_samples_asymmetry " << do_samples_asymmetry << std::endl;
	std::cout << "do_samples_responsefunc " << do_samples_responsefunc << std::endl;
	std::cout << "do_waveforms " << do_waveforms << std::endl;
	std::cout << "do_maxandint " << do_maxandint << std::endl;
	
	TString outtextfilename_ped;
	TString outtextfilename_width;
	//TString webdir = kWebDirectory+ Form("/run_%i/",runnum());
  TString webdir = GetStorageDir();
	//gSystem->mkdir(webdir.Data(),kTRUE);
	outtextfilename_ped = webdir + "pedestalfit.txt";

	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);


	//Int_t yieldrebinnum = 10;


	// *************************** 
	// ***** Set up for the canvas
	// *************************** 
	Int_t canvaswidth,canvasheight,canvasxoff,xpads,ypads;
	if (gROOT->IsBatch()) {
		canvasxoff=0;  canvaswidth=1000;  canvasheight=800; xpads=3; ypads=6; // Batch mode
	} else {
		canvasxoff=100;  canvaswidth=1000;  canvasheight=800; xpads=6; ypads=3; // Interactive vnc
		//canvasxoff=200;  canvaswidth=1000;  canvasheight=800; xpads=3; ypads=6; // Interactive ccomptl1
	}
	if (0) {
		printf("\nKill me now!!!!!\n");
		sleep(10);
	}

	TLatex text;
	text.SetNDC(kTRUE);
	text.SetTextFont(42);
	text.SetTextAlign(22);
	text.SetTextSize(0.04);
	Float_t textspace=0.06;

// 	gStyle->SetPadRightMargin(0.02);
// 	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.06,"hxyz");
	gStyle->SetTitleSize(0.06,"hxyz");
	//gStyle->SetTitleXOffset(0.7);

	TCanvas *waveforms_laserON;
	TPad *c_waveforms_laserON;
	TVirtualPad *padp;
	if (do_waveforms) {
		waveforms_laserON = new TCanvas("waveforms_laserON", Form("waveforms %i",runnum()), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_laserON->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 7: Laser ON snapshots", runnum()));
		c_waveforms_laserON = new TPad("c_waveforms_laserON","c_waveforms_laserON",0,0,1,1.0-textspace);
		c_waveforms_laserON->Draw();
		c_waveforms_laserON->Divide(6,6,0.001,0.001);
		for (Int_t i=1; i<=numwaveforms; i++) {
			padp = c_waveforms_laserON->cd(i);
			TGraph *graph1 = (TGraph*) gDirectory->Get(Form("laserON%02i",i-1));
			if (graph1) {
				graph1->Draw("al");
				padp->SetMargin(0.15,0.02,0.1,0.1);
				graph1->GetXaxis()->SetLabelSize(0.07);
				graph1->GetYaxis()->SetLabelSize(0.07);
				graph1->GetXaxis()->SetTitleSize(0.07);
				graph1->GetYaxis()->SetTitleSize(0.07);
				//graph1->SetTitleSize(0.07);
			}
		}
		//waveforms_laserON->Print(webdir + "waveforms_laserON.png");
    SaveToWeb(waveforms_laserON,"waveforms_laserON");
	}

	TCanvas *waveforms_big;
	TPad *c_waveforms_big;
	if (do_waveforms) {
		waveforms_big = new TCanvas("waveforms_big", Form("waveforms %i",runnum()), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_big->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 10: BIG snapshots (max > 3000)", runnum()));
		c_waveforms_big = new TPad("c_waveforms_big","c_waveforms_big",0,0,1,1.0-textspace);
		c_waveforms_big->Draw();
		c_waveforms_big->Divide(6,6,0.001,0.001);
		for (Int_t i=1; i<=numwaveforms; i++) {
			padp = c_waveforms_big->cd(i);
			TGraph *graph1 = (TGraph*) gDirectory->Get(Form("big%02i",i-1));
			if (graph1) {
				graph1->Draw("al");
				padp->SetMargin(0.15,0.02,0.1,0.1);
				graph1->GetXaxis()->SetLabelSize(0.07);
				graph1->GetYaxis()->SetLabelSize(0.07);
				graph1->GetXaxis()->SetTitleSize(0.07);
				graph1->GetYaxis()->SetTitleSize(0.07);
				//graph1->SetTitleSize(0.07);
			}
		}
		//waveforms_big->Print(webdir + "waveforms_big.png");
    SaveToWeb(waveforms_big,"waveforms_big");
	}

	TCanvas *waveforms_laserOFF;
	TPad *c_waveforms_laserOFF;    
	if (do_waveforms) {
		waveforms_laserOFF = new TCanvas("waveforms_laserOFF", Form("waveforms %i",runnum()), 
										 canvasxoff,0,canvaswidth,canvasheight);
		waveforms_laserOFF->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 8: Laser OFF snapshots", runnum()));
		c_waveforms_laserOFF = new TPad("c_waveforms_laserOFF","c_waveforms_laserOFF",0,0,1,1.0-textspace);
		c_waveforms_laserOFF->Draw();
		c_waveforms_laserOFF->Divide(6,6,0.001,0.001);
		for (Int_t i=1; i<=numwaveforms; i++) {
			padp = c_waveforms_laserOFF->cd(i);
			TGraph *graph2 = (TGraph*) gDirectory->Get(Form("laserOFF%02i",i-1));
			if (graph2) {
				//graph2->SetTitleSize(0.07);
				graph2->Draw("al");
				graph2->GetXaxis()->SetLabelSize(0.07);
				graph2->GetYaxis()->SetLabelSize(0.07);
				graph2->GetXaxis()->SetTitleSize(0.07);
				graph2->GetYaxis()->SetTitleSize(0.07);
				//graph2->SetTitleSize(0.07);
				padp->SetMargin(0.15,0.02,0.1,0.1);
			}
		}
		//waveforms_laserOFF->Print(webdir + "waveforms_laserOFF.png");
    SaveToWeb(waveforms_laserOFF,"waveforms_laserOFF");
	}

	TCanvas *waveforms_beamOFF;
	TPad *c_waveforms_beamOFF;    
	if (do_waveforms) {
		waveforms_beamOFF = new TCanvas("waveforms_beamOFF", Form("waveforms %i",runnum()), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_beamOFF->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 9: Beam OFF snapshots", runnum()));
		c_waveforms_beamOFF = new TPad("c_waveforms_beamOFF","c_waveforms_beamOFF",0,0,1,1.0-textspace);
		c_waveforms_beamOFF->Draw();
		c_waveforms_beamOFF->Divide(6,6,0.001,0.001);
		for (Int_t i=1; i<=numwaveforms; i++) {
			padp = c_waveforms_beamOFF->cd(i);
			TGraph *graph3 = (TGraph*) gDirectory->Get(Form("beamOFF%02i",i-1));
			if (graph3) {
				graph3->Draw("al");
				graph3->GetXaxis()->SetLabelSize(0.07);
				graph3->GetYaxis()->SetLabelSize(0.07);
				graph3->GetXaxis()->SetTitleSize(0.07);
				graph3->GetYaxis()->SetTitleSize(0.07);
				//graph3->SetTitleSize(0.07);
				padp->SetMargin(0.15,0.02,0.1,0.1);
			}
		}
		//waveforms_beamOFF->Print(webdir + "waveforms_beamOFF.png");
    SaveToWeb(waveforms_beamOFF,"waveforms_beamOFF");
	}



	TCanvas *mps;
	TPad *c_mps;
	if (do_mps) {
		mps = new TCanvas("mps", Form("mps %i",runnum()),canvasxoff,0,canvaswidth,canvasheight);
		mps->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 1: MPS tree general",runnum()));
		c_mps = new TPad("c_mps","c_mps",0,0,1,1.0-textspace);
		c_mps->Draw();
		c_mps->Divide(1,5,0.001,0.001);
		c_mps->cd(1);
		projectALLtypeTH1(lasertime,0,"",0,1,1);
		c_mps->cd(2);
		projectALLtypeTH1(bcmtime,0,"",0,1,1);
		c_mps->cd(3);
		projectALLtypeTH1(accum0time,0,"",0,1,1);
		c_mps->cd(4);
		projectALLtypeTH1(samp_pedVStime,0,"",0,1,1);
		c_mps->cd(5);
		snaphist->SetMarkerStyle(20);
		snaphist->SetMarkerSize(0.2);
		snaphist->GetXaxis()->SetLabelSize(0.08);
		snaphist->GetYaxis()->SetLabelSize(0.08);
		snaphist->GetXaxis()->SetTitleSize(0.08);
		snaphist->GetYaxis()->SetTitleSize(0.08);
		snaphist->GetXaxis()->SetTitleOffset(0.6);
		snaphist->GetYaxis()->SetTitleOffset(0.6);
		snaphist->GetXaxis()->CenterTitle();
		snaphist->DrawCopy();

		//cerr << "time 3\n" << endl;
		gPad->SetMargin(0.06,0.02,0.1,0.1);
		//mps->Print(webdir + "mps_summary.png");
    SaveToWeb(mps,"mps_summary");
	}

	Bool_t dobeamoffsamps = num_beamon_laseron + num_beamon_laseroff < num_beamoff;

 	TCanvas *samples_correl;
 	TPad *c_samples_correl;
	if (do_samples_correl) {
		samples_correl = new TCanvas("samples_correl", Form("samples_correl %i",runnum()),
									 canvasxoff,0,canvaswidth,canvasheight*5./3);
		samples_correl->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 2: Snapshot general",runnum()));
		c_samples_correl = new TPad("c_samples_correl","c_samples_correl",0,0,1,1.0-textspace);
		c_samples_correl->Draw();
		c_samples_correl->Divide(3,5,0.001,0.001);

		c_samples_correl->cd(1);
		if (dobeamoffsamps) {
			drawALLtypeTH1(samp_max,0,"",1);
		} else {
			drawscalesubbytypeTH1(samp_max, 1, 2, maxnormintminchan, maxnormintmaxchan, 0, 1, 1, maxcomptonedge);
		}
		c_samples_correl->cd(2);
		drawALLtypeTH1(samp_maxpos,0,"max position (timing cut)",1);
		TLine t1;
		t1.SetLineColor(kRed);
		Double_t linexmin,lineymin,linexmax,lineymax;
//		gPad->GetPadPar(linexmin,lineymin,linexmax,lineymax);
//		gPad->GetRangeAxis(linexmin,lineymin,linexmax,lineymax);
		lineymin = 0.1;
		lineymax = 0.9;
		linexmin=0.1+0.8*maxpeakposcut_min/sampxpos_nbins;
		linexmax=0.1+0.8*maxpeakposcut_max/sampxpos_nbins;
		t1.DrawLineNDC(linexmin,lineymin,linexmin,lineymax);
		t1.DrawLineNDC(linexmax,lineymin,linexmax,lineymax);
		printf("lines %f  %f  %f  %f  %i  %i\n",
			   linexmin,lineymin,linexmax,lineymax,maxpeakposcut_min,maxpeakposcut_max);
		c_samples_correl->cd(3);
		drawALLtypeTH1(samp_min,0,"",1);

		c_samples_correl->cd(4);
		if (dobeamoffsamps) {
			drawALLtypeTH1(samp_integral,0,"",1);
		} else {
			drawscalesubbytypeTH1(samp_subintegral, 1, 2, intnormintminchan, intnormintmaxchan, 0, 1, 1, intcomptonedge);
		}
		c_samples_correl->cd(5);
		draw1typeTH2(samp_intVSmax,-1,0,"",1,5,10);

		c_samples_correl->cd(6);
		drawALLtypeTH1(samp_minpos,0,"",1);

 		c_samples_correl->cd(7);
// 		snap_average->Divide(snap_average_norm);
// 		drawALLtypeTH1(snap_average);
		outtextfilename_width = webdir + "ave_accum0.txt";
		drawALLtypeTH1(accum0,1,"",1,outtextfilename_width);
 		c_samples_correl->cd(8);
		drawALLtypeTH1(laser,1,"",1,outtextfilename_width);
		c_samples_correl->cd(9);
		outtextfilename_width = webdir + "pedestal_ave.txt";
		drawALLtypeTH1(samp_pedprepeakave,1,"",1,outtextfilename_width);
//		drawALLtypeTH1(samp_pedave,1,"",1);


 		c_samples_correl->cd(10);
		drawALLtypeTH1(samp_bcm1,1,"",1);
 		c_samples_correl->cd(11);
		drawALLtypeTH1(samp_bcm2,1,"",1);
		c_samples_correl->cd(12);
		drawALLtypeTH1(samp_bcm6,1,"",1);

		c_samples_correl->cd(13);
		outtextfilename_width = webdir + "width_pairdiff.txt";
		drawALLtypeTH1(accum0_diff_pair,1,"",1,outtextfilename_width); 
		c_samples_correl->cd(14);
		outtextfilename_width = webdir + "width_quaddiff.txt";
		drawALLtypeTH1(accum0_diff_quad,1,"",1,outtextfilename_width);
		c_samples_correl->cd(15);
		outtextfilename_width = webdir + "width_32diff.txt";
		drawALLtypeTH1(accum0_diff_32,1,"",1,outtextfilename_width);

		//samples_correl->Print(webdir + "samples_correl.png");
    SaveToWeb(samples_correl,"samples_correl");
	}


// 	TCanvas *samples_correl;
// 	TPad *c_samples_correl;
// 	if (do_samples_correl) {
// 		samples_correl = new TCanvas("samples_correl", Form("samples_correl %i",runnum()),
// 									 canvasxoff,0,canvaswidth,canvasheight);
// 		samples_correl->cd();
// 		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 2: Snapshot general",runnum()));
// 		c_samples_correl = new TPad("c_samples_correl","c_samples_correl",0,0,1,1.0-textspace);
// 		c_samples_correl->Draw();
// 		c_samples_correl->Divide(3,3,0.001,0.001);
// 		c_samples_correl->cd(1);
// 		drawscalesubbytypeTH1(samp_max, 1, 2, maxmin, maxmax, 1);
// 		c_samples_correl->cd(2);
// 		drawALLtypeTH1(samp_min,0,"",1);
// 		c_samples_correl->cd(3);
// 		drawALLtypeTH1(samp_pedave,1,"",1);
// 		c_samples_correl->cd(4);
// 		drawALLtypeTH1(samp_maxpos,0,"",1);
// 		c_samples_correl->cd(5);
// 		drawALLtypeTH1(samp_minpos,0,"",1);
// 		c_samples_correl->cd(6);
// 		drawALLtypeTH1(samp_pedprepeakave,1,"",1);
// 		c_samples_correl->cd(7);
// 		outtextfilename_width = webdir + "width_pairdiff.txt";
// 		drawALLtypeTH1(accum0_diff_pair,1,"",1,outtextfilename_width.Data());
// 		c_samples_correl->cd(8);
// 		outtextfilename_width = webdir + "width_quaddiff.txt";
// 		drawALLtypeTH1(accum0_diff_quad,1,"",1,outtextfilename_width.Data());
// 		c_samples_correl->cd(9);
// 		outtextfilename_width = webdir + "width_32diff.txt";
// 		drawALLtypeTH1(accum0_diff_32,1,"",1,outtextfilename_width.Data());
// 		//drawALLtypeTH1(accum0_asym_32);
// 		//draw3TH1gen(samp_bcm6,samp_bcm1,samp_bcm2,1,"bcm 6","bcm 1","bcm 2",kRed,kGreen,kBlue,"bcms");
// 		//drawALLtypeTH1(accum0,1,"",1);
// 		//drawscalesub1D(samp_laserOFF_max, samp_laserON_max, 160, 280, 0);
// 		samples_correl->Print(webdir + "samples_correl.png");
// 	}

	TCanvas *samples_pedestal;
	TPad *c_samples_pedestal;
	if (do_samples_pedestal) {
		samples_pedestal = new TCanvas("samples_pedestal",
									   Form("samples_pedestal %i",runnum()),
									   canvasxoff,0,canvaswidth,canvasheight);
		samples_pedestal->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 3: Snapshot timing",runnum()));
		c_samples_pedestal = new TPad("c_samples_pedestal","c_samples_pedestal",0,0,1,1.0-textspace);
		c_samples_pedestal->Draw();
		c_samples_pedestal->Divide(3,3,0.001,0.001);
		c_samples_pedestal->cd(1);
		draw1typeTH2(samp_maxVSmaxpos,1,0,"Laser ON",1,2,10);
		c_samples_pedestal->cd(2);
		draw1typeTH2(samp_maxVSmaxpos,2,0,"Laser OFF",1,2,10);
		c_samples_pedestal->cd(3);
		draw1typeTH2(samp_maxVSmaxpos,3,0,"Beam OFF",1,2,10);
//		c_samples_pedestal->cd(4);
//		draw1typeTH2(samp_maxVSmaxpos,4,0,"laser flash");

		c_samples_pedestal->cd(4);
		draw1typeTH2(samp_minVSminpos,1,0,"Laser ON",1,2,3);
		c_samples_pedestal->cd(5);
		draw1typeTH2(samp_minVSminpos,2,0,"Laser OFF",1,2,3);
		c_samples_pedestal->cd(6);
		draw1typeTH2(samp_minVSminpos,3,0,"Beam OFF",1,2,3);
// 		c_samples_pedestal->cd(8);
// 		draw1typeTH2(samp_minVSminpos,4,0,"laser flash");

		c_samples_pedestal->cd(7);
		draw1typeTH2(samp_pedVStime,1,0,"Laser ON");
		c_samples_pedestal->cd(8);
		draw1typeTH2(samp_pedVStime,2,0,"Laser OFF");
		c_samples_pedestal->cd(9);
		draw1typeTH2(samp_pedVStime,3,0,"Beam OFF");
// 		c_samples_pedestal->cd(12);
// 		draw1typeTH2(samp_pedVStime,4,0,"laser flash");

// 		c_samples_pedestal->cd(13);
// 		draw1typeTH2(samp_pedVSpedpp,1,0,"Laser ON");
// 		c_samples_pedestal->cd(14);
// 		draw1typeTH2(samp_pedVSpedpp,2,0,"Laser OFF");
// 		c_samples_pedestal->cd(15);
// 		draw1typeTH2(samp_pedVSpedpp,3,0,"Beam OFF");
// 		c_samples_pedestal->cd(16);
// 		draw1typeTH2(samp_pedVSpedpp,4,0,"laser flash");

		//samples_pedestal->Print(webdir + "samples_ped.png");
    SaveToWeb(samples_pedestal,"samples_ped");
	}

	TCanvas *samples_pedcorr;
	TPad *c_samples_pedcorr;
	if (do_samples_pedcorr) {
		samples_pedcorr = new TCanvas("samples_pedcorr", Form("samples_pedcorr %i",runnum()),
									  canvasxoff,0,canvaswidth,canvasheight);
		samples_pedcorr->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 4: Pedestal correction analysis",runnum()));
		c_samples_pedcorr = new TPad("c_samples_pedcorr","c_samples_pedcorr",0,0,1,1.0-textspace);
		c_samples_pedcorr->Draw();
		c_samples_pedcorr->Divide(3,3,0.001,0.001);
		c_samples_pedcorr->cd(1);
		draw1typeTH2(samp_pedVSmin,1,0,"Laser ON");
		c_samples_pedcorr->cd(2);
		draw1typeTH2(samp_pedVSmin,2,0,"Laser OFF");
		c_samples_pedcorr->cd(3);
		draw1typeTH2(samp_pedVSmin,3,0,"Beam OFF");
		c_samples_pedcorr->cd(4);
		draw1typeTH2(samp_pedVSyield,1,0,"Laser ON");
		c_samples_pedcorr->cd(5);
		draw1typeTH2(samp_pedVSyield,2,0,"Laser OFF");
		c_samples_pedcorr->cd(6);
		draw1typeTH2(samp_pedVSyield,3,0,"Beam OFF");
 		c_samples_pedcorr->cd(7);
		projectALLtypeTH1(samp_pedVSyield);
 		c_samples_pedcorr->cd(8);
		projectALLtypeTH1andfit(samp_pedVSyield,1,"",0,10,outtextfilename_ped.Data());  //rebin here
		//samples_pedcorr->Print(webdir + "samples_pedcorr.png");
    SaveToWeb(samples_pedcorr,"samples_pedcorr");
	}

/*
	// ******************
	// **** Here correct for pedestal shift
	// ******************
	TH2D* samp_yieldVSsubint_type[numtypes];
	TH3F *samp_yieldVSsubint_pedcorr = 0;
	if (do_pedcorrection) {
		samp_yieldVSsubint_pedcorr = (TH3F*) samp_yieldVSsubint->Clone("samp_yieldVSsubint_pedcorr");
		samp_yieldVSsubint_pedcorr->Reset();
		samp_yieldVSsubint_pedcorr->SetTitle("above, pedestal corrected");
// 		TH2F *samp_laserON_yieldVSsubint_hel0_pedcorr = samp_laserON_yieldVSsubint_hel0
// 			->Clone("samp_laserON_yieldVSsubint_hel0_pedcorr");
// 		samp_laserON_yieldVSsubint_hel0_pedcorr->Reset();
// 		samp_laserON_yieldVSsubint_hel0_pedcorr->SetTitle("hel 0, pedestal corrected");
// 		TH2F *samp_laserON_yieldVSsubint_hel1_pedcorr = samp_laserON_yieldVSsubint_hel1
// 			->Clone("samp_laserON_yieldVSsubint_hel1_pedcorr");
// 		samp_laserON_yieldVSsubint_hel1_pedcorr->Reset();
// 		samp_laserON_yieldVSsubint_hel1_pedcorr->SetTitle("hel 1, pedestal corrected");

		timer.Start(kTRUE);

		Double_t entries[numtypes];
		Double_t binCenter, binWidth, bincont, binerr;
		//Int_t xbins = samp_yieldVSsubint->GetNbinsX();
		//Int_t ybins = samp_yieldVSsubint->GetNbinsY();
		TAxis *xaxis = samp_yieldVSsubint->GetXaxis();
		binWidth = xaxis->GetBinWidth(1);

		Double_t laseron_ped, laseron_pedcorr;
		//Double_t laseroff_ped, laseroff_pedcorr, beamoff_ped, beamoff_pedcorr;
		Int_t laseron_bincorr;
		//Int_t laseroff_bincorr, beamoff_bincorr;

		for (Int_t typestep = 1; typestep <= numtypes; typestep++) {
			// **** first decide if we should analyse this type
			samp_yieldVSsubint->GetZaxis()->SetRange(typestep,typestep);
			samp_yieldVSsubint_type[typestep-1] = (TH2D*)samp_yieldVSsubint->Project3D(Form("yx%i",typestep));
			entries[typestep-1] = samp_yieldVSsubint_type[typestep-1]->GetEntries();
			printf("type %i has %f events\n",typestep,entries[typestep-1]);
			if (entries[typestep-1]>0) {
				Int_t xbinmin = samp_yieldVSsubint_type[typestep-1]->FindFirstBinAbove(0, 1);
				Int_t xbinmax = samp_yieldVSsubint_type[typestep-1]->FindLastBinAbove(0, 1);
				Int_t ybinmin = samp_yieldVSsubint_type[typestep-1]->FindFirstBinAbove(0, 2);
				Int_t ybinmax = samp_yieldVSsubint_type[typestep-1]->FindLastBinAbove(0, 2);
				printf("Working in a box of %i bins from %i to %i and %i to %i\n",
					   (xbinmax-xbinmin)*(ybinmax-ybinmin),xbinmin, xbinmax, ybinmin, ybinmax);
				TAxis *yaxis = samp_yieldVSsubint_type[typestep-1]->GetYaxis();
				for (Int_t ybincount = ybinmin; ybincount <= ybinmax; ybincount++) {
					binCenter = yaxis->GetBinCenter(ybincount);
					laseron_ped = fitpol1[0]->Eval(binCenter);
					//laseron_ped = fitpol1[typestep-1]->Eval(binCenter); // NB change back
					laseron_pedcorr = numsamples * laseron_ped;
					laseron_bincorr = static_cast<int>(laseron_pedcorr/binWidth);
					//	printf("bin %i pedestal for %9.4g is %.2f correction is %.0f in bins  %.0f\n",
					//     ybincount, binCenter, laseron_ped, laseron_pedcorr, laseron_bincorr);
					for (Int_t xbincount = xbinmin; xbincount <= xbinmax; xbincount++) {
						// apply pedestal correction for laser ON
						Int_t newxbin = xbincount-laseron_bincorr;
						if (newxbin > 0) {
							bincont = samp_yieldVSsubint_type[typestep-1]->GetBinContent(xbincount,ybincount);
							binerr = samp_yieldVSsubint_type[typestep-1]->GetBinError(xbincount,ybincount);
							if (bincont>0){
								samp_yieldVSsubint_pedcorr->SetBinContent(newxbin,ybincount,typestep,bincont);
								samp_yieldVSsubint_pedcorr->SetBinError(newxbin,ybincount,typestep,binerr);
							} 
// 							bincont = samp_laserON_yieldVSsubint_hel0->GetBinContent(xbincount,ybincount);
// 							binerr = samp_laserON_yieldVSsubint_hel0->GetBinError(xbincount,ybincount);
// 							if (bincont>0){
// 								samp_laserON_yieldVSsubint_hel0_pedcorr->SetBinContent(newxbin,ybincount,bincont);
// 								samp_laserON_yieldVSsubint_hel0_pedcorr->SetBinError(newxbin,ybincount,binerr);
// 							} 
// 							bincont = samp_laserON_yieldVSsubint_hel1->GetBinContent(xbincount,ybincount);
// 							binerr = samp_laserON_yieldVSsubint_hel1->GetBinError(xbincount,ybincount);
// 							if (bincont>0){
// 								samp_laserON_yieldVSsubint_hel1_pedcorr->SetBinContent(newxbin,ybincount,bincont);
// 								samp_laserON_yieldVSsubint_hel1_pedcorr->SetBinError(newxbin,ybincount,binerr);
// 							} 
						}
					}
				}
			}
			realtime = timer.CpuTime();
			cputime = timer.RealTime();
			printf ("Type %i took %.2f seconds (%.2f s of cpu)\n",typestep,realtime,cputime);
			timer.Start(1);
		}
	}
*/

	TCanvas *samples;
	TPad *c_samples;
	if (do_samples) {
		if (!do_pedcorrection) {
			printf("Need to calculate pedestals to plot this stuff\n");
		} else {
			samples = new TCanvas("samples", Form("samples %i",runnum()), canvasxoff,0,canvaswidth,canvasheight*2);
			samples->cd();
			text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 5: Max and Integral", runnum()));
			c_samples = new TPad("c_samples","c_samples",0,0,1,1.0-textspace);
			c_samples->Draw();
			c_samples->Divide(2,6,0.001,0.001);
			c_samples->cd(1);
			draw1typeTH2(samp_yieldVSsubint,1,0,"Laser ON");
			c_samples->cd(2);
			draw1typeTH2(samp_yieldVSsubint,2,0,"Laser OFF");

// 			c_samples->cd(3);
// 			draw1typeTH2(samp_yieldVSsubint,3,0,"Beam OFF");
			
// 		TH1F *samp_pedcorr_subintproj = (TH1F*)samp_yieldVSsubint_pedcorr->
// 			ProjectionX("samp_yieldVSsubint_pedcorr_proj",1,-1,"e");
// 		samp_pedcorr_subintproj->SetTitle("integral, pedestal corrected;integral");
			
// 		TH1F *samp_laserON_hel0_pedcorr_subintproj = (TH1F*)samp_laserON_yieldVSsubint_hel0_pedcorr->
// 			ProjectionX("samp_laserON_yieldVSsubint_hel0_pedcorr_proj",1,-1,"e");
// 		TH1F *samp_laserON_hel1_pedcorr_subintproj = (TH1F*)samp_laserON_yieldVSsubint_hel1_pedcorr->
// 			ProjectionX("samp_laserON_yieldVSsubint_hel1_pedcorr_proj",1,-1,"e"); 
// 		samp_laserON_hel0_pedcorr_subintproj->SetTitle("integral, hel 0, pedestal corrected;integral");
// 		samp_laserON_hel1_pedcorr_subintproj->SetTitle("integral, hel 1, pedestal corrected;integral");

/* temporarily disabled			
			c_samples->cd(3);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,1,0,"Laser ON, ped. corr.");
			c_samples->cd(4);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,2,0,"Laser OFF, ped. corr.");
*/
// 			c_samples->cd(6);
// 			draw1typeTH2(samp_yieldVSsubint_pedcorr,3,0,"Beam OFF, ped. corr.");
				
			c_samples->cd(5);
			drawscalesubbytypeTH1(samp_integral, 1, 2, intnormintminchan+bigintoffset, intnormintmaxchan, 1);
			c_samples->cd(7);
			drawscalesubbytypeTH1(samp_subintegral, 1, 2,  intnormintminchan, intnormintmaxchan, 1);
			c_samples->cd(9);			
/* temporarily disabled	
			samp_yieldVSsubint_pedcorr->GetZaxis()->SetRange();
			TH2F *samp_subint_pedcorr  = (TH2F*)samp_yieldVSsubint_pedcorr->Project3D("zx");
			samp_subint_pedcorr->SetTitle("integral around maximum sample, timing cut, ped. corr.");
*/
			// Now write out the spectrum
			TH1F *printouthist = drawscalesubbytypeTH1(samp_subintegral, 1, 2,  intnormintminchan, intnormintmaxchan, 1);
			TString spectrumfilename = logdir + "Integral_spectrum.txt";
			std::cout << "Writing integral spectrum to " << spectrumfilename << std::endl;
			FILE* intspectfile = 0;
			if (!(intspectfile = fopen(spectrumfilename,"w"))) {
				std::cout << "Could not open file " << spectrumfilename << " for writing." << std::endl;
				return;
			}
			Int_t tmpminbin = printouthist->FindFirstBinAbove(0, 1);
			Int_t tmpmaxbin = printouthist->FindBin(intnormintminchan);
			for (Int_t count = tmpminbin; count <= tmpmaxbin; count++) {
				fprintf(intspectfile,"%.0f   %f   %f\n", printouthist->GetBinCenter(count), 
						printouthist->GetBinContent(count),  printouthist->GetBinError(count));
			}
			fclose(intspectfile);

	
			c_samples->cd(11);
			drawscalesubbytypeTH1(samp_timing_integral, 1, 2,  intnormintminchan, intnormintmaxchan, 1);


			c_samples->cd(6);
			drawscalesubbytypeTH1(samp_integral, 1, 2, intnormintminchan+bigintoffset, intnormintmaxchan, 0, 1, 1, intcomptonedge);
			c_samples->cd(8);
			drawscalesubbytypeTH1(samp_subintegral, 1, 2, intnormintminchan, intnormintmaxchan, 0, 1, 1, intcomptonedge);
			c_samples->cd(10);
/* temporarily disabled	
   drawscalesubbytypeTH1(samp_subint_pedcorr, 1, 2,  intnormintminchan, intnormintmaxchan, 0, 1, 1, intcomptonedge);
*/
			c_samples->cd(12);
			drawscalesubbytypeTH1(samp_timing_integral, 1, 2, intnormintminchan, intnormintmaxchan, 0, 1, 1, intcomptonedge);

//     c_samples->cd(10);
//     samp_laserON_yieldVSsubint_hel0_pedcorr->Draw("colz");
//     samp_laserON_yieldVSsubint_hel0_pedcorr->Draw("colz");
//     c_samples->cd(11);
//     samp_laserON_yieldVSsubint_hel1_pedcorr->Draw("colz");
//     c_samples->cd(12);
//     samp_laserON_hel0_pedcorr_subintproj->Rebin(10);
//     samp_laserON_hel0_pedcorr_subintproj->Draw();
//     //    samp_laserON_hel1_pedcorr_subintproj-
			//samples->Print(webdir + "samples_summary.png");
      SaveToWeb(samples,"samples_summary");
		}
	}




	TCanvas *maxandint;
	TPad *c_maxandint;
	if (do_maxandint) {
		if (!do_pedcorrection) {
			printf("Need to calculate pedestals to plot this stuff\n");
		} else {
			maxandint = new TCanvas("maxandint", Form("maxandint %i",runnum()), canvasxoff,0,canvaswidth,canvasheight*4/3);
			maxandint->cd();
			text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Max and Integral", runnum()));
			c_maxandint = new TPad("c_maxandint","c_maxandint",0,0,1,1.0-textspace);
			c_maxandint->Draw();
			c_maxandint->Divide(3,4,0.001,0.001);
				
			if (dobeamoffsamps) {
				c_maxandint->cd(1);
				drawALLtypeTH1(samp_integral,0,"",1);
				c_maxandint->cd(4);
				drawALLtypeTH1(samp_integral,0,"",1);
				c_maxandint->cd(2);
				drawALLtypeTH1(samp_subintegral,0,"",1);
				c_maxandint->cd(5);
				drawALLtypeTH1(samp_subintegral,0,"",1);
				c_maxandint->cd(3);
				drawALLtypeTH1(samp_timing_integral,0,"",1);
				c_maxandint->cd(6);
				drawALLtypeTH1(samp_timing_integral,0,"",1);
			} else {
				c_maxandint->cd(1);
				drawscalesubbytypeTH1(samp_integral, 1, 2, intnormintminchan+bigintoffset, intnormintmaxchan, 1);
				c_maxandint->cd(4);
				drawscalesubbytypeTH1(samp_integral, 1, 2, intnormintminchan+bigintoffset, intnormintmaxchan, 0, 1, 1, intcomptonedge);
				c_maxandint->cd(2);
				drawscalesubbytypeTH1(samp_subintegral, 1, 2,  intnormintminchan, intnormintmaxchan, 1);
				c_maxandint->cd(5);
				drawscalesubbytypeTH1(samp_subintegral, 1, 2, intnormintminchan, intnormintmaxchan, 0, 1, 1, intcomptonedge);
				c_maxandint->cd(3);
				drawscalesubbytypeTH1(samp_timing_integral, 1, 2,  intnormintminchan, intnormintmaxchan, 1);
				c_maxandint->cd(6);
				drawscalesubbytypeTH1(samp_timing_integral, 1, 2, intnormintminchan, intnormintmaxchan, 0, 1, 1, intcomptonedge);
			}

			c_maxandint->cd(7);
			draw1typeTH2(samp_intVSmax,-2,0,"",1,0,0);
			c_maxandint->cd(10);
			samp_intVSmax->GetXaxis()->SetRangeUser(0,maxnormintminchan);
			samp_intVSmax->GetYaxis()->SetRangeUser(0,intnormintminchan+bigintoffset);
			draw1typeTH2(samp_intVSmax,-3,0,"",1,0,0);


			c_maxandint->cd(8);
			draw1typeTH2(samp_subintVSmax,-2,0,"",1,0,0);
			c_maxandint->cd(11);
			samp_subintVSmax->GetXaxis()->SetRangeUser(0,maxnormintminchan);
			samp_subintVSmax->GetYaxis()->SetRangeUser(0,intnormintminchan+bigintoffset);
			draw1typeTH2(samp_subintVSmax,-3,0,"",1,0,0);

			c_maxandint->cd(9);
			draw1typeTH2(samp_timingintVSmax,-2,0,"",1,0,0);
			c_maxandint->cd(12);
			samp_timingintVSmax->GetXaxis()->SetRangeUser(0,maxnormintminchan);
			samp_timingintVSmax->GetYaxis()->SetRangeUser(0,intnormintminchan+bigintoffset);
			draw1typeTH2(samp_timingintVSmax,-3,0,"",1,0,0);

			//maxandint->Print(webdir + "samples_maxandint.png");
      SaveToWeb(maxandint,"samples_maxandint");
		}
	}




 	TCanvas *samples_asymmetry;
 	TPad *c_samples_asymmetry;
 	if (do_samples_asymmetry) {
		samples_asymmetry = new TCanvas("samples_asymmetry", Form("samples_asymmetry %i",runnum()),
										canvasxoff,0,canvaswidth,canvasheight);
 		samples_asymmetry->cd();
 		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 6: Asymmetry analysis",runnum()));
		c_samples_asymmetry = new TPad("c_samples_asymmetry","c_samples_asymmetry",0,0,1,1.0-textspace);
		c_samples_asymmetry->Draw();
		c_samples_asymmetry->Divide(3,3,0.001,0.001);
		c_samples_asymmetry->cd(1);
		drawscalesubbytypeTH1(samp_max_hel0, 1, 2, maxnormintminchan, maxnormintmaxchan, 1);
		c_samples_asymmetry->cd(2);
		drawscalesubbytypeTH1(samp_max_hel1, 1, 2, maxnormintminchan, maxnormintmaxchan, 1);

		TH1F *samp_max_hel0_laserON = (TH1F*)samp_max_hel0->ProjectionX("samp_max_hel0_laserON",1,1,"e");
		TH1F *samp_max_hel1_laserON = (TH1F*)samp_max_hel1->ProjectionX("samp_max_hel1_laserON",1,1,"e"); 
		samp_max_hel0_laserON->SetTitle("max, laser ON, hel 0;max");
		samp_max_hel1_laserON->SetTitle("max, laser ON, hel 1;max");
  
 		TH1F *samp_max_laserOFF = (TH1F*)samp_max->ProjectionX("samp_max_laserOFF",2,2,"e"); 
		samp_max_laserOFF->SetTitle("max, laser OFF,;max");

		Int_t maxminbin = samp_max_laserOFF->FindBin(maxnormintminchan);
		Int_t maxmaxbin = samp_max_laserOFF->FindBin(maxnormintmaxchan);
 		Double_t intOFF = samp_max_laserOFF->Integral(maxminbin, maxmaxbin);
 		Double_t intON_hel0 = samp_max_hel0_laserON->Integral(maxminbin, maxmaxbin);
 		Double_t intON_hel1 = samp_max_hel1_laserON->Integral(maxminbin, maxmaxbin);
 		Double_t scalefac = (intON_hel0+intON_hel1)/(2*intOFF);
 		printf("intON_hel0=%3g  intON_hel1=%3g  intOFF=%3g  scalefac=%3g\n",intON_hel0, intON_hel1, intOFF, scalefac);

 		samp_max_hel0_laserON->Rebin(intrebinnum);
 		samp_max_hel1_laserON->Rebin(intrebinnum);
 		samp_max_laserOFF->Rebin(intrebinnum);

 		c_samples_asymmetry->cd(4);
		TH2F *samp_max_hel0_rebin = (TH2F*)samp_max_hel0->Clone("samp_max_hel0_rebin");
 		samp_max_hel0_rebin->RebinX(intrebinnum);
// 		TH1F* samp_max_hel0_rebin_sub = drawscalesubbytypeTH1(samp_max_hel0_rebin, 1, 2, (maxmin+1)/intrebinnum, (maxmax+1)/intrebinnum, 1);
 		TH1F* samp_max_hel0_rebin_sub = drawscalesubbytypeTH1(samp_max_hel0_rebin, 1, 2, maxnormintminchan, maxnormintmaxchan, 1);
 		c_samples_asymmetry->cd(5);
		TH2F *samp_max_hel1_rebin = (TH2F*)samp_max_hel1->Clone("samp_max_hel1_rebin");
 		samp_max_hel1_rebin->RebinX(intrebinnum);
// 		TH1F* samp_max_hel1_rebin_sub = drawscalesubbytypeTH1(samp_max_hel1_rebin, 1, 2, (maxmin+1)intrebinnum, (maxmax+1)/intrebinnum, 1);

 		TH1F* samp_max_hel1_rebin_sub = drawscalesubbytypeTH1(samp_max_hel1_rebin, 1, 2, maxnormintminchan, maxnormintmaxchan, 1);

		c_samples_asymmetry->cd(6);
 		TH1F *asym_notbksub = (TH1F*) samp_max_hel1_laserON->GetAsymmetry(samp_max_hel0_laserON);
 		asym_notbksub->SetMaximum(std::min(asym_notbksub->GetMaximum(),0.06));
 		asym_notbksub->SetMinimum(std::max(asym_notbksub->GetMinimum(),-0.06));
 		asym_notbksub->SetTitle("Asymmetry (NOT background subtracted)");
		asym_notbksub->SetLineColor(kRed);
 		asym_notbksub->Draw();
		asym_notbksub->GetXaxis()->SetRangeUser(0,maxnormintminchan);


// 		c_samples_asymmetry->cd(4);
// 		samp_max_hel0_laserON->DrawCopy();
// 		c_samples_asymmetry->cd(5);
// 		samp_max_hel1_laserON->DrawCopy();
// 		c_samples_asymmetry->cd(6);
// 		samp_max_laserOFF->DrawCopy();
//  		TH1F* samp_max_hel0_laserON_bksub = samp_max_hel0_laserON->Clone("samp_max_hel0_laserON_bksub");
//  		TH1F* samp_max_hel1_laserON_bksub = samp_max_hel1_laserON->Clone("samp_max_hel1_laserON_bksub");
//  		samp_max_laserOFF->Scale(scalefac);
//  		samp_max_hel0_laserON_bksub->Add(samp_max_laserOFF,-1);
//  		samp_max_hel1_laserON_bksub->Add(samp_max_laserOFF,-1);

		c_samples_asymmetry->cd(7);
		samp_max_hel0_rebin_sub->GetXaxis()->SetRangeUser(0,maxnormintminchan);
		samp_max_hel0_rebin_sub->DrawCopy("e");
		c_samples_asymmetry->cd(8);
		samp_max_hel1_rebin_sub->GetXaxis()->SetRangeUser(0,maxnormintminchan);
		samp_max_hel1_rebin_sub->DrawCopy("e");
		c_samples_asymmetry->cd(9);
 		TH1F *asym_bksub = (TH1F*) samp_max_hel1_rebin_sub->GetAsymmetry(samp_max_hel0_rebin_sub);
 		asym_bksub->SetMaximum(std::min(asym_bksub->GetMaximum(),0.06));
 		asym_bksub->SetMinimum(std::max(asym_bksub->GetMinimum(),-0.06));
 		asym_bksub->SetTitle("Asymmetry (background subtracted)");
		asym_bksub->SetLineColor(kRed);
 		asym_bksub->Draw();
		asym_bksub->GetXaxis()->SetRangeUser(0,maxnormintminchan);

		//samples_asymmetry->Print(webdir + "samples_asymmetry.png");
    SaveToWeb(samples_asymmetry,"samples_assymmetry");
	}


	rootoutfile->Write();
	rootoutfile->Close();
	
}



/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */

// Define functions with c symbols (create/destroy instances)·
extern "C" Snapshots* create()
{
  return new Snapshots();
}


extern "C" void destroy( Snapshots *accum )
{
  delete accum;
}
