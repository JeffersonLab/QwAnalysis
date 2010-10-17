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

 private:
   TString mCore;
  enum{ mxMD=8, mxPN=2};// list:  MD, pos/neg
  enum{ mxBPM=4};
  
  // pointers to leaf variables
  Double_t *pLeafMD[mxMD*mxPN]; 
  Double_t *pLeafBCM;
  Double_t *pLeafBPM[mxBPM]; 

  // histograms
  enum {mxHA=128}; TH1 * hA[mxHA];
  void initHistos();
  
 public:
  JbLeafTransform(const char *core); 
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

  enum{ nP=mxBPM,nY=8+4+2+1}; 
  Double_t Pvec[nP], Yvec[nY];
  TString  Pname[nP],  Yname[nY];

  //old
  void init();
  void finish();
  
};




#endif


#if 0

//--------------------------
class CorrelatorEntity {
public:
  std::vector < TString > ivName; // list of independent variables
  std::vector < TString > dvName; // list of dependent variables
  TString inpTree, myName;

  int ndv(){return (int)dvName.size();}
  int niv(){return (int)ivName.size();}

  //..................
  void printCE() {
    printf("CorrFac: myName='%s' , inpTree='%s'  len: dv=%d iv=%d \nDV: ",myName.Data(),inpTree.Data(),(int)dvName.size(),(int)ivName.size());
    for(unsigned int i=0; i<dvName.size(); i++) printf("%s, ",dvName[i].Data());
    printf("\nIV: ");
    for(unsigned int i=0; i<ivName.size(); i++) printf("%s, ",ivName[i].Data());
    printf("\nCorrFac: print-end\n");
  }

  //...................
  int harvestNames(FILE *fp) {
    assert(fp);
    enum {mx=1000};
    char buf[mx];
    int nl=0;
    int state=0; 
    while(  fgets( buf, mx, fp)) {
      nl++;
      if (strstr(buf,"#")>0) continue;
      char *x=strstr(buf, "\n"); if(x)*x=0;
      //printf("line=%d  buf=%s=\n",nl,buf);
      if(state==0) { // found block name
	if (strstr(buf, "block")==0) continue;// first search for new block
	state=1;
	myName=buf+6;
	continue;
      } 
      if(state==1){ // increment iv & dv name lists
	if (strstr(buf, "iv")) ivName.push_back(buf+3);
	if (strstr(buf, "dv")) dvName.push_back(buf+3);
	if (strstr(buf, "treetype")==0) continue;// first search for new block
	state=2;
	inpTree=buf+9;
	continue;	
      }
    }
    printCE();
    return state;
  }

};


#endif
