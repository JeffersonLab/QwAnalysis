/*********************************************************** 
Programmer: Valerie Gray
Purpose: To output graph of interst when it comes to light weighting.

Plots:
- light yield vs. position on bar (in cm)
- Q2 vs position on bar
- Scattering angle  vs position on bar 
- Q2 w/ light weighting w/o pedestal
- Q2 w/o light weighitng
- Q2 w/ light wieghitn and pedestal

there are two text file which this outputs one with how thw Q2 changes
with the different cuts, where the Q2 in in m(GeV)^2, and the other is
the depenedce of Q2, light yield, and scattering angle on the bar - 
for this one Q2 is in GeV^2, light yeild is in ADC units, and 
scatting angle is in degrees.

Entry Conditions: the run number, bool for first 100k
Date: 09-12-2012
Modified:05-08-2014
Assisted By: Wouter Deconinck
*********************************************************/

#include "auto_shared_includes.h"
#include "TSystem.h"

#include "TLeaf.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"

#include "QwEvent.h"
#include "QwTrack.h"

#include <fstream>
#include <iostream>

//define a prefix for all the output files - global don't need to pass
TString Prefix;

//define the distance for all 8 octants from the target to bar
double MD_bars_dist[9] = {0.0, 576.665, 576.705, 577.020, 577.425, 577.515, 577.955, 577.885, 577.060};

//define the low Q2 cut off value
double Q2_cut_off = 0.060; //GeV
//defince constants for the max light yield and Scattering angle for profile histograms
double Max_light_yield = 3000;
double Max_Scat_angle = 20.0;

int bin_size = 300; // bin size for the bar at 300 there is a bin every .5 cm
double bar_length = 150; // half the bar length in cm 

//in case the pedestal and cut off is different in each octant this makes the code flexiable 
double adc_cut_off[9] = {0.0, 600.00, 600.00, 600.00, 600.00, 600.00, 600.00, 600.00, 600.00}; //units?? would be good to know
double adc_pedestal[9] = {0.0, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000, 550.000}; // units adc units or a.u. (arbitrary units)

//define a functions 
int DetermineOctantRegion3(TChain* event_tree, int package);


/***********************************
set up the significant figures right - &plusmn is for html,
  change to +/- if not useing,
  or in this case \t for a tab space in the outputfile
Use this by calling: value_with_error(a,da)
***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
 << std::setprecision((size_t) (std::log10((a)/(da)) - std::log10(a) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)


void LightWeighting (int runnum, bool is100k)
{
  //changed the outputPrefix so that it is compatble with both Root 
  //and writing to a file by setting the enviromnet properly 
  //Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/%d_",runnum,runnum);

  Prefix = Form(
      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
      runnum, runnum);

  // Create and load the chain
  TChain *event_tree = new TChain("event_tree");
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

//get the octants for package one and package 2 in region 3
//read in or figure out?
  int oct[3]= {0, 0, 0};
  oct[1] = DetermineOctantRegion3(event_tree, 1); //octant number for package one
  oct[2] = DetermineOctantRegion3(event_tree, 2); //octant number for package two

  //define the histograms
  //create a vector of vectors of TH1D histogram pointer
  //lw = light weighting
  //p = pedestal
  std::vector< TH2D*> h_q2_proj; //q2 projection back to the MD bar
  std::vector< TH2D*> h_lw_proj; //light weighting projection to MD bar
  std::vector< TH2D*> h_sa_proj; //scatterieng angle  projection back to the MD bar
  std::vector< TProfile* > h_q2_prof; //profile of q2 projection back to the MD bar
  std::vector< TProfile* > h_lw_prof; //profile of light weighting projection to MD bar
  std::vector< TProfile* > h_sa_prof; //proflie of scatterieng angle  projection back to the MD bar
  std::vector< TH1D*> h_x; //x position  on the bar
  std::vector< TH1D*> h_q2; //q2 w/o light weighting 
  std::vector< TH1D*> h_q2_lw; //q2 w/ light weighting and NO pedestal
  std::vector< TH1D*> h_q2_lw_p; //q2 w/ light weighting and including the pedestal

  // size all these vectors
  h_q2_proj.resize(3);
  h_lw_proj.resize(3);
  h_sa_proj.resize(3);

  h_q2_prof.resize(3);
  h_lw_prof.resize(3);
  h_sa_prof.resize(3);

  h_x.resize(3);
  h_q2.resize(3);
  h_q2_lw.resize(3);
  h_q2_lw_p.resize(3);

	//Let's define these histograms
	for (size_t i = 0; i < h_q2.size() ; i ++)
	{
		h_q2_proj[i]= new TH2D (Form("h_q2_proj[%d]",i),Form("Q2 vs. position on bar for Octant %d, Package %d",oct[i],i),bin_size, -bar_length, bar_length, 100, 0.0, Q2_cut_off);
		h_q2_proj[i]->GetYaxis()->SetTitle("Q2 (GeV^2)");
		h_q2_proj[i]->GetXaxis()->SetTitle("position on the bar (cm)");

    h_lw_proj[i]= new TH2D (Form("h_lw_proj[%d]",i),Form("Light Yield vs. position on bar for Octant %d, Package %d",oct[i],i),bin_size, -bar_length, bar_length , 100, 0.0, Max_light_yield);
    h_lw_proj[i]->GetYaxis()->SetTitle("Light yield (a.u.)");
    h_lw_proj[i]->GetXaxis()->SetTitle("position on the bar (cm)");

    h_sa_proj[i]= new TH2D (Form("h_sa_proj[%d]",i),Form("Scattering Angle vs. position on bar for Octant %d, Package %d",oct[i],i),bin_size, -bar_length, bar_length , 100, 0.0, Max_Scat_angle);
    h_sa_proj[i]->GetYaxis()->SetTitle("Sacttering angle (degrees)");
    h_sa_proj[i]->GetXaxis()->SetTitle("position on the bar (cm)");

    h_x[i]= new TH1D (Form("h_x[%d]",i),Form("Position on bar for Octant %d, Package %d",oct[i],i),bin_size, -bar_length, bar_length);
		h_x[i]->GetYaxis()->SetTitle("Frequency");
    h_x[i]->GetXaxis()->SetTitle("position on the bar (cm)");

    h_q2[i]= new TH1D (Form("h_q2[%d]",i),Form("Q2 - unweighted for Octant %d, Package %d",oct[i],i),100,0,0.12);
		h_q2[i]->GetYaxis()->SetTitle("Frequency");
    h_q2[i]->GetXaxis()->SetTitle("Q2 (GeV^2)");

    h_q2_lw[i]= new TH1D (Form("h_q2_lw[%d]",i),Form("Q2 - light weighted no pedestal for Octant %d, Package %d",oct[i],i),100,0,0.12);
		h_q2_lw[i]->GetYaxis()->SetTitle("Frequency");
    h_q2_lw[i]->GetXaxis()->SetTitle("Q2 (GeV^2)");

    h_q2_lw_p[i]= new TH1D (Form("h_q2_lw_p[%d]",i),Form("Q2 - light weighted with pedestal for Octant %d, Package %d",oct[i],i),100,0,0.12);
  	h_q2_lw_p[i]->GetYaxis()->SetTitle("Frequency");
 		h_q2_lw_p[i]->GetXaxis()->SetTitle("Q2 (GeV^2)");

	}

	//figure out how many evernts are in the rootfile so I know how long to have my loop go for
  Int_t nevents=event_tree->GetEntries();

  //To start this I think that I might have to define a QwEvent as a pointer - Why I have no idea :(
  QwEvent* fEvent = 0;

  //Now I have to get a pointer to the events branch to loop through

  //Now get the event branch of the event_tree branch and call it event_branch creatively
  TBranch* event_branch = event_tree->GetBranch("events");
  event_branch->SetAddress(&fEvent);
  //Set events branch to be ON
  event_tree->SetBranchStatus("events",1);

  //Also get the maindet branch- we will need the leaves for the adcs
  TBranch* maindet_branch = event_tree->GetBranch("maindet");
  //Set maindet branch to be ON, if it exists
  if (maindet_branch)
    event_tree->SetBranchStatus("maindet",1);

  //get the leaves of the ADCs
  //here the 1 and 2 coresopnd to the package number
  TLeaf* mdp[3];
  TLeaf* mdm[3];
  mdp[1] = maindet_branch? maindet_branch->GetLeaf(Form("md%dp_adc",oct[1])): 0;
  mdm[1] = maindet_branch? maindet_branch->GetLeaf(Form("md%dm_adc",oct[1])): 0;
  mdp[2] = maindet_branch? maindet_branch->GetLeaf(Form("md%dp_adc",oct[2])): 0;
  mdm[2] = maindet_branch? maindet_branch->GetLeaf(Form("md%dm_adc",oct[2])): 0;

  //Loop through this and fill all the graphs at once

  for (int i = 0; i < nevents; i++)
  {
    //Get the ith entry form the event tree
    if (event_branch)   event_branch->GetEntry(i);
    if (maindet_branch) maindet_branch->GetEntry(i);

    //get the number of Treelines
    int nTracks = fEvent->GetNumberOfTracks();

    // now let's loop through the tree lines and fill all the above histograms
    for (int t = 0; t < nTracks; t++)
    {
			//get a specific treeline
			const QwTrack* tracks = fEvent->GetTrack(t);

			// get the package
			int pkg = tracks->fPackage;

			// what is the MD+ + MD- ADC sum?
			// first test whether mdp[pkg] and mdm[pkg] even exist (data), if they don't exist (simulation) add 50 to the cut off to be above it
			double mdsum = (mdp[pkg] && mdm[pkg])? mdp[pkg]->GetValue() + mdm[pkg]->GetValue(): adc_cut_off[oct[pkg]] + 50.0;

			// if Q2 < Q2 cut-off && MD+ + MD- > ADC cut-off
			if (abs(fEvent->fKin.fQ2) < Q2_cut_off && mdsum > adc_cut_off[oct[pkg]])
			{
				// Original coordinates in global coordinate system
				double x0 = tracks->fBack->fOffsetX + MD_bars_dist[oct[pkg]] * tracks->fBack->fSlopeX;
				double y0 = tracks->fBack->fOffsetY + MD_bars_dist[oct[pkg]] * tracks->fBack->fSlopeY;

				// Rotation to get into local coordinate system of octant 3
				double angle = 3.1415927/4 * (oct[pkg] - 3);

				// Rotate over angle
				double x1 =  cos(angle) * x0 + sin(angle) * y0;
				double y1 = -sin(angle) * x0 + cos(angle) * y0;

				// Fill histograms
				h_x[pkg]->Fill(x1);
				h_q2_proj[pkg]->Fill(x1, fEvent->fKin.fQ2);
				h_lw_proj[pkg]->Fill(x1, mdsum);
				h_sa_proj[pkg]->Fill(x1, fEvent->fScatteringAngle);
				h_q2[pkg]->Fill(fEvent->fKin.fQ2);
				h_q2_lw[pkg]->Fill(fEvent->fKin.fQ2, (mdsum));
				h_q2_lw_p[pkg]->Fill(fEvent->fKin.fQ2, (mdsum - adc_pedestal[pkg]));
			}
    }
  }

  // create profile histogram from 2d histograms
  for (size_t pkg =1; pkg < h_q2.size(); pkg ++)
	{
	  h_q2_prof[pkg] = h_q2_proj[pkg]->ProfileX(Form("h_q2_prof[%d]",pkg));
  	h_lw_prof[pkg] = h_lw_proj[pkg]->ProfileX(Form("h_lw_prof[%d]",pkg));
  	h_sa_prof[pkg] = h_sa_proj[pkg]->ProfileX(Form("h_sa_prof[%d]",pkg));
	}

	//define canvases and draw the histograms and profiles
	//create a vector of vectors of TH1D histogram pointer
	//lw = light weighting
	//p = pedestal
  TCanvas* c_q2_proj = new TCanvas ("c_q2_proj","Q2 vs. position on bar", 700,800);
  TCanvas* c_lw_proj = new TCanvas ("c_lw_proj","Light yield vs. position on bar", 700,800);
  TCanvas* c_sa_proj = new TCanvas ("c_sa_proj","Scattering Angle vs. position on bar", 700,800);
  TCanvas* c_q2_lw = new TCanvas ("c_q2_lw", "Q2 with light wieghting and no pedestal", 700,800);
  TCanvas* c_q2 = new TCanvas ("c_q2", "Q2", 700,800);
  TCanvas* c_q2_lw_p = new TCanvas ("c_q2_lw_p","Q2 value with light wieghting and pedestal", 700,800);

	//divide all the canvases so that pkg one and two are next to each other
	c_q2_proj->Divide(0,2);
	c_lw_proj->Divide(0,2);
	c_sa_proj->Divide(0,2);
	c_q2_lw->Divide(2,0);
	c_q2->Divide(2,0);
	c_q2_lw_p->Divide(2,0);

	for (size_t pkg =1; pkg < h_q2.size(); pkg ++)
	{
		c_q2_proj->cd(pkg);
		h_q2_proj[pkg]->Draw();
    h_q2_prof[pkg]->SetLineColor(kRed);
		h_q2_prof[pkg]->Draw("same");

		c_lw_proj->cd(pkg);
		h_lw_proj[pkg]->Draw();
    h_lw_prof[pkg]->SetLineColor(kRed);
		h_lw_prof[pkg]->Draw("same");

		c_sa_proj->cd(pkg);
		h_sa_proj[pkg]->Draw();
    h_sa_prof[pkg]->SetLineColor(kRed);
		h_sa_prof[pkg]->Draw("same");

		c_q2_lw->cd(pkg);
		h_q2_lw[pkg]->Draw();

		c_q2->cd(pkg);
		h_q2[pkg]->Draw();

		c_q2_lw_p->cd(pkg);
		h_q2_lw_p[pkg]->Draw();

	}
	
	//save the canvas as a png file - right now it goes to the $QWSCRATCH/tracking/www/ directory
	c_q2_proj->SaveAs(Prefix+"Q2_vs_position_on_bar.png");
	c_q2_proj->SaveAs(Prefix+"Q2_vs_position_on_bar.C");
	c_lw_proj->SaveAs(Prefix+"Light_yield_vs_position_on_bar.png");
	c_lw_proj->SaveAs(Prefix+"Light_yield_vs_position_on_bar.C");
	c_sa_proj->SaveAs(Prefix+"Scattering_angle_vs_position_on_bar.png");
	c_sa_proj->SaveAs(Prefix+"Scattering_angle_vs_position_on_bar.C");
	c_q2_lw->SaveAs(Prefix+"Q2_with_light_wieghting_and_NO_pedestal.png");
	c_q2_lw->SaveAs(Prefix+"Q2_with_light_wieghting_and_NO_pedestal.C");
	c_q2->SaveAs(Prefix+"Q2.png");
	c_q2->SaveAs(Prefix+"Q2.C");
	c_q2_lw_p->SaveAs(Prefix+"Q2_light_wieghting_and_pedestal.png");
	c_q2_lw_p->SaveAs(Prefix+"Q2_light_wieghting_and_pedestal.C");


//need to get a table of the values form the profile for bot Q2 and light weight vs. bar poition.....	

	//print to file 
  //this file and evrything related to it is fout
  std::ofstream fout;
	std::ofstream fout2;
    
  //open file
  // open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if needed
  fout.open(Prefix+"Everything_vs_position_on_bar.txt");
  if (!fout.is_open()) cout << "File not opened" << endl;

	//Name what each coulmn is Note Package 0 is the compination of package 1 and package 2
	fout << "Run \t Pkg \t bar pos. \t # Tracks \t Q2 \t Q2 Error \t light yield \t LT Error \t scat. angle \t SA Error" <<endl;

	//some way to loop over all of this
	for (size_t pkg = 1; pkg < h_q2.size(); pkg++)
	{
		for (int bin = 0; bin < bin_size; bin++)
		{
			fout << runnum << " \t " << pkg << " \t " << std::setprecision(10) << h_q2_prof[pkg]->GetBinCenter(bin) << " \t " <<
							std::setprecision(10) << h_x[pkg]->GetBinContent(bin) << " \t" <<
							value_with_error(h_q2_prof[pkg]->GetBinContent(bin), h_q2_prof[pkg]->GetBinError(bin)) << " \t" <<
							value_with_error(h_lw_prof[pkg]->GetBinContent(bin), h_lw_prof[pkg]->GetBinError(bin)) << " \t" <<
							value_with_error(h_sa_prof[pkg]->GetBinContent(bin), h_sa_prof[pkg]->GetBinError(bin)) << endl;
		}
	}
	//close the file
	fout.close();

  //open file
  // open file with outputPrefix+q2.txt which will store the output of the vlaues to a file in a easy way that should be able to be read back into a program if needed
  fout2.open(Prefix+"Q2_with_cuts.txt");
  if (!fout2.is_open()) cout << "File not opened" << endl;

	//Name what each coulmn is Note Package 0 is the compination of package 1 and package 2
	fout2 << "Run \t pkg \t  Q2 \t Q2 Error \t LW Q2 \t LW Q2 Error \t LW & P Q2 \t LW & P Q2 Error " <<endl;

	//some way to loop over all of this
	for (size_t pkg = 1; pkg < h_q2.size(); pkg++)
	{
			fout2 << runnum << " \t " << pkg << " \t" <<
					value_with_error(1000*h_q2[pkg]->GetMean(), 1000*h_q2[pkg]-> GetRMS()/sqrt(h_q2[pkg]->GetEntries())) << " \t"  <<
					value_with_error(1000*h_q2_lw[pkg]->GetMean(), 1000*h_q2[pkg]-> GetRMS()/sqrt(h_q2_lw[pkg]->GetEntries())) << " \t"  <<
					value_with_error(1000*h_q2_lw_p[pkg]->GetMean(), 1000*h_q2[pkg]-> GetRMS()/sqrt(h_q2_lw_p[pkg]->GetEntries())) << endl;

	}
	//close the file
	fout2.close();
    
}

/*********************************************************** 
Function: DetermineOctantRegion3
Purpose: To determinae the octant for a certain package in Region 3
Entry Conditions: TChain - event_tree
				int - package
Global:
Return: int - octant 
Called By: LightWeighting
Date: 09-14-2012
Modified:
*********************************************************/

int DetermineOctantRegion3(TChain* event_tree, int package)
{
  if (event_tree->GetBranch("maindet")) {
    // The maindet branch is present (actual data)
    for (int octant = 1; octant <= 8; octant++)
    {
      TH1D* h_adc = new TH1D("h_adc","h_adc",128,0,4096);
      event_tree->Draw(Form("maindet.md%dm_adc + maindet.md%dp_adc >> h_adc",octant,octant),
                       Form("events.fQwTracks.fPackage==%d",package),"",1000);
      double mean = h_adc->GetMean();
      delete h_adc;
      if (mean > adc_pedestal[octant] + 50.0) return octant;
    }
  } else {
    // The maindet branch is NOT present (simulation)
    TH1D* h_oct = new TH1D("h_oct","h_oct",9,0,9);
    event_tree->Draw("events.fQwHits.fOctant >> h_oct",
                Form("events.fQwTracks.fPackage==%d && events.fQwHits.fRegion==3",package),"",1000);
    int oct = (h_oct->GetMean() + 0.5);
    delete h_oct;
    return oct;
  }
  return 0;
}
