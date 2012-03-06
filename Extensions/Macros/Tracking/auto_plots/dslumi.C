/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs needed from the dslumi data
To graph all 8 dslumi rates vs. time on one canvas

To graph all 4 dslumi rates ratios vs. time on one canvas - the ratios are, 1/5, 2/6, 3/7, 4/8, octants that are across from each other

To graph the sum of all 8 dslumi rates vs. time on one canvas

Entry Conditions: the run number, bool for first 100k
Date: 02-07-2012
Modified:
Assisted By: Juan Carlos
*********************************************************/

#include "auto_shared_includes.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

//define functions first so that they work :)
void dslumi_vs_time(TChain* Mps_Tree);
void dslumi_ratios_vs_time (TChain* Mps_Tree);
void dslumi_sum_vs_time(TChain* Mps_Tree);


void dslumi(int runnum, bool is100k)
{

// groups root files for a run together
TChain* chain = new TChain ("Mps_Tree");

//add the root files to the chain the Mps_Tree branches
chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

//deifne the prefix as the directory that the files will be outputed to
Prefix = Form("$QWSCRATCH/tracking/www/run_%d/dslumi_%d_",runnum, runnum);

dslumi_vs_time(chain);

dslumi_ratios_vs_time(chain);

dslumi_sum_vs_time(chain);

return;
}

/*********************************************************** 
Function:dalumi_vs_time
Purpose: To graph all 8 dslumi rates vs. time on one canvas
Entry Conditions: TChain - Mps_Tree
Global: Prefix - the prefix for the output file
Called By: dslumi_vs_time
Date: 02-03-2012
Modified:
*********************************************************/

void dslumi_vs_time(TChain* Mps_Tree)
{

//Create a canvas
TCanvas c1( "c1", "dslumi rate vs. mps counter", 900, 1000);

//Divide that canvas into 8 rows and 1 columns
c1.Divide(1,8);

//loop over all 8 dslumis and put each graph in a differnt pad
for (int i = 1; i <=8; i++)
	{
		//select pad i
	  	c1.cd(i);

	  	//Graph the ith lumi rate vs time
	  	/*Form("sca_dslumi%d.value:mps_counter",i) - a function to 
	  	insert the variable, i in to the string, where one wants it
	  	the int is represtented by a %d */
	  
	  	Mps_Tree->Draw(Form("sca_dslumi%d.value:mps_counter",i));
		
		//cout <<"I printed " << i <<endl;
	}

//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c1.SaveAs(Prefix+"vs_time.png");

return;
}

/*********************************************************** 
Function: dslumi_ratios_vs_time
Purpose: To graph all 4 dslumi rates ratios vs. time on one canvas - the ratios are, 1/5, 2/6, 3/7, 4/8, octants that are across from each other
Entry Conditions: TChain - Mps_Tree
Global: Prefix - the prefix for the output file
Called by: dslumi_ratios_vs_time
Date: 02-03-2012
Modified:
*********************************************************/


void dslumi_ratios_vs_time(TChain* Mps_Tree)
{


//Create a canvas
TCanvas c2 ( "c2", "dslumi ratios vs. mps counter", 900, 800);

//Divide that canvas into 8 rows and 1 columns
c2.Divide(1,4);

for(int i =1; i<=4;i++)
	{

	   	//select pad i
	   	c2.cd(i);

	   	//Graph the i th dslumi rate/(i+4)th dslumi rate vs time 
	   	/*Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4) - a function to 
	   	insert the variable, i in to the string, where one wants it
	   	the int is represtented by a %d */ 
	   
	   	Mps_Tree->Draw(Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4));
		
		//cout <<"I printed " << i <<endl;
	}

//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c2.SaveAs(Prefix+"ratios_vs_time.png");

return;
}

/*********************************************************** 
Function: dslumi_sum_time
Purpose: To graph the sum of all 8 dslumi rates vs. time on one canvas
Entry Conditions: TChain Mps_Tree
Global: Prefix - the prefix for the output file
Called by: dslumi_ratios_vs_time
Date: 02-03-2012
Modified:
*********************************************************/

void dslumi_sum_vs_time(TChain* Mps_Tree)
{

//Create a canvas
TCanvas c3 ( "c3", "dslumi rate sum vs. mps counter", 900, 900);

//Graph the sum lumi rate vs time - 
Mps_Tree->Draw("(sca_dslumi1.value+sca_dslumi2.value+sca_dslumi3.value+sca_dslumi4.value+sca_dslumi5.value+sca_dslumi6.value+sca_dslumi7.value+sca_dslumi8.value):mps_counter");

//cout <<"I printed " << endl;

//save the canvas as a png file - right now it goes to the rootfile directory
c3.SaveAs(Prefix+"sum_vs_time.png");

return;

}
