TCanvas *can=0;


TString inpPath="./";
TFile* fd=0;
enum{ nP=4,nY=3}; 
TString corName="Acor";

plCor(int page=1) {
  if(page==0) {  doAll();    return;   }

  gStyle->SetFillStyle(0);
  gStyle->SetPalette(1,0);
  TString dataFinalRoot=inpPath+"corKent_5820.000.hist.root";
  char *oPath="./";
  int pl=2; //1=gif, 2=ps, 3=both

  fd=new TFile( dataFinalRoot); assert(fd->IsOpen());

  if(page==1) DV_CMP("DV_comparison","DV's before & after regression");  
  if(page==2) IV_DV("IV_DV","Correlation DV's vs. IV's ");
  if(page==3) IVs("IV","Independent variables");

  if(page==30) DV_1D("DV_1D","dv"); // just one set, old

  if(can) {
    TString tit=Form("%scorAna_page%03d",oPath,page);
    can->SetTitle(tit);
    can->SetName(tit);    
    if(pl&1) can->Print(tit+".gif");
    if(pl&2) can->Print(tit+".ps");
  }

}

//============================================
//============================================
void   IVs(TString cCore, TString text){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",700,700);    TPad *c=makeTitle(can,text);

  c->Divide(nP,nP);

  //...... diagonal
  for(int i=0;i<nP;i++) {    
    c->cd(1+i*nP+i);
    TString name=corName+Form("P%d",i); // cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); assert(h);
    h->Draw();
  }

  // .... correlations 
  for(int i=0;i<nP;i++) {    
    for(int j=i+1;j<nP;j++) {      
      c->cd(1+i*nP+j);
      TString name=corName+Form("P%d_P%d",i,j); // cout<<name.Data()<<endl;
      TH1 * h=(TH1 *)fd->Get(name); assert(h);
      h->Draw("colz");
    }
  }
}

//============================================
//============================================
void   DV_1D(TString cCore,TString text){

  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  //c=new TCanvas(cCore,cCore,700,700); c->SetFillColor(kWhite);
  can=new TCanvas("aa","aa",500,500);    TPad *c=makeTitle(can,text);
  
  c->Divide(nY/2+1,2);

  //...... diagonal
  for(int i=0;i<nY;i++) {    
    c->cd(1+i);
    TString name=corName+Form("Y%d",i); // cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); assert(h);
    h->Draw();
    //h->Fit("gaus");
  }
}

//============================================
//============================================
void   IV_DV(TString cCore, TString text){
  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",700,600);    TPad *c=makeTitle(can,text);
 
  c->Divide(nP,nY);

 
  // .... correlations 
  for(int i=0;i<nP;i++) {    
    for(int j=0;j<nY;j++) {      
      c->cd(1+i*nY+j);
      TString name=corName+Form("P%d_Y%d",i,j); // cout<<name.Data()<<endl;
      TH1 * h=(TH1 *)fd->Get(name); assert(h);
      h->Draw("colz");
    }
  }
}


//============================================
//============================================
void   DV_CMP(TString cCore, TString text){

  gStyle->SetOptStat(1001110);
  gStyle->SetOptFit(1);
  
  can=new TCanvas("aa","aa",800,500);    TPad *c=makeTitle(can,text);
  
  c->Divide(nY,2);

  //...... diagonal
  for(int i=0;i<nY;i++) {    
    c->cd(1+i);
    TString name=Form("AcorY%d",i);  cout<<name.Data()<<endl;
    TH1 * h=(TH1 *)fd->Get(name); assert(h);
    h->Draw();
    double w1=h->GetRMS();
    double ym=h->GetMaximum()*0.25;
    double xm=-600;
    TText *tx=new TText(xm,ym,Form("RMS=%.0f",w1)); tx->Draw();

    c->cd(1+nY+i);
    name=Form("BcorY%d",i);  cout<<name.Data()<<endl;
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
  for(int i=1;i<=3;i++)  {
    plCor(i);
  }
}
