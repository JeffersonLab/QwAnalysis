#include "TFile.h"
#include <iostream>
#include <fstream>
#include <vector>
//
//

Int_t extractRawAsym(Int_t runPeriod=1, TString regScheme="on_set11", TString detector="qwk_mdallbars"){

//TFile *rootfile = new TFile(Form("/home/smacewan/QWeak/scratch/run%iasym/HYDROGEN-CELL_%s_tree.root",runPeriod,regScheme.Data()));
TFile *rootfile = new TFile(Form("/group/qweak/QwAnalysis/db_rootfiles/run%i/parity/HYDROGEN-CELL_%s_tree.root",runPeriod,regScheme.Data()));
TTree *tree = (TTree*)rootfile->Get("tree");

Int_t nRuns = tree->GetEntries();

//-----------------------------------------------------------------------------------
//Determine slug ranges
vector <Int_t> slug;//contains slug numbers
vector <Int_t> slugStart;//contains starting run for the slug
vector <Int_t> slugEnd;//contains ending run for that slug

Int_t oldSlug=0;//tmp variable
Int_t newSlug=0;//tmp variable

tree->SetBranchAddress("slug",&newSlug);
for(Int_t i=0; i<nRuns; i++){
	tree->GetEntry(i);
	if(oldSlug!=newSlug && newSlug>0 && newSlug<400){//only looks at LH2 slugs
		oldSlug=newSlug;
		slug.push_back(oldSlug);
		slugStart.push_back(i);
		if(i!=0) slugEnd.push_back(i-1);
	}
}//loop over all runs and fill slug vectors when the slug number changes.
slugEnd.push_back(nRuns-1);//last entry

Int_t nSlugs=slug.size();//total number of slugs
//-----------------------------------------------------------------------------------
//Compute averages
	cout<<"Done this part too"<<endl;
vector <Double_t> avg;//contains error-weighted averages
vector <Double_t> err;//contains the error on the weighted averages
vector <Int_t> runlets;//total number of runlets in a given slug
vector <Int_t> sign_correction;//to contain the sign_correction factor for that slug

Double_t temp_branch[4];//tmp variable
Int_t sign;//temp variable for physics sign correction factor

	tree->SetBranchAddress(Form("asym_%s",detector.Data()),&temp_branch);
	tree->SetBranchAddress("sign_correction",&sign);

	cout<<"Averaging on channel "<<detector.Data()<<"..."<<endl;
	for(Int_t i=0; i<nSlugs; i++){
		Double_t mean = 0;//accumulating variable
		Double_t sigma = 0;//accumulating variable
		Int_t numRunlets=0;//accumulating variable
		Int_t tmpsign=1;
		for(Int_t j=slugStart[i]; j<=slugEnd[i]; j++){
			tree->GetEntry(j);
			if (temp_branch[0]==-1e6)//error from DB, skip it
				continue;
			numRunlets++;
            
			//temp_branch[0] and temp_branch[1] correspond to the "value" and "err" leaves of the branches.
			mean+=sign*temp_branch[0]/(temp_branch[1]*temp_branch[1]);//accumulate weighted mean
			sigma+=1/(temp_branch[1]*temp_branch[1]);//accumulate weight

			if(sign==-1)
				tmpsign=-1;
		}//loop over all runlets in a slug
	
		mean = mean/sigma;//compute weighted average
		sigma = 1/TMath::Sqrt(sigma);//compute error on the weighted average
		avg.push_back(mean);
		err.push_back(sigma);
		runlets.push_back(numRunlets);
		sign_correction.push_back(tmpsign);
	}//loop over all slugs in a file
	tree->ResetBranchAddresses();


//-----------------------------------------------------------------------------------
// Write to file
ofstream outFile(Form("inputfiles/slugaverage_run%i_reg%s_%s.out",runPeriod,regScheme.Data(),detector.Data()));
for(Int_t i=0; i<nSlugs; i++){
	outFile<<slug[i]<<" "<<sign_correction[i]<<" "<<avg[i]<<" "<<err[i]<<" "<<runlets[i]<<endl;
	
}

return 1;

}



