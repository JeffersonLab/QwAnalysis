#ifndef JBLEAFALIAS_H
#define JBLEAFALIAS_H
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 *  aliasing capability
 *********************************************************************/
#include <TString.h> 

struct  QwkChanJan{ 
  Double_t  hw_sum, block0,block1,block2,block3;
  Double_t  num_samples, Device_Error_Code;
};

class QwkRegBlueAlias{

 private:
  enum typeOper {kOperBad=-1,kOperMinus=0, kOperTimes=1, kOperPlus=2};
  TString rawText;
  typeOper oper;
  int kout,kin1,kin2;
 

 public:

  QwkRegBlueAlias(TString full);
  TString outName, inpName1, inpName2;

  void print();
  void compute_hw_sum(QwkChanJan  *qwkChan);
  void mappChannels( int a,int b,int c){
    kout=a; kin1=b; kin2=c;
  }

};
#endif
