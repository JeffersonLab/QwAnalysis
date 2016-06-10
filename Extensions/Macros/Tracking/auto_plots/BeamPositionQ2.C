/*****************************************

Programmer: Melissa Guidry
Purpose: To output all graphs of Q2 vs. x and
  y positions of rastered beam on bar.

Modified: 05-07-2014 by Valerie Gray
  Changed the output to add in the slope of the
  both pakages combination

*****************************************/

#include "auto_shared_includes.h"

#include "TSystem.h"
#include "TProfile.h"
#include "QwEvent.h"
#include "TLeaf.h"
#include "TF1.h"
#include "TPaveStats.h"
#include <TChain.h>

#include <iostream>
#include <fstream>

/***********************************
set up the significant figures right - &plusmn is for html,
  change to +/- if not useing,
  or in this case \t for a tab space in the outputfile
Use this by calling: value_with_error(a,da)
***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
 << std::setprecision((size_t) (std::log10((a)/(da)) - std::log10(a) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)

// Define Octant number function
int getOctNumber(TChain* event_tree)
{
  // Generate histogram h of the octant cutting on region 3 and pacakge 2. "GOFF" to turn off graphics of h
  TH1F* h = new TH1F("h","Region 3, Package 2 Octant number",9,-0.5,8.5);
  event_tree->Draw("events.fQwPartialTracks.fOctant>>h","events.fQwPartialTracks.fRegion==3&&events.fQwPartialTracks.fPackage==2","GOFF",100000);

  // Mean of histgram h, this returns a double value that is the trunkated to integer which is the region 3 pacakge 2 octant number
  int octant = int (h->GetMean() + 0.5);
  delete h;

  return (octant);
}

// Define the function (does not return value)
void BeamPositionQ2(int runnum, bool is100k)
{
      // Define the preliminary variables
      int bin_size = 100;
      int min_Q2 = 0;
      int max_Q2 = 60;

      //TString Prefix = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/BeamPositionQ2_%d_",runnum,runnum);

      TString Prefix = Form(
          TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
          runnum, runnum);

      TChain* event_tree = new TChain ("event_tree");

      event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));
//      event_tree->Add("~/scratch/rootfiles/Qweak_15121.root");

      std::string package[3] = {"Both Packages", "Package 1","Package 2"}; //3

      // Find number of events for filling the profiles
      Int_t nevents = event_tree->GetEntries();

      QwEvent* fEvent = 0;

      event_tree->SetBranchStatus("events", 1);

      TBranch* event_branch = event_tree->GetBranch("events");
      event_branch->SetAddress(&fEvent);

      // Pull out the raw adc position data
      TBranch* beamline_branch = event_tree->GetBranch("beamline");
      if (beamline_branch) {
        TLeaf* raster_posx = beamline_branch->GetLeaf("raster_posx_adc_raw");
        TLeaf* raster_posy = beamline_branch->GetLeaf("raster_posy_adc_raw");
      } else {
        cout << "Error: no beamline branch found (this script does not support simulation files)." << endl;
        return;
      }

      TChain* slow_tree = new TChain ("Slow_Tree");
      slow_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

      // Pull out the raster widths for x and y
      TBranch* rasterX = slow_tree->GetBranch("EHCFR_LIXWidth");
      TLeaf* rasterX_leaf = rasterX->GetLeaf("EHCFR_LIXWidth");

      TBranch* rasterY = slow_tree->GetBranch("EHCFR_LIYWidth");
      TLeaf* rasterY_leaf = rasterY->GetLeaf("EHCFR_LIYWidth");

      slow_tree->GetEntry();

      // Actually pull raster size as float
      float RasterX_size = float(rasterX_leaf->GetValue());
      float RasterY_size = float(rasterY_leaf->GetValue());

      // Draw histograms of position data -- do not draw
      TH1D h_posx("h_posx","X-Position",100,1000.0,2000.0);
      TH1D h_posy("h_posy","Y-Position",100,1000.0,2000.0);
      event_tree->Draw("beamline.raster_posx_adc_raw>>h_posx","","",1000);
      event_tree->Draw("beamline.raster_posy_adc_raw>>h_posy","","",1000);

      // Take the limits of the position data -- cannot use GetMin() and GetMax() on TLeafs
      int first_bin_above_x = h_posx.FindFirstBinAbove(0);
      int first_bin_above_y = h_posy.FindFirstBinAbove(0);
      int last_bin_above_x = h_posx.FindLastBinAbove(0);
      int last_bin_above_y = h_posy.FindLastBinAbove(0);

      int Min_posx = h_posx.GetBinCenter(first_bin_above_x);
      int Max_posx = h_posx.GetBinCenter(last_bin_above_x);
      int Min_posy = h_posy.GetBinCenter(first_bin_above_y);
      int Max_posy = h_posy.GetBinCenter(last_bin_above_y);

      // Define the scaling factors for converting raw adc units to mm
      float Scaling_factor_x = RasterX_size / (Max_posx - Min_posx);
      float Scaling_factor_y = RasterY_size / (Max_posy - Min_posy);

      // A transformation for centering the histograms at zero
      float Transform_x =  (Max_posx - Min_posx)/2;
      float Transform_y =  (Max_posy - Min_posy)/2;

      // Grab the octant numbers
      int oct = getOctNumber(event_tree);
      if (oct == 0 || oct > 8)
      {
        cout << "octant not valid!" << endl;
        oct = 1;
      }

      int octpkg1 = (oct+4)%8;
      int octpkg2 = oct;

      //create a vector of vectors of TProfile histogram pointer
      std::vector <TProfile*> Q2BeamXPositionTProfile;    //[package]
      std::vector <TProfile*> Q2BeamYPositionTProfile;
      std::vector <TF1*> fitx;
      std::vector <TF1*> fity;

      Q2BeamXPositionTProfile.resize(3);
      fitx.resize(3);

      // Create X and Y Position TProfiles, titles on axis, fit them
      for (int j  = 0; j < Q2BeamXPositionTProfile.size(); j ++)
      {
          Q2BeamXPositionTProfile[j] = new TProfile(Form("Q2BeamXPositionTProfile[%d]",j), Form("Q^2 vs X Beam Position for %s",package[j].c_str()), bin_size,(-1/2)*RasterX_size,(1/2)*RasterX_size, min_Q2, max_Q2);
          Q2BeamXPositionTProfile[j]->GetYaxis()->SetTitle("Q^2 (m(GeV)^2)");
          Q2BeamXPositionTProfile[j]->GetXaxis()->SetTitle("X-Position (mm)");

          fitx[j] = new TF1 (Form("fitx[%d]",j),"pol1", (-1/2)*RasterX_size, (1/2)*RasterX_size);
       }

       Q2BeamYPositionTProfile.resize(3);
       fity.resize(3);

       for (int j  = 0; j < Q2BeamYPositionTProfile.size(); j ++)
       {

          Q2BeamYPositionTProfile[j]= new TProfile(Form("Q2BeamYPositionTProfile[%d]",j), Form("Q^2 vs Y Beam Position for %s",package[j].c_str()), bin_size, (-1/2)*RasterY_size, (1/2)*RasterY_size, min_Q2, max_Q2);
          Q2BeamYPositionTProfile[j]->GetYaxis()->SetTitle("Q^2 (m(GeV)^2)");
          Q2BeamYPositionTProfile[j]->GetXaxis()->SetTitle("Y-Position (mm)");

          fity[j] = new TF1 (Form("fity[%d]",j),"pol1", (-1/2)*RasterY_size, (1/2)*RasterY_size);
       }

      // Fill the profiles with position and momentum transfer values
      for (int i = 0; i < nevents; i++)
      {

      event_tree->GetEvent(i);
      int nTracks = fEvent->GetNumberOfTracks();

        for (int k = 0; k < nTracks; k++)
        {
          const QwTrack* track = fEvent->GetTrack(k);

          Q2BeamXPositionTProfile[track->GetPackage()]->Fill((raster_posx->GetValue()-Min_posx-Transform_x)*Scaling_factor_x, fEvent->fPrimaryQ2*1000);
          Q2BeamYPositionTProfile[track->GetPackage()]->Fill((raster_posy->GetValue()-Min_posy-Transform_y)*Scaling_factor_y, fEvent->fPrimaryQ2*1000);

          Q2BeamXPositionTProfile[0]->Fill((raster_posx->GetValue()-Min_posx-Transform_x)*Scaling_factor_x, fEvent->fPrimaryQ2*1000);
          Q2BeamYPositionTProfile[0]->Fill((raster_posy->GetValue()-Min_posy-Transform_y)*Scaling_factor_y, fEvent->fPrimaryQ2*1000);

        }
       }

//********  Stat Box Values  **********************************

      Q2BeamXPositionTProfile[0]->Fit("fitx[0]");
      Q2BeamYPositionTProfile[0]->Fit("fity[0]");

      fitx[0]->GetChisquare();
      fitx[0]->GetParameters();
      fity[0]->GetChisquare();
      fity[0]->GetParameters();

      Q2BeamXPositionTProfile[1]->Fit("fitx[1]");
      Q2BeamYPositionTProfile[1]->Fit("fity[1]");

      fitx[1]->GetChisquare();
      fitx[1]->GetParameters();
      fity[1]->GetChisquare();
      fity[1]->GetParameters();

      Q2BeamXPositionTProfile[2]->Fit("fitx[2]");
      Q2BeamYPositionTProfile[2]->Fit("fity[2]");

      fitx[2]->GetChisquare();
      fitx[2]->GetParameters();
      fity[2]->GetChisquare();
      fity[2]->GetParameters();

// ------------Output Things------------------

      std::ofstream Output_file_x;
      std::ofstream Output_file_y;

      Output_file_x.open(Prefix+"BeamPositionQ2_X.txt");
      Output_file_y.open(Prefix+"BeamPositionQ2_Y.txt");

//      Output_file_x.open("/home/maguidry/TestScripts/Output/BPQ2_Output_x.txt");
 //     Output_file_y.open("/home/maguidry/TestScripts/Output/BPQ2_Output_y.txt");

      if (!Output_file_x.is_open()) cout<<"File not opened."<<endl;
      if (!Output_file_y.is_open()) cout<<"File not opened."<<endl;

      Output_file_x << "Package \t Octant \t Chi^2 \t Slope \t Error" << endl;
      Output_file_x << "0 \t " << "NA" <<" \t "<< setprecision(5) <<  fitx[0]->GetChisquare()/fitx[0]->GetNDF() <<
        " \t" << value_with_error(fitx[0]->GetParameter(1), fitx[0]->GetParError(1)) << endl;
      Output_file_x << "1 \t " << octpkg1 <<" \t "<< setprecision(5) <<  fitx[1]->GetChisquare()/fitx[1]->GetNDF() <<
        " \t" << value_with_error(fitx[1]->GetParameter(1), fitx[1]->GetParError(1)) << endl;
      Output_file_x << "2 \t " << octpkg2 <<" \t "<< setprecision(5) <<  fitx[2]->GetChisquare()/fitx[2]->GetNDF() <<
        " \t" << value_with_error(fitx[2]->GetParameter(1), fitx[2]->GetParError(1)) << endl;

      Output_file_y << "Package \t Octant \t Chi^2 \t Slope \t Error " << endl;
      Output_file_y << "0 \t " << "NA" <<" \t "<< setprecision(5) <<  fity[0]->GetChisquare()/fitx[1]->GetNDF() <<
        " \t " << value_with_error(fity[0]->GetParameter(1), fity[0]->GetParError(1)) << endl;
      Output_file_y << "1 \t " << octpkg1 <<" \t "<< setprecision(5) <<  fity[1]->GetChisquare()/fitx[1]->GetNDF() <<
        " \t" << value_with_error(fity[1]->GetParameter(1), fity[1]->GetParError(1)) << endl;
      Output_file_y << "2 \t " << octpkg2 <<" \t "<< setprecision(5) <<  fity[2]->GetChisquare()/fitx[2]->GetNDF() <<
        " \t" << value_with_error(fity[2]->GetParameter(1), fity[2]->GetParError(1)) << endl;

      Output_file_x.close();
      Output_file_y.close();

//-------------Draw Things--------------------

      TCanvas* Q2BeamXPositionTCanvas = new TCanvas("Q2BeamXPositionTCanvas", "Q^2 vs. x position", 450, 350);
      TCanvas* Q2BeamYPositionTCanvas = new TCanvas("Q2BeamYPositionTCanvas", "Q^2 vs. y position", 450, 350);

      Q2BeamXPositionTCanvas->Divide(3);
      Q2BeamYPositionTCanvas->Divide(3);

      Q2BeamXPositionTCanvas->cd(1);
      Q2BeamXPositionTProfile[0]->Draw();
      Q2BeamXPositionTCanvas->cd(2);
      Q2BeamXPositionTProfile[1]->Draw();
      Q2BeamXPositionTCanvas->cd(3);
      Q2BeamXPositionTProfile[2]->Draw();

      gStyle->SetOptFit(1);

//      Q2BeamXPositionTCanvas->SaveAs("~/TestScripts/Output/Q2_vs_beamxposition.png");
//      Q2BeamXPositionTCanvas->SaveAs("~/TestScripts/Output/Q2_vs_beamxposition.C");
      Q2BeamXPositionTCanvas->SaveAs(Prefix+"Q2_vs_beamxposition.png");
      Q2BeamXPositionTCanvas->SaveAs(Prefix+"Q2_vs_beamxposition.C");

      Q2BeamYPositionTCanvas->cd(1);
      Q2BeamYPositionTProfile[0]->Draw();
      Q2BeamYPositionTCanvas->cd(2);
      Q2BeamYPositionTProfile[1]->Draw();
      Q2BeamYPositionTCanvas->cd(3);
      Q2BeamYPositionTProfile[2]->Draw();

      gStyle->SetOptFit(1);

//      Q2BeamYPositionTCanvas->SaveAs("~/TestScripts/Output/Q2_vs_beamyposition.png");
//      Q2BeamYPositionTCanvas->SaveAs("~/TestScripts/Output/Q2_vs_beamyposition.C");
      Q2BeamYPositionTCanvas->SaveAs(Prefix+"Q2_vs_beamyposition.png");
      Q2BeamYPositionTCanvas->SaveAs(Prefix+"Q2_vs_beamyposition.C");

//      hprof.SaveAs(Prefix+"Q2_vs_beamposition.png");
//      hprof.SaveAs(Prefix+"Q2_vs_beamposition.C");
      //.SaveAs("~/Q2_vs_beamposition.png");
      //.SaveAs("~/Q2_vs_beamposition.C");

return;

}
