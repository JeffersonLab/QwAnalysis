//********************************************************************//
// Author : B. Waidyawansa
// Date   : March 10, 2009
//********************************************************************//
//
// This macro extract Rotatable Half Wave Plate Scans (RHWP) results of 
// RHWP runs and prints the plots on to a .gif file
// To use this macro, 
// open root and load the macro by typing .L RHWP.C and type in the command.
// RHWP_Scan(run_num,  bcm,  bpm, ihwp, pita)
//   run_num - run number
//   bcm     - bcm that was used for current monitoring
//   bpm     - bpm that needs to be analysed 
//   ihwp    - state of the insertable half wave plate (ihwp). IN or OUT.
//   pita    - value of the pita voltage used in Volts.
//
// e.g.
// root[0] .L RHWP.C
// root[1]  RHWP_Scan(000, "qwk_bcm0l02","qwk_1i02","IN",120)




//********************************************
// Main function
//********************************************

void RHWP_Scan(Int_t run_num, TString bcm, TString bpm, TString ihwp, Int_t pita)
{
  
  gROOT->Reset();
  gDirectory->Delete("pAq*");
  gDirectory->Delete("pdx*");
  gDirectory->Delete("pdy*");

  //Canvas Parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  //Pad parameters
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetLabelSize(0.04,"x");
  gStyle->SetLabelSize(0.04,"y");
  gStyle->SetTitleXSize(0.08);
  gStyle->SetPaperSize(10,12);
  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleYSize(0.10);
  gStyle->SetOptFit(0000);
  gStyle->SetOptStat(0000000);

  //hist parameters
  gStyle->SetTitleOffset(0.8,"x");
  gStyle->SetTitleSize(0.07,"x");
  gStyle->SetTitleOffset(0.8,"y");
  gStyle->SetTitleSize(0.07,"y");
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gROOT->ForceStyle();


  Char_t filename[200];
  TString  plotcommand;
  TString  pos[2]={"RelX","RelY"};
  TString  prof[2]={"pdx","pdy"};
  
  // Get the root file
  TString directory="~/scratch/rootfiles/"; // the location of the rootfile used for calibration
  sprintf(filename,"%sQweak_BeamLine_%d.root",directory.Data(),run_num);
  f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  cout<<"Opening "<<filename<<"\n";
  cout<<"Obtaining data from: "<<filename<<endl;
  cout<<"\n"<<endl;

  
  std::cout<<"Extracting charge from bcm : "<<bcm.Data()<<"\n";;
  
  TTree *nt = (TTree*)f->Get("HEL_Tree"); //load the tree
  TString title = Form("RHWP Scan results of run %i, IHWP:%s, PITA=%i", run_num,ihwp.Data(),pita);
  TCanvas *c1 = new TCanvas("c1",title,40,40,1200,1000);
  
  
  pad1 = new TPad("pad1","pad1",0.05,0.93,0.93,0.97);
  pad2 = new TPad("pad2","pad2",0.05,0.02,0.95,0.92);
  pad1->SetFillColor(11);
  pad1->Draw();
  pad2->Draw();
  
  pad1->cd();
  t1 = new TText(0.30,0.3,title);
  t1->SetTextSize(0.8);
  t1->Draw();
 
  pad2->Divide(1,3);
  
  // TH1F *pAq = new TH1F("pAq","",100,0,190);
  // TH1F *pdx = new TH1F("pdx","",100,0,190);
  //TH1F *pdy = new TH1F("pdy","",100,0,190);

  //TF1 *pdx = new TF1("pdx","",100,0.0,190.0,-100.0,100.0);
  //TProfile *pdy = new TProfile("pdy","",100,0.0,190.0,-100.0,100.0);
  
  TF1 *fAq = new TF1("fAq",fitf,0.0,190.0,5);  //Fit 
  TF1 *fdx = new TF1("fdx",fitf,0.0,190.0,5);  //Fit 
  TF1 *fdy = new TF1("fdy",fitf,0.0,190.0,5);  //Fit 
 
  if (bcm.Contains("bcm0l02"))
    TString current = Form("asym_qwk_%s.hw_sum",bcm.Data());
  else  
    TString current = Form("asym_qwk_%sWSum.hw_sum",bcm.Data());
  
  nt->SetAlias("current",current);
  nt->Draw("current*1000000:scandata1>>pAq","cleandata==1","proff,goff");
  pAq->SetStats(kFALSE);
  pad2->cd(1);
  pAq->Draw();
  pAq->GetYaxis()->SetTitle(Form("%s : Aq (ppm)",bcm.Data()));
  Fit_histogram(pAq,fAq,run_num);
  pAq->SetTitle("");
  pad2->Update();


  pad2->cd(2);
  plotcommand=Form("1000*diff_qwk_%s%s.hw_sum:scandata1>>pdx",bpm.Data(),pos[0].Data());
  nt->Draw(plotcommand,"cleandata==1","proff,goff");
  pdx->SetStats(kFALSE); 
  pdx->Draw();
  pdx->GetYaxis()->SetTitle(Form("%s : #Deltax(nm)",bpm.Data()));
  Fit_histogram(pdx,fdx,run_num);
  pdx->SetTitle("");
  pad2->Update();


  pad2->cd(3);
  plotcommand = Form("1000*diff_qwk_%s%s.hw_sum:scandata1>>pdy",bpm.Data(),pos[1].Data());
  nt->Draw(plotcommand,"cleandata==1","proff,goff");
  pdy->SetStats(kFALSE);
  pdy->Draw(); 
  pdy->GetYaxis()->SetTitle(Form("%s: #Deltay(nm)",bpm.Data()));
  Fit_histogram(pdy,fdy,run_num);
  pdy->SetTitle("");
  pad2->Update();


  c1->cd();
  TString   pstit = Form("%i_RHWP_OUT_%s.gif",run_num,bcm.Data());
  c1->Print(pstit.Data());
  c1->Update();

};


//********************************************
// User Defined fit function
//********************************************

Double_t fitf(Double_t *x, Double_t *par)
{

  Double_t fitval = par[0]+ par[1]*TMath::Sin(2.0*(TMath::Pi()/180.0)*x[0] + par[2]) + par[3]*TMath::Sin(4.0*(TMath::Pi()/180.0)*x[0] + par[4]);
  return fitval;
}

//********************************************
d// Fit function Analysis function
//********************************************

Double_t func(Double_t a, Double_t b,  Double_t c, Double_t theta, Double_t phase)
{

  Double_t val = c + a*TMath::Sin(b*TMath::Pi()/180.0*theta + phase);
  return val;
}


//********************************************
// Histogram fit function
//********************************************

void Fit_histogram(TProfile *p, TF1 *f, Int_t run_num)
{


  TString hist_name;
  TString name;
 
  f->SetParameters(1.0,1.0,1.0,1.0,1.0);
  f->SetParNames("p1","p2","p3","p4","p5");
  f->SetLineColor(32);
  f->SetLineWidth(1);  


  p->Fit(f->GetName(),"r");

  Double_t rectphs = 0;
  rectphs=f->GetParameter(2);
  while (rectphs>TMath::Pi()) rectphs-=2*TMath::Pi();  
  while (rectphs<-TMath::Pi()) rectphs+=2*TMath::Pi(); 

  if (rectphs<0) { 
    rectphs+=TMath::Pi();
    f->SetParameter(1,-1*f->GetParameter(1)); 
  }
  f->SetParameter(2,rectphs); 

  rectphs = f->GetParameter(4); 
  while (rectphs>TMath::Pi()) rectphs-=2*TMath::Pi();
  while (rectphs<-TMath::Pi()) rectphs+=2*TMath::Pi();
  if (rectphs<0) { 
    rectphs+=TMath::Pi();
    f->SetParameter(3,-1*f->GetParameter(3));
  }
  f->SetParameter(4,rectphs);


  p->Fit(f->GetName(),"r");
  p->GetXaxis()->SetTitle("Angle theta (degree)");
  p->SetMarkerSize(0.6);
  p->SetMarkerStyle(20);
  p->SetMarkerColor(4);
  p->SetStats(kTRUE);
  p->Draw("same");




  TString funcstr = Form("Fit: %s  = %7.1f + %7.1f sin (2#theta+%7.1f) +  %7.1f sin (4#theta+%7.1f)      ChiSquare = %7.2f",f->GetName(),f->GetParameter(0),f->GetParameter(1),f->GetParameter(2)*180.0/TMath::Pi(),f->GetParameter(3),f->GetParameter(4)*180.0/TMath::Pi(),f->GetChisquare());
 


  pt = new TPaveText(0.06,0.90,0.95,0.98,"brNDC");
  pt->SetBorderSize(1); 
  pt->SetFillColor(10);
  pt->SetTextAlign(12);pt->SetTextFont(22);
  pt->SetTextColor(1);
  pt->SetTextSize(0.09);
  pt->AddText(funcstr.Data());
  pt->Draw();


}



//********************************************
// Fit analysis function
//********************************************


void Fit_Analysis(TProfile *p, TF1 *f)
{

  c2 = new TCanvas("c2","Fit Analysis",40,40,100,100);
  c2 ->SetGrid();
  c2-Divide(1,3);

  //Get fit data
  const Int_t n=19; 
  Double_t y1[n], y2[n], y3[n], x[n];
  Double_t c = f->GetParameter(0);
  Double_t a1 = f->GetParameter(1);
  Double_t a2 = f->GetParameter(3);
  Double_t phase1 = f->GetParameter(2);
  Double_t phase2 = f->GetParameter(4);



  for(Int_t i=0;i<n;i++)
    {
      x[i]=i*10;
      y1[i]=func(a1,2,c,x[i],phase1);
      y2[i]=func(a2,4,0,x[i],phase2);
      y3[i]=y1[i]+y2[i];
    }

 TMultiGraph *mg = new TMultiGraph();
 mg ->SetTitle("Fit Analysis");
 gr1 = new TGraph(n,x,y1);
 gr1 ->SetLineColor(1);


 gr2 = new TGraph(n,x,y2);
 gr2 ->SetLineColor(4);


 gr3 = new TGraph(n,x,y3);
 gr3 ->SetLineColor(2);
 gr3 ->SetLineWidth(3);

 mg ->Add(gr1);
 mg ->Add(gr2);
 mg ->Add(gr3);

 mg ->Draw("AC");

//  char linetxt[50];
//  sprintf(linetxt,"Fit Analysis for Run %s ",run_num);
//  TString funcstr1 = linetxt;
//  sprintf(linetxt," Fit : %s  = %7.2f + ",f->GetName(),c);
//  funcstr1 += linetxt;
//  sprintf(linetxt," %7.2f sin (2#theta+%6.2f) +",a1,phase1*180.0/TMath::Pi());
//  funcstr1 += linetxt;
//  sprintf(linetxt," %7.2f sin (4#theta+%6.2f)",a2,phase2*180.0/TMath::Pi());
//  funcstr1 += linetxt;

//  pt = new TPaveText(0.2,0.92,0.90,0.98,"brNDC");
//  pt->SetBorderSize(1);pt->SetFillColor(10);
//  pt->SetTextAlign(12);pt->SetTextFont(22);
//  pt->SetTextColor(32);
//  pt->AddText(funcstr1.Data());
//  pt->Draw()
   ;



};

