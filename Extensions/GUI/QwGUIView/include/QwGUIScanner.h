//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUIScanner.h
   \author Michael Gericke
   \author Jie Pan

*/
//=============================================================================
//
//=============================================================================
//
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIScanner

   \brief Handles the display of the main detector data.

   The QwGUIScanner class handles the display of the main detector data.
   It implements severl functions to manipulate the data and calculate certain
   basic diagnostic quantities.

 */
//=============================================================================

///
/// \ingroup QwGUIMain

#define SCANNER_INDEX       30

#ifndef QWGUISCANNER_H
#define QWGUISCANNER_H


enum ScannerDataPlotType
{
  SCANNER_PLOT_TYPE_HISTO,
  SCANNER_PLOT_TYPE_GRAPH,
  SCANNER_PLOT_TYPE_PROFILE,
};

enum ScannerMenuIdentifiers
{
  SCANNER_PLOT_RATEMAP,
  SCANNER_PLOT_PROJECTION,
//   SCANNER_PLOT_HISTOGRAM,
//   SCANNER_PLOT_RAW
};

enum ScannerRunMode {EVENT_MODE, CURRENT_MODE};
enum PosData {PVQWK, PQDC};

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
using std::vector;

#include <TG3DLine.h>
#include <TRootEmbeddedCanvas.h>
#include <TRootCanvas.h>
#include <TMath.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TLeaf.h>
#include <TVirtualFFT.h>
#include "QwGUISubSystem.h"
#include "QwGUIDataWindow.h"
#include "RNumberEntryDialog.h"

class QwGUIScanner : public QwGUISubSystem
  {


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

    //!This function plots histograms of the data in the current file, in the main canvas.
    //!
    //!Parameters:
    //! - none
    //!
    //!Return value: none
    void                 PlotRateMap();
    void                 PlotProjections();
    void                 PlotSinglesMap();

    //!This function plots histograms of the data in the current file, in the main canvas.
    //!
    //!Parameters:
    //! - none
    //!
    //!Return value: none
    void                 PlotRawDataHist();

    //!This function clears the histograms/plots in the plot container (for root files). This is done
    //!everytime a new file is opened. If the displayed plots are not saved prior to opening a new file
    //!any changes on the plots are lost.
    //!
    //!Parameters:
    //! - none
    //!
    //!Return value: none
    void                 ClearRootData();


    //!This function clears the histograms/plots in the plot container (for database files). This is done
    //!everytime a new database is opened. If the displayed plots are not saved prior to opening a new file
    //!any changes on the plots are lost.
    //!
    //!Parameters:
    //! - none
    //!
    //!Return value: none
    void                 ClearDBData();

    Int_t                GetCurrentDataLength(Int_t det)
    {
      return det >= 0 && det < SCANNER_INDEX ? dCurrentData[det].size() : -1;
    };

    void                 SetPlotDataType(ScannerDataPlotType type)
    {
      dDataPlotType = type;
    };
    ScannerDataPlotType       GetPlotDataType()
    {
      return dDataPlotType;
    };

    //!An array that stores the ROOT names of the histograms that I chose to display for now.
    //!These are the names by which the histograms are identified within the root file.
    static const char   *ScannerDataNames[SCANNER_INDEX];
    static const int    *ScannerDataIndex[SCANNER_INDEX];

    //!Stores the data items (events) from the tree for all detectors
    vector <Double_t>    dCurrentData[SCANNER_INDEX];
    Double_t    dCurrentDataMean[SCANNER_INDEX];
    Double_t    dCurrentDataMin[SCANNER_INDEX];
    Double_t    dCurrentDataMax[SCANNER_INDEX];
    Double_t    dCurrentDataRMS[SCANNER_INDEX];

    ScannerDataPlotType       dDataPlotType;

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

    QwGUIScanner(const TGWindow *p, const TGWindow *main, const TGTab *tab,
                 const char *objName, const char *mainname, UInt_t w, UInt_t h);
    ~QwGUIScanner();


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

  private:


    Int_t RunMode;
    Int_t PositionDataType;

    TProfile2D *RateMap;
    TProfile2D *RandomMap;

    TH2D *RateMapProjectionXY;
    TH1D *RateMapProjectionX;
    TH1D *RateMapProjectionY;

    ClassDef(QwGUIScanner,0);
  };

#endif

