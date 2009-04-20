//
// C++ Interface: enum
//
// Description: Header file that defines the enumeration data types
//              for upper/lower, detector region, detector type and
//              wire orientation.
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGENUM_H
#define QWTRACKINGENUM_H

/*! \enum EPackage
    Upper or lower part of the detector
 */
enum EPackage {
  w_nowhere = -1,	/*!< Undefined part of the detector */
  w_upper,		/*!< Upper detector part */
  w_lower		/*!< Lower detector part */
};

/*! \enum Etype
    Detector type
 */
enum Etype {
  d_drift,	/*!< Drift chamber */
  d_gem,	/*!< GEM detector */
  d_trigscint,	/*!< Trigger scintillator */
  d_cerenkov	/*!< Cerenkov detector */
};

#endif // QWTRACKINGENUM_H
