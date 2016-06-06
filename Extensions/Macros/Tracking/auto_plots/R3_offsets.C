/***********************************************************
Programmer: Valerie Gray
Purpose:

To look a the offsets that are in the tracks.

Specifically the momentum
  - Thesta offset
  - Phi offset

Entry Conditions: the run number, bool for first 100k
Date: 02-25-2014
Modified: 05-07-2014
Assisted By: Me :-), Juan Carlos Cornejo, Wouter Deconinck
***********************************************************/

/********************************
Notes on what the *off means
  -fPositionXoff - Difference in X position at matching plane.
  -fPositionYoff - Difference in Y position at matching plane.
  -fPositionRoff - Difference in radial position at matching plane.
  -fPositionPhioff - Difference in azimuthal angle phi position at matching plane.
  -fPositionThetaoff - Difference in polar angle theta position at matching plane.

  -fDirectionXoff - Difference in X momentum at matching plane.
  -fDirectionYoff - Difference in Y momentum at matching plane.
  -fDirectionZoff - Difference in Z momentum at matching plane.
  -fDirectionPhioff - Difference in momentum angle phi at matching plane.
  -fDirectionThetaoff - Difference in momentum angle theta at matching plane.

********************************/

//if make and autoPlot to be generated on the website, then
// un-comment the first include and comment out the next 5 ROOT includes

// ROOT includes
//#include "auto_shared_includes.h"
#include <TCanvas.h>
#include <TChain.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TPad.h>
#include <TSystem.h>

// Qweak includes
#include "QwEvent.h"

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

using std::endl;
using std::cout;

//function definitions
void Define_Histograms();
void Track_Loop(TChain * event_tree);
void Plot();
void Print_To_File(int run_num);

// define integers
static const Int_t NUMPACKAGES = 3;
static const Int_t NUMDIRECTIONOFF = 5;
static const Int_t NUMPOSITIONOFF= 5;

//string to Position or direction
std::string INDEXTOPositionOFF[NUMPOSITIONOFF] = {"X","Y","R","Phi","Theta"};
std::string INDEXTODirectionOFF[NUMDIRECTIONOFF] = {"X","Y","Z","Phi","Theta"};
std::string INDEXTOPACKAGE[NUMPACKAGES] = {"Both","1","2"};

// define the range and bin size of the histograms
Double_t DirectionOFF_THETA_MIN = -0.5;
Double_t DirectionOFF_THETA_MAX = 0.5;
Double_t DirectionOFF_THETA_BIN = 50;

Double_t DirectionOFF_PHI_MIN = -0.8;
Double_t DirectionOFF_PHI_MAX = 0.8;
Double_t DirectionOFF_PHI_BIN = 50;

/***********************************
Double_t PositionOFF_THETA_MIN = 0.0;
Double_t PositionOFF_THETA_MAX = -1.0;
Double_t PositionOFF_THETA_BIN = 50;

Double_t PositionOFF_PHI_MIN = 0.0;
Double_t PositionOFF_PHI_MAX = -1.0;
Double_t PositionOFF_PHI_BIN = 50;
***********************************/

// define the histograms

// Direction offset of Tracks
std::vector<TH1D*> h_Direction_Theta_OFF; // [package]
std::vector<TH1D*> h_Direction_Phi_OFF; // [package]

/***********************************
Setting up what would be needed for the position
if we want it
***********************************/
/***********************************
// Position offset of Tracks
std::vector<TH1D*> h_Position_Theta_OFF; // [package]
std::vector<TH1D*> h_Position_Phi_OFF; // [package]
***********************************/

// define a PREFIX for all the output files for a run
TString PREFIX;

/***********************************
set up the significant figures right - &plusmn is for html,
  change to +/- if not useing,
  or in this case \t for a tab space in the outputfile
Use this by calling: value_with_error(a,da)
***********************************/
#define value_with_error(a,da) std::setiosflags(std::ios::fixed) \
 << std::setprecision((size_t) (std::log10((a)/(da)) - std::log10(a) + 1.5)) \
 << " " << (a) << " \t " << (da) << std::resetiosflags(std::ios::fixed)

void R3_offsets (int runnum, bool is100k)
{

  //changed the output PREFIX so that it is compatible with both Root
  //and writing to a file by setting the environment properly
  //PREFIX = Form(TString(gSystem->Getenv("QWSCRATCH"))+"/tracking/www/run_%d/%d_",runnum,runnum);

  PREFIX = Form(
         TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_",
         runnum, runnum);

  // Create and load the chain
  TChain* event_tree = new TChain("event_tree");
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

  Define_Histograms();

  Track_Loop(event_tree);

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

  - DirectionOFF_THETA_MIN - the min value of the direction theta histogram
  - DirectionOFF_THETA_MAX - the max value of the direction theta histogram
  - DirectionOFF_THETA_BIN - the bin size of the direction theta histogram

  - DirectionOFF_PHI_MIN - the min value of the direction theta histogram
  - DirectionOFF_PHI_MAX - the max value of the direction theta histogram
  - DirectionOFF_PHI_BIN - the bin size of the direction theta histogram

  - h_Direction_Theta_OFF - histogram for the direction theta off
  - h_Direction_Phi_OFF - histogram for the direction theta off

  //IFF we add in position
  - PositionOFF_THETA_MIN - the min value of the direction theta histogram
  - PositionOFF_THETA_MAX - the max value of the direction theta histogram
  - PositionOFF_THETA_BIN - the bin size of the direction theta histogram

  - PositionOFF_PHI_MIN - the min value of the direction theta histogram
  - PositionOFF_PHI_MAX - the max value of the direction theta histogram
  - PositionOFF_PHI_BIN - the bin size of the direction theta histogram

  - h_Position_Theta_OFF - histogram for the direction theta off
  - h_Position_Phi_OFF - histogram for the direction theta off

Exit Conditions: none
Called By: R3_offsets
Date: 03-23-2014
Modified: 03-24-2014
*********************************************************/
void Define_Histograms()
{
  h_Direction_Theta_OFF.resize(NUMPACKAGES);
  h_Direction_Phi_OFF.resize(NUMPACKAGES);

  //loop pver packages
  for(int i=0; i< h_Direction_Theta_OFF.size(); i++)
  {
    h_Direction_Theta_OFF[i] = new TH1D (Form("h_Direction_Theta_OFF[%d]",i),Form("Direction Theta off "
      "for Package %d",i), DirectionOFF_THETA_BIN, DirectionOFF_THETA_MIN, DirectionOFF_THETA_MAX);
    h_Direction_Theta_OFF[i]->GetYaxis()->SetTitle("Frequency");
    h_Direction_Theta_OFF[i]->GetXaxis()->SetTitle("Theta (rad)");

    h_Direction_Phi_OFF[i] = new TH1D (Form("h_Direction_Phi_OFF[%d]",i),Form("Direction Phi off "
      "for Package %d",i), DirectionOFF_PHI_BIN, DirectionOFF_PHI_MIN, DirectionOFF_PHI_MAX);
    h_Direction_Phi_OFF[i]->GetYaxis()->SetTitle("Frequency");
    h_Direction_Phi_OFF[i]->GetXaxis()->SetTitle("Phi (rad)");
  }

  //uncomment and edit if want position - just an outline
  /***********************************
  h_Position_Theta_OFF.resize(NUMPACKAGES);
  h_Position_Phi_OFF.resize(NUMPACKAGES);
  for(int i=0; i< h_Position_Theta_OFF.size(); i++)
  {
    h_Position_Theta_OFF[i] = new TH1D (Form("h_Position_Theta_OFF[%d]",i),Form("Position Theta off "
      "for Package %d",i), PositionOFF_THETA_BIN, PositionOFF_THETA_MIN, PositionOFF_THETA_MAX);
    h_Position_Theta_OFF[i]->GetYaxis()->SetTitle("Frequency");
    h_Position_Theta_OFF[i]->GetXaxis()->SetTitle("Theta (degree)");

    h_Position_Phi_OFF[i] = new TH1D (Form("h_Position_Phi_OFF[%d]",i),Form("Position Phi off "
      "for Package %d",i), PositionOFF_PHI_BIN, PositionOFF_PHI_MIN, PositionOFF_PHI_MAX);
    h_Position_Phi_OFF[i]->GetYaxis()->SetTitle("Frequency");
    h_Position_Phi_OFF[i]->GetXaxis()->SetTitle("Phi (degree)");
  }
  ***********************************/

  return;
}


/***********************************************************
Function: Track_Loop
Purpose:
  To loop over the event_tree and fill in the histograms for the
  offset

Entry Conditions:
  TChain - event_tree
Global:
  - PREFIX - the PREFIX for the output file

  - NUMPACKAGES - number of packages

  - DirectionOFF_THETA_MIN - the min value of the direction theta histogram
  - DirectionOFF_THETA_MAX - the max value of the direction theta histogram
  - DirectionOFF_THETA_BIN - the bin size of the direction theta histogram

  - DirectionOFF_PHI_MIN - the min value of the direction theta histogram
  - DirectionOFF_PHI_MAX - the max value of the direction theta histogram
  - DirectionOFF_PHI_BIN - the bin size of the direction theta histogram

  - h_Direction_Theta_OFF - histogram for the direction theta off
  - h_Direction_Phi_OFF - histogram for the direction theta off

  //IFF we add in position
  - PositionOFF_THETA_MIN - the min value of the direction theta histogram
  - PositionOFF_THETA_MAX - the max value of the direction theta histogram
  - PositionOFF_THETA_BIN - the bin size of the direction theta histogram

  - PositionOFF_PHI_MIN - the min value of the direction theta histogram
  - PositionOFF_PHI_MAX - the max value of the direction theta histogram
  - PositionOFF_PHI_BIN - the bin size of the direction theta histogram

  - h_Position_Theta_OFF - histogram for the direction theta off
  - h_Position_Phi_OFF - histogram for the direction theta off

Exit Conditions: none
Called By: R3_offsets
Date: 02-25-2014
Modified: 02-26-2014
*********************************************************/
void Track_Loop (TChain * event_tree)
{


  /***********************************
  Got through the tree
  ***********************************/

  //get number of enties in the tree
  Int_t nentries = event_tree->GetEntries();

  //define and event pointer. the root files are built such that QwEvent stores everything
  QwEvent* fEvent = 0;

  //get the event branch of the event tree and link that to fEvent
  TBranch* event_branch=event_tree->GetBranch("events");
  event_branch->SetAddress(&fEvent);

  //loop over all the events in the tree
  for (int i = 0; i < nentries; i++)
  {
    //Get the ith entry form the event tree
    event_branch->GetEntry(i);

    /***********************************
    Start looping over the tracks of the tree
    ***********************************/

    //get the number of tracks
    Int_t nTracks = fEvent->GetNumberOfTracks();

    for(int j=0; j<nTracks; j++)
    {
      //get the ith tracks
      const QwTrack* tracks = fEvent->GetTrack(j);

      //fill histogram individual package histograms
      h_Direction_Theta_OFF[tracks->GetPackage()]->Fill(tracks->fDirectionThetaoff
        > M_PI ? tracks->fDirectionThetaoff - 2*M_PI : tracks->fDirectionThetaoff);
      //this fill is an if then statment if the theta is greater then
      //pi then suptract 2pi from it and fill it.  otherwise just
      //fill it with the theta value
      h_Direction_Phi_OFF[tracks->GetPackage()]->Fill(tracks->fDirectionPhioff
        > M_PI ? tracks->fDirectionPhioff - 2*M_PI : tracks->fDirectionPhioff);

      //fill histogram both packages combined histograms
      h_Direction_Theta_OFF[0]->Fill(tracks->fDirectionThetaoff
        > M_PI ? tracks->fDirectionThetaoff - 2*M_PI : tracks->fDirectionThetaoff);
      h_Direction_Phi_OFF[0]->Fill(tracks->fDirectionPhioff
        > M_PI ? tracks->fDirectionPhioff - 2*M_PI : tracks->fDirectionPhioff);

      //Postion example
      /***********************************
      //fill histogram individual package histograms
      h_Position_Theta_OFF[tracks->GetPackage()]->Fill(tracks->fPositionThetaoff
        > M_PI ? tracks->fPositionThetaoff - 2*M_PI : tracks->fPositionThetaoff);
      //this fill is an if then statment if the theta is greater then
      //pi then suptract 2pi from it and fill it.  otherwise just
      //fill it with the theta value
      h_Position_Phi_OFF[tracks->GetPackage()]->Fill(tracks->fPositionPhioff
        > M_PI ? tracks->fPositionPhioff - 2*M_PI : tracks->fPositionPhioff);

      //fill histogram both packages combined histograms
      h_Position_Theta_OFF[0]->Fill(tracks->fDirectionThetaoff
        > M_PI ? tracks->fPositionThetaoff - 2*M_PI : tracks->fPositionThetaoff);
      h_Position_Theta_OFF[0]->Fill(tracks->fPositionPhioff
        > M_PI ? tracks->fPositionPhioff - 2*M_PI : tracks->fPositionPhioff);
      ***********************************/
    }
  }

  return;
}

/***********************************************************
Function: Plot
Purpose:
  To print out the histograms to canvases and save it

  They y-axis is in log scale

Entry Conditions:
  none

Global:
  - PREFIX - the PREFIX for the output file

  - h_Direction_Theta_OFF - histogram for the direction theta off
  - h_Direction_Phi_OFF - histogram for the direction phi off

  - NUMPACKAGES - number of packages
  - NUMDIRECTIONOFF - number of directions

  - INDEXTODirectionOFF - array to take index of canvas vector and
    relates it to a histogram

  //IFF we add in position
  - h_Position_Theta_OFF - histogram for the direction theta of
  - h_Position_Phi_OFF - histogram for the direction theta off

  - NUMPOSITIONOFF - number of postions

  - INDEXTOPositionOFF - array to take index of canvas vector and
    relates it to a histogram

Exit Conditions: none
Called By: R3_offsets
Date: 02-25-2014
Modified: 02-26-2014
*********************************************************/
void Plot()
{
  /***********************************
  Define a histogram which will be used as a temporay
  holder of the histograms that will be drawen
  thia allows for using varable in the name of a
  histogram :)
  ***********************************/
  TH1D* h_Temp;

  /***********************************
  Define my canvases, Draw and save
  ***********************************/

  //Canvas
  std::vector <TCanvas*> c_DirectionOff; //[direction type]

  c_DirectionOff.resize(NUMDIRECTIONOFF);

  for (int i=0; i< c_DirectionOff.size(); i++)
  {

    switch(i)
    {
      case 0:
        break;

      case 1:
        break;

      case 2:
        break;

      case 3:
      case 4:

        //name divide canvases
        c_DirectionOff[i] = new TCanvas (Form("c_Direction[%d]",i),
          Form("%s offset",INDEXTODirectionOFF[i].c_str()), 900,300);

        c_DirectionOff[i]->Divide(3,0);

        //loop over packages and draw
        for(int j=0; j<NUMPACKAGES; j++)
        {
          c_DirectionOff[i]->cd(j+1);
          TPad* Pad = (TPad *)(c_DirectionOff[i]->cd(j+1));
          Pad->SetLogy();
          //fill the histogram that I want to graph into the h_Temp one
          //so that one can use the variables and make this awsome

          //testing
          //gDirectory->Print();
          h_Temp = (TH1D*)gDirectory->Get(Form("h_Direction_%s_OFF[%d]",INDEXTODirectionOFF[i].c_str(),j));
          h_Temp->Draw();
        }

        //save canvases
        c_DirectionOff[i]->SaveAs(PREFIX+Form("%s_offset.png",INDEXTODirectionOFF[i].c_str()));
        c_DirectionOff[i]->SaveAs(PREFIX+Form("%s_offset.C",INDEXTODirectionOFF[i].c_str()));

        break;

      default:
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
  run_num - the run number

Global:
  - PREFIX - the PREFIX for the output file

  - h_Direction_Theta_OFF - histogram for the direction theta off
  - h_Direction_Phi_OFF - histogram for the direction phi off

  - NUMPACKAGES - number of packages
  - NUMDIRECTIONS - number of directions

  - INDEXTODirection - array to take index of canvas vector and
    relates it to a histogram

  //IFF we add in position
  - h_Position_Theta_OFF - histogram for the direction theta of
  - h_Position_Phi_OFF - histogram for the direction theta off

  - NUMPOSITIONOFF - number of postions

  - INDEXTOPositionOFF - array to take index of canvas vector and
    relates it to a histogram

Exit Conditions: none
Called By: R3_offsets
Date: 02-26-2014
Modified: 02-27-2014
*********************************************************/
void Print_To_File(int run_num)
{
  /***********************************
  Define a histogram which will be used as a temporay
  holder of the histograms that will be drawen
  thia allows for using varable in the name of a
  histogram :)
  ***********************************/
  TH1D* h_Temp;

  //print to file
  //this file and evrything related to it is fout
  std::ofstream fout;
  //open file
  // open file with PREFIX+R3_offsets.txt which will
  // store the output of the vlaues to a file in a easy way
  //that should be able to be read back into a program if needed

  for(int j=0; j< NUMDIRECTIONOFF; j++)
  {
    switch(j)
    {
      case 0:
        break;

      case 1:
        break;

      case 2:
        break;

      case 3:
      case 4:
        fout.open(PREFIX+Form("R3_offsets_%s.txt",INDEXTODirectionOFF[j].c_str()));
        if (!fout.is_open()) cout << "File not opened" << endl;
        //Prefix will inculed run number which we need.

        //Name what each coulmn is.
        //Note direction is 0 - all planes, 1 - the X planes, 3 - U planes, 4 - V planes
        //Error is the RMS.sqrt(N)
        fout << "Run \t pkg \t DIR \t Value \t Error" << endl; // add if nee position info \t POS \t Value \t Error" <<endl;

        for (int i=1; i<NUMPACKAGES; i++)
        {
          h_Temp = (TH1D*)gDirectory->Get(Form("h_Direction_%s_OFF[%d]",INDEXTODirectionOFF[j].c_str(),i));

          fout << run_num << " \t " << i << " \t " << INDEXTODirectionOFF[j].c_str() << " \t " <<
             value_with_error(h_Temp->GetMean(),(h_Temp->GetRMS()/sqrt(h_Temp->GetEntries()))) << endl;
        }

        //close the file
        fout.close();
        break;

      default:
        break;
    }
  }

  return;
}
