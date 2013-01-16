#ifndef VCOMPTONMACRO_H
#define VCOMPTONMACRO_H

#include "ComptonSession.h"
/*
 *
 */
class VComptonMacro {
public:
  virtual void init( ComptonSession *session ) {
    fSession = session;
  }
  virtual void run() = 0;
private:
  ComptonSession *fSession;
};

#endif
