///////////////////////////////////////////////////////////////////
//  Macro to help with online analysis
//    B. Moffit  Oct. 2003

#include "ParseConfigFile.h"
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

class DrawSlug {
	// Class that takes care of the GUI
	RQ_OBJECT("DrawSlug")
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
	ParseConfigFile                     *fConfig;
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
	Int_t                             histnumber;

public:
	DrawSlug(ParseConfigFile&,Bool_t);
	void CreateGUI(const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~DrawSlug();
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
//	void ObtainRunNumber();
	void PrintToFile();
	void PrintPages();
	void MyCloseWindow();
	void CloseGUI();
};


/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
