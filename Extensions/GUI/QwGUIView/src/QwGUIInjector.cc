#include "QwGUIInjector.h"
#include <TG3DLine.h>
ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POSSITION,
  BA_CHARGE,
  BA_CHARGE_VAR
};

const char *QwGUIInjector::InjectorTrees[INJECTOR_DET_TRE_NUM] = 
  {
    "HEL_Tree",
    "MPS_Tree"
  };

const char *QwGUIInjector::InjectorBPM[INJECTOR_BPM_NUM]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05"
//    "qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
//    "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
//    "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
//    "qwk_0r06"

    };

const char *QwGUIInjector::InjectorBCM[INJECTOR_BCM_NUM]=
  {"qwk_bcm0l02","qwk_1i02WSum","qwk_1i04WSum","qwk_1i06WSum","qwk_0i02WSum","qwk_0i02aWSum",
   "qwk_0i05WSum"
    };



QwGUIInjector::QwGUIInjector(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dControlsFrame = NULL;
  dCanvas = NULL;  
  dBtnLayout = NULL;
  dButtonPos = NULL;
  dButtonCharge = NULL;
  AddThisTab(this);

}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)       delete dTabFrame;
  if(dControlsFrame)  delete dControlsFrame;
  if(dCanvas)         delete dCanvas;  
  if(dBtnLayout)      delete dBtnLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIInjector::MakeLayout()
{

  SetCleanup(kDeepCleanup);

  dTabFrame= new  TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));


  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));
  
  TGVertical3DLine *separator = new TGVertical3DLine(this);
  dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 10, 10, 10, 10));

 
  //dControlsFrame->AddFrame( dButtonPos, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 0, 5));
  
 //  TGGroupFrame *group = new TGGroupFrame(dControlsFrame, "Controls");
//   group->SetTitlePos(TGGroupFrame::kCenter);

  dButtonPos = new TGTextButton(dControlsFrame, "&Beam Position Differences", BA_POSSITION);
  dButtonCharge = new TGTextButton(dControlsFrame, "&Beam Charge Asymmetries", BA_CHARGE);
  dButtonPosVariation = new TGTextButton(dControlsFrame, "&Beam Position Variation", BA_CHARGE_VAR);
  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  dControlsFrame->AddFrame(dButtonPos,dBtnLayout );
  dControlsFrame->AddFrame(dButtonCharge,dBtnLayout);
  dControlsFrame->AddFrame(dButtonPosVariation, dBtnLayout);
  
  // dControlsFrame->AddFrame(group,new TGLayoutHints(kLHintsExpandX));


  dButtonPos -> Associate(this);
  dButtonCharge -> Associate(this);
  dButtonPosVariation -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUIInjector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

 //  TCanvas *mc = dCanvas->GetCanvas();
//   mc->Divide( 2, 4);

  Int_t wid = dCanvas->GetCanvasWindowId();
  TSuperCanvas *super_canvas = new TSuperCanvas("", 10, 10, wid);
  dCanvas->AdoptCanvas(super_canvas);
  super_canvas -> Divide(2,4);

}

void QwGUIInjector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIInjector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
    printf("Called QwGUIInjector::OnObjClose\n");

    dROOTCont = NULL;
  }
}


void QwGUIInjector::OnNewDataContainer()
{

  TObject *obj;
  TObject *copy;
 
  ClearData();

  if(dROOTCont){
    for(int p = 0; p < INJECTOR_DET_TRE_NUM; p++) {
      obj = dROOTCont->ReadTree(InjectorTrees[p]);
      if(obj)
	{
	  copy = obj->Clone();
	  HistArray.Add(copy);
	}
    }
    
    // PlotPosData();
  }
};

void QwGUIInjector::OnRemoveThisTab()
{

};

void QwGUIInjector::ClearData()
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


void QwGUIInjector::PlotPosData(){

  bool ldebug = false;

  TObject *obj;
  TCanvas *mc = dCanvas->GetCanvas();
  
  mc->Divide(4,3);

  obj = HistArray.At(1);  // Get MPS tree
  if(ldebug) printf("%s\n", obj->GetName());
  
  char histo[128];
  for(Short_t p = 0; p <INJECTOR_BPM_NUM ; p++) 
    {
    
      sprintf (histo, "%sRelX.hw_sum",InjectorBPM[p] );
      if( ((TTree*) obj)->FindLeaf(histo))
	{
	  if(ldebug) printf("%s\n",InjectorBPM[p]);
	  mc -> cd(p+1);
	  obj -> Draw(histo);
	}
      sprintf (histo, "%sRelY.hw_sum",InjectorBPM[p] );
      if( ((TTree*) obj)->FindLeaf(histo))
	{
	  if(ldebug) printf("%s\n",InjectorBPM[p]);
	  mc -> cd(2*(p+1));
	  obj -> Draw(histo);
	}

   }  

  mc->Modified();
  mc->Update();
}



void QwGUIInjector::PlotChargeData(){

  bool ldebug = false;
 
  TObject *objc;
  TCanvas *mc = dCanvas->GetCanvas();

  mc->Divide(4,3);
  
  objc = HistArray.At(0);  // Get HEL tree
  
  if(ldebug) printf("%s\n", objc->GetName());
  
  char histoc[128];

  for(Short_t k = 0; k <INJECTOR_BCM_NUM ; k++) 
    {
    
      sprintf (histoc, "asym_%s.hw_sum",InjectorBCM[k] );
      if(((TTree*)objc)->FindLeaf(histoc))
	{
	  if(ldebug) printf("%s\n",InjectorBCM[k]);
	  mc -> cd(k+1);
	  objc -> Draw(histoc);
	}
    }  

  mc->Modified();
  mc->Update();
}


void QwGUIInjector::CompareCharge(){

 //  bool ldebug = false;

//   TObject *obj;
//   TCanvas *mc = dCanvas->GetCanvas();
  
//   Double_t mena_charge[INJECTOR_BPM_NUM];
//   Double_t relx[INJECTOR_BPM_NUM], rely[INJECTOR_BPM_NUM],
//     erx[INJECTOR_BPM_NUM],ery[INJECTOR_BPM_NUM],
//     err[INJECTOR_BPM_NUM],name[INJECTOR_BPM_NUM+1];

//   //mc->Divide(INJECTOR_BPM_NUM/2,INJECTOR_BPM_NUM/2);

//   obj = HistArray.At(1);  // Get MPS tree
//   if(ldebug) printf("%s\n", obj->GetName());
  
//   for(Short_t p = 0; p <INJECTOR_BPM_NUM ; p++) 
//     {
//       char * histo;
//       sprintf (histo, "%sRelX.hw_sum>>htemp",InjectorBPM[p] );
//       if( ((TTree*) obj)->FindLeaf(histo))
// 	{
// 	  if(ldebug) printf("%s\n",InjectorBCM[p]);
// 	  mc -> cd(p+1);
// 	  obj -> Draw(histo);
// 	  TH1* htemp = mc->GetPrimitive(histo);
// 	  //(TH1D*) htemp = (TH1*)(gDirectory)->GetObject("histo");
// 	  relx[p]=htemp->GetMean();
// 	  erx[p]=(htemp->GetRMS())/sqrt(htemp->GetEntries());
// 	}
      
//       sprintf (histo, "%sRelY.hw_sum>>htemp",InjectorBPM[p] );
//       if( ((TTree*) obj)->FindLeaf(histo))
// 	{
// 	  if(ldebug) printf("%s\n",InjectorBPM[p]);
// 	  mc -> cd(2*(p+1));
// 	  obj -> Draw(histo);
// 	  rel[p]=htemp->GetMean();
// 	  ery[p]=(htemp->GetRMS())/sqrt(htemp->GetEntries());
// 	}
//       name[p]+=0;
//     }  
  
//   gx  = new TGraphErrors(INJECTOR_BPM_NUM,name,relx,err,erx);
//   //gx = new TGraph(21,name,relx);
//   gx->SetTitle("#Delta x Variation");
//   gx->GetYaxis()->SetTitle("#Delta x (#mum)");
//   gx->SetMarkerStyle(8);
//   gx->SetMarkerSize(0.8);

 
//   gy  = new TGraphErrors(ndevices,name,rely,err,ery);
//   //gy = new TGraph(ndevices,name,rely);
//   gy->SetTitle("#Delta y Variation");
//   gy->GetYaxis()->SetTitle("#Delta y(#mum)");
//   gy->SetMarkerStyle(8);
//   gy->SetMarkerSize(0.8);

//   for (Int_t j=1;j<=ndevices;j++)     
//     {
//       gx->GetXaxis()->SetBinLabel(4.5*j,devicelist[j-1]);
//       gy->GetXaxis()->SetBinLabel(4.5*j,devicelist[j-1]); 
//     } 
  
//   c2->cd(2); 
//   gx->Draw("ap");

//   mc->Modified();
//   mc->Update();
}



void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= INJECTOR_BCM_NUM)
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


Bool_t QwGUIInjector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  // Process events generated by the object in the frame.
  
  switch (GET_MSG(msg))
    {
    case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) 
	{
	case kTE_ENTER:
	  switch (parm1) 
	    {
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
	      TCanvas *mc = dCanvas->GetCanvas();
	      TIter next(HistArray.MakeIterator());
	      
	      switch(parm1)
		{
		case   BA_POSSITION:
		  //printf("text button id %ld pressed\n", parm1);	
		  mc->Clear();  
		  PlotPosData();
		  mc->Update();
		  break;
		  
		case BA_CHARGE:
		  
		  //printf("text button id %ld pressed\n", parm1);
		  mc->Clear(); 
		  PlotChargeData();
		  mc->Update();
		  break;
		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
	      case M_TBIN_SELECT:
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
