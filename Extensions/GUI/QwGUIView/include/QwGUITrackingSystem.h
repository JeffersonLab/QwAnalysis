//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUITrackingSystem.h
   \author Michael Gericke
   \author Jeong Han Lee
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUITrackingSystem
    
   \brief 

   This is Gericke's original code for the main detector.  
   Jeong Han tried to test TSuperCanvas within GUI framework
    
 */
//=============================================================================



#ifndef QWGUITRACKINGSYSTEM_H
#define QWGUITRACKINGSYSTEM_H

#include <Riostream.h>
#include "TGWindow.h"
#include "TCanvas.h"
#include "TGFrame.h"
#include "TGTextEntry.h"
#include "TGButton.h"
#include "TGButtonGroup.h"
#include "TGTextEdit.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"
#include "TGComboBox.h"
#include "TGLayout.h"
#include "TTree.h"
#include "TFile.h"

#include "TSuperCanvas.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"

class QwHit;
class QwHitContainer;
class QwHitRootContainer;

class QwGUITrackingSystem : public TGCompositeFrame {

 private:

  TGGroupFrame        *gframe[1];
  TGHorizontalFrame   *g0hframe[2];
  TGVerticalFrame     *g0vframe[1];

  TGNumberEntry       *num_entry[1];
  TGLabel             *num_label[1];
  TGTextButton        *txt_button[1];

  Pixel_t             green;
  Pixel_t             red;

  UInt_t              run_number;

  
 public:
  
  QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h);
  ~QwGUITrackingSystem();
  void    CreateFrame          (TGCompositeFrame *parent, UInt_t w, UInt_t h);
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

  ClassDef(QwGUITrackingSystem,0);
};

#endif
