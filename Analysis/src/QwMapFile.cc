#include "QwMapFile.h"
#include <iostream>
#include <fstream>

ClassImp(QwMapFile)

QwMapFile::QwMapFile() {

  fMapFile = NULL;
}


QwMapFile::QwMapFile(const TString name, const TString  title, Option_t *option) {
  // This constructor tries to keep track of the best suited address in
  // memory for the mapfile
  std::cout<<"Entering QwMapFile constructor\n";
  /*
  TString theMemMapFile;
  //std::cout<<" ROOT map file name "<<name<<std::endl;
  theMemMapFile = getenv("QW_ROOTFILES");
  theMemMapFile += "/dummy.map";
  
  TMapFile *mapfile = TMapFile::Create(theMemMapFile,"RECREATE",kMapFileSize,"Dummy File");
  UInt_t add,iadd, oadd = Int_t(mapfile->GetBaseAddr());
  std::cout<<"QwMapFile:: dummy file is created\n";
  mapfile->Print();
  mapfile->Close(); // Don't delete afterwards

  TString theQwSetupDir = getenv("QW_ROOTFILES");
  TString theMapFileAddressFileName = "/.MapFileBaseAddress";
  theMapFileAddressFileName = theQwSetupDir + theMapFileAddressFileName;
  std::cout<<"File name"<<theMapFileAddressFileName<<std::endl;
  ifstream *ifile = new ifstream(theMapFileAddressFileName);
  *ifile >> iadd;
  add = iadd;
  std::cout<<" dummy file add = "<<oadd<<" default address add = "<<iadd<<std::endl;
  if(iadd<oadd){
    std::cout<<" iadd = "<<iadd<<" oadd = "<<oadd<<std::endl;
    add = oadd;
  }
  ifile->close();
  delete ifile;
  if(add!=iadd) {
    std::cerr << "QwMapFile::QwMapFile: Resetting the master map file address" << " from 0x" << iadd << " to 0x"   << oadd << "." << std::endl;
    ofstream *ofile = new ofstream(theMapFileAddressFileName);
    (*ofile) << add << std::endl;
    ofile->close();
    delete ofile;
    std::cerr << "Map file address in memory is not shared by all processes;" << std::endl;
    std::cerr << "*** KILL ALL ***" << std::endl;
    std::cerr << "Simply restart executables and error shouldn't reproduce" << std::endl;
    system("killall -w QwRealTimeGUI");
    exit(1);

  }

  
  std::cout<<"QwMapFile:: dummy file is closed \n";
  TMapFile::SetMapAddress(add);
  */
  fMapFile = TMapFile::Create(name,"RECREATE",kMapFileSize,title);
  
  std::cout<<"End of QwMapFile constructor\n";
  //exit(1);
}


QwMapFile::~QwMapFile() {

  if(fMapFile) fMapFile->Close("dtor");
  fMapFile = NULL;
}

void QwMapFile::Close() {

  if(fMapFile) fMapFile->Close();
  fMapFile = NULL;

}
