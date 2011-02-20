/**********************************************************\
* File: QwHelicityPattern.cc                              *
*                                                         *
* Author:                                                 *
* Time-stamp:                                              *
\**********************************************************/

#include "QwHelicityPattern.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"
#include "QwHelicity.h"
#include "QwBlinder.h"
#include "VQwDataElement.h"


/*****************************************************************/
/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwHelicityPattern::DefineOptions(QwOptions &options)
{
  options.AddOptions("Helicity pattern")
    ("enable-burstsum", po::value<bool>()->default_value(false)->zero_tokens(),
     "enable burst sum calculation");
  options.AddOptions("Helicity pattern")
    ("enable-runningsum", po::value<bool>()->default_value(true)->zero_tokens(),
     "enable running sum calculation");
  options.AddOptions("Helicity pattern")
    ("enable-differences", po::value<bool>()->default_value(false)->zero_tokens(),
     "store pattern differences in tree");
  options.AddOptions("Helicity pattern")
    ("enable-alternateasym", po::value<bool>()->default_value(false)->zero_tokens(),
     "enable alternate asymmetries");

  QwBlinder::DefineOptions(options);
}

/*****************************************************************/
void QwHelicityPattern::ProcessOptions(QwOptions &options)
{
  fEnableBurstSum   = options.GetValue<bool>("enable-burstsum");
  fEnableRunningSum = options.GetValue<bool>("enable-runningsum");
  fEnableDifference = options.GetValue<bool>("enable-differences");
  fEnableAlternateAsym = options.GetValue<bool>("enable-alternateasym");

  fBlinder.ProcessOptions(options);
}

/*****************************************************************/
QwHelicityPattern::QwHelicityPattern(QwSubsystemArrayParity &event)
  : fBlinder(),
    fHelicityIsMissing(kFALSE), fIgnoreHelicity(kFALSE),
    fLastWindowNumber(0),fLastPatternNumber(0),fLastPhaseNumber(0)
 
{
  // Retrieve the helicity subsystem to query for
  std::vector<VQwSubsystem*> subsys_helicity = event.GetSubsystemByType("QwHelicity");
  if (subsys_helicity.size() > 0) {

    // Take the first helicity subsystem
    //    fHelicitySubsystem = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));
    QwHelicity* helicity = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));

    // Get the maximum pattern phase (i.e. pattern size)
    fPatternSize = helicity->GetMaxPatternPhase();

    // Warn if more than one helicity subsystem defined
    if (subsys_helicity.size() > 1)
      QwWarning << "Multiple helicity subsystems defined! "
                << "Using " << helicity->GetSubsystemName() << "."
                << QwLog::endl;

  } else {
    // We are not using any helicity subsystem
    QwError << "No helicity subsystem defined!  " 
	    << "Calculate asymmetries based on (+--+) quartets!"
	    << QwLog::endl;
    fHelicityIsMissing = kTRUE;
    fPatternSize = 4; // default to quartets
  }
  QwMessage << "QwHelicity::MaxPatternPhase = " << fPatternSize << QwLog::endl;

  // Disable burst analysis
  fEnableBurstSum = kFALSE;
  // Enable running sum
  fEnableRunningSum = kTRUE;
  // Disable alternate asymmetry
  fEnableAlternateAsym = kFALSE;

  // Currently the alternate asym works with quartets only
  if (fPatternSize != 4)
    fEnableAlternateAsym = kFALSE;

  try
    {
      if(fPatternSize%2 == 0)
        {
          fEvents.resize(fPatternSize);
          for(int i=0;i<fPatternSize;i++)
            {
              fHelicity.push_back(-9999);
              fEventLoaded.push_back(kFALSE);
              fEvents[i].Copy(&event);
              fEventNumber.push_back(-1);
            }


          // Initialize the pattern number
          fQuartetNumber = 0;
          fCurrentPatternNumber = -1;

          // Positive and negative helicity sum and difference
          fPositiveHelicitySum.Copy(&event);
          fNegativeHelicitySum.Copy(&event);
          fDifference.Copy(&event);

          // Primary yield and asymmetry
          fYield.Copy(&event);
          fAsymmetry.Copy(&event);
          // Alternate asymmetries
          fAlternateDiff.Copy(&event);
          fAsymmetry1.Copy(&event);
          fAsymmetry2.Copy(&event);

          // Burst sum quantities
          fBurstYield.Copy(&event);
          fBurstDifference.Copy(&event);
          fBurstAsymmetry.Copy(&event);
          // Clear the burst sum
          ClearBurstSum();

          // Running sum quantities
          fRunningYield.Copy(&event);
          fRunningDifference.Copy(&event);
          fRunningAsymmetry.Copy(&event);
          // Running alternate asymmetries
          fRunningAsymmetry1.Copy(&event);
          fRunningAsymmetry2.Copy(&event);
          // Running burst sums
          fRunningBurstYield.Copy(&event);
          fRunningBurstDifference.Copy(&event);
          fRunningBurstAsymmetry.Copy(&event);
          // Clear the running sum
          ClearRunningSum();


        }
      else
        {
          TString loc=
            "Standard exception from QwHelicityPattern : the pattern size has to be even;  right now pattern_size=";
          loc+=Form("%d",fPatternSize);
          throw std::invalid_argument(loc.Data());
        }
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

};


//*****************************************************************
/**
 * Load event data corresponding to the current pattern from the
 * subsystems.
 */
void QwHelicityPattern::LoadEventData(QwSubsystemArrayParity &event)
{

  Bool_t localdebug = kFALSE;
  fPatternIsGood = kFALSE;

  Long_t localEventNumber = -1;
  Long_t localPatternNumber = -1;
  Int_t  localPhaseNumber = -1;
  Int_t  localHelicityActual = -1;
  Bool_t localIgnoreHelicity = kFALSE;
  
  
  // Get the list of helicity subsystems
  if (! fHelicityIsMissing){
    std::vector<VQwSubsystem*> subsys_helicity = event.GetSubsystemByType("QwHelicity");
    QwHelicity* helicity = 0;
    
    if (subsys_helicity.size() > 0) {
      // Take the first helicity subsystem
      helicity = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));
      if (helicity->HasDataLoaded()){
	localIgnoreHelicity = helicity->IsHelicityIgnored();
	// Get the event, pattern, phase number and helicity
	localEventNumber    = helicity->GetEventNumber();
	localPatternNumber  = helicity->GetPatternNumber();
	localPhaseNumber    = helicity->GetPhaseNumber() - helicity->GetMinPatternPhase(); // Use "reduced pattern phase", counts from 0.
	localHelicityActual = helicity->GetHelicityActual();
      } else {
	QwError << "QwHelicityPattern::LoadEventData:  The helicity subsystem does not have valid data!"
		<< QwLog::endl;
      }
    } else {
      // We are not usng any helicity subsystem
      static Bool_t user_has_been_warned = kFALSE;
      if (! user_has_been_warned) {
	QwError << "No helicity subsystem found!  Dropping to \"Missing Helicity\" mode!" << QwLog::endl;
	user_has_been_warned = kTRUE;
	fHelicityIsMissing = kTRUE;
      }
    }
  }
  if (fHelicityIsMissing){
    localIgnoreHelicity = kTRUE;
    localPatternNumber = fLastPatternNumber;
    localEventNumber   = fLastWindowNumber + 1;
    localPhaseNumber   = fLastPhaseNumber + 1;
    if(localPhaseNumber>=fPatternSize){
      localPatternNumber++;
      localPhaseNumber = 0; // Use "reduced pattern phase", counts from 0.
    }
    fLastWindowNumber  = localEventNumber;
    fLastPhaseNumber   = localPhaseNumber;
    fLastPatternNumber = localPatternNumber; 
  }
  if(localdebug) {
    std::cout<<"\n ###################################\n";
    std::cout<<"QwHelicityPattern::LoadEventData :: ";
    std::cout<<" event, pattern, phase # "<<localEventNumber<<" "<<localPatternNumber<<" "<<localPhaseNumber<<"\n";
    std::cout<<" helicity ="<< localHelicityActual<<"\n";
    for(size_t i=0; i<fEvents.size(); i++)
      std::cout<<i<<":"<<fEventLoaded[i]<<"  ";
    std::cout<<"\n";
  }
  if(fCurrentPatternNumber!=localPatternNumber){
    // new pattern
    ClearEventData();
    fCurrentPatternNumber=localPatternNumber;
  }
  if(localPhaseNumber<0){
    QwWarning << "QwHelicityPattern::LoadEventData:  "
	    << "Reduced event phase number is less than zero; ignore this event."
	    << QwLog::endl;
    ClearEventData();
  } else if(localPhaseNumber>=fPatternSize){
    QwWarning<<" In QwHelicityPattern::LoadEventData trying upload an event with a phase larger than expected \n"
	   <<" phase ="<<localPhaseNumber+1<<" maximum expected phase="<<fPatternSize<<"\n"
	   <<" operation impossible, pattern reset to 0: no asymmetries will be computed "<<QwLog::endl;
    ClearEventData();
  } else {
    if(localdebug){
      std::cout<<"QwHelicityPattern::LoadEventData local i="
	       <<localPhaseNumber<<"\n";
    }
    fEvents[localPhaseNumber]      = event;
    fEventLoaded[localPhaseNumber] = kTRUE;
    fHelicity[localPhaseNumber]    = localHelicityActual;
    fEventNumber[localPhaseNumber] = localEventNumber;
    // Check to see if we should ignore the helicity; this is
    // reset to false in ClearEventData.
    fIgnoreHelicity |= localIgnoreHelicity;
    SetDataLoaded(kTRUE);
  }
  if(localdebug){
    Print();
  }
  return;
};

//*****************************************************************
/**
 * Check to see if the pattern is complete.
 */
Bool_t  QwHelicityPattern::IsCompletePattern() const
{
  Bool_t localdebug=kFALSE;
  Bool_t filled=kTRUE;
  Int_t i=fPatternSize-1;
  while(filled && i>-1)
    {
      if (localdebug){
        std::cout<<" i="<<i<<" is loaded ?"
                 <<fEventLoaded[fEvents.size()-i-1]<<"\n";
      }
      if(!fEventLoaded[i])
        filled=kFALSE;
      i--;
    }



  return filled;
}


//*****************************************************************
/**
 * Calculate asymmetries for the current pattern.
 */
void  QwHelicityPattern::CalculateAsymmetry()
{

  Bool_t localdebug=kFALSE;

  if(localdebug)  std::cout<<"Entering QwHelicityPattern::CalculateAsymmetry \n";

  Int_t plushel  = 1;
  Int_t minushel = 0;
  Int_t checkhel = 0;
  Bool_t firstplushel=kTRUE;
  Bool_t firstminushel=kTRUE;

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();

  if (fIgnoreHelicity){
    //  Don't check to see if we have equal numbers of even and odd helicity states in this pattern.
    //  Build an asymmetry with even-parity phases as "+" and odd-parity phases as "-"
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      Int_t localhel = 1;
      for (size_t j = 0; j < (size_t) fPatternSize/2; j++) {
	localhel ^= ((i >> j)&0x1);
      }
      if (localhel == plushel) {
	if (firstplushel) {
	  fPositiveHelicitySum = fEvents.at(i);
	  firstplushel = kFALSE;
	} else {
	  fPositiveHelicitySum += fEvents.at(i);
	}
      } else if (localhel == minushel) {
	if (firstminushel) {
	  fNegativeHelicitySum = fEvents.at(i);
	  firstminushel = kFALSE;
	} else {
	  fNegativeHelicitySum += fEvents.at(i);
	}
      }
    }
  } else {
    //  
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      if (fHelicity[i] == plushel) {
	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  here filling fPositiveHelicitySum \n";
	if (firstplushel) {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with = \n";
	  fPositiveHelicitySum = fEvents.at(i);
	  firstplushel = kFALSE;
	} else {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with += \n";
	  fPositiveHelicitySum += fEvents.at(i);
	}
	checkhel += 1;
      } else if (fHelicity[i] == minushel) {
	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  here filling fNegativeHelicitySum \n";
	if (firstminushel) {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with = \n";
	  fNegativeHelicitySum = fEvents.at(i);
	  firstminushel = kFALSE;
	} else {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with += \n";
	  fNegativeHelicitySum += fEvents.at(i);
	}
	checkhel -= 1;
      } else {
	QwWarning<< "QwHelicityPattern::CalculateAsymmetry:  "
	       << "Helicity should be "<<plushel<<" or "<<minushel<<" but is "<< fHelicity[i]
	       << "; Asymmetry computation aborted!"<<QwLog::endl;
	ClearEventData();
	i = fPatternSize;
	checkhel = -9999;
	// This is an unknown helicity event.
      }
    }
  }

  if (checkhel == -9999) {
    //do nothing the asymmetry computation has been aborted earlier in this function
    fPatternIsGood = kFALSE;
  } else if (checkhel!=0) {
    fPatternIsGood = kFALSE;
    // there is a different number of plus and minus helicity window.
    QwError<<" QwHelicityPattern::CalculateAsymmetry == \n"
	   <<" you do not have the same number of positive and negative \n"
	   <<" impossible to compute assymetry \n"
	   <<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<QwLog::endl;
  } else {
    //  This is a good pattern.
    //  Calculate the asymmetry.
    fPatternIsGood = kTRUE;
    fQuartetNumber++;//Then increment the quartet number
    //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";

    fYield.Sum(fPositiveHelicitySum,fNegativeHelicitySum);
    fYield.Scale(1.0/fPatternSize);
    fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
    fDifference.Scale(1.0/fPatternSize);

    
    
    if (! fIgnoreHelicity){
      //  Only blind the difference if we're using the real helicity.
      fBlinder.Blind(fDifference,fYield);
    }
    fAsymmetry.Ratio(fDifference,fYield);

    /*
      With additional two asymmetry calculations
      Don't blind them!

      quartet pattern + - - +
                      1 2 3 4
                      fAsymmetry  = (1+4)-(2+3)/(1+2+3+4)
                      fAsymmetry1 = (1+2)-(3+4)/(1+2+3+4)
                      fAsymmetry2 = (1+3)-(2+4)/(1+2+3+4)
    */
    if (fEnableAlternateAsym) {
      //  fAsymmetry1:  (first 1/2 pattern - second 1/2 pattern)/fYield
      fPositiveHelicitySum.ClearEventData();
      fNegativeHelicitySum.ClearEventData();
      fPositiveHelicitySum = fEvents.at(0);
      fNegativeHelicitySum = fEvents.at(fPatternSize/2);
      if (fPatternSize/2 > 1){
	for (size_t i = 1; i < (size_t) fPatternSize/2 ; i++){
	  fPositiveHelicitySum += fEvents.at(i);
	  fNegativeHelicitySum += fEvents.at(fPatternSize/2 +i);
	}
      }
      fAlternateDiff.ClearEventData();
      fAlternateDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
      fAlternateDiff.Scale(1.0/fPatternSize);
      //  Do not blind this helicity-uncorrelated difference.
      fAsymmetry1.Ratio(fAlternateDiff,fYield);
      //  fAsymmetry2:  (even events - odd events)/fYield
      //  Only build fAsymmetry2 if fPatternSize>2.
      if (fPatternSize > 2) {
	fPositiveHelicitySum.ClearEventData();
	fNegativeHelicitySum.ClearEventData();
	fPositiveHelicitySum = fEvents.at(0);
	fNegativeHelicitySum = fEvents.at(1);
	if (fPatternSize/2 > 1){
	  for (size_t i = 1; i < (size_t) fPatternSize/2 ; i++){
	    fPositiveHelicitySum += fEvents.at(2*i);
	    fNegativeHelicitySum += fEvents.at(2*i + 1);
	  }
	}
	fAlternateDiff.ClearEventData();
	fAlternateDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
	fAlternateDiff.Scale(1.0/fPatternSize);
	//  Do not blind this helicity-uncorrelated difference.
	fAsymmetry2.Ratio(fAlternateDiff,fYield);
      }
    }

    // Accumulate the burst and running sums
    if (fEnableBurstSum)   AccumulateBurstSum();
    if (fEnableRunningSum) AccumulateRunningSum();
    
    if (localdebug) QwDebug << " pattern number =" << fQuartetNumber << QwLog::endl;
  }

  return;
};

//*****************************************************************
/**
 * Clear event data and the vectors used for the calculation of.
 * yields and asymmetries.
 */
void QwHelicityPattern::ClearEventData()
{
  fIgnoreHelicity = kFALSE;
  for(size_t i=0; i<fEvents.size(); i++)
    {
      fEvents[i].ClearEventData();
      fEventLoaded[i]=kFALSE;
      fHelicity[i]=-999;
    }
  fBlinder.ClearEventData();

  // Primary yield and asymmetry
  fYield.ClearEventData();
  fAsymmetry.ClearEventData();
  // Alternate asymmetries
  if (fEnableAlternateAsym){
    fAsymmetry1.ClearEventData();
    fAsymmetry2.ClearEventData();
  }

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();
  fDifference.ClearEventData();
  fAlternateDiff.ClearEventData();

  fPatternIsGood = kFALSE;
  SetDataLoaded(kFALSE);
};

//*****************************************************************
/**
 * Clear the running sums of yield, difference and asymmetry.
 * Also clear the running burst sums if enabled.
 */
void  QwHelicityPattern::ClearRunningSum()
{
  if (fEnableRunningSum) {
    fRunningYield.ClearEventData();
    fRunningDifference.ClearEventData();
    fRunningAsymmetry.ClearEventData();
    // Running alternate asymmetries
    if (fEnableAlternateAsym) {
      fRunningAsymmetry1.ClearEventData();
      fRunningAsymmetry2.ClearEventData();
    }
    // Running burst sums
    if (fEnableBurstSum) {
      fRunningBurstYield.ClearEventData();
      fRunningBurstDifference.ClearEventData();
      fRunningBurstAsymmetry.ClearEventData();
    }
  }
}

//*****************************************************************
/**
 * Clear the burst sums of yield and difference.  No asymmetry
 * burst sum is used.
 */
void  QwHelicityPattern::ClearBurstSum()
{
  if (fEnableBurstSum) {
    fBurstYield.ClearEventData();
    fBurstDifference.ClearEventData();
    fBurstAsymmetry.ClearEventData();
  }
}

//*****************************************************************
/**
 * Accumulate the burst sum by adding this helicity pattern to the
 * burst sums of yield and difference.  There is no burst sum of
 * asymmetry, because that can only be calculated with meaningful
 * moments at the end of a burst.
 */
void  QwHelicityPattern::AccumulateBurstSum()
{
  if (fPatternIsGood){
    fBurstYield.AccumulateRunningSum(fYield);
    fBurstDifference.AccumulateRunningSum(fDifference);
    // The difference is blinded, so the burst difference is also blinded.
  }
}

//*****************************************************************
/**
 * Accumulate the running sum by adding this helicity pattern to the
 * running sums of yield, difference and asymmetry.
 */
void  QwHelicityPattern::AccumulateRunningSum()
{
  if (fPatternIsGood){
    fRunningYield.AccumulateRunningSum(fYield);
    fRunningDifference.AccumulateRunningSum(fDifference);
    // The difference is blinded, so the running difference is also blinded.
    fRunningAsymmetry.AccumulateRunningSum(fAsymmetry);
    if (fEnableAlternateAsym) {
      fRunningAsymmetry1.AccumulateRunningSum(fAsymmetry1);
      fRunningAsymmetry2.AccumulateRunningSum(fAsymmetry2);
    }
  }
}

//*****************************************************************
/**
 * Accumulate the running burst sum by adding the current burst sum
 * to the running sums of burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::AccumulateRunningBurstSum()
{
  // Accumulate the burst yield and difference
  if (fEnableRunningSum) {
    fRunningBurstYield.AccumulateRunningSum(fBurstYield);
    fRunningBurstDifference.AccumulateRunningSum(fBurstDifference);
    // The burst difference is blinded, so the running burst difference is also blinded.
  }

  // Calculate asymmetry over this entire burst
  fBurstAsymmetry.Ratio(fBurstDifference, fBurstYield);
  // Accumulate this burst asymmetry
  fRunningBurstAsymmetry.AccumulateRunningSum(fBurstAsymmetry);

  // Be sure to clear the burst sums after this function!
}

//*****************************************************************
/**
 * Calculate the average burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::CalculateBurstAverage()
{
  fBurstAsymmetry.CalculateRunningAverage();
  fBurstDifference.CalculateRunningAverage();
  fBurstYield.CalculateRunningAverage();
}

//*****************************************************************
/**
 * Calculate the average running burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::CalculateRunningBurstAverage()
{
  fRunningBurstAsymmetry.CalculateRunningAverage();
  fRunningBurstDifference.CalculateRunningAverage();
  fRunningBurstYield.CalculateRunningAverage();
}

//*****************************************************************
void  QwHelicityPattern::CalculateRunningAverage()
{
  fRunningAsymmetry.CalculateRunningAverage();
  fRunningDifference.CalculateRunningAverage();
  fRunningYield.CalculateRunningAverage();
}

//*****************************************************************
void  QwHelicityPattern::PrintRunningBurstAverage() const
{
  QwMessage << " Running burst average of asymmetry" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstAsymmetry.PrintValue();

  QwMessage << " Running burst average of difference" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstDifference.PrintValue();

  QwMessage << " Running burst average of yields" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstYield.PrintValue();
}

//*****************************************************************
void  QwHelicityPattern::PrintRunningAverage() const
{
  QwMessage << " Running average of asymmetry  " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningAsymmetry.PrintValue();

  QwMessage << " Running average of difference " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningDifference.PrintValue();

  QwMessage << " Running average of yields     " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningYield.PrintValue();
}

//*****************************************************************
void  QwHelicityPattern::ConstructHistograms(TDirectory *folder)
{
  TString prefix = "yield_";
  fYield.ConstructHistograms(folder,prefix);
  prefix = "asym_";
  fAsymmetry.ConstructHistograms(folder,prefix);

  if (fEnableDifference) {
    prefix = "diff_";
    fDifference.ConstructHistograms(folder,prefix);
  }
  if (fEnableAlternateAsym) {
    prefix = "asym1_";
    fAsymmetry1.ConstructHistograms(folder,prefix);
    prefix = "asym2_";
    fAsymmetry2.ConstructHistograms(folder,prefix);
  }
}

void  QwHelicityPattern::FillHistograms()
{
  if (fPatternIsGood) {
    fYield.FillHistograms();
    fAsymmetry.FillHistograms();
    if (fEnableDifference) {
      fDifference.FillHistograms();
    }
    if (fEnableAlternateAsym){
      fAsymmetry1.FillHistograms();
      fAsymmetry2.FillHistograms();
    }
  }
}

void  QwHelicityPattern::DeleteHistograms()
{
  fYield.DeleteHistograms();
  fAsymmetry.DeleteHistograms();

  if (fEnableDifference) {
    fDifference.DeleteHistograms();
  }
  if (fEnableAlternateAsym) {
    fAsymmetry1.DeleteHistograms();
    fAsymmetry2.DeleteHistograms();
  }
}

void QwHelicityPattern::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  TString newprefix = "yield_" + prefix;
  fYield.ConstructBranchAndVector(tree, newprefix, values);
  newprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranchAndVector(tree, newprefix, values);

  if (fEnableDifference) {
    newprefix = "diff_" + prefix;
    fDifference.ConstructBranchAndVector(tree, newprefix, values);
  }
  if (fEnableAlternateAsym) {
    newprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranchAndVector(tree, newprefix, values);
    newprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranchAndVector(tree, newprefix, values);
  }
}

void QwHelicityPattern::ConstructBranch(TTree *tree, TString & prefix)
{
  TString newprefix = "yield_" + prefix;
  fYield.ConstructBranch(tree, newprefix);
  newprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranch(tree, newprefix);

  if (fEnableDifference) {
    newprefix = "diff_" + prefix;
    fDifference.ConstructBranch(tree, newprefix);
  }
  if (fEnableAlternateAsym) {
    newprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranch(tree, newprefix);
    newprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranch(tree, newprefix);
  }
}

void QwHelicityPattern::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile &trim_tree)
{
  TString newprefix = "yield_" + prefix;
  fYield.ConstructBranch(tree, newprefix, trim_tree);
  newprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranch(tree, newprefix, trim_tree);

  if (fEnableDifference) {
    newprefix = "diff_" + prefix;
    fDifference.ConstructBranch(tree, newprefix);
  }
  if (fEnableAlternateAsym) {
    newprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranch(tree, newprefix, trim_tree);
    newprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranch(tree, newprefix, trim_tree);
  }
}

void QwHelicityPattern::FillTreeVector(std::vector<Double_t> &values) const
{
  if (fPatternIsGood) {
    fYield.FillTreeVector(values);
    fAsymmetry.FillTreeVector(values);
    if (fEnableDifference) {
      fDifference.FillTreeVector(values);
    }
    if (fEnableAlternateAsym) {
      fAsymmetry1.FillTreeVector(values);
      fAsymmetry2.FillTreeVector(values);
    }
  }
};


void QwHelicityPattern::FillDB(QwDatabase *db)
{
  fBlinder.FillDB(db,"");

  fRunningYield.FillDB(db, "yield");
  fRunningAsymmetry.FillDB(db, "asymmetry");
  if (fEnableDifference) {
    fDifference.FillDB(db, "difference");
  }
  if (fEnableAlternateAsym) {
    fAsymmetry1.FillDB(db, "asymmetry1");
    fAsymmetry2.FillDB(db, "asymmetry2");
  }
};

//*****************************************************************

void QwHelicityPattern::Print() const
{
  QwOut << "Pattern number = " << fCurrentPatternNumber << QwLog::endl;
  for (Int_t i = 0; i < fPatternSize; i++)
    QwOut << "Event " << fEventNumber[i] << ": "
          << fEventLoaded[i] << ", " << fHelicity[i] << QwLog::endl;
  QwOut << "Is a complete pattern? (n/y:0/1) " << IsCompletePattern() << QwLog::endl;
}
