//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaCutList.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaCutList.hh,v 1.25 2010/04/19 02:08:20 silwal Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Container of cut intervals for a given run. The cut list for a
// given run identifies all the intervals during which a cut condition
// existed.  It also contains extensions for each cut type, telling
// how many events to extend each interval before and after the stored
// event numbers; a tally of events failing each cut type; and labels
// for the cut types.  It provides functions to add cut intervals to
// the list and to determine with intervals, if any, a given event is
// in. The cut list is initialized from the database and updated after
// each event is preprocessed.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaCutList
#define PAN_TaCutList


#include "TObject.h"
#include "PanTypes.hh"
#include <vector>
#include <list>
#include <string>

using namespace std;

class TaCutInterval;
class TaDataBase;
class VaEvent;
class TaOResultsFile;

class TaCutList
{
public:

  // Constructors/destructors/operators
  TaCutList(RunNumber_t run);
  TaCutList();
  virtual ~TaCutList();
  TaCutList (const TaCutList& copy);
  TaCutList& operator= (const TaCutList& assign);

  // Major functions
  void Init(const TaDataBase&);
  Bool_t OK (const VaEvent&) const;  // True if event not in any cut interval
  Bool_t OKC (const VaEvent&) const;  // True if event not in any cut interval (hallC)
  vector<pair<Cut_t,Int_t> > CutsFailed (const VaEvent&) const; // Cuts failed by event
  void UpdateCutInterval (const Cut_t, const Int_t, const EventNumber_t);  // Update interval for this cut type
  void AddExtension (const Cut_t, const UInt_t, const UInt_t);  // Add extensions to list
  void AddName (const Cut_t, const string&);  // Add name to list
  const string& GetName (const Cut_t) const;  // Get name from list
  void PrintInt (ostream&) const;  // Print intervals
  void PrintExt (ostream&) const;  // Print extensions
  void PrintTally (ostream&) const;  // Print tally of events failing cuts
  void WriteTally (TaOResultsFile& resFile,
		   const EventNumber_t ev0,
		   const EventNumber_t ev1
		   ) const;         // Write tally to results file

  // Access functions
  UInt_t GetNumCuts () const {return fNumCuts;} // Number of cuts defined

  friend ostream& operator<< (ostream& s, const TaCutList q);

private:

  // Data members
  RunNumber_t fRunNumber;            // Run number associated with this list
  vector<TaCutInterval>* fIntervals; // List of cut intervals
  list<size_t> fOpenIntIndices;      // Indices of open interval (if any) for each cut type
  UInt_t fNumCuts;                   // Number of cuts defined
  vector<UInt_t> fLowExtension;      // Low-end extension for each cut type
  vector<UInt_t> fHighExtension;     // High-end extension for each cut type
  vector<UInt_t> fTally;             // tally of cut condition failures
  vector<string> fCutNames;          // names of cuts
  static const size_t fgMaxEvent = 1000000; // Probably should be available from VaEvent, really
  Cut_t fLoBeamCNo;                  // cut number for hallC low beam
  Cut_t fLoBeamNo;                   // cut number for low beam 
  Cut_t fBurpNo;                     // cut number for burp beam
  Cut_t fSatNo;                      // cut number for detector saturation
  Cut_t fMonSatNo;                   // cut number for monitor saturation
  Cut_t fPosBurpNo;                  // cut number for pos. burp cut
  Cut_t fPosBurpENo;                 // cut number for energy burp cut
  Cut_t fCBurpNo;                    // cut number for C-beam-burp cut
  Cut_t fAdcxDacBurpNo;              // cut number for 18-bit DAC burp cut
  Cut_t fAdcxBadNo;                  // cut number for 18-bit bad cut
  Cut_t fScalerBadNo;                  // cut number for scaler bad cut

#ifndef NODICT
  ClassDef(TaCutList, 0)   // List of cuts
#endif

};

#endif
