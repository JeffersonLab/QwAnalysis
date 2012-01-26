/**************************************************************************/
/*                              Nuruzzaman                                */
/*                              10/22/2011                                */
/*                                                                        */
/*                        Last Edited:01/22/2012                          */
/*                                                                        */
/* This Macro create plots for beamline optics using beam modulation.     */
/* Please see the USER MANUAL at the bottom of this macro for its usage.  */
/**************************************************************************/
#include <NurClass.h>
void bmod_beamline2(  UInt_t run_number = 0){
  time_t tStart = time(0), tEnd; 
  const Int_t NUM = 23;
  char text_check[255];
  char dircreate[255],saveplot1[255],title1[255];

//   /* Ask to input the run condition to user */
//   printf("%s%sEnter Run Number%s\n%s",SolidLine.Data(),blue,normal,SolidLine.Data()); 
//   cin >> run_number;

//   TString dir = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots";
  TString bmodDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod";
  TString textDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/text";
  TString imageDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/images";
  TString archiveDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/archive";
  TString imageWeb = "https://hallcweb.jlab.org/qweak/bmod/images";
  TString archiveWebHtml = "https://hallcweb.jlab.org/qweak/bmod/archive.html";
  TString archiveWeb = "https://hallcweb.jlab.org/qweak/bmod/archive";

  TString xy[10] = {"X","Y","X","X","Y","Y","X","Y","Y","X"};
  TString mod[10] = {"X","Y","E","Xp","Yp","X","Y","E","Xp","Yp"};
  TString modtype[10] = {"XX","YY","XE","XXp","YYp","YX","XY","YE","YXp","XYp"};
  TString bpms[NUM] = {"target","bpm3h09","bpm3h08","bpm3h07c","bpm3h07b","bpm3h07a",
		       "bpm3h04","bpm3h02","bpm3c21","bpm3c20","bpm3p03a","bpm3p02b","bpm3p02a",
		       "bpm3c19","bpm3c18","bpm3c17","bpm3c16","bpm3c14","bpm3c12","bpm3c11",
		       "bpm3c08","bpm3c07a","bpm3c07"};
  TString l_xx = "BPM-X response to X-Modulation";
  TString l_yy = "BPM-Y response to Y-Modulation";
  TString l_xe = "BPM-X response to E-Modulation";
  TString l_ye = "BPM-Y response to E-Modulation";

  sprintf(text_check,"%s/%dbpmfitXX.txt",textDirectory.Data(),run_number);
  FILE *check;
  check = fopen(text_check,"r");
  if (check == NULL) {printf("%sNo Modulation data for this run !!!. Exiting Program\n%s",red,contact.Data());exit(1);}
  

//   UInt_t pattern = 0;
//   printf("%sPlease Insert %s0%s for X, %s1%s for Y,%s2%s for E, %s3%s for Xp and %s4%s for Yp and hit ENTER\n%s",SolidLine.Data(),blue,red,blue,red,blue,red,blue,red,blue,red,blue,normal,SolidLine.Data());
//   cin >> pattern;
//   if (pattern> 9) {printf("%sPlease insert a correct No. Exiting the program!%s\n",blue,normal); exit(1);}


  /* Canvas parameters */
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
  gStyle->SetTitleSize(0.030);
  gStyle->SetTitleYSize(0.030);
  gStyle->SetTitleXSize(0.030);
  gStyle->SetTitleYOffset(1.20);
  gStyle->SetTitleXOffset(1.30);
  gStyle->SetLabelSize(0.030);
  gStyle->SetLabelSize(0.030,"y");
  gStyle->SetLabelSize(0.030,"x");
  gStyle->SetNdivisions(506,"y");
  //  gStyle->SetNdivisions(-306,"y");
  gStyle->SetTickLength(0.02,"y");
  gStyle->SetTickLength(0.02,"x");
  gStyle->SetOptStat("e");
  gStyle->SetOptFit(1111);
  gStyle->SetStatFontSize(0.045);
  gStyle->SetStatY(1.0);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.10); 
  //   gStyle->SetLineWidth(0);
  //  gStyle->SetTextFont(4);
  gStyle->SetTextFont(62);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");
  gROOT->ForceStyle();

  Float_t signp,signm;
  signp=1;signm=-1;

  Double_t csizx,csizy,csiz3x,csiz3y,cx1,cy1,cx2,cx3,tsiz,tsiz3,tll,tlr,ps1,ps2,ps3,ps4;
  csizx=1600;csizy=1300;csiz3x=1100;csiz3y=780;tsiz=0.40;tsiz3=0.45;tll=0.012;tlr=0.4;cx1=0;cy1=0;cx2=300;cx3=600;
  ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;

  /* Create a directory by run number under plots directory */
  sprintf(dircreate,"%s/%d",imageDirectory.Data(), run_number);
  gSystem->mkdir(dircreate);
  gSystem->Chmod(dircreate, 509);  /* 509 decimal == 775 octal, giving user permission to overwrite */
  printf("%sDirectory%s %s\n%shas been created sucessfully%s\n",blue,red,dircreate,blue,normal);

  for ( Int_t pattern = 0; pattern <10; pattern++ ){

  UInt_t col=0;
  switch(pattern)
    { 
    case 0: col = kRed; break;
    case 1: col = kGreen; break;
    case 2: col = kBlue; break;
    case 3: col = kRed; break;
    case 4: col = kGreen; break;
    case 5: col = kRed; break;
    case 6: col = kGreen; break;
    case 7: col = kBlue; break;
    case 8: col = kRed; break;
    case 9: col = kGreen; break;
      //     default: Console.WriteLine("%sPlease Insert 0,1,2,3 or 4%s",blue,normal); break;
    };
  
  
  sprintf(saveplot1,"%s/%d/%doptics%s.png",imageDirectory.Data(),run_number,run_number,modtype[pattern].Data());
  sprintf(title1,"%d: Hall-C BPM-%s Response to %s-Modulation",run_number,xy[pattern].Data(),mod[pattern].Data());

  TCanvas *c1 = new TCanvas("c1",title1,0,0,1300,900);
  TPad *padc10 = new TPad("padc10","padc10",ps1,ps2,ps4,ps4);
  TPad *padc11 = new TPad("padc11","padc11",ps1,ps1,ps4,ps3);
  padc10->Draw();
  padc11->Draw();
  padc10->cd();
  TText* ct10 = new TText(0.50,0.50,title1);
  ct10->SetTextSize(tsiz3);
  ct10->SetTextFont(42);
  ct10->SetTextAlign(21);
  ct10->Draw();
  padc11->cd();
  padc11->cd()->SetGrid();
  //  c1->cd();

  Double_t ymax,ymin,qline,bpmline,qtext,bpmtext,qhtext,bpmhtext;
  Double_t amp[NUM],amp2[NUM],eamp[NUM],phase[NUM],ephase[NUM],z[NUM],ez[NUM],optim[NUM],eoptim[NUM],x_optimx[NUM],ex_optimx[NUM],y_optimy[NUM],ey_optimy[NUM],e_optimx[NUM],ee_optimx[NUM],xp_optimx[NUM],exp_optimx[NUM],yp_optimy[NUM],eyp_optimy[NUM],e_optimy[NUM],ee_optimy[NUM],control_optim[NUM],econtrol_optim[NUM];

  if (pattern==2){
    ymax=0.8;ymin=-0.8;qline=-0.755;bpmline=0.77;qtext=-0.745;bpmtext=0.758;qhtext=-0.745;bpmhtext=0.748;
//     ymax=0.6;ymin=-0.6;qline=-0.555;bpmline=0.57;qtext=-0.545;bpmtext=0.558;qhtext=-0.545;bpmhtext=0.548;
    printf("Changing scale for energy\n");}
  else{
//     ymax=0.25;ymin=-0.25;qline=-0.2345;bpmline=0.2347;qtext=-0.23215;bpmtext=0.225;qhtext=-0.23216;bpmhtext=0.225;
    ymax=0.5;ymin=-0.5;qline=-0.455;bpmline=0.47;qtext=-0.445;bpmtext=0.458;qhtext=-0.445;bpmhtext=0.448;
    printf("No change in scale\n");}

  // Open data file
  int counter=0;
  Int_t counter2 = 0;
  ifstream in,inz,inx_optimx,iny_optimy,ine_optimx,inxp_optimx,inyp_optimy,ine_optimy,incontrol_optim;
  in.open(Form("%s/%dbpmfit%s.txt",textDirectory.Data(),run_number,modtype[pattern].Data()));
  inz.open(Form("%s/z.txt",textDirectory.Data()));
  inx_optimx.open(Form("%s/x_optimx.txt",textDirectory.Data()));
  iny_optimy.open(Form("%s/y_optimy.txt",textDirectory.Data()));
  ine_optimx.open(Form("%s/e_optimx.txt",textDirectory.Data()));
  inxp_optimx.open(Form("%s/xp_optimx.txt",textDirectory.Data()));
  inyp_optimy.open(Form("%s/yp_optimy.txt",textDirectory.Data()));
  ine_optimy.open(Form("%s/e_optimy.txt",textDirectory.Data()));
  incontrol_optim.open(Form("%s/control_optim.txt",textDirectory.Data()));

  while (1) {
    // Read data file
    in >>amp2[counter2]>>eamp[counter2]>>phase[counter2]>>ephase[counter2];
    if (!in.good()) break;
    // Read OPTIM files
    inz >>z[counter2]>>ez[counter2];
    if (!inz.good()) break;
    inx_optimx >>x_optimx[counter2]>>ex_optimx[counter2];
    if (!inx_optimx.good()) break;
    iny_optimy >>y_optimy[counter2]>>ey_optimy[counter2];
    if (!iny_optimy.good()) break;
    ine_optimx >>e_optimx[counter2]>>ee_optimx[counter2];
    if (!ine_optimx.good()) break;
    inxp_optimx >>xp_optimx[counter2]>>exp_optimx[counter2];
    if (!inxp_optimx.good()) break;
    inyp_optimy >>yp_optimy[counter2]>>eyp_optimy[counter2];
    if (!inyp_optimy.good()) break;
    ine_optimy >>e_optimy[counter2]>>ee_optimy[counter2];
    if (!ine_optimy.good()) break;
    incontrol_optim >>control_optim[counter2]>>econtrol_optim[counter2];
    if (!incontrol_optim.good()) break;

    Double_t tune;
    tune = 1;

    if (counter2<7){
      amp[counter2]= TMath::Abs(amp2[counter2]);
    }
    else{
      amp[counter2]= amp2[counter2];
    }

  if (pattern==0) {optim[counter2]=tune*x_optimx[counter2]; eoptim[counter2]=ex_optimx[counter2];}
  if (pattern==1) {optim[counter2]=tune*y_optimy[counter2]; eoptim[counter2]=ey_optimy[counter2];}
  if (pattern==2) {optim[counter2]=e_optimx[counter2]; eoptim[counter2]=ee_optimx[counter2];}
  if (pattern==3) {optim[counter2]=tune*xp_optimx[counter2]; eoptim[counter2]=exp_optimx[counter2];}
  if (pattern==4) {optim[counter2]=tune*yp_optimy[counter2]; eoptim[counter2]=eyp_optimy[counter2];}
  if (pattern==5) {optim[counter2]=tune*control_optim[counter2]; eoptim[counter2]=econtrol_optim[counter2];}
  if (pattern==6) {optim[counter2]=tune*control_optim[counter2]; eoptim[counter2]=econtrol_optim[counter2];}
  if (pattern==7) {optim[counter2]=e_optimy[counter2]; eoptim[counter2]=ee_optimy[counter2];}
  if (pattern==8) {optim[counter2]=tune*control_optim[counter2]; eoptim[counter2]=econtrol_optim[counter2];}
  if (pattern==9) {optim[counter2]=tune*control_optim[counter2]; eoptim[counter2]=econtrol_optim[counter2];}
//   else{optim[counter2]=tune*control_optim[counter2]; eoptim[counter2]=econtrol_optim[counter2]; printf("Plotting Controls-OPTIM\n");}

  printf("%s%i %s-Modulation %s%s%s %s\t%7.6f \t%7.6f%s\n",blue,counter,mod[pattern].Data(),green,bpms[counter].Data(),xy[pattern].Data(),red,amp[counter],eamp[counter],normal);

  counter2++;
  counter = counter2+1;    
  }

  padc11->cd();
  TGraphErrors *gr_o = new TGraphErrors(counter,z,optim,ez,eoptim);
  gr_o->SetName("gr_o");
  gr_o->SetMarkerColor(col);
  gr_o->SetLineColor(col);
  gr_o->SetMarkerStyle(21);
  gr_o->SetMarkerSize(0.6);
  gr_o->SetLineWidth(1);
  gr_o->Draw("0");
  //    gr_o->GetYaxis()->SetRangeUser(scalemin,scalemax);
  //  gr_o->GetYaxis()->SetTitle("BMod Target Position Amplitude [mm]");
  //  // gr_o->GetXaxis()->SetTitle("RUN NUMBER");
  //  gr_o->SetTitle("Hall-C Beamline OPTICS");
  //  gr_o->GetYaxis()->SetRangeUser(-0.08,0.35);

  TGraphErrors *gr_d = new TGraphErrors(counter,z,amp,ez,eamp);
  gr_d->SetName("gr_d");
  gr_d->SetMarkerColor(col);
  gr_d->SetLineColor(col);
  gr_d->SetMarkerStyle(24);
  gr_d->SetMarkerSize(0.7);
  gr_d->SetLineWidth(1);
  gr_d->Draw("0");
  //    gr_d->GetYaxis()->SetRangeUser(scalemin,scalemax);

  TMultiGraph *gr_all = new TMultiGraph();
  gr_all->Add(gr_o);
  gr_all->Add(gr_d);
  gr_all->Draw("AP");
  gr_all->SetTitle("");
  gr_all->GetXaxis()->SetTitle("Z location from target [cm]");
  gr_all->GetYaxis()->SetTitle("BMod position amplitude [mm]");
  gr_all->GetXaxis()->CenterTitle();
  gr_all->GetYaxis()->CenterTitle();
  gr_all->GetYaxis()->SetRangeUser(ymin,ymax);
  TAxis *axis= gr_all->GetXaxis();
  axis->SetLimits(-10500,500);

  TLegend *leg = new TLegend(0.80,0.92,0.90,0.99);
  gr_o->SetMarkerColor(col);
  gr_o->SetLineColor(col);
  gr_d->SetMarkerColor(col);
  gr_d->SetLineColor(col);
  leg->AddEntry("gr_o","OPTIM","lp");
  leg->AddEntry("gr_d","Data","lp");
  leg->SetTextSize(0.020);
  leg->SetFillColor(0);
  leg->SetBorderSize(1);
  leg->Draw();

  gPad->Update();


  /********************************************************************************************/
  // Beamline elements are shown here:

  gStyle->SetLineStyle(1);
  gStyle->SetLineWidth(3);
  gStyle->SetLineColor(kBlack);
  TLine * l = new TLine(-10450, 0, 480, 0); 
  l->SetLineStyle(3);
  l->Draw();
  TArrow * ar = new TArrow(320,0,480,0,0.015,"|>");
  ar->SetFillStyle(3008);
  ar->Draw();

  gStyle->SetLineColor(kRed);
  TLine * q07 = new TLine(-9918.9, ymin, -9918.9, qline);
  TLine * q08 = new TLine(-9308.9, ymin, -9308.9, qline); 
  TLine * q11 = new TLine(-7748.9, ymin, -7748.9, qline); 
  TLine * q12 = new TLine(-7228.9, ymin, -7228.9, qline); 
  TLine * q13 = new TLine(-6708.9, ymin, -6708.9, qline); 
  TLine * q16 = new TLine(-5148.9, ymin, -5148.9, qline); 
  TLine * q17 = new TLine(-4838.0, ymin, -4838.0, qline); 
  TLine * q18 = new TLine(-4660.3, ymin, -4660.3, qline); 
  TLine * q19 = new TLine(-4529.2, ymin, -4529.9, qline); 
  TLine * q20 = new TLine(-3135.7, ymin, -3135.7, qline); 
  TLine * q21 = new TLine(-2999.9, ymin, -2999.9, qline); 
  q07->Draw();q08->Draw();q11->Draw();q12->Draw();q13->Draw();q16->Draw();q17->Draw();q18->Draw();q19->Draw();q20->Draw();q21->Draw();

  gStyle->SetLineColor(kGreen);
  TLine * m1 = new TLine(-2803.4, ymin, -2803.4, qline); 
  TLine * m3 = new TLine(-2453.4, ymin, -2453.4, qline); 
  m1->Draw();m3->Draw();

  gStyle->SetLineColor(kBlue);
  TLine * dcc1 = new TLine(-4380.1, ymin, -4380.1, qline);
  TLine * dcc2 = new TLine(-4055.7, ymin, -4055.7, qline);
  TLine * dcc3 = new TLine(-3710.7, ymin, -3710.7, qline);
  TLine * dcc4 = new TLine(-3386.3, ymin, -3386.3, qline);
  dcc1->Draw();dcc2->Draw();dcc3->Draw();dcc4->Draw();

  gStyle->SetLineColor(kBlack);
  TLine * bmod1 = new TLine(-9266.9, ymin, -9266.9, qline);
  TLine * bmod2 = new TLine(-8226.9, ymin, -8226.9, qline);
  TLine * target = new TLine(0, ymin, 0, qline);  
  bmod1->Draw();bmod2->Draw();target->Draw();

  gStyle->SetLineWidth(2);
  gStyle->SetLineColor(kBlack);
  TLine * c07a = new TLine(-9971.37, bpmline, -9971.37, ymax);
  TLine * c07 = new TLine(-9570.71, bpmline, -9570.71, ymax);
  TLine * c08 = new TLine(-9361.37, bpmline, -9361.37, ymax);
  TLine * c11 = new TLine(-7801.37, bpmline, -7801.37, ymax);
  TLine * c12 = new TLine(-7281.29, bpmline, -7281.29, ymax);
  TLine * c14 = new TLine(-6241.37, bpmline, -6241.37, ymax);
  TLine * c16 = new TLine(-5201.29, bpmline, -5201.29, ymax);
  TLine * c17 = new TLine(-4892.34, bpmline, -4892.34, ymax);
  TLine * c18 = new TLine(-4712.8, bpmline, -4712.8, ymax);
  TLine * c19 = new TLine(-4581.7, bpmline, -4581.7, ymax);
  TLine * p02a = new TLine(-3952.7, bpmline, -3952.7, ymax);
  TLine * p02b = new TLine(-3813.7, bpmline, -3813.7, ymax);
  TLine * p03a = new TLine(-3544.7, bpmline, -3544.7, ymax);
  TLine * c20 = new TLine(-3188.1, bpmline, -3188.1, ymax);
  TLine * c21 = new TLine(-3052.3, bpmline, -3052.3, ymax);
  TLine * h02 = new TLine(-2158.6, bpmline, -2158.6, ymax);
  TLine * h04 = new TLine(-1910.7, bpmline, -1910.7, ymax);
  TLine * h07a = new TLine(-1033.3, bpmline, -1033.3, ymax);
  TLine * h07b = new TLine(-937.6, bpmline, -937.6, ymax);
  TLine * h07c = new TLine(-842.0, bpmline, -842.0, ymax);
  TLine * h08 = new TLine(-516.3, bpmline, -516.3, ymax);
  TLine * h09 = new TLine(-393.6, bpmline, -393.6, ymax);
  TLine * h09b = new TLine(-138.8, bpmline, -138.8, ymax);
  c07a->Draw();c07->Draw();c08->Draw();c11->Draw();c12->Draw();c14->Draw();c16->Draw();c17->Draw();c18->Draw();c19->Draw();p02a->Draw();p02b->Draw();p03a->Draw();c20->Draw();c21->Draw();h02->Draw();h04->Draw();h07a->Draw();h07b->Draw();h07c->Draw();h08->Draw();h09->Draw();h09b->Draw();

  gStyle->SetLineWidth(1);
  gStyle->SetLineColor(kBlack);
  gStyle->SetTextSize(0.016);
  gStyle->SetTextAngle(45);
  TText * tq07 = new TText(-9918.9, qtext,"qMQA3C07");
  TText * tq08 = new TText(-9308.9, qtext,"qMQA3C08");
  TText * tq11 = new TText(-7748.9, qtext,"qMQA3C11");
  TText * tq12 = new TText(-7228.9, qtext,"qMQA3C12");
  TText * tq13 = new TText(-6708.9, qtext,"qMQA3C13");
  TText * tq16 = new TText(-5148.9, qtext,"qMQA3C16");
  TText * tq17 = new TText(-4838.0, qtext,"qMQA3C17");
  TText * tq18 = new TText(-4660.3, qtext,"qMQA3C18");
  TText * tq19 = new TText(-4429.2, qtext,"qMQA3C19");
  TText * tq20 = new TText(-3155.7, qtext,"qMQA3C20");
  TText * tq21 = new TText(-2950.9, qtext,"qMQA3C21");
  tq07->Draw();tq08->Draw();tq11->Draw();tq12->Draw();tq13->Draw();tq16->Draw();tq17->Draw();tq18->Draw();tq19->Draw();tq20->Draw();tq21->Draw();

  //      gStyle->SetTextSize(0.006);
  gStyle->SetTextAngle(-45);
  TText * tc07a = new TText(-9991.37, bpmtext,"3C07A");
  TText * tc07 = new TText(-9671.37, bpmtext,"3C07");
  TText * tc08 = new TText(-9411.37, bpmtext,"3C08");
  TText * tc11 = new TText(-7831.37, bpmtext,"3C11");
  TText * tc12 = new TText(-7311.37, bpmtext,"3C12");
  TText * tc14 = new TText(-6301.37, bpmtext,"3C14");
  TText * tc16 = new TText(-5219.37, bpmtext,"3C16");
  TText * tc17 = new TText(-4941.37, bpmtext,"3C17");
  TText * tc18 = new TText(-4731.37, bpmtext,"3C18");
  TText * tc19 = new TText(-4531.37, bpmtext,"3C19");
  TText * tp02a = new TText(-4021.37, bpmtext,"3P02A");
  TText * tp02b = new TText(-3811.37, bpmtext,"3P02B");
  TText * tp03a = new TText(-3511.37, bpmtext,"3P03A");
  TText * tc20 = new TText(-3221.37, bpmtext,"3C20");
  TText * tc21 = new TText(-3021.37, bpmtext,"3C21");
  TText * th02 = new TText(-2211.37, bpmtext,"3H02");
  TText * th04 = new TText(-1921.37, bpmtext,"3H04");
  TText * th08 = new TText(-541.37, bpmtext,"3H08");
  TText * th09 = new TText(-381.37, bpmtext,"3H09");
  TText * th09b = new TText(-121.37, bpmtext,"3H09B");
  tc07a->Draw(); tc07->Draw();tc08->Draw();tc11->Draw();tc12->Draw();tc14->Draw();tc16->Draw();tc17->Draw();tc18->Draw();tc19->Draw();tp02a->Draw();tp02b->Draw();tp03a->Draw();tc20->Draw();tc21->Draw();th02->Draw();th04->Draw();th08->Draw();th09->Draw();th09b->Draw();

  gStyle->SetTextAngle(0);
  TText * tdcc = new TText(-4255.4, qhtext,"COMPTON Ds");
  TText * tbmod = new TText(-9166.9, qhtext,"BMOD COIL PAIR");
  TText * tm1 = new TText(-2770.9, qhtext,"MOLLER-Q");
  TText * ttarget = new TText(-250, qhtext,"TARGET");
  TText * th07 = new TText(-1241.37, bpmhtext,"3H07A,B,C");
  tdcc->Draw();tbmod->Draw();ttarget->Draw();th07->Draw();tm1->Draw();

  gPad->Update();

  /********************************************************************************************/



  c1->Update();
  c1->SaveAs(saveplot1);
  
  //   }

  }
  /********************************************************************************************/
  /********************************************************************************************/
  /*                  Updating information to the Beam Modulation Website                     */
  /********************************************************************************************/
  char htmlfile_main[255],htmlfile_run[255],htmlfile_archive[255],htmlfile_archive3[255];
  UInt_t run_number_i;
  run_number_i = run_number-25;
//   run_number_f = 12200;

  Double_t fig_width,fig_height;
  fig_width = 500;fig_height = 350;

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  /* Create a text file for saving fit parameters */
  sprintf(htmlfile_main,"%s/index.html",bmodDirectory.Data());
  ofstream outfile_html(htmlfile_main);
  sprintf(htmlfile_run,"%s/optics.html",bmodDirectory.Data());
  ofstream outfile_run(htmlfile_run);
  sprintf(htmlfile_archive,"%s/%doptics.html",archiveDirectory.Data(),run_number);
  ofstream outfile_archive(htmlfile_archive);
  sprintf(htmlfile_archive3,"%s/archive.html",bmodDirectory.Data());
  ofstream outfile_archive3(htmlfile_archive3);

  TString header = Form("<head><title>Beam Modulation</title><link rel='SHORTCUT ICON' href='bmod_logo_black.png' background='white' type='image/x-icon'></head><body><center><a href='https://hallcweb.jlab.org/qweak/bmod'><img src='bmod_logo_white.png' width='240' height='150' align='left' border='0'></a><img src='bmod_blank.png' width='240' height='150' align='right'><a name='top'><h1>Welcome to Beam Modulation Website</h1><h2>Hall-C Beamline Optics</h2><h3><a href='%s'>Archive</a></h3><p allign='right'>Last updated: %s</p>",archiveWebHtml.Data(),asctime(timeinfo));

  TString body = Form("<a href='optics.html'>Beamline Optics for Most Recent Run (%d)</a><br><br><br><a href='%s/%d/%dopticsXX.png'><img src='%s/%d/%dopticsXX.png' width='%f' height='%f' align='center' border='2'></a><a href='%s/%d/%dopticsYY.png'><img src='%s/%d/%dopticsYY.png' width='%f' height='%f' align='center' border='2'></a><p align='center'><font size='3' face='arial' color='red'><span style='padding-left:10px'>%s</font><font size='3' face='arial' color='green'><span style='padding-left:300px'>%s</font></p><br><br><a href='%s/%d/%dopticsXE.png'><img src='%s/%d/%dopticsXE.png' width='%f' height='%f' align='center' border='2'></a><a href='%s/%d/%dopticsYE.png'><img src='%s/%d/%dopticsYE.png' width='%f' height='%f' align='center' border='2'></a><p align='center'><font size='3' face='arial' color='blue'><span style='padding-left:10px'>%s</font><span style='padding-left:300px'><font size='3' face='arial' color='blue'>%s</font></p>",run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,fig_width,fig_height,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,fig_width,fig_height,l_xx.Data(),l_yy.Data(),imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,fig_width,fig_height,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,fig_width,fig_height,l_xe.Data(),l_ye.Data());

  TString body_run = Form("<h2>Beamline Optics for Run %d</h2><pre><a href='#XX' class='menu'>Bpm-X X-Mod</a>\t<a href='#YY' class='menu'>Bpm-Y Y-Mod</a>\t<a href='#XE' class='menu'>Bpm-X E-Mod</a>\t<a href='#XXp' class='menu'>Bpm-X Xp-Mod</a>\t<a href='#YYp' class='menu'>Bpm-Y Yp-Mod</a><br><br><a href='#YX' class='menu'>Bpm-Y X-Mod</a>\t<a href='#XY' class='menu'>Bpm-X Y-Mod</a>\t<a href='#YE' class='menu'>Bpm-Y E-Mod</a>\t<a href='#YXp' class='menu'>Bpm-Y Xp-Mod</a>\t<a href='#XYp' class='menu'>Bpm-X Yp-Mod</a></pre><a name='XX'><img src='%s/%d/%dopticsXX.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='YY'><img src='%s/%d/%dopticsYY.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='XE'><img src='%s/%d/%dopticsXE.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='XXp'><img src='%s/%d/%dopticsXXp.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='YYp'><img src='%s/%d/%dopticsYYp.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='YX'><img src='%s/%d/%dopticsYX.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='XY'><img src='%s/%d/%dopticsXY.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='YE'><img src='%s/%d/%dopticsYE.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='YXp'><img src='%s/%d/%dopticsYXp.png'><a href='#top' class='menu'>Go to TOP</a><br><a name='XYp'><img src='%s/%d/%dopticsXYp.png'><a href='#top' class='menu'>Go to TOP</a><br>",run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number,imageWeb.Data(),run_number,run_number);

  outfile_html <<Form("%s",header.Data())<<endl;
  outfile_html <<Form("%s",body.Data())<<endl;

  outfile_run <<Form("%s",header.Data())<<endl;
  outfile_run <<Form("%s",body_run.Data())<<endl;

  outfile_archive <<Form("%s",header.Data())<<endl;
  outfile_archive <<Form("%s",body_run.Data())<<endl;

  outfile_archive3 <<Form("%s",header.Data())<<endl;

  for(Int_t run=run_number_i; run< run_number+1; run++) 
    {
      run_number_i = run;

  sprintf(text_check,"%s/%doptics.html",archiveDirectory.Data(),run_number_i);
  FILE *check;
  check = fopen(text_check,"r");
  if (check == NULL) 
    {printf("No Modulation data for run %d !!!.\n",run);
      continue; }
  
  TString body_archive = Form("<a href='%s/%doptics.html'>%d</a><br>",archiveWeb.Data(),run_number_i,run_number_i);
  outfile_archive3 <<Form("%s",body_archive.Data())<<endl;

    }

  printf("%s%sBeam Modulation website has been updated with current run %s%d\n%s%s",DashedLine.Data(),blue,red,run_number,contact.Data(),DashedLine.Data());
  Int_t dif,hr,min,sec;
  tEnd = time(0);
  dif = difftime(tEnd, tStart);
  hr = dif/3600;
  min = dif/60-hr*60;
  sec = dif-min*60-hr*3600;
  printf("%sTime taken to complete : %s%d hour %d min %d sec%s\n",blue,red,hr,min,sec,normal);
  printf ( "%sThe current date/time is: %s%s%s\n",blue,red,asctime(timeinfo),normal);
}
/******************************************************************************************************/
/*                                        USER MANUAL                                                 */
/*                                                                                                    */
/*Step1: Open a terminal and goto directory "/net/cdaqfs/home/cdaq/beamModulation".Open root by       */
/*       doing: "root -l". To run without display add option "-b" with "-l".                          */
/*Step2: Execute the macro inside root using:".x bmod_beamline.C"                                     */
/*Step3: It will ask for a 'run #'. Type 'run#' and press 'ENTER'.                                    */
/*                                                                                                    */
/*       Plots will be saved acording to run# in the directory:                                       */
/*       '/u/group/hallc/www/hallcweb/html/qweak/bmod/images/run#'                                    */
/*       This script will also create necessary files and submit to BMod website:                     */
/*       https://hallcweb.jlab.org/qweak/bmod/                                                        */
/******************************************************************************************************/
