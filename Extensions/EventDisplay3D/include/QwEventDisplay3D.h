#ifndef QWEVENTDISPLAY3D_H
#define QWEVENTDISPLAY3D_H

// The QWeak Event Display in 3D based on the ROOT Event Display Framework.
// authors: Juan Carlos Cornejo, The College of William & Mary, cornejo@jlab.org
//          Zach Addison, MIT, zaddison@mit.edu (Region II display)

// Standard Libraries
#include <stdlib.h>
#include <iostream>

// ROOT includes
#include <TRootBrowser.h>
#include <RQ_OBJECT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TGResourcePool.h>
#include <TBrowser.h>
#include <TMath.h>

// ROOT Geometry/EVE includes
#include <TGeoManager.h>
#include <TEveManager.h>
#include <TGeoNode.h>
#include <TGeoMatrix.h>
#include <TEveGeoNode.h>
#include <TEveViewer.h>
#include <TEveScene.h>
#include <TEveBrowser.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TGLViewer.h>
#include <TGLOrthoCamera.h>
#include <TEveTrack.h>
#include <TEveVSDStructs.h>
#include <TEveTrackPropagator.h>
#include <TEveStraightLineSet.h>
#include <TEveQuadSet.h>
#include <TEveQuadSetGL.h>


// ROOT GUI includes
#include <TGWindow.h>
#include <TGTab.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TGFileDialog.h>
#include <TGTextEntry.h>

// ROOT OpenGL Includes
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoVolume.h>

// Qweak Includes
#include <QwHitRootContainer.h>
#include <QwEvent.h>
#include <QwLog.h>
#include <QwPartialTrack.h>
#include <QwTrackingTreeLine.h>

class QwEventDisplay3D : public TGMainFrame {
RQ_OBJECT("QwEventDisplay3D")
private:
  TGeoManager* fGeoManager;   // For all detectors
  //TGeoManager* fWGeoManager;  // For the wires only
  TEveManager* fEveManager;
  TGeoNode*    fTopNode;
  TEveGeoTopNode* fGeometry;
  TEveGeoTopNode* fVDCFront;
  TEveGeoTopNode* fVDCBack;
  TEveScene*  fTopScene;
  TEveScene*  fSideScene;
  TEveScene*  fWireScene;
  TEveProjectionManager* fTopProjectionManager;
  TEveProjectionManager* fSideProjectionManager;
  TEveProjectionManager* fWireProjectionManager;
  TEveViewer* fTopView;
  TEveViewer* fSideView;
  TEveViewer* fWireView;
  TEveViewer* fRandomView;

  // Event trees and the like
  TTree *fTree;
  QwHitRootContainer *fHitContainer;
  QwEvent *fEvent;
  Long64_t fNumberOfEvents;
  Long64_t fCurrentEvent;

  // A list of good events with at least one hit
  std::list<Int_t> fListOfGoodEvents;
  std::list<Int_t>::iterator fEventsListIt;

  // Event Tracks
  TEveTrack *fCurrentTrack;
  TEveTrack *fCurrentOutline;
  TEveTrackList *fCurrentTrackList;
  TObjArray *fCurrentTrackArray;
  TObjArray *fCurrentOutlineArray;

  //VDC Wire angle and separation constants
  Double_t fVDC_AngleOfWires;  // With respect to x
  Double_t fAngleOfVDC;  // With respect to z
  Double_t fVDC_WireSeparation;   // Perpendicular to the wires
  Double_t fVDC_WireXSpacing;     // in cm
  Double_t fVDC_WireYSpacing;     // in cm
  Double_t fVDC_WireZProjectedSpacing;     // in cm
  Double_t fVDC_XLength;          // Length in X of VDC in cm
  Double_t fVDC_YLength;          // Length in Y of VDC in cm
  Double_t fVDC_PlaneZPos[4];     // Z-position of plane in cm wrt to origin
  Double_t fVDC_PlaneYPos[4];     // Y-position of plane in cm wrt to origin
  Double_t fVDC_SinAngleWires;    // sin(fVDC_AngleOfWires)
  Double_t fVDC_CosAngleWires;    // cos(fVDC_AngleOfWires)
  Double_t fVDC_TanAngleWires;    // tan(fVDC_AngleOfWires)

  // Wire hit count
  Int_t fR3WireHitCount[4][279];

  //HDC Wire angle and separaction constants
  Double_t fHDC_AngleOfWires[2];  // With respect to x
  Double_t fHDC_WireSeparation;   // Perpendicular to wires
  Double_t fHDC_WireXSpacing[2];     // in cm
  Double_t fHDC_WireYSpacing[2];     // in cm
  Double_t fHDC_XLength;          // Lengh in X of HDC in cm
  Double_t fHDC_YLength;          // Lengh in Y of HDC in cm
  Double_t fHDC_PlaneZPos[12];    // Z-position of plane in cm wrt to origin
  Double_t fHDC_PlaneYPos[2];    // Y-position of plane in cm wrt to origin
  Double_t fHDC_SinAngleWires[2];    // sin(fHDC_AngleOfWires[plane#])
  Double_t fHDC_CosAngleWires[2];    // cos(fHDC_AngleOfWires[plane#])
  Double_t fHDC_XAsymetry;          // fHDC_XLength-16*fHDC_WireXSpacing[1]-----Just a guess
  Double_t fHDC_YAsymetry;          // 4/3*fHDC_XAsymetry-----Just a guess 

  // Rotation of the whole tracking system about the z axis
  Double_t fDetectorPhi;
  Double_t fPackageAngle[2];

  // GUI elements
  TGLabel *fRegion1Label;
  TGLabel *fRegion2Label;
  TGLabel *fRegion3Label;
  TGLabel *fTrigLabel;
  TGLabel *fCerenkovLabel;
  TGLabel *fScannerLabel;
  TGLabel *fCurrentEventLabel;
  TGTextButton *fSkipToEventButton;
  TGTextButton *fNextButton;
  TGTextButton *fPreviousButton;
  TGTextButton *fVDC_SwitchViewButton;
  TGTextButton *fHDC_SwitchViewButton;
  TGTextButton *fUpdateViewButton;
  TGTextButton *fOpenRootButton;
  TEveBrowser  *fBrowser;
  TGPopupMenu  *fMenu;
  TGTextEntry   *fSkipToEventLine;

  // Enumerated menu options
  enum EventDisplayMenu_e {
      kOpenRootFile
  };

  // Various detector buttons
  TGTextButton *fTargetButton;
  TGTextButton *fCollimator1Button;
  TGTextButton *fGemsButton;
  TGTextButton *fCollimator2Button;
  TGTextButton *fHDCButton;
  TGTextButton *fCollimator3Button;
  TGTextButton *fQtorButton;
  TGTextButton *fShieldButton;
  TGTextButton *fVDCButton;
  TGTextButton *fTriggerButton;
  TGTextButton *fScannerButton;
  TGTextButton *fCerenkovButton;

  // Keep track of the current view
  Bool_t fVDC_IsWireView;
  Bool_t fHDC_IsWireView;
  Bool_t fShowTarget;
  Bool_t fShowCollimator1;
  Bool_t fShowGems;
  Bool_t fShowCollimator2;
  Bool_t fShowHDC;
  Bool_t fShowCollimator3;
  Bool_t fShowQtor;
  Bool_t fShowShield;
  Bool_t fShowVDC;
  Bool_t fShowScanner;
  Bool_t fShowTrigger;
  Bool_t fShowCerenkov;

  // Various other flags
  Bool_t fDrawTreeLines;
  Bool_t fDrawTracks;

  // Color elements
  Pixel_t   fGreen;
  Pixel_t   fRed;

  // Cameras
  TGLCamera *fCurrentCamera;

  // Filetypes for open file dialog
  static const char *fFiletypes[];

  // ROOT file
  TFile *fRootFile;

  // Functions
  void LoadGeometry();
  void InitGUI();
  void InitViews();
  void InitEvents();
  void DisplayEvent();
  void HideUnecessary();
  void ClearTracks();
  void ClearOutline();
  void OpenRootFile();
  void NextEvent(Bool_t redraw=kTRUE);
  void PreviousEvent(Bool_t redraw=kTRUE);
  void DetectorButtonsEnable(Bool_t status);
  void DisplayWire(Int_t wire, Int_t plane,Int_t package, Int_t region, TString message="");
  void UpdateButtonState(TGTextButton *button, const char* text, Bool_t *state);
  void SetVisibility(const char* volname, Bool_t status);
  Double_t RotateXonZ(Double_t x, Double_t y, Double_t angle);
  Double_t RotateYonZ(Double_t x, Double_t y, Double_t angle);

public:
   QwEventDisplay3D( const TGWindow *window, UInt_t width, UInt_t height  );
   ~QwEventDisplay3D();
   void Init();
   void RedrawViews(Bool_t det = kTRUE, Bool_t tracks = kFALSE);
   void SetRotation(Double_t phi);

   // SLOTS
   /**
    * Respond to the user closing the window by first destroying all
    * created elements. This prevents both a crash and memory leaks!
    */
   void CloseWindow();

   // All  buttons
   void NextEventClicked();
   void PreviousEventClicked();
   void SwitchViewVDC();
   void SwitchViewHDC();
   void SwitchTarget();
   void SwitchCollimator1();
   void SwitchGems();
   void SwitchCollimator2();
   void SwitchHDC();
   void SwitchCollimator3();
   void SwitchQtor();
   void SwitchShield();
   void SwitchVDC();
   void SwitchTrigger();
   void SwitchScanner();
   void SwitchCerenkov();
   void UpdateView();
   void SkipToEvent();
   void MenuEvent(Int_t menuID);
   void OpenRoot(Int_t runnumber);

   // Ask ROOT to make a dictionary of this file so that slots could work
   ClassDef(QwEventDisplay3D,1);
};

#endif // QWEVENTDISPLAY3D
