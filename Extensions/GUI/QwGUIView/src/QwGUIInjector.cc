#include "QwGUIInjector.h"
#include <TG3DLine.h>
#include "TGaxis.h"
ClassImp(QwGUIInjector);

enum QwGUIInjectorIndentificator {
  BA_POSSITION,
  BA_CHARGE,
  BA_POS_VAR
};

const char *QwGUIInjector::InjectorTrees[INJECTOR_DET_TRE_NUM] = 
  {
    "HEL_Tree",
    "MPS_Tree"
  };

const char *QwGUIInjector::InjectorDevices[INJECTOR_DEV_NUM]=
  {"qwk_1i02","qwk_1i04","qwk_1i06","qwk_0i02","qwk_0i02a",
   "qwk_0i05","qwk_0i07","qwk_0l01","qwk_0l02","qwk_bcm0l02",
   "qwk_0l03","qwk_0l04","qwk_0l05","qwk_0l06","qwk_0l07",
   "qwk_0l08","qwk_0l09","qwk_0l10","qwk_0r01","qwk_0r02",
   "qwk_0r05","qwk_0r06"
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
  dButtonPos          = NULL;
  dButtonCharge       = NULL;
  dButtonPosVariation = NULL;

  PosVariation[0] = NULL;
  PosVariation[1] = NULL;
 
  HistArray.Clear();
  DataWindowArray.Clear();

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
  if(dButtonPos)          delete dButtonPos;
  if(dButtonCharge)       delete dButtonCharge;
  if(dButtonPosVariation) delete dButtonPosVariation;

  delete [] PosVariation;

  RemoveThisTab(this);
  IsClosing(GetName());
}

void QwGUIInjector::MakeLayout()
{

  SetCleanup(kDeepCleanup);

  dTabFrame= new TGHorizontalFrame(this);  
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
  dButtonPosVariation = new TGTextButton(dControlsFrame, "&Beam Position Variation", BA_POS_VAR);
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
  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
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
//     printf("Called QwGUIInjector::OnObjClose\n");

    dROOTCont = NULL;
  }
}


void QwGUIInjector::OnNewDataContainer()
{

  TObject *obj = NULL;
  TObject *copy = NULL;
 
  ClearData();

  if(dROOTCont){
    for(Short_t p = 0; p < INJECTOR_DET_TRE_NUM; p++) {
      obj = dROOTCont->ReadTree(InjectorTrees[p]);
      if(obj)
	{
	  copy = obj->Clone();
	  HistArray.Add(copy);
	}
    }
  }
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


void QwGUIInjector::PlotPosData()
{

  TObject *obj = NULL;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();

  obj = HistArray.At(1);  // Get MPS tree
  if(! obj) return;
  // prevent crash when executing this function
  // if a ROOT file doesn't contain MPS tree

  Bool_t ldebug = true;
 
  Int_t cnt = 0;     
  mc->Divide(7,7);
  
  if(ldebug) {
    printf("PlotPosData ---------------------------------\n");
    printf("Found a tree name is %s \n", obj->GetName());
  }

  char histo[128];

  for(Short_t p = 0; p <INJECTOR_DEV_NUM ; p++) 
    {
      
      if( ! strcmp(InjectorDevices[p],"qwk_bcm0l02" )){
	//donothing
      }
      else{
	cnt++;
	sprintf (histo, "%sRelX.hw_sum",InjectorDevices[p] );
	if( ((TTree*) obj)->FindLeaf(histo) )
	  {
	    if(ldebug) printf("Found a histogram name %22s\n", histo);
	    mc -> cd(cnt);
	    obj -> Draw(histo);
	    gPad->Update();
	  }

	sprintf (histo, "%sRelY.hw_sum",InjectorDevices[p] );
	if( ((TTree*) obj)->FindLeaf(histo) )
	  {
	    if(ldebug) printf("Found a histogram name %22s\n", histo);
	    mc -> cd(cnt+INJECTOR_DEV_NUM);
	    obj -> Draw(histo);
	    gPad->Update();
	  }

      }
    }  
  
  if(ldebug) printf("---------------------------------------------\n");
  
  mc->Modified();
  mc->Update();
  
  return;
  
}



void QwGUIInjector::PlotChargeData()
{
  
  TObject *objc = NULL;
  objc = HistArray.At(0);  // Get HEL tree
  if(! objc) return; 
  
  // prevent crash when executing this function
  // if a ROOT file doesn't contain HEL tree

  Bool_t ldebug = false;
 
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(4,6);

  if(ldebug) printf("%s\n", objc->GetName());
  
  char histoc[128];

  for(Short_t k = 0; k <INJECTOR_DEV_NUM ; k++) 
    {

      if(!strcmp(InjectorDevices[k], "qwk_bcm0l02") ) {
	sprintf (histoc, "asym_%s.hw_sum",InjectorDevices[k] );
      }
      else{
	sprintf(histoc, "asym_%sWSum.hw_sum",InjectorDevices[k] );
      }
      
      if( ((TTree*)objc)->FindLeaf(histoc) )
	{
	  if(ldebug) printf("%s\n", histoc);
	  mc -> cd(k+1);
	  objc -> Draw(histoc);
	  gPad->Update();
	}
    }  
  
  mc->Modified();
  mc->Update();
}


void QwGUIInjector::PositionDifferences()
{
  for(Short_t i=0;i<2;i++) 
    {
      if(PosVariation[i]) delete PosVariation[i];  PosVariation[i] = NULL;
    }
  Bool_t ldebug = false;
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();

  mc->Clear();
  mc->Divide(1,2);

  TObject *obj = NULL;
  obj = HistArray.At(1);  // Get MPS tree
  if( !obj ) return; 

  if(ldebug) {
    printf("PositionDiffercences -------------------------------\n");
    printf("Found a tree name is %s \n", obj->GetName());
  }

  char histo[128];
  
  Int_t xcount = 0;
  Int_t ycount = 0;
  
  Double_t offset = 0.5;
  Double_t min_range = - offset;
  Double_t max_range = INJECTOR_DEV_NUM - offset ; 

  PosVariation[0] = new TH1D("dxvar", "#Delta X Variation", INJECTOR_DEV_NUM, min_range, max_range);
  PosVariation[1] = new TH1D("dyvar", "#Delta Y variation", INJECTOR_DEV_NUM, min_range, max_range);

  TH1D *dummyhist = NULL;

  TString dummyname;
 

  for(Short_t p = 0; p <INJECTOR_DEV_NUM ; p++) 
    {
      if(!strcmp(InjectorDevices[p],"qwk_bcm0l02"))
	{
	}
      else
	{
	  sprintf (histo, "%sRelX.hw_sum", InjectorDevices[p]);
	  if( ((TTree*) obj)->FindLeaf(histo) )
	    {
	      xcount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	      if(ldebug) printf("Found %2d : a histogram name %22s\n", xcount, histo);
	      obj -> Draw(histo);
	      dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	      dummyhist -> SetName(histo);
	      dummyname = dummyhist -> GetName();
	      dummyname.Replace(0,4," ");
	      dummyname.ReplaceAll("RelX.hw_sum", "X");
	      PosVariation[0] -> SetBinContent(xcount, dummyhist->GetMean());
	      PosVariation[0] -> SetBinError  (xcount, dummyhist->GetMeanError());
	      PosVariation[0] -> GetXaxis()->SetBinLabel(xcount, dummyname);
	      SummaryHist(dummyhist);
	      delete dummyhist; dummyhist= NULL;
	      
	    }
	  sprintf (histo, "%sRelY.hw_sum", InjectorDevices[p]);
	  if( ((TTree*) obj)->FindLeaf(histo) )
	    {
	      ycount++; // see http://root.cern.ch/root/html/TH1.html#TH1:GetBin
	      if(ldebug) printf("Found %2d : a histogram name %22s\n", ycount, histo);
	      obj -> Draw(histo);
	      dummyhist = (TH1D*)gPad->GetPrimitive("htemp"); 
	      dummyhist -> SetName(histo);
	      dummyname = dummyhist -> GetName();
	      dummyname.Replace(0,4," ");
	      dummyname.ReplaceAll("RelY.hw_sum", "Y");
	      PosVariation[1] -> SetBinContent(ycount, dummyhist->GetMean());
	      PosVariation[1] -> SetBinError  (ycount, dummyhist->GetMeanError());
	      PosVariation[1] -> GetXaxis()->SetBinLabel(ycount, dummyname);
	      SummaryHist(dummyhist);
	      delete dummyhist; dummyhist= NULL;

 
	    }
	}
    }

  

  mc->Clear();
  mc->Divide(1,2);
  
  mc->cd(1);
  SummaryHist(PosVariation[0]);
  PosVariation[0] -> SetMarkerStyle(20);
  PosVariation[0] -> SetTitle("#Delta X Variation");
  PosVariation[0] -> GetYaxis() -> SetTitle("#Delta X (nm)");
  PosVariation[0] -> GetXaxis() -> SetTitle("BPM X");
  PosVariation[0] -> Draw("E1");
  gPad->Update();


  mc->cd(2);
  SummaryHist(PosVariation[1]);
  PosVariation[1] -> SetMarkerStyle(20);
  PosVariation[1] -> SetTitle("#Delta Y Variation");
  PosVariation[1] -> GetYaxis()-> SetTitle ("#Delta Y (nm)");
  PosVariation[1] -> GetXaxis() -> SetTitle("BPM Y");
  PosVariation[1] -> Draw("E1");
  gPad->Update();
  

  if(ldebug) printf("----------------------------------------------------\n");
  mc->Modified();
  mc->Update();
  

  if(dummyhist) delete dummyhist;
  return;
}



// void QwGUIInjector::PositionDifferences(){

//   bool ldebug = false;
  
//   TCanvas *mc = dCanvas->GetCanvas();

//   mc->Clear();
//   mc->Divide(1,2);

//   Double_t relx[INJECTOR_DEV_NUM], rely[INJECTOR_DEV_NUM];
//   Double_t erx [INJECTOR_DEV_NUM], ery[INJECTOR_DEV_NUM];
//   Double_t err [INJECTOR_DEV_NUM], name[INJECTOR_DEV_NUM+1];

//   //  Char_t* post[2]={"RelX","RelY"};

//   TObject *obj;
//   if(! obj) return; 

//   TH1D* histx[INJECTOR_DEV_NUM] = {NULL};
//   TH1D* histy[INJECTOR_DEV_NUM] = {NULL};

//   obj = HistArray.At(1);  // Get MPS tree

//   if(ldebug) {
//     printf("PositionDiffercences -------------------------------\n");
//     printf("Found a tree name is %s \n", obj->GetName());
//   }

//   char histo[128];
  
//   Int_t cnt = 0;

//   for(Short_t p = 0; p <INJECTOR_DEV_NUM ; p++) 
//     {
//       if(!strcmp(InjectorDevices[p],"qwk_bcm0l02"))
// 	{
// 	}
//       else
// 	{
// 	  sprintf (histo, "%sRelX.hw_sum", InjectorDevices[p]);
// 	  if( ((TTree*) obj)->FindLeaf(histo) )
// 	    {
// 	      cnt++;
// 	      if(ldebug) printf("Found %2d : a histogram name %22s\n", cnt, histo);
// 	      obj -> Draw(histo);
// 	      histx[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
// 	      histx[p] -> SetName(histo);
// 	      relx[cnt] = histx[p]->GetMean();
// 	      erx[cnt]  = histx[p]->GetRMS()/sqrt(histx[p]->GetEntries());
// 	      SummaryHist(histx[p]);
// 	      //std::cout<<relx[cnt]<<"       "<<erx[cnt]<<"\n"; 
// 	    }

// 	  sprintf (histo, "%sRelY.hw_sum", InjectorDevices[p]);	  
// 	  if( ((TTree*) obj)->FindLeaf(histo) )
// 	    {
// 	      if(ldebug) printf("\nFound %2d : a histogram name %22s\n", cnt, histo);
// 	      obj -> Draw(histo);
// 	      histy[p] = (TH1D*)gPad->GetPrimitive("htemp"); 
// 	      histy[p] -> SetName(histo);
// 	      rely[cnt] = histy[p]->GetMean();
// 	      ery[cnt]  = histy[p]->GetRMS()/sqrt(histy[p]->GetEntries());
// 	      name[cnt]=cnt+1;
// 	      err[cnt]=0.0; 
// 	      SummaryHist(histy[p]);
// 	    }
// 	}
//     }  
  
//   mc->Clear();
//   mc->Divide(1,2);
  
//   TGraphErrors* gx  = new TGraphErrors(cnt,name,relx,err,erx);
//   gx->SetTitle("#Delta x Variation");
//   gx->GetYaxis()->SetTitle("#Delta x (nm)");
//   gx->SetMarkerStyle(8);
//   gx->SetMarkerSize(0.8);
  
  
//   TGraphErrors* gy  = new TGraphErrors(cnt,name,rely,err,ery);
//   gy->SetTitle("#Delta y Variation");
//   gy->GetYaxis()->SetTitle("#Delta y(nm)");
//   gy->SetMarkerStyle(8);
//   gy->SetMarkerSize(0.8);
  
//   Int_t k=0;
//   for (Int_t j=1;j<=cnt;j++)     
//     {
//       if(!strcmp(InjectorDevices[j],"qwk_bcm0l02"))
// 	{
// 	  //skip
// 	}
//       else{
// 	k++;
// 	gx->GetXaxis()->SetBinLabel((Int_t) 4.5*k,InjectorDevices[j-1]);
// 	gy->GetXaxis()->SetBinLabel((Int_t) 4.5*k,InjectorDevices[j-1]);
//       } 
//     } 
  
//   mc->cd(1);
//   gx->Draw("ap");
//   gPad->Update();

//   mc->cd(2);
//   gy->Draw("ap");
//   gPad->Update();

//   if(ldebug) printf("----------------------------------------------------\n");
//   mc->Modified();
//   mc->Update();
  
//   return;
// }




void QwGUIInjector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= INJECTOR_DEV_NUM)
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
	      switch(parm1)
		{
		case   BA_POSSITION:
		  //printf("text button id %ld pressed\n", parm1);	
		  PlotPosData();
		  break;
		  
		case BA_CHARGE:
		  
		  //printf("text button id %ld pressed\n", parm1);
		  PlotChargeData();
		  break;

		case BA_POS_VAR:
		  //printf("text button id %ld pressed\n", parm1);
		  PositionDifferences();
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
