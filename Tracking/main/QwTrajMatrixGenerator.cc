/*------------------------------------------------------------------------*//*!

 \file QwTrajMatrixGenerator.cc \ingroup QwTracking

 \author J. Pan
 \date Thu Dec  3 15:22:20 CST 2009

 \brief Generate a momentum look-up table indexed by momentum, position and direction

*//*-------------------------------------------------------------------------*/

// Qweak Tracking headers
#include "QwTrajMatrix.h"

int main (int argc, char* argv[])
{
    QwTrajMatrix trajmatrix;
    trajmatrix.GenerateTrajMatrix();

    return 0;
}

