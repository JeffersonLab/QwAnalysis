gcc -shared -Wl,-soname,libQwHit.so -o libQwHit.so $QWANALYSIS/Tracking/src/QwHit.o $QWANALYSIS/Tracking/dictionary/QwHitDict.o

gcc -shared -Wl,-soname,libQwHitContainer.so -o libQwHitContainer.so $QWANALYSIS/Tracking/src/QwHitContainer.o $QWANALYSIS/Tracking/dictionary/QwHitContainerDict.o
gcc -shared -Wl,-soname,libQwHitRootContainer.so -o libQwHitRootContainer.so $QWANALYSIS/Tracking/src/QwHitRootContainer.o $QWANALYSIS/Tracking/dictionary/QwHitRootContainerDict.o
