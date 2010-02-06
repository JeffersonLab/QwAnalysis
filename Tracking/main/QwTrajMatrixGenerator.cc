/*------------------------------------------------------------------------*//*!

 \file QwTrajMatrixGenerator.cc \ingroup QwTracking

 \author J. Pan
 \date Thu Dec  3 15:22:20 CST 2009

 \brief Generate a momentum look-up table indexed by momentum, position and direction

  A family of trajectories starting from the target to endplane will be generated.
  Momentum and scattering direction vary for each trajectory. We record the inter-
  section position and direction at the focal plane (z=570 cm)

*//*-------------------------------------------------------------------------*/

// Z coordinate:
// Z0: start plane = -250 cm, Z1: endplane = +250 cm, Z2: focalplane = +570 cm
// B field values are available from z=-250 cm to z=250 cm
//

// Qweak Tracking headers
#include "Det.h"
#include "QwRayTracer.h"


int main (int argc, char* argv[]) {

    QwRayTracer* raytracer = new QwRayTracer();
    raytracer->LoadMagneticFieldMap();

    raytracer->GenerateLookUpTable();

    delete raytracer;

    return 0;

}

