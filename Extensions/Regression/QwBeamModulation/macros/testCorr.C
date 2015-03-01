{
#include<vector>
  gStyle->SetOptFit(1111);
  const int N = 1;
  Int_t nEvents  = 1000000;
  double a = 2, b = 0, sigma = 2, sigmax = 1, sigmay = 1, theta=3.14159/4.0;
  vector<double>X, Y;
  TRandom3 rand;
  TF1 *f = new TF1("f","pol1",-1,1);
  for(int i=0;i<nEvents;++i){
    if(i%10000==0)
      cout<<"Event "<<i<<endl;
    double x = rand.Gaus(a,sigmax);
    double y = rand.Gaus(b,sigmay);
    double gl = rand.Gaus(0,sigma);
    X.push_back(x+gl*cos(theta));
    Y.push_back(y+gl*sin(theta));
  }
  TCanvas *c = new TCanvas("c","c",0,0,1000,600);
  c->Divide(2,1);
  c->cd(1);
  TGraph *gr1 = new TGraph((int)X.size(),&X[0],&Y[0]);
  gr1->Draw("ap");
  gr1->Fit(f);
  c->cd(2);
//   TGraph *gr2 = new TGraph((int)targetX.size(),&det_tgX[0],&det_tgXP[0]);
//   gr2->Draw("ap");
//   gr2->Fit(f);
  cout<<"Prediction: "<<sigma*sigma*cos(theta)*sin(theta)/(sigmax*sigmax+sigma*sigma*cos(theta)*cos(theta))<<endl;

}
