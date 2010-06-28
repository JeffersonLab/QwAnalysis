//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUILumiDetector.h
   \author Michael Gericke
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUILumiDetector
    
   \brief Handles the display of the lumi detector data.  

   The QwGUILumiDetector class handles the display of the lumi detector data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector.  John Leacock 
   modified it to work with the lumis.
    
 */
//=============================================================================

#define USLUMI_DET_HST_NUM      8
#define DSLUMI_DET_HST_NUM      8   
        

///
/// \ingroup QwGUILumi

#ifndef QWGUILUMIDETECTOR_H
#define QWGUILUMIDETECTOR_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <TRootEmbeddedCanvas.h>
#include <TRootCanvas.h>
#include "QwGUISubSystem.h"
#include "RSDataWindow.h"

class QwGUILumiDetector : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dBtnLayout;

  TGTextButton        *dButtonDownstream;
  TGTextButton        *dButtonUpstream;
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
  //void                 PlotData();
  void ShowUpstream();
  void ShowDownstream();


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
  static const char   *RootTrees[2];
  static const char   *USLumiDetectorHists[USLUMI_DET_HST_NUM];
  static const char   *DSLumiDetectorHists[DSLUMI_DET_HST_NUM];

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
  
  QwGUILumiDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUILumiDetector();


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

  ClassDef(QwGUILumiDetector,0);

};

#endif
