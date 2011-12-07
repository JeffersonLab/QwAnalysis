#include "QwGUIHCLogEntryDialog.h"

// const char *const QwGUIHCLogEntryDialog::numlabel[3] = {
//   "Name:",
//   "Comment:",
//   "Email to:",
// };

// const Double_t QwGUIHCLogEntryDialog::numinit[6] = {
//   -600, 600, 50,0,0,1,
// };

QwGUIHCLogEntryDialog::QwGUIHCLogEntryDialog(const TGWindow * p, const TGWindow * main,
				       const char* objname, const char *mainname,
				       HCLogEntry *entryData, 
				       UInt_t w, UInt_t h, UInt_t options)
  : TGTransientFrame(p, main, w, h, options)
{
  dHCEntries = entryData;
  dFlag = &dHCEntries->setFlag;
  strcpy(dObjName,objname);
  strcpy(dMainName,mainname);
  
  dMainFrame    = new TGVerticalFrame(this, 60, 20);
  dEntryFrame   = new TGVerticalFrame(dMainFrame, 60, 20);
  dSetFrame     = new TGHorizontalFrame(dMainFrame,60, 20);

  
  dNameFrame = new TGHorizontalFrame(dEntryFrame,60, 20);
  NameLabel = new TGLabel(dNameFrame,"Name:");
  dNameFrame->AddFrame(NameLabel,new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));

  dNameEntry = new TGTextEntry(dNameFrame, dNameBuffer = new TGTextBuffer(300));
  dNameFrame->AddFrame(dNameEntry,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2));
  dNameEntry->SetState(1);
  dEntryFrame->AddFrame(dNameFrame,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2));

  dSubjectFrame = new TGHorizontalFrame(dEntryFrame,60, 20);
  SubjectLabel = new TGLabel(dSubjectFrame,"Subject:");
  dSubjectFrame->AddFrame(SubjectLabel,new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));

  dSubjectEntry = new TGTextEntry(dSubjectFrame, dSubjectBuffer = new TGTextBuffer(300));
  dSubjectFrame->AddFrame(dSubjectEntry,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2));
  dSubjectEntry->SetState(1);
  dEntryFrame->AddFrame(dSubjectFrame,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2));

  dCommentFrame = new TGVerticalFrame(dEntryFrame,60, 20);
  CommentLabel = new TGLabel(dCommentFrame,"Post Comments (right click to open file):");
  dCommentFrame->AddFrame(CommentLabel,new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));

  dCommentEntry = new TGTextEdit(dCommentFrame,10,10,5555,kSunkenFrame);
  dCommentFrame->AddFrame(dCommentEntry,new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
  dCommentEntry->Associate(this);
  dCommentEntry->Resize(300,100);
  dEntryFrame->AddFrame(dCommentFrame,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

  dEmailFrame = new TGHorizontalFrame(dEntryFrame,60, 20);
  EmailLabel = new TGLabel(dEmailFrame,"Email post to:");
  dEmailFrame->AddFrame(EmailLabel,new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));

  dEmailEntry = new TGTextEntry(dEmailFrame, dEmailBuffer = new TGTextBuffer(300));
  dEmailFrame->AddFrame(dEmailEntry,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2));
  dEmailEntry->SetState(1);
  dEntryFrame->AddFrame(dEmailFrame,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 2, 2, 2, 2));


  dMainFrame->AddFrame(dEntryFrame,new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
  
  dSetButton = new TGTextButton(dSetFrame, " Set ", M_HISTO_ENTRY_SET);
  dSetButton->Associate(this);
  dSetFrame->AddFrame(dSetButton, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
  dExitButton = new TGTextButton(dSetFrame, " Cancel ", M_HISTO_ENTRY_CANCEL);
  dExitButton->Associate(this);
  dSetFrame->AddFrame(dExitButton, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));

  dMainFrame->AddFrame(dSetFrame,new TGLayoutHints(kLHintsBottom | kLHintsExpandX , 2, 2, 2, 2));
  AddFrame(dMainFrame,new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

  dNameEntry->Resize(200, dNameEntry->GetDefaultHeight());
  dSubjectEntry->Resize(200, dSubjectEntry->GetDefaultHeight());
  dCommentEntry->Resize(200, dCommentEntry->GetDefaultHeight());
  dEmailEntry->Resize(200, dEmailEntry->GetDefaultHeight());
  
  // set dialog box title
  SetWindowName("HC Log Options");
  SetIconName("HC Log Options");
  // TGDimension size = GetDefaultSize();
  // Resize(size);

  // SetClassHints("QwGUIHCLogEntryDialog", "QwGUIHCLogEntryDialog");
  // resize & move to center
  MapSubwindows();
  UInt_t width = GetDefaultWidth();
  UInt_t height = GetDefaultHeight();
  Resize(width, height);
  Int_t ax;
  Int_t ay;
  Window_t wdum;
  // gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
  // 				  (Int_t)(((TGFrame *) main)->GetWidth() -
  // 					  fWidth) >> 1,
  // 				  (Int_t)(((TGFrame *) main)->GetHeight() -
  // 					  fHeight) >> 1, ax, ay, wdum);
  // Move(ax, ay);
  // SetWMPosition(ax, ay);
  // make the message box non-resizable
  // SetWMSize(width, height);
  // SetWMSizeHints(width, height, width, height, 0, 0);
  // SetMWMHints(kMWMDecorAll | kMWMDecorResizeH | kMWMDecorMaximize |
  // 	      kMWMDecorMinimize | kMWMDecorMenu,
  // 	      kMWMFuncAll | kMWMFuncResize | kMWMFuncMaximize |
  // 	      kMWMFuncMinimize, kMWMInputModeless);
    
  // MapSubwindows();
  MapWindow();

  Connect("IsClosing(char*)",dMainName,(void*)main,"OnObjClose(char*)");
  CenterOnParent();
  fClient->WaitFor(this);
}

void QwGUIHCLogEntryDialog::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

QwGUIHCLogEntryDialog::~QwGUIHCLogEntryDialog()
{
  if(dNameEntry)    delete dNameEntry;     dNameEntry = NULL;    
  if(dSubjectEntry) delete dSubjectEntry;  dSubjectEntry = NULL;    
  if(dCommentEntry) delete dCommentEntry;  dCommentEntry = NULL; 
  if(dEmailEntry)   delete dEmailEntry;    dEmailEntry = NULL;   

  if(dNameFrame)    delete dNameFrame;     dNameFrame = NULL;
  if(dSubjectFrame) delete dSubjectFrame;  dSubjectFrame = NULL;
  if(dCommentFrame) delete dCommentFrame;  dCommentFrame = NULL;
  if(dEmailFrame)   delete dEmailFrame;    dEmailFrame = NULL;

  if(NameLabel)    delete NameLabel;     NameLabel = NULL;
  if(SubjectLabel) delete SubjectLabel;  SubjectLabel = NULL;
  if(CommentLabel) delete CommentLabel;  CommentLabel = NULL;
  if(EmailLabel)   delete EmailLabel;    EmailLabel = NULL;

  if(dMainFrame) delete dMainFrame;     dMainFrame = NULL; 
  if(dEntryFrame)delete dEntryFrame;	dEntryFrame = NULL;
  if(dSetFrame)  delete dSetFrame;  	dSetFrame = NULL;  
  if(dSetButton) delete dSetButton; 	dSetButton = NULL; 
  if(dExitButton)delete dExitButton;	dExitButton = NULL;

}

void QwGUIHCLogEntryDialog::CloseWindow()
{
  IsClosing(dObjName);
  DeleteWindow();
}


void QwGUIHCLogEntryDialog::SetEntries()
{
  dHCEntries->name = dNameBuffer->GetString();
  dHCEntries->subject = dSubjectBuffer->GetString();
  dHCEntries->emaillist = dEmailBuffer->GetString();
  dHCEntries->comments = dCommentEntry->GetText()->AsString();
  *dFlag = kTrue;
}


void QwGUIHCLogEntryDialog::FChangeFocus()
{    
  if (gTQSender == dNameEntry)
    dCommentEntry->SetFocus();
  
  if (gTQSender == dCommentEntry)
    dEmailEntry->SetFocus();
  
  if (gTQSender == dEmailEntry)
    dNameEntry->SetFocus();
}



Bool_t QwGUIHCLogEntryDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    {
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
	{
	  switch (parm1) {
	    // exit button
	  case M_HISTO_ENTRY_CANCEL:
	    {
	      *dFlag = kFalse;
	      CloseWindow();
	      break;
	    }
	    // set button
	  case M_HISTO_ENTRY_SET:
	    {
	      SetEntries();
	      CloseWindow();
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
