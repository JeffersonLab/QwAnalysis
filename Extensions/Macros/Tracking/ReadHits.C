// Name  :   ReadHits.C
// Author:   Jeong Han Lee
// email :   jhlee@jlab.org
// Date:     Wed Jan  6 14:46:20 EST 2010
// Version:  0.0.8
//
//  An example script that uses a "generic USER way" in order to read 
//  some useful information from "hit-based" ROOT output file created 
//  by the QwAnalysis.
//  
//  If one wants to modify this script, please copy this to create a
//  new one and modifiy the created one. After it, please commit
//  the script into the Extensions/Macro/Tracking directory




//
// History:
// 0.0.0     2009/11/04   created
//                        * add : load_libraries(), read(), and read_event_number()
//
// 0.0.1     2009/11/11   * introduce a "premature" plot_time_histogram_per_tdc()
//                          to access a single information from ROOT file
//
// 0.0.2     2009/11/12   * add : "if condition" to distingush whether the hits are
//                                or not when the region 3 (W&M) TDC data
//                        * add : extract the histogram range of "Traw" 
// 0.0.3     2009/11/17   * Histogram of the time value of all hits on a given
//                          TDC channel and wire number
// 0.0.4     2009/12/25   * Histogram of the number of events with a given wire 
//                          having a hit, plotted vs. wire (region, direction,
//                          not package)
// 0.0.5     2009/12/27   * introduce "TSuperCanvas", see "TSuperCanvas.cc" for detail
//                          "m" for zoom, "g" for grid, and "l" for log scale
//                          when a pad is selected.
// 0.0.6     2009/12/29   * consider "direction"s in the QwHit data
//                          Thus, we can seperate two different planes in one tracking
//                          subsystem. But why does Region 2 has U and V planes?
// 0.0.7     2010/01/02   * 2D Histogram of the number of wires with hits, per event
//                        * move "lib.sh" into this script by using a quick and dirty
//                          method.
// 0.0.8     2010/01/06   * replace "direction" with "plane" in oder to get reasonable
//                          plots for Region 2
//                        * Region 3 :
//                                     direction 3 (U) == plane 1
//                                     direction 4 (V) == plane 2
//
//                        * Region 2 :
//                                     direction 1 (X,X') == plane 1,4,7,10
//                                     direction 3 (U,U') == plane 2,5,8,11
//                                     direction 4 (V,V;) == plane 3,6,9,12





//
// *) Run 1567 (region 3) and Run 398 (region 2) have only one package
//    If one use this script to test real tracking subsystem,
//    one must consider the package number.
//
// *) If one meets some errors, please restart the script. 





//////////////////////////////////////////////////////////////////////
//
//                             Examples
//
//////////////////////////////////////////////////////////////////////
//
//   cd QwAnalysis_new
//   make distclean
//   make
//   qwtracking -r 398
//   qwtracking -r 1567

//   cd QwAnalysis_new/Extensions/Macros/Tracking/
//   root -l

//   root [0] .L ReadHits.C
//   root [1] read(12)
//   root [2] plot_time_histogram_per_tdc()
//   root [3] plot_time_histogram_per_tdc(1567, 10, 0, 10000)
//   root [ ] plot_nevent_per_wire(398,0,1000)
//   root [ ] plot_nevent_per_wire(1567,0,1000)
//   root [ ] plot_nwire_hits_per_event(1567,0,100)


// Here I use the following defintions:
// event id  == event number ==  HitContainer id
// each event has several hits


gROOT->Reset();


const Int_t BUFFERSIZE  = 1000;      // BufferSize of TH1 for automatic range of histogram
const Int_t BINNUMBER   = 1024;      // Default bin number (not yet done automatically)
const Int_t WIRENUMREG1 = 30;
const Int_t WIRENUMREG2 = 30;
const Int_t WIRENUMREG3 = 290;

const Char_t BRANCHNAME[5] = "hits"; // Must be the same as the branch name in QwTracking.cc
                                     // or a main source which is used to create a ROOT file,
                                     // which one try to open here.

const Short_t LIBNUM          = 3;
const Char_t* LIBNAME[LIBNUM] = 
  {
      "QwHit"
    , "QwHitContainer"
    , "QwHitRootContainer"
//     , "QwEvent"
//     , "QwEventHeader"
//     , "QwTrackingTreeLine"
//     , "VQwTrackingElement"
//     , "QwPartialTrack"
  };


void
check_libraries()
{

  gROOT -> ProcessLine( Form(".L %s/Extensions/Macros/Tracking/TSuperCanvas.cc+", gSystem->Getenv("QWANALYSIS")) );

  for(Short_t i=0; i<LIBNUM; i++)
    {
      if( !TClassTable::GetDict(Form("%s", LIBNAME[i])) )
	{
	  gSystem -> Exec( Form("gcc -shared -Wl,-soname,./lib%s.so -o lib%s.so %s/Tracking/src/%s.o %s/Tracking/dictionary/%sDict.o",  
				LIBNAME[i], LIBNAME[i], gSystem->Getenv("QWANALYSIS"), LIBNAME[i],  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
	  gSystem -> Load( Form("%s/Extensions/Macros/Tracking/lib%s.so",  gSystem->Getenv("QWANALYSIS"), LIBNAME[i]) );
	}
    }

  return;
};


// // This function will be used to calculate or find an automatic bin number of TH1.
// void
// IQR(TH1D &histo)
// {
//   printf("-------------\n");
//   Double_t x[2] = {0.25, 0.75};
//   Double_t y[2] = {0.0};

//   histo.GetQuantiles(2,y,x);

//   printf("%f%f\n", y[0], y[1]);

// };


void 
read(Int_t evID=1, Int_t hitID = -1, Int_t run_number=1567)
{

  check_libraries();
  //  IQR();


  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number));

  QwHitRootContainer* hitContainer = 0;
  Int_t nevent = 0;
  TTree* tree  = (TTree*) file.Get("tree");
  tree->SetBranchAddress(BRANCHNAME, &hitContainer);
  nevent  = tree->GetEntries();
  
  if(evID > nevent) 
    {
      evID = nevent-1;
      printf("You selected the event number which is out of range.\n");
      printf("Thus, the maximum event number %d is selected.\n", evID);
    }
  
  //  access one event (hitContainer) in terms of id (event number)
  tree -> GetEntry(evID);
  //  total number is started from 0
  printf("*** Event %10d\n", evID);
  printf("    --- %d hits (by QwHitRootContainer)\n", hitContainer->GetSize()); 
  
  //  access all hits in a specific event id ( GetEntry(id) )
  TClonesArray *hits_per_event = (TClonesArray*) hitContainer->GetHits();
  
  //one can also access how many hits are in there
  Int_t hitN      = 0;
  Int_t hit_begin = 0;
  Int_t hit_end   = 0;

  hitN = hits_per_event  -> GetEntries();
  printf("    --- %d hits (by TCloneArray)\n", hitN); 

  if(hitN == 0) 
    {
      printf(" *** no hit in the event number %d\n", evID);
    }
  else 
    {
      
      QwHit *hit = 0;
      if(hitID > hitN) 
	{
	  hitID = hitN - 1;
	  printf("    --- You selected the hit number which is out of range.\n");
	  printf("    --- Thus, the maximum hit number %d is selected.\n", hitID);
	}
      
      if( hitID == -1 ) 
	{
	  hit_begin = 0;
	  hit_end   = hitN;
	}
      else
	{
	  hit_begin = hitID;
	  hit_end   = hitID +1;
	}
      for(Int_t i=hit_begin; i< hit_end; i++)
	{
	  hit = (QwHit *) hits_per_event->At(i);
	  // At() : http://root.cern.ch/root/html/src/TObjArray.h.html#R3YkqE
	  // 
	  // All functions are defined in QwHit.h
	  printf("    Hit %d                           \n", i);
	  printf("    -------------------------------- \n"); 
	  printf("    --- Subbank       %6d \n",  hit->GetSubbankID()); 
	  printf("    --- Module        %6d \n",  hit->GetModule()); 
	  printf("    --- Channel       %6d \n",  hit->GetChannel()); 
	  printf("    --- Region        %6d \n",  hit->GetRegion()); 
	  printf("    --- Package       %6d \n",  hit->GetPackage()); 
	  printf("    --- Direction     %6d \n",  hit->GetDirection()); 
	  printf("    -------------------------------- \n"); 
	  printf("    --- DriftDistance %14.2f \n", hit->GetDriftDistance());
	  printf("    --- RawTime       %14.2f \n", hit->GetRawTime());
	  printf("    --- Time          %14.2f \n", hit->GetTime());
	  printf("    --- HitNumber     %14d   \n", hit->GetHitNumber());
	  printf("    --- HitNumberR    %14d   \n", hit->GetHitNumberR());
	  printf("    -------------------------------- \n"); 
	}
      
      // hit = (QwHit *) hits_per_event->UncheckedAt(hitID);
      //// UncheckedAt() : http://root.cern.ch/root/html/src/TObjArray.h.html#KyjP6
      //// All functions are defined in QwHit.h
      //   printf("    -------------------------------- \n"); 
      //   printf("    --- Subbank       %6d \n",  hit->GetSubbankID()); 
      //   printf("    --- Module        %6d \n",  hit->GetModule()); 
      //   printf("    --- Channel       %6d \n",  hit->GetChannel()); 
      //   printf("    --- Region        %6d \n",  hit->GetRegion()); 
      //   printf("    --- Package       %6d \n",  hit->GetPackage()); 
      //   printf("    --- Direction     %6d \n", hit->GetDirection()); 
      //   printf("    -------------------------------- \n"); 
      //   printf("    --- DriftDistance %14.2f \n", hit->GetDriftDistance());
      //   printf("    --- RawTime       %14.2f \n", hit->GetRawTime());
      //   printf("    --- Time          %14.2f \n", hit->GetTime());
      //   printf("    --- HitNumber     %14d   \n", hit->GetHitNumber());
      //   printf("    --- HitNumberR    %14d   \n", hit->GetHitNumberR());
      //   printf("    -------------------------------- \n"); 
    } 
  hitContainer->Clear();
  delete hitContainer; hitContainer = 0;
  
  file.Close();
};

void 
read_event_number(int id=1, Int_t run_number=1567)
{

  check_libraries();

  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  
  TTree* tree = (TTree*) file.Get("tree");
  QwHitRootContainer* hitContainer = 0;
  tree->SetBranchAddress(BRANCHNAME, &hitContainer);


  Int_t nevent  = tree->GetEntries();
  tree -> GetEntry(id);
  printf("Event : %d, Hits %d\n", id, hitContainer->GetSize());
  
  
  
  
  hitContainer->Clear();
  delete hitContainer; hitContainer = NULL;
  file.Close();
};



typedef struct {
  Double_t min;
  Double_t max;
} range;



void 
histogram_range_per_tdc(Int_t run_number=1567, 
			Bool_t debug=false, 
			Int_t tdc_chan=0)
{
  check_libraries();

  TStopwatch timer;
  timer.Start();


  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  if (file.IsZombie()) 
    {
      printf("Error opening file\n");  
      exit(-1);
    }
  else
    {
      TTree* tree = (TTree*) file.Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress(BRANCHNAME,&hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
      Double_t tdc_rawtime = 0.0;
      QwHit    *hit        = NULL;
      
      nevent = tree -> GetEntries();
      printf("Run %d TDC channel %d total event %d\n", run_number, tdc_chan,  nevent);
   
      
      range h_range = {0.0, 0.0};
      
      Int_t ev_i = 0; 
      Int_t hit_i = 0;

      for(ev_i=0; ev_i<nevent; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitContainer->GetHit(hit_i);
	      if( hit->GetChannel() == tdc_chan ) 
		{
		  tdc_rawtime = hit->GetRawTime();
		  h_range.min = (h_range.min < tdc_rawtime) ? h_range.min : tdc_rawtime;
		  h_range.max = (tdc_rawtime < h_range.max) ? h_range.max : tdc_rawtime;
		  if(debug)
		    {
		      printf("TDC Chan %2d (evN,hitN) (%10d,%2d) Traw %16.2f\r", 
			     tdc_chan, ev_i,hit_i, tdc_rawtime);
		    }
		}
	    }
	  
	  hitContainer->Clear();
	}
      
      printf("T raw range is [%12.2lf, %12.2lf]\n", h_range.min, h_range.max);
    }

  file.Close();
  timer.Stop();

  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

  //    range h_range = {0.0, 0.0};
  // T raw range is [        0.00.     61070.00] with 398  (region2)
  // T raw range is [        0.00,      1703.00] with 1567 (region3)

};


// --------------------------------------------------------------------------------
// Histogram of the time value of all hits that are assigned to a given TDC channel
// (tdc_chan) for events  (event_begin:event_end) in a given run (run_number).
// --------------------------------------------------------------------------------
// valid for region 2 and region 3, not sure region 1 

//
void 
plot_time_histogram_per_tdc(Int_t run_number=1567, 
			    Int_t tdc_chan=0, 
			    UInt_t event_begin=0, 
			    UInt_t event_end=0, 
			    Int_t smooth_n=1)
{
  
  if( (event_begin > event_end) )
    {
      printf("You selected the wrong range of the event\n");
      return;
    }
  if ( (event_begin == event_end) && (event_begin != 0))
    {
      event_begin = 0;
      printf("The event range is not allowed\n");
    }

  Bool_t debug =false;

  check_libraries();
  TStopwatch timer;
  timer.Start();

  TSuperCanvas *time_per_tdc_canvas = new TSuperCanvas("time_per_tdc_canvas","time per tdc channel",10,10,1200,800);
    
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number));
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete time_per_tdc_canvas;time_per_tdc_canvas = NULL;
      return;
    }
  else
    {
      TTree* tree = (TTree*) file->Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress(BRANCHNAME,&hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
  
      QwHit    *hit        = NULL;
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;

      TH1D *histo_region1[6]  = {NULL};
      TH1D *histo_region2[12] = {NULL};
      TH1D *histo_region3[2]  = {NULL};
        
      Double_t tdc_time    = 0.0;
      Short_t  region      = 0;
      Short_t  package     = 0;
      Short_t  direction   = 0;
      Short_t  plane       = 0;

      nevent = tree -> GetEntries();
      printf("Run %d TDC channel %d total event %d\n", run_number, tdc_chan,  nevent);
      
      for(Short_t i=0; i<3; i++)
	{
 	  region          = i+1;
	  
	  if( region==1 ) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  histo_region1[j]=  new TH1D(Form("RG%dDIR%d", region, direction), 
					      Form("Time per CH %d  RG %d DIR %d", tdc_chan, region, direction), 
					      BINNUMBER, 0, 0); 
		  histo_region1[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==2 )
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1; 
		  histo_region2[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Time per CH %d  RG %d PL %d", tdc_chan, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region2[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==3 ) 
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1; 
		  histo_region3[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Time per CH %d  RG %d PL %d", tdc_chan, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region3[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	}
      
      Bool_t region_status[4]    = {true, 
				    false, false, false};
      Bool_t direction_status[7] = {true, 
				    false, false, false, false, false, false};
      Bool_t plane_status[13]    = {true, 
				    false, false, false, false, false, false, 
				    false, false, false, false, false, false};
      
      if(event_end == 0) event_end = nevent;
      
      printf("You selected the events with [%d,%d]\n", event_begin, event_end);
      
      for(ev_i=event_begin; ev_i<event_end; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitContainer->GetHit(hit_i);
	      
	      if( hit->GetChannel() == tdc_chan ) 
		{
		  tdc_time  = hit->GetTime();
		  region    = (Short_t) hit->GetRegion();
		  package   = (Short_t) hit->GetPackage();
		  direction = (Short_t) hit->GetDirection();
		  plane     = (Short_t) hit->GetPlane();
		  
		  if(!region_status[region])       region_status[region]       = true;
		  if(!direction_status[direction]) direction_status[direction] = true;
		  if(!plane_status[plane])         plane_status[plane]         = true;
		  
		  if(ev_i%1000==0) 
		    printf("Region %1d, evID %12d package %1d, direction %1d plane %2d tdc chan %4d : time %4.2e\n", 
			   region, ev_i, package, direction, plane, tdc_chan, tdc_time);
		  if(region == 1) histo_region1[direction-1] -> Fill(tdc_time);
		  if(region == 2) histo_region2[plane-1]     -> Fill(tdc_time);
		  if(region == 3) histo_region3[plane-1]     -> Fill(tdc_time);
		}
	    }
	  
	  hitContainer->Clear();
	}
      
      time_per_tdc_canvas -> Divide(6,4);  
      
      
      TLatex *region_tex = NULL;
      region_tex = new TLatex();
      region_tex -> SetTextSize(0.05);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);
      

      Short_t padnumber = 0;

      region = 1;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<6; j++)
	    {
	      direction = j+1;
	      padnumber = direction;
	      time_per_tdc_canvas -> cd(padnumber);
	      
	      if( direction_status[direction] ) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region1[direction-1] -> SetLineColor(kRed);
		  histo_region1[direction-1] -> GetXaxis()-> SetTitle("Time");
		  histo_region1[direction-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region1[direction-1] -> Smooth(smooth_n);
		  histo_region1[direction-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d DIR %1d : NO data", region, direction));
		}
	    }
	}

      
      region = 2;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<12; j++)
	    {
	      plane     = j+1;
	      padnumber = 6 + plane;
	      time_per_tdc_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  // 		  gStyle -> SetOptStat(0);
		  histo_region2[plane-1] -> SetLineColor(kRed);
		  histo_region2[plane-1] -> GetXaxis()-> SetTitle("Time");
		  histo_region2[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region2[plane-1] -> Smooth(smooth_n);
		  histo_region2[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	    }
	}


      region = 3;
      if( region_status[region] )
	{
	  for(Short_t j=0; j<2; j++)
	    {
	      plane     = j+1;
	      padnumber = 18 + plane;
	      //	      printf("region 3 pad %d \n", padnumber);
	      time_per_tdc_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region3[plane-1] -> SetLineColor(kRed);
		  histo_region3[plane-1] -> GetXaxis()-> SetTitle("Time");
		  histo_region3[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region3[plane-1] -> Smooth(smooth_n);
		  histo_region3[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	      
	    }
	}

      //     file->Close();
      // if one cloese the file in the ROOT script, there are nothing on the canvas.
      timer.Stop();
      printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );
      
    }
};

 

// --------------------------------------------------------------------------------
// Histogram of the raw time value of all hits that are assigned to a given TDC channel
// (tdc_chan) for events  (event_begin:event_end) in a given run (run_number).
// --------------------------------------------------------------------------------
// valid for region 2 and region 3, not sure region 1 

//
void 
plot_rawtime_histogram_per_tdc(Int_t run_number=1567, 
			       Int_t tdc_chan=0, 
			       UInt_t event_begin=0, 
			       UInt_t event_end=0, 
			       Int_t smooth_n=1)
{
  
  if( (event_begin > event_end) )
    {
      printf("You selected the wrong range of the event\n");
      return;
    }
  if ( (event_begin == event_end) && (event_begin != 0))
    {
      event_begin = 0;
      printf("The event range is not allowed\n");
    }

  Bool_t debug =false;

  check_libraries();
  TStopwatch timer;
  timer.Start();

  TSuperCanvas *rawtime_per_tdc_canvas = new TSuperCanvas("rawtime_per_tdc_canvas","time per tdc channel",10,10,1200,800);
    
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number));
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete rawtime_per_tdc_canvas;rawtime_per_tdc_canvas = NULL;
      return;
    }
  else
    {
      TTree* tree = (TTree*) file->Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress(BRANCHNAME,&hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
  
      QwHit    *hit        = NULL;
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;

      TH1D *histo_region1[6]  = {NULL};
      TH1D *histo_region2[12] = {NULL};
      TH1D *histo_region3[2]  = {NULL};
        
      Double_t tdc_rawtime = 0.0;
      Short_t  region      = 0;
      Short_t  package     = 0;
      Short_t  direction   = 0;
      Short_t  plane       = 0;

      nevent = tree -> GetEntries();
      printf("Run %d TDC channel %d total event %d\n", run_number, tdc_chan,  nevent);
      
   //    for(Short_t i=0; i<3; i++)
// 	{
//  	  region          = i+1;
// 	  for(Short_t j=0; j<7; j++)
// 	    {
// 	      direction = j+1;
// 	      histo_region[i][j] = new TH1D(Form("Region%dNEvt%d", region, j), 
// 					    Form("Time Raw Histogram per ch %d, Re %d Dir %d", tdc_chan,region, direction), 
// 					    BINNUMBER, 0, 0); 
// 	      histo_region[i][j] -> SetDefaultBufferSize(BUFFERSIZE);

// 	    }
// 	}

     for(Short_t i=0; i<3; i++)
	{
 	  region          = i+1;

	  if( region==1 ) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  histo_region1[j]=  new TH1D(Form("RG%dDIR%d", region, direction), 
					      Form("Raw Time per CH %d  RG %d DIR %d", tdc_chan, region, direction), 
					      BINNUMBER, 0, 0); 
		  histo_region1[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==2 )
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1; 
		  histo_region2[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Raw Time per CH %d  RG %d PL %d", tdc_chan, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region2[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==3 ) 
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1; 
		  histo_region3[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Raw Time per CH %d  RG %d PL %d", tdc_chan, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region3[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	}




      Bool_t region_status[4]    = {true, 
				    false, false, false};
      Bool_t direction_status[7] = {true, 
				    false, false, false, false, false, false};
      Bool_t plane_status[13]    = {true, 
				    false, false, false, false, false, false, 
				    false, false, false, false, false, false};

      if(event_end == 0) event_end = nevent;
      
      printf("You selected the events with [%d,%d]\n", event_begin, event_end);
      
      for(ev_i=event_begin; ev_i<event_end; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitContainer->GetHit(hit_i);
	      
	      if( hit->GetChannel() == tdc_chan ) 
		{
		  tdc_rawtime  = hit->GetRawTime();
		  region       = (Short_t) hit->GetRegion();
		  package      = (Short_t) hit->GetPackage();
		  direction    = (Short_t) hit->GetDirection();
		  plane        = (Short_t) hit->GetPlane();

		  if(!region_status[region])       region_status[region]       = true;
		  if(!direction_status[direction]) direction_status[direction] = true;
		  if(!plane_status[plane])         plane_status[plane]         = true;
		  
		  if(ev_i%1000==0) 
		    printf("Region %1d, evID %12d package %1d, direction %1d plane %2d tdc chan %4d : time %4.2e\n", 
			   region, ev_i, package, direction, plane, tdc_chan, tdc_rawtime);
		  if(region == 1) histo_region1[direction-1] -> Fill(tdc_rawtime);
		  if(region == 2) histo_region2[plane-1]     -> Fill(tdc_rawtime);
		  if(region == 3) histo_region3[plane-1]     -> Fill(tdc_rawtime);
		}

// 	      if( hit->GetChannel() == tdc_chan ) 
// 		{
// 		  tdc_rawtime = hit->GetRawTime();
// 		  region    = (Short_t) hit->GetRegion();
// 		  package   = (Short_t) hit->GetPackage();
// 		  direction = (Short_t) hit->GetDirection();
// 		  if(!region_status[region]) region_status[region] = true;
// 		  if(!direction_status[direction]) direction_status[direction] = true;
// 		  if(ev_i%100==0) printf("evID %d region %d, package %d, direction %d, Traw %d\n", 
// 				       ev_i, region, package, direction, tdc_rawtime);
// 		  histo_region[region-1][direction-1] -> Fill(tdc_rawtime);
// 		}
	    }
	  
	  hitContainer->Clear();
	}
      
      rawtime_per_tdc_canvas -> Divide(6,4);  
      
      
      TLatex *region_tex = NULL;
      region_tex = new TLatex();
      region_tex -> SetTextSize(0.05);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);
      

      Short_t padnumber = 0;

      region = 1;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<6; j++)
	    {
	      direction = j+1;
	      padnumber = direction;
	      rawtime_per_tdc_canvas -> cd(padnumber);
	      
	      if( direction_status[direction] ) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region1[direction-1] -> SetLineColor(kRed);
		  histo_region1[direction-1] -> GetXaxis()-> SetTitle("Raw Time");
		  histo_region1[direction-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region1[direction-1] -> Smooth(smooth_n);
		  histo_region1[direction-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d DIR %1d : NO data", region, direction));
		}
	    }
	}

      
      region = 2;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<12; j++)
	    {
	      plane     = j+1;
	      padnumber = 6 + plane;
	      rawtime_per_tdc_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  // 		  gStyle -> SetOptStat(0);
		  histo_region2[plane-1] -> SetLineColor(kRed);
		  histo_region2[plane-1] -> GetXaxis()-> SetTitle("Raw Time");
		  histo_region2[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region2[plane-1] -> Smooth(smooth_n);
		  histo_region2[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	    }
	}


      region = 3;
      if( region_status[region] )
	{
	  for(Short_t j=0; j<2; j++)
	    {
	      plane     = j+1;
	      padnumber = 18 + plane;
	      //	      printf("region 3 pad %d \n", padnumber);
	      rawtime_per_tdc_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region3[plane-1] -> SetLineColor(kRed);
		  histo_region3[plane-1] -> GetXaxis()-> SetTitle("Raw Time");
		  histo_region3[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region3[plane-1] -> Smooth(smooth_n);
		  histo_region3[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	      
	    }
	}



//       for(Short_t i=0; i<3; i++) 
// 	{
// 	  region = i+1;
// 	  for(Short_t j=0; j<6; j++)
// 	    {
// 	      direction = j+1;
// 	      padnumber = 6*(region-1) + direction;
// 	      rawtime_per_tdc_canvas -> cd(padnumber);
// 	  //	  gPad -> SetLogy();
// 	      if(region_status[region] && direction_status[direction]) 
// 		{
// 		  histo_region[i][j] -> SetLineColor(kRed);
// 		  histo_region[i][j] -> GetXaxis()-> SetTitle("Time");
// 		  histo_region[i][j] -> GetYaxis()-> SetTitle("Number of measurements");
// 		  histo_region[i][j] -> GetXaxis()-> CenterTitle();
// 		  histo_region[i][j] -> GetYaxis()-> CenterTitle();
// 		  histo_region[i][j] -> Smooth(smooth_n);
// 		  histo_region[i][j] -> Draw();
// 		}
// 	      else
// 		{
// 		  region_tex -> DrawLatex(0.5,0.5,Form("Region %d direction %d has no data", region, direction));
// 		}
// 	    }
// 	}
      
      //     file->Close();
      // if one cloese the file in the ROOT script, there are nothing on the canvas.
      timer.Stop();
      printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );
      
    }
};

 

// --------------------------------------------------------------------------------
// Histogram of the time value of all hits that are assigned to a given drift 
// chammber wire (wire) for events  (event_begin:event_end) in a given run (run_number).
// --------------------------------------------------------------------------------
// valid for region 2 and region 3, not sure for region 1
void 
plot_time_histogram_per_wire(Int_t run_number=1567, 
			     Int_t wire=64, 
			     UInt_t event_begin=0, 
			     UInt_t event_end=0, 
			     Int_t smooth_n=1)
{

  if( (event_begin > event_end) )
    {
      printf("You selected the wrong range of the event\n");
      return;
    }
  if ( (event_begin == event_end) && (event_begin != 0))
    {
      event_begin = 0;
      printf("The event range is not allowed\n");
    }

  Bool_t debug =false;

  check_libraries();
  TStopwatch timer;
  timer.Start();

  TSuperCanvas *time_per_wire_canvas = new TSuperCanvas("time_per_wire_canvas","time per wire",10,10,1200,800);
    
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));

  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete time_per_wire_canvas; time_per_wire_canvas = NULL;
      return;
    }
  else
    {
      TTree* tree = (TTree*) file->Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress(BRANCHNAME, &hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
      QwHit    *hit        = NULL;
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;   

      TH1D *histo_region1[6]  = {NULL};
      TH1D *histo_region2[12] = {NULL};
      TH1D *histo_region3[2]  = {NULL};

      Double_t tdc_time   = 0.0;
      Short_t  region     = 0;
      Short_t  package    = 0;
      Short_t  direction  = 0;
      Short_t  plane      = 0;

      nevent = tree -> GetEntries();
      printf("Run %d Wire number  %d total event %d\n", run_number, wire,  nevent);
      
      for(Short_t i=0; i<3; i++)
	{
 	  region          = i+1;

	  if( region==1 ) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  histo_region1[j]=  new TH1D(Form("RG%dDIR%d", region, direction), 
					      Form("Raw Time per Wire %d  RG %d DIR %d", wire, region, direction), 
					      BINNUMBER, 0, 0); 
		  histo_region1[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==2 )
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1; 
		  histo_region2[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
				      Form("Raw Time per Wire %d  RG %d PL %d", wire, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region2[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==3 ) 
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1; 
		  histo_region3[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Raw Time per Wire %d  RG %d PL %d", wire, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region3[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	}


      Bool_t region_status[4]    = {true, 
				    false, false, false};
      Bool_t direction_status[7] = {true, 
				    false, false, false, false, false, false};
      Bool_t plane_status[13]    = {true, 
				    false, false, false, false, false, false, 
				    false, false, false, false, false, false};

      if(event_end == 0) event_end = nevent;
      
      printf("You selected the events with [%d,%d]\n", event_begin, event_end);

      for(ev_i=event_begin; ev_i<event_end; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitContainer->GetHit(hit_i);

	      if( hit->GetElement() == wire ) 
		{
		  tdc_time  = hit->GetTime();
		  region    = (Short_t) hit->GetRegion();
		  package   = (Short_t) hit->GetPackage();
		  direction = (Short_t) hit->GetDirection();
		  plane     = (Short_t) hit->GetPlane();

		  if(!region_status[region])       region_status[region]       = true;
		  if(!direction_status[direction]) direction_status[direction] = true;
		  if(!plane_status[plane])         plane_status[plane]         = true;
		  
		  if(ev_i%1000==0) 
		    printf("Region %1d, evID %12d package %1d, direction %1d plane %2d wire %4d :  time %4.2e\n", 
			   region, ev_i, package, direction, plane, wire, tdc_time);
		  if(region == 1) histo_region1[direction-1] -> Fill(tdc_time);
		  if(region == 2) histo_region2[plane-1]     -> Fill(tdc_time);
		  if(region == 3) histo_region3[plane-1]     -> Fill(tdc_time);
		}
	    }
	  
	  hitContainer->Clear();
	}

      time_per_wire_canvas -> Divide(6,4);  
      
      TLatex *region_tex = NULL;
      region_tex = new TLatex();
      region_tex -> SetTextSize(0.05);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);

      Short_t padnumber = 0;



      region = 1;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<6; j++)
	    {
	      direction = j+1;
	      padnumber = direction;
	      time_per_wire_canvas -> cd(padnumber);
	      
	      if( direction_status[direction] ) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region1[direction-1] -> SetLineColor(kRed);
		  histo_region1[direction-1] -> GetXaxis()-> SetTitle("Time");
		  histo_region1[direction-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region1[direction-1] -> Smooth(smooth_n);
		  histo_region1[direction-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d DIR %1d : NO data", region, direction));
		}
	    }
	}

      
      region = 2;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<12; j++)
	    {
	      plane     = j+1;
	      padnumber = 6 + plane;
	      time_per_wire_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  // 		  gStyle -> SetOptStat(0);
		  histo_region2[plane-1] -> SetLineColor(kRed);
		  histo_region2[plane-1] -> GetXaxis()-> SetTitle("Time");
		  histo_region2[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region2[plane-1] -> Smooth(smooth_n);
		  histo_region2[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	    }
	}


      region = 3;
      if( region_status[region] )
	{
	  for(Short_t j=0; j<2; j++)
	    {
	      plane     = j+1;
	      padnumber = 18 + plane;
	      //	      printf("region 3 pad %d \n", padnumber);
	      time_per_wire_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region3[plane-1] -> SetLineColor(kRed);
		  histo_region3[plane-1] -> GetXaxis()-> SetTitle("Time");
		  histo_region3[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region3[plane-1] -> Smooth(smooth_n);
		  histo_region3[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	      
	    }
	}


// 	  for(Short_t j=0; j<6; j++)
// 	    {
// 	      direction = j+1;
// 	      padnumber = 6*(region-1) + direction;
// 	      time_per_wire_canvas -> cd(padnumber);
// 	      //	  gPad -> SetLogy();
// 	      if(region_status[region] && direction_status[direction]) 
// 		{
// 		  histo_region[i][j] -> SetLineColor(kRed);
// 		  histo_region[i][j] -> GetXaxis()-> SetTitle("Time");
// 		  histo_region[i][j] -> GetYaxis()-> SetTitle("Number of measurements");
// 		  histo_region[i][j] -> GetXaxis()-> CenterTitle();
// 		  histo_region[i][j] -> GetYaxis()-> CenterTitle();
// 		  histo_region[i][j] -> Smooth(smooth_n);
// 		  histo_region[i][j] -> Draw();
// 		}
// 	      else
// 		{
// 		  region_tex -> DrawLatex(0.5,0.5,Form("Region %d direction %d has no data", region, direction));
// 		}
// 	    }
	


    }
  //     file->Close();
  // if one cloese the file in the ROOT script, there are nothing on the canvas.
  timer.Stop();
  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

      
};



// --------------------------------------------------------------------------------
// Histogram of the raw time value of all hits that are assigned to a given drift 
// chammber wire (wire) for events  (event_begin:event_end) in a given run (run_number).
// --------------------------------------------------------------------------------
// valid for region 2 and region 3, not sure for region 1
void 
plot_rawtime_histogram_per_wire(Int_t run_number=1567, 
				Int_t wire=64, 
				UInt_t event_begin=0, 
				UInt_t event_end=0, 
				Int_t smooth_n=1)
{
  
  if( (event_begin > event_end) )
    {
      printf("You selected the wrong range of the event\n");
      return;
    }
  if ( (event_begin == event_end) && (event_begin != 0))
    {
      event_begin = 0;
      printf("The event range is not allowed\n");
    }
  Bool_t debug =false;
  
  check_libraries();
  TStopwatch timer;
  timer.Start();
  
  TSuperCanvas *rawtime_per_wire_canvas = new TSuperCanvas("rawtime_per_wire_canvas","time per wire",10,10,1200,800);
  
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete rawtime_per_wire_canvas; rawtime_per_wire_canvas = NULL;
      return;
    }
  else
    {
      TTree* tree = (TTree*) file->Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress(BRANCHNAME, &hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
      QwHit    *hit        = NULL;
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;   
      //      TH1D *histo_region[3][7] = {{NULL}};
      TH1D *histo_region1[6]  = {NULL};
      TH1D *histo_region2[12] = {NULL};
      TH1D *histo_region3[2]  = {NULL};
      
      Double_t tdc_rawtime = 0.0;
      Short_t  region      = 0;
      Short_t  package     = 0;
      Short_t  direction   = 0;
      Short_t  plane       = 0;

      nevent = tree -> GetEntries();
      printf("Run %d Wire number  %d total event %d\n", run_number, wire,  nevent);
      
      for(Short_t i=0; i<3; i++)
	{
 	  region          = i+1;

	  if( region==1 ) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  histo_region1[j]=  new TH1D(Form("RG%dDIR%d", region, direction), 
					      Form("Raw Time per Wire %d  RG %d DIR %d", wire, region, direction), 
					      BINNUMBER, 0, 0); 
		  histo_region1[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==2 )
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1; 
		  histo_region2[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
				      Form("Raw Time per Wire %d  RG %d PL %d", wire, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region2[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }
	  
	  if( region==3 ) 
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1; 
		  histo_region3[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Raw Time per Wire %d  RG %d PL %d", wire, region, plane), 
					      BINNUMBER, 0, 0); 
		  histo_region3[j] -> SetDefaultBufferSize(BUFFERSIZE);
		}
	    }

// 	  for(Short_t j=0; j<7; j++)
// 	    {
// 	      direction = j+1;
// 	      histo_region[i][j] = new TH1D(Form("Region%dTraw%d", region, j), 
// 					    Form("Time Raw Histogram per wire %d, Re %d Dir %d", wire, region, direction), 
// 					    BINNUMBER, 0, 0); 
// 	      histo_region[i][j] -> SetDefaultBufferSize(1000);

// 	    }
	}


      Bool_t region_status[4]    = {true, 
				    false, false, false};
      Bool_t direction_status[7] = {true, 
				    false, false, false, false, false, false};
      Bool_t plane_status[13]    = {true, 
				    false, false, false, false, false, false, 
				    false, false, false, false, false, false};

      if(event_end == 0) event_end = nevent;
      
      printf("You selected the events with [%d,%d]\n", event_begin, event_end);

      for(ev_i=event_begin; ev_i<event_end; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit = (QwHit*) hitContainer->GetHit(hit_i);

	      if( hit->GetElement() == wire ) 
		{
		  tdc_rawtime = hit->GetRawTime();
		  region      = (Short_t) hit->GetRegion();
		  package     = (Short_t) hit->GetPackage();
		  direction   = (Short_t) hit->GetDirection();
		  plane       = (Short_t) hit->GetPlane();

		  if(!region_status[region])       region_status[region] = true;
		  if(!direction_status[direction]) direction_status[direction] = true;
		  if(!plane_status[plane])         plane_status[plane]         = true;
		  
		  if(ev_i%100==0) 
		    printf("Region %1d, evID %12d package %1d, direction %1d plane %2d wire %4d:  time %4.2e\n", 
			   region, ev_i, package, direction, plane, wire, tdc_rawtime); 
		  if(region == 1) histo_region1[direction-1] -> Fill(tdc_rawtime);
		  if(region == 2) histo_region2[plane-1]     -> Fill(tdc_rawtime);
		  if(region == 3) histo_region3[plane-1]     -> Fill(tdc_rawtime);
		}
	    }
	  
	  hitContainer->Clear();
	}

      rawtime_per_wire_canvas -> Divide(6,4);  
      
      TLatex *region_tex = NULL;
      region_tex = new TLatex();
      region_tex -> SetTextSize(0.05);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);

      Short_t padnumber = 0;



      region = 1;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<6; j++)
	    {
	      direction = j+1;
	      padnumber = direction;
	      rawtime_per_wire_canvas -> cd(padnumber);
	      
	      if( direction_status[direction] ) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region1[direction-1] -> SetLineColor(kRed);
		  histo_region1[direction-1] -> GetXaxis()-> SetTitle("Raw Time");
		  histo_region1[direction-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region1[direction-1] -> Smooth(smooth_n);
		  histo_region1[direction-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d DIR %1d : NO data", region, direction));
		}
	    }
	}

      
      region = 2;

      if( region_status[region] )
	{
	  for(Short_t j=0; j<12; j++)
	    {
	      plane     = j+1;
	      padnumber = 6 + plane;
	      rawtime_per_wire_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  // 		  gStyle -> SetOptStat(0);
		  histo_region2[plane-1] -> SetLineColor(kRed);
		  histo_region2[plane-1] -> GetXaxis()-> SetTitle("Raw Time");
		  histo_region2[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region2[plane-1] -> Smooth(smooth_n);
		  histo_region2[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	    }
	}


      region = 3;
      if( region_status[region] )
	{
	  for(Short_t j=0; j<2; j++)
	    {
	      plane     = j+1;
	      padnumber = 18 + plane;
	      //	      printf("region 3 pad %d \n", padnumber);
	      rawtime_per_wire_canvas -> cd(padnumber);
	      if(plane_status[plane]) 
		{
		  //		  gStyle -> SetOptStat(0);
		  histo_region3[plane-1] -> SetLineColor(kRed);
		  histo_region3[plane-1] -> GetXaxis()-> SetTitle("Raw Time");
		  histo_region3[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		  histo_region3[plane-1] -> Smooth(smooth_n);
		  histo_region3[plane-1] -> Draw();
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	      
	    }
	}
      rawtime_per_wire_canvas -> cd();

    }
  //     file->Close();
  // if one cloese the file in the ROOT script, there are nothing on the canvas.
  timer.Stop();
  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );

      
};

 
 
// need to understand what people wants to do.... not ready to use
// // --------------------------------------------------------------------------------
// // Histogram of the difference between two time values from two specific TDC
// // channels for events  (event_begin:event_end) in a given run (run_number).
// // --------------------------------------------------------------------------------
// // valid for region 3
// void 
// plot_difftime_histogram(Int_t run_number=1567, Int_t tchan_one=0, Int_t tchan_two=9, UInt_t event_begin=0, UInt_t event_end=0, Int_t smooth_n=1)
// {

 
//   if( (event_begin > event_end) )
//     {
//       printf("You selected the wrong range of the event\n");
//       return;
//     }
//   if ( (event_begin == event_end) && (event_begin != 0))
//     {
//       event_begin = 0;
//       printf("The event range is not allowed\n");
//     }

//   Bool_t debug =false;

//   check_libraries();
//   TStopwatch timer;
//   timer.Start();

//   TSuperCanvas *diff_time_canvas = new TSuperCanvas("diff_time_canvas","diff time from 2 TDC channels",10,10,1200,360);
    
//   TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
//   if (file->IsZombie()) 
//     {
//       printf("Error opening file\n"); 
//       delete diff_time_canvas; diff_time_canvas = NULL;
//       return;
//     }
//   else
//     {
//       TTree* tree = (TTree*) file->Get("tree");
//       QwHitRootContainer* hitContainer = NULL;
//       tree->SetBranchAddress("events",&hitContainer);
//       Double_t chan[3]        = {tchan_one, tchan_two, -1};
//       Int_t    nevent         = 0;
//       Int_t    nhit           = 0;
//       Double_t tdc_rawtime[2] = {0.0};
//       Short_t  region         = 0;
//       QwHit    *hit           = NULL;
//       Int_t    ev_i           = 0; 
//       Int_t    hit_i          = 0;
//       TH1D     *histo[3]       = {NULL};
//       Bool_t   chan_status[2] = {false};
//       Double_t diff_time      = 0.0;
      
//       nevent = tree -> GetEntries();
//       printf("Run %d TDC channels [%d,%d] total event %d\n", run_number, tchan_one, tchan_two,  nevent);
      
//       region          = 3;
//       for(Short_t i=0; i<3; i++)
// 	{
// 	  histo[i] = new TH1D(Form("chan_%d", i), 
// 			      Form("Diff Time Raw Histogram with Chan %d", i ), BINNUMBER, 0, 0);
// 	  histo[i] -> SetDefaultBufferSize(BUFFERSIZE);
// 	}
      

//       //   Bool_t region_status[4] = {true, false, false, false};

//       if(event_end == 0) event_end = nevent;
      
//       printf("You selected the events with [%d,%d]\n", event_begin, event_end);

//       for(ev_i=event_begin; ev_i<event_end; ev_i++)
// 	{
// 	  tree -> GetEntry(ev_i);
// 	  nhit = hitContainer->GetSize();
	  
// 	  for(hit_i=0; hit_i<nhit; hit_i++)
// 	    {
// 	      hit    = (QwHit*) hitContainer->GetHit(hit_i);
// 	      region = hit->GetRegion();
// 	      if ( region != 3 ) 
// 		{
// 		  printf("This function is valid for only region 3\n");
// 		  delete diff_time_canvas; diff_time_canvas = NULL;
// 		  return;
// 		}
// 	      if( hit->GetChannel() == chan[0] )
// 		{
// 		  histo[0] -> Fill(hit->GetRawTime());
// 		}
// 	      if( hit->GetChannel() == chan[1] )
// 		{
// 		  histo[1] -> Fill(hit->GetRawTime());
// 		}
// 	    }
// 	  hitContainer->Clear();
// 	}

//       diff_time_canvas -> Divide(3,1);  

//       histo[2] -> Add(histo[0], histo[1], 1, 1);
//       TLatex *region_tex = new TLatex();
//       region_tex -> SetTextSize(0.05);
//       region_tex -> SetTextColor(kRed);
//       region_tex -> SetTextAlign(22);
      
//      for(Short_t i=0; i<3; i++) 
// 	{
// 	  diff_time_canvas -> cd(i+1);
// 	  histo[i] -> SetLineColor(kRed);
// 	  histo[i] -> GetXaxis()-> SetTitle("Diff Time");
// 	  histo[i] -> GetYaxis()-> SetTitle("Number of measurements");
// 	  histo[i] -> GetXaxis()-> CenterTitle();
// 	  histo[i] -> GetYaxis()-> CenterTitle();
// 	  histo[i] -> Smooth(smooth_n);
// 	  histo[i] -> Draw();
// 	}
//  //      else
// // 	{
// // 	  region_tex -> DrawLatex(0.5,0.5, "This data has no information that you are interested");
// // 	}
//     }
//   //     file->Close();
//   // if one cloese the file in the ROOT script, there are nothing on the canvas.
//   timer.Stop();
  
//   printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime());
      
// }

 


// --------------------------------------------------------------------------------
// Plot of the number of events with a given wire having a hit, plotted vs. wire 
// number
// --------------------------------------------------------------------------------

// static const Int_t kNumDirections = 7;
// enum EQwDirectionID {
//   kDirectionNull,
//   kDirectionX, kDirectionY,
//   kDirectionU, kDirectionV,
//   kDirectionR, kDirectionPhi
// };


// Region 3
// direction 3 (U) == plane 1
// direction 4 (V) == plane 2

// Region 2
// direction 1 (X,X') == plane 1,4,7,10
// direction 3 (U,U') == plane 2,5,8,11
// direction 4 (V,V;) == plane 3,6,9,12

void 
plot_nevent_per_wire(Int_t run_number   = 1567, 
		     UInt_t event_begin = 0, 
		     UInt_t event_end   = 0)
{

  if( (event_begin > event_end) )
    {
      printf("You selected the wrong range of the event\n");
      return;
    }
  if ( (event_begin == event_end) && (event_begin != 0) )
    {
      event_begin = 0;
      printf("The event range is not allowed\n");
    }
  
  Bool_t debug = false;
  
  check_libraries();
  
  TStopwatch timer;
  
  timer.Start();
  TSuperCanvas *nevent_per_wire_canvas = NULL;
  nevent_per_wire_canvas = new TSuperCanvas("nevent_per_wire_canvas","nevent per wire",10,10,1000,640);
  
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number));
  
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete nevent_per_wire_canvas;nevent_per_wire_canvas = NULL;
      return;
    }
  else
    {
      QwHitRootContainer* hitContainer = NULL;
      TTree* tree = (TTree*) file->Get("tree");
      tree -> SetBranchAddress(BRANCHNAME, &hitContainer);
      
      Int_t nevent             = 0;
      Int_t nhit               = 0;
      QwHit *hit               = NULL;
      Int_t ev_i               = 0; 
      Int_t hit_i              = 0;
      //      TH1D *histo_region[3][12] = {{NULL}};
      TH1D *histo_region1[6]  = {NULL};
      TH1D *histo_region2[12] = {NULL};
      TH1D *histo_region3[2]  = {NULL};


      range wire_range[3]      = {{0.0,0.0},{0.0,0.0}, {0.0,0.0}};
      Short_t  region          = 0;
      Short_t  wire            = 0;
      Short_t  package         = 0;
      Short_t  direction       = 0;
      Short_t  plane           = 0;
      Short_t  tmp             = 0;

      nevent = tree -> GetEntries();
      printf("Run %d total event %d\n", run_number, nevent);
      
      // offset is introduced to match the axis number with the wire number.
      Double_t offset = -0.5;

      //  Short_t binN[3] = {BINNUMBER, 32, 266};
      
      Short_t binN[3] = {BINNUMBER, WIRENUMREG2, WIRENUMREG3}; // more than the number of wires
      
      for(Short_t i=0; i<3; i++)
	{
	  wire_range[i].min = offset;
	  wire_range[i].max = binN[i]+offset;
	  region            = i+1;

	  if( region==1 ) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  histo_region1[j]=  new TH1D(Form("RG%dDIR%d", region, direction), 
					      Form("Number of Events per Wire of RG %d DIR %d", region, direction), 
					      binN[i], wire_range[i].min, wire_range[i].max);
		}
	    }
	  if( region==2 )
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1; 
		  histo_region2[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					      binN[i], wire_range[i].min, wire_range[i].max);
		}
	    }
	  if( region==3 ) 
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1; 
		  histo_region3[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					      Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					      binN[i], wire_range[i].min, wire_range[i].max);
		}
	    }
	  
	}
      
      Bool_t region_status[4]    = {true, 
				    false, false, false};
      Bool_t direction_status[7] = {true, 
				    false, false, false, false, false, false};
      Bool_t plane_status[13]    = {true, 
				    false, false, false, false, false, false, 
				    false, false, false, false, false, false};
      
      if(event_end == 0) event_end = nevent;
      
      printf("You selected the events with [%d,%d]\n", event_begin, event_end);
      
      
      for(ev_i=event_begin; ev_i<event_end; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit       = (QwHit*)  hitContainer->GetHit(hit_i);
	      wire      = (Short_t) hit->GetElement();
	      region    = (Short_t) hit->GetRegion();
	      package   = (Short_t) hit->GetPackage(); // package will be used to extend this function to different VDCs or HDCs
	      direction = (Short_t) hit->GetDirection();
	      plane     = (Short_t) hit->GetPlane();

	      if(!region_status[region])       region_status[region]       = true;
	      if(!direction_status[direction]) direction_status[direction] = true;
	      if(!plane_status[plane])         plane_status[plane]         = true;

	      if(ev_i%1000==0) 
		printf("Region %1d, evID %12d package %1d, direction %1d plane %2d wire %4d\n", 
		       region, ev_i, package, direction, plane, wire);

	      if(region == 1) histo_region1[direction-1] -> Fill(wire);
	      if(region == 2) histo_region2[plane-1]     -> Fill(wire);
	      if(region == 3) histo_region3[plane-1]     -> Fill(wire);

	    }
	  
	  hitContainer->Clear();
	}

      //      nevent_per_wire_canvas -> SetMenuMargin(0.1);
      nevent_per_wire_canvas -> Divide(6,4);  
      nevent_per_wire_canvas -> ToggleEventStatus();
      //      nevent_per_wire_canvas -> ToggleToolTips();
      
      TLatex *region_tex = NULL;
      region_tex = new TLatex();
      region_tex -> SetTextSize(0.08);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);
      
      Short_t padnumber = 0;
      
      for(Short_t i=0; i<3; i++) 
	{
	  region = i+1;

	  if(region == 1) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  padnumber = (region-1) + direction;
		  nevent_per_wire_canvas -> cd(padnumber);
		  if(region_status[region] && direction_status[direction]) 
		    {
		      histo_region1[direction-1] -> SetLineColor(kRed);
		      histo_region1[direction-1] -> GetXaxis()-> SetTitle("Element #");
		      histo_region1[direction-1] -> GetYaxis()-> SetTitle("Number of Events");
		      histo_region1[direction-1] -> Draw();
		    }
		  else
		    {
		      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d DIR %1d : NO data", region, direction));
		    }
		}
	    }

	  if(region==2)
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1;
		  padnumber = 6 + plane;
		  nevent_per_wire_canvas -> cd(padnumber);
		  if(region_status[region] && plane_status[plane]) 
		    {
		      histo_region2[plane-1] -> SetLineColor(kRed);
		      histo_region2[plane-1] -> GetXaxis()-> SetTitle("Wire #");
		      histo_region2[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		      histo_region2[plane-1] -> Draw();
		    }
		  else
		    {
		      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		    }
		}
	    }


	  if(region==3)
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1;
		  padnumber = 18 + plane;
		  nevent_per_wire_canvas -> cd(padnumber);
		  if(region_status[region] && plane_status[plane]) 
		    {
		      histo_region3[plane-1] -> SetLineColor(kRed);
		      histo_region3[plane-1] -> GetXaxis()-> SetTitle("Wire #");
		      histo_region3[plane-1] -> GetYaxis()-> SetTitle("Number of Events");
		      histo_region3[plane-1] -> Draw();
		    }
		  else
		    {
		      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		    }
		}
	    }
	  //	  nevent_per_wire_canvas -> cd();
	  
	}
    
    }

  timer.Stop();
  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );
      
};

 

// --------------------------------------------------------------------------------
// Histogram of the number of wires with hits, per event
// --------------------------------------------------------------------------------

void 
plot_nwire_hits_per_event(Int_t run_number   = 1567, 
			  UInt_t event_begin = 0, 
			  UInt_t event_end   = 0)
{

  if( (event_begin > event_end) )
    {
      printf("You selected the wrong range of the event\n");
      return;
    }
  if ( (event_begin == event_end) && (event_begin != 0))
    {
      event_begin = 0;
      printf("The event range is not allowed\n");
    }
  
  Bool_t debug = false;
  
  check_libraries();
  
  TStopwatch timer;
  
  timer.Start();
 
  TSuperCanvas *nwire_hits_per_event_canvas = new TSuperCanvas("wire_hits_per_event_canvas","wire_hits_per_event",10,10,1000,640);
  
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number));
  
  if (file->IsZombie()) 
    {
      printf("Error opening file\n"); 
      delete nwire_hits_per_event_canvas;nwire_hits_per_event_canvas = NULL;
      return;
    }
  else
    {
      QwHitRootContainer* hitContainer = NULL;
      TTree* tree = (TTree*) file->Get("tree");
      tree -> SetBranchAddress(BRANCHNAME, &hitContainer);
      
      Int_t nevent             = 0;
      Int_t nhit               = 0;
      QwHit *hit               = NULL;
      Int_t ev_i               = 0; 
      Int_t hit_i              = 0;
      //      TH2D *histo2_region[3][7] = {{NULL}};

      // Because of the limitation of TH2D, which cannot handle a large number of bin numbers (x * y)
      // here TH2I is using for plotting.

      TH2I *histo2_region1[6]  = {NULL, NULL, NULL, NULL, NULL, NULL};
      TH2I *histo2_region2[12] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
      TH2I *histo2_region3[2]  = {NULL, NULL};
      


      range wire_range[3]      = {{0.0,0.0},{0.0,0.0},{0.0,0.0}};
      range event_range[3]     = {{0.0,0.0},{0.0,0.0},{0.0,0.0}};
      Short_t  region          = 0;
      Short_t  wire            = 0;
      Short_t  package         = 0;
      Short_t  direction       = 0;
      Short_t  plane           = 0;
      Short_t  tmp             = 0;

      nevent = tree -> GetEntries();
      if(event_end == 0) event_end = nevent;
      
      printf("Run %d total event %d, selected event ranged [%d,%d]\n", run_number, nevent, event_begin, event_end);
      
      // offset is introduced to match the axis number with the wire number and the event number
      Double_t offset  = -0.5;
      Int_t xbinN[3] = {0, 0, 0};
      Int_t ybinN[3] = {WIRENUMREG1, WIRENUMREG2, WIRENUMREG3}; // more than the number of wires
      
      for(Short_t i=0; i<3; i++)
	{

	  event_range[i].min = event_begin+offset;
	  event_range[i].max = event_end+offset;
	  
	  wire_range [i].min = offset;
	  wire_range [i].max = ybinN[i]+offset;
	  
	  xbinN[i] = (event_range[i].max - event_range[i].min);
	  
 	  region = i+1;
	  
	  printf("xbin %10d, [%8.2f,%8.2f], ybin %10d, [%8.2f,%8.2f]\n",
		 xbinN[i], event_range[i].min, event_range[i].max,
		 ybinN[i], wire_range [i].min, wire_range [i].max);

	  if( region==1 ) 
	    {
	      for(Short_t j=0; j<6; j++)
		{
		  direction = j+1;
		  histo2_region1[j]=  new TH2I(Form("RG%dDIR%d", region, direction), 
					       Form("Number of Events per Wire of RG %d DIR %d", region, direction), 
					       xbinN[i], event_range[i].min, event_range[i].max,
					       ybinN[i], wire_range [i].min, wire_range [i].max);
		}
	    }
	  if( region==2 )
	    {
	      for(Short_t j=0; j<12; j++)
		{
		  plane = j+1; 
		  histo2_region2[j]=  new TH2I(Form("RG%dPL%d", region, plane), 
					       Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					       xbinN[i], event_range[i].min, event_range[i].max,
					       ybinN[i], wire_range [i].min, wire_range [i].max);
		}
	    }
	  if( region==3 ) 
	    {
	      for(Short_t j=0; j<2; j++)
		{
		  plane = j+1; 
		  histo2_region3[j]=  new TH2I(Form("RG%dPL%d", region, plane), 
					       Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					       xbinN[i], event_range[i].min, event_range[i].max,
					       ybinN[i], wire_range [i].min, wire_range [i].max);
		}
	    }
	  

	  // 	  for(Short_t j=0; j<7; j++)
	  // 	    {
	  // 	      direction = j+1;
	  // 	      histo2_region[i][j] = new TH2D(Form("Region%dDir%d", region, j), 
	  // 					     Form("Hitted Wire vs Event- Region %d Direction %d", region, direction), 
	  // 					    xbinN[i], event_range[i].min, event_range[i].max,
	  // 					    ybinN[i], wire_range [i].min, wire_range [i].max);
	  // 	    }
	}
      
      
      Bool_t region_status[4]    = {true, 
				    false, false, false};
      Bool_t direction_status[7] = {true, 
				    false, false, false, false, false, false};
      Bool_t plane_status[13]    = {true, 
				    false, false, false, false, false, false, 
				    false, false, false, false, false, false};
      
  
      for(ev_i=event_begin; ev_i<event_end; ev_i++)
	{
	  tree -> GetEntry(ev_i);
	  nhit = hitContainer->GetSize();
	  
	  for(hit_i=0; hit_i<nhit; hit_i++)
	    {
	      hit       = (QwHit*)  hitContainer->GetHit(hit_i);
	      wire      = (Short_t) hit->GetElement();
	      region    = (Short_t) hit->GetRegion();
	      package   = (Short_t) hit->GetPackage();    // package will be used to extend this function to different VDCs or HDCs
	      direction = (Short_t) hit->GetDirection();
	      plane     = (Short_t) hit->GetPlane();


	      if(!region_status[region])       region_status[region]       = true;
	      if(!direction_status[direction]) direction_status[direction] = true;
	      if(!plane_status[plane])         plane_status[plane]         = true;


	      if(region == 1) histo2_region1[direction-1] -> Fill(ev_i, wire);
	      if(region == 2) histo2_region2[plane-1]     -> Fill(ev_i, wire);
	      if(region == 3) histo2_region3[plane-1]     -> Fill(ev_i, wire);

 	      if(ev_i%1000==0) 
 		printf("Region %1d, evID %12d package %1d, direction %1d plane %2d wire %4d\n", 
 		       region, ev_i, package, direction, plane, wire);
	    }
	  
	  hitContainer->Clear();
	}

      //      nwire_hits_per_event_canvas -> SetMenuMargin(0.1);
      nwire_hits_per_event_canvas -> ToggleEventStatus();
      //      nwire_hits_per_event_canvas -> ToggleToolTips();
      nwire_hits_per_event_canvas -> Divide(6,4); 
      //      nwire_hits_per_event_canvas ->FeedbackMode(kTRUE);
  
      
      TLatex *region_tex = NULL;
      region_tex = new TLatex();
      region_tex -> SetTextSize(0.08);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);

      Short_t padnumber = 0;
      
      TPaletteAxis* palette[32] = {NULL};

      region = 1;

      if(region_status[region])
	{
	  for(Short_t j=0; j<6; j++)
	    {
	      direction = j+1;
	      padnumber = direction;
	      nwire_hits_per_event_canvas -> cd(padnumber);
	      // 	      nwire_hits_per_event_canvas -> FeedbackMode(kTRUE);
	      //	      printf("region 1 pad %d\n", padnumber);
	      if(direction_status[direction]) 
		{
		  gStyle -> SetOptStat(0);
		  gStyle -> SetPalette(1);
		  histo2_region1[direction-1] -> GetXaxis()-> SetTitle("Event #");
		  histo2_region1[direction-1] -> GetYaxis()-> SetTitle("Element #");
		  histo2_region1[direction-1] -> Draw("COLZ");
		  // 		  gPad -> Update();
		  // 		  palette[padnumber-1] = (TPaletteAxis*)histo2_region1[direction-1]->GetListOfFunctions()->FindObject("palette");
		  // 		  palette[padnumber-1] -> SetLabelOffset(0.005);
		  // 		  palette[padnumber-1] -> SetTitleOffset(1.1);
		  // 		  palette[padnumber-1] -> SetY1NDC(0.2);
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d DIR %1d : NO data", region, direction));
		}
	      
	    }
	}

      region = 2;

      if(region_status[region])
	{
	  for(Short_t j=0; j<12; j++)
	    {
	      plane     = j+1;
	      padnumber = 6 + plane;
	      nwire_hits_per_event_canvas -> cd(padnumber);
	      // 	      nwire_hits_per_event_canvas ->FeedbackMode(kTRUE);
	      //	      printf("region 2 pad %d \n", padnumber);
	      if(plane_status[plane]) 
		{
		  gStyle -> SetOptStat(0);
		  gStyle -> SetPalette(1);
		  histo2_region2[plane-1] -> GetXaxis()-> SetTitle("Event #");
		  histo2_region2[plane-1] -> GetYaxis()-> SetTitle("Wire #");
		  histo2_region2[plane-1] -> Draw("COLZ");
		  // 		  gPad -> Update();
		  // 		  palette[padnumber-1] = (TPaletteAxis*)histo2_region2[plane-1]->GetListOfFunctions()->FindObject("palette");
		  // 		  palette[padnumber-1] -> SetLabelOffset(0.005);
		  // 		  palette[padnumber-1] -> SetTitleOffset(1.1);
		  // 		  palette[padnumber-1] -> SetY1NDC(0.2);
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	    }
	}

      
      region = 3;
     if(region_status[region])
	{
	  for(Short_t j=0; j<2; j++)
	    {
	      plane     = j+1;
	      padnumber = 18 + plane;
	      //	      printf("region 3 pad %d \n", padnumber);
	      nwire_hits_per_event_canvas -> cd(padnumber);
 	      nwire_hits_per_event_canvas -> FeedbackMode(kTRUE);
	      if(plane_status[plane]) 
		{
		  gStyle -> SetOptStat(0);
		  gStyle -> SetPalette(1);
		  histo2_region3[plane-1] -> GetXaxis()-> SetTitle("Event #");
		  histo2_region3[plane-1] -> GetYaxis()-> SetTitle("Wire #");
		  histo2_region3[plane-1] -> Draw("COLZ");
		  // 		  gPad -> Update();
		  // 		  palette[padnumber-1] = (TPaletteAxis*)histo2_region3[plane-1]->GetListOfFunctions()->FindObject("palette");
		  //  		  palette[padnumber-1] -> SetLabelOffset(0.005);
		  //  		  palette[padnumber-1] -> SetTitleOffset(1.1);
		  //  		  palette[padnumber-1] -> SetY1NDC(0.2);
		}
	      else
		{
		  region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
		}
	      
	    }
	}
	  
     //     nwire_hits_per_event_canvas -> cd();
// 	  for(Short_t j=0; j<6; j++)
// 	    {
// 	      direction = j+1;
// 	      padnumber = 6*(region-1) + direction;
// 	      nwire_hits_per_event_canvas -> cd(padnumber);

// 	      if(region_status[region] && direction_status[direction]) 
// 		{
// 		  gStyle -> SetOptStat(0);
// 		  histo2_region[i][j] -> SetLineColor(kRed);
// 		  if(i==0)  histo2_region[i][j] -> GetXaxis()-> SetTitle("Element Number");
// 		  else      histo2_region[i][j] -> GetXaxis()-> SetTitle("Event #");
// 		  histo2_region[i][j] -> GetYaxis()-> SetTitle("Wire #");
// 		  histo2_region[i][j] -> Draw("COLZ");
		  
// 		}
// 	      else
// 		{
// 		  region_tex -> DrawLatex(0.5,0.5,Form("Region %d direction %d has no data", region, direction));
// 		}
// 	    }

			    
    }
  //     file->Close();
  // if one cloese the file in the ROOT script, there are nothing on the canvas.
  timer.Stop();
  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime() );
  
};

 
