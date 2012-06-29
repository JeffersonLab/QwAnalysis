#include <iostream>

#include "TChain.h"
#include "TString.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TSystem.h"

#include "covariance.C"

void correction (Int_t run, Int_t max = -1) {

TString treename = "Mps_Tree";

// Independent variables
std::vector<TString> iv;
iv.push_back("qwk_charge");
iv.push_back("qwk_energy");
iv.push_back("qwk_targetX");
iv.push_back("qwk_targetXSlope");
iv.push_back("qwk_targetY");
iv.push_back("qwk_targetYSlope");

// Dependent variables
std::vector<TString> dv;
dv.push_back("qwk_md1neg");
dv.push_back("qwk_md1pos");
/*
dv.push_back("qwk_md2neg");
dv.push_back("qwk_md2pos");
dv.push_back("qwk_md3neg");
dv.push_back("qwk_md3pos");
dv.push_back("qwk_md4neg");
dv.push_back("qwk_md4pos");
dv.push_back("qwk_md5neg");
dv.push_back("qwk_md5pos");
dv.push_back("qwk_md6neg");
dv.push_back("qwk_md6pos");
dv.push_back("qwk_md7neg");
dv.push_back("qwk_md7pos");
dv.push_back("qwk_md8neg");
dv.push_back("qwk_md8pos");
*/


// Concatenate all channels
std::vector<TString> channels;
channels.insert(channels.end(),iv.begin(),iv.end());
channels.insert(channels.end(),dv.begin(),dv.end());


// Open tree for run number
TChain tree(treename);
tree.Add(TString(gSystem->Getenv("QW_ROOTFILES")) + Form("/QwPass?_%d.*.root",run));


// Calculate full covariance matrix
TMatrixDSym variance(channels.size());
for (size_t i = 0; i < channels.size(); i++) {
  for(size_t j = i; j < channels.size(); j++) {
    variance(i,j) = variance(j,i) = covariance(tree, channels[i], channels[j], max);
    std::cout << "cov[" << channels[i] << "," << channels[j] << "] = " << variance(i,j) << std::endl;
  }
}


// Calculate full correlation matrix
TMatrixDSym matrix_r(channels.size());
for (size_t k = 0; k < channels.size(); k++) {
  for (size_t l = k; l < channels.size(); l++) {
    matrix_r(k,l) = (variance(k,l)) / (pow(variance(k,k) * variance(l,l), .5));
  }
}
matrix_r.Print();
matrix_r.Draw("colz,boxz");


// Calculate internal correlation matrix alpha and external correlation vectors beta
TMatrixD matrix_beta(iv.size(),dv.size());
TMatrixDSym matrix_alpha(iv.size());
for (size_t k = 0; k < iv.size(); k++) {
  for(size_t Y = 0; Y < dv.size(); Y++) {
     matrix_beta(k,Y) = matrix_r(k,(iv.size()+Y));
  }
  for(size_t l = 0; l < iv.size(); l++) {
    matrix_alpha(k,l) = matrix_r(k,l);
  }
}
// Invert internal correlation matrix alpha
TMatrixD inverse_alpha(TMatrixD::kInverted, matrix_alpha);

// Calculat principal components of alpha
TMatrixDSymEigen eigen(matrix_r);
TMatrixD matrix_V = eigen.GetEigenVectors();
TMatrixD matrix_lambda = eigen.GetEigenValues();
matrix_lambda.Print();

// Calculate decorrelated vectors D
TMatrixD matrix_D(iv.size(),dv.size());
matrix_D = inverse_alpha * matrix_beta;

// Calculate sensitivities vectors C
TMatrixD matrix_C(iv.size(),dv.size());
for (size_t k = 0; k < iv.size(); k++) {
  for (size_t Y = 0; Y < dv.size(); Y++) {
     matrix_C(k,Y) = matrix_D(k,Y) * pow(variance((iv.size()+Y),(iv.size()+Y)) / variance(k,k), .5);
  }
}
matrix_C.Print();

}
