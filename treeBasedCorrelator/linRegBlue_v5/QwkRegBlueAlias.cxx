/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 *  aliasing capability
 *********************************************************************/
#include <stdio.h>
#include <assert.h>

#include  "QwkRegBlueAlias.h"

//========================
//========================
QwkRegBlueAlias::QwkRegBlueAlias( TString full){
  oper=kOperBad;
  rawText=full;
  kout=kin1=kin2=-1;
  TString copy=full.Copy();
  char *p0=(char*)copy.Data();
  char *p1=strstr(p0,"=");
  assert(p1);
  p1++;
  //printf("p1S=%s\n",p1);
  p1[-1]=0;
  outName=p0;

  char *p2=strstr(p1,"-");
  if(p2) oper=kOperMinus;
  else {
    p2=strstr(p1,"*");
    if(p2) oper=kOperTimes;
  }
  p2++;
  p2[-1]=0;
  inpName1=p1;
  inpName2=p2;

  assert(oper!=kOperBad);

}



//========================
//========================
void
QwkRegBlueAlias::print(){
  printf("QwkRegBlueAlias::print()  rawText: %s \n   oper=%d, outName=%s, inp1=%s, inp2=%s\n",rawText.Data(),oper, outName.Data(), inpName1.Data(), inpName2.Data());
  printf("   alias indexes for chan-array %d %d %d\n",kout,kin1,kin2);
}

//========================
//========================
void
QwkRegBlueAlias::compute_hw_sum(QwkChanJan  *qwkChan){
  //printf("QwkRegBlueAlias::compute()  3*ind %d %d %d\n", kout,kin1,kin2);
  QwkChanJan  *in1=qwkChan+kin1;
  QwkChanJan  *in2=qwkChan+kin2;
  QwkChanJan  *out=qwkChan+kout;
  memset(out,0,sizeof(QwkChanJan ));//clear output, just in case

  // self consistency test
  assert(in1->num_samples==in2->num_samples);
  out->Device_Error_Code=in1->Device_Error_Code+in2->Device_Error_Code;
  out->num_samples=in2->num_samples;

  if(oper==kOperMinus) 
    out->hw_sum=in1->hw_sum - in2->hw_sum;

  if(oper==kOperTimes) 
    out->hw_sum=in1->hw_sum * in2->hw_sum;

  //printf("QwkRegBlueAlias::compute() oper=%d  3*val %f %f %f\n", oper,out->hw_sum ,in1->hw_sum,in2->hw_sum);

}
