TCanvas *can=0;
int pl=2; //1=gif, 2=ps, 3=both

TFile* fd=0;
enum{ nP=5,nY=15}; 
TString cor1="input";
TString runName="R5827.000";

//TString inpPath="./web/R5822.001/";
TString inpPath="./out/";
char *oPath="./out/";

plCor(int page=1, char *runName0="RfixMe.000") {
  //  printf("ss=%s=\n",runName0);
 
  if(page==0) {   runName=runName0;  doAll();    return;   }
  gStyle->SetFillStyle(0);
  gStyle->SetPalette(1,0);
  TString dataFinalRoot=inpPath+"blue"+runName+".hist.root";

  fd=new TFile( dataFinalRoot); assert(fd->IsOpen());
  printf("Opened  histo=%s=\n",fd->GetName());

  if(page==1) mySum("mySummary","summary +QA ,"+runName); // just one set, old
  if(page==2) DV_1D("DV_1Da","regressed DV,"+runName,"regres"); // just one set, old
  if(page==3) DV_1D("DV_1Db","NOT regressed DV, "+runName,"input"); // just one set, old

 
  if(page==4) IV_DV("IV_DV","Correlation MD1..MD4 vs. IV's, "+runName,0,3);
  if(page==5) IV_DV("IV_DV","Correlation MD5..MD8 vs. IV's, "+runName,4,7);
  if(page==6) IV_DV("IV_DV","Correlation MDH,V,D1,D2 vs. IV's, "+runName,8,11);
  if(page==7) IV_DV("IV_DV","Correlation MDC,MDX, MDA vs. IV's, "+runName,12,14);

  if(page==8) IV_IV("IV","Independent variables, "+runName);
  if(page==9) yield_1D("DVyield","Yield of DVs, "+runName,"yieldDV",nY);
  if(page==10) yield_1D("IVyield","Yield of IVs, "+runName,"yieldIV",nP);

 


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

  gStyle->SetOptFit(1); gStyle->SetOptStat(10);
  
  can=new TCanvas("aa","aa",800,600);    TPad *c=makeTitle(can,text);
  c->cd();
  TPad *cU,*cD;   splitPadY(0.55,&cU,&cD);    

  cD->Divide(3,1);

  cU->cd(0);
  h=(TH1 *)fd->Get("inpPattNo"); assert(h);
  TAxis *ax=h->GetXaxis();
  int nb=ax->GetNbins();
  for(int j=nb; j>1; j--) {
    if(h->GetBinContent(j) <=0) continue;
    h->SetAxisRange(0., ax->GetBinCenter(j)*1.05);
    printf("max j=%d of %d, eventID=%.1f\n", j,nb,ax->GetBinCenter(j));
    break;
  }
  double yMax=h->GetMaximum();
  h->SetMaximum(yMax*1.02);
  h->SetMinimum(yMax*0.90);

  h->Draw();
  tx=new TText(ax->GetBinCenter(2), h->GetMaximum(),"     run="+runName); tx->Draw();
  tx->SetTextSize(0.12);


  cD->cd(1);
  h=(TH1 *)fd->Get("myStat"); assert(h);
  h->Draw("h text");
  h->SetMaximum(h->GetMaximum()*1.2);

  cD->cd(2);
  h=(TH1 *)fd->Get("inpBcm1"); assert(h);
  
  h->Draw();
  
 
  cD->cd(3);
  h=(TH1 *)fd->Get("inpDevErr"); assert(h);
  h->Draw();
  if(h->Integral()>20) gPad->SetLogy();
 

}


//============================================
//============================================
void   IV_IV(TString cCore, TString text){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",800,700);    TPad *c=makeTitle(can,text);

  c->Divide(nP,nP);

  //...... diagonal
  for(int i=0;i<nP;i++) {    
    c->cd(1+i*nP+i);
    TString name=Form("inputP%d",i); // cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); 
    if(h==0) continue;
    assert(h);
    h->Draw();
    double xm=-trimDisplayRange(h,5.0);
    TAxis *ax=h->GetXaxis();
    double w1=h->GetRMS();
    double ym=h->GetMaximum()*0.45;
    TText *tx=new TText(xm,ym/2.,Form("RMS=%.0f",w1)); tx->Draw();
    tx->SetTextSize(0.15);
    tx=new TText(xm,ym,ax->GetTitle()+3); tx->Draw();
    tx->SetTextSize(0.14);  tx->SetTextColor(kMagenta);
  }
  
  // .... correlations 
  for(int i=0;i<nP;i++) {    
    for(int j=i+1;j<nP;j++) {      
      c->cd(1+i*nP+j);
      TString name=Form("inputP%d_P%d",i,j); // cout<<name.Data()<<endl;
      TH1 * h=(TH1 *)fd->Get(name); assert(h);
      h->Draw("colz");
      trimDisplayRange(h,5.);
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
    TH1 * h=(TH1 *)fd->Get(name);
    if(h==0) continue;
    assert(h);
    h->Draw();

    double xm=-trimDisplayRange(h,6.0)*.8;

    TAxis *ax=h->GetXaxis();
    ax->SetLabelSize(0.10); 

    double w1=h->GetRMS();
    double ym=h->GetMaximum()*0.75;
    TText *tx=new TText(xm,ym/2.g,Form("RMS=%.0f",w1)); tx->Draw();
    tx->SetTextSize(0.15);  tx->SetTextColor(45);
    tx=new TText(xm,ym,ax->GetTitle()+3); tx->Draw();
    tx->SetTextSize(0.15);  tx->SetTextColor(45);

  }



}

//============================================
//============================================
void   yield_1D(TString cCore,TString text, TString preFix, int np){

  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(1);

  can=new TCanvas("aa","aa",700,500);    TPad *c=makeTitle(can,text);
  c->Divide(4,4);
  //...... diagonal
  for(int i=0;i<np;i++) {    
    c->cd(1+i);
    TString name=preFix+Form("%d",i); cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name);
    if(h==0) continue;
    assert(h);
    h->Draw();
    TAxis *ax=h->GetXaxis();
    ax->SetLabelSize(0.06); 
    double xm=ax->GetBinCenter(1);
    double ym=h->GetMaximum()*0.75;
    tx=new TText(xm,ym,ax->GetTitle()); tx->Draw();
    tx->SetTextSize(0.15);  tx->SetTextColor(45);

  }

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
      TH1 * h=(TH1 *)fd->Get(name); 
      if(h==0) continue;
      assert(h);
      h->Draw("colz");
      trimDisplayRange(h,6.);
      k++;
      h->GetXaxis()->SetLabelSize(0.06); 

      //break;
    }
  }
}

//============================================
//============================================
void  trimDisplayRange(TH1 *h, double fac=0.8) {
  // symetrize displayed axis
  
  TAxis *ax=h->GetXaxis();
  //      double ax1=fabs(ax->GetXmax()),ax2=fabs(ax->GetXmin());
  //  if(ax1>ax2) ax1=ax2;
  
  double basx=h->GetRMS()*fac;
  basx=ax->GetXmin();
  h->SetAxisRange(-basx,basx,"x");
  printf("%s rms=%f xbas=%f %d\n", h->GetName(), h->GetRMS(),basx,ax->GetNbins());
  
	   
  ax=h->GetYaxis();
  if(ax->GetNbins()<10) return -basx; // it was 1D histo
  
  double basy=h->GetRMS(2)*fac;
  h->SetAxisRange(-basy,basy,"y");
  //      double ay1=fabs(ax->GetXmax()),ay2=fabs(ax->GetXmin());
  //if(ay1>ay2) ay1=ay2;
  h->SetAxisRange(-basy,basy,"y");
  
  return basx;
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
  for(int i=1;i<=10;i++)  {
    // if(i==6||i==7) continue; //tmp, if no 2,4,8-sums
    plCor(i);
  }

  cout<<"cat "+runName+"_page*ps  |ps2pdf - all"+runName+".pdf"<<endl;
  cout<<"scp all"+runName+".pdf  balewski@deltag5.lns.mit.edu:0x"<<endl;
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
       
