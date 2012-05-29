#include <iostream>

#include "TChain.h"

Double_t covariance(TChain& Mps_Tree, const TString& channel1, const TString& channel2, Int_t max=-1) {

struct vqwk {
 Double_t hw_sum;
 Double_t block0;
 Double_t block1;
 Double_t block2;
 Double_t block3;
 Double_t num_samples;
 Double_t Device_Error_Code;
 Double_t hw_sum_raw;
 Double_t block0_raw;
 Double_t block1_raw;
 Double_t block2_raw;
 Double_t block3_raw;
 Double_t sequence_number;
};

vqwk qwk_channel1;
vqwk qwk_channel2;

Mps_Tree.ResetBranchAddresses();
Mps_Tree.SetBranchAddress(channel1,&qwk_channel1);
if (channel1 != channel2) {
  Mps_Tree.SetBranchAddress(channel2,&qwk_channel2);
}

Double_t mean1 = 0.0;
Double_t mean2 = 0.0;
Double_t C = 0.0;
Double_t n = 0.0;

for (Int_t i = 0; i < Mps_Tree.GetEntries() && (i < max || max < 0); i++) {

  Mps_Tree.GetEntry(i);
  if (channel1 == channel2) qwk_channel2 = qwk_channel1;

  if (qwk_channel1.Device_Error_Code == 0 && qwk_channel2.Device_Error_Code == 0) {

    n++;

    C += (n - 1.0) / n * (qwk_channel1.hw_sum - mean1) * (qwk_channel2.hw_sum - mean2);

    mean1 += (qwk_channel1.hw_sum - mean1) / n;
    mean2 += (qwk_channel2.hw_sum - mean2) / n;
  }
}

return C;
}
