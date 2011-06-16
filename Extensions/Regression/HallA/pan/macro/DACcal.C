 // Purpose:  examine results of adcdac run.
#include <fstream>

#define ADC_MaxSlot 10
#define ADC_MaxChan 4

void DACcal() {
  Int_t run;
  cout << "Enter adcdac run number : ";
  cin >> run;

  TString txtfile, rootfile, path, prefix, suffix;
  path = getenv("PAN_OUTPUT_FILE_PATH");
  prefix = getenv("PAN_FILE_PREFIX");
  suffix = getenv("PAN_ROOT_FILE_SUFFIX"); 
  if ( prefix.IsNull() ) {
    prefix = "parity02";
  }
  if ( suffix.IsNull() ) {
    suffix = "root";
  }

  txtfile =  path + "/" + prefix + "_" + run + "_adcdac_DAC.txt";
  rootfile =  path + "/" + prefix + "_" + run + "_DAC." + suffix;

  cout << "text file is " << txtfile << endl;
  cout << "root file is " << rootfile << endl;


  Bool_t chanExists[ADC_MaxSlot][ADC_MaxChan];
  Float_t inter[ADC_MaxSlot][ADC_MaxChan];
  Float_t slope[ADC_MaxSlot][ADC_MaxChan];

  Int_t ich,isl;
  Float_t x0,slp;

  for (isl=0; isl<ADC_MaxSlot; isl++) {
    for (ich=0;ich<ADC_MaxChan; ich++) {
      chanExists[isl][ich] = kFALSE;
    }
  }

  ifstream dacfile(txtfile,ios::in);
  while (dacfile >> isl >> ich >> x0 >> slp) {
    if (isl<ADC_MaxSlot && ich < ADC_MaxChan && x0 !=0 && slp != 0) {
      chanExists[isl][ich] = kTRUE;
      inter[isl][ich] = x0;
      slope[isl][ich] = slp;
    }
  }
  dacfile.close();

  gStyle->SetOptStat(1);
  gStyle->SetStatH(0.35);
  gStyle->SetStatW(0.3);
  gStyle->SetTitleH(0.15);
  gStyle->SetTitleW(0.5);
  gStyle->SetLabelSize(0.08,"x");
  gStyle->SetLabelSize(0.08,"y");
  gROOT->ForceStyle();
  
  TFile *fin = new TFile(rootfile);

  TCanvas *cdac[10];
  TPad* pad[80];

  Int_t ncan = 0;

  char buff[10];
  sprintf(buff,"DACcal%d",0);
  cdac[0] = new TCanvas(buff,buff,800,800);

  cdac[ncan]->SetFillColor(42);
  cdac[ncan]->Clear();
  Int_t tpad = 0;
  for (Int_t irow= 0; irow<4; irow++) {
    Float_t Ylo = 0.25*(3-irow)+0.005;
    Float_t Yhi = Ylo +0.25 - 0.01;
    for (Int_t icol=0; icol<2; icol++) {
      sprintf(buff,"Pad%d",tpad);
      Float_t Xlo = 0.5*icol + 0.005;
      Float_t Xhi = Xlo +0.5 - 0.01;
      //      cout << tpad << "th  pad is " << buff << endl;
      pad[tpad] = new TPad(buff,"",Xlo,Ylo,Xhi,Yhi,18);
      pad[tpad]->Draw();
      tpad++;
    }
  }

  Int_t npad = 0;
  TGraphErrors* gdac;
  TGraphErrors* gres;
  TF1 *f1 = new TF1("CalLine","[0] + [1]*x",0,65535);
  for (isl=0; isl<ADC_MaxSlot; isl++) {
    for (ich=0;ich<ADC_MaxChan; ich++) {
      if (chanExists[isl][ich]) {
  	sprintf(buff,"DAC:%d-%d",isl,ich);
  	gdac = (TGraphErrors) fin->Get(buff);
  	sprintf(buff,"RES:%d-%d",isl,ich);
  	gres = (TGraphErrors) fin->Get(buff);
	f1->SetParameter(0,inter[isl][ich]);
	f1->SetParameter(1,slope[isl][ich]);
      if(gdac) {
  	  if (npad>=tpad-1) {
  	    cdac[ncan]->Update();
	    sprintf(buff,"APCdacs%d",++ncan);
	    cdac[ncan] = new TCanvas(buff,buff,800,800);

	    cdac[ncan]->SetFillColor(42);
	    cdac[ncan]->Clear();
	    for (Int_t irow= 0; irow<4; irow++) {
	      Float_t Ylo = 0.25*(3-irow)+0.005;
	      Float_t Yhi = Ylo +0.25 - 0.01;
	      for (Int_t icol=0; icol<2; icol++) {
		Int_t id = irow*2 + icol;
		sprintf(buff,"Pad%d",tpad);
		Float_t Xlo = 0.5*(1-icol) + 0.005;
		Float_t Xhi = Xlo +0.5 - 0.01;
		pad[tpad] = new TPad(buff,"",Xlo,Ylo,Xhi,Yhi,18);
		pad[tpad]->Draw();
		tpad++;
	      }
	    }
  	  }

	  cdac[ncan]->cd();
	  pad[npad++]->cd();
	  TGraph* gtmp = new TGraph(*gdac);
	  gtmp->SetMarkerStyle(7);
	  gtmp->Draw("AP");
  	  f1->SetLineColor(2);
  	  f1->DrawCopy("SAME");
	  pad[npad++]->cd();
	  TGraph* gtmp2 = new TGraph(*gres);
	  gtmp2->SetMarkerStyle(7);
	  gtmp2->Draw("AP");
  	}
      }
    }
  }
  cdac[ncan]->Update();

  //fin.Close();
}






