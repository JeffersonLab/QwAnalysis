// Updated by Derek Jones 7-26-10
// Much of code is copied from QwEventDisplay.cc

#include <QwGUIEventDisplay.h>
#include <iostream>
#include "TLine.h"

//Qweak Header Files
#include "QwDetectorInfo.h"  // establishes detector information
#include "QwHitContainer.h"  // creates augmented vector of QwHits for special functionality
#include "QwHitRootContainer.h"

ClassImp(QwGUIEventDisplay);

QwGUIEventDisplay::QwGUIEventDisplay(
	const TGWindow *p,
	const TGWindow *main,
	const TGTab *tab,
	const char *objName,
	const char *mainname,
	UInt_t w, ///< width
	UInt_t h) ///< heigth
  : QwGUISubSystem(p, main, tab, objName, mainname, w, h)
{

  fTree = NULL;
  fEventNumber = 0;

  AddThisTab(this);
}

QwGUIEventDisplay::~QwGUIEventDisplay()
{
  fMain->Cleanup();

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIEventDisplay::MakeLayout()
{

   // Formatting variables
   ULong_t ucolor; // Will reflect user color changes
   TGFont *ufont;  // Will reflect user font changes
   TGGC   *uGC;    // Will reflect user GC changes

   // Initialize variables to 0
   fEventNumber = 0;    // Current event number
   fTreeEntries = 0;    // Number of entries in tree
   fSubsystemArray = 0; // Organizes subsystems
   fEventBuffer = 0;    // Houses event information
   fHitList = 0;        // Houses hit information

   // Create new pop up window on screen; main frame
   fMain = new TGVerticalFrame(this,10,10, kLHintsExpandX, kLHintsExpandY);
   fMain->SetLayoutBroken(kTRUE);
   fMain->SetWindowName("Qweak 2D Single Event Display");


   ////MENU BAR----------------------------------------------------------------------------------------------------////

   // Create menu bar for file and help functions 
   fMenuBarLayout = new TGLayoutHints(kLHintsTop, kLHintsExpandX);                    // Layout for the menu bar
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight | kRaisedFrame);  // Layout for Help popup menu

   fMenuHelp = new TGPopupMenu(gClient->GetRoot());    // Add a popup menu for help options
   fMenuHelp->AddEntry("&Tutorial", M_HELP_TUTORIAL);  // Add option to connect to the event display tutorial documentation
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);        // Add option to print information about the event display

   fMenuHelp->Connect("Activated(Int_t)", "QwGUIEventDisplay", this, "HandleMenu(Int_t)");

   fMenuBar = new TGMenuBar(fMain, kLHintsTop | kLHintsExpandX);  // Create a menu bar
    fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);    // Add a help popup menu
   gClient->GetColorByName("#deba87", ucolor);                    // Set ucolor to mute orange
   fMenuBar->SetBackgroundColor(ucolor);

   fMain->AddFrame(fMenuBar, fMenuBarLayout);  // Add menu bar to the main frame
   fMenuBar->MoveResize(0,0,900,20);


   ////TITLE AND LOGOS----------------------------------------------------------------------------------------------------////

   // Create horizontal frame for logo and title placement: includes Qweak and GWU logos
   gClient->GetColorByName("#ffffff",ucolor);  // Set ucolor to white
   TGHorizontalFrame *fLogos = new TGHorizontalFrame(fMain,900,80,kHorizontalFrame | kRaisedFrame,ucolor);
   fLogos->SetLayoutBroken(kTRUE);
   // Insert Qweak logo
   TGIcon *QweakLogo = new TGIcon(fLogos, getenv_safe_TString("QWANALYSIS") + "/Extensions/Logos/qweak.jpg");
   fLogos->AddFrame(QweakLogo, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   QweakLogo->MoveResize(0,1,140,77);

   // Graphics context changes for "Qweak 2D Single Event Display" title label
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-34-*-*-*-*-*-*-*"); // Set ufont to bold helvetica 25
   GCValues_t valBox;
   valBox.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valBox.fForeground);
   gClient->GetColorByName("#c0c0c0",valBox.fBackground);
   valBox.fFillStyle = kFillSolid;
   valBox.fFont = ufont->GetFontHandle();
   valBox.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valBox, kTRUE);

   // Create title box for event display
   gClient->GetColorByName("#d45954",ucolor);  // Set ucolor to soft red
   TGLabel *TitleBox = new TGLabel(fLogos,"Qweak 2D Single Event Display",uGC->GetGC(),ufont->GetFontStruct(),kRaisedFrame,ucolor);
   TitleBox->SetTextJustify(36);
   TitleBox->SetMargins(0,0,0,0);
   TitleBox->SetWrapLength(-1);
   fLogos->AddFrame(TitleBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   TitleBox->MoveResize(143,1,627,77);

   // Insert GWU logo
   TGIcon *GWULogo = new TGIcon(fLogos, getenv_safe_TString("QWANALYSIS") + "/Extensions/Logos/gwu_logo_main.gif");
   fLogos->AddFrame(GWULogo, new TGLayoutHints(kLHintsNormal));
   GWULogo->MoveResize(772,1,135,77);

   // Add logos frame to main frame
   fMain->AddFrame(fLogos, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLogos->MoveResize(0,20,900,100);


   ////EVENT BOXES----------------------------------------------------------------------------------------------------////

   // Create horizontal frame for event boxes including current event number, goto event function, wire hits information, and octant identification
   gClient->GetColorByName("#0047ab",ucolor);  // Set ucolor to blue
   fEventBoxes = new TGHorizontalFrame(fMain,900,240,kHorizontalFrame,ucolor);
   fEventBoxes->SetLayoutBroken(kTRUE);

   //EVENT BOX 1--Current Event Number and Goto Event--------------------------------------------------//

   // Create vertical frame for event box 1
   gClient->GetColorByName("#d4cf87",ucolor); // Set ucolor to buff
   fEventBox1 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox1->SetLayoutBroken(kTRUE);

   // Graphics context changes for "Event Counter" label
   gClient->GetColorByName("#deba87", ucolor);                         // Set ucolor to mute orange
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // Set ufont to bold charter 15
   GCValues_t valunterLabel;
   valunterLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valunterLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valunterLabel.fBackground);
   valunterLabel.fFillStyle = kFillSolid;
   valunterLabel.fFont = ufont->GetFontHandle();
   valunterLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valunterLabel, kTRUE);

   // Create label for the event counter
   TGLabel *EventCounterLabel = new TGLabel(fEventBox1,"Event Counter",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   EventCounterLabel->SetTextJustify(36);
   EventCounterLabel->SetMargins(0,0,0,0);
   EventCounterLabel->SetWrapLength(-1);
   fEventBox1->AddFrame(EventCounterLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   EventCounterLabel->MoveResize(45,6,125,20);
   EventCounterLabel->SetBackgroundColor(ucolor);

   // Create vertical frame for event counter information: current event number, goto event number entry, and goto event button
   gClient->GetColorByName("#507ba7", ucolor); // Set ucolor to navy blue
   fEventCounter = new TGVerticalFrame(fEventBox1,144,176,kVerticalFrame | kSunkenFrame, ucolor);
   fEventCounter->SetLayoutBroken(kTRUE);

   // Graphics context changes for "Current Event" group frame
   ufont = gClient->GetFont("-*-courier-bold-r-*-*-12-*-*-*-*-*-*-*"); // Set ufont to bold charter 10
   GCValues_t valentHit;
   valentHit.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valentHit.fForeground);
   gClient->GetColorByName("#c0c0c0",valentHit.fBackground);
   valentHit.fFillStyle = kFillSolid;
   valentHit.fFont = ufont->GetFontHandle();
   valentHit.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valentHit, kTRUE);

   // Create "Current Event" group frame
   fCurrentEvent = new TGGroupFrame(fEventCounter,"Current Event",kVerticalFrame,uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   fCurrentEvent->SetLayoutBroken(kTRUE);

   // Graphics context changes for "Current Event" label
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-33-*-*-*-*-*-*-*"); // Set ufont to bold charter 33
   GCValues_t valntHitLabel;
   valntHitLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valntHitLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valntHitLabel.fBackground);
   valntHitLabel.fFillStyle = kFillSolid;
   valntHitLabel.fFont = ufont->GetFontHandle();
   valntHitLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valntHitLabel, kTRUE);
   gClient->GetColorByName("#fff897",ucolor); // Set ucolor to mute yellow

   // Create label for current event: variable changes based on fEventNumber
   fCurrentEventLabel = new TGLabel(fCurrentEvent, Form("%u", (Int_t) fEventNumber), uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   fCurrentEventLabel->SetTextJustify(36);
   fCurrentEventLabel->SetMargins(0,0,0,0);
   fCurrentEventLabel->SetWrapLength(-1);
   fCurrentEventLabel->MoveResize(11,18,104,32);
   fCurrentEventLabel->SetBackgroundColor(ucolor);

   // Add current event label to "Current Event" group frame
   fCurrentEvent->AddFrame(fCurrentEventLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCurrentEvent->SetLayoutManager(new TGVerticalLayout(fCurrentEvent));

   // Add "Current Event" group frame to event counter frame
   fEventCounter->AddFrame(fCurrentEvent, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCurrentEvent->MoveResize(8,8,126,64);

   // Graphics context changes for "Goto Event" button
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-11-*-*-*-*-*-*-*"); // Set ufont to bold helvetica 11
   GCValues_t valitButton;
   valitButton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valitButton.fForeground);
   gClient->GetColorByName("#c0c0c0",valitButton.fBackground);
   valitButton.fFillStyle = kFillSolid;
   valitButton.fFont = ufont->GetFontHandle();
   valitButton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valitButton, kTRUE);

   // Create goto event button
   gClient->GetColorByName("#d4cf87",ucolor); // Set ucolor to buff
   TGTextButton *GotoEventButton = new TGTextButton(fEventCounter,"&Goto Event",-1,uGC->GetGC(),ufont->GetFontStruct());
   GotoEventButton->Connect("Clicked()", "QwGUIEventDisplay", this, "GotoEvent()");
   GotoEventButton->SetTextJustify(36);
   GotoEventButton->SetMargins(0,0,0,0);
   GotoEventButton->SetWrapLength(-1);
   GotoEventButton->Resize(67,24);
   GotoEventButton->ChangeBackground(ucolor);

   // Add goto event button to event counter frame
   fEventCounter->AddFrame(GotoEventButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   GotoEventButton->MoveResize(64,133,67,24);

   // Create number entry for goto event function
   fCurrentEventEntry = new TGNumberEntry(fEventCounter, (Double_t) 0,11,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
   fEventCounter->AddFrame(fCurrentEventEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCurrentEventEntry->MoveResize(24,94,96,22);

   // Add event counter frame to event box 1
   fEventBox1->AddFrame(fEventCounter, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventCounter->MoveResize(35,32,144,180);

   // Add event box 1 to event box frame
   fEventBoxes->AddFrame(fEventBox1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBox1->MoveResize(3,6,220,226);

   //EVENT BOX 2--Wire Hit Information--------------------------------------------------//

   // Create vertical frame for event box 2
   gClient->GetColorByName("#d4cf87",ucolor); // Set ucolor to buff
   fEventBox2 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox2->SetLayoutBroken(kTRUE);

   // Create list box for wire hit information
   fWireInfoListBox = new TGListBox(fEventBox2);
   fWireInfoListBox->Resize(200,180);
   fEventBox2->AddFrame(fWireInfoListBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fWireInfoListBox->MoveResize(8,32,428,180);
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // Set font to bold charter 15

   // Graphics context changes for "Wire Hit Information" label
   GCValues_t valitsLabel;
   valitsLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valitsLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valitsLabel.fBackground);
   valitsLabel.fFillStyle = kFillSolid;
   valitsLabel.fFont = ufont->GetFontHandle();
   valitsLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valitsLabel, kTRUE);
   gClient->GetColorByName("#deba87", ucolor); // Set ucolor to mute orange

   // Create label for event box 2
   TGLabel *fWireInfoLabel = new TGLabel(fEventBox2,"Wire Hit Information",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   fWireInfoLabel->SetTextJustify(36);
   fWireInfoLabel->SetMargins(0,0,0,0);
   fWireInfoLabel->SetWrapLength(-1);
   fWireInfoLabel->MoveResize(129,6,196,20);
   fWireInfoLabel->SetBackgroundColor(ucolor);
   fEventBox2->AddFrame(fWireInfoLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // Add event box 2 to event box frame
   fEventBoxes->AddFrame(fEventBox2, new TGLayoutHints(kLHintsNormal));
   fEventBox2->MoveResize(227,6,444,226);

   //EVENT BOX 3--Octant Identification--------------------------------------------------//

   // Create vertical frame for event box 3
   gClient->GetColorByName("#d4cf87",ucolor); // Set ucolor to buff
   fEventBox3 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox3->SetLayoutBroken(kTRUE);

   // Graphics context changes for "Octant Identification" label
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // Set ufont to bold charter 15
   GCValues_t valInfoLabel;
   valInfoLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valInfoLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valInfoLabel.fBackground);
   valInfoLabel.fFillStyle = kFillSolid;
   valInfoLabel.fFont = ufont->GetFontHandle();
   valInfoLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valInfoLabel, kTRUE);
   gClient->GetColorByName("#deba87", ucolor); // Set ucolor to mute orange

   // Create label for event box 3
   TGLabel *fOctantLabel = new TGLabel(fEventBox3,"Octant Identification",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   fOctantLabel->SetTextJustify(36);
   fOctantLabel->SetMargins(0,0,0,0);
   fOctantLabel->SetWrapLength(-1);
   fOctantLabel->MoveResize(25,6,175,20);
   fOctantLabel->SetBackgroundColor(ucolor);
   fEventBox3->AddFrame(fOctantLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // Create canvas for octant identification
   fOctantID = new TRootEmbeddedCanvas(0, fEventBox3, 200, 180);
   Int_t wOctantID = fOctantID->GetCanvasWindowId();
   cOctantID = new TCanvas("cOctantID", 10, 10, wOctantID);
   fOctantID->AdoptCanvas(cOctantID);
   fEventBox3->AddFrame(fOctantID, new TGLayoutHints(kVerticalFrame | kSunkenFrame));
   fOctantID->MoveResize(22,32,180,180);
   fOctantID->GetCanvas()->SetFillColor(kAzure-7);
   Octant_1 = new TPaveLabel(.4,.75,.6,.95, "3");        // Create labels for each octant
   Octant_1->SetLineColor(1);
   Octant_1->SetFillColor(0);
   Octant_1->Draw();
   Octant_2 = new TPaveLabel(.675,.675,.875,.875, "4");
   Octant_2->SetLineColor(1);
   Octant_2->SetFillColor(0);
   Octant_2->Draw();
   Octant_3 = new TPaveLabel(.75,.4,.95,.6, "5");
   Octant_3->SetLineColor(1);
   Octant_3->SetFillColor(0);
   Octant_3->Draw();
   Octant_4 = new TPaveLabel(.675,.125,.875,.325, "6");
   Octant_4->SetLineColor(1);
   Octant_4->SetFillColor(0);
   Octant_4->Draw();
   Octant_5 = new TPaveLabel(.4,.05,.6,.25, "7");
   Octant_5->SetLineColor(1);
   Octant_5->SetFillColor(0);
   Octant_5->Draw();
   Octant_6 = new TPaveLabel(.125,.125,.325,.325, "8");
   Octant_6->SetLineColor(1);
   Octant_6->SetFillColor(0);
   Octant_6->Draw();
   Octant_7 = new TPaveLabel(.05,.4,.25,.6, "1");
   Octant_7->SetLineColor(1);
   Octant_7->SetFillColor(0);
   Octant_7->Draw();
   Octant_8 = new TPaveLabel(.125,.675,.325,.875, "2");
   Octant_8->SetLineColor(1);
   Octant_8->SetFillColor(0);
   Octant_8->Draw();
   fOctantID->GetCanvas()->SetEditable(kFALSE);

   // Add event box 3 to event box frame
   fEventBoxes->AddFrame(fEventBox3, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBox3->MoveResize(675,6,222,226);

   // Add event box frame to main frame
   fMain->AddFrame(fEventBoxes, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBoxes->MoveResize(0,100,900,260);


   ////REGION BOXES----------------------------------------------------------------------------------------------------////

   // Graphics context changes for region tabs
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // Set ufont to bold charter 10
   GCValues_t valTabs;
   valTabs.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valTabs.fForeground);
   gClient->GetColorByName("#c0c0c0",valTabs.fBackground);
   valTabs.fFillStyle = kFillSolid;
   valTabs.fFont = ufont->GetFontHandle();
   valTabs.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valTabs, kTRUE);
   gClient->GetColorByName("#0047ab",ucolor); // Set ucolor to blue

   // Create region boxes tab frame
   fRegions = new TGTab(fMain,900,312,uGC->GetGC(),ufont->GetFontStruct(),kChildFrame,ucolor);
   gClient->GetColorByName("#d4cf87",ucolor); // Set ucolor to buff

   //REGION 1--Gas Electron Multiplier--------------------------------------------------//

   // Create tab and container of "Region 1" data
   fRegion1 = fRegions->AddTab("Region 1--GEM");
   fRegion1->SetLayoutManager(new TGMatrixLayout(fRegion1,0,1,0,0));
   fRegion1->SetLayoutBroken(kTRUE);
   fRegion1->SetBackgroundColor(ucolor);

   // Create embedded canvas for Region 1 XY view
   fRegion1XY = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1XY = fRegion1XY->GetCanvasWindowId();
   cR1XY = new TCanvas("cR1XY", 10, 10, wRegion1XY);
   fRegion1XY->AdoptCanvas(cR1XY);
   fRegion1->AddFrame(fRegion1XY, new TGLayoutHints(kLHintsNormal));
   fRegion1XY->MoveResize(12,4,280,280);
   fRegion1XY->GetCanvas()->SetFillColor(0);
   Label_R1XY = new TPaveLabel(.03,.94,.97,.99,"Front View (X-Y Projection)"); // Create canvas label
   Label_R1XY->Draw();
   Box_R1XY = new TBox(.5-(R1_WIDTH*R1_CM*.5), .5-(R1_LENGTH*R1_CM*.5), .5+(R1_WIDTH*R1_CM*.5), .5+(R1_LENGTH*R1_CM*.5)); // Create and center box to frame wire lines
   Box_R1XY->SetLineColor(1);
   Box_R1XY->SetFillStyle(0);
   Box_R1XY->Draw();
   fRegion1XY->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 1 YZ view
   fRegion1YZ = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1YZ = fRegion1YZ->GetCanvasWindowId();
   cR1YZ = new TCanvas("cR1YZ", 10, 10, wRegion1YZ);
   fRegion1YZ->AdoptCanvas(cR1YZ);
   fRegion1->AddFrame(fRegion1YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion1YZ->MoveResize(306,4,280,280);
   fRegion1YZ->GetCanvas()->SetFillColor(0);
   Label_R1YZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Y-Z Projection)"); // Create canvas label
   Label_R1YZ->Draw();
   Box_R1YZ = new TBox(.5-(R1_WIDTH*R1_CM*.5), .5-(R1_DEPTH*R1_CM*.5), .5+(R1_WIDTH*R1_CM*.5), .5+(R1_DEPTH*R1_CM*.5)); // Create and center box to frame wire lines
   Box_R1YZ->SetLineColor(1);
   Box_R1YZ->SetFillStyle(0);
   Box_R1YZ->Draw();
   fRegion1YZ->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 1 XZ view
   fRegion1XZ = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1XZ = fRegion1XZ->GetCanvasWindowId();
   cR1XZ = new TCanvas("cR1XZ", 10, 10, wRegion1XZ);
   fRegion1XZ->AdoptCanvas(cR1XZ);
   fRegion1->AddFrame(fRegion1XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion1XZ->MoveResize(603,4,280,280);
   fRegion1XZ->GetCanvas()->SetFillColor(0);
   Label_R1XZ = new TPaveLabel(.03,.94,.97,.99,"Side View (X-Z Projection)"); // Create canvas label
   Label_R1XZ->Draw();
   Box_R1XZ = new TBox(.5-(R1_DEPTH*R1_CM*.5), .5-(R1_LENGTH*R1_CM*.5), .5+(R1_DEPTH*R1_CM*.5), .5+(R1_LENGTH*R1_CM*.5)); // Create and center box to frame wire lines
   Box_R1XZ->SetLineColor(1);
   Box_R1XZ->SetFillStyle(0);
   Box_R1XZ->Draw();
   fRegion1XZ->GetCanvas()->SetEditable(kFALSE);

   //REGION 2--Horizontal Drift Chamber--------------------------------------------------//

   // Create tab and container of "Region 2" data in chambers 1 through 4
   fRegion2 = fRegions->AddTab("Region 2--HDC (1-4)");
   fRegion2->SetLayoutManager(new TGVerticalLayout(fRegion2));
   fRegion2->SetLayoutBroken(kTRUE);
   fRegion2->SetBackgroundColor(ucolor);

   // Create embedded canvas for Region 2 XY view
   fRegion2XY = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2XY = fRegion2XY->GetCanvasWindowId();
   cR2XY = new TCanvas("cR2XY", 10, 10, wRegion2XY);
   fRegion2XY->AdoptCanvas(cR2XY);
   fRegion2->AddFrame(fRegion2XY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2XY->MoveResize(12,4,280,280);
   fRegion2XY->GetCanvas()->SetFillColor(0);
   Label_R2XY = new TPaveLabel(.03,.94,.97,.99,"Front View (X-Y Projection)"); // Create canvas label
   Label_R2XY->Draw();
   Box_R2XY = new TBox(.5-(R2_WIDTH*R2_CM*.5), .5-(R2_LENGTH*R2_CM*.5), .5+(R2_WIDTH*R2_CM*.5), .5+(R2_LENGTH*R2_CM*.5)); // Create and center box to frame wire lines
   Box_R2XY->SetLineColor(1);
   Box_R2XY->SetFillStyle(0);
   Box_R2XY->Draw();
   fRegion2XY->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 2 YZ view
   fRegion2YZ = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2YZ = fRegion2YZ->GetCanvasWindowId();
   cR2YZ = new TCanvas("cR2YZ", 10, 10, wRegion2YZ);
   fRegion2YZ->AdoptCanvas(cR2YZ);
   fRegion2->AddFrame(fRegion2YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2YZ->MoveResize(306,4,280,280);
   fRegion2YZ->GetCanvas()->SetFillColor(0);
   Label_R2YZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Y-Z Projection)"); // Create canvas label
   Label_R2YZ->Draw();
   Box_R2YZ1 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5),.5 - (1.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM)); // Create and place box for chamber 1
   Box_R2YZ1->SetLineColor(1);
   Box_R2YZ1->SetFillStyle(0);
   Box_R2YZ1->Draw();
   Box_R2YZ2 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM)); // Create and place box for chamber 2
   Box_R2YZ2->SetLineColor(7);
   Box_R2YZ2->SetFillStyle(0);
   Box_R2YZ2->Draw();
   Box_R2YZ3 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM)); // Create and place box for chamber 3
   Box_R2YZ3->SetLineColor(6);
   Box_R2YZ3->SetFillStyle(0);
   Box_R2YZ3->Draw();
   Box_R2YZ4 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM)); // Create and place box for chamber 4
   Box_R2YZ4->SetLineColor(8);
   Box_R2YZ4->SetFillStyle(0);
   Box_R2YZ4->Draw();
   tR2XY1 = new TText(.03, .03, "Cham. 1"); // Color-coded text for HDC chamber labels
   tR2XY1->SetTextColor(1);
   tR2XY1->Draw();
   tR2XY2 = new TText(.29, .03, "Cham. 2");
   tR2XY2->SetTextColor(7);
   tR2XY2->Draw();
   tR2XY3 = new TText(.55, .03, "Cham. 3");
   tR2XY3->SetTextColor(6);
   tR2XY3->Draw();
   tR2XY4 = new TText(.81, .03, "Cham. 4");
   tR2XY4->SetTextColor(8);
   tR2XY4->Draw();
   fRegion2YZ->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 2 XZ view
   fRegion2XZ = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2XZ = fRegion2XZ->GetCanvasWindowId();
   cR2XZ = new TCanvas("cR2XZ",10, 10, wRegion2XZ);
   fRegion2XZ->AdoptCanvas(cR2XZ);
   fRegion2->AddFrame(fRegion2XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2XZ->MoveResize(603,4,280,280);
   fRegion2XZ->GetCanvas()->SetFillColor(0);
   Label_R2XZ = new TPaveLabel(.03,.94,.97,.99,"Side View (X-Z Projection)"); // Create canvas label
   Label_R2XZ->Draw();
   Box_R2XZ1 = new TBox(.5 + (1.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 1
   Box_R2XZ1->SetLineColor(8);
   Box_R2XZ1->SetFillStyle(0);
   Box_R2XZ1->Draw();
   Box_R2XZ2 = new TBox(.5 + (.5*R2_DIST*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 2
   Box_R2XZ2->SetLineColor(6);
   Box_R2XZ2->SetFillStyle(0);
   Box_R2XZ2->Draw();
   Box_R2XZ3 = new TBox(.5 - (.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 3
   Box_R2XZ3->SetLineColor(7);
   Box_R2XZ3->SetFillStyle(0);
   Box_R2XZ3->Draw();
   Box_R2XZ4 = new TBox(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 - (1.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 4
   Box_R2XZ4->SetLineColor(1);
   Box_R2XZ4->SetFillStyle(0);
   Box_R2XZ4->Draw();
   tR2XZ1 = new TText(.03, .03, "Cham. 1"); // Color-coded text for HDC chamber labels
   tR2XZ1->SetTextColor(1);
   tR2XZ1->Draw();
   tR2XZ2 = new TText(.29, .03, "Cham. 2");
   tR2XZ2->SetTextColor(7);
   tR2XZ2->Draw();
   tR2XZ3 = new TText(.55, .03, "Cham. 3");
   tR2XZ3->SetTextColor(6);
   tR2XZ3->Draw();
   tR2XZ4 = new TText(.81, .03, "Cham. 4");
   tR2XZ4->SetTextColor(8);
   tR2XZ4->Draw();
   fRegion2XZ->GetCanvas()->SetEditable(kFALSE);

   //REGION 2b--Horizontal Drift Chamber (2nd Arm)--------------------------------------------------//

   // Create tab and container of "Region 2" data in chambers 5 through 8
   fRegion2b = fRegions->AddTab("Region 2--HDC (5-8)");
   fRegion2b->SetLayoutManager(new TGVerticalLayout(fRegion2));
   fRegion2b->SetLayoutBroken(kTRUE);
   fRegion2b->SetBackgroundColor(ucolor);

   // Create embedded canvas for Region 2b XY view
   fRegion2bXY = new TRootEmbeddedCanvas(0,fRegion2b,280,280);
   Int_t wRegion2bXY = fRegion2bXY->GetCanvasWindowId();
   cR2bXY = new TCanvas("cR2bXY", 10, 10, wRegion2bXY);
   fRegion2bXY->AdoptCanvas(cR2bXY);
   fRegion2b->AddFrame(fRegion2bXY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2bXY->MoveResize(12,4,280,280);
   fRegion2bXY->GetCanvas()->SetFillColor(0);
   Label_R2bXY = new TPaveLabel(.03,.94,.97,.99,"Front View (X-Y Projection)"); // Create canvas label
   Label_R2bXY->Draw();
   Box_R2bXY = new TBox(.5-(R2_WIDTH*R2_CM*.5), .5-(R2_LENGTH*R2_CM*.5), .5+(R2_WIDTH*R2_CM*.5), .5+(R2_LENGTH*R2_CM*.5)); // Create and center box to frame wire lines
   Box_R2bXY->SetLineColor(1);
   Box_R2bXY->SetFillStyle(0);
   Box_R2bXY->Draw();
   fRegion2bXY->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 2b YZ view
   fRegion2bYZ = new TRootEmbeddedCanvas(0,fRegion2b,280,280);
   Int_t wRegion2bYZ = fRegion2bYZ->GetCanvasWindowId();
   cR2bYZ = new TCanvas("cR2bYZ", 10, 10, wRegion2bYZ);
   fRegion2bYZ->AdoptCanvas(cR2bYZ);
   fRegion2b->AddFrame(fRegion2bYZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2bYZ->MoveResize(306,4,280,280);
   fRegion2bYZ->GetCanvas()->SetFillColor(0);
   Label_R2bYZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Y-Z Projection)"); // Create canvas label
   Label_R2bYZ->Draw();
   Box_R2bYZ1 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5),.5 - (1.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM)); // Create and place box for chamber 5
   Box_R2bYZ1->SetLineColor(1);
   Box_R2bYZ1->SetFillStyle(0);
   Box_R2bYZ1->Draw();
   Box_R2bYZ2 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM)); // Create and place box for chamber 6
   Box_R2bYZ2->SetLineColor(7);
   Box_R2bYZ2->SetFillStyle(0);
   Box_R2bYZ2->Draw();
   Box_R2bYZ3 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM)); // Create and place box for chamber 7
   Box_R2bYZ3->SetLineColor(6);
   Box_R2bYZ3->SetFillStyle(0);
   Box_R2bYZ3->Draw();
   Box_R2bYZ4 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM)); // Create and place box for chamber 8
   Box_R2bYZ4->SetLineColor(8);
   Box_R2bYZ4->SetFillStyle(0);
   Box_R2bYZ4->Draw();
   tR2bXY1 = new TText(.03, .03, "Cham. 5"); // color-coded text for HDC chamber labels
   tR2bXY1->SetTextColor(1);
   tR2bXY1->Draw();
   tR2bXY2 = new TText(.29, .03, "Cham. 6");
   tR2bXY2->SetTextColor(7);
   tR2bXY2->Draw();
   tR2bXY3 = new TText(.55, .03, "Cham. 7");
   tR2bXY3->SetTextColor(6);
   tR2bXY3->Draw();
   tR2bXY4 = new TText(.81, .03, "Cham. 8");
   tR2bXY4->SetTextColor(8);
   tR2bXY4->Draw();
   fRegion2bYZ->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 2b XZ view
   fRegion2bXZ = new TRootEmbeddedCanvas(0,fRegion2b,280,280);
   Int_t wRegion2bXZ = fRegion2bXZ->GetCanvasWindowId();
   cR2bXZ = new TCanvas("cR2bXZ",10, 10, wRegion2bXZ);
   fRegion2bXZ->AdoptCanvas(cR2bXZ);
   fRegion2b->AddFrame(fRegion2bXZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2bXZ->MoveResize(603,4,280,280);
   fRegion2bXZ->GetCanvas()->SetFillColor(0);
   Label_R2bXZ = new TPaveLabel(.03,.94,.97,.99,"Side View (X-Z Projection)"); // Create canvas label
   Label_R2bXZ->Draw();
   Box_R2bXZ1 = new TBox(.5 + (1.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 5
   Box_R2bXZ1->SetLineColor(8);
   Box_R2bXZ1->SetFillStyle(0);
   Box_R2bXZ1->Draw();
   Box_R2bXZ2 = new TBox(.5 + (.5*R2_DIST*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 6
   Box_R2bXZ2->SetLineColor(6);
   Box_R2bXZ2->SetFillStyle(0);
   Box_R2bXZ2->Draw();
   Box_R2bXZ3 = new TBox(.5 - (.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 7
   Box_R2bXZ3->SetLineColor(7);
   Box_R2bXZ3->SetFillStyle(0);
   Box_R2bXZ3->Draw();
   Box_R2bXZ4 = new TBox(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 - (1.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // Create and place box for chamber 8
   Box_R2bXZ4->SetLineColor(1);
   Box_R2bXZ4->SetFillStyle(0);
   Box_R2bXZ4->Draw();
   tR2bXZ1 = new TText(.03, .03, "Cham. 5"); // Color-coded text for HDC chamber labels
   tR2bXZ1->SetTextColor(1);
   tR2bXZ1->Draw();
   tR2bXZ2 = new TText(.29, .03, "Cham. 6");
   tR2bXZ2->SetTextColor(7);
   tR2bXZ2->Draw();
   tR2bXZ3 = new TText(.55, .03, "Cham. 7");
   tR2bXZ3->SetTextColor(6);
   tR2bXZ3->Draw();
   tR2bXZ4 = new TText(.81, .03, "Cham. 8");
   tR2bXZ4->SetTextColor(8);
   tR2bXZ4->Draw();
   fRegion2bXZ->GetCanvas()->SetEditable(kFALSE);

   //REGION 3--Vertical Drift Chambers--------------------------------------------------//

   // Create tab and container of "Region 3" VDC data
   fRegion3 = fRegions->AddTab("Region 3--VDC");
   fRegion3->SetLayoutManager(new TGVerticalLayout(fRegion3));
   fRegion3->SetLayoutBroken(kTRUE);
   fRegion3->SetBackgroundColor(ucolor);

   // Create embedded canvas for Region 3 XY view
   fRegion3XY = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3XY = fRegion3XY->GetCanvasWindowId();
   cR3XY = new TCanvas("cR3XY", 10, 10, wRegion3XY);
   fRegion3XY->AdoptCanvas(cR3XY);
   fRegion3->AddFrame(fRegion3XY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3XY->MoveResize(12,4,280,280);
   fRegion3XY->GetCanvas()->SetFillColor(0);
   Label_R3XY = new TPaveLabel(.03,.94,.97,.99,"Front View (Normal to Tilt Angle)"); // Create canvas label
   Label_R3XY->Draw();
   Box_R3XY1 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .65-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .65+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 1
   Box_R3XY1->SetLineColor(1);
   Box_R3XY1->SetFillStyle(0);
   Box_R3XY2 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .75-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .75+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 2
   Box_R3XY2->SetLineColor(7);
   Box_R3XY2->SetFillStyle(0);
   Box_R3XY2->Draw();
   Box_R3XY1->Draw(); // Draw 1 on top of 2
   Box_R3XY3 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .25-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .25+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 3
   Box_R3XY3->SetLineColor(6);
   Box_R3XY3->SetFillStyle(0);
   Box_R3XY4 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .35-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .35+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 4
   Box_R3XY4->SetLineColor(8);
   Box_R3XY4->SetFillStyle(0);
   Box_R3XY4->Draw();
   Box_R3XY3->Draw(); // Draw 3 on top of 4
   tR3XY1 = new TText(.03, .03, "Leia (1)"); // Color-coded text for VDC chamber labels
   tR3XY1->SetTextColor(1);
   tR3XY1->Draw();
   tR3XY2 = new TText(.28, .03, "Vader (2)");
   tR3XY2->SetTextColor(7);
   tR3XY2->Draw();
   tR3XY3 = new TText(.55, .03, "Han (3)");
   tR3XY3->SetTextColor(6);
   tR3XY3->Draw();
   tR3XY4 = new TText(.81, .03, "Yoda (4)");
   tR3XY4->SetTextColor(8);
   tR3XY4->Draw();
   Div_R3XY = new TLine(0,.5,1,.5);
   Div_R3XY->SetLineStyle(2);
   Div_R3XY->Draw();
   fRegion3XY->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 3 YZ view
   fRegion3YZ = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3YZ = fRegion3YZ->GetCanvasWindowId();
   cR3YZ = new TCanvas("cR3YZ", 10, 10, wRegion3YZ);
   fRegion3YZ->AdoptCanvas(cR3YZ);
   fRegion3->AddFrame(fRegion3YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3YZ->MoveResize(306,4,280,280);
   fRegion3YZ->GetCanvas()->SetFillColor(0);
   Label_R3YZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Oriented to Tilt Angle)"); // Create canvas label
   Label_R3YZ->Draw();
   Box_R3YZ1 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .65-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .65+(R3_DEPTH*R3_CM*.5)); // Create and place box for chamber 1
   Box_R3YZ1->SetLineColor(1);
   Box_R3YZ1->SetFillStyle(0);
   Box_R3YZ2 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .75-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .75+(R3_DEPTH*R3_CM*.5)); // Create and place box for chamber 2
   Box_R3YZ2->SetLineColor(7);
   Box_R3YZ2->SetFillStyle(0);
   Box_R3YZ2->Draw();
   Box_R3YZ1->Draw(); // Draw 1 on top of 2
   Box_R3YZ3 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .25-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .25+(R3_DEPTH*R3_CM*.5)); // Create and place box for chamber 3
   Box_R3YZ3->SetLineColor(6);
   Box_R3YZ3->SetFillStyle(0);
   Box_R3YZ4 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .35-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .35+(R3_DEPTH*R3_CM*.5)); // Create and place box for chamber 4
   Box_R3YZ4->SetLineColor(8);
   Box_R3YZ4->SetFillStyle(0);
   Box_R3YZ4->Draw();
   Box_R3YZ3->Draw(); // Draw 3 on top of 4
   tR3YZ1 = new TText(.03, .03, "Leia (1)"); // Color-coded text for VDC chamber labels
   tR3YZ1->SetTextColor(1);
   tR3YZ1->Draw();
   tR3YZ2 = new TText(.28, .03, "Vader (2)");
   tR3YZ2->SetTextColor(7);
   tR3YZ2->Draw();
   tR3YZ3 = new TText(.55, .03, "Han (3)");
   tR3YZ3->SetTextColor(6);
   tR3YZ3->Draw();
   tR3YZ4 = new TText(.81, .03, "Yoda (4)");
   tR3YZ4->SetTextColor(8);
   tR3YZ4->Draw();
   Div_R3YZ = new TLine(0,.5,1,.5);
   Div_R3YZ->SetLineStyle(2);
   Div_R3YZ->Draw();
   fRegion3YZ->GetCanvas()->SetEditable(kFALSE);

   // Create embedded canvas for Region 3 XZ view
   fRegion3XZ = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3XZ = fRegion3XZ->GetCanvasWindowId();
   cR3XZ = new TCanvas("cR3XZ", 10, 10, wRegion3XZ);
   fRegion3XZ->AdoptCanvas(cR3XZ);
   fRegion3->AddFrame(fRegion3XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3XZ->MoveResize(603,4,280,280);
   fRegion3XZ->GetCanvas()->SetFillColor(0);
   Label_R3XZ = new TPaveLabel(.03,.94,.97,.99,"Side View (Rotated to Tilt Angle)"); // Create canvas label
   Label_R3XZ->Draw();
   Box_R3XZ1 = new TBox(.5-(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .65-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .65+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 1
   Box_R3XZ1->SetLineColor(1);
   Box_R3XZ1->SetFillStyle(0);
   Box_R3XZ2 = new TBox(.5-(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .75-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .75+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 2
   Box_R3XZ2->SetLineColor(7);
   Box_R3XZ2->SetFillStyle(0);
   Box_R3XZ2->Draw();
   Box_R3XZ1->Draw();
   Box_R3XZ3 = new TBox(.5-(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .25-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .25+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 3
   Box_R3XZ3->SetLineColor(6);
   Box_R3XZ3->SetFillStyle(0);
   Box_R3XZ4 = new TBox(.5-(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .35-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .35+(R3_LENGTH*R3_CM*.5)); // Create and place box for chamber 4
   Box_R3XZ4->SetLineColor(8);
   Box_R3XZ4->SetFillStyle(0);
   Box_R3XZ4->Draw();
   Box_R3XZ3->Draw();
   tR3XZ1 = new TText(.03, .03, "Leia (1)"); // Color-coded text for VDC chamber labels
   tR3XZ1->SetTextColor(1);
   tR3XZ1->Draw();
   tR3XZ2 = new TText(.28, .03, "Vader (2)");
   tR3XZ2->SetTextColor(7);
   tR3XZ2->Draw();
   tR3XZ3 = new TText(.55, .03, "Han (3)");
   tR3XZ3->SetTextColor(6);
   tR3XZ3->Draw();
   tR3XZ4 = new TText(.81, .03, "Yoda (4)");
   tR3XZ4->SetTextColor(8);
   tR3XZ4->Draw();
   Div_R3XZ = new TLine(0,.5,1,.5);
   Div_R3XZ->SetLineStyle(2);
   Div_R3XZ->Draw();
   fRegion3XZ->GetCanvas()->SetEditable(kFALSE);

   //REGION 3--Trigger Scintillator--------------------------------------------------//  NOT USED CURRENTLY

   // create tab and container of "Region 3" TS data
   fRegion3TS = fRegions->AddTab("Region 3--TS");
   fRegion3TS->SetLayoutManager(new TGVerticalLayout(fRegion3TS));
   fRegion3TS->SetLayoutBroken(kTRUE);
   fRegion3TS->SetBackgroundColor(ucolor);

   // Set region tab attributes
   fRegions->SetTab(0);
   fRegions->Resize(fRegions->GetDefaultSize());
   fMain->AddFrame(fRegions, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegions->MoveResize(0,340,900,332);
   gClient->GetColorByName("#deba87", ucolor); // Set ucolor to mute orange
   fRegions->GetTabTab("Region 1--GEM")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 2--HDC (1-4)")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 2--HDC (5-8)")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 3--VDC")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 3--TS")->SetBackgroundColor(ucolor);


   ////BUTTONS----------------------------------------------------------------------------------------------------////

   // Create horizontal frame for buttons
   gClient->GetColorByName("#0047ab",ucolor);
   TGHorizontalFrame *fButtons = new TGHorizontalFrame(fMain,900,60,kHorizontalFrame,ucolor);
   fButtons->SetLayoutBroken(kTRUE);
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*"); // Set ufont to bold helvetica 12
   gClient->GetColorByName("#d4cf87",ucolor);                            // Set ucolor to buff

   // Graphics context changes for "Exit" button
   GCValues_t valutton;
   valutton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valutton.fForeground);
   gClient->GetColorByName("#c0c0c0",valutton.fBackground);
   valutton.fFillStyle = kFillSolid;
   valutton.fFont = ufont->GetFontHandle();
   valutton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valutton, kTRUE);
   // Create exit button
   ExitButton = new TGTextButton(fButtons,"E&xit","gApplication->Terminate(0)"); // Connect button to application termination
   ExitButton->SetFont(ufont->GetFontStruct());
   ExitButton->SetTextJustify(36);
   ExitButton->SetMargins(0,0,0,0);
   ExitButton->SetWrapLength(-1);
   ExitButton->Resize(100,50);
   ExitButton->ChangeBackground(ucolor);
   // Add exit button to buttons frame
   fButtons->AddFrame(ExitButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   ExitButton->MoveResize(784,6,100,50);

   // Graphics context changes for "Previous Event" button
   GCValues_t valousButton;
   valousButton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valousButton.fForeground);
   gClient->GetColorByName("#c0c0c0",valousButton.fBackground);
   valousButton.fFillStyle = kFillSolid;
   valousButton.fFont = ufont->GetFontHandle();
   valousButton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valousButton, kTRUE);
   // Create previous button
   TGTextButton *PreviousButton = new TGTextButton(fButtons,"&Previous Event",-1,uGC->GetGC(),ufont->GetFontStruct());
   PreviousButton->Connect("Clicked()", "QwGUIEventDisplay", this, "GoPrevious()"); // Connect button to GoPrevious() function
   PreviousButton->SetTextJustify(36);
   PreviousButton->SetMargins(0,0,0,0);
   PreviousButton->SetWrapLength(-1);
   PreviousButton->Resize(100,50);
   PreviousButton->ChangeBackground(ucolor);
   // Add previous button to buttons frame
   fButtons->AddFrame(PreviousButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   PreviousButton->MoveResize(12,6,100,50);


   // Graphics context changes for "Next Event" button
   valutton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valutton.fForeground);
   gClient->GetColorByName("#c0c0c0",valutton.fBackground);
   valutton.fFillStyle = kFillSolid;
   valutton.fFont = ufont->GetFontHandle();
   valutton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valutton, kTRUE);
   // Create next button
   TGTextButton *NextButton = new TGTextButton(fButtons,"&Next Event",-1,uGC->GetGC(),ufont->GetFontStruct());
   NextButton->Connect("Clicked()", "QwGUIEventDisplay", this, "GoNext()"); // Connect button to GoNext() function
   NextButton->SetTextJustify(36);
   NextButton->SetMargins(0,0,0,0);
   NextButton->SetWrapLength(-1);
   NextButton->Resize(100,50);
   NextButton->ChangeBackground(ucolor);
   // Add next button to buttons frame
   fButtons->AddFrame(NextButton, new TGLayoutHints(kLHintsNormal));
   NextButton->MoveResize(125,6,100,50);

   // Add buttons frame to main frame
   fMain->AddFrame(fButtons, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fButtons->MoveResize(0,652,900,80);


   //LOCAL COORDINATE KEY----------------------------------------------------------------------------------------------------//

   // Create embedded canvas to display key for local coordinates
   TRootEmbeddedCanvas *fKey = new TRootEmbeddedCanvas(0,fButtons,280,280);
   Int_t wKey = fKey->GetCanvasWindowId();
   TCanvas *cKey = new TCanvas("cKey", 10, 10, wKey);
   fKey->AdoptCanvas(cKey);
   fButtons->AddFrame(fKey, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fKey->MoveResize(245,6,515,50);
   // Fill in key details
   fKey->GetCanvas()->cd();
   fKey->GetCanvas()->SetFillColor(0);
   TPaveLabel *Label_KeyX = new TPaveLabel(.01,.05,.24,.95," X"); // Create key label for X direction
   Label_KeyX->SetTextAlign(12);
   Label_KeyX->SetTextColor(kRed);
   Label_KeyX->SetShadowColor(0);
   Label_KeyX->Draw();
   TLine *Line_KeyX = new TLine(.1,.5,.2,.5);
   Line_KeyX->SetLineColor(kRed);
   Line_KeyX->Draw();
   TPaveLabel *Label_KeyY = new TPaveLabel(.25,.05,.49,.95," Y"); // Create key label for Y direction
   Label_KeyY->SetTextAlign(12);
   Label_KeyY->SetTextColor(kViolet);
   Label_KeyY->SetShadowColor(0);
   Label_KeyY->Draw();
   TLine *Line_KeyY = new TLine(.4,.1,.4,.9);
   Line_KeyY->SetLineColor(kViolet);
   Line_KeyY->Draw();
   TPaveLabel *Label_KeyU = new TPaveLabel(.5,.05,.74,.95," U");  // Create key label for U direction
   Label_KeyU->SetTextAlign(12);
   Label_KeyU->SetTextColor(kGreen);
   Label_KeyU->SetShadowColor(0);
   Label_KeyU->Draw();
   TLine *Line_KeyU = new TLine(.59,.1,.7,.9);
   Line_KeyU->SetLineColor(kGreen);
   Line_KeyU->Draw();
   TPaveLabel *Label_KeyV = new TPaveLabel(.75,.05,.99,.95," V"); // Create key label for V direction
   Label_KeyV->SetTextAlign(12);
   Label_KeyV->SetTextColor(kBlue);
   Label_KeyV->SetShadowColor(0);
   Label_KeyV->Draw();
   TLine *Line_KeyV = new TLine(.84,.9,.95,.1);
   Line_KeyV->SetLineColor(kBlue);
   Line_KeyV->Draw();
   fKey->GetCanvas()->SetEditable(kFALSE);
   fKey->GetCanvas()->Update();

   // Add main frame to the client window and customize
   fMain->MapSubwindows();
   fMain->Resize(fMain->GetDefaultSize());
   fMain->MapWindow();
   fMain->Resize(900,712);
}

void QwGUIEventDisplay::OnReceiveMessage(char *obj)
{

}

void QwGUIEventDisplay::OnObjClose(char *obj)
{
  if (!strcmp(obj,"dROOTFile")) {
    dROOTCont = NULL;
  }
}

void QwGUIEventDisplay::OnNewDataContainer(RDataContainer *cont)
{
  std::cout << "Opening in QwGUIEventDisplay..." << std::endl;

  fEventNumber = 0;
  GoClear();

  if (!cont) return;

  if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont) {
    TObject* obj = dROOTCont->ReadData("hit_tree");

    if (obj && obj->InheritsFrom("TTree")) {
      fTree = (TTree*) obj->Clone();


      if (fTree->FindBranch("hits")){
   fMenuBar->MoveResize(0,0,900,20);
	Int_t entries = fTree->GetEntries();
	std::cout<<"Entries in tree: "<< entries << std::endl;

	fHitRootList = 0;

	fTree->SetBranchAddress("hits", &fHitRootList);

	std::cout << "Opened in QwGUIEventDisplay" << std::endl;
      }
      else{
	std::cout<<"No hits branch"<<std::endl;
      }
    }
    else{
      std::cout<<"No Tree inherited"<<std::endl;
    }
  }
  else{
    std::cout<<"No directory hit_tree"<<std::endl;
  }
}

void QwGUIEventDisplay::OnRemoveThisTab()
{

}

void QwGUIEventDisplay::HandleMenu(Int_t id){  // Controls the menu bar functions
                                               // Called my menu entry click

  switch (id){
  case M_HELP_TUTORIAL:  // Print a link to the event display tutorial
    printf("\nTutorial documentation is located at https://qweak.jlab.org/wiki/index.php/Track_Reconstruction.\n");
    break;
  case M_HELP_ABOUT:     // Print information about the event display
    printf("\n**************************************\n** Qweak 2D Event Display           **\n** Jefferson Lab -- Hall C          **\n** Author: Derek Jones              **\n** The George Washington University **\n** Contact: dwjones8@gwu.edu        **\n** Last updated: 7-26-2010          **\n**************************************\n");
    break;
  }
} // End HandleMenu(Int_t)

void QwGUIEventDisplay::GoClear(){  // Clears all displayed data
  fCurrentEventLabel->SetText(Form("%d", fEventNumber)); // Updates layout to show current event number
  
  fWireInfoListBox->RemoveAll(); // Clear wire hit information listbox
  
  Line_R1r.clear(); // Clear existing Region 1 vectors
  Line_R1y.clear();
  Line_R2x.clear(); // Clear existing Region 2 vectors
  Line_R2u.clear();
  Line_R2v.clear();
  Line_R3u.clear(); // Clear existing Region 3 vectors
  Line_R3v.clear();

  Octant_1->SetFillColor(0); // Clear octant identification fills
  Octant_2->SetFillColor(0);
  Octant_3->SetFillColor(0);
  Octant_4->SetFillColor(0);
  Octant_5->SetFillColor(0);
  Octant_6->SetFillColor(0);
  Octant_7->SetFillColor(0);
  Octant_8->SetFillColor(0);

  fOctantID->GetCanvas()->Update();  // Update canvases to null state
  fRegion1XY->GetCanvas()->Update();
  fRegion1XZ->GetCanvas()->Update();
  fRegion1YZ->GetCanvas()->Update();
  fRegion2XY->GetCanvas()->Update();
  fRegion2XZ->GetCanvas()->Update();
  fRegion2YZ->GetCanvas()->Update();
  fRegion2bXY->GetCanvas()->Update();
  fRegion2bXZ->GetCanvas()->Update();
  fRegion2bYZ->GetCanvas()->Update();
  fRegion3XY->GetCanvas()->Update();
  fRegion3XZ->GetCanvas()->Update();
  fRegion3YZ->GetCanvas()->Update();
} // End GoClear()

void QwGUIEventDisplay::GotoEvent(){  // Goes to desired event number written in "goto event" number entry
                                      // Called by GotoEventButton click
  
  fEventNumber = fCurrentEventEntry->GetNumberEntry()->GetIntNumber(); // fEventNumber takes value from number entry
  
  if (fEventNumber > 0){
    fCurrentEventLabel->SetText(Form("%d",fCurrentEventEntry->GetNumberEntry()->GetIntNumber())); // Set current event label to value from number entry
    DrawEvent();                                                                                  // Update display information
  }
  
  else{
    fEventNumber = 0; // Go to null event
    GoClear();        // Keep from drawing non-existent negative events
  }
} // End GotoEvent()

void QwGUIEventDisplay::GoPrevious(){  // Subtracts one from current event counter
                                       // Called by PreviousButton click
  
  if (fEventNumber > 1){
    fEventNumber--;  // Go to previous event
    DrawEvent();     // Update display information
  }
  else{
    fEventNumber = 0;  // Go to null event
    GoClear();         // Keep from drawing non-existent negative events
  }
} // End GoPrevious()

void QwGUIEventDisplay::GoNext(){  // Adds one to current event counter
                                   // Called by NextButton click
  
  fEventNumber++;  // Go to next event
  DrawEvent();     // Update display information
} // End GoNext()

int QwGUIEventDisplay::DrawEvent(){  // Draws event data into display
                                     // Lists wire hit information in the listbox
                                     // Fills in the current octants in the octant ID
                                     // Displays orthographic views of triggered wires for each region
                                     // Called by Previous, Next, or GotoEvent buttons
  
  //SETUP FOR FUNCTION--------------------------------------------------//

  printf("Drawing event %d...\n", fEventNumber);

  // Catch to keep from attempting to draw events outside the number of entries
  if (fEventNumber >= fTree->GetEntries()){
    printf("Draw error: There are only %lld entries.\n", fTree->GetEntries());
    fEventNumber = 0;
    GoClear();
    printf("Returned to null event.\n");
    return 0;
  }

  fOctantID->GetCanvas()->SetEditable(kTRUE);  // Restore editing capability of canvases
  fRegion1XY->GetCanvas()->SetEditable(kTRUE);  
  fRegion1XZ->GetCanvas()->SetEditable(kTRUE);
  fRegion1YZ->GetCanvas()->SetEditable(kTRUE);
  fRegion2XY->GetCanvas()->SetEditable(kTRUE);
  fRegion2XZ->GetCanvas()->SetEditable(kTRUE);
  fRegion2YZ->GetCanvas()->SetEditable(kTRUE);
  fRegion2bXY->GetCanvas()->SetEditable(kTRUE);
  fRegion2bXZ->GetCanvas()->SetEditable(kTRUE);
  fRegion2bYZ->GetCanvas()->SetEditable(kTRUE);
  fRegion3XY->GetCanvas()->SetEditable(kTRUE);
  fRegion3XZ->GetCanvas()->SetEditable(kTRUE);
  fRegion3YZ->GetCanvas()->SetEditable(kTRUE);

  GoClear(); // Clear existing data

  //HIGHLIGHT OCTANT POSITION--------------------------------------------------// ##############GET THIS INFORMATION FROM WHEREEVER IT BECOMES AVAILABLE!!!

  // Dark orange is left (lower # chambers)
  // Light orange is right (higher # chambers)
  fOctantID->GetCanvas()->cd();
  int fTest = 1; // USED ONLY FOR DISPLAY PURPOSES
  switch(fTest){
  case 1:
    Octant_1->SetFillColor(kOrange-3);
    Octant_5->SetFillColor(kOrange-2);
    Octant_1->Draw();
    Octant_5->Draw(); 
    break;
    //  etc.........
  }
  fOctantID->GetCanvas()->SetEditable(kFALSE);
  fOctantID->GetCanvas()->Update();

  //LIST WIRE HITS--------------------------------------------------//
  
  // Extract wire hit information
  fTree->GetEntry(fEventNumber);       // Get entry from tree for the current event number
  fHitList = fHitRootList->Convert();  // Convert from ROOT container to a hit container
  printf("Printing hit list...");      // Let user know hit list is printing
  fHitList->Print();                   // Prints the new wire hit information from the hit list
  
  // Create a string buffer to hold hit information
  char fHitBuffer[30];
  
  // List wire hit information in list box
  for(QwHitContainer::iterator fHit = fHitList->begin(); fHit != fHitList->end(); fHit++){ // Loop while hits exist in the hit container
    if(fHit->GetElement() > 0){
      if (fHit->GetRegion() == 1){
	sprintf(fHitBuffer, "Region %i:  Trace %i, Plane %i", fHit->GetRegion(), fHit->GetElement(), fHit->GetPlane()); // Print single hit for Region 1
      }
      else if (fHit->GetRegion() == 2){
	sprintf(fHitBuffer, "Region %i:  Wire %i, Plane %i      Drift Distance: %lf", fHit->GetRegion(), fHit->GetElement(), fHit->GetPlane(), fHit->GetDriftDistance()); // Print single hit for Region 2
      }
      else if (fHit->GetRegion() == 3){
	sprintf(fHitBuffer, "Region %i:  Wire %i, Plane %i      Drift Distance: %lf", fHit->GetRegion(), fHit->GetElement(), fHit->GetPlane(), fHit->GetDriftDistance()); // Print single hit for Region 3
      }
      else{}
      fWireInfoListBox->AddEntry(fHitBuffer, fEventNumber); // Add entry to list box
      fWireInfoListBox->MapSubwindows();
      fWireInfoListBox->Layout();
    }
  }
  
  //GATHER WIRE HIT DATA--------------------------------------------------//

  // Region 1 hits
  QwHitContainer* Hits_R1r = fHitList->GetSubList_Dir(kRegionID1, kPackage1, kDirectionR);
  QwHitContainer* Hits_R1y = fHitList->GetSubList_Dir(kRegionID1, kPackage1, kDirectionY);
  // Region 2 hits
  QwHitContainer* Hits_R2x = fHitList->GetSubList_Dir(kRegionID2, kPackage1, kDirectionX);
  QwHitContainer* Hits_R2u = fHitList->GetSubList_Dir(kRegionID2, kPackage1, kDirectionU);
  QwHitContainer* Hits_R2v = fHitList->GetSubList_Dir(kRegionID2, kPackage1, kDirectionV);
  // Region 3 hits
  QwHitContainer* Hits_R3u = fHitList->GetSubList_Dir(kRegionID3, kPackage1, kDirectionU);
  QwHitContainer* Hits_R3v = fHitList->GetSubList_Dir(kRegionID3, kPackage1, kDirectionV);
  
  // DRAWING WIRE HITS--------------------------------------------------//
  // Three types of wires to draw--full length, left of full length, and right of full length
  // Separating them depends on the first and last full length wire (saved as constants in the header)
  // Wires are drawn and centered with respect to the chamber frame in ROOT coordinates
  // Wire numbers go from right to left; plane numbers go from front to back
  // Hit wire information is stored to the end of a vector and drawn in the appropriate canvas
  // Editing of canvases is removed after drawing

  // DRAW REGION 1 TRACE HITS--------------------------------------------------//
  
  // FRONT VIEW (X-Y)
  fRegion1XY->GetCanvas()->cd();
  // Region 1 X traces (??? total)
  for(QwHitContainer::iterator fHit = Hits_R1r->begin(); fHit != Hits_R1r->end(); fHit++){  // G4 DOES NOT GET ANY HITS FROM THESE
    TLine Line;
    int fTrace = fHit->GetElement();
    Line.SetY1(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*fTrace));
    Line.SetX1(.5 - (R1_LENGTH*R1_CM*.5));
    Line.SetY2(.5 + (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*fTrace));
    Line.SetX2(.5 - (R1_LENGTH*R1_CM*.5));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line);
    Line_R1y.back().Draw();
  }
  // Region 1 Y traces (??? total)
  for(QwHitContainer::iterator fHit = Hits_R1y->begin(); fHit != Hits_R1y->end(); fHit++){
    TLine Line;
    int fTrace = fHit->GetElement();
    Line.SetY1(.5 - (R1_WIDTH*R1_CM*.5));
    Line.SetX1(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fTrace));
    Line.SetY2(.5 + (R1_WIDTH*R1_CM*.5));
    Line.SetX2(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fTrace));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line);
    Line_R1y.back().Draw();
  }
  fRegion1XY->GetCanvas()->SetEditable(kFALSE);
  fRegion1XY->GetCanvas()->Update();

  // TOP VIEW (Y-Z)
  fRegion1YZ->GetCanvas()->cd();
  // Region 1 X traces (??? total)
  // Region 1 Y wires (??? total)
  for(QwHitContainer::iterator fHit = Hits_R1y->begin(); fHit != Hits_R1y->end(); fHit++){
    TLine Line;
    int fTrace = fHit->GetElement();
    Line.SetY1(.5 - (R1_DEPTH*R1_CM*.5));
    Line.SetX1(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fTrace));
    Line.SetY2(.5 + (R1_DEPTH*R1_CM*.5));
    Line.SetX2(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fTrace));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line);
    Line_R1y.back().Draw();
  }
  fRegion1YZ->GetCanvas()->SetEditable(kFALSE);
  fRegion1YZ->GetCanvas()->Update();
  
  // SIDE VIEW (X-Z)
  fRegion1XZ->GetCanvas()->cd();
  // Region 1 X traces (??? total)
  // Region 1 Y wires (??? total)
  for(QwHitContainer::iterator fHit = Hits_R1y->begin(); fHit != Hits_R1y->end(); fHit++){
    TLine Line;
    Line.SetX1(.5);
    Line.SetY1(.5 - (R1_LENGTH*R1_CM*.5));
    Line.SetX2(.5);
    Line.SetY2(.5 + (R1_LENGTH*R1_CM*.5));
    Line.SetLineColor(kViolet);
    Line.SetLineWidth(22); // To fill whole plane; 22 is similar to R1_DEPTH   <--NOT OK!
    Line_R1y.push_back(Line);
    Line_R1y.back().Draw();
  }
  fRegion1XZ->GetCanvas()->SetEditable(kFALSE);
  fRegion1XZ->GetCanvas()->Update();

  //DRAW REGION 2 WIRE HITS--------------------------------------------------//

  // FRONT VIEW (X-Y)
  fRegion2XY->GetCanvas()->cd();
  // Region 2 X wires (32 total)
  for(QwHitContainer::iterator fHit = Hits_R2x->begin(); fHit != Hits_R2x->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Wire plane
    double fPShift = 0;             // Used to shift the wires on the prime planes (ROOT coordinates)
    switch (fPlane){
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 16:
    case 17:
    case 18:
    case 22:
    case 23:
    case 24:
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell distance
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bXY->GetCanvas()->cd();
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fPShift);
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fPShift);
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line);
    Line_R2x.back().Draw();
  }
  // Region 2 U wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2u->begin(); fHit != Hits_R2u->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Wire plane
    int fShiftTrim = 0;             // Used to correct shifted prime wires from going outside the frame
    double fPShift = 0;             // Used to shift the wires on the prime planes (ROOT coordinates)
    switch (fPlane){
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 16:
    case 17:
    case 18:
    case 22:
    case 23:
    case 24:
      fPShift = .5*R2_UVDIST*R2_CM; // Half drift cell distance
      if (fWire == R2_FULLWIRE1 || fWire == R2_FULLWIRE2)
	fShiftTrim = 1;
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bXY->GetCanvas()->cd();
    if (fWire < R2_FULLWIRE1){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (((R2_UVDIST*R2_CM*(fWire - fShiftTrim)) + fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R2_FULLWIRE2){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5) - (((R2_WIDTH*R2_CM) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  // Region 2 V wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2v->begin(); fHit != Hits_R2v->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Wire plane
    int fShiftTrim = 0;             // Used to correct shifted prime wires from going outside the frame
    double fPShift = 0;             // Used to shift the wires in the prime chamber planes (ROOT coordinates)
    switch (fPlane){
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 16:
    case 17:
    case 18:
    case 22:
    case 23:
    case 24:
      fPShift = .5*R2_UVDIST*R2_CM; // Half drift cell distance
      if (fWire == R2_FULLWIRE1 || fWire == R2_FULLWIRE2)
	fShiftTrim = 1;
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bXY->GetCanvas()->cd();
    if (fWire < R2_FULLWIRE1){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5) - (((R2_UVDIST*R2_CM*(fWire - fShiftTrim)) + fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R2_FULLWIRE2){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (((R2_WIDTH*R2_CM) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
      }
    Line.SetLineColor(kBlue);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
  }
  fRegion2XY->GetCanvas()->SetEditable(kFALSE);
  fRegion2XY->GetCanvas()->Update();
  fRegion2bXY->GetCanvas()->SetEditable(kFALSE);
  fRegion2bXY->GetCanvas()->Update();

  // TOP VIEW (Y-Z)
  fRegion2YZ->GetCanvas()->cd();
  // Region 2 X wires (32 total)
  for(QwHitContainer::iterator fHit = Hits_R2x->begin(); fHit != Hits_R2x->end(); fHit++){
    TLine Line;
    int fPlane = fHit->GetPlane(); // Wire plane
    double fYShift = 0;            // Used to shift the wires in the prime planes and to the correct chamber planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
    case 13:
    case 14:
    case 15:
      fYShift = (R2_DEPTH*R2_CM)/4; // Three evenly spaced wires (3+1)
      break;
    case 4:
    case 5:
    case 6:
    case 16:
    case 17:
    case 18:
      fYShift = (R2_DIST*R2_CM) + (R2_DEPTH*R2_CM) + (R2_DEPTH*R2_CM)/4;
      break;
    case 7:
    case 8:
    case 9:
    case 19:
    case 20:
    case 21:
      fYShift = (2*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM) + (R2_DEPTH*R2_CM)/4;
      break;
    case 10:
    case 11:
    case 12:
    case 22:
    case 23:
    case 24:
      fYShift = (3*R2_DIST*R2_CM) + (3*R2_DEPTH*R2_CM) + (R2_DEPTH*R2_CM)/4;
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bYZ->GetCanvas()->cd();
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    Line.SetLineColor(kRed);
    Line.SetLineWidth(2);
    Line_R2x.push_back(Line);
    Line_R2x.back().Draw();
  }
 // Region 2 U wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2u->begin(); fHit != Hits_R2u->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fPShift = 0;             // Used to shift the wires in the prime planes
    double fYShift = 0;             // Used to shift the wires to the correct chamber planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
    case 13:
    case 14:
    case 15:
      fYShift = 2*((R2_DEPTH*R2_CM)/4);
      break;
    case 4:
    case 5:
    case 6:
    case 16:
    case 17:
    case 18:
      fPShift = .5*R2_UVDIST*R2_CM;
      fYShift = (R2_DIST*R2_CM) + (R2_DEPTH*R2_CM) + (2*((R2_DEPTH*R2_CM)/4));
      break;
    case 7:
    case 8:
    case 9:
    case 19:
    case 20:
    case 21:
      fYShift = (2*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM) + (2*((R2_DEPTH*R2_CM)/4));
      break;
    case 10:
    case 11:
    case 12:
    case 22:
    case 23:
    case 24:
      fPShift = .5*R2_UVDIST*R2_CM;
      fYShift = (3*R2_DIST*R2_CM) + (3*R2_DEPTH*R2_CM) + (2*((R2_DEPTH*R2_CM)/4));
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bYZ->GetCanvas()->cd();
    if (fWire < R2_FULLWIRE1){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
      Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    }
    else if (fWire < R2_FULLWIRE2){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
      Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    }
    Line.SetLineColor(kGreen);
    Line.SetLineWidth(2);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  // Region 2 V wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2v->begin(); fHit != Hits_R2v->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fPShift = 0;             // Used to shift the wires in the prime planes
    double fYShift = 0;             // Used to shift the wires to the correct chamber planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
    case 13:
    case 14:
    case 15:
      fYShift = 3*((R2_DEPTH*R2_CM)/4);
      break;
    case 4:
    case 5:
    case 6:
    case 16:
    case 17:
    case 18:
      fPShift = .5*R2_UVDIST*R2_CM;
      fYShift = (R2_DIST*R2_CM) + (R2_DEPTH*R2_CM) + (3*((R2_DEPTH*R2_CM)/4));
      break;
    case 7:
    case 8:
    case 9:
    case 19:
    case 20:
    case 21:
      fYShift = (2*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM) + (3*((R2_DEPTH*R2_CM)/4));
      break;
    case 10:
    case 11:
    case 12:
    case 22:
    case 23:
    case 24:
      fPShift = .5*R2_UVDIST*R2_CM;
      fYShift = (3*R2_DIST*R2_CM) + (3*R2_DEPTH*R2_CM) + (3*((R2_DEPTH*R2_CM)/4));
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bYZ->GetCanvas()->cd();
    if (fWire < R2_FULLWIRE1){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
      Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    }
    else if (fWire < R2_FULLWIRE2){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fPShift);
      Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
      Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift);
      Line.SetY2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fYShift);
      }
    Line.SetLineColor(kBlue);
    Line.SetLineWidth(2);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  fRegion2YZ->GetCanvas()->SetEditable(kFALSE);
  fRegion2YZ->GetCanvas()->Update();
  fRegion2bYZ->GetCanvas()->SetEditable(kFALSE);
  fRegion2bYZ->GetCanvas()->Update();

  // SIDE VIEW (X-Z)
  fRegion2XZ->GetCanvas()->cd();
  // Region 2 X wires (32 total)
  for(QwHitContainer::iterator fHit = Hits_R2x->begin(); fHit != Hits_R2x->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fXShift = 0;             // Used to shift the lines to the correct chamber planes (ROOT coordinates)
    double fPShift = 0;             // Used to shift the wires in the prime planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
    case 13:
    case 14:
    case 15:
      fXShift = (R2_DEPTH*R2_CM)/4; // Three evenly spaced wires (3+1)
      break;
    case 4:
    case 5:
    case 6:
    case 16:
    case 17:
    case 18:
      fXShift = (R2_DIST*R2_CM) + (R2_DEPTH*R2_CM) + (R2_DEPTH*R2_CM)/4;
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell size
      break;
    case 7:
    case 8:
    case 9:
    case 19:
    case 20:
    case 21:
      fXShift = (2*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM) + (R2_DEPTH*R2_CM)/4;
      break;
    case 10:
    case 11:
    case 12:
    case 22:
    case 23:
    case 24:
      fXShift = (3*R2_DIST*R2_CM) + (3*R2_DEPTH*R2_CM) + (R2_DEPTH*R2_CM)/4;
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell size
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bXZ->GetCanvas()->cd();
    Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift); 
    Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fPShift);
    Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift); 
    Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fPShift);
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line);
    Line_R2x.back().Draw();
  }
  // Region 2 U wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2u->begin(); fHit != Hits_R2u->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fXShift = 0;             // Used to shift the lines to the correct chamber planes (ROOT coordinates)
    double fPShift = 0;             // Used to shift the wires in the prime planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
    case 13:
    case 14:
    case 15:
      fXShift = 2*((R2_DEPTH*R2_CM)/4); // Three evenly spaced wires
      break;
    case 4:
    case 5:
    case 6:
    case 16:
    case 17:
    case 18:
      fXShift = (R2_DIST*R2_CM) + (R2_DEPTH*R2_CM) + (2*(R2_DEPTH*R2_CM)/4);
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell size
      break;
    case 7:
    case 8:
    case 9:
    case 19:
    case 20:
    case 21:
      fXShift = (2*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM) + (2*(R2_DEPTH*R2_CM)/4);
      break;
    case 10:
    case 11:
    case 12:
    case 22:
    case 23:
    case 24:
      fXShift = (3*R2_DIST*R2_CM) + (3*R2_DEPTH*R2_CM) + (2*(R2_DEPTH*R2_CM)/4);
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell size
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab
      fRegion2bXZ->GetCanvas()->cd();
    if (fWire < R2_FULLWIRE1){
      Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (((R2_UVDIST*R2_CM*fWire) + fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R2_FULLWIRE2){
      Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5) - (((R2_WIDTH*R2_CM) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
      Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line.SetLineWidth(2);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  // Region 2 V wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2v->begin(); fHit != Hits_R2v->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fXShift = 0;             // Used to shift the lines to the correct chamber planes (ROOT coordinates)
    double fPShift = 0;             // Used to shift the wires on the prime planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
    case 13:
    case 14:
    case 15:
      fXShift = 3*((R2_DEPTH*R2_CM)/4); // Three evenly spaced wires
      break;
    case 4:
    case 5:
    case 6:
    case 16:
    case 17:
    case 18:
      fXShift = (R2_DIST*R2_CM) + (R2_DEPTH*R2_CM) + (3*(R2_DEPTH*R2_CM)/4);
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell size
      break;
    case 7:
    case 8:
    case 9:
    case 19:
    case 20:
    case 21:
      fXShift = (2*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM) + (3*(R2_DEPTH*R2_CM)/4);
      break;
    case 10:
    case 11:
    case 12:
    case 22:
    case 23:
    case 24:
      fXShift = (3*R2_DIST*R2_CM) + (3*R2_DEPTH*R2_CM) + (3*(R2_DEPTH*R2_CM)/4);
      fPShift = .5*R2_XDIST*R2_CM; // Half drift cell size
      break;
    }
    if (fPlane > 12) // If on second arm, draw in appropriate tab)
      fRegion2bXZ->GetCanvas()->cd();
    if (fWire < R2_FULLWIRE1){
      Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5) - (((R2_UVDIST*R2_CM*fWire) + fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R2_FULLWIRE2){
      Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (((R2_WIDTH*R2_CM) - (R2_UVDIST*R2_CM*(fWire - R2_FULLWIRE1)) - fPShift)*tan(R2_ANGLE*TMath::DegToRad())));
      Line.SetX2(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + fXShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    Line.SetLineColor(kBlue);
    Line.SetLineWidth(2);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
  }
  fRegion2XZ->GetCanvas()->SetEditable(kFALSE);
  fRegion2XZ->GetCanvas()->Update();
  fRegion2bXZ->GetCanvas()->SetEditable(kFALSE);
  fRegion2bXZ->GetCanvas()->Update();

  //DRAW REGION 3 WIRE HITS--------------------------------------------------//

  // FRONT VIEW (X-Y)
  fRegion3XY->GetCanvas()->cd();
  // Region 3 U wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3u->begin(); fHit != Hits_R3u->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fYShift = 0;             // Used to shift the lines to the correct chamber (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .65;
      break;
    case 3:
    case 4:
      fYShift = .75;
      break;
    case 5:
    case 6:
      fYShift = .25;
      break;
    case 7: 
    case 8:
      fYShift = .35;
      break;
    }
    if (fWire < R3_FULLWIRE1){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5) + ((R3_UVDIST*R3_CM*fWire)*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R3_FULLWIRE2){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5) - (((R3_WIDTH*R3_CM) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))))*tan(R3_ANGLE*TMath::DegToRad())));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  // Region 3 V wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3v->begin(); fHit != Hits_R3v->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fYShift = 0;             // Used to shift the lines to the correct chamber (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .65;
      break;
    case 3:
    case 4:
      fYShift = .75;
      break;
    case 5:
    case 6:
      fYShift = .25;
      break;
    case 7: 
    case 8:
      fYShift = .35;
      break;
    }
    if (fWire < R3_FULLWIRE1){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5) - ((R3_UVDIST*R3_CM*fWire)*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R3_FULLWIRE2){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5) + (((R3_WIDTH*R3_CM) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))))*tan(R3_ANGLE*TMath::DegToRad())));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
      }
    Line.SetLineColor(kBlue);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
  }
  fRegion3XY->GetCanvas()->SetEditable(kFALSE);
  fRegion3XY->GetCanvas()->Update();

  // TOP VIEW (Y-Z)
  fRegion3YZ->GetCanvas()->cd();
  // Region 3 U wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3u->begin(); fHit != Hits_R3u->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fYShift = 0;             // Used to shift the lines to the correct plane (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .655; // Wires not centered so its easier to see crossover
      break;
    case 3:
    case 4:
      fYShift = .755;
      break;
    case 5:
    case 6:
      fYShift = .255;
      break;
    case 7: 
    case 8:
      fYShift = .355;
      break;
    }
    if (fWire < R3_FULLWIRE1){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift);
    }
    else if (fWire < R3_FULLWIRE2){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift);
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift);
    }
    Line.SetLineColor(kGreen);
    Line.SetLineWidth(3); // Pixel depth of frame; easier to see
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  // Region 3 V wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3v->begin(); fHit != Hits_R3v->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fYShift = 0;             // Used to shift the lines to the correct plane (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .645; // Wires not centered so its easier to see crossover
      break;
    case 3:
    case 4:
      fYShift = .745;
      break;
    case 5:
    case 6:
      fYShift = .245;
      break;
    case 7: 
    case 8:
      fYShift = .345;
      break;
    }
    if (fWire < R3_FULLWIRE1){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift);
    }
    else if (fWire < R3_FULLWIRE2){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift);
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))));
      Line.SetY2(fYShift);
    }
    Line.SetLineColor(kBlue);
    Line.SetLineWidth(3); // Pixel depth of frame; easier to see
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  fRegion3YZ->GetCanvas()->SetEditable(kFALSE);
  fRegion3YZ->GetCanvas()->Update();

  // SIDE VIEW (X-Z)
  fRegion3XZ->GetCanvas()->cd();
  // Region 3 U wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3u->begin(); fHit != Hits_R3u->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fXShift = 0;             // Used to shift the lines to the correct plane (ROOT coordinates)
    double fYShift = 0;             // Used to shift the lines to the correct plane (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
      fXShift = .5 - (R3_DIST*R3_CM*.5) + .005; // Wires not centered so its easier to see crossover
      fYShift = .65;
      break;
    case 3:
    case 4:
      fXShift = .5 + (R3_DIST*R3_CM*.5) + .005;
      fYShift = .75;
      break;
    case 5:
    case 6:
      fXShift = .5 - (R3_DIST*R3_CM*.5) + .005;
      fYShift = .25;
      break;
    case 7: 
    case 8:
      fXShift = .5 + (R3_DIST*R3_CM*.5) + .005;
      fYShift = .35;
      break;
    }
    if (fWire < R3_FULLWIRE1){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5) + ((R3_UVDIST*R3_CM*fWire)*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R3_FULLWIRE2){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(fXShift);
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5) - (((R3_WIDTH*R3_CM) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))))*tan(R3_ANGLE*TMath::DegToRad())));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line.SetLineWidth(3); // Pixel depth of frame; easier to see
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  // Region 3 V wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3v->begin(); fHit != Hits_R3v->end(); fHit++){
    TLine Line;
    int fWire = fHit->GetElement(); // Wire number
    int fPlane = fHit->GetPlane();  // Plane number
    double fXShift = 0;             // Used to shift the lines to the correct plane (ROOT coordinates)
    double fYShift = 0;             // Used to shift the lines to the correct plane (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
      fXShift = .5 - (R3_DIST*R3_CM*.5) - .005; // Wires not centered so its easier to see crossover
      fYShift = .65;
      break;
    case 3:
    case 4:
      fXShift = .5 + (R3_DIST*R3_CM*.5) - .005;
      fYShift = .75;
      break;
    case 5:
    case 6:
      fXShift = .5 - (R3_DIST*R3_CM*.5) - .005;
      fYShift = .35;
      break;
    case 7: 
    case 8:
      fXShift = .5 + (R3_DIST*R3_CM*.5) - .005;
      fYShift = .25;
      break;
    }
    if (fWire < R3_FULLWIRE1){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5) - ((R3_UVDIST*R3_CM*fWire)*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < R3_FULLWIRE2){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(fXShift);
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5) + (((R3_WIDTH*R3_CM) - (R3_UVDIST*R3_CM*(fWire - (R3_FULLWIRE1 - 1))))*tan(R3_ANGLE*TMath::DegToRad())));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kBlue);
    Line.SetLineWidth(3); // Pixel depth of frame; easier to see
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  fRegion3XZ->GetCanvas()->SetEditable(kFALSE);
  fRegion3XZ->GetCanvas()->Update();
  
  // Delete hit list (because it is regenerated every time)
  delete fHitList; fHitList = 0;

  return 0;
} // End DrawEvent()

void QwGUIEventDisplay::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{

}


Bool_t QwGUIEventDisplay::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  return kTRUE;
}
