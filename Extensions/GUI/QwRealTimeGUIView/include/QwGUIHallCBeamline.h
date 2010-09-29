//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIHallCBeamline.h
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
   \class QwGUIHallCBeamline
    
   \brief Handles the display of the lumi detector data.  

   The QwGUIHallCBeamline class handles the display of the hall c beam line data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 


 */
//=============================================================================

#define NUM_POS          2
#define HALLC_DET_TYPES  6
///
/// \ingroup QwGUIHallCBeamline

#ifndef QWGUIHALLCBEAMLINE_H
#define QWGUIHALLCBEAMLINE _H


#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <string>

#include <TMapFile.h>

#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "QwGUISubSystem.h"

#include "RSDataWindow.h"


#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif /* __CINT__ */




class QwGUIHallCBeamline : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  
  TGVerticalFrame     *dControlsFrame;
  TGVerticalFrame     *dHCBCMFrame; //hall c bcm access frame
  TGVerticalFrame     *dHCSCALERFrame; //hall c scaler access frame
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGTextButton        *dButtonPos;  
  TGTextButton        *dButtonCharge;
  TGTextButton        *dButtonPosVariation;
  TGTextButton        *dButtonHCSCALER;
  TGTextButton        *dButtonMeanPos;
  TGTextButton        *dButtonTgtX;
  TGTextButton        *dButtonTgtY;
  TGTextButton        *dButtonTgtAngleX;
  TGTextButton        *dButtonTgtAngleY;
  TGTextButton        *dButtonTgtCharge;


  TGComboBox          *dComboBoxHCBCM;
  TGComboBox          *dComboBoxHCSCALER;

 
  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  TH1F *PosVariation[2] ;

 
  

  //!This function Draws a BPM position diff  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PositionDifferences();

  //!This function Draws a BPM asym  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotBPMAsym();

  //!This function Draws BCM histograms/plots from the Memory map file. 
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

  //!This function Draws target X,X',Y,Y' yield/asym  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - plot to draw pass the enum code
  //!
  //!Return value: none 
  void PlotTargetPos(Short_t );

  //!This function Draws target charge yield/asym  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - plot to draw pass the enum code
  //!
  //!Return value: none 
  void PlotTargetCharge();

  //!This function Sets the combo index/combo element index
  //!
  //!Parameters:
  //! - combo box id 
  //! - combo box element id
  //!Return value: none 
  void SetComboIndex(Short_t cmb_id, Short_t id);

  //!This function loads list of bcms available in the hall c . 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadHCBCMCombo();
  
  //!This function loads list of scalers available in the hall c . 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadHCSCALERCombo();

   //!This function plots the mean BPM X/Y positions 
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotBPMPositions();

  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearData();

  
  //!This function  loads the histogram names from a definition file
  //!Parameters:
  //! - Histogram names map file name
  //!
  //!Return value: none  
  void                 LoadHistoMapFile(TString mapfile);

  

    
  std::vector<std::vector<TString> > fHallCDevices; //2D vector since we have seral types of device - VQWK, SCALAR and COMBINED
  Short_t fCurrentBCMIndex; //Keep the BCM index corresponding to fHallCDevices read from dCombo_HCBCM
  Short_t fCurrentSCALERIndex; //Keep the BCM index corresponding to fHallCDevices read from dCombo_HCSCALER

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

  //!This function  prints stat info of the histogram into the command line
  //!Parameters:
  //! - Histogram 
  //!
  //!Return value: none  
  void                 SummaryHist(TH1*in);

 public:
  
  QwGUIHallCBeamline(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUIHallCBeamline();


  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  virtual void        OnNewDataContainer();
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUIHallCBeamline,0);
};

#endif
