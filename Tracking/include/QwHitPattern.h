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
#include "QwObjectCounter.h"

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
class QwHitPattern: public VQwTrackingElement, public QwObjectCounter<QwHitPattern> {

  public:

    /// \brief Default constructor
    QwHitPattern()
    : fLevels(0),fBins(0),fBinWidth(0),fPattern(0),fPatternHash(0)
    { };
    /// \brief Constructor with hit pattern depth
    QwHitPattern(const unsigned int levels)
    : fLevels(0),fBins(0),fBinWidth(0),fPattern(0),fPatternHash(0) {
      SetNumberOfLevels(levels);
      Reset();
    };
    /// \brief Copy constructor
    QwHitPattern(const QwHitPattern& pattern)
    : VQwTrackingElement(pattern),QwObjectCounter<QwHitPattern>(pattern) {
      *this = pattern;
    };

    /// \brief Delete the hit pattern
    virtual ~QwHitPattern() {
      if (fLevels == 0) return;
      if (fPatternHash) delete[] fPatternHash;
      if (fPattern)     delete[] fPattern;
    };

    /// \brief Set the hit pattern depth
    void SetNumberOfLevels(const unsigned int levels) {
      fLevels = levels;
      if (fLevels == 0) return;
      fBins = (1UL << fLevels); // total number of bins, i.e. 2^fLevels
      fBinWidth = (1UL << (fLevels - 1)); // maximum bin division, i.e. 2^(fLevels-1)
      fPattern = new unsigned char[fBins];
      fPatternHash =  new unsigned int[fBinWidth];
    };
    /// \brief Get the hit pattern depth
    unsigned int GetNumberOfLevels() const { return fLevels; };
    /// \brief Get the number of bins
    unsigned int GetNumberOfBins() const { return fBins; };
    /// \brief Get the finest bin width
    unsigned int GetFinestBinWidth() const { return fBinWidth; };

    /// \brief Reset the contents of the hit pattern
    void Reset() {
      for (unsigned int bin = 0; bin < fBins; bin++)
        fPattern[bin] = 0;
      for (unsigned int hash = 0; hash < fBinWidth; hash++)
        fPatternHash[hash] = 0;
    };

    /// \brief Set the hit pattern bins for the specified HDC-type hit
    void SetHDCHit(double detectorwidth, QwHit* hit);
    /// \brief Set the hit pattern bins for the specified VDC-type hit
    void SetVDCHit(double detectorwidth, QwHit* hit);
    /// \brief Set the hit pattern bins for the specified HDC-type hit list
    void SetHDCHitList(double detectorwidth, QwHitContainer* hitlist);
    /// \brief Set the hit pattern bins for the specified VDC-type hit list
    void SetVDCHitList(double detectorwidth, QwHitContainer* hitlist);

    /// \brief Has this pattern any hit?
    bool HasHits() const {
      if (fPattern == 0) return false;
      else return fPattern[fBins-2] == 1;
    };

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

    /// \brief Assignment operator
    QwHitPattern& operator=(const QwHitPattern& rhs);
    /// \brief Addition-assignment operator
    QwHitPattern& operator+=(const QwHitPattern& rhs);

  private:

    /// \brief Recursive tree pattern method on a point with resolution
    void _SetPoint (double position, double resolution, double detectorwidth);

    /// \brief Recursive tree pattern method on a range of points
    void _SetPoints (double pos_start, double pos_end, double detectorwidth);


    unsigned int fLevels;               ///< Depth of the tree search

    unsigned int fBins;                 ///< Number of bins
    unsigned int fBinWidth;             ///< Width of each bin

    unsigned char* fPattern;	 //!	///< Hit pattern
    unsigned int*  fPatternHash; //!	///< Hash of the hit pattern

  friend std::ostream& operator<< (std::ostream& stream, const QwHitPattern& hitpattern);

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwHitPattern,1);
  #endif

}; // class QwHitPattern

#endif // QWHITPATTERN_H
