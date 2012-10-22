#include <iostream>
vector <TString> ivName, dvName;
vector <double> dvYield, dvYieldRms; 
vector <double> ivYield, ivYieldRms; 
double run_seg=1111.222;
TString inpPath="FIXmE1";
double avrCurr_uA=-1, rmsCurr_uA=-2,pattSum=-2,sumCharge_mC=-3;
TMatrixD *   Mslopes=0, *MsigSlopes=0, 
  *MDVsigReg=0,  *MDVsigUnreg=0,  
  *MDVmeanReg=0, *MDVmeanUnreg=0,
  *MIVsig=0, *MIVcov=0, *MIVmean=0;

prCsvRecordTwo( double run_seg0=7214.001, TString inpPath0="sort7100+/", TString outPath="out/") {
  run_seg=run_seg0;
  inpPath=inpPath0;

  fetchNamesAnd();
  int nP=ivName.size();
  int nY=dvName.size();
  printf(" prCsvRecordTwo: found nIV=%d  nDV=%d \n",nP,nY);
  readSlopes();
  readDVrmsReg();
 
  int iv=0;
  for(int dv=0;dv<dvYield.size(); dv+=5) {
    printf("dv=%d %s  yield=%f  sl(iv=0)=%f  rms=%f\n",dv, dvName[dv].Data(), dvYield[dv],Mslopes(iv,dv), MDVsigReg(dv,0));
  }

  int doLabel=0;
  int irun=int(run_seg);
  int seg= int(1000* ( run_seg-irun)+0.5);
  
  TString csvFileName=outPath+Form("blueR%.3f_DBrecord.txt",run_seg);
  
  printf(" write  --> %s\n",csvFileName.Data());
  FILE *fd=fopen(csvFileName.Data(),"w");
  printDBrecord(fd);
  fclose(fd);
  
  
}



//================================
void printDBrecord(FILE *fd){
  double ppm=1e-6; // to convert back to absolute units 
  unsigned int mask=0xff-0x2-0x80;
  const int csvVer=4;
  double sqN=sqrt(pattSum);

  if(mask&0x1) { fprintf(fd,"#section:0x1:varia:X---------------------\n");
    fprintf(fd,"run.seg:%.3f:X\n",run_seg);
    fprintf(fd,"yield_qwk_bcm1:%g:%g:%g\n",pattSum,avrCurr_uA, rmsCurr_uA/sqN);
  }

  if(mask&0x2) {    fprintf(fd,"#section:0x2:yields for %d DVs:X----------------\n",dvName.size());
    for(int dv=0;dv<dvName.size(); dv++) {
      TString name=dvName[dv];
      name.ReplaceAll("asym_","yield_");
      fprintf(fd,"%s:%g:%g:%g\n",name.Data(),pattSum,dvYield[dv],dvYieldRms[dv]/sqN);
    }
  }

  if(mask&0x4) {    fprintf(fd,"#section:0x4:regressed %d asym DVs:X---------------\n",dvName.size());
    for(int dv=0;dv<dvName.size(); dv++) {
      TString name=dvName[dv];
      fprintf(fd,"%s:%g:%g:%g\n",name.Data(),pattSum,MDVmeanReg(dv,0)*ppm,MDVsigReg(dv,0)*ppm/sqN);
    }
  }

  if(mask&0x8) {    fprintf(fd,"#section:0x8:unregressed %d asym DVs:X--------------\n",dvName.size());
    for(int dv=0;dv<dvName.size(); dv++) {
      TString name=dvName[dv];
      fprintf(fd,"%s:%g:%g:%g\n",name.Data(),pattSum,MDVmeanUnreg(dv,0)*ppm,MDVsigUnreg(dv,0)*ppm/sqN);
    }
  }

  if(mask&0x10) {    fprintf(fd,"#section:0x10:slopes for %d asym DVs vs. %d IVs:X-----------------\n",dvName.size(),ivName.size());
    for(int dv=0;dv<dvName.size(); dv++) {
      TString nameDV=dvName[dv];
      for(int iv=0;iv<ivName.size(); iv++) {
	TString nameIV=ivName[iv];
	fprintf(fd,"%s/%s:%g:%g:%g\n",nameDV.Data(),nameIV.Data(),pattSum,Mslopes(iv,dv),MsigSlopes(iv,dv));
      }
    }
  }


  if(mask&0x20) {    fprintf(fd,"#section:0x20: %d diff  IVs:X---------------\n",ivName.size());
    for(int iv=0;iv<ivName.size(); iv++) {
      TString nameIV=ivName[iv];
      fprintf(fd,"%s:%g:%g:%g\n",nameIV.Data(),pattSum,MIVmean(iv,0)*ppm,MIVsig(iv,0)*ppm/sqN);
    }
  }
  
  if(mask&0x40) {    fprintf(fd,"#section:0x40: correlations for %d diff  IVs:X---------------\n",ivName.size());
    for(int iv=0;iv<ivName.size(); iv++) {
      TString nameIV1=ivName[iv];
      for(int jv=iv+1;jv<ivName.size(); jv++) {
	TString nameIV2=ivName[jv];
	fprintf(fd,"%s:%s:%g:%g:X\n",nameIV1.Data(),nameIV2.Data(),pattSum,MIVcov(iv,jv));
      }
    }
  }
  
  if(mask&0x80) {    fprintf(fd,"#section:0x80:yields for %d IVs:X----------------\n",ivName.size());
    for(int iv=0;iv<ivName.size(); iv++) {
      TString name=ivName[iv];
      name.ReplaceAll("asym_","yield_");
      fprintf(fd,"%s:%g:%g:%g\n",name.Data(),pattSum,ivYield[iv],ivYieldRms[iv]/sqN);
    }
  }



}
  

//================================
void readDVrmsReg() {
 //.......read  matrix root .......
  TString corFileName=inpPath+Form("blueR%.3fnewB.slope.root",run_seg);
  TFile*  corFile=new TFile(corFileName);
  if( !corFile->IsOpen()) {
    printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
    return ;
  }
  MDVmeanReg=(TMatrixD *) corFile->Get("DV_mean");assert(MDVmeanReg);
  MDVsigReg=(TMatrixD *) corFile->Get("DV_sigma");assert(MDVsigReg);
}



//================================
void readSlopes() { // unregressed values
 //.......read  matrix root .......
  TString corFileName=inpPath+Form("blueR%.3f.slope.root",run_seg);
  TFile*  corFile=new TFile(corFileName);
  if( !corFile->IsOpen()) {
    printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
    return ;
  }
  //corFile->ls();
  Mslopes=(TMatrixD *) corFile->Get("slopes");assert(Mslopes);
  MsigSlopes=(TMatrixD *) corFile->Get("sigSlopes");assert(MsigSlopes);
  MIVsig=(TMatrixD *) corFile->Get("IV_sigma");assert(MIVsig);
  MIVmean=(TMatrixD *) corFile->Get("IV_mean");assert(MIVmean);
  MIVcov=(TMatrixD *) corFile->Get("IV_correlation");assert(MIVcov);

  MDVsigUnreg=(TMatrixD *) corFile->Get("DV_sigma");assert(MDVsigUnreg);
  MDVmeanUnreg=(TMatrixD *) corFile->Get("DV_mean");assert(MDVmeanUnreg);

#if 0 
  printf("SL p=%p %p\n",  Mslopes,MsigSlopes);
  int iv=1,dv=1;
  double sl=Mslopes(iv,dv);
  double err=MsigSlopes(iv,dv);
  printf("-->R%.3f  iv=%d sl=%f +/- %f\n", run_seg,iv,sl,err);
#endif
}

//================================
void fetchNamesAnd() {
  TString corFileName=inpPath+Form("blueR%.3f.hist.root",run_seg);
  TFile*  hFile=new TFile(corFileName);
  if( !hFile->IsOpen()) {
    printf("Failed to open %s,  NOT found\n",hFile->GetName());
    return ;
  }

  printf("Opened %s\n",hFile->GetName());

  //........  
  TH1 *hDV=(TH1*) hFile->Get("inputNamesDV"); assert(hDV);
  TAxis *ax=hDV->GetXaxis();
  for(int b=1;b<=ax->GetNbins();b++) {
    TString name=ax->GetBinLabel(b);
    // printf(" dv=%d name=%s\n", b-1,name.Data());
    dvName.push_back(name);
  }

  //........  
  TH1 *hIV=(TH1*) hFile->Get("inputNamesIV"); assert(hIV);
  ax=hIV->GetXaxis();
  for(int b=1;b<=ax->GetNbins();b++) {
    TString name=ax->GetBinLabel(b);
    //printf(" iv=%d name=%s\n", b-1,name.Data());
    ivName.push_back(name);
  }

  //..... AND more ....

  // .... total charge, average current
  hBcm=(TH1D*) hFile->Get("inpBcm1"); assert(hBcm);
  avrCurr_uA=hBcm->GetMean();
  rmsCurr_uA=hBcm->GetRMS();
  pattSum=hBcm->GetEntries();
  sumCharge_mC=avrCurr_uA*pattSum * 4e-6;
  printf(" run=%.3f tot charge=%.1f (mC) , avr curr=%.1f (uA), pattSum=%.2g \n",run_seg,sumCharge_mC,avrCurr_uA,pattSum);

  hFile->Close();
}
