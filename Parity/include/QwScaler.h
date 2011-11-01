
#ifndef __QWSCALER__
#define __QWSCALER__

// System headers
#include <vector>

// Boost headers
#include <boost/shared_ptr.hpp>

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwScaler_Channel.h"


class QwScaler: public VQwSubsystemParity, public MQwCloneable<QwScaler>
{

  public:

    /// \brief Constructor
    QwScaler(TString region_tmp);
    /// \brief Destructor
    virtual ~QwScaler();

    // Handle command line options
    static void DefineOptions(QwOptions &options);
    void ProcessOptions(QwOptions &options);

    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);

    void  ClearEventData();

    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t *buffer, UInt_t num_words);
    void  ProcessEvent();

    using VQwSubsystem::ConstructHistograms;
    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();

    using VQwSubsystem::ConstructBranchAndVector;
    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  ConstructBranch(TTree *tree, TString& prefix) { };
    void  ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
    void  FillTreeVector(std::vector<Double_t> &values) const;

    Bool_t Compare(VQwSubsystem *source);

    VQwSubsystem* Copy();
    void Copy(VQwSubsystem *source);

    VQwSubsystem& operator=(VQwSubsystem *value);
    VQwSubsystem& operator+=(VQwSubsystem *value);
    VQwSubsystem& operator-=(VQwSubsystem *value);
    void Sum(VQwSubsystem *value1, VQwSubsystem *value2);
    void Difference(VQwSubsystem *value1, VQwSubsystem *value2);
    void Ratio(VQwSubsystem *value1, VQwSubsystem  *value2);
    void Scale(Double_t factor);

    void AccumulateRunningSum(VQwSubsystem* value);
    void CalculateRunningAverage();

    Int_t LoadEventCuts(TString filename);
    Bool_t SingleEventCuts();
    Bool_t ApplySingleEventCuts();

    Int_t GetEventcutErrorCounters();
    UInt_t GetEventcutErrorFlag();
    //update the error flag in the classes belong to the subsystem.
    void UpdateEventcutErrorFlag(UInt_t errorflag){
    }
    
    void PrintValue() const;
    void PrintInfo() const;

    Double_t* GetRawChannelArray();

    Double_t GetDataForChannelInModule(Int_t modnum, Int_t channum) {
      Int_t index = fModuleChannel_Map[std::pair<Int_t,Int_t>(modnum,channum)];
      return fScaler.at(index)->GetValue();
    }

    Int_t GetChannelIndex(TString channelName, UInt_t module_number);

  private:

    // Number of good events
    Int_t fGoodEventCount;

    // Mapping from subbank to scaler channels
    typedef std::map< Int_t, std::vector< std::vector<Int_t> > > Subbank_to_Scaler_Map_t;
    Subbank_to_Scaler_Map_t fSubbank_Map;

    // Mapping from module and channel number to scaler channel
    typedef std::map< std::pair<Int_t,Int_t>, Int_t > Module_Channel_to_Scaler_Map_t;
    Module_Channel_to_Scaler_Map_t fModuleChannel_Map;

    // Mapping from name to scaler channel
    typedef std::map< TString, Int_t> Name_to_Scaler_Map_t;
    Name_to_Scaler_Map_t fName_Map;

    // Vector of scaler channels
    std::vector< VQwScaler_Channel* > fScaler; // Raw channels
    std::vector< UInt_t > fBufferOffset; // Offset in scaler buffer
    std::vector< std::pair< VQwScaler_Channel*, double > > fNorm;
};

#endif
