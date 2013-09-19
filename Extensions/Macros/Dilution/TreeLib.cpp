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
  int slug;
  int n_events;
  double value,error,rms,n;
  double temp_branch[4];

  n_events = (int) tree->GetEntries();
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress(name, &temp_branch);


  ofstream output;
  output.open(outfile);
  output <<"Slug \tvalue \terror \trms \tn" <<std::endl;
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    if (temp_branch[0]==-1e6)
      continue;
    value =temp_branch[0];
    error =temp_branch[1];
    rms   =temp_branch[2];
    n     =temp_branch[3];
    output <<Form("%i \t%f \t%f \t%f \t%f\n",slug,value,error,rms,n);
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
    runlet->push_back(i);
    error->push_back(temp_branch[1]);
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

void fill_pull_histo(TH1F* hist, std::vector<double> *vec, float mean, float error) {
  for(std::vector<double>::iterator it = vec->begin(); it != vec->end(); ++it) {
    hist->Fill( (*it-mean)/error );
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
    hist->Fill(sign*temp_branch[1]);
  }
}

void find_bad_runlets(TTree *tree) {
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


void find_bad_runlets(TTree *tree, TString outfile) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int dummy=0;
  int run,run_number_decimal;
  double qtor_current;
  double temp_branch[7];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("run",&run);
  tree->SetBranchAddress("run_number_decimal",&run_number_decimal);
  tree->SetBranchAddress("qtor_current",&qtor_current);
  tree->SetBranchAddress("run_number_decimal",&temp_branch);

  ofstream output;
  output.open(outfile);
  output <<"Run \tRunlet_id \tqtor" <<std::endl;

  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if (dummy==run) continue;
    if ( qtor_current<8000 ) {
      output <<Form("Run: %i \tRunlet_id: %i \tqtor: %f\n",run,run_number_decimal,qtor_current);
      dummy=run;
      for(int j=0; j<7; j++) {
        std::cout <<temp_branch[j] <<std::endl;
      }
    }
  }
  output.close();
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







