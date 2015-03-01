/**********************************************************\
* File: QwScanner.h                                        *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Thu May 21 21:20:41 CDT 2009                             *
\**********************************************************/

#ifndef __QWSCANNER__
#define __QWSCANNER__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"
#include "TFile.h"
#include "TProfile2D.h"

//#include "TRandom3.h"

// Qweak headers
#include "VQwSubsystemTracking.h"
#include "VQwSubsystemParity.h"


#include "QwHit.h"
#include "QwHitContainer.h"

#include "QwTypes.h"
#include "QwDetectorInfo.h"

#include "MQwV775TDC.h"
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"
#include "QwPMT_Channel.h"
#include "QwF1TDContainer.h"

class QwScanner:
  public VQwSubsystemParity,
  public VQwSubsystemTracking,
  public MQwSubsystemCloneable<QwScanner> {

  private:
    /// Private default constructor (not implemented, will throw linker error on use)
    QwScanner();

  public:
    /// Constructor with name
    QwScanner(const TString& name);
    /// Copy constructor
    QwScanner(const QwScanner& source);
    /// Virtual destructor
    virtual ~QwScanner();

    // VQwSubsystem methods
    VQwSubsystem& operator=(VQwSubsystem *value);
    VQwSubsystem& operator+=(VQwSubsystem *value);
    VQwSubsystem& operator-=(VQwSubsystem *value);
    void ProcessOptions(QwOptions &options); //Handle command line options
    void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
    {
      return;
    };
    void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
    {
      return;
    };
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
    {
      return;
    };
    void Scale(Double_t factor)
    {
      return;
    };

    void AccumulateRunningSum(VQwSubsystem* value)
    {
      return;
    };
    //remove one entry from the running sums for devices
    void DeaccumulateRunningSum(VQwSubsystem* value){
    };

    void CalculateRunningAverage()
    {
      return;
    };

    Int_t LoadEventCuts(TString filename)
    {
      return 0;
    };
    Bool_t ApplySingleEventCuts()
    {
      return kTRUE;
    };
    void IncrementErrorCounters()
    {
    };
    void PrintErrorCounters() const
    {
    };
    Bool_t CheckRunningAverages(Bool_t )
    {
      return kTRUE;
    };

    /*  Member functions derived from VQwSubsystem. */
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadGeometryDefinition(TString filename)
    {
      return 0;
    };

    Int_t LoadInputParameters(TString parameterfile);
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    void  InitializeChannel(TString name, TString datatosave);
    void  ClearEventData();

    void  SetPedestal(Double_t ped);
    void  SetCalibrationFactor(Double_t calib);
//  void  RandomizeEventData(int helicity);
//  void  EncodeEventData(std::vector<UInt_t> &buffer);

    Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    void  ProcessEvent();

    using VQwSubsystem::ConstructHistograms;
    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();

    using VQwSubsystem::ConstructBranchAndVector;
    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values) const;

    void  FillDB(QwParityDB *db, TString type)
    {
      return;
    };

    void GetHitList(QwHitContainer & grandHitContainer) {
      grandHitContainer.Append(fTDCHits);
    };

    void  ReportConfiguration(Bool_t verbose);

    Bool_t Compare(VQwSubsystem* value);

    Bool_t ApplyHWChecks() //Check for harware errors in the devices
    {
      Bool_t status = kTRUE;
      for (size_t i=0; i<fADCs.size(); i++)
        {
          status &= fADCs.at(i).ApplyHWChecks();
        }
      return status;
    };

    void PrintValue() const { };
    void PrintInfo() const;

    void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
    void FillHardwareErrorSummary();


  protected:

    EQwModuleType fCurrentType;
    Bool_t fDEBUG;

    TString fRegion;  ///  Name of this subsystem (the region).
    Int_t   fCurrentBankIndex;
    Int_t   fCurrentSlot;
    Int_t   fCurrentModuleIndex;

    static const UInt_t kMaxNumberOfModulesPerROC;
    static const Int_t  kF1ReferenceChannelNumber;

    UInt_t kMaxNumberOfChannelsPerModule;
    Int_t fNumberOfModules;

    MQwV775TDC       fQDCTDC;
    MQwF1TDC         fF1TDCDecoder;
    QwF1TDContainer *fF1TDContainer;



    // --- For F1TDC QwHit
    void  SubtractReferenceTimes(); // be executed in ProcessEvent()
    void  UpdateHits();             // be executed in ProcessEvent()
    
    std::vector< QwHit >              fTDCHits;
    
    std::vector< std::vector< QwDetectorID   > > fDetectorIDs; 
    // Indexed by module_index and Channel; and so on....
    std::vector< std::pair<Int_t, Int_t> >       fReferenceChannels;  
    // reference chans number <first:tdc_index, second:channel_number>
    // fReferenceChannels[tdc_index,channel_number][ num of [tdc,chan] set]
    std::vector< std::vector<Double_t> >         fReferenceData; 
    // fReferenceData[bank_index][channel_number]
    // --- For F1TDC QwHit


    // For reference time substraction
    Int_t fRefTime_SlotNum;
    Int_t fRefTime_ChanNum;
    Double_t fRefTime;
    
    Bool_t IsF1ReferenceChannel (Int_t slot, Int_t chan) { 
      return ( slot == fRefTime_SlotNum &&  chan == fRefTime_ChanNum) ;
    };


    //    We need a mapping of module,channel into PMT index, ADC/TDC
    std::vector< std::vector<QwPMT_Channel> > fPMTs;  // for QDC/TDC and F1TDC

    std::vector<QwSIS3801D24_Channel>         fSCAs;
    std::map<TString,size_t>                  fSCAs_map;
    std::vector<Int_t>                        fSCAs_offset;

    std::vector<QwVQWK_Channel>               fADCs;
    std::map<TString,size_t>                  fADCs_map;
    std::vector<Int_t>                        fADCs_offset;





    void  FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
    void  ClearAllBankRegistrations();
    Int_t RegisterROCNumber(const UInt_t roc_id);
    Int_t RegisterSubbank(const UInt_t bank_id);

    // Tells this object that it will decode data from the current bank
    Int_t RegisterSlotNumber(const UInt_t slot_id);
    EQwModuleType RegisterModuleType(TString moduletype);
    Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;
    Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const
      {
        return (GetModuleIndex(bank_index,slot_num) != -1);
      };

    Int_t LinkChannelToSignal(const UInt_t chan, const TString &name);
    Int_t FindSignalIndex(const EQwModuleType modtype, const TString &name) const;
 
    std::vector< std::vector<Int_t> > fModuleIndex;  /// Module index, indexed by bank_index and slot_number
    std::vector< EQwModuleType > fModuleTypes;
    std::vector< std::vector< std::pair< EQwModuleType, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel

    UInt_t GetEventcutErrorFlag()
    {
      return 0;
    };//return the error flag to the main routine

    //update the error flag in the subsystem level from the top level routines related to stability checks.
    // This will uniquely update the errorflag at each channel based on the error flag in the corresponding
    // channel in the ev_error subsystem

    void UpdateErrorFlag(const VQwSubsystem *ev_error){
    };


    // scanner specified histograms
    TProfile2D* fRateMapCM;
    TProfile2D* fRateMapEM;

    std::vector<TString> fParameterFileNames;
    TH1F*       fParameterFileNamesHist;

  private:
    // Double_t get_value( TH2* h, Double_t x, Double_t y, Int_t& checkvalidity);

  private:

    static const Bool_t bStoreRawData;

    UInt_t fScaEventCounter;

    /// variables for calibrating and calculating scanner positions
    Double_t fHelicityFrequency;

    Double_t fHomePositionX;
    Double_t fHomePositionY;
    Double_t fVoltage_Offset_X;
    Double_t fVoltage_Offset_Y;
    Double_t fChannel_Offset_X;
    Double_t fChannel_Offset_Y;

    Double_t fCal_Factor_VQWK_X;
    Double_t fCal_Factor_VQWK_Y;
    Double_t fCal_Factor_QDC_X;
    Double_t fCal_Factor_QDC_Y;

    Double_t fPowSupply_VQWK;
    Double_t fPositionX_VQWK;
    Double_t fPositionY_VQWK;
    Double_t fFrontSCA;
    Double_t fBackSCA;
    Double_t fCoincidenceSCA;
    Double_t fFrontADC;
    Double_t fFrontTDC;
    Double_t fBackADC;
    Double_t fBackTDC;
    //Double_t fPowSupply_ADC;
    Double_t fPositionX_ADC;
    Double_t fPositionY_ADC;

    Double_t fMeanPositionX_ADC;
    Double_t fMeanPositionY_ADC;

    UInt_t fBankID[4];  //bank ID's of 4 different modules for scanner
    //fBankID[0] for V792/V775 QDC_Bank
    //fBankID[1] for SIS3801   SCA_Bank
    //fBankID[2] for F1TDC     F1TDC_Bank
    //fBankID[3] for VQWK      VQWK_Bank

  };


#endif

