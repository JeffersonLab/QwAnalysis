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

class ParseConfigFile {
	RQ_OBJECT("ParseConfigFile");
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
	ParseConfigFile();
	ParseConfigFile(TString);
	Bool_t ParseConfig();
	TString GetRootFile() { return rootfilename; };
	void    SetRootFile(TString newrootfilename) { rootfilename = newrootfilename; };
	TString GetGoldenFile() { return goldenrootfilename; };
	TString GetGuiColor() { return guicolor; };
	TString GetPlotsDir() { return plotsdir; };
	void    SetPlotsDir(TString newplotsdir) { plotsdir = newplotsdir; };
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
	//  void OverrideRootFile(UInt_t);
	Bool_t IsMonitor() { return fMonitor; };
};



/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
