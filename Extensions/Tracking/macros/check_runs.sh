#/bin/bash

for i in /group/qweak/www/html/tracking/pass6/run_* ; do
	echo "$i: `ls -al $i/* | wc -l`"
done
