//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIHallCBeamline.h
   \author Michael Gericke
   \author Buddhini Waidyawansa
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIHallCBeamline
    
   \brief Handles the display of the hall c beamline data.  

   The QwGUIHallCBeamline class handles the display of the hallc beamline data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 
Added by Buddhini to display the hall c Beamline data.

 */
//=============================================================================

#define HCLINE_BPMS   24 // There are 23 BPMs in the hallC beamline+the virtual target bpm
#define HCLINE_BCMS   4     
#define NUM_TREE      3
///
/// \ingroup QwGUIHallCBeamline

#ifndef QWGUIHALLCBEAMLINE_H
#define QWGUIHALLCBEAMLINE_H


#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <string>
#include <TPaveText.h>
#include <TGButtonGroup.h>

#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "TPaletteAxis.h"
#include "QwGUISubSystem.h"
#include "TStyle.h"
#include "RSDataWindow.h"


 class QwGUIHallCBeamline : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TGButtonGroup       *correlations;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dBtnPosDiff;
  TGTextButton        *dBtnTgtParam;
  TGTextButton        *dBtnCorrelations;
  TGTextButton        *dBtnPlotHistos;
  TGTextButton        *dBtnRaster;
  TGTextButton        *dBtnAqDiff;
  TGComboBox          *dCmbHistos;
  TGLayoutHints       *dCmbLayout;

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //!A histogram array to plot the X and Y position difference variation.
  TH1D *PosDiffVar[2] ;
  TPaveText * errlabel;
  TH2D* fRateMap;

  //!This function just plots some histograms in the main canvas, just for illustrative purposes
  //!for now.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  // void                 PlotData();

  //  void PlotPosData();


/*   void PlotChargeData(); */


  void PositionDifferences(); 
  void DisplayTargetParameters();
  void PlotHistograms();
  void FastRaster();
  void Correlations(TString axis1, TString axis2);
  void BCMDoubleDifference();

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

  static const char   *HallC_BPMS[HCLINE_BPMS];
  static const char   *HallC_BCMS[HCLINE_BCMS]; //The Devices with only charge may later have to be distinguished from those with other data
  static const char   *RootTrees[NUM_TREE];
  
 private:

  char histo[128];
  TString select;
  Bool_t parity_off;
  Bool_t  ReadyHistArray;
  void EnableButtons(Bool_t flag);

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

  void                 SummaryHist(TH1*in);



 public:
  
  QwGUIHallCBeamline(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		     const char *objName, const char *mainname, UInt_t w, UInt_t h);

  ~QwGUIHallCBeamline();


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
  
  ClassDef(QwGUIHallCBeamline,0); 

};

#endif
