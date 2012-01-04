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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>


#include "QwColor.h"

#include "TMapFile.h"
#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "TCut.h"
#include "TTree.h"
#include "TGComboBox.h"
#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TGLabel.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TF1.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TNamed.h"
#include "TString.h"
#include "TQObject.h"
#include "TGTab.h"
#include "TH1.h"
#include "TH2.h"
#include "TPaveText.h"
#include "TMath.h"
#include "TPaveText.h"


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

  TString          dMainName;
  TString          dThisName;
 
  //!The tab menu ID associated with this subsystem
  Long_t           dTabMenuID;   
  

  //!Main window object reference
  TGWindow        *dMain;
  //!Parent window object reference (usually gRoot->Client ...)
  TGWindow        *dParent;

 protected:

  //!histo modes
  Int_t dHistoReset;
  Int_t dHistoAccum;
  Int_t dHistoPause;
  Int_t dHistoAutoReset;
 

  TMapFile *dMapFile;
 

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
  
  //!This function is called by the QwGUIMain::AddATab function once each time a new subsystem tab is created
  //!(not when it is simply toggled on/off). It in turn initiates the actual layout of the subsystem tab.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none    
  void             SubSystemLayout(){this->MakeLayout();};

  
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


  //!Set histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters:
  //! - 1) Histo reset
  //!Return value: none
  void             SetHistoReset(Int_t historeset){
    dHistoReset=historeset;
  };

 //!Set histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters:
  //! - 1) Histo Auto reset entries
  //!Return value: none
  void             SetAutoHistoReset(Int_t histoautoreset){
    dHistoAutoReset=histoautoreset;
  };
  void             UpdateAutoHistoReset(Int_t histoentries){
    if (histoentries>0 && dHistoAutoReset && histoentries>dHistoAutoReset){//trigger histogram reset
      SetHistoReset(1);
      SetHistoAccumulate(0);
      SetHistoPause(0);
    }
  };

  //!Set histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters:
  //! - 1) Histo Accum
  //!Return value: none
  void             SetHistoAccumulate(Int_t histoaccum){
    dHistoAccum=histoaccum;
  }; 

  //!Set histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters:
  //! - 1) Histo pause
  //!Return value: none
  void             SetHistoPause(Int_t histopause){
    dHistoPause=histopause;
  };  

  //!Set histomode to accumulate. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters:
  //! - 1) Histo none
  //!Return value: none
  void             SetHistoDefaultMode(){
    SetHistoReset(0);
    SetHistoAccumulate(1);
    SetHistoPause(0); 
  };      
 
  //!Get histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters: none
  //!Return value: Histo Reset status
  Int_t            GetHistoReset(){
    return dHistoReset;
  };

  //!Get histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters: none
  //!Return value: Histo accumu status
  Int_t            GetHistoAccumulate(){
    return dHistoAccum;
  };

  //!Set histomode. This is called at QwGUIMain and other subsystems
  //!
  //!Parameters: none
  //!Return value: Histo pause status
  Int_t             GetHistoPause(){
    return dHistoPause;
  };
  
 
  virtual void   SetMapFile(TMapFile *file);
  virtual void   SummaryHist(TH1*in);
  Bool_t dMapFileFlag;

  ClassDef(QwGUISubSystem, 1);
};

#endif
