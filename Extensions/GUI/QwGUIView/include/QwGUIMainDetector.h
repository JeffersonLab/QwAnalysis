
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

#define MAIN_DET_BLOCKIND   1

enum ENPlotType {
  PLOT_TYPE_HISTO,           
  PLOT_TYPE_GRAPH,           
  PLOT_TYPE_DFT,           
  PLOT_TYPE_PROFILE,
};


enum MDMenuIdentifiers {
  M_PLOT_HISTO_YIELD,
  M_PLOT_GRAPH_YIELD,
  M_PLOT_DFT_YIELD,
  M_PLOT_DFTPROF_YIELD,

  M_PLOT_HISTO_PMT_ASYM,
  M_PLOT_GRAPH_PMT_ASYM,
  M_PLOT_DFT_PMT_ASYM,
  M_PLOT_DFTPROF_PMT_ASYM,

  M_PLOT_HISTO_DET_ASYM,
  M_PLOT_GRAPH_DET_ASYM,
  M_PLOT_DFT_DET_ASYM,
  M_PLOT_DFTPROF_DET_ASYM,

  M_PLOT_HISTO_CMB_ASYM,
  M_PLOT_GRAPH_CMB_ASYM,
  M_PLOT_DFT_CMB_ASYM,
  M_PLOT_DFTPROF_CMB_ASYM,

  M_PLOT_HISTO_MSC_ASYM,
  M_PLOT_GRAPH_MSC_ASYM,
  M_PLOT_DFT_MSC_ASYM,
  M_PLOT_DFTPROF_MSC_ASYM,

  M_PLOT_HISTO_MSC_YIELD,
  M_PLOT_GRAPH_MSC_YIELD,
  M_PLOT_DFT_MSC_YIELD,
  M_PLOT_DFTPROF_MSC_YIELD,

  M_DATA_PMT_YIELD_B1,
  M_DATA_PMT_YIELD_B2,
  M_DATA_PMT_YIELD_B3,
  M_DATA_PMT_YIELD_B4,
  M_DATA_PMT_YIELD_SUM,

  M_DATA_PMT_ASYM_B1,
  M_DATA_PMT_ASYM_B2,
  M_DATA_PMT_ASYM_B3,
  M_DATA_PMT_ASYM_B4,
  M_DATA_PMT_ASYM_SUM,

  M_DATA_DET_ASYM_B1,
  M_DATA_DET_ASYM_B2,
  M_DATA_DET_ASYM_B3,
  M_DATA_DET_ASYM_B4,
  M_DATA_DET_ASYM_SUM,

  M_DATA_CMB_ASYM_B1,
  M_DATA_CMB_ASYM_B2,
  M_DATA_CMB_ASYM_B3,
  M_DATA_CMB_ASYM_B4,
  M_DATA_CMB_ASYM_SUM,

  M_DATA_MSC_ASYM_B1,
  M_DATA_MSC_ASYM_B2,
  M_DATA_MSC_ASYM_B3,
  M_DATA_MSC_ASYM_B4,
  M_DATA_MSC_ASYM_SUM,

  M_DATA_MSC_YIELD_B1,
  M_DATA_MSC_YIELD_B2,
  M_DATA_MSC_YIELD_B3,
  M_DATA_MSC_YIELD_B4,
  M_DATA_MSC_YIELD_SUM,

  M_DATA_SUM_B1,
  M_DATA_SUM_B2,
  M_DATA_SUM_B3,
  M_DATA_SUM_B4,
  M_DATA_SUM_SUM,

  PMT_ASYM,
  DET_ASYM,
  PMT_YIELD,  
  CMB_ASYM,  
  MSC_ASYM,  
  MSC_YIELD,  
  SUMMARY,  
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
#include "RMsgBox.h"
#include "QwGUIEventWindowSelectionDialog.h"

class QwGUIMainDetectorDataStructure{

 private:
  
  TString Name;

  vector <Double_t>    Data;
  vector <Double_t>    FFT;
  vector <Double_t>    EnsambleMean;
  vector <Double_t>    EnsambleError;
  Double_t    DataSum;
  Double_t    DataSumSq;

  Double_t    EnsambleSum;
  Double_t    EnsambleSumSq;

  Double_t    DataMean;
  Double_t    DataEnMean;
  Double_t    DataMin;
  Double_t    DataMax;
  Double_t    DataRMS;
  Double_t    DataError;
  Double_t    DataEnError;
  Int_t       EnsambleSize;
  Int_t       EnsambleCounter;

 public:

  QwGUIMainDetectorDataStructure() { Clean(); };
  ~QwGUIMainDetectorDataStructure() { Clean(); };

  void Clean();
  
  void EnsambleReset(){
    EnsambleSum = 0;
    EnsambleSumSq = 0;
    EnsambleCounter = 0;
    EnsambleSize = 1000;
  }

  void PushData(Double_t item);
  void CalculateStats();

  void SetEnsambleSize(Int_t size) {EnsambleSize = size;};       
  void SetName(const char *name) {Name = name;};

  Double_t GetData(Int_t ind) {if(ind < 0 || ind >= Length()) return 0; return Data.at(ind);};
  Double_t GetEnsambleMean(Int_t ind) {if(ind < 0 || ind >= GetNumEnsambles()) return 0; return EnsambleMean.at(ind);};
  Double_t GetDataRMS() {return DataRMS;};
  Double_t GetDataError() {return DataError;};
  Double_t GetDataEnError() {return DataEnError;};
  Double_t GetDataMean() {return DataMean;};
  Double_t GetDataEnMean() {return DataEnMean;};
  Double_t GetDataMin() {return DataMin;};
  Double_t GetDataMax() {return DataMax;};
  Int_t    GetNumEnsambles(){return EnsambleMean.size();};
  Int_t    Length() {return Data.size();};
  Int_t    GetEnsambleSize() {return EnsambleSize;};
  const char* GetName(){ return Name.Data();};
  
};

class QwGUIMainDetectorData {

 private:

  TString Name;

  Int_t dSize;

  QwGUIMainDetectorDataStructure *dData;

  FFTOptions fftopts;
  
 public:

  QwGUIMainDetectorData(Int_t size);
  ~QwGUIMainDetectorData() { Clean(); delete[] dData; };

  void Clean();

  void   SetFFTCalculated(Bool_t flag) {fftopts.calcFlag = flag;};
  Bool_t IsFFTCalculated()      {return fftopts.calcFlag;};

  void   SetFFTCancelFlag(Bool_t flag) {fftopts.cancelFlag = flag;};
  void   SetFFTChangeFlag(Bool_t flag) {fftopts.changeFlag = flag;};
  void   SetFFTStart(Int_t start)      {fftopts.Start = start;};
  void   SetFFTLength(Int_t length)    {fftopts.Length = length;};
  void   SetFFTTotalLength(Int_t totallength) {fftopts.TotalLength = totallength;};
  void   SetFFTOptions(FFTOptions opts){
  
    fftopts.calcFlag    = opts.calcFlag;
    fftopts.cancelFlag  = opts.cancelFlag;
    fftopts.changeFlag  = opts.changeFlag;
    fftopts.Start       = opts.Start;
    fftopts.Length      = opts.Length; 
    fftopts.TotalLength = opts.TotalLength;

  };
  void   SetName(const char *name) {Name = name;};

  Bool_t GetFFTCancelFlag() {return fftopts.cancelFlag;};
  Bool_t GetFFTChangeFlag() {return fftopts.changeFlag;};
  Int_t  GetFFTStart(Int_t start) {return fftopts.Start;};
  Int_t  GetFFTLength(Int_t length) {return fftopts.Length;};
  FFTOptions *GetFFTOptions(){ return &fftopts; };
  QwGUIMainDetectorDataStructure *GetElements() { return dData; };
  Int_t GetNumElements() {return dSize;};
  const char *GetName() { return Name.Data();}; 

};


class QwGUIMainDetector : public QwGUISubSystem {


  TGTab                  *dMDTab;
  TGVerticalFrame        *dYieldFrame;
  TGVerticalFrame        *dPMTAsymFrame;
  TGVerticalFrame        *dDETAsymFrame;
  TGVerticalFrame        *dCMBAsymFrame;
  TGVerticalFrame        *dMSCAsymFrame;
  TGVerticalFrame        *dMSCYieldFrame;
  TGVerticalFrame        *dSUMFrame;

  TGHorizontalFrame      *dTabFrame;
  
  TRootEmbeddedCanvas    *dYieldCanvas;  
  TRootEmbeddedCanvas    *dPMTAsymCanvas;  
  TRootEmbeddedCanvas    *dDETAsymCanvas;  
  TRootEmbeddedCanvas    *dCMBAsymCanvas;  
  TRootEmbeddedCanvas    *dMSCAsymCanvas;  
  TRootEmbeddedCanvas    *dMSCYieldCanvas;  
  TRootEmbeddedCanvas    *dSUMCanvas;  

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

  TGMenuBar              *dMenuBar1;
  TGMenuBar              *dMenuBar2;
  TGMenuBar              *dMenuBar3;
  TGMenuBar              *dMenuBar4;
  TGMenuBar              *dMenuBar5;
  TGMenuBar              *dMenuBar6;
  TGMenuBar              *dMenuBar7;

/*   TGPopupMenu            *dMenuData; */

  TGPopupMenu            *dMenuPlot1;
  TGPopupMenu            *dMenuPlot2;
  TGPopupMenu            *dMenuPlot3;
  TGPopupMenu            *dMenuPlot4;
  TGPopupMenu            *dMenuPlot5;
  TGPopupMenu            *dMenuPlot6;
  TGPopupMenu            *dMenuPlot7;


  TGPopupMenu            *dMenuBlock1;
  TGPopupMenu            *dMenuBlock2;
  TGPopupMenu            *dMenuBlock3;
  TGPopupMenu            *dMenuBlock4;
  TGPopupMenu            *dMenuBlock5;
  TGPopupMenu            *dMenuBlock6;
  TGPopupMenu            *dMenuBlock7;

  TGLayoutHints          *dMenuBarLayout; 
  TGLayoutHints          *dMenuBarItemLayout;

  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            YieldHistArray[MAIN_DET_BLOCKIND];
  TObjArray            PMTAsymHistArray[MAIN_DET_BLOCKIND];
  TObjArray            DetAsymHistArray[MAIN_DET_BLOCKIND];
  TObjArray            CMBAsymHistArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCAsymHistArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCYieldHistArray[MAIN_DET_BLOCKIND];

  //!An object array to store graph pointers.
  TObjArray            YieldGraphArray[MAIN_DET_BLOCKIND];
  TObjArray            PMTAsymGraphArray[MAIN_DET_BLOCKIND];
  TObjArray            DetAsymGraphArray[MAIN_DET_BLOCKIND];
  TObjArray            CMBAsymGraphArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCAsymGraphArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCYieldGraphArray[MAIN_DET_BLOCKIND];

  //!An object array to store profile histogram pointers.
  TObjArray            YieldProfileArray[MAIN_DET_BLOCKIND];
  TObjArray            PMTAsymProfileArray[MAIN_DET_BLOCKIND];
  TObjArray            DetAsymProfileArray[MAIN_DET_BLOCKIND];
  TObjArray            CMBAsymProfileArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCAsymProfileArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCYieldProfileArray[MAIN_DET_BLOCKIND];

  //!An object array to store histogram pointers for the DFT.
  TObjArray            YieldDFTArray[MAIN_DET_BLOCKIND];
  TObjArray            PMTAsymDFTArray[MAIN_DET_BLOCKIND];
  TObjArray            DetAsymDFTArray[MAIN_DET_BLOCKIND];
  TObjArray            CMBAsymDFTArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCAsymDFTArray[MAIN_DET_BLOCKIND];
  TObjArray            MSCYieldDFTArray[MAIN_DET_BLOCKIND];

  TObjArray            SummaryGraphArray[MAIN_DET_BLOCKIND];
  
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
  Bool_t                 PlotHistograms();

  //!This function plots time graphs of the data in the current file, in the main canvas.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  Bool_t                 PlotGraphs();

  //!This function plots histograms of the data discrete fourier transform.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  Bool_t                 PlotDFT();

  //!This function plots the event window used to calculate the discrete fourier transform.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  Bool_t                 PlotDFTProfile();

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
  //! - ALL: A boolean that specifies if all DFT arrays are to be cleared or only the one that is currently
  //!        active in the display.
  //!
  //!Return value: none  
  void                 ClearDFTData(Bool_t ALL = kTrue);

  //!This function clears the histograms/plots in the plot container (for database files). This is done  
  //!everytime a new database is opened. If the displayed plots are not saved prior to opening a new file
  //!any changes on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void                 ClearDBData();


  void                 NewDataInit();

/*   Int_t                GetCurrentDataLength(Int_t det) {return det >= 0 && det < MAIN_DET_INDEX ? dCurrentData[det].size() : -1;}; */

  void                 SetPlotType(ENPlotType type, Int_t tab) {dPlotType[tab] = type;};
  ENPlotType           GetPlotType(Int_t tab) {return dPlotType[tab];};

  Int_t                GetActiveTab() {return dMDTab->GetCurrent();};

  void                 SetDataType(MDMenuIdentifiers type) {dDataType = type;};
  MDMenuIdentifiers    GetDataType() {return dDataType;};

  void                 FillYieldPlots(Int_t det, Int_t dTInd);
  void                 FillPMTAsymPlots(Int_t det, Int_t dTInd);
  void                 FillDETAsymPlots(Int_t det, Int_t dTInd);
  void                 FillCMBAsymPlots(Int_t det, Int_t dTInd);
  void                 FillMSCAsymPlots(Int_t det, Int_t dTInd);
  void                 FillMSCYieldPlots(Int_t det, Int_t dTInd);
  void                 FillSummaryPlots(Int_t dTInd);


  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.

  UInt_t MAIN_PMT_INDEX;
  UInt_t MAIN_DET_INDEX;
  UInt_t MAIN_DET_COMBIND;
  UInt_t MAIN_MSC_INDEX;

  vector <TString> MainDetectorPMTNames;//[MAIN_DET_INDEX];
  vector <TString> MainDetectorNames;
  vector <TString> MainDetectorCombinationNames;    
  vector <TString> MainDetectorMscNames;    

  static const char   *MainDetectorBlockTypes[MAIN_DET_BLOCKIND];
  static const char   *MainDetectorBlockTypesRaw[MAIN_DET_BLOCKIND];

  //!Stores the data items (events) from the tree for all detectors

  QwGUIMainDetectorData *dCurrentYields[MAIN_DET_BLOCKIND];   //yields for block 1 - 4 and the hw sum 
  QwGUIMainDetectorData *dCurrentPMTAsyms[MAIN_DET_BLOCKIND]; //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorData *dCurrentDETAsyms[MAIN_DET_BLOCKIND]; //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorData *dCurrentCMBAsyms[MAIN_DET_BLOCKIND]; //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorData *dCurrentMSCYields[MAIN_DET_BLOCKIND]; //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorData *dCurrentMSCAsyms[MAIN_DET_BLOCKIND]; //asyms  for block 1 - 4 and the hw sum

  Bool_t               CalculateDFT();
  void                 SetYieldDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dYieldDataInd = 0;     else dYieldDataInd   = ind;};
  Int_t                GetYieldDataIndex(){return dYieldDataInd;};

  void                 SetPMTAsymDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dPMTAsymDataInd = 0; else dPMTAsymDataInd = ind;};
  Int_t                GetPMTAsymDataIndex(){return dPMTAsymDataInd;};

  void                 SetDETAsymDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dDETAsymDataInd = 0; else dDETAsymDataInd = ind;};
  Int_t                GetDETAsymDataIndex(){return dDETAsymDataInd;};

  void                 SetCMBAsymDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dCMBAsymDataInd = 0; else dCMBAsymDataInd = ind;};
  Int_t                GetCMBAsymDataIndex(){return dCMBAsymDataInd;};

  void                 SetMSCAsymDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dMSCAsymDataInd = 0; else dMSCAsymDataInd = ind;};
  Int_t                GetMSCAsymDataIndex(){return dMSCAsymDataInd;};

  void                 SetMSCYieldDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dMSCYieldDataInd = 0; else dMSCYieldDataInd = ind;};
  Int_t                GetMSCYieldDataIndex(){return dMSCYieldDataInd;};

  void                 SetSummaryDataIndex(Int_t ind) {if(ind < 0 || ind > 4) dSummaryDataInd = 0; else dSummaryDataInd = ind;};
  Int_t                GetSummaryDataIndex(){return dSummaryDataInd;};

  void                 SetSelectedDataWindow(Int_t ind) {dSelectedDataWindow = ind;};
  void                 RemoveSelectedDataWindow() {dSelectedDataWindow = -1;};
  QwGUIDataWindow     *GetSelectedDataWindow();  

  void                 MakeCurrentModeTabs();
  Int_t                LoadCurrentModeChannelMap();
  void                 GetCurrentModeData(TTree *mps, TTree *hel);

  void                 MakeTrackingModeTabs();
  Int_t                LoadTrackingModeChannelMap();
  void                 GetTrackingModeData(TTree *tree);

  MDMenuIdentifiers    dDataType;
  ENPlotType           dPlotType[20];
  Int_t                dYieldDataInd;  
  Int_t                dPMTAsymDataInd;
  Int_t                dDETAsymDataInd;
  Int_t                dCMBAsymDataInd;
  Int_t                dMSCAsymDataInd;
  Int_t                dMSCYieldDataInd;
  Int_t                dSummaryDataInd;
  Int_t                dSelectedDataWindow;
  
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
