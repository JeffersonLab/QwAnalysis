// Name  :   plots_trigscint
// Author:   Katherine Myers
// email :   kamyers@jlab.org
// Date:     8/Jan/11
//

void plot(UInt_t event_start=-1, 
		  UInt_t event_end=-1,Int_t run_number=200113)
{
 gROOT->Reset();

  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES"), run_number));  
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      return;
    }
  else
    {
      QwEvent* fEvent=0;
      TTree* event_tree = (TTree*) file->Get("event_tree");
      TBranch* trig_branch=event_tree->GetBranch("trigscint");
      TCanvas *c1 = new TCanvas("c1","TS Scalers",800,800);
      c1->Divide(2,3);
      c1->cd(1);
      event_tree->Draw("ts1m_sca","ts1m_sca>0");
      c1->cd(2);
      event_tree->Draw("ts2m_sca","ts2m_sca>0");
      c1->cd(3);
      event_tree->Draw("ts1p_sca","ts1p_sca>0");
      c1->cd(4);
      event_tree->Draw("ts2p_sca","ts2p_sca>0");
      c1->cd(5);
      event_tree->Draw("ts1mt_sca","ts1mt_sca>0");
      c1->cd(6);
      event_tree->Draw("ts2mt_sca","ts2mt_sca>0");
      
      TCanvas *c2 = new TCanvas("c2","TS f1 spectra",800,800);
      c2->Divide(2,3);
      c2->cd(1);
      event_tree->Draw("ts1m_f1>>h1(400,-300,-100)","ts1m_f1!=0");
      c2->cd(2);
      event_tree->Draw("ts2m_f1>>h2(400,-300,-100)","ts2m_f1!=0");
      c2->cd(3);
      event_tree->Draw("ts1p_f1>>h3(400,-300,-100)","ts1p_f1!=0");
      c2->cd(4);
      event_tree->Draw("ts2p_f1>>h4(400,-300,-100)","ts2p_f1!=0");
      c2->cd(5);
      event_tree->Draw("ts1mt_f1>>h5(400,-200,-100)","ts1mt_f1!=0");
      c2->cd(6);
      event_tree->Draw("ts2mt_f1>>h6(400,-200,-100)","ts2mt_f1!=0");
      
      TCanvas *c3 = new TCanvas("c3","TS ADC spectra",800,1000);
      c3->Divide(1,2);
      c3->cd(1);
      event_tree->Draw("ts1m_adc>>h7(2000,0,4000)","ts1mt_f1!=0");
      event_tree->Draw("ts1p_adc>>h8(2000,0,4000)","ts1mt_f1!=0");
      event_tree->Draw("ts2m_adc>>h9(2000,0,4000)","ts2mt_f1!=0");
      event_tree->Draw("ts2p_adc>>h10(2000,0,4000)","ts2mt_f1!=0");
      event_tree->Draw("ts1m_adc>>h11(500,100,1100)","ts1mt_f1!=0");
      event_tree->Draw("ts1p_adc>>h12(500,100,1100)","ts1mt_f1!=0");
      event_tree->Draw("ts2m_adc>>h13(500,100,1100)","ts2mt_f1!=0");
      event_tree->Draw("ts2p_adc>>h14(500,100,1100)","ts2mt_f1!=0");
      h7->SetTitle("TS ADC Spectra - full range");
      h8->SetLineColor(2);
      h9->SetLineColor(3);
      h10->SetLineColor(4);
      gPad->SetLogy();
      h7->Draw();
      h8->Draw("same");
      h9->Draw("same");
      h10->Draw("same");
      leg2 = new TLegend(0.6,0.7,0.89,0.89);
      leg2->AddEntry(h7,"ts1m","l");
      leg2->AddEntry(h8,"ts1p","l");
      leg2->AddEntry(h9,"ts2m","l");
      leg2->AddEntry(h10,"ts2p","l");
      leg2->SetFillColor(0);
      leg2->Draw();
      
      c3->cd(2);
      TPad *bot = new TPad("bot","",0,0,1,1);
      bot->Draw();
      bot->Divide(2,1);
      bot->cd(1);
      h11->SetTitle("TS ADC Spectra - peak");
      h12->SetLineColor(2);
      h13->SetLineColor(3);
      h14->SetLineColor(4);
      h11->Draw();
      h12->Draw("same");
      h13->Draw("same");
      h14->Draw("same");
      leg = new TLegend(0.6,0.7,0.89,0.89);
      leg->AddEntry(h11,"ts1m","l");
      leg->AddEntry(h12,"ts1p","l");
      leg->AddEntry(h13,"ts2m","l");
      leg->AddEntry(h14,"ts2p","l");
      leg->SetFillColor(0);
      leg->Draw();
      bot->cd(2);
      gPad->SetLogy();
      h11->Draw();
      h12->Draw("same");
      h13->Draw("same");
      h14->Draw("same");
      leg2->Draw();
      */
	}
}
void plots_trigscint(TString name="") {
 TString file_dir;
 file_dir = gSystem->Getenv("QWSCRATCH");
 file_dir += "/rootfiles/";
 
 if(name.IsNull()) {
   printf("There is no root file\n");
   exit(-1);
 }
 else {
   TFile *file =  new TFile(Form("%s%s", file_dir.Data(), name.Data()));
 }
 gStyle->SetPalette(1); 

 gROOT->Reset();

 plot();
}
