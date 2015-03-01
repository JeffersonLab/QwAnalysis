#define QwDataFile_cxx
#include "QwDataFile.hh"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#ifdef QwDataFile_cxx

void QwDataFile::ReadFile()
{

  std::string line;

  char *token;

  while(!eof()){
    std::getline(*this, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,\t");
    while(token){
      if(!token){
	x.push_back(0); 
	y.push_back(0); 
	y_error.push_back(0); 
	break;
      }
      x.push_back(atof(token));

      token = strtok(NULL, " ,\t");
      if(!token){
	y.push_back(0); 
	y_error.push_back(0);
	break;
      }
      y.push_back(atof(token));

      token = strtok(NULL, " ,\t");
      if(!token){
	y_error.push_back(0); 
	break;
      }
      y_error.push_back(atof(token));

      token = strtok(NULL, " ,\t");
    }
  }
  SetExtrema();

  return;
}

void QwDataFile::SetExtrema()
{
  fUpperX = x[0];
  fLowerX = x[0];
  fUpperY = y[0];
  fLowerY = y[0];

  for(Int_t i = 0; i < (Int_t)GetArraySize(); i++){
    if(x[i] > fUpperX) fUpperX = x[i];
    if(x[i] < fLowerX) fLowerX = x[i];
    if(y[i] > fUpperY) fUpperY = y[i];
    if(y[i] < fLowerY) fLowerY = y[i];

    fMeanY += fMeanY;
  }

  fMeanY /= GetArraySize();
  return;
}

std::vector <Double_t> QwDataFile::GetX() const
{
  return x;
}
std::vector <Double_t> QwDataFile::GetY() const
{
  return y;
}
std::vector <Double_t> QwDataFile::GetYError() const
{
  return y_error;
}

Int_t QwDataFile::GetArraySize() const
{
  
  if( ( x.size() != y.size() ) || (x.size() != y_error.size()) ){
    std::cerr << "Array sizes do not match!" << std::endl;
    exit(1);
  }
  else
    return(x.size());
  
}

void QwDataFile::ScaleEntries(Double_t factor = 1.0)
{
  for(Int_t i = 0; i < (Int_t)GetArraySize(); i++){
    y[i] *= factor;
    y_error[i] *= factor;
  }
  return;
}

#endif
