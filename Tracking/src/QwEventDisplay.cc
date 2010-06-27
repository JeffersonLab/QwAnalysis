/*
**QWEAK EVENT DISPLAY
**Marcus Hendricks and Derek Jones
**The George Washington University
**
**Qweak Collaboration
**Jefferson Lab Hall C
**
**Single event display for Regions 1, 2, and 3
*/

#include "QwEventDisplay.h"

ClassImp(QwEventDisplay); 

// Qweak header files
#include "QwTreeEventBuffer.h"
#include "QwSubsystemArrayTracking.h"
#include "QwDetectorInfo.h"
#include "QwHitContainer.h"


QwEventDisplay::QwEventDisplay(const TGWindow *p,UInt_t w,UInt_t h)
{
  fSubsystemArray = 0;
  fEventBuffer = 0;
  fHitList = 0;

  const char *R2planes[] = {"x","u","v","xp","up","vp"};
  const char *R3planes[] = {"u","v","u_prime","v_prime"};

  // Assign values of listboxes to addresses of Region boxes and the Hit Number
  listboxes[0] =  &Region1Box;
  listboxes[1] =  &Region2Box;
  listboxes[2] =  &Region3Box;
  listboxes[3] =  &EventBox;
/*
  //Assign values of title boxes for listboxes
  boxes[0] = &R1Box;
  boxes[1] = &R2Box;
  boxes[2] = &R3Box;
  boxes[3] = &EBox;
*/
  //Assign values of title boxes for listboxes
  boxes[0] = &Evlabel1;
  boxes[1] = &Evlabel2;
  boxes[2] = &Evlabel3;
  boxes[3] = &Evlabel4;

  // create a main frame called "fMain"
  fMain = new TGMainFrame(p,w,h);
  // set a name to the main frame
  fMain->SetWindowName("Q-Weak Single Event Display");

/*
  //create a horizontal frame to contain listbox titles
  TGCompositeFrame *labelhframe = new TGCompositeFrame(fMain);

   // Event box labels
   Evlabel1 = new TPaveText(.05,.92,.97,.97, "ARC");
   Evlabel2 = new TPaveText(.05,.92,.97,.97, "ARC");
   Evlabel3 = new TPaveText(.05,.92,.97,.97, "ARC");
   Evlabel4 = new TPaveText(.05,.92,.97,.97, "ARC");

   for(int j = 0; j < 4; j++)
     (*boxes[j]) = new TPaveText(labelhframe);

  // put the labels in the hframe and put the hframe in a larger 5,5,5,5)
   labelhframe->AddFrame(Evlabel1, new TGLayoutHints(kLHintsNormal, 5,5,5,5));
   labelhframe->AddFrame(Evlabel2, new TGLayoutHints(kLHintsNormal, 5,5,5,5));
   labelhframe->AddFrame(Evlabel3, new TGLayoutHints(kLHintsNormal, 5,5,5,5));
   labelhframe->AddFrame(Evlabel4, new TGLayoutHints(kLHintsNormal, 5,5,5,5));
  fMain->AddFrame(labelhframe, new TGLayoutHints(kLHintsNormal)); // add the labelhframe to the Main Frame

   //Draw event box labels
   Evlabel1->AddText("Event Counter")->Draw();
   Evlabel2->AddText("[box 2]")->Draw();
   Evlabel3->AddText("Wire Hit List")->Draw();
   Evlabel4->AddText("[box 4]")->Draw();
*/

  // create a horizontal frames (hframe) to contain the listboxes
  TGHorizontalFrame *listboxhframe = new TGHorizontalFrame(fMain);
/*
  //create vertical frames for each listbox
  TGVerticalFrame *listboxvframe = new TGVerticalFrame(listboxhframe);
  // create label for each list box
  for(int j = 0; j < 4; j++)
    (*boxes[j]) = new TPaveText(listboxvframe);
*/
  // create list boxes for each wire plane
  for (int j = 0; j < 4; j++) {
    (*listboxes[j]) = new TGListBox(listboxhframe);
  } // end for
  // initialize the event counter
  fEntry = -1;
   // tab widget
   TGTab *fTab = new TGTab(fMain);
/*
   // Event box labels
   Evlabel1 = new TPaveText(.05,.92,.97,.97, "ARC");
   Evlabel2 = new TPaveText(.05,.92,.97,.97, "ARC");
   Evlabel3 = new TPaveText(.05,.92,.97,.97, "ARC");
   Evlabel4 = new TPaveText(.05,.92,.97,.97, "ARC");
*/
// **need to learn how many planes of wires there will be in the detectors for Region 1


  // Region 2 fit lines
  R2xzfit = new TLine();
  R2yzfit = new TLine();
  // Region 2 boxes
  R2b1 = new TBox();
  R2b2 = new TBox();
  R2b3 = new TBox();
  // Region 2 canvas labels for particle paths
  R2label1 = new TPaveLabel(.05,.92,.97,.97,"Front View (X-Y Projection)");
  R2label2 = new TPaveLabel(.05,.92,.97,.97,"Top View (X-Z Projection)");
  R2label3 = new TPaveLabel(.05,.92,.97,.97,"Side View (Y-Z Projection)");
  R2text = new TPaveText(.85,.17,.95,.9);
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


// **finish Region 3 section

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
  R3label1 = new TPaveLabel(.05,.92,.97,.97,"Front View (X-Y Projection)");
  R3label2 = new TPaveLabel(.05,.92,.97,.97,"Top View (X-Z Projection)");
  R3label3 = new TPaveLabel(.05,.92,.97,.97,"Side View (Y-Z Projection)");


// **need to learn how many planes of wires there will be in the detectors for Region 1


  // Region 2 fit lines
  R2xzfit = new TLine();
  R2yzfit = new TLine();
  // Region 2 boxes
  R2b1 = new TBox();
  R2b2 = new TBox();
  R2b3 = new TBox();
  // Region 2 canvas labels for particle paths
  R2label1 = new TPaveLabel(.05,.92,.97,.97,"Front View (X-Y Projection)");
  R2label2 = new TPaveLabel(.05,.92,.97,.97,"Top View (X-Z Projection)");
  R2label3 = new TPaveLabel(.05,.92,.97,.97,"Side View (Y-Z Projection)");
  R2text = new TPaveText(.85,.17,.95,.9);
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


// **finish Region 3 section

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
  R3label1 = new TPaveLabel(.05,.92,.97,.97,"Front View (X-Y Projection)");
  R3label2 = new TPaveLabel(.05,.92,.97,.97,"Top View (X-Z Projection)");
  R3label3 = new TPaveLabel(.05,.92,.97,.97,"Side View (Y-Z Projection)");


  // put the listboxes in the hframe and put the hframe in a larger hfrakLHintsTop | kLHintsLeft,5,5,5,5)me
  // add listboxes to hframes that contain wire hit data
  listboxhframe->AddFrame(  EventBox  , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  listboxhframe->AddFrame(  Region1Box , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  listboxhframe->AddFrame(  Region2Box , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  listboxhframe->AddFrame(  Region3Box , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));
  fMain->AddFrame(listboxhframe, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal)); // add the listbixhframe to the Main Frame
  for (int j = 0; j < 3; j++) {
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

    // number fEntry box
    TGNumberEntry *fNumberEntry = new TGNumberEntry(fMain, (Double_t) 0,13,-1,(TGNumberFormat::EStyle) 5);
    fNumberEntry->Connect("Clicked()","QwEventDisplay",this,"DoAdvance()"); // code to connect the "Advance" button to the number fEntry widget
    hframe->AddFrame(fNumberEntry, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY|kLHintsNormal,5,5,5,5));

    // add all buttons and number fEntry box to main frame fMain
    fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));


  // Open the input file and load the Tree "h10" into memory (for Region 2 only)
  //f = new TFile("proc_drift.root");
  //t1 = (TTree*)f->Get("h10");

  // Open the input file and load the Tree "QweakG4_Tree" into memory
  // containing data for all three regions
  //f = new TFile("QweakSim.root"); // new data file used to test sev.C
  //t1 = (TTree*)f->Get("QweakG4_Tree");

  // Setup members of h10 to be read into as
  //char tmp[10]; // character array used to display wire number
  //for (int j = 0; j < 6; j++) {
  //  sprintf(tmp,"w_%s",R2planes[j]);
  //  t1->GetBranch(tmp)->SetAddress(&wire[j]);
  //  sprintf(tmp,"%s_dd",R2planes[j]);
  //  t1->GetBranch(tmp)->SetAddress(&R2driftdist[j]);
  //  sprintf(tmp,"s_%s",R2planes[j]);
  //  t1->GetBranch(tmp)->SetAddress(&driftsign[j]);
  //  sprintf(tmp,"%s_px",R2planes[j]);
  //  t1->GetBranch(tmp)->SetAddress(&fitpx[j]);
  //  sprintf(tmp,"%s_py",R2planes[j]);
  //  t1->GetBranch(tmp)->SetAddress(&fitpy[j]);
  //} // end for

/*
// setup members of QweakG4_Tree to be read into array
  char WireHit[10];
  for (int j = 0; j < 6; j++) {
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
void QwEventDisplay::DoAdvance()
{
  // Load event data into memory
  fEntry++;

  DrawEvent();
} // end DoAdvance

// When the "Previous" button is clicked, this function executes
void QwEventDisplay::DoPrevious()
{
  // Load event data into memory
  fEntry--;

  DrawEvent();
} // end DoPrevious

// When the "Remove All" button is clicked, this function executes
void QwEventDisplay::DoRemoveAll()
{
  // Remove all wire hits from listbox
  for (int j = 0; j < 3; j++) {
    (*listboxes[j])->RemoveAll();
    (EventBox)->RemoveAll();
  } // end for
} // end DoRemoveAll()

void QwEventDisplay::DrawEvent()
{
  fEventBuffer->GetEntry(fEntry);
  fHitList = fEventBuffer->GetHitList();

  // fHitList = fSubsystemArray->GetHitList();
  fHitList->Print();

  // Begin Printing events for all planes
  char tmp[25]; //  char event[25];

  // Incrementing variables for Eventbox & Region 1, 2, and 3 listboxes
  for (QwHitContainer::iterator hit  = fHitList->begin();
                                hit != fHitList->end(); hit++) {
    if (hit->GetElement() > 0) {
      sprintf(tmp,"Wire %i", hit->GetElement());
      (*listboxes[1])->AddEntry(tmp,fEntry);
      (*listboxes[1])->MapSubwindows();
      (*listboxes[1])->Layout();
    } // end if
  } // end for

  // Get region 1 hits
  QwHitContainer* hits_r1r = fHitList->GetSubList_Dir(kRegionID1, kPackageUp, kDirectionR);
  QwHitContainer* hits_r1y = fHitList->GetSubList_Dir(kRegionID1, kPackageUp, kDirectionY);
  // Get region 2 hits
  QwHitContainer* hits_r2x = fHitList->GetSubList_Dir(kRegionID2, kPackageUp, kDirectionX);
  QwHitContainer* hits_r2u = fHitList->GetSubList_Dir(kRegionID2, kPackageUp, kDirectionU);
  QwHitContainer* hits_r2v = fHitList->GetSubList_Dir(kRegionID2, kPackageUp, kDirectionV);
  // Get region 3 hits
  QwHitContainer* hits_r3u = fHitList->GetSubList_Dir(kRegionID3, kPackageUp, kDirectionU);
  QwHitContainer* hits_r3v = fHitList->GetSubList_Dir(kRegionID3, kPackageUp, kDirectionV);

  (*listboxes[1])->AddEntry(Form(""),fEntry); // create new line using the "Form" function to display text

  // print the event numnber
  //sprintf(tmp,"Hit %i",fEntry+1);
  (EventBox)->RemoveAll();
  (EventBox)->AddEntry(Form("Hit %i",fEntry+1),fEntry);
  (EventBox)->MapSubwindows();
  (EventBox)->Layout();

  //REGION 1 WIRE HITS!!!

  // draw the wire hits in the Region 2 canvas called "c2"
  c2->GetCanvas()->cd(1)->Clear();
  R2label1->Draw();

  // R2 frame
  R2b1->SetX1(.2);
  R2b1->SetY1(.1);
  R2b1->SetX2(.8);
  R2b1->SetY2(.9);
  R2b1->SetLineColor(1);
  R2b1->SetFillStyle(0);
  R2b1->Draw();

  // R2 x wires
  R2lx.clear();
  for (QwHitContainer::iterator hit  = hits_r2x->begin();
                                hit != hits_r2x->end(); hit++) {
    int wire = hit->GetElement();
    int plane = hit->GetPlane();
    TLine line;
    line.SetX1(.2);
    line.SetY1(.1+0.02353*(wire + 0.1*plane) + R2driftdist[0]*driftsign[0]*2.099737533e-3);
    line.SetX2(.8);
    line.SetY2(.1+0.02353*(wire + 0.1*plane) + R2driftdist[0]*driftsign[0]*2.099737533e-3);
    line.SetLineColor(kRed);
    R2lx.push_back(line);
    R2lx.back().Draw();
  }

  // R2 u wires
  for (QwHitContainer::iterator hit  = hits_r2u->begin();
                                hit != hits_r2u->end(); hit++) {
    int wire = hit->GetElement();
    int plane = hit->GetPlane();
    TLine line;
    if (wire < 17) {
      line.SetX1(.6755974-(wire-1 + 0.1*plane)*0.0306667 - R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
      line.SetY1(.1);
      line.SetX2(.8);
      line.SetY2(.2658285+(wire-1 + 0.1*plane)*0.04087874 + R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
      line.SetLineColor(kGreen);
    } else {
      line.SetX1(.2);
      line.SetY1(.120061+(wire-17 + 0.1*plane)*0.0408534 + R2driftdist[1]*driftsign[1]*1.667*2.099737533e-3);
      line.SetX2(.785100-(wire-17 + 0.1*plane)*0.0306477 - R2driftdist[1]*driftsign[1]*1.25*2.099737533e-3);
      line.SetY2(.9);
      line.SetLineColor(kGreen);
    }
    R2lu.push_back(line);
    R2lu.back().Draw();
  }

  // R2 v wires
  for (QwHitContainer::iterator hit  = hits_r2v->begin();
                                hit != hits_r2v->end(); hit++) {
    int wire = hit->GetElement();
    int plane = hit->GetPlane();
    TLine line;
    if (wire < 17) {
      line.SetX1(.2);
      line.SetY1(.26523+(wire-1 + 0.1*plane)*0.040853);
      line.SetX2(.323957+(wire-1 + 0.1*plane)*0.0306477);
      line.SetY2(.1);
      line.SetLineColor(kBlue);
    } else {
      line.SetX1(0.214179+(wire-17 + 0.1*plane)*0.0306667);
      line.SetY1(.9);
      line.SetX2(.8);
      line.SetY2(.1191+(wire-17 + 0.1*plane)*0.0408787);
      line.SetLineColor(kBlue);
    }
    R2lv.push_back(line);
    R2lv.back().Draw();
  }


/*  // draw the wire hits in the Region 3 canvas called "c3"
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
  // Needs to have all elements included; fix region 2 first then copy over
  c3->GetCanvas()->cd(2)->Clear();  // not sure what the "2" inside cd(2) means
  R3label2->Draw();

  R3b2->SetX1(.1);
  R3b2->SetY1(.634);
  R3b2->SetX2(.9);
  R3b2->SetY2(.367);
  R3b2->SetLineColor(1);
  R3b2->SetFillStyle(0);
  R3b2->Draw();

  //  R3xzfit->SetX1(); FIX
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


  delete hits_r1r;
  delete hits_r1y;

  delete hits_r2x;
  delete hits_r2u;
  delete hits_r2v;

  delete hits_r3u;
  delete hits_r3v;
} // end function DrawEvent()

QwEventDisplay::~QwEventDisplay() {
  //clean up
  fMain->Cleanup();
  delete fMain;
} // end object QwEventDisplay

