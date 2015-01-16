#include "TFile.h"
#include "TLine.h"
#include "TChain.h"
#include "TPaveText.h"
#include "TString.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TROOT.h"
#include "TH1.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TObject.h"
#include "TObjArray.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <utility>

const int nWIEN = 12, nMON = 5;
const double unit_convert = 1000.0;

typedef struct branch_t{
  char *name;
  int wien;
  int sluglo;
  int slughi;
  int runlo;
  int runhi;
  double corr[nMON+1];
  double asym;
  double asym_err;
};
 
using namespace std;

int MakeTablesOfBModAndRegressionCorrections(bool new_monitors = 0)
{
  bool use_bmod_error_weights = 1;
  if( use_bmod_error_weights )
    cout<<"Using dithering corrected mdallbars errors as weights.\n";

  int wien_range_slug[nWIEN][2] = {{42, 58}, {59, 80}, {81, 98}, {99, 116}, 
				   {117, 136}, {137, 155}, {156, 171}, {172, 198}, 
				   {199, 225}, {226, 264}, {265, 306}, {307, 321}};
  int wien_range_run[nWIEN][2] = {{9939, 10187}, {10196, 11129}, {11131, 11390}, 
				  {11391, 11711}, {11714, 12162}, {13843, 14258}, 
				  {14260, 14683}, {15163, 15574}, {15581, 15986}, 
				  {16172, 16717}, {16718, 17505}, {18415, 18927}};
  double Wien[nWIEN] = {1,2,3,4,5,6,7,8,8.5,9,9.5,10};
  char *wien_name[nWIEN] = {"1 ","2 ","3 ","4 ","5 ","6 ","7 ","8a","8b"
			    ,"9a","9b","10"};

  TString NewMonitorList[nMON]={"diff_MX1", "diff_MX2","diff_MY1", "diff_MY2",
			     "diff_qwk_bpm3c12X"};
  TString Set11MonitorList[nMON]={"diff_qwk_targetX", "diff_qwk_targetXSlope",
			     "diff_qwk_targetY", "diff_qwk_targetYSlope",
			     "diff_qwk_bpm3c12X"};
  char *stem = (char*)(new_monitors? "_new_monitors" : "");
  //initialize vectors
  vector<branch_t>vInRaw, vOutRaw, vRaw, vNullRaw;
  vector<branch_t>vInReg, vOutReg, vReg, vNullReg;
  vector<branch_t>vInBMod, vOutBMod, vBMod, vNullBMod;
  for(int i=0;i<nWIEN;++i){
    branch_t temp;
    temp.name = wien_name[i];
    temp.wien = (int)Wien[i];
    temp.runlo = wien_range_run[i][0];
    temp.runhi = wien_range_run[i][1];
    temp.sluglo = wien_range_slug[i][0];
    temp.slughi = wien_range_slug[i][1];
    for(int i=0;i<nMON+1;++i){
      temp.corr[i] = 0;
    }
    temp.asym = 0;
    temp.asym_err = 0;
    vInRaw.push_back(temp);
    vOutRaw.push_back(temp);
    vNullRaw.push_back(temp);
    vRaw.push_back(temp);
    vInReg.push_back(temp);
    vOutReg.push_back(temp);
    vNullReg.push_back(temp);
    vReg.push_back(temp);
    vInBMod.push_back(temp);
    vOutBMod.push_back(temp);
    vNullBMod.push_back(temp);
    vBMod.push_back(temp);
  }

  //Get trees
  TChain *ch = new TChain("reduced_tree");
  TChain *ch_bmod = new TChain("corrected_tree");
  TChain *ch_reg = new TChain("set11_reg_tree");
  TChain *ch_bmod_slopes = new TChain("slopes");
  TChain *ch_reg_slopes[nMON];
  TChain *ch_db = new TChain("tree");
  ch->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run1/"
	       "hydrogen_cell_reduced_tree.root"));
  ch->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run2/"
	       "hydrogen_cell_reduced_tree.root"));
  ch_bmod->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run1/"
		    "hydrogen_cell_corrected_tree%s.root", stem));
  ch_bmod->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run2/"
		    "hydrogen_cell_corrected_tree%s.root", stem));
  ch_reg->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run1/"
		   "hydrogen_cell_set11_regressed_tree.root"));
  ch_reg->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run2/"
		   "hydrogen_cell_set11_regressed_tree.root"));
  ch_bmod_slopes->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run1/"
			   "hydrogen_cell_bmod_slopes_tree%s.root", stem));
  ch_bmod_slopes->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run2/"
			   "hydrogen_cell_bmod_slopes_tree%s.root", stem));
  ch_db->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run1/"
		  "HYDROGEN-CELL_off_tree.root"));
  ch_db->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run2/"
		  "HYDROGEN-CELL_off_tree.root"));
  for(int i=0;i<nMON;++i){
    TString temp = Set11MonitorList[i];
    temp.Replace(4,4,"");
    ch_reg_slopes[i] = new TChain("tree");
    ch_reg_slopes[i]->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run1/"
			       "wrt_%sHYDROGEN-CELL_on_set11_tree.root", 
			       temp.Data()));
    ch_reg_slopes[i]->Add(Form("/net/data2/paschkelab2/DB_rootfiles/run2/"
			       "wrt_%sHYDROGEN-CELL_on_set11_tree.root", 
			       temp.Data()));
  }
  ch->AddFriend(ch_bmod);
  ch->AddFriend(ch_reg);
  ch->AddFriend(ch_bmod_slopes);


  //Turn off all unnecessary branches and set the addresses
  Int_t slug, sign, hwp, run, good;
  ch->SetBranchStatus("*", 0);
  ch_db->SetBranchStatus("*", 0);
  for(int i=0;i<nMON;++i){
    ch_reg_slopes[i]->SetBranchStatus("*", 0);
    ch_reg_slopes[i]->SetBranchStatus("asym_qwk_mdallbars", 1);
  }

  ch->SetBranchStatus("good", 1);
  ch->SetBranchStatus("slug", 1);
  ch->SetBranchAddress("good", &good);
  ch->SetBranchAddress("slug", &slug);
  if(ch->GetEntries()!=ch_db->GetEntries()){
    cout<<"Incompatible trees. Exiting.\n";
    return -1;
  }
  double run_number_decimal;
  ch_db->SetBranchStatus("run_number", 1);
  ch_db->SetBranchStatus("run_number_decimal", 1);
  ch_db->SetBranchStatus("sign_correction", 1);
  ch_db->SetBranchStatus("ihwp_setting", 1);
  ch_db->SetBranchAddress("run_number", &run);
  ch_db->SetBranchAddress("run_number_decimal", &run_number_decimal);
  ch_db->SetBranchAddress("sign_correction", &sign);
  ch_db->SetBranchAddress("ihwp_setting", &hwp);

  TString MonitorList[nMON];
  if(new_monitors)
    for(int i=0;i<nMON;++i)MonitorList[i] = NewMonitorList[i];
  else 
    for(int i=0;i<nMON;++i)MonitorList[i] = Set11MonitorList[i];

  TString monitor[nMON];
  int slopes_exist;
  double bmod_slope[nMON], reg_slope[nMON];
  for(int i=0;i<nMON;++i){
    monitor[i] = MonitorList[i];
    if(monitor[i].Contains("diff_qwk_"))
      monitor[i].Replace(0,9,"");
    if(monitor[i].Contains("diff_"))
      monitor[i].Replace(0,5,"");
    ch->SetBranchStatus(Form("mdallbars_%s",monitor[i].Data()), 1);
    ch->SetBranchStatus(MonitorList[i].Data(), 1);
    ch->SetBranchAddress(Form("mdallbars_%s",monitor[i].Data()), &bmod_slope[i]);
    if(new_monitors){
      ch->SetBranchStatus(Set11MonitorList[i].Data(), 1);
    }
  }
  ch->SetBranchStatus("slopes_exist", 1);
  ch->SetBranchAddress("slopes_exist", &slopes_exist);
  ch->SetBranchStatus("asym_qwk_mdallbars", 1);
  ch->SetBranchStatus("corrected_asym_qwk_mdallbars", 1);
  ch->SetBranchStatus("reg_asym_qwk_mdallbars", 1);



  for(int i=0;i<ch->GetEntries();++i){
    ch->GetEntry(i);
    ch_db->GetEntry(i);
    for(int imon=0;imon<nMON;++imon){
      ch_reg_slopes[imon]->GetEntry(i);
      reg_slope[imon] = ch_reg_slopes[imon]->GetBranch("asym_qwk_mdallbars")->
	GetLeaf("value")->GetValue();
    }

    if(i%1000==0)cout<<"Processing entry "<<i<<" of "<<ch->GetEntries()<<endl;

    //two runlets with wrong wien reversal values
    if(run_number_decimal == 15796.0 || run_number_decimal == 17403.05)continue;

    if(!(good && slopes_exist)) continue;
    if(!good)continue;
    int idx = -1;
    for(int iwien=0;iwien<nWIEN;++iwien){
      if(run >= wien_range_run[iwien][0] && run <= wien_range_run[iwien][1]){
	idx = iwien;
	break;
      }
    }
    if(idx==-1){
      cout<<"Issue with index for run "<<run<<". Exiting.\n";
      return -1;
    }
    double raw_asym = sign * ch->GetBranch("asym_qwk_mdallbars")->GetLeaf("value")
      ->GetValue();
    double bmod_asym = sign * ch->GetBranch("corrected_asym_qwk_mdallbars")
      ->GetLeaf("value")->GetValue();
    double reg_asym = sign * ch->GetBranch("reg_asym_qwk_mdallbars")
      ->GetLeaf("value")->GetValue();

    double raw_asym_err = ch->GetBranch("asym_qwk_mdallbars")->GetLeaf("err")
      ->GetValue();
    double bmod_asym_err = ch->GetBranch("corrected_asym_qwk_mdallbars")
      ->GetLeaf("err")->GetValue();
    double reg_asym_err = ch->GetBranch("reg_asym_qwk_mdallbars")
      ->GetLeaf("err")->GetValue();

    double bmod_corr[nMON], reg_corr[nMON], mon_diff, set11_mon_diff;
    for(int imon=0;imon<nMON;++imon){
      //only targetX(Y)Slope need conversion in regular target variable set
      double mon_convert = unit_convert;
      if((MonitorList[imon].Contains("target") &&
	 !MonitorList[imon].Contains("Slope")) ||
	 (MonitorList[imon].Contains("3c12X") && !new_monitors))
	 mon_convert = 1.0;
      mon_diff = ch->GetBranch(MonitorList[imon].Data())->GetLeaf("value")
      ->GetValue();
      bmod_corr[imon] = sign * mon_convert * bmod_slope[imon] * mon_diff;

      set11_mon_diff = ch->GetBranch(Set11MonitorList[imon].Data())
	->GetLeaf("value")->GetValue();
      reg_corr[imon] = sign * mon_convert * reg_slope[imon] * set11_mon_diff;
    }
    raw_asym *= unit_convert;   
    bmod_asym *= unit_convert; 
    reg_asym *= unit_convert; 
    raw_asym_err *=  unit_convert;   
    bmod_asym_err *= unit_convert; 
    reg_asym_err *= unit_convert; 
    raw_asym_err *= raw_asym_err;   
    bmod_asym_err *= bmod_asym_err;   
    reg_asym_err *= reg_asym_err; 

    if(use_bmod_error_weights && slopes_exist){
      reg_asym_err = bmod_asym_err;   
      raw_asym_err  = bmod_asym_err; 
    }else {
      raw_asym_err  = reg_asym_err; 
    }
  
    if(hwp){
      for(int imon=0;imon<nMON;++imon){
	vInBMod[idx].corr[imon] += bmod_corr[imon]/ bmod_asym_err;
	vInReg[idx].corr[imon] += reg_corr[imon]/ reg_asym_err;
      }
      vInRaw[idx].asym += raw_asym / raw_asym_err;
      vInRaw[idx].asym_err += 1.0 / raw_asym_err;
      vInBMod[idx].asym += bmod_asym / bmod_asym_err;
      vInBMod[idx].asym_err += 1.0 / bmod_asym_err;
      vInReg[idx].asym += reg_asym / reg_asym_err;
      vInReg[idx].asym_err += 1.0 / reg_asym_err;
    }else{
      for(int imon=0;imon<nMON;++imon){
	vOutBMod[idx].corr[imon] += bmod_corr[imon]/ bmod_asym_err;
	vOutReg[idx].corr[imon] += reg_corr[imon]/ reg_asym_err;
       }
      vOutRaw[idx].asym += raw_asym / raw_asym_err;
      vOutRaw[idx].asym_err += 1.0 / raw_asym_err;
      vOutBMod[idx].asym += bmod_asym / bmod_asym_err;
      vOutBMod[idx].asym_err += 1.0 / bmod_asym_err;
      vOutReg[idx].asym += reg_asym / reg_asym_err;
      vOutReg[idx].asym_err += 1.0 / reg_asym_err;
    }
  }

  //Finish weighted average calculations etc
  for(int iwien=0;iwien<nWIEN;++iwien){
    for(int imon=0;imon<nMON;++imon){
      vInBMod[iwien].corr[imon] /= vInBMod[iwien].asym_err;
      vInBMod[iwien].corr[nMON] += vInBMod[iwien].corr[imon];
      vInReg[iwien].corr[imon] /= vInReg[iwien].asym_err;
      vInReg[iwien].corr[nMON] += vInReg[iwien].corr[imon];
    }
    vInRaw[iwien].asym /= vInRaw[iwien].asym_err;
    vInRaw[iwien].asym_err = pow(vInRaw[iwien].asym_err, -0.5);
    vInBMod[iwien].asym /= vInBMod[iwien].asym_err;
    vInBMod[iwien].asym_err = pow(vInBMod[iwien].asym_err, -0.5);
    vInReg[iwien].asym /= vInReg[iwien].asym_err;
    vInReg[iwien].asym_err = pow(vInReg[iwien].asym_err, -0.5);


    for(int imon=0;imon<nMON;++imon){
      vOutBMod[iwien].corr[imon] /= vOutBMod[iwien].asym_err;
      vOutBMod[iwien].corr[nMON] += vOutBMod[iwien].corr[imon];
      vOutReg[iwien].corr[imon] /= vOutReg[iwien].asym_err;
      vOutReg[iwien].corr[nMON] += vOutReg[iwien].corr[imon];
    }
    vOutRaw[iwien].asym /= vOutRaw[iwien].asym_err;
    vOutRaw[iwien].asym_err = pow(vOutRaw[iwien].asym_err, -0.5);
    vOutBMod[iwien].asym /= vOutBMod[iwien].asym_err;
    vOutBMod[iwien].asym_err = pow(vOutBMod[iwien].asym_err, -0.5);
    vOutReg[iwien].asym /= vOutReg[iwien].asym_err;
    vOutReg[iwien].asym_err = pow(vOutReg[iwien].asym_err, -0.5);
  }

  //Calculate In+Out and In-Out
  for(int iwien=0;iwien<nWIEN;++iwien){
    vRaw[iwien].asym = 0.5 * (vInRaw[iwien].asym + vOutRaw[iwien].asym);
    vRaw[iwien].asym_err = sqrt(vInRaw[iwien].asym_err * vInRaw[iwien].asym_err + 
			        vOutRaw[iwien].asym_err *vOutRaw[iwien].asym_err);
    vRaw[iwien].asym_err *= 0.5;
    vNullRaw[iwien].asym = 0.5 * (vInRaw[iwien].asym - vOutRaw[iwien].asym);
    vNullRaw[iwien].asym_err = vRaw[iwien].asym_err;

    vReg[iwien].asym = 0.5 * (vInReg[iwien].asym + vOutReg[iwien].asym);
    vReg[iwien].asym_err = sqrt(vInReg[iwien].asym_err * vInReg[iwien].asym_err + 
			        vOutReg[iwien].asym_err *vOutReg[iwien].asym_err);
    vReg[iwien].asym_err *= 0.5;
    vNullReg[iwien].asym = 0.5 * (vInReg[iwien].asym - vOutReg[iwien].asym);
    vNullReg[iwien].asym_err = vReg[iwien].asym_err;

    vBMod[iwien].asym = 0.5 * (vInBMod[iwien].asym + vOutBMod[iwien].asym);
    vBMod[iwien].asym_err = sqrt(vInBMod[iwien].asym_err*vInBMod[iwien].asym_err +
			       vOutBMod[iwien].asym_err*vOutBMod[iwien].asym_err);
    vBMod[iwien].asym_err *= 0.5;
    vNullBMod[iwien].asym = 0.5 * (vInBMod[iwien].asym - vOutBMod[iwien].asym);
    vNullBMod[iwien].asym_err = vBMod[iwien].asym_err;
    for(int imon=0;imon<nMON+1;++imon){
      vRaw[iwien].corr[imon] = 0.5 * (vInRaw[iwien].corr[imon] +
				      vOutRaw[iwien].corr[imon]);
      vNullRaw[iwien].corr[imon] = 0.5 * (vInRaw[iwien].corr[imon] -
					  vOutRaw[iwien].corr[imon]);
      vReg[iwien].corr[imon] = 0.5 * (vInReg[iwien].corr[imon] +
				      vOutReg[iwien].corr[imon]);
      vNullReg[iwien].corr[imon] = 0.5 * (vInReg[iwien].corr[imon] -
					  vOutReg[iwien].corr[imon]);
      vBMod[iwien].corr[imon] = 0.5 * (vInBMod[iwien].corr[imon] +
				       vOutBMod[iwien].corr[imon]);
      vNullBMod[iwien].corr[imon] = 0.5 * (vInBMod[iwien].corr[imon] -
					   vOutBMod[iwien].corr[imon]);
    }
  }

  //Tabulate Results
  printf("Table 1. Sign Corrected Wien Average Comparison of Raw, Set11"
	 " Regression and Dithering results.\n");
  printf("Wien   Slug Range     Run Range  ||          Raw Asym_mdallbars "
	 "   Set11 Asym_mdallbars    "
	 "BMod Asym_mdallbars");
  for(int imon=0;imon<nMON;++imon){
    if(MonitorList[imon].Contains("diff_qwk_"))
      MonitorList[imon].Replace(0,9,"");
    if(MonitorList[imon].Contains("diff_"))
      MonitorList[imon].Replace(0,5,"");
	printf("%14.14sCorr", MonitorList[imon].Data());
  }
  printf("     Net Correction\n");
  for(int iwien=0;iwien<nWIEN;++iwien){
    //in
    printf("%3s     %3i-%3i     %5i-%5i  || (IN )  %+10.2f  %6.2f"
	   "     %+10.2f  %6.2f     %+10.2f  %6.2f   ",
	   wien_name[iwien], wien_range_slug[iwien][0], 
	   wien_range_slug[iwien][1], wien_range_run[iwien][0],
	   wien_range_run[iwien][1], vInRaw[iwien].asym, 
	   vInRaw[iwien].asym_err, vInReg[iwien].asym, vInReg[iwien].asym_err, 
	   vInBMod[iwien].asym, vInBMod[iwien].asym_err);

    for(int imon=0;imon<nMON+1;++imon){
      printf("     %+13.3f",vInBMod[iwien].corr[imon]);
    }
    printf("\n");

    //out
    printf("                                 || (OUT)  %+10.2f  %6.2f"
	   "     %+10.2f  %6.2f     %+10.2f  %6.2f   ", vOutRaw[iwien].asym, 
	   vOutRaw[iwien].asym_err, vOutReg[iwien].asym, vOutReg[iwien].asym_err, 
	   vOutBMod[iwien].asym, vOutBMod[iwien].asym_err);

    for(int imon=0;imon<nMON+1;++imon){
      printf("     %+13.3f",vOutBMod[iwien].corr[imon]);
    }
    printf("\n");

    //in-out
    printf("                                 || (NET)  %+10.2f  %6.2f"
	   "     %+10.2f  %6.2f     %+10.2f  %6.2f   ", vRaw[iwien].asym, 
	   vRaw[iwien].asym_err, vReg[iwien].asym, vReg[iwien].asym_err, 
	   vBMod[iwien].asym, vBMod[iwien].asym_err);

    for(int imon=0;imon<nMON+1;++imon){
      printf("     %+13.3f",vBMod[iwien].corr[imon]);
    }
    printf("\n");

    //in-out
    printf("                                 || (NULL) %+10.2f  %6.2f"
	   "     %+10.2f  %6.2f     %+10.2f  %6.2f   ", vNullRaw[iwien].asym, 
	   vNullRaw[iwien].asym_err, vNullReg[iwien].asym, 
	   vNullReg[iwien].asym_err, vNullBMod[iwien].asym,
	   vNullBMod[iwien].asym_err);

    for(int imon=0;imon<nMON+1;++imon){
      printf("     %+13.3f",vNullBMod[iwien].corr[imon]);
    }
    printf("\n\n");
  }



  return 0;
}
