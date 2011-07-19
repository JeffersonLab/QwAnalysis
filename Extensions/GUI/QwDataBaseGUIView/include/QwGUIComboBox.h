//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUIComboBox.h
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
   \class QwGUIComboBox.h

   \brief Qweak GUI Help Interface


 */
//=============================================================================

///
/// \ingroup QwGUIMain


#ifndef QWGUICOMBOBOX_H
#define QWGUICOMBOBOX_H

#include "TGComboBox.h"

class QwGUIComboBox : public TGComboBox {
  
 public:
  QwGUIComboBox(const TGWindow *p = 0, const char* txt = 0, Int_t id = -1,
		 UInt_t options = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
		 Pixel_t back = GetWhitePixel());
  virtual ~QwGUIComboBox();

  void PopThisUp();
  
  ClassDef(QwGUIComboBox,0);
};

#endif
