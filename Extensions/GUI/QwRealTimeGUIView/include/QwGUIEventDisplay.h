/**
   \file QwGUIEventDisplay.h
   \author Wouter Deconinck <wdconinc@mit.edu>
*/


/**
   \class QwGUIEventDisplay
   \ingroup QwGUIMain

   \brief Handles the display of the lumi detector data.

   The QwGUIEventDisplay class handles the display of the tracking events.

 */
//=============================================================================

#ifndef QWGUIEVENTDISPLAY_H
#define QWGUIEVENTDISPLAY_H

#include "QwGUISubSystem.h"



class QwGUIEventDisplay : public QwGUISubSystem {

  // Event list frane
  TGHorizontalFrame   *dEventListFrame;
  TGLayoutHints       *dEventListFrameLayout;
  TGVerticalFrame     *dEventListControlFrame;
  TGLayoutHints       *dEventListControlLayout;
  TGNumberEntry       *dEventListNumberEntry;
  TGHorizontalFrame   *dEventListButtonsFrame;
  TGLayoutHints       *dEventListButtonsLayout;
  TGTextButton        *dEventListButtonPrevious;
  TGTextButton        *dEventListButtonNext;
  TGListBox           *dEventListBox;
  TGLayoutHints       *dEventListBoxLayout;

  // Event display tabs
  TGHorizontalFrame   *dEventDisplayFrame;
  TGLayoutHints       *dEventDisplayFrameLayout;
  TGTab               *dEventDisplayTabs;
  TGLayoutHints       *dEventDisplayTabsLayout;
  // All regions
  TGLayoutHints       *dEventDisplayTabRegionLayout;
  // Region 1 tab
  TGCompositeFrame    *dEventDisplayTabRegion1;
  TRootEmbeddedCanvas *dEventDisplayTabRegion1_XY;
  TRootEmbeddedCanvas *dEventDisplayTabRegion1_XZ;
  // Region 2 tab
  TGCompositeFrame    *dEventDisplayTabRegion2;
  TRootEmbeddedCanvas *dEventDisplayTabRegion2_XY;
  TRootEmbeddedCanvas *dEventDisplayTabRegion2_XZ;
  // Region 3 tab
  TGCompositeFrame    *dEventDisplayTabRegion3;
  TRootEmbeddedCanvas *dEventDisplayTabRegion3_XY;
  TRootEmbeddedCanvas *dEventDisplayTabRegion3_XZ;

  // Trees
  TTree* fTree;
  Int_t fCurrentEvent;

  // Plot the next event
  virtual void PlotNext();

  // Plot the previous event
  virtual void PlotPrevious();

  //!This function just plots some histograms in the main canvas, just for illustrative purposes
  //!for now.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotData();

  //!This function clear the histograms/plots in the plot container. This is done everytime a new
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  /* void                 ClearData(); */

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

  QwGUIEventDisplay(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUIEventDisplay();


  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  /* virtual void        OnNewDataContainer(); */
  /* virtual void        OnObjClose(char *); */
  virtual void        OnReceiveMessage(char*);
  /* virtual void        OnRemoveThisTab(); */

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  /* virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject); */

  ClassDef(QwGUIEventDisplay,0);

}; // class QwGUIEventDisplay

#endif // QWGUIEVENTDISPLAY_H
