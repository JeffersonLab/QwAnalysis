//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIInjector.h
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
   \class QwGUIInjector
    
   \brief Handles the display of the lumi detector data.  

   The QwGUIInjector class handles the display of the lumi detector data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 
Added by Buddhini to display the injector beamline data.

 */
//=============================================================================

#define INJECTOR_DET_TYPES        6
#define NUM_POS 2
///
/// \ingroup QwGUIInjector

#ifndef QWGUIINJECTOR_H
#define QWGUIINJECTOR_H


#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <cstring>

//#include "TMapFile.h"

#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "QwGUISubSystem.h"

#include "RSDataWindow.h"


#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif



class QwGUIInjector : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TGVerticalFrame     *dInjectorBCMFrame; //Injector bcm access frame
  TGVerticalFrame     *dInjectorSCALERFrame; //Injector scaler access frame
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dButtonPos;
  TGTextButton        *dButtonCharge;
  TGTextButton        *dButtonPosVariation;
  TGTextButton        *dButtonInjectorSCALER;
  TGTextButton        *dButtonMeanPos;

  TGComboBox          *dComboBoxInjectorBCM;
  TGComboBox          *dComboBoxInjectorSCALER;

   //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  TH1F *PosVariation[2] ;


  

  //!This function Draws a sample  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PositionDifferences();

  //!This function Draws a sample  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotBPMAsym();

  //!This function Draws a sample  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotChargeAsym();

  //!This function Draws scaler yield/asym  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotSCALER();

  //!This function Sets the combo index/combo element index
  //!
  //!Parameters:
  //! - combo box id 
  //! - combo box element id
  //!Return value: none 
  void SetComboIndex(Int_t cmb_id, Int_t id);

  //!This function loads list of bcms available in the injector. 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadInjectorBCMCombo();
  
  //!This function loads list of scalers available in the injector. 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadInjectorSCALERCombo();

   //!This function plots the mean BPM X/Y positions 
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotBPMPositions();

  
  std::vector<std::vector<TString> > fInjectorDevices; //2D vector since we have seral types of device - VQWK, SCALAR and COMBINED
  Int_t fCurrentBCMIndex; //Keep the BCM index corresponding to fHallCDevices read from dCombo_HCBCM
  Int_t fCurrentSCALERIndex; //Keep the BCM index corresponding to fHallCDevices read from dCombo_HCSCALER

  void                 SummaryHist(TH1*in);

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
  
  QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUIInjector();

  //!This function  loads the histogram names from a definition file
  //!Parameters:
  //! - Histogram names map file name
  //!
  //!Return value: none  
  void                 LoadHistoMapFile(TString mapfile);

  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  //virtual void        OnNewDataContainer();
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  //  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUIInjector,1);
};

#endif
