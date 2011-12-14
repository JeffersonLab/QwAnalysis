#include "QwGUIDatabase.h"

#include <TG3DLine.h>
#include "TGaxis.h"
#include "TGTextEntry.h"
#include "TStopwatch.h"
#include "TLatex.h"
#include "TDatime.h"

ClassImp(QwGUIDatabase);



// Parameters to plot in X axis
enum EQwGUIDatabaseXAxisIDs {
  ID_X_HISTO,
  ID_X_RUN,
  ID_X_SLUG,
  ID_X_BEAM,
  ID_X_TIME,
  ID_X_WEIN,
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
    "qwk_mdoddbars",
    "qwk_mdevenbars",
    "qwk_mdallbars",
    "dd_mdeven_odd",
    "dd_md15_37",
    "dd_md26_48"
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

const char *QwGUIDatabase::BeamCurrentMonitors[N_BCMS] = 
{
  "qwk_bcm1","qwk_bcm2","qwk_bcm5","qwk_bcm6","qwk_linephase", "qwk_invert_tstable",
  "bcmdd12","bcmdd15","bcmdd16","bcmdd56"
 
};

const char *QwGUIDatabase::LumiCombos[N_LUMIS] = 
{
  "qwk_uslumi1neg",
  "qwk_uslumi1pos",
  "qwk_uslumi3neg",
  "qwk_uslumi3pos",
  "qwk_uslumi5neg",
  "qwk_uslumi5pos",
  "qwk_uslumi7neg",
  "qwk_uslumi7pos",
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
  "dslumi_sum"
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

const char *QwGUIDatabase::RegressionVars[N_REGRESSION_VARS] = {
  "targetX","targetY","targetXSlope","targetYSlope","energy"
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
    "yq","a","a12", "aeo", "r", "d"
  };

const char *QwGUIDatabase::PositionMeasurementTypes[N_POS_MEAS_TYPES]=
  {
    "yp", "d", "d12", "deo","r"
  };

const char *QwGUIDatabase::DetectorMeasurementTypes[N_DET_MEAS_TYPES]=
  {
    "y", "a", "a12", "aeo", "r"
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
  "Unknown position","US-Pure-Al","DS-Cntring-hole","DS-0.5%-C","US-Cntring-hole"
};


const char *QwGUIDatabase::Plots[N_Plots] =
  {
    "Mean", "RMS", "Both"
  }; 

const char *QwGUIDatabase::GoodForTypes[N_GOODFOR_TYPES] =
{
  "production","commissioning","parity","tracking",
  "centering_target","centering_plug","pedestals","transverse"
};

const char *QwGUIDatabase::X_axis[N_X_AXIS] =
{
  "Vs. Run Number","Vs. Slug","Histogram","Vs. Time", "Vs. Wein"
};


QwGUIDatabase::QwGUIDatabase(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dQualityFrame	      = NULL;
  dRunFrame	      = NULL;
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
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.2);     
 

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.12);  
  gStyle->SetPadRightMargin(0.03);  

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleOffset(2);
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
  dLabRegression      = new TGLabel(dControlsFrame, "Regression Correction");
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
  dRunFrame	      = new TGHorizontalFrame(dControlsFrame);
  dNumStartRun        = new TGNumberEntry(dRunFrame, 9000, 5, NUM_START_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dNumStopRun         = new TGNumberEntry(dRunFrame, 11250, 5, NUM_STOP_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
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
  dCmbInstrument->AddEntry("LUMI Detectors", ID_LUMI);
  dCmbInstrument->AddEntry("Combined BPMs", ID_CMB_BPM);
  dCmbInstrument->AddEntry("Combined BCMs", ID_CMB_BCM);
  dCmbInstrument->AddEntry("Energy Calculator", ID_E_CAL);
  dCmbInstrument->AddEntry("Main Detector Sensitivities", ID_MD_SENS);
  dCmbInstrument->AddEntry("LUMI Detector Sensitivities", ID_LUMI_SENS);
  dCmbInstrument->Connect("Selected(Int_t)","QwGUIDatabase", this, "PopulateDetectorComboBox()");
  dCmbInstrument->Select(dCmbInstrument->FindEntry("Main Detectors")->EntryId());
  dCmbProperty->Connect("Selected(Int_t)","QwGUIDatabase", this, "PopulateMeasurementComboBox()");

  // Populate data blocks
  for (Int_t i = 0; i < N_SUBBLOCKS; i++) {
    dCmbSubblock->AddEntry(Subblocks[i], i);
  }
  // Populate target combo box
  for (Int_t i = 0; i < N_TGTS; i++) {
    dCmbTargetType->AddEntry(Targets[i], i);
  }

  // Populate x-axis combo box
  dCmbXAxis->AddEntry(X_axis[0],ID_X_RUN);
  dCmbXAxis->AddEntry(X_axis[1],ID_X_SLUG);
  dCmbXAxis->AddEntry(X_axis[2],ID_X_HISTO);
  dCmbXAxis->AddEntry(X_axis[3],ID_X_TIME);
  dCmbXAxis->AddEntry(X_axis[4],ID_X_WEIN);

  // Populate regression combo box
  dCmbRegressionType->AddEntry("off", 0);
  dCmbRegressionType->AddEntry("on", 1);
  

  // Populate good for list
  for (Int_t i = 0; i<N_GOODFOR_TYPES; i++){
    dBoxGoodFor->AddEntry(GoodForTypes[i],i+1);
  }

  for (Int_t k = 0; k < N_Plots; k++)
    dCmbPlotType->AddEntry(Plots[k], k);

  dCmbSubblock->Select(0);
  dCmbTargetType->Select(1);
  dCmbRegressionType->Select(0);
  dCmbPlotType->Select(0);
  dCmbXAxis->Select(1);


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
    dCmbRegressionType->SetEnabled(kFALSE);
    dCmbMeasurementType->SetEnabled(kFALSE);

    for (Int_t i = 0; i < N_DETECTORS; i++) {
      dCmbDetector->AddEntry(DetectorCombos[i], i);
    }
    for (Int_t i = 0; i < N_REGRESSION_VARS; i++) {
      dCmbProperty->AddEntry(RegressionVars[i], i);
    }
  }

  if (dCmbInstrument->GetSelected() == ID_LUMI_SENS) {
    dCmbProperty->SetEnabled(kTRUE);
    dCmbSubblock->SetEnabled(kFALSE);
    dCmbRegressionType->SetEnabled(kFALSE);
    dCmbMeasurementType->SetEnabled(kFALSE);

    for (Int_t i = 0; i < N_LUMIS; i++) {
      dCmbDetector->AddEntry(LumiCombos[i], i);
    }
    for (Int_t i = 0; i < N_REGRESSION_VARS; i++) {
      dCmbProperty->AddEntry(RegressionVars[i], i);
    }
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
    gPad->SetLogy(0);
    ((TGraph*)obj)->Draw("ap");
    leg = (TLegend*)LegendArray.At(ind);
    if(leg) leg->Draw("");
    ind++;
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

  vector <TObject*> obja;
  TIter *next1 = new TIter(DataWindowArray.MakeIterator());
  obj = next1->Next();
  while(obj){
    obja.push_back(obj);
//     delete obj;
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
	// 	dDataWindow->SetLimitsFlag(kTrue);
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

  TString correction_flag;


  /*Basic data selections that are valid for any type of query*/
  //Int_t   subblock          = dCmbSubblock -> GetSelected();
  //  TString target            = Targets[dCmbTargetType->GetSelected()];

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
  TList *selected_types = new TList;
  dBoxGoodFor->GetSelectedEntries(selected_types);
  TGTextLBEntry *entry;
  TIter next(selected_types);
  TString good_for_check = " AND data.good_for_id is not NULL";

  for(size_t i=0; i<N_GOODFOR_TYPES;i++){ //initialize them as false
	  good_for[i] = kFALSE;
  }
  while( (entry = (TGTextLBEntry *)next()) ) {
	  good_for[dBoxGoodFor->FindEntry(entry->GetTitle())->EntryId()-1] = kTRUE;
  }

  for(size_t i=0; i<N_GOODFOR_TYPES;i++){
	  if(!good_for[i]){
		  good_for_check = good_for_check + Form(" AND data.good_for_id NOT LIKE \"%%%d%%\"",i+1);
	  }
  }

  /*Slope corrections ON/OFF?*/
  if (dCmbRegressionType->GetSelected()==1)
    correction_flag = "on";
  else
    correction_flag = "off";


  TString querystring 
    = "SELECT "+outputs+" slow_helicity_plate, data.run_quality_id,data.good_for_id,target_position, (scd.value<0)*-2+1 as wien_reversal"
    +" FROM "+tables_used+" analysis_view as ana, slow_controls_settings , slow_controls_data as scd   "
    +" WHERE "+table_links+" data.analysis_id = ana.analysis_id AND ana.runlet_id = slow_controls_settings.runlet_id "
    + Form(" AND data.slope_correction='%s' ",correction_flag.Data())
    +" AND data.subblock = "+ Form("%i",subblock) 
    +" AND data.error !=0 "
    +" AND target_position = '"+Form("%s",target.Data())+"'"
    +" AND scd.runlet_id=ana.runlet_id "
    + quality_check
    + good_for_check
    + special_cuts
    + ";";


  return querystring;
}



/******************************************

Plot detector data with beam parameters (still not finished)

*******************************************/

void QwGUIDatabase::DetectorVsMonitorPlot()
{
  //  TGraphErrors *grp = NULL;

  // if(dDatabaseCont){
//     dDatabaseCont->Connect();

//     mysqlpp::Query query = dDatabaseCont->Query();

//     // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
//   //   Int_t  subblock  = dCmbSubblock->GetSelected();
//     Int_t  run_first = dNumStartRun->GetIntNumber();
//     Int_t  run_last  = dNumStopRun->GetIntNumber();
//     //string measurement_type = DetectorMeasurementTypes[dCmbMeasurementType->GetSelected()];

//     query << "SELECT xt.run_number AS run, xt.segment_number AS segment, ";
//     query << "xt.q_value AS q, xt.q_error AS q_err, xt.energy_value AS e, xt.energy_error AS e_err, ";
//     query << "xt.x_value AS x, xt.x_error AS x_err, xt.theta_x_value AS theta_x, xt.theta_x_error AS theta_x_err, ";
//     query << "xt.y_value AS y, xt.y_error AS y_err, xt.theta_y_value AS theta_y, xt.theta_y_error AS theta_y_err, ";
//     query << "yt.value AS value, yt.error AS error ";
//     query << "FROM summary_b" << measurement_type << " AS xt, ";
//     if (dCmbInstrument->GetSelected() == ID_MD) {
//       query << "summary_d";
//       //    detector = DetectorCombos[dCmbDetector->GetSelected()];
//     }
//     if (dCmbInstrument->GetSelected() == ID_LUMI) {
//       //  detector = LumiCombos[dCmbDetector->GetSelected()];
//       query << "summary_l"; 
//     }
//     else{
//       std::cerr << "Beam monitors not a valid selection in this context." << std::endl;
//       return;
//     }
//     query << measurement_type << "_calc AS yt ";
//     query << "WHERE ";
//     query << "xt.run_number = yt.run_number ";
//     query << "AND xt.segment_number = yt.segment_number ";
//     query << "AND xt.subblock = yt.subblock ";
//     query << "AND yt.detector = " << mysqlpp::quote << detector << " ";
//     query << "AND xt.subblock = " << subblock << " ";
//     query << "AND xt.run_number BETWEEN ";
//     query << run_first << " AND ";
//     query << run_last << " ";
//     query << "ORDER BY xt.run_number, xt.segment_number";

//     std::cout << query.str() << std::endl;

//     mysqlpp::StoreQueryResult res = query.store();

//     dDatabaseCont->Disconnect(); 

//     Int_t res_size = 0;
//       res_size = res.num_rows();
//       std::cout << "Number of rows:  " << res_size << std::endl;
//     TVectorF q(res_size), qerr(res_size), e(res_size), eerr(res_size);
//     TVectorF x(res_size), xerr(res_size), y(res_size), yerr(res_size);
//     TVectorF theta_x(res_size), theta_xerr(res_size), theta_y(res_size), theta_yerr(res_size);
//     TVectorF d(res_size), derr(res_size);

//     if (res) {
//       /*
//       q.ResizeTo(res_size);
//       qerr.ResizeTo(res_size);
//       e.ResizeTo(res_size);
//       eerr.ResizeTo(res_size);
//       x.ResizeTo(res_size);
//       xerr.ResizeTo(res_size);
//       theta_x.ResizeTo(res_size);
//       theta_xerr.ResizeTo(res_size);
//       y.ResizeTo(res_size);
//       yerr.ResizeTo(res_size);
//       theta_y.ResizeTo(res_size);
//       theta_yerr.ResizeTo(res_size);
//       */

//       for (Int_t i = 0; i < res_size; i++) {
//         q[i] = res[i]["q"];
//         qerr[i] = res[i]["q_err"];
//         e[i] = res[i]["e"];
//         eerr[i] = res[i]["e_err"];
//         x[i] = res[i]["x"];
//         xerr[i] = res[i]["x_err"];
//         theta_x[i] = res[i]["theta_x"];
//         theta_xerr[i] = res[i]["theta_x_err"];
//         y[i] = res[i]["y"];
//         yerr[i] = res[i]["y_err"];
//         theta_y[i] = res[i]["theta_y"];
//         theta_yerr[i] = res[i]["theta_y_err"];
//         d[i] = res[i]["value"];
//         derr[i] = res[i]["error"];
//       }

//     //
//     // Construct Graphs for Plotting
//     //

//     string grp_title;

//     for (Int_t i = 0; i < N_BPMS; i++) {
//       switch (i) {
//         case 0: // X position @ target
//           grp = new TGraphErrors(x, d, xerr, derr);
//           grp_title = "Detector vs. X";
//           if (measurement_type == "y") {
// 	          grp->GetXaxis()->SetTitle("Position []");
//           }
//           if (measurement_type == "a") {
// 	          grp->GetXaxis()->SetTitle("Difference []");
//             grp_title += " Difference";
//           }
//           break;
//         case 1: // Y position @ target
//           grp = new TGraphErrors(y, d, yerr, derr);
//           grp_title = "Detector vs. Y";
//           if (measurement_type == "y") {
// 	          grp->GetXaxis()->SetTitle("Position []");
//           }
//           if (measurement_type == "a") {
// 	          grp->GetXaxis()->SetTitle("Difference []");
//             grp_title += " Difference";
//           }
//           break;
//         case 2: // X angle @ target
//           grp = new TGraphErrors(theta_x, d, theta_xerr, derr);
//           grp_title = "Detector vs. X Angle";
//           if (measurement_type == "y") {
//             grp->GetXaxis()->SetTitle("Angle []");
//           }
//           if (measurement_type == "a") {
//             grp->GetXaxis()->SetTitle("Difference []");
//             grp_title += " Difference";
//           }
//           break;
//         case 3: // Y angle @ target
//           grp = new TGraphErrors(theta_y, d, theta_yerr, derr);
//           grp_title = "Detector vs. Y Angle";
//           if (measurement_type == "y") {
// 	          grp->GetXaxis()->SetTitle("Angle []");
//           }
//           if (measurement_type == "a") {
// 	          grp->GetXaxis()->SetTitle("Difference []");
//             grp_title += " Difference";
//           }
//           break;
//         case 4: // Charge @ target
//           grp = new TGraphErrors(q, d, qerr, derr);
//           grp_title = "Detector vs. Charge";
//           if (measurement_type == "y") {
// 	          grp->GetXaxis()->SetTitle("Charge []");
//           }
//           if (measurement_type == "a") {
// 	          grp->GetXaxis()->SetTitle("Asymmetry []");
//             grp_title += " Asymmetry";
//           }
//           break;
//         case 5: // Energy @ target
//           grp = new TGraphErrors(e, d, eerr, derr);
//           grp_title = "Detector vs. Energy";
//           if (measurement_type == "y") {
// 	          grp->GetXaxis()->SetTitle("Energy []");
//           }
//           if (measurement_type == "a") {
// 	          grp->GetXaxis()->SetTitle("Asymmetry? []");
//             grp_title += " Asymmetry?";
//           }
//           break;
//         default:
//           break;
//       }

//       grp->SetTitle(grp_title.c_str());

//       grp->GetXaxis()->CenterTitle();
//   	  grp->GetXaxis()->SetTitleSize(0.03);
//   	  grp->GetXaxis()->SetTitleOffset(1.25);
//       grp->GetYaxis()->SetTitle("Detector []");
//    	  grp->GetYaxis()->CenterTitle();
// 	    grp->GetYaxis()->SetTitleSize(0.03);
// 	    grp->GetYaxis()->SetTitleOffset(1.5);

//       GraphArray.Add(grp);

//     } 
//     //
//     //
//     // Plot Graphs
//     //
//     //
//     Int_t ind = 1;
//     TCanvas *mc = dCanvas->GetCanvas();
//     mc->Clear();
//     mc->Divide(2,3);

//     TObject *obj;
//     TIter next(GraphArray.MakeIterator());
//     obj = next();
//     while(obj){
//       mc->cd(ind);
//       gPad->SetLogy(0);
//       ((TGraph*)obj)->Draw("ap*");
//       ind++;
//       obj = next();
//     }

//     mc->Modified();
//     mc->Update();


//     }
//   }
} //DetectorVsMonitorPlot





/******************************************

Function to query Y vs X data

*******************************************/
mysqlpp::StoreQueryResult  QwGUIDatabase::QueryDetector()
{
  Bool_t ldebug = kTRUE;
  TString reg_iv;

  dDatabaseCont->Connect();

  mysqlpp::Query query1     = dDatabaseCont-> Query();
  mysqlpp::Query query2     = dDatabaseCont-> Query();

  if(!(dCmbMeasurementType->IsEnabled())) measurement_type = "sensitivity";

  if(det_id==ID_MD_SENS || det_id==ID_LUMI_SENS){
	  device			= Form("%s",detector.Data());
	  reg_iv			= Form("wrt_diff_%s",property.Data());
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

  TString querystring;
  TString outputs;
 
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
 
  if(((dCmbXAxis->GetSelected()) == ID_X_RUN) || ((dCmbXAxis->GetSelected()) == ID_X_HISTO)) {
    outputs   = "data.value AS value, data.error AS error, data.error*sqrt(data.n) AS rms, (data.run_number+data.segment_number*0.1) AS x_value,";
    special_cuts += Form(" AND data.run_number >= %i  AND data.run_number <= %i ",index_first, index_last);
  }

  if((dCmbXAxis->GetSelected()) == ID_X_SLUG){
    outputs   = "sum(distinct(data.value/(POWER(data.error,2))))/sum( distinct(1/(POWER(data.error,2)))) AS value, SQRT(1/SUM(distinct(1/(POWER(data.error,2))))) AS error, data.slug AS x_value, ";
    special_cuts += Form(" AND (data.slug >= %i AND data.slug <= %i ) GROUP BY data.slug",index_first,index_last);
  }

  if((dCmbXAxis->GetSelected()) == ID_X_TIME) {
    tables_used+=" run,  ";
    outputs   = "data.value AS value, data.error AS error, data.error*sqrt(data.n) AS rms, run.start_time AS x_value,";
    special_cuts += Form(" AND run.run_number = data.run_number AND (data.run_number >= %i  AND data.run_number <= %i) GROUP BY data.run_number ",index_first, index_last);
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
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.7);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleSize(0.07);
  gStyle->SetTitleOffset(1.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetHistMinimumZero();
  gStyle->SetBarOffset(0.25);
  gStyle->SetBarWidth(0.5);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleColor(kBlack,"X");
  gStyle->SetTitleColor(kBlack,"Y");

  gDirectory->Delete();

  if(dDatabaseCont){

    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);
    if(!(dCmbMeasurementType->IsEnabled())) measurement_type = "sensitivity";
    //
    // Query Database for Data
    //
    mysqlpp::StoreQueryResult read_data;
    size_t row_size;
    read_data = QueryDetector();
    row_size =  read_data.num_rows();
    if(ldebug) std::cout<<" row_size="<<row_size<<"\n";
 
    //check for empty queries. If empty exit with error.
    if(row_size == 0){
      
      std::cout  <<"There is no data for "<<measurement_type<<" of "<<device<<" in the given range!"<<std::endl;
      return;
    }
    
 
    TVectorD x_in(row_size), xerr_in(row_size);
    TVectorD x_out(row_size), xerr_out(row_size);
    TVectorD run_in(row_size), run_out(row_size);
    TVectorD err_in(row_size), err_out(row_size);
    
    TVectorD x_in_L(row_size), xerr_in_L(row_size);
    TVectorD x_out_L(row_size), xerr_out_L(row_size);
    TVectorD run_in_L(row_size), run_out_L(row_size);
    TVectorD err_in_L(row_size), err_out_L(row_size);
    
    TVectorD x_bad(row_size), xerr_bad(row_size);
    TVectorD run_bad(row_size);
    TVectorD err_bad(row_size);
    
    TVectorD x_suspect(row_size), xerr_suspect(row_size);
    TVectorD run_suspect(row_size);
    TVectorD err_suspect(row_size);

    TVectorD x_rms_all(row_size), x_rmserr_all(row_size), run_rms(row_size);

    x_in.Clear();
    x_in.ResizeTo(row_size);
    xerr_in.Clear();
    xerr_in.ResizeTo(row_size);
    run_in.Clear();
    run_in.ResizeTo(row_size);
    err_in.Clear();
    err_in.ResizeTo(row_size);

    x_out.Clear();
    x_out.ResizeTo(row_size);
    xerr_out.Clear();
    xerr_out.ResizeTo(row_size);
    run_out.Clear();
    run_out.ResizeTo(row_size);
    err_out.Clear();
    err_out.ResizeTo(row_size);

    x_in_L.Clear();
    x_in_L.ResizeTo(row_size);
    xerr_in_L.Clear();
    xerr_in_L.ResizeTo(row_size);
    run_in_L.Clear();
    run_in_L.ResizeTo(row_size);
    err_in_L.Clear();
    err_in_L.ResizeTo(row_size);

    x_out_L.Clear();
    x_out_L.ResizeTo(row_size);
    xerr_out_L.Clear();
    xerr_out_L.ResizeTo(row_size);
    run_out_L.Clear();
    run_out_L.ResizeTo(row_size);
    err_out_L.Clear();
    err_out_L.ResizeTo(row_size);

    x_bad.Clear();
    x_bad.ResizeTo(row_size);
    xerr_bad.Clear();
    xerr_bad.ResizeTo(row_size);
    run_bad.Clear();
    run_bad.ResizeTo(row_size);
    err_bad.Clear();
    err_bad.ResizeTo(row_size);

    x_suspect.Clear();
    x_suspect.ResizeTo(row_size);
    xerr_suspect.Clear();
    xerr_suspect.ResizeTo(row_size);
    run_suspect.Clear();
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
    Int_t i_rms = 0;//rms will contain all points, no in/out

    TGraphErrors* grp_in = NULL;
    TGraphErrors* grp_in_L = NULL;
    TGraphErrors* grp_out = NULL;
    TGraphErrors* grp_out_L = NULL;
    TGraphErrors* grp_bad = NULL;
    TGraphErrors* grp_suspect = NULL;
    TGraphErrors* grp_rms = NULL;

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
    for (size_t i = 0; i < row_size; ++i)
      { 	   

	if(x_axis == ID_X_TIME){
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
	  if( (converted >= (DST2010_start->Convert()) and (converted <= (DST2010_end->Convert())))||
	      (converted >= (DST2011_start->Convert()) and (converted <= (DST2011_end->Convert())))||
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

	if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
	  if(plot.Contains("RMS") == 1){
	    x    = (read_data[i]["rms"])*1e6; // convert to  ppm/ nm
	    xerr = 0.0;
	  }
	  else {
	    x    = (read_data[i]["value"])*1e6; // convert to  ppm/ nm
	    xerr = (read_data[i]["error"])*1e6; // convert to ppm/ nm
            x_rms = (read_data[i]["rms"])*1e6; // convert to  ppm/ nm
	  }
	} 
	else{
	  if(plot.Contains("RMS") == 1){
	    x    = (read_data[i]["rms"]);
	    xerr = 0.0;
	  }
	  else {
	    x    = read_data[i]["value"];
	    xerr = read_data[i]["error"];
            x_rms = (read_data[i]["rms"]);
	  }
	}
	  
        
        //Fist fill the rms stuff without quality checks
        if(x_axis == ID_X_TIME){
            run_rms.operator()(i_rms)  = (runtime_start->Convert()) + adjust_for_DST;
        }
        else
            run_rms.operator()(i_rms)  = read_data[i]["x_value"];
        x_rms_all.operator()(i_rms) = x_rms;
        x_rmserr_all.operator()(i_rms) = 0.0;
        i_rms++;
	
        if(read_data[i]["run_quality_id"] == "1"){

	  if(read_data[i]["slow_helicity_plate"] == "out") {
	    if (read_data[i]["wien_reversal"]*1 == 1){

	      if(x_axis == ID_X_TIME){
		run_out.operator()(k)  = (runtime_start->Convert()) + adjust_for_DST;
	      }
	      else
		run_out.operator()(k)  = read_data[i]["x_value"];
	      
	      x_out.operator()(k)    = x;
	      xerr_out.operator()(k) = xerr;
	      err_out.operator()(k)  = 0.0;
	      k++;
	    } else {
	      if(x_axis == ID_X_TIME){
		run_out_L.operator()(l)  = (runtime_start->Convert())+ adjust_for_DST;
	      }
	      else
		run_out_L.operator()(l)  = read_data[i]["x_value"];
	      x_out_L.operator()(l)    = x;
	      xerr_out_L.operator()(l) = xerr;
	      err_out_L.operator()(l)  = 0.0;
	      l++;
	    }
	    
	  }
	  
	  if(read_data[i]["slow_helicity_plate"] == "in") {

	    if (read_data[i]["wien_reversal"]*1 == 1){
	      if(x_axis == ID_X_TIME){
		run_in.operator()(m)  = runtime_start->Convert()+ adjust_for_DST;
	      }
	      else
		run_in.operator()(m)  = read_data[i]["x_value"];
	      x_in.operator()(m)    = x;
	      xerr_in.operator()(m) = xerr;
	      err_in.operator()(m)  = 0.0;
	      m++;
	    } else {
	      if(x_axis == ID_X_TIME){
		run_in_L.operator()(n)  = runtime_start->Convert()+ adjust_for_DST;
	      }
	      else
		run_in_L.operator()(n)  = read_data[i]["x_value"];
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

	  if(x_axis == ID_X_TIME){
	    run_bad.operator()(o)  = runtime_start->Convert()+ adjust_for_DST;
	  }
	  else	    
	    run_bad.operator()(o)  = read_data[i]["x_value"];
	  x_bad.operator()(o)    = x;
	  xerr_bad.operator()(o) = xerr;
	  err_bad.operator()(o)  = 0.0;
	  o++;
	}
	
	if((read_data[i]["run_quality_id"] == "3")  | //or
	   (read_data[i]["run_quality_id"] == "1,3")) {// suspect (but not bad)
	  if(x_axis == ID_X_TIME){
	    run_suspect.operator()(p)  = runtime_start->Convert()+ adjust_for_DST;
	  }
	  else
	    run_suspect.operator()(p)  = read_data[i]["x_value"];
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
      fit5 = grp_out->GetFunction("pol0");
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
    
//Make RMS graph with all events
    //run_rms.ResizeTo(i_rms);
    //x_rms_all.ResizeTo(i_rms);
    //x_rmserr_all.ResizeTo(i_rms);
    grp_rms = new TGraphErrors(run_rms, x_rms_all, x_rmserr_all, x_rmserr_all);


    TMultiGraph * grp = new TMultiGraph();


    TString y_title = GetYTitle(measurement_type, det_id);
    TString title   = GetTitle(measurement_type, device);
    TString x_title;
    if(x_axis == ID_X_TIME)x_title = "Time (YY:MM:DD / HH:MM)";
    else
      if(x_axis == ID_X_RUN)x_title = "Run Number";
      else
	if(x_axis == ID_X_TIME)x_title = "Slug Number";
	else x_title = "";

    if(plot.Contains("RMS"))
      {
	y_title = "RMS of "+y_title;
	title   = "RMS of "+title;
      }


    if(m>0)grp->Add(grp_in);
    if(k>0)grp->Add(grp_out);
    if(n>0)grp->Add(grp_in_L);
    if(l>0)grp->Add(grp_out_L);
    if(o>0)grp->Add(grp_bad);
    if(p>0)grp->Add(grp_suspect);

    TLegend *legend = new TLegend(0.80,0.80,0.99,0.99,"","brNDC");
    if(m>0) legend->AddEntry(grp_in, Form("<IN_R>  = %2.3f #pm %2.3f", 
					  fit1->GetParameter(0), fit1->GetParError(0)), "p");
    if(n>0) legend->AddEntry(grp_in_L, Form("<IN_L>  = %2.3f #pm %2.3f", 
					    fit2->GetParameter(0), fit2->GetParError(0)), "p");
    if(k>0) legend->AddEntry(grp_out,Form("<OUT_R> = %2.3f #pm %2.3f", 
					  fit3->GetParameter(0), fit3->GetParError(0)), "p");
    if(l>0) legend->AddEntry(grp_out_L,Form("<OUT_L> = %2.3f #pm %2.3f", 
					    fit4->GetParameter(0), fit4->GetParError(0)), "p");
    if(o>0) legend->AddEntry(grp_bad,Form("<BAD> = %2.3f #pm %2.3f", 
					  fit5->GetParameter(0), fit5->GetParError(0)), "p");
    if(p>0) legend->AddEntry(grp_suspect,Form("<SUSPECT> = %2.3f #pm %2.3f", 
					      fit6->GetParameter(0), fit6->GetParError(0)), "p");
    legend->SetFillColor(0);

    
    AddNewGraph(grp,legend);
    PlotGraphs();

    if(x_axis == ID_X_TIME){
      grp->GetXaxis()->SetTimeDisplay(1);
      grp->GetXaxis()->SetTimeOffset(0,"gmt");
      grp->GetXaxis()->SetLabelOffset(0.02); 
      grp->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}");
      grp->GetXaxis()->Draw();
    }

    
    grp->SetTitle(title);
    grp->GetYaxis()->SetTitle(y_title);
    grp->GetXaxis()->SetTitle(x_title);
    grp->GetYaxis()->CenterTitle();
    
    if(plot.Contains("Both")){
        mc->cd(2);
        grp_rms->Draw("ab");
        grp_rms->GetHistogram()->SetXTitle(x_title);
        grp_rms->GetHistogram()->SetYTitle("RMS of "+y_title);
        grp_rms->GetYaxis()->CenterTitle();
        if(x_axis == ID_X_TIME){
          grp_rms->GetXaxis()->SetTimeDisplay(1);
          grp_rms->GetXaxis()->SetTimeOffset(0,"gmt");
          grp_rms->GetXaxis()->SetLabelOffset(0.02); 
          grp_rms->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M}");
          grp_rms->GetXaxis()->Draw();
        }
        mc->cd();
    }
    else{
        grp->GetYaxis()->SetTitleOffset(2);
        grp->GetXaxis()->SetTitleOffset(2);
        grp->GetYaxis()->SetTitleSize(0.03);
        grp->GetXaxis()->SetTitleSize(0.03);
    }

    mc->Modified();
    mc->SetBorderMode(0);
    mc->Update();
   
    std::cout<<"QwGUI : Done!"<<std::endl;
    
    
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
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.7);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleSize(0.07);
  gStyle->SetTitleOffset(1.5);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetHistMinimumZero();
  gStyle->SetBarOffset(0.25);
  gStyle->SetBarWidth(0.5);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleColor(kBlack,"X");
  gStyle->SetTitleColor(kBlack,"Y");

  gDirectory->Delete();

  if(dDatabaseCont){

    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);

    if(dCmbMeasurementType->IsEnabled()) measurement_type  = measurements[dCmbMeasurementType->GetSelected()];
    else 
      measurement_type = "sensitivity";

    //Get run quality cut information
    Bool_t quality[3] = {kFALSE, kFALSE, kFALSE};
    if(dChkQualityGood->IsOn())	      quality[0] 	= kTRUE;
    if(dChkQualityBad->IsOn())		  quality[1]	= kTRUE;
    if(dChkQualitySuspect->IsOn())      quality[2] 	= kTRUE;

    TString quality_check = " ";
    for (size_t i=0; i<3;i++){
  	if(!quality[i]){
  		quality_check = quality_check +"  AND run_quality_id not like" + Form("\"%%%d%%\"",i+1);
  	}
    }

    //
    // Query Database for Data
    //
    mysqlpp::StoreQueryResult read_data = QueryDetector();

    //
    // Loop Over Results and Fill Vectors
    //
    size_t row_size =  read_data.num_rows();
    if(ldebug) std::cout<<" row_size="<<row_size<<"\n";

    //check for empty queries. If empty exit with error.
    if(row_size == 0){
      std::cout<<"There is no data for "<<measurement_type<<" of "<<device<<" for subblock "<<subblock
	       <<" in the given run range!"<<std::endl;
      return;
    }

    TH1F all_runs("all_runs","all_runs",1000,0,0);
    Float_t x = 0.0 , xerr = 0.0;
    for (size_t i = 0; i < row_size; ++i){
      if(measurement_type =="a" || measurement_type =="aeo" 
	 || measurement_type =="a12" || measurement_type =="d" 
	 || measurement_type =="deo" || measurement_type =="d12"){
	if(plot.Contains("RMS") == 1){
	  x    = (read_data[i]["rms"])*1e6; // convert to  ppm/ nm
	  xerr = 0.0;
	}
	else {
	  x    = (read_data[i]["value"])*1e6; // convert to  ppm/ nm
	  xerr = (read_data[i]["error"])*1e6; // convert to ppm/ nm
	}
      } 
      else{
	if(plot.Contains("RMS") == 1){
	  x    = (read_data[i]["rms"]);
	  xerr = 0.0;
	}
	else {
	  x    = read_data[i]["value"];
	  xerr = read_data[i]["error"];
	}
      }
      all_runs.Fill(x);
    }

    THStack *hs = new THStack("hs","");
    TH1F *h_in_L = new TH1F("h_in_L","",1000,-10,10);
    TH1F *h_in_R = new TH1F("h_in_R","",1000,-10,10);
    TH1F *h_out_L = new TH1F("h_out_L","",1000,-10,10);
    TH1F *h_out_R = new TH1F("h_out_R","",1000,-10,10);
    TH1F *h_suspect = new TH1F("h_suspect","",1000,-10,10);
    TH1F *h_bad = new TH1F("h_bad","",1000,-10,10);

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
    for (size_t i = 0; i < row_size; ++i)
      { 	    
	if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
	  if(plot.Contains("RMS") == 1){
 	    x    = (read_data[i]["rms"])*1e6; // convert to  ppm/ nm
	  }
	  else {
	    x    = (read_data[i]["value"])*1e6; // convert to  ppm/ nm
	  }
	} 
	else{
	  if(plot.Contains("RMS") == 1){
 	    x    = (read_data[i]["rms"]);
	  }
	  else {
	    x    = read_data[i]["value"];
	  }
	}
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
      std::cout<<"QwGUI : Moving on to draw the graph"<<std::endl;
    

//GROUP_IN_R
    h_in_R ->SetMarkerSize(1.0);
    h_in_R ->SetMarkerStyle(21);
    h_in_R ->SetMarkerColor(kBlue);

//GROUP_IN_L

	h_in_L ->SetMarkerSize(1.0);
	h_in_L ->SetMarkerStyle(21);
	h_in_L ->SetMarkerColor(kOrange-2);
    

//GROUP_OUT_R
	h_out_R ->SetMarkerSize(1.0);
	h_out_R ->SetMarkerStyle(21);
	h_out_R ->SetMarkerColor(kRed);

//GROUP_OUT_L
	h_out_L ->SetMarkerSize(1.0);
	h_out_L ->SetMarkerStyle(21);
	h_out_L ->SetMarkerColor(kViolet);

//GROUP_BAD
	h_bad ->SetMarkerSize(1.0);
	h_bad ->SetMarkerStyle(29);
	h_bad ->SetMarkerColor(kBlack);

//GROUP_SUSPECT
	h_suspect ->SetMarkerSize(1.0);
	h_suspect ->SetMarkerStyle(29);
	h_suspect ->SetMarkerColor(kGreen);



	TString y_title = GetYTitle(measurement_type, det_id);
	TString title   = GetTitle(measurement_type, device);

    if(plot.Contains("RMS"))
      {
	y_title = "RMS of "+y_title;
	title   = "RMS of "+title;
      }


    if(m>0){
      hs->Add(h_in_R);
      h_in_R ->Fit("gaus");
      fit1 = h_in_R->GetFunction("gaus");
      fit1 -> SetLineColor(kBlue);
    }
    if(k>0){
      hs->Add(h_out_R);
      h_out_R ->Fit("gaus");
      fit2 = h_out_R->GetFunction("gaus");
      fit2 -> SetLineColor(kRed);
    }
    if(n>0){
      hs->Add(h_in_L);
      h_in_L ->Fit("gaus");
      fit3 = h_in_L->GetFunction("gaus");
      fit3 -> SetLineColor(kOrange-2);
   
    }
    if(l>0){
      hs->Add(h_out_L);
      h_out_L ->Fit("gaus");
      fit4 = h_out_L->GetFunction("gaus");
      fit4 -> SetLineColor(kViolet);
    }
    if(o>0){
      hs->Add(h_bad);
      h_bad ->Fit("gaus");
      fit5 = h_bad->GetFunction("gaus");
      fit5 -> SetLineColor(kBlack);
    }
    if(p>0){
      hs->Add(h_suspect);
      h_suspect ->Fit("gaus");
      fit6 = h_suspect->GetFunction("gaus");
      fit6 -> SetLineColor(kGreen);
    }


    TLegend *legend = new TLegend(0.80,0.80,0.99,0.99,"","brNDC");
    if(m>0) legend->AddEntry(h_in_R, Form("<IN_R>  = %2.3f #pm %2.3f", 
				  fit1->GetParameter(1), fit1->GetParError(1)), "p");
    if(k>0) legend->AddEntry(h_out_R,Form("<OUT_R> = %2.3f #pm %2.3f", 
				  fit2->GetParameter(1), fit2->GetParError(1)), "p");
    if(n>0) legend->AddEntry(h_in_L, Form("<IN_L>  = %2.3f #pm %2.3f", 
				  fit3->GetParameter(1), fit3->GetParError(1)), "p");
    if(l>0) legend->AddEntry(h_out_L,Form("<OUT_L> = %2.3f #pm %2.3f", 
				  fit4->GetParameter(1), fit4->GetParError(1)), "p");
    if(o>0) legend->AddEntry(h_bad,Form("<BAD> = %2.3f #pm %2.3f", 
				  fit5->GetParameter(1), fit5->GetParError(1)), "p");
    if(p>0) legend->AddEntry(h_suspect,Form("<SUSPECT> = %2.3f #pm %2.3f", 
				  fit6->GetParameter(1), fit6->GetParError(1)), "p");


    legend->SetFillColor(0);
    
    mc->cd();
    hs->Draw("nostack,ap");
    if(m>0)fit1->Draw("same");
    if(k>0)fit2->Draw("same");

    legend->Draw("");
    
    
    hs->SetTitle(title);
    hs->GetYaxis()->SetTitleOffset(2);
    hs->GetYaxis()->SetTitle("Runlets");
    hs->GetXaxis()->SetTitle(y_title);
    hs->GetXaxis()->SetTitleOffset(2);
    hs->GetYaxis()->SetTitleSize(0.03);
    hs->GetXaxis()->SetTitleSize(0.03);
    hs->GetYaxis()->CenterTitle();
    
    
    mc->Modified();
    mc->SetBorderMode(0);
    mc->Update();
    
    std::cout<<"QwGUI : Done!"<<std::endl;
    
    
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

  if (measurement_type == "d" || measurement_type == "deo" || measurement_type == "d12" )
    if (det_id == ID_E_CAL) ytitle  = "Energy Asymetry (ppm)";
    else
      ytitle = "Beam Position Differences (nm)";

  
  if (measurement_type == "yq"){
    if (det_id == ID_BCM || det_id == ID_CMB_BCM )  ytitle  = "Current (#muA)";
    if (det_id == ID_BPM || det_id == ID_CMB_BPM ) ytitle  = "Beam Position (mm)";
  }

  if (measurement_type == "yp")
    if (det_id == ID_E_CAL) ytitle  = "Relative Momentum Change dP/P";
    else 
      ytitle = "Beam Position (mm)";
  
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
      xaxis = "vs Time";
    case ID_X_SLUG:
      xaxis = "vs Slug";
    case ID_X_HISTO:
      xaxis = "";
      break;
    default:
      break;
    }
  if (measurement_type == "y") 
    title = Form("%s Yield ",device.Data());
  if (measurement_type == "a" || measurement_type == "aeo" || measurement_type == "a12" )
    title = Form("%s Asymmetry ",device.Data());
  if (measurement_type == "d" || measurement_type == "deo" || measurement_type == "d12" )
    title = Form("%s Beam Position Difference vs Runlet Number",device.Data());
  if (measurement_type == "yq")
    title = Form("%s Current ",device.Data());
  if (measurement_type == "yp")
    title = Form("%s Beam Position ",device.Data());

  title+=xaxis;
  
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
  target            = Targets[dCmbTargetType->GetSelected()];
  subblock          = dCmbSubblock -> GetSelected();
  plot              = Plots[dCmbPlotType->GetSelected()];
  x_axis            = dCmbXAxis->GetSelected();
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
    property = RegressionVars[dCmbProperty->GetSelected()];
    break;
  case ID_LUMI_SENS:
    detector = LumiCombos[dCmbDetector->GetSelected()];
    property = RegressionVars[dCmbProperty->GetSelected()];
    break;
  default:
    break;
  }
  
  switch (dCmbXAxis->GetSelected()) 
    {
    case ID_X_RUN:
    case ID_X_SLUG:
    case ID_X_TIME:
      PlotDetector();
      break;
    case ID_X_BEAM:
      DetectorVsMonitorPlot();
      break;
    case ID_X_HISTO:
      HistogramDetector();
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



