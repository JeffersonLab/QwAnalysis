// Purpose:  examine results of adcped run.
#include <fstream>

#define ADC_MaxSlot 10
#define ADC_MaxChan 4

void ADCPed() {
  Int_t run;
  cout << "Enter adcped run number : ";
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

  txtfile =  path + "/" + prefix + "_" + run + "_adcped_Peds.txt";
  rootfile =  path + "/" + prefix + "_" + run + "_Peds." + suffix;

  cout << "text file is " << txtfile << endl;
  cout << "root file is " << rootfile << endl;


  Bool_t chanExists[ADC_MaxSlot][ADC_MaxChan];
  Float_t chanAvg[ADC_MaxSlot][ADC_MaxChan];
  Float_t chanSig[ADC_MaxSlot][ADC_MaxChan];

  Int_t ich,isl;
  Float_t avg,sig;

  for (isl=0; isl<ADC_MaxSlot; isl++) {
    for (ich=0;ich<ADC_MaxChan; ich++) {
      chanExists[isl][ich] = kFALSE;
    }
  }

  ifstream pedfile(txtfile.Data(),ios::in);
  while (pedfile >> isl >> ich >> avg >> sig) {
    if (isl<ADC_MaxSlot && ich < ADC_MaxChan) {
      chanExists[isl][ich] = kTRUE;
      chanAvg[isl][ich] = avg;
      chanSig[isl][ich] = sig;
    }
  }
  pedfile.close();

  gStyle->SetOptStat(1);
  gStyle->SetStatH(0.35);
  gStyle->SetStatW(0.3);
  gStyle->SetTitleH(0.1);
  gStyle->SetTitleW(0.3);
  gStyle->SetLabelSize(0.08,"x");
  gStyle->SetLabelSize(0.08,"y");
  gROOT->ForceStyle();
  
  TFile *fin = new TFile(rootfile.Data());

  TCanvas *cped[10];
  TPad* pad[80];

  Int_t ncan = 0;

  char buff[10];
  sprintf(buff,"APCPeds%d",0);
  cped[0] = new TCanvas(buff,buff,800,800);

  cped[ncan]->SetFillColor(42);
  cped[ncan]->Clear();
  Int_t tpad = 0;
  for (Int_t irow= 0; irow<4; irow++) {
    Float_t Ylo = 0.25*(3-irow)+0.005;
    Float_t Yhi = Ylo +0.25 - 0.01;
    for (Int_t icol=0; icol<2; icol++) {
      sprintf(buff,"Pad%d",tpad);
      Float_t Xlo = 0.5*(1-icol) + 0.005;
      Float_t Xhi = Xlo +0.5 - 0.01;
      //      cout << tpad << "th  pad is " << buff << endl;
      pad[tpad] = new TPad(buff,"",Xlo,Ylo,Xhi,Yhi,18);
      pad[tpad]->Draw();
      tpad++;
    }
  }

  Int_t npad = 0;
  TH1F* hped;
  for (isl=0; isl<ADC_MaxSlot; isl++) {
    for (ich=0;ich<ADC_MaxChan; ich++) {
      if (chanExists[isl][ich]) {
  	sprintf(buff,"Ped:%d-%d",isl,ich);
  	hped = (TH1F) fin->Get(buff);
	
  	if(hped) {

  	  if (npad>=tpad) {
  	    cped[ncan]->Update();
	    sprintf(buff,"APCPeds%d",++ncan);
	    cped[ncan] = new TCanvas(buff,buff,800,800);

	    cped[ncan]->SetFillColor(42);
	    cped[ncan]->Clear();
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

	  Float_t xc = hped->GetMean();
	  Float_t xrms = hped->GetRMS();
	  Float_t xd, xl, xh;
	  if (xrms<100 ) {
	    if (xrms>10) xd = 10*xrms;
	    else xd = 100;
	  }
	  if (xc>xd) xl = xc - xd;
	  else xl = 0;
	  if ((xc+xd)<32000) xh = xc + xd;
	  else xh = 32000;

	  xd = 3*xrms;
	  hped->SetAxisRange(xl,xh);
	  cped[ncan]->cd();
	  pad[npad]->cd();
	  hped->Draw();
	  TLine *l = new TLine(xc,0,xc,1.05*((Double_t) hped->GetMaximum()));
	  l->SetLineColor(2);
	  l->Draw();
  	  npad++;
  	}
      }
    }
  }
  cped[ncan]->Update();

  //fin.Close();
}





