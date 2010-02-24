#include "QwGUIPlotOptionsDialog.h"

const char *const QwGUIPlotOptionsDialog::numlabel[8] = {
  "Average over events",
  "Integrate over events",
  "Pedestal Subtract",
  "Background Subtract",
  "Normalize to Beam",
  "Detector Number",
  "Start at event",
  "Read # of events",
};

const Double_t QwGUIPlotOptionsDialog::numinit[6] = {
  -600, 600, 50,0,0,1,
};

QwGUIPlotOptionsDialog::QwGUIPlotOptionsDialog(const TGWindow * p, const TGWindow * main,
				       const char* objname, const char *mainname,
				       DataOptions *opts, 
				       UInt_t w, UInt_t h, UInt_t options)
  : TGTransientFrame(p, main, w, h, options)
{

  char temp[50];
  memset(temp,'\0',sizeof(temp));
  dOptions = opts;
  dFlag = &dOptions->limitsFlag;
  strcpy(dObjName,objname);
  strcpy(dMainName,mainname);
  
  TGNumberFormat::EStyle style[3] = {TGNumberFormat::kNESInteger,
				     TGNumberFormat::kNESInteger,
				     TGNumberFormat::kNESInteger};

  eval[0] = dOptions->channel;
  eval[1] = dOptions->startev; 
  eval[2] = dOptions->numevs;

  dMainFrame    = new TGCompositeFrame(this, 60, 20, kVerticalFrame);
  dEnterFrame   = new TGCompositeFrame(dMainFrame, 60, 20, kHorizontalFrame);
  dCheckFrame   = new TGGroupFrame(dEnterFrame, new TGString("Options"));
  dRangeFrame   = new TGGroupFrame(dEnterFrame, new TGString("Range"));
  dSetFrame     = new TGCompositeFrame(dMainFrame,60, 20, kHorizontalFrame);

  dMainLayout   = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2);
  dEnterLayout  = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2);
  dCheckLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
  dRangeLayout  = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
  dSetLayout    = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2);
  dButtonLayout = new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2);
  dNumEntLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 2, 2);

  for(int i = 0; i < 5; i++){
    dCB[i] = new TGCheckButton(dCheckFrame, new TGHotString(numlabel[i]),i+20);
    dCheckFrame->AddFrame(dCB[i], dCheckLayout);
    dCB[i]->Associate(this);
    dCB[i]->SetState(kButtonUp);
  }

  dEnterFrame->AddFrame(dCheckFrame,dCheckLayout);

  for(int j = 0; j < 3; j++){
    dNumEntFrame[j] = new TGHorizontalFrame(dRangeFrame, 200, 30);
    dRangeFrame->AddFrame(dNumEntFrame[j], dNumEntLayout);
    dNumericEntries[j] = new TGNumberEntry(dNumEntFrame[j],eval[j],
					   12,j+23,style[j]);
    dNumericEntries[j]->GetNumberEntry()->Connect("TabPressed()", 
						  "QwGUIPlotOptionsDialog", 
						  this, "FChangeFocus()");
    dNumericEntries[j]->Associate(this);
    dNumEntFrame[j]->AddFrame(dNumericEntries[j], dNumEntLayout);
    dNumEntLabel[j] = new TGLabel(dNumEntFrame[j], numlabel[j+5]);
    dNumEntFrame[j]->AddFrame(dNumEntLabel[j], dNumEntLayout);      
  }

  dEnterFrame->AddFrame(dRangeFrame,dRangeLayout);
  dMainFrame->AddFrame(dEnterFrame,dEnterLayout);
  
  dSetButton = new TGTextButton(dSetFrame, " Set ", M_HISTO_ENTRY_SET);
  dSetButton->Associate(this);
  dSetFrame->AddFrame(dSetButton, dButtonLayout);
  dExitButton = new TGTextButton(dSetFrame, " Cancel ", M_HISTO_ENTRY_CANCEL);
  dExitButton->Associate(this);
  dSetFrame->AddFrame(dExitButton, dButtonLayout);

  dMainFrame->AddFrame(dSetFrame,dSetLayout);
  AddFrame(dMainFrame,dMainLayout);
  
  // set dialog box title
  SetWindowName("Plot Options");
  SetIconName("Plot Options");
  SetClassHints("QwGUIPlotOptionsDialog", "QwGUIPlotOptionsDialog");
  // resize & move to center
  MapSubwindows();
  UInt_t width = GetDefaultWidth();
  UInt_t height = GetDefaultHeight();
  Resize(width, height);
  Int_t ax;
  Int_t ay;
  if (main) {
    Window_t wdum;
    gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
				    (Int_t)(((TGFrame *) main)->GetWidth() -
					    fWidth) >> 1,
				    (Int_t)(((TGFrame *) main)->GetHeight() -
					    fHeight) >> 1, ax, ay, wdum);
  } else {
    UInt_t root_w, root_h;
    gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay,
			     root_w, root_h);
    ax = (Int_t)(root_w - fWidth) >> 1;
    ay = (Int_t)(root_h - fHeight) >> 1;
  }
  Move(ax, ay);
  SetWMPosition(ax, ay);
  // make the message box non-resizable
  SetWMSize(width, height);
  SetWMSizeHints(width, height, width, height, 0, 0);
  SetMWMHints(kMWMDecorAll | kMWMDecorResizeH | kMWMDecorMaximize |
	      kMWMDecorMinimize | kMWMDecorMenu,
	      kMWMFuncAll | kMWMFuncResize | kMWMFuncMaximize |
	      kMWMFuncMinimize, kMWMInputModeless);
    
  MapWindow();

  Connect("IsClosing(char*)",dMainName,(void*)main,"OnObjClose(char*)");
  fClient->WaitFor(this);
}

void QwGUIPlotOptionsDialog::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

QwGUIPlotOptionsDialog::~QwGUIPlotOptionsDialog()
{
  for (int i = 0; i < 3; i++) {
    delete dNumericEntries[i];
    delete dNumEntLabel[i];
    delete dNumEntFrame[i];
  }
  for (int i = 0; i < 5; i++)     
    delete dCB[i];


  delete dMainFrame; 
  delete dEnterFrame;
  delete dSetFrame;  
  delete dCheckFrame;
  delete dRangeFrame;
  delete dSetButton; 
  delete dExitButton;
  delete dMainLayout;  
  delete dEnterLayout;
  delete dSetLayout;  
  delete dCheckLayout;
  delete dRangeLayout;
  delete dButtonLayout;
  delete dNumEntLayout;

}

void QwGUIPlotOptionsDialog::CloseWindow()
{
  IsClosing(dObjName);
  DeleteWindow();
}


void QwGUIPlotOptionsDialog::SetLimits()
{
  if (dCB[0]->GetState() == kButtonDown)
    dOptions->mpAverage = kTrue;
  else
    dOptions->mpAverage = kFalse;
  if (dCB[1]->GetState() == kButtonDown)
    dOptions->mpIntegrate = kTrue;
  else
    dOptions->mpIntegrate = kFalse;
  if (dCB[2]->GetState() == kButtonDown)
    dOptions->mpPedSubtract = kTrue;
  else
    dOptions->mpPedSubtract = kFalse;
  if (dCB[3]->GetState() == kButtonDown)
    dOptions->mpBgrdSubtract = kTrue;
  else
    dOptions->mpBgrdSubtract = kFalse;
  if (dCB[4]->GetState() == kButtonDown)
    dOptions->mpNormToBeam = kTrue;
  else
    dOptions->mpNormToBeam = kFalse;

  dOptions->channel    = (int)dNumericEntries[0]->GetNumber();
  dOptions->startev    = (int)dNumericEntries[1]->GetNumber();
  dOptions->numevs     = (int)dNumericEntries[2]->GetNumber();
  
  *dFlag = kTrue;
}


void QwGUIPlotOptionsDialog::FChangeFocus()
{    
  if (gTQSender == dNumericEntries[0]->GetNumberEntry())
    dNumericEntries[1]->GetNumberEntry()->SetFocus();
  
  if (gTQSender == dNumericEntries[1]->GetNumberEntry())
    dNumericEntries[2]->GetNumberEntry()->SetFocus();
  
  if (gTQSender == dNumericEntries[2]->GetNumberEntry())
    dNumericEntries[0]->GetNumberEntry()->SetFocus();
}



Bool_t QwGUIPlotOptionsDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
	      SetLimits();
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
	  case 20:
	    {
	      if (dCB[0]->GetState() == kButtonDown){
		dCB[1]->SetState(kButtonUp);
	      }
	    }
	    break;
	  case 21:
	    {
	      if (dCB[1]->GetState() == kButtonDown){
		dCB[0]->SetState(kButtonUp);
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
