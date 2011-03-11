#include "QwGUIProgressDialog.h"



QwGUIProgressDialog::QwGUIProgressDialog(const TGWindow *p, const TGWindow *main,
					 const char* objname, const char *mainname,
					 const char* title, const char* macrotext, const char* microtext,
					 const char* microtext2, int runcnt, int mpcnt, int mpcnt2,
					 UInt_t w,UInt_t h, Bool_t stopable, Bool_t kReverse, Int_t num) :
    TGTransientFrame(p, main, w, h)
{
  if(num > 3) num = 3;
  kStopable = stopable;
  dContinue = kTrue;
  dReverse = kReverse;
  dStop = kFalse;
  dRuncnt = runcnt;
  dMpcnt = mpcnt;
  dMpcnt2 = mpcnt2;
  dNum = num;

  strcpy(dObjName,objname);
  strcpy(dMainName,mainname);
  
  fHProg1 = NULL;
  fHProg2 = NULL;
  fHProg3 = NULL;
 
  // Dialog used to test the different supported progress bars.
  
  ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);
  
  fVframe1 = new TGVerticalFrame(this, 0, 0, 0);
  fHframe1 = new TGHorizontalFrame(this,60, 20, kFixedWidth);

  const char *mactext  = macrotext; //{" "};
  const char *mictext  = microtext; //{" "}; 
  const char *mictext2 = microtext2;//{" "};

//   if(macrotext  && macrotext[0])  mactext  = macrotext;
//   if(microtext  && microtext[0])  mictext  = microtext;
//   if(microtext2 && microtext2[0]) mictext2 = microtext2;

  if(dNum > 0){
    fHProg1 = new TGHProgressBar(fVframe1, 300);
    fHProg1->SetBarColor("lightblue");
    fHProg1->SetRange(0,dRuncnt);
    fHProg1->SetPosition(0);
    if(dReverse)
      sprintf(dTemp," %s %%.0f",mactext);
    else
      sprintf(dTemp,"%%.0f %s",mactext);
    fHProg1->ShowPosition(kTRUE, kFALSE, dTemp);
  }
  if(dNum > 1 && microtext){
    fHProg2 = new TGHProgressBar(fVframe1, TGProgressBar::kFancy, 300);
    fHProg2->SetBarColor("lightblue");
    fHProg2->SetRange(0,dMpcnt);
    if(dReverse)
      sprintf(dTemp," %s %%.0f",mictext);
    else
      sprintf(dTemp,"%%.0f %s",mictext);
      
    fHProg2->ShowPosition(kTRUE, kFALSE, dTemp);
    fHProg2->SetPosition(0);
  }
  if(dNum > 2 && microtext2){
    fHProg3 = new TGHProgressBar(fVframe1, TGProgressBar::kFancy, 300);
    fHProg3->SetBarColor("lightblue");
    fHProg3->SetRange(0,dMpcnt2);
    if(dReverse)
      sprintf(dTemp,"%s %%.0f",mictext2);
    else
      sprintf(dTemp,"%%.0f %s",mictext2);
    fHProg3->ShowPosition(kTRUE, kFALSE, dTemp);
    fHProg3->SetPosition(0);
  }


  fStop = new TGTextButton(fHframe1, "Stop", M_PR_STOP);
  fStop->Associate(this);

  fResume = new TGTextButton(fHframe1, "Resume", M_PR_RESUME);
  fResume->Associate(this);

  fPause = new TGTextButton(fHframe1, "Pause", M_PR_PAUSE);
  fPause->Associate(this);
  
  fVframe1->Resize(300, 300);
  
  fHint2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 
			     5, 5,  5, 10);
  fHint3 = new TGLayoutHints(kLHintsLeft  | kLHintsCenterX, 1, 1, 1, 1);
  fHint4 = new TGLayoutHints(kLHintsBottom | 
			     kLHintsExpandX | 
			     kLHintsRight, 0, 0, 0, 0);
  fHint5 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0);
  
  if(dNum > 0) fVframe1->AddFrame(fHProg1, fHint2);
  if(dNum > 1) fVframe1->AddFrame(fHProg2, fHint2);
  if(dNum > 2) fVframe1->AddFrame(fHProg3, fHint2);
  fHframe1->AddFrame(fStop, fHint3);
  fHframe1->AddFrame(fPause, fHint3);
  fHframe1->AddFrame(fResume, fHint3);
  fVframe1->AddFrame(fHframe1,fHint4);

  AddFrame(fVframe1, fHint5);
  
  SetWindowName(title);
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

  fHframe1->HideFrame(fStop);

  Connect("IsClosing(char*)",dMainName,(void*)main,"OnObjClose(char*)");
//   Connect("HaltProcess()",dMainName,(void*)main,"OnHaltProcess()");
  
  //fClient->WaitFor(this);

}

QwGUIProgressDialog::~QwGUIProgressDialog()
{
  delete fVframe1;
  if(fHProg1) delete fHProg1; 
  if(fHProg2) delete fHProg2; 
  if(fHProg3) delete fHProg3;
  delete fHint2; delete fHint5;delete fHint3;
  delete fStop;
  delete fPause;
  delete fResume;
}

void QwGUIProgressDialog::SetPosition(int p1, int p2, int p3)
{
  if(p1 > 0 && p1 <= dRuncnt && dNum > 0){
    fHProg1->SetPosition(p1);
    gSystem->ProcessEvents();
  }
  if(p2 > 0 && p2 <= dMpcnt && dNum > 1){
    fHProg2->SetPosition(p2);
    gSystem->ProcessEvents();
  }
  if(p3 > 0 && p3 <= dMpcnt2 && dNum > 2){
    fHProg2->SetPosition(p3);
    gSystem->ProcessEvents();
  }  
}

void QwGUIProgressDialog::ResetRange(int runcnt, int mpcnt, int mpcnt2)
{
  if(runcnt > 0 && dNum > 0){
    double pos = fHProg1->GetPosition();
    fHProg1->Reset();
    fHProg1->SetRange(0,runcnt);
    fHProg1->SetPosition(pos);
    dRuncnt = runcnt;
    gSystem->ProcessEvents();
  }
  if(mpcnt > 0 && dNum > 1){
    fHProg2->Reset();
    fHProg2->SetRange(0,mpcnt);
    fHProg2->SetPosition(0);
    dMpcnt = mpcnt;
    gSystem->ProcessEvents();
  }
  if(mpcnt2 > 0 && dNum > 2){
    fHProg3->Reset();
    fHProg3->SetRange(0,mpcnt2);
    fHProg3->SetPosition(0);
    dMpcnt2 = mpcnt2;
    gSystem->ProcessEvents();
  }
}

void QwGUIProgressDialog::SetTitle(char *title)
{
  SetWindowName(title);
}

void QwGUIProgressDialog::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

void QwGUIProgressDialog::HaltProcess()
{
  Emit("HaltProcess()");
}

void QwGUIProgressDialog::CloseWindow()
{
  // Called when window is closed via the window manager.

  IsClosing(dObjName); 
  DeleteWindow();
}


Bool_t QwGUIProgressDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  // Process slider messages.
  
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:
      switch (parm1){
      case M_PR_STOP:
	//dStop = kTrue;
	dContinue = kTrue;
	CloseWindow();
	break;
      case M_PR_PAUSE:
	dContinue = kFalse;
	if(kStopable)
	  fHframe1->ShowFrame(fStop);
	while(!Continue()){gSystem->ProcessEvents();};
	break;
      case M_PR_RESUME:
	fHframe1->HideFrame(fStop);
	dContinue = kTrue;
	break;
      }
      break;
    }
    break;

  case kC_PR_DIALOG:
    switch (GET_SUBMSG(msg)) {
    case kCM_PR_MSG:
      switch (parm1) {
      
      case M_PR_RUN:
	{
// 	  if(fHProg1->GetPosition() < dRuncnt-1){
	    if(fHProg2) fHProg2->Reset();
	    if(fHProg3) fHProg3->Reset();
// 	  }
	  fHProg1->Increment(parm2);
	  //gSystem->ProcessEvents();
	}
	break;

      case M_PR_SEQ:
	{
// 	  if(fHProg2->GetPosition() < dMpcnt-1)
	    if(fHProg3) fHProg3->Reset();
	  fHProg2->Increment(parm2);
	  //gSystem->ProcessEvents();
	}
	break;
	
      case M_PR_SEQ2:
	{
	  fHProg3->Increment(parm2);
	  //gSystem->ProcessEvents();
	}
	break;

      default:
	break;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return kTRUE;
}
