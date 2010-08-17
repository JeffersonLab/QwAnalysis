#!/bin/sh                                                                                 

DATE=`date +%Y%m%d.%H`
filename=$HOSTNAME.$DATE

for a in {1..20}
do
  time ../bin/qwparity -r 5260 > $filename.$a.log
done

exit 0;