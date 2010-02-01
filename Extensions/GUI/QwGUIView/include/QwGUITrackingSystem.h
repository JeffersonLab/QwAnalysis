// Name  :   QwGUITrackingSystem.h
// Author:   Jeong Han Lee
// email :   jhlee@jlab.org

#ifndef QWGUITRACKINGSYSTEM_H
#define QWGUITRACKINGSYSTEM_H

#include <Riostream.h>
#include "TROOT.h"
#include "TSystem.h"
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
#include "TLatex.h"

#include "TGProgressBar.h"


#include "TH1D.h"
#include "TH2I.h"
#include "TObjArray.h"

#include "TSuperCanvas.h"
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"



#define BOT_BUTTON_NUM         3

#define HFRAME_NUM             2
#define GFRAME_NUM             1
#define G0HFRAME_NUM           3
#define G0VFRAME_NUM           1

#define NUM_ENTRY_NUM          4
#define LABEL_NUM              3
#define TXT_BUTTON_NUM         6
#define CHK_BUTTON_NUM         1

#define BUTTON_GRP_NUM         3
#define RAI_BUTTON_NUM         7


#define RG_ONE_HIST_NUM        6
#define RG_TWO_HIST_NUM        12
#define RG_THR_HIST_NUM        2


#define RG_ONE_WIRE_NUM        30
#define RG_TWO_WIRE_NUM        30
#define RG_THR_WIRE_NUM        290

#define BUFFERSIZE             2000      // BufferSize of TH1 for automatic range of histogram
#define BINNUMBER              1024      // Default bin


#define REQUEST_TYPE_NUM       2
#define SELECT_TYPE_NUM        2

#define HIST_ONED_NUM          5
// one for 
/* #define HIST_TWOD_NUM          2 */

class QwGUITrackingSystem : public TGCompositeFrame {

 private:

  TSuperCanvas        *canvas;
  TGHProgressBar      *progress_bar;

  TGHorizontalFrame   *hframe  [HFRAME_NUM];
  TGGroupFrame        *gframe  [GFRAME_NUM];
  TGHorizontalFrame   *g0hframe[G0HFRAME_NUM];
  TGVerticalFrame     *g0vframe[G0VFRAME_NUM];

 

  TGNumberEntry       *num_entry [NUM_ENTRY_NUM];
  TGLabel             *num_label [LABEL_NUM];
  TGTextButton        *txt_button[TXT_BUTTON_NUM];
  TGCheckButton       *chk_button[CHK_BUTTON_NUM];

  TGButtonGroup       *btn_group [BUTTON_GRP_NUM];
  TGRadioButton       *rad_button[RAI_BUTTON_NUM];

  TGHorizontalFrame   *bframe;
  TGTextButton        *b_button[BOT_BUTTON_NUM];

  Pixel_t             green;
  Pixel_t             red;

  UInt_t              run_number;
  UInt_t              ch_wire_number;
  UInt_t              event_range[2];
  UInt_t              default_event_range[2];
  UInt_t              total_physics_event_number;

  UInt_t              d_request; // [0,1] can be extended 
  UInt_t              d_select;  // [0,1]    "
  Short_t             d_region;  // [1,3]    "

  Bool_t              event_range_status;
  Bool_t              load_rootfile_status;

  TFile               *root_file;
  TTree               *qwhit_tree;
  QwHit               *qwhit;
  QwHitRootContainer  *qwhit_container;

  

/*   TH1D *hist1_region1[RG_ONE_HIST_NUM]; */
/*   TH1D *hist1_region2[RG_TWO_HIST_NUM]; */
/*   TH1D *hist1_region3[RG_THR_HIST_NUM]; */

/*   TH1D *hist1_region1[RG_ONE_HIST_NUM]; */
/*   TH1D *hist1_region2[RG_TWO_HIST_NUM]; */
/*   TH1D *hist1_region3[RG_THR_HIST_NUM]; */

/*   TH1D *hist1_region1[RG_ONE_HIST_NUM]; */
/*   TH1D *hist1_region2[RG_TWO_HIST_NUM]; */
/*   TH1D *hist1_region3[RG_THR_HIST_NUM]; */

/*   TH1D *hist1_region1[RG_ONE_HIST_NUM]; */
/*   TH1D *hist1_region2[RG_TWO_HIST_NUM]; */
/*   TH1D *hist1_region3[RG_THR_HIST_NUM]; */

  
  TH2I *hist2_region1[RG_ONE_HIST_NUM];
  TH2I *hist2_region2[RG_TWO_HIST_NUM];
  TH2I *hist2_region3[RG_THR_HIST_NUM];

  
  TH1D *hist1_region1[HIST_ONED_NUM][RG_ONE_HIST_NUM];
  TH1D *hist1_region2[HIST_ONED_NUM][RG_TWO_HIST_NUM];
  TH1D *hist1_region3[HIST_ONED_NUM][RG_THR_HIST_NUM];
  
/*   TH2I *hist2_region1[HIST_TWOD_NUM][RG_ONE_HIST_NUM]; */
/*   TH2I *hist2_region2[HIST_TWOD_NUM][RG_TWO_HIST_NUM]; */
/*   TH2I *hist2_region3[HIST_TWOD_NUM][RG_THR_HIST_NUM]; */

  static const char *const request_types[REQUEST_TYPE_NUM];
  static const char *const select_types [SELECT_TYPE_NUM];
  static const char *const hist1_types  [HIST_ONED_NUM]; 


  Short_t GetRegion () { return d_region; };
  UInt_t  GetSelect () { return d_select; };
  UInt_t  GetRequest() { return d_request;};

  void    SetRegion (Short_t in) { d_region  = in;};
  void    SetSelect (UInt_t  in) { d_select  = in;};
  void    SetRequest(UInt_t  in) { d_request = in;};

  void    CheckOldObject(Char_t* name);
  void    ResetButtons (Bool_t main_buttons);
  void    EnableButtons(Bool_t main_buttons);
  void    SetEventRange();
  void    DefaultEventRange();
  void    ResetEventRange  ();
  void    ResetProgressBar();

  TSuperCanvas *CheckCanvas(const TGWindow *main, TSuperCanvas *temp_canvas, const char* name, bool close_status);

  Char_t  text_buffer[256];

  Bool_t  region_status[4];
  Bool_t  direction_status[7];
  Bool_t  plane_status[13];

 public:
  
  QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h);
  ~QwGUITrackingSystem();
  void    CreateFrame          (TGCompositeFrame *parent, UInt_t w, UInt_t h);
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);


  Bool_t  OpenRootFile ();
  void    CloseRootFile();

  void    PlotDraw();
  void    PlotEventVsWire();
  void    PlotHittedWireVsEvent();
  void    ManipulateHistograms();

  ClassDef(QwGUITrackingSystem,0);
};

#endif
