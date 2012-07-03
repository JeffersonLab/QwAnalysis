/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs to determine the octnat number 

- the graphs are outbut in the same arragment as the detector bars were set up in 

First output the main detector bar signal vs. tigger scintilltor signal in each 
octant in the same arrangment that the detector bars are set up in for each package in region 3.  In the 
center there is the a graph of the octant that was fed to the analyzer for the package of the canvas.

First output the main detector bar signal in each octant in the same arrangment that the 
detector bars are set up in for each package in region 3.  In the center there is the a 
graph of the octant that was fed to the analyzer for the package of the canvas.

Entry Conditions: the run number, bool for first 100k
Date: 06-13-2012
Modified: 60-29-2012
Assisted By: Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

int octant_to_pad[9] = {5, 4, 1, 2, 3, 6, 9, 8, 7};

void OctantDetermination(int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* chain = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	Prefix = Form("$QWSCRATCH/tracking/www/run_%d/OctantDetermination_%d_",runnum, runnum);

	for (int pkg = 1; pkg <= 2; pkg++)
	{
		//Create the canvas
		TCanvas c1 ("c1", Form("Octant Determination - for Package %d",pkg), 600,600);

		//divide the canvas
		c1.Divide(3,3);

		//for the first 4 octant draw what is wanted the main dectctor bar singnals vs trigger scintllator signals
		for (int oct1 = 1; oct1 <=8; oct1 ++)
		{
			//select the pad we want to draw on
			c1.cd(octant_to_pad[oct1]);

			//draw what I want - the oct depenece from main detector info 
                	chain->Draw(Form("maindet.md%dm_adc+maindet.md%dp_adc:trigscint.ts%dm_adc+trigscint.ts%dp_adc",oct1, oct1, pkg, pkg));
	
		}

		//selesct the center pad that we want to draw on
		c1.cd(5);
	
		// draw what we want the fQwHits.fOctant for region 3 and the package we are in
		chain->Draw("events.fQwTreeLines.fOctant",Form("events.fQwTreeLines.fPackage==%d && events.fQwTreeLines.fRegion==3", pkg));

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c1.SaveAs(Prefix+Form("Main_Detector_vs_Trigger_Scint_Package_%d.png",pkg));
	}

	for (int reg = 2 ; reg<=3 ;reg ++) 
	{

		for (int pkg2 = 1; pkg2 <= 2; pkg2++)
		{
        		//Create the canvas
        		TCanvas c2("c2", Form("Main detector signal for each oct for Region %d Package %d", reg, pkg2), 600,600);

        		//divide the canvas
        		c2.Divide(3,3);

			//for the first 4 octant draw what is wanted the main dectctor bar singnals vs trigger scintllator signals
        		for (int oct3 = 1; oct3 <=8; oct3 ++)
        		{
                		//select the pad we want to draw on
                		c2.cd(octant_to_pad[oct3]);

                		//draw what I want - the oct depenece from main detector info
                		//the use of qoutes here in the second and third line of this command is 
				//because the qoutes signify the closing of a string which is needed in 
				//wrapping commands
				chain->Draw(Form("maindet.md%dm_adc+maindet.md%dp_adc",oct3, oct3), 
                                            Form("events.fQwTreeLines.fRegion==%d" 
                                            "&& events.fQwTreeLines.fPackage== %d",reg, pkg2));

        		}

			//selesct the center pad that we want to draw on
                	c2.cd(5);

                	// draw what we want the fQwHits.fOctant for region 3 and the package we are in
                	chain->Draw("events.fQwTreeLines.fOctant",Form("events.fQwTreeLines.fPackage==%d && events.fQwTreeLines.fRegion==%d", pkg2, reg));

        		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
       			c2.SaveAs(Prefix+Form("Main_Detector_Region_%d_Package_%d.png", reg, pkg2));
		}	

	}

}
