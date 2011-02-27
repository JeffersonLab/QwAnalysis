#ifndef JBBLUETREE_H
#define  JBBLUETREE_H

/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Generator of  synthetic QW event with build in fals asymmetries
 *********************************************************************/

using namespace std;
#include <TTree.h>


class JbBlueTree {
 public:
  JbBlueTree(std::vector < TString > dvName) ;
  void finish();
  void FillEmpty(double patternNo);
  void Fill(double patternNo, double *Y);
 private:
  int mxY;
  TTree*  mTree;
  Double_t pattern_number;  // holds helicity info
  Int_t regGlobErrorCode;  // flags all bad or invalid entries
  Double_t *regDvValue;  // array of regressed DVs

};


#endif
