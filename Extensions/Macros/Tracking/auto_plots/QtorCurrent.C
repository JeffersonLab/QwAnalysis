/*********************************************************** 
Programmer: Valerie Gray
Purpose: To graph the QTOR currnt as a function of time.

Entry Conditions: the run number, bool for first 100k
Date: 06-28-2012
Modified: 06-30-2012
Assisted By:
*********************************************************/

#include "auto_shared_includes.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void QtorCurrent(int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* chain = new TChain ("Slow_Tree");

	//add the root files to the chain the Slow_Tree branches
	chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	Prefix = Form("$QWSCRATCH/tracking/www/run_%d/QTOR_%d_",runnum, runnum);

	//Create a canvas
	TCanvas c1( "c1", "QTOR current vs. Event", 1000, 400);

	//Here the "L" draws a line through the point on the graph
	//one needs to place "" in the second place of the draw command (the cuts) part so that 
	//root recognizes that that the L is not a cut but a draw option and then root :)s
	chain->Draw("qw_qt_mps_i_set:Entry$","" ,"L");

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"current_vs_time.png");

	return;
}


