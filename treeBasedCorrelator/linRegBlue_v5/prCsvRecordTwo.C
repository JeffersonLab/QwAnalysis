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
enum {kMD=0,kLUMI=1,kDBdump=2};
TString miscValBlob;
int type;
char *ctypeA[3]={"MD","LUMI","DBdump"}, *cDV=ctypeA[2];

// .... asym for BCM's + some DDs for MD
 const int mxBB=8+1+3;
  TString bbName[mxBB]={"asym_bcm1","asym_bcm2","asym_bcm5","asym_bcm6",
			"bcmDD12","bcmDD15","bcmDD25","bcmDD56",
			"asym_bpm3h09b_EfCh", 
                        "asym_DD_MDeven_odd", "asym_DD_MD15_37",  "asym_DD_MD26_48"};


double bcmRms[mxBB],  bcmMean[mxBB];
  

prCsvRecordTwo(int type0, double run_seg0=7214.001,TString miscVal, TString inpPath0="sort7100+/", TString outPath="out/") {
  run_seg=run_seg0;
  inpPath=inpPath0;
  type=type0;
  miscValBlob=miscVal;
  if(type==kMD)cDV=ctypeA[0];
  if(type==kLUMI)cDV=ctypeA[1];

  fetchNamesAnd();
  int nP=ivName.size();
  int nY=dvName.size();
  printf(" type=%d, found nIV=%d  nDV=%d  miscVal=%s\n",type,nP,nY,miscVal.Data());
  readSlopes();
  readDVrmsReg();
 
  int iv=0;
  for(int dv=0;dv<dvYield.size(); dv+=5) {
    printf("dv=%d %s  yield=%f  sl(iv=0)=%f  rms=%f\n",dv, dvName[dv].Data(), dvYield[dv],Mslopes(iv,dv), MDVsigReg(dv,0));
  }

  int doLabel=0;
  int irun=int(run_seg);
  int seg= int(1000* ( run_seg-irun)+0.5);
  printf("xx %d %d doL=%d\n",irun,seg,irun%5==0 && seg==1);

  TString csvFileName=outPath+Form("blueR%.3f_MD.csv",run_seg);
  if(type==kLUMI) csvFileName.ReplaceAll("_MD","_LUMI");
  if(type==kDBdump) csvFileName.ReplaceAll("_MD.csv","_DBrecord.txt");
  printf(" write  --> %s\n",csvFileName.Data());
  FILE *fd=fopen(csvFileName.Data(),"w");
  if(type!=kDBdump) printCsvRecord(fd,doLabel);
  else  printDBrecord(fd);
  fclose(fd);

  if(type==kDBdump) printHTML();

}


//================================
void printHTML() {

  printf("#regresRMS<th> regres \
         <th bgcolor=\"#FFFFcc\"> %.0f <td> %0.f  <td> %0.f \n\
         <th bgcolor=\"#FFccFF\">      %.0f <td> %0.f  <td> %0.f \n \
	 <th bgcolor=\"#ccFFFF\"> %.0f \n #regresRMS\n",
	 MDVsigReg(14,0),MDVsigReg(13,0),MDVsigReg(12,0),
	 MDVsigReg(15,0),MDVsigReg(17,0),MDVsigReg(16,0), 
	 MDVsigReg(22,0));  
 

  printf("#bcmRMS <th rowspan=2> %.0f <td rowspan=2> %.0f <td rowspan=2> %.0f \
          <td rowspan=2> %.0f\n  <th rowspan=2 bgcolor=\"#f5cece\"> %.0f  #bcmRMS",
         bcmRms[0], bcmRms[4], bcmRms[5], bcmRms[7], bcmRms[8]);
  // <th rowspan=2>  763<td rowspan=2> 123<td rowspan=2> 345<td rowspan=2> 987  
  //<th rowspan=2 bgcolor="#f5cece">  1234
  
  printf("#nonregresRMS<tr > <th><font size=\"-1\"> nonreg \
          <td>  <font size=\"-1\"> %.0f <td>  <font size=\"-1\"> %0.f  <td>  <font size=\"-1\"> %0.f \n\
         <td> <font size=\"-1\">   %.0f <td>  <font size=\"-1\"> %0.f  <td>  <font size=\"-1\"> %0.f \n \
	 <td> <font size=\"-1\"> %.0f \n #nonregresRMS\n",
	 MDVsigUnreg(14,0),MDVsigUnreg(13,0),MDVsigUnreg(12,0),
	 MDVsigUnreg(15,0),MDVsigUnreg(17,0),MDVsigUnreg(16,0), 
	 MDVsigUnreg(22,0));  

  //<tr > <th><font size="-1"> nonreg <td> <font size="-1"> 3653<td><font size="-1"> 6397<td> <font size="-1">4111<td > <font size="-1">7034<td> <font size="-1">1.02e+04<td> <font size="-1">1.02e+04<td> <font size="-1">3742

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
    char *buf=miscValBlob.Data(); 
    char *item=strtok(buf,","); // init 'strtok'
    fprintf(fd,"target:%s:X\n",item);   item=strtok(0,",");
    fprintf(fd,"raster:%s:X\n",item);   item=strtok(0,",");
    fprintf(fd,"half wave plate:%s:X\n",item);   item=strtok(0,",");
    fprintf(fd,"start time unix:%s:X\n",item);   item=strtok(0,",");
    fprintf(fd,"start time human:%s:X\n",item);   item=strtok(0,","); 
    fprintf(fd,"HClog URL:%s:X\n",item);   item=strtok(0,",");
    fprintf(fd,"operator comment:%s:X\n",item);   item=strtok(0,",");
 
    for(int j=0;j<mxBB;j++){
      TString  name=bbName[j];
      fprintf(fd,"%s:%g:%g:%g\n", name.Data(),pattSum,bcmMean[j]*ppm,bcmRms[j]*ppm/sqN);
    }
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
void printCsvRecord(FILE *fd,  int doLabel=0){
  unsigned int mask=0x1ff;
  const int csvVer=4;

  int idv1=0,idv2=15;
  if(type==kLUMI){idv1=15; idv2=45;}
  assert(idv2<=dvName.size());
  if(doLabel) {
    fprintf(fd,"units:  %s yield (~V/uA);   %s asy RMS (ppm);  DV/IV slopes (1/mm);  RMS IV (nm) ;  mean IV (nm);  mean DV (ppm) ; raster (Xmm x Ymm);  asym BCMs RMS&Mean (ppm)\n",cDV,cDV);
    
    if(mask&0x1) fprintf(fd,"run.seg, tot charge (mC), avr current (uA) , tot pattern, target type, raster, Half Wave Plate, start time unix , start date, HClog URL, operator comment, ");
    if(mask&0x2 ) { //......... yields for  DVs ........
      fprintf(fd," %s yields: ,",cDV);
      for(int dv=idv1;dv<idv2; dv++) {
	TString name=dvName[dv];
	name.ReplaceAll("asym_","");
	fprintf(fd,"%s yield, ",name.Data());
      }
    }

    if(mask&0x4) {//......... RMS of regressed DVs .....
      fprintf(fd," RMS reg %ss: , ",cDV);
      for(int dv=idv1;dv<idv2; dv++) 
	fprintf(fd,"%s RMS reg, ",dvName[dv].Data());
    }

    if(mask&0x8) {//......... RMS of unregressed DVs .....
      fprintf(fd," RMS unreg %ss: , ",cDV);
      for(int dv=idv1;dv<idv2; dv++) 
	fprintf(fd,"%s RMS unreg, ",dvName[dv].Data());
    }
    
    if(mask&0x100 && type==kMD) {// .... asym for BCM's    
      fprintf(fd," RMS BCMs asym: ,");
      for(int j=0;j<mxBB;j++) 
	fprintf(fd,"RMS %s ,", bbName[j].Data());
      
      fprintf(fd," Mean BCMs asym: ,");
      for(int j=0;j<mxBB;j++) 
	fprintf(fd,"Mean %s ,", bbName[j].Data());
      
    }

    if(mask&0x10) {//......... Slopes of for  DVs .....
      for(int iv=0;iv<ivName.size(); iv++) {
 	fprintf(fd, "slopes(iv=%d): ,",iv);
	for(int dv=idv1;dv<idv2; dv++) 
	  fprintf(fd,"%s/%s, error, ",dvName[dv].Data(),ivName[iv].Data());
	//	break;
      }
    }
    
    if(mask&0x20) { //......... RMS   of   IVs .....
      fprintf(fd, " RMS IVs: ,");
      for(int iv=0;iv<ivName.size(); iv++) 
	fprintf(fd,"%s RMS ,",ivName[iv].Data());
    }

    if(mask&0x40) { //......... mean   of   IVs .....
      fprintf(fd, " mean IVs: ,");
      for(int iv=0;iv<ivName.size(); iv++) 
	fprintf(fd,"%s mean ,",ivName[iv].Data());
    }

    if(mask&0x80) {//......... mean of unregressed DVs .....
      fprintf(fd," mean unreg %ss: , ",cDV);
      for(int dv=idv1;dv<idv2; dv++) 
	fprintf(fd,"%s mean unreg, ",dvName[dv].Data());
    }
    
    fprintf(fd," %scsv%d_%d\n",cDV, csvVer,mask);
  }


  // VVVVVVVVVVVVVVVVVVVVVVVVV     VALUES          VVVVVVVVVVVVVVVVVV
  if(mask&0x1) fprintf(fd, "%.3f, %.2f, %.1f, %.0f,  %s ,",run_seg,sumCharge_mC,avrCurr_uA,pattSum,miscValBlob.Data());
   if(mask&0x2){     //......... yields for 8 DVs ........
     fprintf(fd,"  %s yields: ,",cDV);
     for(int dv=idv1;dv<idv2; dv++) 
       fprintf(fd,"%6g, ",dvYield[dv]);
   }
   if(mask&0x4) {//......... RMS of regressed DVs .....
     fprintf(fd," RMS reg %ss: , ",cDV);
     for(int dv=idv1;dv<idv2; dv++) 
       fprintf(fd,"%6g, ",MDVsigReg(dv,0));
    }

   if(mask&0x8) {//......... RMS of unregressed DVs .....
     fprintf(fd," RMS unreg %ss: , ",cDV);
     for(int dv=idv1;dv<idv2; dv++) 
       fprintf(fd,"%6g, ",MDVsigUnreg(dv,0));
    }

   
   if(mask&0x100 && type==kMD) {// .... asym for BCM's    
      fprintf(fd," RMS BCMs asym: ,");
      for(int j=0;j<mxBB;j++) 
	fprintf(fd,"%6g ,", bcmRms[j]);
      
     fprintf(fd," Mean BCMs asym: ,");
      for(int j=0;j<mxBB;j++) 
	fprintf(fd,"%6g ,", bcmMean[j]);
      
     }

   if(mask&0x10) {//......... Slopes of for  DVs .....
     for(int iv=0;iv<ivName.size(); iv++) {
       fprintf(fd, "slopes(%s): ,",ivName[iv].Data());
       for(int dv=idv1;dv<idv2; dv++) 
	 fprintf(fd,"%6g, %6g ,",Mslopes(iv,dv),MsigSlopes(iv,dv));
       //break;
      }
    }
  

   if(mask&0x20) { //......... RMS   of   IVs .....
      fprintf(fd, " RMS IVs: ,");
      for(int iv=0;iv<ivName.size(); iv++) 
	fprintf(fd,"%6g,",MIVsig(iv,0));
    }

   if(mask&0x40) { //......... mean   of   IVs .....
      fprintf(fd, " mean IVs: ,");
      for(int iv=0;iv<ivName.size(); iv++) 
	fprintf(fd,"%6g,",MIVmean(iv,0));
    }

    if(mask&0x80) {//......... mean of unregressed DVs .....
      fprintf(fd," mean unreg %ss: , ",cDV);
      for(int dv=idv1;dv<idv2; dv++) 

	fprintf(fd,"%6g,",MDVmeanUnreg(dv,0));
    }
    
    fprintf(fd," %scsv%d_%d\n",cDV, csvVer,mask);
    return;
 

  //XXXXXXXXXXXXXXXXXX
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

 
  if(0) {
  for(int dvm=0;dvm<dvName.size();dvm++) {
    TH1 *hDV=(TH1*) hFile->Get(Form("yieldDV%d",dvm)); assert(hDV);
    double yield=hDV->GetMean();
    dvYield.push_back(yield);
    dvYieldRms.push_back(hDV->GetRMS());
  }


  for(int iv=0;iv<ivName.size();iv++) {
    TH1 *hIV=(TH1*) hFile->Get(Form("yieldIV%d",iv)); assert(hIV);
    double yield=hIV->GetMean();
    ivYield.push_back(yield);
    ivYieldRms.push_back(hIV->GetRMS());
  }

  // .... asym for BCM's
  for(int j=0;j<mxBB;j++){  
    //printf(" j=%d name=%s\n", j,bbName[j].Data());
    TH1 *h=(TH1*) hFile->Get(bbName[j]); assert(h);
    bcmMean[j]=h->GetMean();
    bcmRms[j]=h->GetRMS();
  }
  }
  hFile->Close();
}
