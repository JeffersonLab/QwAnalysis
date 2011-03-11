//********************************************************************//
// Author : B. Waidyawansa
// Date   : March 10, 2009
//********************************************************************//
//
// This macro extract the results of PITA Scans of PITA runs and prints the plots on to a .gif file
// To use this macro, 
// open root and load the macro by typing .L PITA_Scan.C and type in the command.
// plot_pita(run_num,  bcm,  bpm, ihwp)
//   run_num - run number
//   bcm     - the reference bcm used to obtain the charge asymmtery
//   bpm     - the reference bpm used to obtain  the position difference variations
//   ihwp    - state of the insertable half wave plate (ihwp). IN or OUT.
//
// e.g.
// root[0] .L PITA_Scan.C
// root[1]  plot_pita(000, "qwk_bcm0l02","qwk_1i02","IN")



void plot_pita(Int_t runnum, TString bcm, TString bpm, TString ihwp){


  Char_t filename[200];


  //Get the root file
  //TString directory="~/scratch/rootfiles/"; // the location of the rootfile 
  //sprintf(filename,"%sQweak_BeamLine_%d.root",directory.Data(),runnum);
  TString directory="/home/cdaq/qweak/QwScratch/rootfiles/";
  sprintf(filename,"%sQweak_%d.000.root",directory.Data(),runnum);
  f = new TFile(filename);
  if(!f->IsOpen())
    return 0;
  
  std::cout<<"Opening root file "<<filename<<"\n";
  std::cout<<"Run : "<<runnum<<"    IHWP : "<<ihwp<<"\n";
  std::cout<<"Obtaining Charge asymmetry from  :  qwk_"<<bcm<<"\n";
  std::cout<<"Obtaining Position variations from : qwk_"<<bpm<<"\n";

  // IA Scan
  //TString gtitle = Form("PITA Scan  of run %i , %s",runnum,ihwp.Data());

  //PITA Scan
  TString gtitle = Form("PITA Scan  of run %i , IHWP :%s",runnum,ihwp.Data());

  // general parameters
  gStyle->SetOptDate(0);    
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.1);  
  gStyle->SetOptStat(00000);  
  gStyle->SetOptTitle(0); 
  gStyle->SetOptFit(10101);  
  gStyle->SetPaperSize(12,15);
  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);
  //gStyle->SetCanvas Color(-10);
  // pads parameters  
  gStyle->SetPadColor(11); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  //histos
  gStyle->SetTitleSize(0.07,"x");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetTitleOffset(1.0,"x");
  gStyle->SetTitleOffset(0.7, "y");
  gROOT->ForceStyle();  

  TCanvas *a1 = new TCanvas("a1", "PITA Plots", 0,0,900,800);
  TPad * a1_title = new TPad("a1_title","a1_title",0.02,0.90,0.98,0.98);  
  TPad * pq  = new TPad( "pq", "pq",0.02,0.6,0.80,0.88);
  TPad * pqe  = new TPad( "pq", "pq",0.82,0.6,0.98,0.88);  
  TPad * px  = new TPad( "px", "px",0.02,0.3,0.80,0.58); 
  TPad * pxe  = new TPad( "pq", "pq",0.82,0.3,0.98,0.58);  
  TPad * py  = new TPad( "py", "py",0.02,0.02,0.80,0.28);    
  TPad * pye  = new TPad( "pq", "pq",0.82,0.02,0.98,0.28);
 
  a1->Draw();
  a1_title->Draw();
  pq->Draw();
  px->Draw();
  py->Draw();
  pqe->Draw();
  pxe->Draw();
  pye->Draw();

  a1_title->cd();
  TPaveText* pt2 = new TPaveText(0.04,0.04,0.96,0.96,"brNDC");
  pt2->SetBorderSize(1);pt2->SetFillColor(10);
  pt2->SetTextFont(20);pt2->SetTextSize(0.55);
  text = pt2->AddText(0.5,0.9,gtitle.Data());
  pt2->Draw();

  //load the Tree
  nt = (TTree*)f->Get("HEL_Tree"); 

  //Charge
  if(bcm.Contains("bcm0l02"))
    TString current = Form("asym_qwk_%s.hw_sum*1000000",bcm.Data());  
  else
    TString current= Form("asym_qwk_%sWSum.hw_sum*1000000",bcm.Data());  

  std::cout<<current.Data()<<"\n";
  nt->SetAlias("current",current);
  pq->cd();
  nt->Draw("current:scandata1>>hAq","cleandata==1","proff,goff");
  hAq->Fit("pol1");
  TF1 *fq = hAq->GetFunction("pol1");
  hAq->SetMarkerColor(12);
  hAq->SetMarkerSize(1.0);
  hAq->SetMarkerStyle(22);
  hAq->GetXaxis()->CenterTitle();
  hAq->GetXaxis()->SetTitle("Voltage(V)");
  hAq->GetYaxis()->CenterTitle();
  hAq->GetYaxis()->SetTitle("Charge Asymmetry(ppm)");
  TString funcstrq1 = Form("Aq = %7.2f ppm + ",fq->GetParameter(0));
  TString funcstrq2 = Form("%7.2f * x ppm/V",fq->GetParameter(1));
  pqe->cd();
  pt = new TPaveText(0.0,0.3,1.0,0.8,"brNDC");
  pt->SetBorderSize(0);
  pt->SetFillColor(11);
  pt->SetTextAlign(20);
  pt->SetTextFont(22);
  pt->AddText(Form(" bcm : %s",bcm.Data()));
  pt->AddText(funcstrq1.Data());
  pt->AddText(funcstrq2.Data());
  pt->Draw();

  //delta x
  TString dx = Form("diff_qwk_%sRelX.hw_sum*1000/2",bpm.Data());
  nt->SetAlias("dx",dx);
  px->cd();
  nt->Draw("dx:scandata1>>hDx","cleandata==1","proff,goff");
  hDx->Fit("pol1");
  TF1 *fx = hDx->GetFunction("pol1");
  hDx->SetMarkerColor(12);
  hDx->SetMarkerSize(1.0);
  hDx->SetMarkerStyle(22);
  hDx->GetXaxis()->CenterTitle();
  hDx->GetXaxis()->SetTitle("Voltage(V)");
  hDx->GetYaxis()->CenterTitle();
  hDx->GetYaxis()->SetTitle("#DeltaX(#mum)");
  TString funcstrx1 = Form("dx = %7.2f nm + ",(fx->GetParameter(0))*1000);
  TString funcstrx2 = Form("%7.2f * x nm/V",(fx->GetParameter(1))*1000);
  //  TString funcstrx2 = Form("dx = %7.2f nm + \n %7.2f * x nm/V",(fx->GetParameter(0))*1000,(fx->GetParameter(1))*1000);

  pxe->cd();
  pxt = new TPaveText(0.0,0.3,1.0,0.8,"brNDC");
  pxt->SetBorderSize(0);
  pxt->SetFillColor(11);
  pxt->SetTextAlign(20);
  pxt->SetTextFont(22);
  pxt->AddText(Form(" bpm : %s",bpm.Data()));
  pxt->AddText(funcstrx1.Data());
  pxt->AddText(funcstrx2.Data());
  pxt->Draw();

  //delta Y
  TString dy = Form("diff_qwk_%sRelY.hw_sum*1000/2",bpm.Data());
  nt->SetAlias("dy",dy);
  py->cd();
  nt->Draw("dy:scandata1>>hDy","cleandata==1","proff,goff");
  hDy->Fit("pol1");
  TF1 *fy = hDy->GetFunction("pol1");
  hDy->SetMarkerColor(12);
  hDy->SetMarkerSize(1.0);
  hDy->SetMarkerStyle(22);
  hDy->GetXaxis()->CenterTitle();
  hDy->GetXaxis()->SetTitle("Voltage(V)");
  hDy->GetYaxis()->CenterTitle();
  hDy->GetYaxis()->SetTitle(" #DeltaY (#mum)");
  TString funcstry1 = Form("dy = %7.2f nm + ",fy->GetParameter(0)*1000);
  TString funcstry2 = Form(" %7.2f * x nm/V",fy->GetParameter(1)*1000);
  pye->cd();
  pyt = new TPaveText(0.0,0.3,1.0,0.8,"brNDC");
  pyt->SetBorderSize(0);
  pyt->SetFillColor(11);
  pyt->SetTextAlign(20);
  pyt->SetTextFont(22);
  pyt->AddText(Form(" bpm : %s",bpm.Data()));
  pyt->AddText(funcstry1.Data());
  pyt->AddText(funcstry2.Data());
  pyt->Draw();

  //Errors of the fit
  //   const Int_t nwin = 100;
  //   Double_t meanAq[nwin];
  //   Double_t errAq[nwin];
  //   Int_t num = 0;
  //   for(Size_t i=0; i<nwin; i++){
  //     Double_t value = hAq->GetBinContent(i);
  //     Double_t abval = TMath::Abs(value);
  //     if(TMath::Abs(value) >0){
  //       meanAq[num] = hAq->GetBinContent(i);
  //       errAq[num] = hAq->GetBinError(i);
  //       cout << "meanAq = " << meanAq[num] << endl;
  //       cout << "errAq = " << errAq[num] << endl;
  //       cout << "num = " << num << endl;
  //       cout << "**********************" << endl;
  //       num++;
  //     }
  //     errAq[i] = hAq->GetBinError(i+1);
  //   }
  
  a1->cd();
  TString psnam = Form("%i_pita_plot.gif",runnum );
  a1->Print(psnam.Data());

}












