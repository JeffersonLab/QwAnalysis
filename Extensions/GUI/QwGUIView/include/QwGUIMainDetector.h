//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIMainDetector.h
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
#define SAMPLING_RATE       909

enum ENDataPlotType {
  PLOT_TYPE_HISTO,           
  PLOT_TYPE_GRAPH,           
  PLOT_TYPE_DFT,           
  PLOT_TYPE_PROFILE,
};

enum MDMenuIdentifiers {
  M_PLOT_HISTO,
  M_PLOT_GRAPH,
  M_PLOT_DFT,
  M_PLOT_DFTPROF,
  M_DATA_PMT_YIELD,
  M_DATA_DET_YIELD,
  M_DATA_ALL_YIELD,
  M_DATA_PMT_ASYM,
  M_DATA_DET_ASYM,
  M_DATA_ALL_ASYM,
};

#ifndef QWGUIMAINDETECTOR_H
#define QWGUIMAINDETECTOR_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
using std::vector;

#include <TG3DLine.h>
#include <TRootEmbeddedCanvas.h>
#include <TRootCanvas.h>
#include <TMath.h>
#include <TProfile.h>
#include <TVirtualFFT.h>
#include "QwGUISubSystem.h"
#include "QwGUIDataWindow.h"
#include "RNumberEntryDialog.h"
#include "QwGUIFFTWindowSelectionDialog.h"
#ifndef ROOTCINTMODE
#include "QwSSQLS_summary.h"
#endif
#include <TVectorT.h>
#include <TGraphErrors.h>

class QwGUIMainDetector : public QwGUISubSystem {

  
  TGVerticalFrame        *dTabFrame;
  TRootEmbeddedCanvas    *dCanvas;  
  TGLayoutHints          *dTabLayout; 
  TGLayoutHints          *dCnvLayout; 

  TGComboBox             *dTBinEntry;
  TGLayoutHints          *dTBinEntryLayout;
  TGNumberEntry          *dRunEntry;
  TGLayoutHints          *dRunEntryLayout;
  TGLabel                *dRunEntryLabel;
  TGHorizontal3DLine     *dHorizontal3DLine;
  TGHorizontalFrame      *dUtilityFrame;  
  TGLayoutHints          *dUtilityLayout;

  TGMenuBar              *dMenuBar;
  TGPopupMenu            *dMenuData;
  TGPopupMenu            *dMenuPlot;
  TGLayoutHints          *dMenuBarLayout; 
  TGLayoutHints          *dMenuBarItemLayout;

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            HistArray;

  //!An object array to store graph pointers.
  TObjArray            GraphArray;

  //!An object array to store profile histogram pointers.
  TObjArray            ProfileArray;

  //!An object array to store histogram pointers for the DFT.
  TObjArray            DFTArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;

  //!A dioalog for number entry ...  
  RNumberEntryDialog   *dNumberEntryDlg;

  //!This function plots histograms of the data in the current file, in the main canvas.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotHistograms();

  //!This function plots time graphs of the data in the current file, in the main canvas.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotGraphs();

  //!This function plots histograms of the data discrete fourier transform.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotDFT();

  //!This function plots the event window used to calculate the discrete fourier transform.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotDFTProfile();

  //!This function clears the histograms/plots in the plot container (for root files). This is done  
  //!everytime a new file is opened. If the displayed plots are not saved prior to opening a new file
  //!any changes on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearRootData();

  //!This function clears the histograms in the plot container that are generated in the processes of 
  //!calculating the discrete fast fourier transform. This is done everytime the FFT is (re)calculated. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearDFTData();

  //!This function clears the histograms/plots in the plot container (for database files). This is done  
  //!everytime a new database is opened. If the displayed plots are not saved prior to opening a new file
  //!any changes on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearDBData();

  Int_t                GetCurrentDataLength(Int_t det) {return det >= 0 && det < MAIN_DET_INDEX ? dCurrentData[det].size() : -1;};

  void                 SetPlotDataType(ENDataPlotType type) {dDataPlotType = type;};
  ENDataPlotType       GetPlotDataType() {return dDataPlotType;};

  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.
  static const char   *MainDetectorDataNames[MAIN_DET_INDEX];
  static const int    *MainDetectorDataIndex[MAIN_DET_INDEX];

  //!Stores the data items (events) from the tree for all detectors
  vector <Double_t>    dCurrentData[MAIN_DET_INDEX];
  vector <Double_t>    dCurrentDataDFT[MAIN_DET_INDEX];
  Double_t    dCurrentDataMean[MAIN_DET_INDEX];
  Double_t    dCurrentDataMin[MAIN_DET_INDEX];
  Double_t    dCurrentDataMax[MAIN_DET_INDEX];
  Double_t    dCurrentDataRMS[MAIN_DET_INDEX];

  ENDataPlotType       dDataPlotType;
  Bool_t               dDFTCalculated;
  void                 SetDFTCalculated(Bool_t flag) {dDFTCalculated = flag;};
  Bool_t               IsDFTCalculated() {return dDFTCalculated;};
  void                 CalculateDFT();

  FFTOptions           fftopts;
  
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
  virtual void        OnNewDataContainer(RDataContainer *cont);
  virtual void        OnObjClose(char *);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();
          void        OnUpdatePlot(char *);

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

  ClassDef(QwGUIMainDetector,0);
};

#endif
