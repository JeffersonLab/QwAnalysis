///////////////////////////////////////////////////////////////////
//  Macro to help with online analysis
//    B. Moffit  Oct. 2003

#include <TTree.h>
#include <TFile.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include "TGLabel.h"
#include "TGString.h"
#include <RQ_OBJECT.h>
#include <TQObject.h>
#include <vector>
#include <TString.h>
#include <TCut.h>
#include <TTimer.h>
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#define UPDATETIME 2000

using namespace std;

/* TTree *raw; */
/* TFile *file; */

class OnlineConfig {
  RQ_OBJECT("OnlineConfig");
  // Class that takes care of the config file
private:
  TString confFileName;                   // config filename
  ifstream *fConfFile;                    // original config file
  void ParseFile();
  vector < vector <TString> > sConfFile;  // the config file, in memory
  // pageInfo is the vector of the pages containing the sConfFile index
  //   and how many commands issued within that page (title, 1d, etc.)
  TString rootfilename;  //  Just the name
  TString goldenrootfilename; // Golden rootfile for comparisons
  TString protorootfile; // Prototype for getting the rootfilename
  TString guicolor; // User's choice of background color
  TString plotsdir; // Where to store sample plots.. automatically stored as .jpg's).
  vector < pair <UInt_t,UInt_t> > pageInfo; 
  vector <TCut> cutList; 
  vector <UInt_t> GetDrawIndex(UInt_t);
  Bool_t fFoundCfg;
  Bool_t fMonitor;
  

public:
  OnlineConfig();
  OnlineConfig(TString);
  Bool_t ParseConfig();
  TString GetRootFile() { return rootfilename; };
  TString GetGoldenFile() { return goldenrootfilename; };
  TString GetGuiColor() { return guicolor; };
  TString GetPlotsDir() { return plotsdir; };
  TCut GetDefinedCut(TString ident);
  vector <TString> GetCutIdent();
  // Page utilites
  UInt_t  GetPageCount() { return pageInfo.size(); };
  pair <UInt_t,UInt_t> GetPageDim(UInt_t);
  Bool_t IsLogy(UInt_t page);
  TString GetPageTitle(UInt_t);
  UInt_t GetDrawCount(UInt_t);           // Number of histograms in a page
  vector <TString> GetDrawCommand(UInt_t,UInt_t);
  vector <TString> SplitString(TString,TString);
  void OverrideRootFile(UInt_t);
  Bool_t IsMonitor() { return fMonitor; };
};

class OnlineGUI {
  // Class that takes care of the GUI
  RQ_OBJECT("OnlineGUI")
private:
  TGMainFrame                      *fMain;
  TGHorizontalFrame                *fTopframe;
  TGVerticalFrame                  *vframe;
  TGRadioButton                    *fRadioPage[50];
  TGPictureButton                  *wile;
  TRootEmbeddedCanvas              *fEcanvas;
  TGHorizontalFrame                *fBottomFrame;
  TGHorizontalFrame                *hframe;
  TGTextButton                     *fNext;
  TGTextButton                     *fPrev;
  TGTextButton                     *fExit;
  TGLabel                          *fRunNumber;
  TGTextButton                     *fPrint;
  TCanvas                          *fCanvas; // Present Embedded canvas
  OnlineConfig                     *fConfig;
  UInt_t                            current_page;
  TFile*                            fRootFile;
  TFile*                            fGoldenFile;
  Bool_t                            doGolden;
  vector <TTree*>                   fRootTree;
  vector <Int_t>                    fTreeEntries;
  vector < pair <TString,TString> > fileObjects;
  vector < vector <TString> >       treeVars;
  UInt_t                            runNumber;
  TTimer                           *timer;
  Bool_t                            fUpdate;
  Bool_t                            fFileAlive;
  Bool_t                            fPrintOnly;
  TH1D                             *mytemp1d;
  TH2D                             *mytemp2d;
  TH3D                             *mytemp3d;
  TH1D                             *mytemp1d_golden;
  TH2D                             *mytemp2d_golden;
  TH3D                             *mytemp3d_golden;

public:
  OnlineGUI(OnlineConfig&,Bool_t);
  void CreateGUI(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~OnlineGUI();
  void DoDraw();
  void DrawPrev();
  void DrawNext();
  void DoRadio();
  void CheckPageButtons();
  // Specific Draw Methods
  Bool_t IsHistogram(TString);
  void GetFileObjects();
  void GetTreeVars();
  void GetRootTree();
  UInt_t GetTreeIndex(TString);
  UInt_t GetTreeIndexFromName(TString);
  void TreeDraw(vector <TString>); 
  void HistDraw(vector <TString>);
  void MacroDraw(vector <TString>);
  void DoDrawClear();
  void TimerUpdate();
  void BadDraw(TString);
  void CheckRootFile();
  Int_t OpenRootFile();
  void ObtainRunNumber();
  void PrintToFile();
  void PrintPages();
  void MyCloseWindow();
  void CloseGUI();
};
