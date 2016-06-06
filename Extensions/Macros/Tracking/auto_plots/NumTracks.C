/***********************************************************
Programmer: Valerie Gray
Purpose: To look at where the tracks/events are getting lost.

Goal: to output plots to indicate where/when we are losing tracks

Entry Conditions: the run number, bool for first 100k
Date: 07-02-2012
Modified: 07-03-2012
Assisted By: Wouter Deconinck and Juan Carlos Cornejo
*********************************************************/

#include "auto_shared_includes.h"

#include "TH1D.h"

#include "QwEvent.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwHit.h"

//define a prefix for all the output files - global don't need to pass
TString Prefix;

void NumTracks(Int_t runnum, bool is100k)
{

	// groups root files for a run together
	TChain* chain = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form("$QWSCRATCH/tracking/www/run_%d/%d_",runnum, runnum);

	Prefix = Form(
	      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
	      runnum, runnum);

	//define the arrays that will store the infomation - (enrty num = region - 1)
	//entry 0 will be for both regions
	//entry 1 will be for region 2
	//entry 2 will be for region 3

	//the number of events that we have based on the variable
	//right now only the first enrty is being used the other
	// will be joined later

	Int_t Hits [3] = {0,0,0};
	Int_t TreeLines [3] = {0,0,0};
	Int_t PartialTracks [3] = {0,0,0};
	Int_t Tracks [3] = {0,0,0};

	//So I want to step through each entry in the root file and see if
	//it is an entry, treeline, partialtrack, or track for
	//both regions, region 2, or region 3

	//figure out how many evernts are in the rootfile so I know how long to have my loop go for
   	Int_t nevents=chain->GetEntries();

	//To start this I think that I might have to define a QwEvent as a pointer - Why I have nto idea :(
	QwEvent* fEvent = 0;

	//Now I have to get a pointer to the events branch to loop through
	
	//Start by setting the event_tree branch to be on
	chain->SetBranchStatus("events",1);

	//now get the event branch of the event_tree branch and call it event_branch creativly
	TBranch* event_branch=chain->GetBranch("events");
	event_branch->SetAddress(&fEvent);

	for (Int_t i = 0; i < 10000 ; i++)
	{
		//Get the ith entry form the event tree
		event_branch->GetEntry(i);

		//get the number of hits, tracks, patrialtracks, 
		//and treelines in each event
		Int_t nHits=fEvent->GetNumberOfHits();
		Int_t nTreeLines= fEvent->GetNumberOfTreeLines();
		Int_t nTracks=fEvent->GetNumberOfTracks();		 
		Int_t nPartialTracks=fEvent->GetNumberOfPartialTracks();

		//so lets pass over all the hits and start filling in these arrays
		
		//starting with the hits

		//define array to store if there is/what the number of hist are for each plane
		//R2[plane]
//look at the number of planes and directions in R2 that may be you prpblem :)
		//R3[plane]
		int nHitsR2[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
		int nHitsR3[5] = {0,0,0,0,0};
		for (Int_t q = 0 ; q < nHits; q ++)
		{
			const QwHit* hit=fEvent->GetHit(q);
			
			if (hit->GetRegion() == 2)
			{
				nHitsR2[hit->fPlane] = 1;
 				
			}else if (hit->GetRegion() == 3)
			{
				nHitsR3[hit->fPlane]++;
			}
		}

		//in region 2, in every diection there must be 3 or more planes with hits makes it a good event
		//in region 3, all plane must have 5 or more hit in it makes it a good event
		//BOTH of these must be satisfied in order for this event to be a good event under the QwHit class
		int minHitsR2 = 3;
		int minHitsR3 = 5;
/*
		for ( int k = 1 ; k <5 ; k++)
		{
			cout << nHitsR3[k] << " ";
			
		}
		cout << endl;

		for ( int k = 1 ; k <13 ; k++)
		{
			cout << nHitsR2[k] << " ";
		}
		cout << endl;
*/
		if (nHitsR3[1] >= minHitsR3 && nHitsR3[2] >= minHitsR3 && nHitsR3[3] >= minHitsR3 && nHitsR3[4] >= minHitsR3
		 && nHitsR2[1] + nHitsR2[4] + nHitsR2[7] + nHitsR2[10] >= minHitsR2
		 && nHitsR2[2] + nHitsR2[5] + nHitsR2[8] + nHitsR2[11] >= minHitsR2
		 && nHitsR2[3] + nHitsR2[6] + nHitsR2[9] + nHitsR2[12] >= minHitsR2)
		{
			Hits[0]++;

		}

                //define array to store if there is/what the number of Treelines are for each plane
                //R2[direction]
                //R3[package][plane]
                int nTreeLinesR2[5] = {0,0,0,0,0}, nTreeLinesR3[3][5] = {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}};

                //starting with the TreeLines
                for (Int_t t = 0 ; t < nTreeLines; t++)
                {
                        const QwTreeLine* treeline = fEvent->GetTreeLine(t);

                        if (treeline->GetRegion() == 2)
                        {
 				nTreeLinesR2[treeline->fDirection] = 1;                               
                        }else if (treeline->GetRegion() == 3)
                         {
                                nTreeLinesR3[treeline->fPackage][treeline->fPlane] = 1;
                         }
                }
/*
		for ( int j = 0 ; j <5 ; j++)
		{
			cout << nTreeLinesR2[j] << " ";
			
		}
		cout << endl;

		for ( int l = 0 ; l <3 ; l++)
		{
			for (int n = 0; n < 5; n++)
			{
				cout << nTreeLinesR3[l][n] << " ";
			}cout << endl;
		}
		cout << endl;

*/

                //in region 2, in every diection there must be 1 or more treeline to make it a good event
                //in region 3, in each package, each plane must have 1 or more treeline that connect between tht two planes in it makes it a good event
		//BOTH of these must be satisifed in order for this to be a good event under the TreeLine class
		int minTreeLinesR2 = 1;
                int minTreeLinesR3 = 1;
                if ( (nTreeLinesR3[1][0] >= minTreeLinesR3 || 
		 nTreeLinesR3[2][0] >= minTreeLinesR3)
                 && nTreeLinesR2[1] + nTreeLinesR2[3] + nTreeLinesR2[4] > minTreeLinesR2)
                {
                        TreeLines[0]++;
                }

                //define array to store if there is/what the number of Parital Tracks are for each plane
                //R2
                //R3
                int nPartialTracksR2 = 0, nPartialTracksR3 = 0;

                //starting with the PartialTracks
                for (Int_t p = 0 ; p < nPartialTracks; p++)
                {
                        const QwPartialTrack* partialtracks = fEvent->GetPartialTrack(p);

                        if (partialtracks->GetRegion() == 2)
                        {
                                nPartialTracksR2= 1;
                        }else if (partialtracks->GetRegion() == 3)
                                {
                                        nPartialTracksR3 = 1;
                                }
                }
                //in region 2, there must be  1 or more partial tracks to make it a good event
                //in region 3, there must be 1 or more partial track in it makes it a good event
		//BOTH of these must be satisifed in order for this to be a good event under the Partial Tracks class
		int minPartialTracksR2 = 1;
                int minPartialTracksR3 = 1;
                if (nPartialTracksR3 >= minPartialTracksR3 && nPartialTracksR2 >= minPartialTracksR2)
                {
                        PartialTracks[0]++;
                }

                //define array to store if there is/what the number of Tracks 
		// if there is 1 or more tacks in a event then it is a good event
                //starting with the Tracks
		if (nTracks != 0)
		{
			Tracks[0]++;
		} 



	}

	//okay for all events each hit, treeline, partialtrack, and track should be accounted for
	//Right now I will only be using the first one of these - later one might need this to look at the 
	// package differences
	// so lets graph this stuff!

	TString title[3] = { "All Regions - looking for  where/when we are losing tracks"
				, "Region 2 - looking for  where/when we are losing tracks"
				, "Region 3 - looking for  where/when we are losing tracks"};

	//create a vectors of TH1D histogram pointer
	std::vector<TH1D*> h;

	//create a vector of canvas'
	std::vector<TCanvas*> c;

	// size the vectors
	h.resize(3);
	c.resize(3);

	//Create the canvas' and histograms
	for (Int_t w = 0; w <3; w++)
	{
		h[w]= new TH1D (Form("h[%d]",w),title[w],5,0,6);
	}


	TString bin[5] = {"Total Events", "Hits", "TreeLines", "PartialTracks", "Tracks"};
	
	//set the name of each bin in each histogram
	for (Int_t b = 0 ; b < 5 ; b++)
	{
		//set the bin names for each histogram
		//also fill the bins
		for (Int_t j = 0 ; j<3; j++)
		{
			h[j]->GetXaxis()->SetBinLabel(b+1,bin[b]);
		}
	} 
	

	//There has got to be a better way of doing this :( 
	//Fill the bins

	for (Int_t f = 0 ; f < 1 ; f++)
	{
		h[f]->SetBinContent(1,nevents);
		h[f]->SetBinContent(2,Hits[f]);
		h[f]->SetBinContent(3,TreeLines[f]);
		h[f]->SetBinContent(4,PartialTracks[f]);
		h[f]->SetBinContent(5,Tracks[f]);
		
	}
/*
	//Now lets draw these histograms and save them

	for (Int_t d = 0; d < 3 ; d++)
	{
		
		c[d]= new TCanvas (Form("c[%d]",d),title[d],900,600);
		h[d]->Draw();

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c[d]->SaveAs(Prefix+Form("NTracks_Region_%d.png",d));
		c[d]->SaveAs(Prefix+Form("NTracks_Region_%d.C",d));
		
	} 

*/	

	//Now lets graph and save the ONE histogram that is acutally made and used here
	c[0]= new TCanvas ("c[0]",title[0],500,500);
	c[0]->SetLogy();
	h[0]->Draw();

	//Now let's save this
	c[0]->SaveAs(Prefix+"NTracks.png");
	c[0]->SaveAs(Prefix+"NTracks.C");

	// Delete stuff
	for (size_t i = 0; i < h.size(); i++) delete h[i];
	h.resize(0);
	delete c[0];
	delete chain;

	return;
}

