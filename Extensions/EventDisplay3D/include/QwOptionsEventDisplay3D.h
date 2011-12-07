#ifndef QWOPTIONSEVENTDISPLAY3D_H
#define QWOPTIONSEVENTDISPLAY3D_H

#include "QwOptions.h"

void ProcessEventDisplay3DOptions(QwOptions *options)
{
  options->AddOptions()
    ("run,i", po::value<int>(), "Run number");
  options->AddOptions()
    ("rotate,i", po::value<int>(), "Package Rotation");
  options->AddOptions()
    ("disable-drawTracks,b", po::value<bool>()->zero_tokens(),
     "Disable drawing tracks");
  options->AddOptions()
    ("disable-drawTreeLines,b", po::value<bool>()->zero_tokens(),
     "Disable drawing treelines");
  options->AddOptions()
    ("showAllRegion3,b", po::value<bool>()->zero_tokens(),
     "Show all Region 3 Wires");
  options->AddOptions()
    ("showAllRegion2,b", po::value<bool>()->zero_tokens(),
     "Show all Region 2 Wires");
}

#endif //QWOPTIONSEVENTDISPLAY3D_H
