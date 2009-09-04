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
 * contains all data modules of the electron detector (V1495, ...).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#ifndef __QwComptonElectronDetector__
#define __QwComptonElectronDetector__

#include <vector>

#include "VQwSubsystemParity.h"

class QwComptonElectronDetector : public VQwSubsystemParity {

  public:
    QwComptonElectronDetector(TString name): VQwSubsystemParity(name) { };

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

    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();
    void  DeleteHistograms();

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values);

    void Copy(VQwSubsystem *source);
    VQwSubsystem*  Copy();
    Bool_t Compare(VQwSubsystem *source);
    void Print();

  protected:

  private:

    static const Bool_t kDebug = kTRUE;

};

#endif // __QwComptonElectronDetector__
