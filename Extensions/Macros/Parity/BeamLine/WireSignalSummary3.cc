//********************************************************************//
// Author : J. Kaisen
//          J. H. Lee
// Date   : Tuesday, August  3 04:38:48 EDT 2010
//********************************************************************//
//
// This macros plots the pattern based aasymmetries of the bpm signals defined in the 
// devicelist below and the canvas are printed on to .gif files. 
// To do this, the bpms needs to be analysed as bcm.
// To use this macro, 
// open root and load the macro by typing .L WireSignalPlots.cc and type in the command.
//
//  PlotAsyms(run_num)
//  run_num -: run number
//  
//
// e.g.
// root[0] .L WireSignalSummary3.cc
// root[1]  Plot(1160,"qweak_hallc_beamline.map","Mps_Tree","hw_sum")
// root[2]  Plot(1160,"qweak_hallc_beamline.map","Hel_Tree","hw_sum")

// Plot(run number, map file, tree (Hel_Tree or Mps_Tree), histogram to look at)




//********************************************
// Main function
//********************************************

//!!!!For the main detector map this may duplicate functions that have more than one detector in the name.!!!!
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

gROOT->Reset();

void PlotRAW(Int_t runnum, TString mapstring, TString treename);
void Plot(Int_t runnum, TString mapstring, TString treename,TString graphinfo);


void Plot(Int_t runnum, TString mapstring, TString treename, TString graphinfo)
{
  ifstream mapfile; 
  ifstream branchlistinfile;
  std::ofstream branchlistoutfile;
  std::vector<TString>  devicelist1;
  std::vector<TString>  bpmlist;
  std::vector<TString>  devicelist2;
  std::vector<TString>  devicelist3;
  if(graphinfo == NULL) graphinfo = "hw_sum";
  Int_t counter=0,counter1=0,counter2=0,counter3=0;
  TString devicename,throw_away,test,line,det_type,get,textfile;

  TFile *file =  new TFile(Form("$QW_ROOTFILES/Qweak_%d.000.root",runnum));

  textfile = ("Hel_Branches_list_run_%i.in",runnum);
  get =  gSystem->Getenv("QWANALYSIS");

  if(get.IsNull()) {
    std::cout << "Please check your QWANALYSIS path " 
	      << std::endl;
    return;
  }

  mapstring = get +"/Parity/prminput/" + mapstring;
  mapfile.open(mapstring);
  if(!mapfile.is_open()) {
    std::cout << "Please check your mapfile " 
	      << mapstring 
	      << std::endl;
    return;
  }


  while (!mapfile.eof()) {
    line.ReadToken(mapfile);
    if (line.Contains("!"))  line.ReadToDelim(mapfile);
    else if (line.Contains("VQWK")) {
      for(Int_t i=0;i<2;i++)  	line.ReadToken(mapfile);
      line.ReadToken(mapfile);
      det_type=line;
      det_type.ToLower();
      line.ReadToken(mapfile);
      devicename=line;
      throw_away=devicename(devicename.Sizeof()-2,1);
      if(throw_away==",") devicename = devicename(0,devicename.Sizeof()-2);
      if (det_type=="bpmstripline,"){
	TString subname=devicename(devicename.Sizeof()-3,2);
	devicename.ToLower();
	devicename= devicename(0,devicename.Sizeof()-3);
	
      	if (treename == "Hel_Tree") {
	  Bool_t flag=kFALSE;
	  for(size_t ii=0;ii<bpmlist.size();ii++)
	    if(devicename==bpmlist[ii]) flag=kTRUE;
	  if(!flag)
	    bpmlist.push_back(devicename);
	  //	  else
	    //	    std::cout<<"I already found this bpm"<<std::endl;
	}
	else {
	  devicename= devicename + subname;
	  devicelist1.push_back(devicename);
	  counter++;
	}
      }
      else if(det_type=="bpmcavity,"){
	TString subname=devicename(devicename.Sizeof()-2,1);
	devicename.ToLower();
	if(treename == "Hel_Tree") {
	  devicename=(devicename.Sizeof()-2);
	  Bool_t flag=kFALSE;
	  for(size_t ii=0;ii<bpmlist.size();ii++)
	    if(devicename==bpmlist[ii])
	      flag=kTRUE;
	  if(!flag)
	    bpmlist.push_back(devicename);
	  else
	    std::cout<<"I already found this bpm"<<std::endl;
	}
	else{
	  devicename=(devicename.Sizeof()-2) + subname;
	  devicelist1.push_back(devicename);
	  counter++;
	}
      }
      else if (det_type=="bcm,"){
	devicename.ToLower();
	if(treename=="Hel_Tree")
	  bpmlist.push_back(devicename);
	else {
	  devicelist1.push_back(devicename);
	  counter++;
	}
      }
      else if (det_type=="integrationpmt,"){
	devicename.ToLower();
	if(treename=="Hel_Tree")
	  bpmlist.push_back(devicename);
	else {
	devicelist1.push_back(devicename);
	counter++;
	}
      }
      else
	std::cout<< " I don't know this device type" << std::endl;
    }
  }
  mapfile.close();


 
 //This command creates an .in file with all the branches in Hel_Tree listed
  if(treename=="Hel_Tree"){
    branchlistoutfile.open(textfile);
    branchlistoutfile <<"# Tree trim file of run  "<<runnum<<std::endl;
    branchlistoutfile <<"# Hel Tree  "<<std::endl;
    if (file->IsZombie()) {
      printf("Error opening file\n"); 
      return;
    }
    else{    
      TTree * heltree = (TTree *) file->Get("Hel_Tree");
      TObjArray * branch_list;
      branch_list=heltree->GetListOfBranches();
      TObject * branch;
      for(Int_t m = 0;m<branch_list->GetEntries();m++ ){
	branch=branch_list->At(m);
	branchlistoutfile <<branch->GetName() <<std::endl;
      }
    }
    branchlistoutfile.close();
    //This command reads the .in file and gets the particular branches
    std::size_t ii = 0;
    for(ii=0;ii<bpmlist.size();ii++){
      branchlistinfile.open(textfile);
      while (!branchlistinfile.eof()) {
	line.ReadToken(branchlistinfile);
	if (line.Contains("#"))  line.ReadToDelim(branchlistinfile);
	else if (line.Contains(bpmlist[ii])) {
	  if(line.Contains("asym")) {
	    devicename=line;
	    devicelist1.push_back(devicename);
	    counter1++; 
	  }
	  else if(line.Contains("diff")) {
	    devicename=line;
	    devicelist2.push_back(devicename);
	    counter2++;
	  }
	  else if(line.Contains("yield")) {
	    devicename=line;
	    devicelist3.push_back(devicename);
	    counter3++;
	  }
	  else
	    std::cout<<"I don't know this histogram type" << std::endl;
	}
      }
      branchlistinfile.close();
    }
  }
	  
      

  Bool_t canvas_style = false;
  
  if(canvas_style) {
    gStyle->Reset();
    gStyle->SetOptTitle(1);
    gStyle->SetStatColor(10);  
    gStyle->SetStatH(0.4);
    gStyle->SetStatW(0.3);     
    gStyle->SetOptStat(1110); 
    gStyle->SetOptStat(1); 
    // canvas parameters
    gStyle->SetFrameBorderMode(0);
    gStyle->SetFrameBorderSize(0);
    gStyle->SetFrameFillColor(10);
    
    // pads parameters
    gStyle->SetPadColor(0); 
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadBorderSize(0);
    gStyle->SetPadTopMargin(0.1);
    gStyle->SetPadBottomMargin(0.1);
    gStyle->SetPadLeftMargin(0.1);  
    gStyle->SetPadRightMargin(0.08);  
    gStyle->SetLabelSize(0.07,"x");
    gStyle->SetLabelSize(0.07,"y");
    gStyle->SetPadGridX(kTRUE);
    gStyle->SetPadGridY(kTRUE);
  }
  

  Int_t mps_device_num = counter;
  Int_t hel_device_num[3] = {counter1, counter2, counter3};


    
  if(treename == "Hel_Tree") {
    std::cout << "device list1 asym size " 
	      << devicelist1.size() 
	      << " hel_device_num[0] "
	      << hel_device_num[0]
	      << std::endl;
    std::cout << "device list2 diff size " 
	      << devicelist2.size() 
	      << " hel_device_num[1] "
	      << hel_device_num[1]
	      << std::endl;
    std::cout << "device list3 yield size " 
	      << devicelist3.size() 
	      << " hel_device_num[2] "
	      << hel_device_num[2]
	      << std::endl;
  }
  else if (treename == "Mps_Tree") {
    std::cout << "MPS device list size "
	      << devicelist1.size() 
	      << " mps_device_num "
	      << mps_device_num
	      << std::endl;
  }
  else return;


  Int_t plots_num = 0;

  TString mps_canvas_title;
  TString hel_canvas_title[3];

  Double_t w = 1000;
  Double_t h = 800;

  TPaveText *mps_title;
  TPaveText *hel_title[3];
  TString    hel_type[3] = {"asym", "diff", "yield"};

  TPad      *mps_pad;
  TPad      *hel_pad[3];

  TH1D *mps_h1D;    // reuse because of its locality
  TH1D *hel_h1D[3]; // reuse because of its locality




  Double_t *mps_xaxis;
  Double_t *mps_xaxisrms;
  Double_t *mps_xaxisevents; 

  Double_t *hel_xaxis[3];
  Double_t *hel_xaxisrms[3];
  Double_t *hel_xaxisevents[3];

  Double_t xmin=999e+10;
  Double_t xmax=-xmin;
  Double_t xmaxrms=-999e+10;
  Double_t xminrms=99e+10;
  Double_t xmaxevent=-999e+10;
  Double_t xminevent=99e+10;
  

  Int_t i, j, k;
  i = j = k = 0;
  Bool_t output_ps = false;
 
  TTree * nt = file->Get(treename); //load the Tree
  // here only "Mps_Tree" or "Hel_Tree", thus we don't check others,
  // because of the above "std::cout"s codes.

  if(treename == "Mps_Tree") {

   
    plots_num = 1;
    mps_canvas_title = "MPS signal plots";
    TCanvas *mps_canvas;
    mps_canvas = new TCanvas("mps_signal_plots", mps_canvas_title, w, h);
    mps_canvas ->SetWindowSize(w + (w - mps_canvas ->GetWw()), h + (h - mps_canvas->GetWh()));
    mps_canvas -> cd();
    mps_title = new TPaveText(0.01,0.953,0.5,0.999);
    mps_title -> SetFillColor(11);
    mps_title -> AddText(mps_canvas_title);
    mps_title -> Draw();
    mps_pad = new TPad("mpspad","mps signals pad",0.00,0.00,1.00,0.95);
    mps_pad -> Draw();
    mps_pad -> cd();
    mps_pad -> Divide(4,4);

    mps_xaxis = new Double_t [mps_device_num];
    mps_xaxisrms =  new Double_t [mps_device_num];
    mps_xaxisevents = new Double_t [mps_device_num];
    
    if(output_ps) mps_canvas -> Print(Form("run_%i_mps_signal.ps[",runnum));


    j = 0;
    for(i=0; i<mps_device_num; i++){
      mps_pad -> cd(j+1);
      
      nt->Draw(Form("%s.%s/%s.num_samples",
		    devicelist1[i].Data(),
		    graphinfo.Data(),
		    devicelist1[i].Data()),
	       Form("%s.num_samples>0",devicelist1[i].Data())
	       );
      mps_h1D = (TH1D*) gPad -> GetPrimitive("htemp");
      mps_h1D -> SetName(Form("%s.%s/%s.num_samples",
			      devicelist1[i].Data(),
			      graphinfo.Data(),
			      devicelist1[i].Data()));
      mps_xaxis[i]       = mps_h1D -> GetMean();
      mps_xaxisrms[i]    = mps_h1D -> GetRMS();
      mps_xaxisevents[i] = mps_h1D -> GetEntries();
   

      if(mps_xaxis[i]>xmax)         	xmax      = mps_xaxis[i];
      if(mps_xaxis[i]<xmin)       	xmin      = mps_xaxis[i];
      if(mps_xaxisrms[i]>xmaxrms)     	xmaxrms   = mps_xaxisrms[i];
      if(mps_xaxisrms[i]<xminrms)      	xminrms   = mps_xaxisrms[i];
      if(mps_xaxisevents[i]>xmaxevent) 	xmaxevent = mps_xaxisevents[i];
      if(mps_xaxisevents[i]<xminevent) 	xminevent = mps_xaxisevents[i];

      j++;  
      if(j==16){
      	j=0;
	if(output_ps) mps_canvas -> Print(Form("run_%i_mps_signal.ps",runnum));
      }
    }
    if(output_ps) mps_canvas -> Print(Form("run_%i_mps_signal.ps",runnum));
    if(output_ps) mps_canvas -> Print(Form("run_%i_mps_signal.ps]",runnum));
    //  mps_canvas -> Modified();
  }
  else {  //if (treename == "Hel_Tree") {
    plots_num = 3;
    TCanvas *hel_canvas[3];
    hel_canvas_title[0] = "HEL signal plots for asym";
    hel_canvas_title[1] = "HEL signal plots for diff";
    hel_canvas_title[2] = "HEL signal plots for yield";
    for(i = 0; i <plots_num; i++) {
      hel_canvas[i] = new TCanvas(Form("hel_signal_plots_%d",i), hel_canvas_title[i],  w, h);
      hel_canvas[i] ->SetWindowSize(w + (w - hel_canvas[i] ->GetWw()), h + (h - hel_canvas[i]->GetWh()));
    
      hel_canvas[i] -> cd();
      hel_title[i] = new TPaveText(0.01,0.953,0.5,0.999);
      hel_title[i] -> SetFillColor(11);
      hel_title[i] -> AddText(hel_canvas_title[i]);
      hel_title[i] -> Draw();
      
      hel_pad[i] = new TPad(Form("helpad%d",i),"hel signals pad",0.00,0.00,1.00,0.95);
      hel_pad[i] -> Draw();
      hel_pad[i] -> cd();
      hel_pad[i] -> Divide(4,4);
      hel_xaxis[i]       = new Double_t [hel_device_num[i]];
      hel_xaxisrms[i]    = new Double_t [hel_device_num[i]];
      hel_xaxisevents[i] = new Double_t [hel_device_num[i]];
  
      j = 0;   


      if(output_ps) hel_canvas[i]->Print(Form("run_%d_hel_signal_%s.ps[", runnum, hel_type[i].Data()));
      for(k=0; k<hel_device_num[i]; k++){
	hel_pad[i] -> cd(j+1);
	if(i == 0 ) {
	  gPad -> SetLogy();
	  nt->Draw(Form("%s.%s/%s.num_samples",
			devicelist1[i].Data(),
			graphinfo.Data(),
			devicelist1[i].Data()), ""
		   );
	}
	else if ( i == 1 ) {
	  gPad -> SetLogy();
	  nt->Draw(Form("%s.%s/%s.num_samples",
			devicelist2[i].Data(),
			graphinfo.Data(),
			devicelist2[i].Data()), ""
		   );
	}
	else if ( i == 2 ) {
	  nt->Draw(Form("%s.%s/%s.num_samples",
			devicelist3[i].Data(),
			graphinfo.Data(),
			devicelist3[i].Data()), ""
		   );
	}
       	hel_h1D[i] = (TH1D*) gPad -> GetPrimitive("htemp");
	hel_h1D[i] -> SetName(
			      Form("Norm %s.%s",
				   devicelist1[i].Data(),
				   graphinfo.Data())
			      );
	hel_xaxis[i][k]       = hel_h1D[i] -> GetMean();
	hel_xaxisrms[i][k]    = hel_h1D[i] -> GetRMS();
	hel_xaxisevents[i][k] = hel_h1D[i] -> GetEntries();

	if(hel_xaxis[i][k]>xmax)         	xmax      = hel_xaxis[i][k];
	if(hel_xaxis[i][k]<xmin)        	xmin      = hel_xaxis[i][k];
	if(hel_xaxisrms[i][k]>xmaxrms)     	xmaxrms   = hel_xaxisrms[i][k];
	if(hel_xaxisrms[i][k]<xminrms)      	xminrms   = hel_xaxisrms[i][k];
	if(hel_xaxisevents[i][k]>xmaxevent) 	xmaxevent = hel_xaxisevents[i][k];
	if(hel_xaxisevents[i][k]<xminevent) 	xminevent = hel_xaxisevents[i][k];
	j++;  
	if(j==16){
	  j=0;
	  if(output_ps) hel_canvas[i]->Print(Form("run_%d_hel_signal_%s.ps", runnum, hel_type[i].Data()));
	}
      }
      if(output_ps) hel_canvas[i]->Print(Form("run_%d_hel_signal_%s.ps", runnum, hel_type[i].Data()));
      if(output_ps) hel_canvas[i]->Print(Form("run_%d_hel_signal_%s.ps]", runnum, hel_type[i].Data()));
    }
  }
  //  else 
  //    return;

  
  
  //   Int_t i = 0;

    //
    // Mean
    //
  TCanvas *mps_mean_canvas;
  TCanvas *mps_rms_canvas;
  TCanvas *mps_event_canvas;

  TCanvas *hel_mean_canvas[3];
  TCanvas *hel_rms_canvas[3];
  TCanvas *hel_event_canvas[3];

  TH2D *mps_mean_h2D;
  TH2D *mps_rms_h2D;
  TH2D *mps_event_h2D;

  TH2D *hel_mean_h2D[3];
  TH2D *hel_rms_h2D[3];
  TH2D *hel_event_h2D[3];

  Int_t xbinN = 0;
  Int_t ybinN = 0;
  Float_t offset = 0.5;
  Double_t xrange = 0.0;



  if(treename == "Mps_Tree") {
    // 
    // Mean
    //
    mps_mean_canvas = new TCanvas("mean_mps", "Mean Plots MPS Tree", w, h);
    mps_mean_canvas ->SetWindowSize(w + (w - mps_mean_canvas ->GetWw()), h + (h - mps_mean_canvas->GetWh()));
    mps_mean_canvas->cd();
    
    xbinN = 100;
    ybinN = mps_device_num*xbinN;
    xrange = xmax - xmin;
    xmin -= 0.1*xrange;
    xmax += 0.1*xrange;
    
    mps_mean_h2D = new TH2D("MV","Mean Values",
			    xbinN, xmin, xmax,
			    ybinN, offset, ybinN+offset);
    mps_mean_h2D -> SetMarkerStyle(20);
    mps_mean_h2D -> GetYaxis() ->SetLabelSize(0.015);
    
    for(i=0; i<mps_device_num; i++) { // ndevices is Int_t, thus i is Int_t
      mps_mean_h2D->Fill(mps_xaxis[i],((i+1)*xbinN),1);
      mps_mean_h2D->GetYaxis()->SetBinLabel(
					    ((i+1)*xbinN), 
					    Form("%s",devicelist1[i].Data()) 
					    );
    }
    mps_mean_h2D->Draw();
    if(output_ps) mps_mean_canvas -> Print(Form("run_%d_mps_mean.ps", runnum));

    //
    // RMS
    //
    mps_rms_canvas = new TCanvas("rms_mps", "RMS Plots MPS Tree", w, h);
    mps_rms_canvas ->SetWindowSize(w + (w - mps_rms_canvas ->GetWw()), h + (h - mps_rms_canvas->GetWh()));
    mps_rms_canvas->cd();
    
    xrange = xmaxrms - xminrms;
    xminrms -= 0.1*xrange;
    xmaxrms += 0.1*xrange;
    
    mps_rms_h2D = new TH2D("RMSPV","RMS Values",
			    xbinN, xminrms, xmaxrms,
			    ybinN, offset, ybinN+offset);
    mps_rms_h2D -> SetMarkerStyle(20);
    mps_rms_h2D -> GetYaxis() ->SetLabelSize(0.015);
    
    for(i=0; i<mps_device_num; i++) { // ndevices is Int_t, thus i is Int_t
      mps_rms_h2D->Fill(mps_xaxisrms[i],((i+1)*xbinN),1);
      mps_rms_h2D->GetYaxis()->SetBinLabel(
					    ((i+1)*xbinN), 
					    Form("%s",devicelist1[i].Data()) 
					    );
    }
    mps_rms_h2D->Draw();
    if(output_ps) mps_rms_canvas -> Print(Form("run_%d_mps_rms.ps", runnum));

    //
    // Events
    //
    mps_event_canvas = new TCanvas("event_mps", "EVENT Plots MPS Tree", w, h);
    mps_event_canvas ->SetWindowSize(w + (w - mps_event_canvas ->GetWw()), h + (h - mps_event_canvas->GetWh()));
    mps_event_canvas->cd();
    
    xbinN = 1000;

    ybinN = mps_device_num*100;

    xrange = xmaxevent - xminevent;
    xminevent -= 0.1*xrange;
    xmaxevent += 0.1*xrange;
    
    mps_event_h2D = new TH2D("EVENTPV","EVENT Values",
			    xbinN, xminevent, xmaxevent,
			    ybinN, offset, ybinN+offset);
    mps_event_h2D -> SetMarkerStyle(20);
    mps_event_h2D -> GetYaxis() ->SetLabelSize(0.015);
    
    for(i=0; i<mps_device_num; i++) { // ndevices is Int_t, thus i is Int_t
      mps_event_h2D->Fill(mps_xaxisevents[i],((i+1)*100),1);
      mps_event_h2D->GetYaxis()->SetBinLabel(
					    ((i+1)*100), 
					    Form("%s",devicelist1[i].Data()) 
					    );
    }
    mps_event_h2D->Draw();
    if(output_ps) mps_event_canvas -> Print(Form("run_%d_mps_event.ps", runnum));

    std::ofstream       pedestaloutput;
    std::ostringstream  pedestal_stream;

    char string2[100];
    char string3[]="signalinfo10000.txt";
    sprintf(string3,"signalinfo%i.txt",runnum);

    pedestaloutput.open(string3);
    for(i=0; i<mps_device_num; i++){  
      pedestal_stream << "device name = " << std::setw(20) << devicelist1[i]
		      << " || "
		      << " mean = " << std::setw(10) << mps_xaxis[i]
		      << " || "
		      << " rms = " << std::setw(10) << mps_xaxisrms[i]
		      << " || "
		      << " events = " << mps_xaxisevents[i]
		      << "\n";
    } 
    pedestaloutput << pedestal_stream.str();
    std::cout << pedestal_stream.str() <<std::endl;;
    pedestaloutput.close();
  
  }
  // else {
  //   plots_num = 3;
  // }

  //   for(i=0; i< plots_num; i++) {

  //     mean_canvas[i] = new TCanvas(Form("mean_canvas%d",i) ,"Mean",w,h);     
  //     mean_canvas[i] ->SetWindowSize(w + (w - mean_canvas[i] ->GetWw()), h + (h - mean_canvas[i]->GetWh()));
  //     mean_canvas[i]->cd();

  //     TH2D *mean_h2d;
  //     Double_t xrange = 0.0;
  //     xrange = xmax - xmin;
  //     xmin -= 0.1*xrange;
  //     xmax += 0.1*xrange;
      
  //     mean_h2d = new TH2D("MV","Mean Values",
  // 			  100, xmin, xmax,
  // 			  DeviceNum*100, 0.5, (DeviceNum*100)+0.5);
  //     mean_h2d->SetMarkerStyle(20);
  //     mean_h2d->GetYaxis()->SetLabelSize(0.015);
  //     for(i=0; i<DeviceNum; i++) { // ndevices is Int_t, thus i is Int_t
  // 	mean_h2d->Fill(xaxis[i],((i+1)*100),1);
  // 	mean_h2d->GetYaxis()->SetBinLabel(
  // 					  ((i+1)*100), Form("%s",devicelist1[i].Data()) 
  // 					  );
  //     }
  //     mean_h2d->Draw();
  //     mean_canvas->Print(Form("%i_signal_mean_values.ps",runnum));
    
  //   //
  //   // RMS
  //   //
  //   TCanvas *rms_canvas = NULL;
  //   if(rms_canvas) delete rms_canvas; rms_canvas = NULL;
      
  //   rms_canvas = new TCanvas("rms_canvas","RMS",w,h);     
  //   rms_canvas ->SetWindowSize(w + (w - rms_canvas ->GetWw()), h + (h - rms_canvas->GetWh()));
  //   rms_canvas->cd();
  //   TH2D *rms_h2d;
  //   xrange = 0.0; // recycle xrange 
  //   xrange = xmaxrms - xminrms;
  //   xminrms -= 0.1*xrange;
  //   xmaxrms += 0.1*xrange;
    
  //   rms_h2d = new TH2D("RMSPV","RMS Values", 
  // 		       100, xminrms, xmaxrms,
  // 		       DeviceNum*100, 0.5, (DeviceNum*100)+0.5);
  //   rms_h2d -> SetMarkerStyle(20);
  //   rms_h2d -> GetYaxis()->SetLabelSize(0.015);
  //   for(i=0; i<DeviceNum; i++) { // ndevices is Int_t, thus i is Int_t
  //     rms_h2d -> Fill(xaxisrms[i],((i+1)*100),1);
  //     rms_h2d -> GetYaxis()->SetBinLabel(
  // 					 ((i+1)*100), Form("%s",devicelist1[i].Data()) 
  // 					 );
  //   }
  //   rms_h2d->Draw();
  //   rms_canvas->Print(Form("%i_signal_rms_values.ps",runnum));

  //   //
  //   // EVENTS
  //   //
  //   TCanvas *event_canvas = NULL;
  //   if(event_canvas) delete event_canvas; event_canvas = NULL;

  //   event_canvas = new TCanvas("event_canvas","Events", w, h);     
  //   event_canvas ->SetWindowSize(w + (w - event_canvas ->GetWw()), h + (h - event_canvas->GetWh()));
  //   event_canvas->cd();
  //   TH2D *event_h2d;
  //   xrange = 0.0; // recycle xrange 
  //   xrange = xmaxevent - xminevent;
  //   xminevent -= 0.1*xrange;    
  //   xmaxevent += 0.1*xrange;
    
  //   event_h2d = new TH2D("ENV", "Event Number Values",
  // 			 1000,xminevent,xmaxevent,
  // 			 (DeviceNum*100),0.5,(DeviceNum*100)+0.5);
  //   event_h2d->SetMarkerStyle(20);
  //   event_h2d->GetYaxis()->SetLabelSize(0.015);
  //   for(i=0; i<DeviceNum; i++) { // ndevices is Int_t, thus i is Int_t
  //     event_h2d->Fill(xaxisevents[i],((i+1)*100),1);
  //     event_h2d->GetYaxis()->SetBinLabel(
  // 					 ((i+1)*100), Form("%s",devicelist1[i].Data()) 
  // 					 );
  //   }
  //   event_h2d->Draw();
  //   event_canvas->Print(Form("%i_signal_event_values.ps",runnum));

  //   ofstream pedestaloutput;
  //   char string2[100];
  //   char string3[]="signalinfo10000.txt";
  //   sprintf(string3,"signalinfo%i.txt",runnum);
  //   pedestaloutput.open(string3);
  //   for(i=0; i<DeviceNum; i++){  
  //     sprintf(string2,"device name = %s || mean = %f || rms = %f || events=%3.0f",devicelist1[i].Data(),xaxis[i],xaxisrms[i],xaxisevents[i]);
  //     pedestaloutput << string2 << endl;
  //     std::cout << string2 << std::endl;
  //   } 
  //   pedestaloutput.close();
  //   }

  //   if ( treename == "Hel_Tree") { 
      
  //   }
    
  //   std::cout<<"Done plotting!\n";
  // // }
  // // else {
  // //      std::cout<<"Unrecognized tree name"<<std::endl;
  // //      return;
  // //    }
};
