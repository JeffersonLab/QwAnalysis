#include "QwGUISummary.h"

#include "TGaxis.h"
#include "TColor.h"
#include "TStyle.h"
#include <TString.h>



ClassImp(QwGUISummary);

const Int_t QwGUISummary::fSleepTimeMS = 2000;

const char *QwGUISummary::fChannels[N_CH] = {
  "asym_qwk_bcm1_hw", "diff_qwk_targetX_hw", "diff_qwk_targetY_hw",
  "diff_qwk_bpm3c12X_hw", "asym_qwk_mdallbars_hw"
};

QwGUISummary::QwGUISummary(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 

  dTabFrame           = NULL;
  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  AddThisTab(this);
  SetHistoAccumulate(1);
  SetHistoReset(0);

  dtab = tab->GetTabTab("Summary Tab");


}

QwGUISummary::~QwGUISummary()
{
  if(dTabFrame)           delete dTabFrame;
  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
}


void QwGUISummary::MakeLayout()
{

  SetCleanup(kDeepCleanup);

  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));
  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));
  dCanvas->GetCanvas()->SetBorderMode(0);
}

void QwGUISummary::OnReceiveMessage(char *obj)
{

}


Bool_t QwGUISummary::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  return true;
}

void QwGUISummary::PlotMainData(){
  
  Bool_t ldebug = false;

  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,3);
  
  TH1F* histo_buff[N_CH]={NULL};
  TH1F* histo[N_CH]={NULL};
  char hname[128];
  TString dummyname;

  TGraph * grp = NULL;

  SetHistoDefaultMode();//bring the histo mode to accumulate mode

  if(dMapFileFlag){
    while(1){

      // NOTE: I avoided using a single for loop for the whole process
      // because when resetting the histograms it creates issues if the reset
      // happens in the middle of the for loop. Then only the histograms
      // which are in the reminder of the loop will get reset and the ones
      // before will not.
      

	//Check to see if the GUI is in "paused" mode
      if (GetHistoPause()==0){
	for(Int_t i=0;i<N_CH;i++){
	  // If not "paused" grab the histograms from the map file
	  sprintf (hname, "%s", fChannels[i]);
	  histo[i]= (TH1F *)dMapFile->Get(hname);
	}
      }
      
      // Check to see if GUI is in "Reset" mode
      if (GetHistoReset()){
	for(Int_t i=0;i<N_CH;i++){
	  // If Reset, clone the original histogram as histo_buffer
	  histo_buff[i]=(TH1F*)histo[i]->Clone(Form("%s_buff",histo[i]->GetName()));
	  *histo[i]=*histo[i]-*histo_buff[i];
	  if(i == N_CH-1) SetHistoReset(0);   
	  std::cout<<"reset \n";
	}
      }else if (GetHistoAccumulate()==0){
	// If not Reset and not Accumulatem subtract the buffer from the original.
	// This is where resetting is being done.
	for(Int_t i=0;i<N_CH;i++)
	  *histo[i]=*histo[i]-*histo_buff[i];
      }
      
      // Draw the histograms when the tab is active
      for(Int_t i=0;i<N_CH;i++){
	// if at least of them are no emptry/NULL
	if (histo[i]!=NULL && (dtab->IsActive())){
	  if(ldebug) printf("Found %2d : a histogram name %22s\n", i, hname);
	  if(ldebug) SummaryHist(histo[i]);
	  mc->cd(i+1);
	  histo[i]->Draw();
	}
      }
      mc->Modified();
      mc->Update();
	
      gSystem->Sleep(fSleepTimeMS);
      if (gSystem->ProcessEvents()){
	break;
      }
      
    }
  }   
  return;
}
