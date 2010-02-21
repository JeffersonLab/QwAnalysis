/////////////////////////////////////////////////////////////////////////////////////
//NPDGamma Experiment Data Visualization and Analysis
//
//Version 2.01
//
/////////////////////////////////////////////////////////////////////////////////////


#ifndef RNUMBERENTRYDIALOG_H
#define RNUMBERENTRYDIALOG_H

#include <stdlib.h>
#include <math.h>
#include <TVirtualX.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TGButton.h>
#include <TEnv.h>
#include <TGNumberEntry.h>
#include <RDataContainerDef.h>
#include <string.h>

class RNumberEntryDialog : public TGTransientFrame {

private:

  char               dObjName[NAME_STR_MAX];
  char               dOwnerName[NAME_STR_MAX]; 
  char               dTitle[NAME_STR_MAX]; 
  Double_t          *dNumber;
  Int_t             *dRetVal;

  TGNumberEntry     *fNumEntry;
  TGTextButton      *fCancel;
  TGTextButton      *fOk;
  TGVerticalFrame   *frame;
  TGHorizontalFrame *fBttnframe;
  TGHorizontalFrame *fEntrframe;
  TGLayoutHints     *fHint, *fHint2, *fHint3, *fHint4;
  
 public:
  RNumberEntryDialog(const TGWindow *p, const TGWindow *main, 
		     const char* objname, const char *ownername, const char* title,
		     Double_t *Number, Int_t *retval, UInt_t w, UInt_t h);
  virtual ~RNumberEntryDialog();
  
  virtual void      CloseWindow();
  virtual Bool_t    ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void              IsClosing(char *);

  ClassDef(RNumberEntryDialog,0);
};

#endif
