#ifndef QwDataFile_h
#define QwDataFile_h

#include <fstream>
#include <ostream>
#include <iostream>
#include <cstdlib>
#include "TROOT.h"

class QwDataFile : private std::fstream{

private:

//   std::ifstream file;

  std::vector <Double_t > x;
  std::vector <Double_t > y;
  std::vector <Double_t > y_error;

public:

  Double_t fUpperX;
  Double_t fUpperY;
  Double_t fLowerX;
  Double_t fLowerY;
  Double_t fMeanY;

  QwDataFile(TString filename=0, ios_base::openmode mode = ios_base::in) : std::fstream(filename, mode)
  {
    if( (good()) ){
      std::cout << "Opening file: " << filename<< std::endl;
    }
    else{
      std::cerr << "Error opening file: " << std::endl;
      exit(1);
    }
  };

  ~QwDataFile(){close();}

  void ReadFile();
  void SetExtrema();
  void ScaleEntries(Double_t);

  Int_t GetArraySize() const;

  std::vector <Double_t> GetX() const;
  std::vector <Double_t> GetY() const;
  std::vector <Double_t> GetYError() const;

};

#endif
