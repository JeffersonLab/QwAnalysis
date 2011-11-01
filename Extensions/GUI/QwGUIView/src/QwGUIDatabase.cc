#include "QwGUIDatabase.h"

#include <TG3DLine.h>
#include "TGaxis.h"
#include "TGTextEntry.h"
#include "TStopwatch.h"
#include "TLatex.h"

ClassImp(QwGUIDatabase);

QwGUIGoodForSettings::QwGUIGoodForSettings(){
  fGoodForLabels.push_back("production");
  fGoodForLabels.push_back("commissioning");
  fGoodForLabels.push_back("parity");
  fGoodForLabels.push_back("tracking");
  fGoodForLabels.push_back("centering_target");
  fGoodForLabels.push_back("centering_plug");
  fGoodForLabels.push_back("pedestals");
  fGoodForLabels.push_back("transverse");
  fGoodForIDs.resize(fGoodForLabels.size());
  fGoodForReject.resize(fGoodForLabels.size());
  for (size_t i=0; i< fGoodForLabels.size(); i++){
    fGoodForIDs.at(i)    = i+1;
    fGoodForReject.at(i) = kTRUE;
  }
  fGoodForReject.at(0) = kFALSE;
  fGoodForReject.at(2) = kFALSE;
  fGoodForRejectNULLs = kTRUE;

  fQualityLabels.push_back("good");
  fQualityLabels.push_back("bad");
  fQualityLabels.push_back("suspect");
  fQualityIDs.resize(fQualityLabels.size());
  fQualityReject.resize(fQualityLabels.size());
  for (size_t i=0; i<fQualityLabels.size(); i++){
    fQualityIDs.at(i)    = i+1;
    fQualityReject.at(i) = kTRUE;
  }
  fQualityReject.at(0) = kFALSE;
  fQualityRejectNULLs = kTRUE;
}

std::string QwGUIGoodForSettings::GetSelectionString(std::string table){
  std::string selection;
  std::string prefix;
  if (!table.empty()){
    prefix = table + ".";
  } else {
    prefix = "";
  }
  Bool_t firstentry;
  firstentry = kTRUE;
  if (fGoodForRejectNULLs){
    if (!firstentry) selection += " AND";
    firstentry = kFALSE;
    selection += " " + prefix + "good_for_id is not NULL";
  }
  for (size_t i=0; i<fGoodForIDs.size(); i++){
    if (fGoodForReject.at(i)){
      if (!firstentry) selection += " AND";
      firstentry = kFALSE;
      selection += " " + prefix + "good_for_id not like ";
      selection += Form("\"%%%d%%\"",fGoodForIDs.at(i));
    }
  }
  if (fQualityRejectNULLs){
    if (!firstentry) selection += " AND";
    firstentry = kFALSE;
    selection += " " + prefix + "run_quality_id is not NULL";
  }
  for (size_t i=0; i<fQualityIDs.size(); i++){
    if (fQualityReject.at(i)){
      if (!firstentry) selection += " AND";
      firstentry = kFALSE;
      selection += " " + prefix + "run_quality_id not like ";
      selection += Form("\"%%%d%%\"",fQualityIDs.at(i));
    }
  }
  return selection;
}

TString detector;
TString property;

// Parameters to plot in X axis
enum EQwGUIDatabaseXAxisIDs {
  ID_X_HISTO,
  ID_X_RUN,
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
  CMB_PLOT
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
    "qwk_md1_qwk_md5",
    "qwk_md2_qwk_md6",
    "qwk_md3_qwk_md7",
    "qwk_md4_qwk_md8",
    "qwk_mdoddbars",
    "qwk_mdevenbars",
    "qwk_mdallbars"
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
  "qwk_bcm1","qwk_bcm2","qwk_bcm5","qwk_bcm6","qwk_linephase", "qwk_invert_tstable"
 
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
    "yq","a","a12", "aeo", "r", 
  };

const char *QwGUIDatabase::PositionMeasurementTypes[N_POS_MEAS_TYPES]=
  {
    "yp", "d", "d12", "deo","r"
  };

const char *QwGUIDatabase::DetectorMeasurementTypes[N_DET_MEAS_TYPES]=
  {
    "y", "a", "a12","aeo","r"
  };

const char *QwGUIDatabase::OtherMeasurementTypes[N_MEAS_TYPES]=
  {
    "ya", "ym", "ye","p"
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


const char *QwGUIDatabase::Plots[2] =
  {
    "Mean", "RMS"
  }; 


QwGUIDatabase::QwGUIDatabase(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dCmbLayout          = NULL;
  dNumLayout          = NULL;
  dBtnLayout          = NULL;
  dLabLayout          = NULL;
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
  dLabStartRun        = NULL;
  dLabStopRun         = NULL;
  dLabTarget          = NULL;
  dLabRegression      = NULL;
  dLabPlot            = NULL;

  GraphArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);

}

QwGUIDatabase::~QwGUIDatabase()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dCmbLayout)          delete dCmbLayout;
  if(dNumLayout)          delete dNumLayout;
  if(dLabLayout)          delete dLabLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dLabStartRun)        delete dLabStartRun;
  if(dLabStopRun)         delete dLabStopRun;
  if(dNumStartRun)        delete dNumStartRun;
  if(dLabTarget)          delete dLabTarget;
  if(dLabRegression)      delete dLabRegression;
  if(dLabPlot)            delete dLabPlot;
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
  //gStyle->SetTitleColor(1);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.03,"x");
  gStyle->SetLabelSize(0.03,"y");


  dTabFrame = new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator  = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas             = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  dCmbInstrument      = new TGComboBox(dControlsFrame, CMB_INSTRUMENT);
  dCmbXAxis           = new TGComboBox(dControlsFrame, CMB_XAXIS);
  dCmbMeasurementType = new TGComboBox(dControlsFrame, CMB_MEASUREMENT_TYPE);
  dCmbDetector        = new TGComboBox(dControlsFrame, CMB_DETECTOR);
  dCmbProperty        = new TGComboBox(dControlsFrame, CMB_PROPERTY);
  dCmbSubblock        = new TGComboBox(dControlsFrame, CMB_SUBBLOCK);
  dLabTarget          = new TGLabel(dControlsFrame, "Target");
  dCmbTargetType      = new TGComboBox(dControlsFrame, CMB_TGT);
  dLabRegression      = new TGLabel(dControlsFrame, "Regression Correction");
  dCmbRegressionType  = new TGComboBox(dControlsFrame, CMB_REGRESS);
  dLabPlot            = new TGLabel(dControlsFrame, "Plot Type");
  dCmbPlotType        = new TGComboBox(dControlsFrame, CMB_PLOT);
  dLabStartRun        = new TGLabel(dControlsFrame, "First Run");
  dNumStartRun        = new TGNumberEntry(dControlsFrame, 0, 5, NUM_START_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dLabStopRun         = new TGLabel(dControlsFrame, "Last Run");
  dNumStopRun         = new TGNumberEntry(dControlsFrame, 12200, 5, NUM_STOP_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dBtnSubmit          = new TGTextButton(dControlsFrame, "&Submit", BTN_SUBMIT);  //dSlowControls       = new TGButtonGroup(dControlsFrame, "Slow Controls");


  dLabLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dNumLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 10, 10, 5, 5 );
  dBtnLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 5, 5 );

  dCmbXAxis->AddEntry("Histogram", ID_X_HISTO);
  dCmbXAxis->AddEntry("Vs. Run Number", ID_X_RUN);
  dCmbXAxis->AddEntry("Vs. BCM", ID_BCM);
  dCmbXAxis->Select(dCmbXAxis->FindEntry("Vs. Run Number")->EntryId());
  dCmbXAxis->Connect("Selected(Int_t)","QwGUIDatabase", this, "PopulateXDetComboBox()");

  dCmbInstrument->AddEntry("Main Detectors", ID_MD);
  dCmbInstrument->AddEntry("BPMs", ID_BPM);
  dCmbInstrument->AddEntry("BCMs", ID_BCM);
  dCmbInstrument->AddEntry("LUMI Detectors", ID_LUMI);
  dCmbInstrument->AddEntry("Combined BPMs", ID_CMB_BPM);
  dCmbInstrument->AddEntry("Combined BCMs", ID_CMB_BCM);
  dCmbInstrument->AddEntry("Energy Calculator", ID_E_CAL);
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

  // Populate regression combo box
  dCmbRegressionType->AddEntry("off", 0);
  dCmbRegressionType->AddEntry("on", 1);
  
  // Populate plot type
  for (Int_t i = 0; i < 2; i++) {
    dCmbPlotType->AddEntry(Plots[i], i);
  }

  dCmbSubblock->Select(0);
  dCmbTargetType->Select(1);
  dCmbRegressionType->Select(0);
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


  dControlsFrame      -> AddFrame(dCmbInstrument, dCmbLayout ); // detector type
  dControlsFrame      -> AddFrame(dCmbDetector, dCmbLayout );   // detector name
  dControlsFrame      -> AddFrame(dCmbProperty, dCmbLayout );   // detector property
  dControlsFrame      -> AddFrame(dCmbMeasurementType, dCmbLayout ); // measurement type
  dControlsFrame      -> AddFrame(dCmbSubblock, dCmbLayout );   // block type
  dControlsFrame      -> AddFrame(dCmbXAxis, dCmbLayout );
  dControlsFrame      -> AddFrame(dLabTarget, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbTargetType, dCmbLayout );
  dControlsFrame      -> AddFrame(dLabRegression, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbRegressionType, dCmbLayout );
  dControlsFrame      -> AddFrame(dLabStartRun, dLabLayout );
  dControlsFrame      -> AddFrame(dNumStartRun, dNumLayout );
  dControlsFrame      -> AddFrame(dLabStopRun, dLabLayout );
  dControlsFrame      -> AddFrame(dNumStopRun, dNumLayout );
  dControlsFrame      -> AddFrame(dLabPlot, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbPlotType, dCmbLayout );
  dControlsFrame      -> AddFrame(dBtnSubmit, dBtnLayout);

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
  dBtnSubmit          -> Associate(this);

  dCanvas -> GetCanvas() -> SetBorderMode(0);
  dCanvas -> GetCanvas() -> Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "QwGUIDatabase", this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  mc->Initialize();
  dCanvas->AdoptCanvas(mc);

}


/********************************************

Populate the combo boxes based on the detector selection

****************************************** */
void QwGUIDatabase::PopulateDetectorComboBox()
{
  dCmbDetector->RemoveAll();
  dCmbProperty->RemoveAll();
  dCmbMeasurementType->RemoveAll();
  measurements.clear();
  dCmbProperty->SetEnabled(kFALSE);

  // Main detector :default qwk_mdallbar
  if (dCmbInstrument->GetSelected() == ID_MD) {
    for (Int_t i = 0; i < N_DETECTORS; i++) {
      dCmbDetector->AddEntry(DetectorCombos[i], i);
    }
    for (Int_t i = 0; i < N_DET_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(DetectorMeasurementTypes[i], i);
      measurements.push_back(DetectorMeasurementTypes[i]);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_mdallbars")->EntryId());
  }
  
  // Lumis : default qwk_uslumi1neg
  if (dCmbInstrument->GetSelected() == ID_LUMI) {
    for (Int_t i = 0; i < N_LUMIS; i++) {
      dCmbDetector->AddEntry(LumiCombos[i], i);
    }
    for (Int_t i = 0; i < N_DET_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(DetectorMeasurementTypes[i], i);
      measurements.push_back(DetectorMeasurementTypes[i]);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_uslumi1neg")->EntryId());
  }

  // BPMs : default qwk_bpm3c07
  if (dCmbInstrument->GetSelected() == ID_BPM) {
    dCmbProperty->SetEnabled(kTRUE);
    
    for (Int_t i = 0; i < N_BPMS; i++) {
      dCmbDetector->AddEntry(BeamPositionMonitors[i], i);
    }
    for (Int_t i = 0; i < N_BPM_READ; i++) {
      dCmbProperty->AddEntry(BPMReadings[i], i);
    }

    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_bpm3c07")->EntryId());
    dCmbProperty->Select(dCmbProperty->FindEntry("X")->EntryId());
  }

  //BCMs : default qwk_bcm1
  if (dCmbInstrument->GetSelected() == ID_BCM) {
    for (Int_t i = 0; i < N_BCMS; i++) {
      dCmbDetector->AddEntry(BeamCurrentMonitors[i], i);
    }
    for (Int_t i = 0; i < N_Q_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[i], i);
      measurements.push_back(ChargeMeasurementTypes[i]);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_bcm1")->EntryId());
  }

  // combined BPMs : default is qwk_target
  if (dCmbInstrument->GetSelected() == ID_CMB_BPM) {
    dCmbProperty->SetEnabled(kTRUE);
    for (Int_t i = 0; i < N_CMB_BPMS; i++) {
      dCmbDetector->AddEntry(CombinedBPMS[i], i);
    }
    for (Int_t i = 0; i < N_CMB_READ; i++) {
      dCmbProperty->AddEntry(ComboBPMReadings[i], i);
    }

    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_target")->EntryId());
    dCmbProperty->Select(dCmbProperty->FindEntry("X")->EntryId());

  }

  //Combined BCMs : default qwk_charge
  if (dCmbInstrument->GetSelected() == ID_CMB_BCM) {
    for (Int_t i = 0; i < N_CMB_BCMS; i++) {
      dCmbDetector->AddEntry(CombinedBCMS[i],i);
    }
    for (Int_t i = 0; i < N_Q_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[i],i);
      measurements.push_back(ChargeMeasurementTypes[i]);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_charge")->EntryId());
  }

  //energy calculator : default qwk_energy
  if (dCmbInstrument->GetSelected() == ID_E_CAL) {
    for (Int_t i = 0; i < N_ENERGY; i++) {
      dCmbDetector->AddEntry(EnergyCalculators[i], i);
    }
    for (Int_t i = 0; i < N_Q_MEAS_TYPES; i++) {
      dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[i], i);
      measurements.push_back(ChargeMeasurementTypes[i]);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("qwk_energy")->EntryId());
  }

  dCmbMeasurementType->Select(0);

}


/********************************************

Populate the combo boxes based on the detector selection 

****************************************** */
void QwGUIDatabase::PopulateMeasurementComboBox()
{
  dCmbMeasurementType->RemoveAll();
  measurements.clear();
  // Main detector 
  if (dCmbInstrument->GetSelected() == ID_MD){
    // do nothing. measurement were already filled in PopulateDetectroCombobox()
  }

  // Lumi detector 
  if (dCmbInstrument->GetSelected() == ID_LUMI){
    // do nothing. measurement were already filled in PopulateDetectroCombobox()
  }

  // BCMs detector 
  if (dCmbInstrument->GetSelected() == ID_BCM){
    // do nothing. measurement were already filled in PopulateDetectroCombobox()
  }

  // EnergyCalculator
  if (dCmbInstrument->GetSelected() == ID_E_CAL){
    // do nothing. measurement were already filled in PopulateDetectroCombobox()
  }

  // Combined BCMs detector 
  if (dCmbInstrument->GetSelected() == ID_CMB_BCM){
    // do nothing. measurement were already filled in PopulateDetectroCombobox()
  }

  // BPMs
  if (dCmbInstrument->GetSelected() == ID_BPM){
    if(dCmbProperty->GetSelected() == 4){ // effective charge in BPMReadings[]
      for (Int_t k = 0; k < N_Q_MEAS_TYPES; k++){
	dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[k], k);
	measurements.push_back(ChargeMeasurementTypes[k]);
      } 
    }
    else{ // for X, Y, RelX, RelY
      for (Int_t k = 0; k < N_POS_MEAS_TYPES; k++){
	dCmbMeasurementType->AddEntry(PositionMeasurementTypes[k], k);
	measurements.push_back(PositionMeasurementTypes[k]);
      }
    }
  }

  // Combined BPMs
  if (dCmbInstrument->GetSelected() == ID_CMB_BPM){
    if(dCmbProperty->GetSelected() == 6){ // effective charge in ComboBPMReadings[]
      for (Int_t k = 0; k < N_Q_MEAS_TYPES; k++) {
	dCmbMeasurementType->AddEntry(ChargeMeasurementTypes[k], k);
	measurements.push_back(ChargeMeasurementTypes[k]);
      }
    } 
    else {
      for (Int_t k = 0; k < N_POS_MEAS_TYPES; k++) {
	dCmbMeasurementType->AddEntry(PositionMeasurementTypes[k], k);
	measurements.push_back(PositionMeasurementTypes[k]);
      }
    }    
  }
    dCmbMeasurementType->Select(1);

}

/********************************************

Populate the X axis detector combo box

****************************************** */

void QwGUIDatabase::PopulateXDetComboBox()
{
//   dCmbXDet->RemoveAll();

//   if(dCmbXAxis->GetSelected() == ID_X_RUN)
//     dCmbXDet  -> SetEnabled(kFALSE);
//   else
//     dCmbXDet  -> SetEnabled(kTRUE);

//   // Main detector 
//   if (dCmbXAxis->GetSelected() == ID_BCM){
//     for (Int_t i = 0; i < N_BCMS; i++) {
//       dCmbXDet->AddEntry(BeamCurrentMonitors[i], i);
//     }
//   }
//     dCmbXDet->Select(1);

}



void QwGUIDatabase::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIDatabase::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIDatabase::OnObjClose\n");

    dROOTCont = NULL;
  }
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
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){
//    mc->cd(ind);
    gPad->SetLogy(0);
    ((TGraph*)obj)->Draw("ap");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

}

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
}

//Stuff to do after the tab is selected
void QwGUIDatabase::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  /* DTS
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= HCLINE_DEV_NUM)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIDatabase",
						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
	if(!dMiscWindow){
	  return;
	}
	DataWindowArray.Add(dMiscWindow);
	dMiscWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
	dMiscWindow->DrawData(*((TH1D*)HistArray[pad-1]));
	SetLogMessage(Form("Looking at %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);

	return;
      }
  }
  */
}


/******************************************

Plot detector data with beam parameters (still not finished)

*******************************************/

void QwGUIDatabase::DetectorVsMonitorPlot()
{
  TGraphErrors *grp;

  if(dDatabaseCont){
    dDatabaseCont->Connect();

    mysqlpp::Query query = dDatabaseCont->Query();

    // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
    Int_t  subblock  = dCmbSubblock->GetSelected();
    Int_t  run_first = dNumStartRun->GetIntNumber();
    Int_t  run_last  = dNumStopRun->GetIntNumber();
    string detector;
    string measurement_type = DetectorMeasurementTypes[dCmbMeasurementType->GetSelected()];

    query << "SELECT xt.run_number AS run, xt.segment_number AS segment, ";
    query << "xt.q_value AS q, xt.q_error AS q_err, xt.energy_value AS e, xt.energy_error AS e_err, ";
    query << "xt.x_value AS x, xt.x_error AS x_err, xt.theta_x_value AS theta_x, xt.theta_x_error AS theta_x_err, ";
    query << "xt.y_value AS y, xt.y_error AS y_err, xt.theta_y_value AS theta_y, xt.theta_y_error AS theta_y_err, ";
    query << "yt.value AS value, yt.error AS error ";
    query << "FROM summary_b" << measurement_type << " AS xt, ";
    if (dCmbInstrument->GetSelected() == ID_MD) {
      query << "summary_d";
      detector = DetectorCombos[dCmbDetector->GetSelected()];
    }
    if (dCmbInstrument->GetSelected() == ID_LUMI) {
      detector = LumiCombos[dCmbDetector->GetSelected()];
      query << "summary_l"; 
    }
    else{
      std::cerr << "Beam monitors not a valid selection in this context." << std::endl;
      return;
    }
    query << measurement_type << "_calc AS yt ";
    query << "WHERE ";
    query << "xt.run_number = yt.run_number ";
    query << "AND xt.segment_number = yt.segment_number ";
    query << "AND xt.subblock = yt.subblock ";
    query << "AND yt.detector = " << mysqlpp::quote << detector << " ";
    query << "AND xt.subblock = " << subblock << " ";
    query << "AND xt.run_number BETWEEN ";
    query << run_first << " AND ";
    query << run_last << " ";
    query << "ORDER BY xt.run_number, xt.segment_number";

    std::cout << query.str() << std::endl;

    mysqlpp::StoreQueryResult res = query.store();

    dDatabaseCont->Disconnect(); 

    Int_t res_size = 0;
      res_size = res.num_rows();
      std::cout << "Number of rows:  " << res_size << std::endl;
    TVectorF q(res_size), qerr(res_size), e(res_size), eerr(res_size);
    TVectorF x(res_size), xerr(res_size), y(res_size), yerr(res_size);
    TVectorF theta_x(res_size), theta_xerr(res_size), theta_y(res_size), theta_yerr(res_size);
    TVectorF d(res_size), derr(res_size);

    if (res) {
      /*
      q.ResizeTo(res_size);
      qerr.ResizeTo(res_size);
      e.ResizeTo(res_size);
      eerr.ResizeTo(res_size);
      x.ResizeTo(res_size);
      xerr.ResizeTo(res_size);
      theta_x.ResizeTo(res_size);
      theta_xerr.ResizeTo(res_size);
      y.ResizeTo(res_size);
      yerr.ResizeTo(res_size);
      theta_y.ResizeTo(res_size);
      theta_yerr.ResizeTo(res_size);
      */

      for (Int_t i = 0; i < res_size; i++) {
        q[i] = res[i]["q"];
        qerr[i] = res[i]["q_err"];
        e[i] = res[i]["e"];
        eerr[i] = res[i]["e_err"];
        x[i] = res[i]["x"];
        xerr[i] = res[i]["x_err"];
        theta_x[i] = res[i]["theta_x"];
        theta_xerr[i] = res[i]["theta_x_err"];
        y[i] = res[i]["y"];
        yerr[i] = res[i]["y_err"];
        theta_y[i] = res[i]["theta_y"];
        theta_yerr[i] = res[i]["theta_y_err"];
        d[i] = res[i]["value"];
        derr[i] = res[i]["error"];
      }

    //
    // Construct Graphs for Plotting
    //

    string grp_title;

    for (Int_t i = 0; i < N_BPMS; i++) {
      switch (i) {
        case 0: // X position @ target
          grp = new TGraphErrors(x, d, xerr, derr);
          grp_title = "Detector vs. X";
          if (measurement_type == "y") {
	          grp->GetXaxis()->SetTitle("Position []");
          }
          if (measurement_type == "a") {
	          grp->GetXaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 1: // Y position @ target
          grp = new TGraphErrors(y, d, yerr, derr);
          grp_title = "Detector vs. Y";
          if (measurement_type == "y") {
	          grp->GetXaxis()->SetTitle("Position []");
          }
          if (measurement_type == "a") {
	          grp->GetXaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 2: // X angle @ target
          grp = new TGraphErrors(theta_x, d, theta_xerr, derr);
          grp_title = "Detector vs. X Angle";
          if (measurement_type == "y") {
            grp->GetXaxis()->SetTitle("Angle []");
          }
          if (measurement_type == "a") {
            grp->GetXaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 3: // Y angle @ target
          grp = new TGraphErrors(theta_y, d, theta_yerr, derr);
          grp_title = "Detector vs. Y Angle";
          if (measurement_type == "y") {
	          grp->GetXaxis()->SetTitle("Angle []");
          }
          if (measurement_type == "a") {
	          grp->GetXaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 4: // Charge @ target
          grp = new TGraphErrors(q, d, qerr, derr);
          grp_title = "Detector vs. Charge";
          if (measurement_type == "y") {
	          grp->GetXaxis()->SetTitle("Charge []");
          }
          if (measurement_type == "a") {
	          grp->GetXaxis()->SetTitle("Asymmetry []");
            grp_title += " Asymmetry";
          }
          break;
        case 5: // Energy @ target
          grp = new TGraphErrors(e, d, eerr, derr);
          grp_title = "Detector vs. Energy";
          if (measurement_type == "y") {
	          grp->GetXaxis()->SetTitle("Energy []");
          }
          if (measurement_type == "a") {
	          grp->GetXaxis()->SetTitle("Asymmetry? []");
            grp_title += " Asymmetry?";
          }
          break;
        default:
          break;
      }

      grp->SetTitle(grp_title.c_str());

      grp->GetXaxis()->CenterTitle();
  	  grp->GetXaxis()->SetTitleSize(0.03);
  	  grp->GetXaxis()->SetTitleOffset(1.25);
      grp->GetYaxis()->SetTitle("Detector []");
   	  grp->GetYaxis()->CenterTitle();
	    grp->GetYaxis()->SetTitleSize(0.03);
	    grp->GetYaxis()->SetTitleOffset(1.5);

      GraphArray.Add(grp);

    } 
    //
    //
    // Plot Graphs
    //
    //
    Int_t ind = 1;
    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->Divide(2,3);

    TObject *obj;
    TIter next(GraphArray.MakeIterator());
    obj = next();
    while(obj){
      mc->cd(ind);
      gPad->SetLogy(0);
      ((TGraph*)obj)->Draw("ap*");
      ind++;
      obj = next();
    }

    mc->Modified();
    mc->Update();


    }
  }
} //DetectorVsMonitorPlot



mysqlpp::StoreQueryResult  QwGUIDatabase::QueryDetector(TString detector, TString measured_property, Int_t det_id)
{
  Bool_t ldebug = kTRUE;

  dDatabaseCont->Connect();

  mysqlpp::Query query1     = dDatabaseCont-> Query();
  mysqlpp::Query query2     = dDatabaseCont-> Query();

  // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
  Int_t   subblock          = dCmbSubblock -> GetSelected();
  Int_t   run_first         = dNumStartRun -> GetIntNumber();
  Int_t   run_last          = dNumStopRun  -> GetIntNumber();
  TString measurement_type  = measurements[dCmbMeasurementType->GetSelected()];
  TString device            = Form("%s%s",detector.Data(),measured_property.Data());
  TString target            = Targets[dCmbTargetType->GetSelected()];
  TString plot              = Plots[dCmbPlotType->GetSelected()];
  
  TString det_table;
  TString data_table;
  TString det_table_id;
  Int_t   monitor_id = 0;
  Int_t   phi_fg_id = 0;
  
  std::string correction_flag;
  if (dCmbRegressionType->GetSelected()==1)
    correction_flag = "on";
  else
    correction_flag = "off";
    
  // Set the correct tables to read the data and inof from.
  switch (det_id){
  case ID_MD:
    det_table    = "main_detector";
    data_table   = "md_data";
    det_table_id = "main_detector_id";
    break;
  case ID_LUMI:
    det_table    = "lumi_detector";
    data_table   = "lumi_data";
    det_table_id = "lumi_detector_id";
    break;
  case ID_BPM:
  case ID_CMB_BPM:
  case ID_BCM:
  case ID_CMB_BCM:
  case ID_E_CAL:
    det_table    = "monitor";
    data_table   = "beam";
    det_table_id = "monitor_id";
    break;
  default:
    break;
  }


  // Get the monitor ID from the detector table
  // For this we need the detector name: detector+property
    
  if(ldebug) std::cout<< "SELECT * FROM "<<det_table
		      <<" WHERE quantity = '"<<device<<"';"<<std::endl;
  query1 << "SELECT * FROM "<<det_table<<" WHERE quantity = '"<<device<<"';";
  mysqlpp::StoreQueryResult read_data1 = query1.store();

  //check for empty queries. If empty exit with error.
  if( read_data1.num_rows() == 0){
    std::cout<<"There is no device called "<<device<<" in "<<det_table
	     <<" in the given run range!"<<std::endl;
    return read_data1;
  }

  monitor_id = read_data1[0][det_table_id]; 
  //    if(ldebug)  std::cout<< "SELECT "<<det_table_id<<" FROM "<<det_table<<" WHERE quantity = "<<monitor_id<<";";
  

  if(ldebug) std::cout<< "SELECT * FROM sc_detector "
		      <<" WHERE name = 'Phi_FG';"<<std::endl;
  query2 << "SELECT * FROM sc_detector WHERE name = 'Phi_FG';";
  mysqlpp::StoreQueryResult read_data2 = query2.store();
  
  //check for empty queries. If empty exit with error.
  if( read_data2.num_rows() == 0){
    std::cout<<"There is no device called Phi_FG in sc_detector in the given run range!"<<std::endl;
    return read_data2;
  }
  phi_fg_id = read_data2[0]["sc_detector_id"]; 


  // Now get the run number information
  mysqlpp::Query query      = dDatabaseCont->Query();

  TString querystring= 
    " SELECT "+det_table_id+", data.value as value, data.error as error, data.error*sqrt(data.n) as rms, run_number, segment_number, slow_helicity_plate,target_position, (scd.value<0)*-2+1 as wien_reversal FROM "
    + data_table+" as data, analysis_view as ana, slow_controls_settings , slow_controls_data as scd   "
    + " WHERE "
    + fGoodForSelection.GetSelectionString() + " AND "
    + Form(" slope_correction='%s' AND ",correction_flag.c_str())
    + " data.analysis_id = ana.analysis_id AND"
    + " ana.runlet_id = slow_controls_settings.runlet_id AND "
    + " data."+ det_table_id +" = "+Form("%i", monitor_id) +" AND "
    + " data.subblock = "+ Form("%i",subblock) +" AND "
    + " measurement_type_id = '"+measurement_type+"' AND "
    + " run_number > "+ Form("%i",run_first) + " AND "
    + " run_number < "+ Form("%i",run_last)+ " AND "
    + " data.error !=0  AND"
    +"  target_position = '"+Form("%s",target.Data())+"'" 
    +" and scd.runlet_id=ana.runlet_id and scd.sc_detector_id=" 
    + Form("%i", phi_fg_id)
    + " ORDER BY run_number, segment_number;";

   
  if(ldebug) std::cout<<"QUERYSTING="
		      <<querystring<<std::endl;

  query << querystring;
  
  
  mysqlpp::StoreQueryResult read_data = query.store();
  dDatabaseCont->Disconnect(); 
    
  return read_data;
}


/******************************************

Plot detector data vs run number

*******************************************/

void QwGUIDatabase::PlotDetector(TString detector, TString measured_property, Int_t det_id)
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

  gDirectory->Delete();

  if(dDatabaseCont){

    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);

    // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
    Int_t   subblock          = dCmbSubblock -> GetSelected();
    TString measurement_type  = measurements[dCmbMeasurementType->GetSelected()];
    TString device            = Form("%s%s",detector.Data(),measured_property.Data());
    TString target            = Targets[dCmbTargetType->GetSelected()];
    TString plot              = Plots[dCmbPlotType->GetSelected()];

    //
    // Query Database for Data
    //
    mysqlpp::StoreQueryResult read_data = QueryDetector(detector, measured_property, det_id);

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

 
    TVectorF x_in(row_size), xerr_in(row_size);
    TVectorF x_out(row_size), xerr_out(row_size);
    TVectorF run_in(row_size), run_out(row_size);
    TVectorF err_in(row_size), err_out(row_size);

    TVectorF x_in_L(row_size), xerr_in_L(row_size);
    TVectorF x_out_L(row_size), xerr_out_L(row_size);
    TVectorF run_in_L(row_size), run_out_L(row_size);
    TVectorF err_in_L(row_size), err_out_L(row_size);

    x_in.Clear();
    x_in.ResizeTo(row_size);
    xerr_in.Clear();
    xerr_in.ResizeTo(row_size);
    run_in.Clear();
    run_in.ResizeTo(row_size);

    x_out.Clear();
    x_out.ResizeTo(row_size);
    xerr_out.Clear();
    xerr_out.ResizeTo(row_size);
    run_out.Clear();
    run_out.ResizeTo(row_size);

    x_in_L.Clear();
    x_in_L.ResizeTo(row_size);
    xerr_in_L.Clear();
    xerr_in_L.ResizeTo(row_size);
    run_in_L.Clear();
    run_in_L.ResizeTo(row_size);

    x_out_L.Clear();
    x_out_L.ResizeTo(row_size);
    xerr_out_L.Clear();
    xerr_out_L.ResizeTo(row_size);
    run_out_L.Clear();
    run_out_L.ResizeTo(row_size);

    Float_t x = 0.0 , xerr = 0.0;
    Int_t m = 0;
    Int_t k = 0;
    Int_t l = 0;
    Int_t n = 0;

    std::cout<<"############################################\n";
    std::cout<<"QwGUI : Collecting data.."<<std::endl;
    std::cout<<"QwGUI : Retrieved "<<row_size<<" data points\n";
    for (size_t i = 0; i < row_size; ++i)
      { 	    
	if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
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

	if(read_data[i]["slow_helicity_plate"] == "out") {
	  if (read_data[i]["wien_reversal"]*1 == 1){
	    run_out.operator()(k)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_out.operator()(k)    = x;
	    xerr_out.operator()(k) = xerr;
	    err_out.operator()(k)  = 0.0;
	    k++;
	  } else {
	    run_out_L.operator()(l)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_out_L.operator()(l)    = x;
	    xerr_out_L.operator()(l) = xerr;
	    err_out_L.operator()(l)  = 0.0;
	    l++;
	  }
	  
	}
	
	if(read_data[i]["slow_helicity_plate"] == "in") {
	  if (read_data[i]["wien_reversal"]*1 == 1){
	    run_in.operator()(m)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_in.operator()(m)    = x;
	    xerr_in.operator()(m) = xerr;
	    err_in.operator()(m)  = 0.0;
	    m++;
	  } else {
	    run_in_L.operator()(n)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_in_L.operator()(n)    = x;
	    xerr_in_L.operator()(n) = xerr;
	    err_in_L.operator()(n)  = 0.0;
	    n++;
	    
	  }
	}
      }

    // Check to make sure graph is not empty.
    if(m==0 && k==0 && l==0 && n==0){
      std::cout<<"QwGUI : No data were found for the given criteria."<<std::endl;
      exit(1);
    }
    else
      std::cout<<"QwGUI : Moving on to draw the graph"<<std::endl;
    

    run_in.ResizeTo(m);
    x_in.ResizeTo(m);
    err_in.ResizeTo(m);
    xerr_in.ResizeTo(m);

    TGraphErrors* grp_in  = new TGraphErrors(run_in,  x_in, err_in, xerr_in);
    grp_in ->SetMarkerSize(0.6);
    grp_in ->SetMarkerStyle(21);
    grp_in ->SetMarkerColor(kBlue);

    run_in_L.ResizeTo(n);
    x_in_L.ResizeTo(n);
    err_in_L.ResizeTo(n);
    xerr_in_L.ResizeTo(n);

    TGraphErrors* grp_in_L  = new TGraphErrors(run_in_L,  x_in_L, err_in_L, xerr_in_L);
    grp_in_L ->SetMarkerSize(0.6);
    grp_in_L ->SetMarkerStyle(21);
    grp_in_L ->SetMarkerColor(kOrange);

    run_out.ResizeTo(k);
    x_out.ResizeTo(k);
    err_out.ResizeTo(k);
    xerr_out.ResizeTo(k);

    TGraphErrors* grp_out = new TGraphErrors(run_out, x_out, err_out, xerr_out);
    grp_out ->SetMarkerSize(0.6);
    grp_out ->SetMarkerStyle(21);
    grp_out ->SetMarkerColor(kRed);

    run_out_L.ResizeTo(l);
    x_out_L.ResizeTo(l);
    err_out_L.ResizeTo(l);
    xerr_out_L.ResizeTo(l);

    TGraphErrors* grp_out_L = new TGraphErrors(run_out_L, x_out_L, err_out_L, xerr_out_L);
    grp_out_L ->SetMarkerSize(0.6);
    grp_out_L ->SetMarkerStyle(21);
    grp_out_L ->SetMarkerColor(kViolet);

    TMultiGraph * grp = new TMultiGraph();


    TString y_title = GetYTitle(measurement_type, det_id);
    TString title   = GetTitle(measurement_type, device);

    if(plot.Contains("RMS"))
      {
	y_title = "RMS of "+y_title;
	title   = "RMS of "+title;
      }


    if(m>0)grp->Add(grp_in);
    if(k>0)grp->Add(grp_out);
    if(n>0)grp->Add(grp_in_L);
    if(l>0)grp->Add(grp_out_L);


    TLegend *legend = new TLegend(0.80,0.80,0.99,0.99,"","brNDC");
    if(m>0) legend->AddEntry(grp_in,    "IHWP-IN-R", "p");
    if(k>0) legend->AddEntry(grp_out,   "IHWP-OUT-R", "p");
    if(n>0) legend->AddEntry(grp_in_L,  "IHWP-IN-L", "p");
    if(l>0) legend->AddEntry(grp_out_L, "IHWP-OUT-L", "p");
    legend->SetFillColor(0);
    
    mc->cd();
    grp->Draw("AP");
    legend->Draw("");
    
    grp->SetTitle(title);
    grp->GetYaxis()->SetTitle(y_title);
    grp->GetYaxis()->SetTitleOffset(2);
    grp->GetXaxis()->SetTitleOffset(2);
    grp->GetYaxis()->SetTitleSize(0.03);
    grp->GetXaxis()->SetTitleSize(0.03);
    grp->GetYaxis()->CenterTitle();
    
    
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

Plot detector data vs run number

*******************************************/

void QwGUIDatabase::HistogramDetector(TString detector, TString measured_property, Int_t det_id)
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

  gDirectory->Delete();

  if(dDatabaseCont){

    TCanvas *mc = dCanvas->GetCanvas();
    mc->Clear();
    mc->SetFillColor(0);

    // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
    Int_t   subblock          = dCmbSubblock -> GetSelected();
    TString measurement_type  = measurements[dCmbMeasurementType->GetSelected()];
    TString device            = Form("%s%s",detector.Data(),measured_property.Data());
    TString target            = Targets[dCmbTargetType->GetSelected()];
    TString plot              = Plots[dCmbPlotType->GetSelected()];

    //
    // Query Database for Data
    //
    mysqlpp::StoreQueryResult read_data = QueryDetector(detector, measured_property, det_id);

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

    TVectorF x_in(row_size), xerr_in(row_size);
    TVectorF x_out(row_size), xerr_out(row_size);
    TVectorF run_in(row_size), run_out(row_size);
    TVectorF err_in(row_size), err_out(row_size);

    TVectorF x_in_L(row_size), xerr_in_L(row_size);
    TVectorF x_out_L(row_size), xerr_out_L(row_size);
    TVectorF run_in_L(row_size), run_out_L(row_size);
    TVectorF err_in_L(row_size), err_out_L(row_size);

    x_in.Clear();
    x_in.ResizeTo(row_size);
    xerr_in.Clear();
    xerr_in.ResizeTo(row_size);
    run_in.Clear();
    run_in.ResizeTo(row_size);

    x_out.Clear();
    x_out.ResizeTo(row_size);
    xerr_out.Clear();
    xerr_out.ResizeTo(row_size);
    run_out.Clear();
    run_out.ResizeTo(row_size);

    x_in_L.Clear();
    x_in_L.ResizeTo(row_size);
    xerr_in_L.Clear();
    xerr_in_L.ResizeTo(row_size);
    run_in_L.Clear();
    run_in_L.ResizeTo(row_size);

    x_out_L.Clear();
    x_out_L.ResizeTo(row_size);
    xerr_out_L.Clear();
    xerr_out_L.ResizeTo(row_size);
    run_out_L.Clear();
    run_out_L.ResizeTo(row_size);

    Int_t m = 0;
    Int_t k = 0;
    Int_t l = 0;
    Int_t n = 0;

    std::cout<<"############################################\n";
    std::cout<<"QwGUI : Collecting data.."<<std::endl;
    std::cout<<"QwGUI : Retrieved "<<row_size<<" data points\n";
    for (size_t i = 0; i < row_size; ++i)
      { 	    
	if(measurement_type =="a" || measurement_type =="aeo" || measurement_type =="a12" || measurement_type =="d" || measurement_type =="deo" || measurement_type =="d12"){
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

	if(read_data[i]["slow_helicity_plate"] == "out") {
	  if (read_data[i]["wien_reversal"]*1 == 1){
	    run_out.operator()(k)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_out.operator()(k)    = x;
	    xerr_out.operator()(k) = xerr;
	    err_out.operator()(k)  = 0.0;
	    k++;
	  } else {
	    run_out_L.operator()(l)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_out_L.operator()(l)    = x;
	    xerr_out_L.operator()(l) = xerr;
	    err_out_L.operator()(l)  = 0.0;
	    l++;
	  }
	  
	}
	
	if(read_data[i]["slow_helicity_plate"] == "in") {
	  if (read_data[i]["wien_reversal"]*1 == 1){
	    run_in.operator()(m)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_in.operator()(m)    = x;
	    xerr_in.operator()(m) = xerr;
	    err_in.operator()(m)  = 0.0;
	    m++;
	  } else {
	    run_in_L.operator()(n)  = read_data[i]["run_number"]+(read_data[i]["segment_number"]*0.1);
	    x_in_L.operator()(n)    = x;
	    xerr_in_L.operator()(n) = xerr;
	    err_in_L.operator()(n)  = 0.0;
	    n++;
	    
	  }
	}
      }

    // Check to make sure graph is not empty.
    if(m==0 && k==0 && l==0 && n==0){
      std::cout<<"QwGUI : No data were found for the given criteria."<<std::endl;
      exit(1);
    }
    else
      std::cout<<"QwGUI : Moving on to draw the graph"<<std::endl;
    

    run_in.ResizeTo(m);
    x_in.ResizeTo(m);
    err_in.ResizeTo(m);
    xerr_in.ResizeTo(m);

    TGraphErrors* grp_in  = new TGraphErrors(run_in,  x_in, err_in, xerr_in);
    grp_in ->SetMarkerSize(0.6);
    grp_in ->SetMarkerStyle(21);
    grp_in ->SetMarkerColor(kBlue);

    run_in_L.ResizeTo(n);
    x_in_L.ResizeTo(n);
    err_in_L.ResizeTo(n);
    xerr_in_L.ResizeTo(n);

    TGraphErrors* grp_in_L  = new TGraphErrors(run_in_L,  x_in_L, err_in_L, xerr_in_L);
    grp_in_L ->SetMarkerSize(0.6);
    grp_in_L ->SetMarkerStyle(21);
    grp_in_L ->SetMarkerColor(kOrange);

    run_out.ResizeTo(k);
    x_out.ResizeTo(k);
    err_out.ResizeTo(k);
    xerr_out.ResizeTo(k);

    TGraphErrors* grp_out = new TGraphErrors(run_out, x_out, err_out, xerr_out);
    grp_out ->SetMarkerSize(0.6);
    grp_out ->SetMarkerStyle(21);
    grp_out ->SetMarkerColor(kRed);

    run_out_L.ResizeTo(l);
    x_out_L.ResizeTo(l);
    err_out_L.ResizeTo(l);
    xerr_out_L.ResizeTo(l);

    TGraphErrors* grp_out_L = new TGraphErrors(run_out_L, x_out_L, err_out_L, xerr_out_L);
    grp_out_L ->SetMarkerSize(0.6);
    grp_out_L ->SetMarkerStyle(21);
    grp_out_L ->SetMarkerColor(kViolet);

    TMultiGraph * grp = new TMultiGraph();


    TString y_title = GetYTitle(measurement_type, det_id);
    TString title   = GetTitle(measurement_type, device);

    if(plot.Contains("RMS"))
      {
	y_title = "RMS of "+y_title;
	title   = "RMS of "+title;
      }


    if(m>0)grp->Add(grp_in);
    if(k>0)grp->Add(grp_out);
    if(n>0)grp->Add(grp_in_L);
    if(l>0)grp->Add(grp_out_L);


    TLegend *legend = new TLegend(0.80,0.80,0.99,0.99,"","brNDC");
    if(m>0) legend->AddEntry(grp_in,    "IHWP-IN-R", "p");
    if(k>0) legend->AddEntry(grp_out,   "IHWP-OUT-R", "p");
    if(n>0) legend->AddEntry(grp_in_L,  "IHWP-IN-L", "p");
    if(l>0) legend->AddEntry(grp_out_L, "IHWP-OUT-L", "p");
    legend->SetFillColor(0);
    
    mc->cd();
    grp->Draw("AP");
    legend->Draw("");
    
    grp->SetTitle(title);
    grp->GetYaxis()->SetTitle(y_title);
    grp->GetYaxis()->SetTitleOffset(2);
    grp->GetXaxis()->SetTitleOffset(2);
    grp->GetYaxis()->SetTitleSize(0.03);
    grp->GetXaxis()->SetTitleSize(0.03);
    grp->GetYaxis()->CenterTitle();
    
    
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
    //     else if (det_id == ID_BPM || det_id == ID_CMB_BPM ) ytitle  = "Position (mm)";
    //     else if (det_id == ID_BCM || det_it == ID_CMB_BCM)  ytitle  = "Current (#muA)";
    //     else if (det_id == ID_E_CAL) ytitle  = "Energy Change dP/P";
    else  ytitle  = "Yield";
  }
  if (measurement_type == "a" || measurement_type == "aeo" || measurement_type == "a12" )
    ytitle = "Asymmetry (ppm)";

  if (measurement_type == "d" || measurement_type == "deo" || measurement_type == "d12" )
    ytitle = "Beam Position Differences (nm)";
  
  if (measurement_type == "yq"){
    if (det_id == ID_BCM || det_id == ID_CMB_BCM )  ytitle  = "Current (#muA)";
    if (det_id == ID_BPM || det_id == ID_CMB_BPM ) ytitle  = "Beam Position (mm)";
    if (det_id == ID_E_CAL) ytitle  = "Energy Change dP/P";

  }

  if (measurement_type == "yp")
    ytitle = "Beam Position (mm)";
  

  return ytitle;
}

/******************************************

Get the  title for a plot based on the measurement and detector type

*******************************************/
TString QwGUIDatabase::GetTitle(TString measurement_type, TString device)
{

  TString title;
  switch(dCmbXAxis->GetSelected())
    {
    case ID_X_HISTO:
    case ID_X_RUN:
      if (measurement_type == "y") 
	title = Form("%s Yield vs Run Number",device.Data());
      if (measurement_type == "a" || measurement_type == "aeo" || measurement_type == "a12" )
	title = Form("%s Asymmetry vs Run Number",device.Data());
      if (measurement_type == "d" || measurement_type == "deo" || measurement_type == "d12" )
	title = Form("%s Beam Position Difference vs Run Number",device.Data());
      if (measurement_type == "yq")
	title = Form("%s Current vs Run Number",device.Data());
      if (measurement_type == "yp")
	title = Form("%s Beam Position vs Run Number",device.Data());
      break;
    default:
      break;
    }

  return title;
}


/******************************************

Things to do when submit is pushed

*******************************************/
void QwGUIDatabase::OnSubmitPushed() 
{
  ClearData();

  property = "";
  detector = "";

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
  default:
    break;
  }
  
  switch (dCmbXAxis->GetSelected()) 
    {
    case ID_X_RUN:
      PlotDetector(detector,property,dCmbInstrument->GetSelected());
      break;
    case ID_X_BEAM:
      DetectorVsMonitorPlot();
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



