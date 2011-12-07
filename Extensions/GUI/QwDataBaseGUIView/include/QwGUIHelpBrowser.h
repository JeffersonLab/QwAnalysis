//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUIHelpBrowser.h
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
   \class QwGUIHelpBrowser.h

   \brief Qweak GUI Help Interface


 */
//=============================================================================

///
/// \ingroup QwGUIMain


#ifndef QWGUIHELPBROWSER_H
#define QWGUIHELPBROWSER_H

#include "TGHtmlBrowser.h"
#include "QwGUIMainDef.h"

class QwGUIHelpBrowser : public TGHtmlBrowser {

  char dMainName[NAME_STR_MAX];
  char dObjName[NAME_STR_MAX];
  
 public:
  QwGUIHelpBrowser(const TGWindow *p, const TGWindow *main, const char *objName, 
		   const char *mainname, const char* file = 0, UInt_t w = 900, UInt_t h = 600); 
  virtual ~QwGUIHelpBrowser();
  
  void             IsClosing(const char *objname);
  
  
  ClassDef(QwGUIHelpBrowser,0);
};

#endif
