#ifndef __TaPanam__
#define __TaPanam__
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan
//
//           TaPanam.hh
//
// Author: A. Vacheret <http://www.jlab.org/~vacheret>
// @(#)pan/src:$Name:  $:$Id: TaPanam.hh,v 1.2 2003/07/02 20:45:33 vacheret Exp $//
//
////////////////////////////////////////////////////////////////////////
//
//  Graphical user interface using Pan for monitoring purposes and specific 
//  analysis quick launch. Inspiration for layout  
//  is from ALICE testbeam monitoring and ROOT test example.   
//
////////////////////////////////////////////////////////////////////////

#define DEBUG 1

// STD includes 
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

// ROOT includes
#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TDirectory.h>
#include <TVirtualX.h>
#include <TGFrame.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGClient.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
//#include <TGRadioButton.h>
//#include <TGCheckButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
//#include <TGSlider.h>
//#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h> 
#include <TRandom.h>
//#include <TEnv.h>
#include <TGStatusBar.h>
#include <TGWidget.h>
#include <TFile.h>
#include <TKey.h>
#include <TTimer.h>
#include <TQObject.h> 
#include "TaThread.hh"
#include <TCondition.h>
#include <TMutex.h>
#include <TCanvas.h>
#include <TStyle.h>
// Pan includes 
#include "TaAnalysisManager.hh"

enum ETestCommandIdentifiers {
  M_FILE_OPEN,
  M_FILE_EDITDB,
  M_FILE_PRINT,
  M_FILE_PRINT1,
  M_FILE_SAVE,
  M_FILE_SAVE1,
  M_FILE_SAVE2,
  M_FILE_EXIT,

  M_ANA_ADC,
  M_ANA_BEAM, 
  M_ANA_I,
  M_ANA_P, 
  M_ANA_S, 
  M_ANA_SAGDH,
  M_ANA_HAPPEX,
  M_ANA_MAN,

  M_MON_PRESTART,
  M_MON_START,
  M_MON_STOP,
  M_MON_END,
  M_MON_HISTOALLRAZ 
};


class TaPanam : public TGMainFrame, public TaThread {

public:

  enum NumOfChan { maxadc = 2, maxchanadc = 4};

  TaPanam();
  TaPanam(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~TaPanam();
  // GUI functions                     
  virtual   void         CloseWindow();
  Int_t                  SetConfig(UInt_t confignum);
  void                   AddDevToList(TGCheckButton *thebutton, string key1, string key2);
  void                   AddADCToList(TGCheckButton *thebutton, string key1, string key2);
  void                   Add2DDevToList(TGCheckButton *thebutton, string key1, string key2);
  Int_t                  SetDevList();
  Int_t                  CheckOptionConfig();
  Int_t                  InitFlags(); 
  //Int_t                  CheckCanvasConfig(Int_t DAQconfig);
  virtual Bool_t         ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  Int_t                  HandleCheckButton(Long_t parm1);
  Int_t                  HandleMenuBar(Long_t parm1);
  Int_t                  HandleListBox(Long_t parm1);
  void                   UpdateEmCanvas();
  Int_t                  InitParameters();              // initialization of parameters
  void                   ResetDevCheckButtons();              // Reset devices checkbuttons.
  Int_t                  InitDevCheckButtons();              // initialization of checkbuttons.
  string                 itos(Int_t i);                    // integer to string converter
  //  void                   DumpDevList(Int_t devtype);
  Int_t                  InitListOfAllPlots();                // initialization of device list
  void                   SetNumOfPads();                // count data if defined
  void                   DumpOptionState();
  void                   UpdateCanvasWithDevice(Char_t* title, vector<Int_t> idxvect, Int_t devtype, Bool_t disptype);                //
  void                   UpdateIADCStack(Bool_t disptype);
  void                   UpdatePADCStack(Bool_t disptype);
  void                   UpdateSADCStack(Bool_t disptype);
  void                   UpdateIADCRMSStack();
  void                   UpdatePADCRMSStack();
  void                   UpdateSADCRMSStack();
  void                   UpdateBattStack(Bool_t disptype);
  void                   UpdateBCMStack(Bool_t disptype);
  void                   UpdateInjBPMStack(Bool_t disptype);
  void                   UpdateBPMStack(Bool_t disptype);
  void                   UpdateBPMcavStack(Bool_t disptype);
  void                   UpdateLumiBackStack();
  void                   UpdateLumiFrontStack();
  void                   UpdateDaltonStack();
  void                   CanvasSafeDelete(TCanvas* can, char* name);                //
  Int_t                  UpdateCanvasFlock();                //
  Int_t                  InitCanvasPads();                 // Initialization of canvas pads
  Int_t                  UpdateCanvasPads();
  void                   ResetIdxVector();                //
  void                   DisplayThis(TGCheckButton *thebutton,Bool_t showcan);
  Int_t                  InitHistoListBox(vector<string> thelist); // Get list of plots for lsitbox 
  void                   InitGUIFrames();                  // Initialization of the GUI 
  Int_t                  ClearCanvas();
  Int_t                  ClearListBox();
  Int_t                  ClearListOfDevice();                   
  // Thread related functions
  Int_t                  ThreadIsRunning();                // 
  void                   DoPan();                          //  Threaded  method start Pan analysis 
  void                   DoRefreshing();                   //  Threaded method  to refresh display 
  Int_t                  RefreshCanvas();                  //  Actual canvas update method
  void                   SetStyles();
 private: 
  // GUI frame
  TGCompositeFrame    *fMainTitle;                                   // 
  TGCompositeFrame    *fUtilFrame;                                   //
  TGCompositeFrame    *fHistoFrame;                                  //
  TGCompositeFrame    *fButtonFrame;                                 // 
  TGCompositeFrame    *fMainFrame;                                   //
  TGCompositeFrame    *fEmCanFrame;                                  //
  TGCompositeFrame    *fTimeFrame;                                   //
  TGCompositeFrame    *fADCFrame;                              //
  TGCompositeFrame    *fADCSubFrame;                              //
  TGCompositeFrame    *fADCFrame0;                              //
  TGCompositeFrame    *fADCFrame1;                              //
  TGCompositeFrame    *fADCFrame2;                              //
  TGCompositeFrame    *fCurrentFrame;                                       //
  TGCompositeFrame    *fCurrentSubFrame;                                       //
  TGCompositeFrame    *fInjBCMFrame;                                       //
  TGCompositeFrame    *fBCMFrame;                                           //
  TGCompositeFrame    *fBCMDispFrame;                                       //
  TGCompositeFrame    *fBCMOptFrame;                                       //
  TGCompositeFrame    *fPositionFrame;                                       //
  TGCompositeFrame    *fBPMDevFrame;                                       //
  TGCompositeFrame    *fInjBPMFrame;                                      //
  TGCompositeFrame    *fBPMFrame;                                       //
  TGCompositeFrame    *fBPMDispFrame;                                       //
  TGCompositeFrame    *fBPMDFrame;                                       //
  TGCompositeFrame    *fBPMOptFrame;                                       //
//   TGCompositeFrame    *fFeedbackFrame;                                       //
//   TGCompositeFrame    *fFCurFrame;                                       //
//   TGCompositeFrame    *fFPosFrame;
//   TGCompositeFrame    *fDITHFrame;
//   TGCompositeFrame    *fControlFrame;                                         //
  TGCompositeFrame    *fDetFrame;                               //
  TGCompositeFrame    *fLumiFrame;                               //
  TGCompositeFrame    *fDalFrame;                               //
  TGCompositeFrame    *fScanFrame;                               //
  TGGroupFrame        *fTimegFrame;
  TGGroupFrame        *fTOptgFrame;
  TGGroupFrame        *fIADCCrate;
  TGGroupFrame        *fPADCCrate;
  TGGroupFrame        *fSADCCrate;
  TGGroupFrame        *fADCOptgFrame;
  TGGroupFrame        *fCurrentgFrame;
  TGGroupFrame        *fInjBCMgFrame;
  TGGroupFrame        *fBCMgFrame;
  TGGroupFrame        *fBCMDispgFrame;
  TGGroupFrame        *fPositiongFrame;
  TGGroupFrame        *fInjBPMgFrame;
  TGGroupFrame        *fBPMgFrame;
  TGGroupFrame        *fBPMDispgFrame;
  //  TGGroupFrame        *fFCurgFrame;
  //  TGGroupFrame        *fFPosgFrame;
  TGGroupFrame        *fDITHgFrame;
  TGGroupFrame        *fLumigFrame;
  TGGroupFrame        *fDalgFrame;
  TGGroupFrame        *fScangFrame;
  TGLabel             *fTitle;                                       // 
  TGLayoutHints       *fL1,*fL2,*fL3,*fL4,*fL5,*fL6,*fL7,*fL8,*fL9;            //
  TGPictureButton     *fLogoButton;                                  // 
  TGTextButton        *fButton1,*fButton2,*fButton3;                 //
  TGTextBuffer        *fConfigBuff,*fCodaRunBuff,*fRefreshBuff; //
  TGTextEntry         *fConfigText,*fCodaRunText,*fRefreshText; //
  TGLabel             *fTextLab;
  TGCheckButton       *fTimeCheck[7],*fTOptCheck[2];  
  TGCheckButton       *fADCOptCheck[7],*fADCCheck[9],*fIADCCheck[3],*fSADCCheck[2];
  TGCheckButton       *fInjBCMCheck[2]; 
  TGCheckButton       *fBCMCheck[12],*fBCMDispCheck[3],*fBCMOptCheck[2];
  TGCheckButton       *fInjBPMCheck[10];
  TGCheckButton       *fBPMCheck[8],*fBPMDispCheck[3],*fBPMDCheck[3],*fBPMOptCheck[2];
  //  TGCheckButton       *fFCurCheck[2],*fFPosCheck,*fDITHCheck[2];              
  TGCheckButton       *fLumiCheck[6],*fDalCheck[4],*fScanCheck[2];
                
  TGCanvas            *fHistoTreeView;                                // 
  TRootEmbeddedCanvas *fEmCan;                                        //
  TGMenuBar           *fMenuBar;                                      // GUI menu bar widget
  TGLayoutHints       *fMenuBarLayout, *fMenuBarItemLayout;           // 
  TGPopupMenu         *fMenuFile, *fMenuMon, *fMenuAna;               // 
  TGListBox           *fHistoListBox;                                                   //
  TGTab               *fTab; 
  TCanvas             *fIADCCan,*fPADCCan,*fSADCCan;         //
  TCanvas             *fIADCRMSCan,*fPADCRMSCan,*fSADCRMSCan;         //
  TCanvas             *fInjBCMCan,*fBattCan,*fBCMCan;
  TCanvas             *fInjBPMCan,*fBPMCan,*fBPMcavCan;                                               //
  TCanvas             *fLumiFrontCan,*fLumiBackCan,*fDETCan,*fScanCan;               //
  TaAnalysisManager   *fAnaMan;   // Pan Analysis Manager 
  TStyle              *fStripStyle,*fHistoStyle,*fGraphStyle;  
  vector<string>      fDeviceList;
  vector<string>      fADeviceList;
  vector<string>      fAllDeviceList;
  vector<string>      fMultiDeviceList;
  vector<Int_t>       fTimeidx,fIADCidx,fPADCidx,fSADCidx,fBattidx;
  vector<Int_t>       fBCMidx,fBCMcavidx,fInjBPMidx,fBPMidx,fBPMINidx,fBPMcavidx,fLUMIBackidx,fLUMIFrontidx,fDETidx;
  TString             fCODARunNumber;
  TString             fConfigName;
  Int_t               fColor[maxchanadc];                           //
  Int_t               fEntries;                           // 
  Int_t               fHistPadNum,fStripPadNum;            //
  Int_t               fPADCNum, fIADCNum, fSADCNum;
  Int_t               fInjBattNum,fBattNum,fInjBCMNum,fUnserNum;
  Int_t               fQuadNum,fBCM12Num,fBCM34Num, fBCMcavNum; 
  Int_t               fInjBPMNum, fBPMNum, fBPMcavNum;
  Int_t               fLumiBackNum,fLumiFrontNum, fDETNum;
  Int_t               fBPMplot,fBCMplot,fDAQConfig;
  Bool_t              fPanThreadRun;    // Pan Thread flag 
  Bool_t              fShowIADC, fShowPADC, fShowSADC; // 
  Bool_t              fShowInjBCM, fShowBatt, fShowBCM;
  Bool_t              fShowInjBPM, fShowBPM, fShowBPMcav;
  Bool_t              fShowLumiFront, fShowLumiBack, fShowDET, fShowScan;
  Bool_t              fFirstAna;
  Bool_t              fPrestart,fStart,fStop,fEnd;
  Bool_t              fADCdata;
  Bool_t              fTSCorH,fADCSCorH,fBCMSCorH,fBPMSCorH;  

  ClassDef(TaPanam, 1)   // 
};

#endif


