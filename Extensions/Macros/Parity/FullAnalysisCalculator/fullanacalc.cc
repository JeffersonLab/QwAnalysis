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

  std::vector< std::vector<Double_t> > StatErrors;
  
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
    StatErrors.push_back(cal.GetSystErrorList());
    
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
  size_t n_errors = StatErrors.at(0).size();
  Double_t c_total = 0.0;
  //  Double_t wt_factor = 3.28847; // based on stat error ratio
  Double_t wt_factor = 5.46181; //  Based on dA_bias = 2.0 and March24 inputs & formula
  //Double_t wt_factor = 5.60382; // based on dA_bias = 2.0
  //wt_factor = 5.62894; // based on dA_bias = 3.5
  //wt_factor = 5.65916; //based on dA_bias = 4.7
  //wt_factor = 5.09848; // based on dA_bias = 2.0 & Peng's new values
  //wt_factor = 5.11952; // based on dA_bias = 3.5 & Peng's new values
  //  wt_factor = 5.14482; //based on dA_bias = 4.7 & Peng's new values
  Double_t wt_error_term;
  Double_t err_sqr = 0.0;
  Double_t total2_1, total2_2;
  total2_1 =  TMath::Power(APVAll.at(1),2);
  total2_2 =  TMath::Power(APVAll.at(4),2);
  cout << "Error contributions terms"  << std::endl;
  for (size_t i=0; i<n_errors; i++){
    cout << StatErrors.at(0).at(i) << "  " << StatErrors.at(1).at(i) << "  "
	 << StatErrors.at(0).at(i)*StatErrors.at(1).at(i) << std::endl;
    c_total += StatErrors.at(0).at(i)*StatErrors.at(1).at(i);

    total2_1 += TMath::Power(StatErrors.at(0).at(i),2);
    total2_2 += TMath::Power(StatErrors.at(1).at(i),2);

    wt_error_term = StatErrors.at(0).at(i) + wt_factor*StatErrors.at(1).at(i);
    wt_error_term /= (1.0 + wt_factor);

    err_sqr += (wt_error_term*wt_error_term);
  }
  std::cout << "recalculated weight: " << (total2_1 - c_total)/(total2_2 - c_total)
	    << std::endl;

  cout << "c_total = " << c_total << endl;

  cout << "avg using weight="<<wt_factor << ": " 
       << (APVAll.at(0) + wt_factor*APVAll.at(3))/(1+wt_factor) << std::endl;
  cout << "combined syst error using weight="<<wt_factor << ": " << sqrt(err_sqr) << std::endl;
  cout << "stat error using weight="<<wt_factor << ": " 
       << sqrt( TMath::Power(APVAll.at(1),2) + TMath::Power(wt_factor,2)*TMath::Power(APVAll.at(4),2))/(1+wt_factor) << std::endl;

  cout << "total error using weight="<<wt_factor << ": " 
       << sqrt((( TMath::Power(APVAll.at(1),2) + TMath::Power(wt_factor,2)*TMath::Power(APVAll.at(4),2))/TMath::Power(1+wt_factor,2)) + err_sqr)  << std::endl;






  ErlerErrProp finalAsym(APVAll);
  std::vector<Double_t> asymFinal = finalAsym.EvalCumulativeAsym();

  // for(std::vector<Double_t>::iterator it=asymFinal.begin();it!=asymFinal.end();++it){
  //   cout << *it << endl;
  // }

  printf("Asym Final: %.4f +- %.4f +- %.4f \n",asymFinal[0],asymFinal[1],asymFinal[2]);
    
  return 0;
}

  
