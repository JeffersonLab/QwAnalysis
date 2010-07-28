TString inrPath="hel_histo/"; // inside root file
TString outPath="out/";
vector<TString> nameL;
vector<TString> hNameL;
TFile *fd=0;

//  cat *ps |ps2pdf - all.pdf
//  scp all.pdf balewski@deltag5.lns.mit.edu:0x
//back:  scp deltag5.lns.mit.edu:0x/all.pdf .

void plCorrMon(  TString monCore="A", TString fileCore="591.000") {
  fd=new TFile("$QWSCRATCH/rootfiles/Qweak_"+fileCore+".root");
  fd->ls();

  recoverHnames(monCore);

  int ih=0;
  while(ih>=0) 
    plotSingPages(ih, monCore);
  ih=0;
  while(ih>=0)  
    plotCorrPages(ih, monCore);
  
}


//==================================================
//==================================================
void  plotCorrPages(int &ih,TString monCore){
  if(ih>=hNameL.size()) { ih=-1; return;}
   TString core=Form("qw%s_2dCorr_page%03d",monCore.Data(),ih);
  
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(1111110);
  c=new TCanvas();
  int mx=6, nx=3;
  c->Divide(nx,mx/nx);
  for(int i=0; i<mx;i++){
    int jh=ih+i;
    if(jh>=hNameL.size()) break;
    printf(" plot histo %d, =%s=\n", i,hNameL[jh].Data());
    TH1 *h=fd->Get(inrPath+monCore+hNameL[jh]); assert(h);
    c->cd(i+1);
    h->Draw("colz");
  }
  if(i<mx) ih=-1;// end of list
  else  ih+=i;
  
  printf(" end of this page, ih=%d\n",ih);

   c->SetName(core);  c->SetTitle(core);
  c->Print(outPath+core+".ps");


}
  

//==================================================
//==================================================
void  plotSingPages(int &ih,TString monCore){
  if(ih>=hNameL.size()) { ih=-1; return;}
  TString core=Form("qw%s_1dCorr_page%03d",monCore.Data(),ih);
  
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(1111110);
  c=new TCanvas();
  int mx=6, nx=2;
  c->Divide(nx,mx/nx);
  for(int i=0; i<mx;i++){
    int jh=ih+i;
    if(jh>=nameL.size()) break;
    TString name=nameL[jh]+"_hw_single";
    printf(" plot histo %d, =%s=\n", i,name.Data());
    TH1 *h=fd->Get(inrPath+monCore+name); assert(h);
    c->cd(i+1);
    h->Draw();
    gPad->SetLogy();
  }
  if(i<mx) ih=-1;// end of list
  else  ih+=i;
  
  printf(" end of this page, ih=%d\n",ih);

  c->SetName(core);  c->SetTitle(core);
  c->Print(outPath+core+".ps");


}
  
//==================================================
//==================================================
void recoverHnames(TString monCore){
  TH1 *h0=fd->Get(inrPath+monCore+"jan_list"); assert(h0);
  //  h0->Print();
  TString allNames=h0->GetTitle();
  printf(" all names=%s=\n", allNames.Data());
  TObjArray* sa	=allNames.Tokenize(" ");
  TIter it(sa);TObject *ob;
  
  while ((ob = it.Next())) {
    TString name=(ob->GetName());
    printf("s=%s=\n",name.Data());
    nameL.push_back(name);
  }
  
  printf(" variable list len=%d\n", nameL.size());
  TString hnameSufix="_hw_corr";
   
  for(int i=0;i<nameL.size();i++)
    for(int j=i+1;j<nameL.size();j++)
      hNameL.push_back(nameL[i]+"_"+nameL[j]+hnameSufix);
  
  printf(" 2D histo list len=%d, =%s=\n", hNameL.size(),hNameL[0].Data());
}
