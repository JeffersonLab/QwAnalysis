#include <QwGUIScanner.h>

ClassImp(QwGUIScanner);

const char *QwGUIScanner::ScannerDataNames[SCANNER_INDEX] =
  {"RateMap","RandomMap","RateMapProjectionXY","RateMapProjectionX","RateMapProjectionY"};

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
  dMenuPlot->AddEntry("&Projection XY", SCANNER_PLOT_PROJECTION);

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
  RunMode = 0;

  if (!cont) return;

  TObject *obj;
  TTree *tree;
  TH1D *hst;
  TGraph *grp;
  TProfile *prf;
  TProfile2D *prf2d;


  if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont)
    {

      obj = dROOTCont->ReadData("tree");
      if (obj)
        {
          printf("Reading data from 'tree'\n");
          RunMode = 2;//EVENT_MODE;
        }
      else
        {
          obj = dROOTCont->ReadData("Mps_Tree");
          if (obj)
            {
              printf("Reading data from 'Mps_Tree'\n");
              RunMode = 1;//CURRENT_MODE;
            }
        }

        if (!obj)
          {
             printf("Can't find data tree.\n");
             return;
          }

          if (obj->InheritsFrom("TTree"))  
            {
              tree = (TTree*)obj->Clone();
              ClearRootData();

              RateMap  = new TProfile2D("RateMap","Scanner Rate Map",210,-105.0,105.0,40,-350.0,-310.0);
              RandomMap  = new TProfile2D("RandomMap","Scanner Random Rate Map",210,-105.0,105.0,40,-350.0,-310.0);

              //test 100 entries

              UInt_t NumberOfEntries = tree->GetEntries();
              if (NumberOfEntries<200)
                {
                  printf("No enough data to draw a rate map\n");
                  return;
                }

              Int_t i_vqwk=0, i_qdc=0;
              for (Int_t jentry=100; jentry<NumberOfEntries && jentry<200; jentry++)
               {
                 tree->GetEntry(jentry);
                 Double_t tmpt1 =  tree->FindLeaf("scanner.PositionX_VQWK")->GetValue(0);
                 Double_t tmpt2 =  tree->FindLeaf("scanner.PositionY_VQWK")->GetValue(0);
                 if (TMath::IsNaN(tmpt1) || TMath::IsNaN(tmpt2))
                     i_vqwk++;
                 Double_t tmpt3 =  tree->FindLeaf("scanner.PositionX_QDC")->GetValue(0);
                 Double_t tmpt4 =  tree->FindLeaf("scanner.PositionY_QDC")->GetValue(0);
                 if (TMath::IsNaN(tmpt3) || TMath::IsNaN(tmpt4))
                     i_qdc++;

                 printf("VQWK: %f, %f \t QDC: %f, %f\n",tmpt1, tmpt2, tmpt3, tmpt4);
               }

               printf("position data test: %d%% VQWK data is nan, %d%% QDC data is nan\n",i_vqwk, i_qdc);

              if (i_vqwk==0)
                {
                   PositionDataType = PVQWK;
                   printf("==>> VQWK position data available.\n");
                }
              else if (i_qdc==0)
                {
                   PositionDataType = PQDC;
                   printf("==>> QDC position data available.\n");
                }
              else
                {
                   printf("==>> No position data available.\n");
                   return;
                }

               // loop over all entries

/*
              for (Int_t jentry=0; jentry<NumberOfEntries; jentry++)
                {
                  Int_t entryNumber = tree->GetEntryNumber(jentry);
                  if (entryNumber%10000==0)
                        std::cout << "entryNumber: " << entryNumber << std::endl;

                  tree->GetEntry(entryNumber);

                  Double_t PositionX, PositionY;
                  Double_t CoincidenceSCA = tree->FindLeaf("scanner.CoincidenceSCA")->GetValue(0);

                  if(CoincidenceSCA>0)
                    {
                      if (PositionDataType == PVQWK)
                        {
                           PositionX = tree->FindLeaf("scanner.PositionX_VQWK")->GetValue(0);
                           PositionY = tree->FindLeaf("scanner.PositionY_VQWK")->GetValue(0);
                        }
                      else if (PositionDataType == PQDC)
                        {
                           PositionX = tree->FindLeaf("scanner.PositionX_QDC")->GetValue(0);
                           PositionY = tree->FindLeaf("scanner.PositionY_QDC")->GetValue(0);
                        }
                      else if (RunMode == 0)
                        {
                           printf("Warning: Unknown position data type.\n");
                           continue;
                        }

                       Double_t RandomSCA = tree->FindLeaf("scanner.randm_sca_raw")->GetValue(0);

                       RateMap->Fill(PositionX*1.025-6, PositionY+5*2.54,CoincidenceSCA-RandomSCA,1);
                       RandomMap->Fill(PositionX*1.025-6,PositionY+5*2.54,RandomSCA,1);
                    }
                 }

*/
                 TCanvas *mc = dCanvas->GetCanvas();

                 mc->cd(1);
                 // add 5*2.54 cm 
                 if (PositionDataType == PVQWK)
                   {
                     tree->Draw("(scanner.CoincidenceSCA-scanner.randm_sca_raw):(scanner.PositionY_VQWK+5*2.54):(scanner.PositionX_VQWK)>>RateMap","scanner.CoincidenceSCA>0","prof");
                   }
                 else
                   {
                     tree->Draw("(scanner.CoincidenceSCA-scanner.randm_sca_raw):(scanner.PositionY_QDC+5*2.54):(scanner.PositionX_QDC*1.025-6)>>RateMap","scanner.CoincidenceSCA>0","prof");
                   }
                 RateMap->SetStats(0);

                 mc->cd(2);
                 if (PositionDataType == PVQWK)
                   {
                      tree->Draw("(scanner.randm_sca_raw):(scanner.PositionY_VQWK+5*2.54):(scanner.PositionX_VQWK)>>RandomMap","scanner.CoincidenceSCA>0","prof");
                   }
                 else
                   {
                      tree->Draw("(scanner.randm_sca_raw):(scanner.PositionY_QDC+5*2.54):(scanner.PositionX_QDC*1.025-6)>>RandomMap","scanner.CoincidenceSCA>0","prof");
                   }
                 RandomMap->SetStats(0);

                 RateMapProjectionXY = RateMap->ProjectionXY("RateMapProjectionXY");
                 RateMapProjectionX  = RateMapProjectionXY->ProjectionX("RateMapProjectionX");
                 RateMapProjectionY  = RateMapProjectionXY->ProjectionY("RateMapProjectionY");

                 PlotRateMap();

             }  // END OF: if (obj->InheritsFrom("TTree"))
        } // END OF: if (!strcmp(cont->GetDataName(),"ROOT") && dROOTCont)

     if (!strcmp(cont->GetDataName(),"DBASE") && dDatabaseCont)
        {
          ClearDBData();
        }

} // END OF:  void QwGUIScanner::OnNewDataContainer(RDataContainer *cont)

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
                  RandomMap->Draw("COLZ");
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
  //gStyle->SetOptStat(1000011);
  gStyle->SetOptStat(0);
  RandomMap->GetXaxis()->SetTitle("PositionX [cm]");
  RandomMap->GetYaxis()->SetTitle("PositionY [cm]");
  RandomMap->GetZaxis()->SetTitle("Rate");
  //RandomMap->SetOption("lego");
  RandomMap->Draw("colz");
  ProfileArray.Add(RandomMap);

  mc->Modified();
  mc->Update();

  //SetPlotDataType(SCANNER_PLOT_TYPE_HISTO);
  SetPlotDataType(SCANNER_PLOT_TYPE_PROFILE);
}


void QwGUIScanner::PlotProjections()
{
  printf("QwGUIScanner::PlotProjections()\n");
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

