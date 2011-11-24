#include <QwGUIDataWindow.h>

ClassImp(QwGUIDataWindow);

QwGUIDataWindow::QwGUIDataWindow(const TGWindow *p, const TGWindow *main,
			 const char * objName, const char *mainname,
			 const char *datatitle, ERPlotTypes type, 
			 ENDataType dtype, UInt_t w, UInt_t h)
  : RSDataWindow(p, main, objName, mainname, datatitle, type, w, h)
{ 
  dMain = main;
  dOptionsDlg = NULL;
  dHCLogEntryDlg = NULL;

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
  dRunNumber = 0;

  AddPopupEntry("Set &Attributes...", M_PLOT_ATTR);
  AddPopupEntry("Submit To &HClog...", M_HCLOG_SUBM);
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

void QwGUIDataWindow::SubmitToHCLog()
{

  dHCLogEntryDlg = new QwGUIHCLogEntryDialog(fClient->GetRoot(),0,
					     "dHCLogEntryDlg","QwGUIDataWindow",
					     &dHCLogEntries,400, 200);
  if(dHCLogEntries.setFlag){

    TString hcpost;
    TString contentfile = Form("%s/Extensions/GUI/hcpostcomments.txt",gSystem->Getenv("QWANALYSIS"));
    TString attachment = Form("%s/Extensions/GUI/TempHClogAttachment.png",gSystem->Getenv("QWANALYSIS"));
    RDataContainer *tempfile = new RDataContainer(fClient->GetRoot(),this,"tempfile",
						  "QwGUIDataWindow","",FM_WRITE,FT_TEXT);

    hcpost = "hclog_post ";
    if(dHCLogEntries.name.Length())
      hcpost += WrapParameter("author", dHCLogEntries.name);
    if(dHCLogEntries.emaillist.Length())
      hcpost += WrapParameter("emailto",dHCLogEntries.emaillist);
    hcpost += "--tag=\"This is logged using hclog_post by QwGUI\" ";
    hcpost += "  --cleanup ";
    
    if(dHCLogEntries.subject.Length())
      hcpost += WrapParameter("subject",MakeSubject(dHCLogEntries.subject));
    
    tempfile->OpenFile(contentfile);
    tempfile->WriteData(dHCLogEntries.comments.Data(),strlen(dHCLogEntries.comments.Data()));
    tempfile->Close();
    tempfile = NULL;

    if(contentfile.Length())
      hcpost += WrapParameter("textfile", contentfile);
    SaveCanvas(attachment.Data());
    hcpost += WrapAttachment(attachment.Data());

    gSystem->Exec(hcpost.Data());

  }
  dHCLogEntryDlg = NULL;
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
	dOptionsDlg = new QwGUIPlotOptionsDialog(fClient->GetRoot(), 0,
						 "dOptionsDlg","QwGUIDataWindow",
						 &dOptions,400, 200);
	if(dOptions.limitsFlag) UpdatePlot(GetObjectName());
	break;

      case M_HCLOG_SUBM:
	SubmitToHCLog();
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




