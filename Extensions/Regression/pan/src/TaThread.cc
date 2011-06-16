//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan (Online Monitor version)           
//
//        TaThread.cc   (implementation file)
//        ^^^^^^^^^^^
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaThread.cc,v 1.2 2003/07/31 16:12:00 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//   
//
//////////////////////////////////////////////////////////////////////////

#include "TaThread.hh"



ClassImp(TaThread)

TaThread::TaThread(): 
 fPanThread(0),
 fRefreshThread(0)
 {} 

TaThread::TaThread(UInt_t threadnum):
 fPanThread(0),
 fRefreshThread(0)
{
  if(threadnum>2) threadnum=2; 
  mThreadnum=threadnum;
  PanThreadrun=kFALSE;
  RefreshThreadrun=kFALSE;
} 

TaThread::~TaThread(){
  PanThreadStop();
  RefreshThreadStop();  
}




/////////////////////////////////////////////////////////////
// threaded methods calling user functions: 


void TaThread::Thread0(void* arg){
  // thread function which calls user specified action Func0
  TThread::SetCancelAsynchronous();  
  TThread::SetCancelOn();
  //  TThread::SetCancelDeferred();
  TaThread* inst = (TaThread*) arg;
  Bool_t onetime =kTRUE;
  Int_t meid=TThread::SelfId(); // get pthread id
  cout << "\nThread 0, id:" <<meid<< " is running..\n"<<endl;
  while(inst->GetPanThreadRun()){
        // loop keeps thread alive...
        TThread::CancelPoint();
        onetime=kFALSE; 
        inst->DoPan(); // call the user defined threaded function
        if (! onetime ) break;
  }

}

void TaThread::Thread1(void* arg){
  // thread function which calls user specified action Func0
  TThread::SetCancelAsynchronous();  
  TThread::SetCancelOn();
  //  TThread::SetCancelDeferred();
  TaThread* inst = (TaThread*) arg;
  Int_t meid=TThread::SelfId(); // get pthread id
  cout << "\nThread 1, id:" <<meid<< " is running..\n"<<endl;
  while(inst->GetRefreshThreadRun()){
    //        mycondition.Wait();
        // loop keeps thread alive...      
       TThread::CancelPoint();
       //gSystem->Sleep(1000);
       inst->DoRefreshing(); // call the user defined threaded function
  }

}

// void SendThreadSignal(TCondition* cond)
// {
//   cond->Signal();
// }


void TaThread::DoPan(){ 
  cout << "Default Threadfunc 0 running, please overwrite in derived class"<<endl;
  gSystem->Sleep(1000);
}



void TaThread::DoRefreshing(){ 
  cout << "Default Threadfunc 1 running, please overwrite in derived class"<<endl;
  gSystem->Sleep(2000);
}


void TaThread::EndThread()
{
  TThread* ti1=TThread::GetThread("refreshThread");
  TThread::Delete(ti1);
  delete ti1;
}


/////////////////////////////

Int_t TaThread::PanThreadStart(){
  // start all threads
  PanThreadrun=kTRUE;
  if(!fPanThread){
        fPanThread= new TThread("PanThread",
                                                  (void(*) (void *))&Thread0,
                                                  (void*) this);
        fPanThread->Run();
                
  }
  return 0;
}

Int_t TaThread::RefreshThreadStart(){
  // start all threads
  RefreshThreadrun=kTRUE;
  if(!fRefreshThread){
        fRefreshThread= new TThread("refreshThread",
                                                  (void(*) (void *))&Thread1,
                                                  (void*) this);
        fRefreshThread->Run();
        
  }
  return 0;
}

Int_t TaThread::PanThreadStop(){
  // stop all active threads 
  PanThreadrun=kFALSE;    // aborting flag
  gSystem->Sleep(1000); // wait a while for threads to halt
  if(fPanThread){ 
    if (fRefreshThread) RefreshThreadStop();
    TThread::Delete(fPanThread);
    delete fPanThread;
    fPanThread=0;
  }
  return 0;
} 

Int_t TaThread::RefreshThreadStop(){
  // stop all active threads 
 RefreshThreadrun=kFALSE;    // aborting flag
  gSystem->Sleep(1000); // wait a while for threads to halt
  if(fRefreshThread){ 
        TThread::Delete(fRefreshThread);
        delete fRefreshThread;
        fRefreshThread=0;
  }
  return 0;
} 

