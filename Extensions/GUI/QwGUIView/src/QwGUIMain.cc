/*------------------------------------------------------------------------*//*!

 \defgroup QwGUIMain QwGUIMain

 \section myoverview Overview of the Qweak analysis GUI frontend

    This group of classes is meant to implement a basic GUI frontend with
    a tab environment for each major subsystem, which, for the moment, will
    read the ROOT file generated by the main analysis program.

    The GUI is based on ROOT classes and a set of utility classes contained
    in a library called QwGUIRootLib-vers.a, where "vers" is the current
    subversion assigned version of the overall QwAnalyzer software.

    Things are meant to be prepared in such a way, as to make it easy for
    other people to implement their own sub system specific data display.
    One can follow the implementation of the main detector subsystem as an
    example.

*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwGUIMain.cc
 \author Michael Gericke

 \brief main(...) function and MainFrame class for the QwGUI executable


*//*-------------------------------------------------------------------------*/

#include <QwGUIMain.h>

QwGUIMain::QwGUIMain(const TGWindow *p, ClineArgs clargs, UInt_t w, UInt_t h)
  : TGMainFrame(p, w, h)
{

  MCnt = 0;
  dClArgs = clargs;
  std::set_new_handler(0);

  MainDetSubSystem   = NULL;
  LumiDetSubSystem   = NULL;
  InjectorSubSystem  = NULL;

  dMWWidth           = w;
  dMWHeight          = h;
  dCurRun            = 0;

  dTBinEntryLayout   = NULL;
  dTBinEntry         = NULL;
  dUtilityFrame      = NULL;
  dUtilityLayout     = NULL;
  dHorizontal3DLine  = NULL;

  dTabLayout         = NULL;

  dMainCanvas        = NULL;
  dMainTabFrame      = NULL;
  dMainCnvFrame      = NULL;
  dMainCnvLayout     = NULL;
  dMainTabLayout     = NULL;

  dLogTabLayout      = NULL;
  dLogTabFrame       = NULL;
  dLogEditLayout     = NULL;
  dLogText           = NULL;
  dLogEdit           = NULL;

  dMenuBar           = NULL;
  dMenuBarLayout     = NULL;
  dMenuBarItemLayout = NULL;
  dMenuBarHelpLayout = NULL;
  dMenuFile          = NULL;
  dMenuHelp          = NULL;


  memset(dLogfilename,'\0',sizeof(dLogfilename));

  SetRootFileOpen(kFalse);
  SetLogFileOpen(kFalse);
  SetLogFileName("None");

  MakeMenuLayout();
  MakeUtilityLayout();
  MakeMainTab();
  MakeLogTab();

  SetWindowName("Qweak Data Analysis GUI");


  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();


  if(!GetSubSystemPtr("Main Detectors"))
    MainDetSubSystem = new QwGUIMainDetector(fClient->GetRoot(), this, dTab,"Main Detectors",
					     "QwGUIMain", dMWWidth-15,dMWHeight-180);
  if(!GetSubSystemPtr("Lumi Detectors"))
    LumiDetSubSystem = new QwGUILumiDetector(fClient->GetRoot(), this, dTab,"Lumi Detectors",
					     "QwGUIMain", dMWWidth-15,dMWHeight-180);
  if(!GetSubSystemPtr("Injector"))
    InjectorSubSystem = new QwGUIInjector(fClient->GetRoot(), this, dTab,"Injector",
					  "QwGUIMain", dMWWidth-15,dMWHeight-180);
  if(!GetSubSystemPtr("Event Display"))
    EventDisplaySubSystem = new QwGUIEventDisplay(fClient->GetRoot(), this, dTab, "Event Display",
					  "QwGUIMain", dMWWidth-15, dMWHeight-180);

}

QwGUIMain::~QwGUIMain()
{
  delete dTab;
  delete dMainTabFrame;
  delete dMainCnvFrame;
  delete dMainCanvas;

  delete dMainCnvLayout;
  delete dMainTabLayout;
  delete dTabLayout;

  delete dMenuBar;
  delete dMenuFile,
  delete dMenuHelp;
  delete dMenuBarLayout;
  delete dMenuBarItemLayout;
  delete dMenuBarHelpLayout;

  delete dTBinEntryLayout;
  delete dTBinEntry;
  delete dUtilityFrame;
  delete dUtilityLayout;
  delete dHorizontal3DLine;

  delete dLogText;
  delete dLogEdit;
  delete dLogTabFrame;
  delete dLogTabLayout;
  delete dLogEditLayout;

  delete dROOTFile;
}

void QwGUIMain::MakeMenuLayout()
{
  dMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
				     0, 0, 1, 1);
  dMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  dMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

  dMenuFile = new TGPopupMenu(fClient->GetRoot());
//   dMenuFile->AddEntry("&Open (Run file)...", M_FILE_OPEN);
  dMenuFile->AddEntry("O&pen (ROOT file)...", M_ROOT_FILE_OPEN);
//   dMenuFile->AddEntry("&Close (Run file)", M_FILE_CLOSE);
  dMenuFile->AddEntry("C&lose (ROOT file)", M_ROOT_FILE_CLOSE);
  dMenuFile->AddSeparator();

  dMenuFile->AddEntry("Root File Browser", M_VIEW_BROWSER);
  dMenuFile->AddSeparator();
  dMenuFile->AddSeparator();
  dMenuFile->AddEntry("E&xit", M_FILE_EXIT);

  dMenuTabs = new TGPopupMenu(fClient->GetRoot());
  dMenuTabs->AddEntry("View Log", M_VIEW_LOG);

  dMenuHelp = new TGPopupMenu(fClient->GetRoot());
  dMenuHelp->AddEntry("&User manual", M_HELP_USER);
  dMenuHelp->AddEntry("&Code manual", M_HELP_CODE);
  dMenuHelp->AddEntry("View &change history", M_HELP_SEARCH);
  dMenuHelp->AddSeparator();
  dMenuHelp->AddEntry("This is revision " VERS, M_HELP_ABOUT);

  dMenuFile->Associate(this);
  dMenuTabs->Associate(this);
  dMenuHelp->Associate(this);

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&File", dMenuFile, dMenuBarItemLayout);
  dMenuBar->AddPopup("&Tabs", dMenuTabs, dMenuBarItemLayout);
  dMenuBar->AddPopup("&Help", dMenuHelp, dMenuBarHelpLayout);

  AddFrame(dMenuBar, dMenuBarLayout);
  dMenuTabs->CheckEntry(M_VIEW_LOG);
}

void QwGUIMain::MakeUtilityLayout()
{

//   dUtilityLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2,2,2,2);
//   dTBinEntryLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2);
//   dRunEntryLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);

//   dHorizontal3DLine = new TGHorizontal3DLine(this);
//   AddFrame(dHorizontal3DLine, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

//   dUtilityFrame = new TGHorizontalFrame(this,60,10);
//   dTBinEntry = new TGComboBox(dUtilityFrame,M_TBIN_SELECT);
//   dTBinEntry->Associate(this);
//   dUtilityFrame->AddFrame(dTBinEntry,dTBinEntryLayout);
// //   for (int i = 0; i < VME2_MP_SIZE; i++){
// //     tof = GetTBinWidth()*(i+0.5)+GetMPOffset();
// //     eng = pow((21030.0/438/tof),2);
// //     sprintf(dTOFBINStrings[i],"Bin %03d: tof %6.2f ms; Eng %9.2f meV", i, tof, eng);
// //     dTBinEntry->AddEntry(dTOFBINStrings[i], i+1);
// //   }
//   dTBinEntry->Resize(280, 20);

//   if(!dClArgs.realtime){
//     dRunEntry = new TGNumberEntry(dUtilityFrame,GetCurrentRunNumber(),6,M_RUN_SELECT,
// 				  TGNumberFormat::kNESInteger,
// 				  TGNumberFormat::kNEANonNegative,
// 				  TGNumberFormat::kNELLimitMinMax,1,999999);
//     if(dRunEntry){
//       dRunEntryLabel = new TGLabel(dUtilityFrame,"Run Number:");
//       if(dRunEntryLabel){
// 	dUtilityFrame->AddFrame(dRunEntryLabel,dRunEntryLayout);
//       }
//       dRunEntry->Associate(this);
//       dUtilityFrame->AddFrame(dRunEntry,dRunEntryLayout);
//     }
//   }

//   if(dClArgs.realtime){
//     const TGPicture *ipic =(TGPicture *)gClient->GetPicture("realtime.xpm");
//     TGIcon *icon = new TGIcon(dUtilityFrame,ipic,500,40);
//     dUtilityFrame->AddFrame(icon,new TGLayoutHints(kLHintsLeft | kLHintsBottom,1,15,1,1));
//   }

//   AddFrame(dUtilityFrame,dUtilityLayout);
}

void QwGUIMain::MakeMainTab()
{

  dTabLayout = new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsExpandY,
				 2, 2, 5, 1);
  dTab = new TGTab(this,dMWWidth-15,dMWHeight-80);

  if(TabActive("Main")) return;

  dMainTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop |
				     kLHintsExpandX | kLHintsExpandY);
  dMainCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				     0, 0, 1, 2);

  TGCompositeFrame *tf = dTab->AddTab("Main");

  dMainTabFrame = new TGHorizontalFrame(tf,10,10);
  dMainCanvas   = new TRootEmbeddedCanvas("pC", dMainTabFrame,10, 10);
  dMainTabFrame->AddFrame(dMainCanvas,dMainCnvLayout);
  dMainTabFrame->Resize(dMWWidth-15,dMWHeight-110);
  tf->AddFrame(dMainTabFrame,dMainTabLayout);
  AddFrame(dTab, dTabLayout);

  dMainCanvas->GetCanvas()->SetBorderMode(0);
  dMainCanvas->GetCanvas()->Connect("Picked(TPad*, TObject*, Int_t)","QwGUIMain",
  				    this,"PadIsPicked(TPad*, TObject*, Int_t)");
  dMainCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
				    "QwGUIMain",
				    this,"MainTabEvent(Int_t,Int_t,Int_t,TObject*)");
}

void QwGUIMain::MakeLogTab()
{

  TGCompositeFrame *tf = dTab->AddTab("Log Book");

  dLogTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop |
				    kLHintsExpandX | kLHintsExpandY);
  dLogEditLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
				     0, 0, 1, 2);

  dLogTabFrame  = new TGHorizontalFrame(tf,10,10);
  dLogEdit = new TGTextEdit(dLogTabFrame, 10, 10, kSunkenFrame);
  dLogTabFrame->AddFrame(dLogEdit,dLogEditLayout);
  dLogTabFrame->Resize(dMWWidth-15,dMWHeight-80);
  tf->AddFrame(dLogTabFrame,dLogTabLayout);

  dTab->MapSubwindows();
  dTab->Layout();

  dLogEdit->Connect("Opened()","QwGUIMain", this, "LogOpened()" );
  dLogEdit->Connect("Closed()","QwGUIMain", this, "LogClosed()" );
  dLogEdit->Connect("Saved()","QwGUIMain", this,  "LogSaved()"  );
  dLogEdit->Connect("SavedAs()","QwGUIMain", this,"LogSavedAs()");

  if(dLogText) {
    dLogEdit->SetText(dLogText);
    dLogText = NULL;
  }
  else{
    sprintf(dMiscbuffer,"Revision " VERS "\n"
	    "New Session Started: %s at %s",GetDate(),GetTime());
    Append(dMiscbuffer);
  }
  dMenuTabs->CheckEntry(M_VIEW_LOG);
}

void QwGUIMain::RemoveTab(QwGUISubSystem* sbSystem)
{
  if(!sbSystem) return;
  const char *TabName = sbSystem->GetName();
  if(!TabActive(TabName)) return;
  int tab = GetTabIndex(TabName);
  if(tab < 0) return;

  dMenuTabs->UnCheckEntry(GetTabMenuID(TabName));
  sbSystem->TabMenuEntryChecked(kFalse);
  UnMapLayout(tab);
}

void QwGUIMain::AddATab(QwGUISubSystem* sbSystem)
{
  if(!dMenuTabs) return;
  if(!sbSystem) return;

  int flag = 0;
  TString s = sbSystem->GetName();
  TObject *obj;
  TIter next(dMenuTabs->GetListOfEntries());

  //Sequence naming of menu items not currently implemented/useful
  //
  //int seq = 1;
  //char sequence[100];
  //   while(!flag){
  //     flag = 1;
  //     while(obj = next()){
  //       TGMenuEntry *entry = (TGMenuEntry*)obj;
  //       if(s == entry->GetLabel()->GetString()){
  // 	if(seq >= 2) s.Resize(s.Length()-3);
  // 	sprintf(sequence," % 2d",seq); seq++;
  // 	s += sequence;
  // 	flag = 0;
  // 	break;
  //       }
  //     }
  //     next.Reset();
  //   }
  //   MCnt++;
  //   dMenuTabs->AddEntry(s, M_TABS+MCnt);

  obj = next();
  while(obj){
    TGMenuEntry *entry = (TGMenuEntry*)obj;
    //       printf("%s %d\n",entry->GetLabel()->GetString(),entry->GetEntryId());
    if(s == entry->GetLabel()->GetString()){
      flag = 1;
      break;
    }
    obj = next();
  }

  if(!flag){
    Long_t mID = GetNewTabMenuID();
    dMenuTabs->AddEntry(s,mID);
    sbSystem->SetTabMenuID(mID);
    SubSystemArray.Add(sbSystem);
    sbSystem->SubSystemLayout();
  }

  if(TabActive(s.Data())) return;

  dTab->AddTab(s.Data(),sbSystem);
  dMenuTabs->CheckEntry(GetTabMenuID(s.Data()));
  sbSystem->TabMenuEntryChecked(kTrue);
  MapLayout();
}

Int_t QwGUIMain::GetTabMenuID(const char* TabName)
{
  if(!dMenuTabs) return 0;

  TString s = TabName;
  TObject *obj;
  TIter next(dMenuTabs->GetListOfEntries());

  obj = next();
  while(obj){
    TGMenuEntry *entry = (TGMenuEntry*)obj;
    //       printf("%s %d\n",entry->GetLabel()->GetString(),entry->GetEntryId());
    if(s == entry->GetLabel()->GetString()) return entry->GetEntryId();
    obj = next();
  }

  return 0;
}

const char* QwGUIMain::GetTabMenuLabel(Long_t mID)
{
  if(!dMenuTabs) return 0;

  TString s;
  TObject *obj;
  TIter next(dMenuTabs->GetListOfEntries());

  obj = next();
  while(obj){
    TGMenuEntry *entry = (TGMenuEntry*)obj;
    //       printf("%s %d\n",entry->GetLabel()->GetString(),entry->GetEntryId());
    if(mID == entry->GetEntryId()) {
      s = entry->GetLabel()->GetString();
      return s.Data();
    }
    obj = next();
  }

  return 0;
}


Bool_t QwGUIMain::TabActive(const char *str)
{
  int nt = dTab->GetNumberOfTabs();
  for (int i = 0 ; i < nt; i++) {
    TString s = dTab->GetTabTab(i)->GetString();
    if (s == str) {return kTrue;}
  }
  return kFalse;
}

Int_t QwGUIMain::GetTabIndex(const char *str)
{
  TString s;
  TString label = str;
  int nt = dTab->GetNumberOfTabs();
  for (int i = 0 ; i < nt; i++) {
    s = dTab->GetTabTab(i)->GetString();
    if (s == label) {return i;}
  }
  return -1;
}

Char_t *QwGUIMain::GetTime()
{
  time_t *cutime;
  tm *ltime;
  cutime = new time_t;
  time(cutime);
  ltime = localtime((const time_t*)cutime);
  if(ltime->tm_sec < 10)
    sprintf(dTime,"%d:%d:0%d",ltime->tm_hour,ltime->tm_min,ltime->tm_sec);
  else if(ltime->tm_min < 10)
    sprintf(dTime,"%d:0%d:%d",ltime->tm_hour,ltime->tm_min,ltime->tm_sec);
  else if(ltime->tm_hour < 10)
    sprintf(dTime,"0%d:%d:%d",ltime->tm_hour,ltime->tm_min,ltime->tm_sec);
  else
    sprintf(dTime,"%d:%d:%d",ltime->tm_hour,ltime->tm_min,ltime->tm_sec);

  delete cutime; cutime = NULL;
  return dTime;
}

Char_t *QwGUIMain::GetDate()
{
  time_t cutime;
  time(&cutime);
  tm *date = localtime((const time_t*)&cutime);
  sprintf(dDate,"%d-%d-%d",date->tm_mon+1,date->tm_mday,date->tm_year+1900);
  return dDate;
}

void QwGUIMain::RemoveLogTab()
{

  if(!TabActive("Log Book")) return;
  int tab = GetTabIndex("Log Book");
  if(tab < 0) return;
  UnMapLayout(tab);


  if(dLogEdit->GetText()->GetLongestLine() > 0)
    dLogText = new TGText(dLogEdit->GetText());
  else
    dLogText = NULL;
  delete dLogEdit; dLogEdit = NULL;
  delete dLogTabFrame; dLogTabFrame = NULL;
  delete dLogTabLayout; dLogTabLayout = NULL;
  delete dLogEditLayout; dLogEditLayout = NULL;

  dMenuTabs->UnCheckEntry(M_VIEW_LOG);
}


TCanvas *QwGUIMain::SplitCanvas(TRootEmbeddedCanvas *cnv, int r,int c, const char* ttip)
{
  TCanvas *mc = cnv->GetCanvas();
  if(!mc) return NULL;
  mc->Clear();
  mc->Update();
  if(r != 0 && c != 0){
    mc->Divide(r,c,1e-3,1e-3);
    for(int i = 0; i < (int)(r*c); i++){
      mc->cd(i+1);
      if(ttip){
	sprintf(dMiscbuffer,"%s %02d",ttip,i);
	gPad->SetToolTipText(dMiscbuffer,250);
      }
    }
  }
  return mc;
}

void QwGUIMain::LogOpened()
{
  SetLogFileOpen(kTrue);
  Bool_t untitled = !strlen(dLogEdit->GetText()->GetFileName()) ? kTrue : kFalse;
  if(!untitled) SetLogFileName((char*)dLogEdit->GetText()->GetFileName());
}

void QwGUIMain::LogClosed()
{
  SetLogFileOpen(kFalse);
  SetLogFileName("");
}

void QwGUIMain::LogSaved()
{
  SetLogFileOpen(kTrue);
  Bool_t untitled = !strlen(dLogEdit->GetText()->GetFileName()) ? kTrue : kFalse;
  if(!untitled) SetLogFileName((char*)dLogEdit->GetText()->GetFileName());
}

void QwGUIMain::LogSavedAs()
{
  SetLogFileOpen(kTrue);
  Bool_t untitled = !strlen(dLogEdit->GetText()->GetFileName()) ? kTrue : kFalse;
  if(!untitled) SetLogFileName((char*)dLogEdit->GetText()->GetFileName());
}

void QwGUIMain::PadIsPicked(TPad* selpad, TObject* selected, Int_t event)
{
  //printf("Pad number = %d\n",selpad->GetNumber());
}

void QwGUIMain::MainTabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
//   if(event == kButton1Double){
//     Int_t pad = dMainCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
//   }
}


// Wait, but continue to process events.
void QwGUIMain::SleepWithEvents(int seconds)
{
  time_t start, now;
  time(&start);
  do {
    time(&now);
    gSystem->ProcessEvents();
  } while (difftime(now,start) < seconds) ;
}

QwGUISubSystem *QwGUIMain::GetSubSystemPtr(const char *name)
{

  TObject *obj;
  TIter next(SubSystemArray.MakeIterator());
  obj = next();
  while(obj){
    QwGUISubSystem *entry = (QwGUISubSystem*)obj;
    if(!strcmp(name,entry->GetName())){
      return entry;
    }
    obj = next();
  }

  return 0;

}

// void QwGUIMain::PlotMainData(ERPlotTypes ptype)
// {
// }

void QwGUIMain::OnLogMessage(const char *msg)
{
  Append(msg,kFalse);
}

void QwGUIMain::OnObjClose(const char *objname)
{
  if(!objname) return;

  TString name = objname;

  if(name.Contains("dROOTFile")){
    dROOTFile = NULL;
#ifdef QWGUI_DEBUG
    printf("Received dROOTFile IsClosing signal\n");
#endif
  }

//   TObject *obj;
//   TIter next(SubSystemArray.MakeIterator());
//   obj = next();
//   while(obj){
//     QwGUISubSystem *entry = (QwGUISubSystem*)obj;
//     if(!strcmp(objname,entry->GetName())){
//       SubSystemArray.Remove(entry);
//     }
//     obj = next();
//   }
}

void QwGUIMain::OnReceiveMessage(const char *obj)
{
  TString name = obj;
  const char *ptr = NULL;

  QwGUISubSystem* sbSystem = GetSubSystemPtr(obj);
  if(sbSystem){

    ptr = sbSystem->GetMessage();
    if(ptr)
      Append(ptr,sbSystem->IfTimeStamp());
  }
  if(name.Contains("dROOTFile")){

    ptr = dROOTFile->GetMessage();
    if(ptr)
      Append(ptr,kTrue);

  }
}

// void QwGUIMain::PlotCurrentTab()
// {
// //   TString s = dTab->GetCurrentTab()->GetString();
// //   Int_t tab = GetTabIndex(dTab->GetCurrentTab()->GetString());

// //   if(s == "Spin Flipper")
// //     PlotSpinFlipperData(GetCurrentPlotType(tab),GetCurrentDataType(tab));
// //   if(s == "Detectors")
// //     PlotDetectorData(GetCurrentPlotType(tab),GetCurrentDataType(tab));
// //   if(s == "Monitors")
// //     PlotDetectorData(GetCurrentPlotType(tab),GetCurrentDataType(tab));

// }


Int_t QwGUIMain::GetFilenameFromDialog(char *file, const char *ext,
				      ERFileStatus status, Bool_t kNotify,
				      const char *notifytext)
{
  int retval = 0;

  if(kNotify && notifytext){

    new TGMsgBox(fClient->GetRoot(), this,"File Open Operation",
		 notifytext,kMBIconQuestion, kMBOk | kMBCancel, &retval);
    if(retval == kMBCancel) return PROCESS_FAILED;
  }

  if(!ext || !*ext) return PROCESS_FAILED;
  if(!file) return PROCESS_FAILED;
  TString fext = ext;
  const char *filetypes[12];
  Int_t index = 0;


  if(fext.Contains("dat")){
     filetypes[index] = "Data files"; index++;
     filetypes[index] = "*.dat"; index++;
  }
  if(fext.Contains("txt")){
     filetypes[index] = "Data files"; index++;
     filetypes[index] = "*.txt"; index++;
  }
  if(fext.Contains("csv")){
     filetypes[index] = "Data files"; index++;
     filetypes[index] = "*.csv"; index++;
  }
  if(fext.Contains("root")){
     filetypes[index] = "Root files"; index++;
     filetypes[index] = "*.root"; index++;
  }
  if(fext.Contains("log")){
     filetypes[index] = "Log files"; index++;
     filetypes[index] = "*.log"; index++;
  }
  filetypes[index] = "All files"; index++;
  filetypes[index] = "*"; index++;
  filetypes[index] = 0; index++;
  filetypes[index] = 0; index++;

  sprintf(dMiscbuffer2,"/home/%s/scratch",gSystem->Getenv("USER"));
  static TString dir(dMiscbuffer2);
  TGFileInfo fi;

  if(status == FS_OLD){
    fi.fFileTypes = (const char **)filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
    dir = fi.fIniDir;
    if(!fi.fFilename) {return PROCESS_FAILED;};
  }

  if(status == FS_NEW){
    fi.fFileTypes = (const char **)filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);
    dir = fi.fIniDir;

    if(!fi.fFilename) {return PROCESS_FAILED;};
  }
  strcpy(file,fi.fFilename);
  return PROCESS_OK;
}

Int_t QwGUIMain::SaveLogFile(ERFileStatus status, const char* file)
{
  if(!IsLogFileOpen()) return PROCESS_FAILED;
  char filename[NAME_STR_MAX];
  if(!file){
    if(GetFilenameFromDialog(filename,"log",status) == PROCESS_FAILED)
      return PROCESS_FAILED;
  }
  else
    strcpy(filename,file);

  if(strcmp(".log",strrchr(filename,'.'))) return PROCESS_FAILED;

  if(!dLogEdit) return PROCESS_FAILED;
  if(!dLogEdit->SaveFile(filename))
    return PROCESS_FAILED;

  return PROCESS_OK;
}

Int_t QwGUIMain::OpenLogFile(ERFileStatus status, const char* file)
{
  if(IsLogFileOpen()) CloseLogFile();
  char filename[NAME_STR_MAX];
  if(!file){
    if(GetFilenameFromDialog(filename,"log",status) == PROCESS_FAILED)
      return PROCESS_FAILED;
  }
  else
    strcpy(filename,file);

  if(strcmp(".log",strrchr(filename,'.'))) return PROCESS_FAILED;

  if(!TabActive("Log Book") || !dLogEdit) MakeLogTab();
  if(!dLogEdit) return PROCESS_FAILED;
  if(!dLogEdit->LoadFile(filename))
    if(!dLogEdit->SaveFile(filename))
      return PROCESS_FAILED;

  dLogEdit->SaveFile(filename);
  SetLogFileOpen(kTrue);
  SetLogFileName(filename);
  return PROCESS_OK;
}

Int_t QwGUIMain::OpenRootFile(ERFileStatus status, const char* file)
{

  if(IsRootFileOpen()) CloseRootFile();
  char filename[NAME_STR_MAX];
  if(!file){
    if(GetFilenameFromDialog(filename,"root",status) == PROCESS_FAILED)
      return PROCESS_FAILED;
  }
  else
    strcpy(filename,file);

  if(!strstr(filename,".root")) strcat(filename,".root");

  dROOTFile = new RDataContainer(fClient->GetRoot(), this,
				 "dROOTFile","QwGUIMain",
				 "ROOT",FM_UPDATE,FT_ROOT);

  if(!dROOTFile){SetRootFileOpen(kFalse); return PROCESS_FAILED;}

  if(dROOTFile->OpenFile(filename) != FILE_PROCESS_OK) {
    SetRootFileOpen(kFalse);
    dROOTFile->Close();
    dROOTFile = NULL;
    return PROCESS_FAILED;
  }

  dMenuFile->DisableEntry(M_ROOT_FILE_OPEN);
  TObject *obj;
  TIter next(SubSystemArray.MakeIterator());
  obj = next();
  while(obj){
    QwGUISubSystem *entry = (QwGUISubSystem*)obj;
    entry->SetDataContainer(dROOTFile);
    obj = next();
  }

  SetRootFileOpen(kTrue);
  SetRootFileName(filename);
  return PROCESS_OK;
}


// Int_t QwGUIMain::OpenRun(Bool_t kCalculate, Bool_t kSilent)
// {
//   return PROCESS_FAILED;
// }


// void QwGUIMain::CloseRun()
// {
//   SetRunOpen(kFalse);
// }

void QwGUIMain::CloseRootFile()
{

  if(dROOTFile != NULL){
    dROOTFile->Close(kFalse);
    dROOTFile = NULL;
  }
  SetRootFileOpen(kFalse);
  dMenuFile->EnableEntry(M_ROOT_FILE_OPEN);

}

void QwGUIMain::CloseLogFile()
{
  if(TabActive("Log Book") && dLogEdit){
//     SendMessage(dLogEdit,MK_MSG(kC_COMMAND,kCM_MENU),TGTextEdit::kM_FILE_CLOSE,0);
    dLogEdit->Clear();
    dLogText = NULL;
    dLogEdit->Closed();
  }
  SetLogFileOpen(kFalse);
  SetLogFileName("");
}

Int_t QwGUIMain::Append(const char *buffer,Bool_t T_Stamp)
{
  int size = strlen(buffer)+200;
  int lnum = 0;
  int cutoff = 110;//64;
  const char *ptr;
  char *line;
  int  index;
  char *tmpbuffer = new char[size];
  TGLongPosition pos;
  pos.fX = 0;
  TGText * cText = NULL;

  if(!tmpbuffer){
    return LOG_PROCESS_ERROR;
  }
  memset(tmpbuffer,'\0',size);

  if(dLogEdit) {
    cText = new TGText(dLogEdit->GetText());
  }
  else if(dLogText) {
    cText = dLogText;
  }
  else {
    dLogText = new TGText();
    cText = dLogText;
  }

  if(!cText) return LOG_PROCESS_ERROR;

  ptr = buffer;  index = 0;
  lnum = 0;
  for(int i = 0; i <= (int)strlen(buffer); i ++)
    {
      if(buffer[i] == '\n' || i-index > cutoff)
	{
	  line = new char[i-index+1];
	  memset(line,'\0',i-index+1);
	  strncpy(line,ptr,i-index);
	  if(lnum == 0 && T_Stamp)
	    sprintf(tmpbuffer,"%s     %s",GetTime(),line);
	  else
	    sprintf(tmpbuffer,"             %s",line);
	  pos.fY = cText->RowCount();
	  cText->InsText(pos, tmpbuffer);
	  lnum++;

	  delete[] line;
	  if(buffer[i] == '\n'){
	    index = i+1;
	    ptr = &buffer[i+1];
	  }
	  else{
	    index = i;
	    ptr = &buffer[i];
	  }
	}
      else if(buffer[i] == '\0')
	{
	  if(lnum == 0 && T_Stamp)
	    sprintf(tmpbuffer,"%s     %s",GetTime(),ptr);
	  else
	    sprintf(tmpbuffer,"             %s",ptr);
// 	  sprintf(tmpbuffer,"             %s",ptr);
	  pos.fY = cText->RowCount();
	  cText->InsText(pos, tmpbuffer);

	}
    }

  pos.fY = cText->RowCount();
  cText->InsText(pos, "");

  if(dLogEdit){
    dLogEdit->SetText(cText);
    dLogEdit->Goto(cText->RowCount(),
		   cText->GetLineLength((Long_t)(cText->RowCount()-1)));
  }
  delete [] tmpbuffer;
  return LOG_PROCESS_OK;
}

Int_t QwGUIMain::WriteLogData(const char *filename)
{
  if(!IsLogFileOpen()){if(OpenLogFile(FS_NEW,filename) != PROCESS_OK)
    return PROCESS_FAILED;}
  if(!TabActive("Log Book")) return PROCESS_FAILED;
  if(!dLogEdit) return PROCESS_FAILED;
  if(!dLogEdit->SaveFile(filename))
    return PROCESS_FAILED;

  return PROCESS_OK;
}

Int_t QwGUIMain::WriteRootData()
{
//   Int_t retval = 0;

//   if(!IsRootFileOpen()){if(OpenRootFile(FS_NEW) != PROCESS_OK) return PROCESS_FAILED;}

// //   if(dMsgBox7 != NULL) {dMsgBox7->CloseWindow(); dMsgBox7 = NULL;}

//   if(IsRootFileOpen()){
//     dROOTFile->cd();
//     if(dROOTFile->WriteData((TObject*)&AnlObj) != FILE_PROCESS_OK){
//       sprintf(dMiscbuffer2,
// 	      "Can't write objects to file %s\nWrite Process Stopped!"
// 	      ,dROOTFile->GetFileName());
// //       dMsgBox7 = new RMsgBox(fClient->GetRoot(), this, "dMsgBox7", "QwGUIMain",
// // 			     "File Write Error",dMiscbuffer2,kMBIconExclamation,
// // 			     kMBOk);
//       return PROCESS_FAILED;
//     }
//   }
//   else {
//     strcpy(dMiscbuffer2,"Can't Create Data Container\nFor Analysis Output File!");
//     new TGMsgBox(fClient->GetRoot(), this, "Memory Error",
// 		 dMiscbuffer2,kMBIconExclamation, kMBOk, &retval);
//     strcpy(dMiscbuffer2,"Can't Store Analysis Results On File!");

// //     dMsgBox7 = new RMsgBox(fClient->GetRoot(), this, "dMsgBox7", "QwGUIMain",
// // 			   "File Write Error",dMiscbuffer2,kMBIconExclamation,
// // 			   kMBOk);
//     return PROCESS_FAILED;
//   }
  return PROCESS_OK;
}


// void QwGUIMain::InitRunProgressDlg(const char* title, Int_t nruns, Int_t mps, Int_t mps2, Bool_t stopable,
// 				  const char* macrotitle, const char* microtitle, const char* microtitle2)
// {
//   char macro[NAME_STR_MAX];
//   char micro[NAME_STR_MAX];
//   char micro2[NAME_STR_MAX];
//   Int_t n = 0;

//   if(nruns) { n++; if(!macrotitle) strcpy(macro,"Runs Completed"); else strcpy(macro,macrotitle);}
//   if(mps  ) { n++; if(!microtitle) strcpy(micro,"T0's completed"); else strcpy(micro,microtitle);}
//   if(mps2 ) { n++; if(!microtitle2) strcpy(micro2,"Beam T0's completed"); else strcpy(micro2,microtitle2);}

//   dMainPrgrDlg = new NProgressDialog(fClient->GetRoot(), this,
// 				     "dMainPrgrDlg","QwGUIMain",
// 				     title,macro,micro,micro2,nruns,mps,mps2,
// 				     600,300,stopable,n);
//   dProcessHalt = kFalse;
// //   gSystem->ProcessEvents();
// }

// void QwGUIMain::IncreaseProgress(Int_t *run, Int_t *mp, Int_t *mp2, Int_t rinc, Int_t mpinc, Int_t mpinc2)
// {
//   if(dMainPrgrDlg){
//     if(run){
//       if(*run >= rinc){
// 	SendMessage(dMainPrgrDlg,
// 		    MK_MSG((EWidgetMessageTypes)kC_PR_DIALOG,
// 			   (EWidgetMessageTypes)kCM_PR_MSG),
// 		    M_PR_RUN,*run);
// 	gSystem->ProcessEvents();
// 	*run = 0;
//       }
//     }
//     if(mp){
//       if(*mp >= mpinc){
// 	SendMessage(dMainPrgrDlg,
// 		    MK_MSG((EWidgetMessageTypes)kC_PR_DIALOG,
// 			   (EWidgetMessageTypes)kCM_PR_MSG),
// 		    M_PR_SEQ,*mp);
// 	gSystem->ProcessEvents();
// 	*mp = 0;
//       }
//     }
//     if(mp2){
//       if(*mp2 >= mpinc2){
// 	SendMessage(dMainPrgrDlg,
// 		    MK_MSG((EWidgetMessageTypes)kC_PR_DIALOG,
// 			   (EWidgetMessageTypes)kCM_PR_MSG),
// 		    M_PR_SEQ2,*mp2);
// 	gSystem->ProcessEvents();
// 	*mp2 = 0;
//       }
//     }
//   }
// }

void QwGUIMain::CloseWindow()
{
  // Got close message for this MainFrame. Terminate the application
  // or returns from the TApplication event loop (depending on the
  // argument specified in TApplication::Run()).

  // Don't remove the PID file if it's not ours.
//   if(FILE *fp=fopen("QwGUID_PID.DAT","r")) {
//     int pid=int(getpid()), infile=0;
//     fscanf(fp,"%d", &infile);   // don't care if we're successful or not
//     fclose(fp);
//     if (pid==infile) {
//       sprintf(dMiscbuffer,"rm %s","QwGUID_PID.DAT");
//       system(dMiscbuffer);
//     } else
//       printf("%s:%d: Not removing %d's PID file.\n",__FILE__,__LINE__,infile);
//   } else
//     printf ("%s:%d: No PID file to remove.\n",__FILE__,__LINE__);

//   CloseRun();
  gApplication->Terminate(0);

}

Bool_t QwGUIMain::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  // Handle messages send to the MainFrame object. E.g. all menu button
  // messages.

  Long_t mTabID = 0;

  switch (GET_MSG(msg)){

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_ENTER:
      switch (parm1) {

      case M_RUN_SELECT:
	break;

      default:
	break;
      }

    default:
      break;
    }

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {

    case kCM_COMBOBOX:
      {
	switch (parm1) {
	case M_TBIN_SELECT:
	  break;
	}
      }
      break;

    case kCM_MENUSELECT:
      break;

    case kCM_MENU:

      for(int n = M_TABS; n <= M_TABS+MCnt; n++ ){
	mTabID = n;
	if(parm1 == mTabID){

	  if(dMenuTabs->IsEntryChecked(mTabID)){
	    RemoveTab((QwGUISubSystem*)dTab->GetTabContainer(GetTabIndex(GetTabMenuLabel(mTabID))));
	  }
	  else{

	    TObject *obj;
	    TIter next(SubSystemArray.MakeIterator());
	    obj = next();
	    while(obj){
	      QwGUISubSystem *entry = (QwGUISubSystem*)obj;
	      if(!strcmp(GetTabMenuLabel(mTabID),entry->GetName()))
		AddATab(entry);
	      obj = next();
	    }
	  }

	  break;
	}
      }

      switch (parm1) {

      case M_ROOT_FILE_OPEN:
	OpenRootFile();
	break;

      case M_FILE_OPEN:
// 	OpenRun();
	break;

      case M_OPEN_DATAWINDOW:
	break;

      case M_FILE_EXIT:
	CloseWindow();   // this also terminates theApp
	break;

      case M_ROOT_FILE_CLOSE:
	if(IsRootFileOpen()) CloseRootFile();
	break;

      case M_FILE_CLOSE:
// 	CloseRun();
// 	SplitCanvas(dMainCanvas,0,0,NULL);
	break;

      case M_VIEW_LOG:
	if(dMenuTabs->IsEntryChecked(M_VIEW_LOG)){
	  RemoveLogTab();
	}
	else{
	  MakeLogTab();
	}
	break;

      case M_VIEW_BROWSER:
// 	OpenRootBrowser();
	break;

      case M_HELP_USER:
	// Open the online help manual in a web browser.
        {
	  dHelpBrowser = new QwGUIHelpBrowser(this,fClient->GetRoot(),"dHelpBrowser","QwGUIMain",
					      "file:///home/mgericke/user/QWeakAnalysisGUIManual.html");
	}
	break;

      case M_HELP_CODE:
        {
	  sprintf(dMiscbuffer,"file://%s/Doxygen/html/index.html",getenv("QWANALYSIS"));
	  dHelpBrowser = new QwGUIHelpBrowser(this,fClient->GetRoot(),"dHelpBrowser","QwGUIMain",
					      dMiscbuffer);
// 		pid_t child = fork();
// 		if (-1 == child) perror("couldn't fork to open web browser");
// 		if (0 == child) {
// 			execl("/bin/sh", "/bin/sh", "-c",
// 			      "firefox "
// 			      "http://www.physics.umanitoba.ca/qweak/analysis/docs/code/index.html",
// 			      (char*)0);
// 			perror("couldn't exec shell for web browser");
// 			exit(1);
// 		}
		
	}
	break;

      case M_HELP_SEARCH:
	 {
#		define URL "http://sns.phys.utk.edu/svn/npdg/trunk/analysis/online"
		pid_t child = fork();
		if (-1 == child) perror("couldn't fork to show change history");
		if (0 == child) {
			execl("/bin/sh", "/bin/sh", "-c",
			      "xterm -title 'Online Analysis Change History' "
			      " -e \" "
			      "(echo changes to " URL " && svn log " URL " ) | less"
			      " \" "
			      , (char*)0);
			perror("couldn't exec shell for web browser");
			exit(1);
		}
	 }
	 break;

      case M_HELP_ABOUT:
	// were I able to make a cute box, I would put it here
	break;

      default:
	break;
      }

    default:
      break;
    }

  default:
    break;
  }

  return kTRUE;
}


QwGUIMain *gViewMain;
// void RunSignal(int sig)
// {
//   gViewMain->OnNewRunSignal(sig);
// }

// void RunWarning(int sig)
// {
//   gViewMain->OnRunWarningSignal(sig);
// }

int main(int argc, char **argv)
{
  char expl[5000];
  ClineArgs dClArgs;
  int help = 0;
  dClArgs.realtime = kFalse;
  dClArgs.checkmode = kFalse;
//   int ax,ay;
//   unsigned int aw, ah;

  if(argv[1]){
    for( int i=1; i < argc; i++){
      if(strcmp(argv[i],"-r")==0){
	dClArgs.realtime = kTrue;
      }

      if(strcmp(argv[i],"-cm")==0){
	dClArgs.checkmode = kTrue;
      }

      if(strcmp(argv[i],"-b")==0){
// 	dClArgs.bin = kTrue;
// 	dClArgs.txt = kFalse;
      }

      if(strcmp(argv[i],"-t")==0){
// 	dClArgs.bin = kFalse;
// 	dClArgs.txt = kTrue;
      }

      if(strcmp(argv[i],"-help")==0){
	help = 1;
      }

      if(strcmp(argv[i],"-f")==0){
	if(!argv[i+1] || argv[i+1][0] == '-'){
	  printf("\nMissing value for option -f\n\n");
	  return 0;
	}
// 	strcpy(dClArgs.file,argv[i+1]);
      }

      if(strcmp(argv[i],"-c")==0){
	if(!argv[i+1] || argv[i+1][0] == '-'){
	  printf("\nMissing value for option -c\n\n");
	  return 0;
	}
// 	dClArgs.clmns = atoi(argv[i+1]);
// 	printf("Selected Columns = %d\n",dClArgs.clmns);
      }
    }
  }
  else
    printf("\nRun ""QwGUIData -help"" for command line help\n\n");

  if(help){
    strcpy(expl,"\n\nThis program takes the following commandline arguments:\n\n");
    strcat(expl,"1) -b        Read binary format file.\n\n");
    strcat(expl,"2) -t        Read ascii text file in row and column format.\n\n");
    strcat(expl,"3) -f        Starting filename:\n\n");
    strcat(expl,"             Here one of two file types must be used, based on\n");
    strcat(expl,"             the -b or -t parameters passed .\n");
    strcat(expl,"             For case -b, the program expects a\n");
    strcat(expl,"             a binary file with format to be specified\n");
    strcat(expl,"             For case -t, it expects an ascii file \n");
    strcat(expl,"             arranged in rows and columns of data.\n");
    strcat(expl,"             For the second case, the columns of interest must\n");
    strcat(expl,"             be specified with the -c switch (see below).\n");
    strcat(expl,"             Always use the full path for the input files.\n\n");
    strcat(expl,"4) -c        Columns. Ex: (-c 23) selects columns 2 and 3.\n\n");
    strcat(expl,"9) -help     Prints this help \n\n");

    printf("%s",expl);
  }
  else{

    TApplication theApp("QwGUIData", &argc, argv);

    gROOT->SetStyle("Plain");

    if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
    }

    QwGUIMain mainWindow(gClient->GetRoot(), dClArgs, 800,600);

    gViewMain = &mainWindow;

    theApp.Run();
  }

  return 0;
}

void QwGUIMain::WritePid()
{
  printf("%s:%d\n",__FILE__, __LINE__ );

  // First, make sure the PID file doesn't exist.  If it does, die
  // noisily rather than hijack another realtime process.
  FILE *fp = 0;

  fp=fopen("QwGUID_PID.DAT","r");
  if(fp) {
    fclose(fp);
    printf("Dying noisily, please run QwGUIProcWarn\n");
    system("xterm -title 'a noisy death' -e 'echo Another process has abandoned a PID file.  Run QwGUIProcWarn. | less' &");
    exit(1);
  }

  fp=fopen("QwGUID_PID.DAT","w");
  if(fp) {
    memset(dMiscbuffer,'\0',sizeof(dMiscbuffer));
    sprintf(dMiscbuffer,"%d\n",(int)getpid());
    printf("pid = %s\n",dMiscbuffer);
    fwrite(dMiscbuffer,1,strlen(dMiscbuffer),fp);
    fclose(fp);
  } else {
    perror("couldn't write QwGUID_PID.DAT");
  }
}



