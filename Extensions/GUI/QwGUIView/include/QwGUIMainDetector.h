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

enum ENDataPlotType {
  PLOT_TYPE_HISTO,           
  PLOT_TYPE_GRAPH,           
  PLOT_TYPE_DFT,           
};

enum MDMenuIdentifiers {
  M_DATA_HISTO,
  M_DATA_GRAPH,
  M_DFT_HISTO,
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
#include "QwGUISubSystem.h"
#include "QwGUIDataWindow.h"
#include "RNumberEntryDialog.h"

class QwGUIMainDetector : public QwGUISubSystem {

  
  TGVerticalFrame   *dTabFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 

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
  TGLayoutHints          *dMenuBarLayout; 
  TGLayoutHints          *dMenuBarItemLayout;

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

  //!This function plots histograms of the data discrete fourier transform, in the main canvas.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  void                 PlotDFT();

  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearData();

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

  ENDataPlotType       dDataPlotType;
  Bool_t               dDFTCalculated;
  void                 SetDFTCalculated(Bool_t flag) {dDFTCalculated = flag;};
  Bool_t               IsDFTCalculated() {return dDFTCalculated;};
  void                 CalculateDFT();
  
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
