#include "QwGUIFFTWindowSelectionDialog.h"


QwGUIFFTWindowSelectionDialog::QwGUIFFTWindowSelectionDialog(const TGWindow * p, const TGWindow * main,
							   const char* objname, const char *mainname,
							   FFTOptions *opts, UInt_t w, UInt_t h, 
							   UInt_t options)
  : TGTransientFrame(p, main, w, h, options)
{

  char temp[50];
  memset(temp,'\0',sizeof(temp));
  dOptions = opts;
  dChangeFlag = &dOptions->changeFlag;
  dCancelFlag = &dOptions->cancelFlag;
  *dChangeFlag = kFalse;
  *dCancelFlag = kFalse;
  dMain = main;

  defaultVals[0] = dOptions->Start;
  defaultVals[1] = dOptions->Length;
  defaultVals[2] = dOptions->TotalLength;

  strcpy(dObjName,objname);
  strcpy(dMainName,mainname);

//   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);
  
  frame = new TGVerticalFrame(this,500, 50);
  fBttnframe = new TGHorizontalFrame(frame,500, 20);
  fHint = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2,  2, 2);
  fHint2 = new TGLayoutHints(kLHintsCenterX,2, 2, 3, 0);
  fHint3 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2,  2, 2);
  fHint4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);

  fOk = new TGTextButton(fBttnframe, " &Set ", 20);
  fOk->Associate(this);
  fBttnframe->AddFrame(fOk,fHint2);
  fAccept = new TGTextButton(fBttnframe, " Use &All ", 10);
  fAccept->Associate(this);
  fBttnframe->AddFrame(fAccept,fHint2);
  fCancel = new TGTextButton(fBttnframe, " &Cancel ", 30);
  fCancel->Associate(this);
  fBttnframe->AddFrame(fCancel, fHint2);

  fEntrframe1 = new TGHorizontalFrame(frame,500, 30);

  fLabel1 = new TGLabel(fEntrframe1,"Start Event");
  fEntrframe1->AddFrame(fLabel1,fHint);

  fNumEntry1 = new TGNumberEntry(fEntrframe1,1.0,8,30,TGNumberFormat::kNESInteger); 
  fNumEntry1->SetNumber(dOptions->Start);
  fNumEntry1->SetState(1);
  fNumEntry1->Associate(this);

  fEntrframe1->AddFrame(fNumEntry1,new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 2,  2, 2));
  frame->AddFrame(fEntrframe1,fHint3);

  fEntrframe2 = new TGHorizontalFrame(frame,500, 30);

  fLabel2 = new TGLabel(fEntrframe2,Form("Events to Process (<= %d)",dOptions->TotalLength));
  fEntrframe2->AddFrame(fLabel2,fHint);

  fNumEntry2 = new TGNumberEntry(fEntrframe2,1.0,8,30,TGNumberFormat::kNESInteger); 
  fNumEntry2->SetNumber(dOptions->Length);
  fNumEntry2->SetState(1);
  fNumEntry2->Associate(this);

  fEntrframe2->AddFrame(fNumEntry2,new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 2,  2, 2));
  frame->AddFrame(fEntrframe2,fHint3);

  frame->AddFrame(fBttnframe,fHint3);
  AddFrame(frame, fHint4);

  
  SetWindowName("FFT Window Selection");
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

  SetCleanup(kDeepCleanup); 

//   Connect("IsClosing(char*)",dMainName,(void*)main,"OnObjClose(char*)");
  fClient->WaitFor(this);
}

void QwGUIFFTWindowSelectionDialog::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

QwGUIFFTWindowSelectionDialog::~QwGUIFFTWindowSelectionDialog()
{

//   for (int i = 0; i < 4; i++) {
//     delete fFuncEntry[i];
//     delete fFuncBuffer[i];
//   }
//   for (int i = 0; i < 5; i++) {    
//     delete fCB[i];
//     delete fEntrframe[i];
//     delete fLabel;
//   }


//   delete fCancel; 
//   delete fOk;
//   //  delete fAccept;  
//   delete frame;
//   delete fBttnframe;
//   delete fNumEntry; 
//   delete fHint;
//   delete fHint2;
//   delete fHint3;
//   delete fHint4;

}

void QwGUIFFTWindowSelectionDialog::CloseWindow()
{
//   IsClosing(dObjName);
  DeleteWindow();
}


void QwGUIFFTWindowSelectionDialog::SetOptions(Bool_t all)
{
  Int_t retval= 0;
  char buffer[200];
  memset(buffer,'\0',sizeof(buffer));

  if(fNumEntry1->GetNumber() >= dOptions->TotalLength ||
     fNumEntry2->GetNumber() > dOptions->TotalLength ||
     fNumEntry1->GetNumber() + fNumEntry2->GetNumber() > dOptions->TotalLength ||
     fNumEntry1->GetNumber() < 0 ||
     fNumEntry2->GetNumber() < 0){
    
    strcpy(buffer,"Entry not valid!");
    new TGMsgBox(fClient->GetRoot(), dMain,"File Open Operation",
		 buffer,kMBIconStop, kMBOk, &retval);
    if(retval == kMBOk){ 
      return;
    }
  }


  if(!all){
    
    if(fNumEntry1->GetNumber() != dOptions->Start){
      dOptions->Start  = fNumEntry1->GetNumber();
      *dChangeFlag = kTrue;
    }
    
    if(fNumEntry2->GetNumber() != dOptions->Length){
      dOptions->Length  = fNumEntry2->GetNumber();
      *dChangeFlag = kTrue;
    }
  }
  else{
    dOptions->Start  = 0;
    dOptions->Length = dOptions->TotalLength;    
    *dChangeFlag = kTrue;
  }  

  if(*dChangeFlag && dOptions->Length > 50000){

    sprintf(buffer,
	    "You selected %d events to process!\nThis may take a while!\nAre you sure you want to continue?",
	    dOptions->Length);
    
    new TGMsgBox(fClient->GetRoot(), dMain,"File Open Operation",
		 buffer,kMBIconQuestion, kMBOk | kMBCancel, &retval);
    if(retval == kMBCancel){ 
      return;
    }
  }    
  CloseWindow();
}

Bool_t QwGUIFFTWindowSelectionDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    {
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
	{
	  switch (parm1) {
	    // cancel button
	  case 30:
	    {
	      *dCancelFlag = kTrue;
	      CloseWindow();
	      break;
	    }
	    // set new button
	  case 20:
	    {
	      SetOptions();
	      break;
	    }
	  case 10:
	    {
	      SetOptions(kTrue);
	      break;
	    }
	  }
	  break;
	}
	break;
      }
    }
    break;
  }
  return kTRUE;
}
