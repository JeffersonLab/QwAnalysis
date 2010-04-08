#include <QwGUILumiDetector.h>
#include <iostream>
#include "TLine.h"

ClassImp(QwGUILumiDetector);

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
  
  dButtonUser = new TGTextButton(this, "&User",  1);
  //dButtonPos->SetCommand("printf(\"Reading position information %s\\n\","
  //                       "gROOT->GetVersion());");

  dButtonDetail = new TGTextButton(this, "&Detail", 2);
  //dButtonCharge->SetCommand("printf(\"Reading charge asymmetires \n\")" );
 

 
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);
  AddFrame(dButtonUser, dBtnLayout);
  AddFrame(dButtonDetail, dBtnLayout);

  dButtonUser -> Associate(this);
  dButtonDetail -> Associate(this);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				"QwGUILumiDetector",
				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  Int_t wid = dCanvas->GetCanvasWindowId();
  QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
  dCanvas->AdoptCanvas(mc);

  mc->Divide( LUMI_DET_HST_NUM/2, LUMI_DET_HST_NUM/2);

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

void QwGUILumiDetector::OnNewDataContainer()
{

  TObject *obj;
  TObject *copy;
  ClearData();

  std::cout<<"On new Data container!\n";
  if(dROOTCont){
    for(int p = 0; p < LUMI_DET_HST_NUM; p++){
	
      obj = dROOTCont->ReadData(LumiDetectorHists[p]);      
      if(obj==NULL)  std::cout<<"no data for lumis\n";
      if(obj){

	if(obj->InheritsFrom("TH1")){
	  copy = obj->Clone();
	  ((TH1*)copy)->SetName(Form("%s_cp",((TH1*)obj)->GetName()));
	  ((TH1*)copy)->SetDirectory(0);
	  HistArray.Add(copy);
	}
      }
    }
  }  
  PlotData();
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

void QwGUILumiDetector::PlotData()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();

  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();
  while(obj){
    mc->cd(ind);
    ((TH1*)obj)->Draw("");
    ind++;
    obj = next();
  }

  mc->Modified();
  mc->Update();

}

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
    switch (GET_SUBMSG(msg)) 
      {
      
	case kCM_BUTTON:
	  {
	    TCanvas *mc = dCanvas->GetCanvas();

	    TIter next(HistArray.MakeIterator());
	    
	    switch(parm1)
	      {
	      case 1:
		{
		printf("text button id %ld pressed\n", parm1);
		mc->Clear();
		TH2F *hist2_1 = new TH2F("","Asymmetry vs Lumi",100,0,0,100,0,0);
		TH2F *hist2_2 = new TH2F("","Asymmetry Width vs Lumi",100,0,0,100,0,0);
		Double_t x=0,y=0,yw=0;;
		for (int i=1; i<9; i++) {
			x=i;
			y=0;
			yw=0;
			hist2_1->Fill(x,y);
			hist2_2->Fill(x,yw);
			//cout << x<<"\n";
			}
		TAxis *Xaxis = hist2_1->GetXaxis();
		TAxis *Yaxis = hist2_1->GetYaxis();
		Yaxis->SetTitle("Asymmetry");
		Xaxis->SetTitle("Luminosity Monitor #");
		hist2_1->SetMarkerStyle(21);

		TAxis *X2axis = hist2_2->GetXaxis();
		TAxis *Y2axis = hist2_2->GetYaxis();
		Y2axis->SetTitle("Asymmetry Width");
		X2axis->SetTitle("Luminosity Monitor #");
		hist2_1->SetMarkerStyle(21);
		hist2_2->SetMarkerStyle(21);

		mc->Divide(1,2);
		mc->cd(1);
		hist2_1->Draw();
		mc->cd(2);
		hist2_2->Draw();
		TLine *l = new TLine(1,0,8,0);
		l->Draw("same");
		mc->Update();


		break;
		}

	      case 2:
		{
		printf("text button id %ld pressed\n", parm1);
		mc->Clear();
		TH1F *hist1 = new TH1F("Lumi 1","Asymmetry",100,0,0);
		TH1F *hist2 = new TH1F("Lumi 2","Asymmetry",100,0,0);
		TH1F *hist3 = new TH1F("Lumi 3","Asymmetry",100,0,0);
		TH1F *hist4 = new TH1F("Lumi 4","Asymmetry",100,0,0);
		TH1F *hist5 = new TH1F("Lumi 5","Asymmetry",100,0,0);
		TH1F *hist6 = new TH1F("Lumi 6","Asymmetry",100,0,0);
		TH1F *hist7 = new TH1F("Lumi 7","Asymmetry",100,0,0);
		TH1F *hist8 = new TH1F("Lumi 8","Asymmetry",100,0,0);

		hist1->Fill(1,10000);
		hist2->Fill(1,10000);
		hist3->Fill(1,10000);
		hist4->Fill(1,10000);
		hist5->Fill(1,10000);
		hist6->Fill(1,10000);
		hist7->Fill(1,10000);
		hist8->Fill(1,10000);

		mc->Divide( 3,3);

		mc->cd(1);
		hist1->Draw();
		mc->cd(2);
		hist2->Draw();
		mc->cd(3);
		hist3->Draw();
		mc->cd(4);
		hist4->Draw();
		mc->cd(6);
		hist5->Draw();
		mc->cd(7);
		hist6->Draw();
		mc->cd(8);
		hist7->Draw();
		mc->cd(9);
		hist8->Draw();

		mc->Update();
		break;
		}
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
    
  default:
    break;
  }
  
  return kTRUE;
}
