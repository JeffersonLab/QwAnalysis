/**
 * \class	MQwSIS3320_Module	MQwSIS3320_Module.h
 *
 * \brief	Combination of 8 channels of SIS3320 sampling ADC data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320_Module class is defined as 8 MQwSIS3320_Channels.  This
 * class helps keep track of which channels are combined together in one
 * module and therefore read out in the same subbank.
 *
 */

#ifndef __MQwSIS3320_Module__
#define __MQwSIS3320_Module__

/** \def SIS3320_CHANNELS Number of channels in a SIS3320 module */
#define SIS3320_CHANNELS 8

// System headers
#include <iostream>
#include <vector>
#include <numeric>

// ROOT headers
#include <TTree.h>

// Boost math library for random number generation
#include <boost/random.hpp>

// Qweak headers
#include "VQwDataElement.h"
#include "MQwSIS3320_Channel.h"

class MQwSIS3320_Module: std::vector<MQwSIS3320_Channel> {

  public:

    MQwSIS3320_Module() { resize(SIS3320_CHANNELS); }; // create the channels

    /**
     * Print the value of the MQwSIS3320_Channels
     */
    void PrintValue() {
      for (std::vector<MQwSIS3320_Channel>::iterator c = begin(); c != end(); c++)
        c->PrintValue();
    };

    /**
     * Print some debugging information about the MQwSIS3320_Channels
     */
    void PrintInfo() {
      for (std::vector<MQwSIS3320_Channel>::iterator c = begin(); c != end(); c++)
        c->PrintInfo();
    };

  protected:

  private:

};

#endif // __MQwSIS3320_Module__