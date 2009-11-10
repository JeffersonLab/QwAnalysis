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
// load_libraries()
// read(12)
// or 
// read(12,4)


// Here I use the following defintions:
// event id  == event number ==  HitContainer id
// each event has several hits


void 
load_libraries()
{
  gSystem->Load("./libQwHit.so");
  gSystem->Load("./libQwHitContainer.so");
  gSystem->Load("./libQwHitRootContainer.so");
}


void 
read(Int_t evID=1, Int_t hitID = 0, Int_t run_number=398)
{
 
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


  hit = (QwHit *) hits_per_event->UncheckedAt(hitID);
  // UncheckedAt() : http://root.cern.ch/root/html/src/TObjArray.h.html#KyjP6
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

  hitContainer->Clear();
  delete hitContainer; hitContainer=NULL;
 
  file.Close();
}

void 
read_event_number(int id=1, Int_t run_number=398)
{

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
// Histogram of the time value of all hits on a given TDC channel
// TDC channel is started from 0 to N
// Tue Nov 10 11:44:07 EST 2009
//
// premature funtion, doesn't show "plot" .

void 
plot_time_histogram_per_tdc(Int_t run_number=398, Int_t tdc_chan=0)
{

  TFile file(Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"),run_number));
  
  TTree* tree = (TTree*) file.Get("tree");
  QwHitRootContainer* hitContainer = NULL;
  tree->SetBranchAddress("events",&hitContainer);

  Int_t    nevent      = 0;
  Int_t    nhit        = 0;
  Double_t tdc_time    = 0.0;
  Double_t tdc_rawtime = 0.0;
  QwHit    *hit        = NULL;

  nevent = tree -> GetEntries();
   
  for(Int_t i=0; i<nevent; i++)
    {
      tree -> GetEntry(i);
      nhit = hitContainer->GetSize();
      
      for(Int_t j=0; j<nhit; j++)
	{
	  hit = (QwHit*) hitContainer->GetHit(j);
	  if( (hit->GetChannel() == tdc_chan ) )
	    {
	      tdc_time    = hit->GetTime();
	      tdc_rawtime = hit->GetRawTime();
	      printf("TDC Chan %2d (evN,hitN) (%10d,%2d) T %16.2f, Traw %16.2f\n", 
		     tdc_chan, i,j, tdc_time, tdc_rawtime);
	    }
	}
      
      hitContainer->Clear();
    }
  
  file.Close();
}
