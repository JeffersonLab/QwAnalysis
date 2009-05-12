///  This class exists to encapsulate several helper functions to aid in
///  describing histograms.
///  There will be a global copy defined within the analysis framework.

#ifndef __QWHISTOGRAMHELPER__
#define __QWHISTOGRAMHELPER__

#include <string>
#include <vector>
#include <TString.h>
#include <TRegexp.h>
#include <TH1.h>
#include <TH2.h>

#include "QwParameterFile.h"

///
/// \ingroup QwAnalysis
class QwHistogramHelper{
 public:
  QwHistogramHelper():fDEBUG(kFALSE){};
  ~QwHistogramHelper(){};

  void  LoadHistParamsFromFile(const std::string filename);

  TH1F* Construct1DHist(const TString name_title){
    std::string tmpname = name_title.Data();
    return Construct1DHist(tmpname);
  };
  TH2F* Construct2DHist(const TString name_title){
    std::string tmpname = name_title.Data();
    return Construct2DHist(tmpname);
  };

  TH1F* Construct1DHist(const TString inputfile, const TString name_title){
    std::string tmpfile = inputfile.Data();
    std::string tmpname = name_title.Data();
    return Construct1DHist(tmpfile, tmpname);
  };
  TH2F* Construct2DHist(const TString inputfile, const TString name_title){
    std::string tmpfile = inputfile.Data();
    std::string tmpname = name_title.Data();
    return Construct2DHist(tmpfile, tmpname);
  };

  TH1F* Construct1DHist(const std::string name_title);
  TH2F* Construct2DHist(const std::string name_title);

  TH1F* Construct1DHist(const std::string inputfile, const std::string name_title);
  TH2F* Construct2DHist(const std::string inputfile, const std::string name_title);
  
 protected:
  typedef struct HISTPARMS {
    std::string name_title;
    std::string type;
    Int_t nbins;
    Int_t x_nbins;
    Float_t x_min;
    Float_t x_max;
    Int_t y_nbins;
    Float_t y_min;
    Float_t y_max;
    std::string xtitle;
    std::string ytitle;
    Float_t min;
    Float_t max;
  };

 protected:
  TH1F* Construct1DHist(const HISTPARMS &params);
  TH2F* Construct2DHist(const HISTPARMS &params);

  
  const HISTPARMS GetHistParamsFromLine(QwParameterFile &mapstr);

  //look up the histogram parameters from a file according to histname.
  const HISTPARMS GetHistParamsFromFile(const std::string filename,
                                      const std::string histname);
  const HISTPARMS GetHistParamsFromList(const std::string histname);

  Bool_t DoesMatch(const std::string s, const std::string s_wildcard);

 protected:
  static const Double_t fInvalidNumber;
  static const std::string fInvalidName;
  Bool_t fDEBUG;

  std::string fInputFile;
  std::vector<HISTPARMS> fHistParams;
};

//  Declare a global copy of the histogram helper.
//  It is instantiated in the source file.
extern QwHistogramHelper gQwHists;

#endif


