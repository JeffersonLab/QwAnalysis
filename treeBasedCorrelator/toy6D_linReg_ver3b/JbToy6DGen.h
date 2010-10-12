/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Generator of  synthetic QW event with build in fals asymmetries
 *********************************************************************/

#include <assert.h>
class TRandom3 ;
class TH1;
class TTree;
#include "ToyQwChannel.h"
enum {mxP=6, mxY=4,mxPY=mxP+mxY};
enum {mXY=2};

class JbToy6DGen {

 private:
  //.... iv .....
  double par_P[mxP][mxHeli]; // Mean offsets of beam position for generator
  double par_Pavr[mxP]; // Mean offsets of beam position
  double par_sig1[mXY],par_sig2[mXY], par_phi[mXY]; // beam spread and correlation angle
  double par_sigE,par_sigQ; // beam energy and charge spread;
  double par_sigBpm[mxP]; // spread of BPM signals

  //...... dv .....
  double par_Ymean[mxY]; // mean of yields
  double par_Ysig[mxY]; // spread of yields
  // correlations
  double par_Alpha[mxY][mxP]; // Ymeas=Ytrue+alpha_k*(P_k- avrP_k) for generator

  TRandom3 *rnd;
  double  mCosA[mXY],mSinA[mXY];
  TTree*  mTree;

  // sum of yields
  double mYield[mxY][mxHeli];
  int mTotEve;

  // histograms
  enum {mxHA=32}; TH1 * hA[mxHA];
  void initHistos();

 public:
  JbToy6DGen(); 

  void setMeanP( double A[],int heli) {   assert(heli==kheliP || heli==kheliN);
    for(int i=0;i<mxP;i++)   par_P[i][heli]=A[i]; }
  void setBpmSpread( double *A) { for(int i=0;i<mxP;i++)  par_sigBpm[i]=A[i]; } 
  void setBeamSpread( double a,  double b,  double c, int idir ){
    par_sig1[idir]=a; par_sig2[idir]=b; par_phi[idir]=c;}
  void setBeamSpreadEQ( double a,  double b ){
    par_sigE=a; par_sigQ=b;}
  void setMeanY( double *A) { for(int i=0;i<mxY;i++)   par_Ymean[i]=A[i]; } 
  void setSpreadY( double *A) { for(int i=0;i<mxY;i++)   par_Ysig[i]=A[i]; } 
  void setAlphas( double *A, int iy) { assert(iy>=0 && iy<mxY);
    for(int i=0;i<mxP;i++)   par_Alpha[iy][i]=A[i]; }
  void setSeed(int seed);
  void print();
  void init();
  void finish();
  void throwEvent(int ieve);
  
};

