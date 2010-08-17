#!/bin/sh                                                                                 

DATE=`date +%Y%m%d.%H%M`
filename=$HOSTNAME.$DATE

for a in {1..10}
do
  ${QWANALYSIS}/bin/qwparity -r 5260 > $filename.$a.log
done

exit 0;