/*********************************************************** 
Programmer: Valerie Gray
Purpose: To Output graphs of fQwHits.fRegion, fQwHits.fPackage,  
fQwHits.fDirection,  fQwHits.fElement split by fRegion and fPackage, for the tracking website.


Entry Conditions: the run number, bool for first 100k
Date: 06-13-2012
Modified:
Assisted By: Wouter Deconinck 
*********************************************************/

#include "auto_shared_includes.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

//define the functions that will actual do the work for graphing what is needed so that it is not in the main functon :)
void QwHits_Region (TChain * event_tree);
void QwHits_Package (TChain * event_tree);
void QwHits_Direction (TChain * event_tree);
void QwHits_Element (TChain * event_tree);


void QwHitsInfo (int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* chain = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	Prefix = Form("$QWSCRATCH/tracking/www/run_%d/QwHitsInfo_%d_",runnum, runnum);

	QwHits_Region (chain);

	QwHits_Package (chain);

	QwHits_Direction (chain);

	QwHits_Element (chain);

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
Modified:
*********************************************************/

void QwHits_Region (TChain * event_tree)
{

	//Create the canvas
	TCanvas c1("c1", "QwHits by Region - regardless of package", 900,600);

/* edit if want to sperate into packages form region the skeleton is here
	c1.Divide(2,0);

	//create and size a vector of TH1D histograms so I can loop
	//Region 2 is on the left and region 2 is on the right 
	std::vector<TH1D*> h;
	h.resize(2);

	for (int j = 1; j<=2; j++)
	{

		//define the histograms 1 through 2 - one for each package
		h[j - 1]= new TH1D (Form("h[%d]",j -1 ),Form("QwHits - Region %d",j+2),30,-0.5,0.5);
	
		c1.cd(j);

		event_tree->Draw(Form("events.fQwHits>>h[%d]",j - 1),Form("events.fQwHits.fRegion==%d",j+1));

	}
*/

	event_tree->Draw("events.fQwHits.fRegion");

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"Region.png");

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
Modified:
*********************************************************/

void QwHits_Package (TChain * event_tree)
{

	//Create the canvas
	TCanvas c2("c2", "QwHits by Package - regadless of region", 900,600);

/*edit if want to sperate into packages form region the skeleton is here
	c2.Divide(2,0);

	//create and size a vector of TH1D histograms so I can loop
	//Package 1 is on the left and package 2 is on the right
	std::vector<TH1D*> h2;
	h2.resize(2);

	for (int j = 1; j<=2; j++)
	{

        	//define the histograms 1 through 2 - one for each package
        	h[j - 1]= new TH1D (Form("h[%d]",j -1 ),Form("QwHits - Package %d",j),30,-0.5,0.5);

        	c.cd(j);

        	event_tree->Draw(Form("events.fQwHits>>h[%d]",j - 1),Form("events.fQwHits.fPackage==%d",j));

	}
*/

	event_tree->Draw("events.fQwHits.fPackage");

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c2.SaveAs(Prefix+"Package.png");

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
Modified:
*********************************************************/

void QwHits_Direction (TChain * event_tree)
{

	//Create the canvas
	TCanvas c3("c3", "QwHits by Dircetion - in Region 2", 900,600);

/*edit if want to sperate into packages form region the skeleton is here

	c3.Divide(4,0);

	//create and size a vector of TH1D histograms so I can loop
	//Directions (left to right (X,Y,U,V,R,phi))
	std::vector<TH1D*> h3;
	h3.resize(4);
 
	for (int j = 1; j<=4; j++)
	{

        	//define the histograms 1 through 4 - one for each plain and on for the all of them
        	h[j - 1]= new TH1D (Form("h[%d]",j -1 ),Form("QwHits - Diection %d",j),30,-0.5,0.5);

        	c.cd(j);

        	event_tree->Draw(Form("events.fQwHits>>h[%d]",j - 1),Form("events.fQwHits.fDirection==%d",j));

	}
*/

	//Directions (left to right (X,Y,U,V,R,phi,left,right))

	event_tree->Draw("events.fQwHits.fDirection");

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c3.SaveAs(Prefix+"Direction.png");

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
Modified:
*********************************************************/

void QwHits_Element (TChain * event_tree)
{

	for (int r = 2; r <=3; r++)
	{
		//Create the canvas
		TCanvas c4("c4", Form("QwHits by Element - in Region %d",r), 900,600);

		c4.Divide(2,0);

		//create and size a vector of TH1D histograms so I can loop
		//Package 2 is on the left and region 2 is on the right
		std::vector<TH1D*> h4;
		h4.resize(2);

		for (int pkg = 1; pkg <=2; pkg++)
		{

	        	//define the histograms 1 through 4 - one for each plain and on for the all of them
       		 	h4[pkg - 1]= new TH1D (Form("h[%d]",pkg -1 ),Form("QwHits - Element for Region %d, Package %d",r,pkg),30,-0.5,0.5);

        		c4.cd(pkg);
	
			event_tree->Draw("events.fQwHits.fElement",Form("events.fQwHits.fRegion==%d && events.fQwHits.fPackage==%d",r, pkg));

		}

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c4.SaveAs(Prefix+Form("Elements_Region_%d.png",r));
	}

	for (int r2 = 4 ; r2<= 5; r2++)
	{
		//Create the canvas
		TCanvas c5("c5", Form("QwHits by Element - in Region %d",r2), 900,600);

		//create and size a vector of TH1D histograms so I can loop
		//Package 1 is on the left and package 2 is on the right
		std::vector<TH1D*> h5;
		h5.resize(3);

        	//define the histograms 1 through 4 - one for each plain and on for the all of them
        	h5[r2 - 4]= new TH1D (Form("h[%d]",r2 -4 ),Form("QwHits - Element for Region %d",r2),30,-0.5,0.5);

        	event_tree->Draw("events.fQwHits.fElement",Form("events.fQwHits.fRegion==%d",r2));

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c5.SaveAs(Prefix+Form("Elements_Region_%d.png",r2));

	}

	return;

}

