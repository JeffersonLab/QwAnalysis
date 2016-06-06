/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs needed from the dslumi data
To graph all 8 dslumi rates vs. time on one canvas

To graph all 4 dslumi rates ratios vs. time on one canvas - the ratios are, 1/5, 2/6, 3/7, 4/8, octants that are across from each other

To graph the sum of all 8 dslumi rates vs. time on one canvas

NOTE: as of 07/17/2012 the functions do not work the code was rewritten 
      and they were not made to work.  I messsed with the code and broke it and 
      the fixing took pklace in the main function

Entry Conditions: the run number, bool for first 100k
Date: 02-07-2012
Modified: 07-17-2012
Assisted By: Juan Carlos Cornejo and Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"
#include "TLeaf.h"
#include "TH2D.h"
#include <iostream>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

//define functions first so that they work :)
//void dslumi_vs_time(TChain* Mps_Tree);
//void dslumi_ratios_vs_time (TChain* Mps_Tree);
//void dslumi_sum_vs_time(TChain* Mps_Tree);


void dslumi(int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* Mps_Tree = new TChain ("Mps_Tree");

	//add the root files to the chain the Mps_Tree branches
	Mps_Tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	///deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form("$QWSCRATCH/tracking/www/run_%d/dslumi_%d_",runnum, runnum);

	Prefix = Form(
	      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_dslumi_",
	      runnum, runnum);

	//create a vector of 2-D histograms of size 9 and define it
        std::vector<TH2D*> h;
	h.resize(9);
	for (size_t j = 1; j<h.size();j++)
	{
		h[j]= new TH2D (Form("h[%d]",j),Form("dslumi %d rate vs time",j),300,1,0,30,1,0);
	        h[j]->GetYaxis()->SetTitle(Form("dslumi %d rate",j));
        	h[j]->GetXaxis()->SetTitle("Time");
	}
	//create a vector of 2-D histograms of size 9 and define it
        std::vector<TH2D*>  h2;
	h2.resize(5);
	for (size_t j = 1; j<h2.size();j++)
	{
		h2[j]= new TH2D (Form("h2[%d]",j),Form("sca_dslumi%d.value/sca_dslumi%d.value vs time",j,j+4),30,1.0,0.0,30,1.0,0.0);
	        h2[j]->GetYaxis()->SetTitle(Form("dslumi %d rate/dslumi %d rate",j,j+4));
        	h2[j]->GetXaxis()->SetTitle("Time");

	}

	//create a 2-D histogram of define it
	TH2D* h3 = new TH2D ("h3","dslumi rate sum  vs time",30,1.0,0.0,30,1.0,0.0);
        h3->GetYaxis()->SetTitle("dslumi sum rate");
    	h3->GetXaxis()->SetTitle("Time");


//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = Mps_Tree->GetEntries();

        //Now I have to get a pointer to the events branch to loop through

	Double_t mps_counter;
	Double_t CodaEventType;
	Double_t CodaEventNumber;
	std::vector<TLeaf*> dslumi;  //[octant]
	dslumi.resize(9);

	 //selects the leaf that we are going to need
	Mps_Tree->SetBranchAddress("mps_counter",&mps_counter);
	Mps_Tree->SetBranchAddress("CodaEventType",&CodaEventType);
	Mps_Tree->SetBranchAddress("CodaEventNumber",&CodaEventNumber);
	for (size_t u = 1; u < dslumi.size(); u++)
	{
		TBranch* branch = Mps_Tree->GetBranch(Form("sca_dslumi%d",u));
		dslumi[u] = branch->GetLeaf("value");
	}

        //Loop through this and fill all the graphs at once
        for (int i = 0; i < nevents ; i++) 
        {
                //Get the ith entry form the event tree
		Mps_Tree->GetEntry(i);
		if (CodaEventType != 1) continue;

		//cout << mps_counter << ": " << dslumi[1] << endl;
	
		//Fill histograms
		for (size_t q = 1; q < dslumi.size(); q++)
		{
			h[q]->Fill(mps_counter,dslumi[q]->GetValue());
		} 

		for (size_t q = 1; q < 5; q++)
		{
			h2[q]->Fill(mps_counter,dslumi[q]->GetValue()/dslumi[q+4]->GetValue());
		}

		//Fill histogram
		h3->Fill(mps_counter,dslumi[1]->GetValue()+dslumi[2]->GetValue()+dslumi[3]->GetValue()+dslumi[4]->GetValue()
			+dslumi[5]->GetValue()+dslumi[6]->GetValue()+dslumi[7]->GetValue()+dslumi[8]->GetValue());
		
	}


	//Create a canvas
	TCanvas* c1 = new TCanvas( "c1", "dslumi rate vs. mps counter", 1000, 900);

	//Divide that canvas into 8 rows and 1 columns
	c1->Divide(1,8);

	//loop over all 8 dslumis and put each graph in a differnt pad
	for (int i = 1; i <=8; i++)
		{
			//select pad i
	  		c1->cd(i);

		  	//Graph the ith lumi rate vs time
		  	/*Form("sca_dslumi%d.value:mps_counter",i) - a function to 
		  	insert the variable, i in to the string, where one wants it
		  	the int is represtented by a %d */

			h[i]->Draw();
	  
//		  	Mps_Tree->Draw(Form("sca_dslumi%d.value:mps_counter",i));
		
			//cout <<"I printed " << i <<endl;
		}

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1->SaveAs(Prefix+"vs_time.png");
	c1->SaveAs(Prefix+"vs_time.C");

	delete c1;


	//Create a canvas
	TCanvas* c2 = new TCanvas ( "c2", "dslumi ratios vs. mps counter", 500, 500);

	//Divide that canvas into 8 rows and 1 columns
	c2->Divide(1,4);

	for(int i =1; i<=4;i++)
		{

	   		//select pad i
	   		c2->cd(i);

		   	//Graph the i th dslumi rate/(i+4)th dslumi rate vs time 
		   	/*Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4) - a function to 
		   	insert the variable, i in to the string, where one wants it
		   	the int is represtented by a %d */ 
	   
			h2[i]->Draw();
//		   	Mps_Tree->Draw(Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4));
		
			//cout <<"I printed " << i <<endl;
		}

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c2->SaveAs(Prefix+"ratios_vs_time.png");
	c2->SaveAs(Prefix+"ratios_vs_time.C");

	delete c2;


	//Create a canvas
	TCanvas* c3 = new TCanvas ( "c3", "dslumi rate sum vs. mps counter", 400, 500);

	//Graph the sum lumi rate vs time - 
//	Mps_Tree->Draw("(sca_dslumi1.value+sca_dslumi2.value+sca_dslumi3.value+sca_dslumi4.value+sca_dslumi5.value+sca_dslumi6.value+sca_dslumi7.value+sca_dslumi8.value):mps_counter");

	h3->Draw();

	//cout <<"I printed " << endl;

	//save the canvas as a png file - right now it goes to the rootfile directory
	c3->SaveAs(Prefix+"sum_vs_time.png");
	c3->SaveAs(Prefix+"sum_vs_time.C");

	delete c3;


	for (size_t i = 0 ; i < h.size() ; i++) 
	{
		delete h[i];
		h[i] = 0;
	}

	for (size_t i = 0 ; i < h2.size() ; i++) 
	{
		delete h2[i];
		h2[i] = 0;
	}

	delete h3;

	delete Mps_Tree;

//	dslumi_vs_time(chain);

//	dslumi_ratios_vs_time(chain);

//	dslumi_sum_vs_time(chain);

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
/*
void dslumi_vs_time(TChain* Mps_Tree)
{
	//create a vector of 2-D histograms of size 9 and define it
        std::vector<TH2D*> h;
	h.resize(9);
	for (int j =0; j<9;j++)
	{
		h[j]= new TH2D (Form("h[%d]",j),Form("dslumi %d rate vs time",j),300,1,0,30,1,0);
	        h[j]->GetYaxis()->SetTitle(Form("dslumi %d rate",j));
        	h[j]->GetXaxis()->SetTitle("Time");
	}
//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = Mps_Tree->GetEntries();

        //Now I have to get a pointer to the events branch to loop through

	Double_t mps_counter;
	Double_t CodaEventType;
	Double_t CodaEventNumber;
	std::vector<Double_t> dslumi;  //[octant]
	dslumi.resize(9);

	 //selects the leaf that we are going to need
	Mps_Tree->SetBranchAddress("mps_counter",&mps_counter);
	Mps_Tree->SetBranchAddress("CodaEventType",&CodaEventType);
	Mps_Tree->SetBranchAddress("CodaEventNumber",&CodaEventNumber);
	for (int u = 1; u < 9; u++)
	{
		Mps_Tree->SetBranchAddress(Form("sca_dslumi%d",u),&(dslumi[u]));
	}

        //Loop through this and fill all the graphs at once

        for (int i = 0; i < nevents ; i++) 
        {
                //Get the ith entry form the event tree
		Mps_Tree->GetEntry(i);
		if (CodaEventType != 1) continue;

		cout << mps_counter << ": " << dslumi[1] << endl;
	
		//Fill histograms
		for (int q = 1; q < 9; q++)
		{
			h[q]->Fill(mps_counter,dslumi[q]);
		} 
		
	}

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
*/		  	/*Form("sca_dslumi%d.value:mps_counter",i) - a function to 
		  	insert the variable, i in to the string, where one wants it
		  	the int is represtented by a %d */
/*
			h[i]->Draw();
	  
//		  	Mps_Tree->Draw(Form("sca_dslumi%d.value:mps_counter",i));
		
			//cout <<"I printed " << i <<endl;
		}

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"vs_time.png");
	c1.SaveAs(Prefix+"vs_time.C");

	for (int i = 0 ; i <= 8 ; i++) {
		delete h[i];
		h[i] = 0;
	}

	return;
}
*/
/*********************************************************** 
Function: dslumi_ratios_vs_time
Purpose: To graph all 4 dslumi rates ratios vs. time on one canvas - the ratios are, 1/5, 2/6, 3/7, 4/8, octants that are across from each other
Entry Conditions: TChain - Mps_Tree
Global: Prefix - the prefix for the output file
Called by: dslumi_ratios_vs_time
Date: 02-03-2012
Modified:
*********************************************************/

/*
void dslumi_ratios_vs_time(TChain* Mps_Tree)
{
	//create a vector of 2-D histograms of size 9 and define it
        std::vector<TH2D*>  h2;
	h2.resize(5);
	for (int j =0; j<5;j++)
	{
		h2[j]= new TH2D (Form("h2[%d]",j),Form("sca_dslumi%d.value/sca_dslumi%d.value vs time",j,j+4),30,1.0,0.0,30,1.0,0.0);
	        h2[j]->GetYaxis()->SetTitle(Form("dslumi %d rate/dslumi %d rate",j,j+4));
        	h2[j]->GetXaxis()->SetTitle("Time");

	}
//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents=Mps_Tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
//        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        Mps_Tree->SetBranchStatus("Mps_Tree",1);
        
	std:: vector<TBranch*> dslumi;  //[octant]
	dslumi.resize(9);

	//selects the leaf that we are going to need
	for (int u = 1; u < 9; u++)
	{
		dslumi[u]=Mps_Tree->GetBranch(Form("sca_dslumi%d",u));
	}
        //Loop through this and fill all the graphs at once

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
		Mps_Tree->GetEntry(i);
	
		//Fill histograms
		for (int q = 1; q < 5; q++)
		{
			h2[q]->Fill(dslumi[q]->GetValue()/dslumi[q+4]->GetValue(),mps_counter);
		} 
		
	}

	//Create a canvas
	TCanvas c2 ( "c2", "dslumi ratios vs. mps counter", 900, 800);

	//Divide that canvas into 8 rows and 1 columns
	c2.Divide(1,4);

	for(int i =1; i<=4;i++)
		{

	   		//select pad i
	   		c2.cd(i);

		   	//Graph the i th dslumi rate/(i+4)th dslumi rate vs time 
*/		   	/*Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4) - a function to 
		   	insert the variable, i in to the string, where one wants it
		   	the int is represtented by a %d */ 
/*	   
			h2[i]->Draw();
//		   	Mps_Tree->Draw(Form("sca_dslumi%d.value/sca_dslumi%d.value:mps_counter",i,i+4));
		
			//cout <<"I printed " << i <<endl;
		}

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c2.SaveAs(Prefix+"ratios_vs_time.png");
	c2.SaveAs(Prefix+"ratios_vs_time.C");

	return;
}
*/
/*********************************************************** 
Function: dslumi_sum_time
Purpose: To graph the sum of all 8 dslumi rates vs. time on one canvas
Entry Conditions: TChain Mps_Tree
Global: Prefix - the prefix for the output file
Called by: dslumi_ratios_vs_time
Date: 02-03-2012
Modified:
*********************************************************/
/*
void dslumi_sum_vs_time(TChain* Mps_Tree)
{
	//create a 2-D histogram of define it
	h3 = new TH2D ("h3","dslumi rate sum  vs time",30,1.0,0.0,30,1.0,0.0);
        h3->GetYaxis()->SetTitle("dslumi sum rate");
    	h3->GetXaxis()->SetTitle("Time");

//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents=Mps_Tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
//        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        Mps_Tree->SetBranchStatus("Mps_Tree",1);

	std:: vector<TBranch*> dslumi;  //[octant]
	dslumi.resize(9);

	//selects the leaf that we are going to need
	for (int u = 1; u < 9; u++)
	{
		dslumi[u]=Mps_Tree->GetBranch(Form("sca_dslumi%d",u));
	}

        //Loop through this and fill all the graphs at once

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
		Mps_Tree->GetEntry(i);
	
		//Fill histogram
		h3->Fill(dslumi[1]->GetValue()+dslumi[2]->GetValue()+dslumi[3]->GetValue()+dslumi[4]->GetValue()
			+dslumi[5]->GetValue()+dslumi[6]->GetValue()+dslumi[7]->GetValue()++dslumi[8]->GetValue(),mps_counter);
		
	}


	//Create a canvas
	TCanvas c3 ( "c3", "dslumi rate sum vs. mps counter", 400, 500);

	//Graph the sum lumi rate vs time - 
//	Mps_Tree->Draw("(sca_dslumi1.value+sca_dslumi2.value+sca_dslumi3.value+sca_dslumi4.value+sca_dslumi5.value+sca_dslumi6.value+sca_dslumi7.value+sca_dslumi8.value):mps_counter");

	h3->Draw();

	//cout <<"I printed " << endl;

	//save the canvas as a png file - right now it goes to the rootfile directory
	c3.SaveAs(Prefix+"sum_vs_time.png");
	c3.SaveAs(Prefix+"sum_vs_time.C");
	
	return;

}
*/
