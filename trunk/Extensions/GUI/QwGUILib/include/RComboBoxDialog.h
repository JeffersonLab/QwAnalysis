/////////////////////////////////////////////////////////////////////////////////////
//NPDGamma Experiment Data Visualization and Analysis
//
//Version 2.01
//
//RComboBoxDialog header file, containing methods to monitor analyzing
//progress
/////////////////////////////////////////////////////////////////////////////////////


#ifndef RCOMBOBOXDIALOG_H
#define NCOMBOBOXDIALOG_H

#include <stdlib.h>
#include <math.h>
#include <TVirtualX.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TGButton.h>
#include <TEnv.h>
#include <TGComboBox.h>
#include <RDataContainerDef.h>
#include <string.h>

class RComboBoxDialog : public TGTransientFrame {

private:

  char               dObjName[NAME_STR_MAX];
  char               dOwnerName[NAME_STR_MAX]; 
  char               dTitle[NAME_STR_MAX]; 
  int                dEntrycnt;
  Int_t              dListSize;

  char             (*dEntryArray)[NAME_STR_MAX];

  TGComboBox        *fComboBox;
  TGTextButton      *fCancel;
  TGVerticalFrame   *frame;
  TGLayoutHints     *fHint, *fHint2, *fHint3;
  
 public:
  RComboBoxDialog(const TGWindow *p, const TGWindow *main, 
		  const char* objname, const char *ownername,
		  const char* title, Int_t size, UInt_t w, UInt_t h);
  virtual ~RComboBoxDialog();
  
  virtual void      CloseWindow();
  virtual Bool_t    ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void              IsClosing(char *);
  void              AddEntry(char *);
  void              SetSelectedEntry(char *);

  ClassDef(RComboBoxDialog,0);
};

#endif
