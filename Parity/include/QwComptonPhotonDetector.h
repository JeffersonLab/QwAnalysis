/**
 * \class	QwComptonPhotonDetector	QwComptonPhotonDetector.h
 *
 * \brief	Class for the analysis of Compton photon detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonPhotonDetector class is defined as a parity subsystem that
 * contains all data modules of the photon detector (SIS3320, V775, V792).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#ifndef __QwComptonPhotonDetector__
#define __QwComptonPhotonDetector__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwSubsystemParity.h"
#include "MQwSIS3320_Channel.h"
#include "QwVQWK_Channel.h"

class QwComptonPhotonDetector: public VQwSubsystemParity {

  public:

    /// \brief Constructor
    QwComptonPhotonDetector(TString name): VQwSubsystem(name), VQwSubsystemParity(name) { };
    /// \brief Destructor
    ~QwComptonPhotonDetector() {
      DeleteHistograms();
    };


    /* derived from VQwSubsystem */
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);
    Int_t LoadEventCuts(TString & filename);
    Bool_t SingleEventCuts();
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    void  PrintDetectorID();

    void  ClearEventData();
    void  ProcessEvent();
    Bool_t IsGoodEvent();

    void RandomizeEventData(int helicity = 0);
    void EncodeEventData(std::vector<UInt_t> &buffer);

    VQwSubsystem& operator=  (VQwSubsystem *value);
    VQwSubsystem& operator+= (VQwSubsystem *value);
    VQwSubsystem& operator-= (VQwSubsystem *value);
    void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
    void Scale(Double_t factor);

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    Int_t GetEventcutErrorCounters() { return 0; };
    Int_t GetEventcutErrorFlag() { return 0; };
    Bool_t CheckRunningAverages(Bool_t ) { return kTRUE; };

    void Calculate_Running_Average() { };
    void Do_RunningSum() { };

    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();
    void  DeleteHistograms();

    void  ConstructTree(TDirectory *folder, TString &prefix);
    void  FillTree();
    void  DeleteTree();

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values);
    void  FillDB(QwDatabase *db){};

    void Copy(VQwSubsystem *source);
    VQwSubsystem*  Copy();
    Bool_t Compare(VQwSubsystem *source);
    void Print();

    MQwSIS3320_Channel* GetSIS3320Channel(const TString name);

  protected:

    /// Expert tree
    TTree* fTree;
    /// Expert tree fields
    Int_t fTree_fNEvents;

    /// List of sampling ADC channels
    std::vector <MQwSIS3320_Channel> fSamplingADC;

    /// List of integrating QDC and TDC channels
    // Proper MQwV775TDC and MQwV792ADC support not implemented yet (wdc, 2009-09-04)
    // No data available anyway, so what's the point in having support in software yet?
    //    std::vector <MQwV775TDC> fIntegratingTDC;
    //    std::vector <MQwV792ADC> fIntegratingADC;
    std::vector <QwVQWK_Channel> fIntegratingTDC;
    std::vector <QwVQWK_Channel> fIntegratingADC;

  private:

    static const Bool_t kDebug = kTRUE;

};

#endif // __QwComptonPhotonDetector__
