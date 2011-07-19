///////////////////////////////////////////////////////////////////
//  Macro to check each adc
//    B. Moffit  Sept. 2003
//   usage:  .x macro/adc_check(int runnumber)
//    Automatically uses "standard" as the analysis type

#define MAX_ADCS 30;
#define DEBUG
UInt_t adc_id[MAX_ADCS];
UInt_t adcs_found;

TTree raw;

class ADCViewer {

  RQ_OBJECT("ADCViewer")

    private:
  TGMainFrame            *fMain;
  TRootEmbeddedCanvas    *fEcanvas;
  TGRadioButton          *fRadioADC[MAX_ADCS];
  TGRadioButton          *fRadioVS[3];
  TGCheckButton          *fCalib;

public:
  ADCViewer(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~ADCViewer();
  void DoDraw();
  void DoDrawCSR();
  void DoRadio();
};

ADCViewer::ADCViewer(const TGWindow *p, UInt_t w, UInt_t h)
{
  // Create the main frame
  fMain = new TGMainFrame(p,w,h);

  // Top frame, to hold adcs and canvas
  TGHorizontalFrame *fTopframe = new TGHorizontalFrame(fMain,200,200);
  fMain->AddFrame(fTopframe, new TGLayoutHints(kLHintsExpandX 
					      | kLHintsExpandY,10,10,10,1));

  // Create a verticle frame widget with radio buttons
  TGVerticalFrame *vframe = new TGVerticalFrame(fTopframe,40,200);
  TString buff;
  for(int i=0; i<adcs_found; i++) {
    buff = "adc";
    buff += adc_id[i];
    fRadioADC[i] = new TGRadioButton(vframe,buff,i);
  }
  fRadioADC[0]->SetState(kButtonDown);

  for (int i=0; i<adcs_found; i++) {
    vframe->AddFrame(fRadioADC[i], new TGLayoutHints(kLHintsLeft,5,5,3,4));
    fRadioADC[i]->Connect("Pressed()", "ADCViewer", this, "DoRadio()");
  }
  fTopframe->AddFrame(vframe,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  
  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas", fTopframe, 800, 600);
  fTopframe->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandY,10,10,10,1));

  // Create the bottom frame.  Contains buttons and check buttons
  TGHorizontalFrame *fBottomFrame = new TGHorizontalFrame(fMain,800,200);
  fMain->AddFrame(fBottomFrame, new TGLayoutHints(kLHintsExpandX,10,10,10,1));
  
  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *hframeL = new TGHorizontalFrame(fBottomFrame,400,40);
  TGHorizontalFrame *hframeR = new TGHorizontalFrame(fBottomFrame,400,40);

  fCalib = new TGCheckButton(hframeL,"Calibrated");
  fCalib->Connect("Pressed()", "ADCViewer", this, "DoDraw()");
  hframeL->AddFrame(fCalib, new TGLayoutHints(kLHintsCenterX,10,8,3,4));

  fRadioVS[0] = new TGRadioButton(hframeL,"1-D",30);
  fRadioVS[0]->Connect("Pressed()", "ADCViewer", this, "DoRadio()");
  hframeL->AddFrame(fRadioVS[0], new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fRadioVS[1] = new TGRadioButton(hframeL,"vs. Ev#",31);
  fRadioVS[1]->Connect("Pressed()", "ADCViewer", this, "DoRadio()");
  hframeL->AddFrame(fRadioVS[1], new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fRadioVS[2] = new TGRadioButton(hframeL,"vs. DAC",32);
  fRadioVS[2]->Connect("Pressed()", "ADCViewer", this, "DoRadio()");
  hframeL->AddFrame(fRadioVS[2], new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fRadioVS[0]->SetState(kButtonDown);

  TGTextButton *draw = new TGTextButton(hframeR,"&Draw");
  draw->Connect("Clicked()","ADCViewer",this,"DoDraw()");
  hframeR->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *drawcsr = new TGTextButton(hframeR,"C&SRs");
  drawcsr->Connect("Clicked()","ADCViewer",this,"DoDrawCSR()");
  hframeR->AddFrame(drawcsr, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *exit = new TGTextButton(hframeR,"&Exit",
					"gApplication->Terminate(0)");
  hframeR->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fBottomFrame->AddFrame(hframeL,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  fBottomFrame->AddFrame(hframeR,new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  // Set a name to the main frame
  fMain->SetWindowName("ADC Viewer");
  
  // Map all sub windows to main frame
  fMain->MapSubwindows();
  
  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());
  
  // Map main frame
  fMain->MapWindow();

}

void ADCViewer::DoDraw()
{

  // Determines which radio button is selected, then draws
  gStyle->SetTitleH(0);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetNdivisions();
  gStyle->SetOptStat();

  TString buff = "adc";
  TString postbuff = "";
  
  if (fRadioVS[1]->GetState() == kButtonDown) {
    postbuff = ":ev_num";
  } else if (fRadioVS[2]->GetState() == kButtonDown) {
    postbuff = ":dac";
  }

  for (int i=0; i<adcs_found; i++) {
    if (fRadioADC[i]->GetState() == kButtonDown) {
      buff += adc_id[i];
      buff += "_";
      if (fRadioVS[2]->GetState() == kButtonDown) {
	postbuff += adc_id[i];
      }
      break;
    }
  }

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->Clear();
  fCanvas->Divide(2,2);

  TString drawbuff;
  for(int i=0; i<4; i++) {
    fCanvas->cd(i+1);
    drawbuff = buff;
    drawbuff += i;
    if (fCalib->GetState() == kButtonDown) {
      drawbuff += "_cal";
    }
#ifdef DEBUG
    cout << "DoDraw() drawbuff: " << drawbuff << endl;
#endif
    drawbuff += postbuff;
    R->Draw(drawbuff.Data());
  }

  fCanvas->Update();

}

void ADCViewer::DoDrawCSR()
{
  // Draws all of the ADC CSRs
  gStyle->SetTitleH(0.2);
  gStyle->SetLabelSize(0.1,"x");
  gStyle->SetNdivisions(3);
  gStyle->SetOptStat(0);


  TString buff;
  Int_t div = sqrt(adcs_found) + 1;

  TCanvas *fCanvas = fEcanvas->GetCanvas();
  fCanvas->Clear();
  fCanvas->Divide(div,div);
  for(int i=0; i<adcs_found; i++) {    
    buff = "csr";
    buff += adc_id[i];
    fCanvas->cd(i+1);
    R->Draw(buff.Data());
  }

  fCanvas->Update();
}

void ADCViewer::DoRadio()
{
  // Handle the radio buttons

  TGButton *btn = (TGButton *) gTQSender;
  Int_t id = btn->WidgetId();
  
  if (id >= 0 && id <= adcs_found) {  // Take care of "ADC" radios
    for (int i = 0; i < adcs_found; i++)
      if (fRadioADC[i]->WidgetId() != id)
	fRadioADC[i]->SetState(kButtonUp);
  } else {
    for (int i = 0; i < 3; i++)      // Take care of "vs" radios
      if (fRadioVS[i]->WidgetId() != id)
	fRadioVS[i]->SetState(kButtonUp);
  }

  DoDraw();
}

ADCViewer::~ADCViewer()
{
  fMain->Cleanup();
  delete fMain;
}



void adc_check(int run=0) {
  if (run==0) {
    cout << "Run number: ";
    cin >> run;
  }

  gROOT->LoadMacro("macro/open.macro");
  open(run,"standard");
  raw = (TTree*)gROOT.FindObject("R");

  adcs_found = getADCs(); 

#ifdef DEBUG
  for (int i=0; i<adcs_found; i++) {
    cout << "Index " << i << ": " << adc_id[i] << endl;
  }
#endif

  new ADCViewer(gClient->GetRoot(),200,200);

}

UInt_t getADCs() {
  //  Routine to obtain the ADC id number. 
  //  Simply iterates from n = 0 to 30, to determine if csr<n>
  //   exists in the "raw" tree.
  //  Fills the adc_id[] array with found ids and returns the number found.

  int found = 0;
  TString buff;

  for (int i = 0; i<MAX_ADCS; i++) {
    buff = "csr";
    buff += i;
    TBranch* tempBranch = R->GetBranch(buff);
    if (tempBranch != 0) {
      adc_id[found] = i;
      found++;      
    }
  }

  return found;
}

