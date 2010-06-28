#ifndef QWEVENTDISPLAY_H
#define QWEVENTDISPLAY_H

// sev.C Q-Weak event display
// author: ???, Virginia Tech
// editor: Wouter Deconinck, Massachusetts Insitute of Technology email: wdconinc@mit.edu
// editor: Marcus Hendricks, George Washington University email: marcuseh@gwu.edu
// editor: Derek Jones, George Washington University email: dwjones8@gwu.edu

#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

#include <TFile.h>
#include <TTree.h>
#include <TLine.h>
#include <TPaveLabel.h>
#include <TPaveText.h>
#include <TPolyMarker.h>

#include "TGDockableFrame.h"
#include "TGMenu.h"
#include "TGMdiDecorFrame.h"
#include "TG3DLine.h"
#include "TGMdiFrame.h"
#include "TGMdiMainFrame.h"
#include "TGuiBldHintsButton.h"
#include "TRootBrowserLite.h"
#include "TGMdiMenu.h"
#include "TGListBox.h"
#include "TGNumberEntry.h"
#include "TGScrollBar.h"
#include "TGComboBox.h"
#include "TGuiBldHintsEditor.h"
#include "TGFrame.h"
#include "TGFileDialog.h"
#include "TGShutter.h"
#include "TGButtonGroup.h"
#include "TGCanvas.h"
#include "TGFSContainer.h"
#include "TGButton.h"
#include "TGuiBldEditor.h"
#include "TGTextEdit.h"
#include "TGFSComboBox.h"
#include "TGLabel.h"
#include "TGView.h"
#include "TGMsgBox.h"
#include "TRootGuiBuilder.h"
#include "TGTab.h"
#include "TGListView.h"
#include "TGSplitter.h"
#include "TGStatusBar.h"
#include "TGListTree.h"
#include "TGToolTip.h"
#include "TGToolBar.h"
#include "TRootEmbeddedCanvas.h"
#include "TCanvas.h"
#include "TGuiBldDragManager.h"


// Forward declarations to prevent ROOT-Cint from getting confused
class QwHitContainer;
class QwEventBuffer;
class QwTreeEventBuffer;
class QwSubsystemArrayTracking;


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

  // Event canvas labels
  TPaveText *Evlabel1;
  TPaveText *Evlabel2;
  TPaveText *Evlabel3;
  TPaveText *Evlabel4;
  TPaveText **boxes[3];

  TRootEmbeddedCanvas *c1; // Region 1 canvas with hit paths
  TRootEmbeddedCanvas *c2; // Region 2 canvas with hit paths
  TRootEmbeddedCanvas *c3; // Region 3 canvas with hit paths
  Int_t fEntry;
  TFile *f;
  TTree *t1;
  // need to find out how many wire planes will be in Region 1 (R1) for the Region 1 wire array
  Float_t wire[6];
  Float_t R3wire[4];
  Float_t R2driftdist[6];
  Float_t R3driftdist[4];
  Float_t driftsign[6];
  Float_t R3driftsign[4];
  Float_t fitpx[6];;
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

  std::vector<TLine> R2lx;
  std::vector<TLine> R2lu;
  std::vector<TLine> R2lv;
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

  // Event buffer (excluded from dictionary by the comment tag)
  QwTreeEventBuffer* fEventBuffer;		//!
  QwSubsystemArrayTracking* fSubsystemArray;	//!

  // Hit list
  QwHitContainer* fHitList;			//!

 public:

  QwEventDisplay(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~QwEventDisplay();

  void SetEventBuffer(QwTreeEventBuffer *eventbuffer) { fEventBuffer = eventbuffer; };
  void SetSubsystemArray(QwSubsystemArrayTracking *subsystemarray) { fSubsystemArray = subsystemarray; };

  void DoAdvance();
  void DoPrevious();
  void DrawEvent();
  void DoRemoveAll();


  ClassDef(QwEventDisplay,1);

}; // end object QwEventDisplay

#endif // QWEVENTDISPLAY
