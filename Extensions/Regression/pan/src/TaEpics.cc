////////////////////////////////////////////////////////////////////
//
//   TaEpics
//   Hall A EPICS data.
//
//   EPICS data come in two forms
//     (tag, value)   e.g. (IPM1H04B.XPOS,  0.204)
//   and
//     (tag, value, units)  e.g. (HELG0TSETTLEs, 500, usec)
//
//   All data are received as characters and are parsed.
//   'tags' remain characters, 'values' are either character 
//   or double, and 'units' are characters.
//   Data are stored in an STL map and retrievable by
//   'tag' (e.g. IPM1H04B.XPOS) and by proximity to
//   a physics event number (closest one is picked).
//
//   author  Robert Michaels (rom@jlab.org)
//
/////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TaEpics.hh"
#include "VaEvent.hh"
#include "TaDataBase.hh"
#include "TaString.hh"
#include "TMath.h"
#include <iostream>

using namespace std;

TaEpics::TaEpics()
{
  fTreeVal = 0;
}

TaEpics::~TaEpics()
{ 
  if (fTreeVal) delete fTreeVal;
}

void TaEpics::Print() {
  cout << "\n\n====================== \n";
  cout << "Print of Epics Data : "<<endl;
  Int_t j = 0;
  for (map<string, vector<EpicsChan> >::iterator pm =
	 fEpicsData.begin(); pm != fEpicsData.end(); pm++) {
    vector<EpicsChan> vepics = pm->second;
    string tag = pm->first;
    cout << "\n\nEpics Var #" << j++;
    cout << "   Var Name =  '"<<tag<<"'"<<endl;
    cout << "Size of epics vector "<<vepics.size();
    for (UInt_t k=0; k<vepics.size(); k++) {
      cout << "\n Tag = "<<vepics[k].GetTag();
      cout << "   Evnum = "<<vepics[k].GetEvNum();
      cout << "   Date = "<<vepics[k].GetDate();
      cout << "   Timestamp = "<<vepics[k].GetTimeStamp();
      cout << "   Data = "<<vepics[k].GetData();
      cout << "   String = "<<vepics[k].GetString();
      cout << "   Units = "<<vepics[k].GetUnits();
    }
    cout << endl;
  }
}

Bool_t TaEpics::Init(const TaDataBase& db)
{
// Initialize the list of EPICS data to put
// into the output trees.  Returns kTRUE if
// there is a valid list.

  fTreeKey = db.GetEpicsKeys();
  if (fTreeKey.size() == 0) return kFALSE;
// The last two are always timestamp and evnum
  fTreeKey.push_back("epics_timestamp");
  fTreeKey.push_back("epics_evnum");
  if (fTreeKey.size() < fgINSANE_MAX) { 
    fTreeVal = new Double_t[fTreeKey.size()];
  } else {
    fTreeKey.clear();
    cout << "TaEpics::WARN: Too many EPICS variables.";
    cout << "Something wrong; hence no EPICs data."<<endl;
    return kFALSE;
  } 
  return kTRUE;
}


Bool_t TaEpics::IsLoaded(const char* tag) const
{
  const vector<EpicsChan> ep = GetChan(tag);
  if (ep.size() == 0) return kFALSE;
  return kTRUE;
}

Double_t TaEpics::GetData (const char* tag, int event) const
{
  const vector<EpicsChan> ep = GetChan(tag);
  Int_t k = FindEvent(ep, event);
// If this happens, you probably mistyped "tag"
  if ( k < 0) return -1e32;  // Invalid data (undefined).
  return ep[k].GetData();
}  

string TaEpics::GetString (const char* tag, int event) const
{
  const vector<EpicsChan> ep = GetChan(tag);
  Int_t k = FindEvent(ep, event);
  if ( k < 0) return "";
  return ep[k].GetString();
}  

Double_t TaEpics::GetTimeStamp(const char* tag, int event) const
{
  const vector<EpicsChan> ep = GetChan(tag);
  Int_t k = FindEvent(ep, event);
  if ( k < 0) return -1e32;
  return ep[k].GetTimeStamp();
}

vector<EpicsChan> TaEpics::GetChan(const char *tag) const
{
  // Return the vector of Epics data for 'tag' 
  // where 'tag' is the name of the Epics variable.
  vector<EpicsChan> ep;
  ep.clear();
  map< string, vector<EpicsChan> >::const_iterator pm = 
           fEpicsData.find(string(tag));
  if (pm != fEpicsData.end()) ep = pm->second;
  return ep;
}


Int_t TaEpics::FindEvent(const vector<EpicsChan> ep, int event) const
{
  // Return the index in the vector of Epics data 
  // nearest in event number to event 'event'.
  if (ep.size() == 0) return -1;
  int myidx = ep.size()-1;
  if (event == 0) return myidx;  // return last event 
  double min = 9999999;
  for (UInt_t k = 0; k < ep.size(); k++) {
    double diff = event - ep[k].GetEvNum();
    if (diff < 0) diff = -1*diff;
    if (diff < min) {
      min = diff;
      myidx = k;
    }
  }
  return myidx;
}

void TaEpics::AddToTree(TTree &tree) {
// Add data to TTree 'tree'. 
// However, note this only happens if the
// Init(TaDatabase&) returned a nonzero length
// of EPICS keys.
  TaString tinfo;
  Int_t kNbout = 4000;
  if (fTreeKey.size() > 0) {
    for (UInt_t i = 0; i < fTreeKey.size(); i++) {
      TaString epicsbr = CleanEpicsName(fTreeKey[i]);
      tinfo = epicsbr + "/D";
      tree.Branch(epicsbr.c_str(), &fTreeVal[i], 
        tinfo.c_str(), kNbout);
    }
  }
}
        
int TaEpics::Process(const VaEvent& event) {
// process an event.
  static const Int_t MAX=5000;
  int buffer[MAX];
  int evlen = event.GetEvLength();
  if (evlen > MAX) {
    cout << "EPICS event too large.  Truncating."<<endl;
    evlen = MAX;
  }
  memset(buffer,0,MAX*sizeof(int));
  for (int i = 0; i < evlen; i++) 
      buffer[i] = event.GetRawData(i);
  LoadData(buffer, event.GetLastPhyEv());
  if (fTreeKey.size() != 0) {
    for(vector<TaString>::size_type i=0; 
      i<fTreeKey.size()-2; i++) {
       fTreeVal[i] = GetData(fTreeKey[i].c_str());
       // Load timestamp from a valid EPICS variable.
       if (fTreeVal[i] != -1e32) {  // valid.
         fTreeVal[fTreeKey.size()-2] =
              GetTimeStamp(fTreeKey[i].c_str());
       }
    }
    fTreeVal[fTreeKey.size()-1] = event.GetLastPhyEv();  
  }
  return 0;
}

int TaEpics::LoadData(const int* evbuffer, int evnum)
{ // load data from the event buffer 'evbuffer' 
  // for event nearest 'evnum'.
     static const size_t DEBUGL = 0, MAX  = 5000, MAXEPV = 100;
     char *line, *date;
     const char* cbuff = (const char*)evbuffer;
     char wtag[MAXEPV+1],wval[MAXEPV+1],sunit[MAXEPV+1];
     float dval;
     static char fmt[16];
     static bool first = true;
     if( first ) 
         { sprintf(fmt,"%%%ds %%n",MAXEPV); first = false; }
     size_t len = sizeof(int)*(evbuffer[0]+1);  
     size_t nlen = TMath::Min(len,MAX);
     // Nothing to do?
     if( nlen<16 ) return 0;
     // Set up buffer for parsing
     size_t dlen = nlen-16;
     char* buf = new char[ dlen+1 ];
     strncpy( buf, cbuff+16, dlen );
     buf[dlen] = 0;
     
     // Extract date time stamp
     date = strtok(buf,"\n");
     if(DEBUGL) cout << "Timestamp: " << date <<endl;
     // Extract EPICS tags, values, and units.
     while( (line = strtok(0,"\n")) ) {
       // Here we parse the line
       if(DEBUGL) cout << "epics line : "<<line<<endl;
       wtag[0] = 0; wval[0] = 0;
       size_t n, m, slen = strlen(line);
       if( sscanf(line,fmt,wtag,&n) < 1 ) continue;
       // Get the value string _including_ spaces and 
       // excluding the trailing newline
       m = slen-n;
       if( m > 0 ) { 
	 m = TMath::Min(m,MAXEPV); 
	 strncpy(wval,line+n,m); wval[m] = 0; 
       }
       if (DEBUGL) cout << "wtag "<<wtag<<"   wval "<<wval<<endl;
       m = slen-n;
       if( m>0 ) { 
	 m = TMath::Min(m,MAXEPV); 
	 strncpy(wval,line+n,m); wval[m] = 0; 
       }
       // If value is of the form (int num, char* units) parse it.
       dval = 0;  sunit[0] = 0;  
       sscanf(wval,"%f  %s",&dval,sunit);
       if (DEBUGL) cout << "dval "<<dval<<"   sunit "<<sunit<<endl;
       // Add tag/value/units to the EPICS data.
       vector<EpicsChan> vepics;
       vepics.clear();
       map<string, vector<EpicsChan> >::iterator pm =
  	   fEpicsData.find(string(wtag));
       Bool_t isnew = kTRUE;
       if (pm != fEpicsData.end()) {
	 vepics = pm->second;
         isnew = kFALSE;
       } 
       EpicsChan ec;
       ec.Load(wtag, date, evnum, wval, sunit, dval);
       vepics.push_back(ec);
       if (isnew == kTRUE) {
         fEpicsData.insert(epVal(string(wtag), vepics));
       } else {  
         fEpicsData[string(wtag)] = vepics;
       }
     }
     if( DEBUGL==3 ) Print();
     delete [] buf;
     return 1;
}

//_____________________________________________________________________________
TaString TaEpics::CleanEpicsName(TaString& input) const
{
// To clean up EPICS variable names that contain
// bad characters like ":" and arithmetic operations
// that confuse TTree::Draw().
// Replace all 'badchar' with 'goodchar'

  static const char badchar[]=":+-*/=";
  static const string goodchar = "_";
  int numbad = sizeof(badchar)/sizeof(char) - 1;

  TaString output = input;

  for (int i = 0; i < numbad; i++) {
     string sbad(&badchar[i]);
     sbad.erase(1,sbad.size());
     string::size_type pos = input.find(sbad,0);
     while (pos != string::npos) {
       output.replace(pos,1,goodchar);
       pos = input.find(sbad,pos+1);
     }
  }
  return output;
}


ClassImp(TaEpics)
