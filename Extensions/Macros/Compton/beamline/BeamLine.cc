#include "BeamLine.h"

void BeamLine::init(ComptonSession *session)
{
  VComptonMacro::init(session);
}

void BeamLine::run()
{
}

// Define functions with c symbols (create/destroy instances)
extern "C" BeamLine* create()
{
  return new BeamLine();
}

extern "C" void destroy(BeamLine* beamline)
{
  delete beamline;
}
