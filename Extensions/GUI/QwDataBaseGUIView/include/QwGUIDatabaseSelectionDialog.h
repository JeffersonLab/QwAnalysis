#ifndef QWGUIDATABASELECTIONDIALOG_H
#define QWGUIDATABASELECTIONDIALOG_H


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
#include "QwGUIMainDef.h"
#include "RSDataWindow.h"

struct DatabaseOptions {
  Bool_t       cancelFlag;
  Bool_t       changeFlag;
  TString      dbserver;
  TString      dbname;
  TString      uname;
  TString      psswd;
  Int_t        dbport;
};

class QwGUIDatabaseSelectionDialog : public TGTransientFrame {

private:

  Bool_t              *dChangeFlag;
  Bool_t              *dCancelFlag;
  char                 dObjName[NAME_STR_MAX];
  char                 dMainName[NAME_STR_MAX]; 
  DatabaseOptions     *dOptions;

  TGTextEntry       *fFuncEntry[4];
  TGTextBuffer      *fFuncBuffer[4];
  TGTextButton      *fCancel;
  TGTextButton      *fAccept;
  TGTextButton      *fOk;
  TGVerticalFrame   *frame;
  TGHorizontalFrame *fBttnframe;
  TGHorizontalFrame *fEntrframe[5];
  TGLayoutHints     *fHint, *fHint2, *fHint3, *fHint4;
  TGNumberEntry     *fNumEntry;
  TGCheckButton     *fCB[5];
  TGLabel           *fLabel[5];

  static const char *const numlabel[5];
  TString defaultVals[4];

public:
   QwGUIDatabaseSelectionDialog(const TGWindow *p, const TGWindow *main, 
		      const char* objname, const char *mainname,
		      DatabaseOptions *opts, UInt_t w = 400, UInt_t h = 500, 
		      UInt_t options = kVerticalFrame | kHorizontalFrame);
   virtual ~QwGUIDatabaseSelectionDialog();
   virtual void CloseWindow();

   void SetOptions();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void IsClosing(char *objname);

   ClassDef(QwGUIDatabaseSelectionDialog,0)
};


#endif
