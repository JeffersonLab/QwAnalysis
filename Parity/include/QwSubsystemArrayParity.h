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
    QwSubsystemArrayParity(): QwSubsystemArray(CanContain) { };
    /// Constructor with options
    QwSubsystemArrayParity(QwOptions& options): QwSubsystemArray(options, CanContain) { };
    /// Constructor with map file
    QwSubsystemArrayParity(const char* filename): QwSubsystemArray(filename, CanContain) { };
    /// Copy constructor by pointer
    QwSubsystemArrayParity(const QwSubsystemArrayParity* source) { this->Copy(source); };
    /// Copy constructor by reference
    QwSubsystemArrayParity(const QwSubsystemArrayParity& source) { this->Copy(&source); };
    /// Default destructor
    virtual ~QwSubsystemArrayParity() { };

    /// \brief Get the subsystem with the specified name
    VQwSubsystemParity* GetSubsystemByName(const TString& name);


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
    Int_t GetEventcutErrorFlag();

    /// \brief Print value of all channels
    void PrintValue() const;

  public:

    std::vector<TString> sFailedSubsystems;

    //Int_t fSubsystem_Error_Flag;
    //static const Int_t kErrorFlag_Helicity=0x2;   // in Decimal 2. Helicity bit faliure
    //static const Int_t kErrorFlag_Beamline=0x4;    // in Decimal 4.  Beamline faliure

  protected:

    /// Test whether this subsystem array can contain a particular subsystem
    static Bool_t CanContain(VQwSubsystem* subsys) {
      return (dynamic_cast<VQwSubsystemParity*>(subsys) != 0);
    };

}; // class QwSubsystemArrayParity

#endif // __QWSUBSYSTEMARRAYPARITY__
