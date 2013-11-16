/*
 * JoshTreeLib.cpp
 * Author: Josh Magee <magee@jlab.org>
 * Date:   2013-07-16
 *
 * Small program to walk QwDB rootfile trees for plotting purposes
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TLine.h>
#include <TArrow.h>
#include <TPaveLabel.h>
#include <TApplication.h>

#include "PlotLib.h"
#include "TreeLib.h"

using namespace std;


//get_wien_starts walks through the tree and determines where a
//new wien period starts
void get_wien_starts (TTree* tree) {
  int n_events = (int) tree->GetEntries();
  int wien, wien_slug;
//  double run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug", &wien_slug);
//  tree->SetBranchAddress("run_number_decimal", &run_number_decimal);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    if(i==0) {
      wien=wien_slug;
      printf("Wien %i \tRunlet %i\n",wien,i);
    }
    if (wien_slug==0) {
      printf("Wien is zero!! Runet %i\n",i);
      wien=wien_slug;
    }
    if (wien!=wien_slug) {
      printf("Wien %i \tRunlet %i\n",wien,i);
      wien=wien_slug;
    }
  }
}


void get_data_from_tree( TTree* tree, TString name, std::vector<double> *value, std::vector<int> *runlet ) {
  int n_events = (int) tree->GetEntries();
  int run_number_decimal;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("run_number_decimal", &run_number_decimal);
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    if ( temp_branch[0]==-1e6 || temp_branch[1]>10)
      continue;
    value->push_back(temp_branch[1]);
    runlet->push_back(i);
  }
}

void get_data_from_tree( TTree* tree, TString name, std::vector<double> *value ) {
  int n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || temp_branch[1]>10)
      continue;
    value->push_back(temp_branch[1]);
  }
}

void get_data_from_tree( TTree* tree, TString name, std::vector<double> *value, std::vector<double> *error) {
  int n_events = (int) tree->GetEntries();
  double temp_branch[4];
  cout << name.Data() <<endl;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    value->push_back(temp_branch[0]);
    error->push_back(temp_branch[1]);
  }
}


void get_wien_from_tree(int wien, TTree* tree, TString name, std::vector<double> *value ) {
  int n_events, slug, wien_slug, sign;
  n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress("wien_slug", &wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;
    value->push_back(sign*temp_branch[0]);
  }
}

void get_wien_from_tree(int wien, TTree* tree, TString name, std::vector<double> *value, std::vector<double> *error) {
  int n_events, slug, wien_slug, sign;
  n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress("wien_slug", &wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;
    value->push_back(sign*temp_branch[0]);
    error->push_back(temp_branch[1]);
  }
}


void get_wien_from_tree_friend(int wien, TTree* tree, TTree* friendtree, TString name, std::vector<double> *value, std::vector<double> *error) {
  int n_events = (int) tree->GetEntries();
  int n_events_friend = (int) friendtree->GetEntries();

  if (n_events != n_events_friend) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  int slug, wien_slug, sign;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress("wien_slug", &wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name, &temp_branch);

  double current[4], run_decimal_friend;
  friendtree->ResetBranchAddresses();
  friendtree->SetBranchAddress("yield_qwk_charge",&current);
  friendtree->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    friendtree->GetEntry(i);

    //skip bad entries
    if (run_number_decimal != run_decimal_friend ) {
      printf("You have a problem - your trees are not processing the same events for run: %f \t%f\n", run_number_decimal, run_decimal_friend);
      continue;
    }

    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;

    if (run_number_decimal==13957.25 || current[0]<130.0) {
      printf("Disregarded run %f \tCurrent %f\n",run_number_decimal,current[0]);
      continue;
    }
    value->push_back(sign*temp_branch[0]);
    error->push_back(temp_branch[1]);
  }
}


void get_wien_from_tree_friend(int wien, TTree* tree, TTree* friendtree, TString name, TString fName, std::vector<double> *value, std::vector<double> *fValue) {
  int n_events = (int) tree->GetEntries();
  int n_events_friend = (int) friendtree->GetEntries();

  if (n_events != n_events_friend) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  int slug, wien_slug, sign;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress("wien_slug", &wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name, &temp_branch);

  double ftemp[4], run_decimal_friend;
  friendtree->ResetBranchAddresses();
  friendtree->SetBranchAddress(fName, &ftemp);
  friendtree->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    friendtree->GetEntry(i);

    //skip bad entries
    if (run_number_decimal != run_decimal_friend ) {
      printf("You have a problem - your trees are not processing the same events for run: %f \t%f\n", run_number_decimal, run_decimal_friend);
      continue;
    }

    if ( temp_branch[0]==-1e6 || ftemp[0] == -1e6 || wien_slug != wien)
      continue;

    value ->push_back(sign*temp_branch[0]);
    fValue->push_back(ftemp[1]);
  }
}


void get_wien_from_tree_friend(int wien, TTree* tree, TTree* friendtree, TString name, TString fName, std::vector<double> *value, std::vector<double> *fValue, std::vector<double> *entries) {
  int n_events = (int) tree->GetEntries();
  int n_events_friend = (int) friendtree->GetEntries();

  if (n_events != n_events_friend) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  int slug, wien_slug, sign;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress("wien_slug", &wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name, &temp_branch);

  double ftemp[4], run_decimal_friend;
  friendtree->ResetBranchAddresses();
  friendtree->SetBranchAddress(fName, &ftemp);
  friendtree->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    friendtree->GetEntry(i);

    //skip bad entries
    if (run_number_decimal != run_decimal_friend ) {
      printf("You have a problem - your trees are not processing the same events for run: %f \t%f\n", run_number_decimal, run_decimal_friend);
      continue;
    }

    if ( temp_branch[0]==-1e6 || ftemp[0] == -1e6 || wien_slug != wien)
      continue;

    value ->push_back(sign*temp_branch[0]);
    fValue->push_back(ftemp[1]);
    entries->push_back(ftemp[3]);
//    printf("%f \t%i\n",ftemp[3],ftemp[3]);
  }
}

void get_data_from_tree_runlet(TTree* tree, TString name,
    std::vector<double> *value, std::vector<double> *error,
    std::vector<double> *runlet ) {

  int n_events = (int) tree->GetEntries();
  int sign, wien_slug;
  double run_number_decimal;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  tree->SetBranchAddress("wien_slug", &wien_slug);
  tree->SetBranchAddress("sign_correction", &sign);
  tree->SetBranchAddress("run_number_decimal", &run_number_decimal);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    if ( temp_branch[0]==-1e6 )
      continue;
    value->push_back(sign*temp_branch[0]);
    error->push_back(temp_branch[1]);
    runlet->push_back(i);
  }
}


void get_data_from_tree_runlet_decimal(TTree* tree, TString name,
    std::vector<double> *value, std::vector<double> *error,
    std::vector<double> *runlet ) {

  int n_events = (int) tree->GetEntries();
  int sign;
  double run_number_decimal;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  tree->SetBranchAddress("sign_correction", &sign);
  tree->SetBranchAddress("run_number_decimal", &run_number_decimal);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);

    if ( temp_branch[0]==-1e6 )
      continue;
    value->push_back(sign*temp_branch[0]);
    error->push_back(temp_branch[1]);
    runlet->push_back(run_number_decimal);
  }
}

void erase_null_entries(std::vector<int> *value) {
  for (int i=0; i< int( value->size() ); i++) {
    if ( value->at(i)==-1e6 ) {
      printf("Erasing value: %i\n",value->at(i));
      value->erase(value->begin()+i);
      i--;
    }
  }
}

void erase_null_entries(std::vector<double> *value, std::vector<double> *error) {
  if( value->size() != error->size() ) {
    std::cout <<"Vectors of different size passed to erase_null_entries. You lose. Good bye."
      <<std::endl;
    exit(1);
  }

  for (int i=0; i< int( value->size() ); i++) {
    if ( value->at(i)==-1e6 || error->at(i)==-1e6) {
      printf("Erasing value: %f \t error: %f\n",value->at(i),error->at(i));
      value->erase(value->begin()+i);
      error->erase(value->begin()+i);
      i--;
    }
  }
}

void erase_null_entries(std::vector<double> *value, std::vector<double> *error, std::vector<int> *runlet) {
  if( (value->size() != error->size()) || (value->size() != runlet->size()) ) {
    std::cout <<"Vectors of different size passed to erase_null_entries. You lose. Good bye."
      <<std::endl;
    exit(1);
  }

  for (size_t i=0; i<value->size() ;i++) {
    if (value->at(i)==-1e6 || error->at(i)==-1e6 ) {
      value->erase(value->begin()+i);
      error->erase(error->begin()+i);
      runlet->erase(runlet->begin()+i);
      i--;
    }
  }
}

void erase_null_entries(std::vector<double> *value, std::vector<double> *error, std::vector<double> *runlet) {
  if( (value->size() != error->size()) || (value->size() != runlet->size()) ) {
    std::cout <<"Vectors of different size passed to erase_null_entries. You lose. Good bye."
      <<std::endl;
    exit(1);
  }

  for (size_t i=0; i<value->size(); i++) {
    if ( value->at(i)==-1e6 || error->at(i)==-1e6) {
      value->erase(value->begin()+i);
      error->erase(error->begin()+i);
      runlet->erase(runlet->begin()+i);
      i--;
    }
  }
}

void get_single_value_from_tree(TTree *tree, TString name, std::vector<int> *value) {
  int n_events = (int) tree->GetEntries();
  int temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    value->push_back(temp_branch[0]);
  }
}

void get_single_value_from_tree(TTree *tree, TString name, std::vector<double> *value) {
  int n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    value->push_back(temp_branch[0]);
  }
}

void printInfo(TTree *tree, TString name) {
  int slug;
  int n_events;
  double value,error,rms,n;
  double temp_branch[4];

  n_events = (int) tree->GetEntries();
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress(name, &temp_branch);
  std::cout <<"Slug \tvalue \terror \trms \tn" <<std::endl;
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);

    if (temp_branch[0]==-1e6)
      continue;
    value =temp_branch[0];
    error =temp_branch[1];
    rms   =temp_branch[2];
    n     =temp_branch[3];
    printf("%i \t%f \t%f \t%f \t%f\n",slug,value,error,rms,n);
  }

}

void printInfo(TTree *tree, TString name, TString outfile) {
  int n_events = (int) tree->GetEntries();
  //int slug;
  int run_number, runlet_id;
//  double run_number_decimal;
  double value,error,rms;
  double temp_branch[4];

  tree->ResetBranchAddresses();
  //tree->SetBranchAddress("slug", &slug);
  //tree->SetBranchAddress("run_number_decimal", &run_number_decimal);
  tree->SetBranchAddress("run_number",&run_number);
  tree->SetBranchAddress("runlet_id",&runlet_id);
  tree->SetBranchAddress(name, &temp_branch);


  ofstream output;
  output.open(outfile);
  //output <<"Slug \tvalue \terror \trms \tn" <<std::endl;
  //output <<"Run_number \tvalue \terror \trms" <<std::endl;
  output <<"Run_number \trunlet_id \tvalue \terror \trms" <<std::endl;
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    if (temp_branch[0]==-1e6)
      continue;
    value =temp_branch[0];
    error =temp_branch[1];
    rms   =temp_branch[2];
    //n     =temp_branch[3];
    //output <<Form("%i \t%f \t%f \t%f \t%f\n",slug,value,error,rms,n);
    //output <<Form("%f \t%f \t%f \t%f \n",run_number_decimal,value,error,rms);
    output <<Form("%i \t%i \t%f \t%f \t%f \n",run_number,runlet_id,value,error,rms);
  }
  output.close();
  printf("Data written to file %s.\n",outfile.Data());
}


void get_rms_by_wien(int wien, TTree *tree, TString name, std::vector<double> *runlet, std::vector<double>*rms) {
  int n_events = (int) tree->GetEntries();
  int  wien_slug;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;

    runlet->push_back(run_number_decimal);
//    runlet->push_back(i);
    rms->push_back(temp_branch[2]);
  }
}


void get_data_by_wien(int wien, TTree *tree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events = (int) tree->GetEntries();
  int sign, wien_slug;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;

    value->push_back(sign*temp_branch[0]);
    runlet->push_back(i);
    error->push_back(temp_branch[1]);
  }
}


void get_data_by_wien_decimal(int wien, TTree *tree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events = (int) tree->GetEntries();
  int sign, wien_slug;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;

    value->push_back(sign*temp_branch[0]);
    runlet->push_back(run_number_decimal);
    error->push_back(temp_branch[1]);
//    error->push_back(temp_branch[2]); //width, NOT error
  }
}

//separates by IHWP
void get_data_by_wien_decimal_ihwp(int wien, int ihwp, TTree *tree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events = (int) tree->GetEntries();
  int sign, wien_slug, ihwp_setting;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress("ihwp_setting",&ihwp_setting);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;
    if (ihwp_setting != ihwp)
      continue;

    value->push_back(temp_branch[0]);
    runlet->push_back(run_number_decimal);
    error->push_back(temp_branch[1]);
//    error->push_back(temp_branch[2]); //width, NOT error
  }
}


void get_data_wien_decimal_friend(int wien, TTree *tree, TTree* friendtree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events       = (int) tree->GetEntries();
  int n_eventsfriend = (int) friendtree->GetEntries();

  if (n_events != n_eventsfriend) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  int sign, wien_slug;
  double temp_branch[4], run_number_decimal;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);

  double current[4], run_decimal_friend;
  friendtree->ResetBranchAddresses();
  friendtree->SetBranchAddress("yield_qwk_charge",&current);
  friendtree->SetBranchAddress("run_number_decimal",&run_decimal_friend);

  for( int i=0; i<n_events; i++ ) {
    tree->GetEntry(i);
    friendtree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;

    if (run_number_decimal != run_decimal_friend ) {
      printf("You have a problem - your trees are not processing the same events for run: %f \t%f\n", run_number_decimal, run_decimal_friend);
      continue;
    }

    if (run_number_decimal==13957.25 || current[0]<130.0) {
      printf("Disregarded run %f \tCurrent %f\n",run_number_decimal,current[0]);
      continue;
    }

    value->push_back(sign*temp_branch[0]);
    runlet->push_back(run_number_decimal);
    error->push_back(temp_branch[1]);
//    error->push_back(temp_branch[2]); //width, NOT error
  }
}


void get_data_by_wien2(int wien, TTree *tree, TString name1, TString name2, std::vector<double> *val1, std::vector<double>*err1, std::vector<double> *val2, std::vector<double> *err2) {
  int n_events = (int) tree->GetEntries();
  int sign, wien_slug;
  double data1_branch[4], data2_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name1,&data1_branch);
  tree->SetBranchAddress(name2,&data2_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( data1_branch[0]==-1e6 || data2_branch[0]==-1e6 || wien_slug != wien)
      continue;

    val1->push_back(sign*data1_branch[0]);
    err1->push_back(data1_branch[1]);
    val2->push_back(sign*data2_branch[0]);
    err2->push_back(data2_branch[1]);
  }
}

void get_yield_by_wien_decimal(int wien, TTree *tree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events = (int) tree->GetEntries();
  int wien_slug;
  double run_number_decimal;
  struct leaves {
    double val;
    double err;
    double rms;
    int     n;
  };
  leaves temp_data;

  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_data);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_data.val==-1e6 || temp_data.n<=5000 || wien_slug != wien)
      continue;

    value->push_back(temp_data.val);
    runlet->push_back(run_number_decimal);
    error->push_back(temp_data.rms);
  }
}


void get_yield_by_wien_decimal_friend(int wien, TTree *tree, TTree* friendtree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events = (int) tree->GetEntries();
  int n_events_friend = (int) friendtree->GetEntries();

  printf("file1: %i \t file2: %i\n",n_events,n_events_friend);
  if (n_events != n_events_friend) {
    std::cout <<"The two rootfiles have different number of events!\n";
    exit(0);
  }

  int wien_slug;
  double run_number_decimal;
  struct leaves {
    double val;
    double err;
    double rms;
    int     n;
  };
  leaves temp_data;

  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_data);

  double run_decimal_friend;
  leaves friend_data;
  friendtree->ResetBranchAddresses();
  friendtree->SetBranchAddress("yield_qwk_charge",&friend_data);
  friendtree->SetBranchAddress("run_number_decimal",&run_decimal_friend);

for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
//    if (count>10) continue;
    if (int (run_decimal_friend) == 0) continue;
    if (run_number_decimal != run_decimal_friend ) {
      printf("You have a problem - your trees are not processing the same events for run: %f \t%f\n", run_number_decimal, run_decimal_friend);
      continue;
    }

    if ( temp_data.val==-1e6 || temp_data.n<=10000 || wien_slug != wien)
      continue;
    if (friend_data.val <=130)
      continue;

    value->push_back(temp_data.val);
    runlet->push_back(run_number_decimal);
    error->push_back(temp_data.rms);
  }
}


void histo_by_wien(int wien, TTree *tree, TString name, TH1F *hist) {
  int n_events = (int) tree->GetEntries();
  int wien_slug, slug, sign;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;

      hist->Fill(sign*temp_branch[0]);
    }
}

void histo_by_wien(int wien, TTree *tree, TString name, TString weight, TH1F *hist) {
  int n_events = (int) tree->GetEntries();
  int wien_slug, sign;
  double w;
  double data_branch[4], weight_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name,&data_branch);
  tree->SetBranchAddress(weight,&weight_branch);

  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( data_branch[0]==-1e6 || weight_branch[0]==-1e6 || wien_slug != wien)
    { continue; }

    w=1/(pow(weight_branch[1],2));
//    w=1/(pow(data_branch[1],2));
    hist->Fill(sign*data_branch[0],w);
//    printf("Value: %f\tError: %f \tRMS: %f \t%i\n",data_branch[0],data_branch[1],data_branch[2],data_branch[3]);
    //printf("Value: %f\tError: %f \tWeight: %f\n",data_branch[0],data_branch[0],w);
    }
}


void histo_by_wien_weighted_friend(int wien, TTree *tree1, TString name, TTree *tree2, TString weight, TH1F *hist) {
  int n_events = (int) tree1->GetEntries();
  int wien_slug, sign;
  double w;
  double data_branch[4], weight_branch[4];
  tree1->ResetBranchAddresses();
  tree1->SetBranchAddress("wien_slug",&wien_slug);
  tree1->SetBranchAddress("sign_correction",&sign);
  tree1->SetBranchAddress(name,&data_branch);

  tree2->ResetBranchAddresses();
  tree2->SetBranchAddress(weight,&weight_branch);

  for( int i =0; i<n_events; i++ ) {
    tree1->GetEntry(i);
    //skip bad entries
    if ( data_branch[0]==-1e6 || weight_branch[0]==-1e6 || wien_slug != wien)
    { continue; }

    w=1/(pow(weight_branch[1],2));
    hist->Fill(sign*data_branch[0],w);
    //printf("Value: %f\tError: %f \tWeight: %f\n",data_branch[0],data_branch[0],w);
    }
}

void fill_pull_histo(TH1F* hist, float mean, std::vector<double> *vec, std::vector<double> *error) {
  if ( vec->size() != error->size() )
  {
    std::cout <<"In function fill_pull_histo your two vectors are of different sizes!" <<std::endl;
    exit(1);
  }

  std::vector<double>::iterator itvec;
  std::vector<double>::iterator iterr;

  for(itvec = vec->begin(), iterr = error->begin(); itvec != vec->end() && iterr != error->end(); ++itvec, ++iterr) {
    hist->Fill( (*itvec-mean) / *iterr );
  }
}

void histoRMS_by_wien(int wien, TTree *tree, TString name, TH1F *hist) {
  int n_events = (int) tree->GetEntries();
  int wien_slug, slug, sign;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || wien_slug != wien)
      continue;
    //hist->Fill(sign*temp_branch[1]);
    hist->Fill(temp_branch[2]);
  }
}

void print_good_runlets(TTree *tree, TString name, TString outfile) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int sign,wien;
  double run_number_decimal,temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien);
  tree->SetBranchAddress("sign_correction",&sign);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);

  ofstream output;
  output.open(outfile);
  output <<"Wien \tRun_number_decimal \tValue\t Error \tRMS \tNumEntries" <<std::endl;

  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if ( temp_branch[0] == -1e6 )
      continue;
    output <<wien <<"\t" <<std::fixed
      <<std::setprecision(2) <<run_number_decimal <<"\t" <<"\t"
      <<std::setprecision(5) <<sign*temp_branch[0]  <<"\t" <<"\t"
      <<std::setprecision(5) <<temp_branch[1]  <<"\t" <<"\t"
      <<std::setprecision(5) <<temp_branch[2]  <<"\t" <<"\t"
      <<std::setprecision(0) <<temp_branch[3] <<std::endl;

  }
  output.close();
}

void find_bad_runlets(TTree *tree, TString name) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int  wien;
  double  run_number_decimal;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if ( temp_branch[0] == -1e6 || temp_branch[2] >5000 ) {
      printf("Wien: %i \tRunlet_id: %f\tValue: %f\t Error: %f\t RMS: %f\t NumEntries: %f\n",wien,run_number_decimal,temp_branch[0],temp_branch[1],temp_branch[2],temp_branch[3]);
    }
  }
}


void find_conditional_runlets(TTree *tree, TString name, float limit) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int  wien;
  int count=0;
  double  run_number_decimal;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if ( temp_branch[0] == -1e6 )
      continue;
    if ( temp_branch[0] < limit ) {
      printf("Wien: %i \tRunlet_id: %f\tValue: %f\t Error: %f\t RMS: %f\t NumEntries: %f\n",wien,run_number_decimal,temp_branch[0],temp_branch[1],temp_branch[2],temp_branch[3]);
      count++;
    }
  }
  printf("Total number of runs satisfying condition: \t%i\n",count);
}


void find_bad_runlets(TTree *tree, TString name, TString outfile) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int wien;
  double run_number_decimal,temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("wien_slug",&wien);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress(name,&temp_branch);

  ofstream output;
  output.open(outfile);
  output <<"Wien \tRunlet_id \tValue\t Error \tRMS \tNumEntries" <<std::endl;

  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if ( temp_branch[0] == -1e6 || temp_branch[2] >=1e6 ) {
      output <<wien <<"\t"
        <<run_number_decimal <<"\t" <<"\t"
        <<temp_branch[0]  <<"\t" <<"\t"
        <<temp_branch[1]  <<"\t" <<"\t"
        <<temp_branch[2]  <<"\t" <<"\t"
        <<temp_branch[3] <<std::endl;
    }
  }
  output.close();
}

void find_bad_qtor_runlets(TTree *tree) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int dummy=0;
  int run,run_number_decimal;
  float qtor;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("run",&run);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress("qtor_current",&qtor);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if (dummy==run) continue;
    if ( qtor<8000 ) {
      printf("Run: %i \tRunlet_id: %i \tqtor: %f\n",run,run_number_decimal,qtor);
      dummy=run;
    }
  }
}


void check_size(std::vector<double> *runlet,int wien) {
  if (runlet->size() != 0)
    printf("Wien %i vector has data.\n",wien);
  else {
    printf("Wien %i vector doesn't have data! Exiting script.\n",wien);
    exit(0);
  }
}

void fillHistArray(TTree *tree, TString dataname, int wien, TH2F** histArray, int size) {
  int n_events = (int) tree->GetEntries();
  int run_number_decimal, wien_slug;
  double temp_branch[size][4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress("wien_slug",&wien_slug);
  tree->SetBranchAddress(dataname,&temp_branch);

  //set the branch address for detectors 1-8
  //for detecter_all need to set manually
  for (int i=0; i<size-1; i++) {
    tree->SetBranchAddress(Form("asym_md%ipmtavg",i+1),&temp_branch[i]);
  }
  tree->SetBranchAddress("asym_mdallpmtavg",&temp_branch[size]);

  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    if (temp_branch[i][0]==-1e6 || wien_slug != wien) continue;

    //time to fill histArray
    for (int j=0; j<size-1; j++) {
      histArray[j]->Fill(run_number_decimal,temp_branch[j][0]);
    }
      histArray[size]->Fill(run_number_decimal,temp_branch[size][0]);
  }
}







