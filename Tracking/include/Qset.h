#ifndef QSET_H
#define QSET_H


/*! \brief Qset is a generic data read-in class that
reads ascii to set up the detector geometry.  

This class is not meant to be permanent.  I fully
expect to replace the method by which event data
is imported into this program.
*/
class Qset{
public:
Qset();
int FillDetec(char *geomfile);
int numdetectors;

private:
void LinkDetector(void);

};


#endif
