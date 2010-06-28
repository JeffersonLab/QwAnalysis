#include "QwGUIDatabase.h"

#include <TG3DLine.h>
#include "TGaxis.h"

ClassImp(QwGUIDatabase);

/*
enum QwGUIDatabaseIndentificator {
  BA_POS_DIFF,
  BA_TGT_PARAM

};
*/

enum EQwGUIDatabaseInstrumentIDs {
  ID_MD,
  ID_LUMI,
  ID_BEAM
};

enum EQwGUIDatabaseWidgetIDs {
  NUM_START_RUN,
  NUM_STOP_RUN,
  CMB_DETECTOR,
  CMB_SUBBLOCK,
  CMB_MEASUREMENT_TYPE,
  CMB_INSTRUMENT,
  BTN_SUBMIT
};

const char *QwGUIDatabase::DetectorCombos[N_DETECTORS] = 
  {
    "md1neg",
    "md2neg",
    "md3neg",
    "md4neg",
    "md5neg",
    "md6neg",
    "md7neg",
    "md8neg",
    "md1pos",
    "md2pos",
    "md3pos",
    "md4pos",
    "md5pos",
    "md6pos",
    "md7pos",
    "md8pos",
    "md0neg",
    "md0pos",
    "spare1",
    "ancid1",
    "ancid2",
    "ancid3",
    "spare2",
    "spare3",
    "md1neg_md5neg",
    "md2neg_md6neg",
    "md3neg_md7neg",
    "md4neg_md8neg",
    "md1pos_md5pos",
    "md2pos_md6pos",
    "md3pos_md7pos",
    "md4pos_md8pos",
    "combinationallmd"
  };

const char *QwGUIDatabase::BeamMonitors[N_BEAM_MONITORS] = 
{
  "x", "y", "x_theta", "y_theta", "q", "energy"
};

const char *QwGUIDatabase::LumiCombos[N_LUMIS] = 
{
  "uslumi1neg",
  "uslumi1pos",
  "uslumi3neg",
  "uslumi3pos",
  "uslumi5neg",
  "uslumi5pos",
  "uslumi7neg",
  "uslumi7pos",
  "dslumi1",
  "dslumi2",
  "dslumi3",
  "dslumi4",
  "dslumi5",
  "dslumi6",
  "dslumi7",
  "dslumi8"
};

const char *QwGUIDatabase::DetectorMeasurementTypes[N_DET_MEAS_TYPES]=
  {
    "a", "y"
  };

const char *QwGUIDatabase::Subblocks[N_SUBBLOCKS] = 
{
  "0", "1", "2", "3", "4"
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
  dCmbInstrument      = NULL;
  dCmbDetector        = NULL;
  dCmbSubblock        = NULL;
  dCmbMeasurementType = NULL;
  dNumStartRun        = NULL;
  dNumStopRun         = NULL;
  dBtnSubmit          = NULL;
  dLabStartRun        = NULL;
  dLabStopRun         = NULL;

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
  if(dNumStopRun)         delete dNumStopRun;
  if(dCmbInstrument)      delete dCmbInstrument;
  if(dCmbDetector)        delete dCmbDetector;
  if(dCmbSubblock)        delete dCmbSubblock;
  if(dCmbMeasurementType) delete dCmbMeasurementType;
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
  gStyle->SetPadBottomMargin(0.25);
  gStyle->SetPadLeftMargin(0.1);  
  gStyle->SetPadRightMargin(0.03);  

  // histo parameters
  gStyle->SetTitleYOffset(1.0);
  gStyle->SetTitleXOffset(0.8);
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleSize(0.08);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(10);
  gStyle->SetTitleFontSize(0.08);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");


  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  dCmbInstrument = new TGComboBox(dControlsFrame, CMB_INSTRUMENT);
  dLabStartRun = new TGLabel(dControlsFrame, "First Run");
  dNumStartRun = new TGNumberEntry(dControlsFrame, 0, 5, NUM_START_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dLabStopRun = new TGLabel(dControlsFrame, "Last Run");
  dNumStopRun = new TGNumberEntry(dControlsFrame, 10000, 5, NUM_STOP_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dCmbDetector = new TGComboBox(dControlsFrame, CMB_DETECTOR);
  dCmbSubblock = new TGComboBox(dControlsFrame, CMB_SUBBLOCK);
  dCmbMeasurementType = new TGComboBox(dControlsFrame, CMB_MEASUREMENT_TYPE);
  dBtnSubmit = new TGTextButton(dControlsFrame, "&Submit", BTN_SUBMIT);

  dLabLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dNumLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 10, 10, 5, 5 );
  dBtnLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 5, 5 );

  dCmbInstrument->AddEntry("Main Detectors", ID_MD);
  dCmbInstrument->AddEntry("Beam Monitors", ID_BEAM);
  dCmbInstrument->AddEntry("LUMI Detectors", ID_LUMI);
  dCmbInstrument->Connect("Selected(Int_t)","QwGUIDatabase", this, "PopulateDetectorComboBox()");
  dCmbInstrument->Select(dCmbInstrument->FindEntry("Main Detectors")->EntryId());

//  for (Int_t i = 0; i < N_DETECTORS; i++) {
//    dCmbDetector->AddEntry(DetectorCombos[i], i);
//  }
  for (Int_t i = 0; i < N_SUBBLOCKS; i++) {
    dCmbSubblock->AddEntry(Subblocks[i], i);
  }
  for (Int_t i = 0; i < N_DET_MEAS_TYPES; i++) {
    dCmbMeasurementType->AddEntry(DetectorMeasurementTypes[i], i);
  }

//  dCmbDetector->Select(dCmbDetector->FindEntry("combinationallmd")->EntryId());
  dCmbSubblock->Select(0);
  dCmbMeasurementType->Select(dCmbMeasurementType->FindEntry("a")->EntryId());

  dCmbInstrument->Resize(150,20);
  dCmbDetector->Resize(150,20);
  dCmbSubblock->Resize(150,20);
  dCmbMeasurementType->Resize(150,20);

  dControlsFrame->AddFrame(dCmbInstrument, dCmbLayout );
  dControlsFrame->AddFrame(dLabStartRun, dLabLayout );
  dControlsFrame->AddFrame(dNumStartRun, dNumLayout );
  dControlsFrame->AddFrame(dLabStopRun, dLabLayout );
  dControlsFrame->AddFrame(dNumStopRun, dNumLayout );
  dControlsFrame->AddFrame(dCmbDetector, dCmbLayout );
  dControlsFrame->AddFrame(dCmbSubblock, dCmbLayout );
  dControlsFrame->AddFrame(dCmbMeasurementType, dCmbLayout );
  dControlsFrame->AddFrame(dBtnSubmit, dBtnLayout);

  dCmbInstrument -> Associate(this);
  dNumStartRun -> Associate(this);
  dNumStopRun -> Associate(this);
  dCmbDetector-> Associate(this);
  dCmbSubblock-> Associate(this);
  dCmbMeasurementType-> Associate(this);
  dBtnSubmit-> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "QwGUIDatabase", this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  //  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);
  //  super_canvas -> Divide(2,4);

}

void QwGUIDatabase::OnSubmitPushed() {

  ClearData();

  switch (dCmbInstrument->GetSelected()) {
    case ID_MD:
      DetectorPlot();
      break;
    case ID_BEAM:
      BeamMonitorPlot();
      break;
    case ID_LUMI:
      DetectorPlot();
      break;
    default:
      break;
  }

}

void QwGUIDatabase::PopulateDetectorComboBox()
{
  dCmbDetector->RemoveAll();

  if (dCmbInstrument->GetSelected() == ID_MD) {
    for (Int_t i = 0; i < N_DETECTORS; i++) {
      dCmbDetector->AddEntry(DetectorCombos[i], i);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("combinationallmd")->EntryId());
  }
  if (dCmbInstrument->GetSelected() == ID_LUMI) {
    for (Int_t i = 0; i < N_LUMIS; i++) {
      dCmbDetector->AddEntry(LumiCombos[i], i);
    }
    dCmbDetector->Select(dCmbDetector->FindEntry("uslumi1neg")->EntryId());
  }
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
void QwGUIDatabase::BeamMonitorPlot()
{
  if(dDatabaseCont){
//    std::cout << "Beam Monitors selected and Submit pushed." <<std::endl;
    TGraphErrors *grp;

    //
    // Query Database for Data
    //

    dDatabaseCont->Connect();

    mysqlpp::Query query = dDatabaseCont->Query();

    // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
    Int_t subblock = dCmbSubblock->GetSelected();
    Int_t run_first = dNumStartRun->GetIntNumber();
    Int_t run_last = dNumStopRun->GetIntNumber();
    string measurement_type = DetectorMeasurementTypes[dCmbMeasurementType->GetSelected()];

    query << "SELECT * FROM summary_b" << measurement_type << " ";
    query << "WHERE subblock = " << subblock << " ";
    query << "AND run_number BETWEEN ";
    query << run_first << " AND ";
    query << run_last << " ";
    query << "ORDER BY run_number, segment_number";
    vector<QwParityDB::summary_by> res;
    query.storein(res);

    dDatabaseCont->Disconnect(); 

    //
    // Loop Over Results and Fill Vectors
    //

    Int_t res_size = res.size();
    printf("Number of rows returned:  %ld\n",res_size);

    // Loop over all rows
    TVectorF time(res_size), timeerr(res_size);
    TVectorF q(res_size), qerr(res_size), e(res_size), eerr(res_size);
    TVectorF x(res_size), xerr(res_size), y(res_size), yerr(res_size);
    TVectorF theta_x(res_size), theta_xerr(res_size), theta_y(res_size), theta_yerr(res_size);

    Float_t run_number;
    Int_t segment_number;
    Int_t i = 0;
    vector<QwParityDB::summary_by>::iterator it;
    for (it = res.begin(); it != res.end(); ++it) {
      run_number = it->run_number;
      if (!it->segment_number.is_null) 
        segment_number = it->segment_number.data;
      Float_t f_segment_number = segment_number;
      time[i] = run_number + f_segment_number/100;
      timeerr[i] = 0;
      if (!it->x_value.is_null)
        x[i] = it->x_value.data;
      if (!it->x_error.is_null)
        xerr[i] = it->x_error.data;
      if (!it->y_value.is_null)
        y[i] = it->y_value.data;
      if (!it->y_error.is_null)
        yerr[i] = it->y_error.data;
      if (!it->theta_x_value.is_null)
        theta_x[i] = it->theta_x_value.data;
      if (!it->theta_x_error.is_null)
        theta_xerr[i] = it->theta_x_error.data;
      if (!it->theta_y_value.is_null)
        theta_y[i] = it->theta_y_value.data;
      if (!it->theta_y_error.is_null)
        theta_yerr[i] = it->theta_y_error.data;
      if (!it->q_value.is_null)
        q[i] = it->q_value.data;
      if (!it->q_error.is_null)
        qerr[i] = it->q_error.data;
      if (!it->energy_value.is_null)
        e[i] = it->energy_value.data;
      if (!it->energy_error.is_null)
        eerr[i] = it->energy_error.data;

      i++;
    } // End loop over results

    //
    // Construct Graphs for Plotting
    //

    string grp_title;

    for (Int_t i = 0; i < N_BEAM_MONITORS; i++) {
      switch (i) {
        case 0: // X position @ target
          grp = new TGraphErrors(time, x, timeerr, xerr);
          grp_title = "X Position";
          if (measurement_type == "y") {
	          grp->GetYaxis()->SetTitle("Position []");
          }
          if (measurement_type == "a") {
	          grp->GetYaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 1: // Y position @ target
          grp = new TGraphErrors(time, y, timeerr, yerr);
          grp_title = "Y Position";
          if (measurement_type == "y") {
	          grp->GetYaxis()->SetTitle("Position []");
          }
          if (measurement_type == "a") {
	          grp->GetYaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 2: // X angle @ target
          grp = new TGraphErrors(time, theta_x, timeerr, theta_xerr);
          grp_title = "X Angle";
          if (measurement_type == "y") {
            grp->GetYaxis()->SetTitle("Angle []");
          }
          if (measurement_type == "a") {
            grp->GetYaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 3: // Y angle @ target
          grp = new TGraphErrors(time, theta_y, timeerr, theta_yerr);
          grp_title = "Y Angle";
          if (measurement_type == "y") {
	          grp->GetYaxis()->SetTitle("Angle []");
          }
          if (measurement_type == "a") {
	          grp->GetYaxis()->SetTitle("Difference []");
            grp_title += " Difference";
          }
          break;
        case 4: // Charge @ target
          grp = new TGraphErrors(time, q, timeerr, qerr);
          grp_title = "Charge";
          if (measurement_type == "y") {
	          grp->GetYaxis()->SetTitle("Charge []");
          }
          if (measurement_type == "a") {
	          grp->GetYaxis()->SetTitle("Asymmetry []");
            grp_title += " Asymmetry";
          }
          break;
        case 5: // Energy @ target
          grp = new TGraphErrors(time, e, timeerr, eerr);
          grp_title = "Energy";
          if (measurement_type == "y") {
	          grp->GetYaxis()->SetTitle("Energy []");
          }
          if (measurement_type == "a") {
	          grp->GetYaxis()->SetTitle("Asymmetry? []");
            grp_title += " Asymmetry?";
          }
          break;
        default:
          break;
      }

      grp->SetTitle(grp_title.c_str());

      grp->GetXaxis()->SetTitle("Run Number");
      grp->GetXaxis()->CenterTitle();
  	  grp->GetXaxis()->SetTitleSize(0.04);
  	  grp->GetXaxis()->SetLabelSize(0.04);
  	  grp->GetXaxis()->SetTitleOffset(1.25);
   	  grp->GetYaxis()->CenterTitle();
	    grp->GetYaxis()->SetTitleSize(0.04);
	    grp->GetYaxis()->SetLabelSize(0.04);
	    grp->GetYaxis()->SetTitleOffset(1.5);

      GraphArray.Add(grp);
    }

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

void QwGUIDatabase::DetectorPlot()
{

  if(dDatabaseCont){

//    ClearData();

    TGraphErrors *grp;

    dDatabaseCont->Connect();

    mysqlpp::Query query = dDatabaseCont->Query();

    // Subblocks are numbered 0-4 in database just like entry number in dCmbSubblock
    Int_t subblock = dCmbSubblock->GetSelected();
    Int_t run_first = dNumStartRun->GetIntNumber();
    Int_t run_last = dNumStopRun->GetIntNumber();
    string detector = DetectorCombos[dCmbDetector->GetSelected()];
    string measurement_type = DetectorMeasurementTypes[dCmbMeasurementType->GetSelected()];

    if (dCmbInstrument->GetSelected() == ID_MD) {
      query << "SELECT * FROM summary_d" << measurement_type << "_";
      detector = DetectorCombos[dCmbDetector->GetSelected()];
    }
    if (dCmbInstrument->GetSelected() == ID_LUMI) {
      query << "SELECT * FROM summary_l" << measurement_type << "_";
      detector = LumiCombos[dCmbDetector->GetSelected()];
    }
    query << "calc ";
    query << "WHERE subblock = " << subblock << " ";
    query << "AND detector = '" << detector << "' ";
    query << "AND run_number BETWEEN ";
    query << run_first << " AND ";
    query << run_last << " ";
    query << "ORDER BY run_number, segment_number";
    vector<QwParityDB::summary_dy_calc> res;
    query.storein(res);

    printf("Number of rows returned:  %ld\n",res.size());

    Int_t res_size = res.size();

    // Loop over all rows
    TVectorF x(res_size), xerr(res_size), y(res_size), yerr(res_size);
    Float_t run_number;
    Int_t segment_number;
    Int_t i = 0;
    vector<QwParityDB::summary_dy_calc>::iterator it;
    for (it = res.begin(); it != res.end(); ++it) {
      run_number = it->run_number;
      if (!it->segment_number.is_null) 
        segment_number = it->segment_number.data;
      Float_t f_segment_number = segment_number;
      x[i] = run_number + f_segment_number/100;
      xerr[i] = 0;
      if (!it->value.is_null)
        y[i] = it->value.data;
      if (!it->error.is_null)
        yerr[i] = it->error.data;

//      printf("i = %d, run_number = %f, segment_number = %d, f_segment_number = %f, x = %f, xerr = %f, y = %f, yerr = %f \n", i, run_number, segment_number, f_segment_number, x[i], xerr[i], y[i], yerr[i]);
      i++;
    }

    dDatabaseCont->Disconnect(); 

    grp = new TGraphErrors(x, y, xerr, yerr);

    string grp_title = "Detector = ";
    grp_title += detector;
    grp_title += " Type = ";
    grp_title += measurement_type;
    grp_title += " Subblock = ";
    grp_title +=  Subblocks[subblock];

    grp->SetTitle(grp_title.c_str());
    grp->GetXaxis()->SetTitle("Run Number");
    grp->GetXaxis()->CenterTitle();
	  grp->GetXaxis()->SetTitleSize(0.04);
	  grp->GetXaxis()->SetLabelSize(0.04);
	  grp->GetXaxis()->SetTitleOffset(1.25);
    if (measurement_type == "y") {
	    grp->GetYaxis()->SetTitle("Yield []");
    } 
    if (measurement_type == "a") {
	    grp->GetYaxis()->SetTitle("Asymmetry [ppb]");
    }
	  grp->GetYaxis()->CenterTitle();
	  grp->GetYaxis()->SetTitleSize(0.04);
	  grp->GetYaxis()->SetLabelSize(0.04);
	  grp->GetYaxis()->SetTitleOffset(1.5);

    GraphArray.Add(grp);

  TCanvas *mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->cd();
    grp->Draw("ap*");

  mc->Modified();
  mc->Update();

//    PlotGraphs();

  }

} // QwGUIDatabase::DetectorPlot()


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



