#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TFitResultPtr.h"
//#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TUnixSystem.h"
//#include "TPaveText.h"
//#include "TPaveStats.h"
//#include "TProfile.h"
#include "TF1.h"
//#include "TStyle.h"
//#include "TKey.h"
//#include "TStopwatch.h"
//#include <vector>
//#include "TGraph.h"
//#include "TMultiGraph.h"
//#include "TLegend.h"

class QwString: public TString 
{
 public:
  QwString();
  ~QwString(){};

public:
  void SetCommentSymbol(const std::string s) {
    fCommentSymbol = s;
  }
  std::string GetCommentSymbol() { return fCommentSymbol;};

  QwString PurifyFromComment(const std::string s);
  QwString RemoveWhiteSpaces();
  QwString Purify(const std::string s);

  TString TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id) ;

protected:
  std::string fCommentSymbol;
private:
  QwString PurifyFromDefault();
  
};

QwString::QwString()
{
  fCommentSymbol.clear();
};

QwString 
QwString::PurifyFromDefault()
{
  this -> ToLower();
  if (not (this->BeginsWith(fCommentSymbol)) ) {
    if( this->Contains(fCommentSymbol) ) {
      Int_t pos = this->First(fCommentSymbol.c_str()); 
      // pos doesn't need to be initialized by "0", because it always has "value" form "First"
      this->Replace(pos, this->Sizeof() - pos, "");
    }
  }
  else {
    this->Clear();
  }
  return *this;
};

QwString 
QwString::PurifyFromComment(const std::string s)
{
  fCommentSymbol = s;
  this->PurifyFromDefault();
  return *this;
  
};

QwString 
QwString::RemoveWhiteSpaces()
{
  this -> ReplaceAll(' ', '\0');
  return *this;
};


QwString 
QwString::Purify(const std::string s)
{
 
  this -> PurifyFromComment(s);
  if(not this->IsNull()) {
    this -> RemoveWhiteSpaces();
  }
  return *this;
};


TString 
QwString::TokenString(char* delim, Int_t interesting_token_num, Int_t interesting_token_id)
{
  Bool_t local_debug = false;
  TString name;  
  
  TObjArray* Strings = this->Tokenize(delim); // break line into tokens
  Int_t token_numbers = Strings->GetEntriesFast();

  if(token_numbers == interesting_token_num) {
    TIter iString(Strings);
    TObjString* os= NULL;
    Int_t token_ids = 0;
    while ((os=(TObjString*)iString())) {
      if(token_ids == interesting_token_id) {
	name = os->GetString();
	break;
      }
      token_ids++;
    }
    delete Strings;

    if(local_debug) {
      std::cout << " tokens #"
		<< token_numbers
		<< " " << *this
		<< " " << name
		<< std::endl;
    }
  }
  else {   
    name.Clear(); // return empty string
    if(local_debug) {
      std::cout << "Token number is not in the possible token numbers" << std::endl;
    }
  }
  return name;
}
	

class QwBeamMonitorDevice
{
public:
  QwBeamMonitorDevice();
  ~QwBeamMonitorDevice(){};

  Bool_t OpenMapFile(TString mapfilename);

  std::vector<TString > wdevices;
  std::vector<TString > bpms;
  std::vector<TString > bcms;
  std::vector<TString > halos;
  std::vector<TString > others;

  std::vector<TString > monitors;


  std::vector<TString> GetBcmsList() {return bcms;};
  std::vector<TString> GetBpmsList() {return bpms;};
  std::vector<TString> GetHalosList() {return halos;};
  std::vector<TString> GetOthersList() {return others;};
  std::vector<TString> GetMonitorsList() {return monitors;};
  
  void WantBpms(Bool_t in)   {
    if(not IsCustomizedMonitors()) fBpmsOnFlag = in;
    else std::cout << "BPMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
  };

  void WantBcms(Bool_t in)   {
    if(not IsCustomizedMonitors()) fBcmsOnFlag = in;
    else std::cout << "BCMs is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
  };
  void WantHalos(Bool_t in)  {
    if(not IsCustomizedMonitors()) fHalosOnFlag = in;
    else std::cout << "Halos is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
  };
  
  void WantOthers(Bool_t in) {
    if(not IsCustomizedMonitors()) fOthersOnFlag = in;
    else std::cout << "Others is not ready, because of the customized monitors. Please turn off customized monitor list first."<< std::endl;
  };

  void WantCustomizedMonitors(Bool_t in, TString filename);
  

  Bool_t IsBpms()   {return fBpmsOnFlag;};
  Bool_t IsBcms()   {return fBcmsOnFlag;};
  Bool_t IsHalos()  {return fHalosOnFlag;};
  Bool_t IsOthers() {return fOthersOnFlag;};
 
  Bool_t IsCustomizedMonitors() {return fMonitorsOnFlag;};

private:

  ifstream fMapFile; 
  ifstream fWantedDeviceFile;

  void  OpenWantedDeviceFile(TString filename);
  TString GetMapDir(TString mapfilename, Bool_t current_dir_flag);

  Bool_t fBpmsOnFlag;
  Bool_t fBcmsOnFlag;
  Bool_t fHalosOnFlag;
  Bool_t fOthersOnFlag;
  Bool_t fMonitorsOnFlag;
 

};


QwBeamMonitorDevice::QwBeamMonitorDevice()
{
  fBpmsOnFlag   = false;
  fBcmsOnFlag   = false;
  fHalosOnFlag  = false;
  fOthersOnFlag = false;
  fMonitorsOnFlag = false;

  fMapFile.clear(); 
  fWantedDeviceFile.clear();

  wdevices.clear();
  bpms.clear();
  bcms.clear();
  halos.clear();
  others.clear();
  monitors.clear();
};

void 
QwBeamMonitorDevice::WantCustomizedMonitors(Bool_t in, TString filename)
{
  fMonitorsOnFlag = in;
  // if one wants to use their own monitors,
  // turn off all monitors by default
  if(fMonitorsOnFlag) {
    if(IsBpms())   fBpmsOnFlag   = false;
    if(IsBcms())   fBcmsOnFlag   = false;
    if(IsHalos())  fHalosOnFlag  = false;
    if(IsOthers()) fOthersOnFlag = false;
    OpenWantedDeviceFile(filename);
  }
  else {
    if(IsBpms())   fBpmsOnFlag   = true;
    if(IsBcms())   fBcmsOnFlag   = true;
    if(IsHalos())  fHalosOnFlag  = true;
    if(IsOthers()) fOthersOnFlag = false;
  }
  
  return;
}
void
QwBeamMonitorDevice::OpenWantedDeviceFile(TString filename)
{
  Bool_t local_debug = false;
  if(local_debug) {
    std::cout << "QwBeamMonitorDevice::OpenWantedDeviceFile()"
	      << std::endl;
  }
 
  fWantedDeviceFile.clear();
  filename = GetMapDir("", true) + "beam_calib_data/" +filename;
  fWantedDeviceFile.open(filename);
  if(not fWantedDeviceFile.is_open()) {
    std::cout << "I cannot open " 
	      << filename  
	      << ". Use the default configuration."
	      << std::endl;
  }
  else {
    QwString line;
    //    QwString line; line.Clear();
    
    while (not fWantedDeviceFile.eof() ) {
      line.ReadLine(fWantedDeviceFile);   
      line.Purify("#");
      if(not line.IsNull()) {
	if(local_debug) { 
	  std::cout << line << std::endl;
	}
	wdevices.push_back(line);
	line.Clear();
      } // if(not line.IsNull()) {
    } //   while (not fWantedDeviceFile.eof() ) {
    line.Clear();
  }

  fWantedDeviceFile.close();

  return;
};

TString 
QwBeamMonitorDevice::GetMapDir(TString mapfilename, Bool_t current_dir_flag)
{
  TString get;
  if(current_dir_flag) {
    get = getenv("PWD");
    get += "/";
    if(get.IsNull()) {
      std::cout << "Please check your path "  
		<< std::endl;
      return get;
    }
    else {
      mapfilename = get + mapfilename;
      return mapfilename;
    }
  }
  else {
    get =  getenv("QWANALYSIS");
    get += "/";
    if(get.IsNull()) {
      std::cout << "Please check your QWANALYSIS path "  
		<< std::endl;
      return get;
    }
    else {
      mapfilename = get + "Parity/prminput/" + mapfilename;
      return mapfilename;
    }
  }
};


Bool_t
QwBeamMonitorDevice::OpenMapFile(TString mapfilename)
{


  Bool_t local_debug = false;

  if(local_debug) {
    std::cout << "QwBeamMonitorDevice::OpenMapFile " << mapfilename << std::endl;
  }

  fMapFile.clear();
  fMapFile.open(GetMapDir(mapfilename, false));
  
  if(not fMapFile.is_open()) {
    std::cout << "I cannot open your mapfile " 
	      << mapfilename  
	      << ". Please Check it again."
	      << std::endl;
    return false;
  }
  else {
    //    if(local_debug) {
    std::cout << mapfilename << " is opened." << std::endl;
    //    }

    char delimiters[] = ",";
    QwString line; 
    TString device_type; device_type.Clear();
    TString device_name; device_name.Clear();

    std::vector<TString>::iterator pd;

    while (not fMapFile.eof() ) {
      line.ReadLine(fMapFile);   
      line.Purify("!");
      if(not line.IsNull()) {
	if(local_debug) std::cout << line << std::endl;
	device_type = line.TokenString(delimiters, 5, 3);
	if (not device_type.IsNull()) {
	  device_name = line.TokenString(delimiters, 5, 4);
	  if(IsCustomizedMonitors()) {
	    for (pd = wdevices.begin(); pd!=wdevices.end(); pd++) {
	      if(device_name == *pd) {
		monitors.push_back(*pd);
	      }
	    }
	  }
	  else {
	    if(device_type == "bcm") {
	      if(IsBcms()) bcms.push_back(device_name);
	    }
	    else if (device_type == "bpmstripline") {
	      if(IsBpms()) bpms.push_back(device_name);
	    }
	    else if (device_type == "halomonitor") {
	      if(IsHalos()) halos.push_back(device_name);
	    }
	    else {
	      if(IsOthers()) others.push_back(device_name);
	    }
	  }
	} // if (not device_type.IsNull()) {
      } // if(not line.IsNull()) {
    } //   while (not mapfile.eof() ) {
  }
  fMapFile.close();
  return true;
}



// One day, suddenly, I realized
// why I need to read a "map" file 
// in order to select "wanted device (monitor) list".
// 
// It is a real life...... 
// Friday, August  6 15:25:07 EDT 2010, jhlee

//void
//FillTH1Histogram(TBranch *branch, TH1D *in)
//{
//  Int_t entries = 0;
//  Double_t entry = 0.0;
//
//  entries = branch->GetEntries();
//  branch-> SetAddress(&entry);
//  for (Int_t i=0;i<entries; i++) {
//    branch->GetEntry(i);
//    in->Fill(entry);
//  }
//  
//  return;
//}

TCanvas *canvas; 

Int_t
main(int argc, char **argv)
{
  TApplication theApp("App", &argc, argv);

  Int_t w = 1000;
  Int_t h = 800;

  canvas = new TCanvas("bcm_calib_test","BCM Calibration Test", w, h);  
  
  Bool_t local_debug = true;
  TString mapfilename       = "qweak_hallc_beamline.map";
  TString selected_filename = "bcms_only.dat";

  QwBeamMonitorDevice beam_monitors;
  beam_monitors.WantCustomizedMonitors(true, selected_filename);
  beam_monitors.WantBcms(false);
  beam_monitors.WantBpms(false);
  beam_monitors.WantHalos(false);
  beam_monitors.WantOthers(false);

  std::vector<TString> bcm_name_list;
  std::vector<TString> halo_name_list;
  std::vector<TString> bpm_name_list;
  std::vector<TString> other_name_list;

  std::vector<TString> wanted_bcm_list;
  std::vector<TString>::iterator pd;
  
  std::size_t bcm_size = 0;
  bcm_size = wanted_bcm_list.size();

  if(beam_monitors.OpenMapFile(mapfilename)) 
    { 
      bcm_name_list   = beam_monitors.GetBcmsList();
      halo_name_list  = beam_monitors.GetHalosList();
      wanted_bcm_list = beam_monitors.GetMonitorsList();
      bcm_name_list   = beam_monitors.GetBpmsList();

      if(local_debug) {
	std::cout << "Beam Monitors : bpm size " 
		  << beam_monitors.bpms.size()
		  << " bcm size "
		  << bcm_name_list.size()
		  << " halo size "
		  << beam_monitors.halos.size()
		  << " other size "
		  << beam_monitors.others.size()
		  << " wanted size "
		  << wanted_bcm_list.size()
		  << std::endl;

	for( pd = wanted_bcm_list.begin(); pd != wanted_bcm_list.end(); pd++ ) {
	  std::cout << "selected? " << *pd << std::endl;
	}


      }
      std::vector<TString>::iterator pd;
      if(beam_monitors.IsBcms()) {
	for( pd = bcm_name_list.begin(); pd != bcm_name_list.end(); pd++ ) {
	  std::cout << *pd << std::endl;
	}
      }
      if(beam_monitors.IsHalos()) {
	for( pd = halo_name_list.begin(); pd != halo_name_list.end(); pd++ ) {
	  std::cout << *pd << std::endl;
	}
      }
      if(beam_monitors.IsBpms()) {
	for( pd = bpm_name_list.begin(); pd != bpm_name_list.end(); pd++ ) {
	  std::cout << *pd << std::endl;
	}
      }

      TFile *file = new TFile("~/scratch/rootfiles/Qweak_5070.000.root");
      TTree *mps_tree = NULL;
      

      if (file->IsZombie())  {
	printf("Error opening file\n");  
	return EXIT_FAILURE;
      }
      else {
	mps_tree = (TTree*) file->Get("Mps_Tree");
	
	TBranch *branch_obj = NULL;
	// TLeaf *leaf_obj = NULL;
	//	Int_t canvas_size = 0;
	//	if (bcm_size%2) { // odd
	//	  canvas_size = ((Int_t) bcm_size + 1) / 2;
	//	}
	//	else { // even number
	//	  canvas_size = (Int_t bcm_size / 2
	//	}

	//	canvas -> Divide(4,3);
	canvas -> cd();
	Int_t cnt = 0;
	TString branch_name;
	Int_t leaf_number = 0;

	TH1D *sca_unser;
	TH1D *bcm_sca[10];
	TF1 *bcm_fit[10];
	


	branch_name = "qwk_sca_unser";
	mps_tree->Draw(Form("%s", branch_name.Data()), "qwk_sca_unser<320");
	sca_unser = (TH1D*) gPad -> GetPrimitive("htemp");
	sca_unser -> Fit("gaus", "M");
	TF1 *unser_fit = sca_unser -> GetFunction("gaus");

	
	Double_t chi_test = 0.0;
	Double_t mean[2] = {0.0};
	Double_t sigma[2] = {0.0};
	
	mean[0] = unser_fit -> GetParameter(1);
	mean[1] = unser_fit -> GetParError(1);
	sigma[0] = unser_fit -> GetParameter(2);
	sigma[1] = unser_fit -> GetParError(2);
	
	
	printf("mean      %10.5lf +- %8.5lf\n", mean[0], mean[1]);
	printf("sigma     %10.5lf +- %8.5lf\n", sigma[0], sigma[1]);
  
	
	canvas -> Update();

	canvas -> Clear();
	//	canvas -> Divide(2,2);
	
	
	canvas -> cd();
	
	mps_tree->Draw(Form("qwk_sca_bcm1:(%s-%lf)", branch_name.Data(),mean[0]),"","prof");

	bcm_sca[0] = (TH1D*) gPad -> GetPrimitive("htemp");
	bcm_sca[0] -> Fit("pol1", "M");
	bcm_fit[0] = bcm_sca[0] -> GetFunction("pol1");

	Double_t offset[2] = {0.0};
	Double_t slope[2] = {0.0};
	
	offset[0] = bcm_fit[0]-> GetParameter(0);
	offset[1] = bcm_fit[0] -> GetParError(0);
	slope[0] = bcm_fit[0] -> GetParameter(1);
	slope[1] = bcm_fit[0] -> GetParError(1);
	
	
	printf("offset    %10.5lf +- %8.5lf\n", offset[0], offset[1]);
	printf("slope     %10.5lf +- %8.5lf\n", slope[0], slope[1]);
  

	canvas->Update();


	canvas -> Clear();

	// canvas -> Divide(4,3);
	


	// for (pd=wanted_bcm_list.begin(); pd!=wanted_bcm_list.end(); pd++) {
	  
	//   branch_obj = mps_tree->FindBranch(*pd);
	  
	//   if (branch_obj) {
	    
	//     branch_name  = branch_obj->GetName();
	//     leaf_number = branch_obj->GetNleaves();
	    
	//     if (local_debug) {
	//       std::cout << "Found a Branch name " 
	// 		<< branch_name
	// 		<< " GetNleaves " 
	// 		<< leaf_number
	// 		<< std::endl;
	//     }
	    
	//     canvas -> cd(cnt+1);
	    
	//     if(leaf_number == 1) {
	//       mps_tree->Draw(Form("(%s-%lf):event_number", branch_name.Data(),mean[0]));
	//     }
	//     else {
	//       mps_tree->Draw(Form("(%s.hw_sum-%lf):event_number", branch_name.Data(),mean[0]));
	//     }
	//     cnt++;
	//     canvas -> Update();
	    
	//   }
	//   else {
	//     std::cout << " There is no branch with the name " 
	// 	      << *pd
	// 		<< std::endl;
	//   }
	// }
	 
	
	
      }
      
      // if(file->IsOpen())  {
      // 	file->Close();
      // 	delete file; file = NULL;
      // }

   
    }
  else
    return EXIT_FAILURE;

  theApp.Run();
  return 0;
}
