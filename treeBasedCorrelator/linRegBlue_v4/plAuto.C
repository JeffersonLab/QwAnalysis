// convert  -rotate 90  -density 174  plots/wrk/QwOnlRegSum.ps  plots/wrk/QwOnlRegSum-r90n4.png

vector <TString> itemName;
enum {nY=15,nAuxGr=3};
TGraphErrors *grRegDvRms[nY];

TGraphErrors *grAux[nAuxGr];
double limRmsDV[2]={270,650};
TDatime myTime;
TString runRange="";
TString stringTime[2]={"bad T1","bad T2"};
TString stringDayOne;

UInt_t unixTimeOff=234470463-3*3600; // November 2010 
 unixTimeOff-=4*24*3600; // adjusted by hand to match root time to unix time
UInt_t backSec=5*12*3600;
double nomCharge=60 ; // mC

TString inpPath="web/";
TString outPath="plots/wrk/";
TString canCore="QwOnlReg";
int ps=2;

double par_minTotPatt=50000;
double par_minCurrent=100;  // uA

plAuto(int page=1, int psx=0, int toffX=0) {
  ps=psx;
  printf("toffX=%d\n", toffX);
  myTime.Print();
  const char * timeString=myTime.AsString();
  fprintf(stdout, "NOW time: TTime= %u , string=%s=\n",myTime.Get()-1291207091,timeString);
  //return;
  TString csvFile=inpPath+"mother_online_MD.csv";
  int r=readCsv(csvFile.Data(), toffX);
  printf("r=%d  nItemsNames=%d nRuns=%d\n",r,itemName.size(),grAux[0]->GetN());
  //return;
  if(page==1||page==0)  plotRms(grRegDvRms,1, "Regr RMS MD-combos (ppm)");
  if(page==2||page==0) plotRms(grRegDvRms,2, "Regr RMS MD-pairs (ppm)");
  if(page==3||page==0) plotRms(grRegDvRms,3, "Regr RMS MD-singles (ppm)");
  if(page==100||page==0) plotAux(100 ,"Auxiliary");
  if(page==0) {
    TString tit=outPath+canCore+"_"+stringDayOne+".pdf";
    FILE *fd=fopen(tit.Data(),"w");
    cout <<"write empty:"<<tit<<endl;
    fclose(fd);
  }
  return;

  // just dump of variable names
  for(int i=0;i<itemName.size(); i++) {
    printf("i=%d, item=%s=\n",i,itemName[i].Data());
  }
}

//--------------------------------------------------
int readCsv(char *fname, int toffX=0){
  FILE *fd=fopen(fname,"r"); assert(fd);
  /* return:    <0 : error in input     0 : EOF
     K>0: # of good records
  */

  const int mx=30000;
  char buf[mx];
  int k=0;
  double runSeg=99;
  char stringDay[100],stringHour[100];

  while(true) {
    char * ret=fgets(buf,mx,fd);
    if(ret==0) break;
    if(buf[0]=='#') continue;
    char name0[mx];
    int ret1=sscanf(buf,"%s,",name0);
    //printf("k=%d name0=%s=\n",k,name0);
    k++;
    //  buf[300]=0; printf("buf=%s=\n",buf);

    if(k==1) { getNames(buf); continue; }
    double totCharge,avrCur,totPatt;
    int unixTime;
    int n=sscanf(buf,"%lf, %lf, %lf, %lf, %d , %s %s",&runSeg,&totCharge,&avrCur,&totPatt,&unixTime, stringDay, stringHour); //printf("n=%d\n",n); 
    assert(n==7);
    int delTimeSec=myTime.Get()-unixTime+unixTimeOff;
    //printf("k=%d R%.3f   uT=%d sT=%s %s  delSec=%d\n", k,runSeg,unixTime, stringDay, stringHour, delTimeSec);
    if(delTimeSec> backSec+toffX) continue; 
    // if(delTimeSec<toffX) break; // upper time limit
    if(totPatt<par_minTotPatt) continue;
    if(avrCur<par_minCurrent) continue;
    //if(runSeg<7110) continue; if(runSeg>7135.99) break; // best day: Nov-15

    if(runRange.Sizeof()<3) {
      runRange=Form("R%.0f-",runSeg);
      stringDayOne=stringDay;
      stringHour[5]=0;
      stringTime[0]=Form("%s  %s", stringDay, stringHour);
    }
    double runBin=runBin(runSeg);    
    double chargeFact=sqrt(nomCharge/totCharge);
    //printf("n=%d R%.3f Rbin=%.2f %f %f %3g  uT=%d sT=%s %s  delSec=%d\n", n,runSeg,runBin,totCharge,avrCur,totPatt,unixTime, stringDay, stringHour, delTimeSec);
    getRegDvRms(runBin,chargeFact, totPatt,buf,17);

    int n=grAux[0]->GetN();
    grAux[0]->SetPoint(n,runBin,totCharge);
    grAux[1]->SetPoint(n,runBin,avrCur);
    grAux[2]->SetPoint(n,runBin,totPatt);
    // if(k>=40)break;
  }
  fclose(fd);
  // printf("zzz %f\n",runSeg);
  runRange+=Form("R%.0f",runSeg);
  stringHour[5]=0;
  stringTime[1]=Form("%s  %s", stringDay, stringHour);

  for(int i=0;i<2;i++){
    stringTime[i].ReplaceAll("2010-11-","Nov-");
    stringTime[i].ReplaceAll("2010-12-","Dec-");
    stringTime[i].ReplaceAll("2011-01-","Jan-");
  }
  return k;
}


//---------------------
void getNames(char *buf) {
  int i=0;
  char *item=strtok(buf,","); // init 'strtok'
  do {
    //printf("i=%d, item=%s=\n",i++,item);
    itemName.push_back(item);
  } while (item=strtok(0,","));  // advance by one name
  initGraphs();
}


//---------------------
void getRegDvRms(double runBin,double chargeFact, int nPatt, char *buf,int ioff) {// works on clone
  TString buf1=buf;  
  int i=0;
  char *item=strtok(buf1.Data(),","); // init 'strtok'
  //printf("xx =%s=\n",item);
  for(i=0;i<ioff;i++) item=strtok(0,",");
  for(int k=0; k<nY; k++, i++, item=strtok(0,",")) {
    // printf("i=%d, name=%s item=%s=\n",i,itemName[i].Data(),item);
    double val;
    assert(sscanf(item,"%lf",&val)==1);
    double rms=val;
    double err=rms/sqrt(nPatt);
    // if(k==0) printf(" rms=%f err=%f\n",rms,err);

    TGraphErrors* gr=grRegDvRms[k];
    int n=gr->GetN();
    gr->SetPoint(n,runBin,rms);
    gr->SetPointError(n,0, err);

  } while ();  // advance by one name
  
}

//-----------------------------
void initGraphs() {

  for(int i=0;i<nAuxGr;i++) {
    TGraphErrors* gr=new TGraphErrors;
    gr->SetMarkerStyle(27+i); // 24=circ, 21=full-sq, 27=diam
    gr->SetMarkerSize(1.); 
    switch(i) {
    case 0:
      gr->SetTitle("total charge (mC)");gr->SetMarkerColor(kRed); break;
    case 1:
      gr->SetTitle("avr current (#mu A)");gr->SetMarkerColor(kBlue); break;
    case 2:
      gr->SetTitle("used patterns (~4 mSec)");gr->SetMarkerColor(8); break;
    }
    grAux[i]=gr;
  }

  //...... DV RMS, reg
  int ioff=17;
  for(int k=0;k<nY;k++) { 
    TGraphErrors* gr=new TGraphErrors;
    gr->SetMarkerStyle(24); // 24=circ, 21=full-sq
    gr->SetMarkerSize(0.8);
    // gr->SetMarkerColor(44+k);
    int j=ioff+k;
    printf("k=%d name=%s p=%p\n",k,itemName[j].Data(),gr);
    TString tit=itemName[j];
    gr->SetTitle(tit);
 
    grRegDvRms[k]=gr;
  }

}

 //---------------------
double runBin(double runSeg) {
  int run=int(runSeg);
  double eps=100*(runSeg-run);
  if(eps>0.9) eps=0.9;
  return run+eps;
}



//------------ PLOTTER
//------------ PLOTTER
//------------ PLOTTER
void plotRms(TGraphErrors **grA, int page,TString title) {
  printf("plotRms page=%d\n",page);
  TString padTit=title+", runs "+runRange+" ";
  can=new TCanvas("aa","aa",900,650);    TPad *c0=makeTitle(can,padTit,page);
  double  yDw=limRmsDV[0];
  double  yUp=limRmsDV[1];

  if(page==3) { // 8 MDs
    c0->Divide(3,3);
    int padA[nY+1]={4,1,2,3,6,9,8,7,5};
    for(int k=0;k<8;k++) {
      c0->cd(padA[k]);  
      gPad->SetLeftMargin(0.15);
      TGraphErrors *gr=grA[k];
      // printf("k=%d p=%p\n",k,gr);
      //     gr->Print();
      gr->Draw("AP");
      niceIt(gr,yDw*1.5,yUp*1.5);
    }
    c0->cd(5);
    ltx=new TLatex( 0.1,0.6,"RMS  = RMS_{ HISTO}  ");    ltx->Draw();
    ltx=new TLatex( 0.1,0.4,"#sigma(RMS_{ SCALED} ) = RMS  /       #sqrt{ N patterns}");  ltx->Draw();

  } // page 3 end

  
  if(page==2) { // 4 MD-pairs
    c0->Divide(2,2);
    for(int k=0;k<4;k++) {
      c0->cd(1+k);  
      TGraphErrors *gr=grA[8+k];
      gr->Draw("AP");
      niceIt(gr,yDw*1.2,yUp*1.2);
    }
  } // page 2 end


  if(page==1) { // 3 MD-combos
    c0->Divide(2,2);
    for(int k=0;k<3;k++) {
      c0->cd(4-k);  
      gPad->SetLeftMargin(0.15);
      TGraphErrors *gr=grA[12+k];
      gr->Draw("AP");
      if(k<2) niceIt(gr,yDw,yUp);
      else niceIt(gr,yDw,yUp*0.6);
    }
    
    c0->cd(1);  
    gPad->SetLeftMargin(0.15);
    TGraphErrors *gr=grAux[1];
    gr->Draw("AP");
    niceIt(gr,130,160);
  } // page 1 end

  // final printing
  TString tit=canCore+Form("%03d",page);
  can->SetTitle(tit);   can->SetName(tit);
  if(ps&2) can->Print(outPath+tit+".ps");

}

void plotAux( int page,TString title) {
  printf("plotAux page=%d\n",page);
  TString padTit=title+", runs "+runRange+" ";
  can=new TCanvas("aa","aa",900,650);    TPad *c0=makeTitle(can,padTit,page);
  c0->Divide(1,3);
  for(int k=0;k<nAuxGr;k++) {
    c0->cd(k+1);  
    gPad->SetLeftMargin(0.05);
    TGraphErrors *gr=grAux[k];
    gr->Draw("AP");
    niceIt(gr,0 ,0);
  }
  TString tit=canCore+Form("%03d",page);
  can->SetTitle(tit);   can->SetName(tit);
  if(ps&2) can->Print(outPath+tit+".ps");
}

//------------------
void drawTimeLabel(double t, double y, TString full) {
  tx=new TLatex(t,y,full);
  tx->SetTextAngle(90); tx->Draw();
  tx->SetTextColor(kMagenta);
}


//-----------------
void niceIt(   TGraphErrors* gr, double yDw, double yUp){
  assert(gr);
  gPad->SetGrid();
  double ym=yDw*.4+yUp*0.6;
  TH1*h =gr->GetHistogram();     assert(h);
  if(yDw<yUp) { h->SetMinimum(yDw);  h->SetMaximum(yUp);}
  else ym=h->GetMinimum();

  double t0=gr->GetX()[0];
  // printf("t0=%f\n",t0);
  
  drawTimeLabel(t0-0.5,ym,stringTime[0]);
  
  double t1=gr->GetX()[gr->GetN()-1];
  //printf("t1=%f\n",t1);
  drawTimeLabel(t1+1.5,ym,stringTime[1]);
  
  //X-axis ....
  TAxis *ax=h->GetXaxis();    ax->SetTitle("run number     ");
  ax->SetNdivisions(6);  ax->SetLabelSize(0.06);
  //Yaxis ....
  ax=h->GetYaxis();
  ax->SetNdivisions(5);  ax->SetLabelSize(0.06);

}

//------------------------
TPad *makeTitle(TCanvas *c,char *core, int page) {
  c->Range(0,0,1,1);
  TPad *pad0 = new TPad("pad0", "apd0",0.0,0.95,1.,1.);
  pad0->Draw();
  pad0->cd();

  TPaveText *pt = new TPaveText(0,0.,1,1,"br");
  pt->Draw();
  TDatime dt;
  TString txt2=core;
  txt2+=", page=";
  txt2+=page;
  txt2+=",  ";
  txt2+=dt.AsString();
  pt->AddText(txt2);
  txt2="--";
  pt->AddText(txt2);

  c->cd();
  pad = new TPad("pad1", "apd1",0.0,0.0,1,.95);
  pad->Draw();
  return pad;
} 


//------------------------
void splitPadX(float x, TPad **cL, TPad **cR) {
  (*cL) = new TPad("padL", "apdL",0.0,0.,x,0.95);
  (*cL)->Draw();
  (*cR) = new TPad("padL", "apdL",x+0.005,0.,1.0,0.95);
  (*cR)->Draw();
}

//------------------------
void splitPadY(float y, TPad **cU, TPad **cD) {
  (*cU) = new TPad("padD", "apdD",0,y+0.005,1.0,1.);
  (*cU)->Draw();
  (*cD) = new TPad("padU", "apdU",0.0,0.,1.,y);
  (*cD)->Draw();

  /* use case:    
     TPad *cU,*cD;   splitPadY(0.4,&cU,&cD);    cU->cd(); h->Draw()   
  */
}



