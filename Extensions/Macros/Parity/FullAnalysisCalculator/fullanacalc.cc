/*

03-04-2014
Author Buddhini Waidyawansa (buddhini@jlab.org)

Calculate the acceptance corrected tree level asymmetry for the full Qweak data set (run1+run2).

 */

#include "QwReadWrite.cc"

using namespace std;

// Data storage vectors
std::vector<float> val_read;
std::vector<float> err_read;
QwReadFile input("test_input.txt");

void GetData();
void Print();

// Main function
int main()
{

  Bool_t debug = false;

  // Read the data from text file
  while (input.ReadNextDataLine()){
   
    // ignore comments
    input.TrimComment("#!");
    if (input.LineIsEmpty()) continue;

    // start filling the vectors
    GetData();
    if (debug) Print();
    

  }
  return 0;
}

  

// Get data from the input file.
void GetData(){
  
  Bool_t debug = true;
  float tmpval, tmperr;
 
  val_read.clear();
  err_read.clear();
 
  // Extract the asymmetry, polarization and background corrections
  //
  // regressed asymmetry and error
  input.GetTokenPair(",",&tmpval,&tmperr);
  val_read.push_back(tmpval);
  err_read.push_back(tmperr);
  
  // polarization and error
  input.GetTokenPair(",",&tmpval,&tmperr);
  val_read.push_back(tmpval);
  err_read.push_back(tmperr);
  
  // bkg1 
  input.GetTokenPair(",",&tmpval,&tmperr);
  val_read.push_back(tmpval);
  err_read.push_back(tmperr);
  
  // bkg2
  input.GetTokenPair(",",&tmpval,&tmperr);
  val_read.push_back(tmpval);
  err_read.push_back(tmperr);
  
  // R and dR
  input.GetTokenPair(",",&tmpval,&tmperr);
  val_read.push_back(tmpval);
  err_read.push_back(tmperr);

  
}

// print
void Print(){
  std::cout<<"\nReading next line:\n";
  std::cout<<" A_reg (ppm)        = "<<val_read.at(0)<<std::endl;
  std::cout<<" dA_reg (ppm)       = "<<err_read.at(0)<<std::endl;
  std::cout<<" P_beam             = "<<val_read.at(1)<<std::endl;
  std::cout<<" dP_beam            = "<<err_read.at(1)<<std::endl;
  std::cout<<" A_bkg1 (ppm)       = "<<val_read.at(2)<<std::endl;
  std::cout<<" dA_bkg1 err (ppm)  = "<<err_read.at(2)<<std::endl;
  std::cout<<" A_bkg2 (ppm)       = "<<val_read.at(3)<<std::endl;
  std::cout<<" dA_bkg2 (ppm)      = "<<err_read.at(3)<<std::endl;
  std::cout<<" R                  = "<<val_read.at(4)<<std::endl;
  std::cout<<" dR                 = "<<err_read.at(4)<<std::endl;
}

