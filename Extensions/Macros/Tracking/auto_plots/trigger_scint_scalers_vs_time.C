/*********************************************************** 
Programmer: Valerie Gray
Purpose: To graph the two trigger scintillator scaler values vs time as event number
Entry Conditions: the run number
Date: 02-03-2012
Modified:02-07-2012
*********************************************************/

#include "auto_shared_includes.h"

void trigger_scint_scalers_vs_time(int runnum, bool is100k)
{

// groups root files for a run together
TChain* chain = new TChain ("event_tree");

//add the root files to the chain the Mps_Tree branches
chain->Add(Form("$QW_ROOTFILES/Qweak_%d*.root",runnum));


//Create a canvas
TCanvas c1 ( "c1", "Trigger Scintillator Vs. Event Number", 900, 800);

//Divide that canvas into 2 rows and 1 columns
c1.Divide(1,2);


for(int i =1; i<=2; i++)
	{
		//select pad 1
		c1.cd(i);

		//Trigger scintilator vs. Event number - 
		/*Form(trigscint.ts%dmt_sca:CodaEventNumber","trigscint.ts%dmt_sca>0",i,i) - a function to 
		insert the variable, i in to the string, where one wants it the int is represtented by a %d */		
		
		//Note I am not sure that the data is getting printed here however the print statment is working
		
		chain->Draw(Form("trigscint.ts%ddmt_sca:CodaEventNumber>>v%d",i,i),Form("trigscint.ts%ddmt_sca>0",i));
		
		//cout <<"I printed " << i <<endl;

	}

//save the canvas as a png file - right now it goes to the rootfile directory
c1.SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/trigger_scint_scalers_vs_time_%d.png",runnum,runnum));

return;

}
