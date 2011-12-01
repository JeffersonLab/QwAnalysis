/**************************************************************************/
/*                              Nuruzzaman                                */
/*                              07/22/2011                                */
/*                                                                        */
/*                        Last Edited:11/10/2011                          */
/*                                                                        */
/* This Macro create plots for beamline optics using beam modulation.     */
/* Please see the USER MANUAL at the bottom of this macro for its usage.  */
/**************************************************************************/
#include <NurClass.h>
void bmod_bpms(UInt_t run_number=0)
{
  time_t tStart = time(0), tEnd; 
  char textfile[255],textfit[255],run[255], run0[255],text_check[255];

//   /* Ask to input the run condition to user */
//   printf("%s%sEnter Run Number%s\n%s",SolidLine.Data(),blue,normal,SolidLine.Data()); 
//   cin >> run_number;

  /* Directory */
  TString rootDirectory = "$QW_ROOTFILES/";
  //  TString rootDirectory = "/w/cache/mss/hallc/qweak/rootfiles/pass3/";
  TString imageDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/images";
  TString textDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/text";
//   TString imageDirectory = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots/pass3";
//   TString textDirectory = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots/pass3/bpm_response";

  sprintf(run0,"%sQwPass1_%d.000.trees.root",rootDirectory.Data(),run_number);
  TFile f(run0);
  if(!f.IsOpen()) {printf("%sUnable to find Rootfile for Run # %s%d%s.\nPlease insert a correct Run #. Exiting the program!%s\n",blue,red,run_number,blue,normal); exit(1);}

  TChain * chain = new TChain("Mps_Tree");
//   TChain chain("Mps_Tree");
  for(Int_t t=0; t<10; t++) 
    {
      sprintf(run,"%sQwPass1_%d.00%d.trees.root",rootDirectory.Data(),run_number,t);
      TFile f1(run);
      chain->Add(run);
      // 	printf("%sRootfile %s has been found%s\n",blue,run,normal);
    }


  const Int_t NUM10 = 10, NUM = 24;
  TString ptt[NUM10] = {"bm_pattern_number==11","bm_pattern_number==12","bm_pattern_number==13","bm_pattern_number==14","bm_pattern_number==15",
			"bm_pattern_number==11","bm_pattern_number==12","bm_pattern_number==13","bm_pattern_number==14","bm_pattern_number==15"};
  TString var[NUM10] = {"X","Y","E","Xp","Yp","X","Y","E","Xp","Yp"};
  TString xy[NUM10] = {"X","Y","X","X","Y","Y","X","Y","Y","X"};
  TString bpms[NUM] = {"target","bpm3h09b","bpm3h09","bpm3h08","bpm3h07c","bpm3h07b","bpm3h07a","bpm3h04","bpm3h02","bpm3c21","bpm3c20","bpm3p03a",
		       "bpm3p02b","bpm3p02a","bpm3c19","bpm3c18","bpm3c17","bpm3c16","bpm3c14","bpm3c12","bpm3c11","bpm3c08","bpm3c07a","bpm3c07"};
  
  TString d_ramp = "ramp*76.29e-6*1200";
  TString txaxis = "Beam Modulation Phase [Degree]";
  //  TString func = "[0] + [1]*sin(0.0175*x+[2])";

  char dircreate[255],dircreate2[255],saveplot1[255],title1[255];
  char drawstringh1[255],cut1[255],yaxis1[255],histo1[255],padtitle1[255];
  Double_t amp[NUM],amp_e[NUM],phase[NUM],angle[NUM],xact[NUM],modamp[NUM];

  Float_t signp,signm;
  signp = 1;signm = -1;
  Double_t csizx,csizy,csiz3x,csiz3y,cx1,cy1,cx2,cx3,tsiz,tsiz3,tll,tlr,ps1,ps2,ps3,ps4,ypmax,ypmin;
  csizx=1600;csizy=1300;csiz3x=1100;csiz3y=780;tsiz=0.40;tsiz3=0.45;tll=0.012;tlr=0.4;cx1=0;cy1=0;cx2=300;cx3=600;
  ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;

  Double_t ymax,ymin,qline,bpmline,qtext,bpmtext,qhtext,bpmhtext;
//   ymax=0.2;ymin=-0.2;qline=-0.185;bpmline=0.187;qtext=-0.170;bpmtext=0.170;qhtext=-0.168;bpmhtext=0.172;
//  ymax=0.25;ymin=-0.25;qline=-0.2345;bpmline=0.2347;qtext=-0.23415;bpmtext=0.225;qhtext=-0.23416;bpmhtext=0.225;
   ymax=0.6;ymin=-0.6;qline=-0.555;bpmline=0.57;qtext=-0.545;bpmtext=0.558;qhtext=-0.545;bpmhtext=0.548;
//    ymax=0.5;ymin=-0.5;qline=-0.455;bpmline=0.47;qtext=-0.445;bpmtext=0.458;qhtext=-0.445;bpmhtext=0.448;
//   ymax=0.8;ymin=-0.8;qline=-0.755;bpmline=0.77;qtext=-0.745;bpmtext=0.758;qhtext=-0.745;bpmhtext=0.748;

  TString s = "ramp>0 && (ramp.Device_Error_Code&0x80)==0x80 && abs((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))<50 && qwk_bcm1.Device_Error_Code==0";

  /* Create a directory by run number under plots directory */
  sprintf(dircreate,"%s/%d",imageDirectory.Data(), run_number);
  sprintf(dircreate2,"%s",textDirectory.Data());
  gSystem->mkdir(dircreate);
  gSystem->mkdir(dircreate2);
  gSystem->Chmod(dircreate, 509);  /* 509 decimal == 775 octal, giving user permission to overwrite */
  gSystem->Chmod(dircreate2, 509);
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

    sprintf(saveplot1,"%s/%d/%dbpms%s%s.png",imageDirectory.Data(),run_number,run_number,xy[pattern].Data(),var[pattern].Data());
    sprintf(title1,"Run %d: Hall-C BPM %s Response to %s-Modulation",run_number,xy[pattern].Data(),var[pattern].Data());

    /* Create a text file for saving fit parameters */
    sprintf(textfile,"%s/%dbpmfit%s%s.txt",textDirectory.Data(),run_number,xy[pattern].Data(),var[pattern].Data());
    ofstream outfile(textfile);

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
	gStyle->SetStatW(0.20);
	gStyle->SetStatH(0.16);
	gStyle->SetTextFont(42);
	gStyle->SetStatFontSize(0.10);
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
	TProfile * bpm = (TProfile*)gDirectory->Get(histo1);
	Long64_t hnx = bpm->GetEntries();
	if(!hnx){printf("%s%d: %s%s-Modulation: %s%s%s%s -- No modulation data%s\n",red,run_number,var[pattern].Data(),blue,red,bpms[i].Data(),xy[pattern].Data(),blue,normal);continue;}

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

// 	  TF1* func = new TF1("func","[0] + [1]*sin(((pi/180)*x)+[2])",10,350);
	  TF1* func = new TF1("func","[0] + [1]*sin(((pi/180)*x)+[2])",10,350);
	  // TF1* func = new TF1("func","[0] + [1]*(sin((pi/180)*x)*cos([2])+cos((pi/180)*x)*sin([2])) ",10,350);

	  Double_t meanval = bpm->GetMean(2);
	  Int_t minbin = bpm->GetMinimumBin();
	  Double_t minpos = bpm->GetXaxis()->GetBinCenter(minbin);
// 	  func->SetParameters(meanval,0.020,minpos);
// 	  func->SetParameters(meanval,0.020,0);
	  func->SetParameters(meanval,0.020,minpos);
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
// 	  printf("%s%7.4f%s\n",green,phase[i],normal);
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

	if(!hnx){printf("%s%d: %s%s-Modulation: %s%s%s%s -- No modulation data%s\n",red,run_number,var[pattern].Data(),blue,red,bpms[i].Data(),xy[pattern].Data(),blue,normal);continue;}
	else{
	  sprintf(textfit,"%7.4f \t%7.4f",xact[i],amp_e[i]);
	  outfile <<textfit<<endl;
	}
 	  gPad->Update();
 	}

      }

//   sprintf(text_check,"%s/%dbpmfit%s%s.txt",textDirectory.Data(),run_number,xy[pattern].Data(),var[pattern].Data());
//   FILE *check;
//   check = fopen(text_check,"r");
//   if (check == NULL) {printf("%sNo Modulation data for this run !!!%s\n",red,normal);
//     gSystem->Exec(Form("rm -rf %s",textfile));continue;}
//   else{
    c1->Update();
    c1->SaveAs(saveplot1);
//     //     delete bpm;
//   }
  }

  char textfile_dir_check[255];
  sprintf(textfile_dir_check,"%s/%dbpmfitXX.txt",textDirectory.Data(),run_number);
  FILE *check_dir;
  check_dir = fopen(textfile_dir_check,"r");

  if (check_dir == NULL) {printf("%sNo Modulation data for this run !!!. Exiting Program\n%s",red,contact.Data());
    /* Remove directory by run number if data does not exist */
    gSystem->Exec(Form("rm -rf %s",dircreate));
    printf("%s\nDirectory\n%s%s\n%shas been removed%s\n",blue,red,dircreate,blue,normal);exit(1);
  }

  else{
  printf("%s%s#*%s   Congratulation !!!!!   All plots are sucessfully saved in directory %s*#%s\n",DashedLine.Data(),red,blue,red,normal);  
  printf("%s#%s %s/%d %s#%s\n%s",red,blue,imageDirectory.Data(),run_number,red,normal,DashedLine.Data());
  }
  Int_t dif,hr,min,sec;
  tEnd = time(0);
  dif = difftime(tEnd, tStart);
  hr = dif/3600;
  min = dif/60-hr*60;
  sec = dif-min*60-hr*3600;
  printf("%sTime taken to complete : %s%d hour %d min %d sec%s\n",blue,red,hr,min,sec,normal);
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
