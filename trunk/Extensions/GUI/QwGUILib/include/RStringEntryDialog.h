/////////////////////////////////////////////////////////////////////////////////////
//NPDGamma Experiment Data Visualization and Analysis
//
//Version 2.01
//
/////////////////////////////////////////////////////////////////////////////////////


#ifndef RSTRINGENTRYDIALOG_H
#define RSTRINGENTRYDIALOG_H

#include <stdlib.h>
#include <math.h>
#include <TVirtualX.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TGButton.h>
#include <TEnv.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <RDataContainerDef.h>
#include <string.h>

class RStringEntryDialog : public TGTransientFrame {

private:

  Bool_t            *dRetStat;

  char               dObjName[NAME_STR_MAX];
  char               dOwnerName[NAME_STR_MAX]; 
  char               dTitle[NAME_STR_MAX]; 
  char              *dFunc;

  TGTextEntry       *fFuncEntry;
  TGTextBuffer      *fFuncBuffer;
  TGTextButton      *fCancel;
  TGTextButton      *fOk;
  TGVerticalFrame   *frame;
  TGHorizontalFrame *fBttnframe;
  TGHorizontalFrame *fEntrframe;
  TGLayoutHints     *fHint, *fHint2, *fHint3, *fHint4;
  
 public:
  RStringEntryDialog(const TGWindow *p, const TGWindow *main, 
		     char* objname, char *ownername, Bool_t *Stat,
		     char *Func, const char *title, UInt_t w, UInt_t h);
  virtual ~RStringEntryDialog();
  
  virtual void      CloseWindow();
  virtual Bool_t    ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void              IsClosing(char *);

  ClassDef(RStringEntryDialog,0);
};

#endif
