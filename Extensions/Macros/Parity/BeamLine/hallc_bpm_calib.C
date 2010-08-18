//
// Author : Jeong Han Lee
// Date   : Thursday, August 12 20:09:51 EDT 2010
//
//      


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <getopt.h>
#include <sstream>
#include <vector>
#include <algorithm>


#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TUnixSystem.h"
#include "TProfile.h"
#include "TLine.h"
#include "TBox.h"
#include "TCut.h"


// class QwString: public TString 
// {
//  public:
//   QwString();
//   ~QwString(){};

// public:
//   void SetCommentSymbol(const std::string s) {
//     fCommentSymbol = s;
//   }
//   std::string GetCommentSymbol() { return fCommentSymbol;};

//   QwString PurifyFromComment(const std::string s);
//   QwString RemoveWhiteSpaces();
//   QwString Purify(const std::string s);

//   TString TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id) ;

// protected:
//   std::string fCommentSymbol;
// private:
//   QwString PurifyFromDefault();
  
// };

// QwString::QwString()
// {
//   fCommentSymbol.clear();
// };

// QwString 
// QwString::PurifyFromDefault()
// {
//   this -> ToLower();
//   if (not (this->BeginsWith(fCommentSymbol)) ) {
//     if( this->Contains(fCommentSymbol) ) {
//       Int_t pos = this->First(fCommentSymbol.c_str()); 
//       // pos doesn't need to be initialized by "0", because it always has "value" form "First"
//       this->Replace(pos, this->Sizeof() - pos, "");
//     }
//   }
//   else {
//     this->Clear();
//   }
//   return *this;
// };

// QwString 
// QwString::PurifyFromComment(const std::string s)
// {
//   fCommentSymbol = s;
//   this->PurifyFromDefault();
//   return *this;
  
// };


// QwString 
// QwString::RemoveWhiteSpaces()
// {
//   this -> ReplaceAll(' ', '\0');
//   return *this;
// };


// QwString 
// QwString::Purify(const std::string s)
// {
 
//   this -> PurifyFromComment(s);
//   if(not this->IsNull()) {
//     this -> RemoveWhiteSpaces();
//   }
//   return *this;
// };


// TString 
// QwString::TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id)
// {
//   Bool_t local_debug = false;
//   TString name;  
  
//   TObjArray* Strings = this->Tokenize(delim); // break line into tokens
//   Int_t token_numbers = Strings->GetEntriesFast();

//   if(token_numbers == interesting_token_num) {
//     TIter iString(Strings);
//     TObjString* os= NULL;
//     Int_t token_ids = 0;
//     while ((os=(TObjString*)iString())) {
//       if(token_ids == interesting_token_id) {
// 	name = os->GetString();
// 	break;
//       }
//       token_ids++;
//     }
//     delete Strings;

//     if(local_debug) {
//       std::cout << " tokens #"
// 		<< token_numbers
// 		<< " " << *this
// 		<< " " << name
// 		<< std::endl;
//     }
//   }
//   else {   
//     name.Clear(); // return empty string
//     if(local_debug) {
//       std::cout << "Token number is not in the possible token numbers" << std::endl;
//     }
//   }
//   return name;
// }
	



// class QwBeamMonitorDevice
// {
// public:
//   QwBeamMonitorDevice();
//   ~QwBeamMonitorDevice(){};

//   Bool_t OpenMapFile(TString mapfilename);

//   std::vector<TString > wdevices;
//   std::vector<TString > bpms;
//   // std::vector<TString > trimbpms;
//   std::vector<TString > bcms;
//   std::vector<TString > halos;
//   std::vector<TString > others;
//   std::vector<TString > monitors;


//   std::vector<TString> GetBcmsList() {return bcms;};
//   std::vector<TString> GetBpmsList() {return bpms;};
//   std::vector<TString> GetTrimBpmsList();//{return trimbpms;};
//   std::vector<TString> GetHalosList() {return halos;};
//   std::vector<TString> GetOthersList() {return others;};
//   std::vector<TString> GetMonitorsList() {return monitors;};
  
//   void WantBpms(Bool_t in)   {
//     if(not IsCustomizedMonitors()) fBpmsOnFlag = in;
//     else std::cout << "BPMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };

//   // void WantTrimBpms(Bool_t in)   {
//   //   if(not IsCustomizedMonitors())  {
//   //     fTrimBpmsOnFlag = in;
//   //     this->WantBpms(in);
//   //   }
//   //   else std::cout << "Trim BPMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   // };

//   void WantBcms(Bool_t in)   {
//     if(not IsCustomizedMonitors()) fBcmsOnFlag = in;
//     else std::cout << "BCMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };
//   void WantHalos(Bool_t in)  {
//     if(not IsCustomizedMonitors()) fHalosOnFlag = in;
//     else std::cout << "Halos is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };
  
//   void WantOthers(Bool_t in) {
//     if(not IsCustomizedMonitors()) fOthersOnFlag = in;
//     else std::cout << "Others is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
//   };

//   void WantCustomizedMonitors(Bool_t in, TString filename);
//   void TrimBpmsAntenna();

//   Bool_t IsBpms()   {return fBpmsOnFlag;};
//   // Bool_t IsTrimBpms() {return fTrimBpmsOnFlag;};
//   Bool_t IsBcms()   {return fBcmsOnFlag;};
//   Bool_t IsHalos()  {return fHalosOnFlag;};
//   Bool_t IsOthers() {return fOthersOnFlag;};
 
//   Bool_t IsCustomizedMonitors() {return fMonitorsOnFlag;};

// private:

//   ifstream fMapFile; 
//   ifstream fWantedDeviceFile;

//   void  OpenWantedDeviceFile(TString filename);
//   TString GetMapDir(TString mapfilename, Bool_t current_dir_flag);

//   Bool_t fBpmsOnFlag;
//   // Bool_t fTrimBpmsOnFlag;
//   Bool_t fBcmsOnFlag;
//   Bool_t fHalosOnFlag;
//   Bool_t fOthersOnFlag;
//   Bool_t fMonitorsOnFlag;
 

// };


// QwBeamMonitorDevice::QwBeamMonitorDevice()
// {
//   fBpmsOnFlag   = false;
//   // fTrimBpmsOnFlag = false;
//   fBcmsOnFlag   = false;
//   fHalosOnFlag  = false;
//   fOthersOnFlag = false;
//   fMonitorsOnFlag = false;

//   fMapFile.clear(); 
//   fWantedDeviceFile.clear();

//   wdevices.clear();
//   bpms.clear();
//   //  trimbpms.clear();
//   bcms.clear();
//   halos.clear();
//   others.clear();
//   monitors.clear();
// };

// void 
// QwBeamMonitorDevice::WantCustomizedMonitors(Bool_t in, TString filename)
// {
//   fMonitorsOnFlag = in;
//   // if one wants to use their own monitors,
//   // turn off all monitors by default
//   if(fMonitorsOnFlag) {
//     if(IsBpms())   fBpmsOnFlag   = false;
//     // if(IsTrimBpms()) fTrimBpmsOnFlag = false;
//     if(IsBcms())   fBcmsOnFlag   = false;
//     if(IsHalos())  fHalosOnFlag  = false;
//     if(IsOthers()) fOthersOnFlag = false;
//     OpenWantedDeviceFile(filename);
//   }
//   else {
//     if(IsBpms())   fBpmsOnFlag   = true;
//     // if(IsTrimBpms()) fTrimBpmsOnFlag = true;
//     if(IsBcms())   fBcmsOnFlag   = true;
//     if(IsHalos())  fHalosOnFlag  = true;
//     if(IsOthers()) fOthersOnFlag = false;
//   }
  
//   return;
// }


// void
// QwBeamMonitorDevice::OpenWantedDeviceFile(TString filename)
// {
//   Bool_t local_debug = false;
//   if(local_debug) {
//     std::cout << "QwBeamMonitorDevice::OpenWantedDeviceFile()"
// 	      << std::endl;
//   }
 
//   fWantedDeviceFile.clear();
//   filename = GetMapDir("", true) + "beam_calib_data/" +filename;
//   fWantedDeviceFile.open(filename);
//   if(not fWantedDeviceFile.is_open()) {
//     std::cout << "I cannot open " 
// 	      << filename  
// 	      << ". Use the default configuration."
// 	      << std::endl;
//   }
//   else {
//     QwString line;
//     //    QwString line; line.Clear();
    
//     while (not fWantedDeviceFile.eof() ) {
//       line.ReadLine(fWantedDeviceFile);   
//       line.Purify("#");
//       if(not line.IsNull()) {
// 	if(local_debug) { 
// 	  std::cout << line << std::endl;
// 	}
// 	wdevices.push_back(line);
// 	line.Clear();
//       } // if(not line.IsNull()) {
//     } //   while (not fWantedDeviceFile.eof() ) {
//     line.Clear();
//   }

//   fWantedDeviceFile.close();

//   return;
// };

// TString 
// QwBeamMonitorDevice::GetMapDir(TString mapfilename, Bool_t current_dir_flag)
// {
//   TString get;
//   if(current_dir_flag) {
//     get = getenv("PWD");
//     get += "/";
//     if(get.IsNull()) {
//       std::cout << "Please check your path "  
// 		<< std::endl;
//       return get;
//     }
//     else {
//       mapfilename = get + mapfilename;
//       return mapfilename;
//     }
//   }
//   else {
//     get =  getenv("QWANALYSIS");
//     get += "/";
//     if(get.IsNull()) {
//       std::cout << "Please check your QWANALYSIS path "  
// 		<< std::endl;
//       return get;
//     }
//     else {
//       mapfilename = get + "Parity/prminput/" + mapfilename;
//       return mapfilename;
//     }
//   }
// };


// Bool_t
// QwBeamMonitorDevice::OpenMapFile(TString mapfilename)
// {


//   Bool_t local_debug = false;

//   if(local_debug) {
//     std::cout << "QwBeamMonitorDevice::OpenMapFile " << mapfilename << std::endl;
//   }

//   fMapFile.clear();
//   fMapFile.open(GetMapDir(mapfilename, false));
  
//   if(not fMapFile.is_open()) {
//     std::cout << "I cannot open your mapfile " 
// 	      << mapfilename  
// 	      << ". Please Check it again."
// 	      << std::endl;
//     return false;
//   }
//   else {
//     //    if(local_debug) {
//     std::cout << mapfilename << " is opened." << std::endl;
//     //    }

//     char delimiters[] = ",";
//     QwString line; 
//     TString device_type; device_type.Clear();
//     TString device_name; device_name.Clear();

//     std::vector<TString>::iterator pd;

//     while (not fMapFile.eof() ) {
//       line.ReadLine(fMapFile);   
//       line.Purify("!");
//       if(not line.IsNull()) {
// 	if(local_debug) std::cout << line << std::endl;
// 	device_type = line.TokenString(delimiters, 5, 3);
// 	if (not device_type.IsNull()) {
// 	  device_name = line.TokenString(delimiters, 5, 4);
// 	  if(IsCustomizedMonitors()) {
// 	    for (pd = wdevices.begin(); pd!=wdevices.end(); pd++) {
// 	      if(device_name == *pd) {
// 		monitors.push_back(*pd);
// 	      }
// 	    }
// 	  }
// 	  else {
// 	    if(device_type == "bcm") {
// 	      if(IsBcms()) bcms.push_back(device_name);
// 	    }
// 	    else if (device_type == "bpmstripline") {
// 	      if(IsBpms()) bpms.push_back(device_name);
// 	    }
// 	    else if (device_type == "halomonitor") {
// 	      if(IsHalos()) halos.push_back(device_name);
// 	    }
// 	    else {
// 	      if(IsOthers()) others.push_back(device_name);
// 	    }
// 	  }
// 	} // if (not device_type.IsNull()) {
//       } // if(not line.IsNull()) {
//     } //   while (not mapfile.eof() ) {
//   }
//   fMapFile.close();
//   return true;
// }

// std::vector<TString> 
// QwBeamMonitorDevice::GetTrimBpmsList()
// {
//   std::vector<TString > trimbpms;
//   std::vector<TString>::iterator it;
  
//   trimbpms.clear();

//   TString string_old;
//   TString string_new;
//   string_old.Clear();
//   string_new.Clear();

//   Int_t cnt = 0;
//   std::size_t i  = 0;
//   for(i=0; i< bpms.size(); i++ ){
//     string_new = bpms.at(i).Remove(8);
//     if (string_old not_eq string_new) trimbpms.push_back(string_new);
        
//     string_old = string_new;
//     cnt++;
//   //   if(not *it) trimbpms.push_back(temp);
//   }
//   return trimbpms;
// }



class BeamMonitor 
{

public:
  BeamMonitor();
  BeamMonitor(TString in);
  ~BeamMonitor(){};
  friend std::ostream& operator<<(std::ostream& stream, const BeamMonitor &device);

  void SetName(const TString in) {name = in;};
  void SetAliasName(const TString in) {alias_name = in;};
  void SetPed(const Double_t in) {offset[0] = in;};
  void SetPedErr(const Double_t in) {offset[1] = in;};
  void SetSlop(const Double_t in) {slope[0] = in;};
  void SetSlopErr(const Double_t in) {slope[1] = in;}
  void SetFitRange(const Double_t in[2]) {fit_range[0] = in[0]; fit_range[1] = in[1];};

  void SetReference() {reference_flag = true;};
  void SetFileStream() {filestream_flag = true;};

  
  TString GetName() {return name;};
  const char* GetCName() {return name.Data();};
  TString GetAliasName() {return alias_name;};
 const char* GetAliasCName() {return alias_name.Data();};
  Double_t GetPed() {return offset[0];};
  Double_t GetPedErr() {return offset[1];};
  Double_t GetSlop() {return slope[0];};
  Double_t GetSlopErr() {return slope[1];};
  Double_t GetFitRangeMin() {return fit_range[0];};
  Double_t GetFitRangeMax() {return fit_range[1];};

  Bool_t IsReference() { return reference_flag;};
  Bool_t IsFileStream() {return filestream_flag;};

private:
  TString name;
  TString alias_name;
  Double_t offset[2];
  Double_t slope[2];
  Double_t fit_range[2];
  Bool_t   reference_flag;
  Bool_t   filestream_flag;

};

BeamMonitor::BeamMonitor()
{
  name.Clear();
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope[0] = 0.0;
  slope[1] = 0.0;
  // mean[0] = 0.0;
  // mean[1] = 0.0;
  // rms[0] = 0.0;
  // rms[1] = 0.0;
  fit_range[0] = 0.0;
  fit_range[1] = 0.0;
  reference_flag = false;
  filestream_flag = false;
};


BeamMonitor::BeamMonitor(TString in)
{
  name = in;
  alias_name.Clear();
  offset[0] = 0.0;
  offset[1] = 0.0;
  slope[0] = 0.0;
  slope[1] = 0.0;
  // mean[0] = 0.0;
  // mean[1] = 0.0;
  // rms[0] = 0.0;
  // rms[1] = 0.0;
  fit_range[0] = 0.0;
  fit_range[1] = 0.0;
  reference_flag = false;
  filestream_flag = false;
};



std::ostream& operator<< (std::ostream& stream, const BeamMonitor &device)
{
  if(device.filestream_flag) {
    stream <<  std::setw(14) << device.name;
    stream << " ";
    stream <<  std::setw(10) << device.offset[0];
    stream << " " ;
    stream <<  std::setw(10) << device.offset[1];
    stream << " " ;
    stream <<  std::setw(10) << device.slope[0];
    stream << " " ;
    stream <<  std::setw(10) << device.slope[1];
    stream << "\n";
  }
  else {
    stream << " Name : " << device.name;
    stream << " Offset : "  << std::setw(4) << device.offset[0];
    stream << " +- "     << device.offset[1];
    
    if(device.reference_flag) {
      stream << " Fit Range [" << device.fit_range[0];
      stream << " ," << device.fit_range[1];
      stream << "]";
    }
    else {
      stream << " Slope :" << std::setw(4)  << device.slope[0];
      stream << " +- "     << device.slope[1];
    }
  }
  return stream;


  // stream << " Name : " << device.name;
  // stream << " Offset : "  << std::setw(4) << device.offset[0];
  // stream << " +- "     << device.offset[1];
  // stream << " Slope :" << std::setw(4)  << device.slope[0];
  // stream << " +- "     << device.slope[1];
  // stream << " Fit Range [" << device.fit_range[0];
  // stream << " ," << device.fit_range[1];
  // stream << "]";
    
  
  return stream;
};

      

void print_usage(FILE* stream, int exit_code);
Bool_t calibrate(BeamMonitor &bpm, BeamMonitor &bcm);

TH1D*
GetHisto(TTree *tree, const TString name, const TCut cut, Option_t* option = "")
{
  tree ->Draw(name, cut, option);
  TH1D* tmp;
  tmp = (TH1D*)  gPad -> GetPrimitive("htemp");
  if(not tmp) {
    return 0;
  }
  return tmp;
}


const char* program_name;
TTree *mps_tree = NULL;
TFile *file = NULL;

std::vector<BeamMonitor> hallc_bpms_list;
std::vector<BeamMonitor> hallc_bcms_list;

Int_t
main(int argc, char **argv)
{
 

  TApplication theApp("App", &argc, argv);


  char* run_number = NULL;
  TString bcm_ped_filename;


  Bool_t file_flag = false;
  Bool_t bcm_ped_file_flag = false;
  Bool_t fit_range_flag = false;
  Double_t fit_range[2] = {0.0};

  program_name = argv[0];

  int cc = 0; 

  while ( (cc= getopt(argc, argv, "r:e:")) != -1)
    switch (cc)
      {
      case 'r':
	{
	  file_flag = true;
	  run_number = optarg;
	}
	break;
      // case 'i':
      // 	{
      // 	  bcm_ped_file_flag = true;
      // 	  bcm_ped_filename = Form("%s",optarg);
      // 	}
      // 	break;
      case 'e':
	{
	  fit_range_flag = true;
	  char *c;
	  /*
	   * Allow the specification of alterations
	   * to the pty search range.  It is legal to
	   * specify only one, and not change the
	   * other from its default.
	   */
	  c = strchr(optarg, ':');
	  if (c) {
	    *c++ = '\0';
	    fit_range[1] = atof(c);
	  }
	  if (*optarg != '\0') {
	    fit_range[0] = atof(optarg);
	  }
	  if ((fit_range[0] > fit_range[1]) || (fit_range[0] < 0) || (fit_range[1] > 100.0) ) 
	    {
	      print_usage(stdout,0);
	    }
	}
	break;
      case '?':
	{
	  if (optopt == 'e') 
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'r')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  // else if (optopt == 'i')
	  //   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr, "Unknown option character `\\x%x'.\n",  optopt);
	  print_usage(stdout,0);
	  return 1;
	}
	break;
      default:
	abort () ;
      }
  
  if(not fit_range_flag) {
    fit_range[0] = 0.15;
    fit_range[1] = 0.85;
  }
  else {
    fit_range[0] = fit_range[0]/100.0;
    fit_range[1] = fit_range[1]/100.0;
    
  }
 
  if (not file_flag) {
    print_usage(stdout,0);
    exit (-1);
  }

  if (not bcm_ped_file_flag) {
    bcm_ped_filename = "hallc_bcm_pedestal_5070.txt";
  }

  //--// bpm_bcm_calibration returns 

  //--// qwk_3c07xp
  //--// qwk_3c07xm
  //--// qwk_3c07yp
  //--// qwk_3c07ym
  //--//  ...........

  //// but calibrate() needs qwk_3c07 only.



  // std::vector<TString> hallc_bpm_list;
  // std::vector<TString> hallc_bcm_list;

  // hallc_bpm_list.clear();
  // hallc_bcm_list.clear();

  // TString mapfilename       = "qweak_hallc_beamline.map";
  // TString get;
  // get = getenv("QWANALYSIS");
  // mapstring = get +"/Parity/prminput/" + mapfilename;
  // mapfile.open(mapstring);
  // while (!mapfile.eof()) {
  //   line.ReadToken(mapfile);
  //   if (line.Contains("!"))  line.ReadToDelim(mapfile);
  //   else if (line.Contains("VQWK")) {
  //     for(size_t i=0;i<2;i++)  	line.ReadToken(mapfile);
  //     line.ReadToken(mapfile);
  //     det_type=line;
  //     det_type.ToLower();
  //     line.ReadToken(mapfile);
  //     devicename=line;
  //     throw_away=devicename(devicename.Sizeof()-2,1);
  //     if(throw_away==",") devicename = devicename(0,devicename.Sizeof()-2);
  //     if (det_type=="bpmstripline,"){
  // 	TString subname=devicename(devicename.Sizeof()-3,2);
  // 	devicename.ToLower();
  // 	devicename= devicename(0,devicename.Sizeof()-3);
  // 	devicename= devicename + subname;
  // 	hallc_bpm_list.push_back(devicename);
  // 	counterdn++;
  //     }
  //     // else if(det_type=="bpmcavity,"){
  //     //   TString subname=devicename(devicename.Sizeof()-2,1);
  //     //   devicename.ToLower();
  //     // 	 devicename=(devicename.Sizeof()-2) + subname;
  //     // 	 devicelist1.push_back(devicename);
  //     // 	 counter++;
     
  //     // }
  //     else if (det_type=="bcm,"){
  // 	devicename.ToLower();
  // 	hallc_bcm_list.push_back(devicename);
  // 	counterbcm++;
  //     }
  //     else
  // 	std::cout<< " I don't know this device type" << std::endl;
  //   }
  // }
  // mapfile.close();


  // std::vector<TString>::iterator pd;
  // if(local_debug) {
  //   for( pd = hallc_bcm_list.begin(); pd != hallc_bcm_list.end(); pd++ ) {
  //     std::cout << *pd << std::endl;
  //   }
  //   for( pd = hallc_bpm_list.begin(); pd != hallc_bpm_list.end(); pd++ ) {
  //     std::cout << *pd << std::endl;
  //   }
  // }


  //--// I tried to trim xm,xp,ym,yp
  //--// 
  //--// But some devices have more than 4 antennas (xp,xm,yp,ym, axp, axm, ayp, aym)
  //--// And some devices have only other anthennas (axp,axm,ayp,aym)
  //--// Then I decided to add  bpms by hand.

  // Bool_t local_debug = true;
  // TString mapfilename       = "qweak_hallc_beamline.map";
  // TString selected_filename = "";
  

  // QwBeamMonitorDevice beam_monitors;
  // beam_monitors.WantCustomizedMonitors(false, selected_filename);
  // beam_monitors.WantBcms(false);
  // beam_monitors.WantBpms(true);
  // beam_monitors.WantHalos(false);
  // beam_monitors.WantOthers(false);

  // std::vector<TString> bcm_name_list;
  // std::vector<TString> halo_name_list;
  // std::vector<TString> bpm_name_list;
  // std::vector<TString> tbpm_name_list;
  // std::vector<TString> other_name_list;

  // std::vector<TString> wanted_bcm_list;
  // std::vector<TString>::iterator pd;

  // std::size_t bcm_size = 0;
  // bcm_size = wanted_bcm_list.size();

  // if(beam_monitors.OpenMapFile(mapfilename))
  //   {
  //     bcm_name_list   = beam_monitors.GetBcmsList();
  //     halo_name_list  = beam_monitors.GetHalosList();
  //     wanted_bcm_list = beam_monitors.GetMonitorsList();
  //     bpm_name_list   = beam_monitors.GetBpmsList();
  //     tbpm_name_list  = beam_monitors.GetTrimBpmsList();

  //     if(local_debug) {
  //         std::cout << "Beam Monitors : bpm size "
  //                   << beam_monitors.bpms.size()
  // 		    << " trim bpm size "
  //                   << tbpm_name_list.size()
  //                   << " bcm size "
  //                   << bcm_name_list.size()
  //                   << " halo size "
  //                   << beam_monitors.halos.size()
  //                   << " other size "
  //                   << beam_monitors.others.size()
  //                   << " wanted size "
  //                   << wanted_bcm_list.size()
  //                   << std::endl;

  //         for( pd = wanted_bcm_list.begin(); pd != wanted_bcm_list.end(); pd++) {
  //           std::cout << "selected? " << *pd << std::endl;
  //         }


  //     }  
      
  //     std::vector<TString>::iterator pd;
  //     if(local_debug) {
  //         if(beam_monitors.IsBcms()) {
  //           for( pd = bcm_name_list.begin(); pd != bcm_name_list.end(); pd++ ) {
  //             std::cout << *pd << std::endl;
  //           }
  //         }
  //         if(beam_monitors.IsHalos()) {
  //           for( pd = halo_name_list.begin(); pd != halo_name_list.end(); pd++) {
  //             std::cout << *pd << std::endl;
  //           }
  //         }
  //         if(beam_monitors.IsBpms()) {
  //           for( pd = bpm_name_list.begin(); pd != bpm_name_list.end(); pd++ ) {
  //             std::cout << *pd << std::endl;
  //           }
  // 	    std::cout << "---------------- " << std::endl;
  // 	    for( pd = tbpm_name_list.begin(); pd != tbpm_name_list.end(); pd++ ) {
  //             std::cout << *pd << std::endl;
  //           }
  //         }
  //     }
    
  //   }

  



  hallc_bpms_list.push_back(BeamMonitor("qwk_3c07"));// begining of arc
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c08"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c11"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c12"));// highest dispersion
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c14"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c16"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c17"));// end of arc (green wall)
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c18"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c19"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c20"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3c21"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3h02"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3h08"));
  hallc_bpms_list.push_back(BeamMonitor("qwk_3h09"));


  TString filename = Form("Qweak_%s.000.root", run_number);
  file = new TFile(Form("~/scratch/rootfiles/%s", filename.Data()));

  if (file->IsZombie()) {
    std::cout << "Error opening file " << filename << std::endl;
    filename = Form("Qweak_%s.root", run_number);
    std::cout << "Try to open " << filename << std::endl;
    file = new TFile(Form("~/scratch/rootfiles/%s", filename.Data()));
    if (file->IsZombie()) {
      return EXIT_FAILURE;
    }
  }

  
  ifstream in;
  in.open(bcm_ped_filename.Data());
  TString name;
  Double_t offset[2] = {0.0};
  Double_t slope[2] = {0.0};
  Int_t cnt = 0;

  while(in.good()) {

    in >> name >> offset[0] >> offset[1] >> slope[0] >> slope[1];
    if(not in.good()) break;
    else {
      BeamMonitor temp; 
      temp.SetName(name);
      temp.SetPed(offset[0]);
      temp.SetPedErr(offset[1]);
      temp.SetSlop(slope[0]);
      temp.SetSlopErr(slope[1]);
      hallc_bcms_list.push_back(temp);
    }
      
    // std::cout << name << " " 
    // 	      << offset[0] << " " << offset[1] << " "
    // 	      << slope[0] << " " << slope[1] << " "
    // 	      << std::endl;
  }
  in.close();
  mps_tree = (TTree*) file->Get("Mps_Tree");
  std::size_t bcm_id =0;
  TH1D *tmp;
  Double_t tmp_max = 0.0;

  for(bcm_id = 0; bcm_id < hallc_bcms_list.size(); bcm_id ++ ) {
    mps_tree->SetAlias(
     		       Form("qwbcm%d", bcm_id), 
		       Form("%s.hw_sum_raw/%s.num_samples - %lf", 
			    hallc_bcms_list.at(bcm_id).GetName().Data(), hallc_bcms_list.at(bcm_id).GetName().Data(),
			    hallc_bcms_list.at(bcm_id).GetPed())
		       );
    mps_tree->Draw(Form("qwbcm%d>>tmp", bcm_id), 
		   Form("%s.num_samples>0", hallc_bcms_list.at(bcm_id).GetName().Data()),
		   "goff");
    tmp = (TH1D*)gDirectory->Get("tmp");
    if(not tmp) {
      std::cout << "Please check " << Form("qwbcm%d>>tmp", bcm_id)
		<< std::endl;
       theApp.Run();

    }
    tmp_max = tmp -> GetXaxis() -> GetXmax();
    //   std::cout << tmp_max << std::endl;
    fit_range[0] *= tmp_max;
    fit_range[1] *= tmp_max;

    hallc_bcms_list.at(bcm_id).SetAliasName(Form("qwbcm%d", bcm_id)); 
    hallc_bcms_list.at(bcm_id).SetFitRange(fit_range);
  }

  

  std::size_t i = 0; 
  cnt = 0;
  std::cout << "how many bpms ? " << hallc_bpms_list.size() << std::endl;
  for (i=0;i < hallc_bpms_list.size(); i++ ) {
    std::cout << cnt << ": onto calibrating " << hallc_bpms_list.at(i).GetName() << std::endl;
    cnt++;
    Bool_t check = false;
    check = calibrate(hallc_bpms_list.at(i), hallc_bcms_list.at(0)) ; // qwk_bcm1
    //    if(not check) theApp.Run();
  }

  theApp.Run();
  return 0;
}


Bool_t
calibrate(BeamMonitor &bpm, BeamMonitor &bcm)
{

  std::cout << bcm << std::endl;
  
  TString devname;
  devname = bpm.GetName();

  TString antenna[4]={"XP","XM","YP","YM"};
 
  TF1 *fit[4];
  TH1D *hprof[4];

 
  TString plotcommand[4];
  TString bpm_name[4];
  TString bpm_samples[4];

  Int_t w = 1100;
  Int_t h = 600;
  TCanvas *Canvas = new TCanvas(devname.Data() , devname.Data(), w, h);  
  
  Canvas->Clear();
  Canvas->Divide(2,2);
  
  TString bcm_name;
  bcm_name = bcm.GetAliasName();
  Double_t bcm_fit_range[2] = {0.0};
  bcm_fit_range[0] = bcm.GetFitRangeMin();
  bcm_fit_range[1] = bcm.GetFitRangeMax();

  TCut scut = "";

  Int_t i = 0;
  for(i=0;i<4;i++) {
    Canvas->cd(i+1);
    bpm_name[i]    = devname + antenna[i];
    bpm_samples[i] = bpm_name[i] + ".num_samples";
    plotcommand[i] = bpm_name[i]+".hw_sum_raw/" + bpm_samples[i] + ":" + bcm_name;

    hprof[i] = GetHisto(mps_tree, plotcommand[i], scut, "profs");

    if(not hprof[i]) {
      std::cout << "Please check " << plotcommand[i].Data()
		<< std::endl;

      Canvas ->Close();
      delete Canvas; Canvas = NULL;
      return false;
    }
    
    hprof[i] -> Fit("pol1", "E F R Q", "", bcm_fit_range[0],  bcm_fit_range[1]);
    fit[i] = hprof[i]  -> GetFunction("pol1");
    if(fit[i]) {
      bpm.SetPed(fit[i]->GetParameter(0));
      bpm.SetPedErr(fit[i]->GetParError(0));
      bpm.SetSlop(fit[i]->GetParameter(1));
      bpm.SetSlopErr(fit[i]->GetParError(1));
    }
    else {
      bpm.SetPed(-1);
      bpm.SetPedErr(-1);
      bpm.SetSlop(-1);
      bpm.SetSlopErr(-1);
    }
    gPad->Update();
    std::cout << bpm << std::endl;

  }
  Canvas -> Modified();
  Canvas -> Update();
  return true;
}





void 
print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "\n");
  fprintf (stream, "This program is used to do BPM calibrations.\n");
  fprintf (stream, "Usage: %s -r {run number} -i {n} -e {a:b} \n", program_name);
  // fprintf (stream, 
  //	   " -r run number.\n"
  //	   " -i unser beam off offset (n*1e3) \n"
  //	   " -e fit percent range (default [0.01*a*unser_current_range, 0.01*b*unser_current_range]\n"
  //	   );
  fprintf (stream, "\n");
  exit (exit_code);
}


