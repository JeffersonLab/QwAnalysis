#include "QwGUISubSystem.h"

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
  strcpy(dMainName,mainname);
  strcpy(dThisName,objName);

  TabMenuEntryChecked(kFalse);

  dTabMenuID          = 0;
  dTabMenuItemChecked = false;
  dLogTStampFlag      = false;
  dMain               = NULL;
  dParent             = NULL;

  dHistoReset = 0;
  dHistoAccum = 0;
  dHistoPause = 0;

  strcpy(dMiscbuffer,  " ");
  strcpy(dMiscbuffer2, " ");
  
  dROOTCont = NULL;

  // dProgrDlg          = NULL;

  Connect("AddThisTab(QwGUISubSystem*)",dMainName,(void*)main,"AddATab(QwGUISubSystem*)");  
  Connect("RemoveThisTab(QwGUISubSystem*)",dMainName,(void*)main,"RemoveTab(QwGUISubSystem*)");  
  Connect("IsClosing(const char*)",dMainName,(void*)main,"OnObjClose(const char*)");    
  Connect("SendMessageSignal(const char*)",dMainName,(void*)main,"OnReceiveMessage(const char*)");

}

QwGUISubSystem::~QwGUISubSystem()
{

}

const char* QwGUISubSystem::GetNewWindowName()
{
  return Form("dMiscWindow_%02d",GetNewWindowCount());
}

void QwGUISubSystem::SetDataContainer(RDataContainer *cont)
{
  if(cont){    
    if(!strcmp(cont->GetDataName(),"ROOT")){
      dROOTCont = cont;
      Connect(dROOTCont,"SendMessageSignal(char*)","QwGUISubSystem",(void*)this,
	      "OnReceiveMessage(char*)");      	  
      Connect(dROOTCont,"IsClosing(char*)","QwGUISubSystem",(void*)this,
	      "OnObjClose(char*)");
    }
  }
  else
    dROOTCont = NULL;

  sprintf(dMiscbuffer2,"Sub system %s message: Received new data\n",GetName());
  SetLogMessage(dMiscbuffer2, kTrue);

  OnNewDataContainer();
}

void QwGUISubSystem::SetLogMessage(const char *buffer, Bool_t tStamp)
{
  if(!buffer) return;
  Int_t length = strlen(buffer);
  if(length >= MSG_SIZE_MAX) length = MSG_SIZE_MAX-1;
  snprintf(dMiscbuffer,length+2,"%s\n%c",buffer,'\0');
  
  dLogTStampFlag = tStamp;
  SendMessageSignal(GetName());
}

void QwGUISubSystem::OnObjClose(char *obj)
{

};


void QwGUISubSystem::AddThisTab(QwGUISubSystem* sbSystem)
{
  Emit("AddThisTab(QwGUISubSystem*)",(long)sbSystem);
}

void QwGUISubSystem::RemoveThisTab(QwGUISubSystem* sbSystem)
{
  OnRemoveThisTab();
  Emit("RemoveThisTab(QwGUISubSystem*)",(long)sbSystem);
}

void QwGUISubSystem::IsClosing(const char *objname)
{
  Emit("IsClosing(const char*)",(long)objname);
}

void QwGUISubSystem::SendMessageSignal(const char*objname)
{
  Emit("SendMessageSignal(const char*)",(long)objname);
}

Bool_t QwGUISubSystem::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  return kTRUE;
}
