/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Main program , generates  QW event with build in flas ASY and correlations 
 *********************************************************************/

#include <cstdio>  // C++ standard since 1999
#include <TObjArray.h>
#include <TFile.h>

#include "JbToy6DGen.h"

// use only double

int main() {
  printf("myGenerator:  START...\n");

  TString mTreePath="./";
  TString treeName=mTreePath+"/Qweak_1234.000.root";
  TFile*  mHfile=new TFile(treeName,"RECREATE"," histograms & trees Qweak M-C events");
  assert(mHfile->IsOpen());
  printf(" Setup output tree & histo to '%s' ,\n",treeName.Data());
  
  // order of  beam params: x,nx,y,ny,E,Q
  double inpSigBpm[mxP]={0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
  memset(inpSigBpm,0,sizeof(inpSigBpm)); // turn off BPM spread

  // mean values of 6 iv
  double inpMeanP[mxP]={-0.22, -0.19, 0.10,  -0.21, 1260.1, 1.0};
  double inpMeanN[mxP]={-0.26, -0.11, 0.10,  -0.15, 1260.2, 1.0};

  // mean values for 4 dv
  double inpMeanY[mxY]={1.1, 1.2, 1.3, 1.4};
  double inpSigY[mxY]={0.05, 0.06, 0.07, 0.08};

  // correlation for 4 dv
  double inpAlphas0[mxP]={-0.23, 0.18, -0.35, -0.41, 0.27, -0.22};
  double inpAlphas1[mxP]={ 0.11, 0.28, 0.15,  -0.32, -0.17, 0.27};
  double inpAlphas2[mxP]={ 0.05 , 0.05, 0.05,  0.05,  0.05,  0.05};
  double inpAlphas3[mxP]={ -0.41, 0.24, -0.36, 0.28,  0.27, 0.41};

  JbToy6DGen gen;
  //.... iv ....
  gen.setMeanP(inpMeanP,kheliP); //  offsets for generator +heli
  gen.setMeanP(inpMeanN,kheliN); //  offsets for generator -heli
  gen.setBeamSpread(0.2, 0.1, 30.,0);// X-nX plane
  gen.setBeamSpread(0.23, 0.08, -20.,1);//Y-nY plane
  gen.setBeamSpreadEQ(0.14, 0.17);//beam energy and charge spread, no correlation
  gen.setBpmSpread(inpSigBpm); 
  //..... dv .....
  gen.setMeanY(inpMeanY); 
  gen.setSpreadY(inpSigY); 
  //assume correlations: Ymeas=Ytrue+alpha_k*(P_k- avrP_k) 
  gen.setAlphas(inpAlphas0,0); // Generator
  gen.setAlphas(inpAlphas1,1); // Generator
  gen.setAlphas(inpAlphas2,2); // Generator
  gen.setAlphas(inpAlphas3,3); // Generator
  
  gen.init();
  gen.print();

  int totEve=60000;
  printf("main  START for nEve=%d  ...\n",totEve);
  for(int ieve=0;ieve<totEve;ieve++) 
    gen.throwEvent(ieve);
  
  gen.finish();
  
  mHfile->Write();
  mHfile->Close();
  printf("\n Histo saved -->%s<\n",treeName.Data());
  
  return 0;
}
