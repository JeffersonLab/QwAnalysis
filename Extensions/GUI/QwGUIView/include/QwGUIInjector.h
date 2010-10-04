//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIInjector.h
   \author Michael Gericke
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
   \class QwGUIInjector
    
   \brief Handles the display of the lumi detector data.  

   The QwGUIInjector class handles the display of the lumi detector data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 
Added by Buddhini to display the injector beamline data.

 */
//=============================================================================

// There are 22 BPMs in the hallC beamline  
#define INJECTOR_BPMS   21 
#define INJECTOR_BCMS    1     
#define TRE_NUM          2
///
/// \ingroup QwGUIInjector

#ifndef QWGUINJECTOR_H
#define QWGUINJECTOR_H


#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <string>
#include <TPaveText.h>

#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "QwGUISubSystem.h"
#include "TStyle.h"
#include "RSDataWindow.h"


 class QwGUIInjector : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dBtnPosDiff;
  TGTextButton        *dBtnChargeAsym;
  TGTextButton        *dBtnPlotHistos;
  TGComboBox          *dCmbHistos;
  TGLayoutHints       *dCmbLayout;

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //A histogram array to plot the X and Y position difference variation.
  TH1D *PosDiffVar[2] ;
  // The histogram to store the charge asymmetries.
  TH1D *ChargeAsym ;
  TPaveText * errlabel;

  void PositionDifferences(); 
  void DisplayChargeAsymmetry();
  void PlotHistograms();


  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void  ClearData();

  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.

  static const char   *Injector_BPMS[INJECTOR_BPMS];
  static const char   *Injector_BCMS[INJECTOR_BCMS]; 
  //The Devices with only charge may later have to be distinguished from those with other data
  static const char   *RootTrees[TRE_NUM];
  
 private:

  char histo[128];
  TString select;

  Bool_t  ReadyHistArray;
  void EnableButtons(Bool_t flag);

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

  void                 SummaryHist(TH1*in);



 public:
  
  QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		     const char *objName, const char *mainname, UInt_t w, UInt_t h);

  ~QwGUIInjector();


  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  virtual void        OnNewDataContainer(RDataContainer *cont);
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);
  
  ClassDef(QwGUIInjector,0); 

};

#endif
