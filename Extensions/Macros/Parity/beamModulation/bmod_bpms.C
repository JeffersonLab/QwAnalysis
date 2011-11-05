/**************************************************************************/
/*                              Nuruzzaman                                */
/*                              07/22/2011                                */
/*                                                                        */
/*                        Last Edited:10/31/2011                          */
/*                                                                        */
/* This Macro create plots for beamline optics using beam modulation.     */
/* Please see the USER MANUAL at the bottom of this macro for its usage.  */
/**************************************************************************/
#include <NurClass.h>
void bmod_bpms(UInt_t run_number=0)
{
  time_t tStart = time(0), tEnd; 

  char sline[255],dline[255],textfile[255],textfit[255],run[255], run0[255];
//   TChain chain= NULL;

  sprintf(sline,"%s#-------------------------------------------------------------------------#%s\n",magenta,normal);
  sprintf(dline,"%s#*************************************************************************#%s\n",red,normal);


  /* Ask to input the run condition to user */
  printf(sline); printf("%sEnter Run Number%s\n",blue,normal); printf(sline);
  cin >> run_number;


  /* Directory */
  TString dir1 = "/w/cache/mss/hallc/qweak/rootfiles/pass3/";
  TString dir2 = "/u/group/hallc/www/hallcweb/html/qweak/bmod/images";
  TString dir3 = "/u/group/hallc/www/hallcweb/html/qweak/bmod/text";
//   TString dir2 = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots/pass3";
//   TString dir3 = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots/pass3/bpm_response";

  sprintf(run0,"%sQwPass3_%d.000.root",dir1.Data(),run_number);
  TFile f(run0);
  if(!f.IsOpen()) {printf("%sUnable to find Rootfile for Run # %s%d%s.\nPlease insert a correct Run #. Exiting the program!%s\n",blue,red,run_number,blue,normal); exit(1);}

  TChain * chain = new TChain("Mps_Tree");
//   TChain chain("Mps_Tree");
  for(Int_t t=0; t<10; t++) 
    {
      sprintf(run,"%sQwPass3_%d.00%d.root",dir1.Data(),run_number,t);
      TFile f1(run);
      chain->Add(run);
      // 	printf("%sRootfile %s has been found%s\n",blue,run,normal);
    }


  const Int_t NUM1 = 1, NUM5 = 5, NUM6 = 6, NUM10 = 10, NUM = 24;
  TString ptt[NUM10] = {"bm_pattern_number==11","bm_pattern_number==12","bm_pattern_number==13","bm_pattern_number==14","bm_pattern_number==15",
			"bm_pattern_number==11","bm_pattern_number==12","bm_pattern_number==13","bm_pattern_number==14","bm_pattern_number==15"};
  TString var[NUM10] = {"X","Y","E","Xp","Yp","X","Y","E","Xp","Yp"};
  TString xy[NUM10] = {"X","Y","X","X","Y","Y","X","Y","Y","X"};
  TString bpms[NUM] = {"target","bpm3h09b","bpm3h09","bpm3h08","bpm3h07c","bpm3h07b","bpm3h07a","bpm3h04","bpm3h02","bpm3c21","bpm3c20","bpm3p03a",
		       "bpm3p02b","bpm3p02a","bpm3c19","bpm3c18","bpm3c17","bpm3c16","bpm3c14","bpm3c12","bpm3c11","bpm3c08","bpm3c07a","bpm3c07"};
  
  TString d_ramp = "ramp*76.29e-6*1200";
  TString txaxis = "Beam Modulation Phase [Degree]";
  //  TString func = "[0] + [1]*sin(0.0175*x+[2])";

  char dircreate[255],dircreate2[255],saveplot1[255],title1[255],saveplot3[255],title3[255];
  char drawstringh1[255],drawstringh2[255],cut1[255],cut2[255],yaxis1[255],yaxis2[255],xaxis2[255],histo1[255],histo2[255],padtitle1[255],padtitle2[255];
  Double_t amp[NUM],amp_e[NUM],e_amp[NUM],phase[NUM],e_phase[NUM],angle[NUM],xact[NUM],modamp[NUM],optim[NUM],x_optimx_t[NUM],y_optimy_t[NUM],xp_optimx_t[NUM],yp_optimy_t[NUM],pamp[NUM];

  Float_t signp,signm;
  signp = 1;signm = -1;
  Double_t csizx,csizy,csiz3x,csiz3y,cx1,cy1,cx2,cx3,tsiz,tsiz3,tll,tlr,ps1,ps2,ps3,ps4;
  csizx=1600;csizy=1300;csiz3x=1100;csiz3y=780;tsiz=0.40;tsiz3=0.45;tll=0.012;tlr=0.4;cx1=0;cy1=0;cx2=300;cx3=600;
  ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;

//   Float_t zpos[NUM] = {0.0,-138.8,-393.6,-516.3,-842.0,-937.6,-1033.3,-1910.7,-2158.6,-3052.3,-3188.1,-3544.7,-3813.7,-3952.7,-4581.7,-4712.8,-4892.34,-5201.29,-6241.37,-7281.29,-7801.37,-9361.37,-9570.71,-9971.37};
//   Float_t e_zpos[NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//   Float_t x1_optim[NUM] = {0.31000,0.305372,0.292606,0.286461,0.27015,0.265359,0.260564,0.21662,0.2042,0.13162,0.096213,0.0280517,-0.0259141,-0.053922,-0.180479,-0.235167,-0.232511,-0.301092,-0.0635137,0.185382,0.0942072,-3.90E-14,-3.40E-14,-2.53E-14};
//   Float_t y1_optim[NUM] = {0.205355,0.201253,0.19372,0.190095,0.18047,0.177643,0.174814,0.148823,0.141469,0.0875582,0.0765819,0.0264411,-0.0108227,-0.029977,-0.10552,-0.0907212,-0.0986115,-0.0749349,-0.106843,-0.0675235,-0.09167,0,0,0};
// //   Float_t x_optimx[NUM] = {0.24,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.2,0.16,0.09,0.03,0,-0.14,-0.2,-0.2,-0.28,-0.1,0.06,0.07,0,0,0};
// //   Float_t y_optimy[NUM] = {1.57E-01,1.57E-01,1.57E-01,1.57E-01,1.57E-01,1.57E-01,1.57E-01,1.56E-01,1.56E-01,1.29E-01,1.20E-01,6.27E-02,2.10E-02,-6.71E-04,-8.98E-02,-8.34E-02,-9.92E-02,-9.05E-02,-1.92E-01,-1.56E-01,-2.46E-01,0.00E+00,0.00E+00,0.00E+00};
// //   Float_t e_optimx[NUM] = {1.78E-004,1.57E-004,1.18E-004,9.98E-005,5.02E-005,3.56E-005,2.11E-005,-1.12E-004,-1.50E-004,-2.19E-004,-2.01E-004,-1.99E-004,-1.80E-004,-1.68E-004,-9.72E-005,-1.01E-004,-1.06E-004,-1.61E-004,-7.38E-002,-3.43E-001,-1.41E-001,4.41E-005,3.63E-005,2.22E-005};
// //   Float_t xp_optimx[NUM] = {0,0.01,0.02,0.02,0.04,0.04,0.04,0.08,0.09,0.1,0.08,0.07,0.05,0.05,0,-0.01,-0.02,-0.04,-0.05,-0.09,-0.01,0,0,0};
// //   Float_t yp_optimy[NUM] = {-1.22E-03,2.96E-03,1.07E-02,1.44E-02,2.42E-02,2.71E-02,3.00E-02,5.64E-02,6.39E-02,8.32E-02,8.21E-02,5.69E-02,3.94E-02,3.01E-02,-1.17E-02,-1.84E-02,-3.15E-02,-4.43E-02,-1.48E-01,-1.40E-01,-2.36E-01,0,0,0};
//   Float_t x_optimx[NUM] = {0.157,0.157,0.157,0.156,0.156,0.156,0.156,0.155,0.109,0.094,0.074,0.06,0.052,0.04,-0.019,-0.033,-0.043,-0.072,-0.059,-0.072,5.8E-5,0,0,0};
//   Float_t y_optimy[NUM] = {0.081,0.081,0.081,0.081,0.081,0.081,0.081,0.081,0.081,0.096,0.092,0.058,0.034,0.021,-0.033,-0.036,-0.050,-0.057,-0.16,-0.14,-0.24,0,0,0};
//   Float_t e_optimx[NUM] = {1.78E-004,1.57E-004,1.18E-004,9.98E-005,5.02E-005,3.56E-005,2.11E-005,-1.12E-004,-1.50E-004,-2.19E-004,-2.01E-004,-1.99E-004,-1.80E-004,-1.68E-004,-9.72E-005,-1.01E-004,-1.06E-004,-1.61E-004,-7.38E-002,-3.43E-001,-1.41E-001,4.41E-005,3.63E-005,2.22E-005};
//   Float_t xp_optimx[NUM] = {0.004,0.00002,0.007,0.0109,0.02,0.023,0.026,0.052,0.059,0.062,0.058,0.064,0.062,0.059,0.047,0.052,0.04,0.038,-0.036,-0.14,-0.034,0,0,0};
//   Float_t yp_optimy[NUM] = {0.001,0.004,0.01,0.013,0.01,0.022,0.024,0.04,0.047,0.079,0.078,0.056,0.041,0.033,-0.005,-0.012,-0.025,-0.04,-0.14,-0.14,-0.23,0,0,0};
//   Float_t x_optimy[NUM] = {3.68E-07,3.62E-07,3.50E-07,3.44E-07,3.29E-07,3.25E-07,3.20E-07,2.79E-07,2.68E-07,1.75E-07,1.55E-07,1.46E-08,-8.86E-08,-1.03E-07,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00};
//   Float_t y_optimx[NUM] = {-3.29E-07,-3.21E-07,-3.05E-07,-2.98E-07,-2.78E-07,-2.73E-07,-2.67E-07,-2.14E-07,-2.00E-07,-1.22E-07,-8.65E-08,-1.71E-08,3.68E-08,6.47E-08,1.90E-07,2.46E-07,2.41E-07,3.10E-07,9.95E-08,3.23E-14,9.30E-15,-3.90E-14,-3.40E-14,-2.53E-14};
//   Float_t e_optimy[NUM] = {-8.88E-004,-8.89E-004,-8.89E-004,-8.89E-004,-8.89E-004,-8.89E-004,-8.89E-004,1.89E-007,1.32E-007,-1.22E-007,-1.50E-007,2.42E-002,5.03E-002,5.03E-002,0,0,0,0,0,0,0,0,0,0};
//   Float_t xp_optimy[NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//   Float_t yp_optimx[NUM] = {-2.34E-07,-2.25E-07,-2.08E-07,-2.00E-07,-1.78E-07,-1.72E-07,-1.65E-07,-1.07E-07,-9.03E-08,-2.95E-08,-1.19E-08,2.95E-08,5.82E-08,7.25E-08,1.34E-07,1.68E-07,1.59E-07,1.95E-07,5.31E-08,3.23E-14,9.30E-15,-3.90E-14,-3.40E-14,-2.53E-14};

  Double_t ymax,ymin,qline,bpmline,qtext,bpmtext,qhtext,bpmhtext,ypmax,ypmin,yp,ypa;
//   ymax=0.2;ymin=-0.2;qline=-0.185;bpmline=0.187;qtext=-0.170;bpmtext=0.170;qhtext=-0.168;bpmhtext=0.172;
//  ymax=0.25;ymin=-0.25;qline=-0.2345;bpmline=0.2347;qtext=-0.23415;bpmtext=0.225;qhtext=-0.23416;bpmhtext=0.225;
   ymax=0.6;ymin=-0.6;qline=-0.555;bpmline=0.57;qtext=-0.545;bpmtext=0.558;qhtext=-0.545;bpmhtext=0.548;
//    ymax=0.5;ymin=-0.5;qline=-0.455;bpmline=0.47;qtext=-0.445;bpmtext=0.458;qhtext=-0.445;bpmhtext=0.448;
//   ymax=0.8;ymin=-0.8;qline=-0.755;bpmline=0.77;qtext=-0.745;bpmtext=0.758;qhtext=-0.745;bpmhtext=0.748;
  TLine * l;
  TLine * q07,q08,q11,q12,q13,q16,q17,q18,q19,q20,q21,m1,m3,dcc1,dcc2,dcc3,dcc4,bmod1,bmod2;
  TLine * c07a,c07,c08,c11,c12,c14,c16,c17,c18,c19,p02a,p02b,p03a,c20,c21,h02,h04,h07a,h07b,h07c,h08,h09,h09b;
  TText * tq07,tq08,tq11,tq12,tq13,tq16,tq17,tq18,tq19,tq20,tq21,tm1,tbmod,ttarget;
  TText * tc07a,tc07,tc08,tc11,tc12,tc14,tc16,tc17,tc18,tc19,tp02a,tp02b,tp03a,tc20,tc21,th02,th04,th07a,th07b,th07tc,th08,th09,th09b;

//   Double_t amp1[NUM],eamp1[NUM],z1[NUM],ez1[NUM],optim1[NUM],eoptim1[NUM],x_optimx1[NUM],ex_optimx1[NUM],y_optimy1[NUM],ey_optimy1[NUM],e_optimx1[NUM],ee_optimx1[NUM],xp_optimx1[NUM],exp_optimx1[NUM],yp_optimy1[NUM],eyp_optimy1[NUM],e_optimy1[NUM],ee_optimy1[NUM],control_optim1[NUM],econtrol_optim1[NUM];

  TString s = "ramp>0 && (ramp.Device_Error_Code&0x80)==0x80 && abs((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))<50 && qwk_bcm1.Device_Error_Code==0";

  /* Create a directory by run number under plots directory */
  sprintf(dircreate,"%s/%d",dir2.Data(), run_number);
  sprintf(dircreate2,"%s",dir3.Data());
  gSystem->mkdir(dircreate);
  gSystem->mkdir(dircreate2);
  printf("%sDirectory%s %s\n%shas been created sucessfully%s\n",blue,red,dircreate,blue,normal);


  for ( Int_t pattern = 0; pattern <10; pattern++ ){

    UInt_t col=0;
    switch(pattern)
      { 
      case 0: col = kRed-4; break;
      case 1: col = kGreen; break;
      case 2: col = kBlue-7; break;
      case 3: col = kRed-4; break;
      case 4: col = kGreen; break;
      case 5: col = kRed-4; break;
      case 6: col = kGreen; break;
      case 7: col = kBlue-7; break;
      case 8: col = kRed-4; break;
      case 9: col = kGreen; break;
	//     default: Console.WriteLine("%sPlease Insert 0,1,2,3 or 4%s",blue,normal); break;
      };
    UInt_t cold=0;
    switch(pattern)
      { 
      case 0: cold = kRed-2; break;
      case 1: cold = kGreen+2; break;
      case 2: cold = kBlue; break;
      case 3: cold = kRed-2; break;
      case 4: cold = kGreen+2; break;
      case 5: cold = kRed-2; break;
      case 6: cold = kGreen+2; break;
      case 7: cold = kBlue; break;
      case 8: cold = kRed-2; break;
      case 9: cold = kGreen+2; break;
	//     default: Console.WriteLine("%sPlease Insert 0,1,2,3 or 4%s",blue,normal); break;
      };


    sprintf(saveplot1,"%s/%d/%dbpms%s%s.png",dir2.Data(),run_number,run_number,xy[pattern].Data(),var[pattern].Data());
    sprintf(title1,"Run %d: Hall-C BPM %s Response to %s-Modulation",run_number,xy[pattern].Data(),var[pattern].Data());
    sprintf(saveplot3,"%s/%d/%doptics%s%s.png",dir2.Data(),run_number,run_number,xy[pattern].Data(),var[pattern].Data());
    sprintf(title3,"Run %d: Hall-C BPM %s Response to %s-Modulation",run_number,xy[pattern].Data(),var[pattern].Data());


    /* Create a text file for saving fit parameters */
    sprintf(textfile,"%s/%dbpmfit%s%s.txt",dir3.Data(),run_number,xy[pattern].Data(),var[pattern].Data());
    ofstream outfile(textfile);

//     /* Canvas parameters */
//     gDirectory->Delete("*") ;
//     gStyle->Reset();
//     gStyle->SetCanvasColor(kWhite);
//     gStyle->SetPadColor(kWhite);
//     gStyle->SetFrameBorderMode(0);
//     gStyle->SetPadBorderSize(0);
//     gStyle->SetFrameBorderSize(0);
//     gStyle->SetFrameLineColor(kBlack);
//     gStyle->SetFrameLineWidth(2);
//     gStyle->SetPadBorderMode(0);
//     gStyle->SetTitleBorderSize(0);

    gROOT->ForceStyle();

    TCanvas *c1 = new TCanvas("c1",title1,cx1,cy1,csizx,csizy);
    TPad * padc10 = new TPad("padc10","padc10",ps1,ps2,ps4,ps4);
    TPad * padc11 = new TPad("padc11","padc11",ps1,ps1,ps4,ps3);
    padc10->Draw();
    padc11->Draw();
    padc10->cd();
    TText * ct10 = new TText(tll,tlr,title1);
    ct10->SetTextSize(tsiz);
    ct10->Draw();
    padc11->cd();
    padc11->Divide(4,6);


    for(Int_t i=0; i< NUM; i++) 
      {
	gDirectory->Delete("*") ;
	gStyle->Reset();
	gStyle->SetOptTitle(1);
	gStyle->SetCanvasColor(kWhite);
	gStyle->SetPadColor(kWhite);
	gStyle->SetFrameBorderMode(0);
	gStyle->SetPadBorderSize(0);
	gStyle->SetFrameBorderSize(0);
	gStyle->SetFrameLineColor(kBlack);
	gStyle->SetFrameLineWidth(2);
	gStyle->SetPadBorderMode(0);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetNdivisions(506,"y");
	gStyle->SetTitleSize(0.04);
	gStyle->SetTitleYSize(0.04);
	gStyle->SetTitleXSize(0.04);
	gStyle->SetTitleYOffset(1.00);
	gStyle->SetTitleXOffset(1.00);
	gStyle->SetLabelSize(0.055);
	gStyle->SetLabelSize(0.055,"y");
	gStyle->SetLabelSize(0.055,"x");
	gStyle->SetOptFit(1111);
	gStyle->SetOptStat("e");
	gStyle->SetStatY(0.99);
	gStyle->SetStatX(0.99);
	gStyle->SetStatW(0.12);
	gStyle->SetStatH(0.15);
	gStyle->SetTextFont(62);
	gStyle->SetTitleFont(42);
	gStyle->SetTitleFont(42,"y");
	gStyle->SetTitleFont(42,"x");
	gStyle->SetLabelFont(42);
	gStyle->SetLabelFont(42,"y");
	gStyle->SetLabelFont(42,"x");
	gROOT->ForceStyle();

	Int_t idx = i+1;
	padc11->cd(idx);
	padc11->cd(idx)->SetGrid();
	sprintf(drawstringh1,"qwk_%s%s:ramp*76.29e-6*1200>>%s",bpms[i].Data(),xy[pattern].Data(),bpms[i].Data());
	sprintf(histo1,"%s",bpms[i].Data());
	sprintf(cut1,"qwk_%s%s.Device_Error_Code==0 && %s && %s",bpms[i].Data(),xy[pattern].Data(),s.Data(),ptt[pattern].Data());
	sprintf(yaxis1,"%s-%s [mm]",bpms[i].Data(),xy[pattern].Data());
	sprintf(padtitle1,"%s-%s vs Ramp Signal",bpms[i].Data(),xy[pattern].Data());

	chain->Draw(drawstringh1,cut1,"prof");
	TProfile *bpm = (TProfile*)gDirectory->Get(histo1);
	Long64_t hnx = bpm->GetEntries();
	if(!hnx){printf("%sNo modulation data.%s\n",blue,normal);}
      
	else{

	  Double_t limit = bpm->GetMean(2);
	  if(pattern==0){ypmax= limit+0.20;ypmin= limit-0.20;}
	  if(pattern==1){ypmax= limit+0.30;ypmin= limit-0.30;}
	  if(pattern==2){ypmax= limit+0.35;ypmin= limit-0.35;}
	  if(pattern==3){ypmax= limit+0.15;ypmin= limit-0.15;}
	  if(pattern==4){ypmax= limit+0.30;ypmin= limit-0.30;}
	  if(pattern==5){ypmax= limit+0.10;ypmin= limit-0.10;}
	  if(pattern==6){ypmax= limit+0.05;ypmin= limit-0.05;}
	  if(pattern==7){ypmax= limit+0.10;ypmin= limit-0.10;}
	  if(pattern==8){ypmax= limit+0.10;ypmin= limit-0.10;}
	  if(pattern==9){ypmax= limit+0.05;ypmin= limit-0.05;}
// 	  else{ypmax= limit+0.10;ypmin= limit-0.10;}
	  //       ypmax= limit*1.20;ypmin= limit*0.80;

	  bpm->Clone();
	  bpm->SetLineColor(col);
	  bpm->SetMarkerColor(col);
	  bpm->Draw("goff");
	  bpm->GetYaxis()->SetTitle(yaxis1);
	  bpm->GetXaxis()->SetTitle("PHASE [degree]");
	  bpm->SetTitle(padtitle1);
	  bpm->GetYaxis()->SetRangeUser(ypmin,ypmax);

	  TF1* func = new TF1("func","[0] + [1]*sin(((pi/180)*x)+[2])",10,350);
	  // TF1* func = new TF1("func","[0] + [1]*(sin((pi/180)*x)*cos([2])+cos((pi/180)*x)*sin([2])) ",10,350);

	  Double_t meanval = bpm->GetMean(2);
	  Int_t minbin = bpm->GetMinimumBin();
	  Double_t minpos = bpm->GetXaxis()->GetBinCenter(minbin);
// 	  func->SetParameters(meanval,0.020,minpos);
	  func->SetParameters(meanval,0.020,0);
	  func->SetParLimits(2,0,90);
// 	  func->SetParLimits(2,0,92);
	  //func->SetParLimits(2,90,180);
	  bpm->Fit("func","E M R F 0 Q");
	  bpm->DrawCopy("9");
	  func->SetLineColor(cold);
	  func->SetLineWidth(2);
	  func->Draw("same");

	  amp[i] = func->GetParameter(1);
	  amp_e[i] = func->GetParError(1);
	  phase[i] = func->GetParameter(2);
	  angle[i] = func->Derivative(phase[i] = 180);

	if(pattern==3){
	  if(i<4){
	    if (amp[i]<0) xact[i]= signm*amp[i];
	  if (amp[i]>0) xact[i]= signp*amp[i];}
	  else{	
	  if (amp[i]<0) modamp[i]= signm*amp[i];
	  if (amp[i]>0) modamp[i]= signp*amp[i];

	  if (angle[i]<0) xact[i]= signm*modamp[i];
	  if (angle[i]>0) xact[i]= signp*modamp[i];}
	}

	else{
	
	  if (amp[i]<0) modamp[i]= signm*amp[i];
	  if (amp[i]>0) modamp[i]= signp*amp[i];

	  if (angle[i]<0) xact[i]= signm*modamp[i];
	  if (angle[i]>0) xact[i]= signp*modamp[i];
	}

	  printf("%s%d: %s%s-Modulation: %s%s%s%s Amplitude \t=%s %7.4f %7.4f+-%7.4f %smm%s\n",red,run_number,var[pattern].Data(),blue,red,bpms[i].Data(),xy[pattern].Data(),blue,red,xact[i],amp[i],amp_e[i],blue,normal);

	  sprintf(textfit,"%7.4f \t%7.4f",xact[i],amp_e[i]);
	  outfile <<textfit<<endl;

	  gPad->Update();
 	}

      }
    c1->Update();
    c1->SaveAs(saveplot1);
//     delete bpm;

  }

  printf("%s%s#*%s   Congratulation !!!!!   All plots are sucessfully saved in directory %s*#%s\n",dline,red,blue,red,normal);  
  printf("%s#%s %s/%d %s#%s\n%s",red,blue,dir2.Data(),run_number,red,normal,dline);
}
/******************************************************************************************************/
/*                                        USER MANUAL                                                 */
/*                                                                                                    */
/*Step1: Open a terminal and goto directory "/net/cdaqfs/home/cdaq/beamModulation".Open root by       */
/*       doing: "root -l". To run without display add option "-b" with "-l".                          */
/*Step2: Execute the macro inside root using:".x bmod_bpms.C"                                         */
/*Step3: It will ask for a 'run #'. Type 'run #' and press 'ENTER'.                                   */
/*                                                                                                    */
/*       Plots, run information and fit parameters will be saved acording to run # in the directory:  */
/*       '/u/group/hallc/www/hallcweb/html/qweak/bmod/images/run#'                                    */
/*       Another script bmod_beamline.C will use the text file and create necessary files and submit  */
/*       it to BMod website: https://hallcweb.jlab.org/qweak/bmod/                                    */
/******************************************************************************************************/
