/* Simple script that, for a given run, grab the pedestal from beam trips and
 * redo the pedistal subtraction.
 *
 * Currently needs better cuts for beam trip.
 *
 * Author: Wade S Duvall
 * Email: wsduvall@jlab.org
 *
 */

#include <TSystem.h>
#include <TROOT.h>
#include <TCanvas.h>
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TH1.h"
Int_t lumi_y(int run_number)
{
	gROOT->Reset();
	gStyle->SetOptStat(0221);
	 
	Int_t i;

	TChain *mps_tree = new TChain ("Mps_Tree");
	mps_tree->Add(Form("$QWSCRATCH/QwPass1_%i.*.trees.root",run_number));

	TString det_neg[8] = {
		"qwk_uslumi1neg", "qwk_uslumi3neg", 
		"qwk_uslumi5neg", "qwk_uslumi7neg"};
	TString det_pos[8] = {
		"qwk_uslumi1pos", "qwk_uslumi3pos",
		"qwk_uslumi5pos", "qwk_uslumi7pos"};

	Float_t ped_neg[8];
	Float_t ped_pos[8];

	TCanvas *can1 = new TCanvas("poopy");
	can1->Divide(2,2);
	for (i = 0; i < 4; i++)
	{
		can1->cd(i+1);
		TString draw = Form("%s.hw_sum_raw/%s.num_samples>>hn%d",det_neg[i].Data(),det_neg[i].Data(),i);
		TString cuts = Form("%s.Device_Error_Code==0 && qwk_charge < 1",det_neg[i].Data());
		cout << draw << endl;
		mps_tree->Draw(draw.Data(),cuts.Data());
		cout << Form("Getting mean for detector %s",det_neg[i].Data()) << endl;
		TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("hn%i",i));
		ped_neg[i] = htemp->GetMean();
		cout << "done pooping" << endl;
	}

	TCanvas *can2 = new TCanvas("poopyer");
	can2->Divide(2,2);
	for (i = 0; i < 4; i++)
	{
		can2->cd(i+1);
		TString draw = Form("%s.hw_sum_raw/%s.num_samples>>hp%d",det_pos[i].Data(),det_pos[i].Data(),i);
		TString cuts = Form("%s.Device_Error_Code==0 && qwk_charge < 1",det_pos[i].Data());
		cout << draw << endl;
		mps_tree->Draw(draw.Data(),cuts.Data());
		TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("hp%i",i));
		ped_pos[i] = htemp->GetMean();
		cout << "done pooping" << endl;
	}

	TCanvas *can3 = new TCanvas("poopyist");
	can3->Divide(2,2);
	for(i = 0; i < 4; i++)
	{
		can3->cd(i+1);
		TString draw = Form("((((%s.hw_sum_raw/%s.num_samples)-%f)*76.29e-6/qwk_charge)+(((%s.hw_sum_raw/%s.num_samples)-%f)*76.29e-6/qwk_charge))/2",det_neg[i].Data(), det_neg[i].Data(),ped_neg[i],det_pos[i].Data(),det_pos[i].Data(),ped_pos[i]);
		mps_tree->Draw(draw.Data(),"qwk_uslumi1pos.Device_Error_Code==0 && ErrorFlag==0");
	}

	return 0;
}
