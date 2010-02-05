//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUISubSystem.h
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
   \class QwGUISubSystem
    
   \brief Virtual base class implements required routines for the display of the any subsystem data.  

   The QwGUISubSystem class implements the basic required routines required for the display of 
   subsystem data. It mainly implements functions that facilitate the proper interaction with the
   main application window as well as file access.
    
 */
//=============================================================================

///
/// \ingroup QwGUIMain

#ifndef QWGUISUBSYSTEM_H
#define QWGUISUBSYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TNamed.h>
#include <TString.h>
#include <TQObject.h>
#include <TGTab.h>
#include "RDataContainer.h"
#include "QwGUIMainDef.h"

class QwGUISubSystem : public TGCompositeFrame {

 private:

  //!Main window width is passed to and set in the constructor: Needed for tab frame layout
  UInt_t           dWidth;
  //!Main window height is passed to and set in the constructor: Needed for tab frame layout
  UInt_t           dHeight;
  //!Used to provide a unique label/name for each separate data window (set in the constructor).
  //!This should be used for proper clean up when a particular data window is closed by the
  //!user.
  UInt_t           dWinCnt;

  //!The name/label of the main window object
  char             dMainName[NAME_STR_MAX];
  //!The name/lable of this subsystem object, as passed to the constructor
  char             dThisName[NAME_STR_MAX];

  //!The tab menu ID associated with this subsystem
  Long_t           dTabMenuID;   
  
  //!Flag indicates whether the menu item belonging to this subsystem is checked/active.
  //!In other words, is this tab currently visible or not?
  Bool_t           dTabMenuItemChecked;

  //!Flag indicates whether a log message should be displayed with the current time and date.  
  Bool_t           dLogTStampFlag;

  //!Main window object reference
  TGWindow        *dMain;
  //!Parent window object reference (usually gRoot->Client ...)
  TGWindow        *dParent;

 protected:

  //!Buffer, mainly used in message passing and for other temporary storage.
  char             dMiscbuffer[MSG_SIZE_MAX];
  //!Buffer, mainly used in message passing and for other temporary storage.
  char             dMiscbuffer2[MSG_SIZE_MAX];

  //!This is a data container reference, it contains the root file and provides a series of 
  //!convenience access functions. The pointer is set by the QwGUIMain class, when a new ROOT file is opened,
  //!by calling the member function SetDataContainer(RDataContainer *cont) in this class.  There is no 
  //!direct instance of this container kept within the class. 
  RDataContainer  *dROOTCont;

  //!This function must be defined by the derived class, to implement the overal layout of the subsystem class.
  //!For example, this is where the derived class would implement the display of graphs (see QwGUIMainDetector 
  //!class as an example). The function is called when the function SubSystemLayout() is called by the QwGUIMain 
  //!class when the given subsystem is being created for the first time. Layouts and tab contents are not changed
  //!when the tab is simply toggled on/off via the menu. Changes on the data contents that are to be performed
  //!when the tab is toggled on/off need to be implemented by the derived class in the functions OnRemoveThisTab().
  //!and OnAddThisTab().
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  virtual void     MakeLayout() = 0;

  //!This function can be overwritten by the derived class, to perform cleanup tasks, when the subsystem 
  //!tab is toggled on to off.
  //!The function is called everytime the given subsystem tab is
  //!removed (whether it is being destroyed entirely, or just toggled on to off).  
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  virtual void     OnRemoveThisTab(){};

  //!This function can be overwritten by the derived class, to perform additional tasks, when the subsystem 
  //!tab is toggled off to on or created.
  //!The function is called everytime the given subsystem tab is
  //!added (whether it is being created for the first time, or just toggled off to on).  
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  virtual void     OnAddThisTab(){};

  
  //!This function can be overwritten by the derived class, to perform additional tasks, when the use has  
  //!opened a new data file (any file). It is the responsibility of the derived class implmentation to 
  //!identify what type of file was opened.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  virtual void     OnNewDataContainer(){};

  //!This function can be used to pass messages to the log book. The message content is 
  //!reformated slightly and copied to the dMiscbuffer member, to be picked up by the
  //!QwGUIMain class via the GetMessage() member function of this class, when QwGUIMain
  //!receives the message signal from the SendMessageSignal(const char*objname) function 
  //!in this class.
  //!
  //!Parameters:
  //! - 1) Message buffer: Can be passed as a constant string or a preallocated buffer. 
  //! - 2) Flag: To indicate whether the message is to be displayed with a time and date stamp.
  //!
  //!Return value: none  
  void             SetLogMessage(const char *buffer, Bool_t tStamp = kFalse);

 public:
  
  QwGUISubSystem(const TGWindow *p, const TGWindow *main, const TGTab *tab, 
		 const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUISubSystem();

  //!Sender function which connects to QwGUIMain::AddATab(QwGUISubSystem*), which in turn calls
  //!the SubSystemLayout() member function of this class, to implement the tab layout via MakeLayout();
  //!This function should only be called in the constructor of this class. 
  //!
  //!Parameters:
  //! - 1) Subsystem pointer.
  //!
  //!Return value: none  
  void             AddThisTab(QwGUISubSystem*);

  //!Getter function for the main window height (dHeight).
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: Height in pixels.  
  UInt_t           GetHeight(){return dHeight;};

  //!This function is called by QwGUIMain, to retrieve the message that was set by this subsystem, 
  //!when it receives a new log message signal (see function SetLogMessage). The message container
  //!must be allocated with in the subsystem class. This happens automatically if the subsystem 
  //!class is derived from QwGUISubSystem.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: message string pointer.  
  const char*      GetMessage() {return dMiscbuffer;};

  //!This function returns the name/label of this subsystem object.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: message string pointer.    
  const char*      GetName(){return dThisName;};

  //!Getter function for the main window parent pointer (usually gRoot->Client). 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: Window object pointer  
  TGWindow*        GetParent() {return dParent;};


  //!Getter function for the menu ID that corresponds to this subsystem tab menu item. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: Menu ID
  Long_t           GetTabMenuID(){return dTabMenuID;};

  //!Getter function for the main window width (dWidth). 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: Width in pixels.  
  UInt_t           GetWidth(){return dWidth;};

  //!This function returns a new data window count. It is called when a data window name is requested 
  //!and is used to provide a unique name for each data window.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: A new window count number  
  UInt_t           GetNewWindowCount(){ return ++dWinCnt;};

  //!This function returns a new data window name. It is called when a new data window is opened 
  //!(which is usually the result of double clicking on a graph or something like that) and is
  //!used to provide a unique name for each data window.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: A new window name
  const char*      GetNewWindowName();

  //!This function returns a flag indicating whether a message to be placed in the log is to have
  //!a time and date stamp or not. The function is called by QwGUIMain before adding the message to
  //!the log.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: boolean time stamp flag
  Bool_t           IfTimeStamp(){return dLogTStampFlag;}

  //!This function returns a flag indicating whether the tab menu entry for this subsystem is currently
  //!checked (i.e. whether the subsystem tab is visible or not).
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: boolean menu item flag
  Bool_t           IsTabMenuEntryChecked() {return dTabMenuItemChecked;};

  //!Sender function which connects to QwGUIMain::OnObjClose(const char*), to perform cleanup tasks
  //!when this subsystem object is destroyed. 
  //!
  //!Parameters:
  //! - 1) Subsystem object name/lable: must be the same as the one passed to the constructor dThisName.
  //!
  //!Return value: none  
  void             IsClosing(const char *objname);


  //!This revceiver function can be overwritten by the derived subsystem class to connect to another object's 
  //!IsClosing member function (a new data container or window for example), if it is anticipated that there are
  //!cleanup tasks to perform when this object is destroyed. 
  //!
  //!Parameters:
  //! - 1) Subsystem object name/label: will the same as the one passed to the constructor of the object.
  //!      (see QwGUIMainDetector for an example).
  //!
  //!Return value: none  
  virtual void     OnObjClose(char *) {};

  //!This revceiver function can be overwritten by the derived subsystem class to connect to another object's 
  //!SendMessageSignal member function (a new data container or window for example), if it is anticipated that 
  //!there are messages or commands to be processed. 
  //!
  //!Parameters:
  //! - 1) Subsystem object name/label: will the same as the one passed to the constructor of the object.
  //!      (see QwGUIMainDetector for an example).
  //!
  //!Return value: none  
  virtual void     OnReceiveMessage(char*) {};

  //!This revceiver function can be overwritten by the derived subsystem class to connect to a canvas object 
  //!to process mouse events. This is usefull in opening a particular plot in the standard ROOT canvas window,
  //!when double clicking on a graph, for example. 
  //!
  //!Parameters:
  //! - 1) Event type, such as kButton1Double
  //! - 2) Event x coordinate
  //! - 3) Event y coordinate
  //! - 4) Pointer to the selected object (i.e. pad, etc ...)
  //!
  //!Return value: none  
  virtual void     TabEvent(Int_t,Int_t,Int_t,TObject*) {};

  //!This function is overwritten from the TGTransientFrame class, to handle all mouse and keyboard interactions
  //!with any widget within this tab.  
  //!
  //!Parameters:
  //! - 1) Message type, such as kC_TEXTENTRY or kC_COMMAND (which includes kCM_MENU), etc ...
  //! - 2) Widget identifier (the ID passed to the widget on instantiation or addition to a menu) 
  //! - 3) not used
  //!
  //!Return value: none  
  virtual Bool_t   ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  
  //!Sender function which connects to QwGUIMain::RemoveTab(QwGUISubSystem*) and removes the subsystem tab
  //!This function should only be called in the destructor of this class. 
  //!
  //!Parameters:
  //! - 1) Subsystem pointer.
  //!
  //!Return value: none  
  void             RemoveThisTab(QwGUISubSystem*);

  //!This function is called by the QwGUIMain::AddATab function once each time a new subsystem tab is created
  //!(not when it is simply toggled on/off). It in turn initiates the actual layout of the subsystem tab.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none    
  void             SubSystemLayout(){this->MakeLayout();};


  //!This function is called by the QwGUIMain::AddATab function when the user has  
  //!opened a new data file (any file). The actual ownership of the file container is kept in the QwGUIMain class.
  //!
  //!Parameters:
  //! - 1) A pointer to the data container, containing the newly opened file.
  //!
  //!Return value: none
  void             SetDataContainer(RDataContainer *cont);

  //!Setter function for the subsystem tab menu ID. Called once from QwGUIMain::AddATab, when a new subsystem is
  //!is created.
  //!
  //!Parameters:
  //! - 1) The menu ID.
  //!
  //!Return value: none
  void             SetTabMenuID(Long_t mID){dTabMenuID = mID;};

  //!Sender function, which connects to QwGUIMain::OnReceiveMessage(const char*) to let the main class know that
  //!this subsystem wants to add a message to the log.
  //!
  //!Parameters:
  //! - 1) Subsystem object name/label: will the same as the one passed to the constructor of the object.
  //!      (see QwGUIMainDetector for an example).
  //!
  //!Return value: none
  void             SendMessageSignal(const char*objname);

  //!This function is called from QwGUIMain::AddATab or QwGUIMain::RemoveTab, to set the flag indicating
  //!whether the subsystem tab is displayed (on/off) or not. The function also calls the corresponding
  //!functions to implement additional tasks on toggle on/off.
  //!
  //!Parameters:
  //! - 1) Boolean flag indicating whether the subsystem tab is visible or not.
  //!
  //!Return value: none 
  void             TabMenuEntryChecked(Bool_t set) {dTabMenuItemChecked = set; 
    dTabMenuItemChecked ? OnAddThisTab() : OnRemoveThisTab();};
  
  ClassDef(QwGUISubSystem,0);
};

#endif
