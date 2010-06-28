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


////HEADERS////

//Qweak Event Display Header
#include "QwEventDisplayDJ.h"  // includes ROOT headers and common classes and variables for the event display
ClassImp(QwEventDisplayDJ);  // imports the new class from the header to this code

//Qweak Header Files
#include "QwTreeEventBuffer.h"  // create hit list from G4 Monte Carlo
#include "QwSubsystemArrayTracking.h"  // update hit list for each subsystem
#include "QwDetectorInfo.h"  // establishes detector information
#include "QwHitContainer.h"  // creates augmented vector of QwHits for special functionality


////GRAPHICAL COMPONENTS////

QwEventDisplayDJ::QwEventDisplayDJ(const TGWindow *p,UInt_t w,UInt_t h){  // creates main graphical components

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

   // insert Qweak logo
   TGIcon *QweakLogo = new TGIcon(fLogos,"/home/dwjones/Desktop/Logos/qweak.jpg");
   fLogos->AddFrame(QweakLogo, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   QweakLogo->MoveResize(0,0,155,75);

   // insert JLab logo
   TGIcon *JLabLogo = new TGIcon(fLogos,"/home/dwjones/Desktop/Logos/JLab_logo_white1.jpg");
   fLogos->AddFrame(JLabLogo, new TGLayoutHints(kLHintsNormal));
   JLabLogo->MoveResize(144,0,250,75);

   // graphics context changes for "Qweak 2D Single Event Display" title label
   ufont = gClient->GetFont("-*-utopia-bold-r-*-*-25-*-*-*-*-*-*-*"); // set ufont to bold utopia 25
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
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // set ufont to bold charter 15
   GCValues_t valunterLabel;
   valunterLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valunterLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valunterLabel.fBackground);
   valunterLabel.fFillStyle = kFillSolid;
   valunterLabel.fFont = ufont->GetFontHandle();
   valunterLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valunterLabel, kTRUE);
   gClient->GetColorByName("#deba87",ucolor); // set ucolor to mute orange

   // create label for the event counter
   TGLabel *EventCounterLabel = new TGLabel(fEventBox1,"Event Counter",uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   EventCounterLabel->SetTextJustify(36);
   EventCounterLabel->SetMargins(0,0,0,0);
   EventCounterLabel->SetWrapLength(-1);
   fEventBox1->AddFrame(EventCounterLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   EventCounterLabel->MoveResize(50,8,112,18);

   // create vertical frame for event counter information: current event number, goto event entry, and goto event button
   fEventCounter = new TGVerticalFrame(fEventBox1,144,176,kVerticalFrame | kSunkenFrame,ucolor);
   fEventCounter->SetLayoutBroken(kTRUE);

   // graphics context changes for "Current Event" group frame
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-10-*-*-*-*-*-*-*"); // set ufont to bold charter 10
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

   // add current event label to "Current Event" group frame and customize
   fCurrentEvent->AddFrame(CurrentEventLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   CurrentEventLabel->MoveResize(11,18,104,32);
   fCurrentEvent->SetLayoutManager(new TGVerticalLayout(fCurrentEvent));
   fCurrentEvent->Resize(126,64);

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
   GotoEventButton->Connect("Clicked()", "QwEventDisplayDJ", this, "GotoEvent()");
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
   fEventBox1->AddFrame(fEventCounter, new TGLayoutHints(kLHintsNormal));
   fEventCounter->MoveResize(35,32,144,176);

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
   //   WireHitListBox->Connect("PositionChanged()", "QwEventDisplayDJ", this, "TimingListBox->fVScrollbar");  LINK SCROLLBARS TO BE IN SYNC
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
   gClient->GetColorByName("#deba87",ucolor); // set ucolor to mute orange

   // create label for wire hits list box
   TGLabel *WireHitsLabel = new TGLabel(fEventBox2,"Wire Hits",uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   WireHitsLabel->SetTextJustify(36);
   WireHitsLabel->SetMargins(0,0,0,0);
   WireHitsLabel->SetWrapLength(-1);
   fEventBox2->AddFrame(WireHitsLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   WireHitsLabel->MoveResize(59,8,96,18);

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
   gClient->GetColorByName("#deba87",ucolor); // set ucolor to mute orange
   TGLabel *TimingLabel = new TGLabel(fEventBox3,"Timing",uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   TimingLabel->SetTextJustify(36);
   TimingLabel->SetMargins(0,0,0,0);
   TimingLabel->SetWrapLength(-1);
   fEventBox3->AddFrame(TimingLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   TimingLabel->MoveResize(58,8,88,18);

   // create list box to display timing information
   TimingListBox = new TGListBox(fEventBox3);
   TimingListBox->Resize(200,180);
   fEventBox3->AddFrame(TimingListBox, new TGLayoutHints(kLHintsNormal));
   TimingListBox->MoveResize(8,32,200,180);

   // add event box 3 to event boxes frame
   fEventBoxes->AddFrame(fEventBox3, new TGLayoutHints(kLHintsNormal));
   fEventBox3->MoveResize(451,6,220,226);


   //EVENT BOX 4--Other Information//  TRIGGER SCINTILLATOR?

   // create vertical frame for event box 4
   gClient->GetColorByName("#d4cf87",ucolor); // set ucolor to buff
   TGVerticalFrame *fEventBox4 = new TGVerticalFrame(fEventBoxes,220,226,kVerticalFrame | kRaisedFrame,ucolor);
   fEventBox4->SetLayoutBroken(kTRUE);
   ufont = gClient->GetFont("-*-charter-bold-r-*-*-15-*-*-*-*-*-*-*"); // set ufont to bold charter 15

   // graphics context changes for "Other Information" label
   GCValues_t valInfoLabel;
   valInfoLabel.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valInfoLabel.fForeground);
   gClient->GetColorByName("#c0c0c0",valInfoLabel.fBackground);
   valInfoLabel.fFillStyle = kFillSolid;
   valInfoLabel.fFont = ufont->GetFontHandle();
   valInfoLabel.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valInfoLabel, kTRUE);
   gClient->GetColorByName("#deba87",ucolor); // set ucolor to mute orange

   // create label for other information list box
   TGLabel *OtherInfoLabel = new TGLabel(fEventBox4,"Other Information",uGC->GetGC(),ufont->GetFontStruct(),ucolor);
   OtherInfoLabel->SetTextJustify(36);
   OtherInfoLabel->SetMargins(0,0,0,0);
   OtherInfoLabel->SetWrapLength(-1);
   fEventBox4->AddFrame(OtherInfoLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   OtherInfoLabel->MoveResize(32,8,160,18);

   // create list box to display other information
   OtherListBox = new TGListBox(fEventBox4);  // RENAME VARIABLE ONCE I KNOW WHAT IT DOES
   OtherListBox->Resize(200,180);
   fEventBox4->AddFrame(OtherListBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   OtherListBox->MoveResize(8,32,200,180);
   fEventBoxes->AddFrame(fEventBox4, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fEventBox4->MoveResize(675,6,220,226);


  ////REGION BOXES////

   // create region boxes tab frame
   gClient->GetColorByName("#0047ab",ucolor); // set ucolor to blue
   fRegions = new TGTab(fMain,900,312,TGTab::GetDefaultGC()(),TGTab::GetDefaultFontStruct(),kChildFrame,ucolor);

   //REGION 1--Gas Electron Multiplier//

   // create tab and container of "Region 1" data
   fRegion1 = fRegions->AddTab("Region 1--GEM");
   fRegion1->SetLayoutManager(new TGMatrixLayout(fRegion1,0,1,0,0));
   fRegion1->SetLayoutBroken(kTRUE);

   // create embedded canvas for Region 1 XY view
   fRegion1XY = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1XY = fRegion1XY->GetCanvasWindowId();
   cR1XY= new TCanvas("cR1XY", 10, 10, wRegion1XY);
   fRegion1XY->AdoptCanvas(cR1XY);
   fRegion1->AddFrame(fRegion1XY, new TGLayoutHints(kLHintsNormal));
   fRegion1XY->MoveResize(12,4,280,280);
   Label_R1XY = new TPaveLabel(.03,.92,.97,.97,"Front View (X-Y Projection)"); // create canvas label
   Label_R1XY->Draw();
   Box_R1XY = new TBox(.5-(R1_WIDTH*R1_CM*.5), .5-(R1_LENGTH*R1_CM*.5), .5+(R1_WIDTH*R1_CM*.5), .5+(R1_LENGTH*R1_CM*.5)); // create/center box to frame wire lines
   Box_R1XY->SetLineColor(1);
   Box_R1XY->SetFillStyle(0);
   Box_R1XY->Draw();

   // create embedded canvas for Region 1 XZ view
   fRegion1XZ = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1XZ = fRegion1XZ->GetCanvasWindowId();
   cR1XZ = new TCanvas("cR1XZ", 10, 10, wRegion1XZ);
   fRegion1XZ->AdoptCanvas(cR1XZ);
   fRegion1->AddFrame(fRegion1XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion1XZ->MoveResize(306,4,280,280);
   Label_R1XZ = new TPaveLabel(.03,.92,.97,.97,"Top View (X-Z Projection)"); // create canvas label
   Label_R1XZ->Draw();
   Box_R1XZ = new TBox(.5-(R1_WIDTH*R1_CM*.5), .5-(R1_DEPTH*R1_CM*.5), .5+(R1_WIDTH*R1_CM*.5), .5+(R1_DEPTH*R1_CM*.5)); // create/center box to frame wire lines
   Box_R1XZ->SetLineColor(1);
   Box_R1XZ->SetFillStyle(0);
   Box_R1XZ->Draw();

   // create embedded canvas for Region 1 YZ view
   fRegion1YZ = new TRootEmbeddedCanvas(0,fRegion1,280,280);
   Int_t wRegion1YZ = fRegion1YZ->GetCanvasWindowId();
   cR1YZ  = new TCanvas("cR1YZ", 10, 10, wRegion1YZ);
   fRegion1YZ->AdoptCanvas(cR1YZ);
   fRegion1->AddFrame(fRegion1YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion1YZ->MoveResize(603,4,280,280);
   Label_R1YZ = new TPaveLabel(.03,.92,.97,.97,"Side View (Y-Z Projection)"); // create canvas label
   Label_R1YZ->Draw();
   Box_R1YZ = new TBox(.5-(R1_DEPTH*R1_CM*.5), .5-(R1_LENGTH*R1_CM*.5), .5+(R1_DEPTH*R1_CM*.5), .5+(R1_LENGTH*R1_CM*.5)); // create/center box to frame wire lines
   Box_R1YZ->SetLineColor(1);
   Box_R1YZ->SetFillStyle(0);
   Box_R1YZ->Draw();

   //REGION 2--Horizontal Drift Chamber//

   // create tab and container of "Region 2" data
   fRegion2 = fRegions->AddTab("Region 2--HDC");
   fRegion2->SetLayoutManager(new TGVerticalLayout(fRegion2));
   fRegion2->SetLayoutBroken(kTRUE);

   // create embedded canvas for Region 2 XY view
   fRegion2XY = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2XY = fRegion2XY->GetCanvasWindowId();
   cR2XY = new TCanvas("cR2XY", 10, 10, wRegion2XY);
   fRegion2XY->AdoptCanvas(cR2XY);
   fRegion2->AddFrame(fRegion2XY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2XY->MoveResize(12,4,280,280);
   Label_R2XY = new TPaveLabel(.03,.92,.97,.97,"Front View (X-Y Projection)"); // create canvas label
   Label_R2XY->Draw();
   Box_R2XY = new TBox(.5-(R2_WIDTH*R2_CM*.5), .5-(R2_LENGTH*R2_CM*.5), .5+(R2_WIDTH*R2_CM*.5), .5+(R2_LENGTH*R2_CM*.5)); // create/center box to frame wire lines
   Box_R2XY->SetLineColor(1);
   Box_R2XY->SetFillStyle(0);
   Box_R2XY->Draw();

   // create embedded canvas for Region 2 XZ view
   fRegion2XZ = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2XZ = fRegion2XZ->GetCanvasWindowId();
   cR2XZ= new TCanvas("cR2XZ", 10, 10, wRegion2XZ);
   fRegion2XZ->AdoptCanvas(cR2XZ);
   fRegion2->AddFrame(fRegion2XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2XZ->MoveResize(306,4,280,280);
   Label_R2XZ = new TPaveLabel(.03,.92,.97,.97,"Top View (X-Z Projection)"); // create canvas label
   Label_R2XZ->Draw();
   Box_R2XZ = new TBox(.5-(R2_WIDTH*R2_CM*.5), .5-(R2_DEPTH*R2_CM*.5), .5+(R2_WIDTH*R2_CM*.5), .5+(R2_DEPTH*R2_CM*.5)); // create/center box to frame wire lines
   Box_R2XZ->SetLineColor(1);
   Box_R2XZ->SetFillStyle(0);
   Box_R2XZ->Draw();

   // create embedded canvas for Region 2 YZ view
   fRegion2YZ = new TRootEmbeddedCanvas(0,fRegion2,280,280);
   Int_t wRegion2YZ = fRegion2YZ->GetCanvasWindowId();
   cR2YZ= new TCanvas("cR2YZ",10, 10, wRegion2YZ);
   fRegion2YZ->AdoptCanvas(cR2YZ);
   fRegion2->AddFrame(fRegion2YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion2YZ->MoveResize(603,4,280,280);
   Label_R2YZ = new TPaveLabel(.03,.92,.97,.97,"Side View (Y-Z Projection)"); // create canvas label
   Label_R2YZ->Draw();
   Box_R2YZ = new TBox(.5-(R2_DEPTH*R2_CM*.5), .5-(R2_LENGTH*R2_CM*.5), .5+(R2_DEPTH*R2_CM*.5), .5+(R2_LENGTH*R2_CM*.5)); // create/center box to frame wire lines
   Box_R2YZ->SetLineColor(1);
   Box_R2YZ->SetFillStyle(0);
   Box_R2YZ->Draw();

   //REGION 3--Vertical Drift Chamber//

   // create tab and container of "Region 3" data
   fRegion3 = fRegions->AddTab("Region 3--VDC");
   fRegion3->SetLayoutManager(new TGVerticalLayout(fRegion3));
   fRegion3->SetLayoutBroken(kTRUE);

   // create embedded canvas for Region 3 XY view
   fRegion3XY = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3XY = fRegion3XY->GetCanvasWindowId();
   cR3XY = new TCanvas("cR3XY", 10, 10, wRegion3XY);
   fRegion3XY->AdoptCanvas(cR3XY);
   fRegion3->AddFrame(fRegion3XY, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3XY->MoveResize(12,4,280,280);
   Label_R3XY = new TPaveLabel(.03,.92,.97,.97,"Front View (X-Y Projection)"); // create canvas label
   Label_R3XY->Draw();
   Box_R3XY = new TBox(.5-(R3_WIDTH*R3_CM*.5), .5-(R3_LENGTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .5+(R3_LENGTH*R3_CM*.5)); // create/center box to frame wire lines
   Box_R3XY->SetLineColor(1);
   Box_R3XY->SetFillStyle(0);
   Box_R3XY->Draw();

   // create embedded canvas for Region 3 XZ view
   fRegion3XZ = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3XZ = fRegion3XZ->GetCanvasWindowId();
   cR3XZ = new TCanvas("cR3XZ", 10, 10, wRegion3XZ);
   fRegion3XZ->AdoptCanvas(cR3XZ);
   fRegion3->AddFrame(fRegion3XZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3XZ->MoveResize(306,4,280,280);
   Label_R3XZ = new TPaveLabel(.03,.92,.97,.97,"Top View (X-Z Projection)"); // create canvas label
   Label_R3XZ->Draw();
   Box_R3XZ = new TBox(.5-(R3_WIDTH*R3_CM*.5), .5-(R3_DEPTH*R3_CM*.5), .5+(R3_WIDTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5)); // create/center box to frame wire lines
   Box_R3XZ->SetLineColor(1);
   Box_R3XZ->SetFillStyle(0);
   Box_R3XZ->Draw();

   // create embedded canvas for Region 3 YZ view
   fRegion3YZ = new TRootEmbeddedCanvas(0,fRegion3,280,280);
   Int_t wRegion3YZ = fRegion3YZ->GetCanvasWindowId();
   cR3YZ = new TCanvas("cR3YZ", 10, 10, wRegion3YZ);
   fRegion3YZ->AdoptCanvas(cR3YZ);
   fRegion3->AddFrame(fRegion3YZ, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegion3YZ->MoveResize(603,4,280,280);
   Label_R3YZ = new TPaveLabel(.03,.92,.97,.97,"Side View (Y-Z Projection)"); // create canvas label
   Label_R3YZ->Draw();
   Box_R3YZ = new TBox(.5-(R3_DEPTH*R3_CM*.5), .5-(R3_LENGTH*R3_CM*.5), .5+(R3_DEPTH*R3_CM*.5), .5+(R3_LENGTH*R3_CM*.5)); // create/center box to frame wire lines
   Box_R3YZ->SetLineColor(1);
   Box_R3YZ->SetFillStyle(0);
   Box_R3YZ->Draw();


   //REGION 4--Trigger Scintillator//

   // all scintillator info will be in event box 4, but maybe declare variables here?  perhaps just put info in draw portion

   // set tab attributes
   fRegions->SetTab(0);
   fRegions->Resize(fRegions->GetDefaultSize());
   fMain->AddFrame(fRegions, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRegions->MoveResize(0,320,900,312);


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
   ClearButton->Connect("Clicked()", "QwEventDisplayDJ", this, "GoClear()"); // connect button to GoClear() function
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
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*"); // set ufont to bold helvetica 12


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
   PreviousButton->Connect("Clicked()", "QwEventDisplayDJ", this, "GoPrevious()"); // connect button to GoPrevious() function
   PreviousButton->SetTextJustify(36);
   PreviousButton->SetMargins(0,0,0,0);
   PreviousButton->SetWrapLength(-1);
   PreviousButton->Resize(100,50);
   PreviousButton->ChangeBackground(ucolor);
   // add previous button to frame
   fButtons->AddFrame(PreviousButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   PreviousButton->MoveResize(12,6,100,50);
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*"); // set ufont to bold helvetica 12


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
   NextButton->Connect("Clicked()", "QwEventDisplayDJ", this, "GoNext()"); // connect button to GoNext() function
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


   //FINAL TOUCH//

   // add main frame to the pop up window and customize
   fMain->SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
   fMain->MapSubwindows();
   fMain->Resize(fMain->GetDefaultSize());
   fMain->MapWindow();
   fMain->Resize(900,692);
} // end QwEventDisplayDJ()


////FUNCTIONAL COMPONENTS////

void QwEventDisplayDJ::GoPrevious(){  // makes event counter minus one
                                      // called by PreviousButton click

  if (fEventNumber > 1){
    fEventNumber--;  // go to previous event
    DrawEvent();  // update display information
  }

  else
    GoClear(); // event 0 is null event

} // end GoPrevious()


void QwEventDisplayDJ::GoNext(){  // makes event counter plus one
                                  // called by NextButton click

  fEventNumber++;  // go to next event
  DrawEvent();  // update display information
} // end GoNext()


void QwEventDisplayDJ::GoClear(){  // clears all displayed data
                                   // called by ClearButton click
                                   // NEED TO TEST REGION CLEAR

  fEventNumber = 0;  // go to null event
  CurrentEventLabel->SetText(Form("%d", fEventNumber)); // show null event

  WireHitListBox->RemoveAll(); // clear event box 2
  TimingListBox->RemoveAll(); // clear event box 3
  OtherListBox->RemoveAll(); // clear event box 4

  Line_R1r.clear(); // clear existing Region 1 drawn lines
  Line_R1y.clear();
  Line_R2x.clear(); // clear existing Region 2 drawn lines
  Line_R2u.clear();
  Line_R2v.clear();
  Line_R3u.clear(); // clear existing Region 3 drawn lines
  Line_R3v.clear();

} // end GoClear()


void QwEventDisplayDJ::GotoEvent(){  // goes to desired event number written in "goto event" text entry box
                                     // called by GotoEventButton click

  fEventNumber = CurrentEventEntry->GetNumberEntry()->GetIntNumber(); // fEventNumber takes value from number entry

  if (fEventNumber > 0){
    CurrentEventLabel->SetText(Form("%d",CurrentEventEntry->GetNumberEntry()->GetIntNumber())); // set current event label
    DrawEvent();  // update display information
  }

  else
    GoClear(); // event 0 is null event

} // end GotoEvent()


void QwEventDisplayDJ::DrawEvent(){  // draws event data into display: lists wire hits, timing, and other information in event boxes
                                     // displays orthographic views triggered wires for each region
                                     // called by Previous, Next, or GotoEvent buttons  // NOT COMPLETE

  CurrentEventLabel->SetText(Form("%d", fEventNumber)); // updates layout to show current event number

  WireHitListBox->RemoveAll(); // clear event box 2
  TimingListBox->RemoveAll(); // clear event box 3
  OtherListBox->RemoveAll(); // clear event box 4

  Line_R1r.clear(); // clear existing Region 1 drawn lines
  Line_R1y.clear();
  R1_XYfit.clear();
  R1_XZfit.clear();
  Line_R2x.clear(); // clear existing Region 2 drawn lines
  Line_R2u.clear();
  Line_R2v.clear();
  Line_R3u.clear(); // clear existing Region 3 drawn lines
  Line_R3v.clear();

  //LIST WIRE HITS//
  // extract wire hit information
  fEventBuffer->GetEntry(fEventNumber);  // place the current event number into the event buffer
  fHitList = fEventBuffer->GetHitList();  // add the hit list from the current event buffer
  fHitList->Print();  // outputs the new wire hit information from the hit list

  // create a char string buffer to hold hit information
  char HitBuffer[30];

  // list wire hits in wire list hit box
  for(QwHitContainer::iterator hit = fHitList->begin(); hit != fHitList->end(); hit++){ // loop while hits exist in the hit container
    if(hit->GetElement() > 0){
      sprintf(HitBuffer, "Region %i--Wire %i, Plane %i", hit->GetRegion(), hit->GetElement(), hit->GetPlane()); // print single hit with region
      WireHitListBox->AddEntry(HitBuffer, fEventNumber); // add to list box
      WireHitListBox->MapSubwindows(); // re-map window
      WireHitListBox->Layout();
    } // end if
  } //end for

  //  WireHitListBox->AddEntry(Form(""),fEventNumber); NOT NEEDED SINCE EVENT BOXES CLEARED EACH DRAW

  //LIST TIMING DATA//
  for(QwHitContainer::iterator hit = fHitList->begin(); hit != fHitList->end(); hit++){ // loop while hits exist in the hit container
    if(hit->GetElement() > 0){
      sprintf(HitBuffer, "Time: %lf, Raw: %u", hit->GetTime(), hit->GetRawTime()); // print sin
      TimingListBox->AddEntry(HitBuffer, fEventNumber); // add to list box
      TimingListBox->MapSubwindows(); // re-map window
      TimingListBox->Layout();
    } // end if
  } //end for

  //  TimingListBox->AddEntry(Form(""),fEventNumber);

  //LIST OTHER DATA//
  for(QwHitContainer::iterator hit = fHitList->begin(); hit != fHitList->end(); hit++){ // loop while hits exist in the hit container
    if(hit->GetElement() > 0){
      sprintf(HitBuffer, "Drift Dist: %lf", hit->GetDriftDistance()); // print single drift distance with wire hit
      OtherListBox->AddEntry(HitBuffer, fEventNumber); // add to list box
      OtherListBox->MapSubwindows(); // re-map window
      OtherListBox->Layout();
    } // end if
  } //end for

  //  OtherListBox->AddEntry(Form(""),fEventNumber);

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
  //Region 1 R wires
  for(QwHitContainer::iterator hit = Hits_R1r->begin(); hit != Hits_R1r->end(); hit++){
    //WHAT ARE THESE ACTUALLY? G4 DOES NOT GET ANY HITS FROM THESE
  }
  //Region 1 Y wire
  sum = 0; // back to 0 for fitting
  num = 0;
  for(QwHitContainer::iterator hit = Hits_R1y->begin(); hit != Hits_R1y->end(); hit++){
    int wire = hit->GetElement();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*wire));
    Line.SetY1(.5 - (R1_LENGTH*R1_CM*.5));
    Line.SetX2(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*wire));
    Line.SetY2(.5 + (R1_LENGTH*R1_CM*.5));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line); // add Line to end of vector R2x
    Line_R1y.back().Draw(); // draw line;
    sum = sum + wire;
    num++;
  }
  fit = sum/num;
  Line.SetX1(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetY1(.5 - (R1_LENGTH*R1_CM*.5));
  Line.SetX2(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetY2(.5 + (R1_LENGTH*R1_CM*.5));
  Line.SetLineColor(kBlack);
  R1_XYfit.push_back(Line);
  R1_XYfit.back().Draw();
  fRegion1XY->GetCanvas()->Update();

  //TOP VIEW (X-Z)
  fRegion1XZ->GetCanvas()->cd();
  //Region 1 Y wire
  sum = 0; // back to 0 for fitting
  num = 0;
  for(QwHitContainer::iterator hit = Hits_R1y->begin(); hit != Hits_R1y->end(); hit++){
    int wire = hit->GetElement();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*wire));
    Line.SetY1(.5 - (R1_DEPTH*R1_CM*.5));
    Line.SetX2(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*wire));
    Line.SetY2(.5 + (R1_DEPTH*R1_CM*.5));
    Line.SetLineColor(kViolet);
    Line_R1y.push_back(Line); // add Line to end of vector R2x
    Line_R1y.back().Draw(); // draw line;
    sum = sum + wire;
    num++;
  }
  fit = sum/num;
  Line.SetX1(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetY1(.5 - (R1_LENGTH*R1_CM*.5));
  Line.SetX2(.5 - (R1_WIDTH*R1_CM*.5) + (R1_DIST*R1_CM*fit));
  Line.SetY2(.5 + (R1_LENGTH*R1_CM*.5));
  Line.SetLineColor(kBlack);
  R1_XZfit.push_back(Line);
  R1_XZfit.back().Draw();;
  fRegion1XZ->GetCanvas()->Update();

  //SIDE VIEW (Y-Z)
  fRegion1YZ->GetCanvas()->cd();
  //Region 1 Y wire
  for(QwHitContainer::iterator hit = Hits_R1y->begin(); hit != Hits_R1y->end(); hit++){
   Box_R1YZ->SetFillStyle(1001);
   Box_R1YZ->SetFillColor(kViolet);
   Box_R1YZ->Draw();
  }
  fRegion1YZ->GetCanvas()->Update();


  //DRAW REGION 2 WIRE HITS//

  //FRONT VIEW (X-Y)
  fRegion2XY->GetCanvas()->cd();
  //Region 2 X wires
  for(QwHitContainer::iterator hit = Hits_R2x->begin(); hit != Hits_R2x->end(); hit++){
    int wire = hit->GetElement();
    int plane = hit->GetPlane();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));// + dist*.0135); // FIX R2_DIST
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));// + dist*.0135); // removed drift distance addition
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line); // add Line to end of vector R2x
    Line_R2x.back().Draw(); // draw line;
  }
  //Region 2 U wires
  for(QwHitContainer::iterator hit = Hits_R2u->begin(); hit != Hits_R2u->end(); hit++){
    int wire = hit->GetElement();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    if(wire < 17){
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5) - (R2_DIST*R2_CM*wire));// - dist*1.25*2.099737533e-3); // CHECK
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));// + dist*1.667*2.099737533e-3); // CHECK
    }
    else{
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
      Line.SetY1(.5 - (R2_LENGTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));// + dist*1.667*2.099737533e-3); // CHECK
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) - (R2_DIST*R2_CM*wire));// - dist*1.25*2.099737533e-3); // CHECK
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5));
      }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 2 V wires
  for(QwHitContainer::iterator hit = Hits_R2v->begin(); hit != Hits_R2v->end(); hit++){
    int wire = hit->GetElement();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    if (wire < 17) {
      Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));    //.26523+(wire-1)*0.040853); // CHECK
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));    //.323957+(wire-1)*0.0306477); // CHECK
      Line.SetY2(.5 - (R2_LENGTH*R2_CM*.5));
    }
    else{
      Line.SetX1(.5 + (R2_WIDTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));   //0.214179+(wire-17)*0.0306667); // CHECK
      Line.SetY1(.5 + (R2_LENGTH*R2_CM*.5));
      Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
      Line.SetY2(.5 + (R2_LENGTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));    //.1191+(wire-17)*0.0408787); // CHECK
    }
    Line.SetLineColor(kBlue);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
  }
  fRegion2XY->GetCanvas()->Update();

  //TOP VIEW (X-Z)
  fRegion2XZ->GetCanvas()->cd();
  //Region 2 X wires
  for(QwHitContainer::iterator hit = Hits_R2x->begin(); hit != Hits_R2x->end(); hit++){
    int plane = hit->GetPlane();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*plane));// + dist*.0135); // plane 1
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*plane));// + dist*.0135); // removed drift distance addition
    Line.SetLineColor(kRed);
    Line_R2x.push_back(Line); // add Line to end of vector R2x
    Line_R2x.back().Draw(); // draw line;
  }
 //Region 2 U wires FIXXXX
  for(QwHitContainer::iterator hit = Hits_R2u->begin(); hit != Hits_R2u->end(); hit++){
    int plane = hit->GetPlane();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*plane));// + dist*.0135); // plane 1
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5));
    Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*plane));// + dist*.0135); // removed drift distance addition
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 2 V wires FIXXXX
  for(QwHitContainer::iterator hit = Hits_R2v->begin(); hit != Hits_R2v->end(); hit++){
    int wire = hit->GetElement();
    int plane = hit->GetPlane();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R2_WIDTH*R2_CM*.5));
    Line.SetY1(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*plane));// + dist*.0135); // plane 1
    Line.SetX2(.5 + (R2_WIDTH*R2_CM*.5) + (R2_DIST*R2_CM*wire));
    Line.SetY2(.5 - (R2_DEPTH*R2_CM*.5) + (R2_DIST*R2_CM*plane));// + dist*.0135); // removed drift distance addition
    Line.SetLineColor(kBlue);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  fRegion2XZ->GetCanvas()->Update();

  //SIDE VIEW (Y-Z)
  fRegion2YZ->GetCanvas()->cd();
  fRegion2YZ->GetCanvas()->Update();


  //DRAW REGION 3 WIRE HITS//

  //FRONT VIEW (X-Y)
  //Region 3 U wires
  fRegion3XY->GetCanvas()->cd();
  for(QwHitContainer::iterator hit = Hits_R3u->begin(); hit != Hits_R3u->end(); hit++){
    int wire = hit->GetElement();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    if(wire > 17){
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5) - (R3_DIST*R3_CM*wire));// - dist*1.25*2.099737533e-3); // CHECK
      Line.SetY1(.5 - (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(.5 + (R3_LENGTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));// + dist*1.667*2.099737533e-3); // CHECK
    }
    else{
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(.5 - (R3_LENGTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));// + dist*1.667*2.099737533e-3); // CHECK
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) - (R3_DIST*R3_CM*wire));// - dist*1.25*2.099737533e-3); // CHECK
      Line.SetY2(.5 + (R3_LENGTH*R3_CM*.5));
    }
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 3 V wires
  for(QwHitContainer::iterator hit = Hits_R3v->begin(); hit != Hits_R3v->end(); hit++){
    int wire = hit->GetElement();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    if (wire > 17) {
      Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
      Line.SetY1(.5 + (R3_LENGTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));    //.26523+(wire-1)*0.040853); // CHECK
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));    //.323957+(wire-1)*0.0306477); // CHECK
      Line.SetY2(.5 - (R3_LENGTH*R3_CM*.5));
    }
    else{
      Line.SetX1(.5 + (R3_WIDTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));   //0.214179+(wire-17)*0.0306667); // CHECK
      Line.SetY1(.5 + (R3_LENGTH*R3_CM*.5));
      Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
      Line.SetY2(.5 + (R3_LENGTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));    //.1191+(wire-17)*0.0408787); // CHECK
    }
    Line.SetLineColor(kBlue);
    Line_R2v.push_back(Line);
    Line_R2v.back().Draw();
  }
  fRegion3XY->GetCanvas()->Update();

  //TOP VIEW (X-Z)
  fRegion3XZ->GetCanvas()->cd();
 //Region 3 U wires FIXXXX
  for(QwHitContainer::iterator hit = Hits_R3u->begin(); hit != Hits_R3u->end(); hit++){
    int plane = hit->GetPlane();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
    Line.SetY1(.5 - (R3_DEPTH*R3_CM*.5) + (R3_DIST*R3_CM*plane));// + dist*.0135); // plane 1
    Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5));
    Line.SetY2(.5 - (R3_DEPTH*R3_CM*.5) + (R3_DIST*R3_CM*plane));// + dist*.0135); // removed drift distance addition
    Line.SetLineColor(kGreen);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  //Region 3 V wires FIXXXX
  for(QwHitContainer::iterator hit = Hits_R3v->begin(); hit != Hits_R3v->end(); hit++){
    int wire = hit->GetElement();
    int plane = hit->GetPlane();
    //    double dist = hit->GetDriftDistance();
    TLine Line;
    Line.SetX1(.5 - (R3_WIDTH*R3_CM*.5));
    Line.SetY1(.5 - (R3_DEPTH*R3_CM*.5) + (R3_DIST*R3_CM*plane));// + dist*.0135); // plane 1
    Line.SetX2(.5 + (R3_WIDTH*R3_CM*.5) + (R3_DIST*R3_CM*wire));
    Line.SetY2(.5 - (R3_DEPTH*R3_CM*.5) + (R3_DIST*R3_CM*plane));// + dist*.0135); // removed drift distance addition
    Line.SetLineColor(kBlue);
    Line_R2u.push_back(Line);
    Line_R2u.back().Draw();
    }
  fRegion3XZ->GetCanvas()->Update();

  //SIDE VIEW (Y-Z)
  fRegion3YZ->GetCanvas()->cd();
  fRegion3YZ->GetCanvas()->Update();
} // end DrawEvent()


QwEventDisplayDJ::~QwEventDisplayDJ(){  // cleans up memory used by class; default destructor

  fMain->Cleanup();
  delete fMain;

} // end ~QwEventDisplayDJ()


/************************************\
** Qweak 2D Event Display           **
** Jefferson Lab -- Hall C          **
** Author: Derek Jones              **
** The George Washington University **
** Contact: dwjones8@gwu.edu        **
\************************************/
