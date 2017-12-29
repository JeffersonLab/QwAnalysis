#!/bin/bash

for i in ${1:-/cache/hallc/qweak/misc/wdconinc/rootfiles}/* ; do
	ls -al $i
	grep size ${i/cache/mss}
done
