#ifndef __MQWDATAELEMENTS__
#define __MQWDATAELEMENTS__

// System headers
#include <vector>

// Qweak headers
#include "QwLog.h"

// Forward declarations
class VQwDataElement;

class MQwDataElements {

  protected:
    /// Default constructor
    MQwDataElements() { }
    /// Copy constructor
    MQwDataElements(const MQwDataElements& value)
    : fDataElements(value.fDataElements) { }
    /// Virtual destructor
    virtual ~MQwDataElements() { }

    /// Assignment operator
    MQwDataElements& operator=(const MQwDataElements& value) {
      fDataElements = value.fDataElements;
      return *this;
    }

  protected:
    /// Data elements associated with this subsystem
    std::vector<VQwDataElement*> fDataElements;

  protected:
    /// Register a data element
    void AddDataElement(VQwDataElement* h) {
      fDataElements.push_back(h);
    }

}; // class MQwDataElements

#endif // __MQWDATAELEMENTS__

