#ifndef QWOPTIONSEVENTDISPLAY3D_H
#define QWOPTIONSEVENTDISPLAY3D_H

#include "QwOptions.h"

void ProcessEventDisplay3DOptions(QwOptions *options)
{
   options->AddOptions()
      ("run,i", po::value<int>(), "Run number");
}

#endif //QWOPTIONSEVENTDISPLAY3D_H
