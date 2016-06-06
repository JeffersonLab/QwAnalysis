/***********************************************************
 Programmer: Valerie Gray
 Purpose:

 To investigate how well R3, the VDCs:
 - all treelines
 - treelines that make up the partial tracks
 - treelines that make up the tracks

 Specifically looking at:
 - the Chi squared value
 - the slope

 Entry Conditions: the run number, bool for first 100k
 Date: 02-21-2014
 Modified:04-15-2014
 Assisted By: Me :-) && Wouter Deconinck && the magical goblins
 ***********************************************************/

/********************************
 Note on VDC

 The VDC can be defined as such (from David's auto_vdc.C script)
 - Front Pkg 1 (Vader) (absolute Z = 4507.2 mm)
 ~ fRegion ==3
 ~ fPackage == 1
 ~ fPlane == 1 || fPlane == 2
 - Back  Pkg 1 (Leia) (absolute Z = 5037.8 mm)
 ~ fRegion ==3
 ~ fPackage == 1
 ~ fPlane == 3 || fPlane == 3
 - Front Pkg 2 (Yoda) (absoulte Z = 4497.0 mm)
 ~ fRegion ==3
 ~ fPackage == 2
 ~ fPlane == 1 || fPlane == 2
 - Back Pkg 2 (Han) (absolute Z = 5028.4 mm)
 ~ fRegion ==3
 ~ fPackage == 2
 ~ fPlane == 3 || fPlane == 4

 ********************************/

/********************************
 Note on Tree
 Plane 2 || 4 are V planes
 Plane 1 || 3 are U planes

 This is found by using the fDirection leaf
 ********************************/

//if make and autoplot to be generated on the website, then
// un comment the first include and comment out the next 5 ROOT includes
// ROOT includes
//#include "auto_shared_includes.h"
#include <TCanvas.h>
#include <TChain.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TStyle.h>

#include <TSystem.h>
#include <TLeaf.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>

// Qweak includes
#include "QwEvent.h"

#include <fstream>
#include <iostream>

using std::endl;
using std::cout;

//function definitions
void Define_Histograms();
void Chi_Slope_Treelines(TChain * event_tree);
void Chi_Slope_PartialTracks(TChain * event_tree);
void Chi_Slope_Tracks(TChain * event_tree);
void Plot();
void Print_To_File(int run_num);

// define Globals - constant

// define a PREFIX for all the output files for a run
TString PREFIX;

/*******************************
 - Package:
 - 0 == both
 - 1 == 1
 - 2 ==2

 - Which VDCs
 - 0 == both
 - 1 == front
 - 2 == back

 - Plane
 - 0 == both planes for VDC (defult units of cm/wire)
 - fDirection == 3, UU plane treeline matched to both U planes (unitless)
 - fDirection == 4, VV plane treeline matched to both V planes (unitless)
 - 1 == Front V plane for VDC  (default units of cm/wire)
 - 2 == Front U plane for VDC (defult units of cm/wire)
 - 3 == Back V plane for VDC  (default units of cm/wire)
 - 4 == Back U plane for VDC  (default units of cm/wire)

 FYI - Front and back VDCs have a both plane filled
 in them this means, that I have filled the
 both the U and the V plane information for the
 respective VDC in there (ie it is U plane + V plane
 for the front or back VDC)
 *******************************/

/*******************************
 To get this information out of the ROOT file
 I am only looking at the plane direction etc

 |  Plane  |  Direction  |  correlation
 ----------------------------------------
 |    0    |      3      |  Both VDCs, UU plane (see above)
 ----------------------------------------
 |    0    |      4      |  Both VDCs, VV plane (see above)
 ----------------------------------------
 |    1    |     NA      |  Front V plane
 ----------------------------------------
 |    2    |     NA      |  Front U plane
 ----------------------------------------
 |    3    |     NA      |  Back V plane
 ----------------------------------------
 |    4    |     NA      |  Back U plane
 ----------------------------------------
 *******************************/

// define integers
static const Int_t NUMPACKAGES = 3;
static const Int_t NUMVDCS = 3;
static const Int_t NUMPLANES = 5;
static const Int_t NUMBRANCHES = 3;
static const Double_t WIRE_SPACING = 0.496965;  // cm/wire
// index of vector to what it is
std::string INDEXTOVDC[NUMVDCS] = { "Both", "Front", "Back" };
std::string INDEXTOPLANE[NUMPLANES] = { "Both", "U", "V", "UU", "VV" };
std::string INDEXTOBRANCH[NUMBRANCHES] = { "Treelines", "Partial_Tracks",
    "Tracks" };

// define the range and bin size of the histograms
Double_t SLOPE_MIN = 0.0;
Double_t SLOPE_MAX = 2.0;
Double_t SLOPE_BIN = 50;
Double_t CHI2_MIN = 0.0;
Double_t CHI2_MAX = 7.0;
Double_t CHI2_BIN = 100;

// define the histograms

// Slope  and Chi2 of the treelines
std::vector<std::vector<std::vector<TH1D*> > > h_SLOPE_Treelines;  // [package][whichVDC][plane]
std::vector<std::vector<std::vector<TH1D*> > > h_CHI2_Treelines;  //[package][WhichVDC][plane]

// Slope  and Chi2 of the treelines of the partial tracks branch
std::vector<std::vector<std::vector<TH1D*> > > h_SLOPE_Partial_Tracks;  // [package][whichVDC][plane]
std::vector<std::vector<std::vector<TH1D*> > > h_CHI2_Partial_Tracks;  //[package][WhichVDC][plane]

// Slope  and Chi2 of the treelines of the tracks branch
std::vector<std::vector<std::vector<TH1D*> > > h_SLOPE_Tracks;  // [package][whichVDC][plane]
std::vector<std::vector<std::vector<TH1D*> > > h_CHI2_Tracks;  //[package][WhichVDC][plane]

/***********************************
 set up the significant figures right - &plusmn is for html,
 change to +/- if not useing,
 or in this case \t for a tab space in the outputfile
 Use this by calling: value_with_error(a,da)
 ***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
 << std::setprecision((size_t) (std::log10((a)/(da)) - std::log10(a) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)

void R3_treelines_match_up(int runnum, bool is100k)
{

  //changed the outputPREFIX so that it is compatble with both Root
  //and writing to a file by setting the enviromnet properly
//  PREFIX = Form(
//      TString(gSystem->Getenv("QWSCRATCH")) + "/tracking/www/run_%d/%d_",
//      runnum, runnum);
  PREFIX = Form(
      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
      runnum, runnum);

//  std::cout << "PREFIX is... " << PREFIX << std::endl;

  // Create and load the chain
  TChain* event_tree = new TChain("event_tree");
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root", runnum));

  Define_Histograms();

  Chi_Slope_Treelines(event_tree);

  Chi_Slope_PartialTracks(event_tree);

  Chi_Slope_Tracks(event_tree);

  Plot();

  Print_To_File(runnum);

  return;
}

/***********************************************************
 Function: Define_Histograms
 Purpose:
 To define all the histograms,
 ie. name, axis labels

 Entry Conditions:
 none

 Global:

 - NUMPACKAGES - number of packages

 - SLOPE_MIN - the min value of the slope histogram
 - SLOPE_MAX - the max value of the slope histogram
 - SLOPE_BIN - the bin size of the slope histogram
 - CHI2_MIN - the min value of the Chi2 histogram
 - CHI2_MAX - the max value of the Chi2 histogram
 - CHI2_BIN  - the bin size of the slope histogram

 - NUMPACKAGES - number of packages
 - NUMVDCS - number of vdcs
 - NUMPLANES - number of planes

 - INDEXTOVDC - array to take index of histogram/canvas and
 relate it to and actual VDC
 - INDEXTOPLANE - array to take index of histogram/canvas and
 relate it to and actual Plane

 - h_SLOPE_Treelines - histogram for the slope of the treelines
 - h_CHI2_Treelines - histogram for the Chi2 of the treelines

 - h_SLOPE_Partial_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Partial_Tracks - histogram for the Chi2 of the partial tracks

 - h_SLOPE_Tracks - histogram for the slope of the tracks
 - h_CHI2_Tracks - histogram for the Chi2 of the tracks

 Exit Conditions: none
 Called By: R3_treeline_match_up
 Date: 03-24-2014
 Modified: 03-24-2014
 *********************************************************/
void Define_Histograms()
{

  //resize vector for nub. of packages - use 3 so no need for subtraction
  h_SLOPE_Treelines.resize(NUMPACKAGES);
  h_CHI2_Treelines.resize(NUMPACKAGES);

  h_SLOPE_Partial_Tracks.resize(NUMPACKAGES);
  h_CHI2_Partial_Tracks.resize(NUMPACKAGES);

  h_SLOPE_Tracks.resize(NUMPACKAGES);
  h_CHI2_Tracks.resize(NUMPACKAGES);

  for (int i = 0; i < h_SLOPE_Treelines.size(); i++)
  {
    //resize for the number of VDC
    // 0 = both, 1 = front, 2 = back
    //while I will fill 0, it will probably not be very useful
    h_SLOPE_Treelines[i].resize(NUMVDCS);
    h_CHI2_Treelines[i].resize(NUMVDCS);

    h_SLOPE_Partial_Tracks[i].resize(NUMVDCS);
    h_CHI2_Partial_Tracks[i].resize(NUMVDCS);

    h_SLOPE_Tracks[i].resize(NUMVDCS);
    h_CHI2_Tracks[i].resize(NUMVDCS);

    for (int j = 0; j < h_SLOPE_Treelines[i].size(); j++)
    {
      /***********************************
       resize for each plane
       0 = both planes for VDC
       1 = V plane for VDC
       2 = U plane for VDC
       3 = UU plane treeline matched to both U planes, fDirection == 3
       4 = VV plane treeline matched to both V planes, fDirection == 4
       ***********************************/
      h_SLOPE_Treelines[i][j].resize(NUMPLANES);
      h_CHI2_Treelines[i][j].resize(NUMPLANES);

      h_SLOPE_Partial_Tracks[i][j].resize(NUMPLANES);
      h_CHI2_Partial_Tracks[i][j].resize(NUMPLANES);

      h_SLOPE_Tracks[i][j].resize(NUMPLANES);
      h_CHI2_Tracks[i][j].resize(NUMPLANES);

      for (int k = 0; k < h_SLOPE_Treelines[i][j].size(); k++)
      {
        h_SLOPE_Treelines[i][j][k] = new TH1D(
            Form("h_SLOPE_Treelines[%d][%d][%d]", i, j, k),
            Form("Slope distribution "
                "for treelines in package %d, %s VDC %s plane", i,
                INDEXTOVDC[j].c_str(), INDEXTOPLANE[k].c_str()), SLOPE_BIN,
            SLOPE_MIN, SLOPE_MAX);
        h_SLOPE_Treelines[i][j][k]->GetYaxis()->SetTitle("Frequency");
        h_SLOPE_Treelines[i][j][k]->GetXaxis()->SetTitle("Slope");

        h_CHI2_Treelines[i][j][k] = new TH1D(
            Form("h_CHI2_Treelines[%d][%d][%d]", i, j, k),
            Form("Chi squared distribution "
                "for treelines in package %d, %s VDC %s plane", i,
                INDEXTOVDC[j].c_str(), INDEXTOPLANE[k].c_str()), CHI2_BIN,
            CHI2_MIN, CHI2_MAX);
        h_CHI2_Treelines[i][j][k]->GetYaxis()->SetTitle("Frequency");
        h_CHI2_Treelines[i][j][k]->GetXaxis()->SetTitle("Chi squared");

        //parital tracks
        h_SLOPE_Partial_Tracks[i][j][k] =
            new TH1D(Form("h_SLOPE_Partial_Tracks[%d][%d][%d]", i, j, k),
                Form(
                    "Slope distribution for treelines in the partial tracks in package %d, %s VDC %s plane",
                    i, INDEXTOVDC[j].c_str(), INDEXTOPLANE[k].c_str()),
                SLOPE_BIN, SLOPE_MIN, SLOPE_MAX);
        h_SLOPE_Partial_Tracks[i][j][k]->GetYaxis()->SetTitle("Frequency");
        h_SLOPE_Partial_Tracks[i][j][k]->GetXaxis()->SetTitle("Slope");

        h_CHI2_Partial_Tracks[i][j][k] =
            new TH1D(Form("h_CHI2_Partial_Tracks[%d][%d][%d]", i, j, k),
                Form(
                    "Chi squared distribution for treelines in the partial tracks for package %d, %s VDC %s plane",
                    i, INDEXTOVDC[j].c_str(), INDEXTOPLANE[k].c_str()),
                CHI2_BIN, CHI2_MIN, CHI2_MAX);
        h_CHI2_Partial_Tracks[i][j][k]->GetYaxis()->SetTitle("Frequency");
        h_CHI2_Partial_Tracks[i][j][k]->GetXaxis()->SetTitle("Chi squared");

        //tracks
        h_SLOPE_Tracks[i][j][k] =
            new TH1D(Form("h_SLOPE_Tracks[%d][%d][%d]", i, j, k),
                Form(
                    "Slope distribution for treelines in the tracks in package %d, %s VDC %s plane",
                    i, INDEXTOVDC[j].c_str(), INDEXTOPLANE[k].c_str()),
                SLOPE_BIN, SLOPE_MIN, SLOPE_MAX);
        h_SLOPE_Tracks[i][j][k]->GetYaxis()->SetTitle("Frequency");
        h_SLOPE_Tracks[i][j][k]->GetXaxis()->SetTitle("Slope");

        h_CHI2_Tracks[i][j][k] =
            new TH1D(Form("h_CHI2_Tracks[%d][%d][%d]", i, j, k),
                Form(
                    "Chi squared distribution for treelines in the tracks for package %d, %s VDC %s plane",
                    i, INDEXTOVDC[j].c_str(), INDEXTOPLANE[k].c_str()),
                CHI2_BIN, CHI2_MIN, CHI2_MAX);
        h_CHI2_Tracks[i][j][k]->GetYaxis()->SetTitle("Frequency");
        h_CHI2_Tracks[i][j][k]->GetXaxis()->SetTitle("Chi squared");

      }
    }
  }

}

/***********************************************************
 Function: Chi_Slope_Treelines
 Purpose:

 To loop through all the treelines in the event tree and fill
 the needed histograms
 - Chi Squared
 - Slope

 Entry Conditions:
 TChain - event_tree
 Global:
 - PREFIX - the PREFIX for the output file

 - NUMPACKAGES - number of packages
 - NUMVDCS - number of vdcs
 - NUMPLANES - number of planes

 - INDEXTOVDC - array to take index of histogram/canvas and
 relate it to and actual VDC
 - INDEXTOPLANE - array to take index of histogram/canvas and
 relate it to and actual Plane

 - h_SLOPE_Treelines - histogram for the slope of the treelines
 - h_CHI2_Treelines - histogram for the Chi2 of the treelines


 Exit Conditions: none
 Called By: R3_treeline_match_up
 Date: 02-20-2014
 Modified: 02-26-2014
 *********************************************************/
void Chi_Slope_Treelines(TChain * event_tree)
{
  /***********************************
   Got through the tree
   ***********************************/

  //get number of enties in the tree
  Int_t nentries = event_tree->GetEntries();

  //define and event pointer. the root files are built such that QwEvent stores everything
  QwEvent* fEvent = 0;

  //get the event branch of the event tree and link that to fEvent
  TBranch* event_branch = event_tree->GetBranch("events");
  event_branch->SetAddress(&fEvent);

  //loop over all the events in the tree
  for (int i = 0; i < nentries; i++)
  {
    //Get the ith entry form the event tree
    event_branch->GetEntry(i);

    /***********************************
     Start looping over treelines to get the infomation on
     all the treelines (including the ones that don't make
     up a parital track or track
     ***********************************/
    //get the number of treeline
    Int_t nTreelines = fEvent->GetNumberOfTreeLines();

    //loop over all the treeline in the event
    for (int j = 0; j < nTreelines; j++)
    {
      //get the jth treeline
      const QwTreeLine* treeline = fEvent->GetTreeLine(j);

      //start making cuts and filling histograms
      switch (treeline->GetRegion())
      {
        case 2 :
          break;

        case 3 :
          switch (treeline->GetPlane())
          {

            case 0 :
              if (treeline->GetDirection() == 3)  // this is the UU plane
              {
                h_SLOPE_Treelines[treeline->GetPackage()][0][3]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Treelines[treeline->GetPackage()][0][3]->Fill(
                    treeline->fChi);

                //we are not suppose to care about both, so commenting out till told we want it
//                h_SLOPE_Treelines[0][0][3]->Fill(treeline->GetPackage() == 1 ?
//                  treeline->fSlope : treeline->fSlope);
//                h_CHI2_Treelines[0][0][3]->Fill(treeline->fChi);
              } else if (treeline->GetDirection() == 4)  // this is the VV plane
              {
                h_SLOPE_Treelines[treeline->GetPackage()][0][4]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Treelines[treeline->GetPackage()][0][4]->Fill(
                    treeline->fChi);

                //we are not suppose to care about both, so commenting out till told we want it
//                h_SLOPE_Treelines[0][0][4]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                h_CHI2_Treelines[0][0][4]->Fill(treeline->fChi);
              }

              break;

            case 1 :

              //for package X, front VDC, V plane
              h_SLOPE_Treelines[treeline->GetPackage()][1][2]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][1][2]->Fill(
                  treeline->fChi);

              //for both packages X, front VDC, both planes
              h_SLOPE_Treelines[treeline->GetPackage()][1][0]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][1][0]->Fill(
                  treeline->fChi);

              //we are not suppose to care about both, so commenting out till told we want it
              //for both packages (0), front VDC, V plane
//              h_SLOPE_Treelines[0][1][2]->Fill(treeline->GetPackage() == 1 ?
//                  treeline->fSlope : treeline->fSlope);
//              h_CHI2_Treelines[0][1][2]->Fill(treeline->fChi);

              break;

            case 2 :

              //for package X, front VDC, U plane
              h_SLOPE_Treelines[treeline->GetPackage()][1][1]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][1][1]->Fill(
                  treeline->fChi);

              //for both packages X, front VDC, both planes
              h_SLOPE_Treelines[treeline->GetPackage()][1][0]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][1][0]->Fill(
                  treeline->fChi);

              //we are not suppose to care about both, so commenting out till told we want it
              //for both packages (0), front VDC, U plane
//              h_SLOPE_Treelines[0][1][1]->Fill(treeline->GetPackage() == 1 ?
//                  treeline->fSlope : treeline->fSlope);
//              h_CHI2_Treelines[0][1][1]->Fill(treeline->fChi);

              break;

            case 3 :

              //for package X, Back VDC, V plane
              h_SLOPE_Treelines[treeline->GetPackage()][2][2]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][2][2]->Fill(
                  treeline->fChi);

              //for both packages X, back VDC, both planes
              h_SLOPE_Treelines[treeline->GetPackage()][2][0]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][2][0]->Fill(
                  treeline->fChi);

              //we are not suppose to care about both, so commenting out till told we want it
              //for both packages (0), front VDC, V plane
//              h_SLOPE_Treelines[0][2][2]->Fill(treeline->GetPackage() == 1 ?
//                  treeline->fSlope : treeline->fSlope);
//              h_CHI2_Treelines[0][2][2]->Fill(treeline->fChi);

              break;

            case 4 :

              //for package X, Back VDC, U plane
              h_SLOPE_Treelines[treeline->GetPackage()][2][1]->Fill(
                  treeline->GetPackage() == 1 ?
                      (treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][2][1]->Fill(
                  treeline->fChi);

              //for both packages X, back VDC, both planes
              h_SLOPE_Treelines[treeline->GetPackage()][2][0]->Fill(
                  treeline->GetPackage() == 1 ?
                      -(treeline->fSlope / WIRE_SPACING) :
                      (treeline->fSlope / WIRE_SPACING));
              h_CHI2_Treelines[treeline->GetPackage()][2][0]->Fill(
                  treeline->fChi);

              //we are not suppose to care about both, so commenting out till told we want it
              //for both packages (0), front VDC, U plane
//              h_SLOPE_Treelines[0][2][1]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//              h_CHI2_Treelines[0][2][1]->Fill(treeline->fChi);

              break;

            default :
              break;
          }

          break;

        default :
          break;
      }
    }
  }

  return;
}

/***********************************************************
 Function: Chi_Slope_PartialTracks
 Purpose:

 To loop through all the treelines in the partial tracks event tree and fill
 the needed histograms
 - Chi Squared
 - Slope

 Entry Conditions:
 TChain - event_tree
 Global:
 - PREFIX - the PREFIX for the output file

 - NUMPACKAGES - number of packages
 - NUMVDCS - number of vdcs
 - NUMPLANES - number of planes

 - INDEXTOVDC - array to take index of histogram/canvas and
 relate it to and actual VDC
 - INDEXTOPLANE - array to take index of histogram/canvas and
 relate it to and actual Plane

 - h_SLOPE_Partial_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Partial_Tracks - histogram for the Chi2 of the partial tracks

 Exit Conditions: none
 Called By: R3_treeline_match_up
 Date: 02-24-2014
 Modified: 02-26-2014
 *********************************************************/
void Chi_Slope_PartialTracks(TChain * event_tree)
{
  /***********************************
   Got through the tree
   ***********************************/

  //get number of enties in the tree
  Int_t nentries = event_tree->GetEntries();

  //define and event pointer. the root files are built such that QwEvent stores everything
  QwEvent* fEvent = 0;

  //get the event branch of the event tree and link that to fEvent
  TBranch* event_branch = event_tree->GetBranch("events");
  event_branch->SetAddress(&fEvent);

  //loop over all the events in the tree
  for (int i = 0; i < nentries; i++)
  {
    //Get the ith entry form the event tree
    event_branch->GetEntry(i);

    /***********************************
     Start looping over partial tracks to
     get the infomation on all the treelines
     in loop of partial tracks
     ***********************************/

    //get the number of partial tracks
    Int_t nPartialTracks = fEvent->GetNumberOfPartialTracks();

    //loop over partial tracks
    for (Int_t p = 0; p < nPartialTracks; p++)
    {
      // get the pth partial track
      const QwPartialTrack* partialtracks = fEvent->GetPartialTrack(p);

      //get the number of treeline
      Int_t nTreelines = partialtracks->GetNumberOfTreeLines();

      //loop over all the treeline in the event
      for (int j = 0; j < nTreelines; j++)
      {
        //get the jth treeline
        const QwTreeLine* treeline = partialtracks->GetTreeLine(j);

        //start making cuts and filling histograms
        switch (treeline->GetRegion())
        {
          case 2 :
            break;

          case 3 :
            switch (treeline->GetPlane())
            {
              case 0 :
                if (treeline->GetDirection() == 3)  // this is the UU plane
                {
                  h_SLOPE_Partial_Tracks[treeline->GetPackage()][0][3]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Partial_Tracks[treeline->GetPackage()][0][3]->Fill(
                      treeline->fChi);

                  //we are not suppose to care about both, so commenting out till told we want it
//                  h_SLOPE_Partial_Tracks[0][0][3]->Fill(treeline->GetPackage() == 1 ?
//                 -treeline->fSlope : treeline->fSlope);
//                 h_CHI2_Partial_Tracks[0][0][3]->Fill(treeline->fChi);

                } else if (treeline->GetDirection() == 4)  // this is the VV plane
                {
                  h_SLOPE_Partial_Tracks[treeline->GetPackage()][0][4]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Partial_Tracks[treeline->GetPackage()][0][4]->Fill(
                      treeline->fChi);

                  //we are not suppose to care about both, so commenting out till told we want it
//                    h_SLOPE_Partial_Tracks[0][0][4]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                   h_CHI2_Partial_Tracks[0][0][4]->Fill(treeline->fChi);

                }

                break;

              case 1 :
                //for package X, front VDC, V plane
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][1][2]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][1][2]->Fill(
                    treeline->fChi);

                //for both packages X, front VDC, both planes
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][1][0]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][1][0]->Fill(
                    treeline->fChi);

                //we are not suppose to care about both, so commenting out till told we want it
                //for both packages (0), front VDC, V plane
//                h_SLOPE_Partial_Tracks[0][1][2]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//               h_CHI2_Partial_Tracks[0][1][2]->Fill(treeline->fChi);

                break;

              case 2 :
                //for package X, front VDC, U plane
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][1][1]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][1][1]->Fill(
                    treeline->fChi);

                //for both packages X, front VDC, both planes
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][1][0]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][1][0]->Fill(
                    treeline->fChi);

                //we are not suppose to care about both, so commenting out till told we want it
                //for both packages (0), front VDC, U plane
//                h_SLOPE_Partial_Tracks[0][1][1]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//               h_CHI2_Partial_Tracks[0][1][1]->Fill(treeline->fChi);

                break;

              case 3 :
                //for package X, Back VDC, V plane
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][2][2]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][2][2]->Fill(
                    treeline->fChi);

                //for both packages X, back VDC, both planes
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][2][0]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][2][0]->Fill(
                    treeline->fChi);

                //we are not suppose to care about both, so commenting out till told we want it
                //for both packages (0), front VDC, V plane
//                h_SLOPE_Partial_Tracks[0][2][2]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//               h_CHI2_Partial_Tracks[0][2][2]->Fill(treeline->fChi);

                break;

              case 4 :
                //for package X, front VDC, U plane
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][2][1]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][2][1]->Fill(
                    treeline->fChi);

                //for both packages X, back VDC, both planes
                h_SLOPE_Partial_Tracks[treeline->GetPackage()][2][0]->Fill(
                    treeline->GetPackage() == 1 ?
                        -treeline->fSlope : treeline->fSlope);
                h_CHI2_Partial_Tracks[treeline->GetPackage()][2][0]->Fill(
                    treeline->fChi);

                //we are not suppose to care about both, so commenting out till told we want it
                //for both packages (0), front VDC, U plane
//                h_SLOPE_Partial_Tracks[0][2][1]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//               h_CHI2_Partial_Tracks[0][2][1]->Fill(treeline->fChi);

                break;

              default :
                break;

            }

            break;

          default :
            break;

        }
      }
    }
  }

  return;
}

/***********************************************************
 Function: Chi_Slope_Trackspartialtracks
 Purpose:

 To loop through all the treelines in the tracks event tree and fill
 the needed histograms
 - Chi Squared
 - Slope

 Entry Conditions:
 TChain - event_tree
 Global:
 - PREFIX - the PREFIX for the output file

 - NUMPACKAGES - number of packages
 - NUMVDCS - number of vdcs
 - NUMPLANES - number of planes

 - INDEXTOVDC - array to take index of histogram/canvas and
 relate it to and actual VDC
 - INDEXTOPLANE - array to take index of histogram/canvas and
 relate it to and actual Plane

 - h_SLOPE_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Tracks - histogram for the Chi2 of the partial tracks

 Exit Conditions: none
 Called By: R3_treeline_match_up
 Date: 02-26-2014
 Modified:
 *********************************************************/
void Chi_Slope_Tracks(TChain * event_tree)
{
  /***********************************
   Got through the tree
   ***********************************/

  //get number of enties in the tree
  Int_t nentries = event_tree->GetEntries();

  //define and event pointer. the root files are built such that QwEvent stores everything
  QwEvent* fEvent = 0;

  //get the event branch of the event tree and link that to fEvent
  TBranch* event_branch = event_tree->GetBranch("events");
  event_branch->SetAddress(&fEvent);

  //loop over all the events in the tree
  for (int i = 0; i < nentries; i++)
  {
    //Get the ith entry form the event tree
    event_branch->GetEntry(i);

    /***********************************
     Start looping over tracks to
     get the infomation on all the treelines
     in loop of partial tracks under the tracks
     ***********************************/

    //get number of tracks
    Int_t nTracks = fEvent->GetNumberOfTracks();

    //loop over the tracks
    for (int q = 0; q < nTracks; q++)
    {

      //get the qth track
      const QwTrack* track = fEvent->GetTrack(q);

      //get the number of partial tracks in that treeline
      Int_t nPartialTracks = track->GetNumberOfPartialTracks();

      //loop over those partial tracks
      for (Int_t p = 0; p < nPartialTracks; p++)
      {

        //get the pth partial track in the track
        /***********************************
         but treelines under the partial tracks is empty
         but in the fFraont & fBack branchs do have this
         information so, use them
         the frist partial track will always be
         the front and the second will always be
         the back one
         ***********************************/
        const QwPartialTrack* partialtracks;
        if (p == 0)
          partialtracks = track->fFront;
        if (p == 1)
          partialtracks = track->fBack;

        //get the number of treelines in the pth partial track in then
        //qth track
        Int_t nTreelines = partialtracks->GetNumberOfTreeLines();

        //loop over those treelines
        for (int j = 0; j < nTreelines; j++)
        {

          //get the jth treeline
          const QwTreeLine* treeline = partialtracks->GetTreeLine(j);

          //start making and filling the histograms
          switch (treeline->GetRegion())
          {
            case 2 :
              break;

            case 3 :
              switch (treeline->GetPlane())
              {
                case 0 :
                  if (treeline->GetDirection() == 3)  // this is the UU plane
                  {
                    h_SLOPE_Tracks[treeline->GetPackage()][0][3]->Fill(
                        treeline->GetPackage() == 1 ?
                            -treeline->fSlope : treeline->fSlope);
                    h_CHI2_Tracks[treeline->GetPackage()][0][3]->Fill(
                        treeline->GetPackage() == 1 ?
                            -treeline->fSlope : treeline->fSlope);

                    //we are not suppose to care about both,
                    //so commenting out till told we want it
//                    h_SLOPE_Partial_Tracks[0][0][3]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                   h_CHI2_Partial_Tracks[0][0][3]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);

                  } else if (treeline->GetDirection() == 4)  // this is the VV plane
                  {
                    h_SLOPE_Tracks[treeline->GetPackage()][0][4]->Fill(
                        treeline->GetPackage() == 1 ?
                            -treeline->fSlope : treeline->fSlope);
                    h_CHI2_Tracks[treeline->GetPackage()][0][4]->Fill(
                        treeline->GetPackage() == 1 ?
                            -treeline->fSlope : treeline->fSlope);

                    //we are not suppose to care about both,
                    //so commenting out till told we want it
//                      h_SLOPE_Tracks[0][0][4]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                      h_CHI2_Tracks[0][0][4]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);

                  }

                  break;

                case 1 :
                  //for package X, front VDC, V plane
                  h_SLOPE_Tracks[treeline->GetPackage()][1][2]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][1][2]->Fill(
                      treeline->fChi);

                  //for both packages X, front VDC, both planes
                  h_SLOPE_Tracks[treeline->GetPackage()][1][0]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][1][0]->Fill(
                      treeline->fChi);

                  //we are not suppose to care about both,
                  //so commenting out till told we want it
                  //for both packages (0), front VDC, V plane
//                  h_SLOPE_Tracks[0][1][2]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                  h_CHI2_Tracks[0][1][2]->Fill(treeline->fChi);

                  break;

                case 2 :
                  //for package X, front VDC, U plane
                  h_SLOPE_Tracks[treeline->GetPackage()][1][1]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][1][1]->Fill(
                      treeline->fChi);

                  //for both packages X, front VDC, both planes
                  h_SLOPE_Tracks[treeline->GetPackage()][1][0]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][1][0]->Fill(
                      treeline->fChi);

                  //we are not suppose to care about both,
                  //so commenting out till told we want it
                  //for both packages (0), front VDC, U plane
//                  h_SLOPE_Tracks[0][1][1]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                  h_CHI2_Tracks[0][1][1]->Fill(treeline->fChi);

                  break;

                case 3 :
                  //for package X, Back VDC, V plane
                  h_SLOPE_Tracks[treeline->GetPackage()][2][2]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][2][2]->Fill(
                      treeline->fChi);

                  //for both packages X, back VDC, both planes
                  h_SLOPE_Tracks[treeline->GetPackage()][2][0]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][2][0]->Fill(
                      treeline->fChi);

                  //we are not suppose to care about both,
                  //so commenting out till told we want it
                  //for both packages (0), front VDC, V plane
//                  h_SLOPE_Tracks[0][2][2]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                  h_CHI2_Tracks[0][2][2]->Fill(treeline->fChi);

                  break;

                case 4 :
                  //for package X, front VDC, U plane
                  h_SLOPE_Tracks[treeline->GetPackage()][2][1]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][2][1]->Fill(
                      treeline->fChi);

                  //for both packages X, back VDC, both planes
                  h_SLOPE_Tracks[treeline->GetPackage()][2][0]->Fill(
                      treeline->GetPackage() == 1 ?
                          -treeline->fSlope : treeline->fSlope);
                  h_CHI2_Tracks[treeline->GetPackage()][2][0]->Fill(
                      treeline->fChi);

                  //we are not suppose to care about both,
                  //so commenting out till told we want it
                  //for both packages (0), front VDC, U plane
//                  h_SLOPE_Tracks[0][2][1]->Fill(treeline->GetPackage() == 1 ?
//                  -treeline->fSlope : treeline->fSlope);
//                  h_CHI2_Tracks[0][2][1]->Fill(treeline->fChi);

                  break;

                default :
                  break;

              }

            default :
              break;

          }
        }

      }
    }
  }

  return;
}

/***********************************************************
 Function: Plot
 Purpose:
 To print out the histograms to canvases and save it

 Entry Conditions:
 none

 Global:
 - PREFIX - the PREFIX for the output file

 - h_SLOPE_Treelines - histogram for the slope of the treelines
 - h_CHI2_Treelines - histogram for the Chi2 of the treelines
 - h_SLOPE_Partial_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Partial_Tracks - histogram for the Chi2 of the partial tracks
 - h_SLOPE_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Tracks - histogram for the Chi2 of the partial tracks

 - NUMPACKAGES - number of packages
 - NUMPACKAGES - number of branchs

 - INDEXTOBRANCH - array of trees looking at

 Exit Conditions: none
 Called By: R3_treelines_match_up
 Date: 03-24-2014
 Modified: 04-15-2014
 *********************************************************/
void Plot()
{
  /***********************************
   Define a histogram which will be used as a temporay
   holder of the histograms that will be drawen
   thia allows for using varable in the name of a
   histogram :)
   ***********************************/
  TH1D* h_Temp_Chi2;
  TH1D* h_Temp_Slope;

  /***********************************
   Define my canvases, Draw and save
   ***********************************/
  //Canvas
  std::vector < std::vector<std::vector<TCanvas*> > > c_Slope;  //[branch][pkg][whichVDC]
  std::vector < std::vector<std::vector<TCanvas*> > > c_Chi2;  //[branch][pkg][whichVDC]

  //resize to number of branches (trees) we are looking at
  c_Chi2.resize(NUMBRANCHES);
  c_Slope.resize(NUMBRANCHES);

  //loop over branches
  for (int branch = 0; branch < c_Chi2.size(); branch++)
  {
    //resize to number of packages
    c_Chi2[branch].resize(NUMPACKAGES);
    c_Slope[branch].resize(NUMPACKAGES);

    switch (branch)
    {
      case 0 :  //this is the treeline branch
        /***********************************
         This is the treelines there are none zero values
         for all VDCs planes
         ************************************/
        //loop over packages
        for (int pkg = 0; pkg < c_Chi2[branch].size(); pkg++)
        {
          switch (pkg)
          {
            case 0 :
              //we don't care about the both combination
              break;

            case 1 :  //package 1
            case 2 :  //package 2
              //resize to number of planes
              c_Chi2[branch][pkg].resize(NUMVDCS);
              c_Slope[branch][pkg].resize(NUMVDCS);

              //loop over the number of VDCs
              for (int VDC = 0; VDC < c_Chi2[branch][pkg].size(); VDC++)
              {
                //name the canvases
                c_Slope[branch][pkg][VDC] = new TCanvas(
                    Form("c_Slope[%d][%d][%d]", branch, pkg, VDC),
                    Form("Slope for %s in package %d, %s VDC",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()), 900, 300);

                c_Chi2[branch][pkg][VDC] = new TCanvas(
                    Form("c_Chi2[%d][%d][%d]", branch, pkg, VDC),
                    Form("Chi squared for %s in package %d, %s VDC",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()), 900, 300);

                if (VDC == 0)  //this is the both VDC combination
                {
                  c_Chi2[branch][pkg][VDC]->Divide(2, 0);
                  c_Slope[branch][pkg][VDC]->Divide(2, 0);

                  for (int plane = 1; plane <= 2; plane++)  //graphing the UU and VV plane
                  {
                    //draw the Chi 2 stuff
                    c_Chi2[branch][pkg][VDC]->cd(plane);
                    h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                        Form("h_CHI2_%s[%d][%d][%d]",
                            INDEXTOBRANCH[branch].c_str(), pkg, VDC,
                            plane + 2));
                    h_Temp_Chi2->Draw();

                    //draw the slope stuff
                    c_Slope[branch][pkg][VDC]->cd(plane);
                    h_Temp_Slope = (TH1D*) gDirectory->Get(
                        Form("h_SLOPE_%s[%d][%d][%d]",
                            INDEXTOBRANCH[branch].c_str(), pkg, VDC,
                            plane + 2));
                    h_Temp_Slope->Draw();
                  }
                } else  //front or back VDCs then we have both, U, and V planes
                {
                  c_Chi2[branch][pkg][VDC]->Divide(3, 0);
                  c_Slope[branch][pkg][VDC]->Divide(3, 0);

                  for (int plane = 1; plane <= 3; plane++)
                  {
                    //draw the Chi 2 stuff
                    c_Chi2[branch][pkg][VDC]->cd(plane);
                    h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                        Form("h_CHI2_%s[%d][%d][%d]",
                            INDEXTOBRANCH[branch].c_str(), pkg, VDC,
                            plane - 1));
                    if (h_Temp_Chi2 == 0)
                      cout << "Error: Histogram "
                      << Form("h_CHI2_%s[%d][%d][%d]",
                          INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane - 1)
                      << " doesn't exist!" << endl;
                    else
                      h_Temp_Chi2->Draw();

                    //draw the slope stuff
                    c_Slope[branch][pkg][VDC]->cd(plane);
                    h_Temp_Slope = (TH1D*) gDirectory->Get(
                        Form("h_SLOPE_%s[%d][%d][%d]",
                            INDEXTOBRANCH[branch].c_str(), pkg, VDC,
                            plane - 1));
                    h_Temp_Slope->Draw();
                  }
                }

                c_Slope[branch][pkg][VDC]->SaveAs(
                    PREFIX + Form("%s_Slope_package_%d_%s_VDC.png",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()));
                c_Slope[branch][pkg][VDC]->SaveAs(
                    PREFIX + Form("%s_Slope_package_%d_%s_VDC.C",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()));
                c_Chi2[branch][pkg][VDC]->SaveAs(
                    PREFIX + Form("%s_Chi2_package_%d_%s_VDC.png",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()));
                c_Chi2[branch][pkg][VDC]->SaveAs(
                    PREFIX + Form("%s_Chi2_package_%d_%s_VDC.C",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()));
              }

              break;

            default :
              break;
          }
        }
        break;

      case 1 :
      case 2 :
        /***********************************
         This is the Parital tracks and tracks
         there are none zero values for only the
         UU and VV VDCs planes
         ************************************/
        //loop over packages
        for (int pkg = 0; pkg < c_Chi2[branch].size(); pkg++)
        {
          switch (pkg)
          {
            case 0 :
              //we don't care about the both combination
              break;

            case 1 :  //package 1
            case 2 :  //package 2
              //resize to number of planes
              c_Chi2[branch][pkg].resize(NUMVDCS);
              c_Slope[branch][pkg].resize(NUMVDCS);

              //loop over the number of VDCs
              for (int VDC = 0; VDC < c_Chi2[branch][pkg].size(); VDC++)
              {
                //name the canvases
                c_Slope[branch][pkg][VDC] = new TCanvas(
                    Form("c_Slope[%d][%d][%d]", branch, pkg, VDC),
                    Form("Slope for %s in package %d, %s VDC",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()), 900, 300);

                c_Chi2[branch][pkg][VDC] = new TCanvas(
                    Form("c_Chi2[%d][%d][%d]", branch, pkg, VDC),
                    Form("Chi squared for %s in package %d, %s VDC",
                        INDEXTOBRANCH[branch].c_str(), pkg,
                        INDEXTOVDC[VDC].c_str()), 900, 300);

                switch (VDC)
                {
                  case 0 :
                    // this is the both VDCs
                    //has UU and VV planes, and all we care about
                    c_Chi2[branch][pkg][VDC]->Divide(2, 0);
                    c_Slope[branch][pkg][VDC]->Divide(2, 0);

                    for (int plane = 1; plane <= 2; plane++)  //graphing the UU and VV plane
                    {
                      //draw the Chi 2 stuff
                      c_Chi2[branch][pkg][VDC]->cd(plane);
                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC,
                              plane + 2));
                      h_Temp_Chi2->Draw();

                      //draw the slope stuff
                      c_Slope[branch][pkg][VDC]->cd(plane);
                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC,
                              plane + 2));
                      h_Temp_Slope->Draw();
                    }

                    c_Slope[branch][pkg][VDC]->SaveAs(
                        PREFIX + Form("%s_Slope_package_%d_%s_VDC.png",
                            INDEXTOBRANCH[branch].c_str(), pkg,
                            INDEXTOVDC[VDC].c_str()));
                    c_Chi2[branch][pkg][VDC]->SaveAs(
                        PREFIX + Form("%s_Chi2_package_%d_%s_VDC.png",
                            INDEXTOBRANCH[branch].c_str(), pkg,
                            INDEXTOVDC[VDC].c_str()));

                    break;

                  case 1 :  //this the Front therfore only U and V empty
                  case 2 :  //this the Back therfore only U and V empty
                    break;

                  default :
                    break;
                }

              }

              break;

            default :
              break;
          }
        }

        break;

      default :
        break;
    }
  }

  return;
}

/***********************************************************
 Function: Print_To_File
 Purpose:
 To print out the important info found in this macro to a file

 Entry Conditions:
 none

 Global:
 - PREFIX - the PREFIX for the output file

 - h_SLOPE_Treelines - histogram for the slope of the treelines
 - h_CHI2_Treelines - histogram for the Chi2 of the treelines
 - h_SLOPE_Partial_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Partial_Tracks - histogram for the Chi2 of the partial tracks
 - h_SLOPE_Tracks - histogram for the slope of the partial tracks
 - h_CHI2_Tracks - histogram for the Chi2 of the partial tracks

 - NUMPACKAGES - number of packages
 - NUMPACKAGES - number of branchs

 - INDEXTOBRANCH - array of trees looking at

 Exit Conditions:
 run_num - the run number

 Called By: R3_treelines_match_up
 Date: 03-26-2014
 Modified: 04-15-2014
 *********************************************************/
void Print_To_File(int run_num)
{
  /***********************************
   Define a histogram which will be used as a temporay
   holder of the histograms that will be drawen
   thia allows for using varable in the name of a
   histogram :)
   ***********************************/
  TH1D* h_Temp_Chi2;
  TH1D* h_Temp_Slope;

  //print to file
  //this file and evrything related to it is fout
  std::ofstream fout_Treelines_Slope;
  std::ofstream fout_PartialTracks_Slope;
  std::ofstream fout_Tracks_Slope;
  std::ofstream fout_Treelines_Chi2;
  std::ofstream fout_PartialTracks_Chi2;
  std::ofstream fout_Tracks_Chi2;
  //open files
  // open file with outputPrefix+blahblah.txt which will
  // store the output of the vlaues to a file in a easy way
  //that should be able to be read back into a program if needed
  //Prefix will inculed run number which we need.
  fout_Treelines_Slope.open(PREFIX + "R3_Treeline_match_up_Slope.txt");
  if (!fout_Treelines_Slope.is_open())
    cout << "File Treelines Slope not opened" << endl;

  fout_PartialTracks_Slope.open(
      PREFIX + "R3_Partial_Tracks_match_up_Slope.txt");
  if (!fout_PartialTracks_Slope.is_open())
    cout << "File Partial Tracks Slope not opened" << endl;

  fout_Tracks_Slope.open(PREFIX + "R3_Tracks_match_up_Slope.txt");
  if (!fout_Tracks_Slope.is_open())
    cout << "File Tracks Slope not opened" << endl;

  fout_Treelines_Chi2.open(PREFIX + "R3_Treeline_match_up_Chi2.txt");
  if (!fout_Treelines_Chi2.is_open())
    cout << "File Treelines Chi not opened" << endl;

  fout_PartialTracks_Chi2.open(PREFIX + "R3_Partial_Tracks_match_up_Chi2.txt");
  if (!fout_PartialTracks_Chi2.is_open())
    cout << "File Partial Tracks Chi2 not opened" << endl;

  fout_Tracks_Chi2.open(PREFIX + "R3_Tracks_match_up_Chi2.txt");
  if (!fout_Tracks_Chi2.is_open())
    cout << "File Tracks Chi2 not opened" << endl;

  //loop over branches, each branch will have its own output file
  for (int branch = 0; branch < NUMBRANCHES; branch++)
  {
    switch (branch)
    {
      case 0 :  //the treelines
        fout_Treelines_Slope << "Run \t pkg \t VDC \t Plane \t nEvents \t Value \t Error"
        << endl;
        fout_Treelines_Chi2 << "Run \t pkg \t VDC \t Plane \t nEvents \t Value \t RMS"
        << endl;

        for (int pkg = 0; pkg < NUMPACKAGES; pkg++)
        {
          /***********************************
           This is the treelines there are none zero values
           for all VDCs planes
           ************************************/
          //loop over packages
          switch (pkg)
          {
            case 0 :
              //we don't care about the both combination
              break;

            case 1 :  //package 1
            case 2 :  //package 2
              //resize to number of planes
              //loop over the number of VDCs
              for (int VDC = 0; VDC < NUMVDCS; VDC++)
              {

                switch (VDC)
                //this is the both VDC combination
                {
                  case 0 :
                    for (int plane = 3; plane < 5; plane++)  //writing out the UU and VV plane
                    {
                      //Slope
                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Treelines_Slope << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Slope->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Slope->GetMean(),
                          (h_Temp_Slope->GetRMS() / sqrt(
                               h_Temp_Slope->GetEntries())))
                      << endl;

                      //Chi2
                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Treelines_Chi2 << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Chi2->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Chi2->GetMean(),
                          (h_Temp_Chi2->GetRMS() / sqrt(
                               h_Temp_Chi2->GetEntries())))
                      << endl;
                    }
                    break;

                  case 1 :  //Front VDC
                  case 2 :  //Back VDC
                    for (int plane = 0; plane < 3; plane++)  //both U and V planes
                    {

                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Treelines_Slope << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Slope->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Slope->GetMean(),
                          (h_Temp_Slope->GetRMS() / sqrt(
                               h_Temp_Slope->GetEntries())))
                      << endl;

                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Treelines_Chi2 << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Chi2->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Chi2->GetMean(),
                          (h_Temp_Chi2->GetRMS() / sqrt(
                               h_Temp_Chi2->GetEntries())))
                      << endl;

                    }
                    break;

                  default :
                    break;

                }
              }

          }
        }
        break;

      case 1 :  //Patial Tracks branch
        fout_PartialTracks_Slope << "Run \t pkg \t VDC \t Plane \t NumEvents \t Value \t Error"
        << endl;
        fout_PartialTracks_Chi2 << "Run \t pkg \t VDC \t Plane \t NumEvents \t Value \t Error"
        << endl;

        for (int pkg = 0; pkg < NUMPACKAGES; pkg++)
        {
          /***********************************
           This is the Patial Tracks there are none zero values
           for all VDCs planes
           ************************************/
          //loop over packages
          switch (pkg)
          {
            case 0 :
              //we don't care about the both combination
              break;

            case 1 :  //package 1
            case 2 :  //package 2
              //resize to number of planes
              //loop over the number of VDCs
              for (int VDC = 0; VDC < NUMVDCS; VDC++)
              {

                switch (VDC)
                //this is the both VDC combination
                {
                  case 0 :  //The UU && VV planes all we have in this case
                    for (int plane = 3; plane < 5; plane++)
                    {
                      //Slope
                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_PartialTracks_Slope << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Slope->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Slope->GetMean(),
                          (h_Temp_Slope->GetRMS() / sqrt(
                               h_Temp_Slope->GetEntries())))
                      << endl;

                      //Chi2
                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_PartialTracks_Chi2 << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Chi2->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Chi2->GetMean(),
                          (h_Temp_Chi2->GetRMS() / sqrt(
                               h_Temp_Chi2->GetEntries())))
                      << endl;
                    }
                    break;

                  case 1 :  //Front VDC
                  case 2 :  //Back VDC
                    for (int plane = 0; plane < 3; plane++)  //both U and V planes
                    {

                      //Slope
                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_PartialTracks_Slope << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Slope->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Slope->GetMean(),
                          (h_Temp_Slope->GetRMS() / sqrt(
                               h_Temp_Slope->GetEntries())))
                      << endl;

                      //Chi2
                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_PartialTracks_Chi2 << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Chi2->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Chi2->GetMean(),
                          (h_Temp_Chi2->GetRMS() / sqrt(
                               h_Temp_Chi2->GetEntries())))
                      << endl;
                    }
                  default :
                    break;

                }
              }

          }
        }
        break;

      case 2 :  //Tracks branch
        fout_Tracks_Slope << "Run \t pkg \t VDC \t Plane \t NumEvents \t Value \t Error"
        << endl;
        fout_Tracks_Chi2 << "Run \t pkg \t VDC \t Plane \t NumEvents \t Value \t Error"
        << endl;

        for (int pkg = 0; pkg < NUMPACKAGES; pkg++)
        {
          /***********************************
           This is the  Tracks there are none zero values
           for all VDCs planes
           ************************************/
          //loop over packages
          switch (pkg)
          {
            case 0 :
              //we don't care about the both combination
              break;

            case 1 :  //package 1
            case 2 :  //package 2
              //resize to number of planes
              //loop over the number of VDCs
              for (int VDC = 0; VDC < NUMVDCS; VDC++)
              {

                switch (VDC)
                //this is the both VDC combination
                {
                  case 0 :  //The UU && VV planes all we have in this case
                    for (int plane = 3; plane < 5; plane++)
                    {
                      //Slope
                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Tracks_Slope << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Slope->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Slope->GetMean(),
                          (h_Temp_Slope->GetRMS() / sqrt(
                               h_Temp_Slope->GetEntries())))
                      << endl;

                      //Chi2
                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Tracks_Chi2 << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Chi2->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Chi2->GetMean(),
                          (h_Temp_Chi2->GetRMS() / sqrt(
                               h_Temp_Chi2->GetEntries())))
                      << endl;
                    }
                    break;

                  case 1 :  //Front VDC
                  case 2 :  //Back VDC
                    for (int plane = 0; plane < 3; plane++)  //both U and V planes
                    {

                      h_Temp_Slope = (TH1D*) gDirectory->Get(
                          Form("h_SLOPE_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Tracks_Slope << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Slope->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Slope->GetMean(),
                          (h_Temp_Slope->GetRMS() / sqrt(
                               h_Temp_Slope->GetEntries())))
                      << endl;

                      h_Temp_Chi2 = (TH1D*) gDirectory->Get(
                          Form("h_CHI2_%s[%d][%d][%d]",
                              INDEXTOBRANCH[branch].c_str(), pkg, VDC, plane));

                      fout_Tracks_Chi2 << run_num
                      << " \t "
                      << pkg
                      << " \t "
                      << INDEXTOVDC[VDC].c_str()
                      << " \t "
                      << INDEXTOPLANE[plane].c_str()
                      << " \t "
                      << std::setprecision(10)
                      << h_Temp_Chi2->GetEntries()
                      << " \t"
                      << value_with_error(h_Temp_Chi2->GetMean(),
                          (h_Temp_Chi2->GetRMS() / sqrt(
                               h_Temp_Chi2->GetEntries())))
                      << endl;

                    }
                  default :
                    break;

                }
              }

          }
        }
        break;

      default :
        break;
    }
  }

  return;
}
