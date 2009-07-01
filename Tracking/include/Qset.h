#ifndef QSET_H
#define QSET_H

/*------------------------------------------------------------------------*//*!

 \brief A generic data read-in class that reads the detector geometry.

    This class is not meant to be permanent.  I fully
    expect to replace the method by which event data
    is imported into this program.

*//*-------------------------------------------------------------------------*/

///
/// \ingroup QwTrackingAnl
class Qset {

  public:

    Qset();

    int FillDetec (const char *geomfile);

    int numdetectors;	// detector counter

  private:

    int debug;		// debug level

    void DumpDetector (int i);
    void LinkDetector ();
    void DeterminePlanes ();

};


#endif // QSET_H
