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
#ifndef ROOT_TGuiBldGeometryFrame
//#include "TGuiBldGeometryFrame.h"
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

////C++ STANDARD////
#include "Riostream.h"


////FORWARD DECLARATIONS////

class QwEventBuffer;
class QwHitContainer;
class QwSubsystemArrayTracking;
class QwTreeEventBuffer;


////REGION GEOMETRIES////

//All measurements in cm; scalings make longest side of length .8 in canvas; geometry data taken from QweakSim files;
//Length is x, width is y, depth is z in qweak coordinates (x and y flip for GUI use, be careful of distinction)

#define R1_WIDTH 30.00 // Region 1
#define R1_LENGTH 30.00
#define R1_DEPTH 3.00
#define R1_DIST 0.10 // distance between gas cuts CHECK THIS!!!
#define R1_CM  0.0267 //calibrated value of 1 cm in this scale

#define R2_WIDTH 45.09 // Region 2
#define R2_LENGTH 30.00
#define R2_DEPTH 17.20
#define R2_DIST 1.90 // distance between drift cell chambers
#define R2_CM 0.0177

#define R3_WIDTH 50.00 // Region 3 geometries
#define R3_LENGTH 210.00
#define R3_DEPTH 2.54
#define R3_DIST 1.11 // distance between drift cell centers
#define R3_CM 0.0038
#define R3_ANGLE 63.43 // 90-26.57 (which is the wire angle)


////DECLARE GLOBAL CLASS ELEMENTS////

class QwEventDisplay: public TGMainFrame{  // display is a ROOT main frame class

 public:
  RQ_OBJECT("QwEventDisplay")

  UInt_t fEventNumber; // keeps track of the current event number on display
  TLine Line; // used for vector drawing

 private:  // Frames
  TGMainFrame *fMain; // mainframe window

  TGHorizontalFrame *fEventBoxes; // event box frame
  TGTab *fRegions; // tab frame for region data

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
  TGListBox *OtherListBox;

  UInt_t sum; // sum of wire numbers
  double num; // number of wire hits
  double fit; // average wire number

  //Region 1 (GEM) declarations
  TGCompositeFrame *fRegion1;
  TRootEmbeddedCanvas *fRegion1XY, *fRegion1XZ, *fRegion1YZ;
  TCanvas *cR1XY, *cR1XZ, *cR1YZ;
  TPaveLabel *Label_R1XY, *Label_R1XZ, *Label_R1YZ;
  std::vector<TLine> Line_R1r;
  std::vector<TLine> Line_R1y;
  std::vector<TLine> R1_XYfit;
  std::vector<TLine> R1_XZfit;
  TBox *Box_R1XY, *Box_R1XZ, *Box_R1YZ;

  //Region 2 (HDC) declarations
  TGCompositeFrame *fRegion2;
  TRootEmbeddedCanvas *fRegion2XY, *fRegion2XZ, *fRegion2YZ;
  TCanvas *cR2XY, *cR2XZ, *cR2YZ;
  TPaveLabel *Label_R2XY, *Label_R2XZ, *Label_R2YZ;
  std::vector<TLine> Line_R2x;
  std::vector<TLine> Line_R2u;
  std::vector<TLine> Line_R2v;
  Float_t R2driftdist[6]; // WHAT DOES THIS DO?
  Float_t driftsign[6]; // SAME HERE
  TBox *Box_R2XY, *Box_R2XZ, *Box_R2YZ;

  //Region 3 (VDC) declarations
  TGCompositeFrame *fRegion3;
  TRootEmbeddedCanvas *fRegion3XY, *fRegion3XZ, *fRegion3YZ;
  TCanvas *cR3XY, *cR3XZ, *cR3YZ;
  TPaveLabel *Label_R3XY, *Label_R3XZ, *Label_R3YZ;
  std::vector<TLine> Line_R3u;
  std::vector<TLine> Line_R3v;
  TBox *Box_R3XY, *Box_R3XZ, *Box_R3YZ;

  //Region 4 (TS) declarations???

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

  void GoPrevious();
  void GoNext();
  void GoClear();
  void GotoEvent();
  void DrawEvent();

  ClassDef(QwEventDisplay,1);
};

#endif //QWEVENTDISPLAY
