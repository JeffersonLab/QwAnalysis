//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIBeamModulation.h
   \author Michael Gericke
   \author Buddhini Waidyawansa
   \author Nuruzzaman
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIBeamModulation
    
   \brief Handles the display of the Beam Modulation system in the Hall-C beamline data.  

   The QwGUIBeamModulation class handles the display of the hallc beamline data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code modified by Buddhini for the Hallcbeamline class. 
Added by Nuruzzaman to display the Beam Modulation data.

 */
//=============================================================================

#define BEAMMODULATION_DEV_NUM   20
//#define BEAMMODULATION_TRE_NUM  2       
#define TRE_NUM                   2
//#define BEAMMODULATION_DEV_NUM  5
///
/// \ingroup QwGUIBeamModulation

#ifndef QWGUIBEAMMODULATION_H
#define QWGUIBEAMMODULATION_H


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
#include "TFrame.h"


 class QwGUIBeamModulation : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dBtnBPMResp; 

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            TreeArray;
  TObjArray            PlotArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //!A histogram array to plot the X and Y position difference variation.
  TH1D *PosDiffVar[2] ;
  TPaveText * errlabel;

  //!This function just plots some histograms in the main canvas, just for illustrative purposes
  //!for now.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  // void                 PlotData();

  void PlotBPMResponse(void);



  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void  ClearData();

  void                 SetSelectedDataWindow(Int_t ind) {dSelectedDataWindow = ind;};
  void                 RemoveSelectedDataWindow() {dSelectedDataWindow = -1;};
  QwGUIDataWindow     *GetSelectedDataWindow();  

  Int_t                dSelectedDataWindow;


  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.

  static const char   *BeamModulationDevices[BEAMMODULATION_DEV_NUM];
  static const char   *RootTrees[TRE_NUM];
  //  static const char   *BeamModulationTrees[BEAMMODULATION_TRE_NUM];
  //  static const char   *BeamModulationDevices2[BEAMMODULATION_DEV_NUM];

  void                 CleanUp();

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
  
  QwGUIBeamModulation(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		     const char *objName, const char *mainname, UInt_t w, UInt_t h);

  ~QwGUIBeamModulation();


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

  virtual void        MakeHCLogEntry(); 
  
  ClassDef(QwGUIBeamModulation,0); 

};

#endif
