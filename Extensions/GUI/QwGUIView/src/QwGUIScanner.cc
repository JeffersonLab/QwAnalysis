#include <QwGUIScanner.h>

ClassImp(QwGUIScanner);

using namespace QwParityDB;

const char *QwGUIScanner::ScannerDataNames[SCANNER_INDEX] =
  {"RateMap","RateMapProjectionXY","RateMapProjectionX","RateMapProjectionY"};

QwGUIScanner::QwGUIScanner(const TGWindow *p, const TGWindow *main, const TGTab *tab,
                           const char *objName, const char *mainname, UInt_t w, UInt_t h)
    : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{
  dTabFrame = NULL;
  dCanvas = NULL;
  dTabLayout = NULL;
  dCnvLayout = NULL;
  dNumberEntryDlg = NULL;

  AddThisTab(this);

  ClearRootData();
  ClearDBData();

}

QwGUIScanner::~QwGUIScanner()
{
  if (dTabFrame)  delete dTabFrame;
  if (dCanvas)    delete dCanvas;
  if (dTabLayout) delete dTabLayout;
  if (dCnvLayout) delete dCnvLayout;

  RemoveThisTab(this);
  IsClosing(GetName());
  ClearRootData();
  ClearDBData();
}

void QwGUIScanner::MakeLayout()
{
  dTabLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                 kLHintsExpandX | kLHintsExpandY);
  dCnvLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
                                 0, 0, 1, 2);

  dTabFrame = new TGVerticalFrame(this,10,10);


  dMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                                     0, 0, 1, 1);
  dMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

  dMenuPlot = new TGPopupMenu(fClient->GetRoot());
  dMenuPlot->AddEntry("&Rate Map", SCANNER_PLOT_RATEMAP);
  dMenuPlot->AddEntry("&Projection", SCANNER_PLOT_PROJECTION);
  //dMenuPlot->AddEntry("&Histograms (raw data)", SCANNER_PLOT_RAW);
  //dMenuPlot->AddSeparator();

  dMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  dMenuBar->AddPopup("&Plots", dMenuPlot, dMenuBarItemLayout);

  dTabFrame->AddFrame(dMenuBar, dMenuBarLayout);
  dMenuPlot->Associate(this);

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,10, 10);
  dTabFrame->AddFrame(dCanvas,dCnvLayout);
  dTabFrame->Resize(GetWidth(),GetHeight());
  AddFrame(dTabFrame,dTabLayout);

  dMenuPlot->CheckEntry(SCANNER_PLOT_RATEMAP);

  dCanvas->GetCanvas()->SetBorderMode(0);
  dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
                                "QwGUIScanner",
                                this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

  TCanvas *mc = dCanvas->GetCanvas();
  mc->Divide(1,2);

}

void QwGUIScanner::OnReceiveMessage(char *obj)
{
//   TString name = obj;
//   char *ptr = NULL;

}

void QwGUIScanner::OnObjClose(char *obj)
{
  if (!obj) return;
  TString name = obj;

  if (name.Contains("dDataWindow"))
    {
      QwGUIDataWindow* window = (QwGUIDataWindow*)DataWindowArray.Remove(DataWindowArray.FindObject(obj));
      if (window) delete window;
    }

  if (!strcmp(obj,"dNumberEntryDlg"))
    {
      delete dNumberEntryDlg;
      dNumberEntryDlg = NULL;
    }

  if (!strcmp(obj,"dProgrDlg"))
    {
      dProcessHalt = kTrue;
      dProgrDlg = NULL;
    }

  QwGUISubSystem::OnObjClose(obj);
}

void QwGUIScanner::OnNewDataContainer(RDataContainer *cont)
{

  if (!cont) return;

  TObject *obj;
  TTree *tree;
  TH1D *hst;
  TGraph *grp;
  TProfile *prf;
  TProfile2D *prf2d;

  if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont)
    {

      obj = dROOTCont->ReadData("event_tree");
      if (obj)
        {
          printf("Reading data from 'tree'\n");
          RunMode = EVENT_MODE;
        }
      else
        {
          obj = dROOTCont->ReadData("Mps_Tree");
          if (obj)
            {
              printf("Reading data from 'Mps_Tree'\n");
              RunMode = CURRENT_MODE;
            }

          if (obj)
            {
              if (obj->InheritsFrom("TTree"))
                {
                  tree = (TTree*)obj->Clone();

                  ClearRootData();

                  tree->SetBranchAddress("CodaEventNumber",&CodaEventNumber);
                  tree->SetBranchAddress("scanner",&scanner);

                  RateMap  = new TProfile2D("Scanner_Rate_Map_CM",
                                            "Scanner Rate Map (Current Mode)",210,-105.0,105.0,40,-360.0,-320.0);

                  RateMap_FrontQuartz  = new TProfile2D("Scanner_Rate_Map_CM_FrontQuartz",
                                                        "Scanner Rate Map (Current Mode, Measured with Front Quartz)",210,-105.0,105.0,40,-360.0,-320.0);

                  RateMap_BackQuartz  = new TProfile2D("Scanner_Rate_Map_CM_BackQuartz",
                                                       "Scanner Rate Map (Current Mode, Measured with Back Quartz)",210,-105.0,105.0,40,-360.0,-320.0);

                  // loop over all entries
                  UInt_t NumberOfEntries = tree->GetEntries();

                  for (Int_t jentry=0; jentry<NumberOfEntries; jentry++)
                    {

                      Int_t entryNumber = tree->GetEntryNumber(jentry);
                      if (entryNumber%1000==0)
                        std::cout << "entryNumber: " << entryNumber << std::endl;

                      tree->GetEntry(entryNumber);

                      UInt_t index = -1;

                      PowerSupply_VQWK = scanner[++index];
                      PositionX_VQWK   = scanner[++index];
                      PositionY_VQWK   = scanner[++index];
                      FrontSCA         = scanner[++index];
                      BackSCA          = scanner[++index];
                      CoincidenceSCA   = scanner[++index];
                      FrontADC         = scanner[++index];
                      BackADC          = scanner[++index];
                      FrontTDC         = scanner[++index];
                      BackTDC          = scanner[++index];
                      PositionX_QDC    = scanner[++index];
                      PositionY_QDC    = scanner[++index];
                      front_adc_raw    = scanner[++index];
                      back__adc_raw    = scanner[++index];
                      pos_x_adc_raw    = scanner[++index];
                      pos_y_adc_raw    = scanner[++index];
                      front_f1_raw     = scanner[++index];
                      back__f1_raw     = scanner[++index];
                      coinc_f1_raw     = scanner[++index];
                      ref_t_f1_raw     = scanner[++index];
                      bg_wilbr_sca_raw = scanner[++index];
                      bg_chrlt_sca_raw = scanner[++index];
                      randm_sca_raw    = scanner[++index];
                      coinc_sca_raw    = scanner[++index];
                      back__sca_raw    = scanner[++index];
                      front_sca_raw    = scanner[++index];
                      phase_monitor_raw= scanner[++index];
                      power_vqwk_raw   = scanner[++index];
                      pos_y_vqwk_raw   = scanner[++index];
                      pos_x_vqwk_raw   = scanner[++index];

                      if (false)
                        {
                          std::cout << "entryNumber: " << entryNumber << std::endl;
                          std::cout<<"CodaEventNumber: "<<CodaEventNumber<<std::endl;
                          std::cout<<"PositionX_VQWK="<<PositionX_VQWK<<"\n";
                          std::cout<<"PositionY_VQWK="<<PositionY_VQWK<<"\n";
                          std::cout<<"PositionX_QDC="<<PositionX_QDC<<"\n";
                          std::cout<<"PositionY_QDC="<<PositionY_QDC<<"\n";
                          std::cout<<"CoincidenceSCA="<<CoincidenceSCA<<"\n";
                          std::cout<<"FrontSCA="<<FrontSCA<<"\n";
                          std::cout<<"BackSCA="<<BackSCA<<"\n";
                        }

                      if (RunMode == CURRENT_MODE)
                        {
                          RateMap->Fill(PositionX_VQWK,PositionY_VQWK,CoincidenceSCA,1);
                          RateMap_FrontQuartz->Fill(PositionX_VQWK,PositionY_VQWK,FrontSCA,1);
                          RateMap_BackQuartz->Fill(PositionX_VQWK,PositionY_VQWK,BackSCA,1);
                        }
                      else if (RunMode == EVENT_MODE)
                        {
                          RateMap->Fill(PositionX_QDC,PositionY_QDC,CoincidenceSCA,1);
                          RateMap_FrontQuartz->Fill(PositionX_QDC,PositionY_QDC,FrontSCA,1);
                          RateMap_BackQuartz->Fill(PositionX_QDC,PositionY_QDC,BackSCA,1);
                        }
                      else
                        {
                          printf("Warning: Unknown run mode\n");
                        }
                    }
                  RateMapProjectionXY = RateMap->ProjectionXY("RateMapProjectionXY");
                  RateMapProjectionX = RateMapProjectionXY->ProjectionX("RateMapProjectionX");
                  RateMapProjectionY = RateMapProjectionXY->ProjectionY("RateMapProjectionY");
                  PlotRateMap();
                }
            }

          if (!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont)
            {
              ClearDBData();
            }
        }
    }
}
void QwGUIScanner::OnRemoveThisTab()
{

}

void QwGUIScanner::OnUpdatePlot(char *obj)
{
  if (!obj) return;
  TString str = obj;
  if (!str.Contains("dDataWind")) return;

  printf("Received Messager From: %s\n",obj);
}

void QwGUIScanner::ClearDBData()
{

}

void QwGUIScanner::ClearRootData()
{

  TObject *obj;
  TIter nexth(HistArray.MakeIterator());
  obj = nexth();
  while (obj)
    {
      delete obj;
      obj = nexth();
    }
  TIter nextg(GraphArray.MakeIterator());
  obj = nextg();
  while (obj)
    {
      delete obj;
      obj = nextg();
    }

  HistArray.Clear();
  GraphArray.Clear();

  for (int i = 0; i < SCANNER_INDEX; i++)
    {
      dCurrentData[i].clear();
      dCurrentData[i].resize(0);
    }
}

void QwGUIScanner::PlotHistograms()
{
  printf("Ploting histo...");
  TCanvas *mc = dCanvas->GetCanvas();

  if (RunMode == EVENT_MODE)
    {
    }
  if (RunMode == CURRENT_MODE)
    {
    }
  else
    {
      printf("Warning: Unknown run mode.\n");
    }

  mc->Modified();
  mc->Update();
  SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
}

void QwGUIScanner::PlotGraphs()
{
  Int_t ind = 1;

  TCanvas *mc = dCanvas->GetCanvas();
  TObject *obj;
  TIter next(HistArray.MakeIterator());
  obj = next();

  while (obj)
    {
      mc->cd(ind);
      gPad->SetLogy(0);
      ((TH2*)obj)->Draw("");
      ind++;
      obj = next();
    }

  mc->Modified();
  mc->Update();

  SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
}



void QwGUIScanner::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
{
  if (event == kButton1Double)
    {
      Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();

      if (pad > 0 && pad <= SCANNER_INDEX)
        {
          if (GetPlotDataType() == SCANNER_PLOT_TYPE_HISTO)
            {

              QwGUIDataWindow *dDataWindow = new QwGUIDataWindow(GetParent(), this,Form("dDataWindow_%02d",GetNewWindowCount()),
                  "QwGUIScanner",HistArray[pad-1]->GetTitle(), PT_HISTO_3D,
                  DDT_MD,600,400);
              if (!dDataWindow)
                {
                  return;
                }
              DataWindowArray.Add(dDataWindow);
              dDataWindow->SetStaticData(HistArray[pad-1],DataWindowArray.GetLast());

              Connect(dDataWindow,"IsClosing(char*)","QwGUIScanner",(void*)this,"OnObjClose(char*)");
              Connect(dDataWindow,"SendMessageSignal(char*)","QwGUIScanner",(void*)this,"OnReceiveMessage(char*)");
              Connect(dDataWindow,"UpdatePlot(char*)","QwGUIScanner",(void*)this,"OnUpdatePlot(char *)");

              dDataWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
              dDataWindow->DrawData(*((TH1D*)HistArray[pad-1]));
              SetLogMessage(Form("Looking at histogram %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);

              return;
            }
          else if (GetPlotDataType() == SCANNER_PLOT_TYPE_GRAPH)
            {

              RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
                  GetNewWindowName(),"QwGUIScanner",
                  GraphArray[pad-1]->GetTitle(), PT_GRAPH,600,400);
              if (!dMiscWindow)
                {
                  return;
                }
              DataWindowArray.Add(dMiscWindow);
              dMiscWindow->SetPlotTitle((char*)GraphArray[pad-1]->GetTitle());
              dMiscWindow->SetPlotTitleX("Position X [cm]");
              dMiscWindow->SetPlotTitleY("Position Y [cm]");
              dMiscWindow->SetPlotTitleOffset(1.25,1.8);
              dMiscWindow->SetAxisMin(((TGraph*)GraphArray[pad-1])->GetXaxis()->GetXmin(),dCurrentDataMin[pad-1]);
              dMiscWindow->SetAxisMax(((TGraph*)GraphArray[pad-1])->GetXaxis()->GetXmax(),dCurrentDataMax[pad-1]);
              dMiscWindow->SetLimitsFlag(kTrue);

              dMiscWindow->DrawData(*((TGraph*)GraphArray[pad-1]));
              SetLogMessage(Form("Looking at graph %s\n",(char*)GraphArray[pad-1]->GetTitle()),kTrue);
            }

        else if (GetPlotDataType() == SCANNER_PLOT_TYPE_PROFILE)
            {

              RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
                  GetNewWindowName(),"QwGUIScanner",ProfileArray[pad-1]->GetTitle(),
                  PT_PROFILE, 800,200);

              if (!dMiscWindow)
                {
                  return;
                }

              DataWindowArray.Add(dMiscWindow);
              dMiscWindow->SetPlotTitle((char*)ProfileArray[pad-1]->GetTitle());
              dMiscWindow->SetPlotTitleX("Position X [cm]");
              dMiscWindow->SetPlotTitleY("Position Y [cm]");
              dMiscWindow->SetPlotTitleOffset(1.25,1.8);
              dMiscWindow->SetAxisMin(((TProfile2D*)ProfileArray[pad-1])->GetXaxis()->GetXmin(),dCurrentDataMin[pad-1]);
              dMiscWindow->SetAxisMax(((TProfile2D*)ProfileArray[pad-1])->GetXaxis()->GetXmax(),dCurrentDataMax[pad-1]);
              dMiscWindow->SetLimitsFlag(kTrue);

//              dMiscWindow->DrawData(*((TProfile2D*)ProfileArray[pad-1]));
              if (pad == 1)
                  RateMap->Draw("COLZ");
              if (pad ==2 )
                  RateMap->Draw("LEGO");
              SetLogMessage(Form("Looking at profile %s\n",(char*)ProfileArray[pad-1]->GetTitle()),kTrue);
            }
        }
    }
}

Bool_t QwGUIScanner::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg))
    {

    case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg))
        {
        case kTE_ENTER:
          switch (parm1)
            {

            default:
              break;
            }

        default:
          break;
        }

    case kC_COMMAND:
      switch (GET_SUBMSG(msg))
        {

        case kCM_COMBOBOX:
        {
          switch (parm1)
            {
            case M_TBIN_SELECT:
              break;
            }
        }
        break;

        case kCM_MENUSELECT:
          break;

        case kCM_MENU:

          switch (parm1)
            {

            case SCANNER_PLOT_RATEMAP:
              PlotRateMap();
              break;

            case SCANNER_PLOT_PROJECTION:
              PlotProjections();
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


void QwGUIScanner::PlotRateMap()
{
  printf("QwGUIScanner::PlotRateMap()\n");
  gStyle->SetPalette(1);
  TCanvas *mc = dCanvas->GetCanvas();

  mc->cd(1);
  gStyle->SetOptStat(0);
  RateMap->GetXaxis()->SetTitle("PositionX [cm]");
  RateMap->GetYaxis()->SetTitle("PositionY [cm]");
  RateMap->GetZaxis()->SetTitle("Rate");
  //RateMap->SetOption("colz");
  RateMap->Draw("colz");
  ProfileArray.Add(RateMap);

  mc->cd(2);
  gStyle->SetOptStat(1000011);
  RateMap->GetXaxis()->SetTitle("PositionX [cm]");
  RateMap->GetYaxis()->SetTitle("PositionY [cm]");
  RateMap->GetZaxis()->SetTitle("Rate");
  //RateMap->SetOption("lego");
  RateMap->Draw("lego");
  ProfileArray.Add(RateMap);

  mc->Modified();
  mc->Update();

  //SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
  SetPlotDataType(SCANNER_PLOT_TYPE_PROFILE);
}



void QwGUIScanner::PlotProjections()
{
  printf("QwGUIScanner::PlotRateMap()\n");
  TCanvas *mc = dCanvas->GetCanvas();

  mc->cd(1);
  gStyle->SetOptStat(1001111);
  RateMap->GetXaxis()->SetTitle("Position X [cm]");
  RateMap->GetYaxis()->SetTitle("Rate");
  RateMapProjectionX->Draw();
  HistArray.Add(RateMapProjectionX);

  mc->cd(2);
  gStyle->SetOptStat(1001111);
  RateMap->GetXaxis()->SetTitle("Position Y [cm]");
  RateMap->GetYaxis()->SetTitle("Rate");
  RateMapProjectionY->Draw();
  HistArray.Add(RateMapProjectionY);

  mc->Modified();
  mc->Update();

  SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
}
