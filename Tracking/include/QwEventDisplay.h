#ifndef QWEVENTDISPLAY_H
#define QWEVENTDISPLAY_H

#include "QwTreeEventBuffer.h"
#include "QwHitContainer.h"
#include "QwSubsystemArrayTracking.h"

// sev.C Q-Weak event display
// author: ???, Virginia Tech
// editor: Wouter Deconinck, Massachusetts Insitute of Technology email: wdconinc@mit.edu
// editor: Marcus Hendricks, George Washington University email: marcuseh@gwu.edu
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

// wdc: added the following headers
#include <TFile.h>
#include <TTree.h>
#include <TLine.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TPolyMarker.h>

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
#ifndef ROOT_TGuiBldHintsButton
#include "TGuiBldHintsButton.h"
#endif
#ifndef ROOT_TRootBrowserLite
#include "TRootBrowserLite.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
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
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
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
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
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
/* #ifndef ROOT_TGLayoutHints
#include "TGLayoutHints.h"
#endif */

class QwEventDisplay: public TGMainFrame {

 public:

  RQ_OBJECT("QwEventDisplay")

 private:
  TGMainFrame *fMain;
  //TGListBox *xPlaneListBox, *uPlaneListBox, *vPlaneListBox, *xpPlaneListBox, *upPlaneListBox, *vpPlaneListBox;
  TGListBox *Region1Box, *Region2Box, *Region3Box, *EventBox;
  //TGListBox **listboxes[6];
  TGListBox **listboxes[3];  // list boxes with region wire hits
  //TGListBox **eventbox[0]; // list box for event number
  TRootEmbeddedCanvas *c1; // Region 1 canvas with hit paths
  TRootEmbeddedCanvas *c2; // Region 2 canvas with hit paths
  TRootEmbeddedCanvas *c3; // Region 3 canvas with hit paths
  Int_t entry;
  TFile *f;
  TTree *t1;
  // need to find out how many wire planes will be in Region 1 (R1) for the Region 1 wire array
  Float_t wire[6];
  Float_t R3wire[4];
  Float_t R2driftdist[6];
  Float_t R3driftdist[4];
  Float_t driftsign[6];
  Float_t R3driftsign[4];
  Float_t fitpx[6];
  Float_t R3fitpx[4];
  Float_t fitpy[6];
  Float_t R3fitpy[4];
  // add appropriate number of lines for Region 1
  TLine *lx,*lu,*lv,*lxp,*lup,*lvp; // lines for Region 2 canvas
  TLine *R3u,*R3v,*R3uPrime,*R3vPrime; // lines for Region 3 canvas
  TLine *xzfit,*yzfit;
  TLine *R3xzfit,*R3yzfit; // lines for the x and y line fits
  TBox *b1, *b2, *b3;
  TBox *R3b1, *R3b2, *R3b3;
  TPaveLabel *label1, *label2, *label3;
  TPaveLabel *R3label1, *R3label2, *R3label3;
  TPaveText *R1text;
  TPaveText *R2text;
  TPaveText *R3text;
  TPolyMarker *points;
  TPolyMarker *R3points;

  TLine* R2lx;
  TLine* R2lu;
  TLine* R2lv;
  TLine* R2lxPrime;
  TLine* R2luPrime;
  TLine* R2lvPrime;
  // Region 2 fit lines
  TLine* R2xzfit;
  TLine* R2yzfit;
  // Region 2 boxes
  TBox* R2b1;
  TBox* R2b2;
  TBox* R2b3;
  // Region 2 canvas labels for particle paths
  TPaveLabel* R2label1;
  TPaveLabel* R2label2;
  TPaveLabel* R2label3;
  // Region 3 lines
  TLine* R3lu;
  TLine* R3lv;
  TLine* R3luPrime;
  TLine* R3lvPrime;

  // Event buffer
  QwTreeEventBuffer *fEventBuffer;
  QwSubsystemArrayTracking *fSubsystemArray;

 public:

  QwEventDisplay(const TGWindow *p, UInt_t w, UInt_t h);

  void SetEventBuffer(QwTreeEventBuffer *eventbuffer) { fEventBuffer = eventbuffer; };
  void SetSubsystemArray(QwSubsystemArrayTracking *subsystemarray) { fSubsystemArray = subsystemarray; };

  virtual ~QwEventDisplay();
  void DoAdvance();
  void DoPrevious();
  void DrawEvent();
  void DoRemoveAll();
}; // end object QwEventDisplay

#endif // QWEVENTDISPLAY

