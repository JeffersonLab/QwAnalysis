#include "QwGUISubSystem.h"
#include "TMath.h"

ClassImp(QwGUISubSystem);

QwGUISubSystem::QwGUISubSystem(const TGWindow *p, const TGWindow *main,
			       const TGTab *tab, const char *objName, 
			       const char *mainname, UInt_t w, UInt_t h)
  : TGCompositeFrame(tab,w,h)
{ 
  dWidth             = w;
  dHeight            = h;
  dParent            = (TGWindow*)p;
  dMain              = (TGWindow*)main;
  dWinCnt            = 0;

  dMainName = mainname;
  dThisName = objName;

  // TabMenuEntryChecked(kFALSE);

  dTabMenuID          = 0;
  dTabMenuItemChecked = false;
  dLogTStampFlag      = false;
  // dMain               = NULL;
  // dParent             = NULL;

  dHistoReset = 0;
  dHistoAccum = 0;
  dHistoPause = 0;
  dMapFile    = NULL;

  dMapFileFlag = false;


  Connect("AddThisTab(QwGUISubSystem*)",dMainName,(void*)main,"AddATab(QwGUISubSystem*)");  
  //  Connect("RemoveThisTab(QwGUISubSystem*)",dMainName,(void*)main,"RemoveTab(QwGUISubSystem*)");  
  // Connect("IsClosing(const char*)",dMainName,(void*)main,"OnObjClose(const char*)");    
  Connect("SendMessageSignal(const char*)",dMainName,(void*)main,"OnReceiveMessage(const char*)");

}

QwGUISubSystem::~QwGUISubSystem()
{

}

const char* QwGUISubSystem::GetNewWindowName()
{
  return Form("dMiscWindow_%02d",GetNewWindowCount());
}


void QwGUISubSystem::SetMapFile(TMapFile *file)
{
  if(file) {
    dMapFile = file;
    dMapFileFlag = true;
    std::cout << "Subsystem " << std::setw(22) << this->GetName()
	      << " now can access the map file at the address "
	      << dMapFile << std::endl;
  }
  else {
    dMapFileFlag = false;
  }
  
};


void QwGUISubSystem::SetLogMessage(const char *buffer, Bool_t tStamp)
{
  // if(!buffer) return;
  // Int_t length = strlen(buffer);
  // if(length >= MSG_SIZE_MAX) length = MSG_SIZE_MAX-1;
  // snprintf(dMiscbuffer,length+2,"%s\n%c",buffer,'\0');
  
  // dLogTStampFlag = tStamp;
  // SendMessageSignal(GetName());
}

// void QwGUISubSystem::OnObjClose(char *obj)
// {

// };


void QwGUISubSystem::AddThisTab(QwGUISubSystem* sbSystem)
{
  Emit("AddThisTab(QwGUISubSystem*)",(long)sbSystem);
}

// void QwGUISubSystem::RemoveThisTab(QwGUISubSystem* sbSystem)
// {
//   OnRemoveThisTab();
//   Emit("RemoveThisTab(QwGUISubSystem*)",(long)sbSystem);
// }

// void QwGUISubSystem::IsClosing(const char *objname)
// {
//   Emit("IsClosing(const char*)",(long)objname);
// }

void QwGUISubSystem::SendMessageSignal(const char*objname)
{

  Emit("SendMessageSignal(const char*)",(long)objname);
}

Bool_t QwGUISubSystem::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  return kTRUE;
}



void QwGUISubSystem::SummaryHist(TH1 *in)
{

  Double_t out[4] = {0.0};
  Double_t test   = 0.0;

  out[0] = in -> GetMean();
  out[1] = in -> GetMeanError();
  out[2] = in -> GetRMS();
  out[3] = in -> GetRMSError();
  test   = in -> GetRMS()/TMath::Sqrt(in->GetEntries());

  printf("%sName%s", BOLD, NORMAL);
  printf("%22s", in->GetName());
  printf("  %sMean%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[0], NORMAL, BLUE, out[1], NORMAL);
  printf("  %sSD%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[2], NORMAL, GREEN, out[3], NORMAL);
  printf(" %sRMS/Sqrt(N)%s %s%+4.2e%s \n", BOLD, NORMAL, BLUE, test, NORMAL);
  return;
};
