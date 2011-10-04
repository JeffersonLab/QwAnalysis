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

// Boost math library for random number generation
#include <boost/multi_array.hpp>

// Qweak headers
#include "VQwSubsystemParity.h"

class QwComptonElectronDetector: public VQwSubsystemParity, public MQwCloneable<QwComptonElectronDetector> {

  public:

    /// \brief Constructor
    QwComptonElectronDetector(TString name): VQwSubsystem(name), VQwSubsystemParity(name) { };
    /// \brief Destructor
    virtual ~QwComptonElectronDetector() { };


    /* derived from VQwSubsystem */
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);
    Int_t LoadEventCuts(TString & filename);
    Bool_t SingleEventCuts();
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

    void  ClearEventData();
    void  ProcessEvent();
    Bool_t IsGoodEvent();

    void RandomizeEventData(int helicity = 0);
    void EncodeEventData(std::vector<UInt_t> &buffer);

    VQwSubsystem& operator=  (VQwSubsystem *value);
    VQwSubsystem& operator+= (VQwSubsystem *value);
    VQwSubsystem& operator-= (VQwSubsystem *value);
    VQwSubsystem& operator*= (VQwSubsystem *value);

    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
    void Scale(Double_t factor);

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    UInt_t GetEventcutErrorFlag() { return 0; };
    Bool_t CheckRunningAverages(Bool_t ) { return kTRUE; };

    void AccumulateRunningSum(VQwSubsystem* value);
    void CalculateRunningAverage();

    using VQwSubsystem::ConstructHistograms;
    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();

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

    size_t GetNumberOfEvents() const { return (fNumberOfEvents); };
    void SetNumberOfEvents(UInt_t nevents) {
     fNumberOfEvents = nevents;
    };
    void Copy(const VQwSubsystem *source);
    Bool_t Compare(VQwSubsystem *source);
    void PrintValue() const;

    void SetCalibrationFactor(const Double_t factor) { fCalibrationFactor = factor; };
    Double_t GetCalibrationFactor() const { return fCalibrationFactor; };


  protected:
    /// Expert tree
    TTree* fTree;
    /// Expert tree fields
    Int_t fTree_fNEvents;

    std::vector< std::vector <Int_t> > fSubbankIndex;

    /// 1st dimension:NPlanes;2nd dimension: NStrips;
    /// List of V1495 accumulation mode strips    
    std::vector< std::vector <Int_t> > fStrips;
    std::vector< std::vector <Int_t> > fStripsRaw;
    /// List of V1495 single event mode strips
    std::vector< std::vector <Int_t> > fStripsEv;
    std::vector< std::vector <Int_t> > fStripsRawEv; 
    /// List of V1495 scaler counts
    std::vector< std::vector <Int_t> > fStripsRawScal;

    //    boost::multi_array<Double_t, 2> array_type;
    //    array_type fStrips(boost::extents[NPlanes][StripsPerPlane]);
    //   array_type fStripsRaw(boost::extents[NPlanes][StripsPerPlane]);

 /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms()
   */

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
    Int_t eff23;
    Int_t effall;
    Double_t effedet;
//    Double_t edet_acum_sum[4][96];
    
    Int_t fGoodEventCount;

    /// Mapping from ROC/subbank to channel type
    enum ChannelType_t { kUnknown, kV1495Accum, kV1495Single, kV1495Scaler};
    std::map< Int_t, ChannelType_t > fMapping;

        // Assign static const member fields
    static const Int_t NModules = 3;///number of slave modules(!!change to 2?)
    static const Int_t NPlanes = 4;///number of diamond detector planes
    static const Int_t StripsPerModule = 32;///number of strips in each v1495 slave module
    static const Int_t StripsPerPlane = 96;///total number of strips in each detecor
};

#endif // __QwComptonElectronDetector__
