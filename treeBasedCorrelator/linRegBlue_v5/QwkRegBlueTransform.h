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

#include "QwkRegBlueAlias.h"

class TH1;
class TChain;
class QwkRegBlueTransform {
 public:
  std::vector < TString >   chanName; // list of local variable names

  std::vector < TString > ivName; // list of independent variables
  std::vector < int >     iv2chan; // list of independent variables
  std::vector < TString > dvName; // list of dependent variables
  std::vector < int >     dv2chan; // list of independent variables
  std::vector < TString > alsName; // list of aliased variables

  std::vector < QwkRegBlueAlias > aliasVec; // list of aliases operators

  TString inpTree, regVarName, inpPath;
  TString cutName, cutFormula;

  int ndv()  {return (int)dvName.size();}
  int niv()  {return (int)ivName.size();}
  int nals() {return (int)alsName.size();}
  int nchan(){return (int)chanName.size();}

  void readConfig(const char * configFName);
  void presetMyStat(double x1,double x2);
  TString setLeafName2Yield(TString longName);
  double pattern(){return  pattern_number;}

  int findAddChan(TString name);


 private:  
  Double_t pattern_number;
  TString myName;
 
  void getOneBranch(TChain *chain,TString name,QwkChanJan *qwkChan);

  // histograms
  enum {mxHA=8}; TH1 * hA[mxHA];
  void initHistos();
  
  int harvestNames(FILE *fp);

 public:
  QwkRegBlueTransform(const char *core); 
  void print();
  void findInputLeafs(TChain *chain);
  bool unpackEvent();
 

  QwkChanJan  *qwkChan;
  Double_t *Pvec, *Yvec; // used by regression class

  void init();
  void finish();

  TString errorMask;
  
};


#endif

  /* arrays w/ final 15 dv & iv variables
     1-bar tube combos:    MD 1-8 (all bars)
     2-bar combos:             MD H (3+7), V (1+5), D1 (2+6), D2(4+8)
     4-bar combos:             MD C (1+3+5+7), MD X (2+4+6+8)
     all:	                               MD ALL (1+2+3+4+5+6+7+8)

     named: md1,...,8, h,v,d1,d2,c,x,all
  */

