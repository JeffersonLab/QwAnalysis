#ifndef __MQWHISTOGRAMS__
#define __MQWHISTOGRAMS__

// System headers
#include <vector>

// Root headers
#include "TH1.h"

// Qweak headers
#include "QwLog.h"

class MQwHistograms {

    /// Regular pointers for the histograms
    typedef TH1* TH1_ptr;
    // Shared pointers (boost::shared_ptr) are not advisable
    // because ROOT keep ownership of all histograms.  They
    // are automatically deleted when ROOT closes the file.
    // If we put them in a shared_ptr here, they would be
    // deleted by the time the shared_ptr goes out of scope.

  protected:
    /// Default constructor
    MQwHistograms() { }
    /// Copy constructor
    MQwHistograms(const MQwHistograms& value)
    : fHistograms(value.fHistograms) { }
    /// Virtual destructor
    virtual ~MQwHistograms() { }

    /// Copy operator:  Should make a full, identical copy.
    /// In this particular class, the pointers should be copied,
    /// not the objects pointed-to.
    virtual MQwHistograms& Copy(const MQwHistograms& value) {
      fHistograms = value.fHistograms;
      return *this;
    }

    /// Arithmetic assignment operator:  Should only copy event-based data.
    /// In this particular class, there is no event-based data.
    virtual MQwHistograms& operator=(const MQwHistograms& value) {
      return *this;
    }

  protected:
    /// Histograms associated with this data element
    std::vector<TH1_ptr> fHistograms;

  protected:
    /// Register a histogram
    void AddHistogram(TH1* h) {
      fHistograms.push_back(TH1_ptr(h));
    }

}; // class MQwHistograms

#endif // __MQWHISTOGRAMS__
