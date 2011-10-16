//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUIDatabase.h
   \author Michael Gericke
   \author Buddhini Waidyawansa
   \author Damon Spayde
   \author Scott MacEwan
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIDatabase
    
   \brief Handles the display of database summary table data.

   The QwGUIHallCBeamline class handles the display of the hallc beamline data.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This is Gericke's original code for the main detector. 
   Added by Damon to display data from the data base.
   Modified by Buddhini and Scott.

 */
//=============================================================================

#define N_DETECTORS      43
#define N_LUMIS          36
#define N_BPMS           22
#define N_BCMS           10
#define N_CMB_BPMS        1
#define N_CMB_BCMS        1
#define N_ENERGY          1
#define N_SUBBLOCKS       5
#define N_DET_MEAS_TYPES  5
#define N_MEAS_TYPES      4
#define N_Q_MEAS_TYPES    6
#define N_POS_MEAS_TYPES  5
#define N_SENS_TYPES	  1
#define N_BPM_READ        5
#define N_CMB_READ        7
#define N_TGTS           15
#define N_GOODFOR_TYPES   8
#define N_REGRESSION_VARS 5
#define N_X_AXIS          4



///
/// \ingroup QwGUIDatabase

#ifndef QWGUIDATABASE_H
#define QWGUIDATABASE_H


#include <cstdlib>
#include <cstdio>
#include <vector>
using std::vector;

#include <iostream>
#include <iomanip>
#include <string>
#include <TPaveText.h>

#include "TRootEmbeddedCanvas.h"
#include "TRootCanvas.h"
#include "TVirtualPad.h"
#include "QwGUISubSystem.h"
#include "TStyle.h"
#include "QwGUIDataWindow.h"
/* #include "RSDataWindow.h" */
#include <TVectorT.h>
#include <TGraphErrors.h>
#include <THStack.h>



 class QwGUIDatabase : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TGHorizontalFrame	  *dRunFrame;
  TGHorizontalFrame   *dQualityFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dCmbLayout;
  TGLayoutHints       *dNumLayout;
  TGLayoutHints       *dBtnLayout;
  TGLayoutHints       *dLabLayout;
  TGLayoutHints		  *dChkLayout;
  TGLayoutHints		  *dFrmLayout;
  TGLayoutHints		  *dBoxLayout;
  TGNumberEntry       *dNumStartRun;
  TGNumberEntry       *dNumStopRun;
  TGCheckButton		  *dChkQualityGood;
  TGCheckButton		  *dChkQualitySuspect;
  TGCheckButton		  *dChkQualityBad;
  TGListBox			  *dBoxGoodFor;
  TGComboBox          *dCmbXAxis;
  TGComboBox          *dCmbInstrument;
  TGComboBox          *dCmbDetector;
  TGComboBox          *dCmbProperty;
  TGComboBox          *dCmbSubblock;
  TGComboBox          *dCmbMeasurementType;
  TGComboBox          *dCmbTargetType;
  TGComboBox          *dCmbRegressionType;
  TGComboBox          *dCmbPlotType;
  TGTextButton        *dBtnSubmit;
  TGLabel			  *dLabInstrument;
  TGLabel			  *dLabDetector;
  TGLabel			  *dLabProperty;
  TGLabel			  *dLabXAxis;
  TGLabel			  *dLabMeasurement;
  TGLabel			  *dLabSubblock;
  TGLabel			  *dLabQuality;
  TGLabel             *dLabRunRange;
  TGLabel             *dLabTarget;
  TGLabel             *dLabRegression;
  TGLabel             *dLabPlot;
  TGLabel			  *dLabGoodFor;


  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            GraphArray;
  TObjArray            LegendArray;
  
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //!A histogram array to plot the X and Y position difference variation.
  TPaveText * errlabel;


  //!A function to plot the detector data in a Y vs X format
  void DetectorPlot();

  //!A function to create specific queries.
  TString MakeQuery(TString outputs, TString tables_used, TString table_links,TString special_cuts);

#ifndef  ROOTCINTMODE
  mysqlpp::StoreQueryResult QueryDetector();
#endif

  void HistogramDetector();
  void PlotDetector();
  void DetectorVsMonitorPlot();
  TString GetYTitle(TString measurement_type, Int_t detector);
  TString GetTitle(TString measurement_type, TString device);

  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  void  ClearData();

  void                 SetSelectedDataWindow(Int_t ind) {dSelectedDataWindow = ind;};
  void                 RemoveSelectedDataWindow() {dSelectedDataWindow = -1;};
  QwGUIDataWindow     *GetSelectedDataWindow();  

  /**
     Arrays to store the detector names, measurement types and properties as they appear in the
     data base.
   */
  // detectors
  static const char   *DetectorCombos[N_DETECTORS];
  static const char   *BeamPositionMonitors[N_BPMS];
  static const char   *BeamCurrentMonitors[N_BCMS];
  static const char   *LumiCombos[N_LUMIS];
  static const char   *CombinedBPMS[N_CMB_BPMS];
  static const char   *CombinedBCMS[N_CMB_BCMS];
  static const char   *EnergyCalculators[N_ENERGY];
  static const char   *X_axis[N_X_AXIS];

  // measurement types
  static const char   *OtherMeasurementTypes[N_MEAS_TYPES];
  static const char   *ChargeMeasurementTypes[N_Q_MEAS_TYPES];
  static const char   *PositionMeasurementTypes[N_POS_MEAS_TYPES];
  static const char   *DetectorMeasurementTypes[N_DET_MEAS_TYPES];
  static const char   *SensitivityTypes[N_SENS_TYPES];

  // properties
  static const char   *Subblocks[N_SUBBLOCKS];
  static const char   *BPMReadings[N_BPM_READ];
  static const char   *ComboBPMReadings[N_CMB_READ];
  static const char	  *RegressionVars[N_REGRESSION_VARS];

  // target types
  static const char   *Targets[N_TGTS];
  static const char   *Plots[2];


  Int_t                dSelectedDataWindow;


  // good for types
  static const char	  *GoodForTypes[N_GOODFOR_TYPES];


  // static array for temporary measurement type storing. This makes things easier when trying to
  // retrieave data.
  std::vector<TString> measurements;

 /*private:

  QwGUIGoodForSettings fGoodForSelection;
*/
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
  void                 AddNewGraph(TObject* grp, TObject* leg);
  /*TObject*             GetGraph(Int_t ind); */
/*   TObject*             GetLegend(Int_t ind); */
  void                 PlotGraphs();
  void                 OnSubmitPushed();

 public:
  
  QwGUIDatabase(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		     const char *objName, const char *mainname, UInt_t w, UInt_t h);

  ~QwGUIDatabase();


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
  void                OnUpdatePlot(char *obj);
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();
  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);
  void                PopulateDetectorComboBox();
  void                PopulateMeasurementComboBox();
  void                PopulatePlotComboBox();

  ClassDef(QwGUIDatabase,0); 

 };

#endif
