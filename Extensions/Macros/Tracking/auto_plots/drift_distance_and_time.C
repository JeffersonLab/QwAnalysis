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

Entry Conditions: the run number, bool for first 100k
Date: 02-24-2012
Modified:02-26-2012
Assisted By: Juan Carlos
*********************************************************/

#include "auto_shared_includes.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void drift_distance_and_time(int runnum, bool is100k)
{

// groups root files for a run together
TChain* event_tree = new TChain ("event_tree");

//add the root files to the chain the event_tree branches
event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

//deifne the prefix as the directory that the files will be outputed to
Prefix = Form("$QWSCRATCH/tracking/www/run_%d/drift_distance_and_time_%d_",runnum, runnum);



//Create the canvas
TCanvas c1("c1", "Drift Distance and Drift Time for Package 1", 900,1000);

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

//start with graphing the distances - they go across the top
//select pad 1
c1.cd(1);

//draw the drift distance in the x plane
event_tree->Draw("events.fQwHits.fDistance>>dX","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==1");

//select pad 2
c1.cd(2);

//draw the drift distance in the u plane
event_tree->Draw("events.fQwHits.fDistance>>dU","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==2");

//select pad 3
c1.cd(3);

//draw the drift distance in the v plane
event_tree->Draw("events.fQwHits.fDistance>>dV","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==0");


//Now let's graph the times - they go across the bottom
//select pad 4
c1.cd(4);

//draw the drift time in the x plane
event_tree->Draw("events.fQwHits.fTime>>tX","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==1");

//select pad 5
c1.cd(5);

//draw the drift time in the u plane
event_tree->Draw("events.fQwHits.fTime>>tU","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==2");

//select pad 6
c1.cd(6);

//draw the drift time in the v plane
event_tree->Draw("events.fQwHits.fTime>>tV","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==1&&events.fQwHits.fPlane%3==0");

//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c1.SaveAs(Prefix+"Package_1.png");

//Create the canvas - for package 2
TCanvas c2("c2", "Drift Distance and Drift Time for Package 2", 900,1000);

//divide the canvas
c2.Divide(3,2);

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

//start with graphing the distances - they go across the top
//select pad 1
c2.cd(1);

//draw the drift distance in the x plane
event_tree->Draw("events.fQwHits.fDistance>>dX2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==1");

//select pad 2
c2.cd(2);

//draw the drift distance in the u plane
event_tree->Draw("events.fQwHits.fDistance>>dU2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==2");

//select pad 3
c2.cd(3);

//draw the drift distance in the v plane
event_tree->Draw("events.fQwHits.fDistance>>dV2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==0");


//Now let's graph the times - they go across the bottom
//select pad 4
c2.cd(4);

//draw the drift time in the x plane
event_tree->Draw("events.fQwHits.fTime>>tX2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==1");

//select pad 5
c2.cd(5);

//draw the drift time in the u plane
event_tree->Draw("events.fQwHits.fTime>>tU2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==2");

//select pad 6
c2.cd(6);

//draw the drift time in the v plane
event_tree->Draw("events.fQwHits.fTime>>tV2","events.fQwHits.fRegion==2&&events.fQwHits.fPackage==2&&events.fQwHits.fPlane%3==0");


//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c1.SaveAs(Prefix+"Package_2.png");

return;

}
