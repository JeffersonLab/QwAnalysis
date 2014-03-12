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

  // Create the output text file to write the corrected asymmetries
  std::ofstream output("test_output.txt");

  float A       =0;
  float dA_stat =0;
  float dA_sys  =0;

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

    // get the corrected asymmetry and write it to an output file
    cal.GetCorrectedAsymmetry(&A,&dA_stat,&dA_sys);
    if(debug) printf("A_signal(<Q^2>) =%6.4f +/- %6.4f (stat) +/- %6.4f (syst) ppm \n",A,dA_stat,dA_sys);
    output<<A<<","<<dA_stat<<","<<dA_sys<<std::endl;

  }
  return 0;
}

  
