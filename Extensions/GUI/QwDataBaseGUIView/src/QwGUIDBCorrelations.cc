#include "QwGUIDBCorrelations.h"

#include <TG3DLine.h>
#include "TGaxis.h"
#include "TGTextEntry.h"
#include "TStopwatch.h"
#include "TLatex.h"
#include "TDatime.h"

ClassImp(QwGUIDBCorrelations);

using namespace std;

// Device IDs
enum EQwGUIDBCorrelationsInstrumentIDs {
  ID_MD,
  ID_LUMI,
  ID_BEAM,
};

enum EQwGUIDBCorrelationsWidgetIDs {
  NUM_START_RUN,
  NUM_STOP_RUN,
  CMB_YINSTRUMENT,
  CMB_YDETECTOR,
  CMB_XDETECTOR,
  BTN_SUBMIT,
  CMB_TGT,
  CMB_REGRESS,
  BOX_GOODFOR,
};


const char *QwGUIDBCorrelations::MainDetectors[MD_N] = {
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
  "qwk_pmtled",
  "qwk_pmtonl",
  "qwk_pmtltg",
  "qwk_isourc",
  "qwk_preamp",
  "qwk_cagesr",
};

const char *QwGUIDBCorrelations::BeamParameters[BEAM_N] = {
  "diff_qwk_targetX","diff_qwk_targetY","diff_qwk_targetXSlope","diff_qwk_targetYSlope",
  "diff_qwk_energy","asym_qwk_charge","diff_qwk_3c12"
};


const char *QwGUIDBCorrelations::LumiDetectors[LUMI_N] = {
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
  "dslumi_sum",
  "uslumi_sum"
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

const char *QwGUIDBCorrelations::Targets[N_TGTS] = 
{
  "HOME=OutOfBeam","HYDROGEN-CELL","DS-8%-Aluminum","DS-4%-Aluminum","DS-2%-Aluminum",
  "US-4%-Aluminum","US-2%-Aluminum","US-1%-Aluminum","US-Empty-Frame","Optics-3",
  "Unknown position","US-Pure-Al","DS-Cntring-hole","DS-0.5%-C","US-Cntring-hole","All"
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

const char *QwGUIDBCorrelations::GoodForTypes[N_GOODFOR_TYPES] =
{
  "production","commissioning","parity","tracking",
  "centering_target","centering_plug","pedestals","transverse",
  "transverse_horizontal","daq_test","bcm_scan","bpm_scan",
  "ia_scan","pita_scan","rhwp_scan","background_studies",
  "pockels_cell_off","n_to_delta","junk","scanner_on","dis"
};

const char *QwGUIDBCorrelations::RegressionVarsOn[N_REG_VARS_ON]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy"
};


const char *QwGUIDBCorrelations::RegressionVarsOn_5_1[N_REG_VARS_ON_5_1]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy","wrt_asym_charge"

};

const char   *QwGUIDBCorrelations::RegressionVarsOn_3[N_REG_VARS_ON_3]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_bpm3c12X", 
  "wrt_asym_charge",
};

const char   *QwGUIDBCorrelations::RegressionVarsOn_4[N_REG_VARS_ON_4]={
  "wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy",
  "wrt_asym_bcm5",
};

const char   *QwGUIDBCorrelations::RegressionVarsOn_5[N_REG_VARS_ON_5]={
  "wrt_diff_9b_p_4X","wrt_diff_9b_m_4X","wrt_diff_9b_m_4Y","wrt_diff_9b_p_4Y","wrt_diff_bpm3c12X"
};

const char   *QwGUIDBCorrelations::RegressionVarsOn_6[N_REG_VARS_ON_6]={
  "wrt_diff_9b_p_4X","wrt_diff_9b_m_4X","wrt_diff_9b_m_4Y","wrt_diff_9b_p_4Y","wrt_diff_bpm3c12X","wrt_asym_charge"
};

const char   *QwGUIDBCorrelations::RegressionVarsOn_7[N_REG_VARS_ON_7]={
  "wrt_diff_9_p_4X","wrt_diff_9_m_4X","wrt_diff_9_m_4Y","wrt_diff_9_p_4Y","wrt_diff_bpm3c12X"
};

const char   *QwGUIDBCorrelations::RegressionVarsOn_8[N_REG_VARS_ON_8]={
  "wrt_diff_9_p_4X","wrt_diff_9_m_4X","wrt_diff_9_m_4Y","wrt_diff_9_p_4Y","wrt_diff_bpm3c12X","wrt_asym_charge"
};

const char   *QwGUIDBCorrelations::RegressionVarsOn_9[N_REG_VARS_ON_9]={
		"wrt_diff_targetX","wrt_diff_targetY","wrt_diff_targetXSlope","wrt_diff_targetYSlope","wrt_diff_energy","wrt_asym_charge", "wrt_asym_uslumi_sum"};

// Regression types
// To get the raw data from QwAnalysis that pass the standard event cuts use "raw(from rootfiles)"
// To get the unregressed data that pass LinRegBlue event cuts use "off"
const char *QwGUIDBCorrelations::RegressionSchemes[N_REGRESSION_SCHEMES] = {
  "off","on","on_5+1", "on_set3", "on_set4", "on_set5","on_set6","on_set7","on_set8","on_set9","raw(from rootfiles)"
};

QwGUIDBCorrelations::QwGUIDBCorrelations(const TGWindow *p, const TGWindow *main, const TGTab *tab,
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
  dCmbYInstrument     = NULL;
  dCmbYDetector       = NULL;
  dCmbXDetector       = NULL;
  dCmbTargetType      = NULL;
  dCmbRegressionType  = NULL;
  dNumStartRun        = NULL;
  dNumStopRun         = NULL;
  dBtnSubmit          = NULL;
  dLabRunRange        = NULL;
  dLabTarget          = NULL;
  dLabRegression      = NULL;

  GraphArray.Clear();
  LegendArray.Clear();


  RemoveSelectedDataWindow();  
  DataWindowArray.Clear();

  x_detector="";
  y_detector="";
  target="";
  index_first = 0;
  index_last = 0;
  measurements = NULL;



  AddThisTab(this);

}

QwGUIDBCorrelations::~QwGUIDBCorrelations()
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
  if(dLabYInstrument)     delete dLabYInstrument;
  if(dLabYDetector)       delete dLabYDetector;
  if(dLabXDetector)       delete dLabXDetector;
  if(dNumStopRun)         delete dNumStopRun;
  if(dCmbYInstrument)     delete dCmbYInstrument;
  if(dCmbYDetector)       delete dCmbYDetector;
  if(dCmbXDetector)       delete dCmbXDetector;
  if(dCmbTargetType)      delete dCmbTargetType;
  if(dCmbRegressionType)  delete dCmbRegressionType;
  if(dBtnSubmit)          delete dBtnSubmit;

  RemoveThisTab(this);
  IsClosing(GetName());
}



void QwGUIDBCorrelations::MakeLayout()
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

  dLabYInstrument      = new TGLabel(dControlsFrame, "Y Instrument Type");
  dCmbYInstrument      = new TGComboBox(dControlsFrame, CMB_YINSTRUMENT);
  dLabYDetector	       = new TGLabel(dControlsFrame, "Y Detector");
  dCmbYDetector        = new TGComboBox(dControlsFrame, CMB_YDETECTOR);
  dLabXDetector	       = new TGLabel(dControlsFrame, "X Detector");
  dCmbXDetector        = new TGComboBox(dControlsFrame, CMB_XDETECTOR);
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
  dLabRunRange        = new TGLabel(dControlsFrame, "Run Range");
  dRunFrame	      = new TGHorizontalFrame(dControlsFrame);
  dNumStartRun        = new TGNumberEntry(dRunFrame, 13500, 5, NUM_START_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dNumStopRun         = new TGNumberEntry(dRunFrame, 16000, 5, NUM_STOP_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dBtnSubmit          = new TGTextButton(dControlsFrame, "&Submit", BTN_SUBMIT);  //dSlowControls       = new TGButtonGroup(dControlsFrame, "Slow Controls");


  dLabLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 0, 0, 10, 0 );
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 0, 0, 0, 0 );
  dNumLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 10, 10, 0, 0 );
  dChkLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 0, 0, 0, 0 );
  dBtnLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 10, 0 );
  dFrmLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 0, 0 );
  dBoxLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 0, 0 );

  // populate Y
  dCmbYInstrument->AddEntry("Main Detectors", ID_MD);
  dCmbYInstrument->AddEntry("Lumis", ID_LUMI);
  dCmbYInstrument->Connect("Selected(Int_t)","QwGUIDBCorrelations", this, "PopulateYDetectors()");

  // populate X
  for (Int_t i = 0; i < BEAM_N; i++) {
    dCmbXDetector->AddEntry(BeamParameters[i], i);
  }
  dCmbXDetector->Select(0); 
  

  // Populate target combo box
  for (Int_t i = 0; i < N_TGTS; i++) {
    dCmbTargetType->AddEntry(Targets[i], i);
  }
  dCmbTargetType->Select(1); 
 
  // Populate regression combo box
  for (Int_t i = 0; i < N_REGRESSION_SCHEMES; i++) {
    dCmbRegressionType->AddEntry(RegressionSchemes[i], i);
  }
  dCmbRegressionType->Connect("Selected(Int_t)","QwGUIDBCorrelations", this, "RegressionTypeInfo()");
  dCmbRegressionType->Select(1);


  // Populate good for list
  for (Int_t i = 0; i<N_GOODFOR_TYPES; i++){
    dBoxGoodFor->AddEntry(GoodForTypes[i],i+1);
  }


  dCmbYInstrument     -> Resize(150,20);
  dCmbYDetector       -> Resize(150,20);
  dCmbXDetector       -> Resize(150,20);
  dCmbTargetType      -> Resize(150,20);
  dCmbRegressionType  -> Resize(150,20);
  dBoxGoodFor         -> Resize(150,80);


  dControlsFrame      -> AddFrame(dLabYInstrument, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbYInstrument, dCmbLayout ); // detector type
  dControlsFrame      -> AddFrame(dLabYDetector, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbYDetector, dCmbLayout );   // detector name
  dControlsFrame      -> AddFrame(dLabXDetector, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbXDetector, dCmbLayout );   // detector name
  dControlsFrame      -> AddFrame(dLabTarget, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbTargetType, dCmbLayout );	 //target
  dControlsFrame      -> AddFrame(dLabRegression, dLabLayout );
  dControlsFrame      -> AddFrame(dCmbRegressionType, dCmbLayout );  //regression
  dControlsFrame      -> AddFrame(dLabQuality, dLabLayout);
  dControlsFrame      -> AddFrame(dQualityFrame, dFrmLayout);
  dQualityFrame	      -> AddFrame(dChkQualityGood, dChkLayout );
  dChkQualityGood     -> SetState(kButtonDown, kTRUE); //default quality to "good"
  dQualityFrame	      -> AddFrame(dChkQualitySuspect, dChkLayout );
  dQualityFrame	      -> AddFrame(dChkQualityBad, dChkLayout );
  dControlsFrame      -> AddFrame(dLabGoodFor, dLabLayout);
  dControlsFrame      -> AddFrame(dBoxGoodFor, dBoxLayout);
  dBoxGoodFor	      -> SetMultipleSelections(kTRUE);
  dBoxGoodFor	      -> Select(dBoxGoodFor->FindEntry("production")->EntryId());
  dBoxGoodFor	      -> Select(dBoxGoodFor->FindEntry("parity")->EntryId());
  dControlsFrame      -> AddFrame(dLabRunRange, dLabLayout );
  dControlsFrame      -> AddFrame(dRunFrame,dFrmLayout);
  dRunFrame           -> AddFrame(dNumStartRun, dNumLayout );
  dRunFrame           -> AddFrame(dNumStopRun, dNumLayout );
  dControlsFrame      -> AddFrame(dBtnSubmit, dBtnLayout);


  dCmbYInstrument     -> Associate(this);
  dNumStartRun        -> Associate(this);
  dNumStopRun         -> Associate(this);
  dCmbYDetector       -> Associate(this);
  dCmbXDetector       -> Associate(this);
  dCmbTargetType      -> Associate(this);
  dCmbRegressionType  -> Associate(this);
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
  dCanvas -> GetCanvas() -> Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "QwGUIDBCorrelations", this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

}




// Populate Y detectors
void QwGUIDBCorrelations::PopulateYDetectors()
{
  dCmbYDetector->RemoveAll();

  // MD
  if (dCmbYInstrument->GetSelected() == ID_MD){
    for (Int_t k = 0; k < MD_N; k++){
      dCmbYDetector->AddEntry(MainDetectors[k], k);
    } 
  }
  // LUMI
  if (dCmbYInstrument->GetSelected() == ID_LUMI){
    for (Int_t k = 0; k < LUMI_N; k++){
      dCmbYDetector->AddEntry(LumiDetectors[k], k);
    } 
  }

  
  dCmbYDetector->Select(0);
  
}





void QwGUIDBCorrelations::OnReceiveMessage(char *msg)
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


void QwGUIDBCorrelations::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWindow")) return;

  printf("Received Message From: %s\n",obj);
}


QwGUIDataWindow *QwGUIDBCorrelations::GetSelectedDataWindow()
{
  if(dSelectedDataWindow < 0 || dSelectedDataWindow > DataWindowArray.GetLast()) return NULL;

  return (QwGUIDataWindow*)DataWindowArray[dSelectedDataWindow];
}



void QwGUIDBCorrelations::OnObjClose(char *obj)
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


void QwGUIDBCorrelations::OnNewDataContainer(RDataContainer *cont)
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





void QwGUIDBCorrelations::OnRemoveThisTab()
{

};

void QwGUIDBCorrelations::ClearData()
{
  TObject *obj;
  TIter next(GraphArray.MakeIterator());
  obj = next();
  while(obj){ 
    if(obj!=NULL)
      delete obj;
    obj = next();
  }
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
void QwGUIDBCorrelations::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
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
					    "QwGUIDBCorrelations",((TMultiGraph*)plot)->GetTitle(), PT_GRAPH_ER,
					    DDT_DB,600,400);
	  if(!dDataWindow){
	    return;
	  }
	  DataWindowArray.Add(dDataWindow);
	}
	else
	  add = kTrue;

	
	dDataWindow->SetPlotTitle((char*)((TMultiGraph*)plot)->GetTitle());
	dDataWindow->SetLimitsFlag(kFalse);
	dDataWindow->DrawData(*((TMultiGraph*)plot),add,legend);

	SetLogMessage(Form("Looking at graph %s\n",(char*)((TMultiGraph*)plot)->GetTitle()),kTrue);
	
	Connect(dDataWindow,"IsClosing(char*)","QwGUIDBCorrelations",(void*)this,"OnObjClose(char*)");
	Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIDBCorrelations",(void*)this,"OnReceiveMessage(char*)");
	Connect(dDataWindow,"UpdatePlot(char*)","QwGUIDBCorrelations",(void*)this,"OnUpdatePlot(char *)");
	
	return;
      }
    }
  }
}

// Make a mysql query
TString QwGUIDBCorrelations::MakeQuery(TString tables_used, TString x_detector, TString y_detector, TString special_cuts){

  TString slope_calculation_flag;
  TString regression_selected;
  TString target_position;

  /*Basic data selections that are valid for any type of query*/

  /*Get run quality cut information*/
  Bool_t quality[3] = {kFALSE, kFALSE, kFALSE};
  if(dChkQualityGood->IsOn())	      quality[0] 	= kTRUE;
  if(dChkQualityBad->IsOn())	      quality[1]	= kTRUE;
  if(dChkQualitySuspect->IsOn())      quality[2] 	= kTRUE;

  TString quality_check = " AND runlet.runlet_quality_id is not NULL";
  for (size_t i=0; i<3;i++){
	if(!quality[i]){
		quality_check = quality_check + Form(" AND !FIND_IN_SET('%i',runlet.runlet_quality_id)",i+1);
	}
  }

  /*Get good_for cut information*/
  Bool_t good_for[N_GOODFOR_TYPES];
  std::vector<Int_t> good_for_id;

  TList *selected_types = new TList;
  dBoxGoodFor->GetSelectedEntries(selected_types);
  TGTextLBEntry *entry;
  TIter next(selected_types);
  TString good_for_check = " AND y.good_for_id is not NULL ";

  for(size_t i=0; i<N_GOODFOR_TYPES;i++){ //initialize them as false
	  good_for[i] = kFALSE;
  }

  // validate the entries that are selected.
  while( (entry = (TGTextLBEntry *)next()) ) {
	  good_for[dBoxGoodFor->FindEntry(entry->GetTitle())->EntryId()-1] = kTRUE;
  }

  for (size_t i=0; i<N_GOODFOR_TYPES;i++){
    if(good_for[i]){
      good_for_check = good_for_check + Form(" AND FIND_IN_SET('%i',y.good_for_id)",i+1);
    }
  }
  

  if (RegressionSchemes[dCmbRegressionType->GetSelected()]=="off"){
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
    ="SELECT y.value as y_val,y.error as y_err,x.value as x_val,x.error as x_err, slow_helicity_plate,runlet.runlet_quality_id,y.good_for_id,target_position, (scd.value<0)*-2+1 as wien_reversal FROM "
    +tables_used+", analysis,runlet, slow_controls_settings, slow_controls_data  as scd "
    +" WHERE y.analysis_id=x.analysis_id and y.analysis_id=analysis.analysis_id AND analysis.runlet_id=runlet.runlet_id and runlet.runlet_id=slow_controls_settings.runlet_id and runlet.runlet_id= scd.runlet_id "
    +" AND  y.subblock=0 AND "
    +" x.monitor ='"+x_detector+"' AND y.detector='"+y_detector+"' AND "
    +" y.measurement_type='a' AND x.measurement_type='"+beam_measurement+"' AND "
    + Form(" runlet.run_number>=%i and runlet.run_number<=%i " ,index_first,index_last)+
    + Form(" AND analysis.slope_correction='%s' AND analysis.slope_calculation='%s' ",regression_selected.Data(),slope_calculation_flag.Data())+
    + quality_check
    + good_for_check
    + special_cuts
    +" ;";


//    = "SELECT "+outputs+" slow_helicity_plate, data.runlet_quality_id,data.good_for_id,target_position, (scd.value<0)*-2+1 as wien_reversal"
//     +" FROM "+tables_used+" analysis_view as ana, slow_controls_settings , slow_controls_data as scd   "
//     +" WHERE "+table_links+" data.analysis_id = ana.analysis_id AND ana.runlet_id = slow_controls_settings.runlet_id "
//     + Form(" AND data.slope_correction='%s' AND data.slope_calculation='%s' ",regression_selected.Data(),slope_calculation_flag.Data())
//     +" AND data.subblock = "+ Form("%i",subblock) 
//     +" AND data.error !=0 "
//     +target_position+
//     +" AND scd.runlet_id=ana.runlet_id "
//     + quality_check
//     + good_for_check
//     + special_cuts
//     + " ORDER BY data.run_number, data.segment_number;";
  

  return querystring;
}




mysqlpp::StoreQueryResult  QwGUIDBCorrelations::QueryDetector()
{
  Bool_t ldebug = kTRUE;
  TString y_data_table;
  TString x_data_table;

  dDatabaseCont->Connect();

  mysqlpp::Query query1     = dDatabaseCont-> Query();
  mysqlpp::Query query2     = dDatabaseCont-> Query();


  //   TString det_type;
  Int_t   phi_fg_id = 0;
  
  // Set the correct tables to read the data from.
  switch (dCmbYInstrument->GetSelected()){
  case ID_MD:
    y_data_table   = "md_data_view";
    break;
  case ID_LUMI:
    y_data_table   = "lumi_data_view";
    break;
  default:
    break;
  }

  // Right now, in this basic version, I am going to fix X data table to be beam view.
  // Later we can figure out a way to make this for variable data tables
  x_data_table = "beam_view";

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
  TString tables_used = y_data_table+" as y,"+x_data_table+" as x ";
  TString special_cuts = Form(" AND scd.sc_detector_id = '%i' ", phi_fg_id);


  querystring= MakeQuery(tables_used,x_detector,y_detector,special_cuts);

  if(ldebug) std::cout<<"QUERYSTRING="
		      <<querystring<<std::endl;

  query << querystring;
  
  
  mysqlpp::StoreQueryResult read_data = query.store();
  dDatabaseCont->Disconnect(); 


  return read_data;

}


// /******************************************

// Plot detector data vs run number/slug/time

// *******************************************/

void QwGUIDBCorrelations::PlotCorrelation()
{

  Bool_t ldebug = kFALSE;

  // histo parameters
  gDirectory->Delete();
  gStyle->Reset();
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.2);
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.02);
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
      
      std::cout  <<"There is no data for "<<x_detector<<" in the given range!"<<std::endl;
      return;
    }


    
    //vectors to fill IHWP IN & Wien right values
    vector <string> run_in_label;
    vector <string> run_out_label;
    TVectorD x_in(row_size), xerr_in(row_size);
    TVectorD x_out(row_size), xerr_out(row_size);
    TVectorD y_in(row_size), yerr_in(row_size);
    TVectorD y_out(row_size), yerr_out(row_size);
    
    //vectors to fill IHWP IN & Wien left values
    TVectorD x_in_L(row_size), xerr_in_L(row_size);
    TVectorD x_out_L(row_size), xerr_out_L(row_size);
    TVectorD y_in_L(row_size), yerr_in_L(row_size);
    TVectorD y_out_L(row_size), yerr_out_L(row_size);

    //vectors to fill bad values
    TVectorD x_bad(row_size), xerr_bad(row_size);
    TVectorD y_bad(row_size), yerr_bad(row_size);
   
    //vectors to fill suspect values
    TVectorD x_suspect(row_size), xerr_suspect(row_size);
    TVectorD y_suspect(row_size), yerr_suspect(row_size);


    x_in.Clear();
    x_in.ResizeTo(row_size);
    xerr_in.Clear();
    xerr_in.ResizeTo(row_size);
    y_in.Clear();
    y_in.ResizeTo(row_size);
    yerr_in.Clear();
    yerr_in.ResizeTo(row_size);
    x_out.Clear();
    x_out.ResizeTo(row_size);
    xerr_out.Clear();
    xerr_out.ResizeTo(row_size);
    y_out.Clear();
    y_out.ResizeTo(row_size);
    yerr_out.Clear();
    yerr_out.ResizeTo(row_size);
    x_in_L.Clear();
    x_in_L.ResizeTo(row_size);
    xerr_in_L.Clear();
    xerr_in_L.ResizeTo(row_size);
    y_in_L.Clear();
    y_in_L.ResizeTo(row_size);
    yerr_in_L.Clear();
    yerr_in_L.ResizeTo(row_size);

    x_out_L.Clear();
    x_out_L.ResizeTo(row_size);
    xerr_out_L.Clear();
    xerr_out_L.ResizeTo(row_size);
    y_out_L.Clear();
    y_out_L.ResizeTo(row_size);
    yerr_out_L.Clear();
    yerr_out_L.ResizeTo(row_size);

    x_bad.Clear();
    x_bad.ResizeTo(row_size);
    xerr_bad.Clear();
    xerr_bad.ResizeTo(row_size);
    y_bad.Clear();
    y_bad.ResizeTo(row_size);
    yerr_bad.Clear();
    yerr_bad.ResizeTo(row_size);

    x_suspect.Clear();
    x_suspect.ResizeTo(row_size);
    xerr_suspect.Clear();
    xerr_suspect.ResizeTo(row_size);
    y_suspect.Clear();
    y_suspect.ResizeTo(row_size);
    yerr_suspect.Clear();
    yerr_suspect.ResizeTo(row_size);

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


    Float_t x = 0.0 , xerr = 0.0, y = 0.0 , yerr = 0.0;

    std::cout<<"############################################\n";
    std::cout<<"QwGUI : Collecting data.."<<std::endl;
    std::cout<<"QwGUI : Retrieved "<<row_size<<" data points\n";
    for (Int_t i = 0; i < row_size; ++i)
      { 	   

	// Convert asymmetries  in to ppm, convert differences in to nm/murad
	x    = (read_data[i]["x_val"])*1e6; // convert to  ppm/ nm
	xerr = (read_data[i]["x_err"])*1e6; // convert to ppm/ nm
	y    = (read_data[i]["y_val"])*1e6; // convert to  ppm/ nm
	yerr = (read_data[i]["y_err"])*1e6; // convert to ppm/ nm
        

	// Now fill the mean values

	// good data
        if(read_data[i]["runlet_quality_id"] == "1"){
	  if(read_data[i]["slow_helicity_plate"] == "out") {
	    if (read_data[i]["wien_reversal"]*1 == 1){
	      x_out.operator()(k)    = x;
	      xerr_out.operator()(k) = xerr;
	      y_out.operator()(k)    = y;
	      yerr_out.operator()(k) = yerr;
	      k++;
	    } 
	    else {
	      x_out_L.operator()(l)    = x;
	      xerr_out_L.operator()(l) = xerr;
	      y_out_L.operator()(l)    = y;
	      yerr_out_L.operator()(l) = yerr;
	      l++;

	    }
	    
	  }
	  
	  if(read_data[i]["slow_helicity_plate"] == "in") {

	    if (read_data[i]["wien_reversal"]*1 == 1){
	      x_in.operator()(m)    = x;
	      xerr_in.operator()(m) = xerr;
	      y_in.operator()(m)    = y;
	      yerr_in.operator()(m) = yerr;
	      m++;
	    } 
	    else {
	      x_in_L.operator()(n)    = x;
	      xerr_in_L.operator()(n) = xerr;
	      y_in_L.operator()(n)    = y;
	      yerr_in_L.operator()(n) = yerr;
	      n++;
	      
	    }
	  }
	}
	
	// bad data
	if((read_data[i]["runlet_quality_id"] == "2")   | //or
	   (read_data[i]["runlet_quality_id"] == "1,2") | //or
	   (read_data[i]["runlet_quality_id"] == "2,3") ) { //all instances of bad
	  

	  x_bad.operator()(o)    = x;
	  xerr_bad.operator()(o) = xerr;
	  y_bad.operator()(o)    = y;
	  yerr_bad.operator()(o) = yerr;
	  o++;
	}
	
	//suspect data
	if((read_data[i]["runlet_quality_id"] == "3")  | //or
	   (read_data[i]["runlet_quality_id"] == "1,3")) {// suspect (but not bad)
	  x_suspect.operator()(p)    = x;
	  xerr_suspect.operator()(p) = xerr;
	  y_suspect.operator()(p)    = y;
	  yerr_suspect.operator()(p) = yerr;
	  p++;
	}
	
      }
    
    // Check to make sure all the graphs are not empty. We should have at least one graph to draw
    if(m==0 && k==0 && l==0 && n==0 && o==0 && p==0){
      std::cout<<"QwGUI : No data were found for the given criteria."<<std::endl;
      exit(1);
    }
    else
      std::cout<<"QwGUI : Moving on to draw the graph"<<std::endl;
    
//GROUP_IN
    if(m>0){
      x_in.ResizeTo(m);
      xerr_in.ResizeTo(m);
      y_in.ResizeTo(m);
      yerr_in.ResizeTo(m);    
  
      grp_in  = new TGraphErrors(x_in, y_in, xerr_in, yerr_in);
      grp_in ->SetName("IHWP-IN-R"); 
      grp_in ->SetMarkerSize(1.5);
      grp_in ->SetMarkerStyle(21);
      grp_in ->SetMarkerColor(kBlue);
      grp_in ->Fit("pol1");
      fit1 = grp_in->GetFunction("pol1");
      fit1 -> SetLineColor(kBlue);
 
    }

//GROUP_IN_L
    if(n>0){
      x_in_L.ResizeTo(n);
      xerr_in_L.ResizeTo(n);
      y_in_L.ResizeTo(n);
      yerr_in_L.ResizeTo(n);      
      grp_in_L  = new TGraphErrors(x_in_L,y_in_L, xerr_in_L, yerr_in_L);
      grp_in_L ->SetName("IHWP-IN-L"); 
      grp_in_L ->SetMarkerSize(1.5);
      grp_in_L ->SetMarkerStyle(21);
      grp_in_L ->SetMarkerColor(kOrange-2);
      grp_in_L ->Fit("pol1");
      fit2 = grp_in_L->GetFunction("pol1");
      fit2 -> SetLineColor(kOrange-2);

    }

//GROUP_OUT
    if(k>0){
      x_out.ResizeTo(k);
      xerr_out.ResizeTo(k);
      y_out.ResizeTo(k);
      yerr_out.ResizeTo(k);      
      grp_out = new TGraphErrors(x_out, y_out,xerr_out, yerr_out);
      grp_out ->SetName("IHWP-OUT-R");
      grp_out ->SetMarkerSize(1.5);
      grp_out ->SetMarkerStyle(21);
      grp_out ->SetMarkerColor(kRed);
      grp_out ->Fit("pol1");
      fit3 = grp_out->GetFunction("pol1");
      fit3 -> SetLineColor(kRed);
    }

//GROUP_OUT_L
    if(l>0){
      x_out_L.ResizeTo(l);
      xerr_out_L.ResizeTo(l);
      y_out_L.ResizeTo(l);
      yerr_out_L.ResizeTo(l);      
      grp_out_L = new TGraphErrors(x_out_L,y_out_L, xerr_out_L, yerr_out_L);
      grp_out_L ->SetName("IHWP-OUT-L");
      grp_out_L ->SetMarkerSize(1.5);
      grp_out_L ->SetMarkerStyle(21);
      grp_out_L ->SetMarkerColor(kViolet);
      grp_out_L ->Fit("pol1");
      fit4 = grp_out_L->GetFunction("pol1");
      fit4 -> SetLineColor(kViolet);

    }

//GROUP_BAD
    if(o>0){
      x_bad.ResizeTo(o);
      xerr_bad.ResizeTo(o);
      y_bad.ResizeTo(o);
      yerr_bad.ResizeTo(o);      
      grp_bad = new TGraphErrors(x_bad,y_bad, xerr_bad, yerr_bad);
      grp_bad ->SetName("Bad");       
      grp_bad ->SetMarkerSize(1.5);
      grp_bad ->SetMarkerStyle(29);
      grp_bad ->SetMarkerColor(kBlack);
      grp_bad ->Fit("pol1");
      fit5 = grp_out->GetFunction("pol1");
      fit5 -> SetLineColor(kBlack);

    }

//GROUP_SUSPECT
    if(p>0){
      x_suspect.ResizeTo(p);
      xerr_suspect.ResizeTo(p);
      y_suspect.ResizeTo(p);
      yerr_suspect.ResizeTo(p);      
      grp_suspect = new TGraphErrors(x_suspect,y_suspect, xerr_suspect, yerr_suspect);
      grp_suspect ->SetName("Suspect");   
      grp_suspect ->SetMarkerSize(1.5);
      grp_suspect ->SetMarkerStyle(29);
      grp_suspect ->SetMarkerColor(kGreen);
      grp_suspect ->Fit("pol1");
      fit6 = grp_suspect->GetFunction("pol1");
      fit6 -> SetLineColor(kGreen);
 
    }
    
    grp = new TMultiGraph();

    TString title = Form("%s vs %s ",y_detector.Data(),x_detector.Data());
    TString y_title ="Asymmetry (ppm)";
    TString x_title;
    if((dCmbXDetector->GetSelected())<2) x_title = "Difference(nm)";
    if((dCmbXDetector->GetSelected())<4 and (dCmbXDetector->GetSelected())>1) x_title = "Difference(#murad)";
    if((dCmbXDetector->GetSelected())<6 and (dCmbXDetector->GetSelected())>3) x_title = "Asymmetry(ppm)";
    if((dCmbXDetector->GetSelected())>5) x_title = "Difference(nm)";

    
    if(grp){
      if(m>0)grp->Add(grp_in);
      if(k>0)grp->Add(grp_out);
      if(n>0)grp->Add(grp_in_L);
      if(l>0)grp->Add(grp_out_L);
      if(o>0)grp->Add(grp_bad);
      if(p>0)grp->Add(grp_suspect);
      grp->Draw("AEP");
      grp->SetTitle(title);
      grp->GetYaxis()->SetTitle(y_title);
      grp->GetXaxis()->SetTitle(x_title);
      grp->GetYaxis()->CenterTitle();
      grp->GetXaxis()->SetTitleOffset(1.5); 
    } 
    else
      std::cout<<"There are no data in the given run range ! "<<std::endl;
    
    TLegend *legend = new TLegend(0.60,0.80,0.99,0.99,"","brNDC");
    if(m>0) legend->AddEntry(grp_in, Form("<IN_R> Slope = %2.5f #pm %2.5f [chi^2/ndf = %2.1f]", 
					  fit1->GetParameter(1), fit1->GetParError(1), (fit1->GetChisquare())/fit1->GetNDF()), "p");
    if(n>0) legend->AddEntry(grp_in_L, Form("<IN_L> Slope  = %2.5f #pm %2.5f [chi^2/ndf = %2.1f]", 
					    fit2->GetParameter(1), fit2->GetParError(1), (fit2->GetChisquare())/fit2->GetNDF()), "p");
    if(k>0) legend->AddEntry(grp_out,Form("<OUT_R> Slope = %2.5f #pm %2.5f [chi^2/ndf = %2.1f]", 
					  fit3->GetParameter(1), fit3->GetParError(1), (fit3->GetChisquare())/fit3->GetNDF()), "p");
    if(l>0) legend->AddEntry(grp_out_L,Form("<OUT_L> Slope = %2.5f #pm %2.5f [chi^2/ndf = %2.1f]", 
					    fit4->GetParameter(1), fit4->GetParError(1), (fit4->GetChisquare())/fit4->GetNDF()), "p");
    if(o>0) legend->AddEntry(grp_bad,Form("<BAD> Slope = %2.5f #pm %2.5f [chi^2/ndf = %2.1f]", 
					  fit5->GetParameter(1), fit5->GetParError(1), (fit5->GetChisquare())/fit5->GetNDF()), "p");
    if(p>0) legend->AddEntry(grp_suspect,Form("<SUSPECT> Slope = %2.5f #pm %2.5f [chi^2/ndf = %2.1f] ", 
					      fit6->GetParameter(1), fit6->GetParError(1), (fit6->GetChisquare())/fit6->GetNDF()), "p");
    legend->SetFillColor(0);
    legend->Draw();


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



/*****************************************
A function to print out regression selection and when sensitivities are requested to 
fill the different IVs in to the detector combo box

*****************************************/
void QwGUIDBCorrelations::RegressionTypeInfo(){

  TCanvas *mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->cd();
  mc->SetFillColor(0);
  regression_set    = RegressionSchemes[dCmbRegressionType->GetSelected()];


  TPaveText *T1 = new TPaveText(0.05,0.05,0.95,0.95,"ndc");
  T1->SetTextAlign(12);
  T1->SetTextSize(0.03);

  if(regression_set == "raw(from rootfiles)"){
    T1->AddText("Selecting unregressed data that have passed standard QwAnalysis event cuts!");
    T1->AddText("These data are unregressed and are averages of outputs that can be find in rootfiles.");
  }
  else if(regression_set == "off"){
    T1->AddText("Selecting unregressed data that passed regression script event cuts!");
  }
  else{
    T1->AddText("Selecting Regression that use IVs:");

    if(regression_set == "on"){
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_energy");    
      gPad->Update();  
      
    }
    else if(regression_set == "on_5+1"){
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_energy"); 
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set3"){
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_bpm3c12X"); 
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set4"){
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");      
      T1->AddText("#bullet diff_targetXSlope");        
      T1->AddText("#bullet diff_targetYSlope");      
      T1->AddText("#bullet diff_energy"); 
      T1->AddText("#bullet asym_bcm5"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set5"){
      T1->AddText("#bullet diff_9b_m_4X=diff_qwk_3h09bX-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9b_m_4Y=diff_qwk_3h09bY-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9b_p_4X=diff_qwk_3h09bX+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9b_p_4Y=diff_qwk_3h09bY+diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_bpm3c12X"); 
      gPad->Modified();  
      gPad->Update();  

    }
    else if(regression_set == "on_set6"){
      T1->AddText("#bullet diff_9b_m_4X=diff_qwk_3h09bX-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9b_m_4Y=diff_qwk_3h09bY-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9b_p_4X=diff_qwk_3h09bX+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9b_p_4Y=diff_qwk_3h09bY+diff_qwk_3h04Y");
      T1->AddText("#bullet diff__bpm3c12X");       
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set7"){
      T1->AddText("#bullet diff_9_m_4X=diff_qwk_3h09X-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9_m_4Y=diff_qwk_3h09Y-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9_p_4X=diff_qwk_3h09X+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9_p_4Y=diff_qwk_3h09Y+diff_qwk_3h04Y");
      T1->AddText("#bullet diff__bpm3c12X");       
      gPad->Update();  

    }
    else if(regression_set == "on_set8"){
      T1->AddText("#bullet diff_9_m_4X=diff_qwk_3h09X-diff_qwk_3h04X");
      T1->AddText("#bullet diff_9_m_4Y=diff_qwk_3h09Y-diff_qwk_3h04Y");      
      T1->AddText("#bullet diff_9_p_4X=diff_qwk_3h09X+diff_qwk_3h04X");        
      T1->AddText("#bullet diff_9_p_4Y=diff_qwk_3h09Y+diff_qwk_3h04Y");
      T1->AddText("#bullet diff__bpm3c12X");       
      T1->AddText("#bullet asym_charge"); 
      gPad->Update();  

    }
    else if(regression_set == "on_set9"){
      T1->AddText("#bullet diff_targetX");
      T1->AddText("#bullet diff_targetY");
      T1->AddText("#bullet diff_targetXSlope");
      T1->AddText("#bullet diff_targetYSlope");
      T1->AddText("#bullet diff_energy");
      T1->AddText("#bullet asym_charge");
      T1->AddText("#bullet asym_uslumi_sum");
      gPad->Update();

    }
  } 
  T1->Draw();
  mc->Modified();
  mc->Update();
}




/******************************************

Things to do when submit is pushed

*******************************************/
void QwGUIDBCorrelations::OnSubmitPushed() 
{
  ClearData();

  index_first       = dNumStartRun -> GetIntNumber();
  index_last        = dNumStopRun  -> GetIntNumber();
  regression_set    = RegressionSchemes[dCmbRegressionType->GetSelected()];
  target            = Targets[dCmbTargetType->GetSelected()];
  x_detector        = BeamParameters[dCmbXDetector->GetSelected()];
  x_detector.Replace(0,5,"");

  std::cout<<(dCmbYInstrument->GetSelected())<<std::endl;

  switch (dCmbYInstrument->GetSelected()) {
  case ID_MD:
    y_detector = MainDetectors[dCmbYDetector->GetSelected()];
    break;
  case ID_LUMI:
    y_detector = LumiDetectors[dCmbYDetector->GetSelected()];
    break;
  default:
    break;
  }
  
  switch (dCmbXDetector->GetSelected()) 
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
      beam_measurement='d';
      break;
    case 5:
      beam_measurement='a';
      break;
    default:
      break;
    }

  PlotCorrelation();
}

// Process events generated by the object in the frame.
Bool_t QwGUIDBCorrelations::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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



