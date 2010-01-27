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


#define HFRAME_NUM             2
#define GFRAME_NUM             1
#define G0HFRAME_NUM           2
#define G0VFRAME_NUM           1

#define NUM_ENTRY_NUM          3
#define LABEL_NUM              2
#define TXT_BUTTON_NUM         2
#define CHK_BUTTON_NUM         1





class QwGUITrackingSystem : public TGCompositeFrame {

 private:

  TGHorizontalFrame   *hframe[HFRAME_NUM];
  TGGroupFrame        *gframe[GFRAME_NUM];
  TGHorizontalFrame   *g0hframe[G0HFRAME_NUM];
  TGVerticalFrame     *g0vframe[1];

  TGNumberEntry       *num_entry[NUM_ENTRY_NUM];
  TGLabel             *num_label[LABEL_NUM];
  TGTextButton        *txt_button[TXT_BUTTON_NUM];
  TGCheckButton       *chk_button[CHK_BUTTON_NUM];

  Pixel_t             green;
  Pixel_t             red;

  UInt_t              run_number;
  UInt_t              event_range[2];
  UInt_t        default_event_range[2];

  Bool_t              event_range_status;
  Bool_t              load_rootfile_status;

 public:
  
  QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h);
  ~QwGUITrackingSystem();
  void    CreateFrame          (TGCompositeFrame *parent, UInt_t w, UInt_t h);
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);


  TString GetFileName();
  Bool_t  SetEventRange();
  void    DefaultEventRange(Bool_t entry_status);
  void    ResetEventRange(Bool_t entry_status);

  ClassDef(QwGUITrackingSystem,0);
};

#endif
