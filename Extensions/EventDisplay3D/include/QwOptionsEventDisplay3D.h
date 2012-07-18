#ifndef QWOPTIONSEVENTDISPLAY3D_H
#define QWOPTIONSEVENTDISPLAY3D_H

#include "QwOptions.h"

void DefineOptionsEventDisplay3D(QwOptions &options)
{
  /* Define general options */
  QwOptions::DefineOptions(options);

  /* Define event display options */
  options.AddOptions()
    ("disable-drawTracks,b", po::value<bool>()->zero_tokens(),
     "Disable drawing tracks");
  options.AddOptions()
    ("disable-drawTreeLines,b", po::value<bool>()->zero_tokens(),
     "Disable drawing treelines");
  options.AddOptions()
    ("showAllRegion3,b", po::value<bool>()->zero_tokens(),
     "Show all Region 3 Wires");
  options.AddOptions()
    ("showAllRegion2,b", po::value<bool>()->zero_tokens(),
     "Show all Region 2 Wires");
}

#endif //QWOPTIONSEVENTDISPLAY3D_H
