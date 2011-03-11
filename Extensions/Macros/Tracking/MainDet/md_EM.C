void md_EM(string file)
{
const int num_dets = 16;
const string md_dets[num_dets]={
"maindet.md1p_adc",
"maindet.md2p_adc",
"maindet.md3p_adc",
"maindet.md4p_adc",
"maindet.md5p_adc",
"maindet.md6p_adc",
"maindet.md7p_adc",
"maindet.md8p_adc",
"maindet.md1n_adc",
"maindet.md2n_adc",
"maindet.md3n_adc",
"maindet.md4n_adc",
"maindet.md5n_adc",
"maindet.md6n_adc",
"maindet.md7n_adc",
"maindet.md8n_adc"
};


gStyle->SetTitleFontSize(0.1);
TFile *f = new TFile(Form("$QW_ROOTFILES/%s",file));
TCanvas *c1 = new TCanvas("c1","",1100,900);

TH1F *h[num_dets];

c1->Divide(4,4);

for (int i=0;i<num_dets;i++)
{  
	c1->cd(i+1);
	h[i] = new TH1F(Form("h_%i",i),"",100,0,0);
	h[i]->SetDirectory(0);
	gPad->SetLogy(1);
//	event_tree->Draw(Form("%s>>h_%i(100,0,4096)",md_dets[i],i),Form("%s!=0",md_dets[i]));
	event_tree->Draw(Form("%s>>h_%i(100,0,4096)",md_dets[i],i),Form("%s!=0",md_dets[i]));
//	event_tree->Draw(Form("%s>>h_%i",md_dets[i],i));
	TH1F *h_temp = (TH1F*)gPad->GetPrimitive(Form("h_%i",i));
//	h_temp->SetRange(0,4096);
//	h_temp->Draw();
	c1->Update();
};









}





















