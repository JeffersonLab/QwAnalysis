//   r3_hitprofile_3pass.C
//
// author: David Armstrong
//
//   Makes Region 3 partial track-based Hit Map at entrance to Pb pre-radiator
//   Output in the form of a large tab-separated .txt file
//     See: https://qweak.jlab.org/elog/Tracking/225  for details
//
//    Special version for 3-pass data; chain together many runs.
//
//   For now, due to high correlation of "y" and "yslope", decide 
//   not to bin in ylsope... 
//
//  based originally on  r3_projection.C
//
//  Usage: first you must put the rootfile which contains 
//   the data under the $QW_ROOTFILES directory and then type:
//   project_root(package, octant, runnumber,events)
//   where "package" = package number (1 or 2)
//         "octant" = main detector octant number (1 to 8)
//         "runnumber" = run number
//         "events" is number of events to analyze (default is whole run)
//   
//      
#include<iostream>
#include<fstream>
#include <string>
#include <TF1.h>
#include <stdio.h>
#include <stdlib.h>
#include <TChain.h>

#define xbin 15 //number of bins in x
#define ybin 20 //number of bins in y
#define xslopebin 14 //number of bins in xslope
#define yslopebin 20 //number of bins in yslope

const double pe_convert[]={14.0141,16.6726,21.9799,38.5315,20.4254,20.3896,22.1042,22.3945,22.7986,30.0517,28.7274,25.3396,24.6273,16.0718,27.8305,12.3251};

TCanvas *c;


void project_root(int package=1,int md_number=1,int run_number=6327,int max_events=-1,TString file_suffix="Qweak_",string command="MD_")
{

  //  The below are the nominal values from the qweak_MD.geo file.
  //  I believe these are MD centers, not front faces
  //  MDS are 1.25 cm thick, so front face is actually 0.675 cm upstream
  //  of these values. 
  //  Now the Pb pre-radiator is (center) nominally 4.94 cm upstrean of the 
  //  MD (center). Taking into account the width of the Pb (2 cm), this 
  // means the front face of the Pb is 5.94 cm upstream of the center of the
  // MD, i.e. 5.94 cm less than the values in md_zpos

  Double_t md_zpos[9] = {0.0, 576.665,  576.705, 577.020, 577.425, 577.515,  577.955, 577.885, 577.060};

  Double_t Pb_zoffset = 5.94;

  //  Int_t hit_map[15][20][14][20] = {0};  // 4D hit map 
  Int_t hit_map[xbin][ybin][xslopebin] = {0};  // 3D hit map  (no yslope)

  TChain* chain = new TChain("event_tree");
  chain->Add("/cache/hallc/qweak/rootfiles/tracking_pass7/Qweak_17990.root");
  chain->Add("/cache/hallc/qweak/rootfiles/tracking_pass7/Qweak_17991.root");
  chain->Add("/cache/hallc/qweak/rootfiles/tracking_pass7/Qweak_17993.root");
  chain->Add("/cache/hallc/qweak/rootfiles/tracking_pass7/Qweak_17994.root");
  chain->Add("/cache/hallc/qweak/rootfiles/tracking_pass7/Qweak_17996.root");
  chain->Add("/cache/hallc/qweak/rootfiles/tracking_pass7/Qweak_17998.root");
  
  TString outputPrefix(Form("Hit_Map_%d_MD%d_",run_number,md_number));
  TString file_name = "";
  /*
  file_name += gSystem->Getenv ( "QW_ROOTFILES" );
  file_name +="/";
  file_name += file_suffix;
  file_name += run_number;
  file_name +=".root";
  */

  //  TFile *file = new TFile ( file_name );

  //  TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
  QwEvent* fEvent=0;
  QwPartialTrack* pt=0;
  QwTreeLine* tl=0;
  // QwHit* hits=0;

  //check if you put some crazy numbers here
  if (package<=0 || package >4 || md_number <=0 || md_number > 8)
    {
      cout << "put the wrong package or main detector number, please check agian! " << endl;
      return;
    }
  // section to process the command

  size_t type_found;
  string type;
  type_found=command.find_first_of("_");
  if (type_found==string::npos)
    {
      cerr << "bad things happens! You need to type a underscore _" << endl;
      return;
    }
  else
    {
      type.assign(command,0,type_found);
    }

  bool pe=false;
  if (command.find("photon")==string::npos)
    pe=false;
  else pe=true;

  //  int mode=0;   //p,m,m+p,p-m;starting from 1
  int mode=3;   // make mode=3 the default
  size_t found=0;
  if ((found=command.find("+"))!=string::npos)
    mode=3;
  else if ((found=command.find("-"))!=string::npos)
    mode=4;
  else if (found=command.find_first_of("p")!=string::npos)
    {
      if (command.at(++found)!="r" || command.at(++found)!="e")
	mode=1;
    }
  else if (command.find("m")!=string::npos)
    mode=2;

  string w_title;
  cout << "mode: " << mode << endl;
  if (mode==0)
    w_title="not weighted";
  else
    w_title="weighted";


  TH1D* xhits = new TH1D("xhits", Form("X profile at Pb, Run %d, Oct. %d", run_number, md_number), 60, 320, 350);
  TH1D* yangle = new TH1D("yangle", Form("Y angles at Pb, Run %d, Oct. %d", run_number, md_number), 200, -0.2, 0.2);
  TH1D* xangle = new TH1D("xangle", Form("X angles at Pb, Run %d, Oct. %d", run_number, md_number), 40, 0.32, 0.52);
  TH2F* hits = new TH2F("hits", Form("X vs Y at Pb, Run %d, Oct. %d", run_number, md_number),240, -120, 120, 100, 310,360);


  //  TBranch* branch_event=event_tree->GetBranch("events");
  //  TBranch* branch     = event_tree->GetBranch("maindet");

  chain->SetBranchAddress ( "events",&fEvent );

  // TBranch* branch_event=chain->GetBranch("events");
  //  TBranch* branch     = chain->GetBranch("maindet");
  //  branch_event->SetAddress(&fEvent);

  Int_t nevents=chain->GetEntries();

  cout << "total number of entries =  " <<nevents << endl;

  if (max_events == -1)max_events = nevents; 

  Double_t dz = 0.0;
  
  if      (type=="QTOR") dz = 0.0;
  else if (type=="TS")   dz = 539.74;
  else if (type=="MD")   dz = md_zpos[md_number] - Pb_zoffset;
  else if (type=="SC")   dz = 591.515;
  else {
    cerr << "no such plane!" << endl;
    return;
  }


  Double_t ystep = 10.0;     // bin size in cm for y (long axis of bar)
  Double_t xstep = 2.0;      // bin size in cm for x (width of bar)
  Double_t xslopestep = 0.010;  // bin size in rad for xslope (radial)
  Double_t yslopestep = 0.015;  // bin size in rad for yslope (azimuthal)
  Double_t ylow = -100.0;   // lowest edge of fiducial in y
  Double_t xlow = 320.0;   // lowest edge of fiducial in x
  Double_t yslopelow = -0.15;   // lowest edge of fiducial in yslope
  Double_t xslopelow = 0.34;   // lowest edge of fiducial in xslope

  Int_t n_out_of_range = 0;
  Int_t n_filled = 0;
  Int_t n_tracks_used = 0;

  for(int i = 0; i < max_events; i++)
	{

	  //	  cout << " i = " << i << endl; 

	  if(i % 100000 == 0) cout <<"events processed so far:" << i << endl;
	  //	  branch_event->GetEntry(i);
	  //	  branch->GetEntry(i);
	  chain->GetEntry(i);

	  Double_t xoffset, yoffset, xslope, yslope, x, y;

	 for (int num_p=0; num_p< fEvent->GetNumberOfPartialTracks();num_p++)
	    {
	      pt=fEvent->GetPartialTrack(num_p);
              if (pt->GetRegion()==3 && pt->GetPackage()==package)
	{
	      n_tracks_used++;
	      xoffset = pt->fOffsetX;
	      yoffset = pt->fOffsetY;
	      xslope  = pt->fSlopeX;
	      yslope  = pt->fSlopeY;
	      x       = xoffset+xslope*dz;
	      y       = yoffset+yslope*dz;
	      
	      // convert from global to local coordinates

	      Double_t theta = -45.0*3.14159/180.0*(md_number - 1);
	      Double_t xx = x*cos(theta) - y*sin(theta);
	      Double_t yy = x*sin(theta) + y*cos(theta);
	      x = xx;
	      y = yy;
	      Double_t xslopelocal = xslope*cos(theta) - yslope*sin(theta);
	      Double_t yslopelocal = xslope*sin(theta) + yslope*cos(theta);

	      /*
              // Diagnostic output
	      cout << "x = " << x << "  y = " << y << endl;
	      cout << "xslopelocal = " << xslopelocal << "  xslope = " << xslope << "  yslopelocal = " << yslopelocal << "  yslope = " << yslope << endl; 
	      */

	      Bool_t out_of_range = false;
	      Int_t iy = (y-ylow)/ystep; 
	      Int_t ix = (x-xlow)/xstep; 
	      Int_t isy = (yslopelocal-yslopelow)/yslopestep; 
	      Int_t isx = (xslopelocal-xslopelow)/xslopestep; 

	      if (iy>(ybin-1) || iy<0 ){
		out_of_range = true;
		// Diagnostic output
		//		cout << "y = " << y << "  iy = " << iy << "  out of range =  "  << out_of_range << endl;
	      }
	      if (ix>(xbin-1) || ix<0 ){
		out_of_range = true;
		// Diagnostic output
		//		cout << "x = " << x << "  ix = " << ix << "  out of range =  "  << out_of_range << endl;
	      }
	      if (isy>(yslopebin-1) || isy<0){
		out_of_range = true;
		// Diagnostic output
		//		cout << "yslope = " << yslope << "  isy = " << isy << "  out of range =  "  << out_of_range << endl;
	      }
	      if (isx>(xslopebin-1) || isx<0){
		out_of_range = true;
		// Diagnostic output
		//	        cout << "xslope = " << xslope << "  isx = " << isx << "  out of range =  "  << out_of_range << endl;
	      }

	      /*
	      cout << " ix = " << ix << " iy = " << iy << endl;
	      cout << " isx = " << isx << " isy = " << isy << endl;
	      cout << "x = " << x << "  y = " << y << endl;
	      cout << "xslopelocal = " << xslopelocal << "  xslope = " << xslope << "  yslopelocal = " << yslopelocal << "  yslope = " << yslope << endl; 
	      */

	      if(out_of_range) n_out_of_range++;
	      if(!out_of_range) {
		//		hit_map[ix][iy][isx][isy]++;  //increment bin in hitmap
		hit_map[ix][iy][isx]++;  //increment bin in hitmap
		n_filled++;
		yhits->Fill(y);
		xhits->Fill(x);
		yangle->Fill(yslopelocal);
		xangle->Fill(xslopelocal);
		hits->Fill(y,x);
	      }
 
	     } // Region 3 partial track in right octant
	    }  // loop over partial tracks 
	} // loop over events

  std::ofstream fout;
  fout.open(outputPrefix+".txt");
  for (int i1 = 0;  i1< xbin; i1++){
  //  for (int i1 = 3;  i1< 4; i1++){
    for (int i2 = 0;  i2 < ybin; i2++){
      for (int i3= 0; i3 < xslopebin; i3++){
	//	cout << i1 << "\t" << i2 << "\t"  << i3 << "\t"  << hit_map[i1][i2][i3] << endl;
	fout << i1 << "\t" << i2 << "\t"  << i3 << "\t"  << hit_map[i1][i2][i3] << endl;
      }
    }
  }

  cout <<  " number of tracks off fiducial = " << n_out_of_range << endl;
  cout <<  " number of tracks used = " << n_tracks_used << endl;
  cout <<  " entries added to hit_map = " << n_filled << endl;
  cout <<  " events in run = " << nevents << endl;
  cout <<  " events analyzed = " << max_events << endl;

  c_yhits = new TCanvas("c_yhits", "Y profile", 10, 10, 700, 700);
  yhits->GetXaxis()->SetTitle("Y location  (cm)");
  yhits->Draw(); 
  yhits->SaveAs(outputPrefix+"yhits.C"); 
  c_yhits->SaveAs(outputPrefix+"yhits.pdf"); 

  c_xhits = new TCanvas("x_yhits", "X profile", 10, 10, 700, 700);
  yhits->GetXaxis()->SetTitle("X location  (cm)");
  xhits->Draw(); 
  xhits->SaveAs(outputPrefix+"xhits.C"); 
  c_xhits->SaveAs(outputPrefix+"xhits.pdf"); 

  c_yangle = new TCanvas("c_yangle", "Y Angles", 10, 10, 700, 700);
  yangle->GetXaxis()->SetTitle("Y angle  (radians)");
  yangle->Draw(); 
  yangle->SaveAs(outputPrefix+"yangles.C"); 
  c_yangle->SaveAs(outputPrefix+"yangles.pdf"); 

  c_xangle = new TCanvas("c_xangle", "X angles", 10, 10, 700, 700);
  xangle->GetXaxis()->SetTitle("X
 angle  (radians)");
  xangle->Draw(); 
  xangle->SaveAs(outputPrefix+"xangles.C"); 
  c_xangle->SaveAs(outputPrefix+"xangles.pdf"); 

  c_hits = new TCanvas("c_hits", "X vs Y hits", 10, 10, 700, 700);
  hits->GetXaxis()->SetTitle("Y location  (cm)");
  hits->GetYaxis()->SetTitle("X location  (cm)");
  hits->Draw("COLZ");
  hits->SaveAs(outputPrefix+"hits.C"); 
  c_hits->SaveAs(outputPrefix+"hits.pdf"); 

  return;
};


