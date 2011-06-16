{

gROOT->Reset();
gROOT->LoadMacro("macro/open.macro");

// define style here 
// general parameters
gStyle->SetOptDate(0);     

gStyle->SetOptTitle(0);
gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
gStyle->SetPadLeftMargin(0.15);  
gStyle->SetPadRightMargin(0.08); 

// canvas parameters
gStyle->SetFrameBorderMode(0);
gStyle->SetFrameBorderSize(0);
gStyle->SetFrameFillColor(10);
// pads parameters
gStyle->SetPadColor(0); 
gStyle->SetPadBorderMode(0);
gStyle->SetPadBorderSize(0);
gROOT->ForceStyle();

//

Int_t ntot,Npair;
float Mean,Sigave,Rms;
char  Npairstg[50],Meanstg[50],Sigavestg[50],Rmstg[50];
char *Npairp = Npairstg;
char *Meanp = Meanstg;
char *Rmsp = Rmstg;

Int_t run = open(0,"beam");    

Int_t ibcm1;
cout << "Enter ibcm1 : ";
cin >> ibcm1;
cout << "ibcm1 = "<< ibcm1 <<endl;

Int_t OS;
cout << "Enter OS : ";
cin >> OS;
cout << "OS = "<< OS <<endl;

Int_t iscal;
iscal = 120;

char runfile[50];
char psfile[50];

sprintf(runfile,"/adaqfs/halla/apar/e01012/pan/rootfiles/parity03_%d_beam.root",run);


//Draw canvas and pads

TCanvas *a1 = new TCanvas("a1", "Correlation info page 1",40,40,700,900);

a1_1 = new TPad("a1_1", "a1_1",0.02,0.67,0.98,0.98); 
a1_2 = new TPad("a1_2", "a1_2",0.02,0.34,0.98,0.66);
a1_3 = new TPad("a1_3", "a1_3",0.02,0.02,0.48,0.33);
a1_4 = new TPad("a1_4", "a1_4",0.52,0.02,0.98,0.33);

// display plots 

TH2F *spectral = new TH2F("spectral","",iscal,0,iscal,1000,-100.0,100.0);
TH2F *correl = new TH2F("correl","",iscal,0,iscal,1000,-1.00,1.01);
TH2F *asymmetry = new TH2F("asymmetry","",60,0,60,1000,-1000,1000);
TH2F *sigma = new TH2F("sigma","",60,0,60,1000,100,20000);


f = new TFile(runfile);
TTree *raw = (TTree*)f->Get("R");


Int_t nentries = (Int_t)raw->GetEntries();
//TLeaf *l_bcm1 = raw->GetLeaf("lumi2");
TLeaf *l_bcm1 = raw->GetLeaf("bcm1");
TLeaf *l_bcm2 = raw->GetLeaf("bcm2");

TLeaf *l_lumi1 = raw->GetLeaf("lumi1");
TLeaf *l_lumi2 = raw->GetLeaf("lumi2");

Float_t bcm1[nentries], bcm2[nentries];
Float_t lumi1[nentries], lumi2[nentries];

Float_t G[iscal], J,k;

Float_t sum1,Yield, sum2,S, sum3,Cor;
Int_t np;

// to get all values of a leaf for all events in the tree into an array

for (Int_t i=0; i<nentries; i++) {
  raw->GetEntry(i);
  bcm1[i] = l_bcm1->GetValue();
  bcm2[i] = l_bcm2->GetValue();
  lumi1[i] = l_lumi1->GetValue();
  lumi2[i] = l_lumi2->GetValue();
}
//

// Correlation function 


for (Int_t i=0; i<iscal; i++) {
  G[i] = 2;
}

for (Int_t ioffset=0;  ioffset<iscal;  ioffset++) {
  
  sum1=0;
  sum2=0;
  sum3=0;
  Yield=0;
  S=0;
  np=0;
  Cor=2;

  // The cut bcm1[i]> takes out the beam trip. 


  for (Int_t i=0; i<nentries-ioffset; i++) {
    if(bcm1[i] > ibcm1 && bcm1[i+ioffset] > ibcm1) {
      sum1 = sum1 + lumi1[i]/bcm1[i];
      np++;
    }
  }
  Yield = sum1/np;
  np=0;
  
  for (Int_t i=0; i<nentries-ioffset; i++) {
    if(bcm1[i] > ibcm1 && bcm1[i+ioffset] > ibcm1) {
      sum2 = sum2 + (lumi1[i]/bcm1[i]-Yield)**2;
      sum3 = sum3 + (lumi1[i]/bcm1[i]-Yield)*(lumi1[i+ioffset]/bcm1[i+ioffset]-Yield);
      np++;
    }
  }
  
  S = sum2/np;
  
  Cor = 1.0/S*sum3/np;
  cout << "Offset = " << ioffset << "   Cor = " << Cor << endl;

  G[ioffset] = Cor;
  correl->Fill(ioffset,Cor);
  
}



// Spectral Density function

for (Int_t freq=0;  freq<iscal;  freq++) {

  sum1 = 0;
  sum2 = 0;
  J = 0;
  k = 0;
  for (Int_t tau=0;  tau<iscal;  tau++) {
    sum1 = sum1 + G[tau]*cos(freq*tau);
    sum2 = sum2 + G[tau]*sin(2*3.14159*freq*0.03333/OS*tau);
  }

  J = sum1;
  k = sum2;

  cout << "freq = " << freq << "   k = " << k << endl;


  spectral->Fill(freq,k);
} 


////////////



// Asymmetry function 

Float_t win1,win2, A,asym[900],sig[900];
Int_t k1,k2;

for (Int_t ipair=1; ipair<60; ipair++) {

  np=0;
  sum1=0;
  
  for (Int_t j=0; j<nentries/(2*ipair)-2*ipair; j++) {
 
    win1 = 0; k1=0;
    for (Int_t i=0; i<ipair; i++) {
      if(bcm1[2*ipair*j+i] > ibcm1){
	win1 = win1 + lumi1[2*ipair*j+i]/bcm1[2*ipair*j+i];
        k1++;
      }    
    }
    
    win2=0; k2=0;
    for (Int_t i=ipair; i<2*ipair; i++) {
      if(bcm1[2*ipair*j+i] > ibcm1){
	win2 = win2 + lumi1[2*ipair*j+i]/bcm1[2*ipair*j+i];
	k2++;
      }
    }
    
    if (win1==0 || win2==0){
      A = 0; 
    }else {
      A = (win1/k1 - win2/k2)/(win1/k1 + win2/k2)*1000000.0;
      np++;
    }
    
    sum1 = sum1 + A;
  }
  
  asym[ipair] = sum1/(np+0.000001);
  cout << "Pair = " << ipair << " Asymmetry (ppm) = " << asym[ipair] << endl;
  asymmetry->Fill(ipair,asym[ipair]);
  
} 


// Sigma function 

for (Int_t ipair=1; ipair<60; ipair++) {
  
  np=0;
  sum1=0;
  
  for (Int_t j=0; j<nentries/(2*ipair)-2*ipair; j++) {
    
    win1 = 0; k1=0;
    for (Int_t i=0; i<ipair; i++) {
      if(bcm1[2*ipair*j+i] > ibcm1){
	win1 = win1 + lumi1[2*ipair*j+i]/bcm1[2*ipair*j+i];
	k1++;
      }    
    }
    
    win2=0; k2=0;
    for (Int_t i=ipair; i<2*ipair; i++) {
      if(bcm1[2*ipair*j+i] > ibcm1){
	win2 = win2 + lumi1[2*ipair*j+i]/bcm1[2*ipair*j+i];
	k2++;
      }
    }
    
    if (win1==0 || win2==0){
      A = 0; 
    }else {
      A = (win1/k1 - win2/k2)/(win1/k1 + win2/k2);
	np++;
	sum1 = sum1 + (A - asym[ipair]/1000000.0)**2; 
    }
    
  }
  
  sig[ipair] = (sum1/(np+0.000001))**0.5 * 1000000.0;
  cout << "Pair = " << ipair << " np = " << np << " sigma (ppm) = " << sig[ipair] << endl;
  sigma->Fill(ipair,sig[ipair]);
  
} 


//

a1->cd();

a1_1->Draw(); a1_2->Draw(); a1_3->Draw(); a1_4->Draw(); 

a1_1->cd(); // a1_1->SetLogy(1);
correl->SetMarkerStyle(8);
correl->SetMarkerColor(7);correl->SetMarkerSize(0.7);
correl->Draw();
correl->GetXaxis()->CenterTitle();
correl->GetXaxis()->SetTitleOffset(1.2);
correl->GetXaxis()->SetTitle("LUMI1 Correlation Function C(#tau)");

a1_2->cd();
spectral->SetMarkerStyle(8);
spectral->SetMarkerColor(7);spectral->SetMarkerSize(0.7);
spectral->Draw();
spectral->GetXaxis()->CenterTitle();
spectral->GetXaxis()->SetTitleOffset(1.2);
spectral->GetXaxis()->SetTitle("LUMI1 Spectral Density Function k(#font[2]{f})");


//

a1_3->cd();
asymmetry->SetMarkerStyle(8);
asymmetry->SetMarkerColor(8);asymmetry->SetMarkerSize(0.7);
asymmetry->Draw();
asymmetry->GetXaxis()->CenterTitle();
asymmetry->GetXaxis()->SetTitleOffset(1.2);
asymmetry->GetXaxis()->SetTitle("LUMI1 Average Asymmetry vs number of windows (ppm)");

a1_4->cd(); a1_4->SetLogy(1); 
sigma->SetMarkerStyle(8);
sigma->SetMarkerColor(8);sigma->SetMarkerSize(0.7);
sigma->Draw(); 
sigma->GetXaxis()->CenterTitle();
sigma->GetXaxis()->SetTitleOffset(1.2);
sigma->GetXaxis()->SetTitle("LUMI1 #sigma vs number of windows (ppm)");


a1->Modified();
sprintf(psfile,"cor_%d.eps",run);
a1->Print(psfile);  


}
