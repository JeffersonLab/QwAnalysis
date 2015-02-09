#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TMath.h"


#include <vector>
#include <iostream>
#include <fstream>
#include <utility>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////
//Author: Don Jones
//Date: April 14, 2014
//Program averages runlet average trees to produce slug or run average trees. 
//Averages are weighted by statistical error of mdallbars.
//
//Requires that $DB_ROOTFILES be set to the directory of the DB rootfiles.
//
//Usage: CreateAverageDBFriendTree(Int_t run_period, Bool_t run_average,
//				    TString filename, TString treename)
//run_period:  1 or 2
//run_average: 1 is run_average, 0 is slug_average
//filename:    name of rootfile to average
//treename:    name of tree in rootfile
//
//Example: CreateAverageDBFriendTree(1,0,"HYDROGEN-CELL_on_tree.root","tree")
//         creates a slug average of "HYDROGEN-CELL_on_tree.root" for Run 1
//         called "slug_averaged_HYDROGEN-CELL_on_tree.root".
//
//NOTE: not all integer leaves are copied but can be easily added as necessary.
//
// Compile as g++ `root-config --glibs --cflags` -ggdb -O0 -Wall -Wextra -o avg CreateAverageDBFriendTree.cpp
///////////////////////////////////////////////////////////////////////////////

const Int_t nRUNS=20000, nSLUGS=325;
const Double_t ERROR = -1.0e6;

typedef struct Branch_t{
    double value;
    double err;
    double rms;
    int n;

};
typedef struct mytype_t{
    bool isInt;
    TString name;
};

using namespace std;

Int_t CreateAverageDBFriendTree(Int_t run_period, Bool_t run_average, TString dirname, TString filename, TString treename) 
{
    Bool_t debug = 0;

    //Get tree to be averaged
    /////////////////////////////////////////////////////////
    TFile *file = new TFile(Form("%s/%s",dirname.Data(),
			    "HYDROGEN-CELL_offoff_tree.root"));
    TTree *tree = (TTree*)file->Get("tree");

    TFile *average_file = new TFile(Form("%s/%s",dirname.Data(),filename.Data()));
    TTree *average_tree = (TTree*)average_file->Get(treename.Data());
    /////////////////////////////////////////////////////////


    //Get list of branches
    /////////////////////////////////////////////////////////
    vector<mytype_t> leafnames;
    vector<TString> branchnames;
    Int_t nInt = 0, nDouble = 0;
    TObjArray *brArr = average_tree->GetListOfBranches();
    TObjArray *lfArr = average_tree->GetListOfLeaves();
    for(int ibranch=0; ibranch<brArr->GetSize(); ++ibranch){
        Bool_t isleaf = false;
        mytype_t temp;
        for(int ileaf=0; ileaf<lfArr->GetSize(); ++ileaf){
            TString brname = brArr->At(ibranch)->GetName();
            if(brname.CompareTo(lfArr->At(ileaf)->GetName())==0){
	            TString lfname = lfArr->At(ileaf)->ClassName();
	            temp.isInt = lfname.CompareTo("TLeafI")==0;
	            temp.name = lfArr->At(ileaf)->GetName();
	            leafnames.push_back(temp);
	            isleaf = 1;
	            if(lfname.CompareTo("TLeafI"))++nDouble;
	            else ++nInt;
	            break;
            }
        }
        if(!isleaf)
            branchnames.push_back(brArr->At(ibranch)->GetName());
    }
    cout<<"Leaves:"<<nInt<<" "<<nDouble<<endl;
    for(size_t i=0; i<leafnames.size();++i)
        cout<<leafnames.at(i).name.Data()<<" "<<
            (leafnames.at(i).isInt ? "I" : "D")<<endl;

    //cout<<"Branches:"<<endl;
    //for(size_t i=0; i<branchnames.size();++i)
    //    cout<<branchnames.at(i)<<endl;
    /////////////////////////////////////////////////////////


    //Initialize vectors.run_number
    /////////////////////////////////////////////////////////
    Branch_t initval;
    initval.value = 0;
    initval.rms = 0;
    initval.err = 0;
    initval.n = 0;

    Int_t N = (run_average ? nRUNS : nSLUGS);
    vector<Int_t>vNRunlets, vSlug;
    for(int i=0;i<N;++i){
        vNRunlets.push_back(0);
        vSlug.push_back(0);
    }
    vector<vector<Branch_t> >vBranches;
    vector<vector<Double_t> >vWeights;
    vBranches.resize(branchnames.size());
    vWeights.resize(branchnames.size());
    for(int i=0;i<int(branchnames.size());++i){
        for(int j=0;j<N;++j){
            vBranches.at(i).push_back(initval);
            vWeights.at(i).push_back(0);
        }
    }

    cout<<"All initialized and ready!"<<endl;
    /////////////////////////////////////////////////////////


    //Loop over tree filling vectors with MD weighted averages.
    //////////////////////////////////////////////////////////
    const int nINTLEAVES = 6;
    TString integerLeaves[nINTLEAVES] = {"ihwp_setting","phwp_setting",
		"precession_reversal","wien_reversal",
		"sign_correction", "run_number"};
    TObjArray *obj = average_tree->GetListOfBranches();
    TString mdallbars;
    for(int i=0;i<obj->GetSize();++i){
        mdallbars = obj->At(i)->GetName();
        if( mdallbars.Contains("asym_qwk_mdallbars") )
            break;
    }
    if( !mdallbars.Contains("asym_qwk_mdallbars") ){
        cout<<"Leaf asym_qwk_mdallbars not found. Exiting.\n";
        return -1;
    }

    Int_t slug, run_number;
	
	//VARIABLES FOR SCOTT'S CUT
	Int_t runlet_id_test;
    Int_t cut_runlets[10]={146421,190229,190230,190246,190247,190252,190253,190255,190256,190258};

	Int_t size = (run_average? nINTLEAVES:nINTLEAVES-1), nrunlets = 0;
    vector<vector<Int_t> >vLeaves;
    vLeaves.resize(size);
    for(int i=0;i<size;++i)
        vLeaves[i].resize((run_average ? nRUNS : nSLUGS));
    Int_t *intLeaves = new Int_t[size];
    tree->SetBranchAddress("slug",&slug);
    for(int i=0;i<size;++i){
        tree->SetBranchAddress(integerLeaves[i].Data(), &intLeaves[i]);
    }
    for(int ient=0; ient<tree->GetEntries(); ++ient){
        if(ient%1000==0)cout<<"Processing entry "<<ient<<endl;
        tree->GetEntry(ient);
        average_tree->GetEntry(ient);
        run_number = (int)tree->GetLeaf("run_number")->GetValue();
        int idx = (run_average ? run_number : slug);

        //don't include values with no entries
        if(tree->GetBranch("asym_qwk_charge")->GetLeaf("n")->GetValue()<1){
            continue;
        }

		//SCOTT'S CUT::CUTTING ON SCALED MAIN DETECTOR WIDTH < 240 ppm BASED ON PREVIOUSLY DETERMINED runlet_ids
		runlet_id_test=tree->GetLeaf("runlet_id")->GetValue();
		if(find(cut_runlets,cut_runlets+10,runlet_id_test)!=cut_runlets+sizeof(cut_runlets)/sizeof(Int_t)){
			cout<<"Cutting out runlet "<<runlet_id_test<<endl;
			continue;
		}

        //don't include slugs that aren't LH2
        if(!(slug>=42&&slug<=321))continue;
        double err_MD = average_tree->GetBranch(mdallbars.Data())->
            GetLeaf("err")->GetValue();

        //don't include errors
        if(err_MD==ERROR)continue;

        ++vNRunlets[idx];
        vSlug[idx] = slug;

        for(int ileaf=0;ileaf<size;++ileaf){
            vLeaves[ileaf][idx] = intLeaves[ileaf];
            //       cout<<ient<<" "<<slug<<" "<<integerLeaves[ileaf].Data()
            // 	  <<": "<<intLeaves[ileaf]<<endl;
        }

        for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
            double value = average_tree->GetBranch(branchnames[ibranch].Data())->
	            GetLeaf("value")->GetValue();
            double err = average_tree->GetBranch(branchnames[ibranch].Data())->
	            GetLeaf("err")->GetValue();
            int n = (int)average_tree->GetBranch(branchnames[ibranch].Data())->
	            GetLeaf("n")->GetValue();
	        //	cout<<branchnames[ibranch].Data()<<":"<<value<<" "<<err<<" "<<n
            //	    <<"  "<<err_MD<<endl;
            vBranches[ibranch][idx].value += value / (err_MD * err_MD);
            vBranches[ibranch][idx].err += pow(err / (err_MD * err_MD) , 2);
            vBranches[ibranch][idx].rms += pow(value / err_MD, 2);
            vWeights[ibranch][idx] += pow(err_MD , -2);
            vBranches[ibranch][idx].n += n;

        }
    }

    for(int i=0;i<=(run_average ? nRUNS : nSLUGS);++i){
        for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
            if(vBranches[ibranch][i].n>0){
	            vBranches[ibranch][i].value /=  vWeights[ibranch][i];
	            if(vNRunlets[i]>1){
	                vBranches[ibranch][i].rms /=  vWeights[ibranch][i];
	                vBranches[ibranch][i].rms -= pow(vBranches[ibranch][i].value , 2);
	                vBranches[ibranch][i].rms = pow( vBranches[ibranch][i].rms, 0.5);
	            }else
	                vBranches[ibranch][i].rms = 0;
	            vBranches[ibranch][i].err = pow(vBranches[ibranch][i].err, 0.5);
	            vBranches[ibranch][i].err /= vWeights[ibranch][i];
            }
        }
    }
    //////////////////////////////////////////////////////////


    //create new average tree
    //////////////////////////////////////////////////////////
    TFile *new_file = new TFile(Form("%s/cut_%s_averaged_%st", dirname.Data(),
				(run_average ? "run" : "slug"),
				filename.Data()), "recreate");
    TTree *new_tree = new TTree(treename.Data(),treename.Data());
    Branch_t *bBranches = new Branch_t[int(branchnames.size())]; 
    TBranch *brSlug = new_tree->Branch("slug", &slug,"slug/I"); 
    TBranch *brNRunlets = new_tree->Branch("nRunlets", &nrunlets,"nRunlets/I"); 
    TBranch **brILeaves = new TBranch*[size]; 
    TBranch **branches = new TBranch*[int(branchnames.size())]; 

    for(int ileaf=0;ileaf<size;++ileaf){
        brILeaves[ileaf] = new_tree->Branch(integerLeaves[ileaf].Data(), 
				&intLeaves[ileaf],
				Form("%s/I",integerLeaves[ileaf].Data()));
    }
    for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
        branches[ibranch] = new_tree->Branch(branchnames[ibranch].Data(), 
				&bBranches[ibranch],
				"value/D:err/D:rms/D:n/I");
    }
    for(int i=0;i<(int)vNRunlets.size();++i){
        if(vBranches[0][i].n>0){
            slug = vSlug[i];
            nrunlets = vNRunlets[i];
            for(int ileaf=0;ileaf<size;++ileaf){
	            intLeaves[ileaf] = vLeaves[ileaf][i];
            }
            for(int ibranch=0;ibranch<int(branchnames.size());++ibranch){
	            bBranches[ibranch].value = vBranches[ibranch][i].value;
	            bBranches[ibranch].err = vBranches[ibranch][i].err;
	            bBranches[ibranch].rms = vBranches[ibranch][i].rms;
	            bBranches[ibranch].n = vBranches[ibranch][i].n;
            }
            new_tree->Fill();
        }
    }
    //  new_tree->Print();
    new_tree->Write("",TObject::kOverwrite);
    //  new_tree->AutoSave();
    file->Close();
    average_file->Close();
    new_file->Close();
    delete  file;
    delete average_file;
    delete new_file;
    //////////////////////////////////////////////////////////


    return 0;

}

int main(Int_t argc, Char_t* argv[]) {
    CreateAverageDBFriendTree(atoi(argv[3]), 1, argv[1], argv[2], "tree");
    CreateAverageDBFriendTree(atoi(argv[3]), 0, argv[1], argv[2], "tree");
    return 0;
}

