/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs of drift time and drift distance in region 2 for each plane of wires, x,u,v

Each canvas is set up so that going across it is X (Red),U(Blue),V(Green) and Drift Distance is on the top and Drift Time is on the Bottom.

The first canvas is Package 1.
The second canvas is Package 2.

Plane numbers 
x - 1,4,7,10 or fPlan%3==1
u - 2,5,8,11 or fPlane%3==2
v - 3,6,9,12 or fPlane%3==0

Please note this is not coded the most efficently (okay not effiecently at all 
- should have vectors, arrays, loops, and speggiti looking coding),
but it show how far I have come :)

Entry Conditions: the run number, bool for first 100k
Date: 02-24-2012
Modified:07-11-2012
Assisted By: Juan Carlos
*********************************************************/

#include "auto_shared_includes.h"
#include "QwEvent.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void drift_distance_and_time(int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* event_tree = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form("$QWSCRATCH/tracking/www/run_%d/drift_distance_and_time_%d_",runnum, runnum);

	Prefix = Form(
	      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_drift_distance_and_time_",
	      runnum, runnum);

	//Create the canvas
	TCanvas c1("c1", "Drift Distance and Drift Time for Package 1", 600,700);

	//divide the canvas
	c1.Divide(3,2);

	//defines the 3 distance histograms
	TH1D* dX = new TH1D ("dX","X plane Drift Distance - Package 1",11,-0.1,0.7);
	TH1D* dU = new TH1D ("dU","U plane Drift Distance - Package 1",11,-0.1,0.7);
	TH1D* dV = new TH1D ("dV","V plane Drift Distance - Package 1",11,-0.1,0.7);

	//set line colors of all the histograms
	dX->SetLineColor(2);
	dU->SetLineColor(4);
	dV->SetLineColor(8);

	//define the 3 time histograms
	TH1D* tX = new TH1D ("tX","X plane Drift Time - Package 1",15,-100.0,400.0);
	TH1D* tU = new TH1D ("tU","U plane Drift Time - Package 1",15,-100.0,400.0);
	TH1D* tV = new TH1D ("tV","V plane Drift Time - Package 1",15,-100.0,400.0);

	//set line colors of all the histograms
	tX->SetLineColor(2);
	tU->SetLineColor(4);
	tV->SetLineColor(8);

	//defines the 3 distance histograms
	TH1D* dX2 = new TH1D ("dX2","X plane Drift Distance - Package 2",11,-0.1,0.7);
	TH1D* dU2 = new TH1D ("dU2","U plane Drift Distance - Package 2",11,-0.1,0.7);
	TH1D* dV2 = new TH1D ("dV2","V plane Drift Distance - Package 2",11,-0.1,0.7);

	//set line colors of all the histograms
	dX2->SetLineColor(2);
	dU2->SetLineColor(4);
	dV2->SetLineColor(8);

	//define the 3 time histograms
	TH1D* tX2 = new TH1D ("tX2","X plane Drift Time - Package 2",15,-100.0,400.0);
	TH1D* tU2 = new TH1D ("tU2","U plane Drift Time - Package 2",15,-100.0,400.0);
	TH1D* tV2 = new TH1D ("tV2","V plane Drift Time - Package 2",15,-100.0,400.0);

	//set line colors of all the histograms
	tX2->SetLineColor(2);
	tU2->SetLineColor(4);
	tV2->SetLineColor(8);

	//define all the axis for these histograms
	dX->GetYaxis()->SetTitle("frequency");
	dX->GetXaxis()->SetTitle("Drift Distance");
	dU->GetYaxis()->SetTitle("frequency");
	dU->GetXaxis()->SetTitle("Drift Distance");
	dV->GetYaxis()->SetTitle("frequency");
	dV->GetXaxis()->SetTitle("Drift Distance");
	dX2->GetYaxis()->SetTitle("frequency");
	dX2->GetXaxis()->SetTitle("Drift Distance");
	dU2->GetYaxis()->SetTitle("frequency");
	dU2->GetXaxis()->SetTitle("Drift Distance");
	dV2->GetYaxis()->SetTitle("frequency");
	dV2->GetXaxis()->SetTitle("Drift Distance");

	tX->GetYaxis()->SetTitle("frequency");
	tX->GetXaxis()->SetTitle("Drift Time");
	tU->GetYaxis()->SetTitle("frequency");
	tU->GetXaxis()->SetTitle("Drift Time");
	tV->GetYaxis()->SetTitle("frequency");
	tV->GetXaxis()->SetTitle("Drift Time");
	tX2->GetYaxis()->SetTitle("frequency");
	tX2->GetXaxis()->SetTitle("Drift Time");
	tU2->GetYaxis()->SetTitle("frequency");
	tU2->GetXaxis()->SetTitle("Drift Time");
	tV2->GetYaxis()->SetTitle("frequency");
	tV2->GetXaxis()->SetTitle("Drift Time");

	//start with looping over all the events and putting them in the correct histogram 

	//figure out how many evernts are in the rootfile so I know how long to have my loop go for
	Int_t nevents=event_tree->GetEntries();

	//To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
	QwEvent* fEvent = 0;
	
	//Now I have to get a pointer to the events branch to loop through

	//Start by setting the event_tree branch to be on
	event_tree->SetBranchStatus("events",1);

	//now get the event branch of the event_tree branch and call it event_branch creativly
	TBranch* event_branch=event_tree->GetBranch("events");
	event_branch->SetAddress(&fEvent);

	//Loop through this and fill all the graphs at once

	for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
	{
		 //Get the ith entry form the event tree
		event_branch->GetEntry(i);

		//get the number of Hits
		int nHits = fEvent->GetNumberOfHits();

	      // now let's loop through the tree lines and fill all the above histograms
		for (int t = 0; t < nHits; t++)
		{
			const QwHit* hit = fEvent->GetHit(t);

			//Start making the cuts needed to fill the histogram
			if (hit->GetRegion()==2)
			{
				if (hit->GetPackage()==1)
				{
					switch (hit->GetPlane() % 3) {
					case 1:
					{
						dX->Fill(hit->GetDriftDistance());
						tX->Fill(hit->GetTime());
						break;
					}
					case 2:
				  	{
						dU->Fill(hit->GetDriftDistance());
						tU->Fill(hit->GetTime());
						break;
					}
					case 0:
					{
						dV->Fill(hit->GetDriftDistance());
						tV->Fill(hit->GetTime());
						break;
					}
					default:break;
					}
				} else if (hit->GetPackage()==2)
				{
					switch (hit->GetPlane() % 3) {
					case 1:
					{
						dX2->Fill(hit->GetDriftDistance());
						tX2->Fill(hit->GetTime());
						break;
					}
					case 2:
				  	{
						dU2->Fill(hit->GetDriftDistance());
						tU2->Fill(hit->GetTime());
						break;
					}
					case 0:
					{
						dV2->Fill(hit->GetDriftDistance());
						tV2->Fill(hit->GetTime());
						break;
					}
					default:break;
					}

				}

			}

		}

	}

	//start with graphing the distances - they go across the top
	//select pad 1
	c1.cd(1);

	//draw the drift distance in the x plane
	//event_tree->Draw("events.fQwHits.fDistance>>dX","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==1");
	dX->Draw();

	//select pad 2
	c1.cd(2);

	//draw the drift distance in the u plane
	//event_tree->Draw("events.fQwHits.fDistance>>dU","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==2");
	dU->Draw();

	//select pad 3
	c1.cd(3);

	//draw the drift distance in the v plane
	//event_tree->Draw("events.fQwHits.fDistance>>dV","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==0");
	dV->Draw();

	//Now let's graph the times - they go across the bottom
	//select pad 4
	c1.cd(4);

	//draw the drift time in the x plane
	//event_tree->Draw("events.fQwHits.fTimeNs>>tX","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==1");
	tX->Draw();

	//select pad 5
	c1.cd(5);

	//draw the drift time in the u plane
	//event_tree->Draw("events.fQwHits.fTimeNs>>tU","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==2");
	tU->Draw();

	//select pad 6
	c1.cd(6);

	//draw the drift time in the v plane
	//event_tree->Draw("events.fQwHits.fTimeNs>>tV","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==0");
	tV->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"Package_1.png");
	c1.SaveAs(Prefix+"Package_1.C");

	//Create the canvas - for package 2
	TCanvas c2("c2", "Drift Distance and Drift Time for Package 2", 600,700);

	//divide the canvas
	c2.Divide(3,2);

	//start with graphing the distances - they go across the top
	//select pad 1
	c2.cd(1);

	//draw the drift distance in the x plane
	//event_tree->Draw("events.fQwHits.fDistance>>dX2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==1");
	dX2->Draw();

	//select pad 2
	c2.cd(2);

	//draw the drift distance in the u plane
	//event_tree->Draw("events.fQwHits.fDistance>>dU2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==2");
	dU2->Draw();

	//select pad 3
	c2.cd(3);

	//draw the drift distance in the v plane
	//event_tree->Draw("events.fQwHits.fDistance>>dV2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==0");
	dV2->Draw();

	//Now let's graph the times - they go across the bottom
	//select pad 4
	c2.cd(4);

	//draw the drift time in the x plane
	//event_tree->Draw("events.fQwHits.fTimeNs>>tX2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==1");
	tX2->Draw();

	//select pad 5
	c2.cd(5);

	//draw the drift time in the u plane
	//event_tree->Draw("events.fQwHits.fTimeNs>>tU2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==2");
	tU2->Draw();

	//select pad 6
	c2.cd(6);

	//draw the drift time in the v plane
	//event_tree->Draw("events.fQwHits.fTimeNs>>tV2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==0");
	tV2->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c2.SaveAs(Prefix+"Package_2.png");
	c2.SaveAs(Prefix+"Package_2.C");

	return;

}
