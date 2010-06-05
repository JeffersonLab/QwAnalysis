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
    QwSubsystemArrayParity(): QwSubsystemArray() { };
    /// Constructor with map file
    QwSubsystemArrayParity(const char* filename): QwSubsystemArray(filename) { };
    /// Default destructor
    virtual ~QwSubsystemArrayParity() { };

    VQwSubsystemParity* GetSubsystemByName(const TString& name);

    /// Process the command line options
    void ProcessOptions(QwOptions &options);

    /// \brief Construct the branch and tree vector
    void ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values);
    /// \brief Construct the branch and tree vector
    void ConstructBranchAndVector(TTree *tree, std::vector <Double_t> &values) {
      TString tmpstr("");
      ConstructBranchAndVector(tree,tmpstr,values);
    };
    /// \brief Fill the tree vector
    void FillTreeVector(std::vector<Double_t> &values);

    /// \brief Fill the database
    void FillDB(QwDatabase *db, TString type);

    QwSubsystemArrayParity& operator=  (const QwSubsystemArrayParity &value);
    QwSubsystemArrayParity& operator+= (const QwSubsystemArrayParity &value);
    QwSubsystemArrayParity& operator-= (const QwSubsystemArrayParity &value);
    void Copy(QwSubsystemArrayParity *source);
    void Sum(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2);
    void Difference(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2);
    void Ratio(QwSubsystemArrayParity &numer, QwSubsystemArrayParity &denom);
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

  public:

    std::vector<TString> sFailedSubsystems;

    //Int_t fSubsystem_Error_Flag;
    //static const Int_t kErrorFlag_Helicity=0x2;   // in Decimal 2. Helicity bit faliure
    //static const Int_t kErrorFlag_Beamline=0x4;    // in Decimal 4.  Beamline faliure


}; // class QwSubsystemArrayParity

#endif // __QWSUBSYSTEMARRAYPARITY__
