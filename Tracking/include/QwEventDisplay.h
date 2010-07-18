#ifndef QWEVENTDISPLAY_H
#define QWEVENTDISPLAY_H


/************************************\
** Qweak 2D Event Display Header    **
** Jefferson Lab -- Hall C          **
** Author: Derek Jones              **
** The George Washington University **
** Contact: dwjones8@gwu,edu        **
\************************************/


////ROOT HEADERS////

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
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
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
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TImage
#include "TImage.h"
#endif

#include "TMath.h"

////C++ STANDARD////
#include "Riostream.h"


////FORWARD DECLARATIONS////

class QwEventBuffer;
class QwHitContainer;
class QwSubsystemArrayTracking;
class QwTreeEventBuffer;


////DECLARE CLASS ELEMENTS////

class QwEventDisplay: public TGMainFrame{  // display is a ROOT main frame class

// REGION GEOMETRIES

//All length measurements in cm
//All angle measurements in degrees
//Scalings make longest side of length .8 in canvas
//Geometry data taken from QweakSim files unless noted in parentheses with a derivation;
//Length is x, width is y, depth is z in qweak coordinates (x and y flip for GUI use, be careful of distinction)
#define R1_WIDTH 30.00 // Region 1
#define R1_LENGTH 30.00
#define R1_DEPTH 3.00
#define R1_DIST 0.10 // distance between traces CHECK THIS!!!
#define R1_CM  0.0267 //calibrated value of 1 cm in this scale

#define R2_WIDTH 45.09 // Region 2
#define R2_LENGTH 30.00
#define R2_DEPTH 3.35 //single chamber; 17.20 total depth of HDC system
#define R2_DIST 1.27 // distance between drift cell planes
#define R2_XDIST 0.909 // distance between x wires
#define R2_UVDIST 2.25 // distance between u (or v) wires
#define R2_CM 0.0177
#define R2_ANGLE 56.00 

#define R3_WIDTH 210.00 // Region 3
#define R3_LENGTH 50.00
#define R3_DEPTH 2.54
#define R3_DIST 40.00 // distance between drift cell chambers
#define R3_UVDIST 0.839 // distance between u (or v) wires
#define R3_CM 0.0038
#define R3_ANGLE 63.43 // 90-26.57 (which is the wire angle)

 public:
  RQ_OBJECT("QwEventDisplay")

  UInt_t fEventNumber; // keeps track of the current event number on display

 private:  // Frames
  TGMainFrame *fMain; // mainframe window

  TGHorizontalFrame *fEventBoxes; // event box frame
  TGTab *fRegions; // tab frame for region data
  TGTextButton *ExitButton;

  // Menu bar
  TGMenuBar *fMenuBar;
  TGPopupMenu *fMenuFile, *fMenuHelp;
  TGLayoutHints *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;
  enum MenuCommandIdentifiers {
    M_FILE_OPENRUN,
    M_FILE_OPENSIM,
    M_FILE_PRINTSCREEN,
    M_FILE_CLOSE,
    
    M_HELP_TUTORIAL,
    M_HELP_ABOUT
  };

  // Event Box 1 declarations
  TGVerticalFrame *fEventBox1, *fEventCounter;
  TGLabel *CurrentEventLabel;
  TGNumberEntry *CurrentEventEntry;
  TGGroupFrame *fCurrentEvent;

  // Event Box 2 declarations
  TGVerticalFrame *fEventBox2;
  TGListBox *WireHitListBox;

  //Event Box 3 declarations
  TGListBox *TimingListBox;

  //Event Box 4 delcarations
  TGListBox *DriftDistanceListBox;

  //  UInt_t sum; // sum of wire numbers  NOT USED FOR NOW
  //  double num; // number of wire hits
  //  double fit; // average wire number

  //Region 1 (GEM) declarations
  TGCompositeFrame *fRegion1;
  TRootEmbeddedCanvas *fRegion1XY, *fRegion1XZ, *fRegion1YZ;
  TCanvas *cR1XY, *cR1XZ, *cR1YZ;
  TPaveLabel *Label_R1XY, *Label_R1XZ, *Label_R1YZ;
  TBox *Box_R1XY, *Box_R1XZ, *Box_R1YZ;
  std::vector<TLine> Line_R1r;
  std::vector<TLine> Line_R1y;
  std::vector<TLine> R1_XYfit;
  std::vector<TLine> R1_XZfit;

  //Region 2 (HDC) declarations
  TGCompositeFrame *fRegion2;
  TRootEmbeddedCanvas *fRegion2XY, *fRegion2XZ, *fRegion2YZ;
  TCanvas *cR2XY, *cR2XZ, *cR2YZ;
  TPaveLabel *Label_R2XY, *Label_R2XZ, *Label_R2YZ;
  TBox *Box_R2XY1, *Box_R2XY2,*Box_R2XY3,*Box_R2XY4, *Box_R2XZ1, *Box_R2XZ2,*Box_R2XZ3,*Box_R2XZ4, *Box_R2YZ1, *Box_R2YZ2, *Box_R2YZ3, *Box_R2YZ4; // boxes for each chamber
  TBox *Box_R2XY; //TEMPORARY
  TText *tR2XY1, *tR2XY2, *tR2XY3, *tR2XY4, *tR2XZ1, *tR2XZ2, *tR2XZ3, *tR2XZ4,*tR2YZ1, *tR2YZ2, *tR2YZ3, *tR2YZ4 ; // text for HDC chamber labels
  std::vector<TLine> Line_R2x;
  std::vector<TLine> Line_R2u;
  std::vector<TLine> Line_R2v;

  //Region 3 (VDC) declarations
  TGCompositeFrame *fRegion3;
  TRootEmbeddedCanvas *fRegion3XY, *fRegion3XZ, *fRegion3YZ;
  TCanvas *cR3XY, *cR3XZ, *cR3YZ;
  TPaveLabel *Label_R3XY, *Label_R3XZ, *Label_R3YZ;
  TBox *Box_R3XY1, *Box_R3XY2,*Box_R3XY3,*Box_R3XY4, *Box_R3XZ1, *Box_R3XZ2,*Box_R3XZ3,*Box_R3XZ4, *Box_R3YZ1, *Box_R3YZ2, *Box_R3YZ3, *Box_R3YZ4; // boxes for each chamber
  TText *tR3XY1, *tR3XY2, *tR3XY3, *tR3XY4, *tR3XZ1, *tR3XZ2, *tR3XZ3, *tR3XZ4,*tR3YZ1, *tR3YZ2, *tR3YZ3, *tR3YZ4 ; // text for VDC chamber labels
  TLine *Div_R3XY, *Div_R3XZ, *Div_R3YZ; // dividers for views in projections
  std::vector<TLine> Line_R3u;
  std::vector<TLine> Line_R3v;

  //Region 3 (TS) declarations
  TGCompositeFrame *fRegion3TS;

  // create event buffer
  QwTreeEventBuffer* fEventBuffer; //!      // excluded from dictionary
  QwSubsystemArrayTracking* fSubsystemArray;  //!

  // create hit list
  QwHitContainer* fHitList; //!

 public:
  QwEventDisplay(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~QwEventDisplay();

  void SetEventBuffer(QwTreeEventBuffer *eventbuffer) { fEventBuffer = eventbuffer; };
  void SetSubsystemArray(QwSubsystemArrayTracking *subsystemarray) { fSubsystemArray = subsystemarray; };

  void HandleMenu(Int_t id);
  void GoPrevious();
  void GoNext();
  void GoClear();
  void GotoEvent();
  void DrawEvent();
  void DrawFit();

  ClassDef(QwEventDisplay,1);
};

#endif //QWEVENTDISPLAY
