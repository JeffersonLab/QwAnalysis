#ifndef BEAMLINE_H
#define BEAMLINE_H

#include "VComptonMacro.h"
/*
 *
 */
class BeamLine : public VComptonMacro {
public:
  void init( ComptonSession* );
  void run();
};

#endif
