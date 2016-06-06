/*********************************************************** 
Programmer: Valerie Gray
Purpose: To graph all 4 dslumi rates ratios vs. time on one canvas - the ratios are, 1/5, 2/6, 3/7, 4/8, octants that are across from each other
Entry Conditions: the run number
Date: 02-03-2012
Modified:
*********************************************************/

#include "auto_shared_includes.h"

void dslumi_ratios_vs_time(int runnum, bool is100k)
{

// groups root files for a run together
TChain* chain = new TChain ("Mps_Tree");

//add the root files to the chain the Mps_Tree branches
chain->Add(Form("$QW_ROOTFILES/Qweak_%d*.root",runnum));


//Create a canvas
TCanvas c1( "c1", "dslumi ratios vs. mps counter", 900, 800);

//Divide that canvas into 8 rows and 1 columns
c1.Divide(1,4);

for(int i =1; i<=4;i++)
	{

	   	//select pad i
	   	c1.cd(i);

	   	//Graph the i th dslumi rate/(i+4)th dslumi rate vs time 
	   	/*Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4) - a function to 
	   	insert the variable, i in to the string, where one wants it
	   	the int is represtented by a %d */ 
	   
	   	chain->Draw(Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4));
		
		//cout <<"I printed " << i <<endl;
	}

//save the canvas as a png file - right now it goes to the rootfile directory
c1.SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/dslumi_ratios_vs_time_%d.png",runnum, runnum));
c1.SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/dslumi_ratios_vs_time_%d.C",runnum, runnum));

return;
}
