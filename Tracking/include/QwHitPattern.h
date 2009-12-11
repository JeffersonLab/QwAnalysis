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
 *   (1UL << levels) == 2^levels
 *   (1UL << (levels - 1)) == 2^(levels-1)
 * For e.g. 4 levels we need 1 + 2 + 4 + 8 = 15 = (2^4 - 1) bits.
 */
class QwHitPattern: public VQwTrackingElement {

  public:

    /// \brief Create a hit pattern with specified number of layers and depth
    QwHitPattern(unsigned int layers, unsigned int levels) {
      fLayers = layers;
      fLevels = levels;
      fBins = new unsigned int[fLayers];
      fHash = new unsigned int[fLayers];
      fChannel = new char*[fLayers];
      fHashChannel = new int*[fLayers];
      for (unsigned int layer = 0; layer < fLayers; layer++) {
        fBins[layer] = (1UL << fLevels); // i.e. 2^fLevels
        fHash[layer] = (1UL << (fLevels - 1)); // i.e. 2^(fLevels-1)
        fChannel[layer] = new char[fBins[layer]];
        fHashChannel[layer] =  new int[fHash[layer]];
      }
    };

    /// \brief Delete the hit pattern
    ~QwHitPattern() {
      for (unsigned int layer = 0; layer < fLayers; layer++) {
        delete[] fHashChannel[layer];
        delete[] fChannel[layer];
      }
      delete[] fHashChannel;
      delete[] fChannel;
      delete[] fHash; delete[] fBins;
    };

    /// \brief Reset the contents of the hit pattern
    void Reset() {
      for (unsigned int layer = 0; layer < fLayers; layer++) {
        for (unsigned int bin = 0; bin < fBins[layer]; bin++)
          fChannel[layer][bin] = 0;
        for (unsigned int hash = 0; hash < fHash[layer]; hash++)
          fHashChannel[layer][hash] = 0;
      }
    };

  private:

    unsigned int fLayers; ///< Number of tracking layers
    unsigned int fLevels; ///< Depth of the tree search

    unsigned int* fBins;
    unsigned int* fHash;

    char** fChannel;
    int** fHashChannel;

  friend ostream& operator<< (ostream& stream, const QwHitPattern& hitpattern);

}; // class QwHitPattern

inline ostream& operator<< (ostream& stream, const QwHitPattern& hitpattern)
{
  for (unsigned int layer = 0; layer < hitpattern.fLayers; layer++) {
    for (unsigned int bin = 0; bin < (1UL << hitpattern.fLevels) - 1; bin++) {
      if (hitpattern.fChannel[layer][bin] == 1)
          stream << "|";
      else
          stream << ".";
    }
    stream << std::endl;
  }
  return stream;
}

#endif // QWHITPATTERN_H
