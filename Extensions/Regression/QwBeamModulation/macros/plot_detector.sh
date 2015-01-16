#!/bin/bash
stem=$1
#slopes
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,0,0,0\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,0,1,0\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,0,2,0\)

#sine coefficients
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,1,0,0\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,1,1,0\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,1,2,0\)

#cosine coefficients
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,1,0,1\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,1,1,1\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,1,2,1\)

#sine residuals
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,2,0,0\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,2,1,0\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,2,2,0\)

#cosine residuals
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,2,0,1\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,2,1,1\)
nice root -q -b plotDetector.C+\(\"${stem}\",9990,19000,2,2,1\)


