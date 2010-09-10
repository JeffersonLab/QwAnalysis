//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIMainDetector.h
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
   \class QwGUIMainDetector
    
   \brief Handles the display of the main detector data.  

   The QwGUIMainDetector class handles the display of the main detector data.
   It implements severl functions to manipulate the data and calculate certain
   basic diagnostic quantities.
    
 */
//=============================================================================

///
/// \ingroup QwGUIMain

#define MAIN_DET_INDEX      16           
#define MD_DET_TYPES        3


#ifndef QWGUIMAINDETECTOR_H
#define QWGUIMAINDETECTOR_H



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

class QwGUIMainDetector : public QwGUISubSystem {

  
  TGHorizontalFrame      *dTabFrame;
  TGVerticalFrame        *dControlsFrame;
  TGVerticalFrame        *dMDPMTFrame;
  TGVerticalFrame        *dMDVPMTFrame;
  TRootEmbeddedCanvas    *dCanvas;  
  TGLayoutHints          *dTabLayout; 
  TGLayoutHints          *dCnvLayout; 

  TGTextButton           *dButtonMD16;  
  TGTextButton           *dButtonMDBkg;
  TGTextButton           *dButtonMDCmb; 
  TGTextButton           *dButtonMDPMT; 
  TGTextButton           *dButtonMDVPMT; 

  TGComboBox             *dComboBoxMDPMT;
  TGComboBox             *dComboBoxMDVPMT;


  TGLayoutHints          *dTBinEntryLayout;
  TGNumberEntry          *dRunEntry;
  TGLayoutHints          *dRunEntryLayout;
  TGLabel                *dRunEntryLabel;
  TGHorizontal3DLine     *dHorizontal3DLine;
  TGHorizontalFrame      *dUtilityFrame;  
  TGLayoutHints          *dUtilityLayout;

    
  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;

  //!An object array to store graph pointers.
  TObjArray            GraphArray;

  //!An object array to store histogram pointers for the DFT.
  TObjArray            DFTArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;

  //!A dioalog for number entry ...  
  RNumberEntryDialog   *dNumberEntryDlg;
  
  //!Draw Means and widths for MD/MD bkg yields/asym
  TH1F *MDPlots[2] ;

 
  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearData();

  //!Draws MD yield/Asym histograms selected by dComboBoxMDPMT
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 DrawMDPMTPlots();


  //!Draws MD yield/Asym histograms selected by dComboBoxMDVPMT
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 DrawMDVPMTPlots();

    //!Plot MD yield/Asym means+/- widths for 16 PMTs 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 DrawMD16Plots();

  //!Plot MD yield/Asym means+/- widths for aux MD PMTs 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 DrawMDBkgPlots();

  //!Plot Combined MD yield/Asym means+/- widths  
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 DrawMDCmbPlots();

  //!This function Sets the combo index/combo element index
  //!
  //!Parameters:
  //! - combo box id 
  //! - combo box element id
  //!Return value: none 
  void SetComboIndex(Short_t cmb_id, Short_t id);


  
  //!Prints a summary for a given 1D histogram
  //!
  //!Parameters:
  //! - histogram object
  //!
  //!Return value: none  
  void                 SummaryHist(TH1 *in);

  //!This function  loads the histogram names from a definition file (main detector channel map file)
  //!Parameters:
  //! - Main detector channel map file name
  //!
  //!Return value: none  
  void                 LoadHistoMapFile(TString mapfile);

  
  //!This function loads list of PMT  . 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadMDPMTCombo();

  //!This function loads list of Combined PMT  . 
  //!based on the map file read by LoadChannelMap routine
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void LoadMDVPMTCombo();


  std::vector<std::vector<TString> > fMDDevices; //2D vector since we have seral types of device - VQWK and VPMT
  Short_t fCurrentPMTIndex; //Keep the PMT index corresponding to fMDDevices read from dCombo_MDPMT
  Short_t fCurrentVPMTIndex; //Keep the VPMT index corresponding to fMDDevices read from dCombo_MDPMT
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
  
  QwGUIMainDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUIMainDetector();


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
          void        OnUpdatePlot(char *);

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUIMainDetector,0);
};

#endif
