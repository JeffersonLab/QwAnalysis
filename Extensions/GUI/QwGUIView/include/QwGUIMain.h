//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUIMain.h
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
//New Class "QwGUIBeamModulation"  created for Beam Modulation: Nuruzzaman 05/03/2010
//=============================================================================

///
/// \ingroup QwGUIMain


#ifndef QWGUIMAIN_H
#define QWGUIMAIN_H

#include <string.h>
#include <new>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <TGTab.h>

#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TGClient.h>
#include <TRandom.h>
//#include <TGComboBox.h>
#include "QwGUIComboBox.h"
#include <TGNumberEntry.h>
#include <TString.h>
#include <TGSplitter.h>
#include <TGLabel.h>
#include <TGMenu.h>
#include <TObjArray.h>
#include <TRootEmbeddedCanvas.h>
#include <TGTextEntry.h>
#include <TGTextEdit.h>
#include <TGFileDialog.h>
#include <RQ_OBJECT.h>
#include <TMath.h>
#include <TPave.h>
#include "KeySymbols.h"
#include "QwGUIMainDetector.h"
#include "QwGUIHistories.h"
#include "QwGUIScanner.h"
#include "QwGUIBeamModulation.h"
#include "QwGUILumiDetector.h" 
#include "QwGUIInjector.h"
#include "QwGUIHallCBeamline.h"
#include "QwGUITrackFinding.h"
#include "QwGUIEventDisplay.h"
#include "QwGUIHelpBrowser.h"
/* #include "QwGUIDatabaseContainer.h" */
/* #include "QwGUIDatabase.h" */
#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif /* __CINT__ */


class QwGUIMain : public TGMainFrame {

  /* RQ_OBJECT("QwGUIMain"); */

 private:

  //!Database object (there should be only one for all subsystems)
  /* QwGUIDatabaseContainer *dDatabase; */

  //!Every instantiated subsystem gets added to this object array, as a cleanup mechanism.
  TObjArray               SubSystemArray;

  //!Array to store and keep track of separate data plot windows for cleanup and communiation 
  TObjArray               DataWindowArray;

  //!Main detector sub system class
  QwGUIMainDetector      *MainDetSubSystem;
  QwGUIScanner           *ScannerSubSystem;
  QwGUIHistories         *HistoriesSubSystem;
  QwGUIBeamModulation    *BeamModulationSubSystem;
  QwGUILumiDetector      *LumiDetSubSystem;
  QwGUIInjector          *InjectorSubSystem;
  QwGUIHallCBeamline     *HallCBeamlineSubSystem;
  /* QwGUIDatabase          *DatabaseSubSystem; */
  QwGUITrackFinding      *TrackFindingSubSystem;
  QwGUIEventDisplay      *EventDisplaySubSystem;

  QwGUIHelpBrowser          *dHelpBrowser;

  //!Initial width of the application window set in main()
  Int_t                   dMWWidth;

  //!Initial height of the application window set in main()
  Int_t                   dMWHeight;

  //!Current run number
  Int_t                   dCurRun;

  //!Current run type
  RUNTYPE                 dCurRunType;

  //!Menu ID counter
  Int_t                   MCnt;

  Int_t                   dCurrentSegment;

  Double_t                dRasterSize[2];
  void                    SetRasterSize(Double_t rasterX, Double_t rasterY){dRasterSize[0] = rasterX; dRasterSize[1] = rasterY;}
  Double_t               *GetRasterSize(){return dRasterSize;}
  Double_t                dEnergy;
  void                    SetEnergy(Double_t energy) {dEnergy = energy;}
  Double_t                GetEnergy() {return dEnergy;}
  Double_t                dCurrent;
  void                    SetCurrent(Double_t current) {dCurrent = current;}
  Double_t                GetCurrent() {return dCurrent;}

  //!Index (in DataWindowArray) of the currently slected data window
  Int_t                dSelectedDataWindow;

  //!This should be used for proper clean up when a particular data window is closed by the
  //!user.
  UInt_t               dWinCnt;


  //!The following two flags are used in process increment dialog boxes
  Bool_t                  dProcessing;
  Bool_t                  dProcessHalt;

  //!File/Run open flags
  Bool_t                  dRootFileOpen;
  Bool_t                  dDatabaseOpen;
  Bool_t                  dLogFileOpen;
  Bool_t                  dRunOpen;
/*   Bool_t                  dAllSegments; */
  Bool_t                  dAddSegments;
  Bool_t                  dEventMode;
  Bool_t                  dMainPlots;

  Char_t                  dLogfilename[NAME_STR_MAX];//Name for Current log file
  Char_t                  dRootfilename[NAME_STR_MAX];//Name for Current root file

  //!Buffers used in message passing between classes and brief temporary storage
  Char_t                  dMiscbuffer[MSG_SIZE_MAX];
  Char_t                  dMiscbuffer2[MSG_SIZE_MAX];

  Char_t                  dTime[NAME_STR_MAX];
  Char_t                  dDate[NAME_STR_MAX];

  //!Command line argument structure (not currently implemented)
  ClineArgs               dClArgs;

  //!ROOT file data container a wrapper class for many common file types
  RDataContainer         *dROOTFile;

  //!Standard GUI widgets and layouts for the main window
  QwGUIComboBox          *dSegmentEntry;
  QwGUIComboBox          *dPrefixEntry;
  TGCheckButton          *dAddSegmentCheckButton;
  TGCheckButton          *dEventModeCheckButton;
  TGNumberEntry          *dRunEntry;
  TGLayoutHints          *dRunEntryLayout;
  TGLayoutHints          *dSegmentEntryLayout;
  TGLayoutHints          *dPrefixEntryLayout;
  TGLayoutHints          *dAddSegmentLayout;
  TGLabel                *dRunEntryLabel;
  TGLabel                *dAddSegmentLabel;  
  TGLabel                *dPrefixEntryLabel;
  TGLabel                *dRunInfoLabel;
  
  TGHorizontal3DLine     *dHorizontal3DLine;
  TGHorizontalFrame      *dUtilityFrame;
  TGHorizontalFrame      *dRunInfoFrame;
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
  TGText                 *dLogText;
  TGTextEdit             *dLogEdit;
  TGVerticalFrame        *dLogTabFrame;
  TGHorizontalFrame      *dLogEditFrame;
  TGLayoutHints          *dLogTabLayout;
  TGLayoutHints          *dLogEditLayout;
  TGHorizontalFrame      *dDBQueryFrame;
  TGTextEntry            *dDBQueryEntry;
  TGLayoutHints          *dLogEditFrameLayout;
  TGLayoutHints          *dDBQueryEntryLayout;
  TGLayoutHints          *dDBQueryFrameLayout;
  TGTextBuffer           *dDBQueryBuffer;
  TGLabel                *dDBQueryLabel;
  TGLayoutHints          *dDBQueryLabelLayout;
  TGTextButton           *dHCEntryButton;
  QwGUIHCLogEntryDialog  *dHCLogEntryDlg;
  HCLogEntry              dHCLogEntries;
  TGLabel                *dHCLogEntryLabel;

  //!Menubar widgets
  TGMenuBar              *dMenuBar;
  TGPopupMenu            *dMenuFile;
  TGPopupMenu            *dMenuTabs;
  TGPopupMenu            *dMenuHelp;
  TGLayoutHints          *dMenuBarLayout;
  TGLayoutHints          *dMenuBarItemLayout;
  TGLayoutHints          *dMenuBarHelpLayout;

  TString                 fPrefix;
  TString                 fDirectory;

  vector <int>            dRunSegments;
  vector <TString>        dFilePrefix;
  vector <TH1F*>          dMainHistos;
  vector <TGraph*>        dMainGraphs;
  vector <TObject*>       dMainPlotsArray;
  vector <TH1F*>          dHistoryPlotsArray;
  vector <TBox*>          dErrorBoxArray;

  EventOptions            dCurrentRunEventOptions;

  //!This function is used to append new messages to the log book. It cannot
  //!be used from other classes. Instead, the QwGUISubSystem class implements the
  //!SendMessageSignal() and GetMessage() connection mechanism and derived subsystem classes
  //!use SetLogMessage() to pass messages to the log (see QwGUIMainDetector class for an example).
  //!
  //!Parameters:
  //! - 1) Text to be added to the log (must be allocated and filled prior to passing)
  //! - 2) Flag to specify whether the time of the log message should be indicated
  //!
  //!Return value: Error value;
  Int_t                   Append(const char *buffer,Bool_t T_Stamp = kFalse);

  //!This function is used via the program menu and should not be called directly.
  void                    CloseLogFile();
  //!This function is used via the program menu and should not be called directly.
  void                    CloseRootFile();
  //!This function is used via the program menu and should not be called directly.
  void                    CloseDatabase();

  //!This function is related to the SubSystemArray storage array and should be called
  //!before a new subsystem is instantiated. This mechanism prevents multiple instantiations of
  //!of the same subsystem. It can also be called to obtain the pointer to an existing subsystem.
  //!
  //!Parameters:
  //! - 1) Name of the subsystem (identical to the one that appears in the tab label)
  //!
  //!Return value: subsystem pointer
  QwGUISubSystem         *GetSubSystemPtr(const char*);

  //!This function returns the name of a file to be openend/saved, as obtained from user input
  //!via a dialog box. This function does not open the file.
  //!
  //!Parameters:
  //! - 1) Filename container (must be allocated before passing to function)
  //! - 2) File extension container (must be allocated and specify desired file type extension
  //!                             valid extensions are: .root, .dat, .txt, .csv, and .log
  //! - 3) File status:
  //!                - FS_NEW (new file which has never been saved before -- dialog box is opened
  //!                         as a "save as" box ...)
  //!                - FS_OLD (file was previously saved -- dialog box is opened
  //!                        as a standard "file open" box ...)
  //! - 4) Specifies whether to notify user of file operation and give a continue/cancel option
  //!   dialog box. Argument 5 must be allocated when this is set to kTrue.
  //! - 5) Notification text container to be shown when parameter 4 is set to kTrue.
  //!
  //!Return value: Error value.
  Int_t                   GetFilenameFromDialog(char *, const char *,
						ERFileStatus status = FS_OLD,
						Bool_t kNotify = kFalse,
						const char *notifytext = NULL);

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
  void                    MakeUtilityLayout();
  //!This function initiates the final tab layout after a new tab has been added.
  void                   MapLayout(){dTab->MapSubwindows(); dTab->Layout();}
  //!This function initiates the final tab layout after a tab has been removed.
  void                   UnMapLayout(Int_t tab){dTab->RemoveTab(tab); dTab->Layout();};

  //!This function opens a new log file. If there is no log tab, a new tab is created automatically
  //!
  //!Parameters:
  //! - 1) File status: Only used if parameter 2 is NULL. In that case the file status is passed on
  //!                   to the function GetFilenameFromDialog(...).
  //! - 2) Filename container: If this is NULL, then the filename is obtained from a dialog box
  //!                          entry by calling the function GetFilenameFromDialog(...).
  //!
  //!Return value: Error value;
  Int_t                   OpenLogFile(ERFileStatus status = FS_OLD, const char* file = NULL);

  //!This function is called via menu selection.
  //!It opens a new root file. If no file name is specified, the function asks the
  //!user for a filename via the GetFilenameFromDialog(...) function. The function instantiates
  //!a new generic data container from the class RDataContainer. This container is passed to all
  //!instantiated susbsystems and allows each of them to read the data specific to the subsystem.
  //!Currently only one root file can be open at a time, and the function also disables the menu
  //!entry for this function.
  //!
  //!Parameters:
  //! - 1) Event Mode: If this is set to kTrue, then the individual tree events are read, from the  
  //!                  outset. Otherwise only histograms are read. 
  //! - 2) File status: Only used if parameter 3 is NULL. In that case the file status is passed on
  //!                   to the function GetFilenameFromDialog(...).
  //! - 3) Filename container: If this is NULL, then the filename is obtained from a dialog box
  //!                          entry by calling the function GetFilenameFromDialog(...).
  //!
  //!Return value: Error value;
  Int_t                   OpenRootFile(Bool_t EventMode = kFalse, ERFileStatus status = FS_OLD, const char* file = NULL);

  //!This function is called when a run number is entered in the menu, for fast run access.
  //!It opens the root file that has the name with the entered run number, if it can be found. 
  //!The function only opens the ROOT file in histogram mode. The function instantiates
  //!a new generic data container from the class RDataContainer. This container is passed to all
  //!instantiated susbsystems and allows each of them to read the data specific to the subsystem.
  //!Currently only one root file can be open at a time, and the function also disables the menu
  //!entry for this function.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: Error value;
  Int_t                   OpenRun();

  //!This function establishes a single connection to the database.
  //!
  //!
  //!Parameters: None
  //!
  //!Return value: Error value;
  Int_t OpenDatabase();

  //!This function removes the log book tab, when the correpsonding menu item is selected. The
  //!data in the current log book is not lost or closed when the tab is removed.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                    RemoveLogTab();

  //!This function can be used to save a log file, with or without the use of a file dialog box.
  //!
  //!Parameters:
  //! - 1) File status: Only used if parameter 2 is NULL. In that case the file status is passed on
  //!                   to the function GetFilenameFromDialog(...).
  //! - 2) Filename container: If this is NULL, then the filename is obtained from a dialog box
  //!                          entry by calling the function GetFilenameFromDialog(...).
  //!
  //!Return value: Error value;
  Int_t                   SaveLogFile(ERFileStatus status = FS_OLD, const char* file = NULL);


  //!This function is used to set the currently open log book file name, when it is saved or opened
  //!from an existing file.
  //!
  //!Parameters:
  //! - 1) Filename container
  //!
  //!Return value: none
  void                    SetLogFileName(const char *name){strcpy(dLogfilename,name);};

  //!This function sets the log file open or closed flag.
  //!
  //!Parameters:
  //! - 1) Boolean Open/Close Flag
  //!
  //!Return value: none
  void                    SetLogFileOpen(Bool_t open = kFalse){dLogFileOpen = open;};

  //!This function is used to set the current root file name, when it is saved or opened from an existing
  //!file.
  //!
  //!Parameters:
  //! - 1) Filename container
  //!
  //!Return value: none
  void                    SetRootFileName(char *name){strcpy(dRootfilename,name);};

  //!This function sets the root file open or closed flag.
  //!
  //!Parameters:
  //! - 1) Boolean Open/Close Flag
  //!
  //!Return value: none
  void                    SetRootFileOpen(Bool_t open = kFalse){dRootFileOpen = open;};

  //!This function sets the database open or closed flag.
  //!
  //!Parameters:
  //! - 1) Boolean Open/Close Flag
  //!
  //!Return value: none
  void                    SetDatabaseOpen(Bool_t open = kFalse){dDatabaseOpen = open;};

  void                    SleepWithEvents(int seconds);
  TCanvas                *SplitCanvas(TRootEmbeddedCanvas *,int,int,const char*);

  void                    SetSubSystemSegmentAdd(Bool_t add);
/*   void                    SetReadAllRunSegments(Bool_t all) {dAllSegments = all;}; */
/*   Bool_t                  ReadAllRunSegments(){return dAllSegments;}; */

  void                    SetAddSegments(Bool_t add) {dAddSegments = add;};
  Bool_t                  AddSegments() {return dAddSegments;};

  void                    SetCurrentRunSegment(Int_t seg){dCurrentSegment = seg;};
  Int_t                   GetCurrentRunSegment() {return dCurrentSegment;};
  Int_t                   GetRunSegment(UInt_t n) {if(n >= 0 && n < dRunSegments.size()) return dRunSegments[n]; 
                                                   return -1;};

  void                    SetCurrentRunNumber(Int_t run) {dCurRun = run;};
  const char             *GetCurrentFilePrefix(){return fPrefix.Data();};
  const char             *GetCurrentFileDirectory(){return fDirectory.Data();};

  void                    SetCurrentFilePrefix(const char* prefix){ fPrefix = prefix;};
  void                    SetCurrentFileDirectory(const char* dir){fDirectory = dir;};
  void                    StoreFileInfo(const char* filename);
  void                    GetFileInfo(const char *filename, int &run, int &segment);

  UInt_t                  GetCurrentRunEventStart(){return dCurrentRunEventOptions.Start;};
  UInt_t                  GetCurrentRunEventLength(){return dCurrentRunEventOptions.Length;};
  UInt_t                  GetCurrentRunEventStop(){return dCurrentRunEventOptions.Start+dCurrentRunEventOptions.Length-1;};

  void                    SetEventMode(Bool_t evM){dEventMode = evM;};
  Bool_t                  EventMode() {return dEventMode;};

  Int_t                   FindFileAndSegments();
  void                    LoopOverRunSegments();

  void                    PlotMainData();
  void                    SubmitToHCLog();
  

  //!This function checks to see if a tab with a certain name is already active.
  //!
  //!Parameters:
  //! - 1) Tab label container (must be allocated and of nonzero length)
  //!
  //!Return value: Boolean flag = kTrue if a tab of the given name exists, otherwise returns kFalse
  Bool_t                  TabActive(const char *);

  //!If the current text in the log book tab editor is not already saved under a filename, then
  //!function obtains a new filename and saves the text to the file with that name. Otherwise the
  //!the text is written to the file that is currently open.
  //!
  //!Parameters:
  //! - 1) Filename container
  //!
  //!Return value: Error value
  Int_t                   WriteLogData(const char*);

  //!If the current text in the log book tab editor is not already saved under a filename, then
  //!function obtains a new filename and saves the text to the file with that name. Otherwise the
  //!the text is written to the file that is currently open.
  //!
  //!Parameters:
  //! - 1) Filename container
  //!
  //!Return value: Error value

  //!Not yet implemented.
  Int_t                   WriteRootData();

  TString     WrapParameter(TString param, TString value){
    TString tmp("--");
    tmp += param + "=\"" + value + "\" ";
    return tmp;
  }
  TString     WrapAttachment(TString filename){
    TString tmp("--attachment=\"");
    tmp += filename + "\" ";
    return tmp;
  }

  TString     MakeSubject(TString subject){
    TString tmp("Analysis: ");
    tmp += Form("Run %6d - ",GetCurrentRunNumber()) + subject;
    return tmp;
  } 


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

  //!This function returns the current date.
  Char_t                *GetDate();
  //!This function return the current run number.
  Int_t                  GetCurrentRunNumber(){return dCurRun;};
  //!This function return the current log book file name.
  char*                  GetLogFileName(){return dLogfilename;};
  //!This function return a new unique tab menu ID. It is used each time a
  //!new subsystem is added to the list of tabs.
  Int_t                  GetNewTabMenuID() {MCnt++; return M_TABS+MCnt;}
  //!This function return the current root file name.
  char*                  GetRootFileName(){return dRootfilename;};

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

  //!This function returns the current time.
  Char_t                *GetTime();

  //!This function returns a true/false flag to indicate whether a log file is
  //!currently open.
  Bool_t                 IsLogFileOpen(){return dLogFileOpen;};
  //!This function returns a true/false flag to indicate whether a root file is
  //!currently open.
  Bool_t                 IsRootFileOpen(){return dRootFileOpen;};
  //!This function returns a true/false flag to indicate whether a root file is
  //!currently open.
  Bool_t                 IsDatabaseOpen(){return dDatabaseOpen;};

  //!Signal receiver function, called when the log file is closed via the context menu provided
  //!by the TGTextEdit object.
  //!Sets the dLogFileOpen to false and removes the filename string from variable dLogfilename.
  void                   LogClosed();
  //!Signal receiver function, called when a log file is opened via the context menu provided
  //!by the TGTextEdit object.
  //!Sets the dLogFileOpen to true and adds the filename to variable dLogfilename, unless
  //!the file is new and unnamed. The filenames are obtained from the TGTextEdit object.
  void                   LogOpened();
  //!Signal receiver function, called when a log file is saved via the context menu provided
  //!by the TGTextEdit object.
  //!Sets the dLogFileOpen to true and adds the filename to variable dLogfilename.
  //!The filenames are obtained from the TGTextEdit object.
  void                   LogSaved();
  //!Signal receiver function, called when a log file is saved via the context menu.
  //!Sets the dLogFileOpen to true and adds the NEW filename to variable dLogfilename.
  //!The filenames are obtained from the TGTextEdit object.
  void                   LogSavedAs();

  void                   MonitorLogInput();

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

  void                   OnUpdatePlot(const char*);

  void                   OnNewRunSignal(int sig);
  void                   OnRunWarningSignal(int sig);

  //!Receiver function, called when a connected canvas pad is mouse selected.
  //!
  //!Parameters:
  //! - 1) Pointer to the selected pad within the canvas
  //! - 2) Pointer to the selected object within the selected pad
  //! - 3) Event type integer (such as kButton1Double defined in the ROOT TGMainFrame class)
  //!
  //!Return value: none
  void                   PadIsPicked(TPad* selpad, TObject* selected, Int_t event);

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

  virtual Bool_t         HandleKey(Event_t *event);

  void                 SetSelectedDataWindow(Int_t ind) {dSelectedDataWindow = ind;};
  void                 RemoveSelectedDataWindow() {dSelectedDataWindow = -1;};
  QwGUIDataWindow     *GetSelectedDataWindow(); 
  void                 CleanUpDataWindows();
  UInt_t               GetNewWindowCount(){ return ++dWinCnt;};


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

  //!Not currently used!
  void                   WritePid();
  void                   CheckForNewRun();

  ClassDef(QwGUIMain,0);
};

#endif


