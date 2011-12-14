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
   Modified by Buddhini.

 */
//=============================================================================

#define N_DETECTORS     39
#define N_LUMIS         36
#define N_BPMS          22
#define N_BCMS           6
#define N_CMB_BPMS       1
#define N_CMB_BCMS       1
#define N_ENERGY         1
#define N_SUBBLOCKS      5
#define N_DET_MEAS_TYPES 5
#define N_MEAS_TYPES     4
#define N_Q_MEAS_TYPES   5
#define N_POS_MEAS_TYPES 5
#define N_BPM_READ       5
#define N_CMB_READ       7
#define N_TGTS          15



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
#include "RSDataWindow.h"
#include <TVectorT.h>
#include <TGraphErrors.h>


class QwGUIGoodForSettings {
public:
  QwGUIGoodForSettings();
  ~QwGUIGoodForSettings(){
    fGoodForLabels.clear();
    fGoodForIDs.clear();
    fGoodForReject.clear();
    fQualityLabels.clear();
    fQualityIDs.clear();
    fQualityReject.clear();
  }

  std::string GetSelectionString(std::string table = "");

private:
  std::vector<std::string> fGoodForLabels;
  std::vector<std::string> fQualityLabels;
  std::vector<Int_t>  fGoodForIDs;
  std::vector<Int_t>  fQualityIDs;

  Bool_t fGoodForRejectNULLs;
  std::vector<Bool_t>  fGoodForReject;
  Bool_t fQualityRejectNULLs;
  std::vector<Bool_t>  fQualityReject;

};



 class QwGUIDatabase : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGVerticalFrame     *dControlsFrame;
  TRootEmbeddedCanvas *dCanvas;  
  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dCmbLayout;
  TGLayoutHints       *dNumLayout;
  TGLayoutHints       *dBtnLayout;
  TGLayoutHints       *dLabLayout;
  TGNumberEntry       *dNumStartRun;
  TGNumberEntry       *dNumStopRun;
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
  TGLabel             *dLabStartRun;
  TGLabel             *dLabStopRun;
  TGLabel             *dLabTarget;
  TGLabel             *dLabRegression;
  TGLabel             *dLabPlot;


  //!An object array to store histogram pointers -- good for use in cleanup.
  TObjArray            GraphArray;
  
  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;


  //!A histogram array to plot the X and Y position difference variation.
  TPaveText * errlabel;


  //!This function just plots some histograms in the main canvas, just for illustrative purposes
  //!for now.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none
  // void                 PlotData();

  //  void PlotPosData();


/*   void PlotChargeData(); */

  void DetectorPlot();
#ifndef  ROOTCINTMODE
  mysqlpp::StoreQueryResult QueryDetector(TString detector, TString measured_property, Int_t det_id);
#endif
  void HistogramDetector(TString detector, TString measured_property, Int_t det_id);
  void PlotDetector(TString detector, TString measured_property, Int_t det_id);
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

  // measurement types
  static const char   *OtherMeasurementTypes[N_MEAS_TYPES];
  static const char   *ChargeMeasurementTypes[N_Q_MEAS_TYPES];
  static const char   *PositionMeasurementTypes[N_POS_MEAS_TYPES];
  static const char   *DetectorMeasurementTypes[N_DET_MEAS_TYPES];

  // properties
  static const char   *Subblocks[N_SUBBLOCKS];
  static const char   *BPMReadings[N_BPM_READ];
  static const char   *ComboBPMReadings[N_CMB_READ];

  // target types
  static const char   *Targets[N_TGTS];
  static const char   *Plots[2];

  // static array for temporary measurement type storing. This makes things easier when trying to
  // retrieave data.
  std::vector<TString> measurements;

 private:

  QwGUIGoodForSettings fGoodForSelection;

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
  virtual void        OnReceiveMessage(char*);
  virtual void        OnRemoveThisTab();
  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);
  void                PopulateDetectorComboBox();
  void                PopulateMeasurementComboBox();
  void                PopulateXDetComboBox();
  
  ClassDef(QwGUIDatabase,0); 

 };

#endif
