/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs of log (chi) and  log(Chi) values vs time in in region 2 for each plane of wires, x,u,v

Each canvas is set up so that going across it is all planes (black), X (Red),U(Blue),V(Green) and Drift Distance is on the top and Drift Time is on the Bottom.

The first canvas is the log of Chi values for the whole package or the each wire direction.
The second canvas is the log of Chi values vs time (fEvent->fEventHeader->fEventNumber) for the whole package or the each wrie direction.

Plane numbers 
x - fDirection==1
u - fDirection==3
v - fDirection==4

NOTE the time plots are comment out as of now, we can add them later if we can use them later
Right now they don't seam to tell us anything

edited: 05-14-2014
added in the in formation so that the Chi values are also printed out
changed the error so that it prints out the Chi values and not just log Chi
Also changed error to RMS, as we are intersted in the width of the distribution

Entry Conditions: the run number, bool for first 100k
Date: 03-22-2012
Modified:05-14-2014
Assisted By: Juan Carlos Cornejo and Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"
#include "TH2D.h"

#include "QwEvent.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <iomanip>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

/***********************************
set up the significant figures right - &plusmn is for html,
change to +/- if not useing,
or in this case \t for a tab space in the outputfile
Use this by calling: value_with_error(a,da)
***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
 << std::setprecision((size_t) (std::log10((a)/(da)) - std::log10(a) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)

void Chi(int runnum, bool is100k)
{

	// changed the outputPrefix so that it is compatble with both Root and writing to a file by setting the enviromnet properly
	//deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/%d_",runnum,runnum);

	Prefix = Form(
	      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
	      runnum, runnum);

	// groups root files for a run together
	TChain* event_tree = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//Create the canvas
	TCanvas c1("c1", "log(Chi) - Package 1 across top, Package 2 across bottom", 1900,2000);

	//divide the canvas
	c1.Divide(4,2);	

	//Create the canvas this canvas is for the chi vs time plots
//	TCanvas c3("c3", "log(Chi) vs time - Package 1 across top, Package 2 across bottom", 900,1000);

	//divide the canvas
//	c3.Divide(4,2);


	//create a vector of vectors of TH1D histogram pointer
	std::vector< vector<TH1D*> > c;
	//Size of c
	c.resize(2);

	//size each the c[i] vector
	//going from q = 0 till q<h.size() allows for changing the size of h easly without hunting it down elsewhere in the code, and makes sure that one is not outside 
	//of the range of h ever.  NOTE: will get a warning about the c.size() not being a signed varable as it is awarnign it can be ignored, other wise one can through 
	//in absolute value
	for(size_t q = 0; q < c.size(); q++)
	{
		c[q].resize(4);
	}

	//Defin the Plane names
	TString Plane [4] = {"All Planes" , "X plane", "U plane", "V plane"};

	//Let's define the histograms
	for (int pkg = 1; pkg <= 2; pkg ++)
	{
		for (int plane = 0; plane < 4; plane ++)
		{
			c[pkg - 1][plane]= new TH1D (Form("c[%d][%d]",pkg - 1,plane),Plane[plane] + Form(" log(Chi)- Package %d",pkg),30,0.01,5.0);
			c[pkg - 1][plane]->GetYaxis()->SetTitle("frequency");
   		c[pkg - 1][plane]->GetXaxis()->SetTitle("log(Chi)");
		}

		//set line colors of all the histograms
		c[pkg - 1][1]->SetLineColor(2);
		c[pkg - 1][2]->SetLineColor(4);
		c[pkg - 1][3]->SetLineColor(8);

	}

  //for the no log chi values
	std::vector< vector<TH1D*> > h1;
	//Size of c
	h1.resize(2);
	for(size_t q = 0; q < h1.size(); q++)
	{
		h1[q].resize(4);
	}

	//Let's define the histograms
	for (int pkg = 1; pkg <= 2; pkg ++)
	{
		for (int plane = 0; plane < 4; plane ++)
		{
			h1[pkg - 1][plane]= new TH1D (Form("h1[%d][%d]",pkg - 1,plane),Plane[plane] + Form(" Chi- Package %d",pkg),30,0.01,5.0);
			h1[pkg - 1][plane]->GetYaxis()->SetTitle("frequency");
     	h1[pkg - 1][plane]->GetXaxis()->SetTitle("Chi");
		}

    //set line colors of all the histograms
		h1[pkg - 1][1]->SetLineColor(2);
		h1[pkg - 1][2]->SetLineColor(4);
		h1[pkg - 1][3]->SetLineColor(8);

	}


	//create a vector of vectors of TH1D histogram pointer
//	std::vector< vector<TH2D*> > h;
	//Size of c
//	h.resize(2);

	//size each the c[i] vector
	//going from q = 0 till q<h.size() allows for changing the size of h easly without hunting it down elsewhere in the code, and makes sure that one is not outside 
	//of the range of h ever.  NOTE: will get a warning about the c.size() not being a signed varable as it is awarnign it can be ignored, other wise one can through 
	//in absolute value
//	for(size_t q = 0; q < c.size(); q++)
//	{
//		h[q].resize(4);
//	}

	//Let's define the histograms
//	for (int pkg = 1; pkg <= 2; pkg ++)
//	{
//		for (int plane = 0; plane <4; plane ++)
//		{
//			//autpomatic binning by making the hig bin lower then the low bin 
//			h[pkg - 1][plane]= new TH2D (Form("c[%d][%d]",pkg - 1,plane),Plane[plane] + Form(" log(Chi) vs. Time - Package %d",pkg),30,1,0,60,0.01,10.0);
//			h[pkg - 1][plane]->GetYaxis()->SetTitle("time");
//        		h[pkg - 1][plane]->GetXaxis()->SetTitle("log(Chi)");		
//
//		}
//
//		//set line colors of all the histograms
//		h[pkg - 1][1]->SetLineColor(2);
//		h[pkg - 1][2]->SetLineColor(4);
//		h[pkg - 1][3]->SetLineColor(8);
//
//	} 


//start with looping over all the events and putting them in the correct histogram 
	
	//figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents=event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch=event_tree->GetBranch("events");
        event_branch->SetAddress(&fEvent);

	//Loop through this and fill all the graphs at once

	for (int i = 0; i < nevents ; i++) //shouldn't this have and equal to it?
	{
		//Get the ith entry form the event tree
		event_branch->GetEntry(i);

		//get the number of Treelines
		int nTreeLines = fEvent->GetNumberOfTreeLines();

		// now let's loop through the tree lines and fill all the above histograms
		for (int t = 0; t < nTreeLines; t++)
		{
			 const QwTreeLine* treeline = fEvent->GetTreeLine(t);
	
			//Start making the cuts needed to fill the histogram

			//PACKAGE 1 for Region 2
			if (treeline->fRegion==2&&treeline->fPackage==1)
			{
				c[0][0]->Fill(log(treeline->fChi));
				h1[0][0]->Fill(treeline->fChi);
//				h[0][0]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				if (treeline->fDirection==1)
				{	
					c[0][1]->Fill(log(treeline->fChi));
					h1[0][1]->Fill(treeline->fChi);
//					h[0][1]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				}
				if (treeline->fDirection==3)
				{	
					c[0][2]->Fill(log(treeline->fChi));
					h1[0][2]->Fill(treeline->fChi);
//					h[0][3]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				}
				if (treeline->fDirection==4)
				{	
					c[0][3]->Fill(log(treeline->fChi));
					h1[0][3]->Fill(treeline->fChi);
//					h[0][3]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				}
			}

			//PACKAGE 2 for REGION 2	
			if (treeline->fRegion==2&&treeline->fPackage==2)
			{
				c[1][0]->Fill(log(treeline->fChi));
				h1[1][0]->Fill(treeline->fChi);
//				h[1][0]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				if (treeline->fDirection==1)
				{	
					c[1][1]->Fill(log(treeline->fChi));	
					h1[1][1]->Fill(treeline->fChi);	
//					h[1][1]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				}
				if (treeline->fDirection==3)
				{	
					c[1][2]->Fill(log(treeline->fChi));	
					h1[1][2]->Fill(treeline->fChi);	
//					h[1][2]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				}
				if (treeline->fDirection==4)
				{	
					c[1][3]->Fill(log(treeline->fChi));
					h1[1][3]->Fill(treeline->fChi);
//					h[1][3]->Fill(log(treeline->fChi),fEvent->fEventHeader->fEventNumber);
				}
			}
		}
	}

	//Draw these histograms - all, x ,u,v across and package 1 on top and package 2 on the bottom
	c1.cd(1);
	c[0][0]->Draw();
	c1.cd(2);
	c[0][1]->Draw();
	c1.cd(3);
	c[0][2]->Draw();
	c1.cd(4);
	c[0][3]->Draw();
	c1.cd(5);
	c[1][0]->Draw();
	c1.cd(6);
	c[1][1]->Draw();
	c1.cd(7);
	c[1][2]->Draw();
	c1.cd(8);
	c[1][3]->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c1.SaveAs(Prefix+"LogChi.png");
	c1.SaveAs(Prefix+"LogChi.C");


	//Draw the time histograms - all, x ,u,v across and package 1 on top and package 2 on the bottom
//	c3.cd(1);
//	h[0][0]->Draw();
//	c3.cd(2);
//	h[0][1]->Draw();
//	c3.cd(3);
//	h[0][2]->Draw();
//	c3.cd(4);
//	h[0][3]->Draw();
//	c3.cd(5);
//	h[1][0]->Draw();
//	c3.cd(6);
//	h[1][1]->Draw();
//	c3.cd(7);
//	h[1][2]->Draw();
//	c3.cd(8);
//	h[1][3]->Draw();

	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
//	c3.SaveAs(Prefix+"chi_vs_time.png");
//	c3.SaveAs(Prefix+"chi_vs_time.C");

	//write the stats about chi to a file

	//print to file 
	//this file and evrything related to it is fout
	std::ofstream fout;
    
	//open file
	// open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if needed	
	fout.open(Prefix+"LogChi.txt");
	if (!fout.is_open()) cout << "File not opened" << endl;	
    
	//outputPrefix will inculed run number which we need.
    
	//Name what each coulmn is.
	//Note direction is 0 - all planes, 1 - the X planes, 3 - U planes, 4 - V planes
	//Error is the RMS.sqrt(N)
	fout << "What \t Run \t pkg \t DIR \t Value \t RMS" <<endl;
	fout << "LogChi \t " << runnum << " \t 1 \t 0 \t " << value_with_error(c[0][0]->GetMean(),c[0][0]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 1 \t 1 \t" << value_with_error(c[0][1]->GetMean(), c[0][1]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 1 \t 3 \t" << value_with_error(c[0][2]->GetMean(), c[0][2]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 1 \t 4 \t" << value_with_error(c[0][3]->GetMean(), c[0][3]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 2 \t 0 \t" << value_with_error(c[1][0]->GetMean(), c[1][0]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 2 \t 1 \t" << value_with_error(c[1][1]->GetMean(), c[1][1]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 2 \t 3 \t" << value_with_error(c[1][2]->GetMean(), c[1][2]->GetRMS()) << endl;

	fout << "LogChi \t " << runnum << " \t 2 \t 4 \t" << value_with_error(c[1][3]->GetMean(), c[1][3]->GetRMS()) << endl;

	//close the file
	fout.close();

//the Chi values
	fout.open(Prefix+"Chi.txt");
	if (!fout.is_open()) cout << "File not opened" << endl;
	//outputPrefix will inculed run number which we need.

	//Name what each coulmn is.
	//Note direction is 0 - all planes, 1 - the X planes, 3 - U planes, 4 - V planes
	fout << "What \t Run \t pkg \t DIR \t Value \t RMS" <<endl;
	fout << "Chi \t " << runnum << " \t 1 \t 0 \t " << value_with_error(h1[0][0]->GetMean(),h1[0][0]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 1 \t 1 \t" << value_with_error(h1[0][1]->GetMean(), h1[0][1]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 1 \t 3 \t" << value_with_error(h1[0][2]->GetMean(), h1[0][2]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 1 \t 4 \t" << value_with_error(h1[0][3]->GetMean(), h1[0][3]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 2 \t 0 \t" << value_with_error(h1[1][0]->GetMean(), h1[1][0]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 2 \t 1 \t" << value_with_error(h1[1][1]->GetMean(), h1[1][1]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 2 \t 3 \t" << value_with_error(h1[1][2]->GetMean(), h1[1][2]->GetRMS()) << endl;

	fout << "Chi \t " << runnum << " \t 2 \t 4 \t" << value_with_error(h1[1][3]->GetMean(), h1[1][3]->GetRMS()) << endl;

	//close the file
	fout.close();

return;

}
