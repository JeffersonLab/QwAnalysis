#include <QwGUILumiDetector.h>
#include <iostream>
#include "TLine.h"

#include <TG3DLine.h>
#include "TGaxis.h"

ClassImp(QwGUILumiDetector);


enum QwGUILumiDetectorIndentificator {
  BA_DS_LUMI,
  BA_US_LUMI,
  BA_LUMI,
  BA_SCALER,
 
};
enum QwGUILumiDetectorComboBox {
  CMB_LUMI,
  CMB_SCALER,
};

//LUMI_DET_TYPES is the size of the enum
enum QwGUIHallCBeamlineDeviceTypes {
  UNKNOWN_TYPE,
  VQWK_LUMI,
  SCALER_LUMI,
};
const char *QwGUILumiDetector::LumiDetectorHists[LUMI_DET_HST_NUM] = 
  { "C20AXM_hw_raw","C20AXM_block2_raw",
    "AmpCh1_block1_raw","H00_YP_block1_raw"};

QwGUILumiDetector::QwGUILumiDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab, const char *objName, const char *mainname, UInt_t w, UInt_t h)

  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dBtnLayout = NULL;

  dButtonUser = NULL;
  dButtonDetail = NULL;

  dButtonUSLumi = NULL;
  dButtonDSLumi = NULL;
  dButtonLumiAccess = NULL;
  dButtonScalerAccess = NULL;
  dLumiFrame  = NULL;
  dSCALERFrame  = NULL;
  dComboBoxLUMI  = NULL;
  dComboBoxSCALER  = NULL;

  PosVariation[0] = NULL;
  PosVariation[1] = NULL;

  LoadHistoMapFile(Form("%s/Parity/prminput/qweak_lumi.map",gSystem->Getenv("QWANALYSIS")));


  AddThisTab(this);
  LoadLUMICombo();//Load LUMI list into the combo box
  LoadSCALERCombo();//Load LUMI_SCALER list into the combo box

}

QwGUILumiDetector::~QwGUILumiDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;
  if(dBtnLayout) delete dBtnLayout;
  if(dButtonUSLumi) delete dButtonUSLumi;
  if(dButtonDSLumi) delete dButtonDSLumi;
  if(dButtonLumiAccess) delete dButtonLumiAccess;
  if(dButtonScalerAccess) delete dButtonScalerAccess;
  if(dLumiFrame) delete dLumiFrame;
  if(dSCALERFrame) delete dSCALERFrame;
  if(dComboBoxLUMI) delete dComboBoxLUMI;
  if(dComboBoxSCALER) delete dComboBoxSCALER;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUILumiDetector::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | 
				 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				 0, 0, 1, 2);
  dBtnLayout = new TGLayoutHints(kLHintsExpandX | kLHintsTop,
				    5,5,5,5);



  dTabFrame = new TGHorizontalFrame(this);
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));

  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));


  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

  //hall c lumi access frame
  dLumiFrame= new TGVerticalFrame(dControlsFrame,50,100);
  //hall c lumi scaler access frame
  dSCALERFrame= new TGVerticalFrame(dControlsFrame,50,100);

  dButtonLumiAccess=new TGTextButton(dLumiFrame, "&Lumis Asym/Yield", BA_LUMI);
  dButtonLumiAccess->SetEnabled(kFALSE);
  dButtonScalerAccess=new TGTextButton(dSCALERFrame, "&Scaler Asym/Yield", BA_SCALER);
  dButtonScalerAccess->SetEnabled(kFALSE);
  

  dButtonUSLumi=new TGTextButton(dControlsFrame, "&US Lumis Means", BA_US_LUMI);
  dButtonDSLumi=new TGTextButton(dControlsFrame, "&DS Lumis Means", BA_DS_LUMI);

  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  

  dControlsFrame->AddFrame(dButtonUSLumi,dBtnLayout );
  dControlsFrame->AddFrame(dButtonDSLumi,dBtnLayout );
  dControlsFrame->AddFrame(dLumiFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dControlsFrame->AddFrame(dSCALERFrame,new TGLayoutHints( kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  

  //Add LUMI  combo box
  dComboBoxLUMI=new TGComboBox(dLumiFrame,CMB_LUMI);
  dComboBoxLUMI->Resize(100,20);//To make it better looking

  //Add SCALER  combo box
  dComboBoxSCALER=new TGComboBox(dSCALERFrame,CMB_SCALER);
  dComboBoxSCALER->Resize(100,20);//To make it better looking
  

  dLumiFrame->AddFrame(dComboBoxLUMI, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dLumiFrame->AddFrame(dButtonLumiAccess, new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5));
  dSCALERFrame->AddFrame(dComboBoxSCALER, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 10, 10, 5, 5));
  dSCALERFrame->AddFrame(dButtonScalerAccess, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

 
  dButtonUSLumi -> Associate(this);
  dButtonDSLumi -> Associate(this);
  dButtonLumiAccess -> Associate(this);
  dButtonScalerAccess -> Associate(this);
  dComboBoxLUMI -> Associate(this);
  dComboBoxSCALER -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIHallCBeamline",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

}

void QwGUILumiDetector::LoadHistoMapFile(TString mapfile){
  TString varname, varvalue;
  TString modtype, namech,dettype;
  Int_t count_names;
  fLUMIDevices.resize(LUMI_DET_TYPES);
  QwParameterFile mapstr(mapfile.Data());  //Open the file
  //fLUMIDevices.clear();
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    /*
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname=="name"){
	//printf("%s - %s \n",varname.Data(),varvalue.Data());
	fLUMIDevices.at(COMBINED).push_back(varvalue);
      }
    }
    else
    */{

      modtype   = mapstr.GetNextToken(", ").c_str();	// module type
      if (modtype=="VQWK" || modtype=="SCALER"){
	mapstr.GetNextToken(", ");	//slot number
	mapstr.GetNextToken(", ");	//channel number
	dettype=mapstr.GetNextToken(", ");	//type-purpose of the detector
	dettype.ToLower();
	namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
	namech.ToLower();

	if (dettype=="integrationpmt"){
	  //printf("%s - %s \n",modtype.Data(),namech.Data() );
	  fLUMIDevices.at(VQWK_LUMI).push_back(namech);
	}
	else if (dettype=="scalerpmt"){
	  //printf("%s - %s \n",dettype.Data(),namech.Data() );	  
	  fLUMIDevices.at(SCALER_LUMI).push_back(namech);
	}
      }

    }

  }

  //printf("no. of hallC devices %d \n",fLUMIDevices.size());
   printf(" Hall C LUMI  Device List\n" );
  for (Size_t i=0;i<fLUMIDevices.size();i++)
    for (Size_t j=0;j<fLUMIDevices.at(i).size();j++)
      printf("%s \n",fLUMIDevices.at(i).at(j).Data() );	  

};

void QwGUILumiDetector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUILumiDetector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
    dROOTCont = NULL;
  }
}

void QwGUILumiDetector::OnNewDataContainer()
{

}

void QwGUILumiDetector::OnRemoveThisTab()
{

}

void QwGUILumiDetector::ClearData()
{

  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();
  while(obj){    
    delete obj;
    obj = next();
  }
  HistArray.Clear();
}


void QwGUILumiDetector::PlotUSLumi(){
  printf("QwGUILumiDetector::PlotUSLumi() \n");
};

void QwGUILumiDetector::PlotDSLumi(){
   printf("QwGUILumiDetector::PlotDSLumi() \n");
};

void QwGUILumiDetector::SetComboIndex(Short_t cmb_id, Short_t id){
    if (cmb_id==CMB_LUMI)
      fCurrentLUMIIndex=id;

    if (cmb_id==CMB_SCALER)
      fCurrentSCALERIndex=id;
};

void QwGUILumiDetector::LoadLUMICombo(){
  dComboBoxLUMI->RemoveAll();
  printf("QwGUILumiDetector::LoadHCBCMCombo \n");
  for(Size_t i=0;i<fLUMIDevices.at(VQWK_LUMI).size();i++){
    dComboBoxLUMI->AddEntry(fLUMIDevices.at(VQWK_LUMI).at(i),i);
    printf("%s \n",fLUMIDevices.at(VQWK_LUMI).at(i).Data());
  }
  if (fLUMIDevices.at(VQWK_LUMI).size()>0)
    dButtonLumiAccess->SetEnabled(kTRUE);
  fCurrentLUMIIndex=-1;  
};

void QwGUILumiDetector::LoadSCALERCombo(){
  dComboBoxSCALER->RemoveAll();
  printf("QwGUILumiDetector::LoadHCBCMCombo \n");
  for(Size_t i=0;i<fLUMIDevices.at(SCALER_LUMI).size();i++){
    dComboBoxSCALER->AddEntry(fLUMIDevices.at(SCALER_LUMI).at(i),i);
    printf("%s \n",fLUMIDevices.at(VQWK_LUMI).at(i).Data());
  }
  if (fLUMIDevices.at(SCALER_LUMI).size()>0)
    dButtonScalerAccess->SetEnabled(kTRUE);
  fCurrentSCALERIndex=-1;    
};


void QwGUILumiDetector::PlotLumi(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  

   while (1){
     if (fCurrentLUMIIndex<0)
       break;
     sprintf (histo, "asym_%s_hw",fLUMIDevices.at(VQWK_LUMI).at(fCurrentLUMIIndex).Data() );
     histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
     sprintf (histo, "yield_%s_hw",fLUMIDevices.at(VQWK_LUMI).at(fCurrentLUMIIndex).Data() );
     histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
    
    if (histo1!=NULL || histo2!=NULL ) {
    
      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      //SummaryHist(histo1);
      mc->cd(2);
      histo2->Draw();
      //SummaryHist(histo2);
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   printf("---------------PlotLUMI()--------------------\n");
  //mc->Modified();
  //mc->Update(); 
};

void QwGUILumiDetector::PlotLumiScaler(){
  TH1F *histo1=NULL;
  TH1F *histo2=NULL;
  char histo[128]; //name of the histogram
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  

   while (1){
     if (fCurrentSCALERIndex<0)
       break;
     sprintf (histo, "asym_%s",fLUMIDevices.at(SCALER_LUMI).at(fCurrentSCALERIndex).Data() );
     histo1= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
     sprintf (histo, "yield_%s",fLUMIDevices.at(SCALER_LUMI).at(fCurrentSCALERIndex).Data() );
     histo2= (TH1F *)dROOTCont->GetObjFromMapFile(histo);
    
    if (histo1!=NULL || histo2!=NULL ) {
    
      mc->Clear();
      mc->Divide(1,2);

      mc->cd(1);
      histo1->Draw();
      //SummaryHist(histo1);
      mc->cd(2);
      histo2->Draw();
      //SummaryHist(histo2);
      gPad->Update();
      gPad->Update();

      mc->Modified();
      mc->Update();
    }

    gSystem->Sleep(100);
    if (gSystem->ProcessEvents()){
      break;
    }
  }
  

   printf("---------------PlotLumiScaler()--------------------\n");  
};

void QwGUILumiDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= LUMI_DET_HST_NUM)
      {
	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
						     GetNewWindowName(),"QwGUILumiDetector",
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


Bool_t QwGUILumiDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)){

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_ENTER:
      switch (parm1) {

      default:
	break;
      }

    default:
      break;
    }

  case kC_COMMAND:
    if(dROOTCont){
      switch (GET_SUBMSG(msg)) 
	{
      
	case kCM_BUTTON:
	  {
	    switch(parm1)
	      {
	      case BA_DS_LUMI:
		PlotDSLumi();
		break;
	      case BA_US_LUMI:
		PlotUSLumi();
		break;
	      case BA_LUMI:
		PlotLumi();
		break;
	      case BA_SCALER:
		PlotLumiScaler();
		break;
	      }

	    break;
	  }

	case kCM_COMBOBOX:
	  {
	    switch (parm1) {
	    case M_TBIN_SELECT:
	      break;
	    case CMB_LUMI:
	      SetComboIndex(CMB_LUMI,parm2);
	      break;
	    case CMB_SCALER:
	      SetComboIndex(CMB_SCALER,parm2);
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
    }else{
      std::cout<<"Please load the map file to view data. (Use Menubar->MemoryMap->Load Memory)\n";
    }
  
  default:
    break;
  }
  
  return kTRUE;
}
