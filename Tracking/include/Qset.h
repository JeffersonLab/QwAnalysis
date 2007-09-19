#ifndef QSET_H
#define QSET_H

class Qset{
public:
Qset();
int FillDetec(char *geomfile);
int numdetectors;

private:
void LinkDetector(void);

};


#endif
