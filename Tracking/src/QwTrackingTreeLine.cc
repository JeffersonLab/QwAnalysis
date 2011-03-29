/**
 * \file   QwTrackingTreeLine.cc
 * \brief  Definition of the one-dimensional track stubs
 *
 * \author Wouter Deconinck <wdconinc@mit.edu>
 * \author Jie Pan <jpan@jlab.org>
 * \date   Sun May 24 11:05:29 CDT 2009
 */

#include "QwTrackingTreeLine.h"
ClassImp(QwTrackingTreeLine)

// System headers
#include <cmath>

// Qweak headers
#include "QwHit.h"
#include "QwHitPattern.h"
#include "QwHitContainer.h"


/// Default constructor
QwTrackingTreeLine::QwTrackingTreeLine()
{
  // Initialize
  Initialize();
}


/// Constructor with tree search results
QwTrackingTreeLine::QwTrackingTreeLine(int _a_beg, int _a_end, int _b_beg, int _b_end)
{
  // Initialize
  Initialize();

  // Tree search
  a_beg = _a_beg;
  a_end = _a_end;
  b_beg = _b_beg;
  b_end = _b_end;
}


/// Copy constructor
QwTrackingTreeLine::QwTrackingTreeLine(const QwTrackingTreeLine& that)
: VQwTrackingElement(that)
{
  // Initialize
  Initialize();

  // Copy object
  *this = that;
}

/// Copy constructor
QwTrackingTreeLine::QwTrackingTreeLine(const QwTrackingTreeLine* that)
: VQwTrackingElement(*that)
{
  // Initialize
  Initialize();

  // Null pointer
  if (that == 0) return;

  // Copy object
  *this = *that;
}


/**
 * Delete the tree line and the lists of hits depending on it
 */
QwTrackingTreeLine::~QwTrackingTreeLine()
{
  // Delete the hits in this treeline
  for (int i = 0; i < 2 * MAX_LAYERS; i++) {

    if (fHits[i]) delete fHits[i];
    fHits[i] = 0;

    if (fUsedHits[i]) delete fUsedHits[i];
    fUsedHits[i] = 0;
  }

  DeleteHits();

  // No recursive delete of the next pointer is done here
}


/**
 * Perform object initialization
 */
void QwTrackingTreeLine::Initialize()
{
  // Clear the list of hits
  ClearHits();

  // Reset the void and used flags
  fIsVoid = false;      // treeline is not void yet
  fIsUsed = false;      // treeline is not part of a partial track yet

  // Tree search
  a_beg = 0;
  a_end = 0;
  b_beg = 0;
  b_end = 0;

  next = 0;             // no next element yet in linked-list

  fOffset = fSlope = 0.0;
  fChi = 0.0;

  for (int i = 0; i < 3; i++)
    fCov[i] = 0.0;

  fNumHits = 0;
  fNumMiss = 0;

  for (int i = 0; i < 2 * MAX_LAYERS; i++) {
    fHits[i] = 0;
    fUsedHits[i] = 0;
    fHashArray[i] = 0;
  }

  fR3Offset = fR3FirstWire = fR3LastWire = 0;
}


/**
 * Assignment operator
 */
QwTrackingTreeLine& QwTrackingTreeLine::operator=(const QwTrackingTreeLine& that)
{
  if (this == &that) return *this;

  VQwTrackingElement::operator=(that);

  fMatchingPattern = that.fMatchingPattern;

  fOffset = that.fOffset;
  fSlope = that.fSlope;
  fChi = that.fChi;
  fAverageResidual=that.fAverageResidual;
  for (size_t i = 0; i < 3; i++)
    fCov[i] = that.fCov[i];

  a_beg = that.a_beg;
  a_end = that.a_end;
  b_beg = that.b_beg;
  b_end = that.b_end;

  fNumHits = that.fNumHits;
  fNumMiss = that.fNumMiss;

  fR3Offset = that.fR3Offset;
  fR3FirstWire = that.fR3FirstWire;
  fR3LastWire  = that.fR3LastWire;

  SetPackage(that.GetPackage());
  SetRegion(that.GetRegion());
  SetPlane(that.GetPlane());
  SetDirection(that.GetDirection());

  next = 0;

  // Copy the hits
  for (int i = 0; i < 2 * MAX_LAYERS; i++) {

    fHashArray[i] = that.fHashArray[i];

    if (that.fHits[i]) {
      this->fHits[i] = new QwHit(that.fHits[i]);
    }
    if (that.fUsedHits[i]) {
      this->fUsedHits[i] = new QwHit(that.fUsedHits[i]);
    }
  }

  // Copy hits
  ClearHits();
  AddHitList(that.fQwHits);

  return *this;
}


// Clear the list of hits
void QwTrackingTreeLine::ClearHits()
{
  fQwHits.clear();
  fNQwHits = 0;
}

// Delete the hits in the list
void QwTrackingTreeLine::DeleteHits()
{
  for (size_t i = 0; i < fQwHits.size(); i++)
    delete fQwHits.at(i);
  ClearHits();
}

// Add a single hit
void QwTrackingTreeLine::AddHit(const QwHit* hit)
{
  if (hit) fQwHits.push_back(new QwHit(hit));
  fNQwHits++;
}

// Add a list of hits
void QwTrackingTreeLine::AddHitList(const std::vector<QwHit*> &hitlist)
{
  for (std::vector<QwHit*>::const_iterator hit = hitlist.begin();
       hit != hitlist.end(); hit++)
    AddHit(*hit);
}

// Add the hits of a QwHitContainer to the TClonesArray
void QwTrackingTreeLine::AddHitContainer(QwHitContainer* hitlist)
{
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    QwHit* p = &(*hit);
    AddHit(p);
  }
}

// Get the hits from the TClonesArray to a QwHitContainer
QwHit* QwTrackingTreeLine::GetHit(int i)
{
  if (fNQwHits == 0 ||  i >= fNQwHits)
    return 0;
  else
    return fQwHits.at(i);
}

// Get the hits from the TClonesArray to a QwHitContainer
QwHitContainer* QwTrackingTreeLine::GetHitContainer()
{
  QwHitContainer* hitlist = new QwHitContainer();
  for (std::vector<QwHit*>::const_iterator hit = fQwHits.begin();
       hit != fQwHits.end(); hit++)
    if (*hit) hitlist->push_back(*hit);
    else QwError << "null hit in hit list" << QwLog::endl;
  return hitlist;
}


/**
 * Determine the chi^2 for a tree line, weighted by the number of hits.
 *
 * @return Weighted chi^2
 */
double QwTrackingTreeLine::GetChiWeight ()
{
  double weight;
  // NOTE Added +1 to get this to work if fNumHits == fNumMiss (region 2 cosmics)
  if (fNumHits >= fNumMiss)
    weight = (double) (fNumHits + fNumMiss + 1)
                    / (fNumHits - fNumMiss + 1);
  else {
    std::cerr << "miss = " << fNumMiss << ", hits = " << fNumHits << std::endl;
    return 100000.0; // This is bad...
  }
  return weight * fChi;
}

/**
 * Determine the hit with the smallest drift distance, i.e. a first order
 * estimate of the crossing of the track with the central wire plane.
 * @param offset Optional offset to the position
 * @return Hit with smallest drift distance
 */
QwHit* QwTrackingTreeLine::GetBestWireHit (double offset)
{
  double best_position = 9999.9;
  int best_hit = 0;
  // Get the best measured hit in the back
  for (int hit = 0; hit < fNumHits; hit++) {
    double position = fabs(fHits[hit]->GetDriftPosition() - offset);
    if (position < best_position) {
      best_position = position;
      best_hit = hit;
    }
  }
  return fHits[best_hit];
}

/**
 * Calculate average residual of this partial track over all treelines
 * @return Average residual
 */
double QwTrackingTreeLine::CalculateAverageResidual()
{
  int numHits = 0;
  double sumResiduals = 0.0;
  for (int layer = 0; layer < 2 * MAX_LAYERS; layer++) {
    for (QwHit* hit = fHits[layer]; hit; hit = hit->next) {
      if (hit->IsUsed()) {
//         std::cout << "in calculate residual: " << hit->GetTrackPosition() << " " << hit->GetDriftPosition() <<  std::endl;
        
        double residual = hit->GetResidual();
        sumResiduals += residual;
        numHits++;
      }
    } // end of loop over hits (only one of them is used)
  } // end of loop over layers
  fAverageResidual = sumResiduals / numHits;
  return fAverageResidual;
}



/**
 * Print the tree line in a linked list
 */
void QwTrackingTreeLine::Print(const Option_t* options) const {
  if (!this) return;
  std::cout << *this << std::endl;
  if (next) next->Print(options);
}


/**
 * Print the valid tree lines in a linked list
 */
void QwTrackingTreeLine::PrintValid() {
  if (!this) return;
  if (this->IsValid()) std::cout << *this << std::endl;
  next->PrintValid();
}


/**
 * Stream some info about the tree line
 * @param stream Stream as lhs of the operator
 * @param tl Tree line as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const QwTrackingTreeLine& tl) {
  stream << "tl: ";
  if (tl.a_beg + tl.a_end + tl.b_beg + tl.b_end != 0) {
    stream << tl.a_beg << "," << tl.a_end << " -- ";
    stream << tl.b_beg << "," << tl.b_end << " ";
  }
  if (tl.GetRegion() != kRegionIDNull) { // treeline has geometry identification
    stream << "(" << tl.GetRegion() << "/" << "?UD"[tl.GetPackage()];
    stream << "/" << "?xyuvrq"[tl.GetDirection()];
    if (tl.GetPlane() > 0)
      stream << "/" << tl.GetPlane() << ")";
    else
      stream << ")";
  }
  if (tl.fChi > 0.0) { // treeline has been fitted
    stream << "; fOffset = " << tl.fOffset/Qw::cm << " cm";
    stream << ", fSlope = " << tl.fSlope;
    stream << ", fResidual = " << tl.fAverageResidual/Qw::cm << " cm";
    stream << ", fChi = " << tl.fChi;
    stream << "; hits (" << tl.fQwHits.size() << "):";
    for (size_t hit = 0; hit < tl.fQwHits.size(); hit++)
      stream << " " << tl.fQwHits.at(hit)->GetPlane() << ":" << tl.fQwHits.at(hit)->GetElement();
  }
  if (tl.IsVoid()) stream << " (void)";
  return stream;
}

void QwTrackingTreeLine::SetMatchingPattern(std::vector<int>& box)
{
  std::vector<int>::iterator iter = box.begin();
  while (iter != box.end())
    fMatchingPattern.push_back(*iter++);
}

std::pair<double,double> QwTrackingTreeLine::CalculateDistance(int row,double width,unsigned int bins,double resolution)
{
  std::pair<double,double> boundary(0,0);
  int bin = fMatchingPattern.at(row);

  double dx = width / bins, lower = 0, upper = 0;
  bin += 1;
  if (bin <= bins/2) bin = bins - bin + 1;
    lower = (bin - 1) * dx - width / 2 - resolution;
    upper = bin * dx - width / 2 + resolution;
  boundary.first = lower;
  boundary.second = upper;

  return boundary;
}

