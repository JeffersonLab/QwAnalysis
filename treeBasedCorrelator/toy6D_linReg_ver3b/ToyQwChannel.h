#ifndef TOY6DEVENT_H
#define TOY6DEVENT_
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * C-struct encapsulates  synthetic QW channel
 * Utility class CorrelatorEntity  dealing with I/O of list of iv & dv
 *********************************************************************/


enum {kheliP=0, kheliN=1, mxHeli=2};
// for now assume 6 iv and 4 dv, only 2 helicity states

typedef struct {
  Double_t block[2];// not used
  Double_t hw_sum;
} ToyQwChannel;  



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

