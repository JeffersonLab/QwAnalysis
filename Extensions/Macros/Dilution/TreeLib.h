/*
 * JoshTreeLib.cpp
 * Author: Josh Magee <magee@jlab.org>
 * Date:   2013-07-16
 *
 * Small program to walk QwDB rootfile trees for plotting purposes
 *
 */

#ifndef _TreeLib_H_
#define _TreeLib_H_
#include <TString.h>
using namespace std;

class TTree;


void get_wien_from_tree(int, TTree*, TString, std::vector<double> *);

void get_data_from_tree(TTree*, TString, std::vector<double> *);
void get_data_from_tree(TTree*, TString, std::vector<double> *, std::vector<int>* );
void get_data_from_tree(TTree*, TString, std::vector<double>*, std::vector<double>*);
void get_data_from_tree_runlet(TTree*, TString, std::vector<double> *,std::vector<double>*,std::vector<double>*);
void get_data_from_tree_runlet_decimal(TTree*, TString, std::vector<double> *,std::vector<double>*,std::vector<double>*);

void get_single_value_from_tree(TTree *, TString, std::vector<double> *);
void get_single_value_from_tree(TTree *, TString, std::vector<int> *);

void erase_null_entries(std::vector<int>*);
void erase_null_entries(std::vector<double>*, std::vector<double>*);
void erase_null_entries(std::vector<double>*, std::vector<double>*, std::vector<int>*);
void erase_null_entries(std::vector<double>*, std::vector<double>*, std::vector<double>*);

void printInfo(TTree *, TString);
void printInfo(TTree *, TString, TString);

void get_data_by_wien(int,TTree *,TString, std::vector<double>*,std::vector<double>*,std::vector<double>*);

void histo_by_wien(int, TTree*, TString, TH1F*);
void histoRMS_by_wien(int, TTree*, TString, TH1F*);

void fillHistArray(TTree*, TString string, int, TH2F**, int);

void fill_pull_histo(TH1F*,std::vector<double>*, float, float);

void find_bad_runlets(TTree *);
void find_bad_runlets(TTree *, TString);

void check_size(std::vector<double>*,int);

#endif

