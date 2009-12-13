/*!
 * \file   QwHitPattern.h
 * \brief  Definition of the hit patterns used in the tracking tree search
 *
 * \author Wouter Deconinck
 * \date   2009-12-08
 */

#ifndef QWHITPATTERN_H
#define QWHITPATTERN_H

// System headers
#include <iostream>

// Qweak headers
#include "VQwTrackingElement.h"

// Forward declarations
class QwHit;
class QwHitContainer;

/**
 *  \class QwHitPattern
 *  \ingroup QwTracking
 *  \brief Hit patterns used in the tracking tree search
 *
 * The hit patterns used in the tree search have a rather peculiar format.
 * Each level of depth in the pattern has a resolution that is twice as good
 * as the previous level.  The top level has a resolution of one, meaning that
 * one bit indicates whether a track is present or not.  The second level has
 * two bits, etc.
 *
 * A pattern is stored for every layer of the tracking detector (or every wire
 * in the case of the VDCs, where the interpretation is rotated over 90 degrees).
 *
 * The options file indicates the number of tree levels (how many times we
 * go to finer binning).  The total number of bits is determined using
 * bit shift operators:
 *   Total number of bins + 1 = (1UL << levels) == 2^levels
 *   Number of bins at bottom = (1UL << (levels - 1)) == 2^(levels-1)
 * For e.g. 4 levels we need 1 + 2 + 4 + 8 = 15 = (2^4 - 1) bits.
 */
class QwHitPattern: public VQwTrackingElement {

  public:

    /// \brief Default constructor
    QwHitPattern():fLevels(0),fBins(0),fBinWidth(0),fPattern(0),fPatternHash(0) { };
    /// \brief Constructor with hit pattern depth
    QwHitPattern(const unsigned int levels) {
      SetNumberOfLevels(levels);
      Reset();
    };
    /// \brief Copy constructor
    QwHitPattern(const QwHitPattern& pattern) {
      SetNumberOfLevels(pattern.fLevels);
      for (unsigned int bin = 0; bin < fBins; bin++)
        fPattern[bin] = pattern.fPattern[bin];
      for (unsigned int hash = 0; hash < fBinWidth; hash++)
        fPatternHash[hash] = pattern.fPatternHash[hash];
    };

    /// \brief Delete the hit pattern
    ~QwHitPattern() {
      if (fPatternHash) delete[] fPatternHash;
      if (fPattern)     delete[] fPattern;
    };

    /// \brief Set the hit pattern depth
    void SetNumberOfLevels(const unsigned int levels) {
      if (levels == 0) return;
      fLevels = levels;
      fBins = (1UL << fLevels); // total number of bins, i.e. 2^fLevels
      fBinWidth = (1UL << (fLevels - 1)); // maximum bin division, i.e. 2^(fLevels-1)
      fPattern = new char[fBins];
      fPatternHash =  new int[fBinWidth];
    };
    /// \brief Get the hit pattern depth
    const unsigned int GetNumberOfLevels() const { return fLevels; };
    /// \brief Get the number of bins
    const unsigned int GetNumberOfBins() const { return fBins; };
    /// \brief Get the finest bin width
    const unsigned int GetFinestBinWidth() const { return fBinWidth; };

    /// \brief Reset the contents of the hit pattern
    void Reset() {
      for (unsigned int bin = 0; bin < fBins; bin++)
        fPattern[bin] = 0;
      for (unsigned int hash = 0; hash < fBinWidth; hash++)
        fPatternHash[hash] = 0;
    };

    /// \brief Set the hit pattern bins for the specified hit
    void SetHit(double detectorwidth, QwHit* hit);
    /// \brief Set the hit pattern bins for the specified hit list
    void SetHitList(double detectorwidth, QwHitContainer* hitlist);

    /// \brief Get the hit pattern
    void GetPattern(char* pattern) const {
      for (unsigned int bin = 0; bin < fBins; bin++)
        pattern[bin] = fPattern[bin];
    };
    /// \brief Get the hit pattern hash
    void GetPatternHash(int* patternhash) const {
      for (unsigned int hash = 0; hash < fBinWidth; hash++)
        patternhash[hash] = fPatternHash[hash];
    };

  private:

    /// \brief Recursive tree pattern method on a point with resolution
    void _SetPoint (double position, double resolution, double detectorwidth);

    /// \brief Recursive tree pattern method on a range of points
    void _SetPoints (double pos_start, double pos_end, double detectorwidth);


    unsigned int fLevels; ///< Depth of the tree search

    unsigned int fBins;
    unsigned int fBinWidth;

    char* fPattern;
    int*  fPatternHash;

  friend ostream& operator<< (ostream& stream, const QwHitPattern& hitpattern);

  ClassDef(QwHitPattern,1);

}; // class QwHitPattern

#endif // QWHITPATTERN_H
