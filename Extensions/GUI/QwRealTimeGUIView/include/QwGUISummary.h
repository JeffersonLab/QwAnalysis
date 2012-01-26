//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUISummary.h
   \author Michael Gericke
   \author Rakitha Beminiwattha
   \author Buddhini Waidyawansa
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUISummary
    
   \brief Handles the display of five basic parameters
       diff_qwk_targetX   
       diff_qwk_targetY   
       asym_qwk_charge   
       asym_qwk_md_allbars   
       diff_qwk_bpm3c12X   
       events with non-zero ErrorFlag for all of the above parameters  

 */
//=============================================================================


#define N_CH      7
#define N_CH_ERR  8

///
/// \ingroup QwGUISummary

#ifndef QWGUISUMMARY_H
#define QWGUISUMMARY_H


#include "QwGUISubSystem.h"
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <TString.h>
#include <TString.h>
#include <TGraph.h>

#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif /* __CINT__ */



class QwGUISummary : public QwGUISubSystem {

  TGHorizontalFrame   *dTabFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGTabElement        *dtab;

 public:


 private:
  static const Int_t fSleepTimeMS;

 protected:

  //!Overwritten virtual function from QwGUISubSystem::MakeLayout(). This function simply adds an
  //!embeded canvas to the tab frame for this subsystem, but almost anything can be done here ...
  //!See QwGUISubSystem::MakeLayout() for more information.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  virtual void         MakeLayout();

 public:
  
  QwGUISummary(const TGWindow *p, const TGWindow *main, const TGTab *tab,
	       const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUISummary();

  //!An array to store the device channels we want to monitor in the summary tab   
  static const char *fChannels[N_CH];

  //!An array to store the device channels we want to monitor in the error summary plot   
  static const char *fChannelsCheckError[N_CH_ERR];

 
  virtual void        OnReceiveMessage(char*);

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);

  void PlotMainData();

  ClassDef(QwGUISummary,0);
};

#endif
