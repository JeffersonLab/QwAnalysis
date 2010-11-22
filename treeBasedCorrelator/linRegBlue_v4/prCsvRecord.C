#include <iostream>
vector <TString> ivName, dvName;
vector <double> dvmYield; 
double run_seg=1111.222;
TString inpPath="FIXmE1";
double avrCurr_uA=-1, pattSum=-2,sumCharge_mC=-3;
TMatrixD *   Mslopes=0, *MsigSlopes=0, *   MDVsig=0,  *MIVsig=0, *MIVcov=0;
int start_time=-1;

prCsvRecord(double run_seg0=7214.001, int start_time0=12345678, TString inpPath0="sort7100+/", TString outPath="out/") {
  run_seg=run_seg0;
  inpPath=inpPath0;
  start_time=start_time0;
  fetchNamesAnd();
  int nP=ivName.size();
  int nY=dvName.size();
  printf(" found nIV=%d  nDV=%d  time=%d\n",nP,nY,start_time);
  readSlopes();
  readDVrms();

  int iv=0;
  for(int dv=0;dv<dvmYield.size(); dv++) {
    printf("dv=%d %s  yield=%f  sl(iv=0)=%f  rms=%f\n",dv, dvName[dv].Data(), dvmYield[dv],Mslopes(iv,dv), MDVsig(dv,0));
  }

  int doLabel=0;
  int irun=int(run_seg);
  int seg= int(1000* ( run_seg-irun));
  printf("xx %d %d doL=%d\n",irun,seg,irun%5==0 && seg==1);
  if (irun%5==0 && seg==1) doLabel=1; 
  TString csvFileName=outPath+Form("blueR%.3f.csv",run_seg);
  FILE *fd=fopen(csvFileName.Data(),"w");
  printRecord(fd,doLabel);
  fclose(fd);
}


//================================
void printRecord(FILE *fd,  int doLabel=0){
  char eol='\n';
  //......... yields for 8 DVs ........
  if(doLabel) {
    fprintf(fd,"run.seg, yield, tot charge (mC), avr current (uA) , tot pattern, start time ,");
    for(int dv=0;dv<dvmYield.size(); dv++) 
      fprintf(fd,"%s yield, ",dvName[dv].Data());
    fprintf(fd,"%c",eol);
  }
  fprintf(fd, "%.3f, yield , %.2f, %.1f, %.2g, %d ,",run_seg,sumCharge_mC,avrCurr_uA,pattSum,start_time);
  
  for(int dv=0;dv<dvmYield.size(); dv++) 
    fprintf(fd,"%4g, ",dvmYield[dv]);
  fprintf(fd,"%c",eol);

  //......... RMS of regressed DVs .....
  if(doLabel) {
    fprintf(fd,"run.seg, RMS reg DVs, ");
    for(int dv=0;dv<dvName.size(); dv++) 
      fprintf(fd,"%s RMS, ",dvName[dv].Data());
    fprintf(fd,"%c",eol);
  }
  fprintf(fd, "%.3f, RMS reg,",run_seg);
  for(int dv=0;dv<dvName.size(); dv++) 
    fprintf(fd,"%4g, ",MDVsig(dv,0));
  fprintf(fd,"%c",eol);

  //......... Slopes of for  DVs .....
  // int iv=0;
  for(int iv=0;iv<ivName.size(); iv++) 
    {
      if(doLabel) {
	fprintf(fd, "run.seg, slope(iv=%d) ,",iv);
	for(int dv=0;dv<dvName.size(); dv++) 
	  fprintf(fd,"%s/%s, error, ",dvName[dv].Data(),ivName[iv].Data());
	fprintf(fd,"%c",eol);
      }
      fprintf(fd, "%.3f,slope(%s) ,",run_seg,ivName[iv].Data());
      for(int dv=0;dv<dvName.size(); dv++) 
	fprintf(fd,"%4g, %4g ,",Mslopes(iv,dv),MsigSlopes(iv,dv));
      fprintf(fd,"%c",eol);
    }


  //......... RMS   of   IVs .....
  if(doLabel) {
    fprintf(fd, "run.seg, RMS IVs ,");
    for(int iv=0;iv<ivName.size(); iv++) 
      fprintf(fd,"%s RMS ,",ivName[iv].Data());
    fprintf(fd,"%c",eol);
  }
  fprintf(fd, "%.3f,RMS IVs,",run_seg);
  for(int iv=0;iv<ivName.size(); iv++) 
    fprintf(fd,"%4g,",MIVsig(iv,0));
  fprintf(fd,"%c",eol);
  
  //.........  covar   of   IVs .....
  if(doLabel) {
    fprintf(fd, "run.seg, covar IVs ,");
    for(int iv=0;iv<ivName.size(); iv++) 
      for(int jv=iv+1;jv<ivName.size(); jv++) 
	fprintf(fd,"%s:%s ,",ivName[iv].Data(),ivName[jv].Data());
    fprintf(fd,"%c",eol);
  }
  fprintf(fd, "%.3f,covar IVs,",run_seg);
  for(int iv=0;iv<ivName.size(); iv++) 
    for(int jv=iv+1;jv<ivName.size(); jv++) 
      fprintf(fd,"%4g,",MIVcov(iv,jv));
  fprintf(fd,"%c",eol);
  


  
  //...... misc
  if(doLabel) 
    fprintf(fd,"units:  MD yield (~V/uA); regressed  MD asy RMS (ppm);  DV/IV slopes (1/mm);  RMS IV (mm);  corr IV (1) \n");
  
  

} 

//================================
void readDVrms() {
 //.......read  matrix root .......
  TString corFileName=inpPath+Form("blueR%.3fnewB.slope.root",run_seg);
  TFile*  corFile=new TFile(corFileName);
  if( !corFile->IsOpen()) {
    printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
    return ;
  }
  MDVsig=(TMatrixD *) corFile->Get("DV_sigma");assert(MDVsig);
}

//================================
void readSlopes() {
 //.......read  matrix root .......
  TString corFileName=inpPath+Form("blueR%.3f.slope.root",run_seg);
  TFile*  corFile=new TFile(corFileName);
  if( !corFile->IsOpen()) {
    printf("Failed to open %s, slopes NOT found\n",corFile->GetName());
    return ;
  }

  Mslopes=(TMatrixD *) corFile->Get("slopes");assert(Mslopes);
  MsigSlopes=(TMatrixD *) corFile->Get("sigSlopes");assert(MsigSlopes);
  MIVsig=(TMatrixD *) corFile->Get("IV_sigma");assert(MIVsig);
  MIVcov=(TMatrixD *) corFile->Get("IV_covariance");assert(MIVcov);

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
  pattSum=hBcm->GetEntries();
  sumCharge_mC=avrCurr_uA*pattSum * 4e-6;
  printf(" run=%.3f tot charge=%.1f (mC) , avr curr=%.1f (uA), pattSum=%.2g \n",run_seg,sumCharge_mC,avrCurr_uA,pattSum);

  for(int dvm=0;dvm<8;dvm++) {
    TH1 *hDV=(TH1*) hFile->Get(Form("yieldDV%d",dvm)); assert(hDV);
    double yield=hDV->GetMean();
    dvmYield.push_back(yield);
  }


  hFile->Close();
}

