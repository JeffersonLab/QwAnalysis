#include <TROOT.h>
#include <Riostream.h>
#include "QwData.h"

/*
 * QwData constructor is overloaded to allow creation without paramaters and
 * creation while filling.
 */
QwData::QwData(void) {}
/* Constructor for QwData. Fills all data for the object. */
QwData::QwData(Double_t temp_value, Double_t temp_error, Long_t temp_n, Double_t temp_rms) {
    value = temp_value;
    error = temp_error;
    n = temp_n;
    rms = temp_rms;
}

/*
 * Method to fill empty runlets. Mostly exists for compatibility with pass4b
 * and filling empty detectors.
 *
 * FIXME: empty detectors should not be filled.
 */ 

Double_t QwData::return_val(void) {
    return value;
}

Double_t QwData::return_n(void) {
    return n;
}

void QwData::fill_empty(void) {
    /*
     * -1e6 is used instead of zero so that it is distinguishable from small
     * asymmetries
     */
    value = -1e6;
    error = -1e6;
    rms = -1e6;
    n = 0;
}
