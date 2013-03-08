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

#define N_DETECTORS      60
#define N_LUMIS          32
#define N_BPMS           22
#define N_BCMS           11
#define N_BCM_DD         15
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
#define N_TGTS           16
#define N_GOODFOR_TYPES  21
#define N_X_AXIS          7
#define N_Plots           3
#define N_REGRESSION_SCHEMES 14
#define N_REG_VARS_ON     5
#define N_REG_VARS_ON_5_1 6
#define N_REG_VARS_ON_3   6
#define N_REG_VARS_ON_4   6
#define N_REG_VARS_ON_5   5
#define N_REG_VARS_ON_6   6
#define N_REG_VARS_ON_7   6
#define N_REG_VARS_ON_8   6
#define N_REG_VARS_ON_9   7
#define N_REG_VARS_ON_10  6
#define N_REG_VARS_ON_11  5
#define N_REG_VARS_ON_12  5



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

  //!Variables to assign selections from comboboxes.
  Int_t   index_first;
  Int_t   index_last;
  Int_t   det_id;
  Int_t   x_axis;
  Int_t   subblock;
  TString measurement_type;
  TString device;
  TString target;
  TString plot;
  TString detector;
  TString property;
  TString regression_set;
  TString reg_iv;

  const char **measurements;
  const char **regression_ivs;

  //!A function to get data selections from the combo boxes.
  void GetDataSelections();

  //!A function to plot the detector data in a Y vs X format
  void DetectorPlot();

  //!A function to create specific queries.
  TString MakeQuery(TString outputs, TString tables_used, TString table_links,TString special_cuts);


#ifndef  ROOTCINTMODE
  mysqlpp::StoreQueryResult QueryDetector();
#endif

  void HistogramDetector();
  void CreatePullPlot();
  void PlotDetector();
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
  static const char   *BCMDoubleDifferences[N_BCM_DD];

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

  // regression variables ans schemes
  static const char   *RegressionVarsOn[N_REG_VARS_ON];
  static const char   *RegressionVarsOn_5_1[N_REG_VARS_ON_5_1];
  static const char   *RegressionVarsOn_3[N_REG_VARS_ON_3];
  static const char   *RegressionVarsOn_4[N_REG_VARS_ON_4];
  static const char   *RegressionVarsOn_5[N_REG_VARS_ON_5];
  static const char   *RegressionVarsOn_6[N_REG_VARS_ON_6];
  static const char   *RegressionVarsOn_7[N_REG_VARS_ON_7];
  static const char   *RegressionVarsOn_8[N_REG_VARS_ON_8];
  static const char   *RegressionVarsOn_9[N_REG_VARS_ON_9];
  static const char   *RegressionVarsOn_10[N_REG_VARS_ON_10];
  static const char   *RegressionVarsOn_11[N_REG_VARS_ON_11];
  static const char   *RegressionVarsOn_12[N_REG_VARS_ON_12];

  static const char   *RegressionSchemes[N_REGRESSION_SCHEMES];

  // target types
  static const char   *Targets[N_TGTS];
  static const char   *Plots[3];


  Int_t                dSelectedDataWindow;


  // good for types
  static const char	  *GoodForTypes[N_GOODFOR_TYPES];


  // static array for temporary measurement type storing. This makes things easier when trying to
  // retrieave data.


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
  void				  PopulateRegComboBox();
  void                RegressionTypeInfo();

  ClassDef(QwGUIDatabase,0); 

 };

#endif
