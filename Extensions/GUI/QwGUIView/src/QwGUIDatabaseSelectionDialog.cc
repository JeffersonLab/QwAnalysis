#include "QwGUIDatabaseSelectionDialog.h"

const char *const QwGUIDatabaseSelectionDialog::numlabel[5] = {
  "Database Server",
  "Database Name",
  "Username",
  "Password",
  "Port",
};

QwGUIDatabaseSelectionDialog::QwGUIDatabaseSelectionDialog(const TGWindow * p, const TGWindow * main,
							   const char* objname, const char *mainname,
							   DatabaseOptions *opts, UInt_t w, UInt_t h, 
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

  defaultVals[0] = dOptions->dbserver;
  defaultVals[1] = dOptions->dbname;
  defaultVals[2] = dOptions->uname;
  defaultVals[3] = dOptions->psswd;

  strcpy(dObjName,objname);
  strcpy(dMainName,mainname);

//   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);
  
  frame = new TGVerticalFrame(this,500, 50);
  fBttnframe = new TGHorizontalFrame(frame,500, 20);
  fHint = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2,  2, 2);
  fHint2 = new TGLayoutHints(kLHintsCenterX,2, 2, 3, 0);
  fHint3 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2,  2, 2);
  fHint4 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);

  fOk = new TGTextButton(fBttnframe, " &Set New ", 20);
  fOk->Associate(this);
  fBttnframe->AddFrame(fOk,fHint2);
  fAccept = new TGTextButton(fBttnframe, " &Accept Default ", 10);
  fAccept->Associate(this);
  fBttnframe->AddFrame(fAccept,fHint2);
  fCancel = new TGTextButton(fBttnframe, " &Cancel ", 30);
  fCancel->Associate(this);
  fBttnframe->AddFrame(fCancel, fHint2);

  for(int i = 0; i < 4; i++){

    fEntrframe[i] = new TGHorizontalFrame(frame,500, 30);

    fCB[i] = new TGCheckButton(fEntrframe[i], new TGHotString(numlabel[i]),i+21);
    fEntrframe[i]->AddFrame(fCB[i], fHint);
    fCB[i]->Associate(this);
    fCB[i]->SetState(kButtonUp);

    fFuncEntry[i] = new TGTextEntry(fEntrframe[i], fFuncBuffer[i] = new TGTextBuffer(200));
    fFuncEntry[i]->SetText(defaultVals[i].Data());
    //fFuncEntry[i]->Associate(this);
    fEntrframe[i]->AddFrame(fFuncEntry[i],new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 2,  2, 2));
    fFuncEntry[i]->SetState(0);
    frame->AddFrame(fEntrframe[i],fHint3);
    fFuncEntry[i]->Resize(200, fFuncEntry[i]->GetDefaultHeight());
  }

  fEntrframe[4] = new TGHorizontalFrame(frame,500, 30);
  fCB[4] = new TGCheckButton(fEntrframe[4], new TGHotString(numlabel[4]),25);
  fEntrframe[4]->AddFrame(fCB[4], fHint);
  fCB[4]->Associate(this);
  fCB[4]->SetState(kButtonUp);

  fNumEntry = new TGNumberEntry(fEntrframe[4],1.0,8,30,TGNumberFormat::kNESInteger); 
  fNumEntry->SetNumber(dOptions->dbport);
  fNumEntry->SetState(0);
  fNumEntry->Associate(this);
  fEntrframe[4]->AddFrame(fNumEntry,fHint);
  frame->AddFrame(fEntrframe[4],fHint3);

  frame->AddFrame(fBttnframe,fHint3);
  AddFrame(frame, fHint4);

  
  SetWindowName("Database Selection");
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

  Connect("IsClosing(char*)",dMainName,(void*)main,"OnObjClose(char*)");
  fClient->WaitFor(this);
}

void QwGUIDatabaseSelectionDialog::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

QwGUIDatabaseSelectionDialog::~QwGUIDatabaseSelectionDialog()
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

void QwGUIDatabaseSelectionDialog::CloseWindow()
{
  IsClosing(dObjName);
  DeleteWindow();
}


void QwGUIDatabaseSelectionDialog::SetOptions()
{
  if (fCB[0]->GetState() == kButtonDown)
    dOptions->dbserver = fFuncEntry[0]->GetText();
  if (fCB[1]->GetState() == kButtonDown)
    dOptions->dbname = fFuncEntry[1]->GetText();
  if (fCB[2]->GetState() == kButtonDown)
    dOptions->uname = fFuncEntry[2]->GetText();
  if (fCB[3]->GetState() == kButtonDown)
    dOptions->psswd = fFuncEntry[3]->GetText();

  if (fCB[4]->GetState() == kButtonDown)
    dOptions->dbport = (int)fNumEntry->GetNumber();
  
  *dChangeFlag = kTrue;
}

Bool_t QwGUIDatabaseSelectionDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
	      CloseWindow();
	      break;
	    }
	  case 10:
	    {
	      CloseWindow();
	      break;
	    }
	  }
	  break;
	}
	break;
      case kCM_CHECKBUTTON:
	{
	  switch(parm1) {
	  case 21:
	    {
	      if (fCB[0]->GetState() == kButtonDown){
		fFuncEntry[0]->SetState(1);
	      }
	      else{
		fFuncEntry[0]->SetState(0);
	      }		
	    }
	    break;
	  case 22:
	    {
	      if (fCB[1]->GetState() == kButtonDown){
		fFuncEntry[1]->SetState(1);
	      }
	      else{
		fFuncEntry[1]->SetState(0);
	      }		
	    }
	    break;
	  case 23:
	    {
	      if (fCB[2]->GetState() == kButtonDown){
		fFuncEntry[2]->SetState(1);
	      }
	      else{
		fFuncEntry[2]->SetState(0);
	      }		
	    }
	    break;
	  case 24:
	    {
	      if (fCB[3]->GetState() == kButtonDown){
		fFuncEntry[3]->SetState(1);
	      }
	      else{
		fFuncEntry[3]->SetState(0);
	      }		
	    }
	    break;
	  case 25:
	    {
	      if (fCB[4]->GetState() == kButtonDown){
		fNumEntry->SetState(1);
	      }
	      else{
		fNumEntry->SetState(0);
	      }		
	    }
	    break;

	  }
	}
	break;
      }
    }
    break;
  }
  return kTRUE;
}
