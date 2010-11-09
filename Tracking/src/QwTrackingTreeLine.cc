/**
 * \file   QwTrackingTreeLine.cc
 * \brief  Definition of the one-dimensional track stubs
 *
 * \author Wouter Deconinck <wdconinc@mit.edu>
 * \author Jie Pan <jpan@jlab.org>
 * \date   Sun May 24 11:05:29 CDT 2009
 */

#include "QwTrackingTreeLine.h"
ClassImp(QwTrackingTreeLine);

// System headers
#include <cmath>

// Qweak headers
#include "QwHit.h"
#include "QwHitPattern.h"
#include "QwHitContainer.h"


// Initialize the static list of hits
//TClonesArray* QwTrackingTreeLine::gQwHits = 0;


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
/// \todo TODO (wdc) this copy constructor is horribly incomplete!
QwTrackingTreeLine::QwTrackingTreeLine(const QwTrackingTreeLine* treeline)
{
  // Initialize
  Initialize();

  // Naive copy
  *this = *treeline;

  // Copy the hits
  for (int i = 0; i < 2 * MAX_LAYERS; i++) {
    if (treeline->hits[i])
      this->hits[i] = new QwHit(treeline->hits[i]);
  }
}


/**
 * Delete the tree line and the lists of hits depending on it
 */
QwTrackingTreeLine::~QwTrackingTreeLine()
{
  // Delete the hits in this treeline
  for (int i = 0; i < 2 * MAX_LAYERS; i++) {
    if (hits[i]) delete hits[i];
  }
}


/**
 * Perform object initialization
 */
void QwTrackingTreeLine::Initialize()
{
  // Create the static TClonesArray for the hits if not existing yet
  //if (! gQwHits)
  //  gQwHits = new TClonesArray("QwHit", QWTREELINE_MAX_NUM_HITS);
  // Set local TClonesArray to static TClonesArray and zero hits
  //fQwHits = gQwHits;

  //fQwHits = new TRefArray();

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

  for (int i = 0; i < MAX_LAYERS; i++) {
    usedhits[i] = 0;
  }
  for (int i = 0; i < 2 * MAX_LAYERS; i++) {
    hits[i] = 0;        /*!< hitarray */
    hasharray[i] = 0;
  }

  ID = 0;
  fR3Offset = fR3FirstWire = fR3LastWire = 0;
}


// Clear the local TClonesArray of hits
void QwTrackingTreeLine::ClearHits(Option_t *option)
{
  fQwHits.clear(); // Clear the list of hits
  fNQwHits = 0; // No hits in the list hits
};

// Delete the static TClonesArray of hits
void QwTrackingTreeLine::ResetHits(Option_t *option)
{
  //delete gQwHits;
  //gQwHits = 0;
};

// Create a new QwHit
QwHit* QwTrackingTreeLine::CreateNewHit()
{
  //TClonesArray &hits = *fQwHits;
  //QwHit *hit = new (hits[fNQwHits++]) QwHit();
  //return hit;
  QwHit* hit = new QwHit();
  AddHit(hit);
  return hit;
};

// Add an existing QwHit
void QwTrackingTreeLine::AddHit(QwHit* hit)
{
  //QwHit* newhit = CreateNewHit();
  //*newhit = *hit;
  fQwHits.push_back(hit);
  fNQwHits++;
};

// Add the hits of a QwHitContainer to the TClonesArray
void QwTrackingTreeLine::AddHitContainer(QwHitContainer* hitlist)
{
  ClearHits();
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    QwHit* p = &(*hit);
    AddHit(p);
  }
}

// Get the number of hits
Int_t QwTrackingTreeLine::GetNumberOfHits() const
{
  return fQwHits.size();
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
  //TIterator* iterator = fQwHits->MakeIterator();
  //QwHit* hit = 0;
  //while ((hit = (QwHit*) iterator->Next())) {
  //  hitlist->push_back(*hit);
  //}
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
    double position = fabs(hits[hit]->GetDriftPosition() - offset);
    if (position < best_position) {
      best_position = position;
      best_hit = hit;
    }
  }
  return hits[best_hit];
}

/**
 * Calculate average residual of this partial track over all treelines
 * @return Average residual
 */
const double QwTrackingTreeLine::CalculateAverageResidual()
{
  int numHits = 0;
  double sumResiduals = 0.0;
  for (int layer = 0; layer < 2 * MAX_LAYERS; layer++) {
    for (QwHit* hit = hits[layer]; hit; hit = hit->next) {
      if (hit->IsUsed()) {
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
void QwTrackingTreeLine::Print() {
  if (!this) return;
  std::cout << *this << std::endl;
  next->Print();
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
};

void QwTrackingTreeLine::SetMatchingPattern(std::vector<int>& box)
{
	std::vector<int>::iterator iter=box.begin();
	while(iter!=box.end())
		MatchingPattern.push_back(*iter++);
};

std::pair<double,double> QwTrackingTreeLine::CalculateDistance(int row,double width,unsigned int bins,double resolution)
{
   std::pair<double,double> boundary(0,0);
   unsigned int fBins=bins;
   int bin=MatchingPattern.at(row);

	
   double dx=width/bins,lower=0,upper=0;
   bin+=1;
   if(bin<=bins/2) bin=bins-bin+1;
	lower=(bin-1)*width/bins-width/2-resolution;
	upper=bin*width/bins-width/2+resolution;
   boundary.first=lower;
   boundary.second=upper;
   	
   return boundary;
}