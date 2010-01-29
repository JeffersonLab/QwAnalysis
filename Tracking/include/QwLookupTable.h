/*! \file   QwLookupTable.h
 *  \brief  Definition of the forced bridging method
 *
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \date   2010-01-23
 */

#ifndef QWLOOKUPTABLE_H
#define QWLOOKUPTABLE_H

// Qweak headers
#include "VQwBridgingMethod.h"

class QwLookupTable: public VQwBridgingMethod {

  public:

    /// \brief Default constructor
    QwLookupTable();
    /// \brief Destructor
    virtual ~QwLookupTable();

  private:

}; // class QwLookupTable

#endif // QWLOOKUPTABLE_H
