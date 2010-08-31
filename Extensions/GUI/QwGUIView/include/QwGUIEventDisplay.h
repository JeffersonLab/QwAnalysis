/**
   \file QwGUIEventDisplay.h
   \author Wouter Deconinck <wdconinc@mit.edu>
   \Updated by Derek Jones (dwjones8@gwu.edu)
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

////SYSTEM HEADERS--------------------------------------------------////
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "TMath.h"

#include "QwGUISubSystem.h"
#include "RSDataWindow.h"

////ROOT CORE HEADERS--------------------------------------------------////
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_RQ_OBJECT
#include "RQ_OBJECT.h"
#endif
#ifndef ROOT_TGClient
#include "TGClient.h"
#endif
#ifndef ROOT_TF1
#include <TF1.h>
#endif
#ifndef ROOT_TRandom
#include <TRandom.h>
#endif
#ifndef ROOT_TFile
#include <TFile.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif
#ifndef ROOT_TLine
#include <TLine.h>
#endif
#ifndef ROOT_TPaveLabel
#include <TPaveLabel.h>
#endif
#ifndef ROOT_TPaveText
#include <TPaveText.h>
#endif
#ifndef ROOT_TPolyMarker
#include <TPolyMarker.h>
#endif
#ifndef ROOT_TGuiBldHintsButton
#include "TGuiBldHintsButton.h"
#endif
#ifndef ROOT_TRootBrowserLite
#include "TRootBrowserLite.h"
#endif
#ifndef ROOT_TImage
#include "TImage.h"
#endif


////ROOT GUI HEADERS--------------------------------------------------////
#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGFontDialog
#include "TGFontDialog.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif


////C++ STANDARD HEADERS--------------------------------------------------////
#include "Riostream.h"
#include "TMath.h"


////FORWARD DECLARATIONS--------------------------------------------------////
class QwEventBuffer;
class QwHitContainer;
class QwSubsystemArrayTracking;
class QwTreeEventBuffer;
class QwHitRootContainer;


////DECLARE CLASS ELEMENTS--------------------------------------------------////
class QwGUIEventDisplay : public QwGUISubSystem {  // Copied from QwEventDisplay.h 7/26/2010

  // REGION GEOMETRIES
  // All length measurements in cm; measured on inside of a single chamber frame
  // All angle measurements in degrees (converted to radians later)
  // Scalings make longest side of length .8 in canvas (generally the width)
  // Geometry data taken from QweakSim files unless noted in parentheses with a derivation;
  // Length is x, width is y, depth is z in qweak coordinates (x and y flip for GUI use, be careful of distinction)
#define R1_WIDTH 30.00     // Region 1
#define R1_LENGTH 30.00
#define R1_DEPTH 3.00
#define R1_DIST 0.10       // Distance between traces        ###CHECK THIS!!!
#define R1_CM .8/R1_WIDTH  // Drawing scale factor, 1 cm in ROOT coordinates for this region

#define R2_WIDTH 45.09     // Region 2
#define R2_LENGTH 30.00
#define R2_DEPTH 3.35
#define R2_DIST 1.27       // Distance between drift cell planes
#define R2_XDIST 0.909     // Distance between x wires
#define R2_UVDIST 2.25     // Distance between u (or v) wires on bottom of frame
#define R2_FULLWIRE1 9     // 9.29  // (30*.675/2.18)         ###CHECK THIS!!!
#define R2_FULLWIRE2 20    // 20.68 // (45.09/2.18)
#define R2_ANGLE 56.00     // Wire angle from bottom of frame
#define R2_CM .8/R2_WIDTH  // Drawing scale factor, 1 cm in ROOT coordinates for this region

#define R3_WIDTH 204.47    // Region 3
#define R3_LENGTH 53.34
#define R3_DEPTH 2.54
#define R3_DIST 40.00      // Distance between drift cell chambers
#define R3_UVDIST 1.11     // Distance between u (or v) wires on bottom of frame
#define R3_FULLWIRE1 96    // Given in geometry data
#define R3_FULLWIRE2 184   // Given in geometry data
#define R3_ANGLE 26.57     // Wire angle from normal to bottom of frame (90 - 63.43)
#define R3_CM .8/R3_WIDTH  // Drawing scale factor, 1 cm in ROOT coordinates for this region

 public:
  RQ_OBJECT("QwGUIEventDisplay") // Create an object for ROOT to process

  UInt_t fEventNumber; // Keeps track of the current event number on display
  UInt_t fTreeEntries; // Counts the number of events in the file hits tree

 private:
  TGVerticalFrame *fMain; // Mainframe window

  TGHorizontalFrame *fEventBoxes; // Event box frame
  TGTab *fRegions; // Tab frame for region boxes
  TGTextButton *ExitButton;

  // Menu Bar
  TGMenuBar *fMenuBar;                                 // Menu bar
  TGPopupMenu *fMenuHelp;                              // Popup menus
  TGLayoutHints *fMenuBarLayout, *fMenuBarHelpLayout;  // Menu bar layouts
  enum MenuCommandIdentifiers {
    M_HELP_TUTORIAL, // Provides link to the event display tutorial documentation
    M_HELP_ABOUT     // Print information about the event display
  };
  
  // Event Box 1 (Current Event Number) Declarations
  TGVerticalFrame *fEventBox1, *fEventCounter; // Event box 1 frame and internal frame
  TGLabel *fCurrentEventLabel;                 // Label that displays the current event number
  TGNumberEntry *fCurrentEventEntry;           // Number entry for GotoEvent() function
  TGGroupFrame *fCurrentEvent;                 // Group frame to make label more prominent
  
  // Event Box 2 (Wire Hit Information) Declarations
  TGVerticalFrame *fEventBox2; // Event box 2 frame
  TGListBox *fWireInfoListBox; // List box to display wire hit information

  // Event Box 3 (Octant Identification) declarations
  TGVerticalFrame *fEventBox3;    // Event box 3 frame
  TRootEmbeddedCanvas *fOctantID; // Embedded canvas for octant ID
  TCanvas *cOctantID;             // Canvas for octant ID
  TPaveLabel *Octant_1, *Octant_2, *Octant_3, *Octant_4, *Octant_5, *Octant_6, *Octant_7, *Octant_8; // Labels for each of the octants

  // Region 1 (GEM) Declarations
  TGCompositeFrame *fRegion1;                                // Tab frame
  TRootEmbeddedCanvas *fRegion1XY, *fRegion1XZ, *fRegion1YZ; // Embedded canvases for each projection
  TCanvas *cR1XY, *cR1XZ, *cR1YZ;                            // Canvases for each projection
  TPaveLabel *Label_R1XY, *Label_R1XZ, *Label_R1YZ;          // Labels for projection descriptions
  TBox *Box_R1XY, *Box_R1XZ, *Box_R1YZ;                      // Boxes for each chamber
  std::vector<TLine> Line_R1r;                               // Vectors to store hit list from hit containers
  std::vector<TLine> Line_R1y;                               // for each direction and draw wire hit lines

  // Region 2 (HDC) Declarations
  TGCompositeFrame *fRegion2;                                // Tab frame
  TRootEmbeddedCanvas *fRegion2XY, *fRegion2XZ, *fRegion2YZ; // Embedded canvases for each projection
  TCanvas *cR2XY, *cR2XZ, *cR2YZ;                            // Canvases for each projection
  TPaveLabel *Label_R2XY, *Label_R2XZ, *Label_R2YZ;          // Labels for projection descriptions
  TBox *Box_R2XY, *Box_R2XZ1, *Box_R2XZ2,*Box_R2XZ3,*Box_R2XZ4, *Box_R2YZ1, *Box_R2YZ2, *Box_R2YZ3, *Box_R2YZ4;    // Boxes for each chamber
  TText *tR2XY1, *tR2XY2, *tR2XY3, *tR2XY4, *tR2XZ1, *tR2XZ2, *tR2XZ3, *tR2XZ4,*tR2YZ1, *tR2YZ2, *tR2YZ3, *tR2YZ4; // Text for HDC chamber labels
  std::vector<TLine> Line_R2x;                               // Vectors to store hit list from hit containers
  std::vector<TLine> Line_R2u;                               // for each direction and draw wire hit lines
  std::vector<TLine> Line_R2v;

  // Region 2b (HDC second arm) Declarations
  TGCompositeFrame *fRegion2b;                                  // Tab frame
  TRootEmbeddedCanvas *fRegion2bXY, *fRegion2bXZ, *fRegion2bYZ; // Embedded canvases for each projection
  TCanvas *cR2bXY, *cR2bXZ, *cR2bYZ;                            // Canvases for each projection
  TPaveLabel *Label_R2bXY, *Label_R2bXZ, *Label_R2bYZ;          // Labels for projection descriptions
  TBox *Box_R2bXY, *Box_R2bXZ1, *Box_R2bXZ2,*Box_R2bXZ3,*Box_R2bXZ4, *Box_R2bYZ1, *Box_R2bYZ2, *Box_R2bYZ3, *Box_R2bYZ4;       // Boxes for each chamber
  TText *tR2bXY1, *tR2bXY2, *tR2bXY3, *tR2bXY4, *tR2bXZ1, *tR2bXZ2, *tR2bXZ3, *tR2bXZ4,*tR2bYZ1, *tR2bYZ2, *tR2bYZ3, *tR2bYZ4; // Text for HDC chamber labels

  // Region 3 (VDC) Declarations
  TGCompositeFrame *fRegion3;                                // Tab frame
  TRootEmbeddedCanvas *fRegion3XY, *fRegion3XZ, *fRegion3YZ; // Embedded canvases for each projection
  TCanvas *cR3XY, *cR3XZ, *cR3YZ;                            // Canvases for each projection
  TPaveLabel *Label_R3XY, *Label_R3XZ, *Label_R3YZ;          // Labels for projection descriptions
  TBox *Box_R3XY1, *Box_R3XY2,*Box_R3XY3,*Box_R3XY4, *Box_R3XZ1, *Box_R3XZ2,*Box_R3XZ3,*Box_R3XZ4, *Box_R3YZ1, *Box_R3YZ2, *Box_R3YZ3, *Box_R3YZ4; // Boxes for each chamber
  TText *tR3XY1, *tR3XY2, *tR3XY3, *tR3XY4, *tR3XZ1, *tR3XZ2, *tR3XZ3, *tR3XZ4,*tR3YZ1, *tR3YZ2, *tR3YZ3, *tR3YZ4;                                 // Text for VDC chamber labels
  TLine *Div_R3XY, *Div_R3XZ, *Div_R3YZ; // Dividers for arm views in projections
  std::vector<TLine> Line_R3u;           // Vectors to store hit list from hit containers
  std::vector<TLine> Line_R3v;           // for each direction and draw wire hit lines

  // Region 3 (TS) Declarations // NOT USED CURRENTLY
  TGCompositeFrame *fRegion3TS; // Tab frame

  // Create Event Buffer
  QwTreeEventBuffer* fEventBuffer; //!       // Excluded from dictionary
  QwSubsystemArrayTracking* fSubsystemArray; //!

  // Create Hit List
  QwHitContainer* fHitList; //!
  QwHitRootContainer* fHitRootList; //!

  // Trees
  TTree* fTree; // Used to store hits_tree from ROOT file

 public:
  // Function List
  void HandleMenu(Int_t id);
  void GoPrevious();
  void GoNext();
  void GoClear();
  void GotoEvent();
  int DrawEvent();

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
  virtual void        OnNewDataContainer(RDataContainer *cont);
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUIEventDisplay,0);

}; // class QwGUIEventDisplay

#endif // QWGUIEVENTDISPLAY_H
