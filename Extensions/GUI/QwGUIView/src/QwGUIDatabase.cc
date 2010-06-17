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

enum EQwGUIDatabaseWidgetIDs {
  NUM_START_RUN,
  NUM_STOP_RUN,
  CMB_DETECTOR,
  CMB_SUBBLOCK,
  CMB_MEASUREMENT_TYPE,
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

  dLabStartRun = new TGLabel(dControlsFrame, "First Run");
  dNumStartRun = new TGNumberEntry(dControlsFrame, 0, 5, NUM_START_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dLabStopRun = new TGLabel(dControlsFrame, "Last Run");
  dNumStopRun = new TGNumberEntry(dControlsFrame, 10000, 5, NUM_STOP_RUN, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  dCmbDetector = new TGComboBox(dControlsFrame, "Detector", CMB_DETECTOR);
  dCmbSubblock = new TGComboBox(dControlsFrame, "Subblock", CMB_SUBBLOCK);
  dCmbMeasurementType = new TGComboBox(dControlsFrame, "Measurement Type", CMB_MEASUREMENT_TYPE);
  dBtnSubmit = new TGTextButton(dControlsFrame, "&Submit", BTN_SUBMIT);

  dLabLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dNumLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 10, 10, 5, 5 );
  dBtnLayout = new TGLayoutHints( kLHintsCenterX | kLHintsTop, 0, 0, 5, 5 );

  dCmbDetector->Resize(150,20);
  dCmbSubblock->Resize(150,20);
  dCmbMeasurementType->Resize(150,20);

  for (Int_t i = 0; i < N_DETECTORS; i++) {
    dCmbDetector->AddEntry(DetectorCombos[i], i);
  }
  for (Int_t i = 0; i < N_SUBBLOCKS; i++) {
    dCmbSubblock->AddEntry(Subblocks[i], i);
  }
  for (Int_t i = 0; i < N_DET_MEAS_TYPES; i++) {
    dCmbMeasurementType->AddEntry(DetectorMeasurementTypes[i], i);
  }

//  dCmbDetector->Select(0);
//  dCmbSubblock->Select(0);
//  dCmbMeasurementType->Select(0);

  dControlsFrame->AddFrame(dLabStartRun, dLabLayout );
  dControlsFrame->AddFrame(dNumStartRun, dNumLayout );
  dControlsFrame->AddFrame(dLabStopRun, dLabLayout );
  dControlsFrame->AddFrame(dNumStopRun, dNumLayout );
  dControlsFrame->AddFrame(dCmbDetector, dCmbLayout );
  dControlsFrame->AddFrame(dCmbSubblock, dCmbLayout );
  dControlsFrame->AddFrame(dCmbMeasurementType, dCmbLayout );
  dControlsFrame->AddFrame(dBtnSubmit, dBtnLayout);

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

  TGraph *grp = NULL;
 
  if(!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont){
    ClearData();
  
    dDatabaseCont->Connect();

    mysqlpp::Query query = dDatabaseCont->Query();


    query << "SELECT * FROM summary_da_calc where subblock = 0 and detector='combinationallmd' ORDER BY run_number, segment_number";
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
      x[i] = run_number + segment_number/100;
      xerr[i] = 0;
      if (!it->value.is_null)
        y[i] = it->value.data;
      if (!it->error.is_null)
        yerr[i] = it->error.data;

      printf("i = %d, x = %f, xerr = %f, y = %f, yerr = %f \n", i, x[i], xerr[i], y[i], yerr[i]);
      i++;
    }

    dDatabaseCont->Disconnect(); 

    grp = new TGraphErrors(x, y, xerr, yerr);

    grp->GetXaxis()->SetTitle("Run Number");
    grp->GetXaxis()->CenterTitle();
	  grp->GetXaxis()->SetTitleSize(0.04);
	  grp->GetXaxis()->SetLabelSize(0.04);
	  grp->GetXaxis()->SetTitleOffset(1.25);
	  grp->GetYaxis()->SetTitle("Asymmetry [ppb]");
	  grp->GetYaxis()->CenterTitle();
	  grp->GetYaxis()->SetTitleSize(0.04);
	  grp->GetYaxis()->SetLabelSize(0.04);
	  grp->GetYaxis()->SetTitleOffset(1.5);

    GraphArray.Add(grp);

  TCanvas *mc = dCanvas->GetCanvas();
  mc->cd();
    grp->Draw("ap");

  mc->Modified();
  mc->Update();

//    PlotGraphs();

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


void QwGUIDatabase::PositionDifferences()
{

  /*
  Bool_t ldebug = kFALSE;
  TObject *obj = NULL;
  TCanvas *mc = NULL;
  TH1D *dummyhist = NULL;
  TPaveText * errlabel;
  Bool_t status = kTRUE;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);

  // check to see if the TH1s used for the calculation are empty.
  for(Short_t i=0;i<2;i++) 
    {
      if(PosDiffVar[i]) delete PosDiffVar[i];  PosDiffVar[i] = NULL;
    }


  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(1,2);

  // Get HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return; 

  if(ldebug) {
    printf("PositionDiffercences -------------------------------\n");
    printf("Found th tree named %s \n", obj->GetName());
  }

  char histo[128];
  PosDiffVar[0] = new TH1D("dxvar", "#Delta X Variation", HCLINE_DEV_NUM, 0.0, HCLINE_DEV_NUM);
  PosDiffVar[1] = new TH1D("dyvar", "#Delta Y variation", HCLINE_DEV_NUM, 0.0, HCLINE_DEV_NUM);
 
  for(Int_t p = 0; p <HCLINE_DEV_NUM ; p++) 
    {
      sprintf (histo, "diff_%sRelX.hw_sum", HallCBeamlineDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	  obj -> Draw(histo);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetName(histo);
	  PosDiffVar[0] -> SetBinContent(p+1, dummyhist->GetMean()*1000);
	  PosDiffVar[0] -> SetBinError  (p+1, dummyhist->GetMeanError()*1000);
	  PosDiffVar[0] -> GetXaxis()->SetBinLabel(p+1,HallCBeamlineDevices[p]);
	  PosDiffVar[0]->SetStats(0);
	  SummaryHist(dummyhist);
	  delete dummyhist; dummyhist= NULL;
	}
      else 
	{
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  status = kFALSE;
	}

      
      sprintf (histo, "diff_%sRelY.hw_sum", HallCBeamlineDevices[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
	  obj -> Draw(histo);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetName(histo);
	  PosDiffVar[1] -> SetBinContent(p+1, dummyhist->GetMean()*1000); //convert mum to nm
	  PosDiffVar[1] -> SetBinError  (p+1, dummyhist->GetMeanError()*1000);
	  PosDiffVar[1] -> GetXaxis()->SetBinLabel(p+1, HallCBeamlineDevices[p]);
	  PosDiffVar[1]->SetStats(0);
	  SummaryHist(dummyhist);
	  delete dummyhist; dummyhist= NULL;
	}
      else 
	{
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  status = kFALSE;
	}
    }

  
  if (!status) 
    {
      mc->Clear();
      errlabel ->Draw();
      mc->Update();
    }
  else
    {
      mc->Clear();
      mc->Divide(1,2);
      
      mc->cd(1);
      SummaryHist(PosDiffVar[0]);
      PosDiffVar[0] -> SetMarkerStyle(20);
      PosDiffVar[0] -> SetMarkerColor(2);
      PosDiffVar[0] -> SetTitle("#Delta X Variation");
      PosDiffVar[0] -> GetYaxis() -> SetTitle("#Delta X (nm)");
      PosDiffVar[0] -> GetXaxis() -> SetTitle("BPM X");
      PosDiffVar[0] -> Draw("E1");
      gPad->Update();
      
      
      mc->cd(2);
      SummaryHist(PosDiffVar[1]);
      PosDiffVar[1] -> SetMarkerStyle(20);
      PosDiffVar[1] -> SetMarkerColor(4);
      PosDiffVar[1] -> SetTitle("#Delta Y Variation");
      PosDiffVar[1] -> GetYaxis()-> SetTitle ("#Delta Y (nm)");
      PosDiffVar[1] -> GetXaxis() -> SetTitle("BPM Y");
      PosDiffVar[1] -> Draw("E1");
      gPad->Update();
 
  
      if(ldebug) printf("----------------------------------------------------\n");
      mc->Modified();
      mc->Update();
    }
  
  if(dummyhist) delete dummyhist;
  return;
}



void 
QwGUIDatabase::SummaryHist(TH1 *in)
{

  Double_t out[4] = {0.0};
  Double_t test   = 0.0;

  out[0] = in -> GetMean();
  out[1] = in -> GetMeanError();
  out[2] = in -> GetRMS();
  out[3] = in -> GetRMSError();
  test   = in -> GetRMS()/sqrt(in->GetEntries());

  printf("%sName%s", BOLD, NORMAL);
  printf("%22s", in->GetName());
  printf("  %sMean%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[0], NORMAL, BLUE, out[1], NORMAL);
  printf("  %sSD%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[2], NORMAL, GREEN, out[3], NORMAL);
  printf(" %sRMS/Sqrt(N)%s %s%+4.2e%s \n", BOLD, NORMAL, BLUE, test, NORMAL);
  */
  return;
};





// Display the beam postion and angle in X & Y, beam energy and beam charge on the target.
void QwGUIDatabase::DisplayTargetParameters()
{
  
  /*
  TH1D * t[5];
  char histo[128];
  TString xlabel;
  TObject *obj = NULL;
  Bool_t ldebug = kFALSE;
  TCanvas *mc = NULL;

  // create a label to display error messages
  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);


  // Beam energy is not calculated by the analyser yet. 
  const char * TgtBeamParameters[5]=
    {
      "targetX","targetY","targetXSlope","targetYSlope","average_charge"
    }; 


  // clear and divide the canvas
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,3);

  // Get the HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return;  
  if(ldebug) {
    printf("Target beam parameters-----------------------\n");
    printf("Found the tree named %s \n", obj->GetName());
  }

  // Delete the histograms if they exsist.
  for(Short_t i=0;i<5;i++) 
    {
      // if(t[i]) delete t[i]; 
      t[i] = NULL;
    }
  
  for(Int_t p = 0; p <5 ; p++) 
    {
      sprintf (histo, "yield_qwk_%s.hw_sum", TgtBeamParameters[p]);
      if( ((TTree*) obj)->FindLeaf(histo) )
	{
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);

	  if(strcmp( TgtBeamParameters[p],"average_charge") == 0) //convert to current. VQWK input impedance is 10kOhm.
	      sprintf (histo, "yield_qwk_%s.hw_sum*0.000076*100/(4*yield_qwk_%s.num_samples)", 
		       TgtBeamParameters[p],TgtBeamParameters[p]);
	 
	  mc->cd(p+1);
	  obj -> Draw(histo);
	  t[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
	  t[p] -> SetTitle(TgtBeamParameters[p]);
	  t[p] -> GetYaxis()->SetTitle("Quartets");
	  t[p] -> GetYaxis()->SetTitleSize(0.08);
	  t[p] -> GetYaxis()->CenterTitle();
	  t[p] -> GetXaxis()->CenterTitle();
	  t[p]->GetXaxis()->SetTitleOffset(0.9);
	  t[p]->GetYaxis()->SetTitleOffset(0.5);

	  if(strcmp( TgtBeamParameters[p],"average_charge") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("Current (#muA)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(42);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetX") == 0)
	    { 
	      t[p] -> GetXaxis()->SetTitle("X Position (#mum)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(34);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetY") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("Y Position (#mum)");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(46);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetXSlope") == 0)
	    {
	      t[p] -> GetXaxis()->SetTitle("X Slope");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(34);
	    }
	  else 	if(strcmp( TgtBeamParameters[p],"targetYSlope") == 0) 
	    {
	      t[p] -> GetXaxis()->SetTitle("Y Slope");
	      t[p] -> GetXaxis()->SetDecimals();
	      t[p] -> SetFillColor(46);
	    }
	  SummaryHist(t[p]);	      
	}
      else 
	{
	  mc->Clear();
	  errlabel->AddText(Form("Unable to find object %s !",histo));
	  errlabel->Draw();
	}
    }

  if(ldebug) printf("----------------------------------------------------\n");
  mc->Modified();
  mc->Update();

  */
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
            case CMB_DETECTOR:
            case CMB_SUBBLOCK:
            case CMB_MEASUREMENT_TYPE:
            default:
              break;
          } // Combo Box 
        case kCM_BUTTON:
          switch(parm1) {
            case BTN_SUBMIT:
            default:
              break;
          } // Button

        default:
          break;
      } // kC_COMMAND
    }
  
  return kTRUE;
}



