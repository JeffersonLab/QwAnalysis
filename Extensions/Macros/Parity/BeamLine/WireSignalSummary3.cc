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
// root[0] .L WireSignalSummary.cc
// root[1]  Plot(1160,"qweak_hallc_beamline.map","Mps_Tree","hw_sum")



//********************************************
// Main function
//********************************************

//!!!!For the main detector map this may duplicate functions that have more than one detector in the name.!!!!
#include "TMath.h"
#include <iostream>
#include <fstream>

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

  //  TFile *file =  new TFile(Form("$QW_ROOTFILES/Qweak_%d.000.root",runnum));

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
  
  std::cout << "device list size " << devicelist1.size() << std::endl;

  const Int_t ndevices = counter;
     

  Bool_t canvas_style = true;

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
  TString directory="~/scratch/rootfiles"; // the location of the rootfile used for calibration
  Char_t filename[300];
    
  sprintf(filename,"%s/Qweak_%d.000.root",directory.Data(),runnum);

  TFile * file = new TFile(filename);
   if(file -> IsZombie()) {
    printf("Error opening file\n"); 
    delete file; file = NULL;
    return;
  }
  
  std::cout<< "Obtaining data from: "
	   << filename
	   << std::endl;

  TTree* nt = (TTree*) file->Get(treename); //load the Tree
  TString ctitle = Form("Signal plots of  %i",runnum);
  TCanvas *signal_plots_canvas = NULL;
  
 

  Double_t w = 1000;
  Double_t h = 800;
 
  if(signal_plots_canvas) delete signal_plots_canvas; signal_plots_canvas = NULL;

  signal_plots_canvas = new TCanvas("siganl_plots_canvas",ctitle,w,h);     
  signal_plots_canvas ->SetWindowSize(w + (w - signal_plots_canvas ->GetWw()), h + (h - signal_plots_canvas->GetWh()));

  TPaveText *title = new TPaveText(0.01,0.953,0.5,0.999);
  title->SetFillColor(11);
  title->AddText(ctitle);
  title->Draw();

  TPad *plots_pad = new TPad("pad","pad",0.00,0.00,1.00,0.95);
  plots_pad->Draw();
  plots_pad->cd();
  plots_pad->Divide(4,4);

  TH1D *h1D[ndevices]; 
  
  if(treename == "Mps_Tree") {
    Double_t xaxis[ndevices], xaxisrms[ndevices], xaxisevents[ndevices];
    Double_t xmin=999e+10,xmax=-999e+10,xmaxrms=-999e+10,xminrms=99e+10,xmaxevent=-999e+10,xminevent=99e+10;
    Int_t j=0;
    signal_plots_canvas->Print(Form("%i_signal_mps_values.ps[",runnum));
    for(Int_t i=0; i<ndevices; i++){
      plots_pad -> cd(j+1);

      nt->Draw(Form("%s.%s/%s.num_samples",
  		    devicelist1[i].Data(),
  		    graphinfo.Data(),
  		    devicelist1[i].Data()),
  	       Form("%s.num_samples>0",devicelist1[i].Data())
  	       );
      h1D[i] = (TH1D*) gPad -> GetPrimitive("htemp");
      h1D[i] -> SetName(Form("%s.%s/%s.num_samples",
			     devicelist1[i].Data(),
			     graphinfo.Data(),
			     devicelist1[i].Data()));
      xaxis[i]       = h1D[i]->GetMean();
      xaxisrms[i]    = h1D[i]->GetRMS();
      xaxisevents[i] = h1D[i]->GetEntries();
      if(xaxis[i]>xmax)
       	xmax=xaxis[i];
      if(xaxis[i]<xmin)
      	xmin=xaxis[i];
      if(xaxisrms[i]>xmaxrms)
      	xmaxrms=xaxisrms[i];
      if(xaxisrms[i]<xminrms)
      	xminrms=xaxisrms[i];
      if(xaxisevents[i]>xmaxevent)
      	xmaxevent=xaxisevents[i];
      if(xaxisevents[i]<xminevent)
      	xminevent=xaxisevents[i];
      j++;  
      if(j==16){
      	j=0;
	signal_plots_canvas->Print(Form("%i_signal_mps_values.ps",runnum));
      }
    }
    signal_plots_canvas->Print(Form("%i_signal_mps_values.ps",runnum)); 
    signal_plots_canvas->Print(Form("%i_signal_mps_values.ps]",runnum));
  }
  else {
    std::cout<<"Unrecognized tree name"<<std::endl;
    return;
  }


  if(treename=="Mps_Tree"){
    Int_t i = 0;

    //
    // Mean
    //
    TCanvas *mean_canvas = NULL;

    if(mean_canvas) delete mean_canvas; mean_canvas = NULL;

    mean_canvas = new TCanvas("mean_canvas","Mean",w,h);     
    mean_canvas ->SetWindowSize(w + (w - mean_canvas ->GetWw()), h + (h - mean_canvas->GetWh()));
    mean_canvas->cd();
    TH2D *mean_h2d;
    Double_t xrange = 0.0;
    xrange = xmax - xmin;
    xmin -= 0.1*xrange;
    xmax += 0.1*xrange;
    
    mean_h2d = new TH2D("MV","Mean Values",
			100, xmin, xmax,
			(ndevices*100),0.5,(ndevices*100)+0.5);
    mean_h2d->SetMarkerStyle(20);
    mean_h2d->GetYaxis()->SetLabelSize(0.015);
    for(i=0; i<ndevices; i++) { // ndevices is Int_t, thus i is Int_t
      mean_h2d->Fill(xaxis[i],((i+1)*100),1);
      mean_h2d->GetYaxis()->SetBinLabel(
					((i+1)*100), Form("%s",devicelist1[i].Data()) 
					);
    }
    mean_h2d->Draw();
    mean_canvas->Print(Form("%i_signal_mean_values.ps",runnum));
 
    //
    // RMS
    //
    TCanvas *rms_canvas = NULL;
    if(rms_canvas) delete rms_canvas; rms_canvas = NULL;

    rms_canvas = new TCanvas("rms_canvas","Mean",w,h);     
    rms_canvas ->SetWindowSize(w + (w - rms_canvas ->GetWw()), h + (h - rms_canvas->GetWh()));
    rms_canvas->cd();
    TH2D *rms_h2d;
    xrange = 0.0; // recycle xrange 
    xrange = xmaxrms - xminrms;
    xminrms -= 0.1*xrange;
    xmaxrms += 0.1*xrange;
    
    rms_h2d = new TH2D("RMSPV","RMS Values", 
		       100, xminrms, xmaxrms,
		       (ndevices*100), 0.5, (ndevices*100)+0.5);
    rms_h2d -> SetMarkerStyle(20);
    rms_h2d -> GetYaxis()->SetLabelSize(0.015);
    for(i=0; i<ndevices; i++) { // ndevices is Int_t, thus i is Int_t
      rms_h2d -> Fill(xaxisrms[i],((i+1)*100),1);
      rms_h2d -> GetYaxis()->SetBinLabel(
					  ((i+1)*100), Form("%s",devicelist1[i].Data()) 
					  );
    }
    rms_h2d->Draw();
    rms_canvas->Print(Form("%i_signal_rms_values.ps",runnum));

    //
    // EVENTS
    //
    TCanvas *event_canvas = NULL;
    if(event_canvas) delete event_canvas; event_canvas = NULL;

    event_canvas = new TCanvas("event_canvas","Events", w, h);     
    event_canvas ->SetWindowSize(w + (w - event_canvas ->GetWw()), h + (h - event_canvas->GetWh()));
    event_canvas->cd();
    TH2D *event_h2d;
    xrange = 0.0; // recycle xrange 
    xrange = xmaxevent - xminevent;
    xminevent -= 0.1*xrange;    
    xmaxevent += 0.1*xrange;
    
    event_h2d = new TH2D("ENV", "Event Number Values",
			 1000,xminevent,xmaxevent,
			 (ndevices*100),0.5,(ndevices*100)+0.5);
    event_h2d->SetMarkerStyle(20);
    event_h2d->GetYaxis()->SetLabelSize(0.015);
    for(i=0; i<ndevices; i++) { // ndevices is Int_t, thus i is Int_t
      event_h2d->Fill(xaxisevents[i],((i+1)*100),1);
      event_h2d->GetYaxis()->SetBinLabel(
					 ((i+1)*100), Form("%s",devicelist1[i].Data()) 
					 );
    }
    event_h2d->Draw();
    event_canvas->Print(Form("%i_signal_event_values.ps",runnum));

    ofstream pedestaloutput;
    char string2[100];
    char string3[]="signalinfo10000.txt";
    sprintf(string3,"signalinfo%i.txt",runnum);
    pedestaloutput.open(string3);
    for(i=0; i<ndevices; i++){  
      sprintf(string2,"device name = %s || mean = %f || rms = %f || events=%3.0f",devicelist1[i].Data(),xaxis[i],xaxisrms[i],xaxisevents[i]);
      pedestaloutput << string2 << endl;
      std::cout << string2 << std::endl;
    } 
    pedestaloutput.close();
  
    std::cout<<"Done plotting!\n";
  }
  else {
    std::cout<<"Unrecognized tree name"<<std::endl;
    return;
  }
};
  


void PlotRAW(Int_t runnum, TString mapstring, TString treename)
{
  ifstream mapfile; 
  ifstream branchlistinfile;
  std::ofstream branchlistoutfile;
  std::vector<TString>  devicelist1;
  std::vector<TString>  bpmlist;
  std::vector<TString>  devicelist2;
  std::vector<TString>  devicelist3;
  Int_t counter=0,counter1=0,counter2=0,counter3=0;
  TString devicename,throw_away,test,line,det_type,get,textfile;
  TFile *file =  new TFile(Form("$QW_ROOTFILES/Qweak_%d.000.root",runnum));
  textfile = ("Hel_Branches_list_run_%i.in",runnum);
  get = getenv("QWANALYSIS");
  mapstring = get +"/Parity/prminput/" + mapstring;
  mapfile.open(mapstring);
  while (!mapfile.eof()) {
    line.ReadToken(mapfile);
    if (line.Contains("!"))  line.ReadToDelim(mapfile);
    else if (line.Contains("VQWK")) {
      for(size_t i=0;i<2;i++)  	line.ReadToken(mapfile);
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
	counter++;
	}
      }
      else if(det_type=="bpmcavity,"){
	TString subname=devicename(devicename.Sizeof()-2,1);
	devicename.ToLower();
	if(treename == "Hel_Tree") {
	  devicename=(devicename.Sizeof()-2);
	  Bool_t flag=kFALSE;
	  for(size_t i=0;i<bpmlist.size();i++)
	    if(devicename==bpmlist[i])
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
  
  
  const Int_t ndevices = counter;
     

  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetOptStat(1); 

  // canvas parameters
  //  gStyle->SetFrameBorderMode(0);
  // gStyle->SetFrameBorderSize(0);
  // gStyle->SetFrameFillColor(10);

  // pads parameters
  //  gStyle->SetPadColor(0); 
  //  gStyle->SetPadBorderMode(0);
  //  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.1);  
  gStyle->SetPadRightMargin(0.08);  
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);

  //Get the root file
  TString directory="~/scratch/rootfiles"; // the location of the rootfile used for calibration
  Char_t filename[300];
    
  sprintf(filename,"%s/Qweak_%d.000.root",directory.Data(),runnum);

  f = new TFile(filename);
  if(!f->IsOpen())
      return 0;
  std::cout<<"Obtaining data from: "<<filename<<"\n";

 
  TTree * nt = f->Get(treename); //load the Tree

  TString ctitle = Form("Signal plots of  %i",runnum);
  TCanvas *c = new TCanvas("c",ctitle,1200,1000);

  c->SetFillColor(0);

  pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  pad2 = new TPad("pad2","pad2",0.01,0.02,0.99,0.94);
  pad1->SetFillColor(11);
  
  pad2->Draw();

  pad1->cd();
  t1 = new TText(0.20,0.3,ctitle);
  t1->SetTextSize(0.8);
  t1->Draw();

  pad2->Divide(4,4);

  TH1 *h1;
  if(treename == "Mps_Tree") {
    Double_t xaxis[ndevices], xaxisrms[ndevices], xaxisevents[ndevices];
    Double_t xmin=999e+10,xmax=-999e+10,xmaxrms=-999e+10,xminrms=99e+10,xmaxevent=-999e+10,xminevent=99e+10;
    Int_t j=0;
    c->Print(Form("%i_signal_mps_values_raw.ps[",runnum));
    for(size_t i=0; i<ndevices; i++){
      pad2->cd(j+1);
      nt->Draw(Form("%s.hw_sum_raw/%s.num_samples>>h[%d]",
		    devicelist1[i].Data(),
		    devicelist1[i].Data(),i),
	       Form("%s.num_samples>0",devicelist1[i].Data())
	       );
      h1 = (TH1D*)gDirectory->Get(Form("h[%d]",i));  
      xaxis[i] = h1->GetMean();
      xaxisrms[i] = h1->GetRMS();
      xaxisevents[i] = h1->GetEntries();
      if(xaxis[i]>xmax)
  	xmax=xaxis[i];
      if(xaxis[i]<xmin)
  	xmin=xaxis[i];
      if(xaxisrms[i]>xmaxrms)
  	xmaxrms=xaxisrms[i];
      if(xaxisrms[i]<xminrms)
  	xminrms=xaxisrms[i];
      if(xaxisevents[i]>xmaxevent)
  	xmaxevent=xaxisevents[i];
      if(xaxisevents[i]<xminevent)
  	xminevent=xaxisevents[i];
      j++;  
      if(j==16){
	j=0;
	c->Print(Form("%i_signal_mps_values_raw.ps",runnum));
      }
    }
    c->Print(Form("%i_signal_mps_values_raw.ps",runnum)); 
    c->Print(Form("%i_signal_mps_values_raw.ps]",runnum));
  }
  else
    std::cout<<"Unrecognized tree name"<<std::endl;

  if(treename=="Mps_Tree"){
    char string[50];
    TCanvas *canvas = new TCanvas("canvas","Mean",1200,1000);
    canvas->cd();
    TH2F *gr;
    Double_t xrange = xmax - xmin;
    xmin -= 0.1*xrange;
    xmax += 0.1*xrange;
    gr = new TH2F("MV","Mean Values",100,xmin,xmax,(ndevices*100),0.5,(ndevices*100)+0.5);
    gr->SetMarkerStyle(20);
    gr-> GetYaxis()->SetLabelSize(0.015);
    for(size_t i=0; i<(ndevices); i++)
      gr->Fill(xaxis[i],((i+1)*100),1);
    gr->Draw();
    for(size_t i=0; i<(ndevices); i++) {
      sprintf(string,"%s",devicelist1[i].Data());
      gr -> GetYaxis()->SetBinLabel(((i+1)*100),string);
    } 
 canvas->Print(Form("%i_signal_mean_values.ps",runnum));
 
 
 TCanvas *canvas2=new TCanvas("canvas2","RMS",1200,1000);
 canvas2->cd();
 TH2F *gr2;
 Double_t xrangerms = xmaxrms - xminrms;
 xminrms -= 0.1*xrangerms;
 xmaxrms += 0.1*xrangerms;
 gr2 = new TH2F("RMSPV","RMS Values",2000,xminrms,xmaxrms,(ndevices*100),0.5,(ndevices*100)+0.5);
 gr2->SetMarkerStyle(20);
 gr2-> GetYaxis()->SetLabelSize(0.015);
 for(size_t i=0; i<(ndevices); i++)
   gr2->Fill(xaxisrms[i],((i+1)*100),1);
 gr2->Draw();
 for(size_t i=0; i<(ndevices); i++) {
   sprintf(string,"%s",devicelist1[i].Data());
   gr2 -> GetYaxis()->SetBinLabel(((i+1)*100),string);
 }
 canvas2->Print(Form("%i_signal_rms_values.ps",runnum));
 
 TCanvas *canvas3=new TCanvas("canvas3","Events",1200,1000);
 canvas3->cd();
 TH2F *gr3;
 Double_t xrangeevent = xmaxevent - xminevent;
 xminevent -= 0.1*xrangeevent;
 xmaxevent += 0.1*xrangeevent;
 gr3 = new TH2F("ENV","Event Number Values",1000,xminevent,xmaxevent,(ndevices*100),0.5,(ndevices*100)+0.5);
 gr3->SetMarkerStyle(20);
 gr3->GetYaxis()->SetLabelSize(0.015);
 for(size_t i=0; i<(ndevices); i++)
   gr3->Fill(xaxisevents[i],((i+1)*100),1);
 gr3->Draw();
 for(size_t i=0; i<(ndevices); i++) {
   sprintf(string,"%s",devicelist1[i].Data());
   gr3 -> GetYaxis()->SetBinLabel(((i+1)*100),string);
 }
 canvas3->Print(Form("%i_signal_event_values.ps",runnum));
 
 ofstream pedestaloutput;
 char string2[100];
 char string3[]="signalinfo10000.txt";
 sprintf(string3,"signalinfo%i.txt",runnum);
 pedestaloutput.open(string3);
   for(size_t i=0; i<(ndevices); i++){  
     sprintf(string2,"device name = %s || mean = %f || rms = %f || events=%3.0f",devicelist1[i].Data(),xaxis[i],xaxisrms[i],xaxisevents[i]);
     pedestaloutput << string2 << endl;
     std::cout << string2 << std::endl;
   } 
 pedestaloutput.close();
 
  }
  std::cout<<"Done plotting!\n";
};
