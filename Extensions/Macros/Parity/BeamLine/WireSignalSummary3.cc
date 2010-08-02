//********************************************************************//
// Author : J. Kaisen
//          J. H. Lee
// Date   : July 27, 2010
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

// Plot(run number, map file, tree (Hel_Tree or Mps_Tree), histogram to look at)




//********************************************
// Main function
//********************************************

//!!!!For the main detector map this may duplicate functions that have more than one detector in the name.!!!!
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "TH1D.h"

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
	  for(size_t i=0;i<bpmlist.size();i++)
	    if(devicename==bpmlist[i])
	      flag=kTRUE;
	  if(!flag)
	    bpmlist.push_back(devicename);
	  else
	    std::cout<<"I already found this bpm"<<std::endl;
	}
	else {
	devicename= devicename + subname;
	devicelist1.push_back(devicename);
	counter1++;
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
	counter1++;
	}
      }
      else if (det_type=="bcm,"){
	devicename.ToLower();
	if(treename=="Hel_Tree")
	  bpmlist.push_back(devicename);
	else {
	  devicelist1.push_back(devicename);
	  counter1++;
	}
      }
      else if (det_type=="integrationpmt,"){
	devicename.ToLower();
	if(treename=="Hel_Tree")
	  bpmlist.push_back(devicename);
	else {
	devicelist1.push_back(devicename);
	counter1++;
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
	  
      
    
  const Int_t ndevices = counter;
  const Int_t ndevices1 = counter1;
  const Int_t ndevices2 = counter2;
  const Int_t ndevices3 = counter3;    
  
  if(treename == "Hel_Tree") {
    std::cout << "device list1 asym size " 
	      << devicelist1.size() 
	      << " ndevices1 "
	      << counter1
	      << std::endl;
    std::cout << "device list2 diff size " 
	      << devicelist2.size() 
	      << " ndevices2 "
	      << counter2
	      << std::endl;
    std::cout << "device list3 yield size " 
	      << devicelist3.size() 
	      << " ndevices3 "
	      << counter3
	      << std::endl;
  }
  else
    std::cout << "device list size " << devicelist1.size() << std::endl;


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
  
  //Get the root file
  //TString directory="~/scratch/rootfiles"; // the location of the rootfile used for calibration
  //  Char_t filename[300];
    
  //  sprintf(filename,"%s/Qweak_%d.000.root",directory.Data(),runnum);

  //  TFile * file = new TFile(filename);
  //   if(file -> IsZombie()) {
  //    printf("Error opening file\n"); 
  //    delete file; file = NULL;
  //    return;
  //  }
  
  //  std::cout<< "Obtaining data from: "
  //	   << filename
  //	   << std::endl;

  TTree* nt = (TTree*) file->Get(treename); //load the Tree
  TString ctitle[3];
  TCanvas *signal_plots_canvas[3] = {NULL};
  
  Double_t w = 1000;
  Double_t h = 800;
 
  Int_t i = 0;
  for(i=0; i<3; i++ ){
    if(signal_plots_canvas[i]) delete signal_plots_canvas[i]; signal_plots_canvas[i] = NULL;
  }
    
  Int_t plots_num = 0;
  TPaveText *title[3];
  TPad      *pad[3];

  //  devicenum;
  // 
  //const Int_t ndevices = counter;
  //  const Int_t ndevices1 = counter1;
  //  const Int_t ndevices2 = counter2;
  //  const Int_t ndevices3 = counter3; 

  Int_t hel_device_num[3] = {counter1, counter2, counter3};

  // std::vector< std::vector<TH1* > > h1D_list;
  
  if(treename == "Mps_Tree")       {
    plots_num = 1;
    ctitle[0] = "MPS signal plots";

  }
  else if (treename == "Hel_Tree") {
    plots_num = 3;
    ctitle[0] = "HEL signal plots for asym";
    ctitle[1] = "HEL signal plots for diff";
    ctitle[2] = "HEL signal plots for yield";

  }
  else return;

  // h1D_list.resize(plots_num); //1 for MPS, 3 for HEL

  for(i = 0; i <plots_num; i++) {
    signal_plots_canvas[i] = new TCanvas(Form("signal_plots_canvas%d",i), ctitle[i], w, h);     
    signal_plots_canvas[i] ->SetWindowSize(w + (w - signal_plots_canvas[i] ->GetWw()), h + (h - signal_plots_canvas[i]->GetWh()));
  //   h1D_list.at(i).resize(hel_device_num[i]);
  // }
  }
  
  
  for(i = 0; i <plots_num; i++) {
    signal_plots_canvas[i]->cd();
    title[i] =  new TPaveText(0.01,0.953,0.5,0.999);
    title[i] -> SetFillColor(11);
    title[i] -> AddText(ctitle[i]);
    title[i] -> Draw();
    pad[i]   = new TPad("pad","pad",0.00,0.00,1.00,0.95);
    pad[i] -> Draw();
    pad[i] -> cd();
    pad[i] -> Divide(4,4);
    
  }
 

  // if(treename == "Mps_Tree") {
  //   const Int_t DeviceNum = ndevices;
  // }
  // else if (treename == "Hel_Tree") {
  //   const Int_t DeviceNum = ndevices1;
  // }
  // else
  //   return 0;
  

  // Double_t xaxis[DeviceNum], xaxisrms[DeviceNum], xaxisevents[DeviceNum]; // reuse
  // Double_t xmin=999e+10,xmax=-999e+10,xmaxrms=-999e+10,xminrms=99e+10,xmaxevent=-999e+10,xminevent=99e+10;

  // Int_t j=0;
  // if(treename == "Mps_Tree") {

  //   signal_plots_canvas->Print(Form("%i_signal_mps_values.ps[",runnum));
  //   j = 0;
  //   for(Int_t i=0; i<ndevices; i++){
  //     plots_pad -> cd(j+1);

  //     nt->Draw(Form("%s.%s/%s.num_samples",
  // 		    devicelist1[i].Data(),
  // 		    graphinfo.Data(),
  // 		    devicelist1[i].Data()),
  // 	       Form("%s.num_samples>0",devicelist1[i].Data())
  // 	       );
  //     mps_h1D[i] = (TH1D*) gPad -> GetPrimitive("htemp");
  //     mps_h1D[i] -> SetName(Form("%s.%s/%s.num_samples",
  // 			     devicelist1[i].Data(),
  // 			     graphinfo.Data(),
  // 			     devicelist1[i].Data()));
  //     xaxis[i]       = mps_h1D[i]->GetMean();
  //     xaxisrms[i]    = mps_h1D[i]->GetRMS();
  //     xaxisevents[i] = mps_h1D[i]->GetEntries();
  //     if(xaxis[i]>xmax)
  //      	xmax=xaxis[i];
  //     if(xaxis[i]<xmin)
  //     	xmin=xaxis[i];
  //     if(xaxisrms[i]>xmaxrms)
  //     	xmaxrms=xaxisrms[i];
  //     if(xaxisrms[i]<xminrms)
  //     	xminrms=xaxisrms[i];
  //     if(xaxisevents[i]>xmaxevent)
  //     	xmaxevent=xaxisevents[i];
  //     if(xaxisevents[i]<xminevent)
  //     	xminevent=xaxisevents[i];
  //     j++;  
  //     if(j==16){
  //     	j=0;
  // 	signal_plots_canvas->Print(Form("%i_signal_mps_values.ps",runnum));
  //     }
  //   }
  //   signal_plots_canvas->Print(Form("%i_signal_mps_values.ps",runnum)); 
  //   signal_plots_canvas->Print(Form("%i_signal_mps_values.ps]",runnum));
  // }
  // else if ( treename == "Hel_Tree") {   

  //   signal_plots_canvas->Print(Form("asym_%i_signal_hel_values.ps[",runnum));
  //   j = 0;
  //   for(Int_t i=0; i<ndevices1; i++){
  //     plots_pad -> cd(j+1);
  //     nt->Draw(
  // 	       Form("%s.%s/%s.num_samples",
  // 		    devicelist1[i].Data(),
  // 		    graphinfo.Data(),
  // 		    devicelist1[i].Data()), ""
  // 	       );
		     
  //     hel_h1D[i] = (TH1D*) gPad -> GetPrimitive("htemp");
  //     hel_h1D[i] -> SetName(
  // 			    Form("Norm %s.%s",
  // 				 devicelist1[i].Data(),
  // 				 graphinfo.Data())
  // 			    );
      
  //     xaxis[i]       = hel_h1D[i]->GetMean();
  //     xaxisrms[i]    = hel_h1D[i]->GetRMS();
  //     xaxisevents[i] = hel_h1D[i]->GetEntries();

  //     if(xaxis[i]>xmax)      	    xmax=xaxis[i];
  //     if(xaxis[i]<xmin)      	    xmin=xaxis[i];
  //     if(xaxisrms[i]>xmaxrms) 	    xmaxrms=xaxisrms[i];
  //     if(xaxisrms[i]<xminrms)      xminrms=xaxisrms[i];
  //     if(xaxisevents[i]>xmaxevent) xmaxevent=xaxisevents[i];
  //     if(xaxisevents[i]<xminevent) xminevent=xaxisevents[i];

  //     j++;  
  //     if(j==16){
  //     	j=0;
  // 	signal_plots_canvas->Print(Form("asym_%i_signal_hel_values.ps",runnum));
  //     }
  //   }
  //   signal_plots_canvas->Print(Form("asym_%i_signal_hel_values.ps",runnum));
  //   signal_plots_canvas->Print(Form("asym_%i_signal_hel_values.ps]",runnum));
  // }
  // else {
  //   std::cout<<"Unrecognized tree name"<<std::endl;
  //   return;
  // }


  
  //   Int_t i = 0;

  //   //
  //   // Mean
  //   //
  //   TCanvas *mean_canvas[3] = {NULL};
  //   TCanvas *rms_canvas[3]  = {NULL};
  //   TCanvas *event_canvas[3] = {NULL};
    
  //   Int_t i = 0;
  //   for(i=0; i<3; i++ ){
  //     if(mean_canvas[i]) delete mean_canvas[i]; mean_canvas[i] = NULL;
  //     if(rms_canvas[i]) delete rms_canvas[i]; rms_canvas[i] = NULL;
  //     if(event_canvas[i]) delete event_canvas[i]; event_canvas[i] = NULL;
      
  //   }
    
  //   Int_t plots_num = 0;
    
  //   if(treename == "Mps_Tree")       plots_num = 1;
  //   else if (treename == "Hel_Tree") plots_num = 3;

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
