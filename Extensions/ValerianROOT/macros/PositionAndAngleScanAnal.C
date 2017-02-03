/***********************************************************
 Programmer: Valerie Gray
 Purpose: To graph that file data from a txt file and give me the stat for beam Position or angle and save that graph

 Copied and modified of
 https://qweaksvn.jlab.org/repos/Simulation/trunk/QWGEANT4/analysis/SimulationAnalPositionAndAngle.C

 Use #murad for angle :)

 Entry Conditions:
 filename - one octants text file of Q2 data - Blue pkg 2
 filename2 - secont octants text file of Q2 data - Red pkg 1
 direction - what direction the beam is being moved in (X or Y)
 what - what is being moved beam angle or position
 units - for what is being moved (mm or rad)
 octant - the lowest octant number of the two octants of interst
 Date: 05-23-2012
 Modified: 01-24-2013
 Assisted By: Juan Carlos
 *********************************************************/

#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TPaveStats.h>
#include <TLegend.h>
#include <stdlib.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TLatex.h>

using namespace std;

void PositionAndAngleScanAnal(
                              string filename,
                                string filename2,
                                string dir,
                                string what,
                                string units,
                                int oct)
{

  //Calculate the octant in pkg 2
  //Octant 2 is 4 away from ocatant 1
  //But if oct = 5 then oct1 is 1, this is taken care of with the mod (%) opporator
  int oct2 = (oct + 4) % 8;

  TCanvas c1(
      Form("Octant %d and %d - Q2 vs %s beam position", oct, oct2,
           dir.c_str()));

  //create a mulitgraph - so that one can plot 2 graphs on the same graph
  TMultiGraph * mg = new TMultiGraph();

  //add a file to the TGraphErrors - which is at graph formed from data that has error bars
  //for the first graph
  /*
   TGraphErrors * gr1 =
   new TGraphErrors(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Sim_Pass2_Analysis/sim_runlist_1/Q2_vs_BeamPos/%s.txt",
   filename.c_str()));
   */
  /*
   TGraphErrors * gr1 =
   new TGraphErrors(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass2_Analysis/sim_runlist_1/Q2_vs_BeamPos_Corrected/%s.txt",
   filename.c_str()));
   */
  /*
   TGraphErrors * gr1 =
   new TGraphErrors(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass7_Analysis/runlist_102/Q2_vs_BeamPos/%s.txt",
   filename.c_str()));
   */
  TGraphErrors * gr1 =
      new TGraphErrors(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass7_Analysis/runlist_102/Q2_vs_BeamPos_Corrected/%s.txt",
              filename.c_str()));

  //set it so the errors print
  gStyle->SetOptFit(1110);
  //set markercolor - Blue
  gr1->SetMarkerColor(4);
  //set Marker size
  gr1->SetMarkerSize(1);
  //set marker style - filled circle
  gr1->SetMarkerStyle(8);
  //set error line color
  gr1->SetLineColor(kBlue);
  //firt this graph with a linear fit
  gr1->Fit("pol1", "EX0");
  gr1->GetFunction("pol1")->SetLineColor(kBlue);
  //add this to the mulitgraph
  mg->Add(gr1);

  //add a file to the TGraphErrors - which is at graph formed from data that has error bars
  //for the second graph
  /*
   TGraphErrors * gr2 =
   new TGraphErrors(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Sim_Pass2_Analysis/sim_runlist_1/Q2_vs_BeamPos/%s.txt",
   filename2.c_str()));
   */
  /*
   TGraphErrors * gr2 =
   new TGraphErrors(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Sim_Pass2_Analysis/sim_runlist_1/Q2_vs_BeamPos_Corrected/%s.txt",
   filename2.c_str()));
   */

  /*
   TGraphErrors * gr2 =
   new TGraphErrors(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass7_Analysis/runlist_102/Q2_vs_BeamPos/%s.txt",
   filename2.c_str()));
   */
  TGraphErrors * gr2 =
      new TGraphErrors(
          Form(
              "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass7_Analysis/runlist_102/Q2_vs_BeamPos_Corrected/%s.txt",
              filename2.c_str()));

  //set it so the errors print
  gStyle->SetOptFit(1110);
  //set markercolor - Red
  gr2->SetMarkerColor(kRed);
  //set Marker size
  gr2->SetMarkerSize(1);
  //set marker style - filled circle
  gr2->SetMarkerStyle(22);
  //ft this graph with a linear fit
  gr2->Fit("pol1", "EX0");
  gr2->GetFunction("pol1")->SetLineColor(kRed);
  //set line error color
  gr2->SetLineColor(kRed);
  //add this to the mulitgraph
  mg->Add(gr2);

  mg->SetTitle(
      Form(
          "Octants %d and %d - Q^{2} vs %s Beam %s; %s Beam %s [%s] ; Q^{2} [m(GeV^{2})]",
          oct, oct2, dir.c_str(), what.c_str(), dir.c_str(), what.c_str(),
          units.c_str()));
  /*
   mg->SetTitle(
   Form(
   "Octants %d and %d - Q^{2} vs %s beam %s; %s beam %s [#murad] ; Q^{2} [m(GeV^{2})]",
   oct, oct2, dir.c_str(), what.c_str(), dir.c_str(), what.c_str(),
   units.c_str()));
   */

  //Draw this wonderfull data - A=Axis are drawn around the graph - P=Axis are drawn around the graph
  mg->Draw("AP");

  //move the axis label so can read everything
  mg->GetYaxis()->SetTitleOffset(1.26);

  c1.Update();

  //do something with the stats so that they are colored like the lines
  //I have no idea what this does I found it online...
  TPaveStats *stats1 = (TPaveStats*) gr1->GetListOfFunctions()->FindObject(
      "stats");
  TPaveStats *stats2 = (TPaveStats*) gr2->GetListOfFunctions()->FindObject(
      "stats");
  stats1->SetTextColor(kBlue);
  stats2->SetTextColor(kRed);

  stats1->SetX1NDC(0.32);
  stats1->SetX2NDC(0.52);
  stats1->SetY1NDC(0.78);

  stats2->SetX1NDC(0.56);
  stats2->SetX2NDC(0.76);
  stats2->SetY1NDC(0.78);

  //set Title, and axis
  //mg->SetTitle(Form("Octants %d and %d - Q2 vs %s beam %s; %s beam %s (%s) ; Q2 (m(GeV)^2)", oct, oct2, dir.c_str(), what.c_str(), dir.c_str(),what.c_str(), units.c_str()));
  //mg->GetXaxis()->SetTitle(Form("%s beam %s (%s)",dir.c_str(),what.c_str(), units.c_str() ));
  //mg->GetYaxis()->SetTitle("Q2 (m(GeV)^2)");

  TLegend * leg = new TLegend(0.8, 0.45, 0.95, 0.55);
  //  TLegend * leg = new TLegend(0.8, 0.60, 0.95, 0.70);
  //  TLegend * leg = new TLegend(0.8, 0.40, 0.95, 0.50);
  leg->SetHeader("Octant table");
  leg->AddEntry(gr1, Form("Octant %d", oct), "p");
  leg->AddEntry(gr2, Form("Octant %d", oct2), "p");
  leg->Draw();

  //c1.Update();
  gPad->Update();
  /*
   c1.SaveAs(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Sim_Pass2_Analysis/sim_runlist_1/Q2_vs_BeamPos/Octant_%d_and_%d_Q2_vs_%s_%s.png",
   oct, oct2, dir.c_str(), what.c_str()));
   */

  /*
   c1.SaveAs(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Sim_Pass2_Analysis/sim_runlist_1/Q2_vs_BeamPos_Corrected/Octant_%d_and_%d_Q2_vs_%s_%s.png",
   oct, oct2, dir.c_str(), what.c_str()));
   */

  /*
   c1.SaveAs(
   Form(
   "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass7_Analysis/runlist_102/Q2_vs_BeamPos/Octant_%d_and_%d_Q2_vs_%s_%s.png",
   oct, oct2, dir.c_str(), what.c_str()));
   */
  c1.SaveAs(
      Form(
          "~/qweak/QwAnalysis_trunk/Extensions/ValerianROOT/Pass7_Analysis/runlist_102/Q2_vs_BeamPos_Corrected/Octant_%d_and_%d_Q2_vs_%s_%s.png",
          oct, oct2, dir.c_str(), what.c_str()));

  return;
}
