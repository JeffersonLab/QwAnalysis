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


#include "QwSIS3320_Samples.h"


#include "getChain.C"


void snapshots(Int_t runnumber = 0, Int_t maxevents = 0)
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

	TString rootoutdir  = TString(getenv("QW_ROOTFILES")) + "/summary/";
	gSystem->mkdir(rootoutdir.Data(),kTRUE);
	TString rootoutname = rootoutdir + Form("Compton_NewSummary_%i.root",runnumber);
	TFile*  rootoutfile = TFile::Open(rootoutname,"RECREATE");

	TChain *Mps_Chain = getMpsChain(runnumber);

	Int_t nentries = Mps_Chain->GetEntries();
	printf("There are %i entries\n",nentries);
	if (maxevents > 0) {
		nentries=std::min(maxevents+offsetentry,nentries);
		printf("Analysing the first %i entries\n",nentries);
	}
	// **********
	// **** Set constants required for histograms
	Int_t bcm_nbins = 180;
	Double_t bcm_axismin = 0;
	Double_t bcm_axismax = 180;
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

 	// **** read parameter file
	int i_run, i_lowint, i_upint;
	char params[255][15];
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
	laserpowinit->SetDirectory(rootoutfile);
	Int_t nbins = laserpowinit->GetNbinsX();
	Float_t laserpowmax = laserpowinit->GetXaxis()->GetBinUpEdge(nbins);
	Float_t laseroncut = 0.7*laserpowmax;
	printf("max of sca_laser_PowT is %.1f, using %.1f as the cut position\n",laserpowmax,laseroncut);

	Mps_Chain->Draw("fadc_compton_accum0.hw_sum>>accum0init(300)","Entry$>2100","goff");
	TH1F *accum0init = (TH1F*)gDirectory->Get("accum0init");
	accum0init->SetDirectory(rootoutfile);
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
	Mps_Chain->SetBranchStatus("sca_bcm*", 1);
	Mps_Chain->SetBranchStatus("actual_helicity*", 1);

// 	for (Int_t stripnum=0; stripnum < numstrips; stripnum++) {
// 		Mps_Chain->SetBranchStatus(Form("p1s%iRawAc",stripnum), 1);
// 		Mps_Chain->SetBranchStatus(Form("p2s%iRawAc",stripnum), 1);
// 		Mps_Chain->SetBranchStatus(Form("p3s%iRawAc",stripnum), 1);
// 	}

	Double_t bcm1, bcm2, bcm6;
	std::vector<QwSIS3320_Samples>* sampled_events = 0;
	Double_t sca_laser_PowT;
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
				300, 0, 300,
				type_nbins, type_axismin, type_axismax);
	laser->SetDirectory(rootoutfile);

	TH2F *accum0 = new TH2F("accum0","accum0 yield",
				yieldnbins, yield_axismin, yield_axismax,
				type_nbins, type_axismin, type_axismax);
	accum0->SetDirectory(rootoutfile);

	TH3F *lasertime = new TH3F("lasertime","Laser Power vs time;time  (seconds)",
				timebins, starttime, endtime,
				300, 0, 300,
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

	TH1F *samp_bcm1 = new TH1F("samp_bcm1","bcm 1", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm2 = new TH1F("samp_bcm2","bcm 2", bcm_nbins, bcm_axismin, bcm_axismax);
	TH1F *samp_bcm6 = new TH1F("samp_bcm6","bcm 6", bcm_nbins, bcm_axismin, bcm_axismax);
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
	TH2F *samp_subintegral = new TH2F("samp_subintegral"," integral (max pos cut, peak cut);type",
									  integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_subintegral->SetDirectory(rootoutfile);
	TH2F *samp_pedprepeakave = new TH2F("samp_pedprepeakave","samp 10-6 before max ave;pedestal ave;type",
										pedval_nbins,pedval_axismin,pedval_axismax,type_nbins,type_axismin,type_axismax);
	samp_pedprepeakave->SetDirectory(rootoutfile);
	TH2F *samp_integral = new TH2F("samp_integral"," integral;integral;type",
								   integnbins,integmin,integmax,type_nbins,type_axismin,type_axismax);
	samp_integral->SetDirectory(rootoutfile);
	TH2F *samp_max = new TH2F("samp_max"," max;max",
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
	std::string mytext = "";
	Double_t eventtime;
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
			// **********
			// **** Set the switches for the event
			laseron = sca_laser_PowT>laseroncut;
			laseroff = sca_laser_PowT<10;
			beamon = bcm6 > currentCut;
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
			bcmtime->Fill(eventtime,bcm1,type);
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
						   jentry, 100*jentry/nentries, sca_laser_PowT, bcm1, laseron, laseroff, beamon, type,
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
					if (num_graphslaserON<20 && type == 0) {
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
					if (num_graphslaserOFF<20 && type == 1) {
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
					if (num_graphsbeamOFF<20 && type == 2) {
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
						samp_bcm1->Fill(bcm1);
						samp_bcm2->Fill(bcm2);
						samp_bcm6->Fill(bcm6); 
						if (actual_helicity==0) {
							samp_max_hel0->Fill(samp_max_val,type);
							samp_yieldVSsubint_hel0->Fill(samp_integral_val,fadc_compton_accum0[0]);
							samp_bcm1_hel0->Fill(bcm1);
							samp_bcm2_hel0->Fill(bcm2);
							samp_bcm6_hel0->Fill(bcm6); 
						} else {
							if (actual_helicity==1) {
								samp_max_hel1->Fill(samp_max_val,type);
								samp_yieldVSsubint_hel1->Fill(samp_integral_val,fadc_compton_accum0[0]);
								samp_bcm1_hel1->Fill(bcm1); 
								samp_bcm2_hel1->Fill(bcm2); 
								samp_bcm6_hel1->Fill(bcm6);
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
	
// 	rootoutfile->cd();
// 	for (Int_t i=0; i<20; i++) {
// 		std::cout << "Writing beam OFF " << i << "  " << graphsbeamOFF[i] << std::endl;
// 		if (graphsbeamOFF[i]) {
// 			//graphsbeamOFF[i]->SetDirectory(rootoutfile);	
// 			graphsbeamOFF[i]->Write();	
// 		}
// 	}
// 	for (Int_t i=0; i<20; i++) {
// 		std::cout << "Writing laser ON " << i << "  " << graphslaserON[i] << std::endl;
// 		if (graphslaserON[i]){
// 			//	graphslaserON[i]->SetDirectory(rootoutfile);	
// 			graphslaserON[i]->Write();	
// 		}
// 	}
// 	for (Int_t i=0; i<20; i++) {
// 		if (graphslaserOFF[i]){
// 			//	graphslaserOFF[i]->SetDirectory(rootoutfile);	
// 			graphslaserOFF[i]->Write();	
// 		}
// 	}
	

	TString logdir = TString(getenv("QWSCRATCH")) + Form("/www/run_%i/",runnumber);
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
	fprintf(logfile," Total events:           %8i     \n", nentries);
	fprintf(logfile," Snaps, beam and laser:  %8i     \n", num_beamon_laseron);
	fprintf(logfile," Snaps, beam no laser:   %8i     \n", num_beamon_laseroff);
	fprintf(logfile," Snaps, beam some laser: %8i     \n", num_beamon_laserbad);
	fprintf(logfile," Snaps, no beam:         %8i     \n", num_beamoff);
	fprintf(logfile," No snapshots:           %8i     \n", num_no_samples);
	fprintf(logfile," Bad snapshots:          %8i     \n", num_badsnaps);
	fprintf(logfile," Bad helicity:           %8i     \n", num_badhelicity);
	fprintf(logfile," Bad BCM:                %8i     \n", num_badbcm);
	fprintf(logfile," Bad Max position:       %8i     \n", num_badmaxpos);
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

	std::cout << "do_mps: " << do_mps << std::endl;
        std::cout << "do_samples_correl " << do_samples_correl << std::endl;
        std::cout << "do_samples_pedestal " << do_samples_pedestal << std::endl;
        std::cout << "do_samples_pedcorr " << do_samples_pedcorr << std::endl;
        std::cout << "do_pedcorrection " << do_pedcorrection << std::endl;
        std::cout << "do_samples " << do_samples << std::endl;
        std::cout << "do_samples_asymmetry " << do_samples_asymmetry << std::endl;
        std::cout << "do_samples_responsefunc " << do_samples_responsefunc << std::endl;
        std::cout << "do_waveforms " << do_waveforms << std::endl;

	TString outtextfilename_ped;
	TString outtextfilename_width;
	TString rundir = TString(getenv("QWSCRATCH")) + Form("/www/run_%i/",runnumber);
	gSystem->mkdir(rundir.Data(),kTRUE);
	outtextfilename_ped = rundir + "pedestalfit.txt";
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);
	// **** Really important number for pedestal correction
	Double_t numsamples = 190; //256


	//Int_t yieldrebinnum = 10;
	Int_t intrebinnum = 10;

	// *************************** 
	// ***** Set up for the canvas
	// *************************** 
	Int_t canvaswidth,canvasheight,canvasxoff,xpads,ypads;
	if (gROOT->IsBatch()) {
		canvasxoff=0;  canvaswidth=1000;  canvasheight=800; xpads=3; ypads=6; // Batch mode
	} else {
		canvasxoff=100;  canvaswidth=800;  canvasheight=600; xpads=6; ypads=3; // Interactive vnc
		//canvasxoff=200;  canvaswidth=1000;  canvasheight=800; xpads=3; ypads=6; // Interactive ccomptl1
	}
	if (0) {
		printf("\nKill me now!!!!!\n");
		sleep(10);
	}


	Int_t intmin, intmax, subintmin, subintmax;
	intmin = 100; intmax = 550; subintmin = 300; subintmax = 550;
	if (runnumber <= 21136) {
		intmin = 350; intmax = 650; subintmin = 350; subintmax = 650; maxmin=250; maxmax=400;
	}
// 	if (runnumber == 21136) {
// 		intmin = 80; intmax = 650; subintmin = 160; subintmax = 650; maxmin=180; maxmax=330;
// 	}
	if (runnumber >= 21137) {
		intmin = 110; intmax = 650; subintmin = 200; subintmax = 650; maxmin=140; maxmax=330;
	}
	if (runnumber >= 21139) {
		intmin = 300; intmax = 1150; subintmin = 350; subintmax = 1150; maxmin=190; maxmax=330;
	}
	if (runnumber >= 21140) {
		intmin = 550; intmax = 1150; subintmin = 600; subintmax = 1150; maxmin=300; maxmax=330;
	}
	if (runnumber >= 21142) {
		intmin = 500; intmax = 1000; subintmin = 550; subintmax = 1000; maxmin=330; maxmax=350;
	}
	if (runnumber >= 21143) {
		intmin = 450; intmax = 1000; subintmin = 450; subintmax = 1000; maxmin=330; maxmax=350;
	}	
	if (runnumber == 21144) {
		intmin = 100; intmax = 1000; subintmin = 170; subintmax = 1000; maxmin=150; maxmax=330;
	}	
	if (runnumber == 21145) {
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=330; maxmax=350;
	}	
	if (runnumber == 21146 || runnumber == 21147) {
		intmin = 300; intmax = 1000; subintmin = 300; subintmax = 1000; maxmin=250; maxmax=350;
	}
	if (runnumber >= 21148) {
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=250; maxmax=350;
	}	
	if (runnumber >= 21317) {
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=270; maxmax=350;
	}
	if (runnumber >= 21339) { // Start with electron triggers, 1900 V
		intmin = 400; intmax = 1000; subintmin = 450; subintmax = 1000; maxmin=270; maxmax=350;
	}	
	if (runnumber >= 21391) { // Go to 1800 V
		intmin = 250; intmax = 1000; subintmin = 300; subintmax = 1000; maxmin=200; maxmax=350;
	}	
 	if (runnumber == 21393) { // 1900 V
		intmin = 300; intmax = 1000; subintmin = 350; subintmax = 1000; maxmin=270; maxmax=350;
	}  
	if (runnumber >= 21452) { // Go to 1900 V
		intmin = 350; intmax = 1000; subintmin = 400; subintmax = 1000; maxmin=270; maxmax=350;
	}
	if (runnumber >= 21608) { // PbW04
		intmin = 200; intmax = 1000; subintmin = 360; subintmax = 1000; maxmin=80; maxmax=260;
	}
	if (runnumber == 21619) { // PbW04
		intmin = 360; intmax = 1000; subintmin = 400; subintmax = 1000; maxmin=130; maxmax=260;
	}
	if (runnumber >= 21660) { // PbW04
		intmin = 150; intmax = 1000; subintmin = 200; subintmax = 1000; maxmin=140; maxmax=388;
	}
	if (runnumber >= 21987) { // BGO
		intmin = 150; intmax = 1000; subintmin = 200; subintmax = 1000; maxmin=140; maxmax=388;
	}
	printf("Using intmin %i   intmax %i   subintmin %i   subintmax %i   maxmin %f   maxmax %f \n",
		   intmin, intmax, subintmin, subintmax, maxmin, maxmax);

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
		waveforms_laserON = new TCanvas("waveforms_laserON", Form("waveforms %i",runnumber), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_laserON->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 7: Laser ON snapshots", runnumber));
		c_waveforms_laserON = new TPad("c_waveforms_laserON","c_waveforms_laserON",0,0,1,1.0-textspace);
		c_waveforms_laserON->Draw();
		c_waveforms_laserON->Divide(5,4,0.001,0.001);
		for (Int_t i=1; i<=20; i++) {
			padp = c_waveforms_laserON->cd(i);
			TGraph *graph1;
//			graph1 = 
			gDirectory->GetObject(Form("laserON%02i",i-1),graph1);
			//cout << graph1 << endl;
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
		waveforms_laserON->Print(rundir + "waveforms_laserON.png");
	}

	TCanvas *waveforms_laserOFF;
	TPad *c_waveforms_laserOFF;    
	if (do_waveforms) {
		waveforms_laserOFF = new TCanvas("waveforms_laserOFF", Form("waveforms %i",runnumber), 
										 canvasxoff,0,canvaswidth,canvasheight);
		waveforms_laserOFF->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 8: Laser OFF snapshots", runnumber));
		c_waveforms_laserOFF = new TPad("c_waveforms_laserOFF","c_waveforms_laserOFF",0,0,1,1.0-textspace);
		c_waveforms_laserOFF->Draw();
		c_waveforms_laserOFF->Divide(5,4,0.001,0.001);
		for (Int_t i=1; i<=20; i++) {
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
		waveforms_laserOFF->Print(rundir + "waveforms_laserOFF.png");
	}

	TCanvas *waveforms_beamOFF;
	TPad *c_waveforms_beamOFF;    
	if (do_waveforms) {
		waveforms_beamOFF = new TCanvas("waveforms_beamOFF", Form("waveforms %i",runnumber), 
										canvasxoff,0,canvaswidth,canvasheight);
		waveforms_beamOFF->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 9: beam OFF snapshots", runnumber));
		c_waveforms_beamOFF = new TPad("c_waveforms_beamOFF","c_waveforms_beamOFF",0,0,1,1.0-textspace);
		c_waveforms_beamOFF->Draw();
		c_waveforms_beamOFF->Divide(5,4,0.001,0.001);
		for (Int_t i=1; i<=20; i++) {
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
		waveforms_beamOFF->Print(rundir + "waveforms_beamOFF.png");
	}



	TCanvas *mps;
	TPad *c_mps;
	if (do_mps) {
		mps = new TCanvas("mps", Form("mps %i",runnumber),canvasxoff,0,canvaswidth,canvasheight);
		mps->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 1: MPS tree general",runnumber));
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
		snaphist->DrawCopy();
		//cerr << "time 3\n" << endl;
		gPad->SetMargin(0.06,0.02,0.1,0.1);
		mps->Print(rundir + "mps_summary.png");
	}

	TCanvas *samples_correl;
	TPad *c_samples_correl;
	if (do_samples_correl) {
		samples_correl = new TCanvas("samples_correl", Form("samples_correl %i",runnumber),
									 canvasxoff,0,canvaswidth,canvasheight);
		samples_correl->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 2: Snapshot general",runnumber));
		c_samples_correl = new TPad("c_samples_correl","c_samples_correl",0,0,1,1.0-textspace);
		c_samples_correl->Draw();
		c_samples_correl->Divide(3,3,0.001,0.001);
		c_samples_correl->cd(1);
		drawscalesubbytypeTH1(samp_max, 1, 2, maxmin, maxmax, 1);
		c_samples_correl->cd(2);
		drawALLtypeTH1(samp_min,0,"",1);
		c_samples_correl->cd(3);
		drawALLtypeTH1(samp_pedave,1,"",1);
		c_samples_correl->cd(4);
		drawALLtypeTH1(samp_maxpos,0,"",1);
		c_samples_correl->cd(5);
		drawALLtypeTH1(samp_minpos,0,"",1);
		c_samples_correl->cd(6);
		drawALLtypeTH1(samp_pedprepeakave,1,"",1);
		c_samples_correl->cd(7);
		outtextfilename_width = rundir + "width_pairdiff.txt";
		drawALLtypeTH1(accum0_diff_pair,1,"",1,outtextfilename_width.Data());
		c_samples_correl->cd(8);
		outtextfilename_width = rundir + "width_quaddiff.txt";
		drawALLtypeTH1(accum0_diff_quad,1,"",1,outtextfilename_width.Data());
		c_samples_correl->cd(9);
		outtextfilename_width = rundir + "width_32diff.txt";
		drawALLtypeTH1(accum0_diff_32,1,"",1,outtextfilename_width.Data());
		//drawALLtypeTH1(accum0_asym_32);
		//draw3TH1gen(samp_bcm6,samp_bcm1,samp_bcm2,1,"bcm 6","bcm 1","bcm 2",kRed,kGreen,kBlue,"bcms");
		//drawALLtypeTH1(accum0,1,"",1);
		//drawscalesub1D(samp_laserOFF_max, samp_laserON_max, 160, 280, 0);
		samples_correl->Print(rundir + "samples_correl.png");
	}

	TCanvas *samples_pedestal;
	TPad *c_samples_pedestal;
	if (do_samples_pedestal) {
		samples_pedestal = new TCanvas("samples_pedestal",
									   Form("samples_pedestal %i",runnumber),
									   canvasxoff,0,canvaswidth,canvasheight);
		samples_pedestal->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 3: Snapshot pedestal analysis",runnumber));
		c_samples_pedestal = new TPad("c_samples_pedestal","c_samples_pedestal",0,0,1,1.0-textspace);
		c_samples_pedestal->Draw();
		c_samples_pedestal->Divide(4,4,0.001,0.001);
		c_samples_pedestal->cd(1);
		draw1typeTH2(samp_maxVSmaxpos,1,0,"Laser ON");
		c_samples_pedestal->cd(2);
		draw1typeTH2(samp_maxVSmaxpos,2,0,"Laser OFF");
		c_samples_pedestal->cd(3);
		draw1typeTH2(samp_maxVSmaxpos,3,0,"Beam OFF");
		c_samples_pedestal->cd(4);
		draw1typeTH2(samp_maxVSmaxpos,4,0,"laser flash");

		c_samples_pedestal->cd(5);
		draw1typeTH2(samp_minVSminpos,1,0,"Laser ON");
		c_samples_pedestal->cd(6);
		draw1typeTH2(samp_minVSminpos,2,0,"Laser OFF");
		c_samples_pedestal->cd(7);
		draw1typeTH2(samp_minVSminpos,3,0,"Beam OFF");
		c_samples_pedestal->cd(8);
		draw1typeTH2(samp_minVSminpos,4,0,"laser flash");

		c_samples_pedestal->cd(9);
		draw1typeTH2(samp_pedVStime,1,0,"Laser ON");
		c_samples_pedestal->cd(10);
		draw1typeTH2(samp_pedVStime,2,0,"Laser OFF");
		c_samples_pedestal->cd(11);
		draw1typeTH2(samp_pedVStime,3,0,"Beam OFF");
		c_samples_pedestal->cd(12);
		draw1typeTH2(samp_pedVStime,4,0,"laser flash");

		c_samples_pedestal->cd(13);
		draw1typeTH2(samp_pedVSpedpp,1,0,"Laser ON");
		c_samples_pedestal->cd(14);
		draw1typeTH2(samp_pedVSpedpp,2,0,"Laser OFF");
		c_samples_pedestal->cd(15);
		draw1typeTH2(samp_pedVSpedpp,3,0,"Beam OFF");
		c_samples_pedestal->cd(16);
		draw1typeTH2(samp_pedVSpedpp,4,0,"laser flash");

		samples_pedestal->Print(rundir + "samples_ped.png");
	}

	TCanvas *samples_pedcorr;
	TPad *c_samples_pedcorr;
	if (do_samples_pedcorr) {
		samples_pedcorr = new TCanvas("samples_pedcorr", Form("samples_pedcorr %i",runnumber),
									  canvasxoff,0,canvaswidth,canvasheight);
		samples_pedcorr->cd();
		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 4: Pedestal correction analysis",runnumber));
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
		samples_pedcorr->Print(rundir + "samples_pedcorr.png");
	}

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


	TCanvas *samples;
	TPad *c_samples;
	if (do_samples) {
		if (!do_pedcorrection) {
			printf("Need to calculatepedestals to plot this stuff\n");
		} else {
			samples = new TCanvas("samples", Form("samples %i",runnumber), canvasxoff,0,canvaswidth,canvasheight);
			samples->cd();
			text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 5: Pedestal corrected integral analysis", runnumber));
			c_samples = new TPad("c_samples","c_samples",0,0,1,1.0-textspace);
			c_samples->Draw();
			c_samples->Divide(3,4,0.001,0.001);
			c_samples->cd(1);
			draw1typeTH2(samp_yieldVSsubint,1,0,"Laser ON");
			c_samples->cd(2);
			draw1typeTH2(samp_yieldVSsubint,2,0,"Laser OFF");
			c_samples->cd(3);
			draw1typeTH2(samp_yieldVSsubint,3,0,"Beam OFF");
			
// 		TH1F *samp_pedcorr_subintproj = (TH1F*)samp_yieldVSsubint_pedcorr->
// 			ProjectionX("samp_yieldVSsubint_pedcorr_proj",1,-1,"e");
// 		samp_pedcorr_subintproj->SetTitle("integral, pedestal corrected;integral");
			
// 		TH1F *samp_laserON_hel0_pedcorr_subintproj = (TH1F*)samp_laserON_yieldVSsubint_hel0_pedcorr->
// 			ProjectionX("samp_laserON_yieldVSsubint_hel0_pedcorr_proj",1,-1,"e");
// 		TH1F *samp_laserON_hel1_pedcorr_subintproj = (TH1F*)samp_laserON_yieldVSsubint_hel1_pedcorr->
// 			ProjectionX("samp_laserON_yieldVSsubint_hel1_pedcorr_proj",1,-1,"e"); 
// 		samp_laserON_hel0_pedcorr_subintproj->SetTitle("integral, hel 0, pedestal corrected;integral");
// 		samp_laserON_hel1_pedcorr_subintproj->SetTitle("integral, hel 1, pedestal corrected;integral");
			
			c_samples->cd(4);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,1,0,"Laser ON, ped. corr.");
			c_samples->cd(5);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,2,0,"Laser OFF, ped. corr.");
			c_samples->cd(6);
			draw1typeTH2(samp_yieldVSsubint_pedcorr,3,0,"Beam OFF, ped. corr.");
			
			c_samples->cd(7);
			drawscalesubbytypeTH1(samp_integral, 1, 2, intmin, intmax, 1);
			c_samples->cd(8);
			drawscalesubbytypeTH1(samp_subintegral, 1, 2, intmin, intmax, 1);
			c_samples->cd(9);			
// 		samp_yieldVSsubint_pedcorr->GetXaxis()->SetRange();
// 		samp_yieldVSsubint_pedcorr->GetYaxis()->SetRange();
			samp_yieldVSsubint_pedcorr->GetZaxis()->SetRange();
			TH2F *samp_subint_pedcorr  = (TH2F*)samp_yieldVSsubint_pedcorr->Project3D("zx");
			samp_subint_pedcorr->SetTitle("Integral, (max pos cut, peak cut, ped. corr.)");
			//samp_subint_pedcorr->Draw("colz");
			drawscalesubbytypeTH1(samp_subint_pedcorr, 1, 2, subintmin, subintmax, 1);
	
			c_samples->cd(10);
			drawscalesubbytypeTH1(samp_integral, 1, 2, intmin, intmax, 0);
			c_samples->cd(11);
			drawscalesubbytypeTH1(samp_subintegral, 1, 2, intmin, intmax, 0);
			c_samples->cd(12);
			drawscalesubbytypeTH1(samp_subint_pedcorr, 1, 2, subintmin, subintmax, 0);	

//     c_samples->cd(10);
//     samp_laserON_yieldVSsubint_hel0_pedcorr->Draw("colz");
//     samp_laserON_yieldVSsubint_hel0_pedcorr->Draw("colz");
//     c_samples->cd(11);
//     samp_laserON_yieldVSsubint_hel1_pedcorr->Draw("colz");
//     c_samples->cd(12);
//     samp_laserON_hel0_pedcorr_subintproj->Rebin(10);
//     samp_laserON_hel0_pedcorr_subintproj->Draw();
//     //    samp_laserON_hel1_pedcorr_subintproj-
			samples->Print(rundir + "samples_summary.png");
		}
	}




 	TCanvas *samples_asymmetry;
 	TPad *c_samples_asymmetry;
 	if (do_samples_asymmetry) {
		samples_asymmetry = new TCanvas("samples_asymmetry", Form("samples_asymmetry %i",runnumber),
										canvasxoff,0,canvaswidth,canvasheight);
 		samples_asymmetry->cd();
 		text.DrawLatex(0.5,1-(textspace/2.),Form("Run %i, Page 6: Asymmetry analysis",runnumber));
		c_samples_asymmetry = new TPad("c_samples_asymmetry","c_samples_asymmetry",0,0,1,1.0-textspace);
		c_samples_asymmetry->Draw();
		c_samples_asymmetry->Divide(3,3,0.001,0.001);
		c_samples_asymmetry->cd(1);
		drawscalesubbytypeTH1(samp_max_hel0, 1, 2, maxmin, maxmax, 1);
		c_samples_asymmetry->cd(2);
		drawscalesubbytypeTH1(samp_max_hel1, 1, 2, maxmin, maxmax, 1);

		TH1F *samp_max_hel0_laserON = (TH1F*)samp_max_hel0->ProjectionX("samp_max_hel0_laserON",1,1,"e");
		TH1F *samp_max_hel1_laserON = (TH1F*)samp_max_hel1->ProjectionX("samp_max_hel1_laserON",1,1,"e"); 
		samp_max_hel0_laserON->SetTitle("max, laser ON, hel 0;max");
		samp_max_hel1_laserON->SetTitle("max, laser ON, hel 1;max");
  
 		TH1F *samp_max_laserOFF = (TH1F*)samp_max->ProjectionX("samp_max_laserOFF",2,2,"e"); 
		samp_max_laserOFF->SetTitle("max, laser OFF,;max");

		Int_t maxminbin = samp_max_laserOFF->FindBin(maxmin);
		Int_t maxmaxbin = samp_max_laserOFF->FindBin(maxmax);
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
 		TH1F* samp_max_hel0_rebin_sub = drawscalesubbytypeTH1(samp_max_hel0_rebin, 1, 2, (maxmin+1)/intrebinnum, (maxmax+1)/intrebinnum, 1);
 		c_samples_asymmetry->cd(5);
		TH2F *samp_max_hel1_rebin = (TH2F*)samp_max_hel1->Clone("samp_max_hel1_rebin");
 		samp_max_hel1_rebin->RebinX(intrebinnum);
 		TH1F* samp_max_hel1_rebin_sub = drawscalesubbytypeTH1(samp_max_hel1_rebin, 1, 2, (maxmin+1)/intrebinnum, (maxmax+1)/intrebinnum, 1);



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
		samp_max_hel0_rebin_sub->DrawCopy("e");
		c_samples_asymmetry->cd(8);
		samp_max_hel1_rebin_sub->DrawCopy("e");
		c_samples_asymmetry->cd(9);
 		TH1F *asym_bksub = (TH1F*) samp_max_hel1_rebin_sub->GetAsymmetry(samp_max_hel0_rebin_sub);
 		asym_bksub->SetMaximum(std::min(asym_bksub->GetMaximum(),0.06));
 		asym_bksub->SetMinimum(std::max(asym_bksub->GetMinimum(),-0.06));
 		asym_bksub->SetTitle("Asymmetry (background subtracted)");
		asym_bksub->SetLineColor(kRed);
 		asym_bksub->Draw();

		samples_asymmetry->Print(rundir + "samples_asymmetry.png");
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
 
