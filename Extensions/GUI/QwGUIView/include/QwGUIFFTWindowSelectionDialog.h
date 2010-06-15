#ifndef QWGUIFFTWINDOWSELECTIONDIALOG_H
#define QWGUIFFTWINDOWSELECTIONDIALOG_H

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <TVirtualX.h>
#include <TGClient.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TKey.h>
#include <TGMsgBox.h>
#include "RMsgBox.h"
#include "QwGUIMainDef.h"
#include "RSDataWindow.h"

struct FFTOptions {
  Bool_t       cancelFlag;
  Bool_t       changeFlag;
  Int_t        Start;
  Int_t        Length;
  Int_t        TotalLength;
};

class QwGUIFFTWindowSelectionDialog : public TGTransientFrame {

private:

  const TGWindow            *dMain;

  Bool_t              *dChangeFlag;
  Bool_t              *dCancelFlag;
  char                 dObjName[NAME_STR_MAX];
  char                 dMainName[NAME_STR_MAX]; 
  FFTOptions          *dOptions;

/*   TGTextEntry       *fFuncEntry[4]; */
/*   TGTextBuffer      *fFuncBuffer[4]; */
  TGTextButton      *fCancel;
  TGTextButton      *fAccept;
  TGTextButton      *fOk;
  TGVerticalFrame   *frame;
  TGHorizontalFrame *fBttnframe;
  TGHorizontalFrame *fEntrframe1;
  TGHorizontalFrame *fEntrframe2;
  TGLayoutHints     *fHint, *fHint2, *fHint3, *fHint4;
  TGNumberEntry     *fNumEntry1;
  TGNumberEntry     *fNumEntry2;
/*   TGCheckButton     *fCB[5]; */
  TGLabel           *fLabel1;
  TGLabel           *fLabel2;

  TString defaultVals[3];

  void SetOptions(Bool_t all = kFalse);

public:
   QwGUIFFTWindowSelectionDialog(const TGWindow *p, const TGWindow *main, 
		      const char* objname, const char *mainname,
		      FFTOptions *opts, UInt_t w = 400, UInt_t h = 500, 
		      UInt_t options = kVerticalFrame | kHorizontalFrame);
   virtual ~QwGUIFFTWindowSelectionDialog();
   virtual void CloseWindow();

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void IsClosing(char *objname);

   ClassDef(QwGUIFFTWindowSelectionDialog,0)
};


#endif
