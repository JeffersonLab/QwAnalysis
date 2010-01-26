//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIInjector.h
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
   \class QwGUIInjector
    
   \brief Handles the display of the lumi detector data.  

   The QwGUIInjector class handles the display of the lumi detector data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 
Added by Buddhini to display the injector beamline data.

 */
//=============================================================================

#define INJECTOR_BPM_NUM          6 
#define INJECTOR_BCM_NUM          7        
#define INJECTOR_DET_TRE_NUM      2
///
/// \ingroup QwGUIInjector

#ifndef QWGUIINJECTOR_H
#define QWGUIINJECTOR_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <TRootEmbeddedCanvas.h>
#include <TRootCanvas.h>
#include "QwGUISubSystem.h"
#include "RSDataWindow.h"
#include "TSuperCanvas.h"

class QwGUIInjector : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dSubFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dButtonPos;
  TGTextButton        *dButtonCharge;

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;

  //!This function just plots some histograms in the main canvas, just for illustrative purposes
  //!for now.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  // void                 PlotData();

  void                 PlotPosData();


  void                 PlotChargeData();

  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearData();

  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.

  static const char   *InjectorBPM[INJECTOR_BPM_NUM];
  static const char   *InjectorBCM[INJECTOR_BCM_NUM];
  static const char   *InjectorTrees[INJECTOR_DET_TRE_NUM];

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
  
  QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUIInjector();


  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  virtual void        OnNewDataContainer();
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUIInjector,0);
};

#endif
