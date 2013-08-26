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


void get_data_from_tree( TTree* tree, TString name, std::vector<double> *value, std::vector<int> *runlet ) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int runlet_id;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("runlet_id", &runlet_id);
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
  int n_events;
  n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 || temp_branch[1]>10)
      continue;
    value->push_back(temp_branch[1]);
//    runlet->push_back(i+1);
//    histo->Fill(temp_branch[1],i+1);
  }
}

void get_data_from_tree( TTree* tree, TString name, std::vector<double> *value, std::vector<double> *error) {
//  std::cout <<"You are inside function get_data_from_tree...." <<std::endl;
  int n_events;
  n_events = (int) tree->GetEntries();
  double temp_branch[4];
  cout << name.Data() <<endl;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    value->push_back(temp_branch[0]);
    error->push_back(temp_branch[1]);
//    printf("value: %f \terror: %f\n",temp_branch[0],temp_branch[1]);
  }
//  std::cout <<"You are leaving function get_data_from_tree...." <<std::endl;
}


void get_wien_from_tree(int wien, TTree* tree, TString name, std::vector<double> *value ) {
  int n_events, slug, sign;
  n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug", &slug);
  tree->SetBranchAddress("ihwp_setting",&sign);
  tree->SetBranchAddress(name, &temp_branch);
  for(int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    //skip bad entries
    if ( temp_branch[0]==-1e6 )
      continue;
    if (wien==6 && slug>1032) continue;
    if (wien==8 && (slug<1033 || slug>1043)) continue;
    if (wien==9 && (slug<1044 || slug>1058)) continue;
    if (wien==10 && slug<1059) continue;

    if (sign == 0) {
      value->push_back(temp_branch[0]);
    } else {
      value->push_back(-1*temp_branch[0]);
    }
//    runlet->push_back(i+1);
//    histo->Fill(temp_branch[1],i+1);
  }
}

void get_data_with_cuts(TTree* tree, TString name, int low, int high,
    std::vector<double> *valuelow, std::vector<double>* errlow,
    std::vector<double> *valuemid, std::vector<double>* errmid,
    std::vector<double> *valuehigh, std::vector<double>* errhigh) {

  printf("WARNING!!! You are using code that DOES NOT WORK!!!\n");

  printf("you are splicing %s at %i and %i\n",name.Data(),low,high);

  int n_events;
  n_events = (int) tree->GetEntries();
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);

    if (TMath::Abs(temp_branch[0])<=low) {
      valuelow->push_back(temp_branch[0]);
      errlow->push_back(temp_branch[1]);
    } else if (TMath::Abs(temp_branch[0])>low && TMath::Abs(temp_branch[0])<=high) {
      valuemid->push_back(temp_branch[0]);
      errmid->push_back(temp_branch[1]);
    } else {
      valuehigh->push_back(temp_branch[0]);
      errhigh->push_back(temp_branch[1]);
    }
  }
  exit(1);
}

void get_data_with_cuts_replace_runlets(TTree* tree, TString name, int low, int high,
    std::vector<double> *valuelow,  std::vector<double> *errlow,  std::vector<int>*runLow,
    std::vector<double> *valuehigh, std::vector<double> *errhigh, std::vector<int>*runHigh)
{

  printf("you are splicing %s at %i and %i\n",name.Data(),low,high);

  int n_events;
  n_events = (int) tree->GetEntries();

  int slug;
  int runlet_id;
  float dummy = 0.0;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("runlet_id",&runlet_id);
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress(name,&temp_branch);

  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);

    //skip bad entries
    if ( temp_branch[0]==-1e6 || temp_branch[1]>10)
      continue;

    if ( (runlet_id-dummy)>100 && dummy!=0)
      printf("Delta >100 at count: \t%i \tslug: \t%i \trunlet_id: \t%i\n",i,slug,runlet_id);

    if (TMath::Abs(temp_branch[0])<=low) {
      valuelow->push_back(temp_branch[0]);
      errlow->push_back(temp_branch[1]);
      runLow->push_back(i+1);
      valuehigh->push_back(temp_branch[0]);
      errhigh->push_back(temp_branch[1]);
      runHigh->push_back(i+1);
    } else if (TMath::Abs(temp_branch[0])>low && TMath::Abs(temp_branch[0])<=high) {
      valuehigh->push_back(temp_branch[0]);
      errhigh->push_back(temp_branch[1]);
      runHigh->push_back(i+1);
    } else {
      continue;
    }
    dummy = runlet_id;
  }
}


void get_data_with_cuts_replace_runlets(TTree* tree, TString name, int low, int high,
    std::vector<double> *valuelow,  std::vector<double> *errlow,  std::vector<int>*runLow,
    std::vector<double> *valuemid,  std::vector<double> *errmid,  std::vector<int>*runMid,
    std::vector<double> *valuehigh, std::vector<double> *errhigh, std::vector<int>*runHigh)
{

  printf("you are splicing %s at %i and %i\n",name.Data(),low,high);

  int n_events;
  n_events = (int) tree->GetEntries();

  int slug;
  int runlet_id;
  float dummy = 0.0;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("runlet_id",&runlet_id);
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress(name,&temp_branch);

  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);

    //skip bad entries
    if ( temp_branch[0]==-1e6  || temp_branch[3]<5000e3)
      continue;

    if ( (runlet_id-dummy)>100 && dummy!=0)
      printf("Delta >100 at count: \t%i \tslug: \t%i \trunlet_id: \t%i\n",i,slug,runlet_id);

    if (TMath::Abs(temp_branch[0])<=low) {
      valuelow->push_back(temp_branch[0]);
      errlow->push_back(temp_branch[1]);
      runLow->push_back(i+1);
    } else if (TMath::Abs(temp_branch[0])>low && TMath::Abs(temp_branch[0])<=high) {
      valuemid->push_back(temp_branch[0]);
      errmid->push_back(temp_branch[1]);
      runMid->push_back(i+1);
    } else {
      valuehigh->push_back(temp_branch[0]);
      errhigh->push_back(temp_branch[1]);
      runHigh->push_back(i+1);
    }
    dummy = runlet_id;
  }
}

void get_data_with_cuts_keep_runlets(TTree* tree, TString name, int low, int high,
    std::vector<double> *valuelow,  std::vector<double> *errlow,  std::vector<int>*runLow,
    std::vector<double> *valuemid,  std::vector<double> *errmid,  std::vector<int>*runMid,
    std::vector<double> *valuehigh, std::vector<double> *errhigh, std::vector<int>*runHigh)
 {

  printf("you are splicing %s at %i and %i\n",name.Data(),low,high);

  int n_events;
  n_events = (int) tree->GetEntries();
  int    runlet_branch[4];
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("runlet_id",&runlet_branch);
  tree->SetBranchAddress(name,&temp_branch);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);

    //skip bad entries
    if ( temp_branch[0]==-1e6 )
      continue;

    if (TMath::Abs(temp_branch[0])<=low) {
      valuelow->push_back(temp_branch[0]);
      errlow->push_back(temp_branch[1]);
      runLow->push_back(runlet_branch[0]);
    } else if (TMath::Abs(temp_branch[0])>low && TMath::Abs(temp_branch[0])<=high) {
      valuemid->push_back(temp_branch[0]);
      errmid->push_back(temp_branch[1]);
      runMid->push_back(runlet_branch[0]);
    } else {
      valuehigh->push_back(temp_branch[0]);
      errhigh->push_back(temp_branch[1]);
      runHigh->push_back(runlet_branch[0]);
    }
  }
}

void erase_null_entries(std::vector<int> *value) {
//  std::cout <<"You are in erase_null_entries (int)!\n";
  for (int i=0; i< int( value->size() ); i++) {
    if ( value->at(i)==-1e6 ) {
      printf("Erasing value: %i\n",value->at(i));
      value->erase(value->begin()+i);
      i--;
    }
  }
}

void erase_null_entries(std::vector<double> *value, std::vector<double> *error) {
//  std::cout <<"You are in erase_null_entries!\n";
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
    } else {
//      printf("Keeping value: %f \t error: %f\n",value->at(i),error->at(i));
    }
  }
}

void erase_null_entries(std::vector<double> *value, std::vector<double> *error, std::vector<int> *runlet) {
//  std::cout <<"You are in erase_null_entries! (three!)\n";
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
      //printf("Erasing value: %f \t error: %f \trunlet: %i\n",value->at(i),error->at(i),runlet->at(i));
    }
  }
}


void erase_null_entries(std::vector<double> *value, std::vector<double> *error, std::vector<double> *runlet) {
//  std::cout <<"You are in erase_null_entries! (double!)\n";
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
//  std::cout <<"You are inside function get_single_value_from_tree (int)...." <<std::endl;
  int n_events;
  n_events = (int) tree->GetEntries();
  int temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress(name, &temp_branch);
  for (int i=0; i<n_events; i++) {
    tree->GetEntry(i);
    value->push_back(temp_branch[0]);
  }
}

void get_single_value_from_tree(TTree *tree, TString name, std::vector<double> *value) {
//  std::cout <<"You are inside function get_single_value_from_tree (double)...." <<std::endl;
  int n_events;
  n_events = (int) tree->GetEntries();
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

//  int runlet_id;
//  tree->SetBranchAddress("runlet_id", &runlet_id);

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
    //output <<Form("%i \t%i \t%f \t%f \t%f \t%f\n",slug,runlet_id,value,error,rms,n);
    output <<Form("%i \t%f \t%f \t%f \t%f\n",slug,value,error,rms,n);
  }
  output.close();
  printf("Data written to file %s.\n",outfile.Data());
}


void get_data_by_wien(int wien_number, TTree *tree, TString name, std::vector<double> *runlet, std::vector<double> *value, std::vector<double>*error) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int wien=wien_number;
  int slug;
  int runlet_id;
  int sign;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("runlet_id",&runlet_id);
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress("ihwp_setting",&sign);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
//    if ( wien_number != wien || temp_branch[0]==-1e6)
    if ( temp_branch[0]==-1e6)
      continue;
//need to work magic for sign variable
//    printf("The sign for Wien %d is %d\n",wien,sign);
    if (wien==6 && slug>1032) continue;
    if (wien==8 && (slug<1033 || slug>1043)) continue;
    if (wien==9 && (slug<1044 || slug>1058)) continue;
    if (wien==10 && slug<1059) continue;

    if (sign == 0) {
      value->push_back(temp_branch[0]);
    } else {
      value->push_back(-1*temp_branch[0]);
    }
    runlet->push_back(i);
    error->push_back(temp_branch[1]);
  }
}


void histo_by_wien(int wien_number, TTree *tree, TString name, TH1F *hist) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int wien=wien_number;
  int slug;
  int sign;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress("ihwp_setting",&sign);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
//    if ( wien_number != wien || temp_branch[0]==-1e6)
    if ( temp_branch[0]==-1e6)
      continue;
//need to work magic for sign variable
//    printf("The sign for Wien %d is %d\n",wien,sign);
    if (wien==6 && slug>1032) continue;
    if (wien==8 && (slug<1033 || slug>1043)) continue;
    if (wien==9 && (slug<1044 || slug>1058)) continue;
    if (wien==10 && slug<1059) continue;

    if (sign == 0) {
      hist->Fill(temp_branch[0]);
    } else {
      hist->Fill(-1*temp_branch[0]);
    }
  }
}

void fill_pull_histo(TH1F* hist, std::vector<double> *vec, float mean, float error) {
  for(std::vector<double>::iterator it = vec->begin(); it != vec->end(); ++it) {
    hist->Fill( (*it-mean)/error );
  }
}

void histoRMS_by_wien(int wien_number, TTree *tree, TString name, TH1F *hist) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int wien=wien_number;
  int slug;
  int sign;
  double temp_branch[4];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("slug",&slug);
  tree->SetBranchAddress("ihwp_setting",&sign);
  tree->SetBranchAddress(name,&temp_branch);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    //skip bad entries
//    if ( wien_number != wien || temp_branch[0]==-1e6)
    if ( temp_branch[0]==-1e6)
      continue;
//need to work magic for sign variable
//    printf("The sign for Wien %d is %d\n",wien,sign);
    if (wien==6 && slug>1032) continue;
    if (wien==8 && (slug<1033 || slug>1043)) continue;
    if (wien==9 && (slug<1044 || slug>1058)) continue;
    if (wien==10 && slug<1059) continue;

    hist->Fill(temp_branch[1]);
  }
}

void find_bad_runlets(TTree *tree) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int dummy=0;
  int run,runlet_id;
  float qtor;
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("run",&run);
  tree->SetBranchAddress("runlet_id",&runlet_id);
  tree->SetBranchAddress("qtor_current",&qtor);
  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if (dummy==run) continue;
    if ( qtor<8000 ) {
      printf("Run: %i \tRunlet_id: %i \tqtor: %f\n",run,runlet_id,qtor);
      dummy=run;
    }
  }
}


void find_bad_runlets(TTree *tree, TString outfile) {
  int n_events;
  n_events = (int) tree->GetEntries();
  int dummy=0;
  int run,runlet_id;
  double qtor_current;
  double temp_branch[7];
  tree->ResetBranchAddresses();
  tree->SetBranchAddress("run",&run);
  tree->SetBranchAddress("runlet_id",&runlet_id);
  tree->SetBranchAddress("qtor_current",&qtor_current);
  tree->SetBranchAddress("runlet_id",&temp_branch);

  ofstream output;
  output.open(outfile);
  output <<"Run \tRunlet_id \tqtor" <<std::endl;

  for( int i =0; i<n_events; i++ ) {
    tree->GetEntry(i);
    if (dummy==run) continue;
    if ( qtor_current<8000 ) {
      output <<Form("Run: %i \tRunlet_id: %i \tqtor: %f\n",run,runlet_id,qtor_current);
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









