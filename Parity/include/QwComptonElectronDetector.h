/**
 * \class	QwComptonElectronDetector	QwComptonElectronDetector.h
 *
 * \brief	Class for the analysis of Compton electron detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonElectronDetector class is defined as a parity subsystem that
 * contains all data modules of the electron detector (V1495).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#ifndef __QwComptonElectronDetector__
#define __QwComptonElectronDetector__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>
#include "TFile.h"
#include<TH1D.h>

// Boost math library for random number generation
#include <boost/multi_array.hpp>


// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwScaler_Channel.h"
#include "QwVQWK_Channel.h"


class QwComptonElectronDetector: public VQwSubsystemParity {

  public:

    /// \brief Constructor
    QwComptonElectronDetector(TString name): VQwSubsystem(name), VQwSubsystemParity(name) { };
    /// \brief Destructor
    ~QwComptonElectronDetector() {
      DeleteHistograms();
    };


    /* derived from VQwSubsystem */
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);
    Int_t LoadEventCuts(TString & filename);
    Bool_t SingleEventCuts();
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    void  PrintDetectorID() const;

    void  ClearEventData();
    void  ProcessEvent();
    Bool_t IsGoodEvent();

    void RandomizeEventData(int helicity = 0);
    void EncodeEventData(std::vector<UInt_t> &buffer);

    VQwSubsystem& operator=  (VQwSubsystem *value);
    VQwSubsystem& operator+= (VQwSubsystem *value);
    VQwSubsystem& operator-= (VQwSubsystem *value);
    VQwSubsystem& operator*= (VQwSubsystem *value);

    void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
    void Scale(Double_t factor);

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    Int_t GetEventcutErrorCounters() { return 0; };
    UInt_t GetEventcutErrorFlag() { return 0; };
    Bool_t CheckRunningAverages(Bool_t ) { return kTRUE; };

    void AccumulateRunningSum(VQwSubsystem* value) { };
    void CalculateRunningAverage() { };

    using VQwSubsystem::ConstructHistograms;
    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();
    void  DeleteHistograms();

    using VQwSubsystem::ConstructTree;
    void  ConstructTree(TDirectory *folder, TString &prefix);
    void  FillTree();
    void  DeleteTree();

    using VQwSubsystem::ConstructBranchAndVector;
    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  ConstructBranch(TTree *tree, TString& prefix) { };
    void  ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
    void  FillTreeVector(std::vector<Double_t> &values) const;
    void  FillDB(QwDatabase *db, TString datatype){};

    const size_t GetNumberOfEvents() const { return (fNumberOfEvents); };
    void SetNumberOfEvents(UInt_t nevents) {
     fNumberOfEvents = nevents;
    };
    void Copy(VQwSubsystem *source);
    VQwSubsystem*  Copy();
    Bool_t Compare(VQwSubsystem *source);
    void Print() const;

    void SetCalibrationFactor(const Double_t factor) { fCalibrationFactor = factor; };
    const Double_t GetCalibrationFactor() const { return fCalibrationFactor; };



  protected:
    /// Expert tree
    TTree* fTree;
    /// Expert tree fields
    Int_t fTree_fNEvents;

    static const Int_t NModules;
    static const Int_t NPlanes;
    static const Int_t StripsPerModule;
    static const Int_t StripsPerPlane;

    std::vector< std::vector <Int_t> > fSubbankIndex;


    /// List of V1495 accumulation mode strips
    std::vector< std::vector <Double_t> > fStrips;
    std::vector< std::vector <Double_t> > fStripsRaw;
    /// List of V1495 single event mode strips
    std::vector< std::vector <Double_t> > fStripsEv;
    std::vector< std::vector <Double_t> > fStripsRawEv;


    //    boost::multi_array<Double_t, 2> array_type;
    //    array_type fStrips(boost::extents[NPlanes][StripsPerPlane]);
    //   array_type fStripsRaw(boost::extents[NPlanes][StripsPerPlane]);

 /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms()
   */

  std::vector<TH1*> fHistograms1D;
  std::vector<Int_t> fComptonElectronVector;


  private:

    static const Bool_t kDebug = kTRUE;
    Int_t edet_cut_on_x2;
    Int_t edet_cut_on_ntracks;    
    Double_t fCalibrationFactor;
    Double_t fOffset;
    Double_t edet_x2;
    Double_t edet_TotalNumberTracks;
    Int_t fStripsEvBest1;
    Int_t fStripsEvBest2;
    Int_t fStripsEvBest3;
    Double_t edet_angle;
    Int_t fTreeArrayNumEntries;
    Int_t fTreeArrayIndex;
    UInt_t fNumberOfEvents; //! Number of triggered events

    /// Mapping from ROC/subbank to channel type
    enum ChannelType_t { kUnknown, kV1495Accum, kV1495Single, kScaler };
    std::map< Int_t, ChannelType_t > fMapping;

    /// List of scaler channels
    typedef std::map< Int_t, std::vector< std::vector< Int_t > > > Scaler_Mapping_t;
    Scaler_Mapping_t fScaler_Mapping;
    std::vector< QwSIS3801D24_Channel > fScaler;

};

#endif // __QwComptonElectronDetector__
