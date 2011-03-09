/*!
 * \file   VQwTrackingElement.cc
 * \brief  Implementation of virtual base class for all tracking elements
 *
 * \author Wouter Deconinck
 * \date   2009-12-12
 */

#include "VQwTrackingElement.h"
ClassImp(VQwTrackingElement)

#ifdef STATIC_TCLONESARRAY
  template <class T>
  TClonesArray* VQwTrackingElementList<T>::gList = 0;
#endif
