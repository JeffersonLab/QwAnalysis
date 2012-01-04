
#include <QwGUIHistories.h>


ClassImp(QwGUIHistories);

// const char *QwGUIHistories::HistoryPlots[] = {
//   "asym_qwk_charge_hw", "diff_qwk_targetX_hw", "diff_qwk_targetY_hw",
//   "diff_qwk_bpm3c12X_hw", "asym_qwk_mdallbars_hw"
// };

QwGUIHistories::QwGUIHistories(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{
  dTabFrame = NULL;
  dCanvas = NULL;
  dNumberEntryDlg = NULL;
  fOpen = kFalse;
  dCurrentRun = 0;

  NewDataInit();

  RemoveSelectedDataWindow();

  AddThisTab(this);

  //PrintMemInfo("At construction");
}


void QwGUIHistories::NewDataInit()
{
  // if(fOpen)
  //   WriteHistory();

  for(uint i = 0; i < PlotArray.size(); i++){
    if(PlotArray[i]) delete PlotArray[i];
  }
  PlotArray.clear();
  for(uint i = 0; i < BoxArray.size(); i++){
    if(BoxArray[i]) delete BoxArray[i];
  }
  BoxArray.clear();
}

QwGUIHistories::~QwGUIHistories()
{
  CleanUp();

  if (dTabFrame)  delete dTabFrame;
  if (dCanvas)    delete dCanvas;

  // if(dTabFrame)  delete dTabFrame;

  RemoveThisTab(this);
  IsClosing(GetName());

}

void QwGUIHistories::CleanUp()
{
  TObject* obj;
  vector <TObject*> obja;
  TIter *next = new TIter(DataWindowArray.MakeIterator());
  obj = next->Next();
  while(obj){
    obja.push_back(obj);
    obj = next->Next();
  }
  delete next;

  for(uint l = 0; l < obja.size(); l++)
    delete obja[l];

  DataWindowArray.Clear();

  if(fOpen)
    WriteHistory();

  for(uint i = 0; i < PlotArray.size(); i++){
    if(PlotArray[i]) delete PlotArray[i];
  }
  PlotArray.clear();
  for(uint i = 0; i < BoxArray.size(); i++){
    if(BoxArray[i]) delete BoxArray[i];
  }
  BoxArray.clear();
  dROOTCont = NULL;
}

void QwGUIHistories::MakeLayout()
{
  dTabFrame = new TGVerticalFrame(this,10,10);
  
  dCanvas   = new TRootEmbeddedCanvas("scC", dTabFrame,10, 10);
  dTabFrame->AddFrame(dCanvas,new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,0, 0, 1, 2));
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY));
  
  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
                                "QwGUIHistories",
                                this,"TabEvent(Int_t,Int_t,Int_t,TObject*)"); 
  TCanvas *mc = dCanvas->GetCanvas();
  if(mc) mc->Divide(2,4);

}

void QwGUIHistories::OnReceiveMessage(char *msg)
{
  TObject *obj = NULL;
  Int_t ind = 0;
  TString message = msg;

  if(message.Contains("dDataWindow")){

    if(message.Contains("Add to")){

      message.ReplaceAll("Add to dDataWindow_",7,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
	ind = DataWindowArray.IndexOf(obj);
	SetSelectedDataWindow(ind);
      }
    }
    else if(message.Contains("Don't add to")){


      message.ReplaceAll("Don't add to dDataWindow_",13,"",0);

      obj = DataWindowArray.FindObject(message);
      if(obj){
	RemoveSelectedDataWindow();
      }
    }
  }
}

QwGUIDataWindow *QwGUIHistories::GetSelectedDataWindow()
{
  if(dSelectedDataWindow < 0 || dSelectedDataWindow > DataWindowArray.GetLast()) return NULL;

  return (QwGUIDataWindow*)DataWindowArray[dSelectedDataWindow];
}


void QwGUIHistories::OnObjClose(char *obj)
{
  if(!obj) return;
  TString name = obj;

  if(name.Contains("dDataWindow")){
    QwGUIDataWindow* window = (QwGUIDataWindow*)DataWindowArray.Remove(DataWindowArray.FindObject(obj));
    if(window){
      if(window == GetSelectedDataWindow()) { RemoveSelectedDataWindow();}
    }
  }

  if(!strcmp(obj,"dNumberEntryDlg")){
    delete dNumberEntryDlg;
    dNumberEntryDlg = NULL;
  }

  if(!strcmp(obj,"dProgrDlg")){
    dProcessHalt = kTrue;
    dProgrDlg = NULL;
  }

  if(!strcmp(obj,"dROOTFile")){

    if(!AddMultipleFiles())
      CleanUp();

  }

  QwGUISubSystem::OnObjClose(obj);
}


void QwGUIHistories::OnNewDataContainer(RDataContainer *cont)
{
  
  if(!cont) return;

  Connect(cont,"IsClosing(char*)","QwGUIHistories",(void*)this,"OnObjClose(char*)");
  
  NewDataInit();

  // TObject *obj;
  // TTree *SlowTree = NULL;

  // if(!strcmp(cont->GetDataName(),"ROOT") && dROOTCont){
  // //Start filling root data

  //   //Current Mode
  //   obj = dROOTCont->ReadData("Slow_Tree");
  //   if(obj){
  //     if(obj->InheritsFrom("TTree")){
  // 	SlowTree = (TTree*)obj;//->Clone();
  //     }
  //   }

  //   if(SlowTree){
  //     GetData();
  //     PlotData();
  //   }
  // }
}

void QwGUIHistories::OnRemoveThisTab()
{

}

void QwGUIHistories::OnUpdatePlot(char *obj)
{
  if(!obj) return;
  TString str = obj;
  if(!str.Contains("dDataWind")) return;

  printf("Received Message From: %s\n",obj);
}

Int_t QwGUIHistories::GetFilenameFromDialog(char *file, const char *ext,
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

  static TString dir(Form("%s/Extensions/GUI",gSystem->Getenv("QWANALYSIS")));
  TGFileInfo fi;

  if(status == FS_OLD){
    fi.fFileTypes = (const char **)filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), GetMain(), kFDOpen, &fi);
    dir = fi.fIniDir;
    if(!fi.fFilename) {return PROCESS_FAILED;};
  }

  if(status == FS_NEW){
    fi.fFileTypes = (const char **)filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), GetMain(), kFDSave, &fi);
    dir = fi.fIniDir;

    if(!fi.fFilename) {return PROCESS_FAILED;};
  }
  strcpy(file,fi.fFilename);
  return PROCESS_OK;
}

void QwGUIHistories::WriteHistory()
{
  char row[200];
  Double_t *x = NULL;
  Double_t *y = NULL;
  Double_t *ey = NULL;

  //try to make a backup copy of the history file first
  gSystem->CopyFile(Form("%s/Extensions/GUI/QwAutoGUIHistories.dat",gSystem->Getenv("QWANALYSIS")),
		    Form("%s/Extensions/GUI/QwAutoGUIHistories-backup.dat",gSystem->Getenv("QWANALYSIS")),kTrue);

  TString file(Form("%s/Extensions/GUI/QwAutoGUIHistories.dat",gSystem->Getenv("QWANALYSIS")));

  HistoryData = new RDataContainer(fClient->GetRoot(), this,
				   "HistoryData","QwGUIHistories",
				   "TEXT",FM_WRITE,FT_TEXT);
  if(HistoryData){
    if(HistoryData->OpenFile(file) == FILE_PROCESS_OK) {

      for(uint i = 0; i < PlotArray.size(); i++){

	if(PlotArray[i]){
	  
	  x = PlotArray[i]->GetX();
	  y = PlotArray[i]->GetY();
	  ey = PlotArray[i]->GetEY();
	  
	  for(int j = 0; j < PlotArray[i]->GetN(); j++){
	    memset(row,'\0',sizeof(row));
	    sprintf(row,"%s,%d,%1.3e,%1.3e\n",PlotArray[i]->GetName(),(Int_t)x[j],y[j],ey[j]);
	    HistoryData->WriteData(row,strlen(row));
	  }
	}
      }
      StartNew = kFalse;
      HistoryData->Close();
      fOpen = kFalse;
    }
  }  
}


Int_t QwGUIHistories::OpenHistory()
{
  //return 0 if no history can/should be displayed
  //return 1 if a previous history is available 
  //return 2 if a new history should be started.

  TString file(Form("%s/Extensions/GUI/QwAutoGUIHistories.dat",gSystem->Getenv("QWANALYSIS")));
  fOpen = kFalse; 
  StartNew = kFalse;
  int retval = 0;

  HistoryData = new RDataContainer(fClient->GetRoot(), this,
				   "HistoryData","QwGUIHistories",
				   "TEXT",FM_READ,FT_TEXT);
  if(HistoryData){
    if(HistoryData->OpenFile(file) != FILE_PROCESS_OK) {
      fOpen = kFalse;
    }
    else {
      fOpen = kTrue;
      HistoryFile = file;
      FillPlots();
      return 1;
    }
  }
  else
    return 0; //return: Can't even open a file container! Something's really wrong!

  if(!fOpen){

    TString tmp("Could not open the history file!\n"
		"\nThis is either the first time this GUI is run\n"
		"or the history file was reset or removed somehow!\n\n"
		"The history files are backed up and can be restored.\n"
		"\nPress OK to continue and start a new history.\n"
		"Press DISMISS to run without history.\n"
		"Or press RETRY to use a backuped file.\n");
    
    while(!fOpen){
    
      new TGMsgBox(fClient->GetRoot(), GetMain(),"File Open Operation",
		   tmp.Data(),kMBIconQuestion, kMBOk | kMBDismiss | kMBRetry, &retval);
      if(retval == kMBDismiss){
	return 0; //user selects not to have history display
      }
      else if(retval == kMBOk){
	StartNew = kTrue; //user selects to start a new history
	fOpen = kTrue;
	// Restore = kFalse;   
	return 2;
      }
      else if(retval == kMBRetry){
	// Restore = kTrue; 
	StartNew = kFalse; //user wants to restore a backed up history
      
	//let the user try to find a file to restore the history from 
	if(GetFilenameFromDialog((char*)file.Data(),".dat",FS_OLD,kFalse,0) == PROCESS_FAILED){
	  //continue in loop
	}
	else{
	  if(HistoryData->OpenFile(file) == FILE_PROCESS_OK){
	    fOpen = kTrue;
	    HistoryFile = file;
	    FillPlots();
	    return 1; //continue from here
	  }
	  else{ 
	    fOpen = kFalse;
	  }
	}
      }
    }
  }
  return 0;
}

void QwGUIHistories::FillPlots()
{
  // QwGUISubSystem *sbSystem;
  TObjArray *RowParts;
  // TObject *obj;
  TString RowData;
  TString varName; 
  Int_t run;
  Double_t val;
  Double_t rms;
  // Double_t err;  
  char buffer[200];
  Int_t found = -1;
  Int_t p = 0;

  for(int nr = 1; nr <= HistoryData->GetNumOfRows(); nr++){
    memset(buffer,'\0',sizeof(buffer));
    if(HistoryData->ReadRow(buffer,nr) == FILE_PROCESS_OK){
      RowData = buffer;
      if(RowData.Sizeof() && !RowData.BeginsWith('#')){
	RowParts = RowData.Tokenize(",");
	if(RowParts->GetEntries() == 4){
	  varName = ((TObjString*)RowParts->At(0))->GetString();
	  run = (((TObjString*)RowParts->At(1))->GetString()).Atoi();
	  val = (((TObjString*)RowParts->At(2))->GetString()).Atof();
	  rms = (((TObjString*)RowParts->At(3))->GetString()).Atof();
	  delete RowParts;
	  	  
	  if(run == dCurrentRun) continue;

	  found = -1;
	  for(uint j = 0; j < PlotArray.size(); j++){
	    if(PlotArray[j] && !varName.CompareTo(PlotArray[j]->GetName())) found = j;
	  }
      
	  if(found >= 0){
	
	    p = PlotArray[found]->GetN();
	    // printf("Adding to graph: point %d %s,%d,%1.3e,%1.3e\n",p,varName.Data(),run,val,rms);
	    PlotArray[found]->SetPoint(p,run,val);
	    PlotArray[found]->SetPointError(p,0,rms);
	    
	  }
	  else{
	    
	    //there is no graph for this yet, so create it
	    TGraphErrors *grp = new TGraphErrors();
	    // printf("Creating graph: point %d  %s,%d,%1.3e,%1.3e\n",grp->GetN(),varName.Data(),run,val,rms/2);
	    grp->SetName(varName.Data());
	    // grp->SetTitle(Form("%s History (mean & rms)",varName.Data()));
	    grp->SetPoint(0,run,val);
	    grp->SetPointError(0,0,rms);
	    grp->GetXaxis()->SetTitle("Run");
	    grp->SetEditable(kFalse);
	    PlotArray.push_back(grp);
	    
	  }	  
	}
      }
    }
  }
  HistoryData->Close(); //Close file and container after reading all of the history
  HistoryData = NULL;   //Need to recreate the container for later writing to it
}

void QwGUIHistories::PlotData(vector <TH1F*> plots, vector <TBox*> boxes, Int_t Run)
{
  TString newname;
  vector <TH1F*> newplots = plots;
  TCanvas *mc = NULL;
  dCurrentRun = Run;
  Int_t p = 0;
  
  //Load the histories if there are any
  Int_t status = OpenHistory();
  if(!status) return;

      
  if(status == 1){

    //just add to the plots that have been created from reading the previous
    //history
    
    for(uint i  = 0; i < newplots.size(); i++){
      for(uint j = 0; j < PlotArray.size(); j++){
	
	if(PlotArray[j] && newplots[i]){

	  newname = Form("_%s_",newplots[i]->GetName());

	  if(!newname.CompareTo(PlotArray[j]->GetName())){
	    p = PlotArray[j]->GetN();

	    // printf("Adding new point %d to graph: %s,%d,%1.3e,%1.3e\n",PlotArray[j]->GetN(),newname.Data(),Run,newplots[i]->GetMean(),newplots[i]->GetRMS()/2);
	    
	    PlotArray[j]->SetPoint(p,Run,newplots[i]->GetMean());
	    PlotArray[j]->SetPointError(p,0,newplots[i]->GetRMS());
	    PlotArray[j]->GetYaxis()->SetTitle(newplots[i]->GetXaxis()->GetTitle());

	  }
	}
      }
    }
  }
  else if(status == 2){
    
    //start a new history
    
    for(uint i  = 0; i < newplots.size(); i++){
      
      if(newplots[i]){
	TGraphErrors *grp = new TGraphErrors();
	grp->SetName(Form("_%s_",newplots[i]->GetName()));
	// printf("Starting new history with graph: point %d  %s,%d,%1.3e,%1.3e\n",grp->GetN(),grp->GetName(),Run,newplots[i]->GetMean(),newplots[i]->GetRMS()/2);
	// grp->SetTitle(Form("%s History (mean & rms)",newplots[i]->GetName()));
	grp->SetPoint(0,Run,newplots[i]->GetMean());
	grp->SetPointError(0,0,newplots[i]->GetRMS());
	grp->GetXaxis()->SetTitle("Run");
	grp->GetYaxis()->SetTitle(newplots[i]->GetXaxis()->GetTitle());
	grp->SetEditable(kFalse);
	PlotArray.push_back(grp);
      }
    }
  }

  mc = dCanvas->GetCanvas();
  if(!mc) return;
  TBox *abox = NULL;
  Double_t ymin;
  Double_t ymax;
  TString name;

  for(uint n = 0; n < PlotArray.size(); n++){

    abox = NULL;
    mc->cd(n+1);
    
    PlotArray[n]->GetXaxis()->CenterTitle();
    PlotArray[n]->GetYaxis()->CenterTitle();
    PlotArray[n]->GetXaxis()->SetTitleSize(0.08);
    PlotArray[n]->GetYaxis()->SetTitleSize(0.08);
    PlotArray[n]->GetXaxis()->SetLabelSize(0.08);
    PlotArray[n]->GetYaxis()->SetLabelSize(0.08);
    PlotArray[n]->GetXaxis()->SetTitleOffset(0.8);
    PlotArray[n]->GetYaxis()->SetTitleOffset(0.8);
    PlotArray[n]->GetXaxis()->SetTitleColor(1);
    PlotArray[n]->SetMarkerStyle(21);
    PlotArray[n]->SetMarkerColor(2);
    PlotArray[n]->SetLineWidth(2);
    PlotArray[n]->GetHistogram()->SetNdivisions(502,"X");
    PlotArray[n]->GetHistogram()->SetNdivisions(505,"Y");
    // PlotArray[n]->GetYaxis()->SetLabelSize(0.06);
        
    PlotArray[n]->Draw("ap");
    gPad->SetLeftMargin(0.15);
    gPad->SetTopMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->Modified();
    gPad->Update();
    gPad->GetFrame()->SetToolTipText("Double-click this plot to post, edit, and save.", 250);    


    if(n < boxes.size() && boxes[n]){

      if(PlotArray[n]->GetYaxis()->GetXmin() > boxes[n]->GetX1()) 
	ymin = boxes[n]->GetX1();
      else
	ymin = PlotArray[n]->GetYaxis()->GetXmin();

      if(PlotArray[n]->GetYaxis()->GetXmax() < boxes[n]->GetX2()) 
	ymax = boxes[n]->GetX2();
      else
	ymax = PlotArray[n]->GetYaxis()->GetXmax();
                        
      name = PlotArray[n]->GetName();
      if(name.Contains("EffectiveCharge")){

	ymin = boxes[n]->GetX1();//PlotArray[n]->GetYaxis()->GetXmin();
	ymax = boxes[n]->GetX2();//PlotArray[n]->GetYaxis()->GetXmax();
      }

//       if(ymin < 0) PlotArray[n]->SetMinimum(2*ymin);
//       if(ymin >= 0) PlotArray[n]->SetMinimum(ymin - );

//       PlotArray[n]->SetMinimum(ymin-fabs(ymin));
//       PlotArray[n]->SetMaximum(ymax+fabs(ymax));
      PlotArray[n]->SetMinimum(ymin-fabs(ymax-ymin)/2);
      PlotArray[n]->SetMaximum(ymax+fabs(ymax-ymin)/2);


      abox = new TBox(PlotArray[n]->GetXaxis()->GetXmin(),boxes[n]->GetX1(),PlotArray[n]->GetXaxis()->GetXmax(),boxes[n]->GetX2());
      // abox->SetFillColor(2);
      abox->SetLineColor(2);
      abox->SetFillStyle(0);//3002);
      abox->Draw("");
      gPad->Modified();
      gPad->Update();
      BoxArray.push_back(abox);
    }


    TPaveText *ref = new TPaveText(0.2,0.85,0.8,0.98,"blNDC");
    ref->AddText("mean #pm rms width (indicated by the bar)");
    ref->SetBorderSize(0);
    ref->SetFillColor(0);
    ref->Draw();

  }
  
  if(fOpen)
    WriteHistory();

}

void QwGUIHistories::MakeHCLogEntry()
{
  TRootEmbeddedCanvas *cnv = dCanvas;
  if(cnv)
    SubmitToHCLog(cnv->GetCanvas());

}

void QwGUIHistories::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  QwGUIDataWindow *dDataWindow = GetSelectedDataWindow();
  Bool_t add = kFalse;
  TGraphErrors *plot = NULL;
  
  if(event == kButton1Double){
    
    Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();

    plot = PlotArray[pad-1];
    if(!plot) return;
    
    if(plot->InheritsFrom("TGraphErrors")){

      if(!dDataWindow){
	dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
					  "QwGUIHistories",((TGraphErrors*)plot)->GetTitle(), PT_GRAPH_ER,
					  DDT_MD,600,400);
	if(!dDataWindow){
	  return;
	}
	DataWindowArray.Add(dDataWindow);
      }
      else
	add = kTrue;
            
      dDataWindow->SetPlotTitle((char*)((TGraphErrors*)plot)->GetTitle());
      dDataWindow->SetPlotTitleX((char*)((TGraphErrors*)plot)->GetXaxis()->GetTitle());
      dDataWindow->SetPlotTitleY((char*)((TGraphErrors*)plot)->GetYaxis()->GetTitle());
      dDataWindow->SetMarkerColor(((TGraphErrors*)plot)->GetMarkerColor());
      dDataWindow->SetLineColor(((TGraphErrors*)plot)->GetLineColor());
      dDataWindow->SetLineWidth(((TGraphErrors*)plot)->GetLineWidth());
      dDataWindow->SetMarkerStyle(((TGraphErrors*)plot)->GetMarkerStyle());
      dDataWindow->DrawData(*((TGraphErrors*)plot),add);
      if(pad-1 < (Int_t)BoxArray.size() && BoxArray[pad-1])
	dDataWindow->DrawBox(*((TBox*)BoxArray[pad-1]));

      SetLogMessage(Form("Looking at graph %s\n",(char*)((TGraphErrors*)plot)->GetTitle()),kTrue);

      Connect(dDataWindow,"IsClosing(char*)","QwGUIHistories",(void*)this,"OnObjClose(char*)");
      Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIHistories",(void*)this,"OnReceiveMessage(char*)");
      Connect(dDataWindow,"UpdatePlot(char*)","QwGUIHistories",(void*)this,"OnUpdatePlot(char *)");
      dDataWindow->SetRunNumber(GetRunNumber());

      return;
    }
  }
}


Bool_t QwGUIHistories::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//   MDMenuIdentifiers dtype;

  switch (GET_MSG(msg)){

  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_ENTER:
      switch (parm1) {

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
	}
      }
      break;

    case kCM_MENUSELECT:
      break;

    case kCM_TAB:
      {
      }
      break;
      
    case kCM_MENU:
      {
      }
      break;
      
    default:
      break;
    }

  default:
    break;
  }

  return kTRUE;
}


