/*

03-04-2014
Author Buddhini Waidyawansa (buddhini@jlab.org)

Calculate the acceptance corrected tree level asymmetry for the full Qweak data set (run1+run2).

 */

#include "QwCalculator.cc"

using namespace std;


// Main function
int main()
{
  Bool_t debug = false;

  // Read the data from text file
  QwReadFile input("test_input.txt");
  QwCalculator cal;
  while (input.ReadNextDataLine()){

    // ignore comments
    input.TrimComment("#!");
    if (input.LineIsEmpty()) continue;
    
    std::cout<<"Getting data ..\n";

    // get time dependent corrections
    cal.SetTimeDependentQtys(input);
    // get time independend corrections
    cal.SetTimeIndependentQtys();

    // calculate the corrected unblinded physics asymmetry
    cal.ComputePhysAsym();
    
    if (debug){
      cal.PrintInputs();
      cal.PrintErrorContributions();
      cal.PrintAcceltanceCorrectedAsym();
      cal.PrintCorrectionBreakdown();
    }
  }
  return 0;
}

  
