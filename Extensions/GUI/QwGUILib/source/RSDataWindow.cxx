//(01/05/04 10:00 pm)
//
//Edit GetObj and look at the Draw functions ...
//Added functionalty to have the possibilty of opening
//root and data containers from this object...
//this changes the way can pass data to the Draw routines...
//
//
//Remember to edit NDataWindow and NDetector because the access functions
//in RSDataWindow have changed and because additional settings (line color,
//default settings, etc.) need to be made in the functions which call 
//the DrawData routines in RSDataWindow....


/////////////////////////////////////////////////////////////////////////////////////
//NPDGamma Experiment Data Visualization and Analysis
//
//Version 2.01
//
//RSDataWindow source file, containing methods to visualize data
//All methods specifying data visualization must be specified in
//the derived class.
/////////////////////////////////////////////////////////////////////////////////////

#include "RSDataWindow.h"

ClassImp(RSDataWindow);

RSDataWindow::RSDataWindow(const TGWindow *p, const TGWindow *main,
			 const char *objName, const char *mainname, const char *datatitle,
			 ERPlotTypes type,UInt_t w,UInt_t h, UInt_t options)
  : TGTransientFrame(p, main, w, h, options)
//   : TQObject()
{  

  fParent = (const TGWindow*)p;
  fMain   = (const TGWindow*)main;

  dDummyPlot = NULL;
  dDatafit = NULL;
  dFitHistocnt = 0;
  dFitOptions = NULL;
  for(int i = 0; i < 10; i++)
    fFitHisto[i]=NULL;

  dMsgcnt = 0;

  dDrawpat = 0;
  fMrec = NULL;
  dPtype = type;

  memset(dObjName,'\0',NAME_STR_MAX);
  strcpy(dObjName,objName);

  memset(dReceiverName,'\0',sizeof(dReceiverName));
  strcpy(dReceiverName,"");

  memset(dLegEntry,'\0',NAME_STR_MAX);
  strcpy(dLegEntry,"none");

  memset(dLegHeader,'\0',NAME_STR_MAX);
  strcpy(dLegHeader,"none");

  memset(dPlotname,'\0',NAME_STR_MAX);
  strcpy(dPlotname,"none");
  
  memset(dDatatitle,'\0',NAME_STR_MAX);
  strcpy(dDatatitle,datatitle);  

  memset(dMainName,'\0',sizeof(dMainName));
  strcpy(dMainName,mainname);

  memset(dMiscbuffer,'\0',sizeof(dMiscbuffer));
  memset(dMiscbuffer2,'\0',sizeof(dMiscbuffer2));
  for(int i = 0; i < MSG_QUEUE_MAX; i++)
    memset(dMessage[i],'\0',sizeof(dMessage[i]));

  dROOTFileObjDlg = NULL;
  dRootCont = NULL;
  dIntRootCont = kFalse;
  dPlotCont = new RPlotContainer(fClient->GetRoot(), this,
				 (char*)"dPlotCont",(char*)"RSDataWindow",
				 GetDataTitle());
  SetDefaultPlotOptions();
  SetAxisMin();
  SetAxisMax();
  SetNumBins();
  SetDrawOptions();
  SetAddGraphs(kFalse);
  SetLimitsFlag(kFalse);
  dUpdateHisto = kFalse;

  SetPlotTitle(dDatatitle);
  SetPlotTitleX("none");
  SetPlotTitleY("none");
  SetPlotTitleZ("none");
  SetPlotTitleOffset();

  fCurrPlot = NULL;
  fMenuFile = NULL;
  fMenuBar = NULL;
  fMenuBarItemLayout = NULL;
  fMenuBarLayout = NULL;
  fCancelButton = NULL;
  fFrame1 = NULL;
  fDlgFrame = NULL;
  fCnvFrame = NULL;
  fDlgLayout = NULL;
  fCnvLayout = NULL;
  fL1 = NULL;
  fL2 = NULL;
  fPlotCanvas = NULL;  
  //fLMGraph = NULL;

  //for(int i = 0; i < 10; i++)
  //fFitHisto[i]=NULL;
  
  fLegend = NULL;

  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
  
  fMenuTools = new TGPopupMenu(fClient->GetRoot());
  fMenuTools->AddEntry("&Add Next Plot", M_PLOT_ADD);
  // fMenuTools->AddEntry("&Fit...",M_PLOT_FITSETUP);
  fMenuTools->AddEntry("&Legend",M_PLOT_ADDLEGEND);
  // fMenuTools->AddEntry("Normalize",M_PLOT_NORM);
  // fMenuTools->AddEntry("Scale Data ...",M_PLOT_SCALE);
  // fMenuTools->AddEntry("Divide By Data ...",M_PLOT_DIVIDE);
  // fMenuTools->AddEntry("Integrate Data",M_PLOT_INTEG);
  fMenuTools->AddEntry("Write Data (Row-Column) ...",M_PLOT_WRITEMISC); 
  fMenuTools->AddEntry("View Data (Row-Column) ...",M_PLOT_READMISC); 
  fMenuTools->AddEntry("View Data (Root Object) ...",M_PLOT_VIEWROOTOBJ);
  // fMenuTools->AddEntry("Editor ...",M_PLOT_EDITOR);

  fMenuTools->Associate(this);
  fPlotCanvas = TCanvas::MakeDefCanvas();

  TGMenuBar* menubar = ((TRootCanvas*)fPlotCanvas->GetCanvasImp())->GetMenuBar();
//   TGPopupMenu *menu =  menubar->GetPopup("Close Canvas");
  menubar->AddPopup("&Qweak Tools",fMenuTools,fMenuBarItemLayout);
  menubar->MapSubwindows();
  menubar->Layout();
  fMenuTools->UnCheckEntry(M_PLOT_ADD);

  fPlotCanvas->SetBorderMode(0);                            
  fPlotCanvas->SetFillColor(kWhite);
                            
  fPlotCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
		       "RSDataWindow",this,
		       "SetLimits(Int_t,Int_t,Int_t,TObject*)");
  ((TRootCanvas*)fPlotCanvas->GetCanvasImp())->Connect("CloseWindow()","RSDataWindow",this,"CloseWindow()");
  
  SetName(dObjName);
  SetWindowName("Plot Window");

  Connect("IsClosing(char*)",dMainName,(void*)main,"OnObjClose(char*)");  
  Connect("SendMessageSignal(char*)",dMainName,(void*)main,
	  "OnReceiveMessage(char*)");
}

RSDataWindow::~RSDataWindow()
{
  ClearPlots();

  if(fMenuFile         ) delete fMenuFile;
  if(fMenuBar          ) delete fMenuBar;
  if(fMenuBarItemLayout) delete fMenuBarItemLayout;
  if(fMenuBarLayout    ) delete fMenuBarLayout;
  if(fCancelButton     ) delete fCancelButton;
  if(fFrame1           ) delete fFrame1;
  if(fDlgFrame         ) delete fDlgFrame;
  if(fCnvFrame         ) delete fCnvFrame;
  if(fDlgLayout        ) delete fDlgLayout;
  if(fCnvLayout        ) delete fCnvLayout;
  if(fL1               ) delete fL1;
  if(fL2               ) delete fL2;
  if(dPlotCont         ) delete dPlotCont;
  if(fPlotCanvas       ) delete fPlotCanvas;       

  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }  

  delete dDummyPlot;
}

void RSDataWindow::OnObjClose(char *obj)
{
  
  if(!strcmp("dROOTFileObjDlg",obj)){
    dROOTFileObjDlg = NULL;
  }
  if(!strcmp("dDatafit",obj)){
    dDatafit = NULL;
  }
  if(!strcmp("dPlotCont",obj)){
    dPlotCont = NULL;
  }
  if(!strcmp("dRootCont",obj)){
    dRootCont = NULL;
    dIntRootCont = kFalse;
  }
}

void RSDataWindow::OnSelectedEntry(char *name)
{
  Char_t lname[NAME_STR_MAX];
  strcpy(lname,name);
  if(dRootCont){
    TObject *obj = dRootCont->ReadData(lname);
    printf("Selected Obj = %s\n",obj->GetName());

    if(obj->InheritsFrom("TH1D"))
      DrawData(*(TH1D*)(obj),dAddGraphs);

//     if(!strcmp((char*)obj->ClassName(),"TF1")){
//       DrawData(*dynamic_cast<TF1*>(obj),dAddGraphs);
//     }
//     if(!strcmp((char*)obj->ClassName(),"TH1D")){
//       DrawData(*dynamic_cast<TH1D*>(obj),dAddGraphs);
//     }
//     else if(!strcmp((char*)obj->ClassName(),"TGraph")){
//       SetDrawOptions("ap");
//       DrawData(*dynamic_cast<TGraph*>(obj),dAddGraphs);
//     }
    if(!strcmp((char*)obj->ClassName(),"TGraphErrors")){
      SetDrawOptions("ap");
      DrawData(*(TGraphErrors*)(obj),dAddGraphs);
    }
//     else if(!strcmp((char*)obj->ClassName(),"TGraphAsymmErros")){
//       SetDrawOptions("ap");
//       DrawData(*dynamic_cast<TGraphAsymmErrors*>(obj),dAddGraphs);
//     }
    if(IsRootContInternal())
      dRootCont->Close();
  }
}

void RSDataWindow::IsClosing(char *objname)
{
  Emit("IsClosing(char*)",(long)objname);
}

void RSDataWindow::SendMessageSignal(char *objname)
{
  Emit("SendMessageSignal(char*)",(long)objname);
}

void RSDataWindow::ConnectWith(const TGWindow *rec, char *recname)
{
  if(fMrec == NULL && rec != NULL){
    fMrec = (TGWindow*)rec;
    strcpy(dReceiverName,recname);
  }
}

void RSDataWindow::CloseConnection()
{
  if(fMrec != NULL){
    fMrec = NULL;
    strcpy(dReceiverName,"");
  }
}

Int_t RSDataWindow::GetPlotCount()
{
  if(dPlotCont)
    return dPlotCont->GetPlotCount();

  return 0;
}

Int_t RSDataWindow::OpenContainer()
{
  if(IsRootContInternal() && dRootCont)
    dRootCont->Close();
  
  dRootCont = new RDataContainer(fClient->GetRoot(), this,
				 "dRootCont","RSDataWindow",
				 "ROOT",FM_UPDATE,FT_ROOT);
  
  if(!dRootCont){
    FlushMessages();
    SetMessage(ROOTOBJ_CRFAIL_ERROR,"OpenContainer()",(int)dPtype,
	       M_DTWIND_ERROR_MSG);
    return PROCESS_FAILED;
  }
  Connect(dRootCont,"IsClosing(char*)","RSDataWindow",(void*)this,
	  "OnObjClose(char*)");      
  dIntRootCont = kTrue;
  return PROCESS_OK;
}

Int_t RSDataWindow::OpenRootFile()
{
  int retval;
  if(dRootCont == NULL) return PROCESS_FAILED;

  const char *rootfiletypes[] = { "ROOT files",    "*.root",
				  0,               0 };
  static TString dir(".");

  TGFileInfo fi;  
  fi.fFileTypes = (const char **)rootfiletypes;
 
  new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
 
  if(fi.fFilename){	
    if(!strcmp(".root",strrchr(fi.fFilename,'.'))){
      if(dRootCont->OpenFile(fi.fFilename) == FILE_PROCESS_OK){
	return PROCESS_OK;
      } 
      else {
	sprintf(dMiscbuffer,"Cant Open File: %s",fi.fFilename);
	new TGMsgBox(fClient->GetRoot(), this,
		     "File Operation",dMiscbuffer,
		     kMBIconExclamation, kMBOk, &retval);	  
	return PROCESS_FAILED;
      }
    }
  }
  return PROCESS_FAILED;
}

void RSDataWindow::GetRootObjectList(RDataContainer* fileCont)
{
  RDataContainer *cont = fileCont;
  TString *str;
  Int_t size = 0;
  if(!cont){ 
    if(OpenContainer() != PROCESS_OK) return;
    if(OpenRootFile() != PROCESS_OK) return;
    cont = dRootCont;
  }
  if(cont){
    size = cont->GetNumOfRootObjects();
    printf("Number of Root objects = %d\n",size);
    dROOTFileObjDlg = new RComboBoxDialog(fClient->GetRoot(), this,
					  "dROOTFileObjDlg","RSDataWindow",
					  cont->GetFileName(),
					  size,600,300);
    if(dROOTFileObjDlg){
      Connect(dROOTFileObjDlg,"SetSelectedEntry(char*)","RSDataWindow",(void*)this,
	      "OnSelectedEntry(char*)");      
      Connect(dROOTFileObjDlg,"IsClosing(char*)","RSDataWindow",(void*)this,
	      "OnObjClose(char*)");      
      str = new TString[size];
      if(cont->GetListOfRootObjects(str) == FILE_PROCESS_OK){
	for(int i = 0; i < size; i++){
	  dROOTFileObjDlg->AddEntry(&str[i][0]);
	}
      }
      delete[] str;
    }
  }
}


void RSDataWindow::SetRootContainer(RDataContainer *cont)
{
  if(IsRootContInternal() && dRootCont)
    dRootCont->Close();

  dRootCont = cont;
  dIntRootCont = kFalse;
}

void RSDataWindow::SetDefaultPlotOptions()
{
  if(dPlotCont)
    dPlotCont->SetDefaultPlotOptions();
}

void RSDataWindow::SetDrawOptions(const char *opts)
{
  if(opts != NULL) {
    strcpy(dDrawOptions,opts);
    dDrawOptsSet = kTrue;
  }
  else {
    strcpy(dDrawOptions,"");
    dDrawOptsSet = kFalse;
  }
}

void RSDataWindow::SetLineColor(Color_t col)
{
  dPlotCont->SetLineColor(col);
}

void RSDataWindow::SetLineStyle(Style_t sty)
{
  dPlotCont->SetLineStyle(sty);
}

void RSDataWindow::SetLineWidth(Width_t wid)
{
  dPlotCont->SetLineWidth(wid);
}

void RSDataWindow::SetMarkerColor(Color_t col)
{
  dPlotCont->SetMarkerColor(col);
}

void RSDataWindow::SetMarkerStyle(Style_t sty)
{
  dPlotCont->SetMarkerStyle(sty);
}

void RSDataWindow::SetMarkerSize(Size_t sze) 
{
  dPlotCont->SetMarkerSize(sze);
}

void RSDataWindow::SetAxisMin(Double_t xmin, Double_t ymin, Double_t zmin)
{
  dMin[0] = xmin;
  dMin[1] = ymin;
  dMin[2] = zmin;
}

void RSDataWindow::SetAxisMax(Double_t xmax, Double_t ymax, Double_t zmax)
{
  dMax[0] = xmax;
  dMax[1] = ymax;
  dMax[2] = zmax;
}

void RSDataWindow::SetPlotTitleOffset(Double_t xoffs, Double_t yoffs, Double_t zoffs)
{
  dTitleOffset[0] = xoffs;
  dTitleOffset[1] = yoffs;
  dTitleOffset[2] = zoffs;
}


Double_t RSDataWindow::GetAxisMin(Int_t axis)
{
  if(axis >= 0 && axis < 3)
    return dMin[axis];

  return 0.0;
}

Double_t RSDataWindow::GetAxisMax(Int_t axis)
{
  if(axis >= 0 && axis < 3)
    return dMax[axis];

  return 0.0;
}

void RSDataWindow::SetNumBins(Int_t xbins, Int_t ybins, Int_t zbins)
{
  dNumbins[0] = xbins;
  dNumbins[1] = ybins;
  dNumbins[2] = zbins;
}

Int_t RSDataWindow::GetNumBins(Int_t axis)
{
  if(axis >= 0 && axis < 3)
    return dNumbins[axis];

  return 0;
}

void RSDataWindow::SetLimits(Int_t event,Int_t x,Int_t y, TObject *selobj)
{
  if(event == kButton1Down && selobj->InheritsFrom("TAxis"))
    printf("Canvas Modified\n");

  //See PaintBox(...) and kMouseMotion in TPad 
}

void RSDataWindow::IntegrateWindowData()
{
  Double_t *x  = NULL;
  Double_t *y  = NULL;
  Double_t *ye = NULL;

  Double_t integ = 0;
  Double_t inter = 0;
  Int_t    nlow  = 0;
  Int_t    nhigh = 0;
  Double_t width;
  Double_t val;
  Double_t err2;
  Double_t llow  = 0;
  Double_t lhigh = 0;
  Double_t avgw  = 0;
  Int_t range = 0;
  Int_t retval;

  TObject *obj = dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);
  if(obj && !strcmp(obj->ClassName(),"TGraphErrors")){
    TGraphErrors *gr = (TGraphErrors*)obj;
    range = gr->GetN();
    x = new Double_t[range]; memcpy(x,gr->GetX(),range*sizeof(Double_t));
    y = new Double_t[range]; memcpy(y,gr->GetY(),range*sizeof(Double_t));
    ye= new Double_t[range]; memcpy(ye,gr->GetEY(),range*sizeof(Double_t));
    
    llow  = gr->GetXaxis()->GetBinLowEdge(gr->GetXaxis()->GetFirst());
    lhigh = gr->GetXaxis()->GetBinUpEdge(gr->GetXaxis()->GetLast());

    nlow  = 0;
    nhigh = range-1;
    for(int i = 0; i < range; i++)
      if(llow <= x[i]) {nlow = i; break;}
    for(int i = 0; i < range; i++)
      if(lhigh <= x[i]){nhigh = i; break;}

    for(int i = nlow; i < nhigh; i++){
      width = x[i+1] - x[i];
      val   = (gr->GetY()[i]+gr->GetY()[i+1])/2.0;
      err2  = (pow(gr->GetEY()[i],2)+pow(gr->GetEY()[i+1],2))/4.0;
      avgw  += width;
      integ += val*width;
      inter += width*width*err2;
      inter += width*width*val*val/4.0;
    }
  }
  if(obj && !strcmp(obj->ClassName(),"TGraph")){
    TGraph *gr = (TGraph*)obj;
    range = gr->GetN();
    x = new Double_t[range]; memcpy(x,gr->GetX(),range*sizeof(Double_t));
    y = new Double_t[range]; memcpy(y,gr->GetY(),range*sizeof(Double_t));
    
    llow  = gr->GetXaxis()->GetBinLowEdge(gr->GetXaxis()->GetFirst());
    lhigh = gr->GetXaxis()->GetBinUpEdge(gr->GetXaxis()->GetLast());

    nlow  = 0;
    nhigh = range-1;
    for(int i = 0; i < range; i++)
      if(llow <= x[i]) {nlow = i; break;}
    for(int i = 0; i < range; i++)
      if(lhigh <= x[i]){nhigh = i; break;}

    for(int i = nlow; i < nhigh; i++){
      width = x[i+1] - x[i];
      val   = (gr->GetY()[i]+gr->GetY()[i+1])/2.0;
      avgw  += width;
      integ += val*width;
      inter += width*width*val*val/4.0;
    }
  }

  sprintf(dMiscbuffer,"Integral = sum(y*dx) = %2.2e +- %2.2e\n",integ,sqrt(inter));
  strcpy(dMiscbuffer2,dMiscbuffer);
  sprintf(dMiscbuffer,"<dx> = %1.2e (in units of x-axis)\n",avgw/(nhigh-nlow));
  strcat(dMiscbuffer2,dMiscbuffer);
  sprintf(dMiscbuffer,"limits = %1.2e to %1.2e (in units of x-axis)\n",x[nlow],x[nhigh]);
  strcat(dMiscbuffer2,dMiscbuffer);
  new TGMsgBox(fClient->GetRoot(), this,
	       "Integration Result",dMiscbuffer2,
	       kMBIconExclamation, kMBOk, &retval);	  
  if(x)  delete[] x;
  if(y)  delete[] y;
  if(ye) delete[] ye;
}

void RSDataWindow::WriteMiscData()
{
  ERFileTypes ftype;
  const char *filetypes[] = { "Data files",     "*.dat",
			      "Data files",     "*.txt",
			      "MathCad files",  "*.mth",
			      0,               0 };  
  static TString dir(".");
  TString ext;
  TGFileInfo fi;
  fi.fFileTypes = (const char **)filetypes;
  fi.fIniDir    = StrDup(dir);
  new TGFileDialog(fClient->GetRoot(), 0 /*this*/, kFDSave, &fi);
  dir = fi.fIniDir;

  Int_t ngrphs = 0;

  if(!fi.fFilename) return;
  ext = strrchr(fi.fFilename,'.');
  printf("extension = %s\n",strrchr(fi.fFilename,'.'));

  TObject *obj = NULL;//dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);

//   if(obj && obj->InheritsFrom("TMultiGraph")){
//     TMultiGraph *mgr = (TMultiGraph*)obj;
    
//     grpList = mgr->GetListOfGraphs();
//     if(grpList) 
//       ngrphs = grpList->LastIndex()+1;
//     else
//       ngrphs = 0;

//     printf("ngrphs = %d\n",ngrphs);


  ngrphs = dPlotCont->GetPlotCount();

  for(int i = 0; i < ngrphs; i++){


    obj = dPlotCont->GetPlot(i);

    printf("writing graph %d (ngrphs = %d)\n",i,ngrphs);

    if(obj && obj->InheritsFrom("TGraphErrors")){
      ext.Contains(".mth") ? ftype = FT_MATHCAD : ftype = FT_ROWCOLUMN;
      if(ftype == FT_MATHCAD) printf("Write MathCad\n");
      TGraphErrors *gr = (TGraphErrors*)obj;
      RDataContainer * cont = new RDataContainer(fClient->GetRoot(), this,"cont",
						 "RSDataWindow","Raw Data",FM_UPDATE,
						 ftype);
      if(!cont) return;
      
      if(cont->OpenFile(fi.fFilename) != FILE_PROCESS_OK) {cont->Close(); return;}
      cont->WriteData(gr->GetX(),gr->GetY(),gr->GetEX(),gr->GetEY(),gr->GetN(), gr->GetName());
      cont->Close();
    }
    else if(obj && obj->InheritsFrom("TGraph")){
      ext.Contains(".mth") ? ftype = FT_MATHCAD : ftype = FT_ROWCOLUMN;
      if(ftype == FT_MATHCAD) printf("Write MathCad\n");
      TGraph *gr = (TGraph*)obj;
      RDataContainer * cont = new RDataContainer(fClient->GetRoot(), this,"cont",
						 "RSDataWindow","Raw Data",FM_UPDATE,
						 ftype);
      if(!cont) return;
      
      if(cont->OpenFile(fi.fFilename) != FILE_PROCESS_OK) {cont->Close(); return;}
      cont->WriteData(gr->GetX(),gr->GetY(),gr->GetN(), gr->GetName());
      cont->Close();
    }
    else if(obj && obj->InheritsFrom("TH1")){

      
    }
  }
}

void RSDataWindow::NormalizeWindowData()
{
  Double_t *x  = NULL;
  Double_t *y  = NULL;
  Double_t *ye = NULL;
  Double_t max;
  Int_t range = 0;

  TObject *obj = dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);
  if(obj && !strcmp(obj->ClassName(),"TGraphErrors")){
    TGraphErrors *gr = (TGraphErrors*)obj;
    range = gr->GetN();
    x = new Double_t[range]; memcpy(x,gr->GetX(),range*sizeof(Double_t));
    y = new Double_t[range]; memcpy(y,gr->GetY(),range*sizeof(Double_t));
    ye= new Double_t[range]; memcpy(ye,gr->GetEY(),range*sizeof(Double_t));
    max = maxv(y,range);
    for(int i = 0; i < range; i++) {y[i] /= max; ye[i] /= sqrt(max*max);};
    DrawData(x,y,ye,range);
  }
  if(obj && !strcmp(obj->ClassName(),"TGraph")){
    TGraph *gr = (TGraph*)obj;
    range = gr->GetN();
    x = new Double_t[range]; memcpy(x,gr->GetX(),range*sizeof(Double_t));
    y = new Double_t[range]; memcpy(y,gr->GetY(),range*sizeof(Double_t));
    max = maxv(y,range);
    for(int i = 0; i < range; i++) {y[i] /= max;};
    DrawData(x,y,range);
  }

  if(x)  delete[] x;
  if(y)  delete[] y;
  if(ye) delete[] ye;
}

void RSDataWindow::DivideWindowData()
{
  Double_t *x   = NULL;
  Double_t *y   = NULL;
  Double_t *ye  = NULL;
  Double_t *x1  = NULL;
  Double_t *x2  = NULL;
  Double_t *y1  = NULL;
  Double_t *y2  = NULL;
  Double_t *ye1 = NULL;
  Double_t *ye2 = NULL;
  Int_t range1  = 0;
  Int_t range2  = 0;
  TObject *obj1 = NULL;
  TObject *obj2 = NULL;

  if(dPlotCont->GetPlotCount() < 2) return;
  obj1 = dPlotCont->GetObject(dPlotCont->GetPlotCount()-2);
  obj2 = dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);
  if(obj1 && obj2 && !strcmp(obj1->ClassName(),obj2->ClassName())){
    if(!strcmp(obj1->ClassName(),"TGraphErrors")){
      TGraphErrors *gr1 = (TGraphErrors*)obj1;
      TGraphErrors *gr2 = (TGraphErrors*)obj2;
      range1 = gr1->GetN();
      range2 = gr2->GetN();
      range1 > range2 ? range1 = range2 : range2 = range1;
      x  = new Double_t[range1];  
      y  = new Double_t[range1]; 
      ye = new Double_t[range1]; 
      x1 = new Double_t[range1]; memcpy(x1,gr1->GetX(),range1*sizeof(Double_t));
      y1 = new Double_t[range1]; memcpy(y1,gr1->GetY(),range1*sizeof(Double_t));
      ye1= new Double_t[range1]; memcpy(ye1,gr1->GetEY(),range1*sizeof(Double_t));
      x2 = new Double_t[range1]; memcpy(x2,gr2->GetX(),range1*sizeof(Double_t));
      y2 = new Double_t[range1]; memcpy(y2,gr2->GetY(),range1*sizeof(Double_t));
      ye2= new Double_t[range1]; memcpy(ye2,gr2->GetEY(),range1*sizeof(Double_t));
      for(int i = 0; i < range1; i++) {
	if(y2[i] == 0) {y[i] = 0; ye[i] = 0;}
	else{
	  y[i] = y1[i]/y2[i]; ye[i] = sqrt(ye1[i]*ye1[i]/(y2[i]*y2[i]) + 
					   y1[i]*y1[i]*ye2[i]*ye2[i]/
					   (y2[i]*y2[i]*y2[i]*y2[i]));
	}
	x[i] = x1[i];
      };
      DrawData(x,y,ye,range1,AddGraphs());
    }
    if(!strcmp(obj1->ClassName(),"TGraph")){
      TGraph *gr1 = (TGraph*)obj1;
      TGraph *gr2 = (TGraph*)obj2;
      range1 = gr1->GetN();
      range2 = gr2->GetN();
      range1 > range2 ? range1 = range2 : range2 = range1;
      x  = new Double_t[range1];  
      y  = new Double_t[range1]; 
      x1 = new Double_t[range1]; memcpy(x1,gr1->GetX(),range1*sizeof(Double_t));
      y1 = new Double_t[range1]; memcpy(y1,gr1->GetY(),range1*sizeof(Double_t));
      x2 = new Double_t[range1]; memcpy(x2,gr2->GetX(),range1*sizeof(Double_t));
      y2 = new Double_t[range1]; memcpy(y2,gr2->GetY(),range1*sizeof(Double_t));
      for(int i = 0; i < range1; i++) {
	if(y2[i] == 0) {y[i] = 0; ye[i] = 0;} else y[i] = y1[i]/y2[i]; 
	x[i] = x1[i];
      };
      DrawData(x,y,range1,AddGraphs());
    }
  }

  if(x)   delete[] x;
  if(y)   delete[] y;
  if(ye)  delete[] ye;  
  if(x1)  delete[] x1;
  if(y1)  delete[] y1;
  if(ye1) delete[] ye1;  
  if(x2)  delete[] x2;
  if(y2)  delete[] y2;
  if(ye2) delete[] ye2;  
}

void RSDataWindow::ScaleWindowData()
{
  Double_t *x  = NULL;
  Double_t *y  = NULL;
  Double_t *ye = NULL;
  Double_t scaleF = 1.0;
  Int_t range = 0;
  Int_t retval= 0;

  new RNumberEntryDialog(fClient->GetRoot(), this, "ScaleFactor","RSDataWindow",
			 "Scale Factor Entry",0,&scaleF,&retval,600,300);
  if(!retval) return;

  TObject *obj = dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);
//   if(obj && obj->InheritsFrom("TGraphErrors")){
  if(obj && !strcmp(obj->ClassName(),"TGraphErrors")){
    TGraphErrors *gr = (TGraphErrors*)obj;
    range = gr->GetN();
    x = new Double_t[range]; memcpy(x,gr->GetX(),range*sizeof(Double_t));
    y = new Double_t[range]; memcpy(y,gr->GetY(),range*sizeof(Double_t));
    ye= new Double_t[range]; memcpy(ye,gr->GetEY(),range*sizeof(Double_t));
    for(int i = 0; i < range; i++) {y[i] *= scaleF; ye[i] *= sqrt(scaleF*scaleF);};
    DrawData(x,y,ye,range);
  }
  //if(obj && obj->InheritsFrom("TGraph")){
  if(obj && !strcmp(obj->ClassName(),"TGraph")){
    TGraph *gr = (TGraph*)obj;
    range = gr->GetN();
    x = new Double_t[range]; memcpy(x,gr->GetX(),range*sizeof(Double_t));
    y = new Double_t[range]; memcpy(y,gr->GetY(),range*sizeof(Double_t));
    for(int i = 0; i < range; i++) {y[i] *= scaleF;};
    DrawData(x,y,range);
  }

  if(x)  delete[] x;
  if(y)  delete[] y;
  if(ye) delete[] ye;
}

void RSDataWindow::ViewMiscData()
{
  const char *filetypes[] = { "Data files",     "*.dat",
			      "Data files",     "*.txt",
			      "All files",      "*",
			      0,               0 };  
  static TString dir(".");
  
  TGFileInfo fi;
  fi.fFileTypes = (const char **)filetypes;
  fi.fIniDir    = StrDup(dir);
  new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &fi);
  dir = fi.fIniDir;

  if(!fi.fFilename) return;  

  RDataContainer * dMiscCont = new RDataContainer(fClient->GetRoot(), this,"dMiscCont",
						  "RSDataWindow","Raw Data",FM_READ,
						  FT_ROWCOLUMN);
  if(!dMiscCont) return;

  if(dMiscCont->OpenFile(fi.fFilename) != FILE_PROCESS_OK) {dMiscCont->Close(); return;}
  Int_t range = dMiscCont->GetNumOfRows();
  Int_t cols  = dMiscCont->GetNumOfColumns();
    
  Double_t *x = new Double_t[range];
  Double_t *y = new Double_t[range];
  Double_t *ye = NULL;
  if(dMiscCont->ReadData(x,y,1,1,2,dMiscCont->GetNumOfRows()) != FILE_PROCESS_OK)
    {dMiscCont->Close(); return;}
  if(cols == 3){
    ye= new Double_t[range];
    if(dMiscCont->ReadData(x,ye,1,1,3,dMiscCont->GetNumOfRows()) != FILE_PROCESS_OK)
      {dMiscCont->Close(); return;}
  }
  if(cols > 3){
    ye= new Double_t[range];
    if(dMiscCont->ReadData(x,ye,1,1,4,dMiscCont->GetNumOfRows()) != FILE_PROCESS_OK)
      {dMiscCont->Close(); return;}
  }

  dMiscCont->Close();
  dMiscCont = NULL;

  if(cols > 2)
    DrawData(x,y,ye,range,AddGraphs());
  else
    DrawData(x,y,range,AddGraphs());

  if(x) {delete[] x; x  = NULL;}
  if(y) {delete[] y; y  = NULL;}
  if(ye){delete[] ye; ye  = NULL;}
}

void RSDataWindow::SavePlotObjects()
{
  Int_t retval;
  TObject *obj = NULL;

  if(!dPlotCont) return;
  
  if(!dRootCont || !dRootCont->IsFileOpen()){
    strcpy(dMiscbuffer,"There is currently no file open!\n");
    strcat(dMiscbuffer,"Would you like to open a file?\n");
    new TGMsgBox(fClient->GetRoot(), this,
		 "Save Plot",dMiscbuffer,
		 kMBIconQuestion, kMBYes | kMBNo, &retval);
    if(retval == kMBNo) return;
    else {
      if(!dRootCont){
	if(OpenContainer() == PROCESS_FAILED) {
	  new TGMsgBox(fClient->GetRoot(), this,
		       "Save Plot","Can't Open Data Container!",
		       kMBIconExclamation, kMBOk, &retval);
	  return;
	}
	if(OpenRootFile() == PROCESS_FAILED) {
	  dRootCont->Close();
	  return;
	}
      }
      if(!dRootCont->IsFileOpen()){
	if(OpenRootFile() == PROCESS_FAILED){
	  if(IsRootContInternal() && dRootCont)
	    dRootCont->Close();
	  return;
	}
      }
    }
  }

  if(dRootCont->IsFileOpen()){
    dRootCont->cd();
    if(dRootCont->WriteData(fPlotCanvas) != FILE_PROCESS_OK){
      sprintf(dMiscbuffer,
	      "Can't write object to file %s\nWrite Process Stopped!"
	      ,dRootCont->GetFileName());
      new TGMsgBox(fClient->GetRoot(), this,
		   "File Write Error",dMiscbuffer,
		   kMBIconExclamation, kMBOk, &retval);
      if(IsRootContInternal() && dRootCont)
	dRootCont->Close();
    }

    for(int i = 0; i < dPlotCont->GetPlotCount(); i++){
      obj = dPlotCont->GetObject(i);
      if(obj){
	if(dRootCont->WriteData(obj) != FILE_PROCESS_OK){
	  sprintf(dMiscbuffer,
		  "Can't write objects to file %s\nWrite Process Stopped!"
		  ,dRootCont->GetFileName());
	  new TGMsgBox(fClient->GetRoot(), this,
		       "File Write Error",dMiscbuffer,
		       kMBIconExclamation, kMBOk, &retval);
	  if(IsRootContInternal() && dRootCont)
	    dRootCont->Close();
	  return;
	}
      }
    } 
  }

  if(IsRootContInternal() && dRootCont)
    dRootCont->Close();
}

Int_t RSDataWindow::GetNewLineColor(Color_t col)
{
  return dPlotCont->GetNewLineColor(col);
}

Int_t RSDataWindow::GetNewMarkerColor(Color_t col)
{
  return dPlotCont->GetNewMarkerColor(col);
}

void RSDataWindow::ClearPlots()
{
  if(fLegend != NULL){delete fLegend; fLegend = NULL;};
  dPlotCont->ClearPlots();  

  for(int i = 0; i < 10; i++)
    if(fFitHisto[i] != NULL){delete fFitHisto[i];fFitHisto[i]=NULL;}
  dFitHistocnt = 0;

  for(UInt_t i = 0; i < dErrorBoxArray.size(); i++){
    delete dErrorBoxArray[i];
  }
  dErrorBoxArray.clear();

  delete dDummyPlot;
}

void RSDataWindow::AddMenuPopup(const char* str, TGPopupMenu* popup)
{
  if(!popup) return;
  fMenuBar->AddPopup(str, popup, fMenuBarItemLayout);  
}

void RSDataWindow::AddPopupEntry(const char* str, Int_t ID, TGPopupMenu* popup)
{
  if(!popup){
    fMenuTools->AddEntry(str,ID);
    return;
  }
  popup->AddEntry(str,ID);
}

void RSDataWindow::FlushMessages()
{
  while(GetMessage() != NULL){}
}

char *RSDataWindow::GetMessage()
{
  if(dMsgcnt != 0)
    {
      strcpy(dMiscbuffer,dMessage[0]);
      for(int i = 0; i < dMsgcnt-1; i++)
	strcpy(dMessage[i],dMessage[i+1]);
      memset(dMessage[dMsgcnt-1],'\0',sizeof(dMessage[dMsgcnt-1]));
      dMsgcnt--;

      return dMiscbuffer;
    }

  return NULL;
}


Bool_t RSDataWindow::SetMessage(const char *msg, const char *func, int TS, int MESSAGETYPE)
{
#ifdef RDW_DEBUG
  char str1[NAME_STR_MAX];
  char str2[NAME_STR_MAX];

  if(MESSAGETYPE == M_DTWIND_ERROR_MSG){
    sprintf(str1,"Object: %s\n",GetDataTitle());
    sprintf(str2,"Function: %s\n",func);
  }
#endif

  if(dMsgcnt == MSG_QUEUE_MAX)
    {
      for(int i = 0; i < dMsgcnt-1; i++)
	strcpy(dMessage[i],dMessage[i+1]);
      memset(dMessage[MSG_QUEUE_MAX-1],'\0',sizeof(dMessage[MSG_QUEUE_MAX-1]));
      if(MESSAGETYPE == M_DTWIND_ERROR_MSG){ 
#ifdef RDW_DEBUG
	strcpy(dMessage[MSG_QUEUE_MAX-1],str1);
	strcat(dMessage[MSG_QUEUE_MAX-1],str2);
#endif
	strcat(dMessage[MSG_QUEUE_MAX-1],msg);
      }
      else
	strcpy(dMessage[MSG_QUEUE_MAX-1],msg);
    }
  else
    {
      if(MESSAGETYPE == M_DTWIND_ERROR_MSG){ 
#ifdef RDW_DEBUG
	strcpy(dMessage[dMsgcnt],str1);
	strcat(dMessage[dMsgcnt],str2);
#endif
	strcat(dMessage[dMsgcnt],msg);
      }
      else
	strcpy(dMessage[dMsgcnt],msg);
      dMsgcnt++;
    }
      
  SendMessageSignal(dObjName);
//   gSystem->ProcessEvents();

  return kTrue;
}


void RSDataWindow::SaveCanvas(const char* file)
{
  TString filename;

  if(!file){
    const char *filetypes[] = { "Postscript",     "*.ps",
				"Portable Document Format",     "*.pdf",
				0,               0 };

    static TString dir(".");
    TGFileInfo fi;
    fi.fFileTypes = (const char **)filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(fClient->GetRoot(), this, kFDSave, &fi);
    dir = fi.fIniDir;

    if(fi.fFilename)
      filename = fi.fFilename;
  }
  else
    filename = file;

  fPlotCanvas->SaveAs(filename.Data());
  
  memset(dMiscbuffer,'\0',MSG_SIZE_MAX);
  sprintf(dMiscbuffer,"Plot saved as %s",filename.Data());
  SetMessage(dMiscbuffer,"",(int)dPtype,M_DTWIND_LOGTXTTS);
}

void RSDataWindow::PrintCanvas()
{
  int flag;
  strcpy(dMiscbuffer,"printout.ps");
  fPlotCanvas->Print(dMiscbuffer);
  strcpy(dMiscbuffer,"convert -size 600x424 printout.ps ");
  strcat(dMiscbuffer,"-resize 600x424 smallprintout.ps");
  flag = system(dMiscbuffer);
  strcpy(dMiscbuffer,"lpr smallprintout.ps");
  flag = system(dMiscbuffer);
  strcpy(dMiscbuffer,"rm smallprintout.ps printout.ps");
  flag = system(dMiscbuffer);  
}

void RSDataWindow::SleepWithEvents(int seconds)
{
  time_t start, now;
  time(&start);
  do {
    time(&now);
    gSystem->ProcessEvents();
  } while (difftime(now,start) < seconds) ;
}


void RSDataWindow::CloseWindow()
{
  FlushMessages();
  SetMessage(PLOTWIND_CLOSED_MSG,"",(int)dPtype,M_DTWIND_CLOSED);
  
  IsClosing(dObjName);
  
//   SleepWithEvents(1);

//   TGTransientFrame::CloseWindow();
}

Double_t RSDataWindow::minv(Double_t *x, Int_t size)
{
  Double_t mini = x[0];

  for(int i = 0; i < size; i ++)
    {
      if(x[i] < mini) { mini = x[i];};
    }
      
  return mini;
}


Double_t RSDataWindow::maxv(Double_t *x, Int_t size)
{
  Double_t maxi = x[0];

  for(int i = 0; i < size; i ++)
    {
      if(x[i] > maxi) { maxi = x[i];};
    }
      
  return maxi;
}

void RSDataWindow::GetGraphData(TGraph *gr,Double_t *x, Double_t *y)
{
  Double_t *tmp1 = gr->GetX();
  Double_t *tmp2 = gr->GetY();

  if(gr != NULL){
    for(int i = 0; i < gr->GetN(); i++){
      x = new Double_t[gr->GetN()];
      y = new Double_t[gr->GetN()];
      if(x)
	x[i] = tmp1[i];
      if(y)
	y[i] = tmp2[i];
    }
  }
}

void RSDataWindow::GetGraphData(TGraphErrors *gr,Double_t *x, Double_t *y, 
			       Double_t *xe, Double_t *ye)
{
  Double_t *tmp1 = gr->GetX();
  Double_t *tmp2 = gr->GetY();
  Double_t *tmp3 = gr->GetEX();
  Double_t *tmp4 = gr->GetEY();

  if(gr != NULL){
    for(int i = 0; i < gr->GetN(); i++){

      x  = new Double_t[gr->GetN()];
      y  = new Double_t[gr->GetN()];
      xe = new Double_t[gr->GetN()];
      ye = new Double_t[gr->GetN()];
 
      if(x)
	x[i]  = tmp1[i];
      if(y)
	y[i]  = tmp2[i];
      if(xe)
	ye[i] = tmp3[i];
      if(ye)
	ye[i] = tmp4[i];
    }
  }
}

Int_t RSDataWindow::GetGraphBinRange(TGraph *gr ,Double_t min, Double_t max, 
				    Int_t *minbin, Int_t *maxbin)
{
  Double_t xpl   = 0;
  Double_t ypl   = 0;
  Double_t xph   = 0;
  Double_t yph   = 0;
  Double_t width = 0;
  *minbin = -1;
  *maxbin = -1;

  if(gr != NULL){
    GetGraphMinMax(gr,&xpl,&xph,&ypl,&yph);
    width = (xph-xpl)/gr->GetN();
    if(min >= xph) return DATA_PLOT_ERROR;
    if(max <= xpl) return DATA_PLOT_ERROR;
    for(int i = 0; i < gr->GetN()-1; i++){ 
      if(min >= (xpl + i*width) && 
	 min < (xpl + (i+1)*width))
	*minbin = i;
      if(max >= (xpl + i*width) && 
	 max <  (xpl + (i+1)*width))
	*maxbin = i;
    }
    if(min <= xpl) *minbin = 0;
    if(max >= xph) *maxbin = gr->GetN()-1;

    if(minbin > maxbin || 
       minbin < 0 || maxbin < 0) 
      return DATA_PLOT_ERROR;
  }
  return PLOT_PROCESS_OK;
}

void RSDataWindow::GetGraphMinMax(TGraph* gr,Double_t *xmin, Double_t *xmax,
				 Double_t *ymin, Double_t *ymax)
{
  if(gr != NULL){
    gr->GetPoint(0,*xmin,*ymin);
    gr->GetPoint(gr->GetN()-1,*xmax,*ymax);
    
    Double_t xsep = (xmax-xmin)/(gr->GetN()-1);
    *xmin = *xmin-xsep/2;
    *xmax = *xmin+xsep/2;
  }
}

void RSDataWindow::GetObj(TObject* obj)
{
  TObject *lobj = NULL;

  if(!strcmp((char*)obj->ClassName(),"TKey")){
    TKey *key = (TKey*)obj;
    lobj = key->ReadObj();
  }
  else{
    lobj = obj;
  }

  if(!strcmp((char*)lobj->ClassName(),"TF1")){
    DrawData(*dynamic_cast<TF1*>(lobj),dAddGraphs);
  }
  if(!strcmp((char*)lobj->ClassName(),"TH1D")){
    DrawData(*dynamic_cast<TH1D*>(lobj),dAddGraphs);
  }
  else if(!strcmp((char*)lobj->ClassName(),"TGraph")){
    SetDrawOptions("ap");
    DrawData(*dynamic_cast<TGraph*>(lobj),dAddGraphs);
  }
  else if(!strcmp((char*)lobj->ClassName(),"TGraphErrors")){
    SetDrawOptions("ap");
    DrawData(*dynamic_cast<TGraphErrors*>(lobj),dAddGraphs);
  }
  else if(!strcmp((char*)lobj->ClassName(),"TGraphAsymmErros")){
    SetDrawOptions("ap");
    DrawData(*dynamic_cast<TGraphAsymmErrors*>(lobj),dAddGraphs);
  }
}


Int_t RSDataWindow::DrawData(const TF1& f1, Bool_t add)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  TF1 *fnc = NULL;
  TCanvas *aC = GetPlotCanvas();
  if(!add){
    aC->Clear();
    aC->Update();
  }    
  gPad->SetGrid();

  if(!add){
    if(dFitOptions != NULL){
      delete dFitOptions;
      dFitOptions = NULL;
    }
    ClearPlots();
    fnc = dPlotCont->GetNewFunction(f1);
    if(fnc != NULL){
      fnc->Draw(dDrawOptions);
      SetDrawOptions();
      gPad->Modified();
      gPad->Update();
    }
    else{
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TF1&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
  }
  else if(add){
    fnc = dPlotCont->GetNewFunction(f1);
    if(fnc != NULL){
      TString opts = dDrawOptions;
      if(!opts.Contains("SAME")){strcat(dDrawOptions,"SAME");}
      fnc->Draw(dDrawOptions);
      SetDrawOptions();
      gPad->Modified();
      gPad->Update();
    }
    else {
      fnc = NULL;
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TF1&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }    
  }
  fCurrPlot = fnc;
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::UpdateDrawData(TH1D* h1d)
{

  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }
  
  TCanvas *aC = GetPlotCanvas();
  if(!dUpdateHisto && h1d != NULL){    
    aC->Clear();
    aC->Update();
    gPad->SetGrid();
    ClearPlots();
    h1d->SetLineWidth(dPlotCont->GetPlotOptions()->lineWidth);
    h1d->SetLineStyle(dPlotCont->GetPlotOptions()->lineStyle);
    h1d->SetLineColor(dPlotCont->GetPlotOptions()->lineColor);
    h1d->SetMarkerSize(dPlotCont->GetPlotOptions()->markerSize);
    h1d->SetMarkerStyle(dPlotCont->GetPlotOptions()->markerStyle);
    h1d->SetMarkerColor(dPlotCont->GetPlotOptions()->markerColor);
    dUpdateHisto = 1;
  }
    
  if(h1d != NULL){
    if(IsUserLimitSet()){
      h1d->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      SetLimitsFlag(kFalse);
    }
    h1d->Draw(dDrawOptions);
    aC->Modified();
    aC->Update();
  }
  else {
    FlushMessages();
    SetMessage(ROOTOBJ_CRFAIL_ERROR,"UpdateDrawData(const TH1D*)",(int)dPtype,
	       M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::DrawBox(const TBox& box)
{
  
  TBox *mBox = new TBox(*(TBox*)box.Clone());

  TCanvas *aC = GetPlotCanvas();
  aC->cd();
  // abox = new TBox(-5e-4,dMainHistos.back()->GetMinimum(), 5e-4, dMainHistos.back()->GetMaximum());
  // abox->SetFillColor(2);
  // abox->SetFillStyle(3002);
  mBox->Draw("");
  dErrorBoxArray.push_back(mBox);
  gPad->Modified();
  gPad->Update();

  return PLOT_PROCESS_OK;

}

Int_t RSDataWindow::DrawData(const TH1D& h1d, Bool_t add)
{
  Bool_t Add = add;
  if(!Add) Add = AddGraphs();                 //If not pre-specified, see if the menu option was set

  if(!dPlotCont) return DATA_PLOT_ERROR;      //Make sure we have a plot container
  Int_t cnt  = dPlotCont->Get1DHistoCount();  //How many histograms do we currently have
  if(!cnt) Add = kFalse;                      //If we have none, then there is nothing to add to!
  if(!Add) ClearPlots();                      //If we are really not superimposing them then clear all plots
  TH1D *chst[cnt];                            //Make a temporary container for the histograms
  if(Add)
    for(int i = 0; i < cnt; i++)
      chst[i] = (TH1D*)dPlotCont->GetHistogram(i,"TH1D");

  Color_t lcol = dPlotCont->GetNewLineColor();
  Color_t mcol = dPlotCont->GetNewMarkerColor();

  //See if we can actually make a historgram from the one that was supplied
  TH1D *hist = NULL;
  hist = dPlotCont->GetNew1DHistogram(h1d);
  if(!hist){
    FlushMessages();
    SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TH1D&)",(int)dPtype,
	       M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }
  hist->SetLineColor(lcol);
  hist->SetMarkerColor(mcol);

  if(dFitOptions != NULL){                    //Get rid of any previously specified fit options
    delete dFitOptions;
    dFitOptions = NULL;
  }

  
  delete dDummyPlot;
  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Update();
    
  gPad->SetGrid();

  //If we are not adding (superimposing) histrograms on the same plot, then just plot this one
  //and return!
  if(!Add){
    if(IsUserLimitSet())
      hist->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
    
    if(DrawOptionsSet()){hist->Draw(dDrawOptions);} else hist->Draw();
    SetDrawOptions();
    gPad->Modified();
    gPad->Update();
    DrawLegend();

    gPad->SetLogy(1);
    
    fCurrPlot = hist;
    return PLOT_PROCESS_OK;
  }

  //Otherwise we'll have to establish a new axis range

  Double_t cmean = hist->GetMean();
  Double_t crms = hist->GetRMS();

  Double_t minmean = hist->GetMean();
  Double_t maxmean = hist->GetMean();
  
  Double_t minrms = hist->GetRMS();
  Double_t maxrms = hist->GetRMS();
  
  Double_t cmin = chst[0]->GetXaxis()->GetXmin();
  Double_t cmax = chst[0]->GetXaxis()->GetXmax(); 
  
  Double_t cymin = chst[0]->GetMinimum();
  Double_t cymax = chst[0]->GetMaximum();

  dMin[0] = hist->GetXaxis()->GetXmin();
  dMax[0] = hist->GetXaxis()->GetXmax();
  dMin[1] = hist->GetMinimum();
  dMax[1] = hist->GetMaximum();

  for(int i = 0; i < cnt; i++){
    if(chst[i]){
      cmin = chst[i]->GetXaxis()->GetXmin();
      cmax = chst[i]->GetXaxis()->GetXmax();
      
      cymin = chst[i]->GetMinimum();
      cymax = chst[i]->GetMaximum();
      
      cmean = chst[i]->GetMean();
      crms = chst[i]->GetRMS();
      
      if(cmean < minmean) minmean = cmean;
      if(cmean >= maxmean) maxmean = cmean;

      if(crms < minrms) minrms = crms;
      if(crms >= maxrms) maxrms = crms;
      
      if(cmin < dMin[0]) dMin[0] = cmin;
      if(cmax > dMax[0]) dMax[0] = cmax;
      if(cymin < dMin[1]) dMin[1] = cymin;
      if(cymax > dMax[1]) dMax[1] = cymax;
    }
  }

  dDummyPlot = new TH1D("tmp_hst","",500,dMin[0],dMax[0]);

  dDummyPlot->GetYaxis()->SetRangeUser(dMin[1]+0.1,dMax[1]);
  dDummyPlot->GetXaxis()->SetRangeUser(minmean-10*maxrms,maxmean+10*maxrms);
  dDummyPlot->Draw();
  
  for(int i = 0; i < cnt; i++){
    if(chst[i]){
      chst[i]->Draw("SAME");
    }
  }    
  
  TString opts = dDrawOptions;
  if(!opts.Contains("SAME")){strcat(dDrawOptions,"SAME");}
  hist->Draw(dDrawOptions);
  SetDrawOptions();
  gPad->RedrawAxis();      
  gPad->Modified();
  gPad->Update();
  DrawLegend();
  
  gPad->SetLogy(1);
  
  fCurrPlot = hist;
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::DrawData(const TH2D& h2d)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  TH2D *hist = NULL;
  
  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Update();
  ClearPlots();
    
  gPad->SetGrid();

  hist = dPlotCont->GetNew2DHistogram(h2d);
  
  if(!hist){
    FlushMessages();
    SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TH2D&)",(int)dPtype,
	       M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }

  if(DrawOptionsSet()){hist->Draw(dDrawOptions);} else hist->Draw();
  SetDrawOptions();
  gPad->Modified();
  gPad->Update();

  fCurrPlot = hist;
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::DrawData(const TProfile& prf)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  TProfile *prof = NULL;
  
  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Update();
  ClearPlots();
    
  gPad->SetGrid();

  prof = dPlotCont->GetNew1DProfile(prf);
  
  if(!prof){
    FlushMessages();
    SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TProfile&)",(int)dPtype,
	       M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }

  if(DrawOptionsSet()){prof->Draw(dDrawOptions);} else prof->Draw();
  SetDrawOptions();
  gPad->Modified();
  gPad->Update();

  fCurrPlot = prof;
  return PLOT_PROCESS_OK;
}


Int_t RSDataWindow::DrawData(const TGraph& g1d, Bool_t add, TLegend *leg)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }

  if(!add) add = AddGraphs();
  
  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Flush();
  aC->Update();

  gPad->SetGrid();
 
  if(!add){
    ClearPlots();
    TGraph *gr = dPlotCont->GetNewGraph(g1d);
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
      
    TString opts = dDrawOptions;
    if(!opts.Contains("ap")){strcat(dDrawOptions,"ap");}

    gr->Draw(dDrawOptions);
    SetDrawOptions();
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    if(dTitleOffset[0]) gr->GetXaxis()->SetTitleOffset(dTitleOffset[0]);
    if(dTitleOffset[1]) gr->GetYaxis()->SetTitleOffset(dTitleOffset[1]);
    gr->GetXaxis()->SetTitleSize(0.04);
    gr->GetYaxis()->SetTitleSize(0.04);
    gr->GetXaxis()->SetLabelSize(0.04);
    gr->GetYaxis()->SetLabelSize(0.04);
    gr->GetXaxis()->SetTitleColor(1);
    
    if(IsUserLimitSet()){
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->GetYaxis()->SetRangeUser(dMin[1],dMax[1]);
//       gr->SetMinimum(dMin[1]);
//       gr->SetMaximum(dMax[1]);
    }
//     gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    fCurrPlot = gr;
  }
  else{
    TGraph *tmp = (TGraph*)&g1d;
    TMultiGraph *gr = dPlotCont->AddMultiGraphObject((TObject*)tmp,(char*)tmp->GetTitle(),"TGraph","p");
    
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }

    TString opts = dDrawOptions;
    if(!opts.Contains("a")){strcat(dDrawOptions,"a");}
    gr->Draw(dDrawOptions);

    if(IsUserLimitSet()){

      if(((TGraph*)fCurrPlot)->GetXaxis()->GetXmin() < dMin[0])
	dMin[0] = ((TGraph*)fCurrPlot)->GetXaxis()->GetXmin();
      if(((TGraph*)fCurrPlot)->GetXaxis()->GetXmax() > dMax[0])
	dMax[0] = ((TGraph*)fCurrPlot)->GetXaxis()->GetXmax();

      if(((TGraph*)fCurrPlot)->GetYaxis()->GetXmin() < dMin[1])
	dMin[1] = ((TGraph*)fCurrPlot)->GetYaxis()->GetXmin();
      if(((TGraph*)fCurrPlot)->GetYaxis()->GetXmax() > dMax[1])
	dMax[1] = ((TGraph*)fCurrPlot)->GetXaxis()->GetXmax();
	
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->GetYaxis()->SetRangeUser(dMin[1],dMax[1]);
//       gr->SetMinimum(dMin[1]);
//       gr->SetMaximum(dMax[1]);
    }

    SetDrawOptions();
    gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    fCurrPlot = gr;
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    gr->GetXaxis()->SetTitleColor(1);
    DrawLegend(leg);
  }
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::DrawData(const TMultiGraph& g1d, Bool_t add, TLegend *leg)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }
  
  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Flush();
  aC->Update();

  gPad->SetGrid();
 
  if(!add){
    ClearPlots();
    TMultiGraph *gr = NULL;
    TList *graphs = g1d.GetListOfGraphs();
    if(graphs){
      for(int i = 0; i < graphs->GetSize(); i++){
	  gr = dPlotCont->AddMultiGraphObject(graphs->At(i),"",
					      (char *)graphs->At(i)->ClassName(),"p");
      }	 
    }

    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
    if(IsUserLimitSet()){
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->SetMinimum(dMin[1]);
      gr->SetMaximum(dMax[1]);
    }
      
    TString opts = dDrawOptions;
    if(!opts.Contains("ap")){strcat(dDrawOptions,"ap");}
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    gr->GetXaxis()->SetTitleColor(1);

    gr->Draw(dDrawOptions);
    SetDrawOptions();
    gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    fCurrPlot = gr;
    DrawLegend(leg);
  }
  return PLOT_PROCESS_OK;
}


Int_t RSDataWindow::DrawData(const TGraphErrors& g1d, Bool_t add, TLegend *leg)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }

  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Flush();
  aC->Update();

  gPad->SetGrid();

  if(!add){
    ClearPlots();
    TGraphErrors *gr = dPlotCont->GetNewErrorGraph(g1d);
    gr->SetMarkerSize();
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
    if(IsUserLimitSet()){
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->SetMinimum(dMin[1]);
      gr->SetMaximum(dMax[1]);
    }
      
    TString opts = dDrawOptions;
    if(!opts.Contains("ap")){strcat(dDrawOptions,"ap");}
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    gr->GetXaxis()->SetTitleColor(1);
    gr->Draw(dDrawOptions);
    gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    SetDrawOptions();
    fCurrPlot = gr;
  }
  else{
    TGraph *tmp = (TGraph*)&g1d;
    TMultiGraph *gr = dPlotCont->AddMultiGraphObject((TObject*)tmp,(char*)tmp->GetTitle(),
						     "TGraphErrors","p");
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
    if(IsUserLimitSet()){
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->SetMinimum(dMin[1]);
      gr->SetMaximum(dMax[1]);
    }
    TString opts = dDrawOptions;
    if(!opts.Contains("a")){strcat(dDrawOptions,"a");}
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    gr->GetXaxis()->SetTitleColor(1);
    gr->Draw(dDrawOptions);
    SetDrawOptions();
    gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    fCurrPlot = gr;
    DrawLegend(leg);
  }
  return PLOT_PROCESS_OK;
}



Int_t RSDataWindow::DrawData(const TGraphAsymmErrors& g1d, Bool_t add, TLegend *leg)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }

  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Flush();
  aC->Update();
  
  gPad->SetGrid();

  if(!add){
    ClearPlots();
    TGraphAsymmErrors *gr = dPlotCont->GetNewAsymErrorGraph(g1d);
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
    if(IsUserLimitSet()){
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->SetMinimum(dMin[1]);
      gr->SetMaximum(dMax[1]);
    }
      
    TString opts = dDrawOptions;
    if(!opts.Contains("a")){strcat(dDrawOptions,"a");}
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    gr->GetXaxis()->SetTitleColor(1);

    gr->Draw(dDrawOptions);
    SetDrawOptions();
    gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    fCurrPlot = gr;
  }
  else{
    TGraph *tmp = (TGraph*)&g1d;
    TMultiGraph *gr = dPlotCont->AddMultiGraphObject((TObject*)tmp, (char*)tmp->GetTitle(),
						     "TGraphAsymmErrors","p");
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }
    if(IsUserLimitSet()){
      gr->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
      gr->SetMinimum(dMin[1]);
      gr->SetMaximum(dMax[1]);
    }
    TString opts = dDrawOptions;
    if(!opts.Contains("a")){strcat(dDrawOptions,"a");}
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    gr->GetXaxis()->SetTitleColor(1);
    gr->Draw(dDrawOptions);
    SetDrawOptions();
    gPad->RedrawAxis();      
    gPad->Modified();
    gPad->Update();
    fCurrPlot = gr;
    DrawLegend(leg);
  }
  return PLOT_PROCESS_OK;
}


Int_t RSDataWindow::DrawData(Double_t *y, Int_t range , Bool_t add, TLegend *leg)
{
  TH1D *hist = NULL;

  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dPtype != PT_HISTO_1D) {
    FlushMessages();
    SetMessage(PLOT_TYPE_ERROR,"DrawData(double,double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }
  if(!y){
    FlushMessages();
    SetMessage(PARAM_NULL_ERROR,"DrawData(double,double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }

  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }
  TCanvas *aC = GetPlotCanvas();
  if(!add){
    aC->Clear();
    aC->Flush();
    aC->Update();
  }
  gPad->SetGrid();

  if(!add){
    if(!IsUserLimitSet()){
      dMin[0] =  minv(y,range)-(maxv(y,range) - minv(y,range))/2;
      dMax[0] =  maxv(y,range)+(maxv(y,range) - minv(y,range))/2; 
      dNumbins[0] = 10000;
      SetDrawOptions();
    } 
    ClearPlots();
  }
  else {
    if(!IsUserLimitSet()){
      if(dMin[0] >= minv(y,range))
	dMin[0] = minv(y,range)-(maxv(y,range) - minv(y,range))/2;
      if(dMax[0] <= maxv(y,range))
	dMax[0] = maxv(y,range)+(maxv(y,range) - minv(y,range))/2;
      dNumbins[0] = 10000;
      SetDrawOptions("SAME");
    }
    TString opts = dDrawOptions;
    if(!opts.Contains("SAME")){strcat(dDrawOptions,"SAME");} 
  }

  if(!strcmp(GetPlotName(),"none")){
    sprintf(dMiscbuffer,"hist_%d",dPlotCont->Get1DHistoCount());
    SetPlotName(dMiscbuffer);
  }
  hist = dPlotCont->GetNew1DHistogram(GetPlotName(),GetPlotTitle(),dNumbins[0],
				      dMin[0],dMax[0]);

  if(!hist){
    FlushMessages();
    SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }

  for(int n = 0; n < range; n++) {hist->Fill(y[n]);}
  if(strcmp(GetPlotTitleX(),"none")) hist->GetXaxis()->SetTitle(GetPlotTitleX());
  if(strcmp(GetPlotTitleY(),"none")) hist->GetYaxis()->SetTitle(GetPlotTitleY());
  hist->Draw(dDrawOptions);
  SetDrawOptions();
  
  gPad->RedrawAxis();	      	      
  gPad->Modified();
  gPad->Update(); 
  fCurrPlot = hist;
  
  memset(dMiscbuffer,'\0',MSG_SIZE_MAX);
  sprintf(dMiscbuffer,"Plotting %s\n",dDatatitle);
  SetMessage(dMiscbuffer,"",(int)dPtype,M_DTWIND_LOGTXTTS);
  
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::DrawData(Double_t *x, Double_t *y, Int_t range, Bool_t add, TLegend *leg)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dPtype != PT_GRAPH) {
    FlushMessages();
    SetMessage(PLOT_TYPE_ERROR,"DrawData(double,double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }
  if(!y || !x){
    FlushMessages();
    SetMessage(PARAM_NULL_ERROR,"DrawData(double,double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }

  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }

  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Flush();
  aC->Update();
  gPad->SetGrid();

  if(!strcmp(GetPlotName(),"none")){
    sprintf(dMiscbuffer,"graph_%d",dPlotCont->GetGraphCount());
    SetPlotName(dMiscbuffer);
  }

  TString opts = dDrawOptions;
  if(!opts.Contains("a")){strcat(dDrawOptions,"a");}

  if(!add){
    ClearPlots();
    TGraph *gr = dPlotCont->GetNewGraph(GetPlotName(),GetPlotTitle(),range,
					x,y);
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }

    if(!IsUserLimitSet()){
      dMin[0] =  minv(x,range)-(maxv(x,range) - minv(x,range))/2;
      dMax[0] =  maxv(x,range)+(maxv(x,range) - minv(x,range))/2;
      dMin[1] =  minv(y,range)-(maxv(y,range) - minv(y,range))/2;
      dMax[1] =  maxv(y,range)+(maxv(y,range) - minv(y,range))/2; 
      strcpy(dDrawOptions,"ap");
    } 
    gr->Draw(dDrawOptions);  
    gr->GetXaxis()->SetRangeUser(dMin[0],
				 dMax[0]);
    gr->SetMinimum(dMin[1]);
    gr->SetMaximum(dMax[1]);
    fCurrPlot = gr;
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
  }
  else{
//     TGraph *tmp = NULL;
//     TObject *obj = dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);
//     if(obj && obj->InheritsFrom("TGraph")) tmp = new TGraph(*(TGraph*)obj->Clone());
//     ClearPlots();
//     if(tmp) dPlotCont->AddMultiGraphObject(tmp,"","TGraph","p");
//     delete tmp;
    TMultiGraph *gr = dPlotCont->AddMultiGraphObject(GetPlotName(),GetPlotTitle(),
						     range,x,y,0,0,0,0,"TGraph","p");
    if(!gr){
      FlushMessages();
      SetMessage(PLOT_ADD_ERROR,"DrawData(double,double,int)",
		 (int)dPtype,M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;      
    }

    if(!IsUserLimitSet()){
      if(dMin[0] >= minv(x,range))
	dMin[0] = minv(x,range)-(maxv(x,range) - minv(x,range))/2;
      if(dMax[0] <= maxv(x,range))
	dMax[0] = maxv(x,range)+(maxv(x,range) - minv(x,range))/2;
      if(dMin[1] >= minv(y,range))
	dMin[1] = minv(y,range)-(maxv(y,range) - minv(y,range))/2;
      if(dMax[1] <= maxv(y,range))
	dMax[1] = maxv(y,range)+(maxv(y,range) - minv(y,range))/2;
      strcpy(dDrawOptions,"ap");
    }
    gr->Draw(dDrawOptions);  
    gr->GetXaxis()->SetRangeUser(dMin[0],
				 dMax[0]);
    gr->SetMinimum(dMin[1]);
    gr->SetMaximum(dMax[1]);
    fCurrPlot = gr;
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    DrawLegend(leg);
  }

  gPad->RedrawAxis();      
  gPad->Modified();
  gPad->Update();
    
  memset(dMiscbuffer,'\0',MSG_SIZE_MAX);
  sprintf(dMiscbuffer,"Plotting %s\n",dDatatitle);
  SetMessage(dMiscbuffer,"",(int)dPtype,M_DTWIND_LOGTXTTS);
  
  return PLOT_PROCESS_OK;
}

Int_t RSDataWindow::DrawData(Double_t *x, Double_t *y, Double_t *ye, Int_t range, Bool_t add, TLegend *leg)
{
  if(!dPlotCont) return DATA_PLOT_ERROR;
  if(dPtype != PT_GRAPH) {
    FlushMessages();
    SetMessage(PLOT_TYPE_ERROR,"DrawData(double,double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }
  if(!y || !x){
    FlushMessages();
    SetMessage(PARAM_NULL_ERROR,"DrawData(double,double,int)",
	       (int)dPtype,M_DTWIND_ERROR_MSG);
    return DATA_PLOT_ERROR;
  }

  if(dFitOptions != NULL){
    delete dFitOptions;
    dFitOptions = NULL;
  }

  TCanvas *aC = GetPlotCanvas();
  aC->Clear();
  aC->Flush();
  aC->Update();
  gPad->SetGrid();

  if(!strcmp(GetPlotName(),"none")){
    sprintf(dMiscbuffer,"graph_%d",dPlotCont->GetGraphCount());
    SetPlotName(dMiscbuffer);
  }

  TString opts = dDrawOptions;
  if(!opts.Contains("a")){strcat(dDrawOptions,"a");}

  if(!add){
    ClearPlots();
    TGraph *gr = dPlotCont->GetNewErrorGraph(GetPlotName(),GetPlotTitle(),range,
					     x,y,0,ye);
    if(!gr){
      FlushMessages();
      SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TGraph&)",(int)dPtype,
		 M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;
    }

    if(!IsUserLimitSet()){
      dMin[0] =  minv(x,range)-(maxv(x,range) - minv(x,range))/2;
      dMax[0] =  maxv(x,range)+(maxv(x,range) - minv(x,range))/2;
      dMin[1] =  minv(y,range)-(maxv(y,range) - minv(y,range))/2;
      dMax[1] =  maxv(y,range)+(maxv(y,range) - minv(y,range))/2; 
      strcpy(dDrawOptions,"ap");
    } 
    gr->Draw(dDrawOptions);  
    gr->GetXaxis()->SetRangeUser(dMin[0],
				 dMax[0]);
    gr->SetMinimum(dMin[1]);
    gr->SetMaximum(dMax[1]);
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    fCurrPlot = gr;
  }
  else{
    TMultiGraph *gr = dPlotCont->AddMultiGraphObject(GetPlotName(),GetPlotTitle(),
						     range,x,y,0,ye,0,ye,"TGraphErrors","p");
    if(!gr){
      FlushMessages();
      SetMessage(PLOT_ADD_ERROR,"DrawData(double,double,int)",
		 (int)dPtype,M_DTWIND_ERROR_MSG);
      return DATA_PLOT_ERROR;      
    }

    if(!IsUserLimitSet()){
      if(dMin[0] >= minv(x,range))
	dMin[0] = minv(x,range)-(maxv(x,range) - minv(x,range))/2;
      if(dMax[0] <= maxv(x,range))
	dMax[0] = maxv(x,range)+(maxv(x,range) - minv(x,range))/2;
      if(dMin[1] >= minv(y,range))
	dMin[1] = minv(y,range)-(maxv(y,range) - minv(y,range))/2;
      if(dMax[1] <= maxv(y,range))
	dMax[1] = maxv(y,range)+(maxv(y,range) - minv(y,range))/2;
      strcpy(dDrawOptions,"ap");
    }

    gr->Draw(dDrawOptions);  
    gr->GetXaxis()->SetRangeUser(dMin[0],
				 dMax[0]);
    gr->SetMinimum(dMin[1]);
    gr->SetMaximum(dMax[1]);
    fCurrPlot = gr;
    if(strcmp(GetPlotTitleX(),"none")) gr->GetXaxis()->SetTitle(GetPlotTitleX());
    if(strcmp(GetPlotTitleY(),"none")) gr->GetYaxis()->SetTitle(GetPlotTitleY());
    DrawLegend(leg);
  }

  gPad->RedrawAxis();      
  gPad->Modified();
  gPad->Update();
    
  memset(dMiscbuffer,'\0',MSG_SIZE_MAX);
  sprintf(dMiscbuffer,"Plotting %s\n",dDatatitle);
  SetMessage(dMiscbuffer,"",(int)dPtype,M_DTWIND_LOGTXTTS);
  
  return PLOT_PROCESS_OK;
}


Int_t RSDataWindow::DrawLegend(TLegend *leg)
{
  TObject *cplot = NULL;
  TCanvas *aC = GetPlotCanvas();
  aC->cd();

  if(leg){
    leg->Draw("");
  }
  else{

    if(fLegend){delete fLegend; fLegend = NULL;}

    fLegend = new TLegend(0.58,0.82,0.98,0.98);

    if(fLegend){
      fLegend->SetY1NDC(fLegend->GetY1NDC()*3/2-fLegend->GetY2NDC()/2);
            	  	  
      Int_t pcnt = dPlotCont->GetPlotCount();
	  
      for(int i = 0; i < pcnt; i++){
	    
	cplot = dPlotCont->GetPlot(i);
	
	if(cplot){
	  if(cplot->InheritsFrom("TF1"))
	    fLegend->AddEntry(cplot,
			      cplot->GetTitle(),"l");
	  else if(cplot->InheritsFrom("TH1")){
	    fLegend->AddEntry(cplot,
			      cplot->GetTitle(),"l");
	  }
	  else if(cplot->InheritsFrom("TGraphErrors"))
	    fLegend->AddEntry(cplot,
			      cplot->GetTitle(),"lp");	 
	  else if(cplot->InheritsFrom("TGraph"))
	    fLegend->AddEntry(cplot,
			      cplot->GetTitle(),"p");
	  else if(cplot->InheritsFrom("TMultiGraph")){
	    TMultiGraph *mgr = (TMultiGraph *)fCurrPlot;
	    TList *graphs = NULL;
	    if(mgr) graphs = mgr->GetListOfGraphs();
	    if(graphs){
	      for(int i = 0; i < graphs->GetSize(); i++){
		fLegend->AddEntry(graphs->At(i),
				  graphs->At(i)->GetTitle(),"lp");
	      }	 
	    }
	  }
	}
      }
      fLegend->SetTextFont(62);
      fLegend->SetTextSize(0.04);
      fLegend->Draw();
    }
  }
  
  gPad->Modified();
  gPad->Update();

  return PLOT_PROCESS_OK;
}

char *RSDataWindow::GetFitLog()
{
  if(dDatafit != NULL)
    return dDatafit->GetFitLog();
  
  return NULL;
}

Double_t RSDataWindow::OneMinusCos(Double_t *x, Double_t *parm)
{
  //Since this is a 1D fit function, only x[0] is used
  Double_t fitval = parm[0]*cos(parm[1]*(x[0]-parm[2]));//parm[0]*cos(x[0])+parm[1]*sin(x[0]);
//   Double_t fitval = parm[0]*(1.0-cos(parm[1]*(x[0]-parm[2])));
  return fitval;
}

Double_t RSDataWindow::APlusBExp(Double_t *x, Double_t *parm)
{
  Double_t fitval = parm[0] + exp(parm[1]*x[0]);
  return fitval;
}

Int_t RSDataWindow::FitData()
{
  char *func = new char[200];
  memset(func,'\0',sizeof(char)*200);
//  Bool_t retflag;

//   FitOptions *foptions = new FitOptions;
//   FitOptions *tempOpts = foptions;

//   new RDataFitEntry(fClient->GetRoot(), this, NULL, "RSDataWindow",
// 		    foptions,kFalse,400,600);

//   if(!foptions) {delete tempOpts; return PROCESS_FAILED;}

  
//   printf("func  = %s\n",foptions->func);
//   for(int i = 0; i < foptions->npars; i++)
//     printf("parm%d = %f\n",i,foptions->param[i]);


//   new RStringEntryDialog(fClient->GetRoot(), this,"stringentry","RSDataWindow",
// 			 &retflag,func,"Fit Function",400,200);
//   if(!retflag) return PROCESS_FAILED;

  TObject *obj = dPlotCont->GetObject(dPlotCont->GetPlotCount()-1);
  if(obj && !strcmp(obj->ClassName(),"TGraphErrors")){
    TGraphErrors *gr = (TGraphErrors*)obj;


    Double_t XMin = gr->GetXaxis()->GetBinLowEdge(gr->GetXaxis()->GetFirst());
    Double_t XMax = gr->GetXaxis()->GetBinUpEdge(gr->GetXaxis()->GetLast());
    printf("Func = %s\n, XMin = %f; Xmax = %f\n",func,XMin,XMax);
    
    TF1 f1("TofEffFit","[0]+exp([1]+[2]*x)",XMin,XMax);
    
    //TF1 f1("TofEffFit","[0]+exp([1]+[2]*x)",36.2,39.8);
//     TF1 f1("TofEffFit","[0]*exp(-[1]*x)*cosh([1]*[2]*x)",12,30);


    /* The following line gives a good fit to M2/M1 in run 8017 */
    //     TF1 f1("TofEffFit","[0]*exp(-0.0745*x)*cosh(0.0745*[1]*x)",18,32);
    //     f1.SetParameter(0,0.83);
    //     f1.SetParameter(1,0.55);
    /* Now we are trying to fit M3/M1 */
    //     TF1 f1("TofEffFit","[0]*exp(-0.0778*x)*cosh(0.0778*[1]*x)",10,30);
    //     f1.SetParameter(0,10.0);
    //     f1.SetParameter(1,0.55);
    /* This is the fit for polarization zero */
//     TF1 f1("TofEffFit","[0]*exp(-[1]*x)",10,30);
//     f1.SetParameter(0,0.83);
//     f1.SetParameter(1,0.745);
 //    TF1 f1("TofEffFit","cosh(0.0761*[0]*x)",1,33);
//     f1.SetParameter(0,0.5);
    gr->Fit(f1.GetName(),"R+");
  }
  return PROCESS_OK;
}


Bool_t RSDataWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) 
{
  // Process messages coming from widgets associated with the dialog.

  switch (GET_MSG(msg)) {

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:
      switch(parm1) {
      }  
    case kCM_MENU:
      switch (parm1) {
	
      case M_PLOT_CLOSE:
	CloseWindow();
	break;
	
      case M_PLOT_ADD:
	if(fMenuTools->IsEntryChecked(M_PLOT_ADD)){
	  fMenuTools->UnCheckEntry(M_PLOT_ADD);
	  SetAddGraphs(kFalse);
	  SendMessageSignal(Form("Don't add to %s",dObjName));
	}
	else{
	  fMenuTools->CheckEntry(M_PLOT_ADD);
	  SetAddGraphs(kTrue);
	  // dPlotCont->SetNewMarkerColor();
	  // dPlotCont->SetNewLineColor();
	  SendMessageSignal(Form("Add to %s",dObjName));
	}
	break;

      case M_PLOT_ADDLEGEND:
	DrawLegend();
	break;

      case M_PLOT_SAVEPS:
	SaveCanvas();
	break;

      case M_PLOT_PRINT:
	PrintCanvas();
	break;
	
      case M_PLOT_SAVERT:
	SavePlotObjects();
	break;

      case M_PLOT_WRITEMISC:
	WriteMiscData();
	break;

      case M_PLOT_READMISC:
	ViewMiscData();
	break;

      case M_PLOT_FITSETUP:
	FitData();
	break;

      case M_PLOT_NORM:
	NormalizeWindowData();
	break;

      case M_PLOT_VIEWROOTOBJ:
	GetRootObjectList(dRootCont);
	break;

      case M_PLOT_SCALE:
	ScaleWindowData();
	break;
	
      case M_PLOT_DIVIDE:
	DivideWindowData();
	break;

      case M_PLOT_INTEG:
	IntegrateWindowData();
	break;

      case M_PLOT_EDITOR:
	GetPlotCanvas()->EditorBar();
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





// Int_t RSDataWindow::DrawData(const TH1D& h1d, Bool_t add)
// {
//   if(!dPlotCont) return DATA_PLOT_ERROR;
//   Bool_t Add = add;
//   Int_t cnt  = dPlotCont->Get1DHistoCount();
//   if(!cnt) Add = kFalse; //Nothing to add to!
//   TH1D *hist = NULL;
//   TH1D *chst[cnt];
//   if(!Add) Add = AddGraphs(); //If not pre-specified, see if the menu option was set
//   if(dFitOptions != NULL){
//     delete dFitOptions;
//     dFitOptions = NULL;
//   }

//   if(Add){ 
//     for(int i = 0; i < cnt; i++){
//       chst[i] = (TH1D*)dPlotCont->GetHistogram(i,"TH1D");
//     }
//     dPlotCont->RemovePlot(chst[0]);
//   }
  
//   TCanvas *aC = GetPlotCanvas();
//   if(!Add){
//     aC->Clear();
//     aC->Update();
//     ClearPlots();
//   }
    
//   gPad->SetGrid();


//   hist = dPlotCont->GetNew1DHistogram(h1d);
//   //hist->SetTitle(GetPlotTitle());
//   if(!hist){
//     FlushMessages();
//     SetMessage(ROOTOBJ_CRFAIL_ERROR,"DrawData(const TH1D&)",(int)dPtype,
// 	       M_DTWIND_ERROR_MSG);
//     return DATA_PLOT_ERROR;
//   }

//   if(!Add){
//     if(IsUserLimitSet())
//       hist->GetXaxis()->SetRangeUser(dMin[0],dMax[0]);
    
//     if(DrawOptionsSet()){hist->Draw(dDrawOptions);} else hist->Draw();
//     SetDrawOptions();
//     gPad->Modified();
//     gPad->Update();
//   }
//   else{

//     if(!chst[0]) return DATA_PLOT_ERROR;

//     Double_t cmean = hist->GetMean();
//     Double_t crms = hist->GetRMS();

//     Double_t minmean = hist->GetMean();
//     Double_t maxmean = hist->GetMean();

//     Double_t minrms = hist->GetRMS();
//     Double_t maxrms = hist->GetRMS();

//     Double_t cmin = chst[0]->GetXaxis()->GetXmin();
//     Double_t cmax = chst[0]->GetXaxis()->GetXmax(); 
//     Double_t hmin = hist->GetXaxis()->GetXmin();
//     Double_t hmax = hist->GetXaxis()->GetXmax();

//     Double_t cymin = chst[0]->GetMinimum();
//     Double_t cymax = chst[0]->GetMaximum();
//     Double_t hymin = hist->GetMinimum();
//     Double_t hymax = hist->GetMaximum();

//     dMin[0] = hmin;
//     dMax[0] = hmax;
//     dMin[1] = hymin;
//     dMax[1] = hymax;

//     for(int i = 0; i < cnt; i++){
//       if(chst[i]){
// 	cmin = chst[i]->GetXaxis()->GetXmin();
// 	cmax = chst[i]->GetXaxis()->GetXmax();

// 	cymin = chst[i]->GetMinimum();
// 	cymax = chst[i]->GetMaximum();

// 	cmean = chst[i]->GetMean();
// 	crms = chst[i]->GetRMS();

// 	if(cmean < minmean) minmean = cmean;
// 	if(cmean >= maxmean) maxmean = cmean;

// 	if(crms < minrms) minrms = crms;
// 	if(crms >= maxrms) maxrms = crms;

// 	if(cmin < dMin[0]) dMin[0] = cmin;
// 	if(cmax > dMax[0]) dMax[0] = cmax;
// 	if(cymin < dMin[1]) dMin[1] = cymin;
// 	if(cymax > dMax[1]) dMax[1] = cymax;
//       }
//     }

//     cmin = chst[0]->GetXaxis()->GetXmin();
//     cmax = chst[0]->GetXaxis()->GetXmax();
//     TH1D *tmp = dPlotCont->GetNew1DHistogram((char*)chst[0]->GetName(),(char*)chst[0]->GetTitle(),
// 					     (Int_t)( (dMax[0] - dMin[0])*chst[0]->GetNbinsX()/(cmax - cmin)),
// 					     dMin[0],dMax[0]);
    
//     for(int n = 0; n < chst[0]->GetNbinsX(); n++){
// 	tmp->AddBinContent(tmp->FindBin(chst[0]->GetBinCenter(n)),chst[0]->GetBinContent(n));
//     }    
// //     tmp->SetLineColor(dPlotCont->GetNewLineColor(kRed));
//     tmp->GetYaxis()->SetRangeUser(dMin[1]+0.1,dMax[1]);
//     tmp->GetXaxis()->SetRangeUser(minmean-10*maxrms,maxmean+10*maxrms);
//     tmp->Draw();
//     delete chst[0];

//     for(int i = 1; i < cnt; i++){
//       if(chst[i]){
// 	chst[i]->SetLineColor(dPlotCont->GetNewLineColor());
// 	chst[i]->Draw("SAME");
//       }
//     }    

//     TString opts = dDrawOptions;
//     if(!opts.Contains("SAME")){strcat(dDrawOptions,"SAME");}
//     hist->SetLineColor(dPlotCont->GetNewLineColor(0));
//     hist->Draw(dDrawOptions);
//     SetDrawOptions();
//     gPad->RedrawAxis();      
//     gPad->Modified();
//     gPad->Update();
//     printf("Line 1449\n");
//     DrawLegend();

//   }
//   gPad->SetLogy(1);

//   fCurrPlot = hist;
//   return PLOT_PROCESS_OK;
// }
