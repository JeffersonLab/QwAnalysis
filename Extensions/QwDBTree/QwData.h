#ifndef QWDATA_H
#define QWDATA_H
#include <Riostream.h>

/* Class to store data for branching. */
class QwData {
    private:
        /* Data to go into the tree */
        Double_t value;
        Double_t error;
        Double_t rms;
        Long_t n;

    public:
        /* Overloaded constructer */
        QwData(void);
        QwData(Double_t, Double_t, Long_t, Double_t);
        void fill_empty(void);
        /* debug function to output value */
        Double_t return_val(void);
        Double_t return_n(void);
};

#endif
