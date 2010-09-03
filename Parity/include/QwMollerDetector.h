/**
 * \class QwMollerDetector  QwMollerDetector.h
 *
 * \brief Implementation of the analysis of Moller data (copied from QwComptonElectronDetector.h)
 *
 * \author  Andrew Kubera
 * \date  2010-06-07
 * \ingroup QwMoller
 *
 */


#ifndef __QwMollerDetector__
#define __QwMollerDetector__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>
#include "TFile.h"
#include<TH1D.h>

// Boost math library for random number generation
#include <boost/multi_array.hpp>


// Qweak headers
#include "VQwSubsystem.h"
#include "VQwSubsystemParity.h"
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"

class QwMollerChannelID
{
  public:
  QwMollerChannelID():fIndex(-1),fSubbankIndex(-1),fWordInSubbank(0),
    fChannelNumber(-1),fModuleType(""),fChannelName(""),fDetectorType(""){};

  int fIndex;  // index of this detector in the vector containing all the detector of same type
  int fSubbankIndex;

  int fWordInSubbank; //first word reported for this channel in the subbank
                      //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)
                      // The first word of the subbank gets fWordInSubbank=0

  UInt_t fChannelNumber;

  TString fModuleType;
  UInt_t  fModuleNumber;
  TString fChannelName;
  TString fDetectorType;

  void Print(){
    QwMessage << "***************************************" << std::endl;
    QwMessage << " QwMoller channel: " << fChannelName << std::endl;
    QwMessage << " Detector Type: " << fDetectorType << std::endl;
    QwMessage << " Channel #: " << fChannelNumber << std::endl;
    QwMessage << " Word In Sub: " << fWordInSubbank << std::endl;
    QwMessage << " Index: " << fIndex << std::endl;
  };

};

class QwMollerDetector: public VQwSubsystemParity {
  public:

    /// \brief Constructor
    QwMollerDetector(TString name): VQwSubsystem(name), VQwSubsystemParity(name) { };

    /// \brief Destructor
    ~QwMollerDetector() {
      DeleteHistograms();
    };

    /* derived from VQwSubsystem */
    void ProcessOptions(QwOptions &options); //Handle command line options
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);
    Int_t LoadEventCuts(TString & filename);
    Bool_t SingleEventCuts();
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessConfigurationBuffer(UInt_t ev_type, const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t *buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(UInt_t ev_type, UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    void  PrintDetectorID();
    void  ClearEventData();
    void  ProcessEvent();
    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();
    void  DeleteHistograms();
    VQwSubsystem& operator=  (VQwSubsystem *value);
    VQwSubsystem& operator+= (VQwSubsystem *value);
    VQwSubsystem& operator-= (VQwSubsystem *value);
    VQwSubsystem* Copy();
    void  Copy(VQwSubsystem *source);
    void  Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void  Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void  Ratio(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void  Scale(Double_t);
    void  AccumulateRunningSum(VQwSubsystem* value);
    void  CalculateRunningAverage();
    Int_t LoadEventCuts(TString filename);
    Bool_t  ApplySingleEventCuts();
    Int_t GetEventcutErrorCounters();
    Int_t GetEventcutErrorFlag();
    void ConstructBranchAndVector(TTree*, TString&, std::vector<double, std::allocator<double> >&);
    void ConstructBranch(TTree *tree, TString& prefix) { };
    void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
    void FillTreeVector(std::vector<Double_t> &values) const;

    Bool_t Compare(VQwSubsystem *source);
    void print();
    void PrintValue() const;
    float* GetRawChannelArray();

    Int_t GetChannelIndex(TString channelName, UInt_t module_number); 
    float GetDataForChannelInModule(Int_t module_number, Int_t channel_index){
      return fSTR7200_Channel[module_number][channel_index].GetValue();
    }
      
    float GetDataForChannelInModule(Int_t module_number, TString channel_name){
      return GetDataForChannelInModule(module_number, GetChannelIndex(channel_name,module_number));
    }

  protected:
    //Array of ChannelIDs which contain the map file
    std::vector<QwMollerChannelID> fMollerChannelID;
    
    //the running total scaler structure
    std::vector< std::vector<QwSTR7200_Channel> > fSTR7200_Channel;
    std::vector< std::vector<QwSTR7200_Channel> > fPrevious_STR7200_Channel;

  private:
    Int_t fQwMollerErrorCount;
    static const Bool_t bDEBUG = kFALSE;
    UInt_t fNumberOfEvents; //! Number of triggered events

};

#endif // __QwMollerDetector__
