#!/bin/bash

for i in /mss/home/$USER/rootfiles/myTrackingRun_* /volatile/hallc/qweak/$USER/rootfiles/myTrackingRun_* ; do
	if [ ! -f "/mss/hallc/qweak/misc/$USER/rootfiles/`basename $i`_tracking.root" ] ; then
		echo "# $i:"
		echo -n "# "
		if [ ! "${i/mss/}" = "${i}" ] ; then
			n=`ls $i | wc -l`
			m=`ls ${i/mss/cache} | wc -l`
			if [ $n -ne $m ] ; then
				echo "($m of $n files cached out)"
				echo "./jcache.sh `basename $i`"
			else
				echo "($m files cached out)"
			fi
		else
			n=`ls $i | wc -l`
			echo "($n files)"
		fi
		if [ ! -f "/cache/hallc/qweak/misc/$USER/rootfiles/`basename $i`_tracking.root" ] ; then
			echo "./hadd.sh `dirname $i` `basename $i`"
		else
			echo "jcache put /cache/hallc/qweak/misc/$USER/rootfiles/`basename $i`_*.root"
		fi
	fi
done
