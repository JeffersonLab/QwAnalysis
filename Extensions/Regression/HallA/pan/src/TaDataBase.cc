//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       VaAnalysis.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaDataBase.cc,v 1.48 2010/04/19 02:08:20 silwal Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//  Database class.  This always reads database from an ASCII file.
//  For MYSQL access, it first executes a Perl script which generates
//  the ASCII file.
//
//  The database is organized in tables, see list below.
//  The tables are denoted by a string table name (e.g. 'dacnoise').  
//  In each table is a series of columns of information.  The columns 
//  are 'typed' data, i.e. data of a type string, int, or double.
//
//  Tables include:
//
//      1. run  (the CODA run number)
//      2. analysis type
//      3. maxevents (the number of events to analyze)
//      4. pair type ('pair' or 'quad' or 'octet')
//      5. window delay
//      6. oversampling factor
//      7. dac noise parameters
//      8. pedestals 
//      9. datamap and header info
//     10. named cuts ('lobeam', 'burpcut', etc, each a table)
//     11. event intervals where data are cut.
//      
//  For usage instructions, syntax rules, and other details, see
//          /doc/DATABASE.TXT
//
/////////////////////////////////////////////////////////////////////

// Turn off MYSQL.  Hopefully it will be turned on ... soon.
#define TURN_OFF_MYSQL

#include "TaDataBase.hh"
#include "TaFileName.hh"
#include "TaString.hh"
#include "TDatime.h"

#ifndef NODICT
ClassImp(TaDataBase)
ClassImp(TaRootRep)
#endif

TaDataBase::TaDataBase() {
     didinit   = kFALSE; 
     didread   = kFALSE; 
     didput    = kFALSE;
     initdm    = kFALSE;
     usemysql  = kFALSE;
     useroot   = kFALSE;
     usectrl   = kFALSE;
     firstiter = kTRUE;
     fFirstgdn = new Bool_t(kTRUE);
     fFirstAdcPed = new Bool_t(kTRUE);
     fFirstAdcxPed = new Bool_t(kTRUE);
     fFirstVqwkPed = new Bool_t(kTRUE);
     fFirstScalPed = new Bool_t(kTRUE);
     nbadev = 0;
     rootdb = new TaRootRep();
     AnaTDatime = new TDatime();
     fileRead = "None";
     dacparam = new Double_t[MAXADC*MAXCHAN];     
     memset(dacparam,0,MAXADC*MAXCHAN*sizeof(Double_t));
     dacxparam = new Double_t[MAXADCX*MAXCHAN];     
     memset(dacxparam,0,MAXADCX*MAXCHAN*sizeof(Double_t));
     adcped = new Double_t[MAXADC*MAXCHAN];
     memset(adcped,0,MAXADC*MAXCHAN*sizeof(Double_t));
     adcxped = new Double_t[MAXADCX*MAXCHAN];
     memset(adcxped,0,MAXADCX*MAXCHAN*sizeof(Double_t));
     vqwkped = new Double_t[MAXVQWK*MAXVQWKCHAN];
     memset(vqwkped,0,MAXVQWK*MAXVQWKCHAN*sizeof(Double_t));
     scalped = new Double_t[MAXSCAL*MAXSCALCHAN];
     memset(scalped,0,MAXSCAL*MAXSCALCHAN*sizeof(Double_t));
 }

TaDataBase::~TaDataBase() {
     delete [] dacparam;
     delete [] dacxparam;
     delete [] adcped;
     delete [] adcxped;
     delete [] vqwkped;
     delete [] scalped;
     delete AnaTDatime;
     delete rootdb;
     delete fFirstgdn;
     delete fFirstAdcPed;
     delete fFirstAdcxPed;
     delete fFirstVqwkPed;
     delete fFirstScalPed;
     tables.clear();
     for (multimap<string, vector<dtype*> >::iterator im = database.begin();
	  im != database.end();  im++) {
       vector<dtype *> ddata = im->second;
       for (vector<dtype* >::iterator id = ddata.begin();  
	    id != ddata.end(); id++) {
          	 delete *id;
       }
     }
     database.clear();
 }

void TaDataBase::Read(int run, const vector<string>& dbcomm) {
// Load the database for this run which is for example
//     ./db/parity03_1441.db for run = 1441
// The flag 'usectrl' can be set via the '-D control.db' command
// line option; this forces us to use 'control.db' as the database.
// See also the comments in ChkDbCommand()
  InitDB();
  runnum = run;
  dbcommand = dbcomm;
  if (ChkDbCommand() == 0) return;  // ERROR
  if (useroot) return;  // All done, db was read from ROOT.
  rootdb->Clear();
  TaFileName dbFileName ("db");
  fileRead = dbFileName.String();
  ifstream *dbfile = new ifstream(dbFileName.String().c_str());
  if ( ! (*dbfile) || usectrl ) {
    if ( !usectrl ) {
      cerr << "TaDataBase::Load WARNING: run file " << dbFileName.String()
	   << " does not exist" << endl;
    }
    else
      delete dbfile;
    dbFileName = TaFileName ("dbdef");
    fileRead = dbFileName.String();
    dbfile = new ifstream(dbFileName.String().c_str());
    if ( ! (*dbfile) ) { // try one last time : control.db from $pwd
      dbfile = new ifstream("./control.db");
      fileRead = "./control.db";
    } 
    if ( ! (*dbfile) ) {
      cerr << "TaDataBase::Load WARNING: no file "
	   << dbFileName.String() <<endl;
      cerr << "You need a database to run.  Ask an expert."<<endl;
      return;
    }
    cerr << "TaDataBase::Load: Using " << dbFileName.String() << " as default database. (May be ok.)"<<endl;
  }

  clog << "TaDataBase::Load: Database loading from " 
       << dbFileName.String() << endl;
    
  string comment = "#";
  vector<string> strvect;
  TaString sinput,sline;
  while (getline(*dbfile,sinput)) {
    strvect.clear();   strvect = sinput.Split();
    sline = "";
    for (vector<string>::iterator str = strvect.begin();
      str != strvect.end(); str++) {
      if ( *str == comment ) break;
      sline += *str;   sline += " ";
    }
    if (sline == "") continue;
    strvect = sline.Split();
    multimap<string, vector<dtype*> >::iterator dmap = 
          database.find(FindTable(strvect[0]));
    if (dmap == database.end()) continue; 
    vector<dtype*> dv = dmap->second;
    vector<dtype*> datavect;  datavect.clear();
    for (int i = 1; i < (long)strvect.size(); i++) {
      if (i-1 < (long)dv.size()) {
        dtype *dat = new dtype(dv[i-1]->GetType()); 
        dat->Load(strvect[i]);   
        datavect.push_back(dat);
      }
    }
    LoadTable(FindTable(strvect[0]), datavect);
  }
  didread = kTRUE;
  delete dbfile;

  LoadCksum (dbFileName.String());

// Command-line over-ride
  SetDbCommand();
  ToRoot();
};

Int_t TaDataBase::ChkDbCommand() {
// Command line over-ride of database
// Logic does not allow infinite combinations, NOTE:
//  *  If  '-D control.db' we read from control.db.  Other 
//     '-D' command subsequently over-ride table(s).
//  *  If  '-D useroot filename' we read database from ROOT
//     file 'filename' and ignore all other commands.
//  *  If  '-D mysql' we read database from MYSQL. Other '-D'
//     commands subsequently over-ride table(s).
//  *  It is an error to specify both mysql and useroot. If
//     you try that, you'll get no data at all (ERROR).
//  *  '-D table values'  will over-write the table with those
//     values, provided the line has proper syntax.  NOTE:  The 
//     table is dropped; therefore, you cannot add to a table,
//     instead you must add all lines of a table when you start one.
//     Normally this is ok for trivial tables like 'lobeam'.
  usemysql = kFALSE;
  useroot = kFALSE;
  TaString rootfile;
  if (dbcommand.size() == 0) return 1;
  for (int i = 0; i < (long)dbcommand.size(); i++) {
    if (strcasecmp(dbcommand[i].c_str(),"mysql") == 0) 
          usemysql = kTRUE;
    if (strcasecmp(dbcommand[i].c_str(),"control.db") == 0) 
          usectrl = kTRUE;
    if (strcasecmp(dbcommand[i].c_str(),"useroot") == 0) {
      useroot = kTRUE;
      if (i+1<(long)dbcommand.size()) rootfile = dbcommand[i+1];
    }
  }
  if (usemysql && useroot) {
    cout << "ERROR: TaDataBase::SetDbCommand: Cannot use both"<<endl;
    cout << " MYSQL and ROOT file as database input. Choose ONE !!"<<endl;
    return 0;
  }
  if (usemysql) Mysql("read");
  if (useroot) {
    ReadRoot(rootfile);
    ToRoot();
  }
  return 1;
}  

void TaDataBase::SetDbCommand() {
// Use the '-D' database command to over-ride table(s) 
// of database.  See comments for ChkDbCommand()
  if (dbcommand.size() == 0) return;
  for (int i = 0; i < (long)dbcommand.size(); i++) {
    multimap<string, vector<dtype*> >::iterator dmap = 
         database.find(FindTable(dbcommand[i]));
    if (dmap == database.end()) continue; 
    vector<dtype*> dv = dmap->second;
    vector<dtype*> datavect;  datavect.clear();
    int j = i+1;  int k = 0;
    while (j < (long)dbcommand.size() && k < (long)dv.size()) {
       multimap<string, vector<dtype*> >::iterator dnext = 
          database.find(FindTable(dbcommand[j]));
       if (dnext != database.end()) { // found next table
	 cout << "TaDataBase::SetDbCommand: WARNING: Found the next table."<<endl;
         cout << "   This means that either:"<<endl;
         cout << "     1. You entered with -D  a line which was less"<<endl;
         cout << "        the expected length of data, OR"<<endl;
         cout << "     2. Your line contains two database keywords."<<endl;
         cout << "        You CANNOT have a database table name the SAME as"<<endl;
         cout << "        a string of data in that table."<<endl;
         cout << "        E.g.  'anatype feedback'"<<endl;
         cout << "        is wrong since 'feedback' is another table."<<endl;
         break; 
       }  
       dtype *dat = new dtype(dv[k]->GetType()); 
       dat->Load(dbcommand[j]);   
       datavect.push_back(dat);
       j++;  k++;
    }
    PutData(dbcommand[i], datavect);
  }
};

void 
TaDataBase::ReadRoot(TaString filename) {
// Load the database from a ROOT file 'filename'.
// This is done with '-D useroot filename' command line option.
// This choice, if chosen, takes precedence, i.e. all other source
// of data including command line is therefore ignored.
  didread = kFALSE;
  TFile *rfile = new TFile(filename.c_str());
  TaRootRep *rdb = (TaRootRep*)rfile->Get("TaRootRep;1");
  if ( !rdb ) {
    cout << "ERROR: TaDataBase::ReadRoot:";
    cout << " TaRootRep object not found !!"<<endl;
    cout << " Database was *NOT* read from ROOT file "<<filename<<endl;
    cout << " This means that either: "<<endl;
    cout << "   1. The root file you specified does not exist."<<endl;
    cout << "   2. The root file does not contain the TaRootRep object."<<endl;
    return;
  }
  cout << "Loading database from ROOT file "<<filename<<endl;
  string comment = "#";
  InitDB();
  vector<string> strvect;
  TaString sinput,sline;
  while (rdb->NextLine()) {
     strvect.clear();  
     sinput = TaString(rdb->GetLine());
//     cout << sinput;
     strvect = sinput.Split();
     sline = "";
     for (vector<string>::iterator str = strvect.begin();
        str != strvect.end(); str++) {
        if ( *str == comment ) break;
        sline += *str;   sline += " ";
     }
     if (sline == "") continue;
     strvect = sline.Split();
     multimap<string, vector<dtype*> >::iterator dmap = 
         database.find(FindTable(strvect[0]));
     if (dmap == database.end()) continue; 
     vector<dtype*> dv = dmap->second;
     vector<dtype*> datavect;  datavect.clear();
     for (int i = 1; i < (long)strvect.size(); i++) {
       if (i-1 < (long)dv.size()) {
         dtype *dat = new dtype(dv[i-1]->GetType()); 
         dat->Load(strvect[i]);   
         datavect.push_back(dat);
       }
     }
     LoadTable(FindTable(strvect[0]), datavect);
  }
  didread = kTRUE;
};

void TaDataBase::Print() {
// Human-readable printout for end-of-run summary
// This tells the user what variables were used for this run.
// Print() should be called BEFORE any Put() method is used
// because it shows the state of database at start of analysis.
  int i,j;
  clog << endl; 
  for (i = 0; i < 50; i++) clog << "-"; 
  clog << endl;
  clog << "DataBase File Used:  " << fileRead << endl;
  clog << "Database Variables Used for This Run " << endl;
  clog << "   Analysis type :  " << GetAnaType() << endl;
  string str = GetSimulationType();
  if (str != "none") {
    clog << "   !!!                                        !!!" << endl;
    clog << "   !!!!!!!!!! Simulation : " << str <<  "  !!!!!!!!!!" << endl;
    clog << "   !!!                                        !!!" << endl;
  }
  clog << "   ROOT file compression :   " << GetCompress() << endl;
  clog << "   Max events :  " << GetMaxEvents() << endl;
  clog << "   Window delay :  " << GetDelay() << endl;
  clog << "   Oversample :   " << GetOverSamp() << endl;
  clog << "   Pair type :  " << GetPairType() << endl;
  vector<string> cutnames = GetCutNames();
  clog << "   Number of cuts used :  " << GetNumCuts()<<endl;
  for (UInt_t i = 0; i < GetNumCuts(); i++) {
   clog << "     Cut name  " << cutnames[i] <<endl;
  }
  vector<string> trees = GetTrees();
  clog << "   Trees used :  ";
  for (i = 0; i < trees.size(); i++) 
    {
      clog << " " << trees[i];
    }
  clog << endl;
  clog << "   Low beam cut :  " << GetCutValue("lobeam") << endl;
  clog << "   Low beam C cut :  " << GetCutValue("lobeamc") << endl;
  clog << "   Burp cut :  " << GetCutValue("burpcut") << endl;
  clog << "   Det saturation cut :  " << GetCutValue("satcut") << endl;
  clog << "   Mon saturation cut :  " << GetCutValue("monsatcut") << endl;
  clog << "   Hall C Burp cut :  " << GetCutValue("cburpcut") << endl;
  clog << "   ADCX DAC Burp cut : " << GetCutValue("adcxdacburpcut") << endl;

  vector<TaString> vposmon = GetStringVect("posmon");
  UInt_t npm = vposmon.size();
  vector<Double_t> vposbcut = GetCutValueDVector("posburp");
  UInt_t npc = vposbcut.size();
  if (npc < npm) vposmon.resize(npc);
  clog << "   Position Burp Cut : " << endl;
  vector<Double_t>::iterator ipbc = vposbcut.begin();
  for(vector<TaString>::iterator iconst = vposmon.begin();
      iconst != vposmon.end(); iconst++) {
    if (iconst->size()>1 && (*ipbc)!=0 )
      clog << "        " << *iconst << " burpcut = " << *ipbc << endl;
    ipbc++;    
  }

  vector<TaString> vposmonE = GetStringVect("posmone");
  UInt_t npmE = vposmonE.size();
  vector<Double_t> vposbcutE = GetCutValueDVector("posburpe");
  UInt_t npcE = vposbcutE.size();
  if (npcE < npmE) vposmonE.resize(npcE);
  clog << "   Energy Burp Cut : " << endl;
  vector<Double_t>::iterator ipbcE = vposbcutE.begin();
  for(vector<TaString>::iterator iconst = vposmonE.begin();
      iconst != vposmonE.end(); iconst++) {
    if (iconst->size()>1 && (*ipbcE)!=0 )
      clog << "        " << *iconst << " burpcutE = " << *ipbcE << endl;
    ipbcE++;    
  }

  clog << "   Hall C Burp cut :  " << GetCutValue("cburpcut") << endl;

  vector<Double_t> wts = GetDetWts();
  if (wts.size() > 0)
    {
      clog << "   Detector weights :  ";
      for (vector<Double_t>::const_iterator iw = wts.begin(); iw != wts.end(); ++iw)
	clog << (*iw) << " ";
      clog << endl;
    }
  wts = GetBlumiWts();
  if (wts.size() > 0)
    {
      clog << "   Blumi weights :  ";
      for (vector<Double_t>::const_iterator iw = wts.begin(); iw != wts.end(); ++iw)
	clog << (*iw) << " ";
      clog << endl;
    }
  wts = GetFlumiWts();
  if (wts.size() > 0)
    {
      clog << "   Flumi weights :  ";
      for (vector<Double_t>::const_iterator iw = wts.begin(); iw != wts.end(); ++iw)
	clog << (*iw) << " ";
      clog << endl;
    }

  vector<Int_t> extlo = GetExtLo();
  vector<Int_t> exthi = GetExtHi();
  Int_t nlo = extlo.size();
  Int_t nhi = exthi.size();
  Int_t n = nlo;
  if (nlo != nhi) {
    clog << "ERROR: Unequal number of low and high intervals"<<endl;
    if (nhi < nlo) n = nhi;
  }
  Int_t ncutext = 0;
  for (i = 0; i < n; i++) {
    if (extlo[i] != 0 || exthi[i] != 0) ncutext++;
  }
  clog << "   Num of (non-zero) cut extensions :  " << ncutext << endl;
  if (n != 0) clog << "    cut     extlo     exthi"<<endl;  
  for (i = 0; i < ncutext; i++) {
    clog << "      " << i << "      " << extlo[i];
    clog << "       " << exthi[i] << endl;
  }
  clog << "   Number of cut intervals :  "<<GetNumBadEv()<<endl;
  map <Int_t, vector<Int_t> > cutint = GetCutInt();
  n = GetNumBadEv();
  if (GetNumBadEv() > (long)cutint.size()) {
    clog << "ERROR: Contradictory number of cut intervals"<<endl;
    n = cutint.size();
  }
  if (n != 0) 
   clog << "   interval    evlo    evhi   cut num   cut value"<<endl;
  for (i = 0; i < n; i++) {
    clog << "     " << i << "   ";
    vector<Int_t> cuts = cutint[i];
    int nc = cuts.size();
    if (nc > 4) nc = 4;
    for (j = 0; j < nc; j++) {
      clog << "     "<<cuts[j];
    }
    clog << endl;
  }
  for (i = 0; i < 50; i++) clog << "-"; 
  clog << endl;

};

Bool_t TaDataBase::SelfCheck() {
// Returns kTRUE if the database makes good sense.  
// This enforces some rules.
   string stest;
   Int_t itest;
   vector<string> keys;
   TaKeyMap keymap = datamap["tir"];  // must have tir data in datamap
   keys = keymap.GetKeys();
   Bool_t allok = kTRUE;
   Int_t tirok = 0;
   for (vector<string>::iterator ikey = keys.begin(); ikey != keys.end(); ikey++) {
     string key = *ikey;
     if (key == "tirdata") tirok = 1;
   }
   if ( !tirok ) {
     cerr << "DataBase:: SelfCheck ERROR:  No tirdata defined in database"<<endl;
     allok = kFALSE;
   }
   stest = GetPairType();
   if (strlen(stest.c_str())-1 <= 0) {
      cerr << "DataBase:: SelfCheck ERROR:  No pair type defined in database"<<endl;
      allok = kFALSE;
   }
   itest = GetDelay();
   if (itest != 0 && itest != 16 && itest != 8 ) {
     cerr << "DataBase:: WARNING:  windelay = "<<itest<<" seems wrong."<<endl;
   }
   if (itest < 0 || itest > 24 ) {
     cerr << "DataBase:: SelfCheck ERROR:  'windelay' = " << itest << " outside range 0-12" <<endl;
     allok = kFALSE;
   }
   itest = GetOverSamp();
   if (itest <= 0 || itest > 25 ) {
     cerr << "DataBase:: SelfCheck ERROR:  'oversamp' = " << itest << " outside range 1-25" <<endl;
     allok = kFALSE;
   }
   itest = GetCompress();
   if (itest < 0 || itest > 9 ) {
     cerr << "DataBase:: SelfCheck ERROR:  'compression' = " << itest << " outside range 0-9" <<endl;
     allok = kFALSE;
   }
   stest = GetAnaType();
   if (strlen(stest.c_str())-1 <= 0) {
      cerr << "DataBase:: SelfCheck WARNING:  No 'anatype' defined in database"<<endl;
      allok = kFALSE;
   }
   return allok;
};

void 
TaDataBase::Checkout()
{
 // Thorough debug checkout of database.  Ok, this is
 // partly redundant with Print()
  cout << "\n\nCHECKOUT of DATABASE for Run " << GetRunNum() << endl;
  cout << "ROOT file compression = " << GetCompress() << endl;
  cout << "Max events = " << GetMaxEvents() << endl;
  cout << "lobeam  cut = " << GetCutValue("LOBEAM") << endl;
  cout << "lobeamc  cut = " << GetCutValue("LOBEAMC") << endl;
  cout << "burpcut  cut = " << GetCutValue("BURPCUT") << endl;
  cout << "satcut  cut = " << GetCutValue("SATCUT") << endl;
  cout << "monsatcut  cut = " << GetCutValue("MONSATCUT") << endl;
  clog << "cburpcut cut :  " << GetCutValue("cburpcut") << endl;

  vector<TaString> vposmon = GetStringVect("posmon");
  UInt_t npm = vposmon.size();
  vector<Double_t> vposbcut = GetCutValueDVector("posburp");
  UInt_t npc = vposbcut.size();
  if (npc < npm) vposmon.resize(npc);
  clog << "posburp  cut: " << endl;
  vector<Double_t>::iterator ipbc = vposbcut.begin();
  for(vector<TaString>::iterator iconst = vposmon.begin();
       iconst!=vposmon.end(); iconst++) {
    if (iconst->size()>1 && (*ipbc)!=0) 
      clog << "     " << *iconst << " : " << *ipbc << endl;
    ipbc++;
  }
  
  vector<TaString> vposmonE = GetStringVect("posmone");
  UInt_t npmE = vposmonE.size();
  vector<Double_t> vposbcutE = GetCutValueDVector("posburpe");
  UInt_t npcE = vposbcutE.size();
  if (npcE < npmE) vposmonE.resize(npcE);
  clog << "   Energy Burp Cut : " << endl;
  vector<Double_t>::iterator ipbcE = vposbcutE.begin();
  for(vector<TaString>::iterator iconst = vposmonE.begin();
      iconst != vposmonE.end(); iconst++) {
    if (iconst->size()>1 && (*ipbcE)!=0 )
      clog << "        " << *iconst << " burpcutE = " << *ipbcE << endl;
    ipbcE++;    
  }

  cout << "window delay = " << GetDelay() << endl;
  cout << "oversampling factor = " << GetOverSamp() << endl;
  cout << "pair type (i.e. pair or quad or octet) =  " << GetPairType() << endl;
  cout << "\n\nPedestal and Dac noise parameters by ADC# and channel# : " << endl;
  for (int adc = 0; adc < 10 ; adc++) {
    cout << "\n\n-----  For ADC " << adc << endl;
    for (int chan = 0; chan < 4; chan++) {
      cout << "\n  channel " << chan;
      cout << "   ped = " << GetAdcPed(adc,chan);
      cout << "   dac slope = " << GetDacNoise(adc,chan,"slope");
    }
  }  
  for (int adcx = 0; adcx < 10 ; adcx++) {
    cout << "\n\n-----  For ADCX " << adcx << endl;
    for (int chan = 0; chan < 4; chan++) {
      cout << "\n  channel " << chan;
      cout << "   ped = " << GetAdcxPed(adcx,chan);
      cout << "   dac slope = " << GetDacxNoise(adcx,chan,"slope");
    }
  }  
  for (int vqwk = 0; vqwk < 1 ; vqwk++) {
    cout << "\n\n-----  For VQWK " << vqwk << endl;
    for (int chan = 0; chan < 8; chan++) {
      cout << "\n  channel " << chan;
      cout << "   ped = " << GetVqwkPed(vqwk,chan);
    }
  }  
  cout << "\n\nPedestal parameters for a few scalers : " << endl;
  for (int scal = 0; scal < 2 ; scal++) {
    cout << "\n\n-----  For Scaler " << scal << endl;
    for (int chan = 0; chan < 8; chan++) {
      cout << "\n  channel " << chan;
      cout << "   ped = " << GetScalPed(scal,chan);
    }
  }  

  vector<Double_t> wts = GetDetWts();
  cout << "   Detector weights :  ";
  for (vector<Double_t>::const_iterator iw = wts.begin(); iw != wts.end(); ++iw)
    cout << (*iw) << " " << endl;
  wts = GetBlumiWts();
  cout << "   Blumi weights :  ";
  for (vector<Double_t>::const_iterator iw = wts.begin(); iw != wts.end(); ++iw)
    cout << (*iw) << " " << endl;
  wts = GetFlumiWts();
  cout << "   Flumi weights :  ";
  for (vector<Double_t>::const_iterator iw = wts.begin(); iw != wts.end(); ++iw)
    cout << (*iw) << " " << endl;

  cout << "\n\nNumber of cuts " << GetNumCuts() << endl;
  vector<Int_t> extlo = GetExtLo();
  vector<Int_t> exthi = GetExtHi();
  for (int i = 0; i < GetNumCuts(); i++) { 
    if (i >= (long)exthi.size()) cout << "extlo and exthi mismatched size" << endl;
    cout << "Cut " << i << "   Extlo  = " << extlo[i] << "  Exthi = " << exthi[i] << endl;
  }  
  cout << "\n\nNum cut event intervals " << GetNumBadEv() << endl;
  map<Int_t, vector<Int_t> > evint = GetCutInt();
  Int_t k = 0;
  for (map<Int_t, vector<Int_t> >::iterator icut = evint.begin();
     icut != evint.end(); icut++) {
     vector<Int_t> cutint = icut->second;
     cout << "Cut interval " << dec << k++;
     cout << "  from event " << cutint[0] << " to " << cutint[1];
     cout << "  mask " << cutint[2] << "   veto " << cutint[3] << endl;
  }

  vector<Double_t> qpd1const = GetQpd1Const();
  for(vector<Double_t>::iterator iconst = qpd1const.begin();
      iconst != qpd1const.end(); iconst++) {
    Double_t qval = *iconst;
    cout << "qpd1 const "<< qval <<endl;
  }

  vector<Double_t> lina1const = GetLina1Const();
  for(vector<Double_t>::iterator iconst = lina1const.begin();
      iconst != lina1const.end(); iconst++) {
    Double_t laval = *iconst;
    cout << "lina1 const "<< laval <<endl;
  }

  vector<Double_t> cavconst = GetCavConst1();
  vector<Double_t>::iterator iconst = cavconst.begin();
  cout << "Calibration for Cavity 1: " << endl;
  cout << "    X-cal = " << (Double_t) *(iconst) << endl;
  cout << "    Y-cal = " << (Double_t) *(iconst+1) << endl;

  cavconst = GetCavConst2();
  iconst = cavconst.begin();
  cout << "Calibration for Cavity 2: " << endl;
  cout << "    X-cal = " << (Double_t) *(iconst) << endl;
  cout << "    Y-cal = " << (Double_t) *(iconst+1) << endl;

  cavconst = GetCavConst3();
  iconst = cavconst.begin();
  cout << "Calibration for Cavity 3: " << endl;
  cout << "    X-cal = " << (Double_t) *(iconst) << endl;
  cout << "    Y-cal = " << (Double_t) *(iconst+1) << endl;

}

void TaDataBase::PrintDataBase() {
  // Technical printout of entire database
  cout << "\n\n--- Printout of database ---"<<endl;
  for (multimap<string, vector<dtype*> >::iterator dmap = database.begin();
       dmap != database.end() ;  dmap++) {
    string table = dmap->first;
    vector<dtype*> datatypes = dmap->second;
    cout << "\nTable :  "<<table<<endl;
    for (vector<dtype*>::iterator d = datatypes.begin();
      d != datatypes.end(); d++) {
      dtype* dat = *d;
      cout << " | Data Type "<<dat->GetType();
      cout << " Value = ";
      if (dat->GetType() == "i") cout << dat->GetI();
      if (dat->GetType() == "d") cout << dat->GetD();
      if (dat->GetType() == "s") cout << dat->GetS();
    }
  }
};

void TaDataBase::PrintDataMap() {
  // Technical printout of data map.
  cout << "\n\n--- Printout of datamap ---"<<endl;
  for (multimap<string, TaKeyMap >::const_iterator dmap = datamap.begin();
	 dmap != datamap.end() ;  dmap++) {
    string devname = dmap->first;
    cout << "\nDevice name "<<devname<<"  and associated keymaps: "<<endl;
    TaKeyMap keymap = dmap->second;
    keymap.Print();
  }
};

string TaDataBase::GetAnaType() const {
// Get analysis type, e.g. anatype = 'beam' 
   static multimap<string, vector<dtype*> >::const_iterator dmap;
   dmap = database.lower_bound("anatype");
   if (dmap == database.end()) return "unknown";
   if (database.count("anatype") != 1) return "ambiguous";
   vector<dtype*> datatype = dmap->second;
   if (datatype.size() > 1) return "ambiguous";
   if (datatype[0]->GetType() == "s") return datatype[0]->GetS(); 
   return "unknown";
};

string 
TaDataBase::GetTimestamp() const 
{
  // Get timestamp in format "yyyy-mm-dd hh:mm:ss"
   static multimap<string, vector<dtype*> >::const_iterator dmap;
   dmap = database.lower_bound ("timestamp");
   if (dmap == database.end()) 
     return "unknown";
   if (database.count ("timestamp") != 1) 
     return "ambiguous";
   vector<dtype*> datatype = dmap->second;
   if (datatype.size() != 2) 
     return "ambiguous";
   if (datatype[0]->GetType() == "s" && datatype[1]->GetType() == "s") 
     return datatype[0]->GetS() + " " + datatype[1]->GetS(); 
   return "unknown";
};

TDatime
TaDataBase::GetTimeTDatime() const 
{
// Get timestamp as a TDatime object
  TDatime result;
  result.Set(20000000,0);
  static multimap<string, vector<dtype*> >::const_iterator dmap;
  dmap = database.lower_bound ("timestamp");
  if (dmap == database.end()) 
     return result;
   if (database.count ("timestamp") != 1) 
     return result;
   vector<dtype*> datatype = dmap->second;
   if (datatype.size() != 2) 
     return result;
   if (datatype[0]->GetType() == "s" && datatype[1]->GetType() == "s") {
     TaString sdate = TaString(datatype[0]->GetS());
     TaString stime = TaString(datatype[1]->GetS());
     Int_t idate = atoi(sdate.RemoveChar("-").c_str());
     Int_t itime = atoi(stime.RemoveChar(":").c_str());
     result.Set(idate,itime);
   }
   return result;
};

TString 
TaDataBase::GetTimeTString() const 
{
  TString result(GetTimestamp().c_str());
  return result;
};


string 
TaDataBase::GetRandomHeli () const
{
  // Return control for random vs toggled helicity
  
  return GetString ("randomheli");
}

string 
TaDataBase::GetBlindingString () const
{
  // Return the (first word of the) blinding string 
  
  string bs = GetString ("blindstring");
  if (bs == " ")
    bs = "none";
  return bs;
}

vector<Double_t>
TaDataBase::GetBlindingParams() const
{
  // Return the blinding parameters -- sign, mean, and offscale.
  // Return defaults if absent from database.  Length of returned
  // vector is always 3.

  vector<string> keys;
  keys.clear();
  keys.push_back ("sign");   
  keys.push_back ("mean");
  keys.push_back ("offscale");
  vector<Double_t> data = GetData ("blindparams", keys);

  // If parameters missing, push default values.
  if (data.size() == 0)
    data.push_back (1.);
  if (data.size() == 1)
    data.push_back (0.);
  if (data.size() == 2)
    data.push_back (0.);

  // Flush excess entries
  data.resize (3);
  return data;
}

vector<Double_t>
TaDataBase::GetQpd1Const() const
{

  vector<string> keys;
  keys.clear();
  keys.push_back ("pp");   
  keys.push_back ("pm");
  keys.push_back ("mp");
  keys.push_back ("mm");
  keys.push_back ("x-const");
  keys.push_back ("y-const");
  
  vector<Double_t> data = GetData ("qpd1const", keys);

  // If parameters missing, push default values.
  if (data.size() != 6) {
    data.push_back (1.);  // relative gains
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // distance conversions
    data.push_back (1.);  // 
  }
  return data;
}

vector<Double_t>
TaDataBase::GetLina1Const() const
{

  vector<string> keys;
  keys.clear();
  keys.push_back ("npad");   
  keys.push_back ("g1");
  keys.push_back ("g2");
  keys.push_back ("g3");
  keys.push_back ("g4");
  keys.push_back ("g5");
  keys.push_back ("g6");
  keys.push_back ("g7");
  keys.push_back ("g8");
  keys.push_back ("spc");
  
  vector<Double_t> data = GetData ("lina1const", keys);

  // If parameters missing, push default values.
  if (data.size() != 10) {
    data.push_back (4.0);   // number of samples
    data.push_back (1.);  // relative gains
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // 
    data.push_back (1.);  // distance conversion
  }
  return data;
}

vector<Double_t>
TaDataBase::GetCavConst1() const
{
  // Return the Cavity monitor calibration parameters-
  //    xcal, ycal.
  // Return defaults if absent from database.  

  vector<string> keys;
  keys.clear();
  keys.push_back ("x-const");
  keys.push_back ("y-const");

  vector<Double_t> data = GetData ("cav1const", keys);

  // If parameters missing, push default values.
  if (data.size() != 2) {
    data.push_back (1.);  // x-const
    data.push_back (1.);  // y-const
  }
  return data;
}

vector<Double_t>
TaDataBase::GetCavConst2() const
{
  // Return the Cavity monitor calibration parameters-
  //    xcal, ycal.
  // Return defaults if absent from database.  

  vector<string> keys;
  keys.clear();
  keys.push_back ("x-const");
  keys.push_back ("y-const");

  vector<Double_t> data = GetData ("cav2const", keys);

  // If parameters missing, push default values.
  if (data.size() != 2) {
    data.push_back (1.);  // x-const
    data.push_back (1.);  // y-const
  }
  return data;
}

vector<Double_t>
TaDataBase::GetCavConst3() const
{
  // Return the Cavity monitor calibration parameters-
  //    xcal, ycal.
  // Return defaults if absent from database.  

  vector<string> keys;
  keys.clear();
  keys.push_back ("x-const");
  keys.push_back ("y-const");

  vector<Double_t> data = GetData ("cav3const", keys);

  // If parameters missing, push default values.
  if (data.size() != 2) {
    data.push_back (1.);  // x-const
    data.push_back (1.);  // y-const
  }
  return data;
}

string TaDataBase::GetSimulationType()  const {
// returns pair type (pair or quad or octet) for this run.
  string stest = GetString("simtype");
  if (strlen(stest.c_str())-1 <= 0) return "none";
  return stest;
};


string TaDataBase::GetFdbkSwitch( const string &fdbktype )const {
  // get the feedback switch state corresponding to feedback type fdbktype.
  string table = "feedback";
  if ( database.count("feedback") > 1 ){
    clog<<" feedback"<<fdbktype<<" switched "
        <<GetData(table,fdbktype, 0)<<endl;
    return GetData(table,fdbktype, 0);
  }
  else{
    return "";
  }     
}; 

Int_t TaDataBase::GetFdbkTimeScale( const string &fdbktype ) const{
  // get the timescale of feedback type fdbktype
  string table = "feedback";
 if ( database.count("feedback") >1){ 
     multimap<string, vector< dtype* > >::const_iterator lb = database.lower_bound(table);        
     multimap<string, vector< dtype* > >::const_iterator ub = database.upper_bound(table);        
     for ( multimap<string, vector< dtype* > >::const_iterator dmap = lb; dmap != ub; dmap++){
       vector< dtype* > datatype = dmap->second;
       if (datatype[0]->GetType() == "s"){
	 if ( TaString(datatype[0]->GetS()).CmpNoCase(fdbktype) == 0 ){
	   if (datatype[2]->GetType() == "i") return datatype[2]->GetI();
           clog<<" feedback "<<fdbktype<<" timescsale "<<datatype[2]->GetI()<<endl;
	 }
       }
     }
     return 0;
 }
 else{
   cerr<<"DataBase::GetFdbkTimeScale( const string &fdbktype ) ERROR\n"
	<<"TABLE not declared correctly, need feedback table. "
	<<"Example of line format in .db file:\n"
        <<" feedback    AQ   on   3  bcm1 "<<endl;
   return 0;
 }
};
   
string TaDataBase::GetFdbkMonitor( const string &fdbktype ) const{
  // get the timescale of feedback type fdbktype
  string table = "feedback";
 if ( database.count("feedback") >1){ 
     multimap<string, vector< dtype* > >::const_iterator lb = database.lower_bound(table);        
     multimap<string, vector< dtype* > >::const_iterator ub = database.upper_bound(table);        
     for ( multimap<string, vector< dtype* > >::const_iterator dmap = lb; dmap != ub; dmap++){
       vector< dtype* > datatype = dmap->second;
       if (datatype[0]->GetType() == "s"){
	 if ( TaString(datatype[0]->GetS()).CmpNoCase(fdbktype) == 0 ){
	   if (datatype[3]->GetType() == "s") {
             return datatype[3]->GetS();
	   }
	 }
       }
     }
     return 0;
 }
 else{
   cerr<<"DataBase::GetFdbkMonitor( const string &fdbktype ) ERROR\n"
	<<"TABLE not declared correctly, need feedback table. "
	<<"Example of line format in .db file:\n"
        <<" feedback    AQ   on   3  bcm1 "<<endl;
   return 0;
 }
};   

void TaDataBase::Mysql(string action) {
// Read or Write the database from Mysql server.
// If reading, the ASCII database will be overwritten, so it is a 
// good idea to keep backups.
// If writing, it is assumed the ASCII database was already written.
// Syntax of the Mysql script command is:
//     $MYSQL_SCRIPT_DIR/mysql.pl  action  dbfile  run
// where
//   $MYSQL_SCRIPT_DIR = environment variable pointing to script
//   mysql.pl = Perl script that does all the work.
//   action = "read" or "write"
//   dbfile = name of ASCII database file to read or write
//   run = run number

#ifdef TURN_OFF_MYSQL
  
  cout << "Mysql script is not yet ready.  No Mysql I/O."<<endl;

#else

  // The rest of this code should work right away when the
  // Mysql Perl script is ready.

  if (action != "read" && action != "write") {
    cerr << "ERROR: Mysql:  Action must be either read or write"<<endl;
    cerr << "Attempted to use bogus action = "<<action<<endl;
    return;
  }
  rootdb->Clear();
  TaFileName dbFileName ("db");
  char mysql_command[100],srun[20];
  char *script_dir = getenv("MYSQL_SCRIPT_DIR");
  if (script_dir != NULL) strcpy(mysql_command, script_dir);
  sprintf(mysql_command,"mysql.pl ");
  strcat(mysql_command, action.c_str());
  strcat(mysql_command, " ");
  strcat(mysql_command, dbFileName.String().c_str());
  sprintf(srun,"  %d",runnum);
  strcat(mysql_command, srun);
  cout << "Executing MYSQL script command : ";
  cout << mysql_command << endl << endl;
  int systat, retstat, err;
  systat = system(mysql_command);  // Execute the Perl script.
// Parse the system status and return status from script.
  err = 1;
  if (WIFEXITED(systat) != 0) {
    retstat = WEXITSTATUS(systat);
    switch (retstat) {
      case 0:
	cout << "Mysql script succesful."<<endl;
        err = 0;
        break;
      case 1:
// FIXME: We must decide what these errors from MYSQL script mean.
	cout << "Mysql::ERROR: 1"<<endl;
        break;
      case 2:
	cout << "Mysql::ERROR: 2"<<endl;
        break;
      case 3:
	cout << "Mysql::ERROR: 3"<<endl;
        break;
      default:
        cout << "Mysql:: ERROR: Abnormal system status !!"<<endl;
        cout << "Possibly the script does not exist or has a typo ?"<<endl;
        cout << "Try to execute the script by hand.  Type at shell:"<<endl;
        cout << "    " << mysql_command << endl<<endl;
    }
  } else {
// In the following, one could figure out what is wrong by 'man waitpid'
// and use the macros explained there to parse 'systat' but I guess this 
// will be a very rare failure, so I leave the complaint simple.
     cout << "TaDataBase::Mysql:: ERROR: ";
     cout << "  Abnormal system status !!"<<endl;
     cout << "Possibilities include: "<<endl;
     cout << "  process killed, stopped, core dumped, etc."<<endl;
  }
  if (err) {
    cout << "TaDataBase::Mysql: ERROR initializing database"<<endl;
    return;
  }
#endif
}

void 
TaDataBase::Write() {
// Write the database for this run (if data was "Put").
// Output goes to both an ASCII file and to MYSQL.
// This method should be called AFTER all Put() methods are used.
  if ( !didput ) return;
  ToRoot();
  TaFileName dbFileName ("db");
  ofstream *ofile = new ofstream(dbFileName.String().c_str());
  TaString soutput;
  while (rootdb->NextLine()) {
     soutput = TaString(rootdb->GetLine());
     int linesize = strlen(soutput.c_str());   
     ofile->write ( soutput.c_str(), linesize );
  }
  Mysql("write");
};


Cut_t
TaDataBase::GetCutNumber (TaString s) const
{
  // Return cut number corresponding to a given name (case insensitive match). 
  // If no match, return fNumCuts.

  UInt_t numcuts = (UInt_t) GetNumCuts();
  vector<string> cutnames = GetCutNames();
  Cut_t i;
  for (i = 0; i < numcuts && s.CmpNoCase(cutnames[i]) != 0; ++i)
    {} // null loop body
  return i;
}

Bool_t
TaDataBase::GetTreeUsed (TaString s) const
{
  // Return whether the given string is in the list of trees used

  vector<string> trees = GetTrees();
  UInt_t i;
  for (i = 0; i < trees.size() && s.CmpNoCase (trees[i]) != 0; ++i)
    {} // null loop body
  return i < trees.size();
}

Double_t TaDataBase::GetData(const string& table) const {
// Return single value from table "table".  This assumes the data
// are in a pair  "table   value" where table is a unique string and
// value the single Double_t that belongs to it.
   return GetValue(table); 
};

vector<Double_t> TaDataBase::GetData(string table, vector<string> keys) const {
// Generic get method if you know the 'table' and 'keys' you want.
// Structure is 'table (key, data) (key, data)' i.e. a series of
// pairs of (string key, double data) after the string table
// The entire list of keys must be found, not a parital match.
   vector<Double_t> result;  result.clear();
   multimap<string, vector<dtype*> >::const_iterator lb =
        database.lower_bound(table);
   multimap<string, vector<dtype*> >::const_iterator ub = 
        database.upper_bound(table);
   for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
        dmap != ub; dmap++) {
     vector<dtype*> datatype = dmap->second;
     Bool_t foundall = kTRUE;
     Int_t ikey = 0;
     for (vector<string>::const_iterator str = keys.begin(); 
       str != keys.end(); str++) {
       string key = *str;
       Bool_t foundkey = kFALSE;
       for (int k = 0; k < (long)datatype.size(); k++) {         
         if (datatype[k]->GetType() == "s") {
           if ( TaString(datatype[k]->GetS()).CmpNoCase(key) == 0 ) {
             foundkey = kTRUE;
             if (foundall) {
               result.push_back(GetData(datatype[k+1]));
             } else {
               if (ikey == 0) result.push_back(GetData(datatype[k+1]));
             }
           }
         }
       }
       ikey++;
       if (!foundkey) foundall = kFALSE;
     }
   }
   return result;
};

string TaDataBase::GetData(string table, string key, Int_t index) const {
// Return key'd data at index in dtype vector.
   multimap<string, vector<dtype*> >::const_iterator lb =
        database.lower_bound(table);
   multimap<string, vector<dtype*> >::const_iterator ub = 
        database.upper_bound(table);
   for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
        dmap != ub; dmap++) {
     vector<dtype*> datatype = dmap->second;
     if (index >= 0 && index+1 < (long)datatype.size()) {
       if (datatype[0]->GetType() == "s") {
         if ( TaString(datatype[0]->GetS()).CmpNoCase( key) == 0 ) {
            if (datatype[index+1]->GetType() == "s") 
               return datatype[index+1]->GetS();
         }
       }
     }
   }
   return "0";
};

Double_t TaDataBase::GetData(dtype *d) const {
  if (d->GetType() == "i") return (Double_t)d->GetI();
  if (d->GetType() == "d") return d->GetD();
  if (d->GetType() == "s") return 0;
  return 0;
};

Double_t TaDataBase::GetDacNoise(const Int_t& adc, const Int_t& chan, const string& key) const {
// Get Dac noise parameters for adc,chan with key = 'slope'
  if (!didinit) {
      cerr << "DataBase::GetDacNoise ERROR: Database not initialized\n";
      return 0;
  }
  int idx;
  if (*fFirstgdn)  {
     *fFirstgdn = kFALSE;
     if (!didread) {
         cerr << "DataBase::GetDacNoise:: WARNING: ";
         cerr << "Did not read any database yet\n";
     }
     vector<string> keys;
     keys.clear();
     keys.push_back("adc");   
     keys.push_back("chan");
     keys.push_back("slope");
     vector<Double_t> data = GetData("dacnoise",keys);
     int iadc,ichan;
     int index = 0;
     while (index < (long)data.size()) 
       {
        if ((index+(long)keys.size()-1) < (long)data.size()) 
	  {
           iadc = (int)data[index];
           ichan= (int)data[index+1];
           idx = iadc*MAXCHAN+ichan;
           if (idx < MAXADC*MAXCHAN) 
             {
              dacparam[idx] = data[index+2];
             }
          }
        index += keys.size();
      }
  }
    idx = adc*MAXCHAN+chan;
    if (idx >= 0 && idx < MAXADC*MAXCHAN) 
      {
       if (TaString(key).CmpNoCase("slope") == 0) return dacparam[idx];
      }
    else 
      {
     cerr << "WARNING: DataBase::GetDacNoise:";
     cerr << "  illegal combination of adc and channel #"<<endl;
     cerr << "  idx = " << idx << " adc "<<adc<<"  chan "<<chan<<endl;
     return 0;
      }
  return 0;
};

Double_t TaDataBase::GetAdcPed(const Int_t& adc, const Int_t& chan) const {
// Get Pedestals for adc, chan 
  if (!didinit) {
      cerr << "DataBase::GetAdcPed ERROR: Database not initialized\n";
      return 0;
  }
  int idx;
  if (*fFirstAdcPed) {
    *fFirstAdcPed = kFALSE;
    if (!didread) {
         cerr << "DataBase::GetAdcPed:: WARNING: ";
         cerr << "Did not read any database yet\n";
    }
    vector<string> keys;
    keys.clear();
    keys.push_back("adc");   // this must match a string in database
    keys.push_back("chan");
    keys.push_back("value");
    vector<Double_t> data = GetData("ped",keys);
    int iadc,ichan;
    int index = 0;
    while (index < (long)data.size()) {
      if ((index+(long)keys.size()-1) < (long)data.size()) {
        iadc = (int)data[index];
        ichan= (int)data[index+1];
        idx = iadc*MAXCHAN+ichan;
        if (idx < MAXADC*MAXCHAN) {
           adcped[idx] = data[index+2];
         }
      }
      index += keys.size();
    }
  }
  idx = adc*MAXCHAN+chan;
  if (idx >=0 && idx < MAXADC*MAXCHAN) {
     return adcped[idx];
  } else {
     cerr << "WARNING: DataBase::GetAdcPed:";
     cerr << "  illegal combination of adc and channel #"<<endl;
     return 0;
  }
};
Double_t TaDataBase::GetDacxNoise(const Int_t& adcx, const Int_t& chan, const string& key) const {
// Get Dac noise parameters for adcx,chan with key = 'slope'
  if (!didinit) {
      cerr << "DataBase::GetDacxNoise ERROR: Database not initialized\n";
      return 0;
  }
  int idx;
  if (*fFirstgdn)  {
     *fFirstgdn = kFALSE;
     if (!didread) {
         cerr << "DataBase::GetDacxNoise:: WARNING: ";
         cerr << "Did not read any database yet\n";
     }
     vector<string> keys;
     keys.clear();
     keys.push_back("adcx");   
     keys.push_back("chan");
     keys.push_back("slope");
     vector<Double_t> data = GetData("dacnoise",keys);
     int iadcx,ichan;
     int index = 0;
     while (index < (long)data.size()) 
       {
        if ((index+(long)keys.size()-1) < (long)data.size()) 
	  {
           iadcx = (int)data[index];
           ichan= (int)data[index+1];
           idx = iadcx*MAXCHAN+ichan;
           if (idx < MAXADCX*MAXCHAN) 
             {
              dacxparam[idx] = data[index+2];
             }
          }
        index += keys.size();
      }
  }
    idx = adcx*MAXCHAN+chan;
    if (idx >= 0 && idx < MAXADCX*MAXCHAN) 
      {
       if (TaString(key).CmpNoCase("slope") == 0) return dacxparam[idx];
      }
    else 
      {
     cerr << "WARNING: DataBase::GetDacxNoise:";
     cerr << "  illegal combination of adcx and channel #"<<endl;
     cerr << "  idx = " << idx << " adcx "<<adcx<<"  chan "<<chan<<endl;
     return 0;
      }
  return 0;
};

Double_t TaDataBase::GetAdcxPed(const Int_t& adcx, const Int_t& chan) const {
// Get Pedestals for adcx, chan 
  if (!didinit) {
      cerr << "DataBase::GetAdcxPed ERROR: Database not initialized\n";
      return 0;
  }
  int idx;
  if (*fFirstAdcxPed) {
    *fFirstAdcxPed = kFALSE;
    if (!didread) {
         cerr << "DataBase::GetAdcxPed:: WARNING: ";
         cerr << "Did not read any database yet\n";
    }
    vector<string> keys;
    keys.clear();
    keys.push_back("adcx");   // this must match a string in database
    keys.push_back("chan");
    keys.push_back("value");
    vector<Double_t> data = GetData("ped",keys);
    int iadcx,ichan;
    int index = 0;
    while (index < (long)data.size()) {
      if ((index+(long)keys.size()-1) < (long)data.size()) {
        iadcx = (int)data[index];
        ichan= (int)data[index+1];
        idx = iadcx*MAXCHAN+ichan;
        if (idx < MAXADCX*MAXCHAN) {
           adcxped[idx] = data[index+2];
         }
      }
      index += keys.size();
    }
  }
  idx = adcx*MAXCHAN+chan;
  if (idx >=0 && idx < MAXADCX*MAXCHAN) {
     return adcxped[idx];
  } else {
     cerr << "WARNING: DataBase::GetAdcxPed:";
     cerr << "  illegal combination of adcx and channel #"<<endl;
     return 0;
  }
};

Double_t TaDataBase::GetVqwkPed(const Int_t& vqwk, const Int_t& chan) const {
// Get Pedestals for Qweak ADC, chan 
  if (!didinit) {
      cerr << "DataBase::GetVqwkPed ERROR: Database not initialized\n";
      return 0;
  }
  int idx;
  if (*fFirstVqwkPed) {
    *fFirstVqwkPed = kFALSE;
    if (!didread) {
         cerr << "DataBase::GetVqwkPed:: WARNING: ";
         cerr << "Did not read any database yet\n";
    }
    vector<string> keys;
    keys.clear();
    keys.push_back("vqwk");   // this must match a string in the database
    keys.push_back("chan");
    keys.push_back("value");
    vector<Double_t> data = GetData("ped",keys);
    int ivqwk,ichan;
    int index = 0;
    while (index < (long)data.size()) {
      if ((index+(long)keys.size()-1) < (long)data.size()) {
        ivqwk = (int)data[index];
        ichan= (int)data[index+1];
        idx = ivqwk*MAXVQWKCHAN+ichan;
        if (idx < MAXVQWK*MAXVQWKCHAN) {
           vqwkped[idx] = data[index+2];
         }
      }
      index += keys.size();
    }
  }
  idx = vqwk*MAXVQWKCHAN+chan;
  if (idx >=0 && idx < MAXVQWK*MAXVQWKCHAN) {
     return vqwkped[idx];
  } else {
     cerr << "WARNING: DataBase::GetVqwkPed:";
     cerr << "  illegal combination of vqwk #"<<vqwk << " and channel #"<< chan <<endl;
     return 0;
  }
};


Double_t TaDataBase::GetScalPed(const Int_t& scal, const Int_t& chan) const {
// Get Pedestals for scaler, chan 
  if (!didinit) {
      cerr << "DataBase::GetScalPed ERROR: Database not initialized\n";
      return 0;
  }
  int idx;
  if (*fFirstScalPed) {
    *fFirstScalPed = kFALSE;
    if (!didread) {
         cerr << "DataBase::GetScalPed:: WARNING: ";
         cerr << "Did not read any database yet\n";
    }
    vector<string> keys;
    keys.clear();
    keys.push_back("scaler");   // this must match a string in the database
    keys.push_back("chan");
    keys.push_back("value");
    vector<Double_t> data = GetData("ped",keys);
    int iscal,ichan;
    int index = 0;
    while (index < (long)data.size()) {
      if ((index+(long)keys.size()-1) < (long)data.size()) {
        iscal = (int)data[index];
        ichan= (int)data[index+1];
        idx = iscal*MAXSCALCHAN+ichan;
        if (idx < MAXSCAL*MAXSCALCHAN) {
           scalped[idx] = data[index+2];
         }
      }
      index += keys.size();
    }
  }
  idx = scal*MAXSCALCHAN+chan;
  if (idx >=0 && idx < MAXSCAL*MAXSCALCHAN) {
     return scalped[idx];
  } else {
     cerr << "WARNING: DataBase::GetScalPed:";
     cerr << "  illegal combination of scaler and channel #"<<endl;
     return 0;
  }
};

UInt_t TaDataBase::GetHeader(const string& device) const {
// Get Headers for decoding
   string table = "header";
   return TaString(GetData(table, device, 0)).Hex();
};

UInt_t TaDataBase::GetMask(const string& device) const {
// Get Mask for decoding 
   string table = "header";
   return TaString(GetData(table, device, 1)).Hex();
};

Double_t TaDataBase::GetCutValue(const string& cutname) const {
// Get a cut value for 'value'.  e.g. value = 'lobeam', 'burpcut' 
   return GetValue(cutname);
};

vector<Double_t> TaDataBase::GetCutValueDVector(const string& cutname) const {
// Get a vector of values for 'value'.  e.g. value = 'posburp' 
   return GetValueDVector(cutname);
};


Int_t TaDataBase::GetNumCuts() const {
// Get number of cuts "ncuts" in the database.
   return (Int_t)GetValue("ncuts");
};

vector<string> 
TaDataBase::GetCutNames () const 
{
  vector<string> result;
  result.clear();
  multimap<string, vector<dtype*> >::const_iterator lb =
    database.lower_bound(TaString("cutnames").ToLower());
  multimap<string, vector<dtype*> >::const_iterator ub = 
    database.upper_bound(TaString("cutnames").ToLower());
  for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
       dmap != ub; dmap++) {
    vector<dtype*> datav = dmap->second;
    for (vector<dtype*>::const_iterator idat = datav.begin();
	 idat != datav.end(); idat++) {
      if ((*idat)->GetType() == "s") 
	result.push_back((*idat)->GetS());
    }
  }
  return result;
};

vector<string> 
TaDataBase::GetTrees () const 
{
  vector<string> result;
  result.clear();
  multimap<string, vector<dtype*> >::const_iterator lb =
    database.lower_bound(TaString("trees").ToLower());
  multimap<string, vector<dtype*> >::const_iterator ub = 
    database.upper_bound(TaString("trees").ToLower());
  for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
       dmap != ub; dmap++) {
    vector<dtype*> datav = dmap->second;
    for (vector<dtype*>::const_iterator idat = datav.begin();
	 idat != datav.end(); idat++) {
      if ((*idat)->GetType() == "s") 
	{
	  string ts = (*idat)->GetS();
	  if (ts != "" && ts != " ")
	    result.push_back(ts);
	}
    }
  }
  if (result.size() == 0)
    {
      // Not specified, return default
      result.push_back ("R");
      result.push_back ("P");
      result.push_back ("M");
      result.push_back ("E");
    }
  return result;
};

Bool_t TaDataBase::GetFillDitherOnly() const
{
// Return true if only dither-on events are to be included in the
// trees (default=false)
  TaString cv = GetString ("fillditheronly");
  return cv.CmpNoCase("true") == 0;
}

vector<Int_t> TaDataBase::GetExtLo() const {
// Get cut extensions, low and high 
   return GetValueVector("extlo");
};

vector<Int_t> TaDataBase::GetExtHi() const {
// Get cut evhi event intervals 
   return GetValueVector("exthi");
};


vector<Double_t> TaDataBase::GetDetWts() const 
{
  // Get detector weights

  return GetValueDVector ("detwts");
};

vector<Double_t> TaDataBase::GetBlumiWts() const 
{
  // Get blumi weights

  return GetValueDVector ("blumiwts");
};

vector<Double_t> TaDataBase::GetFlumiWts() const 
{
  // Get flumi weights

  return GetValueDVector ("flumiwts");
};

Int_t TaDataBase::GetNumBadEv() const{
// Get number of bad event intervals
   return nbadev;
};


map <Int_t, vector<Int_t> > TaDataBase::GetCutInt() const {
// For bad event intervals, get formatted results 
// First element of map goes from 0 to GetNumBadEv(), second is a vector of
// results in prescribed order: (evlo, evhi, cut num, cut value)
   map <Int_t, vector<Int_t> > result;
   vector<Int_t> temp;
   result.clear();
   Int_t k = 0;
   multimap<string, vector<dtype*> >::const_iterator lb =
          database.lower_bound("evint");
   multimap<string, vector<dtype*> >::const_iterator ub = 
          database.upper_bound("evint");
   for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
         dmap != ub; dmap++) {
     vector<dtype*> datav = dmap->second;
     temp.clear();
     for (vector<dtype*>::iterator idat = datav.begin();
          idat != datav.end(); idat++) {
       if ((*idat)->GetType() == "i") 
           temp.push_back((*idat)->GetI());
     }
     result.insert(make_pair(k++, temp));
   }
   return result;   
};

Int_t TaDataBase::GetCompress() const {
// returns ROOT file compression factor
   return (Int_t)GetValue("compress");
};
 
Int_t TaDataBase::GetMaxEvents() const {
// returns number of events to process
   return (Int_t)GetValue("maxevents");
};

Int_t TaDataBase::GetDelay() const {
// returns helicity delay (in windows)
   return (Int_t)GetValue("windelay");
};

Int_t TaDataBase::GetOverSamp() const {
// returns oversample factor
   return (Int_t)GetValue("oversamp");
};
 
string TaDataBase::GetPairType()  const {
// returns pair type (pair or quad or octet) for this run.
   return GetString("pairtype");
};

string TaDataBase::GetCurMon() const
{
// Get current monitor to use for cuts and normalizations for this run
  string cm = GetString ("curmon");
  if (strlen(cm.c_str())-1 <= 0)
    cm = "none";
  return cm;
};

string TaDataBase::GetCurMonC() const
{
// Get current monitor to use for Hall C cuts for this run
  string cm = GetString ("curmonc");
  if (strlen(cm.c_str())-1 <= 0)
    cm = "none";
  return cm;
};

Bool_t TaDataBase::GetCalVar() const
{
// Return true if calibration variables (not pedestal subtracted) are
// to be included in the raw data tree (default=false)
  TaString cv = GetString ("calvar");
  if (cv.CmpNoCase("true")==0) {
    return kTRUE;
  }
  return kFALSE;
}

Double_t TaDataBase::GetValue(const string& table) const {
// Return single value from table "table".  This assumes the data
// are in a pair  "table   value" where table is a unique string and
// value the single Double_t that belongs to it.
   static multimap<string, vector<dtype*> >::const_iterator dmap;
   dmap = database.lower_bound(TaString(table).ToLower());
   if (dmap == database.end() || 
       database.count(TaString(table).ToLower()) == 0) {
     cerr << "ERROR: DataBase: Unknown database table "<<table<<endl;
     return 0;
   }
   if (database.count(TaString(table).ToLower()) > 1) {
     cerr << "ERROR: DataBase: Mulitply defined table "<<table<<endl;
     cerr << "Fix the database to have one instance."<<endl;
     return 0;
   }
   vector<dtype*> datatype = dmap->second;
   if (datatype[0]->GetType() == "i") return (Double_t)datatype[0]->GetI(); 
   if (datatype[0]->GetType() == "d") return datatype[0]->GetD(); 
   cerr << "ERROR: DataBase: Illegal data type for table "<<table<<endl;
   cerr << "Must be an integer or double."<<endl;
   return 0;
};

string TaDataBase::GetString(const string& table) const {
// Return "string" from table "table".  This assumes the data
// are in a pair  "table  string" where table is unique and
// value the single string that belongs to it.
   static multimap<string, vector<dtype*> >::const_iterator dmap;
   dmap = database.lower_bound(TaString(table).ToLower());
   if (dmap == database.end() || 
       database.count(TaString(table).ToLower()) == 0) {
     cerr << "ERROR: DataBase: Unknown database table "<<table<<endl;
     return 0;
   }
   if (database.count(TaString(table).ToLower()) > 1) {
     cerr << "ERROR: DataBase: Multiply defined table "<<table<<endl;
     cerr << "Fix the database to have one instance."<<endl;
     return 0;
   }
   vector<dtype*> datatype = dmap->second;
   if (datatype[0]->GetType() == "s") return datatype[0]->GetS(); 
   cerr << "ERROR: DataBase: Illegal data type for table "<<table<<endl;
   cerr << "Must be a string."<<endl;
   return 0;
};

vector<TaString> 
TaDataBase::GetStringVect(const string& table) const {
   vector<TaString> result;
   result.clear();
   static multimap<string, vector<dtype*> >::const_iterator dmap;
   dmap = database.lower_bound(TaString(table).ToLower());
   if (dmap == database.end() || 
       database.count(TaString(table).ToLower()) == 0) {
     cerr << "ERROR: DataBase: Unknown database table "<<table<<endl;
     return result;
   }
   multimap<string, vector<dtype*> >::const_iterator lb =
          database.lower_bound(TaString(table).ToLower());
   multimap<string, vector<dtype*> >::const_iterator ub = 
          database.upper_bound(TaString(table).ToLower());
   for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
         dmap != ub; dmap++) {
     vector<dtype*> datav = dmap->second;
     for (vector<dtype*>::const_iterator idat = datav.begin();
          idat != datav.end(); idat++) {
       if ((*idat)->GetType() == "s") 
           result.push_back(TaString((*idat)->GetS()));
     }
   }
   return result;
};

vector<TaString> 
TaDataBase::GetEpicsKeys() const {
  vector<TaString> keys, result;
  result.clear();
  keys = GetStringVect("epics");
  for (vector<TaString>::const_iterator is = keys.begin();
       is != keys.end(); is++) {
    if ((*is).size() > 1) result.push_back(*is);
  }
  return result;
};


vector<Int_t> 
TaDataBase::GetValueVector(const string& table) const {
   vector<Int_t> result;
   result.clear();
   multimap<string, vector<dtype*> >::const_iterator lb =
          database.lower_bound(TaString(table).ToLower());
   multimap<string, vector<dtype*> >::const_iterator ub = 
          database.upper_bound(TaString(table).ToLower());
   for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
         dmap != ub; dmap++) {
     vector<dtype*> datav = dmap->second;
     for (vector<dtype*>::const_iterator idat = datav.begin();
          idat != datav.end(); idat++) {
       if ((*idat)->GetType() == "i") 
           result.push_back((*idat)->GetI());
     }
   }
   return result;
};

vector<Double_t> 
TaDataBase::GetValueDVector(const string& table) const 
{
  vector<Double_t> result;
  result.clear();
  multimap<string, vector<dtype*> >::const_iterator lb =
    database.lower_bound(TaString(table).ToLower());
  multimap<string, vector<dtype*> >::const_iterator ub = 
    database.upper_bound(TaString(table).ToLower());
  for (multimap<string, vector<dtype*> >::const_iterator dmap = lb;
       dmap != ub; dmap++) 
    {
      vector<dtype*> datav = dmap->second;
      for (vector<dtype*>::const_iterator idat = datav.begin();
	   idat != datav.end(); idat++) 
	{
	  if ((*idat)->GetType() == "d") 
	    result.push_back((*idat)->GetD());
	}
    }
  return result;
};

void TaDataBase::PutDacNoise(const Int_t& adc, const Int_t& chan, const Double_t& slope) {
// Put Dac noise parameters slope  for adc,chan 
  if (!didinit) {
      cerr << "DataBase::PutDacNoise ERROR: Database not initialized\n";
  }
  string table = "dacnoise";
  vector<dtype *> dvec;   dvec.clear();
  dtype *dat;
  dat = new dtype("s");  dat->Load("adc");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(adc);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("chan");  dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(chan);    dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("slope"); dvec.push_back(dat);
  dat = new dtype("d");  dat->Load(slope);   dvec.push_back(dat);
  didput = kTRUE;
  PutData(table, dvec);
};

void TaDataBase::PutAdcPed(const Int_t& adc, const Int_t& chan, const Double_t& ped) {
// Put Pedestals for adc, chan 
  if (!didinit) {
      cerr << "DataBase::PutAdcPed ERROR: Database not initialized\n";
  }
  string table = "ped";
  vector<dtype *> dvec;   dvec.clear();
  dtype *dat;
  dat = new dtype("s");  dat->Load("adc");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(adc);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("chan");  dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(chan);    dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("value"); dvec.push_back(dat);
  dat = new dtype("d");  dat->Load(ped);     dvec.push_back(dat);
  didput = kTRUE;
  PutData(table, dvec);
};

void TaDataBase::PutDacxNoise(const Int_t& adcx, const Int_t& chan, const Double_t& slope) {
// Put Dac noise parameters slope  for adcx,chan 
  if (!didinit) {
      cerr << "DataBase::PutDacxNoise ERROR: Database not initialized\n";
  }
  string table = "dacnoise";
  vector<dtype *> dvec;   dvec.clear();
  dtype *dat;
  dat = new dtype("s");  dat->Load("adcx");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(adcx);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("chan");  dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(chan);    dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("slope"); dvec.push_back(dat);
  dat = new dtype("d");  dat->Load(slope);   dvec.push_back(dat);
  didput = kTRUE;
  PutData(table, dvec);
};

void TaDataBase::PutAdcxPed(const Int_t& adcx, const Int_t& chan, const Double_t& ped) {
// Put Pedestals for adcx, chan 
  if (!didinit) {
      cerr << "DataBase::PutAdcxPed ERROR: Database not initialized\n";
  }
  string table = "ped";
  vector<dtype *> dvec;   dvec.clear();
  dtype *dat;
  dat = new dtype("s");  dat->Load("adcx");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(adcx);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("chan");  dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(chan);    dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("value"); dvec.push_back(dat);
  dat = new dtype("d");  dat->Load(ped);     dvec.push_back(dat);
  didput = kTRUE;
  PutData(table, dvec);
};

void TaDataBase::PutScalPed(const Int_t& scal, const Int_t& chan, const Double_t& ped) {
// Put Pedestals for scaler, chan 
  if (!didinit) {
      cerr << "DataBase::PutScalPed ERROR: Database not initialized\n";
  }
  string table = "ped";
  vector<dtype *> dvec;   dvec.clear();
  dtype *dat;
  dat = new dtype("s");  dat->Load("scaler"); dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(scal);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("chan");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(chan);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("value");  dvec.push_back(dat);
  dat = new dtype("d");  dat->Load(ped);      dvec.push_back(dat);
  didput = kTRUE;
  PutData(table, dvec);
};

void TaDataBase::PutVqwkPed(const Int_t& vqwk, const Int_t& chan, const Double_t& ped) {
// Put Pedestals for qweak adc (vqwk), chan 
  if (!didinit) {
      cerr << "DataBase::PutVqwkPed ERROR: Database not initialized\n";
  }
  string table = "ped";
  vector<dtype *> dvec;   dvec.clear();
  dtype *dat;
  dat = new dtype("s");  dat->Load("vqwk");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(vqwk);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("chan");   dvec.push_back(dat);
  dat = new dtype("i");  dat->Load(chan);     dvec.push_back(dat);
  dat = new dtype("s");  dat->Load("value");  dvec.push_back(dat);
  dat = new dtype("d");  dat->Load(ped);      dvec.push_back(dat);
  didput = kTRUE;
  PutData(table, dvec);
};

void TaDataBase::PutCutInt(const vector<Int_t> myevint) {
// Put a single bad event interval into the database.
// The user must call this for each cut interval.
// The vector 'myevint' is a prescribed order: 
//   (evlo, evhi, cut num, cut value)
    string table  = "evint";
    vector<dtype*> dvect;  dvect.clear();
    for (int i = 0; i < (long)myevint.size(); i++) {
       dtype *dat = new dtype("i");
       dat->Load(myevint[i]);
       dvect.push_back(dat);
    } 
    didput = kTRUE;
    PutData(table, dvect);
}

void TaDataBase::PutData(string table, vector<dtype *> dvect) {
// To put data into the database.
// PRIVATE method used by the public "Put" methods.
// WARNINGS: PutData() will REPLACE the table.  
// And, no, you do not want to set 'didput' here.
// Performance is poor but its ok because you will not call this often.
  multimap<string, vector<dtype*> >::iterator dmap = 
          database.find(FindTable(table));
  if (dmap == database.end()) return;
  if (dvect.size() == 0) return;
  multimap<string, vector<dtype*> > mapcopy = database;
  database.clear();
  database.insert(make_pair(table, dvect));
  for (multimap<string, vector<dtype*> >::iterator im = mapcopy.begin();
       im != mapcopy.end(); im++) {
       string sname = im->first;
       vector<dtype*> vdata = im->second;
       if (dbput[table] == kTRUE || table != sname) {
          database.insert(make_pair(sname, vdata));
       }
       else
	 {
	   // This table is being replaced, so must delete old pointers
	   for (vector<dtype* >::iterator id = vdata.begin(); 
		id != vdata.end(); 
		id++)
	     delete *id;
	 }
  }
  if (dbput[table] == kFALSE) dbput[table] = kTRUE;
}

void TaDataBase::InitDB() {
// Define the database structure.
// Table names correspond to first column in Ascii DB.
// Careful, an error here can lead to incomprehensible core dump.

  if (didinit) return;
  didinit = kTRUE;

  vector <dtype*> columns;
  columns.reserve(26);

  tables.clear();
  tables.push_back("run");           //   0
  tables.push_back("anatype");       //   1
  tables.push_back("maxevents");     //   2
  tables.push_back("lobeam");        //   3
  tables.push_back("burpcut");       //   4
  tables.push_back("dacnoise");      //   5
  tables.push_back("ped");           //   6
  tables.push_back("header");        //   7
  tables.push_back("datamap");       //   8
  tables.push_back("ncuts");         //   9
  tables.push_back("extlo");         //  10
  tables.push_back("exthi");         //  11
  tables.push_back("evint");         //  12
  tables.push_back("windelay");      //  13
  tables.push_back("oversamp");      //  14
  tables.push_back("pairtype");      //  15
  tables.push_back("feedback");      //  16 
  tables.push_back("IAparam");       //  17
  tables.push_back("PZTparam");      //  18
  tables.push_back("cutnames");      //  19
  tables.push_back("timestamp");     //  20
  tables.push_back("simtype");       //  21
  tables.push_back("blindstring");   //  22
  tables.push_back("blindparams");   //  23
  tables.push_back("randomheli");    //  24
  tables.push_back("curmon");        //  25
  tables.push_back("qpd1const");     //  26
  tables.push_back("calvar");        //  27
  tables.push_back("epics");         //  28
  tables.push_back("PZTQcoupling");  //  29
  tables.push_back("IAHallCparam");  //  30
  tables.push_back("compress");      //  31
  tables.push_back("lobeamc");       //  32
  tables.push_back("curmonc");       //  33
  tables.push_back("detwts");        //  34
  tables.push_back("blumiwts");      //  35
  tables.push_back("flumiwts");      //  36
  tables.push_back("satcut");        //  37
  tables.push_back("adcxdacburpcut");//  38
  tables.push_back("posmon");        //  39
  tables.push_back("posburp");       //  40
  tables.push_back("posmone");       //  41
  tables.push_back("posburpe");      //  42
  tables.push_back("cburpcut");      //  43
  tables.push_back("monsatcut");     //  44
  tables.push_back("PITAparam");     //  45
  tables.push_back("cav1const");     //  46
  tables.push_back("cav2const");     //  47
  tables.push_back("cav3const");     //  48
  tables.push_back("lina1const");    //  49
  tables.push_back("trees");         //  50
  tables.push_back("fillditheronly"); //  51

  pair<string, int> sipair;
  int k;

  for (int i = 0; i < (long)tables.size(); i++ ) { 
    sipair.first = tables[i];
    sipair.second = 0;
    dbinit.insert(sipair);
    columns.clear();
    if (i == 0) columns.push_back(new dtype("i"));  // run
    if (i == 1) columns.push_back(new dtype("s"));  // anatype
    if (i == 2) columns.push_back(new dtype("i"));  // maxevents
    if (i == 3) columns.push_back(new dtype("d"));  // lobeam
    if (i == 4) columns.push_back(new dtype("d"));  // burpcut
    if (i == 5) {  // dac noise
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("i"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("i"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 6) {  // pedestals
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("i"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("i"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 7) { // header
       for (int k = 0; k < 3; k++) columns.push_back(new dtype("s"));
    }
    if (i == 8) { // datamap
       for (k = 0; k < 3; k++) columns.push_back(new dtype("s"));
       for (k = 0; k < 2; k++) columns.push_back(new dtype("i"));
       for (k = 0; k < 12; k++) columns.push_back(new dtype("s"));
    }
    if (i == 9) columns.push_back(new dtype("i"));  // ncuts
    if (i == 10) {   // extlo
      for (k = 0; k < 40; k++) columns.push_back(new dtype("i"));
    }
    if (i == 11) {  // exthi
      for (k = 0; k < 40; k++) columns.push_back(new dtype("i"));
    }
    if (i == 12) {  // evint
      for (k = 0; k < 20; k++) columns.push_back(new dtype("i"));
    }
    if (i == 13) columns.push_back(new dtype("i"));  // windelay
    if (i == 14) columns.push_back(new dtype("i"));  // oversamp
    if (i == 15) columns.push_back(new dtype("s"));  // pairtype
    if (i == 16) {
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("i"));
      columns.push_back(new dtype("s"));
    }
    if (i == 17) {   // IA slope 
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 18) {   // PZT matrix 
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 19) {   // cutnames
      for (k = 0; k < 40; k++) columns.push_back(new dtype("s"));
    }
    if (i == 20) { 
      columns.push_back (new dtype ("s"));  // timestamp (date)
      columns.push_back (new dtype ("s"));  // timestamp (time)
    }
    if (i == 21) { // simtype
        columns.push_back (new dtype ("s"));  // first spec
    }
    if (i == 22)   // blindstring
      columns.push_back(new dtype("s"));
    if (i == 23) 
      {  // blindparams
	columns.push_back(new dtype("s"));
	columns.push_back(new dtype("d"));
	columns.push_back(new dtype("s"));
	columns.push_back(new dtype("d"));
	columns.push_back(new dtype("s"));
	columns.push_back(new dtype("d"));
      }
    if (i == 24)   // randomheli
      columns.push_back(new dtype("s"));
    if (i == 25)   // curmon
      columns.push_back(new dtype("s"));
    if (i == 26) {  // qpd1const
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
    }
    if (i == 27)    // calvar
      columns.push_back(new dtype("s"));
    if (i == 28) {  // EPICS
      for (int k=0; k < 20; k++) 
        columns.push_back(new dtype("s"));
    }
    if (i == 29) {   // PZT Charge Coupling slopes
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 30) {   // Hall-C IA slope 
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 31) columns.push_back(new dtype("i"));  // compress
    if (i == 32) columns.push_back(new dtype("d"));  // lobeamc
    if (i == 33)   // curmonc
      columns.push_back(new dtype("s"));
    if (i == 34)    // detwts
      for (k = 0; k < 4; k++) columns.push_back(new dtype("d"));
    if (i == 35)    // blumiwts
      for (k = 0; k < 8; k++) columns.push_back(new dtype("d"));
    if (i == 36)    // flumiwts
      for (k = 0; k < 2; k++) columns.push_back(new dtype("d"));
    if (i == 37) columns.push_back(new dtype("d"));  // satcut
    if (i == 38) columns.push_back(new dtype("d"));  // adcxdacburpcut
    if (i == 39) {   // position monitors for the posburp cut
      for (k = 0; k < 40; k++) columns.push_back(new dtype("s"));
    }
    if (i == 40) {   // thresholds for the posburp cut
      for (k = 0; k < 40; k++) columns.push_back(new dtype("d"));
    }
    if (i == 41) {   // position monitors for the posburpE cut
      for (k = 0; k < 40; k++) columns.push_back(new dtype("s"));
    }
    if (i == 42) {   // thresholds for the posburpE cut
      for (k = 0; k < 40; k++) columns.push_back(new dtype("d"));
    }
    if (i == 43) columns.push_back(new dtype("d")); // cburpcut
    if (i == 44) columns.push_back(new dtype("d"));  // monsatcut
    if (i == 45) {   // PITA slope 
       columns.push_back(new dtype("s"));
       columns.push_back(new dtype("d"));
    }
    if (i == 46) {  // cav1const
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
    }
    if (i == 47) {  // cav2const
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
    }
    if (i == 48) {  // cav3const
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));
    }
    if (i == 49) {  // lina1const
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d")); // number of pads (as a double)
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g1
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g2
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g3
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g4
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g5
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g6
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g7
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// g8
      columns.push_back(new dtype("s"));
      columns.push_back(new dtype("d"));// spc
    }
    if (i == 50) 
      {
	// trees
	for (k = 0; k < 10; k++) 
	  columns.push_back(new dtype("s"));
      }
    if (i == 51)    // fillditheronly
      columns.push_back(new dtype("s"));

    sipair.second = columns.size(); 
    colsize.insert(sipair);
    LoadTable(tables[i],columns);
  }
};

void TaDataBase::LoadTable(string table, vector<dtype*> columns) {
// Load the database for a 'table'
// The first call will set up the structure but load empty data.
// The 2nd call (loadflg==1) must overwrite the empty data of 1st call.
// Subsequent calls load data.
  if  ( !didinit ) {
    cerr << "DataBase:: ERROR: cannot LoadTable before InitDB"<<endl;
    return;
  }
  map<string, int>::iterator im = dbinit.find(table);
  if (im == dbinit.end()) return;
  int loadflg = im->second;
  if (loadflg == 1) {  
     multimap<string, vector<dtype*> >::iterator dmap = 
          database.find(table);
     if (dmap == database.end()) return;

    // delete old content before writing new
    vector<dtype *> ddata = dmap->second;
    for (vector<dtype* >::iterator id = ddata.begin(); id != ddata.end(); id++)
      delete *id;

     map<string, int>::iterator si = colsize.find(table);   
     if (si != colsize.end() && columns.size() > 0) {
       dtype *dt = columns[columns.size()-1];
       for (int k = 0; k < colsize[table]-(long)columns.size(); k++) 
	 columns.push_back(new dtype(dt->GetType()));
     }
     dmap->second = columns;
  } else {
     database.insert(make_pair(table, columns));
     dbput.insert(make_pair(table, kFALSE));
  }
  loadflg++;
  im->second = loadflg;
  if (dbinit["evint"] <= 1) {
       nbadev = 0;
  } else {
       nbadev = database.count("evint");
  }
//  PrintDataBase();   // DEBUG
};


string TaDataBase::FindTable(string table) {
// Return 'case insensitive' table name if table exists
  for (int i = 0; i < (long)tables.size(); i++) {
    if ( TaString(table).CmpNoCase(tables[i]) == 0 ) return tables[i];
  }
  return " ";
};

void 
TaDataBase::ToRoot() {
// Copy database to ROOT representation.  
  rootdb->Clear();
  rootdb->SetRunNum(GetRunNum());
  rootdb->SetAnaTDatime(AnaTDatime);
  char cval[40];
  string sline,sval;
  for (multimap<string, vector<dtype*> >::iterator im = database.begin();
    im != database.end(); im++) {
    string sname = im->first;
    vector<dtype*> vdata = im->second;
    sline = sname;
    for (vector<dtype*>::iterator iv = vdata.begin(); 
     iv != vdata.end(); iv++) {
       dtype *dtp = *iv;
       if (dtp->GetType() == "s") {
         sprintf(cval,"  %s  ",(dtp->GetS()).c_str());
       }
       if (dtp->GetType() == "i") {
         sprintf(cval,"  %d  ",dtp->GetI());
       }
       if (dtp->GetType() == "d") {
         sprintf(cval,"  %7.2f  ",(float)dtp->GetD());
       }
       sval = cval;
       sline += sval;
    }
    sline += " \n";
    rootdb->Put(sline.c_str());
  }
};

void TaDataBase::DataMapReStart() {
// To reset the datamap iterator.  Must be done before using
// NextDataMap() the first time.
  firstiter = kTRUE;
  InitDataMap();
  dmapiter = datamap.begin();
};

Bool_t TaDataBase::NextDataMap() {
  InitDataMap();
  if (firstiter) {
      firstiter = kFALSE;
      return kTRUE;
  } 
  dmapiter++;
  if (dmapiter != datamap.end()) return kTRUE;
  return kFALSE; 
};

string TaDataBase::GetDataMapName() const {
  if ( !initdm ) {
    cerr << "DataBase:: ERROR: must first InitDataMap before using it"<<endl;
    string nothing = "";
    return nothing;
  }
  map< string , TaKeyMap >::const_iterator dmiter; 
  dmiter = dmapiter;
  return dmiter->first;
};
  
string TaDataBase::GetDataMapType() const {
  if ( !initdm ) {
    cerr << "DataBase:: ERROR: must first InitDataMap before using it"<<endl;
    string nothing = "";
    return nothing;
  }
  map< string , TaKeyMap >::const_iterator dmiter; 
  dmiter = dmapiter;
  TaKeyMap keymap = dmiter->second;
  return keymap.GetType();
};
 
TaKeyMap TaDataBase::GetKeyMap(string devicename) const {
  map< string, TaKeyMap >::const_iterator mapiter = datamap.find(devicename);
  TaKeyMap nothing;  nothing.LoadType("unknown");
  if (mapiter == datamap.end()) return nothing;
  return mapiter->second;
};

void TaDataBase::InitDataMap() {
  static pair<string, vector<dtype*> > sdt;
  int devnum, chan, evb, crate, istart;
  string key, readout, sevb, scrate;
  if ( !didinit ) {
    cerr << "DataBase:: ERROR: Cannot init datamap without first init DB"<<endl;
    return;
  }
  if (initdm) return;  // already initialized
  datamap.clear();
  string table = "datamap";
  multimap<string, vector<dtype*> >::iterator lb =
          database.lower_bound(table);        
  multimap<string, vector<dtype*> >::iterator ub = 
          database.upper_bound(table);
  for (multimap<string, vector<dtype*> >::iterator dmap = lb;
         dmap != ub; dmap++) {
     vector<dtype*> datav = dmap->second;
     if ((long)datav.size() < 6) {
       cout << "ERROR: datamap string too short, skipping."<<endl; 
       continue;
     }     
     string long_devname = datav[1]->GetS();       
     string devname = StripRotate(long_devname);
     map<string, TaKeyMap >::iterator dm = datamap.find(devname);
     TaKeyMap keymap;
     if (dm != datamap.end()) keymap = dm->second;
     keymap.LoadType(datav[0]->GetS());
     keymap.SetRotate(RotateState(long_devname));
     readout = datav[2]->GetS();
     devnum  = datav[3]->GetI();
// evb, crate are potentially "keymap" strings.  If not, then we interpret
// them as integers.  See datamap rules in ./doc/DATABASE.TXT
     chan  = datav[4]->GetI(); 
     sevb   = datav[5]->GetS();  evb   = atoi(sevb.c_str());
     scrate = "";  crate = 0;
     if ((long)datav.size() > 6) {
       scrate = datav[6]->GetS();  crate = atoi(scrate.c_str());
     }
     istart = FindFirstKey(sevb, scrate);
     for (int k = istart; k < (long)datav.size(); k++) {
       if ( !datav[k]->IsLoaded() ) continue;
       key = datav[k]->GetS();
       keymap.LoadData( key, readout, devnum, chan + k - istart, 
           evb + k - istart, crate, istart-EARLIESTKEY);
     }
     pair<string, TaKeyMap > skm;
     skm.first = devname;  skm.second = keymap;
     pair<map<string, TaKeyMap>::iterator, bool> p = datamap.insert(skm);
     if ( !p.second ) {
           datamap.erase(dm);
           datamap.insert(skm);
     }
  }
  dmapiter = datamap.begin();
  initdm = kTRUE;
// Debug printouts 
// PrintDataBase();
// PrintDataMap();   
};

int
TaDataBase::FindFirstKey(const string& sevb, const string& scrate) {
// Find the location in datamap of the first key, according to the rules
// in ./doc/DATABASE.TXT
  int jlen, jtoi;
  jlen = strlen(sevb.c_str());
  jtoi = atoi(sevb.c_str());
  if ( (jlen > 1) && (jtoi == 0) ) {  
    return EARLIESTKEY;
  }
  jlen = strlen(scrate.c_str());
  jtoi = atoi(scrate.c_str());
  if ( (jlen > 1) && (jtoi == 0) ) {  
    return EARLIESTKEY+1;
  }
  return EARLIESTKEY+2;
}


void
TaDataBase::LoadCksum (const string filename)
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

string TaDataBase::StripRotate (const string long_devname) {
// Strip away the suffix "_r" or "_ur" from the device name
// Of course if anyone invented a device with these suffixes and
// didn't mean rotation, this would cause a problem.  Don't.

  string result = long_devname;
  int pos;
  pos = long_devname.find("_r",1);
  if (pos > 0) {
    result.assign(long_devname, 0, pos);
    return result;
  }
  pos = long_devname.find("_ur",1);
  if (pos > 0) {
    result.assign(long_devname, 0, pos);
    return result;
  }
  return result;
}


Int_t TaDataBase::RotateState(const string long_devname) {
// From the device name, recognize the suffix "_r" or "_ur"
// if it exists, and return an integer flag:
//      1  -->  device is rotated
//      0  -->  device is not rotated
//     -1  -->  unknown

  int result = -1;
  int pos;
  pos = long_devname.find("_r",1);
  if (pos > 0) result = 1;
  pos = long_devname.find("_ur",1);
  if (pos > 0) result = 0;
  return result;
}
