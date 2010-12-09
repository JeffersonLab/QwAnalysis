#ifndef JBLEAFTRANSFORM_H
#define JBLEAFTRANSFORM_H
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * unpacks & transforms variabloes from input tree
 *********************************************************************/

#include <assert.h>
#include <TString.h> 

class TH1;
class TChain;
class JbLeafTransform {
 public:
  std::vector < TString > ivName; // list of independent variables
  std::vector < TString > dvName; // list of dependent variables
  TString inpTree, regVarName, afixName;
  TString cutName, cutFormula;

  int ndv(){return (int)dvName.size();}
  int niv(){return (int)ivName.size();}
  void readConfig(const char * configFName);
  void presetMyStat(double x1,double x2, double thr, double x3);
  TString humanizeLeafName(TString longName);
  TString setLeafName2Yield(TString longName);
  
 private:
  TString myName;
  enum{ mxAux=3+4+1};// auxiliary variables: pattern, bcm1, bcm2
  // added 4 bcm asym, + double differences 
  // + asym_qwk_bpm3h09b_EffectiveCharge

  // pointers to leaf variables
  Double_t **pLeafDV, **pLeafIV, *pLeafAux[mxAux], **pLeafError; 
  Double_t **pYieldDV, **pYieldIV;
  int nLeafError;
  Double_t * getOneLeaf(TChain *chain,TString name,TString sub=""); 
  // histograms
  enum {mxHA=32}; TH1 * hA[mxHA];
  TH1 ** hydv, ** hyiv;
  void initHistos();
  
  int harvestNames(FILE *fp);

 public:
  JbLeafTransform(const char *core); 
  // add destructor for 'readConfig new'
  void print();
  void findInputLeafs(TChain *chain);
  bool unpackEvent();
 
  /* arrays w/ final 15 dv & iv variables
     1-bar tube combos:    MD 1-8 (all bars)
     2-bar combos:             MD H (3+7), V (1+5), D1 (2+6), D2(4+8)
     4-bar combos:             MD C (1+3+5+7), MD X (2+4+6+8)
     all:	                               MD ALL (1+2+3+4+5+6+7+8)

     named: md1,...,8, h,v,d1,d2,c,x,all
  */


  Double_t *Pvec, *Yvec;


  //old
  void init();
  void finish();
  
};


#endif
