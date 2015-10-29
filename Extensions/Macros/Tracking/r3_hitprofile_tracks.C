//   r3_hitprofile_tracks.C
//
// author: David Armstrong
//
//   Makes Region 3 partial track-based Hit Map at entrance to Pb pre-radiator
//   Output in the form of a large tab-separated .txt file
//     See: https://qweak.jlab.org/elog/Tracking/225  for details
//
//    *** This version uses bridged tracks (no cuts other than a valid bridge)
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

  TString outputPrefix(Form("Hit_Map_Tracks_%d_MD%d_",run_number,md_number));
  TString file_name = "";
  file_name += gSystem->Getenv ( "QW_ROOTFILES" );
  file_name +="/";
  file_name += file_suffix;
  file_name += run_number;
  file_name +=".root";
  
  TFile *file = new TFile ( file_name );

  TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
  QwEvent* fEvent=0;
  QwPartialTrack* pt=0;
  QwTreeLine* tl=0;
  QwTrack* track=0;
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



  TProfile2D* hp_2d;
  hp_2d=new TProfile2D(Form("hp_2d %s profile",w_title.c_str()),"hp_2d ",240,0,0,480,0,0);

  TH1D* h_1f=new TH1D("project run","project run",240,280,400);
  TH1D* adcph = new TH1D("ADCP", "ADCP data", 400, 0, 4000);
  TH1D* adcmh = new TH1D("ADCM", "ADCM data", 400, 0, 4000);
  TH1D* tdcph = new TH1D("TDCP", "TDCP data", 1650, 0, 0);
  TH1D* tdcmh = new TH1D("TDCM", "TDCM data", 1650, 0, 0);
  TH1D* adcpped = new TH1D("ADCPPED", "ADCP pedestal", 50, 0, 400);
  TH1D* adcmped = new TH1D("ADCMPED", "ADCM pedestal", 50, 0, 400);

  TH1D* adc_sum = new TH1D("adc_sum", "summed ADC spectrum", 500, 0, 5000);
  TH1D* adc_sum_tight = new TH1D("adc_sum_tight", "summed ADC spectrum, y= 10-20cm", 500, 0, 5000);

  TProfile* adcpp1;
  TProfile* adcpp2;
  TProfile* adcmp1;
  TProfile* adcmp2;
  TProfile* fulladcpp;
  TProfile* fulladcmp;
  TProfile* x1adcpp;
  TProfile* x1adcmp;
  TProfile* x2adcpp;
  TProfile* x2adcmp;
  TProfile* x3adcpp;
  TProfile* xdistribution;
  TProfile* ydistribution;


  adcpp1 = new TProfile("ADCPP1", Form("Run %d  Oct. %d +PMT far end", run_number, md_number), 50, -100, 0);
  adcpp2 = new TProfile("ADCPP2", Form("Run %d  Oct. %d +PMT near end", run_number, md_number), 50, 0, 100);
  adcmp1 = new TProfile("ADCMP1", Form("Run %d  Oct. %d -PMT near end", run_number, md_number), 50, -100, 0);
  adcmp2 = new TProfile("ADCMP2", Form("Run %d  Oct. %d -PMT far end", run_number, md_number), 50, 0, 100);
  fulladcpp = new TProfile("FULLADCPP", Form("Linear Fit Run %d Octant %d positive PMT", run_number, md_number), 150, -105, 105);
  fulladcmp = new TProfile("FULLADCMP", Form("LInear Fit Run %d Octant %d minus PMT", run_number, md_number), 150, -105, 105);
  x1adcpp = new TProfile("X1P", "X1 ADCP profile", 200, -110, 110);
  x1adcmp = new TProfile("X1M", "X1 ADCM profile", 200, -110, 110);
  x2adcpp = new TProfile("X2P", "X2 ADCP profile", 200, -110, 110);
  x2adcmp = new TProfile("X2M", "X2 ADCM profile", 200, -110, 110);
  x3adcpp = new TProfile("X3P", "X3 ADCP profile", 200, -110, 110);
  x3adcmp = new TProfile("X3M", "X3 ADCM profile", 200, -110, 110);
  xdistribution = new TProfile("xdistribution","total ADC pulse height vs. x", 200, 300, 360);
  ydistribution = new TProfile("ydistribution","total ADC pulse height vs. y", 200, -110, 110);

  TH2F* hits = new TH2F("hits", Form("Run %d Octant %d Hit Chart", run_number, md_number), 400, -130, 130, 300, 320, 360);
  TH2F* misses = new TH2F("misses", Form("Run %d Octant %d Miss Chart", run_number, md_number), 400, -130, 130, 300, 320, 360);

  TGraph* effplot;

  TBranch* branch_event=event_tree->GetBranch("events");
  TBranch* branch     = event_tree->GetBranch("maindet");

  branch_event->SetAddress(&fEvent);

  TLeaf* mdp=branch->GetLeaf(Form("md%dp_adc",md_number));
  TLeaf* mdm=branch->GetLeaf(Form("md%dm_adc",md_number));

  TLeaf* mdp_t=branch->GetLeaf(Form("md%dp_f1",md_number));
  TLeaf* mdm_t=branch->GetLeaf(Form("md%dm_f1",md_number));


  Int_t nevents=event_tree->GetEntries();
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


  for (int i=0;i<10000;i++)
    {

      branch_event->GetEntry(i);
      branch->GetEntry(i);

      for(int num_p=0; num_p < fEvent->GetNumberOfPartialTracks(); num_p++)
	{
	  Double_t pdataplus = mdp_t->GetValue();
	  Double_t pdataminus = mdm_t->GetValue();
	  Double_t padcp = mdp->GetValue();
	  Double_t padcm = mdm->GetValue();

	  if(pdataplus == 0 && pdataminus == 0)
	    {
	      adcpped->Fill(padcp);
	      adcmped->Fill(padcm);
	    }
	}
    }

  /*
  Int_t size = 125;
  Double_t hit[125];
  Double_t trial[125];
  Int_t trial_total=0;
  Int_t bin;
  Int_t xmin = 323;
  Double_t width = 0.2;
  Double_t xpos[125];
  Double_t efficiency[125];
  */

  Double_t ystep = 10.0;     // bin size in cm for y (long axis of bar)
  Double_t xstep = 2.0;      // bin size in cm for x (width of bar)
  Double_t xslopestep = 0.010;  // bin size in rad for xslope (radial)
  Double_t yslopestep = 0.015;  // bin size in rad for yslope (azimuthal)
  Double_t ylow = -100.0;   // lowest edge of fiducial in y
  Double_t xlow = 320.0;   // lowest edge of fiducial in x
  Double_t yslopelow = -0.15;   // lowest edge of fiducial in yslope
  Double_t xslopelow = 0.34;   // lowest edge of fiducial in xslope

  Double_t adcpmean = adcpped->GetMean();
  Double_t adcmmean = adcmped->GetMean();

  Int_t n_out_of_range = 0;
  Int_t n_filled = 0;
  Int_t n_tracks_used = 0;

  for(int i = 0; i < max_events; i++)
	{

	  //	  cout << " i = " << i << endl; 

	  if(i % 100000 == 0) cout <<"events processed so far:" << i << endl;
	  branch_event->GetEntry(i);
	  branch->GetEntry(i);

	  Double_t xoffset, yoffset, xslope, yslope, x, y;
	  //    weight = 0;

          double ntracks= fEvent->GetNumberOfTracks();
          if (ntracks>0){
	      track=fEvent->GetTrack ( 0 );
	      pt = track->fBack;
	   if ( pt->GetRegion() ==3 && pt->GetPackage()==package)
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
		//	cout << "yslope = " << yslope << "  isy = " << isy << "  out of range =  "  << out_of_range << endl;
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
  cout <<  " events enalyzed = " << max_events << endl;

  return;
};


