/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Main program , run correlation and computes alphas for  QW event 
 * 
 *********************************************************************/
#include <cstdio>// C++ standard since 1999
#include <iostream>
#include "stdlib.h"
using namespace std;

#include <TFile.h>
#include <TH2.h>
#include <TTree.h> 

#include <TChain.h>
#include <TObjArray.h>
#include <TEventList.h> //tmp

#include "QwkRegBlueTransform.h"
#include "QwkRegBlueCorrelator.h"
#include "QwkRegBlueTree.h"
// use only double

int parMinEve=5000;

int main(int argc, char *argv[]) {
  int mxEve=500;
  char *runSegList=0;

  const char * outPath="./out/";
  const char * configFName="blueReg.conf";
  const char * slopeFName=0;

  // decode input params
  if(argc<2) {
    printf("Syntax: %s runNo [runSeg]  [neve] [config] [slopes] \n STOP",argv[0]);
    printf("    e.g.  %s 5762 [0.2.3]  [6000] [blueReg.conf]  [blue.slope.root] \n",argv[0]); return -1;
  }
  int runNo=atoi(argv[1]);
  if ( argc>=3)  runSegList=argv[2];
  if ( argc>=4)  mxEve =atoi(argv[3]);
  if ( argc>=5)  configFName=argv[4];
  if ( argc>=6)  slopeFName =argv[5];

  printf("Selected run=%d seg=%s  mxEve=%d   config=%s  slope=%s\n", runNo,runSegList,mxEve,configFName,slopeFName);
 
  //...... access to inpute leafs
  QwkRegBlueTransform eve("myFilter"); //  unpacks event, filters out bad events
  eve.readConfig(configFName);

  QwkRegBlueCorrelator corA("input"); //   correlator , passive 

  //..... try to open known correlation coef's.......
  QwkRegBlueCorrelator *corB=0;
  double *YvecNew=0;
  TMatrixD *alphasM=0; // indicator that coef's exist
  QwkRegBlueTree *blueTree=0;
  TFile*  mBlueFile=0;  

  if( slopeFName) {
    TString corFileName=Form("%s%s",outPath,slopeFName);
    TFile*  corFile=new TFile(corFileName);
    if( !corFile->IsOpen()) {
      printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
      return 0;
    }

    alphasM=(TMatrixD *) corFile->Get("slopes");
    assert(alphasM);
    corB=new QwkRegBlueCorrelator("regres"); //   2nd correlator after regression
    YvecNew=new double [eve.ndv()];
    
    printf("opened %s, slopes found, dump:\n",corFile->GetName());
    alphasM->Print();
    corFile->Close();
  }
  
  // .........   input  event file   .........
  TChain *chain = new TChain("Hel_Tree");
  TChain *chain_reg = new TChain("Hel_Tree_Reg");
  TString runName="yyy", treeInpFile="zzz";
  //printf("mmm,%s,%p\n",runSegList,strstr(runSegList,"."));
  if ( strstr(runSegList,".")<=0) { // just 1 run segment
    runName=Form("%d.%03d",runNo,atoi(runSegList));
    treeInpFile=eve.inpPath+runName+".root";
    printf("Open to read  =%s=\n",treeInpFile.Data());
    chain->Add(treeInpFile);
    chain_reg->Add(treeInpFile);
  } else {
    printf(" concatenate series of run segements : %s\n",runSegList);
    char *item=strtok(runSegList,"."); // init 'strtok'
    int i=1;
    do {
      //printf("i=%d, item=%s=\n",i,item);
      runName=Form("%d.%03d",runNo,atoi(item));
      treeInpFile=eve.inpPath+runName+".root";
      printf("Open i=%d =%s=\n",i++,treeInpFile.Data());
      chain->Add(treeInpFile);
      chain_reg->Add(treeInpFile);
    } while ((item=strtok(0,".")));  // advance by one name
    runName=Form("%d.all",runNo);
    treeInpFile=eve.inpPath+runName+".root";
  }
  chain->AddFriend(chain_reg);

  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain \nscan leafs for iv & dv ...\n",nEve);
  printf("#totEve %d\n",nEve);
  if(nEve <parMinEve) {
    printf("aborting linear regerssion due to small # of events\n");
    printf("#abort JB1 nEve=%d\n",nEve);
    exit(1);
  }

  if( slopeFName) {
    TString xxx=treeInpFile;
    printf("xxx=%s=\n",xxx.Data());
    const char *xx2=strstr(xxx,"/");
    assert(xx2); // if input tree name does not start with 'Qw' change the line above
    xx2++;
    const char *xx3;
    do{
      printf("xx2=%s=\n",xx2);
      xx3=strstr(xx2,"/");
      if(xx3 == 0) {
	//	xx3 = xx2;
      } else {
	xx3++;
	xx2 = xx3;
      }
    } while (xx3 != 0);
    if(xx3 == 0) 
      xx3 = xx2;
    printf("xx3=%s=\n",xx3);
    TString treeOutName=Form("%sreg_%s",outPath,xx3);
    mBlueFile=new TFile(treeOutName,"RECREATE"," regressed  Qweak tree");
    printf("Open to write  =%s=\n",treeOutName.Data());
    assert(mBlueFile->IsOpen());
    blueTree=new QwkRegBlueTree(eve.dvName);
  }

  TString hfileName=Form("%sblueR%s.hist.root",outPath,runName.Data());
  TFile*  mHfile=new TFile(hfileName,"RECREATE"," histograms w/ regressed Qweak data");
  assert(mHfile->IsOpen());
  printf(" Setup output  histo to '%s' ,\n",hfileName.Data());

  eve.init();  // creates histo: WARN: output histo file must be already opened   
 
  eve.findInputLeafs(chain);

  eve.print();
  corA.init( eve.ivName, eve.dvName );   // creates histo 
  if(alphasM) corB->init( eve.ivName, eve.dvName); 


  // filter events with custom cut.
  TString cutFormula="ErrorFlag==0"; // apply general pattern QA cut
 
  if(eve.cutFormula.Sizeof()>1) {
    printf("Main: filter events with custom cut: name=%s  formula='%s'\n",eve.cutName.Data(), eve.cutFormula.Data());
    cutFormula+="&&"+eve.cutFormula;
  }

#if 0 // disable cut on beam current stability
  chain->Draw(">>listA",cutFormula); // custom cleanup cut
  TEventList *list = (TEventList*)gDirectory->Get("listA"); 
  list->Print();
  double listA_len= list->GetN();
  printf("main list A size=%g\n", listA_len);

  chain->Draw("yield_qwk_bcm1.hw_sum>>histA",cutFormula);  
  TH1* histA=(TH1*)gDirectory->Get("histA");
  double xAvr=histA->GetMean();
  int bin=histA->GetMaximumBin();
  double xMPV=histA->GetXaxis()->GetBinCenter(bin);

  printf("BCM1 average=%.1f (uA)  MPV=%.1f (uA), nEntries=%.0f\n",xAvr,xMPV,histA->GetEntries());
  double bcmThres=xMPV-5.0;
  cutFormula="("+cutFormula+Form(")&&(yield_qwk_bcm1.hw_sum >%.1f)",bcmThres);
#endif

  cutFormula="("+cutFormula+Form(")");
  printf("final cutForm='%s'\n",cutFormula.Data());
  chain->Draw(">>listB",cutFormula); // cut includes BCM1 
  TEventList *list = (TEventList*)gDirectory->Get("listB"); 
  list->Print();
  double listB_len= list->GetN();
  printf("main list B size=%d\n", list->GetN()); 

  eve.presetMyStat(chain->GetEntries(),  listB_len);

  int t1=time(0);
  int ie;
  int seenEve=0;
  if(nEve>mxEve) nEve=mxEve;
  for( ie=0;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    //printf("ee=%d pat=%.1f\n",ie,eve.pattern());
    if(ie%5000==0) printf(" ieve=%d of %d , seen=%d ...\n",ie,nEve,seenEve);
    if(!list->Contains(ie)) { //tmp && 0 
      if(blueTree) blueTree->FillEmpty(eve.pattern());
      continue;
   }
    //printf("xx %d \n",ie);
    if(!eve.unpackEvent()) {
      if(blueTree)blueTree->FillEmpty(eve.pattern());
      continue;
    }

    seenEve++;
    corA.addEvent(eve.Pvec, eve.Yvec);
    if(alphasM) {// regress dv's
      for (int iy = 0; iy <eve.ndv(); iy++) {
	YvecNew[iy]=eve.Yvec[iy];
	for (int ix = 0; ix < eve.niv(); ix++) {
	  YvecNew[iy]-=eve.Pvec[ix]*(*alphasM)(ix,iy);
	}
      }
      corB->addEvent(eve.Pvec, YvecNew);
      blueTree->Fill(eve.pattern(),YvecNew);
      
    }
  }// end of event loop

  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);
  if(seenEve <parMinEve) {
    printf("aborting linear regerssion due to small # of events\n");
    printf("#abort JB2 seenEve=%d\n",seenEve);
    eve.finish();
    mHfile->Write(); 
    mHfile->Close();
    exit(1);
  }

  printf("#seenEve %d\n",seenEve);

  corA.finish();
  eve.finish();

  if(alphasM) {
    corB->finish();
    blueTree->finish();
    mBlueFile->Write();
    printf("\n Histo saved -->%s<\n",mBlueFile->GetName());
    mBlueFile->Close();
  }

  mHfile->Write(); 
  mHfile->Close();
  TString outAlphas=Form("%sblueR%snew.slope.root",outPath,runName.Data());
  corA.exportAlphas(outAlphas, eve.ivName, eve.dvName); 
  if(alphasM) {
    outAlphas=Form("%sblueR%snewB.slope.root",outPath,runName.Data());
    corB->exportAlphas(outAlphas, eve.ivName, eve.dvName); 
  }


  corA.exportAlias(outPath, "regalias_"+runName, eve.ivName, eve.dvName); 

  printf("#success JB1 nEve=%d  for %s\n",nEve,runName.Data());
}


