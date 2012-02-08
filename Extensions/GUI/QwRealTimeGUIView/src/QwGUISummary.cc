#include "QwGUISummary.h"

#include "TGaxis.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPaveLabel.h"
#include <TString.h>



ClassImp(QwGUISummary);

const Int_t QwGUISummary::fSleepTimeMS = 2000;

const char *QwGUISummary::fChannels[N_CH] = {
  "asym_qwk_charge_hw", "asym_qwk_bcm1_hw", 
  "diff_qwk_targetX_hw", "diff_qwk_targetY_hw",
  "diff_qwk_bpm3c12X_hw", "yield_qwk_bpm3c12_EffectiveCharge_hw",
  "asym_qwk_mdallbars_hw"
};

const char *QwGUISummary::fChannelsCheckError[N_CH_ERR] = {
  "asym_qwk_charge_dev_err", "asym_qwk_bcm1_dev_err", 
  "diff_qwk_targetX_dev_err", "diff_qwk_targetY_dev_err",
  "diff_qwk_targetXSlope_dev_err", "diff_qwk_targetYSlope_dev_err",
  "diff_qwk_bpm3c12X_dev_err", "asym_qwk_mdallbars_dev_err"
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


  //stats
  gStyle->SetOptTitle(1);
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat("eMr");//with error on the mean

  // histo parameters
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.35);
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleOffset(2);
  gStyle->SetTitleColor(kBlack);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);

  gStyle->SetPadBottomMargin(0.2);
  
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  mc->Clear();
  mc->Divide(2,4);
  
  TH1F* histo_buff[N_CH]          = {NULL};
  TH1F* histo[N_CH]               = {NULL};
  TH1F* histo_err_buff[N_CH_ERR]  = {NULL};
  TH1F* histo_err[N_CH_ERR]       = {NULL};
  TH1F * error_summary            = {NULL};
  TBox* abox = NULL;
  TPaveLabel *p = NULL;

  char hname[128];
  TString dummyname;
  Double_t mean = 0;
  Double_t rms = 0;

  error_summary = new TH1F("error_summary","",3,0,3);

  Int_t i;
  Int_t j;
  Int_t EntryCounts=0;

  SetHistoDefaultMode();//bring the histo mode to accumulate mode

  if(dMapFileFlag){
    while(1){

      // NOTE: I avoided using a single "for loop" for the whole process
      // because when resetting the histograms it creates issues if the reset
      // happens in the middle of the for loop. Then only the histograms
      // which are in the reminder of the loop will get reset and the ones
      // before will not.
      

	//!Check to see if the GUI is in "paused" mode
      if (GetHistoPause()==0){
	//!If not "paused" grab the histograms from the map file
	
	for(i=0;i<N_CH;i++){
	  sprintf (hname, "%s", fChannels[i]);
	  histo[i]= (TH1F *)dMapFile->Get(hname);
	}
	for(j=0;j<N_CH_ERR;j++){
	  sprintf (hname, "%s", fChannelsCheckError[j]);
	  histo_err[j]= (TH1F *)dMapFile->Get(hname);
	}
      }
      
      //!Check to see if GUI is in "Reset" mode
      if (GetHistoReset()){
	//!If Reset, clone the original histogram as histo_buffer
	for(i=0;i<N_CH;i++){
	  histo_buff[i]=(TH1F*)histo[i]->Clone(Form("%s_buff",histo[i]->GetName()));
	  *histo[i]=*histo[i]-*histo_buff[i];
	  if(i == N_CH-1) SetHistoReset(0);   
	}

	for(j=0;j<N_CH_ERR;j++){
	  histo_err_buff[j]=(TH1F*)histo_err[j]->Clone(Form("%s_buff",histo_err[j]->GetName()));
	  *histo_err[j]=*histo_err[j]-*histo_err_buff[j];
	  if(j == N_CH_ERR-1) SetHistoReset(0);   
	}
      }
      else if (GetHistoAccumulate()==0){
	//!If not Reset and not Accumulatem subtract the buffer from the original.
	//!This is where resetting is being done.

	for(j=0;j<N_CH_ERR;j++){
	  mean = histo_err[j]->GetMean();
	  *histo_err[j]=*histo_err[j]-*histo_err_buff[j];	  
	}
	for(i=0;i<N_CH;i++)
	  *histo[i]=*histo[i]-*histo_buff[i];

	//I'm using the asym_charge channel to count entries
	EntryCounts=(Int_t)(histo[0]->GetEntries());
	UpdateAutoHistoReset(EntryCounts);//this will compare histogram entries with current auto reset limit

      }
      
      //!Draw the histograms when the tab is active

      if(dtab->IsActive()){
	for(i=0;i<N_CH_ERR;i++){
	  //!if at least of them are no emptry/NULL
	  if (histo[i]!=NULL){
	    if(ldebug) printf("Found %2d : a histogram name %22s\n", i, hname);
	    if(ldebug) SummaryHist(histo[i]);
	    mc->cd(i+1);
	    mean = histo[i]->GetMean();
	    histo[i]->Draw();
	    histo[i]->GetXaxis()->SetLabelSize(0.08);
	    histo[i]->GetYaxis()->SetLabelSize(0.08);
	    histo[i]->GetXaxis()->SetTitleSize(0.08);
	    histo[i]->GetYaxis()->SetTitleSize(0.08);
	    histo[i]->GetYaxis()->SetTitleOffset(0.5);
	    histo[i]->SetFillColor(41+i);
	    
	    if(i==5){
	      abox = new TBox(90e3,0,200e3, histo[i]->GetEntries());
	      abox->SetFillColor(2);
	      abox->SetFillStyle(3003);
	      abox->Draw("");

	      if(mean>200e3){
		p = new TPaveLabel(0.2,0.4,0.6,0.7,"3c12 BPM is saturating!","brNDC");
	      }
	      if(mean<90e3){
		p = new TPaveLabel(0.2,0.4,0.6,0.7,"3c12 BPM charge is too low!","brNDC");
	      }
	      if(p!=NULL && mean>200e3 || mean<90e3){
		p->Draw();
		p->SetTextColor(kRed);	
		p->SetFillStyle(0);
		p->SetBorderSize(0);
	      } 
	      gPad->Modified();
	      gPad->Update();
	    }
	  }
	}
	//!Draw the summary plot when the tab is active
	error_summary->Reset();
	for(j=0;j<N_CH_ERR;j++){
	  if (histo_err[j]!=NULL){
	    if(ldebug) {
	      printf("Found %2d : a histogram name %22s\n", j, hname);
	      SummaryHist(histo_err[j]);
	    }
	    if((histo_err[j]->GetMean())!=0){
	      dummyname = fChannelsCheckError[j];
	      dummyname.ReplaceAll("_dev_err", "");
	      dummyname.ReplaceAll("_qwk", "");
	      error_summary->Fill(dummyname,(histo_err[j]->GetEntries()));
	    }
	  }
	}
	
	if (error_summary!=NULL){
	  mc->cd(8);
	  error_summary->SetStats(0);
	  error_summary->SetFillColor(46);
	  error_summary->SetBarWidth(0.5);
	  error_summary->SetBarOffset(0.06);
	  error_summary->SetBit(TH1::kCanRebin);
	  error_summary->LabelsDeflate();
	  error_summary->SetTitle("Error Summary");
	  error_summary->GetXaxis()->SetLabelSize(0.10);
	  error_summary->GetYaxis()->SetLabelSize(0.08);
	  error_summary->GetXaxis()->SetLabelOffset(0.01);
	  error_summary->GetYaxis()->SetTitleOffset(0.5);
	  error_summary->GetYaxis()->SetTitleSize(0.08);
	  error_summary->GetYaxis()->SetTitle("Bad Events");
	  error_summary->SetMarkerSize(3.0);
	  error_summary->Draw("bar2 TEXT0");
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


