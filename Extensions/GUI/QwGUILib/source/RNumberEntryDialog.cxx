/////////////////////////////////////////////////////////////////////////////////////
//NPDGamma Experiment Data Visualization and Analysis
//
//Version 2.01
//
//RNumberEntryDialog source file, containing methods to monitor analyzing
//progress
/////////////////////////////////////////////////////////////////////////////////////


#include "RNumberEntryDialog.h"



RNumberEntryDialog::RNumberEntryDialog(const TGWindow *p, const TGWindow *main,
				       const char* objname, const char *ownername, 
				       const char *title,
				       Double_t* number, Int_t *retval, UInt_t w, 
				       UInt_t h) :
  TGTransientFrame(p, main, w, h)
{
  dNumber = number;
  dRetVal = retval;

  strcpy(dObjName,objname);
  strcpy(dOwnerName,ownername);
  strcpy(dTitle,title);
  
  ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);
  
  frame = new TGVerticalFrame(this,500, 50);
  fBttnframe = new TGHorizontalFrame(frame,500, 20);
  fEntrframe = new TGHorizontalFrame(frame,500, 30);
  fHint = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2,  2, 2);
  fHint2 = new TGLayoutHints(kLHintsCenterX,2, 2, 3, 0);
  fHint3 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2,  2, 2);
  fHint4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);


  fOk = new TGTextButton(fBttnframe, " &Ok ", 10);
  fOk->Associate(this);
  fBttnframe->AddFrame(fOk,fHint2);
  fCancel = new TGTextButton(fBttnframe, " &Cancel ", 20);
  fCancel->Associate(this);
  fBttnframe->AddFrame(fCancel, fHint2);

  fNumEntry = new TGNumberEntry(fEntrframe,1.0,8,30,TGNumberFormat::kNESReal); 
  fNumEntry->Associate(this);
  fEntrframe->AddFrame(fNumEntry,fHint);
  frame->AddFrame(fEntrframe,fHint3);
  frame->AddFrame(fBttnframe,fHint3);
  AddFrame(frame, fHint4);
  
  SetWindowName(dTitle);
  TGDimension size = GetDefaultSize();
  Resize(size);
  
  // position relative to the parent's window
  Window_t wdummy;
  int ax, ay;
  gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
			     (Int_t)(((TGFrame *) main)->GetWidth() - fWidth) >> 1,
			     (Int_t)(((TGFrame *) main)->GetHeight() - fHeight) >> 1,
				  ax, ay, wdummy);
  Move(ax, ay);
  
  MapSubwindows();
  MapWindow();

  fClient->WaitFor(this);
}

RNumberEntryDialog::~RNumberEntryDialog()
{
  delete frame;
  delete fBttnframe;
  delete fEntrframe;
  delete fHint; 
  delete fHint2;
  delete fHint3;
  delete fHint4;
  delete fNumEntry;
}

void RNumberEntryDialog::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

void RNumberEntryDialog::CloseWindow()
{
  IsClosing(dObjName);
 
  DeleteWindow();
}

Bool_t RNumberEntryDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {

    case kCM_BUTTON:
      {
	switch (parm1) {
	case 10:
	  *dNumber = fNumEntry->GetNumber();
	  *dRetVal = 1;
	  CloseWindow();
	  break;
	case 20:
	  *dRetVal = 0;
	  CloseWindow();
	  break;
	}
      }
      break;
    }
    break;
  }
  return kTRUE;
}
