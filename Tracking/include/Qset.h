#ifndef QSET_H
#define QSET_H

/*------------------------------------------------------------------------*//*!

 \brief A generic data read-in class that reads the detector geometry.

    This class is not meant to be permanent.  I fully
    expect to replace the method by which event data
    is imported into this program.

*//*-------------------------------------------------------------------------*/

///
/// \ingroup QwTracking
class Qset {

  public:

    Qset();

    int FillDetectors (const char *geomfile);
    void LinkDetectors ();
    void DeterminePlanes ();

    void SetNumDetectors (int n) { numdetectors = n; };
    int  GetNumDetectors () { return numdetectors; };

  private:

    int debug;		// debug level
    int numdetectors;	// number of detectors

    void DumpDetector (int i);

};


#endif // QSET_H
