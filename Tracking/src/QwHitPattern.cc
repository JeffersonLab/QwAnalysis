/*!
 * \file   QwHitPattern.cc
 * \brief  Implementation of the hit patterns used in the tracking tree search
 *
 * \author Wouter Deconinck
 * \date   2009-12-11
 */

#include "QwHitPattern.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwHitPattern)
#endif

// Qweak headers
#include "QwHit.h"
#include "QwHitContainer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

static unsigned int static_hashgen = 1;

/**
 * Create a hash key
 * @return A new hash key
 */
static int hashgen()
{
  static_hashgen += 2;
  static_hashgen &= 0x7ffffff;
  return static_hashgen;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Assignment operator for hit patterns.
 * @param that Right hand side
 * @return Left hand side
 */
QwHitPattern& QwHitPattern::operator= (const QwHitPattern& that)
{
  if (this == &that) return *this;

  VQwTrackingElement::operator=(that);

  SetNumberOfLevels(that.fLevels);
  // If lhs is zero we assign that and return
  if (fLevels == 0) return *this;
  // Copy the levels and pattern data
  for (unsigned int bin = 0; bin < fBins; bin++)
    fPattern[bin] = that.fPattern[bin];
  for (unsigned int hash = 0; hash < fBinWidth; hash++)
    fPatternHash[hash] = that.fPatternHash[hash];
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Addition-assignment operator for hit patterns.  This is important for
 * multiple hits on a single wire.
 *
 * TODO figure out how this affects region 2 and region 3
 *
 * Requirements are:
 *   hitpattern += 0 == hitpattern
 *   0 += hitpattern == hitpattern
 *   hitpattern1 += hitpattern2 == hitpattern as obtained by TreeSearch code
 *
 * The hash pattern is not so important, but the hit pattern has to match!
 *
 * @param that Right hand side
 * @return Left hand side
 */
QwHitPattern& QwHitPattern::operator+= (const QwHitPattern& that)
{
  // If lhs is zero we assign that and return
  if (fLevels == 0) return *this = that;
  // Check whether levels is equal
  if (fLevels == that.fLevels) {
    for (unsigned int bin = 0; bin < fBins; bin++)
      fPattern[bin] |= that.fPattern[bin];
  }
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Set the hit pattern bins for the specified HDC-type hit list.
 *
 * @param detectorwidth Width of the detector
 * @param hitlist Hit list
 */
void QwHitPattern::SetHDCHitList(
	const double detectorwidth,
	QwHitContainer* hitlist)
{
  QwHitContainer::iterator end=hitlist->end();
  for (QwHitContainer::iterator hit  = hitlist->begin();
                                hit != end; ++hit){
    if(hit->GetDriftDistance()<0 || hit->GetHitNumber()!=0) continue;
    SetHDCHit(detectorwidth,&(*hit));
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Set the hit pattern bins for the specified VDC-type hit list.
 *
 * @param detectorwidth Width of the detector
 * @param hitlist Hit list
 */
void QwHitPattern::SetVDCHitList(
	const double detectorwidth,
	QwHitContainer* hitlist)
{
  for (QwHitContainer::iterator hit  = hitlist->begin();
                                hit != hitlist->end(); hit++)
    SetVDCHit(detectorwidth, &(*hit));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Set the hit pattern bins for the specified HDC-type hit.
 *
 * The planes are treated as individual tree-detectors.  In this case,
 * a separate hit pattern is created for both planes and the above described
 * searching for paired hits is not employed.  (This method is the standard
 * for the front chambers since they have only four planes per treeline).
 *
 * @param detectorwidth Width of the detector
 * @param hit Hit
 */
void QwHitPattern::SetHDCHit (
	double detectorwidth,
	QwHit* hit)
{
  // Get the wire number
  int wire = hit->GetElement();
  double drift_distance = hit->GetDriftDistance();
  double wire_spacing = hit->GetDetectorInfo()->GetElementSpacing();
  double track_resolution = hit->GetDetectorInfo()->GetTrackResolution();
  

  // Set the points on the front/bottom side of the wire (R3/R2)
  //NOTE:why wire+1,not wire?
//   std::cout << "r2 bottom: " << hit->GetDirection() << " " << hit->GetPlane() << std::endl;
  _SetPoints(wire_spacing * (wire-0.5) - drift_distance - track_resolution,
             wire_spacing * (wire-0.5) - drift_distance + track_resolution,
             detectorwidth);

//   std::cout << "r2 up: " << hit->GetDirection() << " " << hit->GetPlane() << std::endl;  
  // Set the points on the back/top side of the wire (R3/R2)
  _SetPoints(wire_spacing * (wire-0.5) + drift_distance - track_resolution,
             wire_spacing * (wire-0.5) + drift_distance + track_resolution,
             detectorwidth);


  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Set the hit pattern bins for the specified VDC-type hit.
 *
 * (TODO Need to elaborate on how this is different exactly)
 *
 * @param detectorwidth Width of the detector
 * @param hit Hit
 */
void QwHitPattern::SetVDCHit (
	double detectorwidth,
	QwHit *hit)
{
  double halfwidth = (detectorwidth / 2.0); /* half-width of the detector (in cm) */
  double drift_distance = hit->GetDriftDistance();
  double track_resolution = hit->GetDetectorInfo()->GetTrackResolution();

  // Set the points on the front/top side of the wire (R3/R2)
  _SetPoints(halfwidth - drift_distance - track_resolution,
             halfwidth - drift_distance + track_resolution,
             detectorwidth);

  // Set the points on the back/bottom side of the wire (R3/R2)
  _SetPoints(halfwidth + drift_distance - track_resolution,
             halfwidth + drift_distance + track_resolution,
             detectorwidth);

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Set the bins in the hit pattern for a range of positions around a central
 * point within a specified distance/resolution by calling the setpoint()
 * function.
 *
 * @param position Central position
 * @param resolution Resolution
 * @param detectorwidth Width of the detector
 */
void QwHitPattern::_SetPoint (
	double position,
	double resolution,
	double detectorwidth)
{
  _SetPoints (position - resolution, position + resolution, detectorwidth);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Set the bins in a hit pattern for a range of positions.  The range of
 * hit patterns is specified by a start and a stop position in the detector.
 * This function turns on the bins in the hit pattern for each level of
 * the bin-division used in the tree search algorithm.
 *
 * @param pos_start Start position
 * @param pos_end End position
 * @param detectorwidth Width of the detector
 */
void QwHitPattern::_SetPoints (
	double pos_start,
	double pos_end,
	double detectorwidth)
{
  int ia, ie;
  int hashint = hashgen();
  unsigned int binwidth = fBinWidth;
  unsigned char* pattern = fPattern;

  
/* ---- compute the first bin in the deepest tree level to turn on     ---- */

  ia = (int) floor (pos_start / detectorwidth * fBinWidth);

  
/* ---- compute the last bin in the deepest tree level to turn on      ---- */

  ie = (int) floor (pos_end   / detectorwidth * fBinWidth);

/* ---- step through each of the bins at the deepest bin-division
        level in the hit pattern and turn on the bits in the
        pattern at all the bin-division levels for this bin.          ---- */

  for (int j = ia; j <= ie; j++) { /* loop over the bins to be set */

    int i = j; /* remember the bin at the deepest level which is turned on */

    binwidth = fBinWidth;   /* the size of the bit pattern for the detector
                               at the deepest level of bin-division.       */
    pattern  = fPattern;    /* pointer to start of the bit pattern         */

/* ---- check if the bin is inside the detector                       ---- */
    if (i >= (signed int) binwidth)
      return;
    if (i < 0)
      continue;

/* ---- compute the hash value                                        ---- */
    //NOTE:why like this?
    fPatternHash[i] = ((fPatternHash[i] << 1) + hashint) | 1;
//     std::cout << "fPatternHash[" << i << "]=" << fPatternHash[i] << std::endl; 
/* ---- set the bits in the bit pattern at all depths of bin-division ---- */
    if (pattern) {
      while (binwidth) {         /* starting at maximum depth, loop over
                                    each depth of the treesearch           */
//         if(binwidth==1) std::cout << "set i" << i << " " << pos_start << std::endl;
        pattern[i] = 1;          /* turn on the bit in this bin            */
        pattern   += binwidth;   /* set ahead to the part of the bit
                                    pattern in which the bits for the
                                    next higher bin-division are stored    */
        i        >>= 1;          /* go up one depth of the depth           */
        binwidth >>= 1;          /* size of the bit pattern for the next
                                    higher bin-division is half of the
                                    size of the bit pattern for a given
                                    level of bin-division.                 */

      } /* end of loop over the depths of the treesearch */
    }
  } /* end of loop over the bins to be set */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Stream the hit pattern to an std::ostream.
 */
std::ostream& operator<< (std::ostream& stream, const QwHitPattern& hitpattern)
{
  for (unsigned int bin = 0; bin < hitpattern.fBins - 1; bin++) {
    if (hitpattern.fPattern[bin] == 1)
        stream << "|";
    else
        stream << ".";
  }
  return stream;
}

