/**********************************************************\
* File: QwSubsystemArrayParity.h                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2009-02-04 10:30>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAYPARITY__
#define __QWSUBSYSTEMARRAYPARITY__

#include <vector>
#include <TTree.h>

#include "QwSubsystemArray.h"
#include "VQwSubsystemParity.h"

// Forward declarations
class QwBlinder;

/**
 * \class QwSubsystemArrayParity
 * \ingroup QwAnalysis
 *
 * \brief Virtual base class for the parity subsystems
 *
 *   Virtual base class for the classes containing the
 *   event-based information from each parity subsystem.
 *   This will define the interfaces used in communicating
 *   with the CODA routines.
 *
 */
class QwSubsystemArrayParity: public QwSubsystemArray {

 public:

    /// Default constructor
    QwSubsystemArrayParity(): QwSubsystemArray(CanContain),fErrorFlag(0),fErrorFlagTreeIndex(-1){ };
    /// Constructor with options
    QwSubsystemArrayParity(QwOptions& options): QwSubsystemArray(options, CanContain),fErrorFlag(0),fErrorFlagTreeIndex(-1) { };
    /// Constructor with map file
    QwSubsystemArrayParity(const char* filename): QwSubsystemArray(filename, CanContain),fErrorFlag(0),fErrorFlagTreeIndex(-1) { };
    /// Copy constructor by pointer
    QwSubsystemArrayParity(const QwSubsystemArrayParity* source) { this->Copy(source); };
    /// Copy constructor by reference
    QwSubsystemArrayParity(const QwSubsystemArrayParity& source) { this->Copy(&source); };
    /// Default destructor
    virtual ~QwSubsystemArrayParity() { };

    /// \brief Get the subsystem with the specified name
    VQwSubsystemParity* GetSubsystemByName(const TString& name);
    /// \brief Construct a branch and vector for this subsystem with a prefix
    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector <Double_t> &values);
    /// \brief Fill the vector for this subsystem
    void FillTreeVector(std::vector<Double_t>& values) const;
    /// \brief Fill the histograms for this subsystem
    void FillHistograms();

    /// \brief Fill the database
    void FillDB(QwDatabase *db, TString type);


    /// \brief Assignment operator
    QwSubsystemArrayParity& operator=  (const QwSubsystemArrayParity &value);
    /// \brief Addition-assignment operator
    QwSubsystemArrayParity& operator+= (const QwSubsystemArrayParity &value);
    /// \brief Subtraction-assignment operator
    QwSubsystemArrayParity& operator-= (const QwSubsystemArrayParity &value);
    /// \brief Copy a subsystem array
    void Copy(const QwSubsystemArrayParity *source);
    /// \brief Sum of two subsystem arrays
    void Sum(const QwSubsystemArrayParity &value1, const QwSubsystemArrayParity &value2);
    /// \brief Difference of two subsystem arrays
    void Difference(const QwSubsystemArrayParity &value1, const QwSubsystemArrayParity &value2);
    /// \brief Ratio of two subsystem arrays
    void Ratio(const QwSubsystemArrayParity &numer, const QwSubsystemArrayParity &denom);
    /// \brief Scale this subsystem array
    void Scale(Double_t factor);


    /// \brief Update the running sums for devices
    void AccumulateRunningSum(const QwSubsystemArrayParity& value);
    /// \brief Calculate the average for all good events
    void CalculateRunningAverage();

    /// \brief Blind the asymmetry of this subsystem
    void Blind(const QwBlinder* blinder);
    /// \brief Unblind the asymmetry of this subsystem
    void UnBlind(const QwBlinder* blinder)
      { /* Not yet implemented */ };
    /// \brief Blind the difference of this subsystem
    void Blind(const QwBlinder* blinder, const QwSubsystemArrayParity& yield);
    /// \brief Unblind the difference of this subsystem
    void UnBlind(const QwBlinder* blinder, const QwSubsystemArrayParity& yield)
      { /* Not yet implemented */ };


    /// \brief Apply the single event cuts
    Bool_t ApplySingleEventCuts();
    /// \brief Report the number of events failed due to HW and event cut failures
    Int_t GetEventcutErrorCounters();
    /// \brief Return the error flag to the main routine
    UInt_t GetEventcutErrorFlag() const;
    /// \brief Return the error flag to the main routine
    void UpdateEventcutErrorFlag(UInt_t errorflag){
      fErrorFlag|=errorflag;
    };

    /// \brief Print value of all channels
    void PrintValue() const;

    virtual Bool_t CheckForEndOfBurst() const;


  public:

    //Int_t fSubsystem_Error_Flag;
    //static const Int_t kErrorFlag_Helicity=0x2;   // in Decimal 2. Helicity bit faliure
    //static const Int_t kErrorFlag_Beamline=0x4;    // in Decimal 4.  Beamline faliure

  protected:

    /// Test whether this subsystem array can contain a particular subsystem
    static Bool_t CanContain(VQwSubsystem* subsys) {
      return (dynamic_cast<VQwSubsystemParity*>(subsys) != 0);
    };
    UInt_t fErrorFlag;
    Int_t  fErrorFlagTreeIndex;

}; // class QwSubsystemArrayParity

#endif // __QWSUBSYSTEMARRAYPARITY__
