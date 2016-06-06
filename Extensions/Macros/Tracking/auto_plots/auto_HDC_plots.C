//Author: Anna Lee
//Date: Nov 18 2011
//Makes plots of hit pattern and drift time for the use of the shift crew  
//Edited: Nov 2013 to be able to save plots automatically in order to be used in 
//tracking pass5 replay -- Anna 
//Edited: May 2014 - now makes two sets of plots, one for all hits in a run, 
//and one for only the hits used in full tracks
//
#include "TH1D.h"
#include "TCanvas.h"
#include "TChain.h"
#include "QwEvent.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TList.h"
#include "TSystem.h"

void auto_HDC_plots(int runnum, Bool_t isFirst100K = kFALSE){

  //set prefex where the files will be written too
  TString PREFIX = Form(
      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
      runnum, runnum);

        // groups root files for a run together
	TChain* event_tree = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//define hit per plane histos
	TH1D* P1 = new TH1D ("P1","Pkg 1: All hits on Planes",14,0,13);
	TH1D* P2 = new TH1D ("P2","Pkg 1: Track hits on Planes",14,0,13);
	TH1D* P3 = new TH1D ("P3","Pkg 2: All hits on Planes",14,0,13);
	TH1D* P4 = new TH1D ("P4","Pkg 2: Track hits on Planes",14,0,13);

	//define hitspertrack histo
	TH1D* H1 = new TH1D ("H1","Pkg 1: Number of Hits per Track",14,0,13);
	TH1D* H2 = new TH1D ("H2","Pkg 2: Number of Hits per Track",14,0,13);

	//define the hit pattern histograms: trk required
	TH1D* E1 = new TH1D ("E1","Hokie 2: Package 2 Plane 1",34,0,33);
	TH1D* E2 = new TH1D ("E2","Hokie 2: Package 2 Plane 2",34,0,33);
	TH1D* E3 = new TH1D ("E3","Hokie 2: Package 2 Plane 3",34,0,33);
	TH1D* E4 = new TH1D ("E4","Hokie 2: Package 2 Plane 4",34,0,33);
	TH1D* E5 = new TH1D ("E5","Hokie 2: Package 2 Plane 5",34,0,33);
	TH1D* E6 = new TH1D ("E6","Hokie 2: Package 2 Plane 6",34,0,33);
	TH1D* E7 = new TH1D ("E7","Hokie 3: Package 2 Plane 7",34,0,33);
	TH1D* E8 = new TH1D ("E8","Hokie 3: Package 2 Plane 8",34,0,33);
	TH1D* E9 = new TH1D ("E9","Hokie 3: Package 2 Plane 9",34,0,33);
	TH1D* E10 = new TH1D ("E10","Hokie 3: Package 2 Plane 10",34,0,33);
	TH1D* E11 = new TH1D ("E11","Hokie 3: Package 2 Plane 11",34,0,33);
	TH1D* E12 = new TH1D ("E12","Hokie 3: Package 2 Plane 12",34,0,33);
	TH1D* E13 = new TH1D ("E13","Hokie 4: Package 1 Plane 1",34,0,33);
	TH1D* E14 = new TH1D ("E14","Hokie 4: Package 1 Plane 2",34,0,33);
	TH1D* E15 = new TH1D ("E15","Hokie 4: Package 1 Plane 3",34,0,33);
	TH1D* E16 = new TH1D ("E16","Hokie 4: Package 1 Plane 4",34,0,33);
	TH1D* E17 = new TH1D ("E17","Hokie 4: Package 1 Plane 5",34,0,33);
	TH1D* E18 = new TH1D ("E18","Hokie 4: Package 1 Plane 6",34,0,33);
	TH1D* E19 = new TH1D ("E19","Hokie 5: Package 1 Plane 7",34,0,33);
	TH1D* E20 = new TH1D ("E20","Hokie 5: Package 1 Plane 8",34,0,33);
	TH1D* E21 = new TH1D ("E21","Hokie 5: Package 1 Plane 9",34,0,33);
	TH1D* E22 = new TH1D ("E22","Hokie 5: Package 1 Plane 10",34,0,33);
	TH1D* E23 = new TH1D ("E23","Hokie 5: Package 1 Plane 11",34,0,33);
	TH1D* E24 = new TH1D ("E24","Hokie 5: Package 1 Plane 12",34,0,33);

	//define hit pattern histos for full run
	TH1D* F1 = new TH1D ("F1","Hokie 2: Package 2 Plane 1",34,0,33);
	TH1D* F2 = new TH1D ("F2","Hokie 2: Package 2 Plane 2",34,0,33);
	TH1D* F3 = new TH1D ("F3","Hokie 2: Package 2 Plane 3",34,0,33);
	TH1D* F4 = new TH1D ("F4","Hokie 2: Package 2 Plane 4",34,0,33);
	TH1D* F5 = new TH1D ("F5","Hokie 2: Package 2 Plane 5",34,0,33);
	TH1D* F6 = new TH1D ("F6","Hokie 2: Package 2 Plane 6",34,0,33);
	TH1D* F7 = new TH1D ("F7","Hokie 3: Package 2 Plane 7",34,0,33);
	TH1D* F8 = new TH1D ("F8","Hokie 3: Package 2 Plane 8",34,0,33);
	TH1D* F9 = new TH1D ("F9","Hokie 3: Package 2 Plane 9",34,0,33);
	TH1D* F10 = new TH1D ("F10","Hokie 3: Package 2 Plane 10",34,0,33);
	TH1D* F11 = new TH1D ("F11","Hokie 3: Package 2 Plane 11",34,0,33);
	TH1D* F12 = new TH1D ("F12","Hokie 3: Package 2 Plane 12",34,0,33);
	TH1D* F13 = new TH1D ("F13","Hokie 4: Package 1 Plane 1",34,0,33);
	TH1D* F14 = new TH1D ("F14","Hokie 4: Package 1 Plane 2",34,0,33);
	TH1D* F15 = new TH1D ("F15","Hokie 4: Package 1 Plane 3",34,0,33);
	TH1D* F16 = new TH1D ("F16","Hokie 4: Package 1 Plane 4",34,0,33);
	TH1D* F17 = new TH1D ("F17","Hokie 4: Package 1 Plane 5",34,0,33);
	TH1D* F18 = new TH1D ("F18","Hokie 4: Package 1 Plane 6",34,0,33);
	TH1D* F19 = new TH1D ("F19","Hokie 5: Package 1 Plane 7",34,0,33);
	TH1D* F20 = new TH1D ("F20","Hokie 5: Package 1 Plane 8",34,0,33);
	TH1D* F21 = new TH1D ("F21","Hokie 5: Package 1 Plane 9",34,0,33);
	TH1D* F22 = new TH1D ("F22","Hokie 5: Package 1 Plane 10",34,0,33);
	TH1D* F23 = new TH1D ("F23","Hokie 5: Package 1 Plane 11",34,0,33);
	TH1D* F24 = new TH1D ("F24","Hokie 5: Package 1 Plane 12",34,0,33);

	//Define the  drift time histograms: trk required
	TH1D* T1 = new TH1D ("T1","Hokie 2: fTimeNS",100,-100,400);
	TH1D* T2 = new TH1D ("T2","Hokie 3: fTimeNS",100,-100,400);
	TH1D* T3 = new TH1D ("T3","Hokie 4: fTimeNS",100,-100,400);
	TH1D* T4 = new TH1D ("T4","Hokie 5: fTimeNS",100,-100,400);
	TH1D* T5 = new TH1D ("T5","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
	TH1D* T6 = new TH1D ("T6","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
	TH1D* T7 = new TH1D ("T7","Hokie 3: fTimeNS-Log Scale",100,-100,1600);
	TH1D* T8 = new TH1D ("T8","Hokie 3: fTimeNS-Log Scale",100,-100,1600);

	//Define the drift time histos for full run
	TH1D* S1 = new TH1D ("S1","Hokie 2: fTimeNS",100,-100,400);
	TH1D* S2 = new TH1D ("S2","Hokie 3: fTimeNS",100,-100,400);
	TH1D* S3 = new TH1D ("S3","Hokie 4: fTimeNS",100,-100,400);
	TH1D* S4 = new TH1D ("S4","Hokie 5: fTimeNS",100,-100,400);
	TH1D* S5 = new TH1D ("S5","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
	TH1D* S6 = new TH1D ("S6","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
	TH1D* S7 = new TH1D ("S7","Hokie 3: fTimeNS-Log Scale",100,-100,1600);
	TH1D* S8 = new TH1D ("S8","Hokie 3: fTimeNS-Log Scale",100,-100,1600);


	//Create and Divide canvases
	TCanvas* c1 = new TCanvas("c1","Hit Patterns",1200,700);
	c1->Divide(6,4);
	//fElement plots are on c1

	TCanvas* c2 = new TCanvas("c2","Drift Times",1000,600);
	c2->Divide(4,2);
	//fTime plots are on c2  

	TCanvas* c3 = new TCanvas("c3","Hit Patterns: Full Run",1200,700);
	c3->Divide(6,4);
	//fElement plots are on c3

	TCanvas* c4 = new TCanvas("c4","Drift Times",1000,600);
	c4->Divide(4,2);
	//fTime plots are on c4  

	TCanvas* cplane = new TCanvas("cplane","Hits by Plane",900,900);
	cplane->Divide(2,2);

	TCanvas* chits = new TCanvas("chits","Hits per Track",900,450);
	chits->Divide(2,1);


	//Define axis
	E1->GetXaxis()->SetTitle("Wire");
	E2->GetXaxis()->SetTitle("Wire");
	E3->GetXaxis()->SetTitle("Wire");
	E4->GetXaxis()->SetTitle("Wire");
	E5->GetXaxis()->SetTitle("Wire");
	E6->GetXaxis()->SetTitle("Wire");
	E7->GetXaxis()->SetTitle("Wire");
	E8->GetXaxis()->SetTitle("Wire");
	E9->GetXaxis()->SetTitle("Wire");
	E10->GetXaxis()->SetTitle("Wire");
	E11->GetXaxis()->SetTitle("Wire");
	E12->GetXaxis()->SetTitle("Wire");
	E13->GetXaxis()->SetTitle("Wire");
	E14->GetXaxis()->SetTitle("Wire");
	E15->GetXaxis()->SetTitle("Wire");
	E16->GetXaxis()->SetTitle("Wire");
	E17->GetXaxis()->SetTitle("Wire");
	E18->GetXaxis()->SetTitle("Wire");
	E19->GetXaxis()->SetTitle("Wire");
	E20->GetXaxis()->SetTitle("Wire");
	E21->GetXaxis()->SetTitle("Wire");
	E22->GetXaxis()->SetTitle("Wire");
	E23->GetXaxis()->SetTitle("Wire");
	E24->GetXaxis()->SetTitle("Wire");

	F1->GetXaxis()->SetTitle("Wire");
	F2->GetXaxis()->SetTitle("Wire");
	F3->GetXaxis()->SetTitle("Wire");
	F4->GetXaxis()->SetTitle("Wire");
	F5->GetXaxis()->SetTitle("Wire");
	F6->GetXaxis()->SetTitle("Wire");
	F7->GetXaxis()->SetTitle("Wire");
	F8->GetXaxis()->SetTitle("Wire");
	F9->GetXaxis()->SetTitle("Wire");
	F10->GetXaxis()->SetTitle("Wire");
	F11->GetXaxis()->SetTitle("Wire");
	F12->GetXaxis()->SetTitle("Wire");
	F13->GetXaxis()->SetTitle("Wire");
	F14->GetXaxis()->SetTitle("Wire");
	F15->GetXaxis()->SetTitle("Wire");
	F16->GetXaxis()->SetTitle("Wire");
	F17->GetXaxis()->SetTitle("Wire");
	F18->GetXaxis()->SetTitle("Wire");
	F19->GetXaxis()->SetTitle("Wire");
	F20->GetXaxis()->SetTitle("Wire");
	F21->GetXaxis()->SetTitle("Wire");
	F22->GetXaxis()->SetTitle("Wire");
	F23->GetXaxis()->SetTitle("Wire");
	F24->GetXaxis()->SetTitle("Wire");

	T1->GetXaxis()->SetTitle("Drift Time (NS)");
	T2->GetXaxis()->SetTitle("Drift Time (NS)");
	T3->GetXaxis()->SetTitle("Drift Time (NS)");
	T4->GetXaxis()->SetTitle("Drift Time (NS)");
	T5->GetXaxis()->SetTitle("Drift Time (NS)");
	T6->GetXaxis()->SetTitle("Drift Time (NS)");
	T7->GetXaxis()->SetTitle("Drift Time (NS)");
	T8->GetXaxis()->SetTitle("Drift Time (NS)");

	S1->GetXaxis()->SetTitle("Drift Time (NS)");
	S2->GetXaxis()->SetTitle("Drift Time (NS)");
	S3->GetXaxis()->SetTitle("Drift Time (NS)");
	S4->GetXaxis()->SetTitle("Drift Time (NS)");
	S5->GetXaxis()->SetTitle("Drift Time (NS)");
	S6->GetXaxis()->SetTitle("Drift Time (NS)");
	S7->GetXaxis()->SetTitle("Drift Time (NS)");
	S8->GetXaxis()->SetTitle("Drift Time (NS)");

	P1->GetXaxis()->SetTitle("Plane");
	P2->GetXaxis()->SetTitle("Plane");
	P3->GetXaxis()->SetTitle("Plane");
	P4->GetXaxis()->SetTitle("Plane");

	P1->GetYaxis()->SetTitle("Hit");
	P2->GetYaxis()->SetTitle("Hit");
	P3->GetYaxis()->SetTitle("Hit");
	P4->GetYaxis()->SetTitle("Hit");

	H1->GetXaxis()->SetTitle("Number of Hits per Track");
	H2->GetXaxis()->SetTitle("Number of Hits per Track");

	//start looping over events
	//extract number of events
	Int_t nevents=event_tree->GetEntries();

	//Define QwEvent as a pointer?
	QwEvent* fEvent =0;

	//set the event_tree branch to be on
	event_tree->SetBranchStatus("events",1);

	//extract teh event branch
	TBranch* event_branch=event_tree->GetBranch("events");
	event_branch->SetAddress(&fEvent);

	int numhits = 0;
	//Loop through the events, filling histogram as you go
	cout << "Total Events: " << nevents << endl;
	for(int i=0; i<nevents;i++){
		//get event
		event_branch->GetEntry(i);

		// cross section weight for each event, or 1 for data
		double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

		//get number of hits

		int nhitsf = fEvent->GetNumberOfHits();
		for(int k=0;k<nhitsf;k++)
		{
			const QwHit* hit = fEvent->GetHit(k);
			if(hit->GetRegion()==2){
				if(hit->GetPackage()==2){
					P3->Fill(hit->GetPlane(),w);

					if(hit->GetPlane()==1){
						F1->Fill(hit->GetElement(),w);
						S1->Fill(hit->GetTimeNs(),w);
						S5->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==2){
						F2->Fill(hit->GetElement(),w);
						S1->Fill(hit->GetTimeNs(),w);
						S5->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==3){
						F3->Fill(hit->GetElement(),w);
						S1->Fill(hit->GetTimeNs(),w);
						S5->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==4){
						F4->Fill(hit->GetElement(),w);
						S1->Fill(hit->GetTimeNs(),w);
						S5->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==5){
						F5->Fill(hit->GetElement(),w);
						S1->Fill(hit->GetTimeNs(),w);
						S5->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==6){
						F6->Fill(hit->GetElement(),w);
						S1->Fill(hit->GetTimeNs(),w);
						S5->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==7){
						F7->Fill(hit->GetElement(),w);
						S2->Fill(hit->GetTimeNs(),w);
						S6->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==8){
						F8->Fill(hit->GetElement(),w);
						S2->Fill(hit->GetTimeNs(),w);
						S6->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==9){
						F9->Fill(hit->GetElement(),w);
						S2->Fill(hit->GetTimeNs(),w);
						S6->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==10){
						F10->Fill(hit->GetElement(),w);
						S2->Fill(hit->GetTimeNs(),w);
						S6->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==11){
						F11->Fill(hit->GetElement(),w);
						S2->Fill(hit->GetTimeNs(),w);
						S6->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==12){
						F12->Fill(hit->GetElement(),w);
						S2->Fill(hit->GetTimeNs(),w);
						S6->Fill(hit->GetTimeNs(),w);
					}
				}
				if(hit->GetPackage()==1){
					P1->Fill(hit->GetPlane(),w);

					if(hit->GetPlane()==1){
						F13->Fill(hit->GetElement(),w);
						S3->Fill(hit->GetTimeNs(),w);
						S7->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==2){
						F14->Fill(hit->GetElement(),w);
						S3->Fill(hit->GetTimeNs(),w);
						S7->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==3){
						F15->Fill(hit->GetElement(),w);
						S3->Fill(hit->GetTimeNs(),w);
						S7->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==4){
						F16->Fill(hit->GetElement(),w);
						S3->Fill(hit->GetTimeNs(),w);
						S7->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==5){
						F17->Fill(hit->GetElement(),w);
						S3->Fill(hit->GetTimeNs(),w);
						S7->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==6){
						F18->Fill(hit->GetElement(),w);
						S3->Fill(hit->GetTimeNs(),w);
						S7->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==7){
						F19->Fill(hit->GetElement(),w);
						S4->Fill(hit->GetTimeNs(),w);
						S8->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==8){
						F20->Fill(hit->GetElement(),w);
						S4->Fill(hit->GetTimeNs(),w);
						S8->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==9){
						F21->Fill(hit->GetElement(),w);
						S4->Fill(hit->GetTimeNs(),w);
						S8->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==10){
						F22->Fill(hit->GetElement(),w);
						S4->Fill(hit->GetTimeNs(),w);
						S8->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==11){
						F23->Fill(hit->GetElement(),w);
						S4->Fill(hit->GetTimeNs(),w);
						S8->Fill(hit->GetTimeNs(),w);
					}
					if(hit->GetPlane()==12){
						F24->Fill(hit->GetElement(),w);
						S4->Fill(hit->GetTimeNs(),w);
						S8->Fill(hit->GetTimeNs(),w);
					}
				}
			}
		}
		int ntrk = fEvent->GetNumberOfTracks();
		int hitspertrack1 = 0;
		int hitspertrack2 = 0;
		if(ntrk>0)
		{
			//loop over hits
			const QwTrack* fTrack = fEvent->GetTrack(0);
			int nPTs = fTrack->GetNumberOfPartialTracks();
			for (int b = 0; b < nPTs; b++)
			{
				const QwPartialTrack* fPt = fTrack->GetPartialTrack(b);
				if(fPt->GetRegion()==2){
					int nTLs = fPt->GetNumberOfTreeLines();
					for (int a = 0; a < nTLs; a++ )
					{
						const QwTreeLine* const_treeline = fPt->GetTreeLine(a);
						int nhits = const_treeline->GetNumberOfHits();
						for(int j=0;j<nhits;j++)
						{
							const QwHit* hit = const_treeline->GetHit(j);
							//      if(hit->GetRegion()==2){
							numhits++;
							if(hit->GetPackage()==2){
								P4->Fill(hit->GetPlane(),w);
								hitspertrack2++;
								if(hit->GetPlane()==1){
									E1->Fill(hit->GetElement(),w);
									T1->Fill(hit->GetTimeNs(),w);
									T5->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==2){
									E2->Fill(hit->GetElement(),w);
									T1->Fill(hit->GetTimeNs(),w);
									T5->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==3){
									E3->Fill(hit->GetElement(),w);
									T1->Fill(hit->GetTimeNs(),w);
									T5->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==4){
									E4->Fill(hit->GetElement(),w);
									T1->Fill(hit->GetTimeNs(),w);
									T5->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==5){
									E5->Fill(hit->GetElement(),w);
									T1->Fill(hit->GetTimeNs(),w);
									T5->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==6){
									E6->Fill(hit->GetElement(),w);
									T1->Fill(hit->GetTimeNs(),w);
									T5->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==7){
									E7->Fill(hit->GetElement(),w);
									T2->Fill(hit->GetTimeNs(),w);
									T6->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==8){
									E8->Fill(hit->GetElement(),w);
									T2->Fill(hit->GetTimeNs(),w);
									T6->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==9){
									E9->Fill(hit->GetElement(),w);
									T2->Fill(hit->GetTimeNs(),w);
									T6->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==10){
									E10->Fill(hit->GetElement(),w);
									T2->Fill(hit->GetTimeNs(),w);
									T6->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==11){
									E11->Fill(hit->GetElement(),w);
									T2->Fill(hit->GetTimeNs(),w);
									T6->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==12){
									E12->Fill(hit->GetElement(),w);
									T2->Fill(hit->GetTimeNs(),w);
									T6->Fill(hit->GetTimeNs(),w);
								}
							}
							if(hit->GetPackage()==1){
								hitspertrack1++;
								P2->Fill(hit->GetPlane(),w);

								if(hit->GetPlane()==1){
									E13->Fill(hit->GetElement(),w);
									T3->Fill(hit->GetTimeNs(),w);
									T7->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==2){
									E14->Fill(hit->GetElement(),w);
									T3->Fill(hit->GetTimeNs(),w);
									T7->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==3){
									E15->Fill(hit->GetElement(),w);
									T3->Fill(hit->GetTimeNs(),w);
									T7->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==4){
									E16->Fill(hit->GetElement(),w);
									T3->Fill(hit->GetTimeNs(),w);
									T7->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==5){
									E17->Fill(hit->GetElement(),w);
									T3->Fill(hit->GetTimeNs(),w);
									T7->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==6){
									E18->Fill(hit->GetElement(),w);
									T3->Fill(hit->GetTimeNs(),w);
									T7->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==7){
									E19->Fill(hit->GetElement(),w);
									T4->Fill(hit->GetTimeNs(),w);
									T8->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==8){
									E20->Fill(hit->GetElement(),w);
									T4->Fill(hit->GetTimeNs(),w);
									T8->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==9){
									E21->Fill(hit->GetElement(),w);
									T4->Fill(hit->GetTimeNs(),w);
									T8->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==10){
									E22->Fill(hit->GetElement(),w);
									T4->Fill(hit->GetTimeNs(),w);
									T8->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==11){
									E23->Fill(hit->GetElement(),w);
									T4->Fill(hit->GetTimeNs(),w);
									T8->Fill(hit->GetTimeNs(),w);
								}
								if(hit->GetPlane()==12){
									E24->Fill(hit->GetElement(),w);
									T4->Fill(hit->GetTimeNs(),w);
									T8->Fill(hit->GetTimeNs(),w);
								}     
							}
						}
						}  
					}
				}
			if (hitspertrack1!=0)  H1->Fill(hitspertrack1);
			if (hitspertrack2!=0)  H2->Fill(hitspertrack2);
			//cout << "Pkg 1:  " << hitspertrack1 << "   Pkg2: " << hitspertrack2 << endl;
			}
		}
		//Draw historgrams

		cout << "Number of hits this added to things: " << numhits << endl;
		c1->cd(1);
		E1->Draw();
		c1->cd(2);
		E2->Draw();
		c1->cd(3);
		E3->Draw();
		c1->cd(4);
		E4->Draw();
		c1->cd(5);
		E5->Draw();
		c1->cd(6);
		E6->Draw();
		c1->cd(7);
		E7->Draw();
		c1->cd(8);
		E8->Draw();
		c1->cd(9);
		E9->Draw();
		c1->cd(10);
		E10->Draw();
		c1->cd(11);
		E11->Draw();
		c1->cd(12);
		E12->Draw();
		c1->cd(13);
		E13->Draw();
		c1->cd(14);
		E14->Draw();
		c1->cd(15);
		E15->Draw();
		c1->cd(16);
		E16->Draw();
		c1->cd(17);
		E17->Draw();
		c1->cd(18);
		E18->Draw();
		c1->cd(19);
		E19->Draw();
		c1->cd(20);
		E20->Draw();
		c1->cd(21);
		E21->Draw();
		c1->cd(22);
		E22->Draw();
		c1->cd(23);
		E23->Draw();
		c1->cd(24);
		E24->Draw();

		c2->cd(1);
		T1->Draw();
		c2->cd(2);
		T2->Draw();
		c2->cd(3);
		T3->Draw();
		c2->cd(4);
		T4->Draw();
		c2->cd(5);
		T5->Draw();
		gPad->SetLogy(1);
		c2->cd(6);
		T6->Draw();
		gPad->SetLogy(1);
		c2->cd(7);
		T7->Draw();
		gPad->SetLogy(1);
		c2->cd(8);
		T8->Draw();
		gPad->SetLogy(1);

		c3->cd(1);
		F1->Draw();
		c3->cd(2);
		F2->Draw();
		c3->cd(3);
		F3->Draw();
		c3->cd(4);
		F4->Draw();
		c3->cd(5);
		F5->Draw();
		c3->cd(6);
		F6->Draw();
		c3->cd(7);
		F7->Draw();
		c3->cd(8);
		F8->Draw();
		c3->cd(9);
		F9->Draw();
		c3->cd(10);
		F10->Draw();
		c3->cd(11);
		F11->Draw();
		c3->cd(12);
		F12->Draw();
		c3->cd(13);
		F13->Draw();
		c3->cd(14);
		F14->Draw();
		c3->cd(15);
		F15->Draw();
		c3->cd(16);
		F16->Draw();
		c3->cd(17);
		F17->Draw();
		c3->cd(18);
		F18->Draw();
		c3->cd(19);
		F19->Draw();
		c3->cd(20);
		F20->Draw();
		c3->cd(21);
		F21->Draw();
		c3->cd(22);
		F22->Draw();
		c3->cd(23);
		F23->Draw();
		c3->cd(24);
		F24->Draw();

		c4->cd(1);
		S1->Draw();
		c4->cd(2);
		S2->Draw();
		c4->cd(3);
		S3->Draw();
		c4->cd(4);
		S4->Draw();
		c4->cd(5);
		S5->Draw();
		gPad->SetLogy(1);
		c4->cd(6);
		S6->Draw();
		gPad->SetLogy(1);
		c4->cd(7);
		S7->Draw();
		gPad->SetLogy(1);
		c4->cd(8);
		S8->Draw();
		gPad->SetLogy(1);

		cplane->cd(1);
		P1->Draw();
		cplane->cd(2);
		P2->Draw();
		cplane->cd(3);
		P3->Draw();
		cplane->cd(4);
		P4->Draw();

		chits->cd(1);
		H1->Draw();
		chits->cd(2);
		H2->Draw();

		//Save the plots
		c1->SaveAs(PREFIX+"trk_hit_patterns.png");
		c1->SaveAs(PREFIX+"trk_hit_patterns.C");
		c2->SaveAs(PREFIX+"trk_drift_times.png");
		c2->SaveAs(PREFIX+"trk_drift_times.C");
		c3->SaveAs(PREFIX+"full_hit_patterns.png");
		c3->SaveAs(PREFIX+"full_hit_patterns.C");
		c4->SaveAs(PREFIX+"full_drift_times.png");
		c4->SaveAs(PREFIX+"full_drift_times.C");
		cplane->SaveAs(PREFIX+"hdc_hits_plane.png");
		cplane->SaveAs(PREFIX+"hdc_hits_plane.C");
		chits->SaveAs(PREFIX+"hits_per_track.png");
		chits->SaveAs(PREFIX+"hits_per_track.C");

		return;  
	}
