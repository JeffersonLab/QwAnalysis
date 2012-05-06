#ifndef QWAUTOGUI_H
#define QWAUTOGUI_H

#include <TROOT.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TString.h>
#include <TText.h>
#include <TGMsgBox.h>
#include <time.h>
#include <signal.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <unistd.h>


struct ClineArgs {

  int remoterestart;
  
};

class QwAutoGUI : public TGMainFrame {

private:

  ClineArgs dClArgs;

  Char_t dMiscbuffer[500];
  Char_t dMiscbuffer2[500];
  void   DoWarn();

  Int_t  dCurrentRun;
  Int_t  dCurrentSegment;
  Int_t  dPID;

  void   GetFileInfo(const char *filename, int &run, int &segment);

public:
  QwAutoGUI(const TGWindow *p, ClineArgs, UInt_t w, UInt_t h);
  virtual ~QwAutoGUI();
  
  virtual void CloseWindow();
  ClassDef(QwAutoGUI,0);

  /* void OnCloseSignal(int sig); */
};


#endif
