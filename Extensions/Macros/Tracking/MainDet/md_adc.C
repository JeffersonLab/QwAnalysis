void md_adc(TString file)
{
const int num_dets = 16;
const TString md_dets[num_dets]={
"maindet.md1p_adc","maindet.md2p_adc","maindet.md3p_adc","maindet.md4p_adc",
"maindet.md5p_adc","maindet.md6p_adc","maindet.md7p_adc","maindet.md8p_adc",
"maindet.md1m_adc","maindet.md2m_adc","maindet.md3m_adc","maindet.md4m_adc",
"maindet.md5m_adc","maindet.md6m_adc","maindet.md7m_adc","maindet.md8m_adc"
};

gStyle->SetTitleFontSize(0.1);
TFile *f = new TFile(Form("$QW_ROOTFILES/%s",file.Data()));

TCanvas *c1 = new TCanvas("c1","",1100,900);
TH1F *h[num_dets];
c1->Divide(4,4);

for (int i=0;i<num_dets;i++)
{  
	c1->cd(i+1);
	h[i] = new TH1F(Form("h_%i",i),"",100,0,0);
	h[i]->SetDirectory(0);
	gPad->SetLogy(1);
	event_tree->Draw(Form("%s>>h_%i(100,0,4096)",md_dets[i].Data(),i),Form("%s>0",md_dets[i].Data()));
	TH1F *h_temp = (TH1F*)gPad->GetPrimitive(Form("h_%i",i));
	c1->Update();
};

/*
TCanvas *c2 = new TCanvas("c2","",1100,900);
TH1F *h2[num_dets];
c2->Divide(4,4);

for (int i=0;i<num_dets;i++)
{  
	c2->cd(i+1);
	//h2[i] = new TH1F(Form("h2_%i",i),"",100,0,0);
	//h2[i]->SetDirectory(0);
	gPad->SetLogy(0);
	event_tree->Draw(Form("%s:CodaEventNumber>>h2_%i",md_dets[i].Data(),i),"");
	TH1F *h2_temp = (TH1F*)gPad->GetPrimitive(Form("h2_%i",i));
	c2->Update();
};

*/






}





















