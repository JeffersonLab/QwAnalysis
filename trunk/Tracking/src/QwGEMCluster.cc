#include "QwGEMCluster.h"
ClassImp(QwGEMCluster)

/**
 * Output stream operator
 * @param stream Stream
 * @param cluster Cluster object
 * @return Output stream
 */
ostream& operator<< (ostream& stream, const QwGEMCluster& cluster)
{
  stream << "cluster: ";
  stream << "package "   << "?UD"[cluster.GetPackage()] << ", ";
  stream << "region "    << "123"[cluster.GetRegion()] << ", ";
  stream << "dir "       << "?xyuvrf"[cluster.GetDirection()] << ", ";
  stream << "plane "     << cluster.GetPlane();

  if (cluster.GetDetectorInfo()) stream << " (detector " << cluster.GetDetectorInfo() << "), ";
  else                           stream << ", ";

  stream << "strip "  << cluster.fStripMean << " +/- " << cluster.fStripSigma << ", ";
  stream << "position " << cluster.fPosition << " +/- " << cluster.fUncertainty;

  return stream;
}
