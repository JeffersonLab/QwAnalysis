TCanvas *can=0;
int pl=2; //1=gif, 2=ps, 3=both

TFile* fd=0;
enum{ nP=5}; //,nY=15}; 
TString cor1="input";
TString runName="R7684.003";

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

  if(page==1) mySum("summary ,"+runName); 
  if(page==2) IV_IV("Independent variables, "+runName);
  if(page==3) yield_1D("Yield of IVs, "+runName,"yieldIV",0,4);

  if(page==10) DV_1D("Regressed DV: MD, "+runName,"regres",0,15);
  if(page==11) DV_1D("Regressed DV: Lumi coinc "+runName,"regres",16,28);
  if(page==12) DV_1D("Regressed DV: Lumi single "+runName,"regres",29,44);

  if(page==20) DV_1D("NOT regressed DV: MD, "+runName,"input",0,15);
  if(page==21) DV_1D("NOT regressed DV: Lumi coinc "+runName,"input",16,28);
  if(page==22) DV_1D("NOT regressed DV: Lumi single "+runName,"input",29,44);

 
  if(page==30) IV_DV("Correlation MD1..MD4 vs. IV's, "+runName,0,3);
  if(page==31) IV_DV("Correlation MD5..MD8 vs. IV's, "+runName,4,7);
  if(page==32) IV_DV("Correlation MDH,V,D1,D2 vs. IV's, "+runName,8,11);
  if(page==33) IV_DV("Correlation MDC,MDX, MDA vs. IV's, "+runName,12,14);
  if(page==34) IV_DV("Correlation Downstream Lumi combos vs. IV's,"+runName,15,17);
  if(page==35) IV_DV("Correlation Downstream Lumi pairs vs. IV's, "+runName,18,21);
  if(page==36) IV_DV("Correlation Upstream Lumi combos vs. IV's, "+runName,22,24);
  if(page==37) IV_DV("Correlation Upstream Lumi pairs vs. IV's, "+runName,25,28);
  if(page==38) IV_DV("Correlation Downstream Lumi single 1-4 vs. IV's, "+runName,29,32);
  if(page==39) IV_DV("Correlation Downstream Lumi single 5-8 vs. IV's, "+runName,33,36);
  if(page==40) IV_DV("Correlation Upstream Lumi ends 1,3 vs. IV's, "+runName,37,40);
  if(page==41) IV_DV("Correlation Upstream Lumi ends 5,7 vs. IV's, "+runName,41,44);


  if(page==50) yield_1D("Yield of DV: MD, "+runName,"yieldDV",0,15);
  if(page==51) yield_1D("Yield of DV: Lumi coinc "+runName,"yieldDV",16,28);
  if(page==52) yield_1D("Yield of DV: Lumi single "+runName,"yieldDV",29,44);

  can->Update();
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
void   mySum( TString text){

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
  h->GetXaxis()->SetNdivisions(6); 
  
 
  cD->cd(3);
  h=(TH1 *)fd->Get("inpDevErr"); assert(h);
  h->Draw();
  if(h->Integral()>20) gPad->SetLogy();

}

//============================================
//============================================
void   DV_1D(TString text, TString preFix, int i1, int i2){

  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",700,500);    TPad *c=makeTitle(can,text);
  c->Divide(4,4);
  //...... diagonal
  int k=1;
  for(int i=i1;i<=i2;i++) {    
    c->cd(k++);
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
    TText *tx=new TText(xm,ym/2.,Form("RMS=%.0f",w1)); tx->Draw();
    tx->SetTextSize(0.15);  tx->SetTextColor(45);
    tx=new TText(xm,ym,ax->GetTitle()+3); tx->Draw();
    tx->SetTextSize(0.15);  tx->SetTextColor(45);

  }

}



//============================================
//============================================
void   IV_DV( TString text, int iy1, int iy2){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  can=new TCanvas("aa","aa",700,600);    TPad *c=makeTitle(can,text);
  c->Divide(nP,4);
  // .... correlations 
  int k=1;
  for(int j=iy1;j<=iy2;j++) {      
    for(int i=0;i<nP;i++) {    
      c->cd(k);
      gPad->SetLeftMargin(0.15);

      TString name=Form("inputP%d_Y%d",i,j); cout<<k<<name.Data()<<endl;
      TH2F * h=(TH2F *)fd->Get(name); 
      if(h==0) continue;
      assert(h);
      h->Draw("colz");
      prX=h->ProfileX();

      trimDisplayRange(h,6.);
      prX->Draw("same");
      k++;
      h->GetXaxis()->SetLabelSize(0.06); 

      // return;
    }
  }
}



//============================================
//============================================
void   IV_IV(TString text){
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
      TH2F * h2=(TH2F *)fd->Get(name); assert(h);
      h2->Draw("colz");
      prX=h2->ProfileX();
      trimDisplayRange(h2,5.);
      prX->Draw("same");

      //  return;
    }
  }
}


//============================================
//============================================
void   yield_1D(TString text, TString preFix, int i1,int i2){

  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(1);

  can=new TCanvas("aa","aa",700,500);    TPad *c=makeTitle(can,text);
  c->Divide(4,4);
  int k=1;
  for(int i=i1;i<=i2;i++) {    
    c->cd(k++);
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
void  trimDisplayRange(TH1 *h, double fac=0.8) {
  // symetrize displayed axis
  
  TAxis *ax=h->GetXaxis();
  //      double ax1=fabs(ax->GetXmax()),ax2=fabs(ax->GetXmin());
  //  if(ax1>ax2) ax1=ax2;
  
  double basx=h->GetRMS()*fac;
  basx=ax->GetXmin();
  h->SetAxisRange(-basx,basx,"x");
  //  printf("%s rms=%f xbas=%f %d\n", h->GetName(), h->GetRMS(),basx,ax->GetNbins());
  
	   
  ax=h->GetYaxis();
  if(ax->GetNbins()<10) return -basx; // it was 1D histo
  
  double basy=h->GetRMS(2)*fac;
  h->SetAxisRange(-basy,basy,"y");
  //      double ay1=fabs(ax->GetXmax()),ay2=fabs(ax->GetXmin());
  //if(ay1>ay2) ay1=ay2;
  h->SetAxisRange(-basy,basy,"y");
  
  return basx;
}


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
   for(int i=1;i<=3;i++) plCor(i);
   for(int i=10;i<=12;i++) plCor(i);
   for(int i=20;i<=22;i++) plCor(i);
   for(int i=30;i<=41;i++) plCor(i);
   for(int i=50;i<=52;i++) plCor(i);

  cout<<"cat "+runName+"_page*ps  |ps2pdf - all"+runName+".pdf"<<endl;
  cout<<"scp -rp -i ~/balewski/keys/id_rsa-ifarml4 all"+runName+".pdf balewski@ifarml4:/group/qweak/www/html/onlineRegression/B"<<endl;
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
       
