#include "QwGUIDatabase.h"

#include <TG3DLine.h>
#include "TGaxis.h"
#include "TGTextEntry.h"
#include "TStopwatch.h"
#include "TLatex.h"
#include "TDatime.h"

ClassImp(QwGUIDatabase);

using namespace std;

// Parameters to plot in X axis
enum EQwGUIDatabaseXAxisIDs {
  ID_X_RUN,
  ID_X_SLUG,
  ID_X_TIME,
  ID_X_TIME_RUNLET,
  ID_X_HISTO,
  ID_X_PULL_PLOT,
  ID_X_WEIN,
  ID_X_BEAM,
  ID_TGT_X,
  ID_TGT_Y,
  ID_TGT_XSLOPE,
  ID_TGT_YSLOPE,
  ID_TGT_Q,
  ID_TGT_E
};

// Device IDs
enum EQwGUIDatabaseInstrumentIDs {
  ID_MD,
  ID_LUMI,
  ID_BEAM,
  ID_CMB_BPM,
  ID_CMB_BCM,
  ID_E_CAL,
  ID_BPM,
  ID_BCM,
  ID_BCM_DD,
  ID_MD_SENS,
  ID_LUMI_SENS
};

enum EQwGUIDatabaseWidgetIDs {
  NUM_START_RUN,
  NUM_STOP_RUN,
  CMB_DETECTOR,
  CMB_SUBBLOCK,
  CMB_MEASUREMENT_TYPE,
  CMB_INSTRUMENT,
  CMB_PROPERTY,
  BTN_SUBMIT,
  CMB_XAXIS,
  CMB_XDET,
  CMB_TGT,
  CMB_REGRESS,
  CMB_PLOT,
  BOX_GOODFOR,
  };


const char *QwGUIDatabase::DetectorCombos[N_DETECTORS] = 
  {
	"mdallpmtavg",
	"mdallbarsavg",
	"md1pmtavg",
	"md2pmtavg",
	"md3pmtavg",
	"md4pmtavg",
	"md5pmtavg",
	"md6pmtavg",
	"md7pmtavg",
	"md8pmtavg",
	"md9pmtavg",
	"qwk_mdallbars",
    "qwk_mdoddbars",
    "qwk_mdevenbars",
    "qwk_md1barsum",
    "qwk_md2barsum",
    "qwk_md3barsum",
    "qwk_md4barsum",
    "qwk_md5barsum",
    "qwk_md6barsum",
    "qwk_md7barsum",
    "qwk_md8barsum",
    "qwk_md9barsum",
    "qwk_md1_qwk_md5",
    "qwk_md2_qwk_md6",
    "qwk_md3_qwk_md7",
    "qwk_md4_qwk_md8",
    "dd_mdeven_odd",
    "dd_md15_37",
    "dd_md26_48"
    "qwk_md1neg",
    "qwk_md2neg",
    "qwk_md3neg",
    "qwk_md4neg",
    "qwk_md5neg",
    "qwk_md6neg",
    "qwk_md7neg",
    "qwk_md8neg",
    "qwk_md1pos",
    "qwk_md2pos",
    "qwk_md3pos",
    "qwk_md4pos",
    "qwk_md5pos",
    "qwk_md6pos",
    "qwk_md7pos",
    "qwk_md8pos",
    "qwk_md9neg",
    "qwk_md9pos",
    "qwk_pmtled",
    "qwk_pmtonl",
    "qwk_pmtltg",
    "qwk_isourc",
    "qwk_preamp",
    "qwk_cagesr",
    "md15_dipole",
    "md26_dipole",
    "md37_dipole",
    "md48_dipole",
    "mddipole_h",
    "mddipole_v"
  };

const char *QwGUIDatabase::BeamPositionMonitors[N_BPMS] = 
{
  "qwk_bpm3c07","qwk_bpm3c08","qwk_bpm3c11","qwk_bpm3c12",
  "qwk_bpm3c14","qwk_bpm3c16","qwk_bpm3c17","qwk_bpm3c18",
  "qwk_bpm3c19","qwk_bpm3p02a","qwk_bpm3p02b","qwk_bpm3p03a",
  "qwk_bpm3c20","qwk_bpm3c21","qwk_bpm3h02","qwk_bpm3h04",
  "qwk_bpm3h07a","qwk_bpm3h07b","qwk_bpm3h07c","qwk_bpm3h08",
  "qwk_bpm3h09","qwk_bpm3h09b"
 
};

// bcms
const char *QwGUIDatabase::BeamCurrentMonitors[N_BCMS] =
{
  "qwk_bcm1","qwk_bcm2","qwk_bcm5","qwk_bcm6","qwk_bcm7","qwk_bcm8",
  "qwk_bcmgl1","qwk_bcmgl2","bcmddgl",
  "qwk_linephase", "qwk_invert_tstable"
};

// bcm double differences
const char *QwGUIDatabase::BCMDoubleDifferences[N_BCM_DD] =
{
  "bcmdd12","bcmdd15","bcmdd16", "bcmdd17","bcmdd18",
  "bcmdd25", "bcmdd26","bcmdd27", "bcmdd28",
  "bcmdd56", "bcmdd57","bcmdd58", 
  "bcmdd67","bcmdd68", "bcmdd78"
};

const char *QwGUIDatabase::LumiCombos[N_LUMIS] = 
{

  "qwk_dslumi1",
  "qwk_dslumi2",
  "qwk_dslumi3",
  "qwk_dslumi4",
  "qwk_dslumi5",
  "qwk_dslumi6",
  "qwk_dslumi7",
  "qwk_dslumi8",
  "uslumi1_sum",
  "uslumi3_sum",
  "uslumi5_sum",
  "uslumi7_sum",
  "qwk_uslumi1neg",
  "qwk_uslumi1pos",
  "qwk_uslumi3neg",
  "qwk_uslumi3pos",
  "qwk_uslumi5neg",
  "qwk_uslumi5pos",
  "qwk_uslumi7neg",
  "qwk_uslumi7pos",
  "uslumi1_uslumi5_sum",
  "uslumi3_uslumi7_sum",
  "uslumi4_uslumi8_sum",
  "uslumi1_uslumi5_sum",
  "uslumi1_uslumi5_diff",
  "uslumi3_uslumi7_diff",
  "uslumi4_uslumi8_diff",
  "uslumi1_uslumi5_diff",
  "dslumi_odd",
  "dslumi_even", 
  "dslumi_sum",
  "uslumi_sum"
};


const char *QwGUIDatabase::BPMReadings[N_BPM_READ] = {

  "X","Y","RelX","RelY","_EffectiveCharge"
};


const char *QwGUIDatabase::ComboBPMReadings[N_CMB_READ] = {

  "X","Y","XSlope","YSlope","XIntercept","YIntercept","_EffectiveCharge"
};

const char *QwGUIDatabase::CombinedBCMS[N_CMB_BCMS] = {

  "qwk_charge"
};

const char *QwGUIDatabase::CombinedBPMS[N_CMB_BPMS] = {

  "qwk_target"
};

const char *QwGUIDatabase::EnergyCalculators[N_ENERGY] = {

  "qwk_energy"
};


/*
Available slope types and slope calculation types from qw_run2_pass1.
(as of 01/04/2012)
+-----------------------+-------------------+
| slope                 | slope_calculation |
+-----------------------+-------------------+
| wrt_diff_targetX      | on                |
| wrt_diff_targetY      | on                |
| wrt_diff_targetXSlope | on                |
| wrt_diff_targetYSlope | on                |
| wrt_diff_energy       | on                |
| wrt_diff_targetX      | on_5+1            |
| wrt_diff_targetY      | on_5+1            |
| wrt_diff_targetXSlope | on_5+1            |
| wrt_diff_targetYSlope | on_5+1            |
| wrt_diff_energy       | on_5+1            |
| wrt_asym_charge       | on_5+1            |
| wrt_diff_targetX      | on_set3           |
| wrt_diff_targetY      | on_set3           |
| wrt_diff_targetXSlope | on_set3           |
| wrt_diff_targetYSlope | on_set3           |
| wrt_diff_bpm3c12X     | on_set3           |
| wrt_asym_charge       | on_set3           |
| wrt_diff_targetX      | on_set4           |
| wrt_diff_targetY      | on_set4           |
| wrt_diff_targetXSlope | on_set4           |
| wrt_diff_targetYSlope | on_set4           |
| wrt_diff_energy       | on_set4           |
| wrt_asym_bcm5         | on_set4           |
| wrt_diff_9b_p_4X      | on_set5           |
| wrt_diff_9b_p_4Y      | on_set5           |
| wrt_diff_9b_m_4X      | on_set5           |
| wrt_diff_9b_m_4Y      | on_set5           |
| wrt_diff_bpm3c12X     | on_set5           |
| wrt_diff_9b_p_4X      | on_set6           |
| wrt_diff_9b_p_4Y      | on_set6           |
| wrt_diff_9b_m_4X      | on_set6           |
| wrt_diff_9b_m_4Y      | on_set6           |
| wrt_diff_bpm3c12X     | on_set6           |
| wrt_asym_charge       | on_set6           |
|

*/

const char *QwGUIDatabase::RegressionVarsOn[N_REG_VARS_ON]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy"
};


const char *QwGUIDatabase::RegressionVarsOn_5_1[N_REG_VARS_ON_5_1]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy","wrt_asym_charge"

};

const char   *QwGUIDatabase::RegressionVarsOn_3[N_REG_VARS_ON_3]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_bpm3c12X", 
  "wrt_asym_charge",
};

const char   *QwGUIDatabase::RegressionVarsOn_4[N_REG_VARS_ON_4]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy",
  "wrt_asym_bcm5",
};

const char   *QwGUIDatabase::RegressionVarsOn_5[N_REG_VARS_ON_5]={
  "wrt_diff_9b_p_4X","wrt_diff_9b_m_4X","wrt_diff_9b_m_4Y","wrt_diff_9b_p_4Y","wrt_diff_bpm3c12X"
};

const char   *QwGUIDatabase::RegressionVarsOn_6[N_REG_VARS_ON_6]={
  "wrt_diff_9b_p_4X","wrt_diff_9b_m_4X","wrt_diff_9b_m_4Y","wrt_diff_9b_p_4Y","wrt_diff_bpm3c12X","wrt_asym_charge"
};

const char   *QwGUIDatabase::RegressionVarsOn_7[N_REG_VARS_ON_7]={
  "wrt_diff_9_p_4X","wrt_diff_9_m_4X","wrt_diff_9_m_4Y","wrt_diff_9_p_4Y","wrt_diff_bpm3c12X"
};

const char   *QwGUIDatabase::RegressionVarsOn_8[N_REG_VARS_ON_8]={
  "wrt_diff_9_p_4X","wrt_diff_9_m_4X","wrt_diff_9_m_4Y","wrt_diff_9_p_4Y","wrt_diff_bpm3c12X","wrt_asym_charge"
};

const char   *QwGUIDatabase::RegressionVarsOn_9[N_REG_VARS_ON_9]={
		"wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy","wrt_asym_charge", "wrt_asym_uslumi_sum"};

const char   *QwGUIDatabase::RegressionVarsOn_10[N_REG_VARS_ON_10]={
		"wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy","wrt_asym_bcm6"};

const char   *QwGUIDatabase::RegressionVarsOn_11[N_REG_VARS_ON_11]={
		"wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_bpm3c12X"};

const char   *QwGUIDatabase::RegressionVarsOn_12[N_REG_VARS_ON_12]={
		"wrt_diff_target_plus4X","wrt_diff_target_plus4Y","wrt_diff_target_plus4XSlope","wrt_diff_target_plus4YSlope","wrt_diff_bpm3c12X"};

// Regression types
// To get the raw data from QwAnalysis that pass the standard event cuts use "raw(from rootfiles)"
// To get the unregressed data that pass LinRegBlue event cuts use "off"
const char *QwGUIDatabase::RegressionSchemes[N_REGRESSION_SCHEMES] = {
  "off","on","on_5+1", "on_set3", "on_set4", "on_set5","on_set6","on_set7",
  	  "on_set8","on_set9","on_set10","on_set11","on_set12","raw(from rootfiles)"
};



/**

+---------------------+---------------+-----------------------------------+
| measurement_type_id | units         | title                             |
+---------------------+---------------+-----------------------------------+
| a                   | ppb           | Helicity-Correlated Asymmetry     | 
| a12                 | ppb           | First-Half/Second-Half Asymmetry  | 
| aeo                 | ppb           | Even/Odd Asymmetry                | 
| d                   | (?)           | Helicity-Correlated Difference    | 
| d12                 | (?)           | First-Half/Second-Half Difference | 
| deo                 | (?)           | Even/Odd Difference               | 
| p                   | percent       | Beam Polarization                 | 
| r                   | channel       | Raw Signal                        | 
| y                   | counts/nC (?) | Yield                             | 
| ya                  | rad (?)       | Beam Angle                        | 
| ye                  | MeV           | Beam Energy                       | 
| ym                  | mm/m (?)      | Beam Slope along Z                | 
| yp                  | mm            | Beam Position                     | 
| yq                  | nC (?)        | Beam Charge                       | 
+---------------------+---------------+-----------------------------------+

bcms/bpms effective charge - yq, a, r
bpms absolute positions - yp, d, r
main_detectors/lumis - a, y
*/

const char *QwGUIDatabase::ChargeMeasurementTypes[N_Q_MEAS_TYPES]=
  {
    "a","yq","a12", "aeo", "r", "d"
  };

const char *QwGUIDatabase::PositionMeasurementTypes[N_POS_MEAS_TYPES]=
  {
    "d", "yp", "d12", "deo","r"
  };

const char *QwGUIDatabase::DetectorMeasurementTypes[N_DET_MEAS_TYPES]=
  {
    "a", "y", "a12", "aeo", "r"
  };

const char *QwGUIDatabase::OtherMeasurementTypes[N_MEAS_TYPES]=
  {
    "ya", "ym", "ye","p"
  };

const char *QwGUIDatabase::SensitivityTypes[N_SENS_TYPES]= {
	"s"
};

const char *QwGUIDatabase::Subblocks[N_SUBBLOCKS] = 
  {
  "0", "1", "2", "3", "4"
  };


/** Target types in the DB
    +------------------+
    | target_position  |
    +------------------+
    |                  | 
    | HOME=OutOfBeam   | 
    | HYDROGEN-CELL    | 
    | DS-8%-Aluminum   | 
    | DS-4%-Aluminum   | 
    | DS-2%-Aluminum   | 
    | US-4%-Aluminum   | 
    | US-2%-Aluminum   | 
    | US-Empty-Frame   | 
    | US-1%-Aluminum   | 
    | Optics-3         | 
    | Unknown position | 
    | US-Pure-Al       | 
    | DS-Cntring-hole  | 
    | DS-0.5%-C        | 
    | US-Cntring-hole  | 
    +------------------+
*/

const char *QwGUIDatabase::Targets[N_TGTS] = 
{
  "HOME=OutOfBeam","HYDROGEN-CELL","DS-8%-Aluminum","DS-4%-Aluminum","DS-2%-Aluminum",
  "US-4%-Aluminum","US-2%-Aluminum","US-1%-Aluminum","US-Empty-Frame","Optics-3",
  "Unknown position","US-Pure-Al","DS-Cntring-hole","DS-0.5%-C","US-Cntring-hole","All"
};


const char *QwGUIDatabase::Plots[N_Plots] =
  {
    "Mean", "RMS", "Both"
  }; 

/** good_for_types in the DB as of 23/04/2012
 +-------------+-----------------------+
 | good_for_id | type                  |
 +-------------+-----------------------+
 |           1 | production            |
 |           2 | commissioning         |
 |           3 | parity                |
 |           4 | tracking              |
 |           5 | centering_target      |
 |           6 | centering_plug        |
 |           7 | pedestals             |
 |           8 | transverse            |
 |           9 | transverse_horizontal |
 |          10 | daq_test              |
 |          11 | bcm_scan              |
 |          12 | bpm_scan              |
 |          13 | ia_scan               |
 |          14 | pita_scan             |
 |          15 | rhwp_scan             |
 |          16 | background_studies    |
 |          17 | pockels_cell_off      |
 |          18 | n_to_delta            |
 |          19 | junk                  |
 |          20 | scanner_on            |
 |          21 | dis                   |
 +-------------+-----------------------+
*/

const char *QwGUIDatabase::GoodForTypes[N_GOODFOR_TYPES] =
{
  "production","commissioning","parity","tracking",
  "centering_target","centering_plug","pedestals","transverse",
  "transverse_horizontal","daq_test","bcm_scan","bpm_scan",
  "ia_scan","pita_scan","rhwp_scan","background_studies",
  "pockels_cell_off","n_to_delta","junk","scanner_on","dis"
};

const char *QwGUIDatabase::X_axis[N_X_AXIS] =
{
  "Vs. Run Number","Vs. Slug","Vs. Run start time","Vs. Runlet start time","Vs. Wein","Runlet Histogram","Runlet Pull Plot"
};


QwGUIDatabase::QwGUIDatabase(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dQualityFrame	      = NULL;
  dRunFrame	          = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dCmbLayout          = NULL;
  dNumLayout          = NULL;
  dBtnLayout          = NULL;
  dLabLayout          = NULL;
  dChkLayout	      = NULL;
  dBoxLayout	      = NULL;
  dChkQualityGood     = NULL;
  dChkQualitySuspect  = NULL;
  dChkQualityBad      = NULL;
  dBoxGoodFor	      = NULL;
  dCmbXAxis           = NULL;
  dCmbInstrument      = NULL;
  dCmbDetector        = NULL;
  dCmbProperty        = NULL;
  dCmbSubblock        = NULL;
  dCmbTargetType      = NULL;
  dCmbRegressionType  = NULL;
  dCmbPlotType        = NULL;
  dCmbMeasurementType = NULL;
  dNumStartRun        = NULL;
  dNumStopRun         = NULL;
  dBtnSubmit          = NULL;
  dLabRunRange        = NULL;
  dLabTarget          = NULL;
  dLabRegression      = NULL;
  dLabPlot            = NULL;

  RemoveSelectedDataWindow();
  
  GraphArray.Clear();
  LegendArray.Clear();

  DataWindowArray.Clear();

  property = "";
  detector = "";
  measurement_type = "";
  device = "";
  target = "";
  plot="";
  index_first = 0;
  index_last = 0;
  det_id = 0;
  x_axis = 0;
  subblock = 0;
  measurements = NULL;
  reg_iv="";



  AddThisTab(this);

}

QwGUIDatabase::~QwGUIDatabase()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dQualityFrame)       delete dQualityFrame;
  if(dRunFrame)		  delete dRunFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dCmbLayout)          delete dCmbLayout;
  if(dNumLayout)          delete dNumLayout;
  if(dLabLayout)          delete dLabLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dChkLayout)	  delete dChkLayout;
  if(dBoxLayout)	  delete dBoxLayout;
  if(dChkQualityGood)	  delete dChkQualityGood;
  if(dChkQualitySuspect)  delete dChkQualitySuspect;
  if(dChkQualityBad)	  delete dChkQualityBad;
  if(dBoxGoodFor) 	  delete dBoxGoodFor;
  if(dLabRunRange)        delete dLabRunRange;
  if(dNumStartRun)        delete dNumStartRun;
  if(dLabTarget)          delete dLabTarget;
  if(dLabRegression)      delete dLabRegression;
  if(dLabPlot)            delete dLabPlot;
  if(dLabInstrument)      delete dLabInstrument;
  if(dLabDetector)        delete dLabDetector;
  if(dLabMeasurement)     delete dLabMeasurement;
  if(dLabSubblock)        delete dLabSubblock;
  if(dNumStopRun)         delete dNumStopRun;
  if(dCmbInstrument)      delete dCmbInstrument;
  if(dCmbXAxis)           delete dCmbXAxis;
  if(dCmbDetector)        delete dCmbDetector;
  if(dCmbProperty)        delete dCmbProperty;
  if(dCmbSubblock)        delete dCmbSubblock;
  if(dCmbTargetType)      delete dCmbTargetType;
  if(dCmbRegressionType)  delete dCmbRegressionType;
  if(dCmbMeasurementType) delete dCmbMeasurementType;
  if(dCmbPlotType)        delete dCmbPlotType;
  if(dBtnSubmit)          delete dBtnSubmit;

  RemoveThisTab(this);
  IsClosing(GetName());
}



void QwGUIDatabase::MakeLayout()
{

  SetCleanup(kDeepCleanup);
  
  //Set Styles
  //stats
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(0);  
  gStyle->SetStatH(0.25);
  gStyle->SetStatW(0.3);     
 

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadBottomMargin(4.0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleOffset(2.5);
  gStyle->SetTitleColor(kBlack);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");


  dTabFrame = new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandY | kLHintsExpandX, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator  = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas             = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  dLabInstrument      = new TGLabel(dControlsFrame, "Instrument Type");
  dCmbInstrument      = new TGComboBox(dControlsFrame, CMB_INSTRUMENT);
  dLabXAxis	      = new TGLabel(dControlsFrame, "X-Axis");
  dCmbXAxis           = new TGComboBox(dControlsFrame, CMB_XAXIS);
  dLabMeasurement     = new TGLabel(dControlsFrame, "Measurement Type");
  dCmbMeasurementType = new TGComboBox(dControlsFrame, CMB_MEASUREMENT_TYPE);
  dLabDetector	      = new TGLabel(dControlsFrame, "Detector");
  dCmbDetector        = new TGComboBox(dControlsFrame, CMB_DETECTOR);
  dLabProperty	      = new TGLabel(dControlsFrame, "Detector Property/Regression IV");
  dCmbProperty        = new TGComboBox(dControlsFrame, CMB_PROPERTY);
  dLabSubblock	      = new TGLabel(dControlsFrame, "Subblock");
  dCmbSubblock        = new TGComboBox(dControlsFrame, CMB_SUBBLOCK);
  dLabTarget          = new TGLabel(dControlsFrame, "Target");
  dCmbTargetType      = new TGComboBox(dControlsFrame, CMB_TGT);
  dLabRegression      = new TGLabel(dControlsFrame, "Regression Type");
  dCmbRegressionType  = new TGComboBox(dControlsFrame, CMB_REGRESS);
  dLabQuality	      = new TGLabel(dControlsFrame,"Data Quality");
  dQualityFrame       = new TGHorizontalFrame(dControlsFrame);
  dChkQualityGood     = new TGCheckButton(dQualityFrame,"Good");
  dChkQualitySuspect  = new TGCheckButton(dQualityFrame,"Susp.");
  dChkQualityBad      = new TGCheckButton(dQualityFrame,"Bad");
  dLabGoodFor	      = new TGLabel(dControlsFrame, "Good For");
  dBoxGoodFor	      = new TGListBox(dControlsFrame, BOX_GOODFOR);
  dLabPlot            = new TGLabel(dControlsFrame, "Plot Type");
  dCmbPlotType        = new TGComboBox(dControlsFrame, CMB_PLOT);
  dLabRunRange        = new TGLabel(dControlsFrame, "Run/Slug Range");
  dRunFrame	      	  = new TGHorizontalFrame(dControlsFrame);
  dNumStartRun        = new TGNumberEntry(dRunFrame, 9594, 5, NUM_START_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dNumStopRun         = new TGNumberEntry(dRunFrame, 9812, 5, NUM_STOP_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dBtnSubmit          = new TGTextButton(dControlsFrame, "&Submit", BTN_SUBMIT);  //dSlowControls       = new TGButtonGroup(dControlsFrame, "Slow Controls");


  dLabLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 0, 0, 10, 0 );
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 0, 0, 0, 0 );
  dNumLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 10, 10, 0, 0 );
  dChkLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 0, 0, 0, 0 );
  dBtnLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 10, 0 );
  dFrmLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 0, 0 );
  dBoxLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 0, 0 );


  dCmbInstrument->AddEntry("Main Detectors", ID_MD);
  dCmbInstrument->AddEntry("BPMs", ID_BPM);
  dCmbInstrument->AddEntry("BCMs", ID_BCM);
  dCmbInstrument->AddEntry("BCM Double Difference", ID_BCM_DD);
  dCmbInstrument->AddEntry("LUMI Detectors", ID_LUMI);
  dCmbInstrument->AddEntry("Combined BPMs", ID_CMB_BPM);
  dCmbInstrument->AddEntry("Combined BCMs", ID_CMB_BCM);
  dCmbInstrument->AddEntry("Energy Calculator", ID_E_CAL);
  dCmbInstrument->AddEntry("Main Detector Sensitivities", ID_MD_SENS);
  dCmbInstrument->AddEntry("LUMI Detector Sensitivities", ID_LUMI_SENS);
  dCmbInstrument->Connect("Selected(Int_t)","QwGUIDatabase", this, "PopulateDetectorComboBox()");
  dCmbInstrument->Select(dCmbInstrument->FindEntry("Main Detectors")->EntryId());

  dCmbProperty->Connect("Selected(Int_t)","QwGUIDatabase", this, "PopulateMeasurementComboBox()");


  PopulateRegComboBox();
  dCmbRegressionType->Select(0);

  dCmbMeasurementType->Connect("Selected(Int_t)","QwGUIDatabase",this,"PopulateRegComboBox()");

  // Populate data blocks
  for (Int_t i = 0; i < N_SUBBLOCKS; i++) {
    dCmbSubblock->AddEntry(Subblocks[i], i);
  }
  dCmbSubblock->Select(0);

  // Populate target combo box
  for (Int_t i = 0; i < N_TGTS; i++) {
    dCmbTargetType->AddEntry(Targets[i], i);
  }
  dCmbTargetType->Select(1);


  // Populate X axis box. For now we'll ignore the Wien option.
  dCmbXAxis->AddEntry(X_axis[0],ID_X_RUN);
  dCmbXAxis->AddEntry(X_axis[1],ID_X_SLUG);
  dCmbXAxis->AddEntry(X_axis[2],ID_X_TIME);
  dCmbXAxis->AddEntry(X_axis[3],ID_X_TIME_RUNLET);
  dCmbXAxis->AddEntry(X_axis[5],ID_X_HISTO);
  dCmbXAxis->AddEntry(X_axis[6],ID_X_PULL_PLOT);
  dCmbXAxis->Select(0);


  // Populate good for list
  for (Int_t i = 0; i<N_GOODFOR_TYPES; i++){
    dBoxGoodFor->AddEntry(GoodForTypes[i],i+1);
  }

  // Populate plot type
  for (Int_t k = 0; k < N_Plots; k++)
    dCmbPlotType->AddEntry(Plots[k], k);
  dCmbPlotType->Select(0);


  dCmbXAxis           -> Resize(150,20);
  dCmbInstrument      -> Resize(150,20);
  dCmbDetector        -> Resize(150,20);
  dCmbProperty        -> Resize(150,20);
  dCmbSubblock        -> Resize(150,20);
  dCmbMeasurementType -> Resize(150,20);
  dCmbTargetType      -> Resize(150,20);
  dCmbRegressionType  -> Resize(150,20);
  dCmbPlotType        -> Resize(150,20);
  dBoxGoodFor         -> Resize(150,80);


  dControlsFrame      -> AddFrame(dLabInstrument, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbInstrument, dCmbLayout ); // detector type
  dControlsFrame      -> AddFrame(dLabDetector, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbDetector, dCmbLayout );   // detector name
  dControlsFrame      -> AddFrame(dLabProperty, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbProperty, dCmbLayout );   // detector property
  dControlsFrame      -> AddFrame(dLabMeasurement, dLabLayout);
  dControlsFrame      -> AddFrame(dCmbMeasurementType, dCmbLayout ); // measurement type
  dControlsFrame      -> AddFrame(dLabSubblock, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbSubblock, dCmbLayout );   // block type
  dControlsFrame      -> AddFrame(dLabXAxis, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbXAxis, dCmbLayout );		//x-axis
  dControlsFrame      -> AddFrame(dLabTarget, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbTargetType, dCmbLayout );		//target
  dControlsFrame      -> AddFrame(dLabRegression, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbRegressionType, dCmbLayout );		//regression
  dControlsFrame      -> AddFrame(dLabQuality, dLabLayout);
  dControlsFrame      -> AddFrame(dQualityFrame, dFrmLayout);
  dQualityFrame	      -> AddFrame(dChkQualityGood, dChkLayout );
  dChkQualityGood     ->SetState(kButtonDown, kTRUE); //default quality to "good"
  dQualityFrame	      -> AddFrame(dChkQualitySuspect, dChkLayout );
  dQualityFrame	      -> AddFrame(dChkQualityBad, dChkLayout );
  dControlsFrame	  -> AddFrame(dLabGoodFor, dLabLayout);
  dControlsFrame	  -> AddFrame(dBoxGoodFor, dBoxLayout);
  dBoxGoodFor		  -> SetMultipleSelections(kTRUE);
  dBoxGoodFor		  -> Select(dBoxGoodFor->FindEntry("production")->EntryId());
  dBoxGoodFor		  -> Select(dBoxGoodFor->FindEntry("parity")->EntryId());
  dControlsFrame      -> AddFrame(dLabRunRange, dLabLayout );
  dControlsFrame      -> AddFrame(dRunFrame,dFrmLayout);
  dRunFrame           -> AddFrame(dNumStartRun, dNumLayout );
  dRunFrame           -> AddFrame(dNumStopRun, dNumLayout );
  dControlsFrame      -> AddFrame(dLabPlot, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbPlotType, dCmbLayout );
  dControlsFrame      -> AddFrame(dBtnSubmit, dBtnLayout);

  dCmbMeasurementType ->Select(dCmbMeasurementType->FindEntry("a")->EntryId());

  dCmbXAxis           -> Associate(this);
  dCmbInstrument      -> Associate(this);
  dNumStartRun        -> Associate(this);
  dNumStopRun         -> Associate(this);
  dCmbDetector        -> Associate(this);
  dCmbProperty        -> Associate(this);
  dCmbSubblock        -> Associate(this);
  dCmbMeasurementType -> Associate(this);
  dCmbTargetType      -> Associate(this);
  dCmbRegressionType  -> Associate(this);
  dCmbPlotType        -> Associate(this);
  dChkQualityGood     -> Associate(this);
  dChkQualitySuspect  -> Associate(this);
  dChkQualityBad      -> Associate(this);
  dBoxGoodFor	      -> Associate(this);
  dBtnSubmit          -> Associate(this);

  dCanvas -> GetCanvas() -> SetBorderMode(0);

  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  mc->Initialize();
  mc->SetFillColor(0);
  mc->SetHighLightColor(0);
  dCanvas->AdoptCanvas(mc);
  dCanvas -> GetCanvas() -> Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "QwGUIDatabase", this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

}


/********************************************

Populate the combo boxes based on the detector selection

****************************************** */
void QwGUIDatabase::PopulateDetectorComboBox()
{
  dCmbDetector->RemoveAll();
  dCmbProperty->RemoveAll();
  dCmbMeasurementType->RemoveAll();
  dCmbProperty->SetEnabled(kFALSE);
  dCmbSubblock->SetEnabled(kTRUE);
  dCmbRegressionType->SetEnabled(kTRUE);
  dCmbMeasurementType->SetEnabled(kTRUE);

  if (dCmbInstrument->GetSelected() == ID_MD) {
    for (Int_t i = 0; i < N_DETECTORS; i++) {
      dCmbDetector->AddEntry(DetectorCombos[i], i);
    }
    for (Int_t i = 0; i < N_DET_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(DetectorMeasurementTypes[i], i);
    }
    measurements = DetectorMeasurementTypes;
  }


  if (dCmbInstrument->GetSelected() == ID_LUMI) {

    for (Int_t i = 0; i < N_LUMIS; i++) {
      dCmbDetector->AddEntry(LumiCombos[i], i);
    }
    for (Int_t i = 0; i < N_DET_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(DetectorMeasurementTypes[i], i);
    }
    measurements=DetectorMeasurementTypes;
  }

  if (dCmbInstrument->GetSelected() == ID_BPM) {
    dCmbProperty->SetEnabled(kTRUE);
    for (Int_t i = 0; i < N_BPMS; i++) {
      dCmbDetector->AddEntry(BeamPositionMonitors[i], i);
    }
    for (Int_t i = 0; i < N_BPM_READ; i++) {
      dCmbProperty->AddEntry(BPMReadings[i], i);
    }
  }

  if (dCmbInstrument->GetSelected() == ID_BCM) {
    for (Int_t i = 0; i < N_BCMS; i++) {
      dCmbDetector->AddEntry(BeamCurrentMonitors[i], i);
    }
    for (Int_t i = 0; i < N_Q_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[i], i);
    }
    measurements = ChargeMeasurementTypes;
  }

  if (dCmbInstrument->GetSelected() == ID_BCM_DD) {
    //dCmbRegressionType->SetEnabled(kFALSE);
    for (Int_t i = 0; i < N_BCM_DD; i++) {
      dCmbDetector->AddEntry(BCMDoubleDifferences[i], i);
    }
    dCmbMeasurementType->AddEntry("d", 0);
    measurements =PositionMeasurementTypes; // Here I am going to use this array for the sake of using an array
    // with "d" in it. 
  }

  if (dCmbInstrument->GetSelected() == ID_CMB_BPM) {
    dCmbProperty->SetEnabled(kTRUE);
    for (Int_t i = 0; i < N_CMB_BPMS; i++) {
      dCmbDetector->AddEntry(CombinedBPMS[i], i);
    }
    for (Int_t i = 0; i < N_CMB_READ; i++) {
      dCmbProperty->AddEntry(ComboBPMReadings[i], i);
    }
  }

  if (dCmbInstrument->GetSelected() == ID_CMB_BCM) {
    for (Int_t i = 0; i < N_CMB_BCMS; i++) {
      dCmbDetector->AddEntry(CombinedBCMS[i],i);
    }
    for (Int_t i = 0; i < N_Q_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[i],i);
    }
    measurements = ChargeMeasurementTypes;
  }

  if (dCmbInstrument->GetSelected() == ID_E_CAL) {
    for (Int_t i = 0; i < N_ENERGY; i++) {
      dCmbDetector->AddEntry(EnergyCalculators[i], i);
    }
    for (Int_t i = 0; i < N_POS_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(PositionMeasurementTypes[i], i);
    }
    measurements = PositionMeasurementTypes;
  }

  if (dCmbInstrument->GetSelected() == ID_MD_SENS) {
    dCmbProperty->SetEnabled(kTRUE);
    dCmbSubblock->SetEnabled(kFALSE);
    dCmbMeasurementType->SetEnabled(kFALSE);
    
    for (Int_t i = 0; i < N_DETECTORS; i++) {
      dCmbDetector->AddEntry(DetectorCombos[i], i);
    }
  }

  if (dCmbInstrument->GetSelected() == ID_LUMI_SENS) {
    dCmbProperty->SetEnabled(kTRUE);
    dCmbSubblock->SetEnabled(kFALSE);
    dCmbMeasurementType->SetEnabled(kFALSE);

    for (Int_t i = 0; i < N_LUMIS; i++) {
      dCmbDetector->AddEntry(LumiCombos[i], i);
    }
  }

  if((dCmbInstrument->GetSelected() == ID_MD_SENS) || (dCmbInstrument->GetSelected() == ID_LUMI_SENS)){

    dCmbProperty->AddEntry("Frist Select Regression Type",0);
    dCmbProperty->Select(0);
  }


  dCmbMeasurementType->Select(0);
  dCmbDetector->Select(0);
  dCmbProperty->Select(0);
}


/********************************************

Populate the combo boxes based on the property selection for the detectors 

****************************************** */
void QwGUIDatabase::PopulateMeasurementComboBox()
{
  dCmbMeasurementType->RemoveAll();

  // BPMs
  if (dCmbInstrument->GetSelected() == ID_BPM){
    if(dCmbProperty->GetSelected() == 4){ // effective charge in BPMReadings[]
      for (Int_t k = 0; k < N_Q_MEAS_TYPES; k++){
	dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[k], k);
      } 
      measurements = ChargeMeasurementTypes;
    }
    else{ // for X, Y, RelX, RelY
      for (Int_t k = 0; k < N_POS_MEAS_TYPES; k++){
	dCmbMeasurementType->AddEntry(PositionMeasurementTypes[k], k);
      }
      measurements = PositionMeasurementTypes;
    }
  }

  // Combined BPMs
  if (dCmbInstrument->GetSelected() == ID_CMB_BPM){
    if(dCmbProperty->GetSelected() == 6){ // effective charge in ComboBPMReadings[]
      for (Int_t k = 0; k < N_Q_MEAS_TYPES; k++) {
	dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[k], k);
      }
      measurements = ChargeMeasurementTypes;
    } 
    else {
      for (Int_t k = 0; k < N_POS_MEAS_TYPES; k++) {
	dCmbMeasurementType->AddEntry(PositionMeasurementTypes[k], k);
      }
      measurements = PositionMeasurementTypes;
    }    
  }

  dCmbMeasurementType->Select(0);
  
}


/********************************************

Populate the X axis detector combo box

****************************************** */

void QwGUIDatabase::PopulateRegComboBox()
{
	dCmbRegressionType->RemoveAll();

	if( (measurements[dCmbMeasurementType->GetSelected()]=="y")  ||
	    (measurements[dCmbMeasurementType->GetSelected()]=="yp") ||
		(measurements[dCmbMeasurementType->GetSelected()]=="yq") ){
			dCmbRegressionType->AddEntry(RegressionSchemes[N_REGRESSION_SCHEMES-1],N_REGRESSION_SCHEMES-1);
			dCmbRegressionType->Select(N_REGRESSION_SCHEMES-1);
	}
	else{
		// Populate regression combo box normally
		for (Int_t i = 0; i < N_REGRESSION_SCHEMES; i++) {
		  dCmbRegressionType->AddEntry(RegressionSchemes[i], i);
		}
		dCmbRegressionType->Connect("Selected(Int_t)","QwGUIDatabase", this, "RegressionTypeInfo()");
		dCmbRegressionType->Select(1);
	}
}


/********************************************

Populate the X axis detector combo box

****************************************** */

void QwGUIDatabase::PopulatePlotComboBox()
{
    for (Int_t k = 0; k < N_Plots; k++)
      dCmbPlotType->AddEntry(Plots[k], k);

} 



void QwGUIDatabase::OnReceiveMessage(char *msg)
{

  TObject *obj = NULL;
  Int_t ind = 0;
  TString message = msg;

  if(message.Contains("dDataWindow")){

    if(message.Contains("Add to")){

      message.ReplaceAll("Add to dDataWindow_",7,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
	ind = DataWindowArray.IndexOf(obj);
	SetSelectedDataWindow(ind);
      }
    }
    else if(message.Contains("Don't add to")){

      message.ReplaceAll("Don't add to dDataWindow_",13,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
	RemoveSelectedDataWindow();
      }
    }
  }
}

void QwGUIDatabase::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWindow")) return;

  printf("Received Message From: %s\n",obj);
}


QwGUIDataWindow *QwGUIDatabase::GetSelectedDataWindow()
{
  if(dSelectedDataWindow < 0 || dSelectedDataWindow > DataWindowArray.GetLast()) return NULL;

  return (QwGUIDataWindow*)DataWindowArray[dSelectedDataWindow];
}



void QwGUIDatabase::OnObjClose(char *obj)
{
  if(!obj) return;
  TString name = obj;

  if(name.Contains("dDataWindow")){
    QwGUIDataWindow* window = (QwGUIDataWindow*)DataWindowArray.Remove(DataWindowArray.FindObject(obj));
    if(window){
      if(window == GetSelectedDataWindow()) { RemoveSelectedDataWindow();}
    }
  }

  QwGUISubSystem::OnObjClose(obj);

}


void QwGUIDatabase::OnNewDataContainer(RDataContainer *cont)
{

  if (!cont) return;

  if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){
    ClearData();
    if (!dDatabaseCont->Connect()) {
      std::cerr << "No valid database handle.  Check connection to database."  << std::endl;
      exit(1);
    }
    dDatabaseCont->Disconnect();
  }

};

void QwGUIDatabase::PlotGraphs()
{
  // TString plot = Plots[dCmbPlotType->GetSelected()];
  Int_t ind = 0;
  TCanvas *mc = dCanvas->GetCanvas();
  if(plot.Contains("Both"))
    {
         mc->Divide(1,2);
         mc->cd(1);
    }

  TObject *obj;
  TLegend *leg;
  
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind+1);
    gPad->SetBottomMargin(0.2);
    ((TGraph*)obj)->Draw("ap");
    leg = (TLegend*)LegendArray.At(ind);
    if(leg!=NULL) leg->Draw("");
    ind++;
    gPad->Update();
    obj = next();
  }
  mc->Modified();
  mc->Update();

}

void QwGUIDatabase::AddNewGraph(TObject* grp, TObject* leg)
{
  GraphArray.Add(grp);
  LegendArray.Add(leg);
}

// TObject *QwGUIDatabase::GetGraph(Int_t ind)
// {
//   if(ind < 0 || ind >= GraphArray.GetEntries()) return NULL;

//   return GraphArray
// }

// TObject *QwGUIDatabase::GetLegend(Int_t ind)
// {

// }


void QwGUIDatabase::OnRemoveThisTab()
{

};

void QwGUIDatabase::ClearData()
{
  TObject *obj;
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){ 
    if(obj!=NULL)
      delete obj;
    obj = next();
  }
  GraphArray.Clear();//Clear();
  LegendArray.Clear();//Clear();

  DataWindowArray.Clear();

  vector <TObject*> obja;
  TIter *next1 = new TIter(DataWindowArray.MakeIterator());
  obj = next1->Next();
  while(obj){
    obja.push_back(obj);
    obj = next1->Next();
  }
  delete next1;

  for(uint l = 0; l < obja.size(); l++)
    delete obja[l];

  DataWindowArray.Clear();

}

//Stuff to do after the tab is selected
void QwGUIDatabase::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{

  QwGUIDataWindow *dDataWindow = GetSelectedDataWindow();
  Bool_t add = kFalse;
  TObject *plot = NULL;
  TLegend *legend = NULL;
  
  if(event == kButton1Double){

    TCanvas *mc = dCanvas->GetCanvas();
    if(!mc) return;
    
    UInt_t pad = mc->GetSelectedPad()->GetNumber();
//     UInt_t ind = pad-1;
    plot = GraphArray.At(pad);
    legend  = (TLegend*)LegendArray.At(pad);

    if(plot){

      if(plot->InheritsFrom("TMultiGraph")){

	if(!dDataWindow){
	  dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					    "QwGUIDatabase",((TMultiGraph*)plot)->GetTitle(), PT_GRAPH_ER,
					    DDT_DB,600,400);
	  if(!dDataWindow){
	    return;
	  }
	  DataWindowArray.Add(dDataWindow);
	}
	else
	  add = kTrue;

	
	dDataWindow->SetPlotTitle((char*)((TMultiGraph*)plot)->GetTitle());
	//       if(!dCurrentModeData[GetActiveTab()]->IsSummary()){
	// 	dDataWindow->SetPlotTitleX("Time [sec]");
	// 	dDataWindow->SetPlotTitleY("Amplitude [V/#muA]");
	// 	dDataWindow->SetPlotTitleOffset(1.25,1.8);
	// 	dDataWindow->SetAxisMin(((TMultiGraph*)plot)->GetXaxis()->GetXmin(),
	// 				detStr->GetTreeLeafMin(leafInd));
	// 	dDataWindow->SetAxisMax(((TMultiGraph*)plot)->GetXaxis()->GetXmax(),
	// 				detStr->GetTreeLeafMax(leafInd));
	dDataWindow->SetLimitsFlag(kFalse);
	// 	dDataWindow->DrawData(*((TMultiGraph*)plot),add);
	//       }
	//       else{	
	//       }

	dDataWindow->DrawData(*((TMultiGraph*)plot),add,legend);

	SetLogMessage(Form("Looking at graph %s\n",(char*)((TMultiGraph*)plot)->GetTitle()),kTrue);
	
	Connect(dDataWindow,"IsClosing(char*)","QwGUIDatabase",(void*)this,"OnObjClose(char*)");
	Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIDatabase",(void*)this,"OnReceiveMessage(char*)");
	Connect(dDataWindow,"UpdatePlot(char*)","QwGUIDatabase",(void*)this,"OnUpdatePlot(char *)");
	
	return;
      }
    }
  }
}

/******************************************

Create a mysql query with the specific SELECT options

*******************************************/
TString QwGUIDatabase::MakeQuery(TString outputs, TString tables_used, TString table_links, 
				 TString special_cuts){

  TString slope_calculation_flag;
  TString regression_selected;
  TString target_position;

  /*Basic data selections that are valid for any type of query*/

  /*Get run quality cut information*/
  Bool_t quality[3] = {kFALSE, kFALSE, kFALSE};
  if(dChkQualityGood->IsOn())	      quality[0] 	= kTRUE;
  if(dChkQualityBad->IsOn())	      quality[1]	= kTRUE;
  if(dChkQualitySuspect->IsOn())      quality[2] 	= kTRUE;

  TString quality_check = " AND data.run_quality_id is not NULL";
  for (size_t i=0; i<3;i++){
	if(!quality[i]){
		quality_check = quality_check + Form(" AND !FIND_IN_SET('%i',data.run_quality_id)",i+1);
	}
  }

  /*Get good_for cut information*/
  Bool_t good_for[N_GOODFOR_TYPES];
  std::vector<Int_t> good_for_id;

  TList *selected_types = new TList;
  dBoxGoodFor->GetSelectedEntries(selected_types);
  TGTextLBEntry *entry;
  TIter next(selected_types);
  TString good_for_check = " AND data.good_for_id is not NULL ";

  for(size_t i=0; i<N_GOODFOR_TYPES;i++){ //initialize them as false
	  good_for[i] = kFALSE;
  }

  // validate the entries that are selected.
  while( (entry = (TGTextLBEntry *)next()) ) {
	  good_for[dBoxGoodFor->FindEntry(entry->GetTitle())->EntryId()-1] = kTRUE;
  }

  for (size_t i=0; i<N_GOODFOR_TYPES;i++){
    if(good_for[i]){
      good_for_check = good_for_check + Form(" AND FIND_IN_SET('%i',data.good_for_id)",i+1);
    }
  }
  
  //
  // Selecting regressed data and sensitivities
  //
  if (det_id==ID_MD_SENS || det_id==ID_LUMI_SENS){
    /*To get Sensitivities :
      The calculated sensitivities are stored in the data base with the option 'slope_calculation'='regression_type'.
      e.g.slope_calculation'='on_5+1'.
      To access the sensitivities we need to use the selection
      slope_calculation'='regression_type' and slope_correction='off'
      beacuse the regression script is not applying corrections at the same time slopes are being calculated.
    */
    slope_calculation_flag  = regression_set;
    regression_selected = "off";
  }
  else if (RegressionSchemes[dCmbRegressionType->GetSelected()]=="off"){
    /* To get the unregressed data (data that passed regression cuts in LinRegBlue) :
       When slope correction is 'off' all the schemes will have the same unregressed
       values. So I can just pick one scheme for the slope_correction option
    */
    slope_calculation_flag  = "on";
    regression_selected = regression_set;
  }
  else if (RegressionSchemes[dCmbRegressionType->GetSelected()]=="raw(from rootfiles)"){
    /* To get the raw data from QwAnalysis (with standard analyzer cuts) :
     */
    slope_calculation_flag  = "off";
    regression_selected = "off";
  }
  else{
    /* To get regressed data :
       slope_calculation='off' and slope_correction= regression_set
    */
    slope_calculation_flag  = "off";
    regression_selected = regression_set;
  }
  
  //
  // Selecting a target type
  //
  if(target=="All")
    target_position = ""; // No selection. Just show all target types
  else
    target_position = Form("AND target_position = '%s' ",target.Data());
  
  TString querystring 
    = "SELECT "+outputs+" slow_helicity_plate, data.run_quality_id,data.good_for_id,target_position, (scd.value<0)*-2+1 as wien_reversal"
    +" FROM "+tables_used+" analysis_view as ana, slow_controls_settings , slow_controls_data as scd   "
    +" WHERE "+table_links+" data.analysis_id = ana.analysis_id AND ana.runlet_id = slow_controls_settings.runlet_id "
    + Form(" AND data.slope_correction='%s' AND data.slope_calculation='%s' ",regression_selected.Data(),slope_calculation_flag.Data())
    +" AND data.subblock = "+ Form("%i",subblock) 
    +" AND data.error !=0 "
    +target_position+
    +" AND scd.runlet_id=ana.runlet_id "
    + quality_check
    + good_for_check
    + special_cuts
    + " ORDER BY data.run_number, data.segment_number;";


  return querystring;
}




/******************************************

Function to query Y vs X data

*******************************************/
mysqlpp::StoreQueryResult  QwGUIDatabase::QueryDetector()
{
  Bool_t ldebug = kTRUE;
  //TString reg_iv;

  dDatabaseCont->Connect();

  mysqlpp::Query query1     = dDatabaseCont-> Query();
  mysqlpp::Query query2     = dDatabaseCont-> Query();

  // The measurement_type combo box gets disables only when the user is requesting for sensitivities.
  // So use that to know when sensitivities are requested
  if(!(dCmbMeasurementType->IsEnabled())) measurement_type = "sensitivity";

  if(det_id==ID_MD_SENS || det_id==ID_LUMI_SENS){
	  device			= Form("%s",detector.Data());
	  reg_iv			= Form("%s",property.Data());
  }
  else{
    device            = Form("%s%s",detector.Data(),property.Data());
  }
  

  TString det_table;
  TString data_table;
  TString det_type;
  Int_t   phi_fg_id = 0;
  
  // Set the correct tables to read the data from.
  switch (det_id){
  case ID_MD:
    det_table    = "main_detector";
    data_table   = "md_data_view";
    det_type     = "detector";
    break;
  case ID_LUMI:
    det_table    = "lumi_detector";
    data_table   = "lumi_data_view";
    det_type     = "detector";
    break;
  case ID_BPM:
  case ID_CMB_BPM:
  case ID_BCM:
  case ID_BCM_DD:
  case ID_CMB_BCM:
  case ID_E_CAL:
    det_table    = "monitor";
    data_table   = "beam_view";
    det_type     = "monitor";
    break;
  case ID_MD_SENS:
	det_table	   = "main_detector";
	data_table	   = "md_slope_view";
	det_type           = "detector";
	break;
  case ID_LUMI_SENS:
	det_table	   = "lumi_detector";
	data_table	   = "lumi_slope_view";
	det_type           = "detector";
	break;
  default:
    break;
  }

  /*Get the id of the solenoid angle from the sc_detectors table*/
  if(ldebug) std::cout<< "SELECT * FROM sc_detector "
		      <<" WHERE name = 'Phi_FG';"<<std::endl;
  query1 << "SELECT * FROM sc_detector WHERE name = 'Phi_FG';";
  mysqlpp::StoreQueryResult sc_detector_id = query1.store();
  
  //check for empty queries. If empty exit with error.
  if( sc_detector_id.num_rows() == 0){
    std::cout<<"There is no device called Phi_FG in sc_detector in the given run range!"<<std::endl;
    return sc_detector_id;
  }
  phi_fg_id = sc_detector_id[0]["sc_detector_id"]; 
 

 /* Now get the run number information*/
  mysqlpp::Query query      = dDatabaseCont->Query();

  TString querystring="";
  TString outputs="";
 
  TString special_cuts =" AND data."+det_type+"='"+device+"'  AND scd.sc_detector_id = "
    + Form("%i", phi_fg_id)+" ";

  TString table_links = "";
  TString tables_used = data_table+" as data,";

  /* measurements vs sensitivities */
  if(det_id==ID_MD_SENS || det_id==ID_LUMI_SENS){
    special_cuts+=" AND slope = '"+reg_iv+"' ";
  }
  else{
    special_cuts+=" AND measurement_type = '"+measurement_type+"' ";
  }


  /*Runs vs Slugs vs Histograms*/
 
  if(((dCmbXAxis->GetSelected()) == ID_X_RUN) || ((dCmbXAxis->GetSelected()) == ID_X_HISTO)|| ((dCmbXAxis->GetSelected()) == ID_X_PULL_PLOT)) {
    outputs   = "data.value AS value, data.error AS error, data.error*sqrt(data.n) AS rms, (data.run_number+data.segment_number*0.01) AS x_value,";
    special_cuts += Form(" AND data.run_number >= %i  AND data.run_number <= %i ",index_first, index_last);
  }

  if((dCmbXAxis->GetSelected()) == ID_X_SLUG){
    outputs   = "sum(distinct(data.value/(POWER(data.error,2))))/sum( distinct(1/(POWER(data.error,2)))) AS value, SQRT(1/SUM(distinct(1/(POWER(data.error,2))))) AS error, SQRT(SUM(data.n))*SQRT(1/SUM(distinct(1/(POWER(data.error,2))))) AS rms,data.slug AS x_value, ";
    special_cuts += Form(" AND (data.slug >= %i AND data.slug <= %i ) GROUP BY data.slug",index_first,index_last);
  }

  if((dCmbXAxis->GetSelected()) == ID_X_TIME) {
    tables_used+=" run,  ";
    outputs   = "sum(distinct(data.value/(POWER(data.error,2))))/sum( distinct(1/(POWER(data.error,2)))) AS value, SQRT(1/SUM(distinct(1/(POWER(data.error,2))))) AS error, SQRT(SUM(data.n))*SQRT(1/SUM(distinct(1/(POWER(data.error,2))))) AS rms,run.start_time AS x_value, ";
    special_cuts += Form(" AND run.run_number = data.run_number AND (data.run_number >= %i  AND data.run_number <= %i) and run.start_time!='NULL' and run.start_time>'2009-01-01' GROUP BY data.run_number ",index_first, index_last);
  }
  if((dCmbXAxis->GetSelected()) == ID_X_TIME_RUNLET) {
    tables_used+=" runlet,  ";
    outputs   = "data.value AS value, data.error AS error, data.error*sqrt(data.n) AS rms,runlet.start_time AS x_value,";
    special_cuts += Form(" AND runlet.runlet_id = data.runlet_id AND (data.run_number >= %i  AND data.run_number <= %i) AND runlet.start_time!='NULL' ",index_first, index_last);
  }


  querystring= MakeQuery(outputs,tables_used,table_links,special_cuts);

  if(ldebug) std::cout<<"QUERYSTRING="
		      <<querystring<<std::endl;

  query << querystring;
  
  
  mysqlpp::StoreQueryResult read_data = query.store();
  dDatabaseCont->Disconnect(); 


  return read_data;

}


/******************************************

Plot detector data vs run number/slug/time

*******************************************/

void QwGUIDatabase::PlotDetector()
{

  Bool_t ldebug = kFALSE;

  // histo parameters
  gDirectory->Delete();
  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.07);
  gStyle->SetTitleOffset(2.2);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetHistMinimumZero();
  gStyle->SetBarOffset(0.25);
  gStyle->SetBarWidth(0.5);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleColor(kBlack,"X");
  gStyle->SetTitleColor(kBlack,"Y");


  if(dDatabaseCont){

    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);
    mc->SetHighLightColor(0);

    if(!(dCmbMeasurementType->IsEnabled())) measurement_type = "sensitivity";
    //
    // Query Database for Data
    //
    mysqlpp::StoreQueryResult read_data;
    Int_t row_size;
    read_data = QueryDetector();
    row_size =  read_data.num_rows();
    if(ldebug) std::cout<<" row_size="<<row_size<<"\n";
 
    //check for empty queries. If empty exit with error.
    if(row_size == 0){
      
      std::cout  <<"There is no data for "<<measurement_type<<" of "<<device<<" in the given range!"<<std::endl;
      return;
    }

    vector <string> xval;
    Double_t val;
    
    //vectors to fill IHWP IN & Wien right values
    vector <string> run_in_label;
    vector <string> run_out_label;
    TVectorD x_in(row_size), xerr_in(row_size);
    TVectorD x_out(row_size), xerr_out(row_size);
    TVectorD run_in(row_size), run_out(row_size);
    TVectorD err_in(row_size), err_out(row_size);
    
    //vectors to fill IHWP IN & Wien left values
    TVectorD x_in_L(row_size), xerr_in_L(row_size);
    TVectorD x_out_L(row_size), xerr_out_L(row_size);
    TVectorD run_in_L(row_size), run_out_L(row_size);
    TVectorD err_in_L(row_size), err_out_L(row_size);
    vector <string> run_in_L_label; 
    vector <string> run_out_L_label;

    //vectors to fill bad values
    TVectorD x_bad(row_size), xerr_bad(row_size);
    TVectorD run_bad(row_size);
    TVectorD err_bad(row_size);
    vector <string> run_bad_label;
   
    //vectors to fill suspect values
    TVectorD x_suspect(row_size), xerr_suspect(row_size);
    TVectorD run_suspect(row_size);
    TVectorD err_suspect(row_size);
    vector <string> run_suspect_label;

    TVectorD x_rms_all(row_size), x_rmserr_all(row_size), run_rms(row_size);

    xval.clear();
    x_in.Clear();
    x_in.ResizeTo(row_size);
    xerr_in.Clear();
    xerr_in.ResizeTo(row_size);
    run_in.Clear();
    run_in_label.clear();
    run_in.ResizeTo(row_size);
    err_in.Clear();
    err_in.ResizeTo(row_size);

    x_out.Clear();
    x_out.ResizeTo(row_size);
    xerr_out.Clear();
    xerr_out.ResizeTo(row_size);
    run_out.Clear();
    run_out_label.clear();
    run_out.ResizeTo(row_size);
    err_out.Clear();
    err_out.ResizeTo(row_size);

    x_in_L.Clear();
    x_in_L.ResizeTo(row_size);
    xerr_in_L.Clear();
    xerr_in_L.ResizeTo(row_size);
    run_in_L.Clear();
    run_in_L_label.clear();
    run_in_L.ResizeTo(row_size);
    err_in_L.Clear();
    err_in_L.ResizeTo(row_size);

    x_out_L.Clear();
    x_out_L.ResizeTo(row_size);
    xerr_out_L.Clear();
    xerr_out_L.ResizeTo(row_size);
    run_out_L.Clear();
    run_out_L_label.clear();
    run_out_L.ResizeTo(row_size);
    err_out_L.Clear();
    err_out_L.ResizeTo(row_size);

    x_bad.Clear();
    x_bad.ResizeTo(row_size);
    xerr_bad.Clear();
    xerr_bad.ResizeTo(row_size);
    run_bad.Clear();
    run_bad_label.clear();
    run_bad.ResizeTo(row_size);
    err_bad.Clear();
    err_bad.ResizeTo(row_size);

    x_suspect.Clear();
    x_suspect.ResizeTo(row_size);
    xerr_suspect.Clear();
    xerr_suspect.ResizeTo(row_size);
    run_suspect.Clear();
    run_suspect_label.clear();
    run_suspect.ResizeTo(row_size);
    err_suspect.Clear();
    err_suspect.ResizeTo(row_size);
    
    run_rms.Clear();
    run_rms.ResizeTo(row_size);

    Float_t x = 0.0 , xerr = 0.0, x_rms = 0.0;
    Int_t m = 0;
    Int_t k = 0;
    Int_t l = 0;
    Int_t n = 0;
    Int_t o = 0; //bad quality
    Int_t p = 0; //suspect quality

    TGraphErrors* grp_in = NULL;
    TGraphErrors* grp_in_L = NULL;
    TGraphErrors* grp_out = NULL;
    TGraphErrors* grp_out_L = NULL;
    TGraphErrors* grp_bad = NULL;
    TGraphErrors* grp_suspect = NULL;
    TGraphErrors* grp_rms = NULL;
    TMultiGraph * grp = NULL;

    TF1* fit1 = NULL;
    TF1* fit2 = NULL;
    TF1* fit3 = NULL;
    TF1* fit4 = NULL;
    TF1* fit5 = NULL;    
    TF1* fit6 = NULL;

    TDatime *runtime_start =  NULL;
    Double_t converted     = 0;
    TDatime* DST2010_start =  NULL;
    TDatime* DST2010_end   =  NULL;
    TDatime* DST2011_start =  NULL;
    TDatime* DST2011_end   =  NULL;
    TDatime* DST2012_start =  NULL;
    TDatime* DST2012_end   =  NULL;
    Double_t adjust_for_DST = 0; //1 hour

    std::cout<<"############################################\n";
    std::cout<<"QwGUI : Collecting data.."<<std::endl;
    std::cout<<"QwGUI : Retrieved "<<row_size<<" data points\n";
    for (Int_t i = 0; i < row_size; ++i)
      { 	   

	if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET ){
	  /*How to address DST issue in TAxis? 
	  Aparently TAxis time dispaly have a bug (which was not fixed in V 5.27.02) in it which means 
	  that when it converts from UInt to time display it is not correcting for DST.
	  To avoid this, I am hard coding the DST start and end times from 2010, 2011 and 2012 
	  (Qweak run periods) and add or subtract an hour from the time that goes in to the graph. 

	  refference for DST times: http://www.timeanddate.com/worldclock/timezone.html?n=862
	  2010	Sunday, March 14 at 2:00 AM	Sunday, November 7 at 2:00 AM
	  2011	Sunday, March 13 at 2:00 AM	Sunday, November 6 at 2:00 AM
	  2012	Sunday, March 11 at 2:00 AM	Sunday, November 4 at 2:00 AM
	  */

	  DST2010_start =  new TDatime(2010,03,14,02,00,00);
	  DST2010_end   =  new TDatime(2010,11,07,02,00,00);
	  DST2011_start =  new TDatime(2011,03,13,02,00,00);
	  DST2011_end   =  new TDatime(2011,11,06,02,00,00);
	  DST2012_start =  new TDatime(2012,03,11,02,00,00);
	  DST2012_end   =  new TDatime(2012,11,04,02,00,00);

	
	  runtime_start = new TDatime((read_data[i]["x_value"]).c_str());
	  converted = runtime_start->Convert();

	  //If the time is in the DST time zone 
	  if( (converted >= (DST2010_start->Convert()) and (converted <= (DST2010_end->Convert())))or
	      (converted >= (DST2011_start->Convert()) and (converted <= (DST2011_end->Convert())))or
	      (converted >= (DST2012_start->Convert()) and (converted <= (DST2012_end->Convert()))))
	    adjust_for_DST = 3600;	   
	  else
	    adjust_for_DST =0.0;

	  if(ldebug){
	    std::cout<<"run start time = "<<runtime_start->AsString()<<std::endl;
	    std::cout<<"converted UINT = "<<runtime_start->Convert()<<std::endl;
	    std::cout<<""<<adjust_for_DST<<std::endl;
	  }
	}

	// Convert asymmetries  in to ppm, convert differences in to nm/murad
	if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
	  x    = (read_data[i]["value"])*1e6; // convert to  ppm/ nm
	  xerr = (read_data[i]["error"])*1e6; // convert to ppm/ nm
	  x_rms = (read_data[i]["rms"])*1e6; // convert to  ppm/ nm
	} 
	else{
	  x    = read_data[i]["value"];
	  xerr = read_data[i]["error"];
	  x_rms = (read_data[i]["rms"]);
	}	  
        
        //Set the lables for rms plots
	if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
	  run_rms.operator()(i)  = (runtime_start->Convert()) + adjust_for_DST;
        }
        else if(x_axis == ID_X_RUN){
	  run_rms.operator()(i)  = i;
	  val = read_data[i]["x_value"];
	  xval.push_back(Form("%5.2f",val ));
	}
	else{
	  run_rms.operator()(i)  = read_data[i]["x_value"];
	  //do nothing for WEIN/SLUG. Don't need labels for these.
	}
	
	// Fill the rms vectors
	x_rms_all.operator()(i) = x_rms;
        x_rmserr_all.operator()(i) = 0.0;
 
	// Now fill the mean values
        if(read_data[i]["run_quality_id"] == "1"){
	  if(read_data[i]["slow_helicity_plate"] == "out") {
	    if (read_data[i]["wien_reversal"]*1 == 1){

	      if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
		run_out.operator()(k)  = (runtime_start->Convert()) + adjust_for_DST;
		run_out_label.push_back(Form("%f",(runtime_start->Convert()) + adjust_for_DST));
	      }
	      else if(x_axis == ID_X_RUN){
		run_out.operator()(k)  = i;
		run_out_label.push_back((char*)(xval[i].c_str()));

	      }
	      else{
		val = read_data[i]["x_value"];
		run_out.operator()(k) =  read_data[i]["x_value"];
		run_out_label.push_back(Form("%f",val));
	      }
	      x_out.operator()(k)    = x;
	      xerr_out.operator()(k) = xerr;
	      err_out.operator()(k)  = 0.0;
	      k++;

	    } else {
	      if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
		run_out_L.operator()(l)  = (runtime_start->Convert())+ adjust_for_DST;
		run_out_L_label.push_back(Form("%f",(runtime_start->Convert()) + adjust_for_DST));
	      }
	      else if (x_axis == ID_X_RUN){
		run_out_L.operator()(l)  = i;
		run_out_L_label.push_back((char*)(xval[i].c_str()));
	      }
	      else{
		val = read_data[i]["x_value"];
		run_out_L.operator()(l)  = read_data[i]["x_value"];
		run_out_L_label.push_back(Form("%f",val));
	      }
	      x_out_L.operator()(l)    = x;
	      xerr_out_L.operator()(l) = xerr;
	      err_out_L.operator()(l)  = 0.0;
	      l++;
	    }
	    
	  }
	  
	  if(read_data[i]["slow_helicity_plate"] == "in") {

	    if (read_data[i]["wien_reversal"]*1 == 1){
	      if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
		run_in.operator()(m)  = runtime_start->Convert()+ adjust_for_DST;
		run_in_label.push_back(Form("%f",runtime_start->Convert()+ adjust_for_DST));		
	      }
	      else if(x_axis == ID_X_RUN){
		run_in.operator()(m)  = i;
		run_in_label.push_back((char*)(xval[i].c_str()));
	      }
	      else{
		val = read_data[i]["x_value"];
		run_in.operator()(m)  = read_data[i]["x_value"];
		run_in_label.push_back(Form("%f",val));
	      }
	      x_in.operator()(m)    = x;
	      xerr_in.operator()(m) = xerr;
	      err_in.operator()(m)  = 0.0;
	      m++;
	    } else {
	      if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
		run_in_L.operator()(n)  = runtime_start->Convert()+ adjust_for_DST;
		run_in_L_label.push_back(Form("%f",runtime_start->Convert()+ adjust_for_DST));
	      }
	      else if(x_axis == ID_X_RUN){
		run_in_L.operator()(n)  = i;
		run_in_L_label.push_back((char*)(xval[i].c_str()));
	      }
	      else{
		val = read_data[i]["x_value"];
		run_in_L.operator()(n)  = read_data[i]["x_value"];
		run_in_L_label.push_back(Form("%f",val));
	      }
	      x_in_L.operator()(n)    = x;
	      xerr_in_L.operator()(n) = xerr;
	      err_in_L.operator()(n)  = 0.0;
	      n++;
	      
	    }
	  }
	}
	
	if((read_data[i]["run_quality_id"] == "2")   | //or
	   (read_data[i]["run_quality_id"] == "1,2") | //or
	   (read_data[i]["run_quality_id"] == "2,3") ) { //all instances of bad

	  if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
	    run_bad.operator()(o)  = runtime_start->Convert()+ adjust_for_DST;
	    run_bad_label.push_back(Form("%f",runtime_start->Convert()+ adjust_for_DST));
	  }
	  else if(x_axis == ID_X_RUN){
	    run_bad.operator()(o)  = i;
	    run_bad_label.push_back((char*)(xval[i].c_str()));
	  }
	  else{
	    val = read_data[i]["x_value"];
	    run_bad.operator()(o)  = read_data[i]["x_value"];
	    run_bad_label.push_back(Form("%f",val));
	  }
	  x_bad.operator()(o)    = x;
	  xerr_bad.operator()(o) = xerr;
	  err_bad.operator()(o)  = 0.0;
	  o++;
	}
	
	if((read_data[i]["run_quality_id"] == "3")  | //or
	   (read_data[i]["run_quality_id"] == "1,3")) {// suspect (but not bad)
	  if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
	    run_suspect.operator()(p)  = runtime_start->Convert()+ adjust_for_DST;
	    run_suspect_label.push_back(Form("%f",runtime_start->Convert()+ adjust_for_DST));
	  }
	  else  if(x_axis == ID_X_RUN){
	    run_suspect.operator()(p)  = i;
	    run_suspect_label.push_back((char*)(xval[i].c_str()));
	  }
	  else{
	    val = read_data[i]["x_value"];
	    run_suspect.operator()(p)  = read_data[i]["x_value"];
	    run_suspect_label.push_back(Form("%f",val));
	  }
	  x_suspect.operator()(p)    = x;
	  xerr_suspect.operator()(p) = xerr;
	  err_suspect.operator()(p)  = 0.0;
	  p++;
	}
	
      }
    
    // Check to make sure graph is not empty.
    if(m==0 && k==0 && l==0 && n==0 && o==0 && p==0){
      std::cout<<"QwGUI : No data were found for the given criteria."<<std::endl;
      exit(1);
    }
    else
      std::cout<<"QwGUI : Moving on to draw the graph"<<std::endl;
    
//GROUP_IN
    if(m>0){
      run_in.ResizeTo(m);
      x_in.ResizeTo(m);
      err_in.ResizeTo(m);
      xerr_in.ResizeTo(m);
      
      grp_in  = new TGraphErrors(run_in,  x_in, err_in, xerr_in);
      grp_in ->SetName("IHWP-IN-R"); 
      grp_in ->SetMarkerSize(0.6);
      grp_in ->SetMarkerStyle(21);
      grp_in ->SetMarkerColor(kBlue);
      grp_in ->Fit("pol0");
      fit1 = grp_in->GetFunction("pol0");
      fit1 -> SetLineColor(kBlue);
    }

//GROUP_IN_L
    if(n>0){
      run_in_L.ResizeTo(n);
      x_in_L.ResizeTo(n);
      err_in_L.ResizeTo(n);
      xerr_in_L.ResizeTo(n);
      
      grp_in_L  = new TGraphErrors(run_in_L,  x_in_L, err_in_L, xerr_in_L);
      grp_in_L ->SetName("IHWP-IN-L"); 
      grp_in_L ->SetMarkerSize(0.6);
      grp_in_L ->SetMarkerStyle(21);
      grp_in_L ->SetMarkerColor(kOrange-2);
      grp_in_L ->Fit("pol0");
      fit2 = grp_in_L->GetFunction("pol0");
      fit2 -> SetLineColor(kOrange-2);
    }

//GROUP_OUT
    if(k>0){
      run_out.ResizeTo(k);
      x_out.ResizeTo(k);
      err_out.ResizeTo(k);
      xerr_out.ResizeTo(k);
      
      grp_out = new TGraphErrors(run_out, x_out, err_out, xerr_out);
      grp_out ->SetName("IHWP-OUT-R");
      grp_out ->SetMarkerSize(0.6);
      grp_out ->SetMarkerStyle(21);
      grp_out ->SetMarkerColor(kRed);
      grp_out ->Fit("pol0");
      fit3 = grp_out->GetFunction("pol0");
      fit3 -> SetLineColor(kRed);
    }

//GROUP_OUT_L
    if(l>0){
      run_out_L.ResizeTo(l);
      x_out_L.ResizeTo(l);
      err_out_L.ResizeTo(l);
      xerr_out_L.ResizeTo(l);
      
      grp_out_L = new TGraphErrors(run_out_L, x_out_L, err_out_L, xerr_out_L);
      grp_out_L ->SetName("IHWP-OUT-L");
      grp_out_L ->SetMarkerSize(0.6);
      grp_out_L ->SetMarkerStyle(21);
      grp_out_L ->SetMarkerColor(kViolet);
      grp_out_L ->Fit("pol0");
      fit4 = grp_out_L->GetFunction("pol0");
      fit4 -> SetLineColor(kViolet);
    }

//GROUP_BAD
    if(o>0){
      run_bad.ResizeTo(o);
      x_bad.ResizeTo(o);
      err_bad.ResizeTo(o);
      xerr_bad.ResizeTo(o);
      
      grp_bad = new TGraphErrors(run_bad, x_bad, err_bad, xerr_bad);
      grp_bad ->SetName("Bad");       
      grp_bad ->SetMarkerSize(1.2);
      grp_bad ->SetMarkerStyle(29);
      grp_bad ->SetMarkerColor(kBlack);
      grp_bad ->Fit("pol0");
      fit5 = grp_bad->GetFunction("pol0");
      fit5 -> SetLineColor(kBlack);
    }

//GROUP_SUSPECT
    if(p>0){
      run_suspect.ResizeTo(p);
      x_suspect.ResizeTo(p);
      err_suspect.ResizeTo(p);
      xerr_suspect.ResizeTo(p);
      
      grp_suspect = new TGraphErrors(run_suspect, x_suspect, err_suspect, xerr_suspect);
      grp_suspect ->SetName("Suspect");   
      grp_suspect ->SetMarkerSize(1.2);
      grp_suspect ->SetMarkerStyle(29);
      grp_suspect ->SetMarkerColor(kGreen);
      grp_suspect ->Fit("pol0");
      fit6 = grp_suspect->GetFunction("pol0");
      fit6 -> SetLineColor(kGreen);
    }
    
    // Get titles for X, Y and graphs
    TString y_title = GetYTitle(measurement_type, det_id);
    TString title   = GetTitle(measurement_type, device);
    TString x_title;
    if(x_axis == ID_X_TIME)x_title = "Run Start Time (YY:MM:DD / HH:MM)";
    else
      if(x_axis == ID_X_TIME_RUNLET)x_title = "Runlet Start Time (YY:MM:DD / HH:MM)";
      else							
	if(x_axis == ID_X_RUN)x_title = "Run Number";
	else
	  if(x_axis == ID_X_SLUG)x_title = "Slug Number";
	  else x_title = "";



    //Make RMS graph with all events
    if(plot.Contains("RMS")) grp_rms = new TGraphErrors(run_rms, x_rms_all, x_rmserr_all, x_rmserr_all);

    // Multigraph for the mean values
    if(plot.Contains("Mean")) grp = new TMultiGraph("grp",title);

    if(plot.Contains("Both")){
      grp_rms = new TGraphErrors(run_rms, x_rms_all, x_rmserr_all, x_rmserr_all);
      grp = new TMultiGraph("grp",title);
    }
    
   
    if(grp){
      if(m>0)grp->Add(grp_in);
      if(k>0)grp->Add(grp_out);
      if(n>0)grp->Add(grp_in_L);
      if(l>0)grp->Add(grp_out_L);
      if(o>0)grp->Add(grp_bad);
      if(p>0)grp->Add(grp_suspect);
    }
   
    Double_t inR=0;  Double_t inRerr=0;
    Double_t inL=0;  Double_t inLerr=0;
    Double_t outR=0; Double_t outRerr=0;
    Double_t outL=0; Double_t outLerr=0;

    TLegend *legend = new TLegend(0.55,0.80,0.99,0.99,"","brNDC");
    if(m>0){
    	inR = fit1->GetParameter(0);
    	inRerr = fit1->GetParError(0);
    	legend->AddEntry(grp_in, Form("<IN_R>  = %2.5f #pm %2.5f (stat)", inR, inRerr), "p");
    }
    if(n>0){
    	inL = fit2->GetParameter(0);
    	inLerr = fit2->GetParError(0);
    	legend->AddEntry(grp_in_L, Form("<IN_L>  = %2.5f #pm %2.5f (stat)", inL, inLerr), "p");
    }
    if(k>0){
    	outR = fit3->GetParameter(0);
    	outRerr = fit3->GetParError(0);
    	legend->AddEntry(grp_out,Form("<OUT_R> = %2.5f #pm %2.5f (stat)", outR, outRerr), "p");
    }
    if(l>0){
    	outL = fit4->GetParameter(0);
    	outLerr = fit4->GetParError(0);
    	legend->AddEntry(grp_out_L,Form("<OUT_L> = %2.5f #pm %2.5f (stat)", outL, outLerr), "p");
    }
    if(o>0){
    	legend->AddEntry(grp_bad,Form("<BAD> = %2.5f #pm %2.5f (stat)",
					  fit5->GetParameter(0), fit5->GetParError(0)), "p");
    }
    if(p>0){
    	legend->AddEntry(grp_suspect,Form("<SUSPECT> = %2.5f #pm %2.5f (stat)",
					      fit6->GetParameter(0), fit6->GetParError(0)), "p");
    }


    //PRINT OUT INFORMATION TO CONSOLE
    if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
		std::cout<<" "<<std::endl;
		std::cout<<"############################################"<<std::endl;
		std::cout<<" "<<std::endl;
		std::cout<<"Fit Results for "<<title<<std::endl;

		if(m>0 && k>0){
			std::cout<<"in_R = "<<inR<<"  +/- "<<inRerr<<" ppm"<<std::endl;
			std::cout<<"out_R = "<<outR<<" +/- "<<outRerr<<" ppm"<<std::endl;
			std::cout<<"(IN+OUT)/2 = "<<(inR+outR)/2<<" +/- "<<sqrt(inRerr*inRerr+outRerr*outRerr)/2<<" ppm"<<std::endl;
			std::cout<<"(OUT-IN)   = "<<
					( outR/(outRerr*outRerr) - inR/(inRerr*inRerr) )/( 1/(outRerr*outRerr) + 1/(inRerr*inRerr))<<" +/- "<<
					1/sqrt(1/(outRerr*outRerr)+1/(inRerr*inRerr))<<" ppm"<<std::endl;
		}

		if(n>0 && l>0){
			std::cout<<"in_L = "<<inL<<"  +/- "<<inLerr<<" ppm"<<std::endl;
			std::cout<<"out_L = "<<outL<<" +/- "<<outLerr<<" ppm"<<std::endl;
			std::cout<<"(IN+OUT)/2 = "<<(inL+outL)/2<<" +/- "<<sqrt(inLerr*inLerr+outLerr*outLerr)/2<<" ppm"<<std::endl;
			std::cout<<"(IN-OUT)   = "<<
								( inL/(inLerr*inLerr) - outL/(outLerr*outLerr) )/( 1/(outLerr*outLerr) + 1/(inLerr*inLerr))<<" +/- "<<
								1/sqrt(1/(inLerr*inLerr)+1/(outLerr*outLerr))<<" ppm"<<std::endl;
		}
		std::cout<<" "<<std::endl;
		std::cout<<"############################################"<<std::endl;
		std::cout<<" "<<std::endl;
    }

    legend->SetFillColor(0);

    
    if(grp) AddNewGraph(grp,legend);
    if(grp_rms) AddNewGraph(grp_rms,NULL);
    PlotGraphs();
    char * label;

    // redraw rms graph
    if(grp_rms)grp_rms->Draw("ab");
    //by default PlotGraphs() draws graphs in "ap" format. 
    //So to get the bars, we have to redraw the rms graph.
 
   // format axis to display time
    if(x_axis == ID_X_TIME or x_axis == ID_X_TIME_RUNLET){
      if(grp){
	grp->GetXaxis()->SetTimeDisplay(1);
	grp->GetXaxis()->SetTimeOffset(0,"gmt");
	grp->GetXaxis()->SetLabelOffset(0.03); 
	grp->GetXaxis()->SetLabelSize(0.02); 
	grp->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}");
	grp->GetXaxis()->Draw();
      }
      if(grp_rms) {
 	grp_rms->GetXaxis()->SetTimeDisplay(1);
	grp_rms->GetXaxis()->SetTimeOffset(0,"gmt");
	grp_rms->GetXaxis()->SetLabelOffset(0.03); 
	grp_rms->GetXaxis()->SetLabelSize(0.02); 
	grp_rms->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}");
	grp_rms->GetXaxis()->Draw();
      }
    }


    if(x_axis == ID_X_RUN){
      // set the alphanumeric labels for runlets
      if(grp) grp->GetXaxis()->Set(row_size, 0, row_size-1); // rebin
      if(grp_rms) {
	grp_rms->GetXaxis()->Set(row_size, 0, row_size-1);
	gPad->SetBottomMargin(0.2);
	gPad->Update();
      }
      for (k=1;k<=row_size;k++) {
    	label = (char*)(xval[k-1].c_str());
    	if(grp) (grp->GetXaxis())->SetBinLabel(k,label);
	if(grp_rms) (grp_rms->GetXaxis())->SetBinLabel(k,label);
      }
    }
    else{
      // do nothing
      // For WEIN and SLUG we can just use the values we read.
    }

    // set titles for mean value graph
    if(grp){
      grp->SetTitle(title);
      grp->GetYaxis()->SetTitle(y_title);
      grp->GetXaxis()->SetTitle(x_title);
      grp->GetYaxis()->CenterTitle();
      gPad->Update();
    }
 
    // set titles for rms value graph
    if(grp_rms){
      grp_rms->SetTitle(title);
      grp_rms->GetYaxis()->SetTitle(y_title);
      grp_rms->GetXaxis()->SetTitle(x_title);
      grp_rms->GetYaxis()->CenterTitle();
      gPad->Update();
    }   

    if(plot.Contains("Both")){
      mc->cd(1);
      gPad->SetBottomMargin(0.2);
      gPad->Modified();
      gPad->Update();
    }

    mc->Modified();
    mc->SetBorderMode(0);
    mc->Update();
   
    std::cout<<"QwGUI : Done!"<<std::endl;
    
    for(uint w = 0; w < run_in_label.size(); w++){
      grp_in->GetXaxis()->SetBinLabel(w+1,(char*)(run_in_label[w].c_str()));
    }
    for(uint w = 0; w < run_in_L_label.size(); w++)
      grp_in_L->GetXaxis()->SetBinLabel(w+1,(char*)(run_in_L_label[w].c_str()));
    for(uint w = 0; w < run_out_label.size(); w++)
      grp_out->GetXaxis()->SetBinLabel(w+1,(char*)(run_out_label[w].c_str()));
    for(uint  w = 0; w < run_out_L_label.size(); w++){
      grp_out_L->GetXaxis()->SetBinLabel(w+1,(char*)(run_out_L_label[w].c_str()));
    }
    for(uint w = 0; w < run_bad_label.size(); w++)
      grp_bad->GetXaxis()->SetBinLabel(w+1,(char*)(run_bad_label[w].c_str()));
    for(uint w = 0; w < run_suspect_label.size(); w++)
      grp_suspect->GetXaxis()->SetBinLabel(w+1,(char*)(run_suspect_label[w].c_str()));

    
  }
  else{
    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);
   
    mc->cd();
    TLatex T1;
    T1.SetTextAlign(12);
    T1.SetTextSize(0.03);
    T1.DrawLatex(0.1,0.8,"To execute the command, you need first to connect to the database !");
    T1.DrawLatex(0.1,0.6,"To connect to the database:");
    T1.DrawLatex(0.2,0.5,"#bullet Use the pull-down menu at the top left of the GUI (File)");
    T1.DrawLatex(0.2,0.4,"#bullet Choose Open (Database)");      
    T1.DrawLatex(0.2,0.3,"#bullet You can then resubmit your request.");      
    
    mc->Modified();
    mc->SetBorderMode(0);
    mc->Update();

  }
  

}

/******************************************

Histogram detector data for the given range

*******************************************/

void QwGUIDatabase::HistogramDetector()
{

  Bool_t ldebug = kTRUE;

  // histo parameters
  gDirectory->Delete();
  gStyle->Reset();
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(1.0);
  gStyle->SetTitleX(0.05);
  gStyle->SetTitleW(0.5);
  gStyle->SetTitleSize(0.05);
  gStyle->SetTitleOffset(2.2);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetHistMinimumZero();
  gStyle->SetBarOffset(0.25);
  gStyle->SetBarWidth(0.5);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleColor(kBlack,"X");
  gStyle->SetTitleColor(kBlack,"Y");

  if(dDatabaseCont){

    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);

    if(!(dCmbMeasurementType->IsEnabled())) measurement_type = "sensitivity";
    //
    // Query Database for Data
    //
    mysqlpp::StoreQueryResult read_data;
    Int_t row_size;
    read_data = QueryDetector();
    row_size =  read_data.num_rows();
    if(ldebug) std::cout<<" row_size="<<row_size<<"\n";
    
    //check for empty queries. If empty exit with error.
    if(row_size == 0){
      
      std::cout  <<"There is no data for "<<measurement_type<<" of "<<device<<" in the given range!"<<std::endl;
      return;
    }

    // Create histograms
    //    TH1F all_runs("all_runs","all_runs",1000,0,0);
    Float_t x = 0.0, x_rms = 0.0;


    THStack *hs = new THStack("hs","");
    TH1F *h_in_L = new TH1F("h_in_L","",1000,0,0);
    TH1F *h_in_R = new TH1F("h_in_R","",1000,0,0);
    TH1F *h_out_L = new TH1F("h_out_L","",1000,0,0);
    TH1F *h_out_R = new TH1F("h_out_R","",1000,0,0);
    TH1F *h_suspect = new TH1F("h_suspect","",1000,0,0);
    TH1F *h_bad = new TH1F("h_bad","",1000,0,0);
    TH1F *h_rms = new TH1F("h_rms","",1000,0,0);


    TF1* fit1 = NULL;
    TF1* fit2 = NULL;
    TF1* fit3 = NULL;
    TF1* fit4 = NULL;
    TF1* fit5 = NULL;    
    TF1* fit6 = NULL;

    Int_t m = 0;
    Int_t k = 0;
    Int_t l = 0;
    Int_t n = 0;
    Int_t o = 0;
    Int_t p = 0;

    std::cout<<"############################################\n";
    std::cout<<"QwGUI : Collecting data.."<<std::endl;
    std::cout<<"QwGUI : Retrieved "<<row_size<<" data points\n";
    for (Int_t i = 0; i < row_size; ++i)
      {

	if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
	  x    = (read_data[i]["value"])*1e6; // convert to  ppm/ nm
	  x_rms = (read_data[i]["rms"])*1e6; // convert to  ppm/ nm
	} 
	else{
	  x    = read_data[i]["value"];
	  x_rms = (read_data[i]["rms"]);
	}	  


	// Fill the rms histo
	h_rms->Fill(x_rms);

	// Now fill the mean values
	if(read_data[i]["run_quality_id"] == "1"){
	  if(read_data[i]["slow_helicity_plate"] == "out") {
	    if (read_data[i]["wien_reversal"]*1 == 1){
	      h_out_R->Fill(x);
	      k++;
	    } else {
	      h_out_L->Fill(x);
 	      l++;
	    }
	    
	  }

	  if(read_data[i]["slow_helicity_plate"] == "in") {
	    if (read_data[i]["wien_reversal"]*1 == 1){
	      h_in_R->Fill(x);
 	      m++;
	    } else {
	      h_in_L->Fill(x);
	      n++;

	    }
	  }
	}
	
	if((read_data[i]["run_quality_id"] == "2")   | //or
	   (read_data[i]["run_quality_id"] == "1,2") | //or
	   (read_data[i]["run_quality_id"] == "2,3") ) { //all instances of bad
	  h_bad->Fill(x);
 	  o++;
	}

	if((read_data[i]["run_quality_id"] == "3") | //suspect or
	  (read_data[i]["run_quality_id"] == "1,3")) {// suspect (but not bad)
	  h_suspect->Fill(x);
 	  p++;
	}
	
      }

    // Check to make sure graph is not empty.
    if(m==0 && k==0 && l==0 && n==0 && o==0 && p==0){
      std::cout<<"QwGUI : No data were found for the given criteria."<<std::endl;
      exit(1);
    }
    else
      std::cout<<"QwGUI : Moving on to draw the histogram"<<std::endl;

    TString y_title = GetYTitle(measurement_type, det_id);
    TString title   = GetTitle(measurement_type, device);
    


    if(m>0){
      //GROUP_IN_R
      h_in_R ->SetMarkerSize(1.0);
      h_in_R ->SetMarkerStyle(21);
      h_in_R ->SetFillColor(kBlue);
      h_in_R ->SetMarkerColor(kBlue);
      h_in_R ->SetLineColor(kBlue);
      //h_in_R ->Fit("gaus");
      //fit1 = h_in_R->GetFunction("gaus");
      //fit1 -> SetLineColor(kBlue);
      hs->Add(h_in_R);
    }
    if(k>0){
      //GROUP_OUT_R
      h_out_R ->SetMarkerSize(1.0);
      h_out_R ->SetMarkerStyle(21);
      h_out_R ->SetFillColor(kRed);
      h_out_R ->SetMarkerColor(kRed);
      h_out_R ->SetMarkerColor(kRed);
      h_out_R ->SetLineColor(kRed);
      //h_out_R ->Fit("gaus");
      //fit2 = h_out_R->GetFunction("gaus");
      //fit2 -> SetLineColor(kRed);
      hs->Add(h_out_R);
    }
    if(n>0){
      //GROUP_IN_L
      h_in_L ->SetMarkerSize(1.0);
      h_in_L ->SetMarkerStyle(21);
      h_in_L ->SetFillColor(kOrange-2);
      h_in_L ->SetMarkerColor(kOrange-2);
      h_in_L ->SetLineColor(kOrange-2);
      //h_in_L ->Fit("gaus");
      //fit3 = h_in_L->GetFunction("gaus");
      // fit3 -> SetLineColor(kOrange-2);
      hs->Add(h_in_L);
   
    }
    if(l>0){
      //GROUP_OUT_L
      h_out_L ->SetMarkerSize(1.0);
      h_out_L ->SetMarkerStyle(21);
      h_out_L ->SetFillColor(kViolet);
      h_out_L ->SetMarkerColor(kViolet);
      h_out_L ->SetLineColor(kViolet);
      //h_out_L ->Fit("gaus");
      //fit4 = h_out_L->GetFunction("gaus");
      //fit4 -> SetLineColor(kViolet);
      hs->Add(h_out_L);
    }
    if(o>0){
      //GROUP_BAD
      h_bad ->SetMarkerSize(1.0);
      h_bad ->SetMarkerStyle(29);
      h_bad ->SetFillColor(kBlack);
      h_bad ->SetMarkerColor(kBlack);
      h_bad ->SetLineColor(kBlack);
      // h_bad ->Fit("gaus");
      //fit5 = h_bad->GetFunction("gaus");
      //fit5 -> SetLineColor(kBlack);
      hs->Add(h_bad);
    }
    if(p>0){
      //GROUP_SUSPECT
      h_suspect ->SetMarkerSize(1.0);
      h_suspect ->SetMarkerStyle(29);
      h_suspect ->SetFillColor(kGreen);
      h_suspect ->SetMarkerColor(kGreen);
      h_suspect ->SetLineColor(kGreen);
      // h_suspect ->Fit("gaus");
      //fit6 = h_suspect->GetFunction("gaus");
      //fit6 -> SetLineColor(kGreen);
      hs->Add(h_suspect);
    }


    TLegend *legend = new TLegend(0.70,0.70,0.99,0.99,"","brNDC");
    if(m>0) legend->AddEntry(h_in_R, Form("<IN_R> : mean = %2.4f #pm %2.4f ppm; rms = %2.4f ppm", 
					  h_in_R->GetMean(), h_in_R->GetMeanError(),h_in_R->GetRMS()), "p");
    if(k>0) legend->AddEntry(h_out_R,Form("<OUT_R> : mean = %2.4f #pm %2.4f ppm; rms = %2.4f ppm", 
					  h_out_R->GetMean(), h_out_R->GetMeanError(),h_out_R->GetRMS()), "p");
    if(n>0) legend->AddEntry(h_in_L, Form("<IN_L> : mean = %2.4f #pm %2.4f ppm; rms = %2.4f ppm",
					  h_in_L->GetMean(), h_in_L->GetMeanError(),h_in_L->GetRMS()), "p");
    if(l>0) legend->AddEntry(h_out_L,Form("<OUT_L> : mean = %2.4f #pm %2.4f ppm; rms = %2.4f ppm ", 
					  h_out_L->GetMean(), h_out_L->GetMeanError(),h_out_L->GetRMS()), "p");
    if(o>0) legend->AddEntry(h_bad,Form("<BAD> : mean = %2.4f #pm %2.4f ppm; rms = %2.4f ppm ",
					h_bad->GetMean(), h_bad->GetMeanError(),h_bad->GetRMS()), "p"); 
    if(p>0) legend->AddEntry(h_suspect,Form("<SUSPECT> : mean = %2.4f #pm %2.4f ppm; rms = %2.4f ppm ", 
					    h_suspect->GetMean(), h_suspect->GetMeanError(),h_suspect->GetRMS()), "p"); 


    legend->SetFillColor(0);

    mc->cd();
    if(plot.Contains("Mean")){
      hs->Draw();
      legend->Draw("");
      gPad->Update();
    }
    else if(plot.Contains("RMS")){
      h_rms->Draw();
      gPad->Update();
    } 
    else {
      mc->Clear();
      mc->Divide(1,2);
      mc->cd(1);
      hs->Draw();
      legend->Draw();
      gPad->Update();
      mc->cd(2);
      h_rms->Draw();
      gPad->Update();
      mc->Update();
    }


    if(plot.Contains("Mean") or plot.Contains("Both") ){  
      y_title = y_title; 
      hs->SetTitle(title);
      hs->GetYaxis()->SetTitleOffset(1.5);
      hs->GetXaxis()->SetTitleOffset(1.5);
      hs->GetYaxis()->SetTitle("Runlets");
      hs->GetXaxis()->SetTitle(y_title);
      hs->GetYaxis()->SetTitleSize(0.03);
      hs->GetXaxis()->SetTitleSize(0.03);
      hs->GetYaxis()->CenterTitle();
      mc->Modified();
      mc->Update();
    }
    if(plot.Contains("RMS") or plot.Contains("Both") ){ 
      y_title = "RMS's of "+y_title;  
      h_rms->SetTitle(title);
      h_rms->GetYaxis()->SetTitleOffset(1.5);
      h_rms->GetXaxis()->SetTitleOffset(1.5);
      h_rms->GetYaxis()->SetTitle("Runlets");
      h_rms->GetXaxis()->SetTitle(y_title);
      h_rms->GetYaxis()->SetTitleSize(0.03);
      h_rms->GetXaxis()->SetTitleSize(0.03);
      h_rms->GetYaxis()->CenterTitle();
      mc->Modified();
      mc->Update();
    }
 
    std::cout<<"QwGUI : Histogramming Complete!"<<std::endl;
    
    
  }
  else{
    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);
    //
    mc->cd();
    TLatex T1;
    T1.SetTextAlign(12);
    T1.SetTextSize(0.03);
    T1.DrawLatex(0.1,0.8,"To execute the command, you need first to connect to the database !");
    T1.DrawLatex(0.1,0.6,"To connect to the database:");
    T1.DrawLatex(0.2,0.5,"#bullet Use the pull-down menu at the top left of the GUI (File)");
    T1.DrawLatex(0.2,0.4,"#bullet Choose Open (Database)");      
    T1.DrawLatex(0.2,0.3,"#bullet You can then resubmit your request.");      
    
    mc->Modified();
    mc->SetBorderMode(0);
    mc->Update();
    
  }
  
  
}

/******************************************

Create Pull plots

*******************************************/
void QwGUIDatabase::CreatePullPlot()
{

  Bool_t ldebug = kTRUE;
  
  TCanvas *mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->SetFillColor(0);

  if(dDatabaseCont){

    if(plot.Contains("Mean")){

      
      // First draw the histograms and get the mean.
      HistogramDetector();

      TString title   = GetTitle(measurement_type, device);

      THStack *hs = new THStack("hs",title);
      // setting histo limist to 0,0 auto adjusts the histogram range
      TH1F *h_in_L_pull = new TH1F("h_in_L_pull","",100,0,0);
      TH1F *h_in_R_pull = new TH1F("h_in_R_pull","",100,0,0);
      TH1F *h_out_L_pull = new TH1F("h_out_L_pull","",100,0,0);
      TH1F *h_out_R_pull = new TH1F("h_out_R_pull","",100,0,0);
      TH1F *h_suspect_pull = new TH1F("h_suspect_pull","",100,0,0);
      TH1F *h_bad_pull = new TH1F("h_bad_pull","",100,0,0);
      
      TH1F *h_in_L=NULL;
      TH1F *h_in_R = NULL;
      TH1F *h_out_L = NULL;
      TH1F *h_out_R = NULL;
      TH1F *h_suspect = NULL;
      TH1F *h_bad = NULL;
      
      Double_t mean[6]={0,0,0,0,0,0};
      
      Int_t m = 0;
      Int_t k = 0;
      Int_t l = 0;
      Int_t n = 0;
      Int_t o = 0;
      Int_t p = 0;
      
      // Get the mean value for the distributions
      h_in_L=(TH1F*)(gDirectory->Get("h_in_L"));
      h_in_R=(TH1F*)(gDirectory->Get("h_in_R"));
      h_out_L=(TH1F*)(gDirectory->Get("h_out_L"));
      h_out_R=(TH1F*)(gDirectory->Get("h_out_R"));
      h_suspect=(TH1F*)(gDirectory->Get("h_suspect"));
      h_bad=(TH1F*)(gDirectory->Get("h_bad"));
      
      if(h_in_L) mean[0] = h_in_L->GetMean();
      if(h_in_R) mean[1] = h_in_R->GetMean();
      if(h_out_L) mean[2] = h_out_L->GetMean();
      if(h_out_R) mean[3] = h_out_R->GetMean();
      if(h_suspect) mean[4] = h_suspect->GetMean();
      if(h_bad) mean[5] = h_bad->GetMean();


      for(Int_t i=0;i<6;i++)
	std::cout<<mean[i]<<std::endl;


      if(!(dCmbMeasurementType->IsEnabled())) measurement_type = "sensitivity";
      //
      // Query Database for Data
      //
      mysqlpp::StoreQueryResult read_data;
      Int_t row_size;
      read_data = QueryDetector();
      row_size =  read_data.num_rows();
      if(ldebug) std::cout<<" row_size="<<row_size<<"\n";
    
      //check for empty queries. If empty exit with error.
      if(row_size == 0){
      
	std::cout  <<"There is no data for "<<measurement_type<<" of "<<device<<" in the given range!"<<std::endl;
	return;
      }

      Float_t x = 0.0, xerr = 0.0;

      std::cout<<"############################################\n";
      std::cout<<"QwGUI : Collecting data.."<<std::endl;
      std::cout<<"QwGUI : Retrieved "<<row_size<<" data points\n";
      for (Int_t i = 0; i < row_size; ++i)
	{
	  if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
	    x    = (read_data[i]["value"])*1e6; // convert to  ppm/ nm
	    xerr = (read_data[i]["error"])*1e6; // convert to  ppm/ nm
	  } 
	  else{
	    x    = read_data[i]["value"];
	    xerr = (read_data[i]["error"]);
	  }




	  // Now fill the mean values
	  if(read_data[i]["run_quality_id"] == "1"){
	    if(read_data[i]["slow_helicity_plate"] == "out") {
	      if (read_data[i]["wien_reversal"]*1 == 1){
		h_out_R_pull->Fill((x-mean[3])/xerr);
		k++;
	      } else {
		h_out_L_pull->Fill((x-mean[2])/xerr);
		l++;
	      }
	    
	    }

	    if(read_data[i]["slow_helicity_plate"] == "in") {
	      if (read_data[i]["wien_reversal"]*1 == 1){
		h_in_R_pull->Fill((x-mean[1])/xerr);
		m++;
	      } else {
		h_in_L_pull->Fill((x-mean[0])/xerr);
		// 	      std::cout<<"mean = "<<x<<std::endl;	  
		// 	      std::cout<<"mean err = "<<xerr<<std::endl;	  
		// 	      std::cout<<"pull "<<(x-mean[0])/xerr<<std::endl;
		n++;

	      }
	    }
	  }
	
	  if((read_data[i]["run_quality_id"] == "2")   | //or
	     (read_data[i]["run_quality_id"] == "1,2") | //or
	     (read_data[i]["run_quality_id"] == "2,3") ) { //all instances of bad
	    h_bad_pull->Fill((x-mean[5])/xerr);
	    o++;
	  }

	  if((read_data[i]["run_quality_id"] == "3") | //suspect or
	     (read_data[i]["run_quality_id"] == "1,3")) {// suspect (but not bad)
	    h_suspect_pull->Fill((x-mean[4])/xerr);
	    p++;
	  }
	
	}


      // Check to make sure graph is not empty.
      if(m==0 && k==0 && l==0 && n==0 && o==0 && p==0){
	std::cout<<"QwGUI : No data were found for the given criteria."<<std::endl;
	exit(1);
      }
      else
	std::cout<<"QwGUI : Moving on to draw the histogram"<<std::endl;
    
  

      if(m>0){
	//GROUP_IN_R
	h_in_R_pull ->SetMarkerSize(1.0);
	h_in_R_pull ->SetMarkerStyle(21);
	h_in_R_pull ->SetFillColor(kBlue);
	h_in_R_pull ->SetMarkerColor(kBlue);
	h_in_R_pull ->SetLineColor(kBlue);
	//h_in_R_pull ->Fit("gaus");
	//fit1 = h_in_R_pull->GetFunction("gaus");
	//fit1 -> SetLineColor(kBlue);
	hs->Add(h_in_R_pull);
      }
      if(k>0){
	//GROUP_OUT_R
	h_out_R_pull ->SetMarkerSize(1.0);
	h_out_R_pull ->SetMarkerStyle(21);
	h_out_R_pull ->SetFillColor(kRed);
	h_out_R_pull ->SetMarkerColor(kRed);
	h_out_R_pull ->SetMarkerColor(kRed);
	h_out_R_pull ->SetLineColor(kRed);
	//h_out_R_pull ->Fit("gaus");
	//fit2 = h_out_R_pull->GetFunction("gaus");
	// fit2 -> SetLineColor(kRed);
	hs->Add(h_out_R_pull);
      }
      if(n>0){
	//GROUP_IN_L
	h_in_L_pull ->SetMarkerSize(1.0);
	h_in_L_pull ->SetMarkerStyle(21);
	h_in_L_pull ->SetFillColor(kOrange-2);
	h_in_L_pull ->SetMarkerColor(kOrange-2);
	h_in_L_pull ->SetLineColor(kOrange-2);
	// h_in_L_pull ->Fit("gaus");
	//fit3 = h_in_L_pull->GetFunction("gaus");
	//fit3 -> SetLineColor(kOrange-2);
	hs->Add(h_in_L_pull);
   
      }
      if(l>0){
	//GROUP_OUT_L
	h_out_L_pull ->SetMarkerSize(1.0);
	h_out_L_pull ->SetMarkerStyle(21);
	h_out_L_pull ->SetFillColor(kViolet);
	h_out_L_pull ->SetMarkerColor(kViolet);
	h_out_L_pull ->SetLineColor(kViolet);
	// h_out_L_pull ->Fit("gaus");
	//fit4 = h_out_L_pull->GetFunction("gaus");
	//fit4 -> SetLineColor(kViolet);
	hs->Add(h_out_L_pull);
      }
      if(o>0){
	//GROUP_BAD
	h_bad_pull ->SetMarkerSize(1.0);
	h_bad_pull ->SetMarkerStyle(29);
	h_bad_pull ->SetFillColor(kBlack);
	h_bad_pull ->SetMarkerColor(kBlack);
	h_bad_pull ->SetLineColor(kBlack);
	// h_bad_pull ->Fit("gaus");
	//fit5 = h_bad_pull->GetFunction("gaus");
	//fit5 -> SetLineColor(kBlack);
	hs->Add(h_bad_pull);
      }
      if(p>0){
	//GROUP_SUSPECT
	h_suspect_pull ->SetMarkerSize(1.0);
	h_suspect_pull ->SetMarkerStyle(29);
	h_suspect_pull ->SetFillColor(kGreen);
	h_suspect_pull ->SetMarkerColor(kGreen);
	h_suspect_pull ->SetLineColor(kGreen);
	// h_suspect_pull ->Fit("gaus");
	//fit6 = h_suspect_pull->GetFunction("gaus");
	//fit6 -> SetLineColor(kGreen);
	hs->Add(h_suspect_pull);
      }


      TLegend *legend = new TLegend(0.70,0.70,0.99,0.99,"","brNDC");

      if(m>0) {
	legend->AddEntry(h_in_R_pull, "<IN_R> pull", "p"); 
	legend->AddEntry((TObject*)0, Form("[mean = %2.3f #pm %2.3f][rms = %2.3f #pm %2.3f] ", 
					   h_in_R_pull->GetMean(), h_in_R_pull->GetMeanError()
					   ,h_in_R_pull->GetRMS(),h_in_R_pull->GetRMSError()), "");
	legend->AddEntry((TObject*)0, "", "");  
      }
      
      if(k>0) 
	{
	  legend->AddEntry(h_out_R_pull, "<OUT_R> pull", "p"); 
	  legend->AddEntry((TObject*)0,Form("[mean = %2.3f #pm %2.3f][rms = %2.3f #pm %2.3f]", 
					    h_out_R_pull->GetMean(), h_out_R_pull->GetMeanError(),
					    h_out_R_pull->GetRMS(), h_out_R_pull->GetRMSError()),"");
	  legend->AddEntry((TObject*)0, "", "");  
	}
      if(n>0)
	{
	  legend->AddEntry(h_in_L_pull, "<IN_R> pull", "p"); 
	  legend->AddEntry((TObject*)0, Form("[mean = %2.3f #pm %2.3f][rms = %2.3f #pm %2.3f]",
					     h_in_L_pull->GetMean(), h_in_L_pull->GetMeanError(),
					     h_in_L_pull->GetRMS(), h_in_L_pull->GetRMSError()), "");
	  legend->AddEntry((TObject*)0, "", "");  
	  
	}
      if(l>0) 
	{
	  legend->AddEntry(h_out_L_pull, "<OUT_L> pull", "p"); 
	  legend->AddEntry((TObject*)0,Form("[mean = %2.3f #pm %2.3f][rms = %2.3f #pm %2.3f]", 
					    h_out_L_pull->GetMean(), h_out_L_pull->GetMeanError(),
					    h_out_L_pull->GetRMS(), h_out_L_pull->GetRMSError()), "");
	  legend->AddEntry((TObject*)0, "", "");  
	}
      if(o>0) 
	{
	  legend->AddEntry(h_bad_pull, "<BAD> pull", "p"); 
	  legend->AddEntry((TObject*)0,Form("[mean = %2.3f #pm %2.3f][rms = %2.3f #pm %2.3f]",
					    h_bad_pull->GetMean(), h_bad_pull->GetMeanError(),
					    h_bad_pull->GetRMS(), h_bad_pull->GetRMSError()), ""); 
	  legend->AddEntry((TObject*)0, "", "");  
	}
      if(p>0)
	{
	  legend->AddEntry(h_suspect_pull, "<SUSPECT> pull", "p"); 
	  legend->AddEntry((TObject*)0,Form("[mean = %2.3f #pm %2.3f][rms = %2.3f #pm %2.3f]", 
					    h_suspect_pull->GetMean(), h_suspect_pull->GetMeanError(),
					    h_suspect_pull->GetRMS(), h_suspect_pull->GetRMSError()), ""); 
	  
	}
      
      legend->SetFillColor(0);
      
      mc->cd();
      hs->Draw();
      legend->Draw("");
      gPad->Update();
      hs->GetYaxis()->SetTitleOffset(1.5);
      hs->GetXaxis()->SetTitleOffset(1.5);
      hs->GetYaxis()->SetTitle("Runlets");
      hs->GetXaxis()->SetTitle("Pull");
      hs->GetYaxis()->SetTitleSize(0.03);
      hs->GetXaxis()->SetTitleSize(0.03);
      hs->GetYaxis()->CenterTitle();
      mc->Modified();
      mc->Update();
    }
    else{
      mc->Clear();
      TPaveText *T1 = new TPaveText(0.05,0.05,0.95,0.95,"ndc");
      T1->SetTextAlign(12);
      T1->SetTextSize(0.03);
      T1->AddText("Pull plots are not implemented for RMS and BOTH options !");
      T1->Draw();
      mc->Modified();
      mc->SetBorderMode(0);
      mc->Update();
    }
  }
  else{
    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);
    //
    mc->cd();
    TLatex T1;
    T1.SetTextAlign(12);
    T1.SetTextSize(0.03);
    T1.DrawLatex(0.1,0.8,"To execute the command, you need first to connect to the database !");
    T1.DrawLatex(0.1,0.6,"To connect to the database:");
    T1.DrawLatex(0.2,0.5,"#bullet Use the pull-down menu at the top left of the GUI (File)");
    T1.DrawLatex(0.2,0.4,"#bullet Choose Open (Database)");      
    T1.DrawLatex(0.2,0.3,"#bullet You can then resubmit your request.");      
    
    mc->Modified();
    mc->SetBorderMode(0);
    mc->Update();
    
  }
  std::cout<<"QwGUI : Done!"<<std::endl;

}

/******************************************

Get the Y axis title for a plot based on the measurement and detector type

*******************************************/

TString QwGUIDatabase::GetYTitle(TString measurement_type, Int_t det_id)
{

  TString ytitle;

 if (measurement_type == "y") {
    if(det_id == ID_MD || det_id == ID_LUMI)  ytitle = "Normalized Yield mV/uA";
    else  ytitle  = "Yield";
  }
  if (measurement_type == "a" || measurement_type == "aeo" || measurement_type == "a12" )
    ytitle = "Asymmetry (ppm)";

  if (measurement_type == "d" || measurement_type == "deo" || measurement_type == "d12" ){
    if (det_id == ID_E_CAL) ytitle  = "Energy Asymmetry (ppm)";
    else if (det_id == ID_CMB_BPM ){
      if(property.Contains("Slope")){ //angles
	ytitle  = "Beam Angle Differences(#murad)";
      }
      ytitle  = "Beam Position Differences (nm)";
    }
    else if (det_id == ID_BCM_DD){
      ytitle = "BCM Double Difference (ppm)";
    }
    else
      ytitle = "Beam Position Differences (nm)";
  }
  
  if (measurement_type == "yq"){
    if (det_id == ID_BCM || det_id == ID_CMB_BCM )  ytitle  = "Current (#muA)";
    if (det_id == ID_BPM || det_id == ID_CMB_BPM)
      ytitle  = "Effective Charge (arbitary units)";
  }
  
  if (measurement_type == "yp"){
    if (det_id == ID_E_CAL)  ytitle  = "Relative Momentum Change dP/P";
    else if (det_id == ID_CMB_BPM ){
      if(property.Contains("Slope")) //angles
	ytitle  = "Beam Angle (#murad)";
      else if (property.Contains("Intercept")) //intercept
	ytitle  = "Intercept (mm)";
      else
	ytitle  = "Beam Position (mm)";
    }else
      ytitle = "Beam Position (mm)";
  }
  if (measurement_type == "s")
    ytitle = "Sensitivity (ppm/mm)";

  return ytitle;
}

/******************************************

Get the  title for a plot based on the measurement and detector type

*******************************************/
TString QwGUIDatabase::GetTitle(TString measurement_type, TString device)
{

  TString title;
  TString xaxis;

  switch(dCmbXAxis->GetSelected())
    {
    case ID_X_RUN:
      xaxis = "vs Run Number";
    case ID_X_TIME:
      xaxis = "vs Run start time";
    case ID_X_TIME_RUNLET:
      xaxis = "vs Runlet start time";
    case ID_X_SLUG:
      xaxis = "vs Slug";
    case ID_X_HISTO:
    case ID_X_PULL_PLOT:
      xaxis = "";
      break;
    default:
      break;
    }
//   if (measurement_type == "y")
//     title = Form("%s Yield ",device.Data());
//   if (measurement_type == "a" || measurement_type == "aeo" || measurement_type == "a12" )
//     title = Form("%s Asymmetry ",device.Data());
//   if (measurement_type == "d" || measurement_type == "deo" || measurement_type == "d12" ){
//     if (det_id == ID_E_CAL) title  =  Form("%s Asymmetry",device.Data());
//     else if (det_id == ID_CMB_BPM ){
//       if(property.Contains("Slope")){ //angles
// 	title  =  Form("%s Beam Angle differe",device.Data());
//       }
//       ytitle  = "Beam Position Differences (nm)";
//     }
//     else if (det_id == ID_BCM_DD){
//       ytitle = "BCM Double Difference (ppm)";
//     }
//     else
//       ytitle = "Beam Position Differences (nm)";
//     title = Form("%s Beam Position Difference vs Runlet Number",device.Data());
//   }
//   if (measurement_type == "yq")
//     title = Form("%s Current ",device.Data());
//   if (measurement_type == "yp")
//     title = Form("%s Beam Position ",device.Data());

  if(det_id==ID_MD_SENS || det_id==ID_LUMI_SENS){
      title = Form("%s %s %s ",device.Data(),xaxis.Data(),reg_iv.Data());
      title+=xaxis;
  }
  else {
      title = Form("%s %s ",device.Data(),xaxis.Data());
      title+=xaxis;
  }
  
  return title;
}

/*****************************************
A function to get data selection

*****************************************/
void QwGUIDatabase::GetDataSelections(){

  index_first       = dNumStartRun -> GetIntNumber();
  index_last        = dNumStopRun  -> GetIntNumber();
  measurement_type  = measurements[dCmbMeasurementType->GetSelected()];
  det_id            = dCmbInstrument->GetSelected();
  device            = Form("%s",detector.Data());
  regression_set    = RegressionSchemes[dCmbRegressionType->GetSelected()];
  target            = Targets[dCmbTargetType->GetSelected()];
  subblock          = dCmbSubblock -> GetSelected();
  plot              = Plots[dCmbPlotType->GetSelected()];
  x_axis            = dCmbXAxis->GetSelected();
}


/*****************************************
A function to print out regression selection and when sensitivities are requested to 
fill the different IVs in to the detector combo box

*****************************************/
void QwGUIDatabase::RegressionTypeInfo(){

  Bool_t bfill_reg_iv = kFALSE;
  TCanvas *mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Draw();
  mc->SetFillColor(0);
  regression_ivs = NULL;

  regression_set    = RegressionSchemes[dCmbRegressionType->GetSelected()];

  if((dCmbInstrument->GetSelected() == ID_MD_SENS) || (dCmbInstrument->GetSelected() == ID_LUMI_SENS)){
    dCmbProperty->RemoveAll();
    bfill_reg_iv = kTRUE;
  }

  TPaveText *T1 = new TPaveText(0.05,0.05,0.95,0.95,"ndc");
//   TLatex * T1 = new TLatex();
  T1->SetTextAlign(12);
  T1->SetTextSize(0.03);

  if(regression_set == "raw(from rootfiles)"){
    T1->AddText("Selecting unregressed data that have passed standard QwAnalysis event cuts!");

    if(bfill_reg_iv){
      for (Int_t i = 0; i < N_REG_VARS_ON; i++){
	dCmbProperty->AddEntry(RegressionVarsOn[i], i);
	regression_ivs = RegressionVarsOn;
      }
    }
    T1->AddText("These data are unregressed and are averages of outputs that can be find in rootfiles.");
  }
  else if(regression_set == "off"){
    T1->AddText("Selecting unregressed data that passed regression script event cuts!");
    if(bfill_reg_iv){
      for (Int_t i = 0; i < N_REG_VARS_ON; i++){
	dCmbProperty->AddEntry(RegressionVarsOn[i], i);
	regression_ivs = RegressionVarsOn;
      }
    }
  }
  else{
    T1->AddText("Selecting Regression that use IVs:");

    if(regression_set == "on"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn[i], i);
	  regression_ivs = RegressionVarsOn;
	}
      }
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_energy");    
      gPad->Update();  
      
    }
    else if(regression_set == "on_5+1"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_5_1; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_5_1[i], i);
	  regression_ivs = RegressionVarsOn_5_1;
	}
      }
      
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_energy"); 
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set3"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_3; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_3[i], i);
	  regression_ivs = RegressionVarsOn_3;
	}
      }
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_bpm3c12X"); 
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set4"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_4; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_4[i], i);
	  regression_ivs = RegressionVarsOn_4;
	}
      }
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_energy"); 
      T1->AddText("#bullet asym_bcm5"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set5"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_5; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_5[i], i);
	  regression_ivs = RegressionVarsOn_5;
	}
      }
      T1->AddText("#bullet diff_9b_m_4X=diff_qwk_3h09bX-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9b_m_4Y=diff_qwk_3h09bY-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9b_p_4X=diff_qwk_3h09bX+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9b_p_4Y=diff_qwk_3h09bY+diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_bpm3c12X"); 
      gPad->Modified();  
      gPad->Update();  

    }
    else if(regression_set == "on_set6"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_6; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_6[i], i);
	  regression_ivs = RegressionVarsOn_6;
	}
      }
      T1->AddText("#bullet diff_9b_m_4X=diff_qwk_3h09bX-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9b_m_4Y=diff_qwk_3h09bY-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9b_p_4X=diff_qwk_3h09bX+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9b_p_4Y=diff_qwk_3h09bY+diff_qwk_3h04Y");
      T1->AddText("#bullet diff__bpm3c12X");       
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set7"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_7; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_7[i], i);
	  regression_ivs = RegressionVarsOn_7;
	}
      }
      T1->AddText("#bullet diff_9_m_4X=diff_qwk_3h09X-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9_m_4Y=diff_qwk_3h09Y-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9_p_4X=diff_qwk_3h09X+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9_p_4Y=diff_qwk_3h09Y+diff_qwk_3h04Y");
      T1->AddText("#bullet diff__bpm3c12X");       
      gPad->Update();  

    }
    else if(regression_set == "on_set8"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_8; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_8[i], i);
	  regression_ivs = RegressionVarsOn_8;
	}
      }
      T1->AddText("#bullet diff_9_m_4X=diff_qwk_3h09X-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9_m_4Y=diff_qwk_3h09Y-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9_p_4X=diff_qwk_3h09X+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9_p_4Y=diff_qwk_3h09Y+diff_qwk_3h04Y");
      T1->AddText("#bullet diff__bpm3c12X");       
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set9"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_9; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_9[i], i);
	  regression_ivs = RegressionVarsOn_9;
	}
      }
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");
      T1->AddText("#bullet diff_targetXSlope");
      T1->AddText("#bullet diff_targetYSlope");
      T1->AddText("#bullet diff_energy");
      T1->AddText("#bullet asym_charge");
      T1->AddText("#bullet asym_uslumi_sum");
      gPad->Update();

    }
    else if(regression_set == "on_set10"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_10; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_10[i], i);
	  regression_ivs = RegressionVarsOn_10;
	}
      }
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");
      T1->AddText("#bullet diff_targetXSlope");
      T1->AddText("#bullet diff_targetYSlope");
      T1->AddText("#bullet diff_energy");
      T1->AddText("#bullet asym_bcm6");
      gPad->Update();

    }
    else if(regression_set == "on_set11"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_11; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_11[i], i);
	  regression_ivs = RegressionVarsOn_11;
	}
      }
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");
      T1->AddText("#bullet diff_targetXSlope");
      T1->AddText("#bullet diff_targetYSlope");
      T1->AddText("#bullet diff_qwk_bpm3c12X");
      gPad->Update();

    }
    else if(regression_set == "on_set12"){
      if(bfill_reg_iv){
	for (Int_t i = 0; i < N_REG_VARS_ON_12; i++){
	  dCmbProperty->AddEntry(RegressionVarsOn_12[i], i);
	  regression_ivs = RegressionVarsOn_12;
	}
      }
      T1->AddText("#bullet diff_target_plus4X");
      T1->AddText("#bullet diff_target_plus4Y");
      T1->AddText("#bullet diff_target_plus4XSlope");
      T1->AddText("#bullet diff_target_plus4YSlope");
      T1->AddText("#bullet diff_qwk_bpm3c12X");
      gPad->Update();

    }
  } 
  T1->Draw();
  if(bfill_reg_iv)
    dCmbProperty->Select(0);

  mc->Modified();
  mc->Update();
}




/******************************************

Things to do when submit is pushed

*******************************************/
void QwGUIDatabase::OnSubmitPushed() 
{

  ClearData();
  GetDataSelections();

  switch (dCmbInstrument->GetSelected()) {
  case ID_MD:
    detector = DetectorCombos[dCmbDetector->GetSelected()];
    property="";
    break;
  case ID_LUMI:
    detector = LumiCombos[dCmbDetector->GetSelected()];
    property="";
    break;
  case ID_BPM:
    detector = BeamPositionMonitors[dCmbDetector->GetSelected()];
    property = BPMReadings[dCmbProperty->GetSelected()];
    break;
  case ID_BCM:
    detector = BeamCurrentMonitors[dCmbDetector->GetSelected()];
    property = "";
    break;
  case ID_BCM_DD:
    detector = BCMDoubleDifferences[dCmbDetector->GetSelected()];
    property = "";
    break;
  case ID_CMB_BCM:
    detector = CombinedBCMS[dCmbDetector->GetSelected()];
    property = "";
    break;
  case ID_CMB_BPM:
    detector = CombinedBPMS[dCmbDetector->GetSelected()];
    property = ComboBPMReadings[dCmbProperty->GetSelected()];
    break;
  case ID_E_CAL:
    detector = EnergyCalculators[dCmbDetector->GetSelected()];
    property = "";
    break;
  case ID_MD_SENS:
    detector = DetectorCombos[dCmbDetector->GetSelected()];
    if(regression_ivs==NULL){
      std::cout<<"Please select a regression Type. If you are using qw_run1_pass3 please select 'off' option."<<std::endl;
      return;
    } 
    else
      property = regression_ivs[dCmbProperty->GetSelected()];
    break;
  case ID_LUMI_SENS:
    detector = LumiCombos[dCmbDetector->GetSelected()];
    if(regression_ivs==NULL){
      std::cout<<"Please select a regression Type. If you are using qw_run1_pass3 please select 'off' option."<<std::endl;
      return;
    } 
    else
    property = regression_ivs[dCmbProperty->GetSelected()];
    break;
  default:
    break;
  }
  
  switch (dCmbXAxis->GetSelected()) 
    {
    case ID_X_RUN:
    case ID_X_SLUG:
    case ID_X_TIME:
    case ID_X_TIME_RUNLET:
      PlotDetector();
      break;
    case ID_X_BEAM:
      // DetectorVsMonitorPlot();
      break;
    case ID_X_HISTO:
      HistogramDetector();
      break;
   case ID_X_PULL_PLOT:
      CreatePullPlot();
      break;
    default:
      break;
    }
}

// Process events generated by the object in the frame.
Bool_t QwGUIDatabase::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

  switch (GET_MSG(msg))
    {
    case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {
      case kTE_TEXTCHANGED:
      case kTE_ENTER:
      case kTE_TAB:
      default:
	break;
      } // kC_TEXTENTRY
      
      
    case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_COMBOBOX:
	switch(parm1) {
	case CMB_XAXIS:
	case CMB_INSTRUMENT:
	case CMB_DETECTOR:
	case CMB_SUBBLOCK:
	case CMB_MEASUREMENT_TYPE:
	default:
	  break;
	} // Combo Box 
      case kCM_BUTTON:
	switch(parm1) {
	case BTN_SUBMIT:
	  OnSubmitPushed();
	  break;
	default:
	  break;
	} // Button
	
      default:
	break;
      } // kC_COMMAND
    }
  
  return kTRUE;
}



