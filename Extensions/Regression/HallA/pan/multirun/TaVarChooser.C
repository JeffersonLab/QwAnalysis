//////////////////////////////////////////////////////////////////////
//
// TaVarChooser.C
//
//  Class that reads in a file, and provides utilities to determine
//   which variables are to be included in the pair-summary 
//   rootfiles
//
//////////////////////////////////////////////////////////////////////

#include <multirun/TaVarChooser.h>
#include <src/TaString.hh>
#include <string>
#include <iostream>
#include <fstream>

TaVarChooser::TaVarChooser(TString filename) : 
  fVarFile(0),
  isRead(kFALSE),
  opt_batteries(0),
  opt_ditbpms(0),
  opt_bpms(0),
  opt_hallbpms_sat(0),
  opt_bcms(0),
  opt_flumi(0),
  opt_blumi(0),
  opt_he4detectors(0),
  opt_lh2detectors(0),
  opt_bmw_words(0)
{

//   cout << "TaVarChooser():  Reading file: " << filename 
//        << endl;

  fVarFile = new ifstream(filename);
  if ( ! (*fVarFile) ) {
    cerr << "TaVarChooser::Load WARNING: variable file " << filename
	 << " does not exist." << endl;
    isRead = kFALSE;
  } else {
    string comment = "#";
    vector<string> strvect;
    TaString sinput,sline;
    while (getline(*fVarFile,sinput)) {
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
    isRead = kTRUE;
  }
}


void TaVarChooser::ParseLine(vector <string> line) 
{

  TaString devices(line[0]);
  UInt_t option(atoi(line[1].c_str()));
  if(devices.ToLower() == "batteries") {
    opt_batteries.clear();
    for(UInt_t i=1;i<line.size();i++) {
      opt_batteries.push_back(line[i].c_str());
    }
  }
  if(devices.ToLower() == "ditbpms") {
    // Options here, are which BPMs to include for dithering
    opt_ditbpms.clear();
    for(UInt_t i=1;i<line.size();i++) {
      opt_ditbpms.push_back(line[i].c_str());
    }
  }
  if(devices.ToLower() == "bpms") {
    // Options here, are which other BPMs to include
    opt_bpms.clear();
    for(UInt_t i=1;i<line.size();i++) {
      opt_bpms.push_back(line[i].c_str());
    }
  }
  if(devices.ToLower() == "hallbpmsat") {
    opt_hallbpms_sat = option;
  }
  if(devices.ToLower() == "bcms") {
    opt_bcms.clear();
    // Options here, are which BCMs to include..
    for(UInt_t i=1;i<line.size();i++) {
      opt_bcms.push_back(line[i].c_str());
    }
  }
  if(devices.ToLower() == "flumi") {
    opt_flumi = option;
  }
  if(devices.ToLower() == "blumi") {
    opt_blumi = option;
  }
  if(devices.ToLower() == "he4detectors") {
    opt_he4detectors = option;
  }
  if(devices.ToLower() == "lh2detectors") {
    opt_lh2detectors = option;
  }
  if(devices.ToLower() == "bmw") {
    opt_bmw_words = option;
  }

}

void TaVarChooser::Print() 
{

  cout << "TaVarChooser::Print()" << endl;
  cout << "Will include the following variables into the summary rootfile:"
       << endl;

  if(GetBatteries().size()) {
    cout << "\tBatteries: " << endl << "\t\t";
    for(UInt_t i=0;i<GetBatteries().size();i++) {
      cout << GetBatteries()[i] << " ";
    }
  }
  if(GetDitBPMs().size()) {
    cout << "\tBPMs used for dithering: " << endl << "\t\t";
    for(UInt_t i=0;i<GetDitBPMs().size();i++) 
      cout << GetDitBPMs()[i] << " ";
    cout << endl;
  }
  if(GetBPMs().size()) {
    cout << "\tOther (not used for dithering) BPMs: " << endl << "\t\t";
    for(UInt_t i=0;i<GetBPMs().size();i++) 
      cout << GetBPMs()[i] << " ";
    cout << endl;
  }
  if(GetHallBPMSat()) {
    cout << "\tAll (relevant) BPM maximums" << endl;
  }
  if(GetBCMs().size()) {
    cout << "\tBCMs: " << endl << "\t\t";
    for(UInt_t i=0;i<GetBCMs().size();i++) 
      cout << GetBCMs()[i] << " ";
    cout << endl;
  }
  if(GetFLumi()) {
    cout << "\tAll FLumis" << endl;
  }
  if(GetBLumi()) {
    cout << "\tAll BLumis" << endl;
  }
  if(GetHe4Detectors()) {
    cout << "\tHelium-4 Detectors" << endl;
  }
  if(GetLH2Detectors()) {
    cout << "\tLH2 Detectors" << endl;
  }
  if(GetBMW()) {
    cout << "\tBMW words" << endl;
  }
}
