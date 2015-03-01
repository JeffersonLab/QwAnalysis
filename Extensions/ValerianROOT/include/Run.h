/********************************************
 \author <b>Programmer:<\b> Valerie Gray
 \author <b>Assisted By:<\b>

\brief <b>Purpose:</b> This is header file fot
  the Run values

  Everything is a subdirectory to the run

\date <b>Date:</b> 05-08-2014
\date <b>Modified:</b> 05-15-2014

\note <b>Entry Conditions:</b> none

********************************************/

#ifndef RUN_HH_
#define RUN_HH_

//ROOT includes
#include <TROOT.h>

//standard includes
#include <vector>

//Valerian ROOT include
#include "Angle.h"
#include "BeamPositionX.h"
#include "BeamPositionY.h"
#include "Chi.h"
#include "GoodTrack.h"
#include "LogChi.h"
#include "NTrigger.h"
#include "PartialTrackChi2.h"
#include "PartialTrackSlope.h"
#include "PercentGood.h"
#include "PosOnBar.h"
#include "Q2PhiMatch.h"
#include "Q2PhiMatchProj.h"
#include "Q2ThetaMatch.h"
#include "Q2ThetaMatchProj.h"
#include "Q2WithCut.h"
#include "Q2WithLoss.h"
#include "Q2WithOutLoss.h"
#include "QTOR.h"
#include "R2Pkg1.h"
#include "R2Pkg2.h"
#include "R3PhiOffset.h"
#include "R3Pkg1.h"
#include "R3Pkg2.h"
#include "R3ThetaOffset.h"
#include "RawTrack.h"
#include "Residual.h"
#include "ScatEnergy.h"
#include "TrackChi2.h"
#include "TrackSlope.h"
#include "TreelineChi2.h"
#include "TreelineSlope.h"

// Run Structure
struct MyRun_t
{
  Int_t run;  //run number

  //all the structures from the above includes
  //these could be separate and are in the tree, this
  //is for book keeping, and only use internally,
  //this way each structure doesn't have to know about
  //and store the run number - making it better all around

  std::vector<MyAngle_t> angle;  //vector of the scat angles for a run
  std::vector<MyBeamPositionX_t> beamposx;  //vector of the beam pos X values
  std::vector<MyBeamPositionY_t> beamposy;  //vector of the beam pos Y values
  std::vector<MyChi_t> chi;  //vector of the Chi values for a run
  std::vector<MyGoodTrack_t>  goodtrack;  //vector of the % good tracks
  std::vector<MyLogChi_t> logchi;  //vector of the Log Chi values
  std::vector<MyTrigger_t> trigger;  //vector of the number of triggers
  std::vector<MyPartialTrackChi2_t> ptchi;  //vector of the partial tracks chi2
  std::vector<MyPartialTrackSlope_t> ptslope;  //vector of the partial tracks slope
  std::vector<MyPercentGood_t> percgood;  //vector for the q2 percent good tracks
  std::vector<MyPosOnBar_t> posonbar;  //vector for the position on bar data
  std::vector<MyQ2PhiMatch_t> q2phimatch;  //vector for the q2 phi match
  std::vector<MyQ2PhiMatchProj_t> q2phimatchproj;  //vector for q2 phi projection match
  std::vector<MyQ2ThetaMatch_t> q2thetamatch;  //vector for the q2 theta match
  std::vector<MyQ2ThetaMatchProj_t> q2thetamatchproj;  //vector for q2 theta projection match
  std::vector<MyQ2Cut_t> q2cut;  //vector of the q2 cut values
  std::vector<MyQ2Loss_t> q2loss;  //vector for the q2 with loss
  std::vector<MyQ2NoLoss_t> q2noloss;  //vector for the q2 without loss
  std::vector<MyQtor_t> qtor;  //vector of qtor currents
  std::vector<MyR2Pkg1_t> r2pkg1;  //vector of r2 package 1 octant number
  std::vector<MyR2Pkg2_t> r2pkg2;  //vector of r2 package 2 octant number
  std::vector<MyPhiOffset_t> phioffset;  //vector of the phi offset
  std::vector<MyR3Pkg1_t> r3pkg1;  //vector of r3 package 1 octant number
  std::vector<MyR3Pkg2_t> r3pkg2;  //vector of r3 package 2 octant number
  std::vector<MyThetaOffset_t> thetaoffset;  //vector of the phi offset
  std::vector<MyRawTrack_t> rawtrack;  //vector of the raw track value
  std::vector<MyResidual_t> residual;  //vector of the residual
  std::vector<MyScatEnergy_t> scatenergy;  //vectro of the scattering angle
  std::vector<MyTrackChi2_t> trackchi;  //vector of the tracks Chi
  std::vector<MyTrackSlope_t> trackslope;  //vector of the tracks slope
  std::vector<MyTreelineChi2_t> treechi;  //vector of the treeline chi
  std::vector<MyTreelineSlope_t> treeslope;  //vector of the treeline slope
};

std::vector<MyRun_t> GetRun(const char* filename = "runlist.txt");

#endif /* RUN_HH_ */
