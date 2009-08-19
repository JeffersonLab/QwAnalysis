#include "QwEventDisplay.h"

ClassImp(QwEventDisplay)

// Qweak header files
#include "QwTreeEventBuffer.h"
#include "QwSubsystemArrayTracking.h"
#include "QwDetectorInfo.h"
#include "QwHitContainer.h"


QwEventDisplay::QwEventDisplay(const TGWindow *p,UInt_t w,UInt_t h) {
Int_t j,k,s;

  const char *R2planes[] = {"x","u","v","xp","up","vp"};
  const char *R3planes[] = {"u","v","u_prime","v_prime"};


//  assign values of listboxes to addresses of Region boxes and the Hit Number
  listboxes[0] =  &Region1Box;
  listboxes[1] =  &Region2Box;
  listboxes[2] =  &Region3Box;
  listboxes[3] =  &EventBox;

  // create a main frame called "fMain"
  fMain = new TGMainFrame(p,w,h);
  // set a name to the main frame
  fMain->SetWindowName("Q-Weak Single Event Display");

  // create a horizontal frames (hframe) to contain the listboxes
  TGHorizontalFrame *listboxhframe = new TGHorizontalFrame(fMain);
  // create list boxes for each wire plane
  for(j=0;j<4;j++){
    (*listboxes[j]) = new TGListBox(listboxhframe);
  } // end for
  // initialize the event counter
  entry = -1;
   // tab widget
   TGTab *fTab = new TGTab(fMain);


// need to learn how many planes of wires there will be in the detectors for Region 1


  // Region 2 lines
  R2lx = new TLine();
  R2lu = new TLine();
  R2lv = new TLine();
  R2lxPrime = new TLine();
  R2luPrime = new TLine();
  R2lvPrime = new TLine();
  // Region 2 fit lines
  R2xzfit = new TLine();
  R2yzfit = new TLine();
  // Region 2 boxes
  R2b1 = new TBox();
  R2b2 = new TBox();
  R2b3 = new TBox();
  // Region 2 canvas labels for particle paths
  R2label1 = new TPaveLabel(.05,.92,.97,.97,"X-Y Projection");
  R2label2 = new TPaveLabel(.05,.92,.97,.97,"Top View X-Z Projection");
  R2label3 = new TPaveLabel(.05,.92,.97,.97,"Side View Y-Z Projection");
  R2text = new TPaveText(.85,.05,.95,.9);
  // Region 2 plane labels
  R2text->AddText("x");
  R2text->AddText("u");
  R2text->AddText("v");
  // Region 2 prime plane labels
  R2text->AddText("x'");
  R2text->AddText("u'");
  R2text->AddText("v'");
  // Region 2 wire plane labels' colors
  R2text->SetTextSize(.0669);
  R2text->GetLine(0)->SetTextColor(kRed);
  R2text->GetLine(1)->SetTextColor(kGreen);
  R2text->GetLine(2)->SetTextColor(kBlue);
  R2text->GetLine(3)->SetTextColor(kRed+1);
  R2text->GetLine(4)->SetTextColor(kGreen+1);
  R2text->GetLine(5)->SetTextColor(kBlue+1);
  // find out what points are for
  points = new TPolyMarker(6);
  points->SetMarkerStyle(2);


  // Region 3 lines
  R3lu = new TLine();
  R3lv = new TLine();
  R3luPrime = new TLine();
  R3lvPrime = new TLine();
  // Region 3 fit lines
  R3xzfit = new TLine();
  R3yzfit = new TLine();
  // Region 3 boxes
  R3b1 = new TBox();
  R3b2 = new TBox();
  R3b3 = new TBox();
  // Region 3 canvas labels for particle paths
  R3label1 = new TPaveLabel(.05,.92,.97,.97,"X-Y Projection");
  R3label2 = new TPaveLabel(.05,.92,.97,.97,"Top View X-Z Projection");
  R3label3 = new TPaveLabel(.05,.92,.97,.97,"Side View Y-Z Projection");


  // put the listboxes in the hframe and put the hframe in a larger hfrakLHintsTop | kLHintsLeft,5,5,5,5)me
  // add listboxes to hframes that contain wire hit data
  listboxhframe->AddFrame(  EventBox  , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  listboxhframe->AddFrame(  Region1Box , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  listboxhframe->AddFrame(  Region2Box , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  listboxhframe->AddFrame(  Region3Box , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  fMain->AddFrame(listboxhframe, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal)); // add the listbixhframe to the Main Frame
  for(j=0;j<3;j++){
    (*listboxes[j])->Resize(200,212); // controls size of the listboxes--FOR REAL!
    (*listboxes[3])->Resize(110,25); // controlled th size of the listbox for the event number until the kLHintsExpandX etc. was added
  } // end for
//  end listboxes' creation


  // create tabs for each region
  // create container of "Region 1"
   TGCompositeFrame *fCompositeFrame1;
   fCompositeFrame1 = fTab->AddTab("Region 1");
   fCompositeFrame1->SetLayoutManager(new TGVerticalLayout(fCompositeFrame1));
   // embedded canvas for particle paths for Region 1
   TRootEmbeddedCanvas *fRootEmbeddedCanvas1 = new TRootEmbeddedCanvas();
   Int_t wfRootEmbeddedCanvas1 = fRootEmbeddedCanvas1->GetCanvasWindowId();
   c1 = new TRootEmbeddedCanvas("embedded canvas1",fCompositeFrame1,800,350);
   fCompositeFrame1->AddFrame(c1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,5,5));
   c1->GetCanvas()->Divide(3,1);
  // end of embedded canvas for Region 1

  // create container of "Region 2"
   TGCompositeFrame *fCompositeFrame2;
   fCompositeFrame2 = fTab->AddTab("Region 2");
   fCompositeFrame2->SetLayoutManager(new TGVerticalLayout(fCompositeFrame2));
   // embedded canvas for particle paths for Region 2
   TRootEmbeddedCanvas *fRootEmbeddedCanvas2 = new TRootEmbeddedCanvas();
   Int_t wfRootEmbeddedCanvas2 = fRootEmbeddedCanvas2->GetCanvasWindowId();
   c2 = new TRootEmbeddedCanvas("embedded canvas2",fCompositeFrame2,800,350);
   fCompositeFrame2->AddFrame(c2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,5,5));
   c2->GetCanvas()->Divide(3,1);
  // end of embedded canvas for Region 2

  // create container of "Region 3"
   TGCompositeFrame *fCompositeFrame3;
   fCompositeFrame3 = fTab->AddTab("Region 3");
   fCompositeFrame3->SetLayoutManager(new TGVerticalLayout(fCompositeFrame3));
   // embedded canvas for particle paths for Region 3
   TRootEmbeddedCanvas *fRootEmbeddedCanvas3 = new TRootEmbeddedCanvas();
   Int_t wfRootEmbeddedCanvas3 = fRootEmbeddedCanvas3->GetCanvasWindowId();
   c3 = new TRootEmbeddedCanvas("embedded canvas3",fCompositeFrame3,800,350);
   fCompositeFrame3->AddFrame(c3, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5));
   c3->GetCanvas()->Divide(3,1);
 // end embedded canvas for Region 3


   fTab->SetTab(0); //  Brings the composite frame with the index tabIndex to the
                    //  front and generate the following event if the front tab has changed:
   fTab->MoveResize(5,5,800,350); // give position and size of windows for the tab
   fMain->AddFrame(fTab,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5)); // adds tab to main frame fMain

  // create buttons to display the events
    //create a horizontal frame widget with buttons
    TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain);

    // create "Advance" button
    TGTextButton *advance = new TGTextButton(hframe,"&Advance");
    advance->Connect("Clicked()","QwEventDisplay",this,"DoAdvance()");
    hframe->AddFrame(advance, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
    //TQObject::Connect(advance, "Clicked()", "QwEventDisplay", this, "DoAdvance()");

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

    // number entry box
    TGNumberEntry *fNumberEntry = new TGNumberEntry(fMain, (Double_t) 0,13,-1,(TGNumberFormat::EStyle) 5);
    fNumberEntry->Connect("Clicked()","QwEventDisplay",this,"DoAdvance()"); // code to connect the "Advance" button to the number entry widget
    hframe->AddFrame(fNumberEntry, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));

    // add all buttons and number entry box to main frame fMain
    fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));


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
  for(j=0,j<6;j++) {
  sprintf(WireHit,"",R2planes[j]);
  t1->GetBranch(WireHit)->SetAddress(&wire[j]);
  sprintf(
*/


  // map all subwindows of main frame
  fMain->MapSubwindows();
  // initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());
  // map main frame
  fMain->MapWindow();


} // end of object "QwEventDisplay"


// When the "Advance" button is clicked, this function executes
void QwEventDisplay::DoAdvance() {
  //Load event data into memory
  entry++;

  QwHitContainer* hitlist = fEventBuffer->GetHitList(entry);
  //QwHitContainer* hitlist = fSubsystemArray->GetHitList();
  hitlist->Print();

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

  for(j=0;j<7;j++) {
    if(wire[j]>0) {
      sprintf(tmp,"Wire %i",wire[j]);
      (*listboxes[1])->AddEntry(tmp,entry);
      (*listboxes[1])->MapSubwindows();
      (*listboxes[1])->Layout();
    } // end if
  } // end for

  (*listboxes[1])->AddEntry(Form(""),entry); // create new line using the "Form" function to display text

  // print the event numnber
//  sprintf(tmp,"Hit %i",entry+1);
  (EventBox)->RemoveAll();
  (EventBox)->AddEntry(Form("Hit %i",entry+1),entry);
  (EventBox)->MapSubwindows();
  (EventBox)->Layout();

  // draw the wire hits in the Region 2 canvas called "c2"
  c2->GetCanvas()->cd(1)->Clear();
  R2label1->Draw();

  R2b1->SetX1(.2);
  R2b1->SetY1(.1);
  R2b1->SetX2(.8);
  R2b1->SetY2(.9);
  R2b1->SetLineColor(1);
  R2b1->SetFillStyle(0);
  R2b1->Draw();


  R2lx->SetX1(.2);
  R2lx->SetY1(.1+0.02353*wire[0]+R2driftdist[0]*driftsign[0]*2.099737533e-3);
  R2lx->SetX2(.8);
  R2lx->SetY2(.1+0.02353*wire[0]+R2driftdist[0]*driftsign[0]*2.099737533e-3);
  R2lx->SetLineColor(kRed);
  R2lx->Draw();

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
  R3lu->SetY1(.1+0.02353*wire[0]+R3driftdist[0]*driftsign[0]*2.099737533e-3);
  R3lu->SetX2(.8);
  R3lu->SetY2(.1+0.02353*wire[0]+R3driftdist[0]*driftsign[0]*2.099737533e-3);
  R3lu->SetLineColor(kRed);
  R3lu->Draw();
*/


   if(wire[1]<17) {
     R2lu->SetX1(.6755974-(wire[1]-1)*0.0306667-R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
     R2lu->SetY1(.1);
     R2lu->SetX2(.8);
     R2lu->SetY2(.2658285+(wire[1]-1)*0.04087874+R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
     R2lu->SetLineColor(kGreen);
     R2lu->Draw();

   } // end if
   else {
     R2lu->SetX1(.2);
     R2lu->SetY1(.120061+(wire[1]-17)*0.0408534+R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
     R2lu->SetX2(.785100-(wire[1]-17)*0.0306477-R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
     R2lu->SetY2(.9);
     R2lu->SetLineColor(kGreen);
     R2lu->Draw();

   }  // end else
   if(wire[2]<17) {
     R2lv->SetX1(.2);
     R2lv->SetY1(.26523+(wire[2]-1)*0.040853);
     R2lv->SetX2(.323957+(wire[2]-1)*0.0306477);
     R2lv->SetY2(.1);
     R2lv->SetLineColor(kBlue);
     R2lv->Draw();
   } // end if
   else {
   // user-friendly variable name used with code above
     R2lv->SetX1(0.214179+(wire[2]-17)*0.0306667);
     R2lv->SetY1(.9);
     R2lv->SetX2(.8);
     R2lv->SetY2(.1191+(wire[2]-17)*0.0408787);
     R2lv->SetLineColor(kBlue);
     R2lv->Draw();

   } // end else

     // wft should this be within an if or else statement?
     R2lxPrime->SetX1(.2);
     R2lxPrime->SetY1(.1+0.02353*wire[3]+R2driftdist[3]*driftsign[3]*2.099737533e-3);
     R2lxPrime->SetX2(.8);
     R2lxPrime->SetY2(.1+0.02353*wire[3]+R2driftdist[3]*driftsign[3]*2.099737533e-3);
     R2lxPrime->SetLineColor(kRed+3);
     R2lxPrime->Draw();


   if(wire[4]<17) {
     R2luPrime->SetX1(.6755974-(wire[4]-1)*0.0306667-R2driftdist[4]*driftsign[4]*1.25*2.099737533e-3);
     R2luPrime->SetY1(.1);
     R2luPrime->SetX2(.8);
     R2luPrime->SetY2(.2658285+(wire[4]-1)*0.04087874+R2driftdist[4]*driftsign[4]*1.667*2.099737533e-3);
     R2luPrime->SetLineColor(kGreen+3);
     R2luPrime->Draw();
   } // end if
   else {
     R2luPrime->SetX1(.2);
     R2luPrime->SetY1(.120061+(wire[4]-17)*0.0408534+R2driftdist[4]*driftsign[4]*1.667*2.099737533e-3);
     R2luPrime->SetX2(.785100-(wire[4]-17)*0.0306477-R2driftdist[4]*driftsign[4]*1.25*2.099737533e-3);
     R2luPrime->SetY2(.9);
     R2luPrime->SetLineColor(kGreen+3);
     R2luPrime->Draw();
   } // end else
   if(wire[5]<17) {
     R2lvPrime->SetX1(.2);
     R2lvPrime->SetY1(0.26523+(wire[5]-1)*0.040853);
     R2lvPrime->SetX2(0.323957+(wire[5]-1)*0.0306477);
     R2lvPrime->SetY2(.1);
     R2lvPrime->SetLineColor(kBlue+3);
     R2lvPrime->Draw();
   } // end if
   else {
     R2lvPrime->SetX1(0.214179+(wire[5]-17)*0.0306667);
     R2lvPrime->SetY1(.9);
     R2lvPrime->SetX2(.8);
     R2lvPrime->SetY2(.1191+(wire[5]-17)*0.0408787);
     R2lvPrime->SetLineColor(kBlue+3);
     R2lvPrime->Draw();
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
  R2label2->Draw();

  R2b2->SetX1(.1);
  R2b2->SetY1(.634);
  R2b2->SetX2(.9);
  R2b2->SetY2(.367);
  R2b2->SetLineColor(1);
  R2b2->SetFillStyle(0);
  R2b2->Draw();

  R2xzfit->SetX1(fitpx[5]*2.8e-3);
  R2xzfit->SetY1(.367);
  R2xzfit->SetX2(fitpx[1]*2.8e-3);
  R2xzfit->SetY2(.634);
  R2xzfit->Draw();

  c2->GetCanvas()->cd(3)->Clear();
  R2label3->Draw();
//change the size of the box
//check with Mark and Paul
  R2b3->SetX1(.1);
  R2b3->SetY1(.6);
  R2b3->SetX2(.9);
  R2b3->SetY2(.4);
  R2b3->SetLineColor(1);
  R2b3->SetFillStyle(0);
  R2b3->Draw();
//flip x and y
//check with Mark and Paul
  R2yzfit->SetX1(fitpy[5]*2.1e-3);
  R2yzfit->SetY1(.4);
  R2yzfit->SetX2(fitpy[1]*2.1e-3);
  R2yzfit->SetY2(.6);
  R2yzfit->Draw();

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

