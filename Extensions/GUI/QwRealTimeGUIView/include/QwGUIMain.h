//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUIMain.h
   \author Michael Gericke
   \author Rakitha Beminiwattha

*/
//=============================================================================
//
//=============================================================================
//
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIMain

   \brief Qweak GUI Interface

   The QwGUIMain class provides the main data interface GUI. It is the top level
   window, through which most communication with the operating system happens (except
   for transient windows, such as dialog boxes). The main menu and the main tab
   environment are defined in this class. All sub systems are instantiated in this
   class' constructor and their instances are kept in an object array for cleanup
   purposes. Each subsystem has a tab associated with itself and is given a menu entry
   to control visbility of this tab. This happens automatically on creation of the
   instance, with a single function call from a new sub system constructor.

   The class ties together all communication between other classes. It facilitates the
   opening and closing of common files and provides access to the data for all other
   classes (except dialog boxes and special data transient windows, which have their
   own file access methods). The details of what is done with the data is left to the
   implementation of the individual sub systems, which communicate with each other
   and this class via singal passing.

   The class also implements a data log (with its own tab) which receives and posts
   messages, from anywhere within the program. Each sub system can send messages to
   the log, either with or without a time stamp. At the same time, the log serves as
   a basic text editor, so that notes and comments can be added by the user.

 */
//=============================================================================

///
/// \ingroup QwGUIMain


#ifndef QWGUIMAIN_H
#define QWGUIMAIN_H

//#include <cstring>

#include <iostream>
#include <new>
#include <time.h>
#include <signal.h>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <typeinfo>
#include <exception>
#include "TGTab.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TRandom.h"
#include "TGComboBox.h"
#include "TGNumberEntry.h"
#include "TString.h"
#include "TGSplitter.h"
#include "TGLabel.h"
#include "TGMenu.h"
#include "TObjArray.h"
#include "TRootEmbeddedCanvas.h"
#include "TGTextEntry.h"
#include "TGTextEdit.h"
#include "TGFileDialog.h"
#include "RQ_OBJECT.h"
#include "TMath.h"
#include "TGIcon.h"
#include "TMapFile.h"

#include "QwGUIMainDetector.h"
#include "QwGUILumiDetector.h"
#include "QwGUIInjector.h"
#include "QwGUICorrelationPlots.h"
#include "QwGUIHallCBeamline.h"


#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif /* __CINT__ */

 
class QwGUIMain : public TGMainFrame {

  RQ_OBJECT("QwGUIMain");

 private:

  static const Int_t kMaxMapFileSize;
  //!Every instantiated subsystem gets added to this object array, as a cleanup mechanism.
  TObjArray               SubSystemArray;

  //!Main detector sub system class
  QwGUIMainDetector      *MainDetSubSystem;
  QwGUILumiDetector      *LumiDetSubSystem;
  QwGUIInjector          *InjectorSubSystem;
  QwGUICorrelationPlots  *CorrelationSubSystem;
  QwGUIHallCBeamline     *HallCBeamlineSubSystem;

  //!Initial width of the application window set in main()
  Int_t                   dMWWidth;

  //!Initial height of the application window set in main()
  Int_t                   dMWHeight;

  //!Current run number
  Int_t                   dCurRun;

  //!Menu ID counter
  Int_t                   MCnt;

  //!The following two flags are used in process increment dialog boxes
  Bool_t                  dProcessing;
  Bool_t                  dProcessHalt;


  Bool_t                  dMapFileOpen;
  Bool_t                  dRunOpen;


  //!Command line argument structure (not currently implemented)
  ClineArgs               dClArgs;
  Char_t                  dDetMapFile[NAME_STR_MAX];
  TString                 dInjectorChannelMap;
  TString                 dHallCChannelMap;
  TString                 dMDChannelMap;
  TString                 dLumiChannelMap;

  //!ROOT file data container a wrapper class for many common file types
  //  RDataContainer         *dROOTFile;
  TMapFile               *dMemoryMapFile;



 
  //!Standard GUI widgets and layouts for the main window
  TGComboBox             *dTBinEntry;
  TGLayoutHints          *dTBinEntryLayout;
  TGNumberEntry          *dRunEntry;
  TGLayoutHints          *dRunEntryLayout;
  TGLabel                *dRunEntryLabel;
  TGHorizontal3DLine     *dHorizontal3DLine;
  TGHorizontalFrame      *dUtilityFrame;
  TGLayoutHints          *dUtilityLayout;

  //!Main window tab environment
  TGTab                  *dTab;
  TGLayoutHints          *dTabLayout;

  //!Main window canvas -- has its own permanent tab and can display
  //!plots that are always needed
  TRootEmbeddedCanvas    *dMainCanvas;
  TGCompositeFrame       *dMainCnvFrame;
  TGHorizontalFrame      *dMainTabFrame;
  TGLayoutHints          *dMainTabLayout;
  TGLayoutHints          *dMainCnvLayout;

  //!Main window log book environment
  TGTextEdit             *dLogEdit;
  TGHorizontalFrame      *dLogTabFrame;
  TGLayoutHints          *dLogTabLayout;
  TGLayoutHints          *dLogEditLayout;

  //!Menubar widgets
  TGMenuBar              *dMenuBar;
  TGPopupMenu            *dMenuTabs;
  TGPopupMenu            *dMenuLoadMap;
  TGPopupMenu            *dMenuHistoState;//this has options to reset and accumulate histograms
  TGLayoutHints          *dMenuBarLayout;
  TGLayoutHints          *dMenuBarItemLayout;
 
  
  //!This function is used to close the map file
  void                    CloseMapFile();

  //!This function is related to the SubSystemArray storage array and should be called
  //!before a new subsystem is instantiated. This mechanism prevents multiple instantiations of
  //!of the same subsystem. It can also be called to obtain the pointer to an existing subsystem.
  //!
  //!Parameters:
  //! - 1) Name of the subsystem (identical to the one that appears in the tab label)
  //!
  //!Return value: subsystem pointer
  QwGUISubSystem         *GetSubSystemPtr(const char*);


  //!This function looks for the index of a tab with the specified name (same as tab label).
  //!The tab index is used to add and/or remove tabs from the tab environment.
  //!
  //!Parameters:
  //! - 1) Name of label of the tab (must be allocated and filled before passing)
  //!
  //!Return value: Tab index
  Int_t                   GetTabIndex(const char *);

  //!This function implements the log book tab.
  void                    MakeLogTab();
  //!This function implements the main data window tab.
  void                    MakeMainTab();
  //!This function creates the menu layout (additions to the static menu should be made here)
  void                    MakeMenuLayout();
  //!This function creates the tool bar layout (changes to the tool bar should be made here)
  //  void                    MakeUtilityLayout();
  //!This function initiates the final tab layout after a new tab has been added.
  void                   MapLayout(){dTab->MapSubwindows(); dTab->Layout();}
  //!This function initiates the final tab layout after a tab has been removed.
  void                   UnMapLayout(Int_t tab){dTab->RemoveTab(tab); dTab->Layout();};

  
  Bool_t                   OpenMapFile();


  void                    SleepWithEvents(int seconds);


  //!This function checks to see if a tab with a certain name is already active.
  //!
  //!Parameters:
  //! - 1) Tab label container (must be allocated and of nonzero length)
  //!
  //!Return value: Boolean flag = kTrue if a tab of the given name exists, otherwise returns kFalse
  Bool_t                  TabActive(const char *);

  //!Parameters:mapfile name
  //!
  //!Return value: none
  void                   LoadChannelMapFiles(TString detfile);

 public:
  QwGUIMain(const TGWindow *p, ClineArgs clargs, UInt_t w, UInt_t h);
  virtual ~QwGUIMain();

  ///
  ///Each subsystem class must call the following function on
  ///construction, to add a tab for itself.
  ///What actually happens is that the QwGUISubSystem parent class has a function
  ///called AddThisTab(QwGUISubSystem*), which must be called by the derived subsystem
  ///class. This function uses the ROOT "Connect" mechanism to call the QwGUIMain function
  ///AddATab(QwGUISubSystem*), i.e. this function.
  ///
  ///The function adds a menu item to the tabs menu, which enables the
  ///user to show/hide the given subsytem tab. This function should never be
  ///called directly.
  ///It is recommended to use a unique label for the tab; one that is
  ///different from any of the other labels in the menu. This is done by supplying
  ///the subsystem label to the QwGUISubSystem class on construction of the corresponding
  ///object. So, the menu label (and the corresponding tab label) will have
  ///the same name as the subsystem itself. The subsystem classes are constructed in the
  ///QwGUIMain class constructor and one should use the GetSubSystemPtr(const char*) function
  ///to make sure that a subsystem with a given name doesn't already exist.
  ///One can run the compiled program to see which menu labels are already
  ///used.
  //!
  //!Parameters:
  //! - 1) sub system pointer
  //!
  //!Return value: Error value
  void                   AddATab(QwGUISubSystem*);

  //!This function is called when the main application closes it is an overwrite from
  //!from the ROOT TGMainFrame class and can be used to perform cleanup tasks before
  //!the application terminates.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: Error value
  virtual void           CloseWindow();

 
  //!This function return the current run number.
  Int_t                  GetCurrentRunNumber(){return dCurRun;};

  //!This function return a new unique tab menu ID. It is used each time a
  //!new subsystem is added to the list of tabs.
  Int_t                  GetNewTabMenuID() {MCnt++; return M_TABS+MCnt;}
 

  //! This function returns the menu ID for a menu entry corresponding to
  //! the tab with name "TabName"; needed when checking (showing) and
  //! unchecking (hiding) tabs.
  //!
  //!Parameters:
  //! - 1) Tab name container (must be allocated and filled before passing)
  //!
  //!Return value: tab ID integer
  Int_t                  GetTabMenuID(const char *TabName);

  //! This function returns the menu label for a menu entry corresponding to
  //! the tab with menu ID = mID; needed when checking (showing) and
  //! unchecking (hiding) tabs.
  //!
  //!Parameters:
  //! - 1) Menu ID integer
  //!
  //!Return value: tab name container pointer
  const char            *GetTabMenuLabel(Long_t mID);

  //!This function returns a true/false flag to indicate whether a map file is
  //!currently open.
  Bool_t                 IsMapFileOpen(){return dMapFileOpen;};


  //!This function can be used to process events that happen within the main window tab;
  //!such as mouse clicks and movement, etc...
  //!
  //!Parameters:
  //! - 1) Event type (such as kButton1Double defined in the ROOT TGMainFrame class)
  //! - 2) x location of the event, within the tab
  //! - 3) y location of the event, within the tab
  //! - 4) Pointer to a selected object, such as a ROOT canvas, or other ...
  //!
  //!Return value: none
  void                   MainTabEvent(Int_t,Int_t,Int_t,TObject*);

  //!Receiver function, called when a connected object is sending a message to the
  //!to the log. See QwGUISubSuystem class constructor for an example of how this is done.
  //!
  //!Parameters:
  //! - 1) Message string pointer (the string must be allocated and filled before passing)
  //!
  //!Return value: none
  void                   OnLogMessage(const char *);
  //!Receiver function, called when a connected object is being terminated.
  //!Used for cleanup purposes. All classes derived from QwGUISubSuystem are forced to
  //!send a signal to this receiver. See QwGUISubSuystem class constructor for an example
  //!of how this is done.
  //!
  //!Parameters:
  //! - 1) Object name string pointer (the string must be allocated and filled within the object/class
  //!      that sends the signal)
  //!
  //!Return value: none
  void                   OnObjClose(const char *);
  //!Receiver function, called when a connected object is sending a message to the
  //!to the log. See QwGUISubSuystem class constructor for an example of how this is done.
  //!
  //!Parameters:
  //! - 1) Object name string pointer (the string must be allocated and filled within the object/class
  //!      that sends the signal)
  //!
  //!Return value: none
  void                   OnReceiveMessage(const char *);

  //!This function is overwritten from the TGMainFrame class, to handle all mouse and keyboard interactions
  //!with any main window widget or menu item. Dialog box classes and other sub frames can handle
  //!their own interactions within their own ProcessMessage functions.
  //!
  //!Parameters:
  //! - 1) Message type, such as kC_TEXTENTRY or kC_COMMAND (which includes kCM_MENU), etc ...
  //! - 2) Widget identifier (the ID passed to the widget on instantiation or addition to a menu)
  //! - 3) not used
  //!
  //!Return value: none
  virtual Bool_t         ProcessMessage(Long_t msg, Long_t parm1, Long_t);


  ///This function is called to remove a tab.
  ///Each subsystem class must call this function on desstruction, to remove its tab.
  ///What actually happens is that the QwGUISubSystem parent class has a function
  ///called RemoveThisTab(QwGUISubSystem*), which must be called by the derived subsystem
  ///class. This function uses the ROOT "Connect" mechanism to call the QwGUIMain function
  ///RemoveTab(QwGUISubSystem*), i.e. this function.
  ///
  ///The function disables the correspondig menu item on the tabs menu, which enables the
  ///user to show/hide the given subsytem tab. This function should never be
  ///called directly.
  //!
  //!Parameters:
  //! - 1) sub system pointer
  //!
  //!Return value: Error value
  void                   RemoveTab(QwGUISubSystem*);

  ClassDef(QwGUIMain,0);
};

#endif


