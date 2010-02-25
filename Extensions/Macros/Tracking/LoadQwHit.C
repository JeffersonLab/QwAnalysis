// Make sure that the $QWANALYSIS directory is added to the ROOT library search
// path in your ~/.rootrc file using the line:
// 	Unix.*.Root.DynamicPath:	.:$(QWANALYSIS)/lib:/usr/lib
// Otherwise you will have to give the full path to each of the libraries in
// this macro.

// To create the shared object libraries for the QwHit (and related) objects, run the following commands
// .!gcc -shared -Wl,-soname,libQwHit.so -o libQwHit.so $QWANALYSIS/Tracking/src/QwHit.o $QWANALYSIS/Tracking/dictionary/QwHitDict.o
// .!gcc -shared -Wl,-soname,libQwHitContainer.so -o libQwHitContainer.so $QWANALYSIS/Tracking/src/QwHitContainer.o $QWANALYSIS/Tracking/dictionary/QwHitContainerDict.o
// .!gcc -shared -Wl,-soname,libQwHitRootContainer.so -o libQwHitRootContainer.so $QWANALYSIS/Tracking/src/QwHitRootContainer.o $QWANALYSIS/Tracking/dictionary/QwHitRootContainerDict.o
//
// At this point this has to be done manually.  Maybe we'll make it easier by
// adding this to the Makefile.

{
  // Load Boost libraries
  gSystem->Load("libboost_filesystem");
  // Load basic Qweak analysis library
  gSystem->Load("libQw.so");

  // Load QwHit (and related) libraries
  gSystem->Load("libQwHit.so");
  gSystem->Load("libQwHitContainer.so");
  gSystem->Load("libQwHitRootContainer.so");
}
