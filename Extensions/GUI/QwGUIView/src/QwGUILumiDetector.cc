#include <QwGUILumiDetector.h>
#include <iostream>
#include "TLine.h"
#include <TPaveText.h>

ClassImp(QwGUILumiDetector);

enum QwGUILumiDetectorIdentificator {
  SHOW_UPSTREAM,
  SHOW_DOWNSTREAM,
};

const char *QwGUILumiDetector::RootTrees[2] = {"HEL_Tree","MPS_Tree"};

const char *QwGUILumiDetector::USLumiDetectorHists[USLUMI_DET_HST_NUM] = 
  { "uslumi1neg","uslumi1pos","uslumi3neg","uslumi3pos","uslumi5neg","uslumi5pos","uslumi7neg","uslumi7pos"};

const char *QwGUILumiDetector::DSLumiDetectorHists[DSLUMI_DET_HST_NUM] = 
  { "dslumi1","dslumi2","dslumi3","dslumi4","dslumi5","dslumi6","dslumi7","dslumi8"};

QwGUILumiDetector::QwGUILumiDetector(const TGWindow *p, const TGWindow *main, const TGTab *tab, const char *objName, const char *mainname, UInt_t w, UInt_t h)

  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame = NULL;
  dCanvas = NULL;  
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dBtnLayout = NULL;

  dButtonUpstream = NULL;
  dButtonDownstream = NULL;

  HistArray.Clear();
  DataWindowArray.Clear();

  AddThisTab(this);
}

QwGUILumiDetector::~QwGUILumiDetector()
{
  if(dTabFrame)  delete dTabFrame;
  if(dCanvas)    delete dCanvas;  
  if(dTabLayout) delete dTabLayout;
  if(dCnvLayout) delete dCnvLayout;
  if(dBtnLayout) delete dBtnLayout;

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



  dTabFrame = new TGHorizontalFrame(this,10,10);
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);    
  
  dButtonUpstream = new TGTextButton(this, "&Upstream",  SHOW_UPSTREAM);
  //dButtonPos->SetCommand("printf(\"Reading position information %s\\n\","
  //                       "gROOT->GetVersion());");

  dButtonDownstream = new TGTextButton(this, "&Downstream", SHOW_DOWNSTREAM);
  //dButtonCharge->SetCommand("printf(\"Reading charge asymmetires \n\")" );
 

 
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);
  AddFrame(dButtonUpstream, dBtnLayout);
  AddFrame(dButtonDownstream, dBtnLayout);

  dButtonUpstream -> Associate(this);
  dButtonDownstream -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUILumiDetector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);

  //mc->Divide( 2 , 2 );

}

void QwGUILumiDetector::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUILumiDetector::OnObjClose(char *obj)
{
  if(!strcmp(obj,"dROOTFile")){
//     printf("Called QwGUILumiDetector::OnObjClose\n");

    dROOTCont = NULL;
  }
}

void QwGUILumiDetector::OnNewDataContainer(RDataContainer *cont)
{
  TObject *obj = NULL;
  TObject *copy = NULL;
 
  ClearData();

  if(dROOTCont){
    for(Short_t p = 0; p < 2; p++) {
      obj = dROOTCont->ReadTree(RootTrees[p]);
      if(obj)
	{
          std::cout<<"read root trees on new data container\n";
	  copy = obj->Clone();
	  HistArray.Add(copy);
	}
    }
  }

  //ShowUpstream();

};



       



void QwGUILumiDetector::OnRemoveThisTab()
{

}

void QwGUILumiDetector::ClearData()
{

  //TObject *obj;
  //TIter next(HistArray.MakeIterator());
  //obj = next();
  //while(obj){    
  //  delete obj;
  //  obj = next();
  //}
  HistArray.Clear();
}



void QwGUILumiDetector::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if(event == kButton1Double){
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
    if(pad > 0 && pad <= USLUMI_DET_HST_NUM)
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

void QwGUILumiDetector::ShowUpstream()
{

  std::cout<<"showing upstream\n";

  Bool_t status;
  TCanvas *mc = dCanvas->GetCanvas();
  TObject *obj;
  //TTree *tree;
  TPaveText *errlabel;  

  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);
  //ClearData();

  mc->Clear();
  mc->Divide(3,3); 

  // Get HEL_Tree
  obj = HistArray.At(0);
  if(!obj) 
    {
    std::cout<<"histarray not filled\n";
    return;
    }

   
  for(int p = 0; p < USLUMI_DET_HST_NUM; p++)
    {
    if( ((TTree*) obj)->FindLeaf(Form("asym_%s.hw_sum",USLumiDetectorHists[p])) )
      {
        std::cout<<Form("Histogram asym_%s.hw_sum found!\n",USLumiDetectorHists[p]);
        mc->cd(p+1);
        obj->Draw(Form("asym_%s.hw_sum",USLumiDetectorHists[p]));
        status=kTRUE;
      }
      else 
      {
        errlabel->AddText(Form("Unable to find object %s !",USLumiDetectorHists[p]));
        std::cout<<Form("Histogram asym_%s.hw_sum not found\n",USLumiDetectorHists[p]);
        status=kFALSE;
        //mc->cd(p+1);
        //errlabel->Draw();
      }
    }  
    if(!status)
      {
      mc->Clear();
      errlabel->Draw();
      }
      
  mc->Modified();
  mc->Update();
      
   
}





void QwGUILumiDetector::ShowDownstream()
{

  std::cout<<"showing downstream\n";

  Bool_t status;
  TCanvas *mc = dCanvas->GetCanvas();
  TObject *obj;
  //TTree *tree;
  TPaveText *errlabel;  

  errlabel = new TPaveText(0, 0, 1, 1, "NDC");
  errlabel->SetFillColor(0);
  //ClearData();

  mc->Clear();
  mc->Divide(3,3); 

  // Get HEL_Tree
  obj = HistArray.At(0);
  if(!obj) 
    {
    std::cout<<"histarray not filled\n";
    return;
    }

   
  for(int p = 0; p < DSLUMI_DET_HST_NUM; p++)
    {
    if( ((TTree*) obj)->FindLeaf(Form("asym_%s.hw_sum",DSLumiDetectorHists[p])) )
      {
        std::cout<<Form("Histogram asym_%s.hw_sum found!\n",DSLumiDetectorHists[p]);
        mc->cd(p+1);
        obj->Draw(Form("asym_%s.hw_sum",DSLumiDetectorHists[p]));
        status=kTRUE;
      }
      else 
      {
        errlabel->AddText(Form("Unable to find object %s !",DSLumiDetectorHists[p]));
        std::cout<<Form("Histogram asym_%s.hw_sum not found\n",DSLumiDetectorHists[p]);
        status=kFALSE;
        //mc->cd(p+1);
        //errlabel->Draw();
      }
    }  
    if(!status)
      {
      mc->Clear();
      errlabel->Draw();
      }
      
  mc->Modified();
  mc->Update();
      
   
}
















Bool_t QwGUILumiDetector::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
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
	      case SHOW_UPSTREAM:
		//printf("text button id %ld pressed\n", parm1);
                std::cout<<"showing upstream lumis\n";
		ShowUpstream();
		break;
		
	      case SHOW_DOWNSTREAM:
		//printf("text button id %ld pressed\n", parm1);
                std::cout<<"showing downstream lumis\n";
		ShowDownstream();
		break;
		
	      }

	    break;
	  }

    case kCM_COMBOBOX:
      {
	switch (parm1) {
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
  default:
    break;
  }
  
  return kTRUE;
}
