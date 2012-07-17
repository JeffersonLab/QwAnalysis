/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs of residual values in in region 2 for each plane of wires

Each canvas is set up so that going across it is all planes are 1,2,3,4,5,6,
across the top and 7,8,9,10,11,12 across the bottom. 

The first canvas for Package 1. Each plane direction has a color see below.
The second canvas for Package 2. Each plane direction has a color see below.
The thrid canvas is divide and has the total of package one (left) and the toltal for package two (right) in it. 

Note the residual is calculated in via fQwTreeLines.fQwHits,fDriftPosition-fQwTreeLines.fQwHits.fTrackPosition

(think least squared without the squared plot)

Plane Colors
X - Red
U - Blue
V - Green

Plane numbers 
X - fPlane%3=1
U - fPlane%3=2
V - fPlane%3=0

The fourth and fifth plots are of the Mean Residual value vs. Plane number in Region 2 for 
pacakge 1 and 2 respectively.  The error bars with out the horzantal ticks at the end are 
just the RMS values of the plot, and the ones with the ticks are the error defined as 
RMS/sqrt(N)

Entry Conditions: the run number, bool for first 100k
Date: 03-28-2012
Modified:07-17-2012
Assisted By:Juan Carlos Cornejo and Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"

#include "TSystem.h"
#include <TGraphErrors.h>
#include "QwEvent.h"
#include "QwHit.h"

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
	//going from q = 0 till q<h.size() allows for changing the size of h easly without hunting it down elsewhere in the code, and makes sure that one is not outside 
	//of the range of h ever.  NOTE: will get a warning about the h.size() not being a signed varable as it is awarnign it can be ignored, other wise one can through 
	//in absolute value
	for(size_t q = 0; q < h.size(); q++)
	{
		h[q].resize(12);
	}

	//create and size a vector of TH1D histograms so I can loop again :)
	//package on is on the left and package two is on the right
	std::vector<TH1D*> h2;
	h2.resize(2);


	for (int pkg =1; pkg<=2; pkg++)
	{

		for (int i = 1; i<=12; i++)
		{
			//define the histograms 0 through 12
			//when in Form command one can NOT have %d -1 that will not return a number, instead it will think of that as a string so one must do there 
			//subraction (math)  after the "" part of the Form command where the diget, or whatever is passed.
			h[pkg - 1][i-1]= new TH1D (Form("h[%d][%d]",pkg - 1,i - 1),Form("Residual - Package %d - Plane %d",pkg,i),30,-0.5,0.5);
	                h[pkg - 1][i-1]->GetYaxis()->SetTitle("Frequency");
        	        h[pkg - 1][i-1]->GetXaxis()->SetTitle(Form("Residual for plane %d",i));
	
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
	

//		event_tree->Draw(Form("events.fQwTreeLines.fQwHits.fDriftPosition-events.fQwTreeLines.fQwHits.fTrackPosition>>h[%d][%d]",pkg - 1,i - 
//           1),Form("events.fQwTreeLines.fQwHits.fRegion==2&&events.fQwTreeLines.fQwHits.fPackage==%d&&events.fQwTreeLines.fQwHits.fPlane==%d",pkg,i));

		}

		//define the histograms 1 through 2 - one for each package
		h2[pkg - 1]= new TH1D (Form("h2[%d]",pkg -1 ),Form("Residual - Package %d - All Planes",pkg),30,-0.5,0.5);
                h2[pkg - 1]->GetXaxis()->SetTitle(Form("Residual for package %d",pkg));
                h2[pkg - 1]->GetYaxis()->SetTitle("Frequency");


	}

//Now go through the tree to fill the histogram on an event buy event base
	//label an event
	QwEvent* fEvent=0;

   	// How many events are in this rootfile?
   	Int_t nevents=event_tree->GetEntries();

	// Now get a pointer to the branches so that we can loop through the tree
    	event_tree->SetBranchStatus("events",1);
    	TBranch* event_branch=event_tree->GetBranch("events");
	//set the brach address	
	event_branch->SetAddress(&fEvent);

	for (int i = 0; i < nevents; i++)
	{
		event_tree->GetEntry(i);		

		//get number of treelines
		int ntreelines = fEvent->GetNumberOfTreeLines();
		
		for (int t = 0; t < ntreelines ; t++)
		{

			//set pointer to treeline
			const QwTreeLine* treeline=fEvent->GetTreeLine(t);

			//get number of hits for this event
			int nhits=treeline->GetNumberOfHits();

			for (int j = 0; j < nhits ; j++)
			{
				//set pointer to hit
				const QwHit* hit=treeline->GetHit(j);

				if (hit->GetRegion()==2)
				{
					if (hit->GetPackage()==1)
					{
						h2[0]->Fill(hit->GetDriftPosition() - hit->GetTrackPosition() );
						h[0][hit->GetPlane() - 1]->Fill(hit->GetDriftPosition() - hit->GetTrackPosition());
					}
					if (hit->GetPackage()==2)
					{
						h2[1]->Fill(hit->GetDriftPosition() - hit->GetTrackPosition() );
						h[1][hit->GetPlane() - 1]->Fill(hit->GetDriftPosition() - hit->GetTrackPosition());
					}
				}
			}
		}
	}

	for (int pkg1 = 1 ; pkg1 <3 ; pkg1++)
	{
		//Create the canvas
		TCanvas c1("c1", Form("Residual values - Package %d",pkg1), 1000,900);

		//divide the canvas
		c1.Divide(6,2);
		
		for (int plane = 1 ; plane < 13; plane ++)
		{
			c1.cd(plane);
		
			h[pkg1-1][plane -1]->Draw();//Look closer at this - not drawing the first 3 planes
		}

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c1.SaveAs(Prefix+Form("Residual_Package_%d.png",pkg1));

	}


/**************
Create graph of the mean values of the REsidual in each plane (in each package) with errors being the RMS

Mean Residual value vs. Plane number in Region 2 for 
pacakge 1 and 2 respectively.  The error bars with out the horzantal ticks at the end are    
just the RMS values of the plot, and the ones with the ticks are the error defined as 
RMS/sqrt(N)

**************/

	for (int pkg =1; pkg<=2; pkg++)
	{

		//Create the canvas
		TCanvas c3("c3", Form("Residual values for each plane - Package %d",pkg), 500,500);

		//divide the canvas
		c3.Divide(2,0);

		Int_t n = 12;

		//define the arrays for the x and y values and there errors

		Double_t x[n]; // X values - plane number 
		Double_t y[n]; // y values - the mean residual value of each plane
		Double_t ex[n]; // error in x - 0 
		Double_t ey[n]; //error in y - the RMS/sqrt(number of events)
		Double_t ey2[n]; //spread of the guassian - the RMS

		for (int z = 1; z<=n; z++)
		{

			x[z - 1]  = z;
			y[z - 1]  = h[pkg - 1][z - 1]->GetMean();
			ex[z - 1] = 0.0;
			ey[z - 1] = h[pkg - 1][z - 1]->GetRMS()/sqrt(h[pkg - 1][z - 1]->GetEntries()); 
			ey2[z -1] =  h[pkg - 1][z - 1]->GetRMS();

		}
	
		//add the data to a new TGraphErrors
		TGraphErrors * gr = new TGraphErrors(n,x,y,ex,ey);
		TGraphErrors * gr2 = new TGraphErrors(n,x,y,ex,ey2);

		//set the marker color - blue :*(
		gr->SetMarkerColor (4);
		gr2->SetMarkerColor (4);

		//set marker size
		gr->SetMarkerSize(5);
		gr2->SetMarkerSize(5);

		//Lable the titles and the axises
		//gr->SetTitle(Form("Mean Rsidual value vs. Plane number in Region 2 for package %d", pkg));
		//gr->GetXaxis()->SetTitle(Form("Plane number in Region 2 for package %d", pkg));
		//gr->GetYaxis()->SetTitle("Mean Residual value (cm)");

                gr2->SetTitle(Form("Mean Rsidual value vs. Plane number in Region 2 for package %d", pkg));
                gr2->GetXaxis()->SetTitle(Form("Plane number in Region 2 for package %d", pkg));
                gr2->GetYaxis()->SetTitle("Mean Residual value (cm)");

                //Draw the error bar as the "sperad" of the guassian of the resdiuals (ie the RMS values)
		//Draw the  data -Z draw errors without the hash bar at the end
		//A=Axis are drawn around the graph - P=each point is drawen
                gr2->Draw("APZ");

		//draw only the points ant the statysitcal errors
		//Draw the  data - P=each point is drawen
		//do not include A as an option if you want to draw on the same plot 
		//cause it will redraw the plot and you will lose the last graph you drew
		gr->Draw("P");

        	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
        	c3.SaveAs(Prefix+Form("Residual_By_Plane_Package_%d.png",pkg));

	}


	//Create the canvas
	TCanvas c2("c2", "Residual values for both Packages", 500,600);

	c2.Divide(2,0);

	for (int j = 1; j<=2; j++)
	{

		c2.cd(j);

		h2[j - 1]->Draw();

//		event_tree->Draw(Form("events.fQwTreeLines.fQwHits.fDriftPosition-events.fQwTreeLines.fQwHits.fTrackPosition>>h2[%d]",j - 
// 		   1),Form("events.fQwTreeLines.fQwHits.fRegion==2&&events.fQwTreeLines.fQwHits.fPackage==%d",j));

	}

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c2.SaveAs(Prefix+"Residual_Both_Packages.png");

	//write down that stats for the Residual in a file 

	//print to file 
	//this file and evrything related to it is fout
	std::ofstream fout;
    
	//open file
	// open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if 
	//needed
	fout.open(Prefix+"Residual.txt");
	if (!fout.is_open()) cout << "File not opened" << endl;
    
	//outputPrefix will inculed run number which we need.

	//Name what each coulmn is.
	//Note direction is 0 - all planes
	//Error is the RMS.sqrt(N)

	fout << "What \t Run \t pkg \t Plane \t Value \t Error" <<endl; 
	fout << "Resd. \t " << runnum << "\t 1 \t 0 \t" << setprecision(5) << h2[0]->GetMean() << "\t" << setprecision(4) << h2[0]->GetRMS()/sqrt(h2[0]->GetEntries()) << 
endl;

	for (int k = 1; k<=12; k++)
	{
		fout << "Resd. \t " << runnum <<"\t 1 \t " << k << " \t" << setprecision(5) << h[0][k - 1]->GetMean() << "\t" << setprecision(4) << h[0][k - 
1]->GetRMS()/sqrt(h[0][k - 1]->GetEntries()) << endl;
	}

	fout << "Resd. \t " << runnum << "\t 2 \t 0 \t" << setprecision(5) << h2[1]->GetMean() << "\t" << setprecision(4) << h2[1]->GetRMS()/sqrt(h2[1]->GetEntries()) << 
endl;

	for (int m = 1; m<=12; m++)
	{
		fout << "Resd. \t " << runnum <<"\t 2 \t " << m << " \t" << setprecision(5) << h[1][m - 1]->GetMean() << "\t" << setprecision(4) << h[1][m - 
1]->GetRMS()/sqrt(h[1][m - 1]->GetEntries()) << endl;
	}

	//close the file
	fout.close();

}
