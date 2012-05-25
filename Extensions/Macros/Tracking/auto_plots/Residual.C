/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs of residual values in in region 2 for each plane of wires

Each canvas is set up so that going across it is all planes are 1,2,3,4,5,6,across the top and 7,8,9,10,11,12 across the bottom 

The first canvas for Package 1. Each plane direction has a color see below.
The second canvas for Package 2. Each plane direction has a color see below.
The thrid canvas is divide and has the total of package one (left) and the toltal for package two (right) in it. 

Note the residual is calculated in via fQwTreeLines.fQwHits,fDriftPosition-fQwTreeLines.fQwHits.fTrackPosition

Plane Colors
X - Red
U - Blue
V - Green

Plane numbers 
X - fPlane%3=1
U - fPlane%3=2
V - fPlane%3=0

Entry Conditions: the run number, bool for first 100k
Date: 03-28-2012
Modified:04-17-2012
Assisted By:Juan Carlos Cornejo and Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"

#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include <vector>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void Residual(int runnum, bool is100k)
{
// changed the Prefix so that it is compatble with both Root and writing to a file by setting the enviromnet properly
//deifne the prefix as the directory that the files will be outputed to
Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/Residual_%d_",runnum,runnum);

// groups root files for a run together
TChain* event_tree = new TChain ("event_tree");

//add the root files to the chain the event_tree branches
event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

//create a vector of vectors of TH1D histogram pointer
std::vector< vector<TH1D*> > h;

//size the vector h
h.resize(2);
//size each the h[i] vector
//going from q = 0 till q<h.size() allows for changing the size of h easly without hunting it down elsewhere in the code, and makes sure that one is not outside of the range of h ever.  NOTE: will get a warning about the h.size() not being a signed varable as it is awarnign it can be ignored, other wise one can through in absolute value
for(int q = 0; q < h.size(); q++)
{
	h[q].resize(12);
}


for (int pkg =1; pkg<=2; pkg++)
{

//Create the canvas
TCanvas c1("c1", Form("Residual values - Package %d",pkg), 1500,900);

//divide the canvas
c1.Divide(6,2);

	for (int i = 1; i<=12; i++)
	{
		//define the histograms 0 through 12
		//when in Form command one can NOT have %d -1 that will not return a number, instead it will think of that as a string so one must do there subraction (math)  after the "" part of the Form command where the diget, or whatever is passed.
		h[pkg - 1][i-1]= new TH1D (Form("h[%d][%d]",pkg - 1,i - 1),Form("Residual - Package %d - Plane %d",pkg,i),30,-0.5,0.5);
	
		if(i%3==1)
			{	
				//this is the X plane, so it is red
				h[pkg - 1][i - 1]->SetLineColor(kRed);
				}else if (i%3==2)
					{
						//This is the U plane and it is blue :*(
						h[pkg - 1][i - 1]->SetLineColor(kBlue);
					}else 
						{
							//This is the V plane and it is green
							h[pkg - 1][i - 1]->SetLineColor(kGreen);
						}


		c1.cd(i);

	event_tree->Draw(Form("events.fQwTreeLines.fQwHits.fDriftPosition-events.fQwTreeLines.fQwHits.fTrackPosition>>h[%d][%d]",pkg - 1,i - 1),Form("events.fQwTreeLines.fQwHits.fRegion==2&&events.fQwTreeLines.fQwHits.fPackage==%d&&events.fQwTreeLines.fQwHits.fPlane==%d",pkg,i));

	}

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+Form("Residual_Package_%d.png",pkg));
}

//Create the canvas
TCanvas c2("c2", "Residual values for both Packages", 1500,900);

c2.Divide(2,0);

//create and size a vector of TH1D histograms so I can loop again :)
//package on is on the left and package two is on the right
std::vector<TH1D*> h2;
h2.resize(2);

for (int j = 1; j<=2; j++)
{

	//define the histograms 1 through 2 - one for each package
	h2[j - 1]= new TH1D (Form("h2[%d]",j -1 ),Form("Residual - Package %d - All Planes",j),30,-0.5,0.5);
	
	c2.cd(j);

	event_tree->Draw(Form("events.fQwTreeLines.fQwHits.fDriftPosition-events.fQwTreeLines.fQwHits.fTrackPosition>>h2[%d]",j - 1),Form("events.fQwTreeLines.fQwHits.fRegion==2&&events.fQwTreeLines.fQwHits.fPackage==%d",j));

}

//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
c2.SaveAs(Prefix+"Residual_Both_Packages.png");

//write down that stats for the Residual in a file 

//print to file 
//this file and evrything related to it is fout
std::ofstream fout;
    
//open file
// open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if needed
fout.open(Prefix+"Residual.txt");
if (!fout.is_open()) cout << "File not opened" << endl;
    
//outputPrefix will inculed run number which we need.

//Name what each coulmn is.
//Note direction is 0 - all planes
//Error is the RMS.sqrt(N)

fout << "What \t Run \t pkg \t Plane \t Value \t Error" <<endl; 
fout << "Resd. \t " << runnum << "\t 1 \t 0 \t" << setprecision(5) << h2[0]->GetMean() << "\t" << setprecision(4) << h2[0]->GetRMS()/sqrt(h2[0]->GetEntries()) << endl;

for (int k = 1; k<=12; k++)
{
	fout << "Resd. \t " << runnum <<"\t 1 \t " << k << " \t" << setprecision(5) << h[0][k - 1]->GetMean() << "\t" << setprecision(4) << h[0][k - 1]->GetRMS()/sqrt(h[0][k - 1]->GetEntries()) << endl;
}

fout << "Resd. \t " << runnum << "\t 2 \t 0 \t" << setprecision(5) << h2[1]->GetMean() << "\t" << setprecision(4) << h2[1]->GetRMS()/sqrt(h2[1]->GetEntries()) << endl;

for (int m = 1; m<=12; m++)
{
	fout << "Resd. \t " << runnum <<"\t 2 \t " << m << " \t" << setprecision(5) << h[1][m - 1]->GetMean() << "\t" << setprecision(4) << h[1][m - 1]->GetRMS()/sqrt(h[1][m - 1]->GetEntries()) << endl;
}

//close the file
fout.close();

}
