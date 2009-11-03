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
  hit = (QwHit *) hits_per_event->At(hitID);
  
  // All functions are defined in QwHit.h
  
  printf("    --- Subbank       %8d \n",  hit->GetSubbankID()); 
  printf("    --- Module        %8d \n",  hit->GetModule()); 
  printf("    --- Channel       %8d \n",  hit->GetChannel()); 
  printf("    --- Region        %8d \n",  hit->GetRegion()); 
  printf("    --- Package       %8d \n",  hit->GetPackage()); 
  printf("    --- Direction     %8d \n\n", hit->GetDirection()); 
  printf("    --- DriftDistance %12.2f \n", hit->GetDriftDistance());
  printf("    --- RawTime       %12.2f \n", hit->GetRawTime());
  printf("    --- Time          %12.2f \n", hit->GetTime());
  printf("    --- HitNumber     %12d   \n", hit->GetHitNumber());
  printf("    --- HitNumberR    %12d   \n", hit->GetHitNumberR());


  //  hit->Clear();
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
