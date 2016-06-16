/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output all the graphs to determine the octnat number 

- the graphs are outbut in the same arragment as the detector bars were set up in 

First output the main detector bar signal vs. tigger scintilltor signal in each 
octant in the same arrangment that the detector bars are set up in for each package in region 3.  In the 
center there is the a graph of the octant that was fed to the analyzer for the package of the canvas.

First output the main detector bar signal in each octant in the same arrangment that the 
detector bars are set up in for each package in region 3.  In the center there is the a 
graph of the octant that was fed to the analyzer for the package of the canvas.

Entry Conditions: the run number, bool for first 100k
Date: 06-13-2012
Modified: 05-05-2014
Assisted By: Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLeaf.h"
#include "QwEvent.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

int octant_to_pad[9] = {5, 4, 1, 2, 3, 6, 9, 8, 7};

void OctantDetermination(int runnum, bool is100k)
{

	// groups root files for a run together
	TChain* event_tree = new TChain ("event_tree");

	//add the root files to the chain the event_tree branches
	event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

	//deifne the prefix as the directory that the files will be outputed to
	//Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/OctantDetermination_%d_",runnum, runnum);

	 Prefix = Form(
	      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_OctantDetermination_",
	      runnum, runnum);

	//define all of the needed histograms for this scripts
        //create a vectors of TH1D histogram pointer
        std:: vector< vector<TH2D*> > h;  //[package][octant]
	h.resize(4);

        std:: vector < vector < vector<TH1D*> > > h2; //[region][package][octant]
	h2.resize(4);

        std:: vector < vector<TH1D*> > h3; //[region][package]
	h3.resize(4);

	for (size_t k = 0; k < h.size() ;k++)
	{
		h[k].resize(9);
	}

	for (size_t s = 0; s < h2.size(); s++)
	{
		h2[s].resize(3);
		for (size_t k = 0; k < h2[s].size(); k++)
		{
			h2[s][k].resize(9);
		}
		h3[s].resize(3);
	}


	//Let's define these histograms
	for (size_t z = 0; z < h.size() ; z ++)
	{
		for (int y = 0; y < 9 ;y++)
		{
			h[z][y]= new TH2D (Form("h[%d][%d]",z,y),"OC",100,0.0,6000.0 ,100, 0.0, 3000.0 );
	                h[z][y]->GetYaxis()->SetTitle(Form("main detector %d signal",y));
        	        h[z][y]->GetXaxis()->SetTitle(Form("Trigger scintillator %d signal",y));

		}
	
		for (int q = 0; q < 3; q++)
		{
			for (int k = 0;k < 9;k++)
			{
				h2[z][q][k] = new TH1D (Form("h[%d][%d][%d]",z,q,k),"MD",100,0.0,6000.0);
		                h2[z][q][k]->GetYaxis()->SetTitle("frequency");
        		        h2[z][q][k]->GetXaxis()->SetTitle(Form("main detector %d signal",k));

			}

			h3[z][q]= new TH1D (Form("h3[%d][%d]",z,q),"passed octant",10,0,10);
	                h3[z][q]->GetYaxis()->SetTitle("frequency");
        	        h3[z][q]->GetXaxis()->SetTitle(Form("passed octatn number for region %d and pacakge %d",z,q));

		}
	}
	

//start with looping over all the events and putting them in the correct histogram 

        //figure out how many evernts are in the rootfile so I know how long to have my loop go for
        Int_t nevents=event_tree->GetEntries();

        //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
        QwEvent* fEvent = 0;

        //Now I have to get a pointer to the events branch to loop through

        //Start by setting the event_tree branch to be on
        event_tree->SetBranchStatus("events",1);
        event_tree->SetBranchStatus("trigscint",1);
        event_tree->SetBranchStatus("maindet",1);

        //now get the event branch of the event_tree branch and call it event_branch creativly
        TBranch* event_branch=event_tree->GetBranch("events");
        TBranch* maindet_branch=event_tree->GetBranch("maindet");
	TBranch* trigscint_branch=event_tree->GetBranch("trigscint");
	event_branch->SetAddress(&fEvent);

	std:: vector<TLeaf*> mdp;  //[octant]
        mdp.resize(9);
	std:: vector<TLeaf*> mdm;  //[octant]
        mdm.resize(9);
	std:: vector<TLeaf*> tsp;  //[package]
        tsp.resize(3);
	std:: vector<TLeaf*> tsm;  //[package]
        tsm.resize(3);
	
	//selects the leaf that we are going to need
	for (int u = 1; u < 9; u++)
	{
		mdp[u]=maindet_branch->GetLeaf(Form("md%dp_adc",u));
		mdm[u]=maindet_branch->GetLeaf(Form("md%dm_adc",u));
	}

	for (int y = 1; y < 3; y++)
	{
		tsp[y]=trigscint_branch->GetLeaf(Form("ts%dp_adc",y));
		tsm[y]=trigscint_branch->GetLeaf(Form("ts%dm_adc",y));
	}
	
	for(int k = 0; k< nevents; k++)
	{
		//Get the ith entry form the event tree
                event_tree->GetEntry(k);

//help i KNOW I am doing this is wrong!!!
		for (int oct = 1 ; oct < 9; oct++)
		{
			for (int package = 1; package < 3; package ++)
			{
				//selesct the events to see octant dependene in the main detector vs. trigger scint.
				h[package][oct]->Fill(mdm[oct]->GetValue()+mdp[oct]->GetValue(),tsm[package]->GetValue()+tsp[package]->GetValue() );
			}
		}
		
		//get the number of Treelines
                int nTreeLines = fEvent->GetNumberOfTreeLines();
	
		// now let's loop through the tree lines and fill all the above histograms
		// this will fill the passed value to the root tree from the analyzer
                for (int t = 0; t < nTreeLines; t++)
                {
                        const QwTreeLine* treeline = fEvent->GetTreeLine(t);
			if (! treeline) continue;
			h3[treeline->GetRegion()][treeline->GetPackage()]->Fill(treeline->GetOctant());
			h2[treeline->GetRegion()][treeline->GetPackage()][treeline->GetOctant()]->Fill(mdm[treeline->GetOctant()]->GetValue()+mdp[treeline->GetOctant()]->GetValue());
			
		}

	}


	for (int pkg = 1; pkg <= 2; pkg++)
	{
		//Create the canvas
		TCanvas c1 ("c1", Form("Octant Determination - for Package %d",pkg), 400,400);

		//divide the canvas
		c1.Divide(3,3);


		//for the first 4 octant draw what is wanted the main dectctor bar singnals vs trigger scintllator signals
		for (int oct1 = 1; oct1 <=8; oct1 ++)
		{
			//select the pad we want to draw on
			c1.cd(octant_to_pad[oct1]);

			//draw what I want - the oct depenece from main detector info 
			h[pkg][oct1]->Draw();			
	
		}

		//selesct the center pad that we want to draw on
		c1.cd(5);		
		h3[3][pkg]->Draw();	

		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
		c1.SaveAs(Prefix+Form("Main_Detector_vs_Trigger_Scint_Package_%d.png",pkg));
		c1.SaveAs(Prefix+Form("Main_Detector_vs_Trigger_Scint_Package_%d.C",pkg));
	}

	for (int reg = 2 ; reg < 4 ;reg ++) 
	{

		for (int pkg2 = 1; pkg2 <= 2; pkg2++)
		{
        		//Create the canvas
        		TCanvas c2("c2", Form("Main detector signal for each oct for Region %d Package %d", reg, pkg2), 400,400);

        		//divide the canvas
        		c2.Divide(3,3);

			//for the first 4 octant draw what is wanted the main dectctor bar singnals vs trigger scintllator signals
        		for (int oct3 = 1; oct3 <=8; oct3 ++)
        		{
                		//select the pad we want to draw on
                		c2.cd(octant_to_pad[oct3]);

                		//draw what I want - the oct depenece from main detector info
				h2[reg][pkg2][oct3]->Draw();

        		}

			//selesct the center pad that we want to draw on
                	c2.cd(5);

			h3[reg][pkg2]->Draw();	
		
        		//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
       			c2.SaveAs(Prefix+Form("Main_Detector_Region_%d_Package_%d.png", reg, pkg2));
       			c2.SaveAs(Prefix+Form("Main_Detector_Region_%d_Package_%d.C", reg, pkg2));
		}	

	}

  //print to file
  //this file and evrything related to it is fout
  std::ofstream fout;
  std::ofstream fout1;
  std::ofstream fout2;
  std::ofstream fout3;

  //open file
  // open file with outputPrefix+q2.txt which will store the output of the vlaues
  //to a file in a easy way that should be able to be read back into a program if needeD
  fout.open(Prefix+"R2_pkg1.txt");
  if (!fout.is_open()) cout << "File not opened R2_1" << endl;
  fout1.open(Prefix+"R2_pkg2.txt");
  if (!fout1.is_open()) cout << "File not opened R2_2" << endl;

  //what are we printing
  fout << h3[2][1]->GetMean() << endl;
  fout1 << h3[2][2]->GetMean() << endl;
  //close the file
  fout.close();
  fout1.close();

  fout2.open(Prefix+"R3_pkg1.txt");
  if (!fout2.is_open()) cout << "File not opened R3_1" << endl;
  fout3.open(Prefix+"R3_pkg2.txt");
  if (!fout3.is_open()) cout << "File not opened R3_2" << endl;

  //what are we printing
  fout2  << h3[3][1]->GetMean() << endl;
  fout3  << h3[3][2]->GetMean() << endl;
  //close the file
  fout2.close();
  fout3.close();


  return;
}

