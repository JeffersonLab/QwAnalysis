// 2011-3-23  B. Waidyawansa
// Updated for helicity magnet block scan plots.
// This macro plots two plots of X and Y position differences w.r.t scandata1. The assumption is
// scandata1 == DAC counts for helicity magnet changes.
// Each plot have 5 fraphs: hw_sum, block0, block1, block2 and block3.
// The slopes obtained from the fits of these graphs are saved in to a text files inside output directory.
// The canvas is also saved in to the same directory as a .png image.
// 
void plot_element(std::ofstream& file,TString devname, TString localcut, TTree*tree);



void HM_block_scan(Int_t run_number=0,TString device="0l05",TString usercut ="1" ) {
	if (run_number =="0") {
		printf("Usage:\n\t.x HelicityMagent_block_scan.C(runnum,[device],[cut])\n");
		printf("OR\t.x HelicityMagent_block_scan.C(filename,[device],[cut])\n\n");
		printf("defaults:\n\t[device] = \"0l05X\"\n\t[cut]    = \"1\"\n");
		return;
	}

	Int_t ihwp = 4;

	gROOT->Reset();
	Int_t mag=1;
	while (ihwp!=0 && ihwp!=1) {
		printf("Please enter the IHWP position: (1) IN (0) OUT\n");
		cin >> ihwp;
		printf("Please enter the magnet: 1,2,3 or 4\n");
		cin >> mag;
	}

    TString scantype;
    TString gtitle;
    TString plotname;
	TString ihwppos;

	if (ihwp==0) {
		ihwppos="OUT";
	} else {
		if (ihwp==1) {
			ihwppos="IN";	
		} else {
			return;
		}
	}
	gtitle = "HM block scan (IHWP ";
	gtitle += ihwppos;
	gtitle += "), using HM magnet ";
	gtitle += Form("%i",mag);

	if (run_number!=1) {
		gtitle += ", Run ";
		gtitle += run_number;
	} 

    plotname = "hm_block_scan_";
    plotname += run_number;


    
	// define style here 
	// general parameters
	gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
	gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
	gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
  
	// canvas parameters
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameBorderSize(0);
	gStyle->SetFrameFillColor(10);
	// pads parameters
	//  gStyle->SetPadColor(39); 
	gStyle->SetPadColor(0); 
	gStyle->SetPadBorderMode(0);
	gStyle->SetPadBorderSize(0);
	gStyle->SetPadBottomMargin(0.18);
	gStyle->SetPadRightMargin(0.05);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.04,"x");
	gStyle->SetLabelSize(0.04,"y");
	gROOT->ForceStyle();  


	/*Open the rootfile/rootfiles*/
	TString  Rfilename = Form("qwinjector_%i.000.root", run_number);
	TFile  * Rfile     = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));
	if(Rfile==NULL) exit(1);
	

	TTree* tree = Rfile->Get("Hel_Tree");

	/*Open a file to store the output*/
	std::ofstream file;
	TString filename = Form("output/%i_hm_slopes.txt",run_number);
	file.open(filename);
	
	time_t theTime;
	time(&theTime);
	file<<"!  Date of analysis "<<ctime(&theTime)<<std::endl;
	file<<"!  device\tX slopes \tY slopes"<<std::endl;
	std::cout<<"Results will be stored in file "<<filename<<std::endl;
	


	TCanvas *canvas = new TCanvas("canvas",gtitle,1200,600);

	TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
	TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
	pad1->Draw();
	pad2->Draw();

	pad1->cd();

	TPaveText* pt2 = new TPaveText(0.04,0.04,0.96,0.50,"brNDC");
	TString text = gtitle;
	TText*t1 = new TText(0.25,0.3,text);
	t1->SetTextSize(0.6);
	t1->Draw();
  
	pad2->cd();
	pad2->Divide(2,1);

	pad2->cd(1);
	plot_element(file,"qwk_"+device+"X",usercut,tree);
	pad2->cd(2);
	plot_element(file,"qwk_"+device+"Y",usercut,tree);


	canvas->Print(Form("output/%s.png",plotname.Data()));
	file.close();
  
}

void plot_element(std::ofstream& file,TString devname, TString localcut, TTree*tree) {

	TString plotcommand;
	TString cut;

	Int_t icurRun=0;
	char buff[170];


    // plot desired  Aq
    TString titpre = "asym_";
    TString titsum = " (ppm) vs Scandata1";

    //draw hw_sum
	plotcommand = Form("diff_%s.hw_sum*1e6:scandata1>>h",devname.Data());
	// make cut
	cut =Form("ErrorFlag==0 && cleandata == 1 && diff_%s.Device_Error_Code == 0",devname.Data());
	std::cout<<plotcommand<<std::endl;
	std::cout<<cut<<std::endl;
	// fit h linearly
	tree->Draw(plotcommand,cut,"proff");
	TH1 *hw_sum = (TH1*)gDirectory->Get("h");
	if(hw_sum == NULL) exit(1);
	hw_sum->Fit("pol1");
	TF1 *f1 = hw_sum->GetFunction("pol1");
	f1->SetName("f1");
	if(!f1) exit(1);
	f1->SetLineWidth(2);
	f1->SetLineColor(kBlue);
	file << devname<<".hw_sum,\t\t"
		 << f1->GetParameter(1)  << std::endl; 

	hw_sum->GetXaxis()->CenterTitle();
	hw_sum->GetXaxis()->SetTitleOffset(1.2);
	hw_sum->GetYaxis()->SetTitleOffset(2.0);
	hw_sum->GetYaxis()->CenterTitle();
	hw_sum->GetXaxis()->SetTitleSize(0.05);
	hw_sum->GetYaxis()->SetTitle(Form("%s Differences (nm)",devname.Data()));
	hw_sum->GetXaxis()->SetTitle("DAC counts");

    // Draw block0
	plotcommand = Form("diff_%s.block0*1e6:scandata1>>block0",devname.Data());
	std::cout<<plotcommand<<std::endl;
	std::cout<<cut<<std::endl;
	tree->Draw(plotcommand,cut,"proff");
	TH1 *block0 = (TH1*)gDirectory->Get("block0");
	if(!block0) exit(1);
	block0->Fit("pol1");
	TF1 *f2 = block0->GetFunction("pol1");
	if(!f2)  exit(1);
	f2->SetName("f2");
	f2->SetLineWidth(2);
	f2->SetLineColor(kRed);
	file << devname<<".block0,\t\t"
		 << f2->GetParameter(1)  << std::endl; 
 

    // Draw block1
	plotcommand = Form("diff_%s.block1*1e6:scandata1>>block1",devname.Data());
	std::cout<<plotcommand<<std::endl;
	tree->Draw(plotcommand,cut,"proff");
	TH1 *block1 = (TH1*)gDirectory->Get("block1");
	if(!block1) exit(1);
	block1->Fit("pol1");
	TF1 *f3 = block1->GetFunction("pol1");
	if(!f3)  exit(1);
	f3->SetName("f3");
	f3->SetLineWidth(2);
	f3->SetLineColor(kGreen);
	file << devname<<".block1,\t\t"
		 << f3->GetParameter(1)  << std::endl;

	// Draw block2
	plotcommand = Form("diff_%s.block2*1e6:scandata1>>block2",devname.Data());
	std::cout<<plotcommand<<std::endl;
	tree->Draw(plotcommand,cut,"proff");
	TH1 *block2 = (TH1*)gDirectory->Get("block2");
	if(!block2) exit(1);
	block2->Fit("pol1");
	TF1 *f4 = block2->GetFunction("pol1");
	if(!f4)  exit(1);
	f4->SetName("f4");
	f4->SetLineWidth(2);
	f4->SetLineColor(kMagenta);
	file << devname<<".block2,\t\t"
		 << f4->GetParameter(1)  << std::endl;


	// Draw block3
	plotcommand = Form("diff_%s.block3*1e6:scandata1>>block3",devname.Data());
	std::cout<<plotcommand<<std::endl;
	tree->Draw(plotcommand,cut,"proff");
	TH1 *block3 = (TH1*)gDirectory->Get("block3");
	if(!block3) exit(1);
	block3->Fit("pol1");
	TF1 *f5 = block3->GetFunction("pol1");
	if(!f4)  exit(1);
	f5->SetName("f5");
	f5->SetLineWidth(2);
	f5->SetLineColor(kOrange-5);
	file << devname<<".block3,\t\t"
		 << f5->GetParameter(1)  << std::endl;

	hw_sum->DrawCopy();
	f1->DrawCopy("same");
	f2->DrawCopy("same");
	f3->DrawCopy("same");
	f4->DrawCopy("same");
	f5->DrawCopy("same");

	TLegend *legend = new TLegend(0.70,0.80,0.99,0.99,"","brNDC");
	legend->AddEntry("f1", "hw_sum", "l");
	legend->AddEntry("f2", "block0", "l");
	legend->AddEntry("f3", "block1", "l");
	legend->AddEntry("f4", "block2", "l");
	legend->AddEntry("f5", "block3", "l");
	legend->Draw();


}

/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
