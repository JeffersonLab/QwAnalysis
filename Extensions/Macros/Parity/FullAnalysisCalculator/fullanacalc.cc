/*

03-04-2014
Author Buddhini Waidyawansa (buddhini@jlab.org)

Calculate the acceptance corrected tree level asymmetry for the full Qweak data set (run1+run2).

 */

#include "QwCalculator.cc"
#include "ErlerErrProp.cc"

using namespace std;


// Main function
int main()
{
  Bool_t debug = true;

  // Read the data from text file
  QwReadFile input("test_input.txt");
  QwCalculator cal;

  // ********** commented out -- rupesh
  // std::vector<QwCalculator> calVec;
  // **********
  
  // Create the output text file to write the corrected asymmetries
  std::ofstream output("test_output.txt");

  float A       =0;
  float dA_stat =0;
  float dA_syst =0;

  // **********
  // 1D arary with contents stored sequentially as
  // asym_0,stat_0,syst_0, asym_1,stat_1,syst_1,...
  std::vector<Double_t> APVAll;
  // **********
  
  while (input.ReadNextDataLine()){

    // ignore comments
    input.TrimComment("#!");
    if (input.LineIsEmpty()) continue;
    
    std::cout<<"Getting new data list ..\n";

    // get time dependent corrections
    cal.SetTimeDependentQtys(input);
    // get time independend corrections
    cal.SetTimeIndependentQtys();

    // calculate the corrected unblinded physics asymmetry
    cal.ComputePhysAsym();

    // ********** commented out -- rupesh
    // // store cal into vector calVec
    // calVec.push_back(cal);
    // **********
    
    if (debug){
      cal.PrintInputs();
      cal.PrintErrorContributions();
      cal.PrintAcceltanceCorrectedAsym();
      cal.PrintCorrectionBreakdown();
    }

    // get the corrected asymmetry
    cal.GetCorrectedAsymmetry(&A,&dA_stat,&dA_syst);

    // **********
    APVAll.push_back(A);
    APVAll.push_back(dA_stat);
    APVAll.push_back(dA_syst);
    // **********
    
    if(debug) printf("A_signal(<Q^2>) =%6.4f +/- %6.4f (stat) +/- %6.4f (syst) ppm \n",A,dA_stat,dA_syst);

    // write the corrected asymmetry to an output file
    output<<A<<","<<dA_stat<<","<<dA_syst<<std::endl;

    cout << endl << "###### ****** DONE ****** DONE ****** ######" << endl << endl;
  }

  // ********** commented out -- rupesh 
  // for(std::vector<QwCalculator>::iterator it = calVec.begin(); it != calVec.end(); ++it){
  //   //    it->PrintInputs();
  // }
  // **********

  ErlerErrProp finalAsym(APVAll);
  std::vector<Double_t> asymFinal = finalAsym.EvalCumulativeAsym();

  // for(std::vector<Double_t>::iterator it=asymFinal.begin();it!=asymFinal.end();++it){
  //   cout << *it << endl;
  // }

  printf("Asym Final: %.4f +- %.4f +- %.4f \n",asymFinal[0],asymFinal[1],asymFinal[2]);
    
  return 0;
}

  
