// Name  :   ReadHits.C
// Author:   Jeong Han Lee
// email :   jhlee@jlab.org
// Date:     Tue Nov 10 16:55:06 EST 2009
// Version:  0.0.1
//
//  An example script that uses a "generic USER way" in order to read 
//  some useful information from "hit-based" ROOT output file created 
//  by the QwAnalysis

//
// History:
// 0.0.0     2009/11/04   created
//                        add load_libraries(), read(), and read_event_number()
//
// 0.0.1     2009/11/11   introduce a "premature" plot_time_histogram_per_tdc()
//                        to access a single information from ROOT file
//
// 0.0.2     2009/11/12   add : "if condition" to distingush whether the hits are
//                        or not when the region 3 (W&M) TDC data
//                        add : extract the histogram range of "Traw" 
// 0.0.3     2009/11/17   histogram of the time value of all hits on a given
//                        TDC channel and wire number



// Examples
//
// cd $QWANAYSIS
// make distclean
// make 
// cd Extensions/Macro/Tracking/
// sh lib.sh
// qwtracking -r 398
// 
// .L ReadHits.C
// read(12)
// or 
// read(12,4)


// Here I use the following defintions:
// event id  == event number ==  HitContainer id
// each event has several hits


//void 
//load_libraries()
//{
//  gSystem->Load("./libQwHit.so");
//  gSystem->Load("./libQwHitContainer.so");
//  gSystem->Load("./libQwHitRootContainer.so");
//}


void
check_libraries()
{
  if(!TClassTable::GetDict("QwHit"))              gSystem -> Load("./libQwHit.so");
  if(!TClassTable::GetDict("QwHitContainer"))     gSystem -> Load("./libQwHitContainer.so");
  if(!TClassTable::GetDict("QwHitRootContainer")) gSystem -> Load("./libQwHitRootContainer.so");
}



void
IQR(TH1D &histo)
{
  printf("-------------\n");
  Double_t x[2] = {0.25, 0.75};
  Double_t y[2] = {0.0};

  histo.GetQuantiles(2,y,x);

  printf("%f%f\n", y[0], y[1]);

}


void 
read(Int_t evID=1, Int_t hitID = 0, Int_t run_number=398)
{

  check_libraries();
  //  IQR();

  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  TTree* tree = (TTree*) file.Get("tree");
  QwHitRootContainer* hitContainer = NULL;
  
  tree->SetBranchAddress("events",&hitContainer);
  
  Int_t nevent  = tree->GetEntries();
  
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
  Int_t hitN = hits_per_event  -> GetEntries();
  printf("    --- %d hits (by TCloneArray)\n", hitN); 

  if(hitN == 0) 
    {
      printf(" *** no hit in the event number %d\n", evID);
    }
  else 
    {
      
      QwHit *hit = NULL;
      if(hitID > hitN) 
	{
	  hitID = hitN-1;
	  printf("    --- You selected the hit number which is out of range.\n");
	  printf("    --- Thus, the maximum hit number %d is selected.\n", hitID);
	}
      
      
      
      hit = (QwHit *) hits_per_event->At(hitID);
      // At() : http://root.cern.ch/root/html/src/TObjArray.h.html#R3YkqE
      // 
      // All functions are defined in QwHit.h
      printf("    -------------------------------- \n"); 
      printf("    --- Subbank       %6d \n",  hit->GetSubbankID()); 
      printf("    --- Module        %6d \n",  hit->GetModule()); 
      printf("    --- Channel       %6d \n",  hit->GetChannel()); 
      printf("    --- Region        %6d \n",  hit->GetRegion()); 
      printf("    --- Package       %6d \n",  hit->GetPackage()); 
      printf("    --- Direction     %6d \n", hit->GetDirection()); 
      printf("    -------------------------------- \n"); 
      printf("    --- DriftDistance %14.2f \n", hit->GetDriftDistance());
      printf("    --- RawTime       %14.2f \n", hit->GetRawTime());
      printf("    --- Time          %14.2f \n", hit->GetTime());
      printf("    --- HitNumber     %14d   \n", hit->GetHitNumber());
      printf("    --- HitNumberR    %14d   \n", hit->GetHitNumberR());
      printf("    -------------------------------- \n"); 
      
      
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
  delete hitContainer; hitContainer=NULL;
  
  file.Close();
}

void 
read_event_number(int id=1, Int_t run_number=398)
{

 check_libraries();

  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  
  TTree* tree = (TTree*) file.Get("tree");
  QwHitRootContainer* hitContainer = NULL;
  tree->SetBranchAddress("hits",&hitContainer);


  Int_t nevent  = tree->GetEntries();
  tree -> GetEntry(id);
  printf("Event : %d, Hits %d\n", id, hitContainer->GetSize());
  
  
  
  
  hitContainer->Clear();
  delete hitContainer; hitContainer=NULL;
  file.Close();
}



//
// Retrun Histogram range of the time value of all hits on a given TDC channel
// TDC channel is started from 0 to N
// Tue Nov 10 11:44:07 EST 2009
//
// premature funtion, doesn't show "plot" .
// 
// Is there a better way to determine the range of the histogram?    
// T raw range is [        0.00.     61070.00] with 398  (region2)
// T raw range is [        0.00,      1703.00] with 1567 (region3)


typedef struct {
  Double_t min;
  Double_t max;
} range;



void 
histogram_range_per_tdc(Int_t run_number=398, Bool_t debug=false, Int_t tdc_chan=0)
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
      tree->SetBranchAddress("events",&hitContainer);
      
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

  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime());

  //    range h_range = {0.0, 0.0};
  // T raw range is [        0.00.     61070.00] with 398  (region2)
  // T raw range is [        0.00,      1703.00] with 1567 (region3)

}


// --------------------------------------------------------------------------------
// Histogram of the time value of all hits that are assigned to a given TDC channel
// (tdc_chan) for events  (event_begin:event_end) in a given run (run_number).
// --------------------------------------------------------------------------------
void 
plot_time_histogram_per_tdc(Int_t run_number=398, Int_t tdc_chan=0, UInt_t event_begin=0, UInt_t event_end=0, Int_t smooth_n=1)
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

  TCanvas *time_per_tdc_canvas = new TCanvas("time_per_tdc_canvas","time per tdc channel",10,10,1200,360);
    
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  if (file->IsZombie()) 
    {
      printf("Error opening file\n");  
      exit(-1);
    }
  else
    {
      TTree* tree = (TTree*) file->Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress("events",&hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
      Double_t tdc_rawtime = 0.0;
      Short_t  region      = 0;
      QwHit    *hit        = NULL;
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;
      TH1D     *histo_region[3] = {NULL, NULL, NULL};

      nevent = tree -> GetEntries();
      printf("Run %d TDC channel %d total event %d\n", run_number, tdc_chan,  nevent);
      
      for(Short_t i=0; i<3; i++)
	{
 	  region          = i+1;
	  histo_region[i] = new TH1D(Form("Region%dTraw", region), 
  				     Form("Time Raw Histogram per channel %d of Region %d", 
  					  tdc_chan, region), 2048, 0, 0);
	  histo_region[i] -> SetDefaultBufferSize(4000);
	}

      Bool_t region_status[4] = {true, false, false, false};

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
		  tdc_rawtime = hit->GetRawTime();
		  region      = hit->GetRegion();
		  if(!region_status[region]) region_status[region] = true;
		  if(ev_i%100==0) printf("evID %d region %d, Traw %d\n", ev_i, region, tdc_rawtime);
		  histo_region[region-1] -> Fill(tdc_rawtime);
		}
	    }
	  
	  hitContainer->Clear();
	}

      time_per_tdc_canvas -> Divide(3,1);  
      
      TLatex *region_tex = new TLatex();
      region_tex -> SetTextSize(0.05);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);
      
      for(Short_t i=0; i<3; i++) 
	{
	  region = i+1;
	  time_per_tdc_canvas -> cd(region);
	  //	  gPad -> SetLogy();
	  if(region_status[region]) 
	    {
	      histo_region[i] -> SetLineColor(kRed);
	      histo_region[i] -> GetXaxis()-> SetTitle("Time (s)");
	      histo_region[i] -> GetYaxis()-> SetTitle("Number of measurements");
	      histo_region[i] -> GetXaxis()-> CenterTitle();
	      histo_region[i] -> GetYaxis()-> CenterTitle();
	      histo_region[i] -> Smooth(smooth_n);
	      histo_region[i] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("Region %d has no data", region));
	    }
	}
    }
  //     file->Close();
  // if one cloese the file in the ROOT script, there are nothing on the canvas.
  timer.Stop();
  
  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime());
      
}

 

// --------------------------------------------------------------------------------
// Histogram of the time value of all hits that are assigned to a given drift 
// chammber wire (wire) for events  (event_begin:event_end) in a given run (run_number).
// --------------------------------------------------------------------------------
void 
plot_time_histogram_per_wire(Int_t run_number=398, Int_t wire=0, UInt_t event_begin=0, UInt_t event_end=0, Int_t smooth_n=1)
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

  TCanvas *time_per_wire_canvas = new TCanvas("time_per_wire_canvas","time per wire",10,10,1200,360);
    
  TFile *file =  new TFile(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  if (file->IsZombie()) 
    {
      printf("Error opening file\n");  
      exit(-1);
    }
  else
    {
      TTree* tree = (TTree*) file->Get("tree");
      QwHitRootContainer* hitContainer = NULL;
      tree->SetBranchAddress("events",&hitContainer);
      
      Int_t    nevent      = 0;
      Int_t    nhit        = 0;
      Double_t tdc_rawtime = 0.0;
      Short_t  region      = 0;
      QwHit    *hit        = NULL;
      Int_t    ev_i        = 0; 
      Int_t    hit_i       = 0;
      TH1D     *histo_region[3] = {NULL, NULL, NULL};

      nevent = tree -> GetEntries();
      printf("Run %d Wire number  %d total event %d\n", run_number, wire,  nevent);
      
      for(Short_t i=0; i<3; i++)
	{
 	  region          = i+1;
	  histo_region[i] = new TH1D(Form("Region%dTraw", region), 
  				     Form("Time Raw Histogram per wire %d of Region %d", 
  					  wire, region), 2048, 0, 0);
	  histo_region[i] -> SetDefaultBufferSize(4000);
	}

      Bool_t region_status[4] = {true, false, false, false};

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
		  region      = hit->GetRegion();
		  if(!region_status[region]) region_status[region] = true;
		  if(ev_i%100==0) printf("evID %d region %d, Traw %d\n", ev_i, region, tdc_rawtime);
		  histo_region[region-1] -> Fill(tdc_rawtime);
		}
	    }
	  
	  hitContainer->Clear();
	}

      time_per_wire_canvas -> Divide(3,1);  
      
      TLatex *region_tex = new TLatex();
      region_tex -> SetTextSize(0.05);
      region_tex -> SetTextColor(kRed);
      region_tex -> SetTextAlign(22);
      
      for(Short_t i=0; i<3; i++) 
	{
	  region = i+1;
	  time_per_wire_canvas -> cd(region);
	  //	  gPad -> SetLogy();
	  if(region_status[region]) 
	    {
	      histo_region[i] -> SetLineColor(kRed);
	      histo_region[i] -> GetXaxis()-> SetTitle("Time (s)");
	      histo_region[i] -> GetYaxis()-> SetTitle("Number of measurements");
	      histo_region[i] -> GetXaxis()-> CenterTitle();
	      histo_region[i] -> GetYaxis()-> CenterTitle();
	      histo_region[i] -> Smooth(smooth_n);
	      histo_region[i] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("Region %d has no data", region));
	    }
	}
    }
  //     file->Close();
  // if one cloese the file in the ROOT script, there are nothing on the canvas.
  timer.Stop();
  
  printf("Time used : CPU %8.2f, Real %8.2f (sec)\n", timer.CpuTime(), timer.RealTime());
      
}

 
