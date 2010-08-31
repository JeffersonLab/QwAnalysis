//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUITrackFinding.h
   \author Michael Gericke
   \author Buddhini Waidyawansa
   \author Wouter Deconinck
   \author David Zou

*/
//=============================================================================
//
//=============================================================================
//
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUITrackFinding

   \brief Handles the display of the track finding quality.

   The QwGUITrackFinding class handles the display of the track finding quality.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector.
   Modified by Buddhini Waidyawansa to display the hall c Beamline data.
   Modified by Wouter Deconinck to display track finding quality
   Modified by David Zou to display additional track finding qualities
 */
//=============================================================================

#define CHAMBERS_NUM             2
#define TREE_NUM                 1

///
/// \ingroup QwGUITrackFinding

#ifndef QWGUITRACKFINDING_H
#define QWGUITRACKFINDING_H


#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <string>
#include <TPaveText.h>

#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "QwGUISubSystem.h"
#include "TStyle.h"
#include "RSDataWindow.h"


 class QwGUITrackFinding : public QwGUISubSystem {


  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TGVerticalFrame     *dControlsFrame1; //added
  TRootEmbeddedCanvas *dCanvas;
  TGLayoutHints       *dTabLayout;
  TGLayoutHints       *dCnvLayout;
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dBtnChi2;
  TGTextButton        *dBtnResidual;
  TGTextButton        *dBtnOffset;
  TGTextButton        *dBtnSlope;
  TGTextButton        *dBtnpChi2;
  TGTextButton        *dBtnpResidual;
  TGTextButton        *dBtnpOffset;
  TGTextButton        *dBtnpSlope;
  TGTextButton        *dBtnPackage;
// objects needed for drop down menu
  TGVerticalFrame        *dBigFrame;
  TGVerticalFrame        *dTabFrameVert;
  TGMenuBar              *dMenuBar;
  TGPopupMenu            *dMenuData;
  TGPopupMenu            *dMenuPlot1;
  TGPopupMenu            *dMenuPlot2;
  TGLayoutHints          *dMenuBarLayout; 
  TGLayoutHints          *dMenuBarItemLayout;
  TRootEmbeddedCanvas    *dCanvasVert;

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;

  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //! A set of histograms with chi2 distribution
  TH1D *histoChi2[2];
  TPaveText * errlabel;

  std::vector<const char*> fRootTrees;
  std::vector<const char*> fDriftChambers;

  //!This function just plots some histograms in the main canvas, just for illustrative purposes
  //!for now.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void PlotChi2();
  void PlotResidual();
  void PlotOffset();
  void PlotSlope();
  void PlotpChi2();
  void PlotpResidual();
  void PlotpOffset();
  void PlotpSlope();
  void PlotLinear();
  void PlotLog();
  void PlotTLSlopeX();
  void PlotTLSlopeU();
  void PlotTLSlopeV();
  void PlotTLChi2X();
  void PlotTLChi2U();
  void PlotTLChi2V();
  void PlotTLOffsetX();
  void PlotTLOffsetU();
  void PlotTLOffsetV();
  void PlotTLResidualX();
  void PlotTLResidualU();
  void PlotTLResidualV();
  void PlotPTPackage();

  //!This function plots histograms of the data in the current file, in the main canvas.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotHistograms();

  //!This function clear the histograms/plots in the plot container. This is done everytime a new
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void  ClearData();

  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.

 protected:

  //!Overwritten virtual function from QwGUISubSystem::MakeLayout(). This function simply adds an
  //!embeded canvas to the tab frame for this subsystem, but almost anything can be done here ...
  //!See QwGUISubSystem::MakeLayout() for more information.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  virtual void         MakeLayout();

 public:

  QwGUITrackFinding(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		     const char *objName, const char *mainname, UInt_t w, UInt_t h);

  ~QwGUITrackFinding();


  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  virtual void        OnNewDataContainer(RDataContainer *cont);
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUITrackFinding,0);

};

#endif
