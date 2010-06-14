#include <QwGUISubSystem.h>

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
  dROOTCont = NULL;
  dDatabaseCont = NULL;

  TabMenuEntryChecked(kFalse);

  dProgrDlg          = NULL;

  Connect("AddThisTab(QwGUISubSystem*)",dMainName,(void*)main,"AddATab(QwGUISubSystem*)");  
  Connect("RemoveThisTab(QwGUISubSystem*)",dMainName,(void*)main,"RemoveTab(QwGUISubSystem*)");  
  Connect("IsClosing(const char*)",dMainName,(void*)main,"OnObjClose(const char*)");    
  Connect("SendMessageSignal(const char*)",dMainName,(void*)main,"OnReceiveMessage(const char*)");


}

QwGUISubSystem::~QwGUISubSystem()
{

}

void QwGUISubSystem::InitProgressDlg(const char* title, const char *macrotext, const char *microtext, 
				     const char *microtext2, Int_t nitems1, Int_t nitems2, Int_t nitems3, Int_t nLevels)
{
  dProgrDlg = new QwGUIProgressDialog((const TGWindow*)dParent, 
				      (const TGWindow*)dMain, 
				      "dProgrDlg","QwGUIMain",
				      title,macrotext,microtext,
				      microtext2,nitems1,nitems2, 
				      nitems3,600,300,kTrue,nLevels);

  Connect(dProgrDlg,"IsClosing(char*)","QwGUISubSystem",(void*)this,
	  "OnObjClose(char*)");
  dProcessHalt = kFalse;
  gSystem->ProcessEvents();
}

void QwGUISubSystem::IncreaseProgress(Int_t *nItems1, Int_t *nItems2, Int_t *nItems3, 
				      Int_t  nInc1,   Int_t  nInc2,   Int_t  nInc3)
{
  TGFrame *frame = (TGFrame*)dMain;
  if(dProgrDlg){
    if(nItems1){
      if(*nItems1 >= nInc1){
	frame->SendMessage(dProgrDlg,
			   MK_MSG((EWidgetMessageTypes)kC_PR_DIALOG,
				  (EWidgetMessageTypes)kCM_PR_MSG),
			   M_PR_RUN,*nItems1);
	gSystem->ProcessEvents();
	*nItems1 = 0;
      }    
    }
    if(nItems2){
      if(*nItems2 >= nInc2){
	frame->SendMessage(dProgrDlg,
			   MK_MSG((EWidgetMessageTypes)kC_PR_DIALOG,
				  (EWidgetMessageTypes)kCM_PR_MSG),
			   M_PR_SEQ,*nItems2);
	gSystem->ProcessEvents();
	*nItems2 = 0;
      }
    }
    if(nItems3){
      if(*nItems3 >= nInc3){
	frame->SendMessage(dProgrDlg,
			   MK_MSG((EWidgetMessageTypes)kC_PR_DIALOG,
				  (EWidgetMessageTypes)kCM_PR_MSG),
			   M_PR_SEQ2,*nItems3);
	gSystem->ProcessEvents();
	*nItems3 = 0;
      }
    }
  }
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
      sprintf(dMiscbuffer2,"Sub system %s message: Received new ROOT data\n",GetName());
    }

    if(!strcmp(cont->GetDataName(),"DBASE")){
      dDatabaseCont = (QwGUIDatabaseContainer*)cont;
      Connect(dDatabaseCont,"SendMessageSignal(char*)","QwGUISubSystem",(void*)this,
	      "OnReceiveMessage(char*)");      	  
      Connect(dDatabaseCont,"IsClosing(char*)","QwGUISubSystem",(void*)this,
	      "OnObjClose(char*)");

      sprintf(dMiscbuffer2,"Sub system %s message: Received new database data\n",GetName());
    }
  }

  SetLogMessage(dMiscbuffer2, kTrue);
  OnNewDataContainer(cont);
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
  if(!strcmp(obj,"dROOTFile")){
    dROOTCont = NULL;
  }

  if(!strcmp(obj,"dDatabase")){
    dDatabaseCont = NULL;
  }
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
