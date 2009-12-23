#!/bin/sh

gcc -shared -Wl,-soname,libQwHit.so -o libQwHit.so $QWANALYSIS/Tracking/src/QwHit.o $QWANALYSIS/Tracking/dictionary/QwHitDict.o
gcc -shared -Wl,-soname,libQwHitContainer.so -o libQwHitContainer.so $QWANALYSIS/Tracking/src/QwHitContainer.o $QWANALYSIS/Tracking/dictionary/QwHitContainerDict.o
gcc -shared -Wl,-soname,libQwHitRootContainer.so -o libQwHitRootContainer.so $QWANALYSIS/Tracking/src/QwHitRootContainer.o $QWANALYSIS/Tracking/dictionary/QwHitRootContainerDict.o
#gcc -shared -Wl,-soname,libQwEvent.so -o libQwEvent.so $QWANALYSIS/Tracking/src/QwEvent.o $QWANALYSIS/Tracking/dictionary/QwEventDict.o
#gcc -shared -Wl,-soname,libQwEventHeader.so -o libQwEventHeader.so $QWANALYSIS/Tracking/src/QwEventHeader.o $QWANALYSIS/Tracking/dictionary/QwEventHeaderDict.o
#gcc -shared -Wl,-soname,libQwTrackingTreeLine.so -o libQwTrackingTreeLine.so $QWANALYSIS/Tracking/src/QwTrackingTreeLine.o $QWANALYSIS/Tracking/dictionary/QwTrackingTreeLineDict.o
#gcc -shared -Wl,-soname,libVQwTrackingElement.so -o libVQwTrackingElement.so $QWANALYSIS/Tracking/src/VQwTrackingElement.o $QWANALYSIS/Tracking/dictionary/VQwTrackingElementDict.o
#gcc -shared -Wl,-soname,libQwPartialTrack.so -o libQwPartialTrack.so $QWANALYSIS/Tracking/src/QwPartialTrack.o $QWANALYSIS/Tracking/dictionary/QwPartialTrackDict.o