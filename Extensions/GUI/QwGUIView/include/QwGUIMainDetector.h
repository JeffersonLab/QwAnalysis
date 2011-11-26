
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

/* #define MAIN_DET_BLOCKIND   5 */

enum ENPlotType {
  PLOT_TYPE_HISTO,           
  PLOT_TYPE_GRAPH,           
  PLOT_TYPE_DFT,           
  PLOT_TYPE_PROFILE,
};


enum MDMenuIdentifiers {
  M_PLOT_HISTO,
  M_PLOT_GRAPH,
  M_PLOT_FFT,
  M_PLOT_FFTPROF,

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


/* #include <sys/resource.h> */
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
#include "RNumberEntryDialog.h"
#include "RMsgBox.h"
#include "QwGUIEventWindowSelectionDialog.h"
#include "TFrame.h"
#include "TPaveText.h"
#include "TLeaf.h"

//This class defines the basic data structure for
//each detector and detector combination.
//It containes all data structures and plotypes. 
class QwGUIMainDetectorDataStructure{

 private:

  TString   DetectorName;      //Filled in from parameter file input  
  TString   DataType;          //Passed to class in constructor 

  UInt_t     DetectorID;        //Assigned at instantiation

  UInt_t     RunNumber;         //Assigned on file open
  UInt_t     NumTreeLeafs;      //As obtained from the tree
  UInt_t     ActiveTreeLeaf;    //Set by menu option
  UInt_t     EventCounter;      //Incremented during filling process
  UInt_t     NumTreeEvents;     //Number of events in tree: read from tree
  UInt_t     ErrorIndex;

  //This is the address to the data buffer that is passed
  //to TBranch::SetAddress, in order to read the data from the tree
  //Not sure if this needs to be public ... need to test.  //This gets replaced everytime a new entry is obtained from
  //the tree.
  Double_t *TreeLeafData;    //One for each leaf in the tree

  Double_t MPSCounter;

  //All of these are pointers to arrays of size "NumTreeLeafes"
  vector <TString> DataName;          //Combination of the leaf name and the detector name
  vector <TString> TreeLeafName;      //As obtained from the tree
  vector <TString> TreeLeafYUnits;    //Determined according to leaf name
  vector <TString> TreeLeafXUnits;    //Determined accoding to DetectorName (e.g. branch name)
  vector <Double_t> TreeLeafMean;      //Calculated during filling process
  vector <Double_t> TreeLeafSum;       //Calculated during filling process 
  vector <Double_t> TreeLeafSumSq;     //Calculated during filling process
  vector <Double_t> TreeLeafMin;       //Calculated during filling process
  vector <Double_t> TreeLeafMax;       //Calculated during filling process
  vector <Double_t> TreeLeafRMS;       //Calculated during filling process
  vector <Double_t> TreeLeafError;     //Calculated during filling process
  vector <Double_t> TreeLeafRMSError;  //Calculated during filling process
  vector <UInt_t>   StatEventCounter;  //Zero amplitude suppressed. Incremented during filling process.
  vector <Bool_t> TreeLeafFFTCalc;   //Assigned when FFT is calculated
  vector <EventOptions*> TreeLeafFFTOpts;
  RDataContainer *CurrentFile;
  TTree    *CurrentTree;
  TBranch  *ThisDetectorBranch;
  TLeaf    *MPSCntLeaf;

  //arrays of size "NumTreeLeafes" of pointers to plots 
  vector <TGraph*>   DataGraph;        //Filled during the reading process
  vector <TH1D*>     DataHisto;        //Filled after the reading process
  vector <TH1D*>     FFTHisto;         //Filled after the reading process
  vector <TProfile*> LinProfile;       //Filled after the reading process
  vector <TH1D*>     LinHist;          //Filled after the reading process

  Bool_t HistoMode;

 public:

  QwGUIMainDetectorDataStructure(Int_t ID, Int_t Run, TString Type);
  ~QwGUIMainDetectorDataStructure() {};

  void Clean();
 
  Int_t SetTree(TTree *tree);
  Int_t SetHistograms(RDataContainer *cont, TTree *tree);
  void  AddHistograms(RDataContainer *cont, TTree *tree);

  void FillData(Double_t sample);
  void FillHistograms();
  void FillCorrelation(QwGUIMainDetectorDataStructure* XdataStr, const char* title);
  void CalculateStats();
  int CalculateFFTs(EventOptions* evntopts = NULL, UInt_t leaf = -1);
  void SetDetectorName(const char *name) {DetectorName = name;};

  const char* GetDetectorName(){ return DetectorName.Data();};
  const char* GetTreeLeafName(UInt_t n);
  Int_t GetLeafIndex(const char* leaf);
  Double_t GetTreeLeafMin(UInt_t n);
  Double_t GetTreeLeafMax(UInt_t n);
  Double_t GetTreeLeafMean(UInt_t n);
  Double_t GetTreeLeafRMS(UInt_t n);
  Double_t GetTreeLeafError(UInt_t n);
  Double_t GetTreeLeafRMSError(UInt_t n);
  Double_t GetErrorCode(UInt_t event);
  UInt_t GetID(){return DetectorID;};
  UInt_t GetRunNumber() {return RunNumber;};
  void  SetRunNumber(UInt_t run) {RunNumber = run;};
  UInt_t GetNumberOfTreeLeafs() {return NumTreeLeafs;};
  UInt_t ProcessedEvents() { return EventCounter;}; 
  UInt_t ProcessedStatEvents(UInt_t n) { if(n<StatEventCounter.size()) return StatEventCounter[n]; return -1;}; 
  Bool_t IsFFTCalculated(int leaf); 

  void DrawHistogram(UInt_t i);
  void DrawGraph(UInt_t i);
  void DrawFFTHistogram(UInt_t i);
  void DrawCorrelationProfile(UInt_t i);
  void DrawCorrelationProjection(UInt_t i);

  TH1D* GetHistogram(UInt_t i);
  TGraph* GetGraph(UInt_t i);
  TH1D* GetFFTHistogram(UInt_t i);
  TProfile* GetCorrelationProfile(UInt_t i);
  TH1D* GetCorrelationProjection(UInt_t i);
  
  
};


class QwGUIMainDetectorDataType{

 private:

  TString Type;
  TTree *CurrentTree;
  RDataContainer *CurrentFile;

  vector <QwGUIMainDetectorDataStructure*> dDetDataStr;
  vector <TGraphErrors*> dMeanGraph;
  vector <TGraphErrors*> dRMSGraph;

  vector <QwGUIMainDetectorDataType*> dLinkedTypes;

  UInt_t NumberOfLeafs; 

  TGMenuBar              *dMenuBar;
  TGPopupMenu            *dMenuPlot;
  TGPopupMenu            *dMenuBlock;
  TGLayoutHints          *dMenuPlotLayout;
  TGLayoutHints          *dMenuBlockLayout;
  TGLayoutHints          *dMenuBarLayout;
  TGLayoutHints          *dCanvasLayout;
  TRootEmbeddedCanvas    *dCanvas;
  TGVerticalFrame        *dFrame;
  TGCompositeFrame       *dTab;


  UInt_t          MenuBaseID;
  UInt_t          HistoMenuID;
  UInt_t          GraphMenuID;
  UInt_t          FFTMenuID;
  UInt_t          ProfMenuID; 
  UInt_t          ProjectionMenuID; 
  ENPlotType      PlotType;
  vector <UInt_t> dLeafMenuID;
  UInt_t          ID;
  Bool_t          dSummary;
  Bool_t          HistoMode;
  UInt_t          dLinkedInd; 
  
  UInt_t          CurrentPlotMenuItemID;
  UInt_t          CurrentLeafMenuItemID;
  UInt_t          dNumberOfPads;

  UInt_t          dRunNumber;
 public:

  QwGUIMainDetectorDataType(UInt_t ID, Bool_t Summary = kFalse);
  ~QwGUIMainDetectorDataType() {};

  void Clean();

  void   SetType(const char *name) {Type = name;};

  QwGUIMainDetectorDataStructure *GetDetector(UInt_t n) { if(n < dDetDataStr.size()) return dDetDataStr[n]; return NULL; };
  QwGUIMainDetectorDataStructure *GetSelectedDetector();
  UInt_t GetNumDetectors() {return dDetDataStr.size();};
  const char *GetType() { return Type.Data();};
  UInt_t GetID(){return ID;};
  UInt_t SetTree(RDataContainer *cont, TTree *tree, vector <TString> DetNames, UInt_t run);
  UInt_t SetHistograms(RDataContainer *cont, TTree *tree, vector <TString> DetNames, UInt_t run);
  UInt_t AddHistograms(RDataContainer *cont, TTree *tree, vector <TString> DetNames);

  void FillData(Double_t sample);
  void ProcessData(const char* SummaryTitles, Bool_t Add = kFalse);
  void PlotData();
  TObject *GetSelectedPlot();
  TH1D *FindHistogram(const char* detector, const char* leaf);
  TGraph *FindGraph(const char* detector, const char* leaf);
  TProfile *FindProfile(const char* detector, const char* leaf);
  Int_t GetDetectorIndex(const char* detector);
  Int_t GetLeafIndex(const char* detector, const char* leaf);
  void CalculateFFTs(EventOptions*);
  TRootEmbeddedCanvas *MakeDataTab(TGTab *dMDTab, const TGWindow *parent, const TGWindow *client, Int_t w, Int_t h);
  TRootEmbeddedCanvas *GetCanvas(){return dCanvas;};
  void LinkData(vector <QwGUIMainDetectorDataType*> types);

  UInt_t GetHistoMenuID() {return  HistoMenuID;};
  UInt_t GetGraphMenuID() {return  GraphMenuID;};
  UInt_t GetFFTMenuID()   {return  FFTMenuID;};
  UInt_t GetProfMenuID()  {return  ProfMenuID;};
  UInt_t GetNumberOfLeafs() {return NumberOfLeafs;};
  UInt_t GetNumberOfPads() {return dNumberOfPads;};

  UInt_t GetCurrentPlotMenuItemID() {return CurrentPlotMenuItemID;};
  UInt_t GetCurrentLeafMenuItemID() {return CurrentLeafMenuItemID;};
  UInt_t GetCurrentLeafIndex();
  void  SetCurrentMenuItemID(UInt_t id);
  ENPlotType GetCurrentPlotType() {return PlotType;};

  void DrawMeanGraph(UInt_t i);
  void DrawRMSGraph(UInt_t i);
  TObject *GetMeanGraph(UInt_t i);
  TObject *GetRMSGraph(UInt_t i);

  
  Bool_t IsSummary() {return dSummary;};
};


class QwGUIMainDetector : public QwGUISubSystem {


  TGTab                  *dMDTab;
  TGVerticalFrame        *dSUMFrame;
  TGHorizontalFrame      *dTabFrame;
  
  TRootEmbeddedCanvas    *dYieldCanvas;  
  TRootEmbeddedCanvas    *dPMTAsymCanvas;  
  TRootEmbeddedCanvas    *dDETAsymCanvas;  
  TRootEmbeddedCanvas    *dCMBAsymCanvas;  
  TRootEmbeddedCanvas    *dMSCAsymCanvas;  
  TRootEmbeddedCanvas    *dMSCYieldCanvas;  
  TRootEmbeddedCanvas    *dSUMCanvas;  


  TGComboBox             *dTBinEntry;
  TGLayoutHints          *dTBinEntryLayout;
  TGNumberEntry          *dRunEntry;
  TGLayoutHints          *dRunEntryLayout;
  TGLabel                *dRunEntryLabel;
  TGHorizontal3DLine     *dHorizontal3DLine;
  TGHorizontalFrame      *dUtilityFrame;  
  TGLayoutHints          *dUtilityLayout;


  TGMenuBar              *dMenuBar;
  TGPopupMenu            *dMenuBlock;

  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray               DataWindowArray;

  //!A dioalog for number entry ...  
  RNumberEntryDialog   *dNumberEntryDlg;


  //!This function initiate the plotting of all histograms and graphs 
  //!relevant canvas.
  //!
  //!Parameters: 
  //! - tab number to plot data into
  //!
  //!Return value: kTrue on success, kFalse otherwise
  Bool_t PlotCurrentModeData(UInt_t tab);

  void                 NewDataInit();

  Int_t                GetActiveTab() {return dMDTab->GetCurrent();};

  UInt_t MAIN_PMT_INDEX;
  UInt_t MAIN_DET_INDEX;
  UInt_t MAIN_DET_COMBIND;
  UInt_t MAIN_MSC_INDEX;

  vector <TString> MainDetectorPMTYieldNames;
  vector <TString> MainDetectorPMTAsymNames;
  vector <TString> MainDetectorYieldNames;
  vector <TString> MainDetectorAsymNames;
  vector <TString> MainDetectorCombinationNames;    
  vector <TString> MainDetectorMscYieldNames;    
  vector <TString> MainDetectorMscAsymNames;    
  
  vector <QwGUIMainDetectorDataType*> dCurrentModeData;  
  QwGUIMainDetectorDataType *dCurrentYields;    //yields for block 1 - 4 and the hw sum
  QwGUIMainDetectorDataType *dCurrentPMTAsyms;  //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorDataType *dCurrentDETAsyms;  //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorDataType *dCurrentCMBAsyms;  //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorDataType *dCurrentMSCYields; //yields for block 1 - 4 and the hw sum
  QwGUIMainDetectorDataType *dCurrentMSCAsyms;  //asyms  for block 1 - 4 and the hw sum
  QwGUIMainDetectorDataType *dCurrentSummary;   //summary

  void                 SetSelectedDataWindow(Int_t ind) {dSelectedDataWindow = ind;};
  void                 RemoveSelectedDataWindow() {dSelectedDataWindow = -1;};
  QwGUIDataWindow     *GetSelectedDataWindow();  

  Int_t                MakeCurrentModeTabs();
  Int_t                LoadCurrentModeChannelMap(TTree *mps, TTree *hel);
  Int_t                GetCurrentModeData(TTree *mps, TTree *hel);

  Int_t                MakeTrackingModeTabs();
  Int_t                LoadTrackingModeChannelMap();
  Int_t                GetTrackingModeData(TTree *tree);

  void                 SetCurrentModeOpen(Bool_t flag){dCurrentModeOpen = flag;};
  void                 SetTrackingModeOpen(Bool_t flag) {dTrackingModeOpen = flag;};

  Bool_t               IsCurrentModeOpen(){return dCurrentModeOpen;};
  Bool_t               IsTrackingModeOpen() {return dTrackingModeOpen;};
  
  void                 MapTrackingModeTabs();
  void                 MapCurrentModeTabs();
  void                 UnMapTrackingModeTabs();
  void                 UnMapCurrentModeTabs();

  Int_t                dActiveTab;

  Int_t                dSelectedDataWindow;

  Bool_t               dCurrentModeOpen;
  Bool_t               dTrackingModeOpen;

  Int_t                CurrentMenuItemID;
  
  Int_t                SetupCurrentModeDataStructures(TTree *MPSTree,TTree *HELTree);
  Int_t                SetupTrackingModeDataStructures();
  
  Int_t                GetCurrentMenuItemID() {return CurrentMenuItemID;};
  void                 SetCurrentMenuItemID(Int_t id) {CurrentMenuItemID = id;};

  void                 CleanUp();
  void                 AddDataHistograms(TTree *MPSTree,TTree *HELTree);

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
  //! histograms from the ROOT file that is contained within the data container makes copies of
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

  TObject*            GetAsymmetrySummaryPlot();
  TObject*            GetMDAllAsymmetryHisto();

  virtual void        MakeHCLogEntry(); 

  ClassDef(QwGUIMainDetector,0);
};

#endif
