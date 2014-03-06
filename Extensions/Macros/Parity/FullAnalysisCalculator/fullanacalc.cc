/*

03-04-2014
Author Buddhini Waidyawansa (buddhini2jlab.org)

Calculate the acceptance corrected tree level asymmetry for the full Qweak data set (run1+run2).

 */

#include <compute_LH2_asym.h>
#include <QwReadWrite.h>

using namespace std;

sdt::vector<Double_t> data_read;

int main(){

  // Initialize the data storage vector
  data_read.clear();

  // Read the data from text file
  QwReadWrite mapstr("test_input.txt");  //For now hard code input file
 
  while (mapstr.ReadNextDataLine(){
      //ignore comments
      TrimComment();
      if (LineIsEmpty()) continue;

      // Extract the asymmetry, polarization and background corrections
      data_read

    }



  return 0;
}
