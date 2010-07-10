//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIProgressDialog.h
   \author Michael Gericke
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIProgressDialog
    
   \brief Handles the display of extended calculation progress.  

   The QwGUIProgressDialog class handles the display of a progress bar for extended
   calculation processes.
    
 */
//=============================================================================

///
/// \ingroup QwGUIMain


#ifndef QWGUIPROGRESSDIALOG_H
#define QWGUIPROGRESSDIALOG_H

#include <stdlib.h>
#include <math.h>
#include <TVirtualX.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TGButton.h>
#include <TEnv.h>
#include <TGProgressBar.h>
#include "QwGUIMainDef.h"
#include <string.h>

class QwGUIProgressDialog : public TGTransientFrame {

private:

  char               dObjName[NAME_STR_MAX];
  char               dMainName[NAME_STR_MAX]; 
  int                dRuncnt;
  int                dMpcnt;
  int                dMpcnt2;
  int                dNum;
  char               dTemp[500];
  Bool_t             dContinue;
  Bool_t             dStop;
  Bool_t             kStopable;
  Bool_t             dReverse;

  TGHorizontalFrame *fHframe1;
  TGVerticalFrame   *fVframe1;
  TGLayoutHints     *fHint2, *fHint3, *fHint4, *fHint5;
  TGHProgressBar    *fHProg1, *fHProg2, *fHProg3;
  TGTextButton      *fStop, *fPause, *fResume;
  
 public:
  QwGUIProgressDialog(const TGWindow *p, const TGWindow *main, 
		      const char* objname, const char *mainname,
		      const char* title, const char* macrotext, const char* microtext,
		      const char* microtext2, int runcnt, int mpcnt , int mpcnt2, 
		      UInt_t w, UInt_t h, Bool_t stopable = kTrue, Bool_t kReverse = kFalse, Int_t num = 2);
  virtual ~QwGUIProgressDialog();
  
  Bool_t            Continue() {return dContinue;}
  Bool_t            Stop() {return dStop;}
  virtual void      CloseWindow();
  virtual Bool_t    ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void              IsClosing(char *);
  void              HaltProcess();
  void              ResetRange(int,int,int);
  void              SetTitle(char *title);
  void              SetPosition(int p1, int p2, int p3);

  ClassDef(QwGUIProgressDialog,0);
};

#endif
