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
  
  int nevent  = tree->GetEntries();
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
read_event_number(int id=1, char *filename="hitlist")
{

  // Load the tree
  TFile file(Form("%s.root", filename));

  TTree* tree = (TTree*) file.Get("tree");
  QwHitRootContainer* hitContainer = NULL;
  tree->SetBranchAddress("hits",&hitContainer);


  int nevent  = tree->GetEntries();
  tree -> GetEntry(id);
  printf("Event : %d, Hits %d\n", id, hitContainer->GetSize());




  hitContainer->Clear();
  delete hitContainer; hitContainer=NULL;
  file.Close();
}
