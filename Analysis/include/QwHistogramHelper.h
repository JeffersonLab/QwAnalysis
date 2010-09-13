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
#include "QwOptions.h"
///
/// \ingroup QwAnalysis
class QwHistogramHelper{
 public:
  QwHistogramHelper():fDEBUG(kFALSE) { fHistParams.clear(); };
  ~QwHistogramHelper() { };

  /// \brief Define the configuration options
  static void DefineOptions(QwOptions &options);
  /// \brief Process the configuration options
  void ProcessOptions(QwOptions &options);

  void  LoadHistParamsFromFile(const std::string filename);
  void  LoadTreeParamsFromFile(const std::string filename);

  // Print the histogram parameters
  void PrintHistParams() const;

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

  const Bool_t MatchDeviceParamsFromList(const std::string devicename);
  const Bool_t MatchVQWKElementFromList(const std::string subsystemname, const std::string moduletype, const std::string devicename);
  
 protected:

  /// Histogram parameter class
  class HistParams {
   public:
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

   public:
    /// Relational less-than operator overload  
    bool operator< (const HistParams& that) const {
      // Compare only lowercase strings
      TString thisname(this->name_title); thisname.ToLower();
      TString thatname(that.name_title);  thatname.ToLower();
      if (thisname.MaybeRegexp() && thatname.MaybeRegexp()) {
        // Both wildcarded: latest occurrence of 'wildest card'
        if (thisname.Contains("*") != thatname.Contains("*"))
          return thatname.Contains("*");
        else if (thisname.First("*") != thatname.First("*"))
          return (thisname.First("*") > thatname.First("*"));
        else if (thisname.Contains("+") != thatname.Contains("+"))
          return thatname.Contains("+");
        else if (thisname.First("+") != thatname.First("+"))
          return (thisname.First("+") > thatname.First("+"));
        else if (thisname.CountChar('?') != thatname.CountChar('?'))
          return (thisname.CountChar('?') < thatname.CountChar('?'));
        else if (thisname.CountChar('.') != thatname.CountChar('.'))
          return (thisname.CountChar('.') < thatname.CountChar('.'));
        else return (thisname < thatname);
      } else if (thisname.MaybeRegexp() || thatname.MaybeRegexp())
        // One wildcarded: explicit case has precedence
        return thatname.MaybeRegexp();
      else
        // No wildcards: alphabetic ordering on the lower case names
        return (thisname < thatname);
    };

    /// Output stream operator overload
    friend ostream& operator<< (ostream& stream, const HistParams& h) {
      stream << h.type << " " << h.name_title
             << " x (" << h.xtitle << "): " << h.x_min << " -- " << h.x_max << " (" << h.x_nbins << " bins), "
             << " y (" << h.ytitle << "): " << h.y_min << " -- " << h.y_max << " (" << h.y_nbins << " bins)";
      return stream;
    }
  };

 protected:
  TH1F* Construct1DHist(const HistParams &params);
  TH2F* Construct2DHist(const HistParams &params);

  
  const HistParams GetHistParamsFromLine(QwParameterFile &mapstr);

  // Look up the histogram parameters from a file according to histname.
  const HistParams GetHistParamsFromFile(const std::string filename,
                                      const std::string histname);
  const HistParams GetHistParamsFromList(const std::string histname);

  Bool_t DoesMatch(const std::string s, const std::string s_wildcard);

 protected:
  static const Double_t fInvalidNumber;
  static const std::string fInvalidName;
  Bool_t fDEBUG;
  Bool_t fTrimDisable;
  Bool_t fTrimHistoEnable;
  Bool_t fTreeTrimFileLoaded;

  std::string fInputFile;
  std::vector<HistParams> fHistParams;
  std::vector<TString> fTreeParams;

  std::vector<TString> fSubsystemList;//stores the list of subsystems
  std::vector<std::vector<TString> > fModuleList;//will store list modules in  each subsystem (ex. for BCM, BPM etc in Beam line sub system)
  std::vector<std::vector<std::vector<TString> > > fVQWKTrimmedList; //will store list of VQWK elements for each subsystem for each module  
};

//  Declare a global copy of the histogram helper.
//  It is instantiated in the source file.
extern QwHistogramHelper gQwHists;

#endif


