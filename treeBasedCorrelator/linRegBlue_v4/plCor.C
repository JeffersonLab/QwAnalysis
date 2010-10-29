TCanvas *can=0;

TFile* fd=0;
enum{ nP=5,nY=15}; 
TString cor1="input";
int pl=2; //1=gif, 2=ps, 3=both
TString runName="R5843.000";
TString inpPath="./out/";
char *oPath="./out/";

plCor(int page=1, char *runName0="RfixMe.000") {
  //  printf("ss=%s=\n",runName0);

  if(page==0) {   runName=runName0; doAll();    return;   }
  gStyle->SetFillStyle(0);
  gStyle->SetPalette(1,0);
  TString dataFinalRoot=inpPath+runName+".hist.root";

  fd=new TFile( dataFinalRoot); assert(fd->IsOpen());

  if(page==1) mySum("mySummary","summary +QA ,"+runName); // just one set, old
  if(page==2) DV_1D("DV_1Da","regressed DV,"+runName,"regres"); // just one set, old
  if(page==3) DV_1D("DV_1Db","NOT regressed DV, "+runName,"input"); // just one set, old

 
  if(page==4) IV_DV("IV_DV","Correlation MD1..MD4 vs. IV's, "+runName,0,3);
  if(page==5) IV_DV("IV_DV","Correlation MD5..MD8 vs. IV's, "+runName,4,7);
  if(page==6) IV_DV("IV_DV","Correlation MDH,V,D1,D2 vs. IV's, "+runName,8,11);
  if(page==7) IV_DV("IV_DV","Correlation MDC,MDX, MDA vs. IV's, "+runName,12,14);

  if(page==8) IV_IV("IV","Independent variables, "+runName);

  if(page==100) DV_CMP("DV_comparison","DV's before & after regression"); 


  if(can) {
    TString tit=Form("%s%s_page%03d",oPath,runName.Data(),page);
    can->SetTitle(tit);
    can->SetName(tit);    
    if(pl&1) can->Print(tit+".gif");
    if(pl&2) can->Print(tit+".ps");
    // if(page==1) can->Print(tit+".png");
  }

}

//============================================
//============================================
void   mySum(TString cCore, TString text){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",700,700);    TPad *c=makeTitle(can,text);

  c->Divide(1,2);
  //  fd->ls();
  c->cd(1);
  TH1 * h=(TH1 *)fd->Get("myStat"); assert(h);
  h->Draw();


  c->cd(2);
  h=(TH1 *)fd->Get("inpPattNo"); assert(h);
  h->Draw();
}


//============================================
//============================================
void   IV_IV(TString cCore, TString text){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",700,700);    TPad *c=makeTitle(can,text);

  c->Divide(nP,nP);

  //...... diagonal
  for(int i=0;i<nP;i++) {    
    c->cd(1+i*nP+i);
    TString name=Form("inputP%d",i); // cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); assert(h);
    h->Draw();
    trimDisplayRange(h,1.0);
    TAxis *ax=h->GetXaxis();
    double w1=h->GetRMS();
    double ym=h->GetMaximum()*0.45;
    double xm=ax->GetXmin()*0.6 + ax->GetXmax()*0.4;
    TText *tx=new TText(xm,ym/2.,Form("RMS=%.0f",w1)); tx->Draw();
    tx->SetTextSize(0.15);
    tx=new TText(xm,ym,ax->GetTitle()); tx->Draw();
    tx->SetTextSize(0.15);
  }
  
  // .... correlations 
  for(int i=0;i<nP;i++) {    
    for(int j=i+1;j<nP;j++) {      
      c->cd(1+i*nP+j);
      TString name=Form("inputP%d_P%d",i,j); // cout<<name.Data()<<endl;
      TH1 * h=(TH1 *)fd->Get(name); assert(h);
      h->Draw("colz");
      trimDisplayRange(h,0.8);
      //  return;
    }
  }
}

//============================================
//============================================
void   DV_1D(TString cCore,TString text, TString preFix){

  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",700,500);    TPad *c=makeTitle(can,text);
  c->Divide(4,4);
  //...... diagonal
  for(int i=0;i<nY;i++) {    
    c->cd(1+i);
    TString name=preFix+Form("Y%d",i); cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); assert(h);
    h->Draw();
    trimDisplayRange(h,1.0);

    TAxis *ax=h->GetXaxis();
    double w1=h->GetRMS();
    double ym=h->GetMaximum()*0.45;
    double xm=ax->GetXmin()*0.6 + ax->GetXmax()*0.4;
    TText *tx=new TText(xm,ym/2.,Form("RMS=%.0f",w1)); tx->Draw();
    tx->SetTextSize(0.15);
    tx=new TText(xm,ym,ax->GetTitle()); tx->Draw();
    tx->SetTextSize(0.15);

  }


    TH1 * h=(TH1 *)fd->Get("inpBcm1"); assert(h);
    c->cd(16);
    h->Draw();

}

//============================================
//============================================
void   IV_DV(TString cCore, TString text, int iy1, int iy2){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  can=new TCanvas("aa","aa",700,600);    TPad *c=makeTitle(can,text);
  c->Divide(nP,4);
  // .... correlations 
  int k=1;
  for(int j=iy1;j<=iy2;j++) {      
    for(int i=0;i<nP;i++) {    
      c->cd(k);
      TString name=Form("inputP%d_Y%d",i,j); cout<<k<<name.Data()<<endl;
      TH1 * h=(TH1 *)fd->Get(name); assert(h);
      h->Draw("colz");
      trimDisplayRange(h,0.8);
      k++;
    }
  }
}

//============================================
//============================================
void  trimDisplayRange(TH1 *h, double fac=0.8) {
      // symetrize displayed axis
     
      TAxis *ax=h->GetXaxis();
      double ax1=fabs(ax->GetXmax()),ax2=fabs(ax->GetXmin());
      if(ax1>ax2) ax1=ax2;
      h->SetAxisRange(-ax1*fac,ax1*fac,"x");
      ax=h->GetYaxis();
      if(ax->GetNbins()<10) return; // it was 1D histo
      double ay1=fabs(ax->GetXmax()),ay2=fabs(ax->GetXmin());
      if(ay1>ay2) ay1=ay2;
      h->SetAxisRange(-ay1*fac,ay1*fac,"y");
}


//============================================
//============================================
void   DV_CMP(TString cCore, TString text){

  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",800,500);    TPad *c=makeTitle(can,text);
  
  c->Divide(4,4);

  //...... diagonal
  for(int i=0;i<nY;i++) {    
    c->cd(1+i);
    TString name=cor1+Form("Y%d",i);  cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); assert(h);
    h->Draw();
    double w1=h->GetRMS();
    double ym=h->GetMaximum()*0.25;
    double xm=-600;
    TText *tx=new TText(xm,ym,Form("RMS=%.0f",w1)); tx->Draw();

    c->cd(1+nY+i);
    name=Form("regresY%d",i);  cout<<name.Data()<<endl;
    h=(TH1 *)fd->Get(name); 
    if(h==0) continue;
    //assert(h);
    h->Draw();
    double w2=h->GetRMS();
    TText *tx2=new TText(xm,ym,Form("RMS=%.0f",w2)); tx2->Draw();
    double del=sqrt(w1*w1-w2*w2);
    printf("%f %f %f\n", w1,w2,del);
    TText *tx2=new TText(xm,ym/2,Form("DEL=%.0f",del)); tx2->Draw();
    //    break;
  }
}



// c->SetFillColor(kWhite);
  //  gStyle->SetOptStat(1001110);
  //  gStyle->SetOptFit(1);
  

    //    TF1 *ff=h->GetFunction("gaus");
    // ff->SetLineColor(kRed);    ff->SetLineWidth(1);

//------------------------
TPad *makeTitle(TCanvas *c,char *core) {
  c->Range(0,0,1,1);
  TPad *pad0 = new TPad("pad0", "apd0",0.0,0.95,1.,1.);
  pad0->Draw();
  pad0->cd();

  TPaveText *pt = new TPaveText(0,0.,1,1,"br");
  pt->Draw();
  TDatime dt;
  TString txt2=core;
  txt2+=",  ";
  txt2+=dt.AsString();  pt->AddText(txt2);
  txt2="--";
  pt->AddText(txt2);

  c->cd();
  pad = new TPad("pad1", "apd1",0.0,0.0,1,.95);
  pad->Draw();
  return pad;
}

//============================
void doAll(){
  for(int i=1;i<=8;i++)  {
    plCor(i);
  }

  cout<<"cat "+runName+"_page*ps  |ps2pdf - all"+runName+".pdf"<<endl;
  cout<<"scp all"+runName+".pdf  balewski@deltag5.lns.mit.edu:0x"<<endl;
}
