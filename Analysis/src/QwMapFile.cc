#include "QwMapFile.h"
#include <iostream>
#include <fstream>

ClassImp(QwMapFile)

const size_t QwMapFile::kMapFileSize = 1000000000;

QwMapFile::QwMapFile() {

  fMapFile = NULL;
}


QwMapFile::QwMapFile(const TString name, const TString  title, Option_t *option) {
  // This constructor tries to keep track of the best suited address in
  // memory for the mapfile
  std::cout<<"Entering QwMapFile constructor\n";

  TString theMemMapFile;
  //std::cout<<" ROOT map file name "<<name<<std::endl;
  theMemMapFile = getenv("QW_ROOTFILES")? getenv("QW_ROOTFILES") : "."; // TODO should warn
  theMemMapFile += "/dummy.map";

  TMapFile *mapfile = TMapFile::Create(theMemMapFile,"RECREATE",kMapFileSize,"Dummy File");
  ptrdiff_t add,iadd, oadd = size_t(mapfile->GetBaseAddr());
  std::cout<<"QwMapFile:: dummy file is created\n";
  mapfile->Print();
  mapfile->Close(); // Don't delete afterwards

  TString theQwSetupDir = getenv("QW_ROOTFILES")? getenv("QW_ROOTFILES") : "."; // TODO should warn
  TString theMapFileAddressFileName = "/.MapFileBaseAddress";
  theMapFileAddressFileName = theQwSetupDir + theMapFileAddressFileName;
  std::cout<<"File name"<<theMapFileAddressFileName<<std::endl;
  ifstream *ifile = new ifstream(theMapFileAddressFileName);

  if (!(ifile->good())){//if the .MapFileBaseAddress file does not exists create it.
    ofstream *ofile = new ofstream(theMapFileAddressFileName);
    (*ofile) << oadd << std::endl;
    ofile->close();
    delete ofile;
    iadd=0;
  }
  else{
    *ifile >> iadd;//read the map file address from the consumer
    ifile->close();
    delete ifile;
  }
  std::cout<<" dummy file add = "<<std::hex<<oadd<<" default address add = "<<iadd<<std::dec<<std::endl;
  //std::cout<<" default address iadd = "<<std::hex<<iadd<<std::dec<<std::endl;

  add=iadd;

  if (iadd>oadd)
    add=oadd;

  if (add != iadd){//if address is different  from the old one write it to the address file
    add=oadd;
    ofstream *ofile = new ofstream(theMapFileAddressFileName);
    //(*ofile) <<std::hex<< oadd <<std::dec<< std::endl;
    (*ofile) << oadd << std::endl;
    ofile->close();
    delete ofile;
    std::cout<<" default address updated  "<<std::hex<<iadd<<std::dec<<std::endl;
    std::cerr << "Map file address in memory is not shared by all processes;" << std::endl;
    std::cerr << "*** KILL ALL ***" << std::endl;
    std::cerr << "Simply restart executables and error shouldn't reproduce" << std::endl;
    system("killall -w QwRealTimeGUI");

    exit(1);

  }

  std::cout<<"Setting Address \n";
  TMapFile::SetMapAddress(add);

  fMapFile = TMapFile::Create(name,"RECREATE",kMapFileSize,title);
  std::cout<<"End of QwMapFile constructor\n";
}


QwMapFile::~QwMapFile() {

  if(fMapFile)
    fMapFile->Close("dtor");
  //delete fMapFile;
  fMapFile = NULL;
}

void QwMapFile::Close() {

  if(fMapFile)
    fMapFile->Close();
  //delete fMapFile;
  fMapFile = NULL;

}

void QwMapFile::RemoveAll(){
  if(fMapFile)
    fMapFile->RemoveAll();
};
