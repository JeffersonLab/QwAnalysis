//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUILumiDetector.h
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
   \class QwGUILumiDetector
    
   \brief Handles the display of the lumi detector data.  

   The QwGUILumiDetector class handles the display of the lumi detector data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector.  John Leacock 
   modified it to work with the lumis.
    
 */
//=============================================================================

#define LUMI_DET_HST_NUM      4           
#define NUM_POS          2
///
/// \ingroup QwGUILumi

#ifndef QWGUILUMIDETECTOR_H
#define QWGUILUMIDETECTOR_H


#define LUMI_DET_TYPES  7

#include "QwGUISubSystem.h"

#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif /* __CINT__ */

class QwGUILumiDetector : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TRootEmbeddedCanvas *dCanvas; 
  TGVerticalFrame     *dControlsFrame;
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dBtnLayout;

  TGTextButton        *dButtonUser;
  TGTextButton        *dButtonDetail;

  TGTextButton        *dButtonUSLumi;
  TGTextButton        *dButtonDSLumi;
  TGTextButton        *dButtonLumiAccess;
  TGTextButton        *dButtonScalerAccess;

  TGVerticalFrame     *dLumiFrame; //Individual Lumi access frame
  TGVerticalFrame     *dSCALERFrame; //Individual Lumi scaler access frame


  TGComboBox          *dComboBoxLUMI;
  TGComboBox          *dComboBoxSCALER;
  
  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;

  TH1F *PosVariation[2] ;//to store mean values for LUMI/SCALER devices


  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  //  void                 ClearData();

  //!This function Sets the combo index/combo element index
  //!
  //!Parameters:
  //! - combo box id 
  //! - combo box element id
  //!Return value: none 
  void SetComboIndex(Int_t cmb_id, Int_t id);

  //!This function loads list of Lumi available in the hall c . 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadLUMICombo();
  
  //!This function loads list of Lumi scalers available in the hall c . 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadSCALERCombo();


   //!This function Draws US Lumi devices MEAN  histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotUSLumi();

   //!This function Draws DS Lumi devices MEAN histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotDSLumi();

   //!This function Draws Lumi histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotLumi();

   //!This function Draws Lumi scaler histograms/plots from the Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotLumiScaler();


 
  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.
  static const char   *LumiDetectorHists[LUMI_DET_HST_NUM];


  std::vector<std::vector<TString> > fLUMIDevices; //2D vector since we have several types of device - VQWK, SCALAR and COMBINED
  Int_t fCurrentLUMIIndex; //Keep the BCM index corresponding to fHallCDevices read from dCombo_HCBCM
  Int_t fCurrentSCALERIndex; //Keep the BCM index corresponding to fHallCDevices read from dCombo_HCSCALER

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
 private:
  static const Int_t fSleepTimeMS;
 public:
  
  QwGUILumiDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUILumiDetector();

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

  /* virtual void        OnObjClose(char *); */
  virtual void        OnReceiveMessage(char*);
  /* virtual void        OnRemoveThisTab(); */

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  // virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUILumiDetector,1);

};

#endif
