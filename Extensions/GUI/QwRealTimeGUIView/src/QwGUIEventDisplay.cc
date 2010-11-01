#include "QwGUIEventDisplay.h"
//#include <iostream>
#include "TLine.h"

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
  // Event list frame
  dEventListFrame = NULL;
  dEventListFrameLayout = NULL;
  dEventListButtonsFrame = NULL;
  dEventListButtonsLayout = NULL;
  dEventListButtonNext = NULL;
  dEventListButtonPrevious = NULL;
  dEventListBox = NULL;
  dEventListBoxLayout = NULL;

  // Event display tabs
  dEventDisplayFrame = NULL;
  dEventDisplayFrameLayout = NULL;
  dEventDisplayTabs = NULL;
  dEventDisplayTabsLayout = NULL;
  dEventDisplayTabRegion1 = NULL;
  dEventDisplayTabRegion1_XY = NULL;
  dEventDisplayTabRegion1_XZ = NULL;
  dEventDisplayTabRegion2 = NULL;
  dEventDisplayTabRegion2_XY = NULL;
  dEventDisplayTabRegion2_XZ = NULL;
  dEventDisplayTabRegion3 = NULL;
  dEventDisplayTabRegion3_XY = NULL;
  dEventDisplayTabRegion3_XZ = NULL;
  dEventDisplayTabRegionLayout = NULL;

  fTree = NULL;
  fCurrentEvent = 0;

  AddThisTab(this);
}

QwGUIEventDisplay::~QwGUIEventDisplay()
{
  // Event list frame
  if(dEventListFrame)          delete dEventListFrame;
  if(dEventListFrameLayout)    delete dEventListFrameLayout;
  if(dEventListControlFrame)   delete dEventListControlFrame;
  if(dEventListControlLayout)  delete dEventListControlLayout;
  if(dEventListNumberEntry)    delete dEventListNumberEntry;
  if(dEventListButtonsFrame)   delete dEventListButtonsFrame;
  if(dEventListButtonsLayout)  delete dEventListButtonsLayout;
  if(dEventListButtonNext)     delete dEventListButtonNext;
  if(dEventListButtonPrevious) delete dEventListButtonPrevious;
  if(dEventListBox)            delete dEventListBox;
  if(dEventListBoxLayout)      delete dEventListBoxLayout;

  // Event display tabs
  if(dEventDisplayFrame)       delete dEventDisplayFrame;
  if(dEventDisplayFrameLayout) delete dEventDisplayFrameLayout;
  if(dEventDisplayTabs)        delete dEventDisplayTabs;
  if(dEventDisplayTabsLayout)  delete dEventDisplayTabsLayout;
  if(dEventDisplayTabRegion1)  delete dEventDisplayTabRegion1;
  if(dEventDisplayTabRegion1_XY) delete dEventDisplayTabRegion1_XY;
  if(dEventDisplayTabRegion1_XZ) delete dEventDisplayTabRegion1_XZ;
  if(dEventDisplayTabRegion2)  delete dEventDisplayTabRegion2;
  if(dEventDisplayTabRegion2_XY) delete dEventDisplayTabRegion2_XY;
  if(dEventDisplayTabRegion2_XZ) delete dEventDisplayTabRegion2_XZ;
  if(dEventDisplayTabRegion3)  delete dEventDisplayTabRegion3;
  if(dEventDisplayTabRegion3_XY) delete dEventDisplayTabRegion3_XY;
  if(dEventDisplayTabRegion3_XZ) delete dEventDisplayTabRegion3_XZ;
  if(dEventDisplayTabRegionLayout)  delete dEventDisplayTabRegionLayout;

  //  RemoveThisTab(this);
  //  IsClosing(GetName());
}

void QwGUIEventDisplay::MakeLayout()
{
  this->SetLayoutManager(new TGVerticalLayout(this));

  // Event list frame
  dEventListFrame = new TGHorizontalFrame(this);
  dEventListFrameLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsTop |
				kLHintsExpandX);
  AddFrame(dEventListFrame, dEventListFrameLayout);

  // Control frame
  dEventListControlFrame = new TGVerticalFrame(dEventListFrame);
  dEventListControlLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsTop |
				kLHintsExpandY);
  dEventListFrame->AddFrame(dEventListControlFrame, dEventListControlLayout);

  // Event number entry
  dEventListNumberEntry = new TGNumberEntry(dEventListControlFrame, (Int_t) 0, 13, -1, (TGNumberFormat::EStyle) 5);
  dEventListControlFrame->AddFrame(dEventListNumberEntry, new TGLayoutHints(kLHintsTop));

  // Buttons frame
  dEventListButtonsFrame = new TGHorizontalFrame(dEventListFrame);
  dEventListButtonsLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsTop |
				kLHintsExpandX |
				kLHintsExpandY);
  dEventListControlFrame->AddFrame(dEventListButtonsFrame, dEventListButtonsLayout);

  // Buttons
  dEventListButtonPrevious = new TGTextButton(dEventListButtonsFrame,"&Previous");
  //dEventListButtonPrevious->Connect("Clicked()","QwGUIEventDisplay",this,"PlotPrevious()");
  dEventListButtonNext     = new TGTextButton(dEventListButtonsFrame,"&Next");
  //dEventListButtonNext->Connect("Clicked()","QwGUIEventDisplay",this,"PlotNext()");
  dEventListButtonsFrame->AddFrame(dEventListButtonPrevious, new TGLayoutHints(kLHintsLeft));
  dEventListButtonsFrame->AddFrame(dEventListButtonNext,     new TGLayoutHints(kLHintsRight));

  // List box
  dEventListBox   = new TGListBox(dEventListFrame);
  dEventListBoxLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsTop |
				kLHintsExpandX |
				kLHintsExpandY,
				5, 5, 5, 5);
  dEventListFrame->AddFrame(dEventListBox, dEventListBoxLayout);


  // Event display frame
  dEventDisplayFrame = new TGHorizontalFrame(this);
  dEventDisplayFrameLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsBottom |
				kLHintsExpandX |
				kLHintsExpandY);
  AddFrame(dEventDisplayFrame, dEventDisplayFrameLayout);

  // Event display tabs
  dEventDisplayTabs = new TGTab(dEventDisplayFrame);
  dEventDisplayTabsLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsTop |
				kLHintsExpandX |
				kLHintsExpandY);

  dEventDisplayFrame->AddFrame(dEventDisplayTabs, dEventDisplayTabsLayout);
  // Region specific tabs
  dEventDisplayTabRegionLayout = new TGLayoutHints(
				kLHintsLeft |
				kLHintsTop |
				kLHintsExpandX |
				kLHintsExpandY,
				5, 5, 5, 5);
  // Region 1 tab
  dEventDisplayTabRegion1 = dEventDisplayTabs->AddTab("Region 1");
  dEventDisplayTabRegion1->SetLayoutManager(new TGHorizontalLayout(dEventDisplayTabRegion1));
  dEventDisplayTabRegion1_XY = new TRootEmbeddedCanvas("XY",dEventDisplayTabRegion1);
  dEventDisplayTabRegion1_XZ = new TRootEmbeddedCanvas("XZ",dEventDisplayTabRegion1);
  dEventDisplayTabRegion1->AddFrame(dEventDisplayTabRegion1_XY, dEventDisplayTabRegionLayout);
  dEventDisplayTabRegion1->AddFrame(dEventDisplayTabRegion1_XZ, dEventDisplayTabRegionLayout);
  // Region 2 tab
  dEventDisplayTabRegion2 = dEventDisplayTabs->AddTab("Region 2");
  dEventDisplayTabRegion2->SetLayoutManager(new TGHorizontalLayout(dEventDisplayTabRegion2));
  dEventDisplayTabRegion2_XY = new TRootEmbeddedCanvas("XY",dEventDisplayTabRegion2);
  dEventDisplayTabRegion2_XZ = new TRootEmbeddedCanvas("XZ",dEventDisplayTabRegion2);
  dEventDisplayTabRegion2->AddFrame(dEventDisplayTabRegion2_XY, dEventDisplayTabRegionLayout);
  dEventDisplayTabRegion2->AddFrame(dEventDisplayTabRegion2_XZ, dEventDisplayTabRegionLayout);
  // Region 3 tab
  dEventDisplayTabRegion3 = dEventDisplayTabs->AddTab("Region 3");
  dEventDisplayTabRegion3->SetLayoutManager(new TGHorizontalLayout(dEventDisplayTabRegion3));
  dEventDisplayTabRegion3_XY = new TRootEmbeddedCanvas("XY",dEventDisplayTabRegion3);
  dEventDisplayTabRegion3_XZ = new TRootEmbeddedCanvas("XZ",dEventDisplayTabRegion3);
  dEventDisplayTabRegion3->AddFrame(dEventDisplayTabRegion3_XY, dEventDisplayTabRegionLayout);
  dEventDisplayTabRegion3->AddFrame(dEventDisplayTabRegion3_XZ, dEventDisplayTabRegionLayout);
  // Default tab
  dEventDisplayTabs->SetTab(1);
}

void QwGUIEventDisplay::OnReceiveMessage(char *obj)
{

}

// void QwGUIEventDisplay::OnObjClose(char *obj)
// {
//   // if (!strcmp(obj,"dROOTFile")) {
//   //   dROOTCont = NULL;
//   // }
// }

// void QwGUIEventDisplay::OnNewDataContainer()
// {
//   ClearData();
//   /*
// //All tree access are cancelled
//   if (dROOTCont) {
//     fTree = (TTree*) dROOTCont->ReadTree("event_tree");
//   }
//   */
// }

// void QwGUIEventDisplay::OnRemoveThisTab()
// {

// }

// void QwGUIEventDisplay::ClearData()
// {

// }

void QwGUIEventDisplay::PlotPrevious()
{
  fCurrentEvent--;
  PlotData();
}

void QwGUIEventDisplay::PlotNext()
{
  fCurrentEvent++;
  PlotData();
}

void QwGUIEventDisplay::PlotData()
{
  std::cout << "Plotting data" << std::endl;
  //All tree access are cancelled
  //fTree->GetEntry(fCurrentEvent);
}

// void QwGUIEventDisplay::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
// {

// }


Bool_t QwGUIEventDisplay::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  return kTRUE;
}
