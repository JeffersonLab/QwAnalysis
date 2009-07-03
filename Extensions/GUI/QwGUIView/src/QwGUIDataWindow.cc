#include <QwGUIDataWindow.h>

ClassImp(QwGUIDataWindow);

QwGUIDataWindow::QwGUIDataWindow(const TGWindow *p, const TGWindow *main,
			       const TGTab *tab, const char *objName, 
			       const char *mainname, UInt_t w, UInt_t h)
  : TGCompositeFrame(tab,w,h)
{ 
  dWidth             = w;
  dHeight            = h;
  dParent            = (TGWindow*)p;
  dMain              = (TGWindow*)main;
  strcpy(dMainName,mainname);
  strcpy(dThisName,objName);

  dTabMenuItemChecked = kFalse;

  Connect("AddThisTab(QwGUIDataWindow*)",dMainName,(void*)main,"AddATab(QwGUIDataWindow*)");  
  Connect("RemoveThisTab(QwGUIDataWindow*)",dMainName,(void*)main,"RemoveTab(QwGUIDataWindow*)");  
  Connect("IsClosing(const char*)",dMainName,(void*)main,"OnObjClose(const char*)");    
  Connect("SendMessageSignal(const char*)",dMainName,(void*)main,"OnReceiveMessage(const char*)");

}

QwGUIDataWindow::~QwGUIDataWindow()
{

}

void QwGUIDataWindow::SetDataContainer(RDataContainer *cont)
{
  if(cont){    
    if(!strcmp(cont->GetDataName(),"ROOT")){
      dROOTCont = cont;
      Connect(dROOTCont,"SendMessageSignal(char*)","QwGUIDataWindow",(void*)this,
	      "OnReceiveMessage(char*)");      	  
      Connect(dROOTCont,"IsClosing(char*)","QwGUIDataWindow",(void*)this,
	      "OnObjClose(char*)");
    }
  }
  else
    dROOTCont = NULL;

  sprintf(dMiscbuffer2,"Sub system %s message: Received new data\n",GetName());
  SetLogMessage(dMiscbuffer2, kTrue);

  OnNewDataContainer();
}

void QwGUIDataWindow::SetLogMessage(const char *buffer, Bool_t tStamp)
{
  if(!buffer) return;
  Int_t length = strlen(buffer);
  if(length >= MSG_SIZE_MAX) length = MSG_SIZE_MAX-1;
  snprintf(dMiscbuffer,length+2,"%s\n%c",buffer,'\0');
  
  dLogTStampFlag = tStamp;
  SendMessageSignal(GetName());
}

void QwGUIDataWindow::AddThisTab(QwGUIDataWindow* sbSystem)
{
  Emit("AddThisTab(QwGUIDataWindow*)",(long)sbSystem);
}

void QwGUIDataWindow::RemoveThisTab(QwGUIDataWindow* sbSystem)
{
  OnRemoveThisTab();
  Emit("RemoveThisTab(QwGUIDataWindow*)",(long)sbSystem);
}

void QwGUIDataWindow::IsClosing(const char *objname)
{
  Emit("IsClosing(const char*)",(long)objname);
}

void QwGUIDataWindow::SendMessageSignal(const char*objname)
{
  Emit("SendMessageSignal(const char*)",(long)objname);
}

Bool_t QwGUIDataWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  return kTRUE;
}
