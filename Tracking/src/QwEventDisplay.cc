// Implementation

#include "QwEventDisplay.h"


const char *R2planes[] = {"x","u","v","xp","up","vp"};
// change this to "R2planes" and follow suit throughout code

const char *R3planes[] = {"u","v","u_prime","v_prime"};



QwEventDisplay::QwEventDisplay(const TGWindow *p,UInt_t w,UInt_t h) {
Int_t j,k,s;

// wdc: gROOT not declared
//gROOT->SetStyle("Plain");

// listboxes[0] =  &xPlaneListBox;
// listboxes[1] =  &uPlaneListBox;
// listboxes[2] =  &vPlaneListBox;
// listboxes[3] =  &xpPlaneListBox;
// listboxes[4] =  &upPlaneListBox;
// listboxes[5] =  &vpPlaneListBox;

  listboxes[0] =  &Region1Box;
  listboxes[1] =  &Region2Box;
  listboxes[2] =  &Region3Box;
  listboxes[3] =  &EventBox;

/* // need to learn how many planes of wires there will be in the detectors for Region 1
  // Region 1 lines
  R1lu = new TLine();
  R1lv = new TLine();
  R1luPrime = new TLine();
  R1lvPrime = new TLine();
*/

  // Region 2 lines
  lx = new TLine();
  lu = new TLine();
  lv = new TLine();
  lxp = new TLine();
  lup = new TLine();
  lvp = new TLine();

/* //  created to replace the no-too-user-friendly code that was just for Region 2 stuff
  // Region 2 lines
  R2lx = new TLine();
  R2lu = new TLine();
  R2lv = new TLine();
  R2lxPrime = new TLine();
  R2luPrime = new TLine();
  R2lvPrime = new TLine();
*/

  // Region 2 fit lines
  xzfit = new TLine();
  yzfit = new TLine();

/*  // Region 2 fit lines with "R2" in the variable names
  R2xzfit = new TLine();
  R2yzfit = new TLine();
*/

  // Region 2 boxes
  b1 = new TBox();
  b2 = new TBox();
  b3 = new TBox();

  // Region 2 canvas labels for particle paths
  label1 = new TPaveLabel(.05,.92,.97,.97,"Region 2 xy Projection");
  label2 = new TPaveLabel(.05,.92,.97,.97,"Region 2 xz Projection");
  label3 = new TPaveLabel(.05,.92,.97,.97,"Region 2 yz Projection");

  // Region 3 lines
  TLine* R3lu = new TLine();
  TLine* R3lv = new TLine();
  TLine* R3luPrime = new TLine();
  TLine* R3lvPrime = new TLine();

  // Region 3 fit lines
  R3xzfit = new TLine();
  R3yzfit = new TLine();

  // Region 3 boxes
  R3b1 = new TBox();
  R3b2 = new TBox();
  R3b3 = new TBox();

  // Region 3 canvas labels for particle paths
  R3label1 = new TPaveLabel(.05,.92,.97,.97,"Region 3 xy Projection");
  R3label2 = new TPaveLabel(.05,.92,.97,.97,"Region 3 xz Projection");
  R3label3 = new TPaveLabel(.05,.92,.97,.97,"Region 3 yz Projection");

  R2text = new TPaveText(.85,.05,.95,.9);
  points = new TPolyMarker(6);

  // Region 2 plane labels
  R2text->AddText("x");
  R2text->AddText("u");
  R2text->AddText("v");
  // Region 2 prime plane labels
  R2text->AddText("x'");
  R2text->AddText("u'");
  R2text->AddText("v'");

  R2text->SetTextSize(.0669);
  R2text->GetLine(0)->SetTextColor(kRed);
  R2text->GetLine(1)->SetTextColor(kGreen);
  R2text->GetLine(2)->SetTextColor(kBlue);
  R2text->GetLine(3)->SetTextColor(kRed+3);
  R2text->GetLine(4)->SetTextColor(kGreen+3);
  R2text->GetLine(5)->SetTextColor(kBlue+3);
  points->SetMarkerStyle(2);

  // create a main frames
  fMain = new TGMainFrame(p,w,h);

  // create composite frame just because
  //  TGCompositeFrame *fMainFrame838 = new TGCompositeFrame(fMain,1210,859,kVerticalFrame);
  //  fMainFrame838->SetLayoutBroken(kTRUE);


  // create a horizontal frames (hframe) to contain the listboxes
  TGHorizontalFrame *listboxhframe = new TGHorizontalFrame(fMain,2000,600);

/*
  // create a horizontal frame for each listbox
  TGHorizontalFrame *listboxhframe1 = new TGHorizontalFrame(fMain,600,600);
  TGHorizontalFrame *listboxhframe2 = new TGHorizontalFrame(fMain,600,600);
  TGHorizontalFrame *listboxhframe3 = new TGHorizontalFrame(fMain,600,600);
*/

  // create list boxes for each wire plane
  for(j=0;j<4;j++){
    (*listboxes[j]) = new TGListBox(listboxhframe,90);
  } // end for

  entry = -1;
  for(j=0;j<3;j++){
    (*listboxes[j])->Resize(200,200);
    (*listboxes[3])->Resize(120,40);
  } // end for


  // create labels for each wire plane for each region described by the listboxes
    //create composite frame for labels
    TGLabel *fRegionLabels = new TGLabel(fMain,"EVENT NUMBER                  REGION 1 HITS              REGION 2 HITS (Hit Order: x,u,v,x',u',v')                REGION 3 HITS");
    fRegionLabels->SetMargins(20,0,60,0);
    fMain->AddFrame(fRegionLabels, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  // end labels' creation

/* // create labels for each wire plane for each region described by the listboxes
   // create composite frame for labels
   TGCompositeFrame *fLabelFrame = new TGCompositeFrame(listboxhframe1,112,40,kVerticalFrame);
   fLabelFrame->SetLayoutBroken(kTRUE);

   // create label for Region 1
   TGLabel *fRegion1Label = new TGLabel(fLabelFrame,"Region 1 Hits");
   TGLabel *fRegion1Label = new TGLabel(fMain,"Region 1 Hits");
   fRegion1Label->SetTextJustify(36);
   fRegion2Label->SetMargins(0,0,0,0);
   fRegion1Label->SetX(Int_t x = 15);
   fRegion1Label->SetWrapLength(-1);
   fRegion1Label->MoveResize(30,50,112,80);
   fLabelFrame->AddFrame(fRegion1Label, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fMain->AddFrame(fRegion1Label, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // create label for Region 2
   TGLabel *fRegion2Label = new TGLabel(fLabelFrame,"Region 2 Hits");
   TGLabel *fRegion2Label = new TGLabel(fMain,"Region 2 Hits");
   fRegion2Label->SetTextJustify(36);
   fRegion2Label->SetMargins(0,0,0,0);
   fRegion2Label->SetWrapLength(-1);
   fRegion2Label->MoveResize(184,50,112,80);
   fLabelFrame->AddFrame(fRegion2Label, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fMain->AddFrame(fRegion2Label, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // create label for Region 3
   TGLabel *fRegion3Label = new TGLabel(fLabelFrame,"Region 3 Hits");
   TGLabel *fRegion3Label = new TGLabel(fMain,"Region 3 Hits");
   fRegion3Label->SetTextJustify(36);
   fRegion3Label->SetMargins(0,0,0,0);
   fRegion3Label->SetWrapLength(-1);
   fRegion3Label->MoveResize(336,50,112,80);
   fLabelFrame->AddFrame(fLabel_V, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fMain->AddFrame(fRegion3Label, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   fLabelFrame->MoveResize(30,0,112,80);
   fMain->AddFrame(fLabelFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
*/



  // put the listboxes in the hframe and put the hframe in a larger hframe
  // add listboxes to hframes that contain wire hit data
  listboxhframe->AddFrame(EventBox, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
  listboxhframe->AddFrame(Region1Box, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//listboxhframe1->AddFrame(Region1Box, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//listboxhframe1->MoveResize(0,45,370,600);
  listboxhframe->AddFrame(Region2Box, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//listboxhframe2->AddFrame(Region2Box, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//listboxhframe2->MoveResize(375,45,370,600);
  listboxhframe->AddFrame(Region3Box, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//listboxhframe3->AddFrame(Region3Box, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//listboxhframe3->MoveResize(745,45,370,600);


//  fMain->AddFrame(listboxhframe1, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//  fMain->AddFrame(listboxhframe2, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
//  fMain->AddFrame(listboxhframe3, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
  fMain->AddFrame(listboxhframe, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5)); // add the listbixhframe to the Main Frame
  listboxhframe->MoveResize(0,5,2000,600);
  // end listboxes' creation


/* // number entry box code copied straight from sev2.C
   TGNumberEntry *fNumberEntry962 = new TGNumberEntry(fHorizontalFrame845, (Double_t) 0,13,-1,(TGNumberFormat::EStyle) 5);
   fHorizontalFrame845->AddFrame(fNumberEntry962, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry962->MoveResize(8,80,112,22);

   fMainFrame844->AddFrame(fHorizontalFrame845, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,5,5));
*/
  // create tabs for each region
   // tab widget
   TGTab *fTab = new TGTab(fMain,232,400);

   // create container of "Region 1"
   TGCompositeFrame *fCompositeFrame1;
   fCompositeFrame1 = fTab->AddTab("Region 1");
   fCompositeFrame1->SetLayoutManager(new TGVerticalLayout(fCompositeFrame1));
   //fCompositeFame1->AddFrame(listboxhframe, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));

   // create container of "Region 2"
   TGCompositeFrame *fCompositeFrame2;
   fCompositeFrame2 = fTab->AddTab("Region 2");
   fCompositeFrame2->SetLayoutManager(new TGVerticalLayout(fCompositeFrame2));
   //fCompositeFame2->AddFrame(listboxhframe, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));

   // create container of "Region 3"
   TGCompositeFrame *fCompositeFrame3;
   fCompositeFrame3 = fTab->AddTab("Region 3");
   fCompositeFrame3->SetLayoutManager(new TGVerticalLayout(fCompositeFrame3));
   //fCompositeFame3->AddFrame(listboxhframe, new TGLayoutHints(kLHintsTop | kLHintsLeft,5,5,5,5));
  // end tab creation

  // embedded canvas for particle paths for Region 1
   TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas(0,fCompositeFrame1,950,380);
   Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   c1 = new TRootEmbeddedCanvas("embedded canvas1",fCompositeFrame1,1200,400);
   fCompositeFrame1->AddFrame(c1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,1));
   c1->GetCanvas()->Divide(3,1);  // have no idea what this line of code divides, but it looks important
  // end of embedded canvas for Region 2

  // embedded canvas for particle paths for Region 2
   TRootEmbeddedCanvas *fRootEmbeddedCanvas2 = new TRootEmbeddedCanvas(0,fCompositeFrame2,950,380);
   Int_t wfRootEmbeddedCanvas2 = fRootEmbeddedCanvas2->GetCanvasWindowId();
   c2 = new TRootEmbeddedCanvas("embedded canvas2",fCompositeFrame2,1200,400);
   fCompositeFrame2->AddFrame(c2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,1));
   c2->GetCanvas()->Divide(3,1);  // have no idea what this line of code divides, but it looks important
  // end of embedded canvas for Region 2

  // embedded canvas for particle paths for Region 3
   TRootEmbeddedCanvas *fRootEmbeddedCanvas3 = new TRootEmbeddedCanvas(0,fCompositeFrame3,950,380);
   Int_t wfRootEmbeddedCanvas3 = fRootEmbeddedCanvas3->GetCanvasWindowId();
   c3 = new TRootEmbeddedCanvas("embedded canvas3",fCompositeFrame3,1200,400);
   fCompositeFrame3->AddFrame(c3, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,5,1));
   c3->GetCanvas()->Divide(3,1);  // have no idea what this line of code divides, but it looks important
 // end embedded canvas for Region 3

   fTab->SetTab(0);
   fTab->Resize(fTab->GetDefaultSize());
   fMain->AddFrame(fTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,15,2,2,2)); // adds tabs to Main Frame
   fTab->MoveResize(5,700,1200,350); // give position and size of windows for the tab


  // create buttons to display the events
    //create a horizontal frame widget with buttons
    TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40);

    // create "Advance" button
    TGTextButton *advance = new TGTextButton(hframe,"&Advance");
    advance->Connect("Clicked()","QwEventDisplay",this,"DoAdvance()");
    hframe->AddFrame(advance, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

    // create "Previous" button
    TGTextButton *previous = new TGTextButton(hframe,"&Previous");
    previous->Connect("Clicked()","QwEventDisplay",this,"DoPrevious()");
    hframe->AddFrame(previous, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

    // create "Remove All" button
    TGTextButton *removeAll = new TGTextButton(hframe,"&Remove All");
    removeAll->Connect("Clicked()","QwEventDisplay",this,"DoRemoveAll()");
    hframe->AddFrame(removeAll, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

    // create "Exit" button
    TGTextButton *exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
    hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

    // add all buttons to main frame fMain
    fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  // end of buttons to display the events


  // set a name to the main frame
  fMain->SetWindowName("Q-Weak Single Event Display");

  // map all subwindows of main frame
  fMain->MapSubwindows();

  // initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());

  // map main frame
  fMain->MapWindow();

  // open the input file and load the Tree "h10" into memory (for Region 2 only)
  f = new TFile("proc_drift.root");
  t1 = (TTree*)f->Get("h10");

// open the input file and load the Tree "QweakG4_Tree" into memory
// containing data for all three regions
// f = new TFile("QweakSim.root"); // new data file used to test sev.C
// t1 = (TTree*)f->Get("QweakG4_Tree");

  // setup members of h10 to be read into as
  char tmp[10]; // character array used to display wire number
  for(j=0;j<6;j++) {
    sprintf(tmp,"w_%s",R2planes[j]);
    t1->GetBranch(tmp)->SetAddress(&wire[j]);
    sprintf(tmp,"%s_dd",R2planes[j]);
    t1->GetBranch(tmp)->SetAddress(&R2driftdist[j]);
    sprintf(tmp,"s_%s",R2planes[j]);
    t1->GetBranch(tmp)->SetAddress(&driftsign[j]);
    sprintf(tmp,"%s_px",R2planes[j]);
    t1->GetBranch(tmp)->SetAddress(&fitpx[j]);
    sprintf(tmp,"%s_py",R2planes[j]);
    t1->GetBranch(tmp)->SetAddress(&fitpy[j]);
  } // end for

/*
// setup members of QweakG4_Tree to be read into array
  char WireHit[10];
  for(j=0,j<3;j++) {
  sprintf(WireHit,"
*/
} // end of object "QwEventDisplay"


// When the "Advance" button is clicked, this function executes
void QwEventDisplay::DoAdvance() {
  //Load event data into memory
  entry++;
  DrawEvent();
} // end DoAdvance

// When the "Previous" button is clicked, this function executes
void QwEventDisplay::DoPrevious() {
//  Unload event data from memory
  entry--;
  DrawEvent();
} // end DoPrevious

// When the "Remove All" button is clicked, this function executes
void QwEventDisplay::DoRemoveAll() {
  // Remove all wire hits from listbox
  Int_t j;
  for(j=0;j<3;j++) {
  (*listboxes[j])->RemoveAll();
  (EventBox)->RemoveAll();
  } // end for
} // end DoRemoveAll()

void QwEventDisplay::DrawEvent() {
  // get entry from tree
  t1->GetEntry(entry);

  //Begin Printing events for all planes
  char tmp[25]; //  char event[25];

  // incrementing variables for Eventbox & Region 1, 2, and 3 listboxes
  Int_t j,i,s;

  for(j=0;j<6;j++) {
    if(wire[j]>0) {
      sprintf(tmp,"Wire %i",wire[j]);
      (*listboxes[1])->AddEntry(tmp,entry);
      (*listboxes[1])->MapSubwindows();
      (*listboxes[1])->Layout();
    } // end if
  } // end for

  char tmp2 = ' '; // array with blank space; creates new line b/w wire #
  char *ptmp2; // pointer to array with blank space
  ptmp2 = &tmp2; // assign pointer ptmp2 to address location of tmp2 array
  (*listboxes[1])->AddEntry(ptmp2,entry); // create new line

  // print the event numnber
  sprintf(tmp,"Event %i",entry+1);
  (EventBox)->RemoveAll();
  (EventBox)->AddEntry(tmp,entry);
  (EventBox)->MapSubwindows();
  (EventBox)->Layout();

  // draw the wire hits in the Region 2 canvas called "c2"
  c2->GetCanvas()->cd(1)->Clear();
  label1->Draw();

  b1->SetX1(.2);
  b1->SetY1(.1);
  b1->SetX2(.8);
  b1->SetY2(.9);
  b1->SetLineColor(1);
  b1->SetFillStyle(0);
  b1->Draw();

  lx->SetX1(.2);
  lx->SetY1(.1+0.02353*wire[0]+R2driftdist[0]*driftsign[0]*2.099737533e-3);
  lx->SetX2(.8);
  lx->SetY2(.1+0.02353*wire[0]+R2driftdist[0]*driftsign[0]*2.099737533e-3);
  lx->SetLineColor(kRed);
  lx->Draw();
/*  // user-friendly variable name "R2lx" used with code above
  R2lx->SetX1(.2);
  R2lx->SetY1(.1+0.02353*wire[0]+driftdist[0]*driftsign[0]*2.099737533e-3);
  R2lx->SetX2(.8);
  R2lx->SetY2(.1+0.02353*wire[0]+driftdist[0]*driftsign[0]*2.099737533e-3);
  R2lx->SetLineColor(kRed);
  R2lx->Draw();
*/

/*    // draw the wire hits in the Region 3 canvas called "c3"
  c3->GetCanvas()->cd(1)->Clear();
  R3b1->SetX1(.2);
  R3label1->Draw();

  R3b1->SetX1(.2);
  R3b1->SetY1(.1);
  R3b1->SetX2(.8);
  R3b1->SetY2(.9);
  R3b1->SetLineColor(1);
  R3b1->SetFillStyle(0);
  R3b1->Draw();

  // Region 3 line stuff
  R3lu->SetX1(.2);
  R3lu->SetY1(.1+0.02353*wire[0]+driftdist[0]*driftsign[0]*2.099737533e-3);
  R3lu->SetX2(.8);
  R3lu->SetY2(.1+0.02353*wire[0]+driftdist[0]*driftsign[0]*2.099737533e-3);
  R3lu->SetLineColor(kRed);
  R3lu->Draw();
*/


   if(wire[1]<17) {
     //     lu = new TLine(.6755974-(wire[1]-1)*.0306667,.1,.8,.2658285+(wire[1]-1)*.04087874);
     lu->SetX1(.6755974-(wire[1]-1)*0.0306667-R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
     lu->SetY1(.1);
     lu->SetX2(.8);
     lu->SetY2(.2658285+(wire[1]-1)*0.04087874+R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
     lu->SetLineColor(kGreen);
     lu->Draw();
/*   // user-friendly variable name used with code above
     R2lu->SetX1(.6755974-(wire[1]-1)*0.0306667-R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
     R2lu->SetY1(.1);
     R2lu->SetX2(.8);
     R2lu->SetY2(.2658285+(wire[1]-1)*0.04087874+R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
     R2lu->SetLineColor(kGreen);
     R2lu->Draw();
*/
   } // end if
   else {
     //     lu = new TLine(.2,.120061+(wire[1]-17)*.0408534,.785100-(wire[1]-17)*0.0306477,.9);
     lu->SetX1(.2);
     lu->SetY1(.120061+(wire[1]-17)*0.0408534+R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
     lu->SetX2(.785100-(wire[1]-17)*0.0306477-R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
     lu->SetY2(.9);
     lu->SetLineColor(kGreen);
     lu->Draw();
/*   // user-friendly variable name used with code above
     R2lu->SetX1(.2);
     R2lu->SetY1(.120061+(wire[1]-17)*0.0408534+R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
     R2lu->SetX2(.785100-(wire[1]-17)*0.0306477-R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
     R2lu->SetY2(.9);
     R2lu->SetLineColor(kGreen);
     R2lu->Draw();
*/
   }  // end else
   if(wire[2]<17) {
     //     lv = new TLine(.2,.26523+(wire[2]-1)*0.040853,.323957+(wire[2]-1)*0.0306477,.1);
     lv->SetX1(.2);
     lv->SetY1(.26523+(wire[2]-1)*0.040853);
     lv->SetX2(.323957+(wire[2]-1)*0.0306477);
     lv->SetY2(.1);
     lv->SetLineColor(kBlue);
     lv->Draw();
/*   // user-friendly variable name used with code above
     R2lv->SetX1(.2);
     R2lv->SetY1(.26523+(wire[2]-1)*0.040853);
     R2lv->SetX2(.323957+(wire[2]-1)*0.0306477);
     R2lv->SetY2(.1);
     R2lv->SetLineColor(kBlue);
     R2lv->Draw();
*/
   } // end if
   else {
     //     lv = new TLine(0.214179+(wire[2]-17)*0.0306667,.9,.8,.1191+(wire[2]-17)*0.0408787);
     lv->SetX1(0.214179+(wire[2]-17)*0.0306667);
     lv->SetY1(.9);
     lv->SetX2(.8);
     lv->SetY2(.1191+(wire[2]-17)*0.0408787);
     lv->SetLineColor(kBlue);
     lv->Draw();
/*   // user-friendly variable name used with code above
     R2lv->SetX1(0.214179+(wire[2]-17)*0.0306667);
     R2lv->SetY1(.9);
     R2lv->SetX2(.8);
     R2lv->SetY2(.1191+(wire[2]-17)*0.0408787);
     R2lv->SetLineColor(kBlue);
     R2lv->Draw();
*/
   } // end else

   //   lxp = new TLine(.2,.1+0.02353*wire[3],.8,.1+0.02353*wire[3]);
     lxp->SetX1(.2);
     lxp->SetY1(.1+0.02353*wire[3]+R2driftdist[3]*driftsign[3]*2.099737533e-3);
     lxp->SetX2(.8);
     lxp->SetY2(.1+0.02353*wire[3]+R2driftdist[3]*driftsign[3]*2.099737533e-3);
     lxp->SetLineColor(kRed+3);
     lxp->Draw();
/*   // user-friendly variable name used with code above
     R2lxPrime->SetX1(.2);
     R2lxPrime->SetY1(.1+0.02353*wire[3]+R2driftdist[3]*driftsign[3]*2.099737533e-3);
     R2lxPrime->SetX2(.8);
     R2lxPrime->SetY2(.1+0.02353*wire[3]+R2driftdist[3]*driftsign[3]*2.099737533e-3);
     R2lxPrime->SetLineColor(kRed+3);
     R2lxPrime->Draw();
*/

   if(wire[4]<17) {
     //     lup = new TLine(.6755974-(wire[4]-1)*.0306667,.1,.8,.2658285+(wire[4]-1)*.04087874);
     lup->SetX1(.6755974-(wire[4]-1)*0.0306667-R2driftdist[4]*driftsign[4]*1.25*2.099737533e-3);
     lup->SetY1(.1);
     lup->SetX2(.8);
     lup->SetY2(.2658285+(wire[4]-1)*0.04087874+R2driftdist[4]*driftsign[4]*1.667*2.099737533e-3);
     lup->SetLineColor(kGreen+3);
     lup->Draw();
/*   // user-friendly variable name used with code above
     R2luPrime->SetX1(.6755974-(wire[4]-1)*0.0306667-R2driftdist[4]*driftsign[4]*1.25*2.099737533e-3);
     R2luPrime->SetY1(.1);
     R2luPrime->SetX2(.8);
     R2luPrime->SetY2(.2658285+(wire[4]-1)*0.04087874+R2driftdist[4]*driftsign[4]*1.667*2.099737533e-3);
     R2luPrime->SetLineColor(kGreen+3);
     R2luPrime->Draw();
*/
   } // end if
   else {
     //     lup = new TLine(.2,.120061+(wire[4]-17)*.0408534,.785100-(wire[4]-17)*0.0306477,.9);
     lup->SetX1(.2);
     lup->SetY1(.120061+(wire[4]-17)*0.0408534+R2driftdist[4]*driftsign[4]*1.667*2.099737533e-3);
     lup->SetX2(.785100-(wire[4]-17)*0.0306477-R2driftdist[4]*driftsign[4]*1.25*2.099737533e-3);
     lup->SetY2(.9);
     lup->SetLineColor(kGreen+3);
     lup->Draw();
/*   // user-friendly variable name used with code above
     R2luPrime->SetX1(.2);
     R2luPrime->SetY1(.120061+(wire[4]-17)*0.0408534+R2driftdist[4]*driftsign[4]*1.667*2.099737533e-3);
     R2luPrime->SetX2(.785100-(wire[4]-17)*0.0306477-R2driftdist[4]*driftsign[4]*1.25*2.099737533e-3);
     R2luPrime->SetY2(.9);
     R2luPrime->SetLineColor(kGreen+3);
     R2luPrime->Draw();
*/
   } // end else
   if(wire[5]<17) {
     //     lvp = new TLine(.2,.26523+(wire[5]-1)*0.040853,.323957+(wire[5]-1)*0.0306477,.1);
     lvp->SetX1(.2);
     lvp->SetY1(0.26523+(wire[5]-1)*0.040853);
     lvp->SetX2(0.323957+(wire[5]-1)*0.0306477);
     lvp->SetY2(.1);
     lvp->SetLineColor(kBlue+3);
     lvp->Draw();
/*   // user-friendly variable name used with code above
     R2lvPrime->SetX1(.2);
     R2lvPrime->SetY1(0.26523+(wire[5]-1)*0.040853);
     R2lvPrime->SetX2(0.323957+(wire[5]-1)*0.0306477);
     R2lvPrime->SetY2(.1);
     R2lvPrime->SetLineColor(kBlue+3);
     R2lvPrime->Draw();
*/
   } // end if
   else {
     //     lvp = new TLine(0.214179+(wire[5]-17)*0.0306667,.9,.8,.1191+(wire[5]-17)*0.0408787);
     lvp->SetX1(0.214179+(wire[5]-17)*0.0306667);
     lvp->SetY1(.9);
     lvp->SetX2(.8);
     lvp->SetY2(.1191+(wire[5]-17)*0.0408787);
     lvp->SetLineColor(kBlue+3);
     lvp->Draw();
/*   // user-friendly variable name used with code above
     R2lvPrime->SetX1(0.214179+(wire[5]-17)*0.0306667);
     R2lvPrime->SetY1(.9);
     R2lvPrime->SetX2(.8);
     R2lvPrime->SetY2(.1191+(wire[5]-17)*0.0408787);
     R2lvPrime->SetLineColor(kBlue+3);
     R2lvPrime->Draw();
*/
   } // end else

//  R1text->Draw();

   R2text->Draw();

//   R3text->Draw();

// ASK RAKITHIA WHAT THE FOLLOWING CODE WAS SUPPOSED TO DO
//    points->SetNextPoint(.6,.6);
//    points->SetNextPoint(.7,.7);
//    points->SetMarkerColor(kBlue);
//    points->SetNextPoint(.8,.7);
//    points->Draw();


  c2->GetCanvas()->cd(2)->Clear();
  label2->Draw();

  b2->SetX1(.1);
  b2->SetY1(.634);
  b2->SetX2(.9);
  b2->SetY2(.367);
  b2->SetLineColor(1);
  b2->SetFillStyle(0);
  b2->Draw();

  xzfit->SetX1(fitpx[5]*2.8e-3);
  xzfit->SetY1(.367);
  xzfit->SetX2(fitpx[1]*2.8e-3);
  xzfit->SetY2(.634);
  xzfit->Draw();

  c2->GetCanvas()->cd(3)->Clear();
  label3->Draw();
//change the size of the box
//check with Mark and Paul
  b3->SetX1(.1);
  b3->SetY1(.6);
  b3->SetX2(.9);
  b3->SetY2(.4);
  b3->SetLineColor(1);
  b3->SetFillStyle(0);
  b3->Draw();
//flip x and y
//check with Mark and Paul
  yzfit->SetX1(fitpy[5]*2.1e-3);
  yzfit->SetY1(.4);
  yzfit->SetX2(fitpy[1]*2.1e-3);
  yzfit->SetY2(.6);
  yzfit->Draw();

  c2->GetCanvas()->Update();

/*
  // Region 3 draw stuff, basically copied from the Region 2 stuff, but for two planes instead
  c3->GetVanvas()->cd(2)->Clear();  // not sure what the "2" inside cd(2) means
  R3label2->Draw();

  R3b2->SetX1(.1);
  R3b2->SetY1(.634);
  R3b2->SetX2(.9);
  R3b2->SetY2(.367);
  R3b2->SetLineColor(1);
  R3b2->SetFillStyle(0);
  R3b2->Draw();

  R3xzfit->SetX1();
  R3xzfit->SetY1(.367);
  R3xzfit->SetX2(R3fitpx[1]*2.8e-3);
  R3xzfit->SetY2(.634);
  R3xzfit->Draw();

  c3->GetCanvas()->cd(2)->Clear();  // still not sure what the "2" inside cd(2) means
  R3label3->Draw();

  R3b3->SetX1(.1);
  R3b3->SetY1(.6);
  R3b3->SetX2(.9);
  R3b3->SetY2(.4);
  R3b3->SetLineColor(1);
  R3b3->SetFillStyle(0);
  R3b3->Draw();

  R3yzfit->SetX1(R3fitpy[5]*2.1e-3);
  R3yzfit->SetY1(.4);
  R3yzfit->SetX2(R3fitpy[1]*2.1e-3);
  R3yzfit->SetY2(.6);
  R3yzfit->Draw();

  c3->GetCanvas()->Update();
*/
} // end function DrawEvent()

QwEventDisplay::~QwEventDisplay() {
  //clean up
  fMain->Cleanup();
  delete fMain;
} // end object QwEventDisplay
