#ifndef QWGUIHCLOGENTRYDIALOG_H
#define QWGUIHCLOGENTRYDIALOG_H


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <TVirtualX.h>
#include <TGClient.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGTextEdit.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TString.h>
#include <TEnv.h>
#include <TKey.h>
#include "QwGUIMainDef.h"
#include "RSDataWindow.h"

struct HCLogEntry {
  TString name;
  TString subject;
  TString comments;
  TString emaillist;
  Bool_t  setFlag;
};

class QwGUIHCLogEntryDialog : public TGTransientFrame {

private:

  Bool_t              *dFlag;
  Double_t             eval[3];
  char                 dObjName[NAME_STR_MAX];
  char                 dMainName[NAME_STR_MAX]; 
  HCLogEntry          *dHCEntries;

  TGVerticalFrame    *dMainFrame;  
  TGVerticalFrame    *dEntryFrame;
  TGHorizontalFrame    *dSetFrame;
  TGHorizontalFrame    *dNameFrame;
  TGHorizontalFrame    *dSubjectFrame;
  TGVerticalFrame    *dCommentFrame;
  TGHorizontalFrame    *dEmailFrame;

  TGTextEntry         *dNameEntry;
  TGTextEntry         *dSubjectEntry;
  TGTextEdit         *dCommentEntry;
  /* TGTextEntry         *dCommentEntry; */
  TGTextEntry         *dEmailEntry;

  TGTextBuffer        *dNameBuffer;
  TGTextBuffer        *dSubjectBuffer;
  /* TGTextBuffer        *dCommentBuffer; */
  TGTextBuffer        *dEmailBuffer;

  TGLabel             *NameLabel;
  TGLabel             *SubjectLabel;
  TGLabel             *CommentLabel;
  TGLabel             *EmailLabel;

  TGButton            *dSetButton; 
  TGButton            *dExitButton;

public:
   QwGUIHCLogEntryDialog(const TGWindow *p, const TGWindow *main, 
		      const char* objname, const char *mainname,
		      HCLogEntry *entryData,
		      UInt_t w, UInt_t h, 
		      UInt_t options = kVerticalFrame | kHorizontalFrame);
   virtual ~QwGUIHCLogEntryDialog();
   virtual void CloseWindow();

   void SetEntries();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void IsClosing(char *objname);
   void FChangeFocus();

   ClassDef(QwGUIHCLogEntryDialog,0)
};


#endif
