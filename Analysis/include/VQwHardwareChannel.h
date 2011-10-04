/**********************************************************\
* File: VQwHardwareChannel.h                               *
*                                                          *
* Author: P. King                                          *
* Date:   Tue Mar 29 13:08:12 EDT 2011                     *
\**********************************************************/

#ifndef __VQWHARDWARECHANNEL__
#define __VQWHARDWARECHANNEL__

// System headers
#include <cmath>
#include <vector>
#include <stdexcept>

// Qweak headers
#include "VQwDataElement.h"

// Forward declarations
class QwDBInterface;

class VQwHardwareChannel: public VQwDataElement {
/****************************************************************//**
 *  Class: VQwHardwareChannel
 *         Virtual base class to support common functions for all
 *         harware channel data elements.
 *         Only the data element classes which contain raw data
 *         from one physical channel (such as QwVQWK_Channel,
 *         QwScaler_Channel, etc.) should inherit from this class.
 ******************************************************************/
public:
  VQwHardwareChannel();
  VQwHardwareChannel(const VQwHardwareChannel& value);
  virtual ~VQwHardwareChannel();

  /*! \brief Get the number of data words in this data element */
  size_t GetNumberOfDataWords() {return fNumberOfDataWords;}

  /*! \brief Get the number of subelements in this data element */
  size_t GetNumberOfSubelements() {return fNumberOfSubElements;};
  
  Int_t GetRawValue() const       {return this->GetRawValue(0);};
  Double_t GetValue() const       {return this->GetValue(0);};
  Double_t GetValueM2() const     {return this->GetValueM2(0);};
  Double_t GetValueError() const  {return this->GetValueError(0);};
  Double_t GetValueWidth() const  {return this->GetValueWidth(0);};
  virtual Int_t GetRawValue(size_t element) const = 0;
  virtual Double_t GetValue(size_t element) const = 0;
  virtual Double_t GetValueM2(size_t element) const = 0;
  virtual Double_t GetValueError(size_t element) const = 0;
  Double_t GetValueWidth(size_t element) const {
    RangeCheck(element);
    Double_t width;
    if (fGoodEventCount>0){
      width = (GetValueError(element)*std::sqrt(Double_t(fGoodEventCount))); 
    } else {
      width = 0.0;
    } 
    return width;
  };

  UInt_t GetErrorCode() const {return fDeviceErrorCode;};
  void UpdateErrorCode(const UInt_t& error){fDeviceErrorCode |= error;};

  virtual void  ClearEventData(){
    VQwDataElement::ClearEventData();
    fDeviceErrorCode = 0;
    fErrorFlag = fDefErrorFlag;
  };

  /*   virtual void AddChannelOffset(Double_t Offset) = 0; */
  /*   virtual void Scale(Double_t Offset) = 0; */


  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString name){InitializeChannel(name, "raw");};
  virtual void  InitializeChannel(TString name, TString datatosave) = 0;
  virtual void  InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave) = 0;
  
  //Check for harware errors in the devices. This will return the device error code.
  virtual Int_t ApplyHWChecks() = 0;

  void SetEventCutMode(Int_t bcuts){bEVENTCUTMODE=bcuts;};

  virtual Bool_t ApplySingleEventCuts() = 0;//check values read from modules are at desired level
  
  /*! \brief Set the upper and lower limits (fULimit and fLLimit) 
   *         for this channel */
  void SetSingleEventCuts(Double_t min, Double_t max);
  /*! \brief Inherited from VQwDataElement to set the upper and lower 
   *         limits (fULimit and fLLimit), stability % and the 
   *         error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability);

  Double_t GetEventCutUpperLimit() const { return fULimit; };
  Double_t GetEventCutLowerLimit() const { return fLLimit; };

  Double_t GetStabilityLimit() const { return fStability;};


 
  /*! \brief return the error flag on this channel/device*/
  UInt_t GetEventcutErrorFlag();


  virtual void CalculateRunningAverage() = 0;
//   virtual void AccumulateRunningSum(const VQwHardwareChannel *value) = 0;

  /*! \brief Copy method:  Should make a full, identical copy. */
  virtual void Copy(const VQwDataElement *source);
  virtual void Copy(const VQwHardwareChannel& source);

  /// Arithmetic assignment operator:  Should only copy event-based data
  virtual VQwHardwareChannel& operator=(const VQwHardwareChannel& value) {
    VQwDataElement::operator=(value);
    fDeviceErrorCode     = value.fDeviceErrorCode;
    fErrorFlag           = value.fErrorFlag;
    return *this;
  }


  void     SetPedestal(Double_t ped) { fPedestal = ped; kFoundPedestal = 1; };
  Double_t GetPedestal() const       { return fPedestal; };
  void     SetCalibrationFactor(Double_t factor) { fCalibrationFactor = factor; kFoundGain = 1; };
  Double_t GetCalibrationFactor() const          { return fCalibrationFactor; };

  void AddEntriesToList(std::vector<QwDBInterface> &row_list);


protected:
  /*! \brief Set the number of data words in this data element */
  void SetNumberOfDataWords(const UInt_t &numwords) {fNumberOfDataWords = numwords;}
  /*! \brief Set the number of data words in this data element */
  void SetNumberOfSubElements(const size_t elements) {fNumberOfSubElements = elements;};

  /*! \brief Set the flag indicating if raw or derived values are
   *         in this data element */
  void SetDataToSave(TString datatosave) {
    if      (datatosave == "raw")
      fDataToSave = kRaw;
    else if (datatosave == "derived")
      fDataToSave = kDerived;
    else
      fDataToSave = kRaw; // wdc, added default fall-through
  }

  /*! \brief Checks that the requested element is in range, to be
   *         used in accesses to subelements similar to
   *         std::vector::at(). */
  void RangeCheck(size_t element) const {
    if (element<0 || element >= fNumberOfSubElements){
      TString loc="VQwDataElement::RangeCheck for "
	+this->GetElementName()+" failed for subelement "+Form("%zu",element);
      throw std::out_of_range(loc.Data());

    }
  };



protected:
  UInt_t  fNumberOfDataWords; ///< Number of raw data words in this data element
  UInt_t  fNumberOfSubElements; ///< Number of subelements in this data element

  EDataToSave fDataToSave;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*! \name Channel calibration                    */
  // @{
  Double_t fPedestal; /*!< Pedestal of the hardware sum signal,
			   we assume the pedestal level is constant over time
			   and can be divided by four for use with each block,
			   units: [counts / number of samples] */
  Double_t fCalibrationFactor;
  Bool_t kFoundPedestal;
  Bool_t kFoundGain;
  //@}

  /*! \name Single event cuts and errors                    */
  Int_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF
  Double_t fULimit, fLLimit;//this sets the upper and lower limits
  Double_t fStability;//how much deviaton from the stable reading is allowed
  
  /// Unique error code for HW failures
  UInt_t fDeviceErrorCode; 

  //Error flag
  UInt_t fErrorFlag;
  UInt_t fDefErrorFlag;
  //@}

};   // class VQwHardwareChannel

#endif // __MQWHARDWARECHANNEL__
