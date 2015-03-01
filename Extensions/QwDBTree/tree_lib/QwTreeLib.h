/*
 * wshutter.cpp
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 * Small program to correlate two variables, one of which is charge normalized. To compile:
 *
 * g++ `root-config --glibs --cflags` -ggdb -O0 -Wall -Wextra -o correlator correlator.cpp
 *
 * To plot var1 vs var2:
 *
 * ./correlator <var1> <var2> <rootfile>
 *
 * Example: 
 * ./correlator asym_qwk_md1_qwk_md5 asym_qwk_pmtltg offoff_tree.root
 *
 */

#ifndef _QWTREELIB_H_
#define _QWTREELIB_H_

void get_data_from_tree(TTree*, TString, std::vector<double>*);

void get_data_from_tree(TTree*, TString, std::vector<double>*, std::vector<double>*);

void get_data_from_tree(TTree*, TString, std::vector<double>*, std::vector<double>*, std::vector<double>*);

void get_data_from_tree(TTree*, TString, std::vector<double>*, std::vector<double>*, std::vector<double>*, std::vector<double>*);

void get_single_value_from_tree(TTree*, TString, std::vector<double>*);

void get_single_value_from_tree(TTree*, TString, std::vector<int>*);

void plot_histo_branch_value(TTree*, TString); 

void plot_histo_single_value(TTree*, TString);

TGraph* plot_vectors(std::vector<double>, std::vector<double>);

TGraph* plot_vectors(std::vector<int>, std::vector<double>, TString, TString);

#endif
