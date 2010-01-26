#include "QwGUIInjector.h"

ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POSSITION,
  BA_CHARGE
};

const char *QwGUIInjector::InjectorTrees[INJECTOR_DET_TRE_NUM] = 
  {
    "HEL_Tree",
    "MPS_Tree"
  };

const char *QwGUIInjector::InjectorBPM[INJECTOR_BPM_NUM]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05"
   //   "qwk_0i07","qwk_0l01","qwk_0l02","qwk_0l03",
//      "qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07","qwk_0l08",
//      "qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02","qwk_0r05",
//      "qwk_0r06

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
  dSubFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dSubLayout = NULL;
  dBtnLayout = NULL;
  dButtonPos = NULL;
  dButtonCharge = NULL;

  AddThisTab(this);

}

QwGUIInjector::~QwGUIInjector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dSubFrame)  delete dSubFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;
  if(dBtnLayout) delete dBtnLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIInjector::MakeLayout()
{

  dTabLayout = new TGLayoutHints( kLHintsTop | kLHintsExpandX | kLHintsExpandY);  
  dCnvLayout = new TGLayoutHints( kLHintsTop | kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10);
  // dSubLayout = new TGLayoutHints( kLHintsCenterX| kLHintsCenterY, 10, 10, 10, 10);
  dBtnLayout = new TGLayoutHints( kLHintsCenterX , 5,5,5,5);
  
 
  dTabFrame = new TGHorizontalFrame(this,10,10);  
  AddFrame(dTabFrame,dTabLayout); // Add tab to the main frame
 

  //dSubFrame = new TGVerticalFrame(this,10,10);
  //dTabFrame->AddFrame(dSubFrame,dSubLayout);
  //dTabFrame->Resize(GetWidth(),GetHeight());
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,8, 10);  
  dTabFrame->AddFrame(dCanvas,dCnvLayout); //Add the canvas to the tab


   
  dTabFrame->Resize(GetWidth(),GetHeight());


  dButtonPos = new TGTextButton(this, "&Beam Position Differences", BA_POSSITION);
  dButtonCharge = new TGTextButton(this, "&Beam Charge Asymmetries", BA_CHARGE);
  AddFrame(dButtonPos,dBtnLayout);
  AddFrame(dButtonCharge,dBtnLayout);
  //dSubFrame->AddFrame(dButtonPos,dBtnLayout);
  //dSubFrame->Resize(GetWidth(),GetHeight());
  //dSubFrame->AddFrame(dButtonCharge, dBtnLayout);
  //dSubFrame->Resize(GetWidth(),GetHeight());
  

  dButtonPos -> Associate(this);
  dButtonCharge -> Associate(this);


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
  
  mc->Divide(3,2);

  obj = HistArray.At(1);  // Get MPS tree
  if(ldebug) printf("%s\n", obj->GetName());
  
  for(Short_t p = 0; p <INJECTOR_BPM_NUM ; p++) 
    {
      char * histo;
      sprintf (histo, "%sRelX.hw_sum",InjectorBPM[p] );

      if( ((TTree*) obj)->FindLeaf(histo))
	{
	  if(ldebug) printf("%s\n",InjectorBPM[p]);
	  mc -> cd(p+1);
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
 
  mc->Divide(4,2);

  objc = HistArray.At(0);  // Get HEL tree
  
  if(ldebug) printf("%s\n", objc->GetName());
  
  for(Short_t k = 0; k <INJECTOR_BCM_NUM ; k++) 
    {
      char * histoc; 
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
