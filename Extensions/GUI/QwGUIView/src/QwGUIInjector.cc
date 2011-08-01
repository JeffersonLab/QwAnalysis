#include "QwGUIInjector.h"

#include "TG3DLine.h"
#include "TGaxis.h"
#include "TLatex.h"

ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POS_DIFF,
  BA_CHARGE_ASYM,
  BA_PLOT_HISTOS,
  CMB_HISTOS
};

const char *QwGUIInjector::RootTrees[TRE_NUM] = 
  {
    "Hel_Tree",
    "Mps_Tree"
  };

enum EQwGUIDatabaseHistogramIDs {
  ID_XP,
  ID_XM,
  ID_YP,
  ID_YM,
  ID_EF_Q,
  ID_ABSX,
  ID_ABSY,
  ID_BCM_Q,
  ID_SC,
  ID_DX,
  ID_DY,
  ID_YIELDX,
  ID_YIELDY,
  ID_ASYM_EF_Q,
  ID_BCM_YIELD,
  ID_BCM_ASYM,
  ID_SC_YIELD,
  ID_SC_ASYM,

};

// The list of BPMs in the  Injector beamline. Please don't change this. 

const char *QwGUIInjector::Injector_BPMS[INJECTOR_BPMS]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05","qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
   "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
   "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
   "qwk_0r06"
  };



//BCMs were added to this list specifically for the effective charge it may or may not be used later
//Kaisen
const char *QwGUIInjector::Injector_BCMS[INJECTOR_BCMS]=
  {
    "qwk_bcm0l02"
  };


QwGUIInjector::QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dBtnLayout          = NULL;
  dBtnPosDiff         = NULL;
  dBtnChargeAsym        = NULL;
  dCmbHistos          = NULL;
  dBtnPlotHistos      = NULL;
  dCmbLayout          = NULL;

  PosDiffVar[0] = NULL;
  PosDiffVar[1] = NULL;
 
  HistArray.Clear();
  DataWindowArray.Clear();

  ReadyHistArray = false;

  AddThisTab(this);

}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;
  if(dBtnPosDiff)         delete dBtnPosDiff;
  if(dBtnChargeAsym)      delete dBtnChargeAsym;
  if(dCmbHistos)          delete dCmbHistos;
  if(dBtnPlotHistos)      delete dBtnPlotHistos; 
  if(dCmbLayout)          delete dCmbLayout;

  delete [] PosDiffVar;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIInjector::MakeLayout()
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

  dBtnPosDiff  = new TGTextButton(dControlsFrame, "&Position Difference Variation", BA_POS_DIFF);
  dBtnChargeAsym = new TGTextButton(dControlsFrame, "&Charge Asymmetry", BA_CHARGE_ASYM);
  dBtnPlotHistos = new TGTextButton(dControlsFrame, "&Plot Histograms", BA_PLOT_HISTOS);
  dCmbHistos = new TGComboBox(dControlsFrame, CMB_HISTOS);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dCmbLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  //Mps based
  dCmbHistos->AddEntry("XP Wire", ID_XP );
  dCmbHistos->AddEntry("XM Wire", ID_XM );
  dCmbHistos->AddEntry("YP Wire", ID_YP );
  dCmbHistos->AddEntry("YM Wire", ID_YM );
  dCmbHistos->AddEntry("Effective Charge", ID_EF_Q);
  dCmbHistos->AddEntry("AbsX", ID_ABSX);
  dCmbHistos->AddEntry("AbsY", ID_ABSY);
  dCmbHistos->AddEntry("bcm_charge", ID_BCM_Q );
  dCmbHistos->AddEntry("scalers", ID_SC );

  // Helicity based
  dCmbHistos->AddEntry("diff_X", ID_DX);
  dCmbHistos->AddEntry("diff_Y", ID_DY);
  dCmbHistos->AddEntry("yield_X", ID_YIELDX);
  dCmbHistos->AddEntry("yield_Y", ID_YIELDY);
  dCmbHistos->AddEntry("asym_Effective Charge", ID_ASYM_EF_Q);
  dCmbHistos->AddEntry("bcm_yield", ID_BCM_YIELD );
  dCmbHistos->AddEntry("sc_yield", ID_SC_YIELD );
  dCmbHistos->AddEntry("bcm_asyms", ID_BCM_ASYM );
  dCmbHistos->AddEntry("sc_asym", ID_SC_ASYM );

  dCmbHistos->Resize(150,20);

  dControlsFrame -> AddFrame(dBtnPosDiff ,dBtnLayout );
  dControlsFrame -> AddFrame(dBtnChargeAsym,dBtnLayout );
  dControlsFrame -> AddFrame(dCmbHistos,dCmbLayout );
  dControlsFrame -> AddFrame(dBtnPlotHistos,dBtnLayout );

  dBtnPosDiff    -> Associate(this);
  dBtnChargeAsym -> Associate(this);
  dBtnPlotHistos -> Associate(this);
  dCmbHistos     -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIInjector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);

  dCanvas->AdoptCanvas(mc);

  

}

void QwGUIInjector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIInjector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUIInjector::OnObjClose\n");

    dROOTCont = NULL;
  }
}


void QwGUIInjector::OnNewDataContainer(RDataContainer *cont)
{

  TObject *obj = NULL;
  TObject *copy = NULL;
  ClearData();

  if(dROOTCont){
    for(Int_t p = 0; p < TRE_NUM; p++) {
      obj = dROOTCont->ReadTree(RootTrees[p]);
    
      if(obj)
	{  
	  copy = obj->Clone();
	  HistArray.Add(copy);
	  ReadyHistArray = true;
	}
      else
	{
	  ReadyHistArray = false;
	}
    }
  }
  
  this->EnableButtons(ReadyHistArray);

};

void QwGUIInjector::OnRemoveThisTab()
{

};

void QwGUIInjector::ClearData()
{

  //  TObject *obj;
  //   TIter next(HistArray.MakeIterator());
  //   obj = next();
  //   while(obj){    
  //     delete obj;
  //     obj = next();
  //   }
  HistArray.Clear();//Clear();
}

//Stuff to do after the tab is selected
void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= INJECTOR_BPMS)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUIInjector",
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
}

/**
 Plot the pattern (e.g. quartet) based aboslute position differences for bpms X an Y.
*/

void QwGUIInjector::PositionDifferences()
{

  Bool_t ldebug = kFALSE;

  TObject *obj       = NULL;
  TCanvas *mc        = NULL;
  TH1D    *dummyhist = NULL;

  std::vector<Int_t> x_devices;
  std::vector<Int_t> y_devices; 

  std::vector<Double_t> x_mean;  
  std::vector<Double_t> x_err;
  std::vector<Double_t> y_mean;  
  std::vector<Double_t> y_err;

  TText* not_found  = new TText(); 
  not_found->SetTextColor(2);

  // check to see if the TH1s used for the calculation are empty.
  for(Int_t i=0;i<2;i++) {
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
    printf("Found in tree named %s \n", obj->GetName());
  }

  
  for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
    sprintf (histo, "diff_%sX.hw_sum", Injector_BPMS[p]);

    if( ((TTree*) obj)->FindLeaf(histo) ){
      x_devices.push_back(p);
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
      obj -> Draw(histo);
      dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
      dummyhist -> SetName(histo);
      x_mean.push_back(dummyhist->GetMean()*1000);
      x_err.push_back(dummyhist->GetMeanError()*1000);

      SummaryHist(dummyhist);
      delete dummyhist; dummyhist= NULL;
    }
    

    sprintf (histo, "diff_%sY.hw_sum", Injector_BPMS[p]);
    if( ((TTree*) obj)->FindLeaf(histo) ){   
      y_devices.push_back(p);
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
      obj -> Draw(histo);
      dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
      dummyhist -> SetName(histo);
      y_mean.push_back(dummyhist->GetMean()*1000);
      y_err.push_back(dummyhist->GetMeanError()*1000);

      SummaryHist(dummyhist);
      delete dummyhist; dummyhist= NULL;
    }
  }

  PosDiffVar[0] = new TH1D("dxvar", "#Delta X Variation", x_devices.size(), 0.5, x_devices.size()+0.5);
  PosDiffVar[1] = new TH1D("dyvar", "#Delta Y variation", y_devices.size(), 0.5, y_devices.size()+0.5);
  
  mc->Clear();
  mc->Divide(1,2);
  
  if(x_devices.size() !=0){
    for(size_t p = 0; p <x_devices.size() ; p++) {
      PosDiffVar[0] -> SetBinContent(p+1, x_mean[p]); // mm = 1000um
      PosDiffVar[0] -> SetBinError  (p+1, x_err[p]);
      PosDiffVar[0] -> GetXaxis()->SetBinLabel(p+1,Injector_BPMS[x_devices[p]]);
      PosDiffVar[0] -> SetStats(0);
    }
  }
  else{
    mc->cd(1);
    not_found ->DrawText(0.2,0.5,"None of the devices were found!");
    not_found->DrawClone();
    not_found->Clear();
    return;
  }


  if(y_devices.size() !=0){
    for(size_t p = 0; p <y_devices.size() ; p++) {
      PosDiffVar[1] -> SetBinContent(p+1, y_mean[p]); //mm -> um
      PosDiffVar[1] -> SetBinError  (p+1, y_err[p]);
      PosDiffVar[1] -> GetXaxis()->SetBinLabel(p+1, Injector_BPMS[y_devices[p]]);
      PosDiffVar[1]->SetStats(0);
    }
  }else{
    mc->cd(2);
    not_found ->DrawText(0.2,0.5,"None of the devices were found!");
    not_found->DrawClone();
    not_found->Clear();
    return;
  }

    mc->cd(1);
    SummaryHist(PosDiffVar[0]);
    PosDiffVar[0] -> SetMarkerStyle(20);
    PosDiffVar[0] -> SetMarkerColor(2);
    PosDiffVar[0] -> SetTitle("#Delta X Variation");
    PosDiffVar[0] -> GetYaxis() -> SetTitle("#Delta X (#mum)");
    PosDiffVar[0] -> GetXaxis() -> SetTitle("BPM names");
    PosDiffVar[0] -> Draw("E1");
    gPad->Update();
  
  
    mc->cd(2);
    SummaryHist(PosDiffVar[1]);
    PosDiffVar[1] -> SetMarkerStyle(20);
    PosDiffVar[1] -> SetMarkerColor(4);
    PosDiffVar[1] -> SetTitle("#Delta Y Variation");
    PosDiffVar[1] -> GetYaxis()-> SetTitle ("#Delta Y (#mum)");
    PosDiffVar[1] -> GetXaxis() -> SetTitle("BPM names");
    PosDiffVar[1] -> Draw("E1");
    gPad->Update();
    
    
    if(ldebug) printf("----------------------------------------------------\n");
    mc->Modified();
    mc->Update();
    
    if(dummyhist) delete dummyhist;
    x_devices.clear();
    y_devices.clear();
    x_mean.clear();
    x_err.clear();
    y_mean.clear();
    y_err.clear();

    return;
}



void 
QwGUIInjector::SummaryHist(TH1 *in)
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
  return;
};

/**
 Plot the pattern (e.g. quartet) based charge asymmetry from the effective charge of the bpms + bcm.
*/

void QwGUIInjector::DisplayChargeAsymmetry()
{
  Bool_t ldebug = kFALSE;

  TObject *obj       = NULL;
  TCanvas *mc        = NULL;
  TH1D    *dummyhist = NULL;

  TText* not_found  = new TText(); 
  not_found->SetTextColor(2);

  std::vector<Int_t> q_devices;

  std::vector<Double_t> q_mean;  
  std::vector<Double_t> q_err;
 
   // check to see if the TH1s used for the calculation are empty
  // if(ChargeAsym) delete ChargeAsym;  ChargeAsym = NULL;

  mc = dCanvas->GetCanvas();
  mc->Clear();

  // Get HEL tree
  obj = HistArray.At(0);  
  if( !obj ) return; 

  if(ldebug) {
    printf("Charge asymmetries -------------------------------\n");
    printf("Found in tree named %s \n", obj->GetName());
  }

  
  for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
    sprintf (histo, "asym_%s_EffectiveCharge.hw_sum", Injector_BPMS[p]);

    if( ((TTree*) obj)->FindLeaf(histo) ){
      q_devices.push_back(p);
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
      obj -> Draw(histo);
      dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
      dummyhist -> SetName(histo);
      q_mean.push_back(dummyhist->GetMean()*1e+6); //ppm
      q_err.push_back(dummyhist->GetMeanError()*1e+6);

      SummaryHist(dummyhist);
      delete dummyhist; dummyhist= NULL;
    } 
  }

  for(Int_t p = 0; p <INJECTOR_BCMS ; p++) {
    sprintf (histo, "asym_%s.hw_sum", Injector_BCMS[p]);
    
    if( ((TTree*) obj)->FindLeaf(histo) ){
      q_devices.push_back(p);
      if(ldebug) printf("Found %2d : a histogram name %22s\n", p+1, histo);
      obj -> Draw(histo);
      dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
      dummyhist -> SetName(histo);
      q_mean.push_back(dummyhist->GetMean()*1e+6); //ppm
      q_err.push_back(dummyhist->GetMeanError()*1e+6);

      SummaryHist(dummyhist);
      delete dummyhist; dummyhist= NULL;
    } 
  }

  if( q_devices.size() == 0 ){
    not_found ->DrawText(0.2,0.5,"No device was found!");
    not_found->Draw();
    return;
  }

  ChargeAsym = new TH1D("Aqvar", "#Charge Asymmetry Variation", q_devices.size(), 0.5, q_devices.size()+0.5);
   
  for(size_t p = 0; p <q_devices.size() ; p++) {
    ChargeAsym -> SetBinContent(p+1, q_mean[p]); 
    ChargeAsym -> SetBinError  (p+1, q_err[p]);

    if( p < (q_devices.size()-INJECTOR_BCMS))
      ChargeAsym -> GetXaxis()->SetBinLabel(p+1,Injector_BPMS[q_devices[p]]);
    else
      ChargeAsym -> GetXaxis()->SetBinLabel(p+1,Injector_BCMS[q_devices[p]]);

    ChargeAsym -> SetStats(0);
  }

    mc->Clear();

    SummaryHist(ChargeAsym);
    ChargeAsym -> SetMarkerStyle(20);
    ChargeAsym -> SetMarkerColor(2);
    ChargeAsym -> SetTitle("Charge asymmetry variation");
    ChargeAsym -> GetYaxis() -> SetTitle("A_q (ppm)");
    ChargeAsym -> Draw("E1");
    gPad->Update();
  
    if(ldebug) printf("----------------------------------------------------\n");
    mc->Modified();
    mc->Update();
    
    if(dummyhist) delete dummyhist;
    q_devices.clear();
    q_mean.clear();
    q_err.clear();

    return;
}


/**
 Plot the selcted option.
*/
void QwGUIInjector::PlotHistograms()
{ 
  gStyle->SetOptStat(kTRUE);

  Bool_t ldebug = kFALSE;

  TCanvas *mc        = NULL;
  TObject *obj       = NULL;	
  TH1D    *dummyhist = NULL;
  TString element;
  TText* not_found  = new TText(); 
  not_found->SetTextColor(2);

  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(4,6);
  //Plot MPS based variables
  /********************************************************************/
  if(select.Contains("MPS")){
   
    obj = HistArray.At(1);
    char histogram[128];  
    element = "";
  
    // For single wires
    if (select.Contains("XP")||select.Contains("XM")||select.Contains("YP")||select.Contains("YM")){

      element = select.Remove(0,4); 
      for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
	mc->cd(p+1);
	sprintf (histogram, "%s%s.hw_sum", Injector_BPMS[p],element.Data());
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  // obj -> Draw(histogram);	 
	  // dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  // dummyhist -> SetTitle(histogram);
	  // dummyhist -> GetYaxis() -> SetTitle("Events");
	  // dummyhist -> GetXaxis() -> SetTitle("ADC counts");
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	} 
      }
    } // absolute positions
    else if (select.Contains("ABSX")||select.Contains("ABSY")){

      element = select.Remove(0,7); 
      for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {      
	mc->cd(p+1);
	sprintf (histogram, "%s%s.hw_sum", Injector_BPMS[p],element.Data()); 
	if (ldebug) std::cout << "histogram = " << histogram << std::endl;
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp");
	  dummyhist -> SetTitle(histogram);
	  dummyhist -> GetYaxis() -> SetTitle("Events");
	  dummyhist -> GetXaxis() -> SetTitle("Position (mm)");
	}  
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    } // effective charge
    else if (select.Contains("EF_Q")){

      for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
	mc->cd(p+1);
	sprintf (histogram, "%s_EffectiveCharge.hw_sum", Injector_BPMS[p]);
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetTitle(histogram);
	  dummyhist -> GetYaxis() -> SetTitle("Events");
	  dummyhist -> GetXaxis() -> SetTitle("ADC counts");
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    } // bcms
    else if (select.Contains("BCM")){

      for(Int_t p = 0; p <INJECTOR_BCMS ; p++) {
	mc->cd(p+1);
	sprintf (histogram, "%s.hw_sum", Injector_BCMS[p]);
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetTitle(histogram);
	  dummyhist -> GetYaxis() -> SetTitle("Events");
	  dummyhist -> GetXaxis() -> SetTitle("Charge");
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    }
  } // Plot Hel tree based variables
  else if(select.Contains("HEL")){
  /********************************************************************/

    obj = HistArray.At(0);
    char histogram[128];  
    element = "";

    // For position differences
    if (select.Contains("DIFFX")||select.Contains("DIFFY")){

      element = select.Remove(0,8); 

      for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
	mc->cd(p+1);
	sprintf (histogram, "diff_%s%s.hw_sum", Injector_BPMS[p],element.Data());
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetTitle(histogram);
	  dummyhist -> GetYaxis() -> SetTitle("Quartets");
	  dummyhist -> GetXaxis() -> SetTitle("Position (mm)");
	}
	else {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    } // position yields
    else if (select.Contains("YIELDX")||select.Contains("YIELDY")){

      element = select.Remove(0,9); 
      for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
	mc->cd(p+1);
	sprintf (histogram, "yield_%s%s.hw_sum", Injector_BPMS[p],element.Data());
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetTitle(histogram);
	  dummyhist -> GetYaxis() -> SetTitle("Quartets");
	  dummyhist -> GetXaxis() -> SetTitle("Position (mm)");
	}
	else  {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    } // effective charge asymmetry
    else if (select.Contains("AEF_Q")){

      for(Int_t p = 0; p <INJECTOR_BPMS ; p++) {
	mc->cd(p+1);
	sprintf (histogram, "asym_%s_EffectiveCharge.hw_sum", Injector_BPMS[p]);
	if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	
	if( ((TTree*) obj)->FindLeaf(histogram) ) {
	  obj -> Draw(histogram);	 
	  dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	  dummyhist -> SetTitle(histogram);
	  dummyhist -> GetYaxis() -> SetTitle("Quartets");
	  dummyhist -> GetXaxis() -> SetTitle("Charge");
	}
	else  {
	  not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	  not_found->DrawClone();
	  not_found->Clear();
	}
      }
    }
    if (select.Contains("BCM")){
	for(Int_t p = 0; p <INJECTOR_BCMS ; p++) {
	  mc->cd(p+1);
	  if(select.Contains("YBCM"))
	    sprintf (histogram, "yield_%s.hw_sum", Injector_BCMS[p]);
	  else if (select.Contains("DBCM"))
	    sprintf (histogram, "diff_%s.hw_sum", Injector_BCMS[p]);
	  else if(select.Contains("ABCM"))
	    sprintf (histogram, "asym_%s.hw_sum", Injector_BCMS[p]); 
	  if (ldebug) std::cout << "histogram = " << histogram <<  std::endl;
	  if( ((TTree*) obj)->FindLeaf(histogram) ) {
	    obj -> Draw(histogram);	 
	    dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	    dummyhist -> SetTitle(histogram);
	    dummyhist -> GetYaxis() -> SetTitle("Quartets");
	    dummyhist -> GetXaxis() -> SetTitle("Position (mm)");
	  }
	  else {
	    not_found ->DrawText(0.2,0.5,Form("%s not found!", histogram));
	    not_found->DrawClone();
	    not_found->Clear();
	  }
	}
    }
  }
  
  select="";

  mc->Modified();
  mc->Update();
  return;
}


void QwGUIInjector::EnableButtons(Bool_t flag)
{
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  
  if(not flag) {
 
    TLatex text;
    text.SetTextAlign(23);
    text.SetTextSize(0.04);
    text.DrawLatex(0.5,0.6,"No Trees that you are looking for in the selected ROOT file.");
    text.DrawLatex(0.5,0.4,"Please select the right ROOT file.");
    mc->Update();
  }

  dBtnPosDiff    -> SetEnabled(flag);   
  dBtnChargeAsym -> SetEnabled(flag);
  dBtnPlotHistos -> SetEnabled(flag);
  dCmbHistos     -> SetEnabled(flag);
  return;
}

// Process events generated by the object in the frame.
Bool_t QwGUIInjector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

  switch (GET_MSG(msg))
    {
    // case kC_TEXTENTRY:
    //   switch (GET_SUBMSG(msg)) 
    // 	{
    // 	case kTE_ENTER:
    // 	  switch (parm1) 
    // 	    {
    // 	    default:
    // 	      break;
    // 	    }
    // 	default:
    // 	  break;
    // 	}
      
    case kC_COMMAND:
      if(dROOTCont){
	switch (GET_SUBMSG(msg))
	  {
	  case kCM_BUTTON:
	    {
	      switch(parm1)
		{
		case   BA_POS_DIFF:
		  PositionDifferences();
		  break;
		  
		case BA_CHARGE_ASYM:
		  DisplayChargeAsymmetry();
		  break;

		case BA_PLOT_HISTOS:
		  PlotHistograms();
		  break;
		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      // switch (parm1) {
	      // case M_TBIN_SELECT:
	      // break;
	      // }
	      switch (dCmbHistos ->GetSelected()) {
	      case ID_XP:
		select="MPS_XP";
		break;
	      case ID_XM:
		select="MPS_XM";
		break;
	      case ID_YP:
		select="MPS_YP";
		break;
	      case ID_YM:
		select="MPS_YM";
		break;
	      case ID_EF_Q:
		select="MPS_EF_Q";
		break;
	      case ID_SC:
		select="MPS_SC";
		break;
	      case ID_ABSX:
		select="MPS_ABSX";
		break;	      
	      case ID_ABSY:
		select="MPS_ABSY";
		break;
	      case ID_BCM_Q:
		select="MPS_BCM_Q";
		break;
	      case ID_DX:
		select="HEL_DIFFX";
		break;
	      case ID_DY:
		select="HEL_DIFFY";
		break;
	      case ID_YIELDX:
		select="HEL_YIELDX";
		break;
	      case ID_YIELDY:
		select="HEL_YIELDY";
		break;
	      case ID_ASYM_EF_Q:
		select="HEL_AEF_Q";
		break;
	      case ID_BCM_YIELD:
		select="HEL_YBCM";
		break;
	      case ID_BCM_ASYM:
		select="HEL_ABCM";
		break;
	      case ID_SC_YIELD:
		select="HEL_SC_YIELD";
		break;
	      case ID_SC_ASYM:
		select="HEL_SC_ASYM";
		break;
	      default:
		break;
	      }
	    }
	    break;
	    
	  case kCM_MENUSELECT:
	    break;
	    
	  case kCM_MENU:
	    
	    switch (parm1) {
	      
	    case M_FILE_OPEN:
	      break;
	      
	      
	      
	  default:
	    break;
	  }
	  
	default:
	  break;
	}
      }
      else{
	std::cout<<"Please open a root file to view data. \n";
      }
      default:
	break;
    }
  
  return kTRUE;
}



