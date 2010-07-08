/************************************\
** Qweak 2D Event Display           **
** Jefferson Lab -- Hall C          **
** Author: Derek Jones              **
** The George Washington University **
** Contact: dwjones8@gwu.edu        **
\************************************/


////BRIEF////

// GUI macro that displays wire hit, timing, and other information [as requested] for Regions 1-3 of Qweak experiment
// Includes trigger scintillator information (position and timing)
// Provides 2D output of orthographic planes in each region for track reconstruction analysis
// May also be useful for diagnostics of tracking hardware


////HEADERS////

//Qweak Event Display Header
#include "QwEventDisplay.h"  // includes ROOT headers and common classes and variables for the event display
ClassImp(QwEventDisplay);  // imports the new class from the header to this code

//Qweak Header Files
#include "QwTreeEventBuffer.h"  // create hit list from G4 Monte Carlo
#include "QwSubsystemArrayTracking.h"  // update hit list for each subsystem
#include "QwDetectorInfo.h"  // establishes detector information
#include "QwHitContainer.h"  // creates augmented vector of QwHits for special functionality


////GRAPHICAL COMPONENTS////

QwEventDisplay::QwEventDisplay(const TGWindow *p,UInt_t w,UInt_t h){  // creates main graphical components

   // TRootGuiBuilder formatting variables
   ULong_t ucolor;        // will reflect user color changes
   TGFont *ufont;         // will reflect user font changes
   TGGC   *uGC;           // will reflect user GC changes

   // initialize variables to 0 (no draw)
   fEventNumber = 0;
   fSubsystemArray = 0;
   fEventBuffer = 0;
   fHitList = 0;

   // create new pop up window on screen; main frame
   fMain = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMain->SetLayoutBroken(kTRUE);
   fMain->SetWindowName("Qweak 2D Single Event Display");


   ////TITLE AND LOGOS////

   // create horizontal frame for logo and title placement: includes Qweak, JLab, and GWU logos
   gClient->GetColorByName("#ffffff",ucolor);  // set ucolor to white
   TGHorizontalFrame *fLogos = new TGHorizontalFrame(fMain,900,80,kHorizontalFrame | kRaisedFrame,ucolor);
   fLogos->SetLayoutBroken(kTRUE);

   // insert JLab logo
   TGIcon *JLabLogo = new TGIcon(fLogos,"/home/dwjones/Desktop/Logos/JLab_logo_white1.jpg");
   fLogos->AddFrame(JLabLogo, new TGLayoutHints(kLHintsNormal));
   JLabLogo->MoveResize(144,0,250,75);

   // insert Qweak logo
   TGIcon *QweakLogo = new TGIcon(fLogos,"/home/dwjones/Desktop/Logos/qweak.jpg");
   fLogos->AddFrame(QweakLogo, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   QweakLogo->MoveResize(0,0,155,75);

   // graphics context changes for "Qweak 2D Single Event Display" title label
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-25-*-*-*-*-*-*-*"); // set ufont to bold helvetica 25
   GCValues_t valBox;
   valBox.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valBox.fForeground);
   gClient->GetColorByName("#c0c0c0",valBox.fBackground);
   valBox.fFillStyle = kFillSolid;
   valBox.fFont = ufont->GetFontHandle();
   valBox.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valBox, kTRUE);

   // create title box for event display
   gClient->GetColorByName("#d45954",ucolor);  // set ucolor to soft red
   //   gClient->GetColorByName("#fff897",ucolor); // set ucolor to mute yellow
   //   gClient->GetColorByName("#deba87", ucolor); // set ucolor to mute orange
   TGLabel *TitleBox = new TGLabel(fLogos,"Qweak 2D Single Event Display",uGC->GetGC(),ufont->GetFontStruct(),kRaisedFrame,ucolor);
   TitleBox->SetTextJustify(36);
   TitleBox->SetMargins(0,0,0,0);
   TitleBox->SetWrapLength(-1);
   fLogos->AddFrame(TitleBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   TitleBox->MoveResize(385,0,384,75);

   // insert GWU logo
   TGIcon *GWULogo = new TGIcon(fLogos,"/home/dwjones/Desktop/Logos/gwu_logo_main.gif"); ////Logos from dwjones@jlab.org; taken from public domain
   fLogos->AddFrame(GWULogo, new TGLayoutHints(kLHintsNormal));
   GWULogo->MoveResize(772,0,135,75);

   // add fLogos frame to fMain and set color
   fMain->AddFrame(fLogos, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLogos->MoveResize(0,0,900,80);


   ////EVENT BOXES////

   // create horizontal frame for event boxes including event number, goto event function, wire hits, timing, and other information
   gClient->GetColorByName("#0047ab",ucolor);  // set ucolor to blue
   fEventBoxes = new TGHorizontalFrame(fMain,900,240,kHorizontalFrame,ucolor);
   fEventBoxes->SetLayoutBroken(kTRUE);


   //EVENT BOX 1--Event Number and Goto Event//

   // create vertical frame for event box 1
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff
   fEventBox1 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox1->SetLayoutBroken(kTRUE);

   // graphics context changes for "Event Counter" label
   gClient->GetColorByName("#deba87", ucolor); // set ucolor to mute orange
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // set ufont to bold charter 15
   GCValues_t valunterLabel;
   valunterLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valunterLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valunterLabel.fBackground);
   valunterLabel.fFillStyle = kFillSolid;
   valunterLabel.fFont = ufont->GetFontHandle();
   valunterLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valunterLabel, kTRUE);

   // create label for the event counter
   TGLabel *EventCounterLabel = new TGLabel(fEventBox1,"Event Counter",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   EventCounterLabel->SetTextJustify(36);
   EventCounterLabel->SetMargins(0,0,0,0);
   EventCounterLabel->SetWrapLength(-1);
   fEventBox1->AddFrame(EventCounterLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   EventCounterLabel->MoveResize(45,6,125,20);
   EventCounterLabel->SetBackgroundColor(ucolor);

   // create vertical frame for event counter information: current event number, goto event entry, and goto event button
   gClient->GetColorByName("#507ba7", ucolor); // set ucolor to navy blue
   fEventCounter = new TGVerticalFrame(fEventBox1,144,176,kVerticalFrame | kSunkenFrame,ucolor);
   fEventCounter->SetLayoutBroken(kTRUE);

   // graphics context changes for "Current Event" group frame
   ufont = gClient->GetFont("-*-courier-bold-r-*-*-12-*-*-*-*-*-*-*"); // set ufont to bold charter 10
   GCValues_t valentHit;
   valentHit.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valentHit.fForeground);
   gClient->GetColorByName("#c0c0c0",valentHit.fBackground);
   valentHit.fFillStyle = kFillSolid;
   valentHit.fFont = ufont->GetFontHandle();
   valentHit.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valentHit, kTRUE);

   // create "Current Event" group frame
   fCurrentEvent = new TGGroupFrame(fEventCounter,"Current Event",kVerticalFrame,uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   fCurrentEvent->SetLayoutBroken(kTRUE);

   // graphics context changes for "Current Event" label
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-33-*-*-*-*-*-*-*"); // set ufont to bold charter 33
   GCValues_t valntHitLabel;
   valntHitLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valntHitLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valntHitLabel.fBackground);
   valntHitLabel.fFillStyle = kFillSolid;
   valntHitLabel.fFont = ufont->GetFontHandle();
   valntHitLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valntHitLabel, kTRUE);
   gClient->GetColorByName("#fff897",ucolor); // set ucolor to mute yellow

   // create label for current event: variable changes based on fEventNumber
   CurrentEventLabel = new TGLabel(fCurrentEvent, Form("%u", (Int_t) fEventNumber), uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   CurrentEventLabel->SetTextJustify(36);
   CurrentEventLabel->SetMargins(0,0,0,0);
   CurrentEventLabel->SetWrapLength(-1);
   CurrentEventLabel->MoveResize(11,18,104,32);
   CurrentEventLabel->SetBackgroundColor(ucolor);

   // add current event label to "Current Event" group frame and customize
   fCurrentEvent->AddFrame(CurrentEventLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCurrentEvent->SetLayoutManager(new TGVerticalLayout(fCurrentEvent));

   // add "Current Event" group frame to event counter frame
   fEventCounter->AddFrame(fCurrentEvent, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCurrentEvent->MoveResize(8,8,126,64);

   // graphics context changes for "Goto Event" button
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-11-*-*-*-*-*-*-*"); // set ufont to bold helvetica 11
   GCValues_t valitButton;
   valitButton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valitButton.fForeground);
   gClient->GetColorByName("#c0c0c0",valitButton.fBackground);
   valitButton.fFillStyle = kFillSolid;
   valitButton.fFont = ufont->GetFontHandle();
   valitButton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valitButton, kTRUE);

   // create goto event button
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff
   TGTextButton *GotoEventButton = new TGTextButton(fEventCounter,"&Goto Event",-1,uGC->GetGC(),ufont->GetFontStruct());
   GotoEventButton->Connect("Clicked()", "QwEventDisplay", this, "GotoEvent()");
   GotoEventButton->SetTextJustify(36);
   GotoEventButton->SetMargins(0,0,0,0);
   GotoEventButton->SetWrapLength(-1);
   GotoEventButton->Resize(67,24);
   GotoEventButton->ChangeBackground(ucolor);

   // add goto event button to event counter frame
   fEventCounter->AddFrame(GotoEventButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   GotoEventButton->MoveResize(64,133,67,24);

   // create number entry for current events
   CurrentEventEntry = new TGNumberEntry(fEventCounter, (Double_t) 0,11,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
   fEventCounter->AddFrame(CurrentEventEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   CurrentEventEntry->MoveResize(24,94,96,22);
   //CurrentEventEntry->Connect("ValueSet(Long_t)", "QwEventDisplay()", this, "GotoEvent()");  // COMMENTED BECAUSE NOT USING THIS--POSSIBLE IN FUTURE
   //(CurrentEventEntry->GetNumberEntry())->Connect("ReturnPressed()", "QwEventDisplay()", this, "GotoEvent()");

   // add event counter frame to event box 1
   fEventBox1->AddFrame(fEventCounter, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventCounter->MoveResize(35,32,144,180);

   // add event box 1 to event box frame
   fEventBoxes->AddFrame(fEventBox1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBox1->MoveResize(3,6,220,226);


   //EVENT BOX 2--Wire Hits//

   // create vertical frame for event box 2
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff
   fEventBox2 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox2->SetLayoutBroken(kTRUE);

   // create list box for wire hit data
   WireHitListBox = new TGListBox(fEventBox2);
   //   WireHitListBox->Connect("PositionChanged()", "QwEventDisplay", this, "TimingListBox->fVScrollbar");  LINK SCROLLBARS TO BE IN SYNC
   WireHitListBox->Resize(200,180);
   fEventBox2->AddFrame(WireHitListBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   WireHitListBox->MoveResize(8,32,200,180);
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*");

   // graphics context changes for "Wire Hits" label
   GCValues_t valitsLabel;
   valitsLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valitsLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valitsLabel.fBackground);
   valitsLabel.fFillStyle = kFillSolid;
   valitsLabel.fFont = ufont->GetFontHandle();
   valitsLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valitsLabel, kTRUE);
   gClient->GetColorByName("#deba87", ucolor); // set ucolor to mute orange

   // create label for wire hits list box
   TGLabel *WireHitsLabel = new TGLabel(fEventBox2,"Wire Hits",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   WireHitsLabel->SetTextJustify(36);
   WireHitsLabel->SetMargins(0,0,0,0);
   WireHitsLabel->SetWrapLength(-1);
   WireHitsLabel->MoveResize(59,6,96,20);
   WireHitsLabel->SetBackgroundColor(ucolor);
   fEventBox2->AddFrame(WireHitsLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // add event box 2 to event boxes frame
   fEventBoxes->AddFrame(fEventBox2, new TGLayoutHints(kLHintsNormal));
   fEventBox2->MoveResize(227,6,220,226);

   // add event boxes frame to main frame
   fMain->AddFrame(fEventBoxes, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBoxes->MoveResize(0,80,900,240);


   //EVENT BOX 3--Timing//

   // create vertical frame for event box 3
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff
   TGVerticalFrame *fEventBox3 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox3->SetLayoutBroken(kTRUE);
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // set ufont to bold charter 15

   // graphics context changes for "Timing" label
   GCValues_t valgLabel;
   valgLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valgLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valgLabel.fBackground);
   valgLabel.fFillStyle = kFillSolid;
   valgLabel.fFont = ufont->GetFontHandle();
   valgLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valgLabel, kTRUE);

   // create label for timing list box
   gClient->GetColorByName("#deba87", ucolor); // set ucolor to mute orange
   TGLabel *TimingLabel = new TGLabel(fEventBox3,"Timing",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   TimingLabel->SetTextJustify(36);
   TimingLabel->SetMargins(0,0,0,0);
   TimingLabel->SetWrapLength(-1);
   TimingLabel->MoveResize(58,6,88,20);
   TimingLabel->SetBackgroundColor(ucolor);
   fEventBox3->AddFrame(TimingLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // create list box to display timing information
   TimingListBox = new TGListBox(fEventBox3);
   TimingListBox->Resize(200,180);
   fEventBox3->AddFrame(TimingListBox, new TGLayoutHints(kLHintsNormal));
   TimingListBox->MoveResize(8,32,200,180);

   // add event box 3 to event boxes frame
   fEventBoxes->AddFrame(fEventBox3, new TGLayoutHints(kLHintsNormal));
   fEventBox3->MoveResize(451,6,220,226);


   //EVENT BOX 4--Drift Distance

   // create vertical frame for event box 4
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff
   TGVerticalFrame *fEventBox4 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox4->SetLayoutBroken(kTRUE);

   // graphics context changes for "Drift Distance" label
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // set ufont to bold charter 15
   GCValues_t valInfoLabel;
   valInfoLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valInfoLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valInfoLabel.fBackground);
   valInfoLabel.fFillStyle = kFillSolid;
   valInfoLabel.fFont = ufont->GetFontHandle();
   valInfoLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valInfoLabel, kTRUE);
   gClient->GetColorByName("#deba87", ucolor); // set ucolor to mute orange

   // create label for other information list box
   TGLabel *DriftDistanceLabel = new TGLabel(fEventBox4,"Drift Distance",uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame,ucolor);
   DriftDistanceLabel->SetTextJustify(36);
   DriftDistanceLabel->SetMargins(0,0,0,0);
   DriftDistanceLabel->SetWrapLength(-1);
   DriftDistanceLabel->MoveResize(45,6,125,20);
   DriftDistanceLabel->SetBackgroundColor(ucolor);
   fEventBox4->AddFrame(DriftDistanceLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // create list box to display other information
   DriftDistanceListBox = new TGListBox(fEventBox4);  // RENAME VARIABLE ONCE I KNOW WHAT IT DOES
   DriftDistanceListBox->Resize(200,180);
   fEventBox4->AddFrame(DriftDistanceListBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   DriftDistanceListBox->MoveResize(8,32,200,180);
   fEventBoxes->AddFrame(fEventBox4, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBox4->MoveResize(675,6,220,226);


  ////REGION BOXES////

   // graphics context changes for fRegions tabs
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // set ufont to bold charter 10
   GCValues_t valTabs;
   valTabs.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valTabs.fForeground);
   gClient->GetColorByName("#c0c0c0",valTabs.fBackground);
   valTabs.fFillStyle = kFillSolid;
   valTabs.fFont = ufont->GetFontHandle();
   valTabs.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valTabs, kTRUE);
   gClient->GetColorByName("#0047ab",ucolor); // set ucolor to blue

   // create region boxes tab frame
   fRegions = new TGTab(fMain,900,312,uGC->GetGC(),ufont->GetFontStruct(),kChildFrame,ucolor);
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff

   //REGION 1--Gas Electron Multiplier//

   // create tab and container of "Region 1" data
   fRegion1 = fRegions->AddTab("Region 1--GEM");
   fRegion1->SetLayoutManager(new TGMatrixLayout(fRegion1,0,1,0,0));
   fRegion1->SetLayoutBroken(kTRUE);
   fRegion1->SetBackgroundColor(ucolor);


   // create embedded canvas for Region 1 XY view
   fRegion1XY = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1XY = fRegion1XY->GetCanvasWindowId();
   cR1XY= new TCanvas("cR1XY", 10, 10, wRegion1XY);
   fRegion1XY->AdoptCanvas(cR1XY);
   fRegion1->AddFrame(fRegion1XY, new TGLayoutHints(kLHintsNormal));
   fRegion1XY->MoveResize(12,4,280,280);
   fRegion1XY->GetCanvas()->SetFillColor(0);
   Label_R1XY = new TPaveLabel(.03,.94,.97,.99,"Front View (X-Y Projection)"); // create canvas label
   Label_R1XY->Draw();
   Box_R1XY = new TBox(.5-(R1_WIDTH*R1_CM*.5), .5-(R1_LENGTH*R1_CM*.5), .5+(R1_WIDTH*R1_CM*.5), .5+(R1_LENGTH*R1_CM*.5)); // create/center box to frame wire lines
   Box_R1XY->SetLineColor(1);
   Box_R1XY->SetFillStyle(0);
   Box_R1XY->Draw();
   fRegion1XY->GetCanvas()->SetEditable(kFALSE);

   // create embedded canvas for Region 1 YZ view
   fRegion1YZ = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1YZ = fRegion1YZ->GetCanvasWindowId();
   cR1YZ = new TCanvas("cR1YZ", 10, 10, wRegion1YZ);
   fRegion1YZ->AdoptCanvas(cR1YZ);
   fRegion1->AddFrame(fRegion1YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion1YZ->MoveResize(306,4,280,280);
   fRegion1YZ->GetCanvas()->SetFillColor(0);
   Label_R1YZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Y-Z Projection)"); // create canvas label
   Label_R1YZ->Draw();
   Box_R1YZ = new TBox(.5-(R1_WIDTH*R1_CM*.5), .5-(R1_DEPTH*R1_CM*.5), .5+(R1_WIDTH*R1_CM*.5), .5+(R1_DEPTH*R1_CM*.5)); // create/center box to frame wire lines
   Box_R1YZ->SetLineColor(1);
   Box_R1YZ->SetFillStyle(0);
   Box_R1YZ->Draw();
   fRegion1YZ->GetCanvas()->SetEditable(kFALSE);

   // create embedded canvas for Region 1 XZ view
   fRegion1XZ = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1XZ = fRegion1XZ->GetCanvasWindowId();
   cR1XZ  = new TCanvas("cR1XZ", 10, 10, wRegion1XZ);
   fRegion1XZ->AdoptCanvas(cR1XZ);
   fRegion1->AddFrame(fRegion1XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion1XZ->MoveResize(603,4,280,280);
   fRegion1XZ->GetCanvas()->SetFillColor(0);
   Label_R1XZ = new TPaveLabel(.03,.94,.97,.99,"Side View (X-Z Projection)"); // create canvas label
   Label_R1XZ->Draw();
   Box_R1XZ = new TBox(.5-(R1_DEPTH*R1_CM*.5), .5-(R1_LENGTH*R1_CM*.5), .5+(R1_DEPTH*R1_CM*.5), .5+(R1_LENGTH*R1_CM*.5)); // create/center box to frame wire lines
   Box_R1XZ->SetLineColor(1);
   Box_R1XZ->SetFillStyle(0);
   Box_R1XZ->Draw();
   fRegion1XZ->GetCanvas()->SetEditable(kFALSE);

   //REGION 2--Horizontal Drift Chamber//

   // create tab and container of "Region 2" data
   fRegion2 = fRegions->AddTab("Region 2--HDC");
   fRegion2->SetLayoutManager(new TGVerticalLayout(fRegion2));
   fRegion2->SetLayoutBroken(kTRUE);
   fRegion2->SetBackgroundColor(ucolor);

   // create embedded canvas for Region 2 XY view
   fRegion2XY = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2XY = fRegion2XY->GetCanvasWindowId();
   cR2XY = new TCanvas("cR2XY", 10, 10, wRegion2XY);
   fRegion2XY->AdoptCanvas(cR2XY);
   fRegion2->AddFrame(fRegion2XY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2XY->MoveResize(12,4,280,280);
   fRegion2XY->GetCanvas()->SetFillColor(0);
   Label_R2XY = new TPaveLabel(.03,.94,.97,.99,"Front View (X-Y Projection)"); // create canvas label
   Label_R2XY->Draw();
   Box_R2XY = new TBox(.5-(R2_WIDTH*R2_CM*.5), .5-(R2_LENGTH*R2_CM*.5), .5+(R2_WIDTH*R2_CM*.5), .5+(R2_LENGTH*R2_CM*.5)); // create/center box to frame wire lines
   Box_R2XY->SetLineColor(1);
   Box_R2XY->SetFillStyle(0);
   Box_R2XY->Draw();
   fRegion2XY->GetCanvas()->SetEditable(kFALSE);

   // create embedded canvas for Region 2 YZ view
   fRegion2YZ = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2YZ = fRegion2YZ->GetCanvasWindowId();
   cR2YZ = new TCanvas("cR2YZ", 10, 10, wRegion2YZ);
   fRegion2YZ->AdoptCanvas(cR2YZ);
   fRegion2->AddFrame(fRegion2YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2YZ->MoveResize(306,4,280,280);
   fRegion2YZ->GetCanvas()->SetFillColor(0);
   Label_R2YZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Y-Z Projection)"); // create canvas label
   Label_R2YZ->Draw();
   Box_R2YZ1 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5),.5 - (1.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM)); // create box for chamber 1
   Box_R2YZ1->SetLineColor(1);
   Box_R2YZ1->SetFillStyle(0);
   Box_R2YZ1->Draw();
   Box_R2YZ2 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM)); // create box for chamber 2
   Box_R2YZ2->SetLineColor(7);
   Box_R2YZ2->SetFillStyle(0);
   Box_R2YZ2->Draw();
   Box_R2YZ3 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM)); // create box for chamber 3
   Box_R2YZ3->SetLineColor(6);
   Box_R2YZ3->SetFillStyle(0);
   Box_R2YZ3->Draw();
   Box_R2YZ4 = new TBox(.5 - (R2_WIDTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 + (R2_WIDTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM)); // create box for chamber 4
   Box_R2YZ4->SetLineColor(8);
   Box_R2YZ4->SetFillStyle(0);
   Box_R2YZ4->Draw();
   tR2XY1 = new TText(.03, .03, "Cham. 1"); // color-coded text for HDC chamber labels
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

   // create embedded canvas for Region 2 XZ view
   fRegion2XZ = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2XZ = fRegion2XZ->GetCanvasWindowId();
   cR2XZ = new TCanvas("cR2XZ",10, 10, wRegion2XZ);
   fRegion2XZ->AdoptCanvas(cR2XZ);
   fRegion2->AddFrame(fRegion2XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2XZ->MoveResize(603,4,280,280);
   fRegion2XZ->GetCanvas()->SetFillColor(0);
   Label_R2XZ = new TPaveLabel(.03,.94,.97,.99,"Side View (X-Z Projection)"); // create canvas label
   Label_R2XZ->Draw();
   Box_R2XZ1 = new TBox(.5 + (1.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 + (1.5*R2_DIST*R2_CM) + (2*R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // create box for chamber 1 // incorporate X and Y shift (ROOT coord)
   Box_R2XZ1->SetLineColor(1);
   Box_R2XZ1->SetFillStyle(0);
   Box_R2XZ1->Draw();
   Box_R2XZ2 = new TBox(.5 + (.5*R2_DIST*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 + (.5*R2_DIST*R2_CM) + (R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // create box for chamber 2
   Box_R2XZ2->SetLineColor(7);
   Box_R2XZ2->SetFillStyle(0);
   Box_R2XZ2->Draw();
   Box_R2XZ3 = new TBox(.5 - (.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 - (.5*R2_DIST*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // create box for chamber 3
   Box_R2XZ3->SetLineColor(6);
   Box_R2XZ3->SetFillStyle(0);
   Box_R2XZ3->Draw();
   Box_R2XZ4 = new TBox(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM), .5 - (R2_LENGTH*R2_CM*.5), .5 - (1.5*R2_DIST*R2_CM) - (R2_DEPTH*R2_CM), .5 + (R2_LENGTH*R2_CM*.5)); // create box for chamber 4
   Box_R2XZ4->SetLineColor(8);
   Box_R2XZ4->SetFillStyle(0);
   Box_R2XZ4->Draw();
   tR2XZ1 = new TText(.03, .03, "Cham. 1"); // color-coded text for HDC chamber labels
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

   //REGION 3--Vertical Drift Chamber//

   // create tab and container of "Region 3" VDC data
   fRegion3 = fRegions->AddTab("Region 3--VDC");
   fRegion3->SetLayoutManager(new TGVerticalLayout(fRegion3));
   fRegion3->SetLayoutBroken(kTRUE);
   fRegion3->SetBackgroundColor(ucolor);

   // create embedded canvas for Region 3 XY view                       /////////REMOVE HARD-CODED NUMBERS!!!
   fRegion3XY = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3XY = fRegion3XY->GetCanvasWindowId();
   cR3XY = new TCanvas("cR3XY", 10, 10, wRegion3XY);
   fRegion3XY->AdoptCanvas(cR3XY);
   fRegion3->AddFrame(fRegion3XY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3XY->MoveResize(12,4,280,280);
   fRegion3XY->GetCanvas()->SetFillColor(0);
   Label_R3XY = new TPaveLabel(.03,.94,.97,.99,"Front View (X-Y Projection)"); // create canvas label
   Label_R3XY->Draw();
   Box_R3XY1 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .65-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .65+(R3_LENGTH*R3_CM*.5)); // create box for chamber 1
   Box_R3XY1->SetLineColor(1);
   Box_R3XY1->SetFillStyle(0);
   Box_R3XY2 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .75-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .75+(R3_LENGTH*R3_CM*.5)); // create box for chamber 2
   Box_R3XY2->SetLineColor(7);
   Box_R3XY2->SetFillStyle(0);
   Box_R3XY2->Draw();
   Box_R3XY1->Draw(); // put 1 on top
   Box_R3XY3 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .25-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .25+(R3_LENGTH*R3_CM*.5)); // create box for chamber 3
   Box_R3XY3->SetLineColor(6);
   Box_R3XY3->SetFillStyle(0);
   Box_R3XY4 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .35-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .35+(R3_LENGTH*R3_CM*.5)); // create box for chamber 4
   Box_R3XY4->SetLineColor(8);
   Box_R3XY4->SetFillStyle(0);
   Box_R3XY4->Draw();
   Box_R3XY3->Draw(); // put 3 on top
   tR3XY1 = new TText(.03, .03, "Leia (1)"); // color-coded text for VDC chamber labels
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

   // create embedded canvas for Region 3 YZ view
   fRegion3YZ = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3YZ = fRegion3YZ->GetCanvasWindowId();
   cR3YZ = new TCanvas("cR3YZ", 10, 10, wRegion3YZ);
   fRegion3YZ->AdoptCanvas(cR3YZ);
   fRegion3->AddFrame(fRegion3YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3YZ->MoveResize(306,4,280,280);
   fRegion3YZ->GetCanvas()->SetFillColor(0);
   Label_R3YZ = new TPaveLabel(.03,.94,.97,.99,"Top View (Y-Z Projection)"); // create canvas label
   Label_R3YZ->Draw();
   Box_R3YZ1 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .65-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .65+(R3_DEPTH*R3_CM*.5)); // create box for chamber 1
   Box_R3YZ1->SetLineColor(1);
   Box_R3YZ1->SetFillStyle(0);
   Box_R3YZ2 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .75-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .75+(R3_DEPTH*R3_CM*.5)); // create box for chamber 2
   Box_R3YZ2->SetLineColor(7);
   Box_R3YZ2->SetFillStyle(0);
   Box_R3YZ2->Draw();
   Box_R3YZ1->Draw(); // put 1 on top
   Box_R3YZ3 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .25-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .25+(R3_DEPTH*R3_CM*.5)); // create box for chamber 3
   Box_R3YZ3->SetLineColor(6);
   Box_R3YZ3->SetFillStyle(0);
   Box_R3YZ4 = new TBox(.5-(R3_WIDTH*R3_CM*.5), .35-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .35+(R3_DEPTH*R3_CM*.5)); // create box for chamber 4
   Box_R3YZ4->SetLineColor(8);
   Box_R3YZ4->SetFillStyle(0);
   Box_R3YZ4->Draw();
   Box_R3YZ3->Draw(); // put 3 on top
   tR3YZ1 = new TText(.03, .03, "Leia (1)"); // color-coded text for VDC chamber labels
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

   // create embedded canvas for Region 3 XZ view
   fRegion3XZ = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3XZ = fRegion3XZ->GetCanvasWindowId();
   cR3XZ = new TCanvas("cR3XZ", 10, 10, wRegion3XZ);
   fRegion3XZ->AdoptCanvas(cR3XZ);
   fRegion3->AddFrame(fRegion3XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3XZ->MoveResize(603,4,280,280);
   fRegion3XZ->GetCanvas()->SetFillColor(0);
   Label_R3XZ = new TPaveLabel(.03,.94,.97,.99,"Side View (X-Z Projection)"); // create canvas label
   Label_R3XZ->Draw();
   Box_R3XZ1 = new TBox(.5-(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .65-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .65+(R3_LENGTH*R3_CM*.5)); // create box for chamber 1 // incorporate X and Y shift (ROOT coord)
   Box_R3XZ1->SetLineColor(1);
   Box_R3XZ1->SetFillStyle(0);
   Box_R3XZ2 = new TBox(.5-(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .75-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .75+(R3_LENGTH*R3_CM*.5)); // create box for chamber 2
   Box_R3XZ2->SetLineColor(7);
   Box_R3XZ2->SetFillStyle(0);
   Box_R3XZ2->Draw();
   Box_R3XZ1->Draw(); // put 1 on top
   Box_R3XZ3 = new TBox(.5-(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .35-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)+(R3_DIST*R3_CM*.5), .35+(R3_LENGTH*R3_CM*.5)); // create box for chamber 3
   Box_R3XZ3->SetLineColor(6);
   Box_R3XZ3->SetFillStyle(0);
   Box_R3XZ4 = new TBox(.5-(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .25-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)-(R3_DIST*R3_CM*.5), .25+(R3_LENGTH*R3_CM*.5)); // create box for chamber 4
   Box_R3XZ4->SetLineColor(8);
   Box_R3XZ4->SetFillStyle(0);
   Box_R3XZ4->Draw();
   Box_R3XZ3->Draw(); // put 3 on top
   tR3XZ1 = new TText(.03, .03, "Leia (1)"); // color-coded text for VDC chamber labels
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

   //REGION 3--Trigger Scintillator//

   // create tab and container of "Region 3" TS data
   fRegion3b = fRegions->AddTab("Region 3--TS");
   fRegion3b->SetLayoutManager(new TGVerticalLayout(fRegion3b));
   fRegion3b->SetLayoutBroken(kTRUE);
   fRegion3b->SetBackgroundColor(ucolor);

   // set tab attributes
   fRegions->SetTab(0);
   fRegions->Resize(fRegions->GetDefaultSize());
   fMain->AddFrame(fRegions, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegions->MoveResize(0,320,900,312);
   gClient->GetColorByName("#deba87", ucolor); // set ucolor to mute orange
   fRegions->GetTabTab("Region 1--GEM")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 2--HDC")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 3--VDC")->SetBackgroundColor(ucolor);
   fRegions->GetTabTab("Region 3--TS")->SetBackgroundColor(ucolor);


   ////BUTTONS////

   // create horizontal frame for buttons
   gClient->GetColorByName("#0047ab",ucolor);
   TGHorizontalFrame *fButtons = new TGHorizontalFrame(fMain,900,60,kHorizontalFrame,ucolor);
   fButtons->SetLayoutBroken(kTRUE);
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*"); // set ufont to bold helvetica 12
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff

   /* CLEAR BUTTON REDUNDANT?
   // graphics context changes for "Clear Data" button
   GCValues_t valButton;
   valButton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valButton.fForeground);
   gClient->GetColorByName("#c0c0c0",valButton.fBackground);
   valButton.fFillStyle = kFillSolid;
   valButton.fFont = ufont->GetFontHandle();
   valButton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valButton, kTRUE);
   // create clear button: CLEARS ALL DATA
   TGTextButton *ClearButton = new TGTextButton(fButtons,"&Clear Data",-1,uGC->GetGC(),ufont->GetFontStruct());
   ClearButton->Connect("Clicked()", "QwEventDisplay", this, "GoClear()"); // connect button to GoClear() function
   ClearButton->SetTextJustify(36);
   ClearButton->SetMargins(0,0,0,0);
   ClearButton->SetWrapLength(-1);
   ClearButton->Resize(100,50);
   ClearButton->ChangeBackground(ucolor);
   // add clear button to frame
   fButtons->AddFrame(ClearButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   ClearButton->MoveResize(545,6,100,50);
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*"); // set ufont to bold helvetica 12
   */

   // graphics context changes for "Exit" button
   GCValues_t valutton;
   valutton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valutton.fForeground);
   gClient->GetColorByName("#c0c0c0",valutton.fBackground);
   valutton.fFillStyle = kFillSolid;
   valutton.fFont = ufont->GetFontHandle();
   valutton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valutton, kTRUE);
   // create exit button: CLOSES PROGRAM
   TGTextButton *ExitButton = new TGTextButton(fButtons,"&Exit","gApplication->Terminate(0)");
   ExitButton->SetFont(ufont->GetFontStruct()); //set ufont
   ExitButton->SetTextJustify(36);
   ExitButton->SetMargins(0,0,0,0);
   ExitButton->SetWrapLength(-1);
   ExitButton->Resize(100,50);
   ExitButton->ChangeBackground(ucolor);
   // add exit button to frame
   fButtons->AddFrame(ExitButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   ExitButton->MoveResize(784,6,100,50);


   // graphics context changes for "Previous Event" button
   GCValues_t valousButton;
   valousButton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valousButton.fForeground);
   gClient->GetColorByName("#c0c0c0",valousButton.fBackground);
   valousButton.fFillStyle = kFillSolid;
   valousButton.fFont = ufont->GetFontHandle();
   valousButton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valousButton, kTRUE);
   // create previous button: MAKES EVENT COUNTER MINUS ONE
   TGTextButton *PreviousButton = new TGTextButton(fButtons,"&Previous Event",-1,uGC->GetGC(),ufont->GetFontStruct());
   PreviousButton->Connect("Clicked()", "QwEventDisplay", this, "GoPrevious()"); // connect button to GoPrevious() function
   PreviousButton->SetTextJustify(36);
   PreviousButton->SetMargins(0,0,0,0);
   PreviousButton->SetWrapLength(-1);
   PreviousButton->Resize(100,50);
   PreviousButton->ChangeBackground(ucolor);
   // add previous button to frame
   fButtons->AddFrame(PreviousButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   PreviousButton->MoveResize(12,6,100,50);


   // graphics context changes for "Next Event" button
   valutton.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valutton.fForeground);
   gClient->GetColorByName("#c0c0c0",valutton.fBackground);
   valutton.fFillStyle = kFillSolid;
   valutton.fFont = ufont->GetFontHandle();
   valutton.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valutton, kTRUE);
   // create next button: MAKES EVENT COUNTER PLUS ONE
   TGTextButton *NextButton = new TGTextButton(fButtons,"&Next Event",-1,uGC->GetGC(),ufont->GetFontStruct());
   NextButton->Connect("Clicked()", "QwEventDisplay", this, "GoNext()"); // connect button to GoNext() function
   NextButton->SetTextJustify(36);
   NextButton->SetMargins(0,0,0,0);
   NextButton->SetWrapLength(-1);
   NextButton->Resize(100,50);
   NextButton->ChangeBackground(ucolor);
   // add next button to frame
   fButtons->AddFrame(NextButton, new TGLayoutHints(kLHintsNormal));
   NextButton->MoveResize(125,6,100,50);

   // add buttons frame to main frame
   fMain->AddFrame(fButtons, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fButtons->MoveResize(0,632,900,60);


   //FINAL TOUCHES//

   // create embedded canvas to display key for region data
   TRootEmbeddedCanvas *fKey = new TRootEmbeddedCanvas(0,fButtons,280,280);
   Int_t wKey = fKey->GetCanvasWindowId();
   TCanvas *cKey = new TCanvas("cKey", 10, 10, wKey);
   fKey->AdoptCanvas(cKey);
   fButtons->AddFrame(fKey, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fKey->MoveResize(245,6,515,50);
   // fill in details
   fKey->GetCanvas()->cd();
   fKey->GetCanvas()->SetFillColor(0);
   TPaveLabel *Label_KeyX = new TPaveLabel(.01,.05,.24,.95," X"); // create key label for X info
   Label_KeyX->SetTextAlign(12);
   Label_KeyX->SetTextColor(kRed);
   //   Label_KeyX->SetFillColor(0);
   Label_KeyX->SetShadowColor(0);
   Label_KeyX->Draw();
   TLine *Line_KeyX = new TLine(.1,.5,.2,.5);
   Line_KeyX->SetLineColor(kRed);
   Line_KeyX->Draw();
   TPaveLabel *Label_KeyY = new TPaveLabel(.25,.05,.49,.95," Y"); // create key label for Y info
   Label_KeyY->SetTextAlign(12);
   Label_KeyY->SetTextColor(kViolet);
   //   Label_KeyY->SetFillColor(0);
   Label_KeyY->SetShadowColor(0);
   Label_KeyY->Draw();
   TLine *Line_KeyY = new TLine(.4,.1,.4,.9);
   Line_KeyY->SetLineColor(kViolet);
   Line_KeyY->Draw();
   TPaveLabel *Label_KeyU = new TPaveLabel(.5,.05,.74,.95," U"); // create key label for U info
   Label_KeyU->SetTextAlign(12);
   Label_KeyU->SetTextColor(kGreen);
   //   Label_KeyU->SetFillColor(0);
   Label_KeyU->SetShadowColor(0);
   Label_KeyU->Draw();
   TLine *Line_KeyU = new TLine(.59,.1,.7,.9);
   Line_KeyU->SetLineColor(kGreen);
   Line_KeyU->Draw();
   TPaveLabel *Label_KeyV = new TPaveLabel(.75,.05,.99,.95," V"); // create key label for V info
   Label_KeyV->SetTextAlign(12);
   Label_KeyV->SetTextColor(kBlue);
   //   Label_KeyV->SetFillColor(0);
   Label_KeyV->SetShadowColor(0);
   Label_KeyV->Draw();
   TLine *Line_KeyV = new TLine(.84,.9,.95,.1);
   Line_KeyV->SetLineColor(kBlue);
   Line_KeyV->Draw();
   fKey->GetCanvas()->SetEditable(kFALSE);
   fKey->GetCanvas()->Update();

   // add main frame to the pop up window and customize
   fMain->SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
   fMain->MapSubwindows();
   fMain->Resize(fMain->GetDefaultSize());
   fMain->MapWindow();
   fMain->Resize(900,692);
} // end QwEventDisplay()


////FUNCTIONAL COMPONENTS////

void QwEventDisplay::GoPrevious(){  // makes event counter minus one
                                      // called by PreviousButton click

  if (fEventNumber > 1){
    fEventNumber--;  // go to previous event
    DrawEvent();  // update display information
  }

  else{
    fEventNumber = 0;  // go to null event
    GoClear(); // event 0 is null event
  }

} // end GoPrevious()


void QwEventDisplay::GoNext(){  // makes event counter plus one
                                  // called by NextButton click

  fEventNumber++;  // go to next event
  DrawEvent();  // update display information
} // end GoNext()


void QwEventDisplay::GoClear(){  // clears all displayed data

  CurrentEventLabel->SetText(Form("%d", fEventNumber)); // updates layout to show current event number

  WireHitListBox->RemoveAll(); // clear event box 2
  TimingListBox->RemoveAll(); // clear event box 3
  DriftDistanceListBox->RemoveAll(); // clear event box 4

  Line_R1r.clear(); // clear existing Region 1 vectors
  Line_R1y.clear();
  R1_XYfit.clear();
  R1_XZfit.clear();
  Line_R2x.clear(); // clear existing Region 2 vectors
  Line_R2u.clear();
  Line_R2v.clear();
  Line_R3u.clear(); // clear existing Region 3 vectors
  Line_R3v.clear();

  fRegion1XY->GetCanvas()->Update(); // clear existing information and add new event information
  fRegion1XZ->GetCanvas()->Update();
  fRegion1YZ->GetCanvas()->Update();
  fRegion2XY->GetCanvas()->Update();
  fRegion2XZ->GetCanvas()->Update();
  fRegion2YZ->GetCanvas()->Update();
  fRegion3XY->GetCanvas()->Update();
  fRegion3XZ->GetCanvas()->Update();
  fRegion3YZ->GetCanvas()->Update();

} // end GoClear()


void QwEventDisplay::GotoEvent(){  // goes to desired event number written in "goto event" text entry box
                                     // called by GotoEventButton click

  fEventNumber = CurrentEventEntry->GetNumberEntry()->GetIntNumber(); // fEventNumber takes value from number entry

  if (fEventNumber > 0){
    CurrentEventLabel->SetText(Form("%d",CurrentEventEntry->GetNumberEntry()->GetIntNumber())); // set current event label
    DrawEvent();  // update display information
  }

  else{
    fEventNumber = 0; // go to null event
    GoClear(); // event 0 is null event
  }

} // end GotoEvent()


void QwEventDisplay::DrawEvent(){  // draws event data into display: lists wire hits, timing, and other information in event boxes
                                     // displays orthographic views triggered wires for each region
                                     // called by Previous, Next, or GotoEvent buttons  // NOT COMPLETE

  fRegion1XY->GetCanvas()->SetEditable(kTRUE);  // restore editing capability
  fRegion1XZ->GetCanvas()->SetEditable(kTRUE);
  fRegion1YZ->GetCanvas()->SetEditable(kTRUE);
  fRegion2XY->GetCanvas()->SetEditable(kTRUE);
  fRegion2XZ->GetCanvas()->SetEditable(kTRUE);
  fRegion2YZ->GetCanvas()->SetEditable(kTRUE);
  fRegion3XY->GetCanvas()->SetEditable(kTRUE);
  fRegion3XZ->GetCanvas()->SetEditable(kTRUE);
  fRegion3YZ->GetCanvas()->SetEditable(kTRUE);

  GoClear(); // clear existing data


  //LIST WIRE HITS//
  // extract wire hit information
  fEventBuffer->GetSpecificEvent(fEventNumber);   // place the current event number into the event buffer
  fHitList = fEventBuffer->GetHitContainer();  // add the hit list from the current event buffer
  fHitList->Print();  // outputs the new wire hit information from the hit list

  // create a char string buffer to hold hit information
  char HitBuffer[30];

  // list wire hits in wire list hit box
  for(QwHitContainer::iterator fHit = fHitList->begin(); fHit != fHitList->end(); fHit++){ // loop while hits exist in the hit container
    if(fHit->GetElement() > 0){
      if (fHit->GetRegion() == 1){
      sprintf(HitBuffer, "Region %i: Trace %i, Plane %i", fHit->GetRegion(), fHit->GetElement(), fHit->GetPlane()); // print single hit for Region 1
      }
      else if (fHit->GetRegion() == 2){
      sprintf(HitBuffer, "Region %i: Wire %i, Plane %i", fHit->GetRegion(), fHit->GetElement(), fHit->GetPlane()); // print single hit for Region 2
      }
      else if (fHit->GetRegion() == 3){
      sprintf(HitBuffer, "Region %i: Wire %i, Plane %i", fHit->GetRegion(), fHit->GetElement(), fHit->GetPlane()); // print single hit for Region 3
      }
      //      else{
      //      }
      WireHitListBox->AddEntry(HitBuffer, fEventNumber); // add to list box
      WireHitListBox->MapSubwindows(); // re-map window
      WireHitListBox->Layout();
    } // end if
  } //end for

  //LIST TIMING DATA//
  for(QwHitContainer::iterator fHit = fHitList->begin(); fHit != fHitList->end(); fHit++){ // loop while hits exist in the hit container
    if(fHit->GetElement() > 0){
      sprintf(HitBuffer, "Time: %lf, Raw: %u ns", fHit->GetTime(), fHit->GetRawTime()); // print timing info
      TimingListBox->AddEntry(HitBuffer, fEventNumber); // add to list box
      TimingListBox->MapSubwindows(); // re-map window
      TimingListBox->Layout();
    } // end if
  } //end for

  //LIST DRIFT DISTANCE DATA//
  for(QwHitContainer::iterator fHit = fHitList->begin(); fHit != fHitList->end(); fHit++){ // loop while hits exist in the hit container
    if(fHit->GetElement() > 0){
      sprintf(HitBuffer, "Drift Distance: %lf cm", fHit->GetDriftDistance()); // print single drift distance with wire hit
      DriftDistanceListBox->AddEntry(HitBuffer, fEventNumber); // add to list box
      DriftDistanceListBox->MapSubwindows(); // re-map window
      DriftDistanceListBox->Layout();
    } // end if
  } //end for

  //GATHER WIRE HIT DATA//
  // Region 1 hits
  QwHitContainer* Hits_R1r = fHitList->GetSubList_Dir(kRegionID1, kPackageUp, kDirectionR);  // Hits_R[Region #][Plane]
  QwHitContainer* Hits_R1y = fHitList->GetSubList_Dir(kRegionID1, kPackageUp, kDirectionY);
  // Region 2 hits
  QwHitContainer* Hits_R2x = fHitList->GetSubList_Dir(kRegionID2, kPackageUp, kDirectionX);
  QwHitContainer* Hits_R2u = fHitList->GetSubList_Dir(kRegionID2, kPackageUp, kDirectionU);
  QwHitContainer* Hits_R2v = fHitList->GetSubList_Dir(kRegionID2, kPackageUp, kDirectionV);
  // Region 3 hits
  QwHitContainer* Hits_R3u = fHitList->GetSubList_Dir(kRegionID3, kPackageUp, kDirectionU);
  QwHitContainer* Hits_R3v = fHitList->GetSubList_Dir(kRegionID3, kPackageUp, kDirectionV);

  //DRAW REGION 1 WIRE HITS//

  //FRONT VIEW (X-Y)
  fRegion1XY->GetCanvas()->cd();
  //Region 1 X traces (??? total)
  for(QwHitContainer::iterator fHit = Hits_R1r->begin(); fHit != Hits_R1r->end(); fHit++){
    //WHAT ARE THESE ACTUALLY? G4 DOES NOT GET ANY HITS FROM THESE
  }
  //Region 1 Y traces (??? total)
  //  sum = 0; // back to 0 for fitting
  //  num = 0;
  for(QwHitContainer::iterator fHit = Hits_R1y->begin(); fHit != Hits_R1y->end(); fHit++){
    int fWire = fHit->GetElement();
    //    double fDist = fHit->GetDriftDistance();
    TLine Line;
    Line.SetY1(.5 - (R1_WIDTH*R1_CM*.5));
    Line.SetX1(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fWire));
    Line.SetY2(.5 + (R1_WIDTH*R1_CM*.5));
    Line.SetX2(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fWire));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line); // add Line to end of vector R2x
    Line_R1y.back().Draw(); // draw line;
    //    sum = sum + wire;
    //    num++;
  }
/*fit = sum/num;
  Line.SetX1(.5 - (R1_WIDTH*R1_CM*.5));
  Line.SetY1(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetX2(.5 + (R1_WIDTH*R1_CM*.5));
  Line.SetY2(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetLineColor(kBlack);
  R1_XYfit.push_back(Line);
  R1_XYfit.back().Draw();*/
  fRegion1XY->GetCanvas()->SetEditable(kFALSE);
  fRegion1XY->GetCanvas()->Update();

  //TOP VIEW (Y-Z)
  fRegion1YZ->GetCanvas()->cd();
  //Region 1 Y wires
  for(QwHitContainer::iterator fHit = Hits_R1y->begin(); fHit != Hits_R1y->end(); fHit++){
    int fWire = fHit->GetElement();
    //    double fDist = fHit->GetDriftDistance();
    TLine Line;
    Line.SetY1(.5 - (R1_DEPTH*R1_CM*.5));
    Line.SetX1(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fWire));
    Line.SetY2(.5 + (R1_DEPTH*R1_CM*.5));
    Line.SetX2(.5 - (R1_LENGTH*R1_CM*.5) + (R1_DIST*R1_CM*fWire));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line);
    Line_R1y.back().Draw();
  }
  fRegion1YZ->GetCanvas()->SetEditable(kFALSE);
  fRegion1YZ->GetCanvas()->Update();

  //SIDE VIEW (X-Z)
  //  sum = 0; // back to 0 for fitting
  //  num = 0;
  fRegion1XZ->GetCanvas()->cd();
  //Region 1 Y wires
  for(QwHitContainer::iterator fHit = Hits_R1y->begin(); fHit != Hits_R1y->end(); fHit++){
    TLine Line;
    Line.SetX1(.5);
    Line.SetY1(.5 - (R1_LENGTH*R1_CM*.5));
    Line.SetX2(.5);
    Line.SetY2(.5 + (R1_LENGTH*R1_CM*.5));
    Line.SetLineColor(kViolet);
    Line.SetLineWidth(22); // to fill whole plane; 22 is similar to R1_DEPTH
    Line_R1y.push_back(Line); // add Line to end of vector R2x
    Line_R1y.back().Draw(); // draw line;
    //    sum = sum + wire;
    //    num++;
  }
/*fit = sum/num;
  Line.SetX1(.5 - (R1_WIDTH*R1_CM*.5));
  Line.SetY1(.5 - (R1_DEPTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetX2(.5 - (R1_WIDTH*R1_CM*.5));
  Line.SetY2(.5 + (R1_DEPTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetLineColor(kBlack);
  R1_XZfit.push_back(Line);
  R1_XZfit.back().Draw();*/
  fRegion1XZ->GetCanvas()->SetEditable(kFALSE);
  fRegion1XZ->GetCanvas()->Update();

  //DRAW REGION 2 WIRE HITS//

  //FRONT VIEW (X-Y)
  fRegion2XY->GetCanvas()->cd();
  //Region 2 X wires (32 total)
  for(QwHitContainer::iterator fHit = Hits_R2x->begin(); fHit != Hits_R2x->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane(); // wire plane
    //    double fDist = fHit->GetDriftDistance();
    double fYShift = 0.0; // used to shift the wires on the prime planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fYShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fYShift = .5*R2_XDIST*R2_CM; // half drift cell size
      break;
    case 7:
    case 8:
    case 9:
      fYShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fYShift = .5*R2_XDIST*R2_CM;
      break;
    }
    TLine Line;
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fYShift);
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fYShift);
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line); // add Line to end of vector R2x
    Line_R2x.back().Draw(); // draw line;
  }
  //Region 2 U wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2u->begin(); fHit != Hits_R2u->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane(); // wire plane
    //    double fDist = fHit->GetDriftDistance();
    double fXShift = 0.0; // used to shift the wires on the prime planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fXShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    case 7:
    case 8:
    case 9:
      fXShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    }
    TLine Line;
    if (fWire < 12){  //12th wire ends at corner (X2, Y2)   // ISSUES HERE
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - fXShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (((R2_UVDIST*R2_CM*fWire) + fXShift)*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 18){  //18th wire begins at (X1, Y1)   // ISSUES HERE
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - ((R2_UVDIST*R2_CM*(fWire-11.6)) - fXShift) - fXShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5) + fXShift);
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_UVDIST*R2_CM*(fWire-17.4)));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-11.6) - fXShift) - fXShift); // ISSUES HERE
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  //Region 2 V wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2v->begin(); fHit != Hits_R2v->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane(); // wire plane
    //    double fDist = fHit->GetDriftDistance();
    double fXShift; // used to shift the wires in the prime chamber planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fXShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    case 7:
    case 8:
    case 9:
      fXShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    }
    TLine Line;
    if (fWire < 12){  //96th wire ends at corner (X2, Y1)
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - fXShift);
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 18){  //184th wire begins at corner (X1, Y2)
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-11.6)) - fXShift);
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5) + fXShift);
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-17.4)));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-11.6)) - fXShift); // ISSUES HERE!  FIXXX!!
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    Line.SetLineColor(kBlue);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
  }
  fRegion2XY->GetCanvas()->SetEditable(kFALSE);
  fRegion2XY->GetCanvas()->Update();

  //TOP VIEW (Y-Z)
  fRegion2YZ->GetCanvas()->cd();
  //Region 2 X wires (32 total)
  for(QwHitContainer::iterator fHit = Hits_R2x->begin(); fHit != Hits_R2x->end(); fHit++){
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (1.5*R2_DIST*R2_CM) - (2*R2_DEPTH*R2_CM) + (R2_DIST*R2_CM*(fPlane)));
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) - (2*R2_DEPTH*R2_CM) + (R2_DIST*R2_CM*(fPlane)));
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line); // add Line to end of vector R2x
    Line_R2x.back().Draw(); // draw line;
  }
 //Region 2 U wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2u->begin(); fHit != Hits_R2u->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fXShift; // used to shift the wires in the prime chamber planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fXShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    case 7:
    case 8:
    case 9:
      fXShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    }
    TLine Line;
    if (fWire < 12){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - fXShift);
      Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    }
    else if (fWire < 18){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - ((R2_UVDIST*R2_CM*(fWire-11.6)) - fXShift) - fXShift);
      Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5) + fXShift);
      Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-11.6) - fXShift) - fXShift);
      Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
  }
  //Region 2 V wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2v->begin(); fHit != Hits_R2v->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = hit->GetDriftDistance();
    double fXShift; // used to shift the wires in the prime chamber planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fXShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    case 7:
    case 8:
    case 9:
      fXShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    }
    TLine Line;
    if (fWire < 12){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - fXShift);
      Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    }
    else if (fWire < 18){
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire) - fXShift);
      Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-11.6)) - fXShift);
      Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5) + fXShift);
      Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-11.6)) - fXShift);
      Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      }
    Line.SetLineColor(kBlue);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
      }
  fRegion2YZ->GetCanvas()->SetEditable(kFALSE);
  fRegion2YZ->GetCanvas()->Update();

  //SIDE VIEW (X-Z)
  fRegion2XZ->GetCanvas()->cd();
  //Region 2 X wires (32 total)
  for(QwHitContainer::iterator fHit = Hits_R2x->begin(); fHit != Hits_R2x->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fYShift = 0.0; // used to shift the wires on the prime planes (ROOT coordinates)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fYShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fYShift = .5*R2_XDIST*R2_CM; // half drift cell size
      break;
    case 7:
    case 8:
    case 9:
      fYShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fYShift = .5*R2_XDIST*R2_CM;
      break;
    }
    TLine Line;
    Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fYShift);
    Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
    Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (R2_XDIST*R2_CM*fWire) + fYShift);
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line);
    Line_R2x.back().Draw();
    }
  //Region 2 U wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2u->begin(); fHit != Hits_R2u->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fXShift; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fXShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    case 7:
    case 8:
    case 9:
      fXShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    }
    TLine Line;
    if (fWire < 12){
      Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (((R2_UVDIST*R2_CM*fWire) + fXShift)*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 18){
      Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_UVDIST*R2_CM*(fWire-17.4)));
      Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
      }
    Line.SetLineColor(kGreen);
    //    Line.SetLineWidth(3);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 2 V wires (29 total)
  for(QwHitContainer::iterator fHit = Hits_R2v->begin(); fHit != Hits_R2v->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fXShift; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
    case 3:
      fXShift = 0;
      break;
    case 4:
    case 5:
    case 6:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    case 7:
    case 8:
    case 9:
      fXShift = 0;
      break;
    case 10:
    case 11:
    case 12:
      //      fXShift = .5*R2_UVDIST*R2_CM;
      break;
    }
    TLine Line;
    if (fWire < 12){
      Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5) - (R2_UVDIST*R2_CM*fWire*tan(R2_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 18){
      Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5) - (R2_UVDIST*R2_CM*(fWire-17.4)));
      Line.SetX2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*fPlane));
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    Line.SetLineColor(kBlue);
    //    Line.SetLineWidth(3);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
    }
  fRegion2XZ->GetCanvas()->SetEditable(kFALSE);
  fRegion2XZ->GetCanvas()->Update();


  //DRAW REGION 3 WIRE HITS//

  //FRONT VIEW (X-Y)
  //Region 3 U wires (279 total)
  fRegion3XY->GetCanvas()->cd();
  for(QwHitContainer::iterator fHit = Hits_R3u->begin(); fHit != Hits_R3u->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane(); // chamber number
    //    double fDist = fHit->GetDriftDistance();
    double fYShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .75;
      break;
    case 3:
    case 4:
      fYShift = .65;
      break;
    }
    TLine Line;
    if (fWire < 96){  //96th wire ends at corner (X2, Y2)
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5) + (R3_UVDIST*R3_CM*fWire*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 184){  //184th wire begins at (X1, Y1)
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96)));
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5) + (R3_UVDIST*R3_CM*(fWire-184)));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96))); // ISSUES HERE!  FIXXX!!
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 3 V wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3v->begin(); fHit != Hits_R3v->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fYShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .75;
      break;
    case 3:
    case 4:
      fYShift = .65;
      break;
    }
    TLine Line;
    if (fWire < 96){  //96th wire ends at corner (X2, Y1)
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 184){  //184th wire begins at corner (X1, Y2)
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96)));
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-184)));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96))); // ISSUES HERE!  FIXXX!!
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kBlue);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
    }
  fRegion3XY->GetCanvas()->SetEditable(kFALSE);
  fRegion3XY->GetCanvas()->Update();

  //TOP VIEW (Y-Z)
  fRegion3YZ->GetCanvas()->cd(); // wires not centered so easy to see crossover
 //Region 3 U wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3u->begin(); fHit != Hits_R3u->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fYShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .755;
      break;
    case 3:
    case 4:
      fYShift = .655;
      break;
    }
    TLine Line;
    if (fWire < 96){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift);
    }
    else if (fWire < 184){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96)));
      Line.SetY2(fYShift);
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96))); // ISSUES HERE!  FIXXX!!
      Line.SetY2(fYShift);
    }
    Line.SetLineColor(kGreen);
    Line.SetLineWidth(3); // pixel depth of frame
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 3 V wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3v->begin(); fHit != Hits_R3v->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fYShift = 0.0; // used to shift the lines to match correct plane
    switch (fPlane){
    case 1:
    case 2:
      fYShift = .745;
      break;
    case 3:
    case 4:
      fYShift = .645;
      break;
    }
    TLine Line;
    if (fWire < 96){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(fYShift);
    }
    else if (fWire < 184){
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96)));
      Line.SetY2(fYShift);
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(fYShift);
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-96))); // ISSUES HERE!  FIXXX!!
      Line.SetY2(fYShift);
    }
    Line.SetLineColor(kBlue);
    Line.SetLineWidth(3); // pixel depth of frame
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  fRegion3YZ->GetCanvas()->SetEditable(kFALSE);
  fRegion3YZ->GetCanvas()->Update();

  //SIDE VIEW (X-Z)
  fRegion3XZ->GetCanvas()->cd();
  //Region 3 U wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3u->begin(); fHit != Hits_R3u->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fXShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    double fYShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
      fXShift = .5 - (R3_DIST*R3_CM*.5) - .005; // wires not centered so easy to see crossover
      fYShift = .75;
      break;
    case 3:
    case 4:
      fXShift = .5 + (R3_DIST*R3_CM*.5) - .005;
      fYShift = .65;
      break;
    }
    TLine Line;
    if (fWire < 96){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5) + (R3_UVDIST*R3_CM*fWire*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 184){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(fXShift);
      Line.SetY1(fYShift - (R3_LENGTH*R3_CM*.5) + (R3_UVDIST*R3_CM*(fWire-184)));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line.SetLineWidth(3); // pixel depth of frame
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 3 V wires (279 total)
  for(QwHitContainer::iterator fHit = Hits_R3v->begin(); fHit != Hits_R3v->end(); fHit++){
    int fWire = fHit->GetElement();
    int fPlane = fHit->GetPlane();
    //    double fDist = fHit->GetDriftDistance();
    double fXShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    double fYShift = 0.0; // used to shift the lines to match correct plane (ROOT coord)
    switch (fPlane){
    case 1:
    case 2:
      fXShift = .5 - (R3_DIST*R3_CM*.5) + .005; // wires not centered so easy to see crossover
      fYShift = .75;
      break;
    case 3:
    case 4:
      fXShift = .5 + (R3_DIST*R3_CM*.5) + .005;
      fYShift = .65;
      break;
    }
    TLine Line;
    if (fWire < 96){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift + (R3_LENGTH*R3_CM*.5) - (R3_UVDIST*R3_CM*fWire*tan(R3_ANGLE*TMath::DegToRad())));
    }
    else if (fWire < 184){
      Line.SetX1(fXShift);
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(fXShift);
      Line.SetY1(fYShift + (R3_LENGTH*R3_CM*.5) - (R3_UVDIST*R3_CM*(fWire-184)));
      Line.SetX2(fXShift);
      Line.SetY2(fYShift - (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kBlue);
    Line.SetLineWidth(3); // pixel depth of frame
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  fRegion3XZ->GetCanvas()->SetEditable(kFALSE);
  fRegion3XZ->GetCanvas()->Update();
} // end DrawEvent()


QwEventDisplay::~QwEventDisplay(){  // cleans up memory used by class; default destructor

  fMain->Cleanup();
  delete fMain;

} // end ~QwEventDisplay()


/************************************\
** Qweak 2D Event Display           **
** Jefferson Lab -- Hall C          **
** Author: Derek Jones              **
** The George Washington University **
** Contact: dwjones8@gwu.edu        **
\************************************/
