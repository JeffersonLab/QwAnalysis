/**************************************************************************/
/*                              Nuruzzaman                                */
/*                              03/10/2011                                */
/*                                                                        */
/*                        Last Edited:04/22/2011                          */
/*                                                                        */
/* PLEASE CONSULT WITH ME BEFORE CHANGING THE SCRIPT. IF NEEDED YOU CAN   */
/* HAVE YOUR COPY AND EDIT THERE.TRY NOT TO USE qwanalysis DIRECTORY.     */
/* PLEASE MAINTAIN THIS TO GET OVERWRITTEN BY OLD COPIES.                 */
/*                                                                        */
/* This macro create all the necessary plots using first 100k rootfiles   */
/* for the user. Manual at the bottom of this macro for its use           */
/**************************************************************************/
//gROOT->Reset();
#include "NurClass.h"

// TString 
// TabRule(const char* one, Double_t unit_one, Double_t val[4])
// {
//   return Form("\n %14s | %12.3f | %+12.3f +/- %8.3f | %+12.3f +/- %8.3f", one, d_one, val[0], val[2], val[1], val[3]);
// };

void qwanalysis(UInt_t run_number=0)
{

  Bool_t MDPMT=kTRUE; Bool_t CHARGE=kTRUE; Bool_t CHARGEDD=kTRUE; Bool_t BMODCYCLE=kTRUE; Bool_t BPMS=kTRUE; 
  Bool_t MDYIELDVAR=kTRUE; Bool_t MDBKG=kTRUE; Bool_t MDALLASYM=kTRUE; Bool_t SENSITIVITY=kTRUE; 
  Bool_t MDLUMI=kTRUE; Bool_t USLUMI=kTRUE; Bool_t USLUMISEN=kTRUE; Bool_t BMODSEN=kTRUE;
//   Bool_t MDPMT=kFALSE; Bool_t CHARGE=kFALSE; Bool_t CHARGEDD=kFALSE; Bool_t BMODCYCLE=kFALSE; Bool_t BPMS=kFALSE; 
//   Bool_t MDYIELDVAR=kFALSE; Bool_t MDBKG=kFALSE; Bool_t MDALLASYM=kFALSE; Bool_t SENSITIVITY=kFALSE; 
//   Bool_t MDLUMI=kFALSE; Bool_t USLUMI=kFALSE; Bool_t USLUMISEN=kFALSE;
//   Bool_t BMODSEN=kTRUE;

  //  UInt_t run_number = 0;
  char run0[255],run[255],run100k[255],sline[255],dline[255],ssline[255],sslinen[255];
  char red[] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
  char green[] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
  char blue[] = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };
  char magenta[] = { 0x1b, '[', '1', ';', '3', '5', 'm', 0 };
  char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

  sprintf(sline,"%s#-------------------------------------------------------------------------#%s\n",magenta,normal);
  sprintf(dline,"%s#*************************************************************************#%s\n",red,normal);
  sprintf(ssline,"%s-------------------------------------------------%s\n",green,normal);
  sprintf(sslinen,"-------------------------------------------------\n",normal);

  if( run_number == 0 ) {
    /* Ask to input the run condition to user */
    printf(sline); printf("%sEnter Run Number%s\n",blue,normal); printf(sline);
    cin >> run_number;
  }

  //  TFile f(Form("/home/nur/scratch/rootfiles/first100k_%d.root",run_number));
  TFile *f = new TFile(Form("$QW_ROOTFILES/first100k_%d.root",run_number));
  if (!f->IsOpen()) {printf("%sFile not found. Exiting the program!%s\n",red,normal); exit(1); }

  /* Directory */
  Char_t *dir[2];
  //  dir[0] = "/home/nur/Desktop/beamModulation/plots";
  dir[0] = "/net/cdaqfs/home/cdaq/users/qwanalysis/plots";
  dir[1] = "/net/cdaqfs/home/cdaq/users/qwanalysis/hclog_runlist";

//   printf(sline); printf("%sPlease Insert target Information. 1= LH2, 2= 4% US Al, 3= 2% US Al, 4= 1% US Al,\n5= 4% DS Al, 6= 2% DS Al, 7= 1% DS Al, 8= US Pure Al and hit ENTER\n %s",blue,normal); printf(sline);

//    UInt_t target = 0;
//   cin >> target;
//   if (target> 8) {printf("%sPlease insert a correct No. Exiting the program!%s\n",blue,normal); exit(1);}

  char *tar[8];
  tar[1] = "Qweak Target: LH2";
  tar[2] = "Qweak Target: 4% US Al";
  tar[3] = "Qweak Target: 2% US Al";
  tar[4] = "Qweak Target: 1% US Al";
  tar[5] = "Qweak Target: 4% DS Al";
  tar[6] = "Qweak Target: 2% DS Al";
  tar[7] = "Qweak Target: 1% DS Al";
  tar[8] = "Qweak Target: US Pure Al";

  /* load the different Trees. */
  TTree* tm = (TTree*)f->Get("Mps_Tree");
  if(tm == NULL) {printf("%sUnable to find %sMps_Tree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}
  TTree* th = (TTree*)f->Get("Hel_Tree");
  if(th == NULL) {printf("%sUnable to find %sHel_Tree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}
  TTree* ts = (TTree*)f->Get("Slow_Tree");
  if(ts == NULL) {printf("%sUnable to find %sSlow_Tree%s. Exiting the program!%s\n",blue,red,blue,normal); exit(1);}

  char *s1 = "ErrorFlag==0 && mps_counter<99000 && yield_ramp<0";
  char *s2 = "Device_Error_Code==0";

  Double_t fit_range[2] = { -0.02, 0.02};
  TF1* fitfunx = new TF1("fitfunx","pol1",fit_range[0],fit_range[1]);
  TF1* fitfuny = new TF1("fitfuny","pol1",fit_range[0],fit_range[1]);
  Double_t current,raster,cal_mdalla,cal_bcmdd,cal_abcm,cal_abcmm,cal_mdxsen,cal_mdysen,cal_emdxsen,cal_emdysen,mean_charge;
  TCanvas *c0 = new TCanvas("c0","c0",340,340,100,100);
  c0->cd();
  //   ts->Draw("ibcm1>>bcm1","","goff");bcm1->Draw("goff");
  ts->Draw("EHCFR_LIXWidth>>rasterx","","goff");rasterx->Draw("goff");
  th->Draw("yield_qwk_charge>>cur",Form("%s && yield_qwk_charge.%s",s1,s2),"goff");cur->Draw("goff");
  th->Draw("(asym_qwk_bcm1-asym_qwk_bcm2)*1e6>>bcmdd",Form("%s && asym_qwk_bcm1.%s && asym_qwk_bcm2.%s",s1,s2,s2),"goff");bcmdd->Draw("goff");
  th->Draw("asym_qwk_charge*1e6>>abcm",Form("%s && asym_qwk_charge.%s",s1,s2),"goff");abcm->Draw("goff");
  th->Draw("asym_qwk_mdallbars*1e6>>mdalla",Form("%s && asym_qwk_mdallbars.%s",s1,s2),"goff");mdalla->Draw("goff");
  th->Draw("asym_qwk_mdallbars*1e6:diff_qwk_bpm3h09bX>>mdxsen",Form("%s && asym_qwk_mdallbars.%s && diff_qwk_bpm3h09bX.%s",s1,s2,s2),"prof");mdxsen->Draw("");mdxsen->Fit("fitfunx","E M R F Q","",-0.02,0.02);
  th->Draw("asym_qwk_mdallbars*1e6:diff_qwk_bpm3h09bY>>mdysen",Form("%s && asym_qwk_mdallbars.%s && diff_qwk_bpm3h09bY.%s",s1,s2,s2),"prof");mdysen->Draw("");mdysen->Fit("fitfuny","E M R F Q","",-0.02,0.02);
//   tm->Draw("qwk_charge:event_number>>histocrg", "ErrorFlag == 0 && qwk_charge.Device_Error_Code==0");
//   mean_charge = histocrg->GetMean(2);
//   charge = Form("qwk_charge > %f", 0.99*mean_charge);
//   yield_charge = Form("qwk_charge >= %f", 0.98*mean_charge);

  current=cur->GetMean();raster=rasterx->GetMean();cal_mdalla=mdalla->GetRMS();cal_bcmdd=bcmdd->GetRMS();cal_abcm=abcm->GetRMS();cal_abcmm=abcm->GetMean();cal_mdxsen=fitfunx->GetParameter(1);cal_mdysen=fitfuny->GetParameter(1);cal_emdxsen=fitfunx->GetParError(1);cal_emdysen=fitfuny->GetParError(1);

  c0->Update();

  const Int_t NUM = 8, NUM1 = 5,NUM2 = 6, NUM3 = 3, NUM4 = 4;
  TString bcms[NUM1] = {"charge","bcm1","bcm2","bcm5","bcm6"};
  TString bcms2[NUM2] = {"bcm1","bcm1","bcm1","bcm2","bcm2","bcm5"};
  TString bcms3[NUM2] = {"bcm2","bcm5","bcm6","bcm5","bcm6","bcm6"};
  TString bpms[NUM4] = {"target","bpm3h09b","bpm3h07c","bpm3c12"};
  TString mdasym[NUM] = {"md1barsum","md2barsum","md3barsum","md4barsum","md5barsum","md6barsum","md7barsum","md8barsum"};
  TString mdbkg[NUM2] = {"md9pos","md9neg","pmtltg","pmtled","preamp","pmtonl"};
  TString allmd[NUM3] = {"all","even","odd"};
  TString lumi[NUM3] = {"sum","even","odd"};
  TString uslumi[NUM1] = {"uslumi1","uslumi3","uslumi5","uslumi7","uslumi"};
  TString bmod[NUM2] = {"ramp","fgx1","fgx2","fge","fgy1","fgy2"};

  char pcharge[255],pchargeasym[255],pchargeddasym[255],pbpmd[255],pmdyield[255],pmdasym[255],pmdallasym[255],pmdallsenx[255],pmdallseny[255],pdslumiyield[255],pdslumiasym[255],plumisenx[255],plumiseny[255],pmodulation[255],puslumi[255],puslumisen[255],pmdyieldvar[255],pmdbkg[255],psenbmod[255];
  sprintf(pcharge,"%s/%dCharge.png",dir[0],run_number);
  sprintf(pchargeasym,"%s/%dChargeAsym.png",dir[0],run_number);
  sprintf(pchargeddasym,"%s/%dChargeDDAsym.png",dir[0],run_number);
  sprintf(pbpmd,"%s/%dBPMDiff.png",dir[0],run_number);
  sprintf(pmdyield,"%s/%dMDPMTYield.png",dir[0],run_number);
  sprintf(pmdyieldvar,"%s/%dMDYieldVariation.png",dir[0],run_number);
  sprintf(pmdasym,"%s/%dMDAsym.png",dir[0],run_number);
  sprintf(pmdallasym,"%s/%dMDBCMAsym.png",dir[0],run_number);
  sprintf(pmdbkg,"%s/%dMDBackground.png",dir[0],run_number);
  sprintf(pmodulation,"%s/%dModulationCycle.png",dir[0],run_number);
  sprintf(psenbmod,"%s/%dModulationSensitivity.png",dir[0],run_number);
  sprintf(pmdallsenx,"%s/%dMDXSensitivity.png",dir[0],run_number);
  sprintf(pmdallseny,"%s/%dMDYSensitivity.png",dir[0],run_number);
  sprintf(pdslumiyield,"%s/%dDSLumiYield.png",dir[0],run_number);
  sprintf(pdslumiasym,"%s/%dDSLumiAsym.png",dir[0],run_number);
  sprintf(plumisenx,"%s/%dLumiXSensitivity.png",dir[0],run_number);
  sprintf(plumiseny,"%s/%dLumiYSensitivity.png",dir[0],run_number);
  sprintf(puslumi,"%s/%dUSLumiYieldAsym.png",dir[0],run_number);
  sprintf(puslumisen,"%s/%dUSLumiSensitivity.png",dir[0],run_number);

  char txtcharge[255],txtchargeasym[255];
  char comontext[255],txtcharge1[255],txtchargeasym1[255];
  sprintf(txtcharge,"Run %d: Charge (uA)",run_number);
  sprintf(txtchargeasym,"Run %d: Charge Asymmetry (ppm)",run_number);
  //   sprintf(txtchargeddasym,"Run %d: Charge Double Difference Asymmetry (ppm)",run_number);
  //   sprintf(txtbpmd,"Run %d: BPM Difference (mm)",run_number);
  //   sprintf(txtmdyield,"%s/%dMDPMTYield",run_number);
  //   sprintf(txtmdasym,"Run %d: Main Detector Barsum Asymmetries (ppm)",run_number);
  //   sprintf(txtmdallasym,"%dMDAllbarsumAsym",run_number);
  //   sprintf(txtmdallsen,"%dMDAllbarsumSensitivity",run_number);
  //   sprintf(txtdslumiyield,"Run %d: Downstream Lumi Yields (Volts/uA)",run_number);
  //   sprintf(txtdslumiasym,"Run %d: Downstream Lumi Asymmetries (ppm)",run_number);

  sprintf(comontext,"for %s, %2.1f uA, %2.1fx%2.1f mm",tar[1],current,raster,raster);
  sprintf(txtcharge1,"%s %s",txtcharge,comontext);
  sprintf(txtchargeasym1,"%s %s",txtchargeasym,comontext);
  /************************************************************************/
    TString nodatax = "NO Modulation for X"; TString nodataxp = "NO Modulation for X Angle"; TString nodatae = "NO Modulation for E"; TString nodatay = "NO Modulation for Y"; TString nodatayp = "NO Modulation for Y Angle";
    tnodatax = new TText(0.40,0.40,nodatax);tnodataxp = new TText(0.40,0.40,nodataxp);tnodatae = new TText(0.40,0.40,nodatae);tnodatay = new TText(0.40,0.40,nodatay);tnodatayp = new TText(0.40,0.40,nodatayp);
    tnodatax->SetTextSize(0.15);tnodataxp->SetTextSize(0.15);tnodatae->SetTextSize(0.15);tnodatay->SetTextSize(0.15);tnodatayp->SetTextSize(0.15);
  /************************************************************************/
  Double_t csizx,csizy,csiz3x,csiz3y,cx1,cy1,cx2,cx3,tsiz,tsiz3,tll,tlr,ps1,ps2,ps3,ps4;
  csizx=1200;csizy=1000;csiz3x=1100;csiz3y=780;tsiz=0.40;tsiz3=0.45;tll=0.012;tlr=0.4;cx1=0;cy1=0;cx2=300;cx3=600;
  ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;
  /************************************************************************/
  int position1[5] = { 1, 3, 4, 5, 6 };
  int position2[6] = { 1, 2, 3, 4, 5, 6 };
  int position3[4] = { 1, 3, 5, 7 };
  int position4[4] = { 2, 4, 6, 8 };
  int position5[9] = { 0, 4, 1, 2, 3, 6, 9, 8, 7 };
  int position6[9] = { 0, 5, 1, 2, 3, 7, 11, 10, 9 };
  int position7[3] = { 4, 8, 12 };
  int position8[9] = { 0, 7, 1, 2, 3, 9, 15, 14, 13 };
  int position9[9] = { 0, 10, 4, 5, 6, 12, 18, 17, 16 };
  int position10[5] = { 7, 2, 9, 14, 8 };
  int position11[5] = { 10, 5, 12, 17, 11 };
  int position12[5] = { 4, 2, 6, 8, 5 };
  int position13[6] = { 1, 2, 5, 6, 9, 10 };
  int position14[6] = { 3, 4, 7, 8, 11, 12 };
  /****************************************************************************/
  char ycharge[255],cutycharge[255],acharge[255],cutacharge[255],ddcharge[255],cutddcharge[255],bpmx[255],cutbpmx[255],bpmy[255],cutbpmy[255],adet[255],cutadet[255],ydetvar[255],cutydetvar[255],ylumi[255],cutylumi[255],ylumis[255],cutylumis[255],alumi[255],cutalumi[255],alumis[255],cutalumis[255],ypmtpos[255],ypmtneg[255],cutypmtpos[255],cutypmtneg[255],adetsenx[255],adetseny[255],cutadetsenx[255],cutadetseny[255],alldetsenx[255],alldetseny[255],adetsenxp[255],adetsenyp[255],cutalldetsenx[255],cutalldetseny[255],cutadetsenxp[255],cutadetsenyp[255],histox[255],histoy[255],histoxp[255],histoyp[255],hyaxis[255],modulation[255],cutmodulation[255],hbmod[255],ybmod[255],xbmod[255],tbmod[255],yuslumis[255],cutyuslumis[255],auslumis[255],cutauslumis[255];  
  /****************************************************************************/
  char adetsenxf[255],adetsenx[255],histoxf[255],histox[255],cutadetsenxf[255],cutadetsenx[255],yaxis[255],xaxis[255],adetasenxf[255],adetasenx[255],histoxfa[255],histoxa[255],cutadetasenxf[255],cutadetasenx[255],yaxisa[255],xaxisa[255],adetsenyf[255],adetseny[255],histoyf[255],histoy[255],cutadetsenyf[255],cutadetseny[255],yaxis1[255],xaxis1[255],adetasenyf[255],adetaseny[255],histoyfa[255],histoya[255],cutadetasenyf[255],cutadetaseny[255],yaxisa1[255],xaxisa1[255];
  char alumisenxf[255],alumisenx[255],histolxf[255],histolx[255],cutalumisenxf[255],cutalumisenx[255],yaxisl[255],xaxisl[255],alumiasenxf[255],alumiasenx[255],histolxfa[255],histolxa[255],cutalumiasenxf[255],cutalumiasenx[255],yaxisla[255],xaxisla[255],alumisenyf[255],alumiseny[255],histolyf[255],histoly[255],cutalumisenyf[255],cutalumiseny[255],yaxisl1[255],xaxisl1[255],alumiasenyf[255],alumiaseny[255],histolyfa[255],histolya[255],cutalumiasenyf[255],cutalumiaseny[255],yaxisla1[255],xaxisla1[255],auslumisenx[255],cutauslumisenx[255],histouslx[255],auslumiseny[255],cutauslumiseny[255],histously[255],yaxisusl[255],hpmtpos[255],hpmtneg[255];

  char tarbitrary[255],txdetvar[255];
  sprintf(tarbitrary,"Counts [Arbitrary Units]");
  sprintf(txdetvar,"TIME [s]");
  /************************************************************************/
  gStyle->Reset();
  gStyle->SetTitleYOffset(1.3);
  gStyle->SetTitleXOffset(1.1);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetStatFontSize(0.12);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gPad->SetFillColor(kWhite);
  //   gStyle->SetOptStat(1);
  //   gStyle->SetOptFit(1111);
  //    gStyle->SetOptStat(1);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.42);
  gStyle->SetStatW(0.42);
  gStyle->SetStatH(0.36); 
  gStyle->SetCanvasColor(kRed-10);
  /************************************************************************/
  if (MDPMT){
  TCanvas *c15 = new TCanvas("c15",Form("Run %d: Main Detector PMT Yields (Volts/uA)",run_number),0,0,1580,1000);
  pad150 = new TPad("pad150","pad150",ps1,ps2,ps4,ps4);
  pad151 = new TPad("pad151","pad151",ps1,ps1,ps4,ps3);
  pad150->SetFillColor(kRed-10);
  pad150->Draw();
  pad151->SetFillColor(kWhite);
  pad151->Draw();
  pad150->cd();
  TString text = Form("Run %d: Main Detector PMT Yields (Volts/uA) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct150 = new TText(0.012,0.4,text);
  ct150->SetTextSize(0.3);
  TString mdpos = "MD POSITIVE";
  TString mdneg = "MD NEGATIVE";
  ctp = new TText(0.30,0.45,mdpos);
  ctn = new TText(0.30,0.45,mdneg);
  ctp->SetTextSize(0.075);
  ctn->SetTextSize(0.075);
  ct150->Draw();
  pad151->cd();
  pad151->Divide(6,3);
  printf("%sPlotting %sMain Detector PMT Yields (Volts/uA)%s\n",blue,red,normal);
  /************************************************************************/

  pad151->cd(8);
  ctp->Draw();
  pad151->cd(11);
  ctn->Draw();

  int pd = 0;
  for ( int pt=0; pt<8; pt++) {
    pd = pt+1;

    sprintf(ypmtpos,"yield_qwk_md%dpos>>hpmtp%d",pd,pd);
    sprintf(ypmtneg,"yield_qwk_md%dneg>>hpmtn%d",pd,pd);
    sprintf(cutypmtpos,"%s && yield_qwk_md%dpos.%s",s1,pd,s2);
    sprintf(cutypmtneg,"%s && yield_qwk_md%dneg.%s",s1,pd,s2);
    sprintf(hpmtpos,"hpmtp%d",pd);
    sprintf(hpmtneg,"hpmtn%d",pd);

    pad151->cd(position8[pd]);
    th->Draw(ypmtpos,cutypmtpos,"goff");
    histpmtp = (TH1F *)gDirectory->Get(hpmtpos);
    histpmtp->SetLineColor(kRed);
    histpmtp->SetYTitle(tarbitrary);
    histpmtp->SetXTitle(Form("MD PMT %d POS YIELD [V/uA]",pd));
    histpmtp->SetTitle(Form("MD PMT %d POS YIELD ",pd));
    histpmtp->Draw("");

    pad151->cd(position9[pd]);
    th->Draw(ypmtneg,cutypmtneg,"goff");
    histpmtn = (TH1F *)gDirectory->Get(hpmtneg);
    histpmtn->SetLineColor(kBlack);
    histpmtn->SetYTitle(tarbitrary);
    histpmtn->SetXTitle(Form("MD PMT %d NEG YIELD [V/uA]",pd));
    histpmtn->SetTitle(Form("MD PMT %d NEG YIELD ",pd));
    histpmtn->Draw("");

    gPad->Update();
  }
  c15->Update(); c15->SaveAs(pmdyield);}

//   TString in;
//   cout <<"Insert q to exit"<<endl;
//   cin >> in;
//   if(in == "q") exit(-1);

  /****************************************************************************/
  gStyle->SetStatFontSize(0.060);
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25); 
  gStyle->SetCanvasColor(kCyan-10);
  if (CHARGE){
  /****************************************************************************/
  TCanvas *c1 = new TCanvas("c1",Form("Run %d: Charge (uA)",run_number),0,0,csizx,1000);
  pad10 = new TPad("pad10","pad10",ps1,ps2,ps4,ps4);
  pad11 = new TPad("pad11","pad11",ps1,ps1,ps4,ps3);
  pad10->SetFillColor(kCyan-10);
  pad11->SetFillColor(kWhite);
  pad10->Draw();
  pad11->Draw();
  pad10->cd();
  TString text = (txtcharge1);
  ct10 = new TText(0.012,0.4,text);
  ct10->SetTextSize(0.3);
  ct10->Draw();
  pad11->cd();
  pad11->Divide(2,3);
  printf("%sPlotting %sCharge (uA)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  TCanvas *c2 = new TCanvas("c2",Form("Run %d: Charge Asymmetry (ppm)",run_number),20,20,csizx,1000);
  pad20 = new TPad("pad20","pad20",ps1,ps2,ps4,ps4);
  pad21 = new TPad("pad21","pad21",ps1,ps1,ps4,ps3);
  pad20->SetFillColor(kCyan-10);
  pad21->SetFillColor(kWhite);
  pad20->Draw();
  pad21->Draw();
  pad20->cd();
  TString text = (txtchargeasym1);
  ct20 = new TText(0.012,0.4,text);
  ct20->SetTextSize(0.3);
  ct20->Draw();
  pad21->cd();
  pad21->Divide(2,3);
  printf("%sPlotting %sCharge Asymmetry (ppm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  char hycharge[255],hacharge[255],tycharge[255], tacharge[255], hddcharge[255];
  for ( int i=0; i<5; i++) {

    sprintf(ycharge,"yield_qwk_%s>>hycrg%d",bcms[i].Data(),i);
    sprintf(acharge,"asym_qwk_%s*1e6>>hacrg%d",bcms[i].Data(),i);
    sprintf(cutycharge,"%s && yield_qwk_%s.%s",s1,bcms[i].Data(),s2);
    sprintf(cutacharge,"%s && asym_qwk_%s.%s",s1,bcms[i].Data(),s2);
    sprintf(hycharge,"hycrg%d",i);
    sprintf(hacharge,"hacrg%d",i);
    sprintf(tycharge,"%s YIELD [uA]",bcms[i].Data());
    sprintf(tacharge,"%s ASYM [ppm]",bcms[i].Data());

    pad11->cd(position1[i]);
    th->Draw(ycharge,cutycharge);
    histycrg = (TH1F *)gDirectory->Get(hycharge);
    histycrg->SetYTitle(tarbitrary);
    histycrg->SetXTitle(tycharge);
    histycrg->SetTitle(tycharge);
    histycrg->Draw();
    gPad->Update();
   
    pad21->cd(position1[i]);
    th->Draw(acharge,cutacharge);
    histacrg = (TH1F *)gDirectory->Get(hacharge);
    histacrg->SetYTitle(tarbitrary);
    histacrg->SetXTitle(tacharge);
    histacrg->SetTitle(tacharge);
    histacrg->Draw();

    gPad->Update();   
  }
    pad11->cd(2);
    th->Draw("yield_qwk_charge:mps_counter*0.001>>hycharge2",Form("%s && yield_qwk_charge.%s",s1,s2));
    hycharge2->SetYTitle("Charge YIELD [uA]");
    hycharge2->SetXTitle(txdetvar);
    hycharge2->SetTitle("Charge YIELD with TIME");
    hycharge2->Draw();
    pad21->cd(2);
    th->Draw("asym_qwk_charge*1e6:pat_counter>>hacharge2",Form("%s && asym_qwk_charge.%s",s1,s2));
    hacharge2->SetYTitle("Charge ASYM [ppm]");
    hacharge2->SetXTitle("PATTERN COUNTER");
    hacharge2->SetTitle("Charge ASYM with PATTERN");
    hacharge2->Draw();

  c1->Update(); c1->SaveAs(pcharge);
  c2->Update(); c2->SaveAs(pchargeasym);}

  /****************************************************************************/
  if (CHARGEDD){
  TCanvas *c3 = new TCanvas("c3", Form("Run %d: Charge Double Difference (ppm)",run_number),40,40,csizx,1000);
  pad30 = new TPad("pad30","pad30",ps1,ps2,ps4,ps4);
  pad31 = new TPad("pad31","pad31",ps1,ps1,ps4,ps3);
  pad30->SetFillColor(kCyan-10);
  pad31->SetFillColor(kWhite);
  pad30->Draw();
  pad31->Draw();
  pad30->cd();
  TString text = Form("Run %d: Charge Double Difference Asymmetry (ppm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct30 = new TText(0.012,0.4,text);
  ct30->SetTextSize(0.3);
  ct30->Draw();
  pad31->cd();
  pad31->Divide(2,3);
  printf("%sPlotting %sCharge Double Difference (ppm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  gStyle->SetOptStat(1);
  gStyle->SetOptStat("emr");

  for ( int j=0; j<6; j++) {

    sprintf(ddcharge,"asym_qwk_%s*1e6-asym_qwk_%s*1e6>>hdd%d",bcms2[j].Data(),bcms3[j].Data(),j);
    sprintf(cutddcharge,"%s && asym_qwk_%s.%s  && asym_qwk_%s.%s",s1,bcms2[j].Data(),s2,bcms3[j].Data(),s2);
    sprintf(hddcharge,"hdd%d",j);

    pad31->cd(position2[j]);
    th->Draw(ddcharge,cutddcharge);
    hddcrg = (TH1F *)gDirectory->Get(hddcharge);
    hddcrg->SetYTitle(tarbitrary);
    hddcrg->SetXTitle(Form("%s - %s ASYM [ppm]",bcms2[j].Data(),bcms3[j].Data()));
    hddcrg->SetTitle(Form("%s - %s ASYM",bcms2[j].Data(),bcms3[j].Data()));
    hddcrg->Draw();
    gPad->Update();
  }
  c3->Update(); c3->SaveAs(pchargeddasym);}

    gStyle->SetTitleSize(0.09);
    gStyle->SetTitleYSize(0.09);
    gStyle->SetTitleXSize(0.075);
    gStyle->SetTitleYOffset(0.3);
    gStyle->SetTitleXOffset(0.6);
    gStyle->SetLabelSize(0.085);
    gStyle->SetLabelSize(0.085,"y");
    gStyle->SetLabelSize(0.085,"x");
    gStyle->SetCanvasColor(kOrange-9);
  if (BMODCYCLE){
  /****************************************************************************/
    TCanvas *c13 = new TCanvas("c13", Form("Run %d: Modulation Cycle",run_number),120,120,csizx,1000);
    pad130 = new TPad("pad130","pad130",ps1,ps2,ps4,ps4);
    pad131 = new TPad("pad131","pad131",ps1,ps1,ps4,ps3);
    pad130->SetFillColor(kOrange-9);
    pad131->SetFillColor(kWhite);
    pad130->Draw();
    pad131->Draw();
    pad130->cd();
    TString text = Form("Run %d: Modulation Cycle for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
    ct130 = new TText(0.012,0.4,text);
    ct130->SetTextSize(0.3);
    ct130->Draw();
    pad131->cd();
    pad131->Divide(1,6);
    printf("%sPlotting %sModulation Cycle%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
    gStyle->SetOptStat(0);
    TString subblock0 = "((ramp.block3+ramp.block0)-(ramp.block2+ramp.block1)) <  0.5e2";
    TString subblock1 = "((ramp.block3+ramp.block0)-(ramp.block2+ramp.block1)) > -0.5e2";
    TString error = "ErrorFlag == 0 && Device_Error_Code == 0";
    TString cut = Form("%s && %s && %s && event_number*0.001<99", subblock0.Data(), subblock1.Data(), error.Data());
    Double_t bmod_r[2] = { -0.4, 0.4};
    Double_t bmod_bin[1] = {250};
    TH2D * histe  = new TH2D("histe", "histe", bmod_bin[0], 0, 99, bmod_bin[0],bmod_r[0],bmod_r[1]);
    TH2D * histx1 = new TH2D("histx1", "histx1", bmod_bin[0], 0, 99, bmod_bin[0],bmod_r[0],bmod_r[1]);
    TH2D * histx2 = new TH2D("histx2", "histx2", bmod_bin[0], 0, 99, bmod_bin[0],bmod_r[0],bmod_r[1]);
    TH2D * histy1 = new TH2D("histy1", "histy1", bmod_bin[0], 0, 99, bmod_bin[0],bmod_r[0],bmod_r[1]);
    TH2D * histy2 = new TH2D("histy2", "histy2", bmod_bin[0], 0, 99, bmod_bin[0],bmod_r[0],bmod_r[1]);
    TH2D * histramp = new TH2D("histramp", "histramp", bmod_bin[0], 0, 99, bmod_bin[0],bmod_r[0],bmod_r[1]);

    pad131->cd(1);
    tm->Draw("(76.29e-6)*ramp:event_number*0.001>>histramp", cut);
    histramp = (TH2D *)gDirectory->Get("histramp");
    histramp->SetXTitle("TIME [s]");
    histramp->SetYTitle("Ramp [V]");
    histramp->GetYaxis()->SetRangeUser(-0.4, 0.4);
    histramp->Draw("");
    gPad->Update();

    pad131->cd(2);
    tm->Draw("(76.29e-6)*fgx1:event_number*0.001>>histx1", cut);
    histx1 = (TH2D *)gDirectory->Get("histx1");
    histx1->SetXTitle("TIME [s]");
    histx1->SetYTitle("FGX1 [V]");
    histx1->GetYaxis()->SetRangeUser(-0.4, 0.4);
    histx1->SetMarkerColor(kRed-4);
    histx1->Draw("");
    gPad->Update();

    pad131->cd(3);
    tm->Draw("(76.29e-6)*fgx2:event_number*0.001>>histx2", cut);
    histx2 = (TH2D *)gDirectory->Get("histx2");
    histx2->SetXTitle("TIME [s]");
    histx2->SetYTitle("FGX2 [V]");
    histx2->GetYaxis()->SetRangeUser(-0.4, 0.4);
    histx2->SetMarkerColor(kRed-4);
    histx2->Draw("");
    gPad->Update();

    pad131->cd(4);
    tm->Draw("(76.29e-6)*fge:event_number*0.001>>histe", cut);
    histe = (TH2D *)gDirectory->Get("histe");
    histe->SetXTitle("TIME [s]");
    histe->SetYTitle("FGE [V]");
    histe->GetYaxis()->SetRangeUser(-0.4, 0.4);
    histe->SetMarkerColor(kBlue);
    histe->Draw("");
    gPad->Update();

    pad131->cd(5);
    tm->Draw("(76.29e-6)*fgy1:event_number*0.001>>histy1", cut);
    histy1 = (TH2D *)gDirectory->Get("histy1");
    histy1->SetXTitle("TIME [s]");
    histy1->SetYTitle("FGY1 [V]");
    histy1->GetYaxis()->SetRangeUser(-0.4, 0.4);
    histy1->SetMarkerColor(kGreen-3);
    histy1->Draw("");
    gPad->Update();

    pad131->cd(6);
    tm->Draw("(76.29e-6)*fgy2:event_number*0.001>>histy2", cut);
    histy2 = (TH2D *)gDirectory->Get("histy2");
    histy2->SetXTitle("TIME [s]");
    histy2->SetYTitle("FGY2 [V]");
    histy2->GetYaxis()->SetRangeUser(-0.4, 0.4);
//     histy2->GetXaxis()->SetTitleColor(2);
//     histy2->GetYaxis()->SetTitleColor(2);
    histy2->SetMarkerColor(kGreen-3);
    histy2->Draw("");
    gPad->Update();

    c13->Update(); c13->SaveAs(pmodulation);}

    gStyle->SetTitleSize(0.055);
    gStyle->SetTitleYSize(0.055);
    gStyle->SetTitleXSize(0.055);
    gStyle->SetTitleYOffset(0.8);
    gStyle->SetTitleXOffset(0.8);
    gStyle->SetLabelSize(0.045);
    gStyle->SetLabelSize(0.045,"y");
    gStyle->SetLabelSize(0.052,"x");
    gStyle->SetOptStat(1);
    gStyle->SetOptStat("emr");
  /****************************************************************************/
  if (BPMS){
    TCanvas *c4 = new TCanvas("c4", Form("Run %d: BPM Difference (mm)",run_number),60,60,csizx,1000);
  pad40 = new TPad("pad40","pad40",ps1,ps2,ps4,ps4);
  pad41 = new TPad("pad41","pad41",ps1,ps1,ps4,ps3);
  pad40->SetFillColor(kOrange-9);
  pad41->SetFillColor(kWhite);
  pad40->Draw();
  pad41->Draw();
  pad40->cd();
  TString text = Form("Run %d: BPM Difference (mm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct40 = new TText(0.012,0.4,text);
  ct40->SetTextSize(0.3);
  ct40->Draw();
  pad41->cd();
  pad41->Divide(2,4);
  printf("%sPlotting %sBPM Difference (mm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  char hdiffbpmx[255],hdiffbpmy[255],tdiffbpmx[255],tdiffbpmx[255];
  for ( int k=0; k<4; k++) {

    sprintf(bpmx,"diff_qwk_%sX>>difx%d",bpms[k].Data(),k);
    sprintf(bpmy,"diff_qwk_%sY>>dify%d",bpms[k].Data(),k);
    sprintf(cutbpmx,"%s && diff_qwk_%sX.%s",s1,bpms[k].Data(),s2);
    sprintf(cutbpmy,"%s && diff_qwk_%sY.%s",s1,bpms[k].Data(),s2);
    sprintf(hdiffbpmx,"difx%d",k);
    sprintf(hdiffbpmy,"dify%d",k);

    pad41->cd(position3[k]);
    th->Draw(bpmx,cutbpmx,"goff");
    hdiffx = (TH1F *)gDirectory->Get(hdiffbpmx);
    hdiffx->SetYTitle(tarbitrary);
    hdiffx->SetXTitle(Form("%s-X [mm]",bpms[k].Data()));
    hdiffx->SetTitle(Form("%s-X",bpms[k].Data()));
    hdiffx->SetLineColor(kRed);
    hdiffx->Draw();

    pad41->cd(position4[k]);
    th->Draw(bpmy,cutbpmy,"goff");
    hdiffy = (TH1F *)gDirectory->Get(hdiffbpmy);
    hdiffy->SetYTitle(tarbitrary);
    hdiffy->SetXTitle(Form("%s-Y [mm]",bpms[k].Data()));
    hdiffy->SetTitle(Form("%s-Y",bpms[k].Data()));
    hdiffy->SetLineColor(kGreen);
    hdiffy->Draw();

    gPad->Update();
  }
  c4->Update(); c4->SaveAs(pbpmd);}

  gStyle->SetTitleSize(0.04);
  gStyle->SetTitleYSize(0.04);
  gStyle->SetTitleXSize(0.04);
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.1);
  gStyle->SetLabelSize(0.050);
  gStyle->SetLabelSize(0.050,"y");
  gStyle->SetLabelSize(0.050,"x");
  gStyle->SetCanvasColor(kRed-10);
  /****************************************************************************/
  if (MDYIELDVAR){
  TCanvas *c14 = new TCanvas("c14", Form("Run %d: Main Detector Barsum Yield Variation",run_number),90,90,csizx,1000);
  pad140 = new TPad("pad140","pad140",ps1,ps2,ps4,ps4);
  pad141 = new TPad("pad141","pad141",ps1,ps1,ps4,ps3);
  pad140->SetFillColor(kRed-10);
  pad141->SetFillColor(kWhite);
  pad140->Draw();
  pad141->Draw();
  pad140->cd();
  TString text = Form("Run %d: Main Detector Barsum Yield Variation for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct140 = new TText(0.012,0.4,text);
  ct140->SetTextSize(0.3);
  ct140->Draw();
  pad141->cd();
  pad141->Divide(3,3);
  printf("%sPlotting %sMain Detector Barsum Yield Variation%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  char hadet[255],hdetvar[255],hadetall[255], txadet[255], tyadet[255], txalladet[255],tydetvar[255],tyalldetvar[255];
  Double_t mdt_varr[2] = { -0.0300, 0.0450};
 
  sprintf(tyalldetvar,"MD All YIELD");
  int ld=0;
  for ( int l=0; l<8; l++) {
    ld = l+1;

    sprintf(ydetvar,"yield_qwk_md%dbarsum:mps_counter*0.001>>hyvar%d",ld,ld);
    sprintf(cutydetvar,"%s && yield_qwk_md%dbarsum.%s",s1,ld,s2);
    sprintf(hdetvar,"hyvar%d",ld);
    sprintf(tydetvar,"MD %d YIELD",ld);

    pad141->cd(position5[ld]);
    pad141->cd(position5[ld])->SetGrid();
    th->Draw(ydetvar,cutydetvar,"goff");
    hyvar = (TH2F *)gDirectory->Get(hdetvar);
    hyvar->SetYTitle(tydetvar);
    hyvar->SetXTitle(txdetvar);
    hyvar->SetTitle(tydetvar);
    hyvar->Draw();
    gPad->Update();
  }

    pad141->cd(5);
    pad141->cd(5)->SetGrid();
    th->Draw("yield_qwk_mdallbars:mps_counter*0.001>>hvarall",Form("%s && yield_qwk_mdallbars.%s",s1,s2));
    hvarall->SetFillColor(kRed-2);
    hvarall->SetMarkerColor(kRed-2);
//     hvarall->SetFillStyle(3013);
    hvarall->SetYTitle(tyalldetvar);
    hvarall->SetXTitle(txdetvar);
    hvarall->SetTitle(tyalldetvar);
    hvarall->Draw();

    gPad->Update();
    c14->Update(); c14->SaveAs(pmdyieldvar);}
  /****************************************************************************/
  if (MDBKG){
  TCanvas *c18 = new TCanvas("c18", Form("Run %d: Main Detector Background Yields[V/uA] & Asymmetries [ppm]",run_number),100,100,csizx,1000);
  pad180 = new TPad("pad180","pad180",ps1,ps2,ps4,ps4);
  pad181 = new TPad("pad181","pad181",ps1,ps1,ps4,ps3);
  pad180->SetFillColor(kRed-10);
  pad181->SetFillColor(kWhite);
  pad180->Draw();
  pad181->Draw();
  pad180->cd();
  TString text = Form("Run %d: Main Detector Background Yields[V/uA] & Asymmetries [ppm] for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct180 = new TText(0.012,0.4,text);
  ct180->SetTextSize(0.3);
  ct180->Draw();
  pad181->cd();
  pad181->Divide(4,3);
  printf("%sPlotting %sMain Detector Background Yields[V/uA] & Asymmetries [ppm]%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  char amdbkg[255],cutamdbkg[255],ymdbkg[255],cutymdbkg[255],hamdbkg[255],hymdbkg[255];
  int xd=0;
  for ( int x=0; x<6; x++) {
    xd = x+1;

    sprintf(amdbkg,"asym_qwk_%s*1e6>>hab%d",mdbkg[x].Data(),xd);
    sprintf(cutamdbkg,"%s && asym_qwk_%s.%s",s1,mdbkg[x].Data(),s2);
    sprintf(ymdbkg,"yield_qwk_%s>>hyb%d",mdbkg[x].Data(),xd);
    sprintf(cutymdbkg,"%s && yield_qwk_%s.%s",s1,mdbkg[x].Data(),s2);
    sprintf(hamdbkg,"hab%d",xd);
    sprintf(hymdbkg,"hyb%d",xd);

    pad181->cd(position13[x]);
    th->Draw(ymdbkg,cutymdbkg,"goff");
    hybkg = (TH1F *)gDirectory->Get(hymdbkg);
    hybkg->SetYTitle(tarbitrary);
    hybkg->SetXTitle(Form("MD %s YIELD [V/uA]",mdbkg[x].Data()));
    hybkg->SetTitle(Form("MD %s YIELD [V/uA]",mdbkg[x].Data()));
    hybkg->Draw();
    gPad->Update();

    pad181->cd(position14[x]);
    th->Draw(amdbkg,cutamdbkg,"goff");
    habkg = (TH1F *)gDirectory->Get(hamdbkg);
    habkg->SetYTitle(tarbitrary);
    habkg->SetXTitle(Form("MD %s ASYM [ppm]",mdbkg[x].Data()));
    habkg->SetTitle(Form("MD %s ASYM [ppm]",mdbkg[x].Data()));
    habkg->SetFillColor(kBlack);
    habkg->SetFillStyle(3013);
    habkg->Draw();
    gPad->Update();

  }
  c18->Update(); c18->SaveAs(pmdbkg);}
  /****************************************************************************/
  if (MDALLASYM){
  TCanvas *c6 = new TCanvas("c6", Form("Run %d: Main Detector All Barsum Asymmetry w.r.t BCMs (ppm)",run_number),110,110,csizx,1000);
  pad60 = new TPad("pad60","pad60",ps1,ps2,ps4,ps4);
  pad61 = new TPad("pad61","pad61",ps1,ps1,ps4,ps3);
  pad60->SetFillColor(kRed-10);
  pad61->SetFillColor(kWhite);
  pad60->Draw();
  pad61->Draw();
  pad60->cd();
  TString text = Form("Run %d: Main Detector All Barsum Asymmetry (ppm) Normalized to Different BCMs for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct60 = new TText(0.012,0.4,text);
  ct60->SetTextSize(0.3);
  ct60->Draw();
  pad61->cd();
  pad61->Divide(2,3);
  printf("%sPlotting %sMain Detector All Barsum Asymmetries (ppm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  Double_t mdt_r[2] = { -1700, 1700};
  Double_t mdt_bin[1] = {100};
  TH1F * hmdbcm1  = new TH1F("hmdbcm1", "hmdbcm1", mdt_bin[0],mdt_r[0],mdt_r[1]);
  TH1F * hmdbcm2  = new TH1F("hmdbcm2", "hmdbcm2", mdt_bin[0],mdt_r[0],mdt_r[1]);
  TH1F * hmdbcm5  = new TH1F("hmdbcm5", "hmdbcm5", mdt_bin[0],mdt_r[0],mdt_r[1]);
  TH1F * hmdbcm6  = new TH1F("hmdbcm6", "hmdbcm6", mdt_bin[0],mdt_r[0],mdt_r[1]);
  TH1F * hmdeven  = new TH1F("hmdeven", "hmdeven", mdt_bin[0],mdt_r[0],mdt_r[1]);
  TH1F * hmdodd  = new TH1F("hmdodd", "hmdodd", mdt_bin[0],mdt_r[0],mdt_r[1]);

  pad61->cd(1);
  gPad->SetLogy();
  th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-asym_qwk_bcm1)*1e6>>hmdbcm1",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_bcm1.%s && asym_qwk_charge.%s",s1,s2,s2,s2),"goff");
  hmdbcm1 = (TH1F *)gDirectory->Get("hmdbcm1");
  hmdbcm1->SetYTitle(tarbitrary);
  hmdbcm1->SetXTitle("MD BARSUM ASYM [ppm]");
  hmdbcm1->SetTitle("MD BARSUM ASYM normalized to BCM1");
  hmdbcm1->Draw();

  gPad->Modified();
  gPad->Update();

  pad61->cd(2);
  gPad->SetLogy();
  th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-asym_qwk_bcm2)*1e6>>hmdbcm2",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_charge.%s && asym_qwk_bcm2.%s",s1,s2,s2,s2),"goff");
  hmdbcm2 = (TH1F *)gDirectory->Get("hmdbcm2");
  hmdbcm2->SetYTitle(tarbitrary);
  hmdbcm2->SetXTitle("MD BARSUM ASYM [ppm]");
  hmdbcm2->SetTitle("MD BARSUM ASYM normalized to BCM2");
  hmdbcm2->Draw();
  gPad->Update();

  pad61->cd(3);
  gPad->SetLogy();
  th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-asym_qwk_bcm5)*1e6>>hmdbcm5",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_charge.%s && asym_qwk_bcm5.%s",s1,s2,s2,s2),"goff");
  hmdbcm5 = (TH1F *)gDirectory->Get("hmdbcm5");
  hmdbcm5->SetYTitle(tarbitrary);
  hmdbcm5->SetXTitle("MD BARSUM ASYM [ppm]");
  hmdbcm5->SetTitle("MD BARSUM ASYM normalized to BCM5");
  hmdbcm5->Draw();
  gPad->Update();

  pad61->cd(4);
  gPad->SetLogy();
  th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-asym_qwk_bcm6)*1e6>>hmdbcm6",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_charge.%s && asym_qwk_bcm6.%s",s1,s2,s2,s2),"goff");
  hmdbcm6 = (TH1F *)gDirectory->Get("hmdbcm6");
  hmdbcm6->SetYTitle(tarbitrary);
  hmdbcm6->SetXTitle("MD BARSUM ASYM [ppm]");
  hmdbcm6->SetTitle("MD BARSUM ASYM normalized to BCM6");
  hmdbcm6->Draw();
  gPad->Update();

  pad61->cd(5);
  gPad->SetLogy();
  th->Draw("asym_qwk_mdevenbars*1e6>>hmdeven",Form("%s && asym_qwk_mdevenbars.%s",s1,s2),"goff");
  hmdeven = (TH1F *)gDirectory->Get("hmdeven");
  hmdeven->SetYTitle(tarbitrary);
  hmdeven->SetXTitle("MD EVEN BAR ASYM [ppm]");
  hmdeven->SetTitle("MD EVEN BAR ASYM");
  hmdeven->Draw();
  gPad->Update();

  pad61->cd(6);
  gPad->SetLogy();
  th->Draw("asym_qwk_mdoddbars*1e6>>hmdodd",Form("%s && asym_qwk_mdoddbars.%s",s1,s2),"goff");
  hmdodd = (TH1F *)gDirectory->Get("hmdodd");
  hmdodd->SetYTitle(tarbitrary);
  hmdodd->SetXTitle("MD ODD BAR ASYM [ppm]");
  hmdodd->SetTitle("MD ODD BAR ASYM");
  hmdodd->Draw();

  gPad->Update();
  c6->Update(); c6->SaveAs(pmdallasym);}
  /************************************************************************/
  gStyle->SetStatFontSize(0.070);
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.30);
  gStyle->SetStatH(0.30); 
  /************************************************************************/
  if (SENSITIVITY){
  TCanvas *c7 = new TCanvas("c7", Form("Run %d: Main Detector All Barsum X-Sensitivities (ppm/mm)",run_number),120,120,1400,1000);
  pad70 = new TPad("pad70","pad70",ps1,ps2,ps4,ps4);
  pad71 = new TPad("pad71","pad71",ps1,ps1,ps4,ps3);
  pad70->SetFillColor(kRed-10);
  pad71->SetFillColor(kWhite);
  pad70->Draw();
  pad71->Draw();
  pad70->cd();
  TString text = Form("Run %d: Main Detector Barsum X-Sensitivities (ppm/mm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct70 = new TText(0.012,0.4,text);
  ct70->SetTextSize(0.3);
  ct70->Draw();
  pad71->cd();
  pad71->Divide(4,3);
  printf("%sPlotting %sMain Detector Barsum X-Sensitivities (ppm/mm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  TCanvas *c8 = new TCanvas("c8", Form("Run %d: Main Detector All Barsum Y-Sensitivities (ppm/mm)",run_number),140,140,1400,1000);
  pad80 = new TPad("pad80","pad80",ps1,ps2,ps4,ps4);
  pad81 = new TPad("pad81","pad81",ps1,ps1,ps4,ps3);
  pad80->SetFillColor(kRed-10);
  pad81->SetFillColor(kWhite);
  pad80->Draw();
  pad81->Draw();
  pad80->cd();
  TString text = Form("Run %d: Main Detector Barsum Y-Sensitivities (ppm/mm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct80 = new TText(0.012,0.4,text);
  ct80->SetTextSize(0.3);
  ct80->Draw();
  pad81->cd();
  pad81->Divide(4,3);
  printf("%sPlotting %sMain Detector Barsum Y-Sensitivities (ppm/mm)%s\n",blue,red,normal);
  gStyle->SetCanvasColor(kBlue-10);
  /* ----------------------------------------------------------------------- */
  TCanvas *c11 = new TCanvas("c11", Form("Run %d: Downstream Lumi X-Sensitivities (ppm/mm)",run_number),200,200,1400,1000);
  pad110 = new TPad("pad110","pad110",ps1,ps2,ps4,ps4);
  pad111 = new TPad("pad111","pad111",ps1,ps1,ps4,ps3);
  pad110->SetFillColor(kBlue-10);
  pad111->SetFillColor(kWhite);
  pad110->Draw();
  pad111->Draw();
  pad110->cd();
  TString text = Form("Run %d: Downstream Lumi X-Sensitivities (ppm/mm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct110 = new TText(0.012,0.4,text);
  ct110->SetTextSize(0.3);
  ct110->Draw();
  pad111->cd();
  pad111->Divide(4,3);
  printf("%sPlotting %sDownstream Lumi X-Sensitivities (ppm/mm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  TCanvas *c12 = new TCanvas("c12", Form("Run %d: Downstream Lumi Y-Sensitivities (ppm/mm)",run_number),220,220,1400,1000);
  pad120 = new TPad("pad120","pad120",ps1,ps2,ps4,ps4);
  pad121 = new TPad("pad121","pad121",ps1,ps1,ps4,ps3);
  pad120->SetFillColor(kBlue-10);
  pad121->SetFillColor(kWhite);
  pad120->Draw();
  pad121->Draw();
  pad120->cd();
  TString text = Form("Run %d: Downstream Lumi Y-Sensitivities (ppm/mm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct120 = new TText(0.012,0.4,text);
  ct120->SetTextSize(0.3);
  ct120->Draw();
  pad121->cd();
  pad121->Divide(4,3);
  printf("%sPlotting %sDownstream Lumi Y-Sensitivities (ppm/mm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  gStyle -> SetOptFit(1111);
  gStyle -> SetOptStat("e");

  int dt=0;
  for ( int t=0; t<8; t++) {
    dt = t+1;

    sprintf(adetsenx,"asym_qwk_md%dbarsum*1e6:diff_qwk_bpm3h09bX>>htx%d",dt,dt);
    sprintf(cutadetsenx,"%s && asym_qwk_md%dbarsum.%s && diff_qwk_bpm3h09bX.%s",s1,dt,s2,s2);
    sprintf(histox,"htx%d",dt);
    sprintf(yaxis,"MD%d ASYM [ppm]",dt);
    sprintf(xaxis,"BPM 3H09BX [mm]");

    pad71->cd(position6[dt]);
    pad71->cd(position6[dt])->SetGrid();

    th->Draw(adetsenx,cutadetsenx,"prof");
    TH1D *htxf = (TH1D*)gDirectory->Get(histox);
    htxf->GetYaxis()->SetTitle(yaxis);
    htxf->GetXaxis()->SetTitle(xaxis);
    htxf->SetTitle(yaxis);
    htxf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    htxf->SetLineColor(kRed);
    htxf->Draw("");
    gPad->Update();

    TPaveStats *st = (TPaveStats*)htxf->FindObject("stats");
    st->SetX1NDC(0.52); //new x start position
    st->SetX2NDC(0.99);    //new x end position
    st->SetY1NDC(0.72); //new y start position
    st->SetY2NDC(1);    //new y end position
    st->SetBorderSize(2);
    st->SetFillColor(19);
    st->SetTextAlign(12);
    st->Draw("same");

    gPad->Modified();
    gPad->Update();

    sprintf(adetseny,"asym_qwk_md%dbarsum*1e6:diff_qwk_bpm3h09bY>>hty%d",dt,dt);
    sprintf(cutadetseny,"%s && asym_qwk_md%dbarsum.%s && diff_qwk_bpm3h09bY.%s",s1,dt,s2,s2);
    sprintf(histoy,"hty%d",dt);
    sprintf(xaxis1,"BPM 3H09BY [mm]");

    pad81->cd(position6[dt]);
    pad81->cd(position6[dt])->SetGrid();

    th->Draw(adetseny,cutadetseny,"prof");
    TH1D *htyf = (TH1D*)gDirectory->Get(histoy);
    htyf->GetYaxis()->SetTitle(yaxis);
    htyf->GetXaxis()->SetTitle(xaxis1);
    htyf->SetTitle(yaxis);
    htyf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    htyf->SetLineColor(kGreen);
    htyf->Draw("");

    gPad->Modified();
    gPad->Update();

    pad111->cd(position6[dt]);
    pad111->cd(position6[dt])->SetGrid();

    sprintf(alumisenx,"asym_qwk_dslumi%d*1e6:diff_qwk_bpm3h09bX>>hlx%d",dt,dt);
    sprintf(cutalumisenx,"%s && asym_qwk_dslumi%d.%s && diff_qwk_bpm3h09bX.%s",s1,dt,s2,s2);
    sprintf(histolx,"hlx%d",dt);
    sprintf(yaxisl,"DSLUMI%d ASYM [ppm]",dt);

    th->Draw(alumisenx,cutalumisenx,"prof");
    TH1D *hlxf = (TH1D*)gDirectory->Get(histolx);
    hlxf->GetYaxis()->SetTitle(yaxisl);
    hlxf->GetXaxis()->SetTitle(xaxis);
    hlxf->SetTitle(yaxisl);
    hlxf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    hlxf->SetLineColor(kRed);
    hlxf->Draw("");

    gPad->Modified();
    gPad->Update();

    pad121->cd(position6[dt]);
    pad121->cd(position6[dt])->SetGrid();

    sprintf(alumiseny,"asym_qwk_dslumi%d*1e6:diff_qwk_bpm3h09bY>>hly%d",dt,dt);
    sprintf(cutalumiseny,"%s && asym_qwk_dslumi%d.%s && diff_qwk_bpm3h09bY.%s",s1,dt,s2,s2);
    sprintf(histoly,"hly%d",dt);

    th->Draw(alumiseny,cutalumiseny,"prof");
    TH1D *hlyf = (TH1D*)gDirectory->Get(histoly);
    hlyf->GetYaxis()->SetTitle(yaxisl);
    hlyf->GetXaxis()->SetTitle(xaxis1);
    hlyf->SetTitle(yaxisl);
    hlyf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    hlyf->SetLineColor(kGreen);
    hlyf->Draw("");

    gPad->Modified();
   gPad->Update();

  }
  /* ----------------------------------------------------------------------- */
  int dv=0;
  for ( int v=0; v<3; v++) {
    dv = v+1;

    pad71->cd(position7[v]);
    pad71->cd(position7[v])->SetGrid();
    // pad71->cd(position7[v])->SetFillColor(kGreen-10);

    sprintf(adetasenx,"asym_qwk_md%sbars*1e6:diff_qwk_bpm3h09bX>>h1x%d",allmd[v].Data(),v);
    sprintf(cutadetasenx,"%s && asym_qwk_md%sbars.%s && diff_qwk_bpm3h09bX.%s",s1,allmd[v].Data(),s2,s2);
    sprintf(histoxa,"h1x%d",v);
    sprintf(yaxisa,"MD %s BARS ASYM [ppm]",allmd[v].Data());
    sprintf(xaxisa,"BPM 3H09BX [mm]");

    th->Draw(adetasenx,cutadetasenx,"prof");
    TH1D *hxf = (TH1D*)gDirectory->Get(histoxa);
    hxf->GetYaxis()->SetTitle(yaxisa);
    hxf->GetXaxis()->SetTitle(xaxisa);
    hxf->SetTitle(yaxisa);
    hxf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    hxf->SetLineColor(kRed-2);
    hxf->Draw("");

    gPad->Modified();
    gPad->Update();

//     Float_t calculation;
//     if(v == 1){calculation = hxf->Fit("fitfunx","E M R F","",fit_range[0], fit_range[1]);
//       fitfunx->GetParameter(1);
//       printf("Result %f\n",calculation);}


    pad81->cd(position7[v]);
    pad81->cd(position7[v])->SetGrid();

    sprintf(adetaseny,"asym_qwk_md%sbars*1e6:diff_qwk_bpm3h09bY>>h1y%d",allmd[v].Data(),v);
    sprintf(cutadetaseny,"%s && asym_qwk_md%sbars.%s && diff_qwk_bpm3h09bY.%s",s1,allmd[v].Data(),s2,s2);
    sprintf(histoya,"h1y%d",v);
    sprintf(xaxisa1,"BPM 3H09BY [mm]");

    th->Draw(adetaseny,cutadetaseny,"prof");
    TH1D *hyf = (TH1D*)gDirectory->Get(histoya);
    hyf->GetYaxis()->SetTitle(yaxisa);
    hyf->GetXaxis()->SetTitle(xaxisa1);
    hyf->SetTitle(yaxisa);
    hyf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    hyf->SetLineColor(kGreen+2);
    hyf->Draw("");

    pad111->cd(position7[v]);
    pad111->cd(position7[v])->SetGrid();

    sprintf(alumiasenx,"asym_dslumi_%s*1e6:diff_qwk_bpm3h09bX>>hl1x%d",lumi[v].Data(),v);
    sprintf(cutalumiasenx,"%s && asym_dslumi_%s.%s && diff_qwk_bpm3h09bX.%s",s1,lumi[v].Data(),s2,s2);
    sprintf(histolxa,"hl1x%d",v);
    sprintf(yaxisla,"DSLUMI %s ASYM [ppm]",lumi[v].Data());

    th->Draw(alumiasenx,cutalumiasenx,"prof");
    TH1D *hxl = (TH1D*)gDirectory->Get(histolxa);
    hxl->GetYaxis()->SetTitle(yaxisla);
    hxl->GetXaxis()->SetTitle(xaxisa);
    hxl->SetTitle(yaxisla);
    hxl->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    hxl->SetLineColor(kRed-2);
    hxl->Draw("");

    gPad->Modified();
    gPad->Update();

    pad121->cd(position7[v]);
    pad121->cd(position7[v])->SetGrid();

    sprintf(alumiaseny,"asym_dslumi_%s*1e6:diff_qwk_bpm3h09bY>>hl1y%d",lumi[v].Data(),v);
    sprintf(cutalumiaseny,"%s && asym_dslumi_%s.%s && diff_qwk_bpm3h09bY.%s",s1,lumi[v].Data(),s2,s2);
    sprintf(histolya,"hl1y%d",v);

    th->Draw(alumiaseny,cutalumiaseny,"prof");
    TH1D *hyl = (TH1D*)gDirectory->Get(histolya);
    hyl->GetYaxis()->SetTitle(yaxisla);
    hyl->GetXaxis()->SetTitle(xaxisa1);
    hyl->SetTitle(yaxisla);
    hyl->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    hyl->SetLineColor(kGreen+2);
    hyl->Draw("");

    gPad->Modified();
    gPad->Update();

  }
  c7->Update(); c7->SaveAs(pmdallsenx);
  c8->Update(); c8->SaveAs(pmdallseny);
  c11->Update(); c11->SaveAs(plumisenx);
  c12->Update(); c12->SaveAs(plumiseny);}

  gStyle->SetTitleYSize(0.07);
  gStyle->SetTitleXSize(0.07);
  gStyle->SetTitleSize(0.07);
  gStyle->SetTitleYOffset(0.6);
  gStyle->SetTitleXOffset(0.7);
  gStyle->SetLabelSize(0.060);
  gStyle->SetLabelSize(0.060,"y");
  gStyle->SetLabelSize(0.060,"x");
  gStyle->SetCanvasColor(kOrange-9);
  gStyle->SetStatFontSize(0.070);
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.10);
  gStyle->SetStatH(0.20); 
 /****************************************************************************/
  if (BMODSEN){
    TCanvas *c19 = new TCanvas("c19", Form("Run %d: Sensitivities from Modulation [ppm/mm]",run_number),120,120,csizx,1000);
    pad190 = new TPad("pad190","pad190",ps1,ps2,ps4,ps4);
    pad191 = new TPad("pad191","pad191",ps1,ps1,ps4,ps3);
    pad190->SetFillColor(kOrange-9);
    pad191->SetFillColor(kWhite);
    pad190->Draw();
    pad191->Draw();
    pad190->cd();
    TString text = Form("Run %d: Sensitivities from Modulation [ppm/mm] for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
    ct190 = new TText(0.012,0.4,text);
    ct190->SetTextSize(0.3);
    ct190->Draw();
    pad191->cd();
    pad191->Divide(1,3);
    printf("%sPlotting %sSensitivities from Modulation [ppm/mm]%s\n",blue,red,normal);
  gStyle -> SetOptFit(1111);
  gStyle -> SetOptStat("e");

  /* ----------------------------------------------------------------------- */
    TH1D *senbmodx = NULL;TH1D *senbmodxp = NULL;TH1D *senbmode = NULL;TH1D *senbmody = NULL;TH1D *senbmodyp = NULL;
    TF1* fitbmodx = new TF1("fitbmodx","pol1",0.7,1.1);
    TF1* fitbmodxp = new TF1("fitbmodxp","pol1",0.7,1.1);
    TF1* fitbmode = new TF1("fitbmode","pol1",0.7,1.1);
    TF1* fitbmody = new TF1("fitbmody","pol1",0.7,1.1);
    TF1* fitbmodyp = new TF1("fitbmodyp","pol1",0.7,1.1);
    char *s3 = "ErrorFlag==0 && ramp>0 && abs((ramp.block3+ramp.block0)-(ramp.block2+ramp.block1))<50 && event_number>4500";

    Double_t amplitudex = 0.092,amplitudexp = 0.092, amplitudey = 0.130,amplitudeyp = 0.130, amplitudee = 0.400;
    Double_t xbmodslope1,xbmodslope2,exbmodslope1,exbmodslope2,xbmodslope,exbmodslope,xpbmodslope1,xpbmodslope2,expbmodslope1,expbmodslope2,xpbmodslope,expbmodslope,ybmodslope1,ybmodslope2,eybmodslope1,eybmodslope2,ybmodslope,eybmodslope,ypbmodslope1,ypbmodslope2,eypbmodslope1,eypbmodslope2,ypbmodslope,eypbmodslope,ebmodslope1,ebmodslope2,eebmodslope1,eebmodslope2,ebmodslope,eebmodslope,exbmodslope11,exbmodslope12,expbmodslope11,expbmodslope12,eebmodslope11,eebmodslope12,eybmodslope11,eybmodslope12,eypbmodslope11,eypbmodslope12,exbmodslope13,expbmodslope13,eebmodslope13,eybmodslope13,eypbmodslope13;

    pad191->cd(1);
    if (tm->Draw("qwk_mdallbars*1e6:qwk_targetX>>senbmodx",Form("%s && qwk_mdallbars.%s && qwk_targetX.%s && bm_pattern_number==11",s3,s2,s2),"prof goff")>0){
      senbmodx = (TH1D *)gDirectory->Get("senbmodx");
      senbmodx->SetLineColor(kRed);
      senbmodx->Draw("goff");
      fitbmodx->SetRange(senbmodx->GetMean(1) - amplitudex, senbmodx->GetMean(1) + amplitudex );
      fitbmodx->SetParameters(senbmodx->GetMean(2), 6000);
      senbmodx->Fit("fitbmodx","E M R F Q 0");
      xbmodslope1 =fitbmodx->GetParameter(1); exbmodslope1 =fitbmodx->GetParError(1);
      xbmodslope2 =fitbmodx->GetParameter(0); exbmodslope2 =fitbmodx->GetParError(0);
      exbmodslope11 = TMath::Power((exbmodslope1/xbmodslope1),2);exbmodslope12 = TMath::Power((exbmodslope2/xbmodslope2),2);
      xbmodslope = xbmodslope1/xbmodslope2;  exbmodslope13 = TMath::Sqrt(exbmodslope11 + exbmodslope12);
      exbmodslope =  exbmodslope13*xbmodslope;
      Double_t limitx = senbmodx->GetMean(2);
      senbmodx->SetYTitle("MD ALL YIELD [V/A]");
      senbmodx->SetXTitle("TARGETX [mm]");
      senbmodx->SetTitle(Form("MD ALL ASYM vs TARGETX for X-Sensitivities:%2.2f+-%2.2f [ppm/mm]",1e6*xbmodslope,1e6*exbmodslope));
      senbmodx->Draw("goff");
      fitbmodx->Draw("same");
      senbmodx->GetYaxis()->SetRangeUser(limitx*0.995,limitx*1.005);
    }
    gPad->Update();
    else{pad191->cd(1);tnodatax->Draw();}
    
//     pad191->cd(2);
//     if (tm->Draw("qwk_mdallbars*1e6:qwk_targetX>>senbmodxp",Form("%s && qwk_mdallbars.%s && qwk_targetX.%s && bm_pattern_number==14",s3,s2,s2),"prof goff")>0){
//       senbmodxp = (TH1D *)gDirectory->Get("senbmodxp");
//       senbmodxp->SetLineColor(kRed);
//       senbmodxp->Draw("goff");
//       fitbmodxp->SetRange(senbmodxp->GetMean(1) - amplitudexp, senbmodxp->GetMean(1) + amplitudexp );
//       fitbmodxp->SetParameters(senbmodxp->GetMean(2), 6000);
//       senbmodxp->Fit("fitbmodxp","E M R F Q 0");
//       xpbmodslope1 =fitbmodxp->GetParameter(1); expbmodslope1 =fitbmodxp->GetParError(1);
//       xpbmodslope2 =fitbmodxp->GetParameter(0); expbmodslope2 =fitbmodxp->GetParError(0);
//       expbmodslope11 = TMath::Power((expbmodslope1/xpbmodslope1),2);expbmodslope12 = TMath::Power((expbmodslope2/xpbmodslope2),2);
//       xpbmodslope = xpbmodslope1/xpbmodslope2;  expbmodslope13 = TMath::Sqrt(expbmodslope11 + expbmodslope12);
//       expbmodslope =  expbmodslope13*xpbmodslope;
//       Double_t limitxp = senbmodxp->GetMean(2);
//       senbmodxp->SetYTitle("MD ALL YIELD [V/A]");
//       senbmodxp->SetXTitle("TARGETX [mm]");
//       senbmodxp->SetTitle(Form("MD ALL ASYM vs TARGETX for Xp-Sensitivities:%2.2f+-%2.2f [ppm/mm]",1e6*xpbmodslope,1e6*expbmodslope));
//       senbmodxp->Draw("goff");
//       fitbmodxp->Draw("same");
//       senbmodxp->GetYaxis()->SetRangeUser(limitxp*0.995,limitxp*1.005);
//     }
//     gPad->Update();
//     else{pad191->cd(2);tnodataxp->Draw();}

    pad191->cd(3);
    if (tm->Draw("qwk_mdallbars*1e6:qwk_bpm3c12X>>senbmode",Form("%s && qwk_mdallbars.%s && qwk_bpm3c12X.%s && bm_pattern_number==13",s3,s2,s2),"prof goff")>0){
      senbmode = (TH1D *)gDirectory->Get("senbmode");
      senbmode->SetLineColor(kBlue);
      senbmode->Draw("goff");
      fitbmode->SetRange(senbmode->GetMean(1) - amplitudee, senbmode->GetMean(1) + amplitudee );
      fitbmode->SetParameters(senbmode->GetMean(2), 6000);
      senbmode->Fit("fitbmode","E M R F Q 0");
      ebmodslope1 =fitbmode->GetParameter(1); eebmodslope1 =fitbmode->GetParError(1);
      ebmodslope2 =fitbmode->GetParameter(0); eebmodslope2 =fitbmode->GetParError(0);
      eebmodslope11 = TMath::Power((eebmodslope1/ebmodslope1),2);eebmodslope12 = TMath::Power((eebmodslope2/ebmodslope2),2);
      ebmodslope = ebmodslope1/ebmodslope2;  eebmodslope13 = TMath::Sqrt(eebmodslope11 + eebmodslope12);
      eebmodslope =  eebmodslope13*ebmodslope;
      Double_t limite = senbmode->GetMean(2);
      senbmode->SetYTitle("MD ALL YIELD [V/A]");
      senbmode->SetXTitle("BPM 3C12X [mm]");
      senbmode->SetTitle(Form("MD ALL ASYM vs BPM 3C12X for E-Sensitivities:%2.2f+-%2.2f [ppm/mm]",1e6*ebmodslope,1e6*eebmodslope));
      senbmode->Draw("goff");
      fitbmode->Draw("same");
      senbmode->GetYaxis()->SetRangeUser(limite*0.995,limite*1.005);
    }
    gPad->Update();
    else{pad191->cd(3);tnodatae->Draw();}

    pad191->cd(2);
    if (tm->Draw("qwk_mdallbars*1e6:qwk_targetY>>senbmody",Form("%s && qwk_mdallbars.%s && qwk_targetY.%s && bm_pattern_number==12",s3,s2,s2),"prof goff")>0){
      senbmody = (TH1D *)gDirectory->Get("senbmody");
      senbmody->SetLineColor(kGreen);
      senbmody->Draw("goff");
      fitbmody->SetRange(senbmody->GetMean(1) - amplitudey, senbmody->GetMean(1) + amplitudey );
      fitbmody->SetParameters(senbmody->GetMean(2), 6000);
      senbmody->Fit("fitbmody","E M R F Q 0");
      ybmodslope1 =fitbmody->GetParameter(1); eybmodslope1 =fitbmody->GetParError(1);
      ybmodslope2 =fitbmody->GetParameter(0); eybmodslope2 =fitbmody->GetParError(0);
      eybmodslope11 = TMath::Power((eybmodslope1/ybmodslope1),2);eybmodslope12 = TMath::Power((eybmodslope2/ybmodslope2),2);
      ybmodslope = ybmodslope1/ybmodslope2;  eybmodslope13 = TMath::Sqrt(eybmodslope11 + eybmodslope12);
      eybmodslope =  eybmodslope13*ybmodslope;
      Double_t limity = senbmody->GetMean(2);
      senbmody->SetYTitle("MD ALL YIELD [V/A]");
      senbmody->SetXTitle("TARGETY [mm]");
      senbmody->SetTitle(Form("MD ALL ASYM vs TARGETY for Y-Sensitivities:%2.2f+-%2.2f [ppm/mm]",1e6*ybmodslope,1e6*eybmodslope));
      senbmody->Draw("goff");
      fitbmody->Draw("same");
      senbmody->GetYaxis()->SetRangeUser(limity*0.995,limity*1.005);
    }
    gPad->Update();
    else{pad191->cd(2);tnodatay->Draw();}

//     pad191->cd(5);
//     if (tm->Draw("qwk_mdallbars*1e6:qwk_targetY>>senbmodyp",Form("%s && qwk_mdallbars.%s && qwk_targetY.%s && bm_pattern_number==15",s3,s2,s2),"prof goff")>0){
//       senbmodyp = (TH1D *)gDirectory->Get("senbmodyp");
//       senbmodyp->SetLineColor(kGreen);
//       senbmodyp->Draw("goff");
//       fitbmodyp->SetRange(senbmodyp->GetMean(1) - amplitudeyp, senbmodyp->GetMean(1) + amplitudeyp );
//       fitbmodyp->SetParameters(senbmodyp->GetMean(2), 6000);
//       senbmodyp->Fit("fitbmodyp","E M R F Q 0");
//       ypbmodslope1 =fitbmodyp->GetParameter(1); eypbmodslope1 =fitbmodyp->GetParError(1);
//       ypbmodslope2 =fitbmodyp->GetParameter(0); eypbmodslope2 =fitbmodyp->GetParError(0);
//       eypbmodslope11 = TMath::Power((eypbmodslope1/ypbmodslope1),2);eypbmodslope12 = TMath::Power((eypbmodslope2/ypbmodslope2),2);
//       ypbmodslope = ypbmodslope1/ypbmodslope2;  eypbmodslope13 = TMath::Sqrt(eypbmodslope11 + eypbmodslope12);
//       eypbmodslope =  eypbmodslope13*ypbmodslope;
//       Double_t limityp = senbmodyp->GetMean(2);
//       senbmodyp->SetYTitle("MD ALL YIELD [V/A]");
//       senbmodyp->SetXTitle("TARGETY [mm]");
//       senbmodyp->SetTitle(Form("MD ALL ASYM vs TARGETY for Yp-Sensitivities:%2.2f+-%2.2f [ppm/mm]",1e6*ypbmodslope,1e6*eypbmodslope));
//       senbmodyp->Draw("goff");
//       fitbmodyp->Draw("same");
//       senbmodyp->GetYaxis()->SetRangeUser(limityp*0.995,limityp*1.005);
//     }
//     gPad->Update();
//     else{pad191->cd(5);tnodatayp->Draw();}

    c19->Update(); c19->SaveAs(psenbmod);}

  gStyle->SetTitleSize(0.04);
  gStyle->SetTitleYSize(0.04);
  gStyle->SetTitleXSize(0.04);
  gStyle->SetTitleYOffset(1.1);
  gStyle->SetTitleXOffset(1.1);
  gStyle->SetLabelSize(0.050);
  gStyle->SetLabelSize(0.050,"y");
  gStyle->SetLabelSize(0.050,"x");
  gStyle->SetCanvasColor(kRed-10);
  gStyle->SetStatFontSize(0.070);
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.30);
  gStyle->SetStatH(0.30); 
  /****************************************************************************/
  if (MDLUMI){
  TCanvas *c5 = new TCanvas("c5", Form("Run %d: Main Detector Barsum Asymmetries (ppm)",run_number),80,80,1400,1000);
  pad50 = new TPad("pad50","pad50",ps1,ps2,ps4,ps4);
  pad51 = new TPad("pad51","pad51",ps1,ps1,ps4,ps3);
  pad50->SetFillColor(kRed-10);
  pad51->SetFillColor(kWhite);
  pad50->Draw();
  pad51->Draw();
  pad50->cd();
  TString text = Form("Run %d: Main Detector Barsum Asymmetries (ppm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct50 = new TText(0.012,0.4,text);
  ct50->SetTextSize(0.3);
  ct50->Draw();
  pad51->cd();
  pad51->Divide(4,3);
  printf("%sPlotting %sMain Detector Barsum Asymmetries (ppm)%s\n",blue,red,normal);
  gStyle->SetCanvasColor(kBlue-10);
  /* ----------------------------------------------------------------------- */
  TCanvas *c9 = new TCanvas("c9", Form("Run %d: Downstream Lumi Yields (Volts/uA)",run_number),160,160,1400,1000);
  pad90 = new TPad("pad90","pad90",ps1,ps2,ps4,ps4);
  pad91 = new TPad("pad91","pad91",ps1,ps1,ps4,ps3);
  pad90->SetFillColor(kBlue-10);
  pad91->SetFillColor(kWhite);
  pad90->Draw();
  pad91->Draw();
  pad90->cd();
  TString text = Form("Run %d: Downstream Lumi Yields (Volts/uA) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct90 = new TText(0.012,0.4,text);
  ct90->SetTextSize(0.3);
  ct90->Draw();
  pad91->cd();
  pad91->Divide(4,3);
  printf("%sPlotting %sDownstream Lumi Yields (Volts/uA)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  TCanvas *c10 = new TCanvas("c10", Form("Run %d: Downstream Lumi Asymmetries (ppm)",run_number),180,180,1400,1000);
  pad100 = new TPad("pad100","pad100",ps1,ps2,ps4,ps4);
  pad101 = new TPad("pad101","pad101",ps1,ps1,ps4,ps3);
  pad100->SetFillColor(kBlue-10);
  pad101->SetFillColor(kWhite);
  pad100->Draw();
  pad101->Draw();
  pad100->cd();
  TString text = Form("Run %d: Downstream Lumi Asymmetries (ppm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct100 = new TText(0.012,0.4,text);
  ct100->SetTextSize(0.3);
  ct100->Draw();
  pad101->cd();
  pad101->Divide(4,3);
  printf("%sPlotting %sDownstream Lumi Asymmetries (ppm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  gStyle->SetOptStat(1);
  gStyle->SetOptStat("emr");

  char hadslumi[255],hadslumiall[255], txadslumi[255], tyadslumi[255], txadslumiall[255],hydslumi[255],hydslumiall[255], txydslumi[255], txydslumiall[255];

  sprintf(txalladet,"MD All ASYM [ppm]");

  int dm=0;
  for ( int m=0; m<8; m++) {
    dm = m+1;

    sprintf(adet,"asym_qwk_md%dbarsum*1e6>>ha%d",dm,dm);
    sprintf(cutadet,"%s && asym_qwk_md%dbarsum.%s",s1,dm,s2);
    sprintf(hadet,"ha%d",dm);
    sprintf(txadet,"MD %d ASYM [ppm]",dm);

    sprintf(ylumi,"yield_qwk_dslumi%d>>hydsl%d",dm,dm);
    sprintf(alumi,"asym_qwk_dslumi%d*1e6>>hadsl%d",dm,dm);
    sprintf(cutylumi,"%s && yield_qwk_dslumi%d.%s",s1,dm,s2);
    sprintf(cutalumi,"%s && asym_qwk_dslumi%d.%s",s1,dm,s2);
    sprintf(hydslumi,"hydsl%d",dm);
    sprintf(hadslumi,"hadsl%d",dm);
    sprintf(txydslumi,"DS LUMI %d YIELD [V/uA]",dm);
    sprintf(txadslumi,"DS LUMI %d ASYM [ppm]",dm);

    pad51->cd(position6[dm]);
    th->Draw(adet,cutadet,"goff");
    hadt = (TH1F *)gDirectory->Get(hadet);
    hadt->SetYTitle(tarbitrary);
    hadt->SetXTitle(txadet);
    hadt->SetTitle(txadet);
    hadt->Draw();
    gPad->Update();

    pad91->cd(position6[dm]);
    th->Draw(ylumi,cutylumi,"goff");
    hylumi = (TH1F *)gDirectory->Get(hydslumi);
    hylumi->SetYTitle(tarbitrary);
    hylumi->SetXTitle(txydslumi);
    hylumi->SetTitle(txydslumi);
    hylumi->Draw();
    gPad->Update();

    pad101->cd(position6[dm]);
    th->Draw(alumi,cutalumi,"goff");
    halumi = (TH1F *)gDirectory->Get(hadslumi);
    halumi->SetYTitle(tarbitrary);
    halumi->SetXTitle(txadslumi);
    halumi->SetTitle(txadslumi);
    halumi->Draw();

    gPad->Update();
  }
  int dn=0;
  for ( int n=0; n<3; n++) {
    dn = n+1;

    sprintf(ylumis,"yield_dslumi_%s>>hydslall%d",lumi[n].Data(),dn);
    sprintf(alumis,"asym_dslumi_%s*1e6>>hadslall%d",lumi[n].Data(),dn);
    sprintf(cutylumis,"%s && yield_dslumi_%s.%s",s1,lumi[n].Data(),s2);
    sprintf(cutalumis,"%s && asym_dslumi_%s.%s",s1,lumi[n].Data(),s2);
    sprintf(hydslumiall,"hydslall%d",dn);
    sprintf(hadslumiall,"hadslall%d",dn);
    sprintf(txydslumiall,"DS LUMI %s YIELD [V/uA]",lumi[n].Data());
    sprintf(txadslumiall,"DS LUMI %s ASYM [ppm]",lumi[n].Data());

    pad91->cd(position7[n]);
    th->Draw(ylumis,cutylumis,"goff");
    hylumiall = (TH1F *)gDirectory->Get(hydslumiall);
    hylumiall->SetFillStyle(3013);
    hylumiall->SetFillColor(kBlue-2);
    hylumiall->SetYTitle(tarbitrary);
    hylumiall->SetXTitle(txydslumiall);
    hylumiall->SetTitle(txydslumiall);
    hylumiall->Draw();

    gPad->Update();

    pad101->cd(position7[n]);
    th->Draw(alumis,cutalumis,"goff");
    halumiall = (TH1F *)gDirectory->Get(hadslumiall);
    halumiall->SetFillStyle(3013);
    halumiall->SetFillColor(kBlue-2);
    halumiall->SetYTitle(tarbitrary);
    halumiall->SetXTitle(txadslumiall);
    halumiall->SetTitle(txadslumiall);
    halumiall->Draw();
    gPad->Update();
  }

    pad51->cd(6);
    th->Draw("asym_qwk_mdallbars*1e6>>hall",Form("%s && asym_qwk_mdallbars.%s",s1,s2),"goff");
    hall->SetFillColor(kRed-2);
    // hall->SetLineColor(kRed-2);
    hall->SetFillStyle(3013);
    hall->SetYTitle(tarbitrary);
    hall->SetXTitle(txalladet);
    hall->SetTitle(txalladet);
    hall->Draw();

    pad51->cd(4);
    th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-(asym_qwk_bcm5+asym_qwk_bcm6)/2)*1e6>>hall56",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_charge.%s && asym_qwk_bcm5.%s && asym_qwk_bcm6.%s",s1,s2,s2,s2,s2),"goff");
    hall56->SetFillColor(kRed-2);
    hall56->SetFillStyle(3004);
    hall56->SetYTitle(tarbitrary);
    hall56->SetXTitle(txalladet);
    hall56->SetTitle("Normalized to BCM 5 & 6");
    hall56->Draw();

    pad51->cd(8);
    th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-(asym_qwk_bcm1+asym_qwk_bcm5)/2)*1e6>>hall15",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_charge.%s && asym_qwk_bcm1.%s && asym_qwk_bcm5.%s",s1,s2,s2,s2,s2),"goff");
    hall15->SetFillColor(kRed-2);
    hall15->SetFillStyle(3004);
    hall15->SetYTitle(tarbitrary);
    hall15->SetXTitle(txalladet);
    hall15->SetTitle("Normalized to BCM 1 & 5");
    hall15->Draw();

    pad51->cd(12);
    th->Draw("(asym_qwk_mdallbars+asym_qwk_charge-(asym_qwk_bcm2+asym_qwk_bcm6)/2)*1e6>>hall26",Form("%s && asym_qwk_mdallbars.%s && asym_qwk_charge.%s && asym_qwk_bcm2.%s && asym_qwk_bcm6.%s",s1,s2,s2,s2,s2),"goff");
    hall26->SetFillColor(kRed-2);
    hall26->SetFillStyle(3004);
    hall26->SetYTitle(tarbitrary);
    hall26->SetXTitle(txalladet);
    hall26->SetTitle("Normalized to BCM 2 & 6");
    hall26->Draw();

  c5->Update(); c5->SaveAs(pmdasym);
  c9->Update(); c9->SaveAs(pdslumiyield);
  c10->Update(); c10->SaveAs(pdslumiasym);}
  /****************************************************************************/
  if (USLUMI){
  TCanvas *c16 = new TCanvas("c16", Form("Run %d: Upstream Lumi Yields (Volts/uA) & Asymmetry (ppm)",run_number),200,200,1580,1000);
  pad160 = new TPad("pad160","pad160",ps1,ps2,ps4,ps4);
  pad161 = new TPad("pad161","pad161",ps1,ps1,ps4,ps3);
  pad160->SetFillColor(kBlue-10);
  pad161->SetFillColor(kWhite);
  pad160->Draw();
  pad161->Draw();
  pad160->cd();
  TString text = Form("Run %d: Upstream Lumi Yields (Volts/uA) & Asymmetry (ppm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct160 = new TText(0.012,0.4,text);
  ct160->SetTextSize(0.3);
  ct160->Draw();
  pad161->cd();
  pad161->Divide(6,3);
  printf("%sPlotting %sUpstream Lumi Yields (Volts/uA) & Asymmetry (ppm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  char hauslumi[255],txauslumi[255], hyuslumi[255], txyuslumi[255];

  for ( int q=0; q<5; q++) {

    sprintf(yuslumis,"yield_%s_sum>>hyusl%d",uslumi[q].Data(),q);
    sprintf(auslumis,"asym_%s_sum*1e6>>hausl%d",uslumi[q].Data(),q);
    sprintf(cutyuslumis,"%s && yield_%s_sum.%s",s1,uslumi[q].Data(),s2);
    sprintf(cutauslumis,"%s && asym_%s_sum.%s",s1,uslumi[q].Data(),s2);
    sprintf(hyuslumi,"hyusl%d",q);
    sprintf(hauslumi,"hausl%d",q);
    sprintf(txyuslumi,"%s YIELD [V/uA]",uslumi[q].Data());
    sprintf(txauslumi,"%s ASYM [ppm]",uslumi[q].Data());

    pad161->cd(position10[q]);
    th->Draw(yuslumis,cutyuslumis,"goff");
    hyuslumis = (TH1F *)gDirectory->Get(hyuslumi);
    hyuslumis->SetYTitle(tarbitrary);
    hyuslumis->SetXTitle(txyuslumi);
    hyuslumis->SetTitle(txyuslumi);
    hyuslumis->Draw();

    gPad->Update();

    pad161->cd(position11[q]);
    th->Draw(auslumis,cutauslumis,"goff");
    hauslumis = (TH1F *)gDirectory->Get(hauslumi);
    hauslumis->SetFillColor(kBlue-2);
    hauslumis->SetFillStyle(3013);
    hauslumis->SetYTitle(tarbitrary);
    hauslumis->SetXTitle(txauslumi);
    hauslumis->SetTitle(txauslumi);
    hauslumis->Draw();

    gPad->Update();

  }
  c16->Update(); c16->SaveAs(puslumi);}

  /****************************************************************************/
  if (USLUMISEN){
  TCanvas *c17 = new TCanvas("c17", Form("Run %d: Upstream Lumi Sensitivity (ppm/mm)",run_number),220,220,1580,1000);
  pad170 = new TPad("pad170","pad170",ps1,ps2,ps4,ps4);
  pad171 = new TPad("pad171","pad171",ps1,ps1,ps4,ps3);
  pad170->SetFillColor(kBlue-10);
  pad171->SetFillColor(kWhite);
  pad170->Draw();
  pad171->Draw();
  pad170->cd();
  TString text = Form("Run %d: Upstream Lumi Sensitivity (ppm/mm) for %s, %2.1f uA, %2.1fx%2.1f mm",run_number,tar[1],current,raster,raster);
  ct170 = new TText(0.012,0.4,text);
  ct170->SetTextSize(0.3);
  ct170->Draw();
  pad171->cd();
  pad171->Divide(6,3);
  printf("%sPlotting %sUpstream Lumi Sensitivity (ppm/mm)%s\n",blue,red,normal);
  /* ----------------------------------------------------------------------- */
  gStyle -> SetOptFit(1111);
  gStyle -> SetOptStat("e");

  int xn=0;
  for ( int x=0; x<5; x++) {
    xn = x +1;

    sprintf(auslumisenx,"asym_%s_sum*1e6:diff_qwk_bpm3h09bX>>huslx%d",uslumi[x].Data(),x);
    sprintf(cutauslumisenx,"%s && asym_%s_sum.%s && diff_qwk_bpm3h09bX.%s",s1,uslumi[x].Data(),s2,s2);
    sprintf(histouslx,"huslx%d",x);
    sprintf(yaxisusl,"%s ASYM [ppm]",uslumi[x].Data());
    sprintf(auslumiseny,"asym_%s_sum*1e6:diff_qwk_bpm3h09bY>>husly%d",uslumi[x].Data(),x);
    sprintf(cutauslumiseny,"%s && asym_%s_sum.%s && diff_qwk_bpm3h09bY.%s",s1,uslumi[x].Data(),s2,s2);
    sprintf(histously,"husly%d",x);

    pad171->cd(position10[x]);
    pad171->cd(position10[x])->SetGrid();

    th->Draw(auslumisenx,cutauslumisenx,"prof");
    TH1D *huslxf = (TH1D*)gDirectory->Get(histouslx);
    huslxf->GetYaxis()->SetTitle(yaxisusl);
    huslxf->GetXaxis()->SetTitle(xaxis);
    huslxf->SetTitle(yaxisusl);
    huslxf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    huslxf->SetLineColor(kRed);
    huslxf->Draw("");
    gPad->Modified();
    gPad->Update();

    pad171->cd(position11[x]);
    pad171->cd(position11[x])->SetGrid();
    // pad171->cd(position11[x])->SetFillColor(kGreen-10);

    th->Draw(auslumiseny,cutauslumiseny,"prof");
    TH1D *huslyf = (TH1D*)gDirectory->Get(histously);
    huslyf->GetYaxis()->SetTitle(yaxisusl);
    huslyf->GetXaxis()->SetTitle(xaxis1);
    huslyf->SetTitle(yaxisusl);
    huslyf->Fit("pol1","E M R F Q","",fit_range[0], fit_range[1]);
    huslyf->SetLineColor(kGreen);
    huslyf->Draw("");
    gPad->Modified();
    gPad->Update();
  }
  c17->Update(); c17->SaveAs(puslumisen);}

  /****************************************************************************/
  printf("%sSummary Table.%s\n",blue,normal);  
  printf(ssline);printf("%s|             %sRun Number: %d%s             |%s\n",green,blue,run_number,green,normal);
  printf("%s|  %s%s, %2.1f uA, %2.1fx%2.1f mm%s  |%s\n",green,blue,tar[1],current,raster,raster,green,normal);
  printf(ssline);
  printf("%s|%sI                         %s|%suA    %s|%s%2.1f%s      |%s\n",green,blue,green,blue,green,red,current,green,normal);
  printf("%s|%sMDALLBARS width           %s|%sppm   %s|%s%2.1f%s      |%s\n",green,blue,green,blue,green,red,cal_mdalla,green,normal);
  printf("%s|%sBCM12-ddif width          %s|%sppm   %s|%s%2.1f%s      |%s\n",green,blue,green,blue,green,red,cal_bcmdd,green,normal);
  printf("%s|%sA_q mean                  %s|%sppm   %s|%s%2.2f%s      |%s\n",green,blue,green,blue,green,red,cal_abcmm,green,normal);
  printf("%s|%sA_q width                 %s|%sppm   %s|%s%2.1f%s       |%s\n",green,blue,green,blue,green,red,cal_abcm,green,normal);
  printf("%s|%sMDALLBARS X-sensitivity   %s|%sppm/mm%s|%s%2.1f+-%1.1f%s |%s\n",green,blue,green,blue,green,red,cal_mdxsen,cal_emdxsen,green,normal);
  printf("%s|%sMDALLBARS Y-sensitivity   %s|%sppm/mm%s|%s%2.1f+-%1.1f%s |%s\n",green,blue,green,blue,green,red,cal_mdysen,cal_emdysen,green,normal);
  printf(ssline);
  /****************************************************************************/
  printf("%sDone with all the plots.%s\n",blue,normal);
  /****************************************************************************/
  UInt_t hclog_switch = 0;
  printf(sline);printf("%sPlease Insert %s1%s for submiting to HCLOG, otherwise %s0%s and hit ENTER\n%s",blue,red,blue,red,blue,normal);printf(sline);
  cin >> hclog_switch;
  if (hclog_switch> 1) {printf("%sPlease insert a correct No. Exiting the program!%s\n",blue,normal); exit(1);}

  //   cout<<__LINE__<<endl;
  char textfile[255],info[255],line2[255],line3[255],line4[255],line5[255],line51[255],line6[255],line7[255],contact[255],hclog_list[255];
  sprintf(textfile,"%s/%druninfo.txt",dir[0],run_number);
  sprintf(info,"<pre>\n%s|             Run Number: %d                 |\n|  %s, %2.1f uA, %2.1fx%2.1f mm      |\n%s",sslinen,run_number,tar[1],current,raster,raster,sslinen);
  sprintf(line2,"|I                         |uA    |%2.1f        |\n",current);
  sprintf(line3,"|MDALLBARS width           |ppm   |%2.1f        |\n",cal_mdalla);
  sprintf(line4,"|BCM12-ddif width          |ppm   |%2.1f         |\n",cal_bcmdd);
  sprintf(line51,"|A_q maen                  |ppm   |%2.2f        |\n",cal_abcmm);
  sprintf(line5,"|A_q width                 |ppm   |%2.1f        |\n",cal_abcm);
  sprintf(line6,"|MDALLBARS X-sensitivity   |ppm/mm|%2.1f+-%1.1f|\n",cal_mdxsen,cal_emdxsen);
  sprintf(line7,"|MDALLBARS Y-sensitivity   |ppm/mm|%2.1f+-%1.1f|\n%s</pre>",cal_mdysen,cal_emdysen,sslinen);
  sprintf(contact,"%sPlease contact Nuruzzaman (nur@jlab.org) for problems and comments%s\n",blue,normal);
  sprintf(hclog_list,"%s/%drunsummary.txt",dir[1],run_number);
  /****************************************************************************/

  if (hclog_switch==0) {ofstream outfile(textfile);outfile <<info<<line2<<line3<<line4<<line51<<line5<<line6<<line7<<endl;
    printf("%s%s#%s          Plots are not submitted to HCLOG and saved in directory        %s#\n#               %s              #\n%s\n",dline,red,blue,red,dir[0],dline);}

  if (hclog_switch==1) {

    FILE *check;
    check = fopen(hclog_list,"r");
    if (!check == NULL) {printf("%sPlots for this run are already submitted to HCLOG !!!. Exiting Program%s\n",red,normal);printf(contact);exit(1);}
  
    char user_name_hclog[255],subject_hclog[255],comments_text[255];
    TString hclog_post_string;
    //     TString comments_hclog;
    char comments_hclog[255];
    char *email_list;
    email_list = "qweak_autoanalysis@jlab.org";

    printf("%s\nPlease insert comments for the run to post in HCLOG%s\n",blue,normal);
    cin.getline(comments_hclog,255,'\n');
    gets(comments_hclog);

    sprintf(user_name_hclog,"qwanalysis");
    sprintf(subject_hclog,"Analysis: Run %d - Plots and Summary Table for 100k Events.",run_number);
    sprintf(comments_text,"%s\n\n",comments_hclog);
    ofstream outfile(textfile); outfile<<comments_text<<info<<line2<<line3<<line4<<line51<<line5<<line6<<line7<<endl;

    printf("\nUser Name : %s\n", user_name_hclog);
    printf("Subject : %s\n", subject_hclog);
    printf("Comments : %s\n", comments_hclog);
    printf("Email : %s\n", email_list);
    
    hclog_post_string = "hclog_post";
    hclog_post_string += " ";
    hclog_post_string += "--subject=\"";
    hclog_post_string += subject_hclog;
    hclog_post_string += "\" ";
    hclog_post_string += "--author=\"";
    hclog_post_string += user_name_hclog;
    hclog_post_string += "\" ";
    //     hclog_post_string += "--body=\"";
    //     hclog_post_string += comments_text;
    //     hclog_post_string += "\" ";
    hclog_post_string += "--textfile=\"";
    hclog_post_string += textfile;
    hclog_post_string += "\" ";
    hclog_post_string += "--emailto=\"";
    hclog_post_string += email_list;
    hclog_post_string += "\" ";    
    //    if(file_output_flag) {
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pcharge;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pchargeasym;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pchargeddasym;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pbpmd;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdallasym;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdasym;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdallsenx;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdallseny;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdyield;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pdslumiasym;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += plumisenx;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += plumiseny;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pdslumiyield;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += puslumi;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += puslumisen;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmodulation;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += psenbmod;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdbkg;
    hclog_post_string += "\" ";
    hclog_post_string += "--attachment=\"";
    hclog_post_string += pmdyieldvar;
    hclog_post_string += "\" ";
    //     }
    hclog_post_string += "--tag=\"This is loged by hclog_post and qwanalysis\"";
    //    hclog_post_string += "  --test";
    hclog_post_string += "  --cleanup";

    //       printf("%s",hclog_post_string.Data());
    gSystem->Exec(hclog_post_string.Data());

    ofstream outfile(hclog_list); outfile<<comments_text<<info<<line2<<line3<<line4<<line5<<line6<<line7<<endl;
    printf("%s%s#%s    Congratulations !!!!!   All plots are successfully posted to HCLOG     %s#\n%s",dline,red,blue,red,dline);

  }
  
  /****************************************************************************/
  printf(contact);
  printf("%sDone with everything. Exiting the program .......  \n%s",red,normal);
  exit(1);
  //  gDirectory->Delete("*");
  // return(0);
  /****************************************************************************/
}
/******************************************************************************************************/
/*                                User Manual                                                         */
/*                                                                                                    */
/*Step1: Open a terminal and run 'qwanalysis'                                                         */
/*Step2: It will ask for a 'run #'. Type 'run '# and press 'ENTER'.                                   */
/*Step3: Next it will ask for 'target' information. Type 1=LH2, 2=4% US Al, 3=2% US Al, 4=1% US Al,   */
/*       5=4% DS Al, 6=2% DS Al, 7=1% DS Al, 8=US Pure Al and hit ENTER.                              */
/*Step4: If all the plots are done, it will ask wheather you want to submit the plots to HCLOG or not.*/
/*       Insert '1' for to submit the plots to HCLOG otherwise '0' and press 'ENTER'                  */
/*Step5: If you choose to submit all the plots to HCLOG, it will ask for comments                     */
/*       Please insert comments and hit 'ENTER'.                                                      */
/*       e.g; Beam currents was ramping up during the run.                                            */
/*                                                                                                    */
/*       If you didn't submitted plots in HCLOG (otherwise deleted), will be saved in the directory:  */
/*       'users/qwanalysis/plots/'                                                                    */
/*       Type 'display filename.png' & 'emacs filename.txt' to view the figures & run information.    */
/******************************************************************************************************/
