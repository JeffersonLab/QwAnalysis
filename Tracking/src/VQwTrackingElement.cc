/*!
 * \file   VQwTrackingElement.cc
 * \brief  Implementation of virtual base class for all tracking elements
 *
 * \author Wouter Deconinck
 * \date   2009-12-12
 */

#include "VQwTrackingElement.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(VQwTrackingElement)
#endif

#ifdef STATIC_TCLONESARRAY
  template <class T>
  TClonesArray* VQwTrackingElementList<T>::gList = 0;
#endif
