/**************************************************************************/
/*                              Nuruzzaman                                */
/*                              07/22/2011                                */
/*                                                                        */
/*                        Last Edited:22/01/2012                          */
/*                                                                        */
/* This Macro create plots for beamline optics using beam modulation.     */
/* Please see the USER MANUAL at the bottom of this macro for its usage.  */
/**************************************************************************/
#include <NurClass.h>
void bmod_bpms2(UInt_t run_number=0)
{
  time_t tStart = time(0), tEnd; 
  char textfile[255],textfileY[255],textfit[255],textfitY[255],run[255], run0[255],text_check[255];

  //   /* Ask to input the run condition to user */
  //   printf("%s%sEnter Run Number%s\n%s",SolidLine.Data(),blue,normal,SolidLine.Data()); 
  //   cin >> run_number;

  /* Directory */
  TString rootDirectory = "$QW_ROOTFILES/";
  //   TString rootDirectory = "/w/cache/mss/hallc/qweak/rootfiles/pass3/";
  TString imageDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/images";
  TString textDirectory = "/u/group/hallc/www/hallcweb/html/qweak/bmod/text";
  //   TString imageDirectory = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots/pass3";
  //   TString textDirectory = "/w/hallc/qweak/nur/data_analysis/beamModulation/plots/beamModulation_plots/pass3/bpm_response";

  sprintf(run0,"%sQwPass1_%d.000.trees.root",rootDirectory.Data(),run_number);
  TFile f(run0);

  if(!f.IsOpen()) {
    printf("%sUnable to find Rootfile for Run # %s%d%s.\nPlease insert a correct Run #. Exiting the program!%s\n",
	   blue,red,run_number,blue,normal); 
    exit(1);
  }

  //   TChain * chain = new TChain("Mps_Tree");
  //   sprintf(run,"%sQwPass3_%d.*.root",rootDirectory.Data(),run_number);
  //   TFile f1(run);
  //   chain->Add(run);

  TChain * chain = new TChain("Mps_Tree");
  for(Int_t t=0; t<=1; t++) 
    {
      sprintf(run,"%sQwPass1_%d.00%d.trees.root",rootDirectory.Data(),run_number,t);
      TFile f1(run);
      chain->Add(run);
      // 	printf("%sRootfile %s has been found%s\n",blue,run,normal);
    }


  /* All the devices used for Beam Modulation */
  const Int_t NUM5 = 5, NUM10 = 10, NUM23 = 23;
  TString ptt[NUM5] = {"bm_pattern_number==11","bm_pattern_number==12",
		       "bm_pattern_number==13","bm_pattern_number==14",
		       "bm_pattern_number==15"};
  TString BModType[NUM5] = {"X","Y","E","Xp","Yp"};
  TString xy[NUM5] = {"X","Y","X","X","Y"};
  TString deviceBpms[NUM23] = {"target","bpm3h09","bpm3h08",
			       "bpm3h07c","bpm3h07b","bpm3h07a","bpm3h04",
			       "bpm3h02","bpm3c21","bpm3c20","bpm3p03a",
			       "bpm3p02b","bpm3p02a","bpm3c19","bpm3c18",
			       "bpm3c17","bpm3c16","bpm3c14","bpm3c12",
			       "bpm3c11","bpm3c08","bpm3c07a","bpm3c07"};
  TString deviceRAMP = "ramp*76.29e-6*1140.5";
  TString txaxis = "BMod Phase [degree]";
  //  TString func = "[0] + [1]*sin(0.0175*x+[2])";

  /* Standard cuts for Beam Modulation */
  TString RampBaselineCut = "ramp>0";
  TString RampDeviceErrorCut = "(ramp.Device_Error_Code&0x80)==0x80";
  TString RampEdgeCut = "abs((ramp.block0+ramp.block3)-(ramp.block1+ramp.block2))<50";
  TString BeamTripCut = "qwk_bcm1.Device_Error_Code==0";
  TString StandardCut = Form("%s && %s && %s && %s",
			     RampBaselineCut.Data(),
			     RampDeviceErrorCut.Data(),
			     RampEdgeCut.Data(),
			     BeamTripCut.Data());

  /* Deactivating unnecessary braches and activating reqired branches */  
  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("ramp",1);
  chain->SetBranchStatus("event_number",1);
  chain->SetBranchStatus("bm_pattern_number",1);
  chain->SetBranchStatus("qwk_bcm1",1);

  for(Int_t bpm_NUM=0; bpm_NUM<NUM23; bpm_NUM++) 
    {
      chain->SetBranchStatus(Form("qwk_%sX",deviceBpms[bpm_NUM].Data()),1);
      chain->SetBranchStatus(Form("qwk_%sY",deviceBpms[bpm_NUM].Data()),1);
    }


  char createDirectory[255],createDirectory2[255],saveplotBpmVsRamp[255],title1[255];
  char plotBpmVsRamp[255],cutBpmVsRamp[255],yaxis1[255],histoBpmVsRamp[255],padtitle1[255];

  char saveplotBpmYVsRamp[255],title2[255];
  char plotBpmYVsRamp[255],cutBpmYVsRamp[255],yaxis2[255],histoBpmYVsRamp[255],padtitle2[255];
  Double_t amp[NUM23],amp_e[NUM23],phase[NUM23],phase_e[NUM23],angle[NUM23],xact[NUM23],modamp[NUM23];
  Double_t ampY[NUM23],ampY_e[NUM23],phaseY[NUM23],phaseY_e[NUM23],angleY[NUM23],xactY[NUM23],modampY[NUM23]; 


  Float_t signp,signm;
  signp = 1;signm = -1;
  Double_t csizx,csizy,csiz3x,csiz3y,cx1,cy1,cx2,cx3,tsiz,tsiz3,tll,tlr,ps1,ps2,ps3,ps4,ypmax,ypmin,ypmaxY,ypminY;
  csizx=1600;csizy=1300;csiz3x=1100;csiz3y=780;tsiz=0.40;tsiz3=0.45;tll=0.012;tlr=0.4;cx1=0;cy1=0;cx2=300;cx3=600;
  ps1=0.01;ps2=0.93;ps3=0.94;ps4=0.99;

  Double_t ymax,ymin,qline,bpmline,qtext,bpmtext,qhtext,bpmhtext;
  //   ymax=0.2;ymin=-0.2;qline=-0.185;bpmline=0.187;qtext=-0.170;bpmtext=0.170;qhtext=-0.168;bpmhtext=0.172;
  //  ymax=0.25;ymin=-0.25;qline=-0.2345;bpmline=0.2347;qtext=-0.23415;bpmtext=0.225;qhtext=-0.23416;bpmhtext=0.225;
  ymax=0.6;ymin=-0.6;qline=-0.555;bpmline=0.57;qtext=-0.545;bpmtext=0.558;qhtext=-0.545;bpmhtext=0.548;
  //    ymax=0.5;ymin=-0.5;qline=-0.455;bpmline=0.47;qtext=-0.445;bpmtext=0.458;qhtext=-0.445;bpmhtext=0.448;
  //   ymax=0.8;ymin=-0.8;qline=-0.755;bpmline=0.77;qtext=-0.745;bpmtext=0.758;qhtext=-0.745;bpmhtext=0.748;


  /* Create a directory by run number under plots directory */
  FILE *check_dir_create;
  sprintf(createDirectory,"%s/%d",imageDirectory.Data(), run_number);
  sprintf(createDirectory2,"%s",textDirectory.Data());
  check_dir_create = fopen(createDirectory,"r");

  if (check_dir_create == NULL) {
    printf("%sNo Directory %s%s exist.\n\n%s",
	   green,red,createDirectory,normal);
    gSystem->mkdir(createDirectory);
    gSystem->mkdir(createDirectory2);
    gSystem->Chmod(createDirectory, 509);  /* 509 decimal == 775 octal, giving user permission to overwrite */
    gSystem->Chmod(createDirectory2, 509);
    printf("%sDirectory%s %s\n%shas been created sucessfully%s\n",
	   blue,red,createDirectory,blue,normal);
  }
  else{
    printf("%sDirectory %s%s already exist. Skiping...%s\n\n",
	   green,red,createDirectory,normal);
  }

  TProfile* bpm = new TProfile("bpm", "bpm", 300, 0., 360., -10., 10.);
  TProfile* bpmY = new TProfile("bpmY", "bpmY", 300, 0., 360., -10., 10.);

  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );


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
  gStyle->SetTitleFontSize(0.09);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleStyle(0); 
  // 	gStyle->SetTitleColor(-1);
  // 	gStyle->SetFillStyle(4000); 
  gStyle->SetTitleSize(0.04);
  gStyle->SetTitleYSize(0.04);
  gStyle->SetTitleXSize(0.04);
  // 	gStyle->SetTitleOffset(1.40);
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
  gStyle->SetStatFontSize(0.10);
  gStyle->SetTextFont(42);
  gStyle->SetStatFont(42);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetLabelFont(42);
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"x");
  gStyle->SetStatColor(0); 
  gROOT->ForceStyle();


  for ( Int_t pattern = 0; pattern <5; pattern++ ){

    UInt_t col=0,cold=0; Double_t yoffset =0;
    switch(pattern)
      { 
      case 0: col = kRed-4;  cold = kRed-2;    yoffset = 0.20;  break;
      case 1: col = kGreen;  cold = kGreen+2;  yoffset = 0.30;  break;
      case 2: col = kBlue-7; cold = kBlue;     yoffset = 0.35;  break;
      case 3: col = kRed-4;  cold = kRed-2;    yoffset = 0.15;  break;
      case 4: col = kGreen;  cold = kGreen+2;  yoffset = 0.30;  break;
      case 5: col = kRed-4;  cold = kRed-2;    yoffset = 0.10;  break;
      case 6: col = kGreen;  cold = kGreen+2;  yoffset = 0.05;  break;
      case 7: col = kBlue-7; cold = kBlue;     yoffset = 0.10;  break;
      case 8: col = kRed-4;  cold = kRed-2;    yoffset = 0.10;  break;
      case 9: col = kGreen;  cold = kGreen+2;  yoffset = 0.05;  break;
	//     default: Console.WriteLine("%sPlease Insert 0,1,2,3 or 4%s",blue,normal); break;
      };


    sprintf(saveplotBpmVsRamp,"%s/%d/%dbpmsX%s.png",imageDirectory.Data(),run_number,run_number,BModType[pattern].Data());
    sprintf(saveplotBpmYVsRamp,"%s/%d/%dbpmsY%s.png",imageDirectory.Data(),run_number,run_number,BModType[pattern].Data());
    sprintf(title1,"Run %d: Hall-C BPM X Response to %s-Modulation (%s)",run_number,BModType[pattern].Data(),asctime(timeinfo));
    sprintf(title2,"Run %d: Hall-C BPM Y Response to %s-Modulation (%s)",run_number,BModType[pattern].Data(),asctime(timeinfo));

    /* Create a text file for saving fit parameters */
    sprintf(textfile,"%s/%dbpmfitX%s.txt",textDirectory.Data(),run_number,BModType[pattern].Data());
    ofstream outfile(textfile);
    sprintf(textfileY,"%s/%dbpmfitY%s.txt",textDirectory.Data(),run_number,BModType[pattern].Data());
    ofstream outfileY(textfileY);

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

    TCanvas *c2 = new TCanvas("c2",title2,cx1,cy1,csizx,csizy);
    TPad * padc20 = new TPad("padc20","padc20",ps1,ps2,ps4,ps4);
    TPad * padc21 = new TPad("padc21","padc21",ps1,ps1,ps4,ps3);
    padc20->Draw();
    padc21->Draw();
    padc20->cd();
    TText * ct20 = new TText(tll,tlr,title2);
    ct20->SetTextSize(tsiz);
    ct20->Draw();
    padc21->cd();
    padc21->Divide(4,6);


    printf("%sRun :%d, %s-Modulation\nBPMs\t\tAmplitude\t\tPhase%s\n",red,run_number,BModType[pattern].Data(),normal);

    for(Int_t i=0; i< NUM23; i++) 
      {

	Int_t idx = i+1;

	padc11->cd(idx);
	padc11->cd(idx)->SetGrid();
	sprintf(plotBpmVsRamp,"qwk_%s%s:ramp*76.29e-6*1140.5>>%s",deviceBpms[i].Data(),xy[pattern].Data(),deviceBpms[i].Data());
	sprintf(histoBpmVsRamp,"%s",deviceBpms[i].Data());
	sprintf(cutBpmVsRamp,"qwk_%s%s.Device_Error_Code==0 && %s && %s",deviceBpms[i].Data(),xy[pattern].Data(),StandardCut.Data(),ptt[pattern].Data());
	sprintf(yaxis1,"%s-%s [mm]",deviceBpms[i].Data(),xy[pattern].Data());
	sprintf(padtitle1,"%s-%s vs Ramp",deviceBpms[i].Data(),xy[pattern].Data());


	chain->Draw(plotBpmVsRamp,cutBpmVsRamp,"prof");
	TProfile * bpm = (TProfile*)gDirectory->Get(histoBpmVsRamp);
	Long64_t hnx = bpm->GetEntries();
	if(!hnx){
	  printf("%s%d: %s%s-Modulation: %s%sX%s -- No modulation data%s\n",
		 red,run_number,BModType[pattern].Data(),blue,red,deviceBpms[i].Data(),blue,normal);
	  exit(1);
	  // 	  continue;
	}

	else{

	  Double_t limit = bpm->GetMean(2);
	  ypmax= limit+yoffset; ypmin= limit-yoffset;

	  bpm->Clone();
	  bpm->SetLineColor(col);
	  bpm->SetMarkerColor(col);
	  bpm->Draw("goff");
	  bpm->GetYaxis()->SetTitle(yaxis1);
	  bpm->GetXaxis()->SetTitle(Form("%s",txaxis.Data()));
	  bpm->SetTitle(padtitle1);
	  bpm->GetYaxis()->SetRangeUser(ypmin,ypmax);

	  // 	  TF1* func = new TF1("func","[0] + [1]*sin(((pi/180)*x)+[2])",10,350);
	  // 	  TF1* func = new TF1("func","[0] + [1]*sin((pi/180)*([3]*x-[2]))",10,350);
	  TF1* func = new TF1("func","[0] - [1]*cos((pi/180)*(x-[2]))",10,350);
	  // TF1* func = new TF1("func","[0] + [1]*(sin((pi/180)*x)*cos([2])+cos((pi/180)*x)*sin([2])) ",10,350);

	  Double_t meanval = bpm->GetMean(2);
	  Int_t minbin = bpm->GetMinimumBin();
	  Int_t maxbin = bpm->GetMaximumBin();
	  Double_t minpos = bpm->GetXaxis()->GetBinCenter(minbin);
	  Double_t maxpos = bpm->GetXaxis()->GetBinCenter(maxbin);
	  Double_t minbpmval = bpm->GetBinContent(minbin);
	  Double_t maxbpmval = bpm->GetBinContent(maxbin);
	  Double_t bpmamp = meanval-minbpmval;


	  func->SetParameters(meanval,bpmamp,minpos);
	  bpm->Fit("func","E M R F 0 Q");
	  bpm->DrawCopy("9");
	  func->SetLineColor(cold);
	  func->SetLineWidth(2);
	  func->Draw("same");

	  amp[i] = func->GetParameter(1);
	  amp_e[i] = func->GetParError(1);
	  phase[i] = func->GetParameter(2);
	  phase_e[i] = func->GetParError(2);
	  // 	  printf("%s%7.4f%s\n",green,phase[i],normal);
	  angle[i] = func->Derivative(phaseY[i] = 180);



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


	  if(!hnx){
	    printf("%s%d: %s%s-Modulation: %s%sX%s -- No modulation data%s\n",
		   red,run_number,BModType[pattern].Data(),blue,red,deviceBpms[i].Data(),blue,normal);
	    exit(1);
	    // 	    continue;
	  }

	  else{
	    sprintf(textfit,"%7.4f\t%7.4f\t%7.2f\t%7.2f",xact[i],amp_e[i],phase[i],phase_e[i]);
	    outfile <<textfit<<endl;
	    printf("%s%s-X%s\t%7.4f+-%7.4f\t%7.2f+-%7.2f\n",blue,deviceBpms[i].Data(),normal,xact[i],amp_e[i],phase[i],phase_e[i]);

	  }

	  gPad->Update();
	  bpm->Delete();

 	}



	padc21->cd(idx);
	padc21->cd(idx)->SetGrid();
	sprintf(plotBpmYVsRamp,"qwk_%sY:ramp*76.29e-6*1140.5>>%sY",deviceBpms[i].Data(),deviceBpms[i].Data());
	sprintf(histoBpmYVsRamp,"%sY",deviceBpms[i].Data());
	sprintf(cutBpmYVsRamp,"qwk_%sY.Device_Error_Code==0 && %s && %s",deviceBpms[i].Data(),StandardCut.Data(),ptt[pattern].Data());
	sprintf(yaxis2,"%s-Y [mm]",deviceBpms[i].Data());
	sprintf(padtitle2,"%s-Y vs Ramp",deviceBpms[i].Data());


	chain->Draw(plotBpmYVsRamp,cutBpmYVsRamp,"prof");
	TProfile * bpmY = (TProfile*)gDirectory->Get(histoBpmYVsRamp);
	Long64_t hny = bpmY->GetEntries();
	if(!hny){
	  printf("%s%d: %s%s-Modulation: %s%s%s -- No modulation data%s\n",
		 red,run_number,BModType[pattern].Data(),blue,red,deviceBpms[i].Data(),blue,normal);
	  exit(1);
	  // 	  continue;
	}

	else{

	  Double_t limitY = bpmY->GetMean(2);
	  ypmaxY= limitY+yoffset; ypminY= limitY-yoffset;

	  bpmY->Clone();
	  bpmY->SetLineColor(col);
	  bpmY->SetMarkerColor(col);
	  bpmY->Draw("goff");
	  bpmY->GetYaxis()->SetTitle(yaxis2);
	  bpmY->GetXaxis()->SetTitle(Form("%s",txaxis.Data()));
	  bpmY->SetTitle(padtitle2);
	  bpmY->GetYaxis()->SetRangeUser(ypminY,ypmaxY);


	  TF1* funcY = new TF1("funcY","[0] - [1]*cos((pi/180)*(x-[2]))",10,350);

	  Double_t meanvalY = bpmY->GetMean(2);
	  Int_t minbinY = bpmY->GetMinimumBin();
	  Int_t maxbinY = bpmY->GetMaximumBin();
	  Double_t minposY = bpmY->GetXaxis()->GetBinCenter(minbinY);
	  Double_t maxposY = bpmY->GetXaxis()->GetBinCenter(maxbinY);
	  Double_t minbpmvalY = bpmY->GetBinContent(minbinY);
	  Double_t maxbpmvalY = bpmY->GetBinContent(maxbinY);
	  Double_t bpmampY = meanvalY-minbpmvalY;

	  // 	  printf("%s%s-Y %i  %f  %f  %i  %f  %f%s\n",red,deviceBpms[i].Data(),minbinY,minposY,minbpmvalY,maxbinY,maxposY,maxbpmvalY,normal);

	  funcY->SetParameters(meanvalY,bpmampY,minposY);
	  // 	  func->SetParLimits(2,-180,180);

	  // 	  func->SetParLimits(2,0,92);
	  //func->SetParLimits(2,90,180);
	  bpmY->Fit("funcY","E M R F 0 Q");
	  bpmY->DrawCopy("9");
	  funcY->SetLineColor(cold);
	  funcY->SetLineWidth(2);
	  funcY->Draw("same");

	  ampY[i] = funcY->GetParameter(1);
	  ampY_e[i] = funcY->GetParError(1);
	  phaseY[i] = funcY->GetParameter(2);
	  phaseY_e[i] = funcY->GetParError(2);
	  // 	  printf("%s%7.4f%s\n",green,phaseY[i],normal);
	  angleY[i] = funcY->Derivative(phase[i] = 180);


	  if(pattern==3){
	    if(i<4){
	      if (ampY[i]<0) xactY[i]= signm*ampY[i];
	      if (ampY[i]>0) xactY[i]= signp*ampY[i];}
	    else{	
	      if (ampY[i]<0) modampY[i]= signm*ampY[i];
	      if (ampY[i]>0) modampY[i]= signp*ampY[i];

	      if (angleY[i]<0) xactY[i]= signm*modampY[i];
	      if (angleY[i]>0) xactY[i]= signp*modampY[i];}
	  }

	  else{
	
	    if (ampY[i]<0) modampY[i]= signm*ampY[i];
	    if (ampY[i]>0) modampY[i]= signp*ampY[i];

	    if (angleY[i]<0) xactY[i]= signm*modampY[i];
	    if (angleY[i]>0) xactY[i]= signp*modampY[i];
	  }

	  if(!hny){
	    printf("%s%d: %s%s-Modulation: %s%sY%s -- No modulation data%s\n",
		   red,run_number,BModType[pattern].Data(),blue,red,deviceBpms[i].Data(),blue,normal);
	    exit(1);
	    // 	    continue;
	  }

	  else{
	    sprintf(textfitY,"%7.4f\t%7.4f\t%7.2f\t%7.2f",xactY[i],ampY_e[i],phaseY[i],phaseY_e[i]);
	    outfileY <<textfitY<<endl;
	    printf("%s%s-Y%s\t%7.4f+-%7.4f\t%7.2f+-%7.2f\n",blue,deviceBpms[i].Data(),normal,xactY[i],ampY_e[i],phaseY[i],phaseY_e[i]);
	  }

	  gPad->Update();
	  bpmY->Delete();
	}

      }

    //   sprintf(text_check,"%s/%dbpmfit%s%s.txt",textDirectory.Data(),run_number,xy[pattern].Data(),BModType[pattern].Data());
    //   FILE *check;
    //   check = fopen(text_check,"r");
    //   if (check == NULL) {printf("%sNo Modulation data for this run !!!%s\n",red,normal);
    //     gSystem->Exec(Form("rm -rf %s",textfile));continue;}
    //   else{
    c1->Update();
    c1->SaveAs(saveplotBpmVsRamp);
    c2->Update();
    c2->SaveAs(saveplotBpmYVsRamp);
    //     //     delete bpm;
    //   }

    c1->Clear();
    c2->Clear();
    delete c1;
    delete c2;

  }

  char textfile_dir_check[255];
  sprintf(textfile_dir_check,"%s/%dbpmfitXX.txt",textDirectory.Data(),run_number);
  FILE *check_dir;
  check_dir = fopen(textfile_dir_check,"r");

  if (check_dir == NULL) {
    printf("%sNo Modulation data for this run !!!. Exiting Program\n%s",red,contact.Data());

    /* Remove directory by run number if data does not exist */
    gSystem->Exec(Form("rm -rf %s",createDirectory));
    printf("%s\nDirectory\n%s%s\n%shas been removed%s\n",blue,red,createDirectory,blue,normal);
    exit(1);
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
  printf ( "%sThe plot created at time : %s%s%s\n",blue,red,asctime(timeinfo),normal);
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
