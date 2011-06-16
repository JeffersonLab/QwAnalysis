///////////////////////////////////////////////////////////////
// TaConfFile.C
//    - Configuration File Class for ReD analysis
//
//#define DEBUG
#define NOISY
#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "TaConfFile.hh"
#include "TaString.hh"
#include "TaFileName.hh"

ClassImp(TaConfFile);

TaConfFile::TaConfFile() :
  fConfFile(0),
    //  fFileName(0),
  fCksum(0),
  analysis(0),
  regtype(0),
  regmatrixchoice(0),
  treetype(0),
  minirunevt(0),
  usecuts(kFALSE),
  cycles(0),
  rootanatype(0),
  commenttag(0),
  fUseCurMon(0)
{
  //  customcut = make_pair(NULL,NULL);
}

TaConfFile::~TaConfFile() {}

void TaConfFile::Read() {
#ifdef DEBUG
  cout << "TaConfFile::Read()" << endl;
#endif

  // Load the Configuration File for this Analysis

//   TString conffile = "postpan.conf";  // Use default for now
//   ifstream *fConfFile = new ifstream(conffile,ios::in);
//   if( ! (*fConfFile) ) {
//     cerr << "TaConfFile::Read ERROR: no file "
// 	 << conffile << endl;
//     return;
//   }

  TaFileName confFileName ("conf");
  fConfFile = new ifstream(confFileName.String().c_str());
  if ( ! (*fConfFile) ) {
    cerr << "TaConfFile::Load WARNING: run file " << confFileName.String()
	 << " does not exist" << endl;
    confFileName = TaFileName ("confdef");
    fConfFile = new ifstream(confFileName.String().c_str());
    if ( ! (*fConfFile) ) { // try one last time : control.conf from $pwd
      fConfFile = new ifstream("./control.conf");
    } 
    if ( ! (*fConfFile) ) {
      cerr << "TaConfFile::Load WARNING: no file "
	   << confFileName.String() <<endl;
      cerr << "You need a configuration to run.  Ask an expert."<<endl;
      return;
    }
    cerr << "TaConfFile::Load: Using " << confFileName.String() << " as default configuration. (May be ok.)"<<endl;
  }

  clog << "TaConfFile::Load: Database loading from " 
       << confFileName.String() << endl;
  fFileName = confFileName.String();
  Load();

}

void TaConfFile::Read(TString chooseName) {

  TaFileName confFileName;
  fConfFile = new ifstream(chooseName);
  if ( ! (*fConfFile) ) {
    cerr << "TaConfFile::Load WARNING: run file " << chooseName
	 << " does not exist" << endl;
    confFileName = TaFileName ("confdef");
    fConfFile = new ifstream(confFileName.String().c_str());
    chooseName = confFileName.String();
    if ( ! (*fConfFile) ) { // try one last time : control.conf from $pwd
      fConfFile = new ifstream("./control.conf");
      chooseName = confFileName.String();
    } 
    if ( ! (*fConfFile) ) {
      cerr << "TaConfFile::Load WARNING: no file "
	   << confFileName.String() <<endl;
      cerr << "You need a configuration to run.  Ask an expert."<<endl;
      return;
    }
    cerr << "TaConfFile::Load: Using " << confFileName.String() << " as default configuration. (May be ok.)"<<endl;
  }

  fFileName = chooseName;
  clog << "TaConfFile::Load: Database loading from " 
       << fFileName << endl;
  Load();
  

}

void TaConfFile::Load() {
#ifdef DEBUG
  cout << "TaConfFile::Read()" << endl;
#endif
  
  // Below is some code "borrowed" from TaDatabase::Read()
  string comment = "#";
  vector<string> strvect;
  TaString sinput,sline;
  while (getline(*fConfFile,sinput)) {
    strvect.clear();   strvect = sinput.Split();
    sline = "";
    for (vector<string>::iterator str = strvect.begin();
	 str != strvect.end(); str++) {
      if ( *str == comment ) break;
      sline += *str;   sline += " ";
    }
    if (sline == "") continue;
    strvect = sline.Split();
    //Parse here.
    ParseLine(strvect);
  }

  
  fConfFile->close();
  delete fConfFile;

#ifdef NOISY
  Print();
#endif

  LoadCksum(fFileName);
  
}

void TaConfFile::ParseLine(vector <string> line) {
  // Parse the input line for redana arguments

  if (line[0] == "anatype") {
    analysis = (TString)line[1].c_str();
    if(analysis == "regress") {
      regtype = line[2].c_str();
      if(regtype == "matrix") {
	if(line.size()==3) {
	  regmatrixchoice = "all";
	} else {
	  regmatrixchoice = line[3].c_str();
	}
      }
    }
    if(analysis == "dither")  cout << "found dither anatype" << endl;
  }
  if (line[0] == "minirun") {
    minirunevt = atoi(line[1].c_str());
    if(line.size()==3) {
      if(line[2] == "usecuts") {
	usecuts=kTRUE;
      } else if(line[2]=="usepairs") {
	usecuts=kFALSE;
      }
    } else {
      usecuts=kTRUE;  // Use cuts by default.
    }
  }
  if (line[0] == "rootanatype") {
    rootanatype = line[1].c_str();
  }
  if (line[0] == "treetype") {
    treetype = line[1].c_str();
  }
  if (line[0] == "commenttag") {
    commenttag = line[1].c_str();
  }
  if (line[0] == "regresscycles") {
    cycles = atoi(line[1].c_str());
  }
  if (line[0] == "iv") {
    iv.push_back(line[1].c_str());
  }
  if (line[0] == "dv") { 
    dv.push_back(line[1].c_str());
  }
  if (line[0] == "mon") {
    monList.push_back(line[1].c_str());
  }
  if (line[0] == "det") {
    detList.push_back(line[1].c_str());
  }
  if (line[0] == "dit") {
    ditList.push_back(line[1].c_str());
  }
  if (line[0] == "pancut") {
    pancuts.push_back(line[1].c_str());
  }
  if (line[0] == "rawcut") {
    rawcuts.push_back(line[1].c_str());
  }
  if (line[0] == "paircut") {
    paircuts.push_back(line[1].c_str());
  }
  if (line[0] == "curmon") {
    fUseCurMon=kTRUE;
    curmon = line[1].c_str();
  }
  if (line[0] == "customcut") {
    if(line.size()>2) {
      TString cutTree = line[1];
      TString custCut;
      for(UInt_t i=2; i<line.size(); i++) {
	custCut += line[i];
      }
      customcut = make_pair(cutTree,custCut);
    } else {
      cout << "Not enough arguments in customcut." << endl;
    }
  }
  if (line[0] == "coillimit") {
    TString dl = line[1].c_str();
    Double_t lim = (Double_t) atoi(line[2].c_str());
    ditLimList.push_back(make_pair(dl,lim));
//     cout << Form("Testing: coil limit is %f", lim) << endl;
//     UInt_t ind = ditLimList.size() -1;
//     cout << Form("Testing: coil limit is %f", ditLimList[ind].second) << endl;
  }

}

void
TaConfFile::LoadCksum (const string filename)
{
  string tfilename = filename;
  
  for (string::iterator i = tfilename.begin(); i != tfilename.end(); ++i)
    if (*i == '/')
      *i = '_';

  tfilename = string ("/tmp/db_cksum_") + tfilename;
  string ckscmd = string ("cksum ") + filename + " > " + tfilename;
  system (ckscmd.c_str());

  ifstream ifile (tfilename.c_str());
  if (ifile != 0)
    ifile >> fCksum;
}


void 
TaConfFile::Print() {
  // Print to STDOUT the stuff from the config file

  cout << endl;

  cout << "TaConfFile::Print" << endl;

  cout << "\tAnalysis Type: ";
  if(GetAnaType() == "regress") {
    cout << GetRegressType() << " " << GetAnaType() << endl;

    cout << "\tRoot File Analysis Type: " << rootanatype.Data() << endl;
    if(GetRegressType() == "linear") {
      cout << "\tNumber of linear regression cycles: " << GetRegressCycles()
	   << endl;
    } else if (GetRegressType() == "matrix") {
      if (GetMatrixChoice() == "calccoeff") {
	cout << "\tWill only calculate coefficients (no root output)." << endl;
      } else if (GetMatrixChoice() == "useres") {
	cout << "\tWill only regress quantites (requires results file)." 
	     << endl;
      } else if (GetMatrixChoice() == "all") {
	cout << "\tWill calculate coefficients and regress quantities."
	     << endl;
      }
      if(GetRegressMinirun() == -1) {
	cout << "\tWill use BMWcycle to define a minirun" << endl;
      } else {
	cout << "\tNumber of events/minirun: " << GetRegressMinirun();
	  if(usecuts) {
	    cout << "  Defined by cuts.";
	  }
	cout << endl;
      }
    }
    
    cout << "\tDependent Variables: " << endl;
    for (Int_t i=0; i<GetDVNum(); i++) {
      cout << "\t\t" << GetDV(i) << endl;
    }
    cout << "\tIndependent Variables: " << endl;
    for (Int_t i=0; i<GetIVNum(); i++) {
      cout << "\t\t" << GetIV(i) << endl;
    }
    
  } else {
    cout << GetAnaType() << endl;
    cout << "\tRoot File Analysis Type: " << rootanatype.Data() << endl;

    cout << "\tDithering Objects: " << endl;
    for (Int_t i=0; i<GetDitNum(); i++) {
      cout << "\t\t" << GetDit(i) << endl;
    }

    cout << "\tBeam Monitors: " << endl;
    for (Int_t i=0; i<GetMonNum(); i++) {
      cout << "\t\t" << GetMon(i) << endl;
    }

    cout << "\tDetectors: " << endl;
    for (Int_t i=0; i<GetDetNum(); i++) {
      cout << "\t\t" << GetDet(i) << endl;
    }

    cout << "\tCurrent Monitor: " << GetCurMon() << endl;

  }

  cout << "\t Tree Type: " << treetype << endl;

  for (UInt_t i=0; i<pancuts.size(); i++) {
    cout << "\tPAN (regression pair) Cut: " << pancuts[i] << endl;
  }
  for (UInt_t i=0; i<rawcuts.size(); i++) {
    cout << "\tRawTree (dithering) Cut: " << rawcuts[i] << endl;
  }
  for (UInt_t i=0; i<paircuts.size(); i++) {
    cout << "\tPairTree (dithering) Cut: " << paircuts[i] << endl;
  }

  if(!customcut.first.IsNull()) {
    cout << "\tCustom cut from the " << customcut.first << " Tree:" << endl
	 << "\t\t" << customcut.second << endl;
  }

  vector <pair <TString, Double_t> > tmplist = GetDitLimList();
  for (Int_t i=0; i<GetDitLimNum(); i++ ) 
    cout << "\tLimit for coil " << tmplist[i].first
	 << Form(" = %.1f",tmplist[i].second) << endl;

  cout << endl;

}
