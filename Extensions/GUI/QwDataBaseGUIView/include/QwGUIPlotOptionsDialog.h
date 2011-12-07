#ifndef QWGUIPLOTOPTIONSDIALOG_H
#define QWGUIPLOTOPTIONSDIALOG_H


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

struct DataOptions {
  Bool_t       limitsFlag;    /*Have limits been set by user*/
  Int_t        channel;       /*ADC channel number, or better yet, detector number*/
  Int_t        startev;       /*Start at event/helicity window number*/
  Int_t        numevs;        /*Number of helicity windows to be processed*/
  Bool_t       mpAverage;
  Bool_t       mpIntegrate;
  Bool_t       mpPedSubtract;
  Bool_t       mpBgrdSubtract;
  Bool_t       mpNormToBeam;
};

class QwGUIPlotOptionsDialog : public TGTransientFrame {

private:

  Bool_t              *dFlag;
  Double_t             eval[3];
  char                 dObjName[NAME_STR_MAX];
  char                 dMainName[NAME_STR_MAX]; 
  DataOptions         *dOptions;

  TGCompositeFrame    *dMainFrame;  
  TGCompositeFrame    *dEnterFrame;
  TGCompositeFrame    *dSetFrame;
  TGGroupFrame        *dCheckFrame;
  TGGroupFrame        *dRangeFrame;

  TGHorizontalFrame   *dNumEntFrame[3];
  TGNumberEntry       *dNumericEntries[3];
  TGLabel             *dNumEntLabel[3];
  TGCheckButton       *dCB[5];

  TGButton            *dSetButton; 
  TGButton            *dExitButton;

  TGLayoutHints       *dMainLayout; 
  TGLayoutHints       *dEnterLayout;
  TGLayoutHints       *dSetLayout;  
  TGLayoutHints       *dCheckLayout;
  TGLayoutHints       *dRangeLayout;
  TGLayoutHints       *dButtonLayout;
  TGLayoutHints       *dNumEntLayout;

  static const char *const numlabel[8];
  static const Double_t numinit[6];

public:
   QwGUIPlotOptionsDialog(const TGWindow *p, const TGWindow *main, 
		      const char* objname, const char *mainname,
		      DataOptions *opts,
		      UInt_t w, UInt_t h, 
		      UInt_t options = kVerticalFrame | kHorizontalFrame);
   virtual ~QwGUIPlotOptionsDialog();
   virtual void CloseWindow();

   void SetLimits();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void IsClosing(char *objname);
   void FChangeFocus();

   ClassDef(QwGUIPlotOptionsDialog,0)
};


#endif
