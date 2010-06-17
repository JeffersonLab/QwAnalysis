//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIDatabase.h
   \author Michael Gericke
   \author Buddhini Waidyawansa
   \author Damon Spayde
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIDatabase
    
   \brief Handles the display of database summary table data.

   The QwGUIHallCBeamline class handles the display of the hallc beamline data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 
Added by Buddhini to display the hall c Beamline data.

 */
//=============================================================================

#define N_DETECTORS     33
#define N_SUBBLOCKS     5
#define N_DET_MEAS_TYPES 2

///
/// \ingroup QwGUIDatabase

#ifndef QWGUIDATABASE_H
#define QWGUIDATABASE_H


#include <cstdlib>
#include <cstdio>
#include <vector>
using std::vector;

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
#ifndef ROOTCINTMODE
#include "QwSSQLS_summary.h"
#endif
#include <TVectorT.h>
#include <TGraphErrors.h>


 class QwGUIDatabase : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dCmbLayout;
  TGLayoutHints       *dNumLayout;
  TGLayoutHints       *dBtnLayout;
  TGLayoutHints       *dLabLayout;
  TGNumberEntry       *dNumStartRun;
  TGNumberEntry       *dNumStopRun;
  TGComboBox          *dCmbDetector;
  TGComboBox          *dCmbSubblock;
  TGComboBox          *dCmbMeasurementType;
  TGTextButton        *dBtnSubmit;
  TGLabel             *dLabStartRun;
  TGLabel             *dLabStopRun;
  

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            GraphArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //!A histogram array to plot the X and Y position difference variation.
  TPaveText * errlabel;

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

  static const char   *DetectorCombos[N_DETECTORS];
  static const char   *DetectorMeasurementTypes[N_DET_MEAS_TYPES];
  static const char   *Subblocks[N_SUBBLOCKS];


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
  void                 PlotGraphs();



 public:
  
  QwGUIDatabase(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		     const char *objName, const char *mainname, UInt_t w, UInt_t h);

  ~QwGUIDatabase();


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
  
  ClassDef(QwGUIDatabase,0); 

};

#endif
