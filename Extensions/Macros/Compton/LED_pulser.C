Double_t dac_min=0;
Double_t dac_max=248;
Double_t dac_step=8;
Double_t dac_minbin=dac_min-dac_step/2;
Double_t dac_maxbin=dac_max+dac_step/2;
Int_t dac_numbin=(dac_max-dac_min)/dac_step+1;

Double_t sum_min=-2000.0;
Double_t sum_max=10000.0;
Int_t sum_num=250;


TCanvas* c1 = new TCanvas();
c1->Divide(2,2);
c1->cd(1);
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():mps_counter","mps_counter%4==0")
c1->cd(2);
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():mps_counter","mps_counter%4==1")
c1->cd(3);
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():mps_counter","mps_counter%4==2")
c1->cd(4);
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():mps_counter","mps_counter%4==3")


TCanvas* c2_tmp = new TCanvas();

TH2F h0("h0","h0",dac_numbin,dac_minbin,dac_maxbin,sum_num,sum_min,sum_max)
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():sca_haptb_time_int>>h0","mps_counter%4==0")
TProfile* h0_pfx = h0.ProfileX()

TH2F h1("h1","h1",dac_numbin,dac_minbin,dac_maxbin,sum_num,sum_min,sum_max)
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():sca_haptb_time_int>>h1","mps_counter%4==1")
TProfile* h1_pfx = h1.ProfileX()

TH2F h2("h2","h2",dac_numbin,dac_minbin,dac_maxbin,sum_num,sum_min,sum_max)
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():sca_haptb_time_int>>h2","mps_counter%4==2")
TProfile* h2_pfx = h2.ProfileX()

TH2F h3("h3","h3",dac_numbin,dac_minbin,dac_maxbin,sum_num,sum_min,sum_max)
Mps_Tree->Draw("fadc_compton_samples.GetPedestal()*fadc_compton_samples.GetNumberOfSamples()-fadc_compton_samples.GetSum():sca_haptb_time_int>>h3","mps_counter%4==3")
TProfile* h3_pfx = h3.ProfileX()


TCanvas* c2 = new TCanvas();
c2->Divide(2,2);
c2->cd(1)
h0_pfx->SetLineColor(kBlack);
h0_pfx->Draw();
c2->cd(2)
h1_pfx->SetLineColor(kRed);
h1_pfx->Draw("same");
c2->cd(3)
h2_pfx->SetLineColor(kBlue);
h2_pfx->Draw("same");
c2->cd(4)
h3_pfx->SetLineColor(kGreen);
h3_pfx->Draw("same");

TCanvas* c3 = new TCanvas();
TH1F* hdiff = new TH1F("hdiff","hdiff",dac_numbin,dac_minbin,dac_maxbin)
hdiff->Sumw2();
hdiff->Add(h2_pfx,h1_pfx,1.0,-1.0);
hdiff->Draw();

TCanvas* c4 = new TCanvas();
TH1F* hzero = new TH1F("hzero","hzero",dac_numbin,dac_minbin,dac_maxbin)
hzero->Sumw2();
hzero->Add(h2_pfx,h1_pfx,1.0,-1.0);
hzero->Add(h0_pfx,-1.0);
hzero->Draw();

