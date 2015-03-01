#!/bin/bash
NEW_MONITORS=1

#monitor coefficients
nice root -q -b plotDetector.C+\(9990,19000,0,0,${NEW_MONITORS}\)
nice root -q -b plotDetector.C+\(9990,19000,1,0,${NEW_MONITORS}\)
nice root -q -b plotDetector.C+\(9990,19000,2,0,${NEW_MONITORS}\)

