/***********************************************************
Programmer: Valerie Gray
Purpose:

This is the main function that puts all the tracking files
together making one ROOT file with all the pass n tracking data.


Entry Conditions:
Date: 05-08-2014
Modified: 05-19-2014
Assisted By: Juan Carlos Cornejo
*********************************************************/

// Load ROOT includes
#include <TROOT.h> // Contains most of the ROOT data types
#include <TTree.h>
#include <TFile.h>

// Load STD includes
#include <iostream>
#include <vector>

//Load the Valerian ROOT includes
#include "Run.h"


int main(int argc, char** argv)
{
  Int_t pass = 5;
  TString path = Form("home/vmgray/QweakTracking/data/pass%d/",pass);
//  TString path = Form("home/vmgray/QweakTracking/data/pass%d/",pass);

  // Get a list of runs... this puts them in the
//  std::vector<MyRun_t> runs = GetRun(path+Form("List_of_Run_pass%d.txt",pass));
  std::vector<MyRun_t> runs = GetRun("/home/vmgray/QweakTracking/data/pass5b/List_of_Run_pass5b.txt");

  // Loop through the runlist and parse necessary files
  for( size_t i = 0; i < runs.size(); i++ )
  {
    //debugging
//    std::cout << "Processing Run: " << runs[i].run << std::endl;

    // Process Angle
    runs[i].angle = GetAngle(runs[i].run, pass, path);

    // Process BeamPositionX
    runs[i].beamposx = GetBeamPositionX(runs[i].run, pass, path);

    // Process BeamPositionY
    runs[i].beamposy = GetBeamPositionY(runs[i].run, pass, path);

    // Process Chi
    runs[i].chi = GetChi(runs[i].run, pass, path);

    // Process GoodTrack
    runs[i].goodtrack = GetGoodTrack(runs[i].run, pass, path);

    // Process LogChi
    runs[i].logchi = GetLogChi(runs[i].run, pass, path);

    // Process Trigger
    runs[i].trigger = GetTrigger(runs[i].run, pass, path);

    // Process PartialTrackChi2
    runs[i].ptchi = GetPartialTrackChi2(runs[i].run, pass, path);

    // Process PartialTrackSlope
    runs[i].ptslope = GetPartialTrackSlope(runs[i].run, pass, path);

    // Process PosOnBar
    runs[i].posonbar = GetPosOnBar(runs[i].run, pass, path);

    // Process Q2PhiMatch
    runs[i].q2phimatch = GetQ2PhiMatch(runs[i].run, pass, path);

    // Process Q2PhiMatchProj
    runs[i].q2phimatchproj = GetQ2PhiMatchProj(runs[i].run, pass, path);

    // Process Q2ThetaMatch
    runs[i].q2thetamatch = GetQ2ThetaMatch(runs[i].run, pass, path);

    // Process Q2ThetaMatchProj
    runs[i].q2thetamatchproj = GetQ2ThetaMatchProj(runs[i].run, pass, path);

    // Process Q2Cut
    runs[i].q2cut = GetQ2Cut(runs[i].run, pass, path);

    // Process Q2Loss
    runs[i].q2loss = GetQ2Loss(runs[i].run, pass, path);

    // Process Q2NoLoss
    runs[i].q2noloss = GetQ2NoLoss(runs[i].run, pass, path);

    // Process Qtor
    runs[i].qtor = GetQtor(runs[i].run, pass, path);

    // Process R2Pkg1
    runs[i].r2pkg1 = GetR2Pkg1(runs[i].run, pass, path);

    // Process R2Pkg2
    runs[i].r2pkg2 = GetR2Pkg2(runs[i].run, pass, path);

    // Process PhiOffset
    runs[i].phioffset = GetPhiOffset(runs[i].run, pass, path);

    // Process R3Pkg1
    runs[i].r3pkg1 = GetR3Pkg1(runs[i].run, pass, path);

    // Process R3Pkg2
    runs[i].r3pkg2 = GetR3Pkg2(runs[i].run, pass, path);

    // Process ThetaOffset
    runs[i].thetaoffset = GetThetaOffset(runs[i].run, pass, path);

    // Process RawTrack
    runs[i].rawtrack = GetRawTrack(runs[i].run, pass, path);

    // Process Residual
    runs[i].residual = GetResidual(runs[i].run, pass, path);

    // Process ScatEnergy
    runs[i].scatenergy = GetScatEnergy(runs[i].run, pass, path);

    // Process TrackChi2
    runs[i].trackchi = GetTrackChi2(runs[i].run, pass, path);

    // Process TrackSlope
    runs[i].trackslope = GetTrackSlope(runs[i].run, pass, path);

    // Process TreelineChi2
    runs[i].treechi = GetTreelineChi2(runs[i].run, pass, path);

    // Process TreelineSlope
    runs[i].treeslope = GetTreelineSlope(runs[i].run, pass, path);

    // Continue this line of reasoning :)
  }


 // At this point, we have read all the data into memory, and now we can
  // begin to build and write out our tree

  // Create pointers to all the data information that you want to store in the
  // tree and of the appropriate type
  Int_t run;

  // Note, vectors/structures have to be passed by reference/pointer.
  // This is because ROOT does not make copies of these, and so we have
  // to make sure that ROOT always points to an already existing copy

  std::vector<MyAngle_t>* angle =
    new std::vector<MyAngle_t>();

  std::vector<MyBeamPositionX_t>* beamposx =
    new std::vector<MyBeamPositionX_t>();

  std::vector<MyBeamPositionY_t>* beamposy =
    new std::vector<MyBeamPositionY_t>();

  std::vector<MyChi_t>* chi =
    new std::vector<MyChi_t>();

  std::vector<MyGoodTrack_t>* goodtrack =
    new std::vector<MyGoodTrack_t>();

  std::vector<MyLogChi_t>* logchi =
    new std::vector<MyLogChi_t>();

  std::vector<MyTrigger_t>* trigger =
    new std::vector<MyTrigger_t>();

  std::vector<MyPartialTrackChi2_t>* ptchi =
    new std::vector<MyPartialTrackChi2_t>();

  std::vector<MyPartialTrackSlope_t>* ptslope =
    new std::vector<MyPartialTrackSlope_t>();

  std::vector<MyPosOnBar_t>* posonbar =
    new std::vector<MyPosOnBar_t>();

  std::vector<MyQ2PhiMatch_t>* q2phimatch =
    new std::vector<MyQ2PhiMatch_t>();

  std::vector<MyQ2PhiMatchProj_t>* q2phimatchproj =
    new std::vector<MyQ2PhiMatchProj_t>();

  std::vector<MyQ2ThetaMatch_t>* q2thetamatch =
    new std::vector<MyQ2ThetaMatch_t>();

  std::vector<MyQ2ThetaMatchProj_t>* q2thetamatchproj =
    new std::vector<MyQ2ThetaMatchProj_t>();

  std::vector<MyQ2Cut_t>* q2cut =
    new std::vector<MyQ2Cut_t>();

  std::vector<MyQ2Loss_t>* q2loss =
    new std::vector<MyQ2Loss_t>();

  std::vector<MyQ2NoLoss_t>* q2noloss =
    new std::vector<MyQ2NoLoss_t>();

  std::vector<MyQtor_t>* qtor =
    new std::vector<MyQtor_t>();

  std::vector<MyR2Pkg1_t>* r2pkg1 =
    new std::vector<MyR2Pkg1_t>();

  std::vector<MyR2Pkg2_t>* r2pkg2 =
    new std::vector<MyR2Pkg2_t>();

  std::vector<MyPhiOffset_t>* phioffset =
    new std::vector<MyPhiOffset_t>();

  std::vector<MyR3Pkg1_t>* r3pkg1 =
    new std::vector<MyR3Pkg1_t>();

  std::vector<MyR3Pkg2_t>* r3pkg2 =
    new std::vector<MyR3Pkg2_t>();

  std::vector<MyThetaOffset_t>* thetaoffset =
    new std::vector<MyThetaOffset_t>();

  std::vector<MyRawTrack_t>* rawtrack =
    new std::vector<MyRawTrack_t>();

  std::vector<MyResidual_t>* residual =
    new std::vector<MyResidual_t>();

  std::vector<MyScatEnergy_t>* scatenergy =
    new std::vector<MyScatEnergy_t>();

  std::vector<MyTrackChi2_t>* trackchi =
    new std::vector<MyTrackChi2_t>();

  std::vector<MyTrackSlope_t>* trackslope =
    new std::vector<MyTrackSlope_t>();

  std::vector<MyTreelineChi2_t>* treechi =
    new std::vector<MyTreelineChi2_t>();

  std::vector<MyTreelineSlope_t>* treeslope =
    new std::vector<MyTreelineSlope_t>();

  // Now create a ROOT file (recreate==erase and start a new file)
//Change name and comments, etc.
  TFile file("Pass5b_TrackingRuns.root","RECREATE","A comment goes here :)");
//  TFile file(Form("Pass%d_TrackingRuns.root", pass),"RECREATE","A comment goes here :)");
//  TTree *tree = new TTree("TrackThat",Form("Tracking Pass %d Data", pass) );
  TTree *tree = new TTree("TrackThat","Tracking Pass 5b Data");

  // Add simple branches (non-structured) - this uses branch
  tree->Branch("Run",& run);

  // Add "complex" (structured) branches (using the Bronch method)
  // Note: No, it is not mispelled :)
  tree->Bronch("Angle","std::vector<MyAngle_t>",& angle);

  tree->Bronch("BeamPositionX","std::vector<MyBeamPositionX_t>",& beamposx);

  tree->Bronch("BeamPositionY","std::vector<MyBeamPositionY_t>",& beamposy);

  tree->Bronch("Chi","std::vector<MyChi_t>",& chi);

  tree->Bronch("GoodTrack","std::vector<MyGoodTrack_t>",& goodtrack);

  tree->Bronch("LogChi","std::vector<MyLogChi_t>",& logchi);

  tree->Bronch("Trigger","std::vector<MyTrigger_t>",& trigger);

  tree->Bronch("PartialTrackChi2","std::vector<MyPartialTrackChi2_t>",& ptchi);

  tree->Bronch("PartialTrackSlope","std::vector<MyPartialTrackSlope_t>",& ptslope);

  tree->Bronch("PosOnBar","std::vector<MyPosOnBar_t>",& posonbar);

  tree->Bronch("Q2PhiMatch","std::vector<MyQ2PhiMatch_t>",& q2phimatch);

  tree->Bronch("Q2PhiMatchProj","std::vector<MyQ2PhiMatchProj_t>",& q2phimatchproj);

  tree->Bronch("Q2ThetaMatch","std::vector<MyQ2ThetaMatch_t>",& q2thetamatch);

  tree->Bronch("Q2ThetaMatchProj","std::vector<MyQ2ThetaMatchProj_t>",& q2thetamatchproj);

  tree->Bronch("Q2Cut","std::vector<MyQ2Cut_t>",& q2cut);

  tree->Bronch("Q2Loss","std::vector<MyQ2Loss_t>",& q2loss);

  tree->Bronch("Q2NoLoss","std::vector<MyQ2NoLoss_t>",& q2noloss);

  tree->Bronch("Qtor","std::vector<MyQtor_t>",& qtor);

  tree->Bronch("R2Pkg1","std::vector<MyR2Pkg1_t>",& r2pkg1);

  tree->Bronch("R2Pkg2","std::vector<MyR2Pkg2_t>",& r2pkg2);

  tree->Bronch("PhiOffset","std::vector<MyPhiOffset_t>",& phioffset);

  tree->Bronch("R3Pkg1","std::vector<MyR3Pkg1_t>",& r3pkg1);

  tree->Bronch("R3Pkg2","std::vector<MyR3Pkg2_t>",& r3pkg2);

  tree->Bronch("ThetaOffset","std::vector<MyThetaOffset_t>",& thetaoffset);

  tree->Bronch("RawTrack","std::vector<MyRawTrack_t>",& rawtrack);

  tree->Bronch("Residual","std::vector<MyResidual_t>",& residual);

  tree->Bronch("ScatEnergy","std::vector<MyScatEnergy_t>",& scatenergy);

  tree->Bronch("TrackChi2","std::vector<MyTrackChi2_t>",& trackchi);

  tree->Bronch("TrackSlope","std::vector<MyTrackSlope_t>",& trackslope);

  tree->Bronch("TreelineChi2","std::vector<MyTreelineChi2_t>",& treechi);

  tree->Bronch("TreelineSlope","std::vector<MyTreelineSlope_t>",& treeslope);

  // Add your other simple or complex branches by following the above
  // proceedure

  // Finally, we are ready to parse through the runlist and fill in the tree
  for(size_t i = 0; i < runs.size(); i++ )
  {
    run = runs[i].run;
    angle = & runs[i].angle;
    beamposx = & runs[i].beamposx;
    beamposy = & runs[i].beamposy;
    chi = & runs[i].chi;
    goodtrack = & runs[i].goodtrack;
    logchi = & runs[i].logchi;
    trigger = & runs[i].trigger;
    ptchi = & runs[i].ptchi;
    ptslope = & runs[i].ptslope;
    posonbar = & runs[i].posonbar;
    q2phimatch = & runs[i].q2phimatch;
    q2phimatchproj = & runs[i].q2phimatchproj;
    q2thetamatch = & runs[i].q2thetamatch;
    q2thetamatchproj = & runs[i].q2thetamatchproj;
    q2cut = & runs[i].q2cut;
    q2loss = & runs[i].q2loss;
    q2noloss = & runs[i].q2noloss;
    qtor = & runs[i].qtor;
    r2pkg1 = & runs[i].r2pkg1;
    r2pkg2 = & runs[i].r2pkg2;
    phioffset = & runs[i].phioffset;
    r3pkg1 = & runs[i].r3pkg1;
    r3pkg2 = & runs[i].r3pkg2;
    thetaoffset = & runs[i].thetaoffset;
    rawtrack = & runs[i].rawtrack;
    residual = & runs[i].residual;
    scatenergy = & runs[i].scatenergy;
    trackchi = & runs[i].trackchi;
    trackslope = & runs[i].trackslope;
    treechi = & runs[i].treechi;
    treeslope = & runs[i].treeslope;

    if( i % 25 == 0) std::cout << i << " Bottles of ROOT beer on the wall" << std::endl;
    tree->Fill();
  }

  // Finally, make sure ROOT writes the file to disk, and flush its cache
  file.Write();
  file.Flush();


  // Delete allocated memory
/*
  delete angle;
  delete beamposx;
  delete beamposy;
  delete chi;
  delete goodtrack;
  delete logchi;
  delete trigger;
  delete ptchi;
  delete ptslope;
  delete posonbar;
  delete q2phimatch;
  delete q2phimatchproj;
  delete q2thetamatch;
  delete q2thetamatchproj;
  delete q2cut;
  delete q2loss;
  delete q2noloss;
  delete qtor;
  delete r2pkg1;
  delete r2pkg2;
  delete phioffset;
  delete r3pkg1;
  delete r3pkg2;
  delete thetaoffset;
  delete rawtrack;
  delete residual;
  delete scatenergy;
  delete trackchi;
  delete trackslope;
  delete treechi;
  delete treeslope;
*/

  //cleans the runs - whips out the pointers in runs vector
  runs.clear();

  // We are done! Celebrate! :)
  return 0;
}
