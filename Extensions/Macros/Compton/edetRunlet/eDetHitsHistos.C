/** This short macro copies the analyzer generated histograms of Scalers, Accum,
 **.. and Event for a given run and puts them in a 2x2 canvas for a quick view 
 **.. of data from first runlet
 ** A.Narayan */

#include <rootClass.h>
const TString wwwPath("$QWSCRATCH/www");

Int_t eDetHitsHistos(Int_t runnum, Bool_t isFirst100k=kFALSE)
{
  Bool_t debug = 1, chainExists=0;
  Int_t nPlanes = 4;
  Double_t Mps_rate = 960.02;
  TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments
  TH1F *histScaler[nPlanes],*histAccum[nPlanes],*histEvent[nPlanes];
  char *pPath;
  char *folder;
  pPath = getenv("QW_ROOTFILES");
  folder = "mps_histo/Compton_Electron/";
  TFile *f1;

  if(isFirst100k) {
    chainExists = mpsChain->Add(Form("%s/Compton_first100k_%d.root",pPath,runnum));
    f1=new TFile(Form("%s/Compton_first100k_%d.root",pPath,runnum));
    if (debug) f1->ls();// Ask it to list itself.
  } 
  else {
    chainExists = mpsChain->Add(Form("%s/Compton_Pass1_%d.000.root",pPath,runnum));
    f1= new TFile(Form("%s/Compton_Pass1_%d.000.root",pPath,runnum));
    if (debug) f1->ls();// Ask it to list itself.
  }

  if (!chainExists) printf("couldn't add the given run file to Chain\n");
  Int_t nEntries = mpsChain->GetEntries();
  Double_t rate = nEntries/Mps_rate;

  TCanvas *c1 = new TCanvas("c1",Form("Scalers_%d",runnum),50,0,820,700);
  TCanvas *c2 = new TCanvas("c2",Form("Accum_%d",runnum),150,75,820,700);
  TCanvas *c3 = new TCanvas("c3",Form("Event_%d",runnum),250,150,820,700);
  c1->Divide(2,2);
  c2->Divide(2,2);
  c3->Divide(2,2);

  for (Int_t p = 0; p < nPlanes; p++) {
    histScaler[p] = (TH1F*)f1->Get(Form("%s/Compton_eDet_Scal_Raw_Plane%d",folder,p+1));
    histAccum[p] = (TH1F*)f1->Get(Form("%s/Compton_eDet_Accum_Raw_Plane%d",folder,p+1));
    histEvent[p] = (TH1F*)f1->Get(Form("%s/Compton_eDet_Evt_Raw_Plane%d",folder,p+1));
    if (histScaler[p] != NULL) {
      histScaler[p]->Sumw2();
      histScaler[p]->Scale(1/rate);
      histScaler[p]->GetYaxis()->SetTitle("normalized Scaler (Hz)");
      histScaler[p]->GetYaxis()->SetTitleOffset(1.3);
      histScaler[p]->SetTitle(Form("eDetector plane %d",p+1));
      c1->cd(p+1);
      histScaler[p]->DrawCopy("H");
    }
    else printf("***Error: Didn't find Scalers histogram***\n");

    if (histAccum[p] != NULL) {
      histAccum[p]->Sumw2();
      histAccum[p]->Scale(1/rate);
      histAccum[p]->GetYaxis()->SetTitle("normalized Accum mode (Hz)");
      histAccum[p]->GetYaxis()->SetTitleOffset(1.3);
      histAccum[p]->SetTitle(Form("eDetector plane %d",p+1));
      c2->cd(p+1);
      histAccum[p]->DrawCopy("H");
    }
    else printf("***Error: Didn't find Accum histogram***\n");

    if (histEvent[p] != NULL) {
      histEvent[p]->Sumw2();
      histEvent[p]->Scale(1/rate);
      histEvent[p]->GetYaxis()->SetTitle("normalized Event mode (Hz)");
      histEvent[p]->GetYaxis()->SetTitleOffset(1.3);
      histEvent[p]->SetTitle(Form("eDetector plane %d",p+1));
      c3->cd(p+1);
      histEvent[p]->DrawCopy("H");
    }
    else printf("***Error: Didn't find Event histogram***\n");
  }
  c1->SaveAs(Form("%s/run_%d/eDetHitsHistos_%d_scalar.png",wwwPath.Data(),runnum,runnum));
  c2->SaveAs(Form("%s/run_%d/eDetHitsHistos_%d_accum.png",wwwPath.Data(),runnum,runnum));
  c3->SaveAs(Form("%s/run_%d/eDetHitsHistos_%d_event.png",wwwPath.Data(),runnum,runnum));

  delete f1;
  return chainExists;
}
