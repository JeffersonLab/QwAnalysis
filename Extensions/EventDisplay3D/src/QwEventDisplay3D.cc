#include "QwEventDisplay3D.h"
ClassImp(QwEventDisplay3D);

// ROOT Includes

// Global defines
// These represent the conversion from degrees to radians
#define _45DEG_  0.78539816;
#define _90DEG_  1.57079633;
#define _135DEG_ 2.35619449;
#define _180DEG_ 3.14159265;

// Global constants (mostly flags)
const Bool_t kDebug = kTRUE;

const char* QwEventDisplay3D::fFiletypes[] = {"ROOT files", "*.root","All files", "*", 0, 0 };

QwEventDisplay3D::QwEventDisplay3D(const  TGWindow *window, UInt_t width,
      UInt_t height)
{
   // First initialize the wire constants
   // TODO:(jc2) Read these from a configuration. Also check these numbers,
   // these were the figures I had on hand when I did this on 07/14/2010


   // The Following are VDC specs from a survey and are updated
   // as of 09/15/2010 (jcc)
   fVDC_AngleOfWires = 0.463647609;       // atan(1/2) in radians
   fAngleOfVDC  =  1.144935989;           // 65.6deg in radians
   fVDC_WireSeparation = 0.5;             // in cm
   fVDC_WireXSpacing = 1.11125;           // in cm
   fVDC_WireYSpacing = 0.559016994;       // in cm
   fVDC_WireZProjectedSpacing = 0.231198922; // in cm
   fVDC_XLength      = 204.47;            // in cm
   fVDC_YLength      = 53.34;             // in cm
   fVDC_PlaneZPos[0][0] = 448.54;         // in cm
   fVDC_PlaneZPos[0][1] = 450.86;         // in cm
   fVDC_PlaneZPos[0][2] = 501.68;         // in cm
   fVDC_PlaneZPos[0][3] = 501.68;         // in cm
   fVDC_PlaneZPos[1][0] = 449.56;         // in cm
   fVDC_PlaneZPos[1][1] = 451.88;         // in cm
   fVDC_PlaneZPos[1][2] = 502.62;         // in cm
   fVDC_PlaneZPos[1][3] = 504.94;         // in cm
   fVDC_PlaneYPos[0][0] = 271.39;         // in cm
   fVDC_PlaneYPos[0][1] = 270.35;         // in cm
   fVDC_PlaneYPos[0][2] = 293.28;         // in cm
   fVDC_PlaneYPos[0][3] = 292.24;         // in cm
   fVDC_PlaneYPos[1][0] = 269.81;         // in cm
   fVDC_PlaneYPos[1][1] = 268.77;         // in cm
   fVDC_PlaneYPos[1][2] = 291.88;         // in cm
   fVDC_PlaneYPos[1][3] = 290.84;         // in cm
   fVDC_SinAngleWires =  0.447213595;  // unitless
   fVDC_CosAngleWires =  0.894427191;  // unitless
   fVDC_TanAngleWires =  0.500000000;  // unitless


   //The Following is HDC specs from survey
   fHDC_AngleOfWires[0] = 0; // 90deg in radians
   fHDC_AngleOfWires[1] = 0.927295218; // atan(3/4) in radians
   fHDC_WireSeparation = 1.1684;  // in cm
   fHDC_WireXSpacing[0] = 0; // in cm 
   fHDC_WireYSpacing[0] = 1.1684;      // in cm
   fHDC_WireXSpacing[1] = 1.4605; // in cm
   fHDC_WireYSpacing[1] = 1.9473; // in cm
   fHDC_XLength = 28.57;  // in cm
   fHDC_YLength = 38.10;  // in cm
   fHDC_PlaneZPos[0] = -341.749;  // in cm
   fHDC_PlaneZPos[1] = -339.844;  // in cm
   fHDC_PlaneZPos[2] = -337.939;  // in cm
   fHDC_PlaneZPos[3] = -336.034;  // in cm
   fHDC_PlaneZPos[4] = -334.129;  // in cm
   fHDC_PlaneZPos[5] = -332.224;  // in cm
   fHDC_PlaneZPos[6] = -299.435;  // in cm
   fHDC_PlaneZPos[7] = -297.530;  // in cm
   fHDC_PlaneZPos[8] = -295.625;  // in cm
   fHDC_PlaneZPos[9] = -293.720;  // in cm
   fHDC_PlaneZPos[10] = -291.815;  // in cm
   fHDC_PlaneZPos[11] = -289.910;  // in cm
   fHDC_PlaneYPos[0] =  31.7716;  // in cm
   fHDC_PlaneYPos[1] =  35.7716;  // in cm
   fHDC_SinAngleWires[0] = 0;      // unitless
   fHDC_CosAngleWires[0] = 1;      // unitless
   fHDC_SinAngleWires[1] = 0.8;    // unitless
   fHDC_CosAngleWires[1] = 0.6;    // unitless 
   fHDC_XAsymetry = 5.2045; // in cm
   fHDC_YAsymetry = 6.9393; // in cm

   fCurrentTrackArray = new TObjArray();
   fCurrentOutlineArray= new TObjArray();   

   // Setup the rotation for now, assuming a default of horizontal position
   // for the tracking hardware
   SetRotation(0.);

   // Enable various flags (for now, should be done through the GUI later)
   fDrawTracks = kFALSE;
   fDrawTreeLines = kFALSE;


   // For now, just initialize
   Init();
}

QwEventDisplay3D::~QwEventDisplay3D()
{
   // Delete all we could
   delete fCurrentTrackArray;
}

void QwEventDisplay3D::Init()
{
   // Before anything else, initialize EVE
   fEveManager = TEveManager::Create();

   // Now load the geometry
   LoadGeometry();

   // Initialize the views
   InitViews();

   // Initialize the graphical user interface
   InitGUI();

   // Initialize the events (NO! Should only be called _after_ a rootfile is
   // loaded
   //InitEvents();

   // TODO: Erase the following
   SwitchViewVDC();
   SwitchViewHDC();
   //fEveManager->GetBrowser()->Browser()->SetTitle("Hey there!!!");
}

void QwEventDisplay3D::LoadGeometry()
{
   // TODO:
   // Locate the geometry file from GDML. For now, this is hard coded in, but
   // I suppose it does not have to be this way.
   TString filename = TString(getenv("QWANALYSIS"))+
      "/Extensions/EventDisplay3D/prminput/qweak.gdml";

   // Import the geometry from GDML
   fGeoManager = TGeoManager::Import(filename);
   fGeoManager->SetVisLevel(10);

   // Retrieve the top node of the geometry manager
   fTopNode = fGeoManager->GetTopNode();

   // Format for proper usage with EVE
   fGeometry = new TEveGeoTopNode(fGeoManager, fTopNode);

   // Add the global/static elements
   fEveManager->AddGlobalElement(fGeometry);


   // TODO:
   // Rotate the view so that we see the same view as one would see coming
   // out of the doghouse.
   //fEveManager->GetDefaultGLViewer()->CurrentCamera().RotateRad(90.,180.);

   // Hide any unecessary geometries
   HideUnecessary();
}

void QwEventDisplay3D::InitViews()
{
   // These must be the opposite of what we want them to be, because the
   // initial setup inavertedly switches them. I prefer it this way,
   // it's not hard to remember anyway :)
   fVDC_IsWireView = !kTRUE;
   fHDC_IsWireView = !kTRUE;
   fShowTarget = !kTRUE;
   fShowCollimator1 = !kTRUE;
   fShowGems = !kTRUE;
   fShowCollimator2 = !kTRUE;
   fShowHDC = !kTRUE;
   fShowCollimator3 = !kTRUE;
   fShowQtor = !kTRUE;
   fShowShield = !kFALSE;
   fShowVDC = !kTRUE;
   fShowScanner = !kTRUE;
   fShowTrigger = !kTRUE;
   fShowCerenkov = !kTRUE;

   // First initialize the scenes
   fTopScene = fEveManager->SpawnNewScene("Top Scene");
   fSideScene = fEveManager->SpawnNewScene("Side Scene");
   fWireScene = fEveManager->SpawnNewScene("Wire Scene");

   // Create and arrange the various viewers
   TEveWindowSlot *slot = 0;
   TEveWindowPack *pack = 0;

   // The top and side views
   slot = TEveWindow::CreateWindowInTab(
         fEveManager->GetBrowser()->GetTabRight());
   pack = slot->MakePack();
   pack->SetElementName("Top & Side View");
   pack->SetShowTitleBar(kFALSE);
   pack->NewSlot()->MakeCurrent();
   fTopView = fEveManager->SpawnNewViewer("Top View", "");
   fTopView->AddScene(fTopScene);
   fTopView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXnOZ);
   fTopView->AddScene(fEveManager->GetGlobalScene());
   fTopView->AddScene(fEveManager->GetEventScene());

   pack = pack->NewSlot()->MakePack();
   pack->SetElementName("Side View");
   pack->SetShowTitleBar(kFALSE);
   pack->NewSlot()->MakeCurrent();
   fSideView = fEveManager->SpawnNewViewer("Side View", "");
   fSideView->AddScene(fSideScene);
   fSideView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZnOY);
   fSideView->AddScene(fEveManager->GetGlobalScene());
   fSideView->AddScene(fEveManager->GetEventScene());

   // Prepare the Projection managers
   fTopProjectionManager = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
   fSideProjectionManager = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
   fWireProjectionManager = new TEveProjectionManager(TEveProjection::kPT_RPhi);
   fTopScene->AddElement(fTopProjectionManager);
   fSideScene->AddElement(fSideProjectionManager);

   // Prepare appropriate axes
   TEveProjectionAxes *topAxes = new TEveProjectionAxes(fTopProjectionManager);
   topAxes->SetTitle("Top View");
   fTopScene->AddElement(topAxes);
   TEveProjectionAxes *sideAxes =
      new TEveProjectionAxes(fSideProjectionManager);
   sideAxes->SetTitle("Side View");
   fSideScene->AddElement(sideAxes);

   // Add the axes and managers to the tree list
   fEveManager->AddToListTree(topAxes, kTRUE);
   fEveManager->AddToListTree(fTopProjectionManager, kTRUE);
   fEveManager->AddToListTree(sideAxes, kTRUE);
   fEveManager->AddToListTree(fSideProjectionManager, kTRUE);

   // something (not sure what it does, but whatever it does it seems
   // important. If the documentation were better, I'd be able to know
   // if this was true (jc2).
   fTopScene->SetRnrState(kFALSE);
   fSideScene->SetRnrState(kFALSE);

   // Finally add the geometries
   fTopProjectionManager->ImportElements( fGeometry, fTopScene );
   fSideProjectionManager->ImportElements( fGeometry, fSideScene );

   // TODO: (jc2) FIX THIS! It's really annoying.
   // Make sure we first know when the window is closed
   //TGMainFrame *mf = (TGMainFrame*) gTQSender;
   //mf->DontCallClose();
   //mf->Connect("CloseWindow()","QwEventDisplay3D",this,"CloseWindow()");
   //fEveManager->GetBrowser()->Disconnect("CloseWindow()","TEveManager",fEveManager,"CloseEveWindow()");
//   fEveManager->GetBrowser()->DontCallClose();
//   ((TRootBrowser*)fEveManager->GetBrowser()->Browser())->Disconnect("ClosedWindow()");
//   ((TRootBrowser*)fEveManager->GetBrowser()->Browser())->Connect("CloseWindow()","QwEventDisplay3D",this,"CloseWindow()");

}

void QwEventDisplay3D::InitEvents()
{
  // Extract the tree from the rootfile. It seems in the tracking mode, this
  // is simply called "tree." Neat!
  fTree = (TTree*)fRootFile->Get("event_tree");

  // Inside this tree, as I understand it, there is a hit container which
  // contains all the details about which wires were hit. Additionally
  // there is an Event which contains the details of the specific tracks.
  fHitContainer = NULL; // To allocate the memory already!
  fEvent = NULL;        // To allocate the memory already!
  fTree->SetBranchAddress("hits",&fHitContainer);
  fTree->SetBranchAddress("events",&fEvent);

  // Read the number of events available
  fNumberOfEvents = fTree->GetEntries();
  fCurrentEvent = 0;

  // Disable next button if no events in tree
  if ( fNumberOfEvents == 0 ) {
     fNextButton->SetEnabled(kFALSE);
  } else {
     // Get the first event
     fTree->GetEntry(fCurrentEvent);

     // If it is empty, find another. Display if valid.
     if( fEvent->GetNumberOfHits() < 1 ) {
        fEventsListIt = fListOfGoodEvents.end();
        NextEvent();
     } else {
        // Add it to the list of valid events
        fListOfGoodEvents.push_back(fCurrentEvent);
        fEventsListIt = fListOfGoodEvents.begin();
        DisplayEvent();
     }
     // Finally, enable the next button
     fNextButton->SetEnabled(kTRUE);
  }

}

void QwEventDisplay3D::InitGUI()
{
   // Initialize colors
   gClient->GetColorByName("green",fGreen);
   gClient->GetColorByName("red",fRed);

   // What browser should we use?
   fBrowser = fEveManager->GetBrowser();

   // Start a new tab at the left side and fill it
   fBrowser->StartEmbedding(TRootBrowser::kLeft  );
   TGMainFrame *mainFrame = new TGMainFrame(gClient->GetRoot(),100,100);
   mainFrame->ChangeBackground(fGreen);

   // Add the vertical layout frame
   TGVerticalFrame *vFrame = new TGVerticalFrame(mainFrame,100,100);

   // Add the next and previous event buttons side by side
   TGHorizontalFrame *hFrame = new TGHorizontalFrame(vFrame,100,25);
   hFrame->ChangeBackground(fGreen);
   fPreviousButton = new TGTextButton(hFrame,"Previous");
   fNextButton = new TGTextButton(hFrame,"Next");
   hFrame->AddFrame(fPreviousButton,new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX));
   hFrame->AddFrame(fNextButton,new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX));

   // Disable the next and previous buttons for now. They should be enabled
   // only after a valid file has been opened.
   fPreviousButton->SetEnabled(kFALSE);
   fNextButton->SetEnabled(kFALSE);

   vFrame->AddFrame(hFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop |
           kLHintsExpandX));

   // Different inputs
   hFrame = new TGHorizontalFrame(vFrame,100,25);
   fCurrentEventLabel = new TGLabel(hFrame,"--");
   hFrame->AddFrame(new TGLabel(hFrame,"Current Event:"),
         new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX |
            kLHintsExpandY));
   hFrame->AddFrame(fCurrentEventLabel, new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX | kLHintsExpandY));
   vFrame->AddFrame(hFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop |
           kLHintsExpandX));

   hFrame = new TGHorizontalFrame(vFrame,100,25);
   fSkipToEventButton = new TGTextButton(hFrame,"Skip to event: ");
   fSkipToEventLine = new TGTextEntry(hFrame);
   hFrame->AddFrame(fSkipToEventButton, new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX|kLHintsExpandY));
   hFrame->AddFrame(fSkipToEventLine, new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX));
   vFrame->AddFrame(hFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop |
           kLHintsExpandX));


   // Now add the labels
   fRegion1Label = new TGLabel(vFrame,"REGION 1 HIT");
   fRegion2Label = new TGLabel(vFrame,"REGION 2 HIT");
   fRegion3Label = new TGLabel(vFrame,"REGION 3 HIT");
   fTrigLabel = new TGLabel(vFrame,"TRIGGER HIT");
   fCerenkovLabel = new TGLabel(vFrame,"MAIN DETECTOR HIT");
   fScannerLabel = new TGLabel(vFrame,"SCANNER HIT");
   vFrame->AddFrame(fRegion1Label, new TGLayoutHints(kLHintsLeft | kLHintsTop
            | kLHintsExpandX | kLHintsExpandY));
   vFrame->AddFrame(fRegion2Label,new TGLayoutHints(kLHintsLeft | kLHintsTop
            | kLHintsExpandX|kLHintsExpandY));
   vFrame->AddFrame(fRegion3Label,new TGLayoutHints(kLHintsLeft | kLHintsTop
            | kLHintsExpandX|kLHintsExpandY));
   vFrame->AddFrame(fTrigLabel,new TGLayoutHints(kLHintsLeft | kLHintsTop
            | kLHintsExpandX|kLHintsExpandY));
   vFrame->AddFrame(fCerenkovLabel,new TGLayoutHints(kLHintsLeft | kLHintsTop
            | kLHintsExpandX|kLHintsExpandY));
   vFrame->AddFrame(fScannerLabel,new TGLayoutHints(kLHintsLeft | kLHintsTop
            | kLHintsExpandX|kLHintsExpandY));

   // Initialize all labels as not hit
   fRegion1Label->ChangeBackground(fRed);
   fRegion2Label->ChangeBackground(fRed);
   fRegion3Label->ChangeBackground(fRed);
   fTrigLabel->ChangeBackground(fRed);
   fCerenkovLabel->ChangeBackground(fRed);
   fScannerLabel->ChangeBackground(fRed);

   // Add the switch view buttons:
   // For the VDC's
   fVDC_SwitchViewButton = new TGTextButton(vFrame,"Switch to Wire View of VDC's");
   vFrame->AddFrame(fVDC_SwitchViewButton, new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX));
   fVDC_IsWireView = kFALSE;

   // For the HDC's
   fHDC_SwitchViewButton = new TGTextButton(vFrame,"Switch to Wire View of HDC's");
   vFrame->AddFrame(fHDC_SwitchViewButton, new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX));
   fHDC_IsWireView = kFALSE;



   // Add the detector configuration canvas
   TGCanvas *canvas = new TGCanvas(vFrame,100,100);
   TGViewPort *viewPort = canvas->GetViewPort();
   TGVerticalFrame *cFrame = new TGVerticalFrame(viewPort,100,100,kVerticalFrame|kOwnBackground);
   fTargetButton =  new TGTextButton(cFrame,"Target");
   fCollimator1Button =  new TGTextButton(cFrame,"Collimator1");
   fGemsButton =  new TGTextButton(cFrame,"GEMS");
   fCollimator2Button =  new TGTextButton(cFrame,"Collimator2");
   fHDCButton =  new TGTextButton(cFrame,"HDC");
   fCollimator3Button =  new TGTextButton(cFrame,"Collimator3");
   fQtorButton =  new TGTextButton(cFrame,"QTOR");
   fShieldButton =  new TGTextButton(cFrame,"Shield");
   fVDCButton =  new TGTextButton(cFrame,"VDC");
   fTriggerButton =  new TGTextButton(cFrame,"Trigger");
   fScannerButton =  new TGTextButton(cFrame,"Scanner");
   fCerenkovButton =  new TGTextButton(cFrame,"Main Detector");

   cFrame->AddFrame(fTargetButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fCollimator1Button, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fGemsButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fCollimator2Button, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fHDCButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fCollimator3Button, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fQtorButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fShieldButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fVDCButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fTriggerButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fScannerButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));
   cFrame->AddFrame(fCerenkovButton, new TGLayoutHints(kLHintsLeft | 
            kLHintsTop | kLHintsExpandX));

   canvas->SetContainer(cFrame);
   canvas->MapSubwindows();

   vFrame->AddFrame(canvas,new TGLayoutHints(kLHintsLeft | kLHintsTop
                        | kLHintsExpandX|kLHintsExpandY));

   // Add the update  view button
   fUpdateViewButton = new TGTextButton(vFrame,"Update View");
   vFrame->AddFrame(fUpdateViewButton, new TGLayoutHints(kLHintsLeft |
            kLHintsTop | kLHintsExpandX));

   // Clean up the main Frame
   mainFrame->AddFrame(vFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX|kLHintsExpandY));
   mainFrame->MapSubwindows();
   mainFrame->Resize();
   mainFrame->MapWindow();

   // Stop filling the Control Panel tab
   fBrowser->StopEmbedding();
   fBrowser->SetTabTitle("Control Panel",0);

   // Add a new menu bar
   fMenu = new TGPopupMenu(gClient->GetRoot());
   fMenu->AddEntry("&Open ROOT File",kOpenRootFile);
   fMenu->AddSeparator();
   fBrowser->GetMenuBar()->AddPopup("E&vent Display",fMenu, new TGLayoutHints(
            kLHintsTop | kLHintsLeft));

   // Connect the menu bar to a signal
   fMenu->Connect("Activated(Int_t)","QwEventDisplay3D",this,"MenuEvent(Int_t");
   fBrowser->GetTopMenuFrame()->MapSubwindows();

  // Now we have to connect the buttons to the respective signals
   fNextButton->Connect("Clicked()","QwEventDisplay3D",this,"NextEventClicked()");
   fPreviousButton->Connect("Clicked()","QwEventDisplay3D",this,"PreviousEventClicked()");
   fVDC_SwitchViewButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchViewVDC()");
   fHDC_SwitchViewButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchViewHDC()");
   fTargetButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchTarget()");
   fCollimator1Button->Connect("Clicked()","QwEventDisplay3D",this,
         "SwitchCollimator1()");
   fGemsButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchGems()");
   fCollimator2Button->Connect("Clicked()","QwEventDisplay3D",this,
         "SwitchCollimator2()");
   fHDCButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchHDC()");
   fCollimator3Button->Connect("Clicked()","QwEventDisplay3D",this,
         "SwitchCollimator3()");
   fQtorButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchQtor()");
   fShieldButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchShield()");
   fVDCButton->Connect("Clicked()","QwEventDisplay3D",this,"SwitchVDC()");
   fTriggerButton->Connect("Clicked()","QwEventDisplay3D",this,
         "SwitchTrigger()");
   fScannerButton->Connect("Clicked()","QwEventDisplay3D",this,
         "SwitchScanner()");
   fCerenkovButton->Connect("Clicked()","QwEventDisplay3D",this,
         "SwitchCerenkov()");
   fUpdateViewButton->Connect("Clicked()","QwEventDisplay3D",this,
         "UpdateView()");
   fSkipToEventButton->Connect("Clicked()","QwEventDisplay3D",this,
         "SkipToEvent()");

   // This is the reason why the flags above have to be opposite of what we
   // want them to be. We set the button colors just by invoking the
   // respective switch functions
   SwitchTarget();
   SwitchCollimator1();
   SwitchGems();
   SwitchCollimator2();
   SwitchHDC();
   SwitchCollimator3();
   SwitchQtor();
   SwitchShield();
   SwitchVDC();
   SwitchTrigger();
   SwitchScanner();
   SwitchCerenkov();

}

void QwEventDisplay3D::RedrawViews(Bool_t det, Bool_t tracks)
{
   // Fully redraw the required detectors and tracks
   fEveManager->FullRedraw3D(det,tracks);
}

void QwEventDisplay3D::CloseWindow()
{
   // This slot has been so kindly placed here so that ROOT may ignore it.
   // I don't want ROOT to ignore it, but what are you going to do? It has a
   // mind of its own! When and if ROOT stops ignoring it, place here the
   // necessary set of commands to cleanly close the program without a severe
   // crash.
   fEveManager->GetBrowser()->DontCallClose();
   std::cout << "Hear me! For I am ROOT! I have heard your call and I have"
      << " responded a such!\n";
   exit(0);
}

void QwEventDisplay3D::NextEventClicked()
{
   // Respond to the button being clicked by asking the program to find and
   // redraw the next event
   NextEvent();
}

void QwEventDisplay3D::NextEvent(Bool_t redraw)
{
   // Check to see if we are already the last event in the list. If we are not
   // then we search for the next valid event.
   if ( fEventsListIt == --fListOfGoodEvents.end() ) {
      // Get the next event then!
      fTree->GetEntry(++fCurrentEvent);

      // Seek the next valid event with at least one hit in any detector
      while ( fCurrentEvent < fNumberOfEvents && 
           fEvent->GetNumberOfHits() < 1 ) {
         // Get the next event then!
         fCurrentEvent++;
         fTree->GetEntry(fCurrentEvent);
      }

      if ( fCurrentEvent < fNumberOfEvents-2 ) {
         fListOfGoodEvents.push_back(fCurrentEvent);
         fEventsListIt++;
      }
   } else  { // We are not at the end of the list
      fEventsListIt++;
      fCurrentEvent = *fEventsListIt;
      fTree->GetEntry(fCurrentEvent);
   }

   // Make sure we are not on the last event
   if( fCurrentEvent > (fNumberOfEvents-2) ) {
      // Disable the next button
      fNextButton->SetEnabled(kFALSE);
   }
   fPreviousButton->SetEnabled(kTRUE);

   if(redraw)
      DisplayEvent();
}

void QwEventDisplay3D::PreviousEventClicked()
{
   // Draw the previous event
   PreviousEvent(kTRUE);
}

void QwEventDisplay3D::PreviousEvent(Bool_t redraw)
{
   // Iterate backwards (but make sure we are not at the start
   if( fEventsListIt != fListOfGoodEvents.begin() )
      fEventsListIt--;
   else {
      std::cout << "Is this it?\n";
      fPreviousButton->SetEnabled(kFALSE);
      return;
   }

   // set the current event to what he have now
   fCurrentEvent = *fEventsListIt;

   // Make sure the previous event won't be the first
   if( fEventsListIt == fListOfGoodEvents.begin() ) {
      fPreviousButton->SetEnabled(kFALSE);
   }
   if( fNumberOfEvents > 0 ) {
      fNextButton->SetEnabled(kTRUE);
   }

   // Get the next event then!
   fTree->GetEntry(fCurrentEvent);
   if(redraw) {
      DisplayEvent();
   }
}

void QwEventDisplay3D::DisplayEvent()
{
   // Disable all hit labels
   fRegion1Label->ChangeBackground(fRed);
   fRegion2Label->ChangeBackground(fRed);
   fRegion3Label->ChangeBackground(fRed);
   fTrigLabel->ChangeBackground(fRed);
   fCerenkovLabel->ChangeBackground(fRed);
   fScannerLabel->ChangeBackground(fRed);

   // Clear the Wire hit counters
   for(Int_t i=0; i<4;i++)
      for(Int_t j=0; j<279;j++)
         fR3WireHitCount[i][j] = 0;

   // Define a series of place holders for the information we will extract
   // off the trees
   Int_t region;
   Int_t package =-1, plane=-1, wire=-1;
   Double_t direction = -1;
   Int_t numberOfHits = 0;
   Bool_t ambiguousR3Wire = kFALSE;
   QwHit *hit = 0;


   // Check to see if a previous event was already drawn and remove it
     if( fCurrentTrack )
      ClearTracks();


   // Find out if there are any valid "hits" in this event
   numberOfHits = fEvent->GetNumberOfHits();
   //fHitContainer->GetDetectorInfo();
   if ( numberOfHits > 0 ) {

      //#########################################################
      // Not sure what I am doing, but the examples say otherwise
      fCurrentTrackList = new TEveTrackList();
      //TEveTrackPropagator *prop = fCurrentTrackList->GetPropagator();
      //prop->SetFitDaughters(kFALSE);
      //prop->SetMaxZ(1000);

      // End the uncertainty in what I do!
      //##################################


      if (kDebug) {
         std::cout << "========================EVENT=============\n";
         std::cout << "\t\t -- Event: " << fCurrentEvent << " --\n";
         std::cout << "Number reconstructed tracks: " 
            << fEvent->GetNumberOfTracks() << "\n\n";
      }

      // Update the GUI
      char buff[32];
      sprintf(buff,"%d",fCurrentEvent);
      fCurrentEventLabel->SetText(buff);


      // Now loop through the hit container to extract all information about
      // all hits in his event (accross _all_ regions)
      for( Int_t j = 0; j < numberOfHits; j++ ) {
         hit = (QwHit*)fHitContainer->GetHit(j);
         region = hit->GetRegion();
         direction = hit->GetDirection();
         package = hit->GetPackage();
         plane = hit->GetPlane();
         wire = hit->GetElement();
         ambiguousR3Wire = (Bool_t)hit->AmbiguousElement();
         if (kDebug) {
            std::cout << "---Hit: " << j << "\n";
            std::cout << "Region: " << region << "\n";
            std::cout << "Direction: " << direction << "\n";
            std::cout << "Package: " << package << "\n";
            std::cout << "Plane: " << plane << "\n";
            std::cout << "Wire: " << wire << "\n";
            if(ambiguousR3Wire)
               std::cout << "--Ambiguous Wire--\n";
         }

         // Now light up the respective labels to indicate hits
         switch( region ) {
            case 1:
               fRegion1Label->ChangeBackground(fGreen);
               break;
            case 2:
               fRegion2Label->ChangeBackground(fGreen);
               break;
            case 3:
               fRegion3Label->ChangeBackground(fGreen);
               break;
            case 4:
               fTrigLabel->ChangeBackground(fGreen);
               break;
            case 5:
               fCerenkovLabel->ChangeBackground(fGreen);
               break;
            case 6:
               fScannerLabel->ChangeBackground(fGreen);
               break;
            default:
               break;
         }

         // Formulate an additional message to display in each wire
         TString message("");

         // Increment the wire hit counts
         if(region==3) {
            fR3WireHitCount[plane-1][wire-1]++;
            if ( ambiguousR3Wire )
               message = "\n--Ambiguous Wire--";
         }


         // Display the wire hit
         DisplayWire(wire,plane,package,region,message);
     }
     // For now we will display multiple partial, possibly not connected,
     // tracks until a good one is produced in the rootfiles.
     Int_t numberOfPartialTracks = fEvent->GetNumberOfPartialTracks();
     std::cout << "Number of Partial Tracks: " << numberOfPartialTracks <<"\n";
     for (Int_t k = 0; k < numberOfPartialTracks; k++ ) {
        // Extract the PartialTrack
        const QwPartialTrack *partialTrack = fEvent->GetPartialTrack(k);

        // Region 1 & 2 should point back toward the target, while Region 3
        // should point towards the beam dump
        if( fDrawTracks ) {
           Int_t sign = 1;
           if( partialTrack && partialTrack->GetRegion() == kRegionID2)
              sign = -1;

           if( partialTrack->IsValid() ) {
              std::cout << "Region: " << partialTrack->GetRegion() << "\n";

              TEveRecTrack *track = new TEveRecTrack();
              TEveTrackPropagator *prop = new TEveTrackPropagator();
              prop->SetMagField(0.,0.,0.);
              prop->SetFitDaughters(kFALSE);
              prop->SetMaxR(650.);
              prop->SetMaxZ(650.);

              // This assumes the Origin is centered at z=0, and the two offsets
              // found to the partial tracks
              std::cout << "Origin: (" << partialTrack->fOffsetX/10. << "," <<
                 partialTrack->fOffsetY/10. << ",0.)\n";
              track->fV.Set(RotateXonZ(partialTrack->fOffsetX/10.,
                       partialTrack->fOffsetY/10.,fPackageAngle),
                    RotateYonZ(partialTrack->fOffsetX/10.,
                       partialTrack->fOffsetY/10.,fPackageAngle),0.);

              // The components of this vector are represented only by ratios
              // of X and Y to Z. That assumes that fSlopeX and fSlopeY are
              // proper ratios compared to z. Let z = 1 for simplicity
              std::cout << "Slopes: (" << partialTrack->fSlopeX << "," <<
                 partialTrack->fSlopeY << "," << sign*1. << ")\n";
              track->fP.Set(RotateXonZ(partialTrack->fSlopeX*sign,
                    partialTrack->fSlopeY*sign,fPackageAngle),
                    RotateYonZ(partialTrack->fSlopeX*sign,
                    partialTrack->fSlopeY*sign,fPackageAngle),1.);

              // The technical details of drawing the track
              fCurrentTrack = new TEveTrack(track,prop);
              fCurrentTrackArray->Add(fCurrentTrack);
              fCurrentTrack->SetMainColor(6);
              fCurrentTrack->SetMarkerColor(kGreen);
              fCurrentTrack->SetMarkerStyle(5);
              fCurrentTrack->SetMarkerSize(0.5);
              fCurrentTrack->SetTitle(Form("Track for region: %d\n\nOrigin: "
                       "(%.3f,%.3f,%.3f)\nSlope: (%.3f,%.3f,%.3f)",
                       partialTrack->GetRegion(),
                       partialTrack->fOffsetX/10.,
                       partialTrack->fOffsetY/10., 0.,
                       partialTrack->fSlopeX,partialTrack->fSlopeY,1.));
              fEveManager->AddElement(fCurrentTrackList);
              fEveManager->AddElement(fCurrentTrack);
              fCurrentTrack->MakeTrack();
           }
        }
     }

     if( fDrawTreeLines ) {
         // For now, we also draw planes representing the tree lines
         Int_t numberOfTreeLines = fEvent->GetNumberOfTreeLines();
         std::cout << "Number of Tree Lines: " << numberOfTreeLines << "\n";
         for (Int_t k = 0; k < numberOfTreeLines; k++ ) {
            // Extract the TreeLine
            const QwTrackingTreeLine *treeline = fEvent->GetTreeLine(k);

            if (treeline && treeline->IsValid()) {
               std::cout << "Region: " << treeline->GetRegion() << "\t"
                  << "Plane: " << treeline->GetPlane() << "\t"
                  << "Slope: " << treeline->GetSlope() << "\n";

               // Draw the plane
               TEveQuadSet *evePlane = new TEveQuadSet(
                     TEveQuadSet::kQT_RectangleYZ, kTRUE, 1);
               evePlane->AddQuad(0.,0.,0.,100.);
               evePlane->ComputeBBox();
               evePlane->AddQuad(0.,0.,1000.);
               evePlane->AddQuad(0.,1000.,1000.);
               evePlane->AddQuad(0.,1000.,0.);
               //TEveQuadSetGL *planeGL = new TEveQuadSetGL();
               //planeGL->SetModel(evePlane);
               fEveManager->AddElement(evePlane);
            }
        }
     }


/*
      fSideProjectionManager->ImportElements( (TEveElement*)fEveManager->GetCurrentEvent(), fSideScene );
      fTopProjectionManager->ImportElements( (TEveElement*)fEveManager->GetCurrentEvent(), fTopScene );
      gSystem->ProcessEvents();
*/


      if( kDebug )
         std::cout << "==================END EVENT=============\n";

      // Uncomment the following if you want to see the full drawing of
      // the planes for region 3
      /*for(Int_t package = 1; package<=2; package++ )
         for( Int_t plane = 1; plane<=4; plane++ )
            for ( Int_t wire = 1; wire <= 279; wire++ )
               DisplayWire(wire,plane,package,3,"");*/
   }

   fEveManager->Redraw3D(kFALSE,kTRUE);
}

void QwEventDisplay3D::HideUnecessary()
{
   // (jc2) There are some geometrical figures/detectors that I feel
   // limit our view of the real important parts. Hence, I choose to disable
   // some of them from the start.

   // Hide the Hall Floor
   fTopNode->GetVolume()->FindNode("HallFloor_Logical#1380448_115")->GetVolume()->SetInvisible();

   // Hide the collimator walls around GEMS and HDC
   fTopNode->GetVolume()->FindNode("CollimatorSupport_TopCover_Log#13a7340_228")
      ->GetVolume()->SetInvisible();
   fTopNode->GetVolume()
      ->FindNode("CollimatorSupport_BottomCover_Log#13a74b0_229")
      ->GetVolume()->SetInvisible();
   fTopNode->GetVolume()
      ->FindNode("CollimatorSupport_LeftCover_Log#13a7610_230")
      ->GetVolume()->SetInvisible();
   fTopNode->GetVolume()
      ->FindNode("CollimatorSupport_RightCover_Log#13a7780_231")
      ->GetVolume()->SetInvisible();

   // Hide the plane between the two VDC's (I'm not sure what this plane is,
   // actually, but it certainly should not be there I think.)
   SetVisibility("VDC_DriftCellMasterContainer_Log#13acf38_239",kFALSE);
}

void QwEventDisplay3D::SwitchViewVDC()
{
   // Switch between Wire View and Detector View. In wire view, ideally,
   // only the wires and some minimal hint as to where the Drift Chambers
   // are should be shown.


   // TODO: In an attempt to retain the current perspective I keep track of
   // the camera. It doesn't quite work yet, see if we can fi that. (jc2)
   fCurrentCamera = &fEveManager->GetDefaultGLViewer()->CurrentCamera();

   // Wire view
   // For the VDC's
   if (!fVDC_IsWireView) {
      // Reset button text
      fVDC_SwitchViewButton->SetText("Switch to Detector View");     

      // An object array to keep track of all nodes
      TObjArray *nodes = fTopNode->GetVolume()->GetNodes();
      Int_t number = nodes->GetEntries();
      for( Int_t i = 0; i < number; i++ ) {
         ((TGeoNode*)nodes->At(i))->GetVolume()->VisibleDaughters(kFALSE);
         ((TGeoNode*)nodes->At(i))->GetVolume()->SetVisibility(kFALSE);
      }

      // Since we just disabled every single detector and object, let's enable
      // the shell of the VDC's.
      fTopNode->GetVolume()->FindNode("VDC_MasterContainer_Log#13abb58_238")
         ->SetVisibility(kTRUE);
      fTopNode->GetVolume()->FindNode("VDC_MasterContainer_Log#13abb58_237")
         ->SetVisibility(kTRUE);

      // Disable the Detector buttons
      DetectorButtonsEnable(kFALSE);
   } else { // Detector View
      // Reset button text
      fVDC_SwitchViewButton->SetText("Switch to Wire View of VDC's");
      fHDC_SwitchViewButton->SetText("Switch to Wire View of HDC's");

      // An object array to keep track of all nodes
      TObjArray *nodes = fTopNode->GetVolume()->GetNodes();
      Int_t number = nodes->GetEntries();
      for( Int_t i = 0; i < number; i++ ) {
         ((TGeoNode*)nodes->At(i))->GetVolume()->VisibleDaughters(kTRUE);
         ((TGeoNode*)nodes->At(i))->GetVolume()->SetVisibility(kTRUE);
      }

      // Show all detector volumes
      fTopNode->GetVolume()->VisibleDaughters(kTRUE);

      // Of course hide those undesirable objects.
      HideUnecessary();
      fHDC_IsWireView = !fHDC_IsWireView; 

      /// Enable the Detector buttons
      DetectorButtonsEnable(kTRUE);

   }

   // In principle this was supposed to return our old camera view, but
   // clearly this can't be right.
   //fEveManager->GetDefaultGLViewer()->SetCurrentCamera(*fCurrentCamera);

   // Reset flag
    fVDC_IsWireView = !fVDC_IsWireView;

   // Now redraw
   RedrawViews();

   // Clear HDC Wire Plane Outlines
      ClearOutline();

   // Update if necessary
   UpdateView();
}


void QwEventDisplay3D::SwitchViewHDC()
{
   // Switch between Wire View and Detector View. In wire view, ideally,
   // only the wires and some minimal hint as to where the Drift Chambers
   // are should be shown.


   // TODO: In an attempt to retain the current perspective I keep track of
   // the camera. It doesn't quite work yet, see if we can fi that. (jc2)
   fCurrentCamera = &fEveManager->GetDefaultGLViewer()->CurrentCamera();

   // Wire view
   // For the HDC's
   if (!fHDC_IsWireView) {
      // Reset button text
      fHDC_SwitchViewButton->SetText("Switch to Detector View");

      // An object array to keep track of all nodes
      TObjArray *nodes = fTopNode->GetVolume()->GetNodes();
      Int_t number = nodes->GetEntries();
      for( Int_t i = 0; i < number; i++ ) {
         ((TGeoNode*)nodes->At(i))->GetVolume()->VisibleDaughters(kFALSE);
         ((TGeoNode*)nodes->At(i))->GetVolume()->SetVisibility(kFALSE);
      }

      // Since we just disabled every single detector and object, let's enable
      // the shell of the HDC's.
     fTopNode->GetVolume()->FindNode("HDC_MasterContainer_Log#13aab08_235")
         ->SetVisibility(kTRUE);
     fTopNode->GetVolume()->FindNode("HDC_MasterContainer_Log#13aab08_236")
         ->SetVisibility(kTRUE);

     //Attempting to draw the outlines of the planes
     TEveStraightLineSet* outline = new TEveStraightLineSet();
     TEveStraightLineSet* frame = new TEveStraightLineSet();    


    // This will utimately represent the wire
    TEveRecTrack *fakewire  = new TEveRecTrack();
    TEveTrackPropagator *fakeprop = new TEveTrackPropagator();
    fakeprop->SetMagField(0.,0.,0.);
    fakeprop->SetFitDaughters(kFALSE); 

    
     for(int sign=0; sign<=1; sign++)
       {
    for(int i=0; i<=11; i++)
     {

     //Getting correct Y-Pos for this planes #
     Double_t fHDC_NewPlaneYPos;
     if(i<=5)
        {fHDC_NewPlaneYPos=(-2*sign+1)*fHDC_PlaneYPos[0];}
     else
        {fHDC_NewPlaneYPos=(-2*sign+1)*fHDC_PlaneYPos[1];}

     // To outline the HDC Planes:
     outline->AddLine(fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[i],
            fHDC_XLength/2.,
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[i]);
     outline->AddLine(-fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[i],
            -fHDC_XLength/2.,
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[i]);
     outline->AddLine(-fHDC_XLength/2.,
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[i],
            fHDC_XLength/2.,
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[i]);
     outline->AddLine(-fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[i],
            fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[i]);}}


    
     for(int sign=0; sign<=1; sign++)
       {
    for(int i=0; i<=1; i++)
     {
     //Getting correct Y-Pos for this planes #
     Double_t fHDC_NewPlaneYPos;
     if(i==0)
        {fHDC_NewPlaneYPos=(-2*sign+1)*fHDC_PlaneYPos[0];}
     else
        {fHDC_NewPlaneYPos=(-2*sign+1)*fHDC_PlaneYPos[1];}

     // To Outline the HDC Frame
     int f=i*6;
     int g=6*i+5;

     frame->AddLine(-fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[f],
            -fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[g]);
     frame->AddLine(fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[f],
            fHDC_XLength/2., 
            fHDC_NewPlaneYPos-fHDC_YLength/2.,
            fHDC_PlaneZPos[g]);
     frame->AddLine(fHDC_XLength/2.,
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[f],
            fHDC_XLength/2., 
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[g]);
     frame->AddLine(-fHDC_XLength/2.,
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[f],
            -fHDC_XLength/2., 
            fHDC_NewPlaneYPos+fHDC_YLength/2.,
            fHDC_PlaneZPos[g]);}}


   fCurrentOutline = new TEveTrack(fakewire,fakeprop);
   outline->SetMarkerSize(1.5);
   outline->SetMarkerStyle(4);
   outline->SetMainColor(2);
   frame->SetMarkerSize(1.5);
   frame->SetMarkerStyle(4);
   frame->SetMainColor(3);
   fEveManager->AddElement(outline);
   fCurrentOutlineArray->Add(outline);
   fEveManager->AddElement(frame);
   fCurrentOutlineArray->Add(frame);
    }

else { // Detector View
      // Reset button text
      fHDC_SwitchViewButton->SetText("Switch to Wire View of HDC's");
      fVDC_SwitchViewButton->SetText("Switch to Wire View of VDC's");

      // An object array to keep track of all nodes
      TObjArray *nodes = fTopNode->GetVolume()->GetNodes();
      Int_t number = nodes->GetEntries();
      for( Int_t i = 0; i < number; i++ ) {

         ((TGeoNode*)nodes->At(i))->GetVolume()->VisibleDaughters(kTRUE);
         ((TGeoNode*)nodes->At(i))->GetVolume()->SetVisibility(kTRUE);
      }

      // Show all detector volumes
      fTopNode->GetVolume()->VisibleDaughters(kTRUE);

      // Of course hide those undesirable objects.
      HideUnecessary();

      fVDC_IsWireView = !fVDC_IsWireView;

      // Clear HDC Wire Plane Outlines
      ClearOutline();


}


 // In principle this was supposed to return our old camera view, but
   // clearly this can't be right.
   //fEveManager->GetDefaultGLViewer()->SetCurrentCamera(*fCurrentCamera);

   // Reset flag
    fHDC_IsWireView = !fHDC_IsWireView;

   // Now redraw
   RedrawViews();

   // Update if necessary
   UpdateView();
}


void QwEventDisplay3D::ClearOutline()
{
   //This will clear the Outline of the HDC Planes
   Int_t entries = fCurrentOutlineArray->GetEntries();
   std::cout << "Clearing " << entries << " tracks from memory\n";
   for( Int_t i = 0; i< entries; i++ ) {
     fEveManager->RemoveElement((TEveElement*)fCurrentOutlineArray->At(i),
            (TEveElement*)fEveManager->GetCurrentEvent());
   }
   //fEveManager->RemoveElement((TEveElement*)fCurrentOutlineList,
     //    (TEveElement*)fEveManager->GetCurrentEvent());

   fCurrentOutline = 0;
   //fCurrentOutlineList = 0;
   fCurrentOutlineArray->Delete();
}



void QwEventDisplay3D::ClearTracks()
{
   // The title might be misleading, but this clears anything represented
   // as an TEveTrack. This includes, of course, the wires, which I have
   // drawn as tracks. Anyway, as the name suggests, this clears them from
   // memory, otherwise they will always be displayed...always....and forever.
   Int_t entries = fCurrentTrackArray->GetEntries();
   if (kDebug)
      std::cout << "Clearing " << entries << " tracks from memory\n";
   for( Int_t i = 0; i< entries; i++ ) {
     fEveManager->RemoveElement((TEveElement*)fCurrentTrackArray->At(i),
            (TEveElement*)fEveManager->GetCurrentEvent());
   }
   fEveManager->RemoveElement((TEveElement*)fCurrentTrackList,
         (TEveElement*)fEveManager->GetCurrentEvent());

   fCurrentTrack = 0;
   fCurrentTrackList = 0;
   fCurrentTrackArray->Delete();
}

void QwEventDisplay3D::DisplayWire(Int_t wire, Int_t plane, Int_t package,
      Int_t region, TString message)
{
   // This displays the requested wire. The wires are represented as tracks
   // just for simplicity, and because it allows more control of their
   // position. Additionally, we can interact with them :)

   // This will utimately represent the wire
   TEveRecTrack *wireT  = new TEveRecTrack();
   TEveTrackPropagator *prop = new TEveTrackPropagator();
   prop->SetMagField(0.,0.,0.);
   prop->SetFitDaughters(kFALSE);

   //For Region 2 We draw with Lines rather then tracks.
   TEveStraightLineSet* ls = new TEveStraightLineSet();

   // The Following is for region 3:
   if ( region == kRegionID3 ) {
      // TODO: There was a time when we were fed planes 1-8. This should
      // prevent someone from accidentally feeding us these numbers in the
      // future again.If this issue has been fixed, this can safely be removed.
      plane = (plane-1)%4 + 1;

      // The sign corresponds to the direction of the wires. Planes U and V
      // inverted over the local y axis, with respect to each other.
      Int_t sign = 1;
      if( ((plane-1)%2) == 1 )
         sign = -1;

      // In region 3 there are 3 wire length types. The ones in the center 
      // which are all the same length, and consequently happen to be the
      // longest ones. Additionally, there are the ones on either side that
      // are apparently not necessarily symmetric. But that's ok.

      // Place holders for the vector components and origin
      Double_t originX = 0., originY = 0., originZ = 0.;
      Double_t momentumX = 0, momentumY = 0, momentumZ = 0;
      Double_t originYSign = 1;
      if (package == 1 )
         originYSign = -1.;

      if ( wire <= 95 ) {
        if ( package == 1 ) {
           prop->SetMaxZ(fVDC_PlaneZPos[package-1][plane-1]+
                 TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.);

            originX = sign*(fVDC_WireXSpacing*+92.);
            originY = -1*fVDC_PlaneYPos[package-1][plane-1]+
               TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.-
               TMath::Sin(fAngleOfVDC)*fVDC_WireYSpacing*(95-wire);
            originZ = fVDC_PlaneZPos[package-1][plane-1]-
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.+
               (95-wire)*fVDC_WireZProjectedSpacing;
        } else if ( package == 2 ) {
            prop->SetMaxZ(fVDC_PlaneZPos[package-1][plane-1]+
                  TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.
                  -(96-wire)*fVDC_WireZProjectedSpacing);

            originX = sign*(fVDC_WireXSpacing*(92-wire));
            originY = fVDC_PlaneYPos[package-1][plane-1]-
               TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.;
            originZ = fVDC_PlaneZPos[package-1][plane-1]-
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.;
         }
      } else if ( wire <= 184 ) {
         prop->SetMaxZ(fVDC_PlaneZPos[package-1][plane-1]+
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.);

         if ( package == 1 ) {
            originX = sign*(fVDC_WireXSpacing*(92-wire)+2*fVDC_YLength);
            originY = -1*fVDC_PlaneYPos[package-1][plane-1]+
               TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.;
            originZ = fVDC_PlaneZPos[package-1][plane-1]-
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.;
         } else if ( package == 2 ) {
            originX = sign*(fVDC_WireXSpacing*(92-wire));
            originY = originYSign*fVDC_PlaneYPos[package-1][plane-1]-
               TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.;
            originZ = fVDC_PlaneZPos[package-1][plane-1]-
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.;
         }
      } else {
         prop->SetMaxZ(fVDC_PlaneZPos[package-1][plane-1]+
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.);

         if ( package == 1 ) {
            prop->SetMaxZ(fVDC_PlaneZPos[package-1][plane-1]-
                  TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.
                  +(280-wire)*fVDC_WireZProjectedSpacing);

            originX = sign*(fVDC_WireXSpacing*(187-wire));
            originY = -1*fVDC_PlaneYPos[package-1][plane-1]+
               TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.;
            originZ = fVDC_PlaneZPos[package-1][plane-1]-
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.;
        } else if ( package == 2 ) {
            originX = sign*(fVDC_WireXSpacing*-92.);
            originY = originYSign*fVDC_PlaneYPos[package-1][plane-1]-
               TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.+
               TMath::Sin(fAngleOfVDC)*fVDC_WireYSpacing*(wire-184);
            originZ = fVDC_PlaneZPos[package-1][plane-1]-
               TMath::Cos(fAngleOfVDC)*fVDC_YLength/2.+
               (wire-184)*fVDC_WireZProjectedSpacing;
         }

/*         if ( package == 1 ) {
         originY -= (originYSign*fVDC_PlaneYPos[package-1][plane-1]-
            TMath::Sin(fAngleOfVDC)*fVDC_YLength/2.);
         }*/
      }

      // If we are dealing with package 1, then we need to transpose it to
      // the bottom octant, so that once it rotates it can still be consistent
      // with the true configuration.

      // Now we set the origin of this wire, after a proper rotation.
      wireT->fV.Set(RotateXonZ(originX,originY,fPackageAngle),
            RotateYonZ(originX,originY,fPackageAngle),
            originZ);
      momentumX = sign*TMath::Cos(fVDC_AngleOfWires);
      momentumY = TMath::Sin(fVDC_AngleOfWires)*TMath::Sin(fAngleOfVDC);
      momentumZ = TMath::Sin(fVDC_AngleOfWires)*TMath::Cos(fAngleOfVDC);
      wireT->fP.Set(originYSign*RotateXonZ(momentumX,momentumY,fPackageAngle),
            originYSign*RotateYonZ(momentumX,momentumY,fPackageAngle),
            momentumZ);

      fCurrentTrack = new TEveTrack(wireT,prop);
      fCurrentTrackArray->Add(fCurrentTrack);
      fCurrentTrack->SetMainColor(plane+1);
      fCurrentTrack->SetMarkerColor(kGreen);
      fCurrentTrack->SetMarkerStyle(5);
      fCurrentTrack->SetMarkerSize(0.5);
      fCurrentTrack->SetTitle(Form("Plane: %d\nWire: %d\nPackage: %d",
            plane,wire, package));
      fEveManager->AddElement(fCurrentTrackList);
      fEveManager->AddElement(fCurrentTrack);
      fCurrentTrack->MakeTrack();
   }





  // The Following is for region 2:





//#####################################################################################
//#####################################################################################
// This is the track method.  Where wires propage 1cm in the z direction. 
// It does not work for the following reasons.  Propagator only likes to
// propagate in the positive octant of space.  Because the HDC are located
// upstream from the QTOR origin this can not work.

/*
  else  if ( region == kRegionID2 ) {
  
  // Correcting the fHDC_PlaneYPos depending on the package
   Int_t sign;
  if(package==1)
    {sign = 1;}
  else
    {sign = -1;}



  // Getting correct Y-Pos for this planes #
     Double_t fHDC_NewPlaneYPos;
    if(plane<=6)
  {fHDC_NewPlaneYPos=sign*fHDC_PlaneYPos[0];}
  else
  {fHDC_NewPlaneYPos=sign*fHDC_PlaneYPos[1];}
  




  // Horizontal Wires
   Int_t new_var=(plane+2)%3; 
    if (new_var==0) {
      prop->SetMaxZ(1);
      wireT->fV.Set(
            -fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.+(wire-1)*fHDC_WireSeparation,
            fHDC_PlaneZPos[plane-1]);
      wireT->fP.Set(fHDC_XLength/2.,
                0.,
                1.);}

  // Wires angles both positive
    else  if (plane%3==0) {
  if (wire<=16) {
     prop->SetMaxZ(1.);
     wireT->fV.Set(
              fHDC_XLength/2.-fHDC_XAsymetry+fHDC_WireXSpacing[1]*(1-wire),
              fHDC_NewPlaneYPos-fHDC_YLength/2.,
              fHDC_PlaneZPos[plane-1]);
     wireT->fP.Set(
              fHDC_XAsymetry+fHDC_WireXSpacing[1]*(wire-1),
              fHDC_YAsymetry+fHDC_WireYSpacing[1]*(wire-1),
              1);}
 else
{
    prop->SetMaxZ(1.);
    wireT->fV.Set(
              -fHDC_XLength/2.,
              fHDC_NewPlaneYPos-fHDC_YLength/2.+(fHDC_XLength-16*fHDC_WireXSpacing[1]-fHDC_XAsymetry)/(4/3.),
              fHDC_PlaneZPos[plane-1]);
    wireT->fP.Set(
              fHDC_XLength-(fHDC_YLength-16*fHDC_WireYSpacing[1]-fHDC_YAsymetry)*(4/3.)-fHDC_WireXSpacing[1]*(wire-1),
              fHDC_YLength-(fHDC_XLength-16*fHDC_WireXSpacing[1]-fHDC_XAsymetry)/(4/3.)-fHDC_WireYSpacing[1]*(wire-1),
              1);}
}


   // Last Wire Angle Position
    else {
  if (wire<=16) {
     prop->SetMaxZ(1.);
     wireT->fV.Set(
              -fHDC_XLength/2.+fHDC_XAsymetry+fHDC_WireXSpacing[1]*(1-wire),
              fHDC_NewPlaneYPos-fHDC_YLength/2,
              fHDC_PlaneZPos[plane-1]);
     wireT->fP.Set(
              -fHDC_XAsymetry-fHDC_WireXSpacing[1]*(wire-1),
              fHDC_YAsymetry+fHDC_WireYSpacing[1]*(wire-1),
              1);}
 else
{
    prop->SetMaxZ(1.);
    wireT->fV.Set(
              fHDC_XLength/2.,
              fHDC_NewPlaneYPos-fHDC_YLength/2+(fHDC_XLength-16*fHDC_WireXSpacing[1]-fHDC_XAsymetry)/(4/3.),
              fHDC_PlaneZPos[plane-1]);
    wireT->fP.Set(
              -fHDC_XLength+(fHDC_YLength-16*fHDC_WireYSpacing[1]-fHDC_YAsymetry)*(4/3.)+fHDC_WireXSpacing[1]*(wire-1),
              fHDC_YLength-(fHDC_XLength-16*fHDC_WireXSpacing[1]-fHDC_XAsymetry)/(4/3.)-fHDC_WireYSpacing[1]*(wire-1),
              1);}
}
}

 
   fCurrentTrack = new TEveTrack(wireT,prop);
   fCurrentTrackArray->Add(fCurrentTrack);
   fCurrentTrack->SetMainColor(plane+1);
   fCurrentTrack->SetMarkerColor(kGreen);
   fCurrentTrack->SetMarkerStyle(5);
   fCurrentTrack->SetMarkerSize(0.5);
   fCurrentTrack->SetTitle(Form("Plane: %d\nWire: %d\nPackage: %d",plane,wire,
            package));
   fEveManager->AddElement(fCurrentTrackList);
   fEveManager->AddElement(fCurrentTrack);
   fCurrentTrack->MakeTrack();


 }
*/
//#####################################################################################
//#####################################################################################




// This is the 'line' method:

// In region 2 there are 7 wire length types. There is the vertical type,
// 3 at one angle, and 3 at the other angle.


  else if ( region == kRegionID2 ) {  

  //Correcting the fHDC_PlaneYPos depending on the package
  Int_t sign;
  if(package==1)
    {sign = 1;}
  else
    {sign = -1;}
  
  //Getting correct Y-Pos for this planes #
  Double_t fHDC_NewPlaneYPos;
  if(plane<=6)
  {fHDC_NewPlaneYPos=sign*fHDC_PlaneYPos[0];}
  else
  {fHDC_NewPlaneYPos=sign*fHDC_PlaneYPos[1];}



 //Horizontal Wire Planes

Int_t new_var=(plane+2)%3;
    if (new_var==0)
{
ls->AddLine(-fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.+(wire-1)*fHDC_WireSeparation,
            fHDC_PlaneZPos[plane-1],
            fHDC_XLength/2.,
            fHDC_NewPlaneYPos-fHDC_YLength/2.+(wire-1)*fHDC_WireSeparation,
            fHDC_PlaneZPos[plane-1]);}


  //Wires angles both positive
 else  if (plane%3==0)
{
  if (wire<=16)
{
  ls->AddLine(fHDC_XLength/2-fHDC_XAsymetry+fHDC_WireXSpacing[1]*(1-wire),
              fHDC_NewPlaneYPos-fHDC_YLength/2., 
              fHDC_PlaneZPos[plane-1],
              fHDC_XLength/2., 
              fHDC_NewPlaneYPos-fHDC_YLength/2.+fHDC_YAsymetry+fHDC_WireYSpacing[1]*(wire-1),
              fHDC_PlaneZPos[plane-1]);}
 else
{
  ls->AddLine(-fHDC_XLength/2.,
              fHDC_NewPlaneYPos-fHDC_YLength/2+(fHDC_XLength-16*fHDC_WireXSpacing[1]-fHDC_XAsymetry)/(4/3.), 
              fHDC_PlaneZPos[plane-1],
              fHDC_XLength/2.-(fHDC_YLength-16*fHDC_WireYSpacing[1]-fHDC_YAsymetry)*(4/3.),
              fHDC_NewPlaneYPos+fHDC_YLength/2., 
              fHDC_PlaneZPos[plane-1]);}
}


  //Last Wire Angle Position
else
{  
if (wire<=16)
{
  ls->AddLine(-fHDC_XLength/2.+fHDC_XAsymetry-fHDC_WireXSpacing[1]*(1-wire),
              fHDC_NewPlaneYPos-fHDC_YLength/2.,
              fHDC_PlaneZPos[plane-1],
              -fHDC_XLength/2.,
              fHDC_NewPlaneYPos-fHDC_YLength/2.+fHDC_YAsymetry+fHDC_WireYSpacing[1]*(wire-1),
              fHDC_PlaneZPos[plane-1]);}
 else
{
  ls->AddLine(fHDC_XLength/2.,
              fHDC_NewPlaneYPos-fHDC_YLength/2.+(fHDC_XLength-16*fHDC_WireXSpacing[1]-fHDC_XAsymetry)/(4/3.),
              fHDC_PlaneZPos[plane-1],
              -fHDC_XLength/2.+(fHDC_YLength-16*fHDC_WireYSpacing[1]-fHDC_YAsymetry)*(4/3.),
              fHDC_NewPlaneYPos+fHDC_YLength/2.,
              fHDC_PlaneZPos[plane-1]);}
}
   //This is for the QwEventDisplay3D::ClearTracks() that needs a
   // dummy track to clear the track array and thus delete the line
   // after the "next" or "previous" button is pressed.
   fCurrentTrack = new TEveTrack(wireT,prop);


   //This draws the line.  (or perhaps send information to fEveManager that draws it once called upon)
   ls->SetMarkerSize(1.5);
   ls->SetMarkerStyle(4);
   ls->SetMainColor(plane+1);
   fEveManager->AddElement(ls);
   fCurrentTrackArray->Add(ls);
}
}




void QwEventDisplay3D::UpdateView()
{
   // We definitely don't want to show detectors when we are in wire view
   if( fVDC_IsWireView || fHDC_IsWireView )
      return;

   // Only way to do this is one by one...sadly.

   // Target
   SetVisibility("QweakTargetCell_Log#13805f8_116",fShowTarget);
   SetVisibility("QweakTargetWindowFront_Log#1380778_117",fShowTarget);
   SetVisibility("QweakTargetWindowBack_Log#1380948_118",fShowTarget);
   SetVisibility("QweakTargetMaterial_Log#1380c08_119",fShowTarget);

   // Collimators
   SetVisibility("CollimatorHousing_Log#138c830_225",fShowCollimator1);
   SetVisibility("CollimatorHousing_Log#138fa38_226",fShowCollimator2);
   SetVisibility("CollimatorHousing_Log#138afd0_227",fShowCollimator3);
   SetVisibility("ShieldingWallHousing_Log#13a8810_232",fShowShield);

   // GEMS
   SetVisibility("GEM_MasterContainer_Log#13a89c8_233",fShowGems);
   SetVisibility("GEM_MasterContainer_Log#13a89c8_234",fShowGems);

   // HDC
   SetVisibility("HDC_MasterContainer_Log#13aab08_235",fShowHDC);
   SetVisibility("HDC_MasterContainer_Log#13aab08_236",fShowHDC);

   // QTOR (wow...really?)
   SetVisibility("SingleCoil_Plate_Logical#1380e98_120",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_121",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_124",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_125",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_128",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_129",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_132",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_133",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_136",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_137",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_140",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_141",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_144",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_145",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_148",fShowQtor);
   SetVisibility("SingleCoil_Plate_Logical#1380e98_149",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_122",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_123",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_126",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_127",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_130",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_131",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_134",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_135",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_138",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_139",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_142",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_143",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_146",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_147",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_150",fShowQtor);
   SetVisibility("SingleCoil_Wing_Log#1380fe8_151",fShowQtor);
   SetVisibility("UpstreamSpider_Log#1385200_152",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_153",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_154",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_155",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_156",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_157",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_158",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_159",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_160",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_161",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_162",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_163",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_164",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_165",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_166",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_167",fShowQtor);
   SetVisibility("MM_SingleCoil_ClampPlate_Logical#1385388_168",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_169",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_171",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_173",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_175",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_177",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_179",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_181",fShowQtor);
   SetVisibility("MM_SingleCoil_MiniClampPlate_Logical#1386578_183",fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_170",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_172",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_174",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_176",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_178",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_180",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_182",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RightMiniClampPlate_Logical#13861d0_184",
         fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_185",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_186",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_187",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_188",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_189",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_190",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_191",fShowQtor);
   SetVisibility("MM_SingleCoil_FramePlate_Logical#13879f8_192",fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_193",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_194",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_195",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_196",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_197",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_198",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_199",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_200",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_201",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_202",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_203",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_204",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_205",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_206",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_207",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_208",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_209",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_210",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_211",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_212",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_213",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_214",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_215",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_216",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_217",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_218",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_219",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_220",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_221",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_222",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_223",
         fShowQtor);
   SetVisibility("MM_SingleCoil_RadialMountingBlock_Logical#13880e0_224",
         fShowQtor);

   // VDC
   SetVisibility("VDC_MasterContainer_Log#13abb58_237",fShowVDC);
   SetVisibility("VDC_MasterContainer_Log#13abb58_238",fShowVDC);

   //Trigger
   SetVisibility("TriggerScintillatorContainer_Logical#13ba010_248",
         fShowTrigger);

   // Cerenkov/Main Detector
   SetVisibility("CerenkovContainer_Logical#13ae0d8_240",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_241",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_242",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_243",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_244",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_245",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_245",fShowCerenkov);
   SetVisibility("CerenkovContainer_Logical#13ae0d8_246",fShowCerenkov);

   // Redraw just for good measure :)
   RedrawViews();
}

void QwEventDisplay3D::SetVisibility(const char* volname, Bool_t status)
{
   // General function to set the visibility of a given volume
   TGeoNode *node = fTopNode->GetVolume()->
      FindNode(volname);
   node->GetVolume()->SetVisibility(status);
   node->GetVolume()->VisibleDaughters(status);
}

// Various switch view functions that should hopefully be self explanatory
void QwEventDisplay3D::SwitchTarget()
{
  UpdateButtonState(fTargetButton, "Target",&fShowTarget);
}

void QwEventDisplay3D::SwitchCollimator1()
{
  UpdateButtonState(fCollimator1Button, "Collimator1",&fShowCollimator1);
}


void QwEventDisplay3D::SwitchGems()
{
  UpdateButtonState(fGemsButton, "GEMS",&fShowGems);
}


void QwEventDisplay3D::SwitchCollimator2()
{
  UpdateButtonState(fCollimator2Button, "Collimator2",&fShowCollimator2);
}


void QwEventDisplay3D::SwitchHDC()
{
  UpdateButtonState(fHDCButton, "HDC",&fShowHDC);
}


void QwEventDisplay3D::SwitchCollimator3()
{
  UpdateButtonState(fCollimator3Button, "Collimator3",&fShowCollimator3);
}


void QwEventDisplay3D::SwitchQtor()
{
  UpdateButtonState(fQtorButton, "QTOR",&fShowQtor);
}


void QwEventDisplay3D::SwitchShield()
{
  UpdateButtonState(fShieldButton, "Shield",&fShowShield);
}


void QwEventDisplay3D::SwitchVDC()
{
  UpdateButtonState(fVDCButton, "VDC",&fShowVDC);
}


void QwEventDisplay3D::SwitchTrigger()
{
  UpdateButtonState(fTriggerButton, "Trigger",&fShowTrigger);
}


void QwEventDisplay3D::SwitchScanner()
{
  UpdateButtonState(fScannerButton, "Scanner",&fShowScanner);
}


void QwEventDisplay3D::SwitchCerenkov()
{
  UpdateButtonState(fCerenkovButton, "Main Detector",&fShowCerenkov);
}

void QwEventDisplay3D::UpdateButtonState(TGTextButton *button,
      const char* text, Bool_t *state )
{
   // A function to set the overal state of the buttons which control the
   // visibility of the detectors
   *state = !(*state);
   if( *state )
      button->SetBackgroundColor(fGreen);
   else
      button->SetBackgroundColor(fRed);

   button->SetText(text);
}


void QwEventDisplay3D::MenuEvent(Int_t menuID)
{
   // Signal for the Menu Items. Don't forget to update this if you add new
   // menus!
   switch(menuID) {
      case kOpenRootFile:
         OpenRootFile();
         break;
      default:
         std::cout  << "Behold! I am ROOT!. You have clicked on menu item "
            << menuID << ". I have gracefully ignored your request. "
            << "How does this make you feel? Feel freel to discuss your "
            << "feelings with the authors of this program.\n";
         break;
   }
}


void QwEventDisplay3D::OpenRootFile()
{
   // Open and parse a root file for valid hits
   TGFileInfo fileInfo;
   fileInfo.fFileTypes = fFiletypes;
   std::cout << getenv("QW_ROOTFILES") << "\n";
   // TODO: Find out why setting the initial directory produces a severe crash
   TString iniDir(gSystem->Getenv("QW_ROOTFILES"));
   //fileInfo.fIniDir = getenv("QW_ROOTFILES");
   new TGFileDialog(gClient->GetRoot(),
         this, kFDOpen, &fileInfo);

   // Make sure the file selected is valid before we set it
   TFile *tempFile = new TFile(fileInfo.fFilename);
   if( tempFile->IsOpen() ) {
      delete fRootFile;
      fRootFile = tempFile;

      // Initialize the events
      InitEvents();
   }
}

void QwEventDisplay3D::DetectorButtonsEnable(Bool_t status)
{
   // Enable or disable the detector buttons depending on the view
   fUpdateViewButton->SetEnabled(status);
   fTargetButton->SetEnabled(status);
   fCollimator1Button->SetEnabled(status);
   fGemsButton->SetEnabled(status);
   fCollimator2Button->SetEnabled(status);
   fHDCButton->SetEnabled(status);
   fCollimator3Button->SetEnabled(status);
   fQtorButton->SetEnabled(status);
   fShieldButton->SetEnabled(status);
   fVDCButton->SetEnabled(status);
   fTriggerButton->SetEnabled(status);
   fScannerButton->SetEnabled(status);
   fCerenkovButton->SetEnabled(status);
}


void QwEventDisplay3D::SkipToEvent()
{
   // Skip to the specified event in the line edit next to this button
   Int_t event = atoi(fSkipToEventLine->GetText());

   // Sometimes people put in the strangest requests. Check to make sure
   // we aren't already at that event!
   if( event != fCurrentEvent) {
      // Now are we ahead or behind?
      if( event < fCurrentEvent ) { // Ahead of the event
         while (fCurrentEvent>event && fCurrentEvent>0) {
            PreviousEvent(kFALSE);
         }
         // Because of the way we do this we pass it by a bit and will
         // have to retract one step.
         NextEvent(kFALSE);
         PreviousEvent(kTRUE);
      } else { // Behind the event
         while (fCurrentEvent < event && fCurrentEvent<fNumberOfEvents ){
            NextEvent(kFALSE);
         }
         // Because of the way we do this we pass it by a bit and will
         // have to retract one step.
         PreviousEvent(kFALSE);
         NextEvent(kTRUE);
      }
   }
}

void QwEventDisplay3D::OpenRoot(Int_t runnumber)
{
   TFile *tempFile = new TFile(Form("%s/Qweak_%d.root",getenv("QW_ROOTFILES"),
            runnumber));
   if(tempFile->IsOpen() ) {
      delete fRootFile;
      fRootFile = tempFile;

      InitEvents();
   }

}

Double_t QwEventDisplay3D::RotateXonZ(Double_t x, Double_t y, Double_t angle)
{
   // Rotate X about the Z axis by the given angle
   return x*TMath::Cos(angle)-y*TMath::Sin(angle);
}


Double_t QwEventDisplay3D::RotateYonZ(Double_t x, Double_t y, Double_t angle)
{
   // Rotate Y about the Z axis by the given angle
   return x*TMath::Sin(angle)+y*TMath::Cos(angle);
}

void QwEventDisplay3D::SetRotation( Double_t phi )
{
   //! This defines the rotation of the tracking system. Where the octant for
   //! package (1,2) or (up,down) are:
   //!   0deg : (5,1)
   //! +45deg : (6,2)
   //! +90deg : (7,3)
   //! -45deg : (4,8)
   //! -90deg : (3,7)
   //! The way I draw the wires, they are all calculated for a position
   //! assuming phi = +90deg, and then rotated from there by a given
   //! fPackageAngle. From there, if the package is 1, it is translated
   //! towards its correct side.
   fDetectorPhi = phi;
   switch( (int)phi ) {
      case -90:
         fPackageAngle = -_180DEG_;
         break;
      case -45:
         fPackageAngle = -1*_135DEG_;
         break;
      case 90:
         fPackageAngle = 0;
         break;
      case 45:
         fPackageAngle = -1*_45DEG_;
         break;
      case 0:
      default:
         fPackageAngle = -1*_90DEG_;
         break;
   }

   //std::cout << "Initialized the run with rotation of coordinate system to to "
      //<< phi << ".\n The respective packages are: Package1: " << fPackageAngle[0] << "\tPackage2: " << fPackageAngle[1] << "\n.";
}

// END OF THE FILE
////////////////////////////////////////////////////////////////////////////////
// Notes: sample rotation of detectors. Hold on to it for now, will delete it later.
//fTopNode->GetVolume()->FindNode("VDC_MasterContainer_Log#13abb58_238")->GetMatrix()->RotateZ(90.);
