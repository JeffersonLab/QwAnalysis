/***********************************************************
 Programmer: Valerie Gray
 Purpose:

 This is the main function that puts all the tracking files
 together making one ROOT file with all the pass n tracking data.


 Entry Conditions:
 Date: 05-08-2014
 Modified: 05-25-2014
 Assisted By: Juan Carlos Cornejo
 *********************************************************/

// Load ROOT includes
#include <TROOT.h> // Contains most of the ROOT data types
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>

// Load STD includes
#include <iostream>
#include <vector>
#include <cstdlib>

//Load the Valerian ROOT includes
#include "Run.h"

int main(int argc, char** argv)
{

  // Get a list of runs... this puts them in the
  std::vector<MyRun_t> runs = GetRun(
      TString(gSystem->Getenv("VALERIAN")) + "/data/pass"
      + TString(gSystem->Getenv("PASS")) + "/List_of_Run_pass"
      + TString(gSystem->Getenv("PASS")) + ".txt");

  // Loop through the runlist and parse necessary files
  for (size_t i = 0; i < runs.size(); i++)
  {
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
      std::cout << "Processing Run: " << runs[i].run << std::endl;

    // Process Angle
    runs[i].angle = GetAngle(runs[i].run);

    // Process BeamPositionX
    runs[i].beamposx = GetBeamPositionX(runs[i].run);

    // Process BeamPositionY
    runs[i].beamposy = GetBeamPositionY(runs[i].run);

    // Process Chi
    runs[i].chi = GetChi(runs[i].run);

    // Process GoodTrack
    runs[i].goodtrack = GetGoodTrack(runs[i].run);

    // Process LogChi
    runs[i].logchi = GetLogChi(runs[i].run);

    // Process Trigger
    runs[i].trigger = GetTrigger(runs[i].run);

    // Process PartialTrackChi2
    runs[i].percgood = GetPercentGood(runs[i].run);

    // Process PartialTrackChi2
    runs[i].ptchi = GetPartialTrackChi2(runs[i].run);

    // Process PartialTrackSlope
    runs[i].ptslope = GetPartialTrackSlope(runs[i].run);

    // Process PosOnBar
    runs[i].posonbar = GetPosOnBar(runs[i].run);

    // Process Q2PhiMatch
    runs[i].q2phimatch = GetQ2PhiMatch(runs[i].run);

    // Process Q2PhiMatchProj
    runs[i].q2phimatchproj = GetQ2PhiMatchProj(runs[i].run);

    // Process Q2ThetaMatch
    runs[i].q2thetamatch = GetQ2ThetaMatch(runs[i].run);

    // Process Q2ThetaMatchProj
    runs[i].q2thetamatchproj = GetQ2ThetaMatchProj(runs[i].run);

    // Process Q2Cut
    runs[i].q2cut = GetQ2Cut(runs[i].run);

    // Process Q2Loss
    runs[i].q2loss = GetQ2Loss(runs[i].run);

    // Process Q2NoLoss
    runs[i].q2noloss = GetQ2NoLoss(runs[i].run);

    // Process Qtor
    runs[i].qtor = GetQtor(runs[i].run);

    // Process R2Pkg1
    runs[i].r2pkg1 = GetR2Pkg1(runs[i].run);

    // Process R2Pkg2
    runs[i].r2pkg2 = GetR2Pkg2(runs[i].run);

    // Process PhiOffset
    runs[i].phioffset = GetPhiOffset(runs[i].run);

    // Process R3Pkg1
    runs[i].r3pkg1 = GetR3Pkg1(runs[i].run);

    // Process R3Pkg2
    runs[i].r3pkg2 = GetR3Pkg2(runs[i].run);

    // Process ThetaOffset
    runs[i].thetaoffset = GetThetaOffset(runs[i].run);

    // Process RawTrack
    runs[i].rawtrack = GetRawTrack(runs[i].run);

    // Process Residual
    runs[i].residual = GetResidual(runs[i].run);

    // Process ScatEnergy
    runs[i].scatenergy = GetScatEnergy(runs[i].run);

    // Process TrackChi2
    runs[i].trackchi = GetTrackChi2(runs[i].run);

    // Process TrackSlope
    runs[i].trackslope = GetTrackSlope(runs[i].run);

    // Process TreelineChi2
    runs[i].treechi = GetTreelineChi2(runs[i].run);

    // Process TreelineSlope
    runs[i].treeslope = GetTreelineSlope(runs[i].run);

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

  std::vector<MyAngle_t>* angle = new std::vector<MyAngle_t>();

  std::vector<MyBeamPositionX_t>* beamposx =
      new std::vector<MyBeamPositionX_t>();

  std::vector<MyBeamPositionY_t>* beamposy =
      new std::vector<MyBeamPositionY_t>();

  std::vector<MyChi_t>* chi = new std::vector<MyChi_t>();

  std::vector<MyGoodTrack_t>* goodtrack = new std::vector<MyGoodTrack_t>();

  std::vector<MyLogChi_t>* logchi = new std::vector<MyLogChi_t>();

  std::vector<MyTrigger_t>* trigger = new std::vector<MyTrigger_t>();

  std::vector<MyPartialTrackChi2_t>* ptchi = new std::vector<
      MyPartialTrackChi2_t>();

  std::vector<MyPartialTrackSlope_t>* ptslope = new std::vector<
      MyPartialTrackSlope_t>();

  std::vector<MyPercentGood_t>* percgood = new std::vector<MyPercentGood_t>();

  std::vector<MyPosOnBar_t>* posonbar = new std::vector<MyPosOnBar_t>();

  std::vector<MyQ2PhiMatch_t>* q2phimatch = new std::vector<MyQ2PhiMatch_t>();

  std::vector<MyQ2PhiMatchProj_t>* q2phimatchproj = new std::vector<
      MyQ2PhiMatchProj_t>();

  std::vector<MyQ2ThetaMatch_t>* q2thetamatch =
      new std::vector<MyQ2ThetaMatch_t>();

  std::vector<MyQ2ThetaMatchProj_t>* q2thetamatchproj = new std::vector<
      MyQ2ThetaMatchProj_t>();

  std::vector<MyQ2Cut_t>* q2cut = new std::vector<MyQ2Cut_t>();

  std::vector<MyQ2Loss_t>* q2loss = new std::vector<MyQ2Loss_t>();

  std::vector<MyQ2NoLoss_t>* q2noloss = new std::vector<MyQ2NoLoss_t>();

  std::vector<MyQtor_t>* qtor = new std::vector<MyQtor_t>();

  std::vector<MyR2Pkg1_t>* r2pkg1 = new std::vector<MyR2Pkg1_t>();

  std::vector<MyR2Pkg2_t>* r2pkg2 = new std::vector<MyR2Pkg2_t>();

  std::vector<MyPhiOffset_t>* phioffset = new std::vector<MyPhiOffset_t>();

  std::vector<MyR3Pkg1_t>* r3pkg1 = new std::vector<MyR3Pkg1_t>();

  std::vector<MyR3Pkg2_t>* r3pkg2 = new std::vector<MyR3Pkg2_t>();

  std::vector<MyThetaOffset_t>* thetaoffset =
      new std::vector<MyThetaOffset_t>();

  std::vector<MyRawTrack_t>* rawtrack = new std::vector<MyRawTrack_t>();

  std::vector<MyResidual_t>* residual = new std::vector<MyResidual_t>();

  std::vector<MyScatEnergy_t>* scatenergy = new std::vector<MyScatEnergy_t>();

  std::vector<MyTrackChi2_t>* trackchi = new std::vector<MyTrackChi2_t>();

  std::vector<MyTrackSlope_t>* trackslope = new std::vector<MyTrackSlope_t>();

  std::vector<MyTreelineChi2_t>* treechi = new std::vector<MyTreelineChi2_t>();

  std::vector<MyTreelineSlope_t>* treeslope =
      new std::vector<MyTreelineSlope_t>();


  // Now create a ROOT file (recreate==erase and start a new file)
  TFile file(Form("Pass" + TString(gSystem->Getenv("PASS"))+ "_TrackingRuns.root"), "RECREATE",
      "Now creating a relaxing ROOT experience :)");

  TTree *tree = new TTree("TrackThat", "Tracking Pass" + TString(gSystem->Getenv("PASS")) + " Data");

  // Add simple branches (non-structured) - this uses branch
  tree->Branch("Run", &run);

  // Add "complex" (structured) branches (using the Bronch method)
  // Note: No, it is not mispelled :)
  tree->Bronch("Angle", "std::vector<MyAngle_t>", &angle);

  tree->Bronch("BeamPositionX", "std::vector<MyBeamPositionX_t>", &beamposx);

  tree->Bronch("BeamPositionY", "std::vector<MyBeamPositionY_t>", &beamposy);

  tree->Bronch("Chi", "std::vector<MyChi_t>", &chi);

  tree->Bronch("GoodTrack", "std::vector<MyGoodTrack_t>", &goodtrack);

  tree->Bronch("LogChi", "std::vector<MyLogChi_t>", &logchi);

  tree->Bronch("Trigger", "std::vector<MyTrigger_t>", &trigger);

  tree->Bronch("PartialTrackChi2", "std::vector<MyPartialTrackChi2_t>", &ptchi);

  tree->Bronch("PartialTrackSlope", "std::vector<MyPartialTrackSlope_t>",
      &ptslope);

  tree->Bronch("PercentGood", "std::vector<MyPercentGood_t>", &percgood);

  tree->Bronch("PosOnBar", "std::vector<MyPosOnBar_t>", &posonbar);

  tree->Bronch("Q2PhiMatch", "std::vector<MyQ2PhiMatch_t>", &q2phimatch);

  tree->Bronch("Q2PhiMatchProj", "std::vector<MyQ2PhiMatchProj_t>",
      &q2phimatchproj);

  tree->Bronch("Q2ThetaMatch", "std::vector<MyQ2ThetaMatch_t>", &q2thetamatch);

  tree->Bronch("Q2ThetaMatchProj", "std::vector<MyQ2ThetaMatchProj_t>",
      &q2thetamatchproj);

  tree->Bronch("Q2Cut", "std::vector<MyQ2Cut_t>", &q2cut);

  tree->Bronch("Q2Loss", "std::vector<MyQ2Loss_t>", &q2loss);

  tree->Bronch("Q2NoLoss", "std::vector<MyQ2NoLoss_t>", &q2noloss);

  tree->Bronch("Qtor", "std::vector<MyQtor_t>", &qtor);

  tree->Bronch("R2Pkg1", "std::vector<MyR2Pkg1_t>", &r2pkg1);

  tree->Bronch("R2Pkg2", "std::vector<MyR2Pkg2_t>", &r2pkg2);

  tree->Bronch("PhiOffset", "std::vector<MyPhiOffset_t>", &phioffset);

  tree->Bronch("R3Pkg1", "std::vector<MyR3Pkg1_t>", &r3pkg1);

  tree->Bronch("R3Pkg2", "std::vector<MyR3Pkg2_t>", &r3pkg2);

  tree->Bronch("ThetaOffset", "std::vector<MyThetaOffset_t>", &thetaoffset);

  tree->Bronch("RawTrack", "std::vector<MyRawTrack_t>", &rawtrack);

  tree->Bronch("Residual", "std::vector<MyResidual_t>", &residual);

  tree->Bronch("ScatEnergy", "std::vector<MyScatEnergy_t>", &scatenergy);

  tree->Bronch("TrackChi2", "std::vector<MyTrackChi2_t>", &trackchi);

  tree->Bronch("TrackSlope", "std::vector<MyTrackSlope_t>", &trackslope);

  tree->Bronch("TreelineChi2", "std::vector<MyTreelineChi2_t>", &treechi);

  tree->Bronch("TreelineSlope", "std::vector<MyTreelineSlope_t>", &treeslope);

  // Add your other simple or complex branches by following the above
  // Procedure

  // Finally, we are ready to parse through the runlist and fill in the tree
  for (size_t i = 0; i < runs.size(); i++)
  {
    run = runs[i].run;
    angle = &runs[i].angle;
    beamposx = &runs[i].beamposx;
    beamposy = &runs[i].beamposy;
    chi = &runs[i].chi;
    goodtrack = &runs[i].goodtrack;
    logchi = &runs[i].logchi;
    trigger = &runs[i].trigger;
    ptchi = &runs[i].ptchi;
    ptslope = &runs[i].ptslope;
    percgood = &runs[i].percgood;
    posonbar = &runs[i].posonbar;
    q2phimatch = &runs[i].q2phimatch;
    q2phimatchproj = &runs[i].q2phimatchproj;
    q2thetamatch = &runs[i].q2thetamatch;
    q2thetamatchproj = &runs[i].q2thetamatchproj;
    q2cut = &runs[i].q2cut;
    q2loss = &runs[i].q2loss;
    q2noloss = &runs[i].q2noloss;
    qtor = &runs[i].qtor;
    r2pkg1 = &runs[i].r2pkg1;
    r2pkg2 = &runs[i].r2pkg2;
    phioffset = &runs[i].phioffset;
    r3pkg1 = &runs[i].r3pkg1;
    r3pkg2 = &runs[i].r3pkg2;
    thetaoffset = &runs[i].thetaoffset;
    rawtrack = &runs[i].rawtrack;
    residual = &runs[i].residual;
    scatenergy = &runs[i].scatenergy;
    trackchi = &runs[i].trackchi;
    trackslope = &runs[i].trackslope;
    treechi = &runs[i].treechi;
    treeslope = &runs[i].treeslope;

    if (i % 25 == 0)
      std::cout << i << " Bottles of ROOT beer on the wall" << std::endl;
    tree->Fill();
  }

  // Finally, make sure ROOT writes the file to disk, and flush its cache
  file.Write();
  file.Flush();

  //cleans the runs - whips out the pointers in runs vector
  //don't use delete as on is not passing values we need clear
  runs.clear();

  // We are done! Celebrate! :)
  return 0;
}
