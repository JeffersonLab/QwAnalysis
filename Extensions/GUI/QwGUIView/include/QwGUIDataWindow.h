#ifndef QWGUIDATAWINDOW_H
#define QWGUIDATAWINDOW_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <TRootCanvas.h>
#include <TCanvas.h>
#include <TGMenu.h>
#include <TF1.h>
#include "QwGUIMainDef.h"

class QwGUIDataWindow : public TQObject {

 private:

  Int_t            wID;
  
  TCanvas          dCanvas;
  TGMenu           dMenu;

 protected:

 public:
  
  QwGUIDataWindow(const TGWindow *p, const TGWindow *main, const TGTab *tab, 
		 const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUIDataWindow();

  Int_t            DrawData(const TF1&, Bool_t add = kFalse);

  
  ClassDef(QwGUIDataWindow,0);
};

#endif
