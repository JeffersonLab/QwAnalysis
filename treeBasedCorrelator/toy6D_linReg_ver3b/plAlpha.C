plAlpha(int alf=0) {

  h=new TH1F(Form("alf%d",alf),Form("reconstructed alpha(%d); alpha_%d; data sets",alf,alf),64,0,0);
  readData(alf,h);
  //return;

  // c=new TCanvas; c->Divide(1,2);
  
  //h->Rebin(2);
  //h->Draw();
  // c->cd(2);
  h->Fit("gaus");
 double rms=h->GetRMS();
 printf("\nh=%s rms= %f\n",h->GetName(),rms);

  return;
  TF1* ff=h->GetFunction("gaus"); assert(ff);
  ff->SetLineColor(kRed);
  c->cd(1);
 h->Draw();
 // ff->Draw("same");
   ff->Draw("l same");
}

//---------------------------------
void readData(int alf,TH1F*h){
  int k=1;
  TString name=Form("outMC_1L/alpha_%d.dat",alf);
  //name="/u/home/balewski/PAN/regressionHallA/postpan/alpha_2.val";
  //diff_qwk_1i04X.hw_sum log-data-300x22 | cut -f5 -d\  >alpha_0.val
  FILE *fd=fopen(name.Data(),"r"); assert(fd);
  printf("fd=%p =%s=\n",fd,name.Data());
  while(1) {
    float hv;
    k++;
    int ret= fscanf(fd,"%f",&hv);
    if(ret!=1) break;
    if(k%50==0)
     printf("k=%d v=%f \n",k,hv);
    h->Fill(hv);
  }
  fclose(fd);
}
