#include <QwGUIDataWindow.h>

ClassImp(QwGUIDataWindow);

QwGUIDataWindow::QwGUIDataWindow(const TGWindow *p, const TGWindow *main,
			 const char * objName, const char *mainname,
			 const char *datatitle, ERPlotTypes type, 
			 ENDataType dtype, UInt_t w, UInt_t h)
  : RSDataWindow(p, main, objName, mainname, datatitle, type, w, h)
{ 
  dOptionsDlg = NULL;

  memset(dMiscbuffer,'\0',sizeof(dMiscbuffer));
  memset(dMiscbuffer2,'\0',sizeof(dMiscbuffer2));
  SetDataType(dtype);
  SetStaticData(NULL,-1);
  SetChannel(0);
  dOptions.startev        = 0;
  dOptions.numevs         = 1000;
  dOptions.limitsFlag     = kFalse;
  dOptions.mpAverage      = kFalse;
  dOptions.mpIntegrate    = kFalse;
  dOptions.mpPedSubtract  = kFalse;
  dOptions.mpBgrdSubtract = kFalse;

  AddPopupEntry("Set &Attributes...", M_PLOT_ATTR);
}

QwGUIDataWindow::~QwGUIDataWindow()
{  
  RSDataWindow::CloseWindow();
}

void QwGUIDataWindow::SetStaticData(TObject *obj, Int_t chan)
{
  dStaticChannel = chan;
  dOptions.channel = chan;
  dStaticPlotObject = obj;
}

void QwGUIDataWindow::SetChannel(Int_t chan)
{
  dStaticChannel = -1;
  dOptions.channel = chan;
}

Int_t QwGUIDataWindow::GetPlotChannel()
{
  if(dStaticChannel > -1)
    return dStaticChannel;
  return dOptions.channel;
}

Int_t QwGUIDataWindow::GetStartingMacroPulse()
{
  return dOptions.startev;
}

Int_t QwGUIDataWindow::GetMacroPulseRange()
{
  return dOptions.numevs;
}

Bool_t QwGUIDataWindow::AverageMacroPulses()
{
  return dOptions.mpAverage;
}

Bool_t QwGUIDataWindow::IntegrateMacroPulses()
{
  return dOptions.mpIntegrate;
}

Bool_t QwGUIDataWindow::NormalizeToBeam()
{
  return dOptions.mpNormToBeam;
}

Bool_t QwGUIDataWindow::SubtractPedestals()
{
  return dOptions.mpPedSubtract;
}

Bool_t QwGUIDataWindow::SubtractBackgrounds()
{
  return dOptions.mpBgrdSubtract;
}

void QwGUIDataWindow::OnObjClose(char *obj)
{
  RSDataWindow::OnObjClose(obj);

  TString name = obj;
  if(name.Contains("dOptionsDlg")){
    dOptionsDlg = NULL;
  }
}

void QwGUIDataWindow::UpdatePlot(char *obj)
{
  Emit("UpdatePlot(char *)",(long)obj);
}

Bool_t QwGUIDataWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  RSDataWindow::ProcessMessage(msg,parm1,parm2);

  switch (GET_MSG(msg)) {
    
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {

    case kCM_MENU:
      switch (parm1) {
      
      case M_PLOT_ATTR:
	dOptionsDlg = new QwGUIPlotOptionsDialog(fClient->GetRoot(), this,
						 "dOptionsDlg","QwGUIDataWindow",
						 &dOptions,400, 200);
	if(dOptions.limitsFlag) UpdatePlot(GetObjectName());
	break;
	
      default:
	break;
      }
    }
    break;
    
  default:
    break;
  }
  return kTRUE;
}




