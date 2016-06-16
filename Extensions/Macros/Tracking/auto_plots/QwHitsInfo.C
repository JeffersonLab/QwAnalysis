/*********************************************************** 
Programmer: Valerie Gray
Purpose: To Output graphs of fQwHits.fRegion, fQwHits.fPackage,  
fQwHits.fDirection,  fQwHits.fElement split by fRegion and fPackage, for the tracking website.


Entry Conditions: the run number, bool for first 100k
Date: 06-13-2012
Modified:7-7-2012
Assisted By: Wouter Deconinck 
*********************************************************/

#include "auto_shared_includes.h"

#include "QwEvent.h"
#include "QwHit.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

//define the functions that will actual do the work for graphing what is needed so that it is not in the main functon :)
void QwHits_Region (TChain * event_tree);
void QwHits_Package (TChain * event_tree);
void QwHits_Direction (TChain * event_tree);
void QwHits_Element (TChain * event_tree);
void NQwTracks (TChain * event_tree);

void QwHitsInfo (int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* chain = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form("$QWSCRATCH/tracking/www/run_%d/QwHitsInfo_%d_",runnum, runnum);

        Prefix = Form(
             TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_QwHitsInfo_",
             runnum, runnum);

	QwHits_Region (chain);

	QwHits_Package (chain);

	QwHits_Direction (chain);

	QwHits_Element (chain);

	NQwTracks (chain);

	return;

}

/*********************************************************** 
Function:QwHits_Region
Purpose: To graph a histogram of the QwHits for each region

	Region definitions from QwHits.h and 
  	https://qweak.jlab.org/QwAnalysis_Docs/html/a00385.html#bec295a444be9e1b82417c9a4a449298

	0 == Null
	1 == Region 1 
	2 == Region 2 
	3 == Region 3 
	4 == Trigger Scintillator
	5 == Scanner
	6 == numRegions

Entry Conditions: TChain - event_tree
Global: Prefix - the prefix for the output file
Called By: QwHitsinfo
Date: 06-13-2012
Modified:7-9-2012
*********************************************************/

void QwHits_Region (TChain * event_tree)
{
	//Create the canvas
	TCanvas c1("c1", "QwHits by Region - regardless of package", 400,400);

	//define the histogram
	TH1D* h = new TH1D ("h","QwHits - Region",7,0,7);

/* edit if want to sperate into packages form region the skeleton is here
	c1.Divide(2,0);

	//create and size a vector of TH1D histograms so I can loop
	//Region 1 is on the left and region 2 is on the right 
	std::vector<TH1D*> h;
	h.resize(2);

	for (int j = 1; j <= 2; j++)
	{

		//define the histograms 1 through 2 - one for each package
		h[j - 1]= new TH1D (Form("h[%d]",j -1 ),Form("QwHits - Region %d",j+2),2,0,2);
	
		c1.cd(j);

		event_tree->Draw(Form("events.fQwHits>>h[%d]",j - 1),Form("events.fQwHits.fRegion==%d",j+1));

	}
*/

//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch = event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
                event_branch->GetEntry(i);

		//Get weight for this event, and set to 1 if fCrossSection is zero (for data)
		double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

                //get the number of Hits
                int nHits = fEvent->GetNumberOfHits();

                // now let's loop through the tree lines and fill all the above histograms
                for (int t = 0; t < nHits; t++)
		{
			// Get pointer to t'th hit in entry i
			const QwHit* hit = fEvent->GetHit(t);

			h->Fill(hit->GetRegion(),w);
		}

	}
//	event_tree->Draw("events.fQwHits.fRegion");

	h->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"Region.png");
	c1.SaveAs(Prefix+"Region.C");

	return;
}

/***********************************************************
Function:QwHits_Package
Purpose: To graph a histogram of the QwHits for each package

	Package deiftions from QwHits.h or 
	https://qweak.jlab.org/QwAnalysis_Docs/html/a00385.html#fc98197a15b682f0dff482c596324bca

	Which arm of the rotator, or octant number

	0 == Null 
	1 == Package Up
	2 == Package Down
	3 == Numpackages

Entry Conditions: TChain - event_tree
Global: Prefix - the prefix for the output file
Called By: QwHitsInfo
Date: 06-13-2012
Modified:7-9-2012
*********************************************************/

void QwHits_Package (TChain * event_tree)
{

	//Create the canvas
	TCanvas c2("c2", "QwHits by Package - regardless of region", 400,400);

	//define the histogram
	TH1D* h2 = new TH1D ("h2","QwHits - Package",3,0,3);

/*edit if want to sperate into packages form region the skeleton is here
	c2.Divide(2,0);

	//create and size a vector of TH1D histograms so I can loop
	//Package 1 is on the left and package 2 is on the right
	std::vector<TH1D*> h2;
	h2.resize(2);

	for (int j = 1; j <= 2; j++)
	{

        	//define the histograms 1 through 2 - one for each package
        	h[j - 1]= new TH1D (Form("h[%d]",j-1 ),Form("QwHits - Package %d",j),2,0,2);

        	c.cd(j);

        	event_tree->Draw(Form("events.fQwHits>>h[%d]",j - 1),Form("events.fQwHits.fPackage==%d",j));

	}
*/

//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch = event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
                event_branch->GetEntry(i);

		//Get weight for this event, and set to 1 if fCrossSection is zero (for data)
		double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

                //get the number of Hits
                int nHits = fEvent->GetNumberOfHits();

                // now let's loop through the tree lines and fill all the above histograms
                for (int t = 0; t < nHits; t++)
		{
			// Get pointer to j'th hit in entry i
			const QwHit* hit = fEvent->GetHit(t);

			h2->Fill(hit->GetPackage(),w);
		}

	}
//	event_tree->Draw("events.fQwHits.fPackage");
	h2->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c2.SaveAs(Prefix+"Package.png");
	c2.SaveAs(Prefix+"Package.C");

	return;
}

/***********************************************************
Function:QwHits_Directions
Purpose: To graph a histogram of the QwHits for each Direction 

	Package definitions from QwHits.h and 
	https://qweak.jlab.org/QwAnalysis_Docs/html/a00385.html#4f3f02fa5b5a88e688c9631aab796bc

	Direction of the plane:  X, Y, U, V; R, theta; etc.

	0 == Null
	1 == Direction X
	2 == Direction Y
	3 == Direction U
	4 == Direction V
	5 == Direction R
	6 == Direction phi
	7 == Direction Left
	8 == Direction Right 
	9 == numDirections

Entry Conditions: TChain - event_tree
Global: Prefix - the prefix for the output file
Called By: QwHitsinfo
Date: 06-13-2012
Modified: 07-09-2012
*********************************************************/

void QwHits_Direction (TChain * event_tree)
{
	//Create the canvas
	TCanvas c3("c3", "QwHits by Direction - in Region 2", 400,400);

	//define the histogram
	TH1D* h3 = new TH1D ("h3","QwHits - Direction",6,0,6);

/*edit if want to sperate into packages form region the skeleton is here

	c3.Divide(4,0);

	//create and size a vector of TH1D histograms so I can loop
	//Directions (left to right (X,Y,U,V,R,phi))
	std::vector<TH1D*> h3;
	h3.resize(4);
 
	for (int j = 1; j <= 4; j++)
	{

        	//define the histograms 1 through 4 - one for each plain and on for the all of them
        	h[j - 1]= new TH1D (Form("h[%d]",j -1 ),Form("QwHits - Diection %d",j),2,0,2);

        	c.cd(j);

        	event_tree->Draw(Form("events.fQwHits>>h[%d]",j - 1),Form("events.fQwHits.fDirection==%d",j));

	}
*/
//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch = event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
                event_branch->GetEntry(i);

		//Get weight for this event, and set to 1 if fCrossSection is zero (for data)
		double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

                //get the number of Hits
                int nHits = fEvent->GetNumberOfHits();

                // now let's loop through the tree lines and fill all the above histograms
                for (int t = 0; t < nHits; t++)
		{
			// Get pointer to j'th hit in entry i
			const QwHit* hit = fEvent->GetHit(t);

			//Directions (left to right (X,Y,U,V,R,phi,left,right))
			h3->Fill(hit->GetDirection(),w);
		}
	}

//	event_tree->Draw("events.fQwHits.fDirection");
	h3->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c3.SaveAs(Prefix+"Direction.png");
	c3.SaveAs(Prefix+"Direction.C");

	return;

}

/***********************************************************
Function:QwHits_Element
Purpose: To graph a histogram of the QwHits for each Element for each region and package

	Package deiftions from QwHits.h
	
	Trace # for R1; wire # for R2 & R3; PMT # for others

	Can't find better discription then that right now - if know please fill in 
	otherwise I will add something if I stumble on it :) so far 
	everyting is rather circular on the QwAnalysis Doc


Entry Conditions: TChain - event_tree
Global: Prefix - the prefix for the output file
Called By: QwHitsinfo
Date: 06-13-2012
Modified:07-09-2012
*********************************************************/

void QwHits_Element (TChain * event_tree)
{

	//create and size a vector of TH1D histograms so I can loop 
	//This is for region 2 and 3
	std::vector < vector<TH1D*> > h4;
	h4.resize(4);

	for (int q = 0 ; q < 4 ; q++)
	{
		h4[q].resize(3);
		for (int w = 0 ; w < 3; w++)
		{
	        	//define the histograms 1 through 4 - one for each plain and on for the all of them
       		 	h4[q][w]= new TH1D (Form("h4[%d][%d]",q,w),Form("QwHits - Element for Region %d, Package %d",q,w),33,0,33);
			//h4[region][package]
		}
	}

	//create and size a vector of TH1D histograms so I can loop
	std::vector<TH1D*> h5;
	h5.resize(2);

	for(int e = 0 ; e < 2 ; e++)
	{
        	//define the histograms 1 through 4 - one for each plain and on for the all of them
        	h5[e]= new TH1D (Form("h5[%d]",e ),Form("QwHits - Element for Region %d",e + 4),8,0,8);
	}

//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch = event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
                event_branch->GetEntry(i);

		//Get weight for this event, and set to 1 if fCrossSection is zero (for data)
		double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

                //get the number of Hits
                int nHits = fEvent->GetNumberOfHits();

                // now let's loop through the hits and fill all the above histograms
                for (int t = 0; t < nHits; t++)
		{
			// Get pointer to j'th hit in entry i
			const QwHit* hit = fEvent->GetHit(t);

			if (hit->GetRegion() == 2)
			{
				if (hit->GetPackage() == 1)
				{
					//h4[region][package]
					h4[2][1]->Fill(hit->GetElement(),w);
				} else if (hit->GetPackage() == 2)
					{
						h4[2][2]->Fill(hit->GetElement(),w);
					}
			}

			if (hit->GetRegion() == 3)
			{
				if (hit->GetPackage() == 1)
				{
					//h4[region][package]
					h4[3][1]->Fill(hit->GetElement(),w);
				} else if (hit->GetPackage() == 2)
					{
						h4[3][2]->Fill(hit->GetElement(),w);
					}
			}

			if (hit->GetRegion() == 4)
			{
				h5[0]->Fill(hit->GetElement(),w);
			}

			if (hit->GetRegion() == 5)
			{
				h5[1]->Fill(hit->GetElement(),w);
			}
		}
	}

	for (int r = 2; r <= 3; r++)
	{
		//Create the canvas
		TCanvas c4("c4", Form("QwHits by Element - in Region %d",r), 400,400);

		c4.Divide(2,0);

		for (int pkg = 1; pkg <= 2; pkg++)
		{

        		c4.cd(pkg);
			h4[r][pkg]->Draw();
//			event_tree->Draw("events.fQwHits.fElement",Form("events.fQwHits.fRegion==%d && events.fQwHits.fPackage==%d",r, pkg));
		}

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c4.SaveAs(Prefix+Form("Elements_Region_%d.png",r));
		c4.SaveAs(Prefix+Form("Elements_Region_%d.C",r));
	}

	for (int r2 = 4 ; r2<= 5; r2++)
	{
		//Create the canvas
		TCanvas c5("c5", Form("QwHits by Element - in Region %d",r2), 500,400);

		h5[r2 - 4]->Draw();

//        	event_tree->Draw("events.fQwHits.fElement",Form("events.fQwHits.fRegion==%d",r2));

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c5.SaveAs(Prefix+Form("Elements_Region_%d.png",r2));
		c5.SaveAs(Prefix+Form("Elements_Region_%d.C",r2));
	}

	return;

}

/***********************************************************
Function:NQwTracks
Purpose: To graph a histogram of the log of events.fNQwTracks

Entry Conditions: TChain - event_tree
Global: Prefix - the prefix for the output file
Called By: QwHitsinfo
Date: 06-13-2012
Modified:
*********************************************************/

void NQwTracks (TChain * event_tree)
{

	//define the histogram
	TH1D* h6 = new TH1D ("h6","NQwTracks",11,0,11);

	//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents = event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch = event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);

        for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
        {
                //Get the ith entry form the event tree
                event_branch->GetEntry(i);

		//Get weight for this event, and set to 1 if fCrossSection is zero (for data)
		double w = fEvent->fCrossSection? fEvent->fCrossSection: 1;

		h6->Fill(fEvent->GetNumberOfTracks(),w);
	}

        //Create the canvas and set y axis log scale
        TCanvas c4("c4", "Log of events.fNQwTracks", 400,400);
	c4.SetLogy();

	h6->Draw();

//        event_tree->Draw("events.fNQwTracks");

        //save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
        c4.SaveAs(Prefix+"fNQwTracks.png");
        c4.SaveAs(Prefix+"fNQwTracks.C");

        return;

}

