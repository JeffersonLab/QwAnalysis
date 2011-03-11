#include "QwVertex.h"

#include "QwTrack.h"

QwVertex::QwVertex()
{
  // TODO
}

QwVertex::QwVertex(const TVector3& position)
{
  fCoord[0] = position.X();
  fCoord[1] = position.Y();
  fCoord[2] = position.Z();
}

QwVertex::QwVertex(const QwTrack& track1, const QwTrack& track2)
{
  // TODO
}

QwVertex::QwVertex(double x, double y, double z)
{
  fCoord[0] = x;
  fCoord[1] = y;
  fCoord[2] = z;
}
